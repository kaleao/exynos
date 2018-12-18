/*
 * Memory setup for EXYNOS7420
 *
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

#include <config.h>
#include <asm/io.h>
#include <asm/arch/dmc.h>
#include <asm/arch/clock.h>
#include <asm/arch/power.h>
#include <asm/arch/cpu.h>
#include <asm/arch/smc.h>

#include "dmc/dmc.h"
#include "dmc/units.h"
#include "dmc/lpddr4.h"

typedef volatile unsigned char   		u8v;		//-  8-bit unsigned integer
typedef volatile unsigned short  		u16v;		//- 16-bit unsigned integer
typedef volatile unsigned int   		u32v;		//- 32-bit unsigned integer
typedef volatile unsigned long long		u64v;		//- 32-bit unsigned integer

#define __REG8(x)						(*((u8v *)((u64)x)))
#define __REG16(x)						(*((u16v *)((u64)x)))
#define __REG32(x)						(*((u32v *)((u64)x)))
#define __REG64(x)						(*((u64v *)((u64)x)))

#define GET_MAIN_SUB_REV()		(__REG32(0x10000000) & 0x000000FF)		// [7:4] MainRev, [3:0] SubRev

static lp4_mr13_op_t __mr13;
static lp4_fsp_t __fsp[2];

#undef msec
#define msec (800)

#define WRITE_TRAINING_FINE 1

#ifndef CONFIG_SG_BINARY
#define TMRS_MR4X2 1
#else
#define TMRS_MR4X2 0
#endif

/******************************************************************************
 *
 * @fn		__waitf
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static void __waitf(int ch, float sec)
{

#if 1
	float elapsed;
	unsigned int a, b;


	DmcDriver.ppc.init( ch , 1 );

	a = DmcDriver.ppc.ccnt( ch );

	elapsed = 0;
	while( elapsed < sec ) {

		b = DmcDriver.ppc.ccnt( ch );
		elapsed = ( b - a ) / (float)800000000.0;				//TEMP:
	}
	DmcDriver.ppc.init( ch , 0 );
#endif
}

/******************************************************************************
 *
 * @fn		__wait
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static void __wait(int ch, unsigned int cnt)
{

	unsigned int elapsed;
	unsigned int a, b;

	DmcDriver.ppc.init( ch , 1 );

	a = DmcDriver.ppc.ccnt( ch );

	elapsed = 0;
	while( elapsed < cnt ) {

		b = DmcDriver.ppc.ccnt( ch );
		elapsed = b - a;
	}
	DmcDriver.ppc.init( ch , 0 );
}

/******************************************************************************
 *
 * @fn		dram_unreset
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static int dram_unreset(int ch, int resume)
{
//- ISTOR EVT0 W/A
#if 1
	int channel;

	if (ch!=0) {
		return 1;
	}

	if ( resume == 1 ) {
		for( channel = 0; channel < 4; channel++ ) {
			DmcDriver.dfi.mode( channel, 0, 1 );
		}
		return 1;
	}

	channel = 0;
	DmcDriver.init.reset( channel, 0 );
	__wait( channel, 1 * msec);

	for( channel = 0; channel < 4; channel++ ) {

		DmcDriver.dfi.mode( channel, 0, 1 );
		DmcDriver.command.pde( channel, -1 );
		//DmcDriver.command( DMC_PDE_COMMAND, channel, -1 );
	}
	channel = 0;
	__wait( channel, 1 * msec);


	// FIXME: wait 10ns
	DmcDriver.init.reset( channel, 1 );
	__wait( channel, 1 * msec);

#endif

	return 0;
}

/******************************************************************************
*
* @fn		dram_init
*
* @brief
*
* @param	none.
*
* @return	true or false.
*
*****************************************************************************/
static int dram_preset(int ch)
{
#ifdef __KERNEL__
	unsigned int cal_get_mif;
#endif

#if 1 /* ori */
	//DmcDriver.command( DMC_PDX_COMMAND, ch, DMC_RANK_ALL );
	DmcDriver.command.pdx( ch, DMC_RANK_ALL );
	__wait( ch, 1 * msec);

	//DmcDriver.command( DMC_MRW_COMMAND, ch, DMC_RANK_ALL, 3, 0xf1 );			// IO (PU, PD)
	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 3, 0xf1 );			// IO (PU, PD)
	__wait( ch, 1 * msec);

	//DmcDriver.command( DMC_MRW_COMMAND, ch, DMC_RANK_ALL, 11, 0x01 );			// ODT
	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 11, 0x00 );			// ODT
	__wait( ch, 1 * msec);

	//DmcDriver.command( DMC_MRW_COMMAND, ch, DMC_RANK_ALL, 22, 0x26 );			// SOC ODT : [5] ODTD-CA disable
	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 22, 0x26 );			// SOC ODT : [5] ODTD-CA disable
	__wait( ch, 1 * msec);

	//DmcDriver.command( DMC_MPC_COMMAND, ch, DMC_RANK_0, LPDDR4_MPC1_ZQCAL_START );
	DmcDriver.command.mpc( ch, DMC_RANK_0, LPDDR4_MPC1_ZQCAL_START );
	__wait( ch, 1 * msec);

	//DmcDriver.command( DMC_MPC_COMMAND, ch, DMC_RANK_0, LPDDR4_MPC1_ZQCAL_LATCH );
	DmcDriver.command.mpc( ch, DMC_RANK_0, LPDDR4_MPC1_ZQCAL_LATCH );
	__wait( ch, 1 * msec);

	//DmcDriver.command( DMC_MPC_COMMAND, ch, DMC_RANK_1, LPDDR4_MPC1_ZQCAL_START );
	DmcDriver.command.mpc( ch, DMC_RANK_1, LPDDR4_MPC1_ZQCAL_START );
	__wait( ch, 1 * msec);

	//DmcDriver.command( DMC_MPC_COMMAND, ch, DMC_RANK_1, LPDDR4_MPC1_ZQCAL_LATCH );
	DmcDriver.command.mpc( ch, DMC_RANK_1, LPDDR4_MPC1_ZQCAL_LATCH );
	__wait( ch, 1 * msec);


