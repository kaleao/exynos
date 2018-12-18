/*
 * Copyright (C) 2010 Samsung Electronics
 * Minkyu Kang <mk7.kang@samsung.com>
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

#ifndef CONFIG_SPL_BUILD

#include <common.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/clk.h>

static unsigned long exynos5430_get_pll_clk(int pllreg)
{
	struct exynos5430_clock_egl *clk_egl =
		(struct exynos5430_clock_egl *)exynos5430_get_base_clock_egl();
	struct exynos5430_clock_kfc *clk_kfc =
		(struct exynos5430_clock_kfc *)exynos5430_get_base_clock_kfc();
	struct exynos5430_clock_mif *clk_mif =
		(struct exynos5430_clock_mif *)exynos5430_get_base_clock_mif();
	struct exynos5430_clock_top *clk_top =
		(struct exynos5430_clock_top *)exynos5430_get_base_clock_top();
	struct exynos5430_clock_cpif *clk_cpif =
				(struct exynos5430_clock_cpif *)
					    exynos5430_get_base_clock_cpif();
	unsigned long r, m, p, s, k = 0, mask, fout;
	unsigned int pll_ratio = 0;
	unsigned int bus_pll_sub_sel = 0;
	unsigned int freq;

	switch (pllreg) {
	case EGL_PLL:
		r = readl(&clk_egl->egl_pll_con0);
		break;
	case EGL_DPLL:
		r = readl(&clk_egl->egl_dpll_con0);
		break;
	case KFC_PLL:
		r = readl(&clk_kfc->kfc_pll_con0);
		break;
	case MEM_PLL:
		r = readl(&clk_mif->mem_pll_con0);
		pll_ratio = (readl(&clk_mif->clk_div_mif0) >> 4) & 0x3;
		break;
	case BUS_PLL:
#ifdef CONFIG_CPU_EXYNOS5433
		/* BUS_PLL_CON0 */
		r = readl(0x105b0120);
#else
		bus_pll_sub_sel = readl(&clk_mif->clk_mux_sel_mif1);
		if (bus_pll_sub_sel & 0x1) {
			r = readl(&clk_mif->bus_pll_con0);
			pll_ratio = readl(&clk_mif->clk_div_mif0) & 0x3;
		} else {
			r = readl(&clk_mif->bus_dpll_con0);
		}
#endif
		break;
	case MFC_PLL:
		r = readl(&clk_mif->mfc_pll_con0);
		pll_ratio = (readl(&clk_mif->clk_div_mif0) >> 8) & 0x3;
		break;
	case ISP_PLL:
		r = readl(&clk_top->isp_pll_con0);
		break;
	case MPHY_PLL:
		r = readl(&clk_cpif->mphy_pll_con0);
		break;
	default:
		printf("Unsupported PLL (%d)\n", pllreg);
		return 0;
	}

	/*
	 * PLL_CON: MIDV [25:16]
	 */
	mask = 0x3ff;
	m = (r >> 16) & mask;

	/* PDIV [13:8] */
	p = (r >> 8) & 0x3f;
	/* SDIV [2:0] */
	s = r & 0x7;

	freq = CONFIG_SYS_CLK_FREQ;

	/* FOUT = MDIV * FIN / (PDIV * 2^(SDIV)) */
	fout = m * (freq / (p * (1 << s)));
	fout = fout / ((unsigned long)pll_ratio + 1);

	return fout;
}

static unsigned long exynos7420_get_pll_clk(int pllreg)
{
	struct exynos7420_clock_atlas *clk_atlas = exynos7420_get_base_clock_atlas();
	struct exynos7420_clock_apollo *clk_apollo = exynos7420_get_base_clock_apollo();
	struct exynos7420_clock_topc *clk_topc = exynos7420_get_base_clock_topc();
	unsigned long r, m, p, s = 0, fout;
	unsigned int pll_ratio = 0;
	unsigned int freq;

	switch (pllreg) {
	case ATLAS_PLL:
		r = readl(&clk_atlas->atlas_pll_con0);
		break;
	case APOLLO_PLL:
		r = readl(&clk_apollo->apollo_pll_con0);
		break;
	case MIF_PLL:
		r = readl(&clk_topc->mif_pll_con0);
		break;
	case BUS0_PLL:
		r = readl(&clk_topc->bus0_pll_con0);
		pll_ratio = (readl(&clk_topc->clk_div_topc3) >> 0) & 0x7;
		break;
	case BUS1_PLL:
		r = readl(&clk_topc->bus1_pll_con0);
		pll_ratio = (readl(&clk_topc->clk_div_topc3) >> 8) & 0x7;
		break;
	case MFC_PLL:
		r = readl(&clk_topc->mfc_pll_con0);
		pll_ratio = (readl(&clk_topc->clk_div_topc3) >> 16) & 0x7;
		break;
	case CCI_PLL:
		r = readl(&clk_topc->cci_pll_con0);
		pll_ratio = (readl(&clk_topc->clk_div_topc3) >> 12) & 0x7;
		break;
	default:
		printf("Unsupported PLL (%d)\n", pllreg);
		return 0;
	}

	/* MIDV [25:16] */
	m = (r >> 16) & 0x3ff;
	/* PDIV [13:8] */
	p = (r >> 8) & 0x3f;
	/* SDIV [2:0] */
	s = r & 0x7;

	freq = CONFIG_SYS_CLK_FREQ;

	/* FOUT = MDIV * FIN / (PDIV * 2^(SDIV)) */
	fout = m * (freq / (p * (1 << s)));
	fout = fout / ((unsigned long)pll_ratio + 1);

	return fout;
}

