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

#define dmcOutp32(a, d) Outp32(g_uBaseAddr+a, d)
#define dmcInp32(a) Inp32(g_uBaseAddr+a)

typedef enum {
  nRESET = 0x10,                        // Booting sequence #9
  IRAMON_COREON_TOPON    = 0x7,         // Booting sequence #1
  IRAMOFF_COREON_TOPON   = 0x6,         // Booting sequence #2
  IRAMON_COREOFF_TOPON   = 0x5,         // Booting sequence #3
  IRAMOFF_COREOFF_TOPON  = 0x4,         // Booting sequence #4
  IRAMON_COREON_TOPOFF   = 0x3,         // Booting sequence #5
  IRAMOFF_COREON_TOPOFF  = 0x2,         // Booting sequence #6
  IRAMON_COREOFF_TOPOFF  = 0x1,         // Booting sequence #7
  IRAMOFF_COREOFF_TOPOFF = 0x0          // Booting sequence #8
}BOOT_STAT;

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



static void DMC_Delay(u32 x)
{
    while(--x)
		__asm ("NOP");
}

void DMC_RdCalibration(void)
{
	u32 phyBase;
	u32 code, lock, vwmc;
	int dq0, dq1, dq2, dq3, ch;

	for( ch = 0; ch < 2; ch++ ) {

		phyBase = 0x10c00000+(0x10000 * ch);

		Outp32( phyBase + 0x0004, 0x92100FF );	//- Set PHY0.CON1.rdlvl_rddata_adj

		Outp32( phyBase + 0x005C, 0x00000041 );	//- PHY0.CON22.lpddr2_addr=0x041

		Outp32( phyBase + 0x0008, 0x2010044 );	//- Set PHY0.CON2.rdlvl_en
	}

	Outp32( 0x10DD0000+0x00F8, 0x2 );	//- DMC.RDLVLCONFIG.ctrl_rdlvl_data_en=1

	while( ( Inp32( 0x10DD0000+0x0040 ) & 0xC000 ) != 0xC000 );	//- Wait DMC.rdlvl_complete_ch0/1
	Outp32( 0x10DD0000+0x00F8, 0x0 );	//- Set DMC.RDLVLCONFIG.ctrl_rdlvl_data_en=0

	for(ch = 0; ch < 2; ch ++ ) {

		Outp32( phyBase + 0014, 0x8);
		vwmc = Inp32(phyBase + 0x0050);

		lock = Inp32( phyBase + 0x0034 );
		lock &= 0x1FF00;
		lock >>= 8;

		if( (lock & 0x3) == 0x3 ) {
			lock++;
		}

		dq0 = (vwmc & 0xFF) - (lock >> 2);
		dq1 = ((vwmc >> 8) & 0xFF) - (lock >> 2);
		dq2 = ((vwmc >> 16) & 0xFF) - (lock >> 2);
		dq3 = ((vwmc >> 24) & 0xFF) - (lock >> 2);

		if(dq0 < 0) {
			dq0 *= -1;
			dq0 |= 0x80;
		}

		if(dq1 < 0) {
			dq1 *= -1;
			dq1 |= 0x80;
		}

		if(dq2 < 0) {
			dq2 *= -1;
			dq2 |= 0x80;
		}

		if(dq3 < 0) {
			dq3 *= -1;
			dq3 |= 0x80;
		}

		code = ((dq3 & 0xFF) << 24) | ((dq2 & 0xFF) << 16) |
			   ((dq1 & 0xFF) << 8) | (dq0 & 0xFF);

		Outp32( phyBase + 0x0010, code );

		Outp32( phyBase + 0x0008, Inp32( phyBase + 0x0008) & 0xFDFFFFFF );
	}
}

void DMC_WrCalibration(void)
{
	//-Write DQ Calibration.
	while( ( Inp32( 0x10DD0000+0x0048 ) & 0x3 ) != 0x0 );	//- Wait for DMC.chip_busy_state CH0
	while( ( Inp32( 0x10DD0000+0x004C ) & 0x3 ) != 0x0 );	//- Wait for DMC.chip_busy_state CH1
	Outp32( 0x10DD0000+0x00F4, 0x1 );	//- DMC.WRTRACONFIG
	Outp32( 0x10C00000+0x005C, 0x204 );	//-
	Outp32( 0x10C10000+0x005C, 0x204 );	//-
	Outp32( 0x10C00000+0x0004, 0x9210001 );	//-
	Outp32( 0x10C10000+0x0004, 0x9210001 );	//-
	Outp32( 0x10C00000+0x0008, 0x6010044 );	//-
	Outp32( 0x10C10000+0x0008, 0x6010044 );	//-
	Outp32( 0x10C00000+0x0008, 0xE010044 );	//-
	Outp32( 0x10C10000+0x0008, 0xE010044 );	//-
	while( ( Inp32( 0x10DD0000+0x0040 ) & 0xC000 ) != 0xC000 );	//- Wait DMC.rdlvl_complete_ch0/1
	Outp32( 0x10C00000+0x0008, 0x6010044 );	//-
	Outp32( 0x10C10000+0x0008, 0x6010044 );	//-
}

