/*
 * Clock setup for XYREF5260 board based on EXYNOS5260
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

void check_mux_stat(unsigned int *addr, unsigned int mux_stat)
{
	unsigned int mux_status;

	do {
		mux_status = readl(addr);
	} while(mux_status != mux_stat);
}

void check_pll_unlock(unsigned int *addr)
{
	unsigned int pll_unlock;

	do {
		pll_unlock = (readl(addr) >> 21) & 0x1;
	} while(!pll_unlock);

}

void system_clock_init()
{
	struct exynos5260_clock_egl *clk_egl =
		(struct exynos5260_clock_egl *) samsung_get_base_clock_egl();
	struct exynos5260_clock_kfc *clk_kfc =
		(struct exynos5260_clock_kfc *) samsung_get_base_clock_kfc();
	struct exynos5260_clock_mif *clk_mif =
		(struct exynos5260_clock_mif *) samsung_get_base_clock_mif();
	struct exynos5260_clock_top *clk_top =
		(struct exynos5260_clock_top *) samsung_get_base_clock_top();
	struct exynos5260_clock_fsys *clk_fsys =
		(struct exynos5260_clock_fsys *) samsung_get_base_clock_fsys();
	unsigned int ubits = 0;

	/* modify wrong reset value of ARM_EMA_CTRL */
	ubits = readl(&clk_kfc->arm_ema_ctrl);
	ubits &= ~((0x3 << 17) | (0x1 << 16) | (0x1 << 14));
	writel(ubits, &clk_kfc->arm_ema_ctrl);

	/* CMU_EGL clock init. */
	/* Step1. turn off pll fout */
	writel(0x0, &clk_egl->clk_mux_sel_egl);
	check_mux_stat(&clk_egl->clk_mux_stat_egl, 0x00001111);
	/* Step2. set pll locktime */
	writel(0x960, &clk_egl->egl_pll_lock);
	writel(0x960, &clk_egl->egl_dpll_lock);

	/* Step3. set pll enable & pms */
	ubits = (175 << 9) | (3 << 3) | (1 << 0);
	writel(ubits, &clk_egl->egl_pll_con0);
	writel(0x0082fe40, &clk_egl->egl_pll_con1);
	/* enable pll control */
	ubits |= (1 << 23);
	writel(ubits, &clk_egl->egl_pll_con0);
	check_pll_unlock(&clk_egl->egl_pll_con0);

	ubits = (75 << 9) | (1 << 3) | (1 << 0);
	writel(ubits, &clk_egl->egl_dpll_con0);
	/* enable pll control */
	ubits |= (1 << 23);
	writel(ubits, &clk_egl->egl_dpll_con0);
	check_pll_unlock(&clk_egl->egl_dpll_con0);

	/* Step4. set divider */
	/*
	 * CLK_DIV_EGL
	 * bit[24]:EGL_PLL, bit[20]:PCLK_DBG, bit[16]:ATCLK,
	 * bit[12]:PCLK_EGL, bit[8]:ACLK_EGL, bit[4]:EGL2, bit[0]:EGL1
	 */
	ubits = (3 << 24) | (0 << 20) | (3 << 16) |
		(0 << 12) | (1 << 8) | (0 << 4) | (0 << 0);
	writel(ubits, &clk_egl->clk_div_egl);

	/* CMU_KFC clock init. */
	/* Step1. turn off pll fout */
	writel(0x0, &clk_kfc->clk_mux_sel_kfc0);
	check_mux_stat(&clk_kfc->clk_mux_stat_kfc0, 0x00000001);
	writel(0x0, &clk_kfc->clk_mux_sel_kfc2);
	check_mux_stat(&clk_kfc->clk_mux_stat_kfc2, 0x00000001);

	/* Step2. set pll locktime */
	writel(0x960, &clk_kfc->kfc_pll_lock);

	/* Step3. set pll enable & pms */
	ubits = (400 << 9) | (4 << 3) | (2 << 0);
	writel(ubits, &clk_kfc->kfc_pll_con0);
	/* enable pll control */
	ubits |= (1 << 23);
	writel(ubits, &clk_kfc->kfc_pll_con0);
	check_pll_unlock(&clk_kfc->kfc_pll_con0);

	/* Step4. set divider */
	/*
	 * CLK_DIV_KFC
	 * bit[24]:KFC_PLL, bit[20]:PCLK_KFC, bit[16]:ACLK_KFC,
	 * bit[12]:PCLK_DBG, bit[8]:ATCLK, bit[4]:KFC2, bit[0]:KFC1
	 */
	ubits = (1 << 24) | (3 << 20) | (1 << 16) |
		(3 << 12) | (3 << 8) | (0 << 4) | (0 << 0);
	writel(ubits, &clk_kfc->clk_div_kfc);

	/* CMU_MIF clock init. */
	/* Step1. turn off pll fout */
	writel(0x0, &clk_mif->clk_mux_sel_mif);
	check_mux_stat(&clk_mif->clk_mux_stat_mif, 0x01111111);

	/* Step2. set pll locktime */
	writel(0x960, &clk_mif->mem_pll_lock);
	writel(0x960, &clk_mif->bus_pll_lock);
	writel(0x960, &clk_mif->media_pll_lock);

	/* Step3. set pll enable & pms */
	ubits = (400 << 9) | (4 << 3) | (3<< 0);
	writel(ubits, &clk_mif->mem_pll_con0);
	/* enable pll control */
	ubits |= (1 << 23);
	writel(ubits, &clk_mif->mem_pll_con0);
	check_pll_unlock(&clk_mif->mem_pll_con0);

	ubits = (200 << 9) | (3 << 3) | (1 << 0);
	writel(ubits, &clk_mif->bus_pll_con0);
	/* enable pll control */
	ubits |= (1 << 23);
	writel(ubits, &clk_mif->bus_pll_con0);
	check_pll_unlock(&clk_mif->bus_pll_con0);

	ubits = (667 << 9) | (12 << 3) | (1 << 0);
	writel(ubits, &clk_mif->media_pll_con0);
	/* enable pll control */
	ubits |= (1 << 23);
	writel(ubits, &clk_mif->media_pll_con0);
	check_pll_unlock(&clk_mif->media_pll_con0);
	/* Step4. set divider */
	/*
	 * CLK_DIV_MIF
	 * bit[28]:ACLK_BUS_100_RATIO, bit[24]:ACLK_BUS_200_RATIO,
	 * bit[20]:ACLK_MIF_466_RATIO, bit[16]:CLK2X_PHY_RATIO,
	 * bit[12]:CLKM_PHY_RATIO, bit[8]:BUS_PLL_RATIO, bit[4]:MEM_PLL_RATIO,
	 * bit[0]:MEDIA_PLL_RATIO
	 */
	ubits = (7 << 28) | (3 << 24) | (1 << 20) | (0 << 16) |
		(0 << 12) | (0 << 8) | (0 << 4) | (0 << 0);
	writel(ubits, &clk_mif->clk_div_mif);

	/* CMU_TOP clock init. */
	/* Step1. turn off pll fout */
	writel(0x0, &clk_top->clk_mux_sel_top_pll0);
	check_mux_stat(&clk_top->clk_mux_stat_top_pll0, 0x01011111);

	writel(0x0, &clk_top->clk_mux_sel_top_fsys);
	check_mux_stat(&clk_top->clk_mux_stat_top_fsys, 0x01111111);

	writel(0x0, &clk_top->clk_mux_sel_top_peri1);
	check_mux_stat(&clk_top->clk_mux_stat_top_peri1, 0x00111111);

	/* Step2. set pll locktime */
	writel(600, &clk_top->disp_pll_lock);

	/* Step3. set pll enable & pms */
	ubits = ((266 << 9) | (3 << 3) | (2 << 0));
	writel(ubits, &clk_top->disp_pll_con0);
	/* enable pll control */
	ubits |= (1 << 23);
	writel(ubits, &clk_top->disp_pll_con0);
	check_pll_unlock(&clk_top->disp_pll_con0);

	/* Step4. set divider */
	/*
	 * CLK_DIV_TOP_HPM
	 * bit[0]:SCLK_HPM_TARGETCLK_RATIO
	 */
	ubits = (3 << 0);
	writel(ubits, &clk_top->clk_div_top_hpm);

	/* CLK_DIV_TOP_FSYS0, 1 */
	writel(0x34003, &clk_top->clk_div_top_fsys0);
	writel(0x34000, &clk_top->clk_div_top_fsys1);

	/* CLK_DIV_TOP_PERI0, 1 */
	writel(0x70070, &clk_top->clk_div_top_peri0);
	writel(0x07770070, &clk_top->clk_div_top_peri1);
	writel(0x02b00000, &clk_top->clk_div_top_peri2);

	/*
	 * CLK_DIV_TOP_BUS
	 * bit[28]:ACLK_BUS4_100_RATIO, bit[24]:ACLK_BUS4_400_RATIO,
	 * bit[20]:ACLK_BUS3_100_RATIO, bit[16]:ACLK_BUS3_400_RATIO,
	 * bit[12]:ACLK_BUS2_100_RATIO, bit[8]:ACLK_BUS2_400_RATIO,
	 * bit[4]:ACLK_BUS1_100_RATIO, bit[0]:ACLK_BUS1_400_RATIO
	 */
	ubits = (7 << 28) | (1 << 24) | (7 << 20) | (1 << 16) |
		(7 << 12) | (1 << 8) | (7 << 4) | (1 << 0);
	writel(ubits, &clk_top->clk_div_top_bus);

	/* Step5. set mux */
	/* set TOP BUS */
	writel(0x0, &clk_top->clk_mux_sel_top_bus);
	check_mux_stat(&clk_top->clk_mux_stat_top_bus, 0x11111111);

	/* Turn on pll fout */
	/* CMU_EGL */
	writel(0x11, &clk_egl->clk_mux_sel_egl);
	check_mux_stat(&clk_egl->clk_mux_stat_egl, 0x00001122);

	/* CMU_KFC */
	writel(0x1, &clk_kfc->clk_mux_sel_kfc0);
	check_mux_stat(&clk_kfc->clk_mux_stat_kfc0, 0x00000002);

	/* CMU_MIF */
	writel(0x01111111, &clk_mif->clk_mux_sel_mif);
	check_mux_stat(&clk_mif->clk_mux_stat_mif, 0x02222222);

	/* CMU_TOP */
	writel(0x00001111, &clk_top->clk_mux_sel_top_pll0);
	check_mux_stat(&clk_top->clk_mux_stat_top_pll0, 0x01012222);

	writel(0x01111110, &clk_top->clk_mux_sel_top_fsys);
	check_mux_stat(&clk_top->clk_mux_stat_top_fsys, 0x02222221);

	writel(0x00111111, &clk_top->clk_mux_sel_top_peri1);
	check_mux_stat(&clk_top->clk_mux_stat_top_peri1, 0x00222222);
}
