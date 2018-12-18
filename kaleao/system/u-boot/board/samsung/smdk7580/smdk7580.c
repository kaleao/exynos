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
#include <asm/arch/smc.h>
#include <asm/arch/tmu.h>
#include "pmic.h"
#include "muic.h"
#include "if_pmic.h"
#include "setup.h"
#include "dfd.h"
#ifdef CONFIG_ETBDUMP_MODE
#include "coresight.h"
#endif

#define DEBOUNCE_DELAY	10000
#define SECOND_BOOT_MODE	0xFEED0002

DECLARE_GLOBAL_DATA_PTR;
unsigned int pmic;
unsigned int second_boot = 0;

int init_ldfws(void);
void enable_hw_batcher(void);

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

static u32 chipid_reverse_value(u32 value, u32 bitcnt)
{
	u32 tmp, ret = 0;
	u32 i;

	for (i = 0; i < bitcnt; i++)
	{
		tmp = (value >> i) & 0x1;
		ret += tmp << ((bitcnt - 1) - i);
	}

	return ret;
}

static void chipid_dec_to_36(u32 in, char *p)
{
	u32 mod;
	u32 i;

	for (i = 4; i >= 1; i--) {
		mod = in % 36;
		in /= 36;
		p[i] = (mod < 10) ? (mod + '0') : (mod-10 + 'A');
	}

	p[0] = 'N';
	p[5] = 0;
}

static void display_wafer_id(u32 id_0)
{
	u32 reverse_id_0 = 0;
	u32 tmp = 0;
	char wafer_id[7] = {0,};

	reverse_id_0 = chipid_reverse_value(id_0, 32);
	tmp = (reverse_id_0 >> 11) & 0x1FFFFF;
	chipid_dec_to_36(tmp, wafer_id);

	printf("Wafer(LOT) ID : %s\n", wafer_id);
}

static void display_chip_id(void)
{
#if defined(CONFIG_DISPLAY_CHIPID)
	s5p_chip_id[0] = readl(EXYNOS7580_PRO_ID + CHIPID0_OFFSET);
	s5p_chip_id[1] = (readl(EXYNOS7580_PRO_ID + CHIPID1_OFFSET) & 0xFFFF);

	printf("\nChip ID : %04x%08x\n", s5p_chip_id[1], s5p_chip_id[0]);
	display_wafer_id(s5p_chip_id[0]);
#endif
}

