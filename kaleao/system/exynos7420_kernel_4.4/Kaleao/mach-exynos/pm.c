/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * EXYNOS - Power Management support
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <linux/init.h>
#include <linux/suspend.h>
#include <linux/syscore_ops.h>
#include <linux/io.h>
#include <linux/err.h>
#include <asm/suspend.h>

#include <mach/regs-clock.h>
#include <mach/regs-pmu.h>
#ifndef CONFIG_SOC_EXYNOS7580
#include <mach/exynos-powermode.h>
#else
#include <mach/exynos-powermode-smp.h>
#endif
#include <mach/pmu_cal_sys.h>

#define PSCI_INDEX_SLEEP		0x8

#define REG_INFORM0	(S5P_VA_SYSRAM_NS + 0x8)
#define REG_INFORM1	(S5P_VA_SYSRAM_NS + 0xC)

#define WAKEUP_STAT_EINT		(1 << 0)
#define WAKEUP_STAT_RTC_ALARM		(1 << 1)

static void exynos_show_wakeup_reason(void)
{
	unsigned int wakeup_stat;

	wakeup_stat = __raw_readl(EXYNOS_PMU_WAKEUP_STAT);

	if (wakeup_stat & WAKEUP_STAT_RTC_ALARM)
		pr_info("Resume caused by RTC alarm\n");
	else
		pr_info("Resume caused by wakeup_stat 0x%08x\n",
			wakeup_stat);
}

static int exynos_pm_suspend(void)
{
	exynos_prepare_sys_powerdown(SYS_SLEEP);

	if (!(__raw_readl(EXYNOS_PMU_PMU_DEBUG) & CLKOUT_DISABLE))
		__raw_writel(0x1, EXYNOS_PMU_XXTI_SYS_PWR_REG);

	return 0;
}

static void exynos_cpu_prepare(void)
{
	/* inform0: return address, inform1: sleep mode */
	__raw_writel(EXYNOS_CHECK_SLEEP, REG_INFORM1);
	__raw_writel(virt_to_phys(cpu_resume), REG_INFORM0);
}

static int __maybe_unused exynos_cpu_suspend(unsigned long arg)
{
	/* This rule is moved to PSCI */
	return 1; /* abort suspend */
}

static void exynos_pm_resume(void)
{
	bool sleep_abort = exynos_sys_powerdown_enabled();

	exynos_wakeup_sys_powerdown(SYS_SLEEP, sleep_abort);

	exynos_show_wakeup_reason();
	return;
}

static int exynos_pm_enter(suspend_state_t state)
{
	int ret;

	pr_debug("%s: state %d\n", __func__, state);

	exynos_cpu_prepare();

	/* This will also act as our return point when
	 * we resume as it saves its own register state and restores it
	 * during the resume. */
	ret = cpu_suspend(PSCI_INDEX_SLEEP);
	if (ret) {
		pr_info("%s: return to originator\n", __func__);
		return ret;
	}

	pr_debug("%s: post sleep, preparing to return\n", __func__);
	return 0;
}

static int exynos_pm_prepare(void)
{
	/* TODO */
	return 0;
}

static void exynos_pm_finish(void)
{
	/* TODO */
}

static const struct platform_suspend_ops exynos_pm_ops = {
	.enter		= exynos_pm_enter,
	.prepare	= exynos_pm_prepare,
	.finish		= exynos_pm_finish,
	.valid		= suspend_valid_only_mem,
};

static __init int exynos_pm_drvinit(void)
{
	pr_info("Exynos PM intialize\n");

	suspend_set_ops(&exynos_pm_ops);
	return 0;
}
arch_initcall(exynos_pm_drvinit);

static struct syscore_ops exynos_pm_syscore_ops = {
	.suspend	= exynos_pm_suspend,
	.resume		= exynos_pm_resume,
};

static __init int exynos_pm_syscore_init(void)
{
	pr_info("Exynos PM syscore intialize\n");

	register_syscore_ops(&exynos_pm_syscore_ops);
	return 0;
}
arch_initcall(exynos_pm_syscore_init);
