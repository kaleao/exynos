/*
 * Copyright@ Samsung Electronics Co. LTD
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.


 * Alternatively, this program is free software in case of open source projec;
 * you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.

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

#define PLL2X_FIXED_DIV

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

// MUX 변경으로 수정
void InitCmuForWakeup(u32 uEglClkMhz, u32 uKfcClkMhz, u32 uMphyClk) // 1086 825
{
	blOutp32(0x1003060C, 0x777777); /* CLK_DIV_TOP3 */
	blOutp32(0x10030308, 0x10011111);
#ifdef CONFIG_CPU_EXYNOS5433_EVT1
	blOutp32(0x1003030c, 0x111111);
#else
	blOutp32(0x1003030c, 0x11111);
#endif

	//EGL
	blOutp32(0x11800600, 0x00000003); /* CLK_DIV_EGL0 */

	blOutp32(0x11800200, 0x00000000); /* CLK_MUX_SEL_EGL0 - MUX_EGL_PLL: OSCCLK */
	blOutp32(0x11800204, 0x00000001);
	blOutp32(0x11800208, 0x00000000);
	WaitUntilStatus(0x11800400, 0, 7, 4);
	WaitUntilStatus(0x11800404, 0, 7, 4);
	WaitUntilStatus(0x11800408, 0, 7, 4);

	//Wide Lock time for BL
	blOutp32(0x11800000, 0x00000960); /* EGL_PLL_LOCK */

	if (uEglClkMhz == 500)
		blOutp32(0x11800100, 0x80000602|uEglClkMhz<<16);
	else if (uEglClkMhz == 800)
		blOutp32(0x11800100, 0x81900601);
	else if (uEglClkMhz == 1200)
		blOutp32(0x11800100, 0x81f40501);
	else
		while(1);

	WaitUntilStatus(0x11800100, 29, 1, 0);
#ifdef CONFIG_CPU_EXYNOS5433_EVT1
	if (uEglClkMhz == 1200)
		blOutp32(0x11800600, 0x07777300); /* CLK_DIV_EGL0 */
	else
		blOutp32(0x11800600, 0x07777200); /* CLK_DIV_EGL0 */
#else
	blOutp32(0x11800600, 0x00777200); /* CLK_DIV_EGL0 */
#endif
	blOutp32(0x11800604, 0x00000071); /* CLK_DIV_EGL1 [6:4] -> SCLK_HPM_EGL */

	blOutp32(0x11800200, 0x00000001);
	blOutp32(0x11800204, 0x00000001);
	blOutp32(0x11800208, 0x00000000);
	WaitUntilStatus(0x11800400, 0, 7, 4);
	WaitUntilStatus(0x11800404, 0, 7, 4);
	WaitUntilStatus(0x11800408, 0, 7, 4);

	//KFC
	blOutp32(0x11900600, 0x03333103); /* CLK_DIV_KFC0 */

	blOutp32(0x11900200, 0x00000000);
	blOutp32(0x11900204, 0x00000001);
	blOutp32(0x11900208, 0x00000000);
	WaitUntilStatus(0x11900400, 0, 7, 4);
	WaitUntilStatus(0x11900408, 0, 7, 4);
	WaitUntilStatus(0x11900404, 0, 7, 4);

	//Wide Lock time for BL
	blOutp32(0x11900000, 0x00000960);

	if (uKfcClkMhz == 800)
		blOutp32(0x11900100, 0x81900601);
	else if (uKfcClkMhz == 1200)
		blOutp32(0x11900100, 0x81f40501);
	else
		while(1);

	WaitUntilStatus(0x11900100, 29, 1, 0);

	blOutp32(0x11900600, 0x03777200); /* CLK_DIV_KFC0 */
	blOutp32(0x11900604, 0x00000071); /* CLK_DIV_KFC1 [6:4] -> SCLK_HPM_KFC */

	blOutp32(0x11900200, 0x00000001);
	blOutp32(0x11900204, 0x00000001);
	blOutp32(0x11900208, 0x00000000);
	WaitUntilStatus(0x11900400, 0, 7, 4);
	WaitUntilStatus(0x11900404, 0, 7, 4);
	WaitUntilStatus(0x11900408, 0, 7, 4);


	//MIF
	blOutp32(0x105b0200, 0x00000000); /* CLK_MUX_SEL_MIF0 */
	WaitUntilStatus(0x105b0400, 12, 7, 4);
	WaitUntilStatus(0x105b0400, 8, 7, 4);
	WaitUntilStatus(0x105b0400, 4, 7, 4);
	WaitUntilStatus(0x105b0400, 0, 7, 4);

	//Wide Lock time for BL
	Outp32(0x105b0000, 0x00000960);
	Outp32(0x105b0004, 0x00000960);
	Outp32(0x105b0008, 0x00000960);
	Outp32(0x105b000c, 0x00000960);

