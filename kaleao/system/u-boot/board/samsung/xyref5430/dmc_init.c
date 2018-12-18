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
#include <asm/io.h>
#include <asm/arch/dmc.h>
#include <asm/arch/clock.h>
#include <asm/arch/power.h>
#include <asm/arch/cpu.h>
#include <asm/arch/smc.h>
#include <asm/arch/pmu.h>
#include "setup.h"

//- Memory Size Information
typedef enum
{
	LP3_2GB_4Gb_DIE,				// 4Gbit-die x 4ea chips
	LP3_3GB_6Gb_DIE,				// 6Gbit-die x 4ea chips
	LP3_4GB_8Gb_DIE,					// 8Gbit-die x 4ea chips
	OTHERS
}MEMORY_SIZE_INFO;

//- Mode Register Address in LPDDR3 SDRAM
typedef enum
{
	LPDDR3_MR_0,				// MR#0
	LPDDR3_MR_1,				// MR#1
	LPDDR3_MR_2,				// MR#2
	LPDDR3_MR_3,				// MR#3
	LPDDR3_MR_4,				// MR#4
	LPDDR3_MR_5,				// MR#5
	LPDDR3_MR_6,				// MR#6
	LPDDR3_MR_7,				// MR#7
	LPDDR3_MR_8,				// MR#8
	LPDDR3_MR_9,				// MR#9
	LPDDR3_MR_10,				// MR#10
	LPDDR3_MR_11,				// MR#11
	LPDDR3_MR_16=0x10,			// MR#16
	LPDDR3_MR_17=0x11,			// MR#17
	LPDDR3_MR_32=0x20,			// MR#32
	LPDDR3_MR_40=0x28,			// MR#40
	LPDDR3_MR_41=0x29,			// MR#41
	LPDDR3_MR_42=0x2A,			// MR#42
	LPDDR3_MR_48=0x30,			// MR#48
	LPDDR3_MR_63=0x3F			// MR#63
}LP3_MODE_REGISTER_ADDR;

#define Outp64(addr, data)	(*(volatile u64 *)(addr) = (data))
#define Outp32(addr, data)	(*(volatile u32 *)(addr) = (data))
#define Inp32(addr)		(*(volatile u32 *)(addr))
#define GetBits(uAddr, uBaseBit, uMaskValue) \
    ((Inp32(uAddr)>>(uBaseBit))&(uMaskValue))
#ifdef CONFIG_CPU_EXYNOS5433_EVT1
#define GetPOPType() (0)
#else
#define GetPOPType() (GetBits(0x10000004, 4, 0x3))
#endif
#undef FALSE
#undef TRUE

#define FALSE 0
#define TRUE 1
#define false 0
#define true 1

#define PWMTIMER_BASE			(0x14DD0000)						//0x12D90000

#define rTCFG0					(PWMTIMER_BASE+0x00)
#define rTCFG1					(PWMTIMER_BASE+0x04)
#define rTCON					(PWMTIMER_BASE+0x08)
#define rTCNTB0					(PWMTIMER_BASE+0x0C)
#define rTCMPB0					(PWMTIMER_BASE+0x10)
#define rTCNTO0					(PWMTIMER_BASE+0x14)
#define rTINT_CSTAT				(PWMTIMER_BASE+0x44)

void InitMemClk(u32 uMemclk);

//----------------------------------------------------------------------------
//
//- DMC Initialzation Script for LPDDR3
//
//- Copyright 2013 by Samsung Electronics. All rights reserved.
//
//----------------------------------------------------------------------------

static void StartTimer0(void)
{
	u32 uTimer=0;
	u32 uTemp0,uTemp1;
	u32 uEnInt=0;
	u32 uDivider=0;
	u32 uDzlen=0;
	u32 uPrescaler=66;												//- Silicon : uPrescaler=66   /  FPGA : uPrescaler=24
	u32 uEnDz=0;
	u32 uAutoreload=1;
	u32 uEnInverter=0;
	u32 uTCNTB=0xffffffff;
	u32 uTCMPB=(u32)(0xffffffff/2);

	{
		uTemp0 = Inp32(rTCFG1);						// 1 changed into 0xf; dohyun kim 090211
		uTemp0 = (uTemp0 & (~ (0xf << 4*uTimer) )) | (uDivider<<4*uTimer);
		// TCFG1			init. selected MUX 0~4, 	select 1/1~1/16

		Outp32(rTCFG1,uTemp0);

		uTemp0 = Inp32(rTINT_CSTAT);
		uTemp0 = (uTemp0 & (~(1<<uTimer)))|(uEnInt<<(uTimer));
		//		selected timer disable, 		selected timer enable or diable.
		Outp32(rTINT_CSTAT,uTemp0);
	}

	{
		uTemp0 = Inp32(rTCON);
		uTemp0 = uTemp0 & (0xfffffffe);
		Outp32(rTCON, uTemp0);								// Timer0 stop

		uTemp0 = Inp32(rTCFG0);
		uTemp0 = (uTemp0 & (~(0xff00ff))) | ((uPrescaler-1)<<0) |(uDzlen<<16);
		//			init. except prescaler 1 value, 	set the Prescaler 0 value,	set the dead zone length.
		Outp32(rTCFG0, uTemp0);

		Outp32(rTCNTB0, uTCNTB);
		Outp32(rTCMPB0, uTCMPB);


		uTemp1 = Inp32(rTCON);
		uTemp1 = (uTemp1 & (~(0x1f))) |(uEnDz<<4)|(uAutoreload<<3)|(uEnInverter<<2)|(1<<1)|(0<<0);
		//		set all zero in the Tiemr 0 con., Deadzone En or dis, set autoload, 	set invert,  manual uptate, timer stop
		Outp32(rTCON, uTemp1);									//timer0 manual update
		uTemp1 = (uTemp1 & (~(0x1f))) |(uEnDz<<4)|(uAutoreload<<3)|(uEnInverter<<2)|(0<<1)|(1<<0);
		//		set all zero in the Tiemr 0 con., Deadzone En or dis, set autoload, 	set invert,  manual uptate disable, timer start
		Outp32(rTCON, uTemp1);									// timer0 start
	}
}

static void PWM_stop(u32 uNum)
{
    u32 uTemp;

    uTemp = Inp32(rTCON);

    if(uNum == 0)
		uTemp &= ~(0x1);
    else
		uTemp &= ~((0x10)<<(uNum*4));

    Outp32(rTCON,uTemp);
}

static u32 StopTimer0(void)
{
	u32 uVal = 0;

	PWM_stop(0);

	uVal = Inp32(rTCNTO0);
	uVal = 0xffffffff - uVal;

	return uVal;
}

#if 0
void DMC_Delay(int msec)
{
	volatile u32 i;
	for(;msec > 0; msec--);
		for(i=0; i<1000; i++) ;
}
#else
void DMC_Delay(u32 usec)
{
	u32 uElapsedTime, uVal;

	StartTimer0();

	while(1){
		uElapsedTime = Inp32(rTCNTO0);
		uElapsedTime = 0xffffffff - uElapsedTime;

		if(uElapsedTime > usec){			
			StopTimer0();
			break;
		}
	}
}
#endif



void Gate_CLKM_PHY(int gate_en)
{
	u32 temp;

	temp=Inp32(0x105B0800);

	if(gate_en){
		temp=temp&~(0x3<<16);
		Outp32(0x105B0800,temp);
	}
	else{
		temp=temp&~(0x3<<16);
		temp|=(0x3<<16);
		Outp32(0x105B0800,temp);
	}
}

u32 Check_MRR_Status(u32 ch_offset)
{
	u32 resp;

	resp=(Inp32(0x10400054 + ch_offset))&(0xFF);
	return resp;
}


int Check_DAI_Status(u32 ch, u32 cs)
{
	u32 mr0;
	u32 resp;
	u32 loop_cnt=1000;
	int re=true;

	if(cs==0)				mr0=0x09000000;
	else					mr0=0x09100000;

	if(ch==0){
		do{
			Outp32(0x10400010, mr0);
			resp=Inp32(0x10400054)&0x1;
			loop_cnt--;

			if(loop_cnt==0){
				DMC_Delay(10);							//- Although DAI is not completed during polling it, end the loop when 10us delay is time over.
				re=false;
				break;
			}
		}while(resp);
	}
	else{
		do{
			Outp32(0x10440010, mr0);
			resp=Inp32(0x10440054)&0x1;
			loop_cnt--;

			if(loop_cnt==0){
				DMC_Delay(10);							//- Although DAI is not completed during polling it, end the loop when 10us delay is time over.
				re=false;
				break;
			}
		}while(resp);
	}

	return re;
}

u32 Issuing_MRR_CMD(u32 ch,u32 cs,LP3_MODE_REGISTER_ADDR mr_addr)
{
	u32 result;
	u32 ch_offset;
	u32 mrr_cmd=0x9;
	u32 cmd_chip;
	u32 cmd_bank;
	u32 cmd_addr;
	u32 mr;
	u32 op=0x0;

	mr=(mr_addr)&(0xFF);
	cmd_chip=(cs)&(0x1);
	cmd_bank=(mr>>3)&(0x7);
	cmd_addr=((mr&0x7)<<10)|((op&0xFF)<<2)|((mr>>6)&0x3);

	if(ch==0)			ch_offset=0;
	else				ch_offset=0x40000;

	Outp32( 0x10400010 + ch_offset, (mrr_cmd<<24)|(cmd_chip<<20)|(cmd_bank<<16)|(cmd_addr<<0)); 		 //- Issuing MRR Command using DirectCMD
	result=Check_MRR_Status(ch_offset);																	 //- Reading MRR Status Register after MMR command is issued.

	return result;
}

MEMORY_SIZE_INFO DMC_Detecting_LP3_SizeInfo(u32 ch,u32 cs,LP3_MODE_REGISTER_ADDR mr_addr)
{
	u32 i;
	u32 j=0,k=0;
	u32 mem_type;
	u32 density;
	u32 bus_width;
	u32 op_value;
	u32 total_size;
	MEMORY_SIZE_INFO mcp_option;
	u32 channel=ch;
	u32 chip_select=cs;

	if(channel==0){
		if(chip_select==0)		k=0;
		else					k=1;
	}
	else{
		if(chip_select==0)		k=2;
		else					k=3;
	}

	do{
		op_value=(Issuing_MRR_CMD(channel,chip_select,mr_addr))&(0xFF);
		mem_type=(op_value>>0)&(0x3);
		density=(op_value>>2)&(0xF);
		bus_width=(op_value>>6)&(0x3);

		j++;
		if(j>10){			//- 10 times iteration per chip
			k++;			//- k=0 (CH0/CS0), 1(CH0/CS1), 2(CH1/CS0), 3(CH1/CS1)
			if(k==4){
				break;		//- MR8 Read Failure Case
			}

			if(channel==0){
				if(chip_select==1){
					channel=1;
					chip_select=0;
				}
				else{
					chip_select=1;
				}
			}
			else{
				if(chip_select==0){
					chip_select=1;
				}
			}

			j=0;
		}
	}while((mem_type!=0x3)||(density<6)||(density>0xE));

	if(mem_type==0x3){					//- S8 SDRAM
		if(bus_width==0x0){				//- Bus Width : x32
			if(density==0x6){			//- 4Gbit-Die x 4ea
				mcp_option=LP3_2GB_4Gb_DIE;
			}
			else if(density==0xE){		//- 6Gbit-Die x 4ea
				mcp_option=LP3_3GB_6Gb_DIE;
			}
			else if(density==0x7){		//- 8Gbit-Die x 4ea
				mcp_option=LP3_4GB_8Gb_DIE;
			}
			else{						//- There are no devices with those size - (12Gb/16Gb/32Gb die)
				mcp_option=OTHERS;
				for(i=0;i<0x6000000;i++);
			}
		}
		else if(bus_width==0x1){		//- Bus Width : x16
			mcp_option=OTHERS;
			for(i=0;i<0x6000000;i++);		//- Reserved for future
		}
		else{							//- Bus Width Information Detection Failure
			mcp_option=OTHERS;
			for(i=0;i<0x6000000;i++);
		}

	}
	else{								//- Device Information Detection Failure
		mcp_option=OTHERS;
		for(i=0;i<0x6000000;i++);
	}

	return mcp_option;
}

