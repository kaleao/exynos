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
#include <asm/types.h>
#include <asm/arch/smc.h>
#include "dmc/dmc.h"

#define CONFIG_DRAM_MULTI_CAPACITY_SUPPORT

typedef volatile unsigned char   		u8v;		//-  8-bit unsigned integer
typedef volatile unsigned short  		u16v;		//- 16-bit unsigned integer
typedef volatile unsigned int   		u32v;		//- 32-bit unsigned integer
typedef volatile unsigned long long		u64v;		//- 32-bit unsigned integer

#define __REG8(x)							(*((u8v *)((u64)x)))
#define __REG16(x)							(*((u16v *)((u64)x)))
#define __REG32(x)							(*((u32v *)((u64)x)))
#define __REG64(x)							(*((u64v *)((u64)x)))


#if 0
volatile drex_regs_t *__DREX[];
volatile phy_regs_t *__PHY[];
volatile asp_regs_t *__ASP[];
#else
volatile drex_regs_t *__DREX[4];
volatile drex_ppc_regs_t *__PPC[4];
volatile phy_regs_t *__PHY[4];
volatile asp_regs_t *__ASP[4];
#endif

dmc_driver_t DmcDriver;

unsigned int get_vref_fuse_bits(void)
{
	return *(volatile unsigned int *)(0x1000003C);
}

static void* dmc_memset(void *dest, int value, unsigned int size)
{
	unsigned int i;

	void *temp = dest;

	for(i=0;i<size; i++)
	{
		*((char*)temp+i) = value;
	}

	return dest;
}


/******************************************************************************
 *
 * @fn		dmc_ppc_init
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static int dmc_ppc_init(int ch, int enable)
{
	if( enable == 1 ) {
		__DREX[ch]->PPCCLKCON.perev_clk_en = 1;
		__PPC[ch]->PMNC_PPC.start_mode = 0;
		__PPC[ch]->PMNC_PPC.ppc_enable = 1;
		__PPC[ch]->PMNC_PPC.cc_reset = 1;
		__PPC[ch]->PMNC_PPC.ppc_counter_reset = 1;
		__PPC[ch]->CNTENS_PPC.ccnt = 1;
	} else {
		__PPC[ch]->CNTENC_PPC.ccnt = 1;
		__PPC[ch]->PMNC_PPC.cc_reset = 1;
		__PPC[ch]->PMNC_PPC.ppc_counter_reset = 1;
		__PPC[ch]->PMNC_PPC.ppc_enable = 0;
		__DREX[ch]->PPCCLKCON.perev_clk_en = 0;
	}

	return( 0 );
}

/******************************************************************************
 *
 * @fn		dmc_get_ccnt
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static unsigned int dmc_get_ccnt(int ch)
{
	return( __PPC[ch]->CCNT_PPC.ccnt );
}

/******************************************************************************
 *
 * @fn		dmc_wait
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static void dmc_wait(int ch, float sec)
{

#if 0
	float elapsed;
	unsigned int a, b;

	dmc_ppc_init( ch , 1 );

	a = dmc_get_ccnt( ch );

	elapsed = 0;
	while( elapsed < sec ) {

		b = dmc_get_ccnt( ch );
		elapsed = (float)( b - a ) / (float)800000000.0;				//TEMP:
	}

	dmc_ppc_init( ch , 0 );
#else
	int i;
	i=100000;
	while(i--);
#endif

	return;
}

/******************************************************************************
 *
 * @fn		dmc_reset
 *
 * @brief	control DRAM reset signal
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static void dmc_reset(int ch, int reset)
{
	__REG32( 0x10890000 + 0x40 ) = reset;
	__REG32( 0x10990000 + 0x40 ) = reset;
	__REG32( 0x10a90000 + 0x40 ) = reset;
	__REG32( 0x10b90000 + 0x40 ) = reset;
}

/******************************************************************************
 *
 * @fn		dmc_init_done
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static int dmc_init_done(int ch)
{
	__DREX[ch]->CONCONTROL.aref_en = 1;
	__DREX[ch]->ALL_INT_INDI.all_init_done = 1;

	return 0;
}

int dmc_vtc_en(int ch)
{
	__PHY[ch]->CAL_CON0.cal_vtc_en= 1;

	return 0;
}

/******************************************************************************
 *
 * @fn		dmc_set_dds
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static int dmc_set_dds(int ch, int byte, int pu, int pd)
{
	return 0;
}

/******************************************************************************
 *
 * @fn		dmc_set_odt
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
#if 0
static void dmc_set_odt(int ch, int byte, int odt)
{
	if( odt != 0 ) {

		__PHY[ch]->ZQ_CON0.zq_mode_lp4 = 1;
		__PHY[ch]->ZQ_CON6.zq_ds0_lp4 = 1;
		__PHY[ch]->ZQ_CON6.zq_ds1_lp4 = 1;
		__PHY[ch]->CAL_CON2.ctrl_rodt_disable = 0;

		__PHY[ch]->ZQ_CON0.zq_mode_noterm = 0;
		__PHY[ch]->ZQ_CON6.zq_ds0_noterm = 0;
		__PHY[ch]->ZQ_CON6.zq_ds1_noterm = 0;

		__PHY[ch]->ZQ_CON0.zq_mode_term = 1;

		switch( byte ) {

			case 0:
				__PHY[ch]->ZQ_CON6.zq_ds0_term = odt;
				break;

			case 1:
				__PHY[ch]->ZQ_CON6.zq_ds1_term = odt;
				break;

			default:
				__PHY[ch]->ZQ_CON6.zq_ds0_term = odt;
				__PHY[ch]->ZQ_CON6.zq_ds1_term = odt;
		}

	} else {

		__PHY[ch]->ZQ_CON0.zq_mode_lp4 = 0;
		__PHY[ch]->ZQ_CON6.zq_ds0_lp4 = 0;
		__PHY[ch]->ZQ_CON6.zq_ds1_lp4 = 0;

		__PHY[ch]->ZQ_CON0.zq_mode_noterm = 1;
		__PHY[ch]->ZQ_CON6.zq_ds0_noterm = 1;
		__PHY[ch]->ZQ_CON6.zq_ds1_noterm = 1;

		__PHY[ch]->ZQ_CON0.zq_mode_term = 0;
		__PHY[ch]->ZQ_CON6.zq_ds0_term = 0;
		__PHY[ch]->ZQ_CON6.zq_ds1_term = 0;

		__PHY[ch]->CAL_CON2.ctrl_rodt_disable = 1;
	}

	return;
}
#endif
/******************************************************************************
 *
 * @fn
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static void dmc_zq_cal(int ch)
{

	__PHY[ch]->ZQ_CON0.zq_clk_div_en = 1;
	__PHY[ch]->ZQ_CON0.zq_manual_mode = 1;
	__PHY[ch]->ZQ_CON0.zq_manual_str = 1;

	while(__PHY[ch]->ZQ_CON1.zq_done != 1 );

	__PHY[ch]->ZQ_CON0.zq_manual_str = 0;
	__PHY[ch]->ZQ_CON0.zq_clk_div_en = 0;

}

/******************************************************************************
 *
 * @fn
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
#if 0
static void dmc_set_vref(int ch, int byte, dmc_vref_t *vref)
{
	switch( byte ) {

		case 0:
			__PHY[ch]->ZQ_CON9.zq_ds0_vref_pd = vref->pd;
			__PHY[ch]->ZQ_CON9.zq_ds0_vref_fsbst = vref->fsbst;
			__PHY[ch]->ZQ_CON9.zq_ds0_vref = vref->vref;
			break;

		case 1:
			__PHY[ch]->ZQ_CON9.zq_ds1_vref_pd = vref->pd;
			__PHY[ch]->ZQ_CON9.zq_ds1_vref_fsbst = vref->fsbst;
			__PHY[ch]->ZQ_CON9.zq_ds1_vref = vref->vref;
			break;
	}

	return;
}
#endif

void dmc_low_power(int ch)
{
	__DREX[ch]->PWRDNCONFIG.dsref_cyc = 0x3FF;
	__DREX[ch]->PWRDNCONFIG.dpwrdn_cyc = 0x2;

#if MC_INITIATED_SDLL_UPDATE
	__DREX[ch]->MEMCONTROL.dsref_en = 0;
#else
	__DREX[ch]->MEMCONTROL.dsref_en = 0;
#endif

	__DREX[ch]->MEMCONTROL.dpwrdn_en = 0;
	__DREX[ch]->MEMCONTROL.clk_stop_en = 1;
	__REG32( (u64)&__DREX[ch]->CGCONTROL ) = 0x3FF;

#if DRAM_POWER_OPTIMIZE
	__DREX[ch]->PRECHCONFIG0.tp_en = 0xC;
	__DREX[ch]->PRECHCONFIG1.tp_cnt0 = 0xA;	//MM
	__DREX[ch]->PRECHCONFIG1.tp_cnt1 = 0xA;	//CPU

	/* added in evt1 */
	__PHY[ch]->LP_CON0.mdll_cg_en = 1;
