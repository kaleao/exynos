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

#include <common.h>
#include <asm/arch/cpu.h>
#include "muic.h"

void MUIC_IIC0_Delay(void)
{
	unsigned long i;
	for (i = 0; i < MUIC_IIC0_DELAY; i++)
		;
}

void MUIC_IIC0_SCLH_SDAH(void)
{
	MUIC_IIC0_ESCL_Hi;
	MUIC_IIC0_ESDA_Hi;
	MUIC_IIC0_Delay();
}

void MUIC_IIC0_SCLH_SDAL(void)
{
	MUIC_IIC0_ESCL_Hi;
	MUIC_IIC0_ESDA_Lo;
	MUIC_IIC0_Delay();
}

void MUIC_IIC0_SCLL_SDAH(void)
{
	MUIC_IIC0_ESCL_Lo;
	MUIC_IIC0_ESDA_Hi;
	MUIC_IIC0_Delay();
}

void MUIC_IIC0_SCLL_SDAL(void)
{
	MUIC_IIC0_ESCL_Lo;
	MUIC_IIC0_ESDA_Lo;
	MUIC_IIC0_Delay();
}

void MUIC_IIC0_ELow(void)
{
	MUIC_IIC0_SCLL_SDAL();
	MUIC_IIC0_SCLH_SDAL();
	MUIC_IIC0_SCLH_SDAL();
	MUIC_IIC0_SCLL_SDAL();
}

void MUIC_IIC0_EHigh(void)
{
	MUIC_IIC0_SCLL_SDAH();
	MUIC_IIC0_SCLH_SDAH();
	MUIC_IIC0_SCLH_SDAH();
	MUIC_IIC0_SCLL_SDAH();
}

void MUIC_IIC0_EStart(void)
{
	MUIC_IIC0_SCLH_SDAH();
	MUIC_IIC0_SCLH_SDAL();
	MUIC_IIC0_Delay();
	MUIC_IIC0_SCLL_SDAL();
}

void MUIC_IIC0_EEnd(void)
{
	MUIC_IIC0_SCLL_SDAL();
	MUIC_IIC0_SCLH_SDAL();
	MUIC_IIC0_Delay();
	MUIC_IIC0_SCLH_SDAH();
}

void MUIC_IIC0_EAck_write(void)
{
	unsigned long ack;

	/* Function <- Input */
	MUIC_IIC0_ESDA_INP;

	MUIC_IIC0_ESCL_Lo;
	MUIC_IIC0_Delay();
	MUIC_IIC0_ESCL_Hi;
	MUIC_IIC0_Delay();
	ack = MUIC_GPIO_DAT;
	MUIC_IIC0_ESCL_Hi;
	MUIC_IIC0_Delay();
	MUIC_IIC0_ESCL_Hi;
	MUIC_IIC0_Delay();

	/* Function <- Output (SDA) */
	MUIC_IIC0_ESDA_OUTP;

	ack = (ack >> MUIC_GPIO_DAT_SHIFT) & 0x1;

	MUIC_IIC0_SCLL_SDAL();
}

void MUIC_IIC0_EAck_read(void)
{
	/* Function <- Output */
	MUIC_IIC0_ESDA_OUTP;

	MUIC_IIC0_ESCL_Lo;
	MUIC_IIC0_ESCL_Lo;
	MUIC_IIC0_ESDA_Hi;
	MUIC_IIC0_ESCL_Hi;
	MUIC_IIC0_ESCL_Hi;
	/* Function <- Input (SDA) */
	MUIC_IIC0_ESDA_INP;

	MUIC_IIC0_SCLL_SDAL();
}

void MUIC_IIC0_ESetport(void)
{
	/* Pull Up/Down Disable	SCL, SDA */
	MUIC_DIS_GPIO_PUD;

	MUIC_IIC0_ESCL_Hi;
	MUIC_IIC0_ESDA_Hi;

	/* Function <- Output (SCL) */
	MUIC_IIC0_ESCL_OUTP;
	/* Function <- Output (SDA) */
	MUIC_IIC0_ESDA_OUTP;

	MUIC_IIC0_Delay();
}

void MUIC_IIC0_EWrite(unsigned char ChipId,
		unsigned char IicAddr, unsigned char IicData)
{
	unsigned long i;

	MUIC_IIC0_EStart();

	/* write chip id */
	for (i = 7; i > 0; i--) {
		if ((ChipId >> i) & 0x0001)
			MUIC_IIC0_EHigh();
		else
			MUIC_IIC0_ELow();
	}

	/* write */
	MUIC_IIC0_ELow();

	/* ACK */
	MUIC_IIC0_EAck_write();

	/* write reg. addr. */
	for (i = 8; i > 0; i--) {
		if ((IicAddr >> (i-1)) & 0x0001)
			MUIC_IIC0_EHigh();
		else
			MUIC_IIC0_ELow();
	}

	/* ACK */
	MUIC_IIC0_EAck_write();

	/* write reg. data. */
	for (i = 8; i > 0; i--) {
		if ((IicData >> (i-1)) & 0x0001)
			MUIC_IIC0_EHigh();
		else
			MUIC_IIC0_ELow();
	}

	/* ACK */
	MUIC_IIC0_EAck_write();

	MUIC_IIC0_EEnd();
}

