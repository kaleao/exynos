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
typedef signed   int        s32;
typedef unsigned char       bool;

#define true                                    (1)
#define false                                   (0)


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

#define DMC_INIT_SELF_REFRESH	(0x00)

#define AP_TURN	0
#define CP_TURN	1

#define SEMAPHORE_BASE		0x104B0000
#define AP_FLAG				SEMAPHORE_BASE + 0x00000180
#define CP_FLAG				SEMAPHORE_BASE + 0x00000184
#define INIT_TURN			SEMAPHORE_BASE + 0x00000188
#define MIF_INIT_DONE		SEMAPHORE_BASE + 0x0000004C
#define MIF_MUX_DONE		SEMAPHORE_BASE + 0x00000190

#define	rAPL_PLL_LOCK			(0x10800000)
#define	rAPL_PLL_CON0			(0x10800100)
#define	rCLK_MUX_SEL_APL_0		(0x10800200)
#define	rCLK_MUX_SEL_APL_1		(0x10800204)
#define	rCLK_MUX_SEL_APL_2		(0x10800208)
#define	rCLK_DIV_APL_0			(0x10800600)
#define	rCLK_DIV_APL_1			(0x10800604)
#define	rCLK_MUX_SEL_AUD0		(0x110C0200)
#define	rCLK_MUX_SEL_AUD1		(0x110C0204)
#define	rCLK_DIV_AUD0			(0x110C0600)
#define	rCLK_DIV_AUD1			(0x110C0604)
#define	rCLK_DIV_BUS0			(0x11800600)
#define	rCLK_DIV_BUS1			(0x12400600)
#define	rCLK_DIV_BUS2			(0x13000600)
#define	rCPU_PLL_LOCK			(0x10900000)
#define	rCPU_PLL_CON0			(0x10900100)
#define	rCLK_MUX_SEL_CPU_0		(0x10900200)
#define	rCLK_MUX_SEL_CPU_1		(0x10900204)
#define	rCLK_MUX_SEL_CPU_2		(0x10900208)
#define	rCLK_DIV_CPU_0			(0x10900600)
#define	rCLK_DIV_CPU_1			(0x10900604)
#define	rDISP_PLL_LOCK			(0x14870000)
#define	rDISP_PLL_CON0			(0x14870100)
#define	rCLK_MUX_SEL_DISP0		(0x14870200)
#define	rCLK_MUX_SEL_DISP1		(0x14870204)
#define	rCLK_MUX_SEL_DISP2		(0x14870208)
#define	rCLK_MUX_SEL_DISP4		(0x14870210)
#define	rCLK_DIV_DISP			(0x14870600)
#define	rUSB_PLL_LOCK			(0x13730000)
#define	rUSB_PLL_CON0			(0x13730100)
#define	rCLK_MUX_SEL_FSYS0		(0x13730200)
#define	rCLK_MUX_SEL_FSYS1		(0x13730204)
#define	rCLK_MUX_SEL_FSYS2		(0x13730208)
#define	rG3D_PLL_LOCK			(0x11460000)
#define	rG3D_PLL_CON0			(0x11460100)
#define	rCLK_MUX_SEL_G3D0		(0x11460200)
#define	rCLK_MUX_SEL_G3D1		(0x11460204)
#define	rCLK_MUX_SEL_G3D2		(0x11460208)
#define	rCLK_DIV_G3D			(0x11460600)
#define	rISP_PLL_LOCK			(0x14720000)
#define	rISP_PLL_CON0			(0x14720100)
#define	rCLK_MUX_SEL_ISP0		(0x14720200)
#define	rCLK_MUX_SEL_ISP1		(0x14720204)
#define	rCLK_MUX_SEL_ISP2		(0x14720208)
#define	rCLK_MUX_SEL_ISP3		(0x1472020C)
#define	rCLK_MUX_SEL_ISP4		(0x14720210)
#define	rCLK_MUX_SEL_ISP5		(0x14720214)
#define	rCLK_DIV_ISP0			(0x14720600)
#define	rCLK_DIV_ISP1			(0x14720604)
#define	rCLK_DIV_ISP2			(0x14720608)
#define	rCLK_DIV_ISP3			(0x1472060C)
#define	rCLK_DIV_ISP4			(0x14720610)
#define	rCLK_DIV_ISP5			(0x14720614)
#define	rCLK_MUX_SEL_MFCMSCL	(0x12CB0200)
#define	rCLK_DIV_MFCMSCL		(0x12CB0600)
#define	rMEM0_PLL_LOCK			(0x10430000)
#define	rMEDIA_PLL_LOCK			(0x10430008)
#define	rBUS_PLL_LOCK			(0x1043000C)
#define	rMEM0_PLL_CON0			(0x10430100)
#define	rMEDIA_PLL_CON0			(0x10430120)
#define	rBUS_PLL_CON0			(0x10430130)
#define	rCLK_MUX_SEL_MIF0		(0x10430200)
#define	rCLK_MUX_SEL_MIF1		(0x10430204)
#define	rCLK_MUX_SEL_MIF2		(0x10430208)
#define	rCLK_MUX_SEL_MIF3		(0x1043020C)
#define	rCLK_MUX_SEL_MIF4		(0x10430210)
#define	rCLK_MUX_SEL_MIF5		(0x10430214)
#define	rCLK_MUX_SEL_TOP_DISP	(0x10430218)
#define	rCLK_MUX_STAT_CPMUX		(0x10430420)
#define	rCLK_DIV_MIF0			(0x10430600)
#define	rCLK_DIV_MIF1			(0x10430604)
#define	rCLK_DIV_TOP_DISP		(0x10430610)
#define rPAUSE					(0x10431008)
#define	rCLK_MUX_SEL_PERI		(0x139F0200)
#define	rAUD_PLL_LOCK			(0x10010000)
#define	rAUD_PLL_CON0			(0x10010100)
#define	rAUD_PLL_CON1			(0x10010104)
#define	rCLK_MUX_SEL_TOP0		(0x10010200)
#define	rCLK_MUX_SEL_TOP1		(0x10010204)
#define	rCLK_MUX_SEL_TOP2		(0x10010208)
#define	rCLK_MUX_SEL_TOP_FSYS0	(0x10010220)
#define	rCLK_MUX_SEL_TOP_FSYS1	(0x10010224)
#define	rCLK_MUX_SEL_TOP_FSYS2	(0x10010228)
#define	rCLK_MUX_SEL_TOP_PERI	(0x10010230)
#define	rCLK_MUX_SEL_TOP_ISP	(0x10010234)
#define	rCLK_DIV_TOP0			(0x10010600)
#define	rCLK_DIV_TOP1			(0x10010604)
#define	rCLK_DIV_TOP_FSYS0		(0x10010620)
#define	rCLK_DIV_TOP_FSYS1		(0x10010624)
#define	rCLK_DIV_TOP_FSYS2		(0x10010628)
#define	rCLK_DIV_TOP_PERI0		(0x10010630)
#define	rCLK_DIV_TOP_PERI1		(0x10010634)
#define	rCLK_DIV_TOP_PERI2		(0x10010638)
#define	rCLK_DIV_TOP_PERI3		(0x1001063C)
#define	rCLK_DIV_TOP_ISP0		(0x10010640)
#define	rCLK_DIV_TOP_ISP1		(0x10010644)
#define PLL_ENABLEBIT		(0x80000000)
bool g_isBpll_800=0;
#define GetBits(uAddr, uBaseBit, uMaskValue) \
		((Inp32(uAddr)>>(uBaseBit))&(uMaskValue))