//MEM0_PLL
//	Outp32(0x105b0100, 0x82150601); // 1066
	Outp32(0x105b0100, 0x816a0402); // 543


//MEM1_PLL
//	Outp32(0x105b0110, 0x81370401); // 933
//	Outp32(0x105b0110, 0x81330401); // 921
#ifdef PLL2X_FIXED_DIV
	Outp32(0x105b0110, 0x81130400); // 825 pll 2x & div 1/2
#else
	Outp32(0x105b0110, 0x81130401); // 825
#endif

//BUS_PLL
    Outp32(0x105b0120, 0x81900601); // 800 /* BUS_PLL_CON0 */
//	Outp32(0x105b0120, 0x81130401); // 825
//MFC_PLL
	Outp32(0x105b0130, 0x80de0401); // 667 /* MFC_PLL_CON0 */
//	Outp32(0x105b0130, 0x80d30401); // 633

	WaitUntilStatus(0x105b0100, 29, 1, 0);
	WaitUntilStatus(0x105b0110, 29, 1, 0);
	WaitUntilStatus(0x105b0120, 29, 1, 0);
	WaitUntilStatus(0x105b0130, 29, 1, 0);

	//MIF BUS clock DIV
	blOutp32(0x105b0604, 0x00031100); /* CLK_DIV_MIF1 */
	blOutp32(0x105b0608, 0x0029b511); /* CLK_DIV_MIF2 */
	blOutp32(0x105b060c, 0x00010017); /* CLK_DIV_MIF3 */
	blOutp32(0x105b0614, 0x00000001); /* CLK_DIV_MIF5 */

	//MIF MUX
	if (uMphyClk == 1086)
		blOutp32(0x105b0204, 0x00000000);
	else if (uMphyClk == 933 || uMphyClk == 921 || uMphyClk == 825) //MEM1_PLL
		blOutp32(0x105b0204, 0x01001000); /* CLK_MUX_SEL_MIF1 */
	else	//BUS_PLL 800
		blOutp32(0x105b0204, 0x01101100);

	blOutp32(0x105b0208, 0x00000101); /* CLK_MUX_SEL_MIF2 */
	WaitUntilStatus(0x105b0404, 4, 7, 4);
	WaitUntilStatus(0x105b0404, 8, 7, 4);
	WaitUntilStatus(0x105b0404, 0, 7, 4);
	WaitUntilStatus(0x105b0404, 12, 7, 4);
	WaitUntilStatus(0x105b0404, 16, 7, 4);
	WaitUntilStatus(0x105b0404, 20, 7, 4);
	WaitUntilStatus(0x105b0404, 24, 7, 4);
	WaitUntilStatus(0x105b0408, 8, 7, 4);
	WaitUntilStatus(0x105b0408, 0, 7, 4);

	//skip special clock setting
#ifdef PLL2X_FIXED_DIV
	blOutp32(0x105b0200, 0x00101111); // pll 2x & div 1/2
