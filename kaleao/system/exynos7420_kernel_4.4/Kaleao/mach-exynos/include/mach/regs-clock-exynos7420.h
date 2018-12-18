/* arch/arm/mach-exynos/include/mach/regs-clock-exynos7420.h
 *
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *
 * Exynos7420 - Clock register definitions
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __ASM_ARCH_REGS_CLOCK_7420_H
#define __ASM_ARCH_REGS_CLOCK_7420_H __FILE__

#include <plat/cpu.h>
#include <mach/map.h>

#define EXYNOS_CLKREG_TOPC(x)		(EXYNOS7420_VA_CMU_TOPC + (x))		/* PA : 0x10570000 */
#define EXYNOS_CLKREG_TOP0(x)		(EXYNOS7420_VA_CMU_TOP0 + (x))		/* PA : 0x105D0000 */
#define EXYNOS_CLKREG_TOP1(x)		(EXYNOS7420_VA_CMU_TOP1 + (x))		/* PA : 0x105E0000 */
#define EXYNOS_CLKREG_ATLAS(x)		(EXYNOS7420_VA_CMU_ATLAS + (x))		/* PA : 0x11800000 */
#define EXYNOS_CLKREG_APOLLO(x)		(EXYNOS7420_VA_CMU_APOLLO + (x))	/* PA : 0x11900000 */
#define EXYNOS_CLKREG_G3D(x)		(EXYNOS7420_VA_CMU_G3D + (x))		/* PA : 0x14AA0000 */
#define EXYNOS_CLKREG_MIF0(x)		(EXYNOS7420_VA_CMU_MIF0 + (x))		/* PA : 0x10850000 */
#define EXYNOS_CLKREG_MIF1(x)		(EXYNOS7420_VA_CMU_MIF1 + (x))		/* PA : 0x10950000 */
#define EXYNOS_CLKREG_MIF2(x)		(EXYNOS7420_VA_CMU_MIF2 + (x))		/* PA : 0x10A50000 */
#define EXYNOS_CLKREG_MIF3(x)		(EXYNOS7420_VA_CMU_MIF3 + (x))		/* PA : 0x10B50000 */
#define EXYNOS_CLKREG_CCORE(x)		(EXYNOS7420_VA_CMU_CCORE + (x))		/* PA : 0x105B0000 */
#define EXYNOS_CLKREG_IMEM(x)		(EXYNOS7420_VA_CMU_IMEM + (x))		/* PA : 0x11060000 */
#define EXYNOS_CLKREG_CAM0(x)		(EXYNOS7420_VA_CMU_CAM0 + (x))		/* PA : 0x144D0000 */
#define EXYNOS_CLKREG_CAM0_LOCAL(x)	(EXYNOS7420_VA_CMU_CAM0_LOCAL + (x))	/* PA : 0x140F0000 */
#define EXYNOS_CLKREG_CAM1(x)		(EXYNOS7420_VA_CMU_CAM1 + (x))		/* PA : 0x146D0000 */
#define EXYNOS_CLKREG_CAM1_LOCAL(x)	(EXYNOS7420_VA_CMU_CAM1_LOCAL + (x))	/* PA : 0x141F0000 */
#define EXYNOS_CLKREG_ISP0(x)		(EXYNOS7420_VA_CMU_ISP0 + (x))		/* PA : 0x146D0000 */
#define EXYNOS_CLKREG_ISP0_LOCAL(x)	(EXYNOS7420_VA_CMU_ISP0_LOCAL + (x))	/* PA : 0x14290000 */
#define EXYNOS_CLKREG_ISP1(x)		(EXYNOS7420_VA_CMU_ISP1 + (x))		/* PA : 0x147D0000 */
#define EXYNOS_CLKREG_ISP1_LOCAL(x)	(EXYNOS7420_VA_CMU_ISP1_LOCAL + (x))	/* PA : 0x142F0000 */
#define EXYNOS_CLKREG_VPP(x)		(EXYNOS7420_VA_CMU_VPP + (x))		/* PA : 0x13ED0000 */
#define EXYNOS_CLKREG_PERIC0(x)		(EXYNOS7420_VA_CMU_PERIC0 + (x))	/* PA : 0x13610000 */
#define EXYNOS_CLKREG_PERIC1(x)		(EXYNOS7420_VA_CMU_PERIC1 + (x))	/* PA : 0x14C80000 */
#define EXYNOS_CLKREG_PERIS(x)		(EXYNOS7420_VA_CMU_PERIS + (x))		/* PA : 0x10040000 */
#define EXYNOS_CLKREG_BUS0(x)		(EXYNOS7420_VA_CMU_BUS0 + (x))		/* PA : 0x13400000 */
#define EXYNOS_CLKREG_BUS1(x)		(EXYNOS7420_VA_CMU_BUS1 + (x))		/* PA : 0x14800000 */
#define EXYNOS_CLKREG_DISP(x)		(EXYNOS7420_VA_CMU_DISP + (x))		/* PA : 0x13AD0000 */
#define EXYNOS_CLKREG_AUD(x)		(EXYNOS7420_VA_CMU_AUD + (x))		/* PA : 0x114C0000 */
#define EXYNOS_CLKREG_FSYS0(x)		(EXYNOS7420_VA_CMU_FSYS0 + (x))		/* PA : 0x10E90000 */
#define EXYNOS_CLKREG_FSYS1(x)		(EXYNOS7420_VA_CMU_FSYS1 + (x))		/* PA : 0x156E0000 */
#define EXYNOS_CLKREG_G2D(x)		(EXYNOS7420_VA_CMU_G2D + (x))		/* PA : 0x12460000 */
#define EXYNOS_CLKREG_MSCL(x)		(EXYNOS7420_VA_CMU_MSCL + (x))		/* PA : 0x150D0000 */
#define EXYNOS_CLKREG_MFC(x)		(EXYNOS7420_VA_CMU_MFC + (x))		/* PA : 0x15280000 */
#define EXYNOS_CLKREG_HEVC(x)		(EXYNOS7420_VA_CMU_HEVC + (x))		/* PA : 0x14F80000 */

#define CCI_LOCK				EXYNOS_CLKREG_TOPC(0x0000)
#define BUS0_LOCK				EXYNOS_CLKREG_TOPC(0x0004)
#define BUS1_LOCK				EXYNOS_CLKREG_TOPC(0x0008)
#define MFC_LOCK				EXYNOS_CLKREG_TOPC(0x000C)
#define AUD_LOCK				EXYNOS_CLKREG_TOPC(0x0010)
#define	MIF_LOCK				EXYNOS_CLKREG_TOPC(0x0014)
#define ISP_LOCK				EXYNOS_CLKREG_TOP0(0x0004)
#define CAM_LOCK				EXYNOS_CLKREG_TOP0(0x0008)
#define CCI_CON					EXYNOS_CLKREG_TOPC(0x0100)
#define BUS0_CON				EXYNOS_CLKREG_TOPC(0x0110)
#define BUS1_CON				EXYNOS_CLKREG_TOPC(0x0120)
#define MFC_CON					EXYNOS_CLKREG_TOPC(0x0130)
#define AUD_CON					EXYNOS_CLKREG_TOPC(0x0140)
#define	AUD_CON1				EXYNOS_CLKREG_TOPC(0x0144)
#define MIF_CON					EXYNOS_CLKREG_TOPC(0x0150)
#define ISP_CON					EXYNOS_CLKREG_TOP0(0x0110)
#define CAM_CON					EXYNOS_CLKREG_TOP0(0x0120)

#define ATLAS_LOCK				EXYNOS_CLKREG_ATLAS(0x0000)
#define APOLLO_LOCK				EXYNOS_CLKREG_APOLLO(0x0000)
#define ATLAS_CON				EXYNOS_CLKREG_ATLAS(0x0100)
#define APOLLO_CON				EXYNOS_CLKREG_APOLLO(0x0100)

#define G3D_LOCK				EXYNOS_CLKREG_G3D(0x0000)
#define G3D_CON					EXYNOS_CLKREG_G3D(0x0100)
#define G3D_CON1				EXYNOS_CLKREG_G3D(0x0104)
#define G3D_CON2				EXYNOS_CLKREG_G3D(0x0108)
#define DISP_LOCK				EXYNOS_CLKREG_DISP(0x0000)
#define DPHY_LOCK				EXYNOS_CLKREG_DISP(0x0004)
#define DISP_CON				EXYNOS_CLKREG_DISP(0x0100)
#define DISP_CON1				EXYNOS_CLKREG_DISP(0x0104)
#define DISP_CON2				EXYNOS_CLKREG_DISP(0x0108)
#define DPHY_CON				EXYNOS_CLKREG_DISP(0x0110)
#define DPHY_CON1				EXYNOS_CLKREG_DISP(0x0114)
#define HSIC_LOCK				EXYNOS_CLKREG_FSYS0(0x0000)
#define HSIC_CON0				EXYNOS_CLKREG_FSYS0(0x0100)
#define HSIC_CON1				EXYNOS_CLKREG_FSYS0(0x0104)

