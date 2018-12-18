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

#ifdef CONFIG_ARM64
#define GPD2CON		*(volatile unsigned int *)(0x14CC0120)
#define GPD2DAT		*(volatile unsigned int *)(0x14CC0124)
#define GPD2PUD		*(volatile unsigned int *)(0x14CC0128)
#else
#define GPD2CON		*(volatile unsigned long *)(0x14CC0120)
#define GPD2DAT		*(volatile unsigned long *)(0x14CC0124)
#define GPD2PUD		*(volatile unsigned long *)(0x14CC0128)
#endif

#define IIC0_ESCL_Hi	GPD2DAT |= (0x1<<1)
#define IIC0_ESCL_Lo	GPD2DAT &= ~(0x1<<1)
#define IIC0_ESDA_Hi	GPD2DAT |= (0x1<<0)
#define IIC0_ESDA_Lo	GPD2DAT &= ~(0x1<<0)

#define IIC0_ESCL_INP	GPD2CON &= ~(0xf<<4)
#define IIC0_ESCL_OUTP	GPD2CON = (GPD2CON & ~(0xf<<4))|(0x1<<4)

#define IIC0_ESDA_INP	GPD2CON &= ~(0xf<<0)
#define IIC0_ESDA_OUTP	GPD2CON = (GPD2CON & ~(0xf<<0))|(0x1<<0)

#define DELAY		100

/* S2MPA01 slave address */
#define S2MPS13_ADDR	0xCC
#define S2MPS13_RTC	0x0C

#define VDD_BASE_VOLT	40000
#define VDD_VOLT_STEP	625
#define MIN_VOLT	600
#define MAX_RD_VAL	0x90
#define RD_BUCK_VOLT(x)	(((x > MAX_RD_VAL) ? 0 : \
			((x * VDD_VOLT_STEP) + VDD_BASE_VOLT) / 100))
#define WR_BUCK_VOLT(x)	((x < MIN_VOLT) ? 0 : \
			((((x) * 100) - VDD_BASE_VOLT) / VDD_VOLT_STEP))

#define VDD_BASE		(200)
#define VDD_REG_VALUE_BASE	((VDD_BASE * 100) / VDD_VOLT_STEP)

/* S2MPA01 Revision ID */
#define REV_00		0x00

/* S2MPA01 Register Address */
#define PMIC_ID		0x00
#define RTC_BUF		0x0C
#define WRSTBI_CTRL	0x18
#define BUCK1_CTRL	0x19
#define BUCK1_OUT	0x1A
#define BUCK2_CTRL	0x1B
#define BUCK2_OUT	0x1C
#define BUCK3_CTRL	0x1D
#define BUCK3_OUT	0x1E
#define BUCK4_CTRL	0x1F
#define BUCK4_OUT	0x20
#define BUCK5_CTRL	0x21
#define BUCK5_OUT	0x22
#define BUCK6_CTRL	0x23
#define BUCK6_OUT	0x24
#define BUCK7_CTRL	0x25
#define BUCK7_SW	0x26
#define BUCK7_OUT	0x27
#define BUCK8_CTRL	0x28
#define BUCK8_OUT	0x29
#define BUCK9_CTRL	0x2A
#define BUCK9_OUT	0x2B
#define BUCK10_CTRL	0x2C
#define BUCK10_OUT	0x2D

/* LDO DVS Control Register */
#define LDO_DVS1	0x32
#define LDO_DVS2	0x33
#define LDO_DVS3	0x34
#define LDO_DVS4	0x35

#define LDO6_CTRL	0x3B
#define LDO7_CTRL	0x3C
#define LDO19_CTRL	0x48
#define LDO20_CTRL	0x49
#define LDO27_CTRL	0x50
#define LDO28_CTRL	0x51

/* RTC Register Address */
#define RTC_WTSR_SMPL	0x01
#define RTC_UPDATE	0x02
#define RTC_SEC	0x04
#define RTC_MIN	0x05
#define RTC_HOUR	0x06
#define RTC_WEEK	0x07
#define RTC_DATE	0x08
#define RTC_MONTH	0x09
#define RTC_YEAR	0x0A

/* RTC Counter Register offsets */
typedef enum {
	PMIC_RTC_SEC = 0,
	PMIC_RTC_MIN,
	PMIC_RTC_HOUR,
	PMIC_RTC_WEEK,
	PMIC_RTC_DATE,
	PMIC_RTC_MONTH,
	PMIC_RTC_YEAR,
	NR_PMIC_RTC_CNT_REGS,
};

/* LDO CTRL bit */
#define OFF			(0x0)
#define ON			(0x1)
#define OUTPUT_OFF		(~(3 << 6))
#define OUTPUT_PWREN_ON		(1 << 6)
#define OUTPUT_LOWPWR_MODE	(2 << 6)
#define OUTPUT_NORMAL_MODE	(3 << 6)

/*
 * BUCK control
 */
#define BUCK_EN			(0x6)
#define REMOTE_EN		(0x5)
#define DSCH_B			(0x4)
#define MODE_B			(0x2)
#define AVP_EN			(0x1)
/* BUCK_EN */
#define ALWAYS_OFF		(0x0)
#define PWREN_CTRL		(0x1)
#define ALWAYS_ON		(0x3)
/* REMOTE_EN */
#define REMOTE_SENSE_OFF	(0x0)
#define REMOTE_SENSE_ON		(0x1)
/* DSCH */
#define NO_ACTIVE_DISCHARGE	(0x0)
#define ACTIVE_DISCHARGE	(0x1)
/* MODE */
#define AUTO_MODE		(0x2)
#define PWM_MODE		(0x3)
/* AVP(Adaptive Voltage position) */
#define ACP_OFF			(0x0)
#define ACP_ON			(0x1)

/*
 * RTC_BUF
 */
#define LOW_JITTER_EN		(0x1 << 4)
#define BT_32KHZ_EN		(0x1 << 2)
#define CP_32KHZ_EN		(0x1 << 1)
#define AP_32KHZ_EN		(0x1 << 0)

/*
 * WRSTBI
 */
#define WRSTBI_EN		(0x1 << 5)

extern void pmic_init(void);
extern void IIC0_ERead(unsigned char ChipId,
		unsigned char IicAddr, unsigned char *IicData);
extern void IIC0_EWrite(unsigned char ChipId,
		unsigned char IicAddr, unsigned char IicData);

#endif /*__PMIC_H__*/

