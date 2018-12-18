/*
 * Memory setup for XYREF4415 board based on EXYNOS4
 *
 * Copyright (C) 2013 Samsung Electronics
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

#include <config.h>
#include <asm/io.h>
#include <asm/arch/dmc.h>
#include <asm/arch/clock.h>
#include <asm/arch/power.h>
#include <asm/arch/cpu.h>
#include "setup.h"
#include "cmu.h"

typedef int BOOT_STAT;
#define nRESET 1
#define true 1
#define PKG_ID 0x10000004
#define DMC_BASE 0x106F0000
#define PHY0_BASE 0x10600000
#define PHY1_BASE 0x10610000
#define Outp32(addr, data) (*(volatile u32 *)(addr) = (data))
#define Inp32(addr) ((*(volatile u32 *)(addr)))
#define SetBits(uAddr, uBaseBit, uMaskValue, uSetValue) \
    Outp32(uAddr, (Inp32(uAddr) & ~((uMaskValue)<<(uBaseBit))) | (((uMaskValue)&(uSetValue))<<(uBaseBit)))
#define GetBits(uAddr, uBaseBit, uMaskValue) \
    ((Inp32(uAddr)>>(uBaseBit))&(uMaskValue))
#define GetPopType() (GetBits(PKG_ID, 4, 0x3)) //0 : 1G, 1: 1.5G, 2: 2G

//----------------------------------------------------------------------------
//
//- DMC Initialzation Script for LPDDR3
//
//- Copyright 2013 by Samsung Electronics. All rights reserved.
//
//----------------------------------------------------------------------------
void DMC_Delay(int msec)
{
	volatile u32 i;
	for(;msec > 0; msec--);
		for(i=0; i<1000; i++) ;
}

void InitDMC(u32 MemClk, BOOT_STAT eRstStat)
{
	u32 utemp;

	Outp32(0x10600000+0x00a0, (0x7<<6)); // bit flip W/A CKE drive strength MIN -> MAX
	Outp32(0x10610000+0x00a0, (0x7<<6)); // bit flip W/A CKE drive strength MIN -> MAX

	//Outp32(0x105c0504, 546867200.);
	Outp32(0x106F0000+0x0024, 0x00000003);
	Outp32(0x10600000+0x0000, 0x17023A00);
	Outp32(0x10610000+0x0000, 0x17023A00);
	Outp32(0x10600000+0x00AC, 0x809);
	Outp32(0x10610000+0x00AC, 0x809);
	Outp32(0x10600000+0x006C, 0x6000f);
	Outp32(0x10610000+0x006C, 0x6000f);

	Outp32(0x10600000+0x0004, 0x09210001);
	Outp32(0x10610000+0x0004, 0x09210001);
	Outp32(0x10600000+0x000C, 0x00210842);
	Outp32(0x10610000+0x000C, 0x00210842);
	Outp32(0x10600000+0x0038, 0x001F000F);
	Outp32(0x10610000+0x0038, 0x001F000F);
	Outp32(0x10600000+0x0040, 0x0F0C0308);
	Outp32(0x10610000+0x0040, 0x0F0C0308);
	Outp32(0x10600000+0x0040, 0x0F0C030a);
	Outp32(0x10610000+0x0040, 0x0F0C030a);
	Outp32(0x10600000+0x0048, 0x1E9);
	Outp32(0x10610000+0x0048, 0x1E9);
	Outp32(0x10600000+0x0040, 0x0F0C0308);
	Outp32(0x10610000+0x0040, 0x0F0C0308);
	Outp32(0x106F0000+0x0018, 0x00003000);
	Outp32(0x106F0000+0x0000, 0x1FFF1008);
		DMC_Delay(0x10000); //- GOSUB Waiting 100ms
	Outp32(0x106F0000+0x0000, 0x0FFF1000);
		DMC_Delay(0x10000); //- GOSUB Waiting 100ms
	Outp32(0x10600000+0x0010, 0x7f7f7f7f);
	Outp32(0x10610000+0x0010, 0x7f7f7f7f);
	Outp32(0x10600000+0x0018, 0x7f7f7f7f);
	Outp32(0x10610000+0x0018, 0x7f7f7f7f);
	Outp32(0x10600000+0x0028, 0x0000007f);
	Outp32(0x10610000+0x0028, 0x0000007f);
		DMC_Delay(0x10000); //- GOSUB Waiting 100ms
	Outp32(0x106F0000+0x0018, 0x00003008);
	Outp32(0x106F0000+0x0018, 0x00003000);
	Outp32(0x106F0000+0x0004, 0x00302740);
	Outp32(0x106F0000+0x0008, 0x00001323);
	Outp32(0x106F0000+0x000C, 0x00001323);
	Outp32(0x106F0000+0x010C, 0x004007c0);
	Outp32(0x106F0000+0x0014, 0xFF000000);
	Outp32(0x106F0000+0x0028, 0xFFFF00FF);

	// Refresh Timinig
	Outp32(0x106F0000+0x0030, 0x2E);

	Outp32(0x106F0000+0x0034, 0x244764CD);
	Outp32(0x106F0000+0x0038, 0x35302509);
	Outp32(0x106F0000+0x003C, 0x38270335);
	Outp32(0x106F0000+0x00F0, 0x00000007);

	if (eRstStat ==nRESET)
	{
		Outp32(0x106F0000+0x0010, 0x07000000);
			DMC_Delay(0x10000); //- GOSUB Waiting 100ms
		Outp32(0x106F0000+0x0010, 0x00071C00);
			DMC_Delay(0x10000); //- GOSUB Waiting 100ms
		Outp32(0x106F0000+0x0010, 0x00010BFC);
			DMC_Delay(0x10000); //- GOSUB Waiting 100ms
		Outp32(0x106F0000+0x0010, 0x0000078c);
		Outp32(0x106F0000+0x0010, 0x0000081c);
		Outp32(0x106F0000+0x0010, 0x00000C04);
			DMC_Delay(0x10000); //- GOSUB Waiting 100ms
		Outp32(0x106F0000+0x0010, 0x17000000);
			DMC_Delay(0x10000); //- GOSUB Waiting 100ms
		Outp32(0x106F0000+0x0010, 0x10071C00);
			DMC_Delay(0x10000); //- GOSUB Waiting 100ms
		Outp32(0x106F0000+0x0010, 0x10010BFC);
			DMC_Delay(0x10000); //- GOSUB Waiting 100ms
		Outp32(0x106F0000+0x0010, 0x1000078c);
		Outp32(0x106F0000+0x0010, 0x1000081c);
		Outp32(0x106F0000+0x0010, 0x10000C04);
			DMC_Delay(0x10000); //- GOSUB Waiting 100ms
	}
	else
	{
		Outp32( 0x106F0000+0x0010, 0x08000000 ); 				   //- CH0, CS0. Self Refresh Exit Command for only sleep & wakeup
		Outp32( 0x106F0000+0x0010, 0x18000000 ); 				   //- CH1, CS0. Self Refresh Exit Command for only sleep & wakeup
	}

	Outp32(0x10600000+0x0030, 0x10107f1e);   // ctrl_ref 0xf [4:1]
	Outp32(0x10610000+0x0030, 0x10107f1e);   // ctrl_ref 0xf [4:1]
	Outp32(0x10600000+0x0030, 0x10107f3e);   // ctrl_ref 0xf [4:1]
	Outp32(0x10610000+0x0030, 0x10107f3e);   // ctrl_ref 0xf [4:1]
	Outp32(0x10600000+0x0030, 0x10107f7e);   // ctrl_ref 0xf [4:1]
	Outp32(0x10610000+0x0030, 0x10107f7e);   // ctrl_ref 0xf [4:1]

	while(1)
	{
        if(Inp32(0x10600034)&0x5==0x5)
		break;
	}

	#if 0
		utemp = (Inp32(0x10600000+0x0034)>>10)&0x7f; //forcing value
		Outp32(0x10600000+0x0030, Inp32(0x10600000+0x0030)&(~(0x7f<<8))|(utemp<<8));
		Outp32(0x10600000+0x0030, Inp32(0x10600000+0x0030)&(~(0x1<<5))); //forcing value
	#endif

	Outp32(0x10610000+0x0030, 0x10107f1e);   // ctrl_ref 0xf [4:1]
	Outp32(0x10610000+0x0030, 0x10107f3e);   // ctrl_ref 0xf [4:1]
	Outp32(0x10610000+0x0030, 0x10107f7e);   // ctrl_ref 0xf [4:1]

	while(1)
	{
        if(Inp32(0x10610034)&0x5==0x5)
		break;
	}

	#if 0
		utemp = (Inp32(0x10610000+0x0034)>>10)&0x7f; //forcing value
		Outp32(0x10610000+0x0030, Inp32(0x10610000+0x0030)&(~(0x7f<<8))|(utemp<<8));
		Outp32(0x10610000+0x0030, Inp32(0x10610000+0x0030)&(~(0x1<<5))); //forcing value
	#endif

	//---------------- clock change
	//Outp32(0x105C0504, 142641152.);
	//Outp32(0x105C0800, 3.);
	//Outp32(0x105C0300, 0x1510);
	//Outp32(0x105C0218, 2171208705.);
	//	DMC_Delay(0x10000); //- GOSUB Waiting 0.3s
	//Outp32(0x105C0300, 0x1510);
	CMU_InitForMif(MemClk);


	Outp32(0x10600000+0x0010, 0x08080808);
	Outp32(0x10610000+0x0010, 0x08080808);
	Outp32(0x10600000+0x0018, 0x08080808);
	Outp32(0x10610000+0x0018, 0x08080808);
	Outp32(0x10600000+0x0028, 0x00000008);
	Outp32(0x10610000+0x0028, 0x00000008);

	while(1)
	{
        if(Inp32(0x10600000+0x0034)&0x1==0x1)
		break;
	}
	while(1)
	{
        if(Inp32(0x10610000+0x0034)&0x1==0x1)
		break;
	}

	Outp32(0x106F0000+0x0000, 0x1FFF1008);
	//	DMC_Delay(0x10000); //- GOSUB Waiting 100ms
		DMC_Delay(0x10000); //- GOSUB Waiting 0.1s
	Outp32(0x106F0000+0x0018, 0x5008);
	Outp32(0x106F0000+0x0018, 0x5000);
	Outp32(0x10600000+0x0000, 0x17023a00);
	Outp32(0x10610000+0x0000, 0x17023a00);

	#if 0
	Outp32(0x106F0000+0x0010, 0x01000000);
		DMC_Delay(0x10000); //- GOSUB Waiting 100ms
	Outp32(0x106F0000+0x0010, 0x01100000);
		DMC_Delay(0x10000); //- GOSUB Waiting 100ms
	Outp32(0x106F0000+0x0010, 0x11000000);
		DMC_Delay(0x10000); //- GOSUB Waiting 100ms
	Outp32(0x106F0000+0x0010, 0x11100000);
		DMC_Delay(0x10000); //- GOSUB Waiting 100ms
	#endif

	#if 0
	Outp32(0x106F0000+0x0004, 0x00302763);
	Outp32(0x106F0000+0x0000, 0x0FFF1028);
	#endif
	Outp32(0x106F0000+0x001C, 0x17); // MEMIF_CG_EN off
	Outp32(0x106F0000+0x0400, 0x00000001);	// ALL_INIT_INDI[0]=all_init_done=0x1

	// LPI handshaking w/a
	SetBits(0x105C0700, 0, 0x1, 0);
	// Checking Self-Refresh Status
	#if 1				//- 1GB size
		while(((Inp32(0x106F0048)>>8)&0x1)!=0x1);					//- CH0 ChipStatus, chip_sref_state[8] (CS0)
		while(((Inp32(0x106F004C)>>8)&0x1)!=0x1);					//- CH1 ChipStatus, chip_sref_state[8] (CS0)
	#else				//- 2GB size
		while(((Inp32(0x106F0048)>>8)&0x3)!=0x3);					//- CH0 ChipStatus, chip_sref_state[9:8] (CS1,CS0)
		while(((Inp32(0x106F004C)>>8)&0x3)!=0x3);					//- CH1 ChipStatus, chip_sref_state[9:8] (CS1,CS0)
	#endif

	//DMC_Delay(10000);
	SetBits(0x105C0700, 0, 0x1, 1);

	// MEMIF_CG_EN off
	Outp32(0x106F0000+0x001C, 0x1F);

	//check clockout setting
	Outp32(0x10020A00, 0x00000000);
	Outp32(0x105C0A00, 0x10004);

	#if 1
		Outp32(0x106F0000+0x0004, 0x00302762);							//- Setting Power Down Mode, dynamic self refresh, dynamic power down : Enable
		Outp32(0x106F0000+0x0018, 0x5002);									//- SDLL dynamic power gating : Enable
		Outp32(0x106F0000+0x0000, 0x0FFF1028);							//- Auto Refresh : Enable
	#endif


}

void InitDMC_1_5GB(u32 MemClk, BOOT_STAT eRstStat)
{
	u32 utemp;

	Outp32(0x10600000+0x00a0, (0x7<<6)); // bit flip W/A CKE drive strength MIN -> MAX
	Outp32(0x10610000+0x00a0, (0x7<<6)); // bit flip W/A CKE drive strength MIN -> MAX

	//Outp32(0x105c0504, 546867200.);
	Outp32(0x106F0000+0x0024, 0x00000003);
	Outp32(0x10600000+0x0000, 0x17023A00);
	Outp32(0x10610000+0x0000, 0x17023A00);
	Outp32(0x10600000+0x00AC, 0x809);
	Outp32(0x10610000+0x00AC, 0x809);
	Outp32(0x10600000+0x006C, 0x6000f);
	Outp32(0x10610000+0x006C, 0x6000f);

	Outp32(0x10600000+0x0004, 0x09210001);
	Outp32(0x10610000+0x0004, 0x09210001);
	Outp32(0x10600000+0x000C, 0x00210842);
	Outp32(0x10610000+0x000C, 0x00210842);
	Outp32(0x10600000+0x0038, 0x001F000F);
	Outp32(0x10610000+0x0038, 0x001F000F);
	Outp32(0x10600000+0x0040, 0x0F0C0308);
	Outp32(0x10610000+0x0040, 0x0F0C0308);
	Outp32(0x10600000+0x0040, 0x0F0C030a);
	Outp32(0x10610000+0x0040, 0x0F0C030a);
	Outp32(0x10600000+0x0048, 0x1E9);
	Outp32(0x10610000+0x0048, 0x1E9);
	Outp32(0x10600000+0x0040, 0x0F0C0308);
	Outp32(0x10610000+0x0040, 0x0F0C0308);
	Outp32(0x106F0000+0x0018, 0x00003000);
	Outp32(0x106F0000+0x0000, 0x1FFF1008);
		DMC_Delay(0x10000); //- GOSUB Waiting 100ms
	Outp32(0x106F0000+0x0000, 0x0FFF1000);
		DMC_Delay(0x10000); //- GOSUB Waiting 100ms
	Outp32(0x10600000+0x0010, 0x7f7f7f7f);
	Outp32(0x10610000+0x0010, 0x7f7f7f7f);
	Outp32(0x10600000+0x0018, 0x7f7f7f7f);
	Outp32(0x10610000+0x0018, 0x7f7f7f7f);
	Outp32(0x10600000+0x0028, 0x0000007f);
	Outp32(0x10610000+0x0028, 0x0000007f);
		DMC_Delay(0x10000); //- GOSUB Waiting 100ms
	Outp32(0x106F0000+0x0018, 0x00003008);
	Outp32(0x106F0000+0x0018, 0x00003000);
	Outp32(0x106F0000+0x0004, 0x00302740);
	Outp32(0x106F0000+0x0008, 0x00001333);   //1.5GB
	Outp32(0x106F0000+0x000C, 0x00001333);   //1.5GB
//	Outp32(0x106F0000+0x010C, 0x004007c0);
	Outp32(0x106F0000+0x010C, 0x004007a0);   //1.5GB
	Outp32(0x106F0000+0x0014, 0xFF000000);
	Outp32(0x106F0000+0x0028, 0xFFFF00FF);

	// Refresh Timinig
	Outp32(0x106F0000+0x0030, 0x2E);

	Outp32(0x106F0000+0x0034, 0x3A4764CD);  //1.5GB
	Outp32(0x106F0000+0x0038, 0x35302509);  //1.5GB
	Outp32(0x106F0000+0x003C, 0x383C0335);  //1.5GB


	Outp32(0x106F0000+0x00F0, 0x00000007);

	if (eRstStat ==nRESET)
	{
		Outp32(0x106F0000+0x0010, 0x07000000);
			DMC_Delay(0x10000); //- GOSUB Waiting 100ms
		Outp32(0x106F0000+0x0010, 0x00071C00);
			DMC_Delay(0x10000); //- GOSUB Waiting 100ms
		Outp32(0x106F0000+0x0010, 0x00010BFC);
			DMC_Delay(0x10000); //- GOSUB Waiting 100ms
		Outp32(0x106F0000+0x0010, 0x0000078c);
		Outp32(0x106F0000+0x0010, 0x0000081c);
		Outp32(0x106F0000+0x0010, 0x00000C04);
			DMC_Delay(0x10000); //- GOSUB Waiting 100ms
		Outp32(0x106F0000+0x0010, 0x17000000);
			DMC_Delay(0x10000); //- GOSUB Waiting 100ms
		Outp32(0x106F0000+0x0010, 0x10071C00);
			DMC_Delay(0x10000); //- GOSUB Waiting 100ms
		Outp32(0x106F0000+0x0010, 0x10010BFC);
			DMC_Delay(0x10000); //- GOSUB Waiting 100ms
		Outp32(0x106F0000+0x0010, 0x1000078c);
		Outp32(0x106F0000+0x0010, 0x1000081c);
		Outp32(0x106F0000+0x0010, 0x10000C04);
			DMC_Delay(0x10000); //- GOSUB Waiting 100ms
	}
	else
	{
		Outp32( 0x106F0000+0x0010, 0x08000000 ); 				   //- CH0, CS0. Self Refresh Exit Command for only sleep & wakeup
		Outp32( 0x106F0000+0x0010, 0x18000000 ); 				   //- CH1, CS0. Self Refresh Exit Command for only sleep & wakeup
	}

	Outp32(0x10600000+0x0030, 0x10107f1e);   // ctrl_ref 0xf [4:1]
	Outp32(0x10610000+0x0030, 0x10107f1e);   // ctrl_ref 0xf [4:1]
	Outp32(0x10600000+0x0030, 0x10107f3e);   // ctrl_ref 0xf [4:1]
	Outp32(0x10610000+0x0030, 0x10107f3e);   // ctrl_ref 0xf [4:1]
	Outp32(0x10600000+0x0030, 0x10107f7e);   // ctrl_ref 0xf [4:1]
	Outp32(0x10610000+0x0030, 0x10107f7e);   // ctrl_ref 0xf [4:1]

	while(1)
	{
        if(Inp32(0x10600034)&0x5==0x5)
		break;
	}

	#if 0
		utemp = (Inp32(0x10600000+0x0034)>>10)&0x7f; //forcing value
		Outp32(0x10600000+0x0030, Inp32(0x10600000+0x0030)&(~(0x7f<<8))|(utemp<<8));
		Outp32(0x10600000+0x0030, Inp32(0x10600000+0x0030)&(~(0x1<<5))); //forcing value
	#endif

	Outp32(0x10610000+0x0030, 0x10107f1e);   // ctrl_ref 0xf [4:1]
	Outp32(0x10610000+0x0030, 0x10107f3e);   // ctrl_ref 0xf [4:1]
	Outp32(0x10610000+0x0030, 0x10107f7e);   // ctrl_ref 0xf [4:1]

	while(1)
	{
        if(Inp32(0x10610034)&0x5==0x5)
		break;
	}

	#if 0
		utemp = (Inp32(0x10610000+0x0034)>>10)&0x7f; //forcing value
		Outp32(0x10610000+0x0030, Inp32(0x10610000+0x0030)&(~(0x7f<<8))|(utemp<<8));
		Outp32(0x10610000+0x0030, Inp32(0x10610000+0x0030)&(~(0x1<<5))); //forcing value
	#endif

	//---------------- clock change
	//Outp32(0x105C0504, 142641152.);
	//Outp32(0x105C0800, 3.);
	//Outp32(0x105C0300, 0x1510);
	//Outp32(0x105C0218, 2171208705.);
	//	DMC_Delay(0x10000); //- GOSUB Waiting 0.3s
	//Outp32(0x105C0300, 0x1510);
	CMU_InitForMif(MemClk);


	Outp32(0x10600000+0x0010, 0x08080808);
	Outp32(0x10610000+0x0010, 0x08080808);
	Outp32(0x10600000+0x0018, 0x08080808);
	Outp32(0x10610000+0x0018, 0x08080808);
	Outp32(0x10600000+0x0028, 0x00000008);
	Outp32(0x10610000+0x0028, 0x00000008);

	while(1)
	{
        if(Inp32(0x10600000+0x0034)&0x1==0x1)
		break;
	}
	while(1)
	{
        if(Inp32(0x10610000+0x0034)&0x1==0x1)
		break;
	}

	Outp32(0x106F0000+0x0000, 0x1FFF1008);
	//	DMC_Delay(0x10000); //- GOSUB Waiting 100ms
		DMC_Delay(0x10000); //- GOSUB Waiting 0.1s
	Outp32(0x106F0000+0x0018, 0x5008);
	Outp32(0x106F0000+0x0018, 0x5000);
	Outp32(0x10600000+0x0000, 0x17023a00);
	Outp32(0x10610000+0x0000, 0x17023a00);

	#if 0
	Outp32(0x106F0000+0x0010, 0x01000000);
		DMC_Delay(0x10000); //- GOSUB Waiting 100ms
	Outp32(0x106F0000+0x0010, 0x01100000);
		DMC_Delay(0x10000); //- GOSUB Waiting 100ms
	Outp32(0x106F0000+0x0010, 0x11000000);
		DMC_Delay(0x10000); //- GOSUB Waiting 100ms
	Outp32(0x106F0000+0x0010, 0x11100000);
		DMC_Delay(0x10000); //- GOSUB Waiting 100ms
	#endif

	#if 0
	Outp32(0x106F0000+0x0004, 0x00302763);
	Outp32(0x106F0000+0x0000, 0x0FFF1028);
	#endif

	Outp32(0x106F0000+0x001C, 0x17); // MEMIF_CG_EN off
	Outp32(0x106F0000+0x0400, 0x00000001);	// ALL_INIT_INDI[0]=all_init_done=0x1


	// LPI handshaking w/a
	SetBits(0x105C0700, 0, 0x1, 0);
	// Checking Self-Refresh Status
	#if 1				//- 1GB size
		while(((Inp32(0x106F0048)>>8)&0x1)!=0x1);					//- CH0 ChipStatus, chip_sref_state[8] (CS0)
		while(((Inp32(0x106F004C)>>8)&0x1)!=0x1);					//- CH1 ChipStatus, chip_sref_state[8] (CS0)
	#else				//- 2GB size
		while(((Inp32(0x106F0048)>>8)&0x3)!=0x3);					//- CH0 ChipStatus, chip_sref_state[9:8] (CS1,CS0)
		while(((Inp32(0x106F004C)>>8)&0x3)!=0x3);					//- CH1 ChipStatus, chip_sref_state[9:8] (CS1,CS0)
	#endif

	//DMC_Delay(10000);
	SetBits(0x105C0700, 0, 0x1, 1);

	// MEMIF_CG_EN off
	Outp32(0x106F0000+0x001C, 0x1F);

	//check clockout setting
	Outp32(0x10020A00, 0x00000000);
	Outp32(0x105C0A00, 0x10004);


	#if 1
		Outp32(0x106F0000+0x0004, 0x00302762);							//- Setting Power Down Mode, dynamic self refresh, dynamic power down : Enable
		Outp32(0x106F0000+0x0018, 0x5002);									//- SDLL dynamic power gating : Enable
		Outp32(0x106F0000+0x0000, 0x0FFF1028);							//- Auto Refresh : Enable
	#endif


}

void InitDMC_2GB(u32 MemClk, BOOT_STAT eRstStat)
{
	u32 utemp;


	Outp32(0x10600000+0x00a0, (0x7<<6)); // bit flip W/A CKE drive strength MIN -> MAX
	Outp32(0x10610000+0x00a0, (0x7<<6)); // bit flip W/A CKE drive strength MIN -> MAX

	//Outp32(0x105c0504, 546867200.);
	Outp32(0x106F0000+0x0024, 0x00000003);
	Outp32(0x10600000+0x0000, 0x17023A00);
	Outp32(0x10610000+0x0000, 0x17023A00);
	Outp32(0x10600000+0x00AC, 0x809);
	Outp32(0x10610000+0x00AC, 0x809);
	Outp32(0x10600000+0x006C, 0x6000f);
	Outp32(0x10610000+0x006C, 0x6000f);

	Outp32(0x10600000+0x0004, 0x09210001);
	Outp32(0x10610000+0x0004, 0x09210001);
	Outp32(0x10600000+0x000C, 0x00210842);
	Outp32(0x10610000+0x000C, 0x00210842);
	Outp32(0x10600000+0x0038, 0x001F000F);
	Outp32(0x10610000+0x0038, 0x001F000F);
	Outp32(0x10600000+0x0040, 0x0F0C0308);
	Outp32(0x10610000+0x0040, 0x0F0C0308);
	Outp32(0x10600000+0x0040, 0x0F0C030a);
	Outp32(0x10610000+0x0040, 0x0F0C030a);
	Outp32(0x10600000+0x0048, 0x1E9);
	Outp32(0x10610000+0x0048, 0x1E9);
	Outp32(0x10600000+0x0040, 0x0F0C0308);
	Outp32(0x10610000+0x0040, 0x0F0C0308);
	Outp32(0x106F0000+0x0018, 0x00003000);
	Outp32(0x106F0000+0x0000, 0x1FFF1008);
		DMC_Delay(0x10000); //- GOSUB Waiting 100ms
	Outp32(0x106F0000+0x0000, 0x0FFF1000);
		DMC_Delay(0x10000); //- GOSUB Waiting 100ms
	Outp32(0x10600000+0x0010, 0x7f7f7f7f);
	Outp32(0x10610000+0x0010, 0x7f7f7f7f);
	Outp32(0x10600000+0x0018, 0x7f7f7f7f);
	Outp32(0x10610000+0x0018, 0x7f7f7f7f);
	Outp32(0x10600000+0x0028, 0x0000007f);
	Outp32(0x10610000+0x0028, 0x0000007f);
		DMC_Delay(0x10000); //- GOSUB Waiting 100ms
	Outp32(0x106F0000+0x0018, 0x00003008);
	Outp32(0x106F0000+0x0018, 0x00003000);
	Outp32(0x106F0000+0x0004, 0x00312740); //2GB
	Outp32(0x106F0000+0x0008, 0x00001323);
	Outp32(0x106F0000+0x000C, 0x00001323);
//	Outp32(0x106F0000+0x010C, 0x004007c0);
	Outp32(0x106F0000+0x010C, 0x004007c0); //2GB
	Outp32(0x106F0000+0x0110, 0x008007c0); //2GB
	Outp32(0x106F0000+0x0014, 0xFF000000);
	Outp32(0x106F0000+0x0028, 0xFFFF00FF);

	// Refresh Timinig
	Outp32(0x106F0000+0x0030, 0x2E);

	Outp32(0x106F0000+0x0034, 0x244764CD);
	Outp32(0x106F0000+0x0038, 0x35302509);
	Outp32(0x106F0000+0x003C, 0x38270335);

	Outp32(0x106F0000+0x00F0, 0x00000007);

	if (eRstStat ==nRESET) //2GB
	{
		Outp32(0x106F0000+0x0010, 0x07000000);
		Outp32(0x106F0000+0x0010, 0x07100000);
			DMC_Delay(0x10000); //- GOSUB Waiting 100ms
		Outp32(0x106F0000+0x0010, 0x00071C00);
		Outp32(0x106F0000+0x0010, 0x00171C00);
			DMC_Delay(0x10000); //- GOSUB Waiting 100ms
		Outp32(0x106F0000+0x0010, 0x00010BFC);
		Outp32(0x106F0000+0x0010, 0x00110BFC);
			DMC_Delay(0x10000); //- GOSUB Waiting 100ms
		Outp32(0x106F0000+0x0010, 0x0000078C);
		Outp32(0x106F0000+0x0010, 0x0000081C);
		Outp32(0x106F0000+0x0010, 0x00000C04);
		Outp32(0x106F0000+0x0010, 0x0010078C);
		Outp32(0x106F0000+0x0010, 0x0010081C);
		Outp32(0x106F0000+0x0010, 0x00100C04);
			DMC_Delay(0x10000); //- GOSUB Waiting 100ms
		Outp32(0x106F0000+0x0010, 0x17000000);
		Outp32(0x106F0000+0x0010, 0x17100000);
			DMC_Delay(0x10000); //- GOSUB Waiting 100ms
		Outp32(0x106F0000+0x0010, 0x10071C00);
		Outp32(0x106F0000+0x0010, 0x10171C00);
			DMC_Delay(0x10000); //- GOSUB Waiting 100ms
		Outp32(0x106F0000+0x0010, 0x10010BFC);
		Outp32(0x106F0000+0x0010, 0x10110BFC);
			DMC_Delay(0x10000); //- GOSUB Waiting 100ms
		Outp32(0x106F0000+0x0010, 0x1000078C);
		Outp32(0x106F0000+0x0010, 0x1000081C);
		Outp32(0x106F0000+0x0010, 0x10000C04);
		Outp32(0x106F0000+0x0010, 0x1010078C);
		Outp32(0x106F0000+0x0010, 0x1010081C);
		Outp32(0x106F0000+0x0010, 0x10100C04);
			DMC_Delay(0x10000); //- GOSUB Waiting 100ms
	}
	else
	{
		Outp32( 0x106F0000+0x0010, 0x08000000 ); 				   //- CH0, CS0. Self Refresh Exit Command for only sleep & wakeup
		Outp32( 0x106F0000+0x0010, 0x08100000 ); 				   //- CH0, CS1. Self Refresh Exit Command for only sleep & wakeup
		Outp32( 0x106F0000+0x0010, 0x18000000 ); 				   //- CH1, CS0. Self Refresh Exit Command for only sleep & wakeup
		Outp32( 0x106F0000+0x0010, 0x18100000 ); 				   //- CH1, CS1. Self Refresh Exit Command for only sleep & wakeup
	}

	Outp32(0x10600000+0x0030, 0x10107f1e);   // ctrl_ref 0xf [4:1]
	Outp32(0x10610000+0x0030, 0x10107f1e);   // ctrl_ref 0xf [4:1]
	Outp32(0x10600000+0x0030, 0x10107f3e);   // ctrl_ref 0xf [4:1]
	Outp32(0x10610000+0x0030, 0x10107f3e);   // ctrl_ref 0xf [4:1]
	Outp32(0x10600000+0x0030, 0x10107f7e);   // ctrl_ref 0xf [4:1]
	Outp32(0x10610000+0x0030, 0x10107f7e);   // ctrl_ref 0xf [4:1]

	while(1)
	{
        if(Inp32(0x10600034)&0x5==0x5)
		break;
	}

	#if 0
		utemp = (Inp32(0x10600000+0x0034)>>10)&0x7f; //forcing value
		Outp32(0x10600000+0x0030, Inp32(0x10600000+0x0030)&(~(0x7f<<8))|(utemp<<8));
		Outp32(0x10600000+0x0030, Inp32(0x10600000+0x0030)&(~(0x1<<5))); //forcing value
	#endif

	Outp32(0x10610000+0x0030, 0x10107f1e);   // ctrl_ref 0xf [4:1]
	Outp32(0x10610000+0x0030, 0x10107f3e);   // ctrl_ref 0xf [4:1]
	Outp32(0x10610000+0x0030, 0x10107f7e);   // ctrl_ref 0xf [4:1]

	while(1)
	{
        if(Inp32(0x10610034)&0x5==0x5)
		break;
	}

	#if 0
		utemp = (Inp32(0x10610000+0x0034)>>10)&0x7f; //forcing value
		Outp32(0x10610000+0x0030, Inp32(0x10610000+0x0030)&(~(0x7f<<8))|(utemp<<8));
		Outp32(0x10610000+0x0030, Inp32(0x10610000+0x0030)&(~(0x1<<5))); //forcing value
	#endif

	//---------------- clock change
	//Outp32(0x105C0504, 142641152.);
	//Outp32(0x105C0800, 3.);
	//Outp32(0x105C0300, 0x1510);
	//Outp32(0x105C0218, 2171208705.);
	//	DMC_Delay(0x10000); //- GOSUB Waiting 0.3s
	//Outp32(0x105C0300, 0x1510);
	CMU_InitForMif(MemClk);


	Outp32(0x10600000+0x0010, 0x08080808);
	Outp32(0x10610000+0x0010, 0x08080808);
	Outp32(0x10600000+0x0018, 0x08080808);
	Outp32(0x10610000+0x0018, 0x08080808);
	Outp32(0x10600000+0x0028, 0x00000008);
	Outp32(0x10610000+0x0028, 0x00000008);

	while(1)
	{
        if(Inp32(0x10600000+0x0034)&0x1==0x1)
		break;
	}
	while(1)
	{
        if(Inp32(0x10610000+0x0034)&0x1==0x1)
		break;
	}

	Outp32(0x106F0000+0x0000, 0x1FFF1008);
	//	DMC_Delay(0x10000); //- GOSUB Waiting 100ms
		DMC_Delay(0x10000); //- GOSUB Waiting 0.1s
	Outp32(0x106F0000+0x0018, 0x5008);
	Outp32(0x106F0000+0x0018, 0x5000);
	Outp32(0x10600000+0x0000, 0x17023a00);
	Outp32(0x10610000+0x0000, 0x17023a00);

	#if 0
	Outp32(0x106F0000+0x0010, 0x01000000);
		DMC_Delay(0x10000); //- GOSUB Waiting 100ms
	Outp32(0x106F0000+0x0010, 0x01100000);
		DMC_Delay(0x10000); //- GOSUB Waiting 100ms
	Outp32(0x106F0000+0x0010, 0x11000000);
		DMC_Delay(0x10000); //- GOSUB Waiting 100ms
	Outp32(0x106F0000+0x0010, 0x11100000);
		DMC_Delay(0x10000); //- GOSUB Waiting 100ms
	#endif

	#if 0
	Outp32(0x106F0000+0x0004, 0x00302763);
	Outp32(0x106F0000+0x0000, 0x0FFF1028);
	#endif

	Outp32(0x106F0000+0x001C, 0x17); // MEMIF_CG_EN off
	Outp32(0x106F0000+0x0400, 0x00000001);	// ALL_INIT_INDI[0]=all_init_done=0x1


	// LPI handshaking w/a
	SetBits(0x105C0700, 0, 0x1, 0);
	// Checking Self-Refresh Status
	#if 0				//- 1GB size
		while(((Inp32(0x106F0048)>>8)&0x1)!=0x1);					//- CH0 ChipStatus, chip_sref_state[8] (CS0)
		while(((Inp32(0x106F004C)>>8)&0x1)!=0x1);					//- CH1 ChipStatus, chip_sref_state[8] (CS0)
	#else				//- 2GB size
		while(((Inp32(0x106F0048)>>8)&0x3)!=0x3);					//- CH0 ChipStatus, chip_sref_state[9:8] (CS1,CS0)
		while(((Inp32(0x106F004C)>>8)&0x3)!=0x3);					//- CH1 ChipStatus, chip_sref_state[9:8] (CS1,CS0)
	#endif

	//DMC_Delay(10000);
	SetBits(0x105C0700, 0, 0x1, 1);

	// MEMIF_CG_EN off
	Outp32(0x106F0000+0x001C, 0x1F);

	//check clockout setting
	Outp32(0x10020A00, 0x00000000);
	Outp32(0x105C0A00, 0x10004);


	#if 1
		Outp32(0x106F0000+0x0004, 0x00312762);							//2GB - Setting Power Down Mode, dynamic self refresh, dynamic power down : Enable
		Outp32(0x106F0000+0x0018, 0x5002);									//- SDLL dynamic power gating : Enable
		Outp32(0x106F0000+0x0000, 0x0FFF1028);							//- Auto Refresh : Enable
	#endif
}

void BTS_DMC_init(void)
{
	Outp32(0x106F0000+0x00B8, 0x00000080);
	Outp32(0x106F0000+0x00B0, 0x00000080);
	Outp32(0x106F0000+0x00A8, 0x00000080);
	Outp32(0x106F0000+0x00A0, 0x00000080);
}

void mem_ctrl_init (void)
{
	if(0 == GetPopType()) { // POP => 1G
			InitDMC(543, 1);
	} else if (1 == GetPopType()) { // POP => 1.5G
			InitDMC_1_5GB(543, 1);
	} else if (2 == GetPopType()) { // POP => 2G
		InitDMC_2GB(543, 1);
	} else
		; //reserved

	BTS_DMC_init();
}

