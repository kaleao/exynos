/*
 * Memory setup for SMDK5412 board based on EXYNOS5
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

#include <common.h>
#include <config.h>
#include <asm/arch/cpu.h>
#include <asm/arch/smc.h>

#define Outp32(addr, data)	(*(volatile u32 *)(addr) = (data))
#define Inp32(addr)		(*(volatile u32 *)(addr))
#define SetBits(uAddr, uBaseBit, uMaskValue, uSetValue) \
		Outp32(uAddr, (Inp32(uAddr) & ~((uMaskValue)<<(uBaseBit))) \
		| (((uMaskValue)&(uSetValue))<<(uBaseBit)))

////////////////////////////////////////////////////////////////////////////////
///
/// Added by moonki.jun
///
////////////////////////////////////////////////////////////////////////////////
typedef enum __REMAP_CASE{
	REMAP_CASE_1 = 0,
	REMAP_CASE_2 = 1,
	REMAP_CASE_3 = 2
} REMAP_CASE;

#define CA_SWAP				0
#define NUM_CHIP 			1
#define ZQ_MODE_DDS			7
#define PERFORM_LEVELING	0
//#define PHY0_BASE			0x10C00000
//#define PHY1_BASE			0x10C10000
//#define DREX1_0				0x10C20000
//#define DREX1_1				0x10C30000
#define CMU_COREPART		0x10010000
#define CMU_TOPPART			0x10020000
#define CMU_MEMPART			0x10030000
#define TZASC_0				0x10D40000
#define TZASC_1				0x10D50000
#define WIDEIO_PHY0        0x10F00000
#define WIDEIO_PHY1        0x10F40000
#define WIDEIO_PHY2        0x10F80000
#define WIDEIO_PHY3        0x10FC0000
#define WDREX0         0x10F10000
#define WDREX1         0x10F50000
#define WDREX2         0x10F90000
#define WDREX3         0x10FD0000

#define WIO_GATE_TRAINING		0

#if 0 /* In EVT1, REMAP register is removed.*/
static void remap_memory(u32 remap_case)
{
	if (remap_case == REMAP_CASE_1) {
		/* EVT0 TSV: wideIO 1G + LPDDR3 1G */
		Outp32(0x10CE0050, 0x00000100);
	} else if (remap_case == REMAP_CASE_2) {
		/* EVT0 SCP: LPDDR3 2G */
		Outp32(0x10CE0050, 0x00000001);
	} else {
		/* n/a */
		;
	}
}
#endif

