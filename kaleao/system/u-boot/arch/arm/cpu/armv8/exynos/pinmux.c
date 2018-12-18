/*
 * Copyright (c) 2012 Samsung Electronics.
 * Abhilash Kesavan <a.kesavan@samsung.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef CONFIG_SPL_BUILD

#include <common.h>
#include <asm/arch/gpio.h>
#include <asm/arch/pinmux.h>
#include <asm/arch/sromc.h>

static void exynos5430_uart_config(int peripheral)
{
	struct exynos5430_gpio_peric *gpio_peric =
			(struct exynos5430_gpio_peric *)
			    exynos5430_get_base_gpio_peric();
	struct s5p_gpio_bank *bank;
	int i, start, count;

	switch (peripheral) {
	case PERIPH_ID_UART0:
		bank = &gpio_peric->d0;
		start = 0;
		count = 4;
		break;
	case PERIPH_ID_UART1:
		bank = &gpio_peric->d1;
		start = 0;
		count = 4;
		break;
	case PERIPH_ID_UART2:
		bank = &gpio_peric->d1;
		start = 4;
		count = 2;
		break;
	}
	for (i = start; i < start + count; i++) {
		s5p_gpio_set_pull(bank, i, GPIO_PULL_NONE);
		s5p_gpio_cfg_pin(bank, i, GPIO_FUNC(0x2));
	}
}

static void exynos7420_uart_config(int peripheral)
{
	struct exynos7420_gpio_bus0 *gpio_bus0 =
			    exynos7420_get_base_gpio_bus0();
	struct s5p_gpio_bank *bank;
	int i, start, count;

	switch (peripheral) {
	case PERIPH_ID_UART0:
		bank = &gpio_bus0->d0;
		start = 0;
		count = 4;
		break;
	case PERIPH_ID_UART1:
		bank = &gpio_bus0->d1;
		start = 0;
		count = 4;
		break;
	case PERIPH_ID_UART2:
		bank = &gpio_bus0->d1;
		start = 4;
		count = 2;
		break;
	case PERIPH_ID_UART3:
		bank = &gpio_bus0->d5;
		start = 0;
		count = 2;
		break;
	}
	for (i = start; i < start + count; i++) {
		s5p_gpio_set_pull(bank, i, GPIO_PULL_NONE);
		s5p_gpio_cfg_pin(bank, i, GPIO_FUNC(0x2));
	}
}

static void exynos7580_uart_config(int peripheral)
{
        struct exynos7580_gpio_top *gpio_top =
                            exynos7580_get_base_gpio_top();
        struct s5p_gpio_bank *bank;
        int i, start, count;

        switch (peripheral) {
        case PERIPH_ID_UART0:
                bank = &gpio_top->b0;
                start = 4;
                count = 4;
                break;
        case PERIPH_ID_UART1:
                bank = &gpio_top->b0;
                start = 0;
                count = 4;
                break;
        case PERIPH_ID_UART2:
                bank = &gpio_top->b1;
                start = 0;
                count = 2;
                break;
        }
        for (i = start; i < start + count; i++) {
                s5p_gpio_set_pull(bank, i, GPIO_PULL_NONE);
                s5p_gpio_cfg_pin(bank, i, GPIO_FUNC(0x2));
        }
}

static int exynos5430_mmc_config(int peripheral, int flags)
{
	struct exynos5430_gpio_fsys *gpio_fsys =
			(struct exynos5430_gpio_fsys *)
			    exynos5430_get_base_gpio_fsys();
	struct s5p_gpio_bank *bank, *bank_data, *bank_ext_data;
	int i, bank_bit, bank_bit_cnt;
	int bank_data_bit, bank_ext_data_bit;

	switch (peripheral) {
	case PERIPH_ID_SDMMC0:
		bank = &gpio_fsys->r0;
		bank_data = &gpio_fsys->r1;
		bank_ext_data = &gpio_fsys->r1;
		bank_bit = 0;
		bank_bit_cnt = 4;
		bank_data_bit = 0;
		bank_ext_data_bit = 4;
		break;
	case PERIPH_ID_SDMMC1:
		bank = &gpio_fsys->r2;
		bank_data = &gpio_fsys->r3;
		bank_ext_data = &gpio_fsys->r3;
		bank_bit = 0;
		bank_bit_cnt = 2;
		bank_data_bit = 0;
		bank_ext_data_bit = 4;
		break;
	case PERIPH_ID_SDMMC2:
		bank = &gpio_fsys->r4;
		bank_data = &gpio_fsys->r4;
		bank_ext_data = NULL;
		bank_bit = 0;
		bank_bit_cnt = 4;
		bank_data_bit = 4;
		break;
	default:
		debug("%s: invalid peripheral %d", __func__, peripheral);
		return -1;
	}
	if ((flags & PINMUX_FLAG_8BIT_MODE) && !bank_ext_data) {
		debug("SDMMC device %d does not support 8bit mode",
				peripheral);
		return -1;
	}
	for (i = bank_bit; i < bank_bit_cnt; i++) {
		s5p_gpio_cfg_pin(bank, i, GPIO_FUNC(0x2));
		s5p_gpio_set_pull(bank, i, GPIO_PULL_NONE);
		s5p_gpio_set_drv(bank, i, GPIO_DRV_4X);
	}
	for (i = bank_data_bit; i < (bank_data_bit + 4); i++) {
		s5p_gpio_cfg_pin(bank_data, i, GPIO_FUNC(0x2));
		s5p_gpio_set_pull(bank_data, i, GPIO_PULL_UP);
		s5p_gpio_set_drv(bank_data, i, GPIO_DRV_4X);
	}
	if (flags & PINMUX_FLAG_8BIT_MODE) {
		for (i = bank_ext_data_bit; i < (bank_ext_data_bit + 4); i++) {
			s5p_gpio_cfg_pin(bank_ext_data, i, GPIO_FUNC(0x2));
			s5p_gpio_set_pull(bank_ext_data, i, GPIO_PULL_UP);
			s5p_gpio_set_drv(bank_ext_data, i, GPIO_DRV_4X);
		}
	}
#ifdef CONFIG_CPU_EXYNOS5433_EVT1
	/* Exynos5433 EVT1 is ePOP and Espresso5433 for EVT1
	   does not pull up MMC 0 CMD GPIO.
	   So, set it as pull up */
	*(int *)(0x15690048) |= 0xC;
