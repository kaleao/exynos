/*
 * Clock setup for EXYNOS7420
 *
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

#define MMC_BOOT_CLK
/* #define __KERNEL__ */

#ifdef DV /* dv simulation */
#define WAIT_STATUS(_addr1, _addr2)
#define WAIT_STATUS_VAL(_val, _addr2)
#define WAIT_STATUS_VAL_MASK(_val, _addr2, _mask)
#define WAIT_PLL_LOCK(_addr)
#else
#define WAIT_STATUS(_addr1, _addr2)						while (Inp32(_addr1) != Inp32(_addr2)){}
#define WAIT_STATUS_VAL(_val, _addr2)					while ((_val) != Inp32(_addr2)){}
#define WAIT_STATUS_VAL_MASK(_val, _addr2, _mask)		while ((_val) != (Inp32(_addr2)&(_mask))){}
#define WAIT_PLL_LOCK(_addr)							do { \
															bits0 = Inp32(_addr); \
														} while ((bits0>>29)==0);
#endif


#include "clk-boot.h"
#include "config.h"

#ifdef __KERNEL__
#include <config.h>
#include <version.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/power.h>
#include <asm/arch/cpu.h>
#include <asm/arch/gpio.h>

void system_clock_init(void)
{
	struct exynos7420_power *pmu = (struct exynos7420_power *)EXYNOS7420_POWER_BASE;
	volatile unsigned int eBootStat = readl(&pmu->inform1);

	if (eBootStat != S5P_CHECK_SLEEP) /* cold boot */
		clk_init(0);
	else /* resume */
		clk_init(1);
}
#endif /*#ifdef __KERNEL__ */

#define ATLAS_PLL_MHZ 	1200 		/* 2400 MHz, 2300, ..., 200, 100 */
#define APOLLO_PLL_MHZ 	1200 		/* 1600 MHz, 1500, ..., 200, 100 */
#define MIF_PLL_MHZ 	(MEMCLK << 1)	/* 3304 MHz, 3200, 3104, 3100, 2912, 2528, 2136, 2052, 1656, 1264, 1086, 832, 696, 552, 334 */
#define CCI_PLL_MHZ		532			/* 532 MHz, 528, 266 */
#define BUS0_PLL_MHZ	1600		/* 1600 MHz, 800 */
#define BUS1_PLL_MHZ	668			/* 668 MHz, 334 */
#define MFC_PLL_MHZ		468			/* 468 MHz, 234 (UFS 26MHz) */
#define AUD_PLL_MHZ		196			/* 492 MHz(491.520), 196(196.698) */
#define ISP_PLL_MHZ		552			/* 590 MHz, 560, 552, 492, 432 */
#define CAM_PLL_MHZ		660			/* 690 MHz, 660, 532 */
#define G3D_PLL_MHZ		266			/* 772 MHz, 600, 532, 420, 350, 266, 160, 100 */
#define HSIC_PLL_MHZ	480
/* #define DISP_PLL_MHZ */			/* DISP_PLL freq is determined by DISP_RESOL */
#define DPHY_PLL_MHZ	1200		/* 1200 MHz, 1500 */

#define ATLAS_PLL_ON	1
#define APOLLO_PLL_ON	1
#define MIF_PLL_ON		1
#define CCI_PLL_ON		1
#define BUS0_PLL_ON		1
#define BUS1_PLL_ON		1
#define MFC_PLL_ON		1
#define AUD_PLL_ON		1
#define ISP_PLL_ON		0 /* off */
#define CAM_PLL_ON		0 /* off */
#define DISP_PLL_ON		1
#define G3D_PLL_ON		1
#define HSIC_PLL_ON		0 /* off */
#define DPHY_PLL_ON		0 /* off */
#define ACLK_FSYS1_200_FIX	1

#ifndef DISP_RESOL
#define DISP_RESOL 2  /* 1=INT_FHD_EXT_UHD(without MIC), 2=INT_WQHD_EXT_UHD(1/2 MIC), 3=INT_WQXGA_EXT_UHD(1/2 MIC) */
#endif

#if (DISP_RESOL == 1)
	#define DISP_PLL_MHZ						142
	#define SCLK_DECON_INT_ECLK_SEL  				0	// 400MHz = BUS0_PLL 400MHz
	#define SCLK_DECON_INT_ECLK_RATIO  			3	// 100MHz = BUS0_PLL 400MHz/(3+1)
	#define SCLK_DECON_INT_ECLK_SUB_RATIO		0	// 100MHz = SCLK_DECON_INT_ECLK/(0+1)
	#define SCLK_DECON_INT_EXTCLKPLL_SUB_RATIO	0	/* 142MHz = DISP_PLL 142MHz/(0+1), VCLK */
	#define SCLK_DECON_EXT_ECLK_RATIO			3	/* 167MHz = BUS1_PLL 668MHz/(3+1) */
	#define SCLK_DSD_RATIO						3	/* 167MHz = BUS1_PLL 668MHz/(3+1) */
	#define SCLK_HDMI_SPDIF_RATIO				3	/* 100MHz = BUS0_PLL 400MHz/(3+1) */
#elif (DISP_RESOL == 2)
	#define DISP_PLL_MHZ						126//252
	#define SCLK_DECON_INT_ECLK_SEL  				1	// 668MHz = BUS1_PLL 668MHz
	#define SCLK_DECON_INT_ECLK_RATIO  			3	// 167MHz = BUS1_PLL 668MHz/(3+1)
	#define SCLK_DECON_INT_ECLK_SUB_RATIO		0	// 167MHz = SCLK_DECON_INT_ECLK/(0+1)
	#define SCLK_DECON_INT_EXTCLKPLL_SUB_RATIO	0	/* 126MHz = DISP_PLL 126MHz/(0+1), VCLK */
	#define SCLK_DECON_EXT_ECLK_RATIO			3	/* 167MHz = BUS1_PLL 668MHz/(3+1) */
	#define SCLK_DSD_RATIO						3	/* 167MHz = BUS1_PLL 668MHz/(3+1) */
//	#define SCLK_HDMI_SPDIF_RATIO				3	/* 100MHz = BUS0_PLL 400MHz/(3+1) */ //400
	#define SCLK_HDMI_SPDIF_RATIO				7	/* 100MHz = BUS0_PLL 800MHz/(7+1) */ // 800
#elif (DISP_RESOL == 3)
	#define DISP_PLL_MHZ						134
	#define SCLK_DECON_INT_ECLK_SEL  				1	// 668MHz = BUS1_PLL 668MHz
	#define SCLK_DECON_INT_ECLK_RATIO  			3	// 167MHz = BUS1_PLL 668MHz/(3+1)
	#define SCLK_DECON_INT_ECLK_SUB_RATIO		0	/* 134MHz = DISP_PLL 134MHz/(0+1) */
	#define SCLK_DECON_INT_EXTCLKPLL_SUB_RATIO	0	/* 134MHz = DISP_PLL 134MHz/(0+1), VCLK */
	#define SCLK_DECON_EXT_ECLK_RATIO			3	/* 167MHz = BUS1_PLL 668MHz/(3+1) */
	#define SCLK_DSD_RATIO						3	/* 167MHz = BUS1_PLL 668MHz/(3+1) */
//	#define SCLK_HDMI_SPDIF_RATIO				3	/* 100MHz = BUS0_PLL 400MHz/(3+1) */ //400
	#define SCLK_HDMI_SPDIF_RATIO				7	/* 100MHz = BUS0_PLL 800MHz/(7+1) */ // 800
#else
	#error "undefined disp resolution"
#endif


#define Outp32(_addr, _data)	*(volatile unsigned int *)(_addr) = (_data)
#define Inp32(_addr) 			*(volatile unsigned int *)(_addr)