#ifndef CONFIG_EMULATOR
static void display_pmic_info(void)
{
#if defined(CONFIG_PM)
	unsigned char pmic_id = 0;
	unsigned char rtc_ctrl = 0;
	unsigned char wrstbi_ctrl = 0;
	unsigned char read_vol_mif = 0;
	unsigned char read_vol_atlas = 0;
	unsigned char read_vol_int = 0;
	unsigned char read_vol_g3d = 0;
	unsigned char read_vol_apollo = 0;
	unsigned char read_vol_disp = 0;
	unsigned char read_ldo_dvs1 = 0;
	unsigned char read_ldo_dvs2 = 0;
	unsigned char read_ldo_dvs3 = 0;
	unsigned char read_ldo_dvs4 = 0;

	IIC0_ERead(S2MPU03_ADDR, S2MPU03_PMIC_ID, &pmic_id);
	IIC0_ERead(S2MPU03_ADDR, RTC_BUF, &rtc_ctrl);
	if (pmic_id >= 0x11)
		IIC0_ERead(S2MPU03_ADDR, WRSTBI_CTRL_EVT1, &wrstbi_ctrl);
	else
		IIC0_ERead(S2MPU03_ADDR, WRSTBI_CTRL_EVT0, &wrstbi_ctrl);
	IIC0_ERead(S2MPU03_ADDR, BUCK1_OUT, &read_vol_mif);
	IIC0_ERead(S2MPU03_ADDR, BUCK2_OUT, &read_vol_atlas);
	IIC0_ERead(S2MPU03_ADDR, BUCK3_OUT, &read_vol_apollo);
	IIC0_ERead(S2MPU03_ADDR, BUCK4_OUT, &read_vol_int);
	IIC0_ERead(S2MPU03_ADDR, BUCK5_OUT, &read_vol_disp);
	IIC0_ERead(S2MPU03_ADDR, BUCK6_OUT, &read_vol_g3d);
	IIC0_ERead(S2MPU03_ADDR, LDO_DVS1, &read_ldo_dvs1);
	IIC0_ERead(S2MPU03_ADDR, LDO_DVS2, &read_ldo_dvs2);
	IIC0_ERead(S2MPU03_ADDR, LDO_DVS3, &read_ldo_dvs3);
	IIC0_ERead(S2MPU03_ADDR, LDO_DVS4, &read_ldo_dvs4);

	printf("\nPMIC: S2MPU03(REV%x)\n", pmic_id);

	if (pmic_id >= 0x11) {
		display_pmic_rtc_time();
		printf("MIF: %dmV\t", RD_BUCK_VOLT_EVT1((unsigned int)read_vol_mif));
		printf("ATLAS: %dmV\t", RD_BUCK_VOLT_EVT1((unsigned int)read_vol_atlas));
		printf("INT: %dmV\t\n", RD_BUCK_VOLT_EVT1((unsigned int)read_vol_int));
		printf("G3D: %dmV\t", RD_BUCK_VOLT_EVT1((unsigned int)read_vol_g3d));
		printf("APOLLO: %dmV\t", RD_BUCK_VOLT_EVT1((unsigned int)read_vol_apollo));
		printf("DISP: %dmV\t\n", RD_BUCK_VOLT_EVT1((unsigned int)read_vol_disp));
	} else {
		printf("MIF: %dmV\t", RD_BUCK_VOLT((unsigned int)read_vol_mif));
		printf("ATLAS: %dmV\t", RD_BUCK_VOLT((unsigned int)read_vol_atlas));
		printf("INT: %dmV\t\n", RD_BUCK_VOLT((unsigned int)read_vol_int));
		printf("G3D: %dmV\t", RD_BUCK_VOLT((unsigned int)read_vol_g3d));
		printf("APOLLO: %dmV\t", RD_BUCK_VOLT((unsigned int)read_vol_apollo));
		printf("DISP: %dmV\t\n", RD_BUCK_VOLT((unsigned int)read_vol_disp));
	}

	/* print rtc_buf & wrstbi register value */
	printf("RTC_BUF: 0x%x, WRSTBI: 0x%x\n", rtc_ctrl, wrstbi_ctrl);
	/* print ldo register value */
	printf("LDO_DVS1: 0x%x, LDO_DVS2: 0x%x\n", read_ldo_dvs1, read_ldo_dvs2);
	printf("LDO_DVS3: 0x%x, LDO_DVS4: 0x%x\n", read_ldo_dvs3, read_ldo_dvs4);

#ifdef CONFIG_MACH_ESPRESSO7580
	/* enable LDO18 : VCC_2.8V_PERI */
	IIC0_ERead(S2MPU03_ADDR, LDO18_CTRL, &read_ldo_dvs4);
	read_ldo_dvs4 |= OUTPUT_PWREN_ON;
	IIC0_EWrite(S2MPU03_ADDR, LDO18_CTRL, read_ldo_dvs4);

	/* enable LDO26 : UFS */
	IIC0_ERead(S2MPU03_ADDR, LDO26_CTRL, &read_ldo_dvs4);
	read_ldo_dvs4 |= OUTPUT_PWREN_ON;
	IIC0_EWrite(S2MPU03_ADDR, LDO26_CTRL, read_ldo_dvs4);
#endif
#ifdef CONFIG_EXYNOS_LCD_ON_UBOOT
	/* power control for LCD */
	/* enable VDD_MIPI_1V2(LDO22) */
	IIC0_EWrite(S2MPU03_ADDR, 0x38, 0xEC);
	/* enable VCC_1.8V_Peri(LDO15) */
	IIC0_EWrite(S2MPU03_ADDR, 0x42, 0xEC);
	/* enable VCC_1.8V_Peri(LDO18) */
	IIC0_EWrite(S2MPU03_ADDR, 0x45, 0xEC);
	/* enable VCC_3.3V_Peri(LDO19) */
	IIC0_EWrite(S2MPU03_ADDR, 0x46, 0xEC);
	/* enable VCC_2.8V_Peri(LDO20) */
	IIC0_EWrite(S2MPU03_ADDR, 0x47, 0xEC);
#endif

#ifdef CONFIG_MACH_BTYPE7580
	/* power control for BTYPE7580 SD/MMC */
	/* enable LDO25 */
	IIC0_EWrite(S2MPU03_ADDR, LDO25_CTRL, &read_ldo_dvs1);
	read_ldo_dvs1 |= OUTPUT_PWREN_ON;
	IIC0_EWrite(S2MPU03_ADDR, LDO25_CTRL, read_ldo_dvs1);

	/* enable MAX77838 PMIC for display */
	pmic_display_init();

	IIC_DISP_ERead(MAX77838_R_ADDR, MAX77838_REG_EN, &read_ldo_dvs4);
	printf("MAX77838 REG_EN: 0x%x\n", read_ldo_dvs4);
#endif

#ifndef CONFIG_MACH_BTYPE7580
	/* setup PMIC lm3560 */
	IIC2_ESetport();

	/* set GPC0[2], GPC0[3] output low */
	*(unsigned int *)0x13470020 &= ~0xFF00;
	*(unsigned int *)0x13470020 |= 0x1100;
	*(unsigned int *)0x13470024 &= ~0xC;

	/* set GPF5[3] output high */
	*(unsigned int *)0x148700C0 &= ~0xF000;
	*(unsigned int *)0x148700C0 |= 0x1000;
	*(unsigned int *)0x148700C4 |= 0x8;

	IIC2_EWrite(LM3560_ADDR, 0x10, 0x18);
	IIC2_EWrite(LM3560_ADDR, 0xA0, 0x55);
	IIC2_EWrite(LM3560_ADDR, 0xB0, 0xDD);
	IIC2_EWrite(LM3560_ADDR, 0xC0, 0x6F);
	IIC2_EWrite(LM3560_ADDR, 0xE0, 0xEC);
	IIC2_EWrite(LM3560_ADDR, 0xF0, 0x00);

	IIC2_ERead(LM3560_ADDR, 0xE0, &read_ldo_dvs4);
	printf("0xEF: 0x%x\n", read_ldo_dvs4);
	IIC2_ERead(LM3560_ADDR, 0xC0, &read_ldo_dvs4);
	printf("0xFF: 0x%x\n", read_ldo_dvs4);
	IIC2_ERead(LM3560_ADDR, 0x11, &read_ldo_dvs4);
	printf("0x78: 0x%x\n", read_ldo_dvs4);
#endif
#endif
}
#endif