void DMC_CaTraining(int ch)
{
	unsigned char code;
	int find_vmw;
	unsigned int phyBase;
	unsigned int ioRdOffset;
	unsigned int temp, mr41, mr48, vwml, vwmr, vwmc;
	unsigned int lock;


	phyBase = 0x10c00000+(0x10000 * ch);
	ioRdOffset = 0x150 + (0x4 * ch);

	temp = Inp32( phyBase + 0x0000 );
	temp |= (1 << 16);
	Outp32( phyBase + 0x0000, temp );

	temp = Inp32( phyBase + 0x0008 );
	temp |= (1 << 23);
	Outp32( phyBase + 0x0008, temp );

	code = 0x8;
	find_vmw = 0;
	vwml = vwmr = vwmc = 0;

	while (1) {

		//- code update
		temp = Inp32( phyBase + 0x0028 );
		temp &= 0xFFFFFF00;
		temp |= code;
		Outp32( phyBase + 0x0028, temp );

		//- resync
		temp = Inp32( phyBase + 0x0028 );
		temp &= 0xFEFFFFFF;
		Outp32( phyBase + 0x0028, temp );
		temp |= 0x01000000;
		Outp32( phyBase + 0x0028, temp );
		temp &= 0xFEFFFFFF;
		Outp32( phyBase + 0x0028, temp );

		if(ch == 0) {
			Outp32( 0x10DD0000+0x0010, 0x50690 );	//- Send MRW: MA=0x29 OP=0xA4, 0x50690
			//Outp32( 0x10DD0000+0x0010, 0x001050690 );	//- Send MRW: MA=0x29 OP=0xA4, 0x50690
		} else {
			Outp32( 0x10DD0000+0x0010, 0x10050690 );	//- Send MRW: MA=0x29 OP=0xA4, 0x10050690
			//Outp32( 0x10DD0000+0x0010, 0x10150690 );	//- Send MRW: MA=0x29 OP=0xA4, 0x10050690
		}

		Outp32( 0x10DD0000+0x0160, 0x3FF011 );	//- Set DMC.CACAL_CONFIG0.deassert_cke=1
		Outp32( 0x10DD0000+0x0164, 0x1 );		//- Set DMC.CACAL_CONFIG1.cacal_csn=1
		DMC_Delay(0x100);

		mr41 = Inp32( 0x10DD0000 + ioRdOffset );
		mr41 &= 0xFFFF;

		Outp32( 0x10DD0000+0x0160, 0x3FF010 );	//- Set DMC.CACAL_CONFIG0.deassert_cke=0
		DMC_Delay(0x100);

		if( ch == 0 ) {
			Outp32( 0x10DD0000+0x0010, 0x60300 );	//- Send MRW: MA=0x30 OP=0xC0, 0x60300
			//Outp32( 0x10DD0000+0x0010, 0x001060300 );	//- Send MRW: MA=0x30 OP=0xC0, 0x60300
		} else {
			Outp32( 0x10DD0000+0x0010, 0x10060300 );	//- Send MRW: MA=0x30 OP=0xC0, 0x10060300
			//Outp32( 0x10DD0000+0x0010, 0x10160300 );	//- Send MRW: MA=0x30 OP=0xC0, 0x10060300
		}

		Outp32( 0x10DD0000+0x0160, 0x3FF011 );	//- Set DMC.CACAL_CONFIG0.deassert_cke=1
		Outp32( 0x10DD0000+0x0164, 0x1 );		//- Set DMC.CACAL_CONFIG1.cacal_csn=1
		DMC_Delay(0x100);

		mr48 = Inp32( 0x10DD0000 + ioRdOffset );
		mr48 &= 0x0303;

		Outp32( 0x10DD0000+0x0160, 0x3FF010 );	//- Set DMC.CACAL_CONFIG0.deassert_cke=0
		DMC_Delay(0x100);

		if( (find_vmw == 0) && (mr41 == 0x5555 ) && ( mr48 == 0x0101 ) ) {
			find_vmw = 0x1;
			vwml = code;
		}

		if( (find_vmw == 1) && ( (mr41 != 0x5555 ) || ( mr48 != 0x0101 ) ) ) {
			find_vmw = 0x3;
			vwmr = code - 1;

			if( ch == 0 ) {
				Outp32( 0x10DD0000+0x0010, 0x50AA0 );	//- Send MRW: MA=0x2A OP=0xA8, 0x50AA0
				//Outp32( 0x10DD0000+0x0010, 0x001050AA0 );	//- Send MRW: MA=0x2A OP=0xA8, 0x50AA0
			} else {
				Outp32( 0x10DD0000+0x0010, 0x10050AA0 );	//- Send MRW: MA=0x2A OP=0xA8, 0x50AA0
				//Outp32( 0x10DD0000+0x0010, 0x10150AA0 );	//- Send MRW: MA=0x2A OP=0xA8, 0x50AA0
			}
			//DMC_Delay(0x10000);
			break;
		}

		code++;

		if(code == 255) {
			while(1);
		}
	}

	vwmc = (vwml + vwmr) / 2;

#if 1
	{
		u32 lock_force;
		u32 temp = 0;

		lock_force = (Inp32( phyBase + 0x30 ) >> 8) & 0x7F;

		temp = ((vwml & 0xFF) << 16) |
			   ((vwmr & 0xFF) << 8) |
			   ((vwmc & 0xFF));

		if(ch == 0) {
			Outp32(0x10040818, temp);
		}
		else {
			Outp32(0x1004081C, temp);
		}
	}
#endif

	//- code update
	temp = Inp32( phyBase + 0x0028 );
	temp &= 0xFFFFFF00;
	temp |= vwmc;
	Outp32( phyBase + 0x0028, temp );

	//- resync
	temp = Inp32( phyBase + 0x0028 );
	temp &= 0xFEFFFFFF;
	Outp32( phyBase + 0x0028, temp );
	temp |= 0x01000000;
	Outp32( phyBase + 0x0028, temp );
	temp &= 0xFEFFFFFF;
	Outp32( phyBase + 0x0028, temp );

	temp = Inp32( phyBase+0x0000 );
	temp &= 0xFFFEFFFF;
	Outp32( phyBase + 0x0000, temp );

#if 1

	//- vmwc convert to offsetd value.

	lock = Inp32( phyBase + 0x0034 );
	lock &= 0x1FF00;
	lock >>= 8;

	if( (lock & 0x3) == 0x3 ) {
		lock++;
	}

	code = vwmc - (lock >> 2);

	temp = Inp32( phyBase + 0x0028 );
	temp &= 0xFFFFFF00;
	temp |= code;
	Outp32( phyBase + 0x0028, temp );

	temp = Inp32( phyBase + 0x0008 );
	temp &= 0xFF7FFFFF;
	Outp32( phyBase + 0x0008, temp );
#endif
}

