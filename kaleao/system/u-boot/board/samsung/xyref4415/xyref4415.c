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
#include <asm/io.h>
#include <netdev.h>
#include <version.h>
#include <asm/arch/cpu.h>
#include <asm/arch/clock.h>
#include <asm/arch/power.h>
#include <asm/arch/gpio.h>
#include <asm/arch/mmc.h>
#include <asm/arch/pinmux.h>
#include <asm/arch/sromc.h>
#include <asm/arch/sysreg.h>
#include "pmic.h"

#define DEBOUNCE_DELAY  10000

#define Inp32(addr) (*(volatile u32 *)(addr))
#define GetBits(uAddr, uBaseBit, uMaskValue) \
    ((Inp32(uAddr)>>(uBaseBit))&(uMaskValue))
#define GetPOPType() (GetBits(0x10000004, 4, 0x3))

DECLARE_GLOBAL_DATA_PTR;
unsigned int pmic;

#ifdef CONFIG_SMC911X
static int smc9115_pre_init(void)
{
	u32 smc_bw_conf, smc_bc_conf;
	int err;

	/* Ethernet needs data bus width of 16 bits */
	smc_bw_conf = SROMC_DATA16_WIDTH(CONFIG_ENV_SROM_BANK)
			| SROMC_BYTE_ENABLE(CONFIG_ENV_SROM_BANK);

	smc_bc_conf = SROMC_BC_TACS(0x01) | SROMC_BC_TCOS(0x01)
			| SROMC_BC_TACC(0x06) | SROMC_BC_TCOH(0x01)
			| SROMC_BC_TAH(0x0C)  | SROMC_BC_TACP(0x09)
			| SROMC_BC_PMC(0x01);

	/* Select and configure the SROMC bank */
	err = exynos_pinmux_config(PERIPH_ID_SROMC,
				CONFIG_ENV_SROM_BANK | PINMUX_FLAG_16BIT);
	if (err) {
		debug("SROMC not configured\n");
		return err;
	}

	s5p_config_sromc(CONFIG_ENV_SROM_BANK, smc_bw_conf, smc_bc_conf);
	return 0;
}
#endif

static void display_bl1_version(void)
{
	/* display BL1 version */
#if defined(CONFIG_TRUSTZONE_ENABLE)
#endif
}

