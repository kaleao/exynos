/* drivers/video/exynos/decon_7580/panels/s6e3fa0_lcd_ctrl.c
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
#include "s6e3fa0_gamma.h"
#include "s6e3fa0_param.h"
#include "lcd_ctrl.h"
#include "../dsim.h"

#define GAMMA_PARAM_SIZE 26

#define S6E3FA0_CMD_VBP		10
#define S6E3FA0_CMD_VFP		1
#define S6E3FA0_CMD_VSA		1
#define S6E3FA0_CMD_HBP		1
#define S6E3FA0_CMD_HFP		1
#define S6E3FA0_CMD_HSA		1
#define S6E3FA0_HORIZONTAL	1080
#define S6E3FA0_VERTICAL	1920

struct decon_lcd s6e3fa0_lcd_info = {
	.mode = DECON_MIPI_COMMAND_MODE,
	.vfp = S6E3FA0_CMD_VFP,
	.vbp = S6E3FA0_CMD_VBP,
	.hfp = S6E3FA0_CMD_HFP,
	.hbp = S6E3FA0_CMD_HBP,
	.vsa = S6E3FA0_CMD_VSA,
	.hsa = S6E3FA0_CMD_HSA,
	.xres = S6E3FA0_HORIZONTAL,
	.yres = S6E3FA0_VERTICAL,

	/* Maybe, width and height will be removed */
	.width = 70,
	.height = 121,

	/* Mhz */
	.hs_clk = 1100,
	.esc_clk = 20,

	.fps = 60,
	.mic_enabled = 0,
	.mic_ver = 0,
};

struct decon_lcd *decon_get_lcd_info(void)
{
	return &s6e3fa0_lcd_info;
}


void lcd_init(u32 id, struct decon_lcd *lcd)
{
	if (dsim_wr_data(id, MIPI_DSI_DCS_LONG_WRITE,
				(unsigned long)SEQ_TEST_KEY_ON_F0,
				ARRAY_SIZE(SEQ_TEST_KEY_ON_F0)) < 0)
		dsim_err("failed to send SEQ_TEST_KEY_ON_F0 command.\n");
	mdelay(12);

	if (dsim_wr_data(id, MIPI_DSI_DCS_LONG_WRITE,
				(unsigned long)SEQ_TEST_KEY_ON_F1,
				ARRAY_SIZE(SEQ_TEST_KEY_ON_F1)) < 0)
		dsim_err("fail to send SEQ_TEST_KEY_ON_F1 command.\n");
	mdelay(12);

	if (dsim_wr_data(id, MIPI_DSI_DCS_LONG_WRITE,
				(unsigned long)SEQ_TEST_KEY_ON_FC,
				ARRAY_SIZE(SEQ_TEST_KEY_ON_FC)) < 0)
		dsim_err("fail to send SEQ_TEST_KEY_ON_FC command.\n");
	mdelay(12);

	if (dsim_wr_data(id, MIPI_DSI_DCS_LONG_WRITE,
				(unsigned long)SEQ_TEST_KEY_ON_ED,
				ARRAY_SIZE(SEQ_TEST_KEY_ON_ED)) < 0)
		dsim_err("fail to send SEQ_TEST_KEY_ON_ED command.\n");
	mdelay(12);


	if (dsim_wr_data(id, MIPI_DSI_DCS_LONG_WRITE,
			(unsigned long)SEQ_TEST_KEY_ON_FD,
			ARRAY_SIZE(SEQ_TEST_KEY_ON_FD)) < 0)
		dsim_err(
		"fail to send SEQ_TEST_KEY_ON_FD command.\n");
	mdelay(12);

	if (dsim_wr_data(id, MIPI_DSI_DCS_SHORT_WRITE_PARAM,
				SEQ_TEST_KEY_ON_F6[0],
				SEQ_TEST_KEY_ON_F6[1]) < 0)
		dsim_err(
		"fail to send SEQ_TEST_KEY_ON_F6 command.\n");
	mdelay(12);


	if (dsim_wr_data(id, MIPI_DSI_DCS_SHORT_WRITE,
				SEQ_SLEEP_OUT[0], 0) < 0)
		dsim_err("fail to send SEQ_SLEEP_OUT command.\n");
	mdelay(20);

	if (dsim_wr_data(id, MIPI_DSI_DCS_LONG_WRITE,
				(unsigned long)SEQ_TEST_KEY_ON_EB,
				ARRAY_SIZE(SEQ_TEST_KEY_ON_EB)) < 0)
		dsim_err("fail to send SEQ_TEST_KEY_ON_EB command.\n");
	mdelay(12);

	if (dsim_wr_data(id, MIPI_DSI_DCS_LONG_WRITE,
				(unsigned long)SEQ_TEST_KEY_ON_C0,
				ARRAY_SIZE(SEQ_TEST_KEY_ON_C0)) < 0)
		dsim_err("fail to send SEQ_TEST_KEY_ON_C0 command.\n");
	mdelay(12);

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