/* topc */
#define EXYNOS7420_MUX_SEL_TOPC0		EXYNOS_CLKREG_TOPC(0x0200)
#define EXYNOS7420_MUX_SEL_TOPC1		EXYNOS_CLKREG_TOPC(0x0204)
#define EXYNOS7420_MUX_SEL_TOPC2		EXYNOS_CLKREG_TOPC(0x0208)
#define EXYNOS7420_MUX_SEL_TOPC3		EXYNOS_CLKREG_TOPC(0x020C)
#define EXYNOS7420_MUX_SEL_TOPC4		EXYNOS_CLKREG_TOPC(0x0210)
#define EXYNOS7420_MUX_SEL_TOPC5		EXYNOS_CLKREG_TOPC(0x0214)

#define EXYNOS7420_MUX_ENABLE_TOPC0		EXYNOS_CLKREG_TOPC(0x0300)
#define EXYNOS7420_MUX_ENABLE_TOPC1		EXYNOS_CLKREG_TOPC(0x0304)
#define EXYNOS7420_MUX_ENABLE_TOPC2		EXYNOS_CLKREG_TOPC(0x0308)
#define EXYNOS7420_MUX_ENABLE_TOPC3		EXYNOS_CLKREG_TOPC(0x030C)
#define EXYNOS7420_MUX_ENABLE_TOPC4		EXYNOS_CLKREG_TOPC(0x0310)
#define EXYNOS7420_MUX_ENABLE_TOPC5		EXYNOS_CLKREG_TOPC(0x0314)

#define EXYNOS7420_MUX_STAT_TOPC0		EXYNOS_CLKREG_TOPC(0x0400)
#define EXYNOS7420_MUX_STAT_TOPC1		EXYNOS_CLKREG_TOPC(0x0404)
#define EXYNOS7420_MUX_STAT_TOPC2		EXYNOS_CLKREG_TOPC(0x0408)
#define EXYNOS7420_MUX_STAT_TOPC3		EXYNOS_CLKREG_TOPC(0x040C)
#define EXYNOS7420_MUX_STAT_TOPC4		EXYNOS_CLKREG_TOPC(0x0410)
#define EXYNOS7420_MUX_STAT_TOPC5		EXYNOS_CLKREG_TOPC(0x0414)

#define EXYNOS7420_DIV_TOPC0			EXYNOS_CLKREG_TOPC(0x0600)
#define EXYNOS7420_DIV_TOPC1			EXYNOS_CLKREG_TOPC(0x0604)
#define EXYNOS7420_DIV_TOPC2			EXYNOS_CLKREG_TOPC(0x0608)
#define EXYNOS7420_DIV_TOPC3			EXYNOS_CLKREG_TOPC(0x060C)

#define EXYNOS7420_DIV_STAT_TOPC0		EXYNOS_CLKREG_TOPC(0x0700)
#define EXYNOS7420_DIV_STAT_TOPC1		EXYNOS_CLKREG_TOPC(0x0704)
#define EXYNOS7420_DIV_STAT_TOPC2		EXYNOS_CLKREG_TOPC(0x0708)
#define EXYNOS7420_DIV_STAT_TOPC3		EXYNOS_CLKREG_TOPC(0x070C)

#define EXYNOS7420_ENABLE_ACLK_TOPC0		EXYNOS_CLKREG_TOPC(0x0800)
#define EXYNOS7420_ENABLE_ACLK_TOPC1		EXYNOS_CLKREG_TOPC(0x0804)
#define EXYNOS7420_ENABLE_ACLK_TOPC2		EXYNOS_CLKREG_TOPC(0x0808)
#define EXYNOS7420_ENABLE_SCLK_TOPC0		EXYNOS_CLKREG_TOPC(0x0A00)
#define EXYNOS7420_ENABLE_SCLK_TOPC1		EXYNOS_CLKREG_TOPC(0x0A04)

#define EXYNOS7420_CLKOUT_ALL_BLK_CMU		EXYNOS_CLKREG_TOPC(0x0C04)
#define EXYNOS7420_CLKOUT_CMU_TOPC		EXYNOS_CLKREG_TOPC(0x0C00)
/* top0 */
#define EXYNOS7420_MUX_SEL_TOP00		EXYNOS_CLKREG_TOP0(0x0200)
#define EXYNOS7420_MUX_SEL_TOP01		EXYNOS_CLKREG_TOP0(0x0204)
#define EXYNOS7420_MUX_SEL_TOP03		EXYNOS_CLKREG_TOP0(0x020C)
#define EXYNOS7420_MUX_SEL_TOP04		EXYNOS_CLKREG_TOP0(0x0210)
#define EXYNOS7420_MUX_SEL_TOP05		EXYNOS_CLKREG_TOP0(0x0214)
#define EXYNOS7420_MUX_SEL_TOP06		EXYNOS_CLKREG_TOP0(0x0218)
#define EXYNOS7420_MUX_SEL_TOP07		EXYNOS_CLKREG_TOP0(0x021C)
#define EXYNOS7420_MUX_SEL_TOP0_DISP		EXYNOS_CLKREG_TOP0(0x0220)
#define EXYNOS7420_MUX_SEL_TOP0_CAM10		EXYNOS_CLKREG_TOP0(0x0224)
#define EXYNOS7420_MUX_SEL_TOP0_CAM11		EXYNOS_CLKREG_TOP0(0x0228)
#define EXYNOS7420_MUX_SEL_TOP0_PERIC0		EXYNOS_CLKREG_TOP0(0x0230)
#define EXYNOS7420_MUX_SEL_TOP0_PERIC1		EXYNOS_CLKREG_TOP0(0x0234)
#define EXYNOS7420_MUX_SEL_TOP0_PERIC2		EXYNOS_CLKREG_TOP0(0x0238)
#define EXYNOS7420_MUX_SEL_TOP0_PERIC3		EXYNOS_CLKREG_TOP0(0x023C)
#define EXYNOS7420_MUX_SEL_TOP0_PERIC4		EXYNOS_CLKREG_TOP0(0x0240)

#define EXYNOS7420_MUX_ENABLE_TOP00		EXYNOS_CLKREG_TOP0(0x0300)
#define EXYNOS7420_MUX_ENABLE_TOP03		EXYNOS_CLKREG_TOP0(0x030C)
#define EXYNOS7420_MUX_ENABLE_TOP04		EXYNOS_CLKREG_TOP0(0x0310)
#define EXYNOS7420_MUX_ENABLE_TOP05		EXYNOS_CLKREG_TOP0(0x0314)
#define EXYNOS7420_MUX_ENABLE_TOP06		EXYNOS_CLKREG_TOP0(0x0318)
#define EXYNOS7420_MUX_ENABLE_TOP07		EXYNOS_CLKREG_TOP0(0x031C)
#define EXYNOS7420_MUX_ENABLE_TOP0_DISP		EXYNOS_CLKREG_TOP0(0x0320)
#define EXYNOS7420_MUX_ENABLE_TOP0_CAM10	EXYNOS_CLKREG_TOP0(0x0324)
#define EXYNOS7420_MUX_ENABLE_TOP0_CAM11	EXYNOS_CLKREG_TOP0(0x0328)
#define EXYNOS7420_MUX_ENABLE_TOP0_PERIC0	EXYNOS_CLKREG_TOP0(0x0330)
#define EXYNOS7420_MUX_ENABLE_TOP0_PERIC1	EXYNOS_CLKREG_TOP0(0x0334)
#define EXYNOS7420_MUX_ENABLE_TOP0_PERIC2	EXYNOS_CLKREG_TOP0(0x0338)
#define EXYNOS7420_MUX_ENABLE_TOP0_PERIC3	EXYNOS_CLKREG_TOP0(0x033C)
#define EXYNOS7420_ENABLE_TOP0_CAM10		EXYNOS_CLKREG_TOP0(0x0324)

