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

#include <config.h>
#include <asm/arch/smc.h>

extern void InitMemClk(u32 uMemclk);

typedef unsigned int		u32;
typedef unsigned long long  u64;

typedef volatile unsigned int		 u32v;		 //- 32-bit unsigned integer
typedef volatile unsigned long long	 u64v;		 //- 32-bit unsigned integer

#define __REG32(x)							(*((u32v *)((u64)x)))
#define __REG64(x)							(*((u64v *)((u64)x)))

#define Outp32(addr, data) (*(volatile u32 *)(addr) = (data))
#define Inp32(addr) ((*(volatile u32 *)(addr)))

typedef enum{
	DRAM_DS_34_OHM = 1,
	DRAM_DS_40_OHM = 2,
	DRAM_DS_48_OHM = 3,
	DRAM_DS_60_OHM = 4,
	DRAM_DS_80_OHM = 5,
} dram_ds_e;

typedef enum{
	AP_DS_48_OHM = 4,
	AP_DS_40_OHM = 5,
	AP_DS_34_OHM = 6,
	AP_DS_30_OHM = 7,
} ap_ds_e;

typedef struct {
	u32 rfc_pb;
	u32 timing_row;
	u32 timing_data;
	u32 timing_power;
	u32 rd_fetch;
	u32 ctrl_gateduradj;
	u32 ctrl_gateadj;
	u32 rl;
	u32 wl;
	ap_ds_e ap_ds;
	dram_ds_e dram_ds;
} drex_timing_parameter_t;

#define PWM_BASE_ADDRESS 0x13970000   // Insel-D PWM Base Address

drex_timing_parameter_t *drex_timing_parameter;

drex_timing_parameter_t g_timing_parameter[] = {
//    rfcpb    row        data       power     fetch  gateduradj gateadj    RL    WL        AP_DS           DRAM_DS
	{ 0x19, 0x36588652, 0x4740185E, 0x4C3A4746, 0x3,		2,		1,		14,    8,    AP_DS_30_OHM, DRAM_DS_34_OHM }, //825
	{ 0x0D, 0x1C34534A, 0x2620183E,	0x281F2425, 0x2,        0,		1,		14,    8,    AP_DS_30_OHM, DRAM_DS_34_OHM }, //416
};


void DREX_read_timing_parameter (u32 mem_clk)
{
	if (mem_clk==825)
		drex_timing_parameter = &(g_timing_parameter[0]);
	else if (mem_clk==416)
		drex_timing_parameter = &(g_timing_parameter[1]);
}


u32 PWM_get_tcnt(u32 prescaler, u32 divider, u32 target_time)
{
	u32 freq;
	double temp, cnt;

	freq = 66666666;
	temp = (double)freq/(prescaler+1)/divider;

	cnt= (double)target_time * temp / 1000000;

	if (cnt >= 0xFFFFFFFF || cnt <= 0)
	{
		while(1);
	}

	if (cnt-(u32)cnt >= 0.5)
		cnt=(u32)cnt + 1;
	else
		cnt=(u32)cnt;

	if (cnt>0)
		return cnt-1;
	else
		return cnt;
}

void PWM_timer_stop(u32 timer)
{
    if(timer == 0)
		 __REG32(PWM_BASE_ADDRESS+0x08) = __REG32(PWM_BASE_ADDRESS+0x08) & ~( 0x1 );
    else
		 __REG32(PWM_BASE_ADDRESS+0x08) = __REG32(PWM_BASE_ADDRESS+0x08) & ~( (0x1)<<((timer+1)*4) );
}

