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
#include <asm/arch/pmu.h>
#include <asm/arch/tmu.h>
#include "pmic.h"
#include "pmic_display.h"

#include "pmic_lm3560.h"

#include "muic.h"
#include "setup.h"
#include "dfd.h"
#include "clk-boot.h"

#define DEBOUNCE_DELAY	10000
#define SECOND_BOOT_MODE	0xFEED0002

DECLARE_GLOBAL_DATA_PTR;
unsigned int pmic;
unsigned int second_boot = 0;

#ifdef CONFIG_CHECK_BOOTMODE
#define PHONE_REBOOT 0x1
#define POWER_OFF_CHARGE 0x2
unsigned int check_usb_status = 0;
enum RST_TYPE{
	unknown_reset = 0,
	normal_reset,
	charge_reset,
	alarm_reset,
	software_reset,
	hardware_reset,
	watchdog_reset,
	smpl_reset,
};
#endif
#ifdef LCD_TOUCHPANEL_COMPATIBLE
typedef enum
{
	s6e8aa0_720p_video,
	s6e3fa0_1080p_cmd,
	s6e8fa0_1080p_video,
	lcd_null
}lcd_type;
lcd_type lcd_type_avl7420;
#endif
int init_ldfws(void);

static void display_bl1_version(void)
{
	char bl1_version[9] = {0};

	/* display BL1 version */
#if defined(CONFIG_TRUSTZONE_ENABLE)
	printf("\nTrustZone Enabled BSP");
	strncpy(&bl1_version[0], (char *)CONFIG_BL1_VERSION_INFORM, 8);
	printf("\nBL1 version: %s\n", &bl1_version[0]);
#endif

	if (((readl(0x10800004) & (0x1 << 5)) == 0)
		&& ((readl(0x10900004) & (0x1 << 5)) == 0)
		&& ((readl(0x10A00004) & (0x1 << 5)) == 0)
		&& ((readl(0x10B00004) & (0x1 << 5)) == 0))
		printf("### [DREX] disable Dynamic Self Refresh\n");
	else
		printf("### [DREX] enable Dynamic Self Refresh\n");

	if (((readl(0x10800004) & (0x1 << 1)) == 0)
		&& ((readl(0x10900004) & (0x1 << 1)) == 0)
		&& ((readl(0x10A00004) & (0x1 << 1)) == 0)
		&& ((readl(0x10B00004) & (0x1 << 1)) == 0))
		printf("### [DREX] disable Dynamic Power Down\n");
	else
		printf("### [DREX] enable Dynamic Power Down\n");
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

static void display_chip_id2(u32 id_0)
{
	u32 reverse_id_0 = 0;
	u32 tmp = 0;
	char id[7] = {0,};

	reverse_id_0 = chipid_reverse_value(id_0, 32);
	tmp = (reverse_id_0 >> 11) & 0x1FFFFF;
	chipid_dec_to_36(tmp, id);

	printf("ID2 : %s\n", id);
}

static void display_chip_id(void)
{
#if defined(CONFIG_DISPLAY_CHIPID)
	u32 s5p_chip_id2 = 0;

	if (*(unsigned int *)CONFIG_SYS_SDRAM_BASE != 0xabcdef) {
		/* Running on DRAM by TRACE32 */
		s5p_chip_id[0] = readl(EXYNOS7420_PRO_ID + CHIPID0_OFFSET);
		s5p_chip_id[1] = (readl(EXYNOS7420_PRO_ID + CHIPID1_OFFSET) & 0xFFFF);
	} else {
		s5p_chip_id[0] = exynos_smc_read(SMC_CMD_REG,
			SMC_REG_ID_SFR_R(EXYNOS7420_PRO_ID + CHIPID0_OFFSET));
		s5p_chip_id[1] = (exynos_smc_read(SMC_CMD_REG,
			SMC_REG_ID_SFR_R(EXYNOS7420_PRO_ID + CHIPID1_OFFSET)) & 0xFFFF);
	}
	s5p_chip_id2 = readl(EXYNOS7420_PRO_ID + CHIPID2_OFFSET);

	printf("\nChip ID : %04x%08x / 0x%08x\n",
		s5p_chip_id[1], s5p_chip_id[0], s5p_chip_id2);
	display_chip_id2(s5p_chip_id[0]);
#endif
}

static void display_pmic_info(void)
{
#ifndef CONFIG_SG_BINARY
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

	IIC0_ERead(S2MPS15_ADDR, S2MPS15_PMIC_ID, &pmic_id);
	IIC0_ERead(S2MPS15_ADDR, RTC_BUF, &rtc_ctrl);
	if (pmic_id >= 0x11)
		IIC0_ERead(S2MPS15_ADDR, WRSTBI_CTRL_EVT1, &wrstbi_ctrl);
	else
		IIC0_ERead(S2MPS15_ADDR, WRSTBI_CTRL_EVT0, &wrstbi_ctrl);
	IIC0_ERead(S2MPS15_ADDR, BUCK1_OUT, &read_vol_mif);
	IIC0_ERead(S2MPS15_ADDR, BUCK2_OUT, &read_vol_atlas);
	IIC0_ERead(S2MPS15_ADDR, BUCK3_OUT, &read_vol_apollo);
	IIC0_ERead(S2MPS15_ADDR, BUCK4_OUT, &read_vol_int);
	IIC0_ERead(S2MPS15_ADDR, BUCK5_OUT, &read_vol_disp);
	IIC0_ERead(S2MPS15_ADDR, BUCK6_OUT, &read_vol_g3d);
	IIC0_ERead(S2MPS15_ADDR, LDO_DVS1, &read_ldo_dvs1);
	IIC0_ERead(S2MPS15_ADDR, LDO_DVS2, &read_ldo_dvs2);
	IIC0_ERead(S2MPS15_ADDR, LDO_DVS3, &read_ldo_dvs3);
	IIC0_ERead(S2MPS15_ADDR, LDO_DVS4, &read_ldo_dvs4);

	printf("\nPMIC: S2MPS15(REV%x)\n", pmic_id);

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

	IIC0_ERead(S2MPS15_ADDR, S2MPS15_INT1, &read_vol_disp);
	printf("INT1: 0x%x\n", read_vol_disp);
	IIC0_ERead(S2MPS15_ADDR, S2MPS15_INT2, &read_vol_disp);
	printf("INT2: 0x%x\n", read_vol_disp);
	IIC0_ERead(S2MPS15_ADDR, S2MPS15_INT3, &read_vol_disp);
	printf("INT3: 0x%x\n", read_vol_disp);
	IIC0_ERead(S2MPS15_ADDR, S2MPS15_STATUS1, &read_vol_disp);
	printf("STATUS1: 0x%x\n", read_vol_disp);
	IIC0_ERead(S2MPS15_ADDR, S2MPS15_STATUS2, &read_vol_disp);
	printf("STATUS2: 0x%x\n", read_vol_disp);
	IIC0_ERead(S2MPS15_ADDR, PWRONSRC, &read_vol_disp);
	printf("PWRONSRC: 0x%x\n", read_vol_disp);
	IIC0_ERead(S2MPS15_ADDR, OFFSRC, &read_vol_disp);
	printf("OFFSRC: 0x%x\n", read_vol_disp);
#ifdef CONFIG_CHECK_BOOTMODE
	check_usb_status = check_usb();
#endif

	/* print rtc_buf & wrstbi register value */
	printf("RTC_BUF: 0x%x, WRSTBI: 0x%x\n", rtc_ctrl, wrstbi_ctrl);
	/* print ldo register value */
	printf("LDO_DVS1: 0x%x, LDO_DVS2: 0x%x\n", read_ldo_dvs1, read_ldo_dvs2);
	printf("LDO_DVS3: 0x%x, LDO_DVS4: 0x%x\n", read_ldo_dvs3, read_ldo_dvs4);

#if defined(CONFIG_MACH_ESPRESSO7420) || defined(CONFIG_MACH_AVL7420)
	/* enable LDO18 : VCC_2.8V_PERI */
	IIC0_ERead(S2MPS15_ADDR, LDO18_CTRL, &read_ldo_dvs4);
	read_ldo_dvs4 |= OUTPUT_PWREN_ON;
	IIC0_EWrite(S2MPS15_ADDR, LDO18_CTRL, read_ldo_dvs4);

	/* enable LDO26 : UFS */
	IIC0_ERead(S2MPS15_ADDR, LDO26_CTRL, &read_ldo_dvs4);
	read_ldo_dvs4 |= OUTPUT_PWREN_ON;
	IIC0_EWrite(S2MPS15_ADDR, LDO26_CTRL, read_ldo_dvs4);
#endif
#ifdef CONFIG_EXYNOS_LCD_ON_UBOOT
	/* power control for LCD */
	/* enable VDD_MIPI_1V2(LDO22) */
	IIC0_EWrite(S2MPS15_ADDR, 0x38, 0xEC);
	/* enable VCC_1.8V_Peri(LDO15) */
	IIC0_EWrite(S2MPS15_ADDR, 0x42, 0xEC);
	/* enable VCC_1.8V_Peri(LDO18) */
	//IIC0_EWrite(S2MPS15_ADDR, 0x45, 0xEC);//closed by gu
#if defined(CONFIG_MACH_ESPRESSO7420)
	/* enable VCC_3.3V_Peri(LDO19) */
	IIC0_EWrite(S2MPS15_ADDR, 0x46, 0xEC);
#elif defined(CONFIG_MACH_AVL7420)
	/* enable VCC_3.3V_Peri(LDO21) */
	//IIC0_EWrite(S2MPS15_ADDR, 0x48, 0xEC);//closed by gu
#endif
	/* enable VCC_2.8V_Peri(LDO20) */
	IIC0_EWrite(S2MPS15_ADDR, 0x47, 0xEC);
#endif
#endif
#endif
}

void display_pmic_rtc_time(void)
{
#ifndef CONFIG_SG_BINARY
       int i;
       u8 tmp;
       u8 time[NR_PMIC_RTC_CNT_REGS];

       IIC0_ERead(S2MPS15_RTC, RTC_UPDATE, &tmp);
       tmp |= 0x1;
       IIC0_EWrite(S2MPS15_RTC, RTC_UPDATE, tmp);
       udelay(40);

       for (i = 0; i < NR_PMIC_RTC_CNT_REGS; i++)
               IIC0_ERead(S2MPS15_RTC, (RTC_SEC + i), &time[i]);

       printf("RTC TIME: %d-%02d-%02d %02d:%02d:%02d(0x%02x)%s\n",
                       time[PMIC_RTC_YEAR] + 2000, time[PMIC_RTC_MONTH],
                       time[PMIC_RTC_DATE], time[PMIC_RTC_HOUR] & 0x1f, time[PMIC_RTC_MIN],
                       time[PMIC_RTC_SEC], time[PMIC_RTC_WEEK],
                       time[PMIC_RTC_HOUR] & (1 << 6) ? "PM" : "AM");
#endif
}

static void display_boot_device_info(void)
{
	struct exynos7420_power *pmu = (struct exynos7420_power *)EXYNOS7420_POWER_BASE;
	int OmPin;

	OmPin = readl(&pmu->inform3);

	printf("\nChecking Boot Mode ...");

	if (OmPin == BOOT_MMCSD) {
		printf(" SDMMC\n");
//#if SD_FIRM_IN
		exynos7420_emmc_voltage_config();
//#endif
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

static void display_tmu_info(void)
{
	unsigned int tmp, i;
	int fuse_25[4] = {0, };
	int fuse_85[4] = {0, };
	unsigned int is_2point_calib[4] = {0, };
	unsigned int t_buf_vref_sel[4] = {0, };
	int tmu[4] = {0, };
	int temp_code;
	unsigned int trim_info[4] = {EXYNOS7420_TMU_TRIMINFO_0_REG,
					EXYNOS7420_TMU_TRIMINFO_1_REG,
					EXYNOS7420_TMU_TRIMINFO_2_REG,
					EXYNOS7420_TMU_TRIMINFO_3_REG,};

	unsigned int tmu_ctrl[4] = {EXYNOS7420_TMU_CTRL_0_REG,
					EXYNOS7420_TMU_CTRL_1_REG,
					EXYNOS7420_TMU_CTRL_2_REG,
					EXYNOS7420_TMU_CTRL_3_REG,};

	unsigned int cur_temp[4] = {EXYNOS7420_TMU_CURTEMP_0_REG,
					EXYNOS7420_TMU_CURTEMP_1_REG,
					EXYNOS7420_TMU_CURTEMP_2_REG,
					EXYNOS7420_TMU_CURTEMP_3_REG,};

	unsigned int tmu_status[4] = {EXYNOS7420_TMU_STATUS_0_REG,
					EXYNOS7420_TMU_STATUS_1_REG,
					EXYNOS7420_TMU_STATUS_2_REG,
					EXYNOS7420_TMU_STATUS_3_REG,};

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
		fuse_85[i] = (readl(trim_info[i]) >> 9) & 0x1ff;
		is_2point_calib[i] = (readl(trim_info[i]) & 0x00800000) ? 1 : 0;
		t_buf_vref_sel[i] = (readl(trim_info[i]) >> 18) & 0x1f;

		tmp = readl(tmu_ctrl[i]) & 0xe07f00fe ;

		/* if APOLLO TMU */
		if (i == 1) {
			tmp |= (((16 << 24) | (8 << 8)) & ~0xe07f00fe);
		} else {
			if (t_buf_vref_sel[i] != 0)
				tmp |= (((t_buf_vref_sel[i] << 24) | (9 << 8)) & ~0xe07f00fe);
			else
				tmp |= (((17 << 24) | (9 << 8)) & ~0xe07f00fe);
		}

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
		temp_code = readl(cur_temp[i]) & 0x1ff;
		if (is_2point_calib[i])
			tmu[i] = (temp_code - fuse_25[i]) * (85 - 25) /(fuse_85[i] - fuse_25[i]) + 25;
		else
			tmu[i] = temp_code - fuse_25[i] + 25;
	}
	printf("[TMU] %d, %d, %d, %d\n",
		tmu[0], tmu[1], tmu[2], tmu[3]);
}

static void display_trip_info(void)
{
	struct exynos7420_power *pmu = (struct exynos7420_power *)EXYNOS7420_POWER_BASE;
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

static void domain_power_off(void)
{
#ifdef CONFIG_PREP_BOARDCONFIG_FOR_KERNEL
	/* power off domain */
	writel(0x00000000, EXYNOS7420_POWER_CAM0_CONFIGURATION);
	while((readl(EXYNOS7420_POWER_CAM0_CONFIGURATION + 0x4) & 0xF) != 0x0);

	writel(0x00000000, EXYNOS7420_POWER_CAM1_CONFIGURATION);
	while((readl(EXYNOS7420_POWER_CAM1_CONFIGURATION + 0x4) & 0xF) != 0x0);

	writel(0x00000000, EXYNOS7420_POWER_ISP0_CONFIGURATION);
	while((readl(EXYNOS7420_POWER_ISP0_CONFIGURATION + 0x4) & 0xF) != 0x0);

	writel(0x00000000, EXYNOS7420_POWER_ISP1_CONFIGURATION);
	while((readl(EXYNOS7420_POWER_ISP1_CONFIGURATION + 0x4) & 0xF) != 0x0);

	writel(0x00000000, EXYNOS7420_POWER_MFC_CONFIGURATION);
	while((readl(EXYNOS7420_POWER_MFC_CONFIGURATION + 0x4) & 0xF) != 0x0);

	writel(0x00000000, EXYNOS7420_POWER_MSCL_CONFIGURATION);
	while((readl(EXYNOS7420_POWER_MSCL_CONFIGURATION + 0x4) & 0xF) != 0x0);

	writel(0x00000000, EXYNOS7420_POWER_VPP_CONFIGURATION);
	while((readl(EXYNOS7420_POWER_VPP_CONFIGURATION + 0x4) & 0xF) != 0x0);
#endif
}

#ifndef CONFIG_SG_BINARY
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
static void load_secure_payload(void)
{
	unsigned long ret = 0;
	unsigned int boot_dev = 0;

	if (*(unsigned int *)CONFIG_SYS_SDRAM_BASE != 0xabcdef) {
		printf("Running on DRAM by TRACE32: skip load_secure_payload()\n");
	} else {
		if (find_second_boot() != SECOND_BOOT_MODE) {
			boot_dev = readl(EXYNOS7420_POWER_BASE + INFORM3_OFFSET);

			/*
			 * In case WARM Reset/Watchdog Reset and DumpGPR is enabled,
			 * Secure payload doesn't have to be loaded.
			 */
			if ((!(readl(EXYNOS7420_POWER_BASE + RST_STAT_OFFSET)
					& (WRESET | APOLLO_WDTRESET | ATLAS_WDTRESET)))
				|| (!(readl(EXYNOS7420_POWER_BASE + DFD_RESET_SEQUENCER_OFFSET)
					& DFD_EDPCSR_DUMP_EN))) {
				ret = load_sp_image(device(boot_dev));
				if (ret)
					/*
					 * 0xFEED0002 : Signature check fail
					 * 0xFEED0020 : Anti-rollback check fail
					 */
					printf("Fail to load Secure Payload!! [ret = %ld]\n", ret);
				
			}

			/*
			 * If boot device is eMMC, emmc_endbootop() should be
			 * implemented because secure payload is the last image
			 * in boot partition.
			 */
			if (boot_dev == BOOT_EMMC_4_4)
				emmc_endbootop();
		} else {
			second_boot = 1;
		}
	}
}

static void print_el3_monitor_version(void)
{
	char el3_mon_ver[50] = { 0, };

	if (*(unsigned int *)CONFIG_SYS_SDRAM_BASE == 0xabcdef) {
		/* this booting is from eMMC/UFS. not T32 */
		get_el3_mon_version(el3_mon_ver, 50);
		printf("EL3 monitor information => %s\n", el3_mon_ver);
	}
}
#ifdef LCD_TOUCHPANEL_COMPATIBLE
lcd_type check_lcd_type(void)
{
	int error_num, val, i, try_times = 5, error_times = 2;

	error_num = 0;
	exynos_iic_tp_init();
	for (i=0; i<try_times; i++)
	{
		val = melfas_ReadReg(i);
		if (val < 0)
			error_num++;
	}
	if(error_num < error_times)
		return s6e8aa0_720p_video;
	error_num = 0;
	for (i=0; i<try_times; i++)
	{
		val = atmel_ReadReg(i);
		if (val < 0)
			error_num++;
	}
	if(error_num < error_times)
		return s6e8fa0_1080p_video;
	error_num = 0;
	for (i=0; i<try_times; i++)
	{
		val = synaptics_ReadReg(i);
		if (val < 0)
			error_num++;
	}
	if(error_num < error_times)
		return s6e3fa0_1080p_cmd;	

	return lcd_null;
}
#endif

int board_init(void)
{
#ifndef CONFIG_SG_BINARY
	if(*(unsigned int *)CONFIG_SYS_SDRAM_BASE == 0xabcdef)
		set_asv_pmic();
#endif
	clk_init_root_clock_gating();

//	display_bl1_version();

//	display_chip_id();

//	display_pmic_info();

//	display_tmu_info();

	printf("KALEAO KMax Exynos Node\n");

	display_boot_device_info();
//	printf("AVL avl7420.c : board_init() after  display_boot_device_info()\n");
	display_trip_info();
//	printf("AVL avl7420.c : board_init() after  display_trip_info()\n");
	//load_secure_payload();
//	printf("AVL avl7420.c : board_init() after  load_secure_payload()\n");	
	print_el3_monitor_version();
//	printf("AVL avl7420.c : board_init() after  print_el3_monitor_version()\n");
	domain_power_off();
//	printf("AVL avl7420.c : board_init() after  domain_power_off()\n");	
	gd->bd->bi_boot_params = (PHYS_SDRAM_1 + 0x100UL);
#ifdef LCD_TOUCHPANEL_COMPATIBLE
    lcd_type_avl7420 = check_lcd_type();
	printf("check_lcd_type lcd_type = %d\n",lcd_type_avl7420);
#endif		

	return 0;
}

int dram_init(void)
{

	unsigned long long dram_size = readq(CONFIG_PHY_BL_SYS_INFO_DRAM_SIZE);
	
	if (dram_size == 0xC0000000)
	{
		gd->ram_size	= get_ram_size((long *)PHYS_SDRAM_1, PHYS_SDRAM_1_SIZE)
						+ get_ram_size((long *)PHYS_SDRAM_2, PHYS_SDRAM_2_SIZE)
						+ get_ram_size((long *)PHYS_SDRAM_3, PHYS_SDRAM_3_SIZE)
						+ get_ram_size((long *)PHYS_SDRAM_4, PHYS_SDRAM_4_SIZE)
						+ get_ram_size((long *)PHYS_SDRAM_5, PHYS_SDRAM_5_SIZE)
						+ get_ram_size((long *)PHYS_SDRAM_6, PHYS_SDRAM_6_SIZE)
						+ get_ram_size((long *)PHYS_SDRAM_7, PHYS_SDRAM_7_SIZE)
						+ get_ram_size((long *)PHYS_SDRAM_8, PHYS_SDRAM_8_SIZE)
						+ get_ram_size((long *)PHYS_SDRAM_9, PHYS_SDRAM_9_SIZE)
						+ get_ram_size((long *)PHYS_SDRAM_10, PHYS_SDRAM_10_SIZE)
						+ get_ram_size((long *)PHYS_SDRAM_11, PHYS_SDRAM_11_SIZE)
						+ get_ram_size((long *)PHYS_SDRAM_12, PHYS_SDRAM_12_SIZE);
	}else if (dram_size == 0x100000000){
		gd->ram_size	= get_ram_size((long *)PHYS_SDRAM_1, PHYS_SDRAM_1_SIZE)
						+ get_ram_size((long *)PHYS_SDRAM_2, PHYS_SDRAM_2_SIZE)
						+ get_ram_size((long *)PHYS_SDRAM_3, PHYS_SDRAM_3_SIZE)
						+ get_ram_size((long *)PHYS_SDRAM_4, PHYS_SDRAM_4_SIZE)
						+ get_ram_size((long *)PHYS_SDRAM_5, PHYS_SDRAM_5_SIZE)
						+ get_ram_size((long *)PHYS_SDRAM_6, PHYS_SDRAM_6_SIZE)
						+ get_ram_size((long *)PHYS_SDRAM_7, PHYS_SDRAM_7_SIZE)
						+ get_ram_size((long *)PHYS_SDRAM_8, PHYS_SDRAM_8_SIZE)
						+ get_ram_size((long *)PHYS_SDRAM_9, PHYS_SDRAM_9_SIZE)
						+ get_ram_size((long *)PHYS_SDRAM_10, PHYS_SDRAM_10_SIZE)
						+ get_ram_size((long *)PHYS_SDRAM_11, PHYS_SDRAM_11_SIZE)
						+ get_ram_size((long *)PHYS_SDRAM_12, PHYS_SDRAM_12_SIZE)
						+ get_ram_size((long *)PHYS_SDRAM_13, PHYS_SDRAM_13_SIZE)
						+ get_ram_size((long *)PHYS_SDRAM_14, PHYS_SDRAM_14_SIZE)
						+ get_ram_size((long *)PHYS_SDRAM_15, PHYS_SDRAM_15_SIZE)
						+ get_ram_size((long *)PHYS_SDRAM_16, PHYS_SDRAM_16_SIZE);
	}
	
	return 0;
}

void dram_init_banksize(void)
{
	unsigned long long dram_size = readq(CONFIG_PHY_BL_SYS_INFO_DRAM_SIZE);


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
	gd->bd->bi_dram[8].start = PHYS_SDRAM_9;
	gd->bd->bi_dram[8].size = get_ram_size((long *)PHYS_SDRAM_9,
							PHYS_SDRAM_9_SIZE);
	gd->bd->bi_dram[9].start = PHYS_SDRAM_10;
	gd->bd->bi_dram[9].size = get_ram_size((long *)PHYS_SDRAM_10,
							PHYS_SDRAM_10_SIZE);
	gd->bd->bi_dram[10].start = PHYS_SDRAM_11;
	gd->bd->bi_dram[10].size = get_ram_size((long *)PHYS_SDRAM_11,
							PHYS_SDRAM_11_SIZE);
	gd->bd->bi_dram[11].start = PHYS_SDRAM_12;
	gd->bd->bi_dram[11].size = get_ram_size((long *)PHYS_SDRAM_12,
							PHYS_SDRAM_12_SIZE);

	if (dram_size == 0x100000000){
		gd->bd->bi_dram[12].start = PHYS_SDRAM_13;
		gd->bd->bi_dram[12].size = get_ram_size((long *)PHYS_SDRAM_13,
								PHYS_SDRAM_13_SIZE);
		gd->bd->bi_dram[13].start = PHYS_SDRAM_14;
		gd->bd->bi_dram[13].size = get_ram_size((long *)PHYS_SDRAM_14,
								PHYS_SDRAM_14_SIZE);
		gd->bd->bi_dram[14].start = PHYS_SDRAM_15;
		gd->bd->bi_dram[14].size = get_ram_size((long *)PHYS_SDRAM_15,
								PHYS_SDRAM_15_SIZE);
		gd->bd->bi_dram[15].start = PHYS_SDRAM_16;
		gd->bd->bi_dram[15].size = get_ram_size((long *)PHYS_SDRAM_16,
								PHYS_SDRAM_16_SIZE);
	}else {
		gd->bd->bi_dram[12].start = 0;
		gd->bd->bi_dram[12].size = 0;
		gd->bd->bi_dram[13].start = 0;
		gd->bd->bi_dram[13].size = 0;
		gd->bd->bi_dram[14].start = 0;
		gd->bd->bi_dram[14].size = 0;
		gd->bd->bi_dram[15].start = 0;
		gd->bd->bi_dram[15].size = 0;
	}

	
}

int board_eth_init(bd_t *bis)
{
	return 0;
}

#ifdef CONFIG_DISPLAY_BOARDINFO
int checkboard(void)
{
	printf("\nAVL7420_BETA.0.1.1\n");
	printf("\nBoard: " CONFIG_BOARD_NAME "\n");

	return 0;
}
#endif

#ifdef CONFIG_GENERIC_MMC
int board_mmc_init(bd_t *bis)
{
	struct exynos7420_power *pmu = (struct exynos7420_power *)EXYNOS7420_POWER_BASE;
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
	struct exynos7420_power *pmu = (struct exynos7420_power *)EXYNOS7420_POWER_BASE;
	unsigned int om_status_temp;

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
	} 
#if SD_FIRM_IN
	else  
	{
#if	FIRM_IN_UFS
			om_status_temp = BOOT_UFS ;
			writel(BOOT_UFS,EXYNOS7420_POWER_BASE + INFORM3_OFFSET);
#else
			om_status_temp = om_status ;
#endif

			printf("###Recovery from SDcard.\n");
			if(om_status_temp == BOOT_UFS){
				setenv("bootcmd", CONFIG_BOOTCOMMAND2_UFS);
			}
			else {
				setenv("bootcmd", CONFIG_BOOTCOMMAND2);
			}
	}
#else	
	else {
#if 0
		if ((strncmp(commandline, CONFIG_FUSING_COMMAND_SDMMC, strlen(commandline)) == 0)
			|| (strncmp(commandline, CONFIG_FUSING_COMMAND_EMMC, strlen(commandline)) == 0)
			|| (strncmp(commandline, CONFIG_FUSING_COMMAND_UFS, strlen(commandline)) == 0) ) 
#endif
		if ((readl(&pmu->sysip_dat0)) != CONFIG_FACTORY_RESET_MODE)
		{
			if(om_status == BOOT_UFS){
				setenv("bootcmd", CONFIG_BOOTCOMMAND_UFS);
				saveenv();
			}
			else if(om_status == BOOT_EMMC_4_4){
				setenv("bootcmd", CONFIG_BOOTCOMMAND);
				saveenv();
			}
		}
		writel(0x0, &pmu->sysip_dat0);
	}
#endif
}

#ifdef CONFIG_OF_LIBFDT

#ifdef CONFIG_CHECK_BOOTMODE
#if 1
int check_usb(void)
{
	int vcell, soc, tmp;
	char usb_status = 0;
	exynos_iic_init();
	tmp = cw2015_ReadReg(0x02);
	vcell = (tmp << 8) + cw2015_ReadReg(0x03);
	vcell = vcell*305/1000;
	soc = cw2015_ReadReg(0x04);
	printf("vcell: %dmv, soc: %d%%\n", vcell, soc);
	if (bq24297_ReadReg(0x0a) == 0x60)
	{
		usb_status = bq24297_ReadReg(0x08);
		printf("usb_status: 0x%x\n", usb_status);
		if((usb_status & 0x40) || (usb_status & 0x80))
		{
			return 1;
		}
		else 
		{
			if (vcell < 3500)
			{
				printf("voltage is too low, shutdown!\n");
#if 0
				tmp = readl(0x105c330c);
				tmp &= ~(0x1<<8);
				writel(tmp, 0x105c330c);
#endif
			}
			return 0;
		}
	
	}
	else
		return 0;

	
}
#endif
int check_source_boot(void)
{
	struct exynos7420_power *pmu = (struct exynos7420_power *)EXYNOS7420_POWER_BASE;
	unsigned char power_on_src = 0;
	unsigned int rst_status = 0;
	unsigned int inform_val = 0;
	char reset_type =0 ;
	//read the pmic boot source
	IIC0_ERead(S2MPS15_ADDR, PWRONSRC, &power_on_src);
	printf("power_on_src: 0x%x\n", power_on_src);
	//read the AP boot source
	rst_status = readl(&pmu->rst_stat);
	printf("rst_stat : 0x%x\n", rst_status);
	inform_val = readl(&pmu->inform2);
//	printf("inform_val : 0x%x\n", inform_val);


	if(inform_val == PHONE_REBOOT)
	{
		reset_type = software_reset;

	}
#if 1
	else if ((inform_val==0)&&(check_usb_status==1))
	{
			reset_type = charge_reset;
	}
#endif
	else
	{
		if(rst_status & (0x01<<29))
			reset_type = software_reset;
		else if (rst_status & (0x01<<20))
			reset_type = watchdog_reset;
		else if (rst_status & (0x01<<16))
		{
			if (power_on_src == 0x10)
				reset_type = alarm_reset;
			else if (power_on_src == 0x08) 
				reset_type = hardware_reset;
			else if (power_on_src == 0x01 || power_on_src == 0x02)
			{
				//need to power off
				reset_type = normal_reset;
			}
			else if (power_on_src == 0x40)
				reset_type = smpl_reset;
			else
			{
				// power_on_src == 1
				reset_type = unknown_reset;
			}
		}
	}
	if (reset_type == charge_reset)
	{
		writel(POWER_OFF_CHARGE, &pmu->inform2);
	}
	else
	{
		writel(0,&pmu->inform2);
	}
	inform_val = readl(&pmu->inform2);
//	printf("inform_val : 0x%x\n", inform_val);
	return reset_type;

	
}

#endif
char result_apdebug[10] = "on";
extern int apdebug_changed;
extern int g_testmode;
static configure_uart_dtb(void)
{
	char str[256];
	
	switch (g_testmode)
	{
		case 1:
		{
			sprintf(str, "fdt set /uart@13630000 status %s",  "okay");
			run_command(str, 0);
			sprintf(str, "fdt set /fiq_debugger2 status %s",  "disa");
			run_command(str, 0);
			break;
		}
		case 0:
		{
			sprintf(str, "fdt set /fiq_debugger2 status %s",  "okay");
			run_command(str, 0);
			sprintf(str, "fdt set /uart@13630000 status %s",  "disa");
			run_command(str, 0);
			break;
		}
		default:
			break;
	}
	
}
#ifdef LCD_TOUCHPANEL_COMPATIBLE
static void configure_lcd_touchpanel_dtb(void)
{
	char str[256];
	
	switch (lcd_type_avl7420)
	{
		case s6e8aa0_720p_video:
		{
			sprintf(str, "fdt set /dsim@0x13900000/s6e8aa0_hd status %s",  "okay");
			run_command(str, 0);
			sprintf(str, "fdt set /dsim@0x13900000/s6e8fa0_fhd status %s",  "disa");
			run_command(str, 0);
			sprintf(str, "fdt set /gpioi2c0/touchscreen@48 status %s",  "okay");
			run_command(str, 0);
			sprintf(str, "fdt set /hsi2c@14E10000/touchscreen@4a status %s",  "disa");
			run_command(str, 0);
			sprintf(str, "fdt set /hsi2c@14E10000 status %s",  "disa");
			run_command(str, 0);
			sprintf(str, "fdt set /gpioi2c0/ status %s",  "okay");
			run_command(str, 0);
			break;
		}
		case s6e8fa0_1080p_video:
		{
			sprintf(str, "fdt set /dsim@0x13900000/s6e8fa0_fhd status %s",  "okay");
			run_command(str, 0);
			sprintf(str, "fdt set /dsim@0x13900000/s6e8aa0_hd status %s",  "disa");
			run_command(str, 0);		
			sprintf(str, "fdt set /hsi2c@14E10000/touchscreen@4a status %s",  "okay");
			run_command(str, 0);
			sprintf(str, "fdt set /gpioi2c0/touchscreen@48 status %s",  "disa");
			run_command(str, 0);
			sprintf(str, "fdt set /gpioi2c0/ status %s",  "disa");
			run_command(str, 0);
			sprintf(str, "fdt set /hsi2c@14E10000 status %s",  "okay");
			run_command(str, 0);	
			break;
		}
		default:
			break;
	}
}
#endif
static void configure_dtb(ulong start , ulong end )
{
	char str[256];
#ifdef CONFIG_CHECK_BOOTMODE
	char bootmode[8];
#endif
	char *rootfs_len = getenv("rootfslen");
	ulong rd_len = simple_strtoul(rootfs_len, NULL, 16);
	unsigned long long dram_size = readq(CONFIG_PHY_BL_SYS_INFO_DRAM_SIZE);

	unsigned int total = 0;

	if (dram_size = 0xC0000000){	
		total = \
				PHYS_SDRAM_1_SIZE + PHYS_SDRAM_2_SIZE + PHYS_SDRAM_3_SIZE +
				PHYS_SDRAM_4_SIZE + PHYS_SDRAM_5_SIZE + PHYS_SDRAM_6_SIZE +
				PHYS_SDRAM_7_SIZE + PHYS_SDRAM_8_SIZE + PHYS_SDRAM_9_SIZE +
				PHYS_SDRAM_10_SIZE + PHYS_SDRAM_11_SIZE + PHYS_SDRAM_12_SIZE;
	}else if (dram_size == 0x100000000){
		total = \
				PHYS_SDRAM_1_SIZE + PHYS_SDRAM_2_SIZE + PHYS_SDRAM_3_SIZE +
				PHYS_SDRAM_4_SIZE + PHYS_SDRAM_5_SIZE + PHYS_SDRAM_6_SIZE +
				PHYS_SDRAM_7_SIZE + PHYS_SDRAM_8_SIZE + PHYS_SDRAM_9_SIZE +
				PHYS_SDRAM_10_SIZE + PHYS_SDRAM_11_SIZE + PHYS_SDRAM_12_SIZE +
				PHYS_SDRAM_13_SIZE + PHYS_SDRAM_14_SIZE + PHYS_SDRAM_15_SIZE + 	PHYS_SDRAM_16_SIZE;
	}

//BABIS: Removed fdt set memory!
//	sprintf(str, "fdt set /memory reg <0x0 0x%x 0x0 0x%x>",CONFIG_SYS_SDRAM_BASE, total);
//	run_command(str,0);	
//	printf("[FDT] : %s\n", str);

	sprintf(str, "fdt set /chosen linux,initrd-start <0x%x>", start);
	run_command(str, 0);
	printf("[FDT] : %s\n", str);
	sprintf(str, "fdt set /chosen linux,initrd-end <0x%x>", end);
	run_command(str, 0);
	printf("[FDT] : %s\n", str);
#ifdef CONFIG_CHECK_BOOTMODE
	switch (check_source_boot())
	{
		case normal_reset:
			strcpy(bootmode , "normal");	
			break;
		case charge_reset:
			strcpy(bootmode , "charger");
			break;
		case alarm_reset:
			strcpy(bootmode , "alarm");
			break;
		case software_reset:
			strcpy(bootmode , "software");
			break;
		case hardware_reset:
			strcpy(bootmode , "hardware");
			break;
		case watchdog_reset:
			strcpy(bootmode , "watchdog");
			break;
		case smpl_reset:
			strcpy(bootmode , "smpl_reset");
			break;
		default:
			strcpy(bootmode , "unknown");
			break;
	}
	if(!apdebug_changed) {
		run_command("fdt print /chosen bootargs", 0);
	}
	if(g_testmode == 1)
		sprintf(str, "fdt set /chosen bootargs console=ttyS60,115200n8 ess_setup=0x46000000 androidboot.mode=%s decon.bootloaderfb=0xE0000000,0x00800000 apdebug_onoff=%s testmode=%d earlyprintk=exynos,0x13630000 androidboot.hardware=samsungexynos7420",bootmode,result_apdebug,g_testmode);
	else//for engmode uart 
		sprintf(str, "fdt set /chosen bootargs console=ttySAC0,115200n8 ess_setup=0x46000000 androidboot.mode=%s decon.bootloaderfb=0xE0000000,0x00800000 apdebug_onoff=%s testmode=%d earlyprintk=exynos,0x13630000 androidboot.hardware=samsungexynos7420",bootmode,result_apdebug,g_testmode);
	run_command(str, 0);
	printf("[FDT] : %s\n", str);
#endif
#ifdef LCD_TOUCHPANEL_COMPATIBLE
	configure_lcd_touchpanel_dtb();
#endif
	configure_uart_dtb();
}
static void configure_dtb_initrd(ulong start , ulong end )
{
	char str[256];
	sprintf(str, "fdt set /chosen linux,initrd-start <0x%x>", start);
	run_command(str, 0);
	printf("[FDT] : %s\n", str);

	sprintf(str, "fdt set /chosen linux,initrd-end <0x%x>", end);
	run_command(str, 0);
	printf("[FDT] : %s\n", str);

}

void prepare_dtb(ulong start , ulong end )
{
	unsigned int om_status = readl(EXYNOS7420_POWER_BASE + INFORM3_OFFSET);

	if(*(unsigned int *)CONFIG_SYS_SDRAM_BASE == 0xabcdef) {
		switch(om_status) {
		case BOOT_MMCSD:
		case BOOT_SEC_DEV:
			run_command(CONFIG_FDT_LOAD_COMMAND_SDMMC, 0);
			//BABIS: Removed configure dtb
			//configure_dtb(start,end);
//			run_command(CONFIG_FDT_SAVE_COMMAND_SDMMC, 0);
			break;
		case BOOT_EMMC_4_4:
			run_command(CONFIG_FDT_LOAD_COMMAND_EMMC, 0);
			configure_dtb(start,end);
//			run_command(CONFIG_FDT_SAVE_COMMAND_EMMC, 0);
			break;
		case BOOT_UFS:
			run_command(CONFIG_FDT_LOAD_COMMAND_UFS, 0);
			configure_dtb(start,end);
//			run_command(CONFIG_FDT_SAVE_COMMAND_UFS, 0);
			break;
		default:
			break;
		}
	}
}
void prepare_dtb_initrd(ulong start , ulong end )
{
	unsigned int om_status = readl(EXYNOS7420_POWER_BASE + INFORM3_OFFSET);
	if(*(unsigned int *)CONFIG_SYS_SDRAM_BASE == 0xabcdef) {
		switch(om_status) {
		case BOOT_MMCSD:
		case BOOT_SEC_DEV:
			run_command(CONFIG_FDT_LOAD_COMMAND_SDMMC, 0);
			configure_dtb_initrd(start,end);
//			run_command(CONFIG_FDT_SAVE_COMMAND_SDMMC, 0);
			break;
		case BOOT_EMMC_4_4:
			run_command(CONFIG_FDT_LOAD_COMMAND_EMMC, 0);
			configure_dtb_initrd(start,end);
//			run_command(CONFIG_FDT_SAVE_COMMAND_EMMC, 0);
			break;
		case BOOT_UFS:
			run_command(CONFIG_FDT_LOAD_COMMAND_UFS, 0);
			configure_dtb_initrd(start,end);
//			run_command(CONFIG_FDT_SAVE_COMMAND_UFS, 0);
			break;
		default:
			break;
		}
	}
}

#endif

void muic_setup_for_usb(void)
{

}

int board_late_init(void)
{
#ifndef CONFIG_SG_BINARY
	struct exynos7420_power *pmu = (struct exynos7420_power *)EXYNOS7420_POWER_BASE;
	unsigned int om_status = readl(EXYNOS7420_POWER_BASE + INFORM3_OFFSET);
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
	IIC0_ERead(S2MPS15_ADDR, S2MPS15_INT1, &read_int1);
	IIC0_ERead(S2MPS15_ADDR, S2MPS15_INT2, &read_int2);
	IIC0_ERead(S2MPS15_ADDR, PWRONSRC, &read_pwronsrc);
	/* to clear OFFSRC */
	IIC0_EWrite(S2MPS15_ADDR, OFFSRC, 0x0);
	/* read RTC */
	IIC0_ERead(S2MPS15_RTC, RTC_WTSR_SMPL, &read_wtsr_smpl);

	dfd_display_reboot_reason();
	dfd_display_core_stat();

	if ((read_pwronsrc & (1 << 7)) && (read_int2 & (1 << 5))
	/* WTSR detect condition - WTSR_ON && WTSR_INT && ! MRB_INT */
		&& !(read_int1 & (1 << 7))) {
		printf("S2MPS15_INT1: 0x%x\n", read_int1);
		printf("S2MPS15_INT2: 0x%x\n", read_int2);
		printf("PWRONSRC: 0x%x\n", read_pwronsrc);
		printf("WTSR detected\n");
		muic_setup_for_usb();
		run_command("fastboot", 0);
	} else if ((read_pwronsrc & (1 << 6)) && (read_int2 & (1 << 3))
	/* SMPL detect condition - SMPL_ON && SMPL_INT && SMPL_EN */
                && (read_wtsr_smpl & (1 << 7))) {
		printf("S2MPS15_INT2: 0x%x\n", read_int2);
		printf("PWRONSRC: 0x%x\n", read_pwronsrc);
		printf("S2MPS15_RTC_WTSR_SMPL: 0x%x\n", read_wtsr_smpl);
		printf("SMPL detected\n");
		muic_setup_for_usb();
                run_command("fastboot", 0);
	} else if ((rst_stat & (WRESET | APOLLO_WDTRESET | ATLAS_WDTRESET))) {
	/* check reset status for ramdump */
		dfd_run_dump_gpr(EXYNOS7420_POWER_BASE);
		muic_setup_for_usb();
		run_command("fastboot", 0);
	} else if ((readl(&pmu->sysip_dat0) == CONFIG_RAMDUMP_MODE)
		|| (readl(CONFIG_RAMDUMP_SCRATCH) == CONFIG_RAMDUMP_MODE)) {
		muic_setup_for_usb();
		run_command("fastboot", 0);
	}
	/* Set enable to dump_grp function */
	dfd_set_dump_gpr(EXYNOS7420_POWER_BASE, 1);
#endif
#ifdef CONFIG_RECOVERY_MODE
	u32 second_boot_info = readl(second_boot_info_base);
	if (second_boot == 1)
		printf("###Recovery Mode###\n");
#ifdef CONFIG_EXYNOS_DA
	second_boot_info = readl(second_boot_info_base);

	if (second_boot_info == 2) {
		printf("###USB Download### om:%d\n", readl(&pmu->inform3));
#ifndef FAST_ENTER_FASTBOOT 
		writel(0x0, second_boot_info_base);
#endif

#if FIRM_IN_UFS
		writel(BOOT_UFS, &pmu->inform3);
                run_command("gpt write scsi 0", 0); // make partition
#else
		writel(BOOT_EMMC_4_4, &pmu->inform3);
        	run_command("gpt write mmc 0", 0); // make partition
#endif 
	setenv("bootcmd", "dnw v05");
		printf("###USB Download### om:%d\n", readl(&pmu->inform3));
		om_status = readl(EXYNOS7420_POWER_BASE + INFORM3_OFFSET);
		writel(0x0, &pmu->sysip_dat0);
		return 0;
	}
#endif
#endif

	if ((readl(&pmu->sysip_dat0)) == CONFIG_FACTORY_RESET_MODE) {
		//writel(0x0, &pmu->sysip_dat0);
		if(om_status == BOOT_UFS){
			setenv("bootcmd", CONFIG_FACTORY_RESET_BOOTCOMMAND_UFS);
		}
		else {
			setenv("bootcmd", CONFIG_FACTORY_RESET_BOOTCOMMAND);
		}
	}

	configure_auto_boot(om_status);

	return 0;
#endif
}

#ifdef CONFIG_PREP_BOARDCONFIG_FOR_KERNEL
void prep_boardconfig_for_kernel(void)
{
	/* power on domain */
	writel(0x0000000f, EXYNOS7420_POWER_CAM0_CONFIGURATION);
	while((readl(EXYNOS7420_POWER_CAM0_CONFIGURATION + 0x4) & 0xF) != 0xF);
	clk_init_cam0();

	writel(0x0000000f, EXYNOS7420_POWER_CAM1_CONFIGURATION);
	while((readl(EXYNOS7420_POWER_CAM1_CONFIGURATION + 0x4) & 0xF) != 0xF);
	clk_init_cam1();

	writel(0x0000000f, EXYNOS7420_POWER_ISP0_CONFIGURATION);
	while((readl(EXYNOS7420_POWER_ISP0_CONFIGURATION + 0x4) & 0xF) != 0xF);
	clk_init_isp0();

	writel(0x0000000f, EXYNOS7420_POWER_ISP1_CONFIGURATION);
	while((readl(EXYNOS7420_POWER_ISP1_CONFIGURATION + 0x4) & 0xF) != 0xF);
	clk_init_isp1();

	writel(0x0000000f, EXYNOS7420_POWER_MFC_CONFIGURATION);
	while((readl(EXYNOS7420_POWER_MFC_CONFIGURATION + 0x4) & 0xF) != 0xF);
	clk_init_mfc();

	writel(0x0000000f, EXYNOS7420_POWER_MSCL_CONFIGURATION);
	while((readl(EXYNOS7420_POWER_MSCL_CONFIGURATION + 0x4) & 0xF) != 0xF);
	clk_init_mscl();

	writel(0x0000000f, EXYNOS7420_POWER_VPP_CONFIGURATION);
	while((readl(EXYNOS7420_POWER_VPP_CONFIGURATION + 0x4) & 0xF) != 0xF);
	clk_init_vpp();
}
#endif

unsigned int get_board_rev(void)
{
	unsigned int rev = 0;

	return rev;
}