/* fsp-1 */
	//DmcDriver.command( DMC_MRW_COMMAND, ch, DMC_RANK_ALL, 13, 0x40 );			// FSP[0]
	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 13, 0x40 );			// FSP[1]
	__wait( ch, 1 * msec);

	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 1, 0x26 );		// 632
	//DmcDriver.command( DMC_MRW_COMMAND, ch, DMC_RANK_ALL, 1, 0x26 );			// 1150
	//DmcDriver.command.mrw( ch, DMC_RANK_ALL, 1, 0x42 );			// 1150
	__wait( ch, 1 * msec);

#ifdef __KERNEL__
	cal_get_mif = _cal_get_mif();
	if ((MEMCLK_1552 == cal_get_mif) || (MEMCLK_1456 == cal_get_mif)) {
		DmcDriver.command.mrw( ch, DMC_RANK_ALL, 2, 0x2d );
	} else if (MEMCLK_1264 == cal_get_mif) {
		DmcDriver.command.mrw( ch, DMC_RANK_ALL, 2, 0x24 );
	}
#else
#if (MEMCLK == 1552)
	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 2, 0x2d );
#elif (MEMCLK == 1456)
	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 2, 0x2d );
#elif (MEMCLK == 1264)
	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 2, 0x24 );
#elif (MEMCLK == 1068)
	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 2, 0x24 );
#elif (MEMCLK == 828)
	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 2, 0x1B );
#elif (MEMCLK == 632)
	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 2, 0x12 );
#else
	;
#endif
#endif

	//DmcDriver.command( DMC_MRW_COMMAND, ch, DMC_RANK_ALL, 2, 0x2d );			//1150
	//DmcDriver.command.mrw( ch, DMC_RANK_ALL, 2, 0x24 );			//1150
	__wait( ch, 1 * msec);

	//DmcDriver.command( DMC_MRW_COMMAND, ch, DMC_RANK_ALL, 12, 0x5d );
	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 12, 0x5d );			// VREF-CA
	__wait( ch, 1 * msec);

	//DmcDriver.command( DMC_MRW_COMMAND, ch, DMC_RANK_ALL, 14, 0x44 );			// VREF-DQ
	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 14, 0x17 );			// VREF-DQ 19.2%
	__wait( ch, 1 * msec);

	//DmcDriver.command( DMC_MRW_COMMAND, ch, DMC_RANK_ALL, 3, 0xf1 );			// IO (PU, PD)
	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 3, 0xf1 );			// IO (PU, PD)
	__wait( ch, 1 * msec);

	//DmcDriver.command( DMC_MRW_COMMAND, ch, DMC_RANK_ALL, 11, 0x01 );			// ODT
	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 11, 0x04 );			// ODT 0x1 = CA ODT off, DQ ODT 60ohm
	__wait( ch, 1 * msec);

	//DmcDriver.command( DMC_MRW_COMMAND, ch, DMC_RANK_ALL, 22, 0x26 );			// SOC ODT
	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 22, 0x26 );			// SOC ODT
	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 4, 0x40 );			// MR4 thermal offset 10


/* fsp-0 */
	//DmcDriver.command( DMC_MRW_COMMAND, ch, DMC_RANK_ALL, 13, 0x00 );			// FSP[0]
	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 13, 0x00 );			// FSP[0]
	__wait( ch, 1 * msec);

	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 1, 0x26 );		// 632
	//DmcDriver.command( DMC_MRW_COMMAND, ch, DMC_RANK_ALL, 1, 0x26 );			// 1150
	//DmcDriver.command.mrw( ch, DMC_RANK_ALL, 1, 0x42 );			// 1150
	__wait( ch, 1 * msec);

#ifdef __KERNEL__
	if ((MEMCLK_1552 == cal_get_mif) || (MEMCLK_1456 == cal_get_mif)) {
		DmcDriver.command.mrw( ch, DMC_RANK_ALL, 2, 0x2d );
	} else if (MEMCLK_1264 == cal_get_mif) {
		DmcDriver.command.mrw( ch, DMC_RANK_ALL, 2, 0x24 );
	}
#else
#if (MEMCLK == 1552)
	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 2, 0x2d );
#elif (MEMCLK == 1456)
	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 2, 0x2d );
#elif (MEMCLK == 1264)
	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 2, 0x24 );
#elif (MEMCLK == 1068)
	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 2, 0x24 );
#elif (MEMCLK == 828)
	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 2, 0x1B );
#elif (MEMCLK == 632)
	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 2, 0x12 );
#else
	;
#endif
#endif

	//DmcDriver.command( DMC_MRW_COMMAND, ch, DMC_RANK_ALL, 2, 0x2d );			//1150
	//DmcDriver.command.mrw( ch, DMC_RANK_ALL, 2, 0x24 );			//1150
	__wait( ch, 1 * msec);

	//DmcDriver.command( DMC_MRW_COMMAND, ch, DMC_RANK_ALL, 12, 0x5d );
	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 12, 0x5d );			// VREF-CA
	__wait( ch, 1 * msec);

	//DmcDriver.command( DMC_MRW_COMMAND, ch, DMC_RANK_ALL, 14, 0x44 );			// VREF-DQ
	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 14, 0x17 );			// VREF-DQ 19.2%
	__wait( ch, 1 * msec);

	//DmcDriver.command( DMC_MRW_COMMAND, ch, DMC_RANK_ALL, 3, 0xf1 );			// IO (PU, PD)
	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 3, 0xf1 );			// IO (PU, PD)
	__wait( ch, 1 * msec);

	//DmcDriver.command( DMC_MRW_COMMAND, ch, DMC_RANK_ALL, 11, 0x01 );			// ODT
	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 11, 0x04 );			// ODT 0x1 = CA ODT off, DQ ODT 60ohm
	__wait( ch, 1 * msec);

	//DmcDriver.command( DMC_MRW_COMMAND, ch, DMC_RANK_ALL, 22, 0x26 );			// SOC ODT
	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 22, 0x26 );			// SOC ODT
	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 4, 0x40 );			// MR4 thermal offset 10
