/**
 * @file eth10g.h  10g Ethernet NIC structures, addresses and register offsets
 * @author John Velegrakis
 * 
 * @page eth10g
 */

#ifndef ETH10G_H
	#define ETH10G_H
	
#include <linux/module.h>  
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/delay.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>  
#include <linux/skbuff.h>
#include <linux/spinlock.h>
#include <linux/phy.h>
#include <linux/mii.h>
#include <linux/ethtool.h>
#include <linux/platform_device.h>

#include <linux/of_mdio.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/if_vlan.h>

#include <linux/spinlock.h>
#include <linux/interrupt.h>

#include <linux/proc_fs.h>
#include <linux/seq_file.h>	


#define DRIVER "eth10g"
#define DRIVER_NAME		"eth10g"
#define DRIVER_DESCRIPTION	"Kmax Exynos 10 Gbps Ethernet Driver"
#define DRIVER_VERSION		"1.00"


#define pr(fmt, ...) {pr_info(DRIVER_NAME ": " fmt, ##__VA_ARGS__);}
#define prerr(fmt, ...) {pr_err(DRIVER_NAME ": ERROR: " fmt, ##__VA_ARGS__);}

#define TX_CHECK_UNALIGNED
#define RX_CHECK_UNALIGNED

//#define ETH10G_DEBUG_LEVEL_1

#ifdef ETH10G_DEBUG_LEVEL_3
	#ifndef ETH10G_DEBUG_LEVEL_2
		#define ETH10G_DEBUG_LEVEL_2
	#endif
	#define dbg3(fmt, ...) {pr_info(fmt, ##__VA_ARGS__);}
#else
	#define dbg3(fmt, ...) {;}
#endif


#ifdef ETH10G_DEBUG_LEVEL_2
	#ifndef ETH10G_DEBUG_LEVEL_1
		#define ETH10G_DEBUG_LEVEL_1
	#endif
	#define dbg2(fmt, ...) {pr_info(fmt, ##__VA_ARGS__);}
#else
	#define dbg2(fmt, ...) {;}
#endif

#ifdef ETH10G_DEBUG_LEVEL_1
	#define dbg1(fmt, ...) {pr_info(DRIVER_NAME ": " fmt, ##__VA_ARGS__);}
#else
	#define dbg1(fmt, ...) {;}
#endif 


#define DRAM_A_START		0x80000000
#define DRAM_A_END		0xfeffffff
#define DRAM_A_LEN		(0xfeffffff - 0x80000000)
#define DRAM_A_START_TRANS	0xFE00000000

#define DRAM_B_START		0x880000000
#define DRAM_B_END		0x9ffffffff
#define DRAM_B_LEN		(0x9ffffffff - 0x880000000)
#define DRAM_B_START_TRANS	0xFE80000000

#define ETH10G_REGS_N  81


#define ETH10G_F_CSUM	1

#define ETH10G_HDR_SIZE			14 /* Size of Ethernet header */
#define ETH10G_TRL_SIZE			 4 /* Size of Ethernet trailer (FCS) */
#define ETH10G_MTU			      1500 /* Max MTU of an Ethernet frame */
#define ETH10G_JUMBO_MTU		      9000 /* Max MTU of a jumbo Eth. frame */

#define ETH10G_MAX_FRAME_SIZE	 (ETH10G_MTU + ETH10G_HDR_SIZE + ETH10G_TRL_SIZE)
#define ETH10G_MAX_VLAN_FRAME_SIZE  (ETH10G_MTU + VLAN_ETH_HLEN + ETH10G_TRL_SIZE)
#define ETH10G_MAX_JUMBO_FRAME_SIZE (ETH10G_JUMBO_MTU + ETH10G_HDR_SIZE + ETH10G_TRL_SIZE)
#define ETH10G_MAX_FRM_SIZE	1536

#define TX_ENABLE
#define RX_ENABLE 

#define TX_BD_NUM		16384		
#define RX_BD_NUM		16384

/*Enable this to enable proc fs for the eth10g driver*/
//#define ENABLE_PROC

/*Enable this to enable ethtool functionality*/
//#define ENABLE_ETHTOOL

#define TX_THREADSAFE
#define RX_THREADSAFE


/*Register offsets*/

//Write sequence 08 0 10
#define DMA_TX_CR_OFFSET	0x00000000 /* Channel control */
#define DMA_TX_SR_OFFSET	0x00000004 /* Status */
#define DMA_TX_CDESC_OFFSET_LO	0x00000008 /* Current descriptor pointer lower 32 bits*/
#define DMA_TX_CDESC_OFFSET_HI	0x0000000C /* Current descriptor pointer upper 32 bits*/
#define DMA_TX_TDESC_OFFSET_LO  0x00000010 /* Tail descriptor pointer lower 32 bits*/
#define DMA_TX_TDESC_OFFSET_HI  0x00000014 /* Tail descriptor pointer upper 32 bits*/


