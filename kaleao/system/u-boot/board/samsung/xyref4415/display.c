/*
 * Copyright (C) 2013 Samsung Electronics
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <lcd.h>
#include <asm/arch/power.h>
#include <asm/arch/gpio.h>
#include <asm/arch/mipi_dsim.h>
#if defined(CONFIG_S6E8AA0)
#include "logo_588x95.h"
#elif defined(CONFIG_HX8369)
#include "logo_305x50.h"
#elif defined(CONFIG_NT35512)
#include "logo_305x50.h"
#endif

DECLARE_GLOBAL_DATA_PTR;

/* For panel info */
#if defined(CONFIG_S6E8AA0)
#define XYREF4415_LCD_TYPE "s6e8aa0"
#define XYREF4415_HBP    11
#define XYREF4415_HFP    11
#define XYREF4415_VBP    3
#define XYREF4415_VFP    3
#define XYREF4415_HSP    2
#define XYREF4415_VSW    2
#define XYREF4415_XRES   800
#define XYREF4415_YRES   1280
#define XYREF4415_VIRTUAL_X  800
#define XYREF4415_VIRTUAL_Y  1280 * 2
#define XYREF4415_WIDTH    71
#define XYREF4415_HEIGHT   114
#define XYREF4415_MAX_BPP  32
#define XYREF4415_DEFAULT_BPP  24
#define XYREF4415_DEFAULT_FREQ 60

#elif defined(CONFIG_HX8369)
#define XYREF4415_LCD_TYPE "hx8369"
#define XYREF4415_HBP    150
#define XYREF4415_HFP    180
#define XYREF4415_VBP    20
#define XYREF4415_VFP    20
#define XYREF4415_HSP    32
#define XYREF4415_VSW    2
#define XYREF4415_XRES   480
#define XYREF4415_YRES   800
#define XYREF4415_VIRTUAL_X  480
#define XYREF4415_VIRTUAL_Y  800 * 2
#define XYREF4415_WIDTH    56
#define XYREF4415_HEIGHT   94
#define XYREF4415_MAX_BPP  32
#define XYREF4415_DEFAULT_BPP  24
#define XYREF4415_DEFAULT_FREQ 60

#elif defined(CONFIG_NT35512)
#define XYREF4415_LCD_TYPE "nt35512"
#define XYREF4415_HBP    8
#define XYREF4415_HFP    8
#define XYREF4415_VBP    8
#define XYREF4415_VFP    27
#define XYREF4415_HSP    2
#define XYREF4415_VSW    2
#define XYREF4415_XRES   480
#define XYREF4415_YRES   800
#define XYREF4415_VIRTUAL_X  480
#define XYREF4415_VIRTUAL_Y  800 * 2
#define XYREF4415_WIDTH    56
#define XYREF4415_HEIGHT   94
#define XYREF4415_MAX_BPP  32
#define XYREF4415_DEFAULT_BPP  24
#define XYREF4415_DEFAULT_FREQ 60

#endif

/* For dsim info */
#if defined(CONFIG_S6E8AA0)
#define	DSIM_DATA_LANE	DSIM_DATA_LANE_4
#define DSIM_PLL_OUT_DIV DSIM_PLL_OUT_DIV8
#define DSIM_BURST_MODE	DSIM_BURST
#define DSIM_P	2
#define DSIM_M	50
#define DSIM_S	0
#define DSIM_ESC_CLK	20 * 1000000 /* escape clk : 20MHz */

#elif defined(CONFIG_HX8369)
#define	DSIM_DATA_LANE	DSIM_DATA_LANE_2
#define DSIM_PLL_OUT_DIV DSIM_PLL_OUT_DIV8
#define DSIM_BURST_MODE	DSIM_BURST
#define DSIM_P	2
#define DSIM_M	80
#define DSIM_S	1
#define DSIM_ESC_CLK	20 * 1000000 /* escape clk : 20MHz */

#elif defined(CONFIG_NT35512)
#define	DSIM_DATA_LANE	DSIM_DATA_LANE_2
#define DSIM_PLL_OUT_DIV DSIM_PLL_OUT_DIV8
#define DSIM_BURST_MODE	DSIM_BURST
#define DSIM_P	2
#define DSIM_M  80
#define DSIM_S	1
#define DSIM_ESC_CLK	20 * 1000000 /* escape clk : 20MHz */
#endif

#if defined(CONFIG_S6E8AA0)
static void lcd_reset(void)
{
	struct exynos4_gpio_part2 *gpio2 =
		(struct exynos4_gpio_part2 *)samsung_get_base_gpio_part2();

	/* 1.8v Level Sequence: H->L->H */
	s5p_gpio_direction_output(&gpio2->x2, 4, 1);
	udelay(10000);
	s5p_gpio_direction_output(&gpio2->x2, 4, 0);
	udelay(10000);
	s5p_gpio_direction_output(&gpio2->x2, 4, 1);
}

