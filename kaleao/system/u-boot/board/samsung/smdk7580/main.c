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
typedef unsigned int        u32;
typedef unsigned short      u16;
typedef unsigned char       u8;
typedef unsigned char       bool;
typedef signed   int        S32;

#define Outp32(addr, data) (*(volatile u32 *)(addr) = (data))
#define Inp32(addr) ((*(volatile u32 *)(addr)))

#define WaitUntilStatus(reg, offset, mask, stat) 	while( ((Inp32(reg)>>(offset))&(mask)) == (stat) );
#define SetBits(uAddr, uBaseBit, uMaskValue, uSetValue) \
{ \
	u32 x; \
	x = Inp32(uAddr); \
	x &= ~((unsigned)(uMaskValue)<<(uBaseBit)); \
	x |= ((unsigned)(uSetValue)<<(uBaseBit)); \
	Outp32(uAddr, x); \
}

#define true					(1)
#define false					(0)
#define rRST_STAT				(0x10460404)
#define rWAKEUP_STAT			(0x10460600)
#define rPAUSE					(0x10431008)

#define SEMAPHORE_BASE		0x104B0000
#define AP_FLAG				SEMAPHORE_BASE + 0x00000180
#define CP_FLAG				SEMAPHORE_BASE + 0x00000184
#define INIT_TURN			SEMAPHORE_BASE + 0x00000188
#define MIF_INIT_DONE		SEMAPHORE_BASE + 0x0000004C
#define MIF_MUX_DONE		SEMAPHORE_BASE + 0x00000190

#define GetBits(uAddr, uBaseBit, uMaskValue) \
		((Inp32(uAddr)>>(uBaseBit))&(uMaskValue))

extern void InitCmuForBl(bool bIsReset);
extern void DMC_Init(u32 mem_clk, u32 resume);

#define CP_WDTRESET				(1<<30)
#define SWRESET					(1<<29)
#define WRESET					(1<<28)
#define A5IS_WDTRESET			(1<<27)
#define	SSS_WDTRESET			(1<<26)
#define APL_WDTRESET			(1<<24)
#define CPU_WDTRESET			(1<<23)
#define PINRESET				(1<<16)

#define WAKEUP					(1<<30)
#define DMC_INIT_POR			(0x10)
#define DMC_INIT_SELF_REFRESH	(0x00)

void InitCmuAndDmc()
{
	u32 SW_or_Warm_Reset = 0;
	u32 Wdt_or_Por_Reset = 0;
	u32 WakeUp = 0;

	//- RESETSTATE(0x105C_0404) : [30]=CP_WDRST,[29]=SWRST,[28]=WRESET
	SW_or_Warm_Reset=(Inp32(rRST_STAT)>>28)&0x3;
	Wdt_or_Por_Reset=(Inp32(rRST_STAT)& (PINRESET|A5IS_WDTRESET|SSS_WDTRESET|APL_WDTRESET|CPU_WDTRESET));
	WakeUp = GetBits(rWAKEUP_STAT, 31, 0x1);
	//Power On Reset and Watch Dog Timer Reset Case
	if(Wdt_or_Por_Reset && !SW_or_Warm_Reset && !WakeUp)
	{
		//Init CMU
		InitCmuForBl(true);
		//Init DMC
		DMC_Init(825, DMC_INIT_POR);
		Outp32(rPAUSE,0x1);
		Outp32(MIF_INIT_DONE,0x4);
	}
	else
	{
		if(WakeUp)
		{
		//Wakeup Case : Check CP Status in cmu code
			InitCmuForBl(false);
		}
		else
		{
		//Warm reset, SW reset case
			//Init CMU
			InitCmuForBl(true);
			//Init DMC
			DMC_Init(825, DMC_INIT_SELF_REFRESH);
			Outp32(rPAUSE,0x1);
			Outp32(MIF_INIT_DONE,0x4);
		}
	}
}