#if defined(CONFIG_PM)
static void display_pmic_info(void)
{
	unsigned char pmic_id = 0;
	unsigned char read_buck1_ctrl1 = 0;
	unsigned char read_buck1_ctrl2 = 0;
	unsigned char read_buck2_ctrl1 = 0;
	unsigned char read_buck2_dvs1 = 0;
	unsigned char read_buck3_ctrl1 = 0;
	unsigned char read_buck3_dvs1 = 0;
	unsigned char read_buck4_ctrl1 = 0;
	unsigned char read_buck4_dvs1 = 0;
	unsigned char read_0x54 = 0;
	unsigned char read_0x5A = 0;
	unsigned char read_0x62 = 0;
	unsigned char read_0x34 = 0;
	unsigned char read_0x46 = 0;
	unsigned char read_0x0A = 0;

	unsigned char read_ONOFFINT1 = 0;
	unsigned char read_ONOFFINT2 = 0;
	unsigned char read_RTCINT3 = 0;
	unsigned char read_ONOFFINT1M = 0;
	unsigned char read_ONOFFINT2M = 0;
	unsigned char read_RTCINT3M = 0;
	unsigned char read_STATUS1 = 0;
	unsigned char read_STATUS2 = 0;
	unsigned char read_CTRL1 = 0;
	unsigned char read_CTRL2 = 0;
	unsigned char read_BUCK9_CTRL1 = 0;
	unsigned char read_BUCK9_CTRL2 = 0;
	unsigned char read_PWRONSRC = 0;

	IIC0_ERead(S5M8767_RD_ADDR, PMIC_ID, &pmic_id);

	IIC0_ERead(S5M8767_RD_ADDR, 0x32, &read_buck1_ctrl1);
	IIC0_ERead(S5M8767_RD_ADDR, 0x33, &read_buck1_ctrl2);
	IIC0_ERead(S5M8767_RD_ADDR, 0x34, &read_buck2_ctrl1);
	IIC0_ERead(S5M8767_RD_ADDR, 0x35, &read_buck2_dvs1);
	IIC0_ERead(S5M8767_RD_ADDR, 0x3d, &read_buck3_ctrl1);
	IIC0_ERead(S5M8767_RD_ADDR, 0x3e, &read_buck3_dvs1);
	IIC0_ERead(S5M8767_RD_ADDR, 0x46, &read_buck4_ctrl1);
	IIC0_ERead(S5M8767_RD_ADDR, 0x47, &read_buck4_dvs1);

	IIC0_ERead(S5M8767_RD_ADDR, 0x54, &read_0x54);
	IIC0_ERead(S5M8767_RD_ADDR, 0x5A, &read_0x5A);
	IIC0_ERead(S5M8767_RD_ADDR, 0x62, &read_0x62);
	IIC0_ERead(S5M8767_RD_ADDR, 0x34, &read_0x34);
	IIC0_ERead(S5M8767_RD_ADDR, 0x46, &read_0x46);
	IIC0_ERead(S5M8767_RD_ADDR, 0x0A, &read_0x0A);

	IIC0_ERead(S5M8767_RD_ADDR, ONOFFINT1, &read_ONOFFINT1);
	IIC0_ERead(S5M8767_RD_ADDR, ONOFFINT2, &read_ONOFFINT2);
	IIC0_ERead(S5M8767_RD_ADDR, RTCINT3, &read_RTCINT3);
	IIC0_ERead(S5M8767_RD_ADDR, ONOFFINT1M, &read_ONOFFINT1M);
	IIC0_ERead(S5M8767_RD_ADDR, ONOFFINT2M, &read_ONOFFINT2M);
	IIC0_ERead(S5M8767_RD_ADDR, RTCINT3M, &read_RTCINT3M);
	IIC0_ERead(S5M8767_RD_ADDR, STATUS1, &read_STATUS1);
	IIC0_ERead(S5M8767_RD_ADDR, STATUS2, &read_STATUS2);
	IIC0_ERead(S5M8767_RD_ADDR, CTRL1, &read_CTRL1);
	IIC0_ERead(S5M8767_RD_ADDR, CTRL2, &read_CTRL2);
	IIC0_ERead(S5M8767_RD_ADDR, BUCK9_CTRL1, &read_BUCK9_CTRL1);
	IIC0_ERead(S5M8767_RD_ADDR, BUCK9_CTRL2, &read_BUCK9_CTRL2);
	IIC0_ERead(S5M8767_RD_ADDR, PWRONSRC, &read_PWRONSRC);

	printf("\nPMIC: S5M8767(REV%x)\n", pmic_id);
	printf("BUCK1_CTRL1: %X\n", read_buck1_ctrl1);
	printf("BUCK1_CTRL2(VDD_MIF): %X\n", read_buck1_ctrl2);
	printf("BUCK2_CTRL: %X\n", read_buck2_ctrl1);
	printf("BUCK2_DVS1(VDD_CPU): %X\n", read_buck2_dvs1);
	printf("BUCK3_CTRL: %X\n", read_buck3_ctrl1);
	printf("BUCK3_DVS1(VDD_INT): %X\n", read_buck3_dvs1);
	printf("BUCK4_CTRL: %X\n", read_buck4_ctrl1);
	printf("BUCK4_DVS1(VDD_G3D): %X\n", read_buck4_dvs1);

	printf("0x54: %X\n", read_0x54);
	printf("0x5A: %X\n", read_0x5A);
	printf("0x62: %X\n", read_0x62);
	printf("0x34: %X\n", read_0x34);
	printf("0x46: %X\n", read_0x46);
	printf("0x0A: %X\n", read_0x0A);

	printf("ONOFFINT1: %X\n", read_ONOFFINT1);
	printf("ONOFFINT2: %X\n", read_ONOFFINT2);
	printf("RTCINT3: %X\n", read_RTCINT3);
	printf("ONOFFINT1M: %X\n", read_ONOFFINT1M);
	printf("ONOFFINT2M: %X\n", read_ONOFFINT2M);
	printf("RTCINT3M: %X\n", read_RTCINT3M);
	printf("STATUS1: %X\n", read_STATUS1);
	printf("STATUS2: %X\n", read_STATUS2);
	printf("CTRL1: %X\n", read_CTRL1);
	printf("CTRL2: %X\n", read_CTRL2);
	printf("BUCK9_CTRL1: %X\n", read_BUCK9_CTRL1);
	printf("BUCK9_CTRL2: %X\n", read_BUCK9_CTRL2);
	printf("PWRONSRC: %X\n", read_PWRONSRC);
}

