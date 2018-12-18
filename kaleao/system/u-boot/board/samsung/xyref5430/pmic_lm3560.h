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

#ifndef __PMIC_LM3560_H__
#define __PMIC_LM3560_H__

#define GPD5CON		*(volatile unsigned long *)(0x14CC0160)
#define GPD5DAT		*(volatile unsigned long *)(0x14CC0164)
#define GPD5PUD		*(volatile unsigned long *)(0x14CC0168)

#define IIC2_ESCL_Hi	GPD5DAT |= (0x1<<3)
#define IIC2_ESCL_Lo	GPD5DAT &= ~(0x1<<3)
#define IIC2_ESDA_Hi	GPD5DAT |= (0x1<<2)
#define IIC2_ESDA_Lo	GPD5DAT &= ~(0x1<<2)

#define IIC2_ESCL_INP	GPD5CON &= ~(0xf<<12)
#define IIC2_ESCL_OUTP	GPD5CON = (GPD5CON & ~(0xf<<12))|(0x1<<12)

#define IIC2_ESDA_INP	GPD5CON &= ~(0xf<<8)
#define IIC2_ESDA_OUTP	GPD5CON = (GPD5CON & ~(0xf<<8))|(0x1<<8)

#define DELAY		100

/* LM3560 slave address */
#define LM3560_ADDR	0xA6

/* LM3560 Register Address */
#define ENABLE_REGISTER	0x10

extern void pmic_init(void);
extern void IIC2_ERead(unsigned char ChipId,
		unsigned char IicAddr, unsigned char *IicData);
extern void IIC2_EWrite(unsigned char ChipId,
		unsigned char IicAddr, unsigned char IicData);

#endif /*__PMIC_H__*/

