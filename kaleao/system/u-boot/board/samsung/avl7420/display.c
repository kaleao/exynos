/*
 * SAMSUNG Exynos display configuration file
 *
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * Sewoon Park <seuni.park@samsung.com>
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
static void exynos7_mipi_phy_control(unsigned int dev_index,
					unsigned int enable)
{
	struct exynos7420_power *pmu =
	    (struct exynos7420_power *)samsung_get_base_power();
	struct exynos7420_sysreg *sysreg =
		(struct exynos7420_sysreg *)samsung_get_base_sysreg();

	unsigned int addr, cfg = 0;

	/* Enable DPHY reset(0) -> Release DPHY reset(1) */
	addr = (unsigned int)&sysreg->mipi_dphy_con;
	cfg = readl(addr);

	cfg &= ~(MIPI_DPHY_M4Sx_RESETN(dev_index));
	writel(cfg, addr);
	cfg |= MIPI_DPHY_M4Sx_RESETN(dev_index);
	writel(cfg, addr);

	/* DPHY isolation with PMU */
	if (dev_index == 0)
		addr = (unsigned int)&pmu->mipi_phy_m4s4_control;
	else
		addr = (unsigned int)&pmu->mipi_phy_m4s0_control;

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
	exynos7_mipi_phy_control(0, enable);
	exynos7_mipi_phy_control(1, enable);
}

#define DISP_CFG_UNMASK_GLOBAL                 (1 << 4)
/* Set GPIO internal interrupt when DECON are using HW trigger */
static void set_gpio_hw_te(void)
{
	struct exynos7420_gpio_bus0 *gpio_bus0 =
			    exynos7420_get_base_gpio_bus0();
	struct exynos7420_sysreg *sysreg =
		(struct exynos7420_sysreg *)samsung_get_base_sysreg();

	struct s5p_gpio_bank *bank;
	unsigned int addr, cfg = 0;

	/* GPIO: TE_DECON_INT(gpg0 1 0x2) */
	/* GPG0CON[1]  [7:4] 0x2 = TE_DECON_INT */
	bank = &gpio_bus0->g0;
	s5p_gpio_set_pull(bank, 1, GPIO_PULL_DOWN);
	s5p_gpio_cfg_pin(bank, 1, GPIO_FUNC(0x2));
	s5p_gpio_set_drv(bank, 1, GPIO_DRV_4X);

	/* Set a DISP_CFG_USER_REG in SYSREG */
	addr = (unsigned int)&sysreg->disp_cfg_user_reg;
	cfg = readl(addr);
	cfg |= DISP_CFG_UNMASK_GLOBAL;
	writel(cfg, addr);
}

/*
 * ########## Board dependency ##########
*/
/* Configure and set a GPIO for LCD_RESET */
static void set_gpio_lcd_reset(enum board_gpio_type gpio_type)
{
	struct exynos7420_gpio_bus0 *gpio_bus0 =
			    exynos7420_get_base_gpio_bus0();
	struct exynos7420_gpio_fsys1 *gpio_fsys1 =
			    exynos7420_get_base_gpio_fsys1();
	struct s5p_gpio_bank *bank;
	int gpio;

	switch (gpio_type) {
	case BOARD_ESPRESSO:
		/* GPIO: LCD RESET(gpd4 0 0x1) */
		/* GPF3CON[1]  [7:4] 0x1 = Output */
		bank = &gpio_bus0->d4;
		gpio = 0;
		break;
	default:
		debug("%s: Does not support it!!\n", __func__);
		break;
	}
	//s5p_gpio_set_pull(bank, gpio, GPIO_PULL_DOWN);
	s5p_gpio_set_pull(bank, gpio, GPIO_PULL_UP);/*xufei 20150107*/
	s5p_gpio_cfg_pin(bank, gpio, GPIO_OUTPUT);
	s5p_gpio_set_drv(bank, gpio, GPIO_DRV_4X);

	/* RESET: "0" -> "1" */
    s5p_gpio_set_value(bank, gpio, 1);
	udelay(5000);
	s5p_gpio_set_value(bank, gpio, 0);
	udelay(5000);
	s5p_gpio_set_value(bank, gpio, 1);
	udelay(1000);
}

/* Configure and set a GPIO for LCD_POWER_ON */
static void set_gpio_lcd_power(enum board_gpio_type gpio_type)
{
	struct exynos7420_gpio_bus1 *gpio_bus1 =
			    exynos7420_get_base_gpio_bus1();
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

struct exynos_display_config display_config = {
	.set_mipi_phy = set_mipi_phy_control,
	.set_gpio_hw_te = set_gpio_hw_te,
	.set_gpio_lcd_reset = set_gpio_lcd_reset,
	.set_gpio_lcd_power = set_gpio_lcd_power,
};

/* Get type of board */
enum board_gpio_type get_exynos_board_type(void)
{
	return BOARD_ESPRESSO;
}

/* Get display configuration callbacks */
struct exynos_display_config *get_exynos_display_config(void)
{
	return &display_config;
}