#define CHECK_DONE	(0x4)

//DVFS_MIF_L0/L5/51Mhz
//SCLK_PHY	ACLK_MIF_400	ACLK_MIF_200	ACLK_MIF_100	ACLK_MIF_FIX_100	ACLK_DISP_200
//0			1				3				7				7					2
//0			2				4				8				7					3
void InitMemClk(u32 uMemclk)//825,416,51
{
	u32 m,p,s;

	//MUX_MEM0_PLL
	Outp32(rCLK_MUX_SEL_MIF0,0<<0);
	//MUX_MEDIA_PLL
	Outp32(rCLK_MUX_SEL_MIF1,0<<8);
	//MUX_BUS_PLL
	Outp32(rCLK_MUX_SEL_MIF2,0<<12);

	//MEM0_PLL=825Mhz
	Outp32(rCLK_MUX_SEL_MIF3,(0<<24)|(0<<16));
	WaitUntilStatus(rCLK_MUX_SEL_MIF3+0x200, 24, 7, 4);
	WaitUntilStatus(rCLK_MUX_SEL_MIF3+0x200, 16, 7, 4);

	if(uMemclk!=51)
	{
//rCLK_DIV_MIF1
	//DIV_ACLK_MIF_FIX_100[14:12]
	//DIV_ACLK_MIF_100[11:8]
	//DIV_ACLK_MIF_200[5:4]
	//DIV_ACLK_MIF_400[2:0]
//CLK_DIV_TOP_DISP
	//SCLK_DISP_DECON_INT_VCLK_RATIO	[23:20]->1
	//SCLK_DISP_DECON_INT_ECLK_RATIO	[19:16]->2
	//ACLK_DISP_200_RATIO				[3:0]->2
		if(uMemclk==825)
		{
			m=825;p=13;s=1;
			Outp32(rCLK_DIV_MIF1,(7<<12)|(8<<8)|(4<<4)|(2<<0));
		}
		else
		{
			m=256;p=4;s=2; //416 Mhz
			Outp32(rCLK_DIV_MIF1,(7<<12)|(8<<8)|(4<<4)|(2<<0));
			Outp32(rCLK_DIV_TOP_DISP,(1<<20)|(2<<16)|(3<<0));
		}
		Outp32(rMEM0_PLL_LOCK, 200*p);
		Outp32(rMEM0_PLL_CON0,(1<<31)|(m<<12)|(p<<4)|(s<<0));
		//Wait MEM0_PLL Lock
		while((Inp32(rMEM0_PLL_CON0)&(0x1<<29)==0));
	}
	//MEDIA_PLL=667Mhz
	if((Inp32(rMEDIA_PLL_CON0)&PLL_ENABLEBIT)==0)
	{
		m=308;p=6;s=1;
		Outp32(rMEDIA_PLL_LOCK, 200*p);
		Outp32(rMEDIA_PLL_CON0,(1<<31)|(m<<12)|(p<<4)|(s<<0));
		//Wait MEDIA_PLL Lock
		while((Inp32(rMEDIA_PLL_CON0)&(0x1<<29)==0));
	}
	//BUS_PLL=800Mhz
	if(!g_isBpll_800)
	{
		m=400;p=13;s=0;
		Outp32(rBUS_PLL_LOCK, 200*p);
		Outp32(rBUS_PLL_CON0,(1<<31)|(m<<12)|(p<<4)|(s<<0));
		g_isBpll_800=true;
		//Wait BUS_PLL Lock
		while((Inp32(rBUS_PLL_CON0)&(0x1<<29)==0));
	}

	//MUX_MEDIA_PLL
	Outp32(rCLK_MUX_SEL_MIF1,1<<8);
	//MUX_MEM0_PLL
	Outp32(rCLK_MUX_SEL_MIF0,1<<0);
	//MUX_BUS_PLL
	Outp32(rCLK_MUX_SEL_MIF2,1<<12);

	WaitUntilStatus(rCLK_MUX_SEL_MIF1+0x200, 8, 7, 4);
	WaitUntilStatus(rCLK_MUX_SEL_MIF0+0x200, 0, 7, 4);
	WaitUntilStatus(rCLK_MUX_SEL_MIF2+0x200, 12, 7, 4);

	Outp32(rCLK_MUX_SEL_MIF4,(0<<20)|(0<<4));
	WaitUntilStatus(rCLK_MUX_SEL_MIF4+0x200, 20, 7, 4);
	WaitUntilStatus(rCLK_MUX_SEL_MIF4+0x200, 4, 7, 4);

	switch(uMemclk)
	{
	case 667:
		Outp32(rCLK_MUX_SEL_MIF4,(1<<20)|(1<<4));
		WaitUntilStatus(rCLK_MUX_SEL_MIF4+0x200, 20, 7, 4);
		WaitUntilStatus(rCLK_MUX_SEL_MIF4+0x200, 4, 7, 4);
		Outp32(rCLK_DIV_MIF0,(0<<4)|(0<<0));
		break;
	case 825:
	case 416:
		Outp32(rCLK_DIV_MIF0,(0<<4)|(0<<0));
		break;
	case 51:
		if(GetBits(rMEM0_PLL_CON0, 12, 0x3FF)==825) //825Mhz
			Outp32(rCLK_DIV_MIF0,(15<<4)|(15<<0));
		else //416Mhz
			Outp32(rCLK_DIV_MIF0,(7<<4)|(7<<0));
		break;
	default:
		Outp32(rCLK_DIV_MIF0,(0<<4)|(0<<0));
		break;
	}
}

extern u32 DMC_Init(u32 mem_clk, u32 resume);

void ApInitMif(void)
{
	InitMemClk(416);
///////////////////////////////////////////////////////////////////
//	Add to DREX Init code
///////////////////////////////////////////////////////////////////
	DMC_Init(416, false);
	Outp32(rPAUSE,0x1);
}

void PostCpInitApChange(void)
{
	u32 m,p,s;
	//MEDIA_PLL=667Mhz
	m=308;p=6;s=1;
	Outp32(rMEDIA_PLL_LOCK, 200*p);
	Outp32(rMEDIA_PLL_CON0,(1<<31)|(m<<12)|(p<<4)|(s<<0));

	//Wait MEDIA_PLL Lock
	while((Inp32(rMEDIA_PLL_CON0)&(0x1<<29)==0));

	Outp32(rCLK_MUX_SEL_MIF1,1<<8);
	WaitUntilStatus(rCLK_MUX_SEL_MIF1+0x200, 8, 7, 4);

}

