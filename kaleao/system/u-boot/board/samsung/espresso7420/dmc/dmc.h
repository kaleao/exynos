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

#ifndef DMC_H_
#define DMC_H_

#define DREX1_V2_0_5
#define PHY_V7_R0

#include "drex.h"
#include "phy.h"

#define MHz (1e6)
#define msec (1e-3)
#define usec (1e-6)
#define nsec (1e-9)
#define psec (1e-12)

#define MC_INITIATED_SDLL_UPDATE	0

#define DRAM_POWER_OPTIMIZE		1
#define DRAM_REF_INTVL_2X			0

#ifndef __ASSEMBLY__

#define DMC_DIFF_IO		0
#define DMC_CMOS_IO		1

#define DMC_DQS_MODE0		0
#define	DMC_DQS_MODE1		1		/* DQS balanced */

#define DMC_IO_TYPE		DMC_DIFF_IO
#define DMC_DQS_TYPE	DMC_DQS_MODE1

typedef enum {
	DMC_RANK_0 = 0,
	DMC_RANK_1 = 1,
	DMC_RANK_ALL = -1,
} dmc_rank_t;

typedef enum {
	DMC_CHANNEL_0 = 0,
	DMC_CHANNEL_1 = 0,
	DMC_CHANNEL_ALL = -1,
} dmc_channel_t;

typedef enum {
	DMC_BYTE_0 = 0,
	DMC_BYTE_1 = 0,
	DMC_BYTE_ALL = -1,
} dmc_byte_t;

typedef enum {
	DMC_LPDDR2_MEM_TYPE,
	DMC_LPDDR3_MEM_TYPE,
	DMC_LPDDR4_MEM_TYPE,
	DMC_DDR2_MEM_TYPE,
	DMC_DDR3_MEM_TYPE,
	DMC_WIO_MEM_TYPE,
	DMC_WIO2_MEM_TYPE,
} dmc_type_t;

typedef enum {
	DMC_MRW_COMMAND  	= 0x0,
	DMC_PALL_COMMAND 	= 0x1,
	DMC_PRE_COMMAND  	= 0x2,
	DMC_MPC_COMMAND		= 0x3,
	DMC_SRE_COMMAND 	= 0x4,
	DMC_REFA_COMMAND 	= 0x5,
	DMC_PDE_COMMAND		= 0x6,
	DMC_PDX_COMMAND		= 0x7,
	DMC_SRX_COMMAND		= 0x8,
	DMC_MRR_COMMAND		= 0x9,
	DMC_ACT_COMMAND		= 0xe,
} dmc_direct_command_t;

typedef enum {
	DMC_WRITE_LEVELING,
	DMC_GATE_LEVELING,
	DMC_CA_TRAINING,
	DMC_READ_DQ_CALIBRATION,
	DMC_WRITE_DQ_CALIBRATION,
	DMC_VREF_TRAINING,
} dmc_cal_t;

typedef enum {
	MR3_DS_34 = 1,
	MR3_DS_40 = 2,
	MR3_DS_48 = 3,
} mr3_op_t;

typedef enum {
	DMC_DS_48 = 4,
	DMC_DS_40 = 5,
	DMC_DS_34 = 6,
	DMC_DS_30 = 7,
} dmc_ds_t;

typedef struct {
	int refi;
	int refpb;
	int rfc;
	int rrd;
	int rp;
	int rcd;
	int rc;
	int ras;
	int wtr;
	int wr;
	int rtp;
	int dqsck;
	int faw;
	int xsr;
	int xp;
	int cke;
	int mrd;
} timing_param_t;


typedef enum {
	DMC_TERM_0,
	DMC_TERM_120,
	DMC_TERM_240
} dmc_term_t;

typedef struct {
	signed char byte[4];
} dmc_dqs_offset_t;

typedef union {
	int addr;
	dmc_dqs_offset_t read;
	dmc_dqs_offset_t write;
} dmc_offset_t;

typedef struct {
	dmc_ds_t data[4];
	dmc_ds_t ck;
	dmc_ds_t cke;
	dmc_ds_t cs;
	dmc_ds_t ca;
} dmc_dds_t;

typedef struct {
	unsigned char BL					: 3;
	unsigned char reserved_3_4			: 2;
	unsigned char nWR					: 3;
} mr1_t;

typedef struct {
	unsigned char RL_WL				: 4;
	unsigned char nWRE					: 1;
	const unsigned char reserved_5	: 1;
	unsigned char WL_Select			: 1;
	unsigned char WR_Leveling			: 1;
} mr2_t;

typedef struct {
	unsigned char DQ_ODT				: 2;
	unsigned char PD_Control			: 1;
	const unsigned char reserved_3_7	: 5;
} mr11_t;

typedef struct {
	unsigned char DS					: 4;
	const unsigned char reserved_4_7	: 4;
} mem_mr3_t;

typedef enum {
	MR11_DQ_ODT_DISABLE,
	MR11_DQ_ODT_RZQ_BY_2,
	MR11_DQ_ODT_RZQ_BY_4
} mem_mr11_t;

