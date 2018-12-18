/**
 * @file kgex-main.c Exynos 10G ethernet driver main file.
 *
 * Contains network and PCI API functions for linkage with the linux kernel for
 * network
 * functionality.
 * It also has the implementation of the 'reviver' module that provisions for
 * detecting
 * and correcting error in the data flow between the exynos SOC and the FPGA.
 */
#include "kgex.h"
#include "kgex-debug.h"
#include "kgex-ethtool.h"
#include "kgex-hw.h"
#include "kgex-pci.h"
#include <linux/ethtool.h>

#include "version.h"

//#define DEBUG_FUNCTIONS

struct net_device *net_dev_tbl[KGEX_MAX_NET_DEVS] = {NULL, NULL};
static uint32_t num_net_devs = 0;

char *version = DRIVER_VERSION;
char *build_date = BUILD_DATE;

int tx_frag = 1;
module_param(tx_frag, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(tx_frag, "Enable frame fragmentation in TX path.");

int rx_align = 0;
module_param(rx_align, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(rx_align, "Align Socket Buffers for RX path. Use this for "
		"aligned AXI DMA RX channel.");

int tx_bd_num = TX_BD_NUM;
module_param(tx_bd_num, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(tx_bd_num, "Number of TX descriptors.");

int rx_bd_num = RX_BD_NUM;
module_param(rx_bd_num, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(rx_bd_num, "Number of RX descriptors.");

int recv_skb_sanity = 0;
module_param(recv_skb_sanity, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(recv_skb_sanity,
		"Check for malformed frames in the RX channel.");

int print_dropped_skb = 0;
module_param(print_dropped_skb, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(print_dropped_skb, "When an RX frame is dropped print it.");

unsigned int dma_mask = 0xFFFFFFFF;
module_param(dma_mask, uint, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(dma_mask, "Set DMA coherency mask.");

unsigned char exynos_default_mac_A_addr[] = {0x00, 0x00, 0xC0,
	0xA8, 0x03, 0x64};

unsigned char exynos_default_mac_C_addr[] = {0x00, 0x00, 0xC0,
	0xA8, 0x03, 0xC8};

/* Function Prototypes */
//   Platform Driver Functions
static int kgex_probe(struct pci_dev *pdev, const struct pci_device_id *ent);
static void kgex_remove(struct pci_dev *pdev);
static void kgex_shutdown(struct pci_dev *pdev);

// PCI Callbacks
static struct pci_driver kgex_driver = {
	.name = DRIVER_NAME,
	.id_table = kgex_pci_tbl,
	.probe = kgex_probe,
	.remove = kgex_remove,
	.shutdown = kgex_shutdown,
	//	.err_handler    = &netfpga_error_h,
};

module_pci_driver(kgex_driver);

// Network Device Functions
int kgex_open(struct net_device *dev);
int kgex_stop(struct net_device *dev);
static int kgex_xmit(struct sk_buff *skb, struct net_device *dev);
static struct net_device_stats *kgex_get_stats(struct net_device *dev);
static int kgex_change_mtu(struct net_device *ndev, int new_mtu);
static int kgex_set_mac_address(struct net_device *ndev, void *address);
static int kgex_xmit_done(struct net_device *ndev);

static void kgex_tx_ring_destroy(struct net_device *ndev);
static void kgex_rx_ring_destroy(struct net_device *ndev);

static void kgex_dma_revive_func(struct work_struct *work_arg);
static void kgex_xmit_work_func(struct work_struct *work_arg);

static irqreturn_t kgex_tx_irq(int irq, void *_ndev);
static irqreturn_t kgex_rx_irq(int irq, void *_ndev);

/**
 * (Networking API cb) Called when a network device feature is changed.
 * @param ndev			The network device.
 * @param features		The new features of the net device.
 *
 * @retval 0			On success
 * @retval non-zero		On failure
 **/
static int kgex_set_features(struct net_device *ndev,
		netdev_features_t features) {

	return 0;
}

/**
 * (Networking API cb) Called when a TX transaction has timed out.
 * Possible problem in the TX path of the adapter. The device needs
 * reset.
 *
 * @param ndev			The network device.
 **/
static void kgex_tx_timeout(struct net_device *ndev) {
	struct net_private *lp = NULL;
	lp = netdev_priv(ndev);
	pr("%s: Timeout\n", ndev->name);
}

static const struct net_device_ops kgex_ops = {
	.ndo_open = kgex_open,
	.ndo_stop = kgex_stop,
	.ndo_start_xmit = kgex_xmit,
	.ndo_get_stats = kgex_get_stats,
	.ndo_set_mac_address = kgex_set_mac_address,
	.ndo_change_mtu = kgex_change_mtu,
	.ndo_tx_timeout = kgex_tx_timeout,
	.ndo_set_features = kgex_set_features,
};

static const struct ethtool_ops kgex_ethtool_ops = {
	.get_settings = kgex_get_settings,
	.set_settings = kgex_set_settings,
	.get_drvinfo = kgex_get_drvinfo,
};

/**
 * (Helper) Changes the RX checksum calculation.
 *
 * @param ndev                  The network device.
 * @param csum_type		Value determining the checksum method.
 * 				CHECKSUM_NONE - SW (kernel stack)
 *				must calculate the RX checksum.
 *				CHECKSUM_UNNECESSARY - The checksum
 *				is calculated on the adapter.
 *
 * @retval 0			On success.
 * @retval non-zero		On failure.
 **/
int kgex_set_rx_ip_checksum(struct net_device *ndev, uint32_t csum_type) {
	struct net_private *lp = NULL;

	if (csum_type != CHECKSUM_UNNECESSARY && csum_type != CHECKSUM_NONE) {
		prerr("invalied RX IP checksum type value = %u\n", csum_type);
		return 1;
	}

	lp = netdev_priv(ndev);
	lp->rx_ip_checksum = csum_type;

	pr("Set RX IP checksum type to value %u\n", lp->rx_ip_checksum);

	return 0;
}

/**
 * (Helper) Returns the RX checksum calculation method.
 *
 * @param ndev                  The network device.
 *
 * @retval CHECKSUM_NONE	SW must calculate the csum.
 * @retval CHECKSUM_UNNECESSARY	Checksum is calsulated on adapter.
 **/
uint32_t kgex_get_rx_ip_checksum(struct net_device *ndev) {
	struct net_private *lp = NULL;
	lp = netdev_priv(ndev);
	return lp->rx_ip_checksum;
}

/**
 * (Helper) Calculates the number of active descriptors
 * in a queue. For the TX queue, active descriptors are the ones submitted
 * for TX, but not yet send by the DMA and/or cleared up by the driver.
 * For the RX queue, active descriptors are those for which the received
 * data are not pushed to the kernel network stack yet.
 *
 * @param queue_size 		Size of the queue
 * @param tail  		Tail descripor index
 * @param curr  		Current descriptor index
 *
 * @retval descs		Active descriptors number
 **/
static inline uint32_t get_queue_active_len(uint32_t queue_size, uint32_t tail,
		uint32_t curr) {
	if (curr <= tail) {
		return (tail - curr);
	} else if (curr > tail) {
		return (tail + (queue_size - curr));
	}
	return 0;
}

/**
 * (Helper) Performs a total reset of the network link.
 * Assuming that all network activity has stopped, it generates an 'external'
 * reset. That means that it writes the reset pins provided by the hardware
 * for both the DMA and the PCI endpoint and then (after the reset process
 * has finished) restores the state of the device. To achieve the reset
 * this function uses a callback that is defined in the pci-exynos driver.
 *
 * @param ndev  		The network device on which to perform the
 *				reset.
 *
 * @retval 0			On success.
 * @retval non-zero		On failure.
 **/
static inline int kgex_request_external_reset(struct net_device *ndev) {
	struct net_private *lp = NULL;

	lp = netdev_priv(ndev);

	if (lp->error_print) {
		prwarn("     %s: requesting external reset!\n", ndev->name);
	}

	if (lp->error_print) {
		pr("           %s resetting PCIe RC\n", ndev->name);
	}
	if (exynos_pcie_reset_rc(lp->pdev->bus->self) != 0) {
		return -1;
	}

	if (lp->error_print) {
		pr("           %s loading PCIe saved state\n", ndev->name);
	}
	pci_load_saved_state(lp->pdev, lp->pdev_state);
	if (lp->error_print) {
		pr("           %s restoring PCIe state\n", ndev->name);
	}
	kgex_pci_restore_state(ndev);
	kgex_pci_restore_msi(ndev);
	wmb();
	mdelay(10);
	exynos_pcie_link_restored(lp->pdev->bus->self);
	if (lp->error_print) {
		pr("           %s restoring MSI-X\n", ndev->name);
	}

	lp->reset_required = 0;
	lp->nr_external_resets++;
	return 0;
}

/**
 * (Helper) Checks if the TX queue has enough free descriptors.
 * Check if there is room to fit a specified skb (ethernet frame) fragments
 *number.
 *
 * @param lp			Pointer to the net_private structure (driver
 *specific).
 * @param num_frag		Number of skb (ethernet frame) fragments.
 *
 * @retval 0			On success.
 * @retval NETDEV_TX_BUSY	On failure.
 **/
static inline int kgex_check_tx_bd_space(struct net_private *lp, int num_frag) {
	struct dma_bd *cur_p;
	cur_p = &lp->tx_bd_virt[(lp->tx_tail + num_frag) % tx_bd_num];
	if (cur_p->status & DMA_BD_STS_ALL_MASK)
		return NETDEV_TX_BUSY;

	return 0;
}

/**
 * (Helper) Allocates memory space for the TX/RX rings DMA descriptors.
 *
 * @param ndev			The network device.
 *
 * @retval 0			On success.
 * @retval non-zero		On failure.
 **/
static int kgex_rings_alloc(struct net_device *ndev) {
	struct net_private *lp = NULL;
	int status = 0;

	if (ndev == NULL) {
		prerr("ndev == NULL\n");
		return 1;
	}
	lp = netdev_priv(ndev);
	if (lp == NULL) {
		prerr("lp == NULL\n");
		return 1;
	}

	status = dma_set_coherent_mask(ndev->dev.parent, dma_mask);

	if (lp->tx_bd_virt == NULL) {
		// Allocate TX descriptors (DMA capable)
		lp->tx_bd_virt =
			dma_alloc_coherent(ndev->dev.parent, sizeof(struct dma_bd) * tx_bd_num,
					&(lp->tx_bd_phys), GFP_KERNEL);

		if (lp->tx_bd_virt == NULL) {
			prerr("cannot allocate TX BD ring\n");
			return -ENOMEM;
		}
		if (lp->tx_bd_phys == 0x0) {
			prerr("cannot dma map TX BD ring\n");
			return -ENOMEM;
		}
		memset(lp->tx_bd_virt, 0, sizeof(struct dma_bd) * tx_bd_num);
	} else {
		prwarn("TX ring is already allocated\n");
	}

	if (lp->rx_bd_virt == NULL) {
		// Allocate RX descriptors (DMA capable)
		lp->rx_bd_virt =
			dma_alloc_coherent(ndev->dev.parent, sizeof(struct dma_bd) * rx_bd_num,
					&lp->rx_bd_phys, GFP_KERNEL);

		if (lp->rx_bd_virt == NULL) {
			prerr("cannot allocate RX BD ring\n");
			return -ENOMEM;
		}
		if (lp->rx_bd_phys == 0x0) {
			prerr("cannot dma map RX BD ring\n");
			return -ENOMEM;
		}
		memset(lp->rx_bd_virt, 0, sizeof(struct dma_bd) * rx_bd_num);
	} else {
		prwarn("RX ring is already allocated\n");
	}

	if (lp->rx_descs == NULL) {
		lp->rx_descs = (struct desc_struct *)kmalloc(
				sizeof(struct desc_struct) * rx_bd_num, GFP_ATOMIC);
		if (!lp->rx_descs) {
			prerr("cannot allocate parallel RX ring\n");
			return -ENOMEM;
		}
	} else {
		prwarn("RX parallel ring is already allocated\n");
	}

	if (lp->tx_descs == NULL) {
		lp->tx_descs = (struct desc_struct *)kmalloc(
				sizeof(struct desc_struct) * tx_bd_num, GFP_ATOMIC);
		if (!lp->tx_descs) {
			prerr("cannot allocate parallel TX ring\n");
			return -ENOMEM;
		}
	} else {
		prwarn("TX parallel ring is already allocated\n");
	}

	return 0;
}

/**
 * (Helper) Sets up the TX ring descriptors.
 *
 * @param ndev			The network device.
 *
 * @retval 0			On success.
 * @retval non-zero		On failure.
 **/
static int kgex_tx_ring_setup(struct net_device *ndev) {
	uint32_t i = 0;
	struct net_private *lp = NULL;
	struct dma_bd *cur_bd = NULL;

	if (!ndev) {
		prierr("kgex_tx_ring_setup(): ndev==NULL\n");
		return -EINVAL;
	}
	lp = netdev_priv(ndev);
	if (!lp) {
		prierr("kgex_tx_ring_setup(): lp==NULL\n");
		return -EINVAL;
	}

	lp->tx_curr = 0;
	lp->tx_tail = 0;
	lp->tx_xmit_times = 0;

	if (lp->tx_dma_mapped_descs != 0) {
		prierr("JVELEG: kgex_tx_ring_setup(): tx_dma_mapped_descs == %u\n",
				lp->tx_dma_mapped_descs);
	}

	wmb();
	/*Create linked-list of TX BDs*/
	for (i = 0; i < tx_bd_num; i++) {
		cur_bd = &(lp->tx_bd_virt[i]);
		cur_bd->next =
			LO32(lp->tx_bd_phys) + sizeof(struct dma_bd) * ((i + 1) % tx_bd_num);

		cur_bd->phys = 0x0;
		cur_bd->cntrl = 0x0;
		cur_bd->status = 0x0;
		cur_bd->app0 = 0x0;
		cur_bd->app1 = 0x0;
		cur_bd->app2 = 0x0;
		cur_bd->app3 = 0x0;
		cur_bd->app4 = 0x0;
		cur_bd->app5 = 0x0;
		lp->tx_descs[i].phys = 0;
		lp->tx_descs[i].skb = NULL;
		lp->tx_descs[i].len = 0;
	}

	lp->tx_rings_init = 1;

	return 0;
}

/**
 * (Helper) Sets up the RX ring descriptors.
 *
 * @param ndev			The network device.
 *
 * @retval 0			On success.
 * @retval non-zero		On failure.
 **/
static int kgex_rx_ring_setup(struct net_device *ndev) {
	uint32_t i = 0;
	struct net_private *lp = NULL;
	struct sk_buff *skb = NULL;
	dma_addr_t temp = 0x0;

	if (!ndev) {
		prierr("kgex_rx_ring_setup(): ndev==NULL\n");
		return -EINVAL;
	}
	lp = netdev_priv(ndev);
	if (!lp) {
		prierr("kgex_rx_ring_setup(): lp==NULL\n");
		return -EINVAL;
	}

	wmb();

	if (lp->rx_dma_mapped_descs != 0) {
		prierr("JVELEG: kgex_rx_ring_setup(): rx_dma_mapped_descs == %u\n",
				lp->rx_dma_mapped_descs);
	}

	/*Create linked-list and init RX BDs*/
	for (i = 0; i < rx_bd_num; i++) {
		// Create physical linked-list
		lp->rx_bd_virt[i].next =
			LO32(lp->rx_bd_phys) + sizeof(struct dma_bd) * ((i + 1) % rx_bd_num);
		skb = NULL;
		skb = napi_alloc_skb(&lp->napi, lp->max_frm_size + 2);
		if (!skb) {
			prerr("%s cannot allocate RX SKB #%u\n", ndev->name, i);
			return -ENOMEM;
		}
		if (recv_skb_sanity != 0) {
			memset(skb->data, 0xAA, lp->max_frm_size);
		}

		wmb();
		temp = 0x0;
		temp = dma_map_single(ndev->dev.parent, skb->data, lp->max_frm_size + 2,
				DMA_FROM_DEVICE);
		if (dma_mapping_error(ndev->dev.parent, temp)) {
			prerr("%s cannot DMA map RX skb No. #%u\n", ndev->name, i);
			return -ENOMEM;
		}
		if (temp == 0x0) {
			prerr("%s DMA map returned 0x0 for RX skb No. #%u\n", ndev->name, i);
			return -ENOMEM;
		}

		wmb();
		lp->rx_dma_mapped_descs++;
		lp->rx_bd_virt[i].phys = LO32(temp); /*HI32 bits of temp are unusable*/
		lp->rx_bd_virt[i].cntrl =
			lp->max_frm_size | DMA_BD_CTRL_TXSOF_MASK | DMA_BD_CTRL_TXEOF_MASK;
		lp->rx_bd_virt[i].status = 0x0;
		wmb();

		lp->rx_descs[i].phys = temp;
		lp->rx_descs[i].skb = skb;
		wmb();
	}

	wmb();
	rmb();

	lp->rx_rings_init = 1;

	return 0;
}

/**
 * (Netwoking API cb) Transmit an ethernet frame.
 * Sets up descriptors in the TX queue and submits them to
 * the AXI DMA.
 * @param skb			The socket buffer to be sent.
 * @param ndev			The network device.
 *
 * @retval 0			On success.
 * @retval non-zero		On failure
 **/
static int kgex_xmit(struct sk_buff *skb, struct net_device *ndev) {
	int i;
	uint32_t len;
	struct net_private *lp;
	uint32_t num_frag;
	struct dma_bd *cur_p;
	skb_frag_t *frag;
	dma_addr_t tail_phys;
	dma_addr_t temp = 0x0;

	dbg1("%s: kgex_xmit(): ENTER\n", ndev->name);

	lp = netdev_priv(ndev);

	num_frag = skb_shinfo(skb)->nr_frags;

	if (kgex_check_tx_bd_space(lp, num_frag)) {
		netif_stop_queue(ndev);
		return NETDEV_TX_BUSY;
	}

	cur_p = &lp->tx_bd_virt[lp->tx_tail];
	cur_p->cntrl = skb_headlen(skb) | DMA_BD_CTRL_TXSOF_MASK;
	temp = dma_map_single(ndev->dev.parent, skb->data, skb_headlen(skb),
			DMA_TO_DEVICE);
	if (dma_mapping_error(ndev->dev.parent, temp)) {
		prerr("%s kgex_xmit(): dma mapping error for skb->data\n", ndev->name);
		return NETDEV_TX_OK;
	}
	if (temp == 0x0) {
		prerr("%s kgex_xmit(): cannot dma map skb->data, temp==0x0\n", ndev->name);
		return NETDEV_TX_OK;
	}
	lp->tx_dma_mapped_descs++;
	lp->tx_descs[lp->tx_tail].phys = temp;
	lp->tx_descs[lp->tx_tail].skb = 0x0;
	lp->tx_descs[lp->tx_tail].dma_mapping = DMA_MAP_SINGLE;
	lp->tx_descs[lp->tx_tail].len = skb_headlen(skb);
	cur_p->phys = LO32(temp);

	if (HI32(temp) != 0x0) {
		prwarn("HI32(temp)!=0x0 temp=0x%llX\n", temp);
	}

	if (tx_frag == 0) {
		if (num_frag > 0) {
			pr("TX num_frag = %u\n", (uint32_t)(num_frag + 1));
			pr("TX frag #0 frag_size = %u\n", skb_headlen(skb));
		}
	}

	for (i = 0; i < num_frag; i++) {
		++lp->tx_tail;
		lp->tx_tail %= tx_bd_num;
		cur_p = &lp->tx_bd_virt[lp->tx_tail];
		frag = &skb_shinfo(skb)->frags[i];
		len = skb_frag_size(frag);
		temp = 0x0;
		temp = skb_frag_dma_map(ndev->dev.parent, frag, 0, len, DMA_TO_DEVICE);

		if (dma_mapping_error(ndev->dev.parent, temp)) {
			prerr("%s kgex_xmit(): dma mapping error for skb frag\n", ndev->name);
			return NETDEV_TX_OK;
		}
		if (temp == 0x0) {
			prerr("%s kgex_xmit(): cannot dma map skb frag, temp==0x0\n", ndev->name);
			return NETDEV_TX_OK;
		}
		if (HI32(temp) != 0x0) {
			prwarn("HI32(temp)!=0x0 temp=0x%llX\n", temp);
		}
		lp->tx_dma_mapped_descs++;
		cur_p->phys = temp;
		cur_p->cntrl = 0x0 | len;
		lp->tx_descs[lp->tx_tail].phys = temp;
		lp->tx_descs[lp->tx_tail].dma_mapping = DMA_MAP_PAGE;
		lp->tx_descs[lp->tx_tail].len = len;
		lp->tx_descs[lp->tx_tail].skb = 0x0;
	}

	cur_p->cntrl |= DMA_BD_CTRL_TXEOF_MASK;
	lp->tx_descs[lp->tx_tail].skb = skb;

	/*Find last BD physical address*/
	tail_phys = lp->tx_bd_phys + sizeof(struct dma_bd) * lp->tx_tail;
	wmb();

	if (HI32(tail_phys) != 0x0) {
		prwarn("HI32(tail_phys)!=0x0 tail_phys=0x%llX\n", tail_phys);
	}
	/*Start DMA transfer*/
	dma_write32_prot(lp, DMA_TX_TDESC_OFFSET, LO32(tail_phys));

	/*Compute next new tail*/
	++(lp->tx_tail);
	lp->tx_tail %= tx_bd_num;

	lp->tx_xmit_times++;

	dbg1("%s: kgex_xmit(): RETURN\n", ndev->name);

	return NETDEV_TX_OK;
}

/**
 * (Helper) Clean up the TX ring.
 * Looks up for completed descriptors in the TX channel
 * - meaning successfully transmitted ethernet frames. Resets the completed
 * descriptors for further use.
 *
 * @param ndev			The network device.
 *
 * @retval 0			On success.
 * @retval non-zero		On error.
 **/
static int kgex_xmit_done(struct net_device *ndev) {
	unsigned int status = 0;
	unsigned int status2 = 0;
	uint32_t size = 0;
	uint32_t packets = 0;
	struct net_private *lp = NULL;
	struct dma_bd *cur_bd;

	dbg1("%s: kgex_xmit_done() ENTER\n", ndev->name);

	lp = netdev_priv(ndev);

	cur_bd = &lp->tx_bd_virt[lp->tx_curr];
	status = cur_bd->status;
	while ((status & DMA_BD_STS_COMPLETE_MASK) ||
			(status & DMA_BD_STS_ALL_ERR_MASK)) {
		dbg2("kgex_xmit_done(): cur_bd->phys = 0x%08X\n", cur_bd->phys);
		dbg2("kgex_xmit_done(): cur_bd->next = 0x%08X\n", cur_bd->next);

		if (lp->tx_descs[lp->tx_curr].dma_mapping == DMA_MAP_SINGLE) {
			dma_unmap_single(ndev->dev.parent,
					(dma_addr_t)(lp->tx_descs[lp->tx_curr].phys),
					lp->tx_descs[lp->tx_curr].len, DMA_TO_DEVICE);
		} else {
			dma_unmap_page(ndev->dev.parent,
					(dma_addr_t)(lp->tx_descs[lp->tx_curr].phys),
					lp->tx_descs[lp->tx_curr].len, DMA_TO_DEVICE);
		}
		lp->tx_dma_mapped_descs--;

		if (lp->tx_descs[lp->tx_curr].skb != NULL) {
			status2 = kgex_virt_addr_error((uint64_t)lp->tx_descs[lp->tx_curr].skb);
			if (status2 == 0) {
				dev_kfree_skb_irq(lp->tx_descs[lp->tx_curr].skb);
			} else {
				prwarn("kgex_xmit_done(): Invalid virt addr==%llX\n",
						(uint64_t)lp->tx_descs[lp->tx_curr].skb);
			}
		}
		cur_bd->phys = 0;
		cur_bd->app0 = 0;
		cur_bd->app4 = 0;
		cur_bd->app5 = 0;
		cur_bd->status = 0;
		lp->tx_descs[lp->tx_curr].phys = 0;
		lp->tx_descs[lp->tx_curr].skb = 0;
		lp->tx_descs[lp->tx_curr].len = 0;

		smp_mb();

		if (kgex_dma_status_bd_decode(status)) {
			lp->tx_dma_desc_errors++;
			if (lp->error_print) {
				prerr("DMA TX descriptor ERROR - curr desc #%u at virt 0x%llX, Frame "
						"phys 0x%llX\n",
						lp->tx_curr, (uint64_t)(&lp->tx_bd_virt[lp->tx_curr]),
						(uint64_t)cur_bd->phys);
			}
			return -1;
		}

		size += status & DMA_BD_STS_ACTUAL_LEN_MASK;
		ndev->stats.tx_bytes += status & DMA_BD_STS_ACTUAL_LEN_MASK;

		if (cur_bd->cntrl & DMA_BD_CTRL_TXEOF_MASK) {
			packets++;
			ndev->stats.tx_packets += 1;
		}

		++lp->tx_curr;
		lp->tx_curr %= tx_bd_num;
		cur_bd = &lp->tx_bd_virt[lp->tx_curr];
		status = cur_bd->status;
	}

	netif_start_queue(ndev);

	return 0;
}

/**
 * (Helper) Receive one socket buffer.
 * It gets a ring descriptor, extracts the information,
 * pushes the packet up to the network stack and makes
 * the descriptor ready to accepet the next packet.
 *
 * @param ndev			The network device.
 *
 * @retval size			The number of received bytes.
 * @retval <=0			On error.
 **/
static inline int kgex_recv_one(struct net_device *ndev) {
	unsigned int len = 0;
	unsigned int size = 0;
	dma_addr_t tail_phys = 0;
	struct net_private *lp = NULL;
	struct sk_buff *skb, *new_skb;
	struct dma_bd *cur_bd;
	int netrx = 0;
	dma_addr_t phys_cpu = 0x0;
	struct desc_struct *cur_desc = NULL;

	lp = netdev_priv(ndev);

	if (!lp->rx_rings_init)
		return -1;

	cur_bd = &lp->rx_bd_virt[lp->rx_curr];
	cur_desc = &lp->rx_descs[lp->rx_curr];

	tail_phys = lp->rx_bd_phys + sizeof(struct dma_bd) * lp->rx_curr;
	rmb();

	skb = cur_desc->skb;

	len = cur_bd->status & DMA_BD_STS_ACTUAL_LEN_MASK;

	if (cur_bd->phys == 0x0) {
		prerr("%s RX desc cur_bd->phys==0x0\n", ndev->name);
	}

	if (cur_desc->phys != cur_bd->phys) {
		prierr("JVELEG: dbg kgex_recv(): cur_desc->phys!=cur_bd->phys\n");
	}

	dma_unmap_single(ndev->dev.parent, cur_desc->phys, lp->max_frm_size,
			DMA_FROM_DEVICE);
	cur_desc->phys = 0x0;
	cur_bd->phys = 0x0;
	lp->rx_dma_mapped_descs--;
	wmb();

	if (kgex_dma_status_bd_decode(cur_bd->status)) {
		lp->rx_dma_desc_errors++;
		if (lp->error_print) {
			prerr("DMA RX descriptor ERROR - curr desc #%u at virt 0x%llX phys "
					"0x%llX, len = %u, Frame phys 0x%llX\n",
					lp->rx_curr, (uint64_t)(&lp->rx_bd_virt[lp->rx_curr]), tail_phys,
					len, (uint64_t)cur_bd->phys);
		}
		return -1;
	}
	wmb();

	skb->dev = ndev;
	skb_put(skb, len);
	skb->protocol = eth_type_trans(skb, ndev);
	skb->ip_summed = lp->rx_ip_checksum;

	netrx = napi_gro_receive(&lp->napi, skb);

	size += len;
	if (netrx == GRO_DROP) {
		size = 0;
	}

	new_skb = napi_alloc_skb(&lp->napi, lp->max_frm_size + 2);
	if (new_skb == NULL) {
		prerr("cannot allocate new RX skb\n");
		cur_desc->skb = NULL;
		return -1;
	}

	wmb();
	phys_cpu = 0x0;
	phys_cpu = dma_map_single(ndev->dev.parent, new_skb->data, lp->max_frm_size,
			DMA_FROM_DEVICE);

	if (phys_cpu == 0x0) {
		prerr("kgex_recv(): %s Cannot DMA map new skb\n", ndev->name);
		return -1;
	}

	cur_desc->phys = phys_cpu;
	cur_desc->skb = new_skb;
	lp->rx_dma_mapped_descs++;
	wmb();

	cur_bd->phys = LO32(phys_cpu);
	cur_bd->cntrl =
		lp->max_frm_size | DMA_BD_CTRL_TXSOF_MASK | DMA_BD_CTRL_TXEOF_MASK;
	cur_bd->status = 0x0;

	return size;
}

/**
 * (Helper) Cleans up RX ring from completed packets.
 * Looks up for completed descriptors in the RX queue.
 * The data associated with completed descriptors are converted
 * into socket buffers (skb) and pushed into the kernel network stack.
 * AXI DMA channels. The completed descriptors are then freed and
 * reset for further incoming ethernet frames.
 *
 * @param ndev			The network device.
 *
 * @retval packets		The number of packets received in this round.
 **/
int kgex_recv(struct net_device *ndev, int budget) {
	unsigned int packets = 0;
	unsigned int size = 0;
	dma_addr_t tail_phys = 0;
	struct net_private *lp = NULL;
	struct dma_bd *cur_bd;
	struct desc_struct *cur_desc = NULL;

	lp = netdev_priv(ndev);

	rmb();

	cur_bd = &lp->rx_bd_virt[lp->rx_curr];
	cur_desc = &lp->rx_descs[lp->rx_curr];
	while (((cur_bd->status & DMA_BD_STS_COMPLETE_MASK) ||
				(cur_bd->status & DMA_BD_STS_ALL_ERR_MASK)) &&
			packets < budget) {
		tail_phys = lp->rx_bd_phys + sizeof(struct dma_bd) * lp->rx_curr;
		rmb();

		size = kgex_recv_one(ndev);
		if (size >= 0) {
			ndev->stats.rx_packets += 1;
			ndev->stats.rx_bytes += size;
		} else {
			ndev->stats.rx_dropped += 1;
		}

		packets++;
		++lp->rx_curr;
		lp->rx_curr %= rx_bd_num;

		cur_bd = &lp->rx_bd_virt[lp->rx_curr];
		cur_desc = &lp->rx_descs[lp->rx_curr];

		rmb();
		wmb();
	}

	if (tail_phys) {
		dma_write32_prot(lp, DMA_RX_TDESC_OFFSET, LO32(tail_phys));
		if (HI32(tail_phys) != 0) {
			prwarn("kgex_recv(): HI32(tail_phys)");
		}
	}

	return packets;
}

/**
 * (Networking API cb) NAPI polling routine.
 * Polls for received packets up to a budget value.
 *
 * @param napi			The NAPI struct for the network device.
 *
 * @retval packets		The number of packets received in this round.
 **/
static int napi_poll(struct napi_struct *napi, int budget) {
	struct net_private *lp = NULL;
	int work_done = 0;
	lp = netdev_priv(napi->dev);
	work_done = kgex_recv(napi->dev, budget);

	if (work_done < budget) {
		napi_complete(&lp->napi);
		dma_write32_prot(lp, DMA_RX_SR_OFFSET, DMA_IRQ_ALL_MASK);
	}

	return work_done;
}

/**
 * (Helper) Workqueue function for cleaning up the TX ring.
 *
 * @param work_arg		The struct to the workqueue info.
 **/
static void kgex_xmit_work_func(struct work_struct *work_arg) {
	struct net_private *lp = NULL;
	struct net_device *ndev = NULL;
	struct delayed_work *kwork = NULL;
	struct kgex_delayed_work *c_ptr;
	int status = 0;

	kwork = container_of(work_arg, struct delayed_work, work);
	c_ptr = container_of(kwork, struct kgex_delayed_work, work);
	if (!kwork) {
		prierr("kwork==NULL\n");
		return;
	}
	ndev = c_ptr->ndev;
	if (!ndev) {
		prierr("kwork->ndev==NULL");
		return;
	}
	lp = netdev_priv(ndev);

	status = kgex_xmit_done(ndev);

	if (status != 0) {
		pr("Error in xmit!\n");
	}

	lp->xmit_working = 0;
	dma_write32_prot(lp, DMA_TX_SR_OFFSET, DMA_IRQ_ALL_MASK);
}

/**
 * (Helper) Detects and corrects errors on the netork adapter.
 * This function is the basic block of the KGEX reviver mechanism
 * (@ref kgexreviver). It is executed every 'reviver_jiffies'
 * jiffies and checks for any error, DMA or PCI link related.
 * If it detects any of them, it performs an 'external' reset.
 * An external reset is a 'hard' reset on all the components of
 * the network link. It then waits for the reset to end and
 * resumes normal operation of the network device.
 *
 * @param work_arg		The struct to the workqueue info.
 **/
static void kgex_dma_revive_func(struct work_struct *work_arg) {
	struct net_private *lp = NULL;
	struct net_device *ndev = NULL;
	struct delayed_work *kwork = NULL;
	struct kgex_delayed_work *c_ptr;
	uint32_t status = 0;
	int gen = 0;
	uint32_t tx_cdesc = 0x00000000;
	uint32_t tx_tdesc = 0x00000000;
	static uint32_t cnt = 0;

	kwork = container_of(work_arg, struct delayed_work, work);
	c_ptr = container_of(kwork, struct kgex_delayed_work, work);
	if (!kwork) {
		prierr("kwork==NULL\n");
		return;
	}
	ndev = c_ptr->ndev;
	if (!ndev) {
		prierr("kwork->ndev==NULL");
		return;
	}
	lp = netdev_priv(ndev);

	cnt++;
	wmb();

	gen = kgex_pci_get_gen(ndev);
	///@todo: Decide whether we use rest or retrain only in that case
	if (gen < 2 && gen != 0) {
		if (lp->error_print) {
			prwarn("%s PCI not gen2. CPU core = %u\n", ndev->name,
					smp_processor_id());
		}
		lp->reset_required = 1;
		goto reset;
		// exynos_pcie_link_retrain(lp->pdev->bus->self);
	}

	status = dma_read32_prot(lp, DMA_TX_SR_OFFSET);
	status = kgex_dma_status_reg_decode(ndev, status);
	if (status) {
		lp->tx_dma_errors++;
		lp->reset_required = 1;
		goto reset;
	}

	status = dma_read32_prot(lp, DMA_RX_SR_OFFSET);
	status = kgex_dma_status_reg_decode(ndev, status);
	if (status) {
		lp->rx_dma_errors++;
		lp->reset_required = 1;
		goto reset;
	}

	status = dma_read32_prot(lp, DMA_TX_SR_OFFSET);
	if ((!(status & DMA_STS_IDLE)) && status != 0xFFFFFFFF) {
		tx_cdesc = dma_read32_prot(lp, DMA_TX_CDESC_OFFSET);
		tx_tdesc = dma_read32_prot(lp, DMA_TX_TDESC_OFFSET);
		if (tx_tdesc != 0x00000000 && tx_cdesc != tx_tdesc &&
				tx_cdesc == lp->dma_tx_cdesc_prev) {
			(lp->reviver_dma_consecutive_ongoing)++;
		} else {
			lp->reviver_dma_consecutive_ongoing = 0;
			lp->dma_tx_cdesc_prev = tx_cdesc;
		}

		if (lp->reviver_dma_consecutive_ongoing > 1) {
			if (lp->error_print) {
				prerr("%s Ongoing for %u jiffies. Requesting FULL reset.\n", ndev->name,
						lp->reviver_dma_consecutive_ongoing * lp->revive_jiffies);
			}
			lp->reset_required = 1;
			lp->dma_tx_cdesc_prev = 0x00000000;
			lp->reviver_dma_consecutive_ongoing = 0;
			goto reset;
		}
	}

reset:
	if (lp->reset_required) {
		netif_tx_disable(ndev);
		wmb();
		smp_mb();

		if (lp->error_print) {
			pr("%s reset required\n", ndev->name);
		}

		if (lp->error_print) {
			pr("     %s stoping DMA TX channel and IRQs\n", ndev->name);
		}
		kgex_dma_tx_stop_irq_disable(ndev);
		if (lp->error_print) {
			pr("     %s stoping DMA RX channel and IRQs\n", ndev->name);
		}
		kgex_dma_rx_stop_irq_disable(ndev);

		status = kgex_request_external_reset(ndev);
		if (status != 0) {
			goto revive_reschedule;
		}

		if (lp->error_print) {
			pr("     %s destroying TX ring\n", ndev->name);
		}
		kgex_tx_ring_destroy(ndev);
		if (lp->error_print) {
			pr("     %s destroying RX ring\n", ndev->name);
		}
		kgex_rx_ring_destroy(ndev);

		if (lp->error_print) {
			pr("     %s setting up TX ring\n", ndev->name);
		}
		status = kgex_tx_ring_setup(ndev);
		if (status != 0) {
			prerr("%s cannot setup tx ring, stopping reviver\n", ndev->name);
			lp->revive_jiffies = 0;
			lp->reset_required = 0;
			goto revive_reschedule;
		}

		if (lp->error_print) {
			pr("     %s setting up RX ring\n", ndev->name);
		}
		status = kgex_rx_ring_setup(ndev);
		if (status != 0) {
			prerr("%s cannot setup rx ring, stopping reviver\n", ndev->name);
			lp->revive_jiffies = 0;
			kgex_tx_ring_destroy(ndev);
			lp->reset_required = 0;
			goto revive_reschedule;
		}

		if (lp->error_print) {
			pr("     %s starting DMA TX channel and IRQs\n", ndev->name);
		}
		kgex_dma_tx_start(ndev);
		if (lp->error_print) {
			pr("     %s starting DMA RX channel and IRQs\n", ndev->name);
		}
		kgex_dma_rx_start(ndev);
		lp->reset_required = 0;
		wmb();
		if (lp->error_print) {
			pr("     %s waking kernel TX netdev queue\n", ndev->name);
		}
		netif_wake_queue(ndev);
		wmb();
		smp_mb();

		if (lp->error_print) {
			pr(" %s TX/RX recovered\n", ndev->name);
		}
		goto revive_reschedule;
	}

revive_reschedule:
	if (lp->revive_jiffies > 0) {
		schedule_delayed_work_on(lp->reviver_affinity, &(lp->reviver_work->work),
				lp->revive_jiffies);
	}
}

/**
 * (Helper) Deallocates (frees) the TX/RX descriptor rings.
 *
 * @param ndev			The network device.
 **/
static void kgex_rings_dealloc(struct net_device *ndev) {
	struct net_private *lp = NULL;

	lp = netdev_priv(ndev);

	if (lp->tx_bd_virt != NULL) {
		dma_free_coherent(ndev->dev.parent, sizeof(struct dma_bd) * tx_bd_num,
				lp->tx_bd_virt, lp->tx_bd_phys);
		lp->tx_bd_virt = NULL;
		lp->tx_bd_phys = 0x0;
	} else {
		prwarn("TX ring previously deallocated. Doing nothing\n");
	}

	if (lp->rx_bd_virt != NULL) {
		dma_free_coherent(ndev->dev.parent, sizeof(struct dma_bd) * rx_bd_num,
				lp->rx_bd_virt, lp->rx_bd_phys);
		lp->rx_bd_virt = NULL;
		lp->rx_bd_phys = 0x0;
	} else {
		prwarn("RX ring previously deallocated. Doing nothing\n");
	}
}

/**
 * (Helper) Empties the TX descriptor ring.
 *
 * @param ndev			The network device.
 **/
static void kgex_tx_ring_destroy(struct net_device *ndev) {
	uint32_t i = 0;
	struct net_private *lp = NULL;
	struct dma_bd *cur_bd = NULL;

	dbg1("kgex_tx_ring_destroy(): ENTER\n");
	lp = netdev_priv(ndev);

	/*Free TX BD ring*/
	for (i = 0; i < tx_bd_num; i++) {
		cur_bd = &lp->tx_bd_virt[i];

		if (lp->tx_descs[i].phys) {
			if (lp->tx_descs[i].dma_mapping == DMA_MAP_SINGLE) {
				dma_unmap_single(ndev->dev.parent, (dma_addr_t)(lp->tx_descs[i].phys),
						lp->tx_descs[i].len, DMA_TO_DEVICE);
			} else {
				dma_unmap_page(ndev->dev.parent, (dma_addr_t)(lp->tx_descs[i].phys),
						lp->tx_descs[i].len, DMA_TO_DEVICE);
			}
			lp->tx_dma_mapped_descs--;
		}

		if (lp->tx_descs[i].skb != NULL) {
			if (kgex_virt_addr_error((uint64_t)lp->tx_descs[i].skb) == 0) {
				dev_kfree_skb_irq(lp->tx_descs[i].skb);
			} else {
				if (lp->error_print) {
					prwarn("%s - Invalid virt addr==%llX\n", __func__,
							(uint64_t)lp->tx_descs[i].skb);
				}
			}
		}

		cur_bd->phys = 0;
		cur_bd->cntrl = 0;
		cur_bd->status = 0;
		cur_bd->app0 = 0;
		cur_bd->app1 = 0;
		cur_bd->app2 = 0;
		cur_bd->app3 = 0;
		cur_bd->app4 = 0;
		cur_bd->app5 = 0;
		lp->tx_descs[i].phys = 0;
		lp->tx_descs[i].skb = 0;
		lp->tx_descs[i].len = 0;
		wmb();
	}

	lp->tx_tail = 0;
	lp->tx_curr = 0;
	lp->tx_rings_init = 0;

	wmb();

	if (lp->tx_dma_mapped_descs != 0) {
		prierr("JVELEG: kgex_tx_ring_destroy(): tx_dma_mapped_descs == %u\n",
				lp->tx_dma_mapped_descs);
	}
}

/**
 * (Helper) Empties the RX descriptor ring.
 *
 * @param ndev			The network device.
 **/
static void kgex_rx_ring_destroy(struct net_device *ndev) {
	uint32_t i = 0;
	struct net_private *lp = NULL;
	struct dma_bd *cur_bd = NULL;
	uint32_t status;

	dbg1("kgex_rx_ring_destroy(): ENTER\n");
	lp = netdev_priv(ndev);

	/*Free RX BD ring*/
	for (i = 0; i < rx_bd_num; i++) {
		cur_bd = &lp->rx_bd_virt[i];

		if (lp->rx_descs[i].skb) {

			if (lp->rx_descs[i].phys) {
				dma_unmap_single(ndev->dev.parent, lp->rx_descs[i].phys,
						lp->max_frm_size + 2, DMA_FROM_DEVICE);
				lp->rx_dma_mapped_descs--;
			} else {
				prwarn("kgex_rx_ring_destroy(): %s Found unmapped skb "
						"cur_desc[%u].phys = 0x%llX \n",
						ndev->name, i, lp->rx_descs[i].phys);
			}
			status = kgex_virt_addr_error((uint64_t)(lp->rx_descs[i].skb));
			if (status != 0) {
				prwarn(
						"kgex_rx_ring_destroy(): %s Invalid virt addr==%llX for desc #%u\n",
						ndev->name, (uint64_t)(lp->rx_descs[i].skb), i);
			} else {
				dev_kfree_skb_irq(lp->rx_descs[i].skb);
			}
		}
		lp->rx_descs[i].phys = 0x0;
		lp->rx_descs[i].skb = NULL;
		lp->rx_descs[i].len = 0;

		cur_bd->phys = 0;
		cur_bd->status = 0;
		cur_bd->cntrl = 0;
		cur_bd->app0 = 0;
		cur_bd->app1 = 0;
		cur_bd->app2 = 0;
		cur_bd->app3 = 0;
		cur_bd->app4 = 0;
		cur_bd->app5 = 0;
	}

	lp->rx_tail = 0;
	lp->rx_curr = 0;
	lp->rx_rings_init = 0;

	wmb();
	if (lp->rx_dma_mapped_descs != 0) {
		prierr("JVELEG: kgex_rx_ring_destroy(): rx_dma_mapped_descs == %u\n",
				lp->rx_dma_mapped_descs);
	}
}

/* Interrupt Handlers */
/**
 * (IRQ) Interrupt handler for the TX channel.
 * Invoked when the AXI DMA completes transmission of
 * a specified number of ethernet frames. When an interrupt
 * occurs, a workqueue is scheduled to clear the completed
 * packets. When there are no more packets the interrupt status
 * is cleared and the device can produce new interrupts.
 *
 * @param irq 			IRQ number.
 * @param _ndev			The network device.
 *
 * @retval IRQ_HANDLED		If the interrupt was served.
 * @retval IRQ_NONE		If the interrupt was spurious.
 **/
static irqreturn_t kgex_tx_irq(int irq, void *_ndev) {
	unsigned int status = 0;
	volatile unsigned int ctrl = 0;
	struct net_device *ndev;
	struct net_private *lp;

	ndev = _ndev;
	lp = netdev_priv(ndev);

	dbg1("%s: kgex_tx_irq(): ENTER\n", ndev->name);

	if (lp->tx_irq_enabled == 0) {
		return IRQ_HANDLED;
	}

	lp->nr_tx_irqs += 1;

	if (lp->reset_required) {
		return IRQ_HANDLED;
	}

	status = dma_read32_prot(lp, DMA_TX_SR_OFFSET);

	ctrl = dma_read32_prot(lp, DMA_TX_CR_OFFSET);
	if (!(ctrl & DMA_IRQ_ALL_MASK)) {
		lp->tx_irq_spurious++;

		return IRQ_HANDLED;
	}

	if ((status & DMA_IRQ_ERROR_MASK) || (status & DMA_STS_HALTED)) {
		kgex_dma_tx_stop_irq_disable(ndev);
		kgex_dma_rx_stop_irq_disable(ndev);

		return IRQ_HANDLED;
	}

	if (status & (DMA_IRQ_IOC_MASK | DMA_IRQ_DELAY_MASK)) {
		if (lp->xmit_working) {
			pr("xmit already working");
		} else {
			lp->xmit_working = 1;
			schedule_delayed_work(&(lp->xmit_work->work), 0);
		}

		return IRQ_HANDLED;
	}

	lp->tx_irq_unknown++;

	return IRQ_HANDLED;
}

/**
 * (IRQ) Interrupt handler for the RX channel.
 * Invoked when the AXI DMA receives a specified number of
 * packets
 * a specified number of ethernet frames. When an interrupt
 * occurs, a workqueue is scheduled to clear the completed
 * packets. When there are no more packets the interrupt status
 * is cleared and the device can produce new interrupts.
 *
 * @param irq 			IRQ number.
 * @param _ndev			The network device.
 *
 * @retval IRQ_HANDLED		If the interrupt was served.
 * @retval IRQ_NONE		If the interrupt was spurious.
 **/
static irqreturn_t kgex_rx_irq(int irq, void *_ndev) {
	struct net_device *ndev;
	struct net_private *lp;
	unsigned int status = 0;
	volatile unsigned int ctrl = 0;

	ndev = _ndev;
	lp = netdev_priv(ndev);

	if (lp->rx_irq_enabled == 0) {
		return IRQ_HANDLED;
	}

	if (lp->reset_required) {
		return IRQ_HANDLED;
	}

	lp->nr_rx_irqs++;

	status = dma_read32_prot(lp, DMA_RX_SR_OFFSET);
	ctrl = dma_read32_prot(lp, DMA_RX_CR_OFFSET);

	if (status == 0xFFFFFFFF) {
		pr("%s: kgex_rx_irq(): Link Down... status: %X\n", ndev->name, status);

		return IRQ_HANDLED;
	}
	if (!(ctrl & DMA_IRQ_ALL_MASK)) {
		pr("%s: kgex_rx_irq(): Interrupts are disabled!!\n", ndev->name);
		lp->rx_irq_spurious++;

		return IRQ_HANDLED;
	}

	if ((status & DMA_IRQ_ERROR_MASK) || (status & DMA_STS_HALTED)) {
		pr("%s: kgex_rx_irq(): Error interrupt! status: %X\n", ndev->name, status);

		return IRQ_HANDLED;
	}

	// if (status & (DMA_IRQ_IOC_MASK | DMA_IRQ_DELAY_MASK))
	//{
	// pr("%s: kgex_rx_irq(): IOC interrupt! status: %X\n", ndev->name, status);
	// NAPI
	// pr("napi schedule!\n");

	napi_schedule(&lp->napi);

	return IRQ_HANDLED;
	//}

	lp->rx_irq_unknown++;
	prwarn("%s Unknown RX interrupt\n", ndev->name);
	return IRQ_HANDLED;
}
/* End of  Interrupt Handlers */

/* Network Device Functions */
/**
 * (Networking API cb) Change MTU size.
 *
 * @param  			IRQ number.
 * @param _ndev			The network device.
 *
 * @retval IRQ_HANDLED		If the interrupt was served.
 * @retval IRQ_NONE		If the interrupt was spurious.
 **/
int kgex_set_mtu(struct net_device *ndev, uint32_t new_mtu) {
	struct net_private *lp = NULL;

	if (!ndev) {
		prerr("ndev==NULL\n");
		return 1;
	}

	lp = netdev_priv(ndev);
	if (!lp) {
		prerr("lp==NULL\n");
		return 2;
	}

	if (new_mtu > KGE_JUMBO_MTU) {
		prwarn("Given MTU is %u > 9000 Bytes. Setting MTU = 9000 Bytes.\n",
				(uint32_t)new_mtu);
		lp->max_frm_size = KGE_MAX_VLAN_JUMBO_FRAME_SIZE;
		ndev->mtu = KGE_JUMBO_MTU;
	} else if (new_mtu < 64) {
		prwarn("Given MTU is %u < 64 Bytes. Setting MTU = 1500 Bytes.\n",
				(uint32_t)new_mtu);
		lp->max_frm_size = KGE_MAX_VLAN_FRAME_SIZE;
		ndev->mtu = KGE_MTU;
	} else {
		lp->max_frm_size = new_mtu + KGE_HDR_SIZE + VLAN_HLEN + KGE_TRL_SIZE;
		ndev->mtu = new_mtu;
	}

	return 0;
}

/**
 * kgex_open - opens a network device. Usually called
 * when ifconfig up is executed. Also resets the HW
 * and TX/RX queues.
 * @ndev: pointer to net_device structure
 *
 * Return: 0 on success.
 **/
int kgex_open(struct net_device *ndev) {
	int status = -1;
	struct net_private *lp;

	dbg1("kgex_open(): ENTER\n");

	lp = netdev_priv(ndev);
	lp->cnt = 0;

	ndev->mtu = lp->max_frm_size - KGE_HDR_SIZE - VLAN_HLEN - KGE_TRL_SIZE;

	status = kgex_tx_ring_setup(ndev);
	if (status != 0) {
		prerr("Cannot init TX rings! %s open failed.\n", ndev->name);
		return -ENOMEM;
	}

	status = kgex_rx_ring_setup(ndev);
	if (status != 0) {
		prerr("Cannot init RX rings! %s open failed.\n", ndev->name);
		return -ENOMEM;
	}

	lp->reset_required = 1;

	pr("Enabling NAPI!\n");
	napi_enable(&lp->napi);
	status = request_irq(lp->tx_irq, kgex_tx_irq, IRQF_SHARED, ndev->name, ndev);
	if (status) {
		prerr("ERROR: cannot get TX IRQ %u\n", lp->tx_irq);
		return -ENOMEM;
	}

	status = request_irq(lp->rx_irq, kgex_rx_irq, IRQF_SHARED, ndev->name, ndev);
	if (status) {
		prerr("ERROR: cannot get RX IRQ %u\n", lp->rx_irq);
		return -ENOMEM;
	}
	lp->irqs = 1;
	lp->rx_ip_checksum = CHECKSUM_UNNECESSARY;
	lp->tx_irq_spurious = 0;
	lp->rx_irq_spurious = 0;
	lp->tx_active_descs = 0;
	lp->tx_active_descs = 0;
	lp->queue_stopped_times = 0;
	lp->tx_xmit_times = 0;
	lp->tx_irq_unknown = 0;
	lp->rx_irq_unknown = 0;
	lp->nr_pci_retrains = 0;
	lp->nr_pci_gen_transitions = 0;
	lp->nr_external_resets = 0;
	lp->dma_tx_cdesc_prev = 0;
	lp->reviver_dma_consecutive_ongoing = 0;
	kgex_dma_tx_start(ndev);
	kgex_dma_rx_start(ndev);

	netif_start_queue(ndev);

	pr("%s opened. \n  MTU = %u, Max Frame Size = %u TX SKB Fragmentation = %d  "
			"RX IP Csum = %u  TX/RX IRQ = %d/%d \n  TX/RX coalescing = %u/%u, TX/RX "
			"timeout = %u/%u. TX/RX descriptors = %u/%u.\n  DMA mask = 0x%08X, TX "
			"ring base phys = 0x%08X , RX ring base phys = 0x%08X\n  Print dropped "
			"skb = %d SKB Sanity Check =  %d\n",
			ndev->name, ndev->mtu, lp->max_frm_size, tx_frag, lp->rx_ip_checksum,
			lp->tx_irq, lp->rx_irq, lp->coalesce_count_tx, lp->coalesce_count_rx,
			lp->timeout_tx, lp->timeout_rx, tx_bd_num, rx_bd_num, dma_mask,
			(uint32_t)lp->tx_bd_phys, (uint32_t)lp->rx_bd_phys, print_dropped_skb,
			recv_skb_sanity);

	if (lp->reviver_work != NULL && lp->revive_jiffies > 0) {
		schedule_delayed_work(&(lp->reviver_work->work), lp->revive_jiffies);
	}

	dbg1("kgex_open(): RETURN\n");
	return 0;
}

/**
 * kgex_stop - closes a network device. Usually called when
 * ifconfig down is executed.
 * @ndev: pointer to net_device structure
 *
 * Return: 0 on success.
 **/
int kgex_stop(struct net_device *ndev) {
	struct net_private *lp = NULL;

	dbg1("kgex_stop() ENTER\n");
	if (!ndev) {
		prwarn("kgex_stop(): ndev==NULL\n");
		return 0;
	}

	lp = netdev_priv(ndev);
	napi_disable(&lp->napi);
	if (lp->reviver_work) {
		cancel_delayed_work_sync(&(lp->reviver_work->work));
	}

	if (!netif_queue_stopped(ndev)) {
		netif_stop_queue(ndev);
	}

	kgex_dma_tx_stop_irq_disable(ndev);
	kgex_dma_rx_stop_irq_disable(ndev);

	if (lp->tx_rings_init) {
		kgex_tx_ring_destroy(ndev);
	}

	if (lp->rx_rings_init) {
		kgex_rx_ring_destroy(ndev);
	}

	if (lp->irqs) {
		free_irq(lp->tx_irq, ndev);
		free_irq(lp->rx_irq, ndev);
		lp->irqs = 0;
	}

	pr("%s down\n", ndev->name);
	return 0;
}

/**
 * kgex_get_stats - gets the statistics of a network device
 * @ndev: pointer to net_device structure
 *
 * Return: pointer to the network device statistics structure.
 **/
static struct net_device_stats *kgex_get_stats(struct net_device *ndev) {
	return &ndev->stats;
}

/**
 * kgex_change_mtu - sets a new MTU in a network device
 * @ndev: pointer to net_device structure
 * @new_mtu: new MTU
 *
 * Return: 0 on success.
 **/
static int kgex_change_mtu(struct net_device *ndev, int new_mtu) {
	int status = -1;
	struct net_private *lp = NULL;

	lp = netdev_priv(ndev);
	if (new_mtu == ndev->mtu) {
		pr("Requested MTU is equal to current MTU. Doing nothing...\n");
		return 0;
	}

	netif_tx_disable(ndev);
	wmb();
	smp_mb();

	kgex_dma_tx_stop_irq_disable(ndev);
	kgex_dma_rx_stop_irq_disable(ndev);

	kgex_tx_ring_destroy(ndev);
	kgex_rx_ring_destroy(ndev);

	kgex_set_mtu(ndev, new_mtu);

	status = kgex_tx_ring_setup(ndev);
	if (status != 0) {
		prerr("cannot setup tx ring, stopping reviver\n");
		lp->revive_jiffies = 0;
		return 0;
	}

	status = kgex_rx_ring_setup(ndev);
	if (status != 0) {
		prerr("cannot setup rx ring, stopping reviver\n");
		lp->revive_jiffies = 0;
		kgex_tx_ring_destroy(ndev);
		return 0;
	}

	kgex_dma_tx_start(ndev);
	kgex_dma_rx_start(ndev);

	netif_wake_queue(ndev);
	wmb();
	smp_mb();

	lp->reset_required = 1;

	pr("set MTU = %u Bytes => Max Frame Size = %u Bytes\n", ndev->mtu,
			lp->max_frm_size);

	return 0;
}

/**
 * kgex_set_mac_address - sets a new MAC address in a network device
 * @ndev: pointer to net_device structure
 * @address: new MAC address
 *
 * Return: 0 on success.
 **/
static int kgex_set_mac_address(struct net_device *ndev, void *address) {
	struct net_private *lp = NULL;
	struct sockaddr *addr = NULL;
	void *mac;

	BUG_ON(ndev == NULL);

	lp = netdev_priv(ndev);
	addr = address;

	mac = addr->sa_data;
	if (mac) {
		if (!is_valid_ether_addr(mac)) {
			prerr("Invalid MAC address. Doing nothing...\n");
			return 0;
		} else {
			memcpy(ndev->dev_addr, mac, ndev->addr_len);
		}
	}
	return 0;
}

/*========== End of Network Device Functions ====*/

/**
 * kgex_probe - initializes the driver and registers the assosiated
 * network device.
 * @op: pointer to platform_device structure
 *
 * Return: 0 on success.
 **/
static int kgex_probe(struct pci_dev *pdev, const struct pci_device_id *ent) {
	int status = 0;
	struct net_device *ndev = NULL;
	struct net_private *lp = NULL;

	dbg1("kgex_probe(): ENTER\n");
	pr("version: %s  build date: %s\n", DRIVER_VERSION, BUILD_DATE);

	ndev = alloc_etherdev(sizeof(struct net_private));
	if (!ndev) {
		prerr("ERROR: Could not allocate ethernet device\n");
		return -ENOMEM;
	}

	SET_NETDEV_DEV(ndev, &(pdev->dev));
	pci_set_drvdata(pdev, ndev);

	lp = netdev_priv(ndev);
	lp->pdev = pdev;
	lp->dev = &pdev->dev;
	lp->ndev = ndev;

	status = kgex_pci_probe(ndev);
	if (status) {
		return status;
	}

	lp->dma_regs = (void __iomem *)((uint64_t)(lp->regs) + 0x2000);

	if (tx_frag == 0) {
		pr("Support for TX ethernet frame fragmentation disabled.\n");
	} else {
		ndev->netdev_ops = &kgex_ops;
		ndev->ethtool_ops = &kgex_ethtool_ops;
		ndev->features = NETIF_F_SG | NETIF_F_FRAGLIST;
		pr("KALEAO: features: %llX\n", ndev->features);
		ndev->features &= ~0x800;
	}

#ifdef ENABLE_ETHTOOL
	eth10g_ethtool_init(ndev);
#endif
	//	ndev->hard_header_len = 14;
	ndev->addr_len = ETH_ALEN; // added recently (usable?)
	// ndev->flags &= ~IFF_MULTICAST;  /* clear multicast */
	// ndev->flags |= IFF_NOARP;
	ndev->flags |= IFF_BROADCAST;
	ndev->flags |= IFF_MULTICAST;

	////	ndev->features = NETIF_F_SG | NETIF_F_FRAGLIST;
	//	ndev->features |= /*NETIF_F_IP_CSUM |*/ /*NETIF_F_HW_CSUM*/
	//NETIF_F_ALL_CSUM;
	// ndev->features = NETIF_F_IP_CSUM |  NETIF_F_SG;

	//	ndev->features |= NETIF_F_RXCSUM;

	// Enable this for Scatter-Gather
	// ndev->features = NETIF_F_SG;

	// Setup user-changeable features
	ndev->hw_features |= NETIF_F_ALL_CSUM;
	// ndev->hw_features |= NETIF_F_SG | NETIF_F_FRAGLIST;

	if (rx_align != 0) {
		pr("Configured for aligned AXI DMA RX Channel\n");
	} else {
		pr("Configured for unaligned AXI DMA RX Channel\n");
	}

	if (lp->id == KGEX_NIC_ID_NPU) {
		strcpy(ndev->name, "eth%d");
		memcpy(ndev->dev_addr, exynos_default_mac_A_addr, ndev->addr_len);
		net_dev_tbl[0] = ndev;
		lp->configuration_space = (void __iomem *)((uint64_t)(lp->regs) + 0x2800);

		// pr("%s - reseting SFLR...\n", ndev->name);
		// kgex_sflr_reset(ndev);
	} else if (lp->id == KGEX_NIC_ID_SPU) {
		strcpy(ndev->name, "stg%d");
		memcpy(ndev->dev_addr, exynos_default_mac_C_addr, ndev->addr_len);
		net_dev_tbl[1] = ndev;
		lp->configuration_space = NULL;
	}

	netif_napi_add(ndev, &lp->napi, napi_poll, KGEX_NAPI_WEIGHT);

	register_netdev(ndev);
	// ndev->features &= ~NETIF_F_GSO;
	num_net_devs += 1;

	lp->irqs = 0;
	lp->tx_rings_init = 0;
	lp->rx_rings_init = 0;

	lp->coalesce_count_tx = DMA_DFT_TX_THRESHOLD;
	lp->timeout_tx = DMA_DFT_TX_WAITBOUND;
	lp->coalesce_count_rx = DMA_DFT_RX_THRESHOLD;
	lp->timeout_rx = DMA_DFT_RX_WAITBOUND;
	lp->nr_rx_irqs = 0;
	lp->tx_dma_errors = 0;
	lp->rx_dma_errors = 0;
	lp->tx_dma_desc_errors = 0;
	lp->rx_dma_desc_errors = 0;
	lp->nr_pci_retrains = 0;
	lp->nr_pci_gen_transitions = 0;
	// lp->error_print = 1;
	lp->error_print = 0;
	lp->pci_link_down = 0;
	lp->reset_required = 0;
	lp->nr_rc_resets_ok = 0;
	lp->tx_irq_unknown = 0;
	lp->rx_irq_unknown = 0;
	lp->dump_cnts = 0;
	lp->nr_external_resets = 0;
	lp->reviver_affinity = KGEX_REVIVER_DEFAULT_AFFINITY;
	lp->tx_dma_mapped_descs = 0;
	lp->rx_dma_mapped_descs = 0;

	lp->max_frm_size = KGE_MTU + KGE_HDR_SIZE + VLAN_HLEN + KGE_TRL_SIZE;

	lp->revive_jiffies = 0;
	lp->reviver_work = NULL;
	lp->reviver_work = (struct kgex_delayed_work *)kmalloc(
			sizeof(struct kgex_delayed_work), GFP_KERNEL);
	if (!lp->reviver_work) {
		prerr("cannot allocate work_struct\n");
		return -ENOMEM;
	}
	INIT_DELAYED_WORK(&(lp->reviver_work->work), kgex_dma_revive_func);
	lp->reviver_work->ndev = ndev;

	lp->xmit_work = (struct kgex_delayed_work *)kmalloc(
			sizeof(struct kgex_delayed_work), GFP_KERNEL);
	if (!lp->xmit_work) {
		prerr("cannot allocate work_struct\n");
		return -ENOMEM;
	}
	INIT_DELAYED_WORK(&(lp->xmit_work->work), kgex_xmit_work_func);
	lp->xmit_work->ndev = ndev;

#ifdef ENABLE_PROC
	if (kgex_proc == NULL) {
		kgex_proc_create();
	}
#endif

	status = kgex_rings_alloc(ndev);
	if (status != 0) {
		prerr("Cannot create %s\n", ndev->name);
		return status;
	}

	if (recv_skb_sanity != 0) {
		pr("%s initialized. RX frame sanity check enabled. \n", ndev->name);

	} else {
		pr("%s initialized.\n", ndev->name);
	}

	dbg1("kgex_probe(): RETURN\n");
	return 0;
}

/**
 * kgex_remove - stops the HW, frees all associated data
 * and removes the driver.
 * @op: pointer to platform_device structure
 *
 * Return: 0 on success.
 **/
static void kgex_remove(struct pci_dev *pdev) {
	struct net_device *ndev = NULL;
	struct net_private *lp = NULL;
	pr("kgex_remove(): ENTER\n");

	ndev = pci_get_drvdata(pdev);

	if (!ndev) {
		prierr("kgex_remove(): ndev==NULL\n");
		return;
	}

	lp = netdev_priv(ndev);

	if (lp->reviver_work) {
		cancel_delayed_work_sync(&(lp->reviver_work->work));
		kfree(lp->reviver_work);
		lp->reviver_work = NULL;
	}
	if (!netif_queue_stopped(ndev)) {
		netif_stop_queue(ndev);
	}

	kgex_dma_tx_stop_irq_disable(ndev);
	kgex_dma_rx_stop_irq_disable(ndev);

	if (lp->irqs) {
		free_irq(lp->tx_irq, ndev);
		free_irq(lp->rx_irq, ndev);
		lp->irqs = 0;
	}

	kgex_proc_remove();

	if (!ndev->dev.parent) {
		prwarn("ndev->dev.parent==NULL\n");
	}

	kgex_tx_ring_destroy(ndev);
	kgex_rx_ring_destroy(ndev);

	kgex_rings_dealloc(ndev);

	kgex_pci_remove(ndev);

	if (lp->pci_rc_regs) {
		iounmap(lp->pci_rc_regs);
		lp->pci_rc_regs = NULL;
	}

	if (lp->regs) {
		dbg1("kgex_remove(): iounmapping I/O space\n");
		iounmap(lp->regs);
		lp->dma_regs = lp->regs = NULL;
	}

	unregister_netdev(ndev);
	netif_napi_del(&lp->napi);
	free_netdev(ndev);
	dev_set_drvdata(&pdev->dev, NULL);
	pci_set_drvdata(pdev, NULL);

	pr("removed\n");
}

static void kgex_shutdown(struct pci_dev *pdev) { pr("Shuting down.\n"); }

/*===== End of Platform Driver Functions =========*/

MODULE_AUTHOR("John Velegrakis, Babis Aronis, Kaleao Ltd.");
MODULE_DESCRIPTION("Kmax 10Gbps Ethenet driver");
MODULE_LICENSE("GPL");