int clk_init(unsigned int cmu_resume)
{
	unsigned int bits0, bits1;
	int kdiv;

	/* To fix the problem when using C2 */
	Outp32(0x11801000, 0x00); /* ATLAS_ARMCLK_STOPCTRL */
	Outp32(0x11901000, 0x00); /* APOLLO_ARMCLK_STOPCTRL */


/* ********** Turn off ********** */
	Outp32(0x105D0200, 0x00000000); /* CLK_MUX_SEL_TOP00 */
	WAIT_STATUS(0x105D0200, 0x105D0400); /* CLK_MUX_STAT_TOP00 */

	Outp32(0x105E0200, 0x00000000); /* CLK_MUX_SEL_TOP10 */
	WAIT_STATUS(0x105E0200, 0x105E0400); /* CLK_MUX_STAT_TOP10 */

	/* Apply F/F setting before pll mux setting to guarautee post-dividers margine */
	/* TOPC_BUS0_PLL=400MHz=1600MHz/4 */
	Outp32(0x10570200, 0x00020000); /* CLK_MUX_SEL_TOPC0 */
	WAIT_STATUS(0x10570200, 0x10570400); /* CLK_MUX_STAT_TOPC0 */

	/* TOP0,1 BUS0_PLL=800MHz=1600MHz/2 in TOPC */
	Outp32(0x10570204, 0x00111110); /*  CLK_MUX_SEL_TOPC1 */
	WAIT_STATUS(0x10570204, 0x10570404); /* CLK_MUX_STAT_TOPC1 */

	/* TOP0_BUS0_PLL=400MHz=800MHz/2 in TOP0 */
	Outp32(0x105D0204, 0x00010000); /* CLK_MUX_SEL_TOP01 */
	WAIT_STATUS(0x105D0204, 0x105D0404); /* CLK_MUX_STAT_TOP01 */

	/* TOP1_BUS0_PLL=800MHz=800MHz/1 in TOP1 */
	Outp32(0x105E0204, 0x00000000); /* CLK_MUX_SEL_TOP11 */
	WAIT_STATUS(0x105E0204, 0x105E0404); /* CLK_MUX_STAT_TOP11 */




/* ********** CMU_TOPC ********** */
#if (CCI_PLL_MHZ == 532)
	bits1 = (133 << 16) | (6 << 8) | (0 << 0); /* CCI_PLL=532MHz */
#elif (CCI_PLL_MHZ == 528)
	bits1 = (66 << 16) | (3 << 8) | (0 << 0); /* CCI_PLL=528MHz */
#elif (CCI_PLL_MHZ == 266)
	bits1 = (133 << 16) | (6 << 8) | (1 << 0); /* CCI_PLL=266MHz */
#else
	/* printf("check variable &CCI_PLL_MHZ"); */
	while (1);
#endif
	bits0 = ((bits1 >> 8) & 0x3F) * 150;
	Outp32(0x10570000, bits0); /*  CCI_PLL_LOCK */

	/* ENABLE_SDIV_CLKSTOP=0, ENABLE_MDIV_CLKSTOP=1 */
	bits1 = (bits1) | (0 << 26) | (1 << 5);
#if (CCI_PLL_ON != 0)
	bits1 = (bits1) | (1 << 31);
#endif
	Outp32(0x10570100, bits1); /* CCI_PLL_CON0(PLL1452X) */
#if (CCI_PLL_ON != 0)
	WAIT_PLL_LOCK(0x10570100); /* CCI_PLL_CON0 */
#endif


#if (BUS0_PLL_MHZ == 1600)
	bits1 = (200 << 16) | (3 << 8) | (0 << 0); /* BUS0_PLL=1600MHz */
#elif (BUS0_PLL_MHZ == 800)
	bits1 = (200 << 16) | (3 << 8) | (1 << 0); /* BUS0_PLL=800MHz */
#else
	/* printf("check variable &BUS0_PLL_MHZ"); */
	while (1);
#endif
	bits0 = ((bits1 >> 8) & 0x3F) * 150;
	Outp32(0x10570004, bits0); /* BUS0_PLL_LOCK */

	bits0 = Inp32(0x10570114); /* BUS0_PLL_CON1 */
	bits0 = bits0 & ~(0x3 << 6);
	bits0 = bits0 | (2 << 6);
	Outp32(0x10570114, bits0); /* BUS0_PLL_CON1 */

	/* ENABLE_SDIV_CLKSTOP=0, ENABLE_MDIV_CLKSTOP=1 */
	bits1 = (bits1) | (0 << 26) | (1 << 5);
#if (BUS0_PLL_ON != 0)
	bits1 = (bits1) | (1 << 31);
#endif
	Outp32(0x10570110, bits1); /* BUS0_PLL_CON0(PLL_1451X) */
#if (BUS0_PLL_ON != 0)
	WAIT_PLL_LOCK(0x10570110); /* BUS0_PLL_CON0 */
#endif


#if (BUS1_PLL_MHZ == 668)
	bits1 = (167 << 16) | (6 << 8) | (0 << 0); /* BUS1_PLL=668MHz */
#elif (BUS1_PLL_MHZ == 334)
	bits1 = (167 << 16) | (6 << 8) | (1 << 0); /* BUS1_PLL=334MHz */
#else
	/* printf("check variable &BUS1_PLL_MHZ"); */
	while (1);
#endif
	bits0 = ((bits1 >> 8) & 0x3F) * 150;
	Outp32(0x10570008, bits0); /* BUS1_PLL_LOCK */

	/* ENABLE_SDIV_CLKSTOP=0, ENABLE_MDIV_CLKSTOP=1 */
	bits1 = (bits1) | (0 << 26) | (1 << 5);
#if (BUS1_PLL_ON != 0)
	bits1 = (bits1) | (1 << 31);
#endif
	Outp32(0x10570120, bits1); /* BUS1_PLL_CON0(PLL1452X) */
#if (BUS1_PLL_ON != 0)
	WAIT_PLL_LOCK(0x10570120); /* BUS1_PLL_CON0 */
#endif


#if (MFC_PLL_MHZ == 468)
	bits1 = (117 << 16) | (3 << 8) | (1 << 0); /* MFC_PLL=468MHz */
#elif (MFC_PLL_MHZ == 234)
	bits1 = (117 << 16) | (3 << 8) | (2 << 0); /* MFC_PLL=234MHz */
#else
	/* printf("check variable &MFC_PLL_MHZ"); */
	while (1);
#endif
	bits0 = ((bits1 >> 8) & 0x3F) * 150;
	Outp32(0x1057000C, bits0); /* MFC_PLL_LOCK */

	/* ENABLE_SDIV_CLKSTOP=0, ENABLE_MDIV_CLKSTOP=1 */
	bits1 = (bits1) | (0 << 26) | (1 << 5);
#if (MFC_PLL_ON != 0)
	bits1 = (bits1) | (1 << 31);
#endif
	Outp32(0x10570130, bits1); /* MFC_PLL_CON0(PLL1452X) */
#if (MFC_PLL_ON != 0)
	WAIT_PLL_LOCK(0x10570130); /* MFC_PLL_CON0 */
#endif


#if (AUD_PLL_MHZ == 492)
	bits1 = (20 << 16) | (1 << 8) | (0 << 0); /* AUD_PLL=492MHz (491.520MHz) */
	kdiv=31457;
#elif (AUD_PLL_MHZ == 196)
	bits1 = (33 << 16) | (1 << 8) | (2 << 0); /* AUD_PLL=196MHz (196.608MHz) */
	kdiv=-15204;
#else
	/* printf("check variable &AUD_PLL_MHZ"); */
	while (1);
#endif
	bits0 = ((bits1 >> 8) & 0x3F) * 3000; /* Fractional mode  */

	Outp32(0x10570010, bits0); /* AUD_PLL_LOCK */

	/* ENABLE_SDIV_CLKSTOP=0, ENABLE_MDIV_CLKSTOP=1 */
	bits1 = (bits1) | (0 << 26) | (1 << 5);
#if (AUD_PLL_ON != 0)
	bits1 = (bits1) | (1 << 31);
#endif
	Outp32(0x10570140, bits1); /* AUD_PLL_CON0(PLL1460X) */
	bits0 = (kdiv << 0);
	Outp32(0x10570144, bits0); /* AUD_PLL_CON1 */
#if (AUD_PLL_ON != 0)
	WAIT_PLL_LOCK(0x10570140); /* AUD_PLL_CON0 */
#endif

	/*
	ACLK_IMEM_100_RATIO	[31:28]  INT_DVFS
	ACLK_IMEM_200_RATIO	[27:24]  INT_DVFS
	ACLK_IMEM_266_RATIO	[23:20]  INT_DVFS
	ACLK_BUS1_200_RATIO	[19:16]  INT_DVFS
	ACLK_BUS1_532_RATIO	[15:12]  INT_DVFS
	ACLK_BUS0_532_RATIO	[11:8]  INT_DVFS
	ACLK_CCORE_133_RATIO	[7:4]  MIF_DVFS
	ACLK_CCORE_532_RATIO	[3:0]  MIF_DVFS
	*/
	bits0 = (3 << 28) | (3 << 24) | (1 << 20) | (1 << 16) | (0 << 12) | (0 << 8) | (4 << 4) | (0 << 0);
	Outp32(0x10570600, bits0); /* CLK_DIV_TOPC0 */

	/*
	0=MOUT_TOPC_BUS0_PLL, 1=MOUT_MOUT_TOPC_BUS1_PLL, 2=MOUT_TOPC_CCI_PLL, 3=MOUT_TOPC_MFC_PLL
	ACLK_IMEM_100_SEL	[29:28] INT_DVFS
	ACLK_IMEM_200_SEL	[25:24] INT_DVFS
	ACLK_IMEM_266_SEL	[21:20] INT_DVFS
	ACLK_BUS1_200_SEL	[17:16] INT_DVFS

	0=MOUT_TOPC_BUS0_PLL_HALF, 1=MOUT_TOPC_BUS1_PLL_CTRL, 2=MOUT_TOPC_CCI_PLL_CTRL, 3=MOUT_TOPC_MFC_PLL_CTRL,
	4=MOUT_TOPC_BUS0_PLL_HALF_DIV2, 5=MOUT_TOPC_BUS1_PLL_CTRL_DIV2, 6=MOUT_TOPC_CCI_PLL_CTRL_DIV2,
	7=MOUT_TOPC_MFC_PLL_CTRL_DIV2, 8=MOUT_TOPC_BUS0_PLL_HALF_DIV4, 9=MOUT_TOPC_BUS1_PLL_CTRL_DIV2,
	10=MOUT_TOPC_CCI_PLL_CTRL_DIV2, 11=MOUT_TOPC_MFC_PLL_CTRL_DIV4,
	ACLK_BUS1_532_SEL	[15:12] INT_DVFS
	ACLK_BUS0_532_SEL	[11:8] INT_DVFS

	0=MOUT_TOPC_BUS0_PLL, 1=MOUT_MOUT_TOPC_BUS1_PLL, 2=MOUT_TOPC_CCI_PLL, 3=MOUT_TOPC_MFC_PLL
	ACLK_CCORE_133_SEL	[5:4] MIF_DVFS

	0=MOUT_TOPC_BUS0_PLL_HALF, 1=MOUT_TOPC_BUS1_PLL_CTRL, 2=MOUT_TOPC_CCI_PLL_CTRL, 3=MOUT_TOPC_MFC_PLL_CTRL,
	4=MOUT_TOPC_BUS0_PLL_HALF_DIV2, 5=MOUT_TOPC_BUS1_PLL_CTRL_DIV2, 6=MOUT_TOPC_CCI_PLL_CTRL_DIV2,
	7=MOUT_TOPC_MFC_PLL_CTRL_DIV2, 8=MOUT_TOPC_BUS0_PLL_HALF_DIV4, 9=MOUT_TOPC_BUS1_PLL_CTRL_DIV2,
	10=MOUT_TOPC_CCI_PLL_CTRL_DIV2, 11=MOUT_TOPC_MFC_PLL_CTRL_DIV4,
	ACLK_CCORE_532_SEL	[3:0] MIF_DVFS
	*/

	bits0 = (0 << 28) | (2 << 24) | (0 << 20) | (0 << 16) | (0 << 12) | (0 << 8) | (0 << 4) | (2 << 0); // INT L7
//	bits0 = (0 << 28) | (1 << 24) | (2 << 20) | (0 << 16) | (2 << 12) | (2 << 8) | (0 << 4) | (2 << 0);
	Outp32(0x10570208, bits0); /* CLK_MUX_SEL_TOPC2 */

	/*
	PCLK_BUS01_133_RATIO	[31:28]
	ACLK_PERIS_66_RATIO	[27:24] INT_DVFS
	ACLK_MSCL_532_RATIO	[23:20] INT_DVFS
	ACLK_MFC_532_RATIO	[11:8] INT_DVFS
	*/
	bits0 = (3 << 28) | (5 << 24) | (0 << 20) | (1 << 8); // INT L7
//	bits0 = (2 << 28) | (5 << 24) | (0 << 20) | (0 << 8);
	Outp32(0x10570604, bits0); /* CLK_DIV_TOPC1 */

	/*
	0=MOUT_TOPC_BUS0_PLL, 1=MOUT_MOUT_TOPC_BUS1_PLL, 2=MOUT_TOPC_CCI_PLL, 3=MOUT_TOPC_MFC_PLL
	PCLK_BUS01_133_SEL	[29:28] INT_DVFS
	ACLK_PERIS_66_SEL	[25:24] INT_DVFS
	ACLK_MSCL_532_SEL	[21:20] INT_DVFS
	ACLK_MFC_532_SEL	[9:8] INT_DVFS
	*/
	bits0 = (0 << 28) | (0 << 24) | (0 << 20) | (1 << 8);  // INT L7
//	bits0 = (0 << 28) | (0 << 24) | (2 << 20) | (2 << 8);
	Outp32(0x1057020C, bits0); /* CLK_MUX_SEL_TOPC3 */

	/*
	SCLK_CMUC_PAUSE_RATIO	[7:4]
	SCLK_CMU_APBIF_RATIO	[3:0]
	*/
	bits0 = (3 << 4) | (3 << 0);
	Outp32(0x10570608, bits0); /* CLK_DIV_TOPC2 */

	/*
	0=OSCCLK, 1=SCLK_CMU_APBIF
	SCLK_CMUC_APBIF_SEL	[16]
	0=MOUT_TOPC_BUS0_PLL, 1=MOUT_MOUT_TOPC_BUS1_PLL, 2=MOUT_TOPC_CCI_PLL, 3=MOUT_TOPC_MFC_PLL
	SCLK_CMUC_PAUSE_SEL	[5:4]
	SCLK_CMU_APBIF_SEL	[1:0]
	*/
	bits0 = (0 << 16) | (0 << 4) | (0 << 0);
	Outp32(0x10570210, bits0); /* CLK_MUX_SEL_TOPC4 */


	/*
	SCLK_AUD_PLL_RATIO	[31:28]
	SCLK_BUS_PLL_G3D_RATIO	[23:20]
	SCLK_MFC_PLL_RATIO	[19:16]
	SCLK_CCI_PLL_RATIO	[15:12]
	SCLK_BUS1_PLL_RATIO	[11:8]
	SCLK_BUS0_PLL_MIF_RATIO	[7:4]
	SCLK_BUS0_PLL_RATIO	[3:0]
	*/
	bits0 = (0 << 28) | (1 << 20) | (0 << 16) | (0 << 12) | (0 << 8) | (0 << 4) | (0 << 0);
	Outp32(0x1057060C, bits0); /* CLK_DIV_TOPC3 */


/* ********** CMU_TOP0 ********** */
#if (ISP_PLL_MHZ == 590)
	bits1 = (295 << 16) | (6 << 8) | (1 << 0); /* ISP_PLL=590MHz */
#elif (ISP_PLL_MHZ == 560)
	bits1 = (140 << 16) | (3 << 8) | (1 << 0); /* ISP_PLL=560MHz */
#elif (ISP_PLL_MHZ == 552)
	bits1 = (92 << 16) | (2 << 8) | (1 << 0); /* ISP_PLL=552MHz */
#elif (ISP_PLL_MHZ == 492)
	bits1 = (82 << 16) | (2 << 8) | (1 << 0); /* ISP_PLL=492MHz */
#elif (ISP_PLL_MHZ == 432)
	bits1 = (144 << 16) | (2 << 8) | (2 << 0); /* ISP_PLL=432MHz */
#else
	/* printf("check variable &ISP_PLL_MHZ"); */
	while (1);
#endif
	bits0 = ((bits1 >> 8) & 0x3F) * 150;
	Outp32(0x105D0004, bits0); /* ISP_PLL_LOCK */

	bits0 = Inp32(0x105D0114); /* ISP_PLL_CON1 */
	bits0 = bits0 & ~(0x3 << 6);
	bits0 = bits0 | (2 << 6);
	Outp32(0x105D0114, bits0); /* ISP_PLL_CON1 */

	/* ENABLE_SDIV_CLKSTOP=0, ENABLE_MDIV_CLKSTOP=1 */
	bits1 = (bits1) | (0 << 26) | (1 << 5);
#if (ISP_PLL_ON != 0)
	bits1 = (bits1) | (1 << 31);
#endif
	Outp32(0x105D0110, bits1); /* ISP_PLL_CON0(PLL1451X) */
#if (ISP_PLL_ON != 0)
	WAIT_PLL_LOCK(0x105D0110); /* ISP_PLL_CON0 */
#endif

#if (CAM_PLL_MHZ == 690)
	bits1 = (115 << 16) | (2 << 8) | (1 << 0); /* CAM_PLL=690MHz */
#elif (CAM_PLL_MHZ == 660)
	bits1 = (165 << 16) | (3 << 8) | (1 << 0); /* CAM_PLL=660MHz */
#elif (CAM_PLL_MHZ == 532)
	bits1 = (133 << 16) | (3 << 8) | (1 << 0); /* CAM_PLL=532MHz */
#else
	/* printf("check variable &CAM_PLL_MHZ"); */
	while (1);
#endif
	bits0 = ((bits1 >> 8) & 0x3F) * 150;
	Outp32(0x105D0008, bits0); /* CAM_PLL_LOCK */

	bits0 = Inp32(0x105D0124); /* CAM_PLL_CON1 */
	bits0 = bits0 & ~(0x3 << 6);
	bits0 = bits0 | (2 << 6);
	Outp32(0x105D0124, bits0); /* CAM_PLL_CON1 */

	/* ENABLE_SDIV_CLKSTOP=0, ENABLE_MDIV_CLKSTOP=1 */
	bits1 = (bits1) | (0 << 26) | (1 << 5);
#if (CAM_PLL_ON != 0)
	bits1 = (bits1) | (1 << 31);
#endif
	Outp32(0x105D0120, bits1); /* CAM_PLL_CON0(PLL1451X) */
#if (CAM_PLL_ON != 0)
	WAIT_PLL_LOCK(0x105D0120); /* CAM_PLL_CON0 */
#endif

	/*
	ACLK_DISP_400_RATIO	[31:28] DISP_CAM0_DVFS
	ACLK_PERIC0_66_RATIO	[25:20] INT_DVFS
	ACLK_PERIC1_66_RATIO	[17:12] INT_DVFS
	ACLK_VPP0_400_RATIO	[11:8] INT_DVFS
	ACLK_VPP1_400_RATIO	[7:4] INT_DVFS
	*/
	bits0 = (2 << 28) | (11 << 20) | (11 << 12) | (1 << 8) | (1 << 4);  // 800
//	bits0 = (0 << 28) | (5 << 20) | (5 << 12) | (0 << 8) | (0 << 4); //400
	Outp32(0x105D060C, bits0); /* CLK_DIV_TOP03 */

	/*
	0=MOUT_TOP0_BUS0_PLL_USER, 1=MOUT_TOP0_BUS1_PLL_USER, 2=MOUT_TOP0_CCI_PLL_USER, 3=MOUT_TOP0_MFC_PLL_USER,
	4=TOP0_BUS0_PLL_USER_DIV2, 5=TOP0_BUS1_PLL_USER_DIV2, 6=TOP0_CCI_PLL_USER_DIV2, 7=TOP0_MFC_PLL_USER_DIV2
	ACLK_DISP_400_SEL	[30:28] DISP_CAM0_DVFS
	ACLK_PERIC0_66_SEL	[21:20] INT_DVFS
	ACLK_PERIC1_66_SEL	[13:12] INT_DVFS
	ACLK_VPP0_400_SEL	[9:8] INT_DVFS
	ACLK_VPP1_400_SEL	[5:4] INT_DVFS
	*/
	bits0 = (0 << 28) | (0 << 20) | (0 << 12) | (1 << 8) | (1 << 4);	// INT L7
//	bits0 = (0 << 28) | (0 << 20) | (0 << 12) | (0 << 8) | (0 << 4);	// 800
	Outp32(0x105D020C, bits0); /* CLK_MUX_SEL_TOP03 */

	/*
	ACLK_ISP0_ISP0_590_RATIO	[31:28] INT_DVFS
	ACLK_ISP0_TPU_590_RATIO	[27:24] INT_DVFS
	ACLK_ISP0_TREX_532_RATIO	[23:20] INT_DVFS
	ACLK_ISP1_ISP1_468_RATIO	[19:16] INT_DVFS
	ACLK_ISP1_AHB_117_RATIO	[15:12] INT_DVFS
	*/
	bits0 = (7 << 28) | (7 << 24) | (7 << 20) | (7 << 16) | (7 << 12);  // INT L7
//	bits0 = (1 << 28) | (1 << 24) | (0 << 20) | (1 << 16) | (3 << 12);  // 800  DVFS ver. == 2.5
	Outp32(0x105D0610, bits0); /* CLK_DIV_TOP04 */

	/*
	0=MOUT_TOP0_BUS0_PLL, 1=MOUT_TOP0_BUS1_PLL, 2=MOUT_TOP0_CCI_PLL, 3=MOUT_TOP0_MFC_PLL,
	4=MOUT_TOP0_AUD_PLL, 5=MOUT_TOP0_ISP_PLL, 6=MOUT_TOP0_CAM_PLL, 7=Reserved
	ACLK_ISP0_ISP0_590_SEL	[30:28] INT_DVFS
	ACLK_ISP0_TPU_590_SEL	[26:24] INT_DVFS
	ACLK_ISP0_TREX_532_SEL	[22:20] INT_DVFS
	ACLK_ISP1_ISP1_468_SEL	[18:16] INT_DVFS
	ACLK_ISP1_AHB_117_SEL	[14:12] INT_DVFS
	*/
	bits0 = (5 << 28) | (5 << 24) | (5 << 20) | (5 << 16) | (5 << 12);  // INT L7
//	bits0 = (0 << 28) | (0 << 24) | (2 << 20) | (0 << 16) | (0 << 12);  // DVFS ver. == 2.5
	Outp32(0x105D0210, bits0); /* CLK_MUX_SEL_TOP04 */

	/*
	ACLK_CAM0_CSIS0_690_RATIO	[31:28] DISP_CAM_DVFS
	ACLK_CAM0_BNSA_690_RATIO	[27:24] DISP_CAM_DVFS
	ACLK_CAM0_BNSB_690_RATIO	[23:20] DISP_CAM_DVFS
	ACLK_CAM0_BNSD_690_RATIO	[19:16] DISP_CAM_DVFS
	ACLK_CAM0_CSIS1_174_RATIO	[15:12] DISP_CAM_DVFS
	ACLK_CAM0_CSIS3_133_RATIO	[11:8] DISP_CAM_DVFS
	*/
	bits0 = (0 << 28) | (0 << 24) | (1 << 20) | (0 << 16) | (1 << 12) | (3 << 8);
	Outp32(0x105D0614, bits0); /* CLK_DIV_TOP05 */

	/*
	0=MOUT_TOP0_BUS0_PLL, 1=MOUT_TOP0_BUS1_PLL, 2=MOUT_TOP0_CCI_PLL, 3=MOUT_TOP0_MFC_PLL,
	4=MOUT_TOP0_AUD_PLL, 5=MOUT_TOP0_ISP_PLL, 6=MOUT_TOP0_CAM_PLL, 7=Reserved
	ACLK_CAM0_CSIS0_690_SEL	[30:28] DISP_CAM_DVFS
	ACLK_CAM0_BNSA_690_SEL	[26:24] DISP_CAM_DVFS
	ACLK_CAM0_BNSB_690_SEL	[22:20] DISP_CAM_DVFS
	ACLK_CAM0_BNSD_690_SEL	[18:16] DISP_CAM_DVFS
	ACLK_CAM0_CSIS1_174_SEL	[14:12] DISP_CAM_DVFS
	ACLK_CAM0_CSIS3_133_SEL	[10:8]DISP_CAM_DVFS
	*/
	bits0 = (6 << 28) | (6 << 24) | (5 << 20) | (6 << 16) | (5 << 12) | (2 << 8);
	Outp32(0x105D0214, bits0); /* CLK_MUX_SEL_TOP05 */

	/*
	ACLK_CAM0_3AA0_690_RATIO	[31:28] DISP_CAM_DVFS
	ACLK_CAM0_3AA1_468_RATIO	[27:24] DISP_CAM_DVFS
	ACLK_CAM0_TREX_532_RATIO	[23:20] DISP_CAM_DVFS
	ACLK_CAM0_NOCP_133_RATIO	[19:16] DISP_CAM_DVFS
	*/
	bits0 = (0 << 28) | (0 << 24) | (0 << 20) | (3 << 16);
	Outp32(0x105D0618, bits0); /* CLK_DIV_TOP06 */

	/*
	0=MOUT_TOP0_BUS0_PLL, 1=MOUT_TOP0_BUS1_PLL, 2=MOUT_TOP0_CCI_PLL, 3=MOUT_TOP0_MFC_PLL,
	4=MOUT_TOP0_AUD_PLL, 5=MOUT_TOP0_ISP_PLL, 6=MOUT_TOP0_CAM_PLL, 7=Reserved
	ACLK_CAM0_3AA0_690_SEL	[30:28] DISP_CAM_DVFS
	ACLK_CAM0_3AA1_468_SEL	[26:24] DISP_CAM_DVFS
	ACLK_CAM0_TREX_532_SEL	[22:20] DISP_CAM_DVFS
	ACLK_CAM0_NOCP_133_SEL	[18:16] DISP_CAM_DVFS
	*/
	bits0 = (6 << 28) | (3 << 24) | (2 << 20) | (2 << 16);
	Outp32(0x105D0218, bits0); /* CLK_MUX_SEL_TOP06 */

	/*
	ACLK_CAM1_SCLVRA_491_RATIO	[31:28] INT_DVFS
	ACLK_CAM1_ARM_668_RATIO	[27:24] INT_DVFS
	ACLK_CAM1_BUSPERI_334_RATIO	[23:20] INT_DVFS
	ACLK_CAM1_BNSCSIS_133_RATIO	[19:16] INT_DVFS
	ACLK_CAM1_NOCP_133_RATIO	[15:12] INT_DVFS
	ACLK_CAM1_TREX_532_RATIO	[11:8] INT_DVFS
	*/
	bits0 = (7 << 28) | (0 << 24) | (1 << 20) | (7 << 16) | (7 << 12) | (7 << 8); //	 INT L7
//	bits0 = (1 << 28) | (0 << 24) | (1 << 20) | (3 << 16) | (3 << 12) | (1 << 8); //		DVFS ver. == 2.5
	Outp32(0x105D061C, bits0); /* CLK_DIV_TOP07 */

	/*
	0=MOUT_TOP0_BUS0_PLL, 1=MOUT_TOP0_BUS1_PLL, 2=MOUT_TOP0_CCI_PLL, 3=MOUT_TOP0_MFC_PLL,
	4=MOUT_TOP0_AUD_PLL, 5=MOUT_TOP0_ISP_PLL, 6=MOUT_TOP0_CAM_PLL, 7=Reserved
	ACLK_CAM1_SCLVRA_491_SEL	[30:28] INT_DVFS
	ACLK_CAM1_ARM_668_SEL	[26:24] INT_DVFS
	ACLK_CAM1_BUSPERI_334_SEL	[22:20] INT_DVFS
	ACLK_CAM1_BNSCSIS_133_SEL	[18:16] INT_DVFS
	ACLK_CAM1_NOCP_133_SEL	[14:12] INT_DVFS
	ACLK_CAM1_TREX_532_SEL	[10:8] INT_DVFS
	*/
	bits0 = (5 << 28) | (5 << 24) | (5 << 20) | (5 << 16) | (5 << 12) | (5 << 8);	// INT L7
//	bits0 = (0 << 28) | (5 << 24) | (5 << 20) | (2 << 16) | (2 << 12) | (2 << 8);	//		DVFS ver. == 2.5
	Outp32(0x105D021C, bits0); /* CLK_MUX_SEL_TOP07 */

	/*
	SCLK_DECON_INT_ECLK_RATIO	[31:28]
	SCLK_DECON_EXT_ECLK_RATIO	[27:24]
	SCLK_DECON_VCLK_RATIO	[23:20]
	SCLK_DSD_RATIO	[19:16]
	SCLK_HDMI_SPDIF_RATIO	[15:12]
	*/
	bits0=(SCLK_DECON_INT_ECLK_RATIO<<28)|((SCLK_DECON_EXT_ECLK_RATIO)<<24)|(15<<20)|((SCLK_DSD_RATIO)<<16)|((SCLK_HDMI_SPDIF_RATIO)<<12);
	Outp32(0x105D0620, bits0); /* CLK_DIV_TOP0_DISP */

	/*
	0=MOUT_TOP0_BUS0_PLL, 1=MOUT_TOP0_BUS1_PLL, 2=MOUT_TOP0_CCI_PLL, 3=MOUT_TOP0_MFC_PLL
	SCLK_DECON_INT_ECLK_SEL	[29:28]
	SCLK_DECON_EXT_ECLK_SEL	[25:24]
	SCLK_DECON_VCLK_SEL	[21:20]
	SCLK_DSD_SEL	[17:16]

	0=IOCLK_AUDIOCDCLK0, 1=IOCLK_AUDIOCDCLK1, 2=IOCLK_SPDIF_EXTCLK, 3=MOUT_TOP0_AUD_PLL,
	4=MOUT_TOP0_BUS0_PLL, 5=MOUT_TOP0_BUS1_PLL, 6=Reserved, 7=Reserved
	SCLK_HDMI_SPDIF_SEL	[14:12]
	*/
	bits0=(SCLK_DECON_INT_ECLK_SEL<<28)|(1<<24)|(0<<20)|(1<<16)|(4<<12);
	Outp32(0x105D0220, bits0); /* CLK_MUX_SEL_TOP0_DISP */

	/*
	SCLK_ISP_SPI0_RATIO	[31:20]
	SCLK_ISP_SPI1_RATIO	[19:8]
	SCLK_ISP_UART_RATIO	[7:4]
	*/
	bits0 = (9 << 20) | (9 << 8) | (5 << 4); // 800
//	bits0 = (4 << 20) | (4 << 8) | (2 << 4); // 400
	Outp32(0x105D0624, bits0); /* CLK_DIV_TOP0_CAM10 */

	/*
	0=MOUT_TOP0_BUS0_PLL, 1=MOUT_TOP0_BUS1_PLL, 2=MOUT_TOP0_CCI_PLL, 3=MOUT_TOP0_MFC_PLL
	SCLK_ISP_SPI0_SEL	[21:20]
	SCLK_ISP_SPI1_SEL	[9:8]
	SCLK_ISP_UART_SEL	[5:4]
	*/
	bits0 = (0 << 20) | (0 << 8) | (0 << 4);
	Outp32(0x105D0224, bits0); /* CLK_MUX_SEL_TOP0_CAM10 */

	/*
	SCLK_ISP_SENSOR0_RATIO	[23:16]
	SCLK_ISP_SENSOR1_RATIO	[15:8]
	SCLK_ISP_SENSOR2_RATIO	[7:0]
	*/
	bits0 = (0 << 16) | (0 << 8) | (0 << 0);
	Outp32(0x105D0628, bits0); /* CLK_DIV_TOP0_CAM11 */

	/*
	0=MOUT_TOP0_BUS0_PLL_HALF, 1=MOUT_TOP0_BUS1_PLL_HALF, 2=MOUT_TOP0_CCI_PLL_HALF, 3=OSCCLK
	SCLK_ISP_SENSOR0_SEL	[17:16]
	SCLK_ISP_SENSOR1_SEL	[9:8]
	SCLK_ISP_SENSOR2_SEL	[1:0]
	*/
	bits0 = (3 << 16) | (3 << 8) | (3 << 0);
	Outp32(0x105D0228, bits0); /* CLK_MUX_SEL_TOP0_CAM11 */

	/*
	SCLK_I2S1_RATIO	[29:20]
	SCLK_PCM1_RATIO	[19:8]
	SCLK_SPDIF_RATIO	[7:4]
	*/
	bits0 = (15 << 20) | (15 << 8) | (15 << 4);
	Outp32(0x105D0630, bits0); /* CLK_DIV_TOP0_PERIC0 */

	/*
	0=IOCLK_AUDIOCDCLK1, 1=MOUT_TOP0_AUD_PLL, 2=MOUT_TOP0_BUS0_PLL, 3=MOUT_TOP0_BUS1_PLL
	SCLK_I2S1_SEL	[21:20]
	SCLK_PCM1_SEL	[9:8]

	0=IOCLK_AUDIOCDCLK0, 1=IOCLK_AUDIOCDCLK1, 2=IOCLK_SPDIF_EXTCLK, 3=MOUT_TOP0_AUD_PLL,
	4=MOUT_TOP0_BUS0_PLL, 5=MOUT_TOP0_BUS1_PLL, 6=Reserved, 7=Reserved
	SCLK_SPDIF_SEL	[6:4]
	*/
	bits0 = (1 << 20) | (1 << 8) | (3 << 4);
	Outp32(0x105D0230, bits0); /* CLK_MUX_SEL_TOP0_PERIC0 */

	/*
	SCLK_SPI0_RATIO	[31:20]
	SCLK_SPI1_RATIO	[19:8]
	*/
	bits0 = (9 << 20) | (9 << 8); // 800
//	bits0 = (4 << 20) | (4 << 8); // 400
	Outp32(0x105D0634, bits0); /* CLK_DIV_TOP0_PERIC1 */

	/*
	0=MOUT_TOP0_BUS0_PLL, 1=MOUT_TOP0_BUS1_PLL, 2=MOUT_TOP0_CCI_PLL, 3=MOUT_TOP0_MFC_PLL
	SCLK_SPI0_SEL	[21:20]
	SCLK_SPI1_SEL	[9:8]
	*/
	bits0 = (0 << 20) | (0 << 8);
	Outp32(0x105D0234, bits0); /* CLK_MUX_SEL_TOP0_PERIC1 */

	/*
	SCLK_SPI2_RATIO	[31:20]
	SCLK_SPI3_RATIO	[19:8]
	*/
	bits0 = (9 << 20) | (9 << 8); // 800
//	bits0 = (4 << 20) | (4 << 8); // 400
	Outp32(0x105D0638, bits0); /* CLK_DIV_TOP0_PERIC2 */

	/*
	0=MOUT_TOP0_BUS0_PLL, 1=MOUT_TOP0_BUS1_PLL, 2=MOUT_TOP0_CCI_PLL, 3=MOUT_TOP0_MFC_PLL
	SCLK_SPI2_SEL	[21:20]
	SCLK_SPI3_SEL	[9:8]
	*/
	bits0 = (0 << 20) | (0 << 8);
	Outp32(0x105D0238, bits0); /* CLK_MUX_SEL_TOP0_PERIC2 */

	/*
	SCLK_SPI4_RATIO	[31:20]
	SCLK_UART0_RATIO	[19:16]
	SCLK_UART1_RATIO	[15:12]
	SCLK_UART2_RATIO	[11:8]
	SCLK_UART3_RATIO	[7:4]
	*/
	bits0 = (9 << 20) | (5 << 16) | (5 << 12) | (5 << 8) | (5 << 4);	// 800
//	bits0 = (4 << 20) | (2 << 16) | (2 << 12) | (2 << 8) | (2 << 4);	//400
	Outp32(0x105D063C, bits0); /* CLK_DIV_TOP0_PERIC3 */

	/*
	0=MOUT_TOP0_BUS0_PLL, 1=MOUT_TOP0_BUS1_PLL, 2=MOUT_TOP0_CCI_PLL, 3=MOUT_TOP0_MFC_PLL
	SCLK_SPI4_SEL	[21:20]
	SCLK_UART0_SEL	[17:16]
	SCLK_UART1_SEL	[13:12]
	SCLK_UART2_SEL	[9:8]
	SCLK_UART3_SEL	[5:4]
	*/
	bits0 = (0 << 20) | (0 << 16) | (0 << 12) | (0 << 8) | (0 << 4);
	Outp32(0x105D023C, bits0); /* CLK_MUX_SEL_TOP0_PERIC3 */

	/*
	SCLK_SPI5_RATIO	[31:20]
	*/
	bits0 = (9 << 20) ; // 800
//	bits0 = (4 << 20) ; // 400
	Outp32(0x105D0640, bits0); /* CLK_DIV_TOP0_PERIC4 */

	/*
	0=MOUT_TOP0_BUS0_PLL, 1=MOUT_TOP0_BUS1_PLL, 2=MOUT_TOP0_CCI_PLL, 3=MOUT_TOP0_MFC_PLL
	SCLK_SPI5_SEL	[21:20]
	*/
	bits0 = (0 << 20);
	Outp32(0x105D0240, bits0); /* CLK_MUX_SEL_TOP0_PERIC4 */


/* ********** CMU_TOP1 ********** */
	/*
	ACLK_FSYS0_200_RATIO	[31:28] INT_DVFS
	ACLK_FSYS1_200_RATIO	[27:24] INT_DVFS
	*/
#if (ACLK_FSYS1_200_FIX != 0)
	bits0 = (7 << 28) | (7 << 24);
#else
	bits0 = (7 << 28) | (4 << 24);
#endif
	Outp32(0x105E060C, bits0); /* CLK_DIV_TOP13 */

	/*
	0=MOUT_TOP1_BUS0_PLL, 1=MOUT_TOP1_BUS1_PLL, 2=MOUT_TOP1_CCI_PLL, 3=MOUT_TOP1_MFC_PLL
	ACLK_FSYS0_200_SEL	[29:28] INT_DVFS
	ACLK_FSYS1_200_SEL	[25:24] INT_DVFS
	*/
	bits0 = (0 << 28) | (0 << 24);
	Outp32(0x105E020C, bits0); /* CLK_MUX_SEL_TOP13 */

	/*
	SCLK_USBDRD300_RATIO	[31:28]
	SCLK_MMC2_RATIO	[25:16]
	SCLK_PHY_FSYS0_RATIO	[13:8]
	SCLK_PHY_FSYS0_26M_RATIO	[5:0]
	*/
#ifdef MMC_BOOT_CLK
	bits0 = (15 << 28) | (15 << 16) | (7 << 8) | (17 << 0); /* MMC 50MHz setting for bootloader */
#else
	bits0 = (15 << 28) | (0 << 16) | (7 << 8) | (17 << 0);
#endif
	Outp32(0x105E0624, bits0); /* CLK_DIV_TOP1_FSYS0 */

	/*
	0=MOUT_TOP1_BUS0_PLL, 1=MOUT_TOP1_BUS1_PLL, 2=MOUT_TOP1_CCI_PLL, 3=MOUT_TOP1_MFC_PLL
	SCLK_USBDRD300_SEL	[29:28]
	SCLK_MMC2_SEL	[17:16]
	SCLK_PHY_FSYS0_SEL	[9:8]
	SCLK_PHY_FSYS0_26M_SEL	[1:0]
	*/
	bits0 = (0 << 28) | (0 << 16) | (0 << 8) | (3 << 0);
	Outp32(0x105E0224, bits0); /* CLK_MUX_SEL_TOP1_FSYS0 */

	/*
	SCLK_UFSUNIPRO20_RATIO	[21:16]
	SCLK_TLX400_WIFI1_RATIO	[13:8]
	SCLK_PHY_FSYS1_RATIO	[5:0]
	*/
	bits0 = (3 << 16) | (3 << 8) | (7 << 0);
	Outp32(0x105E0628, bits0); /* CLK_DIV_TOP1_FSYS1 */

	/*
	0=MOUT_TOP1_BUS0_PLL, 1=MOUT_TOP1_BUS1_PLL, 2=MOUT_TOP1_CCI_PLL, 3=MOUT_TOP1_MFC_PLL
	SCLK_UFSUNIPRO20_SEL	[17:16]
	SCLK_TLX400_WIFI1_SEL	[9:8]
	SCLK_PHY_FSYS1_SEL	[1:0]
	*/
	bits0 = (1 << 16) | (0 << 8) | (0 << 0);
	Outp32(0x105E0228, bits0); /* CLK_MUX_SEL_TOP1_FSYS1 */

	/*
	SCLK_PHY_FSYS1_26M_RATIO	[29:24]
	SCLK_MMC0_RATIO	[21:12]
	SCLK_MMC1_RATIO	[9:0]
	*/
#ifdef MMC_BOOT_CLK
	bits0 = (17 << 24) | (15 << 12) | (15 << 0);		/* MMC 50MHz setting for bootloader */
#else
	bits0 = (17 << 24) | (0 << 12) | (0 << 0);
#endif
	Outp32(0x105E062C, bits0); /* CLK_DIV_TOP1_FSYS11 */

	/*
	0=MOUT_TOP1_BUS0_PLL, 1=MOUT_TOP1_BUS1_PLL, 2=MOUT_TOP1_CCI_PLL, 3=MOUT_TOP1_MFC_PLL
	SCLK_PHY_FSYS1_26M_SEL	[25:24]
	SCLK_MMC0_SEL	[13:12]
	SCLK_MMC1_SEL	[1:0]
	*/
	bits0 = (3 << 24) | (0 << 12) | (0 << 0);
	Outp32(0x105E022C, bits0); /* CLK_MUX_SEL_TOP1_FSYS11 */


/* ********** CMU_CCORE ********** */
	/*
	ACLK_CCORE_266_RATIO	[2:0]
	*/
	bits0 = (1 << 0);
	Outp32(0x105B0600, bits0); /* CLK_DIV_CCORE */

	/*
	0=OSCCLK, 1=ACLK_CCORE_133
	ACLK_CCORE_133_USER_SEL	[1]

	0=OSCCLK, 1=ACLK_CCORE_532
	ACLK_CCORE_532_USER_SEL	[0]
	*/
	bits0 = (1 << 1) | (1 << 0);
	Outp32(0x105B0200, bits0); /* CLK_MUX_SEL_CCORE */


/* ********** CMU_IMEM ********** */
	/*
	ACLK_IMEM_100_USER_SEL	[8]
	ACLK_IMEM_200_USER_SEL	[4]
	ACLK_IMEM_266_USER_SEL	[0]
	*/
	bits0 = (1 << 8) | (1 << 4) | (1 << 0);
	Outp32(0x11060200, bits0); /* CLK_MUX_SEL_IMEM */


/* ********** CMU_PERIC0 ********** */
	/*
	0=OSCCLK, 1=SCLK_UART0
	SCLK_UART0_USER_SEL	[16]

	0=OSCCLK, 1=ACLK_PERIC0_66
	ACLK_PERIC0_66_USER_SEL	[0]
	*/
	bits0 = (1 << 16) | (1 << 0);
	Outp32(0x13610200, bits0); /* CLK_MUX_SEL_PERIC0 */


/* ********** CMU_PERIC1 ********** */
	/*
	0=OSCCLK, 1=ACLK_PERIC1_66
	ACLK_PERIC1_66_USER_SEL	[0]
	*/
	bits0 = (1 << 0);
	Outp32(0x14C80200, bits0); /* CLK_MUX_SEL_PERIC10 */

	/*
	0=OSCCLK, 1=SCLK_UART3
	SCLK_UART3_USER_SEL	[28]

	0=OSCCLK, 1=SCLK_UART2
	SCLK_UART2_USER_SEL	[24]

	0=OSCCLK, 1=SCLK_UART1
	SCLK_UART1_USER_SEL	[20]

	0=OSCCLK, 1=SCLK_SPI4
	SCLK_SPI4_USER_SEL	[16]

	0=OSCCLK, 1=SCLK_SPI3
	SCLK_SPI3_USER_SEL	[12]

	0=OSCCLK, 1=SCLK_SPI2
	SCLK_SPI2_USER_SEL	[8]

	0=OSCCLK, 1=SCLK_SPI1
	SCLK_SPI1_USER_SEL	[4]

	0=OSCCLK, 1=SCLK_SPI0
	SCLK_SPI0_USER_SEL	[0]
	*/
	bits0 = (1 << 28) | (1 << 24) | (1 << 20) | (1 << 16) | (1 << 12) | (1 << 8) | (1 << 4) | (1 << 0);
	Outp32(0x14C80204, bits0); /* CLK_MUX_SEL_PERIC11 */

	/*
	0=OSCCLK, 1=SCLK_SPI5
	SCLK_SPI5_USER_SEL	[0]
	*/
	bits0 = (1 << 0);
	Outp32(0x14C80208, bits0); /* CLK_MUX_SEL_PERIC12 */

/* ********** CMU_PERIS ********** */
	/*
	0=OSCCLK, 1=ACLK_PERIS_66
	ACLK_PERIS_66_USER_SEL	[0]
	*/
	bits0 = (1 << 0);
	Outp32(0x10040200, bits0); /* CLK_MUX_SEL_PERIS */


/* ********** CMU_BUS0 ********** */
	/*
	0=OSCCLK, 1=PCLK_BUS0_133
	PCLK_BUS0_133_USER_SEL	[4]

	0=OSCCLK, 1=ACLK_BUS0_532
	ACLK_BUS0_532_USER_SEL	[0]
	*/
	bits0 = (1 << 4) | (1 << 0);
	Outp32(0x13400200, bits0); /* CLK_MUX_SEL_BUS0 */


/* ********** CMU_BUS1 ********** */
	/*
	0=OSCCLK, 1=PCLK_BUS1_133
	PCLK_BUS1_133_USER_SEL	[8]

	0=OSCCLK, 1=ACLK_BUS1_200
	ACLK_BUS1_200_USER_SEL	[4]

	0=OSCCLK, 1=ACLK_BUS1_532
	ACLK_BUS1_532_USER_SEL	[0]
	*/
	bits0 = (1 << 8) | (1 << 4) | (1 << 0);
	Outp32(0x14800200, bits0); /* CLK_MUX_SEL_BUS1 */

/* ********** CMU_ATLAS ********** */
	clk_init_big_cpu();
/* ********** CMU_APOLLO ********** */
	clk_init_little_cpu();

/* ********** CMU_MIF ********** */
	clk_init_mif();

#if !defined(SRAM)
	if (cmu_resume == 0) {
/* ********** CMU_G3D ********** */
		clk_init_g3d();
/* ********** CMU_CAM0 ********** */
		clk_init_cam0();
/* ********** CMU_CAM1 ********** */
		clk_init_cam1();
/* ********** CMU_ISP0 ********** */
		clk_init_isp0();
/* ********** CMU_ISP1 ********** */
		clk_init_isp1();
/* ********** CMU_VPP ********** */
		clk_init_vpp();
/* ********** CMU_DISP ********** */
		clk_init_disp();
/* ********** CMU_AUD ********** */
		clk_init_aud();
/* ********** CMU_FSYS0 ********** */
		clk_init_fsys0();
/* ********** CMU_FSYS1 ********** */
		clk_init_fsys1();
/* ********** CMU_MSCL ********** */
		clk_init_mscl();
/* ********** CMU_MFC ********** */
		clk_init_mfc();
	}
#endif

/* ********** Turn on ********** */
	/*
	0=MOUT_TOPC_MFC_PLL, 1=TOPC_MFC_PLL¡¯s 1/2 frequency clock
	TOPC_MFC_PLL_HALF_SEL [28]

	0=MOUT_TOPC_CCI_PLL, 1=TOPC_CCI_PLL¡¯s 1/2 frequency clock
	TOPC_CCI_PLL_HALF_SEL [24]

	0=MOUT_TOPC_BUS1_PLL, 1=MOUT_TOPC_BUS1_PLL¡¯s 1/2 frequency clock
	MOUT_TOPC_BUS1_PLL_HALF_SEL	[20]

	0=TOPC_BUS0_PLL, 1=TOPC_BUS0_PLL¡¯s 1/2 frequency clock, 2=TOPC_BUS0_PLL¡¯s 1/4 frequency clock, 3=Reserved
	TOPC_BUS0_PLL_HALF_SEL	[17:16]

	0=OSCCLK, 1=FOUT_MFC_PLL
	TOPC_MFC_PLL_SEL	[12]

	0=OSCCLK, 1=FOUT_CCI_PLL
	TOPC_CCI_PLL_SEL	[8]

	0=OSCCLK, 1=FOUT_BUS1_PLL
	MOUT_TOPC_BUS1_PLL_SEL	[4]

	0=OSCCLK, 1=FOUT_BUS0_PLL
	TOPC_BUS0_PLL_SEL	[0]
	*/
#if (BUS0_PLL_MHZ == 1600)
	bits0 = (0 << 28) | (0 << 24) | (0 << 20) | (2 << 16);
#elif (BUS0_PLL_MHZ == 800)
	bits0 = (0 << 28) | (0 << 24) | (0 << 20) | (1 << 16);
#else
	/* printf("check variable &BUS0_PLL_MHZ"); */
	while (1);
#endif

#if (MFC_PLL_ON != 0)
	bits0 = (bits0) | (1 << 12);
#endif
#if (CCI_PLL_ON != 0)
	bits0 = (bits0) | (1 << 8);
#endif
#if (BUS1_PLL_ON != 0)
	bits0 = (bits0) | (1 << 4);
#endif
#if (BUS0_PLL_ON != 0)
	bits0 = (bits0) | (1 << 0);
#endif
	Outp32(0x10570200, bits0); /* CLK_MUX_SEL_TOPC0 */
	WAIT_STATUS(0x10570200, 0x10570400); /* CLK_MUX_STAT_TOPC0 */

	/*
	0=TOPC_BUS0_PLL, 1=TOPC_BUS0_PLL¡¯s 1/2 frequency clock, 2=MOUT_TOPC_BUS1_PLL, 3=MOUT_TOPC_CCI_PLL
	SCLK_BUS_PLL_G3D_SEL	[21:20]

	0=TOPC_BUS0_PLL, 1=TOPC_BUS0_PLL¡¯s 1/2 frequency clock
	SCLK_BUS0_PLL_OUT_SEL [16]

	0=TOPC_BUS0_PLL, 1=TOPC_BUS0_PLL¡¯s 1/2 frequency clock, 2=MOUT_TOPC_BUS1_PLL, 3=MOUT_TOPC_CCI_PLL
	SCLK_BUS0_PLL_MIF_SEL [13:12]

	0=TOPC_BUS0_PLL, 1=TOPC_BUS0_PLL¡¯s 1/2 frequency clock, 2=MOUT_TOPC_BUS1_PLL, 3=MOUT_TOPC_CCI_PLL
	SCLK_BUS0_PLL_APOLLO_SEL	[9:8]

	0=TOPC_BUS0_PLL, 1=TOPC_BUS0_PLL¡¯s 1/2 frequency clock, 2=MOUT_TOPC_BUS1_PLL, 3=MOUT_TOPC_CCI_PLL
	SCLK_BUS0_PLL_ATLAS_SEL	[5:4]

	0=OSCCLK, 1=FOUT_AUD_PLL
	TOPC_AUD_PLL_SEL	[0]
	*/
	bits0 = (1 << 20) | (1 << 16) | (1 << 12) | (1 << 8) | (1 << 4); /* Switching clock = 800MHz=1600MHz/2 */
#if (AUD_PLL_ON != 0)
	bits0 = (bits0) | (1 << 0);
#endif
	Outp32(0x10570204, bits0); /* CLK_MUX_SEL_TOPC1 */
	WAIT_STATUS(0x10570204, 0x10570404); /* CLK_MUX_STAT_TOPC1 */


	/*
	0=OSCCLK, 1=FOUT_ISP_PLL
	TOP0_ISP_PLL_SEL	[24]

	0=OSCCLK, 1=FOUT_CAM_PLL
	TOP0_CAM_PLL_SEL	[20]

	0=OSCCLK, 1=SCLK_BUS0_PLL_A
	TOP0_BUS0_PLL_USER_SEL [16]

	0=OSCCLK, 1=SCLK_BUS1_PLL_A
	TOP0_BUS1_PLL_USER_SEL [12]

	0=OSCCLK, 1=SCLK_CCI_PLL_A
	TOP0_CCI_PLL_USER_SEL	[8]

	0=OSCCLK, 1=SCLK_MFC_PLL_A
	TOP0_MFC_PLL_USER_SEL	[4]

	0=OSCCLK, 1=SCLK_AUD_PLL
	TOP0_AUD_PLL_USER_SEL	[0]
	*/
	bits0 = (1 << 16) | (1 << 12) | (1 << 8) | (1 << 4) | (1 << 0);
#if (ISP_PLL_ON != 0)
	bits0 = (bits0) | (1 << 24);
#endif
#if (CAM_PLL_ON != 0)
	bits0 = (bits0) | (1 << 20);
#endif
	Outp32(0x105D0200, bits0); /* CLK_MUX_SEL_TOP00 */
	WAIT_STATUS(0x105D0200, 0x105D0400); /* CLK_MUX_STAT_TOP00 */

	/*
	0=OSCCLK, 1=SCLK_CMU_APBIF
	SCLK_CMU0_APBIF_SEL	[28]

	0=TOP0_BUS0_PLL, 1=TOP0_BUS0_PLL¡¯s 1/2 frequency clock
	TOP0_BUS0_PLL_HALF_SEL	[16]

	0=TOP0_BUS1_PLL, 1=TOP0_BUS1_PLL¡¯s 1/2 frequency clock
	TOP0_BUS1_PLL_HALF_SEL	[12]

	0=TOP0_CCI_PLL, 1=TOP0_CCI_PLL¡¯s 1/2 frequency clock
	TOP0_CCI_PLL_HALF_SEL [8]

	0=TOP0_MFC_PLL, 1=TOP0_MFC_PLL¡¯s 1/2 frequency clock
	TOP0_MFC_PLL_HALF_SEL [4]
	*/
	bits0 = (0 << 28) | (0 << 16) | (0 << 12) | (0 << 8) | (0 << 4);  // 800MHz
//	bits0 = (0 << 28) | (1 << 16) | (0 << 12) | (0 << 8) | (0 << 4);	// 400MHz
	Outp32(0x105D0204, bits0); /* CLK_MUX_SEL_TOP01 */
	WAIT_STATUS(0x105D0204, 0x105D0404); /* CLK_MUX_STAT_TOP01 */

	/*
	0=OSCCLK, 1=SCLK_BUS0_PLL_B
	TOP1_BUS0_PLL_USER_SEL	[16]

	0=OSCCLK, 1=SCLK_BUS1_PLL_B
	TOP1_BUS1_PLL_USER_SEL	[12]

	0=OSCCLK, 1=SCLK_CCI_PLL_B
	TOP1_CCI_PLL_USER_SEL	[8]

	0=OSCCLK, 1=SCLK_MFC_PLL_B
	TOP1_MFC_PLL_USER_SEL	[4]
	*/
	bits0 = (1 << 16) | (1 << 12) | (1 << 8) | (1 << 4);
	Outp32(0x105E0200, bits0); /* CLK_MUX_SEL_TOP10 */
	WAIT_STATUS(0x105E0200, 0x105E0400); /* CLK_MUX_STAT_TOP10 */

	/*
	0=OSCCLK, 1=SCLK_CMU_APBIF
	SCLK_CMU1_APBIF_SEL	[28]

	0=TOP1_BUS0_PLL, 1=TOP1_BUS0_PLL¡¯s 1/2 frequency clock
	TOP1_BUS0_PLL_HALF_SEL	[16]

	0=TOP1_BUS1_PLL, 1=TOP1_BUS1_PLL¡¯s 1/2 frequency clock
	TOP1_BUS1_PLL_HALF_SEL	[12]

	0=TOP1_CCI_PLL, 1=TOP1_CCI_PLL¡¯s 1/2 frequency clock
	TOP1_CCI_PLL_HALF_SEL	[8]

	0=TOP1_MFC_PLL, 1=TOP1_MFC_PLL¡¯s 1/2 frequency clock
	TOP1_MFC_PLL_HALF_SEL	[4]
	*/
	bits0 = (0 << 28) | (0 << 16) | (0 << 12) | (0 << 8) | (0 << 4);
	Outp32(0x105E0204, bits0); /* CLK_MUX_SEL_TOP11 */
	WAIT_STATUS(0x105E0204, 0x105E0404); /* CLK_MUX_STAT_TOP11 */

	return 0;
}