typedef struct {
	int num_bank;
	int num_row;
	int num_col;
	int data_width;
} mem_org_t;

typedef struct {
	int random;
	int rank;
	int bank;
} mem_interleave_t;

typedef struct {
	int num_channel;
	int num_rank;
	dmc_type_t mem_type;
	int read_latency;
	int write_latency;
	int burst_length;
	int mclk;
	int aclk;
	int rclk;
	int rd_fetch;
#if defined( DREX1_V2_0_3 )||defined( DREX1_V2_0_4 )
	int t_ppd;
	int rl_4;
#else
#endif
	int t_w2w_c2c;
	int t_r2r_c2c;
	int add_lat_pall;
	int dynamic_sref;
	int dynamic_clkstop;
	int dynamic_powerdown;
#if defined( DREX1_V2_0_3 )||defined( DREX1_V2_0_4 )
	unsigned int mem_chip_base[2];
	unsigned int mem_chip_mask[2];
#elif defined( DREX1_V2_0_5 )
	unsigned int chunk_start_addr[2];
	unsigned int chunk_end_addr[2];
#endif
	mem_interleave_t iv;
	dmc_ds_t ds;
	dmc_dds_t dds;
	dmc_term_t term;
	mem_mr3_t mr3;
	mem_mr11_t mr11;
	mem_org_t mem_org;
	timing_param_t timing;
} dmc_init_config_t;

typedef struct {
	unsigned char vref		:6;
	unsigned char fsbst	:1;
	unsigned char pd		:1;
} dmc_vref_t;

typedef int (*dmc_preset_func_t)(int ch);
typedef int (*dmc_done_func_t)(int ch);
typedef void (*dmc_reset_func_t)(int ch, int reset);

typedef void (*dmc_io_cal_func_t)(int ch);
typedef int (*dmc_io_dds_func_t)(int ch, int byte, int pu, int pd);
typedef int (*dmc_io_odt_func_t)(int ch, int byte, int odt);
typedef int (*dmc_io_vref_func_t)(int ch, int byte, dmc_vref_t *vref);

typedef void (*dmc_dfi_init_func_t)(int ch, int init);
typedef int (*dmc_dfi_stat_func_t)(int ch);
typedef int (*dmc_dfi_update_func_t)(int ch);
typedef void (*dmc_dfi_mode_func_t)(int ch, int mode, int ignore);

typedef int (*dmc_offset_byte_func_t)(int ch, int byte, signed char offset);
typedef void (*dmc_offset_addr_func_t)(int ch, signed char offset);
typedef void (*dmc_offset_resync_func_t)(int ch);

typedef int (*dmc_cal_preset_func_t)(int ch);
typedef int (*dmc_cal_pattern_func_t)(int ch, unsigned char upper, unsigned char lower);
typedef int (*dmc_cal_wrlvl_func_t)(int ch);
typedef int (*dmc_cal_gate_func_t)(int ch, int rank);
typedef int (*dmc_cal_ca_func_t)(int ch, int(*clock)(int *), int freq);
typedef int (*dmc_cal_read_func_t)(int ch, int rank);
typedef int (*dmc_cal_write_func_t)(int ch, int rank);
typedef int (*dmc_cal_vref_func_t)(int ch);
typedef int (*dmc_cal_status_func_t)(int ch, int type);
typedef int (*dmc_cal_hwperiodic_func_t)(int ch, int periodic_en, int retrain_en);
typedef int (*dmc_cal_postset_func_t)(int ch);

typedef int (*dmc_dll_enable_func_t)(int ch, int clock);
typedef int (*dmc_dll_relock_func_t)(int ch);
typedef int (*dmc_dll_stat_func_t)(int ch);
typedef int (*dmc_dll_lock_func_t)(int ch);
typedef int (*dmc_dll_force_func_t)(int ch, int force);
typedef int (*dmc_dll_resync_func_t)(int ch);

typedef int (*dmc_dvfs_voltage_func_t)(float voltage);
typedef int (*dmc_dvfs_freq_func_t)(int freq);
typedef int (*dmc_dvfs_level_func_t)(int level);

typedef int (*dmc_ppc_init_func_t)(int ch, int enable);
typedef unsigned int (*dmc_ppc_ccnt_func_t)(int ch);
typedef unsigned int (*dmc_ppc_pmcnt_func_t)(int ch);