void Issuing_TMRS_DirectCMD(void)
{

	u32 ch_offset;
	u32 cs_offset;
	u32 ch,cs;
	u32 i;
	u32 cmd_addr,cmd_addr_1_0,cmd_addr_9_2,cmd_addr_12_10;
	u32 cmd_bank;
	u32 direct_cmd_value;
	u32 sw_n_warm_reset;
	u32 cmd_type;

	static u32 ma[19]={0x9,0x9,0x9,0x9,0x89,
			   0x9,0x9,0x9,0x9,0x9,
			   0x9,0x9,0x9,0x9,0x9,
			   0x9,0x89,0x9,0x9};
	static u32 op[19]={0x20,0x20,0x20,0xFF,0x00,
			   0x04,0x80,0x00,0x00,0x80,
			   0x01,0x08,0x00,0x00,0x02,
			   0x08,0x00,0x00,0x00};

	//All bank precharge

	for(ch=0;ch<2;ch++)
	{
		for(cs=0;cs<2;cs++)
		{
			Outp32( 0x10400010 + (ch*0x40000), (0x1<<24)|(cs<<20));	//- PALL

			for(i=0;i<19;i++)
			{
				cmd_addr_1_0   = ((ma[i]&(0x3<<6))>>6);	//MA[7:6]
				cmd_bank       = ((ma[i]&(0x7<<3))>>3);	//MA[5:3]
				cmd_addr_12_10 = ma[i]&(0x7<<0);	//MA[2:0]
				cmd_addr_9_2   = op[i];
				cmd_addr = (cmd_addr_12_10<<10)|(cmd_addr_9_2<<2)|(cmd_addr_1_0);
				cmd_type =0;

				//cmd_type	[27:24]
				//cmd_chip	[20]
				//cmd_bank	[18:16]
				//cmd_addr	[15:0]

				direct_cmd_value = (cmd_type<<24)|(cs<<20)|(cmd_bank<<16)|cmd_addr;
				Outp32( 0x10400010 + (ch*0x40000), direct_cmd_value);		 //- MRS
				Outp32( 0x10400010 + (ch*0x40000), 0x07000000|(cs<<20));	 //- nop
				Outp32( 0x10400010 + (ch*0x40000), 0x07000000|(cs<<20));	 //- nop
				Outp32( 0x10400010 + (ch*0x40000), 0x07000000|(cs<<20));	 //- nop
				Outp32( 0x10400010 + (ch*0x40000), 0x07000000|(cs<<20));	 //- nop
				Outp32( 0x10400010 + (ch*0x40000), 0x07000000|(cs<<20));	 //- nop


				//m_printf("\n%d  ma = 0x%x op = 0x%x direct_cmd_value = 0x%08x  \n",i,ma[i],op[i],direct_cmd_value);
			}
		}
		//m_printf("\n\n");
	}
}

void Issuing_TMRS_DQskew_DirectCMD(void)
{

	u32 ch_offset;
	u32 cs_offset;
	u32 ch,cs;
	u32 i;
	u32 cmd_addr,cmd_addr_1_0,cmd_addr_9_2,cmd_addr_12_10;
	u32 cmd_bank;
	u32 direct_cmd_value;
	u32 sw_n_warm_reset;
	u32 cmd_type;

	static u32 ma[9]={0x9,0x9,0x9,0x9,
	           0x9,0x9,0x9,
	           0x9,0x9};
//	static u32 op[9]={0x20,0x20,0x20,0xFF,0x01,
//	           0x04,0x01,0x00,0x00};
	static u32 op[9]={0x20,0x20,0x20,0xFF,
			   0x10,0x08,0x08,
			   0x00,0x00};


	for(ch=0;ch<2;ch++)
	{
		for(cs=0;cs<2;cs++)
		{
			Outp32( 0x10400010 + (ch*0x40000), (0x1<<24)|(cs<<20));	//- PALL

			for(i=0;i<9;i++)
			{
				cmd_addr_1_0   = ((ma[i]&(0x3<<6))>>6);	//MA[7:6]
				cmd_bank       = ((ma[i]&(0x7<<3))>>3);	//MA[5:3]
				cmd_addr_12_10 = ma[i]&(0x7<<0);	//MA[2:0]
				cmd_addr_9_2   = op[i];
				cmd_addr = (cmd_addr_12_10<<10)|(cmd_addr_9_2<<2)|(cmd_addr_1_0);
				cmd_type =0;

				//cmd_type	[27:24]
				//cmd_chip	[20]
				//cmd_bank	[18:16]
				//cmd_addr	[15:0]

				direct_cmd_value = (cmd_type<<24)|(cs<<20)|(cmd_bank<<16)|cmd_addr;
				Outp32( 0x10400010 + (ch*0x40000), direct_cmd_value);		 //- MRS
				Outp32( 0x10400010 + (ch*0x40000), 0x07000000|(cs<<20));	 //- nop
				Outp32( 0x10400010 + (ch*0x40000), 0x07000000|(cs<<20));	 //- nop
				Outp32( 0x10400010 + (ch*0x40000), 0x07000000|(cs<<20));	 //- nop
				Outp32( 0x10400010 + (ch*0x40000), 0x07000000|(cs<<20));	 //- nop
				Outp32( 0x10400010 + (ch*0x40000), 0x07000000|(cs<<20));	 //- nop


				//m_printf("\n%d  ma = 0x%x op = 0x%x direct_cmd_value = 0x%08x  \n",i,ma[i],op[i],direct_cmd_value);
			}
		}
		//m_printf("\n\n");
	}
}

int Issuing_Basic_DirectCMD(void)
{
#ifdef CONFIG_ARM64
	int re[4];
#else
	int re[]={true,true,true,true};
#endif
	int result=true;
	u32 ch_offset;
	u32 cs_offset;
	u32 ch,cs;
	u32 sw_n_warm_reset;

#ifdef CONFIG_ARM64
	re[0] = true;
	re[1] = true;
	re[2] = true;
	re[3] = true;
#endif

	sw_n_warm_reset=(Inp32(0x105C0404)>>28)&0x3;				//- RESETSTATE(0x105C_0404) : [29]=SWRST,  [28]=WRESET

	for(ch=0;ch<2;ch++){
		if(ch==0)			ch_offset=0;
		else				ch_offset=0x40000;

		for(cs=0;cs<2;cs++){
			if(cs==0)			cs_offset=0;
			else				cs_offset=0x100000;

			if(sw_n_warm_reset){
				Outp32( 0x10400010 + ch_offset, (0x08000000)|(cs_offset));					 //- Self Refresh Exit
				//Outp32( 0x10400010 + ch_offset, (0x00000870)|(cs_offset));					 //- MR2 (MA[7:0]=0x02), RL/WL	 , RL&WL=OP[3:0]=0xC(RL/WL=14/8,for 825MHz), nWRE=OP[4]=1 (WR > 9(667MHz))
				//Outp32( 0x10400010 + ch_offset, (0x0000060C)|(cs_offset));					 //- MR1 (MA[7:0]=0x01), nWR		 , BL=OP[2:0]=0x3, nWR=OP[7:5]=4 (for 825MHz)
			}
			else{
				Outp32( 0x10400010 + ch_offset, (0x07000000)|(cs_offset));					 //- NOP
				DMC_Delay(200); 															 //- Wait for 200us
				Outp32( 0x10400010 + ch_offset, (0x00071C00)|(cs_offset));					 //- MR63(MA[7:0]=0x3F), Reset
				DMC_Delay(1);																 //- Wait for 1us
				re[(ch<<1)|(cs)]=Check_DAI_Status(ch,cs); 													 //- Check if DAI is complete.
				Outp32( 0x10400010 + ch_offset, (0x00010BFC)|(cs_offset));					 //- MR10(MA[7:0]=0x0A), Calibration, OP[7:0]=0xFF(Calibration command after initialization)
				DMC_Delay(1);																 //- Wait for 1us
				Outp32( 0x10400010 + ch_offset, (0x00000870)|(cs_offset));					 //- MR2 (MA[7:0]=0x02), RL/WL	 , RL&WL=OP[3:0]=0xC(RL/WL=14/8,for 825MHz), nWRE=OP[4]=1 (WR > 9(667MHz))
				Outp32( 0x10400010 + ch_offset, (0x0000060C)|(cs_offset));					 //- MR1 (MA[7:0]=0x01), nWR		 , BL=OP[2:0]=0x3, nWR=OP[7:5]=4 (for 825MHz)
			}

		}
	}

	result=(re[3])|(re[2])|(re[1])|(re[0]);

	return result;
}

void Issuing_Optinal_DirectCMD(u32 mem_ds, u32 odt)
{
	u32 ch_offset;
	u32 cs_offset;
	u32 ch,cs;
	u32 mr3,mr11;

	//*** Setting Memory Driver Strength & ODT Termination ***
	//---------------------------------------------------------------
	//- DS : 0x1 : 34.3 ohm typical pull-down/pull-up
	//- DS : 0x2 : 40 ohm typical pull-down/pull-up (default)
	//- DS : 0x3 : 48 ohm typical pull-down/pull-up
	//- DS : 0x9 : 34.3 ohm typical pull-down/40 ohm typical pull-up
	//- DS : 0xA : 40 ohm typical pull-down/48 ohm typical pull-up
	//- DS : 0xB : 34.3 ohm typical pull-down/48 ohm typical pull-up

	//- ODT Term : 0x0 : Disable (disable)
	//- ODT Term : 0x1 : RZQ/4(60 ohm, for 933MHz / 1066MHz)
	//- ODT Term : 0x2 : RZQ/2(120 ohm)
	//- ODT Term : 0x3 : RZQ/1(240 ohm)
	//---------------------------------------------------------------
	//- Selection of Memory Option : DS -> 0x2,   ODT Termination -> 0x0

	//* mr3 : I/O configuration
	if (mem_ds==0x1)				mr3=0x00000C04; 	// 34.3 ohm typical pull-down/pull-up
	else if (mem_ds==0x2)			mr3=0x00000C08; 	// 40 ohm typical pull-down/pull-up (default)
	else if (mem_ds==0x3)			mr3=0x00000C0C; 	// 48 ohm typical pull-down/pull-up
	else if (mem_ds==0x9)			mr3=0x00000C24; 	// 34.3 ohm typical pull-down/40 ohm typical pull-up
	else if (mem_ds==0xA)			mr3=0x00000C28; 	// 40 ohm typical pull-down/48 ohm typical pull-up
	else if (mem_ds==0xB)			mr3=0x00000C2C; 	// 34.3 ohm typical pull-down/48 ohm typical pull-up
	else							mr3=0x00000C08; 	// 40 ohm typical pull-down/pull-up (default)

	//* on die termination
	if (odt==0)						mr11=0x00010C00;		// ODT Disable
	else if (odt==1)				mr11=0x00010C04;		// RZQ/4
	else if (odt==2)				mr11=0x00010C08;		// RZQ/2
	else if (odt==3)				mr11=0x00010C0C;		// RZQ/1
	else							mr11=0x00010C00;		// ODT Disable

	for(ch=0;ch<2;ch++){
		if(ch==0)				ch_offset=0;
		else					ch_offset=0x40000;

		for(cs=0;cs<2;cs++){
			if(cs==0)			cs_offset=0;
			else				cs_offset=0x100000;

			Outp32(0x10400010 + ch_offset, (mr3)|(cs_offset) );						//- CHx, CSx mr3 command
			Outp32(0x10400010 + ch_offset, (mr11)|(cs_offset) ); 				    //- CHx, CSx mr11 command
		}
	}
}