static int lcd_power(void)
{
	struct exynos4_gpio_part1 *gpio1 =
		(struct exynos4_gpio_part1 *)samsung_get_base_gpio_part1();
	struct exynos4_gpio_part2 *gpio2 =
		(struct exynos4_gpio_part2 *)samsung_get_base_gpio_part2();

	/* power1 GPX2-5 */
	s5p_gpio_direction_output(&gpio2->x2, 5, 0);
	s5p_gpio_direction_output(&gpio2->x2, 5, 1);
	/* power2 GPD0-0 */
	s5p_gpio_direction_output(&gpio1->d0, 0, 0);
	s5p_gpio_direction_output(&gpio1->d0, 0, 1);

	return 0;
}

static int mipi_power(void)
{
 /*
	*	MIPI power already enabled on smdk4270 board as default
	*	LDO7: VDD10 1.0v , LDO8: VDD18 1.8v
	*/
	return 0;
}

static void get_logo_info(vidinfo_t *vid)
{
	vid->logo_width	=	588;
	vid->logo_height	= 95;
	vid->logo_addr	= (ulong)gzip_bmp_logo;
}
#elif defined(CONFIG_HX8369)
static void lcd_reset(void)
{
	struct exynos4_gpio_part2 *gpio2 =
		(struct exynos4_gpio_part2 *)samsung_get_base_gpio_part2();

	/* 1.8v Level Sequence: H->L->H */
	s5p_gpio_direction_output(&gpio2->x2, 4, 1);
	udelay(20000);
	s5p_gpio_direction_output(&gpio2->x2, 4, 0);
	udelay(20000);
	s5p_gpio_direction_output(&gpio2->x2, 4, 1);
}

static int lcd_power(void)
{
	struct exynos4_gpio_part1 *gpio1 =
		(struct exynos4_gpio_part1 *)samsung_get_base_gpio_part1();
	struct exynos4_gpio_part2 *gpio2 =
		(struct exynos4_gpio_part2 *)samsung_get_base_gpio_part2();

	/* power1 GPM1-6 */
	s5p_gpio_direction_output(&gpio2->m1, 6, 0);
	s5p_gpio_direction_output(&gpio2->m1, 6, 1);
	/* power2 GPM4-5 */
	s5p_gpio_direction_output(&gpio2->m4, 5, 0);
	s5p_gpio_direction_output(&gpio2->m4, 5, 1);
	udelay(20000);

	/* backlight GPD0-1 */
	s5p_gpio_direction_output(&gpio1->d0, 1, 0);
	s5p_gpio_direction_output(&gpio1->d0, 1, 1);

	lcd_reset();

	return 0;
}

static int mipi_power(void)
{
 /*
	*	MIPI power already enabled on smdk4270 board as default
	*	LDO7: VDD10 1.0v , LDO8: VDD18 1.8v
	*/
	return 0;
}

static void get_logo_info(vidinfo_t *vid)
{
	vid->logo_width	=	305;
	vid->logo_height	= 50;
	vid->logo_addr	= (ulong)gzip_bmp_logo;
}

#elif defined(CONFIG_NT35512)
static void lcd_reset(void)
{
	struct exynos4_gpio_part2 *gpio2 =
		(struct exynos4_gpio_part2 *)samsung_get_base_gpio_part2();

	/* 1.8v Level Sequence: H->L->H */
	s5p_gpio_direction_output(&gpio2->x2, 4, 1);
	udelay(20000);
	s5p_gpio_direction_output(&gpio2->x2, 4, 0);
	udelay(20000);
	s5p_gpio_direction_output(&gpio2->x2, 4, 1);
}

static int lcd_power(void)
{
	struct exynos4_gpio_part1 *gpio1 =
		(struct exynos4_gpio_part1 *)samsung_get_base_gpio_part1();

	/* backlight GPD0-1 */
	s5p_gpio_direction_output(&gpio1->d0, 1, 0);
	mdelay(1);
	s5p_gpio_direction_output(&gpio1->d0, 1, 1);

	mdelay(150);
	lcd_reset();
	mdelay(150);

	return 0;
}

static int mipi_power(void)
{
 /*
	*	MIPI power already enabled on smdk4270 board as default
	*	LDO7: VDD10 1.0v , LDO8: VDD18 1.8v
	*/
	return 0;
}

static void get_logo_info(vidinfo_t *vid)
{
	vid->logo_width		= 305;
	vid->logo_height	= 50;
	vid->logo_addr		= (ulong)gzip_bmp_logo;
}
#endif

