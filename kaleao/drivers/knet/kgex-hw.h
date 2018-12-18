#ifndef KGEX_HW_H
#define KGEX_HW_H

#include "kgex.h"

#define hw_write32(base, reg, val)                                             \
	__raw_writel((val), (void __iomem *)((uint64_t)(base) + (uint64_t)(reg)))

#define hw_write64(base, reg, val)                                             \
	__raw_writeq((val), (void __iomem *)((uint64_t)(base) + (uint64_t)(reg)))

#define ReadReg32(BASE_ADDRESS, RegOffset)                                     \
	*(unsigned int *)(BASE_ADDRESS + RegOffset);

#define ReadReg64(BASE_ADDRESS, RegOffset)                                     \
	*(uint64_t *)(BASE_ADDRESS + RegOffset);

#ifdef AXI_DMA_64

static inline void dma_write32(struct net_private *lp, off_t offset,
		uint32_t val) {
	hw_write32(lp->dma_regs, offset, val);
}

static inline uint32_t dma_read32(struct net_private *lp, off_t offset) {
	return ReadReg32(lp->dma_regs, offset);
}
#else

static inline struct pci_dev *get_pci_rc(struct pci_dev *pdev) {
	return pdev->bus->self;
}

static inline void gen_io_wr32(void __iomem *base, uint32_t reg, uint32_t val) {
	rmb();
	wmb();
	hw_write32(base, reg, val);
	wmb();
}

static inline void gen_io_wr32_prot(struct net_private *lp, void __iomem *base,
		uint32_t reg, uint32_t val) {

	if (lp->pdev != NULL)
		if (!exynos_pcie_is_linkup(lp->pdev->bus->self)) {
			return;
		}
	gen_io_wr32(base, reg, val);
}

static inline uint32_t gen_io_rd32(void __iomem *base, uint32_t reg) {
	uint32_t val = 0;
	rmb();
	// val = ReadReg32(base, reg);
	val = __raw_readl(base + reg);
	rmb();
	return val;
}

static inline uint32_t gen_io_rd32_prot(struct net_private *lp,
		void __iomem *base, uint32_t reg) {
	if (lp->pdev != NULL)
		if (!exynos_pcie_is_linkup(lp->pdev->bus->self)) {
			return 0xFFFFFFFF;
		}
	return gen_io_rd32(base, reg);
}

static inline void dma_write32(struct net_private *lp, uint32_t offset,
		uint32_t val) {
	wmb();
	hw_write32(lp->dma_regs, offset, val);
	wmb();
}

static inline void dma_write32_prot(struct net_private *lp, off_t offset,
		uint32_t val) {
	if (lp->pdev != NULL)
		if (!exynos_pcie_is_linkup(lp->pdev->bus->self)) {
			return;
		}
	dma_write32(lp, offset, val);
}

static inline uint32_t dma_read32(struct net_private *lp, uint32_t offset) {
	wmb();
	return ReadReg32(lp->dma_regs, offset);
}

static inline uint32_t dma_read32_prot(struct net_private *lp, off_t offset) {
	if (lp->pdev != NULL)
		if (!exynos_pcie_is_linkup(lp->pdev->bus->self)) {
			return 0xFFFFFFFF;
		}
	return dma_read32(lp, offset);
}

#endif

/* SFLR (Small Frame Latency Reduce) registers*/
#define SFLR_CTRL_OFF 0x600
#define SFLR_FRAME_SIZE_OFF 0x604
#define SFLR_IRQ_DELAY_OFF 0x608

/* DMA Functions */
static inline void kgex_dma_tx_irqs_disable(struct net_device *ndev) {
	struct net_private *lp = NULL;
	uint32_t cr = 0;
	lp = netdev_priv(ndev);
	cr = dma_read32_prot(lp, DMA_TX_CR_OFFSET);
	cr &= (~DMA_IRQ_ALL_MASK);
	dma_write32_prot(lp, DMA_TX_CR_OFFSET, cr);
}