static void init_wideIO_sdr(u32 PHY_address, u32 DREX_address, u32 wioFreq, u32 init_DRAM)
{
#if defined(CONFIG_SMC_CMD)
	reg_arr_t reg_arr;
#endif
	u32 data;

	/*  MEMCONTROL
		bl=1 (burst length 2), mem_width=4 (128-bit), clk_stop_en=0 (if CGCONTROL is set to save power, this bit should be zero.)
	*/
	Outp32(DREX_address+0x0004, 0x00104000);

	/*  MEMCONFIG0
		bank_interleave on, col_address=7bits, row_address=15bits, bank=4
	*/
#if defined(CONFIG_SMC_CMD)
	reg_arr.set0.addr = DREX_address+0x10F10;
	reg_arr.set0.val = 0x00001032;

	set_secure_reg((u32)&reg_arr, 1);
#else
	Outp32(DREX_address+0x10F10, 0x00001032);
#endif

	/* PHYCONTROL0
		drv_bus_en=1
	*/
	Outp32(DREX_address+0x0018, 0x00000004);

	/*  TIMINGAREF
		Need to set tREFI for hot temp. compensation - 1/2 tREFI (0x5D = 3.9us x 24MHz so, 1/2 * 0x5D = 0x2E)
	*/
	Outp32( DREX_address+0x0030, 0x0000002E);

	/* TIMING
		For WDREX clk 266MHz (1tCLK=3.75ns)
		TIMINGROW: t_rfc = 210 ns, t_rrd = 10 ns, t_rp = 18 ns, t_rcd = 18ns, t_rc = 60 ns, t_ras = 42 ns
	*/

	if (wioFreq == 266) {
		Outp32(DREX_address+0x0034, 0x3835540C); //0x3835540C
		//  TIMINGDATA: t_wtr = 15 ns, t_wr = 15 ns, trtp = 2, t_srr = 2, dqsck = 2, wl = 1, t_src = 7, rl = 3
		Outp32(DREX_address+0x0038, 0x44300123); //0x44222173
		//  TIMINGPOWER: t_faw = 50 ns, t_xsr = 220 ns, t_xp = 10 ns, t_cke = 3, t_mrd = 2
		Outp32(DREX_address+0x003C, 0x383B0332); //0x383B0332
	} else {
		;
	}

	/*  MEMBASECONFIG
		: dram_base=0x2000_0000, dram_siz=256M --> chip_base=0x08, chip_mask=0x7f0
	*/
#if defined(CONFIG_SMC_CMD)
	reg_arr.set0.addr = DREX_address+0x10F00;
	reg_arr.set0.val = 0x000807F0;

	set_secure_reg((u32)&reg_arr, 1);
#else
	Outp32(DREX_address+0x10F00, 0x000807F0);
#endif

	if (init_DRAM) {
		/*  DRAMNOPCTRL
			For WDREX clk 266MHz (1tCLK=3.75ns)
			.assert NOP, wait for tINIT5
			.tINIT5 min. should be 200us. Thus 200us/1tCLK = 200us/3.75ns = 53333.33..
		*/
		Outp32(PHY_address+0x0074, 0x000D0FC1);
		while(( Inp32(PHY_address+0x0074) & 0x00000001 ) != 0x00000001);

		/* DRAMMRSCTRL
		*/
		// wideIO mode register setting, WL=1, RL=3, BL=2
		Outp32(PHY_address+0x0078, 0x20001311);
		while((Inp32(PHY_address+0x0078) & 0x00000001 ) != 0x00000001);
		// wideIO mode register setting, nWR=3, drive_strength=weak, thermal_offset=5~15
		Outp32(PHY_address+0x0078, 0x22002211);
		//Outp32(PHY_address+0x0078, 0x22001011); // drive_strength=strong
		while((Inp32(PHY_address+0x0078) & 0x00000001 ) != 0x00000001);

		if (WIO_GATE_TRAINING) {
			/* STARTPOINT
			*/
			Outp32(PHY_address+0x0018, 0x00000010);

			/* INC
			*/
			Outp32(PHY_address+0x0020, 0x00000010);

			Outp32(PHY_address+0x002C, 0x00000001);     // DLLON - DLL clock enable
			Outp32(PHY_address+0x0024, 0x00000001);     // START - DLL lock start
			while((Inp32(PHY_address+0x0054) & 0x00000007) != 0x00000007);      // LOCKSTATUS - DLL lock check

			Outp32(PHY_address+0x0070, 0x00000002);     // PHYFASTINIT - gate trainig
			while((Inp32(PHY_address+0x0070) & 0x00000002) != 0x00000002);      // check gate training completion

			Outp32(PHY_address+0x002C, 0x00000000);     // DLLON - DLL clock disable

			//  Outp32(DREX_address+0x0018, 0x20000004);
			Outp32(DREX_address+0x0018, 0x00000004);

			/* GATEDURATION
				: gate duration=1, butst_length(2)-1
			*/
			Outp32(PHY_address+0x00e4, 0x00000001);
		}

		/* Dummy read for EVT1 WIO DRAM
			Venus EVT1 WIO DRAM has non-high-z DQS state, to eliminate the need of gate training while DVFS,
			and which is enabled after a READ operation. (ACT->READ->PRECH)
		*/
		{
			// direct cmd - ACT
			Outp32(PHY_address+0x007C, 0x000000C1);
			while((Inp32(PHY_address+0x007C) & 0x00000001) != 0x00000001);
			// direct cmd - READ
			Outp32(PHY_address+0x006C, 0x53000001);
			while((Inp32(PHY_address+0x006C) & 0x00000001) != 0x00000001);
			// direct cmd - PRECH
			Outp32(PHY_address+0x0080, 0x00800011);
			while((Inp32(PHY_address+0x0080) & 0x00000001) != 0x00000001);

			// resync
			Outp32(PHY_address+0x0034, 0x00000001);
			Outp32(PHY_address+0x0034, 0x00000000);
		}
	}
	/* DRAMCOMMANDSEL
		: DREX takes over the DRAM control
	*/
	Outp32(PHY_address+0x0068, 0x00000000);

	/* CONCONTROL
		: auto refresh enable
	*/
	Outp32(DREX_address+0x0000, 0x0FFF1028);

	/* CGCONTROL
		DREX dynamic clock gating control (added in EVT1 WDREX 2.0.3)
	*/
	Outp32(DREX_address+0x0008, 0x0000001F);
}

