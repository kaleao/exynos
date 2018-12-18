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

#define GPD2CON         *(volatile unsigned long *)(0x14CC0120)
#define GPD2DAT         *(volatile unsigned long *)(0x14CC0124)
#define GPD2PUD         *(volatile unsigned long *)(0x14CC0128)

#define IIC2_ESCL_Hi    GPD2DAT |= (0x1<<7)
#define IIC2_ESCL_Lo    GPD2DAT &= ~(0x1<<7)
#define IIC2_ESDA_Hi    GPD2DAT |= (0x1<<6)
#define IIC2_ESDA_Lo    GPD2DAT &= ~(0x1<<6)

#define IIC2_ESCL_INP   GPD2CON &= ~(0xf<<28)
#define IIC2_ESCL_OUTP  GPD2CON = (GPD2CON & ~(0xf<<28))|(0x1<<28)

#define IIC2_ESDA_INP   GPD2CON &= ~(0xf<<24)
#define IIC2_ESDA_OUTP  GPD2CON = (GPD2CON & ~(0xf<<24))|(0x1<<24)

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

