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

#define GPB3CON		*(volatile unsigned long *)(0x116000C0)
#define GPB3DAT		*(volatile unsigned long *)(0x116000C4)
#define GPB3PUD		*(volatile unsigned long *)(0x116000C8)

#define GPB4CON		*(volatile unsigned long *)(0x116000E0)
#define GPB4DAT		*(volatile unsigned long *)(0x116000E4)
#define GPB4PUD		*(volatile unsigned long *)(0x116000E8)

#ifdef CONFIG_MACH_XYREF5260
#define IIC0_ESCL_Hi	GPB3DAT |= (0x1<<1)
#define IIC0_ESCL_Lo	GPB3DAT &= ~(0x1<<1)
#define IIC0_ESDA_Hi	GPB3DAT |= (0x1<<0)
#define IIC0_ESDA_Lo	GPB3DAT &= ~(0x1<<0)

#define IIC0_ESCL_INP	GPB3CON &= ~(0xf<<4)
#define IIC0_ESCL_OUTP	GPB3CON = (GPB3CON & ~(0xf<<4))|(0x1<<4)

#define IIC0_ESDA_INP	GPB3CON &= ~(0xf<<0)
#define IIC0_ESDA_OUTP	GPB3CON = (GPB3CON & ~(0xf<<0))|(0x1<<0)

#define GPIO_DAT	GPB3DAT
#define GPIO_DAT_SHIFT	(0)
#define DIS_GPIO_PUD	GPB3PUD &= ~(0xf<<0)
#else
#define IIC0_ESCL_Hi	GPB4DAT |= (0x1<<3)
#define IIC0_ESCL_Lo	GPB4DAT &= ~(0x1<<3)
#define IIC0_ESDA_Hi	GPB4DAT |= (0x1<<2)
#define IIC0_ESDA_Lo	GPB4DAT &= ~(0x1<<2)

#define IIC0_ESCL_INP	GPB4CON &= ~(0xf<<12)
#define IIC0_ESCL_OUTP	GPB4CON = (GPB4CON & ~(0xf<<12))|(0x1<<12)

#define IIC0_ESDA_INP	GPB4CON &= ~(0xf<<8)
#define IIC0_ESDA_OUTP	GPB4CON = (GPB4CON & ~(0xf<<8))|(0x1<<8)

#define GPIO_DAT	GPB4DAT
#define GPIO_DAT_SHIFT	(2)
#define DIS_GPIO_PUD	GPB4PUD &= ~(0xf<<4)
#endif

#define DELAY		100

/* S2MPA01 slave address */
#define S2MPA01_ADDR	0xCC
#define S2MPA01_RTC	0x0C

#define VDD_BASE_VOLT	60000
#define VDD_VOLT_STEP	625
#define MIN_VOLT	600
#define MAX_RD_VAL	0xA0
#define RD_BUCK_VOLT(x)	(((x > MAX_RD_VAL) ? 0 : \
			((x * VDD_VOLT_STEP) + VDD_BASE_VOLT) / 100))
#define WR_BUCK_VOLT(x)	((x < MIN_VOLT) ? 0 : \
			((((x) * 100) - VDD_BASE_VOLT) / VDD_VOLT_STEP))

/* S2MPA01 Revision ID */
#define REV_00		0x00

/* S2MPA01 Register Address */
#define PMIC_ID		0x00
#define S2MPA01_INT1	0x01
#define S2MPA01_INT2	0x02
#define S2MPA01_INT3	0x03
#define S2MPA01_STATUS1	0x07
#define S2MPA01_STATUS2	0x08
#define PWRONSRC	0x09
#define OFFSRC		0x0A
#define RTC_BUF		0x0B
#define WRSTBI_CTRL	0x27
#define BUCK1_CTRL	0x28
#define BUCK1_OUT	0x29
#define BUCK2_CTRL	0x2A
#define BUCK2_OUT	0x2B
#define BUCK3_CTRL	0x2C
#define BUCK3_OUT	0x2D
#define BUCK4_CTRL	0x2E
#define BUCK4_OUT	0x2F
#define BUCK5_CTRL	0x30
#define BUCK5_SW	0x31
#define BUCK5_OUT	0x32
#define BUCK5_OUT2	0x33
#define BUCK5_OUT3	0x34
#define BUCK5_OUT4	0x35
#define BUCK6_CTRL	0x36
#define BUCK6_OUT	0x37
#define BUCK7_CTRL	0x38
#define BUCK7_OUT	0x39
#define BUCK8_CTRL	0x3A
#define BUCK8_OUT	0x3B
#define BUCK9_CTRL	0x3C
#define BUCK9_OUT	0x3D
#define BUCK10_CTRL	0x3E
#define BUCK10_OUT	0x3F

#define BUCK9_VDD_3_5V	0xD4

#define LDO24_CTRL	0x57
#define LDO25_CTRL	0x58

/* S2MPA01 RTC Register Address */
#define RTC_WTSR_SMPL	0x01

/* LDO CTRL bit */
#define OFF			(0x0)
#define ON			(0x1)
#define OUTPUT_OFF		(~(3 << 6))
#define OUTPUT_PWREN_ON		(1 << 6)
#define OUTPUT_LOWPWR_MODE	(2 << 6)
#define OUTPUT_NORMAL_MODE	(3 << 6)

/*
 * RTC_BUF
 */
#define LOW_JITTER_EN		(0x1 << 4)
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

