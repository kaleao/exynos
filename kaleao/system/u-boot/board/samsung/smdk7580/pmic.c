/*
 * (C) Copyright 2014 Samsung Electronics Co. Ltd
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <common.h>
#include <asm/arch/cpu.h>
#include <asm/arch/smc.h>
#include "pmic.h"

#define ASV_TBL_ADDR_BASE	(0x101E0160)

#ifndef CONFIG_SPL_BUILD
u32 uBigAsvGrpVolt;
u32 uLitAsvGrpVolt;
u32 uG3dAsvGrpVolt;
u32 uMifAsvGrpVolt;
u32 uIntAsvGrpVolt;
u32 uDisAsvGrpVolt;
#endif
static unsigned char pmic_id;
u32 asv_efuse = 0;


void Delay(void)
{
	unsigned long i;
	for (i = 0; i < DELAY; i++)
		;
}

void IIC0_SCLH_SDAH(void)
{
	IIC0_ESCL_Hi;
	IIC0_ESDA_Hi;
	Delay();
}

void IIC0_SCLH_SDAL(void)
{
	IIC0_ESCL_Hi;
	IIC0_ESDA_Lo;
	Delay();
}

void IIC0_SCLL_SDAH(void)
{
	IIC0_ESCL_Lo;
	IIC0_ESDA_Hi;
	Delay();
}

void IIC0_SCLL_SDAL(void)
{
	IIC0_ESCL_Lo;
	IIC0_ESDA_Lo;
	Delay();
}

void IIC0_ELow(void)
{
	IIC0_SCLL_SDAL();
	IIC0_SCLH_SDAL();
	IIC0_SCLH_SDAL();
	IIC0_SCLL_SDAL();
}

void IIC0_EHigh(void)
{
	IIC0_SCLL_SDAH();
	IIC0_SCLH_SDAH();
	IIC0_SCLH_SDAH();
	IIC0_SCLL_SDAH();
}

void IIC0_EStart(void)
{
	IIC0_SCLH_SDAH();
	IIC0_SCLH_SDAL();
	Delay();
	IIC0_SCLL_SDAL();
}

void IIC0_EEnd(void)
{
	IIC0_SCLL_SDAL();
	IIC0_SCLH_SDAL();
	Delay();
	IIC0_SCLH_SDAH();
}

void IIC0_EAck_write(void)
{
	unsigned long ack;

	/* Function <- Input */
	IIC0_ESDA_INP;

	IIC0_ESCL_Lo;
	Delay();
	IIC0_ESCL_Hi;
	Delay();
	ack = GPIO_DAT;
	IIC0_ESCL_Hi;
	Delay();
	IIC0_ESCL_Hi;
	Delay();

	/* Function <- Output (SDA) */
	IIC0_ESDA_OUTP;

	ack = (ack >> GPIO_DAT_SHIFT) & 0x1;

	IIC0_SCLL_SDAL();
}

void IIC0_EAck_read(void)
{
	/* Function <- Output */
	IIC0_ESDA_OUTP;

	IIC0_ESCL_Lo;
	IIC0_ESCL_Lo;
	IIC0_ESDA_Hi;
	IIC0_ESCL_Hi;
	IIC0_ESCL_Hi;
	/* Function <- Input (SDA) */
	IIC0_ESDA_INP;

	IIC0_SCLL_SDAL();
}

void IIC0_ESetport(void)
{
	/* Pull Up/Down Disable	SCL, SDA */
	DIS_GPIO_PUD;

	IIC0_ESCL_Hi;
	IIC0_ESDA_Hi;

	/* Function <- Output (SCL) */
	IIC0_ESCL_OUTP;
	/* Function <- Output (SDA) */
	IIC0_ESDA_OUTP;

	Delay();
}

void IIC0_EWrite(unsigned char ChipId,
		unsigned char IicAddr, unsigned char IicData)
{
	unsigned long i;

	IIC0_EStart();

	/* write chip id */
	for (i = 7; i > 0; i--) {
		if ((ChipId >> i) & 0x0001)
			IIC0_EHigh();
		else
			IIC0_ELow();
	}

	/* write */
	IIC0_ELow();

	/* ACK */
	IIC0_EAck_write();

	/* write reg. addr. */
	for (i = 8; i > 0; i--) {
		if ((IicAddr >> (i-1)) & 0x0001)
			IIC0_EHigh();
		else
			IIC0_ELow();
	}

	/* ACK */
	IIC0_EAck_write();

	/* write reg. data. */
	for (i = 8; i > 0; i--) {
		if ((IicData >> (i-1)) & 0x0001)
			IIC0_EHigh();
		else
			IIC0_ELow();
	}

	/* ACK */
	IIC0_EAck_write();

	IIC0_EEnd();
}

