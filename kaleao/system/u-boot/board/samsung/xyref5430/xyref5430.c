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
#include <asm/arch/pmu.h>
#include <asm/arch/tmu.h>
#include <asm/arch/sromc.h>
#include <asm/arch/sysreg.h>
#include <asm/arch/smc.h>
#include "pmic.h"
#include "pmic_lm3560.h"
#include "setup.h"
#if defined(CONFIG_CMD_SCSI) && defined(CONFIG_SCSI_UFS)
#include <scsi.h>
#include <ufs.h>
#endif

#define DEBOUNCE_DELAY	10000
#define SECOND_BOOT_MODE       0xFEED0002

#define Outp32(addr, data)	(*(volatile u32 *)(addr) = (data))
#define Inp32(addr) (*(volatile u32 *)(addr))
#define GetBits(uAddr, uBaseBit, uMaskValue) \
    ((Inp32(uAddr)>>(uBaseBit))&(uMaskValue))
#ifdef CONFIG_CPU_EXYNOS5433_EVT1
#define GetPOPType() (0)
#else
#define GetPOPType() (GetBits(0x10000004, 4, 0x3))
#endif

DECLARE_GLOBAL_DATA_PTR;
unsigned int pmic;

#ifdef CONFIG_CPU_EXYNOS5433_EVT1
int otp_tmu_parse(u32 ulOffset);
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
	s5p_chip_id[0] = readl(EXYNOS5430_PRO_ID + CHIPID0_OFFSET);
	s5p_chip_id[1] = (readl(EXYNOS5430_PRO_ID + CHIPID1_OFFSET) & 0xFFFF);

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
	unsigned char read_vol_kfc = 0;
	unsigned char read_vol_int = 0;
	unsigned char read_vol_cam0_disp = 0;
	unsigned char read_vol_g3d = 0;
	unsigned char ldo6 = 0, ldo7 = 0;
	unsigned char ldo19 = 0, ldo20 = 0;
	unsigned char ldo27 = 0, ldo28 = 0;
	unsigned char ldo_dvs1 = 0, ldo_dvs2 = 0, ldo_dvs3 = 0;
	unsigned char enable_reg = 0;

	/* Read PMIC register value */
	IIC0_ERead(S2MPS13_ADDR, PMIC_ID, &pmic_id);
	IIC0_ERead(S2MPS13_ADDR, RTC_BUF, &rtc_ctrl);
	IIC0_ERead(S2MPS13_ADDR, WRSTBI_CTRL, &wrstbi_ctrl);
	IIC0_ERead(S2MPS13_ADDR, LDO_DVS1, &ldo_dvs1);
	IIC0_ERead(S2MPS13_ADDR, LDO_DVS2, &ldo_dvs2);
	IIC0_ERead(S2MPS13_ADDR, LDO_DVS3, &ldo_dvs3);
	IIC0_ERead(S2MPS13_ADDR, LDO6_CTRL, &ldo6);
	IIC0_ERead(S2MPS13_ADDR, LDO7_CTRL, &ldo7);
	IIC0_ERead(S2MPS13_ADDR, LDO19_CTRL, &ldo19);
	IIC0_ERead(S2MPS13_ADDR, LDO20_CTRL, &ldo20);
	IIC0_ERead(S2MPS13_ADDR, LDO27_CTRL, &ldo27);
	IIC0_ERead(S2MPS13_ADDR, LDO28_CTRL, &ldo28);
	IIC0_ERead(S2MPS13_ADDR, BUCK1_OUT, &read_vol_mif);
	IIC0_ERead(S2MPS13_ADDR, BUCK2_OUT, &read_vol_egl);
	IIC0_ERead(S2MPS13_ADDR, BUCK3_OUT, &read_vol_kfc);
	IIC0_ERead(S2MPS13_ADDR, BUCK4_OUT, &read_vol_int);
	IIC0_ERead(S2MPS13_ADDR, BUCK5_OUT, &read_vol_cam0_disp);
	IIC0_ERead(S2MPS13_ADDR, BUCK6_OUT, &read_vol_g3d);

#ifdef CONFIG_CPU_EXYNOS5433
#ifndef CONFIG_CPU_EXYNOS5433_EVT1
	printf("\nIDS_EGL: %x\n", Inp32(0x105d0000) & 0xff);
#endif
#endif
	printf("\nPMIC: S2MPS13(REV%x)\n", pmic_id);
	display_pmic_rtc_time();
	/* print BUCK voltage */
	if (pmic_id == REV_00) {
		read_vol_mif += VDD_REG_VALUE_BASE;
		read_vol_egl += VDD_REG_VALUE_BASE;
		read_vol_kfc += VDD_REG_VALUE_BASE;
		read_vol_int += VDD_REG_VALUE_BASE;
		read_vol_g3d += VDD_REG_VALUE_BASE;
	}
	printf("MIF: %dmV\t", RD_BUCK_VOLT((unsigned int)read_vol_mif));
#ifdef CONFIG_CPU_EXYNOS5433_EVT1
	printf("ATLAS: %dmV\t", RD_BUCK_VOLT((unsigned int)read_vol_egl));
	printf("APOLLO: %dmV\t\n", RD_BUCK_VOLT((unsigned int)read_vol_kfc));
#else
	printf("EGL: %dmV\t", RD_BUCK_VOLT((unsigned int)read_vol_egl));
	printf("KFC: %dmV\t\n", RD_BUCK_VOLT((unsigned int)read_vol_kfc));
#endif
	printf("INT: %dmV\t", RD_BUCK_VOLT((unsigned int)read_vol_int));
#ifdef CONFIG_CPU_EXYNOS5433
	printf("CAM0_DISP: %dmV\t", RD_BUCK_VOLT((unsigned int)read_vol_cam0_disp));
#endif
	printf("G3D: %dmV\t\n", RD_BUCK_VOLT((unsigned int)read_vol_g3d));
	/* print rtc_buf & wrstbi register value */
	printf("RTC_BUF: 0x%x, WRSTBI: 0x%x\n", rtc_ctrl, wrstbi_ctrl);
	/* print ldo register value */
	printf("LDO6: 0x%x, LDO7: 0x%x\n", ldo6, ldo7);
	printf("LDO19: 0x%x, LDO20: 0x%x\n", ldo19, ldo20);
	printf("LDO27: 0x%x, LDO28: 0x%x\n", ldo27, ldo28);
	printf("LDO_DVS1: 0x%x, LDO_DVS2: 0x%x, LDO_DVS3: 0x%x\n",
					ldo_dvs1, ldo_dvs2, ldo_dvs3);

	/* setup PMIC lm3560 */
	IIC2_ESetport();
	/* set GPC0[2], GPC0[3] output low */
	*(unsigned int *)0x14CC0040 &= ~0xFF00;
	*(unsigned int *)0x14CC0040 |= 0x1100;
	*(unsigned int *)0x14CC0044 &= ~0xC;
	/* set GPF0[1] output high */