#endif

	return;
}

void dmc_io_pd(int ch)
{
	__PHY[ch]->LP_CON0.cs_io_pd = 1;
	__PHY[ch]->LP_CON0.ds_io_pd = 1;
	__PHY[ch]->LP_CON0.ds_pd= 1;
	__PHY[ch]->LP_CON0.pcl_pd = 1;

//	__PHY[ch]->CAL_CON0.dgate_en = 1;
//	__PHY[ch]->CAL_CON0.freq_offset_en = 1;

	return;
}

/******************************************************************************
 *
 * @fn
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static void dmc_dfi_init(int ch, int init)
{
	__DREX[ch]->CONCONTROL.dfi_init_start = init;
}

/******************************************************************************
 *
 * @fn
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static void dmc_dfi_update(int ch)
{
	__DREX[ch]->PHYCONTROL0.fp_resync = 1;
	__DREX[ch]->PHYCONTROL0.fp_resync = 0;

	return;
}

void dmc_phy_upd_mode(int ch, int mode)
{
	__PHY[ch]->OFFSETD_CON0.upd_mode = mode;

	return;
}

/******************************************************************************
 *
 * @fn
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static int dmc_dfi_status(int ch)
{
	return( __DREX[ch]->PHYSTATUS.dfi_init_complete );
}

/******************************************************************************
 *
 * @fn
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static void dmc_dfi_mode(int ch, int mode, int ignore)
{
	if( ignore == 1) {
		__DREX[ch]->CONCONTROL.ignore_dic = ignore;
		__DREX[ch]->CONCONTROL.update_mode = __PHY[ch]->OFFSETD_CON0.upd_mode = mode;
	} else {
		__DREX[ch]->CONCONTROL.update_mode = __PHY[ch]->OFFSETD_CON0.upd_mode = mode;
		__DREX[ch]->CONCONTROL.ignore_dic = ignore;
	}
}

/******************************************************************************
 *
 * @fn
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
#if 0
static void phy_set_addr_offset(int ch, signed char offset)
{
	unsigned char offsetd;

	if( offset < 0 ) {
		offsetd = ( -offset ) | 0x80;
	} else {
		offsetd = offset;
	}

	__PHY[ch]->OFFSETD_CON0.ctrl_offsetd = offsetd;
}
#endif


/******************************************************************************
 *
 * @fn
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
#if 0
static int phy_set_write_dq_offset(int ch, int byte, signed char offset)
{
	int ret = 1;
	unsigned char offsetw;

	if( offset < 0 ) {
		offsetw = ( -offset ) | 0x80;
	} else {
		offsetw = offset;
	}

	switch( byte ) {

		case 0:
			__PHY[ch]->OFFSETW_CON0.ctrl_offsetw0 = offsetw;
			break;

		case 1:
			__PHY[ch]->OFFSETW_CON0.ctrl_offsetw1 = offsetw;
			break;

		default:
			ret = 0;
	}

	return( ret );
}
#endif

/******************************************************************************
 *
 * @fn
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
#if 0
static int phy_set_read_dqs_offset(int ch, int byte, signed char offset)
{
	int ret = 1;
	unsigned char offsetr;

	if( offset < 0 ) {
		offsetr = ( ( -offset ) & 0x7f ) | 0x80;
	} else {
		offsetr = offset & 0x7f;
	}

	switch( byte ) {

		case 0:
			__PHY[ch]->OFFSETR_CON0.ctrl_offsetr0 = offsetr;
			break;

		case 1:
			__PHY[ch]->OFFSETR_CON0.ctrl_offsetr1 = offsetr;
			break;

		default:
			ret = 0;
	}

	return( ret );
}
#endif

/******************************************************************************
 *
 * @fn
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static int dmc_dll_resync(int ch)
{
	__PHY[ch]->OFFSETD_CON0.ctrl_resync = 1;
	__PHY[ch]->OFFSETD_CON0.ctrl_resync = 0;

	return 0;
}


/******************************************************************************
 *
 * @fn
 *
 * @brief
 *
 * @author
 *
 * @date
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static int dmc_dll_enable(int ch, int enable)
{
	__PHY[ch]->MDLL_CON0.ctrl_dll_on = enable;
	return 0;
}

/******************************************************************************
 *
 * @fn
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static int dmc_dll_lock_stat(int ch)
{
	return 0;
}

/******************************************************************************
 *
 * @fn
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static int dmc_dll_relock(int ch)
{
	return 0;
}

/******************************************************************************
 *
 * @fn
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static int dmc_get_dll_lock_value(int ch)
{
	return 0;
}

/******************************************************************************
 *
 * @fn
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static int dmc_dll_force(int ch, int force)
{
	return 0;
}

/******************************************************************************
 *
 * @fn
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static void dmc_mrw_command(unsigned char ch, unsigned char rank, unsigned char ma, unsigned char op)
{
	unsigned char bank_bf;
	unsigned short addr_bf;
	unsigned char addr_bf_1_0;
	unsigned char addr_bf_12_10;
	unsigned char addr_bf_9_2;

	bank_bf 	  = ( ma >> 3 ) & 0x7;
	addr_bf_12_10 = ma & 0x7;
	addr_bf_1_0   = ma >> 6;
	addr_bf_9_2   = op;

	addr_bf = 0;
	addr_bf = ( addr_bf_12_10 << 10 ) | ( addr_bf_9_2 << 2 ) | ( addr_bf_1_0 );

	switch( rank ) {
		case 0:
		case 1:
			__REG32( (u64)&__DREX[ch]->DIRECTCMD ) = ( DREX_MRW_COMMAND << 24 ) | ( rank << 20 ) | ( bank_bf << 16 ) | addr_bf;
			break;

		default:
			__REG32( (u64)&__DREX[ch]->DIRECTCMD ) = ( DREX_MRW_COMMAND << 24 ) | ( 1 << 21 ) | ( bank_bf << 16 ) | addr_bf;
	}
}

/******************************************************************************
 *
 * @fn
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static void dmc_mrr_command(unsigned char ch, unsigned char rank, unsigned char ma)
{
	unsigned char bank_bf;
	unsigned short addr_bf;
	unsigned char addr_bf_1_0;
	unsigned char addr_bf_12_10;

	bank_bf 	  = ( ma >> 3 ) & 0x7;
	addr_bf_12_10 = ma & 0x7;
	addr_bf_1_0   = ma >> 6;


	addr_bf = 0;
	addr_bf = ( addr_bf_12_10 << 10 ) | ( addr_bf_1_0 );

	switch( rank ) {
	case 0:
	case 1:
		__REG32( (u64)&__DREX[ch]->DIRECTCMD ) = ( DREX_MRR_COMMAND << 24 ) | ( rank << 20 ) | ( bank_bf << 16 ) | addr_bf;
		break;

	default:
		__REG32( (u64)&__DREX[ch]->DIRECTCMD ) = ( DREX_MRR_COMMAND << 24 ) | ( 1 << 21 ) | ( bank_bf << 16 ) | addr_bf;
	}

	return;
}

/******************************************************************************
 *
 * @fn
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static void dmc_pre_command(unsigned char ch, unsigned char rank, signed char bank)
{
	if( bank < 0 ) {
	} else {
	}
}


/******************************************************************************
 *
 * @fn
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static void dmc_ref_command(unsigned char ch, unsigned char rank, signed char bank)
{
	if( bank < 0 ) {
	} else {
	}
}

/******************************************************************************
 *
 * @fn
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static void dmc_sre_command(unsigned char ch, unsigned char rank)
{
	__REG32( (u64)&__DREX[ch]->DIRECTCMD )  = ( DREX_SRE_COMMAND << 24 ) | ( rank << 20 );
}

/******************************************************************************
 *
 * @fn
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static void dmc_srx_command(unsigned char ch, unsigned char rank)
{
	__REG32( (u64)&__DREX[ch]->DIRECTCMD )  = ( DREX_SRX_COMMAND << 24 ) | ( rank << 20 );

	return;
}

/******************************************************************************
 *
 * @fn
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static void dmc_pd_command(unsigned char ch, unsigned char rank)
{
}

/******************************************************************************
 *
 * @fn
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static void dmc_mpc_command(int ch, int rank, unsigned char op)
{
	switch( rank ) {

		case 0:
		case 1:
			__REG32( (u64)&__DREX[ch]->DIRECTCMD ) = ( DREX_MPC_COMMAND << 24 ) | ( rank << 20 ) | ( op << 2 );
			break;

		default:
			__REG32( (u64)&__DREX[ch]->DIRECTCMD )  = ( DREX_MPC_COMMAND << 24 ) | ( 1 << 21 ) | ( op << 2 );
	}

}

/******************************************************************************
 *
 * @fn
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static void dmc_pde_command(int ch, int rank)
{
	switch( rank ) {

		case 0:
		case 1:
			__REG32( (u64)&__DREX[ch]->DIRECTCMD )  = ( DREX_PDE_COMMAND << 24 ) | ( rank << 20 );
			break;

		default:
			__REG32( (u64)&__DREX[ch]->DIRECTCMD )  = ( DREX_PDE_COMMAND << 24 ) | ( 1 << 21 );

	}
}

/******************************************************************************
 *
 * @fn
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static void dmc_pdx_command(int ch, int rank)
{
	switch( rank ) {

		case 0:
		case 1:
			__REG32( (u64)&__DREX[ch]->DIRECTCMD )  = ( DREX_PDX_COMMAND << 24 ) | ( rank << 20 );
			break;

		default:
			__REG32( (u64)&__DREX[ch]->DIRECTCMD )  = ( DREX_PDX_COMMAND << 24 ) | ( 1 << 21 );
	}

}

/******************************************************************************
 *
 * @fn
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static void dmc_act_command(unsigned char ch, unsigned char rank)
{
	switch( rank ) {

		case 0:
		case 1:
			__REG32( (u64)&__DREX[ch]->DIRECTCMD ) = ( DREX_ACT_COMMAND << 24 ) | ( rank << 20 );
			break;

		default:
			__REG32( (u64)&__DREX[ch]->DIRECTCMD ) = ( DREX_ACT_COMMAND << 24 ) | ( 1 << 21 );
	}

}

#if 0
/******************************************************************************
 *
 * @fn
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static unsigned char dmc_send_command(int command, ...)
{
	int argc;
	va_list argptr;
	int ch, channels, rank;
	unsigned char ma, op;

	unsigned char bank_bf;
	unsigned short addr_bf;
	unsigned char addr_bf_1_0;
	unsigned char addr_bf_12_10;
	unsigned char addr_bf_9_2;

	va_start(argptr, command);

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

	if( rank == DMC_RANK_ALL ) {
		rank = 0x2;
	}

	__REG32( (u32)&__DREX[ch]->DIRECTCMD ) = ( command << 24 ) | ( rank << 20 ) | ( bank_bf << 16 ) | addr_bf;
}
#endif


/******************************************************************************
 *
 * @fn
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static void dmc_cal_preset(int ch)
{
	__PHY[ch]->CAL_CON0.gate_cal_mode = 0;
	__PHY[ch]->CAL_CON2.ctrl_shgate = 0;
	__PHY[ch]->CAL_CON2.ctrl_gateadj = 0;
	__PHY[ch]->CAL_CON2.ctrl_gateduradj = 4;
	__PHY[ch]->CAL_CON2.ctrl_readadj= 0;
	__PHY[ch]->CAL_CON2.ctrl_readduradj= 7;

	__PHY[ch]->OFFSETC_CON0.ctrl_offsetc0 = 0;
	__PHY[ch]->OFFSETC_CON0.ctrl_offsetc1 = 0;
	__PHY[ch]->SHIFTC_CON0.ctrl_shiftc0 = 0;
	__PHY[ch]->SHIFTC_CON0.ctrl_shiftc1 = 0;

	__PHY[ch]->LP_CON0.ctrl_pulld_dqs = 0x0;

	//__REG32( (u32)&__DREX[ch]->TRAIN_TIMING0 ) = 0x319ba10a;
	//__REG32( (u32)&__DREX[ch]->TRAIN_TIMING1 ) = 0x319ba10a;

	return;
}

/******************************************************************************
 *
 * @fn
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static void dmc_cal_postset(int ch)
{
	__PHY[ch]->CAL_CON1.rdlvl_periodic_incr_adj = 0x40;
	__PHY[ch]->CAL_CON1.glvl_periodic_incr_adj = 0x40;
	__PHY[ch]->CAL_CON1.rdlvl_pass_adj = 1;
	__PHY[ch]->CAL_CON1.rdlvl_incr_adj = 0x40;
	__PHY[ch]->CAL_CON1.rdlvl_start_adj = 0; //0x3;

	return;
}

/******************************************************************************
 *
 * @fn
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static int dmc_cal_pattern(int ch, unsigned char upper, unsigned char lower)
{
	__PHY[ch]->CAL_PATTERN_CON3.rdtrn_rddata_adj_w0 = lower;
	__PHY[ch]->CAL_PATTERN_CON3.rdtrn_rddata_adj_w1 = upper;

	return 0;
}

void dmc_cal_wr_pattern(int ch, lp4_density_t density)
{
	if (LP4_8Gb_Die == density) {
		 __DREX[ch]->WRTRA_PATTERN0.wrtra_wrdata_p0= 0xEE11;
		 __DREX[ch]->WRTRA_PATTERN0.wrtra_wrdata_p1= 0xDD22;
		 __DREX[ch]->WRTRA_PATTERN1.wrtra_wrdata_p2= 0xBB44;
		 __DREX[ch]->WRTRA_PATTERN1.wrtra_wrdata_p3= 0x7788;
		 __DREX[ch]->WRTRA_PATTERN2.wrtra_wrdata_dm_p0= 0x2;
		 __DREX[ch]->WRTRA_PATTERN2.wrtra_wrdata_dm_p1= 0x2;
		 __DREX[ch]->WRTRA_PATTERN2.wrtra_wrdata_dm_p2= 0x2;
		 __DREX[ch]->WRTRA_PATTERN2.wrtra_wrdata_dm_p3= 0x2;
		 __PHY[ch]->CAL_PATTERN_CON0.wrtrn_rddata_adj_w0= 0xEE11;
		 __PHY[ch]->CAL_PATTERN_CON0.wrtrn_rddata_adj_w1= 0xDD22;
		 __PHY[ch]->CAL_PATTERN_CON1.wrtrn_rddata_adj_w2= 0xBB44;
		 __PHY[ch]->CAL_PATTERN_CON1.wrtrn_rddata_adj_w3= 0x7788;
		 __PHY[ch]->CAL_PATTERN_CON2.wrtrn_rddata_dm_adj_w0= 0x2;
		 __PHY[ch]->CAL_PATTERN_CON2.wrtrn_rddata_dm_adj_w1= 0x2;
		 __PHY[ch]->CAL_PATTERN_CON2.wrtrn_rddata_dm_adj_w2= 0x2;
		 __PHY[ch]->CAL_PATTERN_CON2.wrtrn_rddata_dm_adj_w3= 0x2;
	} else {
		__DREX[ch]->WRTRA_PATTERN0.wrtra_wrdata_p0= 0x00FF;
		__DREX[ch]->WRTRA_PATTERN0.wrtra_wrdata_p1= 0x00FF;
		__DREX[ch]->WRTRA_PATTERN1.wrtra_wrdata_p2= 0x55AA;
		__DREX[ch]->WRTRA_PATTERN1.wrtra_wrdata_p3= 0x55AA;
		__DREX[ch]->WRTRA_PATTERN2.wrtra_wrdata_dm_p0= 0x1;
		__DREX[ch]->WRTRA_PATTERN2.wrtra_wrdata_dm_p1= 0x1;
		__DREX[ch]->WRTRA_PATTERN2.wrtra_wrdata_dm_p2= 0x1;
		__DREX[ch]->WRTRA_PATTERN2.wrtra_wrdata_dm_p3= 0x1;
		__PHY[ch]->CAL_PATTERN_CON0.wrtrn_rddata_adj_w0= 0x00FF;
		__PHY[ch]->CAL_PATTERN_CON0.wrtrn_rddata_adj_w1= 0x00FF;
		__PHY[ch]->CAL_PATTERN_CON1.wrtrn_rddata_adj_w2= 0x55AA;
		__PHY[ch]->CAL_PATTERN_CON1.wrtrn_rddata_adj_w3= 0x55AA;
		__PHY[ch]->CAL_PATTERN_CON2.wrtrn_rddata_dm_adj_w0= 0x1;
		__PHY[ch]->CAL_PATTERN_CON2.wrtrn_rddata_dm_adj_w1= 0x1;
		__PHY[ch]->CAL_PATTERN_CON2.wrtrn_rddata_dm_adj_w2= 0x1;
		__PHY[ch]->CAL_PATTERN_CON2.wrtrn_rddata_dm_adj_w3= 0x1;
	}
}

/******************************************************************************
 *
 * @fn
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static int dmc_cal_status(int ch, int where)
{
	int status = 0;

	switch( where ) {

	case 0:
		status = __REG32( &__PHY[ch]->CAL_FAIL_STAT0 ) & 0xff;
		break;

	case 1:
		status = (__REG32( &__PHY[ch]->CAL_FAIL_STAT0 ) >> 8 ) & 0xff;
		break;

	}

	return ( status );
}

/******************************************************************************
 *
 * @fn
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static void dmc_hw_periodic_train(int ch, int periodic_en, int retrain_en, int num_rank)
{
	if (periodic_en) {
		/* gate */
		__DREX[ch]->HW_PTRAIN_PERIOD0.t_ptrain_period = 0x00005dc0;
		__DREX[ch]->HW_PTRAIN_PERIOD1.t_ptrain_period = 0x00005dc0;
		__DREX[ch]->HWPTRAINCONFIG0.periodic_gate = 1;
		__DREX[ch]->HWPTRAINCONFIG1.periodic_gate = 1;

		if(2 == num_rank) {
			__DREX[ch]->HWPTRAINCONFIG0.periodic_gate_all_rank = 1;
			__DREX[ch]->HWPTRAINCONFIG1.periodic_gate_all_rank = 1;
		}
		/*
		__DREX[ch]->HW_PTRAIN_PERIOD0.t_ptrain_period = 0x0003FFFF;
		__DREX[ch]->HW_PTRAIN_PERIOD1.t_ptrain_period = 0x0003FFFF;
		__DREX[ch]->HWPTRAINCONFIG0.periodic_write = 1;
		__DREX[ch]->HWPTRAINCONFIG0.periodic_write_all_rank= 1;
		__DREX[ch]->HWPTRAINCONFIG1.periodic_write = 1;
		__DREX[ch]->HWPTRAINCONFIG1.periodic_write_all_rank = 1;
		*/
	}

	if (retrain_en) {
		/* gate re-train */
		/*
		__DREX[ch]->HWPTRAINCONFIG0.retrain_gate = 1;
		__DREX[ch]->HWPTRAINCONFIG0.retrain_gate_all_rank = 1;
		__DREX[ch]->HWPTRAINCONFIG1.retrain_gate = 1;
		__DREX[ch]->HWPTRAINCONFIG1.retrain_gate_all_rank = 1;
		*/
		__DREX[ch]->HWPTRAINCONFIG0.retrain_write= 1;
		__DREX[ch]->HWPTRAINCONFIG1.retrain_write = 1;
		if(2 == num_rank) {
			__DREX[ch]->HWPTRAINCONFIG0.retrain_write_all_rank = 1;
			__DREX[ch]->HWPTRAINCONFIG1.retrain_write_all_rank = 1;
		}
	}

	if (periodic_en) {
		__DREX[ch]->HWPTRAINCONTROL0.hw_periodic_train_en = 1;
		__DREX[ch]->HWPTRAINCONTROL1.hw_periodic_train_en = 1;
	}

	if (retrain_en) {
		__DREX[ch]->HWPTRAINCONTROL0.retrain_en= 1;
		__DREX[ch]->HWPTRAINCONTROL1.retrain_en = 1;
	}

	return;
}

