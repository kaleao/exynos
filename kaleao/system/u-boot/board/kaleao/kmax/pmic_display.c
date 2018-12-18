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
#include "pmic_display.h"

#ifndef CONFIG_SG_BINARY
void Delay(void);

void IIC_DISP_SCLH_SDAH(void)
{
	IIC_DISP_ESCL_Hi;
	IIC_DISP_ESDA_Hi;
	Delay();
}

void IIC_DISP_SCLH_SDAL(void)
{
	IIC_DISP_ESCL_Hi;
	IIC_DISP_ESDA_Lo;
	Delay();
}

void IIC_DISP_SCLL_SDAH(void)
{
	IIC_DISP_ESCL_Lo;
	IIC_DISP_ESDA_Hi;
	Delay();
}

void IIC_DISP_SCLL_SDAL(void)
{
	IIC_DISP_ESCL_Lo;
	IIC_DISP_ESDA_Lo;
	Delay();
}

void IIC_DISP_ELow(void)
{
	IIC_DISP_SCLL_SDAL();
	IIC_DISP_SCLH_SDAL();
	IIC_DISP_SCLH_SDAL();
	IIC_DISP_SCLL_SDAL();
}

void IIC_DISP_EHigh(void)
{
	IIC_DISP_SCLL_SDAH();
	IIC_DISP_SCLH_SDAH();
	IIC_DISP_SCLH_SDAH();
	IIC_DISP_SCLL_SDAH();
}

void IIC_DISP_EStart(void)
{
	IIC_DISP_SCLH_SDAH();
	IIC_DISP_SCLH_SDAL();
	Delay();
	IIC_DISP_SCLL_SDAL();
}

void IIC_DISP_EEnd(void)
{
	IIC_DISP_SCLL_SDAL();
	IIC_DISP_SCLH_SDAL();
	Delay();
	IIC_DISP_SCLH_SDAH();
}

void IIC_DISP_EAck_write(void)
{
	unsigned long ack;

	/* Function <- Input */
	IIC_DISP_ESDA_INP;

	IIC_DISP_ESCL_Lo;
	Delay();
	IIC_DISP_ESCL_Hi;
	Delay();
	ack = GPIO_DAT;
	IIC_DISP_ESCL_Hi;
	Delay();
	IIC_DISP_ESCL_Hi;
	Delay();

	/* Function <- Output (SDA) */
	IIC_DISP_ESDA_OUTP;

	ack = (ack >> GPIO_DAT_SHIFT) & 0x1;

	IIC_DISP_SCLL_SDAL();
}

void IIC_DISP_EAck_read(void)
{
	/* Function <- Output */
	IIC_DISP_ESDA_OUTP;

	IIC_DISP_ESCL_Lo;
	IIC_DISP_ESCL_Lo;
	IIC_DISP_ESDA_Hi;
	IIC_DISP_ESCL_Hi;
	IIC_DISP_ESCL_Hi;
	/* Function <- Input (SDA) */
	IIC_DISP_ESDA_INP;

	IIC_DISP_SCLL_SDAL();
}

void IIC_DISP_ESetport(void)
{
	/* Pull Up/Down Disable	SCL, SDA */
	DIS_GPIO_PUD;

	IIC_DISP_ESCL_Hi;
	IIC_DISP_ESDA_Hi;

	/* Function <- Output (SCL) */
	IIC_DISP_ESCL_OUTP;
	/* Function <- Output (SDA) */
	IIC_DISP_ESDA_OUTP;

	Delay();
}

void IIC_DISP_EWrite(unsigned char ChipId,
		unsigned char IicAddr, unsigned char IicData)
{
	unsigned long i;

	IIC_DISP_EStart();

	/* write chip id */
	for (i = 7; i > 0; i--) {
		if ((ChipId >> i) & 0x0001)
			IIC_DISP_EHigh();
		else
			IIC_DISP_ELow();
	}

	/* write */
	IIC_DISP_ELow();

	/* ACK */
	IIC_DISP_EAck_write();

	/* write reg. addr. */
	for (i = 8; i > 0; i--) {
		if ((IicAddr >> (i-1)) & 0x0001)
			IIC_DISP_EHigh();
		else
			IIC_DISP_ELow();
	}

	/* ACK */
	IIC_DISP_EAck_write();

	/* write reg. data. */
	for (i = 8; i > 0; i--) {
		if ((IicData >> (i-1)) & 0x0001)
			IIC_DISP_EHigh();
		else
			IIC_DISP_ELow();
	}

	/* ACK */
	IIC_DISP_EAck_write();

	IIC_DISP_EEnd();
}

void IIC_DISP_ERead(unsigned char ChipId,
		unsigned char IicAddr, unsigned char *IicData)
{
	unsigned long i, reg;
	unsigned char data = 0;

	IIC_DISP_EStart();

	/* write chip id */
	for (i = 7; i > 0; i--) {
		if ((ChipId >> i) & 0x0001)
			IIC_DISP_EHigh();
		else
			IIC_DISP_ELow();
	}

	/* write */
	IIC_DISP_ELow();

	/* ACK */
	IIC_DISP_EAck_write();

	/* write reg. addr. */
	for (i = 8; i > 0; i--) {
		if ((IicAddr >> (i-1)) & 0x0001)
			IIC_DISP_EHigh();
		else
			IIC_DISP_ELow();
	}

	/* ACK */
	IIC_DISP_EAck_write();

	IIC_DISP_EStart();

	/* write chip id */
	for (i = 7; i > 0; i--) {
		if ((ChipId >> i) & 0x0001)
			IIC_DISP_EHigh();
		else
			IIC_DISP_ELow();
	}

	/* read */
	IIC_DISP_EHigh();
	/* ACK */
	IIC_DISP_EAck_write();

	/* read reg. data. */
	IIC_DISP_ESDA_INP;

	IIC_DISP_ESCL_Lo;
	IIC_DISP_ESCL_Lo;
	Delay();

	for (i = 8; i > 0; i--) {
		IIC_DISP_ESCL_Lo;
		IIC_DISP_ESCL_Lo;
		Delay();
		IIC_DISP_ESCL_Hi;
		IIC_DISP_ESCL_Hi;
		Delay();
		reg = GPIO_DAT;
		IIC_DISP_ESCL_Hi;
		IIC_DISP_ESCL_Hi;
		Delay();
		IIC_DISP_ESCL_Lo;
		IIC_DISP_ESCL_Lo;
		Delay();

		reg = (reg >> GPIO_DAT_SHIFT) & 0x1;

		data |= reg << (i-1);
	}

	/* ACK */
	IIC_DISP_EAck_read();
	IIC_DISP_ESDA_OUTP;

	IIC_DISP_EEnd();

	*IicData = data;
}

void pmic_display_init(void)
{
	unsigned char reg_en = 0;

	IIC_DISP_ESetport();

	IIC_DISP_ERead(MAX77838_R_ADDR, MAX77838_REG_EN, &reg_en);
	reg_en = (L1_EN | L2_EN | L3_EN | L4_EN | B_EN);
	IIC_DISP_EWrite(MAX77838_W_ADDR, MAX77838_REG_EN, reg_en);
}
#endif