static void keyled_ctrl(int ctrl)
{
#if defined(CONFIG_KEYLED_ENABLE)
	unsigned char pmic_id = 0;
	unsigned char ldo33_ctrl = 0;

	IIC0_ERead(S5M8767_RD_ADDR, PMIC_ID, &pmic_id);
	if (pmic_id == REV_01) {
		/* Key LED on */
		IIC0_ERead(S5M8767_RD_ADDR, REV01_LDO33_CTRL, &ldo33_ctrl);
		if (ctrl) {
			ldo33_ctrl |= OUTPUT_PWREN_ON;
		} else {
			ldo33_ctrl &= OUTPUT_OFF;
		}
		IIC0_EWrite(S5M8767_WR_ADDR, REV01_LDO33_CTRL, ldo33_ctrl);
	}
#endif
}

static void motor_ctrl(int ctrl)
{
#if defined(CONFIG_MOTOR_CTRL)
	unsigned char pmic_id = 0;
	unsigned char ldo34_ctrl = 0;

	IIC0_ERead(S5M8767_RD_ADDR, PMIC_ID, &pmic_id);
	if (pmic_id == REV_01) {
		/* Motor on/off control */
		IIC0_ERead(S5M8767_RD_ADDR, REV01_LDO34_CTRL, &ldo34_ctrl);
		if (ctrl) {
			ldo34_ctrl |= OUTPUT_PWREN_ON;
		} else {
			ldo34_ctrl &= OUTPUT_OFF;
		}
		IIC0_EWrite(S5M8767_WR_ADDR, REV01_LDO34_CTRL, ldo34_ctrl);
	}
#endif
}

#endif

static void display_boot_device_info(void)
{
	struct exynos4_power *pmu = (struct exynos4_power *)EXYNOS4_POWER_BASE;
	int OmPin;

	OmPin = readl(&pmu->inform3);

	printf("\nChecking Boot Mode ...");

	if (OmPin == BOOT_MMCSD) {
		printf(" SDMMC\n");
	} else if (OmPin == BOOT_EMMC) {
		printf(" EMMC\n");
	} else if (OmPin == BOOT_EMMC_4_4) {
		printf(" EMMC\n");
	} else {
		printf(" Please check OM_pin\n");
	}
}

int board_init(void)
{
	keyled_ctrl(ON);

	display_bl1_version();

#if defined(CONFIG_PM)
	display_pmic_info();
#endif
	display_boot_device_info();

	gd->bd->bi_boot_params = (PHYS_SDRAM_1 + 0x100UL);

	return 0;
}

int dram_init(void)
{
#if defined(CONFIG_SDRAM_SIZE_1536MB)
	gd->ram_size	= get_ram_size((long *)PHYS_SDRAM_1, PHYS_SDRAM_1_SIZE)
			+ get_ram_size((long *)PHYS_SDRAM_2, PHYS_SDRAM_2_SIZE)
			+ get_ram_size((long *)PHYS_SDRAM_3, PHYS_SDRAM_3_SIZE)
			+ get_ram_size((long *)PHYS_SDRAM_4, PHYS_SDRAM_4_SIZE)
			+ get_ram_size((long *)PHYS_SDRAM_5, PHYS_SDRAM_5_SIZE)
			+ get_ram_size((long *)PHYS_SDRAM_6, PHYS_SDRAM_6_SIZE);
#else
	if ((GetPOPType() == 2) || (GetPOPType() == 1)) {
		gd->ram_size	= get_ram_size((long *)PHYS_SDRAM_1, PHYS_SDRAM_1_SIZE)
			+ get_ram_size((long *)PHYS_SDRAM_2, PHYS_SDRAM_2_SIZE)
			+ get_ram_size((long *)PHYS_SDRAM_3, PHYS_SDRAM_3_SIZE)
			+ get_ram_size((long *)PHYS_SDRAM_4, PHYS_SDRAM_4_SIZE)
			+ get_ram_size((long *)PHYS_SDRAM_5, PHYS_SDRAM_5_SIZE)
			+ get_ram_size((long *)PHYS_SDRAM_6, PHYS_SDRAM_6_SIZE);
	} else {
		gd->ram_size	= get_ram_size((long *)PHYS_SDRAM_1, PHYS_SDRAM_1_SIZE)
			+ get_ram_size((long *)PHYS_SDRAM_2, PHYS_SDRAM_2_SIZE)
			+ get_ram_size((long *)PHYS_SDRAM_3, PHYS_SDRAM_3_SIZE)
			+ get_ram_size((long *)PHYS_SDRAM_4, PHYS_SDRAM_4_END_SIZE);
	}
#endif
	return 0;
}