/******************************************************************************
 *
 * @fn
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
void dmc_sel_drex_timing_set(int ch, int set)
{
	__DREX[ch]->TIMINGSETSW.timing_set_sw=set;
	__DREX[ch]->TIMINGSETSW.timing_set_sw_con=1;

	return;
}

/******************************************************************************
 *
 * @fn
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
#if (DMC_DQS_TYPE == DMC_DQS_MODE1)
static int dmc_gate_training(int ch, int rank)
{
	unsigned char status;

	/* preset for gate train */
	__PHY[ch]->CAL_CON1.rdlvl_periodic_incr_adj=0x40;
	__PHY[ch]->CAL_CON1.glvl_periodic_incr_adj=0x40;
	__PHY[ch]->CAL_CON1.rdlvl_pass_adj=4;
	__PHY[ch]->CAL_CON1.rdlvl_incr_adj=1;
	__PHY[ch]->CAL_CON1.rdlvl_start_adj=0;

	__PHY[ch]->CAL_CON2.ctrl_readadj=0;
	__PHY[ch]->CAL_CON2.ctrl_readduradj=6;

	__PHY[ch]->CAL_CON2.ctrl_gateadj=0;
	__PHY[ch]->CAL_CON2.ctrl_rodt_disable=0;


	__PHY[ch]->CAL_CON2.ctrl_shgate = 1;

	if( __PHY[ch]->CAL_CON2.ctrl_rpre_opt == 0 ) {
		__PHY[ch]->CAL_CON2.ctrl_gateduradj = 0xb;
	} else {
		__PHY[ch]->CAL_CON2.ctrl_gateduradj = 0xa;
	}
