/*
 * Clock setup for XYREF5430 board based on EXYNOS5430
 *
 * Copyright (C) 2012 Samsung Electronics
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
#include <version.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/power.h>
#include <asm/arch/cpu.h>
#include <asm/arch/gpio.h>
#include <asm/arch/pmu.h>
#include "setup.h"

#define Outp32(addr, data) (*(volatile u32 *)(addr) = (data))
#define Inp32(addr) ((*(volatile u32 *)(addr)))

//#define WaitForMuxChange(reg, offset)	while( ((Inp32(reg)>>(offset))&7) >=4 );

#if 0
#define blOutp32(addr, data) {cprintf("Outp32(0x%08x, 0x%08x);\n", (addr), (data)); Outp32(addr, data);} 
#define WaitUntilStatus(reg, offset, mask, stat) 	\
	{ DEBUGMSG(1, ("WaitUntilStatus(0x%08x, %d, %d, %d);\n", reg, offset, mask, stat)); while( ((Inp32(reg)>>(offset))&(mask)) == (stat) ); }
#else
#define blOutp32(addr, data) Outp32(addr, data)
#define WaitUntilStatus(reg, offset, mask, stat) 	while( ((Inp32(reg)>>(offset))&(mask)) == (stat) );
#endif
#define blSetBits(uAddr, uBaseBit, uMaskValue, uSetValue) \
    blOutp32(uAddr, (Inp32(uAddr) & ~((uMaskValue)<<(uBaseBit))) | (((uMaskValue)&(uSetValue))<<(uBaseBit)))

#ifdef EVT0
#include "cmu_evt0.c"
#else
// MUX 변경으로 수정
void InitCmuForWakeup(u32 uEglClkMhz, u32 uKfcClkMhz, u32 uMphyClk) // 1066 933 800
{
	blOutp32(0x1003060C, 0x777777);


	//MIF
	blOutp32(0x105b0200, 0x00000000);
	WaitUntilStatus(0x105b0400, 12, 7, 4);
	WaitUntilStatus(0x105b0400, 4, 7, 4);
	WaitUntilStatus(0x105b0400, 0, 7, 4);
	WaitUntilStatus(0x105b0400, 8, 7, 4);

	Outp32(0x105b0004, 0x00000258);
	Outp32(0x105b0000, 0x00000258);
	Outp32(0x105b000c, 0x00000259);
	Outp32(0x105b0008, 0x00000384);

#if 0
	Outp32(0x105b0100, 0x82150601);  // 1066
	Outp32(0x105b0110, 0x81370401); // 933
	Outp32(0x105b0120, 0x81900601); // 800
	Outp32(0x105b0130, 0x80de0401); // 667
#else
	Outp32(0x105b0100, 0x816a0402);  // 543
	Outp32(0x105b0110, 0x81130401); // 825
	Outp32(0x105b0120, 0x81900601); // 800
	Outp32(0x105b0130, 0x80d30401); // 633
#endif
	WaitUntilStatus(0x105b0100, 29, 1, 0);
	WaitUntilStatus(0x105b0110, 29, 1, 0);
	WaitUntilStatus(0x105b0120, 29, 1, 0);
	WaitUntilStatus(0x105b0130, 29, 1, 0);

//	blOutp32(0x105b0600, 0x00002100);
//	blOutp32(0x105b0600, 0x00000000);
	blOutp32(0x105b0608, 0x00077411);
	blOutp32(0x105b0604, 0x00031100);
	blOutp32(0x105b060c, 0x00011213); // 0x10013

	if (uMphyClk == 1066)
		blOutp32(0x105b0204, 0x00000000);
	else if (uMphyClk == 933 || uMphyClk == 921)
		blOutp32(0x105b0204, 0x01001000);
	else if (uMphyClk == 667 || uMphyClk == 633)
		blOutp32(0x105b0204, 0x01111110);
	else
		blOutp32(0x105b0204, 0x01001000);

	blOutp32(0x105b0208, 0x00000101);
	blOutp32(0x105b0614, 0);

	WaitUntilStatus(0x105b0404, 4, 7, 4);
	WaitUntilStatus(0x105b0404, 8, 7, 4);
	WaitUntilStatus(0x105b0404, 0, 7, 4);
	WaitUntilStatus(0x105b0404, 12, 7, 4);
	WaitUntilStatus(0x105b0404, 16, 7, 4);
	WaitUntilStatus(0x105b0404, 20, 7, 4);
	WaitUntilStatus(0x105b0404, 24, 7, 4);
	WaitUntilStatus(0x105b0408, 4, 7, 4);
	WaitUntilStatus(0x105b0408, 0, 7, 4);
	WaitUntilStatus(0x105b040c, 12, 7, 4);
	WaitUntilStatus(0x105b040c, 16, 7, 4);
	WaitUntilStatus(0x105b040c, 0, 7, 4);
	WaitUntilStatus(0x105b040c, 4, 7, 4);
	WaitUntilStatus(0x105b040c, 8, 7, 4);

	blOutp32(0x105b0610, 0x00011111);

	blOutp32(0x105b0214, 0x00000001);
	blOutp32(0x105b0210, 0x00000001);
	blOutp32(0x105b0200, 0x00001111);
	WaitUntilStatus(0x105b0414, 0, 7, 4);
	WaitUntilStatus(0x105b0414, 4, 7, 4);
	WaitUntilStatus(0x105b0414, 8, 7, 4);
	WaitUntilStatus(0x105b0410, 0, 7, 4);
	WaitUntilStatus(0x105b0410, 4, 7, 4);
	WaitUntilStatus(0x105b0410, 8, 7, 4);
	WaitUntilStatus(0x105b0410, 16, 7, 4);
	WaitUntilStatus(0x105b0410, 20, 7, 4);
	WaitUntilStatus(0x105b0410, 24, 7, 4);
	WaitUntilStatus(0x105b0410, 28, 7, 4);
	WaitUntilStatus(0x105b0400, 12, 7, 4);
	WaitUntilStatus(0x105b0400, 4, 7, 4);
	WaitUntilStatus(0x105b0400, 0, 7, 4);
	WaitUntilStatus(0x105b0400, 8, 7, 4);

	//BUS1
	blOutp32(0x14800600, 0x00000002);

	//BUS2
	blOutp32(0x13400600, 0x00000002);
	blOutp32(0x13400200, 0x00000001);
	WaitUntilStatus(0x13400200, 0, 7, 4);

	//TOP
	blOutp32(0x1003023c, 0x00000011);
	blOutp32(0x10030204, 0x00000000);
	blOutp32(0x10030200, 0x00000000);
	WaitUntilStatus(0x10030404, 12, 7, 4);
	WaitUntilStatus(0x10030404, 16, 7, 4);
	WaitUntilStatus(0x10030404, 0, 7, 4);
	WaitUntilStatus(0x10030404, 4, 7, 4);
	WaitUntilStatus(0x10030404, 8, 7, 4);
	WaitUntilStatus(0x10030400, 4, 7, 4);
	WaitUntilStatus(0x10030400, 8, 7, 4);
	WaitUntilStatus(0x10030400, 0, 7, 4);

	blOutp32(0x10030610, 0x1); // BUS1

	blOutp32(0x1003060c, 0x00441515);
	blOutp32(0x10030644, 0x00001777);
	blOutp32(0x10030608, 0x00000005);
	blOutp32(0x1003062c, 0x00017017);
	blOutp32(0x10030630, 0x00000017);
	blOutp32(0x10030634, 0x0000001b);
	blOutp32(0x10030604, 0x01000700);
	blOutp32(0x10030608, 0x00000015);
	blOutp32(0x10030604, 0x51000700);
	blOutp32(0x10030618, 0x00000001);


	blOutp32(0x1003020c, 0x00000000);
	blOutp32(0x10030220, 0x00000001);
	blOutp32(0x10030230, 0x01010001);
	blOutp32(0x10030234, 0x00000001);

	WaitUntilStatus(0x1003040c, 12, 7, 4);
	WaitUntilStatus(0x1003040c, 8, 7, 4);
	WaitUntilStatus(0x1003040c, 16, 7, 4);

	blOutp32(0x10030604, 0x51000701);
	blOutp32(0x10030604, 0x51000201);

	blOutp32(0x1003020c, 0x00000000);
	WaitUntilStatus(0x1003040c, 0, 7, 4);
	WaitUntilStatus(0x1003040c, 4, 7, 4);

	blOutp32(0x10030604, 0x51111201);
	blOutp32(0x10030620, 0x00171717);
	blOutp32(0x10030600, 0x07107011);

	blOutp32(0x10030600, 0x11107011);
	blOutp32(0x10030208, 0x00000110);
	WaitUntilStatus(0x10030408, 0, 7, 4);
	WaitUntilStatus(0x10030408, 4, 7, 4);
	WaitUntilStatus(0x10030408, 8, 7, 4);
	WaitUntilStatus(0x10030408, 12, 7, 4);

	blOutp32(0x10030600, 0x11111011);

	blOutp32(0x10030000, 0x000003e9);
	blOutp32(0x10030100, 0x81cc0502);
	WaitUntilStatus(0x10030100, 29, 1, 0);

	blOutp32(0x10030204, 0x00001111);
	blOutp32(0x10030200, 0x00000001);
	WaitUntilStatus(0x10030404, 0, 7, 4);
	WaitUntilStatus(0x10030404, 4, 7, 4);
	WaitUntilStatus(0x10030404, 8, 7, 4);
	WaitUntilStatus(0x10030400, 0, 7, 4);

	//EGL
#if 1
	blOutp32(0x11800600, 0x00000003);
	blOutp32(0x11800200, 0x00000000);
	blOutp32(0x11800208, 0x00000000);
	blOutp32(0x11800208, 0x00000000);
	blOutp32(0x11800204, 0x00000001);
	WaitUntilStatus(0x11800400, 0, 7, 4);
	WaitUntilStatus(0x11800408, 0, 7, 4);
	WaitUntilStatus(0x11800404, 0, 7, 4);

	blOutp32(0x11800000, 0x000004b1);
	if (uEglClkMhz <= 500)
		blOutp32(0x11800100, 0x80000602|uEglClkMhz<<16);
	else if (uKfcClkMhz <= 1000)
		blOutp32(0x11800100, 0x80000601|(uEglClkMhz/2)<<16);
	else
		blOutp32(0x11800100, 0x80000600|(uEglClkMhz/4)<<16);
	WaitUntilStatus(0x11800100, 29, 1, 0);

	blOutp32(0x11800600, 0x00777200);
	blOutp32(0x11800604, 0x00000071);

	blOutp32(0x11800200, 0x00000001);
	WaitUntilStatus(0x11800400, 0, 7, 4);
#endif
	//KFC
	blOutp32(0x11900600, 0x03333103);

	blOutp32(0x11900200, 0x00000000);
	blOutp32(0x11900208, 0x00000000);
	blOutp32(0x11900204, 0x00000001);
	WaitUntilStatus(0x11900400, 0, 7, 4);
	WaitUntilStatus(0x11900408, 0, 7, 4);
	WaitUntilStatus(0x11900404, 0, 7, 4);

	blOutp32(0x11900000, 0x000004b1);
	if (uKfcClkMhz <= 500)
		blOutp32(0x11900100, 0x80000602|uKfcClkMhz<<16);
	else if (uKfcClkMhz <= 1000)
		blOutp32(0x11900100, 0x80000601|(uKfcClkMhz/2)<<16);
	else
		blOutp32(0x11900100, 0x80000600|(uKfcClkMhz/4)<<16);

	WaitUntilStatus(0x11900100, 29, 1, 0);

	blOutp32(0x11900600, 0x03777100);
	blOutp32(0x11900604, 0x00000071);

	blOutp32(0x11900200, 0x00000001);
	WaitUntilStatus(0x11900400, 0, 7, 4);

	//CPIF
	blOutp32(0x10fc0200, 0x00000000);
	blOutp32(0x10fc0208, 0x00000000);
	WaitUntilStatus(0x10fc0400, 0, 7, 4);
	WaitUntilStatus(0x10fc0408, 0, 7, 4);
	blOutp32(0x10fc0600, 0x00000016);

	blOutp32(0x10fc0000, 0x00000384);
	blOutp32(0x10fc0100, 0x81F40602);
	WaitUntilStatus(0x10fc0100, 29, 1, 0);
	blOutp32(0x10fc0200, 0x00000001);
	WaitUntilStatus(0x10fc0400, 0, 7, 4);
}

void InitMemClk(u32 uMemclk)
{
	blOutp32(0x105b0614, 0);
	if (uMemclk == 800 || uMemclk == 825) {
		blSetBits(0x105b1000, 12, 0xFFFFF, 0x00100);
//		blOutp32(0x105b1000, 0x00110110);
		blOutp32(0x105b0208, 0x00000100);
		blOutp32(0x105b0608, 0x00089511);
		blOutp32(0x105b060c, 0x00011214);
		blOutp32(0x105b0614, 1);
	}
	else // 50~100Mhz for DREX Initital
	{
		blSetBits(0x105b1000, 12, 0xFFFFF, 0xf0100);
//		blOutp32(0x105b1000, 0x70110110);
		blOutp32(0x105b0208, 0x00000100);
		blOutp32(0x105b0608, 0x000bb717);
		blOutp32(0x105b060c, 0x00011217);
		blOutp32(0x105b0614, 1);
	}
}

void InitCmuForBl(int initial, u32 uEglClkMhz, u32 uKfcClkMhz, u32 uMphyClk)
{
	volatile u32 i;

//	blOutp32(0x120f031c, 0x13); // cam0
//	blOutp32(0x145F031c, 0x13); // cam1
//	blOutp32(0x146f031c, 0x13); // isp
//	blOutp32(0x14fa031c, 0x13); // hevc

	blOutp32(0x10030308, 0x11111111);
	blOutp32(0x1003030c, 0x11111);
	for (i=0; i<1000; i++);

	if (initial==1)
	{

		blOutp32(0x14800600, 0x7);
		blOutp32(0x13400600, 0x7);
		blOutp32(0x12460600, 0x7);
		blOutp32(0x150D0600, 0x7);
		blOutp32(0x14AA0600, 0x777 );
		blOutp32(0x15280600, 0x7 );
		blOutp32(0x15380600, 0x7 );
		blOutp32(0x14F80600, 0x7 );
		blOutp32(0x10030600, 0x77777777);
		blOutp32(0x10030604, 0x77777777);
		blOutp32(0x10030608, 0x77);
		blOutp32(0x10030618, 0x7);
		blOutp32(0x1003061C, 0x77777777);
		blOutp32(0x10030620, 0x77777777);
		blOutp32(0x10030634, 0x77777777);
		blOutp32(0x10030638, 0x00019777);
		blOutp32(0x1003063C, 0x77777777);
		blOutp32(0x10030644, 0x77777777);

		blOutp32(0x13b90200, 0x00000000);
		blOutp32(0x13b9020c, 0x00000000);
		blOutp32(0x13b90204, 0x00000000);
		blOutp32(0x156e0200, 0x00000000);
		// For ufs, 24th bit should be 1.
		blOutp32(0x156e0204, 0x00000000);
		blOutp32(0x13cf0200, 0x00000000);
		blOutp32(0x150d0200, 0x00000000);
		blOutp32(0x12460200, 0x00000000);
		blOutp32(0x14f80200, 0x00000000);
		blOutp32(0x15380200, 0x00000000);
		blOutp32(0x146d0200, 0x00000000);
		blOutp32(0x120d0200, 0x00000000);
		blOutp32(0x145d0200, 0x00000000);

//		EnablePPROForBL();
//		EnableRCGForBL();
	}

	InitCmuForWakeup(uEglClkMhz, uKfcClkMhz, uMphyClk);

	if (initial==1)
	{
		//AUD
		blOutp32(0x10030200, 0x00000001);
		blOutp32(0x10030200, 0x00000001);
		blOutp32(0x114c0200, 0x00000000);
		WaitUntilStatus(0x10030400, 8, 7, 4);
		WaitUntilStatus(0x10030400, 4, 7, 4);
		WaitUntilStatus(0x114c0400, 8, 7, 4);

		blOutp32(0x10030004, 0x00002329);
		blOutp32(0x10030114, 0x00000000);
		blOutp32(0x10030110, 0x80c50303);
		WaitUntilStatus(0x10030110, 29, 1, 0);

		blOutp32(0x114c0600, 0x00000520);
		blOutp32(0x114c0604, 0x000715f3);

		blOutp32(0x10030200, 0x00000111);
		blOutp32(0x114c0200, 0x00000011);
		WaitUntilStatus(0x10030400, 8, 7, 4);
		WaitUntilStatus(0x10030400, 4, 7, 4);
		WaitUntilStatus(0x114c0400, 4, 7, 4);
		WaitUntilStatus(0x114c0400, 0, 7, 4);

		//G3D
		blOutp32(0x14aa0200, 0x00000000);
		WaitUntilStatus(0x14aa0400, 0, 7, 4);

		blOutp32(0x14aa0000, 0x000004b1);
		blOutp32(0x14aa0100, 0x810a0602);
		WaitUntilStatus(0x14aa0100, 29, 1, 0);
		blOutp32(0x14aa0600, 0x00000030);

		blOutp32(0x14aa0200, 0x00000001);
		WaitUntilStatus(0x14aa0400, 0, 7, 4);

		//FSYS
		blOutp32(0x156e0200, 0x00000001);
		// For ufs, 24th bit should be 1.
		blOutp32(0x156e0204, 0x00111000);
		WaitUntilStatus(0x156e0404, 16, 7, 4);
		WaitUntilStatus(0x156e0404, 12, 7, 4);
		WaitUntilStatus(0x156e0404, 24, 7, 4);
		WaitUntilStatus(0x156e0400, 0, 7, 4);
		WaitUntilStatus(0x156e0404, 0, 7, 4);
		WaitUntilStatus(0x156e0404, 20, 7, 4);

		//G2D
		blOutp32(0x12460600, 0x00000001);
		blOutp32(0x12460200, 0x00000011);
		WaitUntilStatus(0x12460400, 0, 7, 4);
		WaitUntilStatus(0x12460400, 4, 7, 4);

		//GSCL
		blOutp32(0x150d0600, 0x00000003);
		blOutp32(0x13cf0200, 0x00000011);
		WaitUntilStatus(0x13cf0400, 0, 7, 4);
		WaitUntilStatus(0x13cf0400, 4, 7, 4);

		//MSCL
		blOutp32(0x150d0600, 0x00000001);
		blOutp32(0x150d0200, 0x00000011);
		WaitUntilStatus(0x150d0400, 4, 7, 4);
		WaitUntilStatus(0x150d0400, 0, 7, 4);

		//HEVC
		blOutp32(0x14f80600, 0x00000003);
		blOutp32(0x14f80200, 0x00000001);
		WaitUntilStatus(0x14f80400, 0, 7, 4);


		//MFC0
		blOutp32(0x15280600, 0x00000003);
		blOutp32(0x15280200, 0x00000001);
		WaitUntilStatus(0x15280400, 0, 7, 4);

		//MFC1
		blOutp32(0x15380600, 0x00000003);
		blOutp32(0x15380200, 0x00000001);
		WaitUntilStatus(0x15380400, 0, 7, 4);

		EnableDynamicClkGate();
	}

	/* disable UFS related clocks */
	/* CLK_ENABLE_IP_FSYS0 - CLK_UFS_ENABLE, CLK_MPHY_ENABLE */
	blOutp32(0x156e0b00, (Inp32(0x156e0b00) & ~(1 << 14 | 1 << 5)));
	/* CLK_ENABLE_ACLK_FSYS0 - ACLK_UFS_ENABLE */
	//blOutp32(0x156e0800, (Inp32(0x156e0800) & ~(1 << 5)));
	/* CLK_ENABLE_IP_CPIF0 - CLK_UFS_MPHY_ENABLE */
	blOutp32(0x10fc0b00, (Inp32(0x10fc0b00) & ~(1 << 16)));
	/* CLK_ENABLE_SCLK_TOP_FSYS - SCLK_UFSUNIPRO_ENABLE */
	blOutp32(0x10030a10, (Inp32(0x10030a10) & ~(1 << 3)));
	/* CLK_ENABLE_SCLK_FSYS */
	blOutp32(0x156e0a00, (Inp32(0x156e0a00) & ~(0xf << 13 | 0x2 << 5)));
	/* CLK_ENABLE_SCLK_CPIF - SCLK_UFS_MPHY_ENABLE */
	blOutp32(0x10fc0a00, (Inp32(0x10fc0a00) & ~(1 << 4)));
	/* CLK_MUX_ENABLE_FSYS0 - SCLK_UFS_PLL_USER_ENABLE */
	blOutp32(0x156e0300, (Inp32(0x156e0300) & ~(1 << 4)));
	/* CLK_MUX_ENABLE_FSYS1 - SCLK_UFSUNIPRO_USER_ENABLE */
	blOutp32(0x156e0304, (Inp32(0x156e0304) & ~(1 << 24)));
	/* CLK_MUX_ENABLE_FSYS3 */
	blOutp32(0x156e030c, (Inp32(0x156e030c) & ~(1 << 4 | 1 << 8 | 1 << 12 | 1 << 16)));
	/* CLK_MUX_ENABLE_CPIF1 - PHYCLK_UFS_MPHY_TO_LLI_USER_ENABLE */
	blOutp32(0x10fc0304, (Inp32(0x10fc0304) & ~(1 << 0)));

	/* disable LLI related clocks */
	/* CLK_ENABLE_IP_CPIF0 */
	blOutp32(0x10fc0b00, (Inp32(0x10fc0b00) & ~0xfff));
}