#define EXYNOS7420_MUX_STAT_TOP00		EXYNOS_CLKREG_TOP0(0x0400)
#define EXYNOS7420_MUX_STAT_TOP01		EXYNOS_CLKREG_TOP0(0x0404)
#define EXYNOS7420_MUX_STAT_TOP03		EXYNOS_CLKREG_TOP0(0x040C)
#define EXYNOS7420_MUX_STAT_TOP04		EXYNOS_CLKREG_TOP0(0x0410)
#define EXYNOS7420_MUX_STAT_TOP05		EXYNOS_CLKREG_TOP0(0x0414)
#define EXYNOS7420_MUX_STAT_TOP06		EXYNOS_CLKREG_TOP0(0x0418)
#define EXYNOS7420_MUX_STAT_TOP07		EXYNOS_CLKREG_TOP0(0x041C)
#define EXYNOS7420_MUX_STAT_TOP0_DISP		EXYNOS_CLKREG_TOP0(0x0420)
#define EXYNOS7420_MUX_STAT_TOP0_CAM10		EXYNOS_CLKREG_TOP0(0x0424)
#define EXYNOS7420_MUX_STAT_TOP0_CAM11		EXYNOS_CLKREG_TOP0(0x0428)
#define EXYNOS7420_MUX_STAT_TOP0_PERIC1		EXYNOS_CLKREG_TOP0(0x0434)
#define EXYNOS7420_MUX_STAT_TOP0_PERIC2		EXYNOS_CLKREG_TOP0(0x0438)
#define EXYNOS7420_MUX_STAT_TOP0_PERIC3		EXYNOS_CLKREG_TOP0(0x043C)
#define EXYNOS7420_MUX_STAT_TOP0_PERIC4		EXYNOS_CLKREG_TOP0(0x0440)

#define EXYNOS7420_DIV_TOP03			EXYNOS_CLKREG_TOP0(0x060C)
#define EXYNOS7420_DIV_TOP04			EXYNOS_CLKREG_TOP0(0x0610)
#define EXYNOS7420_DIV_TOP05			EXYNOS_CLKREG_TOP0(0x0614)
#define EXYNOS7420_DIV_TOP06			EXYNOS_CLKREG_TOP0(0x0618)
#define EXYNOS7420_DIV_TOP07			EXYNOS_CLKREG_TOP0(0x061C)
#define EXYNOS7420_DIV_TOP0_DISP		EXYNOS_CLKREG_TOP0(0x0620)
#define EXYNOS7420_DIV_TOP0_CAM10		EXYNOS_CLKREG_TOP0(0x0624)
#define EXYNOS7420_DIV_TOP0_CAM11		EXYNOS_CLKREG_TOP0(0x0628)
#define EXYNOS7420_DIV_TOP0_PERIC0		EXYNOS_CLKREG_TOP0(0x0630)
#define EXYNOS7420_DIV_TOP0_PERIC1		EXYNOS_CLKREG_TOP0(0x0634)
#define EXYNOS7420_DIV_TOP0_PERIC2		EXYNOS_CLKREG_TOP0(0x0638)
#define EXYNOS7420_DIV_TOP0_PERIC3		EXYNOS_CLKREG_TOP0(0x063C)
#define EXYNOS7420_DIV_TOP0_PERIC4		EXYNOS_CLKREG_TOP0(0x0640)

#define EXYNOS7420_DIV_STAT_TOP03		EXYNOS_CLKREG_TOP0(0x070C)
#define EXYNOS7420_DIV_STAT_TOP04		EXYNOS_CLKREG_TOP0(0x0710)
#define EXYNOS7420_DIV_STAT_TOP05		EXYNOS_CLKREG_TOP0(0x0714)
#define EXYNOS7420_DIV_STAT_TOP06		EXYNOS_CLKREG_TOP0(0x0718)
#define EXYNOS7420_DIV_STAT_TOP07		EXYNOS_CLKREG_TOP0(0x071C)
#define EXYNOS7420_DIV_STAT_TOP0_DISP		EXYNOS_CLKREG_TOP0(0x0720)
#define EXYNOS7420_DIV_STAT_TOP0_CAM10		EXYNOS_CLKREG_TOP0(0x0724)
#define EXYNOS7420_DIV_STAT_TOP0_CAM11		EXYNOS_CLKREG_TOP0(0x0728)
#define EXYNOS7420_DIV_STAT_TOP0_PERIC0		EXYNOS_CLKREG_TOP0(0x0730)
#define EXYNOS7420_DIV_STAT_TOP0_PERIC1		EXYNOS_CLKREG_TOP0(0x0734)
#define EXYNOS7420_DIV_STAT_TOP0_PERIC2		EXYNOS_CLKREG_TOP0(0x0738)
#define EXYNOS7420_DIV_STAT_TOP0_PERIC3		EXYNOS_CLKREG_TOP0(0x073C)
#define EXYNOS7420_DIV_STAT_TOP0_PERIC4		EXYNOS_CLKREG_TOP0(0x0740)

#define EXYNOS7420_ENABLE_ACLK_TOP02		EXYNOS_CLKREG_TOP0(0x0808)
#define EXYNOS7420_ENABLE_ACLK_TOP03		EXYNOS_CLKREG_TOP0(0x080C)
#define EXYNOS7420_ENABLE_ACLK_TOP04		EXYNOS_CLKREG_TOP0(0x0810)
#define EXYNOS7420_ENABLE_ACLK_TOP05		EXYNOS_CLKREG_TOP0(0x0814)
#define EXYNOS7420_ENABLE_ACLK_TOP06		EXYNOS_CLKREG_TOP0(0x0818)
#define EXYNOS7420_ENABLE_ACLK_TOP07		EXYNOS_CLKREG_TOP0(0x081C)

#define EXYNOS7420_ENABLE_SCLK_TOP0_DISP	EXYNOS_CLKREG_TOP0(0x0A20)
#define EXYNOS7420_ENABLE_SCLK_TOP0_CAM10	EXYNOS_CLKREG_TOP0(0x0A24)
#define EXYNOS7420_ENABLE_SCLK_TOP0_CAM11	EXYNOS_CLKREG_TOP0(0x0A28)
#define EXYNOS7420_ENABLE_SCLK_TOP0_PERIC0	EXYNOS_CLKREG_TOP0(0x0A30)
#define EXYNOS7420_ENABLE_SCLK_TOP0_PERIC1	EXYNOS_CLKREG_TOP0(0x0A34)
#define EXYNOS7420_ENABLE_SCLK_TOP0_PERIC2	EXYNOS_CLKREG_TOP0(0x0A38)
#define EXYNOS7420_ENABLE_SCLK_TOP0_PERIC3	EXYNOS_CLKREG_TOP0(0x0A3C)
#define EXYNOS7420_ENABLE_SCLK_TOP0_PERIC4	EXYNOS_CLKREG_TOP0(0x0A40)

#define EXYNOS7420_CLKOUT_CMU_TOP0		EXYNOS_CLKREG_TOP0(0x0C00)
/* top1 */
#define EXYNOS7420_MUX_SEL_TOP10		EXYNOS_CLKREG_TOP1(0x0200)
#define EXYNOS7420_MUX_SEL_TOP11		EXYNOS_CLKREG_TOP1(0x0204)
#define EXYNOS7420_MUX_SEL_TOP13		EXYNOS_CLKREG_TOP1(0x020C)
#define EXYNOS7420_MUX_SEL_TOP1_FSYS0		EXYNOS_CLKREG_TOP1(0x0224)
#define EXYNOS7420_MUX_SEL_TOP1_FSYS1		EXYNOS_CLKREG_TOP1(0x0228)
#define EXYNOS7420_MUX_SEL_TOP1_FSYS11		EXYNOS_CLKREG_TOP1(0x022C)

#define EXYNOS7420_MUX_ENABLE_TOP10		EXYNOS_CLKREG_TOP1(0x0300)
#define EXYNOS7420_MUX_ENABLE_TOP13		EXYNOS_CLKREG_TOP1(0x030C)
#define EXYNOS7420_MUX_ENABLE_TOP1_FSYS0	EXYNOS_CLKREG_TOP1(0x0324)
#define EXYNOS7420_MUX_ENABLE_TOP1_FSYS1	EXYNOS_CLKREG_TOP1(0x0328)
#define EXYNOS7420_MUX_ENABLE_TOP1_FSYS11	EXYNOS_CLKREG_TOP1(0x032C)

#define EXYNOS7420_MUX_STAT_TOP10		EXYNOS_CLKREG_TOP1(0x0400)
#define EXYNOS7420_MUX_STAT_TOP11		EXYNOS_CLKREG_TOP1(0x0404)
#define EXYNOS7420_MUX_STAT_TOP13		EXYNOS_CLKREG_TOP1(0x040C)
#define EXYNOS7420_MUX_STAT_TOP1_FSYS0		EXYNOS_CLKREG_TOP1(0x0424)
#define EXYNOS7420_MUX_STAT_TOP1_FSYS1		EXYNOS_CLKREG_TOP1(0x0428)
#define EXYNOS7420_MUX_STAT_TOP1_FSYS11		EXYNOS_CLKREG_TOP1(0x042C)