int Init_Mem(u32 mem_ds, u32 odt)
{
	int re;
	u32 sw_n_warm_reset;

	sw_n_warm_reset=(Inp32(0x105C0404)>>28)&0x3;				//- RESETSTATE(0x105C_0404) : [29]=SWRST,  [28]=WRESET

	//*** Setting Memory Driver Strength & ODT Termination ***
	//---------------------------------------------------------------
	//- DS : 0x1 : 34.3 ohm typical pull-down/pull-up
	//- DS : 0x2 : 40 ohm typical pull-down/pull-up (default)
	//- DS : 0x3 : 48 ohm typical pull-down/pull-up
	//- DS : 0x9 : 34.3 ohm typical pull-down/40 ohm typical pull-up
	//- DS : 0xA : 40 ohm typical pull-down/48 ohm typical pull-up
	//- DS : 0xB : 34.3 ohm typical pull-down/48 ohm typical pull-up

	//- ODT Term : 0x0 : Disable (disable)
	//- ODT Term : 0x1 : RZQ/4(60 ohm, for 933MHz / 1066MHz)
	//- ODT Term : 0x2 : RZQ/2(120 ohm)
	//- ODT Term : 0x3 : RZQ/1(240 ohm)
	//---------------------------------------------------------------
	//- Selection of Memory Option : DS -> 0x2,   ODT Termination -> 0x0

	re=Issuing_Basic_DirectCMD();							//- Issuing DirectCMD for CHx, CSx
	if(!sw_n_warm_reset){
		Issuing_Optinal_DirectCMD(mem_ds,odt);					//- Setting Memory Driver Strength(MR3) & ODT Termination(MR11)
		Issuing_TMRS_DirectCMD();						//- TMRS setting
		//Issuing_TMRS_DQskew_DirectCMD();
	}

	return re;
}

void DMC_ODT_enble(void)
{
	//-----------------------------------------------------------------------
	//--- Read ODT		                ---
	//-----------------------------------------------------------------------
	Outp32(0x104200C0,(Inp32(0x104200C0)&(~(0x7<<21)))|(0x2<<21));
	Outp32(0x104600C0,(Inp32(0x104600C0)&(~(0x7<<21)))|(0x2<<21));

	Outp32(0x10420018,Inp32(0x10420018)&(~(0xF<<0)));
	Outp32(0x104200C0,(Inp32(0x104200C0)&(~(0x1<<20)))|(0x1<<20));
	Outp32(0x1042000C,(Inp32(0x1042000C)&(~(0x1<<30)))|(0x1<<30));
	Outp32(0x10420000,Inp32(0x10420000)&(~(0x1<<26)));
	Outp32(0x1042000C,(Inp32(0x1042000C)&(~(0xf<<12)))|(0xB<<12));
	Outp32(0x1042000C,(Inp32(0x1042000C)&(~(0xf<<16)))|(0x6<<16));
	Outp32(0x1042000C,(Inp32(0x1042000C)&(~(0xF<<20)))|(0x1<<20));

	Outp32(0x10460018,Inp32(0x10460018)&(~(0xF<<0)));
	Outp32(0x104600C0,(Inp32(0x104600C0)&(~(0x1<<20)))|(0x1<<20));
	Outp32(0x1046000C,(Inp32(0x1046000C)&(~(0x1<<30)))|(0x1<<30));
	Outp32(0x10460000,Inp32(0x10460000)&(~(0x1<<26)));
	Outp32(0x1046000C,(Inp32(0x1046000C)&(~(0xf<<12)))|(0xB<<12));
	Outp32(0x1046000C,(Inp32(0x1046000C)&(~(0xf<<16)))|(0x6<<16));
	Outp32(0x1046000C,(Inp32(0x1046000C)&(~(0xF<<20)))|(0x1<<20));

}

void DMC_Calibration(void)
{
	//-----------------------------------------------------------------------
	//--- CA Cal : temporary		                ---
	//-----------------------------------------------------------------------
	Outp32(0x10420004,(Inp32(0x10420004)&(~(0x1<<0)))|(0x1<<0));
	Outp32(0x10420004,(Inp32(0x10420004)&(~(0x1<<2)))|(0x1<<2));

	Outp32(0x10420060,(Inp32(0x10420060)&(~(0x3FF<<16)))|(0x3FF<<16));
	Outp32(0x10420004,(Inp32(0x10420004)&(~(0x1<<10)))|(0x1<<10));
	while(1)
	{
		u32 temp;
		temp= Inp32(0x10420004)&(0x1<<15);
		temp= temp>>15;
		if(temp == 1)
			break;
	}
	Outp32(0x10420004,Inp32(0x10420004)&(~(0x1<<10)));
	Outp32(0x10420004,Inp32(0x10420004)&(~(0x1<<0)));

	Outp32(0x10400010,0x50800);


	Outp32(0x10460004,(Inp32(0x10460004)&(~(0x1<<0)))|(0x1<<0));
	Outp32(0x10460004,(Inp32(0x10460004)&(~(0x1<<2)))|(0x1<<2));

	Outp32(0x10460060,(Inp32(0x10460060)&(~(0x3FF<<16)))|(0x3FF<<16));
	Outp32(0x10460004,(Inp32(0x10460004)&(~(0x1<<10)))|(0x1<<10));
	while(1)
	{
		u32 temp;
		temp= Inp32(0x10460004)&(0x1<<15);
		temp= temp>>15;
		if(temp == 1)
			break;
	}
	Outp32(0x10460004,Inp32(0x10460004)&(~(0x1<<10)));
	Outp32(0x10460004,Inp32(0x10460004)&(~(0x1<<0)));
	Outp32(0x10440010,0x50800);


	//-----------------------------------------------------------------------
	//--- Read Cal : temporary		                ---
	//-----------------------------------------------------------------------

	Outp32(0x1042000C,(Inp32(0x1042000C)&(~(0xf<<24)))|(0x4<<24));
	Outp32(0x1046000C,(Inp32(0x1046000C)&(~(0xf<<24)))|(0x4<<24));

	Outp32(0x10420004,Inp32(0x10420004)| (0x1<<3));
	Outp32(0x10420004,Inp32(0x10420004)| (0x1<<11));


	while(1)
	{
		u32 temp;
		temp= Inp32(0x10420004)&(0x1<<17);
		temp= temp>>17;
		if(temp == 1)
			break;
	}
	Outp32(0x10420004,Inp32(0x10420004)&(~(0x1<<11)));

	Outp32(0x10460004,Inp32(0x10460004)| (0x1<<3));
	Outp32(0x10460004,Inp32(0x10460004)| (0x1<<11));

	while(1)
	{
		u32 temp;
		temp= Inp32(0x10460004)&(0x1<<17);
		temp= temp>>17;
		if(temp == 1)
			break;
	}
	Outp32(0x10460004,Inp32(0x10460004)&(~(0x1<<11)));

	Outp32(0x10420004,Inp32(0x10420004)| (0x1<<22));
	Outp32(0x10460004,Inp32(0x10460004)| (0x1<<22));


}

