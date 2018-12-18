/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * Header file for Exynos DECON driver

 * Sewoon Park <seuni.park@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef ___SAMSUNG_DECON_CONFIG_H__
#define ___SAMSUNG_DECON_CONFIG_H__

/*
 ***** Machine Dependency *****
*/
#ifdef CONFIG_CPU_EXYNOS7420 /* EXYNOS7420 */
/* SFR Base Address */
#define DECON_INT_BASE_ADDR	0x13930000
#define DECON_EXT_BASE_ADDR	0x13950000
#define DSIM_1ST_BASE_ADDR	0x13900000
#define DSIM_2ND_BASE_ADDR	0x13910000
#define DECON_SMMU_BASE_ADDR 0x13A10000

#define DECON_IP_VER		IP_VER_DECON_7I
#define DECON_MAX_WIN		7

#endif /* CONFIG_CPU_EXYNOS7420 */

#ifdef CONFIG_CPU_EXYNOS7580 /* EXYNOS7580 */
/* SFR Base Address */
#define DECON_INT_BASE_ADDR	0x14830000
#define DSIM_1ST_BASE_ADDR	0x14800000
#define DECON_SMMU_BASE_ADDR	0x14860000

/* Dummy Addresses to fix Build errors */
#define DECON_EXT_BASE_ADDR	0x00000000
#define DSIM_2ND_BASE_ADDR	0x00000000

#define DECON_IP_VER		IP_VER_DECON_7I
#define DECON_MAX_WIN		3
#endif /* CONFIG_CPU_EXYNOS7580 */

/*
 ***** Board Dependency *****
*/
#ifdef CONFIG_MACH_AVL7420 /* ESPRESSO7420 */
/* Display Properties */

#ifdef CONFIG_DECON_LCD_S6E3FA0
#define BD_PSR_MODE	DECON_MIPI_COMMAND_MODE
#define BD_TRIG_MODE	DECON_HW_TRIG
#define BD_DSI_MODE	DSI_MODE_SINGLE
#else
#define BD_PSR_MODE	DECON_VIDEO_MODE
#define BD_TRIG_MODE	DECON_HW_TRIG
#define BD_DSI_MODE	DSI_MODE_SINGLE
#endif

#ifdef CONFIG_DECON_LCD_S6TNMR7
#define BD_PSR_MODE	DECON_MIPI_COMMAND_MODE
#define BD_TRIG_MODE	DECON_HW_TRIG
#define BD_DSI_MODE	DSI_MODE_DUAL
#endif

#else
/* Display Properties */
#define BD_PSR_MODE	DECON_MIPI_COMMAND_MODE
#define BD_TRIG_MODE	DECON_HW_TRIG
#define BD_DSI_MODE	DSI_MODE_SINGLE

#endif /* CONFIG_MACH_AVL7420 */

/*
 ***** COMMON *****
*/

/* Display Properties */
static struct exynos_decon_platdata exynos_decon_pdata = {
	.ip_ver 	= DECON_IP_VER,
	.psr_mode 	= BD_PSR_MODE,
	.trig_mode 	= BD_TRIG_MODE,
	.dsi_mode 	= BD_DSI_MODE,
	.max_win 	= DECON_MAX_WIN,
	.default_win 	= 0,
};

/* GET platdata API list */
static inline struct exynos_decon_platdata *get_decon_pdata(void)
{
	return &exynos_decon_pdata;
}

#endif /* SAMSUNG_DECON_CONFIG_H */