void MUIC_IIC0_ERead(unsigned char ChipId,
		unsigned char IicAddr, unsigned char *IicData)
{
	unsigned long i, reg;
	unsigned char data = 0;

	MUIC_IIC0_EStart();

	/* write chip id */
	for (i = 7; i > 0; i--) {
		if ((ChipId >> i) & 0x0001)
			MUIC_IIC0_EHigh();
		else
			MUIC_IIC0_ELow();
	}

	/* write */
	MUIC_IIC0_ELow();

	/* ACK */
	MUIC_IIC0_EAck_write();

	/* write reg. addr. */
	for (i = 8; i > 0; i--) {
		if ((IicAddr >> (i-1)) & 0x0001)
			MUIC_IIC0_EHigh();
		else
			MUIC_IIC0_ELow();
	}

	/* ACK */
	MUIC_IIC0_EAck_write();

	MUIC_IIC0_EStart();

	/* write chip id */
	for (i = 7; i > 0; i--) {
		if ((ChipId >> i) & 0x0001)
			MUIC_IIC0_EHigh();
		else
			MUIC_IIC0_ELow();
	}

	/* read */
	MUIC_IIC0_EHigh();
	/* ACK */
	MUIC_IIC0_EAck_write();

	/* read reg. data. */
	MUIC_IIC0_ESDA_INP;

	MUIC_IIC0_ESCL_Lo;
	MUIC_IIC0_ESCL_Lo;
	MUIC_IIC0_Delay();

	for (i = 8; i > 0; i--) {
		MUIC_IIC0_ESCL_Lo;
		MUIC_IIC0_ESCL_Lo;
		MUIC_IIC0_Delay();
		MUIC_IIC0_ESCL_Hi;
		MUIC_IIC0_ESCL_Hi;
		MUIC_IIC0_Delay();
		reg = MUIC_GPIO_DAT;
		MUIC_IIC0_ESCL_Hi;
		MUIC_IIC0_ESCL_Hi;
		MUIC_IIC0_Delay();
		MUIC_IIC0_ESCL_Lo;
		MUIC_IIC0_ESCL_Lo;
		MUIC_IIC0_Delay();

		reg = (reg >> MUIC_GPIO_DAT_SHIFT) & 0x1;

		data |= reg << (i-1);
	}

	/* ACK */
	MUIC_IIC0_EAck_read();
	MUIC_IIC0_ESDA_OUTP;

	MUIC_IIC0_EEnd();

	*IicData = data;
}

void muic_init_to_usb(void)
{
	MUIC_IIC0_ESetport();

	/* Set muic for USB */
	MUIC_IIC0_EWrite(MUIC_ADDR, 0x2B, 0x80);
	MUIC_IIC0_EWrite(MUIC_ADDR, 0x13, 0x26);	/* Set USB path */
	MUIC_IIC0_EWrite(MUIC_ADDR, 0x2, 0x1b);		/* Set Manual mode */
}

void muic_init_to_uart(void)
{
	unsigned char device_type1;
	unsigned char device_type2;
	unsigned char device_type3;
	unsigned char int1;
	unsigned char int2;

	MUIC_IIC0_ESetport();

	MUIC_IIC0_ERead(MUIC_ADDR, 0x0A, &device_type1);
	MUIC_IIC0_ERead(MUIC_ADDR, 0x0B, &device_type2);
	MUIC_IIC0_ERead(MUIC_ADDR, 0x15, &device_type3);
	MUIC_IIC0_ERead(MUIC_ADDR, 0x03, &int1);
	MUIC_IIC0_ERead(MUIC_ADDR, 0x04, &int2);

	printf("device_type1 : 0x%x \n", device_type1);
	printf("device_type2 : 0x%x \n", device_type2);
	printf("device_type3 : 0x%x \n", device_type3);
	printf("interrupt1 : 0x%x \n", int1);
	printf("interrupt2 : 0x%x \n", int2);

	if((device_type1 & (DEV_TYPE1_CDP | DEV_TYPE1_USB)) ||
		(device_type2 & (DEV_TYPE2_JIG_USB_OFF | DEV_TYPE2_JIG_USB_ON)) ||
		(device_type3 & DEV_TYPE3_NO_STD_CHG)) {
		MUIC_IIC0_EWrite(MUIC_ADDR, 0x2B, 0x80);
		MUIC_IIC0_EWrite(MUIC_ADDR, 0x13, 0x26); /* Set USB path */
	}
	else if((device_type1 & DEV_TYPE1_DEDICATED_CHG) ||
		(device_type3 & (DEV_TYPE3_U200_CHG | DEV_TYPE3_APPLE_CHG | DEV_TYPE3_AV_WITH_VBUS))) {
		MUIC_IIC0_EWrite(MUIC_ADDR, 0x13, 0x02); /* Set open path */
	}
	else
		MUIC_IIC0_EWrite(MUIC_ADDR, 0x13, 0x6E); /* Set UART path */

	MUIC_IIC0_EWrite(MUIC_ADDR, 0x02, 0x1B);  /* Set Manual mode */
}
