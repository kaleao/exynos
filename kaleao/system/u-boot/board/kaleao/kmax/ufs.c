/*
 * Copyright (C) 2014 Samsung Electronics
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/arch/pinmux.h>
#include <scsi.h>
#include <ufs.h>
#include "setup.h"

#define UPSCTRL_KEY256_EN	(1<<8) // 256 bit Key Enable
#define UPSCTRL_SR_EN		(1<<7) // Secure Read Enable
#define UPSCTRL_SW_EN		(1<<6) // Secure Write Enable
#define UPSCTRL_NSR_EN		(1<<5) // Non-Secure Read Enable
#define UPSCTRL_NSW_EN		(1<<4) // Non-Secure Write Enable
#define UPSCTRL_UFK_EN		(1<<3) // Use Fuse Key
#define UPSCTRL_ECB_EN		(1<<2) // ECB Enable
#define UPSCTRL_ENC_EN		(1<<1) // Encrypt Enable
#define UPSCTRL_VALID_EN	(1<<0) // Valid

/*
	0 : External OSC 26Mhz -> 26MHz Refclk out
	1 : Internal PLL 26Mhz -> 26MHz Refclk out
	2 : External 26Mhz Xtal -> 26MHz Refclk out
	3 : External 24Mhz Xtal -> MPHY PLL -> 26Mhz Refclk out
*/
#define UFS_REFCLK_SET		3
#define UFS_GEAR		2
#define UFS_RATE		2
//#define UFS_LANE		2
#define UFS_LANE		1
#define UFS_POWER_MODE	1
#define UFS_RXTX_POWER_MODE	((UFS_POWER_MODE<<4)|UFS_POWER_MODE)

#define INPUT_PCLK_RATE		(100 * (1000 * 1000))	/* 100MHz */
#define INPUT_MCLK_RATE		(167 * (1000 * 1000))	/* 167MHz */
#define UNIPRO_PCLK_RATIO	0

#define UNIPRO_PCLK_SRC_PERIOD_NS	(1000000000L / INPUT_PCLK_RATE)
#define UNIPRO_MCLK_SRC_PERIOD_NS	(1000000000L / INPUT_MCLK_RATE)
#define HOST_UNIPRO_PCLK_PERIOD_NS	UNIPRO_PCLK_SRC_PERIOD_NS
#define RX_HIBER8_WAIT_NS		4000000
#define TX_DIF_P_PERIOD_NS		3000000
#define TX_DIF_N_PERIOD_NS		1000000
#define RX_DIF_P_PERIOD_NS		1000000

#define uHigh_Z_Cnt	(20000/HOST_UNIPRO_PCLK_PERIOD_NS)
#define uTx_Sleep_Cnt	(((1000/HOST_UNIPRO_PCLK_PERIOD_NS) >> 5)&0x7F)
#define uTx_Base_Unit	(100000/HOST_UNIPRO_PCLK_PERIOD_NS)
#define uTx_Gran_Unit	(4000/HOST_UNIPRO_PCLK_PERIOD_NS)
#define uRx_Base_Unit	(100000/HOST_UNIPRO_PCLK_PERIOD_NS)
#define uRx_Gran_Unit	(4000/HOST_UNIPRO_PCLK_PERIOD_NS)
#define uRx_SleepCntTimer	(((1280/HOST_UNIPRO_PCLK_PERIOD_NS) >> 6)&0x3f)
#define uRx_StallCntTimer	(((320/HOST_UNIPRO_PCLK_PERIOD_NS) >> 4)&0xff)
#define uTx_linereset_pvalue	(3000000/(HOST_UNIPRO_PCLK_PERIOD_NS * (1<<12)))
#define uRx_linereset_pvalue	(1000000/(HOST_UNIPRO_PCLK_PERIOD_NS * (1<<12)))

#if (UFS_GEAR == 1)
#define uRx_sync_pattern	((((80*1000) / HOST_UNIPRO_PCLK_PERIOD_NS) >> 8) & 0xff)
#elif (UFS_GEAR == 2)
#define uRx_sync_pattern	((((40*1000) / HOST_UNIPRO_PCLK_PERIOD_NS) >> 8) & 0xff)
#endif