int DMCInit_For_LPDDR3(int initial, int nEnableForce, u32 uPopOption)
{
	u32 lock_val;
	int re=true;
#if defined(CONFIG_SMC_CMD)
	reg_arr_t reg_arr;
#endif
	//u32 sw_n_warm_reset;
	u32 loop_cnt=0;
	int qa_test=true;											//- qa_test : "true"  -->  ¡ÚFor Only QA Test,         "false"  --> For the other tests
	MEMORY_SIZE_INFO mcp_option;
	u32 ap_ds;
	u32 rd_odt_en=1;											//- rd_odt_en=0 : Read ODT  -->  Disable,  rd_odt_en=1 : Read ODT  -->  Enable
	u32 zq_mode_term;
	u32 mem_ds;
	u32 wr_odt_term;
	u32 zq_manual_str=0x1;
	u32 gate_training_en;
	u32 wrapper_pd=1;											//-wrapper_pd = 0 --> Wrapper Power Down "Enable",   wrapper_pd = 1 --> Wrapper Power Down "Disable"

	//Assert(uPopOption<=1);										//- MCP Size : 0 --> 2GB,                  1 --> : 3GB

	//sw_n_warm_reset=(Inp32(0x105C0404)>>28)&0x3;				//- RESETSTATE(0x105C_0404) : [29]=SWRST,  [28]=WRESET

	if(initial == true)
	{
		InitMemClk(50);
	}

	//-----------------------------------------------------------------------
	//--- PHASE 1 : Initializing DREX                                  			                 ---
	//-----------------------------------------------------------------------
	Outp32( 0x10400310, 0x00000021 );					 		//- DREX CH0 : ¡ÚPIPELINE_CONFIG reg, dq_pipeline[6:4]=2, ca_pipeline[6:4]=1
	Outp32( 0x10440310, 0x00000021 );							//- DREX CH1 : ¡ÚPIPELINE_CONFIG reg, dq_pipeline[6:4]=2, ca_pipeline[6:4]=1

	Outp32( 0x10400000+0x0014, 0x0 );					 		//- DMC.PrechConfig0.tp_en=0x0, port_policy=0x0
	Outp32( 0x10400000+0x001C, 0xFFFFFFFF );					//- DMC.PrechConfig1 : tp_cnt3=0xFF, tp_cnt2=0xFF, tp_cnt1=0xFF, tp_cnt0=0xFF
	Outp32( 0x10440000+0x0014, 0x0 );					 		//- DMC.PrechConfig0.tp_en=0x0, port_policy=0x0
	Outp32( 0x10440000+0x001C, 0xFFFFFFFF );					//- DMC.PrechConfig1 : tp_cnt3=0xFF, tp_cnt2=0xFF, tp_cnt1=0xFF, tp_cnt0=0xFF

	Outp32( 0x10400000+0x0004, 0x202600 );						//- set dynamic powe down to OFF mode for CH0.
	Outp32( 0x10440000+0x0004, 0x202600 );						//- set dynamic powe down to OFF mode for CH1.

	Outp32( 0x10400000+0x0018, 0x3000 );						//- CH0. DMC.PHYCONTROL0.sl_dll_dyn_con=0x0
	Outp32( 0x10440000+0x0018, 0x3000 );						//- CH1. DMC.PHYCONTROL0.sl_dll_dyn_con=0x0

	Outp32( 0x10400000+0x0000, 0xFFF1100 ); 					//- CH0. DMC.PHYCONTROL.io_pd_con=0x0, DMC.CONCONTROL.update_mode[3]=0, aref disabled
	Outp32( 0x10440000+0x0000, 0xFFF1100 ); 					//- CH1. DMC.PHYCONTROL.io_pd_con=0x0, DMC.CONCONTROL.update_mode[3]=0, aref disabled

	Outp32( 0x10420000+0x004C, 0x0 );							//-  PHY0 : SHIFTC_CON0 : shiftc3 = shiftc2 = shiftc1= shiftc0=0x0
	Outp32( 0x10460000+0x004C, 0x0 );							//-  PHY1 : SHIFTC_CON0 : shiftc3 = shiftc2 = shiftc1= shiftc0=0x0

	if(initial == true)		//- Low Frequency PHY Setting @ POR reset, SW reset,Warm reset
	{
		#if 0
		Outp32( 0x10420020, 0x70707070 );					 	//-  PHY0 : OFFSETR_CON0 : offsetr3=0x70,offsetr2=0x70,offsetr1=0x70,offsetr0=0x70
		Outp32( 0x10420030, 0x70707070 );					 	//-  PHY0 : OFFSETW_CON0 : offsetw3=0x7F,offsetw2=0x7F,offsetw1=0x7F,offsetw0=0x7F
		Outp32( 0x10420050, 0x00000070 );					 	//-  PHY0 OFFSETD_CON0(0x50).upd_mode=0(PHY-Initiated Update Mode), offsetd=0x18
		Outp32( 0x10460020, 0x70707070 );					 	//-  PHY1 : OFFSETR_CON0 : offsetr3=0x70,offsetr2=0x70,offsetr1=0x70,offsetr0=0x70
		Outp32( 0x10460030, 0x70707070 );					 	//-  PHY1 : OFFSETW_CON0 : offsetw3=0x7F,offsetw2=0x7F,offsetw1=0x7F,offsetw0=0x7F
		Outp32( 0x10460050, 0x00000070 );					 	//-  PHY1 OFFSETD_CON0(0x50).upd_mode=0(PHY-Initiated Update Mode), offsetd=0x18

		Outp32(0x10420000+0x00B0, (Inp32(0x10420000+0x00B0)&~(0x1FF<<7))|(0x1FF<<7));		//- PHY0 : forcing lock value
		Outp32(0x10460000+0x00B0, (Inp32(0x10460000+0x00B0)&~(0x1FF<<7))|(0x1FF<<7));		//- PHY1 : forcing lock value

		Outp32(0x10420000+0x00B0, (Inp32(0x10420000+0x00B0)&~(0x1<<5)));		//- PHY0 : ctrl_dll_on=0
		Outp32(0x10460000+0x00B0, (Inp32(0x10460000+0x00B0)&~(0x1<<5)));		//- PHY1 : ctrl_dll_on=0

		//- Update DLL Information for CH0
		Outp32( 0x10420000+0x0050, 0x1000070 ); 				//- PHY0 ctrl_resync=1
		Outp32( 0x10420000+0x0050, 0x70 );					  	//- PHY0 ctrl_resync=0
		//- Update DLL Information for CH1
		Outp32( 0x10460000+0x0050, 0x1000070 ); 				//- PHY1 ctrl_resync=1
		Outp32( 0x10460000+0x0050, 0x70 );					  	//- PHY1 ctrl_resync=0
		#endif
	}

	//-- PHY CH0,1 : GNR_CON0 reg, ctrl_upd_mode[31:30]=0x1, ctrl_upd_range[29:28]=0x0, ctrl_dfdqs[26]=0x1, ctrl_ddr_mode[25:24]=0x3, ctrl_fnc_fb[23:21]=0x0
	//- 						   ctrl_wrlat[20:16]=0x7(WL+1), ctrl_bstlen[12:8]=0x8, ctrl_ckdis[6]=0x0, ctrl_cmosrcv[5]=0x0, ctrl_rdlat[4:0]=0xC
	Outp32( 0x10420000, 0x5709080E );                  		 	//- PHY0 : GNR_CON0, ¡Ú ctrl_upd_range=0x1 for 825MHz on SILICON
	Outp32( 0x10460000, 0x5709080E );                   	 	//- PHY1 : GNR_CON0, ¡Ú ctrl_upd_range=0x1 for 825MHz on SILICON

	Outp32( 0x10420018, 0x0000000f );					 		//- PHY0 : LP_CON0(0x18), Active HIGH signal to pull-up or down PDQS/NDQS signals.
	Outp32( 0x10460018, 0x0000000f );					 		//- PHY1 : LP_CON0(0x18), Active HIGH signal to pull-up or down PDQS/NDQS signals.

	Outp32( 0x10420064, 0x0000000E );							//- PHY0 : LP_DDR_CON3.cmd_read[12:0]=0x000E (LPDDR2/LPDDR3)
	Outp32( 0x10460064, 0x0000000E );							//- PHY1 : LP_DDR_CON3.cmd_read[12:0]=0x000E (LPDDR2/LPDDR3)

	Outp32( 0x10420068, 0x0000000F );							//- PHY0 : LP_DDR_CON4.cmd_default[12:0]=0x000F (LPDDR2/LPDDR3)
	Outp32( 0x10460068, 0x0000000F );							//- PHY1 : LP_DDR_CON4.cmd_default[12:0]=0x000F (LPDDR2/LPDDR3)

	//- ¡Ú Execute ZQ calibration
	// ¡Ø Note : Don't execute ZQ calibration for PHY0
	// ¡Ø Note : It needs to execute ZQ calibration for only PHY1
	//*** Setting AP IO Driver Strength ***
	//---------------------------------------------------------------
	//- DS : 0x4 : 48 ohm Impedance output driver
	//- DS : 0x5 : 40 ohm Impedance output driver
	//- DS : 0x6 : 34 ohm Impedance output driver
	//- DS : 0x7 : 30 ohm Impedance output driver
	//---------------------------------------------------------------
	//- Selection of AP DRV Option : DS -> 0x6(34 ohm)
	ap_ds=6;
	Outp32( 0x10420000+0x00C0, 0xE0C7304 );                     //- Set PHY0.ZQ_CON0.zq_mode_dds[26:24]=0x6, PHY0.ZQ_CON0.zq_manual_mode[3:2]=1
	Outp32( 0x10420000+0x00C0, (Inp32(0x10420000+0x00C0)&~(0x7<<24))|(ap_ds<<24));			//- Set PHY0.ZQ_CON0.zq_mode_dds[26:24]=0x6, PHY0.ZQ_CON0.zq_manual_mode[3:2]=1

	Outp32( 0x10460000+0x00C0, 0xE0C7304 );                     //- Set PHY1.ZQ_CON0.zq_mode_dds[26:24]=0x6, PHY1.ZQ_CON0.zq_manual_mode[3:2]=1
	Outp32( 0x10460000+0x00C0, (Inp32(0x10460000+0x00C0)&~(0x7<<24))|(ap_ds<<24) );                     //- Set PHY1.ZQ_CON0.zq_mode_dds[26:24]=0x6, PHY1.ZQ_CON0.zq_manual_mode[3:2]=1
	Outp32( 0x10460000+0x00C0, Inp32(0x10460000+0x00C0)|(zq_manual_str<<1) );				//- Set PHY1.ZQ_CON0.zq_manual_str[1]=1
	do{
		if(loop_cnt==100000){													//- Just Delay
			loop_cnt=0;
			break;
		}
		else					loop_cnt++;
	}while( ( Inp32( 0x10460000+0x00C4 ) & 0x1 ) != 0x1 );	        //- Wait for PHY1.ZQ_CON1.zq_done[0]
	Outp32( 0x10460000+0x00C0, Inp32(0x10460000+0x00C0)&~(zq_manual_str<<1) );                     //- Set PHY1.ZQ_CON0.zq_manual_str[1]=0

	//* Resync : Update DLL Info and Reset Read FIFO pointer
	//----------------------------------------------------------------------------------------
	if(initial == true)		//- for low speed(10~55MHz) at POR reset, SW reset,Warm reset
	{
		//- Update DLL Information for CH0
		Outp32( 0x10420000+0x0050, 0x1000070 ); 				//- PHY0 ctrl_resync=1
		Outp32( 0x10420000+0x0050, 0x70 );						//- PHY0 ctrl_resync=0
		//- Update DLL Information for CH1
		Outp32( 0x10460000+0x0050, 0x1000070 ); 				//- PHY1 ctrl_resync=1
		Outp32( 0x10460000+0x0050, 0x70 );						//- PHY1 ctrl_resync=0
	}
	else		//- for high speed(max) at Resume
	{
		//- Update DLL Information for CH0
		Outp32( 0x10420000+0x0050, 0x1000000 ); 				//- PHY0 ctrl_resync=1
		Outp32( 0x10420000+0x0050, 0x0 );						//- PHY0 ctrl_resync=0
		//- Update DLL Information for CH1
		Outp32( 0x10460000+0x0050, 0x1000000 ); 				//- PHY1 ctrl_resync=1
		Outp32( 0x10460000+0x0050, 0x0 );						//- PHY1 ctrl_resync=0
	}
	//----------------------------------------------------------------------------------------

	Outp32( 0x10420000+0x00C4, 0x3f0000 );                    	//- Set PHY0.ZQ_CON1.zq_vref[21:16]=0x20
	Outp32( 0x10460000+0x00C4, 0x3f0000 );                    	//- Set PHY1.ZQ_CON1.zq_vref[21:16]=0x20
	Outp32( 0x1042000c, 0x42101000 );                    		//- PHY0 : CAL_CON2(0x0C), ctrl_shgate=1
	Outp32( 0x1046000c, 0x42101000 );                   		//- PHY1 : CAL_CON2(0x0C), ctrl_shgate=1

	Outp32( 0x10400000+0x0000, 0xFFF1100 ); 				   	//- set CA Swap mode for CH0.
	Outp32( 0x10440000+0x0000, 0xFFF1100 ); 				   	//- set CA Swap mode for CH1.

	//* DFI PHY Initialization : CH0
	//----------------------------------------------------------------------------------------
	Outp32( 0x10400000+0x0000, 0x1FFF1100 );					//- assert dfi_init_start for CH0
	loop_cnt=0;
	do{
		if(loop_cnt==100000){													//- Just Delay
			loop_cnt=0;
			break;
		}
		else					loop_cnt++;
	}while( ( Inp32( 0x10400000+0x0040 ) & 0x8 ) != 0x8 );		//- Wait for DMC.dfi_init_complete_ch0
	Outp32( 0x10400000+0x0000, 0xFFF1100 ); 				   	//- deassert dfi_init_start for CH0

	Outp32( 0x10440000+0x0000, 0x1FFF1100 );					//- assert dfi_init_start for CH1
	loop_cnt=0;
	do{
		if(loop_cnt==100000){													//- Just Delay
			loop_cnt=0;
			break;
		}
		else					loop_cnt++;
	}while( ( Inp32( 0x10440000+0x0040 ) & 0x8 ) != 0x8 );		//- Wait for DMC.dfi_init_complete_ch1
	Outp32( 0x10440000+0x0000, 0xFFF1100 ); 				   	//- deassert dfi_init_start for CH1

	if(initial == true)		//- Low Frequency PHY Setting @ POR reset, SW reset,Warm reset
	{
		//- Added by cju, 2014.06.24
		Outp32( 0x10420020, 0x70707070 );						//-  PHY0 : OFFSETR_CON0 : offsetr3=0x70,offsetr2=0x70,offsetr1=0x70,offsetr0=0x70
		Outp32( 0x10420030, 0x70707070 );						//-  PHY0 : OFFSETW_CON0 : offsetw3=0x7F,offsetw2=0x7F,offsetw1=0x7F,offsetw0=0x7F
		Outp32( 0x10420050, 0x00000070 );						//-  PHY0 OFFSETD_CON0(0x50).upd_mode=0(PHY-Initiated Update Mode), offsetd=0x18
		Outp32( 0x10460020, 0x70707070 );						//-  PHY1 : OFFSETR_CON0 : offsetr3=0x70,offsetr2=0x70,offsetr1=0x70,offsetr0=0x70
		Outp32( 0x10460030, 0x70707070 );						//-  PHY1 : OFFSETW_CON0 : offsetw3=0x7F,offsetw2=0x7F,offsetw1=0x7F,offsetw0=0x7F
		Outp32( 0x10460050, 0x00000070 );						//-  PHY1 OFFSETD_CON0(0x50).upd_mode=0(PHY-Initiated Update Mode), offsetd=0x18

		Outp32(0x10420000+0x00B0, (Inp32(0x10420000+0x00B0)&~(0x1FF<<7))|(0x1FF<<7));		//- PHY0 : forcing lock value
		Outp32(0x10460000+0x00B0, (Inp32(0x10460000+0x00B0)&~(0x1FF<<7))|(0x1FF<<7));		//- PHY1 : forcing lock value

		Outp32(0x10420000+0x00B0, (Inp32(0x10420000+0x00B0)&~(0x1<<5)));		//- PHY0 : ctrl_dll_on=0
		Outp32(0x10460000+0x00B0, (Inp32(0x10460000+0x00B0)&~(0x1<<5)));		//- PHY1 : ctrl_dll_on=0
	}

	//* Resync : Update DLL Info and Reset Read FIFO pointer
	//----------------------------------------------------------------------------------------
	if(initial == true)		//- for low speed(10~55MHz) at POR reset, SW reset,Warm reset
	{
		//- Update DLL Information for CH0
		Outp32( 0x10420000+0x0050, 0x1000070 ); 				//- PHY0 ctrl_resync=1
		Outp32( 0x10420000+0x0050, 0x70 );						//- PHY0 ctrl_resync=0
		//- Update DLL Information for CH1
		Outp32( 0x10460000+0x0050, 0x1000070 ); 				//- PHY1 ctrl_resync=1
		Outp32( 0x10460000+0x0050, 0x70 );						//- PHY1 ctrl_resync=0
	}
	else		//- for high speed(max) at Resume
	{
		//- Update DLL Information for CH0
		Outp32( 0x10420000+0x0050, 0x1000000 ); 				//- PHY0 ctrl_resync=1
		Outp32( 0x10420000+0x0050, 0x0 );						//- PHY0 ctrl_resync=0
		//- Update DLL Information for CH1
		Outp32( 0x10460000+0x0050, 0x1000000 ); 				//- PHY1 ctrl_resync=1
		Outp32( 0x10460000+0x0050, 0x0 );						//- PHY1 ctrl_resync=0
	}
	//----------------------------------------------------------------------------------------

	Outp32( 0x10400000+0x0018, 0x5000 );						//- Set Recovery Time From PHY clock Gate for CH0 : rec_gate_cyc --> "5"
	Outp32( 0x10440000+0x0018, 0x5000 );						//- Set Recovery Time From PHY clock Gate for CH1 : rec_gate_cyc --> "5"

	Outp32( 0x10400004, 0x8312700 );					 		//- DREX0 : num_chunk[19:18]=0x0 (1 chunk enable)
	Outp32( 0x10440004, 0x8312700 );					 		//- DREX1 : num_chunk[19:18]=0x0 (1 chunk enable)

	Outp32( 0x104000E0, 0x00000000 );					 		//-  DREX0 Timing Parameter Set #0 Enable
	Outp32( 0x104400E0, 0x00000000 );					 		//-  DREX1 Timing Parameter Set #0 Enable

	if(!qa_test){
		#if 0
		Outp32( 0x10400030, 0x000B005D );						//-  DREX0 : rclk=24MHz, t_refipb=0x4875us*24MHz=11(0xB), t_refi=3.9us*24MHz(0x5D)
		Outp32( 0x10440030, 0x000B005D );						//-  DREX1 : rclk=24MHz, t_refipb=0x4875us*24MHz=11(0xB), t_refi=3.9us*24MHz(0x5D)
		#else
		Outp32( 0x10400030, 0x0005002E );						//-  DREX0 : rclk=24MHz, t_refipb=0x4875us*24MHz=11(0xB), t_refi=3.9us*24MHz(0x5D)
		Outp32( 0x10440030, 0x0005002E );						//-  DREX1 : rclk=24MHz, t_refipb=0x4875us*24MHz=11(0xB), t_refi=3.9us*24MHz(0x5D)
		#endif
	}
	else{														//- ¡Ú for just QA Test
		Outp32( 0x10400030, 0x0005002E );						//-  DREX0 : rclk=24MHz, t_refipb=0x4875us*24MHz=11(0xB), t_refi=3.9us*24MHz(0x5D)
		Outp32( 0x10440030, 0x0005002E );						//-  DREX1 : rclk=24MHz, t_refipb=0x4875us*24MHz=11(0xB), t_refi=3.9us*24MHz(0x5D)
	}


	Outp32( 0x10400044, 0x00002270 );					 		//-  DREX0, ETCTIMING, t_mrr[13:12]=0x2(for LPDDR3), t_srr[11:10]=0x2(for WideIO), t_src[7:4]=0x7(for WideIO)
	Outp32( 0x10440044, 0x00002270 );					 		//-  DREX1, ETCTIMING, t_mrr[13:12]=0x2(for LPDDR3), t_srr[11:10]=0x2(for WideIO), t_src[7:4]=0x7(for WideIO)

	Outp32( 0x1040004C, 0x00000003 );					 		//-  DREX0, RDFETCH0(timing parameter set #0)
	Outp32( 0x1044004C, 0x00000003 );					 		//-  DREX1, RDFETCH0(timing parameter set #0)


	Outp32( 0x10400034, 0x575A9713 );                    		//-  TIMINGROW0 : DREX0, CLK2X=825MHz
	Outp32( 0x10440034, 0x575A9713 );                    		//-  TIMINGROW0 : DREX1, CLK2X=825MHz
	Outp32( 0x10400038, 0x4740085E );                    		//-  TIMINGDATA0 : DREX0, CLK2X=825MHz
	Outp32( 0x10440038, 0x4740085E );                   		//-  TIMINGDATA0 : DREX1, CLK2X=825MHz
	Outp32( 0x1040003C, 0x54AA0446 );                    		//-  tXSR 220nsec - > 410nsec TIMINGPOWER0 : DREX0, CLK2X=825MHz
	Outp32( 0x1044003C, 0x54AA0446 );                    		//-  tXSR 220nsec - > 410nsec TIMINGPOWER0 : DREX1, CLK2X=825MHz

	Outp32( 0x10400058, 0x50000000 );                    		//-  TermControl0 : DREX0, CLK2X=800MHz
	Outp32( 0x10440058, 0x50000000 );                    		//-  TermControl0 : DREX1, CLK2X=800MHz

	Outp32( 0x10400050, 0x00000003 );                    		//-  DREX0, RDFETCH1(timing parameter set #1)
	Outp32( 0x10440050, 0x00000003 );                    		//-  DREX1, RDFETCH1(timing parameter set #1)

	Outp32( 0x104000E4, 0x575A9713 );                    		//-  TIMINGROW1 : DREX0, CLK2X=825MHz
	Outp32( 0x104400E4, 0x575A9713 );                    		//-  TIMINGROW1 : DREX1, CLK2X=825MHz
	Outp32( 0x104000E8, 0x4740085E );                    		//-  TIMINGDATA1 : DREX0, CLK2X=825MHz
	Outp32( 0x104400E8, 0x4740085E );                    		//-  TIMINGDATA1 : DREX1, CLK2X=825MHz
	Outp32( 0x104000EC, 0x54AA0446 );                    		//-  tXSR 220nsec - > 410nsec TIMINGPOWER1 : DREX0, CLK2X=825MHz
	Outp32( 0x104400EC, 0x54AA0446 );                    		//-  tXSR 220nsec - > 410nsec TIMINGPOWER1 : DREX1, CLK2X=825MHz

	Outp32( 0x1040005C, 0x50000000 );                    		//-  TermControl1 : DREX0, CLK2X=800MHz
	Outp32( 0x1044005C, 0x50000000 );                    		//-  TermControl1 : DREX1, CLK2X=800MHz

	Outp32( 0x10400000+0x0100, 0x0 );					 		//- CH0 brbrsvcontrol
	Outp32( 0x10440000+0x0100, 0x0 );					 		//- CH1 brbrsvcontrol
	Outp32( 0x10400000+0x0104, 0x88888888 );					//- CH0 brbrsvconfig
	Outp32( 0x10440000+0x0104, 0x88888888 );					//- CH1 brbrsvconfig

		//*** Setting Memory Driver Strength & ODT Termination ***
		//---------------------------------------------------------------
		//- DS : 0x1 : 34.3 ohm typical pull-down/pull-up
		//- DS : 0x2 : 40 ohm typical pull-down/pull-up (default)
		//- DS : 0x3 : 48 ohm typical pull-down/pull-up
		//- DS : 0x9 : 34.3 ohm typical pull-down/40 ohm typical pull-up
		//- DS : 0xA : 40 ohm typical pull-down/48 ohm typical pull-up
		//- DS : 0xB : 34.3 ohm typical pull-down/48 ohm typical pull-up

		//- ODT Term : 0x0 : Disable (disable)
		//- ODT Term : 0x1 : RZQ/4(60 ohm, for 933MHz / 1066MHz)
		//- ODT Term : 0x2 : RZQ/2(120 ohm)
		//- ODT Term : 0x3 : RZQ/1(240 ohm)
		//---------------------------------------------------------------
	//- Selection of Memory Option : DS -> 0x2
	mem_ds=2;

	//-Setting ODT feature in DREX side for Memory ODT(LPDDR3) when executing write operation.
	//- mem_term_en(Controlling ODT Pin for write operation) -> 0x0
	//- mem_term_type_single=0x0
	//- mem_term_type_all=0x0
	//- t_odte=0x5
	#if 1
	wr_odt_term=0;

	if(wr_odt_term > 0){
		Outp32(0x10400000 + 0x0058, Inp32(0x10400000 + 0x0058)|(0x1<<3));			//- CH0 timing set#0 : mem_term_en=1
		Outp32(0x10400000 + 0x005C, Inp32(0x10400000 + 0x005C)|(0x1<<3));			//- CH0 timing set#1 : mem_term_en=1
		Outp32(0x10440000 + 0x0058, Inp32(0x10440000 + 0x0058)|(0x1<<3));			//- CH1 timing set#0 : mem_term_en=1
		Outp32(0x10440000 + 0x005C, Inp32(0x10440000 + 0x005C)|(0x1<<3));			//- CH1 timing set#1 : mem_term_en=1
	}
	#endif


	//-----------------------------------------------------------------------
	//--- PHASE 2 : Issuing DIRECTCMD, for LPDDR3 @ 0x800			                ---
	//-----------------------------------------------------------------------
	//¡Ú Assume that DRAM interface of Helsinki has only two channels and two chips per channel.

	//if((initial == true)&&(!sw_n_warm_reset))			// That means that sleep & wakeup is not , or This is for all reset state
	if(initial == true)
	{
		#if 1
			re=Init_Mem(mem_ds,wr_odt_term);
		#else
		//- [DREX0]
		//-------------------------------------------
		//- [CS0]
		Outp32( 0x10400010, 0x07000000 );					 	//- NOP
		DMC_Delay(200); 									 	//- Wait for 200us
		Outp32( 0x10400010, 0x00071C00 );					 	//- MR63(MA[7:0]=0x3F), Reset
		DMC_Delay(1); 										 	//- Wait for 1us
		re=Check_DAI_Status(0,0);								 	//- Check if DAI is complete.
		Outp32( 0x10400010, 0x00010BFC );					 	//- MR10(MA[7:0]=0x0A), Calibration, OP[7:0]=0xFF(Calibration command after initialization)
		DMC_Delay(1); 										 	//- Wait for 1us
		Outp32( 0x10400010, 0x00000870 );					 	//- MR2 (MA[7:0]=0x02), RL/WL 	 , RL&WL=OP[3:0]=0xC(RL/WL=14/8,for 825MHz), nWRE=OP[4]=1 (WR > 9(667MHz))
		Outp32( 0x10400010, 0x0000060C );					 	//- MR1 (MA[7:0]=0x01), nWR		 , BL=OP[2:0]=0x3, nWR=OP[7:5]=4 (for 825MHz)

		//- [CS1]
		Outp32( 0x10400010, 0x07100000 );					 	//- NOP
		DMC_Delay(200); 									 	//- Wait for 200us
		Outp32( 0x10400010, 0x00171C00 );					 	//- MR63(MA[7:0]=0x3F), Reset
		DMC_Delay(1); 										 	//- Wait for 1us
		re=Check_DAI_Status(0,1);								 	//- Check if DAI is complete.
		Outp32( 0x10400010, 0x00110BFC );					 	//- MR10(MA[7:0]=0x0A), Calibration, OP[7:0]=0xFF(Calibration command after initialization)
		DMC_Delay(1); 										 	//- Wait for 1us
		Outp32( 0x10400010, 0x00100870 );					 	//- MR2 (MA[7:0]=0x02), RL/WL 	 , RL&WL=OP[3:0]=0xC(RL/WL=14/8,for 825MHz), nWRE=OP[4]=1 (WR > 9(667MHz))
		Outp32( 0x10400010, 0x0010060C );					 	//- MR1 (MA[7:0]=0x01), nWR		 , BL=OP[2:0]=0x3, nWR=OP[7:5]=4 (for 825MHz)
		//-------------------------------------------

		//- [DREX1]
		//-------------------------------------------
		//- [CS0]
		Outp32( 0x10440010, 0x07000000 );					 	//- NOP
		DMC_Delay(200); 									 	//- Wait for 200us
		Outp32( 0x10440010, 0x00071C00 );					 	//- MR63(MA[7:0]=0x3F), Reset
		DMC_Delay(1); 										 	//- Wait for 1us
		re=Check_DAI_Status(1,0);								 	//- Check if DAI is complete.
		Outp32( 0x10440010, 0x00010BFC );					 	//- MR10(MA[7:0]=0x0A), Calibration, OP[7:0]=0xFF(Calibration command after initialization)
		DMC_Delay(1); 										 	//- Wait for 1us
		Outp32( 0x10440010, 0x00000870 );					 	//- MR2 (MA[7:0]=0x02), RL/WL 	 , RL&WL=OP[3:0]=0xA(RL/WL=12/6,for 800MHz), nWRE=OP[4]=1 (WR > 9(600MHz))
		Outp32( 0x10440010, 0x0000060C );					 	//- MR1 (MA[7:0]=0x01), nWR		 , BL=OP[2:0]=0x3, nWR=OP[7:5]=2 (for 800MHz)

		//- [CS1]
		Outp32( 0x10440010, 0x07100000 );					 	//- NOP
		DMC_Delay(200); 									 	//- Wait for 200us
		Outp32( 0x10440010, 0x00171C00 );					 	//- MR63(MA[7:0]=0x3F), Reset
		DMC_Delay(1); 										 	//- Wait for 1us
		re=Check_DAI_Status(1,1);								 	//- Check if DAI is complete.
		Outp32( 0x10440010, 0x00110BFC );					 	//- MR10(MA[7:0]=0x0A), Calibration, OP[7:0]=0xFF(Calibration command after initialization)
		DMC_Delay(1); 										 	//- Wait for 1us
		Outp32( 0x10440010, 0x00100870 );					 	//- MR2 (MA[7:0]=0x02), RL/WL 	 , RL&WL=OP[3:0]=0xA(RL/WL=12/6,for 800MHz), nWRE=OP[4]=1 (WR > 9(600MHz))
		Outp32( 0x10440010, 0x0010060C );					 	//- MR1 (MA[7:0]=0x01), nWR		 , BL=OP[2:0]=0x3, nWR=OP[7:5]=2 (for 800MHz)
		//-------------------------------------------


		//*** Setting Memory Driver Strength & ODT Termination ***
		//---------------------------------------------------------------
		//- DS : 0x1 : 34.3 ohm typical pull-down/pull-up
		//- DS : 0x2 : 40 ohm typical pull-down/pull-up (default)
		//- DS : 0x3 : 48 ohm typical pull-down/pull-up
		//- DS : 0x9 : 34.3 ohm typical pull-down/40 ohm typical pull-up
		//- DS : 0xA : 40 ohm typical pull-down/48 ohm typical pull-up
		//- DS : 0xB : 34.3 ohm typical pull-down/48 ohm typical pull-up

		//- ODT Term : 0x0 : Disable (disable)
		//- ODT Term : 0x1 : RZQ/4(60 ohm, for 933MHz / 1066MHz)
		//- ODT Term : 0x2 : RZQ/2(120 ohm)
		//- ODT Term : 0x3 : RZQ/1(240 ohm)
		//---------------------------------------------------------------
		//- Selection of Memory Option : DS -> 0x2,   ODT Termination -> 0x0
		Outp32( (0x10400000+0x0010), 0xC08 );					//- CH0, CS0. mr3 command
		DMC_Delay(0x100); //- wait 1ms
		Outp32( (0x10400000+0x0010), 0x10C00 ); 				//- CH0, CS0. mr11 command
		DMC_Delay(0x10000); //- wait 100ms
		Outp32( (0x10400000+0x0010), 0x100C08 );				//- CH0, CS1. mr3 command
		DMC_Delay(0x100); //- wait 1ms
		Outp32( (0x10400000+0x0010), 0x110C00 );				//- CH0, CS1. mr11 command
		DMC_Delay(0x10000); //- wait 100ms
		Outp32( (0x10440000+0x0010), 0xC08 );					//- CH1, CS0. mr3 command
		DMC_Delay(0x100); //- wait 1ms
		Outp32( (0x10440000+0x0010), 0x10C00 ); 				//- CH1, CS0. mr11 command
		DMC_Delay(0x10000); //- wait 100ms
		Outp32( (0x10440000+0x0010), 0x100C08 );				//- CH1, CS1. mr3 command
		DMC_Delay(0x100); //- wait 1ms
		Outp32( (0x10440000+0x0010), 0x110C00 );				//- CH1, CS1. mr11 command
		DMC_Delay(0x10000); //- wait 100ms
		#endif
	}
	else{				//- Sleep & Wakeup, SW reset, Warm Reset
		Outp32( (0x10400000+0x0010), 0x08000000 ); 				//- CH0, CS0. Self Refresh Exit Command for only sleep & wakeup
		Outp32( (0x10400000+0x0010), 0x08100000 ); 				//- CH0, CS0. Self Refresh Exit Command for only sleep & wakeup
		Outp32( (0x10440000+0x0010), 0x08000000 ); 				//- CH0, CS0. Self Refresh Exit Command for only sleep & wakeup
		Outp32( (0x10440000+0x0010), 0x08100000 ); 				//- CH0, CS0. Self Refresh Exit Command for only sleep & wakeup
	}

	//* Setting PHY High Speed Offset before DLL is On and Start
	//----------------------------------------------------------------------------------------
	Outp32( 0x10420020, 0x0 );					  				//-  PHY0 : OFFSETR_CON0 : offsetr3=0x08,offsetr2=0x08,offsetr1=0x08,offsetr0=0x08
	Outp32( 0x10420030, 0x0 );					  				//-  PHY0 : OFFSETW_CON0 : offsetw3=0x08,offsetw2=0x08,offsetw1=0x08,offsetw0=0x08
	Outp32( 0x10420050, 0x0 );					  				//-  PHY0 OFFSETD_CON0(0x50).upd_mode=0(PHY-Initiated Update Mode), offsetd=0x08
	Outp32( 0x10460020, 0x0 );					  				//-  PHY1 : OFFSETR_CON0 : offsetr3=0x08,offsetr2=0x08,offsetr1=0x08,offsetr0=0x08
	Outp32( 0x10460030, 0x0 );					  				//-  PHY1 : OFFSETW_CON0 : offsetw3=0x08,offsetw2=0x08,offsetw1=0x08,offsetw0=0x08
	Outp32( 0x10460050, 0x0 );					  				//-  PHY1 OFFSETD_CON0(0x50).upd_mode=0(PHY-Initiated Update Mode), offsetd=0x08

	//- Update DLL Information for CH0
	Outp32( 0x10420000+0x0050, 0x1000000 ); 				   	//- PHY0 ctrl_resync=1
	Outp32( 0x10420000+0x0050, 0x0 );					 		//- PHY0 ctrl_resync=0
	//- Update DLL Information for CH1
	Outp32( 0x10460000+0x0050, 0x1000000 ); 				   	//- PHY1 ctrl_resync=1
	Outp32( 0x10460000+0x0050, 0x0 );					 		//- PHY1 ctrl_resync=0


	//-----------------------------------------------------------------------
	//--- Executing DLL locking Process 								  ---
	//--- And Then, 													  ---
	//--- If training is required, Please execute it.					  ---
	//-----------------------------------------------------------------------
	if(initial == true)
	{
		InitMemClk(825);
	}

	Outp32( 0x10420000+0x00B0, 0x10100010|(0xF<<1) );                    //-  PHY0 : ctrl_dll_on[5] = 0, ctrl_start = 0, ctrl_ref=0xF
	Outp32( 0x10420000+0x00B0, 0x10100030|(0xF<<1) );                    //-  PHY0 : ctrl_dll_on[5] = 1, ctrl_ref=0xF
	Outp32( 0x10420000+0x00B0, 0x10100070|(0xF<<1) );                    //-  PHY0 : ctrl_start[6] = 1, ctrl_ref=0xF
	loop_cnt=0;
	do{
		if(loop_cnt==100000){													//- Just Delay
			loop_cnt=0;
			break;
		}
		else					loop_cnt++;
	}while( ( Inp32( 0x10420000+0x00B4 ) & 0x5 ) != 0x5 );	    //- Wait until PHY0. DLL is locked!

	//- CH0 lock value forcing
	Outp32(0x10420000+0x00B0, Inp32(0x10420000+0x00B0)&~(0x1<<5));							//- PHY0 : ctrl_dll_on=0
	lock_val=(Inp32(0x10420000+0x00B4)>>8)&0x1FF;
	Outp32(0x10420000+0x00B0, Inp32(0x10420000+0x00B0)|(0x1<<5));							//- PHY0 : ctrl_dll_on=1
	Outp32(0x10420000+0x00B0, (Inp32(0x10420000+0x00B0)&~(0x1FF<<7))|(lock_val<<7));		//- PHY0 : forcing lock value
	//Outp32(0x10420000+0x00B0, Inp32(0x10420000+0x00B0)&~(0x1<<5));							//- PHY0 : ctrl_dll_on=0

	if (nEnableForce == TRUE)
	{
		//¡Ú¡Ú¡Ú Open for only EVT0 Silicon, the code below is to force lock value for System Power Mode
		Outp32(0x10420000+0x00B0, Inp32(0x10420000+0x00B0)&~(0x1<<5));							//- PHY0 : ctrl_dll_on=0
		//lock_val=(Inp32(0x10420000+0x00B4)>>8)&0x1FF;
		//Outp32(0x10420000+0x00B0, (Inp32(0x10420000+0x00B0)&~(0x1FF<<7))|(lock_val<<7));		//- PHY0 : forcing lock value
	}

	Outp32( 0x10460000+0x00B0, 0x10100010|(0xF<<1) );                    //-  PHY1 : ctrl_dll_on[5] = 0, ctrl_start = 0
	Outp32( 0x10460000+0x00B0, 0x10100030|(0xF<<1) );                    //-  PHY1 : ctrl_dll_on[5] = 1
	Outp32( 0x10460000+0x00B0, 0x10100070|(0xF<<1) );                    //-  PHY1 : ctrl_start[6] = 1
	loop_cnt=0;
	do{
		if(loop_cnt==100000){													//- Just Delay
			loop_cnt=0;
			break;
		}
		else					loop_cnt++;
	}while( ( Inp32( 0x10460000+0x00B4 ) & 0x5 ) != 0x5 );	    //- Wait until PHY1. DLL is locked!

	//- CH1 lock value forcing
	Outp32(0x10460000+0x00B0, Inp32(0x10460000+0x00B0)&~(0x1<<5));							//- PHY1 : ctrl_dll_on=0
	lock_val=(Inp32(0x10460000+0x00B4)>>8)&0x1FF;
	Outp32(0x10460000+0x00B0, Inp32(0x10460000+0x00B0)|(0x1<<5));							//- PHY1 : ctrl_dll_on=1
	Outp32(0x10460000+0x00B0, (Inp32(0x10460000+0x00B0)&~(0x1FF<<7))|(lock_val<<7));		//- PHY1 : forcing lock value

	if (nEnableForce == TRUE)
	{
		//¡Ú¡Ú¡Ú Open for only EVT0 Silicon, the code below is to force lock value for System Power Mode
		Outp32(0x10460000+0x00B0, Inp32(0x10460000+0x00B0)&~(0x1<<5));							//- PHY1 : ctrl_dll_on=0
		//lock_val=(Inp32(0x10460000+0x00B4)>>8)&0x1FF;
		//Outp32(0x10460000+0x00B0, (Inp32(0x10460000+0x00B0)&~(0x1FF<<7))|(lock_val<<7));		//- PHY1 : forcing lock value
	}

	//* Resync : Update DLL Info and Reset Read FIFO pointer
	//----------------------------------------------------------------------------------------
	//- Update DLL Information for CH0
	Outp32( 0x10420000+0x0050, 0x1000000 ); 				   	//- PHY0 ctrl_resync=1
	Outp32( 0x10420000+0x0050, 0x0 );					 		//- PHY0 ctrl_resync=0
	//- Update DLL Information for CH1
	Outp32( 0x10460000+0x0050, 0x1000000 ); 				   	//- PHY1 ctrl_resync=1
	Outp32( 0x10460000+0x0050, 0x0 );						 	//- PHY1 ctrl_resync=0

	//- Configuration of ODT function for read operation
	//*** Setting On-Die-Termination(ODT) resistor value for READ operation ***
	//---------------------------------------------------------------
	//- 0x1 : 120 ohm Receiver Termination
	//- 0x2 : 60 ohm Receiver Termination
	//- 0x3 : 40 ohm Receiver Termination
	//- 0x4 : 30 ohm Receiver Termination
	//---------------------------------------------------------------
	//- Selection of ODT termination Option : 0x4(30 ohm)
	#if 0
	if(rd_odt_en){
		zq_mode_term=0x0;
		gate_training_en=0;																			//- "0" : The gate training function is not used

		//Outp32(0x10420000 + 0x0018, Inp32(0x10420000 + 0x0018)&~(0xF<<0));							//- CH0 LP_CON0[3:0]=ctrl_pulld_dqs=0x0
		//Outp32(0x10460000 + 0x0018, Inp32(0x10460000 + 0x0018)&~(0xF<<0));							//- CH1 LP_CON0[3:0]=ctrl_pulld_dqs=0x0

		Outp32(0x10420000 + 0x00C0, (Inp32(0x10420000 + 0x00C0)&~((0x1<<19)|(0x7<<21)))|(zq_mode_term<<21));		//- CH0 ZQ_CON0[20]=zq_rgddr3=1, zq_mode_noterm=0
		Outp32(0x10460000 + 0x00C0, (Inp32(0x10460000 + 0x00C0)&~((0x1<<19)|(0x7<<21)))|(zq_mode_term<<21));		//- CH1 ZQ_CON0[20]=zq_rgddr3=1, zq_mode_noterm=0

		Outp32(0x10420000 + 0x000C, (Inp32(0x10420000 + 0x000C)&~(0x1<<30)));							//- CH0 CAL_CON2[30]=ctrl_read_dis=0
		Outp32(0x10460000 + 0x000C, (Inp32(0x10460000 + 0x000C)&~(0x1<<30)));							//- CH1 CAL_CON2[30]=ctrl_read_dis=0

		#if 0
		if(gate_training_en){
			Outp32(0x10420000, Inp32(0x10420000)&~(0x1<<26));										//- CH0 GNR_CON0[26]=ctrl_dfdqs=0
			Outp32(0x10460000, Inp32(0x10460000)&~(0x1<<26));										//- CH1 GNR_CON0[26]=ctrl_dfdqs=0

			Outp32(0x10420000 + 0x000C, (Inp32(0x10420000 + 0x000C)&~(0xF<<12))|(0x9<<12));		//- CH0 CAL_CON2[15:12]=ctrl_readadj=9
			Outp32(0x10460000 + 0x000C, (Inp32(0x10460000 + 0x000C)&~(0xF<<12))|(0x9<<12));		//- CH1 CAL_CON2[15:12]=ctrl_readadj=9

			Outp32(0x10420000 + 0x000C, (Inp32(0x10420000 + 0x000C)&~(0xF<<16))|(0x4<<16));		//- CH0 CAL_CON2[19:16]=ctrl_readduradj=4
			Outp32(0x10460000 + 0x000C, (Inp32(0x10460000 + 0x000C)&~(0xF<<16))|(0x4<<16));		//- CH1 CAL_CON2[19:16]=ctrl_readduradj=4

			Outp32(0x10420000 + 0x000C, (Inp32(0x10420000 + 0x000C)&~(0xF<<20))|(0x4<<20));		//- CH0 CAL_CON2[23:20]=ctrl_gateadj=4
			Outp32(0x10460000 + 0x000C, (Inp32(0x10460000 + 0x000C)&~(0xF<<20))|(0x4<<20));		//- CH1 CAL_CON2[23:20]=ctrl_gateadj=4
		}
		else{
			Outp32(0x10420000, Inp32(0x10420000)|(0x1<<26));		//- CH0 GNR_CON0[26]=ctrl_dfdqs=0
			Outp32(0x10460000, Inp32(0x10460000)|(0x1<<26));		//- CH1 GNR_CON0[26]=ctrl_dfdqs=0

			Outp32(0x10420000 + 0x000C, (Inp32(0x10420000 + 0x000C)&~(0xF<<12))|(0x9<<12));		//- CH0 CAL_CON2[15:12]=ctrl_readadj=9
			Outp32(0x10460000 + 0x000C, (Inp32(0x10460000 + 0x000C)&~(0xF<<12))|(0x9<<12));		//- CH1 CAL_CON2[15:12]=ctrl_readadj=9

			Outp32(0x10420000 + 0x000C, (Inp32(0x10420000 + 0x000C)&~(0xF<<16))|(0x4<<16));		//- CH0 CAL_CON2[19:16]=ctrl_readduradj=4
			Outp32(0x10460000 + 0x000C, (Inp32(0x10460000 + 0x000C)&~(0xF<<16))|(0x4<<16));		//- CH1 CAL_CON2[19:16]=ctrl_readduradj=4

			Outp32(0x10420000 + 0x000C, (Inp32(0x10420000 + 0x000C)&~(0xF<<20))|(0x4<<20));		//- CH0 CAL_CON2[23:20]=ctrl_gateadj=4
			Outp32(0x10460000 + 0x000C, (Inp32(0x10460000 + 0x000C)&~(0xF<<20))|(0x4<<20));		//- CH1 CAL_CON2[23:20]=ctrl_gateadj=4
		}
		#endif
	}
	#endif


	//-----------------------------------------------------------------------
	//--- PHASE 3 : Detecting DRAM size information by reading MR8		                ---
	//-----------------------------------------------------------------------
	//¡Ú Assume that DRAM interface of Helsinki has only two channels and two chips per channel.
#if 1

	mcp_option=DMC_Detecting_LP3_SizeInfo(0,0,LPDDR3_MR_8);

	if(mcp_option==LP3_3GB_6Gb_DIE)
	{
#ifdef CONFIG_CPU_EXYNOS5433_EVT1
		Outp64(CONFIG_PHY_BL_SYS_INFO_DRAM_SIZE, 0xC0000000);
#endif
#if defined(CONFIG_SMC_CMD)
	       reg_arr.set0.addr = 0x10410f10;
	       reg_arr.set0.val = 0x4E2333;
	       reg_arr.set1.addr = 0x10410f14;
	       reg_arr.set1.val = 0x4E2333;
	       reg_arr.set4.addr = 0x10450f10;
	       reg_arr.set4.val = 0x4E2333;
	       reg_arr.set5.addr = 0x10450f14;
	       reg_arr.set5.val = 0x4E2333;
	       reg_arr.set2.addr = 0x10410f00;
	       reg_arr.set2.val = 0x00010006;
	       reg_arr.set6.addr = 0x10450f00;
	       reg_arr.set6.val = 0x00010006;
	       reg_arr.set3.addr = 0x10410f20;
	       reg_arr.set3.val = 0x00000002;
	       reg_arr.set7.addr = 0x10450f20;
	       reg_arr.set7.val = 0x00000002;
	       set_secure_reg((u32)&reg_arr, 8);
#else
		Outp32( 0x10410000+0x0f10, 0x4E2333 );					  //- CH0. MemConfig0 : bank_lsb=0x4, rank_inter_en=0x0, bit_sel_en=0x0, bit_sel=0x2
		Outp32( 0x10410000+0x0f14, 0x4E2333 );					  //- CH0. MemConfig1 : bank_lsb=0x4, rank_inter_en=0x0, bit_sel_en=0x0, bit_sel=0x2
		Outp32( 0x10450000+0x0f10, 0x4E2333 );					  //- CH1. MemConfig0 : bank_lsb=0x4, rank_inter_en=0x0, bit_sel_en=0x0, bit_sel=0x2
		Outp32( 0x10450000+0x0f14, 0x4E2333 );					  //- CH1. MemConfig1 : bank_lsb=0x4, rank_inter_en=0x0, bit_sel_en=0x0, bit_sel=0x2

		Outp32( 0x10410f00, 0x00010006 );                    //-  DREX0 chunk_start[23:16]=0x1 means that chunk base is 0x2(0x20000000), chunk_end[7:0]=0x6 means that chunk base is 0xD(0xD0000000)
		Outp32( 0x10450f00, 0x00010006 );                    //-  DREX1 chunk_start[23:16]=0x1 means that chunk base is 0x2(0x20000000), chunk_end[7:0]=0x6 means that chunk base is 0xD(0xD0000000)

		Outp32( 0x10410f20, 0x00000002 );                    //-  DREX0 chip0_size[7:0]=0x2 (chip size is 768MB)
		Outp32( 0x10450f20, 0x00000002 );                    //-  DREX1 chip0_size[7:0]=0x2 (chip size is 768MB)
#endif

		Outp32( 0x10400020, 0x00002626 );            //-  DREX0 : ACLK/CCLK=400MHz, CLK2X=825MHz(on SILICON), t_rfcpb1[13:8]=60ns/2.5ns=24(0x18)(for timing set #1), t_rfcpb0[5:0]=60ns/2.5ns=24(0x18)(for timing set #0)
		Outp32( 0x10440020, 0x00002626 );            //-  DREX1 : ACLK/CCLK=400MHz, CLK2X=825MHz(on SILICON), t_rfcpb1[13:8]=60ns/2.5ns=24(0x18)(for timing set #1), t_rfcpb0[5:0]=60ns/2.5ns=24(0x18)(for timing set #0)
	}
	else if(mcp_option==LP3_2GB_4Gb_DIE)
	{
#ifdef CONFIG_CPU_EXYNOS5433_EVT1
		Outp64(CONFIG_PHY_BL_SYS_INFO_DRAM_SIZE, 0x80000000);
#endif
#if defined(CONFIG_SMC_CMD)
	       reg_arr.set0.addr = 0x10410f10;
	       reg_arr.set0.val = 0x4E2323;
	       reg_arr.set1.addr = 0x10410f14;
	       reg_arr.set1.val = 0x4E2323;
	       reg_arr.set4.addr = 0x10450f10;
	       reg_arr.set4.val = 0x4E2323;
	       reg_arr.set5.addr = 0x10450f14;
	       reg_arr.set5.val = 0x4E2323;
	       reg_arr.set2.addr = 0x10410f00;
	       reg_arr.set2.val = 0x00010004;
	       reg_arr.set6.addr = 0x10450f00;
	       reg_arr.set6.val = 0x00010004;
	       reg_arr.set3.addr = 0x10410f20;
	       reg_arr.set3.val = 0x00000001;
	       reg_arr.set7.addr = 0x10450f20;
	       reg_arr.set7.val = 0x00000001;
	       set_secure_reg((u32)&reg_arr, 8);
#else
		Outp32( 0x10410000+0x0f10, 0x4E2323 );                    //- CH0. MemConfig0 : bank_lsb=0x4, rank_inter_en=0x0, bit_sel_en=0x0, bit_sel=0x2
		Outp32( 0x10410000+0x0f14, 0x4E2323 );                    //- CH0. MemConfig1 : bank_lsb=0x4, rank_inter_en=0x0, bit_sel_en=0x0, bit_sel=0x2
		Outp32( 0x10450000+0x0f10, 0x4E2323 );                    //- CH1. MemConfig0 : bank_lsb=0x4, rank_inter_en=0x0, bit_sel_en=0x0, bit_sel=0x2
		Outp32( 0x10450000+0x0f14, 0x4E2323 );                    //- CH1. MemConfig1 : bank_lsb=0x4, rank_inter_en=0x0, bit_sel_en=0x0, bit_sel=0x2

		Outp32( 0x10410f00, 0x00010004 );                    //-  DREX0 chunk_start[23:16]=0x1 means that chunk base is 0x2(0x20000000), chunk_end[7:0]=0x4 means that chunk base is 0x9(0x90000000)
		Outp32( 0x10450f00, 0x00010004 );                    //-  DREX1 chunk_start[23:16]=0x1 means that chunk base is 0x2(0x20000000), chunk_end[7:0]=0x6 means that chunk base is 0x9(0x90000000)

		Outp32( 0x10410f20, 0x00000001 );                    //-  DREX0 chip0_size[7:0]=0x1 (chip size is 512MB)
		Outp32( 0x10450f20, 0x00000001 );                    //-  DREX1 chip0_size[7:0]=0x1 (chip size is 512MB)
#endif
		Outp32( 0x10400020, 0x00001919 );                    //-  DREX0 : ACLK/CCLK=400MHz, CLK2X=825MHz(on SILICON), t_rfcpb1[13:8]=60ns/2.5ns=24(0x18)(for timing set #1), t_rfcpb0[5:0]=60ns/2.5ns=24(0x18)(for timing set #0)
		Outp32( 0x10440020, 0x00001919 );                    //-  DREX1 : ACLK/CCLK=400MHz, CLK2X=825MHz(on SILICON), t_rfcpb1[13:8]=60ns/2.5ns=24(0x18)(for timing set #1), t_rfcpb0[5:0]=60ns/2.5ns=24(0x18)(for timing set #0)
	}

	//* Resync : Update DLL Info and Reset Read FIFO pointer
	//----------------------------------------------------------------------------------------
	//- Update DLL Information for CH0
	Outp32( 0x10420000+0x0050, 0x1000000 ); 				   	//- PHY0 ctrl_resync=1
	Outp32( 0x10420000+0x0050, 0x0 );					 		//- PHY0 ctrl_resync=0
	//- Update DLL Information for CH1
	Outp32( 0x10460000+0x0050, 0x1000000 ); 				   	//- PHY1 ctrl_resync=1
	Outp32( 0x10460000+0x0050, 0x0 );						 	//- PHY1 ctrl_resync=0
	//----------------------------------------------------------------------------------------
#endif

	Outp32( 0x10400000+0x0014, 0x0 );					 		//- DMC.PrechConfig0.tp_en=0x0, port_policy=0x0
	Outp32( 0x10400000+0x001C, 0xFFFFFFFF );					//- DMC.PrechConfig1 : tp_cnt3=0xFF, tp_cnt2=0xFF, tp_cnt1=0xFF, tp_cnt0=0xFF
	Outp32( 0x10440000+0x0014, 0x0 );					 		//- DMC.PrechConfig0.tp_en=0x0, port_policy=0x0
	Outp32( 0x10440000+0x001C, 0xFFFFFFFF );					//- DMC.PrechConfig1 : tp_cnt3=0xFF, tp_cnt2=0xFF, tp_cnt1=0xFF, tp_cnt0=0xFF

	Outp32( 0x10400000+0x0028, 0x1FFF000D );					//- CH0 : DMC.PWRDNCONFIG.dsref_cnt=0x1FFF0000, DMC.PWRDNCONFIG.dpwrdn_cyc=0x0D
	Outp32( 0x10440000+0x0028, 0x1FFF000D );					//- CH1 : DMC.PWRDNCONFIG.dsref_cnt=0x1FFF0000, DMC.PWRDNCONFIG.dpwrdn_cyc=0x0D

#if 0		//- Disabling PBR
	Outp32( 0x10400000+0x0004, 0x3127A2 );						//- Channel0. DMC.MEMCONTROL.clk_stop_en=0x1, dsref_en=1, dpwrdn_en=1, add_lat_pall=0x2
	Outp32( 0x10440000+0x0004, 0x3127A2 );						//- Channel1. DMC.MEMCONTROL.clk_stop_en=0x1, dsref_en=1, dpwrdn_en=1, add_lat_pall=0x2
#else		//- Enabling PBR
	#if 1		//- Enabling Dynamic Self Refresh, Dynamic Power Down
		Outp32( 0x10400000+0x0004, 0x8312722 ); 				//- Channel0. DMC.MEMCONTROL.clk_stop_en=0x1, dsref_en=1, dpwrdn_en=1, add_lat_pall=0x2
		Outp32( 0x10440000+0x0004, 0x8312722 ); 				//- Channel1. DMC.MEMCONTROL.clk_stop_en=0x1, dsref_en=1, dpwrdn_en=1, add_lat_pall=0x2
	#else		//- Disabling Dynamic Self Refresh, Dynamic Power Down
		Outp32( 0x10400000+0x0004, 0x8312780 ); 				//- Channel0. DMC.MEMCONTROL.clk_stop_en=0x1, dsref_en=0, dpwrdn_en=0, add_lat_pall=0x2
		Outp32( 0x10440000+0x0004, 0x8312780 ); 				//- Channel1. DMC.MEMCONTROL.clk_stop_en=0x1, dsref_en=0, dpwrdn_en=0, add_lat_pall=0x2
	#endif
#endif

#if 0		//- Disabling Pause Resync
	Outp32( 0x10400000+0x0018, 0x5002 );						//- CH0. DMC.PHYCONTROL0.sl_dll_dyn_con=0x1
	Outp32( 0x10440000+0x0018, 0x5002 );						//- CH1. DMC.PHYCONTROL0.sl_dll_dyn_con=0x1
#else		//- Enabling Pause Resync
	//Outp32( 0x10400000+0x0018, 0x5082 );						//- CH0. DMC.PHYCONTROL0.sl_dll_dyn_con=0x1, pause_resync=1
	//Outp32( 0x10440000+0x0018, 0x5082 );						//- CH1. DMC.PHYCONTROL0.sl_dll_dyn_con=0x1, pause_resync=1
	Outp32( 0x10400000+0x0018, 0x5080 );						//- CH0. DMC.PHYCONTROL0.sl_dll_dyn_con=0x1, pause_resync=1
	Outp32( 0x10440000+0x0018, 0x5080 );						//- CH1. DMC.PHYCONTROL0.sl_dll_dyn_con=0x1, pause_resync=1
#endif

	Outp32( 0x10400000+0x0000, 0xFFF11A0 ); 				   	//- CH0. DMC.PHYCONTROL.aref enabled, DMC.PHYCONTROL.io_pd_con=0x2
	Outp32( 0x10440000+0x0000, 0xFFF11A0 ); 				   	//- CH1. DMC.PHYCONTROL.aref enabled, DMC.PHYCONTROL.io_pd_con=0x2

	Outp32( 0x10400000+0x0400, 0x1 );					 		//- CH0. all init complete!
	Outp32( 0x10440000+0x0400, 0x1 );					 		//- CH1. all init complete!

#if 1		//- Enabling Dynamic Clock Gating(for DMC)
	Outp32( 0x10400000+0x0008, 0x3F );					  		//- CH0 Clock Gating ---> Enable
	Outp32( 0x10440000+0x0008, 0x3F );					  		//- CH1 Clock Gating ---> Enable
#else		//- Disabling Dynamic Clock Gating(for DMC)
	Outp32( 0x10400000+0x0008, 0x0 );							//- CH0 Clock Gating ---> Disable
	Outp32( 0x10440000+0x0008, 0x0 );							//- CH1 Clock Gating ---> Disable
#endif

	//- Gating PHY0,1 clock source for SFR and logic not changing after DRAM initialization
	//- The function above is possible to reduce dynamic power consumption of PHY0,1
	//- Actually, Clock Source fof idle logic & SFR during dynamic operation is gated.
	//- When DRAM operation is idle, CG gating function of DREX operate to reduce MIF Power during idle.
	#if 1
		Outp32( 0x10420000+0x0018, (Inp32(0x10420000+0x0018)&~(0x1<<12))|(wrapper_pd<<12)|(0xF<<0) );						//- PHY0. Wrapper Power Mode Setting : LP_CON0[12] = wrapper_pd = 0x1
		Outp32( 0x10460000+0x0018, (Inp32(0x10460000+0x0018)&~(0x1<<12))|(wrapper_pd<<12)|(0xF<<0) );						//- PHY1. Wrapper Power Mode Setting : LP_CON0[12] = wrapper_pd = 0x1
	#else
	if(rd_odt_en){
		Outp32( 0x10420000+0x0018, (Inp32(0x10420000+0x0018)&~(0x1<<12))|(wrapper_pd<<12) );                    	//- PHY0. Wrapper Power Mode Setting : LP_CON0[12] = wrapper_pd = 0x1
		Outp32( 0x10460000+0x0018, (Inp32(0x10460000+0x0018)&~(0x1<<12))|(wrapper_pd<<12) );                    	//- PHY1. Wrapper Power Mode Setting : LP_CON0[12] = wrapper_pd = 0x1
	}
	else{
		Outp32( 0x10420000+0x0018, (Inp32(0x10420000+0x0018)&~(0x1<<12))|(wrapper_pd<<12)|(0xF<<0) );                    	//- PHY0. Wrapper Power Mode Setting : LP_CON0[12] = wrapper_pd = 0x1
		Outp32( 0x10460000+0x0018, (Inp32(0x10460000+0x0018)&~(0x1<<12))|(wrapper_pd<<12)|(0xF<<0) );                    	//- PHY1. Wrapper Power Mode Setting : LP_CON0[12] = wrapper_pd = 0x1
	}
	#endif

	return re;
}

