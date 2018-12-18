/*
 * Header file for Samsung MIPI-DSI common driver.
 *
 * Copyright (c) 2013 Samsung Electronics
 * Haowei Li <haowei.li@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef __DSIM_H__
#define __DSIM_H__

#include <config.h>
#include <common.h>
#include <linux/err.h>
#include <lcd.h>

#ifdef CONFIG_CPU_EXYNOS7420
#include "regs-dsim.h"
#elif defined(CONFIG_CPU_EXYNOS7580)
#include "regs-dsim-7580.h"
#endif
#include "dsim_common.h"

extern int decon_log_level;

#define DSIM_RX_FIFO_READ_DONE	(0x30800002)
#define DSIM_MAX_RX_FIFO	(64)

#define dsim_err(fmt, ...)							\
	do {									\
		if (decon_log_level >= 3) 					\
			printf(fmt, ##__VA_ARGS__);				\
	} while (0)

#define dsim_info(fmt, ...)							\
	do {									\
		if (decon_log_level >= 5) 					\
			printf(fmt, ##__VA_ARGS__);				\
	} while (0)

#define dsim_dbg(fmt, ...)							\
	do {									\
		if (decon_log_level >= 6) 					\
			printf(fmt, ##__VA_ARGS__);				\
	} while (0)

#define call_panel_ops(q, op, args...)					\
	(((q)->panel_ops->op) ? ((q)->panel_ops->op(args)) : 0)

#define call_config_ops(q, op, args...)					\
	(((q)->config_ops->op) ? ((q)->config_ops->op(args)) : 		\
	printf("%s: Operation("#op")isn't mapped\n", __func__))

extern struct dsim_device *dsim0_for_decon;
extern struct dsim_device *dsim1_for_decon;

/* operation state of dsim driver */
enum dsim_state {
	DSIM_STATE_HSCLKEN,	/* HS clock was enabled. */
	DSIM_STATE_ULPS,	/* DSIM was entered ULPS state */
	DSIM_STATE_SUSPEND	/* DSIM is suspend state */
};

struct dsim_device {
	size_t reg_base;

	enum dsim_state state;

	unsigned int data_lane;
	unsigned long hs_clk;
	unsigned long byte_clk;
	unsigned long escape_clk;
	unsigned char freq_band;
	unsigned char id;

	struct decon_lcd	*lcd_info;
	struct dphy_timing_value	timing;

	u32 data_lane_cnt;
	struct mipidsim_lcd_driver *panel_ops;

	enum board_gpio_type board_type;
	struct exynos_display_config *config_ops;
};

/**
 * driver structure for mipi-dsi based lcd panel.
 *
 * this structure should be registered by lcd panel driver.
 * mipi-dsi driver seeks lcd panel registered through name field
 * and calls these callback functions in appropriate time.
 */

struct mipidsim_lcd_driver {
	int	(*probe)(struct dsim_device *dsim);
	int	(*suspend)(struct dsim_device *dsim);
	int	(*displayon)(struct dsim_device *dsim);
	int	(*resume)(struct dsim_device *dsim);
};

int dsim_write_data(struct dsim_device *dsim, unsigned int
	data_id, unsigned long data0, unsigned int data1);
int dsim_read_data(struct dsim_device *dsim, u32 data_id,
	unsigned long addr, u32 count, u8 *buf);

#ifdef CONFIG_DECON_LCD_S6E8FA0X01
//extern struct mipidsim_lcd_driver s6e3ha0_mipi_lcd_driver;
#endif

#ifdef CONFIG_DECON_LCD_S6E8AA0
extern struct mipidsim_lcd_driver s6e8aa0_mipi_lcd_driver;
#endif

#ifdef CONFIG_DECON_LCD_S6E3FA0
extern struct mipidsim_lcd_driver s6e3fa0_mipi_lcd_driver;
#endif

enum mipi_ddi_interface {
	RGB_IF = 0x4000,
	I80_IF = 0x8000,
	YUV_601 = 0x10000,
	YUV_656 = 0x20000,
	MIPI_VIDEO = 0x1000,
	MIPI_COMMAND = 0x2000,
};

enum mipi_ddi_panel_select {
	DDI_MAIN_LCD = 0,
	DDI_SUB_LCD = 1,
};

enum mipi_ddi_model {
	S6DR117 = 0,
};

enum mipi_ddi_parameter {
	/* DSIM video interface parameter */
	DSI_VIRTUAL_CH_ID = 0,
	DSI_FORMAT = 1,
	DSI_VIDEO_MODE_SEL = 2,
};

static inline struct dsim_device *get_dsim_drvdata(u32 id)
{
	if (id)
		return dsim1_for_decon;
	else
		return dsim0_for_decon;
}

static inline int dsim_rd_data(u32 id, u32 cmd_id, unsigned long d0, u32 d1, u8 *buf)
{
	int ret;
	struct dsim_device *dsim = get_dsim_drvdata(id);

	ret = dsim_read_data(dsim, cmd_id, d0, d1, buf);
	if (ret)
		return ret;

	return 0;
}

static inline int dsim_wr_data(u32 id, u32 cmd_id, unsigned long d0, u32 d1)
{
	int ret;
	struct dsim_device *dsim = get_dsim_drvdata(id);

	ret = dsim_write_data(dsim, cmd_id, d0, d1);
	if (ret)
		return ret;

	return 0;
}

/* register access subroutines */
static inline u32 dsim_read(u32 id, u32 reg_id)
{
	struct dsim_device *dsim = get_dsim_drvdata(id);
	return readl(dsim->reg_base + reg_id);
}

static inline u32 dsim_read_mask(u32 id, u32 reg_id, u32 mask)
{
	u32 val = dsim_read(id, reg_id);
	val &= (~mask);
	return val;
}

static inline void dsim_write(u32 id, u32 reg_id, u32 val)
{
	struct dsim_device *dsim = get_dsim_drvdata(id);
	writel(val, dsim->reg_base + reg_id);
}

static inline void dsim_write_mask(u32 id, u32 reg_id, u32 val, u32 mask)
{
	struct dsim_device *dsim = get_dsim_drvdata(id);
	u32 old = dsim_read(id, reg_id);

	val = (val & mask) | (old & ~mask);
	writel(val, dsim->reg_base + reg_id);
}

/* CAL APIs list */
int dsim_probe(u32 dev_id);

#endif /* __DSIM_H__ */
