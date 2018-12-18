/* drivers/video/exynos/decon_7580/panels/s6e3fa0_mipi_lcd.c
 *
 * Samsung SoC MIPI LCD driver.
 *
 * Copyright (c) 2014 Samsung Electronics
 *
 * Haowei Li, <haowei.li@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include "../dsim.h"
#include "lcd_ctrl.h"
#include "decon_lcd.h"

#define MAX_BRIGHTNESS 255
#define MIN_BRIGHTNESS 0
#define DEFAULT_BRIGHTNESS 0

static int s6e3fa0_get_brightness(struct backlight_device *bd)
{
}

static int s6e3fa0_set_brightness(struct backlight_device *bd)
{
	return 1;
}

static int s6e3fa0_probe(struct dsim_device *dsim)
{
	return 1;
}

static int s6e3fa0_displayon(struct dsim_device *dsim)
{
	lcd_init(dsim->id, &dsim->lcd_info);
	lcd_enable_exynos(dsim->id);
	return 1;
}

static int s6e3fa0_suspend(struct dsim_device *dsim)
{
	return 1;
}

static int s6e3fa0_resume(struct dsim_device *dsim)
{
	return 1;
}

struct mipidsim_lcd_driver s6e3fa0_mipi_lcd_driver = {
	.probe		= s6e3fa0_probe,
	.displayon	= s6e3fa0_displayon,
	.suspend		= s6e3fa0_suspend,
	.resume		= s6e3fa0_resume,
};

struct mipidsim_lcd_driver *decon_get_panel_info(void)
{
	return &s6e3fa0_mipi_lcd_driver;
}