#endif
	return 0;
}

int exynos7420_emmc_voltage_config(void)
{
	struct exynos7420_gpio_bus1 *gpio_bus1 =
		(struct exynos7420_gpio_bus1 *)exynos7420_get_base_gpio_bus1();
	struct s5p_gpio_bank *bank;
	/* bootn */
	bank = &gpio_bus1->h1;
	s5p_gpio_direction_output(bank, 4, 1);
	return 0;
}
static int exynos7420_mmc_config(int peripheral, int flags)
{
	struct exynos7420_gpio_fsys0 *gpio_fsys0 =
			    exynos7420_get_base_gpio_fsys0();
	struct exynos7420_gpio_fsys1 *gpio_fsys1 =
			    exynos7420_get_base_gpio_fsys1();
	struct s5p_gpio_bank *bank, *bank_data, *bank_ext_data;
	int i, bank_bit, bank_bit_cnt;
	int bank_data_bit, bank_ext_data_bit;

	switch (peripheral) {
	case PERIPH_ID_SDMMC0:
		bank = &gpio_fsys1->r0;
		bank_data = &gpio_fsys1->r1;
		bank_ext_data = &gpio_fsys1->r1;
		bank_bit = 0;
		bank_bit_cnt = 4;
		bank_data_bit = 0;
		bank_ext_data_bit = 4;
		break;
	case PERIPH_ID_SDMMC1:
		bank = &gpio_fsys1->r2;
		bank_data = &gpio_fsys1->r3;
		bank_ext_data = &gpio_fsys1->r3;
		bank_bit = 0;
		bank_bit_cnt = 2;
		bank_data_bit = 0;
		bank_ext_data_bit = 4;
		break;
	case PERIPH_ID_SDMMC2:
		bank = &gpio_fsys0->r4;
		bank_data = &gpio_fsys0->r4;
		bank_ext_data = NULL;
		bank_bit = 0;
		bank_bit_cnt = 4;
		bank_data_bit = 4;
		break;
	default:
		debug("%s: invalid peripheral %d", __func__, peripheral);
		return -1;
	}
	if ((flags & PINMUX_FLAG_8BIT_MODE) && !bank_ext_data) {
		debug("SDMMC device %d does not support 8bit mode",
				peripheral);
		return -1;
	}
	for (i = bank_bit; i < bank_bit_cnt; i++) {
		s5p_gpio_cfg_pin(bank, i, GPIO_FUNC(0x2));
		s5p_gpio_set_pull(bank, i, GPIO_PULL_NONE);
		s5p_gpio_set_drv(bank, i, GPIO_DRV_4X);
	}
	for (i = bank_data_bit; i < (bank_data_bit + 4); i++) {
		s5p_gpio_cfg_pin(bank_data, i, GPIO_FUNC(0x2));
		s5p_gpio_set_pull(bank_data, i, GPIO_PULL_UP);
		s5p_gpio_set_drv(bank_data, i, GPIO_DRV_4X);
	}
	if (flags & PINMUX_FLAG_8BIT_MODE) {
		for (i = bank_ext_data_bit; i < (bank_ext_data_bit + 4); i++) {
			s5p_gpio_cfg_pin(bank_ext_data, i, GPIO_FUNC(0x2));
			s5p_gpio_set_pull(bank_ext_data, i, GPIO_PULL_UP);
			s5p_gpio_set_drv(bank_ext_data, i, GPIO_DRV_4X);
		}
	}
	return 0;
}

