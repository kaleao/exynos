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

#include <common.h>
#include <asm/arch/cpu.h>
#include "if_pmic.h"

void IF_PMIC_IIC0_Delay(void)
{
	unsigned long i;
	for (i = 0; i < IF_PMIC_IIC0_DELAY; i++)
		;
}

void IF_PMIC_IIC0_SCLH_SDAH(void)
{
	IF_PMIC_IIC0_ESCL_Hi;
	IF_PMIC_IIC0_ESDA_Hi;
	IF_PMIC_IIC0_Delay();
}

void IF_PMIC_IIC0_SCLH_SDAL(void)
{
	IF_PMIC_IIC0_ESCL_Hi;
	IF_PMIC_IIC0_ESDA_Lo;
	IF_PMIC_IIC0_Delay();
}

void IF_PMIC_IIC0_SCLL_SDAH(void)
{
	IF_PMIC_IIC0_ESCL_Lo;
	IF_PMIC_IIC0_ESDA_Hi;
	IF_PMIC_IIC0_Delay();
}

void IF_PMIC_IIC0_SCLL_SDAL(void)
{
	IF_PMIC_IIC0_ESCL_Lo;
	IF_PMIC_IIC0_ESDA_Lo;
	IF_PMIC_IIC0_Delay();
}

void IF_PMIC_IIC0_ELow(void)
{
	IF_PMIC_IIC0_SCLL_SDAL();
	IF_PMIC_IIC0_SCLH_SDAL();
	IF_PMIC_IIC0_SCLH_SDAL();
	IF_PMIC_IIC0_SCLL_SDAL();
}

void IF_PMIC_IIC0_EHigh(void)
{
	IF_PMIC_IIC0_SCLL_SDAH();
	IF_PMIC_IIC0_SCLH_SDAH();
	IF_PMIC_IIC0_SCLH_SDAH();
	IF_PMIC_IIC0_SCLL_SDAH();
}

void IF_PMIC_IIC0_EStart(void)
{
	IF_PMIC_IIC0_SCLH_SDAH();
	IF_PMIC_IIC0_SCLH_SDAL();
	IF_PMIC_IIC0_Delay();
	IF_PMIC_IIC0_SCLL_SDAL();
}

void IF_PMIC_IIC0_EEnd(void)
{
	IF_PMIC_IIC0_SCLL_SDAL();
	IF_PMIC_IIC0_SCLH_SDAL();
	IF_PMIC_IIC0_Delay();
	IF_PMIC_IIC0_SCLH_SDAH();
}

void IF_PMIC_IIC0_EAck_write(void)
{
	unsigned long ack;

	/* Function <- Input */
	IF_PMIC_IIC0_ESDA_INP;

	IF_PMIC_IIC0_ESCL_Lo;
	IF_PMIC_IIC0_Delay();
	IF_PMIC_IIC0_ESCL_Hi;
	IF_PMIC_IIC0_Delay();
	ack = IF_PMIC_GPIO_DAT;
	IF_PMIC_IIC0_ESCL_Hi;
	IF_PMIC_IIC0_Delay();
	IF_PMIC_IIC0_ESCL_Hi;
	IF_PMIC_IIC0_Delay();

	/* Function <- Output (SDA) */
	IF_PMIC_IIC0_ESDA_OUTP;

	ack = (ack >> IF_PMIC_GPIO_DAT_SHIFT) & 0x1;

	IF_PMIC_IIC0_SCLL_SDAL();
}

void IF_PMIC_IIC0_EAck_read(void)
{
	/* Function <- Output */
	IF_PMIC_IIC0_ESDA_OUTP;

	IF_PMIC_IIC0_ESCL_Lo;
	IF_PMIC_IIC0_ESCL_Lo;
	IF_PMIC_IIC0_ESDA_Hi;
	IF_PMIC_IIC0_ESCL_Hi;
	IF_PMIC_IIC0_ESCL_Hi;
	/* Function <- Input (SDA) */
	IF_PMIC_IIC0_ESDA_INP;

	IF_PMIC_IIC0_SCLL_SDAL();
}

void IF_PMIC_IIC0_ESetport(void)
{
	/* Pull Up/Down Disable	SCL, SDA */
	IF_PMIC_DIS_GPIO_PUD;

	IF_PMIC_IIC0_ESCL_Hi;
	IF_PMIC_IIC0_ESDA_Hi;

	/* Function <- Output (SCL) */
	IF_PMIC_IIC0_ESCL_OUTP;
	/* Function <- Output (SDA) */
	IF_PMIC_IIC0_ESDA_OUTP;

	IF_PMIC_IIC0_Delay();
}

void IF_PMIC_IIC0_EWrite(unsigned char ChipId,
		unsigned char IicAddr, unsigned char IicData)
{
	unsigned long i;

	IF_PMIC_IIC0_EStart();

	/* write chip id */
	for (i = 7; i > 0; i--) {
		if ((ChipId >> i) & 0x0001)
			IF_PMIC_IIC0_EHigh();
		else
			IF_PMIC_IIC0_ELow();
	}

	/* write */
	IF_PMIC_IIC0_ELow();

	/* ACK */
	IF_PMIC_IIC0_EAck_write();

	/* write reg. addr. */
	for (i = 8; i > 0; i--) {
		if ((IicAddr >> (i-1)) & 0x0001)
			IF_PMIC_IIC0_EHigh();
		else
			IF_PMIC_IIC0_ELow();
	}

	/* ACK */
	IF_PMIC_IIC0_EAck_write();

	/* write reg. data. */
	for (i = 8; i > 0; i--) {
		if ((IicData >> (i-1)) & 0x0001)
			IF_PMIC_IIC0_EHigh();
		else
			IF_PMIC_IIC0_ELow();
	}

	/* ACK */
	IF_PMIC_IIC0_EAck_write();

	IF_PMIC_IIC0_EEnd();
}

