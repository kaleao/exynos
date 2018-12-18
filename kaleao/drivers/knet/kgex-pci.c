#include "kgex.h"
#include "kgex-hw.h"
#include "kgex-pci.h"
#include <linux/pci.h>

u32 npu_pci_config[] = {
	0x11111556, 0x100406, 0x7800002, 0x20,       0x1C10000C, 0x0,  0x0, 0x0,
	0x0,        0x0,      0x0,       0x11111556, 0x0,        0x80, 0x0, 0x100};

/* Internal restore functions */
static struct pci_cap_saved_state *
kgex_pci_find_saved_cap(struct pci_dev *pci_dev, u16 cap);
static void kgex_pci_restore_config_dword(struct pci_dev *pdev, int offset,
		u32 saved_val, int retry);
static void kgex_pci_restore_config_space_range(struct pci_dev *pdev, int start,
		int end, int retry);
static void kgex_pci_restore_config_space(struct pci_dev *pdev);
static void kgex_pci_restore_pcie_state(struct pci_dev *dev);

void kgex_pci_restore_msi(struct net_device *ndev) {
	uint64_t msi_target;
	struct net_private *lp = NULL;

	lp = netdev_priv(ndev);
	msi_target =
		virt_to_phys((void *)exynos_pcie_get_msi_addr(lp->pdev->bus->self));

	gen_io_wr32(lp->regs, 0x3000, (u32)(msi_target & 0xffffffff));
	gen_io_wr32(lp->regs, 0x3004, (u32)((msi_target >> 32) & 0xffffffff));
	gen_io_wr32(lp->regs, 0x3008, 0x1);
	gen_io_wr32(lp->regs, 0x300C, 0x0);

	gen_io_wr32(lp->regs, 0x3010, (u32)(msi_target & 0xffffffff));
	gen_io_wr32(lp->regs, 0x3014, (u32)((msi_target >> 32) & 0xffffffff));
	gen_io_wr32(lp->regs, 0x3018, 0x2);
	gen_io_wr32(lp->regs, 0x301C, 0x0);
}

int kgex_pci_get_gen(struct net_device *ndev) {
	struct net_private *lp = NULL;
	uint32_t val = 0;

	if (!ndev) {
		prierr("ndev==NULL\n");
		return -1;
	}
	lp = netdev_priv(ndev);
	if (!lp) {
		prierr("lp==NULL\n");
		return -2;
	}

	pci_read_config_dword(lp->pdev->bus->self, PCIE_RC_LINK_STATUS, &val);

	if (val == 0xFFFFFFFF)
		return 0;

	if (val & PCIE_LINK_STATUS_GEN1_MASK)
		return 1;
	else if (val & PCIE_LINK_STATUS_GEN2_MASK)
		return 2;

	return 0;
}

int kgex_pci_set_target_gen(struct net_device *ndev, int gen) {
	struct net_private *lp = NULL;
	uint32_t val = 0;

	if (gen < 1 || gen > 2) {
		prwarn("requested target PCIe Gen=%d is invalid.\n", gen);
		return -1;
	}

	if (!ndev) {
		prierr("ndev==NULL\n");
		return -1;
	}
	lp = netdev_priv(ndev);
	if (!lp) {
		prierr("lp==NULL\n");
		return -2;
	}

	val = gen_io_rd32_prot(lp, lp->pci_rc_regs, PCIE_RC_LINK_CTRL);
	val = val & (~PCIE_LINK_CTRL_GEN_CLEAR_MASK);
	if (gen == 1) {
		val = val | PCIE_LINK_CTRL_GEN1_MASK;
	} else if (gen == 2) {
		val = val | PCIE_LINK_CTRL_GEN2_MASK;
	} else {
		prierr("invalid PCIe target gen = %d\n", gen);
		return -3;
	}

	gen_io_wr32_prot(lp, lp->pci_rc_regs, PCIE_RC_LINK_CTRL, val);

	return 0;
}

int kgex_pci_retrain(struct net_device *ndev) {
	struct net_private *lp = NULL;
	uint32_t val = 0;

	if (!ndev) {
		prierr("ndev==NULL\n");
		return -1;
	}
	lp = netdev_priv(ndev);
	if (!lp) {
		prierr("lp==NULL\n");
		return -2;
	}
	val = gen_io_rd32_prot(lp, lp->pci_rc_regs, PCIE_RC_LINK_RETRAIN);
	val = val | PCIE_LINK_RETRAIN_FORCE_TRAIN_MASK;
	gen_io_wr32_prot(lp, lp->pci_rc_regs, PCIE_RC_LINK_RETRAIN, val);

	return 0;
}