#else /* ori */
	DmcDriver.command( DMC_PDX_COMMAND, ch, DMC_RANK_ALL );
	//DmcDriver.command.pdx( ch, DMC_RANK_ALL );
	__wait( ch, 1 * msec);

	DmcDriver.command( DMC_MRW_COMMAND, ch, DMC_RANK_ALL, 3, 0xf1 );			// IO (PU, PD)
	//DmcDriver.command.mrw( ch, DMC_RANK_ALL, 3, 0xf1 );			// IO (PU, PD)
	__wait( ch, 1 * msec);

	DmcDriver.command( DMC_MRW_COMMAND, ch, DMC_RANK_ALL, 11, 0x01 );			// ODT
	//DmcDriver.command.mrw( ch, DMC_RANK_ALL, 11, 0x00 );			// ODT
	__wait( ch, 1 * msec);

	DmcDriver.command( DMC_MRW_COMMAND, ch, DMC_RANK_ALL, 22, 0x20 );			// SOC ODT : [5] ODTD-CA disable
	//DmcDriver.command.mrw( ch, DMC_RANK_ALL, 22, 0x20 );			// SOC ODT : [5] ODTD-CA disable
	__wait( ch, 1 * msec);

	DmcDriver.command( DMC_MPC_COMMAND, ch, DMC_RANK_ALL, LPDDR4_MPC1_ZQCAL_START );
	//DmcDriver.command.mpc( ch, DMC_RANK_ALL, LPDDR4_MPC1_ZQCAL_START );
	__wait( ch, 1 * msec);

	DmcDriver.command( DMC_MPC_COMMAND, ch, DMC_RANK_ALL, LPDDR4_MPC1_ZQCAL_LATCH );
	//DmcDriver.command.mpc( ch, DMC_RANK_ALL, LPDDR4_MPC1_ZQCAL_LATCH );
	__wait( ch, 1 * msec);

#if 1

	//DmcDriver.command.mrw( ch, DMC_RANK_ALL, 13, 0x40 );			// FSP[1]
	//__wait( ch, 1 * msec);

	//DmcDriver.command.mrw( ch, DMC_RANK_ALL, 1, 0x22 );		// 632
	DmcDriver.command( DMC_MRW_COMMAND, ch, DMC_RANK_ALL, 1, 0x26 );			// 1150
	//DmcDriver.command.mrw( ch, DMC_RANK_ALL, 1, 0x42 );			// 1150
	__wait( ch, 1 * msec);

	//DmcDriver.command.mrw( ch, DMC_RANK_ALL, 2, 0x12 );		//632
	DmcDriver.command( DMC_MRW_COMMAND, ch, DMC_RANK_ALL, 2, 0x2d );			//1150
	//DmcDriver.command.mrw( ch, DMC_RANK_ALL, 2, 0x24 );			//1150
	__wait( ch, 1 * msec);

	DmcDriver.command( DMC_MRW_COMMAND, ch, DMC_RANK_ALL, 12, 0x5d );
	//DmcDriver.command.mrw( ch, DMC_RANK_ALL, 12, 0x5d );			// VREF-CA
	__wait( ch, 1 * msec);

	DmcDriver.command( DMC_MRW_COMMAND, ch, DMC_RANK_ALL, 14, 0x44 );			// VREF-DQ
	//DmcDriver.command.mrw( ch, DMC_RANK_ALL, 14, 0x4d );			// VREF-DQ
	__wait( ch, 1 * msec);

	DmcDriver.command( DMC_MRW_COMMAND, ch, DMC_RANK_ALL, 3, 0xf1 );			// IO (PU, PD)
	//DmcDriver.command.mrw( ch, DMC_RANK_ALL, 3, 0xf1 );			// IO (PU, PD)
	__wait( ch, 1 * msec);

	DmcDriver.command( DMC_MRW_COMMAND, ch, DMC_RANK_ALL, 11, 0x01 );			// ODT
	//DmcDriver.command.mrw( ch, DMC_RANK_ALL, 11, 0x00 );			// ODT
	__wait( ch, 1 * msec);

	DmcDriver.command( DMC_MRW_COMMAND, ch, DMC_RANK_ALL, 22, 0x20 );			// SOC ODT
	//DmcDriver.command.mrw( ch, DMC_RANK_ALL, 22, 0x20 );			// SOC ODT


	DmcDriver.command( DMC_MRW_COMMAND, ch, DMC_RANK_ALL, 13, 0x00 );			// FSP[0]
	//DmcDriver.command.mrw( ch, DMC_RANK_ALL, 13, 0x00 );			// FSP[0]
	__wait( ch, 1 * msec);

	//DmcDriver.command.mrw( ch, DMC_RANK_ALL, 13, 0xc0 );			// FSP
	//__wait( ch, 1 * msec);

