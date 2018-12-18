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

#ifndef __MUIC_H__
#define __MUIC_H__

#define GPD2CON         *(volatile unsigned int *)(0x134700E0)
#define GPD2DAT         *(volatile unsigned int *)(0x134700E4)
#define GPD2PUD         *(volatile unsigned int *)(0x134700E8)

#define GPIO_CON	GPD2CON
#define GPIO_DAT	GPD2DAT
#define GPIO_PUD	GPD2PUD
#define GPIO_DAT_SHIFT	(2)

#define DIS_GPIO_PUD	GPIO_PUD &= ~(0xf<<GPIO_DAT_SHIFT)

#define IIC_MUIC_ESCL_Hi    GPIO_DAT |= (0x1<<3)
#define IIC_MUIC_ESCL_Lo    GPIO_DAT &= ~(0x1<<3)
#define IIC_MUIC_ESDA_Hi    GPIO_DAT |= (0x1<<2)
#define IIC_MUIC_ESDA_Lo    GPIO_DAT &= ~(0x1<<2)

#define IIC_MUIC_ESCL_INP   GPIO_CON &= ~(0xf<<12)
#define IIC_MUIC_ESCL_OUTP  GPIO_CON = (GPIO_CON & ~(0xf<<12))|(0x1<<12)

#define IIC_MUIC_ESDA_INP   GPIO_CON &= ~(0xf<<8)
#define IIC_MUIC_ESDA_OUTP  GPIO_CON = (GPIO_CON & ~(0xf<<8))|(0x1<<8)

#define DELAY		100

/* MUIC slave address */
#define MUIC_ADDR	0x4A

#define MAX77833_MUIC_REG_DAT_IN_OP	0x0F
#define MAX77833_MUIC_ADCMODE_ONE_SHOT	0x1
#define MAX77833_MUIC_REG_DAT_IN1	0x10
#define MAX77833_MUIC_REG_DAT_IN8	0x17

extern void IIC_MUIC_ERead(unsigned char ChipId,
		unsigned char IicAddr, unsigned char *IicData);
extern void IIC_MUIC_EWrite(unsigned char ChipId,
		unsigned char IicAddr, unsigned char IicData);

#endif /*__MUIC_H__*/
