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

#include <linux/pci.h>
#include <linux/msi.h>

#include <linux/if_vlan.h>

#include <linux/spinlock.h>
#include <linux/interrupt.h>

#include <linux/proc_fs.h>
#include <linux/seq_file.h>	

#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/workqueue.h>

#include <linux/time.h>
#include <linux/smp.h>

#include <linux/gpio.h>

#define DRIVER              "kgex"
#define DRIVER_NAME	    "kgex"
#define DRIVER_DESCRIPTION  "Kmax 10 Gbps NIC Driver"
#define DRIVER_VERSION "7.0.0"

#define pr(fmt, ...) {pr_info(DRIVER_NAME ": " fmt, ##__VA_ARGS__);}
#define prerr(fmt, ...) {pr_info(DRIVER_NAME ": ERROR: " fmt, ##__VA_ARGS__);}
#define prierr(fmt, ...) {pr_info(DRIVER_NAME ": Internal ERROR: " fmt, ##__VA_ARGS__);}
#define prwarn(fmt, ...) {pr_info(DRIVER_NAME ": WARNING: " fmt, ##__VA_ARGS__);}
#define prcont(fmt, ...) {printk(KERN_CONT fmt, ##__VA_ARGS__);}

//#define ETH10G_DEBUG_LEVEL_1

#ifdef ETH10G_DEBUG_LEVEL_3
#ifndef ETH10G_DEBUG_LEVEL_2
#define ETH10G_DEBUG_LEVEL_2
#endif
#define dbg3(fmt, ...) {pr(fmt, ##__VA_ARGS__);}
#else
#define dbg3(fmt, ...) {;}
#endif


#ifdef ETH10G_DEBUG_LEVEL_2
#ifndef ETH10G_DEBUG_LEVEL_1
#define ETH10G_DEBUG_LEVEL_1
#endif
#define dbg2(fmt, ...) {pr(fmt, ##__VA_ARGS__);}
#else
#define dbg2(fmt, ...) {;}
#endif


#ifdef ETH10G_DEBUG_LEVEL_1
#define dbg1(fmt, ...) {pr(fmt, ##__VA_ARGS__);}
#else
#define dbg1(fmt, ...) {;}
#endif 


#define DEVICE_COUNT_RESOURCE	(6)
#define MSIX_VECTORS		0x8

#define KGEX_MAX_NET_DEVS   2
#define KGEX_NIC_ID_NPU 0
#define KGEX_NIC_ID_SPU 1

#define KGE_HDR_SIZE			14 /* Size of Ethernet header */
#define KGE_TRL_SIZE			 4 /* Size of Ethernet trailer (FCS) */
#define KGE_MTU			      1500 

#define KGE_JUMBO_MTU		      9000 /* Max MTU of a jumbo Eth. frame */

#define KGE_MAX_FRAME_SIZE	 (KGE_MTU + KGE_HDR_SIZE + KGE_TRL_SIZE)
#define KGE_MAX_VLAN_FRAME_SIZE  (KGE_MTU + KGE_HDR_SIZE + VLAN_HLEN + KGE_TRL_SIZE)
#define KGE_MAX_JUMBO_FRAME_SIZE (KGE_JUMBO_MTU + KGE_HDR_SIZE + KGE_TRL_SIZE)
#define KGE_MAX_VLAN_JUMBO_FRAME_SIZE (KGE_JUMBO_MTU + KGE_HDR_SIZE + VLAN_HLEN + KGE_TRL_SIZE)
#define KGE_MAX_FRM_SIZE	1536

#define TX_BD_NUM		2048
#define RX_BD_NUM		4196

#define KGEX_NAPI_WEIGHT	256

#define ENABLE_PROC
// #define ENABLE_ETHTOOL

//Enable this if the Xilinx AXI DMA endpoint is 64-bit
//#define AXI_DMA_64

#define KGEX_REVIVER_DEFAULT_AFFINITY   7

/* ===== PCIe Addresses and Offsets  =============================================================*/
#define PCI_RC_NPU_BASE         0x157B0000      /* Physical Address of PCIe Root Complex to NPU */
#define PCI_RC_SPU_BASE         0x155C0000      /* Physical Address of PCIe Root Complex to SPU */

#define PCIE_RC_LINK_CTRL       0x007C        /* Link Control Register Offset*/
#define PCIE_RC_LINK_RETRAIN    0x080C        /* Link Retrain Register Offset*/
#define PCIE_RC_LINK_STATUS     0x0080        /* Link Status Register Offset*/