static int exynos7580_mmc_config(int peripheral, int flags)
{
	struct exynos7580_gpio_fsys *gpio_fsys =
			    exynos7580_get_base_gpio_fsys();
	struct s5p_gpio_bank *bank, *bank_data, *bank_ext_data;
	int i, bank_bit, bank_bit_cnt;
	int bank_data_bit, bank_ext_data_bit;

	switch (peripheral) {
	case PERIPH_ID_SDMMC0:
		bank = &gpio_fsys->r0;
		bank_data = &gpio_fsys->r1;
		bank_ext_data = &gpio_fsys->r1;
		bank_bit = 0;
		bank_bit_cnt = 3;
		bank_data_bit = 0;
		bank_ext_data_bit = 4;
		break;
	case PERIPH_ID_SDMMC1:
		bank = &gpio_fsys->r2;
		bank_data = &gpio_fsys->r3;
		bank_ext_data = &gpio_fsys->r3;
		bank_bit = 0;
		bank_bit_cnt = 2;
		bank_data_bit = 0;
		bank_ext_data_bit = 2;
		break;
	case PERIPH_ID_SDMMC2:
		bank = &gpio_fsys->r4;
		bank_data = &gpio_fsys->r4;
		bank_ext_data = NULL;
		bank_bit = 0;
		bank_bit_cnt = 3;
		bank_data_bit = 3;
		break;
	default:
		debug("%s: invalid peripheral %d", __func__, peripheral);
		return -1;
	}

	if ((flags & PINMUX_FLAG_8BIT_MODE) && !bank_ext_data) {
		debug("SDMMC device %d does not support 8bit mode", peripheral);
		return -1;
	}

	for (i = bank_bit; i < bank_bit_cnt; i++) {
		s5p_gpio_cfg_pin(bank, i, GPIO_FUNC(0x2));
		s5p_gpio_set_pull(bank, i, GPIO_PULL_NONE);
		s5p_gpio_set_drv(bank, i, GPIO_DRV_4X);
	}

	/* HW PULL-UP resistors of eMMC are removed for reducing BOM cost.
	   So, CMD of MMC0 (GPR0[1]) needs internal PULL-UP */
	if (peripheral == PERIPH_ID_SDMMC0)
		s5p_gpio_set_pull(bank, 1, GPIO_PULL_UP);

	for (i = bank_data_bit; i < (bank_data_bit + 4); i++) {
		s5p_gpio_cfg_pin(bank_data, i, GPIO_FUNC(0x2));
		s5p_gpio_set_pull(bank_data, i, GPIO_PULL_UP);
		s5p_gpio_set_drv(bank_data, i, GPIO_DRV_4X);
	}

	if (flags & PINMUX_FLAG_8BIT_MODE) {
		for (i = bank_ext_data_bit; i < (bank_ext_data_bit + 4); i++) {
			s5p_gpio_cfg_pin(bank_ext_data, i, GPIO_FUNC(0x2));
			s5p_gpio_set_pull(bank_ext_data, i, GPIO_PULL_UP);
			s5p_gpio_set_drv(bank_ext_data, i, GPIO_DRV_4X);
		}
	}

        return 0;
}