//Write sequence 38 30 40
#define DMA_RX_CR_OFFSET	0x00000030 /* Channel control */
#define DMA_RX_SR_OFFSET	0x00000034 /* Status */
#define DMA_RX_CDESC_OFFSET_LO	0x00000038 /* Current descriptor pointer lower 32 bits*/
#define DMA_RX_CDESC_OFFSET_HI	0x0000003C /* Current descriptor pointer higher 32 bits*/
#define DMA_RX_TDESC_OFFSET_LO	0x00000040 /* Tail descriptor pointer lower 32 bits*/
#define DMA_RX_TDESC_OFFSET_HI	0x00000044 /* Tail descriptor pointer higher 32 bits*/

/*Control register masks*/
#define DMA_CR_RUNSTOP_MASK	0x00000001 /* Start/stop DMA channel */
#define DMA_CR_RESET_MASK	0x00000004 /* Reset DMA engine */

#define DMA_DELAY_MASK		0xFF000000 /* Delay timeout counter */
#define DMA_COALESCE_MASK	0x00FF0000 /* Coalesce counter */

#define DMA_DELAY_SHIFT		24
#define DMA_COALESCE_SHIFT	16

#define DMA_IRQ_IOC_MASK		0x00001000 /* Completion intr */
#define DMA_IRQ_DELAY_MASK		0x00002000 /* Delay interrupt */
#define DMA_IRQ_ERROR_MASK		0x00004000 /* Error interrupt */
#define DMA_IRQ_ALL_MASK		0x00007000 /* All interrupts */

/* Default TX/RX Threshold and waitbound values for SGDMA mode */
#define DMA_DFT_TX_THRESHOLD	254
#define DMA_DFT_TX_WAITBOUND	254
#define DMA_DFT_RX_THRESHOLD	24
#define DMA_DFT_RX_WAITBOUND	254

/*BD regs masks*/
/*BD cntrl register masks*/
#define DMA_BD_CTRL_LENGTH_MASK	0x007FFFFF /* Requested len */
#define DMA_BD_CTRL_TXSOF_MASK	0x08000000 /* First tx packet */
#define DMA_BD_CTRL_TXEOF_MASK	0x04000000 /* Last tx packet */
#define DMA_BD_CTRL_ALL_MASK	0x0C000000 /* All control bits */

/*BD status register masks*/
#define DMA_BD_STS_ACTUAL_LEN_MASK	0x007FFFFF /* Actual len */
#define DMA_BD_STS_COMPLETE_MASK	0x80000000 /* Completed */
#define DMA_BD_STS_DEC_ERR_MASK	0x40000000 /* Decode error */
#define DMA_BD_STS_SLV_ERR_MASK	0x20000000 /* Slave error */
#define DMA_BD_STS_INT_ERR_MASK	0x10000000 /* Internal err */
#define DMA_BD_STS_ALL_ERR_MASK	0x70000000 /* All errors */
#define DMA_BD_STS_RXSOF_MASK	0x08000000 /* First rx pkt */
#define DMA_BD_STS_RXEOF_MASK	0x04000000 /* Last rx pkt */
#define DMA_BD_STS_ALL_MASK		0xFC000000 /* All status bits */

