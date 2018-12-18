/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com/
 *
 * EXYNOS - PMU(Power Management Unit) support
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef __EXYNOS_PMU_H
#define __EXYNOS_PMU_H __FILE__

#ifndef CONFIG_SOC_EXYNOS7580
#include <mach/exynos-powermode.h>
#else
#include <mach/exynos-powermode-smp.h>
#endif

/* clsuter type */
enum cluster_type {
	BIG,
	LITTLE,
	CLUSTER_TYPE_MAX,
};


/* cpu power operation */
struct exynos_cpu_power_ops {
	void (*power_up)(unsigned int cpu_id);
	void (*power_down)(unsigned int cpu_id);
	int (*power_state)(unsigned int cpu_id);
};

extern struct exynos_cpu_power_ops exynos_cpu;


/* wakeup mask */
extern void exynos_set_wakeupmask(enum sys_powerdown mode);
extern void exynos_clear_wakeupmask(void);

#ifdef CONFIG_PINCTRL_EXYNOS
extern u64 exynos_get_eint_wake_mask(void);
#else
static inline u64 exynos_get_eint_wake_mask(void) { return 0xffffffffL; }
#endif


/* cpu sequencer control function */
extern void exynos_cpu_sequencer_ctrl(bool enable);


/* watch dog timer control */
enum type_pmu_wdt_reset {
	PMU_WDT_RESET_TYPE0 = 0,
	PMU_WDT_RESET_TYPE1,
};

extern void exynos_pmu_wdt_control(bool on, unsigned int pmu_wdt_reset_type);

#endif /* __EXYNOS_PMU_H */