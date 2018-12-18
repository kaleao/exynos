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
#include <asm/arch/smc.h>
#include "charge.h"
#include "asv-exynos_cal.h"



void Delay_CH(void)
{
	unsigned long i;
	for (i = 0; i < DELAY_CH; i++)
		;
}
void IIC_SCLH_SDAH(void)
{
	IIC_ESCL_Hi;
	IIC_ESDA_Hi;
	Delay_CH();
}

void IIC_SCLH_SDAL(void)
{
	IIC_ESCL_Hi;
	IIC_ESDA_Lo;
	Delay_CH();
}

void IIC_SCLL_SDAH(void)
{
	IIC_ESCL_Lo;
	IIC_ESDA_Hi;
	Delay_CH();
}

void IIC_SCLL_SDAL(void)
{
	IIC_ESCL_Lo;
	IIC_ESDA_Lo;
	Delay_CH();
}

void IIC_ELow(void)
{
	IIC_SCLL_SDAL();
	IIC_SCLH_SDAL();
	IIC_SCLH_SDAL();
	IIC_SCLL_SDAL();
}

void IIC_EHigh(void)
{
	IIC_SCLL_SDAH();
	IIC_SCLH_SDAH();
	IIC_SCLH_SDAH();
	IIC_SCLL_SDAH();
}

void IIC_EStart(void)
{
	IIC_SCLH_SDAH();
	IIC_SCLH_SDAL();
	Delay_CH();
	IIC_SCLL_SDAL();
}

void IIC_EEnd(void)
{
	IIC_SCLL_SDAL();
	IIC_SCLH_SDAL();
	Delay_CH();
	IIC_SCLH_SDAH();
}

void IIC_EAck_write(void)
{
	unsigned long ack;

	/* Function <- Input */
	IIC_ESDA_INP;

	IIC_ESCL_Lo;
	Delay_CH();
	IIC_ESCL_Hi;
	Delay_CH();
	ack = GPIO_CHARGE_DAT;
	IIC_ESCL_Hi;
	Delay_CH();
	IIC_ESCL_Hi;
	Delay_CH();

	/* Function <- Output (SDA) */
	IIC_ESDA_OUTP;

	ack = (ack >> GPIO_CHARGE_DAT_SHIFT) & 0x1;

	IIC_SCLL_SDAL();
}

void IIC_EAck_read(void)
{
	/* Function <- Output */
	IIC_ESDA_OUTP;

	IIC_ESCL_Lo;
	IIC_ESCL_Lo;
	IIC_ESDA_Hi;
	IIC_ESCL_Hi;
	IIC_ESCL_Hi;
	/* Function <- Input (SDA) */
	IIC_ESDA_INP;

	IIC_SCLL_SDAL();
}

void IIC_ESetport(void)
{
	/* Pull Up/Down Disable	SCL, SDA */
	DIS_GPIO_CHARGE_PUD;

	IIC_ESCL_Hi;
	IIC_ESDA_Hi;

	/* Function <- Output (SCL) */
	IIC_ESCL_OUTP;
	/* Function <- Output (SDA) */
	IIC_ESDA_OUTP;

	Delay_CH();
}

void IIC_EWrite(unsigned char ChipId,
		unsigned char IicAddr, unsigned char IicData)
{
	unsigned long i;

	IIC_EStart();

	/* write chip id */
	for (i = 7; i > 0; i--) {
		if ((ChipId >> i) & 0x0001)
			IIC_EHigh();
		else
			IIC_ELow();
	}

	/* write */
	IIC_ELow();

	/* ACK */
	IIC_EAck_write();

	/* write reg. addr. */
	for (i = 8; i > 0; i--) {
		if ((IicAddr >> (i-1)) & 0x0001)
			IIC_EHigh();
		else
			IIC_ELow();
	}

	/* ACK */
	IIC_EAck_write();

	/* write reg. data. */
	for (i = 8; i > 0; i--) {
		if ((IicData >> (i-1)) & 0x0001)
			IIC_EHigh();
		else
			IIC_ELow();
	}

	/* ACK */
	IIC_EAck_write();

	IIC_EEnd();
}

void IIC_ERead(unsigned char ChipId,
		unsigned char IicAddr, unsigned char *IicData)
{
	unsigned long i, reg;
	unsigned char data = 0;

	IIC_EStart();

	/* write chip id */
	for (i = 7; i > 0; i--) {
		if ((ChipId >> i) & 0x0001)
			IIC_EHigh();
		else
			IIC_ELow();
	}

	/* write */
	IIC_ELow();

	/* ACK */
	IIC_EAck_write();

	/* write reg. addr. */
	for (i = 8; i > 0; i--) {
		if ((IicAddr >> (i-1)) & 0x0001)
			IIC_EHigh();
		else
			IIC_ELow();
	}

	/* ACK */
	IIC_EAck_write();

	IIC_EStart();

	/* write chip id */
	for (i = 7; i > 0; i--) {
		if ((ChipId >> i) & 0x0001)
			IIC_EHigh();
		else
			IIC_ELow();
	}

	/* read */
	IIC_EHigh();
	/* ACK */
	IIC_EAck_write();

	/* read reg. data. */
	IIC_ESDA_INP;

	IIC_ESCL_Lo;
	IIC_ESCL_Lo;
	Delay_CH();

	for (i = 8; i > 0; i--) {
		IIC_ESCL_Lo;
		IIC_ESCL_Lo;
		Delay_CH();
		IIC_ESCL_Hi;
		IIC_ESCL_Hi;
		Delay_CH();
		reg = GPIO_CHARGE_DAT;
		IIC_ESCL_Hi;
		IIC_ESCL_Hi;
		Delay_CH();
		IIC_ESCL_Lo;
		IIC_ESCL_Lo;
		Delay_CH();

		reg = (reg >> GPIO_CHARGE_DAT_SHIFT) & 0x1;

		data |= reg << (i-1);
	}

	/* ACK */
	IIC_EAck_read();
	IIC_ESDA_OUTP;

	IIC_EEnd();

	*IicData = data;
}


void charge_init(void)
{
	IIC_ESetport();

}