int clk_init_big_cpu(void)
{
	unsigned int bits0, bits1, bits2, bits3;

	Outp32(0x11800200, 0x00000000); /* CLK_MUX_SEL_ATLAS0 */
	WAIT_STATUS(0x11800200, 0x11800400); /* CLK_MUX_STAT_ATLAS0 */

#if (ATLAS_PLL_MHZ == 2400)
	bits1 = (200 << 16) | (2 << 8) | (0 << 0);					/* ATLAS_PLL=2400MHz */
	bits2 = (6 << 26) | (6 << 20) | (6 << 12) | (2 << 8) | (0 << 4) | (0 << 0); /* CLK_DIV_ATLAS0 */
	bits3 = (6 << 8) | (5 << 4) | (1 << 0); /* CLK_DIV_ATLAS1 */
#elif (ATLAS_PLL_MHZ == 2300)
	bits1 = (575 << 16) | (6 << 8) | (0 << 0);					/* ATLAS_PLL=2300MHz */
	bits2 = (6 << 26) | (6 << 20) | (6 << 12) | (2 << 8) | (0 << 4) | (0 << 0); /* CLK_DIV_ATLAS0 */
	bits3 = (6 << 8) | (5 << 4) | (1 << 0); /* CLK_DIV_ATLAS1 */
#elif (ATLAS_PLL_MHZ == 2200)
	bits1 = (275 << 16) | (3 << 8) | (0 << 0);					/* ATLAS_PLL=2200MHz */
	bits2 = (6 << 26) | (6 << 20) | (6 << 12) | (2 << 8) | (0 << 4) | (0 << 0); /* CLK_DIV_ATLAS0 */
	bits3 = (6 << 8) | (5 << 4) | (1 << 0); /* CLK_DIV_ATLAS1 */
#elif (ATLAS_PLL_MHZ == 2100)
	bits1 = (175 << 16) | (2 << 8) | (0 << 0);					/* ATLAS_PLL=2100MHz */
	bits2 = (6 << 26) | (6 << 20) | (6 << 12) | (2 << 8) | (0 << 4) | (0 << 0); /* CLK_DIV_ATLAS0 */
	bits3 = (6 << 8) | (5 << 4) | (1 << 0); /* CLK_DIV_ATLAS1 */
#elif (ATLAS_PLL_MHZ == 2000)
	bits1 = (250 << 16) | (3 << 8) | (0 << 0);					/* ATLAS_PLL=2000MHz */
	bits2 = (6 << 26) | (6 << 20) | (6 << 12) | (2 << 8) | (0 << 4) | (0 << 0); /* CLK_DIV_ATLAS0 */
	bits3 = (6 << 8) | (5 << 4) | (1 << 0); /* CLK_DIV_ATLAS1 */
#elif (ATLAS_PLL_MHZ == 1900)
	bits1 = (475 << 16) | (6 << 8) | (0 << 0);					/* ATLAS_PLL=1900MHz */
	bits2 = (6 << 26) | (6 << 20) | (6 << 12) | (2 << 8) | (0 << 4) | (0 << 0); /* CLK_DIV_ATLAS0 */
	bits3 = (6 << 8) | (4 << 4) | (1 << 0); /* CLK_DIV_ATLAS1 */
#elif (ATLAS_PLL_MHZ == 1800)
	bits1 = (150 << 16) | (2 << 8) | (0 << 0);					/* ATLAS_PLL=1800MHz */
	bits2 = (6 << 26) | (6 << 20) | (6 << 12) | (2 << 8) | (0 << 4) | (0 << 0); /* CLK_DIV_ATLAS0 */
	bits3 = (6 << 8) | (4 << 4) | (1 << 0); /* CLK_DIV_ATLAS1 */
#elif (ATLAS_PLL_MHZ == 1700)
	bits1 = (425 << 16) | (6 << 8) | (0 << 0);					/* ATLAS_PLL=1700MHz */
	bits2 = (6 << 26) | (6 << 20) | (6 << 12) | (2 << 8) | (0 << 4) | (0 << 0); /* CLK_DIV_ATLAS0 */
	bits3 = (6 << 8) | (4 << 4) | (1 << 0); /* CLK_DIV_ATLAS1 */
#elif (ATLAS_PLL_MHZ == 1600)
	bits1 = (200 << 16) | (3 << 8) | (0 << 0);					/* ATLAS_PLL=1600MHz */
	bits2 = (6 << 26) | (6 << 20) | (6 << 12) | (2 << 8) | (0 << 4) | (0 << 0); /* CLK_DIV_ATLAS0 */
	bits3 = (6 << 8) | (4 << 4) | (1 << 0); /* CLK_DIV_ATLAS1 */
#elif (ATLAS_PLL_MHZ == 1500)
	bits1 = (250 << 16) | (2 << 8) | (1 << 0);					/* ATLAS_PLL=1500MHz */
	bits2 = (6 << 26) | (6 << 20) | (6 << 12) | (2 << 8) | (0 << 4) | (0 << 0); /* CLK_DIV_ATLAS0 */
	bits3 = (6 << 8) | (4 << 4) | (1 << 0); /* CLK_DIV_ATLAS1 */
#elif (ATLAS_PLL_MHZ == 1400)
	bits1 = (350 << 16) | (3 << 8) | (1 << 0);					/* ATLAS_PLL=1400MHz */
	bits2 = (6 << 26) | (6 << 20) | (6 << 12) | (2 << 8) | (0 << 4) | (0 << 0); /* CLK_DIV_ATLAS0 */
	bits3 = (6 << 8) | (3 << 4) | (1 << 0); /* CLK_DIV_ATLAS1 */
#elif (ATLAS_PLL_MHZ == 1300)
	bits1 = (325 << 16) | (3 << 8) | (1 << 0);					/* ATLAS_PLL=1300MHz */
	bits2 = (6 << 26) | (6 << 20) | (6 << 12) | (2 << 8) | (0 << 4) | (0 << 0); /* CLK_DIV_ATLAS0 */
	bits3 = (6 << 8) | (3 << 4) | (1 << 0); /* CLK_DIV_ATLAS1 */
#elif (ATLAS_PLL_MHZ == 1200)
	bits1 = (200 << 16) | (2 << 8) | (1 << 0);					/* ATLAS_PLL=1200MHz */
	bits2 = (6 << 26) | (6 << 20) | (6 << 12) | (2 << 8) | (0 << 4) | (0 << 0); /* CLK_DIV_ATLAS0 */
	bits3 = (6 << 8) | (3 << 4) | (1 << 0); /* CLK_DIV_ATLAS1 */
#elif (ATLAS_PLL_MHZ == 1100)
	bits1 = (275 << 16) | (3 << 8) | (1 << 0);					/* ATLAS_PLL=1100MHz */
	bits2 = (6 << 26) | (6 << 20) | (6 << 12) | (2 << 8) | (0 << 4) | (0 << 0); /* CLK_DIV_ATLAS0 */
	bits3 = (6 << 8) | (3 << 4) | (1 << 0); /* CLK_DIV_ATLAS1 */
#elif (ATLAS_PLL_MHZ == 1000)
	bits1 = (250 << 16) | (3 << 8) | (1 << 0);					/* ATLAS_PLL=1000MHz */
	bits2 = (6 << 26) | (6 << 20) | (6 << 12) | (2 << 8) | (0 << 4) | (0 << 0); /* CLK_DIV_ATLAS0 */
	bits3 = (6 << 8) | (3 << 4) | (1 << 0); /* CLK_DIV_ATLAS1 */
#elif (ATLAS_PLL_MHZ == 900)
	bits1 = (150 << 16) | (2 << 8) | (1 << 0);					/* ATLAS_PLL=900MHz */
	bits2 = (6 << 26) | (6 << 20) | (6 << 12) | (2 << 8) | (0 << 4) | (0 << 0); /* CLK_DIV_ATLAS0 */
	bits3 = (6 << 8) | (3 << 4) | (1 << 0); /* CLK_DIV_ATLAS1 */
#elif (ATLAS_PLL_MHZ == 800)
	bits1 = (200 << 16) | (3 << 8) | (1 << 0);					/* ATLAS_PLL=800MHz */
	bits2 = (5 << 26) | (5 << 20) | (5 << 12) | (2 << 8) | (0 << 4) | (0 << 0); /* CLK_DIV_ATLAS0 */
	bits3 = (5 << 8) | (3 << 4) | (1 << 0); /* CLK_DIV_ATLAS1 */
#elif (ATLAS_PLL_MHZ == 700)
	bits1 = (350 << 16) | (3 << 8) | (2 << 0);					/* ATLAS_PLL=700MHz */
	bits2 = (5 << 26) | (5 << 20) | (5 << 12) | (2 << 8) | (0 << 4) | (0 << 0); /* CLK_DIV_ATLAS0 */
	bits3 = (5 << 8) | (3 << 4) | (1 << 0); /* CLK_DIV_ATLAS1 */
#elif (ATLAS_PLL_MHZ == 600)
	bits1 = (200 << 16) | (2 << 8) | (2 << 0);					/* ATLAS_PLL=600MHz */
	bits2 = (4 << 26) | (4 << 20) | (4 << 12) | (2 << 8) | (0 << 4) | (0 << 0); /* CLK_DIV_ATLAS0 */
	bits3 = (4 << 8) | (3 << 4) | (1 << 0); /* CLK_DIV_ATLAS1 */
#elif (ATLAS_PLL_MHZ == 500)
	bits1 = (250 << 16) | (3 << 8) | (2 << 0);					/* ATLAS_PLL=500MHz */
	bits2 = (3 << 26) | (3 << 20) | (3 << 12) | (2 << 8) | (0 << 4) | (0 << 0); /* CLK_DIV_ATLAS0 */
	bits3 = (3 << 8) | (2 << 4) | (1 << 0); /* CLK_DIV_ATLAS1 */
#elif (ATLAS_PLL_MHZ == 400)
	bits1 = (200 << 16) | (3 << 8) | (2 << 0);					/* ATLAS_PLL=400MHz */
	bits2 = (3 << 26) | (3 << 20) | (3 << 12) | (2 << 8) | (0 << 4) | (0 << 0); /* CLK_DIV_ATLAS0 */
	bits3 = (3 << 8) | (2<< 4) | (1 << 0); /* CLK_DIV_ATLAS1 */
#elif (ATLAS_PLL_MHZ == 300)
	bits1 = (200 << 16) | (2 << 8) | (3 << 0);					/* ATLAS_PLL=300MHz */
	bits2 = (3 << 26) | (3 << 20) | (3 << 12) | (2 << 8) | (0 << 4) | (0 << 0); /* CLK_DIV_ATLAS0 */
	bits3 = (3 << 8) | (2 << 4) | (1 << 0); /* CLK_DIV_ATLAS1 */
#elif (ATLAS_PLL_MHZ == 200)
	bits1 = (200 << 16) | (3 << 8) | (3 << 0);					/* ATLAS_PLL=200MHz */
	bits2 = (3 << 26) | (3 << 20) | (3 << 12) | (2 << 8) | (0 << 4) | (0 << 0); /* CLK_DIV_ATLAS0 */
	bits3 = (3 << 8) | (1 << 4) | (1 << 0); /* CLK_DIV_ATLAS1 */
#elif (ATLAS_PLL_MHZ == 100)
	bits1 = (200 << 16) | (3 << 8) | (4 << 0);					/* ATLAS_PLL=100MHz */
	bits2 = (3 << 26) | (3 << 20) | (3 << 12) | (2 << 8) | (0 << 4) | (0 << 0); /* CLK_DIV_ATLAS0 */
	bits3 = (3 << 8) | (1 << 4) | (1 << 0); /* CLK_DIV_ATLAS1 */
#else
	/* printf("check variable &ATLAS_PLL_MHZ"); */
	while (1);
#endif

	bits0 = ((bits1 >> 8) & 0x3F) * 150;
	Outp32(0x11800000, bits0); /* ATLAS_PLL_LOCK */

	/* ENABLE_SDIV_CLKSTOP=0, ENABLE_MDIV_CLKSTOP=1 */
	bits1 = (bits1) | (0 << 26) | (1 << 5);
#if (ATLAS_PLL_ON != 0)
	bits1 = (bits1) | (1 << 31);
#endif
	Outp32(0x11800100, bits1); /* ATLAS_PLL_CON0(PLL1450X) */
#if (ATLAS_PLL_ON != 0)
	WAIT_PLL_LOCK(0x11800100); /* ATLAS_PLL_CON0 */
#endif

	/*
	PCLK_DBG_ATLAS_RATIO	[31:26]
	ATCLK_ATLAS_RATIO	[25:20]
	PCLK_ATLAS_RATIO	[17:12]
	ACLK_ATLAS_RATIO	[10:8]
	ATLAS2_RATIO	[6:4]
	ATLAS1_RATIO	[2:0]
	*/
	Outp32(0x11800600, bits2); /* CLK_DIV_ATLAS0 */

	/*
	CNTCLK_ATLAS_RATIO	[11:8]
	SCLK_HPM_ATLAS_RATIO	[6:4]
	ATLAS_PLL_RATIO	[2:0]
	*/
	Outp32(0x11800604, bits3); /* CLK_DIV_ATLAS1 */

	/*
	0=OSCCLK, 1=SCLK_BUS0_PLL_ATLAS
	SCLK_BUS0_PLL_ATLAS_USER_SEL	[0]
	*/
	bits0 = (1 << 0);
	Outp32(0x11800204, bits0); /* CLK_MUX_SEL_ATLAS1 */

	/*
	0=MOUT_ATLAS_PLL, 1=MOUT_BUS0_PLL_ATLAS
	ATLAS_SEL	[0]
	*/
	bits0 = (0 << 0);
	Outp32(0x11800208, bits0); /* CLK_MUX_SEL_ATLAS2 */

	/*
	0=OSCCLK, 1=FOUT_ATLAS_PLL
	ATLAS_PLL_SEL	[0]
	*/
#if (ATLAS_PLL_ON != 0)
	bits0 = (1 << 0);
	Outp32(0x11800200, bits0); /* CLK_MUX_SEL_ATLAS0 */
	WAIT_STATUS(0x11800200, 0x11800400); /* CLK_MUX_STAT_ATLAS0 */
#endif
	return 0;
}

