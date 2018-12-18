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

#include <common.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/clk.h>

/* exynos4: return pll clock frequency */
static unsigned long exynos4_get_pll_clk(int pllreg)
{
	struct exynos4_clock *clk =
		(struct exynos4_clock *)samsung_get_base_clock();
	unsigned long r, m, p, s, k = 0, mask, fout;
	unsigned int freq;

	switch (pllreg) {
	case APLL:
		r = readl(&clk->apll_con0);
		break;
	case MPLL:
		r = readl(&clk->mpll_con0);
		break;
	case EPLL:
		r = readl(&clk->epll_con0);
		k = readl(&clk->epll_con1);
		break;
	case VPLL:
		r = readl(&clk->vpll_con0);
		k = readl(&clk->vpll_con1);
		break;
#if defined(CONFIG_CPU_EXYNOS3250) || defined(CONFIG_CPU_EXYNOS3472)
	case BPLL:
		r = readl(&clk->bpll_con0);
		break;
	case UPLL:
		r = readl(&clk->upll_con0);
		break;
#endif
	default:
		printf("Unsupported PLL (%d)\n", pllreg);
		return 0;
	}

	/*
	 * APLL_CON: MIDV [25:16]
	 * MPLL_CON: MIDV [25:16]
	 * EPLL_CON: MIDV [24:16]
	 * VPLL_CON: MIDV [24:16]
	 */
	if (pllreg == APLL || pllreg == MPLL)
		mask = 0x3ff;
	else
		mask = 0x1ff;

	m = (r >> 16) & mask;

	/* PDIV [13:8] */
	p = (r >> 8) & 0x3f;
	/* SDIV [2:0] */
	s = r & 0x7;

	freq = CONFIG_SYS_CLK_FREQ;

	if (pllreg == EPLL) {
		k = k & 0xffff;
		/* FOUT = (MDIV + K / 65536) * FIN / (PDIV * 2^SDIV) */
		fout = (m + k / 65536) * (freq / (p * (1 << s)));
	} else if (pllreg == VPLL) {
		k = k & 0xfff;
		/* FOUT = (MDIV + K / 1024) * FIN / (PDIV * 2^SDIV) */
		fout = (m + k / 1024) * (freq / (p * (1 << s)));
	} else {
		if ((s5p_cpu_id == 0x4415) || (s5p_cpu_id == 0x3250) || (s5p_cpu_id == 0x3472)) {
			/* FOUT = MDIV * FIN / (PDIV * 2^(SDIV)) */
			fout = m * (freq / (p * (1 << s)));
		} else {
			if (s < 1)
				s = 1;
			/* FOUT = MDIV * FIN / (PDIV * 2^(SDIV - 1)) */
			fout = m * (freq / (p * (1 << (s - 1))));
		}
	}

	return fout;
}

/* exynos5: return pll clock frequency */
static unsigned long exynos5_get_pll_clk(int pllreg)
{
#ifdef CONFIG_CPU_EXYNOS5410
	struct exynos5410_clock *clk =
		(struct exynos5_clock *)samsung_get_base_clock();
#elif defined(CONFIG_CPU_EXYNOS5412)
	struct exynos5412_clock *clk =
		(struct exynos5412_clock *)samsung_get_base_clock();
#else
	struct exynos5_clock *clk =
		(struct exynos5_clock *)samsung_get_base_clock();
#endif
	unsigned long r, m, p, s, k = 0, mask, fout;
	unsigned int freq;
	unsigned int  clk_mux_stat_cdrex, mpll_fout_sel;

	switch (pllreg) {
	case APLL:
		r = readl(&clk->apll_con0);
		break;
	case MPLL:
		r = readl(&clk->mpll_con0);
		break;
	case EPLL:
		r = readl(&clk->epll_con0);
		k = readl(&clk->epll_con1);
		break;
	case VPLL:
		r = readl(&clk->vpll_con0);
		k = readl(&clk->vpll_con1);
		break;
#if defined(CONFIG_CPU_EXYNOS5410) || defined(CONFIG_CPU_EXYNOS5412)
	case KPLL:
		r = readl(&clk->kpll_con0);
		break;
	case BPLL:
		r = readl(&clk->bpll_con0);
		break;
	case CPLL:
		r = readl(&clk->cpll_con0);
		break;
#endif
#if defined(CONFIG_CPU_EXYNOS5412)
	case SPLL:
		r = readl(&clk->spll_con0);
		break;
#endif
	default:
		printf("Unsupported PLL (%d)\n", pllreg);
		return 0;
	}

	/*
	 * APLL_CON: MIDV [25:16]
	 * MPLL_CON: MIDV [25:16]
	 * EPLL_CON: MIDV [24:16]
	 * VPLL_CON: MIDV [24:16]
	 */
	if (pllreg == APLL || pllreg == MPLL ||
			pllreg == KPLL || pllreg == BPLL ||
			pllreg == CPLL || pllreg == SPLL)
		mask = 0x3ff;
	else
		mask = 0x1ff;

	m = (r >> 16) & mask;

	/* PDIV [13:8] */
	p = (r >> 8) & 0x3f;
	/* SDIV [2:0] */
	s = r & 0x7;

	freq = CONFIG_SYS_CLK_FREQ;

	if (pllreg == EPLL) {
		k = k & 0xffff;
		/* FOUT = (MDIV + K / 65536) * FIN / (PDIV * 2^SDIV) */
		fout = (m + k / 65536) * (freq / (p * (1 << s)));
	} else if (pllreg == VPLL) {
		k = k & 0xfff;
		/* FOUT = (MDIV + K / 1024) * FIN / (PDIV * 2^SDIV) */
		fout = (m + k / 1024) * (freq / (p * (1 << s)));
	} else {
#if defined(CONFIG_CPU_EXYNOS5410) || defined(CONFIG_CPU_EXYNOS5412)
		s += 1;
#else
		if (s < 1)
			s = 1;
#endif
		/* FOUT = MDIV * FIN / (PDIV * 2^(SDIV - 1)) */
		fout = m * (freq / (p * (1 << (s - 1))));

		/* Check mpll_fout_sel status for Rev1.0 */
		if ((pllreg == MPLL) && (s5p_cpu_id == 0x5250) &&
				(s5p_get_cpu_rev() >= 0x1)) {
			clk_mux_stat_cdrex = readl(&clk->mux_stat_cdrex);

			mpll_fout_sel = ( clk_mux_stat_cdrex >> 16 ) && 0x1;

			if(mpll_fout_sel)
				fout = fout >> 1;
		}
	}

	return fout;
}