static struct mipi_dsim_config dsim_config = {
	.e_interface		= DSIM_VIDEO,
	.e_pixel_format		= DSIM_24BPP_888,

	.auto_flush		= 0,
	.eot_disable		= 1,
	.auto_vertical_cnt	= 0,

	.hse			= 0,
	.hfp			= 0,
	.hbp			= 0,
	.hsa			= 0,

	.e_no_data_lane		= DSIM_DATA_LANE,
	.e_byte_clk		= DSIM_PLL_OUT_DIV,
	.e_burst_mode		= DSIM_BURST_MODE,

	.e_virtual_ch		= DSIM_VIRTUAL_CH_0,

	.p			= DSIM_P,
	.m			= DSIM_M,
	.s			= DSIM_S,

	.esc_clk		= DSIM_ESC_CLK, /* escape clk : 20MHz */

	/* stop state holding counter after bta change count 0 ~ 0xfff */
	.stop_holding_cnt	= 0x0fff,
	/* bta timeout 0 ~ 0xff */
	.bta_timeout		= 0xff,
	/* lp rx timeout 0 ~ 0xffff */
	.rx_timeout		= 0xffff,
	/* D-PHY PLL stable time spec :min = 200usec ~ max 400usec */
	.pll_stable_time	= 0xffffff,
};

static struct exynos_platform_mipi_dsim dsim_platform_data = {
	.lcd_panel_info	= NULL,
	.dsim_config	= &dsim_config,
};

static struct mipi_dsim_lcd_device mipi_lcd_device = {
	.name	= XYREF4415_LCD_TYPE,
	.id	= -1,
	.bus_id	= 0,
	.platform_data	= (void *)&dsim_platform_data,
};

vidinfo_t panel_info = {
	.vl_col			= XYREF4415_XRES,  /* Number of columns (i.e. 640) */
	.vl_row			= XYREF4415_YRES, /* Number of rows (i.e. 480) */
	.vl_width		= XYREF4415_XRES,	/* Width of display area in millimeters */
	.vl_height		= XYREF4415_YRES,	/* Height of display area in millimeters */

	/* LCD configuration register */
	.vl_freq		= XYREF4415_DEFAULT_FREQ,	/* Frequency */
	.vl_clkp		= CONFIG_SYS_HIGH,/* Clock polarity */
	.vl_hsp			= CONFIG_SYS_LOW, /* Horizontal Sync polarity */
	.vl_vsp			= CONFIG_SYS_LOW, /* Vertical Sync polarity */
	.vl_dp			= CONFIG_SYS_LOW, /* Data polarity */
	.vl_bpix		= 5,              /* Bits per pixel, 2^5 = 32 */

	/* Horizontal control register. Timing from data sheet */
	.vl_hspw		= XYREF4415_HSP,  /* Horizontal sync pulse width */
	.vl_hfpd		= XYREF4415_HFP, /* Wait before of line */
	.vl_hbpd		= XYREF4415_HBP, /* Wait end of line */

	/* Vertical control register. */
	.vl_vspw		= XYREF4415_VSW,  /* Vertical sync pulse width */
	.vl_vfpd		= XYREF4415_VFP,  /* Wait before of frame */
	.vl_vbpd		= XYREF4415_VBP,  /* Wait end of frame */
	.vl_stable_vfp		= 1,
	.vl_cmd_allow_len	= 0,  /* Wait end of frame */

	.cfg_gpio		= NULL,
	.backlight_on		= NULL,
	.lcd_power_on		= NULL, /* lcd_power_on in mipi dsi driver */
	.reset_lcd		= NULL,
	.dual_lcd_enabled	= 0,

	.win_id			= 0,
	.init_delay		= 0,
	.power_on_delay		= 0,
	.reset_delay		= 0,
	.interface_mode		= FIMD_RGB_INTERFACE,
	.mipi_enabled		= 1,
};

#define HD_RESOLUTION 1
void init_panel_info(vidinfo_t *vid)
{
	vid->logo_on	= 1;
	vid->resolution	= HD_RESOLUTION;
	vid->rgb_mode	= MODE_BGR_P;
#ifdef CONFIG_TIZEN
	get_tizen_logo_info(vid);
#else
	get_logo_info(vid);
#endif

	strcpy(dsim_platform_data.lcd_panel_name, mipi_lcd_device.name);
	dsim_platform_data.lcd_power = lcd_power;
	dsim_platform_data.mipi_power = mipi_power;
	dsim_platform_data.phy_enable = set_mipi_phy_ctrl;
	dsim_platform_data.lcd_panel_info = (void *)vid;
	exynos_mipi_dsi_register_lcd_device(&mipi_lcd_device);
	dsim_lcd_init();
	exynos_set_dsim_platform_data(&dsim_platform_data);
}
