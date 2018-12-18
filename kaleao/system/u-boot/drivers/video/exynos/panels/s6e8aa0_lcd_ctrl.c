/*
 * Samsung SoC MIPI LCD CONTROL functions
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/
#include <asm/arch/mipi_dsim.h>

#include "s6e8aa0_gamma.h"
#include "lcd_ctrl.h"
#include "../dsim.h"


/* Porch values. It depends on command or video mode */
#define S6E8AA0_CMD_VBP		1
#define S6E8AA0_CMD_VFP		2
#define S6E8AA0_CMD_VSA		1
#define S6E8AA0_CMD_HBP		51
#define S6E8AA0_CMD_HFP		51
#define S6E8AA0_CMD_HSA		1

/* These need to define */
//From Kernel
#define S6E8AA0_VIDEO_VBP	1
#define S6E8AA0_VIDEO_VFP	2
#define S6E8AA0_VIDEO_VSA	1
#define S6E8AA0_VIDEO_HBP	51
#define S6E8AA0_VIDEO_HFP	51
#define S6E8AA0_VIDEO_HSA	1

/*
#define S6E8AA0_VIDEO_VBP	1
#define S6E8AA0_VIDEO_VFP	8
#define S6E8AA0_VIDEO_VSA	1
#define S6E8AA0_VIDEO_HBP	102
#define S6E8AA0_VIDEO_HFP	51
#define S6E8AA0_VIDEO_HSA	1
*/

//From CM1
/*#define S6E8AA0_VIDEO_VBP	1
#define S6E8AA0_VIDEO_VFP	13
#define S6E8AA0_VIDEO_VSA	2
#define S6E8AA0_VIDEO_HBP	50
#define S6E8AA0_VIDEO_HFP	50
#define S6E8AA0_VIDEO_HSA	5*/


#define S6E8AA0_HORIZONTAL	720
#define S6E8AA0_VERTICAL	1280
#if 0
s6e8aa0: s6e8aa0_hd {
         mode = <0>; /* 2: MIPI command mode */
         resolution = <720 1280>;
         size = <71 114>;
         timing,refresh = <60>;
         timing,h-porch = <51 51 1>;
         timing,v-porch = <1 2 1>;
         timing,dsi-hs-clk = <480>;
         timing,dsi-escape-clk = <20>;
         mic = <0>;
         mic_ver = <1>;         /* 0: mic v1.1, 1: v1.2, 2: v2.0 */
         type_of_ddi = <0>; /* 0:Samsung Mobile */
};
#endif
#define msleep(a) udelay(a * 1000)

#define CONFIG_FB_VIDEO_MODE

struct decon_lcd s6e8aa0_lcd_info = {
#ifdef CONFIG_FB_VIDEO_MODE
	.mode = DECON_VIDEO_MODE,
	.vfp = S6E8AA0_VIDEO_VFP,
	.vbp = S6E8AA0_VIDEO_VBP,
	.hfp = S6E8AA0_VIDEO_HFP,
	.hbp = S6E8AA0_VIDEO_HBP,
	.vsa = S6E8AA0_VIDEO_VSA,
	.hsa = S6E8AA0_VIDEO_HSA,
#else
	.mode = DECON_MIPI_COMMAND_MODE,
	.vfp = S6E8AA0_CMD_VFP,
	.vbp = S6E8AA0_CMD_VBP,
	.hfp = S6E8AA0_CMD_HFP,
	.hbp = S6E8AA0_CMD_HBP,
	.vsa = S6E8AA0_CMD_VSA,
	.hsa = S6E8AA0_CMD_HSA,
#endif
	.xres = S6E8AA0_HORIZONTAL,
	.yres = S6E8AA0_VERTICAL,

	/* Maybe, width and height will be removed */
	.width = 71,
	.height = 114,

	/* Mhz */
	.hs_clk = 480,
	.esc_clk = 20,

	.fps = 60,
	.mic_enabled = 0,//data compress !! Attenton
};

struct decon_lcd *decon_get_lcd_info(void)
{
	return &s6e8aa0_lcd_info;
}