static unsigned long exynos7580_get_pll_clk(int pllreg)
{
        struct exynos7580_clock_cpu *clk_cpu = exynos7580_get_base_clock_cpu();
        struct exynos7580_clock_apl *clk_apl = exynos7580_get_base_clock_apl();
        struct exynos7580_clock_mif *clk_mif = exynos7580_get_base_clock_mif();
        struct exynos7580_clock_fsys *clk_fsys = exynos7580_get_base_clock_fsys();
        unsigned long r, m, p, s = 0, fout;
        unsigned int pll_ratio = 0;
        unsigned int freq;

        switch (pllreg) {
        case PLL_CPU:
                r = readl(&clk_cpu->cpu_pll_con0);
                break;
        case PLL_APL:
                r = readl(&clk_apl->apl_pll_con0);
                break;
        case PLL_MEM0:
                r = readl(&clk_mif->mem0_pll_con0);
                break;
        case PLL_MEDIA:
                r = readl(&clk_mif->media_pll_con0);
                break;
        case PLL_BUS:
                r = readl(&clk_mif->bus_pll_con0);
                break;
        case PLL_USB:
                r = readl(&clk_fsys->usb_pll_con0);
                break;
        default:
                printf("Unsupported PLL (%d)\n", pllreg);
                return 0;
        }

        /* MIDV [21:12] */
        m = (r >> 12) & 0x3ff;
        /* PDIV [9:4] */
        p = (r >> 4) & 0x3f;
        /* SDIV [2:0] */
        s = r & 0x7;

        freq = CONFIG_SYS_CLK_FREQ;

        /* FOUT = MDIV * FIN / (PDIV * 2^(SDIV)) */
        fout = m * (freq / (p * (1 << s)));
        fout = fout / ((unsigned long)pll_ratio + 1);

        return fout;
}

/* exynos5430: return pwm clock frequency */
static unsigned long exynos5430_get_pwm_clk(void)
{
	struct exynos5430_clock_top *clk =
		(struct exynos5430_clock_top *)exynos5430_get_base_clock_top();
	unsigned long pclk;
	unsigned int clk_div_top3;
	unsigned int aclk_peri_66_ratio_a, aclk_peri_66_ratio_b;

	/*
	 * CLK_DIV_TOP3
	 * ACLK_PERI_66_RATIO_A[10:8], ACLK_PERI_66_RATIO_B[14:12]
	 */
	clk_div_top3 = readl(&clk->clk_div_top3);
	aclk_peri_66_ratio_a = ((clk_div_top3 >> 8) & 0x7) + 1;
	aclk_peri_66_ratio_b = ((clk_div_top3 >> 12) & 0x7) + 1;

	pclk = get_pll_clk(BUS_PLL) / (unsigned long)aclk_peri_66_ratio_a;
	pclk = pclk / (unsigned long)aclk_peri_66_ratio_b;

	return pclk;
}

