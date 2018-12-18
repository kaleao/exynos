/* drivers/video/exynos/panels/s6tnmr7_mipi_lcd.c
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

#include <asm/arch/mipi_dsim.h>

#include "s6tnmr7_param.h"
#include "lcd_ctrl.h"
#include "../dsim.h"
#include "decon_lcd.h"

#define MAX_BRIGHTNESS 255
#define MIN_BRIGHTNESS 0
#define DEFAULT_BRIGHTNESS 0
#define msleep(a) udelay(a * 1000)

struct decon_lcd s6tnmr7_lcd_info = {
	.mode = DECON_MIPI_COMMAND_MODE,
	.vfp = 10,
	.vbp = 10,
	.hfp = 30,
	.hbp = 30,
	.vsa = 1,
	.hsa = 1,
	.xres = LCD_WIDTH,
	.yres = LCD_HEIGHT,

	/* Maybe, width and height will be removed */
	.width = 71,
	.height = 114,

	/* Mhz */
	.hs_clk = 800,
	.esc_clk = 20,

	.fps = 60,
	.mic_enabled = 0,//data compress !! Attenton
};

struct decon_lcd *decon_get_lcd_info(void)
{
	return &s6tnmr7_lcd_info;
}

void lcd_init(u32 id, struct decon_lcd *lcd)
{
	unsigned char TSP_TE[] = {0xB1, 0x06, 0xF8};

	msleep(1);
	printf("+++%s+++\n", __func__);
	if(dsim_wr_data(id, MIPI_DSI_DCS_SHORT_WRITE, (unsigned long)0x11, 0))
		printf("failed to send 0x11\n");
	msleep(120);
	if(dsim_wr_data(id, MIPI_DSI_DCS_SHORT_WRITE_PARAM, (unsigned long)0x82, (unsigned long)(s6tgamma105)))
		printf("failed to send 0x82\n");
	msleep(5);
	if(dsim_wr_data(id, MIPI_DSI_DCS_LONG_WRITE, (unsigned long)s6tAOR105, sizeof(s6tAOR105)))
		printf("failed to send AOR\n");
	msleep(5);
	if(dsim_wr_data(id, MIPI_DSI_DCS_SHORT_WRITE_PARAM, (unsigned long)0xB0, 0x34))
		printf("failed to send 0xB0\n");
	msleep(5);
	if(dsim_wr_data(id, MIPI_DSI_DCS_SHORT_WRITE_PARAM, (unsigned long)0xBB, (unsigned long)(s6tACL105)))
		printf("failed to send 0xBB\n");
	msleep(5);
	if(dsim_wr_data(id, MIPI_DSI_DCS_SHORT_WRITE_PARAM, (unsigned long)0xB0, 0x2E))
		printf("failed to send 0xB0\n");
	msleep(5);
	if(dsim_wr_data(id, MIPI_DSI_DCS_SHORT_WRITE_PARAM, (unsigned long)0xBB, 0x01))
		printf("failed to send 0xBB\n");
	msleep(5);
	if(dsim_wr_data(id, MIPI_DSI_DCS_SHORT_WRITE_PARAM, (unsigned long)0xB0, 0xC7))
		printf("failed to send 0xB0\n");
	msleep(5);
	if(dsim_wr_data(id, MIPI_DSI_DCS_LONG_WRITE, (unsigned long)TSP_TE, sizeof(TSP_TE)))
		printf("failed to send TSP_TE\n");
	msleep(5);
	if(dsim_wr_data(id, MIPI_DSI_DCS_SHORT_WRITE, (unsigned long)0x29, 0))
		printf("failed to send 0x29\n");
	msleep(20);
	printf("---%s---\n", __func__);
	return 1;
}

static int s6tnmr7_probe(struct dsim_device *dsim)
{
	return 0;
}

static int s6tnmr7_displayon(struct dsim_device *dsim)
{
    dsim_dbg("+++%s+++\n",__func__);
    struct decon_lcd *lcd = decon_get_lcd_info();
    
	lcd_init(dsim->id, lcd);

    	dsim_dbg("---%s---\n",__func__);
	return 0;
}

static int s6tnmr7_suspend(struct dsim_device *dsim)
{
	return 0;
}

static int s6tnmr7_resume(struct dsim_device *dsim)
{
	return 0;
}

struct mipidsim_lcd_driver s6tnmr7_mipi_lcd_driver = {
	.probe		= s6tnmr7_probe,
	.displayon	= s6tnmr7_displayon,
	.suspend		= s6tnmr7_suspend,
	.resume		= s6tnmr7_resume,
};

struct mipidsim_lcd_driver *decon_get_panel_info(void)
{
	return &s6tnmr7_mipi_lcd_driver;
}

