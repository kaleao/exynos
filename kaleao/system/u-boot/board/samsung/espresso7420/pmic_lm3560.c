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
#include "pmic_lm3560.h"

static void Delay(void)
{
	unsigned long i;
	for (i = 0; i < DELAY; i++)
		;
}

void IIC2_SCLH_SDAH(void)
{
	IIC2_ESCL_Hi;
	IIC2_ESDA_Hi;
	Delay();
}

void IIC2_SCLH_SDAL(void)
{
	IIC2_ESCL_Hi;
	IIC2_ESDA_Lo;
	Delay();
}

void IIC2_SCLL_SDAH(void)
{
	IIC2_ESCL_Lo;
	IIC2_ESDA_Hi;
	Delay();
}

void IIC2_SCLL_SDAL(void)
{
	IIC2_ESCL_Lo;
	IIC2_ESDA_Lo;
	Delay();
}

void IIC2_ELow(void)
{
	IIC2_SCLL_SDAL();
	IIC2_SCLH_SDAL();
	IIC2_SCLH_SDAL();
	IIC2_SCLL_SDAL();
}

void IIC2_EHigh(void)
{
	IIC2_SCLL_SDAH();
	IIC2_SCLH_SDAH();
	IIC2_SCLH_SDAH();
	IIC2_SCLL_SDAH();
}

void IIC2_EStart(void)
{
	IIC2_SCLH_SDAH();
	IIC2_SCLH_SDAL();
	Delay();
	IIC2_SCLL_SDAL();
}

void IIC2_EEnd(void)
{
	IIC2_SCLL_SDAL();
	IIC2_SCLH_SDAL();
	Delay();
	IIC2_SCLH_SDAH();
}

void IIC2_EAck_write(void)
{
	unsigned long ack;

	/* Function <- Input */
	IIC2_ESDA_INP;

	IIC2_ESCL_Lo;
	Delay();
	IIC2_ESCL_Hi;
	Delay();
	ack = GPIO_DAT;
	IIC2_ESCL_Hi;
	Delay();
	IIC2_ESCL_Hi;
	Delay();

	/* Function <- Output (SDA) */
	IIC2_ESDA_OUTP;

	ack = (ack >> GPIO_DAT_SHIFT) & 0x1;

	IIC2_SCLL_SDAL();
}

void IIC2_EAck_read(void)
{
	/* Function <- Output */
	IIC2_ESDA_OUTP;

	IIC2_ESCL_Lo;
	IIC2_ESCL_Lo;
	IIC2_ESDA_Hi;
	IIC2_ESCL_Hi;
	IIC2_ESCL_Hi;
	/* Function <- Input (SDA) */
	IIC2_ESDA_INP;

	IIC2_SCLL_SDAL();
}

void IIC2_ESetport(void)
{
	/* Pull Up/Down Disable	SCL, SDA */
	DIS_GPIO_PUD;

	IIC2_ESCL_Hi;
	IIC2_ESDA_Hi;

	/* Function <- Output (SCL) */
	IIC2_ESCL_OUTP;
	/* Function <- Output (SDA) */
	IIC2_ESDA_OUTP;

	Delay();
}

void IIC2_EWrite(unsigned char ChipId,
		unsigned char IicAddr, unsigned char IicData)
{
	unsigned long i;

	IIC2_EStart();

	/* write chip id */
	for (i = 7; i > 0; i--) {
		if ((ChipId >> i) & 0x0001)
			IIC2_EHigh();
		else
			IIC2_ELow();
	}

	/* write */
	IIC2_ELow();

	/* ACK */
	IIC2_EAck_write();

	/* write reg. addr. */
	for (i = 8; i > 0; i--) {
		if ((IicAddr >> (i-1)) & 0x0001)
			IIC2_EHigh();
		else
			IIC2_ELow();
	}

	/* ACK */
	IIC2_EAck_write();

	/* write reg. data. */
	for (i = 8; i > 0; i--) {
		if ((IicData >> (i-1)) & 0x0001)
			IIC2_EHigh();
		else
			IIC2_ELow();
	}

	/* ACK */
	IIC2_EAck_write();

	IIC2_EEnd();
}

void IIC2_ERead(unsigned char ChipId,
		unsigned char IicAddr, unsigned char *IicData)
{
	unsigned long i, reg;
	unsigned char data = 0;

	IIC2_EStart();

	/* write chip id */
	for (i = 7; i > 0; i--) {
		if ((ChipId >> i) & 0x0001)
			IIC2_EHigh();
		else
			IIC2_ELow();
	}

	/* write */
	IIC2_ELow();

	/* ACK */
	IIC2_EAck_write();

	/* write reg. addr. */
	for (i = 8; i > 0; i--) {
		if ((IicAddr >> (i-1)) & 0x0001)
			IIC2_EHigh();
		else
			IIC2_ELow();
	}

	/* ACK */
	IIC2_EAck_write();

	IIC2_EStart();

	/* write chip id */
	for (i = 7; i > 0; i--) {
		if ((ChipId >> i) & 0x0001)
			IIC2_EHigh();
		else
			IIC2_ELow();
	}

	/* read */
	IIC2_EHigh();
	/* ACK */
	IIC2_EAck_write();

	/* read reg. data. */
	IIC2_ESDA_INP;

	IIC2_ESCL_Lo;
	IIC2_ESCL_Lo;
	Delay();

	for (i = 8; i > 0; i--) {
		IIC2_ESCL_Lo;
		IIC2_ESCL_Lo;
		Delay();
		IIC2_ESCL_Hi;
		IIC2_ESCL_Hi;
		Delay();
		reg = GPIO_DAT;
		IIC2_ESCL_Hi;
		IIC2_ESCL_Hi;
		Delay();
		IIC2_ESCL_Lo;
		IIC2_ESCL_Lo;
		Delay();

		reg = (reg >> GPIO_DAT_SHIFT) & 0x1;

		data |= reg << (i-1);
	}

	/* ACK */
	IIC2_EAck_read();
	IIC2_ESDA_OUTP;

	IIC2_EEnd();

	*IicData = data;
}