static int ufs_device_reset(struct ufs_host *ufs, int onoff)
{
	writel((onoff), ufs->ioaddr + VS_GPIO_OUT);
	return 0;
}

static int ufs_device_power(struct ufs_host *ufs, int onoff)
{
	/* GPH1_DAT[4] : GPH1_4=1 */
	UFS_SET_SFR(0x14870124, onoff, 0x1, 4);

	return 0;
}

static int ufs_pre_setup(struct ufs_host *ufs)
{
	u32 reg;
	int res = CMD_RET_FAILURE;
	struct ufs_uic_cmd reset_cmd = {UIC_CMD_DME_RESET,};
	struct ufs_uic_cmd enable_cmd = {UIC_CMD_DME_ENABLE,};
	/* UFS_PHY_CONTROL : 1 = Isolation bypassed, PMU MPHY ON */
	if ( (readl(0x105C0720)&1)==0 )
	{
		writel(0x01, 0x105C0720);
	}
	/* VS_SW_RST */
	if ( (readl(ufs->ioaddr + VS_FORCE_HCS)>>4)&0xf )
	{
		writel(0x0, VS_FORCE_HCS);
	}
	writel(3, (ufs->ioaddr + VS_SW_RST));
	while ( readl(ufs->ioaddr + VS_SW_RST) );
	/* VENDOR_SPECIFIC_IS[20] : clear UFS_IDLE_Indicator bit (if UFS_LINK is reset, this bit is asserted) */
	reg = readl(ufs->ioaddr + VS_IS);
	if ( (reg>>20)&0x1 )
	{
		writel(reg, (ufs->ioaddr + VS_IS));
	}
	/* Set Unipro PCLK Divide value to 1 */
	reg = readl(ufs->ioaddr + VS_UNIPRO_APB_CLK_CTRL);
	reg = (reg & ~0xf) | (UNIPRO_PCLK_RATIO & 0xf);
	writel(reg, (ufs->ioaddr + VS_UNIPRO_APB_CLK_CTRL));
	ufs_device_reset(ufs, 0);
	udelay(5);
	ufs_device_reset(ufs, 1);
	ufs_device_power(ufs, 1);
	if(ufs->quirks&UFS_QUIRK_BROKEN_HCE) {
		ufs->uic_cmd = &reset_cmd;
		res = send_uic_cmd(ufs);
		if (res) {
				printf("UFS%d reset error!\n",ufs->host_index);
				return res;
			}
		ufs->uic_cmd = &enable_cmd;
		res = send_uic_cmd(ufs);
			if (res) {
				printf("UFS%d reset error!\n",ufs->host_index);
				return res;
			}
	}
	else {
		writel(1 , (ufs->ioaddr + REG_CONTROLLER_ENABLE));

		while (!(readl(ufs->ioaddr + REG_CONTROLLER_ENABLE) & 0x1)) {
			udelay(1);
		}
	}

	writel(0xf , (ufs->ioaddr + VS_AXIDMA_RWDATA_BURST_LEN));

	memset( ufs->cmd_desc_addr, 0x00, UFS_NUTRS*sizeof(struct ufs_cmd_desc));
	memset( ufs->utrd_addr, 0x00, UFS_NUTRS*sizeof(struct ufs_utrd));
	memset( ufs->utmrd_addr, 0x00, UFS_NUTMRS*sizeof(struct ufs_utmrd));
	ufs->utrd_addr->cmd_desc_addr_l = cpu_to_le32(ufs->cmd_desc_addr);
	ufs->utrd_addr->rsp_upiu_off = cpu_to_le16(offsetof(struct ufs_cmd_desc, response_upiu));
	ufs->utrd_addr->rsp_upiu_len= cpu_to_le16(ALIGNED_UPIU_SIZE);

	writel(ufs->utmrd_addr, (ufs->ioaddr + REG_UTP_TASK_REQ_LIST_BASE_L));
	writel(0, (ufs->ioaddr + REG_UTP_TASK_REQ_LIST_BASE_H));

	writel(ufs->utrd_addr, (ufs->ioaddr + REG_UTP_TRANSFER_REQ_LIST_BASE_L));
	writel(0, (ufs->ioaddr + REG_UTP_TRANSFER_REQ_LIST_BASE_H));

	return CMD_RET_SUCCESS;
}