/* exynos7420: return pwm clock frequency */
static unsigned long exynos7420_get_pwm_clk(void)
{
	struct exynos7420_clock_top0 *clk_top0 =
		(struct exynos7420_clock_top0 *)exynos7420_get_base_clock_top0();
	struct exynos7420_clock_topc *clk_topc =
		(struct exynos7420_clock_topc *)exynos7420_get_base_clock_topc();
	unsigned int sel, ratio, pclk;

	/*
	 * CLK_MUX_SEL_TOP03
	 * ACKL_PERIC0_66 [21:20]
	 *
	 * CLK_DIV_TOP03
	 * ACLK_PERIC0_66[25:20]
	 */
	sel = readl(&clk_top0->clk_mux_sel_top03);
	ratio = readl(&clk_top0->clk_div_top03);

	sel = (sel >> 20) & 0x3;
	ratio = (ratio >> 20) & 0x3f;

	if (sel == 0) {
		pclk = get_pll_clk(BUS0_PLL);
		sel = readl(&clk_top0->clk_mux_sel_top01);
		if ((sel >> 16) & 0x1) pclk = pclk / 2;
		sel = readl(&clk_topc->clk_mux_sel_topc1);
		if ((sel >> 16) & 0x1) pclk = pclk / 2;
	} else if (sel == 1) {
		pclk = get_pll_clk(BUS1_PLL);
		sel = readl(&clk_top0->clk_mux_sel_top01);
		if ((sel >> 12) & 0x1) pclk = pclk / 2;
	} else if (sel == 2) {
		pclk = get_pll_clk(CCI_PLL);
		sel = readl(&clk_top0->clk_mux_sel_top01);
		if ((sel >> 8) & 0x1) pclk = pclk / 2;
	} else if (sel == 3) {
		pclk = get_pll_clk(MFC_PLL);
		sel = readl(&clk_top0->clk_mux_sel_top01);
		if ((sel >> 4) & 0x1) pclk = pclk / 2;
	} else {
		printf("Unsupported pwm mux (%d)\n", sel);
		return 0;
	}

	pclk = pclk / (ratio + 1);

	return pclk;
}

/* exynos7580: return pwm clock frequency */
static unsigned long exynos7580_get_pwm_clk(void)
{
        struct exynos7580_clock_top *clk =
                (struct exynos7580_clock_top *)exynos7580_get_base_clock_top();
        unsigned long pclk;
        unsigned int clk_div_top0;
        unsigned int aclk_peri_66_ratio;

        /*
         * CLK_DIV_TOP0
         * ACLK_PERI_66_RATIO[31:28]
         */
        clk_div_top0 = readl(&clk->clk_div_top0);
        aclk_peri_66_ratio = ((clk_div_top0 >> 28) & 0xf) + 1;

        pclk = get_pll_clk(BUS_PLL) / (unsigned long)aclk_peri_66_ratio;

        // print pclk for test
        printf("pwm pclk : %ldHZ\n", pclk);

        return pclk;
}

/* exynos5430: return uart clock frequency */
static unsigned long exynos5430_get_uart_clk(int dev_index)
{
	struct exynos5430_clock_top *clk =
		(struct exynos5430_clock_top *)exynos5430_get_base_clock_top();
	unsigned long uclk, sclk;
	unsigned int sel;
	unsigned int ratio;

	/*
	 * CLK_MUX_SEL_TOP_PERI1
	 * - UART0_SEL [20], UART1_SEL [12], UART2_SEL [16]
	 *
	 * CLK_DIV_TOP_PERI1
	 * - UART0_RATIO [27:24], UART1_RATIO [19:16], UART2_RATIO [23:20]
	 */
#ifdef CONFIG_CPU_EXYNOS5433
	/* CLK_MUX_SEL_TOP_PERIC0 */
	sel = readl(0x10030238);
#else
	sel = readl(&clk->clk_mux_sel_top_peric0);
#endif
	ratio = readl(&clk->clk_div_top_peric2);

	switch (dev_index) {
	case 0:
		sel = (sel >> 12) & 0x1;
		ratio = (ratio >> 0) & 0xf;
		break;
	case 1:
		sel = (sel >> 16) & 0x1;
		ratio = (ratio >> 4) & 0xf;
		break;
	case 2:
		sel = (sel >> 20) & 0x1;
		ratio = (ratio >> 8) & 0xf;
		break;
	}

	if (sel) {
		sclk = get_pll_clk(BUS_PLL);
	} else {
		sclk = CONFIG_SYS_CLK_FREQ;
	}

	uclk = sclk / (ratio + 1);

	return uclk;
}