static void DMC_ZqInit(u8 dq, u8 ck, u8 cke, u8 cs, u8 ca)
{
	u32 temp;
	u32 zqBase;
	int ch;

	for( ch = 0; ch < 2; ch++ ) {

		zqBase = 0x10c00000 + ( 0x10000 * ch );

		temp = Inp32( zqBase + 0x40 );
		temp &= 0xF8FBFFF1;
		temp |= ( ( dq & 0x7 ) << 24 );
		temp |= ( ( 1 << 18 ) | ( 1 << 2 ) );

		Outp32( zqBase + 0x40, temp );

		temp |= (1 << 1);

		Outp32( zqBase + 0x40, temp );

		while( ( Inp32( zqBase + 0x48 ) & 0x5 ) != 0x1 );

		temp = Inp32( zqBase + 0x40 );

		temp &= ~( 1 << 18 );

		Outp32( zqBase + 0x40, temp );

		temp = ( ( ck & 0x7 ) << 9 ) | ( ( cke & 0x7 ) << 6 ) |
			   ( ( cs & 0x7 ) << 3 ) | ( ca & 0x7 );

		Outp32( zqBase + 0xA0, temp );
	}
}

#define CA_SWAP				0
#define NUM_CHIP 			1
#define ZQ_MODE_DDS			6
#define PERFORM_LEVELING	0
#define PHY0_BASE			0x10C00000
#define PHY1_BASE			0x10C10000
#define DREX1_0				0x10C20000
#define DREX1_1				0x10C30000
#define CMU_COREPART		0x10010000
#define CMU_TOPPART			0x10020000
#define CMU_MEMPART			0x10030000
#define TZASC_0				0x10D40000
#define TZASC_1				0x10D50000
#define USED_DYNAMIC_AUTO_CLOCK_GATING 1

#define D25_3GB				0
#define CONFIG_MEMCLK 			825



void CA_swap_lpddr3(u32 DREX_address)
{
	u32 data;

	// ca_swap_mode[0]=1
	data = Inp32( DREX_address+0x0000 );
	data=data&(~0x00000001);
	data=data|(0x00000001);
	Outp32( DREX_address+0x0000, data );

	return;
}




void Low_frequency_init_lpddr3(u32 PHY_address, u32 DREX_address)
{
	u32 data;

	// 1. To provide stable power for controller and memory device, the controller must assert and hold CKE to a logic low level
	// 2. DRAM mode setting
	Outp32( PHY_address+0x0000, 0x17021A00 );	// PHY_CON0 ctrl_ddr_mode[12:11]=3(LPDDR3), ctrl_atgate	(automatic gate control-controller drive gate signals)[6]=1

	// 3. Force lock values (DLL cannot be locked under 400MHz)
	Outp32( PHY_address+0x0030, 0x10107F50 );	// PHY_CON12 ctrl_start_point	[30:24]=0x10, ctrl_inc[22:16]=0x10, ctrl_force[14:8]=0x7F, ctrl_start[6]=1, ctrl_dll_on[5]=0, ctrl_ref	[4:1]=0x8 
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

	// Start :: Adding option for handshaking between DREX and PHY
	// Deasserting the dfi_init_start
	// 2012.11.08 :: rd_fetch 3 -> 2
	// 2013.05.14 :: rd_fetch 3@933MHz
	Outp32( DREX_address+0x0000, 0x1FFF3100); // CONCONTROL dfi_init_start[28]=0 auto refresh not yet.

	// Disable DLL
	Outp32( PHY_address+0x0030, 0x10107F10); // PHY_CON12
	// End :: Adding option for handshaking between DREX and PHY

	// Direct Command P0 CH0..!
	// 9. CKE low for tINIT1 and CKE high for tINIT3
	Outp32( DREX_address+0x0010, 0x07000000); // 0x7 = NOP (exit from active/precharge power down or deep power down,

	#if 0
	SetBits(0x14000060, 0, 0xF, 0x1);
	Outp32(0x14000068, 0x0);
	SetBits(0x14000064, 0, 0x1, 0x1);
	#endif

	DMC_Delay(53333); // MIN 200us
	DMC_Delay(53333); // MIN 200us
	DMC_Delay(53333); // MIN 200us
	DMC_Delay(53333); // MIN 200us

	#if 0
	SetBits(0x14000064, 0, 0x1, 0x0);
	#endif

	// 10. RESET command to LPDDR3
	// Add :: 2012.11.01 :: Not send reset command when occured by wake-up
	Outp32( DREX_address+0x0010, 0x00071C00); // 0x0 = MRS/EMRS (mode register setting), MR63_Reset (MA<7:0> = 3FH): MRW only

	// tINIT4(MIN 1us), tINIT5(MAX 10us)
	// 2013.04.15 :: Check DAI complete..!
	DMC_Delay(267);	// MIN 1us
	do{
		Outp32( DREX_address+0x0010, 0x09000000); // 0x9 = MRR (mode register reading), MR0_Device Information
	}while ((Inp32(DREX_address+0x0054) & (1 << 0)) != 0);	 // OP0=DAI (Device Auto-Initialization Status)

	// 12. DRAM ZQ calibration
	Outp32( DREX_address+0x0010, 0x00010BFC); // 0x0 = MRS/EMRS (mode register setting), MR10_Calibration, FFH: Calibration command after initialization
	// 13. Wait for minimum 1us (tZQINIT).

	#if 0
	SetBits(0x14000064, 0, 0x1, 0x1);
	#endif

	DMC_Delay(267);	// MIN 1us
	DMC_Delay(267);	// MIN 1us
	DMC_Delay(267);	// MIN 1us
	DMC_Delay(267);	// MIN 1us

	#if 0
	SetBits(0x14000064, 0, 0x1, 0x0);
	#endif

	// 13. DRAM parameter settings
	Outp32( DREX_address+0x0010, 0x0000050C); // 0x0 = MRS/EMRS (mode register setting), MR1_Device Feature, 011B: BL8, 010B: nWR=12

	// 2012.10.11
	// Outp32( DREX_address+0x0010, 0x00000828); // 0x0 = MRS/EMRS (mode register setting), MR2_Device Feature, 1B : Enable nWR programming > 9,Write Leveling(0)
	Outp32( DREX_address+0x0010, 0x00000868); // 0x0 = MRS/EMRS (mode register setting), MR2_Device Feature, 1B : Enable nWR programming > 9,Write Leveling(0)

	// Add 20120501..!
	// 14. I/O Configuration :: Drive Strength
	Outp32( DREX_address+0x0010, 0x00000C04); // MR(3) OP(1)

	DMC_Delay(267);	// MIN 1us
	DMC_Delay(267);	// MIN 1us
	DMC_Delay(267);	// MIN 1us
	DMC_Delay(267);	// MIN 1us

	// Initialization of second DRAM
	if(NUM_CHIP == 1)
	{
		#if 0
		SetBits(0x14000064, 4, 0x1, 0x1);
		#endif

		// 9. CKE low for tINIT1 and CKE high for tINIT3
		Outp32( DREX_address+0x0010, 0x07100000); // 0x7 = NOP (exit from active/precharge power down or deep power down,

		DMC_Delay(53333);	// MIN 200us
		DMC_Delay(53333);	// MIN 200us
		DMC_Delay(53333);	// MIN 200us
		DMC_Delay(53333);	// MIN 200us

		// 10. RESET command to LPDDR3
		// Add :: 2012.11.01 :: Not send reset command when occured by wake-up
		Outp32( DREX_address+0x0010, 0x00171C00); // 0x0 = MRS/EMRS (mode register setting), MR63_Reset (MA<7:0> = 3FH): MRW only

		// tINIT4(MIN 1us), tINIT5(MAX 10us)
		// 2013.04.15 :: Check DAI complete..!
		DMC_Delay(267);	// MIN 1us
		do{
			Outp32( DREX_address+0x0010, 0x09100000); // 0x9 = MRR (mode register reading), MR0_Device Information
		}while ((Inp32(DREX_address+0x0054) & (1 << 0)) != 0);	 // OP0=DAI (Device Auto-Initialization Status)

		// 12. DRAM ZQ calibration
		Outp32( DREX_address+0x0010, 0x00110BFC); // 0x0 = MRS/EMRS (mode register setting), MR10_Calibration, FFH: Calibration command after initialization

		// 13. Wait for minimum 1us (tZQINIT).
		DMC_Delay(267);	// MIN 1us
		DMC_Delay(267);	// MIN 1us
		DMC_Delay(267);	// MIN 1us
		DMC_Delay(267);	// MIN 1us

		// 13. DRAM parameter settings
		Outp32( DREX_address+0x0010, 0x0010050C); // 0x0 = MRS/EMRS (mode register setting), MR1_Device Feature, 011B: BL8, 010B: nWR=12
		Outp32( DREX_address+0x0010, 0x00100868); // 0x0 = MRS/EMRS (mode register setting), MR2_Device Feature, 1B : Enable nWR programming > 9,Write Leveling(0)

		// Add 20120501..!
		// 14. I/O Configuration :: Drive Strength
		Outp32( DREX_address+0x0010, 0x00100C04); // MR(3) OP(1)

		DMC_Delay(267);	// MIN 1us
		DMC_Delay(267);	// MIN 1us
		DMC_Delay(267);	// MIN 1us
		DMC_Delay(267);	// MIN 1us
	}

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

	return;
}