#define PCIE_LINK_STATUS_GEN1_MASK 0x00010000
#define PCIE_LINK_STATUS_GEN2_MASK 0x00020000
#define PCIE_LINK_CTRL_GEN1_MASK   0x00000001
#define PCIE_LINK_CTRL_GEN2_MASK   0x00000002
#define PCIE_LINK_CTRL_GEN_CLEAR_MASK    0x0000000F
#define PCIE_LINK_RETRAIN_FORCE_TRAIN_MASK  (1<<17)

#define PCIE_RETRAIN_MAX_RETRIES   10000

/* ===== Configuration Space Offsets =============================================================*/
#define CONF_CMD_OFF		0x100
#define CONF_ARG_OFF		0x110
#define CONF_STAT_OFF           0x120
#define CONF_RESP_OFF   	0x130
#define CONF_PROD_RST_OFF	0x140
#define CONF_INFO_OFF	        0x200
#define CONF_PCIERRS_OFF	0x400
#define CONF_NIC_A_OFF          0x0
#define CONF_NIC_B_OFF          0x4
#define CONF_MNG_OFF		0x8
#define CONF_STG_OFF		0xC
#define CONF_UPDATE_IP_OFF	0x80
#define CONF_SSD_STATUS_OFF 	0x84

/*Register offsets*/
#ifdef AXI_DMA_64
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

#else
//Write sequence 08 0 10
#define DMA_TX_CR_OFFSET	0x00000000 /* Channel control */
#define DMA_TX_SR_OFFSET	0x00000004 /* Status */
#define DMA_TX_CDESC_OFFSET	0x00000008 /* Current descriptor pointer */
#define DMA_TX_TDESC_OFFSET	0x00000010 /* Tail descriptor pointer */

//Write sequence 38 30 40
#define DMA_RX_CR_OFFSET	0x00000030 /* Channel control */
#define DMA_RX_SR_OFFSET	0x00000034 /* Status */
#define DMA_RX_CDESC_OFFSET	0x00000038 /* Current descriptor pointer */
#define DMA_RX_TDESC_OFFSET	0x00000040 /* Tail descriptor pointer */
#endif

/* Read request counters */
#define AXI_RD_PEND_RDS_OFF     0x400
#define AXI_RD_REQS_OFF         0x404
#define AXI_RD_ACKS_OFF         0x408
#define AXI_RD_NACKS_OFF        0x40C

/* Read request counters */
#define AXI_WR_PEND_RDS_OFF     0x418
#define AXI_WR_REQS_OFF         0x41C
#define AXI_WR_ACKS_OFF         0x420
#define AXI_WR_NACKS_OFF        0x424

/*Control register masks*/
#define DMA_CR_RUNSTOP_MASK	0x00000001 /* Start/stop DMA channel */
#define DMA_CR_RESET_MASK	0x00000004 /* Reset DMA engine */

#define DMA_DELAY_MASK		0xFF000000 /* Delay timeout counter */
#define DMA_COALESCE_MASK		0x00FF0000 /* Coalesce counter */

#define DMA_DELAY_SHIFT		24
#define DMA_COALESCE_SHIFT		16

#define DMA_IRQ_IOC_MASK		0x00001000 /* Completion intr */
#define DMA_IRQ_DELAY_MASK		0x00002000 /* Delay interrupt */
#define DMA_IRQ_ERROR_MASK		0x00004000 /* Error interrupt */
#define DMA_IRQ_ALL_MASK		0x00007000 /* All interrupts */


#define DMA_STS_HALTED		0x00000001
#define DMA_STS_IDLE		0x00000002
#define DMA_STS_INT_ERR		0x00000010
#define DMA_STS_SLV_ERR		0x00000020
#define DMA_STS_DEC_ERR		0x00000020
#define DMA_STS_SG_INT_ERR	0x00000100
#define DMA_STS_SG_SLV_ERR	0x00000200
#define DMA_STS_SG_DEC_ERR	0x00000400

/* Default TX/RX Threshold and waitbound values for SGDMA mode */
#define DMA_DFT_TX_THRESHOLD	10
#define DMA_DFT_TX_WAITBOUND	20
#define DMA_DFT_RX_THRESHOLD	1
#define DMA_DFT_RX_WAITBOUND	20

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
#ifdef IO_WIRE_METHOD_WRITEL
#define dma_write(lp, reg, val) writel(val, (off_t)((lp)->dma_regs) + offset);
#else
#define dma_write(lp, reg, val) hw_write((lp)->dma_regs, reg, val)
#endif
 */

//For 32-bit endpoint
struct dma_bd {
	uint32_t next;	/* Physical address of next buffer descriptor */
	uint32_t reserved1;
	uint32_t phys;        /*Physical address of data*/
	uint32_t reserved2;
	uint32_t reserved3;
	uint32_t reserved4;
	uint32_t cntrl;
	uint32_t status;
	uint32_t app0;
	uint32_t app1;   /*Modified for hybrid 64-bit 32-bit operation*/
	uint32_t app2;   /*Modified for hybrid 64-bit 32-bit operation*/
	uint32_t app3;            
	uint32_t app4;   	   /*Modified for hybrid 64-bit 32-bit operation*/
	uint32_t app5;   	   /*Modified for hybrid 64-bit 32-bit operation*/
	uint32_t reserved5;
	uint32_t reserved6;
};