/* exynos7420: return uart clock frequency */
static unsigned long exynos7420_get_uart_clk(int dev_index)
{
	struct exynos7420_clock_top0 *clk_top0 =
		(struct exynos7420_clock_top0 *)exynos7420_get_base_clock_top0();
	struct exynos7420_clock_topc *clk_topc =
		(struct exynos7420_clock_topc *)exynos7420_get_base_clock_topc();
	unsigned int uclk, sclk;
	unsigned int sel, ratio;

	/*
	 * CLK_MUX_SEL_TOP0_PERIC3
	 * - UART0_SEL [17:16], UART1_SEL [13:12]
	 * - UART2_SEL [9:8], UART3_SEL [5:4]
	 *
	 * CLK_DIV_TOP0_PERIC3
	 * - UART0_RATIO [19:16], UART1_RATIO [15:12]
	 * - UART2_RATIO [11:8], UART3_RATIO [7:4]
	 */
	sel = readl(&clk_top0->clk_mux_sel_top0_peric3);
	ratio = readl(&clk_top0->clk_div_top0_peric3);

	sel = (sel >> ((4 - dev_index) * 4)) & 0x3;
	ratio = (ratio >> ((4 - dev_index) * 4)) & 0xf;

	if (sel == 0) {
		sclk = get_pll_clk(BUS0_PLL);
		sel = readl(&clk_top0->clk_mux_sel_top01);
		if ((sel >> 16) & 0x1) sclk = sclk / 2;
		sel = readl(&clk_topc->clk_mux_sel_topc1);
		if ((sel >> 16) & 0x1) sclk = sclk / 2;
	} else if (sel == 1) {
		sclk = get_pll_clk(BUS1_PLL);
		sel = readl(&clk_top0->clk_mux_sel_top01);
		if ((sel >> 12) & 0x1) sclk = sclk / 2;
	} else if (sel == 2) {
		sclk = get_pll_clk(CCI_PLL);
		sel = readl(&clk_top0->clk_mux_sel_top01);
		if ((sel >> 8) & 0x1) sclk = sclk / 2;
	} else if (sel == 3) {
		sclk = get_pll_clk(MFC_PLL);
		sel = readl(&clk_top0->clk_mux_sel_top01);
		if ((sel >> 4) & 0x1) sclk = sclk / 2;
	} else {
		printf("Unsupported uart mux (%d)\n", sel);
		return 0;
	}

	uclk = sclk / (ratio + 1);

	return uclk;
}

static unsigned long exynos7580_get_uart_clk(int dev_index)
{
        struct exynos7580_clock_top *clk =
                (struct exynos7580_clock_top *)exynos7580_get_base_clock_top();
        unsigned long uclk, sclk;
        unsigned int sel;
        unsigned int ratio;

        /*
         * CLK_MUX_SEL_TOP_PERI
         * - UART0_SEL [20], UART1_SEL [24], UART2_SEL [28]
         *
         * CLK_DIV_TOP_PERI3
         * - UART0_RATIO [19:16], UART1_RATIO [23:20], UART2_RATIO [27:24]
         */

        sel = readl(&clk->clk_mux_sel_top_peri);
        ratio = readl(&clk->clk_div_top_peri3);

        switch (dev_index) {
        case 0:
                sel = (sel >> 20) & 0x1;
                ratio = (ratio >> 16) & 0xf;
                break;
        case 1:
                sel = (sel >> 24) & 0x1;
                ratio = (ratio >> 20) & 0xf;
                break;
        case 2:
                sel = (sel >> 28) & 0x1;
                ratio = (ratio >> 24) & 0xf;
                break;
        }

        if (sel) {
                sclk = get_pll_clk(BUS_PLL);
        } else {
                sclk = CONFIG_SYS_CLK_FREQ;
        }

        uclk = sclk / (ratio + 1);

        return uclk;
}

static unsigned long exynos5430_get_usbdrd_clk(void)
{
	struct exynos5430_clock_top *clk =
		(struct exynos5430_clock_top *)exynos5430_get_base_clock_top();
	unsigned int addr;
	unsigned int ratio;
	unsigned long sclk;

	sclk = CONFIG_SYS_CLK_FREQ;

	/*
	 * CLK_DIV_TOP_FSYS2
	 * SCLK_USBDRD30_RATIO[3:0]
	 */
	addr = (unsigned int)&clk->clk_div_top_fsys2;
	ratio = readl(addr);

	ratio = ratio & 0xff;

	return (sclk / (ratio + 1));
}

static unsigned long exynos7420_get_usbdrd_clk(void)
{
	struct exynos7420_clock_fsys0 *clk =
		(struct exynos7420_clock_fsys0 *)exynos7420_get_base_clock_fsys0();
	unsigned int addr;
	unsigned int ratio;
	unsigned long sclk;

	sclk = CONFIG_SYS_CLK_FREQ;

	/*
	 * CLK_DIV_FSYS0
	 * USB20_RATIO[7:4]
	 */
	addr = (unsigned int)&clk->clk_div_fsys0;
	ratio = readl(addr);

	ratio = (ratio >> 4) & 0xf;

	return (sclk / (ratio + 1));
}