void dram_init_banksize(void)
{
#if defined(CONFIG_SDRAM_SIZE_1536MB)
	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	gd->bd->bi_dram[0].size = get_ram_size((long *)PHYS_SDRAM_1,
							PHYS_SDRAM_1_SIZE);
	gd->bd->bi_dram[1].start = PHYS_SDRAM_2;
	gd->bd->bi_dram[1].size = get_ram_size((long *)PHYS_SDRAM_2,
							PHYS_SDRAM_2_SIZE);
	gd->bd->bi_dram[2].start = PHYS_SDRAM_3;
	gd->bd->bi_dram[2].size = get_ram_size((long *)PHYS_SDRAM_3,
							PHYS_SDRAM_3_SIZE);
	gd->bd->bi_dram[3].start = PHYS_SDRAM_4;
	gd->bd->bi_dram[3].size = get_ram_size((long *)PHYS_SDRAM_4,
							PHYS_SDRAM_4_SIZE);
	gd->bd->bi_dram[4].start = PHYS_SDRAM_5;
	gd->bd->bi_dram[4].size = get_ram_size((long *)PHYS_SDRAM_5,
							PHYS_SDRAM_5_SIZE);
	gd->bd->bi_dram[5].start = PHYS_SDRAM_6;
	gd->bd->bi_dram[5].size = get_ram_size((long *)PHYS_SDRAM_6,
							PHYS_SDRAM_6_SIZE);
#else
	if ((GetPOPType() == 2) || (GetPOPType() == 1)) {
		gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
		gd->bd->bi_dram[0].size = get_ram_size((long *)PHYS_SDRAM_1,
								PHYS_SDRAM_1_SIZE);
		gd->bd->bi_dram[1].start = PHYS_SDRAM_2;
		gd->bd->bi_dram[1].size = get_ram_size((long *)PHYS_SDRAM_2,
								PHYS_SDRAM_2_SIZE);
		gd->bd->bi_dram[2].start = PHYS_SDRAM_3;
		gd->bd->bi_dram[2].size = get_ram_size((long *)PHYS_SDRAM_3,
								PHYS_SDRAM_3_SIZE);
		gd->bd->bi_dram[3].start = PHYS_SDRAM_4;
		gd->bd->bi_dram[3].size = get_ram_size((long *)PHYS_SDRAM_4,
								PHYS_SDRAM_4_SIZE);
		gd->bd->bi_dram[4].start = PHYS_SDRAM_5;
		gd->bd->bi_dram[4].size = get_ram_size((long *)PHYS_SDRAM_5,
								PHYS_SDRAM_5_SIZE);
		gd->bd->bi_dram[5].start = PHYS_SDRAM_6;
		gd->bd->bi_dram[5].size = get_ram_size((long *)PHYS_SDRAM_6,
								PHYS_SDRAM_6_SIZE);
	} else {
		gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
		gd->bd->bi_dram[0].size = get_ram_size((long *)PHYS_SDRAM_1,
								PHYS_SDRAM_1_SIZE);
		gd->bd->bi_dram[1].start = PHYS_SDRAM_2;
		gd->bd->bi_dram[1].size = get_ram_size((long *)PHYS_SDRAM_2,
								PHYS_SDRAM_2_SIZE);
		gd->bd->bi_dram[2].start = PHYS_SDRAM_3;
		gd->bd->bi_dram[2].size = get_ram_size((long *)PHYS_SDRAM_3,
								PHYS_SDRAM_3_SIZE);
		gd->bd->bi_dram[3].start = PHYS_SDRAM_4;
		gd->bd->bi_dram[3].size = get_ram_size((long *)PHYS_SDRAM_4,
								PHYS_SDRAM_4_END_SIZE);
	}
#endif
}

int board_eth_init(bd_t *bis)
{
#ifdef CONFIG_SMC911X
	if (smc9115_pre_init())
		return -1;
	return smc911x_initialize(0, CONFIG_SMC911X_BASE);
#endif
	return 0;
}

#ifdef CONFIG_DISPLAY_BOARDINFO
int checkboard(void)
{
	printf("\nBoard: XYREF4415\n");

	return 0;
}
#endif

