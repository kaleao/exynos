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

#ifndef DREX1_V2_0_5_H_
#define DREX1_V2_0_5_H_

#ifndef __ASSEMBLY__

typedef volatile unsigned int rw_bf_t;
typedef const unsigned int ro_bf_t;
typedef const unsigned int wo_bf_t;

typedef struct {
   rw_bf_t ca_swap                   : ( 0 - 0 + 1 );
   ro_bf_t reserved_1_2              : ( 2 - 1 + 1 );
   rw_bf_t update_mode               : ( 3 - 3 + 1 );
   ro_bf_t reserved_4                : ( 4 - 4 + 1 );
   rw_bf_t aref_en                   : ( 5 - 5 + 1 );
   rw_bf_t io_pd_con                 : ( 7 - 6 + 1 );
   ro_bf_t empty                     : ( 8 - 8 + 1 );
   ro_bf_t reserved_9_11             : ( 11 - 9 + 1 );
   rw_bf_t same_dir_prep             : ( 12 - 12 + 1 );
   ro_bf_t reserved_13_15            : ( 15 - 13 + 1 );
   rw_bf_t timeout_level0            : ( 27 - 16 + 1 );
   rw_bf_t dfi_init_start            : ( 28 - 28 + 1 );
   ro_bf_t reserved_29_30            : ( 30 - 29 + 1 );
   rw_bf_t ignore_dic                : ( 31 - 31 + 1 );
} drex_concontrol_t;

typedef struct {
   rw_bf_t clk_stop_en               : ( 0 - 0 + 1 );
   rw_bf_t dpwrdn_en                 : ( 1 - 1 + 1 );
   rw_bf_t dpwrdn_type               : ( 3 - 2 + 1 );
   ro_bf_t reserved_4                : ( 4 - 4 + 1 );
   rw_bf_t dsref_en                  : ( 5 - 5 + 1 );
   rw_bf_t add_lat_pall              : ( 7 - 6 + 1 );
   rw_bf_t mem_type                  : ( 11 - 8 + 1 );
   rw_bf_t mem_width                 : ( 15 - 12 + 1 );
   rw_bf_t num_chip                  : ( 17 - 16 + 1 );
   rw_bf_t num_chunk                 : ( 19 - 18 + 1 );
   rw_bf_t bl                        : ( 22 - 20 + 1 );
   rw_bf_t dbi_en                    : ( 23 - 23 + 1 );
   ro_bf_t reserved_24            : ( 24 - 24 + 1 );
   rw_bf_t mrr_byte                    : ( 25 - 25 + 1 );
   ro_bf_t reserved_26            : ( 26 - 26 + 1 );
   rw_bf_t pb_ref_en                 : ( 27 - 27 + 1 );
   rw_bf_t sp_en                     : ( 28 - 28 + 1 );
   rw_bf_t pause_ref_en              : ( 29 - 29 + 1 );
   ro_bf_t reserved_30_31            : ( 31 - 30 + 1 );
} drex_memcontrol_t;

typedef struct {
   rw_bf_t busif_rd_cg_en            : ( 0 - 0 + 1 );
   rw_bf_t busif_wr_cg_en            : ( 1 - 1 + 1 );
   rw_bf_t scg_cg_en                 : ( 2 - 2 + 1 );
   rw_bf_t mif_cg_en                 : ( 3 - 3 + 1 );
   rw_bf_t phy_cg_en                 : ( 4 - 4 + 1 );
   rw_bf_t asp_cg_en                  : ( 5 - 5 + 1 );
   rw_bf_t scg_wr_cg_en              : ( 6 - 6 + 1 );
   rw_bf_t scg_rd_cg_en              : ( 7 - 7 + 1 );
   rw_bf_t mif_wr_cg_en              : ( 8 - 8 + 1 );
   rw_bf_t mif_rd_cg_en              : ( 9 - 9 + 1 );
   ro_bf_t reserved_10_31               : ( 31 - 10 + 1 );
} drex_cgcontrol_t;

typedef struct {
   rw_bf_t cmd_addr                  : ( 15 - 0 + 1 );
   rw_bf_t cmd_bank                  : ( 18 - 16 + 1 );
   rw_bf_t rsvd2                     : ( 19 - 19 + 1 );
   rw_bf_t cmd_chip                  : ( 20 - 20 + 1 );
   rw_bf_t cmd_chip_all              : ( 21 - 21 + 1 );
   ro_bf_t reserved_22_23               : ( 23 - 22 + 1 );
   rw_bf_t cmd_type                  : ( 27 - 24 + 1 );
   ro_bf_t reserved_28_31               : ( 31 - 28 + 1 );
} drex_directcmd_t;

typedef struct {
   ro_bf_t reserved_0_15                : ( 15 - 0 + 1 );
   rw_bf_t port_policy               : ( 19 - 16 + 1 );
   ro_bf_t reserved_20_27               : ( 27 - 20 + 1 );
   rw_bf_t tp_en                     : ( 31 - 28 + 1 );
} drex_prechconfig0_t;

typedef struct {
   rw_bf_t mem_term_chips            : ( 0 - 0 + 1 );
   rw_bf_t sl_dll_dyn_con            : ( 1 - 1 + 1 );
   rw_bf_t drv_bus_en                : ( 2 - 2 + 1 );
   rw_bf_t fp_resync                 : ( 3 - 3 + 1 );
   rw_bf_t dqs_delay                 : ( 6 - 4 + 1 );
   rw_bf_t pause_no_relock                   : ( 7 - 7 + 1 );
   rw_bf_t reserved_8_11             : ( 11 - 8 + 1 );
   rw_bf_t rec_gate_cyc              : ( 15 - 12 + 1 );
   ro_bf_t reserved_16_23               : ( 23 - 16 + 1 );
   rw_bf_t ctrl_pd                   : ( 24 - 24 + 1 );
   ro_bf_t reserved_25_28               : ( 28 - 25 + 1 );
   rw_bf_t ctrl_shgate               : ( 29 - 29 + 1 );
   ro_bf_t reserved_30_31               : ( 31 - 30 + 1 );
} drex_phycontrol0_t;

typedef struct {
   rw_bf_t tp_cnt0                   : ( 7 - 0 + 1 );
   rw_bf_t tp_cnt1                   : ( 15 - 8 + 1 );
   rw_bf_t tp_cnt2                   : ( 23 - 16 + 1 );
   rw_bf_t tp_cnt3                   : ( 31 - 24 + 1 );
} drex_prechconfig1_t;

typedef struct {
   rw_bf_t t_rfcpb0                  : ( 5 - 0 + 1 );
   ro_bf_t rsvd1                     : ( 7 - 6 + 1 );
   rw_bf_t t_rfcpb1                  : ( 13 - 8 + 1 );
   ro_bf_t reserved_14_31               : ( 31 - 14 + 1 );
} drex_timingrfcpb_t;

typedef struct {
   rw_bf_t t_zqcs                    : ( 7 - 0 + 1 );
   rw_bf_t t_zqoper                  : ( 17 - 8 + 1 );
   rw_bf_t t_zqinit                  : ( 27 - 18 + 1 );
   rw_bf_t t_zqlat               : ( 31 - 28 + 1 );
} drex_zqtiming_t;

typedef struct {
   rw_bf_t dpwrdn_cyc                : ( 7 - 0 + 1 );
   ro_bf_t rsvd0                     : ( 15 - 8 + 1 );
   rw_bf_t dsref_cyc                 : ( 31 - 16 + 1 );
} drex_pwrdnconfig_t;