#define EXYNOS7420_DIV_TOP13			EXYNOS_CLKREG_TOP1(0x060C)
#define EXYNOS7420_DIV_TOP1_FSYS0		EXYNOS_CLKREG_TOP1(0x0624)
#define EXYNOS7420_DIV_TOP1_FSYS1		EXYNOS_CLKREG_TOP1(0x0628)
#define EXYNOS7420_DIV_TOP1_FSYS11		EXYNOS_CLKREG_TOP1(0x062C)

#define EXYNOS7420_DIV_STAT_TOP13		EXYNOS_CLKREG_TOP1(0x070C)
#define EXYNOS7420_DIV_STAT_TOP1_FSYS0		EXYNOS_CLKREG_TOP1(0x0724)
#define EXYNOS7420_DIV_STAT_TOP1_FSYS1		EXYNOS_CLKREG_TOP1(0x0728)
#define EXYNOS7420_DIV_STAT_TOP1_FSYS11		EXYNOS_CLKREG_TOP1(0x072C)

#define EXYNOS7420_ENABLE_ACLK_TOP12		EXYNOS_CLKREG_TOP1(0x0808)
#define EXYNOS7420_ENABLE_ACLK_TOP13		EXYNOS_CLKREG_TOP1(0x080C)
#define EXYNOS7420_ENABLE_SCLK_TOP1_FSYS0	EXYNOS_CLKREG_TOP1(0x0A24)
#define EXYNOS7420_ENABLE_SCLK_TOP1_FSYS1	EXYNOS_CLKREG_TOP1(0x0A28)
#define EXYNOS7420_ENABLE_SCLK_TOP1_FSYS11	EXYNOS_CLKREG_TOP1(0x0A2C)
/* atlas */
#define EXYNOS7420_ATLAS_PLL_LOCK		EXYNOS_CLKREG_ATLAS(0x0000)
#define EXYNOS7420_ATLAS_PLL_CON0		EXYNOS_CLKREG_ATLAS(0x0100)
#define EXYNOS7420_ATLAS_PLL_CON1		EXYNOS_CLKREG_ATLAS(0x0104)
#define EXYNOS7420_ATLAS_PLL_CON2		EXYNOS_CLKREG_ATLAS(0x0108)

#define EXYNOS7420_MUX_SEL_ATLAS0		EXYNOS_CLKREG_ATLAS(0x0200)
#define EXYNOS7420_MUX_SEL_ATLAS1		EXYNOS_CLKREG_ATLAS(0x0204)
#define EXYNOS7420_MUX_SEL_ATLAS2		EXYNOS_CLKREG_ATLAS(0x0208)

#define EXYNOS7420_MUX_STAT_ATLAS0		EXYNOS_CLKREG_ATLAS(0x0400)
#define EXYNOS7420_MUX_STAT_ATLAS1		EXYNOS_CLKREG_ATLAS(0x0404)
#define EXYNOS7420_MUX_STAT_ATLAS2		EXYNOS_CLKREG_ATLAS(0x0408)

#define EXYNOS7420_DIV_ATLAS0			EXYNOS_CLKREG_ATLAS(0x0600)
#define EXYNOS7420_DIV_ATLAS1			EXYNOS_CLKREG_ATLAS(0x0604)

#define EXYNOS7420_DIV_STAT_ATLAS0		EXYNOS_CLKREG_ATLAS(0x0700)
#define EXYNOS7420_DIV_STAT_ATLAS1		EXYNOS_CLKREG_ATLAS(0x0704)

#define EXYNOS7420_ENABLE_ACLK_ATLAS		EXYNOS_CLKREG_ATLAS(0x0800)
#define EXYNOS7420_ENABLE_PCLK_ATLAS		EXYNOS_CLKREG_ATLAS(0x0900)
#define EXYNOS7420_ENABLE_SCLK_ATLAS		EXYNOS_CLKREG_ATLAS(0x0A00)
#define EXYNOS7420_ATLAS_ARMCLK_STOPCTRL	EXYNOS_CLKREG_ATLAS(0x1000)
#define EXYNOS7420_ATLAS_PWR_CTRL		EXYNOS_CLKREG_ATLAS(0x1020)
#define EXYNOS7420_ATLAS_PWR_CTRL2		EXYNOS_CLKREG_ATLAS(0x1024)

#define EXYNOS7420_ATLAS_SMPL_CTRL0		EXYNOS_CLKREG_ATLAS(0x0F00)
#define EXYNOS7420_ATLAS_SMPL_CTRL1		EXYNOS_CLKREG_ATLAS(0x0F04)
/* apollo */
#define EXYNOS7420_APOLLO_PLL_LOCK		EXYNOS_CLKREG_APOLLO(0x0000)
#define EXYNOS7420_APOLLO_PLL_CON0		EXYNOS_CLKREG_APOLLO(0x0100)
#define EXYNOS7420_APOLLO_PLL_CON1		EXYNOS_CLKREG_APOLLO(0x0104)
#define EXYNOS7420_APOLLO_PLL_CON2		EXYNOS_CLKREG_APOLLO(0x0108)

#define EXYNOS7420_MUX_SEL_APOLLO0		EXYNOS_CLKREG_APOLLO(0x0200)
#define EXYNOS7420_MUX_SEL_APOLLO1		EXYNOS_CLKREG_APOLLO(0x0204)
#define EXYNOS7420_MUX_SEL_APOLLO2		EXYNOS_CLKREG_APOLLO(0x0208)

#define EXYNOS7420_MUX_STAT_APOLLO0		EXYNOS_CLKREG_APOLLO(0x0400)
#define EXYNOS7420_MUX_STAT_APOLLO1		EXYNOS_CLKREG_APOLLO(0x0404)
#define EXYNOS7420_MUX_STAT_APOLLO2		EXYNOS_CLKREG_APOLLO(0x0408)

#define EXYNOS7420_DIV_APOLLO0			EXYNOS_CLKREG_APOLLO(0x0600)
#define EXYNOS7420_DIV_APOLLO1			EXYNOS_CLKREG_APOLLO(0x0604)

#define EXYNOS7420_DIV_STAT_APOLLO0		EXYNOS_CLKREG_APOLLO(0x0700)
#define EXYNOS7420_DIV_STAT_APOLLO1		EXYNOS_CLKREG_APOLLO(0x0704)

#define EXYNOS7420_ENABLE_ACLK_APOLLO		EXYNOS_CLKREG_APOLLO(0x0800)
#define EXYNOS7420_ENABLE_PCLK_APOLLO		EXYNOS_CLKREG_APOLLO(0x0900)
#define EXYNOS7420_ENABLE_SCLK_APOLLO		EXYNOS_CLKREG_APOLLO(0x0A00)
#define EXYNOS7420_APOLLO_ARMCLK_STOPCTRL	EXYNOS_CLKREG_APOLLO(0x1000)
#define EXYNOS7420_APOLLO_PWR_CTRL		EXYNOS_CLKREG_APOLLO(0x1020)
#define EXYNOS7420_APOLLO_PWR_CTRL2		EXYNOS_CLKREG_APOLLO(0x1024)
/* g3d */
#define EXYNOS7420_MUX_SEL_G3D			EXYNOS_CLKREG_G3D(0x0200)
#define EXYNOS7420_MUX_ENABLE_G3D		EXYNOS_CLKREG_G3D(0x0300)
#define EXYNOS7420_MUX_STAT_G3D			EXYNOS_CLKREG_G3D(0x0400)

#define EXYNOS7420_DIV_G3D			EXYNOS_CLKREG_G3D(0x0600)
#define EXYNOS7420_DIV_STAT_G3D			EXYNOS_CLKREG_G3D(0x0700)

#define EXYNOS7420_ENABLE_ACLK_G3D		EXYNOS_CLKREG_G3D(0x0800)
#define EXYNOS7420_ENABLE_PCLK_G3D		EXYNOS_CLKREG_G3D(0x0900)
#define EXYNOS7420_ENABLE_SCLK_G3D		EXYNOS_CLKREG_G3D(0x0A00)
#define EXYNOS7420_CLK_ENABLE_IP_G3D		EXYNOS_CLKREG_G3D(0x0B00)
/* ccore */
#define EXYNOS7420_MUX_SEL_CCORE		EXYNOS_CLKREG_CCORE(0x0200)
#define EXYNOS7420_MUX_STAT_CCORE		EXYNOS_CLKREG_CCORE(0x0208)
#define EXYNOS7420_DIV_CCORE			EXYNOS_CLKREG_CCORE(0x0600)
#define EXYNOS7420_DIV_STAT_CCORE		EXYNOS_CLKREG_CCORE(0x0700)
#define EXYNOS7420_ENABLE_ACLK_CCORE0		EXYNOS_CLKREG_CCORE(0x0800)
#define EXYNOS7420_ENABLE_ACLK_CCORE1		EXYNOS_CLKREG_CCORE(0x0804)
#define EXYNOS7420_ENABLE_PCLK_CCORE		EXYNOS_CLKREG_CCORE(0x0900)
/* imem */
#define EXYNOS7420_MUX_SEL_IMEM			EXYNOS_CLKREG_IMEM(0x0200)
#define EXYNOS7420_MUX_ENABLE_IMEM		EXYNOS_CLKREG_IMEM(0x0300)
#define EXYNOS7420_MUX_STAT_IMEM		EXYNOS_CLKREG_IMEM(0x0400)