static int ufs_vendor_setup(struct ufs_host *ufs)
{
	/* DMA little endian, order change */
	writel(0xa, (ufs->ioaddr + VS_DATA_REORDER));

	writel(1, (ufs->ioaddr + REG_UTP_TASK_REQ_LIST_RUN_STOP));

	writel(1, (ufs->ioaddr + REG_UTP_TRANSFER_REQ_LIST_RUN_STOP));

	writel(UFS_SG_BLOCK_SIZE_BIT, (ufs->ioaddr + VS_TXPRDT_ENTRY_SIZE));
	writel(UFS_SG_BLOCK_SIZE_BIT, (ufs->ioaddr + VS_RXPRDT_ENTRY_SIZE));
	writel(0, (ufs->ioaddr + VS_TO_CNT_DIV_VAL));
	writel(100, (ufs->ioaddr + VS_40US_TO_CNT_VAL));

	writel(0xFFFFFFFF, (ufs->ioaddr + VS_UTRL_NEXUS_TYPE));
	writel(0xFFFFFFFF, (ufs->ioaddr + VS_UMTRL_NEXUS_TYPE));

	return CMD_RET_SUCCESS;
}

static struct ufs_uic_cmd mphy_unipro_setup[] =
{
	{UIC_CMD_DME_SET, UIC_ARG_MIB(0x9514), 0, UNIPRO_MCLK_SRC_PERIOD_NS},	/* DBG_CLK_PERIOD */
	{UIC_CMD_DME_SET, UIC_ARG_MIB(0x402), 0, 0x02},
	/* PHY_PMA_COMN *//* M-PHY Spec p127 talbe49 */
	{PHY_PMA_COMN_SET, MPHY_PMA_COMN_ADDR(0x00f), 0xfa, 0},
	{PHY_PMA_COMN_SET, MPHY_PMA_COMN_ADDR(0x010), 0x82, 0},
	{PHY_PMA_COMN_SET, MPHY_PMA_COMN_ADDR(0x011), 0x1e, 0},
	{PHY_PMA_COMN_SET, MPHY_PMA_COMN_ADDR(0x017), 0x84, 0},
	/* PHY_PMA_TRSV, Lane0*//* M-PHY Spec p127 talbe49 */
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x035, 0), 0x58, 0},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x036, 0), 0x32, 0},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x037, 0), 0x40, 0},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x03b, 0), 0x83, 0},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x042, 0), 0x88, 0},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x043, 0), 0xa6, 0},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x048, 0), 0x74, 0},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x04c, 0), 0x5b, 0},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x04d, 0), 0x83, 0},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x05c, 0), 0x14, 0},

	/* PHY_PMA_TRSV, Lane1 */
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x035, 1), 0x58, 0},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x036, 1), 0x32, 0},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x037, 1), 0x40, 0},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x03b, 1), 0x83, 0},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x042, 1), 0x88, 0},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x043, 1), 0xa6, 0},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x048, 1), 0x74, 0},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x04c, 1), 0x5b, 0},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x04d, 1), 0x83, 0},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x05c, 1), 0x14, 0},

	/* UniproSetMinimalCodeForMPHY() */
	{UIC_CMD_REGISTER_SET, 0x10c8, 0x01, 0},		/* rH_UNIPRO_DME_PACP_CNFBIT */

	{UIC_CMD_DME_SET, UIC_ARG_MIB(0x9540), 0, 0x01},	/* DBG_OV_TM */

	/* Lane0 *//* UFS 2.0 spec */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x8b, 0), 0, 0x4f},	/* RX_HS_G1SYNC_LENGTH_CAPABILITY */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x94, 0), 0, 0x4f},	/* RX_HS_G2SYNC_LENGTH_CAPABILITY */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x95, 0), 0, 0x4f},	/* RX_HS_G3SYNC_LENGTH_CAPABILITY */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x8c, 0), 0, 0x4f},	/* RX_HS_G1PREPARE_LENGTH_CAPABILITY */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x96, 0), 0, 0x4f},	/* RX_HS_G2PREPARE_LENGTH_CAPABILITY */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x97, 0), 0, 0x4f},	/* RX_HS_G3PREPARE_LENGTH_CAPABILITY */

	/* Lane1 *//* UFS 2.0 spec */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x8b, 1), 0, 0x4f},	/* RX_HS_G1SYNC_LENGTH_CAPABILITY */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x94, 1), 0, 0x4f},	/* RX_HS_G2SYNC_LENGTH_CAPABILITY */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x95, 1), 0, 0x4f},	/* RX_HS_G3SYNC_LENGTH_CAPABILITY */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x8c, 1), 0, 0x4f},	/* RX_HS_G1PREPARE_LENGTH_CAPABILITY */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x96, 1), 0, 0x4f},	/* RX_HS_G2PREPARE_LENGTH_CAPABILITY */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x97, 1), 0, 0x4f},	/* RX_HS_G3PREPARE_LENGTH_CAPABILITY */

	/* Lane0 */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x28c, 0), 0, (uHigh_Z_Cnt>>8)&0x0F},	/* TX_HIGH_Z_CNT_11_8 */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x28d, 0), 0, (uHigh_Z_Cnt)&0xFF},	/* TX_HIGH_Z_CNT_7_0 */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x28e, 0), 0, (0x80|((uTx_Sleep_Cnt)&0x7F))},	/* TX_OV_SLEEP_CNT_TIMER_6_0 */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x293, 0), 0, (uTx_Base_Unit)&0xFF},	/* TX_BASE_NVALUE_7_0 */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x294, 0), 0, (uTx_Base_Unit>>8)&0xFF},	/* TX_BASE_NVALUE_15_8 */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x295, 0), 0, (uTx_Gran_Unit)&0xFF},	/* TX_GRAN_NVALUE_7_0 */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x296, 0), 0, (uTx_Gran_Unit>>8)&0x03},	/* TX_GRAN_NVALUE_10_8 */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x340, 0), 0, uRx_SleepCntTimer},	/* RX_OV_SLEEP_CNT_TIMER_6_0 */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x341, 0), 0, uRx_StallCntTimer},	/* RX_OV_STALL_CND_TIMER_7_0 */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x355, 0), 0, (uRx_Base_Unit)&0xff},	/* RX_BASE_NVALUE_7_0 */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x354, 0), 0, (uRx_Base_Unit>>8)&0xff},		/* RX_BASE_NVALUE_15_8 */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x353, 0), 0, (uRx_Gran_Unit)&0xff},		/* RX_GRAN_NVALUE_7_0 */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x352, 0), 0, (uRx_Gran_Unit>>8)&0x03},	/* RX_GRAN_NVALUE_10_8 */

	/* Lane1 */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x28c, 1), 0, (uHigh_Z_Cnt>>8)&0x0F},	/* TX_HIGH_Z_CNT_11_8 */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x28d, 1), 0, (uHigh_Z_Cnt)&0xFF},	/* TX_HIGH_Z_CNT_7_0 */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x28e, 1), 0, (0x80|((uTx_Sleep_Cnt)&0x7F))},	/* TX_OV_SLEEP_CNT_TIMER_6_0 */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x293, 1), 0, (uTx_Base_Unit)&0xFF},	/* TX_BASE_NVALUE_7_0 */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x294, 1), 0, (uTx_Base_Unit>>8)&0xFF},	/* TX_BASE_NVALUE_15_8 */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x295, 1), 0, (uTx_Gran_Unit)&0xFF},	/* TX_GRAN_NVALUE_7_0 */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x296, 1), 0, (uTx_Gran_Unit>>8)&0x03},	/* TX_GRAN_NVALUE_10_8 */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x340, 1), 0, uRx_SleepCntTimer},	/* RX_OV_SLEEP_CNT_TIMER_6_0 */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x341, 1), 0, uRx_StallCntTimer},	/* RX_OV_STALL_CND_TIMER_7_0 */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x355, 1), 0, (uRx_Base_Unit)&0xff},	/* RX_BASE_NVALUE_7_0 */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x354, 1), 0, (uRx_Base_Unit>>8)&0xff},		/* RX_BASE_NVALUE_15_8 */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x353, 1), 0, (uRx_Gran_Unit)&0xff},		/* RX_GRAN_NVALUE_7_0 */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x352, 1), 0, (uRx_Gran_Unit>>8)&0x03},	/* RX_GRAN_NVALUE_10_8 */

	/* Lane0 */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x033, 0), 0, 0x0a},	/* 0xA = 1ms */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x27d, 0), 0, uTx_linereset_pvalue},	/* Tx Linereset Pvalue */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x317, 0), 0, uRx_linereset_pvalue},	/* Rx Linereset Pvalue */

	/* Lane1 */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x033, 1), 0, 0x0a},	/* 0xA = 1ms */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x27d, 1), 0, uTx_linereset_pvalue},	/* Tx Linereset Pvalue */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x317, 1), 0, uRx_linereset_pvalue},	/* Rx Linereset Pvalue */
	
	{UIC_CMD_DME_SET, UIC_ARG_MIB(0x9564), 0, 0x20103},	/* DBG_PA_OPTION_SUITE */

	/* UniproSetHostUniProAttr() */
	{UIC_CMD_DME_SET, UIC_ARG_MIB(0x3000), 0, 0x00},	/* N_DeviceID */
	{UIC_CMD_DME_SET, UIC_ARG_MIB(0x3001), 0, 0x01},	/* N_DeviceID_valid */
	{UIC_CMD_DME_SET, UIC_ARG_MIB(0x4021), 0, 0x01},	/* T_PeerDeviceID */

	//T_CPortFlags[0] : if(1) E2E FC ON, else if(0) E2E FC OFF
	//T_CPortFlags[1] : if(1) CSD OFF, else if(0) CSD ON
	{UIC_CMD_DME_SET, UIC_ARG_MIB(0x4025), 0, 0x06},	/* T_CPortFlags */
	{UIC_CMD_DME_SET, UIC_ARG_MIB(0x4023), 0, 0x00},	/* T_TrafficClass */
	{UIC_CMD_DME_SET, UIC_ARG_MIB(0x4020), 0, 0x01},	/* T_ConnectionState */

	/* UniproSetHostUniProAttr() */
	{UIC_CMD_DME_SET, UIC_ARG_MIB(0x9536), 0, 0x222e},
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x316, 0), 0, 0x02},	/* UFS_MPHY_RX_FILLER_ENABLE */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x316, 1), 0, 0x02},	/* UFS_MPHY_RX_FILLER_ENABLE */

	{UIC_CMD_DME_SET, UIC_ARG_MIB(0x9540), 0, 0x00},	/* DBG_OV_TM */

	/* NULL TERMINATION */
	{0,0,0,0}
};