// NIC MAC Statistics Registers
#define NIC_STAT_RXBLSW_OFFSET	0x00000200 /* Received Bytes LSW */
#define NIC_STAT_RXBMSW_OFFSET	0x00000204 /* Received Bytes MSW */
#define NIC_STAT_TXBLSW_OFFSET	0x00000208 /* Transmitted Bytes LSW */
#define NIC_STAT_TXBMSW_OFFSET	0x0000020C /* Transmitted Bytes MSW */
#define NIC_STAT_RXUFLSW_OFFSET	0x00000210 /* Undersize Frames Received LSW */
#define NIC_STAT_RXUFMSW_OFFSET	0x00000214 /* Undersize Frames Received MSW */
#define NIC_STAT_RXFFLSW_OFFSET	0x00000218 /* Fragmented Frames Received LSW */
#define NIC_STAT_RXFFMSW_OFFSET	0x0000021C /* Fragmented Frames Received MSW */
#define NIC_STAT_RX64LSW_OFFSET	0x00000220 /* 64 bytes Frames Received LSW */
#define NIC_STAT_RX64MSW_OFFSET	0x00000224 /* 64 bytes Frames Received MSW */
#define NIC_STAT_RX64127LSW_OFFSET	0x00000228 /* 64-127 bytes Frames Received LSW */
#define NIC_STAT_RX64127MSW_OFFSET	0x0000022C /* 64-127 bytes Frames Received MSW */
#define NIC_STAT_RX128255LSW_OFFSET	0x00000230 /* 128-255 bytes Frames Received LSW */
#define NIC_STAT_RX128255MSW_OFFSET	0x00000234 /* 128-255 bytes Frames Received MSW */
#define NIC_STAT_RX256511LSW_OFFSET	0x00000238 /* 256-511 bytes Frames Received LSW */
#define NIC_STAT_RX256511MSW_OFFSET	0x0000023C /* 256-511 bytes Frames Received MSW */
#define NIC_STAT_RX5121023LSW_OFFSET	0x00000240 /* 512-1023 bytes Frames Received LSW */
#define NIC_STAT_RX5121023MSW_OFFSET	0x00000244 /* 512-1023 bytes Frames Received MSW */
#define NIC_STAT_RX1024LSW_OFFSET	0x00000248 /* 1024-MAX bytes Frames Received LSW */
#define NIC_STAT_RX1024MSW_OFFSET	0x0000024C /* 1024-MAX bytes Frames Received MSW */
#define NIC_STAT_RXOSLSW_OFFSET	0x00000250 /* Oversized Frames Received LSW */
#define NIC_STAT_RXOSMSW_OFFSET	0x00000254 /* Oversized Frames Received MSW */
#define NIC_STAT_TX64LSW_OFFSET	0x00000258 /* 64 bytes Frames Transmitted LSW */
#define NIC_STAT_TX64MSW_OFFSET	0x0000025C /* 64 bytes Frames Transmitted MSW */
#define NIC_STAT_TX64127LSW_OFFSET	0x00000260 /* 64-127 bytes Frames Transmitted LSW */
#define NIC_STAT_TX64127MSW_OFFSET	0x00000264 /* 64-127 bytes Frames Transmitted MSW */
#define NIC_STAT_TX128255LSW_OFFSET	0x00000268 /* 128-255 bytes Frames Transmitted LSW */
#define NIC_STAT_TX128255MSW_OFFSET	0x0000026C /* 128-255 bytes Frames Transmitted MSW */
#define NIC_STAT_TX256511LSW_OFFSET	0x00000270 /* 256-511 bytes Frames Transmitted LSW */
#define NIC_STAT_TX256511MSW_OFFSET	0x00000274 /* 256-511 bytes Frames Transmitted MSW */
#define NIC_STAT_TX5121023LSW_OFFSET	0x00000278 /* 512-1023 bytes Frames Transmitted LSW */
#define NIC_STAT_TX5121023MSW_OFFSET	0x0000027C /* 512-1023 bytes Frames Transmitted MSW */
#define NIC_STAT_TX1024LSW_OFFSET	0x00000280 /* 1024-MAX bytes Frames Transmitted LSW */
#define NIC_STAT_TX1024MSW_OFFSET	0x00000284 /* 1024-MAX bytes Frames Transmitted MSW */
#define NIC_STAT_TXOSLSW_OFFSET	0x00000288 /* Oversized Frames Transmitted LSW */
#define NIC_STAT_TXOSMSW_OFFSET	0x0000028C /* Oversized Frames Transmitted MSW */
#define NIC_STAT_RXOKFLSW_OFFSET 0x00000290  /* Frames Received OK LSW*/
#define NIC_STAT_RXOKFMSW_OFFSET 0x00000294  /* Frames Received OK MSW*/
#define NIC_STAT_RXERRLSW_OFFSET 0x00000298  /* Frames Received w/ CRC Errors LSW*/
#define NIC_STAT_RXERRMSW_OFFSET 0x0000029C  /* Frames Received w/ CRC Errors MSW*/
#define NIC_STAT_RXBOKFLSW_OFFSET 0x000002A0  /* Broadcast Frames Received OK LSW*/
#define NIC_STAT_RXBOKFMSW_OFFSET 0x000002A4  /* Broadcast Frames Received OK MSW*/
#define NIC_STAT_RXMOKFLSW_OFFSET 0x000002A8  /* Multicast Frames Received OK LSW*/
#define NIC_STAT_RXMOKFMSW_OFFSET 0x000002AC  /* Multicast Frames Received OK MSW*/
#define NIC_STAT_RXCOKFLSW_OFFSET 0x000002B0  /* Control Frames Received OK LSW*/
#define NIC_STAT_RXCOKFMSW_OFFSET 0x000002B4  /* Control Frames Received OK MSW*/
#define NIC_STAT_RXCLOFLSW_OFFSET 0x000002B8  /* Length Out of Range Frames Received LSW*/
#define NIC_STAT_RXCLOFMSW_OFFSET 0x000002BC  /* Length Out of Range Frames Received MSW*/
#define NIC_STAT_RXVFLSW_OFFSET 0x000002C0  /* VLAN Tagged Frames Received LSW*/
#define NIC_STAT_RXVFMSW_OFFSET 0x000002C4  /* VLAN Tagged Frames Received MSW*/
#define NIC_STAT_RXPFLSW_OFFSET 0x000002C8  /* Pause Frames Received LSW*/
#define NIC_STAT_RXPFMSW_OFFSET 0x000002CC  /* Pause Frames Received MSW*/
#define NIC_STAT_RXCFOPLSW_OFFSET 0x000002D0  /* Control Frames w/ Unsupported Opcode Received LSW*/
#define NIC_STAT_RXCFOPMSW_OFFSET 0x000002D4  /* Control Frames w/ Unsupported Opcode Received MSW*/
#define NIC_STAT_TXOKFLSW_OFFSET 0x000002D8  /* Frames Transmitted OK LSW*/
#define NIC_STAT_TXOKFMSW_OFFSET 0x000002DC  /* Frames Transmitted OK MSW*/
#define NIC_STAT_TXBOKFLSW_OFFSET 0x000002E0  /* Broadcast Frames Transmitted OK LSW*/
#define NIC_STAT_TXBOKFMSW_OFFSET 0x000002E4  /* Broadcast Frames Transmitted OK MSW*/
#define NIC_STAT_TXMOKFLSW_OFFSET 0x000002E8  /* Multicast Frames Transmitted OK LSW*/
#define NIC_STAT_TXMOKFMSW_OFFSET 0x000002EC  /* Multicast Frames Transmitted OK MSW*/
#define NIC_STAT_TXMUERRLSW_OFFSET 0x000002F0  /* Underrun Errors LSW*/
#define NIC_STAT_TXMUERRMSW_OFFSET 0x000002F4  /* Underrun Errors MSW*/
#define NIC_STAT_TXCOKFLSW_OFFSET 0x000002F8  /* Control Frames Transmitted OK LSW*/
#define NIC_STAT_TXCOKFMSW_OFFSET 0x000002FC  /* Control Frames Transmitted OK MSW*/
#define NIC_STAT_TXVFLSW_OFFSET 0x00000300  /* VLAN Tagged Frames Transmitted LSW*/
#define NIC_STAT_TXVFMSW_OFFSET 0x00000304  /* VLAN Tagged Frames Transmitted MSW*/
#define NIC_STAT_TXPFLSW_OFFSET 0x00000308  /* Pause Frames Transmitted LSW*/
#define NIC_STAT_TXPFMSW_OFFSET 0x0000030C  /* Pause Frames Transmitted MSW*/