int kgex_pci_wait_gen2(struct net_device *ndev) {
	struct net_private *lp = NULL;
	uint32_t retries = 0;

	if (!ndev) {
		prierr("ndev==NULL\n");
		return -1;
	}
	lp = netdev_priv(ndev);
	if (!lp) {
		prierr("lp==NULL\n");
		return -2;
	}
	if (lp->pdev != NULL && !exynos_pcie_is_linkup(lp->pdev->bus->self)) {
		return -1;
	}
	while (kgex_pci_get_gen(ndev) != 2 && retries < PCIE_RETRAIN_MAX_RETRIES) {
		udelay(10);
		retries++;
	}

	if (kgex_pci_get_gen(ndev) != 2)
		return 1;

	return 0;
}

int kgex_pci_endpoint_online(struct net_device *ndev) {
	struct net_private *lp = NULL;
	u16 val = 0;
	int pos = 0;

	lp = netdev_priv(ndev);

	pos = pci_pcie_cap(lp->pdev);
	if (!pos)
		return 0;

	pci_read_config_word(lp->pdev, pos + PCI_EXP_DEVCTL, &val);

	if (val == 0xFFFF)
		return 0;

	return 1;
}

void kgex_pci_restore_state(struct net_device *ndev) {
	struct net_private *lp = NULL;

	lp = netdev_priv(ndev);

	kgex_pci_restore_pcie_state(lp->pdev);

	kgex_pci_restore_config_space(lp->pdev);
}

int kgex_pci_probe(struct net_device *ndev) {
	struct net_private *lp = NULL;
	int status = 0;
	int bars = 0;
	uint32_t i = 0;

	lp = netdev_priv(ndev);

	status = pci_enable_device(lp->pdev);
	if (status) {
		netdev_err(ndev, "cannot enable PCI device. Exiting.\n");
		return status;
	}

	bars = pci_select_bars(lp->pdev, IORESOURCE_MEM);
	status = pci_enable_device_mem(lp->pdev);
	if (status)
		goto err_disable_dev;

	// Check if PCI express capability is present
	if (!pci_is_pcie(lp->pdev)) {
		netdev_warn(ndev, "not PCIe!\n");
	}

	status = pci_request_selected_regions(lp->pdev, bars, DRIVER_NAME);
	if (status) {
		netdev_err(ndev, "cannot request regions\n");
		goto err_disable_dev;
	}

	// Set the bus pci master
	pci_set_master(lp->pdev);

	// Enable Mem-Wr-Invalidate functionality
	if (pci_set_mwi(lp->pdev) < 0) {
		netdev_warn(ndev, "Mem-Wr-Inval unavailable\n");
	}

	lp->regs = pci_ioremap_bar(lp->pdev, 0);
	if (lp->regs == NULL) {
		status = -ENOMEM;
		netdev_err(ndev, "Cannot ioremap BAR\n");
		goto err_release_regions;
	}

	lp->msix_entries =
		kzalloc(MSIX_VECTORS * sizeof(struct msix_entry), GFP_KERNEL);

	if (lp->msix_entries == NULL) {
		netdev_err(ndev, "Cannot Allocate MSI-X Entries structure!\n");
		status = -ENOMEM;
		goto err_iounmap_bars;
	}

	for (i = 0; i < MSIX_VECTORS; i++) {
		(lp->msix_entries)[i].entry = i;
	}

	status = pci_enable_msix(lp->pdev, lp->msix_entries, MSIX_VECTORS);
	if (status) {
		netdev_err(ndev, "Could not allocate msi-x interrupt vectors!\n");
		goto err_free_msix_entries;
	}

	if ((lp->msix_entries[0].vector == 0) || (lp->msix_entries[1].vector == 0)) {
		status = -ENOMEM;
		netdev_err(ndev, "MSI-X vector is incorrect!\n");
		goto err_disable_msix;
	}

	lp->tx_irq = lp->msix_entries[0].vector;
	lp->rx_irq = lp->msix_entries[1].vector;

	lp->pci_bars = bars;

	// TODO: Change the way we identify each RC (in device tree)
	if (strcmp(lp->pdev->bus->name, "PCI Bus 0001:01") == 0) {
		lp->id = KGEX_NIC_ID_NPU;
		// TODO: Remove pci_rc_regs and for everything depending on that use
		// pci_read...
		lp->pci_rc_regs = ioremap(PCI_RC_NPU_BASE, 0x1000);
		if (!lp->pci_rc_regs) {
			netdev_err(ndev, "Cannot ioremap PCI Root Complex space for NPU\n");
			goto err_disable_msix;
		}
	} else if (strcmp(lp->pdev->bus->name, "PCI Bus 0000:01") == 0) {
		lp->id = KGEX_NIC_ID_SPU;
		// TODO: Remove pci_rc_regs and for everything depending on that use
		// pci_read...
		lp->pci_rc_regs = ioremap(PCI_RC_SPU_BASE, 0x1000);
		if (!lp->pci_rc_regs) {
			netdev_err(ndev, "Cannot ioremap PCI Root Complex space for SPU\n");
			goto err_disable_msix;
		}
	} else {
		netdev_err(ndev, "Unknown PCI bus name\n");
		goto err_disable_msix;
	}

	pci_save_state(lp->pdev);
	lp->pdev_state = pci_store_saved_state(lp->pdev);

	if (lp->id == KGEX_NIC_ID_NPU) {
		int cnt;

		for (cnt = 0; cnt < 16; cnt++) {
			if (((uint32_t *)(lp->pdev_state))[cnt] == 0xFFFFFFFF) {
				//			pr("Inconsistent State!!!!\n");
			}
			((uint32_t *)(lp->pdev_state))[cnt] = npu_pci_config[cnt];
		}
	}
	return 0;

err_disable_msix:
	pci_disable_msix(lp->pdev);
err_free_msix_entries:
	kfree(lp->msix_entries);
err_iounmap_bars:
	// TODO: iounmap BAR
err_release_regions:
	pci_release_selected_regions(lp->pdev, bars);
err_disable_dev:
	pci_disable_device(lp->pdev);

	return status;
}