#else
	freqSetPoint[modeRegister13.FSP_WR].MR3.PU_CAL = 1;
	freqSetPoint[modeRegister13.FSP_WR].MR3.PDDS = 6;
	freqSetPoint[modeRegister13.FSP_WR].MR3.DBI_RD = 1;
	freqSetPoint[modeRegister13.FSP_WR].MR3.DBI_WR = 1;

	DmcDriver.command.mrw( ch, -1, 3, *((unsigned char *)&freqSetPoint[0].MR3) );
	__wait( ch, 1 * msec);

	freqSetPoint[modeRegister13.FSP_WR].MR22.CODT = 0;
	freqSetPoint[modeRegister13.FSP_WR].MR22.ODTE_CK = 0;
	freqSetPoint[modeRegister13.FSP_WR].MR22.ODTE_CS = 0;
	freqSetPoint[modeRegister13.FSP_WR].MR22.ODTD_CA = 0;

	DmcDriver.command.mrw( ch, -1, 22, *((unsigned char *)&freqSetPoint[0].MR22) );
	__wait( ch, 1 * msec);


	DmcDriver.command.mpc( ch, -1, LPDDR4_MPC1_ZQCAL_START );
	__wait( ch, 1 * msec);
	DmcDriver.command.mpc( ch, -1, LPDDR4_MPC1_ZQCAL_LATCH );
	__wait( ch, 1 * msec);

	modeRegister13.CBT = 0;
	modeRegister13.VRO = 0;
	modeRegister13.VRCG = 0;
	modeRegister13.DM_DIS = 0;
	modeRegister13.FSP_WR = 1;
	modeRegister13.FSP_OP = 0;

	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 13, *( (unsigned char *)&modeRegister13 ) );
	//__wait( ch, 1 * msec);

	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 1, 0x42 );				// 1150
	//__wait( ch, 1 * msec);

	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 2, 0x24 );				//11150
	//__wait( ch, 1 * msec);

	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 11, 0x01 );
	//__wait( ch, 1 * msec);

	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 12, 0x5d );			// VREF-CA
	//__wait( ch, 1 * msec);

	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 14, 0x4d );			// VREF-DQ
	//__wait( ch, 1 * msec);

	modeRegister13.CBT = 0;
	modeRegister13.VRO = 0;
	modeRegister13.VRCG = 0;
	modeRegister13.DM_DIS = 0;
	modeRegister13.FSP_WR = 1;
	modeRegister13.FSP_OP = 1;
	DmcDriver.command.mrw( ch, -1, 13, *( (unsigned char *)&modeRegister13 ) );
	__wait( ch, 1 * msec);

#endif

#endif /* ori */

	return 0;
}