void Low_frequency_init_lpddr3_ReInit(u32 PHY_address, u32 DREX_address)
{
	u32 data;

	// 1. To provide stable power for controller and memory device, the controller must assert and hold CKE to a logic low level
	// 2. DRAM mode setting
	Outp32( PHY_address+0x0000, 0x17021A00 );	// PHY_CON0 ctrl_ddr_mode[12:11]=3(LPDDR3), ctrl_atgate	(automatic gate control-controller drive gate signals)[6]=1

	// 8. Set MemControl. At this moment, all power down modes should be off.
	Outp32( DREX_address+0x0004, 0x00312700); // MEMCONTROL bl[22:20]=Memory Burst Length 0x3 = 8, mem_width[15:12]=Width of Memory Data Bus 0x2 = 32-bit, mem_type	[11:8]=Type of Memory 0x7 = LPDDR3

	return;
}

void High_frequency_init_lpddr3(u32 PHY_address, u32 DREX_address, u32 TZASC_address, u32 nMEMCLK)
{
#if defined(CONFIG_SMC_CMD)
	reg_arr_t reg_arr;
#endif
	u32 data;
	u32 data_temp;

	// Pulldn and Pullup enable
	// ctrl_pulld_dq[11:8]=Active HIGH signal to down DQ signals. For normal operation this field should be zero.
	// ctrl_pulld_dqs[3:0]=Active HIGH signal to pull-up or down PDQS/NDQS signals.
	// Outp32( PHY_address+0x0038, 0x001F000F); // PHY_CON14 ctrl_pulld_dq[11:8]=0xf, ctrl_pulld_dqs[3:0]=0xf
	Outp32( PHY_address+0x0038, 0x001F000F); // PHY_CON14 ctrl_pulld_dq[11:8]=0xf, ctrl_pulld_dqs[3:0]=0xf

	// ZQ calibration
	// zq_mode_dds :: Driver strength selection. . It recommends one of the following settings instead of 3'h0.
	// Outp32( PHY_address+0x0040, 0x0F040306); // PHY_CON16 zq_clk_en[27]=ZQ I/O Clock enable, zq_manual_mode[3:2]=Manual calibration mode selection 2'b01: long calibration, zq_manual_str[1]=Manual calibration start
	// PHY_CON39 :: Driver Strength
	// Outp32( PHY_address+0x00A0 , 0x0FFF0FFF); // PHY_CON39
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
	// GOSUB Delay 100ms; GOSUB read &PHY_address+0x0048; zq_done[0]=1
	// GOSUB Delay 100ms; GOSUB read &PHY1_BASE+0x0048; zq_done[0]=1
	while( ( Inp32( PHY_address+0x0048 ) & 0x00000001 ) != 0x00000001 );	// PHY_CON17 zq_done[0]=ZQ Calibration is finished.

	// ZQ calibration exit
	data_temp = Inp32( PHY_address+0x0040 );
	data_temp = data_temp&(~0x000FFFFF);
	data = data_temp|0x00040304;
	Outp32( PHY_address+0x0040, data); // PHY_CON16 zq_mode_dds[26:24], zq_mode_term[23:21], zq_manual_start[1]=0

	// 1. Set DRAM burst length and READ latency
	Outp32( PHY_address+0x00AC, 0x0000080C); // PHY_CON42 ctrl_bstlen(Burst Length(BL))[12:8]=8, ctrl_rdlat(Read Latency(RL))[4:0]=12
	// 2. Set DRAM write latency
	Outp32( PHY_address+0x006C, 0x0007000F); // PHY_CON26 T_wrdata_en[20:16]=WL for DDR3

	// DLL LOCK Setting..!
	// Set the DLL lock parameters
	// Reserved	[31] ctrl_start_point	[30:24]=0x10 Reserved	[23] ctrl_inc	[22:16]=0x10 ctrl_force	[14:8] ctrl_start	[6]=0x1 ctrl_dll_on	[5]=0x1 ctrl_ref	[4:1]=0x8 Reserved	[0]
	// Next Step : Same Operation "CONCONTROL dfi_init_start[28]=1"
	// 2012.10.11
	// Outp32( PHY_address+0x0030, 0x10100070); // PHY_CON12
	Outp32( PHY_address+0x0030, 0x10100030); // PHY_CON12, "ctrl_dll_on[6] = 0"
	DMC_Delay(20); // PCLK 10 cycle.

	#if 0
	SetBits(0x14000060, 16, 0xF, 0x1);
	Outp32(0x14000068, 0x0);
	SetBits(0x14000064, 4, 0x1, 0x1);
	#endif

	Outp32( PHY_address+0x0030, 0x10100070); // PHY_CON12, "ctrl_start[6] = 1"
	DMC_Delay(1);

	//while( ( Inp32( PHY_address+0x0034 ) & 0x00000001 ) != 0x00000001 );	// ctrl_locked[0]=1

	#if 0
	SetBits(0x14000064, 4, 0x1, 0x0);
	#endif

	// 1. Set the Concontrol. At this moment, an auto refresh counter should be off.
	// 20120511 :: Change dll lock start point.
	Outp32( DREX_address+0x0000, 0x1FFF3000); // CONCONTROL dfi_init_start	[28], timeout_level0	[27:16], rd_fetch	[14:12], empty	[8], aref_en	[5], clk_ratio	[2:1]		

	// 2. Set the MemConfig0 register. If there are two external memory chips, also set the MemConfig1 register.
	// LPDDR2_P0_CS0 : 32'h2000_000 ~ 32'h27FF_FFFF (4Gbit)
	//Outp32( DREX_address+0x010C, 0x006007E0); // MemBaseConfig0 chip_base[26:16]=0x10, chip_mask[10:0]=0x7E0
	//Outp32( DREX_address+0x0110, 0x008007E0); // MemBaseConfig1 chip_base[26:16]=0x30, chip_mask[10:0]=0x7E0
#if defined(CONFIG_SMC_CMD)
	reg_arr.set0.addr = TZASC_address+0x0F00;
	reg_arr.set0.val = 0x006007C0;
	reg_arr.set1.addr = TZASC_address+0x0F04;
	reg_arr.set1.val = 0x00A007C0;
	reg_arr.set2.addr = TZASC_address+0x0F10;
	reg_arr.set2.val = 0x00402423;
	reg_arr.set3.addr = TZASC_address+0x0F14;
	reg_arr.set3.val = 0x00402423;

	set_secure_reg((u32)&reg_arr, 4);
#else
	Outp32( TZASC_address+0x0F00, 0x006007C0); // MemBaseConfig0 chip_base[26:16]=0x10, chip_mask[10:0]=0x7E0
	Outp32( TZASC_address+0x0F04, 0x00A007C0); // MemBaseConfig1 chip_base[26:16]=0x30, chip_mask[10:0]=0x7E0

	// 3. Set the MemConfig0
	// chip_map	[15:12]	Address Mapping Method (AXI to Memory). 0x1 = Interleaved ({row, bank, column, width})
	// chip_col	[11:8]	Number of Column Address Bits. 0x3 = 10 bits
	// chip_row	[7:4]	Number of Row Address Bits. 0x2 = 14 bits
	// chip_bank	[3:0]	Number of Banks. 0x3 = 8 banks
	//Outp32( DREX_address+0x0008, 0x00001323); // MemConfig0 chip_map	[15:12],chip_col	[11:8],chip_row	[7:4],chip_bank	[3:0]
	//Outp32( DREX_address+0x000C, 0x00001323); // MemConfig1 chip_map	[15:12],chip_col	[11:8],chip_row	[7:4],chip_bank	[3:0]
	Outp32( TZASC_address+0x0F10, 0x00402423); // MemConfig0 chip_map	[15:12],chip_col	[11:8],chip_row	[7:4],chip_bank	[3:0]
	Outp32( TZASC_address+0x0F14, 0x00402423); // MemConfig1 chip_map	[15:12],chip_col	[11:8],chip_row	[7:4],chip_bank	[3:0]
#endif
	// 4. Set the PrechConfig and PwrdnConfig registers.
	// 2013.04.18 :: DREX1_0_3 adding..!
	// Outp32( DREX_address+0x0014, 0xFF000000); // PrechConfig tp_cnt[31:24]=Timeout Precharge Cycles. 0xn = n cclk cycles. Refer to chapter 1.6.2 .Timeout precharge 
	Outp32( DREX_address+0x0014, 0xF0000000); // PrechConfig tp_en[31:28]=Timeout Precharge per Port
	Outp32( DREX_address+0x001C, 0xFFFFFFFF);

	// 2013.06.07 :: (0xD) Dynamic PD & Dynamic Self Change..!
	// Outp32( DREX_address+0x0028, 0xFFFF00FF); // PwrdnConfig dsref_cyc[31:16]=Number of Cycles for dynamic self refresh entry. 0xn = n aclk cycles. Refer to chapter 1.5.3 . Dynamic self refresh
	Outp32( DREX_address+0x0028, 0x1FFF000D); // PwrdnConfig dsref_cyc[31:16]=Number of Cycles for dynamic self refresh entry. 0xn = n aclk cycles. Refer to chapter 1.5.3 . Dynamic self refresh

	// 5. Set the TimingAref, TimingRow, TimingData and TimingPower registers.
	//     according to memory AC parameters. At this moment, TimingData.w1 and TimingData.r1
	//     registers must be programmed according to initial clock frequency.
	// 2012.10.10
	// Outp32( DREX_address+0x0030, 0x000000BB); // TimingAref autorefresh counter @24MHz
	// Outp32( DREX_address+0x0030, 0x0000005E); // TimingAref autorefresh counter @24MHz
	// 2012.12.20 :: 3.875 us * 8192 row = 250us
	Outp32( DREX_address+0x0030, 0x0000005D); // TimingAref autorefresh counter @24MHz

	// TimingAref autorefresh counter @24MHz
	if (nMEMCLK == 800) {
		Outp32( DREX_address+0x0034, 0x345A96D3); // TimingRow
		Outp32( DREX_address+0x0038, 0x3630065C); // TimingData
		Outp32( DREX_address+0x003C, 0x50380336); // TimingPower
	} else if (nMEMCLK == 825) {
		Outp32( DREX_address+0x0034, 0x36598692); // TimingRow
		Outp32( DREX_address+0x0038, 0x4640065C); // TimingData
		Outp32( DREX_address+0x003C, 0x543A0446); // TimingPower
	} else {
		;
	}

	// If QoS scheme is required, set the QosControl10~15 and QosConfig0~15 registers.

	// 6. Wait until dfi_init_complete become 1.
	while( ( Inp32( DREX_address+0x0040 ) & 0x00000008 ) != 0x00000008 );	// PhyStatus dfi_init_complete[3]=1

	// Outp32( DREX_address+0x0040, 0x00000008); // PhyStatus dfi_init_complete[3]=1

	// Deasserting the dfi_init_start
	// 2012.11.08 :: rd_fetch 3 -> 2
	// 2013.05.14 :: rd_fetch 3@933MHz
	Outp32( DREX_address+0x0000, 0x0FFF3000 );	// CONCONTROL dfi_init_start[0]=0

	// 7. Forcing DLL resynchronization - dfi_ctrlupd_req
	Outp32( DREX_address+0x0018, 0x00000008); 	// PhyControl0 ctrl_shgate[29]=1, fp_resync[3]=1
	DMC_Delay(20); // Wait for 10 PCLK cycles, PCLK(200MHz=10clock=50ns), DMC_Delay(40us)
	Outp32( DREX_address+0x0018, 0x00000000); 	// PhyControl0 ctrl_shgate[29]=1, fp_resync[3]=0

	// 8. calibration & levelings

	//-----------------------------------------------
	//- end_levelings_lpddr3_l
	//-----------------------------------------------

	// ctrl_atgate = 0
	// T_WrWrCmd	[30:24] 	It controls the interval between Write and Write during DQ Calibration. This value should be always kept by 5'h17. It will be used for debug purpose. 
	// T_WrRdCmd	[19:17] 	It controls the interval between Write and Read by cycle unit during Write Calibration. It will be used for debug purpose. 3'b111 : tWTR = 6 cycles (=3'b001)
	// ctrl_ddr_mode[12:11] 	2'b11: LPDDR3
	// ctrl_dfdqs[9] 	1¡¯b1: differential DQS
	Outp32( PHY_address+0x0000, 0x17021A00); // PHY_CON0 byte_rdlvl_en[13]=1, ctrl_ddr_mode[12:11]=01, ctrl_atgate[6]=1, Bit Leveling

	#if 0	// Move..! 2012.11.30
	// 26. Set the ConControl to turn on an auto refresh counter.
	// aref_en[5]=Auto Refresh Counter. 0x1 = Enable
	// 2012.11.08 :: rd_fetch 3 -> 2
	// 2013.05.14 :: rd_fetch 3@933MHz
	if(nMEMCLK == 933)	{
		Outp32( DREX_address+0x0000, 0x0FFF3128); // CONCONTROL aref_en[5]=1
	} else {
		Outp32( DREX_address+0x0000, 0x0FFF2128); // CONCONTROL aref_en[5]=1
	}
	#endif

	// 27. If power down modes are required, set the MemControl register.
	// bl[22:20]=Memory Burst Length 0x3 = 8
	// mem_width[15:12]=Width of Memory Data Bus 0x2 = 32-bit
	// mem_type[11:8]=Type of Memory 0x7 = LPDDR3
	// dsref_en[5]=Dynamic Self Refresh. 0x1 = Enable.
	// dpwrdn_en[1]=Dynamic Power Down. 0x1 = Enable
	// clk_stop_en[0]=Dynamic Clock Control. 0x1 = Stops during idle periods.
	Outp32( DREX_address+0x0004, 0x00312722); // MemControl bl[22:20]=8, mem_type[11:8]=7, two chip selection

	return;
}

