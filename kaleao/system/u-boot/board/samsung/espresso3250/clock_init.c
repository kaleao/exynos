/*
 * Clock setup for SMDK4270 board based on EXYNOS4
 *
 * Copyright (C) 2012 Samsung Electronics
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

#include <config.h>
#include <version.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/power.h>
#include <asm/arch/cpu.h>
#include <asm/arch/gpio.h>
#include "setup.h"

#include "smdk3250_val.h"


#define ASS_CLK_SRC		(0x03810000)
#define ASS_CLK_DIV		(0x03810004)
#define DIV_UNSTABLE		(0x1)
#define DIV_DMCP_OFFSET		(16)
#define MUX_G2D_ACP_SEL_OFFSET	(28)
#define PLL_ENABLE		(31)

void system_clock_init(void)
{
	struct exynos4_clock *clk = (struct exynos4_clock *)EXYNOS4_CLOCK_BASE;
	volatile unsigned int reg_val;

	/*
	** CPU Clock setting
	*/
	/* Set mux_cpu into fin MOUT_APLL = FIN*/
	writel((readl(&clk->src_cpu) & ~(1 << 0)), &clk->src_cpu);

	/* PLL LOCK */
	writel(APLL_LOCK_VAL, &clk->apll_lock);

	/* Set CMU_CPU, MUX & DIV */
	writel(CLK_DIV_CPU0_VAL, &clk->div_cpu0);
	writel(CLK_DIV_CPU1_VAL, &clk->div_cpu1);

	/* Set CMU_CPU MUX: CORE_CLK = MOUT_APLL */
	writel(0x01000000, &clk->src_cpu);

	/* Set APLL */
	writel(APLL_CON1_VAL, &clk->apll_con1);
	writel(APLL_CON2_VAL, &clk->apll_con2);
	writel(APLL_CON0_VAL, &clk->apll_con0);

	/* Enable APLL */
	reg_val = readl(&clk->apll_con0);
	reg_val = (reg_val) | (1 << PLL_ENABLE);
	writel(reg_val, &clk->apll_con0);
	/* Wait PLL lock */
	while ((readl(&clk->apll_con0) & (1 << 29)) == 0);

	/* Turn On PLL Mout: MOUT_APLL = FOUT_APLL*/
	reg_val = readl(&clk->src_cpu);
	reg_val = (reg_val) | (1 << 0);
	writel(reg_val, &clk->src_cpu);

	/* Check mux_apll status */
	while(((readl(&clk->mux_stat_cpu)) & (1 << 2)) != 0);	//wait MOUT_APLL stable

	/*
	** TOP Clock setting
	*/
	/* Set mux_mpll into fin:  MOUTMPLL = FIN */
	writel(CLK_SRC_TOP1_VAL_0, &clk->src_top1);

	/* Check mux status */
	while(((readl(&clk->mux_stat_top1)) & (1 << 14)) != 0);	//clk->mux_stat_top1.bit14

	/*set top MUX & DIV*/
	writel(CLK_SRC_TOP0_VAL_0, &clk->src_top0);
	writel(CLK_DIV_TOP_VAL, &clk->div_top);

	/* Set CMU_LEFTBUS, MUX & DIV */
	writel(CLK_SRC_LEFTBUS_VAL, &clk->src_leftbus);
	writel(CLK_DIV_LEFTBUS_VAL, &clk->div_leftbus);

	/* Set CMU_RIGHTBUS, MUX & DIV */
	writel(CLK_SRC_RIGHTBUS_VAL, &clk->src_rightbus);
	writel(CLK_DIV_RIGHTBUS_VAL, &clk->div_rightbus);

	writel(0x10000666, &clk->src_fsys);			//src_MMC = sclk_MPLL
	writel(0x00066666, &clk->src_peril0);			//src_uart = sclk_MPLL
	writel(0x00007777, &clk->div_peril0);
	writel(0x0f000f00, &clk->div_fsys1);
	writel(0x0f000f00, &clk->div_fsys2);

	/* Set MPLL */
	writel(MPLL_LOCK_VAL, &clk->mpll_lock);
	writel(MPLL_CON1_VAL, &clk->mpll_con1);
	writel(MPLL_CON2_VAL, &clk->mpll_con2);
	writel(MPLL_CON0_VAL, &clk->mpll_con0);
	/* Enable MPLL */
	reg_val = readl(&clk->mpll_con0);
	reg_val = (reg_val) | (1 << PLL_ENABLE);
	writel(reg_val, &clk->mpll_con0);
	/* Wait PLL lock */
	while ((readl(&clk->mpll_con0) & (1 << 29)) == 0);

	/* Set VPLL */
	writel(VPLL_LOCK_VAL, &clk->vpll_lock);
	writel(VPLL_CON1_VAL, &clk->vpll_con1);
	writel(VPLL_CON2_VAL, &clk->vpll_con2);
	writel(VPLL_CON0_VAL, &clk->vpll_con0);
	/* Enable VPLL */
	reg_val = readl(&clk->vpll_con0);
	reg_val = (reg_val) | (1 << PLL_ENABLE);
	writel(reg_val, &clk->vpll_con0);
	/* Wait PLL lock */
	while ((readl(&clk->vpll_con0) & (1 << 29)) == 0);

	/* Set mux_mpll into fin:  MOUTMPLL = FOUT_MPLL ; MOUTUPLL = FOUT_UPLL */
	writel(CLK_SRC_TOP1_VAL_1, &clk->src_top1);
	/* Check mux status */
	while(((readl(&clk->mux_stat_top1)) & (1 << 14)) != 0);	//clk->mux_stat_top1.bit14: MPLL
}
