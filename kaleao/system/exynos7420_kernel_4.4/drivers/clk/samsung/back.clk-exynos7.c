/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 * Author: Naveen Krishna Ch <naveenkrishna.ch@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
*/

#include <linux/clk-provider.h>
#include <linux/of.h>
#include <linux/err.h>
#include <linux/clk.h>

#include "clk.h"
#include <dt-bindings/clock/exynos7-clk.h>


/***********************\
* CMU_TOPC (0x10570000) *
\***********************/

/* Register Offset definitions */
#define CC_PLL_LOCK		0x0000
#define BUS0_PLL_LOCK		0x0004
#define BUS1_PLL_LOCK		0x0008
#define MFC_PLL_LOCK		0x000C
#define AUD_PLL_LOCK		0x0010
#define CC_PLL_CON0		0x0100
#define BUS0_PLL_CON0		0x0110
#define BUS1_PLL_CON0		0x0120
#define MFC_PLL_CON0		0x0130
#define AUD_PLL_CON0		0x0140
#define MUX_SEL_TOPC0		0x0200
#define MUX_SEL_TOPC1		0x0204
#define MUX_SEL_TOPC2		0x0208
#define MUX_SEL_TOPC3		0x020C
#define DIV_TOPC0		0x0600
#define DIV_TOPC1		0x0604
#define DIV_TOPC3		0x060C
#define ENABLE_ACLK_TOPC0	0x0800
#define ENABLE_ACLK_TOPC1	0x0804
#define ENABLE_SCLK_TOPC1	0x0A04

static const unsigned long topc_clk_regs[] __initconst = {
	CC_PLL_LOCK,
	BUS0_PLL_LOCK,
	BUS1_PLL_LOCK,
	MFC_PLL_LOCK,
	AUD_PLL_LOCK,
	CC_PLL_CON0,
	BUS0_PLL_CON0,
	BUS1_PLL_CON0,
	MFC_PLL_CON0,
	AUD_PLL_CON0,
	MUX_SEL_TOPC0,
	MUX_SEL_TOPC1,
	MUX_SEL_TOPC2,
	MUX_SEL_TOPC3,
	DIV_TOPC0,
	DIV_TOPC1,
	DIV_TOPC3,
	ENABLE_ACLK_TOPC0,
	ENABLE_ACLK_TOPC1,
	ENABLE_SCLK_TOPC1,
};

/* List of parent clocks for Muxes */
PNAME(mout_topc_aud_pll_ctrl_p) = {"fin_pll", "fout_aud_pll"};
PNAME(mout_topc_bus0_pll_ctrl_p) = {"fin_pll", "fout_bus0_pll"};
PNAME(mout_topc_bus1_pll_ctrl_p) = {"fin_pll", "fout_bus1_pll"};
PNAME(mout_topc_cci_pll_ctrl_p) = {"fin_pll", "fout_cci_pll"};
PNAME(mout_topc_mfc_pll_ctrl_p) = {"fin_pll", "fout_mfc_pll"};

PNAME(mout_topc_group1) = {"mout_topc_bus0_pll", "ffac_topc_bus0_pll_div2",
			   "mout_topc_bus1_pll", "mout_topc_cci_pll"};

PNAME(mout_topc_group2) = {"mout_topc_bus0_pll_half", "mout_topc_bus1_pll",
			   "mout_topc_cci_pll", "mout_topc_mfc_pll",
			   "ffac_topc_bus0_pll_half_div2",
			   "ffac_topc_bus1_pll_div2",
			   "ffac_topc_cci_pll_div2",
			   "ffac_topc_mfc_pll_div2",
			   "ffac_topc_bus0_pll_half_div4",
			   "ffac_topc_bus1_pll_div4",
			   "ffac_topc_cci_pll_div4",
			   "ffac_topc_mfc_pll_div4"};

PNAME(mout_topc_group3) = {"mout_topc_bus0_pll_half",
			   "mout_topc_bus1_pll_half",
			   "mout_topc_cci_pll_half",
			   "mout_topc_mfc_pll_half"};

PNAME(mout_topc_bus0_pll_half_p) = {"mout_topc_bus0_pll",
	    			    "ffac_topc_bus0_pll_div2",
				    "ffac_topc_bus0_pll_div4"};

PNAME(mout_topc_bus1_pll_half_p) = {"mout_topc_bus1_pll",
				    "ffac_topc_bus1_pll_div2"};

PNAME(mout_topc_cci_pll_half_p) = {"mout_topc_cci_pll",
				   "ffac_topc_cci_pll_div2"};

PNAME(mout_topc_mfc_pll_half_p) = {"mout_topc_mfc_pll",
				   "ffac_topc_mfc_pll_div2"};

PNAME(mout_sclk_bus0_pll_out_p) = {"mout_topc_bus0_pll",
				   "ffac_topc_bus0_pll_div2"};

/* PLL Settings */
static const struct samsung_pll_rate_table bus0_pll_table[] __initconst = {
	PLL_35XX_RATE(1600000000, 200, 3, 0),
	PLL_35XX_RATE(800000000, 200, 3, 1),
	PLL_35XX_RATE(400000000, 200, 3, 2),
	{},
};

static const struct samsung_pll_rate_table cci_pll_table[] __initconst = {
	PLL_35XX_RATE(532000000, 133, 6, 0),
	{},
};

static const struct samsung_pll_rate_table bus1_pll_table[] __initconst = {
	PLL_35XX_RATE(668000000, 167, 6, 0),
	{},
};

static const struct samsung_pll_rate_table mfc_pll_table[] __initconst = {
	PLL_35XX_RATE(468000000, 234, 6, 1),
	{},
};

static const struct samsung_pll_rate_table aud_pll_table[] __initconst = {
	PLL_36XX_RATE(491520000, 20, 1, 0, 31457),
	PLL_36XX_RATE(196608000, 33, 1, 2, -15204),
	{},
};

static const struct samsung_pll_clock topc_pll_clks[] __initconst = {
	PLL(pll_1451x, 0, "fout_bus0_pll", "fin_pll", BUS0_PLL_LOCK,
	    BUS0_PLL_CON0, bus0_pll_table),
	PLL(pll_1452x, 0, "fout_cci_pll", "fin_pll", CC_PLL_LOCK,
	    CC_PLL_CON0, cci_pll_table),
	PLL(pll_1452x, 0, "fout_bus1_pll", "fin_pll", BUS1_PLL_LOCK,
	    BUS1_PLL_CON0, bus1_pll_table),
	PLL(pll_1452x, 0, "fout_mfc_pll", "fin_pll", MFC_PLL_LOCK,
	    MFC_PLL_CON0, mfc_pll_table),
	/* Also used by CMU_AUD */
	PLL(pll_1460x, FOUT_AUD_PLL, "fout_aud_pll", "fin_pll", AUD_PLL_LOCK,
	    AUD_PLL_CON0, aud_pll_table),
};

/* Fixed factor multipliers/dividers */
static const struct samsung_fixed_factor_clock topc_fixed_factor_clks[] __initconst = {
	FFACTOR(0, "ffac_topc_bus0_pll_div2", "mout_topc_bus0_pll", 1, 2, 0),

	FFACTOR(0, "ffac_topc_bus0_pll_div4",
		   "ffac_topc_bus0_pll_div2", 1, 2, 0),

	FFACTOR(0, "ffac_topc_bus0_pll_half_div2",
		   "mout_topc_bus0_pll_half", 1, 2, 0),

	FFACTOR(0, "ffac_topc_bus0_pll_half_div4",
		   "ffac_topc_bus0_pll_half_div2", 1, 2, 0),

	FFACTOR(0, "ffac_topc_bus1_pll_div2", "mout_topc_bus1_pll", 1, 2, 0),

	FFACTOR(0, "ffac_topc_bus1_pll_div4",
		   "ffac_topc_bus1_pll_div2", 1, 2, 0),

	FFACTOR(0, "ffac_topc_cci_pll_div2", "mout_topc_cci_pll", 1, 2, 0),
	FFACTOR(0, "ffac_topc_cci_pll_div4", "ffac_topc_cci_pll_div2", 1, 2, 0),

	FFACTOR(0, "ffac_topc_mfc_pll_div2", "mout_topc_mfc_pll", 1, 2, 0),
	FFACTOR(0, "ffac_topc_mfc_pll_div4", "ffac_topc_mfc_pll_div2", 1, 2, 0),
};

/* Muxes */
static const struct samsung_mux_clock topc_mux_clks[] __initconst = {
	MUX(0, "mout_topc_bus0_pll", mout_topc_bus0_pll_ctrl_p,
	    MUX_SEL_TOPC0, 0, 1),
	MUX(0, "mout_topc_bus1_pll", mout_topc_bus1_pll_ctrl_p,
	    MUX_SEL_TOPC0, 4, 1),
	MUX(0, "mout_topc_cci_pll", mout_topc_cci_pll_ctrl_p,
	    MUX_SEL_TOPC0, 8, 1),
	MUX(0, "mout_topc_mfc_pll", mout_topc_mfc_pll_ctrl_p,
	    MUX_SEL_TOPC0, 12, 1),
	MUX(0, "mout_topc_bus0_pll_half", mout_topc_bus0_pll_half_p,
	    MUX_SEL_TOPC0, 16, 2),
	MUX(0, "mout_topc_bus1_pll_half", mout_topc_bus1_pll_half_p,
	    MUX_SEL_TOPC0, 20, 1),
	MUX(0, "mout_topc_cci_pll_half", mout_topc_cci_pll_half_p,
	    MUX_SEL_TOPC0, 24, 1),
	MUX(0, "mout_topc_mfc_pll_half", mout_topc_mfc_pll_half_p,
	    MUX_SEL_TOPC0, 28, 1),

	MUX(0, "mout_topc_aud_pll", mout_topc_aud_pll_ctrl_p,
	    MUX_SEL_TOPC1, 0, 1),
	MUX(0, "mout_sclk_bus0_pll_out", mout_sclk_bus0_pll_out_p,
	    MUX_SEL_TOPC1, 16, 1),
	MUX(0, "mout_sclk_bus_pll_g3d", mout_topc_group1,
	    MUX_SEL_TOPC1, 20, 2),

	MUX(0, "mout_aclk_ccore_532", mout_topc_group2, MUX_SEL_TOPC2, 0, 2),
	MUX(0, "mout_aclk_ccore_133", mout_topc_group3, MUX_SEL_TOPC2, 4, 2),

	MUX(0, "mout_aclk_mfc_532", mout_topc_group3, MUX_SEL_TOPC3, 8, 2),
	MUX(0, "mout_aclk_mscl_532", mout_topc_group3, MUX_SEL_TOPC3, 20, 2),
	MUX(0, "mout_aclk_peris_66", mout_topc_group3, MUX_SEL_TOPC3, 24, 2),
};

/* Dividers */
static const struct samsung_div_clock topc_div_clks[] __initconst = {
	DIV(0, "dout_aclk_ccore_532", "mout_aclk_ccore_532",
	    DIV_TOPC0, 0, 4),
	DIV(0, "dout_aclk_ccore_133", "mout_aclk_ccore_133",
	    DIV_TOPC0, 4, 5),

	DIV(0, "dout_aclk_mfc_532", "mout_aclk_mfc_532",
	    DIV_TOPC1, 8, 4),
	DIV(0, "dout_aclk_mscl_532", "mout_aclk_mscl_532",
	    DIV_TOPC1, 20, 4),
	DIV(0, "dout_aclk_peris_66", "mout_aclk_peris_66",
	    DIV_TOPC1, 24, 4),

	DIV(0, "dout_sclk_bus0_pll", "mout_sclk_bus0_pll_out",
	    DIV_TOPC3, 0, 4),
	DIV(0, "dout_sclk_bus1_pll", "mout_topc_bus1_pll",
	    DIV_TOPC3, 8, 4),
	DIV(0, "dout_sclk_cci_pll", "mout_topc_cci_pll",
	    DIV_TOPC3, 12, 4),
	DIV(0, "dout_sclk_mfc_pll", "mout_topc_mfc_pll",
	    DIV_TOPC3, 16, 4),
	DIV(0, "dout_sclk_bus_pll_g3d", "mout_sclk_bus_pll_g3d",
	    DIV_TOPC3, 20, 4),
	DIV(0, "dout_sclk_aud_pll", "mout_topc_aud_pll",
	    DIV_TOPC3, 28, 4),
};

/* Clock gates */
static const struct samsung_gate_clock topc_gate_clks[] __initconst = {
	GATE(ACLK_CCORE_532, "aclk_ccore_532", "dout_aclk_ccore_532",
	     ENABLE_ACLK_TOPC0, 0, CLK_IGNORE_UNUSED, 0),
	GATE(ACLK_CCORE_133, "aclk_ccore_133", "dout_aclk_ccore_133",
	     ENABLE_ACLK_TOPC0, 4, CLK_IGNORE_UNUSED, 0),

	GATE(ACLK_MFC_532, "aclk_mfc_532", "dout_aclk_mfc_532",
	     ENABLE_ACLK_TOPC1, 8, 0, 0),
	GATE(ACLK_MSCL_532, "aclk_mscl_532", "dout_aclk_mscl_532",
	     ENABLE_ACLK_TOPC1, 20, 0, 0),
	GATE(ACLK_PERIS_66, "aclk_peris_66", "dout_aclk_peris_66",
	     ENABLE_ACLK_TOPC1, 24, 0, 0),

	GATE(SCLK_CCI_PLL_A, "sclk_cci_pll_a", "dout_sclk_cci_pll",
	     ENABLE_SCLK_TOPC1, 0, 0, 0),
	GATE(SCLK_CCI_PLL_B, "sclk_cci_pll_b", "dout_sclk_cci_pll",
	     ENABLE_SCLK_TOPC1, 1, 0, 0),
	GATE(SCLK_BUS0_PLL_A, "sclk_bus0_pll_a", "dout_sclk_bus0_pll",
	     ENABLE_SCLK_TOPC1, 4, 0, 0),
	GATE(SCLK_BUS0_PLL_B, "sclk_bus0_pll_b", "dout_sclk_bus0_pll",
	     ENABLE_SCLK_TOPC1, 5, 0, 0),
	GATE(SCLK_BUS_PLL_G3D, "sclk_bus_pll_g3d", "dout_sclk_bus_pll_g3d",
	     ENABLE_SCLK_TOPC1, 9, 0, 0),
	GATE(SCLK_BUS1_PLL_A, "sclk_bus1_pll_a", "dout_sclk_bus1_pll",
	     ENABLE_SCLK_TOPC1, 12, 0, 0),
	GATE(SCLK_BUS1_PLL_B, "sclk_bus1_pll_b", "dout_sclk_bus1_pll",
	     ENABLE_SCLK_TOPC1, 13, 0, 0),
	GATE(SCLK_MFC_PLL_A, "sclk_mfc_pll_a", "dout_sclk_mfc_pll",
	     ENABLE_SCLK_TOPC1, 16, 0, 0),
	GATE(SCLK_MFC_PLL_B, "sclk_mfc_pll_b", "dout_sclk_mfc_pll",
	     ENABLE_SCLK_TOPC1, 17, 0, 0),
	GATE(SCLK_AUD_PLL, "sclk_aud_pll", "dout_sclk_aud_pll",
	     ENABLE_SCLK_TOPC1, 20, 0, 0),
};

static const struct samsung_cmu_info topc_cmu_info __initconst = {
	.pll_clks = topc_pll_clks,
	.nr_pll_clks = ARRAY_SIZE(topc_pll_clks),
	.mux_clks = topc_mux_clks,
	.nr_mux_clks = ARRAY_SIZE(topc_mux_clks),
	.div_clks = topc_div_clks,
	.nr_div_clks = ARRAY_SIZE(topc_div_clks),
	.gate_clks = topc_gate_clks,
	.nr_gate_clks = ARRAY_SIZE(topc_gate_clks),
	.fixed_factor_clks = topc_fixed_factor_clks,
	.nr_fixed_factor_clks = ARRAY_SIZE(topc_fixed_factor_clks),
	.nr_clk_ids = TOPC_NR_CLK,
	.clk_regs = topc_clk_regs,
	.nr_clk_regs = ARRAY_SIZE(topc_clk_regs),
};

static void __init exynos7_clk_topc_init(struct device_node *np)
{
	samsung_cmu_register_one(np, &topc_cmu_info);
}

CLK_OF_DECLARE(exynos7_clk_topc, "samsung,exynos7-clock-topc",
	       exynos7_clk_topc_init);


/***********************\
* CMU_TOP0 (0x105D0000) *
\***********************/

/* Register Offset definitions */
#define	ISP_PLL_LOCK			0x0004
#define	ISP_PLL_CON0			0x0110
#define	CAM_PLL_LOCK			0x0008
#define	CAM_PLL_CON0			0x0120
#define MUX_SEL_TOP00			0x0200
#define MUX_SEL_TOP01			0x0204
#define MUX_SEL_TOP03			0x020C
#define MUX_SEL_TOP04			0x0210
#define MUX_SEL_TOP05			0x0214
#define MUX_SEL_TOP06			0x0218
#define MUX_SEL_TOP07			0x021C
#define MUX_SEL_TOP0_DISP		0x0220
#define MUX_SEL_TOP0_CAM10		0x0224
#define MUX_SEL_TOP0_PERIC0		0x0230
#define MUX_SEL_TOP0_PERIC1		0x0234
#define MUX_SEL_TOP0_PERIC2		0x0238
#define MUX_SEL_TOP0_PERIC3		0x023C
#define MUX_SEL_TOP0_PERIC4		0x0240
#define DIV_TOP03			0x060C
#define DIV_TOP04			0x0610
#define DIV_TOP05			0x0614
#define DIV_TOP06			0x0618
#define DIV_TOP07			0x061C
#define DIV_TOP0_DISP			0x0620
#define DIV_TOP0_CAM10			0x0624
#define DIV_TOP0_PERIC0			0x0630
#define DIV_TOP0_PERIC1			0x0634
#define DIV_TOP0_PERIC2			0x0638
#define DIV_TOP0_PERIC3			0x063C
#define DIV_TOP0_PERIC4			0x0640
#define ENABLE_ACLK_TOP03		0x080C
#define ENABLE_ACLK_TOP04		0x0810
#define ENABLE_ACLK_TOP05		0x0814
#define ENABLE_ACLK_TOP06		0x0818
#define ENABLE_ACLK_TOP07		0x081C
#define	ENABLE_SCLK_TOP0_DISP		0x0A20
#define	ENABLE_SCLK_TOP0_CAM10		0x0A24
#define ENABLE_SCLK_TOP0_PERIC0		0x0A30
#define ENABLE_SCLK_TOP0_PERIC1		0x0A34
#define ENABLE_SCLK_TOP0_PERIC2		0x0A38
#define ENABLE_SCLK_TOP0_PERIC3		0x0A3C
#define ENABLE_SCLK_TOP0_PERIC4		0x0A40

static const unsigned long top0_clk_regs[] __initconst = {
	ISP_PLL_LOCK,
	ISP_PLL_CON0,
	CAM_PLL_LOCK,
	CAM_PLL_CON0,
	MUX_SEL_TOP00,
	MUX_SEL_TOP01,
	MUX_SEL_TOP03,
	MUX_SEL_TOP04,
	MUX_SEL_TOP05,
	MUX_SEL_TOP06,
	MUX_SEL_TOP07,
	MUX_SEL_TOP0_DISP,
	MUX_SEL_TOP0_CAM10,
	MUX_SEL_TOP0_PERIC0,
	MUX_SEL_TOP0_PERIC1,
	MUX_SEL_TOP0_PERIC2,
	MUX_SEL_TOP0_PERIC3,
	MUX_SEL_TOP0_PERIC4,
	DIV_TOP03,
	DIV_TOP04,
	DIV_TOP05,
	DIV_TOP06,
	DIV_TOP07,
	DIV_TOP0_DISP,
	DIV_TOP0_CAM10,
	DIV_TOP0_PERIC0,
	DIV_TOP0_PERIC1,
	DIV_TOP0_PERIC2,
	DIV_TOP0_PERIC3,
	DIV_TOP0_PERIC4,
	ENABLE_ACLK_TOP03,
	ENABLE_ACLK_TOP04,
	ENABLE_ACLK_TOP05,
	ENABLE_ACLK_TOP06,
	ENABLE_ACLK_TOP07,
	ENABLE_SCLK_TOP0_DISP,
	ENABLE_SCLK_TOP0_CAM10,
	ENABLE_SCLK_TOP0_PERIC0,
	ENABLE_SCLK_TOP0_PERIC1,
	ENABLE_SCLK_TOP0_PERIC2,
	ENABLE_SCLK_TOP0_PERIC3,
	ENABLE_SCLK_TOP0_PERIC4,
};

/* List of parent clocks for Muxes */
PNAME(mout_top0_bus0_pll_user_p) = {"fin_pll", "sclk_bus0_pll_a"};
PNAME(mout_top0_bus1_pll_user_p) = {"fin_pll", "sclk_bus1_pll_a"};
PNAME(mout_top0_cci_pll_user_p) = {"fin_pll", "sclk_cci_pll_a"};
PNAME(mout_top0_mfc_pll_user_p) = {"fin_pll", "sclk_mfc_pll_a"};
PNAME(mout_top0_aud_pll_user_p) = {"fin_pll", "sclk_aud_pll"};
PNAME(mout_top0_isp_pll_ctrl_p) = {"fin_pll", "fout_isp_pll"};
PNAME(mout_top0_cam_pll_ctrl_p) = {"fin_pll", "fout_cam_pll"};

PNAME(mout_top0_bus0_pll_half_p) = {"mout_top0_bus0_pll_user",
				    "ffac_top0_bus0_pll_div2"};

PNAME(mout_top0_bus1_pll_half_p) = {"mout_top0_bus1_pll_user",
				    "ffac_top0_bus1_pll_div2"};

PNAME(mout_top0_cci_pll_half_p) = {"mout_top0_cci_pll_user",
				   "ffac_top0_cci_pll_div2"};

PNAME(mout_top0_mfc_pll_half_p) = {"mout_top0_mfc_pll_user",
				   "ffac_top0_mfc_pll_div2"};

PNAME(mout_top0_group0) = {"mout_top0_bus0_pll_half",
			   "mout_top0_bus1_pll_half",
			   "mout_top0_cci_pll_half",
			   "mout_top0_mfc_pll_half",
			   "ffac_top0_bus0_pll_div2",
			   "ffac_top0_bus1_pll_div2",
			   "ffac_top0_cci_pll_div2",
			   "ffac_top0_mfc_pll_div2"};

PNAME(mout_top0_group1) = {"mout_top0_bus0_pll_half",
			   "mout_top0_bus1_pll_half",
			   "mout_top0_cci_pll_half",
			   "mout_top0_mfc_pll_half"};

PNAME(mout_top0_group2) = {"mout_top0_bus0_pll_half",
			   "mout_top0_bus1_pll_half",
			   "mout_top0_cci_pll_half",
			   "mout_top0_mfc_pll_half",
			   "mout_top0_aud_pll_user",
			   "mout_top0_isp_pll_ctrl",
			   "mout_top0_cam_pll_ctrl"};

PNAME(mout_top0_group3) = {"ioclk_audiocdclk0",
			   "ioclk_audiocdclk1",
			   "ioclk_spdif_extclk",
			   "mout_top0_aud_pll_user",
			   "mout_top0_bus0_pll_half",
			   "mout_top0_bus1_pll_half"};

PNAME(mout_top0_group4) = {"ioclk_audiocdclk1",
			   "mout_top0_aud_pll_user",
			   "mout_top0_bus0_pll_half",
			   "mout_top0_bus1_pll_half"};

/* PLLs */
static const struct samsung_pll_rate_table isp_pll_table[] __initconst = {
	PLL_35XX_RATE(590000000, 295, 3, 2),
	PLL_35XX_RATE(588000000, 98, 2, 1),
	PLL_35XX_RATE(558000000, 93, 2, 1),
	PLL_35XX_RATE(552000000, 184, 2, 2),
	PLL_35XX_RATE(432000000, 144, 2, 2),
	{},
};

static const struct samsung_pll_rate_table cam_pll_table[] __initconst = {
	PLL_35XX_RATE(690000000, 230, 2, 2),
	PLL_35XX_RATE(660000000, 165, 3, 1),
	PLL_35XX_RATE(532000000, 266, 3, 2),
	{},
};

static const struct samsung_pll_clock top0_pll_clks[] __initconst = {
	PLL(pll_1450x, 0, "fout_isp_pll", "fin_pll", ISP_PLL_LOCK,
	    ISP_PLL_CON0, isp_pll_table),
	PLL(pll_1450x, 0, "fout_cam_pll", "fin_pll", CAM_PLL_LOCK,
	    CAM_PLL_CON0, cam_pll_table),
};

/* Fixed factor multipliers/dividers */
static const struct samsung_fixed_factor_clock top0_fixed_factor_clks[] __initconst = {
	FFACTOR(0, "ffac_top0_bus0_pll_div2",
		   "mout_top0_bus0_pll_user", 1, 2, 0),

	FFACTOR(0, "ffac_top0_bus1_pll_div2",
		   "mout_top0_bus1_pll_user", 1, 2, 0),

	FFACTOR(0, "ffac_top0_cci_pll_div2", "mout_top0_cci_pll_user", 1, 2, 0),
	FFACTOR(0, "ffac_top0_mfc_pll_div2", "mout_top0_mfc_pll_user", 1, 2, 0),
};

/* Muxes */
static const struct samsung_mux_clock top0_mux_clks[] __initconst = {
	MUX(0, "mout_top0_aud_pll_user", mout_top0_aud_pll_user_p,
	    MUX_SEL_TOP00, 0, 1),
	MUX(0, "mout_top0_mfc_pll_user", mout_top0_mfc_pll_user_p,
	    MUX_SEL_TOP00, 4, 1),
	MUX(0, "mout_top0_cci_pll_user", mout_top0_cci_pll_user_p,
	    MUX_SEL_TOP00, 8, 1),
	MUX(0, "mout_top0_bus1_pll_user", mout_top0_bus1_pll_user_p,
	    MUX_SEL_TOP00, 12, 1),
	MUX(0, "mout_top0_bus0_pll_user", mout_top0_bus0_pll_user_p,
	    MUX_SEL_TOP00, 16, 1),
	MUX(0, "mout_top0_cam_pll_ctrl", mout_top0_cam_pll_ctrl_p,
	    MUX_SEL_TOP00, 20, 1),
	MUX(0, "mout_top0_isp_pll_ctrl", mout_top0_isp_pll_ctrl_p,
	    MUX_SEL_TOP00, 24, 1),

	MUX(0, "mout_top0_mfc_pll_half", mout_top0_mfc_pll_half_p,
	    MUX_SEL_TOP01, 4, 1),
	MUX(0, "mout_top0_cci_pll_half", mout_top0_cci_pll_half_p,
	    MUX_SEL_TOP01, 8, 1),
	MUX(0, "mout_top0_bus1_pll_half", mout_top0_bus1_pll_half_p,
	    MUX_SEL_TOP01, 12, 1),
	MUX(0, "mout_top0_bus0_pll_half", mout_top0_bus0_pll_half_p,
	    MUX_SEL_TOP01, 16, 1),

	MUX(0, "mout_aclk_vpp1_400", mout_top0_group1, MUX_SEL_TOP03, 4, 2),
	MUX(0, "mout_aclk_vpp0_400", mout_top0_group1, MUX_SEL_TOP03, 8, 2),
	MUX(0, "mout_aclk_peric1_66", mout_top0_group1, MUX_SEL_TOP03, 12, 2),
	MUX(0, "mout_aclk_peric0_66", mout_top0_group1, MUX_SEL_TOP03, 20, 2),
	MUX(0, "mout_aclk_disp_400", mout_top0_group0, MUX_SEL_TOP03, 28, 3),

	MUX(0, "mout_aclk_isp1_ahb_117", mout_top0_group2,
	    MUX_SEL_TOP04, 12, 3),
	MUX(0, "mout_aclk_isp1_isp1_468", mout_top0_group2,
	    MUX_SEL_TOP04, 16, 3),
	MUX(0, "mout_aclk_isp0_trex_532", mout_top0_group2,
	    MUX_SEL_TOP04, 20, 3),
	MUX(0, "mout_aclk_isp0_tpu_590", mout_top0_group2,
	    MUX_SEL_TOP04, 24, 3),
	MUX(0, "mout_aclk_isp0_isp0_590", mout_top0_group2,
	    MUX_SEL_TOP04, 28, 3),

	MUX(0, "mout_aclk_cam0_csis3_133", mout_top0_group2,
	    MUX_SEL_TOP05, 8, 3),
	MUX(0, "mout_aclk_cam0_csis1_174", mout_top0_group2,
	    MUX_SEL_TOP05, 12, 3),
	MUX(0, "mout_aclk_cam0_bnsd_690", mout_top0_group2,
	    MUX_SEL_TOP05, 16, 3),
	MUX(0, "mout_aclk_cam0_bnsb_690", mout_top0_group2,
	    MUX_SEL_TOP05, 20, 3),
	MUX(0, "mout_aclk_cam0_bnsa_690", mout_top0_group2,
	    MUX_SEL_TOP05, 24, 3),
	MUX(0, "mout_aclk_cam0_csis0_690", mout_top0_group2,
	    MUX_SEL_TOP05, 28, 3),

	MUX(0, "mout_aclk_cam0_nocp_133", mout_top0_group2,
	    MUX_SEL_TOP06, 16, 3),
	MUX(0, "mout_aclk_cam0_trex_532", mout_top0_group2,
	    MUX_SEL_TOP06, 20, 3),
	MUX(0, "mout_aclk_cam0_3aa1_468", mout_top0_group2,
	    MUX_SEL_TOP06, 24, 3),
	MUX(0, "mout_aclk_cam0_3aa0_690", mout_top0_group2,
	    MUX_SEL_TOP06, 28, 3),

	MUX(0, "mout_aclk_cam1_trex_532", mout_top0_group2,
	    MUX_SEL_TOP07, 8, 3),
	MUX(0, "mout_aclk_cam1_nocp_133", mout_top0_group2,
	    MUX_SEL_TOP07, 12, 3),
	MUX(0, "mout_aclk_cam1_bnscsis_133", mout_top0_group2,
	    MUX_SEL_TOP07, 16, 3),
	MUX(0, "mout_aclk_cam1_busperi_334", mout_top0_group2,
	    MUX_SEL_TOP07, 20, 3),
	MUX(0, "mout_aclk_cam1_arm_668", mout_top0_group2,
	    MUX_SEL_TOP07, 24, 3),
	MUX(0, "mout_aclk_cam1_sclvra_491", mout_top0_group2,
	    MUX_SEL_TOP07, 28, 3),

	MUX(0, "mout_sclk_hdmi_spdif", mout_top0_group3,
	    MUX_SEL_TOP0_DISP, 0, 3),
	MUX(0, "mout_sclk_dsd", mout_top0_group1,
	    MUX_SEL_TOP0_DISP, 0, 2),
	MUX(0, "mout_sclk_decon_vclk", mout_top0_group1,
	    MUX_SEL_TOP0_DISP, 0, 2),
	MUX(0, "mout_sclk_decon_ext_eclk", mout_top0_group1,
	    MUX_SEL_TOP0_DISP, 0, 2),
	MUX(0, "mout_sclk_decon_int_eclk", mout_top0_group1,
	    MUX_SEL_TOP0_DISP, 0, 2),

	MUX(0, "mout_sclk_isp_uart", mout_top0_group1,
	    MUX_SEL_TOP0_CAM10, 4, 2),
	MUX(0, "mout_sclk_isp_spi1", mout_top0_group1,
	    MUX_SEL_TOP0_CAM10, 8, 2),
	MUX(0, "mout_sclk_isp_spi0", mout_top0_group1,
	    MUX_SEL_TOP0_CAM10, 20, 2),

	MUX(0, "mout_sclk_spdif", mout_top0_group3, MUX_SEL_TOP0_PERIC0, 4, 3),
	MUX(0, "mout_sclk_pcm1", mout_top0_group4, MUX_SEL_TOP0_PERIC0, 8, 2),
	MUX(0, "mout_sclk_i2s1", mout_top0_group4, MUX_SEL_TOP0_PERIC0, 20, 2),

	MUX(0, "mout_sclk_spi1", mout_top0_group1, MUX_SEL_TOP0_PERIC1, 8, 2),
	MUX(0, "mout_sclk_spi0", mout_top0_group1, MUX_SEL_TOP0_PERIC1, 20, 2),

	MUX(0, "mout_sclk_spi3", mout_top0_group1, MUX_SEL_TOP0_PERIC2, 8, 2),
	MUX(0, "mout_sclk_spi2", mout_top0_group1, MUX_SEL_TOP0_PERIC2, 20, 2),

	MUX(0, "mout_sclk_uart3", mout_top0_group1, MUX_SEL_TOP0_PERIC3, 4, 2),
	MUX(0, "mout_sclk_uart2", mout_top0_group1, MUX_SEL_TOP0_PERIC3, 8, 2),
	MUX(0, "mout_sclk_uart1", mout_top0_group1, MUX_SEL_TOP0_PERIC3, 12, 2),
	MUX(0, "mout_sclk_uart0", mout_top0_group1, MUX_SEL_TOP0_PERIC3, 16, 2),
	MUX(0, "mout_sclk_spi4", mout_top0_group1, MUX_SEL_TOP0_PERIC3, 20, 2),

	MUX(0, "mout_sclk_spi5", mout_top0_group1, MUX_SEL_TOP0_PERIC4, 20, 2),
};