/******************************************************************************
 *
 * @fn		soc_preset
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static int soc_preset(int ch)
{
	DmcDriver.init.preset( ch );

	return 0;
}

static int io_preset(int ch)
{
	DmcDriver.io.odt( ch, 2, 1 );

	return 0;
}

/******************************************************************************
 *
 * @fn		soc_zq_calibration
 *
 * @brief
 *
 * @author	seongjae.moon@samsung.com
 *
 * @date
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static int soc_zq_calibration(int ch)
{
	if( ( ch == 0 ) || ( ch == 3 ) ) {
		DmcDriver.io.cal( ch );
	}

	return 0;
}

/******************************************************************************
 *
 * @fn		calibration_preset
 *
 * @brief
 *
 * @author	seongjae.moon@samsung.com
 *
 * @date
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static int calibration_preset(int ch)
{
//	DmcDriver.cal.preset( ch );

	//DmcDriver.command( DMC_MRW_COMMAND, ch, DMC_RANK_ALL, 32, (unsigned char)0x55 );
	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 32, (unsigned char)0x55 );
	__wait( ch, 1 * msec);
	//DmcDriver.command( DMC_MRW_COMMAND, ch, DMC_RANK_ALL, 15, (unsigned char)0x55 );
	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 15, (unsigned char)0x55 );
	__wait( ch, 1 * msec);
	//DmcDriver.command( DMC_MRW_COMMAND, ch, DMC_RANK_ALL, 40, (unsigned char)0x55 );
	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 40, (unsigned char)0x55 );
	__wait( ch, 1 * msec);
	//DmcDriver.command( DMC_MRW_COMMAND, ch, DMC_RANK_ALL, 20, (unsigned char)0x55 );
	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 20, (unsigned char)0x55 );
	__wait( ch, 1 * msec);

	return 0;
}

/******************************************************************************
 *
 * @fn		calibration_preset
 *
 * @brief
 *
 * @author	seongjae.moon@samsung.com
 *
 * @date
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static int calibration_preset_gate_train_evt0(int ch)
{
//	DmcDriver.cal.preset( ch );

	//DmcDriver.command( DMC_MRW_COMMAND, ch, DMC_RANK_ALL, 32, (unsigned char)0x55 );
	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 32, (unsigned char)0xFF );
	__wait( ch, 1 * msec);
	//DmcDriver.command( DMC_MRW_COMMAND, ch, DMC_RANK_ALL, 15, (unsigned char)0x55 );
	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 15, (unsigned char)0x55 );
	__wait( ch, 1 * msec);
	//DmcDriver.command( DMC_MRW_COMMAND, ch, DMC_RANK_ALL, 40, (unsigned char)0x55 );
	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 40, (unsigned char)0x5F );
	__wait( ch, 1 * msec);
	//DmcDriver.command( DMC_MRW_COMMAND, ch, DMC_RANK_ALL, 20, (unsigned char)0x55 );
	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 20, (unsigned char)0x55 );
	__wait( ch, 1 * msec);

	return 0;
}

/******************************************************************************
 *
 * @fn		command_bus_training
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
#if 0
static int command_bus_training(int ch, int vref, int(*clock)(int *), int freq)
{
	//DmcDriver.command( DMC_MRW_COMMAND, ch, DMC_RANK_ALL, 13, 0x9 );
	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 13, 0x9 );

	DmcDriver.cal.command_bus( ch, clock, freq );

	//DmcDriver.command( DMC_MRW_COMMAND, ch, DMC_RANK_ALL, 13, 0x0 );
	DmcDriver.command.mrw( ch, DMC_RANK_ALL, 13, 0x0 );

	return 0;
}
#endif

/******************************************************************************
 *
 * @fn		read_dq_calibration
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
#if (DMC_DQS_TYPE == DMC_DQS_MODE1)
static int gate_training(int ch, int rank)
{
	DmcDriver.cal.gate( ch, DMC_RANK_ALL );

	return 0;
}
#endif

/******************************************************************************
 *
 * @fn		read_dq_calibration
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static int read_dq_calibration(int ch, int rank)
{
	DmcDriver.cal.read_dq( ch, rank );

	return 0;
}

/******************************************************************************
 *
 * @fn		write_dq_calibration
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static int write_dq_calibration(int ch, int rank)
{
	DmcDriver.cal.write_dq( ch, rank );

	return 0;
}

void enable_pause_mif(void)
{
	__REG32( 0x10571008  ) |= 0x1;
}


void enable_root_clk_gating(void)
{
	__REG32(0x10050714)=0xffffffff;
	__REG32(0x1005071c)=0x3;
	__REG32(0x10050724)=0x3;

	__REG32(0x10051818)=0x1;
	__REG32(0x10051824)=0x3;
	__REG32(0x10051828)=0x777;

	__REG32(0x10051918)=0x1;
	__REG32(0x10051924)=0x3;
	__REG32(0x10051928)=0x777;

	__REG32(0x10052018)=0x1;
	__REG32(0x10052024)=0x3;
	__REG32(0x10052028)=0x777;

	__REG32(0x10052118)=0x1;
	__REG32(0x10052124)=0x3;
	__REG32(0x10052128)=0x777;
}

void sel_timing_set_source(unsigned int set)
{
	if (set == 1) {
		__REG32( 0x108000e0 ) |= 0x00000001;
		__REG32( 0x109000e0 ) |= 0x00000001;
		__REG32( 0x10a000e0 ) |= 0x00000001;
		__REG32( 0x10b000e0 ) |= 0x00000001;
	} else {
		__REG32( 0x108000e0 ) &= 0xFFFFFFFE;
		__REG32( 0x109000e0 ) &= 0xFFFFFFFE;
		__REG32( 0x10a000e0 ) &= 0xFFFFFFFE;
		__REG32( 0x10b000e0 ) &= 0xFFFFFFFE;
	}
}

/******************************************************************************
 *
 * @fn		get_training_result
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static void *get_training_result(int training, ...)
{
#if 0
	va_list argptr;

	va_start( argptr, training );

	ch = va_arg(argptr, int);
	rank = va_arg(argptr, int);

	bank_bf = addr_bf = ma = op = 0;
	switch( command ) {

		case DREX_MRW_COMMAND:
			ma = va_arg(argptr, unsigned char);
			op = va_arg(argptr, unsigned char);

			bank_bf 	  = ( ma >> 3 ) & 0x7;
			addr_bf_12_10 = ma & 0x7;
			addr_bf_1_0   = ma >> 6;
			addr_bf_9_2   = op;

			addr_bf = 0;
			addr_bf = ( addr_bf_12_10 << 10 ) | ( addr_bf_9_2 << 2 ) | ( addr_bf_1_0 );
			break;

		case DREX_MPC_COMMAND:
			op = va_arg(argptr, unsigned char);

			addr_bf = ( op & 0xff ) << 2;
			break;

		case DREX_MRR_COMMAND:
			ma = va_arg(argptr, unsigned char);

			bank_bf 	  = ( ma >> 3 ) & 0x7;
			addr_bf_12_10 = ma & 0x7;
			addr_bf_1_0   = ma >> 6;

			addr_bf = 0;
			addr_bf = ( addr_bf_12_10 << 10 ) | ( addr_bf_1_0 );
			break;
	}
#endif

	return 0;
}
/******************************************************************************
 *
 * @fn		memory_init_done
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static int memory_init_done(int ch, int resume)
{
	if( resume == 1 ) {
		//DmcDriver.command( DMC_SRX_COMMAND, ch, DMC_RANK_0 );
		DmcDriver.command.srx( ch, DMC_RANK_0 );
		//DmcDriver.command( DMC_SRX_COMMAND, ch, DMC_RANK_1 );
		DmcDriver.command.srx( ch, DMC_RANK_1 );
	}
	DmcDriver.init.done( ch );

	return 0;
}

/******************************************************************************
 *
 * @fn		dmc_driver_freq
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static int change_freq(int mega_hertz)
{
#if 0
	__REG32( 0x10850200 ) = ( 1 << 16 ) | ( 1 << 12 ) | ( 2 << 8 ) | ( 3 << 4 ) | ( 0 << 0 );
	__REG32( 0x10950200 ) = ( 1 << 16 ) | ( 1 << 12 ) | ( 2 << 8 ) | ( 3 << 4 ) | ( 0 << 0 );
	__REG32( 0x10a50200 ) = ( 1 << 16 ) | ( 1 << 12 ) | ( 2 << 8 ) | ( 3 << 4 ) | ( 0 << 0 );
	__REG32( 0x10b50200 ) = ( 1 << 16 ) | ( 1 << 12 ) | ( 2 << 8 ) | ( 3 << 4 ) | ( 0 << 0 );

	__REG32( 0x10570214 ) = ( 1 << 20 ) | ( 1 << 16 ) | ( 0 << 12 ) | ( 0 << 8 ) | ( 0 << 0 );
#else
	unsigned int tmp;

	__REG32( 0x10850200 ) = (1<<16) | __REG32( 0x10850200 ); //CLK_MUX_SEL_MIF0	pause=0
	__REG32( 0x10950200 ) = (1<<16) | __REG32( 0x10950200 ); //CLK_MUX_SEL_MIF1	pause=0
	__REG32( 0x10a50200 ) = (1<<16) | __REG32( 0x10a50200 ); //CLK_MUX_SEL_MIF2	pause=0
	__REG32( 0x10b50200 ) = (1<<16) | __REG32( 0x10b50200 ); //CLK_MUX_SEL_MIF3	pause=0
	__REG32( 0x10570214 ) = (1<<20) | __REG32( 0x10570214 ); //CLK_MUX_SEL_TOPC5	pause=1
	while(0x00000000 != (__REG32(0x10570414)&0x0F000000));// "CLK_MUX_STAT_TOPC5

	//mif switching pll changes
	//800/(7+1) = 100MHz (low freq init) -> 1600/(0+1)=1600MHz (for dvfs switching)

	tmp = __REG32( 0x10570204 ); //CLK_MUX_SEL_TOPC1
	tmp &= ~(0x3<<12);			 //SCLK_BUS0_PLL_MIF=1600MHz
	__REG32( 0x10570204 ) = tmp;

	tmp = __REG32( 0x1057060C ); //CLK_DIV_TOPC3
	tmp &= ~(0x7<<4);			 //SCLK_BUS0_PLL_MIF ratio = 0+1 (bypass)
	__REG32( 0x1057060C ) = tmp;
#endif

	return 0;
}

static void set_vt_mon(unsigned int ch, unsigned int dqs_osc_en, unsigned int aref_en)
{
	unsigned int DREX_BASE = 0;
	unsigned int VTMON_BASE = 0;
	unsigned int PHY_BASE = 0;

	unsigned int OSC_STOP_CYCLE=0x20;	// ~2.56us @ 200MHz
	unsigned int MPC_to_MRR=0x48;  		// ~3us
	unsigned int MPC_to_MPC=0x60;  		// ~3.98us

	unsigned int AREF_INTERVAL_CNT=1000;
	unsigned int AREF_UPDATE_EN=1;
	unsigned int PER_MES_EN=1;

	if (ch==0) {
		DREX_BASE = 0x10800000;
		VTMON_BASE = 0x10890000;
		PHY_BASE = 0x10820000;
	} else if (ch==1) {
		DREX_BASE = 0x10900000;
		VTMON_BASE = 0x10990000;
		PHY_BASE = 0x10920000;
	} else if (ch==2) {
		DREX_BASE = 0x10a00000;
		VTMON_BASE = 0x10a90000;
		PHY_BASE = 0x10a20000;
	} else if (ch==3) {
		DREX_BASE = 0x10b00000;
		VTMON_BASE = 0x10b90000;
		PHY_BASE = 0x10b20000;
	}

	/*; set DRAM cnt stop timeout
	d.s SD:&DREX_BASE+0x10 %LE %LONG (0x00221c00)|(&OSC_STOP_CYCLE<<2.)  ; cycle 16
	wait 0.1s
	*/
	__REG32( DREX_BASE+0x10 ) = (0x00221c00)|(OSC_STOP_CYCLE<<2) ;

	/*; EVT0 - set VT_MON run cnt limit time (MPC->MRR)
	d.s SD:&VTMON_BASE+0x04 %LE %LONG (0x0)|&MPC_to_MRR
	wait 0.1s
	*/
	//__REG32( VTMON_BASE+0x04 ) = (0x0)|MPC_to_MRR;

	/*; EVT1 - set VT_MON run cnt limit time (MPC->MRR)
	;d.s SD:&VTMON_BASE+0x80 %LE %LONG 0x02002000
	;wait 0.1s
	*/
	//__REG32( VTMON_BASE+0x80 ) = 0x02002000;

	/*; set MRR18 for VT_MON - CS0
	d.s SD:&VTMON_BASE+0x24 %LE %LONG 0x09020800	 ;MR18
	wait 0.1s
	*/
	__REG32( VTMON_BASE+0x24) = 0x09020800;

	/*; set MRR19 for VT_MON - CS0
	d.s SD:&VTMON_BASE+0x28 %LE %LONG 0x09020C00  ; MR19
	wait 0.1s
	*/
	__REG32( VTMON_BASE+0x28 ) = 0x09020C00;

	/*; set MRR18 for VT_MON - CS1
	d.s SD:&VTMON_BASE+0x2C %LE %LONG 0x09120800	 ;MR18
	wait 0.1s
	*/
	__REG32( VTMON_BASE+0x2C) = 0x09120800;

	/*; set MRR19 for VT_MON - CS1
	d.s SD:&VTMON_BASE+0x30 %LE %LONG 0x09120C00  ; MR19
	wait 0.1s
	*/
	__REG32( VTMON_BASE+0x30) = 0x09120C00;

	/*; set MPC for VT_MON
	d.s SD:&VTMON_BASE+0x34 %LE %LONG 0x0320012C
	wait 0.1s
	*/
	__REG32( VTMON_BASE+0x34) = 0x0320012C;

	/*; MR4
	d.s SD:&VTMON_BASE+0x38 %LE %LONG 0x09001000
	wait 0.1s
	*/
	__REG32( VTMON_BASE+0x38) = 0x09001000;

	/*; Set 1xtREFI for VT_MON
	d.s SD:&VTMON_BASE+0x70 %LE %LONG 0x000B005D
	wait 0.1s
	*/
	__REG32( VTMON_BASE+0x70) = 0x000C005D;

	/* EVT0 - Aref_interval_cnt_limit
	    Set TimingAref SFR addr to VTMON
	d.s SD:&VTMON_BASE+0x6C %LE %LONG 0x00300001
	*/
	__REG32( VTMON_BASE+0x6C) = 0x00300000; // 0x00300001 ;

	/*; PHY- dqs_int_runtime
	GOSUB setReg &PHY_BASE+0x0600 0xFF &OSC_STOP_CYCLE
	*/
	__REG32( PHY_BASE+0x0600) &= ~(0xff);
	__REG32( PHY_BASE+0x0600) |= (OSC_STOP_CYCLE >> 1);

	/*; PHY- ctrl_dqs_osc_en
	GOSUB setReg &PHY_BASE+0x04 0x00080000 0x00080000
	*/
	// Cleared PER_MRS_EN bit for the reason of stability.
	__REG32( PHY_BASE+0x04) |= 0x00080000;

	/*; EVT1 - set VT_MON run cnt limit time (MPC->MRR)
	;d.s SD:&VTMON_BASE+0x80 %LE %LONG 0x02002000
	;wait 0.1s
	*/
	__REG32( VTMON_BASE+0x80 ) &= ~((0x3FFF<<16)|(0x3FFF<<0));
	__REG32( VTMON_BASE+0x80 ) |= (MPC_to_MRR<<16)|(MPC_to_MPC<<0);

	__REG32( VTMON_BASE+0x84 ) &= ~(0xFFFF);
	__REG32( VTMON_BASE+0x84 ) |= (AREF_INTERVAL_CNT);

	__REG32( VTMON_BASE+0x88 ) &= ~((0x3FFF<<16)|(0x3FFF<<0));
	__REG32( VTMON_BASE+0x88 ) |= (MPC_to_MRR<<16)|(MPC_to_MPC<<0);

	__REG32( VTMON_BASE+0x8C ) &= ~(0xFFFF);
	__REG32( VTMON_BASE+0x8C ) |= (AREF_INTERVAL_CNT);


	/*; Set VT_MON_CON (MPC->next MPC)
	; EVT0
	d.s SD:&VTMON_BASE+0x14 %LE %LONG (0x80000000)|(0x30000000)|(0x00070000)|(&MPC_to_MPC)|(1<<23.)
	*/
	__REG32( VTMON_BASE+0x14 ) = (0x80000000)|(0x20000000)|(0x00050000)|(1<<23)|(aref_en<<28)|(dqs_osc_en<<17);
}