#define EXYNOS7420_ENABLE_ACLK_IMEM0		EXYNOS_CLKREG_IMEM(0x0800)
#define EXYNOS7420_ENABLE_ACLK_IMEM1		EXYNOS_CLKREG_IMEM(0x0804)
#define EXYNOS7420_ENABLE_PCLK_IMEM		EXYNOS_CLKREG_IMEM(0x0900)
/* cam0 */
#define EXYNOS7420_MUX_SEL_CAM00		EXYNOS_CLKREG_CAM0(0x0200)
#define EXYNOS7420_MUX_SEL_CAM01		EXYNOS_CLKREG_CAM0(0x0204)
#define EXYNOS7420_MUX_SEL_CAM02		EXYNOS_CLKREG_CAM0(0x0208)

#define EXYNOS7420_MUX_ENABLE_CAM00		EXYNOS_CLKREG_CAM0(0x0300)
#define EXYNOS7420_MUX_ENABLE_CAM01		EXYNOS_CLKREG_CAM0(0x0304)
#define EXYNOS7420_MUX_ENABLE_CAM02		EXYNOS_CLKREG_CAM0(0x0308)

#define EXYNOS7420_MUX_STAT_CAM00		EXYNOS_CLKREG_CAM0(0x0400)
#define EXYNOS7420_MUX_STAT_CAM01		EXYNOS_CLKREG_CAM0(0x0404)
#define EXYNOS7420_MUX_STAT_CAM02		EXYNOS_CLKREG_CAM0(0x0408)

#define EXYNOS7420_MUX_IGNORE_CAM0		EXYNOS_CLKREG_CAM0(0x0500)

#define EXYNOS7420_DIV_CAM0			EXYNOS_CLKREG_CAM0(0x0600)
#define EXYNOS7420_DIV_STAT_CAM0		EXYNOS_CLKREG_CAM0(0x0700)

#define EXYNOS7420_ENABLE_ACLK_CAM00		EXYNOS_CLKREG_CAM0(0x0800)
#define EXYNOS7420_ENABLE_ACLK_CAM01		EXYNOS_CLKREG_CAM0(0x0804)
#define EXYNOS7420_ENABLE_PCLK_CAM00		EXYNOS_CLKREG_CAM0(0x0900)
#define EXYNOS7420_ENABLE_PCLK_CAM01		EXYNOS_CLKREG_CAM0(0x0904)
#define EXYNOS7420_ENABLE_SCLK_CAM0		EXYNOS_CLKREG_CAM0(0x0A00)

#define EXYNOS7420_ENABLE_IP_CAM00		EXYNOS_CLKREG_CAM0(0x0B00)
#define EXYNOS7420_ENABLE_IP_CAM01		EXYNOS_CLKREG_CAM0(0x0B04)
#define EXYNOS7420_ENABLE_IP_CAM02		EXYNOS_CLKREG_CAM0(0x0B08)

#define EXYNOS7420_CLKOUT_CMU_CAM0		EXYNOS_CLKREG_CAM0(0x0C00)
#define EXYNOS7420_CMU_CAM0_DIV_STAT		EXYNOS_CLKREG_CAM0(0x0C04)

#define EXYNOS7420_ENABLE_ACLK_CAM0_LOCAL	EXYNOS_CLKREG_CAM0_LOCAL(0x0800)
#define EXYNOS7420_ENABLE_PCLK_CAM0_LOCAL	EXYNOS_CLKREG_CAM0_LOCAL(0x0900)
/* cam1 */
#define EXYNOS7420_MUX_SEL_CAM10		EXYNOS_CLKREG_CAM1(0x0200)
#define EXYNOS7420_MUX_SEL_CAM11		EXYNOS_CLKREG_CAM1(0x0204)

#define EXYNOS7420_MUX_ENABLE_CAM10		EXYNOS_CLKREG_CAM1(0x0300)
#define EXYNOS7420_MUX_ENABLE_CAM11		EXYNOS_CLKREG_CAM1(0x0304)

#define EXYNOS7420_MUX_STAT_CAM10		EXYNOS_CLKREG_CAM1(0x0400)
#define EXYNOS7420_MUX_STAT_CAM11		EXYNOS_CLKREG_CAM1(0x0404)

#define EXYNOS7420_MUX_IGNORE_CAM1		EXYNOS_CLKREG_CAM1(0x0500)

#define EXYNOS7420_DIV_CAM1			EXYNOS_CLKREG_CAM1(0x0600)
#define EXYNOS7420_DIV_STAT_CAM1		EXYNOS_CLKREG_CAM1(0x0700)

#define EXYNOS7420_ENABLE_ACLK_CAM10		EXYNOS_CLKREG_CAM1(0x0800)
#define EXYNOS7420_ENABLE_ACLK_CAM11		EXYNOS_CLKREG_CAM1(0x0804)
#define EXYNOS7420_ENABLE_ACLK_CAM12		EXYNOS_CLKREG_CAM1(0x0808)
#define EXYNOS7420_ENABLE_ACLK_CAM13		EXYNOS_CLKREG_CAM1(0x080C)
#define EXYNOS7420_ENABLE_PCLK_CAM10		EXYNOS_CLKREG_CAM1(0x0900)
#define EXYNOS7420_ENABLE_PCLK_CAM11		EXYNOS_CLKREG_CAM1(0x0904)
#define EXYNOS7420_ENABLE_PCLK_CAM12		EXYNOS_CLKREG_CAM1(0x0908)
#define EXYNOS7420_ENABLE_SCLK_CAM10		EXYNOS_CLKREG_CAM1(0x0A00)
#define EXYNOS7420_ENABLE_SCLK_CAM12		EXYNOS_CLKREG_CAM1(0x0A08)

#define EXYNOS7420_ENABLE_IP_CAM10		EXYNOS_CLKREG_CAM1(0x0B00)
#define EXYNOS7420_ENABLE_IP_CAM11		EXYNOS_CLKREG_CAM1(0x0B04)
#define EXYNOS7420_ENABLE_IP_CAM12		EXYNOS_CLKREG_CAM1(0x0B08)

#define EXYNOS7420_CLKOUT_CMU_CAM1		EXYNOS_CLKREG_CAM1(0x0C00)
#define EXYNOS7420_CMU_CAM1_DIV_STAT		EXYNOS_CLKREG_CAM1(0x0C04)

#define EXYNOS7420_ENABLE_ACLK_CAM1_LOCAL	EXYNOS_CLKREG_CAM1_LOCAL(0x0800)
#define EXYNOS7420_ENABLE_PCLK_CAM1_LOCAL	EXYNOS_CLKREG_CAM1_LOCAL(0x0900)
/* isp0 */
#define EXYNOS7420_MUX_SEL_ISP0			EXYNOS_CLKREG_ISP0(0x0200)
#define EXYNOS7420_MUX_ENABLE_ISP0		EXYNOS_CLKREG_ISP0(0x0300)
#define EXYNOS7420_MUX_STAT_ISP0		EXYNOS_CLKREG_ISP0(0x0400)

#define EXYNOS7420_DIV_ISP0			EXYNOS_CLKREG_ISP0(0x0600)
#define EXYNOS7420_DIV_STAT_ISP0		EXYNOS_CLKREG_ISP0(0x0700)

#define EXYNOS7420_ENABLE_ACLK_ISP00		EXYNOS_CLKREG_ISP0(0x0800)
#define EXYNOS7420_ENABLE_ACLK_ISP01		EXYNOS_CLKREG_ISP0(0x0804)
#define EXYNOS7420_ENABLE_PCLK_ISP0		EXYNOS_CLKREG_ISP0(0x0900)

#define EXYNOS7420_ENABLE_IP_ISP0		EXYNOS_CLKREG_ISP0(0x0B00)
#define EXYNOS7420_CLKOUT_CMU_ISP0		EXYNOS_CLKREG_ISP0(0x0C00)
#define EXYNOS7420_CLKOUT_CMU_ISP0_DIV_STAT	EXYNOS_CLKREG_ISP0(0x0C00)