void EnableDynamicClkGate(void)
{
	// PowerPro dynamic clock gating
	// But The parts of CAM0,CAM1,ISP doesn't support dcg.
	blOutp32(0x13b80500, 0x00000000);	// DISP
	blOutp32(0x124c0500, 0x00000000);	// G2D
	blOutp32(0x13cb0500, 0x00000000);	// GSCL
	blOutp32(0x150e0500, 0x00000000);	// MSCL
	blOutp32(0x120f0500, 0x0000001F);	// CAM0
	blOutp32(0x146f0500, 0x0000000C);	// ISP

	// Bus dynamic clock gating
	blOutp32(0x11810204, 0x00000001);	// EGL_XIU
	blOutp32(0x11460204, 0x00000001);	// AUD_XIU
	blOutp32(0x120f0204, 0x00000003);	// CAM0_XIU
	blOutp32(0x120f0208, 0x00000007);	// CAM0_AXI_US
	blOutp32(0x120f020c, 0x000007ff);	// CAM0_XIU_ASYNC
	blOutp32(0x145f0204, 0x00000003);	// CAM1_XIU_TOP
	blOutp32(0x145f0208, 0x00000007);	// CAM1_AXI_US
	blOutp32(0x145f020c, 0x00000fff);	// CAM1_XIU_ASYNC
	blOutp32(0x10fd0204, 0x00000003);	// CPIF_XIU_TOP
	blOutp32(0x10fd0208, 0x00000003);	// CPIF_AXI_US
	blOutp32(0x13b80204, 0x0000001f);	// DISP_XIU_TOP
	blOutp32(0x13b80208, 0x0000001f);	// DISP_AXI_US
	blOutp32(0x156f0204, 0x00000003);	// FSYS_XIU_TOP
	blOutp32(0x156f0208, 0x00000007);	// FSYS_AXI_US
	blOutp32(0x124c0204, 0x00000001);	// G2D_XIU_TOP
	blOutp32(0x124c0208, 0x00000001);	// G2D_AXI_US
	blOutp32(0x124c020c, 0x00000001);	// G2D_XIU_ASYNC
	blOutp32(0x13cb0204, 0x00000001);	// GSCL_XIU_TOP
	blOutp32(0x14fa0204, 0x00000001);	// HEVC_XIU_TOP
	blOutp32(0x11050204, 0x00000003);	// IMEM_XIU_TOP
	blOutp32(0x11050208, 0x00000001);	// IMEM_AXI_US
	blOutp32(0x1105020c, 0x00000001);	// IMEM_XIU_ASYNC
	blOutp32(0x146f0204, 0x00000003);	// ISP_XIU_TOP
	blOutp32(0x146f0208, 0x00000007);	// ISP_AXI_US
	blOutp32(0x146f020c, 0x0000007f);	// ISP_ASYNC
	blOutp32(0x152a0204, 0x00000001);	// MFC0_XIU_TOP
	blOutp32(0x153a0204, 0x00000001);	// MFC1_XIU_TOP
	blOutp32(0x105e0204, 0x00000001);	// MIF_XIU_TOP
	blOutp32(0x105e0208, 0x00000002);	// MIF_AXI_US
	blOutp32(0x105e020c, 0x00001ffe);	// MIF_XIU_ASYNC
	blOutp32(0x150e0204, 0x00000001);	// MSCL_XIU_TOP
}
#endif

void CLKOUT_Disable (void)
{
	/* CLKOUT disable */
	blOutp32(0x14AA0c00, 0x1f); /* G3D */
	blOutp32(0x105B0c00, 0x1f); /* MIF */
	blOutp32(0x10FC0c00, 0x1f); /* CPIF */
	blOutp32(0x13B90c00, 0x1f); /* DISP */
	blOutp32(0x10030c00, 0x1f); /* TOP */
	blOutp32(0x11800c00, 0x1f); /* EGL */
	blOutp32(0x11900c00, 0x1f); /* KFC */
}

void system_clock_init (void)
{
	unsigned int wakeup_stat;

	wakeup_stat = readl(EXYNOS5430_POWER_WAKEUP_STAT);
	wakeup_stat &= WAKEUP_MASK;

	if (wakeup_stat)
		InitCmuForBl(0, 800, 800, 800);
	else {
		InitCmuForBl(1, 500, 800, 800);
		CLKOUT_Disable();
	}
}