typedef int (*dmc_nop_func_t)(unsigned char ch, unsigned char rank);
typedef void (*dmc_pre_func_t)(unsigned char ch, unsigned char rank, signed char bank);
typedef void (*dmc_ref_func_t)(unsigned char ch, unsigned char rank, signed char bank);
typedef void (*dmc_sre_func_t)(unsigned char ch, unsigned char rank);
typedef int (*dmc_srx_func_t)(unsigned char ch, unsigned char rank);
typedef void (*dmc_pd_func_t)(unsigned char ch, unsigned char rank);
typedef int (*dmc_zqinit_func_t)(unsigned char ch, unsigned char rank);
typedef int (*dmc_zqcl_func_t)(unsigned char ch, unsigned char rank);
typedef int (*dmc_zqcs_func_t)(unsigned char ch, unsigned char rank);
typedef int (*dmc_zqcal_func_t)(unsigned char ch, unsigned char rank);
typedef int (*dmc_zqlat_func_t)(unsigned char ch, unsigned char rank);
typedef void (*dmc_mrw_func_t)(unsigned char ch, unsigned char rank, unsigned char ma, unsigned char op);
typedef unsigned char (*dmc_mrr_func_t)(unsigned char ch, unsigned char rank, unsigned char ma);
typedef void (*dmc_pdx_func_t)(int ch, int rank);
typedef void (*dmc_pde_func_t)(int ch, int rank);
typedef void (*dmc_act_func_t)(unsigned char ch, unsigned char rank);
typedef void (*dmc_mpc_func_t)(int ch, int rank, unsigned char op);

typedef struct {
	dmc_reset_func_t reset;
	dmc_preset_func_t preset;
	dmc_done_func_t done;
} dmc_init_func_t;

typedef struct {
	dmc_io_cal_func_t cal;
	dmc_io_dds_func_t dds;
	dmc_io_odt_func_t odt;
	dmc_io_vref_func_t vref;
} dmc_io_func_t;

typedef struct {
	dmc_dfi_init_func_t init;
	dmc_dfi_stat_func_t status;
	dmc_dfi_update_func_t update;
	dmc_dfi_mode_func_t mode;
} dmc_dfi_func_t;

typedef struct {
	dmc_offset_addr_func_t addr;
	dmc_offset_byte_func_t gate;
	dmc_offset_byte_func_t read;
	dmc_offset_byte_func_t write;
	dmc_offset_resync_func_t resync;
} dmc_offset_func_t;

typedef struct {
	dmc_cal_preset_func_t preset;
	dmc_cal_pattern_func_t pattern;
	dmc_cal_wrlvl_func_t write_level;
	dmc_cal_gate_func_t gate;
	dmc_cal_ca_func_t command_bus;
	dmc_cal_read_func_t read_dq;
	dmc_cal_write_func_t write_dq;
	dmc_cal_vref_func_t vref;
	dmc_cal_status_func_t status;
	dmc_cal_hwperiodic_func_t hw_periodic;
	dmc_cal_postset_func_t postset;
} dmc_cal_func_t;

typedef struct {
	dmc_dll_enable_func_t enable;
	dmc_dll_relock_func_t relock;
	dmc_dll_stat_func_t status;
	dmc_dll_lock_func_t lock_value;
	dmc_dll_force_func_t force;
	dmc_dll_resync_func_t resync;
} dmc_dll_func_t;

typedef struct {
	dmc_dvfs_voltage_func_t voltage;
	dmc_dvfs_freq_func_t freq;
	dmc_dvfs_level_func_t level;
} dmc_dvfs_func_t;

#if 1
typedef struct {
	dmc_pre_func_t pre;
	dmc_ref_func_t ref;
	dmc_sre_func_t sre;
	dmc_srx_func_t srx;
	dmc_pd_func_t pd;
	dmc_mrw_func_t mrw;
	dmc_mrr_func_t mrr;
#if defined( DREX1_V2_0_3 ) || defined( DREX1_V2_0_4 )
	dmc_nop_func_t nop;
	dmc_zqinit_func_t zqinit;
	dmc_zqcl_func_t zqcl;
	dmc_zqcs_func_t zqcs;
#elif defined( DREX1_V2_0_5 )
	dmc_mpc_func_t mpc;
	dmc_pde_func_t pde;
	dmc_pdx_func_t pdx;
	dmc_act_func_t act;
#endif
} dmc_command_func_t;
#endif

typedef struct {
	dmc_ppc_init_func_t init;
	dmc_ppc_ccnt_func_t ccnt;
	dmc_ppc_pmcnt_func_t pmcnt;
} dmc_ppc_func_t;

//- DMC driver type
typedef struct {
	dmc_init_config_t *config;
	dmc_init_func_t init;
	dmc_io_func_t io;
	dmc_offset_func_t offset;
	dmc_cal_func_t cal;
	dmc_dfi_func_t dfi;
	dmc_dll_func_t dll;
	dmc_dvfs_func_t dvfs;
	dmc_ppc_func_t ppc;
	dmc_command_func_t command;
} dmc_driver_t;

extern dmc_driver_t DmcDriver;
extern int InitDmcDriver(void);
extern int dmc_vtc_en(int ch);
extern int dmc_io_pd(int ch);
extern int dmc_low_power(int ch);
extern int clk_init_root_clock_gating(void);
extern int dmc_phy_upd_mode(int ch, int mode);
extern int dmc_sel_drex_timing_set(int ch, int set);

#ifdef __KERNEL__
extern unsigned int get_chip_status();
#endif

#endif

#endif /* DMC_H_ */