#ifndef CONFIG_EMULATOR
void display_pmic_rtc_time(void)
{
       int i;
       u8 tmp;
       u8 time[NR_PMIC_RTC_CNT_REGS];

       IIC0_ERead(S2MPU03_RTC, RTC_UPDATE, &tmp);
       tmp |= 0x1;
       IIC0_EWrite(S2MPU03_RTC, RTC_UPDATE, tmp);
       udelay(40);

       for (i = 0; i < NR_PMIC_RTC_CNT_REGS; i++)
               IIC0_ERead(S2MPU03_RTC, (RTC_SEC + i), &time[i]);

       printf("RTC TIME: %d-%02d-%02d %02d:%02d:%02d(0x%02x)%s\n",
                       time[PMIC_RTC_YEAR] + 2000, time[PMIC_RTC_MONTH],
                       time[PMIC_RTC_DATE], time[PMIC_RTC_HOUR] & 0x1f, time[PMIC_RTC_MIN],
                       time[PMIC_RTC_SEC], time[PMIC_RTC_WEEK],
                       time[PMIC_RTC_HOUR] & (1 << 6) ? "PM" : "AM");
}
#endif

static void display_boot_device_info(void)
{
	struct exynos7580_power *pmu = (struct exynos7580_power *)EXYNOS7580_POWER_BASE;
	int OmPin;

	OmPin = readl(&pmu->inform3);

	printf("\nChecking Boot Mode ...");

	if (OmPin == BOOT_MMCSD) {
		printf(" SDMMC\n");
	} else if (OmPin == BOOT_EMMC) {
		printf(" EMMC\n");
	} else if (OmPin == BOOT_EMMC_4_4) {
		printf(" EMMC\n");
	} else if (OmPin == BOOT_UFS) {
		printf(" UFS\n");
	} else {
		printf(" Please check OM_pin\n");
	}
}