static unsigned long exynos5260_get_pll_clk(int pllreg)
{
	struct exynos5260_clock_egl *clk_egl =
		(struct exynos5260_clock_egl *)samsung_get_base_clock_egl();
	struct exynos5260_clock_kfc *clk_kfc =
		(struct exynos5260_clock_kfc *)samsung_get_base_clock_kfc();
	struct exynos5260_clock_mif *clk_mif =
		(struct exynos5260_clock_mif *)samsung_get_base_clock_mif();
	unsigned long r, m, p, s, k = 0, mask, fout;
	unsigned int pll_ratio = 0;
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
		pll_ratio = (readl(&clk_mif->clk_div_mif) >> 4) & 0x7;
		break;
	case BUS_PLL:
		r = readl(&clk_mif->bus_pll_con0);
		pll_ratio = (readl(&clk_mif->clk_div_mif) >> 8) & 0x7;
		break;
	case MEDIA_PLL:
		r = readl(&clk_mif->media_pll_con0);
		pll_ratio = readl(&clk_mif->clk_div_mif) & 0x7;
		break;
	default:
		printf("Unsupported PLL (%d)\n", pllreg);
		return 0;
	}

	/*
	 * PLL_CON: MIDV [25:16]
	 */
	mask = 0x3ff;
	m = (r >> 9) & mask;

	/* PDIV [13:8] */
	p = (r >> 3) & 0x3f;
	/* SDIV [2:0] */
	s = r & 0x7;

	freq = CONFIG_SYS_CLK_FREQ;

	/* FOUT = MDIV * FIN / (PDIV * 2^(SDIV)) */
	fout = m * (freq / (p * (1 << s)));
	fout = fout / ((unsigned long)pll_ratio + 1);

	return fout;
}

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

/* exynos4: return ARM clock frequency */
static unsigned long exynos4_get_arm_clk(void)
{
	struct exynos4_clock *clk =
		(struct exynos4_clock *)samsung_get_base_clock();
	unsigned long div;
	unsigned long armclk;
	unsigned int core_ratio;
	unsigned int core2_ratio;

	div = readl(&clk->div_cpu0);

	/* CORE_RATIO: [2:0], CORE2_RATIO: [30:28] */
	core_ratio = (div >> 0) & 0x7;
	core2_ratio = (div >> 28) & 0x7;

	armclk = get_pll_clk(APLL) / (core_ratio + 1);
	armclk /= (core2_ratio + 1);

	return armclk;
}

/* exynos5: return ARM clock frequency */
static unsigned long exynos5_get_arm_clk(void)
{
	struct exynos5_clock *clk =
		(struct exynos5_clock *)samsung_get_base_clock();
	unsigned long div;
	unsigned long armclk;
	unsigned int arm_ratio;
	unsigned int arm2_ratio;

	div = readl(&clk->div_cpu0);

	/* ARM_RATIO: [2:0], ARM2_RATIO: [30:28] */
	arm_ratio = (div >> 0) & 0x7;
	arm2_ratio = (div >> 28) & 0x7;

	armclk = get_pll_clk(APLL) / (arm_ratio + 1);
	armclk /= (arm2_ratio + 1);

	return armclk;
}

/* exynos5260: return ARM clock frequency */
static unsigned long exynos5260_get_arm_clk(void)
{
	struct exynos5260_clock_egl *clk =
		(struct exynos5260_clock_egl *)samsung_get_base_clock_egl();
	unsigned long div;
	unsigned long armclk;
	unsigned int egl1_ratio;
	unsigned int egl2_ratio;

	div = readl(&clk->clk_div_egl);

	/* EGL1_RATIO: [2:0], EGL2_RATIO: [6:4] */
	egl1_ratio = (div >> 0) & 0x7;
	egl2_ratio = (div >> 4) & 0x7;

	armclk = get_pll_clk(APLL) / (egl1_ratio + 1);
	armclk /= (egl2_ratio + 1);

	return armclk;
}

/* exynos5430: return ARM clock frequency */
static unsigned long exynos5430_get_arm_clk(void)
{
	struct exynos5430_clock_egl *clk =
		(struct exynos5430_clock_egl *)exynos5430_get_base_clock_egl();
	unsigned long div;
	unsigned long armclk;
	unsigned int egl1_ratio;
	unsigned int egl2_ratio;

	div = readl(&clk->clk_div_egl0);

	/* EGL1_RATIO: [2:0], EGL2_RATIO: [6:4] */
	egl1_ratio = (div >> 0) & 0x7;
	egl2_ratio = (div >> 4) & 0x7;

	armclk = get_pll_clk(APLL) / (unsigned long)(egl1_ratio + 1);
	armclk /= (unsigned long)(egl2_ratio + 1);

	return armclk;
}

/* exynos7420: return ARM clock frequency */
static unsigned long exynos7420_get_arm_clk(void)
{
	struct exynos7420_clock_atlas *clk =
		(struct exynos7420_clock_atlas *)exynos7420_get_base_clock_atlas();
	unsigned long div;
	unsigned long armclk;
	unsigned int atlas1_ratio;
	unsigned int atlas2_ratio;

	div = readl(&clk->clk_div_atlas0);

	/* ATLAS1_RATIO: [2:0], ATLAS2_RATIO: [6:4] */
	atlas1_ratio = (div >> 0) & 0x7;
	atlas2_ratio = (div >> 4) & 0x7;

	armclk = get_pll_clk(ATLAS_PLL) / (unsigned long)(atlas1_ratio + 1);
	armclk /= (unsigned long)(atlas2_ratio + 1);

	return armclk;
}

/* exynos4: return pwm clock frequency */
static unsigned long exynos4_get_pwm_clk(void)
{
	struct exynos4_clock *clk =
		(struct exynos4_clock *)samsung_get_base_clock();
	unsigned long pclk, sclk;
	unsigned int sel;
	unsigned int ratio;

	if (s5p_cpu_id == 0x4415)
		return 100000000;
	else if (s5p_cpu_id == 0x3250)
		return 50000000;
	else if (s5p_cpu_id == 0x3472)
		return 200000000;

	if (s5p_get_cpu_rev() == 0) {
		/*
		 * CLK_SRC_PERIL0
		 * PWM_SEL [27:24]
		 */
		sel = readl(&clk->src_peril0);
		sel = (sel >> 24) & 0xf;

		if (sel == 0x6)
			sclk = get_pll_clk(MPLL);
		else if (sel == 0x7)
			sclk = get_pll_clk(EPLL);
		else if (sel == 0x8)
			sclk = get_pll_clk(VPLL);
		else
			return 0;

		/*
		 * CLK_DIV_PERIL3
		 * PWM_RATIO [3:0]
		 */
		ratio = readl(&clk->div_peril3);
		ratio = ratio & 0xf;
	} else if (s5p_get_cpu_rev() >= 1) {
		sclk = get_pll_clk(MPLL);
		ratio = 8;
	} else
		return 0;

	pclk = sclk / (ratio + 1);

	return pclk;
}

/* exynos5: return pwm clock frequency */
static unsigned long exynos5_get_pwm_clk(void)
{
	struct exynos5_clock *clk =
		(struct exynos5_clock *)samsung_get_base_clock();
	unsigned long pclk, div_aclk_pre, div_aclk;
	unsigned int ratio;

	/*
	 * CLK_DIV_PERIC3
	 * PWM_RATIO [3:0]
	 */
	div_aclk_pre = readl(&clk->div_top1);
	div_aclk_pre = (div_aclk_pre >> 24)& 0x7;

	div_aclk = readl(&clk->div_top0);
	div_aclk = div_aclk& 0x7;

	pclk = (get_pll_clk(MPLL) / (div_aclk_pre + 1)) / (div_aclk +1);

	return pclk;
}