// NIC MAC Configuration Registers
#define NIC_RCW0_OFFSET		0x00000400 /* RX Configuration Word 0 */
#define NIC_RCW1_OFFSET		0x00000404 /* RX Configuration Word 1 */
#define NIC_TC_OFFSET		0x00000408 /* TX Configuration */
#define NIC_FCC_OFFSET		0x0000040C /* Flow Control Configuration */
#define NIC_RSC_OFFSET		0x00000410 /* Reconciliation Sublayer Configuration */
#define NIC_RMCW_OFFSET		0x00000414 /* Receiver MTU Configuration Word */
#define NIC_TMCW_OFFSET		0x00000418 /* Transmitter MTU Configuration Word */
#define NIC_V_OFFSET		0x000004F8 /* Version Register (Read Only) */
#define NIC_C_OFFSET		0x000004FC /* Capability Register (Read Only) */

// NIC RX Configuration Word 1 Masks
#define NIC_RCW1_CFLCD_MASK (1 << 24)   /* Control Frame Length Check Disable bit */
#define NIC_RCW1_LTECD_MASK (1 << 25)   /* Length/Type Error Check Disable bit */
#define NIC_RCW1_RPPE_MASK  (1 << 26)   /* Receiver Preserve Preamble Enable bit */
#define NIC_RCW1_VLAN_MASK  (1 << 27)   /* VLAN Enable bit */
#define NIC_RCW1_RXE_MASK   (1 << 28)   /* Receiver Enable bit */
#define NIC_RCW1_IBFE_MASK  (1 << 29)   /* In-band FCS Enable bit */
#define NIC_RCW1_JFE_MASK   (1 << 30)   /* Jumbo Frame Enable bit*/
#define NIC_RCW1_RXR_MASK   (1 << 31)   /* Receiver Reset bit*/

// NIC TX Configuration Word Masks
#define NIC_TC_TPPE_MASK   (1 << 23)   /* Transmitter Preserve Preamble Enable bit */
#define NIC_TC_DICE_MASK   (1 << 24)   /* Deficit Idle Count Enable bit */
#define NIC_TC_IGAE_MASK   (1 << 25)   /* Interframe Gap Adjust Enable bit */
#define NIC_TC_WANE_MASK   (1 << 26)   /* WAN Mode Enable bit */
#define NIC_TC_VLAN_MASK   (1 << 27)   /* VLAN Enable bit */
#define NIC_TC_TXE_MASK    (1 << 28)   /* Transmitter Enable bit */
#define NIC_TC_IBFE_MASK   (1 << 29)   /* In-band FCS Enable bit*/
#define NIC_TC_JFE_MASK    (1 << 30)   /* Jumbo Frames Enable bit*/
#define NIC_TC_TXR_MASK    (1 << 31)   /* Transmitter Reset bit*/