#define EXYNOS7420_ENABLE_ACLK_ISP0_LOCAL0	EXYNOS_CLKREG_ISP0_LOCAL(0x0800)
#define EXYNOS7420_ENABLE_ACLK_ISP0_LOCAL1	EXYNOS_CLKREG_ISP0_LOCAL(0x0804)
#define EXYNOS7420_ENABLE_PCLK_ISP0_LOCAL	EXYNOS_CLKREG_ISP0_LOCAL(0x0900)
/* isp1 */
#define EXYNOS7420_MUX_SEL_ISP1			EXYNOS_CLKREG_ISP1(0x0200)
#define EXYNOS7420_MUX_ENABLE_ISP1		EXYNOS_CLKREG_ISP1(0x0300)
#define EXYNOS7420_MUX_STAT_ISP1		EXYNOS_CLKREG_ISP1(0x0400)

#define EXYNOS7420_DIV_ISP1			EXYNOS_CLKREG_ISP1(0x0600)
#define EXYNOS7420_DIV_STAT_ISP1		EXYNOS_CLKREG_ISP1(0x0700)

#define EXYNOS7420_ENABLE_ACLK_ISP1		EXYNOS_CLKREG_ISP1(0x0800)
#define EXYNOS7420_ENABLE_PCLK_ISP1		EXYNOS_CLKREG_ISP1(0x0900)

#define EXYNOS7420_ENABLE_IP_ISP1		EXYNOS_CLKREG_ISP1(0x0B00)
#define EXYNOS7420_CLKOUT_CMU_ISP1		EXYNOS_CLKREG_ISP1(0x0C00)
#define EXYNOS7420_CLKOUT_CMU_ISP1_DIV_STAT	EXYNOS_CLKREG_ISP1(0x0C00)

#define EXYNOS7420_ENABLE_ACLK_ISP1_LOCAL	EXYNOS_CLKREG_ISP1_LOCAL(0x0800)
#define EXYNOS7420_ENABLE_PCLK_ISP1_LOCAL	EXYNOS_CLKREG_ISP1_LOCAL(0x0900)
/* vpp */
#define EXYNOS7420_MUX_SEL_VPP			EXYNOS_CLKREG_VPP(0x0200)
#define EXYNOS7420_MUX_ENABLE_VPP		EXYNOS_CLKREG_VPP(0x0300)
#define EXYNOS7420_MUX_STAT_VPP			EXYNOS_CLKREG_VPP(0x0400)

#define EXYNOS7420_DIV_VPP			EXYNOS_CLKREG_VPP(0x0600)
#define EXYNOS7420_DIV_STAT_VPP			EXYNOS_CLKREG_VPP(0x0700)

#define EXYNOS7420_ENABLE_ACLK_VPP		EXYNOS_CLKREG_VPP(0x0800)
#define EXYNOS7420_ENABLE_PCLK_VPP		EXYNOS_CLKREG_VPP(0x0900)
/* peric */
#define EXYNOS7420_MUX_SEL_PERIC0		EXYNOS_CLKREG_PERIC0(0x0200)
#define EXYNOS7420_MUX_SEL_PERIC10		EXYNOS_CLKREG_PERIC1(0x0200)
#define EXYNOS7420_MUX_SEL_PERIC11		EXYNOS_CLKREG_PERIC1(0x0204)
#define EXYNOS7420_MUX_SEL_PERIC12		EXYNOS_CLKREG_PERIC1(0x0208)

#define EXYNOS7420_MUX_ENABLE_PERIC0		EXYNOS_CLKREG_PERIC0(0x0300)
#define EXYNOS7420_MUX_ENABLE_PERIC11		EXYNOS_CLKREG_PERIC1(0x0304)

#define EXYNOS7420_MUX_STAT_PERIC0		EXYNOS_CLKREG_PERIC0(0x0400)
#define EXYNOS7420_MUX_STAT_PERIC10		EXYNOS_CLKREG_PERIC1(0x0400)
#define EXYNOS7420_MUX_STAT_PERIC11		EXYNOS_CLKREG_PERIC1(0x0404)
#define EXYNOS7420_MUX_STAT_PERIC12		EXYNOS_CLKREG_PERIC1(0x0408)

#define EXYNOS7420_ENABLE_ACLK_PERIC0		EXYNOS_CLKREG_PERIC0(0x0800)
#define EXYNOS7420_ENABLE_PCLK_PERIC0		EXYNOS_CLKREG_PERIC0(0x0900)
#define EXYNOS7420_ENABLE_SCLK_PERIC0		EXYNOS_CLKREG_PERIC0(0x0A00)

#define EXYNOS7420_ENABLE_ACLK_PERIC1		EXYNOS_CLKREG_PERIC1(0x0800)
#define EXYNOS7420_ENABLE_PCLK_PERIC1		EXYNOS_CLKREG_PERIC1(0x0900)
#define EXYNOS7420_ENABLE_SCLK_PERIC10		EXYNOS_CLKREG_PERIC1(0x0A00)
#define EXYNOS7420_ENABLE_SCLK_PERIC11		EXYNOS_CLKREG_PERIC1(0x0A04)
/* peris */
#define EXYNOS7420_MUX_SEL_PERIS		EXYNOS_CLKREG_PERIS(0x0200)
#define EXYNOS7420_MUX_STAT_PERIS		EXYNOS_CLKREG_PERIS(0x0400)

#define EXYNOS7420_ENABLE_ACLK_PERIS		EXYNOS_CLKREG_PERIS(0x0800)
#define EXYNOS7420_ENABLE_PCLK_PERIS		EXYNOS_CLKREG_PERIS(0x0900)
#define EXYNOS7420_ENABLE_SCLK_PERIS		EXYNOS_CLKREG_PERIS(0x0A00)

#define EXYNOS7420_ENABLE_PCLK_PERIS_SECURE_RTC EXYNOS_CLKREG_PERIS(0x090C)
#define EXYNOS7420_ENABLE_SCLK_PERIS_SECURE_RTC EXYNOS_CLKREG_PERIS(0x0A0C)
/* bus0 */
#define EXYNOS7420_MUX_SEL_BUS0 		EXYNOS_CLKREG_BUS0(0x0200)
#define EXYNOS7420_MUX_STAT_BUS0		EXYNOS_CLKREG_BUS0(0x0400)

#define EXYNOS7420_ENABLE_ACLK_BUS0		EXYNOS_CLKREG_BUS0(0x0800)
#define EXYNOS7420_ENABLE_PCLK_BUS0		EXYNOS_CLKREG_BUS0(0x0900)
/* bus1 */
#define EXYNOS7420_MUX_SEL_BUS1			EXYNOS_CLKREG_BUS1(0x0200)
#define EXYNOS7420_MUX_ENABLE_BUS1		EXYNOS_CLKREG_BUS1(0x0204)
#define EXYNOS7420_MUX_STAT_BUS1		EXYNOS_CLKREG_BUS1(0x0208)

#define EXYNOS7420_ENABLE_ACLK_BUS1		EXYNOS_CLKREG_BUS1(0x0800)
#define EXYNOS7420_ENABLE_PCLK_BUS1		EXYNOS_CLKREG_BUS1(0x0900)
/* disp */
#define EXYNOS7420_MUX_SEL_DISP0		EXYNOS_CLKREG_DISP(0x0200)
#define EXYNOS7420_MUX_SEL_DISP1		EXYNOS_CLKREG_DISP(0x0204)
#define EXYNOS7420_MUX_SEL_DISP2		EXYNOS_CLKREG_DISP(0x0208)
#define EXYNOS7420_MUX_SEL_DISP3		EXYNOS_CLKREG_DISP(0x020C)
#define EXYNOS7420_MUX_SEL_DISP4		EXYNOS_CLKREG_DISP(0x0210)
#define EXYNOS7420_MUX_SEL_DISP5		EXYNOS_CLKREG_DISP(0x0214)

#define EXYNOS7420_MUX_ENABLE_DISP0		EXYNOS_CLKREG_DISP(0x0300)
#define EXYNOS7420_MUX_ENABLE_DISP1		EXYNOS_CLKREG_DISP(0x0304)
#define EXYNOS7420_MUX_ENABLE_DISP2		EXYNOS_CLKREG_DISP(0x0308)
#define EXYNOS7420_MUX_ENABLE_DISP3		EXYNOS_CLKREG_DISP(0x030C)

#define EXYNOS7420_MUX_STAT_DISP0		EXYNOS_CLKREG_DISP(0x0400)
#define EXYNOS7420_MUX_STAT_DISP1		EXYNOS_CLKREG_DISP(0x0404)
#define EXYNOS7420_MUX_STAT_DISP2		EXYNOS_CLKREG_DISP(0x0408)
#define EXYNOS7420_MUX_STAT_DISP3		EXYNOS_CLKREG_DISP(0x040C)

#define EXYNOS7420_DIV_DISP			EXYNOS_CLKREG_DISP(0x0600)
#define EXYNOS7420_DIV_STAT_DISP		EXYNOS_CLKREG_DISP(0x0700)