void CpInitApChange(void)
{
	u32 m,p,s;

//	m=825;p=13;s=1;//825Mhz
	m=256;p=4;s=2; //416 Mhz
	Outp32(rMEM0_PLL_LOCK, 200*p);
	Outp32(rMEM0_PLL_CON0,(1<<31)|(m<<12)|(p<<4)|(s<<0));
	//BUS_PLL=800Mhz
	m=800;p=13;s=1;
	Outp32(rBUS_PLL_LOCK, 200*p);
	Outp32(rBUS_PLL_CON0,(1<<31)|(m<<12)|(p<<4)|(s<<0));

	//Wait MEM0_PLL Lock
	while((Inp32(rMEM0_PLL_CON0)&(0x1<<29)==0));
	//Wait BUS_PLL Lock
	while((Inp32(rBUS_PLL_CON0)&(0x1<<29)==0));

//rCLK_DIV_MIF1
	//DIV_ACLK_MIF_FIX_100[14:12]->7
	//DIV_ACLK_MIF_100[11:8]->8
	//DIV_ACLK_MIF_200[5:4]->4
	//DIV_ACLK_MIF_400[2:0]->2
	Outp32(rCLK_DIV_MIF1,(7<<12)|(8<<8)|(4<<4)|(2<<0));
	Outp32(rCLK_DIV_TOP_DISP,(1<<20)|(2<<16)|(3<<0));

	//MUX_MEM0_PLL
	Outp32(rCLK_MUX_SEL_MIF0,1<<0);
	//MUX_BUS_PLL
	Outp32(rCLK_MUX_SEL_MIF2,1<<12);
	WaitUntilStatus(rCLK_MUX_SEL_MIF0+0x200, 0, 7, 4);
	WaitUntilStatus(rCLK_MUX_SEL_MIF2+0x200, 12, 7, 4);

	Outp32(rCLK_MUX_SEL_MIF4,(0<<20)|(0<<4));
	WaitUntilStatus(rCLK_MUX_SEL_MIF4+0x200, 20, 7, 4);
	WaitUntilStatus(rCLK_MUX_SEL_MIF4+0x200, 4, 7, 4);

	//MUX_ACLK_MIF_400=0
	//MUX_ACLK_MIF_100=0
	//MUX_ACLK_MIF_FIX_100=0
	Outp32(rCLK_MUX_SEL_MIF5,(0<<12)|(0<<8)|(0<<4)|(0<<0));

	WaitUntilStatus(rCLK_MUX_SEL_MIF5+0x200, 8, 7, 4);
	WaitUntilStatus(rCLK_MUX_SEL_MIF5+0x200, 4, 7, 4);
	WaitUntilStatus(rCLK_MUX_SEL_MIF5+0x200, 0, 7, 4);
	WaitUntilStatus(rCLK_MUX_STAT_CPMUX, 0, 7, 4);
}

void CpInitCpChange(void)
{
	u32 m,p,s;
	//MEDIA_PLL=667Mhz
	m=308;p=6;s=1;
	Outp32(rMEDIA_PLL_LOCK, 200*p);
	Outp32(rMEDIA_PLL_CON0,(1<<31)|(m<<12)|(p<<4)|(s<<0));

	//BUS_PLL=800Mhz
	m=800;p=13;s=1;
	Outp32(rBUS_PLL_LOCK, 200*p);
	Outp32(rBUS_PLL_CON0,(1<<31)|(m<<12)|(p<<4)|(s<<0));

	//Wait MEDIA_PLL Lock
	while((Inp32(rMEDIA_PLL_CON0)&(0x1<<29)==0));
	//Wait BUS_PLL Lock
	while((Inp32(rBUS_PLL_CON0)&(0x1<<29)==0));

//rCLK_DIV_MIF1
	//DIV_ACLK_MIF_FIX_100[14:12]->7
	//DIV_ACLK_MIF_100[11:8]->8
	//DIV_ACLK_MIF_200[5:4]->4
	//DIV_ACLK_MIF_400[2:0]->2
	Outp32(rCLK_DIV_MIF1,(7<<12)|(8<<8)|(4<<4)|(2<<0));
	Outp32(rCLK_DIV_TOP_DISP,(1<<20)|(2<<16)|(2<<0));

	//MUX_MEDIA_PLL
	Outp32(rCLK_MUX_SEL_MIF1,1<<8);
	//MUX_BUS_PLL
	Outp32(rCLK_MUX_SEL_MIF2,1<<12);
	WaitUntilStatus(rCLK_MUX_SEL_MIF1+0x200, 8, 7, 4);
	WaitUntilStatus(rCLK_MUX_SEL_MIF2+0x200, 12, 7, 4);

	//MUX_ACLK_MIF_400=0
	//MUX_ACLK_MIF_100=0
	//MUX_ACLK_MIF_FIX_100=0
	Outp32(rCLK_MUX_SEL_MIF5,(0<<8)|(0<<4)|(0<<0));

	WaitUntilStatus(rCLK_MUX_SEL_MIF5+0x200, 8, 7, 4);
	WaitUntilStatus(rCLK_MUX_SEL_MIF5+0x200, 4, 7, 4);
	WaitUntilStatus(rCLK_MUX_SEL_MIF5+0x200, 0, 7, 4);
}

//DVFS_CPU_L11
//CPU_1/2	ATCLK_CPU	PCLK_DBG_CPU	ACLK_CPU	PCLK_CPU	CNTCLK_CPU	SCLK_HPM_CPU	CPU_PLL
//0			7			7				1			7			3			7				4
void InitCpuClk(void)
{
	u32 m,p,s;
	m=248;p=4;s=1; //PLL_CPU -> 806Mhz
	//MUX_CPU_PLL(CLK_MUX_SEL_CPU_0[0] 0: OSC 1: FOUT_PLL_CPU)
	SetBits(rCLK_MUX_SEL_CPU_0, 0, 0x1, 0);
	WaitUntilStatus(rCLK_MUX_SEL_CPU_0 + 0x200, 0, 7, 4);

	Outp32(rCPU_PLL_LOCK, 200*p);
	Outp32(rCPU_PLL_CON0,(1<<31)|(m<<12)|(p<<4)|(s<<0));

//CLK_DIV_CPU_0
	//CNTCLK_CPU_RATIO		[26:24]->3
	//PCLK_CPU_RATIO		[22:20]->7
	//ACLK_CPU_RATIO		[18:16]->1
	//PCLK_DBG_CPU_RATIO	[14:12]->7
	//ATCLK_CPU_RATIO		[10:8]->7
	//CPU_2_RATIO			[6:4]->0
	//CPU_1_RATIO			[2:0]->0
	Outp32(rCLK_DIV_CPU_0,(3<<24)|(7<<20)|(1<<16)|(7<<12)|(7<<8)|(0<<4)|(0<<0));
//CLK_DIV_CPU_1
	//SCLK_HPM_CPU_RATIO	[6:4]->2
	//CPU_PLL_RATIO			[2:0]->4
	Outp32(rCLK_DIV_CPU_1,(2<<4)|(4<<0));

	//Wait CPU_PLL Lock
	while((Inp32(rCPU_PLL_CON0)&(0x1<<29)==0));
	//MUX_CPU_PLL(CLK_MUX_SEL_CPU_0[0] 0: OSC 1: FOUT_PLL_CPU)
	SetBits(rCLK_MUX_SEL_CPU_0, 0, 0x1, 1);
	WaitUntilStatus(rCLK_MUX_SEL_CPU_0 + 0x200, 0, 7, 4);

	//MUX_PLL_CPU_USER(CLK_MUX_SEL_CPU_1[0] 0: OSC 1: SCLK_BUS_PLL_CPU)
	SetBits(rCLK_MUX_SEL_CPU_1, 0, 0x1, 1);
	WaitUntilStatus(rCLK_MUX_SEL_CPU_1 + 0x200, 0, 7, 4);
	//MUX_CPU(CLK_MUX_SEL_CPU_2[0] 0: MOUT_CPU_PLL 1: MOUT_BUS_PLL_CPU_USER)
	SetBits(rCLK_MUX_SEL_CPU_2, 0, 0x1, 0);
	WaitUntilStatus(rCLK_MUX_SEL_CPU_2 + 0x200, 0, 7, 4);

}

