/*
 * (C) Copyright 2013 Samsung Electronics Co. Ltd
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
#include "pmic.h"

#define Inp32(addr) (*(volatile u32 *)(addr))
#define GetBits(uAddr, uBaseBit, uMaskValue) \
    ((Inp32(uAddr)>>(uBaseBit))&(uMaskValue))

#define APOLLO_SRAM_DELTA      0x1000402C

extern u32 uEglAsvGrpVolt;
extern u32 uKfcAsvGrpVolt;
extern u32 uMifAsvGrpVolt;
extern u32 uIntAsvGrpVolt;
extern u32 uG3dAsvGrpVolt;
#ifdef CONFIG_CPU_EXYNOS5433
extern u32 uCamAsvGrpVolt;
#endif

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
	ack = GPD2DAT;
	IIC0_ESCL_Hi;
	Delay();
	IIC0_ESCL_Hi;
	Delay();

	/* Function <- Output (SDA) */
	IIC0_ESDA_OUTP;

	ack = (ack>>0)&0x1;

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
	GPD2PUD &= ~(0xf<<0);

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
		reg = GPD2DAT;
		IIC0_ESCL_Hi;
		IIC0_ESCL_Hi;
		Delay();
		IIC0_ESCL_Lo;
		IIC0_ESCL_Lo;
		Delay();

		reg = (reg >> 0) & 0x1;

		data |= reg << (i-1);
	}

	/* ACK */
	IIC0_EAck_read();
	IIC0_ESDA_OUTP;

	IIC0_EEnd();

	*IicData = data;
}