static struct ufs_uic_cmd mphy_unipro_post_setup[] =
{
	/*set after link_startup*/
	{UIC_CMD_DME_SET, UIC_ARG_MIB(0x9529), 0, 0x01},	/* PA_DBG_MODE */
	{UIC_CMD_DME_SET, UIC_ARG_MIB(0x15a4), 0, 0xbb8},	/* PA_SaveConfigTime */
	{UIC_CMD_DME_SET, UIC_ARG_MIB(0x9529), 0, 0x00},	/* PA_DBG_MODE */

	/* NULL TERMINATION */
	{0,0,0,0}
};

static struct ufs_uic_cmd mphy_unipro_gear_change[] =
{
	{UIC_CMD_DME_SET, UIC_ARG_MIB(0x1568), 0, UFS_GEAR},	/* PA_TxGear */
	{UIC_CMD_DME_SET, UIC_ARG_MIB(0x1583), 0, UFS_GEAR},	/* PA_RxGear */
	{UIC_CMD_DME_SET, UIC_ARG_MIB(0x1560), 0, UFS_LANE},	/* PA_ActiveTxLane */
	{UIC_CMD_DME_SET, UIC_ARG_MIB(0x1580), 0, UFS_LANE},	/* PA_ActiveRxLane */
#if ((UFS_POWER_MODE==1) || (UFS_POWER_MODE==4))
/* FAST Mode */
	{UIC_CMD_DME_SET, UIC_ARG_MIB(0x156a), 0, UFS_RATE},	/* PA_HSSeries */