#define GAMMA_PARAM_SIZE 26
#define MAX_BRIGHTNESS 255
#define MIN_BRIGHTNESS 0
#define DEFAULT_BRIGHTNESS 100
static u32 dsim_base = 0;

   
static const unsigned char apply_level_2_key[3] = {
    /* command */
    0xfc,
    /* parameter */
    0x5a, 0x5a
};

static const unsigned char sleep_out[1] = {
    0x11,
};

static const unsigned char panel_condition_set[39] = {
    0xF8,
    0x25, 0x34, 0x00, 0x00, 0x00, 0x95, 0x00, 0x3c, 0x7d, 0x08,
    0x27, 0x00, 0x00, 0x10, 0x00, 0x00, 0x20, 0x02, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x02, 0x08, 0x08, 0x23, 0x63, 0xc0, 0xc1,
    0x01, 0x81, 0xc1, 0x00, 0xc8, 0xc1, 0xd3, 0x01
};

static const unsigned char display_condition_set[4] = {
    0xF2,
    0x80, 0x03, 0x0D
};

static const unsigned char gamma_update[4] = {
    0xF7,
    0x03, 0x00, 0x00
};

static const unsigned char etc_set_source_ctrl[4] = {
    0xF6,
    0x00, 0x02, 0x00
};

static const unsigned char etc_set_pentile_ctrl[10] = {
    0xB6,
    0x0C, 0x02, 0x03, 0x32, 0xC0, 0x44, 0x44, 0xC0, 0x00
};

static const unsigned char etc_set_power_ctrl[8] = {
    0xF4,
    0xCF, 0x0A, 0x15, 0x10, 0x19, 0x33, 0x02
};

static const unsigned char elvss_NVM_set[15] = {
    0xD9,
    0x14, 0x40, 0x0C, 0xCB, 0xCE, 0x6E, 0xC4, 0x07, 0x40, 0x41,
    0xC1, 0x00, 0x60, 0x19
};

static const unsigned char elvss_ctrl_set[3] = {
    0xB1, 0x04, 0x00
};

static int get_backlight_level(int brightness)
{
    int backlightlevel;

    switch (brightness) {
    case 0:
        backlightlevel = 0;
        break;
    case 1 ... 29:
        backlightlevel = 0;
        break;
    case 30 ... 34:
        backlightlevel = 1;
        break;
    case 35 ... 39:
        backlightlevel = 2;
        break;
    case 40 ... 44:
        backlightlevel = 3;
        break;
    case 45 ... 49:
        backlightlevel = 4;
        break;
    case 50 ... 54:
        backlightlevel = 5;
        break;
    case 55 ... 64:
        backlightlevel = 6;
        break;
    case 65 ... 74:
        backlightlevel = 7;
        break;
    case 75 ... 83:
        backlightlevel = 8;
        break;
    case 84 ... 93:
        backlightlevel = 9;
        break;
    case 94 ... 103:
        backlightlevel = 10;
        break;
    case 104 ... 113:
        backlightlevel = 11;
        break;
    case 114 ... 122:
        backlightlevel = 12;
        break;
    case 123 ... 132:
        backlightlevel = 13;
        break;
    case 133 ... 142:
        backlightlevel = 14;
        break;
    case 143 ... 152:
        backlightlevel = 15;
        break;
    case 153 ... 162:
        backlightlevel = 16;
        break;
    case 163 ... 171:
        backlightlevel = 17;
        break;
    case 172 ... 181:
        backlightlevel = 18;
        break;
    case 182 ... 191:
        backlightlevel = 19;
        break;
    case 192 ... 201:
        backlightlevel = 20;
        break;
    case 202 ... 210:
        backlightlevel = 21;
        break;
    case 211 ... 220:
        backlightlevel = 22;
        break;
    case 221 ... 230:
        backlightlevel = 23;
        break;
    case 231 ... 240:
        backlightlevel = 24;
        break;
    case 241 ... 250:
        backlightlevel = 25;
        break;
    case 251 ... 255:
        backlightlevel = 26;
        break;
    default:
        backlightlevel = 12;
        break;
    }

    return backlightlevel;
}