typedef struct {
   rw_bf_t t_refi                    : ( 15 - 0 + 1 );
   rw_bf_t t_refipb                  : ( 31 - 16 + 1 );
} drex_timingaref_t;

typedef struct {
   rw_bf_t t_ras                     : ( 5 - 0 + 1 );
   rw_bf_t t_rc                      : ( 11 - 6 + 1 );
   rw_bf_t t_rcd                     : ( 15 - 12 + 1 );
   rw_bf_t t_rp                      : ( 19 - 16 + 1 );
   rw_bf_t t_rrd                     : ( 23 - 20 + 1 );
   rw_bf_t t_rfc                     : ( 31 - 24 + 1 );
} drex_timingrow0_t;

typedef struct {
   rw_bf_t rl                        : ( 5 - 0 + 1 );
   rw_bf_t r_to_w                    : ( 9 - 6 + 1 );
   rw_bf_t wl                        : ( 13 - 10 + 1 );
   rw_bf_t t_r2r_c2c                 : ( 15 - 14 + 1 );
   rw_bf_t t_w2w_c2c                 : ( 18 - 16 + 1 );
   rw_bf_t r_to_w_5th                : ( 19 - 19 + 1 );
   rw_bf_t t_rtp                     : ( 23 - 20 + 1 );
   rw_bf_t t_wr                      : ( 27 - 24 + 1 );
   rw_bf_t t_wtr                     : ( 31 - 28 + 1 );
} drex_timingdata0_t;

typedef struct {
   rw_bf_t t_mrd                     : ( 3 - 0 + 1 );
   rw_bf_t t_cke                     : ( 7 - 4 + 1 );
   rw_bf_t t_xp                      : ( 15 - 8 + 1 );
   rw_bf_t t_xsr                     : ( 25 - 16 + 1 );
   rw_bf_t t_faw                     : ( 31 - 26 + 1 );
} drex_timingpower0_t;

typedef struct {
   ro_bf_t reserved_0_2                 : ( 2 - 0 + 1 );
   ro_bf_t dfi_init_complete         : ( 3 - 3 + 1 );
   ro_bf_t reserved_4_13                : ( 13 - 4 + 1 );
   ro_bf_t training_complete         : ( 14 - 14 + 1 );
   ro_bf_t reserved_15_30               : ( 30 - 15 + 1 );
   rw_bf_t timing_set_sw               : ( 31 - 31 + 1 );
} drex_phystatus_t;

typedef struct {
   ro_bf_t reserved_0_3                 : ( 3 - 0 + 1 );
   rw_bf_t t_src                     : ( 7 - 4 + 1 );
   rw_bf_t t_srr                     : ( 9 - 8 + 1 );
   ro_bf_t reserved_10_11               : ( 11 - 10 + 1 );
   rw_bf_t t_mrr                     : ( 13 - 12 + 1 );
   ro_bf_t reserved_14_31               : ( 31 - 14 + 1 );
} drex_etctiming_t;

typedef struct {
   ro_bf_t chip_busy_state           : ( 3 - 0 + 1 );
   ro_bf_t chip_pd_state             : ( 7 - 4 + 1 );
   ro_bf_t chip_sref_state           : ( 11 - 8 + 1 );
   ro_bf_t reserved_12_31               : ( 31 - 12 + 1 );
} drex_chipstatus_t;

typedef struct {
   rw_bf_t rd_fetch                  : ( 2 - 0 + 1 );
   ro_bf_t reserved_3                   : ( 3 - 3 + 1 );
   rw_bf_t vref_setting              : ( 11 - 4 + 1 );
   ro_bf_t reserved_12_31               : ( 31 - 12 + 1 );
} drex_rdfetch0_t;

typedef struct {
   rw_bf_t rd_fetch                  : ( 2 - 0 + 1 );
   ro_bf_t reserved_3                   : ( 3 - 3 + 1 );
   rw_bf_t vref_setting              : ( 11 - 4 + 1 );
   ro_bf_t reserved_12_31               : ( 31 - 12 + 1 );
} drex_rdfetch1_t;

typedef struct {
   ro_bf_t mr_status                 : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} drex_mrstatus_t;