/* Dividers */
static const struct samsung_div_clock top0_div_clks[] __initconst = {
	DIV(0, "dout_aclk_vpp1_400", "mout_aclk_vpp1_400",
	    DIV_TOP03, 4, 4),
	DIV(0, "dout_aclk_vpp0_400", "mout_aclk_vpp0_400",
	    DIV_TOP03, 8, 4),
	DIV(0, "dout_aclk_peric1_66", "mout_aclk_peric1_66",
	    DIV_TOP03, 12, 6),
	DIV(0, "dout_aclk_peric0_66", "mout_aclk_peric0_66",
	    DIV_TOP03, 20, 6),
	DIV(0, "dout_aclk_disp_400", "mout_aclk_disp_400",
	    DIV_TOP03, 28, 4),

	DIV(0, "dout_aclk_isp1_ahb_117", "mout_aclk_isp1_ahb_117",
	    DIV_TOP04, 12, 4),
	DIV(0, "dout_aclk_isp1_isp1_468", "mout_aclk_isp1_isp1_468",
	    DIV_TOP04, 16, 4),
	DIV(0, "dout_aclk_isp0_trex_532", "mout_aclk_isp0_trex_532",
	    DIV_TOP04, 20, 4),
	DIV(0, "dout_aclk_isp0_tpu_590", "mout_aclk_isp0_tpu_590",
	    DIV_TOP04, 24, 4),
	DIV(0, "dout_aclk_isp0_isp0_590", "mout_aclk_isp0_isp0_590",
	    DIV_TOP04, 28, 4),

	DIV(0, "dout_aclk_cam0_csis3_133", "mout_aclk_cam0_csis3_133",
	    DIV_TOP05, 8, 4),
	DIV(0, "dout_aclk_cam0_csis1_174", "mout_aclk_cam0_csis1_174",
	    DIV_TOP05, 12, 4),
	DIV(0, "dout_aclk_cam0_bnsd_690", "mout_aclk_cam0_bnsd_690",
	    DIV_TOP05, 16, 4),
	DIV(0, "dout_aclk_cam0_bnsb_690", "mout_aclk_cam0_bnsb_690",
	    DIV_TOP05, 20, 4),
	DIV(0, "dout_aclk_cam0_bnsa_690", "mout_aclk_cam0_bnsa_690",
	    DIV_TOP05, 24, 4),
	DIV(0, "dout_aclk_cam0_csis0_690", "mout_aclk_cam0_csis0_690",
	    DIV_TOP05, 28, 4),

	DIV(0, "dout_aclk_cam0_nocp_133", "mout_aclk_cam0_nocp_133",
	    DIV_TOP06, 16, 4),
	DIV(0, "dout_aclk_cam0_trex_532", "mout_aclk_cam0_trex_532",
	    DIV_TOP06, 20, 4),
	DIV(0, "dout_aclk_cam0_3aa1_468", "mout_aclk_cam0_3aa1_468",
	    DIV_TOP06, 24, 4),
	DIV(0, "dout_aclk_cam0_3aa0_690", "mout_aclk_cam0_3aa0_690",
	    DIV_TOP06, 28, 4),

	DIV(0, "dout_aclk_cam1_trex_532", "mout_aclk_cam1_trex_532",
	    DIV_TOP07, 8, 4),
	DIV(0, "dout_aclk_cam1_nocp_133", "mout_aclk_cam1_nocp_133",
	    DIV_TOP07, 12, 4),
	DIV(0, "dout_aclk_cam1_bnscsis_133", "mout_aclk_cam1_bnscsis_133",
	    DIV_TOP07, 16, 4),
	DIV(0, "dout_aclk_cam1_busperi_334", "mout_aclk_cam1_busperi_334",
	    DIV_TOP07, 20, 4),
	DIV(0, "dout_aclk_cam1_arm_668", "mout_aclk_cam1_arm_668",
	    DIV_TOP07, 24, 4),
	DIV(0, "dout_aclk_cam1_sclvra_491", "mout_aclk_cam1_sclvra_491",
	    DIV_TOP07, 28, 4),

	DIV(0, "dout_sclk_hdmi_spdif", "mout_sclk_hdmi_spdif",
	    DIV_TOP0_DISP, 12, 4),
	DIV(0, "dout_sclk_dsd", "mout_sclk_dsd",
	    DIV_TOP0_DISP, 16, 4),
	DIV(0, "dout_sclk_decon_vclk", "mout_sclk_decon_vclk",
	    DIV_TOP0_DISP, 20, 4),
	DIV(0, "dout_sclk_decon_ext_eclk", "mout_sclk_decon_ext_eclk",
	    DIV_TOP0_DISP, 24, 4),
	DIV(0, "dout_sclk_decon_int_eclk", "mout_sclk_decon_int_eclk",
	    DIV_TOP0_DISP, 28, 4),

	DIV(0, "dout_sclk_isp_uart", "mout_sclk_isp_uart",
	    DIV_TOP0_CAM10, 4, 4),
	DIV(0, "dout_sclk_isp_spi1", "mout_sclk_isp_spi1",
	    DIV_TOP0_CAM10, 8, 12),
	DIV(0, "dout_sclk_isp_spi0", "mout_sclk_isp_spi0",
	    DIV_TOP0_CAM10, 20, 12),

	DIV(0, "dout_sclk_spdif", "mout_sclk_spdif", DIV_TOP0_PERIC0, 4, 4),
	DIV(0, "dout_sclk_pcm1", "mout_sclk_pcm1", DIV_TOP0_PERIC0, 8, 12),
	DIV(0, "dout_sclk_i2s1", "mout_sclk_i2s1", DIV_TOP0_PERIC0, 20, 10),

	DIV(0, "dout_sclk_spi1", "mout_sclk_spi1", DIV_TOP0_PERIC1, 8, 12),
	DIV(0, "dout_sclk_spi0", "mout_sclk_spi0", DIV_TOP0_PERIC1, 20, 12),

	DIV(0, "dout_sclk_spi3", "mout_sclk_spi3", DIV_TOP0_PERIC2, 8, 12),
	DIV(0, "dout_sclk_spi2", "mout_sclk_spi2", DIV_TOP0_PERIC2, 20, 12),

	DIV(0, "dout_sclk_uart3", "mout_sclk_uart3", DIV_TOP0_PERIC3, 4, 4),
	DIV(0, "dout_sclk_uart2", "mout_sclk_uart2", DIV_TOP0_PERIC3, 8, 4),
	DIV(0, "dout_sclk_uart1", "mout_sclk_uart1", DIV_TOP0_PERIC3, 12, 4),
	DIV(0, "dout_sclk_uart0", "mout_sclk_uart0", DIV_TOP0_PERIC3, 16, 4),
	DIV(0, "dout_sclk_spi4", "mout_sclk_spi4", DIV_TOP0_PERIC3, 20, 12),

	DIV(0, "dout_sclk_spi5", "mout_sclk_spi5", DIV_TOP0_PERIC4, 20, 12),
};

/* Clock gates */
static const struct samsung_gate_clock top0_gate_clks[] __initconst = {
	GATE(ACLK_VPP1_400, "aclk_vpp1_400", "dout_aclk_vpp1_400",
	     ENABLE_ACLK_TOP03, 4, 0, 0),
	GATE(ACLK_VPP0_400, "aclk_vpp0_400", "dout_aclk_vpp0_400",
	     ENABLE_ACLK_TOP03, 8, 0, 0),
	GATE(ACLK_PERIC1_66, "aclk_peric1_66", "dout_aclk_peric1_66",
	     ENABLE_ACLK_TOP03, 12, CLK_SET_RATE_PARENT, 0),
	GATE(ACLK_PERIC0_66, "aclk_peric0_66", "dout_aclk_peric0_66",
	     ENABLE_ACLK_TOP03, 20, CLK_SET_RATE_PARENT, 0),
	GATE(ACLK_DISP_400, "aclk_disp_400", "dout_aclk_disp_400",
	     ENABLE_ACLK_TOP03, 28, 0, 0),

	GATE(ACLK_ISP1_AHB_117, "aclk_isp1_ahb_117", "dout_aclk_isp1_ahb_117",
	     ENABLE_ACLK_TOP04, 12, 0, 0),
	GATE(ACLK_ISP1_ISP1_468, "aclk_isp1_isp1_468",
	     "dout_aclk_isp1_isp1_468",
	     ENABLE_ACLK_TOP04, 16, 0, 0),
	GATE(ACLK_ISP0_TREX_532, "aclk_isp0_trex_532",
	     "dout_aclk_isp0_trex_532",
	     ENABLE_ACLK_TOP04, 20, 0, 0),
	GATE(ACLK_ISP0_TPU_590, "aclk_isp0_tpu_590", "dout_aclk_isp0_tpu_590",
	     ENABLE_ACLK_TOP04, 24, 0, 0),
	GATE(ACLK_ISP0_ISP0_590, "aclk_isp0_isp0_590",
	     "dout_aclk_isp0_isp0_590",
	     ENABLE_ACLK_TOP04, 28, 0, 0),

	GATE(ACLK_CAM0_CSIS3_133, "aclk_cam0_csis3_133",
	     "dout_aclk_cam0_csis3_133",
	     ENABLE_ACLK_TOP05, 8, 0, 0),
	GATE(ACLK_CAM0_CSIS1_174, "aclk_cam0_csis1_174",
	     "dout_aclk_cam0_csis1_174",
	     ENABLE_ACLK_TOP05, 12, 0, 0),
	GATE(ACLK_CAM0_BNSD_690, "aclk_cam0_bnsd_690",
	     "dout_aclk_cam0_bnsd_690",
	     ENABLE_ACLK_TOP05, 16, 0, 0),
	GATE(ACLK_CAM0_BNSB_690, "aclk_cam0_bnsb_690",
	     "dout_aclk_cam0_bnsb_690",
	     ENABLE_ACLK_TOP05, 20, 0, 0),
	GATE(ACLK_CAM0_BNSA_690, "aclk_cam0_bnsa_690",
	     "dout_aclk_cam0_bnsa_690",
	     ENABLE_ACLK_TOP05, 24, 0, 0),
	GATE(ACLK_CAM0_CSIS0_690, "aclk_cam0_csis0_690",
	     "dout_aclk_cam0_csis0_690",
	     ENABLE_ACLK_TOP05, 28, 0, 0),

	GATE(ACLK_CAM0_NOCP_133, "aclk_cam0_nocp_133",
	     "dout_aclk_cam0_nocp_133",
	     ENABLE_ACLK_TOP06, 16, 0, 0),
	GATE(ACLK_CAM0_TREX_532, "aclk_cam0_trex_532",
	     "dout_aclk_cam0_trex_532",
	     ENABLE_ACLK_TOP06, 20, 0, 0),
	GATE(ACLK_CAM0_3AA1_468, "aclk_cam0_3aa1_468",
	     "dout_aclk_cam0_3aa1_468",
	     ENABLE_ACLK_TOP06, 24, 0, 0),
	GATE(ACLK_CAM0_3AA0_690, "aclk_cam0_3aa0_690",
	     "dout_aclk_cam0_3aa0_690",
	     ENABLE_ACLK_TOP06, 28, 0, 0),

	GATE(ACLK_CAM1_TREX_532, "aclk_cam1_trex_532",
	     "dout_aclk_cam1_trex_532",
	     ENABLE_ACLK_TOP07, 8, 0, 0),
	GATE(ACLK_CAM1_NOCP_133, "aclk_cam1_nocp_133",
	     "dout_aclk_cam1_nocp_133",
	     ENABLE_ACLK_TOP07, 12, 0, 0),
	GATE(ACLK_CAM1_BNSCSIS_133, "aclk_cam1_bnscsis_133",
	     "dout_aclk_cam1_bnscsis_133",
	     ENABLE_ACLK_TOP07, 16, 0, 0),
	GATE(ACLK_CAM1_BUSPERI_334, "aclk_cam1_busperi_334",
	     "dout_aclk_cam1_busperi_334",
	     ENABLE_ACLK_TOP07, 20, 0, 0),
	GATE(ACLK_CAM1_ARM_668, "aclk_cam1_arm_668", "dout_aclk_cam1_arm_668",
	     ENABLE_ACLK_TOP07, 24, 0, 0),
	GATE(ACLK_CAM1_SCLVRA_491, "aclk_cam1_sclvra_491",
	     "dout_aclk_cam1_sclvra_491",
	     ENABLE_ACLK_TOP07, 28, 0, 0),

	GATE(SCLK_HDMI_SPDIF, "sclk_hdmi_spdif", "dout_sclk_hdmi_spdif",
	     ENABLE_SCLK_TOP0_DISP, 12, 0, 0),
	GATE(SCLK_DSD, "sclk_dsd", "dout_sclk_dsd",
	     ENABLE_SCLK_TOP0_DISP, 16, 0, 0),
	GATE(SCLK_DECON_VCLK, "sclk_decon_vclk", "dout_sclk_decon_vclk",
	     ENABLE_SCLK_TOP0_DISP, 20, 0, 0),
	GATE(SCLK_DECON_EXT_ECLK, "sclk_decon_ext_eclk",
	     "dout_sclk_decon_ext_eclk",
	     ENABLE_SCLK_TOP0_DISP, 24, 0, 0),
	GATE(SCLK_DECON_INT_ECLK, "sclk_decon_int_eclk",
	     "dout_sclk_decon_int_eclk",
	     ENABLE_SCLK_TOP0_DISP, 28, 0, 0),

	GATE(SCLK_ISP_MTCADC, "sclk_isp_mtcadc", "fin_pll",
	     ENABLE_SCLK_TOP0_CAM10, 0, 0, 0),
	GATE(SCLK_ISP_UART, "sclk_isp_uart", "dout_sclk_isp_uart",
	     ENABLE_SCLK_TOP0_CAM10, 4, 0, 0),
	GATE(SCLK_ISP_SPI1, "sclk_isp_spi1", "dout_sclk_isp_spi1",
	     ENABLE_SCLK_TOP0_CAM10, 8, 0, 0),
	GATE(SCLK_ISP_SPI0, "sclk_isp_spi0", "dout_sclk_isp_spi0",
	     ENABLE_SCLK_TOP0_CAM10, 20, 0, 0),

	GATE(SCLK_SPDIF, "sclk_spdif", "dout_sclk_spdif",
	     ENABLE_SCLK_TOP0_PERIC0, 4, CLK_SET_RATE_PARENT, 0),
	GATE(SCLK_PCM1, "sclk_pcm1", "dout_sclk_pcm1",
	     ENABLE_SCLK_TOP0_PERIC0, 8, CLK_SET_RATE_PARENT, 0),
	GATE(SCLK_I2S1, "sclk_i2s1", "dout_sclk_i2s1",
	     ENABLE_SCLK_TOP0_PERIC0, 20, CLK_SET_RATE_PARENT, 0),

	GATE(SCLK_SPI1, "sclk_spi1", "dout_sclk_spi1",
	     ENABLE_SCLK_TOP0_PERIC1, 8, CLK_SET_RATE_PARENT, 0),
	GATE(SCLK_SPI0, "sclk_spi0", "dout_sclk_spi0",
	     ENABLE_SCLK_TOP0_PERIC1, 20, CLK_SET_RATE_PARENT, 0),

	GATE(SCLK_SPI3, "sclk_spi3", "dout_sclk_spi3",
	     ENABLE_SCLK_TOP0_PERIC2, 8, CLK_SET_RATE_PARENT, 0),
	GATE(SCLK_SPI2, "sclk_spi2", "dout_sclk_spi2",
	     ENABLE_SCLK_TOP0_PERIC2, 20, CLK_SET_RATE_PARENT, 0),

	GATE(SCLK_UART3, "sclk_uart3", "dout_sclk_uart3",
	     ENABLE_SCLK_TOP0_PERIC3, 4, 0, 0),
	GATE(SCLK_UART2, "sclk_uart2", "dout_sclk_uart2",
	     ENABLE_SCLK_TOP0_PERIC3, 8, 0, 0),
	GATE(SCLK_UART1, "sclk_uart1", "dout_sclk_uart1",
	     ENABLE_SCLK_TOP0_PERIC3, 12, 0, 0),
	GATE(SCLK_UART0, "sclk_uart0", "dout_sclk_uart0",
	     ENABLE_SCLK_TOP0_PERIC3, 16, 0, 0),
	GATE(SCLK_SPI4, "sclk_spi4", "dout_sclk_spi4",
	     ENABLE_SCLK_TOP0_PERIC3, 20, CLK_SET_RATE_PARENT, 0),

	GATE(SCLK_SPI5, "sclk_spi5", "dout_sclk_spi5",
	     ENABLE_SCLK_TOP0_PERIC4, 20, CLK_SET_RATE_PARENT, 0),
};

static const struct samsung_cmu_info top0_cmu_info __initconst = {
	.pll_clks = top0_pll_clks,
	.nr_pll_clks = ARRAY_SIZE(top0_pll_clks),
	.mux_clks = top0_mux_clks,
	.nr_mux_clks = ARRAY_SIZE(top0_mux_clks),
	.div_clks = top0_div_clks,
	.nr_div_clks = ARRAY_SIZE(top0_div_clks),
	.gate_clks = top0_gate_clks,
	.nr_gate_clks = ARRAY_SIZE(top0_gate_clks),
	.fixed_factor_clks = top0_fixed_factor_clks,
	.nr_fixed_factor_clks = ARRAY_SIZE(top0_fixed_factor_clks),
	.nr_clk_ids = TOP0_NR_CLK,
	.clk_regs = top0_clk_regs,
	.nr_clk_regs = ARRAY_SIZE(top0_clk_regs),
};

static void __init exynos7_clk_top0_init(struct device_node *np)
{
	samsung_cmu_register_one(np, &top0_cmu_info);
}

CLK_OF_DECLARE(exynos7_clk_top0, "samsung,exynos7-clock-top0",
	       exynos7_clk_top0_init);


/***********************\
* CMU_TOP1 (0x105E0000) *
\***********************/

/* Register Offset definitions */
#define MUX_SEL_TOP10			0x0200
#define MUX_SEL_TOP11			0x0204
#define MUX_SEL_TOP13			0x020C
#define MUX_SEL_TOP1_FSYS0		0x0224
#define MUX_SEL_TOP1_FSYS1		0x0228
#define MUX_SEL_TOP1_FSYS11		0x022C
#define DIV_TOP13			0x060C
#define DIV_TOP1_FSYS0			0x0624
#define DIV_TOP1_FSYS1			0x0628
#define DIV_TOP1_FSYS11			0x062C
#define ENABLE_ACLK_TOP13		0x080C
#define ENABLE_SCLK_TOP1_FSYS0		0x0A24
#define ENABLE_SCLK_TOP1_FSYS1		0x0A28
#define ENABLE_SCLK_TOP1_FSYS11		0x0A2C

static const unsigned long top1_clk_regs[] __initconst = {
	MUX_SEL_TOP10,
	MUX_SEL_TOP11,
	MUX_SEL_TOP13,
	MUX_SEL_TOP1_FSYS0,
	MUX_SEL_TOP1_FSYS1,
	MUX_SEL_TOP1_FSYS11,
	DIV_TOP13,
	DIV_TOP1_FSYS0,
	DIV_TOP1_FSYS1,
	DIV_TOP1_FSYS11,
	ENABLE_ACLK_TOP13,
	ENABLE_SCLK_TOP1_FSYS0,
	ENABLE_SCLK_TOP1_FSYS1,
	ENABLE_SCLK_TOP1_FSYS11,
};

/* List of parent clocks for Muxes */
PNAME(mout_top1_bus0_pll_user_p) = {"fin_pll", "sclk_bus0_pll_b"};
PNAME(mout_top1_bus1_pll_user_p) = {"fin_pll", "sclk_bus1_pll_b"};
PNAME(mout_top1_cci_pll_user_p)  = {"fin_pll", "sclk_cci_pll_b"};
PNAME(mout_top1_mfc_pll_user_p)  = {"fin_pll", "sclk_mfc_pll_b"};

PNAME(mout_top1_bus0_pll_half_p) = {"mout_top1_bus0_pll_user",
				    "ffac_top1_bus0_pll_div2"};

PNAME(mout_top1_bus1_pll_half_p) = {"mout_top1_bus1_pll_user",
				    "ffac_top1_bus1_pll_div2"};

PNAME(mout_top1_cci_pll_half_p) = {"mout_top1_cci_pll_user",
				   "ffac_top1_cci_pll_div2"};

PNAME(mout_top1_mfc_pll_half_p) = {"mout_top1_mfc_pll_user",
				   "ffac_top1_mfc_pll_div2"};

PNAME(mout_top1_group1) = {"mout_top1_bus0_pll_half",
			   "mout_top1_bus1_pll_half",
			   "mout_top1_cci_pll_half",
			   "mout_top1_mfc_pll_half"};

/* Fixed factor multipliers/dividers */
static const struct samsung_fixed_factor_clock top1_fixed_factor_clks[] __initconst = {
	FFACTOR(0, "ffac_top1_bus0_pll_div2", "mout_top1_bus0_pll_user",
		1, 2, 0),
	FFACTOR(0, "ffac_top1_bus1_pll_div2", "mout_top1_bus1_pll_user",
		1, 2, 0),
	FFACTOR(0, "ffac_top1_cci_pll_div2", "mout_top1_cci_pll_user", 1, 2, 0),
	FFACTOR(0, "ffac_top1_mfc_pll_div2", "mout_top1_mfc_pll_user", 1, 2, 0),
};

/* Muxes */
static const struct samsung_mux_clock top1_mux_clks[] __initconst = {
	MUX(0, "mout_top1_mfc_pll_user", mout_top1_mfc_pll_user_p,
	    MUX_SEL_TOP10, 4, 1),
	MUX(0, "mout_top1_cci_pll_user", mout_top1_cci_pll_user_p,
	    MUX_SEL_TOP10, 8, 1),
	MUX(0, "mout_top1_bus1_pll_user", mout_top1_bus1_pll_user_p,
	    MUX_SEL_TOP10, 12, 1),
	MUX(0, "mout_top1_bus0_pll_user", mout_top1_bus0_pll_user_p,
	    MUX_SEL_TOP10, 16, 1),

	MUX(0, "mout_top1_mfc_pll_half", mout_top1_mfc_pll_half_p,
	    MUX_SEL_TOP11, 4, 1),
	MUX(0, "mout_top1_cci_pll_half", mout_top1_cci_pll_half_p,
	    MUX_SEL_TOP11, 8, 1),
	MUX(0, "mout_top1_bus1_pll_half", mout_top1_bus1_pll_half_p,
	    MUX_SEL_TOP11, 12, 1),
	MUX(0, "mout_top1_bus0_pll_half", mout_top1_bus0_pll_half_p,
	    MUX_SEL_TOP11, 16, 1),

	MUX(0, "mout_aclk_fsys1_200", mout_top1_group1, MUX_SEL_TOP13, 24, 2),
	MUX(0, "mout_aclk_fsys0_200", mout_top1_group1, MUX_SEL_TOP13, 28, 2),

	MUX(0, "mout_sclk_phy_fsys0_26m", mout_top1_group1,
	    MUX_SEL_TOP1_FSYS0, 0, 2),
	MUX(0, "mout_sclk_phy_fsys0", mout_top1_group1,
	    MUX_SEL_TOP1_FSYS0, 8, 2),
	MUX(0, "mout_sclk_mmc2", mout_top1_group1, MUX_SEL_TOP1_FSYS0, 16, 2),
	MUX(0, "mout_sclk_usbdrd300", mout_top1_group1,
	    MUX_SEL_TOP1_FSYS0, 28, 2),

	MUX(0, "mout_sclk_phy_fsys1", mout_top1_group1,
	    MUX_SEL_TOP1_FSYS1, 0, 2),
	MUX(0, "mout_sclk_tlx400_wifi1", mout_top1_group1,
	    MUX_SEL_TOP1_FSYS1, 8, 2),
	MUX(0, "mout_sclk_ufsunipro20", mout_top1_group1,
	    MUX_SEL_TOP1_FSYS1, 16, 2),

	MUX(0, "mout_sclk_mmc1", mout_top1_group1, MUX_SEL_TOP1_FSYS11, 0, 2),
	MUX(0, "mout_sclk_mmc0", mout_top1_group1, MUX_SEL_TOP1_FSYS11, 12, 2),
	MUX(0, "mout_sclk_phy_fsys1_26m", mout_top1_group1,
	    MUX_SEL_TOP1_FSYS11, 24, 2),
};

/* Dividers */
static const struct samsung_div_clock top1_div_clks[] __initconst = {
	DIV(0, "dout_aclk_fsys1_200", "mout_aclk_fsys1_200",
	    DIV_TOP13, 24, 4),
	DIV(0, "dout_aclk_fsys0_200", "mout_aclk_fsys0_200",
	    DIV_TOP13, 28, 4),

	DIV(0, "dout_sclk_phy_fsys1", "mout_sclk_phy_fsys1",
	    DIV_TOP1_FSYS1, 0, 6),
	DIV(0, "dout_sclk_tlx400_wifi1", "mout_sclk_tlx400_wifi1",
	    DIV_TOP1_FSYS1, 8, 6),
	DIV(0, "dout_sclk_ufsunipro20",
	       "mout_sclk_ufsunipro20", DIV_TOP1_FSYS1, 16, 6),

	DIV(0, "dout_sclk_phy_fsys0_26m", "mout_sclk_phy_fsys0_26m",
	    DIV_TOP1_FSYS0, 0, 6),
	DIV(0, "dout_sclk_phy_fsys0", "mout_sclk_phy_fsys0",
	    DIV_TOP1_FSYS0, 8, 6),
	DIV(0, "dout_sclk_mmc2", "mout_sclk_mmc2",
	    DIV_TOP1_FSYS0, 16, 10),
	DIV(0, "dout_sclk_usbdrd300", "mout_sclk_usbdrd300",
	    DIV_TOP1_FSYS0, 28, 4),

	DIV(0, "dout_sclk_mmc1", "mout_sclk_mmc1",
	    DIV_TOP1_FSYS11, 0, 10),
	DIV(0, "dout_sclk_mmc0", "mout_sclk_mmc0",
	    DIV_TOP1_FSYS11, 12, 10),
	DIV(DOUT_SCLK_PHY_FSYS1_26M, "dout_sclk_phy_fsys1_26m",
	       "mout_sclk_phy_fsys1_26m", DIV_TOP1_FSYS11, 24, 6),
};

/* Clock gates */
static const struct samsung_gate_clock top1_gate_clks[] __initconst = {
	GATE(SCLK_PHY_FSYS0_26M, "sclk_phy_fsys0_26m",
	     "dout_sclk_phy_fsys0_26m",
	     ENABLE_SCLK_TOP1_FSYS0, 0, 0, 0),
	GATE(SCLK_PHY_FSYS0, "sclk_phy_fsys0", "dout_sclk_phy_fsys0",
	     ENABLE_SCLK_TOP1_FSYS0, 8, 0, 0),
	GATE(SCLK_MMC2, "sclk_mmc2", "dout_sclk_mmc2",
	     ENABLE_SCLK_TOP1_FSYS0, 16, CLK_SET_RATE_PARENT, 0),
	GATE(SCLK_USBDRD300, "sclk_usbdrd300", "dout_sclk_usbdrd300",
	     ENABLE_SCLK_TOP1_FSYS0, 28, 0, 0),

	GATE(SCLK_PHY_FSYS1, "sclk_phy_fsys1", "dout_sclk_phy_fsys1",
	     ENABLE_SCLK_TOP1_FSYS1, 0, CLK_SET_RATE_PARENT, 0),
	GATE(SCLK_TLX400_WIFI1, "sclk_tlx400_wifi1", "dout_sclk_tlx400_wifi1",
	     ENABLE_SCLK_TOP1_FSYS1, 8, CLK_SET_RATE_PARENT, 0),
	GATE(SCLK_UFSUNIPRO20, "sclk_ufsunipro20", "dout_sclk_ufsunipro20",
	     ENABLE_SCLK_TOP1_FSYS1, 16, CLK_SET_RATE_PARENT, 0),

	GATE(SCLK_MMC1, "sclk_mmc1", "dout_sclk_mmc1",
	     ENABLE_SCLK_TOP1_FSYS11, 0, CLK_SET_RATE_PARENT, 0),
	GATE(SCLK_MMC0, "sclk_mmc0", "dout_sclk_mmc0",
	     ENABLE_SCLK_TOP1_FSYS11, 12, CLK_SET_RATE_PARENT, 0),
	GATE(SCLK_PHY_FSYS1_26M, "sclk_phy_fsys1_26m",
	     "dout_sclk_phy_fsys1_26m",
	     ENABLE_SCLK_TOP1_FSYS11, 24, CLK_SET_RATE_PARENT, 0),

	GATE(ACLK_FSYS0_200, "aclk_fsys0_200", "dout_aclk_fsys0_200",
	     ENABLE_ACLK_TOP13, 28, CLK_SET_RATE_PARENT, 0),
	GATE(ACLK_FSYS1_200, "aclk_fsys1_200", "dout_aclk_fsys1_200",
	     ENABLE_ACLK_TOP13, 24, CLK_SET_RATE_PARENT, 0),

        GATE(CLK_SCLK_PHY_FSYS1_26M, "sclk_phy_fsys1_26m",
                "dout_sclk_phy_fsys1_26m", ENABLE_SCLK_TOP1_FSYS11,
                24, CLK_SET_RATE_PARENT, 0),
};

static const struct samsung_cmu_info top1_cmu_info __initconst = {
	.mux_clks = top1_mux_clks,
	.nr_mux_clks = ARRAY_SIZE(top1_mux_clks),
	.div_clks = top1_div_clks,
	.nr_div_clks = ARRAY_SIZE(top1_div_clks),
	.gate_clks = top1_gate_clks,
	.nr_gate_clks = ARRAY_SIZE(top1_gate_clks),
	.fixed_factor_clks = top1_fixed_factor_clks,
	.nr_fixed_factor_clks = ARRAY_SIZE(top1_fixed_factor_clks),
	.nr_clk_ids = TOP1_NR_CLK,
	.clk_regs = top1_clk_regs,
	.nr_clk_regs = ARRAY_SIZE(top1_clk_regs),
};

static void __init exynos7_clk_top1_init(struct device_node *np)
{
	samsung_cmu_register_one(np, &top1_cmu_info);
}

CLK_OF_DECLARE(exynos7_clk_top1, "samsung,exynos7-clock-top1",
	       exynos7_clk_top1_init);


/**********************\
* CMU_G3D (0x14AA0000) *
\**********************/

/* Register Offset definitions */
#define G3D_PLL_LOCK		0x0000
#define G3D_PLL_CON0		0x0100
#define MUX_SEL_G3D		0x0200
#define DIV_G3D			0x0600
#define ENABLE_ACLK_G3D		0x0800
#define ENABLE_PCLK_G3D		0x0900

static const unsigned long g3d_clk_regs[] __initconst = {
	G3D_PLL_LOCK,
	G3D_PLL_CON0,
	MUX_SEL_G3D,
	DIV_G3D,
	ENABLE_ACLK_G3D,
	ENABLE_PCLK_G3D,
};

/* List of parent clocks for Muxes */
PNAME(mout_g3d_pll_ctrl_p) = {"fin_pll", "fout_g3d_pll"};
PNAME(mout_sclk_bus_pll_g3d_user_p) = {"fin_pll", "sclk_bus_pll_g3d"};
PNAME(mout_g3d_p) = {"mout_g3d_pll_ctrl", "mout_sclk_bus_pll_g3d_user"};

/* PLLs */
static const struct samsung_pll_rate_table g3d_pll_table[] __initconst = {
	PLL_35XX_RATE(772000000, 193, 6, 0),
	PLL_35XX_RATE(700000000, 175, 6, 0),
	PLL_35XX_RATE(600000000, 150, 6, 0),
	PLL_35XX_RATE(544000000, 68, 3, 0),
	PLL_35XX_RATE(420000000, 210, 6, 1),
	PLL_35XX_RATE(350000000, 175, 6, 1),
	PLL_35XX_RATE(266000000, 166, 6, 1),
	{},
};

static const struct samsung_pll_clock g3d_pll_clks[] __initconst = {
	PLL(pll_1452x, 0, "fout_g3d_pll", "fin_pll", G3D_PLL_LOCK,
	    G3D_PLL_CON0, g3d_pll_table),
};

/* Muxes */
static const struct samsung_mux_clock g3d_mux_clks[] __initconst = {
	MUX(0, "mout_g3d_pll_ctrl", mout_g3d_pll_ctrl_p,
	    MUX_SEL_G3D, 0, 1),
	MUX(0, "mout_sclk_bus_pll_g3d_user", mout_sclk_bus_pll_g3d_user_p,
	    MUX_SEL_G3D, 4, 1),
	MUX(0, "mout_g3d", mout_g3d_p,
	    MUX_SEL_G3D, 8, 1),
};

/* Dividers */
static const struct samsung_div_clock g3d_div_clks[] __initconst = {
	/* XXX: Ignore CLKMUX_ACLK_G3D since it's not controlled
	 * by the CMU */
	DIV(0, "dout_pclk_g3d", "mout_g3d",
	    DIV_G3D, 4, 3),
};