void IIC0_ERead(unsigned char ChipId,
		unsigned char IicAddr, unsigned char *IicData)
{
	unsigned long i, reg;
	unsigned char data = 0;

	IIC0_EStart();

	/* write chip id */
	for (i = 7; i > 0; i--) {
		if ((ChipId >> i) & 0x0001)
			IIC0_EHigh();
		else
			IIC0_ELow();
	}

	/* write */
	IIC0_ELow();

	/* ACK */
	IIC0_EAck_write();

	/* write reg. addr. */
	for (i = 8; i > 0; i--) {
		if ((IicAddr >> (i-1)) & 0x0001)
			IIC0_EHigh();
		else
			IIC0_ELow();
	}

	/* ACK */
	IIC0_EAck_write();

	IIC0_EStart();

	/* write chip id */
	for (i = 7; i > 0; i--) {
		if ((ChipId >> i) & 0x0001)
			IIC0_EHigh();
		else
			IIC0_ELow();
	}

	/* read */
	IIC0_EHigh();
	/* ACK */
	IIC0_EAck_write();

	/* read reg. data. */
	IIC0_ESDA_INP;

	IIC0_ESCL_Lo;
	IIC0_ESCL_Lo;
	Delay();

	for (i = 8; i > 0; i--) {
		IIC0_ESCL_Lo;
		IIC0_ESCL_Lo;
		Delay();
		IIC0_ESCL_Hi;
		IIC0_ESCL_Hi;
		Delay();
		reg = GPIO_DAT;
		IIC0_ESCL_Hi;
		IIC0_ESCL_Hi;
		Delay();
		IIC0_ESCL_Lo;
		IIC0_ESCL_Lo;
		Delay();

		reg = (reg >> GPIO_DAT_SHIFT) & 0x1;

		data |= reg << (i-1);
	}

	/* ACK */
	IIC0_EAck_read();
	IIC0_ESDA_OUTP;

	IIC0_EEnd();

	*IicData = data;
}

#ifndef CONFIG_EMULATOR
void get_asv_efuse(void)
{
	u64 tmp = 0;

	tmp = exynos_smc_read(SMC_CMD_REG, SMC_REG_ID_SFR_R((unsigned long) ASV_TBL_ADDR_BASE + 0xC));
	asv_efuse = (u32)tmp;
}
#endif

void pmic_lcd_tsp(void)
{
	unsigned char ldo_ctrl = 0;

	IIC0_ESetport();

	/* enable LDO30 : VCC_LCD_3V0 */
	IIC0_ERead(S2MPU03_ADDR, LDO30_CTRL, &ldo_ctrl);
	ldo_ctrl |= OUTPUT_PWREN_ON;
	IIC0_EWrite(S2MPU03_ADDR, LDO30_CTRL, ldo_ctrl);

#ifdef CONFIG_MACH_SMDK7580
	/* enable LDO31 : VCC_LCD_1V8 */
	IIC0_ERead(S2MPU03_ADDR, LDO31_CTRL, &ldo_ctrl);
	ldo_ctrl |= OUTPUT_PWREN_ON;
	IIC0_EWrite(S2MPU03_ADDR, LDO31_CTRL, ldo_ctrl);
#endif

	/* enable LDO33 : TSP_AVDD_3V3 */
	IIC0_ERead(S2MPU03_ADDR, LDO33_CTRL, &ldo_ctrl);
	ldo_ctrl |= OUTPUT_PWREN_ON;
	IIC0_EWrite(S2MPU03_ADDR, LDO33_CTRL, ldo_ctrl);

#ifdef CONFIG_MACH_SMDK7580
	/* enable LDO36 : TSP_AVDD_1V8 */
	IIC0_ERead(S2MPU03_ADDR, LDO36_CTRL, &ldo_ctrl);
	ldo_ctrl |= OUTPUT_PWREN_ON;
	IIC0_EWrite(S2MPU03_ADDR, LDO36_CTRL, ldo_ctrl);
#endif
}