void DMC_Low_Freqeuncy_Setting(u32 nMEMCLK)
{
	u32 data;

	// Pause Enable..!
	// data = Inp32( 0x1003091C );
	// data = data | (0x1<<0);
	// Outp32(0x1003091C, data);

	// uBits = (1 << 21) | (3 << 14) |(3 << 12) | (3 << 8);
	Outp32( CMU_MEMPART+0x0114, 0x0020F300);	// rBPLL_CON1

	if(nMEMCLK==800)
	{
		// Lock Time Setting
		Outp32(0x10030010, 0x00000258);	// PDIV*200 = 3*200

		// ENABLE(1), MDIV(200), PDIV(3), SDIV(1)
		// uBits = (1 << 31) | (200 << 16) | (3 << 8) | (1 << 0);
		Outp32( CMU_MEMPART+0x0110, 0x80C80301);	// rBPLL_CON0	BPLL=800MHz(3:200:1)
	}
	else if (nMEMCLK==825)
	{
		// //LOCKTIME(P*200(P=4))
		// uBits = 800;
		Outp32(0x10030010, 0x00000320);		// rXPLL_LOCK

		// //ENABLE(1), MDIV(275), PDIV(4), SDIV(1)
		// uBits = (1 << 31) | (275 << 16) | (4 << 8) | (1 << 0);
		Outp32(0x10030110, 0x81130401);		// rXPLL_CON0	XPLL=825MHz(4:275:1)
	}
	else if(nMEMCLK==933)
	{
		// Lock Time Setting
		Outp32(0x10030010, 0x00000320);	// PDIV*200 = 4*200

		// ENABLE(1), MDIV(200), PDIV(3), SDIV(1)
		// uBits = (1 << 31) | (311 << 16) | (3 << 8) | (1 << 0);
		Outp32( CMU_MEMPART+0x0110, 0x81370401);	// rBPLL_CON0	BPLL=933MHz(4:311:1)
	}
	else
	{
		// Lock Time Setting
		Outp32(0x10030010, 0x00000258);	// PDIV*200 = 3*200

		// ENABLE(1), MDIV(200), PDIV(3), SDIV(1)
		// uBits = (1 << 31) | (200 << 16) | (3 << 8) | (1 << 0);
		Outp32( CMU_MEMPART+0x0110, 0x80C80301);	// rBPLL_CON0	BPLL=800MHz(3:200:1)
	}

	// PLL locking indication
	// 0 = Unlocked
	// 1 = Locked
	while ((Inp32(0x10030110) & (1 << 29)) == 0);

	// ByPass :: BYPASS = 1, bypass mode is enabled - FOUT=FIN
	SetBits(CMU_MEMPART+0x0114, 22, 0x1, 0x1);

	// MUX_MCLK_CDREX(0), BPLL(1)
	// uBits =  (0 << 4) | (1 << 0);
	Outp32( CMU_MEMPART+0x0200, 0x00000001);	// rCLK_SRC_CDREX

	// CLK_MUX_STAT_CDREX Check
	do {
		data = Inp32(0x10030400) & (7 << 0);
	}while (data != 0x2);													// Opened by cju, 13.01.16
	//}while ((data == 0)||(data > 2));						// Closed by cju, 13.01.16

	// ByPass :: BYPASS = 1, bypass mode is Disabled - FOUT=BPLL FOUT
	SetBits(0x10030114, 22, 0x1, 0x0);
	DMC_Delay(200);

	//* Add CLK_DIV_CDREX0, PCLK_CDREX(28:1/2),SCLK_CDREX(24:1/8),ACLK_CDREX1(16:1/2),CCLK_DREX(8:1/2),CLK2X_PHY0(3:1/1)
	data=(1<<28)|(15<<24)|(1<<16)|(1<<8)|(0<<3);
	Outp32(0x10030500, data);
}

