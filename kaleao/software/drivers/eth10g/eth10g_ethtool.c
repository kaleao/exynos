///@cond DEV_DOC

#include "eth10g.h"

//   Ethtool Functions
int eth10g_ethtool_init(struct net_device *ndev);

//static int eth10g_get_settings(struct net_device *ndev, struct ethtool_cmd *ecmd);
static void eth10g_ethtool_get_ringparam(struct net_device *ndev, struct ethtool_ringparam *erp);
static int  eth10g_ethtool_get_regs_len(struct net_device *ndev);
static void eth10g_ethtool_get_regs(struct net_device *ndev, struct ethtool_regs *regs, void *ret);
static void eth10g_ethtool_get_drvinfo(struct net_device *ndev, struct ethtool_drvinfo *ed);
static void eth10g_ethtool_get_pauseparam(struct net_device *ndev, struct ethtool_pauseparam *epauseparm);
static int eth10g_ethtool_get_coalesce(struct net_device *ndev, struct ethtool_coalesce *ecoalesce);

/*========  Ethtool Functions ===================================*/
static int eth10g_ethtool_get_coalesce(struct net_device *ndev, struct ethtool_coalesce *ecoalesce)
{
	uint32_t reg = 0;
	struct net_private *lp = netdev_priv(ndev);
	reg = dma_read32(lp, DMA_RX_CR_OFFSET);
	ecoalesce->rx_max_coalesced_frames = (reg & DMA_COALESCE_MASK) >> DMA_COALESCE_SHIFT;
	reg = dma_read32(lp, DMA_TX_CR_OFFSET);
	ecoalesce->tx_max_coalesced_frames = (reg & DMA_COALESCE_MASK) >> DMA_COALESCE_SHIFT;
	
	return 0;
}

static void eth10g_ethtool_get_pauseparam(struct net_device *ndev, struct ethtool_pauseparam *epauseparm)
{
	uint32_t reg;
	struct net_private *lp = netdev_priv(ndev);
	epauseparm->autoneg  = 0;
	reg = nic_read32(lp, NIC_FCC_OFFSET);
	epauseparm->tx_pause = reg & NIC_FCC_FCTX_MASK;
	epauseparm->rx_pause = reg & NIC_FCC_FCRX_MASK;
}

static void eth10g_ethtool_get_drvinfo(struct net_device *ndev, struct ethtool_drvinfo *ed)
{
	strlcpy(ed->driver, DRIVER_NAME, sizeof(ed->driver));
	strlcpy(ed->version, DRIVER_VERSION, sizeof(ed->version));
	ed->regdump_len = sizeof(uint32_t) * ETH10G_REGS_N;
}

static void eth10g_ethtool_get_ringparam(struct net_device *ndev, struct ethtool_ringparam *erp)
{
	memset(erp, 0, sizeof(struct ethtool_ringparam));
	
	erp->tx_max_pending = TX_BD_NUM;
	erp->rx_max_pending = RX_BD_NUM;
	erp->tx_pending = 0;
	erp->rx_pending = 0;
}

static int eth10g_ethtool_get_regs_len(struct net_device *ndev)
{
	return sizeof(uint32_t) * ETH10G_REGS_N;	
}

static void eth10g_ethtool_get_regs(struct net_device *ndev,
									  struct ethtool_regs *regs, void *ret)
{
	uint32_t *data = (uint32_t *) ret;
	size_t len = sizeof(uint32_t) * ETH10G_REGS_N;
	struct net_private *lp = netdev_priv(ndev);

	regs->version = 0;
	regs->len = len;
	
