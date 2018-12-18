/*
 * Copyright (C) 2009 Samsung Electronics
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
#include <asm/arch/smc.h>

/* Each chip has own unique ID */
unsigned int s5p_chip_id[2] = {0x0, 0x0};

/* Default is s5pc100 */
unsigned int s5p_cpu_id = 0xC100;
/* Default is EVT1 */
unsigned int s5p_cpu_rev = 1;
unsigned int s5p_get_cpu_id(void)
{
#if defined(CONFIG_CPU_EXYNOS7420)
	unsigned int pro_id = 0;

	if (*(unsigned int *)CONFIG_SYS_SDRAM_BASE != 0xabcdef) {
		/* Running on DRAM by TRACE32 */
		pro_id = (readl(EXYNOS4_PRO_ID) & 0x00FFF000) >> 12;
	} else {
		pro_id = exynos_smc_read(SMC_CMD_REG, SMC_REG_ID_SFR_R(EXYNOS4_PRO_ID));
		pro_id = (pro_id & 0x00FFF000) >> 12;
	}

#else
	unsigned int pro_id = (readl(EXYNOS4_PRO_ID) & 0x00FFF000) >> 12;
#endif

	switch (pro_id) {
	case 0x200:
		/* Exynos4210 EVT0 */
		return 0x4210;
	case 0x210:
		/* Exynos4210 EVT1 */
		return 0x4210;
	case 0x220:
		/* Exynos4212 */
		return 0x4212;
	case 0x415:
		/* Exynos4415 */
		return 0x4415;
	case 0x260:
		/* Exynos5260 */
		return 0x5260;
	case 0x412:
		pro_id = (readl(EXYNOS4_PRO_ID) & 0x0FFFF000) >> 12;
		if (pro_id == 0x4412) {
			/* Exynos4412 */
			return 0x4412;
		} else {
			/* Exynos5412 */
			return 0x5412;
		}
	case 0x520:
		/* Exynos5250 */
		return 0x5250;
	case 0x410:
		/* Exynos5410 */
		return 0x5410;
	case 0x420:
		/* Exynos7420 */
		return 0x7420;
	case 0x430:
		/* Exynos5430 */
		return 0x5430;
	case 0x433:
		/* Exynos5433 - use almost same codes with Exynos5430 */
		return 0x5430;
#if defined(CONFIG_CPU_EXYNOS3250) || defined(CONFIG_CPU_EXYNOS3472)
	case 0x472:
#if defined(CONFIG_CPU_EXYNOS3250)
		/* Exynos3250 */
		return 0x3250;
#elif defined(CONFIG_CPU_EXYNOS3472)
		/* Exynos3472 */
		return 0x3472;
#endif
#endif
	case 0x580:
		/* Exynos7580 */
		return 0x7580;
	}
}
void s5p_set_cpu_id(void)
{
#if defined(CONFIG_CPU_EXYNOS7420)
	unsigned int rev_num = 0;

	if (*(unsigned int *)CONFIG_SYS_SDRAM_BASE != 0xabcdef) {
		/* Running on DRAM by TRACE32 */
		rev_num = (readl(EXYNOS4_PRO_ID) & 0x000000F0) >> 4;
	} else {
		rev_num = exynos_smc_read(SMC_CMD_REG, SMC_REG_ID_SFR_R(EXYNOS4_PRO_ID));
		rev_num = (rev_num & 0x000000F0) >> 4;
	}
#else
	unsigned int rev_num = (readl(EXYNOS4_PRO_ID) & 0x000000F0) >> 4;
#endif

	s5p_cpu_id = s5p_get_cpu_id();

	if(s5p_cpu_id == 0x4210)
		s5p_cpu_rev = 0;
	else
		s5p_cpu_rev = rev_num;
}

#ifdef CONFIG_ARCH_CPU_INIT
int arch_cpu_init(void)
{
	s5p_set_cpu_id();

	return 0;
}
#endif

void arch_preboot_os(ulong start , ulong end )
{
#ifdef CONFIG_OF_LIBFDT
	prepare_dtb(start,end);
#endif
}
void arch_preboot_initrd(ulong start , ulong end )
{
#ifdef CONFIG_OF_LIBFDT
	prepare_dtb_initrd(start,end);
#endif
}

u32 get_device_type(void)
{
	return s5p_cpu_id;
}

