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

#ifndef __EXYNOS_POWERMODE_SMP_H
#define __EXYNOS_POWERMODE_SMP_H __FILE__

#include <asm/io.h>

enum sys_powerdown {
	SYS_AFTR,
	SYS_STOP,
	SYS_DSTOP,
	SYS_DSTOP_PSR,
	SYS_LPD,
	SYS_LPA,
	SYS_SLEEP,
	NUM_SYS_POWERDOWN,
	SYS_ALPA,
};

struct exynos_pmu_conf {
	void __iomem *reg;
	unsigned int val[NUM_SYS_POWERDOWN];
};

/* SFR bit control to enter system power down */
struct sfr_bit_ctrl {
	void __iomem	*reg;
	unsigned int	mask;
	unsigned int	val;
};

struct sfr_save {
	void __iomem	*reg;
	unsigned int	val;
};

#define SFR_CTRL(_reg, _mask, _val)	\
	{				\
		.reg	= _reg,		\
		.mask	= _mask,	\
		.val	= _val,		\
	}

#define SFR_SAVE(_reg)			\
	{				\
		.reg	= _reg,		\
	}

/* setting SFR helper function */
static inline void exynos_set_sfr(struct sfr_bit_ctrl *ptr, int count)
{
	unsigned int tmp;

	for (; count > 0; count--, ptr++) {
		tmp = __raw_readl(ptr->reg);
		__raw_writel((tmp & ~ptr->mask) | ptr->val, ptr->reg);
	}
}

static inline void exynos_save_sfr(struct sfr_save *ptr, int count)
{
	for (; count > 0; count--, ptr++)
		ptr->val = __raw_readl(ptr->reg);
}

static inline void exynos_restore_sfr(struct sfr_save *ptr, int count)
{
	for (; count > 0; count--, ptr++)
		__raw_writel(ptr->val, ptr->reg);
}

/* power mode check flag */
#define EXYNOS_CHECK_DIDLE	0xBAD00000
#define EXYNOS_CHECK_LPA	0xABAD0000
#define EXYNOS_CHECK_DSTOP	0xABAE0000
#define EXYNOS_CHECK_SLEEP	0x00000BAD

#define MAX_NUM_REGS		20

struct hw_info {
	void __iomem	*addr;
	unsigned int	mask;
};

static inline int check_hw_status(struct hw_info *regs, unsigned int num)
{
	while (num--)
		if (__raw_readl(regs[num].addr) & regs[num].mask)
			return -EBUSY;

	return 0;
}

#if defined(CONFIG_PWM_SAMSUNG)
extern int pwm_check_enable_cnt(void);
#else
static int pwm_check_enable_cnt(void)
{
	return 0;
}
#endif

#ifdef CONFIG_SERIAL_SAMSUNG
extern void s3c24xx_serial_fifo_wait(void);
#else
static void s3c24xx_serial_fifo_wait(void)
{
}
#endif

extern int determine_lpm(void);
extern int determine_cpd(int index, int c2_index, unsigned int cpu,
			 unsigned int target_residency);
extern void wakeup_from_c2(unsigned int cpu);

/* system power down function */
extern void exynos_prepare_sys_powerdown(enum sys_powerdown mode);
extern void exynos_wakeup_sys_powerdown(enum sys_powerdown mode,
					bool early_wakeup);
extern bool exynos_sys_powerdown_enabled(void);

#endif /* __EXYNOS_POWERMODE_SMP_H */