//DVFS_APL_L14
//APL_1/2	ATCLK_APL	PCLK_DBG_APL	ACLK_APL	PCLK_APL	CNTCLK_APL	SCLK_HPM_APL	APL_PLL
//0			7			7				1			7			3			7				4
void InitAplClk(void)
{
	u32 m,p,s;
	m=248;p=4;s=1; //PLL_APL -> 806Mhz
	//MUX_APL_PLL(CLK_MUX_SEL_APL_0[0] 0: OSC 1: FOUT_PLL_CPU)
	SetBits(rCLK_MUX_SEL_APL_0, 0, 0x1, 0);
	WaitUntilStatus(rCLK_MUX_SEL_APL_0 + 0x200, 0, 7, 4);

	Outp32(rAPL_PLL_LOCK, 200*p);
	Outp32(rAPL_PLL_CON0,(1<<31)|(m<<12)|(p<<4)|(s<<0));

//CLK_DIV_APL_0
	//CNTCLK_APL_RATIO		[26:24]->3
	//PCLK_APL_RATIO		[22:20]->7
	//ACLK_APL_RATIO		[18:16]->1
	//PCLK_DBG_APL_RATIO	[14:12]->7
	//ATCLK_APL_RATIO		[10:8]->7
	//APL_2_RATIO			[6:4]->0
	//APL_1_RATIO			[2:0]->0
	Outp32(rCLK_DIV_APL_0,(3<<24)|(7<<20)|(1<<16)|(7<<12)|(7<<8)|(0<<4)|(0<<0));
//CLK_DIV_APL_1
	//SCLK_HPM_APL_RATIO	[6:4]->2
	//APL_PLL_RATIO			[2:0]->4
	Outp32(rCLK_DIV_APL_1,(2<<4)|(4<<0));

	//Wait APL_PLL Lock
	while((Inp32(rAPL_PLL_CON0)&(0x1<<29)==0));
	//MUX_APL_PLL(CLK_MUX_SEL_CPU_0[0] 0: OSC 1: FOUT_PLL_CPU)
	SetBits(rCLK_MUX_SEL_APL_0, 0, 0x1, 1);
	WaitUntilStatus(rCLK_MUX_SEL_APL_0 + 0x200, 0, 7, 4);
	//MUX_APL(CLK_MUX_SEL_APL_2[0] 0: MOUT_APL_PLL 1: MOUT_BUS_PLL_APL_USER)
	SetBits(rCLK_MUX_SEL_APL_2, 0, 0x1, 0);
	WaitUntilStatus(rCLK_MUX_SEL_APL_2 + 0x200, 0, 7, 4);
	//MUX_PLL_APL_USER(CLK_MUX_SEL_APL_1[0] 0: OSC 1: SCLK_BUS_PLL_APL)
	SetBits(rCLK_MUX_SEL_APL_1, 0, 0x1, 1);
	WaitUntilStatus(rCLK_MUX_SEL_APL_1 + 0x200, 0, 7, 4);

}
void InitIntClk(bool bIsReset)
{
	s32 m,p,s,k;

	//FSYS Block is always on block in Insel-D
	//PLL_USB(CLK_MUX_SEL_FSYS0[0] 0: OSC 1: FOUT_PLL_USB)
	SetBits(rCLK_MUX_SEL_FSYS0, 0, 0x1, 0);
	WaitUntilStatus(rCLK_MUX_SEL_FSYS0 + 0x200, 0, 7, 4);

	//PLL_USB
	m=400;p=13;s=4; //PLL_USB -> 50Mhz
	Outp32(rUSB_PLL_LOCK, 200*p);
	Outp32(rUSB_PLL_CON0,(1<<31)|(1<<22)|(m<<12)|(p<<4)|(s<<0));

	//Wait USB_PLL Lock
	while((Inp32(rUSB_PLL_CON0)&(0x1<<29)==0));
	SetBits(rCLK_MUX_SEL_FSYS0, 0, 0x1, 1);
	WaitUntilStatus(rCLK_MUX_SEL_FSYS0 + 0x200, 0, 7, 4);

	if(bIsReset)
	{
		//PLL_DISP(CLK_MUX_SEL_DISP0[0] 0: OSC 1: FOUT_PLL_DISP)
		SetBits(rCLK_MUX_SEL_DISP0, 0, 0x1, 0);
		WaitUntilStatus(rCLK_MUX_SEL_DISP0 + 0x200, 0, 7, 4);
		m=284;p=13;s=2; //PLL_DISP -> 142Mhz
		Outp32(rDISP_PLL_LOCK, 200*p);
		Outp32(rDISP_PLL_CON0,(1<<31)|(m<<12)|(p<<4)|(s<<0));

		//PLL_G3D(CLK_MUX_SEL_G3D0[0] 0: OSC 1: FOUT_PLL_G3D)
		SetBits(rCLK_MUX_SEL_G3D0, 0, 0x1, 0);
		WaitUntilStatus(rCLK_MUX_SEL_G3D0 + 0x200, 0, 7, 4);

		m=287;p=14;s=0; //PLL_G3D -> 533Mhz
		Outp32(rG3D_PLL_LOCK, 200*p);
		Outp32(rG3D_PLL_CON0,(1<<31)|(m<<12)|(p<<4)|(s<<0));

		//PLL_ISP(CLK_MUX_SEL_ISP0[0] 0: OSC 1: FOUT_PLL_ISP)
		SetBits(rCLK_MUX_SEL_ISP0, 0, 0x1, 0);
		WaitUntilStatus(rCLK_MUX_SEL_ISP0 + 0x200, 0, 7, 4);

		m=430;p=13;s=0; //PLL_ISP -> 860Mhz
		Outp32(rISP_PLL_LOCK, 200*p);
		Outp32(rISP_PLL_CON0,(1<<31)|(m<<12)|(p<<4)|(s<<0));

		//PLL_AUD(CLK_MUX_SEL_TOP0[0] 0: OSC 1: FOUT_PLL_AUD)
		SetBits(rCLK_MUX_SEL_TOP0, 0, 0x1, 0);
		WaitUntilStatus(rCLK_MUX_SEL_TOP0 + 0x200, 0, 7, 4);
		//PLL_AUD(CLK_MUX_SEL_AUD0[0] 0: OSC 1: FOUT_PLL_AUD)
		SetBits(rCLK_MUX_SEL_AUD0, 0, 0x1, 0);
		WaitUntilStatus(rCLK_MUX_SEL_AUD0 + 0x200, 0, 7, 4);

		m=181;p=3;s=2;k=31740; //PLL_AUD -> 393.216Mhz
		Outp32(rAUD_PLL_LOCK, 3000*p);
		Outp32(rAUD_PLL_CON0,(1<<31)|(m<<12)|(p<<4)|(s<<0));
		SetBits(rAUD_PLL_CON1,0,0xFFFF,k);
	}
////////////////////////////////////////////////////////////////////////////////////////////////
//	Set CMU_TOP
////////////////////////////////////////////////////////////////////////////////////////////////
//CLK_DIV_TOP0
	//ACLK_PERI_66_RATIO		[31:28]->11
	//ACLK_BUS2_400_RATIO		[27:24]->7
	//ACLK_BUS1_400_RATIO		[19:16]->1
	//ACLK_BUS0_400_RATIO		[15:12]->1
	//ACLK_IMEM_200_RATIO		[11:8]->3
	//ACLK_IMEM_266_RATIO		[7:4]->2
	//ACLK_FSYS_200_RATIO		[3:0]->7
	Outp32(rCLK_DIV_TOP0,(11<<28)|(7<<24)|(1<<16)|(1<<12)|(3<<8)|(2<<4)|(7<<0));
//CLK_DIV_TOP1
	//ACLK_G3D_400_RATIO		[31:28]->1
	//ACLK_MFCMSCL_400_RATIO	[27:24]->1
	//ACLK_MFCMSCL_266_RATIO	[23:20]->2
	//ACLK_ISP_266_RATIO		[19:16]->2
	//ACLK_ISP_333_RATIO		[15:12]->1
	//ACLK_ISP_400_RATIO		[11:8]->1
	Outp32(rCLK_DIV_TOP1,(1<<28)|(1<<24)|(2<<20)|(2<<16)|(1<<12)|(1<<8));

	if(bIsReset)
	{

	//SCLK Divider setting For Boot
	//CLK_DIV_TOP_FSYS0
		//SCLK_FSYS_MMC0_SDCLKIN_B_RATIO	[11:4]->3
		//SCLK_FSYS_MMC0_SDCLKIN_A_RATIO	[3:0]->4
		Outp32(rCLK_DIV_TOP_FSYS0,(3<<4)|(4<<0));
	//CLK_DIV_TOP_FSYS1
		//SCLK_FSYS_MMC1_SDCLKIN_B_RATIO	[11:4]->3
		//SCLK_FSYS_MMC1_SDCLKIN_A_RATIO	[3:0]->4
		Outp32(rCLK_DIV_TOP_FSYS1,(3<<4)|(4<<0));
	//CLK_DIV_TOP_FSYS2
		//SCLK_FSYS_MMC2_SDCLKIN_B_RATIO	[11:4]->3
		//SCLK_FSYS_MMC2_SDCLKIN_A_RATIO	[3:0]->4
		Outp32(rCLK_DIV_TOP_FSYS2,(3<<4)|(4<<0));
	//CLK_DIV_TOP_PERI0
		//SCLK_PERI_I2S_I2SCODCLKI_RATIO	[9:4]->3
		//SCLK_PERI_AUD_RATIO	[3:0]->0
		Outp32(rCLK_DIV_TOP_PERI0,(3<<4)|(0<<0));
	//CLK_DIV_TOP_PERI1
		//SCLK_PERI_SPI1_SPI_EXT_CLK_B_RATIO	[27:20]->0
		//SCLK_PERI_SPI1_SPI_EXT_CLK_A_RATIO	[19:16]->7 //SPI 100Mhz
		//SCLK_PERI_SPI0_SPI_EXT_CLK_B_RATIO	[11:4]->0
		//SCLK_PERI_SPI0_SPI_EXT_CLK_A_RATIO	[3:0]->7
		Outp32(rCLK_DIV_TOP_PERI1,(0<<20)|(7<<16)|(0<<4)|(7<<0));
	//CLK_DIV_TOP_PERI2
		//SCLK_PERI_SPI2_SPI_EXT_CLK_B_RATIO	[11:4]->0
		//SCLK_PERI_SPI2_SPI_EXT_CLK_A_RATIO	[3:0]->7
		Outp32(rCLK_DIV_TOP_PERI2,(0<<4)|(7<<0));
	//CLK_DIV_TOP_PERI3
		//SCLK_PERI_UART2_EXT_UCLK_RATIO	[27:24]->7
		//SCLK_PERI_UART1_EXT_UCLK_RATIO	[23:20]->7
		//SCLK_PERI_UART0_EXT_UCLK_RATIO	[19:16]->7
		Outp32(rCLK_DIV_TOP_PERI3,(7<<24)|(7<<20)|(7<<16));
	//CLK_DIV_TOP_ISP0
		//SCLK_ISP_UART_ISP_EXT_UCLK_RATIO	[27:24]->7
		//SCLK_ISP_SPI1_ISP_SPI_EXT_CLK_B_RATIO	[23:16]->0
		//SCLK_ISP_SPI1_ISP_SPI_EXT_CLK_A_RATIO	[15:12]->7
		//SCLK_ISP_SPI0_ISP_SPI_EXT_CLK_B_RATIO	[11:4]->0
		//SCLK_ISP_SPI0_ISP_SPI_EXT_CLK_A_RATIO	[3:0]->7
		Outp32(rCLK_DIV_TOP_ISP0,(7<<24)|(0<<16)|(7<<12)|(0<<4)|(7<<0));
	//CLK_DIV_TOP_ISP1
		//SCLK_ISP_SENSOR1_B_RATIO	[15:12]->0
		//SCLK_ISP_SENSOR1_A_RATIO	[11:8]->15
		//SCLK_ISP_SENSOR0_B_RATIO	[7:4]->0
		//SCLK_ISP_SENSOR0_A_RATIO	[3:0]->15
		Outp32(rCLK_DIV_TOP_ISP1,(0<<12)|(15<<8)|(0<<4)|(15<<0));

	//CLK_MUX_SEL_TOP2
		//ACLK_BUS2_400_SEL	[20]->1
		SetBits(rCLK_MUX_SEL_TOP2, 20, 1, 1);
		WaitUntilStatus(rCLK_MUX_SEL_TOP2 + 0x200, 20, 7, 4);
		//ACLK_BUS1_400_SEL	[16]->0
		SetBits(rCLK_MUX_SEL_TOP2, 16, 1, 0);
		WaitUntilStatus(rCLK_MUX_SEL_TOP2 + 0x200, 16, 7, 4);
		//ACLK_BUS0_400_SEL	[12]->0
		SetBits(rCLK_MUX_SEL_TOP2, 12, 1, 0);
		WaitUntilStatus(rCLK_MUX_SEL_TOP2 + 0x200, 12, 7, 4);
		//ACLK_MFCMSCL_400_SEL	[8]->0
		SetBits(rCLK_MUX_SEL_TOP2, 8, 1, 0);
		WaitUntilStatus(rCLK_MUX_SEL_TOP2 + 0x200, 8, 7, 4);
		//ACLK_MFCMSCL_266_SEL	[4]->0
		SetBits(rCLK_MUX_SEL_TOP2, 4, 1, 0);
		WaitUntilStatus(rCLK_MUX_SEL_TOP2 + 0x200, 4, 7, 4);
		//ACLK_ISP_266_SEL	[0]->0
		SetBits(rCLK_MUX_SEL_TOP2, 0, 1, 0);
		WaitUntilStatus(rCLK_MUX_SEL_TOP2 + 0x200, 0, 7, 4);

#if 0
	//CLK_MUX_SEL_TOP1
		//MUX_BUS_PLL_TOP_USER -> SCLK_BUS_PLL_TOP
		SetBits(rCLK_MUX_SEL_TOP1, 4, 1, 1);
		WaitUntilStatus(rCLK_MUX_SEL_TOP1 + 0x200, 4, 7, 4);
		//MUX_MEDIA_PLL_TOP_USER -> SCLK_MEDIA_PLL_TOP
		SetBits(rCLK_MUX_SEL_TOP1, 8, 1, 1);
		WaitUntilStatus(rCLK_MUX_SEL_TOP1 + 0x200, 8, 7, 4);
		//MUX_AUD_PLL_USER -> MOUT_AUD_PLL
		SetBits(rCLK_MUX_SEL_TOP1, 8, 1, 1);
		WaitUntilStatus(rCLK_MUX_SEL_TOP1 + 0x200, 8, 7, 4);

	//CLK_MUX_SEL_TOP0
		SetBits(rCLK_MUX_SEL_TOP0, 0, 1, 1);
		WaitUntilStatus(rCLK_MUX_SEL_TOP0 + 0x200, 0, 7, 4);
#endif
	////////////////////////////////////////////////////////////////////////////////////////////////
	//	Set CMU_G3D
	////////////////////////////////////////////////////////////////////////////////////////////////
	//CLK_DIV_G3D
		//PCLK_G3D_150_RATIO	[6:4]->3
		//ACLK_G3D_600_RATIO	[2:0]->0
		Outp32(rCLK_DIV_G3D,(3<<4)|(0<<0));

		//MUX_G3D
		SetBits(rCLK_MUX_SEL_G3D2, 0, 0x1, 0);
		WaitUntilStatus(rCLK_MUX_SEL_G3D2 + 0x200, 0, 7, 4);

		//MUX_ACLK_G3D_400_USER
		SetBits(rCLK_MUX_SEL_G3D1, 0, 0x1, 1);
		WaitUntilStatus(rCLK_MUX_SEL_G3D1 + 0x200, 0, 7, 4);

		//Wait PLL_G3D Lock
		while((Inp32(rG3D_PLL_CON0)&(0x1<<29)==0));

		//PLL_G3D(CLK_MUX_SEL_G3D0[0] 0: OSC 1: FOUT_PLL_G3D)
		SetBits(rCLK_MUX_SEL_G3D0, 0, 0x1, 1);
		WaitUntilStatus(rCLK_MUX_SEL_G3D0 + 0x200, 0, 7, 4);
	}
////////////////////////////////////////////////////////////////////////////////////////////////
//	Set CMU_BUS0/1/2
////////////////////////////////////////////////////////////////////////////////////////////////
//CLK_DIV_BUS0
	//PCLK_BUS0_100_RATIO	[2:0]->3
	Outp32(rCLK_DIV_BUS0,3<<0);
//CLK_DIV_BUS1
	//PCLK_BUS1_100_RATIO	[2:0]->3
	Outp32(rCLK_DIV_BUS1,3<<0);
//CLK_DIV_BUS2
	//PCLK_BUS2_100_RATIO	[2:0]->3
	Outp32(rCLK_DIV_BUS2,3<<0);
////////////////////////////////////////////////////////////////////////////////////////////////
//	Set CMU_ISP
////////////////////////////////////////////////////////////////////////////////////////////////
	if(bIsReset)
	{
	//CLK_DIV_ISP0
		//ISP_PLL_DIV3_RATIO			[5:4]->2
		//ISP_PLL_DIV2_RATIO			[1:0]->1
		Outp32(rCLK_DIV_ISP0, (2<<4)|(1<<0));
	//CLK_DIV_ISP1
		//SCLK_CPU_ISP_PCLKDBG_RATIO	[10:8]->3
		//SCLK_CPU_ISP_ATCLKIN_RATIO	[6:4]->1
		//SCLK_CPU_ISP_CLKIN_RATIO		[2:0]->0
		Outp32(rCLK_DIV_ISP1,(3<<8)|(1<<4)|(0<<0));
	//CLK_DIV_ISP2
		//PCLK_CSI_LINK1_37_RATIO		[9:8]->2
		//ACLK_CSI_LINK1_75_RATIO		[6:4]->0
		//PCLK_CSI_LINK0_225_RATIO		[1:0]->1
		Outp32(rCLK_DIV_ISP2,(2<<8)|(0<<4)|(1<<0));
	//CLK_DIV_ISP3
		//PCLK_FIMC_FD_150_RATIO		[13:12]->1
		//ACLK_FIMC_FD_300_RATIO		[10:8]->0
		//PCLK_FIMC_ISP_225_RATIO		[6:4]->1
		//ACLK_FIMC_ISP_450_RATIO		[2:0]->0
		Outp32(rCLK_DIV_ISP3,(1<<12)|(0<<8)|(1<<4)|(0<<0));
	//CLK_DIV_ISP4
		//ACLK_ISP_67_RATIO 			[9:8]->3
		//ACLK_ISP_133_RATIO			[5:4]->1
		//ACLK_ISP_266_RATIO			[2:0]->0
		Outp32(rCLK_DIV_ISP4,(3<<8)|(1<<4)|(0<<0));

	//Wait ISP_PLL Lock
		while((Inp32(rISP_PLL_CON0)&(0x1<<29)==0));
	//MUX_ISP_PLL
		SetBits(rCLK_MUX_SEL_ISP0, 0, 0x1, 1);
		WaitUntilStatus(rCLK_MUX_SEL_ISP0 + 0x200, 0, 7, 4);
	////////////////////////////////////////////////////////////////////////////////////////////////
	//	Set CMU_MFCMSCL
	////////////////////////////////////////////////////////////////////////////////////////////////
		SetBits(rCLK_MUX_SEL_MFCMSCL, 4, 0x1, 1);
		WaitUntilStatus(rCLK_MUX_SEL_MFCMSCL + 0x200, 4, 7, 4);
		SetBits(rCLK_MUX_SEL_MFCMSCL, 0, 0x1, 1);
		WaitUntilStatus(rCLK_MUX_SEL_MFCMSCL + 0x200, 0, 7, 4);
	//CLK_DIV_MFCMSCL
		//PCLK_MFCMSCL_100_RATIO	[1:0]->3
		Outp32(rCLK_DIV_MFCMSCL,3<<0);
	}

////////////////////////////////////////////////////////////////////////////////////////////////
//	Set CMU_MIF_DISP
////////////////////////////////////////////////////////////////////////////////////////////////
//CLK_DIV_TOP_DISP
	//SCLK_DISP_DECON_INT_VCLK_RATIO	[23:20]->1
	//SCLK_DISP_DECON_INT_ECLK_RATIO	[19:16]->4
	//ACLK_DISP_200_RATIO				[3:0]->2 //ACLK
	Outp32(rCLK_DIV_TOP_DISP,(1<<20)|(2<<16)|(2<<0));
//CLK_MUX_SEL_TOP_DISP
	//SCLK_DISP_DECON_INT_VCLK_B_SEL	[20]->0
	//SCLK_DISP_DECON_INT_VCLK_A_SEL	[16]->1
	//SCLK_DISP_DECON_INT_ECLK_B_SEL	[12]->0
	//SCLK_DISP_DECON_INT_ECLK_A_SEL	[8]->1
	//ACLK_DISP_200_SEL					[0]->0
	SetBits(rCLK_MUX_SEL_TOP_DISP, 20, 1, 0);
	WaitUntilStatus(rCLK_MUX_SEL_TOP_DISP + 0x200, 20, 7, 4);
	SetBits(rCLK_MUX_SEL_TOP_DISP, 16, 1, 1);
	WaitUntilStatus(rCLK_MUX_SEL_TOP_DISP + 0x200, 16, 7, 4);
	SetBits(rCLK_MUX_SEL_TOP_DISP, 12, 1, 0);
	WaitUntilStatus(rCLK_MUX_SEL_TOP_DISP + 0x200, 12, 7, 4);
	SetBits(rCLK_MUX_SEL_TOP_DISP, 8, 1, 1);
	WaitUntilStatus(rCLK_MUX_SEL_TOP_DISP + 0x200, 8, 7, 4);
	SetBits(rCLK_MUX_SEL_TOP_DISP, 0, 1, 0);
	WaitUntilStatus(rCLK_MUX_SEL_TOP_DISP + 0x200, 0, 7, 4);

////////////////////////////////////////////////////////////////////////////////////////////////
//	Set CMU_DISP
////////////////////////////////////////////////////////////////////////////////////////////////
	if(bIsReset)
	{
	//CLK_DIV_DISP
		//SCLK_DECON_INT_VCLK_RATIO	[10:8]	->0
		//SCLK_DECON_INT_ECLK_RATIO	[6:4]	->0
		//PCLK_DISP_100_RATIO		[1:0]	->1
		Outp32(rCLK_DIV_DISP,(0<<8)|(0<<4)|(1<<0));
	//CLK_MUX_SEL_DISP1
		//SCLK_DECON_INT_VCLK_USER_SEL	[12]->1
		SetBits(rCLK_MUX_SEL_DISP1, 12, 0x1, 1);
		WaitUntilStatus(rCLK_MUX_SEL_DISP1 + 0x200, 12, 7, 4);
		//SCLK_DECON_INT_ECLK_USER_SEL	[8]->1
		SetBits(rCLK_MUX_SEL_DISP1, 8, 0x1, 1);
		WaitUntilStatus(rCLK_MUX_SEL_DISP1 + 0x200, 8, 7, 4);
		//ACLK_DISP_200_USER_SEL	[0]->1
		SetBits(rCLK_MUX_SEL_DISP1, 0, 0x1, 1);
		WaitUntilStatus(rCLK_MUX_SEL_DISP1 + 0x200, 0, 7, 4);
	//CLK_MUX_SEL_DISP4
		//SCLK_DECON_INT_VCLK_SEL	[4]
		SetBits(rCLK_MUX_SEL_DISP4, 4, 0x1, 1);
		WaitUntilStatus(rCLK_MUX_SEL_DISP4 + 0x200, 4, 7, 4);
		//SCLK_DECON_INT_ECLK_SEL	[0]
		SetBits(rCLK_MUX_SEL_DISP4, 0, 0x1, 1);
		WaitUntilStatus(rCLK_MUX_SEL_DISP4 + 0x200, 0, 7, 4);

	//Wait DISP_PLL Lock
		while((Inp32(rDISP_PLL_CON0)&(0x1<<29)==0));

	//CLK_MUX_SEL_DISP0
		//DISP_PLL_SEL	[0]->1
		SetBits(rCLK_MUX_SEL_DISP0, 0, 0x1, 1);
		WaitUntilStatus(rCLK_MUX_SEL_DISP0 + 0x200, 0, 7, 4);
	////////////////////////////////////////////////////////////////////////////////////////////////
	//	Set CMU_AUD
	////////////////////////////////////////////////////////////////////////////////////////////////
	//CLK_DIV_AUD0
		//ACLK_AUD_133_RATIO		[3:0]->2
		Outp32(rCLK_DIV_AUD0,2<<0);
	//CLK_DIV_AUD1
		//SCLK_AUDMIXER_AUD_RATIO	[23:20]->7
		//SCLK_UART_AUD_RATIO		[15:12]->1
		//SCLK_PCM_AUD_RATIO		[11:4]->31
		//SCLK_MI2S_AUD_RATIO		[3:0]->3
		Outp32(rCLK_DIV_AUD1,(7<<20)|(1<<12)|(31<<4)|(3<<0));

		//PLL_AUD(CLK_MUX_SEL_AUD0[0] 0: OSC 1: FOUT_PLL_AUD)
		SetBits(rCLK_MUX_SEL_AUD0, 0, 0x1, 1);
		WaitUntilStatus(rCLK_MUX_SEL_AUD0 + 0x200, 0, 7, 4);
	}
////////////////////////////////////////////////////////////////////////////////////////////////
//	Set TOP User Mux/SCLK Mux
////////////////////////////////////////////////////////////////////////////////////////////////
//CLK_MUX_SEL_TOP1
	//MUX_BUS_PLL_TOP_USER -> SCLK_BUS_PLL_TOP
	SetBits(rCLK_MUX_SEL_TOP1, 4, 1, 1);
	WaitUntilStatus(rCLK_MUX_SEL_TOP1 + 0x200, 4, 7, 4);
	//MUX_MEDIA_PLL_TOP_USER -> SCLK_MEDIA_PLL_TOP
	SetBits(rCLK_MUX_SEL_TOP1, 8, 1, 1);
	WaitUntilStatus(rCLK_MUX_SEL_TOP1 + 0x200, 8, 7, 4);

	if(bIsReset)
	{
		//MUX_AUD_PLL_USER -> MOUT_AUD_PLL
		SetBits(rCLK_MUX_SEL_TOP1, 0, 1, 1);
		WaitUntilStatus(rCLK_MUX_SEL_TOP1 + 0x200, 8, 7, 4);
	//CLK_MUX_SEL_TOP0
		SetBits(rCLK_MUX_SEL_TOP0, 0, 1, 1);
		WaitUntilStatus(rCLK_MUX_SEL_TOP0 + 0x200, 0, 7, 4);

	//rCLK_MUX_SEL_TOP_FSYS0
		//MUX_SCLK_FSYS_MMC0_SDCLKIN_B	[4]	MOUT_SCLK_FSYS_MMC0_SDCLKIN_A	SCLK_FSYS_MMC0_SDCLKIN_B_SEL
		SetBits(rCLK_MUX_SEL_TOP_FSYS0, 4, 1, 0);
		WaitUntilStatus(rCLK_MUX_SEL_TOP_FSYS0 + 0x200, 4, 7, 4);
		//MUX_SCLK_FSYS_MMC0_SDCLKIN_A	[0]	OSCCLK	SCLK_FSYS_MMC0_SDCLKIN_A_SEL
		SetBits(rCLK_MUX_SEL_TOP_FSYS0, 0, 1, 1);
		WaitUntilStatus(rCLK_MUX_SEL_TOP_FSYS0 + 0x200, 0, 7, 4);
	//rCLK_MUX_SEL_TOP_FSYS1
		//MUX_SCLK_FSYS_MMC1_SDCLKIN_B	[4]	MOUT_SCLK_FSYS_MMC1_SDCLKIN_A	SCLK_FSYS_MMC1_SDCLKIN_B_SEL
		SetBits(rCLK_MUX_SEL_TOP_FSYS1, 4, 1, 0);
		WaitUntilStatus(rCLK_MUX_SEL_TOP_FSYS1 + 0x200, 4, 7, 4);
		//MUX_SCLK_FSYS_MMC1_SDCLKIN_A	[0]	OSCCLK	SCLK_FSYS_MMC1_SDCLKIN_A_SEL
		SetBits(rCLK_MUX_SEL_TOP_FSYS1, 0, 1, 1);
		WaitUntilStatus(rCLK_MUX_SEL_TOP_FSYS1 + 0x200, 0, 7, 4);
	//rCLK_MUX_SEL_TOP_FSYS2
		//MUX_SCLK_FSYS_MMC2_SDCLKIN_B	[4]	MOUT_SCLK_FSYS_MMC2_SDCLKIN_A	SCLK_FSYS_MMC2_SDCLKIN_B_SEL
		SetBits(rCLK_MUX_SEL_TOP_FSYS2, 4, 1, 0);
		WaitUntilStatus(rCLK_MUX_SEL_TOP_FSYS2 + 0x200, 4, 7, 4);
		//MUX_SCLK_FSYS_MMC2_SDCLKIN_A	[0]	OSCCLK	SCLK_FSYS_MMC2_SDCLKIN_A_SEL
		SetBits(rCLK_MUX_SEL_TOP_FSYS2, 0, 1, 1);
		WaitUntilStatus(rCLK_MUX_SEL_TOP_FSYS2 + 0x200, 0, 7, 4);
	//rCLK_MUX_SEL_TOP_PERI
		//MUX_SCLK_PERI_UART2_EXT_UCLK	[28]	OSCCLK	SCLK_PERI_UART2_EXT_UCLK_SEL
		SetBits(rCLK_MUX_SEL_TOP_PERI, 28, 1, 1);
		WaitUntilStatus(rCLK_MUX_SEL_TOP_PERI + 0x200, 28, 7, 4);
		//MUX_SCLK_PERI_UART1_EXT_UCLK	[24]	OSCCLK	SCLK_PERI_UART1_EXT_UCLK_SEL
		SetBits(rCLK_MUX_SEL_TOP_PERI, 24, 1, 1);
		WaitUntilStatus(rCLK_MUX_SEL_TOP_PERI + 0x200, 24, 7, 4);
		//MUX_SCLK_PERI_UART0_EXT_UCLK	[20]	OSCCLK	SCLK_PERI_UART0_EXT_UCLK_SEL
		SetBits(rCLK_MUX_SEL_TOP_PERI, 20, 1, 1);
		WaitUntilStatus(rCLK_MUX_SEL_TOP_PERI + 0x200, 20, 7, 4);
		//MUX_SCLK_PERI_SPI2_SPI_EXT_CLK	[8]	OSCCLK	SCLK_PERI_SPI2_SPI_EXT_CLK_SEL
		SetBits(rCLK_MUX_SEL_TOP_PERI, 8, 1, 1);
		WaitUntilStatus(rCLK_MUX_SEL_TOP_PERI + 0x200, 8, 7, 4);
		//MUX_SCLK_PERI_SPI1_SPI_EXT_CLK	[4]	OSCCLK	SCLK_PERI_SPI1_SPI_EXT_CLK_SEL
		SetBits(rCLK_MUX_SEL_TOP_PERI, 4, 1, 1);
		WaitUntilStatus(rCLK_MUX_SEL_TOP_PERI + 0x200, 4, 7, 4);
		//MUX_SCLK_PERI_SPI0_SPI_EXT_CLK	[0]	OSCCLK	SCLK_PERI_SPI0_SPI_EXT_CLK_SEL
		SetBits(rCLK_MUX_SEL_TOP_PERI, 0, 1, 1);
		WaitUntilStatus(rCLK_MUX_SEL_TOP_PERI + 0x200, 0, 7, 4);
	//rCLK_MUX_SEL_TOP_ISP
		//MUX_SCLK_ISP_SENSOR1	[24]	OSCCLK	SCLK_ISP_SENSOR1_SEL
		SetBits(rCLK_MUX_SEL_TOP_ISP, 24, 1, 1);
		WaitUntilStatus(rCLK_MUX_SEL_TOP_ISP + 0x200, 24, 7, 4);
		//MUX_SCLK_ISP_SENSOR0	[20]	OSCCLK	SCLK_ISP_SENSOR0_SEL
		SetBits(rCLK_MUX_SEL_TOP_ISP, 20, 1, 1);
		WaitUntilStatus(rCLK_MUX_SEL_TOP_ISP + 0x200, 20, 7, 4);
		//MUX_SCLK_ISP_UART_ISP_EXT_UCLK	[16]	OSCCLK	SCLK_ISP_UART_ISP_EXT_UCLK_SEL
		SetBits(rCLK_MUX_SEL_TOP_ISP, 16, 1, 1);
		WaitUntilStatus(rCLK_MUX_SEL_TOP_ISP + 0x200, 16, 7, 4);
		//MUX_SCLK_ISP_SPI1_ISP_SPI_EXT_CLK	[4]	OSCCLK	SCLK_ISP_SPI1_ISP_SPI_EXT_CLK_SEL
		SetBits(rCLK_MUX_SEL_TOP_ISP, 4, 1, 1);
		WaitUntilStatus(rCLK_MUX_SEL_TOP_ISP + 0x200, 4, 7, 4);
		//MUX_SCLK_ISP_SPI0_ISP_SPI_EXT_CLK	[0]	OSCCLK	SCLK_ISP_SPI0_ISP_SPI_EXT_CLK_SEL
		SetBits(rCLK_MUX_SEL_TOP_ISP, 0, 1, 1);
		WaitUntilStatus(rCLK_MUX_SEL_TOP_ISP + 0x200, 0, 7, 4);
	//rCLK_MUX_SEL_ISP1
		//MUX_ACLK_ISP_266_USER	8
		//MUX_ACLK_ISP_333_USER	4
		//MUX_ACLK_ISP_400_USER	0
		Outp32(rCLK_MUX_SEL_ISP1,(1<<8)|(1<<4)|(1<<0));
		WaitUntilStatus(rCLK_MUX_SEL_ISP1 + 0x200, 8, 7, 4);
		WaitUntilStatus(rCLK_MUX_SEL_ISP1 + 0x200, 4, 7, 4);
		WaitUntilStatus(rCLK_MUX_SEL_ISP1 + 0x200, 0, 7, 4);
	}
}