static int update_brightness(int id,int brightness)
{
    int backlightlevel;
    

    backlightlevel = get_backlight_level(brightness);

    if (dsim_wr_data(id, MIPI_DSI_DCS_LONG_WRITE,
            (unsigned long)gamma22_table[backlightlevel],
                GAMMA_PARAM_SIZE) < 0)
        dsim_err("fail to write gamma value.\n");
    mdelay(5);
    if (dsim_wr_data(id, MIPI_DSI_DCS_LONG_WRITE,
            (unsigned long)gamma_update,
                ARRAY_SIZE(gamma_update)) < 0)
        dsim_err("fail to update gamma value.\n");
    return 1;
}

static int s6e8aa0_set_brightness(int id,int brightness)
{

    update_brightness(id,brightness);

    return 1;
}

/*
 * s6e8aa0 lcd init sequence
 *
 * Parameters
 *	- mic_enabled : if mic is enabled, MIC_ENABLE command must be sent
 *	- mode : LCD init sequence depends on command or video mode
 */

void lcd_init(u32 id, struct decon_lcd *lcd)
{

    dsim_dbg("+++%s+++\n",__func__);
    while (dsim_wr_data(id, MIPI_DSI_DCS_LONG_WRITE,
        (unsigned long)apply_level_2_key,
            ARRAY_SIZE(apply_level_2_key)) == -1)
         dsim_err("fail to send apply_level_2_key command.\n");

    udelay(16000);
    dsim_wr_data(id, MIPI_DSI_DCS_SHORT_WRITE,
        (unsigned long)0x11, 0);

    udelay(16000);

    if (dsim_wr_data(id, MIPI_DSI_DCS_LONG_WRITE,
            (unsigned long)panel_condition_set,
                ARRAY_SIZE(panel_condition_set)) < 0)
         dsim_err("fail to send panel_condition_set command.\n");
    mdelay(5);// origin is 1

    while (dsim_wr_data(id, MIPI_DSI_DCS_LONG_WRITE,
            (unsigned long)display_condition_set,
                ARRAY_SIZE(display_condition_set)) < 0)
         dsim_err("fail to send display_condition_set command.\n");

    mdelay(5);
    
    //Set default Brightness in Uboot
    update_brightness(id,DEFAULT_BRIGHTNESS);
    mdelay(5);
    while (dsim_wr_data(id, MIPI_DSI_DCS_LONG_WRITE,
            (unsigned long)etc_set_source_ctrl,
                ARRAY_SIZE(etc_set_source_ctrl)) < 0)
         dsim_err("fail to send set_source_ctrl command.\n");
    mdelay(5);

    while (dsim_wr_data(id, MIPI_DSI_DCS_LONG_WRITE,
            (unsigned long)etc_set_pentile_ctrl,
                ARRAY_SIZE(etc_set_pentile_ctrl)) < 0)
         dsim_err("fail to send set_pentile_ctrl command.\n");
    mdelay(5);

    while (dsim_wr_data(id, MIPI_DSI_DCS_LONG_WRITE,
            (unsigned long)etc_set_power_ctrl,
                ARRAY_SIZE(etc_set_power_ctrl)) < 0)
         dsim_err("fail to send set_power_ctrl command.\n");
    mdelay(5);

    while (dsim_wr_data(id, MIPI_DSI_DCS_LONG_WRITE,
            (unsigned long)elvss_NVM_set,
                ARRAY_SIZE(elvss_NVM_set)) < 0)
         dsim_err("fail to send elvss_NVM_set command.\n");
    mdelay(5);

    while (dsim_wr_data(id, MIPI_DSI_DCS_LONG_WRITE,
            (unsigned long)elvss_ctrl_set,
                ARRAY_SIZE(elvss_ctrl_set)) < 0)
         dsim_err("fail to send elvss_ctrl_set command.\n");

    msleep(20);

    while (dsim_wr_data(id, MIPI_DSI_DCS_SHORT_WRITE,
        (unsigned long)0x29, 0) < 0)
        dsim_err("fail to send display on command\n");
    mdelay(20);
    dsim_dbg("---%s---\n", __func__);
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
static int lcd_read_id(u32 id)
{
    return 0;
}