void pmic_init(void)
{
	unsigned char pmic_id;
	unsigned char rtc_ctrl;
	unsigned char wrstbi_ctrl;
	unsigned char ldo_ctrl;
	unsigned char buck6_ctrl;

#ifdef CONFIG_CPU_EXYNOS5433
	/* set GPF5[4] output high - AP Warm Reset Detect GPIO Setting: prevent PMIC reset */
	*(unsigned int *)0x110900A0 &= ~0xF0000;
	*(unsigned int *)0x110900A0 |= 0x10000;
	*(unsigned int *)0x110900A4 |= 0x10;
#endif
#ifndef CONFIG_CPU_EXYNOS5433_EVT1
	Get_ASV_Group();
#endif

	IIC0_ESetport();

	/* read pmic revision number */
	IIC0_ERead(S2MPS13_ADDR, PMIC_ID, &pmic_id);

	/* LDO DVS VTHR control */
	/* set INT LDO DVS VTHR : 0.9V */
	IIC0_EWrite(S2MPS13_ADDR, LDO_DVS1, 0xC8);
#ifdef CONFIG_CPU_EXYNOS5433
	u32 delta = GetBits(APOLLO_SRAM_DELTA, 1, 0x3);
	if (delta == 0x00)
		/* set KFC LDO DVS VTHR : 1.000V */
		IIC0_EWrite(S2MPS13_ADDR, LDO_DVS2, 0xDC);
	else if (delta == 0x01)
		/* set KFC LDO DVS VTHR : 0.975V */
		IIC0_EWrite(S2MPS13_ADDR, LDO_DVS2, 0xD4);
	else
		/* set KFC LDO DVS VTHR : 0.950V */
		IIC0_EWrite(S2MPS13_ADDR, LDO_DVS2, 0xCC);

	/* set KFC LDO DVS VTHR : 0.975V */
	IIC0_EWrite(S2MPS13_ADDR, LDO_DVS2, 0xDC);
#else
	/* set KFC LDO DVS VTHR : 0.9V */
	IIC0_EWrite(S2MPS13_ADDR, LDO_DVS2, 0xCC);
#endif
	/* set G3D LDO DVS VTHR : 0.9V */
	IIC0_EWrite(S2MPS13_ADDR, LDO_DVS3, 0xCC);

	/* enable low_jitter, 32kHz BT, CP, AP at RTC_BUF */
	IIC0_ERead(S2MPS13_ADDR, RTC_BUF, &rtc_ctrl);
	rtc_ctrl |= (LOW_JITTER_EN | BT_32KHZ_EN | CP_32KHZ_EN | AP_32KHZ_EN);
	IIC0_EWrite(S2MPS13_ADDR, RTC_BUF, rtc_ctrl);

	/* enable WRSTBI detection */
	IIC0_ERead(S2MPS13_ADDR, WRSTBI_CTRL, &wrstbi_ctrl);
	wrstbi_ctrl |= WRSTBI_EN;
	IIC0_EWrite(S2MPS13_ADDR, WRSTBI_CTRL, wrstbi_ctrl);

	/* power control for peri device */
	/* enable LDO6 : VDD_HSIF_1.0V_AP */
	IIC0_ERead(S2MPS13_ADDR, LDO6_CTRL, &ldo_ctrl);
	ldo_ctrl |= OUTPUT_PWREN_ON;
	IIC0_EWrite(S2MPS13_ADDR, LDO6_CTRL, ldo_ctrl);

	/* enable LDO7 : VDD_HSIF_1.8V_AP */
	IIC0_ERead(S2MPS13_ADDR, LDO7_CTRL, &ldo_ctrl);
	ldo_ctrl |= OUTPUT_PWREN_ON;
	IIC0_EWrite(S2MPS13_ADDR, LDO7_CTRL, ldo_ctrl);

	/* enable LDO27 : VCC_LCD_3.0V */
	IIC0_ERead(S2MPS13_ADDR, LDO27_CTRL, &ldo_ctrl);
	ldo_ctrl |= OUTPUT_PWREN_ON;
	IIC0_EWrite(S2MPS13_ADDR, LDO27_CTRL, ldo_ctrl);

	/* enable LDO28 : VCC_LCD_1.8V */
	IIC0_ERead(S2MPS13_ADDR, LDO28_CTRL, &ldo_ctrl);
	ldo_ctrl |= OUTPUT_PWREN_ON;
	IIC0_EWrite(S2MPS13_ADDR, LDO28_CTRL, ldo_ctrl);

#ifdef CONFIG_CPU_EXYNOS5433
#ifdef CONFIG_CPU_EXYNOS5433_EVT1
	/* EGL */
	IIC0_EWrite(S2MPS13_ADDR, BUCK2_OUT,
		WR_BUCK_VOLT(1075000/1000));
	/* KFC */
	IIC0_EWrite(S2MPS13_ADDR, BUCK3_OUT,
		WR_BUCK_VOLT(1175000/1000));
	/* INT */
	IIC0_EWrite(S2MPS13_ADDR, BUCK4_OUT,
		WR_BUCK_VOLT(1000000/1000));
	/* CAM0_DISP */
	IIC0_EWrite(S2MPS13_ADDR, BUCK5_OUT,
		WR_BUCK_VOLT(988000/1000));
	/* G3D */
	IIC0_EWrite(S2MPS13_ADDR, BUCK6_OUT,
		WR_BUCK_VOLT(900000/1000));
#else
	/* EGL */
	IIC0_EWrite(S2MPS13_ADDR, BUCK2_OUT,
		WR_BUCK_VOLT(uEglAsvGrpVolt/1000));
	/* KFC */
	IIC0_EWrite(S2MPS13_ADDR, BUCK3_OUT,
		WR_BUCK_VOLT(uKfcAsvGrpVolt/1000));
	/* INT */
	IIC0_EWrite(S2MPS13_ADDR, BUCK4_OUT,
		WR_BUCK_VOLT(uIntAsvGrpVolt/1000));
	/* CAM0_DISP */
	IIC0_EWrite(S2MPS13_ADDR, BUCK5_OUT,
		WR_BUCK_VOLT(uCamAsvGrpVolt/1000));
	/* G3D */
	IIC0_EWrite(S2MPS13_ADDR, BUCK6_OUT,
		WR_BUCK_VOLT(uG3dAsvGrpVolt/1000));
#endif
#else
	if (pmic_id == REV_00) {
		/* BUCK Control */
		IIC0_EWrite(S2MPS13_ADDR, BUCK2_OUT,
			WR_BUCK_VOLT(uEglAsvGrpVolt/10000 - VDD_BASE));
		IIC0_EWrite(S2MPS13_ADDR, BUCK3_OUT,
			WR_BUCK_VOLT(uKfcAsvGrpVolt/10000 - VDD_BASE));
		IIC0_EWrite(S2MPS13_ADDR, BUCK4_OUT,
			WR_BUCK_VOLT(uIntAsvGrpVolt/10000 - VDD_BASE));

		/* BUCK6 : set pwm mode */
		IIC0_ERead(S2MPS13_ADDR, BUCK6_CTRL, &buck6_ctrl);
		buck6_ctrl |= (PWM_MODE << MODE_B);
		IIC0_EWrite(S2MPS13_ADDR, BUCK6_CTRL, buck6_ctrl);

		IIC0_EWrite(S2MPS13_ADDR, BUCK6_OUT,
			WR_BUCK_VOLT(uG3dAsvGrpVolt/10000 - VDD_BASE));

		/* BUCK6 : set auto mode */
		IIC0_ERead(S2MPS13_ADDR, BUCK6_CTRL, &buck6_ctrl);
		buck6_ctrl &= ~(PWM_MODE << MODE_B);
		buck6_ctrl |= (AUTO_MODE << MODE_B);
		IIC0_EWrite(S2MPS13_ADDR, BUCK6_CTRL, buck6_ctrl);
	} else {
		/* BUCK Control */
		IIC0_EWrite(S2MPS13_ADDR, BUCK2_OUT,
				WR_BUCK_VOLT(uEglAsvGrpVolt/10000));
		IIC0_EWrite(S2MPS13_ADDR, BUCK3_OUT,
				WR_BUCK_VOLT(uKfcAsvGrpVolt/10000));
		IIC0_EWrite(S2MPS13_ADDR, BUCK4_OUT,
				WR_BUCK_VOLT(uIntAsvGrpVolt/10000));
		IIC0_EWrite(S2MPS13_ADDR, BUCK6_OUT,
				WR_BUCK_VOLT(uG3dAsvGrpVolt/10000));
	}
#endif
}