#define DMA_MAP_SINGLE 	0
#define DMA_MAP_PAGE	1

struct desc_struct {
	uint32_t dma_mapping;
	dma_addr_t phys;
	uint32_t len;       
	struct sk_buff *skb;
};



struct axi_dma_regs {
	uint32_t tx_cr;
	uint32_t tx_sr;
	uint32_t tx_cdesc;
	uint32_t tx_tdesc;
	uint32_t rx_cr;
	uint32_t rx_sr;
	uint32_t rx_cdesc;
	uint32_t rx_tdesc;    
};

struct kgex_dma_proc_struct {
	struct proc_dir_entry *dir;

	struct file_operations *fops_ctrl;

	struct proc_dir_entry *tx_queue_stopped_times;
	struct proc_dir_entry *tx_xmit_times;
	struct proc_dir_entry *tx_active_descs;
	struct proc_dir_entry *tx_xmit_affinity;
	struct proc_dir_entry *tx_poll_coalesce;
	struct proc_dir_entry *tx_poll_affinity;
	struct proc_dir_entry *tx_poll_jiffies;

	//PCIe Configuration
	struct proc_dir_entry *pci_gen;
	struct proc_dir_entry *reviver;
	struct proc_dir_entry *reviver_affinity;

	struct proc_dir_entry *error_print;

	struct proc_dir_entry *netif_queue;        

	//AXI DMA Configuration    
	struct proc_dir_entry *regs;
	struct proc_dir_entry *tx_ctrl_reg;
	struct proc_dir_entry *tx_status_reg;
	struct proc_dir_entry *rx_ctrl_reg;
	struct proc_dir_entry *rx_status_reg;

	struct proc_dir_entry *tx_coalesce;
	struct proc_dir_entry *tx_timeout;
	struct proc_dir_entry *rx_coalesce;
	struct proc_dir_entry *rx_timeout;    
	struct proc_dir_entry *rx_ip_checksum;
	struct proc_dir_entry *wrong_rx_irqs;
	struct proc_dir_entry *tx_irq_spurious;
	struct proc_dir_entry *rx_irq_spurious;
	struct proc_dir_entry *tx_irq_unknown;
	struct proc_dir_entry *rx_irq_unknown;

	struct proc_dir_entry * nr_pci_gen_transitions;
	struct proc_dir_entry * nr_pci_retrains;
	struct proc_dir_entry * nr_dma_errors;
	struct proc_dir_entry * nr_rc_resets_ok;

	struct proc_dir_entry * nr_external_resets;
	
	struct proc_dir_entry * tx_irq_wdog;
	struct proc_dir_entry * rx_irq_wdog;

	struct proc_dir_entry *tx_dma_mapped_descs;
	struct proc_dir_entry *rx_dma_mapped_descs;

	struct proc_dir_entry * errors;
	//AXI DMA errors
	struct proc_dir_entry * tx_dma_errors;	
	struct proc_dir_entry * rx_dma_errors;

	//AXI DMA descriptor errors
	struct proc_dir_entry * tx_dma_desc_errors;
	struct proc_dir_entry * rx_dma_desc_errors;

	struct proc_dir_entry * mem;	

	//SFLR
	struct proc_dir_entry * sflr_enable;
	struct proc_dir_entry * sflr_frame_size;
	struct proc_dir_entry * sflr_irq_delay;
};


struct kgex_conf_proc_struct {
	struct proc_dir_entry *dir;    
	struct file_operations *fops;

	struct proc_dir_entry *mac_mng_addr;
	struct proc_dir_entry *mac_a_addr;
	struct proc_dir_entry *mac_b_addr;
	struct proc_dir_entry *mac_stg_addr;
	struct proc_dir_entry *update_srv_ip;
	struct proc_dir_entry *ssd_status;

	struct proc_dir_entry *dir_mng;

	struct proc_dir_entry *mng_command;
	struct proc_dir_entry *mng_cmd_arg;
	struct proc_dir_entry *mng_cmd_resp;
	struct proc_dir_entry *mng_status;
	struct proc_dir_entry *mng_info;
	struct proc_dir_entry *mng_pci_npuerrs;

};

struct kgex_proc_struct {
	struct proc_dir_entry *dir_main;	
	struct kgex_dma_proc_struct dma_proc[2];
	struct kgex_conf_proc_struct conf_proc;	