void InitCmuForWakeupWithCp(void) // 1086 825
{
	InitCpuClk();
	if(Inp32(MIF_INIT_DONE)==false)
	{
		Outp32(AP_FLAG,true);
		Outp32(INIT_TURN,CP_TURN);
		while((Inp32(CP_FLAG)==true)&&(Inp32(INIT_TURN)==CP_TURN));
		if(Inp32(MIF_INIT_DONE)==false)
		{
			ApInitMif();
			Outp32(MIF_INIT_DONE,CHECK_DONE);
		}
		Outp32(AP_FLAG,false);
	}
	else
	{
		if(Inp32(MIF_MUX_DONE)==false)
		{
			Outp32(AP_FLAG,true);
			Outp32(INIT_TURN,CP_TURN);
			while((Inp32(CP_FLAG)==true)&&(Inp32(INIT_TURN)==CP_TURN));
			if(Inp32(MIF_MUX_DONE)==false)
			{
				CpInitApChange();
				Outp32(MIF_MUX_DONE,CHECK_DONE);
				PostCpInitApChange();
			}
			else
			{
				CpInitCpChange();
			}
			Outp32(AP_FLAG,false);
		}
		else
		{
			CpInitCpChange();
		}
	}
	InitAplClk();
}

void InitCmuForBl(bool bIsReset)
{
//Set TOP User Mux Low Freq
	//CLK_MUX_SEL_TOP1
	//MEDIA_PLL_TOP_USER_SEL	[8]
	//BUS_PLL_TOP_USER_SEL		[4]
	//AUD_PLL_USER_SEL			[0]
	Outp32(rCLK_MUX_SEL_TOP1,(0<<8)|(0<<4)|(0<<0));
	WaitUntilStatus(rCLK_MUX_SEL_TOP1+0x200, 8, 7, 4);
	WaitUntilStatus(rCLK_MUX_SEL_TOP1+0x200, 4, 7, 4);
	WaitUntilStatus(rCLK_MUX_SEL_TOP1+0x200, 0, 7, 4);
	if(bIsReset)
	{
		InitCpuClk();
		InitAplClk();
		InitMemClk(825);
		InitIntClk(bIsReset);
	}
	else
	{
		InitCmuForWakeupWithCp();
		InitIntClk(bIsReset);
	}
}
