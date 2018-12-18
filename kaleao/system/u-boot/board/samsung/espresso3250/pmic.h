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

#define IIC1_ESCL_Hi	GPD1DAT |= (0x1<<3)
#define IIC1_ESCL_Lo	GPD1DAT &= ~(0x1<<3)
#define IIC1_ESDA_Hi	GPD1DAT |= (0x1<<2)
#define IIC1_ESDA_Lo	GPD1DAT &= ~(0x1<<2)

#define IIC0_ESCL_INP	GPD1CON &= ~(0xf<<4)
#define IIC0_ESCL_OUTP	GPD1CON = (GPD1CON & ~(0xf<<4))|(0x1<<4)

#define IIC0_ESDA_INP	GPD1CON &= ~(0xf<<0)
#define IIC0_ESDA_OUTP	GPD1CON = (GPD1CON & ~(0xf<<0))|(0x1<<0)

#define DELAY		100

/* S2MPS14 slave address */
#define S2MPS14_WR_ADDR	0xCC
#define S2MPS14_RD_ADDR	0xCD

#define VDD_BASE_VOLT_BUCK1	65000
#define VDD_VOLT_STEP_BUCK1	625
#define MIN_VOLT_BUCK1	650
#define MAX_RD_VAL_BUCK1	0xFC
#define RD_BUCK_VOLT_BUCK1(x)	(((x > MAX_RD_VAL_BUCK1) ? 0 : \
			((x * VDD_VOLT_STEP_BUCK1) + VDD_BASE_VOLT_BUCK1) / 100))
#define WR_BUCK_VOLT_BUCK1(x)	((x < MIN_VOLT_BUCK1) ? 0 : \
			((((x) * 100) - VDD_BASE_VOLT_BUCK1) / VDD_VOLT_STEP_BUCK1))

#define VDD_BASE_VOLT	60000
#define VDD_VOLT_STEP	625
#define MIN_VOLT	600
#define MAX_RD_VAL	0xA0
#define RD_BUCK_VOLT(x)	(((x > MAX_RD_VAL) ? 0 : \
			((x * VDD_VOLT_STEP) + VDD_BASE_VOLT) / 100))
#define WR_BUCK_VOLT(x)	((x < MIN_VOLT) ? 0 : \
			((((x) * 100) - VDD_BASE_VOLT) / VDD_VOLT_STEP))

#define RTC_BUF			0x0C
#define WRSTBI			0x18
#define LDO18_CTRL		0x34
#define LDO23_CTRL		0x39

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