#ifndef CONFIG_SPL_BUILD
#ifdef CONFIG_MACH_SMDK7580
void pmic_audio_gps_enable(void)
{
	unsigned char ldo_ctrl = 0;

	IIC0_ESetport();

	/* enable LDO23 : TSP_AUDIO2_1V8 */
	IIC0_ERead(S2MPU03_ADDR, LDO23_CTRL, &ldo_ctrl);
	ldo_ctrl |= OUTPUT_NORMAL_MODE;
	IIC0_EWrite(S2MPU03_ADDR, LDO23_CTRL, ldo_ctrl);

	/* enable LDO38 : TSP_GPS_3V3 */
	IIC0_ERead(S2MPU03_ADDR, LDO38_CTRL, &ldo_ctrl);
	ldo_ctrl |= OUTPUT_NORMAL_MODE;
	IIC0_EWrite(S2MPU03_ADDR, LDO38_CTRL, ldo_ctrl);

	/* enable LDO39 : TSP_GPS_1V8 */
	IIC0_ERead(S2MPU03_ADDR, LDO39_CTRL, &ldo_ctrl);
	ldo_ctrl |= OUTPUT_NORMAL_MODE;
	IIC0_EWrite(S2MPU03_ADDR, LDO39_CTRL, ldo_ctrl);
}
#endif
#endif

void pmic_init(void)
{
/* comment out for Insel-D */
#if 0
	IIC0_ESetport();

	/* LDO offset 0V */
	if (CONFIG_PM_VDD_ATLAS >= 1000)
		IIC0_EWrite(S2MPU03_ADDR, LDO_DVS1, 0x84);
	if (CONFIG_PM_VDD_INT >= 1000)
		IIC0_EWrite(S2MPU03_ADDR, LDO_DVS2, 0x84);
	if (CONFIG_PM_VDD_APOLLO >= 1000)
		IIC0_EWrite(S2MPU03_ADDR, LDO_DVS3, 0x84);
	if (CONFIG_PM_VDD_G3D >= 1000)
		IIC0_EWrite(S2MPU03_ADDR, LDO_DVS4, 0x84);

	/* BUCK Control (EVT0) */
	IIC0_EWrite(S2MPU03_ADDR, BUCK1_OUT,
		WR_BUCK_VOLT(CONFIG_PM_VDD_MIF));
	IIC0_EWrite(S2MPU03_ADDR, BUCK2_OUT,
		WR_BUCK_VOLT(CONFIG_PM_VDD_ATLAS));
	IIC0_EWrite(S2MPU03_ADDR, BUCK3_OUT,
		WR_BUCK_VOLT(CONFIG_PM_VDD_APOLLO));
	IIC0_EWrite(S2MPU03_ADDR, BUCK4_OUT,
		WR_BUCK_VOLT(CONFIG_PM_VDD_INT));
	IIC0_EWrite(S2MPU03_ADDR, BUCK5_OUT,
		WR_BUCK_VOLT(CONFIG_PM_VDD_DISP));
	IIC0_EWrite(S2MPU03_ADDR, BUCK6_OUT,
		WR_BUCK_VOLT(CONFIG_PM_VDD_G3D));
#endif
}

#ifndef CONFIG_EMULATOR
#ifndef CONFIG_SPL_BUILD
void pmic_asv_init(void)
{
	/* MIF */
	IIC0_EWrite(S2MPU03_ADDR, BUCK1_OUT,
		WR_BUCK_VOLT_EVT1((float)uMifAsvGrpVolt/1000));
	/* EGL */
	IIC0_EWrite(S2MPU03_ADDR, BUCK2_OUT,
		WR_BUCK_VOLT_EVT1((float)uBigAsvGrpVolt/1000));
	/* KFC */
	IIC0_EWrite(S2MPU03_ADDR, BUCK3_OUT,
		WR_BUCK_VOLT_EVT1((float)uLitAsvGrpVolt/1000));
	/* INT */
	IIC0_EWrite(S2MPU03_ADDR, BUCK4_OUT,
		WR_BUCK_VOLT_EVT1((float)uIntAsvGrpVolt/1000));
	/* CAM0_DISP */
	IIC0_EWrite(S2MPU03_ADDR, BUCK5_OUT,
		WR_BUCK_VOLT_EVT1((float)uDisAsvGrpVolt/1000));
	/* G3D */
	IIC0_EWrite(S2MPU03_ADDR, BUCK6_OUT,
		WR_BUCK_VOLT_EVT1((float)uG3dAsvGrpVolt/1000));
}