/* Clock gates */
static const struct samsung_gate_clock g3d_gate_clks[] __initconst = {
	GATE(ACLK_G3D, "aclk_g3d", "mout_g3d",
	     ENABLE_ACLK_G3D, 0, 0, 0),
	GATE(ACLK_G3DND_800, "aclk_g3dnd_800", "mout_g3d",
	     ENABLE_ACLK_G3D, 1, 0, 0),
	GATE(ACLK_ASYNCAPBM_G3D, "aclk_asyncapbm_g3d", "mout_g3d",
	     ENABLE_ACLK_G3D, 2, 0, 0),
	GATE(ACLK_BTS_G3D0, "aclk_bts_g3d0", "mout_g3d",
	     ENABLE_ACLK_G3D, 3, 0, 0),
	GATE(ACLK_BTS_G3D1, "aclk_bts_g3d1", "mout_g3d",
	     ENABLE_ACLK_G3D, 4, 0, 0),
	GATE(ACLK_XIU_G3D, "aclk_xiu_g3d", "mout_g3d",
	     ENABLE_ACLK_G3D, 7, 0, 0),
	GATE(ACLK_SFW100_G3D, "aclk_sfw100_g3d", "mout_g3d",
	     ENABLE_ACLK_G3D, 8, 0, 0),
	GATE(0, "aclk_g3dnp_160", "dout_pclk_g3d",
	     ENABLE_ACLK_G3D, 16, CLK_IGNORE_UNUSED, 0),
	GATE(0, "aclk_ahb2apb_g3dp", "dout_pclk_g3d",
	     ENABLE_ACLK_G3D, 17, CLK_IGNORE_UNUSED, 0),
	GATE(ACLK_ASYNCAPBS_G3D, "aclk_asyncapbs_g3d", "dout_pclk_g3d",
	     ENABLE_ACLK_G3D, 18, 0, 0),

	GATE(0, "pclk_pmu_g3d", "dout_pclk_g3d",
	     ENABLE_PCLK_G3D, 0, CLK_IGNORE_UNUSED, 0),
	GATE(PCLK_BTS_G3D0, "pclk_bts_g3d0", "dout_pclk_g3d",
	     ENABLE_PCLK_G3D, 1, 0, 0),
	GATE(PCLK_BTS_G3D1, "pclk_bts_g3d1", "dout_pclk_g3d",
	     ENABLE_PCLK_G3D, 2, 0, 0),
	GATE(PCLK_GRAY_DEC, "pclk_gray_dec", "dout_pclk_g3d",
	     ENABLE_PCLK_G3D, 6, 0, 0),
	GATE(PCLK_SFW100_G3D, "pclk_sfw100_g3d", "dout_pclk_g3d",
	     ENABLE_PCLK_G3D, 7, 0, 0),
};

static const struct samsung_cmu_info g3d_cmu_info __initconst = {
	.pll_clks = g3d_pll_clks,
	.nr_pll_clks = ARRAY_SIZE(g3d_pll_clks),
	.mux_clks = g3d_mux_clks,
	.nr_mux_clks = ARRAY_SIZE(g3d_mux_clks),
	.div_clks = g3d_div_clks,
	.nr_div_clks = ARRAY_SIZE(g3d_div_clks),
	.gate_clks = g3d_gate_clks,
	.nr_gate_clks = ARRAY_SIZE(g3d_gate_clks),
	.nr_clk_ids = G3D_NR_CLK,
	.clk_regs = g3d_clk_regs,
	.nr_clk_regs = ARRAY_SIZE(g3d_clk_regs),
};

static void __init exynos7_clk_g3d_init(struct device_node *np)
{
	samsung_cmu_register_one(np, &g3d_cmu_info);
}

CLK_OF_DECLARE(exynos7_clk_g3d, "samsung,exynos7-clock-g3d",
	       exynos7_clk_g3d_init);


/************************\
* CMU_CCORE (0x105B0000) *
\************************/

/* Register Offset definitions */
#define MUX_SEL_CCORE			0x0200
#define DIV_CCORE			0x0600
#define ENABLE_ACLK_CCORE0		0x0800
#define ENABLE_ACLK_CCORE1		0x0804
#define ENABLE_PCLK_CCORE		0x0900

static const unsigned long ccore_clk_regs[] __initconst = {
	MUX_SEL_CCORE,
	DIV_CCORE,
	ENABLE_ACLK_CCORE0,
	ENABLE_ACLK_CCORE1,
	ENABLE_PCLK_CCORE,
};

/*
 * List of parent clocks for Muxes in CMU_CCORE
 */
PNAME(mout_aclk_ccore_133_user_p) = {"fin_pll", "aclk_ccore_133"};
PNAME(mout_aclk_ccore_532_user_p) = {"fin_pll", "aclk_ccore_532"};

/* Muxes */
static const struct samsung_mux_clock ccore_mux_clks[] __initconst = {
	MUX(0, "mout_aclk_ccore_532_user", mout_aclk_ccore_532_user_p,
	    MUX_SEL_CCORE, 0, 1),
	MUX(0, "mout_aclk_ccore_133_user", mout_aclk_ccore_133_user_p,
	    MUX_SEL_CCORE, 1, 1),
};

/* Dividers */
static const struct samsung_div_clock ccore_div_clks[] __initconst = {
	DIV(0, "dout_aclk_ccore_266", "mout_aclk_ccore_532_user",
	    DIV_CCORE, 0, 3),
};

/* Clock gates */
static const struct samsung_gate_clock ccore_gate_clks[] __initconst = {
	GATE(ACLK_CCI, "aclk_cci", "mout_aclk_ccore_532_user",
	     ENABLE_ACLK_CCORE0, 0, CLK_IGNORE_UNUSED, 0),
	GATE(0, "aclk_axi_syncdn_ccis", "mout_aclk_ccore_532_user",
	     ENABLE_ACLK_CCORE0, 9, CLK_IGNORE_UNUSED, 0),

	GATE(0, "aclk_axi_ds_ccim", "dout_aclk_ccore_266",
	     ENABLE_ACLK_CCORE1, 0, CLK_IGNORE_UNUSED, 0),
	GATE(0, "aclk_xiu_ccore_sfrx", "dout_aclk_ccore_266",
	     ENABLE_ACLK_CCORE1, 2, CLK_IGNORE_UNUSED, 0),
	GATE(0, "aclk_axi_as_si_sfr", "dout_aclk_ccore_266",
	     ENABLE_ACLK_CCORE1, 3, CLK_IGNORE_UNUSED, 0),
	GATE(0, "aclk_noc_p_ccore", "mout_aclk_ccore_133_user",
	     ENABLE_ACLK_CCORE1, 16, CLK_IGNORE_UNUSED, 0),
	GATE(0, "aclk_axi_as_mi_sfr", "mout_aclk_ccore_532_user",
	     ENABLE_ACLK_CCORE1, 17, CLK_IGNORE_UNUSED, 0),

	GATE(0, "pclk_topc_apbif", "mout_aclk_ccore_133_user",
	     ENABLE_PCLK_CCORE, 3, CLK_IGNORE_UNUSED, 0),
	GATE(0, "pclk_top0_apbif", "mout_aclk_ccore_133_user",
	     ENABLE_PCLK_CCORE, 4, CLK_IGNORE_UNUSED, 0),
	GATE(0, "pclk_top1_apbif", "mout_aclk_ccore_133_user",
	     ENABLE_PCLK_CCORE, 5, CLK_IGNORE_UNUSED, 0),
	GATE(0, "pclk_pmu_apbif", "mout_aclk_ccore_133_user",
	     ENABLE_PCLK_CCORE, 6, CLK_IGNORE_UNUSED, 0),
	GATE(0, "pclk_pmu_ccore", "mout_aclk_ccore_133_user",
	     ENABLE_PCLK_CCORE, 7, CLK_IGNORE_UNUSED, 0),
	GATE(PCLK_RTC, "pclk_rtc", "mout_aclk_ccore_133_user",
	     ENABLE_PCLK_CCORE, 8, 0, 0),
	GATE(0, "pclk_gpio_alive", "mout_aclk_ccore_133_user",
	     ENABLE_PCLK_CCORE, 10, CLK_IGNORE_UNUSED, 0),
	GATE(0, "pclk_axi_as_si_sfr", "mout_aclk_ccore_133_user",
	     ENABLE_PCLK_CCORE, 31, CLK_IGNORE_UNUSED, 0),
};

static const struct samsung_cmu_info ccore_cmu_info __initconst = {
	.mux_clks = ccore_mux_clks,
	.nr_mux_clks = ARRAY_SIZE(ccore_mux_clks),
	.div_clks = ccore_div_clks,
	.nr_div_clks = ARRAY_SIZE(ccore_div_clks),
	.gate_clks = ccore_gate_clks,
	.nr_gate_clks = ARRAY_SIZE(ccore_gate_clks),
	.nr_clk_ids = CCORE_NR_CLK,
	.clk_regs = ccore_clk_regs,
	.nr_clk_regs = ARRAY_SIZE(ccore_clk_regs),
};

static void __init exynos7_clk_ccore_init(struct device_node *np)
{
	samsung_cmu_register_one(np, &ccore_cmu_info);
}

CLK_OF_DECLARE(exynos7_clk_ccore, "samsung,exynos7-clock-ccore",
	       exynos7_clk_ccore_init);


/***********************\
* CMU_CAM0 (0x144D0000) *
\***********************/

/* Register Offset definitions */
#define MUX_SEL_CAM00			0x0200
#define MUX_SEL_CAM01			0x0204
#define MUX_SEL_CAM02			0x0208
#define DIV_CAM0			0x0600
#define ENABLE_ACLK_CAM00		0x0800
#define ENABLE_ACLK_CAM01		0x0804
#define ENABLE_PCLK_CAM00		0x0900
#define ENABLE_PCLK_CAM01		0x0904
#define ENABLE_SCLK_CAM0		0x0A00

static const unsigned long cam0_clk_regs[] __initconst = {
	MUX_SEL_CAM00,
	MUX_SEL_CAM01,
	MUX_SEL_CAM02,
	DIV_CAM0,
	ENABLE_ACLK_CAM00,
	ENABLE_ACLK_CAM01,
	ENABLE_PCLK_CAM00,
	ENABLE_PCLK_CAM01,
	ENABLE_SCLK_CAM0,
};

/* List of parent clocks for Muxes */
PNAME(mout_aclk_cam0_csis0_690_user_p) = {"fin_pll", "aclk_cam0_csis0_690"};
PNAME(mout_aclk_cam0_bnsa_690_user_p) = {"fin_pll", "aclk_cam0_bnsa_690"};
PNAME(mout_aclk_cam0_bnsb_690_user_p) = {"fin_pll", "aclk_cam0_bnsb_690"};
PNAME(mout_aclk_cam0_bnsd_690_user_p) = {"fin_pll", "aclk_cam0_bnsd_690"};
PNAME(mout_aclk_cam0_csis1_174_user_p) = {"fin_pll", "aclk_cam0_csis1_174"};
PNAME(mout_aclk_cam0_3aa0_690_user_p) = {"fin_pll", "aclk_cam0_3aa0_690"};
PNAME(mout_aclk_cam0_3aa1_468_user_p) = {"fin_pll", "aclk_cam0_3aa1_468"};
PNAME(mout_aclk_cam0_trex_532_user_p) = {"fin_pll", "aclk_cam0_trex_532"};
PNAME(mout_aclk_cam0_nocp_133_user_p) = {"fin_pll", "aclk_cam0_nocp_133"};
PNAME(mout_aclk_cam0_csis3_133_user_p) = {"fin_pll", "aclk_cam0_csis3_133"};

PNAME(mout_phyclk_rxbyteclkhs0_s2a_user_p) = {"fin_pll",
					      "phyclk_rxbyteclkhs0_s2a"};

PNAME(mout_phyclk_rxbyteclkhs0_s4_user_p) = {"fin_pll",
					     "phyclk_rxbyteclkhs0_s4"};

PNAME(mout_phyclk_rxbyteclkhs1_s4_user_p) = {"fin_pll",
					     "phyclk_rxbyteclkhs1_s4"};

PNAME(mout_phyclk_rxbyteclkhs2_s4_user_p) = {"fin_pll",
					     "phyclk_rxbyteclkhs2_s4"};

PNAME(mout_phyclk_rxbyteclkhs3_s4_user_p) = {"fin_pll",
					     "phyclk_rxbyteclkhs3_s4"};

PNAME(mout_phyclk_rxbyteclkhs0_s1a_user_p) = {"fin_pll",
					      "phyclk_rxbyteclkhs0_s1a"};

/* Fixed rate clocks */
static const struct samsung_fixed_rate_clock fixed_rate_clks_cam0[] __initconst = {
	FRATE(0, "phyclk_rxbyteclkhs0_s2a", NULL, CLK_IS_ROOT, 250000000),
	FRATE(0, "phyclk_rxbyteclkhs0_s4", NULL, CLK_IS_ROOT, 250000000),
	FRATE(0, "phyclk_rxbyteclkhs1_s4", NULL, CLK_IS_ROOT, 250000000),
	FRATE(0, "phyclk_rxbyteclkhs2_s4", NULL, CLK_IS_ROOT, 250000000),
	FRATE(0, "phyclk_rxbyteclkhs3_s4", NULL, CLK_IS_ROOT, 250000000),
	FRATE(0, "phyclk_rxbyteclkhs0_s1a", NULL, CLK_IS_ROOT, 250000000),
};

/* Muxes */
static const struct samsung_mux_clock cam0_mux_clks[] __initconst = {
	MUX(0, "mout_aclk_cam0_csis0_690_user",
	    mout_aclk_cam0_csis0_690_user_p, MUX_SEL_CAM00, 0, 1),
	MUX(0, "mout_aclk_cam0_bnsa_690_user",
	    mout_aclk_cam0_bnsa_690_user_p, MUX_SEL_CAM00, 4, 1),
	MUX(0, "mout_aclk_cam0_bnsb_690_user",
	    mout_aclk_cam0_bnsb_690_user_p, MUX_SEL_CAM00, 8, 1),
	MUX(0, "mout_aclk_cam0_bnsd_690_user",
	    mout_aclk_cam0_bnsd_690_user_p, MUX_SEL_CAM00, 12, 1),
	MUX(0, "mout_aclk_cam0_csis1_174_user",
	    mout_aclk_cam0_csis1_174_user_p, MUX_SEL_CAM00, 16, 1),
	MUX(0, "mout_aclk_cam0_3aa0_690_user",
	    mout_aclk_cam0_3aa0_690_user_p, MUX_SEL_CAM00, 20, 1),
	MUX(0, "mout_aclk_cam0_3aa1_468_user",
	    mout_aclk_cam0_3aa1_468_user_p, MUX_SEL_CAM00, 24, 1),
	MUX(0, "mout_aclk_cam0_trex_532_user",
	    mout_aclk_cam0_trex_532_user_p, MUX_SEL_CAM00, 28, 1),

	MUX(0, "mout_aclk_cam0_nocp_133_user",
	    mout_aclk_cam0_nocp_133_user_p, MUX_SEL_CAM01, 0, 1),
	MUX(0, "mout_aclk_cam0_csis3_133_user",
	    mout_aclk_cam0_csis3_133_user_p, MUX_SEL_CAM01, 4, 1),

	MUX(0, "mout_phyclk_rxbyteclkhs0_s2a_user",
	    mout_phyclk_rxbyteclkhs0_s2a_user_p, MUX_SEL_CAM02, 0, 1),
	MUX(0, "mout_phyclk_rxbyteclkhs0_s4_user",
	    mout_phyclk_rxbyteclkhs0_s4_user_p, MUX_SEL_CAM02, 4, 1),
	MUX(0, "mout_phyclk_rxbyteclkhs1_s4_user",
	    mout_phyclk_rxbyteclkhs1_s4_user_p, MUX_SEL_CAM02, 8, 1),
	MUX(0, "mout_phyclk_rxbyteclkhs2_s4_user",
	    mout_phyclk_rxbyteclkhs2_s4_user_p, MUX_SEL_CAM02, 12, 1),
	MUX(0, "mout_phyclk_rxbyteclkhs3_s4_user",
	    mout_phyclk_rxbyteclkhs3_s4_user_p, MUX_SEL_CAM02, 16, 1),
	MUX(0, "mout_phyclk_rxbyteclkhs0_s1a_user",
	    mout_phyclk_rxbyteclkhs0_s1a_user_p, MUX_SEL_CAM02, 20, 1),
};

/* Dividers */
static const struct samsung_div_clock cam0_div_clks[] __initconst = {
	DIV(0, "dout_pclk_cam0_bnsa_345", "mout_aclk_cam0_bnsa_690_user",
	    DIV_CAM0, 0, 3),
	DIV(0, "dout_pclk_cam0_bnsb_345", "mout_aclk_cam0_bnsb_690_user",
	    DIV_CAM0, 3, 3),
	DIV(0, "dout_pclk_cam0_bnsd_345", "mout_aclk_cam0_bnsd_690_user",
	    DIV_CAM0, 8, 3),
	DIV(0, "dout_pclk_cam0_3aa0_345", "mout_aclk_cam0_3aa0_690_user",
	    DIV_CAM0, 12, 3),
	DIV(0, "dout_pclk_cam0_3aa1_234", "mout_aclk_cam0_3aa1_468_user",
	    DIV_CAM0, 16, 3),
	DIV(0, "dout_pclk_cam0_trex_266", "mout_aclk_cam0_trex_532_user",
	    DIV_CAM0, 20, 3),
	DIV(0, "dout_pclk_cam0_trex_133", "dout_pclk_cam0_trex_266",
	    DIV_CAM0, 24, 3),
};

/* Clock gates */
static const struct samsung_gate_clock cam0_gate_clks[] __initconst = {
	GATE(ACLK_FIMC_3AA0, "aclk_fimc_3aa0", "mout_aclk_cam0_3aa0_690_user",
	     ENABLE_ACLK_CAM00, 0, 0, 0),
	GATE(ACLK_FIMC_3AA1, "aclk_fimc_3aa1", "mout_aclk_cam0_3aa1_468_user",
	     ENABLE_ACLK_CAM00, 1, 0, 0),
	GATE(ACLK_FIMC_BNS_A, "aclk_fimc_bns_a", "mout_aclk_cam0_bnsa_690_user",
	     ENABLE_ACLK_CAM00, 2, 0, 0),
	GATE(ACLK_FIMC_BNS_B, "aclk_fimc_bns_b", "mout_aclk_cam0_bnsb_690_user",
	     ENABLE_ACLK_CAM00, 3, 0, 0),
	GATE(ACLK_FIMC_BNS_D, "aclk_fimc_bns_d", "mout_aclk_cam0_bnsd_690_user",
	     ENABLE_ACLK_CAM00, 4, 0, 0),
	GATE(ACLK_PXL_ASBM_FIMC_BNS_C_INT, "aclk_lxk_asbm_fimc_bns_c_int",
	     "fin_pll",
	     ENABLE_ACLK_CAM00, 8, 0, 0),
	GATE(ACLK_PXL_ASBS_3AA0_IN, "aclk_pxl_asbs_3aa0_in",
	     "mout_aclk_cam0_3aa0_690_user",
	     ENABLE_ACLK_CAM00, 9, 0, 0),
	GATE(ACLK_PXL_ASBS_3AA1_IN, "aclk_pxl_asbs_3aa1_in",
	     "mout_aclk_cam0_3aa1_468_user",
	     ENABLE_ACLK_CAM00, 10, 0, 0),
	GATE(ACLK_PXL_ASBS_FIMC_BNS_C, "aclk_plc_asbs_fimc_bns_c",
	     "mout_aclk_cam0_3aa0_690_user",
	     ENABLE_ACLK_CAM00, 11, 0, 0),
	GATE(ACLK_PXL_ASBS_FIMC_BNS_C_INT, "aclk_px_asbs_fimc_bns_c_int",
	     "fin_pll",
	     ENABLE_ACLK_CAM00, 12, 0, 0),
	GATE(ACLK_CSIS0_I_WRAP, "aclk_csis0_i_wrap",
	     "mout_aclk_cam0_csis0_690_user",
	     ENABLE_ACLK_CAM00, 16, 0, 0),
	GATE(ACLK_CSIS1_I_WRAP, "aclk_cscs1_i_wrap",
	     "mout_aclk_cam0_csis1_174_user",
	     ENABLE_ACLK_CAM00, 17, 0, 0),
	GATE(ACLK_CSIS3_I_WRAP, "aclk_csis3_i_wrap",
	     "mout_aclk_cam0_csis3_133_user",
	     ENABLE_ACLK_CAM00, 18, CLK_IGNORE_UNUSED, 0),
	GATE(ACLK_TREX_A_5X1, "aclk_trex_a_5x1", "mout_aclk_cam0_trex_532_user",
	     ENABLE_ACLK_CAM00, 20, 0, 0),
	GATE(ACLK_TREX_A_5X1_AA0, "aclk_trex_a_5x1_aa0",
	     "mout_aclk_cam0_3aa0_690_user",
	     ENABLE_ACLK_CAM00, 21, 0, 0),
	GATE(ACLK_TREX_A_5X1_AA1, "aclk_trex_a_5x1_aa1",
	     "mout_aclk_cam0_3aa1_468_user",
	     ENABLE_ACLK_CAM00, 22, 0, 0),
	GATE(ACLK_TREX_A_5X1_BNS_A, "aclk_trex_a_5x1_bns_a",
	     "mout_aclk_cam0_bnsa_690_user",
	     ENABLE_ACLK_CAM00, 23, 0, 0),
	GATE(ACLK_TREX_A_5X1_BNS_B, "aclk_trex_a_5x1_bns_b",
	     "mout_aclk_cam0_bnsb_690_user",
	     ENABLE_ACLK_CAM00, 24, 0, 0),
	GATE(ACLK_TREX_A_5X1_BNS_D, "aclk_trex_a_5x1_bns_d",
	     "mout_aclk_cam0_bnsd_690_user",
	     ENABLE_ACLK_CAM00, 25, 0, 0),

	GATE(ACLK_SYSMMU6, "aclk_sysmmu6", "mout_aclk_cam0_trex_532_user",
	     ENABLE_ACLK_CAM01, 0, 0, 0),
	GATE(0, "aclk_axi2ahb_is0p", "dout_pclk_cam0_trex_266",
	     ENABLE_ACLK_CAM01, 8, CLK_IGNORE_UNUSED, 0),
	GATE(0, "aclk_axi2apb_bridge_is0p", "dout_pclk_cam0_trex_266",
	     ENABLE_ACLK_CAM01, 9, CLK_IGNORE_UNUSED, 0),
	GATE(ACLK_AXI_LH_ASYNC_SI_TOP_CAM0, "aclk_axi_lh_async_si_top_cam0",
	     "mout_aclk_cam0_trex_532_user",
	     ENABLE_ACLK_CAM01, 10, 0, 0),
	GATE(0, "hclk_ahb2apb_bridge_is0p", "dout_pclk_cam0_trex_133",
	     ENABLE_ACLK_CAM01, 11, CLK_IGNORE_UNUSED, 0),
	GATE(0, "hclk_ahbsyncdn_cam0", "dout_pclk_cam0_trex_266",
	     ENABLE_ACLK_CAM01, 12, CLK_IGNORE_UNUSED, 0),
	GATE(0, "aclk_xiu_async_mi_cam0", "dout_pclk_cam0_trex_266",
	     ENABLE_ACLK_CAM01, 16, CLK_IGNORE_UNUSED, 0),
	GATE(0, "aclk_xiu_async_mi_is0x", "dout_pclk_cam0_trex_266",
	     ENABLE_ACLK_CAM01, 17, CLK_IGNORE_UNUSED, 0),
	GATE(0, "aclk_xiu_async_si_is0x", "mout_aclk_cam0_nocp_133_user",
	     ENABLE_ACLK_CAM01, 18, CLK_IGNORE_UNUSED, 0),
	GATE(0, "aclk_xiu_is0x", "dout_pclk_cam0_trex_266",
	     ENABLE_ACLK_CAM01, 19, CLK_IGNORE_UNUSED, 0),
	GATE(CCLK_ASYNCAPB_SOCP_3AA0, "cclk_asyncapb_socp_3aa0",
	     "dout_pclk_cam0_3aa0_345",
	     ENABLE_ACLK_CAM01, 20, 0, 0),
	GATE(CCLK_ASYNCAPB_SOCP_3AA1, "cclk_asyncapb_socp_3aa1",
	     "dout_pclk_cam0_3aa1_234",
	     ENABLE_ACLK_CAM01, 21, 0, 0),
	GATE(CCLK_ASYNCAPB_SOCP_FIMC_BNS_A, "cclk_asyncapb_socp_fimc_bns_a",
	     "dout_pclk_cam0_bnsa_345",
	     ENABLE_ACLK_CAM01, 22, 0, 0),
	GATE(CCLK_ASYNCAPB_SOCP_FIMC_BNS_B, "cclk_asyncapb_socp_fimc_bns_b",
	     "dout_pclk_cam0_bnsb_345",
	     ENABLE_ACLK_CAM01, 23, 0, 0),
	GATE(CCLK_ASYNCAPB_SOCP_FIMC_BNS_D, "cclk_asyncapb_socp_fimc_bns_d",
	     "dout_pclk_cam0_bnsd_345",
	     ENABLE_ACLK_CAM01, 24, 0, 0),
	GATE(0, "aclk_200_cam0_noc_p_cam0", "mout_aclk_cam0_nocp_133_user",
	     ENABLE_ACLK_CAM01, 28, CLK_IGNORE_UNUSED, 0),

	GATE(PCLK_FIMC_3AA0, "pclk_fimc_3aa0", "dout_pclk_cam0_3aa0_345",
	     ENABLE_PCLK_CAM00, 0, 0, 0),
	GATE(PCLK_FIMC_3AA1, "pclk_fimc_3aa1", "dout_pclk_cam0_3aa1_234",
	     ENABLE_PCLK_CAM00, 1, 0, 0),
	GATE(PCLK_FIMC_BNS_A, "pclk_fimc_bns_a", "dout_pclk_cam0_bnsa_345",
	     ENABLE_PCLK_CAM00, 2, 0, 0),
	GATE(PCLK_FIMC_BNS_B, "pclk_fimc_bns_b", "dout_pclk_cam0_bnsb_345",
	     ENABLE_PCLK_CAM00, 3, 0, 0),
	GATE(PCLK_FIMC_BNS_D, "pclk_fimc_bns_d", "dout_pclk_cam0_bnsd_345",
	     ENABLE_PCLK_CAM00, 4, 0, 0),
	GATE(PCLK_CSIS0, "pclk_csis0", "dout_pclk_cam0_trex_266",
	     ENABLE_PCLK_CAM00, 16, 0, 0),
	GATE(PCLK_CSIS1, "pclk_csis1", "dout_pclk_cam0_trex_266",
	     ENABLE_PCLK_CAM00, 17, 0, 0),
	GATE(PCLK_CSIS3, "pclk_csis3", "dout_pclk_cam0_trex_266",
	     ENABLE_PCLK_CAM00, 18, 0, 0),
	GATE(PCLK_TREX_A_5X1, "pclk_trex_a_5x1", "dout_pclk_cam0_trex_133",
	     ENABLE_PCLK_CAM00, 20, 0, 0),

	GATE(PCLK_SYSMMU6, "pclk_sysmmu6", "dout_pclk_cam0_trex_266",
	     ENABLE_PCLK_CAM01, 0, 0, 0),
	GATE(0, "pclk_xiu_async_mi_cam0", "dout_pclk_cam0_trex_266",
	     ENABLE_PCLK_CAM01, 16, CLK_IGNORE_UNUSED, 0),
	GATE(0, "pclk_xiu_async_mi_is0x", "dout_pclk_cam0_trex_133",
	     ENABLE_PCLK_CAM01, 17, CLK_IGNORE_UNUSED, 0),
	GATE(PCLK_ASYNCAPB_SOCP_3AA0, "pclk_asyncapb_socp_3aa0",
	     "dout_pclk_cam0_trex_266",
	     ENABLE_PCLK_CAM01, 20, 0, 0),
	GATE(PCLK_ASYNCAPB_SOCP_3AA1, "pclk_asyncapb_socp_3aa1",
	     "dout_pclk_cam0_trex_266",
	     ENABLE_PCLK_CAM01, 21, 0, 0),
	GATE(PCLK_ASYNCAPB_SOCP_FIMC_BNS_A, "pclk_asyncapb_socp_fimc_bns_a",
	     "dout_pclk_cam0_trex_266",
	     ENABLE_PCLK_CAM01, 22, 0, 0),
	GATE(PCLK_ASYNCAPB_SOCP_FIMC_BNS_B, "pclk_asyncapb_socp_fimc_bns_b",
	     "dout_pclk_cam0_trex_266",
	     ENABLE_PCLK_CAM01, 23, 0, 0),
	GATE(PCLK_ASYNCAPB_SOCP_FIMC_BNS_D, "pclk_asyncapb_socp_fimc_bns_d",
	     "dout_pclk_cam0_trex_266",
	     ENABLE_PCLK_CAM01, 24, 0, 0),
	GATE(PCLK_FREERUNCNT, "pclk_freeruncnt", "dout_pclk_cam0_trex_266",
	     ENABLE_PCLK_CAM01, 30, 0, 0),
	GATE(0, "pclk_pmu_cam0", "dout_pclk_cam0_trex_133",
	     ENABLE_PCLK_CAM01, 31, CLK_IGNORE_UNUSED, 0),

	GATE(PHYCLK_RXBYTECLKHS0_S2A_USER, "phyclk_rxbyteclkhs0_s2a_user",
	     "mout_phyclk_rxbyteclkhs0_s2a_user",
	     ENABLE_SCLK_CAM0, 0, 0, 0),
	GATE(PHYCLK_RXBYTECLKHS0_S4_USER, "phyclk_rxbyteclkhs0_s4_user",
	     "mout_phyclk_rxbyteclkhs0_s4_user",
	     ENABLE_SCLK_CAM0, 4, 0, 0),
	GATE(PHYCLK_RXBYTECLKHS1_S4_USER, "phyclk_rxbyteclkhs1_s4_user",
	     "mout_phyclk_rxbyteclkhs1_s4_user",
	     ENABLE_SCLK_CAM0, 5, 0, 0),
	GATE(PHYCLK_RXBYTECLKHS2_S4_USER, "phyclk_rxbyteclkhs2_s4_user",
	     "mout_phyclk_rxbyteclkhs2_s4_user",
	     ENABLE_SCLK_CAM0, 6, 0, 0),
	GATE(PHYCLK_RXBYTECLKHS3_S4_USER, "phyclk_rxbyteclkhs3_s4_user",
	     "mout_phyclk_rxbyteclkhs3_s4_user",
	     ENABLE_SCLK_CAM0, 7, 0, 0),
	GATE(PHYCLK_RXBYTECLKHS0_S1A_USER, "phyclk_rxbyteclkhs0_s1a_user",
	     "mout_phyclk_rxbyteclkhs0_s1a_user",
	     ENABLE_SCLK_CAM0, 8, 0, 0),
};

static const struct samsung_cmu_info cam0_cmu_info __initconst = {
	.fixed_clks = fixed_rate_clks_cam0,
	.nr_fixed_clks = ARRAY_SIZE(fixed_rate_clks_cam0),
	.mux_clks = cam0_mux_clks,
	.nr_mux_clks = ARRAY_SIZE(cam0_mux_clks),
	.div_clks = cam0_div_clks,
	.nr_div_clks = ARRAY_SIZE(cam0_div_clks),
	.gate_clks = cam0_gate_clks,
	.nr_gate_clks = ARRAY_SIZE(cam0_gate_clks),
	.nr_clk_ids = CAM0_NR_CLK,
	.clk_regs = cam0_clk_regs,
	.nr_clk_regs = ARRAY_SIZE(cam0_clk_regs),
};

static void __init exynos7_clk_cam0_init(struct device_node *np)
{
	samsung_cmu_register_one(np, &cam0_cmu_info);
}

CLK_OF_DECLARE(exynos7_clk_cam0, "samsung,exynos7-clock-cam0",
	       exynos7_clk_cam0_init);

/***********************\
* CMU_CAM1 (0x145D0000) *
\***********************/

/* Register Offset definitions */
#define MUX_SEL_CAM10			0x0200
#define MUX_SEL_CAM11			0x0204
#define DIV_CAM1			0x0600
#define ENABLE_ACLK_CAM10		0x0800
#define ENABLE_ACLK_CAM11		0x0804
#define ENABLE_ACLK_CAM12		0x0808
#define ENABLE_ACLK_CAM13		0x080C
#define ENABLE_PCLK_CAM10		0x0900
#define ENABLE_PCLK_CAM11		0x0904
#define ENABLE_PCLK_CAM12		0x0908
#define ENABLE_SCLK_CAM10		0x0A00
#define ENABLE_SCLK_CAM12		0x0A08

static const unsigned long cam1_clk_regs[] __initconst = {
	MUX_SEL_CAM10,
	MUX_SEL_CAM11,
	DIV_CAM1,
	ENABLE_ACLK_CAM10,
	ENABLE_ACLK_CAM11,
	ENABLE_ACLK_CAM12,
	ENABLE_ACLK_CAM13,
	ENABLE_PCLK_CAM10,
	ENABLE_PCLK_CAM11,
	ENABLE_PCLK_CAM12,
	ENABLE_SCLK_CAM10,
	ENABLE_SCLK_CAM12,
};

/* List of parent clocks for Muxes */
PNAME(mout_aclk_cam1_sclvra_491_user_p) = {"fin_pll", "aclk_cam1_sclvra_491"};
PNAME(mout_aclk_cam1_arm_668_user_p) 	= {"fin_pll", "aclk_cam1_arm_668"};
PNAME(mout_aclk_cam1_busperi_334_user_p) = {"fin_pll", "aclk_cam1_busperi_334"};
PNAME(mout_aclk_cam1_bnscsis_133_user_p) = {"fin_pll", "aclk_cam1_bnscsis_133"};
PNAME(mout_aclk_cam1_nocp_133_user_p) 	= {"fin_pll", "aclk_cam1_nocp_133"};
PNAME(mout_aclk_cam1_trex_532_user_p) 	= {"fin_pll", "aclk_cam1_trex_532"};
PNAME(mout_sclk_isp_spi0_user_p) 	= {"fin_pll", "sclk_isp_spi0"};
PNAME(mout_sclk_isp_spi1_user_p) 	= {"fin_pll", "sclk_isp_spi1"};
PNAME(mout_sclk_isp_uart_user_p) 	= {"fin_pll", "sclk_isp_uart"};

PNAME(mout_phyclk_hs0_csis2_rx_byte_user_p) = {"fin_pll",
					       "phyclk_hs0_csis2_rx_byte"};

/* Fixed rate clocks */
static const struct samsung_fixed_rate_clock fixed_rate_clks_cam1[] __initconst = {
	FRATE(0, "phyclk_hs0_csis2_rx_byte", NULL,
	      CLK_IS_ROOT, 250000000),
};