#if TMRS_MR4X2
//--------------------------------------------------------
// EnterTMRS
//    - Function to enter TRMS Mode for LPDDR4
//    - Paramter : Channel number, rank Number
//
//					channel number : 0 ~ 4
//                  Rank number : -1, 0, 1
//
//    - Return : void
//--------------------------------------------------------

void EnterTMRS(int ch, dmc_rank_t rank)
{
    // Safty Key1
    DmcDriver.command.mrw( ch, rank, 9, 0x20 );

    __wait ( ch, 1*msec);

    // Safty Key2
    DmcDriver.command.mrw( ch, rank, 9, 0x20 );

    __wait ( ch, 1*msec);

    // Safty Key3
    DmcDriver.command.mrw( ch, rank, 9, 0x20 );

    __wait ( ch, 1*msec);

    // Safty Test
    DmcDriver.command.mrw( ch, rank, 9, 0xFF );

    __wait ( ch, 1*msec);

}

//--------------------------------------------------------
// Set_TMRS
//    - Function to set-up the TMRS
//    - Paramter : Channel number, Rank number
//		 			Main Set, Sub Set, Group Set
//
//					Channel number : 0 ~ 4
//                  Rank number : -1, 0, 1
//
//    - Return : void
//--------------------------------------------------------

void SetTMRS(int ch, dmc_rank_t rank, int main, int sub, int group)
{
    unsigned int ca4_to_2, op7_to_0;


    //// Main TMRS
    //	    -	MR = 9
    //      - 	C[8:10](CA[4:2]) = ca4_to_2
    //		    C[7:0](OP[7:0]) = op7_to_0
    ////
	switch(main)
	{
		case 8:
			ca4_to_2=0x10;
            op7_to_0=0x0;
			break;
		case 9:
			ca4_to_2=0x8;
            op7_to_0=0x0;
			break;
		case 10:
			ca4_to_2=0x4;
            op7_to_0=0x0;
			break;
        default:
            ca4_to_2=0x0;
            op7_to_0 = ((0x1 << main) & (0x0FF));
			break;
	}

    __REG32(0x108000F0+ch*0x100000) = ((0x00000)|(0x1<<1)|(ca4_to_2));

    DmcDriver.command.mrw( ch, rank, 9, op7_to_0);

    __wait ( ch, 1*msec);


    //// Sub TMRS
    //	    -	MR = 9
    //      - 	C[8:10](CA[4:2]) = ca4_to_2
    //		    C[7:0](OP[7:0]) = op7_to_0
    ////
	switch(sub)
	{
		case 8:
			ca4_to_2=0x10;
            op7_to_0=0x0;
			break;
		case 9:
			ca4_to_2=0x8;
            op7_to_0=0x0;
			break;
		case 10:
			ca4_to_2=0x4;
            op7_to_0=0x0;
			break;
        default:
            ca4_to_2=0x0;
            op7_to_0 = ((0x1 << sub) & (0x0FF));
			break;

	}

    __REG32(0x108000F0+ch*0x100000) = ((0x00000)|(0x1<<1)|(ca4_to_2));

    DmcDriver.command.mrw( ch, rank, 9, op7_to_0);

    __wait ( ch, 1*msec);

    //// Group TMRS
    //	    -	MR = 9
    //      - 	C[8:10](CA[4:2]) = ca4_to_2
    //		    C[7:0](OP[7:0]) = op7_to_0
    ////
    switch(group)
	{
		case 8:
			ca4_to_2=0x10;
            op7_to_0=0x0;
			break;
		case 9:
			ca4_to_2=0x8;
            op7_to_0=0x0;
			break;
		case 10:
			ca4_to_2=0x4;
            op7_to_0=0x0;
			break;
        default:
            ca4_to_2=0x0;
            op7_to_0 = ((0x1 << group) & (0x0FF));
			break;

	}

	__REG32(0x108000F0+ch*0x100000) = ((0x00000)|(0x1<<1)|(ca4_to_2));

    DmcDriver.command.mrw( ch, rank, 9, op7_to_0);

    __wait ( ch, 1*msec);

}