#ifdef CONFIG_DISPLAY_CPUINFO
int print_cpuinfo(void)
{
#if defined(CONFIG_EXYNOS5) || defined(CONFIG_ARCH_EXYNOS5)
	unsigned int cpuid;
	unsigned int subrev;

	__asm__ __volatile__("mrs %0, midr_el1":"=r"(cpuid));
	subrev = (readl(EXYNOS5_PRO_ID) & 0x0000000F);

	printf("CPU: %s%x Rev%x.%x [Samsung SOC on SMP Platform Base on ARM CortexA5%d]\n"	\
		, s5p_get_cpu_name(), s5p_cpu_id, s5p_cpu_rev, subrev, ((cpuid >> 4) & 0xf));

#if defined(CONFIG_CPU_EXYNOS5430)
#ifdef CONFIG_CPU_EXYNOS5433
	return 0;
#endif
	if (s5p_cpu_rev != 0)
		return 0;
	unsigned int egl_pll = get_pll_clk(EGL_PLL);
	unsigned int kfc_pll = get_pll_clk(KFC_PLL);
	unsigned int mem_pll = get_pll_clk(MEM_PLL);
	unsigned int bus_pll = get_pll_clk(BUS_PLL);

	printf("EGL_PLL = %ldMHz, KFC_PLL = %ldMHz\n",
					egl_pll/1000000, kfc_pll/1000000);
	printf("MEM_PLL = %ldMHz, BUS_PLL = %ldMHz\n",
					mem_pll/1000000, bus_pll/1000000);

#endif
#elif defined(CONFIG_ARCH_EXYNOS7)
	unsigned int cpuid;
	unsigned int subrev;

	__asm__ __volatile__("mrs %0, midr_el1":"=r"(cpuid));
#if defined(CONFIG_CPU_EXYNOS7420)
	if (*(unsigned int *)CONFIG_SYS_SDRAM_BASE != 0xabcdef) {
		/* Running on DRAM by TRACE32 */
		subrev = (readl(EXYNOS7_PRO_ID) & 0x0000000F);
	} else {
		subrev = exynos_smc_read(SMC_CMD_REG, SMC_REG_ID_SFR_R(EXYNOS7_PRO_ID));
		subrev = subrev & 0x0000000F;
	}
#else
	subrev = (readl(EXYNOS7_PRO_ID) & 0x0000000F);
#endif

	printf("CPU: %s%x Rev%x.%x [Samsung SOC on SMP Platform Base on ARM CortexA5%d]\n"	\
		, s5p_get_cpu_name(), s5p_cpu_id, s5p_cpu_rev, subrev, ((cpuid >> 4) & 0xf));

#ifdef CONFIG_CPU_EXYNOS7420
	unsigned long atlas_pll = get_pll_clk(ATLAS_PLL);
	unsigned long apollo_pll = get_pll_clk(APOLLO_PLL);
	unsigned long mif_pll = get_pll_clk(MIF_PLL);
	unsigned long mfc_pll = get_pll_clk(MFC_PLL);
	unsigned long cci_pll = get_pll_clk(CCI_PLL);
	unsigned long bus0_pll = get_pll_clk(BUS0_PLL);
	unsigned long bus1_pll = get_pll_clk(BUS1_PLL);

	printf("ATLAS_PLL = %ldMHz\tAPOLLO_PLL = %ldMHz\tMIF_PLL = %ldMHz\n",
		atlas_pll/1000000, apollo_pll/1000000, mif_pll/1000000);
	printf("MFC_PLL = %ldMHz\tCCI_PLL = %ldMHz\n",
		mfc_pll/1000000, cci_pll/1000000);
	printf("BUS0_PLL = %ldMHz\tBUS1_PLL = %ldMHz\n",
		bus0_pll/1000000, bus1_pll/1000000);
#endif

#ifdef CONFIG_CPU_EXYNOS7580
       unsigned long cpu_pll = get_pll_clk(PLL_CPU);
       unsigned long apl_pll = get_pll_clk(PLL_APL);
       unsigned long mem0_pll = get_pll_clk(PLL_MEM0);
       unsigned long media_pll = get_pll_clk(PLL_MEDIA);
       unsigned long bus_pll = get_pll_clk(PLL_BUS);
       unsigned long usb_pll = get_pll_clk(PLL_USB);

       printf("PLL_CPU = %ldMHz\tPLL_APL = %ldMHz\n",
               cpu_pll/1000000, apl_pll/1000000);
       printf("PLL_MEM0 = %ldMHz\tPLL_MEDIA = %ldMHz\n",
               mem0_pll/1000000, media_pll/1000000);
       printf("PLL_BUS = %ldMHz\tPLL_USB = %ldMHz\n",
               bus_pll/1000000, usb_pll/1000000);
#endif

#endif

	return 0;
}
#endif
