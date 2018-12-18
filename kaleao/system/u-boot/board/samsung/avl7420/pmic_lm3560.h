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

#ifndef __PMIC_LM3560_H__
#define __PMIC_LM3560_H__

#define GPD2CON         *(volatile unsigned int *)(0x134700E0)
#define GPD2DAT         *(volatile unsigned int *)(0x134700E4)
#define GPD2PUD         *(volatile unsigned int *)(0x134700E8)

#define GPIO_CON	GPD2CON
#define GPIO_DAT	GPD2DAT
#define GPIO_PUD	GPD2PUD
#define GPIO_DAT_SHIFT	(0)

#define DIS_GPIO_PUD	GPIO_PUD &= ~(0xf<<GPIO_DAT_SHIFT)

#define IIC2_ESCL_Hi    GPIO_DAT |= (0x1<<1)
#define IIC2_ESCL_Lo    GPIO_DAT &= ~(0x1<<1)
#define IIC2_ESDA_Hi    GPIO_DAT |= (0x1<<0)
#define IIC2_ESDA_Lo    GPIO_DAT &= ~(0x1<<0)

#define IIC2_ESCL_INP   GPIO_CON &= ~(0xf<<4)
#define IIC2_ESCL_OUTP  GPIO_CON = (GPIO_CON & ~(0xf<<4))|(0x1<<4)

#define IIC2_ESDA_INP   GPIO_CON &= ~(0xf<<0)
#define IIC2_ESDA_OUTP  GPIO_CON = (GPIO_CON & ~(0xf<<0))|(0x1<<0)

#define DELAY		100

/* LM3560 slave address */
#define LM3560_ADDR	0xA6

/* LM3560 Register Address */
#define ENABLE_REGISTER	0x10

extern void IIC2_ERead(unsigned char ChipId,
		unsigned char IicAddr, unsigned char *IicData);
extern void IIC2_EWrite(unsigned char ChipId,
		unsigned char IicAddr, unsigned char IicData);

#endif /*__PMIC_H__*/