// NIC MDIO Management Registers
#define NIC_MDIO_CW0_OFFSET	0x00000500 /* MDIO Configuration Word 0 */
#define NIC_MDIO_CW1_OFFSET	0x00000504 /* MDIO Configuration Word 1 */
#define NIC_MDIO_TX_OFFSET	0x00000508 /* MDIO TX Data */
#define NIC_MDIO_RX_OFFSET	0x0000050C /* MDIO RX Data */

#define NIC_MDIO_MIS_OFFSET	0x00000600 /* MII Management Interrupt Status */
#define NIC_MDIO_MIP_OFFSET	0x00000610 /* MII Mgmt Interrupt Pending register offset */
#define XAE_MDIO_MIE_OFFSET	0x00000620 /* MII Management Interrupt Enable register offset */
#define XAE_MDIO_MIC_OFFSET	0x00000630 /* MII Management Interrupt ACK register offset. */



#define XAE_TX_VLAN_DATA_OFFSET 0x00004000 /* TX VLAN data table address */
#define XAE_RX_VLAN_DATA_OFFSET 0x00008000 /* RX VLAN data table address */
#define XAE_MCAST_TABLE_OFFSET	0x00020000 /* Multicast table address */

/* Bit Masks for Axi Ethernet RAF register */
#define XAE_RAF_MCSTREJ_MASK		0x00000002 /* Reject receive multicast
						    * destination address */
#define XAE_RAF_BCSTREJ_MASK		0x00000004 /* Reject receive broadcast
						    * destination address */
#define XAE_RAF_TXVTAGMODE_MASK		0x00000018 /* Tx VLAN TAG mode */
#define XAE_RAF_RXVTAGMODE_MASK		0x00000060 /* Rx VLAN TAG mode */
#define XAE_RAF_TXVSTRPMODE_MASK	0x00000180 /* Tx VLAN STRIP mode */
#define XAE_RAF_RXVSTRPMODE_MASK	0x00000600 /* Rx VLAN STRIP mode */
#define XAE_RAF_NEWFNCENBL_MASK		0x00000800 /* New function mode */
#define XAE_RAF_EMULTIFLTRENBL_MASK	0x00001000 /* Exteneded Multicast
						    * Filtering mode
						    */
#define XAE_RAF_STATSRST_MASK		0x00002000 /* Stats. Counter Reset */
#define XAE_RAF_RXBADFRMEN_MASK		0x00004000 /* Recv Bad Frame Enable */
#define XAE_RAF_TXVTAGMODE_SHIFT	3 /* Tx Tag mode shift bits */
#define XAE_RAF_RXVTAGMODE_SHIFT	5 /* Rx Tag mode shift bits */
#define XAE_RAF_TXVSTRPMODE_SHIFT	7 /* Tx strip mode shift bits*/
#define XAE_RAF_RXVSTRPMODE_SHIFT	9 /* Rx Strip mode shift bits*/

/* Bit Masks for Axi Ethernet TPF and IFGP registers */
#define XAE_TPF_TPFV_MASK		0x0000FFFF /* Tx pause frame value */
#define XAE_IFGP0_IFGP_MASK		0x0000007F /* Transmit inter-frame
						    * gap adjustment value */

/* Bit Masks for Axi Ethernet IS, IE and IP registers, Same masks apply
 * for all 3 registers. */
#define XAE_INT_HARDACSCMPLT_MASK	0x00000001 /* Hard register access
						    * complete */
#define XAE_INT_AUTONEG_MASK		0x00000002 /* Auto negotiation
						    * complete */
#define XAE_INT_RXCMPIT_MASK		0x00000004 /* Rx complete */
#define XAE_INT_RXRJECT_MASK		0x00000008 /* Rx frame rejected */
#define XAE_INT_RXFIFOOVR_MASK		0x00000010 /* Rx fifo overrun */
#define XAE_INT_TXCMPIT_MASK		0x00000020 /* Tx complete */
#define XAE_INT_RXDCMLOCK_MASK		0x00000040 /* Rx Dcm Lock */
#define XAE_INT_MGTRDY_MASK		0x00000080 /* MGT clock Lock */
#define XAE_INT_PHYRSTCMPLT_MASK	0x00000100 /* Phy Reset complete */
#define XAE_INT_ALL_MASK		0x0000003F /* All the ints */

#define XAE_INT_RECV_ERROR_MASK				\
	(XAE_INT_RXRJECT_MASK | XAE_INT_RXFIFOOVR_MASK) /* INT bits that
							 * indicate receive
							 * errors */

/* Bit masks for Axi Ethernet VLAN TPID Word 0 register */
#define XAE_TPID_0_MASK		0x0000FFFF /* TPID 0 */
#define XAE_TPID_1_MASK		0xFFFF0000 /* TPID 1 */

