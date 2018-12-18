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

#ifndef __PMIC_DISPLAY_H__
#define __PMIC_DISPLAY_H__

#define GPG3CON		*(volatile unsigned int *)(0x134701C0)
#define GPG3DAT		*(volatile unsigned int *)(0x134701C4)
#define GPG3PUD		*(volatile unsigned int *)(0x134701C8)

#define IIC_DISP_ESCL_Hi	GPG3DAT |= (0x1<<3)
#define IIC_DISP_ESCL_Lo	GPG3DAT &= ~(0x1<<3)
#define IIC_DISP_ESDA_Hi	GPG3DAT |= (0x1<<2)
#define IIC_DISP_ESDA_Lo	GPG3DAT &= ~(0x1<<2)

#define IIC_DISP_ESCL_INP	GPG3CON &= ~(0xf<<12)
#define IIC_DISP_ESCL_OUTP	GPG3CON = (GPG3CON & ~(0xf<<12))|(0x1<<12)
#define IIC_DISP_ESDA_INP	GPG3CON &= ~(0xf<<8)
#define IIC_DISP_ESDA_OUTP	GPG3CON = (GPG3CON & ~(0xf<<8))|(0x1<<8)

#define GPIO_DAT	GPG3DAT
#define GPIO_DAT_SHIFT	(2)
#define DIS_GPIO_PUD	GPG3PUD &= ~(0xf<<2)

#define DELAY		100

/* MAX77838 slave address */
#define MAX77838_W_ADDR	0xC0
#define MAX77838_R_ADDR	0xC1

/* MAX77838 Register Address */
#define MAX77838_DEVICE_ID	0x00
#define MAX77838_TOPSYS_STAT	0x01
#define MAX77838_REG_STAT	0x02
#define MAX77838_REG_EN		0x03

#define L1_EN	(1 << 0)
#define L2_EN	(1 << 1)
#define L3_EN	(1 << 2)
#define L4_EN	(1 << 3)
#define B_EN	(1 << 4)

extern void pmic_display_init(void);
extern void IIC_DISP_ERead(unsigned char ChipId,
		unsigned char IicAddr, unsigned char *IicData);
extern void IIC_DISP_EWrite(unsigned char ChipId,
		unsigned char IicAddr, unsigned char IicData);

#endif /*__PMIC_DISPLAY_H__*/