#ifdef CONFIG_CPU_EXYNOS5433_EVT1
        *(unsigned int *)0x11090000 &= ~0xF0;
        *(unsigned int *)0x11090000 |= 0x10;
        *(unsigned int *)0x11090004 |= 0x2;
#else
	*(unsigned int *)0x14CC01E0 &= ~0xF0;
	*(unsigned int *)0x14CC01E0 |= 0x10;
	*(unsigned int *)0x14CC01E4 |= 0x2;
#endif
	IIC2_EWrite(LM3560_ADDR, 0xE0, 0xEF);
	IIC2_EWrite(LM3560_ADDR, 0xC0, 0xFF);
	IIC2_EWrite(LM3560_ADDR, 0x11, 0x78);
#endif
}

void display_pmic_rtc_time(void)
{
       int i;
       u8 tmp;
       u8 time[NR_PMIC_RTC_CNT_REGS];

       IIC0_ERead(S2MPS13_RTC, RTC_UPDATE, &tmp);
       tmp |= 0x1;
       IIC0_EWrite(S2MPS13_RTC, RTC_UPDATE, tmp);
       udelay(40);

       for (i = 0; i < NR_PMIC_RTC_CNT_REGS; i++)
               IIC0_ERead(S2MPS13_RTC, (RTC_SEC + i), &time[i]);

       printf("RTC TIME: %d-%02d-%02d %02d:%02d:%02d(0x%02x)%s\n",
                       time[PMIC_RTC_YEAR] + 2000, time[PMIC_RTC_MONTH],
                       time[PMIC_RTC_DATE], time[PMIC_RTC_HOUR] & 0x1f, time[PMIC_RTC_MIN],
                       time[PMIC_RTC_SEC], time[PMIC_RTC_WEEK],
                       time[PMIC_RTC_HOUR] & (1 << 6) ? "PM" : "AM");
}

static void display_boot_device_info(void)
{
	struct exynos5430_power *pmu = (struct exynos5430_power *)EXYNOS5430_POWER_BASE;
	int OmPin;

	OmPin = readl(&pmu->inform3);

	printf("\nChecking Boot Mode ...");

	switch (OmPin) {
	case BOOT_MMCSD:
		printf(" SDMMC\n");
		break;
	case BOOT_EMMC:
	case BOOT_EMMC_4_4:
		printf(" EMMC\n");
		break;
	case BOOT_UFS:
		printf(" UFS\n");
		break;
	default:
		printf(" Please check OM_pin\n");
		break;
	}
}

#ifdef CONFIG_CPU_EXYNOS5433
static void board_trip_info(void)
{
	struct exynos5430_power *pmu = (struct exynos5430_power *)EXYNOS5430_POWER_BASE;
	unsigned int trip_data;

	trip_data = readl(&pmu->pmu_spare3);

	switch (trip_data) {
	case LATCHED_SW_TRIP_DATA:
		printf("\nSW TRIP Detacted.\n");
		writel(0x0, &pmu->pmu_spare3);
		break;
	case LATCHED_HW_TRIP_DATA:
		printf("\nHW TRIP Detacted.\n");
		writel(0x0, &pmu->pmu_spare3);
		break;
	case LATCHED_SWHW_TRIP_DATA:
		printf("\nSW/HW TRIP Detacted.\n");
		writel(0x0, &pmu->pmu_spare3);
		break;
	default:
		printf("\nNot Detacted TRIP.\n");
	}
}
#endif