void kgex_pci_remove(struct net_device *ndev) {
	struct net_private *lp = NULL;

	lp = netdev_priv(ndev);

	pci_disable_msix(lp->pdev);
	kfree(lp->msix_entries);

	pci_release_selected_regions(lp->pdev, lp->pci_bars);
	pci_disable_device(lp->pdev);
}

static struct pci_cap_saved_state *
kgex_pci_find_saved_cap(struct pci_dev *pci_dev, u16 cap) {
	struct pci_cap_saved_state *tmp;

	hlist_for_each_entry(tmp, &pci_dev->saved_cap_space, next) {
		if (tmp->cap.cap_extended == true && tmp->cap.cap_nr == cap)
			return tmp;
	}
	return NULL;
}

static void kgex_pci_restore_config_dword(struct pci_dev *pdev, int offset,
		u32 saved_val, int retry) {
	u32 val;

	pci_read_config_dword(pdev, offset, &val);
	if (val == saved_val)
		return;

	for (;;) {
		pci_write_config_dword(pdev, offset, saved_val);
		if (retry-- <= 0)
			return;

		pci_read_config_dword(pdev, offset, &val);
		if (val == saved_val)
			return;

		mdelay(1);
	}
}

static void kgex_pci_restore_config_space_range(struct pci_dev *pdev, int start,
		int end, int retry) {
	int index;

	for (index = end; index >= start; index--)
		kgex_pci_restore_config_dword(pdev, 4 * index,
				pdev->saved_config_space[index], retry);
}

static void kgex_pci_restore_config_space(struct pci_dev *pdev) {
	if (pdev->hdr_type == PCI_HEADER_TYPE_NORMAL) {
		kgex_pci_restore_config_space_range(pdev, 10, 15, 0);
		kgex_pci_restore_config_space_range(pdev, 4, 9, 10);
		kgex_pci_restore_config_space_range(pdev, 0, 3, 0);
	} else {
		kgex_pci_restore_config_space_range(pdev, 0, 15, 0);
	}
}

static void kgex_pci_restore_pcie_state(struct pci_dev *dev) {
	int i = 0;
	struct pci_cap_saved_state *save_state;
	u16 *cap;

	save_state = kgex_pci_find_saved_cap(dev, PCI_CAP_ID_EXP);
	if (!save_state)
		return;

	cap = (u16 *)&save_state->cap.data[0];

	pcie_capability_write_word(dev, PCI_EXP_DEVCTL, cap[i++]);
	pcie_capability_write_word(dev, PCI_EXP_LNKCTL, cap[i++]);
	pcie_capability_write_word(dev, PCI_EXP_SLTCTL, cap[i++]);
	pcie_capability_write_word(dev, PCI_EXP_RTCTL, cap[i++]);
	pcie_capability_write_word(dev, PCI_EXP_DEVCTL2, cap[i++]);
	pcie_capability_write_word(dev, PCI_EXP_LNKCTL2, cap[i++]);
	pcie_capability_write_word(dev, PCI_EXP_SLTCTL2, cap[i++]);
}
