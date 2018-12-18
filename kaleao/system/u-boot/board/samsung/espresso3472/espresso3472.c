/*
 * Copyright (C) 2014 Samsung Electronics
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
#include <asm/arch/cpu.h>
#include <asm/arch/clk.h>
#include <asm/arch/clock.h>
#include <asm/arch/power.h>
#include <asm/arch/gpio.h>
#include <asm/arch/mmc.h>
#include <asm/arch/pinmux.h>
#include <asm/arch/sromc.h>
#include <asm/arch/sysreg.h>
#include "pmic.h"

#define DEBOUNCE_DELAY	10000

DECLARE_GLOBAL_DATA_PTR;
unsigned int OmPin;


int board_init(void)
{
	u8 read_vol_arm;
	u8 read_vol_int, read_vol_g3d;
	u8 read_vol_mif, read_rtc_ctrl;
	u8 read_buck;
	u8 temp;
	unsigned int reg;

	char bl1_version[9] = {0};

	/* display BL1 version */
#ifdef CONFIG_TRUSTZONE_ENABLE
	printf("\nTrustZone Enabled BSP");
	strncpy(&bl1_version[0], (char *)(CONFIG_PHY_IRAM_NS_BASE + 0x810), 8);
	printf("\nBL1 version: %s\n", &bl1_version[0]);
#endif

#if defined(CONFIG_PM)
#if 0
	IIC0_ERead(S2MPU02X_RD_ADDR, BUCK1_CTRL2, &read_vol_mif);
	IIC0_ERead(S2MPU02X_RD_ADDR, BUCK2_CTRL2, &read_vol_arm);
	IIC0_ERead(S2MPU02X_RD_ADDR, BUCK3_CTRL2, &read_vol_int);
	IIC0_ERead(S2MPU02X_RD_ADDR, BUCK4_CTRL2, &read_vol_g3d);
	printf("MIF: %dmV\t", RD_BUCK_VOLT((unsigned int)read_vol_mif));
	printf("ARM: %dmV\t", RD_BUCK_VOLT((unsigned int)read_vol_arm));
	printf("INT: %dmV\t", RD_BUCK_VOLT((unsigned int)read_vol_int));
	printf("G3D: %dmV\t\n", RD_BUCK_VOLT((unsigned int)read_vol_g3d));
#endif
#endif

	gd->bd->bi_boot_params = (PHYS_SDRAM_1+0x100);

	OmPin = __REG(EXYNOS4_POWER_BASE + INFORM3_OFFSET);
	#define SDMMC_DEV		0x4
	unsigned int om_status = readl(EXYNOS4_POWER_BASE + OM_STATUS_OFFSET);
	if(om_status == SDMMC_DEV) {
		OmPin = BOOT_EMMC_4_4;
	}

	printf("\nChecking Boot Mode ...");
	if (OmPin == BOOT_ONENAND) {
		printf(" OneNand\n");
	} else if (OmPin == BOOT_NAND) {
		printf(" NAND\n");
	} else if (OmPin == BOOT_MMCSD) {
		printf(" SDMMC\n");
	} else if (OmPin == BOOT_EMMC) {
		printf(" EMMC4.3\n");
	} else if (OmPin == BOOT_EMMC_4_4) {
		printf(" EMMC4.41\n");
	} else {
		printf(" Please check OM_pin\n");
	}

	return 0;
}

int dram_init(void)
{
	gd->ram_size	= get_ram_size((long *)PHYS_SDRAM_1, PHYS_SDRAM_1_SIZE)
			+ get_ram_size((long *)PHYS_SDRAM_2, PHYS_SDRAM_2_SIZE)
			+ get_ram_size((long *)PHYS_SDRAM_3, PHYS_SDRAM_3_SIZE)
			+ get_ram_size((long *)PHYS_SDRAM_4, PHYS_SDRAM_4_SIZE)
			+ get_ram_size((long *)PHYS_SDRAM_5, PHYS_SDRAM_7_SIZE)
			+ get_ram_size((long *)PHYS_SDRAM_6, PHYS_SDRAM_7_SIZE)
			+ get_ram_size((long *)PHYS_SDRAM_7, PHYS_SDRAM_7_SIZE)
			+ get_ram_size((long *)PHYS_SDRAM_8, PHYS_SDRAM_8_SIZE);

	return 0;
}