void pmic_late_init(void)
{
	unsigned char pmic_id;

	/* read pmic revision number */
	IIC0_ERead(S2MPS13_ADDR, PMIC_ID, &pmic_id);

#ifdef CONFIG_CPU_EXYNOS5433
#ifdef CONFIG_CPU_EXYNOS5433_EVT1
	/* MIF */
	IIC0_EWrite(S2MPS13_ADDR, BUCK1_OUT,
		WR_BUCK_VOLT(1000000/1000));
#else
	/* MIF */
	IIC0_EWrite(S2MPS13_ADDR, BUCK1_OUT,
		WR_BUCK_VOLT(uMifAsvGrpVolt/1000));
#endif
#else
	if (pmic_id == REV_00) {
		/* BUCK Control */
		IIC0_EWrite(S2MPS13_ADDR, BUCK1_OUT,
			WR_BUCK_VOLT(uMifAsvGrpVolt/10000 - VDD_BASE));
	} else {
		/* BUCK Control */
		IIC0_EWrite(S2MPS13_ADDR, BUCK1_OUT,
				WR_BUCK_VOLT(uMifAsvGrpVolt/10000));
	}
#endif
}

#ifdef CONFIG_CPU_EXYNOS5433_EVT1
void pmic_asv_init(void)
{
#ifndef CONFIG_SPL_BUILD
	/* EGL */
	IIC0_EWrite(S2MPS13_ADDR, BUCK2_OUT,
		WR_BUCK_VOLT((float)uEglAsvGrpVolt/1000));
	/* KFC */
	IIC0_EWrite(S2MPS13_ADDR, BUCK3_OUT,
		WR_BUCK_VOLT((float)uKfcAsvGrpVolt/1000));
	/* INT */
	IIC0_EWrite(S2MPS13_ADDR, BUCK4_OUT,
		WR_BUCK_VOLT((float)uIntAsvGrpVolt/1000));
	/* CAM0_DISP */
	IIC0_EWrite(S2MPS13_ADDR, BUCK5_OUT,
		WR_BUCK_VOLT((float)uCamAsvGrpVolt/1000));
	/* G3D */
	IIC0_EWrite(S2MPS13_ADDR, BUCK6_OUT,
		WR_BUCK_VOLT((float)uG3dAsvGrpVolt/1000));
	/* MIF */
	IIC0_EWrite(S2MPS13_ADDR, BUCK1_OUT,
		WR_BUCK_VOLT((float)uMifAsvGrpVolt/1000));
#endif
}
#endif

void pmic_enable_peric_dev(void)
{
	unsigned char ldo_ctrl;

	IIC0_ESetport();

	/* enable LDO19 : VCC_1.8V_PERI */
	IIC0_ERead(S2MPS13_ADDR, LDO19_CTRL, &ldo_ctrl);
	ldo_ctrl |= OUTPUT_PWREN_ON;
	IIC0_EWrite(S2MPS13_ADDR, LDO19_CTRL, ldo_ctrl);

	/* enable LDO20 : VCC_2.8V_PERI */
	IIC0_ERead(S2MPS13_ADDR, LDO20_CTRL, &ldo_ctrl);
	ldo_ctrl |= OUTPUT_PWREN_ON;
	IIC0_EWrite(S2MPS13_ADDR, LDO20_CTRL, ldo_ctrl);
}