#if 0
	__PHY[ch]->OFFSETC_CON0.ctrl_offsetc0 = 0;
	__PHY[ch]->OFFSETC_CON0.ctrl_offsetc1 = 0;
	__PHY[ch]->SHIFTC_CON0.ctrl_shiftc0 = 0;
	__PHY[ch]->SHIFTC_CON0.ctrl_shiftc1 = 0;

	__PHY[ch]->LP_CON0.ctrl_pulld_dqs = 0xf;
#else
	__PHY[ch]->LP_CON0.ctrl_pulld_dqs = 0x0;
#endif
	__PHY[ch]->CAL_CON0.gate_cal_mode = 1;

	switch( rank ) {

		case 0:
			__DREX[ch]->INITTRAINCONFIG.init_gate_train_chip1 = 0;
			__DREX[ch]->INITTRAINCONFIG.init_gate_train_chip0 = 1;
			break;

		case 1:
			__DREX[ch]->INITTRAINCONFIG.init_gate_train_chip1 = 1;
			__DREX[ch]->INITTRAINCONFIG.init_gate_train_chip0 = 0;
			break;

		default:
			__DREX[ch]->INITTRAINCONFIG.init_gate_train_chip1 = 1;
			__DREX[ch]->INITTRAINCONFIG.init_gate_train_chip0 = 1;
	}

	__DREX[ch]->INITTRAINCONTROL.init_train_start = 1;

	while( __DREX[ch]->PHYSTATUS.training_complete != 1 );

	__DREX[ch]->INITTRAINCONFIG.init_gate_train_chip0 = 0;
	__DREX[ch]->INITTRAINCONFIG.init_gate_train_chip1 = 0;
	__DREX[ch]->INITTRAINCONTROL.init_train_start = 0;

	return 0;
}
#endif

