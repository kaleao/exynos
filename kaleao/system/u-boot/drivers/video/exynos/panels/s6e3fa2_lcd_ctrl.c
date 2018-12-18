/* drivers/video/exynos/decon_7580/panels/s6e3fa2_lcd_ctrl.c
 *
 * Samsung SoC MIPI LCD CONTROL functions
 *
 * Copyright (c) 2014 Samsung Electronics
 *
 * Jiun Yu, <jiun.yu@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/
#include <asm/arch/mipi_dsim.h>
#include "s6e3fa2_gamma.h"
#include "s6e3fa2_param.h"
#include "lcd_ctrl.h"
#include "../dsim.h"

#define msleep(a) udelay(a * 1000)

#define GAMMA_PARAM_SIZE 26

#define S6E3FA2_CMD_VFP	3
#define S6E3FA2_CMD_VBP	4
#define S6E3FA2_CMD_HFP	10
#define S6E3FA2_CMD_HBP	10
#define S6E3FA2_CMD_VSA	1
#define S6E3FA2_CMD_HSA	10
#define S6E3FA2_HORIZONTAL	1080
#define S6E3FA2_VERTICAL	1920

struct decon_lcd s6e3fa2_lcd_info = {
	.mode = DECON_MIPI_COMMAND_MODE,
	.vfp = S6E3FA2_CMD_VFP,
	.vbp = S6E3FA2_CMD_VBP,
	.hfp = S6E3FA2_CMD_HFP,
	.hbp = S6E3FA2_CMD_HBP,
	.vsa = S6E3FA2_CMD_VSA,
	.hsa = S6E3FA2_CMD_HSA,
	.xres = S6E3FA2_HORIZONTAL,
	.yres = S6E3FA2_VERTICAL,

	/* Maybe, width and height will be removed */
	.width = 70,
	.height = 121,

	/* Mhz */
	.hs_clk = 1092,
	.esc_clk = 19,

	.fps = 60,
	.mic_enabled = 0,
	.mic_ver = 0,
};

struct decon_lcd *decon_get_lcd_info(void)
{
	return &s6e3fa2_lcd_info;
}


void lcd_init(u32 id, struct decon_lcd *lcd)
{
	if (dsim_wr_data(id, MIPI_DSI_DCS_LONG_WRITE,
			(unsigned long)SEQ_TEST_KEY_ON_F0,
			ARRAY_SIZE(SEQ_TEST_KEY_ON_F0)) < 0)
		dsim_err("failed to send SEQ_TEST_KEY_ON_F0 command.\n");

	dsim_wr_data(id, MIPI_DSI_DCS_SHORT_WRITE, 0x11, 0);

	msleep(120);

	if (dsim_wr_data(id, MIPI_DSI_DCS_LONG_WRITE,
			(unsigned long)SEQ_TEST_KEY_OFF_F0,
			ARRAY_SIZE(SEQ_TEST_KEY_OFF_F0)) < 0)
		dsim_err("failed to send SEQ_TEST_KEY_OFF_F0 command.\n");


	if (dsim_wr_data(id, MIPI_DSI_DCS_SHORT_WRITE,
			SEQ_TE_ON[0], 0) < 0)
		dsim_err("fail to send SEQ_TE_ON command.\n");
}

void lcd_enable_exynos(u32 id)
{
	if (dsim_wr_data(id, MIPI_DSI_DCS_SHORT_WRITE,
			SEQ_DISPLAY_ON[0], 0) < 0)
		dsim_err("fail to send SEQ_DISPLAY_ON command.\n");
}

void lcd_disable(int id)
{
	/* This function needs to implement */
}

int lcd_gamma_ctrl(u32 id, u32 backlightlevel)
{
	int ret;

	ret = dsim_wr_data(id, MIPI_DSI_DCS_LONG_WRITE,
			(unsigned long)gamma22_table[backlightlevel],
			GAMMA_PARAM_SIZE);
	if (ret)
		dsim_err("failed to write gamma value.\n");

	return ret;
}

int lcd_gamma_update(int id)
{
	int ret;

	ret = dsim_wr_data(id, MIPI_DSI_DCS_LONG_WRITE,
			(unsigned long)SEQ_GAMMA_UPDATE,
			ARRAY_SIZE(SEQ_GAMMA_UPDATE));
	if (ret)
		dsim_err("failed to update gamma value.\n");

	return ret;
}