static void exynos5430_input_config(int peripheral)
{
	struct exynos5430_gpio_alive *gpio_alive =
			(struct exynos5430_gpio_alive *)
			    exynos5430_get_base_gpio_alive();
	struct s5p_gpio_bank *bank;
	int i, start = 0, count = 0;

	switch (peripheral) {
	case PERIPH_ID_INPUT_A0_3:
		bank = &gpio_alive->a0;
		start = 3;
		count = 1;
		break;
	case PERIPH_ID_INPUT_A2_0:
		bank = &gpio_alive->a2;
		start = 0;
		count = 1;
		break;
	case PERIPH_ID_INPUT_A2_1:
		bank = &gpio_alive->a2;
		start = 1;
		count = 1;
		break;
	}
	for (i = start; i < start + count; i++) {
		s5p_gpio_set_pull(bank, i, GPIO_PULL_NONE);
		s5p_gpio_direction_input(bank, i);
	}
}

static int exynos7420_ufs_config(int peripheral, int flags)
{
	struct exynos7420_gpio_bus1 *gpio_bus1 =
		(struct exynos7420_gpio_bus1 *)exynos7420_get_base_gpio_bus1();
	struct s5p_gpio_bank *bank;

	switch (peripheral) {
		case PERIPH_ID_UFS0:
			/* refclk */
			bank = &gpio_bus1->g2;
			s5p_gpio_cfg_pin(bank, 4, GPIO_FUNC(0x2));
			s5p_gpio_set_pull(bank, 4, GPIO_PULL_NONE);

			/* reset_n */
			bank = &gpio_bus1->h1;
			s5p_gpio_cfg_pin(bank, 5, GPIO_FUNC(0x2));
			s5p_gpio_set_pull(bank, 5, GPIO_PULL_UP);

			/* boot */
			s5p_gpio_cfg_pin(bank, 4, GPIO_OUTPUT);
			break;
		default:
			debug("%s: invalid peripheral %d", __func__, peripheral);
			return -1;
	}
	return 0;
}

static int exynos5_pinmux_config(int peripheral, int flags)
{
	switch (peripheral) {
	case PERIPH_ID_UART0:
	case PERIPH_ID_UART1:
	case PERIPH_ID_UART2:
	case PERIPH_ID_UART3:
		if (s5p_cpu_id == 0x5430) {
			exynos5430_uart_config(peripheral);
		}
		break;
	case PERIPH_ID_SDMMC0:
	case PERIPH_ID_SDMMC1:
	case PERIPH_ID_SDMMC2:
	case PERIPH_ID_SDMMC3:
		if (s5p_cpu_id == 0x5430) {
			return exynos5430_mmc_config(peripheral, flags);
		}
	case PERIPH_ID_SROMC:
	case PERIPH_ID_INPUT_X0_0:
	case PERIPH_ID_INPUT_X2_0:
	case PERIPH_ID_INPUT_A0_3:
	case PERIPH_ID_INPUT_A2_0:
	case PERIPH_ID_INPUT_A2_1:
		if (s5p_cpu_id == 0x5430) {
			exynos5430_input_config(peripheral);
		}
		break;
	default:
		debug("%s: invalid peripheral %d", __func__, peripheral);
		return -1;
	}

	return 0;
}

static int exynos7_pinmux_config(int peripheral, int flags)
{
	switch (peripheral) {
	case PERIPH_ID_UART0:
	case PERIPH_ID_UART1:
	case PERIPH_ID_UART2:
	case PERIPH_ID_UART3:
		if (s5p_cpu_id == 0x7420) {
			exynos7420_uart_config(peripheral);
		}
		if (s5p_cpu_id == 0x7580) {
			exynos7580_uart_config(peripheral);
		}
		break;
	case PERIPH_ID_SDMMC0:
	case PERIPH_ID_SDMMC1:
	case PERIPH_ID_SDMMC2:
	case PERIPH_ID_SDMMC3:
		if (s5p_cpu_id == 0x7420) {
			return exynos7420_mmc_config(peripheral, flags);
		}
		if (s5p_cpu_id == 0x7580) {
			return exynos7580_mmc_config(peripheral, flags);
		}
		break;
	case PERIPH_ID_UFS0:
		if (s5p_cpu_id == 0x7420)
			return exynos7420_ufs_config(peripheral, flags);
		break;
	default:
		debug("%s: invalid peripheral %d", __func__, peripheral);
		return -1;
	}

	return 0;
}

int exynos_pinmux_config(int peripheral, int flags)
{
	if (cpu_is_exynos5())
		return exynos5_pinmux_config(peripheral, flags);
	if (cpu_is_exynos7())
		return exynos7_pinmux_config(peripheral, flags);
	else {
		debug("pinmux functionality not supported\n");
		return -1;
	}
}
#endif /* CONFIG_SPL_BUILD */