//--------------------------------------------------------
// Latch_or_Exit_TMRS
//    - Function to check whether TRMS Mode or not.
//
//    - Paramter : Channel number, Rank number
//
//					Channel number : 0 ~ 4
//                  Rank number : -1, 0, 1
//;
//;    - Return : void
//;--------------------------------------------------------
void LatchExitTMRS(int ch, dmc_rank_t rank)
{
    __REG32(0x108000F0+ch*0x100000) = 0x10000;

    DmcDriver.command.mrw( ch, rank, 9, 0x0);

    __wait ( ch, 1*msec);

}

void LockTMRS(int ch, dmc_rank_t rank)
{
    SetTMRS(ch, DMC_RANK_ALL, 2,5,8);
    LatchExitTMRS(ch, DMC_RANK_ALL);
    LatchExitTMRS(ch, DMC_RANK_ALL);
}

void TMRSItems_MR4_RefreshRateX2(int ch, dmc_rank_t rank)
{
    if ( GET_MAIN_SUB_REV() < 0x12 )
		return;

    EnterTMRS(ch, DMC_RANK_ALL);

    SetTMRS(ch, DMC_RANK_ALL, 0,9,7);
    LatchExitTMRS(ch, DMC_RANK_ALL);
    LatchExitTMRS(ch, DMC_RANK_ALL);

    SetTMRS(ch, DMC_RANK_ALL, 4,10,3);
    LatchExitTMRS(ch, DMC_RANK_ALL);
    LatchExitTMRS(ch, DMC_RANK_ALL);

    SetTMRS(ch, DMC_RANK_ALL, 4,6,9);
    LatchExitTMRS(ch, DMC_RANK_ALL);
    LatchExitTMRS(ch, DMC_RANK_ALL);

    LockTMRS(ch, DMC_RANK_ALL);
}
#endif