/******************************************************************************
 *
 * @fn		dmc_read_dq_cal
 *
 * @brief	read DQ calibration
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static int dmc_read_dq_cal(int ch, int rank)
{
	int i;

	__PHY[ch]->CAL_CON0.fine_train_en = 0;

	__PHY[ch]->CAL_CON0.rd_cal_mode = 1;

	__DREX[ch]->INITTRAINCONFIG.init_read_train_chip0 = 1;
	__DREX[ch]->INITTRAINCONTROL.init_train_start = 1;

	do {

	} while( __DREX[ch]->PHYSTATUS.training_complete != 1 );

//	__DREX[ch]->INITTRAINCONFIG.init_read_train_chip0 = 0;
	__DREX[ch]->INITTRAINCONTROL.init_train_start = 0;

	return( __REG32( &__PHY[ch]->CAL_FAIL_STAT0 ) & 0xffff );
}

int dmc_read_dq_cal_fine_train(int ch, int rank)
{
	int i;

	__PHY[ch]->CAL_CON0.fine_train_en = 1;

	__PHY[ch]->CAL_CON0.rd_cal_mode = 1;

	__DREX[ch]->INITTRAINCONFIG.init_read_train_chip0 = 1;
	__DREX[ch]->INITTRAINCONTROL.init_train_start = 1;

	do {

	} while( __DREX[ch]->PHYSTATUS.training_complete != 1 );

	__DREX[ch]->INITTRAINCONFIG.init_read_train_chip0 = 0;
	__DREX[ch]->INITTRAINCONTROL.init_train_start = 0;

	return( __REG32( &__PHY[ch]->CAL_FAIL_STAT0 ) & 0xffff );
}

/******************************************************************************
 *
 * @fn
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static int dmc_write_dq_cal(int ch, int rank)
{
	int ret = 1;

	__PHY[ch]->CAL_CON0.fine_train_en = 0;

	__PHY[ch]->CAL_CON0.wr_cal_mode = 1;

	switch( rank ) {

	case 0:
		__DREX[ch]->INITTRAINCONFIG.init_write_train_chip0 = 1;
		break;

	case 1:
		__DREX[ch]->INITTRAINCONFIG.init_write_train_chip1 = 1;
		break;

	default:
		__DREX[ch]->INITTRAINCONFIG.init_write_train_chip0 = 1;
		__DREX[ch]->INITTRAINCONFIG.init_write_train_chip1 = 1;
	}


	__DREX[ch]->INITTRAINCONTROL.init_train_start = 1;

	do {


	} while( __DREX[ch]->PHYSTATUS.training_complete != 1 );


	switch( rank ) {

	case 0:
		__DREX[ch]->INITTRAINCONFIG.init_write_train_chip0 = 0;
		break;

	case 1:
		__DREX[ch]->INITTRAINCONFIG.init_write_train_chip1 = 0;
		break;

	default:
//		__DREX[ch]->INITTRAINCONFIG.init_write_train_chip0 = 0;
//		__DREX[ch]->INITTRAINCONFIG.init_write_train_chip1 = 0;
		break;
	}

	__DREX[ch]->INITTRAINCONTROL.init_train_start = 0;

	return( __REG32( &__PHY[ch]->CAL_FAIL_STAT0 ) & 0xff );
}

int dmc_write_dq_cal_fine(int ch, int rank)
{
	int ret = 1;

	__PHY[ch]->CAL_CON0.fine_train_en = 1;

	__PHY[ch]->CAL_CON0.wr_cal_mode = 1;

	switch( rank ) {

	case 0:
		__DREX[ch]->INITTRAINCONFIG.init_write_train_chip0 = 1;
		break;

	case 1:
		__DREX[ch]->INITTRAINCONFIG.init_write_train_chip1 = 1;
		break;

	default:
		__DREX[ch]->INITTRAINCONFIG.init_write_train_chip0 = 1;
		__DREX[ch]->INITTRAINCONFIG.init_write_train_chip1 = 1;
	}

	__DREX[ch]->INITTRAINCONTROL.init_train_start = 1;

	do {

	} while( __DREX[ch]->PHYSTATUS.training_complete != 1 );

	switch( rank ) {

	case 0:
		__DREX[ch]->INITTRAINCONFIG.init_write_train_chip0 = 0;
		break;

	case 1:
		__DREX[ch]->INITTRAINCONFIG.init_write_train_chip1 = 0;
		break;

	default:
//		__DREX[ch]->INITTRAINCONFIG.init_write_train_chip0 = 0;
//		__DREX[ch]->INITTRAINCONFIG.init_write_train_chip1 = 0;
		break;
	}

	__DREX[ch]->INITTRAINCONTROL.init_train_start = 0;

	return( __REG32( &__PHY[ch]->CAL_FAIL_STAT0 ) & 0xff );
}

/******************************************************************************
 *
 * @fn
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
#if 0
static void dmc_command_bus_training(int ch, int(*clock)(int *), int freq)
{
	int offset;
	int i;
	int rank;
	unsigned char cs_train;
	unsigned char read_back;

	unsigned char where;
	unsigned short lock_value;

	struct cbt {
		unsigned char left;
		unsigned char right;
		unsigned char center;
	};

	struct cbt cs[2], ca;

	// NOTE: configure PHY in command bus training
	__PHY[ch]->CAL_CON0.wrlvl_mode = 1;

	__PHY[ch]->CAL_CON0.ca_cal_mode = 1;

	// NOTE: configure CA Vref setting mode -

	__PHY[ch]->CBT_CON0.cbt_ca_vref_mode_ds_0 = 1;
	__PHY[ch]->CBT_CON0.cbt_ca_vref = 0x4d;

	__PHY[ch]->CBT_CON0.cbt_cmd_ca_vref = 1;
	dmc_wait(1, 1);
	__PHY[ch]->CBT_CON0.cbt_cmd_ca_vref = 0;

	// FIXME: Wait tVREF_LONG
	dmc_wait( ch, 200 * nsec );

#if 0
	rank = 0;
	do {

		// NOTE: CS train
		// NOTE: k
		where = 0;
		offset = 0;
		do {

			if( rank == 0 ) {
				__PHY[ch]->CA_DSKEW_CON1.cs0deskewcode = offset;
			} else if( rank == 1 ) {
				//__PHY[ch]->CA_DSKEW_CON1.cs1deskewcode = offset;
			}

			dmc_dll_resync( ch );

			if( where == 0 ) {

				__PHY[ch]->CBT_CON0.cbt_cmd_cs_vwml = 1;
				dmc_wait(1, 1);
				__PHY[ch]->CBT_CON0.cbt_cmd_cs_vwml = 0;

				//if( __PHY[ch]->DQ_IO_RDATA0.dq_io_rd1 != 0x3f ) {
				if( __PHY[ch]->DQ_IO_RDATA0.dq_io_rd0 != 0x3f ) {

					cs[rank].left = offset - 1;		// NOTE:
					where++;
				} else {
					offset++;
				}

			} else {

				__PHY[ch]->CBT_CON0.cbt_cmd_cs_vwmr = 1;
				dmc_wait(1, 1);
				__PHY[ch]->CBT_CON0.cbt_cmd_cs_vwmr = 0;

				//if( __PHY[ch]->DQ_IO_RDATA0.dq_io_rd1 == 0x3f ) {
				if( __PHY[ch]->DQ_IO_RDATA0.dq_io_rd0 == 0x3f ) {

					cs[rank].right = offset - 1;		// NOTE:
					where++;
				} else {
					offset++;
				}
			}

			// NOTE: clear
			__PHY[ch]->CBT_CON0.cbt_cmd_clear = 1;
			dmc_wait(1, 1);
			__PHY[ch]->CBT_CON0.cbt_cmd_clear = 0;

		} while( where < 2 ) ;

		cs[rank].center = ( cs[rank].right + cs[rank].left ) >> 1;
		lock_value = __PHY[ch]->MDLL_CON1.ctrl_lock_value_init;

		if( cs[rank].center > ( lock_value >> 1 ) ) {
			cs[rank].center -= ( lock_value >> 1 );
		}
		dmc_dll_resync( ch );

		rank++;

	} while( rank < 2 );
#endif

#if 1
	//- 2 rank
	// NOTE: CA train
	// NOTE: fjdio

		dmc_memset( &ca, 0, sizeof(ca) );
		offset = i = 0;
		where = 0;
		do {

			__PHY[ch]->OFFSETD_CON0.ctrl_offsetd = offset;

			dmc_dll_resync( ch );

			if( where == 0 ) {

				__PHY[ch]->CBT_CON0.cbt_cmd_ca_vwml = 1;
				dmc_wait(1, 1);
				__PHY[ch]->CBT_CON0.cbt_cmd_ca_vwml = 0;

				if( ( ch == 1 ) || ( ch == 2 ) ) {
					read_back = __PHY[ch]->DQ_IO_RDATA0.dq_io_rd0;
				} else {
					read_back = __PHY[ch]->DQ_IO_RDATA0.dq_io_rd1;
				}

				if( read_back != 0x3f ) {
				//if( __PHY[ch]->DQ_IO_RDATA0.dq_io_rd0 != 0x3f ) {
					ca.left = offset - 1;		// NOTE:
					where++;
					offset = 0;
				} else {
					offset++;
				}

			} else {

				__PHY[ch]->CBT_CON0.cbt_cmd_ca_vwmr = 1;
				dmc_wait(1, 1);
				__PHY[ch]->CBT_CON0.cbt_cmd_ca_vwmr = 0;

				if( ( ch == 1 ) || ( ch == 2 ) ) {
					read_back = __PHY[ch]->DQ_IO_RDATA0.dq_io_rd0;
				} else {
					read_back = __PHY[ch]->DQ_IO_RDATA0.dq_io_rd1;
				}

				if( read_back == 0x3f ) {
				//if( __PHY[ch]->DQ_IO_RDATA0.dq_io_rd0 == 0x3f ) {
					ca.right = offset - 1;		// NOTE:
					where++;
				} else {
					offset++;
				}
			}

			__PHY[ch]->CBT_CON0.cbt_cmd_clear = 1;
			dmc_wait(1, 1);
			__PHY[ch]->CBT_CON0.cbt_cmd_clear = 0;

		} while( where < 2 );

		ca.center = ( ca.right + ca.left ) >> 1;
		lock_value = __PHY[ch]->MDLL_CON1.ctrl_lock_value_init;

		if( ca.center > ( lock_value >> 1 ) ) {
			ca.center -= ( lock_value >> 1 );
		} else {
			ca.center = 0;
		}

		__PHY[ch]->OFFSETD_CON0.ctrl_offsetd = ca.center;
		dmc_dll_resync( ch );
#endif
	__PHY[ch]->CAL_CON0.wrlvl_mode = 0;
	__PHY[ch]->CBT_CON0.cbt_ca_vref_mode_ds_0 = 0;

	return;
}
#endif

/******************************************************************************
 *
 * @fn		dmc_preset
 *
 * @brief	prepare DRAM controller & phy before initialization.
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static void dmc_preset(int ch)
{
#if defined(CONFIG_SMC_CMD)
	reg_arr_t reg_arr;
#endif
	unsigned int cal_get_mif;
	unsigned char soc_vref;

	soc_vref = (get_vref_fuse_bits() & 0x0000003F);
	if (soc_vref == 0)		// fused nothing
		soc_vref = 0x16;	// 23%

	//- channel hash off
	/* __REG32( 0x10050720 ) = 0x00000004;
	*/

	#if 0 /* EVT0 */
	if( ( ch == 1 ) || ( ch == 2 ) ) {
		__DREX[ch]->CONCONTROL.ca_swap = 1;
		__PHY[ch]->CAL_CON0.ca_swap_mode = 1;
		__DREX[ch]->MEMCONTROL.mrr_byte = 1;
	} else {
		__DREX[ch]->CONCONTROL.ca_swap = 0;
		__PHY[ch]->CAL_CON0.ca_swap_mode = 0;
		__DREX[ch]->MEMCONTROL.mrr_byte = 0;
	}
	#else /* EVT1 */
	//__DREX[ch]->CONCONTROL.ca_swap = 0;
	//__PHY[ch]->CAL_CON0.ca_swap_mode = 0;
	//__DREX[ch]->MEMCONTROL.mrr_byte = 0;
	#endif

	__DREX[ch]->CONCONTROL.dfi_init_start = 0;
	__DREX[ch]->CONCONTROL.timeout_level0 = 0xfff;
	__DREX[ch]->CONCONTROL.same_dir_prep = 0;
	__DREX[ch]->CONCONTROL.io_pd_con = 0;
	__DREX[ch]->CONCONTROL.aref_en = 0;

	__DREX[ch]->MEMCONTROL.sp_en = 0;