#ifndef CONFIG_EMULATOR
static void display_tmu_info(void)
{
	unsigned int tmp, i;
	unsigned int fuse_25[4] = {0, };
	unsigned int tmu[4] = {0, };
	unsigned int trim_info[4] = {EXYNOS7580_TMU_TRIMINFO_0_REG,
					EXYNOS7580_TMU_TRIMINFO_1_REG,
					EXYNOS7580_TMU_TRIMINFO_2_REG,
					EXYNOS7580_TMU_TRIMINFO_3_REG,};

	unsigned int tmu_ctrl[4] = {EXYNOS7580_TMU_CTRL_0_REG,
					EXYNOS7580_TMU_CTRL_1_REG,
					EXYNOS7580_TMU_CTRL_2_REG,
					EXYNOS7580_TMU_CTRL_3_REG,};

	unsigned int cur_temp[4] = {EXYNOS7580_TMU_CURTEMP_0_REG,
					EXYNOS7580_TMU_CURTEMP_1_REG,
					EXYNOS7580_TMU_CURTEMP_2_REG,
					EXYNOS7580_TMU_CURTEMP_3_REG,};

	unsigned int tmu_status[4] = {EXYNOS7580_TMU_STATUS_0_REG,
					EXYNOS7580_TMU_STATUS_1_REG,
					EXYNOS7580_TMU_STATUS_2_REG,
					EXYNOS7580_TMU_STATUS_3_REG,};

	int timeout = 20000;

	for (i = 0; i < 4; i++) {
		/* read fuse of each sensor */
		timeout = 20000;
		while (1) {
			if (((readl(tmu_status[i]) >> 8) & 0x3) == 0)
				break;

			timeout --;
			if (!timeout) {
				printf("%s: timeout TMU sensing probe\n", __func__);
				break;
			}
			udelay(2);
		}

		fuse_25[i] = readl(trim_info[i]) & 0x1ff;

		tmp = readl(tmu_ctrl[i]) & 0xe07f00fe ;

		/* if APOLLO TMU */
		if (i == 1)
			tmp |= (((16 << 24) | (8 << 8)) & ~0xe07f00fe);
		else
			tmp |= (((17 << 24) | (9 << 8)) & ~0xe07f00fe);

		writel(tmp, tmu_ctrl[i]);

		tmp |= 0x1;
		writel(tmp, tmu_ctrl[i]);
		tmp &= 0xfffffffe;
		writel(tmp, tmu_ctrl[i]);

		timeout = 20000;
		while (1) {
			if (readl(tmu_status[i]) & 0x1)
				break;

			timeout--;
			if (!timeout) {
				printf("%s: timeout TMU busy\n", __func__);
				break;
			}
			udelay(2);
		};

		tmp |= 0x1;
		writel(tmp, tmu_ctrl[i]);
	}

	mdelay(2);

	/* read current temp */
	for (i = 0; i < 4; i++) {
		tmp = readl(cur_temp[i]) & 0x1ff;
		tmu[i] = tmp - fuse_25[i] + 25;
	}
	printf("[TMU] %d, %d, %d, %d\n",
		tmu[0], tmu[1], tmu[2], tmu[3]);
}
#endif