#define EXYNOS7420_ENABLE_ACLK_DISP		EXYNOS_CLKREG_DISP(0x0800)
#define EXYNOS7420_ENABLE_PCLK_DISP		EXYNOS_CLKREG_DISP(0x0900)
#define EXYNOS7420_ENABLE_SCLK_DISP1		EXYNOS_CLKREG_DISP(0x0A04)
#define EXYNOS7420_ENABLE_SCLK_DISP2		EXYNOS_CLKREG_DISP(0x0A08)
#define EXYNOS7420_ENABLE_SCLK_DISP4		EXYNOS_CLKREG_DISP(0x0A10)

#define EXYNOS7420_ENABLE_ACLK_DISP_RO_SMMU	EXYNOS_CLKREG_DISP(0x0804)
#define EXYNOS7420_ENABLE_ACLK_DISP_RW_SMMU	EXYNOS_CLKREG_DISP(0x0808)
#define EXYNOS7420_ENABLE_ACLK_DISP_RO_SFW	EXYNOS_CLKREG_DISP(0x080C)
#define EXYNOS7420_ENABLE_ACLK_DISP_RW_SFW	EXYNOS_CLKREG_DISP(0x0810)
#define EXYNOS7420_ENABLE_PCLK_DISP_RO_SMMU	EXYNOS_CLKREG_DISP(0x0904)
#define EXYNOS7420_ENABLE_PCLK_DISP_RW_SMMU	EXYNOS_CLKREG_DISP(0x0908)
#define EXYNOS7420_ENABLE_PCLK_DISP_RO_SFW	EXYNOS_CLKREG_DISP(0x090C)
#define EXYNOS7420_ENABLE_PCLK_DISP_RW_SFW	EXYNOS_CLKREG_DISP(0x0910)

/* aud */
#define EXYNOS7420_MUX_SEL_AUD			EXYNOS_CLKREG_AUD(0x0200)
#define EXYNOS7420_MUX_ENABLE_AUD		EXYNOS_CLKREG_AUD(0x0300)
#define EXYNOS7420_MUX_STAT_AUD			EXYNOS_CLKREG_AUD(0x0400)

#define EXYNOS7420_DIV_AUD0			EXYNOS_CLKREG_AUD(0x0600)
#define EXYNOS7420_DIV_AUD1			EXYNOS_CLKREG_AUD(0x0604)

#define EXYNOS7420_DIV_STAT_AUD0		EXYNOS_CLKREG_AUD(0x0700)
#define EXYNOS7420_DIV_STAT_AUD1		EXYNOS_CLKREG_AUD(0x0704)

#define EXYNOS7420_ENABLE_ACLK_AUD		EXYNOS_CLKREG_AUD(0x0800)
#define EXYNOS7420_ENABLE_PCLK_AUD		EXYNOS_CLKREG_AUD(0x0900)
#define EXYNOS7420_ENABLE_SCLK_AUD		EXYNOS_CLKREG_AUD(0x0A00)

#define EXYNOS7420_OUT_CMU_AUD			EXYNOS_CLKREG_AUD(0x0C00)
#define EXYNOS7420_OUT_CMU_AUD_DIV_STAT		EXYNOS_CLKREG_AUD(0x0C04)
/* fsys0 */
#define EXYNOS7420_MUX_SEL_FSYS00		EXYNOS_CLKREG_FSYS0(0x0200)
#define EXYNOS7420_MUX_SEL_FSYS01		EXYNOS_CLKREG_FSYS0(0x0204)
#define EXYNOS7420_MUX_SEL_FSYS02		EXYNOS_CLKREG_FSYS0(0x0208)
#define EXYNOS7420_MUX_SEL_FSYS03		EXYNOS_CLKREG_FSYS0(0x020C)

#define EXYNOS7420_MUX_ENABLE_FSYS00		EXYNOS_CLKREG_FSYS0(0x0300)
#define EXYNOS7420_MUX_ENABLE_FSYS01		EXYNOS_CLKREG_FSYS0(0x0304)

#define EXYNOS7420_MUX_STAT_FSYS00		EXYNOS_CLKREG_FSYS0(0x0400)
#define EXYNOS7420_MUX_STAT_FSYS01		EXYNOS_CLKREG_FSYS0(0x0404)
#define EXYNOS7420_MUX_STAT_FSYS02		EXYNOS_CLKREG_FSYS0(0x0408)
#define EXYNOS7420_MUX_STAT_FSYS03		EXYNOS_CLKREG_FSYS0(0x040C)

#define EXYNOS7420_DIV_FSYS0			EXYNOS_CLKREG_FSYS0(0x0600)
#define EXYNOS7420_DIV_STAT_FSYS0		EXYNOS_CLKREG_FSYS0(0x0700)

#define EXYNOS7420_ENABLE_ACLK_FSYS00		EXYNOS_CLKREG_FSYS0(0x0800)
#define EXYNOS7420_ENABLE_ACLK_FSYS01		EXYNOS_CLKREG_FSYS0(0x0804)
#define EXYNOS7420_ENABLE_PCLK_FSYS0		EXYNOS_CLKREG_FSYS0(0x0900)
#define EXYNOS7420_ENABLE_SCLK_FSYS01		EXYNOS_CLKREG_FSYS0(0x0A04)
#define EXYNOS7420_ENABLE_SCLK_FSYS02		EXYNOS_CLKREG_FSYS0(0x0A08)
#define EXYNOS7420_ENABLE_SCLK_FSYS03		EXYNOS_CLKREG_FSYS0(0x0A0C)
#define EXYNOS7420_ENABLE_SCLK_FSYS04		EXYNOS_CLKREG_FSYS0(0x0A10)

/* fsys1 */
#define EXYNOS7420_MUX_SEL_FSYS10		EXYNOS_CLKREG_FSYS1(0x0200)
#define EXYNOS7420_MUX_SEL_FSYS11		EXYNOS_CLKREG_FSYS1(0x0204)
#define EXYNOS7420_MUX_SEL_FSYS12		EXYNOS_CLKREG_FSYS1(0x0208)

#define EXYNOS7420_MUX_ENABLE_FSYS11		EXYNOS_CLKREG_FSYS1(0x0304)

#define EXYNOS7420_MUX_STAT_FSYS10		EXYNOS_CLKREG_FSYS1(0x0400)
#define EXYNOS7420_MUX_STAT_FSYS11		EXYNOS_CLKREG_FSYS1(0x0404)
#define EXYNOS7420_MUX_STAT_FSYS12		EXYNOS_CLKREG_FSYS1(0x0408)

#define EXYNOS7420_DIV_FSYS1			EXYNOS_CLKREG_FSYS1(0x0600)
#define EXYNOS7420_DIV_STAT_FSYS1		EXYNOS_CLKREG_FSYS1(0x0700)

#define EXYNOS7420_ENABLE_ACLK_FSYS1		EXYNOS_CLKREG_FSYS1(0x0800)
#define EXYNOS7420_ENABLE_PCLK_FSYS1		EXYNOS_CLKREG_FSYS1(0x0900)
#define EXYNOS7420_ENABLE_SCLK_FSYS11		EXYNOS_CLKREG_FSYS1(0x0A04)
#define EXYNOS7420_ENABLE_SCLK_FSYS12		EXYNOS_CLKREG_FSYS1(0x0A08)
#define EXYNOS7420_ENABLE_SCLK_FSYS13		EXYNOS_CLKREG_FSYS1(0x0A0C)

/* g2d */
#define EXYNOS7420_MUX_SEL_G2D			EXYNOS_CLKREG_G2D(0x0200)
#define EXYNOS7420_MUX_ENABLE_G2D		EXYNOS_CLKREG_G2D(0x0300)
#define EXYNOS7420_MUX_STAT_G2D			EXYNOS_CLKREG_G2D(0x0400)

#define EXYNOS7420_DIV_G2D			EXYNOS_CLKREG_G2D(0x0600)
#define EXYNOS7420_DIV_STAT_G2D			EXYNOS_CLKREG_G2D(0x0700)

#define EXYNOS7420_ENABLE_ACLK_G2D		EXYNOS_CLKREG_G2D(0x0800)
#define EXYNOS7420_ENABLE_PCLK_G2D		EXYNOS_CLKREG_G2D(0x0900)
/* mscl */
#define EXYNOS7420_MUX_SEL_MSCL			EXYNOS_CLKREG_MSCL(0x0200)
#define EXYNOS7420_MUX_STAT_MSCL		EXYNOS_CLKREG_MSCL(0x0400)

#define EXYNOS7420_DIV_MSCL			EXYNOS_CLKREG_MSCL(0x0600)
#define EXYNOS7420_DIV_STAT_MSCL		EXYNOS_CLKREG_MSCL(0x0700)