/* Muxes */
static const struct samsung_mux_clock cam1_mux_clks[] __initconst = {
	MUX(0, "mout_aclk_cam1_sclvra_491_user",
	    mout_aclk_cam1_sclvra_491_user_p, MUX_SEL_CAM10, 0, 1),
	MUX(0, "mout_aclk_cam1_arm_668_user",
	    mout_aclk_cam1_arm_668_user_p, MUX_SEL_CAM10, 4, 1),
	MUX(0, "mout_aclk_cam1_busperi_334_user",
	    mout_aclk_cam1_busperi_334_user_p, MUX_SEL_CAM10, 8, 1),
	MUX(0, "mout_aclk_cam1_bnscsis_133_user",
	    mout_aclk_cam1_bnscsis_133_user_p, MUX_SEL_CAM10, 12, 1),
	MUX(0, "mout_aclk_cam1_nocp_133_user",
	    mout_aclk_cam1_nocp_133_user_p, MUX_SEL_CAM10, 16, 1),
	MUX(0, "mout_aclk_cam1_trex_532_user",
	    mout_aclk_cam1_trex_532_user_p, MUX_SEL_CAM10, 20, 1),

	MUX(0, "mout_sclk_isp_spi0_user",
	    mout_sclk_isp_spi0_user_p, MUX_SEL_CAM11, 0, 1),
	MUX(0, "mout_sclk_isp_spi1_user",
	    mout_sclk_isp_spi1_user_p, MUX_SEL_CAM11, 4, 1),
	MUX(0, "mout_sclk_isp_uart_user",
	    mout_sclk_isp_uart_user_p, MUX_SEL_CAM11, 8, 1),
	MUX(0, "mout_phyclk_hs0_csis2_rx_byte_user",
	    mout_phyclk_hs0_csis2_rx_byte_user_p, MUX_SEL_CAM11, 28, 1),
};

/* Dividers */
static const struct samsung_div_clock cam1_div_clks[] __initconst = {
	DIV(0, "dout_pclk_cam1_arm_167", "mout_aclk_cam1_arm_668_user",
	    DIV_CAM1, 0, 3),
	DIV(0, "dout_pclk_cam1_busperi_167", "mout_aclk_cam1_busperi_334_user",
	    DIV_CAM1, 4, 3),
	DIV(0, "dout_pclk_cam1_busperi_84", "mout_aclk_cam1_busperi_334_user",
	    DIV_CAM1, 8, 3),
	DIV(0, "dout_pclk_cam1_sclvra_264", "mout_aclk_cam1_sclvra_491_user",
	    DIV_CAM1, 12, 3),
};

/* Clock gates */
static const struct samsung_gate_clock cam1_gate_clks[] __initconst = {
	GATE(ACLK_FIMC_BNS_C, "aclk_fimc_bns_c",
	     "mout_aclk_cam1_bnscsis_133_user",
	     ENABLE_ACLK_CAM10, 0, CLK_IGNORE_UNUSED, 0),
	GATE(ACLK_FIMC_SCALER, "aclk_fimc_scaler",
	     "mout_aclk_cam1_sclvra_491_user",
	     ENABLE_ACLK_CAM10, 2, CLK_IGNORE_UNUSED, 0),
	GATE(ACLK_FIMC_VRA, "aclk_fimc_vra", "mout_aclk_cam1_sclvra_491_user",
	     ENABLE_ACLK_CAM10, 3, CLK_IGNORE_UNUSED, 0),
	GATE(CLK_B_TREX_B, "clk_b_trex_b", "mout_aclk_cam1_trex_532_user",
	     ENABLE_ACLK_CAM10, 4, CLK_IGNORE_UNUSED, 0),
	GATE(CLK_BNS_C_TREX_B, "clk_bns_c_trex_b",
	     "mout_aclk_cam1_bnscsis_133_user",
	     ENABLE_ACLK_CAM10, 5, CLK_IGNORE_UNUSED, 0),
	GATE(ACLK_WRAP_CSIS2, "aclk_wrap_csis2",
	     "mout_aclk_cam1_bnscsis_133_user",
	     ENABLE_ACLK_CAM10, 6, CLK_IGNORE_UNUSED, 0),
	GATE(CLK_CSATBDOWNSIZER_CAM1, "aclk_csatbdownsizer_cam1",
	     "dout_pclk_cam1_arm_167",
	     ENABLE_ACLK_CAM10, 7, 0, 0),
	GATE(ACLK_SYSMMU6_FIMC_BNS_C, "aclk_sysmmu6_fimc_bns_c",
	     "mout_aclk_cam1_bnscsis_133_user",
	     ENABLE_ACLK_CAM10, 8, CLK_IGNORE_UNUSED, 0),
	GATE(ACLK_SYSMMU6_FIMC_ISP1, "aclk_sysmmu6_fimc_isp1",
	     "mout_aclk_cam1_trex_532_user",
	     ENABLE_ACLK_CAM10, 9, CLK_IGNORE_UNUSED, 0),
	GATE(ACLK_SYSMMU6_FIMC_SCALER, "aclk_sysmmu6_fimc_scaler",
	     "mout_aclk_cam1_sclvra_491_user",
	     ENABLE_ACLK_CAM10, 11, CLK_IGNORE_UNUSED, 0),
	GATE(ACLK_SYSMMU6_FIMC_VRA, "aclk_sysmmu6_fimc_vra",
	     "mout_aclk_cam1_sclvra_491_user",
	     ENABLE_ACLK_CAM10, 12, CLK_IGNORE_UNUSED, 0),
	GATE(ACLK_SYSMMU6_ISPCX, "aclk_sysmmu6_ispcx",
	     "mout_aclk_cam1_busperi_334_user",
	     ENABLE_ACLK_CAM10, 13, 0, 0),
	GATE(ACLK_GIC_ISP_ARM_SYS, "aclk_gic_isp_arm_sys",
	     "mout_aclk_cam1_busperi_334_user",
	     ENABLE_ACLK_CAM10, 16, 0, 0),
	GATE(CLK_ISP_CPU_TREX_B, "clk_isp_cpu_trex_b",
	     "mout_aclk_cam1_busperi_334_user",
	     ENABLE_ACLK_CAM10, 17, 0, 0),
	GATE(CLK_MCU_ISP_400_ISP_ARM_SYS, "clk_mcu_isp_400_isp_arm_sys",
	     "mout_aclk_cam1_arm_668_user",
	     ENABLE_ACLK_CAM10, 18, 0, 0),
	GATE(ACLK_PXL_ASBS_FROM_BLKC, "aclk_pxl_asbs_from_blkc",
	     "mout_aclk_cam1_sclvra_491_user",
	     ENABLE_ACLK_CAM10, 19, CLK_IGNORE_UNUSED, 0),
	GATE(0, "aclk_r_axispcx", "mout_aclk_cam1_busperi_334_user",
	     ENABLE_ACLK_CAM10, 20, CLK_IGNORE_UNUSED, 0),
	GATE(0, "aclk_r_axisphx", "mout_aclk_cam1_busperi_334_user",
	     ENABLE_ACLK_CAM10, 21, CLK_IGNORE_UNUSED, 0),
	GATE(CLK_SCALER_TREX_B, "clk_scaler_trex_b",
	     "mout_aclk_cam1_sclvra_491_user",
	     ENABLE_ACLK_CAM10, 22, CLK_IGNORE_UNUSED, 0),
	GATE(CLK_VRA_TREX_B, "clk_vra_trex_b", "mout_aclk_cam1_sclvra_491_user",
	     ENABLE_ACLK_CAM10, 25, CLK_IGNORE_UNUSED, 0),
	GATE(ATCLKS_ASATBSLV_CAM1_CSSYS, "atclks_asatbslv_cam1_cssys",
	     "dout_pclk_cam1_arm_167",
	     ENABLE_ACLK_CAM10, 28, 0, 0),

	GATE(HCLK_AHB_SFRISP2H, "hclk_ahb_sfrisp2h",
	     "dout_pclk_cam1_busperi_84",
	     ENABLE_ACLK_CAM11, 0, 0, 0),
	GATE(0, "hclk_ahb2apb_bridge_is3p", "dout_pclk_cam1_busperi_84",
	     ENABLE_ACLK_CAM11, 1, CLK_IGNORE_UNUSED, 0),
	GATE(HCLK_AHB2APB_BRIDGE_IS5P, "hclk_ahb2apb_bdirge_is5p",
	     "dout_pclk_cam1_busperi_84",
	     ENABLE_ACLK_CAM11, 2, 0, 0),
	GATE(HCLK_AHBSYNCDN_ISP_PERI, "hclk_ahbsyncdn_isp_peri",
	     "mout_aclk_cam1_busperi_334_user",
	     ENABLE_ACLK_CAM11, 3, 0, 0),
	GATE(0, "hclk_ahbsyncdn_isp2h", "mout_aclk_cam1_busperi_334_user",
	     ENABLE_ACLK_CAM11, 4, CLK_IGNORE_UNUSED, 0),
	GATE(0, "hclks_asyncahb_cam1", "mout_aclk_cam1_nocp_133_user",
	     ENABLE_ACLK_CAM11, 8, CLK_IGNORE_UNUSED, 0),
	GATE(0, "hclkm_asyncahb_cam1", "mout_aclk_cam1_busperi_334_user",
	     ENABLE_ACLK_CAM11, 9, CLK_IGNORE_UNUSED, 0),
	GATE(HCLK_ASYNCAHBSLAVE_TO_BLKC, "hclk_asyncahbslave_to_blck",
	     "dout_pclk_cam1_busperi_84",
	     ENABLE_ACLK_CAM11, 10, 0, 0),
	GATE(HCLK_ASYNCAHBSLAVE_TO_BLKD, "hclk_asyncahbslave_to_blkd",
	     "dout_pclk_cam1_busperi_84",
	     ENABLE_ACLK_CAM11, 11, 0, 0),
	GATE(CCLK_ASYNCAPB_SOCP_FIMC_BNS_C, "cclk_asyncapb_socp_fimc_bns_c",
	     "mout_aclk_cam1_bnscsis_133_user",
	     ENABLE_ACLK_CAM11, 12, CLK_IGNORE_UNUSED, 0),
	GATE(CCLK_ASYNCAPB_SOCP_FIMC_SCALER, "cclk_asyncapb_fimc_scaler",
	     "dout_pclk_cam1_sclvra_264",
	     ENABLE_ACLK_CAM11, 13, CLK_IGNORE_UNUSED, 0),
	GATE(CCLK_ASYNCAPB_SOCP_FIMC_VRA_S0, "cclk_asyncapb_socp_fimc_vra_s0",
	     "dout_pclk_cam1_sclvra_264",
	     ENABLE_ACLK_CAM11, 14, CLK_IGNORE_UNUSED, 0),
	GATE(CCLK_ASYNCAPB_SOCP_FIMC_VRA_S1, "cclk_asyncapb_socp_fimc_vra_s1",
	     "dout_pclk_cam1_sclvra_264",
	     ENABLE_ACLK_CAM11, 15, CLK_IGNORE_UNUSED, 0),
	GATE(0, "aclk_axi2apb_bridge_is3p", "mout_aclk_cam1_busperi_334_user",
	     ENABLE_ACLK_CAM11, 16, CLK_IGNORE_UNUSED, 0),
	GATE(0, "aclk_axispcx", "mout_aclk_cam1_busperi_334_user",
	     ENABLE_ACLK_CAM11, 17, CLK_IGNORE_UNUSED, 0),
	GATE(0, "aclk_axixphx", "mout_aclk_cam1_busperi_334_user",
	     ENABLE_ACLK_CAM11, 18, CLK_IGNORE_UNUSED, 0),
	GATE(0, "aclk_xiu_ispx_1x4", "mout_aclk_cam1_busperi_334_user",
	     ENABLE_ACLK_CAM11, 20, CLK_IGNORE_UNUSED, 0),
	GATE(ACLK_XIU_N_ASYNC_MI_CORTEX, "aclk_xiu_n_async_mi_cortex",
	     "mout_aclk_cam1_busperi_334_user",
	     ENABLE_ACLK_CAM11, 21, 0, 0),
	GATE(ACLK_XIU_N_ASYNC_MI_FROM_BLKD, "aclk_xiu_n_async_mi_from_blkd",
	     "mout_aclk_cam1_trex_532_user",
	     ENABLE_ACLK_CAM11, 22, CLK_IGNORE_UNUSED, 0),
	GATE(ACLK_XIU_N_ASYNC_SI_CAM1, "aclk_xiu_n_async_si_cam1",
	     "mout_aclk_cam1_busperi_334_user",
	     ENABLE_ACLK_CAM11, 23, 0, 0),
	GATE(ACLK_XIU_N_ASYNC_SI_CORTEX, "aclk_xiu_n_async_si_cortex",
	     "mout_aclk_cam1_arm_668_user",
	     ENABLE_ACLK_CAM11, 24, 0, 0),
	GATE(ACLK_XIU_N_ASYNC_SI_TO_BLKC, "aclk_xiu_n_async_si_to_blkc",
	     "mout_aclk_cam1_busperi_334_user",
	     ENABLE_ACLK_CAM11, 25, 0, 0),
	GATE(ACLK_XIU_N_ASYNC_SI_TO_BLKD, "aclk_xiu_n_async_si_to_blkd",
	     "mout_aclk_cam1_busperi_334_user",
	     ENABLE_ACLK_CAM11, 26, 0, 0),
	GATE(CLK_AXLH_ASYNC_SI_TOP_CAM1, "clk_axlh_async_si_top_cam1",
	     "mout_aclk_cam1_trex_532_user",
	     ENABLE_ACLK_CAM11, 28, CLK_IGNORE_UNUSED, 0),

	GATE(CLK_133_CAM1_NOC_P_CAM1, "clk_133_cam1_noc_p_cam1",
	     "mout_aclk_cam1_nocp_133_user",
	     ENABLE_ACLK_CAM12, 24, CLK_IGNORE_UNUSED, 0),

	GATE(0, "clk_fimc_scaler", "mout_aclk_cam1_sclvra_491_user",
	     ENABLE_ACLK_CAM13, 2, CLK_IGNORE_UNUSED, 0),
	GATE(0, "clk_fimc_vra", "mout_aclk_cam1_sclvra_491_user",
	     ENABLE_ACLK_CAM13, 3, CLK_IGNORE_UNUSED, 0),

	GATE(PCLK_FIMC_BNS_C, "pclk_fimc_bns_c",
	     "mout_aclk_cam1_bnscsis_133_user",
	     ENABLE_PCLK_CAM10, 0, CLK_IGNORE_UNUSED, 0),
	GATE(PCLK_FIMC_IS_B_GLUE, "pclk_fimc_is_b_glue",
	     "dout_pclk_cam1_busperi_167",
	     ENABLE_PCLK_CAM10, 1, 0, 0),
	GATE(PCLK_FIMC_SCALER, "pclk_fimc_scaler", "dout_pclk_cam1_sclvra_264",
	     ENABLE_PCLK_CAM10, 4, CLK_IGNORE_UNUSED, 0),
	GATE(PCLK_FIMC_VRA, "pclk_fimc_vra", "dout_pclk_cam1_sclvra_264",
	     ENABLE_PCLK_CAM10, 3, CLK_IGNORE_UNUSED, 0),
	GATE(PCLK_CSIS2, "pclk_csis2", "dout_pclk_cam1_busperi_167",
	     ENABLE_PCLK_CAM10, 6, 0, 0),
	GATE(PCLK_SYSMMU6_FIMC_BNS_C, "pclk_sysmmu6_fimc_bns_c",
	     "dout_pclk_cam1_busperi_167",
	     ENABLE_PCLK_CAM10, 8, 0, 0),
	GATE(PCLK_SYSMMU6_FIMC_ISP1, "pclk_sysmmu6_fimc_isp1",
	     "dout_pclk_cam1_busperi_167",
	     ENABLE_PCLK_CAM10, 9, 0, 0),
	GATE(PCLK_SYSMMU6_FIMC_ISPCX, "pclk_sysmmu6_fimc_ispcx",
	     "dout_pclk_cam1_busperi_167",
	     ENABLE_PCLK_CAM10, 10, 0, 0),
	GATE(PCLK_SYSMMU6_FIMC_SCALER, "pclk_sysmmu6_fimc_scaler",
	     "dout_pclk_cam1_busperi_167",
	     ENABLE_PCLK_CAM10, 11, 0, 0),
	GATE(PCLK_SYSMMU6_FIMC_VRA, "pclk_sysmmu6_fimc_vra",
	     "dout_pclk_cam1_busperi_167",
	     ENABLE_PCLK_CAM10, 12, 0, 0),
	GATE(PCLK_TREX_B, "pclk_trex_b", "dout_pclk_cam1_busperi_84",
	     ENABLE_PCLK_CAM10, 24, 0, 0),
	GATE(PCLKDBG_ASAPBMST_CSSYS_CAM1, "pclkdbg_asapbmst_cssys_cam1",
	     "dout_pclk_cam1_arm_167",
	     ENABLE_PCLK_CAM10, 29, 0, 0),

	GATE(PCLK_ASYNCAPB_SOCP_FIMC_BNS_C, "pclk_asyncapb_socp_fimc_bns_c",
	     "dout_pclk_cam1_busperi_167",
	     ENABLE_PCLK_CAM11, 12, 0, 0),
	GATE(PCLK_ASYNCAPB_SOCP_FIMC_SCALER, "pclk_asyncapb_socp_fimc_scaler",
	     "dout_pclk_cam1_busperi_167",
	     ENABLE_PCLK_CAM11, 13, 0, 0),
	GATE(PCLK_ASYNCAPB_SOCP_FIMC_VRA_S0, "pclk_asyncapb_socp_fimc_vra_s0",
	     "dout_pclk_cam1_busperi_167",
	     ENABLE_PCLK_CAM11, 14, 0, 0),
	GATE(PCLK_ASYNCAPB_SOCP_FIMC_VRA_S1, "pclk_asyncapb_socp_fimc_vra_s1",
	     "dout_pclk_cam1_busperi_167",
	     ENABLE_PCLK_CAM11, 15, 0, 0),
	GATE(PCLK_XIU_N_ASYNC_MI_CORTEX, "pclk_xiu_n_async_mi_cortex",
	     "dout_pclk_cam1_busperi_167",
	     ENABLE_PCLK_CAM11, 21, 0, 0),
	GATE(PCLK_XIU_N_ASYNC_MI_FROM_BLKD, "pclk_xiu_n_async_mi_from_blkd",
	     "dout_pclk_cam1_busperi_84",
	     ENABLE_PCLK_CAM11, 22, CLK_IGNORE_UNUSED, 0),

	GATE(0, "pclk_pmu_cam1", "dout_pclk_cam1_busperi_84",
	     ENABLE_PCLK_CAM12, 1, CLK_IGNORE_UNUSED, 0),
	GATE(PCLK_I2C0_ISP, "pclk_i2c0_isp", "dout_pclk_cam1_busperi_84",
	     ENABLE_PCLK_CAM12, 4, 0, 0),
	GATE(PCLK_I2C1_ISP, "pclk_i2c1_isp", "dout_pclk_cam1_busperi_84",
	     ENABLE_PCLK_CAM12, 5, 0, 0),
	GATE(PCLK_I2C2_ISP, "pclk_i2c2_isp", "dout_pclk_cam1_busperi_84",
	     ENABLE_PCLK_CAM12, 6, 0, 0),
	GATE(PCLK_I2C3_ISP, "pclk_i2c3_isp", "dout_pclk_cam1_busperi_84",
	     ENABLE_PCLK_CAM12, 7, 0, 0),
	GATE(PCLK_MCUCTL_ISP, "pclk_mcuctl_isp", "dout_pclk_cam1_busperi_84",
	     ENABLE_PCLK_CAM12, 8, 0, 0),
	GATE(PCLK_MPWM_ISP, "pclk_mpwm_isp", "dout_pclk_cam1_busperi_84",
	     ENABLE_PCLK_CAM12, 9, 0, 0),
	GATE(PCLK_MTCADC_ISP, "pclk_mtcadc_isp", "dout_pclk_cam1_busperi_84",
	     ENABLE_PCLK_CAM12, 10, 0, 0),
	GATE(PCLK_PWM_ISP, "pclk_pwm_isp", "dout_pclk_cam1_busperi_84",
	     ENABLE_PCLK_CAM12, 11, 0, 0),
	GATE(PCLK_SPI0_ISP, "pclk_spi0_isp", "dout_pclk_cam1_busperi_84",
	     ENABLE_PCLK_CAM12, 12, 0, 0),
	GATE(PCLK_SPI1_ISP, "pclk_spi1_isp", "dout_pclk_cam1_busperi_84",
	     ENABLE_PCLK_CAM12, 13, 0, 0),
	GATE(PCLK_UART_ISP, "pclk_uart_isp", "dout_pclk_cam1_busperi_84",
	     ENABLE_PCLK_CAM12, 14, 0, 0),
	GATE(PCLK_WDT_ISP, "pclk_wdt_isp", "dout_pclk_cam1_busperi_84",
	     ENABLE_PCLK_CAM12, 15, 0, 0),

	GATE(PHYCLK_RXBYTECLKHS0_S2B_USER, "phyclk_rxbyteclkhs0_s2b_user",
	     "mout_phyclk_hs0_csis2_rx_byte_user",
	     ENABLE_SCLK_CAM10, 6, 0, 0),

	GATE(SCLK_I2C0_ISP, "sclk_i2c0_isp", "fin_pll",
	     ENABLE_SCLK_CAM12, 4, 0, 0),
	GATE(SCLK_I2C1_ISP, "sclk_i2c1_isp", "fin_pll",
	     ENABLE_SCLK_CAM12, 5, 0, 0),
	GATE(SCLK_I2C2_ISP, "sclk_i2c2_isp", "fin_pll",
	     ENABLE_SCLK_CAM12, 6, 0, 0),
	GATE(SCLK_I2C3_ISP, "sclk_i2c3_isp", "fin_pll",
	     ENABLE_SCLK_CAM12, 7, 0, 0),
	GATE(SCLK_ISP_MTCADC_USER, "sclk_isp_mtcadc_user", "sclk_isp_mtcadc",
	     ENABLE_SCLK_CAM12, 10, 0, 0),
	GATE(SCLK_ISP_PWM, "sclk_isp_pwm", "fin_pll",
	     ENABLE_SCLK_CAM12, 11, 0, 0),
	GATE(SCLK_ISP_SPI0_USER, "sclk_isp_spi0_user",
	     "mout_sclk_isp_spi0_user",
	     ENABLE_SCLK_CAM12, 12, 0, 0),
	GATE(SCLK_ISP_SPI1_USER, "sclk_isp_spi1_user",
	     "mout_sclk_isp_spi1_user",
	     ENABLE_SCLK_CAM12, 13, 0, 0),
	GATE(SCLK_ISP_UART_USER, "sclk_usp_uart_user",
	     "mout_sclk_isp_uart_user",
	     ENABLE_SCLK_CAM12, 14, 0, 0),
};

static const struct samsung_cmu_info cam1_cmu_info __initconst = {
	.fixed_clks = fixed_rate_clks_cam1,
	.nr_fixed_clks = ARRAY_SIZE(fixed_rate_clks_cam1),
	.mux_clks = cam1_mux_clks,
	.nr_mux_clks = ARRAY_SIZE(cam1_mux_clks),
	.div_clks = cam1_div_clks,
	.nr_div_clks = ARRAY_SIZE(cam1_div_clks),
	.gate_clks = cam1_gate_clks,
	.nr_gate_clks = ARRAY_SIZE(cam1_gate_clks),
	.nr_clk_ids = CAM1_NR_CLK,
	.clk_regs = cam1_clk_regs,
	.nr_clk_regs = ARRAY_SIZE(cam1_clk_regs),
};

static void __init exynos7_clk_cam1_init(struct device_node *np)
{
	samsung_cmu_register_one(np, &cam1_cmu_info);
}

CLK_OF_DECLARE(exynos7_clk_cam1, "samsung,exynos7-clock-cam1",
	       exynos7_clk_cam1_init);

/***********************\
* CMU_ISP0 (0x146D0000) *
\***********************/

/* Register Offset definitions */
#define MUX_SEL_ISP0		0x0200
#define DIV_ISP0		0x0600
#define ENABLE_ACLK_ISP00	0x0800
#define ENABLE_ACLK_ISP01	0x0804
#define ENABLE_PCLK_ISP0	0x0900

static const unsigned long isp0_clk_regs[] __initconst = {
	MUX_SEL_ISP0,
	DIV_ISP0,
	ENABLE_ACLK_ISP00,
	ENABLE_ACLK_ISP01,
	ENABLE_PCLK_ISP0,
};

/* List of parent clocks for Muxes */
PNAME(mout_aclk_isp0_isp0_590_user_p) = {"fin_pll", "aclk_isp0_isp0_590"};
PNAME(mout_aclk_isp0_tpu_590_user_p)  = {"fin_pll", "aclk_isp0_tpu_590"};
PNAME(mout_aclk_isp0_trex_532_user_p) = {"fin_pll", "aclk_isp0_trex_532"};

/* Muxes */
static const struct samsung_mux_clock isp0_mux_clks[] __initconst = {
	MUX(0, "mout_aclk_isp0_isp0_590_user", mout_aclk_isp0_isp0_590_user_p,
	    MUX_SEL_ISP0, 0, 1),
	MUX(0, "mout_aclk_isp0_tpu_590_user", mout_aclk_isp0_tpu_590_user_p,
	    MUX_SEL_ISP0, 4, 1),
	MUX(0, "mout_aclk_isp0_trex_532_user", mout_aclk_isp0_trex_532_user_p,
	    MUX_SEL_ISP0, 8, 1),
};

/* Dividers */
static const struct samsung_div_clock isp0_div_clks[] __initconst = {
	DIV(0, "dout_pclk_isp0_isp0_295", "mout_aclk_isp0_isp0_590_user",
	    DIV_ISP0, 0, 3),
	DIV(0, "dout_pclk_isp0_tpu_295", "mout_aclk_isp0_tpu_590_user",
	    DIV_ISP0, 4, 3),
	DIV(0, "dout_pclk_isp0_trex_266", "mout_aclk_isp0_trex_532_user",
	    DIV_ISP0, 8, 3),
	DIV(0, "dout_pclk_isp0_trex_133", "mout_aclk_isp0_trex_532_user",
	    DIV_ISP0, 12, 3),
};

/* Clock gates */
static const struct samsung_gate_clock isp0_gate_clks[] __initconst = {
	GATE(ACLK_ISP_V4, "aclk_isp_v4", "mout_aclk_isp0_isp0_590_user",
	     ENABLE_ACLK_ISP00, 0, CLK_IGNORE_UNUSED, 0),
	GATE(ACLK_TPU_V1, "aclk_tpu_v1", "mout_aclk_isp0_tpu_590_user",
	     ENABLE_ACLK_ISP00, 1, CLK_IGNORE_UNUSED, 0),
	GATE(CLK_ISP0_TREX_C, "clk_isp0_trex_c", "mout_aclk_isp0_isp0_590_user",
	     ENABLE_ACLK_ISP00, 2, CLK_IGNORE_UNUSED, 0),
	GATE(CLK_TPU_TREX_C, "clk_tpu_trex_c", "mout_aclk_isp0_tpu_590_user",
	     ENABLE_ACLK_ISP00, 3, CLK_IGNORE_UNUSED, 0),
	GATE(CCLK_ASYNCAPB_ISP, "cclk_asyncapb_isp", "dout_pclk_isp0_isp0_295",
	     ENABLE_ACLK_ISP00, 4, CLK_IGNORE_UNUSED, 0),
	GATE(CCLK_ASYNCAPB_TPU, "cclk_asyncapb_tpu", "dout_pclk_isp0_tpu_295",
	     ENABLE_ACLK_ISP00, 5, CLK_IGNORE_UNUSED, 0),
	GATE(ACLK_SYSMMU600_ISP, "aclk_sysmmu600_isp",
	     "mout_aclk_isp0_isp0_590_user",
	     ENABLE_ACLK_ISP00, 6, CLK_IGNORE_UNUSED, 0),
	GATE(ACLK_SYSMMU600_TPU, "aclk_sysmmu600_tpu",
	     "mout_aclk_isp0_tpu_590_user",
	     ENABLE_ACLK_ISP00, 7, CLK_IGNORE_UNUSED, 0),
	GATE(CLK_C_TREX_C, "clk_c_trex_c", "mout_aclk_isp0_trex_532_user",
	     ENABLE_ACLK_ISP00, 8, CLK_IGNORE_UNUSED, 0),
	GATE(CLK_PXL_ASB_S_IN, "clk_pxl_asb_s_in",
	     "mout_aclk_isp0_isp0_590_user",
	     ENABLE_ACLK_ISP00, 12, CLK_IGNORE_UNUSED, 0),
	GATE(CLK_AXI_LH_ASYNC_SI_TOP_ISP0, "clk_axi_lh_async_si_top_isp0",
	     "mout_aclk_isp0_trex_532_user",
	     ENABLE_ACLK_ISP00, 13, CLK_IGNORE_UNUSED, 0),
	GATE(0, "aclk_axi2apb_bridge", "dout_pclk_isp0_trex_266",
	     ENABLE_ACLK_ISP00, 14, CLK_IGNORE_UNUSED, 0),
	GATE(0, "aclk_xiu_async_m", "dout_pclk_isp0_trex_266",
	     ENABLE_ACLK_ISP00, 15, CLK_IGNORE_UNUSED, 0),
	GATE(0, "hclk_ahb2apb_bridge", "dout_pclk_isp0_trex_133",
	     ENABLE_ACLK_ISP00, 16, CLK_IGNORE_UNUSED, 0),
	GATE(0, "hclk_ahb_async_m", "dout_pclk_isp0_trex_133",
	     ENABLE_ACLK_ISP00, 17, CLK_IGNORE_UNUSED, 0),

	GATE(CLK_ISP_V4, "clk_isp_v4", "mout_aclk_isp0_isp0_590_user",
	     ENABLE_ACLK_ISP01, 0, CLK_IGNORE_UNUSED, 0),
	GATE(CLK_TPU_V1, "clk_tpu_v1", "mout_aclk_isp0_tpu_590_user",
	     ENABLE_ACLK_ISP01, 1, CLK_IGNORE_UNUSED, 0),

	GATE(PCLK_ISP_V4, "pclk_isp_v4", "dout_pclk_isp0_isp0_295",
	     ENABLE_PCLK_ISP0, 0, CLK_IGNORE_UNUSED, 0),
	GATE(PCLK_TPU_V1, "pclk_tpu_v1", "dout_pclk_isp0_tpu_295",
	     ENABLE_PCLK_ISP0, 1, CLK_IGNORE_UNUSED, 0),
	GATE(PCLK_ASYNCAPB_ISP, "pclk_asyncapb_isp", "dout_pclk_isp0_trex_266",
	     ENABLE_PCLK_ISP0, 4, CLK_IGNORE_UNUSED, 0),
	GATE(PCLK_ASYNCAPB_TPU, "pclk_asyncapb_tpu", "dout_pclk_isp0_trex_266",
	     ENABLE_PCLK_ISP0, 5, CLK_IGNORE_UNUSED, 0),
	GATE(PCLK_SYSMMU600_ISP, "pclk_sysmmu600_isp",
	     "dout_pclk_isp0_trex_266",
	     ENABLE_PCLK_ISP0, 6, CLK_IGNORE_UNUSED, 0),
	GATE(PCLK_SYSMMU600_TPU, "pclk_sysmmu600_tpu",
	     "dout_pclk_isp0_trex_266",
	     ENABLE_PCLK_ISP0, 7, CLK_IGNORE_UNUSED, 0),
	GATE(PCLK_C_TREX_C, "plk_c_trex_c", "dout_pclk_isp0_trex_133",
	     ENABLE_PCLK_ISP0, 8, CLK_IGNORE_UNUSED, 0),
	GATE(0, "plk_pmu_isp0", "dout_pclk_isp0_trex_266",
	     ENABLE_PCLK_ISP0, 20, CLK_IGNORE_UNUSED, 0),

};

static const struct samsung_cmu_info isp0_cmu_info __initconst = {
	.mux_clks = isp0_mux_clks,
	.nr_mux_clks = ARRAY_SIZE(isp0_mux_clks),
	.div_clks = isp0_div_clks,
	.nr_div_clks = ARRAY_SIZE(isp0_div_clks),
	.gate_clks = isp0_gate_clks,
	.nr_gate_clks = ARRAY_SIZE(isp0_gate_clks),
	.nr_clk_ids = ISP0_NR_CLK,
	.clk_regs = isp0_clk_regs,
	.nr_clk_regs = ARRAY_SIZE(isp0_clk_regs),
};

static void __init exynos7_clk_isp0_init(struct device_node *np)
{
	samsung_cmu_register_one(np, &isp0_cmu_info);
}

CLK_OF_DECLARE(exynos7_clk_isp0, "samsung,exynos7-clock-isp0",
	       exynos7_clk_isp0_init);


/***********************\
* CMU_ISP1 (0x147D0000) *
\***********************/

/* Register Offset definitions */
#define MUX_SEL_ISP1		0x0200
#define DIV_ISP1		0x0600
#define ENABLE_ACLK_ISP1	0x0800
#define ENABLE_PCLK_ISP1	0x0900

static const unsigned long isp1_clk_regs[] __initconst = {
	MUX_SEL_ISP1,
	DIV_ISP1,
	ENABLE_ACLK_ISP1,
	ENABLE_PCLK_ISP1,
};

/* List of parent clocks for Muxes */
PNAME(mout_aclk_isp1_isp1_468_user_p) = {"fin_pll", "aclk_isp1_isp1_468"};
PNAME(mout_aclk_isp1_ahb_117_user_p)  = {"fin_pll", "aclk_isp1_ahb_117"};

/* Muxes */
static const struct samsung_mux_clock isp1_mux_clks[] __initconst = {
	MUX(0, "mout_aclk_isp1_isp1_468_user", mout_aclk_isp1_isp1_468_user_p,
	    MUX_SEL_ISP1, 0, 1),
	MUX(0, "mout_aclk_isp1_ahb_117_user", mout_aclk_isp1_ahb_117_user_p,
	    MUX_SEL_ISP1, 4, 1),
};

