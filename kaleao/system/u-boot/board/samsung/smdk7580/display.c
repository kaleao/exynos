/*
 * SAMSUNG Exynos display configuration file
 *
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 * Copyright (C) 2014 Samsung Electronics
 */

#include <common.h>
#include <lcd.h>
#include <asm/arch/gpio.h>
#include <asm/arch/power.h>
#include <asm/arch/sysreg.h>

#define MIPI_DPHY_M4Sx_RESETN(_id)	(1 << ((_id) * 4))
/*
 * ########## Machine dependency ##########
*/
static void exynos7580_mipi_phy_control(unsigned int dev_index,
					unsigned int enable)
{
	struct exynos7580_power *pmu =
	    (struct exynos7580_power *)samsung_get_base_power();
	struct exynos7580_sysreg_disp *sysreg_disp =
		(struct exynos7580_sysreg_disp *)exynos7580_get_base_sysreg_disp();

	unsigned int addr, cfg = 0;

	/* Enable DPHY reset(0) -> Release DPHY reset(1) */
	addr = (unsigned int)&sysreg_disp->mipi_dphy_con;
	cfg = readl(addr);

	cfg &= ~(MIPI_DPHY_M4Sx_RESETN(dev_index));
	writel(cfg, addr);
	cfg |= MIPI_DPHY_M4Sx_RESETN(dev_index);
	writel(cfg, addr);

	addr = (unsigned int)&pmu->mipi_phy_m4s4_control;

	cfg = readl(addr);
	if (enable)
		cfg |= EXYNOS_MIPI_PHY_ENABLE;
	else
		cfg &= ~(EXYNOS_MIPI_PHY_ENABLE);

	writel(cfg, addr);
}


/* MIPI-PHY related with master interface */
static void set_mipi_phy_control(unsigned int enable)
{
	exynos7580_mipi_phy_control(0, enable);
}

/* Set GPIO internal interrupt when DECON are using HW trigger */
static void set_gpio_hw_te(void)
{
	struct exynos7580_gpio_top *gpio_top =
			    exynos7580_get_base_gpio_top();
	struct s5p_gpio_bank *bank;

	/* GPIO: TE_DECON_INT(gpe1 4 0x2) */
	/* GPE1CON[4]  [19:16] 0x2 = DISP_TES */
	bank = &gpio_top->e1;
	s5p_gpio_set_pull(bank, 4, GPIO_PULL_DOWN);
	s5p_gpio_cfg_pin(bank, 4, GPIO_FUNC(0x2));
	s5p_gpio_set_drv(bank, 4, GPIO_DRV_4X);
}

/*
 * ########## Board dependency ##########
*/
/* Configure and set a GPIO for LCD_RESET */
static void set_gpio_lcd_reset(enum board_gpio_type gpio_type)
{
	struct exynos7580_gpio_top *gpio_top =
			    exynos7580_get_base_gpio_top();
	struct s5p_gpio_bank *bank;
	int gpio;

	switch (gpio_type) {
	case BOARD_ESPRESSO:
		/* GPIO: LCD RESET(gpd1 3 0x1) */
		/* GPD1CON[3]  [15:12] 0x1 = Output */
		bank = &gpio_top->d1;
		gpio = 3;
		break;
	default:
		debug("%s: Does not support it!!\n", __func__);
		break;
	}
	s5p_gpio_set_pull(bank, gpio, GPIO_PULL_DOWN);
	s5p_gpio_cfg_pin(bank, gpio, GPIO_OUTPUT);
	s5p_gpio_set_drv(bank, gpio, GPIO_DRV_4X);

	/* RESET: "0" -> "1" */
	s5p_gpio_set_value(bank, gpio, 0);
	udelay(5000);
	s5p_gpio_set_value(bank, gpio, 1);
	udelay(10000);
}

/* Configure and set a GPIO for LCD_POWER_ON */
static void set_gpio_lcd_power(enum board_gpio_type gpio_type)
{
	struct exynos7580_gpio_top *gpio_top =
			    exynos7580_get_base_gpio_top();
	struct s5p_gpio_bank *bank;
	int gpio;

	switch (gpio_type) {
	case BOARD_ESPRESSO:
		/* PIN are not connected on board */
		break;
	default:
		debug("%s: Does not support it!!\n", __func__);
		break;
	}
}

static struct exynos_display_config display_config = {
	.set_mipi_phy = set_mipi_phy_control,
	.set_gpio_hw_te = set_gpio_hw_te,
	.set_gpio_lcd_reset = set_gpio_lcd_reset,
	.set_gpio_lcd_power = set_gpio_lcd_power,
};

/* Get type of board */
enum board_gpio_type get_exynos_board_type(void)
{
#if defined(CONFIG_MACH_SMDK7580)
	return BOARD_ESPRESSO;
#else
	return BOARD_ESPRESSO;
#endif
}

/* Get display configuration callbacks */
struct exynos_display_config *get_exynos_display_config(void)
{
	return &display_config;
}
