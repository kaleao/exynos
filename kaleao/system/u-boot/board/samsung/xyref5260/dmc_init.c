/*
 * Memory setup for XYREF5260 board based on EXYNOS5260
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
#include <asm/arch/smc.h>
#include "setup.h"

#define Outp32(addr, data)	(*(volatile u32 *)(addr) = (data))
#define Inp32(addr)		(*(volatile u32 *)(addr))

volatile unsigned int *dram_pad_con;
volatile unsigned int *dram_pad_stat;
static u32 g_uBaseAddr;
static const u32 IROM_ARM_CLK = 400;

static unsigned int g_nMEMCLK;
static unsigned int g_cal = 0;
static unsigned int g_zq_mode_dds = 6;

unsigned int crl_dfdqs = 0;

enum DMC_SFR
{
	CONCONTROL		= 0x0000,
	MEMCONTROL		= 0x0004,
	MEMCONFIG0		= 0x0008,
	MEMCONFIG1		= 0x000c,
	DIRECTCMD		= 0x0010,
	PRECHCONFIG		= 0x0014,
	PHYCONTROL0	= 0x0018,
	PHYCONTROL1	= 0x001c,
	PHYCONTROL2	= 0x0020,
	PHYCONTROL3	= 0x0024,
	PWRDNCONFIG	= 0x0028,
	TIMINGPZQ		= 0x002c,
	TIMINGAREF		= 0x0030,
	TIMINGROW		= 0x0034,
	TIMINGDATA		= 0x0038,
	TIMINGPOWER		= 0x003c,
	PHYSTATUS		= 0x0040,
	PHYZQCONTROL	= 0x0044,
	CHIP0STATUS		= 0x0048,
	CHIP1STATUS		= 0x004c,
	AREFSTATUS		= 0x0050,
	MRSTATUS		= 0x0054,
	IVCONTROL		= 0x00f0,
};

void DMC_Delay(u32 x)
{
    while(--x)
	__asm ("NOP");
}

void DMC_SetClockForDREX(void)
{
	volatile u32 uBits;

	// MCLK_CDREX = 800
	// MCLK_DPHY  = 800
	// ACLK_CDREX = 400
	// PCLK_CDREX = 133

	// MCLK_CDREX2(1/1), ACLK_EFCON(1/2), MCLK_DPHY(1/1), MCLK_CDREX(1/1), ACLK_C2C_200(1/2), C2C_CLK_400(1/2), PCLK_CDREX(1/6), ACLK_CDREX(1/2)
	uBits = (0 << 28) | (1 << 24) | (0 << 20) | (0 << 16) | (1 << 12) | (1 << 8) | (5 << 4) | (1 << 0);
	Outp32(0x10030500, uBits);			// rCLK_DIV_CDREX

	// MCLK_EFPHY(1), C2C_CLK_400(1), MCLK_DPHY(1), MCLK_CDREX(1), BPLL(0)
	uBits = (1 << 16) | (1 << 12) | (1 << 8) | (1 << 4) | (0 << 0);
	Outp32(0x10030200, uBits);			// rCLK_SRC_CDREX

	// Setting BPLL  [P,M,S]
	//
	uBits = (1 << 21) | (3 << 12) | (8 << 8);
	Outp32(0x10030114, uBits);			// rBPLL_CON1

	// ENABLE(1), MDIV(100), PDIV(3), SDIV(0)
	uBits = (1u << 31) | (100 << 16) | (3 << 8) | (0 << 0);
	Outp32(0x10030110, uBits);			// rBPLL_CON0 	BPLL=800MHz(3:100:0)

	while ((Inp32(0x10030110) & (1 << 29)) == 0);

	// MCLK_EFPHY(1), C2C_CLK_400(1), MCLK_DPHY(1), MCLK_CDREX(1), BPLL(1)
	uBits = (1 << 16) | (1 << 12) | (1 << 8) | (1 << 4) | (1 << 0);
	Outp32(0x10030200, uBits);			// rCLK_SRC_CDREX
}

#define CA_SWAP				1
#define NUM_CHIP 			1
#define ZQ_MODE_DDS			7
#define PERFORM_LEVELING	0
#define PHY0_BASE			0x10C00000
#define PHY1_BASE			0x10C10000
#define DREX1_0				0x10C20000
#define DREX1_1				0x10C30000
#define TZASC_0				0x10c40000
#define TZASC_1				0x10c50000

#define CMU_COREPART			0x10010000
#define CMU_TOPPART			0x10020000
#define CMU_MEMPART			0x10030000

#define WAKEUP_STAT_OFFSET		0x0600
#define WAKEUP_MASK			0x80000000
#define WAKEUP_RESET			0
#define nRESET				1

void CA_swap_lpddr3(u32 DREX_address,u32 PHY_address)
{
	u32 data;

	// ca_swap_mode[0]=1
	//- DREX part
	data = Inp32( DREX_address+0x0000 );
	data=data&(~0x00000001);
	data=data|(0x00000001);
	Outp32( DREX_address+0x0000, data );

	//- PHY part
	// Added by cju, 2013.06.26, found that the code below is needed
	data = Inp32( PHY_address+0x0064 );
	data=data&(~0x00000001);
	data=data|(0x00000001);
	Outp32( PHY_address+0x0064, data );
}

void Low_frequency_init_lpddr3(u32 PHY_address, u32 DREX_address, u32 nMEMCLK)
{
	u32 wakeup_stat;
	u32 data;
	u32 data_RST_STAT;
	u32 rd_fetch=3;
	u32 update_mode=1;						//- CONCONTROL.update_mode[3]=0x1 : MC initiated update/acknowledge mode
	u32 i=0;
	u32 sw_n_warm_reset;

	// Reset Status Check..!
	wakeup_stat = readl(EXYNOS5260_POWER_BASE + WAKEUP_STAT_OFFSET);
	wakeup_stat &= WAKEUP_MASK;

	sw_n_warm_reset=(Inp32(0x10D50404)>>28)&0x3;			//- RESETSTATE(0x105C_0404) : [29]=SWRST,  [28]=WRESET
	if (wakeup_stat) {
		data_RST_STAT = WAKEUP_RESET;
	} else {
		data_RST_STAT = nRESET;
	}

	// 1. To provide stable power for controller and memory device, the controller must assert and hold CKE to a logic low level
	// 2. DRAM mode setting
	Outp32( PHY_address+0x0000, 0x17031A00 );	// PHY_CON0 ctrl_ddr_mode[12:11]=3(LPDDR3), ctrl_atgate	(automatic gate control-controller drive gate signals)[6]=1

	//- Setting rd_fetch and updated mode
	//¡Ú MC updated mode should be applied with PHY v5.
	data=Inp32( DREX_address+0x0000)&~((0x7<<12)|(0x1<<3));
	data|=((rd_fetch<<12)|(update_mode<<3));
	Outp32( DREX_address+0x0000, data);						 				//- CONCONTROL : rd_fetch[14:12], update_mode[3]

	if(1)
	{
		// 3. Force lock values (DLL cannot be locked under 400MHz)
		Outp32( PHY_address+0x0030, 0x10107F50|(0xF<<1) );	// PHY_CON12 ctrl_start_point	[30:24]=0x10, ctrl_inc[22:16]=0x10, ctrl_force[14:8]=0x7F, ctrl_start[6]=1, ctrl_dll_on[5]=0, ctrl_ref	[4:1]=0xF
		Outp32( PHY_address+0x0028, 0x0000007F );	// ctrl_offsetd[7:0]=0x7F

		// 4. Set ctrl_offsetr, crtrl_offsetw to 0x7F
		Outp32( PHY_address+0x0010, 0x7F7F7F7F );	// PHY_CON4 ctrl_offsetr, MEM1 Port 0, Read Leveling Manual Value, ¡Ú Best Tuning Value
		Outp32( PHY_address+0x0018, 0x7F7F7F7F );	// PHY_CON6 ctrl_offsetw, MEM1 Port 0, Write Training Manual Value

		// 5. set CA deskew code to 7h'60
		Outp32( PHY_address+0x0080, 0x0C183060 );	// PHY_CON31 DeSkew Code for CA
		Outp32( PHY_address+0x0084, 0x60C18306 );	// PHY_CON32 DeSkew Code for CA
		Outp32( PHY_address+0x0088, 0x00000030 );	// PHY_CON33 DeSkew Code for CA

		// Setting PHY_CON12 later
		// 6. Set ctrl_dll_on to 0
		// Outp32( PHY_address+0x0030, 0x10107F50); // PHY_CON12 ctrl_start_point	[30:24]=0x10, ctrl_inc[22:16]=0x10, ctrl_force[14:8]=0x7F, ctrl_start[6]=1, ctrl_dll_on[5]=0, ctrl_ref	[4:1]=0x8
		// DMC_Delay(100); // Wait for 10 PCLK cycles

		// 7. Issue dfi_ctrlupd_req for more than 10 cycles
		Outp32( DREX_address+0x0018, 0x00000008); // PHYCONTROL0 assert fp_resync[3]=1(Force DLL Resyncronization)
		// "dfi_ctrlupd_req" should be issued more than 10 cycles after ctrl_dll_on is disabled.
		DMC_Delay(100); // Wait for 10 PCLK cycles
		Outp32( DREX_address+0x0018, 0x00000000); // PHYCONTROL0 deassert fp_resync[3]=1(Force DLL Resyncronization)

		// 8. Set MemControl. At this moment, all power down modes should be off.
		Outp32( DREX_address+0x0004, 0x00312700); // MEMCONTROL bl[22:20]=Memory Burst Length 0x3 = 8, mem_width[15:12]=Width of Memory Data Bus 0x2 = 32-bit, mem_type	[11:8]=Type of Memory 0x7 = LPDDR3
	}
	#if 1							// Closed by cju,2013.01.16
	else
	{
		// 8. Set MemControl. At this moment, all power down modes should be off.
		Outp32( DREX_address+0x0004, 0x00312700); // MEMCONTROL bl[22:20]=Memory Burst Length 0x3 = 8, mem_width[15:12]=Width of Memory Data Bus 0x2 = 32-bit, mem_type	[11:8]=Type of Memory 0x7 = LPDDR3
	}
	#endif

	// 9. Set DRAM burst length and READ latency
	// 10. Set DRAM write latency
	if(nMEMCLK==921){
		Outp32( PHY_address+0x00AC, 0x0000080E); // PHY_CON42 ctrl_bstlen(Burst Length(BL))[12:8]=8, ctrl_rdlat(Read Latency(RL))[4:0]=12
		Outp32( PHY_address+0x006C, 0x0009000F); // PHY_CON26 T_wrdata_en[20:16]=WL for LPDDR2/LPDDR3
	}
	else if(nMEMCLK==825){
		Outp32( PHY_address+0x00AC, 0x0000080E); // PHY_CON42 ctrl_bstlen(Burst Length(BL))[12:8]=8, ctrl_rdlat(Read Latency(RL))[4:0]=12
		Outp32( PHY_address+0x006C, 0x0009000F); // PHY_CON26 T_wrdata_en[20:16]=WL for LPDDR2/LPDDR3
	}
	else if(nMEMCLK==800){
		Outp32( PHY_address+0x00AC, 0x0000080C); // PHY_CON42 ctrl_bstlen(Burst Length(BL))[12:8]=8, ctrl_rdlat(Read Latency(RL))[4:0]=12
		Outp32( PHY_address+0x006C, 0x0007000F); // PHY_CON26 T_wrdata_en[20:16]=WL for LPDDR2/LPDDR3
	}
	else if(nMEMCLK==667){
		Outp32( PHY_address+0x00AC, 0x0000080A); // PHY_CON42 ctrl_bstlen(Burst Length(BL))[12:8]=8, ctrl_rdlat(Read Latency(RL))[4:0]=12
		Outp32( PHY_address+0x006C, 0x0007000F); // PHY_CON26 T_wrdata_en[20:16]=WL for LPDDR2/LPDDR3
	}
	else{
		Outp32( PHY_address+0x00AC, 0x0000080A); // PHY_CON42 ctrl_bstlen(Burst Length(BL))[12:8]=8, ctrl_rdlat(Read Latency(RL))[4:0]=12
		Outp32( PHY_address+0x006C, 0x0007000F); // PHY_CON26 T_wrdata_en[20:16]=WL for LPDDR2/LPDDR3
	}

#if 1
	// Pulldn and Pullup enable
	// ctrl_pulld_dq[11:8]=Active HIGH signal to down DQ signals. For normal operation this field should be zero.
	// ctrl_pulld_dqs[3:0]=Active HIGH signal to pull-up or down PDQS/NDQS signals.
	Outp32( PHY_address+0x0038, 0x0000000F); // PHY_CON14 ctrl_pulld_dq[11:8]=0xf, ctrl_pulld_dqs[3:0]=0xf

	// ¡Ú ZQ Calibration
	// zq_mode_dds :: Driver strength selection. . It recommends one of the following settings instead of 3'h0.
// PHY_CON16 : zq_clk_en[27]=ZQ I/O Clock enable, zq_manual_mode[3:2]=Manual calibration mode selection 2'b01: long calibration, zq_manual_str[1]=Manual calibration start
// PHY_CON39 : Driver Strength
//-------------------------------------------------------------------------------------------------------
	if (ZQ_MODE_DDS == 7)
	{
		Outp32( PHY_address+0x0040, 0x0F040306);
		Outp32( PHY_address+0x00A0, 0x0FFF0FFF);
	}
	else if  (ZQ_MODE_DDS == 6)
	{
		Outp32( PHY_address+0x0040, 0x0E040306);
		Outp32( PHY_address+0x00A0, 0x0DB60DB6);
	}
	else if  (ZQ_MODE_DDS == 5)
	{
		Outp32( PHY_address+0x0040, 0x0D040306);
		Outp32( PHY_address+0x00A0, 0x0B6D0B6D);
	}
	else if  (ZQ_MODE_DDS == 4)
	{
		Outp32( PHY_address+0x0040, 0x0C040306);
		Outp32( PHY_address+0x00A0, 0x09240924);
	}
	else
	{
		Outp32( PHY_address+0x0040, 0x0F040306);
		Outp32( PHY_address+0x00A0 , 0x0FFF0FFF);
	}

	// Checking the completion of ZQ calibration
	do{
		if(i==2000000){													//- Just Delay
			i=0;
			break;
		}
		else					i++;
	}while( ( Inp32( PHY_address+0x0048 ) & 0x00000001 ) != 0x00000001 );	// PHY_CON17 zq_done[0]=ZQ Calibration is finished.

	// ZQ calibration exit
	data = Inp32( PHY_address+0x0040 );
	data = data&(~0x000FFFFF);
	data = data|0x00040304;
	Outp32( PHY_address+0x0040, data); // PHY_CON16 zq_mode_dds[26:24], zq_mode_term[23:21], zq_manual_start[1]=0
//-------------------------------------------------------------------------------------------------------
#endif

	// Disable DLL to initialize DFI --->   just  "ctrl_dll_on=0"
	Outp32( PHY_address+0x0030, 0x10107F50|(0xF<<1)); // PHY_CON12

// ★ DFI Initialization.
//-------------------------------------------------------------------------------------------------------
	// Asserting the dfi_init_start
	data=Inp32( DREX_address+0x0000)&~(0x1<<28);
	data|=(0x1<<28);
	Outp32( DREX_address+0x0000, data);			 							//- CONCONTROL : dfi_init_start=1

	// 6. Wait until dfi_init_complete become 1.
	i=0;
	do{										 //- Checking lock status.
		if(i==2000){
			i=0;
			break;
		}
		else					i++;
	}while( ( Inp32( DREX_address+0x0040 ) & 0x00000008 ) != 0x00000008 );	// PhyStatus dfi_init_complete[3]=1
	//Deasserting the dfi_init_start
	Outp32( DREX_address+0x0000, Inp32( DREX_address+0x0000)&~(0x1<<28));	//- CONCONTROL : dfi_init_start=0

#if 1
	// 7. ¡Ú fp_resync : Issue dfi_ctrlupd_req for more than 10 cycles
	// At this time, fp_resync is not necessarilly needed
	Outp32( DREX_address+0x0018, 0x00000008); // PHYCONTROL0 assert fp_resync[3]=1(Force DLL Resyncronization)
	// "dfi_ctrlupd_req" should be issued more than 10 cycles after ctrl_dll_on is disabled.
	DMC_Delay(100); // Wait for 10 PCLK cycles
	Outp32( DREX_address+0x0018, 0x00000000); // PHYCONTROL0 deassert fp_resync[3]=1(Force DLL Resyncronization)
#endif
//-------------------------------------------------------------------------------------------------------

	//if((status != S5P_CHECK_SLEEP) && (data_RST_STAT == 0))
	//if(eBootStat == nRESET) //((eBootStat != IRAMOFF_MIFOFF_TOPOFF) && (data_RST_STAT == 0))
	if((data_RST_STAT == nRESET)&&(!sw_n_warm_reset))			// That means that sleep & wakeup is not , or This is for all reset state
	{
		// Direct Command P0 CH0..!
		// 9. CKE low for tINIT1 and CKE high for tINIT3
		Outp32( DREX_address+0x0010, 0x07000000); // 0x7 = NOP (exit from active/precharge power down or deep power down,
		DMC_Delay(53333); // MIN 200us

		// 10. RESET command to LPDDR3
		// Add :: 2012.11.01 :: Not send reset command when occured by wake-up
		Outp32( DREX_address+0x0010, 0x00071C00); // 0x0 = MRS/EMRS (mode register setting), MR63_Reset (MA<7:0> = 3FH): MRW only

		// tINIT4(MIN 1us), tINIT5(MAX 10us)
		// 2013.04.15 :: Check DAI complete..!
		i=0;
		do{
			Outp32( DREX_address+0x0010, 0x09000000); // 0x9 = MRR (mode register reading), MR0_Device Information
			if(i==20000){									//- DAI status is complete within 10us.
				i=0;
				break;
			}
			else					i++;
		}while ((Inp32(DREX_address+0x0054) & (1 << 0)) != 0);	 // OP0=DAI (Device Auto-Initialization Status)

		// 12. DRAM ZQ calibration
		Outp32( DREX_address+0x0010, 0x00010BFC); // 0x0 = MRS/EMRS (mode register setting), MR10_Calibration, FFH: Calibration command after initialization
		// 13. Wait for minimum 1us (tZQINIT).
		DMC_Delay(267);	// MIN 1us

		if(nMEMCLK==921){
			// 13. DRAM parameter settings
			Outp32( DREX_address+0x0010, 0x00000870); // 0x0 = MRS/EMRS (mode register setting), MR2_Device Feature, 1B : Enable nWR programming > 9,Write Leveling(0)
			Outp32( DREX_address+0x0010, 0x0000060C); // 0x0 = MRS/EMRS (mode register setting), MR1_Device Feature, 011B: BL8, 010B: nWR=12
		}
		else if(nMEMCLK==825){
			// 13. DRAM parameter settings
			Outp32( DREX_address+0x0010, 0x00000870); // 0x0 = MRS/EMRS (mode register setting), MR2_Device Feature, 1B : Enable nWR programming > 9,Write Leveling(0)
			Outp32( DREX_address+0x0010, 0x0000060C); // 0x0 = MRS/EMRS (mode register setting), MR1_Device Feature, 011B: BL8, 010B: nWR=12
		}
		else if(nMEMCLK==800){
			// 13. DRAM parameter settings
			Outp32( DREX_address+0x0010, 0x00000868); // 0x0 = MRS/EMRS (mode register setting), MR2_Device Feature, 1B : Enable nWR programming > 9,Write Leveling(0)
			Outp32( DREX_address+0x0010, 0x0000050C); // 0x0 = MRS/EMRS (mode register setting), MR1_Device Feature, 011B: BL8, 010B: nWR=12
		}
		else if(nMEMCLK==667){
			// 13. DRAM parameter settings
			Outp32( DREX_address+0x0010, 0x00000860); // 0x0 = MRS/EMRS (mode register setting), MR2_Device Feature, 1B : Enable nWR programming > 9,Write Leveling(0)
			Outp32( DREX_address+0x0010, 0x0000040C); // 0x0 = MRS/EMRS (mode register setting), MR1_Device Feature, 011B: BL8, 010B: nWR=12
		}
		else{
			// 13. DRAM parameter settings
			Outp32( DREX_address+0x0010, 0x00000860); // 0x0 = MRS/EMRS (mode register setting), MR2_Device Feature, 1B : Enable nWR programming > 9,Write Leveling(0)
			Outp32( DREX_address+0x0010, 0x0000040C); // 0x0 = MRS/EMRS (mode register setting), MR1_Device Feature, 011B: BL8, 010B: nWR=12
		}

		// Add 20120501..!
		// 14. I/O Configuration :: Drive Strength
		Outp32( DREX_address+0x0010, 0x00000C04); // MR(3) OP(1)
		DMC_Delay(267);	// MIN 1us
	}
	else
	{
		Outp32( DREX_address+0x0010, 0x08000000 ); 				   //- CH0, CS0. Self Refresh Exit Command for only sleep & wakeup
	}



	// Initialization of second DRAM
	if(NUM_CHIP == 1)										//- NUM_CHIP=0 --> 1 chip,            NUM_CHIP=1 --> 2 chips
	{
		// if((eBootStat != IRAMOFF_MIFOFF_TOPOFF) && (data_RST_STAT == 0))
		//if(eBootStat == nRESET)
		if((data_RST_STAT == nRESET)&&(!sw_n_warm_reset))			// That means that sleep & wakeup is not , or This is for all reset state
		{
			#if 0
			SetBits(0x14000064, 4, 0x1, 0x1);
			#endif

			// 9. CKE low for tINIT1 and CKE high for tINIT3
			Outp32( DREX_address+0x0010, 0x07100000); // 0x7 = NOP (exit from active/precharge power down or deep power down,
			DMC_Delay(53333);	// MIN 200us

			// 10. RESET command to LPDDR3
			// Add :: 2012.11.01 :: Not send reset command when occured by wake-up
			Outp32( DREX_address+0x0010, 0x00171C00); // 0x0 = MRS/EMRS (mode register setting), MR63_Reset (MA<7:0> = 3FH): MRW only

			// tINIT4(MIN 1us), tINIT5(MAX 10us)
			// 2013.04.15 :: Check DAI complete..!
			i=0;
			do{
				Outp32( DREX_address+0x0010, 0x09100000); // 0x9 = MRR (mode register reading), MR0_Device Information
				if(i==20000){									//- DAI status is complete within 10us.
					i=0;
					break;
				}
				else					i++;
			}while ((Inp32(DREX_address+0x0054) & (1 << 0)) != 0);	 // OP0=DAI (Device Auto-Initialization Status)

			// 12. DRAM ZQ calibration
			Outp32( DREX_address+0x0010, 0x00110BFC); // 0x0 = MRS/EMRS (mode register setting), MR10_Calibration, FFH: Calibration command after initialization
			// 13. Wait for minimum 1us (tZQINIT).
			DMC_Delay(267);	// MIN 1us

			if(nMEMCLK==921){
				// 13. DRAM parameter settings
				Outp32( DREX_address+0x0010, 0x00100870); // 0x0 = MRS/EMRS (mode register setting), MR2_Device Feature, 1B : Enable nWR programming > 9,Write Leveling(0)
				Outp32( DREX_address+0x0010, 0x0010060C); // 0x0 = MRS/EMRS (mode register setting), MR1_Device Feature, 011B: BL8, 010B: nWR=12
			}
			else if(nMEMCLK==825){
				// 13. DRAM parameter settings
				Outp32( DREX_address+0x0010, 0x00100870); // 0x0 = MRS/EMRS (mode register setting), MR2_Device Feature, 1B : Enable nWR programming > 9,Write Leveling(0)
				Outp32( DREX_address+0x0010, 0x0010060C); // 0x0 = MRS/EMRS (mode register setting), MR1_Device Feature, 011B: BL8, 010B: nWR=12
			}
			else if(nMEMCLK==800){
				// 13. DRAM parameter settings
				Outp32( DREX_address+0x0010, 0x00100868); // 0x0 = MRS/EMRS (mode register setting), MR2_Device Feature, 1B : Enable nWR programming > 9,Write Leveling(0)
				Outp32( DREX_address+0x0010, 0x0010050C); // 0x0 = MRS/EMRS (mode register setting), MR1_Device Feature, 011B: BL8, 010B: nWR=12
			}
			else if(nMEMCLK==667){
				// 13. DRAM parameter settings
				Outp32( DREX_address+0x0010, 0x00100860); // 0x0 = MRS/EMRS (mode register setting), MR2_Device Feature, 1B : Enable nWR programming > 9,Write Leveling(0)
				Outp32( DREX_address+0x0010, 0x0010040C); // 0x0 = MRS/EMRS (mode register setting), MR1_Device Feature, 011B: BL8, 010B: nWR=12
			}
			else{
				// 13. DRAM parameter settings
				Outp32( DREX_address+0x0010, 0x00100860); // 0x0 = MRS/EMRS (mode register setting), MR2_Device Feature, 1B : Enable nWR programming > 9,Write Leveling(0)
				Outp32( DREX_address+0x0010, 0x0010040C); // 0x0 = MRS/EMRS (mode register setting), MR1_Device Feature, 011B: BL8, 010B: nWR=12
			}


			// Add 20120501..!
			// 14. I/O Configuration :: Drive Strength
			Outp32( DREX_address+0x0010, 0x00100C04); // MR(3) OP(1)
			DMC_Delay(267);	// MIN 1us
		}
		else
		{
			Outp32( DREX_address+0x0010, 0x08100000 );				   //- CH0, CS0. Self Refresh Exit Command for only sleep & wakeup
		}
	}

	if(1)//((eBootStat != IRAMOFF_MIFOFF_TOPOFF) && (data_RST_STAT == 0))
	{
		// Reset SDLL codes
		// 2012.10.11
		// Outp32( PHY_address+0x0028, 0x00000000); // PHY_CON10 ctrl_offsetd[7:0]=0x8
		Outp32( PHY_address+0x0028, 0x00000008); // PHY_CON10 ctrl_offsetd[7:0]=0x8

		// 4. Set ctrl_offsetr, crtrl_offsetw to 0x7F
		Outp32( PHY_address+0x0010, 0x08080808); // PHY_CON4 ctrl_offsetr, MEM1 Port 0, Read Leveling Manual Value, ¡Ú Best Tuning Value
		Outp32( PHY_address+0x0018, 0x08080808); // PHY_CON5 ctrl_offsetw, MEM1 Port 0, Write Training Manual Value

		// 5. set CA deskew code to 7h'60
		Outp32( PHY_address+0x0080, 0x00000000); // PHY_CON31 DeSkew Code for CA
		Outp32( PHY_address+0x0084, 0x00000000); // PHY_CON32 DeSkew Code for CA
		Outp32( PHY_address+0x0088, 0x00000000); // PHY_CON33 DeSkew Code for CA
	}

	return;
}

void High_frequency_init_lpddr3(u32 PHY_address, u32 DREX_address, u32 TZASC_address, u32 nMEMCLK)
{
#if defined(CONFIG_SMC_CMD)
	reg_arr_t reg_arr;
#endif
	u32 data,ch;
	u32 i=0;
	u32 tmp;
//	BOOT_STAT eBootStat;

//	eBootStat = GetRstStat();
	if (DREX_address == DREX1_0)
		ch =0;
	else
		ch =1;

	// 1. Set DRAM burst length and READ latency
	//Outp32( PHY_address+0x00AC, 0x0000080C); // PHY_CON42 ctrl_bstlen(Burst Length(BL))[12:8]=8, ctrl_rdlat(Read Latency(RL))[4:0]=12

	// 2. Set DRAM write latency
	//Outp32( PHY_address+0x006C, 0x0007000F); // PHY_CON26 T_wrdata_en[20:16]=WL for DDR3

	// DLL LOCK Setting..!
	// Set the DLL lock parameters
	// Reserved	[31] ctrl_start_point	[30:24]=0x10 Reserved	[23] ctrl_inc	[22:16]=0x10 ctrl_force	[14:8] ctrl_start	[6]=0x1 ctrl_dll_on	[5]=0x1 ctrl_ref	[4:1]=0x8 Reserved	[0]
	// Next Step : Same Operation "CONCONTROL dfi_init_start[28]=1"
	// 2012.10.11
	// Outp32( PHY_address+0x0030, 0x10100070); // PHY_CON12
	Outp32( PHY_address+0x0030, 0x10100010|(0xF<<1)); // PHY_CON12, "ctrl_dll_on[6] = 0", "ctrl_start[6] = 0"
	Outp32( PHY_address+0x0030, 0x10100030|(0xF<<1)); // PHY_CON12, "ctrl_dll_on[6] = 1", "ctrl_start[6] = 0"
	Outp32( PHY_address+0x0030, 0x10100070|(0xF<<1)); // PHY_CON12, "ctrl_dll_on[6] = 1", "ctrl_start[6] = 1"
	do{
		if(i==2000){													//- Just Delay
			i=0;
		   break;
                }
		else					i++;
	}while((Inp32(PHY_address+0x0034) & 0x5) != 0x5);	// ctrl_locked[0]=1

	// 7. ¡Ú¡Ú¡Ú fp_resync : Forcing DLL resynchronization - dfi_ctrlupd_req
	// ¡Ú¡Ú¡Ú At this time, fp_resync is necessarilly needed
	Outp32( DREX_address+0x0018, 0x00000008); 	// PhyControl0 ctrl_shgate[29]=1, fp_resync[3]=1
	DMC_Delay(20); // Wait for 10 PCLK cycles, PCLK(200MHz=10clock=50ns), DMC_Delay(40us)
	Outp32( DREX_address+0x0018, 0x00000000); 	// PhyControl0 ctrl_shgate[29]=1, fp_resync[3]=0

#if defined(CONFIG_SMC_CMD)
	reg_arr.set0.addr = TZASC_address+0x0F00;
	reg_arr.set0.val = 0x001007E0;
	reg_arr.set1.addr = TZASC_address+0x0F04;
	reg_arr.set1.val = 0x003007E0;
	reg_arr.set2.addr = TZASC_address+0x0F10;
	reg_arr.set2.val = 0x00462323;
	reg_arr.set3.addr = TZASC_address+0x0F14;
	reg_arr.set3.val = 0x00462323;
	set_secure_reg((u32)&reg_arr, 4);
#else
	// 2. Set the MemConfig0 register. If there are two external memory chips, also set the MemConfig1 register.
	// LPDDR2_P0_CS0 : 32'h2000_000 ~ 32'h27FF_FFFF (4Gbit)
	// Outp32( DREX_address+0x010C, 0x002007E0); // MemBaseConfig0 chip_base[26:16]=0x10, chip_mask[10:0]=0x7E0
	// Outp32( DREX_address+0x0110, 0x004007E0); // MemBaseConfig1 chip_base[26:16]=0x30, chip_mask[10:0]=0x7E0
	Outp32( TZASC_address+0x0F00, 0x001007E0); // MemBaseConfig0 chip_base[26:16]=0x10, chip_mask[10:0]=0x7E0
	Outp32( TZASC_address+0x0F04, 0x003007E0); // MemBaseConfig1 chip_base[26:16]=0x30, chip_mask[10:0]=0x7E0

	// 3. Set the MemConfig0
	// chip_map	[15:12]	Address Mapping Method (AXI to Memory). 0x1 = Interleaved ({row, bank, column, width})
	// chip_col	[11:8]	Number of Column Address Bits. 0x3 = 10 bits
	// chip_row	[7:4]	Number of Row Address Bits. 0x2 = 14 bits
	// chip_bank	[3:0]	Number of Banks. 0x3 = 8 banks
	// Outp32( DREX_address+0x0008, 0x00001323); // MemConfig0 chip_map	[15:12],chip_col	[11:8],chip_row	[7:4],chip_bank	[3:0]
	// Outp32( DREX_address+0x000C, 0x00001323); // MemConfig1 chip_map	[15:12],chip_col	[11:8],chip_row	[7:4],chip_bank	[3:0]
	Outp32( TZASC_address+0x0F10, 0x00462323); // MemConfig0 : bank_lsb[26:16], rank_inter_en[19], bit_sel_en[18], bit_sel[17:16], chip_map[15:12], chip_col[11:8], chip_row[7:4], chip_bank[3:0]
	Outp32( TZASC_address+0x0F14, 0x00462323); // MemConfig1 : bank_lsb[26:16], rank_inter_en[19], bit_sel_en[18], bit_sel[17:16], chip_map[15:12], chip_col[11:8], chip_row[7:4], chip_bank[3:0]
#endif
	// 4. Set the PrechConfig and PwrdnConfig registers.
	// 2013.04.18 :: DREX1_0_3 adding..!
	// Outp32( DREX_address+0x0014, 0xFF000000); // PrechConfig tp_cnt[31:24]=Timeout Precharge Cycles. 0xn = n cclk cycles. Refer to chapter 1.6.2 .Timeout precharge
	Outp32( DREX_address+0x0014, 0xF0000000); // PrechConfig tp_en[31:28]=Timeout Precharge per Port

	Outp32( DREX_address+0x001C, 0xFFFFFFFF);
	Outp32( DREX_address+0x0028, 0xFFFF00FF); // PwrdnConfig dsref_cyc[31:16]=Number of Cycles for dynamic self refresh entry. 0xn = n aclk cycles. Refer to chapter 1.5.3 . Dynamic self refresh

	// 5. Set the TimingAref, TimingRow, TimingData and TimingPower registers.
	//     according to memory AC parameters. At this moment, TimingData.w1 and TimingData.r1
	//     registers must be programmed according to initial clock frequency.
	// 2012.12.20 :: 3.875 us * 8192 row = 250us

	Outp32( DREX_address+0x0030, 0x0000002E); // TimingAref autorefresh counter @24MHz
	//Outp32( DREX_address+0x0030, g_AveragePeriodic_interval); // TimingAref autorefresh counter @24MHz


	// TimingAref autorefresh counter @24MHz
	// 2012.10.11
	// Outp32( DREX_address+0x0034, 0x34498611); // TimingRow
	// 2012.11.08 :: tRC : 0x18(24) ---> 0x1A(26), tRAS : 0x12(18) ---> 0x11(17)
	#if 0
		if (nMEMCLK==933) {
			Outp32( DREX_address+0x0034, 0x3D5A9794); // TimingRow
			Outp32( DREX_address+0x0038, 0x4742086E); // TimingData
			Outp32( DREX_address+0x003C, 0x60420447); // TimingPower
		}
		else if(nMEMCLK==800)
		{
			Outp32( DREX_address+0x0034, 0x34498691); // TimingRow
			Outp32( DREX_address+0x0038, 0x3630065C); // TimingData
			Outp32( DREX_address+0x003C, 0x50380336); // TimingPower
		}
		else
		{
			Outp32( DREX_address+0x0034, 0x3D5A9794); // TimingRow
			Outp32( DREX_address+0x0038, 0x4642065C); // TimingData
			Outp32( DREX_address+0x003C, 0x60420447); // TimingPower
		}
	#else
		if (nMEMCLK==921)							//- If max freq is 921MHz
		{
			Outp32( DREX_address+0x0034, 0x3C6BA7D5); // TimingRow
			Outp32( DREX_address+0x0038, 0x4740086E); // TimingData
			Outp32( DREX_address+0x003C, 0x60410447); // TimingPower
		}
		else if(nMEMCLK==825)						//- If max freq is 825MHz
		{
			Outp32( DREX_address+0x0034, 0x365A9713); // TimingRow
			Outp32( DREX_address+0x0038, 0x4740085E); // TimingData
			Outp32( DREX_address+0x003C, 0x543A0446); // TimingPower
		}
		else if(nMEMCLK==800)						//- If max freq is 825MHz
		{
			Outp32( DREX_address+0x0034, 0x345A96D3); // TimingRow
			Outp32( DREX_address+0x0038, 0x3630065C); // TimingData
			Outp32( DREX_address+0x003C, 0x50380336); // TimingPower
		}
		else if(nMEMCLK==667)						//- If max freq is 667MHz
		{
			Outp32( DREX_address+0x0034, 0x2C4885D0); // TimingRow
			Outp32( DREX_address+0x0038, 0x3630064A); // TimingData
			Outp32( DREX_address+0x003C, 0x442F0335); // TimingPower
		}
		else
		{
			Outp32( DREX_address+0x0034, 0x2C4885D0); // TimingRow
			Outp32( DREX_address+0x0038, 0x3630064A); // TimingData
			Outp32( DREX_address+0x003C, 0x442F0335); // TimingPower
		}
	#endif

	// If QoS scheme is required, set the QosControl10~15 and QosConfig0~15 registers.
	//-

	// 8. calibration & levelings
	if( PERFORM_LEVELING == 1)
	{
                #if 0
		Prepare_levelings_lpddr3( PHY_address, DREX_address);
		CA_calibration_lpddr3( PHY_address, DREX_address);
		Write_leveling_lpddr3( PHY_address, DREX_address);
		Read_leveling_lpddr3( PHY_address, DREX_address);
		Write_dq_leveling_lpddr3( PHY_address, DREX_address);
                #endif
	}

	//-----------------------------------------------
	//- end_levelings_lpddr3_l
	//-----------------------------------------------

	// ctrl_atgate = 0
	// T_WrWrCmd	[30:24] 	It controls the interval between Write and Write during DQ Calibration. This value should be always kept by 5'h17. It will be used for debug purpose.
	// T_WrRdCmd	[19:17] 	It controls the interval between Write and Read by cycle unit during Write Calibration. It will be used for debug purpose. 3'b111 : tWTR = 6 cycles (=3'b001)
	// ctrl_ddr_mode[12:11] 	2'b11: LPDDR3
	// ctrl_dfdqs[9] 	1¡¯b1: differential DQS
	Outp32( PHY_address+0x0000, 0x17021A00); // PHY_CON0 byte_rdlvl_en[13]=1, ctrl_ddr_mode[12:11]=01, ctrl_atgate[6]=1, Bit Leveling

	if( PERFORM_LEVELING == 1)
	{
		// dfi_ctrlupd_req to make sure ALL SDLL is updated
		// forcing DLL resynchronization - dfi_ctrlupd_req
		Outp32( DREX_address+0x0018, 0x00000008); // PhyControl0 ctrl_shgate[29]=1, fp_resync[3]=1
		DMC_Delay(20); // Wait for 10 PCLK cycles, PCLK(200MHz=10clock=50ns), DMC_Delay(40us)
		Outp32( DREX_address+0x0018, 0x00000000); // PhyControl0 ctrl_shgate[29]=1, fp_resync[3]=0
	}

	// 27. If power down modes are required, set the MemControl register.
	// bl[22:20]=Memory Burst Length 0x3 = 8
	// mem_width[15:12]=Width of Memory Data Bus 0x2 = 32-bit
	// mem_type[11:8]=Type of Memory 0x7 = LPDDR3
	// dsref_en[5]=Dynamic Self Refresh. 0x1 = Enable.
	// dpwrdn_en[1]=Dynamic Power Down. 0x1 = Enable
	// clk_stop_en[0]=Dynamic Clock Control. 0x1 = Stops during idle periods.

	Outp32( DREX_address+0x0004, 0x00312722); // MemControl bl[22:20]=8, mem_type[11:8]=7, two chip selection

	if(nMEMCLK==100)
	{
		// 3. Force lock values (DLL cannot be locked under 400MHz)
		Outp32( PHY_address+0x0030, 0x10107F50|(0xF<<1) );	// PHY_CON12 ctrl_start_point	[30:24]=0x10, ctrl_inc[22:16]=0x10, ctrl_force[14:8]=0x7F, ctrl_start[6]=1, ctrl_dll_on[5]=0, ctrl_ref	[4:1]=0xF
		Outp32( PHY_address+0x0028, 0x0000007F );	// ctrl_offsetd[7:0]=0x7F

		// 4. Set ctrl_offsetr, crtrl_offsetw to 0x7F
		Outp32( PHY_address+0x0010, 0x7F7F7F7F );	// PHY_CON4 ctrl_offsetr, MEM1 Port 0, Read Leveling Manual Value, ¡Ú Best Tuning Value
		Outp32( PHY_address+0x0018, 0x7F7F7F7F );	// PHY_CON6 ctrl_offsetw, MEM1 Port 0, Write Training Manual Value

		// 5. set CA deskew code to 7h'60
		Outp32( PHY_address+0x0080, 0x0C183060 );	// PHY_CON31 DeSkew Code for CA
		Outp32( PHY_address+0x0084, 0x60C18306 );	// PHY_CON32 DeSkew Code for CA
		Outp32( PHY_address+0x0088, 0x00000030 );	// PHY_CON33 DeSkew Code for CA

		//Outp32( PHY_address+0x0030, 0x10107F10|(0xF<<1) );	// PHY_CON12 ctrl_start_point	[30:24]=0x10, ctrl_inc[22:g16]=0x10, ctrl_force[14:8]=0x7F, ctrl_start[6]=1, ctrl_dll_on[5]=0, ctrl_ref	[4:1]=0x8

		// Setting PHY_CON12 later
		// 6. Set ctrl_dll_on to 0
		// Outp32( PHY_address+0x0030, 0x10107F50); // PHY_CON12 ctrl_start_point	[30:24]=0x10, ctrl_inc[22:16]=0x10, ctrl_force[14:8]=0x7F, ctrl_start[6]=1, ctrl_dll_on[5]=0, ctrl_ref	[4:1]=0x8
		// DMC_Delay(100); // Wait for 10 PCLK cycles

	#if 1
		// 7. ¡Úfp_resync : Issue dfi_ctrlupd_req for more than 10 cycles
		Outp32( DREX_address+0x0018, 0x00000008); // PHYCONTROL0 assert fp_resync[3]=1(Force DLL Resyncronization)
		// "dfi_ctrlupd_req" should be issued more than 10 cycles after ctrl_dll_on is disabled.
		DMC_Delay(100); // Wait for 10 PCLK cycles
		Outp32( DREX_address+0x0018, 0x00000000); // PHYCONTROL0 deassert fp_resync[3]=1(Force DLL Resyncronization)
	#endif

	}

	return;
}

static void mem_ctrl_init_lpddr3(u32 nMEMCLK)
{
	u32 data;
	u32 io_pd_con=2;
	u32 aref_en=1;
//	BOOT_STAT eBootStat;
	u32 nLoop;
	u32 nCnt;
	u32 uMif_Divider;

//	eBootStat = GetRstStat();

	/****************************************/
	/*****	                    CA SWAP                   *****/
	/****************************************/
	if (CA_SWAP == 1)
	{
		CA_swap_lpddr3(DREX1_0, PHY0_BASE);
		CA_swap_lpddr3(DREX1_1, PHY1_BASE);
	}