	/* Set Termination for HS mode */
	{UIC_CMD_DME_SET, UIC_ARG_MIB(0x1569), 0, 0x01},	/* PA_TxTermination */
	{UIC_CMD_DME_SET, UIC_ARG_MIB(0x1584), 0, 0x01},	/* PA_RxTermination */
#if (UFS_RATE == 1)	/* Series A */
	{UIC_CMD_DME_SET, UIC_ARG_MIB(0x9540), 0, 0x01},	/* DBG_OV_TM */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x362, 0), 0, 0xff},/*enable Synmbol error masking*/
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x363, 0), 0, 0x00},
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x321, 0), 0, uRx_sync_pattern},
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x362, 1), 0, 0xff},/*enable Synmbol error masking*/
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x363, 1), 0, 0x00},
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x321, 1), 0, uRx_sync_pattern},
	{UIC_CMD_DME_SET, UIC_ARG_MIB(0x9540), 0, 0x00},	/* DBG_OV_TM */
	{PHY_PMA_COMN_SET, MPHY_PMA_COMN_ADDR(0x0f), 0xfa, 0x00},
	{PHY_PMA_COMN_SET, MPHY_PMA_COMN_ADDR(0x10), 0x82, 0x00},
	{PHY_PMA_COMN_SET, MPHY_PMA_COMN_ADDR(0x11), 0x1e, 0x00},
	{PHY_PMA_COMN_SET, MPHY_PMA_COMN_ADDR(0x16), 0xff, 0x00},
	{PHY_PMA_COMN_SET, MPHY_PMA_COMN_ADDR(0x15), 0x80, 0x00},
	{PHY_PMA_COMN_SET, MPHY_PMA_COMN_ADDR(0x17), 0x84, 0x00},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x36, 0), 0x32, 0x00},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x37, 0), 0x40, 0x00},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x42, 0), 0x88, 0x00},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x43, 0), 0xa6, 0x00},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x48, 0), 0x74, 0x00},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x38, 0), 0x3f, 0x00},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x36, 1), 0x32, 0x00},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x37, 1), 0x40, 0x00},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x42, 1), 0x88, 0x00},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x43, 1), 0xa6, 0x00},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x48, 1), 0x74, 0x00},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x38, 1), 0x3f, 0x00},