	memset(data, 0, len);
	data[0] = nic_read32(lp, NIC_STAT_RXBLSW_OFFSET);
	data[1] = nic_read32(lp, NIC_STAT_RXBMSW_OFFSET);
	data[2] = nic_read32(lp, NIC_STAT_TXBLSW_OFFSET);
	data[3] = nic_read32(lp, NIC_STAT_TXBMSW_OFFSET);
	data[4] = nic_read32(lp, NIC_STAT_RXUFLSW_OFFSET);
	data[5] = nic_read32(lp, NIC_STAT_RXUFMSW_OFFSET);
	data[6] = nic_read32(lp, NIC_STAT_RXFFLSW_OFFSET);
	data[7] = nic_read32(lp, NIC_STAT_RXFFMSW_OFFSET);
	data[8] = nic_read32(lp, NIC_STAT_RX64LSW_OFFSET);
	data[9] = nic_read32(lp, NIC_STAT_RX64MSW_OFFSET);
	data[10] = nic_read32(lp, NIC_STAT_RX64127LSW_OFFSET);
	data[11] = nic_read32(lp, NIC_STAT_RX64127MSW_OFFSET);
	data[12] = nic_read32(lp, NIC_STAT_RX128255LSW_OFFSET);
	data[13] = nic_read32(lp, NIC_STAT_RX128255MSW_OFFSET);
	data[14] = nic_read32(lp, NIC_STAT_RX256511LSW_OFFSET);
	data[15] = nic_read32(lp, NIC_STAT_RX256511MSW_OFFSET);
	data[16] = nic_read32(lp, NIC_STAT_RX5121023LSW_OFFSET);
	data[17] = nic_read32(lp, NIC_STAT_RX5121023MSW_OFFSET);
	data[18] = nic_read32(lp, NIC_STAT_RX1024LSW_OFFSET);
	data[19] = nic_read32(lp, NIC_STAT_RX1024MSW_OFFSET);
	data[20] = nic_read32(lp, NIC_STAT_RXOSLSW_OFFSET);
	data[21] = nic_read32(lp, NIC_STAT_RXOSMSW_OFFSET);
	data[22] = nic_read32(lp, NIC_STAT_TX64LSW_OFFSET);
	data[23] = nic_read32(lp, NIC_STAT_TX64MSW_OFFSET);
	data[24] = nic_read32(lp, NIC_STAT_TX64127LSW_OFFSET);
	data[25] = nic_read32(lp, NIC_STAT_TX64127MSW_OFFSET);
	data[26] = nic_read32(lp, NIC_STAT_TX128255LSW_OFFSET);
	data[27] = nic_read32(lp, NIC_STAT_TX128255MSW_OFFSET);
	data[28] = nic_read32(lp, NIC_STAT_TX256511LSW_OFFSET);
	data[29] = nic_read32(lp, NIC_STAT_TX256511MSW_OFFSET);
	data[30] = nic_read32(lp, NIC_STAT_TX5121023LSW_OFFSET);
	data[31] = nic_read32(lp, NIC_STAT_TX5121023MSW_OFFSET);
	data[32] = nic_read32(lp, NIC_STAT_TX1024LSW_OFFSET);
	data[33] = nic_read32(lp, NIC_STAT_TX1024MSW_OFFSET);	
	data[34] = nic_read32(lp, NIC_STAT_TXOSLSW_OFFSET);
	data[35] = nic_read32(lp, NIC_STAT_TXOSMSW_OFFSET);
	data[36] = nic_read32(lp, NIC_STAT_RXOKFLSW_OFFSET);
	data[37] = nic_read32(lp, NIC_STAT_RXOKFMSW_OFFSET); 
	data[38] = nic_read32(lp, NIC_STAT_RXERRLSW_OFFSET);
	data[39] = nic_read32(lp, NIC_STAT_RXERRMSW_OFFSET); 
	data[40] = nic_read32(lp, NIC_STAT_RXBOKFLSW_OFFSET);
	data[41] = nic_read32(lp, NIC_STAT_RXBOKFMSW_OFFSET); 
	data[42] = nic_read32(lp, NIC_STAT_RXMOKFLSW_OFFSET); 
	data[43] = nic_read32(lp, NIC_STAT_RXMOKFMSW_OFFSET); 
	data[44] = nic_read32(lp, NIC_STAT_RXCOKFLSW_OFFSET); 
	data[45] = nic_read32(lp, NIC_STAT_RXCOKFMSW_OFFSET); 
	data[46] = nic_read32(lp, NIC_STAT_RXCLOFLSW_OFFSET); 
	data[47] = nic_read32(lp, NIC_STAT_RXCLOFMSW_OFFSET); 
	data[48] = nic_read32(lp, NIC_STAT_RXVFLSW_OFFSET); 
	data[49] = nic_read32(lp, NIC_STAT_RXVFMSW_OFFSET); 
	data[50] = nic_read32(lp, NIC_STAT_RXPFLSW_OFFSET); 
	data[51] = nic_read32(lp, NIC_STAT_RXPFMSW_OFFSET); 
	data[52] = nic_read32(lp, NIC_STAT_RXCFOPLSW_OFFSET);
	data[53] = nic_read32(lp, NIC_STAT_RXCFOPMSW_OFFSET); 
	data[54] = nic_read32(lp, NIC_STAT_TXOKFLSW_OFFSET); 
	data[55] = nic_read32(lp, NIC_STAT_TXOKFMSW_OFFSET); 
	data[56] = nic_read32(lp, NIC_STAT_TXBOKFLSW_OFFSET);
	data[57] = nic_read32(lp, NIC_STAT_TXBOKFMSW_OFFSET);
	data[58] = nic_read32(lp, NIC_STAT_TXMOKFLSW_OFFSET);
	data[59] = nic_read32(lp, NIC_STAT_TXMOKFMSW_OFFSET); 
	data[60] = nic_read32(lp, NIC_STAT_TXMUERRLSW_OFFSET);
	data[61] = nic_read32(lp, NIC_STAT_TXMUERRMSW_OFFSET);
	data[62] = nic_read32(lp, NIC_STAT_TXCOKFLSW_OFFSET);
	data[63] = nic_read32(lp, NIC_STAT_TXCOKFMSW_OFFSET); 
	data[64] = nic_read32(lp, NIC_STAT_TXVFLSW_OFFSET);
	data[65] = nic_read32(lp, NIC_STAT_TXVFMSW_OFFSET); 
	data[66] = nic_read32(lp, NIC_STAT_TXPFLSW_OFFSET); 
	data[67] = nic_read32(lp, NIC_STAT_TXPFMSW_OFFSET); 

