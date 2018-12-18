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

static void Delay(void)
{
	unsigned long i;
	for (i = 0; i < DELAY; i++)
		;
}

void IIC_MUIC_SCLH_SDAH(void)
{
	IIC_MUIC_ESCL_Hi;
	IIC_MUIC_ESDA_Hi;
	Delay();
}

void IIC_MUIC_SCLH_SDAL(void)
{
	IIC_MUIC_ESCL_Hi;
	IIC_MUIC_ESDA_Lo;
	Delay();
}

void IIC_MUIC_SCLL_SDAH(void)
{
	IIC_MUIC_ESCL_Lo;
	IIC_MUIC_ESDA_Hi;
	Delay();
}

void IIC_MUIC_SCLL_SDAL(void)
{
	IIC_MUIC_ESCL_Lo;
	IIC_MUIC_ESDA_Lo;
	Delay();
}

void IIC_MUIC_ELow(void)
{
	IIC_MUIC_SCLL_SDAL();
	IIC_MUIC_SCLH_SDAL();
	IIC_MUIC_SCLH_SDAL();
	IIC_MUIC_SCLL_SDAL();
}

void IIC_MUIC_EHigh(void)
{
	IIC_MUIC_SCLL_SDAH();
	IIC_MUIC_SCLH_SDAH();
	IIC_MUIC_SCLH_SDAH();
	IIC_MUIC_SCLL_SDAH();
}

void IIC_MUIC_EStart(void)
{
	IIC_MUIC_SCLH_SDAH();
	IIC_MUIC_SCLH_SDAL();
	Delay();
	IIC_MUIC_SCLL_SDAL();
}

void IIC_MUIC_EEnd(void)
{
	IIC_MUIC_SCLL_SDAL();
	IIC_MUIC_SCLH_SDAL();
	Delay();
	IIC_MUIC_SCLH_SDAH();
}

void IIC_MUIC_EAck_write(void)
{
	unsigned long ack;

	/* Function <- Input */
	IIC_MUIC_ESDA_INP;

	IIC_MUIC_ESCL_Lo;
	Delay();
	IIC_MUIC_ESCL_Hi;
	Delay();
	ack = GPIO_DAT;
	IIC_MUIC_ESCL_Hi;
	Delay();
	IIC_MUIC_ESCL_Hi;
	Delay();

	/* Function <- Output (SDA) */
	IIC_MUIC_ESDA_OUTP;

	ack = (ack >> GPIO_DAT_SHIFT) & 0x1;

	IIC_MUIC_SCLL_SDAL();
}

void IIC_MUIC_EAck_read(void)
{
	/* Function <- Output */
	IIC_MUIC_ESDA_OUTP;

	IIC_MUIC_ESCL_Lo;
	IIC_MUIC_ESCL_Lo;
	IIC_MUIC_ESDA_Hi;
	IIC_MUIC_ESCL_Hi;
	IIC_MUIC_ESCL_Hi;
	/* Function <- Input (SDA) */
	IIC_MUIC_ESDA_INP;

	IIC_MUIC_SCLL_SDAL();
}

void IIC_MUIC_ESetport(void)
{
	/* Pull Up/Down Disable	SCL, SDA */
	DIS_GPIO_PUD;

	IIC_MUIC_ESCL_Hi;
	IIC_MUIC_ESDA_Hi;

	/* Function <- Output (SCL) */
	IIC_MUIC_ESCL_OUTP;
	/* Function <- Output (SDA) */
	IIC_MUIC_ESDA_OUTP;

	Delay();
}

void IIC_MUIC_EWrite(unsigned char ChipId,
		unsigned char IicAddr, unsigned char IicData)
{
	unsigned long i;

	IIC_MUIC_EStart();

	/* write chip id */
	for (i = 7; i > 0; i--) {
		if ((ChipId >> i) & 0x0001)
			IIC_MUIC_EHigh();
		else
			IIC_MUIC_ELow();
	}

	/* write */
	IIC_MUIC_ELow();

	/* ACK */
	IIC_MUIC_EAck_write();

	/* write reg. addr. */
	for (i = 8; i > 0; i--) {
		if ((IicAddr >> (i-1)) & 0x0001)
			IIC_MUIC_EHigh();
		else
			IIC_MUIC_ELow();
	}

	/* ACK */
	IIC_MUIC_EAck_write();

	/* write reg. data. */
	for (i = 8; i > 0; i--) {
		if ((IicData >> (i-1)) & 0x0001)
			IIC_MUIC_EHigh();
		else
			IIC_MUIC_ELow();
	}

	/* ACK */
	IIC_MUIC_EAck_write();

	IIC_MUIC_EEnd();
}

void IIC_MUIC_ERead(unsigned char ChipId,
		unsigned char IicAddr, unsigned char *IicData)
{
	unsigned long i, reg;
	unsigned char data = 0;

	IIC_MUIC_EStart();

	/* write chip id */
	for (i = 7; i > 0; i--) {
		if ((ChipId >> i) & 0x0001)
			IIC_MUIC_EHigh();
		else
			IIC_MUIC_ELow();
	}

	/* write */
	IIC_MUIC_ELow();

	/* ACK */
	IIC_MUIC_EAck_write();

	/* write reg. addr. */
	for (i = 8; i > 0; i--) {
		if ((IicAddr >> (i-1)) & 0x0001)
			IIC_MUIC_EHigh();
		else
			IIC_MUIC_ELow();
	}

	/* ACK */
	IIC_MUIC_EAck_write();

	IIC_MUIC_EStart();

	/* write chip id */
	for (i = 7; i > 0; i--) {
		if ((ChipId >> i) & 0x0001)
			IIC_MUIC_EHigh();
		else
			IIC_MUIC_ELow();
	}

	/* read */
	IIC_MUIC_EHigh();
	/* ACK */
	IIC_MUIC_EAck_write();

	/* read reg. data. */
	IIC_MUIC_ESDA_INP;

	IIC_MUIC_ESCL_Lo;
	IIC_MUIC_ESCL_Lo;
	Delay();

	for (i = 8; i > 0; i--) {
		IIC_MUIC_ESCL_Lo;
		IIC_MUIC_ESCL_Lo;
		Delay();
		IIC_MUIC_ESCL_Hi;
		IIC_MUIC_ESCL_Hi;
		Delay();
		reg = GPIO_DAT;
		IIC_MUIC_ESCL_Hi;
		IIC_MUIC_ESCL_Hi;
		Delay();
		IIC_MUIC_ESCL_Lo;
		IIC_MUIC_ESCL_Lo;
		Delay();

		reg = (reg >> GPIO_DAT_SHIFT) & 0x1;

		data |= reg << (i-1);
	}

	/* ACK */
	IIC_MUIC_EAck_read();
	IIC_MUIC_ESDA_OUTP;

	IIC_MUIC_EEnd();

	*IicData = data;
}