static inline void kgex_dma_rx_irqs_disable(struct net_device *ndev) {
	struct net_private *lp = NULL;
	uint32_t cr = 0;
	lp = netdev_priv(ndev);
	cr = dma_read32_prot(lp, DMA_RX_CR_OFFSET);
	cr &= (~DMA_IRQ_ALL_MASK);
	dma_write32_prot(lp, DMA_RX_CR_OFFSET, cr);
}

static inline void kgex_dma_irqs_disable(struct net_device *ndev) {
	kgex_dma_tx_irqs_disable(ndev);
	kgex_dma_rx_irqs_disable(ndev);
}

static inline void kgex_dma_tx_stop_irq_disable(struct net_device *ndev) {
	uint32_t cr = 0;
	struct net_private *lp = NULL;
	lp = netdev_priv(ndev);

	lp->tx_irq_enabled = 0;
	// cr = dma_read32_prot(lp, DMA_TX_CR_OFFSET);
	cr = cr & (~DMA_CR_RUNSTOP_MASK);
	cr = cr & (~DMA_IRQ_ALL_MASK);
	dma_write32_prot(lp, DMA_TX_CR_OFFSET, cr);
}

static inline void kgex_dma_rx_stop_irq_disable(struct net_device *ndev) {
	uint32_t cr = 0;
	struct net_private *lp = NULL;
	lp = netdev_priv(ndev);

	lp->rx_irq_enabled = 0;
	// cr = dma_read32_prot(lp, DMA_RX_CR_OFFSET);
	cr = cr & (~DMA_CR_RUNSTOP_MASK);
	cr = cr & (~DMA_IRQ_ALL_MASK);

	dma_write32_prot(lp, DMA_RX_CR_OFFSET, cr);
}

/**
 * kgex_set_tx_coalesce - sets a new interrupt coalescing
 * value in the AXI DMA TX channel
 * @ndev: network device - pointer to net_device structure
 * @timeout:  the new timeout value
 **/
static inline void kgex_dma_set_tx_coalesce(struct net_device *ndev,
		uint32_t coalesce) {
	uint32_t cr = 0;
	struct net_private *lp = NULL;

	lp = netdev_priv(ndev);

	cr = dma_read32_prot(lp, DMA_TX_CR_OFFSET);
	cr = (((cr & ~DMA_COALESCE_MASK)) | ((coalesce) << DMA_COALESCE_SHIFT));
	cr = (((cr & ~DMA_DELAY_MASK)) | (lp->timeout_tx << DMA_DELAY_SHIFT));
	cr |= DMA_IRQ_ALL_MASK;
	dma_write32_prot(lp, DMA_TX_CR_OFFSET, cr | DMA_CR_RUNSTOP_MASK);
}

/**
 * kgex_set_tx_timeout - sets a new timeout value in the AXI DMA
 * TX channel timer.
 * @ndev: network device - pointer to net_device structure
 * @timeout:  the new timeout value
 **/
static inline void kgex_dma_set_tx_timeout(struct net_device *ndev,
		uint32_t timeout) {
	uint32_t cr = 0;
	struct net_private *lp = NULL;

	lp = netdev_priv(ndev);

	cr = dma_read32_prot(lp, DMA_TX_CR_OFFSET);
	cr = (((cr & ~DMA_COALESCE_MASK)) |
			((lp->coalesce_count_tx) << DMA_COALESCE_SHIFT));
	cr = (((cr & ~DMA_DELAY_MASK)) | (timeout << DMA_DELAY_SHIFT));
	cr |= DMA_IRQ_ALL_MASK;
	dma_write32_prot(lp, DMA_TX_CR_OFFSET, cr | DMA_CR_RUNSTOP_MASK);
}

/**
 * kgex_set_rx_coalesce - sets a new interrupt coalescing
 * value in the AXI DMA RX channel
 * @ndev: network device - pointer to net_device structure
 * @timeout:  the new timeout value
 **/