static void display_tmu_info(void)
{
	unsigned int tmp, i;
	unsigned int fuse_25[5] = {0, };
	unsigned int fuse_85[5] = {0, };
	unsigned int vptat_ctrl[5] ={0, };
	unsigned int is_2point_calib[5] = {0, };
	unsigned int tmu[5] = {0, };
	unsigned int trim_info[5] = {EXYNOS5430_TMU_TRIMINFO_0_REG,
					EXYNOS5430_TMU_TRIMINFO_1_REG,
					EXYNOS5430_TMU_TRIMINFO_2_REG,
					EXYNOS5430_TMU_TRIMINFO_3_REG,
					EXYNOS5430_TMU_TRIMINFO_4_REG,};

	unsigned int tmu_ctrl[5] = {EXYNOS5430_TMU_CTRL_0_REG,
					EXYNOS5430_TMU_CTRL_1_REG,
					EXYNOS5430_TMU_CTRL_2_REG,
					EXYNOS5430_TMU_CTRL_3_REG,
					EXYNOS5430_TMU_CTRL_4_REG,};

	unsigned int cur_temp[5] = {EXYNOS5430_TMU_CURTEMP_0_REG,
					EXYNOS5430_TMU_CURTEMP_1_REG,
					EXYNOS5430_TMU_CURTEMP_2_REG,
					EXYNOS5430_TMU_CURTEMP_3_REG,
					EXYNOS5430_TMU_CURTEMP_4_REG,};

	unsigned int tmu_status[5] = {EXYNOS5430_TMU_STATUS_0_REG,
					EXYNOS5430_TMU_STATUS_1_REG,
					EXYNOS5430_TMU_STATUS_2_REG,
					EXYNOS5430_TMU_STATUS_3_REG,
					EXYNOS5430_TMU_STATUS_4_REG,};

	int timeout = 20000;

#ifdef CONFIG_CPU_EXYNOS5433_EVT1
	unsigned int otp_fuse_25[5] = {EXYNOS5433_OTP_TEMP25_ATLAS0,
					EXYNOS5433_OTP_TEMP25_ATLAS1,
					EXYNOS5433_OTP_TEMP25_GPU,
					EXYNOS5433_OTP_TEMP25_APOLO,
					EXYNOS5433_OTP_TEMP25_ISP,};

	unsigned int otp_fuse_85[5] = {EXYNOS5433_OTP_TEMP85_ATLAS0,
					EXYNOS5433_OTP_TEMP85_ATLAS1,
					EXYNOS5433_OTP_TEMP85_GPU,
					EXYNOS5433_OTP_TEMP85_APOLO,
					EXYNOS5433_OTP_TEMP85_ISP,};

	unsigned int otp_vptat_ctrl[5] = {EXYNOS5433_OTP_VPTAT_ATLAS0,
					EXYNOS5433_OTP_VPTAT_ATLAS1,
					EXYNOS5433_OTP_VPTAT_GPU,
					EXYNOS5433_OTP_VPTAT_APOLO,
					EXYNOS5433_OTP_VPTAT_ISP,};

	unsigned int otp_caltype[5] = {EXYNOS5433_OTP_CALTYPE_ATLAS0,
					EXYNOS5433_OTP_CALTYPE_ATLAS1,
					EXYNOS5433_OTP_CALTYPE_GPU,
					EXYNOS5433_OTP_CALTYPE_APOLO,
					EXYNOS5433_OTP_CALTYPE_ISP,};

	unsigned int is_otp_valid = 0;

	for (i = 0; i < 5; i++) {
		fuse_25[i] = otp_tmu_parse(otp_fuse_25[i]);
		fuse_85[i] = otp_tmu_parse(otp_fuse_85[i]);
		vptat_ctrl[i] = otp_tmu_parse(otp_vptat_ctrl[i]) << 20;
		is_2point_calib[i] = otp_tmu_parse(otp_caltype[i]);
	}

	if (fuse_25[0] != 0)
		is_otp_valid = 1;
#endif

	for (i = 0; i < 5; i++) {
		/* read fuse of each sensor */
#ifdef CONFIG_CPU_EXYNOS5433_EVT1
		if (is_otp_valid == 0) {
			fuse_25[i] = readl(trim_info[i]) & 0xff;
			fuse_85[i] = (readl(trim_info[i]) & 0xff00) >> 8;
			is_2point_calib[i] = (readl(trim_info[i]) & 0x00800000) ? 1 : 0;
			vptat_ctrl[i] = (readl(trim_info[i]) & 0x700000);
		}
#else
		fuse_25[i] = readl(trim_info[i]) & 0xff;
		fuse_85[i] = (readl(trim_info[i]) & 0xff00) >> 8;
		is_2point_calib[i] = (readl(trim_info[i]) & 0x00800000) ? 1 : 0;
		vptat_ctrl[i] = (readl(trim_info[i]) & 0x700000);
#endif

		tmp = readl(tmu_ctrl[i]) & 0xe08f00fe ;

		/* if G3D TMU */
		if (i == 2) {
			if (is_2point_calib[i])
				tmp |= (((23 << 24) | (6 << 20) | (8 << 8)) & ~0xe08f00fe);
			else
				tmp |= (((16 << 24) | (6 << 20) | (8 << 8)) & ~0xe08f00fe);
		} else {
			if (is_2point_calib[i])
				tmp |= (((16 << 24) | vptat_ctrl[i] | (8 << 8)) & ~0xe08f00fe);
			tmp |= (((16 << 24) | (8 << 8)) & ~0xe08f00fe);
		}
		writel(tmp, tmu_ctrl[i]);

		tmp |= 0x1;
		writel(tmp, tmu_ctrl[i]);
		tmp &= 0xfffffffe;
		writel(tmp, tmu_ctrl[i]);

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
	for (i = 0; i < 5; i++) {
		if (is_2point_calib[i])
			tmu[i] = (readl(cur_temp[i]) - fuse_25[i]) * (85 - 25) / (fuse_85[i] - fuse_25[i]) + 25;
		else
			tmu[i] = readl(cur_temp[i]) - fuse_25[i] + 25;
	}
	printf("[TMU] %d, %d, %d, %d, %d\n",
		tmu[0], tmu[1], tmu[2], tmu[3], tmu[4]);
}

static void domain_power_off(void)
{
#ifdef CONFIG_PREP_BOARDCONFIG_FOR_KERNEL
	/* power off domain */
	writel(0x00000000, EXYNOS5430_POWER_G2D_CONFIGURATION);
	writel(0x00000000, EXYNOS5430_POWER_GSCL_CONFIGURATION);
	writel(0x00000000, EXYNOS5430_POWER_MSCL_CONFIGURATION);
#ifndef CONFIG_CPU_EXYNOS5433
	writel(0x00000000, EXYNOS5430_POWER_G3D_CONFIGURATION);
#endif
	writel(0x00000000, EXYNOS5430_POWER_AUD_CONFIGURATION);
	//writel(0x00000000, EXYNOS5430_POWER_FSYS_CONFIGURATION);
	writel(0x00000000, EXYNOS5430_POWER_MFC0_CONFIGURATION);
#ifndef CONFIG_CPU_EXYNOS5433
	writel(0x00000000, EXYNOS5430_POWER_MFC1_CONFIGURATION);
#endif
	writel(0x00000000, EXYNOS5430_POWER_HEVC_CONFIGURATION);
	writel(0x00000000, EXYNOS5430_POWER_ISP_CONFIGURATION);
	writel(0x00000000, EXYNOS5430_POWER_CAM0_CONFIGURATION);
	writel(0x00000000, EXYNOS5430_POWER_CAM1_CONFIGURATION);
	writel(0x00000000, EXYNOS5430_POWER_DISP_CONFIGURATION);
#endif
}

#ifdef CONFIG_CPU_EXYNOS5433_EVT1
void set_asv_pmic(void)
{
	Get_ASV_Group();
	pmic_asv_init();
}

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
	case BOOT_UFS:
		boot_dev = UFS;
		break;
	default:
		while(1);
	}
	return boot_dev;
}

/* This function should be called before running eMMC init */
static void load_secure_payload(void)
{
	unsigned int om_status = readl(EXYNOS5430_POWER_BASE + INFORM3_OFFSET);

	load_sp_image(device(om_status));
}

#ifndef CONFIG_ARM64
static void print_el3_monitor_version(void)
{
	char el3_mon_ver[50] = { 0, };

	get_el3_mon_version(el3_mon_ver, 50);

	printf("EL3 monitor information => %s\n", el3_mon_ver);
}
#endif
#endif

int board_init(void)
{
	u32 second_boot_info = readl(second_boot_info_base);
#ifdef CONFIG_CPU_EXYNOS5433_EVT1
	set_asv_pmic();
#endif

	display_bl1_version();

	display_chip_id();

	display_pmic_info();

	display_tmu_info();

	display_boot_device_info();
#ifdef CONFIG_CPU_EXYNOS5433
	board_trip_info();
#endif

#ifdef CONFIG_CPU_EXYNOS5433_EVT1
	if(*(unsigned int *)CONFIG_SYS_SDRAM_BASE != 0xabcdef) {
		printf("Running on DRAM by TRACE32: skip load_secure_payload()\n");
	} else {
		if (find_second_boot() != SECOND_BOOT_MODE)
			load_secure_payload();
#ifndef CONFIG_ARM64
		print_el3_monitor_version();
#endif
	}
#endif

	domain_power_off();

	gd->bd->bi_boot_params = (PHYS_SDRAM_1 + 0x100UL);

	return 0;
}

int dram_init(void)
{
	gd->ram_size	= get_ram_size((long *)PHYS_SDRAM_1, PHYS_SDRAM_1_SIZE)
			+ get_ram_size((long *)PHYS_SDRAM_2, PHYS_SDRAM_2_SIZE)
			+ get_ram_size((long *)PHYS_SDRAM_3, PHYS_SDRAM_3_SIZE)
			+ get_ram_size((long *)PHYS_SDRAM_4, PHYS_SDRAM_5_SIZE)
			+ get_ram_size((long *)PHYS_SDRAM_5, PHYS_SDRAM_4_SIZE)
			+ get_ram_size((long *)PHYS_SDRAM_6, PHYS_SDRAM_5_SIZE)
			+ get_ram_size((long *)PHYS_SDRAM_7, PHYS_SDRAM_6_SIZE);

	gd->ram_size += get_ram_size((long *)PHYS_SDRAM_8, PHYS_SDRAM_8_END_SIZE);

	return 0;
}

