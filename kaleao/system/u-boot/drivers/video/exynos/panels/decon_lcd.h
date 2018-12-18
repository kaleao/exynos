/* drivers/video/exynos_decon/lcd.h
 *
 * Copyright (c) 2011 Samsung Electronics
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef __DECON_LCD__
#define __DECON_LCD__

enum decon_psr_mode {
	DECON_VIDEO_MODE = 0,
	DECON_DP_PSR_MODE = 1,
	DECON_MIPI_COMMAND_MODE = 2,
};

enum mic_ver {
	MIC_VER_1_1,
	MIC_VER_1_2,		/* as a default */
	MIC_VER_2_0,
};

enum type_of_ddi {
	TYPE_OF_SM_DDI = 0,	/* Samsung Mobile: as a default */
	TYPE_OF_MAGNA_DDI,	/* MAGNA */
	TYPE_OF_NORMAL_DDI,	/* No Samsung and MAGNA */
};

struct decon_lcd {
	enum decon_psr_mode mode;
	unsigned int vfp;
	unsigned int vbp;
	unsigned int hfp;
	unsigned int hbp;

	unsigned int vsa;
	unsigned int hsa;

	unsigned int xres;
	unsigned int yres;

	unsigned int width;
	unsigned int height;

	unsigned int hs_clk;
	unsigned int esc_clk;

	unsigned int fps;
	unsigned int mic_enabled;
	enum mic_ver mic_ver;
	enum type_of_ddi ddi_type;
};

#define LOGO_KERNEL  0xE0000000
#define LOGO_ADDR 0xE1000000
#define UBOOT_ON_LCD_START_ADDR  0xE2000000

#ifdef CONFIG_DECON_LCD_S6E8AA0
#define LCD_WIDTH		(720)
#define LCD_HEIGHT		(1280)
#elif defined (CONFIG_DECON_LCD_S6TNMR7) 
#define LCD_WIDTH		(2560)
#define LCD_HEIGHT		(1600)
#else
#define LCD_WIDTH		(1080)
#define LCD_HEIGHT		(1920)
#endif

#endif
