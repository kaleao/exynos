/*
 * (C) Copyright 2013 Samsung Electronics Co. Ltd
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
#include "pmic.h"

void Delay(void)
{
	unsigned long i;
	for (i = 0; i < DELAY; i++);
}

void IIC0_SCLH_SDAH(void)
{
	IIC0_ESCL_Hi;
	IIC0_ESDA_Hi;
	Delay();
}

void IIC0_SCLH_SDAL(void)
{
	IIC0_ESCL_Hi;
	IIC0_ESDA_Lo;
	Delay();
}

void IIC0_SCLL_SDAH(void)
{
	IIC0_ESCL_Lo;
	IIC0_ESDA_Hi;
	Delay();
}

void IIC0_SCLL_SDAL(void)
{
	IIC0_ESCL_Lo;
	IIC0_ESDA_Lo;
	Delay();
}

void IIC0_ELow(void)
{
	IIC0_SCLL_SDAL();
	IIC0_SCLH_SDAL();
	IIC0_SCLH_SDAL();
	IIC0_SCLL_SDAL();
}

void IIC0_EHigh(void)
{
	IIC0_SCLL_SDAH();
	IIC0_SCLH_SDAH();
	IIC0_SCLH_SDAH();
	IIC0_SCLL_SDAH();
}

void IIC0_EStart(void)
{
	IIC0_SCLH_SDAH();
	IIC0_SCLH_SDAL();
	Delay();
	IIC0_SCLL_SDAL();
}

void IIC0_EEnd(void)
{
	IIC0_SCLL_SDAL();
	IIC0_SCLH_SDAL();
	Delay();
	IIC0_SCLH_SDAH();
}

void IIC0_EAck_write(void)
{
	unsigned long ack;

	/* Function <- Input */
	IIC0_ESDA_INP;

	IIC0_ESCL_Lo;
	Delay();
	IIC0_ESCL_Hi;
	Delay();
	ack = GPD1DAT;
	IIC0_ESCL_Hi;
	Delay();
	IIC0_ESCL_Hi;
	Delay();

	/* Function <- Output (SDA) */
	IIC0_ESDA_OUTP;

	ack = (ack>>0)&0x1;

	IIC0_SCLL_SDAL();
}

void IIC0_EAck_read(void)
{
	/* Function <- Output */
	IIC0_ESDA_OUTP;

	IIC0_ESCL_Lo;
	IIC0_ESCL_Lo;
	IIC0_ESDA_Hi;
	IIC0_ESCL_Hi;
	IIC0_ESCL_Hi;
	/* Function <- Input (SDA) */
	IIC0_ESDA_INP;

	IIC0_SCLL_SDAL();
}

void IIC0_ESetport(void)
{
	/* Pull Up/Down Disable	SCL, SDA */
	GPD1PUD &= ~(0xf<<0);

	IIC0_ESCL_Hi;
	IIC0_ESDA_Hi;

	/* Function <- Output (SCL) */
	IIC0_ESCL_OUTP;
	/* Function <- Output (SDA) */
	IIC0_ESDA_OUTP;

	Delay();
}

void IIC0_EWrite(unsigned char ChipId,
		unsigned char IicAddr, unsigned char IicData)
{
	unsigned long i;

	IIC0_EStart();

	/* write chip id */
	for (i = 7; i > 0; i--) {
		if ((ChipId >> i) & 0x0001)
			IIC0_EHigh();
		else
			IIC0_ELow();
	}

	/* write */
	IIC0_ELow();

	/* ACK */
	IIC0_EAck_write();

	/* write reg. addr. */
	for (i = 8; i > 0; i--) {
		if ((IicAddr >> (i-1)) & 0x0001)
			IIC0_EHigh();
		else
			IIC0_ELow();
	}

	/* ACK */
	IIC0_EAck_write();

	/* write reg. data. */
	for (i = 8; i > 0; i--) {
		if ((IicData >> (i-1)) & 0x0001)
			IIC0_EHigh();
		else
			IIC0_ELow();
	}

	/* ACK */
	IIC0_EAck_write();

	IIC0_EEnd();
}