/* exynos5260: return pwm clock frequency */
static unsigned long exynos5260_get_pwm_clk(void)
{
	struct exynos5260_clock_top *clk =
		(struct exynos5260_clock_top *)samsung_get_base_clock_top();
	unsigned long pclk;
	unsigned int aclk_peri_66_ratio;

	/*
	 * CLK_DIV_TOP_PERI2[23:20]
	 * ACLK_PERI_66_RATIO
	 */
	aclk_peri_66_ratio = readl(&clk->clk_div_top_peri2);
	aclk_peri_66_ratio = (aclk_peri_66_ratio >> 20)& 0xf;

	pclk = get_pll_clk(BUS_PLL) / (aclk_peri_66_ratio + 1);

	return pclk;
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

/* exynos4: return uart clock frequency */
static unsigned long exynos4_get_uart_clk(int dev_index)
{
	struct exynos4_clock *clk =
		(struct exynos4_clock *)samsung_get_base_clock();
	unsigned long uclk, sclk;
	unsigned int sel;
	unsigned int ratio;
	unsigned int mpll_ratio_pre;

	/*
	 * CLK_SRC_PERIL0
	 * UART0_SEL [3:0]
	 * UART1_SEL [7:4]
	 * UART2_SEL [8:11]
	 * UART3_SEL [12:15]
	 * UART4_SEL [16:19]
	 * UART5_SEL [23:20]
	 */
	sel = readl(&clk->src_peril0);
	sel = (sel >> (dev_index << 2)) & 0xf;

	if (sel == 0x6) {
		sclk = get_pll_clk(MPLL);
#if defined(CONFIG_CPU_EXYNOS3250) || defined(CONFIG_CPU_EXYNOS3472)
		mpll_ratio_pre = (readl(&clk->div_top) >> 28) && 0x3;
		sclk = sclk / MPLL_PRE_DIV / (mpll_ratio_pre + 1);
#endif
	} else if (sel == 0x7)
		sclk = get_pll_clk(EPLL);
	else if (sel == 0x8)
		sclk = get_pll_clk(VPLL);
	else if ((sel == 0x0) ||(sel == 0x1))
		sclk = CONFIG_SYS_CLK_FREQ;
	else
		return 0;

	/*
	 * CLK_DIV_PERIL0
	 * UART0_RATIO [3:0]
	 * UART1_RATIO [7:4]
	 * UART2_RATIO [8:11]
	 * UART3_RATIO [12:15]
	 * UART4_RATIO [16:19]
	 * UART5_RATIO [23:20]
	 */
	ratio = readl(&clk->div_peril0);
	ratio = (ratio >> (dev_index << 2)) & 0xf;

	uclk = sclk / (ratio + 1);

	return uclk;
}

/* exynos5: return uart clock frequency */
static unsigned long exynos5_get_uart_clk(int dev_index)
{
	struct exynos5_clock *clk =
		(struct exynos5_clock *)samsung_get_base_clock();
	unsigned long uclk, sclk;
	unsigned int sel;
	unsigned int ratio;

	/*
	 * CLK_SRC_PERIC0
	 * UART0_SEL [3:0]
	 * UART1_SEL [7:4]
	 * UART2_SEL [8:11]
	 * UART3_SEL [12:15]
	 * UART4_SEL [16:19]
	 * UART5_SEL [23:20]
	 */
	sel = readl(&clk->src_peric0);
#if defined(CONFIG_CPU_EXYNOS5412)
	sel = (sel >> ((dev_index + 1) << 2)) & 0xf;
#else
	sel = (sel >> (dev_index << 2)) & 0xf;
#endif

#if defined(CONFIG_CPU_EXYNOS5412)
	if (sel == 0x3)
		sclk = get_pll_clk(MPLL);
	else if (sel == 0x6)
		sclk = get_pll_clk(EPLL);
	else
		return 0;
#else
	if (sel == 0x6)
		sclk = get_pll_clk(MPLL);
	else if (sel == 0x7)
		sclk = get_pll_clk(EPLL);
	else if (sel == 0x8)
		sclk = get_pll_clk(VPLL);
	else if (sel == 0x9)
		sclk = get_pll_clk(CPLL);
	else
		return 0;
#endif
	/*
	 * CLK_DIV_PERIC0
	 * UART0_RATIO [3:0]
	 * UART1_RATIO [7:4]
	 * UART2_RATIO [8:11]
	 * UART3_RATIO [12:15]
	 * UART4_RATIO [16:19]
	 * UART5_RATIO [23:20]
	 */
	ratio = readl(&clk->div_peric0);
#if defined(CONFIG_CPU_EXYNOS5412)
	ratio = (ratio >> ((dev_index + 2) << 2)) & 0xf;
#else
	ratio = (ratio >> (dev_index << 2)) & 0xf;
#endif

	uclk = sclk / (ratio + 1);

	return uclk;
}

/* exynos5260: return uart clock frequency */
static unsigned long exynos5260_get_uart_clk(int dev_index)
{
	struct exynos5260_clock_top *clk =
		(struct exynos5260_clock_top *)samsung_get_base_clock_top();
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
	sel = readl(&clk->clk_mux_sel_top_peri1);
	ratio = readl(&clk->clk_div_top_peri1);

	switch (dev_index) {
	case 0:
		sel = (sel >> 20) & 0xf;
		ratio = (ratio >> 24) & 0xf;
		break;
	case 1:
		ratio = (ratio >> 16) & 0xf;
		sel = (sel >> 16) & 0xf;
		break;
	case 2:
		sel = (sel >> 12) & 0xf;
		ratio = (ratio >> 20) & 0xf;
		break;
	}

	if (sel) {
		sclk = get_pll_clk(BUS_PLL);
	} else {
		sclk = CONFIG_SYS_CLK_FREQ;
		return 0;
	}

	uclk = sclk / (ratio + 1);

	return uclk;
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

/* exynos4: get the usbdrd clock */
static unsigned int exynos4_get_usbdrd_clk(void)
{
	/* TODO */
	return 0;
}

/* exynos5: get the usbdrd clock */
#if defined(CONFIG_CPU_EXYNOS5260)
static unsigned long exynos5_get_usbdrd_clk(void)
{
	struct exynos5260_clock_top *clk =
		(struct exynos5260_clock_top *)samsung_get_base_clock_top();
	unsigned int addr;
	unsigned int ratio;
	unsigned long sclk;

	sclk = 24000000;

	/*
	 * CLK_DIV_TOP_FSYS0
	 * SCLK_FSYS_USBDRD30_SUSPEND_CLK_RATIO[7:4]
	 */
	addr = (unsigned int)&clk->clk_div_top_fsys0;
	ratio = readl(addr);

	ratio = (ratio >> 4) & 0xff;

	return (sclk / (ratio + 1));
}
#else
static unsigned long exynos5_get_usbdrd_clk(void)
{
	struct exynos5_clock *clk =
		(struct exynos5_clock *)samsung_get_base_clock();
	unsigned int addr;
	unsigned int sel;
	unsigned int ratio;
	unsigned long sclk;

	sel = readl(&clk->src_fsys);
	sel = (sel >> 28) & 0xf;

#if defined(CONFIG_CPU_EXYNOS5410) || defined(CONFIG_CPU_EXYNOS5412)
	sclk = 24000000;
#else
	if (sel == 0x0)
		sclk = get_pll_clk(MPLL);
	else {
		printf("Can't get SCLK_CPLL\n");
		return 0;
	}
#endif

	/*
	 * CLK_DIV_FSYS0
	 * USBDRD30_RATIO[27:24], SATA_RATIO[23:20]
	 */
	addr = (unsigned int)&clk->div_fsys0;
	ratio = readl(addr);

	ratio = (ratio >> 24) & 0xff;

	return (sclk / (ratio + 1));
}
#endif
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

/* exynos4: set the mmc clock */
static void exynos4_set_mmc_clk(int dev_index, unsigned int div)
{
	struct exynos4_clock *clk =
		(struct exynos4_clock *)samsung_get_base_clock();
	unsigned int addr;
	unsigned int val;

	/*
	 * CLK_DIV_FSYS1
	 * MMC0_PRE_RATIO [15:8], MMC1_PRE_RATIO [31:24]
	 * MMC0_RATIO [3:0],      MMC1_RATIO [16:19]
	 * CLK_DIV_FSYS2
	 * MMC2_PRE_RATIO [15:8], MMC3_PRE_RATIO [31:24]
	 * MMC2_RATIO [3:0],      MMC3_RATIO [16:19]
	 * CLK_DIV_FSYS3
	 * MMC4_PRE_RATIO [15:8]
	 * MMC4_RATIO [3:0]
	 */
	if (dev_index < 2) {
		addr = (unsigned int)&clk->div_fsys1;
	} else if (2 <= dev_index && dev_index < 4) {
		addr = (unsigned int)&clk->div_fsys2;
		dev_index -= 2;
	} else {
		addr = (unsigned int)&clk->div_fsys3;
		dev_index = 0;
	}

	val = readl(addr);
	/* clear MMCx_PRE_RATIO */
	val &= ~(0xff << ((dev_index << 4) + 8));
	/* clear MMCx_RATIO */
	val &= ~(0xff << (dev_index << 4));
	val |= (div & 0xff) << ((dev_index << 4) + 8);
	writel(val, addr);
}

/* exynos5: set the mmc clock */
static void exynos5_set_mmc_clk(int dev_index, unsigned int div)
{
	struct exynos5_clock *clk =
		(struct exynos5_clock *)samsung_get_base_clock();
	unsigned int addr;
	unsigned int val;

	/*
	 * CLK_DIV_FSYS1
	 * MMC0_PRE_RATIO [15:8], MMC1_PRE_RATIO [31:24]
	 * MMC0_RATIO [3:0],	  MMC1_RATIO [16:19]
	 * CLK_DIV_FSYS2
	 * MMC2_PRE_RATIO [15:8], MMC3_PRE_RATIO [31:24]
	 * MMC2_RATIO [3:0],	  MMC3_RATIO [16:19]
	 */
#if defined(CONFIG_CPU_EXYNOS5412)
	addr = (unsigned int)&clk->div_fsys1;
#else
	if (dev_index < 2) {
		addr = (unsigned int)&clk->div_fsys1;
	} else {
		addr = (unsigned int)&clk->div_fsys2;
		dev_index -= 2;
	}
#endif

#if defined(CONFIG_CPU_EXYNOS5412)
	val = readl(addr);
	if (dev_index == 0) {
		val &= ~(0x3ff);
		val |= (div & 0x3ff);
	} else {
		val &= ~(((0x3ff) << dev_index * 10));
		val |= ((div & 0x3ff) << (dev_index * 10));
	}
	writel(val, addr);
#else
	val = readl(addr);
	/* clear MMCx_PRE_RATIO */
	val &= ~(0xff << ((dev_index << 4) + 8));
	/* clear MMCx_RATIO */
	val &= ~(0xff << (dev_index << 4));
	val |= (div & 0xff) << ((dev_index << 4) + 8);
	writel(val, addr);
#endif
}

/* exynos5260: set the mmc clock */
static void exynos5260_set_mmc_clk(int dev_index, unsigned int div)
{
	struct exynos5260_clock_top *clk =
		(struct exynos5260_clock_top *)samsung_get_base_clock_top();
	unsigned int addr;
	unsigned int ratio;

	/*
	 * CLK_DIV_TOP_FSYS0
	 * SCLK_FSYS_MMC0_SDCLKIN_B_RATIO[23:16]
	 * SCLK_FSYS_MMC0_SDCLKIN_A_RATIO[15:12]
	 * CLK_DIV_TOP_FSYS1
	 * SCLK_FSYS_MMC2_SDCLKIN_B_RATIO[23:16]
	 * SCLK_FSYS_MMC2_SDCLKIN_A_RATIO[15:12]
	 * SCLK_FSYS_MMC1_SDCLKIN_B_RATIO[11:4]
	 * SCLK_FSYS_MMC1_SDCLKIN_A_RATIO[3:0]
	 */
	if (dev_index == 0) {
		addr = (unsigned int)&clk->clk_div_top_fsys0;
		ratio = readl(addr);
		/* set SCLK_FSYS_MMC0_SDCLKIN_A_RATIO for div_1 */
		ratio &= ~(0xf << 12);
		/* set SCLK_FSYS_MMC0_SDCLKIN_B_RATIO */
		ratio &= ~(0xff << 16);
		ratio |= ((div & 0xff) << 16);
	} else {
		addr = (unsigned int)&clk->clk_div_top_fsys1;
		ratio = readl(addr);
		/* set SCLK_FSYS_MMCx_SDCLKIN_A_RATIO for div_1 */
		ratio &= ~(0xf << ((dev_index - 1) * 12));
		/* get SCLK_FSYS_MMCx_SDCLKIN_B_RATIO */
		ratio &= ~(0xff << (((dev_index - 1) * 12) + 4));
		ratio |= ((div & 0xff) << (((dev_index - 1) * 12) + 4));
	}

	writel(ratio, addr);
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

#define UFS_GET_SFR(addr, mask, shift)	((readl(addr)>>(shift))&(mask))
#define UFS_SET_SFR(addr, value, mask, shift)	writel((readl(addr)&(~((mask)<<(shift))))|((value)<<(shift)), (addr))
#define UFS_WAIT_CLK(addr, value, mask, shift)	while( ((readl(addr)>>(shift))&(mask))!=(value) )

static void exynos5430_set_ufs_clk(int dev_index)
{
	u32 reg;

	/* CLK_DIV_CPIF : SCLK_MPHY_RATIO = 9 */
	writel(0x12, 0x10fc0600);
	UFS_WAIT_CLK(0x10fc0700, 0, 0x1, 0);

	/* CLK_MUX_SEL_CPIF0 : MUX_MPHY_PLL = FOUT_MPHY_PLL */
	writel(1, 0x10fc0200);

	/* HELSINKI EVT1 change CLK_MUX_STAT_XXX register : 001, 010, 1xx... */
	UFS_WAIT_CLK(0x10fc0400, 2, 0x7, 0);

	/* clock.c */
	/* CLK_DIV_TOP_FSYS2[7:4] : SCLK_UFSUNIPRO_RATIO=3 */
	UFS_SET_SFR(0x10030634, 2, 0xF, 4);
	UFS_WAIT_CLK(0x10030734, 0, 0x1, 4);

	/* CLK_DIV_TOP2[2:0] : ACLK_FSYS_200_RATIO=4 */
	UFS_SET_SFR(0x10030608, 5, 0x7, 0);
	UFS_WAIT_CLK(0x10030708, 0, 0x1, 0);

	/* CLK_MUX_SEL_FSYS0[4] : if SCLK_UFS_PLL_USER_SEL=0 */
	if (UFS_GET_SFR(0x156e0200, 1, 4) !=0) {
		/* CLK_MUX_SEL_FSYS0[0] : SCLK_UFS_PLL_USER_SEL=0 */
		UFS_SET_SFR(0x156e0200, 0, 0x1, 4);
		/* HELSINKI EVT1 change CLK_MUX_STAT_XXX register : 001, 010, 1xx... */
		UFS_WAIT_CLK(0x156e0400, 1, 0x7, 4);
	}

	/* CLK_MUX_ENABLE_FSYS0[4] : if SCLK_UFS_PLL_USER_ENABLE=0 */
	if (UFS_GET_SFR(0x156e0300, 1, 4)==0) {
		/* CLK_MUX_ENABLE_FSYS0[4] : SCLK_UFS_PLL_USER_ENABLE=1 */
		UFS_SET_SFR(0x156e0300, 1, 0x1, 4);
	}

	/* CLK_MUX_SEL_FSYS0[0] : if ACLK_FSYS_200_USER_SEL=0 */
	if (UFS_GET_SFR(0x156e0200, 1, 0)==0) {
		/* CLK_MUX_ENABLE_FSYS0[0] : ACLK_FSYS_200_USER_ENABLE=0 */
		UFS_SET_SFR(0x156e0300, 0, 0x1, 0);
		/* CLK_MUX_SEL_FSYS0[0] : ACLK_FSYS_200_USER_SEL=1 */
		UFS_SET_SFR(0x156e0200, 1, 0x1, 0);
		/* CLK_MUX_ENABLE_FSYS0[0] : ACLK_FSYS_200_USER_ENABLE=1 */
		UFS_SET_SFR(0x156e0300, 1, 0x1, 0);
		/* HELSINKI EVT1 change CLK_MUX_STAT_XXX register : 001, 010, 1xx... */
		UFS_WAIT_CLK(0x156e0400, 2, 0x7, 0);
	}

	/* CLK_MUX_ENABLE_FSYS0[0] : if ACLK_FSYS_200_USER_ENABLE=0 */
	if (UFS_GET_SFR(0x156e0300, 1, 0)==0) {
		/* CLK_MUX_ENABLE_FSYS0[0] : ACLK_FSYS_200_USER_ENABLE=1 */
		UFS_SET_SFR(0x156e0300, 1, 0x1, 0);
	}

	/* CLK_MUX_SEL_TOP_FSYS1[8] : if SCLK_UFSUNIPRO_SEL=0 */
	if (UFS_GET_SFR(0x10030234, 1, 8)==0) {
		/* CLK_MUX_ENABLE_TOP_FSYS1[8] : SCLK_UFSUNIPRO_ENABLE=0 */
		UFS_SET_SFR(0x10030334, 0, 0x1, 8);
		/* CLK_MUX_SEL_TOP_FSYS1[8] : SCLK_UFSUNIPRO_SEL=1 */
		UFS_SET_SFR(0x10030234, 1, 0x1, 8);
		/* CLK_MUX_ENABLE_TOP_FSYS1[8] : SCLK_UFSUNIPRO_ENABLE=1 */
		UFS_SET_SFR(0x10030334, 1, 0x1, 8);
		/* HELSINKI EVT1 change CLK_MUX_STAT_XXX register : 001, 010, 1xx... */
		UFS_WAIT_CLK(0x10030434, 2, 0x7, 8);
	}

	/* CLK_MUX_ENABLE_TOP_FSYS1[8] : if SCLK_UFSUNIPRO_ENABLE=0 */
	if (UFS_GET_SFR(0x10030334, 1, 8)==0) {
		/* CLK_MUX_ENABLE_TOP_FSYS1[8] : SCLK_UFSUNIPRO_ENABLE=1 */
		UFS_SET_SFR(0x10030334, 1, 0x1, 8);
	}

	/* CLK_MUX_SEL_FSYS1[24] : if SCLK_UFSUNIPRO_USER_SEL=0 */
	if (UFS_GET_SFR(0x156e0204, 1, 24)==0) {
		/* CLK_MUX_ENABLE_FSYS1[24] : SCLK_UFSUNIPRO_ENABLE=0 */
		UFS_SET_SFR(0x156e0304, 0, 0x1, 24);
		/* CLK_MUX_SEL_FSYS1[24] : SCLK_UFSUNIPRO_USER_SEL=1 */
		UFS_SET_SFR(0x156e0204, 1, 0x1, 24);
		/* CLK_MUX_ENABLE_FSYS1[24] : SCLK_UFSUNIPRO_ENABLE=1 */
		UFS_SET_SFR(0x156e0304, 1, 0x1, 24);
		/* HELSINKI EVT1 change CLK_MUX_STAT_XXX register : 001, 010, 1xx... */
		UFS_WAIT_CLK(0x156e0404, 2, 0x7, 24);
	}

	/* CLK_MUX_ENABLE_FSYS1[24] : if SCLK_UFSUNIPRO_ENABLE=0 */
	if (UFS_GET_SFR(0x156e0304, 1, 24)==0) {
		/* CLK_MUX_ENABLE_FSYS1[24] : SCLK_UFSUNIPRO_ENABLE=1 */
		UFS_SET_SFR(0x156e0304, 1, 0x1, 24);
	}

	/* MUX_PHYCLK_UFS_RX/TX_SYMBOL_USER */

	/* CLK_MUX_SEL_FSYS3[16] : PHYCLK_UFS_RX1_SYMBOL_USER_SEL=0 */
	if (UFS_GET_SFR(0x156e020c, 1, 16)==0) {
		/* CLK_MUX_ENABLE_FSYS3[16] : PHYCLK_UFS_RX1_SYMBOL_USER_ENABLE=0 */
		UFS_SET_SFR(0x156e030c, 0, 0x1, 16);
		/* CLK_MUX_SEL_FSYS3[16] : PHYCLK_UFS_RX1_SYMBOL_USER_SEL=1 */
		UFS_SET_SFR(0x156e020c, 1, 0x1, 16);
		/* CLK_MUX_ENABLE_FSYS3[16] : PHYCLK_UFS_RX1_SYMBOL_USER_ENABLE=1 */
		UFS_SET_SFR(0x156e030c, 1, 0x1, 16);
	}

	/* CLK_MUX_SEL_FSYS3[12] : PHYCLK_UFS_RX0_SYMBOL_USER_SEL=0 */
	if (UFS_GET_SFR(0x156e020c, 1, 12)==0) {
		/* CLK_MUX_ENABLE_FSYS3[16] : PHYCLK_UFS_RX0_SYMBOL_USER_ENABLE=0 */
		UFS_SET_SFR(0x156e030c, 0, 0x1, 12);
		/* CLK_MUX_SEL_FSYS3[16] : PHYCLK_UFS_RX0_SYMBOL_USER_SEL=1 */
		UFS_SET_SFR(0x156e020c, 1, 0x1, 12);
		/* CLK_MUX_ENABLE_FSYS3[16] : PHYCLK_UFS_RX0_SYMBOL_USER_ENABLE=1 */
		UFS_SET_SFR(0x156e030c, 1, 0x1, 12);
	}

	/* CLK_MUX_SEL_FSYS3[8] : PHYCLK_UFS_TX1_SYMBOL_USER_SEL=0 */
	if (UFS_GET_SFR(0x156e020c, 1, 8)==0) {
		/* CLK_MUX_ENABLE_FSYS3[8] : PHYCLK_UFS_TX1_SYMBOL_USER_ENABLE=0 */
		UFS_SET_SFR(0x156e030c, 0, 0x1, 8);
		/* CLK_MUX_SEL_FSYS3[8] : PHYCLK_UFS_TX1_SYMBOL_USER_SEL=1 */
		UFS_SET_SFR(0x156e020c, 1, 0x1, 8);
		/* CLK_MUX_ENABLE_FSYS3[8] : PHYCLK_UFS_TX1_SYMBOL_USER_ENABLE=1 */
		UFS_SET_SFR(0x156e030c, 1, 0x1, 8);
	}

	/* CLK_MUX_SEL_FSYS3[4] : PHYCLK_UFS_TX0_SYMBOL_USER_SEL=0 */
	if (UFS_GET_SFR(0x156e020c, 1, 4)==0) {
		/* CLK_MUX_ENABLE_FSYS3[4] : PHYCLK_UFS_TX0_SYMBOL_USER_ENABLE=0 */
		UFS_SET_SFR(0x156e030c, 0, 0x1, 4);
		/* CLK_MUX_SEL_FSYS3[4] : PHYCLK_UFS_TX0_SYMBOL_USER_SEL=1 */
		UFS_SET_SFR(0x156e020c, 1, 0x1, 4);
		/* CLK_MUX_ENABLE_FSYS3[4] : PHYCLK_UFS_TX0_SYMBOL_USER_ENABLE=1 */
		UFS_SET_SFR(0x156e030c, 1, 0x1, 4);
	}

	/* MUX_PHYCLK_UFS_RX/TX_SYMBOL_USER_ENABLE */

	/* CLK_MUX_ENABLE_FSYS3[16] : if PHYCLK_UFS_RX1_SYMBOL_USER_ENABLE=0 */
	if (UFS_GET_SFR(0x156e030c, 1, 16)==0) {
		/* CLK_MUX_ENABLE_FSYS3[16] : PHYCLK_UFS_RX1_SYMBOL_USER_ENABLE=1 */
		UFS_SET_SFR(0x156e030c, 1, 0x1, 16);
	}

	/* CLK_MUX_ENABLE_FSYS3[12] : if PHYCLK_UFS_RX0_SYMBOL_USER_ENABLE=0 */
	if (UFS_GET_SFR(0x156e030c, 1, 12)==0) {
		/* CLK_MUX_ENABLE_FSYS3[12] : PHYCLK_UFS_RX0_SYMBOL_USER_ENABLE=1 */
		UFS_SET_SFR(0x156e030c, 1, 0x1, 12);
	}

	/* CLK_MUX_ENABLE_FSYS3[8] : if PHYCLK_UFS_TX1_SYMBOL_USER_ENABLE=0 */
	if (UFS_GET_SFR(0x156e030c, 1, 8)==0) {
		/* CLK_MUX_ENABLE_FSYS3[8] : PHYCLK_UFS_TX1_SYMBOL_USER_ENABLE=1 */
		UFS_SET_SFR(0x156e030c, 1, 0x1, 8);
	}

	/* CLK_MUX_ENABLE_FSYS3[4] : if PHYCLK_UFS_TX0_SYMBOL_USER_ENABLE=0 */
	if (UFS_GET_SFR(0x156e030c, 1, 4)==0) {
		/* CLK_MUX_ENABLE_FSYS3[4] : PHYCLK_UFS_TX0_SYMBOL_USER_ENABLE=1 */
		UFS_SET_SFR(0x156e030c, 1, 0x1, 4);
	}
}

/* exynos4: get the mmc clock */
static unsigned int exynos4_get_mmc_clk(int dev_index)
{
	struct exynos4_clock *clk =
		(struct exynos4_clock *)samsung_get_base_clock();
	unsigned int addr;
	unsigned int sel;
	unsigned int pre_ratio, ratio;
	unsigned long sclk;
	unsigned int mpll_ratio_pre;

	sel = readl(&clk->src_fsys);
	sel = (sel >> (dev_index << 2)) & 0xf;

	if (sel == 0x6) {
		sclk = get_pll_clk(MPLL);
#if defined(CONFIG_CPU_EXYNOS3250) || defined(CONFIG_CPU_EXYNOS3472)
		mpll_ratio_pre = (readl(&clk->div_top) >> 28) && 0x3;
		sclk = sclk / MPLL_PRE_DIV / (mpll_ratio_pre + 1);
#endif
	} else if (sel == 0x7)
		sclk = get_pll_clk(EPLL);
	else if (sel == 0x8)
		sclk = get_pll_clk(VPLL);
	else
		return 0;

	/*
	 * CLK_DIV_FSYS1
	 * MMC0_PRE_RATIO [15:8], MMC1_PRE_RATIO [31:24]
	 * MMC0_RATIO [3:0],	  MMC1_RATIO [16:19]
	 * CLK_DIV_FSYS2
	 * MMC2_PRE_RATIO [15:8], MMC3_PRE_RATIO [31:24]
	 * MMC2_RATIO [3:0],	  MMC3_RATIO [16:19]
	 * CLK_DIV_FSYS3
	 * MMC4_PRE_RATIO [15:8]
	 * MMC4_RATIO [3:0]
	 */
	if (dev_index < 2) {
		addr = (unsigned int)&clk->div_fsys1;
	} else if (2 <= dev_index && dev_index < 4) {
		addr = (unsigned int)&clk->div_fsys2;
		dev_index -= 2;
	} else {
		addr = (unsigned int)&clk->div_fsys3;
		dev_index = 0;
	}

	ratio = readl(addr);
	/* get MMCx_PRE_RATIO */
	pre_ratio = (ratio >> ((dev_index << 4) + 8)) & 0xff;
	/* get MMCx_RATIO */
	ratio = (ratio >> (dev_index << 4)) & 0xff;

	return (sclk / (pre_ratio + 1)) / (ratio + 1);
}

/* exynos5: get the mmc clock */
static unsigned long exynos5_get_mmc_clk(int dev_index)
{
	struct exynos5_clock *clk =
		(struct exynos5_clock *)samsung_get_base_clock();
	unsigned int addr;
	unsigned int sel;
	unsigned int pre_ratio, ratio;
	unsigned long sclk;

	sel = readl(&clk->src_fsys);
#if defined(CONFIG_CPU_EXYNOS5412)
	sel = (sel >> ((dev_index + 2) << 2)) & 0xf;
#else
	sel = (sel >> (dev_index << 2)) & 0xf;
#endif

#if defined(CONFIG_CPU_EXYNOS5412)
	if (sel == 0x3)
		sclk = get_pll_clk(MPLL);
	else if (sel == 0x4)
		sclk = get_pll_clk(SPLL);
	else if (sel == 0x6)
		sclk = get_pll_clk(EPLL);
	else
		return 0;
#else
	if (sel == 0x6)
		sclk = get_pll_clk(MPLL);
	else if (sel == 0x7)
		sclk = get_pll_clk(EPLL);
	else if (sel == 0x8)
		sclk = get_pll_clk(VPLL);
	else
		return 0;
#endif

	/*
	 * CLK_DIV_FSYS1
	 * MMC0_PRE_RATIO [15:8], MMC1_PRE_RATIO [31:24]
	 * MMC0_RATIO [3:0],	  MMC1_RATIO [16:19]
	 * CLK_DIV_FSYS2
	 * MMC2_PRE_RATIO [15:8], MMC3_PRE_RATIO [31:24]
	 * MMC2_RATIO [3:0],	  MMC3_RATIO [16:19]
	 */
#if defined(CONFIG_CPU_EXYNOS5412)
	addr = (unsigned int)&clk->div_fsys1;
#else
	if (dev_index < 2) {
		addr = (unsigned int)&clk->div_fsys1;
	} else {
		addr = (unsigned int)&clk->div_fsys2;
		dev_index -= 2;
	}
#endif

	ratio = readl(addr);
#if defined(CONFIG_CPU_EXYNOS5412)
	if (dev_index == 0)
		ratio = ratio & 0x3ff;
	else
		ratio = (ratio >> (dev_index * 10)) & 0x3ff;

	return sclk / (ratio + 1);
#else
	/* get MMCx_PRE_RATIO */
	pre_ratio = (ratio >> ((dev_index << 4) + 8)) & 0xff;
	/* get MMCx_RATIO */
	ratio = (ratio >> (dev_index << 4)) & 0xff;

	return (sclk / (pre_ratio + 1)) / (ratio + 1);
#endif
}

/* exynos5260: get the mmc clock */
static unsigned long exynos5260_get_mmc_clk(int dev_index)
{
	struct exynos5260_clock_top *clk =
		(struct exynos5260_clock_top *)samsung_get_base_clock_top();
	unsigned int sel, sel_a, sel_b;
	unsigned int ratio, ratio_a, ratio_b;
	unsigned long sclk;

	sel = readl(&clk->clk_mux_sel_top_fsys);
	sel_b = (sel >> (24 - (dev_index * 8))) & 0x1;

	if (sel_b) {
		sclk = get_pll_clk(MEDIA_PLL);
	} else {
		sel_a = (sel >> (20 - (dev_index * 8))) & 0x1;
		if (sel_a) {
			sclk = get_pll_clk(BUS_PLL);
		} else {
			sclk = CONFIG_SYS_CLK_FREQ;
		}
	}

	/*
	 * CLK_DIV_TOP_FSYS0
	 * SCLK_FSYS_MMC0_SDCLKIN_B_RATIO[23:16]
	 * SCLK_FSYS_MMC0_SDCLKIN_A_RATIO[15:12]
	 * CLK_DIV_TOP_FSYS1
	 * SCLK_FSYS_MMC2_SDCLKIN_B_RATIO[23:16]
	 * SCLK_FSYS_MMC2_SDCLKIN_A_RATIO[15:12]
	 * SCLK_FSYS_MMC1_SDCLKIN_B_RATIO[11:4]
	 * SCLK_FSYS_MMC1_SDCLKIN_A_RATIO[3:0]
	 */
	if (dev_index == 0) {
		ratio = readl(&clk->clk_div_top_fsys0);
		/* get SCLK_FSYS_MMC0_SDCLKIN_A_RATIO */
		ratio_a = (ratio >> 12) & 0xf;
		/* get SCLK_FSYS_MMC0_SDCLKIN_B_RATIO */
		ratio_b = (ratio >> 16) & 0xff;
	} else {
		ratio = readl(&clk->clk_div_top_fsys1);
		/* get SCLK_FSYS_MMCx_SDCLKIN_A_RATIO */
		ratio_a = (ratio >> ((dev_index - 1) * 12)) & 0xf;
		/* get SCLK_FSYS_MMCx_SDCLKIN_B_RATIO */
		ratio_b = (ratio >> (((dev_index - 1) * 12) + 4)) & 0xff;
	}

	return (sclk / (ratio_a + 1)) / (ratio_b + 1);
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

/* get_lcd_clk: return lcd clock frequency */
static unsigned long exynos4_get_lcd_clk(void)
{
	struct exynos4_clock *clk =
		(struct exynos4_clock *)samsung_get_base_clock();
	unsigned long pclk, sclk;
	unsigned int sel;
	unsigned int ratio;

	/*
	 * CLK_SRC_LCD0
	 * FIMD0_SEL [3:0]
	 */
	sel = readl(&clk->src_lcd0);
	sel = sel & 0xf;

	/*
	 * 0x6: SCLK_MPLL
	 * 0x7: SCLK_EPLL
	 * 0x8: SCLK_VPLL
	 */
	if (sel == 0x6)
		sclk = get_pll_clk(MPLL);
	else if (sel == 0x7)
		sclk = get_pll_clk(EPLL);
	else if (sel == 0x8)
		sclk = get_pll_clk(VPLL);
	else
		return 0;

	/*
	 * CLK_DIV_LCD0
	 * FIMD0_RATIO [3:0]
	 */
	ratio = readl(&clk->div_lcd0);
	ratio = ratio & 0xf;

	pclk = sclk / (ratio + 1);

	return pclk;
}

void exynos4_set_lcd_clk(void)
{
	struct exynos4_clock *clk =
	    (struct exynos4_clock *)samsung_get_base_clock();
	unsigned int cfg = 0;

	/*
	 * CLK_GATE_BLOCK
	 * CLK_CAM	[0]
	 * CLK_TV	[1]
	 * CLK_MFC	[2]
	 * CLK_G3D	[3]
	 * CLK_LCD0	[4]
	 * CLK_LCD1	[5]
	 * CLK_GPS	[7]
	 */
	cfg = readl(&clk->gate_block);
	cfg |= 1 << 4;
	writel(cfg, &clk->gate_block);

	/*
	 * CLK_SRC_LCD0
	 * FIMD0_SEL		[3:0]
	 * MIPI0_SEL		[12:15]
	 * set lcd0 src clock 0x6: SCLK_MPLL
	 */
	cfg = readl(&clk->src_lcd0);
	cfg &= ~(0xf);
	cfg |= 0x6;
	writel(cfg, &clk->src_lcd0);

	/*
	 * CLK_GATE_IP_LCD0
	 * CLK_FIMD0		[0]
	 * CLK_MIE0		[1]
	 * CLK_DSIM0		[3]
	 * CLK_SMMUFIMD0	[4]
	 * CLK_PPMULCD0		[5]
	 * Gating all clocks for FIMD0
	 */
	cfg = readl(&clk->gate_ip_lcd0);
	cfg |= 1 << 0;
	writel(cfg, &clk->gate_ip_lcd0);

	/*
	 * CLK_DIV_LCD0
	 * FIMD0_RATIO		[3:0]
	 * MIPI0_RATIO		[19:16]
	 * MIPI0_PRE_RATIO	[23:20]
	 * set fimd ratio
	 */
	cfg &= ~(0xf);
	cfg |= 0x1;
	writel(cfg, &clk->div_lcd0);
}

void exynos4_set_mipi_clk(void)
{
	struct exynos4_clock *clk =
	    (struct exynos4_clock *)samsung_get_base_clock();
	unsigned int cfg = 0;

	/*
	 * CLK_SRC_LCD0
	 * FIMD0_SEL		[3:0]
	 * MIPI0_SEL		[12:15]
	 * set mipi0 src clock 0x6: SCLK_MPLL
	 */
	cfg = readl(&clk->src_lcd0);
	cfg &= ~(0xf << 12);
	cfg |= (0x6 << 12);
	writel(cfg, &clk->src_lcd0);

	/*
	 * CLK_SRC_MASK_LCD0
	 * FIMD0_MASK		[0]
	 * MIPI0_MASK		[12]
	 * set src mask mipi0 0x1: Unmask
	 */
	cfg = readl(&clk->src_mask_lcd0);
	cfg |= (0x1 << 12);
	writel(cfg, &clk->src_mask_lcd0);

	/*
	 * CLK_GATE_IP_LCD0
	 * CLK_FIMD0		[0]
	 * CLK_MIE0		[1]
	 * CLK_DSIM0		[3]
	 * CLK_SMMUFIMD0	[4]
	 * CLK_PPMULCD0		[5]
	 * Gating all clocks for MIPI0
	 */
	cfg = readl(&clk->gate_ip_lcd0);
	cfg |= 1 << 3;
	writel(cfg, &clk->gate_ip_lcd0);

	/*
	 * CLK_DIV_LCD0
	 * FIMD0_RATIO		[3:0]
	 * MIPI0_RATIO		[19:16]
	 * MIPI0_PRE_RATIO	[23:20]
	 * set mipi ratio
	 */
	cfg &= ~(0xf << 16);
	cfg |= (0x1 << 16);
	writel(cfg, &clk->div_lcd0);
}

unsigned long get_pll_clk(int pllreg)
{
	if (cpu_is_exynos5()) {
		if (s5p_cpu_id == 0x5260) {
			return exynos5260_get_pll_clk(pllreg);
		} else if(s5p_cpu_id == 0x5430) {
			return exynos5430_get_pll_clk(pllreg);
		} else {
			return exynos5_get_pll_clk(pllreg);
		}
	} else if (cpu_is_exynos7()) {
		if(s5p_cpu_id == 0x7420) {
			return exynos7420_get_pll_clk(pllreg);
		}
	} else
		return exynos4_get_pll_clk(pllreg);
}

unsigned long get_arm_clk(void)
{
	if (cpu_is_exynos5()) {
		if (s5p_cpu_id == 0x5260) {
			return exynos5260_get_arm_clk();
		} else if(s5p_cpu_id == 0x5430) {
			return exynos5430_get_arm_clk();
		} else if(s5p_cpu_id == 0x7420) {
			return exynos7420_get_arm_clk();
		} else {
			return exynos5_get_arm_clk();
		}
	} else
		return exynos4_get_arm_clk();
}

unsigned long get_pwm_clk(void)
{
	if (cpu_is_exynos5()) {
		if (s5p_cpu_id == 0x5260) {
			return exynos5260_get_pwm_clk();
		} else if(s5p_cpu_id == 0x5430) {
			return exynos5430_get_pwm_clk();
		} else {
			return exynos5_get_pwm_clk();
		}
	} else if (cpu_is_exynos7()) {
		if(s5p_cpu_id == 0x7420) {
			return exynos7420_get_pwm_clk();
		}
	} else
		return exynos4_get_pwm_clk();
}

unsigned long get_uart_clk(int dev_index)
{
	if (cpu_is_exynos5()) {
		if (s5p_cpu_id == 0x5260) {
			return exynos5260_get_uart_clk(dev_index);
		} else if(s5p_cpu_id == 0x5430) {
			return exynos5430_get_uart_clk(dev_index);
		} else {
			return exynos5_get_uart_clk(dev_index);
		}
	} else if (cpu_is_exynos7()) {
		if(s5p_cpu_id == 0x7420) {
			return exynos7420_get_uart_clk(dev_index);
		}
	} else
		return exynos4_get_uart_clk(dev_index);
}

unsigned long get_usbdrd_clk(void)
{
	if (cpu_is_exynos5())
		if(s5p_cpu_id == 0x5430) {
			return exynos5430_get_usbdrd_clk();
		} else {
			return exynos5_get_usbdrd_clk();
		}
	else if (cpu_is_exynos7())
		if(s5p_cpu_id == 0x7420) {
			return exynos7420_get_usbdrd_clk();
		}
	else
		return exynos4_get_usbdrd_clk();
}

void set_mmc_clk(int dev_index, unsigned int div)
{
	if (cpu_is_exynos5()) {
		if (s5p_cpu_id == 0x5260) {
			exynos5260_set_mmc_clk(dev_index, div);
		} else if(s5p_cpu_id == 0x5430) {
			exynos5430_set_mmc_clk(dev_index, div);
		} else {
			exynos5_set_mmc_clk(dev_index, div);
		}
	} else if (cpu_is_exynos7()) {
		if(s5p_cpu_id == 0x7420) {
			exynos7420_set_mmc_clk(dev_index, div);
		}
	} else
		exynos4_set_mmc_clk(dev_index, div);
}

unsigned long get_mmc_clk(int dev_index)
{
	if (cpu_is_exynos5()) {
		if (s5p_cpu_id == 0x5260) {
			return exynos5260_get_mmc_clk(dev_index);
		} else if(s5p_cpu_id == 0x5430) {
			return exynos5430_get_mmc_clk(dev_index);
		} else {
			return exynos5_get_mmc_clk(dev_index);
		}
	} else if (cpu_is_exynos7()) {
		if(s5p_cpu_id == 0x7420) {
			return exynos7420_get_mmc_clk(dev_index);
		}
	} else
		return exynos4_get_mmc_clk(dev_index);
}

/* set UNIPRO/MPHY clk */
void set_ufs_clk(int dev_index)
{
	if (cpu_is_exynos5()) {
		if(s5p_cpu_id == 0x5430) {
			exynos5430_set_ufs_clk(dev_index);
		}
	}
}

unsigned long get_lcd_clk(void)
{
	if (cpu_is_exynos4())
		return exynos4_get_lcd_clk();
	else
		return 0;
}

void set_lcd_clk(void)
{
	if (cpu_is_exynos4())
		exynos4_set_lcd_clk();
}

void set_mipi_clk(void)
{
	if (cpu_is_exynos4())
		exynos4_set_mipi_clk();
}
