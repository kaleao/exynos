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

#ifndef __PMIC_H__
#define __PMIC_H__

#define GPD1CON		*(volatile unsigned long *)(0x114000C0)
#define GPD1DAT		*(volatile unsigned long *)(0x114000C4)
#define GPD1PUD		*(volatile unsigned long *)(0x114000C8)

#define IIC0_ESCL_Hi	GPD1DAT |= (0x1<<1)
#define IIC0_ESCL_Lo	GPD1DAT &= ~(0x1<<1)
#define IIC0_ESDA_Hi	GPD1DAT |= (0x1<<0)
#define IIC0_ESDA_Lo	GPD1DAT &= ~(0x1<<0)

#define IIC0_ESCL_INP	GPD1CON &= ~(0xf<<4)
#define IIC0_ESCL_OUTP	GPD1CON = (GPD1CON & ~(0xf<<4))|(0x1<<4)

#define IIC0_ESDA_INP	GPD1CON &= ~(0xf<<0)
#define IIC0_ESDA_OUTP	GPD1CON = (GPD1CON & ~(0xf<<0))|(0x1<<0)

#define DELAY		100

/* S5M8767 slave address */
#define S5M8767_WR_ADDR		0xCC
#define S5M8767_RD_ADDR		0xCD

#define VDD_BASE_VOLT	60000
#define VDD_VOLT_STEP	625
#define MIN_VOLT	600
#define MAX_RD_VAL	0xA0
#define RD_BUCK_VOLT(x)	(((x > MAX_RD_VAL) ? 0 : \
			((x * VDD_VOLT_STEP) + VDD_BASE_VOLT) / 100))
#define WR_BUCK_VOLT(x)	((x < MIN_VOLT) ? 0 : \
			((((x) * 100) - VDD_BASE_VOLT) / VDD_VOLT_STEP))
/* RTC CTRL */
#define RTC_CTRL2	(0x1 << 4)
#define RTC_CTRL1	(0x1 << 3)
#define BT_32KHZ_EN	(0x1 << 2)
#define CP_32KHZ_EN	(0x1 << 1)
#define AP_32KHZ_EN	(0x1 << 0)

/* S5M8767 Revision ID */
#define REV_00		0x00
#define REV_01		0x01

/* S5M8767 Register Address */
#define PMIC_ID		0x00
#define RTC_CTRL	0x0B
#define BUCK1_CTRL1	0x2D
#define BUCK1_CTRL2	0x2E
#define BUCK2_CTRL1	0x2F
#define BUCK2_CTRL2	0x30
#define BUCK3_CTRL1	0x31
#define BUCK3_CTRL2	0x32
#define BUCK4_CTRL1	0x33
#define BUCK4_CTRL2	0x34
#define BUCK5_CTRL1	0x35
#define BUCK5_CTRL2	0x36
#define CM_DUAL_TEST1	0x89
#define CM_SINGLE_TEST4	0x95
#define VMBUCK_ALL	0x9A

#define ONOFFINT1	0x01
#define ONOFFINT2	0x02
#define RTCINT3	0x03
#define ONOFFINT1M	0x04
#define ONOFFINT2M	0x05
#define RTCINT3M	0x06
#define STATUS1	0x07
#define STATUS2	0x08
#define CTRL1		0x0A
#define CTRL2		0x0B
#define BUCK9_CTRL1	0x5A
#define BUCK9_CTRL2	0x5B
#define PWRONSRC	0xE0

/* S5M8767 Register Address */
#define REV01_RTC_CTRL		0x0B
#define REV01_BUCK1_CTRL1	0x28
#define REV01_BUCK1_CTRL2	0x29
#define REV01_BUCK2_CTRL1	0x2A
#define REV01_BUCK2_CTRL2	0x2B
#define REV01_BUCK3_CTRL1	0x2C
#define REV01_BUCK3_CTRL2	0x2D
#define REV01_BUCK4_CTRL1	0x2E
#define REV01_BUCK4_CTRL2	0x2F
#define REV01_BUCK5_CTRL1	0x30
#define REV01_BUCK5_CTRL2	0x31
#define REV01_LDO33_CTRL	0x4B
#define REV01_LDO34_CTRL	0x4C
#define REV01_SELMIF		0x59
#define REV01_LDO29		0x47
#define REV01_LDO37		0x4F

/* LDO CTRL bit */
#define OFF			(0x0)
#define ON			(0x1)
#define OUTPUT_OFF		(~(3 << 6))
#define OUTPUT_PWREN_ON		(1 << 6)
#define OUTPUT_LOWPWR_MODE	(2 << 6)
#define OUTPUT_NORMAL_MODE	(3 << 6)

/* SELMIF bit */
#define SELMIF_BUCK1		(1 << 0)
#define SELMIF_LDO2		(1 << 1)
#define SELMIF_LDO5		(1 << 2)
#define SELMIF_LDO6		(1 << 3)

/* CM_DUAL_TEST1 bit */
#define CM_DT1_PROTECT_EN		(1 << 7)
#define CM_DT1_DVS_PFM_EN		(1 << 6)
#define CM_DT1_TEMP_SELB		(1 << 5)
#define CM_DT1_H2L			(1 << 4)
#define CM_DT1_ZERO_VOSTEP		(1 << 3)
#define CM_DT1_DVS_FPWM			(1 << 2)
#define CM_DT1_PCLK_DLY_EN		(1 << 1)
#define CM_DT1_PWMVTHL_DNB		(1 << 0)

/* CM_SINGLE_TEST4 bit */
#define CM_ST4_RS_PMUP_EN		(1 << 7)
#define CM_ST4_OVP_EN			(1 << 6)
#define CM_ST4_DVS_FPWM			(1 << 5)
#define CM_ST4_CAP_ADD_FB		(1 << 4)
#define CM_ST4_PROTECT_EN		(1 << 3)
#define CM_ST4_PON_USE			(1 << 2)
#define CM_ST4_PFMVTH_DNB_PNOFF		(1 << 1)
#define CM_ST4_PFM_MIN_EN		(1 << 0)

/* VMBUCK_ALL bit */
#define VMBUCK_ALL_EN_DSCHG_DVS_DN	(1 << 7)
#define VMBUCK_ALL_PFM_VTH_DNB_PNOFF	(1 << 6)
#define VMBUCK_ALL_PWMVTHH_UPB		(1 << 5)
#define VMBUCK_ALL_PWMVTHL_DNB		(1 << 4)
#define VMBUCK_ALL_PFMCALSEL		(1 << 3)
#define VMBUCK_ALL_MINOFF_EN		(1 << 2)
#define VMBUCK_ALL_ADDC			(1 << 1)
#define VMBUCK_ALL_EN_PWM_DVS_DN	(1 << 0)

extern void pmic_init(void);
extern void IIC0_ERead(unsigned char ChipId,
		unsigned char IicAddr, unsigned char *IicData);
extern void IIC0_EWrite(unsigned char ChipId,
		unsigned char IicAddr, unsigned char IicData);

#endif /*__PMIC_H__*/