#else
	blOutp32(0x105b0200, 0x00001111);
#endif
	blOutp32(0x105b0210, 0x01000001);
	blOutp32(0x105b0214, 0x00000001);

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

	//BUS0
	blOutp32(0x13600600, 0x00000002); /* fixed - not included in DVFS */

	//BUS1
	blOutp32(0x14800600, 0x00000002); /* fixed - not included in DVFS */

	//BUS2
	blOutp32(0x13400600, 0x00000002); /* fixed - not included in DVFS */
	blOutp32(0x13400200, 0x00000001);
	WaitUntilStatus(0x13400400, 0, 7, 4);

	//TOP
	blOutp32(0x10030200, 0x00000000); /* CLK_MUX_SEL_TOP0 */
	blOutp32(0x10030204, 0x00000000); /* CLK_MUX_SEL_TOP1 */
	WaitUntilStatus(0x10030400, 0, 7, 4);
	WaitUntilStatus(0x10030400, 4, 7, 4);
	WaitUntilStatus(0x10030404, 0, 7, 4);
	WaitUntilStatus(0x10030404, 4, 7, 4);
	WaitUntilStatus(0x10030404, 8, 7, 4);
	WaitUntilStatus(0x10030404, 12, 7, 4);

	//TOP DIVIDER
	blOutp32(0x10030600, 0x11011011); /* CLK_DIV_TOP0 */
	blOutp32(0x10030604, 0x51101201); /* CLK_DIV_TOP1 */
	blOutp32(0x10030608, 0x15); /* CLK_DIV_TOP2 */
	blOutp32(0x1003060C, 0x02441515); /* CLK_DIV_TOP3 */
	blOutp32(0x10030610, 0x111); /* CLK_DIV_TOP4 */
	//SDMMC clock setting
	blOutp32(0x1003062C, 0x34);

	blOutp32(0x10030208, 0x00000000); /* CLK_MUX_SEL_TOP2 */
	blOutp32(0x1003020C, 0x00000000); /* CLK_MUX_SEL_TOP3 */
	blOutp32(0x10030210, 0x00000010); /* CLK_MUX_SEL_TOP4 */
	blOutp32(0x1003023C, 0x00001022); /* CLK_MUX_SEL_TOP0_PERIC1 */
	WaitUntilStatus(0x10030408, 28, 7, 4);
	WaitUntilStatus(0x10030408, 16, 7, 4);
	WaitUntilStatus(0x10030408, 12, 7, 4);
	WaitUntilStatus(0x10030408, 8, 7, 4);
	WaitUntilStatus(0x10030408, 4, 7, 4);
	WaitUntilStatus(0x10030408, 0, 7, 4);
	WaitUntilStatus(0x1003040c, 16, 7, 4);
	WaitUntilStatus(0x1003040c, 12, 7, 4);
	WaitUntilStatus(0x1003040c, 8, 7, 4);
	WaitUntilStatus(0x1003040c, 4, 7, 4);
	WaitUntilStatus(0x1003040c, 0, 7, 4);
	WaitUntilStatus(0x10030410, 8, 7, 4);
	WaitUntilStatus(0x10030410, 4, 7, 4);
	WaitUntilStatus(0x10030410, 0, 7, 4);
}