#if 0
	Outp32( 0x10C20000+0x0000, 0x1FFF1101 );	//- DREX0 assert dfi_init_start
	DMC_Delay(0x100); //- wait 1ms
	Outp32( 0x10C20000+0x0000, 0xFFF1101 );	//- DREX0 deassert dfi_init_start
	Outp32( 0x10C30000+0x0000, 0x1FFF1101 );	//- DREX1 assert dfi_init_start
	DMC_Delay(0x100); //- wait 1ms
	Outp32( 0x10C30000+0x0000, 0xFFF1101 );	//- DREX1 deassert dfi_init_start
#endif
	// Remove because handshaking between DREX and PHY when operate in low frequency(24MHz)
	// DLL LOCK Setting..!
	// DLL_lock_lpddr3(PHY0_BASE, DREX1_0);
	// DLL_lock_lpddr3(PHY1_BASE, DREX1_1);

	// Remove because handshaking between DREX and PHY when operate in low frequency(24MHz)
	// CMU Setting..!
	// Clock = 50MHz
	// Outp32( CMU_MEMPART+0x0114, 0x0020F300); // BPLL_CON1
	// Outp32( CMU_MEMPART+0x0110, 0x80C80305); // BPLL_CON0
	// DMC_Delay(100);

	/****************************************/
	/*****   CLOCK Pause Enable               *****/
	/****************************************/
	// Pause Enable..!
	data = Inp32( 0x10CE1004);
	data = data | (0x1<<0);
	Outp32(0x10CE1004, data);

	/****************************************/
	/*****   CLOCK SETTTING               *****/
	/****************************************/


	uMif_Divider=Inp32(0x10CE0600)&0xFFF;
	uMif_Divider=(uMif_Divider|(0x7<<28)|(0x3<<24)|(0x1<<20)|(0xf<<16)|(0x0<<12));
        //&CLKM_PHY_RATIO &CLK2X_PHY_RATIO &ACLK_MIF_466_RATIO &ACLK_BUS_200_RATIO &ACLK_BUS_100_RATIO
	Outp32(0x10CE0600,uMif_Divider);

	//CMU_InitForMif(50); //Max Frequency Setting


	/****************************************/
	/*****	     LOW FREQUENCY            *****/
	/****************************************/
	// PHY0+DREX1_0
	Low_frequency_init_lpddr3(PHY0_BASE, DREX1_0, nMEMCLK);
	// PHY1+DREX1_1
	Low_frequency_init_lpddr3(PHY1_BASE, DREX1_1, nMEMCLK);

	/****************************************/
	/*****	  CLOCK SETTTING              *****/
	/****************************************/
	uMif_Divider=Inp32(0x10CE0600)&0xFFF;
	uMif_Divider=(uMif_Divider|(0x7<<28)|(0x3<<24)|(0x1<<20)|(0x0<<16)|(0x0<<12)); //Max CLK Setting
        //&CLKM_PHY_RATIO &CLK2X_PHY_RATIO &ACLK_MIF_466_RATIO &ACLK_BUS_200_RATIO &ACLK_BUS_100_RATIO
	Outp32(0x10CE0600,uMif_Divider);

	/****************************************/
	/*****	              HIGH FREQUENCY              *****/
	/****************************************/
	// PHY0+DREX1_0
	High_frequency_init_lpddr3(PHY0_BASE, DREX1_0, TZASC_0, nMEMCLK);
	// PHY1+DREX1_1
	High_frequency_init_lpddr3(PHY1_BASE, DREX1_1, TZASC_1, nMEMCLK);


	#if 1	// Move..! 2012.11.30
	// 26. Set the ConControl to turn on an auto refresh counter.
	// aref_en[5]=Auto Refresh Counter. 0x1 = Enable
	// 2012.11.08 :: rd_fetch 3 -> 2
	// 2013.04.12 :: Automatic control for ctrl_pd in none read state
	data=Inp32( DREX1_0+0x0000)&~((0x3<<6)|(0x1<<5));
	data|=((io_pd_con<<6)|(aref_en<<5));
	Outp32(DREX1_0+0x0000, data);					//- CH0 : CONCONTROL : io_pd_con[7:6]=2, aref_en[5]=1

	data=Inp32( DREX1_1+0x0000)&~((0x3<<6)|(0x1<<5));
	data|=((io_pd_con<<6)|(aref_en<<5));
	Outp32(DREX1_1+0x0000, data);
	#endif


	//- CH0 : CONCONTROL : io_pd_con[7:6]=2, aref_en[5]=1
	#if 0
	Outp32( DREX1_0+0x0000, 0x0FFF31A1); // CONCONTROL aref_en[5]=1
	Outp32( DREX1_1+0x0000, 0x0FFF31A1); // CONCONTROL aref_en[5]=1
	#endif

	//- Cck Gating Control Register..!
	#if 1
		Outp32( DREX1_0+0x0018, 0x00005000); //to resolve Phy Gating Problem.
		Outp32( DREX1_1+0x0018, 0x00005000);

		#if 0	//- In case of EVT0
			Outp32( DREX1_0+0x0008, 0x00000010);					//- CH0 : Only Support Phy CLK Gating
			Outp32( DREX1_1+0x0008, 0x00000010);					//- CH1 : Only Support Phy CLK Gating
		#else		//- In case of EVT1
			Outp32( DREX1_0+0x0008, 0x0000003F);					//- CH0 : CG Control : tz_cg_en[5], phy_cg_en[4], memif_cg_en[3], scg_cg_en[2], busif_wr_cg_en[1], busif_rd_cg_en[0]
			Outp32( DREX1_1+0x0008, 0x0000003F);					//- CH1 : CG Control : tz_cg_en[5], phy_cg_en[4], memif_cg_en[3], scg_cg_en[2], busif_wr_cg_en[1], busif_rd_cg_en[0]
		#endif
	#endif

	return;
}

void mem_ctrl_init()
{
	/* init_mif */
	mem_ctrl_init_lpddr3(667);
}