void dram_init_banksize(void)
{
	/* if U-boot is executed with T32, assume that DRAM is 2Gbyte */
	if(*(unsigned int *)CONFIG_SYS_SDRAM_BASE != 0xabcdef) {
		printf("Running on DRAM by TRACE32: assume 2Gbyte DRAM\n");
		*(unsigned int *)(CONFIG_SYS_SDRAM_BASE + 4) = 0x80000000;
	} else {
#ifdef CONFIG_CPU_EXYNOS5433_EVT1
		Outp32(CONFIG_SYS_SDRAM_BASE + 4, Inp32(CONFIG_PHY_BL_SYS_INFO_DRAM_SIZE));
#endif
	}

	printf("DRAM size from DMC: 0x%X\n", *(unsigned int *)(CONFIG_SYS_SDRAM_BASE + 4));

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
	if (*(unsigned int *)(CONFIG_SYS_SDRAM_BASE + 4) == 0x80000000) {
		gd->bd->bi_dram[7].start = PHYS_SDRAM_8;
		gd->bd->bi_dram[7].size = get_ram_size((long *)PHYS_SDRAM_8,
			PHYS_SDRAM_8_END_SIZE - CONFIG_UBOOT_ATAG_RESERVED_DRAM);
#ifdef CONFIG_ARM64
		gd->bd->bi_dram[8].start = 0;
		gd->bd->bi_dram[8].size = 0;
		gd->bd->bi_dram[9].start = 0;
		gd->bd->bi_dram[9].size = 0;
		gd->bd->bi_dram[10].start = 0;
		gd->bd->bi_dram[10].size = 0;
		gd->bd->bi_dram[11].start = 0;
		gd->bd->bi_dram[11].size = 0;
#endif
	} else {
		gd->bd->bi_dram[7].start = PHYS_SDRAM_8;
		gd->bd->bi_dram[7].size = PHYS_SDRAM_8_SIZE;
		gd->bd->bi_dram[8].start = PHYS_SDRAM_9;
		gd->bd->bi_dram[8].size = PHYS_SDRAM_9_SIZE;
		gd->bd->bi_dram[9].start = PHYS_SDRAM_10;
		gd->bd->bi_dram[9].size = PHYS_SDRAM_10_SIZE;
		gd->bd->bi_dram[10].start = PHYS_SDRAM_11;
		gd->bd->bi_dram[10].size = PHYS_SDRAM_11_SIZE;
		gd->bd->bi_dram[11].start = PHYS_SDRAM_12;
		gd->bd->bi_dram[11].size = PHYS_SDRAM_12_SIZE - CONFIG_UBOOT_ATAG_RESERVED_DRAM;
	}
}

int board_eth_init(bd_t *bis)
{
	return 0;
}

#ifdef CONFIG_DISPLAY_BOARDINFO
int checkboard(void)
{
#else
#ifdef CONFIG_CPU_EXYNOS5433
	printf("\nBoard: XYREF5433\n");
#else
	printf("\nBoard: XYREF5430\n");
#endif
#endif

	return 0;
}
#endif