int clk_init_little_cpu(void)
{
	unsigned int bits0, bits1, bits2, bits3;

	Outp32(0x11900200, 0x00000000); /* CLK_MUX_SEL_APOLLO0 */
	WAIT_STATUS(0x11900200, 0x11900400); /* CLK_MUX_STAT_APOLLO0 */

#if (APOLLO_PLL_MHZ == 1600)
	bits1 = (200 << 16) | (3 << 8) | (0 << 0);									/* APOLLO_PLL=1600MHz */
	bits2 = (5 << 24) | (5 << 20) | (5 << 16) | (5 << 12) | (2 << 8) | (0 << 4) | (0 << 0); /* CLK_DIV_APOLLO0 */
	bits3 = (3 << 4) | (1 << 0); /* CLK_DIV_APOLLO1 */
#elif (APOLLO_PLL_MHZ == 1500)
	bits1 = (250 << 16) | (4 << 8) | (0 << 0);									/* APOLLO_PLL=1500MHz */
	bits2 = (5 << 24) | (5 << 20) | (5 << 16) | (5 << 12) | (2 << 8) | (0 << 4) | (0 << 0); /* CLK_DIV_APOLLO0 */
	bits3 = (3 << 4) | (1 << 0); /* CLK_DIV_APOLLO1 */
#elif (APOLLO_PLL_MHZ == 1400)
	bits1 = (175 << 16) | (3 << 8) | (0 << 0);									/* APOLLO_PLL=1400MHz */
	bits2 = (5 << 24) | (5 << 20) | (5 << 16) | (5 << 12) | (2 << 8) | (0 << 4) | (0 << 0); /* CLK_DIV_APOLLO0 */
	bits3 = (3 << 4) | (1 << 0); /* CLK_DIV_APOLLO1 */
#elif (APOLLO_PLL_MHZ == 1300)
	bits1 = (325 << 16) | (6 << 8) | (0 << 0);									/* APOLLO_PLL=1300MHz */
	bits2 = (5 << 24) | (5 << 20) | (5 << 16) | (5 << 12) | (2 << 8) | (0 << 4) | (0 << 0); /* CLK_DIV_APOLLO0 */
	bits3 = (3 << 4) | (1 << 0); /* CLK_DIV_APOLLO1 */
#elif (APOLLO_PLL_MHZ == 1200)
	bits1 = (100 << 16) | (2 << 8) | (0 << 0);									/* APOLLO_PLL=1200MHz */
	bits2 = (5 << 24) | (5 << 20) | (5 << 16) | (5 << 12) | (2 << 8) | (0 << 4) | (0 << 0); /* CLK_DIV_APOLLO0 */
	bits3 = (3 << 4) | (1 << 0); /* CLK_DIV_APOLLO1 */
#elif (APOLLO_PLL_MHZ == 1100)
	bits1 = (275 << 16) | (6 << 8) | (0 << 0);									/* APOLLO_PLL=1100MHz */
	bits2 = (5 << 24) | (5 << 20) | (5 << 16) | (5 << 12) | (2 << 8) | (0 << 4) | (0 << 0); /* CLK_DIV_APOLLO0 */
	bits3 = (3 << 4) | (1 << 0); /* CLK_DIV_APOLLO1 */
#elif (APOLLO_PLL_MHZ == 1000)
	bits1 = (125 << 16) | (3 << 8) | (0 << 0);									/* APOLLO_PLL=1000MHz */
	bits2 = (5 << 24) | (5 << 20) | (5 << 16) | (5 << 12) | (2 << 8) | (0 << 4) | (0 << 0); /* CLK_DIV_APOLLO0 */
	bits3 = (3 << 4) | (1 << 0); /* CLK_DIV_APOLLO1 */
#elif (APOLLO_PLL_MHZ == 900)
	bits1 = (150 << 16) | (2 << 8) | (1 << 0);									/* APOLLO_PLL=900MHz */
	bits2 = (5 << 24) | (5 << 20) | (5 << 16) | (5 << 12) | (2 << 8) | (0 << 4) | (0 << 0); /* CLK_DIV_APOLLO0 */
	bits3 = (2 << 4) | (1 << 0); /* CLK_DIV_APOLLO1 */
#elif (APOLLO_PLL_MHZ == 800)
	bits1 = (200 << 16) | (3 << 8) | (1 << 0);									/* APOLLO_PLL=800MHz */
	bits2 = (5 << 24) | (5 << 20) | (5 << 16) | (5 << 12) | (2 << 8) | (0 << 4) | (0 << 0); /* CLK_DIV_APOLLO0 */
	bits3 = (2 << 4) | (1 << 0); /* CLK_DIV_APOLLO1 */
#elif (APOLLO_PLL_MHZ == 700)
	bits1 = (175 << 16) | (3 << 8) | (1 << 0);									/* APOLLO_PLL=700MHz */
	bits2 = (4 << 24) | (4 << 20) | (4 << 16) | (4 << 12) | (2 << 8) | (0 << 4) | (0 << 0); /* CLK_DIV_APOLLO0 */
	bits3 = (2 << 4) | (1 << 0); /* CLK_DIV_APOLLO1 */
#elif (APOLLO_PLL_MHZ == 600)
	bits1 = (100 << 16) | (2 << 8) | (1 << 0);									/* APOLLO_PLL=600MHz */
	bits2 = (4 << 24) | (4 << 20) | (4 << 16) | (4 << 12) | (2 << 8) | (0 << 4) | (0 << 0); /* CLK_DIV_APOLLO0 */
	bits3 = (2 << 4) | (1 << 0); /* CLK_DIV_APOLLO1 */
#elif (APOLLO_PLL_MHZ == 500)
	bits1 = (125 << 16) | (3 << 8) | (1 << 0);									/* APOLLO_PLL=500MHz */
	bits2 = (4 << 24) | (4 << 20) | (4 << 16) | (4 << 12) | (2 << 8) | (0 << 4) | (0 << 0); /* CLK_DIV_APOLLO0 */
	bits3 = (2 << 4) | (1 << 0); /* CLK_DIV_APOLLO1 */
#elif (APOLLO_PLL_MHZ == 400)
	bits1 = (200 << 16) | (3 << 8) | (2 << 0);									/* APOLLO_PLL=400MHz */
	bits2 = (3 << 24) | (3 << 20) | (3 << 16) | (3 << 12) | (2 << 8) | (0 << 4) | (0 << 0); /* CLK_DIV_APOLLO0 */
	bits3 = (2 << 4) | (1 << 0); /* CLK_DIV_APOLLO1 */
#elif (APOLLO_PLL_MHZ == 300)
	bits1 = (100 << 16) | (2 << 8) | (2 << 0);									/* APOLLO_PLL=300MHz */
	bits2 = (3 << 24) | (3 << 20) | (3 << 16) | (3 << 12) | (2 << 8) | (0 << 4) | (0 << 0); /* CLK_DIV_APOLLO0 */
	bits3 = (2 << 4) | (1 << 0); /* CLK_DIV_APOLLO1 */
#elif (APOLLO_PLL_MHZ == 200)
	bits1 = (200 << 16) | (3 << 8) | (3 << 0);									/* APOLLO_PLL=200MHz */
	bits2 = (3 << 24) | (3 << 20) | (3 << 16) | (3 << 12) | (2 << 8) | (0 << 4) | (0 << 0); /* CLK_DIV_APOLLO0 */
	bits3 = (1 << 4) | (1 << 0); /* CLK_DIV_APOLLO1 */
#elif (APOLLO_PLL_MHZ == 100)
	bits1 = (200 << 16) | (3 << 8) | (4 << 0);									/* APOLLO_PLL=100MHz */
	bits2 = (3 << 24) | (3 << 20) | (3 << 16) | (3 << 12) | (2 << 8) | (0 << 4) | (0 << 0); /* CLK_DIV_APOLLO0 */
	bits3 = (2 << 4) | (1 << 0); /* CLK_DIV_APOLLO1 */
#else
	/* printf("check variable &APOLLO_PLL_MHZ"); */
	while (1);
#endif

	bits0 = ((bits1 >> 8) & 0x3F) * 150;
	Outp32(0x11900000, bits0); /* APOLLO_PLL_LOCK */

	bits0 = Inp32(0x11900104); /* APOLLO_PLL_CON1 */
	bits0 = bits0 & ~(0x3 << 6);
	bits0 = bits0 | (2 << 6);
	Outp32(0x11900104, bits0); /* APOLLO_PLL_CON1 */

	/* ENABLE_SDIV_CLKSTOP=0, ENABLE_MDIV_CLKSTOP=1 */
	bits1 = (bits1) | (0 << 26) | (1 << 5);
#if (APOLLO_PLL_ON != 0)
	bits1 = (bits1) | (1 << 31);
#endif
	Outp32(0x11900100, bits1); /* APOLLO_PLL_CON0(PLL1451X) */
#if (APOLLO_PLL_ON != 0)
	WAIT_PLL_LOCK(0x11900100); /* APOLLO_PLL_CON0 */
#endif

	/*
	CNTCLK_APOLLO_RATIO	[27:24]
	PCLK_DBG_APOLLO_RATIO	[23:20]
	ATCLK_APOLLO_RATIO	[19:16]
	PCLK_APOLLO_RATIO [15:12]
	ACLK_APOLLO_RATIO [10:8]
	APOLLO2_RATIO [6:4]
	APOLLO1_RATIO [2:0]
	*/
	Outp32(0x11900600, bits2); /* CLK_DIV_APOLLO0 */

	/*
	SCLK_HPM_APOLLO_RATIO [6:4]
	APOLLO_PLL_RATIO	[2:0]
	*/
	Outp32(0x11900604, bits3); /* CLK_DIV_APOLLO1 */

	/*
	0=OSCCLK, 1=SCLK_BUS0_PLL_APOLLO
	SCLK_BUS0_PLL_APOLLO_USER_SEL	[0]
	*/
	bits0 = (1 << 0);
	Outp32(0x11900204, bits0); /* CLK_MUX_SEL_APOLLO1 */

	/*
	0=MOUT_APOLLO_PLL, 1=MOUT_BUS0_PLL_APOLLO
	APOLLO_SEL	[0]
	*/
	bits0 = (0 << 0);
	Outp32(0x11900208, bits0); /* CLK_MUX_SEL_APOLLO2 */

#if (APOLLO_PLL_ON != 0)
	/*
	0=OSCCLK, 1=FOUT_APOLLO_PLL
	APOLLO_PLL_SEL	[0]
	*/
	bits0 = (1 << 0);
	Outp32(0x11900200, bits0); /* CLK_MUX_SEL_APOLLO0 */
	WAIT_STATUS(0x11900200, 0x11900400); /* CLK_MUX_STAT_APOLLO0 */
#endif

	return 0;
}