/* Bit masks for Axi Ethernet VLAN TPID Word 1 register */
#define XAE_TPID_2_MASK		0x0000FFFF /* TPID 0 */
#define XAE_TPID_3_MASK		0xFFFF0000 /* TPID 1 */

/* Bit masks for Axi Ethernet RCW1 register */
#define XAE_RCW1_RST_MASK	0x80000000 /* Reset */
#define XAE_RCW1_JUM_MASK	0x40000000 /* Jumbo frame enable */
#define XAE_RCW1_FCS_MASK	0x20000000 /* In-Band FCS enable
					    * (FCS not stripped) */
#define XAE_RCW1_RX_MASK	0x10000000 /* Receiver enable */
#define XAE_RCW1_VLAN_MASK	0x08000000 /* VLAN frame enable */
#define XAE_RCW1_LT_DIS_MASK	0x02000000 /* Length/type field valid check
					    * disable */
#define XAE_RCW1_CL_DIS_MASK	0x01000000 /* Control frame Length check
					    * disable */
#define XAE_RCW1_PAUSEADDR_MASK 0x0000FFFF /* Pause frame source address
					    * bits [47:32]. Bits [31:0] are
					    * stored in register RCW0 */

/* Bit masks for Axi Ethernet TC register */
#define XAE_TC_RST_MASK		0x80000000 /* Reset */
#define XAE_TC_JUM_MASK		0x40000000 /* Jumbo frame enable */
#define XAE_TC_FCS_MASK		0x20000000 /* In-Band FCS enable
					    * (FCS not generated) */
#define XAE_TC_TX_MASK		0x10000000 /* Transmitter enable */
#define XAE_TC_VLAN_MASK	0x08000000 /* VLAN frame enable */
#define XAE_TC_IFG_MASK		0x02000000 /* Inter-frame gap adjustment
					    * enable */

/* NIC FCC register bit masks*/
#define NIC_FCC_FCRX_MASK	0x20000000 /* RX flow control enable */
#define NIC_FCC_FCTX_MASK	0x40000000 /* TX flow control enable */

/* Bit masks for Axi Ethernet EMMC register */
#define XAE_EMMC_LINKSPEED_MASK	0xC0000000 /* Link speed */
#define XAE_EMMC_RGMII_MASK	0x20000000 /* RGMII mode enable */
#define XAE_EMMC_SGMII_MASK	0x10000000 /* SGMII mode enable */
#define XAE_EMMC_GPCS_MASK	0x08000000 /* 1000BaseX mode enable */
#define XAE_EMMC_HOST_MASK	0x04000000 /* Host interface enable */
#define XAE_EMMC_TX16BIT	0x02000000 /* 16 bit Tx client enable */
#define XAE_EMMC_RX16BIT	0x01000000 /* 16 bit Rx client enable */
#define XAE_EMMC_LINKSPD_10	0x00000000 /* Link Speed mask for 10 Mbit */
#define XAE_EMMC_LINKSPD_100	0x40000000 /* Link Speed mask for 100 Mbit */
#define XAE_EMMC_LINKSPD_1000	0x80000000 /* Link Speed mask for 1000 Mbit */

/* Bit masks for Axi Ethernet PHYC register */
#define XAE_PHYC_SGMIILINKSPEED_MASK	0xC0000000 /* SGMII link speed mask*/
#define XAE_PHYC_RGMIILINKSPEED_MASK	0x0000000C /* RGMII link speed */
#define XAE_PHYC_RGMIIHD_MASK		0x00000002 /* RGMII Half-duplex */
#define XAE_PHYC_RGMIILINK_MASK		0x00000001 /* RGMII link status */
#define XAE_PHYC_RGLINKSPD_10		0x00000000 /* RGMII link 10 Mbit */
#define XAE_PHYC_RGLINKSPD_100		0x00000004 /* RGMII link 100 Mbit */
#define XAE_PHYC_RGLINKSPD_1000		0x00000008 /* RGMII link 1000 Mbit */
#define XAE_PHYC_SGLINKSPD_10		0x00000000 /* SGMII link 10 Mbit */
#define XAE_PHYC_SGLINKSPD_100		0x40000000 /* SGMII link 100 Mbit */
#define XAE_PHYC_SGLINKSPD_1000		0x80000000 /* SGMII link 1000 Mbit */

/* Bit masks for Axi Ethernet MDIO interface MC register */
#define XAE_MDIO_MC_MDIOEN_MASK		0x00000040 /* MII management enable */
#define XAE_MDIO_MC_CLOCK_DIVIDE_MAX	0x3F	   /* Maximum MDIO divisor */

