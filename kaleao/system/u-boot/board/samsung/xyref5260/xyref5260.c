/*
 * Copyright (C) 2012 Samsung Electronics
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
	char bl1_version[9] = {0};

	/* display BL1 version */
#if defined(CONFIG_TRUSTZONE_ENABLE)
	printf("\nTrustZone Enabled BSP");
	strncpy(&bl1_version[0], (char *)CONFIG_BL1_VERSION_INFORM, 8);
	printf("\nBL1 version: %s\n", &bl1_version[0]);
#endif
}

static void display_chip_id(void)
{
#if defined(CONFIG_DISPLAY_CHIPID)
	s5p_chip_id[0] = readl(EXYNOS5260_PRO_ID + CHIPID0_OFFSET);
	s5p_chip_id[1] = (readl(EXYNOS5260_PRO_ID + CHIPID1_OFFSET) & 0xFFFF);

	printf("\nChip ID : %04x%08x\n", s5p_chip_id[1], s5p_chip_id[0]);
#endif
}

static void display_pmic_info(void)
{
#if defined(CONFIG_PM)
	unsigned char pmic_id = 0;
	unsigned char rtc_ctrl = 0;
	unsigned char wrstbi_ctrl = 0;
	unsigned char read_vol_mif = 0;
	unsigned char read_vol_egl = 0;
	unsigned char read_vol_int = 0;
	unsigned char read_vol_g3d = 0;
	unsigned char read_vol_kfc = 0;
	unsigned char read_buck9 = 0;
	unsigned char read_int1 = 0;
	unsigned char read_int2 = 0;
	unsigned char read_int3 = 0;
	unsigned char read_status1 = 0;
	unsigned char read_status2 = 0;
	unsigned char read_pwronsrc = 0;
	unsigned char read_offsrc = 0;
	unsigned char read_wtsr_smpl = 0;

	IIC0_ERead(S2MPA01_ADDR, PMIC_ID, &pmic_id);
	IIC0_ERead(S2MPA01_ADDR, RTC_BUF, &rtc_ctrl);
	IIC0_ERead(S2MPA01_ADDR, WRSTBI_CTRL, &wrstbi_ctrl);
	IIC0_ERead(S2MPA01_ADDR, BUCK1_OUT, &read_vol_mif);
	IIC0_ERead(S2MPA01_ADDR, BUCK2_OUT, &read_vol_egl);
	IIC0_ERead(S2MPA01_ADDR, BUCK3_OUT, &read_vol_int);
	IIC0_ERead(S2MPA01_ADDR, BUCK4_OUT, &read_vol_g3d);
	IIC0_ERead(S2MPA01_ADDR, BUCK6_OUT, &read_vol_kfc);
	IIC0_ERead(S2MPA01_ADDR, BUCK9_OUT, &read_buck9);
	IIC0_ERead(S2MPA01_ADDR, BUCK9_OUT, &read_buck9);

	IIC0_ERead(S2MPA01_ADDR, S2MPA01_INT1, &read_int1);
	IIC0_ERead(S2MPA01_ADDR, S2MPA01_INT2, &read_int2);
	IIC0_ERead(S2MPA01_ADDR, S2MPA01_INT3, &read_int3);
	IIC0_ERead(S2MPA01_ADDR, S2MPA01_STATUS1, &read_status1);
	IIC0_ERead(S2MPA01_ADDR, S2MPA01_STATUS2, &read_status2);
	IIC0_ERead(S2MPA01_ADDR, PWRONSRC, &read_pwronsrc);
	IIC0_ERead(S2MPA01_ADDR, OFFSRC, &read_offsrc);
	IIC0_EWrite(S2MPA01_ADDR, OFFSRC, 0xff); // to clear OFFSRC
	IIC0_ERead(S2MPA01_RTC, RTC_WTSR_SMPL, &read_wtsr_smpl);

	printf("\nPMIC: S2MPA01(REV%x)\n", pmic_id);
	printf("MIF: %dmV\t", RD_BUCK_VOLT((unsigned int)read_vol_mif));
	printf("EGL: %dmV\t", RD_BUCK_VOLT((unsigned int)read_vol_egl));
	printf("INT: %dmV\t", RD_BUCK_VOLT((unsigned int)read_vol_int));
	printf("G3D: %dmV\t", RD_BUCK_VOLT((unsigned int)read_vol_g3d));
	printf("KFC: %dmV\t\n", RD_BUCK_VOLT((unsigned int)read_vol_kfc));
	/* print rtc_buf & wrstbi register value */
	printf("RTC_BUF: 0x%x, WRSTBI: 0x%x\n", rtc_ctrl, wrstbi_ctrl);
	printf("BUCK9: 0x%x\n", read_buck9);

	printf("S2MPA01_INT1: 0x%x\n", read_int1);
	printf("S2MPA01_INT2: 0x%x\n", read_int2);
	printf("S2MPA01_INT3: 0x%x\n", read_int3);
	printf("S2MPA01_STATUS1: 0x%x\n", read_status1);
	printf("S2MPA01_STATUS2: 0x%x\n", read_status2);
	printf("PWRONSRC: 0x%x\n", read_pwronsrc);
	printf("OFFSRC: 0x%x\n", read_offsrc);
	printf("S2MPA01_RTC: 0x%x\n", read_wtsr_smpl);

	if (((read_pwronsrc & 0x80) >> 7)  && ((read_int2 & 0x20) >> 5) &&
		((read_int1 & 0x80) >> 7))
		printf("Warning: WTSR detected!!!!\n");
	else
		printf("WTSR not detected\n");

	if (((read_pwronsrc & 0x40) >> 6)  && ((read_int3 & 0x08) >> 3) &&
		((read_wtsr_smpl & 0x80) >> 7))
		printf("Warning: SMPL detected!!!!\n");
	else
		printf("SMPL not detected\n");
#endif
}