void enable_bypass_pll()
{
	u32 data;

	#if 0
	/* For Venus EVT0 - MPLL_CON1 */
	data = Inp32(0x10014104);
	data=data&(~0x400000);
	data=data|(0x400000);
	Outp32(0x10014104, data);
	Delay(100*1000);
	#else
	/* For Venus EVT1 - TPLL_CON1 */
	data = Inp32(0x10020194);
	data=data&(~0x400000);
	data=data|(0x400000);
	Outp32(0x10020194, data);
	#endif
}

void disable_bypass_pll()
{
	u32 data;

	#if 0
	/* For Venus EVT0 -MPLL_CON1 */
	data = Inp32(0x10014104);
	data=data&(~0x400000);
	Outp32(0x10014104, data);
	#else
	/* For Venus EVT0 - TPLL_CON1 */
	data = Inp32(0x10020194);
	data=data&(~0x400000);
	Outp32(0x10020194, data);
	#endif
}

 void wdmc_ctrl_init(void)
{
	u32 data;
	u32 uFreq;

	/* CMU setting for WDREX
	*/
	;

	/*  Memory usage policy for Venus
		.REMAP_CASE_1: LPDDR3 1G + WideIO SDR 1G (evt0 TSV, evt1)
		.REMAP_CASE_2: LPDDR3 2G (evt0 SCP)
	*/
	/* In EVT1, REMAP register is removed.*/
	//remap_memory(REMAP_CASE_1);

	/*  DRAM reset
		: WideIO SDR reset should be done by WDREX ch1.
	*/
	//  Enable bypass MPLL - DRAM reset should be performed along with low frequency
	enable_bypass_pll();

	/* USEATGATEGEN
		: automatic gate generation disable by mhjang : for re-initializing
	*/
	//Outp32(WIDEIO_PHY0+0x00e0, 0x00000000);
	//Outp32(WIDEIO_PHY1+0x00e0, 0x00000000);
	//Outp32(WIDEIO_PHY2+0x00e0, 0x00000000);
	//Outp32(WIDEIO_PHY3+0x00e0, 0x00000000);

	Outp32(WIDEIO_PHY0+0x0068, 0x00000001);	// PHY wrapper takes over the DRAM control for DRAM/PHY initialization
	Outp32(WIDEIO_PHY2+0x0068, 0x00000001);
	Outp32(WIDEIO_PHY3+0x0068, 0x00000001);
	Outp32(WIDEIO_PHY1+0x0068, 0x00000001);

	//  Reset control order on channels is suggested as ch0->ch2->ch3->ch1
	Outp32(WIDEIO_PHY0+0x0050, 0x00050001);     // tINIT3 = 0x5000, De-assert RESET signal to DRAM
	Outp32(WIDEIO_PHY2+0x0050, 0x00050001);
	Outp32(WIDEIO_PHY3+0x0050, 0x00050001);
	Outp32(WIDEIO_PHY1+0x0050, 0x00050001);
	while(( Inp32( WIDEIO_PHY1+0x0050 ) & 0x00000001 ) != 0x00000001);

	//  Disable bypass MPLL - DRAM reset should be performed along with low frequency
	disable_bypass_pll();

	/*  Initialize wideIO SDR
		: WDREX channel initialization sequency is recommended as follows. ch0->ch1->ch2->ch3
	*/
	uFreq = 266;
	init_wideIO_sdr(WIDEIO_PHY0, WDREX0, uFreq, 1);
	init_wideIO_sdr(WIDEIO_PHY1, WDREX1, uFreq, 1);
	init_wideIO_sdr(WIDEIO_PHY2, WDREX2, uFreq, 1);
	init_wideIO_sdr(WIDEIO_PHY3, WDREX3, uFreq, 1);
}

