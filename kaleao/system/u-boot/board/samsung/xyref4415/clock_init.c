/*
 * Clock setup for XYREF4415 board based on EXYNOS4
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
#include <version.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/power.h>
#include <asm/arch/cpu.h>
#include <asm/arch/gpio.h>
#include "setup.h"
#include "cmu.h"

#define Outp32(addr, data) (*(volatile u32 *)(addr) = (data))
#define Inp32(addr) ((*(volatile u32 *)(addr)))

void CMU_Delay(int msec)
{
	volatile u32 i;
	for(;msec > 0; msec--);
		for(i=0; i<1000; i++) ;
}








void CMU_SetMuxDiv_LRBUS(void)
{
	volatile u32 uBits;

	// ¡Ú Set CMU_LEFTBUS, MUX & DIV
	// MPLL_USER(1:4), GDL(1:0)
	uBits = (1 << 4) | (0 << 0);
	Outp32(rCLK_SRC_LEFTBUS, uBits);			// rCLK_SRC_LEFTBUS

	// GPL(1/2:4), GDL(1/4:0)
	uBits = (1 << 4) | (3 << 0);
	Outp32(rCLK_DIV_LEFTBUS, uBits);			// rCLK_DIV_LEFTBUS


	// ¡Ú Set CMU_RIGHTBUS, MUX & DIV
	// MPLL_USER(1:4), GDR(0:0)
	uBits = (1 << 4) | (0 << 0);
	Outp32(rCLK_SRC_RIGHTBUS, uBits);			// rCLK_SRC_RIGHTBUS

	// GPR(1/2:4), GDR(1/4:0)
	uBits = (1 << 4) | (3 << 0);
	Outp32(rCLK_DIV_RIGHTBUS, uBits);			// rCLK_DIV_RIGHTBUS
}



void CMU_SetMuxDiv_ISP(void)
{
	volatile u32 uBits;

	// ¡Ú Set CMU_ISP, MUX & DIV
	//
	// Never write to CMU_ISP, because system hang when PMU power test (in case of Gaia)
#if 0
	// Do not access ISP clk div sfr, when ISP is off. (added at Carmen for the first time)
	uBits = Inp32(0x10023CA4);		// ISP_STATUS
	if((uBits & 0x7) == 0x7)		// ISP_STATUS.STATUS [2:0]
	{
		// ISPDIV1(1/4:4), ISPDIV0(1/2:0)
		uBits = (3 << 4) | (1 << 0);
		Outp32(0x10048300, uBits);			// rCLK_DIV_ISP0

		// MCUISPDIV1(1/4:8), MCUISPDIV0(1/2:4), MPWMDIV(1/1:0)
		uBits = (3 << 8) | (1 << 4) | (0 << 0);
		Outp32(0x10048304, uBits);			// rCLK_DIV_ISP1
	}
#else

	// PHYCLK_RXBYTE_CLKHS0_S2A(1:24), PHYCLK_RXBYTE_CLKHS0_S4(1:20), PWM(1:16), MTCADC(1:12), SPI0(1:8), SPI1(1:4), UART(1:0)
	uBits = (1 << 24) | (1 << 20) | (1 << 16) | (1 << 12) | (1 << 8) | (1 << 4) | (1 << 0);
	Outp32(rCLK_SRC_ISP0, uBits);			// 0x12060200

	// rCLK_SRC_ISP1 :: Reserved
	Outp32(rCLK_SRC_ISP1, 0x0); 		// 0x12070200


	// PCLK_ISP0_A_150(1/2:20), ISP_MPWM(1/4:16), SCLK_PCLKDBG(1/4:12), SCLK_ATCLK(1/2:8), PCLK_ISP0_B_75(1/4:4), PCLK_ISP0_B_150(1/2:0)
	uBits = (1 << 20) | (3 << 16) | (3 << 12) | (1 << 8) | (3 << 4) | (1 << 0);
	Outp32(rCLK_DIV_ISP0, uBits);			// 0x12060500

	// PCLK_ISP1_75(1/4:4), PCLK_ISP1_150(1/2:0)
	uBits = (3 << 4) | (1 << 0);
	Outp32(rCLK_DIV_ISP1, uBits);			// 0x12070500

#endif

}


void CMU_SetMuxDiv_TOP(void)
{
	volatile u32 uBits;

	// ¡Ú Set CMU_TOP, MUX & DIV
	//
	// EBI(0:28), ACLK_200(0:24), ACLK_160(0:20), ACLK_100(0:16), ACLK_266(0:12), G3D_PLL(0:8), EPLL(0:4), EBI_1(0:0)
	uBits = (0 << 28) | (0 << 24) | (0 << 20) | (0 << 16) | (0 << 12) | (0 << 8) | (0 << 4) | (0 << 0);
	Outp32(rCLK_SRC_TOP0, uBits);			//0x1003C210

#if 0
	// MPHY_PLL(0:28), ACLK_400_MCUISP_SUB(1:24), ACLK_266_SUB(1:20), MPLL_USER(1:12), ACLK_400_MCUISP(0:8), VPLLSRC(0:0)
	uBits = (0 << 28) | (1 << 24) | (1 << 20) | (1 << 12) | (0 << 8) | (0 << 0);
#else
	// ISP_PLL(0:28), DISP_PLL(1:16), MPLL_USER(1:12), ACLK_400_MCUISP(0:8), G3D_PLLSRC_SEL(0:0)
	uBits = (1 << 28) | (1 << 16) | (1 << 12) | (0 << 8) | (0 << 0);
#endif	
	Outp32(rCLK_SRC_TOP1, uBits);			// 0x1003C214
	
#if 0 // for NOR Booting
	// 2013.01.04 support for nor booting
	if ((Inp32(0x10020000) & 0x3F) == 0x1D)
	{
		// NOR Boot case
		// ACLK_400_MCUISP(1/2:24), EBI(1/2:16), ACLK_200(1/8:12), ACLK_160(1/5:8), ACLK_100(1/8:4), ACLK_266(1/3:0)
		uBits = (1 << 24) | (1 << 16) | (7 << 12) | (4 << 8) | (7 << 4) | (2 << 0);
	}
	else
	{
		// ACLK_400_MCUISP(1/2:24), EBI(1/1:16), ACLK_200(1/4:12), ACLK_160(1/5:8), ACLK_100(1/8:4), ACLK_266(1/3:0)
		uBits = (1 << 24) | (0 << 16) | (3 << 12) | (4 << 8) | (7 << 4) | (2 << 0);
	}	
	Outp32(0x1003C510, uBits);											// rCLK_DIV_TOP
#endif	
	// ACLK_400_MCUISP(1/2:24), EBI(1/1:16), ACLK_200(1/4:12), ACLK_160(1/5:8), ACLK_100(1/8:4), ACLK_266(1/3:0)
	uBits = (1 << 24) | (0 << 16) | (3 << 12) | (4 << 8) | (7 << 4) | (7 << 0);
	Outp32(rCLK_DIV_TOP, uBits);											//0x1003C510
}


//DMC <== Core_L
void CMU_SetMuxDiv_DMC(void)
{
	volatile u32 uBits;

	// ¡Ú Set CMU_CORE_L, MUX & DIV
	// MPLL(0:12), BPLL(0:10), DPHY(0:8), DMC_BUS(0:4)
	uBits = (0 << 12) | (0 << 10) | (1 << 8) | (1 << 4);		// use BPLL_IN source for DPHY,DMC in Hudson
	Outp32(rCLK_SRC_DMC, uBits);											// 0x105c0300
	// BPLL_PRE(1/2:30),DPHY(0:8) => Fixed
	// DMC(1/2:27), DMC_PRE(1/1:19), COREP(1/2:15), CORED(1/2:11), MPLL_PRE(1/1:8)
	//uBits = (4 << 27) | (1 << 23) | (3 << 19) | (1 << 15) | (1 << 11) | (0 << 8);		// DMC [PLL/20]
																						// - 1266Mhz => 63.3MHz
																						// - 1066Mhz => 53.3MHz
	//Temp for MIF L0 667 Test
	uBits = (5 << 27) | (1 << 23) | (3 << 19) | (1 << 15) | (1 << 11) | (0 << 8);		// DMC [PLL/24]
																						// - 1334Mhz => 55.58MHz
																						// - 1066Mhz => 44.42MHz


	Outp32(rCLK_DIV_DMC1, uBits);	// 0x105C0504
}



//ACP <== Core_R
void CMU_SetMuxDiv_ACP(void)
{
	volatile u32 uBits;

	// ¡Ú Set CMU_CORE_R, MUX & DIV
	// G2D_ACP(0:28), G2D_ACP_1(1:24), G2D_ACP_0(0:20) PWI(0:16), MPLL_USER(1:13), BPLL_USER(1:11), CORE_BUS(0:4)
	uBits = (0 << 28) | (1 << 24) | (0 << 20) | (1 << 16) | (1 << 13) | (1 << 11) | (0 << 4);
	Outp32(rCLK_SRC_ACP, uBits);										// 0x10450300
	
	// COREP(1/2:16), CORED(1/2:12), DMC(1/2:8), ACP_PCLK(1/2:4), ACP(1/4:0)
	uBits = (1 << 16) | (1 << 12) | (1 << 8) | (1 << 4) | (3 << 0);
	Outp32(rCLK_DIV_ACP0, uBits);										// 0x10450500		

	// PWI(1/1:5), G2D_ACP(1/4:0)
	uBits = (0 << 5) | (3 << 0);
	Outp32(rCLK_DIV_ACP1, uBits);										// 0x10450504
	
}


void CMU_SetMuxDiv_TOP_Sub(void)
{
	volatile u32 uBits;

// ¡Ú Set MUX
	//TSADC(1:28), UFS_UNIPRO(1:16), MMC3(1:12), *MMC2(6:8), MMC1(6:4), *MMC0(6:0), // (*) : That means IROM booting device.
	uBits = (1<<28)|(1<<16)|(1<<12)|(6<<8)|(6<<4)|(6<<0);
	Outp32(rCLK_SRC_FSYS, uBits);				// 0x1003C240	

	//UART4(1:16), *UART3(6:12), UART2(6:8), UART1(6:4), UART0(6:0) 					// Only UART3 is MPLL for IROM UART Booting
	uBits = (1<<16)|(6<<12)|(1<<8)|(1<<4)|(1<<0);
	Outp32(rCLK_SRC_PERIL0, uBits); 			// 0x1003C250	

	//SPI2(6:24), *SPI1(6:20), SPI0(6:16), SPDIF(0:8), AUDIO2(7:4), AUDIO1(7:0) 			// - MPLL(SPI) , EPLL(AUDIO)
	uBits = (6<<24)|(6<<20)|(6<<16)|(0<<8)|(7<<4)|(7<<0);
	Outp32(rCLK_SRC_PERIL1, uBits); 			// 0x1003C254	

	//AUDIO0(7:0)							// - EPLL(AUDIO)
	uBits = (7<<0);
	Outp32(rCLK_SRC_MAUDIO, uBits); 			// 0x1003C23C

	//G3D(1:8), G3D_1(1:4), G3D_0(0:0), 	// - G3DPLL
	uBits = (1<<8)|(1<<4)|(0<<0);
	Outp32(rCLK_SRC_G3D, uBits);				// 0x1003C22C

	//MIXER_SEL(4), HDMI_SEL(0)
	uBits = (1<<4) | (1<<0);				//ToDo ::
	Outp32(rCLK_SRC_TV, uBits); 			// 0x1003C224

	//MFC(0:8), MFC_1(1:4), MFC_0(0:0), 	// - MPLL(MFC)
	uBits = (0<<8)|(1<<4)|(0<<0);
	Outp32(rCLK_SRC_MFC, uBits);				// 0x1003C228


	//ACLK_ISP0_300(1:8), USER_MUX_ACLK_ISP0_400(1:4), USER_MUX_ACLK_ISP0_300(1:0), ; - rCLK_SRC_ISP0_T
	uBits=(0<<8)|(1<<4)|(1<<0);
	Outp32(rCLK_SRC_ISP0_T, uBits); 			// 0x1003C25c

	//ACLK_ISP1_300(1:4), USER_MUX_ACLK_ISP1_300(0:0),		; - rCLK_SRC_ISP1_T
	uBits=(1<<4)|(1<<0);
	Outp32(rCLK_SRC_ISP1_T, uBits); 			// 0x1003C260



	//CAM, LCD0,ISP CLK SRC  ????????????????????????
	//ISP : all 6 - MPLL »ç¿ë ==> Carmen  
	// hudson  : 0x3 = ISPPLL ??
	



	

	// ¡Ú Set DIV
	//UART4(1/1:16), *UART3(1/16:12), UART2(1/1:8), UART1(1/1:4), UART0(1/1:0)				// UART3=800MHz/16=50MHz, UART0,1,2,4=24/1=24
	uBits = (0<<16)|(15<<12)|(0<<8)|(0<<4)|(0<<0);
	Outp32(rCLK_DIV_PERIL0, uBits); 			// 0x1003C550

	//SPI1_PRE(1/5:24), *SPI1(1/4:16), SPI1_PRE(1/5:8), SPI0(1/4:0) 							// - 800MHz/4/5=40MHz
	uBits = (4<<24)|(3<<16)|(4<<8)|(3<<0);
	Outp32(rCLK_DIV_PERIL1, uBits); 			// 0x1003C554

	//SPI2_PRE(1/5:8), SPI2(1/4:0)															// - 800MHz/4/5=40MHz
	uBits = (4<<8)|(3<<0);
	Outp32(rCLK_DIV_PERIL2, uBits); 			// 0x1003C558

	//PCM2(1/2:20), AUDIO2(1/2:16), PCM1(1/2:4), AUDIO1(1/2:0)
	uBits = (1<<20)|(1<<16)|(1<<4)|(1<<0);
	Outp32(rCLK_DIV_PERIL4, uBits); 			// 0x1003C560

	//I2S2(1/2:8), I2S1(1/2:0)
	uBits = (1<<8)|(1<<0);
	Outp32(rCLK_DIV_PERIL5, uBits); 			// 0x1003C564

	

	//JPEG(1/5:0)		, ACLK_JPEG=160MHz
	uBits = (4<<0);
	Outp32(rCLK_DIV_CAM1, (Inp32(rCLK_DIV_CAM1)&~(0xF<<0))|(uBits));				// 0x1003C568

	//PCM0(1/2:4), AUDIO0(1/2:0)
	uBits = (1<<4)|(1<<0);
	Outp32(rCLK_DIV_MAUDIO, uBits); 			// 0x1003C53C

	//TV(1/4:0)
	uBits = (3<<0);
	Outp32(rCLK_DIV_TV, uBits); 				// 0x1003C524

	//MFC(1/4:0)
	uBits = (3<<0);
	Outp32(rCLK_DIV_MFC, uBits);				// 0x1003C528

	//G3D(1/1:0)
	uBits = (0<<0);
	Outp32(rCLK_DIV_G3D, uBits);				// 0x1003C52C

	//TSADC_PRE(1/1:8), TSADC(1/8:0)
	uBits = (0<<8)|(7<<0);
	Outp32(rCLK_DIV_FSYS0, uBits);				// 0x1003C540

	//MMC1_PRE(1/5:24), MMC1(1/4:16), MMC0_PRE(1/5:8), MMC0(1/4:0)							// - 800MHz/4/5=40MHz
	uBits = (4<<24)|(3<<16)|(4<<8)|(3<<0);
	Outp32(rCLK_DIV_FSYS1, uBits);				// 0x1003C544

	//MMC3_PRE(1/5:24), MMC3(1/4:16), MMC2_PRE(1/5:8), MMC2(1/4:0)							// - 800MHz/4/5=40MHz
	uBits = (15<<24)|(15<<16)|(4<<8)|(3<<0);
	Outp32(rCLK_DIV_FSYS2, uBits);				// 0x1003C548

	// UFS_UNIPRO(1/16:0)
	uBits = (15 << 0);
	Outp32(rCLK_DIV_FSYS3, uBits);				// 0x1003C54C


	//ACLK_ISP1_300(1/3:16) TSADC_ISP_B(1/5:8), TSADC_ISP_A(1/4:0)
	uBits=(2<<16)|(4<<8)|(3<<0);
	Outp32(rCLK_DIV_ISP1_T, uBits);				// 0x1003C56c

	//ACLK_ISP0_300(1/16:0)
	uBits=(0<<0);
	Outp32(rCLK_DIV_ISP0_T, uBits);				// 0x1003C570

}




void CMU_Init(u32 nARMCLK)
{
	volatile u32 uBits;

// ¡Ú Set PLL Time 
	Outp32(rAPLL_LOCK, 2400);
	Outp32(rMPLL_LOCK, 2400);
	Outp32(rBPLL_LOCK, 2400);
	Outp32(rG3DPLL_LOCK, 2400);
	Outp32(rDISPPLL_LOCK, 2400);
	Outp32(rISPPLL_LOCK, 2400);

	Outp32(rEPLL_LOCK, 7200);

// ¡Ú Turn Off PLL Mout 
	//APLL
	uBits = (1 << 0);
	Outp32(rCLK_SRC_CPU, Inp32(rCLK_SRC_CPU) & ~uBits);	// rCLK_SRC_CPU
	//MPLL, BPLL
	uBits = (1<<12)|(1<<10);
	Outp32(rCLK_SRC_DMC, Inp32(rCLK_SRC_DMC) & ~uBits);	// rCLK_SRC_DMC
	//G3DPLL, EPLL
	uBits = (1<<8)|(1<<4);
	Outp32(rCLK_SRC_TOP0, Inp32(rCLK_SRC_TOP0) & ~uBits);	// rCLK_SRC_TOP0
	//ISPPLL, DISPPLL
	uBits = (1<<28)|(1<<16);
	Outp32(rCLK_SRC_TOP1, Inp32(rCLK_SRC_TOP1) & ~uBits);	// rCLK_SRC_TOP1


	// ¡Ú Set CMU_CPU, MUX & DIV
	// ¡Ú Set CMU_CPU, MUX & DIV
	// MPLL_USER(1:24), HPM(0:20), CORE(0:16), APLL(0:0)
	uBits = (1 << 24) | (0 << 20) | (0 << 16) | (0 << 0);
	Outp32(rCLK_SRC_CPU, uBits);			// 0x10044200

	// ¢º CORE2(1/1:28), APLL(1/2:24), PCLK_DBG(1/2:20), ATB(1/4:16), PERIPHCLK_RATIO(1/8:12), COREM1(1/4:8), COREM0(1/3:4), CORE(1/1:0)
	uBits = (0 << 28) | (1 << 24) | (1 << 20) | (3 << 16) | (7 << 12) | (3 << 8) | (2 << 4) | (0 << 0);
	Outp32(rCLK_DIV_CPU0, uBits);			// 0x10044500

	// ¢º HPM(1/1:4), COPY(1/3:0)
	uBits = (7 << 4) | (7 << 0);
	Outp32(rCLK_DIV_CPU1, uBits);			// 0x10044504


	// ¡Ú Set PLL
	// ¡Ú Setting APLL [P,M,S]
	//
	uBits = (1 << 23) | (1 << 21) | (3 << 14) | (3 << 12) | (3 << 10);
	Outp32(rAPLL_CON1, uBits);		//	0x10044104

	uBits = (2 << 4);
	Outp32(rAPLL_CON2, uBits);		// 0x10044108


	// ARM Clock = 1200 MHz
	// ENABLE(1:31), MDIV(m:16), PDIV(p:8), SDIV(s:0)
	if (nARMCLK == 1700)
	{
		uBits = (1 << 31) | (425 << 16) | (6 << 8) | (0 << 0);	// APLL=1700MHz(6:425:0)	
	}	
	else if (nARMCLK == 1600)
	{
		uBits = (1 << 31) | (200 << 16) | (3 << 8) | (0 << 0);	// APLL=1600MHz(3:200:0)	
	}		
	else if (nARMCLK == 1500)		
	{
		uBits = (1 << 31) | (250 << 16) | (4 << 8) | (0 << 0);	// APLL=1500MHz(4:250:0)
	}		
	else if (nARMCLK == 1400)		
	{
		uBits = (1 << 31) | (175 << 16) | (3 << 8) | (0 << 0);	// APLL=1400MHz(3:175:0)
	}		
	else if (nARMCLK == 1300)
	{
		uBits = (1 << 31) | (325 << 16) | (6 << 8) | (0 << 0);	// APLL=1300MHz(6:325:0)
	}		
	else if (nARMCLK == 1200)
	{
		uBits = (1 << 31) | (400 << 16) | (4 << 8) | (1 << 0);	// APLL=1200MHz(4:400:1)
	}		
	else if (nARMCLK == 1100)
	{
		uBits = (1 << 31) | (275 << 16) | (3 << 8) | (1 << 0);	// APLL=1100MHz(3:275:1)
	}		
	else if (nARMCLK == 1000)
	{
		uBits = (1 << 31) | (250 << 16) | (3 << 8) | (1 << 0);	// APLL=1000MHz(3:250:1)
	}		
	else if (nARMCLK == 900)
	{
		uBits = (1 << 31) | (300 << 16) | (4 << 8) | (1 << 0);	// APLL=900MHz(4:300:1)
	}		
	else if (nARMCLK == 800)
	{
		uBits = (1 << 31) | (200 << 16) | (3 << 8) | (1 << 0);	// APLL=800MHz(3:200:1)
	}	
	else if (nARMCLK == 700)
	{
		uBits = (1 << 31) | (175 << 16) | (3 << 8) | (1 << 0);	// APLL=700MHz(3:175:1)
	}		
	else if (nARMCLK == 600)
	{
		uBits = (1 << 31) | (400 << 16) | (4 << 8) | (2 << 0);	// APLL=600MHz(4:400:2)
	}
	else if (nARMCLK == 500)
	{
		uBits = (1 << 31) | (250 << 16) | (3 << 8) | (2 << 0);	// APLL=500MHz(3:250:2)
	}
	else if (nARMCLK == 400)
	{
		uBits = (1 << 31) | (200 << 16) | (3 << 8) | (2 << 0);	// APLL=400MHz(3:200:2)
	}	
	else if (nARMCLK == 300)
	{
		uBits = (1 << 31) | (400 << 16) | (4 << 8) | (3 << 0);	// APLL=300MHz(4:400:3)
	}	
	else if (nARMCLK == 200)
	{
		uBits = (1 << 31) | (200 << 16) | (3 << 8) | (3 << 0);	// APLL=200MHz(3:200:3)
	}
	else //if (nARMCLK == 100)
	{
		uBits = (1 << 31) | (200 << 16) | (3 << 8) | (4 << 0);	// APLL=100MHz(3:200:4)
	}	

	Outp32(rAPLL_CON0, uBits);		// 0x10044100
	while ((Inp32(rAPLL_CON0) & (1 << 29)) == 0);

	


	// ¡Ú Setting MPLL [P,M,S]
	uBits = (1 << 23) | (1 << 21) | (3 << 14) | (3 << 12) | (3 << 10);
	Outp32(rMPLL_CON1, uBits);		// 0x105C010C	

	uBits = (2 << 4);
	Outp32(rMPLL_CON2, uBits);		// 0x105C0110

//	uBits = (1 << 31) | (200 << 16) | (3 << 8) | (1 << 0);	// MPLL=800MHz(3:200:1)
	uBits = (1 << 31) | (275 << 16) | (4 << 8) | (1 << 0);	// MPLL=825MHz(4:275:1)
	Outp32(rMPLL_CON0, uBits);		// 0x105C0108
	while ((Inp32(rMPLL_CON0) & (1 << 29)) == 0);

	


	// ¡Ú Setting BPLL [P,M,S]
	uBits = (1 << 23) | (1 << 21) | (3 << 14) | (3 << 12) | (3 << 10);
	Outp32(rBPLL_CON1, uBits);		// 0x105C021C

	uBits = (2 << 4);
	Outp32(rBPLL_CON2, uBits);		// 0x105C0220

	// ENABLE(1:31), MDIV(m:16), PDIV(p:8), SDIV(s:0) ... Set Max Value, Clock is on Div
	//uBits = (1 << 31) | (533 << 16) | (6 << 8) | (0 << 0);	// BPLL=2132MHz(6:533:0)
	uBits = (1 << 31) | (362 << 16) | (4 << 8) | (1 << 0);	// BPLL= 10864MHz(4:362:1) //543
	//uBits = (1 << 31) | (667 << 16) | (12 << 8) | (0 << 0);	// BPLL=1334MHz(12:667:0) //667
	//uBits = (1 << 31) | (211 << 16) | (4 << 8) | (0 << 0);	// BPLL=1266MHz(4:211:0) //633
	//uBits = (1 << 31) | (533 << 16) | (6 << 8) | (1 << 0);	// BPLL=1066MHz(6:533:1)
	//uBits = (1 << 31) | (220 << 16) | (3 << 8) | (1 << 0);	// BPLL=880MHz(6:533:1)
	//	uBits = (1 << 31) | (200 << 16) | (3 << 8) | (1 << 0);	// BPLL= 800MHz(3:200:1)
	Outp32(rBPLL_CON0, uBits);		// 0x105C0218
	while ((Inp32(rBPLL_CON0) & (1 << 29)) == 0);

	

	// ¡Ú Setting EPLL : SEL_PF[31:30],MRR[29:24],MFR[23:16], K[15:00]
	uBits = (3 << 30) | (6 << 24) | (1 << 16)| (1 << 0);
	Outp32(rEPLL_CON1, uBits);		// 0x1003C114

	// VCO_BOOST[28],AFCINIT_SEL[27]
	uBits = (1 << 28) | (1 << 27);
	Outp32(rEPLL_CON2, uBits);		// 0x1003C118

	// ENABLE(1:31), MDIV(m:16), PDIV(p:8), SDIV(s:0)
	uBits = (1 << 31) | (128 << 16) | (2 << 8) | (3 << 0);	// EPLL=192MHz(2:128:3), 
	Outp32(rEPLL_CON0, uBits);		// 0x1003C110
	while ((Inp32(rEPLL_CON0) & (1 << 29)) == 0);

	

	
	// ¡Ú Setting G3DPLL
	uBits = (1 << 23) | (1 << 21) | (3 << 14) | (3 << 12) | (3 << 10);
	Outp32(rG3D_PLL_CON1, uBits);			// 0x1003C124

	uBits = (2 << 4);
	Outp32(rG3D_PLL_CON2, uBits);			// 0x1003C128

	uBits = (1 << 31) | (175 << 16) | (3 << 8) | (2 << 0);				// G3DPLL=350MHz(3:175:2)
	Outp32(rG3D_PLL_CON0, uBits);			// 0x1003C120
	while ((Inp32(rG3D_PLL_CON0) & (1 << 29)) == 0);




	// ¡Ú Setting ISPPLL
	uBits = (1 << 23) | (1 << 21) | (3 << 14) | (3 << 12) | (3 << 10);
	Outp32(rISP_PLL_CON1, uBits);		// 0x1003C134

	uBits = (2 << 4);
	Outp32(rISP_PLL_CON2, uBits);		// 0x1003C138


	// ENABLE(1:31), MDIV(m:16), PDIV(p:8), SDIV(s:0)
	uBits = (1 << 31) | (400 << 16) | (4 << 8) | (3 << 0);	// ISPPLL=300MHz(4:400:3)
	//uBits = (1 << 31) | (533 << 16) | (6 << 8) | (3 << 0);	// ISPPLL=266MHz(6:533:3)
	Outp32(rISP_PLL_CON0, uBits);		// 0x1003C130
	while ((Inp32(rISP_PLL_CON0) & (1 << 29)) == 0);



	// ¡Ú Setting DISPPLL [P,M,S]
	uBits = (1 << 23) | (1 << 21) | (3 << 14) | (3 << 12) | (3 << 10);
	Outp32(rDISP_PLL_CON1, uBits);		// 0x1003C144

	uBits = (2 << 4);
	Outp32(rDISP_PLL_CON2, uBits);		// 0x1003C148


	// ENABLE(1:31), MDIV(m:16), PDIV(p:8), SDIV(s:0)
	//uBits = (1 << 31) | (260 << 16) | (3 << 8) | (2 << 0);	// DISPPLL=520MHz(3:260:2)
	uBits = (1 << 31) | (266 << 16) | (3 << 8) | (3 << 0);	// DISPPLL=266MHz(3:266:3)
	Outp32(rDISP_PLL_CON0, uBits);		// 0x1003C140
	while ((Inp32(rDISP_PLL_CON0) & (1 << 29)) == 0);




	// ¡Ú Set clk Mux and Div .. Sub_block
	CMU_SetMuxDiv_ACP();
	CMU_SetMuxDiv_LRBUS();
	CMU_SetMuxDiv_ISP();
	CMU_SetMuxDiv_TOP();

	//Special Clock MuxDiv in TOP
	CMU_SetMuxDiv_TOP_Sub();
	CMU_SetMuxDiv_DMC();


// ¡Ú Turn On PLL Mout
	//APLL
	uBits = (1 << 0);
	Outp32(rCLK_SRC_CPU, Inp32(rCLK_SRC_CPU) | uBits);	// rCLK_SRC_CPU
	//BPLL
	uBits = (1<<10);
	Outp32(rCLK_SRC_DMC, Inp32(rCLK_SRC_DMC) | uBits);	// rCLK_SRC_DMC
	//MPLL
	uBits = (1<<12);
	Outp32(rCLK_SRC_DMC, Inp32(rCLK_SRC_DMC) | uBits);	// rCLK_SRC_DMC
	//G3DPLL, EPLL
	uBits = (1<<8)|(1<<4);
	Outp32(rCLK_SRC_TOP0, Inp32(rCLK_SRC_TOP0) | uBits);	// rCLK_SRC_TOP0
	//ISPPLL, DISPPLL
	uBits = (1<<28)|(1<<16);
	Outp32(rCLK_SRC_TOP1, Inp32(rCLK_SRC_TOP1) | uBits);	// rCLK_SRC_TOP1
	
}

void CMU_InitForMif(u32 uMemClk)
{
	volatile u32 uBits,uTemp;

	// SCLK_DMC clock change to high
	// BPLL_PRE(1/2:30),DPHY(0:8) => Fixed
	// DMC(1/2:27), DMC_PHY(1/2:23),  DMC_PRE(1/1:19), COREP(1/2:15), CORED(1/2:11), MPLL_PRE(1/1:8)
	uBits = (1 << 27) | (1 << 23) | (0 << 19) | (1 << 15) | (1 << 11) | (0 << 8);		// DMC Up to Working Clock
	Outp32(rCLK_DIV_DMC1, uBits);	// 0x105C0504

	if (uMemClk <= 667)
	{
		//;DPHY clock gating for DLL lock
		//&uBits=(0.<<3.)|(0.<<2.)|(3.<<0.);	; [3:2]SCLK_DPHY0,1 clock gating,[1:0]RSVD 0x3
		//GOSUB write 0x105C0800 &uBits  ;"SCLK_DPHY0,1 gating"
		//uBits=(0<<3)|(0<<2)|(3<<0);      // [3:2]SCLK_DPHY0,1 clock gating,[1:0]RSVD 0x3
		//Outp32(rCLK_GATE_SCLK_DMC, uBits);	// 0x105C0504

		//;BPLL -> MPLL mux change
		//GOSUB read 0x105C0300
		//ENTRY &clk_src_dmc
		//&temp=&clk_src_dmc&(~(0x1<<4)) ; [4]0:SCLKMPLL_IN, 1: SCLKBPLL_IN
		uTemp= Inp32(rCLK_SRC_DMC);
		uBits=uTemp &(~(0x1<<4)) ;  //[4]0:SCLKMPLL_IN, 1: SCLKBPLL_IN
		Outp32(rCLK_SRC_DMC, uBits);	// 0x105C0504

		if (uMemClk==667)
		{
			//; ENABLE(1:31), MDIV(m:16), PDIV(p:8), SDIV(s:0)
			uBits=(1<<31)|(667<<16)|(12<<8)|(0<<0);	//; BPLL=1334MHz(12:667:0), for DMC,DREX PHY
		}
		else if (uMemClk==633)
		{
			//; ENABLE(1:31), MDIV(m:16), PDIV(p:8), SDIV(s:0)
			uBits=(1<<31)|(211<<16)|(4<<8)|(0<<0);	//; BPLL=1266MHz(4:211:0), for DMC,DREX PHY
		}
		else if (uMemClk==543)
		{
			//; ENABLE(1:31), MDIV(m:16), PDIV(p:8), SDIV(s:0)
			//&uBits=(1.<<31.)|(362.<<16.)|(4.<<8.)|(1.<<0.);	; BPLL=1086MHz(4:362:1), for DMC,DREX PHY
			uBits=(1<<31)|(362<<16)|(4<<8)|(1<<0);	//; BPLL=1086MHz(4:362:1), for DMC,DREX PHY
		}
		else if (uMemClk==480)
		{
			//; ENABLE(1:31), MDIV(m:16), PDIV(p:8), SDIV(s:0)
			//&uBits=(1.<<31.)|(320.<<16.)|(4.<<8.)|(1.<<0.);	; BPLL=960MHz(4:320:1) for DMC,DREX PHY
			uBits=(1<<31)|(320<<16)|(4<<8)|(1<<0);	//; BPLL=960MHz(4:320:1) for DMC,DREX PHY
		}
		else if (uMemClk==410)
		{
			//; ENABLE(1:31), MDIV(m:16), PDIV(p:8), SDIV(s:0)
			//&uBits=(1.<<31.)|(205.<<16.)|(3.<<8.)|(1.<<0.);	; BPLL=820MHz(3:205:1) for DMC,DREX PHY
			uBits=(1<<31)|(205<<16)|(3<<8)|(1<<0);	//; BPLL=820MHz(3:205:1) for DMC,DREX PHY
		}
		else if (uMemClk==275)
		{
			//; ENABLE(1:31), MDIV(m:16), PDIV(p:8), SDIV(s:0)
			//&uBits=(1.<<31.)|(275.<<16.)|(3.<<8.)|(2.<<0.); ; BPLL=550MHz(3:275:2) for DMC,DREX PHY
			uBits=(1<<31)|(275<<16)|(3<<8)|(2<<0); // ; BPLL=550MHz(3:275:2) for DMC,DREX PHY
		}
		else if (uMemClk==206)
		{
			//; ENABLE(1:31), MDIV(m:16), PDIV(p:8), SDIV(s:0)
			//&uBits=(1.<<31.)|(206.<<16.)|(3.<<8.)|(2.<<0.); ; BPLL=412MHz(3:206:2) for DMC,DREX PHY
			uBits=(1<<31)|(206<<16)|(3<<8)|(2<<0); // ; BPLL=412MHz(3:206:2) for DMC,DREX PHY
		}
		else if (uMemClk==165)
		{
			//; ENABLE(1:31), MDIV(m:16), PDIV(p:8), SDIV(s:0)
			//&uBits=(1.<<31.)|(220.<<16.)|(4.<<8.)|(2.<<0.);	; BPLL=330MHz(4:220:2) for DMC,DREX PHY
			uBits=(1<<31)|(220<<16)|(4<<8)|(2<<0);	// ; BPLL=330MHz(4:220:2) for DMC,DREX PHY
		}
		else if (uMemClk==138)
		{
			//; ENABLE(1:31), MDIV(m:16), PDIV(p:8), SDIV(s:0)
			//&uBits=(1.<<31.)|(368.<<16.)|(4.<<8.)|(3.<<0.);	; BPLL=276MHz(4:368:3) for DMC,DREX PHY
			uBits=(1<<31)|(368<<16)|(4<<8)|(3<<0);//	; BPLL=276MHz(4:368:3) for DMC,DREX PHY
		}
		else
		{
			//; ENABLE(1:31), MDIV(m:16), PDIV(p:8), SDIV(s:0)
			//&uBits=(1.<<31.)|(368.<<16.)|(4.<<8.)|(3.<<0.);	; BPLL=276MHz(4:368:3) for DMC,DREX PHY
			uBits=(1<<31)|(368<<16)|(4<<8)|(3<<0);//	; BPLL=276MHz(4:368:3) for DMC,DREX PHY
		}

		//GOSUB write 0x105C0218 &uBits ;"rBPLL_CON0"
		//GOSUB Waiting 0.3s
		Outp32(rBPLL_CON0,uBits);
		CMU_Delay(0x30000);

		//;MPLL -> BPLL mux change
		//GOSUB read 0x105C0300
		//ENTRY &clk_src_dmc
		//&temp=&clk_src_dmc|((0x1<<4)) ; [4]0:SCLKMPLL_IN, 1: SCLKBPLL_IN
		uTemp= Inp32(rCLK_SRC_DMC);
		uBits=uTemp |((0x1<<4)) ;  //[4]0:SCLKMPLL_IN, 1: SCLKBPLL_IN
		Outp32(rCLK_SRC_DMC, uBits);	// 0x105C0504

	}

}

void system_clock_init(void)
{
	CMU_Init(800);
	return;
}