int clk_init_mif(void)
{
	unsigned int bits0, bits1, bits2, cal_get_mif;

	Outp32(0x10851000, (1 << 20)); /* PAUSE_MIF0 */
	Outp32(0x10951000, (1 << 20)); /* PAUSE_MIF1 */
	Outp32(0x10A51000, (1 << 20)); /* PAUSE_MIF2 */
	Outp32(0x10B51000, (1 << 20)); /* PAUSE_MIF3 */

	Outp32(0x10570214, 0x00000000); /* CLK_MUX_SEL_TOPC5 */
	WAIT_STATUS_VAL(0x00000000, 0x10570414); /* CLK_MUX_STAT_TOPC5 */

	/*
	SCLK_BUS0_PLL_MIF_RATIO	[6:4]
	*/
	bits0 = Inp32(0x1057060C);
	bits0 &=~(0x7 << 4);
	bits0 |= (7 << 4);
	Outp32(0x1057060C, bits0); /* CLK_DIV_TOPC3 */

#ifdef __KERNEL__
	cal_get_mif = _cal_get_mif();
	if (MEMCLK_1552 == cal_get_mif)
		bits1 = (388 << 16) | (3 << 8) | (0 << 0); /* MIF_PLL=3104MHz */ // MEMCLK = 1552 MHz
	else if (MEMCLK_1456 == cal_get_mif)
		bits1 = (364 << 16) | (3 << 8) | (0 << 0); /* MIF_PLL=2912MHz */ // MEMCLK = 1456 MHz
	else if (MEMCLK_1264 == cal_get_mif)
		bits1 = (316 << 16) | (3 << 8) | (0 << 0); /* MIF_PLL=2528MHz */ //MEMCLK = 1264 MHz
#else
#if (MIF_PLL_MHZ == 3304)
	bits1 = (413 << 16) | (3 << 8) | (0 << 0); /* MIF_PLL=3304MHz */
#elif (MIF_PLL_MHZ == 3200)
	bits1 = (400 << 16) | (3 << 8) | (0 << 0); /* MIF_PLL=3200MHz */
#elif (MIF_PLL_MHZ == 3104)
	bits1 = (388 << 16) | (3 << 8) | (0 << 0); /* MIF_PLL=3104MHz */
#elif (MIF_PLL_MHZ == 3100)
	bits1 = (775 << 16) | (6 << 8) | (0 << 0); /* MIF_PLL=3100MHz */
#elif (MIF_PLL_MHZ == 2912)
	bits1 = (364 << 16) | (3 << 8) | (0 << 0); /* MIF_PLL=2912MHz */
#elif (MIF_PLL_MHZ == 2528)
	bits1 = (316 << 16) | (3 << 8) | (0 << 0); /* MIF_PLL=2528MHz */
#elif (MIF_PLL_MHZ == 2136)
	bits1 = (178 << 16) | (2 << 8) | (0 << 0); /* MIF_PLL=2136MHz */
#elif (MIF_PLL_MHZ == 2052)
	bits1 = (171 << 16) | (2 << 8) | (0 << 0); /* MIF_PLL=2052MHz */
#elif (MIF_PLL_MHZ == 1656)
	bits1 = (207 << 16) | (3 << 8) | (0 << 0); /* MIF_PLL=1656MHz */
#elif (MIF_PLL_MHZ == 1264)
	bits1 = (316 << 16) | (3 << 8) | (1 << 0); /* MIF_PLL=1264MHz */
#elif (MIF_PLL_MHZ == 1086)
	bits1 = (181 << 16) | (2 << 8) | (1 << 0); /* MIF_PLL=1086MHz */
#elif (MIF_PLL_MHZ == 832)
	bits1 = (208 << 16) | (3 << 8) | (1 << 0); /* MIF_PLL=832MHz */
#elif (MIF_PLL_MHZ == 696)
	bits1 = (348 << 16) | (3 << 8) | (2 << 0); /* MIF_PLL=696MHz */
#elif (MIF_PLL_MHZ == 552)
	bits1 = (276 << 16) | (3 << 8) | (2 << 0); /* MIF_PLL=552MHz */
#elif (MIF_PLL_MHZ == 334)
	bits1 = (334 << 16) | (3 << 8) | (3 << 0); /* MIF_PLL=334MHz */
#elif (MIF_PLL_MHZ == 328)
	bits1 = (328 << 16) | (3 << 8) | (3 << 0); /* MIF_PLL=328MHz */
#elif (MIF_PLL_MHZ == 276)
	bits1 = (276 << 16) | (3 << 8) | (3 << 0); /* MIF_PLL=276MHz */
#elif (MIF_PLL_MHZ == 266)
	bits1 = (266 << 16) | (3 << 8) | (3 << 0); /* MIF_PLL=266MHz */
#elif (MIF_PLL_MHZ == 200)
	bits1 = (200 << 16) | (3 << 8) | (3 << 0); /* MIF_PLL=200MHz */
#else
	/* printf("check variable &MIF_PLL_MHZ"); */
	while (1);
#endif
#endif

	bits0 = ((bits1 >> 8) & 0x3F) * 150;
	Outp32(0x10570014, bits0); /* MIF_PLL_LOCK */

	/* ENABLE_SDIV_CLKSTOP=0, ENABLE_MDIV_CLKSTOP=1 */
	bits1 = (bits1) | (0 << 26) | (1 << 5);
#if (MIF_PLL_ON != 0)
	bits1 = (bits1) | (1 << 31);
#endif
	Outp32(0x10570150, bits1); /* MIF_PLL_CON0(PLL1450X) */
#if (MIF_PLL_ON != 0)
	WAIT_PLL_LOCK(0x10570150); /* MIF_PLL_CON0 */
#endif

	/*
	SCLK_HPM_MIF_RATIO	[6:4]MIF_DVFS
	PCLK_MIF_RATIO	[2:0]MIF_DVFS
	*/
	/* bits0 = (1 << 4) | (4 << 0); */
	bits0 = (1 << 4) | (1 << 0);
#ifdef __KERNEL__
	if ((MEMCLK_1552 == cal_get_mif) || (MEMCLK_1456 == cal_get_mif))
		bits0 = (2 << 4) | (1 << 0);
#endif
	Outp32(0x10850600, bits0); /* CLK_DIV_MIF0 */
	Outp32(0x10950600, bits0); /* CLK_DIV_MIF1 */
	Outp32(0x10A50600, bits0); /* CLK_DIV_MIF2 */
	Outp32(0x10B50600, bits0); /* CLK_DIV_MIF3 */

	/*
	0=CLK_PHY0, 1=CLK_PHY0¡¯s 1/2 frequency clock, 2=CLK_PHY0¡¯s 1/4 frequency clock, 3=CLK_PHY0¡¯s 1/8 frequency clock
	SCLK_HPM_MIF_SEL	[9:8]
	PCLK_MIF_SEL	[5:4]

	Apply F/F settings before pll mux setting to guarautee post-dividers margine
	*/
	bits0 = (2 << 8) | (2 << 4);
	Outp32(0x10850200, bits0); /* CLK_MUX_SEL_MIF0 */
	Outp32(0x10950200, bits0); /* CLK_MUX_SEL_MIF1 */
	Outp32(0x10A50200, bits0); /* CLK_MUX_SEL_MIF2 */
	Outp32(0x10B50200, bits0); /* CLK_MUX_SEL_MIF3 */

	/*
	ACLK_MIF_PLL_SEL	[20]

	Controls CLKMUX_MIF0~3_PLL_USER only when PAUSE_MIF0~3[20]=1 0=OSCCLK 1=FOUT_MIF01_PLL for MIF0 and MIF1 FOUT_MIF23_PLL for MIF2 and MIF3
	MIF_PLL_USER_SEL	[16]

	ACLK_MIF_USER_SEL	[12]
	ACLK_MIF_SEL	[8]
	MIF_PLL_SEL	[0]
	*/
	bits0 = (0 << 20) | (1 << 12) | (1 << 8) | (0 << 0);
#if (MIF_PLL_ON != 0)
	bits0 = (bits0) | (1 << 16);
#endif

	Outp32(0x10570214, bits0); /* CLK_MUX_SEL_TOPC5 */
	bits0=bits0&0xFFF;
	bits0 = (bits0) | ((bits0&0xF) << 4) | ((bits0&0xF00) << 4);
	WAIT_STATUS_VAL(bits0, 0x10570414); /* CLK_MUX_STAT_TOPC5 */

	return 0;
}


/*  0=MOUT_TOPC_BUS0_PLL(1600MHz), 1=MOUT_TOPC_BUS0_PLL_DIV2(800MHz), 2=MOUT_TOPC_BUS1_PLL(668MHz), 3=MOUT_TOPC_CCI_PLL(532MHz) */
#define SCLK_BUS0_PLL_MIF_SEL	0
/* MOUT_SCLK_BUS0_PLL_MIF/(SCLK_BUS0_PLL_MIF_RATIO+1) */
#define SCLK_BUS0_PLL_MIF_RATIO 0

int clk_set_mif_after_dmc_init(void)
{
	unsigned int bits;

	bits = Inp32(0x10570214);
	Outp32(0x10570214, bits | (1 << 20)); /* CLK_MUX_SEL_TOPC5	pause=1 */
	WAIT_STATUS_VAL_MASK(0x00000000, 0x10570414, 0x0F000000); /* CLK_MUX_STAT_TOPC5 */

	/*
	mif switching pll changes
	800/(7+1) = 100MHz (low freq init) -> 1600/(0+1)=1600MHz (for dvfs switching)
	*/

	bits = Inp32(0x10570204); /* CLK_MUX_SEL_TOPC1 */
	bits &= ~(3 << 12);
	bits |= ((SCLK_BUS0_PLL_MIF_SEL) << 12);
	Outp32(0x10570204, bits);

	bits = Inp32(0x1057060C); /* CLK_DIV_TOPC3 */
	bits &= ~(0x7 << 4);
	bits |= ((SCLK_BUS0_PLL_MIF_RATIO) << 4);
	Outp32(0x1057060C, bits);

	return 0;
}

#if !defined(SRAM)
int clk_init_g3d(void)
{
	unsigned int bits0, bits1;

	Outp32(0x14AA0200, 0x00000000); /* CLK_MUX_SEL_G3D */
	WAIT_STATUS(0x14AA0200, 0x14AA0400); /* CLK_MUX_STAT_G3D */

#if (G3D_PLL_MHZ == 1000)
	bits1 = (125 << 16) | (3 << 8) | (0 << 0); /* G3D_PLL=1000MHz */
#elif (G3D_PLL_MHZ == 952)
	bits1 = (119 << 16) | (3 << 8) | (0 << 0); /* G3D_PLL=952MHz */
#elif (G3D_PLL_MHZ == 900)
	bits1 = (150 << 16) | (4 << 8) | (0 << 0); /* G3D_PLL=900MHz */
#elif (G3D_PLL_MHZ == 832)
	bits1 = (104 << 16) | (3 << 8) | (0 << 0); /* G3D_PLL=832MHz */
#elif (G3D_PLL_MHZ == 772)
	bits1 = (193 << 16) | (6 << 8) | (0 << 0); /* G3D_PLL=772MHz */
#elif (G3D_PLL_MHZ == 700)
	bits1 = (175 << 16) | (6 << 8) | (0 << 0); /* G3D_PLL=700MHz */
#elif (G3D_PLL_MHZ == 600)
	bits1 = (150 << 16) | (6 << 8) | (0 << 0); /* G3D_PLL=600MHz */
#elif (G3D_PLL_MHZ == 532)
	bits1 = (133 << 16) | (6 << 8) | (0 << 0); /* G3D_PLL=532MHz */
#elif (G3D_PLL_MHZ == 420)
	bits1 = (210 << 16) | (6 << 8) | (1 << 0); /* G3D_PLL=420MHz */
#elif (G3D_PLL_MHZ == 350)
	bits1 = (175 << 16) | (6 << 8) | (1 << 0); /* G3D_PLL=350MHz */
#elif (G3D_PLL_MHZ == 266)
	bits1 = (133 << 16) | (6 << 8) | (1 << 0); /* G3D_PLL=266MHz */
#elif (G3D_PLL_MHZ == 160)
	bits1 = (80 << 16) | (3 << 8) | (2 << 0); /* G3D_PLL=160MHz */
#elif (G3D_PLL_MHZ == 100)
	bits1 = (100 << 16) | (3 << 8) | (3 << 0); /* G3D_PLL=100MHz */
#else
	/* printf("check variable &G3D_PLL_MHZ"); */
	while (1);
#endif

	bits0 = ((bits1 >> 8) & 0x3F) * 150;
	Outp32(0x14AA0000, bits0); /* G3D_PLL_LOCK */

	/* ENABLE_SDIV_CLKSTOP=0, ENABLE_MDIV_CLKSTOP=1 */
	bits1 = (bits1) | (0 << 26) | (1 << 5);
#if (G3D_PLL_ON != 0)
	bits1 = (bits1) | (1 << 31);
#endif
	Outp32(0x14AA0100, bits1); /* G3D_PLL_CON0(PLL1452X) */
#if (G3D_PLL_ON != 0)
	WAIT_PLL_LOCK(0x14AA0100); /* G3D_PLL_CON0 */
#endif

	/*
	SCLK_HPM_G3D_RATIO	[9:8]
	PCLK_G3D_RATIO	[6:4]
	*/
	bits0 = (1 << 8) | (4 << 4);
	Outp32(0x14AA0600, bits0); /* CLK_DIV_G3D */

	/*
	G3D_SEL	[8]	0=MOUT_G3D_PLL, 1=MOUT_BUS_PLL_G3D
	SCLK_BUS_PLL_G3D_USER_SEL	[4]	0=OSCCLK, 1=SCLK_BUS_PLL_G3D

	0=OSCCLK, 1=FOUT_G3D_PLL
	G3D_PLL_SEL	[0]
	*/
	bits0 = (0 << 8) | (1 << 4);
#if (G3D_PLL_ON != 0)
	bits0 = (bits0) | (1 << 0);
#endif
	Outp32(0x14AA0200, bits0); /* CLK_MUX_SEL_G3D */
	WAIT_STATUS(0x14AA0200, 0x14AA0400); /* CLK_MUX_STAT_G3D */

	return 0;
}


int clk_init_cam0(void)
{
	unsigned int bits0;

	/*
	PCLK_CAM0_TREX_133_RATIO	[26:24]
	PCLK_CAM0_TREX_266_RATIO	[22:20]
	PCLK_CAM0_3AA1_234_RATIO	[18:16]
	PCLK_CAM0_3AA0_345_RATIO	[14:12]
	PCLK_CAM0_BNSD_345_RATIO	[10:8]
	PCLK_CAM0_BNSB_345_RATIO	[6:4]
	PCLK_CAM0_BNSA_345_RATIO	[2:0]
	*/
	bits0 = (1 << 24) | (1 << 20) | (1 << 16) | (1 << 12) | (1 << 8) | (1 << 4) | (1 << 0);
	Outp32(0x144D0600, bits0); /* CLK_DIV_CAM0 */

	/*
	0=OSCCLK, 1=ACLK_CAM0_TREX_532
	ACLK_CAM0_TREX_532_USER_SEL	[28]

	0=OSCCLK, 1=ACLK_CAM0_3AA1_468
	ACLK_CAM0_3AA1_468_USER_SEL	[24]

	0=OSCCLK, 1=ACLK_CAM0_3AA0_690
	ACLK_CAM0_3AA0_690_USER_SEL	[20]

	0=OSCCLK, 1=ACLK_CAM0_CSIS1_174
	ACLK_CAM0_CSIS1_174_USER_SEL	[16]

	0=OSCCLK, 1=ACLK_CAM0_BNSD_690
	ACLK_CAM0_BNSD_690_USER_SEL	[12]

	0=OSCCLK, 1=ACLK_CAM0_BNSB_690
	ACLK_CAM0_BNSB_690_USER_SEL	[8]

	0=OSCCLK, 1=ACLK_CAM0_BNSA_690
	ACLK_CAM0_BNSA_690_USER_SEL	[4]

	0=OSCCLK, 1=ACLK_CAM0_CSIS0_690
	ACLK_CAM0_CSIS0_690_USER_SEL	[0]
	*/
	bits0 = (1 << 28) | (1 << 24) | (1 << 20) | (1 << 16) | (1 << 12) | (1 << 8) | (1 << 4) | (1 << 0);
	Outp32(0x144D0200, bits0); /* CLK_MUX_SEL_CAM00 */

	/*
	0=OSCCLK, 1=ACLK_CAM0_CSIS3_133
	ACLK_CAM0_CSIS3_133_USER_SEL	[4]

	0=OSCCLK, 1=ACLK_CAM0_NOCP_133
	ACLK_CAM0_NOCP_133_USER_SEL	[0]
	*/
	bits0 = (1 << 4) | (1 << 0);
	Outp32(0x144D0204, bits0); /* CLK_MUX_SEL_CAM01 */

	/*
	0=OSCCLK, 1=PHYCLK_RXBYTECLKHS0_S1A
	PHYCLK_RXBYTECLKHS0_S1A_USER_SEL	[20]

	0=OSCCLK, 1=PHYCLK_RXBYTECLKHS3_S4
	PHYCLK_RXBYTECLKHS3_S4_USER_SEL	[16]

	0=OSCCLK, 1=PHYCLK_RXBYTECLKHS2_S4
	PHYCLK_RXBYTECLKHS2_S4_USER_SEL	[12]

	0=OSCCLK, 1=PHYCLK_RXBYTECLKHS1_S4
	PHYCLK_RXBYTECLKHS1_S4_USER_SEL	[8]

	0=OSCCLK, 1=PHYCLK_RXBYTECLKHS0_S4
	PHYCLK_RXBYTECLKHS0_S4_USER_SEL	[4]

	0=OSCCLK, 1=PHYCLK_RXBYTECLKHS0_S2A
	PHYCLK_RXBYTECLKHS0_S2A_USER_SEL	[0]
	*/
	bits0 = (1 << 20) | (1 << 16) | (1 << 12) | (1 << 8) | (1 << 4) | (1 << 0);
	Outp32(0x144D0208, bits0); /* CLK_MUX_SEL_CAM02 */

	return 0;
}