void InitMemClk(u32 uMemclk)
{
	//Mem Clk is same as Helsinki..
	if (uMemclk == 1066 || uMemclk == 921) {
		blSetBits(0x105b1000, 12, 0xFFFFF, 0x00000);
//		blOutp32(0x105b1000, 0x00000110);
		blOutp32(0x105b0208, 0x00000100);
		blOutp32(0x105b0608, 0x00244211);
		blOutp32(0x105b060c, 0x00000013);
	}
	else if (uMemclk == 933)  {
		blSetBits(0x105b1000, 12, 0xFFFFF, 0x00100);
//		blOutp32(0x105b1000, 0x00100110);
		blOutp32(0x105b0208, 0x00000000);
		blOutp32(0x105b0608, 0x00277411);
		blOutp32(0x105b060c, 0x00011213);
	}
	else if (uMemclk == 825) {
		blSetBits(0x105b1000, 12, 0xFFFFF, 0x00100);
//		blOutp32(0x105b1000, 0x00110110);
		blOutp32(0x105b0208, 0x00000101);
		blOutp32(0x105b0608, 0x0029b511); /* CLK_DIV_MIF2 */
		blOutp32(0x105b060c, 0x00000017); /* CLK_DIV_MIF3 */
		blOutp32(0x105b0614, 0x00000001); /* CLK_DIV_MIF5 */
	}
	else if (uMemclk == 800) {
		blSetBits(0x105b1000, 12, 0xFFFFF, 0x00110);
//		blOutp32(0x105b1000, 0x00110110);
		blOutp32(0x105b0208, 0x00000101);
		blOutp32(0x105b0608, 0x00289511);
		blOutp32(0x105b060c, 0x00011213);
	}
	else if (uMemclk == 667 || uMemclk == 666 || uMemclk == 633) {
		blSetBits(0x105b1000, 12, 0xFFFFF, 0x00111);
//		blOutp32(0x105b1000, 0x00111110);
		blOutp32(0x105b0208, 0x00000011);
		blOutp32(0x105b0608, 0x0028b511);
		blOutp32(0x105b060c, 0x00011214);
	}
	else if (uMemclk == 533 || uMemclk == 543) {
		blSetBits(0x105b1000, 12, 0xFFFFF, 0x10000);
//		blOutp32(0x105b1000, 0x10000110);
		blOutp32(0x105b0208, 0x00000010);
		blOutp32(0x105b0608, 0x0028b512);
		blOutp32(0x105b060c, 0x00011215);
	}
	else if (uMemclk == 400) {
		blSetBits(0x105b1000, 12, 0xFFFFF, 0x10110);
//		blOutp32(0x105b1000, 0x10110110);
		blOutp32(0x105b0208, 0x00000010);
		blOutp32(0x105b0608, 0x0028b513);
		blOutp32(0x105b060c, 0x00011217);
	}
	else if (uMemclk == 266) {
		blSetBits(0x105b1000, 12, 0xFFFFF, 0x20110);
//		blOutp32(0x105b1000, 0x20110110);
		blOutp32(0x105b0208, 0x00000010);
		blOutp32(0x105b0608, 0x004bb714);
		blOutp32(0x105b060c, 0x00011217);
	}
	else if (uMemclk == 200) {
		blSetBits(0x105b1000, 12, 0xFFFFF, 0x30110);
//		blOutp32(0x105b1000, 0x30110110);
		blOutp32(0x105b0208, 0x00000010);
		blOutp32(0x105b0608, 0x004bb714);
		blOutp32(0x105b060c, 0x00011217);
	}
	else if (uMemclk == 166) {
		blSetBits(0x105b1000, 12, 0xFFFFF, 0x40110);
//		blOutp32(0x105b1000, 0x40110110);
		blOutp32(0x105b0208, 0x00000010);
		blOutp32(0x105b0608, 0x004bb715);
		blOutp32(0x105b060c, 0x00011217);
	}
	else if (uMemclk == 133) {
		blSetBits(0x105b1000, 12, 0xFFFFF, 0x50110);
//		blOutp32(0x105b1000, 0x50110110);
		blOutp32(0x105b0208, 0x00000010);
		blOutp32(0x105b0608, 0x004bb715);
		blOutp32(0x105b060c, 0x00011217);
	}
	else if (uMemclk == 100)
	{
		blSetBits(0x105b1000, 12, 0xFFFFF, 0x70110);
//		blOutp32(0x105b1000, 0x70110110);
		blOutp32(0x105b0208, 0x00000010);
		blOutp32(0x105b0608, 0x004bb717);
		blOutp32(0x105b060c, 0x00011217);
	}
	else // 50~100Mhz for DREX Initital
	{
		blSetBits(0x105b1000, 12, 0xFFFFF, 0xf0110);
//		blOutp32(0x105b1000, 0x70110110);
		blOutp32(0x105b0208, 0x00000101);
		blOutp32(0x105b0608, 0x004bb717);
		blOutp32(0x105b060c, 0x00011217);
	}
}