void DMC_High_Freqeuncy_Setting(u32 nMEMCLK)
{
	u32 data;

	if (nMEMCLK==400)
	{
		// Lock Time Setting
		Outp32(0x10030010, 0x00000258);	// PDIV*200 = 3*200

		// ENABLE(1), MDIV(200), PDIV(3), SDIV(2)
		// uBits = (1 << 31) | (200 << 16) | (3 << 8) | (2 << 0);
		Outp32( CMU_MEMPART+0x0110, 0x80C80302); // rBPLL_CON0	BPLL=400MHz(3:200:2)
		DMC_Delay(100);
	}
	else if (nMEMCLK==533)
	{
		// Lock Time Setting
		Outp32(0x10030010, 0x00000258);	// PDIV*200 = 3*200

		// ENABLE(1), MDIV(266), PDIV(3), SDIV(2)
		// uBits = (1 << 31) | (266 << 16) | (3 << 8) | (2 << 0);
		Outp32( CMU_MEMPART+0x0110, 0x810A0302);		// rBPLL_CON0	BPLL=532MHz(3:266:2)
		DMC_Delay(100);
	}
	else if (nMEMCLK==667)
	{
		// Lock Time Setting
		Outp32(0x10030010, 0x00000190);	// PDIV*200 = 2*200

		// ENABLE(1), MDIV(111), PDIV(2), SDIV(1)
		// uBits = (1 << 31) | (111 << 16) | (2 << 8) | (1 << 0);
		Outp32( CMU_MEMPART+0x0110, 0x806F0201);	// rBPLL_CON0	BPLL=666MHz(2:111:1)
		DMC_Delay(100);
	}
	else if (nMEMCLK==733)
	{
		// Lock Time Setting
		Outp32(0x10030010, 0x00000190);	// PDIV*200 = 2*200

		// ENABLE(1), MDIV(122), PDIV(2), SDIV(1)
		// uBits = (1 << 31) | (122 << 16) | (2 << 8) | (1 << 0);
		Outp32( CMU_MEMPART+0x0110, 0x807A0201);	// rBPLL_CON0	  BPLL=732MHz(2:122:1)
		DMC_Delay(100);
	}
	else if (nMEMCLK==800)
	{
		// used previous setting
	}
	else if (nMEMCLK==825)
	{
		// used previous setting
	}
	else if (nMEMCLK==933)
	{
		// used previous setting
	}
	else if (nMEMCLK==1065)
	{
		// Lock Time Setting
		//Outp32(0x10030010, 0x00000320);	// PDIV*200 = 4*200

		// ENABLE(1), MDIV(355), PDIV(4), SDIV(1)
		// uBits = (1 << 31) | (355 << 16) | (4 << 8) | (1 << 0);
		// Outp32( CMU_MEMPART+0x0110, 0x81630401);	// rBPLL_CON0	BPLL=1065MHz(4:355:1)
		// DMC_Delay(100);
	}
	else
	{
	}

	// PLL locking indication
	// 0 = Unlocked
	// 1 = Locked
	while ((Inp32(0x10030110) & (1 << 29)) == 0);

	// ACLK_CDREX1_RATIO and CCLK_DREX0_RATIO should always have same
	// value to keep synchronization between two DREXs and BUS.
	// PCLK_CDREX(1/4), SCLK_CDREX(1/1), ACLK_CDREX1(1/2), CCLK_DREX0(1/2), CLK2X_PHY0(1/1)
	// uBits = (3 << 28) | (0 << 24) | (1 << 16) | (1 << 8) | (0 << 3) ;
	Outp32( CMU_MEMPART+0x0500, 0x30010100);		// rCLK_DIV_CDREX0
	DMC_Delay(100);
}