/* Bit masks for Axi Ethernet MDIO interface MCR register */
#define XAE_MDIO_MCR_PHYAD_MASK		0x1F000000 /* Phy Address Mask */
#define XAE_MDIO_MCR_PHYAD_SHIFT	24	   /* Phy Address Shift */
#define XAE_MDIO_MCR_REGAD_MASK		0x001F0000 /* Reg Address Mask */
#define XAE_MDIO_MCR_REGAD_SHIFT	16	   /* Reg Address Shift */
#define XAE_MDIO_MCR_OP_MASK		0x0000C000 /* Operation Code Mask */
#define XAE_MDIO_MCR_OP_SHIFT		13	   /* Operation Code Shift */
#define XAE_MDIO_MCR_OP_READ_MASK	0x00008000 /* Op Code Read Mask */
#define XAE_MDIO_MCR_OP_WRITE_MASK	0x00004000 /* Op Code Write Mask */
#define XAE_MDIO_MCR_INITIATE_MASK	0x00000800 /* Ready Mask */
#define XAE_MDIO_MCR_READY_MASK		0x00000080 /* Ready Mask */

/* Bit masks for Axi Ethernet MDIO interface MIS, MIP, MIE, MIC registers */
#define XAE_MDIO_INT_MIIM_RDY_MASK	0x00000001 /* MIIM Interrupt */

/* Bit masks for Axi Ethernet UAW1 register */
#define XAE_UAW1_UNICASTADDR_MASK	0x0000FFFF /* Station address bits
						    * [47:32]; Station address
						    * bits [31:0] are stored in
						    * register UAW0 */

/* Bit masks for Axi Ethernet FMI register */
#define XAE_FMI_PM_MASK			0x80000000 /* Promis. mode enable */
#define XAE_FMI_IND_MASK		0x00000003 /* Index Mask */

#define XAE_MDIO_DIV_DFT		29 /* Default MDIO clock divisor */

/* Defines for different options for C_PHY_TYPE parameter in Axi Ethernet IP */
#define XAE_PHY_TYPE_MII		0
#define XAE_PHY_TYPE_GMII		1
#define XAE_PHY_TYPE_RGMII_1_3		2
#define XAE_PHY_TYPE_RGMII_2_0		3
#define XAE_PHY_TYPE_SGMII		4
#define XAE_PHY_TYPE_1000BASE_X		5

#define XAE_MULTICAST_CAM_TABLE_NUM	4 /* Total number of entries in the
					   * hardware multicast table. */
/*
#define hw_write(base, reg, val)					\
	__raw_writel((val), (void __iomem *)((uint32_t)(base) + (uint32_t)(reg)))
*/	

#define hw_write64(base, reg, val)					\
	__raw_writeq((val), (void __iomem *)((uint64_t)(base) + (uint64_t)(reg)))	

#define hw_write(base, reg, val)					\
	__raw_writel((val), (void __iomem *)((uint64_t)(base) + (uint32_t)(reg)))	
	
	
/*
#ifdef IO_WIRE_METHOD_WRITEL
	#define dma_write(lp, reg, val) writel(val, (off_t)((lp)->dma_regs) + offset);
#else
	#define dma_write(lp, reg, val) hw_write((lp)->dma_regs, reg, val)
#endif
*/

#define LO32(val)	((uint32_t)((val) & 0xFFFFFFFF))
#define HI32(val)	((uint32_t)((val) >> 32))
#define WHOLE(lo, hi)	((uint64_t) (hi) << 32 | (lo)); 
		
#define ReadReg32(BASE_ADDRESS, RegOffset) \
          *(uint32_t *)(BASE_ADDRESS + RegOffset);
          
#define ReadReg64(BASE_ADDRESS, RegOffset) \
          *(uint64_t *)(BASE_ADDRESS + RegOffset);
          
          
#define RX_BUFFER_SIZE	128
#define RX_RING_SIZE	512
#define RX_RING_BYTES	(sizeof(struct dma_desc) * RX_RING_SIZE)

#define TX_RING_SIZE	128
#define TX_RING_BYTES	(sizeof(struct dma_desc) * TR_RING_SIZE)


struct dma_bd {
/*0x00*/	uint32_t next_lo; /* (TRANSLATED) Physical address of next buffer descriptor lower 32 bits*/
/*0x04*/	uint32_t next_hi; /* (TRANSLATED) Physical address of next buffer descriptor upper 32 bits*/	
/*0x08*/	uint32_t phys_lo; /* (TRANSLATED) Physical address of data lower 32 bits*/
/*0x0C*/	uint32_t phys_hi; /* (TRANSLATED) Physical address of data upper 32 bits*/
/*0x10*/	uint32_t reserved0;
/*0x14*/	uint32_t reserved1;	
/*0x18*/	uint32_t cntrl;
/*0x1C*/	uint32_t status;
/*0x20*/	uint32_t app0;		
/*0x24*/	uint32_t rx_skb_virt_lo; /* RX skb virtual address lower 32 bits */
/*0x28*/	uint32_t rx_skb_virt_hi; /* RX skb virtual address upper 32 bits */	
/*0x2C*/	uint32_t app3;	
/*0x30*/	uint32_t tx_skb_virt_lo; /* TX skb virtual address lower 32 bits */
/*0x34*/	uint32_t tx_skb_virt_hi; /* TX skb virtual address upper 32 bits */	
/*0x38*/	uint32_t phys_cpu_lo;  /* (UNTRANSLATED) Physical address of data lower 32 bits */
/*0x3C*/	uint32_t phys_cpu_hi;  /* (UNTRANSLATED) Physical address of data upper 32 bits */
};