static inline void kgex_dma_set_rx_coalesce(struct net_device *ndev,
		uint32_t coalesce) {
	uint32_t cr = 0;
	struct net_private *lp = NULL;

	lp = netdev_priv(ndev);

	cr = dma_read32_prot(lp, DMA_RX_CR_OFFSET);
	cr = (((cr & ~DMA_COALESCE_MASK)) | ((coalesce) << DMA_COALESCE_SHIFT));
	cr = (((cr & ~DMA_DELAY_MASK)) | (lp->timeout_rx << DMA_DELAY_SHIFT));
	cr |= DMA_IRQ_ALL_MASK;
	dma_write32_prot(lp, DMA_RX_CR_OFFSET, cr | DMA_CR_RUNSTOP_MASK);
}

/**
 * kgex_set_rx_timeout - sets a new timeout value in the AXI DMA
 * RX channel timer.
 * @ndev: network device - pointer to net_device structure
 * @timeout:  the new timeout value
 **/
static inline void kgex_dma_set_rx_timeout(struct net_device *ndev,
		uint32_t timeout) {
	uint32_t cr = 0;
	struct net_private *lp = NULL;

	lp = netdev_priv(ndev);

	cr = dma_read32_prot(lp, DMA_RX_CR_OFFSET);
	cr = (((cr & ~DMA_COALESCE_MASK)) |
			((lp->coalesce_count_rx) << DMA_COALESCE_SHIFT));
	cr = (((cr & ~DMA_DELAY_MASK)) | (timeout << DMA_DELAY_SHIFT));
	cr |= DMA_IRQ_ALL_MASK;
	dma_write32_prot(lp, DMA_RX_CR_OFFSET, cr | DMA_CR_RUNSTOP_MASK);
}

/**
 * kgex_get_tx_coalesce - queries the HW and gets the AXI DMA
 * interrupt coalescing value for the TX channel
 * @ndev: network device - pointer to net_device structure
 *
 * Return : integer = interrupt coalescing value
 **/
static inline int kgex_dma_get_tx_coalesce(struct net_device *ndev) {
	uint32_t cr = 0;
	uint32_t coalesce = 0;
	struct net_private *lp = NULL;

	lp = netdev_priv(ndev);
	cr = dma_read32_prot(lp, DMA_TX_CR_OFFSET);
	coalesce = (cr & DMA_COALESCE_MASK) >> DMA_COALESCE_SHIFT;
	return coalesce;
}

/**
 * kgex_get_tx_timeout - queries the HW and gets the AXI DMA timer
 * timeout value for the TX channel.
 * @ndev: network device - pointer to net_device structure
 *
 * Return : integer = timeout value
 **/
static inline int kgex_dma_get_tx_timeout(struct net_device *ndev) {
	uint32_t cr = 0;
	uint32_t timeout = 0;
	struct net_private *lp = NULL;

	lp = netdev_priv(ndev);
	cr = dma_read32_prot(lp, DMA_TX_CR_OFFSET);
	timeout = (cr & DMA_DELAY_MASK) >> DMA_DELAY_SHIFT;
	return timeout;
}

/**
 * kgex_get_rx_coalesce - queries the HW and gets the AXI DMA
 * interrupt coalescing value for the RX channel
 * @ndev: network device - pointer to net_device structure
 *
 * Return : integer = interrupt coalescing value
 **/
static inline int kgex_dma_get_rx_coalesce(struct net_device *ndev) {
	uint32_t cr = 0;
	uint32_t coalesce = 0;
	struct net_private *lp = NULL;

	lp = netdev_priv(ndev);
	cr = dma_read32_prot(lp, DMA_RX_CR_OFFSET);
	coalesce = (cr & DMA_COALESCE_MASK) >> DMA_COALESCE_SHIFT;
	return coalesce;
}

/**
 * kgex_get_rx_timeout - queries the HW and gets the AXI DMA timer
 * timeout value for the RX channel.
 * @ndev: network device - pointer to net_device structure
 *
 * Return : integer = timeout value
 **/