void PWM_configuration (u32 timer,u32 prescaler,u32 divider,u32 tcntb,u32 tcmpb,u32 en_int)
{
	PWM_timer_stop(timer);

	__REG32(PWM_BASE_ADDRESS+0x04) = ( __REG32(PWM_BASE_ADDRESS+0x04) & (~(0xF << 4*timer)) ) | ( divider << 4*timer );
    __REG32(PWM_BASE_ADDRESS+0x00) = ( __REG32(PWM_BASE_ADDRESS+0x00) & (~(0xFF00)) ) | ( prescaler << 8 ) ;
	__REG32(PWM_BASE_ADDRESS+0x24) = tcntb;
	__REG32(PWM_BASE_ADDRESS+0x28) = tcmpb;

	__REG32(PWM_BASE_ADDRESS+0x08) = __REG32(PWM_BASE_ADDRESS+0x08) & ~(0xF000) | (1<<13) | (0<<12);
	__REG32(PWM_BASE_ADDRESS+0x08) = __REG32(PWM_BASE_ADDRESS+0x08) & ~(0xF000) | (0<<13) | (1<<12);
}

u32 PWM_delay(u32 target_time)
{
	u32  start_tcntb, current_tcntb, cnt, pwm_runnning;

	PWM_timer_stop(2);

	cnt = PWM_get_tcnt(0, 1, target_time);
	cnt = cnt + 1;																// !!! Must (+1) since it doesnt add interrupt tick.

	pwm_runnning = ( __REG32(PWM_BASE_ADDRESS+0x08) & (2 << 12) );
	if ( !pwm_runnning )
	{
		current_tcntb= __REG32(PWM_BASE_ADDRESS+0x2C);
		PWM_configuration(1, 0, 0, 0xFFFFFFFF, (u32)(0xFFFFFFFF/2), 0);
		while( __REG32(PWM_BASE_ADDRESS+0x2C) == current_tcntb );
	}
	else
		while(1);

	start_tcntb = __REG32(PWM_BASE_ADDRESS+0x2C);
	do
	{
		current_tcntb = __REG32(PWM_BASE_ADDRESS+0x2C);
		if( (start_tcntb-current_tcntb) >= cnt )
		{
			PWM_timer_stop(2);
			break;
		}
	}while(1);

	return 0;
}