int clk_init_cam1(void)
{
	unsigned int bits0;

	/*
	PCLK_CAM1_SCLVRA_246_RATIO	[14:12]
	PCLK_CAM1_BUSPERI_84_RATIO	[10:8]
	PCLK_CAM1_BUSPERI_167_RATIO	[6:4]
	PCLK_CAM1_ARM_167_RATIO	[2:0]
	*/
	bits0 = (1 << 12) | (3 << 8) | (1 << 4) | (3 << 0);
	Outp32(0x145D0600, bits0); /* CLK_DIV_CAM1 */

	/*
	0=OSCCLK, 1=ACLK_CAM1_TREX_532
	ACLK_CAM1_TREX_532_USER_SEL	[20]

	0=OSCCLK, 1=ACLK_CAM1_NOCP_133
	ACLK_CAM1_NOCP_133_USER_SEL	[16]

	0=OSCCLK, 1=ACLK_CAM1_BNSCSIS_133
	ACLK_CAM1_BNSCSIS_133_USER_SEL	[12]

	0=OSCCLK, 1=ACLK_CAM1_BUSPERI_334
	ACLK_CAM1_BUSPERI_334_USER_SEL	[8]

	0=OSCCLK, 1=ACLK_CAM1_ARM_668
	ACLK_CAM1_ARM_668_USER_SEL	[4]

	0=OSCCLK, 1=ACLK_CAM1_SCLVRA_491
	ACLK_CAM1_SCLVRA_491_USER_SEL [0]
	*/
	bits0 = (1 << 20) | (1 << 16) | (1 << 12) | (1 << 8) | (1 << 4) | (1 << 0);
	Outp32(0x145D0200, bits0); /* CLK_MUX_SEL_CAM10 */

	/*
	0=OSCCLK, 1=PHYCLK_HS0_CSIS2_RX_BYTE
	PHYCLK_HS0_CSIS2_RX_BYTE_USER_SEL [28]

	0=OSCCLK, 1=SCLK_ISP_UART
	SCLK_ISP_UART_USER_SEL	[8]

	0=OSCCLK, 1=SCLK_ISP_SPI1
	SCLK_ISP_SPI1_USER_SEL	[4]

	0=OSCCLK, 1=SCLK_ISP_SPI0
	SCLK_ISP_SPI0_USER_SEL	[0]
	*/
	bits0 = (0 << 28) | (0 << 8) | (0 << 4) | (0 << 0);
	Outp32(0x145D0204, bits0); /* CLK_MUX_SEL_CAM11 */

	return 0;
}

int clk_init_isp0(void)
{
	unsigned int bits0;

	/*
	PCLK_ISP0_TREX_133_RATIO	[14:12]
	PCLK_ISP0_TREX_266_RATIO	[10:8]
	PCLK_ISP0_TPU_295_RATIO	[6:4]
	PCLK_ISP0_ISP0_295_RATIO	[2:0]
	*/
	bits0 = (3 << 12) | (1 << 8) | (1 << 4) | (1 << 0);
	Outp32(0x146D0600, bits0); /* CLK_DIV_ISP0 */

	/*
	0=OSCCLK, 1=ACLK_ISP0_TREX_532
	ACLK_ISP0_TREX_532_USER_SEL	[8]

	0=OSCCLK, 1=ACLK_ISP0_TPU_590
	ACLK_ISP0_TPU_590_USER_SEL	[4]

	0=OSCCLK, 1=ACLK_ISP0_ISP0_590
	ACLK_ISP0_ISP0_590_USER_SEL	[0]
	*/
	bits0 = (1 << 8) | (1 << 4) | (1 << 0);
	Outp32(0x146D0200, bits0); /* CLK_MUX_SEL_ISP0 */

	return 0;
}

int clk_init_isp1(void)
{
	unsigned int bits0;

	/*
	PCLK_ISP1_ISP1_234_RATIO	[2:0]
	*/
	bits0 = (1 << 0);
	Outp32(0x147D0600, bits0); /* CLK_DIV_ISP1 */

	/*
	0=OSCCLK, 1=ACLK_ISP1_AHB_117
	ACLK_ISP1_AHB_117_USER_SEL	[4]

	0=OSCCLK, 1=ACLK_ISP1_ISP1_468
	ACLK_ISP1_ISP1_468_USER_SEL	[0]
	*/
	bits0 = (1 << 4) | (1 << 0);
	Outp32(0x147D0200, bits0); /* CLK_MUX_SEL_ISP1 */

	return 0;
}

int clk_init_vpp(void)
{
	unsigned int bits0;

	/*
	PCLK_VPP_133_RATIO	[2:0]
	 */
	bits0 = (2 << 0);
	Outp32(0x13ED0600, bits0); /* CLK_DIV_VPP */

	/*
	0=OSCCLK, 1=ACLK_VPP1_400
	ACLK_VPP1_400_USER_SEL	[4]

	0=OSCCLK, 1=ACLK_VPP0_400
	ACLK_VPP0_400_USER_SEL	[0]
	 */
	bits0 = (1 << 4) | (1 << 0);
	Outp32(0x13ED0200, bits0); /* CLK_MUX_SEL_VPP */

	return 0;
}

int clk_init_disp(void)
{
	unsigned int bits0, bits1;
	int kdiv;

	Outp32(0x13AD0200, 0x00000000); /* CLK_MUX_SEL_DISP0 */
	WAIT_STATUS(0x13AD0200, 0x13AD0400); /* CLK_MUX_STAT_DISP0 */


#if (DISP_PLL_MHZ == 142)
	bits1 = (71 << 16) | (3 << 8) | (2 << 0); /* DISP_PLL=142MHz */
	kdiv=0;
#elif (DISP_PLL_MHZ == 252)
	bits1 = (42 << 16) | (2 << 8) | (1 << 0); /* DISP_PLL=252MHz */
	kdiv=0;
#elif (DISP_PLL_MHZ == 134)
	bits1 = (67 << 16) | (3 << 8) | (2 << 0); /* DISP_PLL=134MHz */
	kdiv=0;
#elif (DISP_PLL_MHZ == 126)
	bits1 = (21 << 16) | (1 << 8) | (2 << 0); /* DISP_PLL=126MHz */
	kdiv=0;
#elif (DISP_PLL_MHZ == 140)
	bits1 = (70 << 16) | (3 << 8) | (2 << 0); /* DISP_PLL=140MHz */
	kdiv=0;
#elif (DISP_PLL_MHZ == 96)
	bits1 = (32 << 16) | (1 << 8) | (3 << 0); /* DISP_PLL=96MHz */
	kdiv=0;
#else
	/* printf("check variable &DISP_PLL_MHZ"); */
	while (1);
#endif

	bits0 = ((bits1 >> 8) & 0x3F) * 3000; /* Fractional mode  */

	Outp32(0x13AD0000, bits0); /* DISP_PLL_LOCK */

	/* ENABLE_SDIV_CLKSTOP=0, ENABLE_MDIV_CLKSTOP=1 */
	bits1 = (bits1) | (0 << 26) | (1 << 5);
#if (DISP_PLL_ON != 0)
	bits1 = (bits1) | (1 << 31);
#endif
	Outp32(0x13AD0100, bits1); /* DISP_PLL_CON0(PLL1460X) */
	bits0 = (kdiv << 0);
	Outp32(0x13AD0104, bits0); /* DISP_PLL_CON1 */
#if (DISP_PLL_ON != 0)
	WAIT_PLL_LOCK(0x13AD0100); /* DISP_PLL_CON0 */
#endif

#if (DPHY_PLL_MHZ == 1500)
	bits1 = (250 << 16) | (2 << 8) | (1 << 0); /* DPHY_PLL=1500MHz */
#elif (DPHY_PLL_MHZ == 1200)
	bits1 = (200 << 16) | (2 << 8) | (1 << 0); /* DPHY_PLL=1200MHz */
#else
	/* printf("check variable &DPHY_PLL_MHZ"); */
	while (1);
#endif

	bits0 = ((bits1 >> 8) & 0x3F) * 150;
	Outp32(0x13AD0004, bits0); /* DPHY_PLL_LOCK */

	/* ENABLE_SDIV_CLKSTOP=0, ENABLE_MDIV_CLKSTOP=1 */
	bits1 = (bits1) | (0 << 26) | (1 << 5);
#if (DPHY_PLL_ON != 0)
	bits1 = (bits1) | (1 << 31);
#endif
	Outp32(0x13AD0110, bits1); /* DPHY_PLL_CON0(PLL1450X) */
#if (DPHY_PLL_ON != 0)
	WAIT_PLL_LOCK(0x13AD0110); /* DPHY_PLL_CON0 */
#endif

	/*
	SCLK_DECON_INT_ECLK_SUB_RATIO	[22:20]
	SCLK_DECON_EXT_ECLK_SUB_RATIO	[18:16]
	SCLK_DECON_INT_EXTCLKPLL_SUB_RATIO	[14:12]
	SCLK_DECON_EXT_EXTCLKPLL_SUB_RATIO	[10:8]
	PCLK_DISP_RATIO	[1:0]
	*/
	bits0 = ((SCLK_DECON_INT_ECLK_SUB_RATIO) << 20)
			| (0 << 16)
			| ((SCLK_DECON_INT_EXTCLKPLL_SUB_RATIO) << 12)
			| (7 << 8) | (2 << 0);
	Outp32(0x13AD0600, bits0); /* CLK_DIV_DISP */

	/*
	0=OSCCLK, 1=FOUT_DISP_PLL
	DISP_PLL_SEL	[28]

	0=OSCCLK, 1=ACLK_DISP_400
	ACLK_DISP_400_USER_SEL	[24]
	*/
	bits0 = (1 << 24);
#if (DISP_PLL_ON != 0)
	bits0 = (bits0) | (1 << 28);
#endif
	Outp32(0x13AD0200, bits0); /* CLK_MUX_SEL_DISP0 */
	WAIT_STATUS(0x13AD0200, 0x13AD0400); /* CLK_MUX_STAT_DISP0 */

	/*
	0=OSCCLK, 1=SCLK_DECON_INT_ECLK
	SCLK_DECON_INT_ECLK_USER_SEL	[28]

	0=OSCCLK, 1=SCLK_DECON_EXT_ECLK
	SCLK_DECON_EXT_ECLK_USER_SEL	[24]

	0=OSCCLK, 1=SCLK_DECON_VCLK
	SCLK_DECON_VCLK_USER_SEL	[20]

	0=OSCCLK, 1=SCLK_DSD
	SCLK_DSD_USER_SEL	[16]
	*/
#if 0// if SCLK_DECON_INT_ECLK ==  SCLK_DECON_VCLK
	bits0 = (0 << 28) | (1 << 24) | (0 << 20) | (1 << 16);
#else
	bits0 = (1 << 28) | (1 << 24) | (0 << 20) | (1 << 16);
#endif
	Outp32(0x13AD0204, bits0); /* CLK_MUX_SEL_DISP1 */

	/*
	0=OSCCLK, 1=PHYCLK_HDMIPHY_PIXEL_CLKO
	PHYCLK_HDMIPHY_PIXEL_CLKO_SEL	[20]

	0=OSCCLK, 1=PHYCLK_HDMIPHY_TMDS_CLKO
	PHYCLK_HDMIPHY_TMDS_CLKO_SEL	[16]

	0=OSCCLK, 1=PHYCLK_MIPIDPHY0_RXCLKESC0
	PHYCLK_MIPIDPHY0_RXCLKESC0_SEL	[12]

	0=OSCCLK, 1=PHYCLK_MIPIDPHY0_BITCLKDIV8
	PHYCLK_MIPIDPHY0_BITCLKDIV8_SEL	[8]

	0=OSCCLK, 1=PHYCLK_MIPIDPHY1_RXCLKESC0
	PHYCLK_MIPIDPHY1_RXCLKESC0_SEL	[4]

	0=OSCCLK, 1=PHYCLK_MIPIDPHY1_BITCLKDIV8
	PHYCLK_MIPIDPHY1_BITCLKDIV8_SEL	[0]
	*/
	bits0 = (1 << 20) | (1 << 16) | (1 << 12) | (1 << 8) | (1 << 4) | (1 << 0);
	Outp32(0x13AD0208, bits0); /* CLK_MUX_SEL_DISP2 */

	/*
	0=MOUT_SCLK_DECON_INT_ECLK_USER, 1=MOUT_DISP_PLL
	SCLK_DECON_INT_ECLK_SUB_SEL	[20]

	0=MOUT_SCLK_DECON_EXT_ECLK_USER, 1=MOUT_DISP_PLL
	SCLK_DECON_EXT_ECLK_SUB_SEL	[16]

	0=MOUT_SCLK_DECON_VCLK_USER, 1=MOUT_DISP_PLL
	SCLK_DECON_INT_EXTCLKPLL_SUB_SEL	[12]

	0=MOUT_SCLK_DECON_VCLK_USER, 1=MOUT_DISP_PLL
	SCLK_DECON_EXT_EXTCLKPLL_SUB_SEL	[8]
	*/
#if 0// if SCLK_DECON_INT_ECLK ==  SCLK_DECON_VCLK
	bits0 = (1 << 20) | (0 << 16) | (1 << 12) | (0 << 8);
#else
	bits0 = (0 << 20) | (0 << 16) | (1 << 12) | (0 << 8);
#endif
	Outp32(0x13AD020C, bits0); /* CLK_MUX_SEL_DISP3 */

	/*
	0=OSCCLK, 1=PHYCLK_DPPHY_CH0_TXD_CLK
	PHYCLK_DPPHY_CH0_TXD_CLK_SEL	[12]

	0=OSCCLK, 1=PHYCLK_DPPHY_CH1_TXD_CLK
	PHYCLK_DPPHY_CH1_TXD_CLK_SEL	[8]

	0=OSCCLK, 1=PHYCLK_DPPHY_CH2_TXD_CLK
	PHYCLK_DPPHY_CH2_TXD_CLK_SEL	[4]

	0=OSCCLK, 1=PHYCLK_DPPHY_CH3_TXD_CLK
	PHYCLK_DPPHY_CH3_TXD_CLK_SEL	[0]
	*/
	bits0 = (1 << 12) | (1 << 8) | (1 << 4) | (1 << 0);
	Outp32(0x13AD0210, bits0); /* CLK_MUX_SEL_DISP4 */

	/*
	0=USER_PHYCLK_HDMIPHY_PIXEL_CLKO, 1=USER_SCLK_DECON_EXT_EXTCLKPLL and 0=SCLK_RGB_VCLK_DSIM1, 1=SCLK_DECON_EXT_VCLK
	DECON_EXT_MODE_SEL	[28]
	*/
	bits0 = (0 << 28);
	Outp32(0x13AD0214, bits0); /* CLK_MUX_SEL_DISP5 */

	return 0;
}

int clk_init_aud(void)
{
	unsigned int bits0;

	/*
	ATCLK_AUD_RATIO	[15:12]
	PCLK_DBG_AUD_RATIO	[11:8]
	ACLK_AUD_RATIO	[7:4]
	AUD_CA5_RATIO	[3:0]
	*/
	bits0 = (2 << 12) | (2 << 8) | (1 << 4) | (1 << 0);
	Outp32(0x114C0600, bits0); /* CLK_DIV_AUD0 */

	/*
	AUD_CDCLK_RATIO	[27:24]
	SCLK_SLIMBUS_RATIO	[20:16]
	SCLK_UART_RATIO	[15:12]
	SCLK_PCM_RATIO	[11:4]
	SCLK_I2S_RATIO	[3:0]
	*/
	bits0 = (1 << 24) | (19 << 16) | (1 << 12) | (49 << 4) | (9 << 0);
	Outp32(0x114C0604, bits0); /* CLK_DIV_AUD1 */

	/*
	0=OSCCLK, 1=FOUT_AUD_PLL
	AUD_PLL_USER_SEL	[20]

	0=DOUT_AUD_CDCLK, 1=IOCLK_AUDIOCDCLK0
	SCLK_PCM_SEL	[16]

	0=DOUT_AUD_CDCLK, 1=IOCLK_AUDIOCDCLK0
	SCLK_I2S_SEL	[12]
	*/
	bits0 = (0 << 16) | (0 << 12);
#if (AUD_PLL_ON != 0)
	bits0 = (bits0) | (1 << 20);
#endif
	Outp32(0x114C0200, bits0); /* CLK_MUX_SEL_AUD */

	return 0;
}


/* FSYS0 is Retention Block. No need to reconfigure */
int clk_init_fsys0(void)
{
	unsigned int bits0, bits1;

	Outp32(0x10E90200, 0x00000000); /* CLK_MUX_SEL_FSYS00 */
	WAIT_STATUS(0x10E90200, 0x10E90400); /* CLK_MUX_STAT_FSYS00 */

#if (HSIC_PLL_MHZ == 480)
	bits1 = (240 << 16) | (6 << 8) | (1 << 0); /* HSIC_PLL=480MHz */
#else
	/* printf("check variable &HSIC_PLL_MHZ"); */
	while (1);
#endif
	bits0 = ((bits1 >> 8) & 0x3F) * 150;
	Outp32(0x10E90000, bits0); /* HSIC_PLL_LOCK */

	/* ENABLE_SDIV_CLKSTOP=0, ENABLE_MDIV_CLKSTOP=1 */
	bits1 = (bits1) | (0 << 26) | (1 << 5);
#if (HSIC_PLL_ON != 0)
	bits1 = (bits1) | (1 << 31);
#endif
	Outp32(0x10E90100, bits1); /* HSIC_PLL_CON0(PLL1452X) */
#if (HSIC_PLL_ON != 0)
	WAIT_PLL_LOCK(0x10E90100); /* HSIC_PLL_CON0 */
#endif

	/*
	ACLK_100_RATIO	[13:12]
	PCLK_COMBO_PHY_MODEM_RATIO	[9:8]
	USB20_RATIO	[7:4]
	*/
	bits0 = (9 << 12) | (9 << 8) | (9 << 4);
	Outp32(0x10E90600, bits0); /* CLK_DIV_FSYS0 */

	/*
	0=OSCCLK, 1=FOUT_HSIC_PLL
	HSIC_PLL_SEL	[28]

	0=OSCCLK, 1=ACLK_FSYS0_200
	ACLK_FSYS0_200_USER_SEL	[24]

	0=OSCCLK, 1=OSCCLK_26M, 2=SCLK_FSYS0_26M, 3=Reserved
	PHYCLK_SEL0	[21:20]
	*/
	bits0 = (1 << 24) | (0 << 20);
#if (HSIC_PLL_ON != 0)
	bits0 = (bits0) | (1 << 28);
#endif
	Outp32(0x10E90200, bits0); /* CLK_MUX_SEL_FSYS00 */
	WAIT_STATUS(0x10E90200, 0x10E90400); /* CLK_MUX_STAT_FSYS00 */

	/*
	0=OSCCLK, 1=SCLK_USBDRD300
	SCLK_USBDRD300_USER_SEL	[28]

	0=OSCCLK, 1=SCLK_MMC2
	SCLK_MMC2_USER_SEL	[24]

	0=OSCCLK, 1=SCLK_PHY_FSYS0
	SCLK_PHY_FSYS0_USER_SEL	[16]
	*/
	bits0 = (0 << 28) | (1 << 24) | (1 << 16);
	Outp32(0x10E90204, bits0); /* CLK_MUX_SEL_FSYS01 */

	/*
	0=OSCCLK, 1=PHYCLK_USBDRD300_UDRD30_PHYCLOCK
	PHYCLK_USBDRD300_UDRD30_PHYCLOCK	[28]

	0=OSCCLK, 1=PHYCLK_USBDRD300_UDRD30_PIPE_PCLK
	PHYCLK_USBDRD300_UDRD30_PIPE_PCLK	[24]

	0=OSCCLK, 1=PHYCLK_USBHOST20_PHY_FREECLK__HSIC1
	PHYCLK_USBHOST20_PHY_FREECLK__HSIC1_USER_SEL	[20]

	0=OSCCLK, 1=PHYCLK_USBHOST20_PHY_PHYCLK__HSIC1
	PHYCLK_USBHOST20_PHY_PHYCLK__HSIC1_USER_SEL	[16]
	*/
	bits0 = (1 << 28) | (1 << 24) | (1 << 20) | (1 << 16);
	Outp32(0x10E90208, bits0); /* CLK_MUX_SEL_FSYS02 */

	/*
	0=OSCCLK, 1=PHYCLK_LLI_TX0_SYMBOL
	PHYCLK_LLI_TX0_SYMBOL_USER_SEL	[20]

	0=OSCCLK, 1=PHYCLK_LLI_RX0_SYMBOL
	PHYCLK_LLI_RX0_SYMBOL_USER_SEL	[16]

	0=OSCCLK, 1=PHYCLK_PCIE_TX0
	PHYCLK_PCIE_TX0_USER_SEL	[12]

	0=OSCCLK, 1=PHYCLK_PCIE_RX0
	PHYCLK_PCIE_RX0_USER_SEL	[8]
	*/
	bits0 = (1 << 20) | (1 << 16) | (1 << 12) | (1 << 8);
	Outp32(0x10E9020C, bits0); /* CLK_MUX_SEL_FSYS03 */

	return 0;
}