static inline int kgex_dma_get_rx_timeout(struct net_device *ndev) {
	uint32_t cr = 0;
	uint32_t timeout = 0;
	struct net_private *lp = NULL;

	lp = netdev_priv(ndev);
	cr = dma_read32_prot(lp, DMA_RX_CR_OFFSET);
	timeout = (cr & DMA_DELAY_MASK) >> DMA_DELAY_SHIFT;
	return timeout;
}

static inline void kgex_dma_tx_start(struct net_device *ndev) {
	struct net_private *lp = NULL;
	uint32_t cr = 0;

	if (ndev == NULL) {
		prerr("ndev == NULL\n");
		return;
	}
	lp = netdev_priv(ndev);
	if (lp == NULL) {
		prerr("lp == NULL\n");
		return;
	}

	lp->tx_irq_enabled = 1;
	/*Init TX part*/
	dma_write32_prot(lp, DMA_TX_CDESC_OFFSET, LO32(lp->tx_bd_phys));

	// TX IRQs coalescing
	// cr = dma_read32_prot(lp, DMA_TX_CR_OFFSET);
	cr = (((cr & ~DMA_COALESCE_MASK)) |
			((lp->coalesce_count_tx) << DMA_COALESCE_SHIFT));
	cr = (((cr & ~DMA_DELAY_MASK)) | (lp->timeout_tx << DMA_DELAY_SHIFT));
	cr |= DMA_IRQ_ALL_MASK;
	dma_write32_prot(lp, DMA_TX_CR_OFFSET, cr | DMA_CR_RUNSTOP_MASK);
}

static inline void kgex_dma_rx_start(struct net_device *ndev) {

	struct net_private *lp = NULL;
	uint32_t cr = 0;

	if (ndev == NULL) {
		prerr("ndev == NULL\n");
		return;
	}
	lp = netdev_priv(ndev);
	if (lp == NULL) {
		prerr("lp == NULL\n");
		return;
	}

	lp->rx_irq_enabled = 1;
	/*Init RX part*/
	dma_write32_prot(lp, DMA_RX_CDESC_OFFSET, LO32(lp->rx_bd_phys));

	// RX IRQs coalescing
	// cr = dma_read32_prot(lp, DMA_RX_CR_OFFSET);
	cr = ((cr & ~DMA_COALESCE_MASK) |
			((lp->coalesce_count_rx) << DMA_COALESCE_SHIFT));
	cr = (((cr & ~DMA_DELAY_MASK)) | (lp->timeout_rx << DMA_DELAY_SHIFT));
	cr |= DMA_IRQ_ALL_MASK;

	dma_write32_prot(lp, DMA_RX_CR_OFFSET, cr | DMA_CR_RUNSTOP_MASK);
	// TODO: Change RX_BD_NUM with lp->rx_bd_num when introduced!!
	dma_write32_prot(lp, DMA_RX_TDESC_OFFSET,
			LO32(lp->rx_bd_phys) +
			(sizeof(struct dma_bd) * (RX_BD_NUM - 1)));
}

static inline int kgex_dma_status_reg_decode(struct net_device *ndev,
		uint32_t status) {
	struct net_private *lp = NULL;

	lp = netdev_priv(ndev);

	if (status & DMA_STS_INT_ERR) {
		if (lp->error_print) {
			prerr("%s DMA Internal Error = 0x%08X. CPU core = %u\n", ndev->name,
					status, smp_processor_id());
		}
		return -1;
	}
	if (status & DMA_STS_SLV_ERR) {
		if (lp->error_print) {
			prerr("%s DMA Slave Error = 0x%08X. CPU core = %u\n", ndev->name, status,
					smp_processor_id());
		}
		return -1;
	}
	if (status & DMA_STS_DEC_ERR) {
		if (lp->error_print) {
			prerr("%s DMA Decode Error = 0x%08X. CPU core = %u\n", ndev->name, status,
					smp_processor_id());
		}
		return -1;
	}
	if (status & DMA_STS_SG_INT_ERR) {
		if (lp->error_print) {
			prerr("%s DMA SG Internal Error = 0x%08X. CPU core = %u\n", ndev->name,
					status, smp_processor_id());
		}
		return -1;
	}
	if (status & DMA_STS_SG_SLV_ERR) {
		if (lp->error_print) {
			prerr("%s DMA SG Slave Error = 0x%08X. CPU core = %u\n", ndev->name,
					status, smp_processor_id());
		}
		return -1;
	}
	if (status & DMA_STS_SG_DEC_ERR) {
		if (lp->error_print) {
			prerr("%s DMA SG Decode Error = 0x%08X. CPU core = %u\n", ndev->name,
					status, smp_processor_id());
		}
		return -1;
	}
	if (status & DMA_STS_HALTED) {
		if (lp->error_print) {
			prerr("%s DMA halted status: 0x%08X CPU core = %u\n", ndev->name, status,
					smp_processor_id());
		}
		return -1;
	}

	return 0;
}