	// NIC MAC Configuration Registers
	data[68] = nic_read32(lp, NIC_RCW0_OFFSET);
	data[69] = nic_read32(lp, NIC_RCW1_OFFSET);
	data[70] = nic_read32(lp, NIC_TC_OFFSET);
	data[71] = nic_read32(lp, NIC_FCC_OFFSET);
	data[72] = nic_read32(lp, NIC_RSC_OFFSET);
	data[73] = nic_read32(lp, NIC_RMCW_OFFSET);
	data[74] = nic_read32(lp, NIC_TMCW_OFFSET);
	data[75] = nic_read32(lp, NIC_V_OFFSET);
	data[76] = nic_read32(lp, NIC_C_OFFSET);

	// NIC MDIO Management Registers
	data[77] = nic_read32(lp, NIC_MDIO_CW0_OFFSET);
	data[78] = nic_read32(lp, NIC_MDIO_CW1_OFFSET);
	data[79] = nic_read32(lp, NIC_MDIO_TX_OFFSET);
	data[80] = nic_read32(lp, NIC_MDIO_RX_OFFSET);
}

static struct ethtool_ops eth10g_ethtool_ops = {
	.get_drvinfo    = eth10g_ethtool_get_drvinfo,
	.get_link       = ethtool_op_get_link,      /*ethtool default*/
	.get_ringparam  = eth10g_ethtool_get_ringparam,	
	.get_regs_len	= eth10g_ethtool_get_regs_len,	
	.get_regs       = eth10g_ethtool_get_regs,
	.get_pauseparam = eth10g_ethtool_get_pauseparam,
	.get_coalesce   = eth10g_ethtool_get_coalesce,
};

int eth10g_ethtool_init(struct net_device *ndev)
{
	ndev->ethtool_ops = &eth10g_ethtool_ops;
	return 0;
}


// /*======== End of Ethtool Functions =============================*/

///@endcond