/* exynos5430: set the mmc clock */
static void exynos5430_set_mmc_clk(int dev_index, unsigned int div)
{
	struct exynos5430_clock_top *clk =
		(struct exynos5430_clock_top *)exynos5430_get_base_clock_top();
	unsigned int ratio, addr;

	/*
	 * CLK_DIV_TOP_FSYS0
	 * SCLK_MMC1_B_RATIO[23:16], SCLK_MMC1_A_RATIO[15:12]
	 * SCLK_MMC0_B_RATIO[11:4], SCLK_MMC0_A_RATIO[3:0]
	 * CLK_DIV_TOP_FSYS1
	 * SCLK_MMC2_B_RATIO[11:4], SCLK_MMC2_A_RATIO[3:0]
	 */
	if (dev_index < 2) {
		addr = (unsigned int)&clk->clk_div_top_fsys0;
		ratio = readl(addr);
		/* clear SCLK_MMCx_A_RATIO */
		ratio &= ~(0xf << (dev_index * 12));
		/* clear SCLK_MMCx_B_RATIO */
		ratio &= ~(0xff << ((dev_index * 12) + 4));
		/* set SCLK_MMCx_B_RATIO */
		ratio |= ((div & 0xff) << ((dev_index * 12) + 4));
	} else {
		addr = (unsigned int)&clk->clk_div_top_fsys1;
		ratio = readl(addr);
		/* clear SCLK_MMCx_A_RATIO */
		ratio &= ~(0xf);
		/* clear SCLK_MMCx_B_RATIO */
		ratio &= ~(0xff << 4);
		/* set SCLK_MMCx_B_RATIO */
		ratio |= ((div & 0xff) << 4);
	}

	writel(ratio, addr);
}

static void exynos7420_set_mmc_clk(int dev_index, unsigned int div)
{
	struct exynos7420_clock_top1 *clk =
		(struct exynos7420_clock_top1 *)exynos7420_get_base_clock_top1();
	unsigned int ratio, addr;

	/*
	 * CLK_DIV_TOP1_FSYS0
	 * SCLK_MMC2[25:16]
	 * CLK_DIV_TOP1_FSYS1
	 * SCLK_MMC0[21:12]
	 * SCLK_MMC1[9:0]
	 */
	if (dev_index < 2) {
#ifdef CONFIG_CPU_EXYNOS7420_EVT1
		addr = (unsigned int)&clk->clk_div_top1_fsys11;
		ratio = readl(addr);
		/* clear SCLK_MMCx_RATIO */
		ratio &= ~(0x3ff << (1 - dev_index) * 12);
		/* set SCLK_MMCx_RATIO */
		ratio |= ((div & 0x3ff) << (1 - dev_index) * 12);
#else
		addr = (unsigned int)&clk->clk_div_top1_fsys1;
		ratio = readl(addr);
		/* clear SCLK_MMCx_RATIO */
		ratio &= ~(0xf << (24 + (1 - dev_index) * 4));
		/* set SCLK_MMCx_RATIO */
		ratio |= ((div & 0xf) << (24 + (1 - dev_index) * 4));
#endif
	} else {
#ifdef CONFIG_CPU_EXYNOS7420_EVT1
		addr = (unsigned int)&clk->clk_div_top1_fsys0;
		ratio = readl(addr);
		/* clear SCLK_MMCx_RATIO */
		ratio &= ~(0x3ff << 16);
		/* set SCLK_MMCx_RATIO */
		ratio |= ((div & 0x3ff) << 16);
#else
		addr = (unsigned int)&clk->clk_div_top1_fsys0;
		ratio = readl(addr);
		/* clear SCLK_MMCx_RATIO */
		ratio &= ~(0xf << 24);
		/* set SCLK_MMCx_RATIO */
		ratio |= ((div & 0xf) << 24);
#endif
	}

	writel(ratio, addr);
}