/* Dividers */
static const struct samsung_div_clock isp1_div_clks[] __initconst = {
	DIV(0, "dout_pclk_isp1_isp1_234", "mout_aclk_isp1_isp1_468_user",
	    DIV_ISP1, 0, 3),
};

/* Clock gates */
static const struct samsung_gate_clock isp1_gate_clks[] __initconst = {
	GATE(ACLK_FIMC_ISP1, "aclk_fimc_isp1", "mout_aclk_isp1_isp1_468_user",
	     ENABLE_ACLK_ISP1, 0, CLK_IGNORE_UNUSED, 0),
	GATE(CLK_FIMC_ISP1, "clk_fimc_isp1", "mout_aclk_isp1_isp1_468_user",
	     ENABLE_ACLK_ISP1, 1, CLK_IGNORE_UNUSED, 0),
	GATE(ACLK_PXL_ASBS, "aclk_pxl_asbs", "mout_aclk_isp1_isp1_468_user",
	     ENABLE_ACLK_ISP1, 4, CLK_IGNORE_UNUSED, 0),
	GATE(ACLK_XIU_N_ASYNC_SI, "aclk_xiu_n_async_si",
	     "mout_aclk_isp1_isp1_468_user",
	     ENABLE_ACLK_ISP1, 8, CLK_IGNORE_UNUSED, 0),
	GATE(0, "aclk_xiu_n_async_mi", "dout_pclk_isp1_isp1_234",
	     ENABLE_ACLK_ISP1, 9, CLK_IGNORE_UNUSED, 0),
	GATE(0, "aclk_axi2apb_bridge_is2p", "dout_pclk_isp1_isp1_234",
	     ENABLE_ACLK_ISP1, 10, CLK_IGNORE_UNUSED, 0),
	GATE(0, "hclk_ahb2apb_bridge_is2p", "mout_aclk_isp1_ahb_117_user",
	     ENABLE_ACLK_ISP1, 11, CLK_IGNORE_UNUSED, 0),
	GATE(0, "hclk_asynchbmaster", "mout_aclk_isp1_ahb_117_user",
	     ENABLE_ACLK_ISP1, 12, CLK_IGNORE_UNUSED, 0),

	GATE(PCLK_FIMC_ISP1, "pclk_fimc_isp1", "dout_pclk_isp1_isp1_234",
	     ENABLE_PCLK_ISP1, 0, CLK_IGNORE_UNUSED, 0),
	GATE(0, "pclk_pmu_isp1", "mout_aclk_isp1_ahb_117_user",
	     ENABLE_PCLK_ISP1, 16, CLK_IGNORE_UNUSED, 0),
};

static const struct samsung_cmu_info isp1_cmu_info __initconst = {
	.mux_clks = isp1_mux_clks,
	.nr_mux_clks = ARRAY_SIZE(isp1_mux_clks),
	.div_clks = isp1_div_clks,
	.nr_div_clks = ARRAY_SIZE(isp1_div_clks),
	.gate_clks = isp1_gate_clks,
	.nr_gate_clks = ARRAY_SIZE(isp1_gate_clks),
	.nr_clk_ids = ISP1_NR_CLK,
	.clk_regs = isp1_clk_regs,
	.nr_clk_regs = ARRAY_SIZE(isp1_clk_regs),
};

static void __init exynos7_clk_isp1_init(struct device_node *np)
{
	samsung_cmu_register_one(np, &isp1_cmu_info);
}

CLK_OF_DECLARE(exynos7_clk_isp1, "samsung,exynos7-clock-isp1",
	       exynos7_clk_isp1_init);

/**********************\
* CMU_VPP (0x13ED0000) *
\**********************/

/* Register Offset definitions */
#define MUX_SEL_VPP		0x0200
#define DIV_VPP			0x0600
#define ENABLE_ACLK_VPP		0x0800
#define ENABLE_PCLK_VPP		0x0900

static const unsigned long vpp_clk_regs[] __initconst = {
	MUX_SEL_VPP,
	DIV_VPP,
	ENABLE_ACLK_VPP,
	ENABLE_PCLK_VPP,
};

/* List of parent clocks for Muxes */
PNAME(mout_aclk_vpp0_400_user_p) = {"fin_pll", "aclk_vpp0_400"};
PNAME(mout_aclk_vpp1_400_user_p) = {"fin_pll", "aclk_vpp1_400"};

/* Muxes */
static const struct samsung_mux_clock vpp_mux_clks[] __initconst = {
	MUX(0, "mout_aclk_vpp0_400_user", mout_aclk_vpp0_400_user_p,
	    MUX_SEL_CCORE, 0, 1),
	MUX(0, "mout_aclk_vpp1_400_user", mout_aclk_vpp1_400_user_p,
	    MUX_SEL_CCORE, 4, 1),
};

/* Dividers */
static const struct samsung_div_clock vpp_div_clks[] __initconst = {
	DIV(0, "dout_pclk_vpp_133", "mout_aclk_vpp0_400_user",
	    DIV_CCORE, 0, 3),
};

/* Clock gates */
static const struct samsung_gate_clock vpp_gate_clks[] __initconst = {
	GATE(ACLK_VPP_IDMA_VG0, "aclk_vpp_idma_vg0", "mout_aclk_vpp0_400_user",
	     ENABLE_ACLK_VPP, 0, 0, 0),
	GATE(ACLK_VPP_IDMA_VG1, "aclk_vpp_idma_vg1", "mout_aclk_vpp1_400_user",
	     ENABLE_ACLK_VPP, 1, CLK_IGNORE_UNUSED, 0),
	GATE(ACLK_VPP_IDMA_VGR0,
	     "aclk_vpp_idma_vgr0", "mout_aclk_vpp0_400_user",
	     ENABLE_ACLK_VPP, 2, 0, 0),
	GATE(ACLK_VPP_IDMA_VGR1,
	     "aclk_vpp_idma_vgr1", "mout_aclk_vpp1_400_user",
	     ENABLE_ACLK_VPP, 3, CLK_IGNORE_UNUSED, 0),
	GATE(ACLK_SMMU_VPP_0, "aclk_smmu_vpp_0", "mout_aclk_vpp0_400_user",
	     ENABLE_ACLK_VPP, 4, 0, 0),
	GATE(ACLK_SMMU_VPP_SFW_0,
	     "aclk_smmu_vpp_sfw_0", "mout_aclk_vpp0_400_user",
	     ENABLE_ACLK_VPP, 5, 0, 0),
	GATE(ACLK_SMMU_VPP_1, "aclk_smmu_vpp_1", "mout_aclk_vpp1_400_user",
	     ENABLE_ACLK_VPP, 6, CLK_IGNORE_UNUSED, 0),
	GATE(ACLK_SMMU_VPP_SFW_1,
	     "aclk_smmu_vpp_sfw_1", "mout_aclk_vpp1_400_user",
	     ENABLE_ACLK_VPP, 7, CLK_IGNORE_UNUSED, 0),
	GATE(ACLK_BTS_IDMA_VG0, "aclk_bts_idma_vg0", "mout_aclk_vpp0_400_user",
	     ENABLE_ACLK_VPP, 10, 0, 0),
	GATE(ACLK_BTS_IDMA_VG1, "aclk_bts_idma_vg1", "mout_aclk_vpp1_400_user",
	     ENABLE_ACLK_VPP, 11, CLK_IGNORE_UNUSED, 0),
	GATE(ACLK_BTS_IDMA_VGR0,
	     "aclk_bts_idma_vgr0", "mout_aclk_vpp0_400_user",
	     ENABLE_ACLK_VPP, 12, 0, 0),
	GATE(ACLK_BTS_IDMA_VGR1,
	     "aclk_bts_idma_vgr1", "mout_aclk_vpp1_400_user",
	     ENABLE_ACLK_VPP, 13, CLK_IGNORE_UNUSED, 0),
	GATE(ACLK_AXI_LH_ASYNC_SI_VPP_0,
	     "aclk_axi_lh_async_si_vpp_0", "mout_aclk_vpp0_400_user",
	     ENABLE_ACLK_VPP, 14, 0, 0),
	GATE(ACLK_AXI_LH_ASYNC_SI_VPP_1,
	     "aclk_axi_lh_async_si_vpp_1", "mout_aclk_vpp1_400_user",
	     ENABLE_ACLK_VPP, 15, CLK_IGNORE_UNUSED, 0),
	GATE(ACLK_XIU_VPPX_0, "aclk_xiu_vppx_0", "mout_aclk_vpp0_400_user",
	     ENABLE_ACLK_VPP, 16, 0, 0),
	GATE(ACLK_XIU_VPPX_1, "aclk_xiu_vppx_1", "mout_aclk_vpp1_400_user",
	     ENABLE_ACLK_VPP, 17, CLK_IGNORE_UNUSED, 0),
	GATE(0, "aclk_vppnp_133", "dout_pclk_vpp_133",
	     ENABLE_ACLK_VPP, 18, CLK_IGNORE_UNUSED, 0),
	GATE(0, "aclk_ahb2apb_vpp", "dout_pclk_vpp_133",
	     ENABLE_ACLK_VPP, 19, CLK_IGNORE_UNUSED, 0),

	GATE(PCLK_SMMU_VPP_0, "pclk_smmu_vpp_0", "dout_pclk_vpp_133",
	     ENABLE_PCLK_VPP, 4, 0, 0),
	GATE(PCLK_SMMU_VPP_SFW_0, "pclk_smmu_vpp_sfw_0", "dout_pclk_vpp_133",
	     ENABLE_PCLK_VPP, 5, 0, 0),
	GATE(PCLK_SMMU_VPP_1, "pclk_smmu_vpp_1", "dout_pclk_vpp_133",
	     ENABLE_PCLK_VPP, 6, 0, 0),
	GATE(PCLK_SMMU_VPP_SFW_1, "pclk_smmu_vpp_sfw_1", "dout_pclk_vpp_133",
	     ENABLE_PCLK_VPP, 7, 0, 0),
	GATE(PCLK_BTS_IDMA_VG0, "pclk_bts_idma_vg0", "dout_pclk_vpp_133",
	     ENABLE_PCLK_VPP, 10, 0, 0),
	GATE(PCLK_BTS_IDMA_VG1, "pclk_bts_idma_vg1", "dout_pclk_vpp_133",
	     ENABLE_PCLK_VPP, 11, 0, 0),
	GATE(PCLK_BTS_IDMA_VGR0, "pclk_bts_idma_vgr0", "dout_pclk_vpp_133",
	     ENABLE_PCLK_VPP, 12, 0, 0),
	GATE(PCLK_BTS_IDMA_VGR1, "pclk_bts_idma_vgr1", "dout_pclk_vpp_133",
	     ENABLE_PCLK_VPP, 13, 0, 0),
	GATE(PCLK_VPP, "pclk_vpp", "dout_pclk_vpp_133",
	     ENABLE_PCLK_VPP, 20, 0, 0),
	GATE(0, "pclk_pmu_vpp", "dout_pclk_vpp_133",
	     ENABLE_PCLK_VPP, 21, CLK_IGNORE_UNUSED, 0),

};

static const struct samsung_cmu_info vpp_cmu_info __initconst = {
	.mux_clks = vpp_mux_clks,
	.nr_mux_clks = ARRAY_SIZE(vpp_mux_clks),
	.div_clks = vpp_div_clks,
	.nr_div_clks = ARRAY_SIZE(vpp_div_clks),
	.gate_clks = vpp_gate_clks,
	.nr_gate_clks = ARRAY_SIZE(vpp_gate_clks),
	.nr_clk_ids = VPP_NR_CLK,
	.clk_regs = vpp_clk_regs,
	.nr_clk_regs = ARRAY_SIZE(vpp_clk_regs),
};

static void __init exynos7_clk_vpp_init(struct device_node *np)
{
	samsung_cmu_register_one(np, &vpp_cmu_info);
}

CLK_OF_DECLARE(exynos7_clk_vpp, "samsung,exynos7-clock-vpp",
	       exynos7_clk_vpp_init);


/*************************\
* CMU_PERIC0 (0x13610000) *
\*************************/

/* Register Offset definitions */
#define MUX_SEL_PERIC0			0x0200
#define ENABLE_ACLK_PERIC0		0x0800
#define ENABLE_PCLK_PERIC0		0x0900
#define ENABLE_SCLK_PERIC0		0x0A00

static const unsigned long peric0_clk_regs[] __initconst = {
	MUX_SEL_PERIC0,
	ENABLE_ACLK_PERIC0,
	ENABLE_PCLK_PERIC0,
	ENABLE_SCLK_PERIC0,
};

/* List of parent clocks for Muxes */
PNAME(mout_aclk_peric0_66_user_p) = {"fin_pll", "aclk_peric0_66"};
PNAME(mout_sclk_uart0_user_p) 	  = {"fin_pll", "sclk_uart0"};

/* Muxes */
static const struct samsung_mux_clock peric0_mux_clks[] __initconst = {
	MUX(0, "mout_aclk_peric0_66_user", mout_aclk_peric0_66_user_p,
	    MUX_SEL_PERIC0, 0, 1),
	MUX(0, "mout_sclk_uart0_user", mout_sclk_uart0_user_p,
	    MUX_SEL_PERIC0, 16, 1),
};

/* Clock gates */
static const struct samsung_gate_clock peric0_gate_clks[] __initconst = {
	GATE(0, "aclk_noc_p_peric0", "mout_aclk_peric0_66_user",
	     ENABLE_ACLK_PERIC0, 0, CLK_IGNORE_UNUSED, 0),
	GATE(0, "aclk_ahb2apb_peric0p_0", "mout_aclk_peric0_66_user",
	     ENABLE_ACLK_PERIC0, 1, CLK_IGNORE_UNUSED, 0),

	GATE(0, "pclk_pmu_peric0", "mout_aclk_peric0_66_user",
	     ENABLE_PCLK_PERIC0, 4, CLK_IGNORE_UNUSED, 0),
	GATE(PCLK_HSI2C0, "pclk_hsi2c0", "mout_aclk_peric0_66_user",
	     ENABLE_PCLK_PERIC0, 8, 0, 0),
	GATE(PCLK_HSI2C1, "pclk_hsi2c1", "mout_aclk_peric0_66_user",
	     ENABLE_PCLK_PERIC0, 9, 0, 0),
	GATE(PCLK_HSI2C4, "pclk_hsi2c4", "mout_aclk_peric0_66_user",
	     ENABLE_PCLK_PERIC0, 10, 0, 0),
	GATE(PCLK_HSI2C5, "pclk_hsi2c5", "mout_aclk_peric0_66_user",
	     ENABLE_PCLK_PERIC0, 11, 0, 0),
	GATE(PCLK_HSI2C9, "pclk_hsi2c9", "mout_aclk_peric0_66_user",
	     ENABLE_PCLK_PERIC0, 12, 0, 0),
	GATE(PCLK_HSI2C10, "pclk_hsi2c10", "mout_aclk_peric0_66_user",
	     ENABLE_PCLK_PERIC0, 13, 0, 0),
	GATE(PCLK_HSI2C11, "pclk_hsi2c11", "mout_aclk_peric0_66_user",
	     ENABLE_PCLK_PERIC0, 14, 0, 0),
	GATE(PCLK_UART0, "pclk_uart0", "mout_aclk_peric0_66_user",
	     ENABLE_PCLK_PERIC0, 16, 0, 0),
	GATE(PCLK_ADCIF, "pclk_adcif", "mout_aclk_peric0_66_user",
	     ENABLE_PCLK_PERIC0, 20, 0, 0),
	GATE(PCLK_PWM, "pclk_pwm", "mout_aclk_peric0_66_user",
	     ENABLE_PCLK_PERIC0, 21, 0, 0),

	GATE(SCLK_UART0_USER, "sclk_uart0_user", "mout_sclk_uart0_user",
	     ENABLE_SCLK_PERIC0, 16, 0, 0),
	GATE(SCLK_PWM, "sclk_pwm", "fin_pll",
	     ENABLE_SCLK_PERIC0, 21, 0, 0),
};

static const struct samsung_cmu_info peric0_cmu_info __initconst = {
	.mux_clks = peric0_mux_clks,
	.nr_mux_clks = ARRAY_SIZE(peric0_mux_clks),
	.gate_clks = peric0_gate_clks,
	.nr_gate_clks = ARRAY_SIZE(peric0_gate_clks),
	.nr_clk_ids = PERIC0_NR_CLK,
	.clk_regs = peric0_clk_regs,
	.nr_clk_regs = ARRAY_SIZE(peric0_clk_regs),
};

static void __init exynos7_clk_peric0_init(struct device_node *np)
{
	samsung_cmu_register_one(np, &peric0_cmu_info);
}

CLK_OF_DECLARE(exynos7_clk_peric0, "samsung,exynos7-clock-peric0",
	       exynos7_clk_peric0_init);


/*************************\
* CMU_PERIC1 (0x14C80000) *
\*************************/

/* Register Offset definitions */
#define MUX_SEL_PERIC10			0x0200
#define MUX_SEL_PERIC11			0x0204
#define MUX_SEL_PERIC12			0x0208
#define ENABLE_ACLK_PERIC1		0x0800
#define ENABLE_PCLK_PERIC1		0x0900
#define ENABLE_SCLK_PERIC10		0x0A00
#define ENABLE_SCLK_PERIC11		0x0A04

static const unsigned long peric1_clk_regs[] __initconst = {
	MUX_SEL_PERIC10,
	MUX_SEL_PERIC11,
	MUX_SEL_PERIC12,
	ENABLE_ACLK_PERIC1,
	ENABLE_PCLK_PERIC1,
	ENABLE_SCLK_PERIC10,
};

/* List of parent clocks for Muxes in CMU_PERIC1 */
PNAME(mout_aclk_peric1_66_user_p) = {"fin_pll", "aclk_peric1_66"};
PNAME(mout_sclk_uart1_user_p) = {"fin_pll", "sclk_uart1"};
PNAME(mout_sclk_uart2_user_p) = {"fin_pll", "sclk_uart2"};
PNAME(mout_sclk_uart3_user_p) = {"fin_pll", "sclk_uart3"};
PNAME(mout_sclk_spi0_user_p) = {"fin_pll", "sclk_spi0"};
PNAME(mout_sclk_spi1_user_p) = {"fin_pll", "sclk_spi1"};
PNAME(mout_sclk_spi2_user_p) = {"fin_pll", "sclk_spi2"};
PNAME(mout_sclk_spi3_user_p) = {"fin_pll", "sclk_spi3"};
PNAME(mout_sclk_spi4_user_p) = {"fin_pll", "sclk_spi4"};
PNAME(mout_sclk_spi5_user_p) = {"fin_pll", "sclk_spi5"};

/* Muxes */
static const struct samsung_mux_clock peric1_mux_clks[] __initconst = {
	MUX(0, "mout_aclk_peric1_66_user", mout_aclk_peric1_66_user_p,
	    MUX_SEL_PERIC10, 0, 1),

	MUX_F(0, "mout_sclk_spi0_user", mout_sclk_spi0_user_p,
	      MUX_SEL_PERIC11, 0, 1, CLK_SET_RATE_PARENT, 0),
	MUX_F(0, "mout_sclk_spi1_user", mout_sclk_spi1_user_p,
	      MUX_SEL_PERIC11, 4, 1, CLK_SET_RATE_PARENT, 0),
	MUX_F(0, "mout_sclk_spi2_user", mout_sclk_spi2_user_p,
	      MUX_SEL_PERIC11, 8, 1, CLK_SET_RATE_PARENT, 0),
	MUX_F(0, "mout_sclk_spi3_user", mout_sclk_spi3_user_p,
	      MUX_SEL_PERIC11, 12, 1, CLK_SET_RATE_PARENT, 0),
	MUX_F(0, "mout_sclk_spi4_user", mout_sclk_spi4_user_p,
	      MUX_SEL_PERIC11, 16, 1, CLK_SET_RATE_PARENT, 0),
	MUX(0, "mout_sclk_uart1_user", mout_sclk_uart1_user_p,
	    MUX_SEL_PERIC11, 20, 1),
	MUX(0, "mout_sclk_uart2_user", mout_sclk_uart2_user_p,
	    MUX_SEL_PERIC11, 24, 1),
	MUX(0, "mout_sclk_uart3_user", mout_sclk_uart3_user_p,
	    MUX_SEL_PERIC11, 28, 1),

	MUX_F(0, "mout_sclk_spi5_user", mout_sclk_spi5_user_p,
	      MUX_SEL_PERIC12, 0, 1, CLK_SET_RATE_PARENT, 0),
};

/* Clock gates */
static const struct samsung_gate_clock peric1_gate_clks[] __initconst = {
	GATE(0, "aclk_noc_p_peric1", "mout_aclk_peric1_66_user",
	     ENABLE_ACLK_PERIC1, 0, CLK_IGNORE_UNUSED, 0),
	GATE(0, "aclk_ahb2apb_peric0p", "mout_aclk_peric1_66_user",
	     ENABLE_ACLK_PERIC1, 1, CLK_IGNORE_UNUSED, 0),
	GATE(ACLK_AHB2APB_PERIC1P, "aclk_ahb2apb_peric1p",
	     "mout_aclk_peric1_66_user",
	     ENABLE_ACLK_PERIC1, 2, 0, 0),
	GATE(ACLK_AHB2APB_PERIC2P, "aclk_ahb2apb_peric2p",
	     "mout_aclk_peric1_66_user",
	     ENABLE_ACLK_PERIC1, 3, 0, 0),

	GATE(PCLK_HSI2C2, "pclk_hsi2c2", "mout_aclk_peric1_66_user",
	     ENABLE_PCLK_PERIC1, 4, 0, 0),
	GATE(PCLK_HSI2C3, "pclk_hsi2c3", "mout_aclk_peric1_66_user",
	     ENABLE_PCLK_PERIC1, 5, 0, 0),
	GATE(PCLK_HSI2C6, "pclk_hsi2c6", "mout_aclk_peric1_66_user",
	     ENABLE_PCLK_PERIC1, 6, 0, 0),
	GATE(PCLK_HSI2C7, "pclk_hsi2c7", "mout_aclk_peric1_66_user",
	     ENABLE_PCLK_PERIC1, 7, 0, 0),
	GATE(PCLK_HSI2C8, "pclk_hsi2c8", "mout_aclk_peric1_66_user",
	     ENABLE_PCLK_PERIC1, 8, 0, 0),
	GATE(PCLK_UART1, "pclk_uart1", "mout_aclk_peric1_66_user",
	     ENABLE_PCLK_PERIC1, 9, 0, 0),
	GATE(PCLK_UART2, "pclk_uart2", "mout_aclk_peric1_66_user",
	     ENABLE_PCLK_PERIC1, 10, 0, 0),
	GATE(PCLK_UART3, "pclk_uart3", "mout_aclk_peric1_66_user",
	     ENABLE_PCLK_PERIC1, 11, 0, 0),
	GATE(PCLK_SPI0, "pclk_spi0", "mout_aclk_peric1_66_user",
	     ENABLE_PCLK_PERIC1, 12, 0, 0),
	GATE(PCLK_SPI1, "pclk_spi1", "mout_aclk_peric1_66_user",
	     ENABLE_PCLK_PERIC1, 13, 0, 0),
	GATE(PCLK_SPI2, "pclk_spi2", "mout_aclk_peric1_66_user",
	     ENABLE_PCLK_PERIC1, 14, 0, 0),
	GATE(PCLK_SPI3, "pclk_spi3", "mout_aclk_peric1_66_user",
	     ENABLE_PCLK_PERIC1, 15, 0, 0),
	GATE(PCLK_SPI4, "pclk_spi4", "mout_aclk_peric1_66_user",
	     ENABLE_PCLK_PERIC1, 16, 0, 0),
	GATE(PCLK_I2S1, "pclk_i2s1", "mout_aclk_peric1_66_user",
	     ENABLE_PCLK_PERIC1, 17, CLK_SET_RATE_PARENT, 0),
	GATE(PCLK_PCM1, "pclk_pcm1", "mout_aclk_peric1_66_user",
	     ENABLE_PCLK_PERIC1, 18, 0, 0),
	GATE(PCLK_SPDIF, "pclk_spdif", "mout_aclk_peric1_66_user",
	     ENABLE_PCLK_PERIC1, 19, 0, 0),
	GATE(PCLK_SPI5, "pclk_spdi5", "mout_aclk_peric1_66_user",
	     ENABLE_PCLK_PERIC1, 20, 0, 0),
	GATE(PCLK_GPIO_NFC, "pclk_gpio_nfc", "mout_aclk_peric1_66_user",
	     ENABLE_PCLK_PERIC1, 24, 0, 0),
	GATE(PCLK_GPIO_TOUCH, "pclk_gpio_touch", "mout_aclk_peric1_66_user",
	     ENABLE_PCLK_PERIC1, 25, 0, 0),
	GATE(PCLK_GPIO_FF, "pclk_gpio_ff", "mout_aclk_peric1_66_user",
	     ENABLE_PCLK_PERIC1, 26, 0, 0),
	GATE(PCLK_GPIO_ESE, "pclk_gpio_ese", "mout_aclk_peric1_66_user",
	     ENABLE_PCLK_PERIC1, 27, 0, 0),
	GATE(0, "pclk_pmu_peric1", "mout_aclk_peric1_66_user",
	     ENABLE_PCLK_PERIC1, 28, CLK_IGNORE_UNUSED, 0),

	GATE(SCLK_UART1_USER, "sclk_uart1_user", "mout_sclk_uart1_user",
	     ENABLE_SCLK_PERIC10, 9, 0, 0),
	GATE(SCLK_UART2_USER, "sclk_uart2_user", "mout_sclk_uart2_user",
	     ENABLE_SCLK_PERIC10, 10, 0, 0),
	GATE(SCLK_UART3_USER, "sclk_uart3_user", "mout_sclk_uart3_user",
	     ENABLE_SCLK_PERIC10, 11, 0, 0),
	GATE(SCLK_SPI0_USER, "sclk_spi0_user", "mout_sclk_spi0_user",
	     ENABLE_SCLK_PERIC10, 12, CLK_SET_RATE_PARENT, 0),
	GATE(SCLK_SPI1_USER, "sclk_spi1_user", "mout_sclk_spi1_user",
	     ENABLE_SCLK_PERIC10, 13, CLK_SET_RATE_PARENT, 0),
	GATE(SCLK_SPI2_USER, "sclk_spi2_user", "mout_sclk_spi2_user",
	     ENABLE_SCLK_PERIC10, 14, CLK_SET_RATE_PARENT, 0),
	GATE(SCLK_SPI3_USER, "sclk_spi3_user", "mout_sclk_spi3_user",
	     ENABLE_SCLK_PERIC10, 15, CLK_SET_RATE_PARENT, 0),
	GATE(SCLK_SPI4_USER, "sclk_spi4_user", "mout_sclk_spi4_user",
	     ENABLE_SCLK_PERIC10, 16, CLK_SET_RATE_PARENT, 0),
	GATE(SCLK_I2S1_USER, "sclk_i2s1_user", "sclk_i2s1",
	     ENABLE_SCLK_PERIC10, 17, CLK_SET_RATE_PARENT, 0),
	GATE(SCLK_PCM1_USER, "sclk_pcm1_user", "sclk_pcm1",
	     ENABLE_SCLK_PERIC10, 18, CLK_SET_RATE_PARENT, 0),
	GATE(SCLK_SPDIF_USER, "sclk_spdif_user", "sclk_spdif",
	     ENABLE_SCLK_PERIC10, 19, CLK_SET_RATE_PARENT, 0),
	GATE(SCLK_SPI5_USER, "sclk_spi5_user", "mout_sclk_spi5_user",
	     ENABLE_SCLK_PERIC10, 20, CLK_SET_RATE_PARENT, 0),

	GATE(IOCLK_SPI0_USER, "ioclk_spi0_user", "ioclk_spi0",
	     ENABLE_SCLK_PERIC11, 12, 0, 0),
	GATE(IOCLK_SPI1_USER, "ioclk_spi1_user", "ioclk_spi1",
	     ENABLE_SCLK_PERIC11, 13, 0, 0),
	GATE(IOCLK_SPI2_USER, "ioclk_spi2_user", "ioclk_spi2",
	     ENABLE_SCLK_PERIC11, 14, 0, 0),
	GATE(IOCLK_SPI3_USER, "ioclk_spi3_user", "ioclk_spi3",
	     ENABLE_SCLK_PERIC11, 15, 0, 0),
	GATE(IOCLK_SPI4_USER, "ioclk_spi4_user", "ioclk_spi4",
	     ENABLE_SCLK_PERIC11, 16, 0, 0),
	GATE(IOCLK_I2S1_USER, "ioclk_i2s1_bclk_user", "ioclk_i2s1",
	     ENABLE_SCLK_PERIC11, 17, 0, 0),
	GATE(IOCLK_SPI5_USER, "ioclk_spi5_user", "ioclk_spi5",
	     ENABLE_SCLK_PERIC11, 20, 0, 0),
};

static const struct samsung_cmu_info peric1_cmu_info __initconst = {
	.mux_clks = peric1_mux_clks,
	.nr_mux_clks = ARRAY_SIZE(peric1_mux_clks),
	.gate_clks = peric1_gate_clks,
	.nr_gate_clks = ARRAY_SIZE(peric1_gate_clks),
	.nr_clk_ids = PERIC1_NR_CLK,
	.clk_regs = peric1_clk_regs,
	.nr_clk_regs = ARRAY_SIZE(peric1_clk_regs),
};

static void __init exynos7_clk_peric1_init(struct device_node *np)
{
	samsung_cmu_register_one(np, &peric1_cmu_info);
}

CLK_OF_DECLARE(exynos7_clk_peric1, "samsung,exynos7-clock-peric1",
	       exynos7_clk_peric1_init);


/************************\
* CMU_PERIS (0x10040000) *
\************************/

/* Register Offset definitions */
#define MUX_SEL_PERIS			0x0200
#define ENABLE_ACLK_PERIS		0x0800
#define ENABLE_PCLK_PERIS		0x0900
#define ENABLE_PCLK_PERIS_SECURE_TZPC	0x0904
#define ENABLE_PCLK_PERIS_SECURE_OTP	0x0908
#define ENABLE_PCLK_PERIS_SECURE_RTC	0x090C
#define ENABLE_PCLK_PERIS_SECURE_CHIPID	0x0910
#define ENABLE_SCLK_PERIS		0x0A00
#define ENABLE_SCLK_PERIS_SECURE_OTP	0x0A08
#define ENABLE_SCLK_PERIS_SECURE_RTC	0x0A08
#define ENABLE_SCLK_PERIS_SECURE_CHIPID	0x0A10

static const unsigned long peris_clk_regs[] __initconst = {
	MUX_SEL_PERIS,
	ENABLE_PCLK_PERIS,
	ENABLE_PCLK_PERIS_SECURE_TZPC,
	ENABLE_PCLK_PERIS_SECURE_OTP,
	ENABLE_PCLK_PERIS_SECURE_RTC,
	ENABLE_PCLK_PERIS_SECURE_CHIPID,
	ENABLE_SCLK_PERIS,
	ENABLE_SCLK_PERIS_SECURE_OTP,
	ENABLE_SCLK_PERIS_SECURE_RTC,
	ENABLE_SCLK_PERIS_SECURE_CHIPID,
};

/* List of parent clocks for Muxes */
PNAME(mout_aclk_peris_66_user_p) = {"fin_pll", "aclk_peris_66"};

/* Muxes */
static const struct samsung_mux_clock peris_mux_clks[] __initconst = {
	MUX(0, "mout_aclk_peris_66_user",
	    mout_aclk_peris_66_user_p, MUX_SEL_PERIS, 0, 1),
};