typedef struct {
   ro_bf_t reserved_0                   : ( 0 - 0 + 1 );
   rw_bf_t phy_wrcsgap_en            : ( 1 - 1 + 1 );
   rw_bf_t ca_odt_en                 : ( 3 - 2 + 1 );
   rw_bf_t ca_stable                 : ( 4 - 4 + 1 );
   rw_bf_t vrcg_reset                : ( 5 - 5 + 1 );
   rw_bf_t fsp_wr                    : ( 6 - 6 + 1 );
   rw_bf_t fsp_op                    : ( 7 - 7 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} drex_etccontrol0_t;

typedef struct {
   ro_bf_t reserved_0                   : ( 0 - 0 + 1 );
   rw_bf_t phy_wrcsgap_en            : ( 1 - 1 + 1 );
   rw_bf_t ca_odt_en                 : ( 3 - 2 + 1 );
   rw_bf_t ca_stable                 : ( 4 - 4 + 1 );
   rw_bf_t vrcg_reset                : ( 5 - 5 + 1 );
   rw_bf_t fsb_wr                    : ( 6 - 6 + 1 );
   rw_bf_t fsp_op                    : ( 7 - 7 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} drex_etccontrol1_t;

typedef struct {
   rw_bf_t cfg_qos                   : ( 11 - 0 + 1 );
   ro_bf_t rsvd1                     : ( 15 - 12 + 1 );
   rw_bf_t cfg_qos_th                : ( 27 - 16 + 1 );
   ro_bf_t reserved_28_31               : ( 31 - 28 + 1 );
} drex_qoscontrol_t;

typedef struct {
	drex_qoscontrol_t qos;
	ro_bf_t reserved;
}drex_qoscontroln_t;

typedef struct {
   rw_bf_t timing_set_sw_con         : ( 0 - 0 + 1 );
   ro_bf_t reserved_1_3                 : ( 3 - 1 + 1 );
   rw_bf_t timing_set_sw             : ( 4 - 4 + 1 );
   ro_bf_t reserved_5_31                : ( 31 - 5 + 1 );
} drex_timingsetsw_t;

typedef struct {
   rw_bf_t t_ras                     : ( 5 - 0 + 1 );
   rw_bf_t t_rc                      : ( 11 - 6 + 1 );
   rw_bf_t t_rcd                     : ( 15 - 12 + 1 );
   rw_bf_t t_rp                      : ( 19 - 16 + 1 );
   rw_bf_t t_rrd                     : ( 23 - 20 + 1 );
   rw_bf_t t_rfc                     : ( 31 - 24 + 1 );
} drex_timingrow1_t;

typedef struct {
   rw_bf_t rl                        : ( 5 - 0 + 1 );
   rw_bf_t dqsck                     : ( 9 - 6 + 1 );
   rw_bf_t wl                        : ( 13 - 10 + 1 );
   rw_bf_t t_r2r_c2c                 : ( 15 - 14 + 1 );
   rw_bf_t t_w2w_c2c                 : ( 18 - 16 + 1 );
   rw_bf_t t_to_w_5th                : ( 19 - 19 + 1 );
   rw_bf_t t_rtp                     : ( 23 - 20 + 1 );
   rw_bf_t t_wr                      : ( 27 - 24 + 1 );
   rw_bf_t t_wtr                     : ( 31 - 28 + 1 );
} drex_timingdata1_t;

typedef struct {
   rw_bf_t t_mrd                     : ( 3 - 0 + 1 );
   rw_bf_t t_cke                     : ( 7 - 4 + 1 );
   rw_bf_t t_xp                      : ( 15 - 8 + 1 );
   rw_bf_t t_xsr                     : ( 25 - 16 + 1 );
   rw_bf_t t_faw                     : ( 31 - 26 + 1 );
} drex_timingpower1_t;

typedef struct {
   rw_bf_t brb_rsv_en_r0             : ( 0 - 0 + 1 );
   rw_bf_t brb_rsv_en_r1             : ( 1 - 1 + 1 );
   rw_bf_t brb_rsv_en_r2             : ( 2 - 2 + 1 );
   rw_bf_t brb_rsv_en_r3             : ( 3 - 3 + 1 );
   rw_bf_t brb_rsv_en_w0             : ( 4 - 4 + 1 );
   rw_bf_t brb_rsv_en_w1             : ( 5 - 5 + 1 );
   rw_bf_t brb_rsv_en_w2             : ( 6 - 6 + 1 );
   rw_bf_t brb_rsv_en_w3             : ( 7 - 7 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} drex_brbrsvcontrol_t;

typedef struct {
   rw_bf_t brb_rsv_th_r0             : ( 3 - 0 + 1 );
   rw_bf_t brb_rsv_th_r1             : ( 7 - 4 + 1 );
   rw_bf_t brb_rsv_th_r2             : ( 11 - 8 + 1 );
   rw_bf_t brb_rsv_th_r3             : ( 15 - 12 + 1 );
   rw_bf_t brb_rsv_th_w0             : ( 19 - 16 + 1 );
   rw_bf_t brb_rsv_th_w1             : ( 23 - 20 + 1 );
   rw_bf_t brb_rsv_th_w2             : ( 27 - 24 + 1 );
   rw_bf_t brb_rsv_th_w3             : ( 31 - 28 + 1 );
} drex_brbrsvconfig_t;

typedef struct {
   rw_bf_t brb_qos_timer_dec         : ( 11 - 0 + 1 );
   ro_bf_t reserved_12_31               : ( 31 - 12 + 1 );
} drex_brbqosconfig_t;

typedef struct {
   rw_bf_t data_rsv_en_r0            : ( 0 - 0 + 1 );
   rw_bf_t data_rsv_en_r1            : ( 1 - 1 + 1 );
   rw_bf_t data_rsv_en_r2            : ( 2 - 2 + 1 );
   rw_bf_t data_rsv_en_r3            : ( 3 - 3 + 1 );
   rw_bf_t data_rsv_en_w0            : ( 4 - 4 + 1 );
   rw_bf_t data_rsv_en_w1            : ( 5 - 5 + 1 );
   rw_bf_t data_rsv_en_w2            : ( 6 - 6 + 1 );
   rw_bf_t data_rsv_en_w3            : ( 7 - 7 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} drex_datarsvcontrol_t;

typedef struct {
   rw_bf_t data_rsv_th_r0            : ( 7 - 0 + 1 );
   rw_bf_t data_rsv_th_r1            : ( 15 - 8 + 1 );
   rw_bf_t data_rsv_th_r2            : ( 23 - 16 + 1 );
   rw_bf_t data_rsv_th_r3            : ( 31 - 24 + 1 );
} drex_rdatarsvconfig_t;

typedef struct {
   rw_bf_t data_rsv_th_w0            : ( 7 - 0 + 1 );
   rw_bf_t data_rsv_th_w1            : ( 15 - 8 + 1 );
   rw_bf_t data_rsv_th_w2            : ( 23 - 16 + 1 );
   rw_bf_t data_rsv_th_w3            : ( 31 - 24 + 1 );
} drex_wdatarsvconfig_t;

typedef struct {
   ro_bf_t reserved_0_3                 : ( 3 - 0 + 1 );
   rw_bf_t t_wlo                     : ( 7 - 4 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} drex_wrlvlconfig0_t;

typedef struct {
   rw_bf_t wrlvl_wrdata_en           : ( 0 - 0 + 1 );
   ro_bf_t reserved_1_31                : ( 31 - 1 + 1 );
} drex_wrlvlconfig1_t;

typedef struct {
   ro_bf_t wrlvl_fsm                 : ( 4 - 0 + 1 );
   ro_bf_t reserved_5_31                : ( 31 - 5 + 1 );
} drex_wrlvlstatus_t;

typedef struct {
   rw_bf_t perev_clk_en              : ( 0 - 0 + 1 );
   ro_bf_t reserved_1_31                : ( 31 - 1 + 1 );
} drex_ppcclkcon_t;

typedef struct {
   rw_bf_t perev0_sel                : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} drex_perevconfig0_t;

typedef struct {
   rw_bf_t perev1_sel                : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} drex_perevconfig1_t;

typedef struct {
   rw_bf_t perev2_sel                : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} drex_perevconfig2_t;

typedef struct {
   rw_bf_t perev3_sel                : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} drex_perevconfig3_t;

typedef struct {
   ro_bf_t ctrl_io_rdata             : ( 31 - 0 + 1 );
} drex_ctrl_io_rdata_t;

typedef struct {
   ro_bf_t reserved_0                   : ( 0 - 0 + 1 );
   rw_bf_t emergent_r_config         : ( 31 - 1 + 1 );
} drex_emergent_config0_t;

typedef struct {
   ro_bf_t reserved_0                   : ( 0 - 0 + 1 );
   rw_bf_t emergent_w_config         : ( 31 - 1 + 1 );
} drex_emergent_config1_t;

typedef struct {
   rw_bf_t bp_en                     : ( 0 - 0 + 1 );
   rw_bf_t bp_under_emergent         : ( 1 - 1 + 1 );
   ro_bf_t reserved_2_31                : ( 31 - 2 + 1 );
} drex_bp_control_t;

typedef struct {
   rw_bf_t bp_on_th_brb              : ( 2 - 0 + 1 );
   ro_bf_t reserved_3_7                 : ( 7 - 3 + 1 );
   rw_bf_t bp_off_th_brb             : ( 10 - 8 + 1 );
   ro_bf_t reserved_11_15               : ( 15 - 11 + 1 );
   rw_bf_t bp_on_th_data             : ( 21 - 16 + 1 );
   ro_bf_t reserved_22_23               : ( 23 - 22 + 1 );
   rw_bf_t bp_off_th_data            : ( 29 - 24 + 1 );
   ro_bf_t reserved_30_31               : ( 31 - 30 + 1 );
} drex_bp_config_r_t;

typedef struct {
   rw_bf_t bp_on_th_brb              : ( 2 - 0 + 1 );
   ro_bf_t reserved_3_7                 : ( 7 - 3 + 1 );
   rw_bf_t bp_off_th_brb             : ( 10 - 8 + 1 );
   ro_bf_t reserved_11_15               : ( 15 - 11 + 1 );
   rw_bf_t bp_on_th_data             : ( 21 - 16 + 1 );
   ro_bf_t reserved_22_23               : ( 23 - 22 + 1 );
   rw_bf_t bp_off_th_data            : ( 29 - 24 + 1 );
   ro_bf_t reserved_30_31               : ( 31 - 30 + 1 );
} drex_bp_config_w_t;

typedef struct {
	drex_bp_control_t BP_CONTROL;
	drex_bp_config_r_t BP_CONFIG_R;
	drex_bp_config_w_t BP_CONFIG_W;
	ro_bf_t reserved;
}drex_bp_control_set_t;

typedef struct {
   rw_bf_t diable_fifo_emergent      : ( 3 - 0 + 1 );
   ro_bf_t reserved_4_31                : ( 31 - 4 + 1 );
} drex_fifo_emer_config_t;

typedef struct {
   rw_bf_t ca_pipeline               : ( 1 - 0 + 1 );
   ro_bf_t reserved_2_3                 : ( 3 - 2 + 1 );
   rw_bf_t dq_pipeline               : ( 6 - 4 + 1 );
   ro_bf_t reserved_7_31                : ( 31 - 7 + 1 );
} drex_pl_config_t;

typedef struct {
   rw_bf_t all_init_done             : ( 0 - 0 + 1 );
   ro_bf_t reserved_1_31                : ( 31 - 1 + 1 );
} drex_all_int_indi_t;

typedef struct {
   rw_bf_t t_rdlvl_en                : ( 3 - 0 + 1 );
   rw_bf_t t_rdlvl_rr                : ( 8 - 4 + 1 );
   ro_bf_t reserved_9_11                : ( 11 - 9 + 1 );
   rw_bf_t t_wrtrn_en                : ( 15 - 12 + 1 );
   rw_bf_t t_wrtrn_wr                : ( 19 - 16 + 1 );
   rw_bf_t t_wrtrn_rw                : ( 24 - 20 + 1 );
   ro_bf_t reserved_25_27               : ( 27 - 25 + 1 );
   rw_bf_t t_rdtrd                   : ( 31 - 28 + 1 );
} drex_train_timing0_t;

typedef struct {
   rw_bf_t t_rdlvl_en                : ( 3 - 0 + 1 );
   rw_bf_t t_rdlvl_rr                : ( 8 - 4 + 1 );
   ro_bf_t reserved_9_11                : ( 11 - 9 + 1 );
   rw_bf_t t_wrtrn_en                : ( 15 - 12 + 1 );
   rw_bf_t t_wrtrn_wr                : ( 19 - 16 + 1 );
   rw_bf_t t_wrtrn_rw                : ( 24 - 20 + 1 );
   ro_bf_t reserved_25_27               : ( 27 - 25 + 1 );
   rw_bf_t t_rdtrd                   : ( 31 - 28 + 1 );
} drex_train_timing1_t;

typedef struct {
   rw_bf_t t_ptrain_period           : ( 23 - 0 + 1 );
   ro_bf_t reserved_24_31               : ( 31 - 24 + 1 );
} drex_hw_ptrain_period0_t;

typedef struct {
   rw_bf_t t_ptrain_period           : ( 23 - 0 + 1 );
   ro_bf_t reserved_24_31               : ( 31 - 24 + 1 );
} drex_hw_ptrain_period1_t;

typedef struct {
   rw_bf_t init_gate_train_chip0		: ( 0 - 0 + 1 );
   rw_bf_t init_read_train_chip0		: ( 1 - 1 + 1 );
   rw_bf_t init_write_train_chip0		: ( 2 - 2 + 1 );
   ro_bf_t reserved_3_15				: ( 15 - 3 + 1 );
   rw_bf_t init_gate_train_chip1		: ( 16 - 16 + 1 );
   ro_bf_t reserved_17					: ( 17 - 17 + 1 );
   rw_bf_t init_write_train_chip1		: ( 18 - 18 + 1 );
   ro_bf_t reserved_19_31				: ( 31 - 19 + 1 );
} drex_inittrainconfig_t;

typedef struct {
   rw_bf_t init_train_start          : ( 0 - 0 + 1 );
   ro_bf_t reserved_1_31                : ( 31 - 1 + 1 );
} drex_inittraincontrol_t;

typedef struct {
   rw_bf_t swp_gate_train            : ( 0 - 0 + 1 );
   rw_bf_t swp_read_train            : ( 1 - 1 + 1 );
   rw_bf_t swp_write_train           : ( 2 - 2 + 1 );
   ro_bf_t reserved_3_15                : ( 15 - 3 + 1 );
   rw_bf_t gate_all_rank             : ( 16 - 16 + 1 );
   ro_bf_t reserved_17                  : ( 17 - 17 + 1 );
   rw_bf_t write_all_rank            : ( 18 - 18 + 1 );
   ro_bf_t reserved_19_31               : ( 31 - 19 + 1 );
} drex_swptrainconfig_t;

typedef struct {
   rw_bf_t swp_train_start           : ( 0 - 0 + 1 );
   ro_bf_t reserved_1_31                : ( 31 - 1 + 1 );
} drex_swptraincontrol_t;

typedef struct {
   rw_bf_t retrain_gate              : ( 0 - 0 + 1 );
   rw_bf_t retrain_read              : ( 1 - 1 + 1 );
   rw_bf_t retrain_write             : ( 2 - 2 + 1 );
   ro_bf_t reserved_3_7                 : ( 7 - 3 + 1 );
   rw_bf_t periodic_gate             : ( 8 - 8 + 1 );
   rw_bf_t periodic_read             : ( 9 - 9 + 1 );
   rw_bf_t periodic_write            : ( 10 - 10 + 1 );
   ro_bf_t reserved_11_15               : ( 15 - 11 + 1 );
   rw_bf_t retrain_gate_all_rank     : ( 16 - 16 + 1 );
   ro_bf_t reserved_17                  : ( 17 - 17 + 1 );
   rw_bf_t retrain_write_all_rank    : ( 18 - 18 + 1 );
   ro_bf_t reserved_19_23               : ( 23 - 19 + 1 );
   rw_bf_t periodic_gate_all_rank    : ( 24 - 24 + 1 );
   ro_bf_t reserved_25                  : ( 25 - 25 + 1 );
   rw_bf_t periodic_write_all_rank   : ( 26 - 26 + 1 );
   ro_bf_t reserved_27_31               : ( 31 - 27 + 1 );
} drex_hwptrainconfig0_t;

typedef struct {
   rw_bf_t retrain_gate              : ( 0 - 0 + 1 );
   rw_bf_t retrain_read              : ( 1 - 1 + 1 );
   rw_bf_t retrain_write             : ( 2 - 2 + 1 );
   ro_bf_t reserved_3_7                 : ( 7 - 3 + 1 );
   rw_bf_t periodic_gate             : ( 8 - 8 + 1 );
   rw_bf_t periodic_read             : ( 9 - 9 + 1 );
   rw_bf_t periodic_write            : ( 10 - 10 + 1 );
   ro_bf_t reserved_11_15               : ( 15 - 11 + 1 );
   rw_bf_t retrain_gate_all_rank     : ( 16 - 16 + 1 );
   ro_bf_t reserved_17                  : ( 17 - 17 + 1 );
   rw_bf_t retrain_write_all_rank    : ( 18 - 18 + 1 );
   ro_bf_t reserved_19_23               : ( 23 - 19 + 1 );
   rw_bf_t periodic_gate_all_rank    : ( 24 - 24 + 1 );
   ro_bf_t reserved_25                  : ( 25 - 25 + 1 );
   rw_bf_t periodic_write_all_rank   : ( 26 - 26 + 1 );
   ro_bf_t reserved_27_31               : ( 31 - 27 + 1 );
} drex_hwptrainconfig1_t;

typedef struct {
   rw_bf_t retrain_en                : ( 0 - 0 + 1 );
   rw_bf_t hw_periodic_train_en      : ( 1 - 1 + 1 );
   ro_bf_t reserved_2_31                : ( 31 - 2 + 1 );
} drex_hwptraincontrol0_t;

typedef struct {
   rw_bf_t retrain_en                : ( 0 - 0 + 1 );
   rw_bf_t hw_periodic_train_en      : ( 1 - 1 + 1 );
   ro_bf_t reserved_2_31                : ( 31 - 2 + 1 );
} drex_hwptraincontrol1_t;

typedef struct {
   rw_bf_t wrtra_wrdata_p1           : ( 15 - 0 + 1 );
   rw_bf_t wrtra_wrdata_p0           : ( 31 - 16 + 1 );
} drex_wrtra_pattern0_t;

typedef struct {
   rw_bf_t wrtra_wrdata_p3           : ( 15 - 0 + 1 );
   rw_bf_t wrtra_wrdata_p2           : ( 31 - 16 + 1 );
} drex_wrtra_pattern1_t;

typedef struct {
   rw_bf_t wrtra_wrdata_dm_p3        : ( 1 - 0 + 1 );
   rw_bf_t wrtra_wrdata_dm_p2        : ( 3 - 2 + 1 );
   rw_bf_t wrtra_wrdata_dm_p1        : ( 5 - 4 + 1 );
   rw_bf_t wrtra_wrdata_dm_p0        : ( 7 - 6 + 1 );
   rw_bf_t rsvd                      : ( 31 - 8 + 1 );
} drex_wrtra_pattern2_t;

typedef struct {
   ro_bf_t csysreq                   : ( 0 - 0 + 1 );
   ro_bf_t csysack                   : ( 1 - 1 + 1 );
   ro_bf_t reserved_2_3                 : ( 3 - 2 + 1 );
   ro_bf_t lpi_state                 : ( 6 - 4 + 1 );
   ro_bf_t reserved_7                   : ( 7 - 7 + 1 );
   ro_bf_t agent_axl_state           : ( 14 - 8 + 1 );
   ro_bf_t reserved_15                  : ( 15 - 15 + 1 );
   ro_bf_t pause_req                 : ( 16 - 16 + 1 );
   ro_bf_t pause_ack                 : ( 17 - 17 + 1 );
   ro_bf_t reserved_18_19               : ( 19 - 18 + 1 );
   ro_bf_t pause_state               : ( 22 - 20 + 1 );
   ro_bf_t reserved_23_31               : ( 31 - 23 + 1 );
} drex_lpi_pause_state_t;

typedef struct {
   ro_bf_t fifo_empty_ar             : ( 3 - 0 + 1 );
   ro_bf_t fifo_empty_aw             : ( 7 - 4 + 1 );
   ro_bf_t brb_empty_rd              : ( 8 - 8 + 1 );
   ro_bf_t brb_empty_wr              : ( 9 - 9 + 1 );
   ro_bf_t fifo_empty_bresp          : ( 14 - 10 + 1 );
   ro_bf_t reserved_15_31               : ( 31 - 15 + 1 );
} drex_empty_state_t;

typedef struct {
   ro_bf_t occupancy_p0              : ( 7 - 0 + 1 );
   ro_bf_t occupancy_p1              : ( 15 - 8 + 1 );
   ro_bf_t occupancy_p2              : ( 23 - 16 + 1 );
   ro_bf_t occupancy_p3              : ( 31 - 24 + 1 );
} drex_rbuf_occupancy_t;

typedef struct {
   ro_bf_t occupancy_p0              : ( 7 - 0 + 1 );
   ro_bf_t occupancy_p1              : ( 15 - 8 + 1 );
   ro_bf_t occupancy_p2              : ( 23 - 16 + 1 );
   ro_bf_t occupancy_p3              : ( 31 - 24 + 1 );
} drex_wbuf_occupancy_t;

typedef struct {
   ro_bf_t reserved_0                   : ( 0 - 0 + 1 );
   ro_bf_t emergent                  : ( 31 - 1 + 1 );
} drex_emergent_r_t;

typedef struct {
   ro_bf_t reserved_0                   : ( 0 - 0 + 1 );
   ro_bf_t emergent                  : ( 31 - 1 + 1 );
} drex_emergent_w_t;

typedef struct {
   ro_bf_t backpressure_r            : ( 3 - 0 + 1 );
   ro_bf_t backpressure_w            : ( 7 - 4 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} drex_backpressure_t;

typedef struct {
   ro_bf_t fifo_p0                   : ( 3 - 0 + 1 );
   ro_bf_t fifo_p1                   : ( 7 - 4 + 1 );
   ro_bf_t fifo_p2                   : ( 11 - 8 + 1 );
   ro_bf_t fifo_p3                   : ( 15 - 12 + 1 );
   ro_bf_t reserved_16_31                : ( 31 - 16 + 1 );
} drex_rfifo_valid_state_t;

typedef struct {
   ro_bf_t fifo_p0                   : ( 3 - 0 + 1 );
   ro_bf_t fifo_p1                   : ( 7 - 4 + 1 );
   ro_bf_t fifo_p2                   : ( 11 - 8 + 1 );
   ro_bf_t fifo_p3                   : ( 15 - 12 + 1 );
   ro_bf_t reserved_16_31                : ( 31 - 16 + 1 );
} drex_wfifo_valid_state_t;

typedef struct {
   ro_bf_t fifo_p0                   : ( 3 - 0 + 1 );
   ro_bf_t fifo_p1                   : ( 7 - 4 + 1 );
   ro_bf_t fifo_p2                   : ( 11 - 8 + 1 );
   ro_bf_t fifo_p3                   : ( 15 - 12 + 1 );
   ro_bf_t reserved_16_31                : ( 31 - 16 + 1 );
} drex_cfifo_valid_state_t;

typedef struct {
   rw_bf_t r_brb                     : ( 31 - 0 + 1 );
} drex_r_brb_valid_state_t;

typedef struct {
   rw_bf_t w_brb                     : ( 31 - 0 + 1 );
} drex_w_brb_valid_state_t;

typedef struct {
   rw_bf_t axmarker_or_axid          : ( 15 - 0 + 1 );
   ro_bf_t reserved_16_31                : ( 31 - 16 + 1 );
} drex_info_config_t;

typedef struct {
   ro_bf_t fifo_brb0_0               : ( 15 - 0 + 1 );
   ro_bf_t fifo_brb0_1               : ( 31 - 16 + 1 );
} drex_r_fifo_info_p0_0_t;

typedef struct {
   ro_bf_t fifo_brb1_0               : ( 15 - 0 + 1 );
   ro_bf_t fifo_brb1_1               : ( 31 - 16 + 1 );
} drex_r_fifo_info_p0_1_t;

typedef struct {
   ro_bf_t fifo_brb0_0               : ( 15 - 0 + 1 );
   ro_bf_t fifo_brb0_1               : ( 31 - 16 + 1 );
} drex_r_fifo_info_p1_0_t;

typedef struct {
   ro_bf_t fifo_brb1_0               : ( 15 - 0 + 1 );
   ro_bf_t fifo_brb1_1               : ( 31 - 16 + 1 );
} drex_r_fifo_info_p1_1_t;

typedef struct {
   ro_bf_t fifo_brb0_0               : ( 15 - 0 + 1 );
   ro_bf_t fifo_brb0_1               : ( 31 - 16 + 1 );
} drex_r_fifo_info_p2_0_t;

typedef struct {
   ro_bf_t fifo_brb1_0               : ( 15 - 0 + 1 );
   ro_bf_t fifo_brb1_1               : ( 31 - 16 + 1 );
} drex_r_fifo_info_p2_1_t;

typedef struct {
   ro_bf_t fifo_brb0_0               : ( 15 - 0 + 1 );
   ro_bf_t fifo_brb0_1               : ( 31 - 16 + 1 );
} drex_r_fifo_info_p3_0_t;

typedef struct {
   ro_bf_t fifo_brb1_0               : ( 15 - 0 + 1 );
   ro_bf_t fifo_brb1_1               : ( 31 - 16 + 1 );
} drex_r_fifo_info_p3_1_t;

typedef struct {
   ro_bf_t fifo_brb0_0               : ( 15 - 0 + 1 );
   ro_bf_t fifo_brb0_1               : ( 31 - 16 + 1 );
} drex_w_fifo_info_p0_0_t;

typedef struct {
   ro_bf_t fifo_brb1_0               : ( 15 - 0 + 1 );
   ro_bf_t fifo_brb1_1               : ( 31 - 16 + 1 );
} drex_w_fifo_info_p0_1_t;

typedef struct {
   ro_bf_t fifo_brb0_0               : ( 15 - 0 + 1 );
   ro_bf_t fifo_brb0_1               : ( 31 - 16 + 1 );
} drex_w_fifo_info_p1_0_t;

typedef struct {
   ro_bf_t fifo_brb1_0               : ( 15 - 0 + 1 );
   ro_bf_t fifo_brb1_1               : ( 31 - 16 + 1 );
} drex_w_fifo_info_p1_1_t;

typedef struct {
   ro_bf_t fifo_brb0_0               : ( 15 - 0 + 1 );
   ro_bf_t fifo_brb0_1               : ( 31 - 16 + 1 );
} drex_w_fifo_info_p2_0_t;

typedef struct {
   ro_bf_t fifo_brb1_0               : ( 15 - 0 + 1 );
   ro_bf_t fifo_brb1_1               : ( 31 - 16 + 1 );
} drex_w_fifo_info_p2_1_t;

typedef struct {
   ro_bf_t fifo_brb0_0               : ( 15 - 0 + 1 );
   ro_bf_t fifo_brb0_1               : ( 31 - 16 + 1 );
} drex_w_fifo_info_p3_0_t;

typedef struct {
   ro_bf_t fifo_brb1_0               : ( 15 - 0 + 1 );
   ro_bf_t fifo_brb1_1               : ( 31 - 16 + 1 );
} drex_w_fifo_info_p3_1_t;

typedef struct {
   ro_bf_t fifo_brb1_0               : ( 15 - 0 + 1 );
   ro_bf_t fifo_brb1_1               : ( 31 - 16 + 1 );
} drex_c_fifo_info_p0_t;

typedef struct {
   ro_bf_t fifo_brb1_0               : ( 15 - 0 + 1 );
   ro_bf_t fifo_brb1_1               : ( 31 - 16 + 1 );
} drex_c_fifo_info_p1_t;

typedef struct {
   ro_bf_t fifo_brb1_0               : ( 15 - 0 + 1 );
   ro_bf_t fifo_brb1_1               : ( 31 - 16 + 1 );
} drex_c_fifo_info_p2_t;

typedef struct {
   ro_bf_t fifo_brb1_0               : ( 15 - 0 + 1 );
   ro_bf_t fifo_brb1_1               : ( 31 - 16 + 1 );
} drex_c_fifo_info_p3_t;

typedef struct {
   ro_bf_t r_brb0_0                  : ( 15 - 0 + 1 );
   ro_bf_t r_brb0_1                  : ( 31 - 16 + 1 );
} drex_r_brb0_1_0_axid_t;

typedef struct {
   ro_bf_t r_brb0_2                  : ( 15 - 0 + 1 );
   ro_bf_t r_brb0_3                  : ( 31 - 16 + 1 );
} drex_r_brb0_3_2_axid_t;

typedef struct {
   ro_bf_t r_brb0_4                  : ( 15 - 0 + 1 );
   ro_bf_t r_brb0_5                  : ( 31 - 16 + 1 );
} drex_r_brb0_5_4_axid_t;

typedef struct {
   ro_bf_t r_brb0_6                  : ( 15 - 0 + 1 );
   ro_bf_t r_brb0_7                  : ( 31 - 16 + 1 );
} drex_r_brb0_7_6_axid_t;

typedef struct {
   ro_bf_t r_brb1_0                  : ( 15 - 0 + 1 );
   ro_bf_t r_brb1_1                  : ( 31 - 16 + 1 );
} drex_r_brb1_1_0_axid_t;

typedef struct {
   ro_bf_t r_brb1_2                  : ( 15 - 0 + 1 );
   ro_bf_t r_brb1_3                  : ( 31 - 16 + 1 );
} drex_r_brb1_3_2_axid_t;

typedef struct {
   ro_bf_t r_brb1_4                  : ( 15 - 0 + 1 );
   ro_bf_t r_brb1_5                  : ( 31 - 16 + 1 );
} drex_r_brb1_5_4_axid_t;

typedef struct {
   ro_bf_t r_brb1_6                  : ( 15 - 0 + 1 );
   ro_bf_t r_brb1_7                  : ( 31 - 16 + 1 );
} drex_r_brb1_7_6_axid_t;

typedef struct {
   ro_bf_t w_brb0_0                  : ( 15 - 0 + 1 );
   ro_bf_t w_brb0_1                  : ( 31 - 16 + 1 );
} drex_w_brb0_1_0_axid_t;

typedef struct {
   ro_bf_t w_brb0_2                  : ( 15 - 0 + 1 );
   ro_bf_t w_brb0_3                  : ( 31 - 16 + 1 );
} drex_w_brb0_3_2_axid_t;

typedef struct {
   ro_bf_t w_brb0_4                  : ( 15 - 0 + 1 );
   ro_bf_t w_brb0_5                  : ( 31 - 16 + 1 );
} drex_w_brb0_5_4_axid_t;

typedef struct {
   ro_bf_t w_brb0_6                  : ( 15 - 0 + 1 );
   ro_bf_t w_brb0_7                  : ( 31 - 16 + 1 );
} drex_w_brb0_7_6_axid_t;

typedef struct {
   ro_bf_t w_brb1_0                  : ( 15 - 0 + 1 );
   ro_bf_t w_brb1_1                  : ( 31 - 16 + 1 );
} drex_w_brb1_1_0_axid_t;

typedef struct {
   ro_bf_t w_brb1_2                  : ( 15 - 0 + 1 );
   ro_bf_t w_brb1_3                  : ( 31 - 16 + 1 );
} drex_w_brb1_3_2_axid_t;

typedef struct {
   ro_bf_t w_brb1_4                  : ( 15 - 0 + 1 );
   ro_bf_t w_brb1_5                  : ( 31 - 16 + 1 );
} drex_w_brb1_5_4_axid_t;

typedef struct {
   ro_bf_t w_brb1_6                  : ( 15 - 0 + 1 );
   ro_bf_t w_brb1_7                  : ( 31 - 16 + 1 );
} drex_w_brb1_7_6_axid_t;

typedef struct {
   ro_bf_t phy_upd_req               : ( 0 - 0 + 1 );
   ro_bf_t phy_upd_ack               : ( 1 - 1 + 1 );
   ro_bf_t reserved_2_31                 : ( 31 - 2 + 1 );
} drex_phy_upd_state_t;

typedef struct {
   ro_bf_t dfi_rdlvl_gate_en         : ( 0 - 0 + 1 );
   ro_bf_t dfi_rdlvl_en              : ( 1 - 1 + 1 );
   ro_bf_t dfi_rdlvl_wr_en           : ( 2 - 2 + 1 );
   ro_bf_t dfi_lvl_periodic          : ( 3 - 3 + 1 );
   ro_bf_t dfi_train_csn             : ( 5 - 4 + 1 );
   ro_bf_t reserved_6_7                 : ( 7 - 6 + 1 );
   ro_bf_t train_state               : ( 16 - 8 + 1 );
   ro_bf_t reserved_17_31               : ( 31 - 17 + 1 );
} drex_train_state_t;

typedef struct {
   rw_bf_t ppc_enable                : ( 0 - 0 + 1 );
   rw_bf_t ppc_counter_reset         : ( 1 - 1 + 1 );
   rw_bf_t cc_reset                  : ( 2 - 2 + 1 );
   rw_bf_t cc_divider                : ( 3 - 3 + 1 );
   ro_bf_t rsvd1                     : ( 15 - 4 + 1 );
   rw_bf_t start_mode                : ( 16 - 16 + 1 );
   ro_bf_t reserved_17_31               : ( 31 - 17 + 1 );
} drex_pmnc_ppc_t;

typedef struct {
   rw_bf_t pmcnt0                    : ( 0 - 0 + 1 );
   rw_bf_t pmcnt1                    : ( 1 - 1 + 1 );
   rw_bf_t pmcnt2                    : ( 2 - 2 + 1 );
   rw_bf_t pmcnt3                    : ( 3 - 3 + 1 );
   ro_bf_t reserved_4_30                : ( 30 - 4 + 1 );
   rw_bf_t ccnt                      : ( 31 - 31 + 1 );
} drex_cntens_ppc_t;

typedef struct {
   rw_bf_t pmcnt0                    : ( 0 - 0 + 1 );
   rw_bf_t pmcnt1                    : ( 1 - 1 + 1 );
   rw_bf_t pmcnt2                    : ( 2 - 2 + 1 );
   rw_bf_t pmcnt3                    : ( 3 - 3 + 1 );
   ro_bf_t reserved_4_30                : ( 30 - 4 + 1 );
   rw_bf_t ccnt                      : ( 31 - 31 + 1 );
} drex_cntenc_ppc_t;

typedef struct {
   rw_bf_t pmcnt0                    : ( 0 - 0 + 1 );
   rw_bf_t pmcnt1                    : ( 1 - 1 + 1 );
   rw_bf_t pmcnt2                    : ( 2 - 2 + 1 );
   rw_bf_t pmcnt3                    : ( 3 - 3 + 1 );
   ro_bf_t reserved_4_30                : ( 30 - 4 + 1 );
   rw_bf_t ccnt                      : ( 31 - 31 + 1 );
} drex_intens_ppc_t;

typedef struct {
   rw_bf_t pmcnt0                    : ( 0 - 0 + 1 );
   rw_bf_t pmcnt1                    : ( 1 - 1 + 1 );
   rw_bf_t pmcnt2                    : ( 2 - 2 + 1 );
   rw_bf_t pmcnt3                    : ( 3 - 3 + 1 );
   ro_bf_t reserved_4_30                : ( 30 - 4 + 1 );
   rw_bf_t ccnt                      : ( 31 - 31 + 1 );
} drex_intenc_ppc_t;

typedef struct {
   rw_bf_t pmcnt0                    : ( 0 - 0 + 1 );
   rw_bf_t pmcnt1                    : ( 1 - 1 + 1 );
   rw_bf_t pmcnt2                    : ( 2 - 2 + 1 );
   rw_bf_t pmcnt3                    : ( 3 - 3 + 1 );
   ro_bf_t reserved_4_30                : ( 30 - 4 + 1 );
   rw_bf_t ccnt                      : ( 31 - 31 + 1 );
} drex_flag_ppc_t;

typedef struct {
   rw_bf_t ccnt                      : ( 31 - 0 + 1 );
} drex_ccnt_ppc_t;

typedef struct {
   rw_bf_t pmcnt                     : ( 31 - 0 + 1 );
} drex_pmcnt0_ppc_t;

typedef struct {
   rw_bf_t pmcnt                     : ( 31 - 0 + 1 );
} drex_pmcnt1_ppc_t;

typedef struct {
   rw_bf_t pmcnt                     : ( 31 - 0 + 1 );
} drex_pmcnt2_ppc_t;

typedef struct {
   rw_bf_t pmcnt                     : ( 31 - 0 + 1 );
} drex_pmcnt3_ppc_t;


typedef struct {
   drex_concontrol_t CONCONTROL;
   drex_memcontrol_t MEMCONTROL;
   drex_cgcontrol_t CGCONTROL;
   ro_bf_t reserved_0xC;
   drex_directcmd_t DIRECTCMD;
   drex_prechconfig0_t PRECHCONFIG0;
   drex_phycontrol0_t PHYCONTROL0;
   drex_prechconfig1_t PRECHCONFIG1;
   drex_timingrfcpb_t TIMINGRFCPB;
   drex_zqtiming_t ZQTIMING;
   drex_pwrdnconfig_t PWRDNCONFIG;
   ro_bf_t reserved_0x2C;
   drex_timingaref_t TIMINGAREF;
   drex_timingrow0_t TIMINGROW0;
   drex_timingdata0_t TIMINGDATA0;
   drex_timingpower0_t TIMINGPOWER0;
   drex_phystatus_t PHYSTATUS;
   drex_etctiming_t ETCTIMING;
   drex_chipstatus_t CHIPSTATUS;
   drex_rdfetch0_t RDFETCH0;
   drex_rdfetch1_t RDFETCH1;
   drex_mrstatus_t MRSTATUS;
   drex_etccontrol0_t ETCCONTROL0;
   drex_etccontrol1_t ETCCONTROL1;
   drex_qoscontroln_t QOSCONTROL[16];
   drex_timingsetsw_t TIMINGSETSW;
   drex_timingrow1_t TIMINGROW1;
   drex_timingdata1_t TIMINGDATA1;
   drex_timingpower1_t TIMINGPOWER1;
   ro_bf_t reserved_0xF0_0xFC[4];
   drex_brbrsvcontrol_t BRBRSVCONTROL;
   drex_brbrsvconfig_t BRBRSVCONFIG;
   drex_brbqosconfig_t BRBQOSCONFIG;
   drex_datarsvcontrol_t DATARSVCONTROL;
   drex_rdatarsvconfig_t RDATARSVCONFIG;
   drex_wdatarsvconfig_t WDATARSVCONFIG;
   ro_bf_t reserved_0x118_0x11C[2];
   drex_wrlvlconfig0_t WRLVLCONFIG0;
   drex_wrlvlconfig1_t WRLVLCONFIG1;
   drex_wrlvlstatus_t WRLVLSTATUS;
   ro_bf_t reserved_0x12C;
   drex_ppcclkcon_t PPCCLKCON;
   drex_perevconfig0_t PEREVCONFIG0;
   drex_perevconfig1_t PEREVCONFIG1;
   drex_perevconfig2_t PEREVCONFIG2;
   drex_perevconfig3_t PEREVCONFIG3;
   ro_bf_t reserved_0x144_0x14C[3];
   drex_ctrl_io_rdata_t CTRL_IO_RDATA;
   ro_bf_t reserved_0x154_0x1FC[43];
   drex_emergent_config0_t EMERGENT_CONFIG0;
   drex_emergent_config1_t EMERGENT_CONFIG1;
   ro_bf_t reserved_0x208_0x20C[2];
   drex_bp_control_set_t BPCON[4];
   drex_fifo_emer_config_t FIFO_EMER_CONFIG;
   ro_bf_t reserved_0x254_0x30C[47];
   drex_pl_config_t PL_CONFIG;
   ro_bf_t reserved_0x314_0x3FC[59];
   drex_all_int_indi_t ALL_INT_INDI;
   ro_bf_t reserved_0x404_0x40C[3];
   drex_train_timing0_t TRAIN_TIMING0;
   drex_train_timing1_t TRAIN_TIMING1;
   ro_bf_t reserved_0x418_0x41C[2];
   drex_hw_ptrain_period0_t HW_PTRAIN_PERIOD0;
   drex_hw_ptrain_period1_t HW_PTRAIN_PERIOD1;
   ro_bf_t reserved_0x428_0x42C[2];
   drex_inittrainconfig_t INITTRAINCONFIG;
   drex_inittraincontrol_t INITTRAINCONTROL;
   drex_swptrainconfig_t SWPTRAINCONFIG;
   drex_swptraincontrol_t SWPTRAINCONTROL;
   drex_hwptrainconfig0_t HWPTRAINCONFIG0;
   drex_hwptrainconfig1_t HWPTRAINCONFIG1;
   ro_bf_t reserved_0x448_0x44C[2];
   drex_hwptraincontrol0_t HWPTRAINCONTROL0;
   drex_hwptraincontrol1_t HWPTRAINCONTROL1;
   ro_bf_t reserved_0x458_0x45C[2];
   drex_wrtra_pattern0_t WRTRA_PATTERN0;
   drex_wrtra_pattern1_t WRTRA_PATTERN1;
   drex_wrtra_pattern2_t WRTRA_PATTERN2;
   ro_bf_t reserved_0x46C_0x4FC[37];
   drex_lpi_pause_state_t LPI_PAUSE_STATE;
   drex_empty_state_t EMPTY_STATE;
   drex_rbuf_occupancy_t RBUF_OCCUPANCY;
   drex_wbuf_occupancy_t WBUF_OCCUPANCY;
   drex_emergent_r_t EMERGENT_R;
   drex_emergent_w_t EMERGENT_W;
   drex_backpressure_t BACKPRESSURE;
   drex_rfifo_valid_state_t RFIFO_VALID_STATE;
   drex_wfifo_valid_state_t WFIFO_VALID_STATE;
   drex_cfifo_valid_state_t CFIFO_VALID_STATE;
   drex_r_brb_valid_state_t R_BRB_VALID_STATE;
   drex_w_brb_valid_state_t W_BRB_VALID_STATE;
   drex_info_config_t INFO_CONFIG;
   ro_bf_t reserved_0x534_0x53C[3];
   drex_r_fifo_info_p0_0_t R_FIFO_INFO_P0_0;
   drex_r_fifo_info_p0_1_t R_FIFO_INFO_P0_1;
   drex_r_fifo_info_p1_0_t R_FIFO_INFO_P1_0;
   drex_r_fifo_info_p1_1_t R_FIFO_INFO_P1_1;
   drex_r_fifo_info_p2_0_t R_FIFO_INFO_P2_0;
   drex_r_fifo_info_p2_1_t R_FIFO_INFO_P2_1;
   drex_r_fifo_info_p3_0_t R_FIFO_INFO_P3_0;
   drex_r_fifo_info_p3_1_t R_FIFO_INFO_P3_1;
   drex_w_fifo_info_p0_0_t W_FIFO_INFO_P0_0;
   drex_w_fifo_info_p0_1_t W_FIFO_INFO_P0_1;
   drex_w_fifo_info_p1_0_t W_FIFO_INFO_P1_0;
   drex_w_fifo_info_p1_1_t W_FIFO_INFO_P1_1;
   drex_w_fifo_info_p2_0_t W_FIFO_INFO_P2_0;
   drex_w_fifo_info_p2_1_t W_FIFO_INFO_P2_1;
   drex_w_fifo_info_p3_0_t W_FIFO_INFO_P3_0;
   drex_w_fifo_info_p3_1_t W_FIFO_INFO_P3_1;
   drex_c_fifo_info_p0_t C_FIFO_INFO_P0;
   drex_c_fifo_info_p1_t C_FIFO_INFO_P1;
   drex_c_fifo_info_p2_t C_FIFO_INFO_P2;
   drex_c_fifo_info_p3_t C_FIFO_INFO_P3;
   drex_r_brb0_1_0_axid_t R_BRB0_1_0_AXID;
   drex_r_brb0_3_2_axid_t R_BRB0_3_2_AXID;
   drex_r_brb0_5_4_axid_t R_BRB0_5_4_AXID;
   drex_r_brb0_7_6_axid_t R_BRB0_7_6_AXID;
   drex_r_brb1_1_0_axid_t R_BRB1_1_0_AXID;
   drex_r_brb1_3_2_axid_t R_BRB1_3_2_AXID;
   drex_r_brb1_5_4_axid_t R_BRB1_5_4_AXID;
   drex_r_brb1_7_6_axid_t R_BRB1_7_6_AXID;
   drex_w_brb0_1_0_axid_t W_BRB0_1_0_AXID;
   drex_w_brb0_3_2_axid_t W_BRB0_3_2_AXID;
   drex_w_brb0_5_4_axid_t W_BRB0_5_4_AXID;
   drex_w_brb0_7_6_axid_t W_BRB0_7_6_AXID;
   drex_w_brb1_1_0_axid_t W_BRB1_1_0_AXID;
   drex_w_brb1_3_2_axid_t W_BRB1_3_2_AXID;
   drex_w_brb1_5_4_axid_t W_BRB1_5_4_AXID;
   drex_w_brb1_7_6_axid_t W_BRB1_7_6_AXID;
   ro_bf_t reserved_0x5D0_0x6FC[76];
   drex_phy_upd_state_t PHY_UPD_STATE;
   drex_train_state_t TRAIN_STATE;
} drex_regs_t;

typedef struct {
   drex_pmnc_ppc_t PMNC_PPC;
   ro_bf_t reserved_0xE004_0xE00C[3];
   drex_cntens_ppc_t CNTENS_PPC;
   ro_bf_t reserved_0xE014_0xE01C[3];
   drex_cntenc_ppc_t CNTENC_PPC;
   ro_bf_t reserved_0xE024_0xE02C[3];
   drex_intens_ppc_t INTENS_PPC;
   ro_bf_t reserved_0xE034_0xE03C[3];
   drex_intenc_ppc_t INTENC_PPC;
   ro_bf_t reserved_0xE044_0xE04C[3];
   drex_flag_ppc_t FLAG_PPC;
   ro_bf_t reserved_0xE054_0xE0FC[43];
   drex_ccnt_ppc_t CCNT_PPC;
   ro_bf_t reserved_0xE104_0xE10C[3];
   drex_pmcnt0_ppc_t PMCNT0_PPC;
   ro_bf_t reserved_0xE114_0xE11C[3];
   drex_pmcnt1_ppc_t PMCNT1_PPC;
   ro_bf_t reserved_0xE124_0xE12C[3];
   drex_pmcnt2_ppc_t PMCNT2_PPC;
   ro_bf_t reserved_0xE134_0xE13C[3];
   drex_pmcnt3_ppc_t PMCNT3_PPC;
} drex_ppc_regs_t;

#endif

#endif
