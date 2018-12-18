/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * Header file for Exynos DECON driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef ___SAMSUNG_DEOCN_H__
#define ___SAMSUNG_DECON_H__

#include <config.h>
#include <common.h>
#include <linux/err.h>

#ifdef CONFIG_CPU_EXYNOS7420
#include "regs-decon.h"
#elif defined(CONFIG_CPU_EXYNOS7580)
#include "regs-decon-7580.h"
#endif

#include "decon_common.h"
#include "./panels/decon_lcd.h"

extern struct decon_device *decon_int_drvdata;
extern struct decon_device *decon_ext_drvdata;
extern int decon_log_level;

#define VSYNC_TIMEOUT_MSEC	70
#define MAX_BW_PER_WINDOW	(2560 * 1600 * 4 * 60)
#define MAX_DECON_ID		1
#define MAX_VPP_SUBDEV          4

#define decon_err(fmt, ...)							\
	do {									\
		if (decon_log_level >= 3)					\
			printf(fmt, ##__VA_ARGS__);			\
	} while (0)

#define decon_warn(fmt, ...)							\
	do {									\
		if (decon_log_level >= 4)					\
			printf(fmt, ##__VA_ARGS__);			\
	} while (0)

#define decon_info(fmt, ...)							\
	do {									\
		if (decon_log_level >= 5)					\
			printf(fmt, ##__VA_ARGS__);			\
	} while (0)

#define decon_dbg(fmt, ...)							\
	do {									\
		if (decon_log_level >= 6)					\
			printf(fmt, ##__VA_ARGS__);			\
	} while (0)

enum decon_ip_version {
	IP_VER_DECON_7I,
};

struct exynos_decon_platdata {
	enum decon_ip_version	ip_ver;
	enum decon_psr_mode	psr_mode;
	enum decon_trig_mode	trig_mode;
	enum decon_dsi_mode	dsi_mode;
	int	max_win;
	int	default_win;
};

struct decon_device {
	size_t				regs;
	size_t				sys_regs;
	struct exynos_decon_platdata	*pdata;

	int	mic_enabled;
	int	id;
};

static inline struct decon_device *get_decon_drvdata(u32 id)
{
	if (id)
		return decon_ext_drvdata;
	else
		return decon_int_drvdata;
}

/* register access subroutines */
static inline u32 decon_read(u32 id, u32 reg_id)
{
	struct decon_device *decon = get_decon_drvdata(id);
	return readl(decon->regs + reg_id);
}

static inline u32 decon_read_mask(u32 id, u32 reg_id, u32 mask)
{
	u32 val = decon_read(id, reg_id);
	val &= (~mask);
	return val;
}

static inline void decon_write(u32 id, u32 reg_id, u32 val)
{
	struct decon_device *decon = get_decon_drvdata(id);
	writel(val, decon->regs + reg_id);
}

static inline void decon_write_mask(u32 id, u32 reg_id, u32 val, u32 mask)
{
	struct decon_device *decon = get_decon_drvdata(id);
	u32 old = decon_read(id, reg_id);

	val = (val & mask) | (old & ~mask);
	writel(val, decon->regs + reg_id);
}

#endif /* SAMSUNG_DECON_H */