/* Clock gates */
static const struct samsung_gate_clock peris_gate_clks[] __initconst = {
	GATE(0, "aclk_perisnp_66", "mout_aclk_peris_66_user",
	     ENABLE_ACLK_PERIS, 0, CLK_IGNORE_UNUSED, 0),
	GATE(0, "aclk_ahb2apb_peris0p", "mout_aclk_peris_66_user",
	     ENABLE_ACLK_PERIS, 1, CLK_IGNORE_UNUSED, 0),
	GATE(ACLK_AHB2APB_PERIS1P, "aclk_ahb2apb_peris1p",
	     "mout_aclk_peris_66_user",
	     ENABLE_ACLK_PERIS, 2, CLK_IGNORE_UNUSED, 0),

	GATE(0, "pclk_pmu_peris", "mout_aclk_peris_66_user",
	     ENABLE_PCLK_PERIS, 3, CLK_IGNORE_UNUSED, 0),
	GATE(PCLK_HDMI_CEC, "pclk_hdmi_cec", "mout_aclk_peris_66_user",
	     ENABLE_PCLK_PERIS, 4, 0, 0),
	GATE(PCLK_MCT, "pclk_mct", "mout_aclk_peris_66_user",
	     ENABLE_PCLK_PERIS, 5, 0, 0),
	GATE(PCLK_WDT_ATLAS, "pclk_wdt_atlas", "mout_aclk_peris_66_user",
	     ENABLE_PCLK_PERIS, 6, 0, 0),
	GATE(PCLK_WDT_APOLLO, "pclk_wdt_apollo", "mout_aclk_peris_66_user",
	     ENABLE_PCLK_PERIS, 7, 0, 0),
	GATE(PCLK_SYSREG, "pclk_sysreg", "mout_aclk_peris_66_user",
	     ENABLE_PCLK_PERIS, 9, 0, 0),
	GATE(PCLK_TMU, "pclk_tmu_apbif", "mout_aclk_peris_66_user",
	     ENABLE_PCLK_PERIS, 10, 0, 0),

	GATE(PCLK_TZPC0, "pclk_tzpc0", "mout_aclk_peris_66_user",
	     ENABLE_PCLK_PERIS_SECURE_TZPC, 0, 0, 0),
	GATE(PCLK_TZPC1, "pclk_tzpc1", "mout_aclk_peris_66_user",
	     ENABLE_PCLK_PERIS_SECURE_TZPC, 1, 0, 0),
	GATE(PCLK_TZPC2, "pclk_tzpc2", "mout_aclk_peris_66_user",
	     ENABLE_PCLK_PERIS_SECURE_TZPC, 2, 0, 0),
	GATE(PCLK_TZPC3, "pclk_tzpc3", "mout_aclk_peris_66_user",
	     ENABLE_PCLK_PERIS_SECURE_TZPC, 3, 0, 0),
	GATE(PCLK_TZPC4, "pclk_tzpc4", "mout_aclk_peris_66_user",
	     ENABLE_PCLK_PERIS_SECURE_TZPC, 4, 0, 0),
	GATE(PCLK_TZPC5, "pclk_tzpc5", "mout_aclk_peris_66_user",
	     ENABLE_PCLK_PERIS_SECURE_TZPC, 5, 0, 0),
	GATE(PCLK_TZPC6, "pclk_tzpc6", "mout_aclk_peris_66_user",
	     ENABLE_PCLK_PERIS_SECURE_TZPC, 6, 0, 0),
	GATE(PCLK_TZPC7, "pclk_tzpc7", "mout_aclk_peris_66_user",
	     ENABLE_PCLK_PERIS_SECURE_TZPC, 7, 0, 0),
	GATE(PCLK_TZPC8, "pclk_tzpc8", "mout_aclk_peris_66_user",
	     ENABLE_PCLK_PERIS_SECURE_TZPC, 8, 0, 0),
	GATE(PCLK_TZPC9, "pclk_tzpc9", "mout_aclk_peris_66_user",
	     ENABLE_PCLK_PERIS_SECURE_TZPC, 9, 0, 0),
	GATE(PCLK_TZPC10, "pclk_tzpc10", "mout_aclk_peris_66_user",
	     ENABLE_PCLK_PERIS_SECURE_TZPC, 10, 0, 0),
	GATE(PCLK_TZPC11, "pclk_tzpc11", "mout_aclk_peris_66_user",
	     ENABLE_PCLK_PERIS_SECURE_TZPC, 11, 0, 0),
	GATE(PCLK_TZPC12, "pclk_tzpc12", "mout_aclk_peris_66_user",
	     ENABLE_PCLK_PERIS_SECURE_TZPC, 12, 0, 0),
	GATE(PCLK_TZPC13, "pclk_tzpc13", "mout_aclk_peris_66_user",
	     ENABLE_PCLK_PERIS_SECURE_TZPC, 13, 0, 0),
	GATE(PCLK_TZPC14, "pclk_tzpc14", "mout_aclk_peris_66_user",
	     ENABLE_PCLK_PERIS_SECURE_TZPC, 14, 0, 0),
	GATE(PCLK_TZPC15, "pclk_tzpc15", "mout_aclk_peris_66_user",
	     ENABLE_PCLK_PERIS_SECURE_TZPC, 15, 0, 0),

	GATE(PCLK_OTP_CON_TOP, "pclk_otp_con_top", "mout_aclk_peris_66_user",
	     ENABLE_PCLK_PERIS_SECURE_OTP, 0, 0, 0),
	GATE(SCLK_OTP_CON_TOP, "sclk_otp_con_top", "fin_pll",
	     ENABLE_SCLK_PERIS_SECURE_OTP, 0, 0, 0),

	GATE(PCLK_TOPRTC, "pclk_toprtc", "mout_aclk_peris_66_user",
	     ENABLE_PCLK_PERIS_SECURE_RTC, 0, 0, 0),
	GATE(SCLK_TOPRTC, "sclk_toprtc", "rtc_clk",
	     ENABLE_SCLK_PERIS_SECURE_RTC, 0, 0, 0),

	GATE(PCLK_CHIPID, "pclk_chipid", "mout_aclk_peris_66_user",
	     ENABLE_PCLK_PERIS_SECURE_CHIPID, 0, 0, 0),
	GATE(SCLK_CHIPID, "sclk_chipid", "fin_pll",
	     ENABLE_SCLK_PERIS_SECURE_CHIPID, 0, 0, 0),

	GATE(SCLK_TMU, "sclk_tmu", "fin_pll", ENABLE_SCLK_PERIS, 10, 0, 0),
};

static const struct samsung_cmu_info peris_cmu_info __initconst = {
	.mux_clks = peris_mux_clks,
	.nr_mux_clks = ARRAY_SIZE(peris_mux_clks),
	.gate_clks = peris_gate_clks,
	.nr_gate_clks = ARRAY_SIZE(peris_gate_clks),
	.nr_clk_ids = PERIS_NR_CLK,
	.clk_regs = peris_clk_regs,
	.nr_clk_regs = ARRAY_SIZE(peris_clk_regs),
};

static void __init exynos7_clk_peris_init(struct device_node *np)
{
	samsung_cmu_register_one(np, &peris_cmu_info);
}

CLK_OF_DECLARE(exynos7_clk_peris, "samsung,exynos7-clock-peris",
	       exynos7_clk_peris_init);

/***********************\
* CMU_DISP (0x13AD0000) *
\***********************/

/* Register Offset definitions */
#define DISP_PLL_LOCK		0x0000
#define DPHY_PLL_LOCK		0x0004
#define DISP_PLL_CON0		0x0100
#define DPHY_PLL_CON0		0x0104
#define MUX_SEL_DISP0		0x0200
#define MUX_SEL_DISP1		0x0204
#define MUX_SEL_DISP2		0x0208
#define MUX_SEL_DISP3		0x020C
#define MUX_SEL_DISP4		0x0210
#define MUX_SEL_DISP5		0x0214
#define DIV_DISP		0x0600
#define ENABLE_ACLK_DISP	0x0800
#define ENABLE_ACLK_DISP_SECURE_SMMU_RO_MMU	0x0804
#define ENABLE_ACLK_DISP_SECURE_SMMU_RW_MMU	0x0808
#define ENABLE_ACLK_DISP_SECURE_SMMU_RO_SFW	0x080C
#define ENABLE_ACLK_DISP_SECURE_SMMU_RW_SFW	0x0810
#define ENABLE_PCLK_DISP	0x0900
#define ENABLE_PCLK_DISP_SECURE_SMMU_RO_MMU	0x0904
#define ENABLE_PCLK_DISP_SECURE_SMMU_RW_MMU	0x0908
#define ENABLE_PCLK_DISP_SECURE_SMMU_RO_SFW	0x090C
#define ENABLE_PCLK_DISP_SECURE_SMMU_RW_SFW	0x0910
#define ENABLE_SCLK_DISP1	0x0A04
#define ENABLE_SCLK_DISP2	0x0A08
#define ENABLE_SCLK_DISP4	0x0A10

static const unsigned long disp_clk_regs[] __initconst = {
	DISP_PLL_LOCK,
	DPHY_PLL_LOCK,
	DISP_PLL_CON0,
	DPHY_PLL_CON0,
	MUX_SEL_DISP0,
	MUX_SEL_DISP1,
	MUX_SEL_DISP2,
	MUX_SEL_DISP3,
	MUX_SEL_DISP4,
	MUX_SEL_DISP5,
	DIV_DISP,
	ENABLE_ACLK_DISP,
	ENABLE_ACLK_DISP_SECURE_SMMU_RO_MMU,
	ENABLE_ACLK_DISP_SECURE_SMMU_RW_MMU,
	ENABLE_ACLK_DISP_SECURE_SMMU_RO_SFW,
	ENABLE_ACLK_DISP_SECURE_SMMU_RW_SFW,
	ENABLE_PCLK_G3D,
	ENABLE_PCLK_DISP_SECURE_SMMU_RO_MMU,
	ENABLE_PCLK_DISP_SECURE_SMMU_RW_MMU,
	ENABLE_PCLK_DISP_SECURE_SMMU_RO_SFW,
	ENABLE_PCLK_DISP_SECURE_SMMU_RW_SFW,
	ENABLE_SCLK_DISP1,
	ENABLE_SCLK_DISP2,
	ENABLE_SCLK_DISP4,
};

/* List of parent clocks for Muxes */
PNAME(mout_aclk_disp_400_user_p) 	= {"fin_pll", "aclk_disp_400"};
PNAME(mout_sclk_dsd_user_p) 		= {"fin_pll", "sclk_dsd"};
PNAME(mout_sclk_decon_int_eclk_user_p) 	= {"fin_pll", "sclk_decon_int_eclk"};
PNAME(mout_sclk_decon_ext_eclk_user_p) 	= {"fin_pll", "sclk_decon_ext_eclk"};
PNAME(mout_sclk_decon_vclk_user_p) 	= {"fin_pll", "sclk_decon_vclk"};
PNAME(mout_disp_pll_ctrl_p) 		= {"fin_pll", "fout_disp_pll"};

PNAME(mout_sclk_decon_int_eclk_sub_p) = {"mout_sclk_decon_int_eclk_user",
					 "mout_disp_pll_ctrl"};

PNAME(mout_sclk_decon_ext_eclk_sub_p) = {"mout_sclk_decon_ext_eclk_user",
					 "mout_disp_pll_ctrl"};

PNAME(mout_group1_p) = {"mout_sclk_decon_vclk_user", "mout_disp_pll_ctrl"};

PNAME(mout_phyclk_hdmiphy_pixel_clko_user_p) = {"fin_pll",
						"phyclk_hdmiphy_pixel_clko"};

PNAME(mout_phyclk_hdmiphy_tmds_clko_user_p)  = {"fin_pll",
						"phyclk_hdmiphy_tmds_clko"};

PNAME(mout_phyclk_mipidphy0_rxclkesc0_user_p) = {"fin_pll",
						 "phyclk_mipidphy0_rxclkesc0"};

PNAME(mout_phyclk_mipidphy0_bitclkdiv8_user_p) = {"fin_pll",
						"phyclk_mipidphy0_bitclkdiv8"};

PNAME(mout_phyclk_mipidphy1_rxclkesc0_user_p) = {"fin_pll",
						"phyclk_mipidphy1_rxclkesc0"};

PNAME(mout_phyclk_mipidphy1_bitclkdiv8_user_p) = {"fin_pll",
						"phyclk_mipidphy1_bitclkdiv8"};

PNAME(mout_phyclk_dphy_ch0_tdx_clk_user_p)   = {"fin_pll", 
						"phyclk_dphy_ch0_tdx_clk"};

PNAME(mout_phyclk_dphy_ch1_tdx_clk_user_p)   = {"fin_pll",
						"phyclk_dphy_ch1_tdx_clk"};

PNAME(mout_phyclk_dphy_ch2_tdx_clk_user_p)   = {"fin_pll",
						"phyclk_dphy_ch2_tdx_clk"};

PNAME(mout_phyclk_dphy_ch3_tdx_clk_user_p)   = {"fin_pll",
						"phyclk_dphy_ch3_tdx_clk"};

PNAME(mout_decon_ext0_p) = {"mout_phyclk_hdmiphy_pixel_clko_user",
			    "dout_sclk_decon_ext_extclkpll_sub"};

PNAME(mout_decon_ext1_p) = {"sclk_rgb_vclk_dsim1", "sclk_decon_ext_vclk"};

/* PLLs */
static const struct samsung_pll_rate_table disp_pll_table[] __initconst = {
	PLL_36XX_RATE(266000000, 22, 1, 1, 10923),
	PLL_36XX_RATE(252000000, 21, 1, 1, 0),
	PLL_36XX_RATE(240000000, 20, 1, 1, 0),
	PLL_36XX_RATE(133000000, 22, 1, 2, 10923),
	PLL_36XX_RATE(126000000, 21, 1, 2, 0),
	PLL_36XX_RATE(96000000, 32, 1, 3, 0),
	{},
};

static const struct samsung_pll_clock disp_pll_clks[] __initconst = {
	PLL(pll_1460x, 0, "fout_disp_pll", "fin_pll", DISP_PLL_LOCK,
	    DISP_PLL_CON0, disp_pll_table),
};

/* Fixed rate clocks */
static const struct samsung_fixed_rate_clock fixed_rate_clks_disp[] __initconst = {
	FRATE(0, "phyclk_hdmiphy_pixel_clko", NULL,
	      CLK_IS_ROOT, 300000000),
	FRATE(0, "phyclk_hdmiphy_tmds_clko", NULL,
	      CLK_IS_ROOT, 300000000),
	FRATE(0, "phyclk_mipidphy0_rxclkesc0", NULL,
	      CLK_IS_ROOT, 20000000),
	FRATE(0, "phyclk_mipidphy0_bitclkdiv8", NULL,
	      CLK_IS_ROOT, 187500000),
	FRATE(0, "phyclk_mipidphy1_rxclkesc0", NULL,
	      CLK_IS_ROOT, 20000000),
	FRATE(0, "phyclk_mipidphy1_bitclkdiv8", NULL,
	      CLK_IS_ROOT, 187500000),

	FRATE(0, "sclk_rgb_vclk_dsim0", NULL,
	      CLK_IS_ROOT, 300000000),
	FRATE(0, "sclk_rgb_vclk_dsim1", NULL,
	      CLK_IS_ROOT, 300000000),
	FRATE(0, "sclk_decon_ext_vclk", NULL,
	      CLK_IS_ROOT, 300000000),

	FRATE(0, "phyclk_dphy_ch0_tdx_clk", NULL,
	      CLK_IS_ROOT, 270000000),
	FRATE(0, "phyclk_dphy_ch1_tdx_clk", NULL,
	      CLK_IS_ROOT, 270000000),
	FRATE(0, "phyclk_dphy_ch2_tdx_clk", NULL,
	      CLK_IS_ROOT, 270000000),
	FRATE(0, "phyclk_dphy_ch3_tdx_clk", NULL,
	      CLK_IS_ROOT, 270000000),
};

/* Muxes */
static const struct samsung_mux_clock disp_mux_clks[] __initconst = {
	MUX(0, "mout_aclk_disp_400_user", mout_aclk_disp_400_user_p,
	    MUX_SEL_DISP0, 24, 1),
	MUX(0, "mout_disp_pll_ctrl", mout_disp_pll_ctrl_p,
	    MUX_SEL_DISP0, 28, 1),

	MUX(0, "mout_sclk_dsd_user", mout_sclk_dsd_user_p,
	    MUX_SEL_DISP1, 16, 1),
	MUX(0, "mout_sclk_decon_vclk_user", mout_sclk_decon_vclk_user_p,
	    MUX_SEL_DISP1, 20, 1),
	MUX(0, "mout_sclk_decon_ext_eclk_user", mout_sclk_decon_ext_eclk_user_p,
	    MUX_SEL_DISP1, 24, 1),
	MUX(0, "mout_sclk_decon_int_eclk_user", mout_sclk_decon_int_eclk_user_p,
	    MUX_SEL_DISP1, 28, 1),

	MUX(0, "mout_phyclk_mipidphy1_bitclkdiv8_user",
	    mout_phyclk_mipidphy1_bitclkdiv8_user_p,
	    MUX_SEL_DISP2, 0, 1),
	MUX(0, "mout_phyclk_mipidphy1_rxclkesc0_user",
	    mout_phyclk_mipidphy1_rxclkesc0_user_p,
	    MUX_SEL_DISP2, 4, 1),
	MUX(0, "mout_phyclk_mipidphy0_bitclkdiv8_user",
	    mout_phyclk_mipidphy0_bitclkdiv8_user_p,
	    MUX_SEL_DISP2, 8, 1),
	MUX(0, "mout_phyclk_mipidphy0_rxclkesc0_user",
	    mout_phyclk_mipidphy0_rxclkesc0_user_p,
	    MUX_SEL_DISP2, 12, 1),
	MUX(0, "mout_phyclk_hdmiphy_tmds_clko_user",
	    mout_phyclk_hdmiphy_tmds_clko_user_p,
	    MUX_SEL_DISP2, 16, 1),
	MUX(0, "mout_phyclk_hdmiphy_pixel_clko_user",
	    mout_phyclk_hdmiphy_pixel_clko_user_p,
	    MUX_SEL_DISP2, 20, 1),

	MUX(0, "mout_sclk_decon_ext_extclkpll_sub", mout_group1_p,
	    MUX_SEL_DISP3, 8, 1),
	MUX(0, "mout_sclk_decon_int_extclkpll_sub", mout_group1_p,
	    MUX_SEL_DISP3, 12, 1),
	MUX(0, "mout_sclk_decon_ext_eclk_sub", mout_sclk_decon_ext_eclk_sub_p,
	    MUX_SEL_DISP3, 16, 1),
	MUX(0, "mout_sclk_decon_int_eclk_sub", mout_sclk_decon_int_eclk_sub_p,
	    MUX_SEL_DISP3, 20, 1),

	MUX(0, "mout_phyclk_dphy_ch3_tdx_clk_user",
	    mout_phyclk_dphy_ch3_tdx_clk_user_p,
	    MUX_SEL_DISP4, 0, 1),
	MUX(0, "mout_phyclk_dphy_ch2_tdx_clk_user",
	    mout_phyclk_dphy_ch2_tdx_clk_user_p,
	    MUX_SEL_DISP4, 4, 1),
	MUX(0, "mout_phyclk_dphy_ch1_tdx_clk_user",
	    mout_phyclk_dphy_ch1_tdx_clk_user_p,
	    MUX_SEL_DISP4, 8, 1),
	MUX(0, "mout_phyclk_dphy_ch0_tdx_clk_user",
	    mout_phyclk_dphy_ch0_tdx_clk_user_p,
	    MUX_SEL_DISP4, 12, 1),

	/* Note: These two are controlled by the same bit */
	MUX(0, "mout_decon_ext_0", mout_decon_ext0_p,
	    MUX_SEL_DISP5, 28, 1),
	MUX(0, "mout_decon_ext_1", mout_decon_ext1_p,
	    MUX_SEL_DISP5, 28, 1),

};

/* Dividers */
static const struct samsung_div_clock disp_div_clks[] __initconst = {
	DIV(0, "dout_pclk_disp", "mout_aclk_disp_400_user",
	    DIV_DISP, 0, 3),
	DIV(0, "dout_sclk_decon_ext_extclkpll_sub",
	       "mout_sclk_decon_ext_extclkpll_sub",
	    DIV_DISP, 8, 3),
	DIV(0, "dout_sclk_decon_int_extclk_pll_sub",
	       "mout_sclk_decon_int_extclkpll_sub",
	    DIV_DISP, 12, 3),
	DIV(0, "dout_sclk_decon_ext_eclk_sub", "mout_sclk_decon_ext_eclk_sub",
	    DIV_DISP, 16, 3),
	DIV(0, "dout_sclk_decon_int_eclk_sub", "mout_sclk_decon_int_eclk_sub",
	    DIV_DISP, 20, 3),
};

/* Clock gates */
static const struct samsung_gate_clock disp_gate_clks[] __initconst = {
	GATE(ACLK_BTS_AXI_DISP_RW_1, "aclk_bts_axi_disp_rw_1",
	     "mout_aclk_disp_400_user",
	     ENABLE_ACLK_DISP, 22, 0, 0),
	GATE(ACLK_BTS_AXI_DISP_RW_0, "aclk_bts_axi_disp_rw_0",
	     "mout_aclk_disp_400_user",
	     ENABLE_ACLK_DISP, 23, 0, 0),
	GATE(ACLK_BTS_AXI_DISP_RO_1, "aclk_bts_axi_disp_ro_1",
	     "mout_aclk_disp_400_user",
	     ENABLE_ACLK_DISP, 24, 0, 0),
	GATE(ACLK_BTS_AXI_DISP_RO_0, "aclk_bts_axi_dsp_ro_0",
	     "mout_aclk_disp_400_user",
	     ENABLE_ACLK_DISP, 25, 0, 0),
	GATE(ACLK_XIU_DISP_RW, "aclk_xiu_disp_rw", "mout_aclk_disp_400_user",
	     ENABLE_ACLK_DISP, 26, 0, 0),
	GATE(ACLK_XIU_DISP_RO, "aclk_xiu_disp_ro", "mout_aclk_disp_400_user",
	     ENABLE_ACLK_DISP, 27, 0, 0),
	GATE(ACLK_LH_ASYNC_SI_TOP_DISP, "aclk_lh_async_si_top_disp",
	     "mout_aclk_disp_400_user",
	     ENABLE_ACLK_DISP, 28, 0, 0),
	GATE(ACLK_LH_ASYNC_SI_R_TOP_DISP, "aclk_lh_async_si_r_top_disp",
	     "mout_aclk_disp_400_user",
	     ENABLE_ACLK_DISP, 29, 0, 0),
	GATE(ACLK_DECON_EXT, "aclk_decon_ext", "mout_aclk_disp_400_user",
	     ENABLE_ACLK_DISP, 30, 0, 0),
	GATE(ACLK_DECON_INT, "aclk_decon_int", "mout_aclk_disp_400_user",
	     ENABLE_ACLK_DISP, 31, 0, 0),

	GATE(ACLK_SMMU_DISP_RO_MMU, "aclk_smmu_disp_ro_mmu",
	     "mout_aclk_disp_400_user",
	     ENABLE_ACLK_DISP_SECURE_SMMU_RO_MMU, 0, 0, 0),

	GATE(ACLK_SMMU_DISP_RW_MMU, "aclk_smmu_disp_rw_mmu",
	     "mout_aclk_disp_400_user",
	     ENABLE_ACLK_DISP_SECURE_SMMU_RW_MMU, 0, 0, 0),

	GATE(ACLK_SMMU_DISP_RO_SFW, "aclk_smmu_disp_ro_sfw",
	     "mout_aclk_disp_400_user",
	     ENABLE_ACLK_DISP_SECURE_SMMU_RO_SFW, 0, 0, 0),

	GATE(ACLK_SMMU_DISP_RW_SFW, "aclk_smmu_disp_rw_sfw",
	     "mout_aclk_disp_400_user",
	     ENABLE_ACLK_DISP_SECURE_SMMU_RW_SFW, 0, 0, 0),

	GATE(PCLK_DP, "pclk_dp", "dout_pclk_disp",
	     ENABLE_PCLK_DISP, 14, 0, 0),
	GATE(0, "pclk_peri_ip_bus_noc_disp", "dout_pclk_disp",
	     ENABLE_PCLK_DISP, 15, CLK_IGNORE_UNUSED, 0),
	GATE(PCLK_BTS_AXI_DISP_RW_1, "pclk_bts_axi_disp_rw_1", "dout_pclk_disp",
	     ENABLE_PCLK_DISP, 19, 0, 0),
	GATE(PCLK_BTS_AXI_DISP_RW_0, "pclk_bts_axi_disp_rw_0", "dout_pclk_disp",
	     ENABLE_PCLK_DISP, 20, 0, 0),
	GATE(PCLK_BTS_AXI_DISP_RO_1, "pclk_bts_axi_disp_ro_1", "dout_pclk_disp",
	     ENABLE_PCLK_DISP, 21, 0, 0),
	GATE(PCLK_BTS_AXI_DISP_RO_0, "pclk_bts_axi_disp_ro_0", "dout_pclk_disp",
	     ENABLE_PCLK_DISP, 22, 0, 0),
	GATE(0, "pclk_pmu_disp", "dout_pclk_disp",
	     ENABLE_PCLK_DISP, 23, CLK_IGNORE_UNUSED, 0),
	GATE(0, "pclk_ahb2apb_disp1p", "dout_pclk_disp",
	     ENABLE_PCLK_DISP, 24, CLK_IGNORE_UNUSED, 0),
	GATE(PCLK_AHB2APB_DISP0P, "pclk_ahb2apb_disp0p", "dout_pclk_disp",
	     ENABLE_PCLK_DISP, 25, 0, 0),
	GATE(PCLK_HDMI_PHY, "pclk_hdmi_phy", "dout_pclk_disp",
	     ENABLE_PCLK_DISP, 26, 0, 0),
	GATE(0, "pclk_hdmi", "dout_pclk_disp",
	     ENABLE_PCLK_DISP, 27, CLK_IGNORE_UNUSED, 0),
	GATE(PCLK_DSIM1, "pclk_dsim1", "dout_pclk_disp",
	     ENABLE_PCLK_DISP, 28, 0, 0),
	GATE(PCLK_DSIM0, "pclk_dsim0", "dout_pclk_disp",
	     ENABLE_PCLK_DISP, 29, 0, 0),
	GATE(PCLK_DECON_EXT, "pclk_decon_ext", "dout_pclk_disp",
	     ENABLE_PCLK_DISP, 30, 0, 0),
	GATE(PCLK_DECON_INT, "pclk_decon_int", "dout_pclk_disp",
	     ENABLE_PCLK_DISP, 31, 0, 0),

	GATE(PCLK_SMMU_DISP_RO_MMU, "pclk_smmu_disp_ro_mmu", "dout_pclk_disp",
	     ENABLE_PCLK_DISP_SECURE_SMMU_RO_MMU, 0, 0, 0),

	GATE(PCLK_SMMU_DISP_RW_MMU, "pclk_smmu_disp_rw_mmu", "dout_pclk_disp",
	     ENABLE_PCLK_DISP_SECURE_SMMU_RW_MMU, 0, 0, 0),

	GATE(PCLK_SMMU_DISP_RO_SFW, "pclk_smmu_disp_ro_sfw", "dout_pclk_disp",
	     ENABLE_PCLK_DISP_SECURE_SMMU_RO_SFW, 0, 0, 0),

	GATE(PCLK_SMMU_DISP_RW_SFW, "pclk_smmu_disp_rw_sfw", "dout_pclk_disp",
	     ENABLE_PCLK_DISP_SECURE_SMMU_RW_SFW, 0, 0, 0),

	GATE(SCLK_DECON_EXT_EXTCLKPLL_SUB, "sclk_decon_ext_extclkpll_sub",
	     "mout_decon_ext_0",
	     ENABLE_SCLK_DISP1, 4, 0, 0),
	GATE(SCLK_DECON_INT_EXTCLKPLL_SUB, "sclk_decon_int_extclkpll_sub",
	     "dout_sclk_decon_int_extclk_pll_sub",
	     ENABLE_SCLK_DISP1, 8, 0, 0),
	GATE(SCLK_HDMI_SPDIF_USER, "sclk_hdmi_spdif_user", "sclk_hdmi_spdif",
	     ENABLE_SCLK_DISP1, 12, 0, 0),
	GATE(SCLK_DSD_USER, "sclk_dsd_user", "mout_sclk_dsd_user",
	     ENABLE_SCLK_DISP1, 16, 0, 0),
	GATE(SCLK_DECON_EXT_ECLK_SUB, "sclk_decon_ext_eclk_sub",
	     "dout_sclk_decon_ext_eclk_sub",
	     ENABLE_SCLK_DISP1, 24, 0, 0),
	GATE(SCLK_DECON_INT_ECLK_SUB, "sclk_decon_int_eclk_sub",
	     "dout_sclk_decon_int_eclk_sub",
	     ENABLE_SCLK_DISP1, 28, 0, 0),

	GATE(PHYCLK_MIPIDPHY1_BITCLKDIV8_USER,
	     "phyclk_mipidphy1_bitclkdiv8_user",
	     "mout_phyclk_mipidphy1_bitclkdiv8_user",
	     ENABLE_SCLK_DISP2, 0, 0, 0),
	GATE(PHYCLK_MIPIDPHY1_RXCLKESC0_USER, "phyclk_mipidphy1_rxclkesc0_user",
	     "mout_phyclk_mipidphy1_rxclkesc0_user",
	     ENABLE_SCLK_DISP2, 4, 0, 0),
	GATE(PHYCLK_MIPIDPHY0_BITCLKDIV8_USER,
	     "phyclk_mipidphy0_bitclkdiv8_user",
	     "mout_phyclk_mipidphy0_bitclkdiv8_user",
	     ENABLE_SCLK_DISP2, 8, 0, 0),
	GATE(PHYCLK_MIPIDPHY0_RXCLKESC0_USER, "phyclk_mipidphy0_rxclkesc0_user",
	     "mout_phyclk_mipidphy0_rxclkesc0_user",
	     ENABLE_SCLK_DISP2, 12, 0, 0),
	GATE(0, "phyclk_hdmiphy_tmds_clko_user",
	     "mout_phyclk_hdmiphy_tmds_clko_user",
	     ENABLE_SCLK_DISP2, 16, CLK_IGNORE_UNUSED, 0),
	GATE(0, "phyclk_hdmiphy_pixel_clko_user",
	     "mout_phyclk_hdmiphy_pixel_clko_user",
	     ENABLE_SCLK_DISP2, 20, CLK_IGNORE_UNUSED, 0),
	GATE(SCLK_RGB_VCLK_DSIM1_GATED, "sclk_rgb_vclk_dsim1_gated",
	     "mout_decon_ext_1",
	     ENABLE_SCLK_DISP2, 24, 0, 0),
	GATE(SCLK_RGB_VCLK_DSIM0_GATED, "sclk_rgb_vclk_dsim0_gated",
	     "sclk_rgb_vclk_dsim0",
	     ENABLE_SCLK_DISP2, 28, 0, 0),

	GATE(PHYCLK_DPPHY_CH3_TXD_CLK_USER, "phyclk_dpphy_ch3_txd_clk_user",
	     "mout_phyclk_dphy_ch3_tdx_clk_user",
	     ENABLE_SCLK_DISP4, 0, 0, 0),
	GATE(PHYCLK_DPPHY_CH2_TXD_CLK_USER, "phyclk_dpphy_ch2_txd_clk_user",
	     "mout_phyclk_dphy_ch2_tdx_clk_user",
	     ENABLE_SCLK_DISP4, 4, 0, 0),
	GATE(PHYCLK_DPPHY_CH1_TXD_CLK_USER, "phyclk_dpphy_ch1_txd_clk_user",
	     "mout_phyclk_dphy_ch1_tdx_clk_user",
	     ENABLE_SCLK_DISP4, 8, 0, 0),
	GATE(PHYCLK_DPPHY_CH0_TXD_CLK_USER, "phyclk_dpphy_ch0_txd_clk_user",
	     "mout_phyclk_dphy_ch0_tdx_clk_user",
	     ENABLE_SCLK_DISP4, 12, 0, 0),
	GATE(SCLK_DP_PHY_24M, "sclk_dp_phy_24M", "fin_pll",
	     ENABLE_SCLK_DISP4, 20, 0, 0),
	GATE(SCLK_DP_LINK_24M, "sclk_dp_link_24M", "fin_pll",
	     ENABLE_SCLK_DISP4, 24, 0, 0),
	GATE(SCLK_RGB_VCLK_DP_GATED, "sclk_rgb_vclk_dp_gated",
	     "sclk_rgb_vclk_dsim0",
	     ENABLE_SCLK_DISP4, 28, 0, 0),
};

static const struct samsung_cmu_info disp_cmu_info __initconst = {
	.pll_clks = disp_pll_clks,
	.nr_pll_clks = ARRAY_SIZE(disp_pll_clks),
	.mux_clks = disp_mux_clks,
	.nr_mux_clks = ARRAY_SIZE(disp_mux_clks),
	.div_clks = disp_div_clks,
	.nr_div_clks = ARRAY_SIZE(disp_div_clks),
	.gate_clks = disp_gate_clks,
	.nr_gate_clks = ARRAY_SIZE(disp_gate_clks),
	.fixed_clks = fixed_rate_clks_disp,
	.nr_fixed_clks = ARRAY_SIZE(fixed_rate_clks_disp),
	.nr_clk_ids = DISP_NR_CLK,
	.clk_regs = disp_clk_regs,
	.nr_clk_regs = ARRAY_SIZE(disp_clk_regs),
};

static void __init exynos7_clk_disp_init(struct device_node *np)
{
	samsung_cmu_register_one(np, &disp_cmu_info);
}

CLK_OF_DECLARE(exynos7_clk_disp, "samsung,exynos7-clock-disp",
	       exynos7_clk_disp_init);

/**********************\
* CMU_AUD (0x114C0000) *
\**********************/

/* Register Offset definitions */
#define	MUX_SEL_AUD			0x0200
#define	DIV_AUD0			0x0600
#define	DIV_AUD1			0x0604
#define	ENABLE_ACLK_AUD			0x0800
#define	ENABLE_PCLK_AUD			0x0900
#define	ENABLE_SCLK_AUD			0x0A00

static const unsigned long aud_clk_regs[] __initconst = {
	MUX_SEL_AUD,
	DIV_AUD0,
	DIV_AUD1,
	ENABLE_ACLK_AUD,
	ENABLE_PCLK_AUD,
	ENABLE_SCLK_AUD,
};

/* List of parent clocks for Muxes */
PNAME(mout_aud_pll_user_p) = {"fin_pll", "fout_aud_pll"};
PNAME(mout_aud_group_p) = {"dout_aud_cdclk", "ioclk_audiocdclk0"};

/* Muxes */
static const struct samsung_mux_clock aud_mux_clks[] __initconst = {
	MUX(0, "mout_sclk_i2s", mout_aud_group_p, MUX_SEL_AUD, 12, 1),
	MUX(0, "mout_sclk_pcm", mout_aud_group_p, MUX_SEL_AUD, 16, 1),
	MUX(0, "mout_aud_pll_user", mout_aud_pll_user_p, MUX_SEL_AUD, 20, 1),
};

/* Dividers */
static const struct samsung_div_clock aud_div_clks[] __initconst = {
	DIV(0, "dout_aud_ca5", "mout_aud_pll_user", DIV_AUD0, 0, 4),
	DIV(0, "dout_aclk_aud", "dout_aud_ca5", DIV_AUD0, 4, 4),
	DIV(0, "dout_pclk_dbg", "dout_aud_ca5", DIV_AUD0, 8, 4),
	DIV(0, "dout_atclk_aud", "dout_aud_ca5", DIV_AUD0, 8, 4),

	DIV(0, "dout_sclk_i2s", "mout_sclk_i2s", DIV_AUD1, 0, 4),
	DIV(0, "dout_sclk_pcm", "mout_sclk_pcm", DIV_AUD1, 4, 8),
	DIV(0, "dout_sclk_uart", "dout_aud_cdclk", DIV_AUD1, 12, 4),
	DIV(0, "dout_sclk_slimbus", "dout_aud_cdclk", DIV_AUD1, 16, 5),
	DIV(0, "dout_aud_cdclk", "mout_aud_pll_user", DIV_AUD1, 24, 4),
};