#if (UFS_GEAR == 2)
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x34, 0), 0x35, 0x00},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x35, 0), 0x5b, 0x00},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x34, 1), 0x35, 0x00},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x35, 1), 0x5b, 0x00},
#endif
#elif (UFS_RATE == 2)	/* Series B */
	{UIC_CMD_DME_SET, UIC_ARG_MIB(0x9540), 0, 0x01},	/* DBG_OV_TM */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x362, 0), 0, 0xff},/*enable Synmbol error masking*/
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x363, 0), 0, 0x00},
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x321, 0), 0, uRx_sync_pattern},
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x362, 1), 0, 0xff},/*enable Synmbol error masking*/
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x363, 1), 0, 0x00},
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x321, 1), 0, uRx_sync_pattern},
#if (UFS_GEAR==1)
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x42, 0), 0x88, 0x00},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x42, 1), 0x88, 0x00},
#elif (UFS_GEAR==2)
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x42, 0), 0xbb, 0x00},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x34, 0), 0x36, 0x00},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x35, 0), 0x5c, 0x00},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x42, 1), 0xbb, 0x00},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x34, 1), 0x36, 0x00},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x35, 1), 0x5c, 0x00},
#endif
	{UIC_CMD_DME_SET, UIC_ARG_MIB(0x9540), 0, 0x00},	/* DBG_OV_TM */
	{PHY_PMA_COMN_SET, MPHY_PMA_COMN_ADDR(0x0f), 0xfa, 0x00},
	{PHY_PMA_COMN_SET, MPHY_PMA_COMN_ADDR(0x10), 0x82, 0x00},
	{PHY_PMA_COMN_SET, MPHY_PMA_COMN_ADDR(0x11), 0x1e, 0x00},
	{PHY_PMA_COMN_SET, MPHY_PMA_COMN_ADDR(0x16), 0xff, 0x00},
	{PHY_PMA_COMN_SET, MPHY_PMA_COMN_ADDR(0x15), 0x80, 0x00},
	{PHY_PMA_COMN_SET, MPHY_PMA_COMN_ADDR(0x17), 0x84, 0x00},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x36, 0), 0x32, 0x00},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x37, 0), 0x40, 0x00},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x43, 0), 0xa6, 0x00},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x48, 0), 0x74, 0x00},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x38, 0), 0x3f, 0x00},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x36, 1), 0x32, 0x00},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x37, 1), 0x40, 0x00},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x43, 1), 0xa6, 0x00},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x48, 1), 0x74, 0x00},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x38, 1), 0x3f, 0x00},