void IF_PMIC_IIC0_ERead(unsigned char ChipId,
		unsigned char IicAddr, unsigned char *IicData)
{
	unsigned long i, reg;
	unsigned char data = 0;

	IF_PMIC_IIC0_EStart();

	/* write chip id */
	for (i = 7; i > 0; i--) {
		if ((ChipId >> i) & 0x0001)
			IF_PMIC_IIC0_EHigh();
		else
			IF_PMIC_IIC0_ELow();
	}

	/* write */
	IF_PMIC_IIC0_ELow();

	/* ACK */
	IF_PMIC_IIC0_EAck_write();

	/* write reg. addr. */
	for (i = 8; i > 0; i--) {
		if ((IicAddr >> (i-1)) & 0x0001)
			IF_PMIC_IIC0_EHigh();
		else
			IF_PMIC_IIC0_ELow();
	}

	/* ACK */
	IF_PMIC_IIC0_EAck_write();

	IF_PMIC_IIC0_EStart();

	/* write chip id */
	for (i = 7; i > 0; i--) {
		if ((ChipId >> i) & 0x0001)
			IF_PMIC_IIC0_EHigh();
		else
			IF_PMIC_IIC0_ELow();
	}

	/* read */
	IF_PMIC_IIC0_EHigh();
	/* ACK */
	IF_PMIC_IIC0_EAck_write();

	/* read reg. data. */
	IF_PMIC_IIC0_ESDA_INP;

	IF_PMIC_IIC0_ESCL_Lo;
	IF_PMIC_IIC0_ESCL_Lo;
	IF_PMIC_IIC0_Delay();

	for (i = 8; i > 0; i--) {
		IF_PMIC_IIC0_ESCL_Lo;
		IF_PMIC_IIC0_ESCL_Lo;
		IF_PMIC_IIC0_Delay();
		IF_PMIC_IIC0_ESCL_Hi;
		IF_PMIC_IIC0_ESCL_Hi;
		IF_PMIC_IIC0_Delay();
		reg = IF_PMIC_GPIO_DAT;
		IF_PMIC_IIC0_ESCL_Hi;
		IF_PMIC_IIC0_ESCL_Hi;
		IF_PMIC_IIC0_Delay();
		IF_PMIC_IIC0_ESCL_Lo;
		IF_PMIC_IIC0_ESCL_Lo;
		IF_PMIC_IIC0_Delay();

		reg = (reg >> IF_PMIC_GPIO_DAT_SHIFT) & 0x1;

		data |= reg << (i-1);
	}

	/* ACK */
	IF_PMIC_IIC0_EAck_read();
	IF_PMIC_IIC0_ESDA_OUTP;

	IF_PMIC_IIC0_EEnd();

	*IicData = data;
}

void if_pmic_init(void)
{
	unsigned char temp;

	IF_PMIC_IIC0_ESetport();

	/* Work Around for IF PMIC EVT0 */
	/* Manual Reset Disable : 0x47[3]= 0 for EVT0 charger work-around */
	IF_PMIC_IIC0_ERead(IF_PMIC_ADDR, 0x47, &temp);
	temp &= 0xF7;
	IF_PMIC_IIC0_EWrite(IF_PMIC_ADDR, 0x47, temp);

	/* Fine tuning for charger */
	IF_PMIC_IIC0_ERead(IF_PMIC_ADDR, 0x8A, &temp);
	temp &= 0xF8;
	temp |= 0x00;
	IF_PMIC_IIC0_EWrite(IF_PMIC_ADDR, 0x8A, temp);   /* [2:0] = 000 */

	IF_PMIC_IIC0_ERead(IF_PMIC_ADDR, 0x87, &temp);
	temp &= 0xF8;
	temp |= 0x00;
	IF_PMIC_IIC0_EWrite(IF_PMIC_ADDR, 0x87, temp);   /* [2:0] = 000 */

	IF_PMIC_IIC0_ERead(IF_PMIC_ADDR, 0x93, &temp);
	temp &= 0xF7;
	temp |= 0x00;
	IF_PMIC_IIC0_EWrite(IF_PMIC_ADDR, 0x93, temp);  /* [3] = 0 */

	IF_PMIC_IIC0_ERead(IF_PMIC_ADDR, 0x8B, &temp);
	temp &= 0x8F;
	temp |= 0x40;
	IF_PMIC_IIC0_EWrite(IF_PMIC_ADDR, 0x8B, temp);   /* [6:4] = 100 */

	IF_PMIC_IIC0_ERead(IF_PMIC_ADDR, 0x6A, &temp);
	temp &= 0x7F;
	temp |= 0x80;
	IF_PMIC_IIC0_EWrite(IF_PMIC_ADDR, 0x6A, temp);   /* [7] = 1 */

	IF_PMIC_IIC0_ERead(IF_PMIC_ADDR, 0x09, &temp);
	temp &= 0xE3;
	temp |= 0x00;
	IF_PMIC_IIC0_EWrite(IF_PMIC_ADDR, 0x09,temp);  /* [4:2] = 000 */
}