/* Clock gates */
static const struct samsung_gate_clock aud_gate_clks[] __initconst = {
	GATE(ACLK_ATCLK_AUD, "aclk_atclk_aud", "dout_atclk_aud",
	     ENABLE_ACLK_AUD, 24, 0, 0),
	GATE(0, "aclk_intr", "dout_aclk_aud",
	     ENABLE_ACLK_AUD, 25, CLK_IGNORE_UNUSED, 0),
	GATE(0, "aclk_xiu_lpassx", "dout_aclk_aud",
	     ENABLE_ACLK_AUD, 26, CLK_IGNORE_UNUSED, 0),
	GATE(ACLK_SMMU_AUD, "aclk_smmu_aud", "dout_aclk_aud",
	     ENABLE_ACLK_AUD, 27, 0, 0),
	GATE(ACLK_ACEL_LH_ASYNC_SI_TOP_133,
	     "aclk_acel_lh_async_si_top_133", "dout_aclk_aud",
	     ENABLE_ACLK_AUD, 28, 0, 0),
	GATE(0, "aclk_audnp_133", "dout_aclk_aud",
	     ENABLE_ACLK_AUD, 29, CLK_IGNORE_UNUSED, 0),
	GATE(0, "aclk_sramc", "dout_aclk_aud",
	     ENABLE_ACLK_AUD, 30, CLK_IGNORE_UNUSED, 0),
	GATE(0, "aclk_dmac", "dout_aclk_aud",
	     ENABLE_ACLK_AUD, 31, CLK_IGNORE_UNUSED, 0),

	GATE(PCLK_DBG_AUD, "pclk_dbg_aud", "dout_pclk_dbg",
	     ENABLE_PCLK_AUD, 19, 0, 0),
	GATE(PCLK_GPIO_AUD, "pclk_gpio_aud", "dout_aclk_aud",
	     ENABLE_PCLK_AUD, 20, 0, 0),
	GATE(0, "pclk_pmu_aud", "dout_aclk_aud",
	     ENABLE_PCLK_AUD, 21, CLK_IGNORE_UNUSED, 0),
	GATE(PCLK_WDT1, "pclk_wdt1", "dout_aclk_aud",
	     ENABLE_PCLK_AUD, 22, 0, 0),
	GATE(PCLK_WDT0, "pclk_wdt0", "dout_aclk_aud",
	     ENABLE_PCLK_AUD, 23, 0, 0),
	GATE(PCLK_SLIMBUS, "pclk_slimbus", "dout_aclk_aud",
	     ENABLE_PCLK_AUD, 24, 0, 0),
	GATE(PCLK_UART, "pclk_uart", "dout_aclk_aud",
	     ENABLE_PCLK_AUD, 25, 0, 0),
	GATE(PCLK_PCM, "pclk_pcm", "dout_aclk_aud",
	     ENABLE_PCLK_AUD, 26, CLK_SET_RATE_PARENT, 0),
	GATE(PCLK_I2S, "pclk_i2s", "dout_aclk_aud",
	     ENABLE_PCLK_AUD, 27, CLK_SET_RATE_PARENT, 0),
	GATE(PCLK_TIMER, "pclk_timer", "dout_aclk_aud",
	     ENABLE_PCLK_AUD, 28, 0, 0),
	GATE(PCLK_SFR1, "pclk_sfr1", "dout_aclk_aud",
	     ENABLE_PCLK_AUD, 29, 0, 0),
	GATE(PCLK_SFR0, "pclk_sfr0", "dout_aclk_aud",
	     ENABLE_PCLK_AUD, 30, 0, 0),
	GATE(PCLK_SMMU_AUD, "pclk_smmu_aud", "dout_aclk_aud",
	     ENABLE_PCLK_AUD, 31, 0, 0),

	GATE(SCLK_I2S_BCLK, "sclk_i2s_bclk", "ioclk_i2s_bclk",
	     ENABLE_SCLK_AUD, 25, 0, 0),
	GATE(SCLK_SLIMBUS_CLKIN, "sclk_slimbus_clkin", "ioclk_slimbus_clk",
	     ENABLE_SCLK_AUD, 26, 0, 0),
	GATE(SCLK_PCM, "sclk_pcm", "dout_sclk_pcm",
	     ENABLE_SCLK_AUD, 27, CLK_SET_RATE_PARENT, 0),
	GATE(SCLK_I2S, "sclk_i2s", "dout_sclk_i2s",
	     ENABLE_SCLK_AUD, 28, CLK_SET_RATE_PARENT, 0),
	GATE(SCLK_UART, "sclk_uart", "dout_sclk_uart",
	     ENABLE_SCLK_AUD, 29, 0, 0),
	GATE(SCLK_SLIMBUS, "sclk_slimbus", "dout_sclk_slimbus",
	     ENABLE_SCLK_AUD, 30, 0, 0),
	GATE(0, "sclk_ca5", "dout_aud_ca5",
	     ENABLE_SCLK_AUD, 31, CLK_IGNORE_UNUSED, 0),
};

static const struct samsung_cmu_info aud_cmu_info __initconst = {
	.mux_clks = aud_mux_clks,
	.nr_mux_clks = ARRAY_SIZE(aud_mux_clks),
	.div_clks = aud_div_clks,
	.nr_div_clks = ARRAY_SIZE(aud_div_clks),
	.gate_clks = aud_gate_clks,
	.nr_gate_clks = ARRAY_SIZE(aud_gate_clks),
	.nr_clk_ids = AUD_NR_CLK,
	.clk_regs = aud_clk_regs,
	.nr_clk_regs = ARRAY_SIZE(aud_clk_regs),
};

static void __init exynos7_clk_aud_init(struct device_node *np)
{
	samsung_cmu_register_one(np, &aud_cmu_info);
}

CLK_OF_DECLARE(exynos7_clk_aud, "samsung,exynos7-clock-aud",
	       exynos7_clk_aud_init);


/************************\
* CMU_FSYS0 (0x10E90000) *
\************************/

/* Register Offset definitions */
#define	HSIC_PLL_LOCK			0x0000
#define	HSIC_PLL_CON0			0x0100
#define MUX_SEL_FSYS00			0x0200
#define MUX_SEL_FSYS01			0x0204
#define MUX_SEL_FSYS02			0x0208
#define MUX_SEL_FSYS03			0x020C
#define	DIV_FSYS0			0x0600
#define ENABLE_ACLK_FSYS00		0x0800
#define ENABLE_ACLK_FSYS01		0x0804
#define	ENABLE_PCLK_FSYS0		0x0900
#define ENABLE_SCLK_FSYS01		0x0A04
#define ENABLE_SCLK_FSYS02		0x0A08
#define	ENABLE_SCLK_FSYS03		0x0A0C
#define ENABLE_SCLK_FSYS04		0x0A10

static const unsigned long fsys0_clk_regs[] __initconst = {
	HSIC_PLL_LOCK,
	HSIC_PLL_CON0,
	MUX_SEL_FSYS00,
	MUX_SEL_FSYS01,
	MUX_SEL_FSYS02,
	MUX_SEL_FSYS03,
	DIV_FSYS0,
	ENABLE_ACLK_FSYS00,
	ENABLE_ACLK_FSYS01,
	ENABLE_PCLK_FSYS0,
	ENABLE_SCLK_FSYS01,
	ENABLE_SCLK_FSYS02,
	ENABLE_SCLK_FSYS03,
	ENABLE_SCLK_FSYS04,
};

/* List of parent clocks for Muxes */
PNAME(mout_fsys0_phyclk_sel0_p) = {"fin_pll", "fin_pll_26m",
				   "sclk_phy_fsys0_26m"};

PNAME(mout_aclk_fsys0_200_user_p) = {"fin_pll", "aclk_fsys0_200"};
PNAME(mout_hsic_pll_p) 		  = {"fin_pll", "fout_hsic_pll"};
PNAME(mout_sclk_phy_fsys0_user_p) = {"fin_pll", "sclk_phy_fsys0"};
PNAME(mout_sclk_mmc2_user_p) 	  = {"fin_pll", "sclk_mmc2"};
PNAME(mout_sclk_usbdrd300_user_p) = {"fin_pll", "sclk_usbdrd300"};

PNAME(mout_phyclk_usbdrd300_udrd30_phyclk_user_p) = {
"fin_pll", "phyclk_usbdrd300_udrd30_phyclock"};

PNAME(mout_phyclk_usbdrd300_udrd30_pipe_pclk_user_p) = {
"fin_pll", "phyclk_usbdrd300_udrd30_pipe_pclk"};

PNAME(mout_phyclk_usbhost20_phy_freeclk_hsic1_user_p) = {
"fin_pll", "phyclk_usbhost20_phy_freeclk_hsic1"};

PNAME(mout_phyclk_usbhost20_phy_phyclk_hsic1_user_p) = {
"fin_pll", "phyclk_usbhost20_phy_phyclk_hsic1"};

PNAME(mout_phyclk_lli_tx0_symbol_user_p) = {"fin_pll", "phyclk_lli_tx0_symbol"};
PNAME(mout_phyclk_lli_rx0_symbol_user_p) = {"fin_pll", "phyclk_lli_rx0_symbol"};
PNAME(mout_phyclk_pcie_tx0_user_p) = {"fin_pll", "phyclk_pcie_tx0"};
PNAME(mout_phyclk_pcie_rx0_user_p) = {"fin_pll", "phyclk_pcie_rx0"};

/* PLLs */
static const struct samsung_pll_rate_table hsic_pll_table[] __initconst = {
	PLL_35XX_RATE(480000000, 240, 6, 1),
	{},
};

static const struct samsung_pll_clock fsys0_pll_clks[] __initconst = {
	PLL(pll_1452x, 0, "fout_hsic_pll", "fin_pll", HSIC_PLL_LOCK,
	    HSIC_PLL_CON0, hsic_pll_table),
};

/* Fixed rate clocks */
static const struct samsung_fixed_rate_clock fixed_rate_clks_fsys0[] __initconst = {
	FRATE(0, "phyclk_usbdrd300_udrd30_phyclock", NULL,
	      CLK_IS_ROOT, 60000000),
	FRATE(0, "phyclk_usbdrd300_udrd30_pipe_pclk", NULL,
	      CLK_IS_ROOT, 125000000),
	FRATE(0, "phyclk_usbhost20_phy_freeclk_hsic1", NULL,
	      CLK_IS_ROOT, 60000000),
	FRATE(0, "phyclk_usbhost20_phy_phyclk_hsic1", NULL,
	      CLK_IS_ROOT, 60000000),
	FRATE(0, "phyclk_lli_tx0_symbol", NULL,
	      CLK_IS_ROOT, 150000000),
	FRATE(0, "phyclk_lli_rx0_symbol", NULL,
	      CLK_IS_ROOT, 150000000),
	FRATE(0, "phyclk_pcie_tx0", NULL,
	      CLK_IS_ROOT, 250000000),
	FRATE(0, "phyclk_pcie_rx0", NULL,
	      CLK_IS_ROOT, 250000000)
};

/* Fixed factor multipliers/dividers */
static const struct samsung_fixed_factor_clock fsys0_fixed_factor_clks[] __initconst = {
	FFACTOR(0, "ffac_fin_pll_div2", "fin_pll",
		1, 2, 0)
};

/* Muxes */
static const struct samsung_mux_clock fsys0_mux_clks[] __initconst = {
	MUX(0, "mout_fsys0_phyclk_sel0", mout_fsys0_phyclk_sel0_p,
	    MUX_SEL_FSYS00, 20, 2),
	MUX(0, "mout_aclk_fsys0_200_user", mout_aclk_fsys0_200_user_p,
	    MUX_SEL_FSYS00, 24, 1),
	MUX(0, "mout_hsic_pll", mout_hsic_pll_p,
	    MUX_SEL_FSYS00, 28, 1),

	MUX(0, "mout_sclk_phy_fsys0_user", mout_sclk_phy_fsys0_user_p,
	    MUX_SEL_FSYS01, 16, 1),
	MUX(0, "mout_sclk_mmc2_user", mout_sclk_mmc2_user_p,
	    MUX_SEL_FSYS01, 24, 1),
	MUX(0, "mout_sclk_usbdrd300_user", mout_sclk_usbdrd300_user_p,
	    MUX_SEL_FSYS01, 28, 1),

	MUX(0, "mout_phyclk_usbhost20_phy_phyclk_hsic1_user",
	    mout_phyclk_usbhost20_phy_phyclk_hsic1_user_p,
	    MUX_SEL_FSYS02, 16, 1),
	MUX(0, "mout_phyclk_usbhost20_phy_freeclk_hsic1_user",
	    mout_phyclk_usbhost20_phy_freeclk_hsic1_user_p,
	    MUX_SEL_FSYS02, 20, 1),
	MUX(0, "mout_phyclk_usbdrd300_udrd30_pipe_pclk_user",
	    mout_phyclk_usbdrd300_udrd30_pipe_pclk_user_p,
	    MUX_SEL_FSYS02, 24, 1),
	MUX(0, "mout_phyclk_usbdrd300_udrd30_phyclk_user",
	    mout_phyclk_usbdrd300_udrd30_phyclk_user_p,
	    MUX_SEL_FSYS02, 28, 1),

	MUX(0, "mout_phyclk_pcie_rx0_user",
	    mout_phyclk_pcie_rx0_user_p,
	    MUX_SEL_FSYS03, 8, 1),
	MUX(0, "mout_phyclk_pcie_tx0_user",
	    mout_phyclk_pcie_tx0_user_p,
	    MUX_SEL_FSYS03, 12, 1),
	MUX(0, "mout_phyclk_lli_rx0_symbol_user",
	    mout_phyclk_lli_rx0_symbol_user_p,
	    MUX_SEL_FSYS03, 16, 1),
	MUX(0, "mout_phyclk_lli_tx0_symbol_user",
	    mout_phyclk_lli_tx0_symbol_user_p,
	    MUX_SEL_FSYS03, 20, 1)
};

/* Dividers */
static const struct samsung_div_clock fsys0_div_clks[] __initconst = {
	DIV(0, "dout_usb20", "mout_hsic_pll", DIV_FSYS0, 4, 4),
	DIV(0, "dout_pclk_combo_phy_modem", "mout_aclk_fsys0_200_user",
	    DIV_FSYS0, 8, 2),
	DIV(0, "dout_aclk_100", "mout_aclk_fsys0_200_user", DIV_FSYS0, 12, 2)
};

/* Clock gates */
static const struct samsung_gate_clock fsys0_gate_clks[] __initconst = {
	GATE(ACLK_PDMA1, "aclk_pdma1", "mout_aclk_fsys0_200_user",
	     ENABLE_ACLK_FSYS00, 3, 0, 0),
	GATE(ACLK_PDMA0, "aclk_pdma0", "mout_aclk_fsys0_200_user",
	     ENABLE_ACLK_FSYS00, 4, 0, 0),
	GATE(ACLK_SYSMMU600_PDMAX, "aclk_sysmmu600_pdmax",
	     "mout_aclk_fsys0_200_user",
	     ENABLE_ACLK_FSYS00, 9, 0, 0),
	GATE(ACLK_BTS_MODEMX, "aclk_bts_modemx", "mout_aclk_fsys0_200_user",
	     ENABLE_ACLK_FSYS00, 10, 0, 0),
	GATE(ACLK_BTS_SDCARDX, "aclk_bts_sdcardx", "mout_aclk_fsys0_200_user",
	     ENABLE_ACLK_FSYS00, 11, 0, 0),
	GATE(ACLK_BTS_USBDRD300, "aclk_bts_usbdrd300",
	     "mout_aclk_fsys0_200_user",
	     ENABLE_ACLK_FSYS00, 12, 0, 0),
	GATE(ACLK_AXIUS_LLISFRX_LLIBE, "aclk_axius_llisfrx_llibe",
	     "mout_aclk_fsys0_200_user",
	     ENABLE_ACLK_FSYS00, 13, 0, 0),
	GATE(ACLK_AXIUS_LLISFRX_LLILL, "aclk_axius_llisfrx_llill",
	     "mout_aclk_fsys0_200_user",
	     ENABLE_ACLK_FSYS00, 14, 0, 0),
	GATE(ACLK_AXIUS_USBHS_FSYS0X, "aclk_axius_usbhs_fsys0x",
	     "mout_aclk_fsys0_200_user",
	     ENABLE_ACLK_FSYS00, 15, 0, 0),
	GATE(ACLK_AXIUS_LLI_LL, "aclk_axius_lli_ll", "mout_aclk_fsys0_200_user",
	     ENABLE_ACLK_FSYS00, 16, 0, 0),
	GATE(ACLK_AXIUS_LLI_BE, "aclk_axius_lli_be", "mout_aclk_fsys0_200_user",
	     ENABLE_ACLK_FSYS00, 17, 0, 0),
	GATE(ACLK_AXIUS_PDMAX, "aclk_axius_pdmax", "mout_aclk_fsys0_200_user",
	     ENABLE_ACLK_FSYS00, 18, 0, 0),
	GATE(ACLK_AXIUS_USBDRD30X_FSYS0X, "aclk_axius_usbdrd30x_fsys0x",
	     "mout_aclk_fsys0_200_user",
	     ENABLE_ACLK_FSYS00, 19, 0, 0),
	GATE(ACLK_AHB2AXI_USBHS_MODEMX, "aclk_ahb2axi_usbhs_modemx",
	     "mout_aclk_fsys0_200_user",
	     ENABLE_ACLK_FSYS00, 20, 0, 0),
	GATE(ACLK_AHB_USBHS, "aclk_ahb_usbhs", "mout_aclk_fsys0_200_user",
	     ENABLE_ACLK_FSYS00, 21, 0, 0),
	GATE(ACLK_XIU_LLISFRX, "aclk_xiu_llisfrx", "mout_aclk_fsys0_200_user",
	     ENABLE_ACLK_FSYS00, 22, 0, 0),
	GATE(ACLK_XIU_PDMAX, "aclk_xiu_pdmax", "mout_aclk_fsys0_200_user",
	     ENABLE_ACLK_FSYS00, 23, 0, 0),
	GATE(ACLK_XIU_MODEMX, "aclk_xiu_modemx", "mout_aclk_fsys0_200_user",
	     ENABLE_ACLK_FSYS00, 24, 0, 0),
	GATE(ACLK_XIU_SDCARDX, "aclk_xiu_sdcardx", "mout_aclk_fsys0_200_user",
	     ENABLE_ACLK_FSYS00, 25, 0, 0),
	GATE(ACLK_XIU_FSYS0X, "aclk_xiu_fsys0x", "mout_aclk_fsys0_200_user",
	     ENABLE_ACLK_FSYS00, 26, 0, 0),
	GATE(0, "aclk_ahb2apb_fsys0p", "mout_aclk_fsys0_200_user",
	     ENABLE_ACLK_FSYS00, 27, CLK_IGNORE_UNUSED, 0),
	GATE(ACLK_AHB_FSYS0H, "aclk_ahb_fsys0h", "mout_aclk_fsys0_200_user",
	     ENABLE_ACLK_FSYS00, 28, 0, 0),
	GATE(ACLK_AHB_USBDRD300_LINKH,
	     "aclk_ahb_usbdrd300_linkh", "mout_aclk_fsys0_200_user",
	     ENABLE_ACLK_FSYS00, 29, 0, 0),
	GATE(0, "aclk_fsys0np_100", "dout_aclk_100",
	     ENABLE_ACLK_FSYS00, 30, CLK_IGNORE_UNUSED, 0),
	GATE(ACLK_FSYS0ND_200, "aclk_fsys0nd_200", "mout_aclk_fsys0_200_user",
	     ENABLE_ACLK_FSYS00, 31, 0, 0),

	GATE(ACLK_ETR_USB_FSYS0,
	     "aclk_etr_usb_fsys0", "mout_aclk_fsys0_200_user",
	     ENABLE_ACLK_FSYS01, 14, 0, 0),
	GATE(ACLK_AXI_LH_ASYNC_MI_ETR,
	     "aclk_axi_lh_async_mi_etr", "mout_aclk_fsys0_200_user",
	     ENABLE_ACLK_FSYS01, 15, 0, 0),
	GATE(ACLK_COMBO_PHY_MODEM_PCS_PCLK,
	     "aclk_combo_phy_modem_pcs_pclk", "mout_aclk_fsys0_200_user",
	     ENABLE_ACLK_FSYS01, 18, 0, 0),
	GATE(ACLK_LLI_SVC_REM, "aclk_lli_svc_rem", "mout_aclk_fsys0_200_user",
	     ENABLE_ACLK_FSYS01, 19, 0, 0),
	GATE(ACLK_LLI_SVC_LOC, "aclk_lli_svc_loc", "mout_aclk_fsys0_200_user",
	     ENABLE_ACLK_FSYS01, 20, 0, 0),
	GATE(ACLK_LLI_BE_TARG, "aclk_lli_be_targ", "mout_aclk_fsys0_200_user",
	     ENABLE_ACLK_FSYS01, 21, 0, 0),
	GATE(ACLK_LLI_BE_INIT, "aclk_lli_be_init", "mout_aclk_fsys0_200_user",
	     ENABLE_ACLK_FSYS01, 22, 0, 0),
	GATE(ACLK_LLI_LL_TARG, "aclk_lli_ll_targ", "mout_aclk_fsys0_200_user",
	     ENABLE_ACLK_FSYS01, 23, 0, 0),
	GATE(ACLK_LLI_LL_INIT, "aclk_lli_ll_init", "mout_aclk_fsys0_200_user",
	     ENABLE_ACLK_FSYS01, 24, 0, 0),
	GATE(ACLK_PCIE_MODEM_DBI_ACLK,
	     "aclk_pcie_modem_dbi_aclk", "mout_aclk_fsys0_200_user",
	     ENABLE_ACLK_FSYS01, 25, 0, 0),
	GATE(ACLK_PCIE_MODEM_SLV_ACLK,
	     "aclk_pcie_modem_slv_aclk", "mout_aclk_fsys0_200_user",
	     ENABLE_ACLK_FSYS01, 26, 0, 0),
	GATE(ACLK_PCIE_MODEM_MSTR_ACLK,
	     "aclk_pcie_modem_mstr_aclk", "mout_aclk_fsys0_200_user",
	     ENABLE_ACLK_FSYS01, 27, 0, 0),
	GATE(ACLK_USBHOST20, "aclk_usbhost20", "mout_aclk_fsys0_200_user",
	     ENABLE_ACLK_FSYS01, 28, 0, 0),
	GATE(ACLK_USBDRD300, "aclk_usbdrd300", "mout_aclk_fsys0_200_user",
	     ENABLE_ACLK_FSYS01, 29, 0, 0),
	GATE(ACLK_MMC2, "aclk_mmc2", "mout_aclk_fsys0_200_user",
	     ENABLE_ACLK_FSYS01, 31, 0, 0),

	GATE(PCLK_ASYNC_COMBO_PHY_MODEM,
	     "pclk_async_combo_phy_modem", "dout_pclk_combo_phy_modem",
	     ENABLE_PCLK_FSYS0, 23, 0, 0),
	GATE(PCLK_GPIO_FSYS0,
	     "pclk_gpio_fsys0", "mout_aclk_fsys0_200_user",
	     ENABLE_PCLK_FSYS0, 24, 0, 0),
	GATE(PCLK_SYSMMU600_PDMAX,
	     "pclk_sysmmu600_pdmax", "mout_aclk_fsys0_200_user",
	     ENABLE_PCLK_FSYS0, 26, 0, 0),
	GATE(PCLK_BTS_MODEMX,
	     "pclk_bts_modemx", "mout_aclk_fsys0_200_user",
	     ENABLE_PCLK_FSYS0, 27, 0, 0),
	GATE(PCLK_BTS_SDCARDX,
	     "pclk_bts_sdcardx", "mout_aclk_fsys0_200_user",
	     ENABLE_PCLK_FSYS0, 28, 0, 0),
	GATE(PCLK_BTS_USBDRD300,
	     "pclk_bts_usbdrd300", "mout_aclk_fsys0_200_user",
	     ENABLE_PCLK_FSYS0, 29, 0, 0),
	GATE(0, "pclk_pmu_fsys0", "mout_aclk_fsys0_200_user",
	     ENABLE_PCLK_FSYS0, 31, CLK_IGNORE_UNUSED, 0),

	GATE(SCLK_USBDRD300_SUSPENDCLK,
	     "sclk_usbdrd300_suspendclk", "mout_sclk_usbdrd300_user",
	     ENABLE_SCLK_FSYS01, 4, 0, 0),
	GATE(SCLK_USBDRD300_REFCLK, "sclk_usbdrd300_refclk", "fin_pll",
	     ENABLE_SCLK_FSYS01, 8, 0, 0),
	GATE(SCLK_PCIE_MODEM, "sclk_pcie_modem", "fin_pll",
	     ENABLE_SCLK_FSYS01, 12, 0, 0),
	GATE(SCLK_PHY_FSYS0_USER,
	     "sclk_phy_fsys0_user", "mout_sclk_phy_fsys0_user",
	     ENABLE_SCLK_FSYS01, 16, 0, 0),
	GATE(SCLK_MMC2_USER, "sclk_mmc2_user", "mout_sclk_mmc2_user",
	     ENABLE_SCLK_FSYS01, 24, CLK_SET_RATE_PARENT, 0),

	GATE(PHYCLK_USBHOST20_PHY_PHYCLK_HSIC1_USER,
	     "phyclk_usbhost20_phy_phyclk_hsic1_user",
	     "mout_phyclk_usbhost20_phy_phyclk_hsic1_user",
	     ENABLE_SCLK_FSYS02, 16, 0, 0),
	GATE(PHYCLK_USBHOST20_PHY_FREECLK_HSIC1_USER,
	     "phyclk_usbhost20_phy_freeclk_hsic1_user",
	     "mout_phyclk_usbhost20_phy_freeclk_hsic1_user",
	     ENABLE_SCLK_FSYS02, 20, 0, 0),
	GATE(PHYCLK_USBDRD300_UDRD30_PIPE_PCLK_USER,
	     "phyclk_usbdrd300_udrd30_pipe_pclk_user",
	     "mout_phyclk_usbdrd300_udrd30_pipe_pclk_user",
	     ENABLE_SCLK_FSYS02, 24, 0, 0),
	GATE(PHYCLK_USBDRD300_UDRD30_PHYCLK_USER,
	     "phyclk_usbdrd300_udrd30_phyclk_user",
	     "mout_phyclk_usbdrd300_udrd30_phyclk_user",
	     ENABLE_SCLK_FSYS02, 28, 0, 0),

	GATE(PHYCLK_PCIE_RX0_USER,
	     "phyclk_pcie_rx0_user", "mout_phyclk_pcie_rx0_user",
	     ENABLE_SCLK_FSYS03, 8, 0, 0),
	GATE(PHYCLK_PCIE_TX0_USER,
	     "phyclk_pcie_tx0_user", "mout_phyclk_pcie_tx0_user",
	     ENABLE_SCLK_FSYS03, 12, 0, 0),
	GATE(PHYCLK_LLI_RX0_SYMBOL_USER,
	     "phyclk_lli_rx0_symbol_user",
	     "mout_phyclk_lli_rx0_symbol_user",
	     ENABLE_SCLK_FSYS03, 16, 0, 0),
	GATE(PHYCLK_LLI_TX0_SYMBOL_USER,
	     "phyclk_lli_tx0_symbol_user",
	     "mout_phyclk_lli_tx0_symbol_user",
	     ENABLE_SCLK_FSYS03, 20, 0, 0),

	GATE(SCLK_COMBO_PHY_MODEM_26M,
	     "sclk_combo_phy_modem_26m", "mout_fsys0_phyclk_sel0",
	     ENABLE_SCLK_FSYS04, 8, 0, 0),
	GATE(SCLK_HSB20HOST_CLK48MOHCI,
	     "sclk_usb20host_clk48mohci", "dout_usb20",
	     ENABLE_SCLK_FSYS04, 12, 0, 0),
	GATE(SCLK_USB20PHY_HSIC_PLL_480MCLK,
	     "sclk_usb20phy_hsic_pll_480mclk", "mout_hsic_pll",
	     ENABLE_SCLK_FSYS04, 16, 0, 0),
	GATE(OSCCLK_PHY_CLKOUT_USB20_HSIC_PHY,
	     "oscclk_phy_clkout_usb20_hsic_phy", "ffac_fin_pll_div2",
	     ENABLE_SCLK_FSYS04, 24, 0, 0),
	GATE(OSCCLK_PHY_CLKOUT_USB30_PHY,
	     "oscclk_phy_clkout_usb30_phy", "fin_pll",
	     ENABLE_SCLK_FSYS04, 28, 0, 0),
};

static const struct samsung_cmu_info fsys0_cmu_info __initconst = {
	.pll_clks = fsys0_pll_clks,
	.nr_pll_clks = ARRAY_SIZE(fsys0_pll_clks),
	.fixed_clks = fixed_rate_clks_fsys0,
	.nr_fixed_clks = ARRAY_SIZE(fixed_rate_clks_fsys0),
	.fixed_factor_clks = fsys0_fixed_factor_clks,
	.nr_fixed_factor_clks = ARRAY_SIZE(fsys0_fixed_factor_clks),
	.mux_clks = fsys0_mux_clks,
	.nr_mux_clks = ARRAY_SIZE(fsys0_mux_clks),
	.div_clks = fsys0_div_clks,
	.nr_div_clks = ARRAY_SIZE(fsys0_div_clks),
	.gate_clks = fsys0_gate_clks,
	.nr_gate_clks = ARRAY_SIZE(fsys0_gate_clks),
	.nr_clk_ids = FSYS0_NR_CLK,
	.clk_regs = fsys0_clk_regs,
	.nr_clk_regs = ARRAY_SIZE(fsys0_clk_regs),
};

static void __init exynos7_clk_fsys0_init(struct device_node *np)
{
	samsung_cmu_register_one(np, &fsys0_cmu_info);
}

CLK_OF_DECLARE(exynos7_clk_fsys0, "samsung,exynos7-clock-fsys0",
	       exynos7_clk_fsys0_init);

/************************\
* CMU_FSYS1 (0x156E0000) *
\************************/

/* Register Offset definitions */
#define MUX_SEL_FSYS10			0x0200
#define MUX_SEL_FSYS11			0x0204
#define MUX_SEL_FSYS12			0x0208
#define DIV_FSYS1			0x0600
#define ENABLE_ACLK_FSYS1		0x0800
#define ENABLE_PCLK_FSYS1               0x0900
#define ENABLE_SCLK_FSYS11              0x0A04
#define ENABLE_SCLK_FSYS12              0x0A08
#define ENABLE_SCLK_FSYS13              0x0A0C

static const unsigned long fsys1_clk_regs[] __initconst = {
	MUX_SEL_FSYS10,
	MUX_SEL_FSYS11,
	MUX_SEL_FSYS12,
	DIV_FSYS1,
	ENABLE_ACLK_FSYS1,
	ENABLE_PCLK_FSYS1,
	ENABLE_SCLK_FSYS11,
	ENABLE_SCLK_FSYS12,
	ENABLE_SCLK_FSYS13,
};

/* List of parent clocks for Muxes */
PNAME(mout_aclk_fsys1_200_user_p) = {"fin_pll", "aclk_fsys1_200"};

PNAME(mout_fsys1_group_p)    = {"fin_pll", "fin_pll_26m", "sclk_phy_fsys1_26m"};
PNAME(mout_fsys1_group_a_p)  = {"fin_pll_26m", "fin_pll", "sclk_phy_fsys1_26m"};

PNAME(mout_sclk_mmc0_user_p) = {"fin_pll", "sclk_mmc0"};
PNAME(mout_sclk_mmc1_user_p) = {"fin_pll", "sclk_mmc1"};
PNAME(mout_sclk_ufsunipro20_user_p)  = {"fin_pll", "sclk_ufsunipro20"};
PNAME(mout_sclk_phy_fsys1_user_p)    = {"fin_pll", "sclk_phy_fsys1"};
PNAME(mout_sclk_tlx400_wifi1_user_p) = {"fin_pll", "sclk_tlx400_wifi1"};

PNAME(mout_phyclk_ufs20_tx0_user_p) = {"fin_pll", "phyclk_ufs20_tx0_symbol"};
PNAME(mout_phyclk_ufs20_rx0_user_p) = {"fin_pll", "phyclk_ufs20_rx0_symbol"};
PNAME(mout_phyclk_ufs20_rx1_user_p) = {"fin_pll", "phyclk_ufs20_rx1_symbol"};
PNAME(mout_phyclk_pcie_wifi1_tx0_user_p) = {"fin_pll", "phyclk_pcie_wifi1_tx0"};
PNAME(mout_phyclk_pcie_wifi1_rx0_user_p) = {"fin_pll", "phyclk_pcie_wifi1_rx0"};


/* Fixed rate clocks */
static const struct samsung_fixed_rate_clock fixed_rate_clks_fsys1[] __initconst = {
	FRATE(0, "phyclk_ufs20_tx0_symbol", NULL,
	      CLK_IS_ROOT, 300000000),
	FRATE(0, "phyclk_ufs20_rx0_symbol", NULL,
	      CLK_IS_ROOT, 300000000),
	FRATE(0, "phyclk_ufs20_rx1_symbol", NULL,
	      CLK_IS_ROOT, 300000000),
	FRATE(0, "phyclk_pcie_wifi1_tx0", NULL,
	      CLK_IS_ROOT, 250000000),
	FRATE(0, "phyclk_pcie_wifi1_rx0", NULL,
	      CLK_IS_ROOT, 250000000),
};