static void display_boot_device_info(void)
{
	struct exynos5260_power *pmu = (struct exynos5260_power *)EXYNOS5260_POWER_BASE;
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
	display_bl1_version();

	display_chip_id();

	display_pmic_info();

	display_boot_device_info();

	gd->bd->bi_boot_params = (PHYS_SDRAM_1 + 0x100UL);

	return 0;
}

int dram_init(void)
{
	gd->ram_size	= get_ram_size((long *)PHYS_SDRAM_1, PHYS_SDRAM_1_SIZE)
			+ get_ram_size((long *)PHYS_SDRAM_2, PHYS_SDRAM_2_SIZE)
			+ get_ram_size((long *)PHYS_SDRAM_3, PHYS_SDRAM_3_SIZE)
			+ get_ram_size((long *)PHYS_SDRAM_4, PHYS_SDRAM_4_SIZE)
			+ get_ram_size((long *)PHYS_SDRAM_5, PHYS_SDRAM_5_SIZE)
			+ get_ram_size((long *)PHYS_SDRAM_6, PHYS_SDRAM_6_SIZE)
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
	printf("\nBoard: XYREF5260\n");

	return 0;
}
#endif

#ifdef CONFIG_GENERIC_MMC
int board_mmc_init(bd_t *bis)
{
	struct exynos5260_power *pmu = (struct exynos5260_power *)EXYNOS5260_POWER_BASE;
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
		set_mmc_clk(PERIPH_ID_SDMMC0, 1);

		err = s5p_mmc_init(PERIPH_ID_SDMMC0, 8);
#endif
#if defined(USE_MMC2)
		set_mmc_clk(PERIPH_ID_SDMMC2, 1);

		err = s5p_mmc_init(PERIPH_ID_SDMMC2, 4);
#endif
		break;
	default:
#if defined(USE_MMC2)
		set_mmc_clk(PERIPH_ID_SDMMC2, 1);

		err = s5p_mmc_init(PERIPH_ID_SDMMC2, 4);
#endif
#if defined(USE_MMC0)
		set_mmc_clk(PERIPH_ID_SDMMC0, 1);

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
	struct exynos5260_power *pmu = (struct exynos5260_power *)EXYNOS5260_POWER_BASE;
	unsigned int rst_stat = 0;

	rst_stat = readl(&pmu->rst_stat);
	printf("rst_stat : 0x%x\n", rst_stat);

#ifdef CONFIG_RAMDUMP_MODE
	/* check reset status for ramdump */
	if ((rst_stat & (WRESET | CORTEXA7_WDTRESET | CORTEXA15_WDTRESET))
		|| (readl(&pmu->sysip_dat0) == CONFIG_RAMDUMP_MODE)
		|| (readl(CONFIG_RAMDUMP_SCRATCH) == CONFIG_RAMDUMP_MODE)) {
		/* run fastboot */
		run_command("fastboot", 0);
	}
#endif
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

	if ((readl(&pmu->sysip_dat0)) == CONFIG_FACTORY_RESET_MODE) {
		writel(0x0, &pmu->sysip_dat0);
		setenv("bootcmd", CONFIG_FACTORY_RESET_BOOTCOMMAND);
	}
	return 0;
}

unsigned int get_board_rev(void)
{
	unsigned int rev = 0;

	return rev;
}