void EnablePPROForBL(void)
{
	/* PowerPRO */
	blOutp32(0x124C0500, 0x0);  // G2D
	blOutp32(0x150e0500, 0x0);  // mscl
	blOutp32(0x13B80500, 0x0);  // DISP
	blOutp32(0x13CB0500, 0x0);  // GSCL
	blOutp32(0x146F0500, 0xC);  // ISP
	blOutp32(0x120f0500, 0x1F); // cam0
}

void InitCmuForBl(int initial, u32 uEglClkMhz, u32 uKfcClkMhz, u32 uMphyClk)
{
	if (initial==1)
	{
		//clock divider setting for low current
		blOutp32(0x13600600, 0x7);			//DIV_BUS0
		blOutp32(0x14800600, 0x7);			//DIV_BUS1
		blOutp32(0x13400600, 0x7);			//DIV_BUS2
		blOutp32(0x12460600, 0x3);			//DIV_G2D
		blOutp32(0x150D0600, 0x7);			//DIV_MSCL
		blOutp32(0x14AA0600, 0x777);		//DIV_G3D
		blOutp32(0x15280600, 0x7);			//DIV_MFC
		blOutp32(0x14F80600, 0x3);			//DIV_HEVC
		blOutp32(0x10030600, 0x77777777); 	//DIV_TOP0
		blOutp32(0x10030604, 0x77777777); 	//DIV_TOP1
		blOutp32(0x10030608, 0x75);			//DIV_TOP2
		blOutp32(0x1003060C, 0x7777777);	//DIV_TOP3
		blOutp32(0x10030610, 0x777);		//DIV_TOP4
		blOutp32(0x10030618, 0x7);			//DIV_TOP_MSCL
		blOutp32(0x1003061C, 0x7777777);	//DIV_TOP_CAM10
		blOutp32(0x10030620, 0x7777777);	//DIV_TOP_CAM11
		blOutp32(0x10030630, 0x91);             //DIV_TOP_FSYS1
		blOutp32(0x10030634, 0x7777707);	//DIV_TOP_FSYS2
		blOutp32(0x10030638, 0x7777777);	//DIV_TOP_PERIC0
		blOutp32(0x1003063C, 0x7777777);	//DIV_TOP_PERIC1
		blOutp32(0x10030644, 0x77777777);	//DIV_TOP_PERIC3

		EnablePPROForBL();
	}

	InitCmuForWakeup(uEglClkMhz, uKfcClkMhz, uMphyClk);

	if (initial==1)
	{

//PERIC
		blOutp32(0x14C80600, 0x66);
//FSYS
		blOutp32(0x156E0600, 0x1);
		blOutp32(0x156E0200, 0x1);
		WaitUntilStatus(0x156E0400, 0, 7, 4);

//G3D
		//STEP1. Turn Off PLL FOUT (Set MOUT : OSC)
		blOutp32(0x14AA0200, 0x000);
		WaitUntilStatus(0x14AA0400, 0, 7, 4);
		WaitUntilStatus(0x14AA0400, 4, 7, 4);
		WaitUntilStatus(0x14AA0400, 8, 7, 4);
// DVFS level 5
		//STEP2. SET PLL Locktime
		blOutp32(0x14AA0000, 0x384); //G3D PLL Locktime = 6*150 = 900

		//STEP3. SET PLL Enable & PMS
		blOutp32(0x14AA0100, 0x82140603); //G3D PLL 266Mhz

		//STEP4. SET Divider
		blOutp32(0x14AA0600, 0x330); /* CLK_DIV_G3D */

//ISP
		blOutp32(0x10030000, 0x000002ee); //LOCK time = P value * 150

		//ISP_PLL 552MHZ
		blOutp32(0x10030100, 0x81cc0502);
		WaitUntilStatus(0x10030100, 29, 1, 0);

//AUD
		//STEP1. Turn Off PLL FOUT (Set MOUT : OSC)
		blOutp32(0x114C0200, 0x00000000);
		WaitUntilStatus(0x114C0400, 0, 7, 4);
		WaitUntilStatus(0x114C0400, 4, 7, 4);
		WaitUntilStatus(0x114C0400, 8, 7, 4);

		blOutp32(0x10030004, 0x00001770); //AUD PLL Lock time 6000

		//AUD_PLL 196.608Mhz
		blOutp32(0x10030114, 0x0000126F);
		blOutp32(0x10030110, 0x80830203);

		//AUD_PLL has been turned on at CMU_TOP
		//STEP4. SET Divider
#ifdef CONFIG_CPU_EXYNOS5433_EVT1
		blOutp32(0x114C0600, 0x00003520);
#else
		blOutp32(0x114C0600, 0x00000520);
#endif
		blOutp32(0x114C0604, 0x1FBF7);
		blOutp32(0x114C0608, 0x00000000);

		//SETP5. SET MUXs (OSCLK Indirect MUXs)
		blOutp32(0x114C0204, 0x00000000);
		//WaitUntilStatus(0x114C0404, 0, 7, 4);
		//WaitUntilStatus(0x114C0404, 8, 7, 4);

//DISP
		//STEP1. Turn Off PLL FOUT (Set MOUT : OSC)
		blOutp32(0x13B90200, 0x0);
		WaitUntilStatus(0x13B90400, 0, 7, 4);

		//STEP2. SET PLL Locktime
		blOutp32(0x13B90000, 0x384);

		//STEP3. SET PLL Enable & PMS
		blOutp32(0x13B90100, 0x81F40502); //DISP_PLL 600M

		//STEP4. SET Divider
		blOutp32(0x13B90600, 0x2);

		//STEP5. SET MUX
		blOutp32(0x13B90204, 0x1);
		WaitUntilStatus(0x13B90404, 0, 7, 4);

//G2D
		//STEP4. SET Divider
		blOutp32(0x12460600, 0x1);

		//SETP5. SET MUXs (OSCLK Indirect MUXs)
		blOutp32(0x12460200, 0x11);
		WaitUntilStatus(0x12460400, 0, 7, 4);
		WaitUntilStatus(0x12460400, 4, 7, 4);

//GSCL
		//SETP5. SET MUXs (OSCLK Indirect MUXs)
		blOutp32(0x13CF0200, 0x11);
		WaitUntilStatus(0x13CF0400, 0, 7, 4);
		WaitUntilStatus(0x13CF0400, 4, 7, 4);


//HEVC
		//STEP4. SET Divider
		blOutp32(0x14F80600, 0x3);
		//SETP5. SET MUXs (OSCLK Indirect MUXs)
		blOutp32(0x14F80200, 0x1);
		WaitUntilStatus(0x14F80400, 0, 7, 4);

//MFC
		//STEP4. SET Divider
		blOutp32(0x15280600, 0x3);

		//SETP5. SET MUXs (OSCLK Indirect MUXs)
		blOutp32(0x15280200, 0x1);
		WaitUntilStatus(0x15280400, 0, 7, 4);

//MSCL
		//STEP4. SET Divider
		blOutp32(0x150D0600, 0x1);

		//SETP5. SET MUXs (OSCLK Indirect MUXs)
		blOutp32(0x150D0200, 0x11);
		WaitUntilStatus(0x150D0400, 0, 7, 4);
		WaitUntilStatus(0x150D0400, 4, 7, 4);

		blOutp32(0x150D0204, 0x1);
		WaitUntilStatus(0x150D0404, 0, 7, 4);

//CPIF
		//STEP1. Turn Off PLL FOUT (Set MOUT : OSC)
		blOutp32(0x10FC0200, 0x0);
		WaitUntilStatus(0x10FC0400, 0, 7, 4);

		//STEP2. SET PLL Locktime
		blOutp32(0x10FC0000, 0x384); //MPHY_PLL Locktime 6*150 = 900

		//STEP3. SET PLL Enable & PMS
		blOutp32(0x10FC0100, 0x81D20602); //MPHY_PLL 466Mhz

		//STEP4. SET Divider
		blOutp32(0x10FC0600, 0x34);

		//SETP5. SET MUXs (OSCLK Indirect MUXs)

// ********** Turn On PLL (PLL Adjacent Mux change) **********

//ATLAS PLL
		blOutp32(0x11800200, 0x1);
		WaitUntilStatus(0x11800400, 0, 7, 4);

//APOLLO PLL
		blOutp32(0x11900200, 0x1);
		WaitUntilStatus(0x11900400, 0, 7, 4);

//MEM0_PLL / MEM1_PLL / MFC_PLL / BUS_PLL
#ifdef PLL2X_FIXED_DIV
		blOutp32(0x105B0200, 0x00101111); // pll 2x & div 1/2
#else
		blOutp32(0x105B0200, 0x00001111);
#endif
		WaitUntilStatus(0x105B0400, 0, 7, 4);
		WaitUntilStatus(0x105B0400, 4, 7, 4);
		WaitUntilStatus(0x105B0400, 8, 7, 4);
		WaitUntilStatus(0x105B0400, 12, 7, 4);
		WaitUntilStatus(0x105B0400, 16, 7, 4);
		WaitUntilStatus(0x105B0400, 20, 7, 4);
		WaitUntilStatus(0x105B0400, 24, 7, 4);
		WaitUntilStatus(0x105B0400, 28, 7, 4);

//ISP_PLL / AUD_PLL
		blOutp32(0x10030200, 0x00000011);
		WaitUntilStatus(0x10030400, 0, 7, 4);
		WaitUntilStatus(0x10030400, 4, 7, 4);

		blOutp32(0x10030204, 0x1111);
		WaitUntilStatus(0x10030404, 0, 7, 4);
		WaitUntilStatus(0x10030404, 4, 7, 4);
		WaitUntilStatus(0x10030404, 8, 7, 4);
		WaitUntilStatus(0x10030404, 12, 7, 4);

// DISP_PLL
		blOutp32(0x13B90200, 0x00000001);
		WaitUntilStatus(0x13B90400, 0, 7, 4);

//G3D_PLL
#ifdef CONFIG_CPU_EXYNOS5433_EVT1
		blOutp32(0x14AA0200, 0x00000001);
#else
		blOutp32(0x14AA0200, 0x00000011);
#endif
		WaitUntilStatus(0x14AA0400, 0, 7, 4);
		WaitUntilStatus(0x14AA0400, 4, 7, 4);
		WaitUntilStatus(0x14AA0400, 8, 7, 4);

//AUD_PLL_USER
		blOutp32(0x114C0200, 0x1);
		WaitUntilStatus(0x114C0400, 0, 7, 4);

//MPHY_PLL
		blOutp32(0x10FC0200, 0x1);
		WaitUntilStatus(0x10FC0400, 0, 7, 4);

	}

//	OpenConsole();
}

void system_clock_init (void)
{
	unsigned int wakeup_stat;

	wakeup_stat = readl(EXYNOS5430_POWER_WAKEUP_STAT);
	wakeup_stat &= WAKEUP_MASK;

	if (wakeup_stat)
		InitCmuForBl(0, 800, 800, 825);
	else {
		InitCmuForBl(1, 1200, 1200, 825);
	}
}