void IIC0_ERead(unsigned char ChipId,
		unsigned char IicAddr, unsigned char *IicData)
{
	unsigned long i, reg;
	unsigned char data = 0;

	IIC0_EStart();

	/* write chip id */
	for (i = 7; i > 0; i--) {
		if ((ChipId >> i) & 0x0001)
			IIC0_EHigh();
		else
			IIC0_ELow();
	}

	/* write */
	IIC0_ELow();

	/* ACK */
	IIC0_EAck_write();

	/* write reg. addr. */
	for (i = 8; i > 0; i--) {
		if ((IicAddr >> (i-1)) & 0x0001)
			IIC0_EHigh();
		else
			IIC0_ELow();
	}

	/* ACK */
	IIC0_EAck_write();

	IIC0_EStart();

	/* write chip id */
	for (i = 7; i > 0; i--) {
		if ((ChipId >> i) & 0x0001)
			IIC0_EHigh();
		else
			IIC0_ELow();
	}

	/* read */
	IIC0_EHigh();
	/* ACK */
	IIC0_EAck_write();

	/* read reg. data. */
	IIC0_ESDA_INP;

	IIC0_ESCL_Lo;
	IIC0_ESCL_Lo;
	Delay();

	for (i = 8; i > 0; i--) {
		IIC0_ESCL_Lo;
		IIC0_ESCL_Lo;
		Delay();
		IIC0_ESCL_Hi;
		IIC0_ESCL_Hi;
		Delay();
		reg = GPD1DAT;
		IIC0_ESCL_Hi;
		IIC0_ESCL_Hi;
		Delay();
		IIC0_ESCL_Lo;
		IIC0_ESCL_Lo;
		Delay();

		reg = (reg >> 0) & 0x1;

		data |= reg << (i-1);
	}

	/* ACK */
	IIC0_EAck_read();
	IIC0_ESDA_OUTP;

	IIC0_EEnd();

	*IicData = data;
}

void PMIC_Delay(int t)
{
	volatile u32 i;
	for(;t > 0; t--)
		for(i=0; i<1000; i++) ;
}

void pmic_init(void)
{
	unsigned char reg, tmp;

	IIC0_ESetport();

	/* 0x54[7:6] = 01   eMMC Buck6 Mode '01' */
	IIC0_ERead(S5M8767_RD_ADDR, 0x54, &reg);
	tmp = reg;
	reg &= ~0xC0;
	reg |= 0x40 & 0xC0;
	IIC0_EWrite(S5M8767_WR_ADDR, 0x54, reg);
	if((tmp & 0xC0) == 0) {
		PMIC_Delay(0x300);
	}

	/* 0x5A[7:6] = 01   eMMC Buck9 Mode '01' */
	IIC0_ERead(S5M8767_RD_ADDR, 0x5A, &reg);
	reg &= ~0xC0;
	reg |= 0x40 & 0xC0;
	IIC0_EWrite(S5M8767_WR_ADDR, 0x5A, reg);

	/* 0x62[7:6] = 00   eMMC LDO4 Mode '00' */
	IIC0_ERead(S5M8767_RD_ADDR, 0x62, &reg);
	reg &= ~0xC0;
	reg |= 0x00 & 0xC0;
	IIC0_EWrite(S5M8767_WR_ADDR, 0x62, reg);

	/* 0x34[5] = 0   ARM Buck2 remote sense off */
	IIC0_ERead(S5M8767_RD_ADDR, 0x34, &reg);
	reg &= ~0x20;
	reg |= 0x00 & 0x20;
	IIC0_EWrite(S5M8767_WR_ADDR, 0x34, reg);

	/* 0x46[5] = 0   G3D Buck4 remote sense off */
	IIC0_ERead(S5M8767_RD_ADDR, 0x46, &reg);
	reg &= ~0x20;
	reg |= 0x00 & 0x20;
	IIC0_EWrite(S5M8767_WR_ADDR, 0x46, reg);

	/* 0x0A[3:0] = 1111 Low jitter & 32kHz CLK on */
	IIC0_ERead(S5M8767_RD_ADDR, 0x0A, &reg);
	reg &= ~0x0F;
	reg |= 0x0F & 0x0F;
	IIC0_EWrite(S5M8767_WR_ADDR, 0x0A, reg);

	/* BUCK1_CTRL2: VDD_MIF*/
	IIC0_EWrite(S5M8767_WR_ADDR, 0x33, 0x44);
	/* BUCK2_DVS1: VDD_CPU*/
	IIC0_EWrite(S5M8767_WR_ADDR, 0x35, 0x6C);
	/* BUCK3_DVS1: VDD_INT*/
	IIC0_EWrite(S5M8767_WR_ADDR, 0x3e, 0x44);
	/* BUCK4_DVS1: VDD_G3D*/
	IIC0_EWrite(S5M8767_WR_ADDR, 0x47, 0x48);

	/* LDO5_CTRL : disable LDO5_EN */
	IIC0_ERead(S5M8767_RD_ADDR, 0x63, &reg);
	reg &= ~0xC0;
	IIC0_EWrite(S5M8767_WR_ADDR, 0x63, reg);
}

void pmic_enable_peric_dev(void)
{
	IIC0_ESetport();

	/* enable LDO9 and set 1.8V for UART */
	IIC0_EWrite(S5M8767_WR_ADDR, 0x67, 0xD4);
}