#ifdef CONFIG_GENERIC_MMC
int board_mmc_init(bd_t *bis)
{
	struct exynos5430_power *pmu = (struct exynos5430_power *)EXYNOS5430_POWER_BASE;
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

#if defined(CONFIG_CMD_SCSI) && defined(CONFIG_SCSI_UFS)

#define UPSCTRL_KEY256_EN	(1<<8) // 256 bit Key Enable
#define UPSCTRL_SR_EN		(1<<7) // Secure Read Enable
#define UPSCTRL_SW_EN		(1<<6) // Secure Write Enable
#define UPSCTRL_NSR_EN		(1<<5) // Non-Secure Read Enable
#define UPSCTRL_NSW_EN		(1<<4) // Non-Secure Write Enable
#define UPSCTRL_UFK_EN		(1<<3) // Use Fuse Key
#define UPSCTRL_ECB_EN		(1<<2) // ECB Enable
#define UPSCTRL_ENC_EN		(1<<1) // Encrypt Enable
#define UPSCTRL_VALID_EN	(1<<0) // Valid

/*
	0 : External OSC 26Mhz -> 26MHz Refclk out
	1 : Internal PLL 26Mhz -> 26MHz Refclk out
	2 : External 26Mhz Xtal -> 26MHz Refclk out
	3 : External 24Mhz Xtal -> MPHY PLL -> 26Mhz Refclk out
*/
#define UFS_REFCLK_SET		3
#define UFS_GEAR		2
#define UFS_RATE		2
#define UFS_LANE		1
#define UFS_POWER_MODE	1
#define UFS_RXTX_POWER_MODE	((UFS_POWER_MODE<<4)|UFS_POWER_MODE)

#define UNIPRO_PCLK_SRC_PERIOD_NS	7.5
#define HOST_UNIPRO_PCLK_DIV_POWER	0
#define HOST_UNIPRO_PCLK_PERIOD_NS	(UNIPRO_PCLK_SRC_PERIOD_NS * (1 << HOST_UNIPRO_PCLK_DIV_POWER))
#define RX_HIBER8_WAIT_NS		4000000
#define TX_DIF_P_PERIOD_NS		3000000
#define TX_DIF_N_PERIOD_NS		1000000
#define RX_DIF_P_PERIOD_NS		1000000

static int ufs_device_reset(struct ufs_host *ufs, int onoff)
{
	writel((onoff<<1), ufs->ioaddr + VS_GPIO_OUT);
	return 0;
}

static int ufs_device_power(struct ufs_host *ufs, int onoff)
{
#ifdef CONFIG_CPU_EXYNOS5433_EVT1
	UFS_SET_SFR(0x14cc0184, onoff, 0x1, 6);
#else
	/* GPF3_DAT[5] : GPF3_5=1 */
	UFS_SET_SFR(0x14cc0244, onoff, 0x1, 5);
#endif
	return 0;
}

static int ufs_pre_setup(struct ufs_host *ufs)
{
	u32 reg;

	/* UFS_PHY_CONTROL : 1 = Isolation bypassed, PMU MPHY ON */
	if ( (readl(0x105c0724)&1)==0 )
	{
		writel(0x01, 0x105c0724);
	}
	/* VS_SW_RST */
	if ( (readl(ufs->ioaddr + VS_FORCE_HCS)>>4)&0xf )
	{
		writel(0x0, VS_FORCE_HCS);
	}
	writel(1, (ufs->ioaddr + VS_SW_RST));
	while ( readl(ufs->ioaddr + VS_SW_RST) );
	/* VENDOR_SPECIFIC_IS[20] : clear UFS_IDLE_Indicator bit (if UFS_LINK is reset, this bit is asserted) */
	reg = readl(ufs->ioaddr + VS_IS);
	if ( (reg>>20)&0x1 )
	{
		writel(reg, (ufs->ioaddr + VS_IS));
	}
	/* Set Unipro PCLK Divide value to 1 */
	writel((1<<4), (ufs->ioaddr + VS_UNIPRO_APB_CLK_CTRL));

	ufs_device_reset(ufs, 0);
	/* GPF3_DAT[5] : GPF3_5=0 */
	ufs_device_power(ufs, 0);

	udelay(2000);

	writel(0xffffffff , (ufs->ioaddr + 0x10e4));		/* rH_UNIPRO_DME_INTR_ENABLE */
	writel(1 , (ufs->ioaddr + REG_CONTROLLER_ENABLE));

	while (!(readl(ufs->ioaddr + REG_CONTROLLER_ENABLE) & 0x1)) {
		udelay(1);
	}

	writel(0xf , (ufs->ioaddr + VS_AXIDMA_RWDATA_BURST_LEN));

	ufs_device_reset(ufs, 1);
	ufs_device_power(ufs, 1);
	udelay(1220);

	reg = readl(ufs->ioaddr + UFSP_UPRSECURITY);
	writel(reg & (~0x00020012), ufs->ioaddr + UFSP_UPRSECURITY);
	reg = readl(ufs->ioaddr + UFSP_UPWSECURITY);
	writel(reg & (~0x00000012), ufs->ioaddr + UFSP_UPWSECURITY);

	memset( ufs->cmd_desc_addr, 0x00, UFS_NUTRS*sizeof(struct ufs_cmd_desc));
	memset( ufs->utrd_addr, 0x00, UFS_NUTRS*sizeof(struct ufs_utrd));
	memset( ufs->utmrd_addr, 0x00, UFS_NUTMRS*sizeof(struct ufs_utmrd));
	ufs->utrd_addr->cmd_desc_addr_l = cpu_to_le32(ufs->cmd_desc_addr);
	ufs->utrd_addr->rsp_upiu_off = cpu_to_le16(offsetof(struct ufs_cmd_desc, response_upiu));
	ufs->utrd_addr->rsp_upiu_len= cpu_to_le16(ALIGNED_UPIU_SIZE);

	writel(ufs->utmrd_addr, (ufs->ioaddr + REG_UTP_TASK_REQ_LIST_BASE_L));
	writel(0, (ufs->ioaddr + REG_UTP_TASK_REQ_LIST_BASE_H));

	writel(ufs->utrd_addr, (ufs->ioaddr + REG_UTP_TRANSFER_REQ_LIST_BASE_L));
	writel(0, (ufs->ioaddr + REG_UTP_TRANSFER_REQ_LIST_BASE_H));

	return CMD_RET_SUCCESS;
}

static int ufs_vendor_setup(struct ufs_host *ufs)
{
	/* DMA little endian, order change */
	writel(0xa, (ufs->ioaddr + VS_DATA_REORDER));

	writel(1, (ufs->ioaddr + REG_UTP_TASK_REQ_LIST_RUN_STOP));

	writel(1, (ufs->ioaddr + REG_UTP_TRANSFER_REQ_LIST_RUN_STOP));

	writel(UFS_SG_BLOCK_SIZE_BIT, (ufs->ioaddr + VS_TXPRDT_ENTRY_SIZE));
	writel(UFS_SG_BLOCK_SIZE_BIT, (ufs->ioaddr + VS_RXPRDT_ENTRY_SIZE));
	writel(0, (ufs->ioaddr + VS_TO_CNT_DIV_VAL));
	writel(100, (ufs->ioaddr + VS_40US_TO_CNT_VAL));

	writel(0x810001FF, (ufs->ioaddr + REG_UTP_TRANSFER_REQ_INT_AGG_CONTROL));

	writel(0xFFFFFFFF, (ufs->ioaddr + VS_UTRL_NEXUS_TYPE));
	writel(0xFFFFFFFF, (ufs->ioaddr + VS_UMTRL_NEXUS_TYPE));

	return CMD_RET_SUCCESS;
}

static struct ufs_uic_cmd mphy_unipro_setup[] =
{
	{UIC_CMD_DME_SET, (0x1614<<16), 0, HOST_UNIPRO_PCLK_PERIOD_NS}	/* DBG_CLK_PERIOD */

	/* _UFS_MPHY_Basic_Tune() for series A */
	, {UIC_CMD_DME_SET, (0x40a<<16), 0, 0x06}
	, {UIC_CMD_DME_SET, (0x411<<16), 0, 0x03}		//restore reset value
	, {UIC_CMD_DME_SET, (0x412<<16), 0, 0x03}
	, {UIC_CMD_DME_SET, (0x413<<16), 0, 0x03}
	, {UIC_CMD_DME_SET, (0x414<<16), 0, 0x02}
	, {UIC_CMD_DME_SET, (0x416<<16), 0, 0x07}
	, {UIC_CMD_DME_SET, (0x417<<16), 0, 0x01}
	, {UIC_CMD_DME_SET, (0x419<<16), 0, 0x98}
	, {UIC_CMD_DME_SET, (0x41a<<16), 0, 0x10}
	, {UIC_CMD_DME_SET, (0x421<<16), 0, 0x4e}
	, {UIC_CMD_DME_SET, (0x431<<16), 0, 0x10}
	, {UIC_CMD_DME_SET, (0x444<<16), 0, 0x07}
	, {UIC_CMD_DME_SET, (0x44d<<16), 0, 0x01}

	, {UIC_CMD_DME_SET, (0x1640<<16), 0, 0x01}	/* DBG_OV_TM */

	, {UIC_CMD_DME_SET, (0x276<<16), 0, 0x02}		// TX
	, {UIC_CMD_DME_SET, (0x284<<16), 0, 0x01}
	, {UIC_CMD_DME_SET, (0x285<<16), 0, 0xaa}
	, {UIC_CMD_DME_SET, (0x276<<16)|1, 0, 0x02}
	, {UIC_CMD_DME_SET, (0x284<<16)|1, 0, 0x01}
	, {UIC_CMD_DME_SET, (0x285<<16)|1, 0, 0xaa}

	, {UIC_CMD_DME_SET, (0x30a<<16), 0, 0xac}		// RX
	, {UIC_CMD_DME_SET, (0x329<<16), 0, 0x14}		//restore reset value
	, {UIC_CMD_DME_SET, (0x32e<<16), 0, 0xc0}		//restore reset value
	, {UIC_CMD_DME_SET, (0x32f<<16), 0, 0xab}
	, {UIC_CMD_DME_SET, (0x30a<<16)|1, 0, 0xac}		// RX
	, {UIC_CMD_DME_SET, (0x329<<16)|1, 0, 0x14}		//restore reset value
	, {UIC_CMD_DME_SET, (0x32e<<16)|1, 0, 0xc0}		//restore reset value
	, {UIC_CMD_DME_SET, (0x32f<<16)|1, 0, 0xab}

	, {UIC_CMD_DME_SET, (0x1640<<16), 0, 0x00}	/* DBG_OV_TM */

	/* UniproSetMinimalCodeForMPHY() */
	/* UniproSetPACP_CNFBIT */
	, {UIC_CMD_REGISTER_SET, 0x10c8, 0x01, 0}		/* rH_UNIPRO_DME_PACP_CNFBIT */
	/* UniproSetPACP_CNFBIT */

	, {UIC_CMD_DME_SET, (0x1640<<16), 0, 0x01}	/* DBG_OV_TM */
	, {UIC_CMD_DME_SET, (0x1614<<16), 0, HOST_UNIPRO_PCLK_PERIOD_NS}	/* DBG_CLK_PERIOD */

	, {UIC_CMD_DME_SET, (0x1659<<16), 0, 0x01}	/* DBG_IGNORE_INCOMING */
	, {UIC_CMD_DME_SET, (0x15a7<<16), 0, 10}		/* PA_Hibern8Time */
	, {UIC_CMD_DME_SET, (0x28e<<16), 0, 0x82}		/* TX_H8_Enter_Timer */
	, {UIC_CMD_DME_SET, (0x28e<<16)|1, 0, 0x82}	/* TX_H8_Enter_Timer */

		/* Lane0 */
	, {UIC_CMD_DME_SET, (0x317<<16), 0, (RX_DIF_P_PERIOD_NS/(HOST_UNIPRO_PCLK_PERIOD_NS*(1<<12)))}	/* RX_LineResetValue */
	, {UIC_CMD_DME_SET, (0x277<<16), 0, (TX_DIF_N_PERIOD_NS/(HOST_UNIPRO_PCLK_PERIOD_NS*(1<<10)))}	/* TX_LineResetNvalue */
	, {UIC_CMD_DME_SET, (0x27d<<16), 0, (TX_DIF_P_PERIOD_NS/(HOST_UNIPRO_PCLK_PERIOD_NS*(1<<12)))}	/* TX_LineResetPvalue */
	, {UIC_CMD_DME_SET, (0x15a8<<16), 0, 400}	/* PA_TActivate: 10us unit */

		/* Lane1 */
	, {UIC_CMD_DME_SET, (0x317<<16)|1, 0, (RX_DIF_P_PERIOD_NS/(HOST_UNIPRO_PCLK_PERIOD_NS*(1<<12)))}	/* RX_LineResetValue */
	, {UIC_CMD_DME_SET, (0x277<<16)|1, 0, (TX_DIF_N_PERIOD_NS/(HOST_UNIPRO_PCLK_PERIOD_NS*(1<<10)))}	/* TX_LineResetNvalue */
	, {UIC_CMD_DME_SET, (0x27d<<16)|1, 0, (TX_DIF_P_PERIOD_NS/(HOST_UNIPRO_PCLK_PERIOD_NS*(1<<12)))}	/* TX_LineResetPvalue */

		/* Lane0 */
	, {UIC_CMD_DME_SET, (0x331<<16), 0, (((u32)(RX_HIBER8_WAIT_NS/HOST_UNIPRO_PCLK_PERIOD_NS)>>16)&0x1f)}	/* reg_rx_linereset_value */
	, {UIC_CMD_DME_SET, (0x332<<16), 0, (((u32)(RX_HIBER8_WAIT_NS/HOST_UNIPRO_PCLK_PERIOD_NS)>>8)&0xff)}	/* reg_rx_linereset_value */
	, {UIC_CMD_DME_SET, (0x333<<16), 0, ((u32)(RX_HIBER8_WAIT_NS/HOST_UNIPRO_PCLK_PERIOD_NS)&0xff)}		/* reg_rx_linereset_value */

		/* Lane1 */
	, {UIC_CMD_DME_SET, (0x331<<16)|1, 0, (((u32)(RX_HIBER8_WAIT_NS/HOST_UNIPRO_PCLK_PERIOD_NS)>>16)&0x1f)}	/* reg_rx_linereset_value */
	, {UIC_CMD_DME_SET, (0x332<<16)|1, 0, (((u32)(RX_HIBER8_WAIT_NS/HOST_UNIPRO_PCLK_PERIOD_NS)>>8)&0xff)}	/* reg_rx_linereset_value */
	, {UIC_CMD_DME_SET, (0x333<<16)|1, 0, ((u32)(RX_HIBER8_WAIT_NS/HOST_UNIPRO_PCLK_PERIOD_NS)&0xff)}	/* reg_rx_linereset_value */

	, {UIC_CMD_DME_SET, (0x316<<16), 0, 0x02}					/* UFS_MPHY_RX_FILLER_ENABLE */
	, {UIC_CMD_DME_SET, (0x316<<16)|1, 0, 0x02}				/* UFS_MPHY_RX_FILLER_ENABLE */

	, {UIC_CMD_DME_SET, (0x2041<<16), 0, 0x1FFF}				/* DL_FC0ProtTimeOutVal */
	, {UIC_CMD_DME_SET, (0x2042<<16), 0, 0xFFFF}				/* DL_TC0ReplayTimeOutVal */
	, {UIC_CMD_DME_SET, (0x2043<<16), 0, 0x7FFF}				/* DL_AFC0ReqTimeOutVal */

	, {UIC_CMD_DME_SET, (0x2061<<16), 0, 0x1FFF}				/* DL_FC1ProtTimeOutVal */
	, {UIC_CMD_DME_SET, (0x2062<<16), 0, 0xFFFF}				/* DL_TC1ReplayTimeOutVal */
	, {UIC_CMD_DME_SET, (0x2063<<16), 0, 0x7FFF}				/* DL_AFC1ReqTimeOutVal */

	, {UIC_CMD_DME_SET, (0x15b0<<16), 0, 0x1FFF}				/* PA_PWRModeUserData0 */
	, {UIC_CMD_DME_SET, (0x15b1<<16), 0, 0xFFFF}				/* PA_PWRModeUserData1 */
	, {UIC_CMD_DME_SET, (0x15b2<<16), 0, 0x7FFF}				/* PA_PWRModeUserData2 */

	, {UIC_CMD_REGISTER_SET, 0x1098, 0x1FFF, 0}		/* rH_UNIPRO_DME_POWERMODE_REQ_LOCALL2TIMER0 */
	, {UIC_CMD_REGISTER_SET, 0x109c, 0xFFFF, 0}		/* rH_UNIPRO_DME_POWERMODE_REQ_LOCALL2TIMER1 */
	, {UIC_CMD_REGISTER_SET, 0x10a0, 0x7FFF, 0}		/* rH_UNIPRO_DME_POWERMODE_REQ_LOCALL2TIMER2 */

	, {UIC_CMD_REGISTER_SET, 0x10a4, 0x1FFF, 0}		/* rH_UNIPRO_DME_POWERMODE_REQ_REMOTEL2TIMER0 */
	, {UIC_CMD_REGISTER_SET, 0x10a8, 0xFFFF, 0}		/* rH_UNIPRO_DME_POWERMODE_REQ_REMOTEL2TIMER1 */
	, {UIC_CMD_REGISTER_SET, 0x10ac, 0x7FFF, 0}		/* rH_UNIPRO_DME_POWERMODE_REQ_REMOTEL2TIMER2 */

	, {UIC_CMD_DME_SET, (0x340<<16), 0, 0x1a}			/* RX SleepCountTimer(r_ov_sleep_cnt_timer) */
	, {UIC_CMD_DME_SET, (0x340<<16)|1, 0, 0x1a}		/* RX SleepCountTimer(r_ov_sleep_cnt_timer) */

	, {UIC_CMD_DME_SET, (0x282<<16), 0, 0x26}			/* next_rct_mode_lcc */
	, {UIC_CMD_DME_SET, (0x282<<16)|1, 0, 0x26}		/* next_rct_mode_lcc */

	, {UIC_CMD_DME_SET, (0x1640<<16), 0, 0x00}	/* DBG_OV_TM */

	/* UniproSetMinimalCodeForMPHY() */

	/* UniproSetHostUniProAttr() */
	, {UIC_CMD_DME_SET, (0x3000<<16), 0, 0x00}	/* N_DeviceID */
	, {UIC_CMD_DME_SET, (0x3001<<16), 0, 0x01}	/* N_DeviceID_valid */
	, {UIC_CMD_DME_SET, (0x4021<<16), 0, 0x01}	/* T_PeerDeviceID */

	//T_CPortFlags[0] : if(1) E2E FC ON, else if(0) E2E FC OFF
	//T_CPortFlags[1] : if(1) CSD OFF, else if(0) CSD ON
	, {UIC_CMD_DME_SET, (0x4025<<16), 0, 0x02}	/* T_CPortFlags */
	, {UIC_CMD_DME_SET, (0x4023<<16), 0, 0x00}	/* T_TrafficClass */
	, {UIC_CMD_DME_SET, (0x4020<<16), 0, 0x01}	/* T_ConnectionState */
	/* UniproSetHostUniProAttr() */

	/* NULL TERMINATION */
	, {0,0,0,0}
};

static struct ufs_uic_cmd mphy_unipro_gear_change[] =
{
#if (UFS_LANE == 1)
	{UIC_CMD_DME_SET, (0x1640<<16), 0, 0x01}	/* DBG_OV_TM */
	, {UIC_CMD_DME_SET, (0x302<<16)|1, 0, 0xff}	/* RX Lane1 Power off */
	, {UIC_CMD_DME_SET, (0x303<<16)|1, 0, 0xff}
	, {UIC_CMD_DME_SET, (0x27f<<16)|1, 0, 0x01}	/* TX SW reset */
	, {UIC_CMD_DME_SET, (0x1640<<16), 0, 0x00}	/* DBG_OV_TM */
	, {UIC_CMD_DME_SET, (0x1568<<16), 0, UFS_GEAR}	/* PA_TxGear */
#else
	{UIC_CMD_DME_SET, (0x1568<<16), 0, UFS_GEAR}	/* PA_TxGear */
#endif
	, {UIC_CMD_DME_SET, (0x1583<<16), 0, UFS_GEAR}	/* PA_RxGear */

	, {UIC_CMD_DME_SET, (0x156a<<16), 0, UFS_RATE}	/* PA_HSSeries */

#if ((UFS_POWER_MODE==1) || (UFS_POWER_MODE==4))
	/* Set Termination for HS mode */
	, {UIC_CMD_DME_SET, (0x1569<<16), 0, 0x01}	/* PA_TxTermination */
	, {UIC_CMD_DME_SET, (0x1584<<16), 0, 0x01}	/* PA_RxTermination */
#if (UFS_RATE == 2)
	, {UIC_CMD_DME_SET, (0x415<<16), 0, 0x10}
	, {UIC_CMD_DME_SET, (0x447<<16), 0, 0xa3}
	, {UIC_CMD_DME_SET, (0x448<<16), 0, 0x16}
#endif
#else
	/* Termination off */
	, {UIC_CMD_DME_SET, (0x1569<<16), 0, 0x00}	/* PA_TxTermination */
	, {UIC_CMD_DME_SET, (0x1584<<16), 0, 0x00}	/* PA_RxTermination */
#endif

	, {UIC_CMD_DME_SET, (0x1571<<16), 0, UFS_RXTX_POWER_MODE}	/* PA_PWRMode */
	, {UIC_CMD_WAIT_ISR, REG_INTERRUPT_STATUS, (1<<4), 0}	/* PA_PWRMode */

#ifdef SCSI_UFS_DEBUG
	, {UIC_CMD_DME_GET, (0x1571<<16), 0, 0}		/* PA_PWRMode */
	, {UIC_CMD_DME_GET, (0x1568<<16), 0, 0}		/* PA_TxGear */
	, {UIC_CMD_DME_GET, (0x1583<<16), 0, 0}		/* PA_RxGear */
	, {UIC_CMD_DME_GET, (0x156a<<16), 0, 0}		/* PA_HSSeries */
	, {UIC_CMD_DME_PEER_GET, (0x156a<<16), 0, 0}	/* PA_HSSeries */
#endif

	, {UIC_CMD_REGISTER_SET, 0x2204, 0xFFFFFFFF, 0}	/* UPSEND[0] */
	, {UIC_CMD_REGISTER_SET, 0x220c, 0xF1, 0}		/* UPSCTRL[0] */
	, {UIC_CMD_REGISTER_SET, 0x2214, 0xFFFFFFFF, 0}	/* UPSEND[1] */
	, {UIC_CMD_REGISTER_SET, 0x221c, 0xF1, 0}		/* UPSCTRL[1] */
	, {UIC_CMD_REGISTER_SET, 0x2224, 0xFFFFFFFF, 0}	/* UPSEND[2] */
	, {UIC_CMD_REGISTER_SET, 0x222c, 0xF1, 0}		/* UPSCTRL[2] */
	, {UIC_CMD_REGISTER_SET, 0x2234, 0xFFFFFFFF, 0}	/* UPSEND[3] */
	, {UIC_CMD_REGISTER_SET, 0x223c, 0xF1, 0}		/* UPSCTRL[3] */
	, {UIC_CMD_REGISTER_SET, 0x2244, 0xFFFFFFFF, 0}	/* UPSEND[4] */
	, {UIC_CMD_REGISTER_SET, 0x224c, 0xF1, 0}		/* UPSCTRL[4] */
	, {UIC_CMD_REGISTER_SET, 0x2254, 0xFFFFFFFF, 0}	/* UPSEND[5] */
	, {UIC_CMD_REGISTER_SET, 0x225c, 0xF1, 0}		/* UPSCTRL[5] */
	, {UIC_CMD_REGISTER_SET, 0x2264, 0xFFFFFFFF, 0}	/* UPSEND[6] */
	, {UIC_CMD_REGISTER_SET, 0x226c, 0xF1, 0}		/* UPSCTRL[6] */
	, {UIC_CMD_REGISTER_SET, 0x2274, 0xFFFFFFFF, 0}	/* UPSEND[7] */
	, {UIC_CMD_REGISTER_SET, 0x227c, 0xF1, 0}		/* UPSCTRL[7] */

	/* NULL TERMINATION */
	, {0,0,0,0}
};


/*
	3. called by driver/block/scsi_ufs.c
*/
int ufs_board_init(int host_index, struct ufs_host *ufs)
{
	u32 reg, err;

	if (host_index) {
		printf("Currently multi UFS host is not supported!\n");
		return -1;
	}

	sprintf(ufs->host_name,"ufs%d", host_index);
	ufs->irq = 224;
	ufs->ioaddr = 0x15570000;
	ufs->host_index = host_index;

	ufs->ufs_board_pre_setup = ufs_pre_setup;
	ufs->ufs_board_post_setup = ufs_vendor_setup;
	ufs->ufs_hw_reset = ufs_device_reset;
	ufs->ufs_device_power = ufs_device_power;

	ufs->pre_uic_cmd = mphy_unipro_setup;
	ufs->post_uic_cmd = mphy_unipro_gear_change;

//	get_ufs_clk(host_index);
	set_ufs_clk(host_index);
	err = exynos_pinmux_config(PERIPH_ID_UFS0, 0);
//	get_ufs_clk(host_index);
#if 0
	/* PAD_RETENTION_BOOTLDO_OPTION : AUTOMATIC_WAKEUP=0, INITIATE_WAKEUP_FROM_LOWPWR=1 */
	if ((readl(0x105c3244)&1)==0) {
		writel(0x10000000, 0x105c3248);
	}

	/* PAD_RETENTION_MMC0_OPTION : AUTOMATIC_WAKEUP=0, INITIATE_WAKEUP_FROM_LOWPWR=1 */
	if ((readl(0x105c3144)&1)==0) {
		writel(0x10000000, 0x105c3148);
	}
#endif
	return 0;
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
	struct exynos5430_power *pmu = (struct exynos5430_power *)EXYNOS5430_POWER_BASE;
	unsigned int rst_stat = 0;
	unsigned char read_0x01;
	unsigned char read_0x02;
	unsigned char read_0x01_rtc;
	unsigned char read_0x09;

	rst_stat = readl(&pmu->rst_stat);
	printf("rst_stat : 0x%x\n", rst_stat);
#ifdef CONFIG_RAMDUMP_MODE
	struct exynos5430_gpio_alive *gpio_alive =
		(struct exynos5430_gpio_alive *) exynos5430_get_base_gpio_alive();
	unsigned int gpio_debounce_cnt = 0;
	int err;

	err = exynos_pinmux_config(PERIPH_ID_INPUT_X2_0, PINMUX_FLAG_NONE);
	if (err) {
		debug("GPX2_0 INPUT not configured\n");
		return err;
	}
	while (s5p_gpio_get_value(&gpio_alive->a2, 0) == 0) {
		/* wait for 50ms */
		if (gpio_debounce_cnt < 5) {
			udelay(DEBOUNCE_DELAY);
			gpio_debounce_cnt++;
		} else {
			/* run fastboot */
			run_command("fastboot", 0);
			break;
		}
	}
	/* read PMIC */
	IIC0_ERead(S2MPS13_ADDR, 0x01, &read_0x01);
	IIC0_ERead(S2MPS13_ADDR, 0x02, &read_0x02);
	IIC0_ERead(S2MPS13_ADDR, 0x09, &read_0x09);
	/* read RTC */
	IIC0_ERead(0x0C, 0x01, &read_0x01_rtc);
	/* WTSR detect condition - WTSR_ON && WTSR_INT && ! MRB_INT */
	if ((read_0x09 & (1 << 7)) && (read_0x02 & (1 << 5))
		&& !(read_0x01 & (1 << 7))) {
		printf("WTSR detected\n");
		run_command("fastboot", 0);
	}
	/* SMPL detect condition - SMPL_ON && SMPL_INT && SMPL_EN */
        if ((read_0x09 & (1 << 6)) && (read_0x02 & (1 << 3))
                && (read_0x01_rtc & (1 << 7))) {
		printf("SMPL detected\n");
                run_command("fastboot", 0);
	}

	/* check reset status for ramdump */
	if ((rst_stat & (WRESET | CORTEXA7_WDTRESET | CORTEXA15_WDTRESET))
		|| (readl(&pmu->sysip_dat0) == CONFIG_RAMDUMP_MODE)
		|| (readl(CONFIG_RAMDUMP_SCRATCH) == CONFIG_RAMDUMP_MODE)) {
		/* run fastboot */
		run_command("fastboot", 0);
	}
#endif
#ifdef CONFIG_RECOVERY_MODE
	u32 second_boot_info = readl(second_boot_info_base);

	if (second_boot_info == 1) {
		printf("###Recovery Mode###\n");
		writel(0x0, second_boot_info_base);
		setenv("bootcmd", CONFIG_RECOVERYCOMMAND);
	}
#endif

	if ((readl(&pmu->sysip_dat0)) == CONFIG_FACTORY_RESET_MODE) {
		writel(0x0, &pmu->sysip_dat0);
		setenv("bootcmd", CONFIG_FACTORY_RESET_BOOTCOMMAND);
	}
#ifdef CONFIG_ARM64
	if(*(unsigned int *)CONFIG_SYS_SDRAM_BASE != 0xabcdef) {
		printf("Running on DRAM by TRACE32: execute auto fusing\n");
		setenv("bootcmd", CONFIG_FUSING_COMMAND);
	}
#endif
	return 0;
}

#ifdef CONFIG_PREP_BOARDCONFIG_FOR_KERNEL
void prep_boardconfig_for_kernel(void)
{
	/* power on domain */
	writel(0x0000000f, EXYNOS5430_POWER_G2D_CONFIGURATION);
	writel(0x0000000f, EXYNOS5430_POWER_GSCL_CONFIGURATION);
	writel(0x0000000f, EXYNOS5430_POWER_MSCL_CONFIGURATION);
#ifndef CONFIG_CPU_EXYNOS5433
	writel(0x0000000f, EXYNOS5430_POWER_G3D_CONFIGURATION);
#endif
	writel(0x0000000f, EXYNOS5430_POWER_AUD_CONFIGURATION);
	//writel(0x0000000f, EXYNOS5430_POWER_FSYS_CONFIGURATION);
	writel(0x0000000f, EXYNOS5430_POWER_MFC0_CONFIGURATION);
#ifndef CONFIG_CPU_EXYNOS5433
	writel(0x0000000f, EXYNOS5430_POWER_MFC1_CONFIGURATION);
#endif
	writel(0x0000000f, EXYNOS5430_POWER_HEVC_CONFIGURATION);
	writel(0x0000000f, EXYNOS5430_POWER_ISP_CONFIGURATION);
	writel(0x0000000f, EXYNOS5430_POWER_CAM0_CONFIGURATION);
	writel(0x0000000f, EXYNOS5430_POWER_CAM1_CONFIGURATION);
	writel(0x0000000f, EXYNOS5430_POWER_DISP_CONFIGURATION);
}
#endif

unsigned int get_board_rev(void)
{
	struct exynos5430_power *pmu = (struct exynos5430_power *)EXYNOS5430_POWER_BASE;
	unsigned int rev = 0;
	int adc_val = 0;
	unsigned int timeout, con;

	return (rev | pmic);
}