void dram_init_banksize(void)
{
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
	gd->bd->bi_dram[6].start = PHYS_SDRAM_7;
	gd->bd->bi_dram[6].size = get_ram_size((long *)PHYS_SDRAM_7,
							PHYS_SDRAM_7_SIZE);
	gd->bd->bi_dram[7].start = PHYS_SDRAM_8;
	gd->bd->bi_dram[7].size = get_ram_size((long *)PHYS_SDRAM_8,
							PHYS_SDRAM_8_SIZE);
}

int board_eth_init(bd_t *bis)
{
	return 0;
}

#ifdef CONFIG_DISPLAY_BOARDINFO
int checkboard(void)
{

	printf("\nBoard: ESPRESSO3472\n");

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

	if (OmPin == BOOT_EMMC_4_4 || OmPin == BOOT_EMMC) {
#ifdef USE_MMC0
		set_mmc_clk(PERIPH_ID_SDMMC0, 1);
		err = s5p_mmc_init(PERIPH_ID_SDMMC0, 8);
#endif
#ifdef USE_MMC2
		set_mmc_clk(PERIPH_ID_SDMMC2, 1);
		err = s5p_mmc_init(PERIPH_ID_SDMMC2, 4);
#endif
	}
	else {
#ifdef USE_MMC2
		set_mmc_clk(PERIPH_ID_SDMMC2, 1);
		err = s5p_mmc_init(PERIPH_ID_SDMMC2, 4);
#endif
#ifdef USE_MMC0
		set_mmc_clk(PERIPH_ID_SDMMC0, 1);
		err = s5p_mmc_init(PERIPH_ID_SDMMC0, 8);
#endif
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

//Evan Tan
unsigned char gIsGoCpFactoryMode = 0;
#define GPE1CON	0x11000140
#define GPE1DAT	0x11000144
#define GPE1PUD	0x11000148

int board_late_init(void)
{
	struct exynos4_power *pmu = (struct exynos4_power *)EXYNOS4_POWER_BASE;
	uint rst_stat;

	rst_stat = readl(&pmu->rst_stat);
	printf("rst_stat : 0x%x\n", rst_stat);

#ifdef CONFIG_RECOVERY_MODE
	u32 second_boot_info = readl(CONFIG_SECONDARY_BOOT_INFORM_BASE);

	if (second_boot_info == 1) {
		printf("###Recovery Mode(SDMMC)###\n");
		writel(0x0, CONFIG_SECONDARY_BOOT_INFORM_BASE);
		setenv("bootcmd", CONFIG_RECOVERYCOMMAND_SDMMC);
	} else if (second_boot_info == 2) {
		printf("###Recovery Mode(USB)###\n");
		writel(0x0, CONFIG_SECONDARY_BOOT_INFORM_BASE);
		setenv("bootcmd", CONFIG_RECOVERYCOMMAND_USB);
	}
#endif

	if ((readl(&pmu->inform4)) == CONFIG_FACTORY_RESET_MODE) {
		writel(0x0, &pmu->inform4);
		setenv("bootcmd", CONFIG_FACTORY_RESET_BOOTCOMMAND);
	}

#ifdef CONFIG_RAMDUMP_MODE
	/*   check reset status for ramdump */
	if ((rst_stat & (WRESET | SYS_WDTRESET | ISP_ARM_WDTRESET))
		|| (readl(CONFIG_RAMDUMP_SCRATCH) == CONFIG_RAMDUMP_MODE)) {
		/*   run fastboot */
		run_command("fastboot", 0);
	}
#endif
	return 0;
}
