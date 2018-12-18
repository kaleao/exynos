/*
 * Samsung SoC MIPI LCD CONTROL functions
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/
#include <asm/arch/mipi_dsim.h>

#include "lcd_ctrl.h"
#include "../dsim.h"

#define GAMMA_PARAM_SIZE 26
#if 0
s6e8fa0: s6e8fa0_fhd {
  mode = <0>; /* 0: MIPI video mode */
  resolution = <1080 1920>;
  size = <71 114>;
  timing,refresh = <60>;
  timing,h-porch = <14 30 10>;
  timing,v-porch = <1 2 1>;
  timing,dsi-hs-clk = <960>;
  timing,dsi-escape-clk = <20>;
  mic = <0>;
  mic_ver = <1>;         /* 0: mic v1.1, 1: v1.2, 2: v2.0 */
  type_of_ddi = <0>; /* 0:Samsung Mobile */
};
#endif

/* These need to define */
#define S6E8FA0X01_VIDEO_VBP	1
#define S6E8FA0X01_VIDEO_VFP	2
#define S6E8FA0X01_VIDEO_VSA	1
#define S6E8FA0X01_VIDEO_HBP	14
#define S6E8FA0X01_VIDEO_HFP	30
#define S6E8FA0X01_VIDEO_HSA	10

#define S6E8FA0X01_HORIZONTAL	1080
#define S6E8FA0X01_VERTICAL	1920

#define msleep(a) udelay(a * 1000)

struct decon_lcd s6e8fa0x01_lcd_info = {
	.mode = DECON_VIDEO_MODE,
	.vfp = S6E8FA0X01_VIDEO_VFP,
	.vbp = S6E8FA0X01_VIDEO_VBP,
	.hfp = S6E8FA0X01_VIDEO_HFP,
	.hbp = S6E8FA0X01_VIDEO_HBP,
	.vsa = S6E8FA0X01_VIDEO_VSA,
	.hsa = S6E8FA0X01_VIDEO_HSA,
	.xres = S6E8FA0X01_HORIZONTAL,
	.yres = S6E8FA0X01_VERTICAL,

	/* Maybe, width and height will be removed */
	.width = 71,
	.height = 114,

	/* Mhz */
	.hs_clk = 960,
	.esc_clk = 20,

	.fps = 60,
	.mic_enabled = 0,
};

struct decon_lcd *decon_get_lcd_info(void)
{
	return &s6e8fa0x01_lcd_info;
}

/*
 * Read ID for LCD panel
*/
#if 0
static int lcd_read_id(u32 id)
{
	int i, ret;
	u8 buf[LDI_ID_LEN + 1] = {0,};
	unsigned int lcd_type;

	ret = dsim_rd_data(id, MIPI_DSI_DCS_READ, LDI_ID_REG, LDI_ID_LEN, buf);

	if (!ret) {
		dsim_err("%s: failed\n", __func__);
		return -EBUSY;
	}

	lcd_type = buf[0] << 16 | buf[1] << 8 | buf[2];

	dsim_dbg("%s is successfully , data = 0x%08x\n", __func__, lcd_type);

	return ret;
}
#endif
/*
 * 3FAH0 lcd init sequence
 *
 * Parameters
 *	- mic_enabled : if mic is enabled, MIC_ENABLE command must be sent
 *	- mode : LCD init sequence depends on command or video mode
 */
void lcd_init(u32 id, struct decon_lcd *lcd)
{
	dsim_dbg("+++%s+++\n",__func__);

    if(dsim_wr_data(id, MIPI_DSI_DCS_SHORT_WRITE, (unsigned long)0x11, 0)<0)
       dsim_err("fail to write 0x11 command.\n");
	msleep(20);				
	if(dsim_wr_data(id, MIPI_DSI_DCS_SHORT_WRITE_PARAM, (unsigned long)0x53, 0x28)<0)
       dsim_err("fail to write 0x53 command.\n");
	msleep(5);
	if(dsim_wr_data(id, MIPI_DSI_DCS_SHORT_WRITE_PARAM, (unsigned long)0x51, 0xff)<0)
       dsim_err("fail to write 0x51 command.\n");
	msleep(10);					
	if(dsim_wr_data(id, MIPI_DSI_DCS_SHORT_WRITE, (unsigned long)0x29, 0)<0)
       dsim_err("fail to write 0x29 command.\n");
    msleep(20);

	dsim_dbg("---%s---\n",__func__);
}

void lcd_enable_exynos(u32 id)
{
     return;
}

void lcd_disable_exynos(u32 id)
{
	return;
}

/*
 * Set gamma values
 *
 * Parameter
 *	- backlightlevel : It is from 0 to 26.
 */
int lcd_gamma_ctrl(u32 backlightlevel)
{
	return 0;
}

int lcd_gamma_update(void)
{
	return 0;
}