#endif
#else
 /* Slow Mode */
	/* Termination off */
	{UIC_CMD_DME_SET, UIC_ARG_MIB(0x1569), 0, 0x00},	/* PA_TxTermination */
	{UIC_CMD_DME_SET, UIC_ARG_MIB(0x1584), 0, 0x00},	/* PA_RxTermination */
	{UIC_CMD_DME_SET, UIC_ARG_MIB(0x9540), 0, 0x01},	/* DBG_OV_TM */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x337, 0), 0, 0x21},	/* Use DIF-P when Entering Hibernation-LANE0 */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x337, 1), 0, 0x21},	/* Use DIF-P when Entering Hibernation-LANE1 */
	{UIC_CMD_DME_SET, UIC_ARG_MIB(0x9540), 0, 0x00},	/* DBG_OV_TM */
#endif
	{UIC_CMD_DME_SET, UIC_ARG_MIB(0x2041), 0, 65535},			/* DL_FC0ProtTimeOutVal */
	{UIC_CMD_DME_SET, UIC_ARG_MIB(0x2042), 0, 32767},			/* DL_TC0ReplayTimeOutVal */
	{UIC_CMD_DME_SET, UIC_ARG_MIB(0x2043), 0, 8191},			/* DL_AFC0ReqTimeOutVal */
	{UIC_CMD_DME_SET, UIC_ARG_MIB(0x15b0), 0, 65535},			/* PA_PWRModeUserData0 */
	{UIC_CMD_DME_SET, UIC_ARG_MIB(0x15b1), 0, 32767},			/* PA_PWRModeUserData1 */
	{UIC_CMD_DME_SET, UIC_ARG_MIB(0x15b2), 0, 8191},			/* PA_PWRModeUserData2 */


	{UIC_CMD_REGISTER_SET, 0x1094, 65535, 0},		/* rH_UNIPRO_DME_POWERMODE_REQ_LOCALL2TIMER0 */
	{UIC_CMD_REGISTER_SET, 0x1098, 32767, 0},		/* rH_UNIPRO_DME_POWERMODE_REQ_LOCALL2TIMER1 */
	{UIC_CMD_REGISTER_SET, 0x109c, 8191, 0},		/* rH_UNIPRO_DME_POWERMODE_REQ_LOCALL2TIMER2 */
	{UIC_CMD_REGISTER_SET, 0x10a4, 8191, 0},		/* rH_UNIPRO_DME_POWERMODE_REQ_REMOTEL2TIMER0 */
	{UIC_CMD_REGISTER_SET, 0x10a8, 65535, 0},		/* rH_UNIPRO_DME_POWERMODE_REQ_REMOTEL2TIMER1 */
	{UIC_CMD_REGISTER_SET, 0x10ac, 32767, 0},		/* rH_UNIPRO_DME_POWERMODE_REQ_REMOTEL2TIMER2 */
	{UIC_CMD_REGISTER_SET, 0x1150, 0x22, 0},		/* UNIPRO_DME_CONTROL_STATE */