/* exynos7580: set the mmc clock */
static void exynos7580_set_mmc_clk(int dev_index, unsigned int div)
{
        struct exynos7580_clock_top *clk =
                (struct exynos7580_clock_top *)exynos7580_get_base_clock_top();
        unsigned int ratio, addr;

        /*
         * CLK_DIV_TOP_FSYS0
         * SCLK_FSYS_MMC0_SDCLKIN_B_RATIO[11:4], SCLK_FSYS_MMC0_SDCLKIN_A_RATIO[3:0]
         * CLK_DIV_TOP_FSYS1
         * SCLK_FSYS_MMC1_SDCLKIN_B_RATIO[11:4], SCLK_FSYS_MMC1_SDCLKIN_A_RATIO[3:0]
         * CLK_DIV_TOP_FSYS2
         * SCLK_FSYS_MMC2_SDCLKIN_B_RATIO[11:4], SCLK_FSYS_MMC2_SDCLKIN_A_RATIO[3:0]
         */
        if (dev_index == 0) {
                addr = (unsigned int)&clk->clk_div_top_fsys0;
        } else if (dev_index == 1) {
                addr = (unsigned int)&clk->clk_div_top_fsys1;
        } else {
                addr = (unsigned int)&clk->clk_div_top_fsys2;
        }

        ratio = readl(addr);
        /* clear SCLK_FSYS_MMCx_SDCLKIN_A_RATIO */
        ratio &= ~(0xf);
        /* clear SCLK_FSYS_MMCx_SDCLKIN_B_RATIO */
        ratio &= ~(0xff << 4);
        /* set SCLK_FSYS_MMCx_SDCLKIN_B_RATIO */
        ratio |= ((div & 0xff) << 4);

        writel(ratio, addr);
}

static void exynos7420_set_ufs_clk(int dev_index)
{
	return ;
}

/* exynos5430: get the mmc clock */
#define MMC0_SRC_MASK		0x1111
#define MMCx_SRC_MASK		0x0011
#define MMC_SRC_ISP_PLL		0x1000
#define MMC_SRC_MPHY_PLL_USER	0x0100
#define MMC_SRC_MFC_PLL_USER	0x0010
#define MMC_SRC_BUS_PLL_USER	0x0001

static unsigned long exynos5430_get_mmc_clk(int dev_index)
{
	struct exynos5430_clock_top *clk =
		(struct exynos5430_clock_top *)exynos5430_get_base_clock_top();
	unsigned int sel, sel_mmc;
	unsigned int ratio, ratio_a, ratio_b;
	unsigned long sclk;

#ifdef CONFIG_CPU_EXYNOS5433
	/* CLK_MUX_SEL_TOP_FSYS0 */
	sel = readl(0x10030230);
#else
	sel = readl(&clk->clk_mux_sel_top_fsys0);
#endif

	if (dev_index == 0) {
		sel_mmc = sel & MMC0_SRC_MASK;
	} else {
		sel_mmc = (sel >> (24 - ((2 - dev_index) * 8))) & MMCx_SRC_MASK;
	}

	/* get src pll clock */
	if (sel_mmc & MMC_SRC_ISP_PLL) {
		sclk = get_pll_clk(ISP_PLL);
	} else if(sel_mmc & MMC_SRC_MPHY_PLL_USER) {
		sclk = get_pll_clk(MPHY_PLL);
	} else if(sel_mmc & MMC_SRC_MFC_PLL_USER) {
		sclk = get_pll_clk(MFC_PLL);
	} else if(sel_mmc & MMC_SRC_BUS_PLL_USER) {
		sclk = get_pll_clk(BUS_PLL);
	} else {
		sclk = CONFIG_SYS_CLK_FREQ;
	}

	/*
	 * CLK_DIV_TOP_FSYS0
	 * SCLK_MMC1_B_RATIO[23:16], SCLK_MMC1_A_RATIO[15:12]
	 * SCLK_MMC0_B_RATIO[11:4], SCLK_MMC0_A_RATIO[3:0]
	 * CLK_DIV_TOP_FSYS1
	 * SCLK_MMC2_B_RATIO[11:4], SCLK_MMC2_A_RATIO[3:0]
	 */
	if (dev_index < 2) {
		ratio = readl(&clk->clk_div_top_fsys0);
		/* get SCLK_MMCx_A_RATIO */
		ratio_a = (ratio >> (dev_index * 12)) & 0xf;
		/* get SCLK_MMCx_B_RATIO */
		ratio_b = (ratio >> ((dev_index * 12) + 4)) & 0xff;
	} else {
		ratio = readl(&clk->clk_div_top_fsys1);
		/* get SCLK_MMCx_A_RATIO */
		ratio_a = (ratio >> 0) & 0xf;
		/* get SCLK_MMCx_B_RATIO */
		ratio_b = (ratio >> 4) & 0xff;
	}

	return (sclk / (ratio_a + 1)) / (ratio_b + 1);
}