#if MC_INITIATED_SDLL_UPDATE
	__DREX[ch]->MEMCONTROL.pb_ref_en = 0;
#else
	__DREX[ch]->MEMCONTROL.pb_ref_en = 1;
#endif
	__DREX[ch]->MEMCONTROL.dbi_en = 1;
	__DREX[ch]->MEMCONTROL.bl = 4;
	__DREX[ch]->MEMCONTROL.num_chunk = 0;
	__DREX[ch]->MEMCONTROL.num_chip = 1;
	__DREX[ch]->MEMCONTROL.mem_width = 1;
	__DREX[ch]->MEMCONTROL.mem_type = 8;
	__DREX[ch]->MEMCONTROL.add_lat_pall = 1;
	__DREX[ch]->MEMCONTROL.dsref_en = 0;
//	__DREX[ch]->MEMCONTROL.dpwrdn_type = 0;
	__DREX[ch]->MEMCONTROL.dpwrdn_en = 0;
	__DREX[ch]->MEMCONTROL.clk_stop_en = 0;

#if defined(CONFIG_SMC_CMD)
	reg_arr.set0.addr = ((u32)&__ASP[ch]->MEMBASECONFIG0);
	reg_arr.set0.val = 0x00010004;
	reg_arr.set1.addr = ((u32)&__ASP[ch]->MEMCONFIG0);
	reg_arr.set1.val = 0x01302333;
	reg_arr.set2.addr = ((u32)&__ASP[ch]->MEMCONFIG1);
	reg_arr.set2.val = 0x01302333;
	reg_arr.set3.addr = ((u32)&__ASP[ch]->MEMSIZE0);
	reg_arr.set3.val = 0x00000001;
	set_secure_reg((u64)&reg_arr, 4);
#else
	__REG32( (u64)&__ASP[ch]->MEMBASECONFIG0 ) = 0x00010004;
	__REG32( (u64)&__ASP[ch]->MEMCONFIG0 ) = 0x01302333;
	__REG32( (u64)&__ASP[ch]->MEMCONFIG1 ) = 0x01302333;
	__REG32( (u64)&__ASP[ch]->MEMSIZE0 ) = 0x00000001;
#endif
	__REG32( (u64)&__DREX[ch]->CGCONTROL ) = 0;

	#if 0 //(DMC_IO_TYPE == DMC_CMOS_IO)
	__PHY[ch]->GNR_CON0.ctrl_cmosrcv = 1; //1;
	#elif 1 //(DMC_IO_TYPE == DMC_DIFF_IO)
	__PHY[ch]->GNR_CON0.ctrl_cmosrcv = 0; //1;
	#else
	;
	#endif

	#if (DMC_DQS_TYPE == DMC_DQS_MODE1)
	__PHY[ch]->GNR_CON0.ctrl_dfdqs = 1;
	#elif (DMC_DQS_TYPE == DMC_DQS_MODE0)
	__PHY[ch]->GNR_CON0.ctrl_dfdqs = 0;

	__PHY[ch]->CAL_CON2.ctrl_shgate = 0;
	__PHY[ch]->LP_CON0.ctrl_pulld_dqs = 0x0;
	#else
	;
	#endif

	__PHY[ch]->GNR_CON0.ctrl_twpre = 0; //1;
	__PHY[ch]->CAL_CON2.ctrl_rpre_opt =0;

	__DREX[ch]->ZQTIMING.t_zqcs = 0x20;
	__DREX[ch]->ZQTIMING.t_zqoper = 0x80;
	__DREX[ch]->ZQTIMING.t_zqinit = 0x100;
	__DREX[ch]->ZQTIMING.t_zqlat = 0x4;

#if DRAM_REF_INTVL_2X
	__DREX[ch]->TIMINGAREF.t_refi = 0xBA;
	__DREX[ch]->TIMINGAREF.t_refipb = 24;
#else
	__DREX[ch]->TIMINGAREF.t_refi = 0x17;
	__DREX[ch]->TIMINGAREF.t_refipb = 3;	// 11->12, because x1/4 should be 3.
#endif

	__DREX[ch]->ETCTIMING.t_mrr = 0x3;
	__DREX[ch]->ETCTIMING.t_srr = 0x2;
	__DREX[ch]->ETCTIMING.t_src = 0x7;

	cal_get_mif = _cal_get_mif();

	if (MEMCLK_1552 == cal_get_mif) { /* 1552 */
		__REG32( (u64)&__DREX[ch]->TIMINGROW1 ) = 0x6D588652; //0x3436648d;
		__REG32( (u64)&__DREX[ch]->TIMINGDATA1 ) = 0x4732BBE0; // 0x3652b3dc;
		__REG32( (u64)&__DREX[ch]->TIMINGPOWER1 ) = 0x40710336; //0x30370335;

		__REG32( (u64)&__DREX[ch]->TIMINGROW0 ) = 0x6D588652; //0x3436648d;
		__REG32( (u64)&__DREX[ch]->TIMINGDATA0 ) = 0x4732BBE0; //0x3652b3dc;
		__REG32( (u64)&__DREX[ch]->TIMINGPOWER0 ) = 0x40710336; //0x30370335;

		__REG32( (u64)&__DREX[ch]->RDFETCH0 ) = 0x4E03; //0x3a02;
		__REG32( (u64)&__DREX[ch]->RDFETCH1 ) = 0x4E03; //0x3a02;

		__REG32( (u64)&__DREX[ch]->TRAIN_TIMING0 ) = 0x319ba14a;//0x319ba1f5;
		__REG32( (u64)&__DREX[ch]->TRAIN_TIMING1 ) = 0x319ba14a;//0x319ba1f5;

		// For power mode support, Recovery Time From PHY Clock Gate = 4
		__DREX[ch]->PHYCONTROL0.rec_gate_cyc = 4;

		__DREX[ch]->TIMINGRFCPB.t_rfcpb0 = 0x37;
		__DREX[ch]->TIMINGRFCPB.t_rfcpb1 = 0x37;
	} else if (MEMCLK_1456 == cal_get_mif) { /* 1456 */
		__REG32( (u64)&__DREX[ch]->TIMINGROW1 ) = 0x66588611;
		__REG32( (u64)&__DREX[ch]->TIMINGDATA1 ) = 0x4732BBE0;
		__REG32( (u64)&__DREX[ch]->TIMINGPOWER1 ) = 0x3C6A0336;

		__REG32( (u64)&__DREX[ch]->TIMINGROW0 ) = 0x66588611;
		__REG32( (u64)&__DREX[ch]->TIMINGDATA0 ) = 0x4732BBE0;
		__REG32( (u64)&__DREX[ch]->TIMINGPOWER0 ) = 0x3C6A0336;

		__REG32( (u64)&__DREX[ch]->RDFETCH0 ) = 0x4903;
		__REG32( (u64)&__DREX[ch]->RDFETCH1 ) = 0x4903;

		__REG32( (u64)&__DREX[ch]->TRAIN_TIMING0 ) = 0x319ba14a;
		__REG32( (u64)&__DREX[ch]->TRAIN_TIMING1 ) = 0x319ba14a;

		// For power mode support, Recovery Time From PHY Clock Gate = 4
		__DREX[ch]->PHYCONTROL0.rec_gate_cyc = 4;

		__DREX[ch]->TIMINGRFCPB.t_rfcpb0 = 0x33;
		__DREX[ch]->TIMINGRFCPB.t_rfcpb1 = 0x33;
	} else if (MEMCLK_1264 == cal_get_mif) { /* 1264 */
		__REG32( (u64)&__DREX[ch]->TIMINGROW1 ) = 0x5947754F;
		__REG32( (u64)&__DREX[ch]->TIMINGDATA1 ) = 0x4632B3DC;
		__REG32( (u64)&__DREX[ch]->TIMINGPOWER1 ) = 0x345C0335;

		__REG32( (u64)&__DREX[ch]->TIMINGROW0 ) = 0x5947754F;
		__REG32( (u64)&__DREX[ch]->TIMINGDATA0 ) = 0x4632B3DC;
		__REG32( (u64)&__DREX[ch]->TIMINGPOWER0 ) = 0x345C0335;

		__REG32( (u64)&__DREX[ch]->RDFETCH0 ) = 0x4003;
		__REG32( (u64)&__DREX[ch]->RDFETCH1 ) = 0x4003;

		__REG32( (u64)&__DREX[ch]->TRAIN_TIMING0 ) = 0x319BA13A;
		__REG32( (u64)&__DREX[ch]->TRAIN_TIMING1 ) = 0x319BA13A;

		// For power mode support, Recovery Time From PHY Clock Gate = 4
		__DREX[ch]->PHYCONTROL0.rec_gate_cyc = 4;

		__DREX[ch]->TIMINGRFCPB.t_rfcpb0 = 0x2D;
		__DREX[ch]->TIMINGRFCPB.t_rfcpb1 = 0x2D;
	}

