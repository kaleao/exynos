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

#ifndef __CHARGE_H__
#define __CHARGE_H__

#define GPD2CON		*(volatile unsigned int *)(0x134700E0)
#define GPD2DAT		*(volatile unsigned int *)(0x134700E4)
#define GPD2PUD		*(volatile unsigned int *)(0x134700E8)

#define IIC_ESCL_Hi	GPD2DAT |= (0x1<<1)
#define IIC_ESCL_Lo	GPD2DAT &= ~(0x1<<1)
#define IIC_ESDA_Hi	GPD2DAT |= (0x1<<0)
#define IIC_ESDA_Lo	GPD2DAT &= ~(0x1<<0)

#define IIC_ESCL_INP	GPD2CON &= ~(0xf<<4)
#define IIC_ESCL_OUTP	GPD2CON = (GPD2CON & ~(0xf<<4))|(0x1<<4)

#define IIC_ESDA_INP	GPD2CON &= ~(0xf<<0)
#define IIC_ESDA_OUTP	GPD2CON = (GPD2CON & ~(0xf<<0))|(0x1<<0)

#define GPIO_CHARGE_DAT	GPD2DAT
#define GPIO_CHARGE_DAT_SHIFT	(0)
#define DIS_GPIO_CHARGE_PUD	GPD2PUD &= ~(0xf<<0)

#define DELAY_CH		100


extern void charge_init(void);
extern void IIC_ERead(unsigned char ChipId,
		unsigned char IicAddr, unsigned char *IicData);
extern void IIC_EWrite(unsigned char ChipId,
		unsigned char IicAddr, unsigned char IicData);

#endif /*__CHAEGE_H__*/

