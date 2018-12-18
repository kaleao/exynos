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

#include <config.h>

#ifdef CONFIG_MACH_SMDK7580 /* For SMDK7580 */
#define GBC2CON		*(volatile unsigned int *)(0x139C0080)
#define GBC2DAT		*(volatile unsigned int *)(0x139C0084)
#define GBC2PUD		*(volatile unsigned int *)(0x139C0088)

#define MUIC_IIC0_ESCL_Hi	GBC2DAT |= (0x1<<1)
#define MUIC_IIC0_ESCL_Lo	GBC2DAT &= ~(0x1<<1)
#define MUIC_IIC0_ESDA_Hi	GBC2DAT |= (0x1<<0)
#define MUIC_IIC0_ESDA_Lo	GBC2DAT &= ~(0x1<<0)

#define MUIC_IIC0_ESCL_INP	GBC2CON &= ~(0xf<<4)
#define MUIC_IIC0_ESCL_OUTP	GBC2CON = (GBC2CON & ~(0xf<<4))|(0x1<<4)

#define MUIC_IIC0_ESDA_INP	GBC2CON &= ~(0xf<<0)
#define MUIC_IIC0_ESDA_OUTP	GBC2CON = (GBC2CON & ~(0xf<<0))|(0x1<<0)

#define MUIC_GPIO_DAT	GBC2DAT
#define MUIC_GPIO_DAT_SHIFT	(0)
#define MUIC_DIS_GPIO_PUD	GBC2PUD &= ~(0xf<<0)

#define MUIC_IIC0_DELAY		100
#endif

/* MUIC slave address */
#define MUIC_ADDR	0x4A

/* S2MM001 Device Type 1 register */
#define DEV_TYPE1_USB_OTG		(0x1 << 7)
#define DEV_TYPE1_DEDICATED_CHG		(0x1 << 6)
#define DEV_TYPE1_CDP			(0x1 << 5)
#define DEV_TYPE1_T1_T2_CHG		(0x1 << 4)
#define DEV_TYPE1_UART			(0x1 << 3)
#define DEV_TYPE1_USB			(0x1 << 2)
#define DEV_TYPE1_AUDIO_2		(0x1 << 1)
#define DEV_TYPE1_AUDIO_1		(0x1 << 0)

/* S2MM001 Device Type 2 register */
#define DEV_TYPE2_AV			(0x1 << 6)
#define DEV_TYPE2_TTY			(0x1 << 5)
#define DEV_TYPE2_PPD			(0x1 << 4)
#define DEV_TYPE2_JIG_UART_OFF		(0x1 << 3)
#define DEV_TYPE2_JIG_UART_ON		(0x1 << 2)
#define DEV_TYPE2_JIG_USB_OFF		(0x1 << 1)
#define DEV_TYPE2_JIG_USB_ON		(0x1 << 0)

/* S2MM001 Device Type 3 register */
#define DEV_TYPE3_U200_CHG		(0x1 << 6)
#define DEV_TYPE3_APPLE_CHG		(0x1 << 5)
#define DEV_TYPE3_AV_WITH_VBUS		(0x1 << 4)
#define DEV_TYPE3_NO_STD_CHG		(0x1 << 2)
#define DEV_TYPE3_MHL			(0x1 << 0)

extern void muic_init_to_usb(void);
extern void muic_init_to_uart(void);
extern void MUIC_IIC0_ESetport(void);
extern void MUIC_IIC0_ERead(unsigned char ChipId,
		unsigned char IicAddr, unsigned char *IicData);
extern void MUIC_IIC0_EWrite(unsigned char ChipId,
		unsigned char IicAddr, unsigned char IicData);

#endif /*__MUIC_H__*/