static void display_trip_info(void)
{
	struct exynos7580_power *pmu = (struct exynos7580_power *)EXYNOS7580_POWER_BASE;
	unsigned int hw_trip, sw_trip;

	hw_trip = readl(&pmu->ps_hold_hw_trip) & 0x1;
	sw_trip = readl(&pmu->ps_hold_sw_trip) & 0x1;

	if (hw_trip && sw_trip) {
		printf("\nSW/HW TRIP Detected.\n");
		writel(0x0, &pmu->ps_hold_hw_trip);
		writel(0x0, &pmu->ps_hold_sw_trip);
	} else if (hw_trip) {
		printf("\nHW TRIP Detected.\n");
		writel(0x0, &pmu->ps_hold_hw_trip);
	} else if (sw_trip) {
		printf("\nSW TRIP Detected.\n");
		writel(0x0, &pmu->ps_hold_sw_trip);
	}
}

#ifndef CONFIG_EMULATOR
void set_asv_pmic(void)
{
	Get_ASV_Group();
	pmic_asv_init();
}
#endif

static unsigned int device(unsigned int boot_dev)
{
	switch(boot_dev) {
	case BOOT_MMCSD:
	case BOOT_SEC_DEV:
		boot_dev = SDMMC_CH2;
		break;
	case BOOT_EMMC_4_4:
		boot_dev = EMMC;
		break;
	case BOOT_USB:
		boot_dev = USB;
		break;
	case BOOT_UFS:
		boot_dev = UFS;
		break;
	default:
		while(1);
	}
	return boot_dev;
}

/* This function should be called before running eMMC init */
static unsigned long load_secure_payload(void)
{
	unsigned int om_status = readl(EXYNOS7580_POWER_BASE + INFORM3_OFFSET);

	return load_sp_image(device(om_status));
}

int board_init(void)
{
	unsigned long ret = 0;

#ifndef CONFIG_EMULATOR
	if(*(unsigned int *)CONFIG_SYS_SDRAM_BASE == 0xabcdef)
		set_asv_pmic();
#endif

	display_bl1_version();

	display_chip_id();

#ifndef CONFIG_EMULATOR
	display_pmic_info();

	display_tmu_info();
#endif

#ifdef CONFIG_MACH_SMDK7580
	pmic_audio_gps_enable();
#endif

#ifdef CONFIG_MUIC
#ifdef CONFIG_MACH_SMDK7580
       /* Set MUIC for USB */
	muic_init_to_usb();
#endif
#endif
	pmic_lcd_tsp();

	display_boot_device_info();

	display_trip_info();

	enable_hw_batcher();

	/* Work Around for I/F PMIC EVT0 and EVT1 */
	if_pmic_init();

	if (*(unsigned int *)CONFIG_SYS_SDRAM_BASE != 0xabcdef) {
		printf("Running on DRAM by TRACE32: skip load_secure_payload()\n");
	} else {
		if (find_second_boot() != SECOND_BOOT_MODE) {
			ret = load_secure_payload();
			if (ret)
				/*
				 * -1 : Signature check fail
				 * -2 : Anti-rollback check fail
				 */
				printf("Fail to load Secure Payload!! [ret = %ld]\n", ret);
		} else {
			second_boot = 1;
		}
	}

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
	return 0;
}

#ifdef CONFIG_DISPLAY_BOARDINFO
int checkboard(void)
{
	printf("\nBoard: " CONFIG_BOARD_NAME "\n");

	return 0;
}
#endif