/* FSYS1 is Retention Block. No need to reconfigure */
int clk_init_fsys1(void)
{
	unsigned int bits0;

	/*
	PCLK_COMBO_PHY_WIFI1_RATIO	[9:8]
	PCLK_FSYS1_RATIO	[1:0]
	*/
	bits0 = (0 << 8) | (0 << 0);
	Outp32(0x156E0600, bits0); /* CLK_DIV_FSYS1 */

	/*
	0=OSCCLK, 1=ACLK_FSYS1_200
	ACLK_FSYS1_200_USER_SEL	[28]

	0=OSCCLK, 1=OSCCLK_26M, 2=SCLK_FSYS0_26M, 3=Reserved
	PHYCLK_SEL0 [21:20]

	0=OSCCLK_26M, 1=OSCCLK, 2=SCLK_FSYS0_26M, 3=Reserved
	PHYCLK_SEL1 [17:16]
	*/
	bits0 = (1 << 28) | (0 << 20) | (2 << 16);
	Outp32(0x156E0200, bits0); /* CLK_MUX_SEL_FSYS10 */

	/*
	0=OSCCLK, 1=SCLK_MMC0
	SCLK_MMC0_USER_SEL	[28]

	0=OSCCLK, 1=SCLK_MMC1
	SCLK_MMC1_USER_SEL	[24]

	0=OSCCLK, 1=SCLK_UFSUNIPRO20
	SCLK_UFSUNIPRO20_USER_SEL	[20]

	0=OSCCLK, 1=SCLK_PHY_FSYS1
	SCLK_PHY_FSYS1_USER_SEL	[16]

	0=OSCCLK, 1=SCLK_TLX400_WIFI1
	SCLK_TLX400_WIFI1_USER_SEL	[12]
	*/
	bits0 = (1 << 28) | (1 << 24) | (1 << 20) | (1 << 16) | (1 << 12);
	Outp32(0x156E0204, bits0); /* CLK_MUX_SEL_FSYS11 */

	/*
	0=OSCCLK, 1=PHYCLK_UFS20_TX0_SYMBOL
	PHYCLK_UFS20_TX0_SYMBOL_USER_SEL [28]

	0=OSCCLK, 1=PHYCLK_UFS20_RX0_SYMBOL
	PHYCLK_UFS20_RX0_SYMBOL_USER_SEL [24]

	0=OSCCLK, 1=PHYCLK_UFS20_RX1_SYMBOL
	PHYCLK_UFS20_RX1_SYMBOL_USER_SEL [16]

	0=OSCCLK, 1=PHYCLK_PCIE_WIFI1_TX0
	PHYCLK_PCIE_WIFI1_TX0_USER_SEL	[12]

	0=OSCCLK, 1=PHYCLK_PCIE_WIFI1_RX0
	PHYCLK_PCIE_WIFI1_RX0_USER_SEL	[8]
	*/
	bits0 = (1 << 28) | (1 << 24) | (1 << 16) | (1 << 12) | (1 << 8);
	Outp32(0x156E0208, bits0); /* CLK_MUX_SEL_FSYS12 */

	return 0;
}

int clk_init_mscl(void)
{
	unsigned int bits0;

	/*
	PCLK_MSCL_RATIO	[2:0]
	*/
	bits0 = (3 << 0);
	Outp32(0x150D0600, bits0); /* CLK_DIV_MSCL */

	/*
	0=OSCCLK, 1=ACLK_MSCL_532
	ACLK_MSCL_532_USER_SEL	[0]
	*/
	bits0 = (1 << 0);
	Outp32(0x150D0200, bits0); /* CLK_MUX_SEL_MSCL */

	return 0;
}

int clk_init_mfc(void)
{
	unsigned int bits0;

	/*
	PCLK_MFC_PROMISE_RATIO	[5:4]
	PCLK_MFC_RATIO	[1:0]
	*/
	bits0 = (0 << 4) | (3 << 0);
	Outp32(0x15280600, bits0); /* CLK_DIV_MFC */

	/*
	0=OSCCLK, 1=ACLK_MFC_BUS_532
	ACLK_MFC_BUS_532_USER_SEL	[4]

	0=OSCCLK, 1=ACLK_MFC_532
	ACLK_MFC_532_USER_SEL	[0]
	*/
	bits0 = (1 << 4) | (1 << 0);
	Outp32(0x15280200, bits0); /* CLK_MUX_SEL_MFC */

	return 0;
}
#endif

#ifndef CONFIG_SPL_BUILD
int clk_init_root_clock_gating(void)
{
	Outp32(0x10050018, 0x00000001); /* APOLLO_NOC_RCG_EN */
	Outp32(0x10050118, 0x00000001); /* ATLAS_NOC_RCG_EN */
	Outp32(0x1005016C, 0x00000006); /* ATLAS_RCG_EN */
	Outp32(0x10050218, 0x00000001); /* AUD_NOC_RCG_EN */
	Outp32(0x1005021C, 0x00000001); /* AUD_XIU_TOP_RCG_EN */
	Outp32(0x1005023C, 0x00000001); /* AUD_AXI_LH_SI_DRCG_EN */
	Outp32(0x10050240, 0x00000000); /* AUD_CG_OVERRIDE */
	Outp32(0x10050318, 0x007F001F); /* BUS0_RCGEN */
	Outp32(0x10050320, 0x0000007F); /* BUS0_AXI_LH_MI_DRCG_EN */
	Outp32(0x10050418, 0x00FB001C); /* BUS1_RCGEN */
	Outp32(0x10050420, 0x0000003F); /* BUS1_AXI_LH_MI_DRCG_EN */
	Outp32(0x10050518, 0x00000001); /* CAM0_NOC_RCG_EN */
	Outp32(0x1005051C, 0x00000002); /* CAM0_XIU_RCG_EN */
	Outp32(0x10050524, 0x00000F00); /* CAM0_XIU_ASYNC_RCG_EN */
	Outp32(0x10050540, 0x00000001); /* CAM0_AXI_LH_SI_DRCG_EN */
	Outp32(0x10050544, 0x00000000); /* CAM0_CG_OVERRIDE */
	Outp32(0x10050618, 0x00000001); /* CAM1_NOC_RCG_EN */
	Outp32(0x10050624, 0x000010FD); /* CAM1_XIU_ASYNC_RCG_EN */
	Outp32(0x1005064C, 0x00000001); /* CAM1_AXI_LH_SI_DRCG_EN */
	Outp32(0x10050650, 0x00000000); /* CAM1_CG_OVERRIDE */
	Outp32(0x10050714, 0xFFFFFFFF); /* CCORE_RCG_EN */
	Outp32(0x1005071C, 0x00000003); /* CCORE_RCG_EN_1 */
	Outp32(0x10050724, 0x00000003); /* CCORE_QE_ACE_RCG_EN */
	Outp32(0x10050818, 0x00000001); /* DISP_NOC_RCG_EN */
	Outp32(0x1005081C, 0x00000003); /* DISP_XIU_TOP_RCG_EN */
	Outp32(0x100508D0, 0x00000001); /* DISP_AXI_LH_SI_DRCG_EN */
	Outp32(0x100508D4, 0x00000001); /* DISP_AXI_LH_SI_R_DRCG_EN */
	Outp32(0x100508D8, 0x0000000F); /* DISP_QE_AXI_RCG_EN */
	Outp32(0x100508DC, 0x00000000); /* DISP_CG_OVERRIDE */
	Outp32(0x10051018, 0x00000001); /* FSYS0_NOC_RCG_EN */
	Outp32(0x1005101C, 0x0000001F); /* FSYS0_XIU_TOP_RCG_EN */
	Outp32(0x10051020, 0x0000007F); /* FSYS0_AXI_US_RCG_EN */
	Outp32(0x10051068, 0x00000001); /* FSYS0_AXI_LH_SI_DRCG_EN */
	Outp32(0x1005106C, 0x00000001); /* FSYS0_AXI_LH_MI_DRCG_EN */
	Outp32(0x10051074, 0x00000007); /* FSYS0_QE_AXI_RCG_EN */
	Outp32(0x10051078, 0x00000000); /* FSYS0_CG_OVERRIDE */
	Outp32(0x10051134, 0x00000002); /* G2D_QE_AXI_RCG_EN */
	Outp32(0x10051138, 0x00000000); /* G2D_CG_OVERRIDE */
	Outp32(0x10051218, 0x00000001); /* G3D_NOC_RCG_EN */
	Outp32(0x10051240, 0x00000003); /* G3D_ACEL_LH_SI_DRCG_EN */
	Outp32(0x10051244, 0x00000003); /* G3D_QE_ACE_LITE_RCG_EN */
	Outp32(0x1005124C, 0x00000001); /* G3D_XIU_TOP_RCG_EN */
	Outp32(0x10051418, 0x0000000F); /* IMEM_XIU_TOP_RCG_EN */
	Outp32(0x1005141C, 0x00000003); /* IMEM_AXI_US_RCG_EN */
	Outp32(0x10051420, 0x00000003); /* IMEM_XIU_ASYNC_RCG_EN */
	Outp32(0x10051434, 0x00000001); /* IMEM_AXI_LH_SI_DRCG_EN */
	Outp32(0x10051444, 0x0000000F); /* IMEM_QE_AXI_RCG_EN */
	Outp32(0x10051448, 0x00000000); /* IMEM_CG_OVERRIDE */
	Outp32(0x1005151C, 0x00000001); /* ISP0_XIU_TOP_RCG_EN */
	Outp32(0x10051524, 0x00000001); /* ISP0_XIU_ASYNC_RCG_EN */
	Outp32(0x10051538, 0x00000000); /* ISP0_CG_OVERRIDE */
	Outp32(0x1005153C, 0x00000001); /* ISP0_AXI_LH_SI_DRCG_EN */
	Outp32(0x10051618, 0x00000001); /* MFC_NOC_RCG_EN */
	Outp32(0x1005162C, 0x00000003); /* MFC_AXI_LH_SI_DRCG_EN */
	Outp32(0x10051630, 0x00000003); /* MFC_QE_AXI_RCG_EN */
	Outp32(0x10051634, 0x00000000); /* MFC_CG_OVERRIDE */
	Outp32(0x10051724, 0x00000003); /* ISP1_XIU_ASYNC_RCG_EN */
	Outp32(0x10051738, 0x00000000); /* ISP1_CG_OVERRIDE */
	Outp32(0x10051818, 0x00000001); /* MIF0_AXI_ASYNC_MI_RCG_EN */
	Outp32(0x10051824, 0x00000003); /* MIF0_AXI_LH_MI_DRCG_EN */
	Outp32(0x10051828, 0x00000777); /* MIF0_PCLK_DRCG_EN */
	Outp32(0x10051918, 0x00000001); /* MIF1_AXI_ASYNC_MI_RCG_EN */
	Outp32(0x10051924, 0x00000003); /* MIF1_AXI_LH_MI_DRCG_EN */
	Outp32(0x10051928, 0x00000777); /* MIF1_PCLK_DRCG_EN */
	Outp32(0x10052018, 0x00000001); /* MIF2_AXI_ASYNC_MI_RCG_EN */
	Outp32(0x10052024, 0x00000003); /* MIF2_AXI_LH_MI_DRCG_EN */
	Outp32(0x10052028, 0x00000777); /* MIF2_PCLK_DRCG_EN */
	Outp32(0x10052118, 0x00000001); /* MIF3_AXI_ASYNC_MI_RCG_EN */
	Outp32(0x10052124, 0x00000003); /* MIF3_AXI_LH_MI_DRCG_EN */
	Outp32(0x10052128, 0x00000777); /* MIF3_PCLK_DRCG_EN */
	Outp32(0x10052218, 0x00000001); /* MSCL_NOC_RCG_EN */
	Outp32(0x1005221C, 0x00000003); /* MSCL_XIU_TOP_RCG_EN */
	Outp32(0x1005222C, 0x00000003); /* MSCL_AXI_LH_SI_DRCG_EN */
	Outp32(0x10052230, 0x00000007); /* MSCL_QE_AXI_RCG_EN */
	Outp32(0x10052234, 0x00000000); /* MSCL_CG_OVERRIDE */
	Outp32(0x10052318, 0x00000001); /* PERIC_NOC_RCG_EN */
	Outp32(0x10052418, 0x00000001); /* PERIS_NOC_RCG_EN */
	Outp32(0x10052440, 0xFFEF7F70); /* ALL_PPMU_ACG_ENABLE */
	Outp32(0x10052518, 0x00000001); /* VPP_NOC_RCG_EN */
	Outp32(0x1005251C, 0x00000003); /* VPP_XIU_TOP_RCG_EN */
	Outp32(0x10052550, 0x00000003); /* VPP_AXI_LH_SI_DRCG_EN */
	Outp32(0x10052554, 0x0000000F); /* VPP_QE_AXI_RCG_EN */
	Outp32(0x10052558, 0x00000000); /* VPP_CG_OVERRIDE */
	Outp32(0x10052618, 0x00000001); /* FSYS1_NOC_RCG_EN */
	Outp32(0x1005261C, 0x00000007); /* FSYS1_XIU_TOP_RCG_EN */
	Outp32(0x10052620, 0x00000001); /* FSYS1_AXI_US_RCG_EN */
	Outp32(0x10052624, 0x00000003); /* FSYS1_XIU_ASYNC_RCG_EN */
	Outp32(0x10052640, 0x00000001); /* FSYS1_AXI_LH_SI_DRCG_EN */
	Outp32(0x10052648, 0x00000003); /* FSYS1_QE_AXI_RCG_EN */
	Outp32(0x10052818, 0x00000001); /* PERIC1_NOC_RCG_EN */
	Outp32(0x10570D08, 0x00E04BFF); /* CMU_TOPC_ROOTCLKEN_ON */
	Outp32(0x105D0D04, 0xFFFFFFF0); /* CMU_TOP0_ROOTCLKEN_ON0 */
	Outp32(0x105D0D08, 0xF7007FE0); /* CMU_TOP0_ROOTCLKEN_ON1 */
	Outp32(0x105E0D04, 0xFDF80000); /* CMU_TOP1_ROOTCLKEN_ON */

	return 0;
}
#endif