/******************************************************************************
 *
 * @fn		MemoryInit
 *
 * @brief	memory initialization
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
int MemoryInit(int dmc_clk, int resume) /*(int resume, int(*clock)(int *), lp4_conf_t *conf)*/
{
	const int NUM_DMC_CH = 4;

	int ch;
	int rank;
	int freq;

	InitDmcDriver();

	for( ch = 0; ch < NUM_DMC_CH; ch++ ) {

		dram_unreset( ch, resume );

		soc_preset( ch );

//		io_preset(ch);

		soc_zq_calibration( ch );

		dram_preset( ch );

#if TMRS_MR4X2
		TMRSItems_MR4_RefreshRateX2(ch, DMC_RANK_ALL);
#endif

		calibration_preset( ch );
	}

	change_freq( freq );

	for( ch = 0; ch < NUM_DMC_CH; ch++ ) {

//		calibration_preset( ch );

#if 0
		DmcDriver.dfi.mode( ch, 1, 0 );
		DmcDriver.dfi.init( ch , 1 );
		while( DmcDriver.dfi.status( ch ) == 0 );
		DmcDriver.dll.resync( ch );
#else
		DmcDriver.dfi.mode( ch, 1, 0 );

		DmcDriver.dll.enable( ch, 1 );

		DmcDriver.dfi.init( ch , 1 );
		while( DmcDriver.dfi.status( ch ) == 0 );
		DmcDriver.dfi.init( ch , 0 );

		DmcDriver.dll.resync( ch );

		/* set sdll update mode mutual */
		DmcDriver.dfi.mode( ch, 0, 0 );
		dmc_phy_upd_mode( ch, 1 );

		dmc_sel_drex_timing_set( ch, 0);
#endif

		#if (DMC_DQS_TYPE == DMC_DQS_MODE1)
		/* modified pattern for gate train */
		//calibration_preset_gate_train_evt0( ch );
		gate_training( ch, DMC_RANK_ALL );
		#endif

		/* correct pattern for rd cal, wr cal */
		calibration_preset( ch );

		#if (DMC_DQS_TYPE == DMC_DQS_MODE1)
		/* No need calibration preset after gate train */
		;
		#else
		DmcDriver.cal.preset( ch );
		#endif

		read_dq_calibration( ch, DMC_RANK_0 );

#if WRITE_TRAINING_FINE
		dmc_cal_wr_pattern(ch);
		dmc_write_dq_cal_fine( ch, DMC_RANK_0 );
		dmc_write_dq_cal_fine( ch, DMC_RANK_1 );
#else
		write_dq_calibration( ch, DMC_RANK_ALL );
#endif
		dmc_read_dq_cal_fine_train( ch, DMC_RANK_0 );

		#if (DMC_DQS_TYPE == DMC_DQS_MODE1)
		/* modified pattern for gate train */
		//calibration_preset_gate_train_evt0( ch );
		//DmcDriver.cal.postset(ch);
		DmcDriver.cal.hw_periodic(ch, 1, 0);
		#else
		DmcDriver.cal.hw_periodic(ch, 0, 0);
		#endif

		dmc_vtc_en(ch);

#if MC_INITIATED_SDLL_UPDATE
		/* set mc update mode */
		DmcDriver.dfi.mode( ch, 1, 0 );
		dmc_phy_upd_mode( ch, 1 );
#else
		/* set phy update mode */
		DmcDriver.dfi.mode( ch, 0, 0 );
		dmc_phy_upd_mode( ch, 0 );
#endif
	}

	for( ch = 0; ch < NUM_DMC_CH; ch++ ) {

		memory_init_done( ch, resume );
	}

	for( ch = 0; ch < NUM_DMC_CH; ch++ ) {
		set_vt_mon(ch, 0, 0);
	}

	sel_timing_set_source(0);

	enable_pause_mif();

	//enable_root_clk_gating();

	for( ch = 0; ch < NUM_DMC_CH; ch++ ) {
		dmc_low_power(ch);
		dmc_io_pd(ch);
	}

	return 1;
}

void mem_ctrl_init(void)
{
	struct exynos7420_power *pmu = (struct exynos7420_power *)EXYNOS7420_POWER_BASE;
	volatile unsigned int eBootStat = readl(&pmu->inform1);

	/* istor assume only 3GB DRAM */
	__REG64((u64)CONFIG_PHY_BL_SYS_INFO_DRAM_SIZE) = 0xC0000000;

	if(eBootStat != S5P_CHECK_SLEEP) /* cold boot */
		MemoryInit(MEMCLK, 0);
	else /* resume */
		MemoryInit(MEMCLK, 1);
}