#ifdef CONFIG_GENERIC_MMC
int board_mmc_init(bd_t *bis)
{
	struct exynos7580_power *pmu = (struct exynos7580_power *)EXYNOS7580_POWER_BASE;
	int err, OmPin;
	int mmc_ratio = 3;

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
		set_mmc_clk(PERIPH_ID_SDMMC0, mmc_ratio);

		err = s5p_mmc_init(PERIPH_ID_SDMMC0, 8);
#endif
#if defined(USE_MMC2)
		set_mmc_clk(PERIPH_ID_SDMMC2, mmc_ratio);

		err = s5p_mmc_init(PERIPH_ID_SDMMC2, 4);
#endif
		break;
	default:
#if defined(USE_MMC2)
		set_mmc_clk(PERIPH_ID_SDMMC2, mmc_ratio);

		err = s5p_mmc_init(PERIPH_ID_SDMMC2, 4);
#endif
#if defined(USE_MMC0)
		set_mmc_clk(PERIPH_ID_SDMMC0, mmc_ratio);

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

static void configure_auto_boot(unsigned int om_status)
{
	char *commandline = getenv("bootcmd");

	if(*(unsigned int *)CONFIG_SYS_SDRAM_BASE != 0xabcdef) {
		printf("Running on DRAM by TRACE32: execute auto fusing\n");
		switch(om_status) {
		case BOOT_MMCSD:
		case BOOT_SEC_DEV:
			setenv("bootcmd", CONFIG_FUSING_COMMAND_SDMMC);
			break;
		case BOOT_EMMC_4_4:
			setenv("bootcmd", CONFIG_FUSING_COMMAND_EMMC);
			break;
		case BOOT_UFS:
			setenv("bootcmd", CONFIG_FUSING_COMMAND_UFS);
			break;
		default:
			break;
		}
	} else {
		if ((strncmp(commandline, CONFIG_FUSING_COMMAND_SDMMC, strlen(commandline)) == 0)
			|| (strncmp(commandline, CONFIG_FUSING_COMMAND_EMMC, strlen(commandline)) == 0)
			|| (strncmp(commandline, CONFIG_FUSING_COMMAND_UFS, strlen(commandline)) == 0)) {
			setenv("bootcmd", CONFIG_BOOTCOMMAND);
			saveenv();
		}
	}
}

#ifdef CONFIG_OF_LIBFDT
static void configure_dtb(void)
{
	char str[256];
	char *rootfs_len = getenv("rootfslen");
	ulong rd_len = simple_strtoul(rootfs_len, NULL, 16);
	unsigned int total = 8 * SDRAM_BANK_SIZE;

	sprintf(str, "fdt set /memory reg <0x0 0x%x 0x0 0x%x>", CONFIG_SYS_SDRAM_BASE, total);
	run_command(str, 0);
	printf("[FDT] : %s\n", str);
	sprintf(str, "fdt set /chosen linux,initrd-start <0x%x>", CONFIG_SYS_ROOTFS_BASE);
	run_command(str, 0);
	printf("[FDT] : %s\n", str);
	sprintf(str, "fdt set /chosen linux,initrd-end <0x%x>", CONFIG_SYS_ROOTFS_BASE + rd_len);
	run_command(str, 0);
	printf("[FDT] : %s\n", str);
}

void prepare_dtb(void)
{
	unsigned int om_status = readl(EXYNOS7580_POWER_BASE + INFORM3_OFFSET);

	if(*(unsigned int *)CONFIG_SYS_SDRAM_BASE == 0xabcdef) {
		switch(om_status) {
		case BOOT_MMCSD:
		case BOOT_SEC_DEV:
			run_command(CONFIG_FDT_LOAD_COMMAND_SDMMC, 0);
			configure_dtb();
			run_command(CONFIG_FDT_SAVE_COMMAND_SDMMC, 0);
			break;
		case BOOT_EMMC_4_4:
			run_command(CONFIG_FDT_LOAD_COMMAND_EMMC, 0);
			configure_dtb();
			run_command(CONFIG_FDT_SAVE_COMMAND_EMMC, 0);
			break;
		case BOOT_UFS:
			run_command(CONFIG_FDT_LOAD_COMMAND_UFS, 0);
			configure_dtb();
			run_command(CONFIG_FDT_SAVE_COMMAND_UFS, 0);
			break;
		default:
			break;
		}
	}
}
#endif

int board_late_init(void)
{
	struct exynos7580_power *pmu = (struct exynos7580_power *)EXYNOS7580_POWER_BASE;
	unsigned int om_status = readl(EXYNOS7580_POWER_BASE + INFORM3_OFFSET);
	unsigned int rst_stat = 0;
	unsigned char read_int1;
	unsigned char read_int2;
	unsigned char read_wtsr_smpl;
	unsigned char read_pwronsrc;

	rst_stat = readl(&pmu->rst_stat);
	printf("rst_stat : 0x%x\n", rst_stat);

	/* init ldfw */
	if (*(unsigned int *)CONFIG_SYS_SDRAM_BASE == 0xabcdef) {
		if (!init_ldfws())
			printf("ldfw: init done successfully.\n");
		else
			printf("ldfw: init failed.\n");
	}

#ifdef CONFIG_RAMDUMP_MODE
	/* read PMIC */
	IIC0_ERead(S2MPU03_ADDR, S2MPU03_INT1, &read_int1);
	IIC0_ERead(S2MPU03_ADDR, S2MPU03_INT2, &read_int2);
	IIC0_ERead(S2MPU03_ADDR, PWRONSRC, &read_pwronsrc);
	/* to clear OFFSRC */
	IIC0_EWrite(S2MPU03_ADDR, OFFSRC, 0x0);
	/* read RTC */
	IIC0_ERead(S2MPU03_RTC, RTC_WTSR_SMPL, &read_wtsr_smpl);

	dfd_display_reboot_reason();
	dfd_display_core_stat();

	if ((read_pwronsrc & (1 << 7)) && (read_int2 & (1 << 5))
	/* WTSR detect condition - WTSR_ON && WTSR_INT && ! MRB_INT */
		&& !(read_int1 & (1 << 7))) {
		printf("S2MPU03_INT1: 0x%x\n", read_int1);
		printf("S2MPU03_INT2: 0x%x\n", read_int2);
		printf("PWRONSRC: 0x%x\n", read_pwronsrc);
		printf("WTSR detected\n");
		run_command("fastboot", 0);
	} else if ((read_pwronsrc & (1 << 6)) && (read_int2 & (1 << 3))
	/* SMPL detect condition - SMPL_ON && SMPL_INT && SMPL_EN */
                && (read_wtsr_smpl & (1 << 7))) {
		printf("S2MPU03_INT2: 0x%x\n", read_int2);
		printf("PWRONSRC: 0x%x\n", read_pwronsrc);
		printf("S2MPU03_RTC_WTSR_SMPL: 0x%x\n", read_wtsr_smpl);
		printf("SMPL detected\n");
                run_command("fastboot", 0);
	} else if ((rst_stat & (WRESET | APL_WDTRESET | CPU_WDTRESET))) {
	/* check reset status for ramdump */
		dfd_run_dump_gpr(EXYNOS7580_POWER_BASE);
#ifdef CONFIG_ETBDUMP_MODE
		etb_dump();
#endif
		run_command("fastboot", 0);
	} else if ((readl(&pmu->sysip_dat0) == CONFIG_RAMDUMP_MODE)
		|| (readl(CONFIG_RAMDUMP_SCRATCH) == CONFIG_RAMDUMP_MODE)) {
#ifdef CONFIG_ETBDUMP_MODE
		etb_dump();
#endif
		run_command("fastboot", 0);
	}
	/* Set enable to dump_grp function */
	dfd_set_dump_gpr(EXYNOS7580_POWER_BASE, 1);
#endif

#ifdef CONFIG_RECOVERY_MODE
	if (second_boot == 1)
		printf("###Recovery Mode###\n");
#endif

	if ((readl(&pmu->sysip_dat0)) == CONFIG_FACTORY_RESET_MODE) {
		writel(0x0, &pmu->sysip_dat0);
		setenv("bootcmd", CONFIG_FACTORY_RESET_BOOTCOMMAND);
	}

	configure_auto_boot(om_status);

	return 0;
}

unsigned int get_board_rev(void)
{
	unsigned int rev = 0;

	return rev;
}

/* enable H/W semaphore operation mode for Batcher */
void enable_hw_batcher(void)
{
	writel(0x44030444, 0x1046005C);
}