/*
[atlas_pll(cmu_atlas)]
fout_atlas_pll                                     =  900 MHz (en ) (fout_atlas_pll)
[apollo_pll(cmu_apollo)]
fout_apollo_pll                                    =  800 MHz (en ) (fout_apollo_pll)
[top_pll]
fout_mif_pll                                       = 1656 MHz (en ) (fout_mif_pll)
fout_isp_pll                                       =  590 MHz (dis) (fout_isp_pll)
fout_cam_pll                                       =  690 MHz (dis) (fout_cam_pll)
fout_bus0_pll                                      = 1600 MHz (en ) (fout_bus0_pll)
fout_bus1_pll                                      =  668 MHz (en ) (fout_bus1_pll)
fout_cci_pll                                       =  532 MHz (en ) (fout_cci_pll)
fout_mfc_pll                                       =  468 MHz (en ) (fout_mfc_pll)
[g3d_pll(cmu_g3d)]
fout_g3d_pll                                       =  532 MHz (en ) (fout_g3d_pll)
[hsic_pll(cmu_fsys0)]
fout_hsic_pll                                      =  480 MHz (dis) (fout_hsic_pll)
[aud_pll(cmu_aud)]
fout_aud_pll                                       =  196 MHz (en ) (fout_aud_pll)
[disp_pll(cmu_disp)]
fout_disp_pll                                      =  134 MHz (en ) (fout_disp_pll)
fout_dphy_pll                                      = 1200 MHz (dis) (fout_dphy_pll)
[cmu_topc]
dout_sclk_aud_pll                                  =  196 MHz (en ) (fout_aud_pll)
dout_sclk_bus_pll_g3d                              =  400 MHz (en ) (fout_bus0_pll)
dout_sclk_bus0_pll_mif                             = 1600 MHz (en ) (fout_bus0_pll)
mout_sclk_bus0_pll_apollo                          =  800 MHz (en ) (fout_bus0_pll)
mout_sclk_bus0_pll_atlas                           =  800 MHz (en ) (fout_bus0_pll)
sclk_bus0_pll_a                                    =  800 MHz (en ) (fout_bus0_pll)
sclk_bus0_pll_b                                    =  800 MHz (en ) (fout_bus0_pll)
sclk_bus1_pll_a                                    =  668 MHz (en ) (fout_bus1_pll)
sclk_bus1_pll_b                                    =  668 MHz (en ) (fout_bus1_pll)
sclk_cci_pll_a                                     =  532 MHz (en ) (fout_cci_pll)
sclk_cci_pll_b                                     =  532 MHz (en ) (fout_cci_pll)
sclk_mfc_pll_a                                     =  468 MHz (en ) (fout_mfc_pll)
sclk_mfc_pll_b                                     =  468 MHz (en ) (fout_mfc_pll)
dout_aclk_imem_100                                 =  100 MHz (en ) (fout_bus0_pll)
dout_aclk_imem_200                                 =  167 MHz (en ) (fout_bus1_pll)
dout_aclk_imem_266                                 =  266 MHz (en ) (fout_cci_pll)
dout_aclk_bus1_200                                 =  200 MHz (en ) (fout_bus0_pll)
dout_pclk_bus01_133                                =  133 MHz (en ) (fout_bus0_pll)
dout_aclk_bus1_532                                 =  532 MHz (en ) (fout_cci_pll)
dout_aclk_bus0_532                                 =  532 MHz (en ) (fout_cci_pll)
dout_aclk_ccore_133                                =   80 MHz (en ) (fout_bus0_pll)
dout_aclk_ccore_532                                =  532 MHz (en ) (fout_cci_pll)
dout_aclk_peris_66                                 =   66 MHz (en ) (fout_bus0_pll)
dout_aclk_mscl_532                                 =  532 MHz (en ) (fout_cci_pll)
dout_aclk_mfc_532                                  =  532 MHz (en ) (fout_cci_pll)
dout_sclk_cmu_apbif                                =  100 MHz (en ) (fout_bus0_pll)
dout_sclk_cmuc_pause                               =  100 MHz (en ) (fout_bus0_pll)
[cmu_top0]
mout_aclk_mif                                      = 1600 MHz (en ) (fout_bus0_pll)
dout_aclk_disp_400                                 =  400 MHz (en ) (fout_bus0_pll)
dout_aclk_peric0_66                                =   66 MHz (en ) (fout_bus0_pll)
dout_aclk_peric1_66                                =   66 MHz (en ) (fout_bus0_pll)
dout_aclk_vpp0_400                                 =  468 MHz (en ) (fout_mfc_pll)
dout_aclk_vpp1_400                                 =  468 MHz (en ) (fout_mfc_pll)
dout_aclk_isp0_isp0_590                            =  468 MHz (en ) (fout_mfc_pll)
dout_aclk_isp0_tpu_590                             =  468 MHz (en ) (fout_mfc_pll)
dout_aclk_isp0_trex_532                            =  532 MHz (en ) (fout_cci_pll)
dout_aclk_isp1_isp1_468                            =  400 MHz (en ) (fout_bus0_pll)
dout_aclk_isp1_ahb_117                             =  200 MHz (en ) (fout_bus0_pll)
dout_aclk_cam0_csis0_690                           =   24 MHz (en ) (oscclk)
dout_aclk_cam0_bnsa_690                            =   24 MHz (en ) (oscclk)
dout_aclk_cam0_bnsb_690                            =   24 MHz (en ) (oscclk)
dout_aclk_cam0_bnsd_690                            =   24 MHz (en ) (oscclk)
dout_aclk_cam0_csis1_174                           =   12 MHz (en ) (oscclk)
dout_aclk_cam0_csis3_133                           =  133 MHz (en ) (fout_cci_pll)
dout_aclk_cam0_3aa0_690                            =   24 MHz (en ) (oscclk)
dout_aclk_cam0_3aa1_468                            =  468 MHz (en ) (fout_mfc_pll)
dout_aclk_cam0_trex_532                            =  532 MHz (en ) (fout_cci_pll)
dout_aclk_cam0_nocp_133                            =  133 MHz (en ) (fout_cci_pll)
dout_aclk_cam1_sclvra_491                          =  468 MHz (en ) (fout_mfc_pll)
dout_aclk_cam1_arm_668                             =  668 MHz (en ) (fout_bus1_pll)
dout_aclk_cam1_busperi_334                         =  334 MHz (en ) (fout_bus1_pll)
dout_aclk_cam1_bnscsis_133                         =  133 MHz (en ) (fout_cci_pll)
dout_aclk_cam1_nocp_133                            =  133 MHz (en ) (fout_cci_pll)
dout_aclk_cam1_trex_532                            =  532 MHz (en ) (fout_cci_pll)
dout_sclk_decon_int_eclk                           =   25 MHz (en ) (fout_bus0_pll)
dout_sclk_decon_ext_eclk                           =  167 MHz (en ) (fout_bus1_pll)
dout_sclk_decon_vclk                               =   25 MHz (en ) (fout_bus0_pll)
dout_sclk_dsd                                      =  167 MHz (en ) (fout_bus1_pll)
dout_sclk_hdmi_spdif                               =   25 MHz (en ) (fout_bus0_pll)
dout_sclk_isp_spi0                                 =   80 MHz (en ) (fout_bus0_pll)
dout_sclk_isp_spi1                                 =   80 MHz (en ) (fout_bus0_pll)
dout_sclk_isp_uart                                 =  133 MHz (en ) (fout_bus0_pll)
dout_sclk_isp_sensor0                              =   24 MHz (en ) (oscclk)
dout_sclk_isp_sensor1                              =   24 MHz (en ) (oscclk)
dout_sclk_isp_sensor2                              =   24 MHz (en ) (oscclk)
dout_sclk_i2s1                                     =   12 MHz (en ) (fout_aud_pll)
dout_sclk_pcm1                                     =   12 MHz (en ) (fout_aud_pll)
dout_sclk_spdif                                    =   12 MHz (en ) (fout_aud_pll)
dout_sclk_spi0                                     =   80 MHz (en ) (fout_bus0_pll)
dout_sclk_spi1                                     =   80 MHz (en ) (fout_bus0_pll)
dout_sclk_spi2                                     =   80 MHz (en ) (fout_bus0_pll)
dout_sclk_spi3                                     =   80 MHz (en ) (fout_bus0_pll)
dout_sclk_spi4                                     =   80 MHz (en ) (fout_bus0_pll)
dout_sclk_spi5                                     =   80 MHz (en ) (fout_bus0_pll)
dout_sclk_uart0                                    =  133 MHz (en ) (fout_bus0_pll)
dout_sclk_uart1                                    =  133 MHz (en ) (fout_bus0_pll)
dout_sclk_uart2                                    =  133 MHz (en ) (fout_bus0_pll)
dout_sclk_uart3                                    =  133 MHz (en ) (fout_bus0_pll)
[cmu_top1]
dout_aclk_fsys0_200                                =  100 MHz (en ) (fout_bus0_pll)
dout_aclk_fsys1_200                                =  160 MHz (en ) (fout_bus0_pll)
dout_sclk_usbdrd300                                =   50 MHz (en ) (fout_bus0_pll)
dout_sclk_mmc2                                     =   50 MHz (en ) (fout_bus0_pll)
dout_sclk_phy_fsys0                                =  100 MHz (en ) (fout_bus0_pll)
dout_sclk_phy_fsys0_26m                            =   26 MHz (en ) (fout_mfc_pll)
dout_sclk_mmc0                                     =   50 MHz (en ) (fout_bus0_pll)
dout_sclk_mmc1                                     =   50 MHz (en ) (fout_bus0_pll)
dout_sclk_ufsunipro20                              =  334 MHz (en ) (fout_bus1_pll)
dout_sclk_tlx400_wifi1                             =  200 MHz (en ) (fout_bus0_pll)
dout_sclk_phy_fsys1                                =  100 MHz (en ) (fout_bus0_pll)
dout_sclk_phy_fsys1_26m                            =   26 MHz (en ) (fout_mfc_pll)
[cmu_atlas]
dout_atlas1                                        =  900 MHz (en ) (fout_atlas_pll)
dout_atlas2                                        =  900 MHz (en ) (fout_atlas_pll)
dout_aclk_atlas                                    =  450 MHz (en ) (fout_atlas_pll)
dout_pclk_atlas                                    =  128 MHz (en ) (fout_atlas_pll)
dout_atclk_atlas                                   =  128 MHz (en ) (fout_atlas_pll)
dout_pclk_dbg_atlas                                =  128 MHz (en ) (fout_atlas_pll)
dout_cntclk_atlas                                  =  128 MHz (en ) (fout_atlas_pll)
dout_sclk_hpm_atlas                                =  225 MHz (en ) (fout_atlas_pll)
dout_sclk_atlas_pll                                =  450 MHz (en ) (fout_atlas_pll)
[cmu_apollo]
dout_apollo1                                       =  800 MHz (en ) (fout_apollo_pll)
dout_apollo2                                       =  800 MHz (en ) (fout_apollo_pll)
dout_aclk_apollo                                   =  400 MHz (en ) (fout_apollo_pll)
dout_pclk_apollo                                   =  133 MHz (en ) (fout_apollo_pll)
dout_atclk_apollo                                  =  133 MHz (en ) (fout_apollo_pll)
dout_pclk_dbg_apollo                               =  133 MHz (en ) (fout_apollo_pll)
dout_cntclk_apollo                                 =  133 MHz (en ) (fout_apollo_pll)
dout_sclk_hpm_apollo                               =  266 MHz (en ) (fout_apollo_pll)
dout_sclk_apollo_pll                               =  400 MHz (en ) (fout_apollo_pll)
[cmu_g3d]
mout_g3d                                           =  532 MHz (en ) (fout_g3d_pll)
dout_pclk_g3d                                      =  106 MHz (en ) (fout_g3d_pll)
dout_sclk_hpm_g3d                                  =  266 MHz (en ) (fout_g3d_pll)
[cmu_mif]
mout_aclk_mif0_pll_pause0                          = 1656 MHz (en ) (fout_mif_pll)
mout_aclk_mif1_pll_pause0                          = 1656 MHz (en ) (fout_mif_pll)
mout_aclk_mif2_pll_pause0                          = 1656 MHz (en ) (fout_mif_pll)
mout_aclk_mif3_pll_pause0                          = 1656 MHz (en ) (fout_mif_pll)
sclk_drex0_from_ddrphy                             =  207 MHz (en ) (fout_mif_pll)
sclk_drex1_from_ddrphy                             =  207 MHz (en ) (fout_mif_pll)
sclk_drex2_from_ddrphy                             =  207 MHz (en ) (fout_mif_pll)
sclk_drex3_from_ddrphy                             =  207 MHz (en ) (fout_mif_pll)
dout_pclk_mif0                                     =  207 MHz (en ) (fout_mif_pll)
dout_pclk_mif1                                     =  207 MHz (en ) (fout_mif_pll)
dout_pclk_mif2                                     =  207 MHz (en ) (fout_mif_pll)
dout_pclk_mif3                                     =  207 MHz (en ) (fout_mif_pll)
dout_sclk_hpm_mif0                                 =  207 MHz (en ) (fout_mif_pll)
dout_sclk_hpm_mif1                                 =  207 MHz (en ) (fout_mif_pll)
dout_sclk_hpm_mif2                                 =  207 MHz (en ) (fout_mif_pll)
dout_sclk_hpm_mif3                                 =  207 MHz (en ) (fout_mif_pll)
[cmu_ccore]
mout_aclk_ccore_532_user                           =  532 MHz (en ) (fout_cci_pll)
dout_aclk_ccore_266                                =  266 MHz (en ) (fout_cci_pll)
mout_aclk_ccore_133_user                           =   80 MHz (en ) (fout_bus0_pll)
[cmu_imem]
mout_aclk_imem_266_user                            =  266 MHz (en ) (fout_cci_pll)
mout_aclk_imem_200_user                            =  167 MHz (en ) (fout_bus1_pll)
mout_aclk_imem_100_user                            =  100 MHz (en ) (fout_bus0_pll)
[cmu_cam0]
mout_aclk_cam0_csis0_690_user                      =   24 MHz (en ) (oscclk)
mout_aclk_cam0_bnsa_690_user                       =   24 MHz (en ) (oscclk)
dout_pclk_cam0_bnsa_345                            =   12 MHz (en ) (oscclk)
mout_aclk_cam0_bnsb_690_user                       =   24 MHz (en ) (oscclk)
dout_pclk_cam0_bnsb_345                            =   12 MHz (en ) (oscclk)
mout_aclk_cam0_bnsd_690_user                       =   24 MHz (en ) (oscclk)
dout_pclk_cam0_bnsd_345                            =   12 MHz (en ) (oscclk)
mout_aclk_cam0_csis1_174_user                      =   12 MHz (en ) (oscclk)
mout_aclk_cam0_csis3_133_user                      =  133 MHz (en ) (fout_cci_pll)
mout_aclk_cam0_3aa0_690_user                       =   24 MHz (en ) (oscclk)
dout_pclk_cam0_3aa0_345                            =   12 MHz (en ) (oscclk)
mout_aclk_cam0_3aa1_468_user                       =  468 MHz (en ) (fout_mfc_pll)
dout_pclk_cam0_3aa1_234                            =  234 MHz (en ) (fout_mfc_pll)
mout_aclk_cam0_trex_532_user                       =  532 MHz (en ) (fout_cci_pll)
dout_pclk_cam0_trex_266                            =  266 MHz (en ) (fout_cci_pll)
dout_pclk_cam0_trex_133                            =  133 MHz (en ) (fout_cci_pll)
mout_aclk_cam0_nocp_133_user                       =  133 MHz (en ) (fout_cci_pll)
mout_phyclk_rxbyteclkhs0_s2a_user                  =    0 MHz (en ) (phyclk_rxbyteclkhs0_s2a)
mout_phyclk_rxbyteclkhs0_s4_user                   =    0 MHz (en ) (phyclk_rxbyteclkhs0_s4)
mout_phyclk_rxbyteclkhs1_s4_user                   =    0 MHz (en ) (phyclk_rxbyteclkhs1_s4)
mout_phyclk_rxbyteclkhs2_s4_user                   =    0 MHz (en ) (phyclk_rxbyteclkhs2_s4)
mout_phyclk_rxbyteclkhs3_s4_user                   =    0 MHz (en ) (phyclk_rxbyteclkhs3_s4)
mout_phyclk_rxbyteclkhs0_s1a_user                  =    0 MHz (en ) (phyclk_rxbyteclkhs0_s1a)
[cmu_cam1]
mout_aclk_cam1_sclvra_491_user                     =  468 MHz (en ) (fout_mfc_pll)
dout_pclk_cam1_sclvra_246                          =  234 MHz (en ) (fout_mfc_pll)
mout_aclk_cam1_arm_668_user                        =  668 MHz (en ) (fout_bus1_pll)
dout_pclk_cam1_arm_167                             =  167 MHz (en ) (fout_bus1_pll)
mout_aclk_cam1_busperi_334_user                    =  334 MHz (en ) (fout_bus1_pll)
dout_pclk_cam1_busperi_167                         =  167 MHz (en ) (fout_bus1_pll)
dout_pclk_cam1_busperi_84                          =   83 MHz (en ) (fout_bus1_pll)
mout_aclk_cam1_bnscsis_133_user                    =  133 MHz (en ) (fout_cci_pll)
mout_aclk_cam1_nocp_133_user                       =  133 MHz (en ) (fout_cci_pll)
mout_aclk_cam1_trex_532_user                       =  532 MHz (en ) (fout_cci_pll)
mout_sclk_isp_spi0_user                            =   24 MHz (en ) (oscclk)
mout_sclk_isp_spi1_user                            =   24 MHz (en ) (oscclk)
mout_sclk_isp_uart_user                            =   24 MHz (en ) (oscclk)
mout_phyclk_hs0_csis2_rx_byte_user                 =   24 MHz (en ) (oscclk)
[cmu_isp0]
mout_aclk_isp0_isp0_590_user                       =  468 MHz (en ) (fout_mfc_pll)
dout_pclk_isp0_isp0_295                            =  234 MHz (en ) (fout_mfc_pll)
mout_aclk_isp0_tpu_590_user                        =  468 MHz (en ) (fout_mfc_pll)
dout_pclk_isp0_tpu_295                             =  234 MHz (en ) (fout_mfc_pll)
mout_aclk_isp0_trex_532_user                       =  532 MHz (en ) (fout_cci_pll)
dout_pclk_isp0_trex_266                            =  266 MHz (en ) (fout_cci_pll)
dout_pclk_isp0_trex_133                            =  133 MHz (en ) (fout_cci_pll)
[cmu_isp1]
mout_aclk_isp1_isp1_468_user                       =  400 MHz (en ) (fout_bus0_pll)
dout_pclk_isp1_isp1_234                            =  200 MHz (en ) (fout_bus0_pll)
mout_aclk_isp1_ahb_117_user                        =  200 MHz (en ) (fout_bus0_pll)
[cmu_vpp]
mout_aclk_vpp1_400_user                            =  468 MHz (en ) (fout_mfc_pll)
mout_aclk_vpp0_400_user                            =  468 MHz (en ) (fout_mfc_pll)
dout_pclk_vpp_133                                  =  156 MHz (en ) (fout_mfc_pll)
[cmu_disp]
mout_aclk_disp_400_user                            =  400 MHz (en ) (fout_bus0_pll)
dout_pclk_disp                                     =  133 MHz (en ) (fout_bus0_pll)
mout_sclk_dsd_user                                 =  167 MHz (en ) (fout_bus1_pll)
dout_sclk_decon_int_eclk_sub                       =  134 MHz (en ) (fout_disp_pll)
dout_sclk_decon_ext_eclk_sub                       =  167 MHz (en ) (fout_bus1_pll)
dout_sclk_decon_int_extclkpll_sub                  =  134 MHz (en ) (fout_disp_pll)
dout_sclk_decon_ext_extclkpll_sub                  =    3 MHz (en ) (oscclk)
mout_phyclk_hdmiphy_pixel_clko_user                =    0 MHz (en ) (phyclk_hdmiphy_pixel_clko)
mout_phyclk_hdmiphy_tmds_clko_user                 =    0 MHz (en ) (phyclk_hdmiphy_tmds_clko)
mout_phyclk_mipidphy0_rxclkesc0_user               =    0 MHz (en ) (phyclk_mipidphy0_rxclkesc0)
mout_phyclk_mipidphy0_bitclkdiv8_user              =    0 MHz (en ) (phyclk_mipidphy0_bitclkdiv8)
mout_phyclk_mipidphy1_rxclkesc0_user               =    0 MHz (en ) (phyclk_mipidphy1_rxclkesc0)
mout_phyclk_mipidphy1_bitclkdiv8_user              =    0 MHz (en ) (phyclk_mipidphy1_bitclkdiv8)
mout_phyclk_dpphy_ch0_txd_clk_user                 =    0 MHz (en ) (phyclk_dpphy_ch0_txd_clk)
mout_phyclk_dpphy_ch1_txd_clk_user                 =    0 MHz (en ) (phyclk_dpphy_ch1_txd_clk)
mout_phyclk_dpphy_ch2_txd_clk_user                 =    0 MHz (en ) (phyclk_dpphy_ch2_txd_clk)
mout_phyclk_dpphy_ch3_txd_clk_user                 =    0 MHz (en ) (phyclk_dpphy_ch3_txd_clk)
mout_decon_ext0_mode                               =    0 MHz (en ) (phyclk_hdmiphy_pixel_clko)
[cmu_peric0]
mout_aclk_peric0_66_user                           =   66 MHz (en ) (fout_bus0_pll)
mout_sclk_uart0_user                               =  133 MHz (en ) (fout_bus0_pll)
[cmu_peric1]
mout_aclk_peric1_66_user                           =   66 MHz (en ) (fout_bus0_pll)
mout_sclk_uart1_user                               =  133 MHz (en ) (fout_bus0_pll)
mout_sclk_uart2_user                               =   24 MHz (en ) (oscclk)
mout_sclk_uart3_user                               =  133 MHz (en ) (fout_bus0_pll)
mout_sclk_spi0_user                                =   80 MHz (en ) (fout_bus0_pll)
mout_sclk_spi1_user                                =   80 MHz (en ) (fout_bus0_pll)
mout_sclk_spi2_user                                =   80 MHz (en ) (fout_bus0_pll)
mout_sclk_spi3_user                                =   80 MHz (en ) (fout_bus0_pll)
mout_sclk_spi4_user                                =   80 MHz (en ) (fout_bus0_pll)
mout_sclk_spi5_user                                =   80 MHz (en ) (fout_bus0_pll)
[cmu_peris]
mout_aclk_peris_66_user                            =   66 MHz (en ) (fout_bus0_pll)
[cmu_bus0]
mout_aclk_bus0_532_user                            =  532 MHz (en ) (fout_cci_pll)
mout_pclk_bus0_133_user                            =  133 MHz (en ) (fout_bus0_pll)
[cmu_bus1]
mout_aclk_bus1_200_user                            =  200 MHz (en ) (fout_bus0_pll)
mout_aclk_bus1_532_user                            =  532 MHz (en ) (fout_cci_pll)
mout_pclk_bus1_133_user                            =  133 MHz (en ) (fout_bus0_pll)
[cmu_aud]
dout_atclk_aud                                     =   32 MHz (en ) (fout_aud_pll)
dout_sclk_aud_ca5                                  =   98 MHz (en ) (fout_aud_pll)
dout_aclk_aud                                      =   49 MHz (en ) (fout_aud_pll)
dout_pclk_dbg_aud                                  =   32 MHz (en ) (fout_aud_pll)
dout_aud_cdclk                                     =   98 MHz (en ) (fout_aud_pll)
dout_sclk_aud_slimbus                              =    4 MHz (en ) (fout_aud_pll)
dout_sclk_aud_uart                                 =   49 MHz (en ) (fout_aud_pll)
dout_sclk_aud_pcm                                  =    1 MHz (en ) (fout_aud_pll)
dout_sclk_aud_i2s                                  =    9 MHz (en ) (fout_aud_pll)
[cmu_fsys0]
mout_aclk_fsys0_200_user                           =  100 MHz (en ) (fout_bus0_pll)
dout_aclk_fsys0_100                                =   50 MHz (en ) (fout_bus0_pll)
dout_pclk_combo_phy_modem                          =   50 MHz (en ) (fout_bus0_pll)
mout_fsys0_phyclk_sel0                             =   24 MHz (en ) (oscclk)
mout_sclk_usbdrd300_user                           =   24 MHz (en ) (oscclk)
mout_sclk_mmc2_user                                =   50 MHz (en ) (fout_bus0_pll)
mout_sclk_phy_fsys0_user                           =  100 MHz (en ) (fout_bus0_pll)
mout_phyclk_usbdrd300_udrd30_phyclock_user         =    0 MHz (en ) (phyclk_usbdrd300_udrd30_phyclock)
mout_phyclk_usbdrd300_udrd30_pipe_pclk_user        =    0 MHz (en ) (phyclk_usbdrd300_udrd30_pipe_pclk)
mout_phyclk_usbhost20_phy_freeclk__hsic1_user      =    0 MHz (en ) (phyclk_usbhost20_phy_freeclk__hsic1)
mout_phyclk_usbhost20_phy_phyclk__hsic1_user       =    0 MHz (en ) (phyclk_usbhost20_phy_phyclk__hsic1)
mout_phyclk_lli_tx0_symbol_user                    =    0 MHz (en ) (phyclk_lli_tx0_symbol)
mout_phyclk_lli_rx0_symbol_user                    =    0 MHz (en ) (phyclk_lli_rx0_symbol)
mout_phyclk_pcie_tx0_user                          =    0 MHz (en ) (phyclk_pcie_tx0)
mout_phyclk_pcie_rx0_user                          =    0 MHz (en ) (phyclk_pcie_rx0)
mout_hsic_pll_ctrl                                 =   24 MHz (en ) (oscclk)
dout_sclk_usbhost20_clk48mohci                     =    2 MHz (en ) (oscclk)
[cmu_fsys1]
mout_aclk_fsys1_200_user                           =  160 MHz (en ) (fout_bus0_pll)
dout_pclk_fsys1                                    =  160 MHz (en ) (fout_bus0_pll)
mout_fsys1_phyclk_sel0                             =   24 MHz (en ) (oscclk)
mout_fsys1_phyclk_sel1                             =   26 MHz (en ) (fout_mfc_pll)
mout_sclk_mmc0_user                                =   50 MHz (en ) (fout_bus0_pll)
mout_sclk_mmc1_user                                =   50 MHz (en ) (fout_bus0_pll)
mout_sclk_ufsunipro20_user                         =  334 MHz (en ) (fout_bus1_pll)
mout_sclk_phy_fsys1_user                           =  100 MHz (en ) (fout_bus0_pll)
mout_sclk_tlx400_wifi1_user                        =  200 MHz (en ) (fout_bus0_pll)
mout_phyclk_ufs20_tx0_symbol_user                  =    0 MHz (en ) (phyclk_ufs20_tx0_symbol)
mout_phyclk_ufs20_rx0_symbol_user                  =    0 MHz (en ) (phyclk_ufs20_rx0_symbol)
mout_phyclk_ufs20_rx1_symbol_user                  =    0 MHz (en ) (phyclk_ufs20_rx1_symbol)
mout_phyclk_pcie_wifi1_tx0_user                    =    0 MHz (en ) (phyclk_pcie_wifi1_tx0)
mout_phyclk_pcie_wifi1_rx0_user                    =    0 MHz (en ) (phyclk_pcie_wifi1_rx0)
[cmu_mscl]
mout_aclk_mscl_532_user                            =  532 MHz (en ) (fout_cci_pll)
dout_pclk_mscl                                     =  133 MHz (en ) (fout_cci_pll)
[cmu_mfc]
mout_aclk_mfc_532_user                             =  532 MHz (en ) (fout_cci_pll)
dout_pclk_mfc_promise                              =  532 MHz (en ) (fout_cci_pll)
dout_pclk_mfc                                      =  133 MHz (en ) (fout_cci_pll)
*/