//	{UIC_CMD_DME_GET, UIC_ARG_MIB(0x1571), 0, 0},	/* PA_PWRMode */
	{UIC_CMD_DME_SET, UIC_ARG_MIB(0x1571), 0, UFS_RXTX_POWER_MODE},	/* PA_PWRMode */
	{UIC_CMD_WAIT_ISR, REG_INTERRUPT_STATUS, (1<<4), 0},	/* PA_PWRMode */

#if (UFS_LANE==1)
	{UIC_CMD_DME_SET, UIC_ARG_MIB(0x9519), 0, 0x01},	/* DBG_RXPHY_CFGUPDT */
#endif

#if ((UFS_POWER_MODE==1) || (UFS_POWER_MODE==4))
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x04d, 0), 0x83, 0},
	{PHY_PMA_TRSV_SET, MPHY_PMA_TRSV_ADDR(0x04d, 1), 0x83, 0},
	{PHY_PMA_COMN_SET, MPHY_PMA_COMN_ADDR(0x15), 0x00, 0x00},
	{UIC_CMD_DME_SET, UIC_ARG_MIB(0x9540), 0, 0x01},	/* DBG_OV_TM */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x363, 0), 0, 0x00},
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x363, 1), 0, 0x00},
	{UIC_CMD_DME_SET, UIC_ARG_MIB(0x9540), 0, 0x00},	/* DBG_OV_TM */
	{PHY_PMA_COMN_WAIT, MPHY_PMA_COMN_ADDR(0x1e), 1<<5, 0},
	{PHY_PMA_TRSV_WAIT, MPHY_PMA_TRSV_ADDR(0x5e, 0), 1<<4, 0},
#else
	{UIC_CMD_DME_SET, UIC_ARG_MIB(0x9540), 0, 0x01},	/* DBG_OV_TM */
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x363, 0), 0, 0x00},
	{UIC_CMD_DME_SET, UIC_ARG_MIB_SEL(0x363, 1), 0, 0x00},
	{UIC_CMD_DME_SET, UIC_ARG_MIB(0x9540), 0, 0x00},	/* DBG_OV_TM */
#endif
#ifdef SCSI_UFS_DEBUG
	{UIC_CMD_DME_GET, UIC_ARG_MIB(0x1571), 0, 0},		/* PA_PWRMode */
	{UIC_CMD_DME_GET, UIC_ARG_MIB(0x1568), 0, 0},		/* PA_TxGear */
	{UIC_CMD_DME_GET, UIC_ARG_MIB(0x1583), 0, 0},		/* PA_RxGear */
	{UIC_CMD_DME_GET, UIC_ARG_MIB(0x156a), 0, 0},		/* PA_HSSeries */
	{UIC_CMD_DME_PEER_GET, UIC_ARG_MIB(0x156a), 0, 0},	/* PA_HSSeries */
#endif

	/* NULL TERMINATION */
	{0,0,0,0}
};

/*
	3. called by driver/block/scsi_ufs.c
*/
int ufs_board_init(int host_index, struct ufs_host *ufs)
{
	u32 reg, err;

	if (host_index) {
		printf("Currently multi UFS host is not supported!\n");
		return -1;
	}

	sprintf(ufs->host_name,"ufs%d", host_index);
	ufs->irq = 200;
	ufs->ioaddr = EXYNOS7420_UFS_BASE;
	ufs->phy_pma = EXYNOS7420_UFS_BASE + 0x1800;
	ufs->host_index = host_index;
	ufs->quirks = UFS_QUIRK_BROKEN_HCE;

	ufs->ufs_board_pre_setup = ufs_pre_setup;
	ufs->ufs_board_post_setup = ufs_vendor_setup;
	ufs->ufs_hw_reset = ufs_device_reset;
	ufs->ufs_device_power = ufs_device_power;

	ufs->pre_uic_cmd = mphy_unipro_setup;
	ufs->post_uic_cmd = mphy_unipro_post_setup;
	ufs->pwr_mode_change = mphy_unipro_gear_change;
	set_ufs_clk(host_index);
	err = exynos_pinmux_config(PERIPH_ID_UFS0, PINMUX_FLAG_NONE);
	return 0;
}