#if MC_INITIATED_SDLL_UPDATE
	__PHY[ch]->GNR_CON0.ctrl_upd_range= 0x0;
	__PHY[ch]->GNR_CON0.ctrl_upd_time= 0x0;
#else
	__PHY[ch]->GNR_CON0.ctrl_upd_range= 0x1;
	__PHY[ch]->GNR_CON0.ctrl_upd_time= 0x1;
#endif

	__PHY[ch]->CAL_CON2.ctrl_readadj = 0;
	__PHY[ch]->CAL_CON2.ctrl_readduradj = 3;
	__PHY[ch]->CAL_CON2.ctrl_rodt_disable = 0; //1;

       __PHY[ch]->ZQ_CON1.zq_vref = 0x04; //1; 30.9%

       __PHY[ch]->ZQ_CON0.zq_mode_term = 1; //1; 240ohm
	__PHY[ch]->ZQ_CON0.zq_mode_noterm = 0; //1;
	__PHY[ch]->ZQ_CON6.zq_ds1_term = 2; //1;	120ohm
	__PHY[ch]->ZQ_CON6.zq_ds1_noterm = 0;
	__PHY[ch]->ZQ_CON6.zq_ds0_term = 2; //1;	120ohm
	__PHY[ch]->ZQ_CON6.zq_ds0_noterm = 0;

	__PHY[ch]->ZQ_CON9.zq_ds0_vref = soc_vref;
	__PHY[ch]->ZQ_CON9.zq_ds1_vref = soc_vref;

	__DREX[ch]->HWPTRAINCONFIG0.retrain_gate = 0;
	__DREX[ch]->HWPTRAINCONFIG0.retrain_gate_all_rank = 0;
	__DREX[ch]->HWPTRAINCONTROL0.retrain_en = 0;
	__DREX[ch]->HWPTRAINCONFIG1.retrain_gate = 0;
	__DREX[ch]->HWPTRAINCONFIG1.retrain_gate_all_rank = 0;
	__DREX[ch]->HWPTRAINCONTROL1.retrain_en = 0;

	__PHY[ch]->GNR_CON0.ctrl_bstlen = 16;

	if (MEMCLK_1552 == cal_get_mif) {
		__PHY[ch]->GNR_CON0.ctrl_rdlat = 32;
		__PHY[ch]->GNR_CON0.ctrl_wrlat = 14;

		__PHY[ch]->DVFS_CON2.freq_train= 1552;
	} else if (MEMCLK_1456 == cal_get_mif) {
		__PHY[ch]->GNR_CON0.ctrl_rdlat = 32;
		__PHY[ch]->GNR_CON0.ctrl_wrlat = 14;

		__PHY[ch]->DVFS_CON2.freq_train= 1456;
	} else if (MEMCLK_1264 == cal_get_mif) {
		__PHY[ch]->GNR_CON0.ctrl_rdlat = 28;
		__PHY[ch]->GNR_CON0.ctrl_wrlat = 12;

		__PHY[ch]->DVFS_CON2.freq_train= 1264;
	}


	return;
}

int dmc_get_num_rank(lp4_density_t density)
{
	int num_rank;

	/* there's no way to detect the number of rank at this time
		we assume that number of rank is determined by density
		it is necessary to modify this function if MCP configuration is changed
	*/
	switch(density) {
		case LP4_12Gb_Die:
			num_rank = 1;
			break;
		default:
			num_rank = 2;
			break;
	}

	return num_rank;
}

int dmc_set_by_density(lp4_density_t density, u32 num_chip)
{
	int ch=0;
	u32 rank_inter_en;
	u32 chip_row;
	u32 chip0_size;
	u32 pb_ref_en = 1;
	u32 cal_get_mif;
	u32 timingrow, timingpower,trfcpb;
#if defined(CONFIG_SMC_CMD)
	reg_arr_t reg_arr;
#endif

#if defined(CONFIG_DRAM_MULTI_CAPACITY_SUPPORT)
	unsigned char dram_vref;
#endif
	cal_get_mif = _cal_get_mif();

	switch(density) {
		case LP4_12Gb_Die:
			chip_row=4;	//16bits
			chip0_size=2;	//768MB

			if (3 == __DREX[ch]->TIMINGAREF.t_refipb)	// ABR in case of 1/4 refresh interval
				pb_ref_en = 0;

			if (MEMCLK_1552 == cal_get_mif) {
				timingrow = 0x6D588652;
				timingpower = 0x40710336;
				trfcpb = 0x37;
			} else if (MEMCLK_1456 == cal_get_mif) {
				timingrow = 0x66588611;
				timingpower = 0x3C6A0336;
				trfcpb = 0x33;
			} else if (MEMCLK_1264 == cal_get_mif) {
				timingrow = 0x5947754F;
				timingpower = 0x345C0335;
				trfcpb = 0x2D;
			} else {
				;
			}
			break;
		case LP4_6Gb_Die:
			chip_row=3;	//15bits
			chip0_size=1;	//512MB

			if (MEMCLK_1552 == cal_get_mif) {
				timingrow = 0x46588652;
				timingpower = 0x404A0336;
				trfcpb = 0x23;
			} else if (MEMCLK_1456 == cal_get_mif) {
				timingrow = 0x42588611;
				timingpower = 0x3C460336;
				trfcpb = 0x21;
			} else if (MEMCLK_1264 == cal_get_mif) {
				timingrow = 0x3947754F;
				timingpower = 0x343D0335;
				trfcpb = 0x1D;
			} else {
				;
			}
			break;
		case LP4_4Gb_Die:
		case LP4_8Gb_Die:
		case LP4_16Gb_Die:
		case LP4_24Gb_Die:
		case LP4_32Gb_Die:
		case LP4_Invalid_Die:
		default:
			chip_row=3;	//15bits
			chip0_size=1;	//512MB
			if (MEMCLK_1552 == cal_get_mif) {
				timingrow = 0x46588652;
				timingpower = 0x404A0336;
				trfcpb = 0x23;
			} else if (MEMCLK_1456 == cal_get_mif) {
				timingrow = 0x42588611;
				timingpower = 0x3C460336;
				trfcpb = 0x21;
			} else if (MEMCLK_1264 == cal_get_mif) {
				timingrow = 0x3947754F;
				timingpower = 0x343D0335;
				trfcpb = 0x1D;
			} else {
				;
			}
			break;
		}

	if (2 == num_chip)
		rank_inter_en=1;
	else
		rank_inter_en=0;

	for(ch=0;ch<4;ch++) {
		__REG32( (u64)&__DREX[ch]->TIMINGROW1 ) = timingrow;
		__REG32( (u64)&__DREX[ch]->TIMINGPOWER1 ) = timingpower;
		__REG32( (u64)&__DREX[ch]->TIMINGROW0 ) = timingrow;
		__REG32( (u64)&__DREX[ch]->TIMINGPOWER0 ) = timingpower;
		__DREX[ch]->TIMINGRFCPB.t_rfcpb0 = trfcpb;
		__DREX[ch]->TIMINGRFCPB.t_rfcpb1 = trfcpb;

		__DREX[ch]->MEMCONTROL.pb_ref_en = pb_ref_en;
		__DREX[ch]->MEMCONTROL.num_chip=num_chip-1;

#if defined(CONFIG_SMC_CMD)
		reg_arr.set0.addr = ((u32)&__ASP[ch]->MEMCONFIG0);
		reg_arr.set0.val = ((0x01302333)&~((0x1<<24)|(0xF<<4)))|((rank_inter_en<<24)|(chip_row<<4));
		reg_arr.set1.addr = ((u32)&__ASP[ch]->MEMCONFIG1);
		reg_arr.set1.val = ((0x01302333)&~((0x1<<24)|(0xF<<4)))|((rank_inter_en<<24)|(chip_row<<4));
		reg_arr.set2.addr = ((u32)&__ASP[ch]->MEMSIZE0);
		reg_arr.set2.val = chip0_size;
		set_secure_reg((u64)&reg_arr, 3);
#else
		__ASP[ch]->MEMCONFIG0.rank_inter_en=rank_inter_en;
		__ASP[ch]->MEMCONFIG0.chip_row=chip_row;
		__ASP[ch]->MEMCONFIG1.rank_inter_en=rank_inter_en;
		__ASP[ch]->MEMCONFIG1.chip_row=chip_row;
		__ASP[ch]->MEMSIZE0.chip0_size=chip0_size;
#endif
	}

#if defined(CONFIG_DRAM_MULTI_CAPACITY_SUPPORT)
	dram_vref = (get_vref_fuse_bits() & 0x00003F00) >> 8;
	if (dram_vref == 0) {
		if (density == LP4_8Gb_Die) {
			dram_vref = 0x10;               // 16.4%
		} else {
			dram_vref = 0x17;               // 19.2%
		}

		for (ch = 0; ch < 4; ch++) {
			dmc_mrw_command(ch, DMC_RANK_ALL, 13, 0x40);
			dmc_mrw_command(ch, DMC_RANK_ALL, 14, dram_vref);
			dmc_mrw_command(ch, DMC_RANK_ALL, 13, 0x00);
			dmc_mrw_command(ch, DMC_RANK_ALL, 14, dram_vref);
		}
	}
#endif

	return 1;
}

