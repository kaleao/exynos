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
/* S2MU003 Interface PMIC control */
/* HSI2C_2_SCL : GPC2[5], HSI2C_2_SDA : GPC2[4] */

#ifndef __IF_PMIC_H__
#define __IF_PMIC_H__

#define GBC2CON		*(volatile unsigned int *)(0x139C0080)
#define GBC2DAT		*(volatile unsigned int *)(0x139C0084)
#define GBC2PUD		*(volatile unsigned int *)(0x139C0088)

#define IF_PMIC_IIC0_ESCL_Hi	GBC2DAT |= (0x1<<5)
#define IF_PMIC_IIC0_ESCL_Lo	GBC2DAT &= ~(0x1<<5)
#define IF_PMIC_IIC0_ESDA_Hi	GBC2DAT |= (0x1<<4)
#define IF_PMIC_IIC0_ESDA_Lo	GBC2DAT &= ~(0x1<<4)

#define IF_PMIC_IIC0_ESCL_INP	GBC2CON &= ~(0xf<<20)
#define IF_PMIC_IIC0_ESCL_OUTP	GBC2CON = (GBC2CON & ~(0xf<<20))|(0x1<<20)

#define IF_PMIC_IIC0_ESDA_INP	GBC2CON &= ~(0xf<<16)
#define IF_PMIC_IIC0_ESDA_OUTP	GBC2CON = (GBC2CON & ~(0xf<<16))|(0x1<<16)

#define IF_PMIC_GPIO_DAT	GBC2DAT
#define IF_PMIC_GPIO_DAT_SHIFT	(4)
#define IF_PMIC_DIS_GPIO_PUD	GBC2PUD &= ~(0xf<<8)

#define IF_PMIC_IIC0_DELAY		100

/* IF_PMIC slave address */
#define IF_PMIC_ADDR	0x68


extern void if_pmic_init(void);
extern void IF_PMIC_IIC0_ESetport(void);
extern void IF_PMIC_IIC0_ERead(unsigned char ChipId,
		unsigned char IicAddr, unsigned char *IicData);
extern void IF_PMIC_IIC0_EWrite(unsigned char ChipId,
		unsigned char IicAddr, unsigned char IicData);

#endif /*__IF_PMIC_H__*/