struct proc_eth10g_struct {
	struct proc_dir_entry *dir_main;	
	
	struct proc_dir_entry *dir_mac;	
	struct proc_dir_entry *rx_stats;
	struct file_operations *fops_rx_stats;
	struct proc_dir_entry *tx_stats;
	struct file_operations *fops_tx_stats;
	
	struct proc_dir_entry *frm_rx_ok;
	struct proc_dir_entry *frm_tx_ok;
	struct file_operations *fops_frm_tx_ok;
	struct file_operations *fops_frm_rx_ok;
	
	struct proc_dir_entry *mac_config;
	struct file_operations *fops_mac_config;
	
	struct proc_dir_entry *rx_config;
	struct file_operations *fops_rx_config;	

	struct proc_dir_entry *tx_config;
	struct file_operations *fops_tx_config;	

	struct proc_dir_entry *tx_unaligned;
	struct file_operations *fops_tx_unaligned;	
	
	struct proc_dir_entry *rx_unaligned;
	struct file_operations *fops_rx_unaligned;	
		
	struct proc_dir_entry *dir_dma;	
	struct proc_dir_entry *dir_drv;							
};

struct net_private {
	struct net_device *ndev;
	struct device *dev;

	void *mmio_addr;

	void __iomem *regs;
	void __iomem *dma_regs;	

	uint64_t regs_len;

	struct napi_struct napi;        /* napi information*/

	uint32_t max_frm_size;
	uint32_t jumbo_support;

	struct tasklet_struct dma_err_task;
	
	/*RX rings*/
	struct dma_bd *rx_bd_virt;
	dma_addr_t rx_bd_phys;	              
	
	/*TX rings*/
	struct dma_bd *tx_bd_virt;
	dma_addr_t tx_bd_phys;	

	/*  Indices */
	uint32_t rx_curr;
	uint32_t rx_tail;

	uint32_t tx_curr;
	uint32_t tx_tail;
	uint32_t tx_head; 
	
	/*Interrupts*/
	int irq;
	int tx_irq;
	int rx_irq;

	spinlock_t  tx_tail_lock;		
	spinlock_t  tx_curr_lock;
	spinlock_t  rx_lock;		
	uint32_t coalesce_count_rx;
	uint32_t coalesce_count_tx;
	
	
	uint32_t tx_coa_frags;
	uint32_t rx_coa_frags;
	
	int cnt;
	
	uint64_t tx_unaligned32;
	uint64_t rx_unaligned32;
	
	struct proc_eth10g_struct *proc_eth10g;
};

static inline void nic_write32(struct net_private *lp, off_t offset, uint32_t val)
{
		hw_write(lp->regs, offset, val);
}

static inline uint32_t nic_read32(struct net_private *lp, off_t offset)
{
		return ReadReg32(lp->regs, offset);
}

static inline void dma_write32(struct net_private *lp, uint64_t offset, uint32_t val)
{
//		writel(val, (off_t)(lp->dma_regs) + offset);
		hw_write(lp->dma_regs, offset, val);		
}

static inline uint32_t dma_read32(struct net_private *lp, uint64_t offset)
{
//		return readl((off_t)(lp->dma_regs) + offset);
		return ReadReg32(lp->dma_regs, offset);
}

static inline void dma_write64(struct net_private *lp, uint64_t offset, uint64_t val)
{
	//		writel(val, (off_t)(lp->dma_regs) + offset);
	hw_write64(lp->dma_regs, offset, val);		
}

static inline uint64_t dma_read64(struct net_private *lp, uint64_t offset)
{
	//		return readl((off_t)(lp->dma_regs) + offset);
	return ReadReg64(lp->dma_regs, offset);
}

static inline uint64_t addr_trans(uint64_t cpu_addr)
{
	uint64_t naked_addr = 0;
	uint64_t new_addr = 0;
	
	if (cpu_addr>=DRAM_A_START && cpu_addr<=DRAM_A_END)
	{	
		naked_addr = cpu_addr - DRAM_A_START;
		new_addr = naked_addr + DRAM_A_START_TRANS;
		return new_addr;
	}
	if (cpu_addr>=DRAM_B_START && cpu_addr<=DRAM_B_END)
	{
		naked_addr = cpu_addr - DRAM_B_START;
		new_addr = naked_addr + DRAM_B_START_TRANS;
		return new_addr;
	}
	return 0;
}

extern int create_proc_eth10g(struct net_device *ndev);
extern int remove_proc_eth10g(struct net_device *dev);

extern int eth10g_ethtool_init(struct net_device *ndev);
#endif