void DMC_InitForLPDDR3(u32 nMEMCLK, BOOT_STAT eBootStat)
{
	u32 data;

	/* CORE_MISC */
	// Outp32(0x10CE0050, 0x00000001);
	//Outp32(0x10CE0050, 0x00000100); // EVT1 no exist

	/****************************************/
	/*****	                    CA SWAP                   *****/
	/****************************************/
	if (CA_SWAP == 1)	{
		CA_swap_lpddr3(DREX1_0);
	}

	/****************************************/
	/*****   		CLOCK SETTTING               *****/
	/****************************************/
	DMC_Low_Freqeuncy_Setting(nMEMCLK);

	/****************************************/
	/*****	     LOW FREQUENCY            *****/
	/****************************************/
	Low_frequency_init_lpddr3(PHY0_BASE, DREX1_0);

	/****************************************/
	/*****   		CLOCK SETTTING               *****/
	/****************************************/
	DMC_High_Freqeuncy_Setting(nMEMCLK);

	/****************************************/
	/*****	              HIGH FREQUENCY              *****/
	/****************************************/
	High_frequency_init_lpddr3(PHY0_BASE, DREX1_0, TZASC_0, nMEMCLK);

	#if 1	// Move..! 2012.11.30
	// 26. Set the ConControl to turn on an auto refresh counter.
	// aref_en[5]=Auto Refresh Counter. 0x1 = Enable
	// 2012.11.08 :: rd_fetch 3 -> 2
	// 2013.04.12 :: Automatic control for ctrl_pd in none read state
	// 2013.05.08 :: update_mode[3] :: 0x1 = MC initiated update/acknowledge mode
	// 2013.05.14 :: rd_fetch 3@933MHz
	Outp32( DREX1_0+0x0000, 0x0FFF31A8); // CONCONTROL aref_en[5]=1
	#endif

	#if 0
	// BRB Space Reservation Setting..!
	Outp32( DREX1_0+0x0100, 0x00000033);	// BRBRSVCONTROL
	Outp32( DREX1_0+0x0104, 0x88588858); 	// BRBRSVCONFIG
	Outp32( DREX1_0+0x00D8, 0x00000000); 	// QOSCONTROL
	Outp32( DREX1_0+0x00C0, 0x00000080); 	// QOSCONTROL
	Outp32( DREX1_0+0x0108, 0x00000001); 	// BRBQOSCONFIG
	#endif

	#ifdef USED_DYNAMIC_AUTO_CLOCK_GATING
	// Clock Gating Control Register..!
	Outp32( DREX1_0+0x0008, 0x0000001F);
	#endif

	Outp32( DREX1_0+0x0018, 0x00000002);

	return;
}