void mif_dvfs_init(void)
{
	unsigned int tmp;

	/* enable drex pause */
	tmp = Inp32(0x105B0000 + 0x1008);
	tmp |= 0x1;
	Outp32(0x105B0000 + 0x1008, tmp);

	/* Configuring Automatic Direct Command Operation */
	tmp = 0x00130010;
	Outp32(0x105E0000 + 0x1020, tmp);

	/* Decision for Timing Parameter Set Switch Control */
	tmp = 0x0;
	Outp32(0x10400000 + 0x00E0, tmp);
	Outp32(0x10440000 + 0x00E0, tmp);

	/* Setting DVFS Mode Control of PHY */
	tmp = 0x0C0C2121;
	Outp32(0x10420000 + 0x00BC, tmp);
	tmp = 0x0C0C2121;
	Outp32(0x10460000 + 0x00BC, tmp);
}

void mem_ctrl_init(void)
{
	unsigned int wakeup_stat;

	wakeup_stat = readl(EXYNOS5430_POWER_WAKEUP_STAT);
	wakeup_stat &= WAKEUP_MASK;

	if (wakeup_stat) {
		DMCInit_For_LPDDR3(0, FALSE, GetPOPType());
	} else {
		DMCInit_For_LPDDR3(1, FALSE, GetPOPType());
	}

	/* prepare mif dvfs */
	mif_dvfs_init();
}
