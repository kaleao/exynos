/*
 * Copyright (C) 2014 Samsung Electronics
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include "pmic.h"

void Delay(void)
{
	unsigned long i;
	for(i=0;i<DELAY;i++);
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

	IIC0_ESDA_INP;			// Function <- Input

	IIC0_ESCL_Lo;
	Delay();
	IIC0_ESCL_Hi;
	Delay();
	ack = GPD1DAT;
	IIC0_ESCL_Hi;
	Delay();
	IIC0_ESCL_Hi;
	Delay();

	IIC0_ESDA_OUTP;			// Function <- Output (SDA)

	ack = (ack>>0)&0x1;
//	while(ack!=0);

	IIC0_SCLL_SDAL();
}

void IIC0_EAck_read(void)
{
	IIC0_ESDA_OUTP;			// Function <- Output

	IIC0_ESCL_Lo;
	IIC0_ESCL_Lo;
	IIC0_ESDA_Hi;
	IIC0_ESCL_Hi;
	IIC0_ESCL_Hi;

	IIC0_ESDA_INP;			// Function <- Input (SDA)

	IIC0_SCLL_SDAL();
}

void IIC0_ESetport(void)
{
	GPD1PUD &= ~(0xf<<0);	// Pull Up/Down Disable	SCL, SDA

	IIC0_ESCL_Hi;
	IIC0_ESDA_Hi;

	IIC0_ESCL_OUTP;		// Function <- Output (SCL)
	IIC0_ESDA_OUTP;		// Function <- Output (SDA)

	Delay();
}

void IIC0_EWrite (unsigned char ChipId, unsigned char IicAddr, unsigned char IicData)
{
	unsigned long i;

	IIC0_EStart();

////////////////// write chip id //////////////////
	for(i = 7; i>0; i--)
	{
		if((ChipId >> i) & 0x0001)
			IIC0_EHigh();
		else
			IIC0_ELow();
	}

	IIC0_ELow();	// write

	IIC0_EAck_write();	// ACK

////////////////// write reg. addr. //////////////////
	for(i = 8; i>0; i--)
	{
		if((IicAddr >> (i-1)) & 0x0001)
			IIC0_EHigh();
		else
			IIC0_ELow();
	}

	IIC0_EAck_write();	// ACK

////////////////// write reg. data. //////////////////
	for(i = 8; i>0; i--)
	{
		if((IicData >> (i-1)) & 0x0001)
			IIC0_EHigh();
		else
			IIC0_ELow();
	}

	IIC0_EAck_write();	// ACK

	IIC0_EEnd();
}

void IIC0_ERead (unsigned char ChipId, unsigned char IicAddr, unsigned char *IicData)
{
	unsigned long i, reg;
	unsigned char data = 0;

	IIC0_EStart();

////////////////// write chip id //////////////////
	for(i = 7; i>0; i--)
	{
		if((ChipId >> i) & 0x0001)
			IIC0_EHigh();
		else
			IIC0_ELow();
	}

	IIC0_ELow();	// write

	IIC0_EAck_write();	// ACK

////////////////// write reg. addr. //////////////////
	for(i = 8; i>0; i--)
	{
		if((IicAddr >> (i-1)) & 0x0001)
			IIC0_EHigh();
		else
			IIC0_ELow();
	}

	IIC0_EAck_write();	// ACK

	IIC0_EStart();

////////////////// write chip id //////////////////
	for(i = 7; i>0; i--)
	{
		if((ChipId >> i) & 0x0001)
			IIC0_EHigh();
		else
			IIC0_ELow();
	}

	IIC0_EHigh();	// read

	IIC0_EAck_write();	// ACK

////////////////// read reg. data. //////////////////
	IIC0_ESDA_INP;

	IIC0_ESCL_Lo;
	IIC0_ESCL_Lo;
	Delay();

	for(i = 8; i>0; i--)
	{
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

	IIC0_EAck_read();	// ACK
	IIC0_ESDA_OUTP;

	IIC0_EEnd();

	*IicData = data;
}

void pmic_init(void)
{
	unsigned char rtc_ctrl;
	unsigned char wrstbi_ctrl;

	IIC0_ESetport();

	/* Low Jitter Setting, AP 32kHz On */
	IIC0_EWrite(S2MPU02X_WR_ADDR, RTC_BUF, 0x11);
}

void pmic_enable_peric_dev(void)
{
	unsigned char ldo_ctrl;

	IIC0_ESetport();

	/* enable LDO4 : VCC_3.0V_PERI */
	IIC0_ERead(S2MPU02X_WR_ADDR, 0x32, &ldo_ctrl);
	ldo_ctrl |= (1 << 6);
	IIC0_EWrite(S2MPU02X_WR_ADDR, 0x32, ldo_ctrl);
#if 0
	/* enable LDO18 : VCC_2.8V_PERI */
	IIC0_ERead(S2MPU02X_WR_ADDR, 0x40, &ldo_ctrl);
	ldo_ctrl |= (1 << 6);
	IIC0_EWrite(S2MPU02X_WR_ADDR, 0x40, ldo_ctrl);

	/* enable LDO23 : VCC_1.8V_PERI */
	IIC0_ERead(S2MPU02X_WR_ADDR, 0x39, &ldo_ctrl);
	ldo_ctrl |= (1 << 6);
	IIC0_EWrite(S2MPU02X_WR_ADDR, 0x39, ldo_ctrl);
#endif
}