static unsigned long exynos7420_get_mmc_clk(int dev_index)
{
	struct exynos7420_clock_top1 *clk =
		(struct exynos7420_clock_top1 *)exynos7420_get_base_clock_top1();
	struct exynos7420_clock_top0 *clk_top0 =
		(struct exynos7420_clock_top0 *)exynos7420_get_base_clock_top0();
	struct exynos7420_clock_topc *clk_topc =
		(struct exynos7420_clock_topc *)exynos7420_get_base_clock_topc();
	unsigned int sel, sel_mmc;
	unsigned int ratio;
	unsigned long sclk;

	if (dev_index < 2) {
#ifdef CONFIG_CPU_EXYNOS7420_EVT1
		sel = readl(&clk->clk_mux_sel_top1_fsys11);
		sel_mmc = (sel >> (12 * (1 - dev_index))) & 0x3;
#else
		sel = readl(&clk->clk_mux_sel_top1_fsys1);
		sel_mmc = (sel >> (24 + (1 - dev_index) * 4)) & 0x3;
#endif
	} else {
#ifdef CONFIG_CPU_EXYNOS7420_EVT1
		sel = readl(&clk->clk_mux_sel_top1_fsys0);
		sel_mmc = (sel >> 16) & 0x3;
#else
		sel = readl(&clk->clk_mux_sel_top1_fsys0);
		sel_mmc = (sel >> 24) & 0x3;
#endif
	}

	/* get src pll clock */
	if (sel_mmc == 0) {
		sclk = get_pll_clk(BUS0_PLL);
		sel = readl(&clk->clk_mux_sel_top11);
		if ((sel >> 16) & 0x1) sclk = sclk / 2;
		sel = readl(&clk_topc->clk_mux_sel_topc1);
		if ((sel >> 16) & 0x1) sclk = sclk / 2;
	} else if(sel_mmc == 1) {
		sclk = get_pll_clk(BUS1_PLL);
		sel = readl(&clk->clk_mux_sel_top11);
		if ((sel >> 12) & 0x1) sclk = sclk / 2;
	} else if(sel_mmc == 2) {
		sclk = get_pll_clk(CCI_PLL);
		sel = readl(&clk->clk_mux_sel_top11);
		if ((sel >> 8) & 0x1) sclk = sclk / 2;
	} else if(sel_mmc == 3) {
		sclk = get_pll_clk(MFC_PLL);
		sel = readl(&clk->clk_mux_sel_top11);
		if ((sel >> 4) & 0x1) sclk = sclk / 2;
	} else {
		printf("Unsupported mmc mux (%d)\n", sel);
		return 0;
	}

	/*
	 * CLK_DIV_TOP1_FSYS0
	 * SCLK_MMC2[25:16]
	 * CLK_DIV_TOP1_FSYS1
	 * SCLK_MMC0[21:12]
	 * SCLK_MMC1[9:0]
	 */
	if (dev_index < 2) {
#ifdef CONFIG_CPU_EXYNOS7420_EVT1
		ratio = readl(&clk->clk_div_top1_fsys11);
		/* get SCLK_MMCx_RATIO */
		ratio = (ratio >> (1 - dev_index) * 12) & 0x3ff;
#else
		ratio = readl(&clk->clk_div_top1_fsys1);
		/* get SCLK_MMCx_RATIO */
		ratio = (ratio >> (24 + (1 - dev_index) * 4)) & 0xf;
#endif
	} else {
#ifdef CONFIG_CPU_EXYNOS7420_EVT1
		ratio = readl(&clk->clk_div_top1_fsys0);
		/* get SCLK_MMCx_RATIO */
		ratio = (ratio >> 16) & 0x3ff;
#else
		ratio = readl(&clk->clk_div_top1_fsys0);
		/* get SCLK_MMCx_RATIO */
		ratio = (ratio >> 24) & 0xf;
#endif
	}

	return (sclk / (ratio + 1));
}

