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

#define Outp32(addr, data)	(*(volatile u32 *)(addr) = (data))
#define Inp32(addr)		(*(volatile u32 *)(addr))

typedef volatile unsigned char   		u8v;		//-  8-bit unsigned integer
typedef volatile unsigned short  		u16v;		//- 16-bit unsigned integer
typedef volatile unsigned int   		u32v;		//- 32-bit unsigned integer
typedef volatile unsigned long long		u64v;		//- 32-bit unsigned integer

#define __REG8(x)							(*((8v *)(x)))
#define __REG16(x)							(*((u16v *)(x)))
#define __REG32(x)							(*((u32v *)(x)))
#define __REG64(x)							(*((u64v *)(x)))


extern volatile drex_regs_t *__DREX[4];
extern volatile drex_ppc_regs_t *__PPC[4];
extern volatile phy_regs_t *__PHY[4];
extern volatile asp_regs_t *__ASP[4];

typedef struct {
	int left;
	int right;
	int center;
	int size;
	int lock_value;
} valid_window_margin_t;


static lp4_mr13_op_t modeRegister13;
static lp4_fsp_t freqSetPoint[2];


static void wait(int ch, float sec)
{
#if 0
	float elapsed;
	unsigned int a, b;

	DmcDriver.ppc.init( ch , 1 );

	a = DmcDriver.ppc.ccnt( ch );

	elapsed = 0;
	while( elapsed < sec ) {

		b = DmcDriver.ppc.ccnt( ch );
		elapsed = (float)( b - a ) / (float)158000000.0;				//TEMP:
	}
#else
	int i=100000;
	while(i--);
#endif
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
	DmcDriver.zq.init( ch, 1 );
	while( DmcDriver.zq.status( ch ) == 0 );
	DmcDriver.zq.init( ch, 0 );
}

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
static int read_dq_calibration(int ch, int enable)
{
	if( enable == 1 ) {

		DmcDriver.command.mrw( ch, 0, 32, (unsigned char)0x55 );
		DmcDriver.command.mrw( ch, 1, 32, (unsigned char)0x55 );
		wait( ch, 1 * msec);

		DmcDriver.command.mrw( ch, 0, 15, (unsigned char)0x55 );
		wait( ch, 1 * msec);
		DmcDriver.command.mrw( ch, 1, 15, (unsigned char)0x55 );
		wait( ch, 1 * msec);

		DmcDriver.command.mrw( ch, 0, 40, (unsigned char)0x55 );
		wait( ch, 1 * msec);
		DmcDriver.command.mrw( ch, 1, 40, (unsigned char)0x55 );
		wait( ch, 1 * msec);

		DmcDriver.command.mrw( ch, 0, 20, (unsigned char)0x55 );
		wait( ch, 1 * msec);
		DmcDriver.command.mrw( ch, 1, 20, (unsigned char)0x55 );
		wait( ch, 1 * msec);


		DmcDriver.cal.read_dq( ch, 0, 0x5555, 0x5555 );
	}
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
static int write_dq_calibration(int ch, int enable)
{
	unsigned short dq[16];
	unsigned short dmi;
	unsigned char vref;

	dq[0] = 0xffff;
	dq[1] = 0x55;
	dq[2] = 0xffff;
	dq[3] = 0x55;
	dq[4] = 0xffff;
	dq[5] = 0x55;
	dq[6] = 0xffff;
	dq[7] = 0x55;
	dmi = 0x5ff5;

	if( enable == 1 ) {
		DmcDriver.cal.write_dq( ch, 0, dq, &dmi );
	}

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
static int dmc_driver_freq(int mega_hertz)
{
	unsigned int tmp;
#if 0 /* ori */
		__REG32( 0x10850200 ) = ( 1 << 16 ) | ( 1 << 12 ) | ( 2 << 8 ) | ( 2 << 4 ) | ( 1 << 0 );
		__REG32( 0x10950200 ) = ( 1 << 16 ) | ( 1 << 12 ) | ( 2 << 8 ) | ( 2 << 4 ) | ( 1 << 0 );
		__REG32( 0x10a50200 ) = ( 1 << 16 ) | ( 1 << 12 ) | ( 2 << 8 ) | ( 2 << 4 ) | ( 1 << 0 );
		__REG32( 0x10b50200 ) = ( 1 << 16 ) | ( 1 << 12 ) | ( 2 << 8 ) | ( 2 << 4 ) | ( 1 << 0 );

		__REG32( 0x10570214 ) = ( 1 << 20 ) | ( 1 << 16 ) | ( 1 << 12 ) | ( 1 << 8 ) | ( 0 << 0 );

		// DATA.SET SD:0x1057060C  %LE %LONG 0x00100000
		__REG32( 0x1057060c ) = 0x00100000;
#elif 0
		__REG32( 0x10850200 ) = ( 1 << 16 ) | ( 1 << 12 ) | ( 2 << 8 ) | ( 2 << 4 ) | ( 1 << 0 );
		__REG32( 0x10950200 ) = ( 1 << 16 ) | ( 1 << 12 ) | ( 2 << 8 ) | ( 2 << 4 ) | ( 1 << 0 );
		__REG32( 0x10a50200 ) = ( 1 << 16 ) | ( 1 << 12 ) | ( 2 << 8 ) | ( 2 << 4 ) | ( 1 << 0 );
		__REG32( 0x10b50200 ) = ( 1 << 16 ) | ( 1 << 12 ) | ( 2 << 8 ) | ( 2 << 4 ) | ( 1 << 0 );

		__REG32( 0x10570214 ) = ( 1 << 20 ) | ( 1 << 16 ) | ( 1 << 12 ) | ( 1 << 8 ) | ( 0 << 0 );

		// DATA.SET SD:0x1057060C  %LE %LONG 0x00100000
		__REG32( 0x1057060c ) = 0x00100000;
#else
	__REG32( 0x10570214 ) = (1<<20) | __REG32( 0x10570214 ); //CLK_MUX_SEL_TOPC5	pause=1
	while(0x00000000 != (__REG32(0x10570414)&0x0F000000));// "CLK_MUX_STAT_TOPC5

#define SCLK_BUS0_PLL_MIF_SEL 0	// 0=MOUT_TOPC_BUS0_PLL(1600MHz), 1=MOUT_TOPC_BUS0_PLL_DIV2(800MHz), 2=MOUT_TOPC_BUS1_PLL(668MHz), 3=MOUT_TOPC_CCI_PLL(532MHz)
#define SCLK_BUS0_PLL_MIF_RATIO 0 //MOUT_SCLK_BUS0_PLL_MIF/(SCLK_BUS0_PLL_MIF_RATIO+1)

		//mif switching pll changes
		//800/(7+1) = 100MHz (low freq init) -> 1600/(0+1)=1600MHz (for dvfs switching)

		tmp = __REG32( 0x10570204 ); //CLK_MUX_SEL_TOPC1
		tmp &= ~(3<<12);
		tmp |= ((SCLK_BUS0_PLL_MIF_SEL)<<12);
		__REG32( 0x10570204 ) = tmp;

		tmp = __REG32( 0x1057060C ); //CLK_DIV_TOPC3
		tmp &= ~(0x7<<4);
		tmp |= ((SCLK_BUS0_PLL_MIF_RATIO)<<4);
		__REG32( 0x1057060C ) = tmp;
#endif
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

void set_vt_mon(unsigned int ch)
{
#if 1
	unsigned int DREX_BASE;
	unsigned int VTMON_BASE;
	unsigned int PHY_BASE;

	unsigned int OSC_STOP_CYCLE=0x10; //	;512 cycle
	unsigned int MPC_to_MRR=0x200;
	unsigned int MPC_to_MPC=0x2000;
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
	__REG32( DREX_BASE+0x10 ) = (0x00221c00)|(OSC_STOP_CYCLE<<2);

	/*; EVT0 - set VT_MON run cnt limit time (MPC->MRR)
	d.s SD:&VTMON_BASE+0x04 %LE %LONG (0x0)|&MPC_to_MRR
	wait 0.1s
	*/
	__REG32( VTMON_BASE+0x04 ) = (0x0)|MPC_to_MRR;

	/*; EVT1 - set VT_MON run cnt limit time (MPC->MRR)
	;d.s SD:&VTMON_BASE+0x80 %LE %LONG 0x02002000
	;wait 0.1s
	*/
	__REG32( VTMON_BASE+0x80 ) = 0x02002000;

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
	__REG32( VTMON_BASE+0x70) = 0x000C005D ;

	/*;Aref_interval_cnt_limit
	d.s SD:&VTMON_BASE+0x6C %LE %LONG 0x00300001
	*/
	__REG32( VTMON_BASE+0x6C) =0x00300001 ;

	/*; PHY- dqs_int_runtime
	GOSUB setReg &PHY_BASE+0x0600 0xFF &OSC_STOP_CYCLE
	*/
	__REG32( PHY_BASE+0x0600) &= ~(0xff);
	__REG32( PHY_BASE+0x0600) |= (OSC_STOP_CYCLE);

	/*; PHY- ctrl_dqs_osc_en
	GOSUB setReg &PHY_BASE+0x04 0x00080000 0x00080000
	*/
//	__REG32( PHY_BASE+0x04) |= 0x00080000;

	/*; Set VT_MON_CON (MPC->next MPC)
	; EVT0
	d.s SD:&VTMON_BASE+0x14 %LE %LONG (0x80000000)|(0x30000000)|(0x00070000)|(&MPC_to_MPC)|(1<<23.)
	*/
//	__REG32( VTMON_BASE+0x14 ) = (0x80000000)|(0x30000000)|(0x00070000)|(MPC_to_MPC)|(1<<23);
	__REG32( VTMON_BASE+0x14 ) = (0x80000000)|(0x00000000)|(0x00050000)|(MPC_to_MPC)|(1<<23);

#else
	//d.s SD:&VTMON_BASE+0x14 %LE %LONG (0x80000000)|(0x30000000)|(0x00070000)|(&MPC_to_MPC)|(1<<23.)
	__REG32( 0x10890014 ) = 0x80000000 | (1<<23);
	__REG32( 0x10990014 ) = 0x80000000 | (1<<23);
	__REG32( 0x10a90014 ) = 0x80000000 | (1<<23);
	__REG32( 0x10b90014 ) = 0x80000000 | (1<<23);
#endif
}

void set_sdll_update_mode(unsigned int set)
{
	if (set) {
		__REG32( 0x10800000 ) |= (unsigned int)(1<<3);
		__REG32( 0x10900000 ) |= (unsigned int)(1<<3);
		__REG32( 0x10a00000 ) |= (unsigned int)(1<<3);
		__REG32( 0x10b00000 ) |= (unsigned int)(1<<3);

		__REG32( 0x10820050 ) |= (unsigned int)(1<<28);
		__REG32( 0x10920050 ) |= (unsigned int)(1<<28);
		__REG32( 0x10a20050 ) |= (unsigned int)(1<<28);
		__REG32( 0x10b20050 ) |= (unsigned int)(1<<28);
	} else {
		__REG32( 0x10800000 ) &= (unsigned int)~(1<<3);
		__REG32( 0x10900000 ) &= (unsigned int)~(1<<3);
		__REG32( 0x10a00000 ) &= (unsigned int)~(1<<3);
		__REG32( 0x10b00000 ) &= (unsigned int)~(1<<3);

		__REG32( 0x10820050 ) &= (unsigned int)~(1<<28);
		__REG32( 0x10920050 ) &= (unsigned int)~(1<<28);
		__REG32( 0x10a20050 ) &= (unsigned int)~(1<<28);
		__REG32( 0x10b20050 ) &= (unsigned int)~(1<<28);
	}
}

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
int MemoryInit(int dmc_clk, int resume)
{
	int ch;
	int rank;

	const int NUM_DMC_CH = 4;

	InitDmcDriver();
	DmcDriver.dvfs.freq = dmc_driver_freq;

	//- EVT0
	{
		int channel;

		channel = 0;
		if (!resume) {
			DmcDriver.init.reset( channel, 0 );
			wait( channel, 1 * msec);
		}

		for( channel = 0; channel < 4; channel++ ) {

			#if 1	/* evt0 */
			if( ( channel == 1 ) || ( channel == 2 ) ) {
				__DREX[channel]->CONCONTROL.ca_swap = 1;
				__PHY[channel]->CAL_CON0.ca_swap_mode = 1;
				__DREX[channel]->MEMCONTROL.mrr_byte = 1;
			} else {
				__DREX[channel]->CONCONTROL.ca_swap = 0;
				__PHY[channel]->CAL_CON0.ca_swap_mode = 0;
				__DREX[channel]->MEMCONTROL.mrr_byte = 0;
			}
			#else	/* evt1 */
			__DREX[channel]->CONCONTROL.ca_swap = 0;
			__PHY[channel]->CAL_CON0.ca_swap_mode = 0;
			__DREX[channel]->MEMCONTROL.mrr_byte = 0;
			#endif

			DmcDriver.dfi.mode( channel, 0, 1 );
			if (!resume) {
				DmcDriver.command.epd( channel, -1 );
			}
		}
		wait( channel, 1 * msec);
		channel = 0;

		// FIXME: wait 10ns
		if (!resume) {
			DmcDriver.init.reset( channel, 1 );
			wait( channel, 1 * msec);
		}
	}

	//- EVT0
	for( ch = 0; ch < NUM_DMC_CH; ch++ ) {

		soc_preset( ch );

		if( ( ch == 0 ) || ( ch == 3 ) ) {
			soc_zq_calibration( ch );
		}

		//DmcDriver.dfi.mode( ch, 0, 1 );

		DmcDriver.command.xpd( ch, 0 );
		wait( ch, 1 * msec);
		DmcDriver.command.xpd( ch, 1 );
		wait( ch, 1 * msec);

#if 0
		freqSetPoint[modeRegister13.FSP_WR].MR3.PU_CAL = 1;
		freqSetPoint[modeRegister13.FSP_WR].MR3.PDDS = 6;
		freqSetPoint[modeRegister13.FSP_WR].MR3.DBI_RD = 0;
		freqSetPoint[modeRegister13.FSP_WR].MR3.DBI_WR = 0;

		DmcDriver.command.mrw( ch, -1, 3, *((unsigned char *)&freqSetPoint[0].MR3) );
		wait( ch, 1 * msec);

		freqSetPoint[modeRegister13.FSP_WR].MR22.CODT = 0;
		freqSetPoint[modeRegister13.FSP_WR].MR22.ODTE_CK = 0;
		freqSetPoint[modeRegister13.FSP_WR].MR22.ODTE_CS = 0;
		freqSetPoint[modeRegister13.FSP_WR].MR22.ODTD_CA = 0;

		DmcDriver.command.mrw( ch, -1, 22, *((unsigned char *)&freqSetPoint[0].MR22) );
		wait( ch, 1 * msec);


		DmcDriver.command.mpc( ch, -1, LPDDR4_MPC1_ZQCAL_START );
		wait( ch, 1 * msec);
		DmcDriver.command.mpc( ch, -1, LPDDR4_MPC1_ZQCAL_LATCH );
		wait( ch, 1 * msec);

		modeRegister13.CBT = 0;
		modeRegister13.VRO = 0;
		modeRegister13.VRCG = 0;
		modeRegister13.DM_DIS = 0;
		modeRegister13.FSP_WR = 1;
		modeRegister13.FSP_OP = 0;

		DmcDriver.command.mrw( ch, -1, 13, *( (unsigned char *)&modeRegister13 ) );
		wait( ch, 1 * msec);
#else
		DmcDriver.command.mpc( ch, 0, LPDDR4_MPC1_ZQCAL_START );
		wait( ch, 1 * msec);
		DmcDriver.command.mpc( ch, 0, LPDDR4_MPC1_ZQCAL_LATCH );
		wait( ch, 1 * msec);
		DmcDriver.command.mpc( ch, 1, LPDDR4_MPC1_ZQCAL_START );
		wait( ch, 1 * msec);
		DmcDriver.command.mpc( ch, 1, LPDDR4_MPC1_ZQCAL_LATCH );
		wait( ch, 1 * msec);

//		DmcDriver.command.mrw( ch, 0, 13, 0x40 );
//		DmcDriver.command.mrw( ch, 0, 13, 0x00 );
		wait( ch, 1 * msec);
//		DmcDriver.command.mrw( ch, 1, 13, 0x40 );
//		DmcDriver.command.mrw( ch, 1, 13, 0x00 );
		wait( ch, 1 * msec);
#endif

		{
		DmcDriver.command.mrw( ch, 0, 13, 0x40 );
		wait( ch, 1 * msec);
		DmcDriver.command.mrw( ch, 1, 13, 0x40 );
		wait( ch, 1 * msec);

		DmcDriver.command.mrw( ch, 0, 1, 0x26 );
		wait( ch, 1 * msec);
		DmcDriver.command.mrw( ch, 1, 1, 0x26 );
		wait( ch, 1 * msec);

#if (MEMCLK == 632)
		DmcDriver.command.mrw( ch, 0, 2, 0x12); //0x24); //0x12 );
		wait( ch, 1 * msec);
		DmcDriver.command.mrw( ch, 1, 2, 0x12); //0x24); //0x12 );
		wait( ch, 1 * msec);
#elif (MEMCLK == 828)
		DmcDriver.command.mrw( ch, 0, 2, 0x1b); //0x24); //0x12 );
		wait( ch, 1 * msec);
		DmcDriver.command.mrw( ch, 1, 2, 0x1b); //0x24); //0x12 );
		wait( ch, 1 * msec);
#elif (MEMCLK == 1068)
		DmcDriver.command.mrw( ch, 0, 2, 0x24); //0x12 );
		wait( ch, 1 * msec);
		DmcDriver.command.mrw( ch, 1, 2, 0x24); //0x12 );
		wait( ch, 1 * msec);
#else
		while(1);
#endif

		DmcDriver.command.mrw( ch, 0, 3, 0xf1 );
		wait( ch, 1 * msec);
		DmcDriver.command.mrw( ch, 1, 3, 0xf1 );
		wait( ch, 1 * msec);

		DmcDriver.command.mrw( ch, 0, 12, 0x5d );
		wait( ch, 1 * msec);
		DmcDriver.command.mrw( ch, 1, 12, 0x5d );
		wait( ch, 1 * msec);

		DmcDriver.command.mrw( ch, 0, 22, 0x20 );
		wait( ch, 1 * msec);
		DmcDriver.command.mrw( ch, 1, 22, 0x20 );
		wait( ch, 1 * msec);

		DmcDriver.command.mrw( ch, 0, 14, 0x4d );
		wait( ch, 1 * msec);
		DmcDriver.command.mrw( ch, 1, 14, 0x4d );
		wait( ch, 1 * msec);

		DmcDriver.command.mrw( ch, 0, 11, 0x01 );
		wait( ch, 1 * msec);
		DmcDriver.command.mrw( ch, 1, 11, 0x01 );
		wait( ch, 1 * msec);

		DmcDriver.command.mrw( ch, 0, 13, 0x00 );
		wait( ch, 1 * msec);
		DmcDriver.command.mrw( ch, 1, 13, 0x00 );
		wait( ch, 1 * msec);

		}

		{
		DmcDriver.command.mrw( ch, 0, 13, 0x00 );
		wait( ch, 1 * msec);
		DmcDriver.command.mrw( ch, 1, 13, 0x00 );
		wait( ch, 1 * msec);

		DmcDriver.command.mrw( ch, 0, 1, 0x26 );
		wait( ch, 1 * msec);
		DmcDriver.command.mrw( ch, 1, 1, 0x26 );
		wait( ch, 1 * msec);
#if (MEMCLK == 632)
			DmcDriver.command.mrw( ch, 0, 2, 0x12); //0x24); //0x12 );
			wait( ch, 1 * msec);
			DmcDriver.command.mrw( ch, 1, 2, 0x12); //0x24); //0x12 );
			wait( ch, 1 * msec);
#elif (MEMCLK == 828)
			DmcDriver.command.mrw( ch, 0, 2, 0x1b); //0x24); //0x12 );
			wait( ch, 1 * msec);
			DmcDriver.command.mrw( ch, 1, 2, 0x1b); //0x24); //0x12 );
			wait( ch, 1 * msec);
#elif (MEMCLK == 1068)
			DmcDriver.command.mrw( ch, 0, 2, 0x24); //0x24); //0x12 );
			wait( ch, 1 * msec);
			DmcDriver.command.mrw( ch, 1, 2, 0x24); //0x24); //0x12 );
			wait( ch, 1 * msec);
#else
			while(1);
#endif

		DmcDriver.command.mrw( ch, 0, 3, 0xf1 );
		wait( ch, 1 * msec);
		DmcDriver.command.mrw( ch, 1, 3, 0xf1 );
		wait( ch, 1 * msec);

		DmcDriver.command.mrw( ch, 0, 12, 0x5d );
		wait( ch, 1 * msec);
		DmcDriver.command.mrw( ch, 1, 12, 0x5d );
		wait( ch, 1 * msec);

		DmcDriver.command.mrw( ch, 0, 22, 0x20 );
		wait( ch, 1 * msec);
		DmcDriver.command.mrw( ch, 1, 22, 0x20 );
		wait( ch, 1 * msec);

		DmcDriver.command.mrw( ch, 0, 14, 0x4d );
		wait( ch, 1 * msec);
		DmcDriver.command.mrw( ch, 1, 14, 0x4d );
		wait( ch, 1 * msec);

		DmcDriver.command.mrw( ch, 0, 11, 0x01 );
		wait( ch, 1 * msec);
		DmcDriver.command.mrw( ch, 1, 11, 0x01 );
		wait( ch, 1 * msec);

		DmcDriver.command.mrw( ch, 0, 13, 0x00 );
		wait( ch, 1 * msec);
		DmcDriver.command.mrw( ch, 1, 13, 0x00 );
		wait( ch, 1 * msec);

		}

		// fsp-op=0
		DmcDriver.command.mrw( ch, 0, 13, 0x00 );
		DmcDriver.command.mrw( ch, 1, 13, 0x00 );

#if 0
		modeRegister13.CBT = 0;
		modeRegister13.VRO = 0;
		modeRegister13.VRCG = 0;
		modeRegister13.DM_DIS = 0;
		modeRegister13.FSP_WR = 1;
		modeRegister13.FSP_OP = 1;
		DmcDriver.command.mrw( ch, -1, 13, *( (unsigned char *)&modeRegister13 ) );
		wait( ch, 1 * msec);
#else
//		DmcDriver.command.mrw( ch, 0, 13, 0xc0 );
		wait( ch, 1 * msec);

//		DmcDriver.command.mrw( ch, 1, 13, 0xc0 );
		wait( ch, 1 * msec);
#endif
	}

	DmcDriver.dvfs.freq( 632 );

	for( ch = 0; ch < NUM_DMC_CH; ch++ ) {

		DmcDriver.cal.preset( ch );
		DmcDriver.dfi.mode( ch, 1, 0 );

		DmcDriver.dfi.init( ch , 1 );
		while( DmcDriver.dfi.status( ch ) == 0 );

		DmcDriver.dfi.init( ch , 0 );

		DmcDriver.dfi.update( ch );
		wait( ch, 1 * msec);

		read_dq_calibration( ch, 1 );

		write_dq_calibration( ch, 1 );

		dmc_vtc_en(ch);

		DmcDriver.dfi.update( ch );

		set_sdll_update_mode(0);

		if (resume) {
			DmcDriver.command.xsref( ch, 0 );
			DmcDriver.command.xsref( ch, 1 );
		}

		DmcDriver.init.done( ch );

		dmc_low_power(ch);

		// Added 20140519
		dmc_io_pd(ch);
	}

	for( ch = 0; ch < NUM_DMC_CH; ch++ ) {
		set_vt_mon(ch);
	}

	sel_timing_set_source(0);

	enable_pause_mif();

	// Added 20140519
	enable_root_clk_gating();

	return 1;
}

void mem_ctrl_init()
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