void DMC_Init(u32 mem_clk, u32 resume)
{
#if defined(CONFIG_SMC_CMD)
	reg_arr_t reg_arr;
#endif
	DREX_read_timing_parameter(mem_clk);

	InitMemClk(51);

	// PHY INITIALIZATION
	Outp32(0x10420000, 0x6708080C);   //"__PHY->GNR_CON0.ctrl_ddr_mode=0x3 .ctrl_upd_time=0x1 .ctrl_upd_range=0x2"
	Outp32(0x10400004, 0x08312700);   //"__DREX->MEMCONTROL.dpwrdn_type=0x0, .mem_type=0x7, .mem_width=0x2, .num_chip=0x1, .num_chunk=0x0, .bl=0x3, .pb_ref_en=0x1"
	Outp32(0x104200D0, 0x0F087304);   //"__PHY->ZQ_CON0.zq_mode_dds=0x7"
	Outp32(0x10400310, 0x00000021);   //"__DREX-->PLCONFIG.dq_pipeline=0x2, __DREX-->PLCONFIG.ca_pipeline=0x1"
	Outp32(0x10400000, 0x0FFF1100);   //"__DREX->CONCONTROL.aref_en=0x0, __DREX->CONCONTROL.io_pd_con=0x0"
	Outp32(0x10420000, 0x6709080E);   //"__PHY->GNR_CON0.rdlat=0x0E, __PHY->GNR_CON0.bstlen=0x8, __PHY->GNR_CON0.wrlat=0x9"
	Outp32(0x10420018, 0x0000000F);   //"__PHY->LPCON0.ctrl_pulld_dqs=0x0F"
	Outp32(0x104200D4, 0x003F0000);   //"__PHY->ZQ_CON1.zq_vref=0x3F"
	Outp32(0x104200D0, 0x0F0C7304);   //"__PHY->ZQ_CON0.zq_clk_div_en=0x1"
	Outp32(0x104200D0, 0x0F0C7306);   //"__PHY->ZQ_CON0.zq_manual_str=0x1"
	while( (Inp32(0x104200D4)&0x1) != 0x1 );
	Outp32(0x104200D0, 0x0F0C7304);   //"__PHY->ZQ_CON0.zq_manual_str=0x0"
	Outp32(0x10420050, 0x01004700);   //"__PHY->OFFSETD_CON0.ctrl_resync=0x1"
	Outp32(0x10420050, 0x00004700);   //"__PHY->OFFSETD_CON0.ctrl_resync=0x0"
	Outp32(0x104200B0, 0x10100050);   //"__PHY->MDLL_CON0.ctrl_dll_on=0x0"
	Outp32(0x10400000, 0x1FFF1100);   //"__DREX->CONCONTROL.dfi_init_start=0x1"
	while( (Inp32(0x10400040)&0x8) != 0x8 );
	Outp32(0x10400000, 0x0FFF1100);   //"__DREX->CONCONTROL.dfi_init_start=0x0"
	Outp32(0x10400000, 0x0FFF1108);   //"__DREX->CONCONTROL.update_mode=0x1"
	Outp32(0x10420050, 0x10004700);   //"__PHY->OFFSETD_CON0.upd_mode=0x1"

#if defined(CONFIG_SMC_CMD)
	reg_arr.set0.addr = 0x10410F00;
	reg_arr.set0.val = 0x00020009;
	reg_arr.set1.addr = 0x10410f20;
	reg_arr.set1.val = 0x00000003;
	reg_arr.set2.addr = 0x10410f10;
	reg_arr.set2.val = 0x01432423;
	reg_arr.set3.addr = 0x10410F14;
	reg_arr.set3.val = 0x01432423;
	set_secure_reg((u64)&reg_arr, 4);
#else
	// DREX INITIALIZATION
	Outp32(0x10410F00, 0x00020009);   //"__ASP->MEMBASECONFIG0.chunk_start=0x2, __ASP->MEMBASECONFIG0.chunk_end=0x9,"
	Outp32(0x10410F20, 0x00000003);   //"__ASP->MEMSIZE0.chip0_size=0x3"
	Outp32(0x10410F10, 0x01432423);   //"__ASP->MEMCONFIG0"
	Outp32(0x10410F14, 0x01432423);   //"__ASP->MEMCONFIG1"
#endif
	// Memroy Size Info
	__REG64((u64)CONFIG_PHY_BL_SYS_INFO_DRAM_SIZE) = 0x80000000;

	Outp32(0x10400030, 0x000C0065);   //"__DREX->TIMINGAREF=0xC0065"
	Outp32(0x10400034, drex_timing_parameter->timing_row);          //__DREX->TIMINGROW0=0x36588652
	Outp32(0x10400038, drex_timing_parameter->timing_data);         //__DREX->TIMINGDATA0=0x4740185E
	Outp32(0x1040003C, drex_timing_parameter->timing_power);        //__DREX->TIMINGPOWER0=0x4C3A4746
	Outp32(0x1040004C, drex_timing_parameter->rd_fetch);           //__DREX->RDFETCH0.rd_fetch=0x3
	Outp32(0x10400020, (Inp32(0x10400020) & ~(0x3F<<0) | (drex_timing_parameter->rfc_pb<<0)));   //__DREX->TIMINGRFCPB.t_rfcpb0=0x19
	Outp32(0x1042000C, (Inp32(0x1042000C) & ~(0xF<<24) & ~(0xF<<20) | (drex_timing_parameter->ctrl_gateduradj<<24) | (drex_timing_parameter->ctrl_gateadj<<20)));
	Outp32(0x10420020, 0x70707070);   //"__PHY->OFFSETR_CON0.ctrl_offsetr(0~3)=0x70"
	Outp32(0x10420030, 0x70707070);   //"__PHY->OFFSETW_CON0.ctrl_offsetw(0~3)=0x70"
	Outp32(0x10420050, 0x10004770);   //"__PHY->OFFSETD_CON0.ctrl_offsetd=0x70"
	Outp32(0x104200B0, 0x1010FFD0);   //"__PHY->MDLL_CON0.ctrl_force=0x1FF"
	Outp32(0x104200B0, 0x1010FFD0);   //"__PHY->MDLL_CON0.ctrl_on=0x0"
	Outp32(0x10420050, 0x11004770);   //"__PHY->OFFSETD_CON0.ctrl_resync=0x1"
	Outp32(0x10420050, 0x10004770);   //"__PHY->OFFSETD_CON0.ctrl_resync=0x0"
	Outp32(0x10420050, 0x11004770);   //"__PHY->OFFSETD_CON0.ctrl_resync=0x1"
	Outp32(0x10420050, 0x10004770);   //"__PHY->OFFSETD_CON0.ctrl_resync=0x0"

	// MEMORY INITIALIZATION
	if (resume == 0x10)
	{
	Outp32(0x10400010, 0x07000000);   //"__DREX->DIRECTCMD=DREX_NOP_COMMAND, DREX_CHIP_0, X, X"
	PWM_delay(200);
	Outp32(0x10400010, 0x00071C00);   //"__DREX->DIRECTCMD=DREX_MRW_COMMAND, DREX_CHIP_0, MR63_RESET, X"
	Outp32(0x10400010, 0x09000000);   //"__DREX->DIRECTCMD=DREX_MRR_COMMAND, DREX_CHIP_0, X, X"
	while( (Inp32(0x10400054)&0x1) != 0x0 );
	PWM_delay(10);
	Outp32(0x10400010, 0x00010BFC);   //"__DREX->DIRECTCMD=DREX_MRW_COMMAND, DREX_CHIP_0, MR10_IO_CAL, 0xFF"
	Outp32(0x10400010, 0x00000870);   //"__DREX->DIRECTCMD=DREX_MRW_COMMAND, DREX_CHIP_0, MR2_DEVICE_FEATURE2, dram_timing_parameter->mr2"
	Outp32(0x10400010, 0x0000060C);   //"__DREX->DIRECTCMD=DREX_MRW_COMMAND, DREX_CHIP_0, MR1_DEVICE_FEATURE1, dram_timing_parameter->mr1"
	Outp32(0x10400010, 0x00000C04);   //"__DREX->DIRECTCMD=DREX_MRW_COMMAND, DREX_CHIP_0, MR3_IO_CONFIG, dram_timing_parameter->mr3"
	Outp32(0x10400010, 0x07100000);   //"__DREX->DIRECTCMD=DREX_NOP_COMMAND, DREX_CHIP_1, X, X"
	PWM_delay(200);
	Outp32(0x10400010, 0x00171C00);   //"__DREX->DIRECTCMD=DREX_MRW_COMMAND, DREX_CHIP_1, MR63_RESET, X"
	Outp32(0x10400010, 0x09100000);   //"__DREX->DIRECTCMD=DREX_MRR_COMMAND, DREX_CHIP_1, X, X"
	while( (Inp32(0x10400054)&0x1) != 0x0 );
	PWM_delay(10);
	Outp32(0x10400010, 0x00110BFC);   //"__DREX->DIRECTCMD=DREX_MRW_COMMAND, DREX_CHIP_1, MR10_IO_CAL, 0xFF"
	Outp32(0x10400010, 0x00100870);   //"__DREX->DIRECTCMD=DREX_MRW_COMMAND, DREX_CHIP_1, MR2_DEVICE_FEATURE2, dram_timing_parameter->mr2"
	Outp32(0x10400010, 0x0010060C);   //"__DREX->DIRECTCMD=DREX_MRW_COMMAND, DREX_CHIP_1, MR1_DEVICE_FEATURE1, dram_timing_parameter->mr1"
	Outp32(0x10400010, 0x00100C04);   //"__DREX->DIRECTCMD=DREX_MRW_COMMAND, DREX_CHIP_1, MR3_IO_CONFIG, dram_timing_parameter->mr3"
	}
	else
	{
	Outp32(0x10400010, 0x08000000);   //__DREX->DIRECTCMD=DREX_SRX_COMMAND, DREX_CHIP_0, X, X
	Outp32(0x10400010, 0x08100000);   //__DREX->DIRECTCMD=DREX_SRX_COMMAND, DREX_CHIP_1, X, X
	}

	// DYNAMIC FEATURE, CLOCK GATING SETTINGS
	InitMemClk(mem_clk);
	Outp32(0x10420020, 0x00000000);   //"__PHY->OFFSETR_CON0.ctrl_offsetr(0~3)=0x0"
	Outp32(0x10420030, 0x00000000);   //"__PHY->OFFSETW_CON0.ctrl_offsetw(0~3)=0x0"
	Outp32(0x10420050, 0x10004700);   //"__PHY->OFFSETD_CON0.ctrl_offsetd=0x0"
	Outp32(0x10420050, 0x11004700);   //"__PHY->OFFSETD_CON0.ctrl_resync=0x1"
	Outp32(0x10420050, 0x10004700);   //"__PHY->OFFSETD_CON0.ctrl_resync=0x0"
	Outp32(0x104200B0, 0x1010FFD0);   //"__PHY->MDLL_CON0.ctrl_dll_on=0x0"
	Outp32(0x104200B0, 0x1010FF90);   //"__PHY->MDLL_CON0.ctrl_start=0x0"
	Outp32(0x104200B0, 0x1010FF90);   //"__PHY->MDLL_CON0.ctrl_ref=0x8"
	Outp32(0x104200B0, 0x1010FFB0);   //"__PHY->MDLL_CON0.ctrl_dll_on=0x1"
	Outp32(0x104200B0, 0x1010FFF0);   //"__PHY->MDLL_CON0.ctrl_start=0x1"
	while( (Inp32(0x104200B4) & 0x5) != 0x5 );
	Outp32(0x10420050, 0x11004700);   //"__PHY->OFFSETD_CON0.ctrl_resync=0x1"
	Outp32(0x10420050, 0x10004700);   //"__PHY->OFFSETD_CON0.ctrl_resync=0x0"
	Outp32(0x10400014, 0x10000000);   //"__DREX->PRECHCONFIG0.tp_en=0x1, __DREX->PRECHCONFIG0.port_policy=0x0"
	Outp32(0x1040001C, 0xFFFFFFFF);   //"__DREX->PRECHCONFIG1.tp_cnt(0~3)=0xFF"
	Outp32(0x10400004, 0x08312763);   //"__DREX->MEMCONTROL.clk_stop_en=0x1, __DREX->MEMCONTROL.dpwrdn_en=0x1, __DREX->MEMCONTROL.dsref_en=0x1, __DREX->MEMCONTROL.add_lat_pall=0x1"
	Outp32(0x10400028, 0x1FFF000D);   //"__DREX->PWRDNCONFIG.dsref_cyc=0x1FFF, __DREX->PWRDNCONFIG.dpwrdn_cyc=0x0D"
	Outp32(0x10400008, 0x000003FF);   //"__DREX->CGCONTROL"
	Outp32(0x10400000, 0x0FFF11A8);   //"__DREX->CONCONTROL.aref_en=0x1, __DREX->CONCONTROL.io_pd_con=0x2"
	Outp32(0x10420004, 0x04600000);   //"__PHY->CAL_CON0.cal_vtc_en=0x1"
	Outp32(0x10400000, 0x0FFF11A0);   //"__DREX->CONCONTROL.update_mode=0x0"
	Outp32(0x10420050, 0x00004700);   //"__PHY->OFFSETD_CON0.upd_mode=0x0"
	Outp32(0x10400400, 0x00000001);   //"__DREX->ALL_INIT_DONE.all_init_done=0x1"
}