static lp4_density_t DMC_Detecting_LP4_SizeInfo(unsigned int ch, int cs, char mr)
{
	unsigned int mem_type, mem_density, mem_width;
	unsigned int mrstatus;
	int loop=20, delay;

	do{
		dmc_mrr_command(ch, cs, mr);
		mrstatus = __DREX[ch]->MRSTATUS.mr_status;

		mem_type = mrstatus&0x3;
		mem_density = (mrstatus >> 2)&0xF;
		mem_width = (mrstatus >> 6)&0x3;

		loop--;

		if(loop < 1)
		{
			ch++;

			if(ch > 3)
			{
				for(delay = 0 ; delay < 0xFF000000 ; delay++);
				mem_density = 7;
				break;
			}

			loop=20;
		}
	}while( (mem_type != 0x0) || (mem_width != 0x0) || (mem_density < 1) || (mem_density > 3) );

#if 1
	// This code mabe not need.
	switch(mem_density)
	{
		case 1:
			mem_density = LP4_6Gb_Die;
			break;
		case 2:
			mem_density = LP4_8Gb_Die;
			break;
		case 3:
			mem_density = LP4_12Gb_Die;
			break;
		default:
			mem_density = LP4_Invalid_Die;
			break;
	}
#endif
	return mem_density;
}

/******************************************************************************
 *
 * @fn
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
static void dmc_base_addr(void)
{

	__DREX[0] = (volatile drex_regs_t *)0x10800000;
	__DREX[1] = (volatile drex_regs_t *)0x10900000;
	__DREX[2] = (volatile drex_regs_t *)0x10a00000;
	__DREX[3] = (volatile drex_regs_t *)0x10b00000;

	__PPC[0] = (volatile drex_ppc_regs_t *)( 0x10800000 + 0xe000 );
	__PPC[1] = (volatile drex_ppc_regs_t *)( 0x10900000 + 0xe000 );
	__PPC[2] = (volatile drex_ppc_regs_t *)( 0x10a00000 + 0xe000 );
	__PPC[3] = (volatile drex_ppc_regs_t *)( 0x10b00000 + 0xe000 );

	__PHY[0]  = (volatile phy_regs_t *)0x10820000;
	__PHY[1]  = (volatile phy_regs_t *)0x10920000;
	__PHY[2]  = (volatile phy_regs_t *)0x10a20000;
	__PHY[3]  = (volatile phy_regs_t *)0x10b20000;

	__ASP[0]  = (volatile asp_regs_t *)( 0x10810000 + 0xf00 );
	__ASP[1]  = (volatile asp_regs_t *)( 0x10910000 + 0xf00 );
	__ASP[2]  = (volatile asp_regs_t *)( 0x10a10000 + 0xf00 );
	__ASP[3]  = (volatile asp_regs_t *)( 0x10b10000 + 0xf00 );
}


/******************************************************************************
 *
 * @fn
 *
 * @brief
 *
 * @param	none.
 *
 * @return	true or false.
 *
 *****************************************************************************/
void InitDmcDriver(void)
{
	dmc_base_addr();

	DmcDriver.init.reset = dmc_reset;
	DmcDriver.init.preset = dmc_preset;
	DmcDriver.init.done = dmc_init_done;
	DmcDriver.init.sizeinfo = DMC_Detecting_LP4_SizeInfo;
	DmcDriver.init.setbydensity = dmc_set_by_density;
	DmcDriver.init.getnumrank = dmc_get_num_rank;

	DmcDriver.io.cal = dmc_zq_cal;
	DmcDriver.io.dds = dmc_set_dds;
//	DmcDriver.io.odt = dmc_set_odt;
//	DmcDriver.io.vref = dmc_set_vref;

	DmcDriver.dfi.init = dmc_dfi_init;
	DmcDriver.dfi.status = dmc_dfi_status;
	DmcDriver.dfi.update = dmc_dfi_update;
	DmcDriver.dfi.mode = dmc_dfi_mode;

	DmcDriver.dll.enable = dmc_dll_enable;
	DmcDriver.dll.relock = dmc_dll_relock;
	DmcDriver.dll.status = dmc_dll_lock_stat;
	DmcDriver.dll.lock_value = dmc_get_dll_lock_value;
	DmcDriver.dll.force = dmc_dll_force;
	DmcDriver.dll.resync = dmc_dll_resync;

//	DmcDriver.offset.read = phy_set_read_dqs_offset;
//	DmcDriver.offset.write = phy_set_write_dq_offset;
//	DmcDriver.offset.addr = phy_set_addr_offset;

	DmcDriver.cal.preset = dmc_cal_preset;
//	DmcDriver.cal.command_bus = dmc_command_bus_training;
#if (DMC_DQS_TYPE == DMC_DQS_MODE1)
	DmcDriver.cal.gate = dmc_gate_training;
#endif
	DmcDriver.cal.write_dq = dmc_write_dq_cal;
	DmcDriver.cal.read_dq = dmc_read_dq_cal;
	DmcDriver.cal.status = dmc_cal_status;
	DmcDriver.cal.hw_periodic = dmc_hw_periodic_train;
	DmcDriver.cal.postset = dmc_cal_postset;

	DmcDriver.ppc.init = dmc_ppc_init;
	DmcDriver.ppc.ccnt = dmc_get_ccnt;

#if 0
	DmcDriver.command = dmc_send_command;
#else
	DmcDriver.command.mrr = dmc_mrr_command;
	DmcDriver.command.mrw = dmc_mrw_command;
	DmcDriver.command.pre = dmc_pre_command;
	DmcDriver.command.sre = dmc_sre_command;
	DmcDriver.command.srx = dmc_srx_command;
	DmcDriver.command.ref = dmc_ref_command;
	DmcDriver.command.act = dmc_act_command;
	DmcDriver.command.mpc = dmc_mpc_command;
	DmcDriver.command.pdx = dmc_pdx_command;
	DmcDriver.command.pde = dmc_pde_command;

#endif

	return;
}
