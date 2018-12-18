#ifndef KGEX_DEBUG_H
#define KGEX_DEBUG_H

#include "kgex-hw.h"
#include "kgex.h"

static inline int kgex_dma_check_skb_sanity(struct sk_buff *skb, uint32_t len) {
	uint32_t i = 0;

	if (!skb) {
		prierr("skb==NULL\n");
		return 1;
	}
	if (!skb->data) {
		prierr("skb->data==NULL\n");
		return 1;
	}

	for (i = 0; i < len; i++) {
		if (skb->data[i] != 0xAA && skb->data[i] != 0xBB && skb->data[i] != 0xFF &&
				skb->data[i] != 0x00) {
			return 0;
		}
	}
	return 1;
}

/**
 * print_skb - prints the Ethernet Frame described in skb->data in
 * Hexadecimal format
 * @skb: pointer to socket buffer (ethernet frame)
 * @len:  number of bytes to print
 *
 * Return: 0
 **/
static inline int print_skb(struct sk_buff *skb, unsigned int len) {
#define PBUFF_SIZE 3000
	unsigned int i = 0;
	unsigned int index = 0;
	char *pbuff = NULL;
	int bytes = 0;

	if (len < 16) {
		return 0;
	}
	if (skb == NULL) {
		pr("===== skb =====\n");
		pr("     NULL      \n");
		pr("===============\n");
		return 0;
	}
	if (skb->data == NULL) {
		pr("===== skb =====\n");
		pr("   data NULL   \n");
		pr("===============\n");
	}

	pbuff = (char *)kmalloc(PBUFF_SIZE * sizeof(char), GFP_KERNEL);
	for (i = 0; i < PBUFF_SIZE; i++) {
		pbuff[i] = ' ';
	}

	pr("===== skb =====\n");
	for (i = 0, index = 0; i < len; i++) {
		bytes = sprintf(&pbuff[index], "%.2X ", (uint32_t)(skb->data[i]));
		index += bytes;

		if ((i + 1) % 8 == 0 && i != 0) {
			if ((i + 1) % 16 == 0) {
				bytes = sprintf(&pbuff[index], "%c", '\n');
				index += bytes;
			} else {
				bytes = sprintf(&pbuff[index], "%c%c%c", ' ', ' ', ' ');
				index += bytes;
			}
		}
	}
	sprintf(&pbuff[index], "%c", '\0');
	pr("%s\n", pbuff);
	pr("===============\n");
	kfree(pbuff);
	return 0;
}

static inline void kgex_dump_cnts(struct net_device *ndev) {
	struct net_private *lp = NULL;
	uint32_t rd_pend, rd_req, rd_ack, rd_nack;
	uint32_t wr_pend, wr_req, wr_ack, wr_nack;

	lp = netdev_priv(ndev);
	if (!lp) {
		prierr("cannot get private data for network device\n");
		return;
	}

	rd_pend = dma_read32_prot(lp, AXI_RD_PEND_RDS_OFF);
	rd_req = dma_read32_prot(lp, AXI_RD_REQS_OFF);
	rd_ack = dma_read32_prot(lp, AXI_RD_ACKS_OFF);
	rd_nack = dma_read32_prot(lp, AXI_RD_NACKS_OFF);

	wr_pend = dma_read32_prot(lp, AXI_WR_PEND_RDS_OFF);
	wr_req = dma_read32_prot(lp, AXI_WR_REQS_OFF);
	wr_ack = dma_read32_prot(lp, AXI_WR_ACKS_OFF);
	wr_nack = dma_read32_prot(lp, AXI_WR_NACKS_OFF);

	if (lp->error_print) {
		pr("RD Pend: %u\n", rd_pend);
		pr("RD Req: %u\n", rd_req);
		pr("RD Ack: %u\n", rd_ack);
		pr("RD NAck: %u\n", rd_nack);
		pr("WR Pend: %u\n", wr_pend);
		pr("WR Req: %u\n", wr_req);
		pr("WR Ack: %u\n", wr_ack);
		pr("WR NAck: %u\n", wr_nack);
	}
}

static inline struct axi_dma_regs *kgex_get_dma_regs(struct net_device *ndev) {
	struct net_private *lp = NULL;
	struct axi_dma_regs *regs = NULL;

	lp = netdev_priv(ndev);
	if (!lp) {
		prierr("cannot get private data fro network device\n");
		return NULL;
	}

	regs =
		(struct axi_dma_regs *)kmalloc(sizeof(struct axi_dma_regs), GFP_KERNEL);
	if (!regs) {
		prwarn("cannot allocate mem for axi_dma_regs\n");
		return NULL;
	}

	memset(regs, 0, sizeof(struct axi_dma_regs));

	regs->tx_cr = dma_read32_prot(lp, DMA_TX_CR_OFFSET);
	regs->tx_sr = dma_read32_prot(lp, DMA_TX_SR_OFFSET);
	regs->tx_cdesc = dma_read32_prot(lp, DMA_TX_CDESC_OFFSET);
	regs->tx_tdesc = dma_read32_prot(lp, DMA_TX_TDESC_OFFSET);
	regs->rx_cr = dma_read32_prot(lp, DMA_RX_CR_OFFSET);
	regs->rx_sr = dma_read32_prot(lp, DMA_RX_SR_OFFSET);
	regs->rx_cdesc = dma_read32_prot(lp, DMA_RX_CDESC_OFFSET);
	regs->rx_tdesc = dma_read32_prot(lp, DMA_RX_TDESC_OFFSET);

	return regs;
}

#endif /* KGEX_DEBUG_H */