static inline int kgex_dma_status_bd_decode(uint32_t status) {
	if (status & (DMA_BD_STS_DEC_ERR_MASK | DMA_BD_STS_INT_ERR_MASK |
				DMA_BD_STS_SLV_ERR_MASK)) {
		return -1;
	}

	return 0;
}

/* SFLR core Functions */
static inline void kgex_sflr_reset(struct net_device *ndev) {
	struct net_private *lp = NULL;

	lp = netdev_priv(ndev);
	if (!lp) {
		prierr("cannot get private data for network device\n");
		return;
	}

	dma_write32_prot(lp, SFLR_CTRL_OFF, 0x01);
}

static inline uint32_t kgex_sflr_is_enabled(struct net_device *ndev) {
	struct net_private *lp = NULL;
	uint32_t reg = 0x0;

	lp = netdev_priv(ndev);
	if (!lp) {
		prierr("cannot get private data for network device\n");
		return 0;
	}

	reg = dma_read32_prot(lp, SFLR_CTRL_OFF);

	return reg;
}

static inline void kgex_sflr_enable(struct net_device *ndev) {
	struct net_private *lp = NULL;

	lp = netdev_priv(ndev);
	if (!lp) {
		prierr("cannot get private data for network device\n");
		return;
	}

	dma_write32_prot(lp, SFLR_CTRL_OFF, 0x2);
}

static inline void kgex_sflr_disable(struct net_device *ndev) {
	struct net_private *lp = NULL;

	lp = netdev_priv(ndev);
	if (!lp) {
		prierr("cannot get private data for network device\n");
		return;
	}

	dma_write32_prot(lp, SFLR_CTRL_OFF, 0x0);
}

static inline uint32_t kgex_sflr_get_frame_size(struct net_device *ndev) {
	struct net_private *lp = NULL;
	uint32_t reg = 0x0;

	lp = netdev_priv(ndev);
	if (!lp) {
		prierr("cannot get private data for network device\n");
		return 0;
	}

	reg = dma_read32_prot(lp, SFLR_FRAME_SIZE_OFF);
	return (reg * 8);
}

static inline void kgex_sflr_set_frame_size(struct net_device *ndev,
		uint32_t val) {
	struct net_private *lp = NULL;

	lp = netdev_priv(ndev);
	if (!lp) {
		prierr("cannot get private data for network device\n");
		return;
	}

	dma_write32_prot(lp, SFLR_FRAME_SIZE_OFF, (val / 8));
}

static inline int32_t kgex_sflr_get_irq_delay(struct net_device *ndev) {
	struct net_private *lp = NULL;
	uint32_t reg = 0x0;

	lp = netdev_priv(ndev);
	if (!lp) {
		prierr("cannot get private data for network device\n");
		return 0;
	}

	reg = dma_read32_prot(lp, SFLR_IRQ_DELAY_OFF);
	return reg;
}

static inline void kgex_sflr_set_irq_delay(struct net_device *ndev,
		uint32_t val) {
	struct net_private *lp = NULL;

	lp = netdev_priv(ndev);
	if (!lp) {
		prierr("cannot get private data for network device\n");
		return;
	}

	dma_write32_prot(lp, SFLR_IRQ_DELAY_OFF, val);
}

#endif /* KGEX_HW_H */