void DMC_InitForLPDDR3_ReInit(u32 nMEMCLK, BOOT_STAT eBootStat)
{
	u32 data;

	/* CORE_MISC */
	// Outp32(0x10CE0050, 0x00000001);
	// Outp32(0x10CE0050, 0x00000100);

	/****************************************/
	/*****	                    CA SWAP                   *****/
	/****************************************/
	if (CA_SWAP == 1)	{
		CA_swap_lpddr3(DREX1_0);
	}

	/****************************************/
	/*****   		CLOCK SETTTING               *****/
	/****************************************/
	DMC_Low_Freqeuncy_Setting(nMEMCLK);

	/****************************************/
	/*****	     LOW FREQUENCY            *****/
	/****************************************/
	// PHY0+DREX1_0
	Low_frequency_init_lpddr3_ReInit(PHY0_BASE, DREX1_0);

	/****************************************/
	/*****   		CLOCK SETTTING               *****/
	/****************************************/
	DMC_High_Freqeuncy_Setting(nMEMCLK);

	/****************************************/
	/*****	              HIGH FREQUENCY              *****/
	/****************************************/
	// PHY0+DREX1_0
	High_frequency_init_lpddr3(PHY0_BASE, DREX1_0, TZASC_0, nMEMCLK);


	#if 1	// Move..! 2012.11.30
	// 26. Set the ConControl to turn on an auto refresh counter.
	// aref_en[5]=Auto Refresh Counter. 0x1 = Enable
	// 2012.11.08 :: rd_fetch 3 -> 2
	// 2013.04.12 :: Automatic control for ctrl_pd in none read state
	// 2013.05.08 :: update_mode[3] :: 0x1 = MC initiated update/acknowledge mode
	// 2013.05.14 :: rd_fetch 3@933MHz
	Outp32( DREX1_0+0x0000, 0x0FFF21A8); // CONCONTROL aref_en[5]=1
	#endif

	#if 0
	// BRB Space Reservation Setting..!
	Outp32( DREX1_0+0x0100, 0x00000033);	// BRBRSVCONTROL
	Outp32( DREX1_0+0x0104, 0x88588858); 	// BRBRSVCONFIG
	Outp32( DREX1_0+0x00D8, 0x00000000); 	// QOSCONTROL
	Outp32( DREX1_0+0x00C0, 0x00000080); 	// QOSCONTROL
	Outp32( DREX1_0+0x0108, 0x00000001); 	// BRBQOSCONFIG
	#endif

	#ifdef USED_DYNAMIC_AUTO_CLOCK_GATING
	// Clock Gating Control Register..!
	Outp32( DREX1_0+0x0008, 0x0000001F);
	#endif

	return;
}

void dmc_ctrl_init(void)
{
	u32 eBootStat;
	u32 nMEMCLK;

	nMEMCLK = CONFIG_MEMCLK;
	eBootStat = __REG(EXYNOS5_POWER_BASE + INFORM1_OFFSET);

	DMC_InitForLPDDR3(nMEMCLK, eBootStat);
}

void dmc_ctrl_init_resume(void)
{
	u32 eBootStat;
	u32 nMEMCLK;

	nMEMCLK = CONFIG_MEMCLK;
	eBootStat = __REG(EXYNOS5_POWER_BASE + INFORM1_OFFSET);

	DMC_InitForLPDDR3_ReInit(nMEMCLK, eBootStat);
}