#define EXYNOS7420_ENABLE_ACLK_MSCL		EXYNOS_CLKREG_MSCL(0x0800)
#define EXYNOS7420_ENABLE_PCLK_MSCL		EXYNOS_CLKREG_MSCL(0x0900)
#define EXYNOS7420_ENABLE_SCLK_MSCL		EXYNOS_CLKREG_MSCL(0x0A00)

#define EXYNOS7420_ENABLE_ACLK_MSCL_SMMU0	EXYNOS_CLKREG_MSCL(0x0804)
#define EXYNOS7420_ENABLE_ACLK_MSCL_SFW0	EXYNOS_CLKREG_MSCL(0x0808)
#define EXYNOS7420_ENABLE_ACLK_MSCL_SMMU1	EXYNOS_CLKREG_MSCL(0x080C)
#define EXYNOS7420_ENABLE_ACLK_MSCL_SFW1	EXYNOS_CLKREG_MSCL(0x0810)
#define EXYNOS7420_ENABLE_ACLK_G2D_SMMU		EXYNOS_CLKREG_MSCL(0x0814)
#define EXYNOS7420_ENABLE_ACLK_G2D_SFW		EXYNOS_CLKREG_MSCL(0x0818)

#define EXYNOS7420_ENABLE_PCLK_MSCL_SMMU0	EXYNOS_CLKREG_MSCL(0x0904)
#define EXYNOS7420_ENABLE_PCLK_MSCL_SFW0	EXYNOS_CLKREG_MSCL(0x0908)
#define EXYNOS7420_ENABLE_PCLK_MSCL_SMMU1	EXYNOS_CLKREG_MSCL(0x090C)
#define EXYNOS7420_ENABLE_PCLK_MSCL_SFW1	EXYNOS_CLKREG_MSCL(0x0910)
#define EXYNOS7420_ENABLE_PCLK_G2D_SMMU		EXYNOS_CLKREG_MSCL(0x0914)
#define EXYNOS7420_ENABLE_PCLK_G2D_SFW		EXYNOS_CLKREG_MSCL(0x0918)
/* mfc */
#define EXYNOS7420_MUX_SEL_MFC			EXYNOS_CLKREG_MFC(0x0200)
#define EXYNOS7420_MUX_ENABLE_MFC		EXYNOS_CLKREG_MFC(0x0300)
#define EXYNOS7420_MUX_STAT_MFC			EXYNOS_CLKREG_MFC(0x0400)

#define EXYNOS7420_DIV_MFC			EXYNOS_CLKREG_MFC(0x0600)
#define EXYNOS7420_DIV_STAT_MFC			EXYNOS_CLKREG_MFC(0x0700)

#define EXYNOS7420_ENABLE_ACLK_MFC		EXYNOS_CLKREG_MFC(0x0800)
#define EXYNOS7420_ENABLE_PCLK_MFC		EXYNOS_CLKREG_MFC(0x0900)

#define EXYNOS7420_ENABLE_ACLK_MFC_0_SMMU	EXYNOS_CLKREG_MFC(0x0804)
#define EXYNOS7420_ENABLE_ACLK_MFC_0_SFW	EXYNOS_CLKREG_MFC(0x0808)
#define EXYNOS7420_ENABLE_ACLK_MFC_1_SMMU	EXYNOS_CLKREG_MFC(0x080C)
#define EXYNOS7420_ENABLE_ACLK_MFC_1_SFW	EXYNOS_CLKREG_MFC(0x0810)
#define EXYNOS7420_ENABLE_PCLK_MFC_0_SMMU	EXYNOS_CLKREG_MFC(0x0904)
#define EXYNOS7420_ENABLE_PCLK_MFC_0_SFW	EXYNOS_CLKREG_MFC(0x0908)
#define EXYNOS7420_ENABLE_PCLK_MFC_1_SMMU	EXYNOS_CLKREG_MFC(0x090C)
#define EXYNOS7420_ENABLE_PCLK_MFC_1_SFW	EXYNOS_CLKREG_MFC(0x0910)

/* hevc */
#define EXYNOS7420_MUX_SEL_HEVC			EXYNOS_CLKREG_HEVC(0x0200)
#define EXYNOS7420_MUX_STAT_HEVC		EXYNOS_CLKREG_HEVC(0x0400)
#define EXYNOS7420_DIV_HEVC			EXYNOS_CLKREG_HEVC(0x0600)
#define EXYNOS7420_DIV_STAT_HEVC		EXYNOS_CLKREG_HEVC(0x0700)

#define EXYNOS7420_ENABLE_ACLK_HEVC		EXYNOS_CLKREG_HEVC(0x0800)
#define EXYNOS7420_ENABLE_PCLK_HEVC		EXYNOS_CLKREG_HEVC(0x0900)
#define EXYNOS7420_CLK_ENABLE_IP_HEVC		EXYNOS_CLKREG_HEVC(0x0B00)

/* mif */
#define EXYNOS7420_MUX_ENABLE_MIF0		EXYNOS_CLKREG_MIF0(0x0300)
#define EXYNOS7420_MUX_ENABLE_MIF1		EXYNOS_CLKREG_MIF1(0x0300)
#define EXYNOS7420_MUX_ENABLE_MIF2		EXYNOS_CLKREG_MIF2(0x0300)
#define EXYNOS7420_MUX_ENABLE_MIF3		EXYNOS_CLKREG_MIF3(0x0300)

#define EXYNOS7420_MUX_SEL_MIF0			EXYNOS_CLKREG_MIF0(0x0200)
#define EXYNOS7420_MUX_SEL_MIF1			EXYNOS_CLKREG_MIF1(0x0200)
#define EXYNOS7420_MUX_SEL_MIF2			EXYNOS_CLKREG_MIF2(0x0200)
#define EXYNOS7420_MUX_SEL_MIF3			EXYNOS_CLKREG_MIF3(0x0200)

#define EXYNOS7420_MUX_STAT_MIF0		EXYNOS_CLKREG_MIF0(0x0400)
#define EXYNOS7420_MUX_STAT_MIF1		EXYNOS_CLKREG_MIF1(0x0400)
#define EXYNOS7420_MUX_STAT_MIF2		EXYNOS_CLKREG_MIF2(0x0400)
#define EXYNOS7420_MUX_STAT_MIF3		EXYNOS_CLKREG_MIF3(0x0400)

#define EXYNOS7420_DIV_MIF0			EXYNOS_CLKREG_MIF0(0x0600)
#define EXYNOS7420_DIV_MIF1			EXYNOS_CLKREG_MIF1(0x0600)
#define EXYNOS7420_DIV_MIF2			EXYNOS_CLKREG_MIF2(0x0600)
#define EXYNOS7420_DIV_MIF3			EXYNOS_CLKREG_MIF3(0x0600)

#define EXYNOS7420_DIV_STAT_MIF0		EXYNOS_CLKREG_MIF0(0x0700)
#define EXYNOS7420_DIV_STAT_MIF1		EXYNOS_CLKREG_MIF1(0x0700)
#define EXYNOS7420_DIV_STAT_MIF2		EXYNOS_CLKREG_MIF2(0x0700)
#define EXYNOS7420_DIV_STAT_MIF3		EXYNOS_CLKREG_MIF3(0x0700)

#define EXYNOS7420_MUX_SEL_TOPC_LP4_PHY_DIV	EXYNOS_CLKREG_TOPC(0x0D04)

/* Atlas/Apollo Bit Mask & Shit macros */
/* core_pll_con0 */
#define EXYNOS7420_PLL_SDIV_SHIFT			(0)
#define EXYNOS7420_PLL_SDIV_MASK			(0x7 << EXYNOS7420_PLL_SDIV_SHIFT)
#define EXYNOS7420_PLL_PDIV_SHIFT			(8)
#define EXYNOS7420_PLL_PDIV_MASK			(0x3F << EXYNOS7420_PLL_PDIV_SHIFT)
#define EXYNOS7420_PLL_MDIV_SHIFT			(16)
#define EXYNOS7420_PLL_MDIV_MASK			(0x3FF << EXYNOS7420_PLL_MDIV_SHIFT)
#define EXYNOS7420_CORE_PLL_CON0_LOCKED_SHIFT		(29)
/* core_pll_con1 */
#define EXYNOS7420_PLL_BYPASS_SHIFT			(22)
#define EXYNOS7420_PLL_BYPASS_MASK			(0x1 << EXYNOS7420_PLL_BYPASS_SHIFT)
/* mux_stat_core2 */
#define EXYNOS7420_MUX_STAT_CORE2_SHIFT		(0)
#define EXYNOS7420_MUX_STAT_CORE2_MASK		(0x7 << EXYNOS7420_MUX_STAT_CORE2_SHIFT)

/* L2 Q-Channel clock down control bit */
#define USE_L2QACTIVE					(1 << 14)

#endif