static unsigned long exynos7580_get_mmc_clk(int dev_index)
{
        struct exynos7580_clock_top *clk =
                (struct exynos7580_clock_top *)exynos7580_get_base_clock_top();
        unsigned int sel_mmc;
        unsigned int ratio, ratio_a, ratio_b;
        unsigned long sclk;

        if (dev_index == 0) {
                sel_mmc = readl(&clk->clk_mux_sel_top_fsys0);
        } else if (dev_index == 1) {
                sel_mmc = readl(&clk->clk_mux_sel_top_fsys1);
        } else {
                sel_mmc = readl(&clk->clk_mux_sel_top_fsys2);
        }


        /* get src pll clock */
        if (sel_mmc & 0x8) {
                sclk = get_pll_clk(PLL_MEDIA);
        } else if (sel_mmc & 0x1) {
                sclk = get_pll_clk(PLL_BUS);
        } else {
                sclk = CONFIG_SYS_CLK_FREQ;
        }

        /*
         * CLK_DIV_TOP_FSYS0
         * SCLK_FSYS_MMC0_SDCLKIN_B_RATIO[11:4], SCLK_FSYS_MMC0_SDCLKIN_A_RATIO[3:0]
         * CLK_DIV_TOP_FSYS1
         * SCLK_FSYS_MMC1_SDCLKIN_B_RATIO[11:4], SCLK_FSYS_MMC1_SDCLKIN_A_RATIO[3:0]
         * CLK_DIV_TOP_FSYS2
         * SCLK_FSYS_MMC2_SDCLKIN_B_RATIO[11:4], SCLK_FSYS_MMC2_SDCLKIN_A_RATIO[3:0]
         */
        if (dev_index == 0) {
                ratio = readl(&clk->clk_div_top_fsys0);
        } else if (dev_index == 1) {
                ratio = readl(&clk->clk_div_top_fsys1);
        } else {
                ratio = readl(&clk->clk_div_top_fsys2);
        }

        /* get SCLK_FSYS_MMC0_SDCLKIN_A_RATIO */
        ratio_a = ratio & 0xf;
        /* get SCLK_FSYS_MMC0_SDCLKIN_B_RATIO */
        ratio_b = (ratio >> 4) & 0xff;

        return (sclk / (ratio_a + 1)) / (ratio_b + 1);
}

unsigned long get_pll_clk(int pllreg)
{
	if (cpu_is_exynos5()) {
		if(s5p_cpu_id == 0x5430) {
			return exynos5430_get_pll_clk(pllreg);
		}
	} else if (cpu_is_exynos7()) {
		if(s5p_cpu_id == 0x7420) {
			return exynos7420_get_pll_clk(pllreg);
		}
		if(s5p_cpu_id == 0x7580) {
			return exynos7580_get_pll_clk(pllreg);
		}
	}
}

unsigned long get_pwm_clk(void)
{
	if (cpu_is_exynos5()) {
		if(s5p_cpu_id == 0x5430) {
			return exynos5430_get_pwm_clk();
		}
	} else if (cpu_is_exynos7()) {
		if(s5p_cpu_id == 0x7420) {
			return exynos7420_get_pwm_clk();
		}
		if(s5p_cpu_id == 0x7580) {
			return exynos7580_get_pwm_clk();
		}
	}
}

unsigned long get_uart_clk(int dev_index)
{
	if (cpu_is_exynos5()) {
		if(s5p_cpu_id == 0x5430) {
			return exynos5430_get_uart_clk(dev_index);
		}
	} else if (cpu_is_exynos7()) {
		if(s5p_cpu_id == 0x7420) {
			return exynos7420_get_uart_clk(dev_index);
		}
		if(s5p_cpu_id == 0x7580) {
			return exynos7580_get_uart_clk(dev_index);
}
	}
}

unsigned long get_usbdrd_clk(void)
{
	if (cpu_is_exynos5()) {
		if(s5p_cpu_id == 0x5430) {
			return exynos5430_get_usbdrd_clk();
		}
	} else if (cpu_is_exynos7()) {
		if(s5p_cpu_id == 0x7420) {
			return exynos7420_get_usbdrd_clk();
		}
	}
}

void set_mmc_clk(int dev_index, unsigned int div)
{
	if (cpu_is_exynos5()) {
		if(s5p_cpu_id == 0x5430) {
			exynos5430_set_mmc_clk(dev_index, div);
		}
	} else if (cpu_is_exynos7()) {
		if(s5p_cpu_id == 0x7420) {
			exynos7420_set_mmc_clk(dev_index, div);
		}
		if(s5p_cpu_id == 0x7580) {
			exynos7580_set_mmc_clk(dev_index, div);
}
	}
}

unsigned long get_mmc_clk(int dev_index)
{
	if (cpu_is_exynos5()) {
		if(s5p_cpu_id == 0x5430) {
			return exynos5430_get_mmc_clk(dev_index);
		}
	} else if (cpu_is_exynos7()) {
		if(s5p_cpu_id == 0x7420) {
			return exynos7420_get_mmc_clk(dev_index);
		}
		if(s5p_cpu_id == 0x7580) {
			return exynos7580_get_mmc_clk(dev_index);
		}
	}
}

/* set HCI/UNIPRO/MPHY CLK */
void set_ufs_clk(int dev_index)
{
	if (cpu_is_exynos7()) {
		if(s5p_cpu_id == 0x7420) {
			exynos7420_set_ufs_clk(dev_index);
		}
	}
}
#endif /* CONFIG_SPL_BUILD */