/* Muxes */
static const struct samsung_mux_clock fsys1_mux_clks[] __initconst = {
	MUX(MOUT_FSYS1_PHYCLK_SEL1, "mout_fsys1_phyclk_sel1", mout_fsys1_group_p,
	    MUX_SEL_FSYS10, 16, 2),
	MUX(0, "mout_fsys1_phyclk_sel0", mout_fsys1_group_p,
	    MUX_SEL_FSYS10, 20, 2),
	MUX(0, "mout_aclk_fsys1_200_user", mout_aclk_fsys1_200_user_p,
	    MUX_SEL_FSYS10, 28, 1),

	MUX(0, "mout_sclk_tlx400_wifi1_user", mout_sclk_tlx400_wifi1_user_p,
	    MUX_SEL_FSYS11, 12, 1),
	MUX(0, "mout_sclk_phy_fsys1_user", mout_sclk_phy_fsys1_user_p,
	    MUX_SEL_FSYS11, 16, 1),
	MUX(0, "mout_sclk_ufsunipro20_user", mout_sclk_ufsunipro20_user_p,
	    MUX_SEL_FSYS11, 20, 1),
	MUX(0, "mout_sclk_mmc1_user", mout_sclk_mmc1_user_p,
	    MUX_SEL_FSYS11, 24, 1),
	MUX(0, "mout_sclk_mmc0_user", mout_sclk_mmc0_user_p,
	    MUX_SEL_FSYS11, 28, 1),

	MUX(0, "mout_phyclk_pcie_wifi1_rx0_user",
	    mout_phyclk_pcie_wifi1_rx0_user_p, MUX_SEL_FSYS12, 8, 1),
	MUX(0, "mout_phyclk_pcie_wifi1_tx0_user",
	    mout_phyclk_pcie_wifi1_tx0_user_p, MUX_SEL_FSYS12, 12, 1),
	MUX(0, "mout_phyclk_ufs20_rx1_symbol_user",
	    mout_phyclk_ufs20_rx1_user_p, MUX_SEL_FSYS12, 16, 1),
	MUX(0, "mout_phyclk_ufs20_rx0_symbol_user",
	    mout_phyclk_ufs20_rx0_user_p, MUX_SEL_FSYS12, 24, 1),
	MUX(0, "mout_phyclk_ufs20_tx0_symbol_user",
	    mout_phyclk_ufs20_tx0_user_p, MUX_SEL_FSYS12, 28, 1),
    
};

/* Dividers */
static const struct samsung_div_clock fsys1_div_clks[] __initconst = {
	DIV(0, "dout_pclk_fsys1", "mout_aclk_fsys1_200_user",
	    DIV_FSYS1, 0, 2),
	DIV(0, "dout_pclk_combo_phy_wifi1", "mout_aclk_fsys1_200_user",
	    DIV_FSYS1, 8, 2),
};

/* Clock gates */
static const struct samsung_gate_clock fsys1_gate_clks[] __initconst = {
	GATE(ACLK_COMBO_PHY_PCS_PCLK_WIFI1,
	     "aclk_combo_phy_pcs_pclk_wifi1", "mout_aclk_fsys1_200_user",
	     ENABLE_ACLK_FSYS1, 9, 0, 0),
	GATE(ACLK_PCIE_WIFI1_DBI_ACLK,
	     "aclk_pcie_wifi1_dbi_aclk", "mout_aclk_fsys1_200_user",
	     ENABLE_ACLK_FSYS1, 10, 0, 0),
	GATE(ACLK_PCIE_WIFI1_SLV_ACLK,
	     "aclk_pcie_wifi1_slv_aclk", "mout_aclk_fsys1_200_user",
	     ENABLE_ACLK_FSYS1, 11, 0, 0),
	GATE(ACLK_PCIE_WIFI1_MSTR_ACLK,
	     "aclk_pcie_wifi1_mstr_aclk", "mout_aclk_fsys1_200_user",
	     ENABLE_ACLK_FSYS1, 12, 0, 0),
	GATE(ACLK_AHB2AXI_PCIE_WIFI1,
	     "aclk_ahb2axi_pcie_wifi1", "mout_aclk_fsys1_200_user",
	     ENABLE_ACLK_FSYS1, 13, 0, 0),
	GATE(ACLK_BTS_WIFI1, "aclk_bts_wifi1", "mout_aclk_fsys1_200_user",
	     ENABLE_ACLK_FSYS1, 17, 0, 0),
	GATE(ACLK_BTS_EMBEDDED,
	     "aclk_bts_embedded", "mout_aclk_fsys1_200_user",
	     ENABLE_ACLK_FSYS1, 18, 0, 0),
	GATE(0, "aclk_xiuasync_tlx400_axius_wifi1", "mout_aclk_fsys1_200_user",
	     ENABLE_ACLK_FSYS1, 20, 0, 0),
	GATE(0, "aclk_axius_tlx400_wifi1", "mout_aclk_fsys1_200_user",
	     ENABLE_ACLK_FSYS1, 21, 0, 0),
	GATE(0, "aclk_ahb2apb_fsys1p", "mout_aclk_fsys1_200_user",
	     ENABLE_ACLK_FSYS1, 22, CLK_IGNORE_UNUSED, 0),
	GATE(ACLK_AHB_FSYS1H, "aclk_ahb_fsys1h", "mout_aclk_fsys1_200_user",
	     ENABLE_ACLK_FSYS1, 23, 0, 0),
	GATE(ACLK_XIU_WIFI1X, "aclk_xiu_wifi1x", "mout_aclk_fsys1_200_user",
	     ENABLE_ACLK_FSYS1, 24, 00, 0),
	GATE(ACLK_XIU_EMBEDDEDX,
	     "aclk_xiu_embeddedx", "mout_aclk_fsys1_200_user",
	     ENABLE_ACLK_FSYS1, 25, 0, 0),
	GATE(ACLK_XIU_FSYS1X, "aclk_xiu_fsys1x", "mout_aclk_fsys1_200_user",
	     ENABLE_ACLK_FSYS1, 26, 0, 0),
	GATE(0, "aclk_fsys1np_200", "mout_aclk_fsys1_200_user",
	     ENABLE_ACLK_FSYS1, 27, CLK_IGNORE_UNUSED, 0),
	GATE(ACLK_LH_ASYNC_FSYS1_200, "aclk_lh_async_fsys1_200",
	     "mout_aclk_fsys1_200_user",
	     ENABLE_ACLK_FSYS1, 28, 0, 0),
	GATE(ACLK_MMC1, "aclk_mmc1", "mout_aclk_fsys1_200_user",
	     ENABLE_ACLK_FSYS1, 29, 0, 0),
	GATE(ACLK_MMC0, "aclk_mmc0", "mout_aclk_fsys1_200_user",
	     ENABLE_ACLK_FSYS1, 30, 0, 0),
	GATE(ACLK_UFS20_LINK, "aclk_ufs20_link", "dout_pclk_fsys1",
	     ENABLE_ACLK_FSYS1, 31, 0, 0),

	GATE(PCLK_ASYNC_COMBO_PHY_WIFI1,
	     "pclk_async_combo_phy_wifi1", "dout_pclk_combo_phy_wifi1",
	     ENABLE_PCLK_FSYS1, 22, 0, 0),
	GATE(0, "pclk_pmu_fsys1", "mout_aclk_fsys1_200_user",
	     ENABLE_PCLK_FSYS1, 24, CLK_IGNORE_UNUSED, 0),
	GATE(PCLK_BTS_WIFI1,
	     "pclk_bts_wifi1", "mout_aclk_fsys1_200_user",
	     ENABLE_PCLK_FSYS1, 26, 0, 0),
	GATE(PCLK_BTS_EMBEDDED,
	     "pclk_bts_embedded", "mout_aclk_fsys1_200_user",
	     ENABLE_PCLK_FSYS1, 27, 0, 0),
	GATE(PCLK_XIUASYNC_AXISEL_TLX400_WIFI1,
	     "pclk_xiuasync_axisel_tlx400_wifi1",
	     "mout_aclk_fsys1_200_user",
	     ENABLE_PCLK_FSYS1, 28, 0, 0),
	GATE(PCLK_XIUASYNC_TLX400_AXIUS_WIFI1,
	     "pclk_xiuasync_tlx400_axius_wifi1",
	     "mout_aclk_fsys1_200_user",
	     ENABLE_PCLK_FSYS1, 29, 0, 0),
	GATE(PCLK_GPIO_FSYS1, "pclk_gpio_fsys1", "mout_aclk_fsys1_200_user",
	     ENABLE_PCLK_FSYS1, 30, 0, 0),
	GATE(PCLK_TLX400_WIFI1,
	     "pclk_tlx400_wifi1", "mout_aclk_fsys1_200_user",
	     ENABLE_PCLK_FSYS1, 31, 0, 0),

	GATE(SCLK_PCIE_LINK_WIFI1, "sclk_pcie_link_wifi1", "fin_pll",
	     ENABLE_SCLK_FSYS11, 0, 0, 0),
	GATE(SCLK_TLX400_WIFI1_TX_USER,
	     "sclk_tlx400_wifi1_tx_user", "mout_sclk_tlx400_wifi1_user",
	     ENABLE_SCLK_FSYS11, 8, 0, 0),
	GATE(SCLK_TLX400_WIFI1_SLOW, "sclk_tlx400_wifi1_slow", "fin_pll",
	     ENABLE_SCLK_FSYS11, 12, 0, 0),
	GATE(SCLK_PHY_FSYS1_USER,
	     "sclk_phy_fsys1_user", "mout_sclk_phy_fsys1_user",
	     ENABLE_SCLK_FSYS11, 16, 0, 0),
	GATE(SCLK_UFSUNIPRO20_USER,
	     "sclk_ufsunipro20_user", "mout_sclk_ufsunipro20_user",
	     ENABLE_SCLK_FSYS11, 20, 0, 0),
	GATE(SCLK_MMC1_USER, "sclk_mmc1_user", "mout_sclk_mmc1_user",
	     ENABLE_SCLK_FSYS11, 24, CLK_SET_RATE_PARENT, 0),
	GATE(SCLK_MMC0_USER, "sclk_mmc0_user", "mout_sclk_mmc0_user",
	     ENABLE_SCLK_FSYS11, 28, CLK_SET_RATE_PARENT, 0),

	GATE(OSCCLK_PHY_CLKOUT_EMBEDDED_COMBO_PHY,
	     "oscclk_phy_clkout_embedded_combo_phy", "fin_pll",
	     ENABLE_SCLK_FSYS12, 4, 0, 0),
	GATE(PHYCLK_PCIE_WIFI1_RX0_USER,
	     "phyclk_pcie_wifi1_rx0_user",
	     "mout_phyclk_pcie_wifi1_rx0_user",
	     ENABLE_SCLK_FSYS12, 8, 0, 0),
	GATE(PHYCLK_PCIE_WIFI1_TX0_USER,
	     "phyclk_pcie_wifi1_tx0_user",
	     "mout_phyclk_pcie_wifi1_tx0_user",
	     ENABLE_SCLK_FSYS12, 12, 0, 0),
	GATE(PHYCLK_UFS20_RX1_SYMBOL_USER,
	     "phyclk_ufs20_rx1_symbol_user",
	     "mout_phyclk_ufs20_rx1_symbol_user",
	     ENABLE_SCLK_FSYS12, 16, 0, 0),
	GATE(PHYCLK_UFS20_RX0_SYMBOL_USER,
	     "phyclk_ufs20_rx0_symbol_user",
	     "mout_phyclk_ufs20_rx0_symbol_user",
	     ENABLE_SCLK_FSYS12, 24, 0, 0),
	GATE(PHYCLK_UFS20_TX0_SYMBOL_USER,
	     "phyclk_ufs20_tx0_symbol_user",
	     "mout_phyclk_ufs20_tx0_symbol_user",
	     ENABLE_SCLK_FSYS12, 28, 0, 0),

	GATE(SCLK_COMBO_PHY_EMBEDDED_26M,
	     "sclk_combo_phy_embedded_26m", "mout_fsys1_phyclk_sel1",
	     ENABLE_SCLK_FSYS13, 24, 0, 0),
	GATE(SCLK_COMBO_PHY_WIFI1_26M,
	     "sclk_combo_phy_wifi1_26m", "mout_fsys1_phyclk_sel0",
	     ENABLE_SCLK_FSYS13, 28, 0, 0),
};

static const struct samsung_cmu_info fsys1_cmu_info __initconst = {
	.mux_clks = fsys1_mux_clks,
	.nr_mux_clks = ARRAY_SIZE(fsys1_mux_clks),
	.div_clks = fsys1_div_clks,
	.nr_div_clks = ARRAY_SIZE(fsys1_div_clks),
	.gate_clks = fsys1_gate_clks,
	.nr_gate_clks = ARRAY_SIZE(fsys1_gate_clks),
	.fixed_clks = fixed_rate_clks_fsys1,
	.nr_fixed_clks = ARRAY_SIZE(fixed_rate_clks_fsys1),
	.nr_clk_ids = FSYS1_NR_CLK,
	.clk_regs = fsys1_clk_regs,
	.nr_clk_regs = ARRAY_SIZE(fsys1_clk_regs),
};

static void __init exynos7_clk_fsys1_init(struct device_node *np)
{
	samsung_cmu_register_one(np, &fsys1_cmu_info);
}

CLK_OF_DECLARE(exynos7_clk_fsys1, "samsung,exynos7-clock-fsys1",
	       exynos7_clk_fsys1_init);


/***********************\
* CMU_MSCL (0x150D0000) *
\***********************/

/* Register Offset definitions */
#define MUX_SEL_MSCL				0x0200
#define DIV_MSCL				0x0600
#define ENABLE_ACLK_MSCL			0x0800
#define ENABLE_ACLK_MSCL_SMMU_MSCL_0_MMU	0x0804
#define ENABLE_ACLK_MSCL_SMMU_MSCL_0_SFW	0x0808
#define ENABLE_ACLK_MSCL_SMMU_MSCL_1_MMU	0x080C
#define ENABLE_ACLK_MSCL_SMMU_MSCL_1_SFW	0x0810
#define ENABLE_ACLK_MSCL_SMMU_G2D_MMU		0x0814
#define ENABLE_ACLK_MSCL_SMMU_G2D_SFW		0x0818
#define ENABLE_PCLK_MSCL			0x0900
#define ENABLE_PCLK_MSCL_SMMU_MSCL_0_MMU	0x0904
#define ENABLE_PCLK_MSCL_SMMU_MSCL_0_SFW	0x0908
#define ENABLE_PCLK_MSCL_SMMU_MSCL_1_MMU	0x090C
#define ENABLE_PCLK_MSCL_SMMU_MSCL_1_SFW	0x0910
#define ENABLE_PCLK_MSCL_SMMU_G2D_MMU		0x0914
#define ENABLE_PCLK_MSCL_SMMU_G2D_SFW		0x0918
#define ENABLE_SCLK_MSCL			0x0A00

static const unsigned long mscl_clk_regs[] __initconst = {
	MUX_SEL_MSCL,
	DIV_MSCL,
	ENABLE_ACLK_MSCL,
	ENABLE_ACLK_MSCL_SMMU_MSCL_0_MMU,
	ENABLE_ACLK_MSCL_SMMU_MSCL_0_SFW,
	ENABLE_ACLK_MSCL_SMMU_MSCL_1_MMU,
	ENABLE_ACLK_MSCL_SMMU_MSCL_1_SFW,
	ENABLE_ACLK_MSCL_SMMU_G2D_MMU,
	ENABLE_ACLK_MSCL_SMMU_G2D_SFW,
	ENABLE_PCLK_MSCL,
	ENABLE_PCLK_MSCL_SMMU_MSCL_0_MMU,
	ENABLE_PCLK_MSCL_SMMU_MSCL_0_SFW,
	ENABLE_PCLK_MSCL_SMMU_MSCL_1_MMU,
	ENABLE_PCLK_MSCL_SMMU_MSCL_1_SFW,
	ENABLE_PCLK_MSCL_SMMU_G2D_MMU,
	ENABLE_PCLK_MSCL_SMMU_G2D_SFW,
	ENABLE_SCLK_MSCL,
};

/* List of parent clocks for Muxes */
PNAME(mout_aclk_mscl_532_user_p) = {"fin_pll", "aclk_mscl_532"};

/* Muxes */
static const struct samsung_mux_clock mscl_mux_clks[] __initconst = {
	MUX(0, "mout_aclk_mscl_532_user",
	    mout_aclk_mscl_532_user_p, MUX_SEL_MSCL, 0, 1),
};

/* Dividers */
static const struct samsung_div_clock mscl_div_clks[] __initconst = {
	DIV(0, "dout_pclk_mscl", "mout_aclk_mscl_532_user",
	    DIV_MSCL, 0, 3),
};

/* Clock gates */
static const struct samsung_gate_clock mscl_gate_clks[] __initconst = {
	GATE(0, "aclk_ahb2apb_mscl1p", "mout_aclk_mscl_532_user",
	     ENABLE_ACLK_MSCL, 14, CLK_IGNORE_UNUSED, 0),
	GATE(0, "aclk_ahb2apb_mscl0p", "mout_aclk_mscl_532_user",
	     ENABLE_ACLK_MSCL, 15, CLK_IGNORE_UNUSED, 0),
	GATE(0, "aclk_msclnp_133", "mout_aclk_mscl_532_user",
	     ENABLE_ACLK_MSCL, 16, CLK_IGNORE_UNUSED, 0),
	GATE(ACLK_PPMU_MSCL_1, "aclk_ppmu_mscl_1", "mout_aclk_mscl_532_user",
	     ENABLE_ACLK_MSCL, 17, 0, 0),
	GATE(ACLK_PPMU_MSCL_0, "aclk_ppmu_mscl_0", "mout_aclk_mscl_532_user",
	     ENABLE_ACLK_MSCL, 18, 0, 0),
	GATE(ACLK_BTS_G2D, "aclk_bts_g2d", "mout_aclk_mscl_532_user",
	     ENABLE_ACLK_MSCL, 19, 0, 0),
	GATE(ACLK_BTS_JPEG, "aclk_bts_jpeg", "mout_aclk_mscl_532_user",
	     ENABLE_ACLK_MSCL, 20, 0, 0),
	GATE(ACLK_BTS_MSCL_1, "aclk_bts_mscl_1", "mout_aclk_mscl_532_user",
	     ENABLE_ACLK_MSCL, 21, 0, 0),
	GATE(ACLK_BTS_MSCL_0, "aclk_bts_mscl_0", "mout_aclk_mscl_532_user",
	     ENABLE_ACLK_MSCL, 22, 0, 0),
	GATE(ACLK_AXI2ACEL_BRIDGE,
	     "aclk_axi2acel_bridge", "mout_aclk_mscl_532_user",
	     ENABLE_ACLK_MSCL, 23, 0, 0),
	GATE(ACLK_XIU_MSCLX_1, "aclk_xiu_msclx_1", "mout_aclk_mscl_532_user",
	     ENABLE_ACLK_MSCL, 24, 0, 0),
	GATE(ACLK_XIU_MSCLX_0, "aclk_xiu_msclx_0", "mout_aclk_mscl_532_user",
	     ENABLE_ACLK_MSCL, 25, 0, 0),
	GATE(ACLK_LH_ASYNC_SI_MSCL_1,
	     "aclk_lh_async_si_mscl_1", "mout_aclk_mscl_532_user",
	     ENABLE_ACLK_MSCL, 26, 0, 0),
	GATE(ACLK_LH_ASYNC_SI_MSCL_0,
	     "aclk_lh_async_si_mscl_0", "mout_aclk_mscl_532_user",
	     ENABLE_ACLK_MSCL, 27, 0, 0),
	GATE(ACLK_G2D, "aclk_g2d", "mout_aclk_mscl_532_user",
	     ENABLE_ACLK_MSCL, 28, 0, 0),
	GATE(ACLK_JPEG, "aclk_jpeg", "mout_aclk_mscl_532_user",
	     ENABLE_ACLK_MSCL, 29, 0, 0),
	GATE(ACLK_MSCL_1, "aclk_mscl_1", "mout_aclk_mscl_532_user",
	     ENABLE_ACLK_MSCL, 30, 0, 0),
	GATE(ACLK_MSCL_0, "aclk_mscl_0", "mout_aclk_mscl_532_user",
	     ENABLE_ACLK_MSCL, 31, 0, 0),

	GATE(ACLK_MSCL_SMMU_MSCL_0_MMU,
	     "aclk_mscl_smmu_mscl_0_mmu", "mout_aclk_mscl_532_user",
	     ENABLE_ACLK_MSCL_SMMU_MSCL_0_MMU, 0, 0, 0),
	GATE(ACLK_MSCL_SMMU_MSCL_0_SFW,
	     "aclk_mscl_smmu_mscl_0_sfw", "mout_aclk_mscl_532_user",
	     ENABLE_ACLK_MSCL_SMMU_MSCL_0_SFW, 0, 0, 0),

	GATE(ACLK_MSCL_SMMU_MSCL_1_MMU,
	     "aclk_mscl_smmu_mscl_1_mmu", "mout_aclk_mscl_532_user",
	     ENABLE_ACLK_MSCL_SMMU_MSCL_1_MMU, 0, 0, 0),
	GATE(ACLK_MSCL_SMMU_MSCL_1_SFW,
	     "aclk_mscl_smmu_mscl_1_sfw", "mout_aclk_mscl_532_user",
	     ENABLE_ACLK_MSCL_SMMU_MSCL_1_SFW, 0, 0, 0),

	GATE(ACLK_MSCL_SMMU_G2D_MMU,
	     "aclk_mscl_smmu_g2d_mmu", "mout_aclk_mscl_532_user",
	     ENABLE_ACLK_MSCL_SMMU_G2D_MMU, 0, 0, 0),
	GATE(ACLK_MSCL_SMMU_G2D_SFW,
	     "aclk_mscl_smmu_g2d_sfw", "mout_aclk_mscl_532_user",
	     ENABLE_ACLK_MSCL_SMMU_G2D_SFW, 0, CLK_IGNORE_UNUSED, 0),

	GATE(0, "pclk_pmu_mscl", "dout_pclk_mscl",
	     ENABLE_PCLK_MSCL, 20, CLK_IGNORE_UNUSED, 0),
	GATE(PCLK_AXI2ACEL_BRIDGE, "pclk_axi2acel_bridge", "dout_pclk_mscl",
	     ENABLE_PCLK_MSCL, 21, 0, 0),
	GATE(PCLK_PPMU_MSCL_1, "pclk_ppmu_mscl_1", "dout_pclk_mscl",
	     ENABLE_PCLK_MSCL, 22, 0, 0),
	GATE(PCLK_PPMU_MSCL_0, "pclk_ppmu_mscl_0", "dout_pclk_mscl",
	     ENABLE_PCLK_MSCL, 23, 0, 0),
	GATE(PCLK_BTS_G2D, "pclk_bts_g2d", "dout_pclk_mscl",
	     ENABLE_PCLK_MSCL, 24, 0, 0),
	GATE(PCLK_BTS_JPEG, "pclk_bts_jpeg", "dout_pclk_mscl",
	     ENABLE_PCLK_MSCL, 25, 0, 0),
	GATE(PCLK_BTS_MSCL_1, "pclk_bts_mscl_1", "dout_pclk_mscl",
	     ENABLE_PCLK_MSCL, 26, 0, 0),
	GATE(PCLK_BTS_MSCL_0, "pclk_bts_mscl_0", "dout_pclk_mscl",
	     ENABLE_PCLK_MSCL, 27, 0, 0),
	GATE(PCLK_G2D, "pclk_g2d", "dout_pclk_mscl",
	     ENABLE_PCLK_MSCL, 28, 0, 0),
	GATE(PCLK_JPEG, "pclk_jpeg", "dout_pclk_mscl",
	     ENABLE_PCLK_MSCL, 29, 0, 0),
	GATE(PCLK_MSCL_1, "pclk_mscl_1", "dout_pclk_mscl",
	     ENABLE_PCLK_MSCL, 30, 0, 0),
	GATE(PCLK_MSCL_0, "pclk_mscl_0", "dout_pclk_mscl",
	     ENABLE_PCLK_MSCL, 31, 0, 0),

	GATE(PCLK_MSCL_SMMU_MSCL_0_MMU,
	     "pclk_mscl_smmu_mscl_0_mmu", "dout_pclk_mscl",
	     ENABLE_PCLK_MSCL_SMMU_MSCL_0_MMU, 0, 0, 0),
	GATE(PCLK_MSCL_SMMU_MSCL_0_SFW,
	     "pclk_mscl_smmu_mscl_0_sfw", "dout_pclk_mscl",
	     ENABLE_PCLK_MSCL_SMMU_MSCL_0_SFW, 0, 0, 0),

	GATE(PCLK_MSCL_SMMU_MSCL_1_MMU,
	     "pclk_mscl_smmu_mscl_1_mmu", "dout_pclk_mscl",
	     ENABLE_PCLK_MSCL_SMMU_MSCL_1_MMU, 0, 0, 0),
	GATE(PCLK_MSCL_SMMU_MSCL_1_SFW,
	     "pclk_mscl_smmu_mscl_1_sfw", "dout_pclk_mscl",
	     ENABLE_PCLK_MSCL_SMMU_MSCL_1_SFW, 0, 0, 0),

	GATE(PCLK_MSCL_SMMU_G2D_MMU,
	     "pclk_mscl_smmu_g2d_mmu", "dout_pclk_mscl",
	     ENABLE_PCLK_MSCL_SMMU_G2D_MMU, 0, 0, 0),
	GATE(PCLK_MSCL_SMMU_G2D_SFW,
	     "pclk_mscl_smmu_g2d_sfw", "dout_pclk_mscl",
	     ENABLE_PCLK_MSCL_SMMU_G2D_SFW, 0, CLK_IGNORE_UNUSED, 0),

	GATE(SCLK_JPEG, "sclk_jpeg", "mout_aclk_mscl_532_user",
	     ENABLE_SCLK_MSCL, 0, 0, 0),
};

static const struct samsung_cmu_info mscl_cmu_info __initconst = {
	.mux_clks = mscl_mux_clks,
	.nr_mux_clks = ARRAY_SIZE(mscl_mux_clks),
	.div_clks = mscl_div_clks,
	.nr_div_clks = ARRAY_SIZE(mscl_div_clks),
	.gate_clks = mscl_gate_clks,
	.nr_gate_clks = ARRAY_SIZE(mscl_gate_clks),
	.nr_clk_ids = MSCL_NR_CLK,
	.clk_regs = mscl_clk_regs,
	.nr_clk_regs = ARRAY_SIZE(mscl_clk_regs),
};

static void __init exynos7_clk_mscl_init(struct device_node *np)
{
	samsung_cmu_register_one(np, &mscl_cmu_info);
}

CLK_OF_DECLARE(exynos7_clk_mscl, "samsung,exynos7-clock-mscl",
	       exynos7_clk_mscl_init);

/**********************\
* CMU_MFC (0x15280000) *
\**********************/

/* Register Offset definitions */
#define	MUX_SEL_MFC			0x0200
#define	DIV_MFC				0x0600
#define	ENABLE_ACLK_MFC			0x0800
#define	ENABLE_ACLK_SMMU_MFC_0		0x0804
#define	ENABLE_ACLK_SFW_MFC_0		0x0808
#define	ENABLE_ACLK_SMMU_MFC_1		0x080C
#define	ENABLE_ACLK_SFW_MFC_1		0x0810
#define	ENABLE_PCLK_MFC			0x0900
#define	ENABLE_PCLK_SMMU_MFC_0		0x0904
#define	ENABLE_PCLK_SFW_MFC_0		0x0908
#define	ENABLE_PCLK_SMMU_MFC_1		0x090C
#define	ENABLE_PCLK_SFW_MFC_1		0x0910

static const unsigned long mfc_clk_regs[] __initconst = {
	MUX_SEL_MFC,
	DIV_MFC,
	ENABLE_ACLK_MFC,
	ENABLE_ACLK_SMMU_MFC_0,
	ENABLE_ACLK_SFW_MFC_0,
	ENABLE_ACLK_SMMU_MFC_1,
	ENABLE_ACLK_SFW_MFC_1,
	ENABLE_PCLK_MFC,
	ENABLE_PCLK_SMMU_MFC_0,
	ENABLE_PCLK_SFW_MFC_0,
	ENABLE_PCLK_SMMU_MFC_1,
	ENABLE_PCLK_SFW_MFC_1,
};

/* List of parent clocks for Muxes  */
PNAME(mout_aclk_mfc_532_user_p) = {"fin_pll", "aclk_mfc_532"};

/* Muxes */
static const struct samsung_mux_clock mfc_mux_clks[] __initconst = {
	MUX(0, "mout_aclk_mfc_532_user", mout_aclk_mfc_532_user_p,
	    MUX_SEL_MFC, 0, 1),
};

/* Dividers */
static const struct samsung_div_clock mfc_div_clks[] __initconst = {
	DIV(0, "dout_pclk_mfc", "mout_aclk_mfc_532_user", DIV_MFC, 0, 2),
};

/* Clock gates */
static const struct samsung_gate_clock mfc_gate_clks[] __initconst = {
	GATE(0, "aclk_ahb2apb_mfc", "dout_pclk_mfc",
	     ENABLE_ACLK_MFC, 13, CLK_IGNORE_UNUSED, 0),
	GATE(ACLK_BTS_MFC_1, "aclk_bts_mfc_1", "mout_aclk_mfc_532_user",
	     ENABLE_ACLK_MFC, 15, 0, 0),
	GATE(ACLK_BTS_MFC_0, "aclk_bts_mfc_0", "mout_aclk_mfc_532_user",
	     ENABLE_ACLK_MFC, 16, 0, 0),
	GATE(ACLK_MFC, "aclk_mfc", "mout_aclk_mfc_532_user",
	     ENABLE_ACLK_MFC, 19, 0, 0),
	GATE(ACLK_LH_S_MFC_1, "aclk_lh_s_mfc_1", "mout_aclk_mfc_532_user",
	     ENABLE_ACLK_MFC, 30, 0, 0),
	GATE(ACLK_LH_S_MFC_0, "aclk_lh_s_mfc_0", "mout_aclk_mfc_532_user",
	     ENABLE_ACLK_MFC, 31, 0, 0),

	GATE(PCLK_BTS_1, "pclk_bts_1", "dout_pclk_mfc",
	     ENABLE_PCLK_MFC, 27, 0, 0),
	GATE(PCLK_BTS_0, "pclk_bts_0", "dout_pclk_mfc",
	     ENABLE_PCLK_MFC, 28, 0, 0),
	GATE(0, "pclk_pmu_mfc", "dout_pclk_mfc",
	     ENABLE_PCLK_MFC, 29, CLK_IGNORE_UNUSED, 0),
	GATE(0, "pclk_noc_mfc", "dout_pclk_mfc",
	     ENABLE_PCLK_MFC, 30, CLK_IGNORE_UNUSED, 0),
	GATE(PCLK_MFC, "pclk_mfc", "dout_pclk_mfc",
	     ENABLE_PCLK_MFC, 31, 0, 0),

	GATE(ACLK_SMMU_MFC_0, "aclk_smmu_mfc_0", "mout_aclk_mfc_532_user",
	     ENABLE_ACLK_SMMU_MFC_0, 0, 0, 0),
	GATE(PCLK_SMMU_MFC_0, "pclk_smmu_mfc_0", "dout_pclk_mfc",
	     ENABLE_PCLK_SMMU_MFC_0, 0, 0, 0),

	GATE(ACLK_SFW_MFC_0, "aclk_sfw_mfc_0", "mout_aclk_mfc_532_user",
	     ENABLE_ACLK_SFW_MFC_0, 0, 0, 0),
	GATE(PCLK_SFW_MFC_0, "pclk_sfw_mfc_0", "dout_pclk_mfc",
	     ENABLE_PCLK_SFW_MFC_0, 0, 0, 0),

	GATE(ACLK_SMMU_MFC_1, "aclk_smmu_mfc_1", "mout_aclk_mfc_532_user",
	     ENABLE_ACLK_SMMU_MFC_1, 0, 0, 0),
	GATE(PCLK_SMMU_MFC_1, "pclk_smmu_mfc_1", "dout_pclk_mfc",
	     ENABLE_PCLK_SMMU_MFC_1, 0, 0, 0),

	GATE(ACLK_SFW_MFC_1, "aclk_sfw_mfc_1", "mout_aclk_mfc_532_user",
	     ENABLE_ACLK_SFW_MFC_1, 0, 0, 0),
	GATE(PCLK_SFW_MFC_1, "pclk_sfw_mfc_1", "dout_pclk_mfc",
	     ENABLE_PCLK_SFW_MFC_1, 0, 0, 0),
};

static const struct samsung_cmu_info mfc_cmu_info __initconst = {
	.mux_clks = mfc_mux_clks,
	.nr_mux_clks = ARRAY_SIZE(mfc_mux_clks),
	.div_clks = mfc_div_clks,
	.nr_div_clks = ARRAY_SIZE(mfc_div_clks),
	.gate_clks = mfc_gate_clks,
	.nr_gate_clks = ARRAY_SIZE(mfc_gate_clks),
	.nr_clk_ids = MFC_NR_CLK,
	.clk_regs = mfc_clk_regs,
	.nr_clk_regs = ARRAY_SIZE(mfc_clk_regs),
};

static void __init exynos7_clk_mfc_init(struct device_node *np)
{
	samsung_cmu_register_one(np, &mfc_cmu_info);
}

CLK_OF_DECLARE(exynos7_clk_mfc, "samsung,exynos7-clock-mfc",
	       exynos7_clk_mfc_init);

/* Handling of auxiliary clocks: Clocks needed for normal operation
 * of a device but not used by the device's driver */
static void __init exynos7_clk_aux_init(struct device_node *np)
{
	int i = 0;
	struct clk *clk = NULL;

	for (i = 0; i < 20; i++) {
		clk = of_clk_get(np, i);
		if (IS_ERR(clk))
			break;
		clk_prepare_enable(clk);
	}
}

CLK_OF_DECLARE(exynos7_clk_aux, "samsung,exynos7-clock-aux",
	       exynos7_clk_aux_init);