void wdmc_ctrl_init_resume(void)
{
	u32 data;
	u32 uFreq;

	/* CMU setting for WDREX
	*/

	/*  Memory usage policy for Venus
		.REMAP_CASE_1: LPDDR3 1G + WideIO SDR 1G (evt0 TSV, evt1)
		.REMAP_CASE_2: LPDDR3 2G (evt0 SCP)
	*/
	/* In EVT1, REMAP register is removed.*/
	//remap_memory(REMAP_CASE_1);

	#if 1
	/* In this routine, we want to initialize DREX only. DRAM should not be initialized.
	But, here, we have to assert DRAM reset as well, because Venus DRAM reset control also resets PHY FSM.
	*/
	/*  DRAM reset
		: WideIO SDR reset should be done by WDREX ch1.
	*/
	//  Enable bypass MPLL - DRAM reset should be performed along with low frequency
	enable_bypass_pll();

	/* USEATGATEGEN
	: automatic gate generation disable by mhjang : for re-initializing
	*/
	//Outp32(WIDEIO_PHY0+0x00e0, 0x00000000);
	//Outp32(WIDEIO_PHY1+0x00e0, 0x00000000);
	//Outp32(WIDEIO_PHY2+0x00e0, 0x00000000);
	//Outp32(WIDEIO_PHY3+0x00e0, 0x00000000);

	Outp32(WIDEIO_PHY0+0x0068, 0x00000001);	// PHY wrapper takes over the DRAM control for DRAM/PHY initialization
	Outp32(WIDEIO_PHY2+0x0068, 0x00000001);
	Outp32(WIDEIO_PHY3+0x0068, 0x00000001);
	Outp32(WIDEIO_PHY1+0x0068, 0x00000001);

	//  Reset control order on channels is suggested as ch0->ch2->ch3->ch1
	Outp32(WIDEIO_PHY0+0x0050, 0x00050001);     // tINIT3 = 0x5000, De-assert RESET signal to DRAM
	Outp32(WIDEIO_PHY2+0x0050, 0x00050001);
	Outp32(WIDEIO_PHY3+0x0050, 0x00050001);
	Outp32(WIDEIO_PHY1+0x0050, 0x00050001);
	while(( Inp32( WIDEIO_PHY1+0x0050 ) & 0x00000001 ) != 0x00000001);

	//  Disable bypass MPLL - DRAM reset should be performed along with low frequency
	disable_bypass_pll();
	#endif

	/*  Initialize wideIO SDR
		: WDREX channel initialization sequency is recommended as follows. ch0->ch1->ch2->ch3
	*/
	uFreq = 266;
	init_wideIO_sdr(WIDEIO_PHY0, WDREX0, uFreq, 0);
	init_wideIO_sdr(WIDEIO_PHY1, WDREX1, uFreq, 0);
	init_wideIO_sdr(WIDEIO_PHY2, WDREX2, uFreq, 0);
	init_wideIO_sdr(WIDEIO_PHY3, WDREX3, uFreq, 0);
}

void pad_retention_release(void)
{
	Outp32(0x100431E8, 0x10000000);
	while ((Inp32(0x10043004) & (0x1)) == 0);
}