void Get_ASV_Group(void)
{
	cal_init();

	/* get voltage from table */
	uBigAsvGrpVolt = cal_get_volt(SYSC_DVFS_BIG, 13);
	uLitAsvGrpVolt = cal_get_volt(SYSC_DVFS_LIT, 8);
	uG3dAsvGrpVolt = cal_get_volt(SYSC_DVFS_G3D, 7);
	asv_efuse = (asv_efuse & 0xf);
	if ((asv_efuse <= 1) || (asv_efuse == 4))	/* 1264 MHz */
		uMifAsvGrpVolt = cal_get_volt(SYSC_DVFS_MIF, 2);
	else if ((asv_efuse == 2) || (asv_efuse == 3))	/* 1552MHz */
		uMifAsvGrpVolt = cal_get_volt(SYSC_DVFS_MIF, 0);
	uIntAsvGrpVolt = cal_get_volt(SYSC_DVFS_INT, 0);
	uDisAsvGrpVolt = cal_get_volt(SYSC_DVFS_CAM, 0);
}
#endif
#endif

void pmic_enable_peric_dev(void)
{
	unsigned char ldo_ctrl;
	unsigned char rtc_ctrl;
	unsigned char wrstbi_ctrl;
	unsigned char mrstb_ctrl;
	unsigned char wrstb_ctrl;

	IIC0_ESetport();

	/* read pmic revision number */
	IIC0_ERead(S2MPU03_ADDR, S2MPU03_PMIC_ID, &pmic_id);

	/* enable WRSTBI detection */
	IIC0_ERead(S2MPU03_ADDR, WRSTBI_CTRL, &wrstbi_ctrl);
	wrstbi_ctrl |= WRSTBI_EN;
	IIC0_EWrite(S2MPU03_ADDR, WRSTBI_CTRL, wrstbi_ctrl);

	/* CP 32KHz On, AP 32KHz On */
	IIC0_ERead(S2MPU03_ADDR, RTC_BUF, &rtc_ctrl);
	rtc_ctrl |= (CP32KHZ_EN | AP32KHZ_EN);
	IIC0_EWrite(S2MPU03_ADDR, RTC_BUF, rtc_ctrl);

	/* Manual Reset Disable for Warm Reset Enahle */
	IIC0_ERead(S2MPU03_ADDR, MRSTB, &mrstb_ctrl);
	mrstb_ctrl &= (~MRSTB_EN);
	IIC0_EWrite(S2MPU03_ADDR, MRSTB, mrstb_ctrl);

	/* Warm Reset Enable */
	IIC0_ERead(S2MPU03_ADDR, WRSTBI_CTRL, &wrstb_ctrl);
	wrstb_ctrl |= WRST_EN;
	IIC0_EWrite(S2MPU03_ADDR, WRSTBI_CTRL, wrstb_ctrl);

	/* comment out for Insel-D */
#if 0
	/* power control for SERIAL */
	/* enable LDO14 : VCC_2.8V_PERI */
	IIC0_ERead(S2MPU03_ADDR, LDO14_CTRL, &ldo_ctrl);
	ldo_ctrl |= OUTPUT_PWREN_ON;
	IIC0_EWrite(S2MPU03_ADDR, LDO14_CTRL, ldo_ctrl);

	/* enable LDO15 : VCC_1.8V_PERI */
	IIC0_ERead(S2MPU03_ADDR, LDO15_CTRL, &ldo_ctrl);
	ldo_ctrl |= OUTPUT_PWREN_ON;
	IIC0_EWrite(S2MPU03_ADDR, LDO15_CTRL, ldo_ctrl);

	/* enable LDO17 : VCC_2.8V_PERI */
	IIC0_ERead(S2MPU03_ADDR, LDO17_CTRL, &ldo_ctrl);
	ldo_ctrl |= OUTPUT_PWREN_ON;
	IIC0_EWrite(S2MPU03_ADDR, LDO17_CTRL, ldo_ctrl);
#endif
}