	struct file_operations *fops;
	struct proc_dir_entry *version;
};

struct kgex_work {
	struct work_struct work;
	struct net_device *ndev;
};

struct kgex_delayed_work {
	struct delayed_work work;
	struct net_device *ndev;
};

struct net_private {
	struct pci_dev *pdev;        
	struct pci_saved_state *pdev_state;
	int pci_bars;
	uint64_t pci_phys_addr[DEVICE_COUNT_RESOURCE];
	uint64_t pci_len[DEVICE_COUNT_RESOURCE];

	struct msix_entry *msix_entries;

	uint64_t nr_pci_gen_transitions;
	uint64_t nr_pci_retrains;
	uint32_t pci_link_down;

	struct net_device *ndev;
	struct device *dev;

	uint32_t id;

	uint32_t reset_required;

	void __iomem *regs;
	void __iomem *dma_regs;
	void __iomem *configuration_space;
	void __iomem *pci_rc_regs;

	struct napi_struct napi;

	uint32_t max_frm_size;

	/*RX rings*/
	struct dma_bd *rx_bd_virt;
	dma_addr_t rx_bd_phys;	              
	struct desc_struct *rx_descs;

	/*TX rings*/
	struct dma_bd *tx_bd_virt;        
	dma_addr_t tx_bd_phys;
	struct desc_struct *tx_descs;

	struct kgex_delayed_work *reviver_work;
	uint32_t revive_jiffies;
	uint32_t reviver_affinity;
	uint32_t dma_tx_cdesc_prev;
	uint32_t reviver_dma_consecutive_ongoing;

	struct kgex_delayed_work *xmit_work;
	int xmit_working;

	int tx_rings_init;	
	int rx_rings_init;

	uint32_t tx_bd_num;
	uint32_t rx_bd_num;

	uint32_t tx_dma_mapped_descs;
	uint32_t rx_dma_mapped_descs;

	uint32_t rx_curr;
	uint32_t rx_tail;

	uint32_t tx_curr;
	uint32_t tx_tail;
	uint32_t tx_head; 

	/*Interrupts*/
	int tx_irq;
	int rx_irq;
	uint64_t nr_tx_irqs;
	uint64_t nr_rx_irqs;
	uint64_t nr_tx_irqs_prev;
	uint64_t nr_rx_irqs_prev;
	
	int tx_irq_wdog;
	int rx_irq_wdog;

	int irqs;
	
	int tx_xmit_affinity;        
	uint32_t tx_poll_coalesce;
	uint32_t tx_poll_jiffies;
	uint32_t tx_poll_affinity;

	uint32_t coalesce_count_rx;
	uint32_t coalesce_count_tx;
	uint32_t timeout_rx;
	uint32_t timeout_tx;

	uint32_t rx_ip_checksum;

	volatile uint32_t tx_irq_enabled;
	volatile uint32_t rx_irq_enabled;

	int cnt;

	struct kgex_eth_proc_struct *kgex_eth_proc;

	uint32_t error_print;

	uint64_t tx_xmit_times;
	uint64_t queue_stopped_times;
	uint32_t tx_active_descs;
	uint64_t tx_irq_unknown;
	uint64_t rx_irq_unknown;

	uint64_t tx_irq_spurious;
	uint64_t rx_irq_spurious;

	uint32_t nr_rc_resets_ok;
	uint32_t nr_external_resets;

	//AXI DMA errors
	uint32_t tx_dma_errors;	
	uint32_t rx_dma_errors;	

	//AXI DMA descriptor errors
	uint32_t tx_dma_desc_errors;
	uint32_t rx_dma_desc_errors;

	//AXI DMA ongoing issue debug
	int dump_cnts;

};


#define LO32(val)	((uint32_t)((val) & 0xFFFFFFFF))
#define HI32(val)	((uint32_t)((val) >> 32))
#define WHOLE(lo, hi)	((uint64_t) (hi) << 32 | (lo)); 


static inline int kgex_virt_addr_error(uint64_t addr)
{
	if (((addr >> (VA_BITS)) << (VA_BITS)) == ((((uint64_t)(0xFFFFFFFFFFFFFFFF)) >> (VA_BITS)) << (VA_BITS))) {
		return 0;
	}

	return 1;
}

extern int exynos_pcie_is_linkup(struct pci_dev *rc);

extern struct kgex_proc_struct *kgex_proc;

extern int kgex_eth_proc_create(uint32_t ethid);

extern int kgex_set_rx_ip_checksum(struct net_device *ndev, uint32_t csum_type);
extern uint32_t kgex_get_rx_ip_checksum(struct net_device *ndev);
extern int kgex_proc_create(void);
extern int kgex_proc_remove(void);

extern int eth10g_ethtool_init(struct net_device *ndev);


#endif