#ifdef CONFIG_GENERIC_MMC
int board_mmc_init(bd_t *bis)
{
	struct exynos4_power *pmu = (struct exynos4_power *)EXYNOS4_POWER_BASE;
	int err, OmPin;

	OmPin = readl(&pmu->inform3);

	err = exynos_pinmux_config(PERIPH_ID_SDMMC2, PINMUX_FLAG_NONE);
	if (err) {
		debug("SDMMC2 not configured\n");
		return err;
	}

	err = exynos_pinmux_config(PERIPH_ID_SDMMC0, PINMUX_FLAG_8BIT_MODE);
	if (err) {
		debug("MSHC0 not configured\n");
		return err;
	}

	switch (OmPin) {
	case BOOT_EMMC_4_4:
#if defined(USE_MMC0)
		set_mmc_clk(PERIPH_ID_SDMMC0, 2);

		err = s5p_mmc_init(PERIPH_ID_SDMMC0, 8);
#endif
#if defined(USE_MMC2)
		set_mmc_clk(PERIPH_ID_SDMMC2, 2);

		err = s5p_mmc_init(PERIPH_ID_SDMMC2, 4);
#endif
		break;
	default:
#if defined(USE_MMC2)
		set_mmc_clk(PERIPH_ID_SDMMC2, 2);

		err = s5p_mmc_init(PERIPH_ID_SDMMC2, 4);
#endif
#if defined(USE_MMC0)
		set_mmc_clk(PERIPH_ID_SDMMC0, 2);

		err = s5p_mmc_init(PERIPH_ID_SDMMC0, 8);
#endif
		break;
	}

	return err;
}
#endif

static int board_uart_init(void)
{
	int err;

	err = exynos_pinmux_config(PERIPH_ID_UART0, PINMUX_FLAG_NONE);
	if (err) {
		debug("UART0 not configured\n");
		return err;
	}

	err = exynos_pinmux_config(PERIPH_ID_UART1, PINMUX_FLAG_NONE);
	if (err) {
		debug("UART1 not configured\n");
		return err;
	}

	err = exynos_pinmux_config(PERIPH_ID_UART2, PINMUX_FLAG_NONE);
	if (err) {
		debug("UART2 not configured\n");
		return err;
	}

	err = exynos_pinmux_config(PERIPH_ID_UART3, PINMUX_FLAG_NONE);
	if (err) {
		debug("UART3 not configured\n");
		return err;
	}

	return 0;
}

#ifdef CONFIG_BOARD_EARLY_INIT_F
int board_early_init_f(void)
{
	return board_uart_init();
}
#endif

int board_late_init(void)
{
	struct exynos4_power *pmu = (struct exynos4_power *)EXYNOS4_POWER_BASE;
	struct exynos4_gpio_part2 *gpio2 =
		(struct exynos4_gpio_part2 *) samsung_get_base_gpio_part2();
	unsigned int gpio_debounce_cnt = 0;
	unsigned int rst_stat = 0;
	int err;
	u32 second_boot_info = readl(CONFIG_SECONDARY_BOOT_INFORM_BASE);

	rst_stat = readl(&pmu->rst_stat);
	printf("rst_stat : 0x%x\n", rst_stat);

	err = exynos_pinmux_config(PERIPH_ID_INPUT_X0_0, PINMUX_FLAG_NONE);
	if (err) {
		debug("GPX0_0 INPUT not configured\n");
		return err;
	}

#if 0
	while (s5p_gpio_get_value(&gpio2->x0, 0x0) == 0x0) {
		/* wait for 2s */
		if (gpio_debounce_cnt < 5) {
			udelay(DEBOUNCE_DELAY);
			gpio_debounce_cnt++;
		} else {
			second_boot_info = 1;
			break;
		}
	}
#endif
#ifdef CONFIG_RAMDUMP_MODE
	/*  check reset status for ramdump */
	if ((rst_stat & (WRESET | SYS_WDTRESET | ISP_ARM_WDTRESET))
		|| (readl(CONFIG_RAMDUMP_SCRATCH) == CONFIG_RAMDUMP_MODE)) {
		/*  run fastboot */
		run_command("fastboot", 0);
	}
#endif
	if (second_boot_info == 1) {
		printf("###Recovery Boot Mode###\n");
		setenv("bootcmd", CONFIG_RECOVERYCOMMAND);
		/* clear secondary boot inform */
		writel(0x0, CONFIG_SECONDARY_BOOT_INFORM_BASE);
	}

	keyled_ctrl(OFF);
	return 0;
}

unsigned int get_board_rev(void)
{
	struct exynos4_clock *clk = (struct exynos4_clock *)EXYNOS4_CLOCK_BASE;
	struct exynos4_power *pmu = (struct exynos4_power *)EXYNOS4_POWER_BASE;
	unsigned int rev = 0;
	int adc_val = 0;
	unsigned int timeout, con;

	return (rev | pmic);
}
