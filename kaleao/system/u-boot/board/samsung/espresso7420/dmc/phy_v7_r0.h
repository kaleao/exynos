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

#ifndef PHY_V7_R0_H_
#define PHY_V7_R0_H_

#ifndef __ASSEMBLY__

typedef struct {
   rw_bf_t ctrl_rdlat                : ( 5 - 0 + 1 );
   rw_bf_t ctrl_cmosrcv              : ( 6 - 6 + 1 );
   rw_bf_t ctrl_ckdis                : ( 7 - 7 + 1 );
   rw_bf_t ctrl_bstlen               : ( 13 - 8 + 1 );
   rw_bf_t ctrl_otf_bl               : ( 14 - 14 + 1 );
   rw_bf_t ctrl_twpre                : ( 15 - 15 + 1 );
   rw_bf_t ctrl_wrlat                : ( 20 - 16 + 1 );
   ro_bf_t reserved_21_23               : ( 23 - 21 + 1 );
   rw_bf_t ctrl_ddr_mode             : ( 25 - 24 + 1 );
   rw_bf_t ctrl_dfdqs                : ( 26 - 26 + 1 );
   ro_bf_t reserved_27                  : ( 27 - 27 + 1 );
   rw_bf_t ctrl_upd_range            : ( 29 - 28 + 1 );
   rw_bf_t ctrl_upd_time             : ( 31 - 30 + 1 );
} phy_gnr_con0_t;

typedef struct {
   rw_bf_t wrlvl_mode                : ( 0 - 0 + 1 );
   rw_bf_t gate_cal_mode             : ( 1 - 1 + 1 );
   rw_bf_t ca_cal_mode               : ( 2 - 2 + 1 );
   rw_bf_t rd_cal_mode               : ( 3 - 3 + 1 );
   ro_bf_t reserved_4                : ( 4 - 4 + 1 );
   rw_bf_t wr_cal_mode               : ( 5 - 5 + 1 );
   ro_bf_t reserved_6_7              : ( 7 - 6 + 1 );
   rw_bf_t wrlvl_start               : ( 8 - 8 + 1 );
   ro_bf_t reserved_9_15             : ( 15 - 9 + 1 );
   ro_bf_t wrlvl_rep                 : ( 16 - 16 + 1 );
   ro_bf_t reserved_17_18            : ( 18 - 17 + 1 );
   rw_bf_t ctrl_dqs_osc_en           : ( 19 - 19 + 1 );
   rw_bf_t byte_rdlvl_en             : ( 20 - 20 + 1 );
   rw_bf_t ca_swap_mode              : ( 21 - 21 + 1 );
   rw_bf_t cal_vtc_en                : ( 22 - 22 + 1 );
   rw_bf_t freq_offset_en            : ( 23 - 23 + 1 );
   rw_bf_t dgate_en                  : ( 24 - 24 + 1 );
   rw_bf_t fastdeskewen              : ( 25 - 25 + 1 );
   ro_bf_t reserved_26               : ( 26 - 26 + 1 );
   rw_bf_t gate_rdchk_en             : ( 27 - 27 + 1 );
   rw_bf_t fine_train_en             : ( 28 - 28 + 1 );
   rw_bf_t wr_per_rank_en            : ( 29 - 29 + 1 );
   rw_bf_t wr_cs_n_default           : ( 31 - 30 + 1 );
} phy_cal_con0_t;

typedef struct {
   rw_bf_t rdlvl_periodic_incr_adj   : ( 6 - 0 + 1 );
   ro_bf_t reserved_7                   : ( 7 - 7 + 1 );
   rw_bf_t glvl_periodic_incr_adj    : ( 14 - 8 + 1 );
   ro_bf_t reserved_15                  : ( 15 - 15 + 1 );
   rw_bf_t rdlvl_pass_adj            : ( 19 - 16 + 1 );
   rw_bf_t rdlvl_incr_adj            : ( 26 - 20 + 1 );
   ro_bf_t reserved_27                  : ( 27 - 27 + 1 );
   rw_bf_t rdlvl_start_adj           : ( 31 - 28 + 1 );
} phy_cal_con1_t;

typedef struct {
   ro_bf_t reserved_0_11                : ( 11 - 0 + 1 );
   rw_bf_t ctrl_readadj              : ( 15 - 12 + 1 );
   rw_bf_t ctrl_readduradj           : ( 19 - 16 + 1 );
   rw_bf_t ctrl_gateadj              : ( 23 - 20 + 1 );
   rw_bf_t ctrl_gateduradj           : ( 27 - 24 + 1 );
   ro_bf_t reserved_28                  : ( 28 - 28 + 1 );
   rw_bf_t ctrl_shgate               : ( 29 - 29 + 1 );
   rw_bf_t ctrl_rodt_disable         : ( 30 - 30 + 1 );
   rw_bf_t ctrl_rpre_opt             : ( 31 - 31 + 1 );
} phy_cal_con2_t;

typedef struct {
   rw_bf_t pcfg_mode                 : ( 2 - 0 + 1 );
   ro_bf_t reserved_3_7                 : ( 7 - 3 + 1 );
   rw_bf_t rd_sw_mode                : ( 8 - 8 + 1 );
   rw_bf_t wr_sw_mode                : ( 9 - 9 + 1 );
   ro_bf_t reserved_10_11               : ( 11 - 10 + 1 );
   rw_bf_t upd_ack_cycle             : ( 14 - 12 + 1 );
   ro_bf_t reserved_15                  : ( 15 - 15 + 1 );
   rw_bf_t upd_req_cycle             : ( 19 - 16 + 1 );
   rw_bf_t dvfs_wait_cycle           : ( 23 - 20 + 1 );
   rw_bf_t phyupd_req_cycle          : ( 28 - 24 + 1 );
   ro_bf_t reserved_29_31               : ( 31 - 29 + 1 );
} phy_cal_con3_t;

typedef struct {
   rw_bf_t ctrl_wrlat_plus0          : ( 2 - 0 + 1 );
   rw_bf_t ctrl_wrlat_plus1          : ( 5 - 3 + 1 );
   rw_bf_t ctrl_wrlat_plus2          : ( 8 - 6 + 1 );
   rw_bf_t ctrl_wrlat_plus3          : ( 11 - 9 + 1 );
   rw_bf_t ctrl_wrlat_plus4          : ( 14 - 12 + 1 );
   rw_bf_t ctrl_wrlat_plus5          : ( 17 - 15 + 1 );
   rw_bf_t ctrl_wrlat_plus6          : ( 20 - 18 + 1 );
   rw_bf_t ctrl_wrlat_plus7          : ( 23 - 21 + 1 );
   rw_bf_t ctrl_wrlat_plus8          : ( 26 - 24 + 1 );
   rw_bf_t ctrl_wrlat_plus9          : ( 27 - 27 + 1 );
   ro_bf_t reserved_28_31               : ( 31 - 28 + 1 );
} phy_wlat_con0_t;

typedef struct {
   rw_bf_t ctrl_pulld_dqs            : ( 10 - 0 + 1 );
   rw_bf_t mdll_cg_en            : ( 11 - 11 + 1 );
   rw_bf_t pcl_pd                    : ( 12 - 12 + 1 );
   rw_bf_t ds_pd                     : ( 13 - 13 + 1 );
   rw_bf_t ds_io_pd                  : ( 14 - 14 + 1 );
   rw_bf_t cs_io_pd                  : ( 15 - 15 + 1 );
   ro_bf_t reserved_16_31                : ( 31 - 16 + 1 );
} phy_lp_con0_t;

typedef struct {
   rw_bf_t rank0_dsen                : ( 1 - 0 + 1 );
   ro_bf_t reserved_2_11                : ( 11 - 2 + 1 );
   rw_bf_t rank1_dsen                : ( 13 - 12 + 1 );
   ro_bf_t reserved_14_23               : ( 23 - 14 + 1 );
   rw_bf_t rank_en                   : ( 25 - 24 + 1 );
   ro_bf_t reserved_26_31               : ( 31 - 26 + 1 );
} phy_gate_con0_t;

typedef struct {
   rw_bf_t ctrl_offsetr0			: ( 7 - 0 + 1 );
   rw_bf_t ctrl_offsetr1			: ( 15 - 8 + 1 );
   rw_bf_t ctrl_offsetr2			: ( 23 - 16 + 1 );
   rw_bf_t ctrl_offsetr3			: ( 31 - 24 + 1 );
} phy_offsetr_con0_t;

typedef struct {
   ro_bf_t reserved_0_7         : ( 7 - 0 + 1 );
   rw_bf_t ctrl_offsetr5             : ( 15 - 8 + 1 );
   rw_bf_t ctrl_offsetr6             : ( 23 - 16 + 1 );
   rw_bf_t ctrl_offsetr7             : ( 31 - 24 + 1 );
} phy_offsetr_con1_t;

typedef struct {
   rw_bf_t ctrl_offsetr8             : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_offsetr_con2_t;

typedef struct {
   rw_bf_t ctrl_dq_lat_ds0           : ( 0 - 0 + 1 );
   rw_bf_t ctrl_dq_lat_ds1           : ( 1 - 1 + 1 );
   rw_bf_t ctrl_dq_lat_ds2           : ( 2 - 2 + 1 );
   rw_bf_t ctrl_dq_lat_ds3           : ( 3 - 3 + 1 );
   rw_bf_t ctrl_dq_lat_ds4           : ( 4 - 4 + 1 );
   rw_bf_t ctrl_dq_lat_ds5           : ( 5 - 5 + 1 );
   rw_bf_t ctrl_dq_lat_ds6           : ( 6 - 6 + 1 );
   rw_bf_t ctrl_dq_lat_ds7           : ( 7 - 7 + 1 );
   rw_bf_t ctrl_dq_lat_ds8           : ( 8 - 8 + 1 );
   ro_bf_t reserved_9_31                : ( 31 - 9 + 1 );
} phy_dqlat_con0_t;

typedef struct {
   rw_bf_t ctrl_offsetw0             : ( 7 - 0 + 1 );
   rw_bf_t ctrl_offsetw1             : ( 15 - 8 + 1 );
   rw_bf_t ctrl_offsetw2             : ( 23 - 16 + 1 );
   rw_bf_t ctrl_offsetw3             : ( 31 - 24 + 1 );
} phy_offsetw_con0_t;

typedef struct {
   rw_bf_t ctrl_offsetw4             : ( 7 - 0 + 1 );
   rw_bf_t ctrl_offsetw5             : ( 15 - 8 + 1 );
   rw_bf_t ctrl_offsetw6             : ( 23 - 16 + 1 );
   rw_bf_t ctrl_offsetw7             : ( 31 - 24 + 1 );
} phy_offsetw_con1_t;

typedef struct {
   rw_bf_t ctrl_offsetw8             : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_offsetw_con2_t;

typedef struct {
   rw_bf_t ctrl_offsetr_dq           : ( 5 - 0 + 1 );
   ro_bf_t reserved_6_31                : ( 31 - 6 + 1 );
} phy_offset_dq_con0_t;

typedef struct {
   rw_bf_t ctrl_offsetc0             : ( 7 - 0 + 1 );
   rw_bf_t ctrl_offsetc1             : ( 15 - 8 + 1 );
   rw_bf_t ctrl_offsetc2             : ( 23 - 16 + 1 );
   rw_bf_t ctrl_offsetc3             : ( 31 - 24 + 1 );
} phy_offsetc_con0_t;

typedef struct {
   rw_bf_t ctrl_offsetc4             : ( 7 - 0 + 1 );
   rw_bf_t ctrl_offsetc5             : ( 15 - 8 + 1 );
   rw_bf_t ctrl_offsetc6             : ( 23 - 16 + 1 );
   rw_bf_t ctrl_offsetc7             : ( 31 - 24 + 1 );
} phy_offsetc_con1_t;

typedef struct {
   rw_bf_t ctrl_offsetc8             : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_offsetc_con2_t;

typedef struct {
   rw_bf_t ctrl_shiftc0              : ( 2 - 0 + 1 );
   rw_bf_t ctrl_shiftc1              : ( 5 - 3 + 1 );
   rw_bf_t ctrl_shiftc2              : ( 8 - 6 + 1 );
   rw_bf_t ctrl_shiftc3              : ( 11 - 9 + 1 );
   rw_bf_t ctrl_shiftc4              : ( 14 - 12 + 1 );
   rw_bf_t ctrl_shiftc5              : ( 17 - 15 + 1 );
   rw_bf_t ctrl_shiftc6              : ( 20 - 18 + 1 );
   rw_bf_t ctrl_shiftc7              : ( 23 - 21 + 1 );
   rw_bf_t ctrl_shiftc8              : ( 26 - 24 + 1 );
   ro_bf_t reserved_27_31               : ( 31 - 27 + 1 );
} phy_shiftc_con0_t;

typedef struct {
   rw_bf_t ctrl_offsetd              : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_23                : ( 23 - 8 + 1 );
   rw_bf_t ctrl_resync               : ( 24 - 24 + 1 );
   ro_bf_t reserved_25_27               : ( 27 - 25 + 1 );
   rw_bf_t upd_mode                  : ( 28 - 28 + 1 );
   ro_bf_t reserved_29_31               : ( 31 - 29 + 1 );
} phy_offsetd_con0_t;

typedef struct {
   rw_bf_t ctrl_offseto0             : ( 7 - 0 + 1 );
   rw_bf_t ctrl_offseto1             : ( 15 - 8 + 1 );
   rw_bf_t ctrl_offseto2             : ( 23 - 16 + 1 );
   rw_bf_t ctrl_offseto3             : ( 31 - 24 + 1 );
} phy_offseto_con0_t;

typedef struct {
   rw_bf_t ctrl_offseto4             : ( 7 - 0 + 1 );
   rw_bf_t ctrl_offseto5             : ( 15 - 8 + 1 );
   rw_bf_t ctrl_offseto6             : ( 23 - 16 + 1 );
   rw_bf_t ctrl_offseto7             : ( 31 - 24 + 1 );
} phy_offseto_con1_t;

typedef struct {
   ro_bf_t reserved_0_23                : ( 23 - 0 + 1 );
   rw_bf_t ctrl_offseto8             : ( 31 - 24 + 1 );
} phy_offseto_con2_t;

typedef struct {
   rw_bf_t ctrl_wrlvl0_code          : ( 7 - 0 + 1 );
   rw_bf_t ctrl_wrlvl1_code          : ( 15 - 8 + 1 );
   rw_bf_t ctrl_wrlvl2_code          : ( 23 - 16 + 1 );
   rw_bf_t ctrl_wrlvl3_code          : ( 31 - 24 + 1 );
} phy_wr_lvl_con0_t;

typedef struct {
   rw_bf_t ctrl_wrlvl4_code          : ( 7 - 0 + 1 );
   rw_bf_t ctrl_wrlvl5_code          : ( 15 - 8 + 1 );
   rw_bf_t ctrl_wrlvl6_code          : ( 23 - 16 + 1 );
   rw_bf_t ctrl_wrlvl7_code          : ( 31 - 24 + 1 );
} phy_wr_lvl_con1_t;

typedef struct {
   rw_bf_t ctrl_wrlvl8_code          : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_wr_lvl_con2_t;

typedef struct {
   rw_bf_t ctrl_wrlvl_resync         : ( 0 - 0 + 1 );
   ro_bf_t reserved_1_31                : ( 31 - 1 + 1 );
} phy_wr_lvl_con3_t;

typedef struct {
   rw_bf_t ca0deskewcode             : ( 7 - 0 + 1 );
   rw_bf_t ca1deskewcode             : ( 15 - 8 + 1 );
   rw_bf_t ca2deskewcode             : ( 23 - 16 + 1 );
   rw_bf_t ca3deskewcode             : ( 31 - 24 + 1 );
} phy_ca_dskew_con0_t;

typedef struct {
   rw_bf_t ca4deskewcode             : ( 7 - 0 + 1 );
   rw_bf_t ca5deskewcode             : ( 15 - 8 + 1 );
   rw_bf_t ckdeskewcode              : ( 23 - 16 + 1 );
   rw_bf_t cs0deskewcode             : ( 31 - 24 + 1 );
} phy_ca_dskew_con1_t;

typedef struct {
   rw_bf_t cs1deskewcode             : ( 7 - 0 + 1 );
   rw_bf_t cke0deskewcode            : ( 15 - 8 + 1 );
   rw_bf_t cke1deskewcode            : ( 23 - 16 + 1 );
   rw_bf_t rsvd0                     : ( 31 - 24 + 1 );
} phy_ca_dskew_con2_t;

typedef struct {
   rw_bf_t rstdeskewcode             : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_ca_dskew_con4_t;

typedef struct {
   rw_bf_t wrtrn_rddata_adj_w1       : ( 15 - 0 + 1 );
   rw_bf_t wrtrn_rddata_adj_w0       : ( 31 - 16 + 1 );
} phy_cal_pattern_con0_t;

typedef struct {
   rw_bf_t wrtrn_rddata_adj_w3       : ( 15 - 0 + 1 );
   rw_bf_t wrtrn_rddata_adj_w2       : ( 31 - 16 + 1 );
} phy_cal_pattern_con1_t;

typedef struct {
   rw_bf_t wrtrn_rddata_dm_adj_w3    : ( 1 - 0 + 1 );
   rw_bf_t wrtrn_rddata_dm_adj_w2    : ( 3 - 2 + 1 );
   rw_bf_t wrtrn_rddata_dm_adj_w1    : ( 5 - 4 + 1 );
   rw_bf_t wrtrn_rddata_dm_adj_w0    : ( 7 - 6 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_cal_pattern_con2_t;

typedef struct {
   rw_bf_t rdtrn_rddata_adj_w1       : ( 15 - 0 + 1 );
   rw_bf_t rdtrn_rddata_adj_w0       : ( 31 - 16 + 1 );
} phy_cal_pattern_con3_t;

typedef struct {
   rw_bf_t rdtrn_rddata_adj_w3       : ( 15 - 0 + 1 );
   rw_bf_t rdtrn_rddata_adj_w2       : ( 31 - 16 + 1 );
} phy_cal_pattern_con4_t;

typedef struct {
   rw_bf_t rdtrn_rddata_dm_adj_w3    : ( 1 - 0 + 1 );
   rw_bf_t rdtrn_rddata_dm_adj_w2    : ( 3 - 2 + 1 );
   rw_bf_t rdtrn_rddata_dm_adj_w1    : ( 5 - 4 + 1 );
   rw_bf_t rdtrn_rddata_dm_adj_w0    : ( 7 - 6 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_cal_pattern_con5_t;

typedef struct {
   rw_bf_t ctrl_half                 : ( 0 - 0 + 1 );
   rw_bf_t ctrl_ref                  : ( 4 - 1 + 1 );
   rw_bf_t ctrl_dll_on               : ( 5 - 5 + 1 );
   rw_bf_t ctrl_start                : ( 6 - 6 + 1 );
   rw_bf_t ctrl_force                : ( 15 - 7 + 1 );
   rw_bf_t ctrl_inc                  : ( 22 - 16 + 1 );
   ro_bf_t reserved_23               : ( 23 - 23 + 1 );
   rw_bf_t ctrl_start_point          : ( 30 - 24 + 1 );
   ro_bf_t reserved_31               : ( 31 - 31 + 1 );
} phy_mdll_con0_t;

typedef struct {
   ro_bf_t ctrl_locked               : ( 0 - 0 + 1 );
   ro_bf_t ctrl_flock                : ( 1 - 1 + 1 );
   ro_bf_t ctrl_clock                : ( 2 - 2 + 1 );
   ro_bf_t harmonic_lock             : ( 3 - 3 + 1 );
   ro_bf_t reserved_4_7              : ( 7 - 4 + 1 );
   ro_bf_t ctrl_lock_value           : ( 16 - 8 + 1 );
   ro_bf_t reserved_17_21            : ( 21 - 17 + 1 );
   rw_bf_t lock_value_init_override  : ( 22 - 22 + 1 );
   ro_bf_t ctrl_lock_value_init      : ( 31 - 23 + 1 );
} phy_mdll_con1_t;

typedef struct {
   rw_bf_t dvfs0_offset              : ( 7 - 0 + 1 );
   rw_bf_t dvfs1_offset              : ( 15 - 8 + 1 );
   rw_bf_t dvfs0_dqs_osc_en          : ( 16 - 16 + 1 );
   rw_bf_t dvfs1_dqs_osc_en          : ( 17 - 17 + 1 );
   rw_bf_t dvfs0_half                : ( 18 - 18 + 1 );
   rw_bf_t dvfs1_half                : ( 19 - 19 + 1 );
   rw_bf_t dvfs0_dll_on              : ( 20 - 20 + 1 );
   rw_bf_t dvfs1_dll_on              : ( 21 - 21 + 1 );
   rw_bf_t dvfs0_rodt_disable        : ( 22 - 22 + 1 );
   rw_bf_t dvfs1_rodt_disable        : ( 23 - 23 + 1 );
   rw_bf_t dvfs_mode                 : ( 25 - 24 + 1 );
   ro_bf_t reserved_26_27               : ( 27 - 26 + 1 );
   rw_bf_t dvfs0_dgatelvl_en         : ( 28 - 28 + 1 );
   rw_bf_t dvfs1_dgatelvl_en         : ( 29 - 29 + 1 );
   rw_bf_t dvfs0_wr_per_rank_en      : ( 30 - 30 + 1 );
   rw_bf_t dvfs1_wr_per_rank_en      : ( 31 - 31 + 1 );
} phy_dvfs_con0_t;

typedef struct {
   rw_bf_t dvfs0_gateadj             : ( 3 - 0 + 1 );
   rw_bf_t dvfs0_gateduradj          : ( 7 - 4 + 1 );
   rw_bf_t dvfs1_gateadj             : ( 11 - 8 + 1 );
   rw_bf_t dvfs1_gateduradj          : ( 15 - 12 + 1 );
   rw_bf_t dvfs0_rdat                : ( 21 - 16 + 1 );
   ro_bf_t reserved_22_23            : ( 23 - 22 + 1 );
   rw_bf_t dvfs1_rdat                : ( 29 - 24 + 1 );
   rw_bf_t dvfs0_cmosrcv             : ( 30 - 30 + 1 );
   rw_bf_t dvfs1_cmosrcv             : ( 31 - 31 + 1 );
} phy_dvfs_con1_t;

typedef struct {
   rw_bf_t freq_train                : ( 11 - 0 + 1 );
   ro_bf_t reserved_12_17            : ( 17 - 12 + 1 );
   rw_bf_t dvfs0_start_point         : ( 24 - 18 + 1 );
   rw_bf_t dvfs1_start_point         : ( 31 - 25 + 1 );
} phy_dvfs_con2_t;

typedef struct {
   rw_bf_t dvfs0_freq                : ( 11 - 0 + 1 );
   rw_bf_t dvfs1_freq                : ( 23 - 12 + 1 );
   ro_bf_t reserved_24_25               : ( 25 - 24 + 1 );
   rw_bf_t dvfs0_shgate              : ( 26 - 26 + 1 );
   rw_bf_t dvfs1_shgate              : ( 27 - 27 + 1 );
   rw_bf_t dvfs0_rpre_opt            : ( 28 - 28 + 1 );
   rw_bf_t dvfs1_rpre_opt            : ( 29 - 29 + 1 );
   rw_bf_t dvfs0_twpre               : ( 30 - 30 + 1 );
   rw_bf_t dvfs1_twpre               : ( 31 - 31 + 1 );
} phy_dvfs_con3_t;

typedef struct {
   rw_bf_t dvfs0_ds0_vref            : ( 5 - 0 + 1 );
   rw_bf_t dvfs0_ds1_vref            : ( 11 - 6 + 1 );
   rw_bf_t dvfs1_ds0_vref            : ( 17 - 12 + 1 );
   rw_bf_t dvfs1_ds1_vref            : ( 23 - 18 + 1 );
   ro_bf_t reserved_24_30               : ( 30 - 24 + 1 );
   rw_bf_t dvfs_fsbst_en             : ( 31 - 31 + 1 );
} phy_dvfs_con4_t;

typedef struct {
   rw_bf_t dvfs0_pulld_dqs           : ( 3 - 0 + 1 );
   rw_bf_t dvfs1_pulld_dqs           : ( 7 - 4 + 1 );
   rw_bf_t dvfs0_readadj             : ( 11 - 8 + 1 );
   rw_bf_t dvfs1_readadj             : ( 15 - 12 + 1 );
   rw_bf_t dvfs0_readduradj          : ( 19 - 16 + 1 );
   rw_bf_t dvfs1_readduradj          : ( 23 - 20 + 1 );
   ro_bf_t reserved_24_31               : ( 31 - 24 + 1 );
} phy_dvfs_con5_t;

typedef struct {
   ro_bf_t t0_rdata_en               : ( 5 - 0 + 1 );
   ro_bf_t t1_rdata_en               : ( 11 - 6 + 1 );
   ro_bf_t t2_rdata_en               : ( 17 - 12 + 1 );
   ro_bf_t t3_rdata_en               : ( 23 - 18 + 1 );
   ro_bf_t t_rddata_en               : ( 28 - 24 + 1 );
   ro_bf_t reserved_29_31               : ( 31 - 29 + 1 );
} phy_t_rddata_con0_t;

typedef struct {
   ro_bf_t t4_rdata_en               : ( 5 - 0 + 1 );
   ro_bf_t t5_rdata_en               : ( 11 - 6 + 1 );
   ro_bf_t t6_rdata_en               : ( 17 - 12 + 1 );
   ro_bf_t t7_rdata_en               : ( 23 - 18 + 1 );
   ro_bf_t reserved_24_31               : ( 31 - 24 + 1 );
} phy_t_rddata_con1_t;

typedef struct {
   ro_bf_t t8_rdata_en               : ( 5 - 0 + 1 );
   ro_bf_t reserved_6_31                : ( 31 - 6 + 1 );
} phy_t_rddata_con2_t;

typedef struct {
   ro_bf_t dq_fail_status_sl0        : ( 7 - 0 + 1 );
   ro_bf_t dq_fail_status_sl1        : ( 15 - 8 + 1 );
   ro_bf_t dq_fail_status_sl2        : ( 23 - 16 + 1 );
   ro_bf_t dq_fail_status_sl3        : ( 31 - 24 + 1 );
} phy_cal_fail_stat0_t;

typedef struct {
   ro_bf_t dq_fail_status_sl4        : ( 7 - 0 + 1 );
   ro_bf_t dq_fail_status_sl5        : ( 15 - 8 + 1 );
   ro_bf_t dq_fail_status_sl6        : ( 23 - 16 + 1 );
   ro_bf_t dq_fail_status_sl7        : ( 31 - 24 + 1 );
} phy_cal_fail_stat1_t;

typedef struct {
   ro_bf_t dq_fail_status_sl8        : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_cal_fail_stat2_t;

typedef struct {
   ro_bf_t dm_fail_status            : ( 8 - 0 + 1 );
   ro_bf_t reserved_9_31                : ( 31 - 9 + 1 );
} phy_cal_fail_stat3_t;

typedef struct {
   ro_bf_t gt_vwmc0_cs0              : ( 7 - 0 + 1 );
   ro_bf_t gt_vwmc1_cs0              : ( 15 - 8 + 1 );
   ro_bf_t gt_vwmc2_cs0              : ( 23 - 16 + 1 );
   ro_bf_t gt_vwmc3_cs0              : ( 31 - 24 + 1 );
} phy_cal_gt_wvmc0_t;

typedef struct {
   ro_bf_t gt_vwmc4_cs0              : ( 7 - 0 + 1 );
   ro_bf_t gt_vwmc5_cs0              : ( 15 - 8 + 1 );
   ro_bf_t gt_vwmc6_cs0              : ( 23 - 16 + 1 );
   ro_bf_t gt_vwmc7_cs0              : ( 31 - 24 + 1 );
} phy_cal_gt_wvmc1_t;

typedef struct {
   ro_bf_t gt_vwmc8_cs0              : ( 7 - 0 + 1 );
   ro_bf_t rsvd0                     : ( 31 - 8 + 1 );
} phy_cal_gt_wvmc2_t;

typedef struct {
   ro_bf_t gt_cyc0_cs0               : ( 2 - 0 + 1 );
   ro_bf_t reserved_3_7                 : ( 7 - 3 + 1 );
   ro_bf_t gt_cyc1_cs0               : ( 10 - 8 + 1 );
   ro_bf_t reserved_11_31               : ( 31 - 11 + 1 );
} phy_cal_gt_cyc_t;

typedef struct {
   ro_bf_t rd_vwmc0                  : ( 7 - 0 + 1 );
   ro_bf_t rd_vwmc1                  : ( 15 - 8 + 1 );
   ro_bf_t rd_vwmc2                  : ( 23 - 16 + 1 );
   ro_bf_t rd_vwmc3                  : ( 31 - 24 + 1 );
} phy_cal_rd_vwmc0_t;

typedef struct {
   ro_bf_t rd_vwmc4                  : ( 7 - 0 + 1 );
   ro_bf_t rd_vwmc5                  : ( 15 - 8 + 1 );
   ro_bf_t rd_vwmc6                  : ( 23 - 16 + 1 );
   ro_bf_t rd_vwmc7                  : ( 31 - 24 + 1 );
} phy_cal_rd_vwmc1_t;

typedef struct {
   ro_bf_t rd_vwmc8                  : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_cal_rd_vwmc2_t;

typedef struct {
   ro_bf_t rd_vwml0                  : ( 7 - 0 + 1 );
   ro_bf_t rd_vwml1                  : ( 15 - 8 + 1 );
   ro_bf_t rd_vwml2                  : ( 23 - 16 + 1 );
   ro_bf_t rd_vwml3                  : ( 31 - 24 + 1 );
} phy_cal_rd_vwml0_t;

typedef struct {
   ro_bf_t rd_vwml4                  : ( 7 - 0 + 1 );
   ro_bf_t rd_vwml5                  : ( 15 - 8 + 1 );
   ro_bf_t rd_vwml6                  : ( 23 - 16 + 1 );
   ro_bf_t rd_vwml7                  : ( 31 - 24 + 1 );
} phy_cal_rd_vwml1_t;

typedef struct {
   ro_bf_t rd_vwmc8                  : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_cal_rd_vwml2_t;

typedef struct {
   ro_bf_t rd_vwmr0                  : ( 7 - 0 + 1 );
   ro_bf_t rd_vwmr1                  : ( 15 - 8 + 1 );
   ro_bf_t rd_vwmr2                  : ( 23 - 16 + 1 );
   ro_bf_t rd_vwmr3                  : ( 31 - 24 + 1 );
} phy_cal_rd_vwmr0_t;

typedef struct {
   ro_bf_t rd_vwmr4                  : ( 7 - 0 + 1 );
   ro_bf_t rd_vwmr5                  : ( 15 - 8 + 1 );
   ro_bf_t rd_vwmr6                  : ( 23 - 16 + 1 );
   ro_bf_t rd_vwmr7                  : ( 31 - 24 + 1 );
} phy_cal_rd_vwmr1_t;

typedef struct {
   ro_bf_t rd_vwmr8                  : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_cal_rd_vwmr2_t;

typedef struct {
   ro_bf_t wr_vwmc0                  : ( 7 - 0 + 1 );
   ro_bf_t wr_vwmc1                  : ( 15 - 8 + 1 );
   ro_bf_t wr_vwmc2                  : ( 23 - 16 + 1 );
   ro_bf_t wr_vwmc3                  : ( 31 - 24 + 1 );
} phy_cal_wr_vwmc0_t;

typedef struct {
   ro_bf_t wr_vwmc4                  : ( 7 - 0 + 1 );
   ro_bf_t wr_vwmc5                  : ( 15 - 8 + 1 );
   ro_bf_t wr_vwmc6                  : ( 23 - 16 + 1 );
   ro_bf_t wr_vwmc7                  : ( 31 - 24 + 1 );
} phy_cal_wr_vwmc1_t;

typedef struct {
   ro_bf_t wr_vwmc8                  : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_cal_wr_vwmc2_t;

typedef struct {
   ro_bf_t wr_vwml0                  : ( 7 - 0 + 1 );
   ro_bf_t wr_vwml1                  : ( 15 - 8 + 1 );
   ro_bf_t wr_vwml2                  : ( 23 - 16 + 1 );
   ro_bf_t wr_vwml3                  : ( 31 - 24 + 1 );
} phy_cal_wr_vwml0_t;

typedef struct {
   ro_bf_t wr_vwml4                  : ( 7 - 0 + 1 );
   ro_bf_t wr_vwml5                  : ( 15 - 8 + 1 );
   ro_bf_t wr_vwml6                  : ( 23 - 16 + 1 );
   ro_bf_t wr_vwml7                  : ( 31 - 24 + 1 );
} phy_cal_wr_vwml1_t;

typedef struct {
   ro_bf_t wr_vwml8                  : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_cal_wr_vwml2_t;

typedef struct {
   ro_bf_t wr_vwmr0                  : ( 7 - 0 + 1 );
   ro_bf_t wr_vwmr1                  : ( 15 - 8 + 1 );
   ro_bf_t wr_vwmr2                  : ( 23 - 16 + 1 );
   ro_bf_t wr_vwmr3                  : ( 31 - 24 + 1 );
} phy_cal_wr_vwmr0_t;

typedef struct {
   ro_bf_t wr_vwmr4                  : ( 7 - 0 + 1 );
   ro_bf_t wr_vwmr5                  : ( 15 - 8 + 1 );
   ro_bf_t wr_vwmr6                  : ( 23 - 16 + 1 );
   ro_bf_t wr_vwmr7                  : ( 31 - 24 + 1 );
} phy_cal_wr_vwmr1_t;

typedef struct {
   ro_bf_t wr_vwmr8                  : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_cal_wr_vwmr2_t;

typedef struct {
   ro_bf_t wr_vwmc0                  : ( 7 - 0 + 1 );
   ro_bf_t wr_vwmc1                  : ( 15 - 8 + 1 );
   ro_bf_t wr_vwmc2                  : ( 23 - 16 + 1 );
   ro_bf_t wr_vwmc3                  : ( 31 - 24 + 1 );
} phy_cal_dm_vwmc0_t;

typedef struct {
   ro_bf_t wr_vwmc4                  : ( 7 - 0 + 1 );
   ro_bf_t wr_vwmc5                  : ( 15 - 8 + 1 );
   ro_bf_t wr_vwmc6                  : ( 23 - 16 + 1 );
   ro_bf_t wr_vwmc7                  : ( 31 - 24 + 1 );
} phy_cal_dm_vwmc1_t;

typedef struct {
   ro_bf_t wr_vwmc8                  : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_cal_dm_vwmc2_t;

typedef struct {
   ro_bf_t wr_vwml0                  : ( 7 - 0 + 1 );
   ro_bf_t wr_vwml1                  : ( 15 - 8 + 1 );
   ro_bf_t wr_vwml2                  : ( 23 - 16 + 1 );
   ro_bf_t wr_vwml3                  : ( 31 - 24 + 1 );
} phy_cal_dm_vwml0_t;

typedef struct {
   ro_bf_t wr_vwml4                  : ( 7 - 0 + 1 );
   ro_bf_t wr_vwml5                  : ( 15 - 8 + 1 );
   ro_bf_t wr_vwml6                  : ( 23 - 16 + 1 );
   ro_bf_t wr_vwml7                  : ( 31 - 24 + 1 );
} phy_cal_dm_vwml1_t;

typedef struct {
   ro_bf_t wr_vwml8                  : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_cal_dm_vwml2_t;

typedef struct {
   ro_bf_t wr_vwmr0                  : ( 7 - 0 + 1 );
   ro_bf_t wr_vwmr1                  : ( 15 - 8 + 1 );
   ro_bf_t wr_vwmr2                  : ( 23 - 16 + 1 );
   ro_bf_t wr_vwmr3                  : ( 31 - 24 + 1 );
} phy_cal_dm_vwmr0_t;

typedef struct {
   ro_bf_t wr_vwmr4                  : ( 7 - 0 + 1 );
   ro_bf_t wr_vwmr5                  : ( 15 - 8 + 1 );
   ro_bf_t wr_vwmr6                  : ( 23 - 16 + 1 );
   ro_bf_t wr_vwmr7                  : ( 31 - 24 + 1 );
} phy_cal_dm_vwmr1_t;

typedef struct {
   ro_bf_t wr_vwmr8                  : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_cal_dm_vwmr2_t;

typedef struct {
   ro_bf_t rd0deskew0                : ( 7 - 0 + 1 );
   ro_bf_t rd0deskew1                : ( 15 - 8 + 1 );
   ro_bf_t rd0deskew2                : ( 23 - 16 + 1 );
   ro_bf_t rd0deskew3                : ( 31 - 24 + 1 );
} phy_rd_deskew_con0_t;

typedef struct {
   ro_bf_t rd0deskew4                : ( 7 - 0 + 1 );
   ro_bf_t rd0deskew5                : ( 15 - 8 + 1 );
   ro_bf_t rd0deskew6                : ( 23 - 16 + 1 );
   ro_bf_t rd0deskew7                : ( 31 - 24 + 1 );
} phy_rd_deskew_con1_t;

typedef struct {
   ro_bf_t rd0deskew8                : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_rd_deskew_con2_t;

typedef struct {
   ro_bf_t rd1deskew0                : ( 7 - 0 + 1 );
   ro_bf_t rd1deskew1                : ( 15 - 8 + 1 );
   ro_bf_t rd1deskew2                : ( 23 - 16 + 1 );
   ro_bf_t rd1deskew3                : ( 31 - 24 + 1 );
} phy_rd_deskew_con3_t;

typedef struct {
   ro_bf_t rd1deskew4                : ( 7 - 0 + 1 );
   ro_bf_t rd1deskew5                : ( 15 - 8 + 1 );
   ro_bf_t rd1deskew6                : ( 23 - 16 + 1 );
   ro_bf_t rd1deskew7                : ( 31 - 24 + 1 );
} phy_rd_deskew_con4_t;

typedef struct {
   ro_bf_t rd1deskew8                : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_rd_deskew_con5_t;

typedef struct {
   ro_bf_t rd2deskew0                : ( 7 - 0 + 1 );
   ro_bf_t rd2deskew1                : ( 15 - 8 + 1 );
   ro_bf_t rd2deskew2                : ( 23 - 16 + 1 );
   ro_bf_t rd2deskew3                : ( 31 - 24 + 1 );
} phy_rd_deskew_con6_t;

typedef struct {
   ro_bf_t rd2deskew4                : ( 7 - 0 + 1 );
   ro_bf_t rd2deskew5                : ( 15 - 8 + 1 );
   ro_bf_t rd2deskew6                : ( 23 - 16 + 1 );
   ro_bf_t rd2deskew7                : ( 31 - 24 + 1 );
} phy_rd_deskew_con7_t;

typedef struct {
   ro_bf_t rd2deskew8                : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_rd_deskew_con8_t;

typedef struct {
   ro_bf_t rd3deskew0                : ( 7 - 0 + 1 );
   ro_bf_t rd3deskew1                : ( 15 - 8 + 1 );
   ro_bf_t rd3deskew2                : ( 23 - 16 + 1 );
   ro_bf_t rd3deskew3                : ( 31 - 24 + 1 );
} phy_rd_deskew_con9_t;

typedef struct {
   ro_bf_t rd3deskew4                : ( 7 - 0 + 1 );
   ro_bf_t rd3deskew5                : ( 15 - 8 + 1 );
   ro_bf_t rd3deskew6                : ( 23 - 16 + 1 );
   ro_bf_t rd3deskew7                : ( 31 - 24 + 1 );
} phy_rd_deskew_con10_t;

typedef struct {
   ro_bf_t rd4deskew8                : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_rd_deskew_con11_t;

typedef struct {
   ro_bf_t rd4deskew0                : ( 7 - 0 + 1 );
   ro_bf_t rd4deskew1                : ( 15 - 8 + 1 );
   ro_bf_t rd4deskew2                : ( 23 - 16 + 1 );
   ro_bf_t rd4deskew3                : ( 31 - 24 + 1 );
} phy_rd_deskew_con12_t;

typedef struct {
   ro_bf_t rd4deskew4                : ( 7 - 0 + 1 );
   ro_bf_t rd4deskew5                : ( 15 - 8 + 1 );
   ro_bf_t rd4deskew6                : ( 23 - 16 + 1 );
   ro_bf_t rd4deskew7                : ( 31 - 24 + 1 );
} phy_rd_deskew_con13_t;

typedef struct {
   ro_bf_t rd4deskew8                : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_rd_deskew_con14_t;

typedef struct {
   ro_bf_t rd5deskew0                : ( 7 - 0 + 1 );
   ro_bf_t rd5deskew1                : ( 15 - 8 + 1 );
   ro_bf_t rd5deskew2                : ( 23 - 16 + 1 );
   ro_bf_t rd5deskew3                : ( 31 - 24 + 1 );
} phy_rd_deskew_con15_t;

typedef struct {
   ro_bf_t rd5deskew4                : ( 7 - 0 + 1 );
   ro_bf_t rd5deskew5                : ( 15 - 8 + 1 );
   ro_bf_t rd5deskew6                : ( 23 - 16 + 1 );
   ro_bf_t rd5deskew7                : ( 31 - 24 + 1 );
} phy_rd_deskew_con16_t;

typedef struct {
   ro_bf_t rd5deskew8                : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_rd_deskew_con17_t;

typedef struct {
   ro_bf_t rd6deskew0                : ( 7 - 0 + 1 );
   ro_bf_t rd6deskew1                : ( 15 - 8 + 1 );
   ro_bf_t rd6deskew2                : ( 23 - 16 + 1 );
   ro_bf_t rd6deskew3                : ( 31 - 24 + 1 );
} phy_rd_deskew_con18_t;

typedef struct {
   ro_bf_t rd6deskew4                : ( 7 - 0 + 1 );
   ro_bf_t rd6deskew5                : ( 15 - 8 + 1 );
   ro_bf_t rd6deskew6                : ( 23 - 16 + 1 );
   ro_bf_t rd6deskew7                : ( 31 - 24 + 1 );
} phy_rd_deskew_con19_t;

typedef struct {
   ro_bf_t rd6deskew8                : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_rd_deskew_con20_t;

typedef struct {
   ro_bf_t rd7deskew0                : ( 7 - 0 + 1 );
   ro_bf_t rd7deskew1                : ( 15 - 8 + 1 );
   ro_bf_t rd7deskew2                : ( 23 - 16 + 1 );
   ro_bf_t rd7deskew3                : ( 31 - 24 + 1 );
} phy_rd_deskew_con21_t;

typedef struct {
   ro_bf_t rd7deskew4                : ( 7 - 0 + 1 );
   ro_bf_t rd7deskew5                : ( 15 - 8 + 1 );
   ro_bf_t rd7deskew6                : ( 23 - 16 + 1 );
   ro_bf_t rd7deskew7                : ( 31 - 24 + 1 );
} phy_rd_deskew_con22_t;

typedef struct {
   ro_bf_t rd7deskew8                : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_rd_deskew_con23_t;

typedef struct {
   ro_bf_t wr0deskew0                : ( 7 - 0 + 1 );
   ro_bf_t wr0deskew1                : ( 15 - 8 + 1 );
   ro_bf_t wr0deskew2                : ( 23 - 16 + 1 );
   ro_bf_t wr0deskew3                : ( 31 - 24 + 1 );
} phy_wr_deskew_con0_t;

typedef struct {
   ro_bf_t wr0deskew4                : ( 7 - 0 + 1 );
   ro_bf_t wr0deskew5                : ( 15 - 8 + 1 );
   ro_bf_t wr0deskew6                : ( 23 - 16 + 1 );
   ro_bf_t wr0deskew7                : ( 31 - 24 + 1 );
} phy_wr_deskew_con1_t;

typedef struct {
   ro_bf_t wr0deskew8                : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_wr_deskew_con2_t;

typedef struct {
   ro_bf_t wr1deskew0                : ( 7 - 0 + 1 );
   ro_bf_t wr1deskew1                : ( 15 - 8 + 1 );
   ro_bf_t wr1deskew2                : ( 23 - 16 + 1 );
   ro_bf_t wr1deskew3                : ( 31 - 24 + 1 );
} phy_wr_deskew_con3_t;

typedef struct {
   ro_bf_t wr1deskew4                : ( 7 - 0 + 1 );
   ro_bf_t wr1deskew5                : ( 15 - 8 + 1 );
   ro_bf_t wr1deskew6                : ( 23 - 16 + 1 );
   ro_bf_t wr1deskew7                : ( 31 - 24 + 1 );
} phy_wr_deskew_con4_t;

typedef struct {
   ro_bf_t wr1deskew8                : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_wr_deskew_con5_t;

typedef struct {
   ro_bf_t wr2deskew0                : ( 7 - 0 + 1 );
   ro_bf_t wr2deskew1                : ( 15 - 8 + 1 );
   ro_bf_t wr2deskew2                : ( 23 - 16 + 1 );
   ro_bf_t wr2deskew3                : ( 31 - 24 + 1 );
} phy_wr_deskew_con6_t;

typedef struct {
   ro_bf_t wr2deskew4                : ( 7 - 0 + 1 );
   ro_bf_t wr2deskew5                : ( 15 - 8 + 1 );
   ro_bf_t wr2deskew6                : ( 23 - 16 + 1 );
   ro_bf_t wr2deskew7                : ( 31 - 24 + 1 );
} phy_wr_deskew_con7_t;

typedef struct {
   ro_bf_t wr2deskew8                : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_wr_deskew_con8_t;

typedef struct {
   ro_bf_t wr3deskew0                : ( 7 - 0 + 1 );
   ro_bf_t wr3deskew1                : ( 15 - 8 + 1 );
   ro_bf_t wr3deskew2                : ( 23 - 16 + 1 );
   ro_bf_t wr3deskew3                : ( 31 - 24 + 1 );
} phy_wr_deskew_con9_t;

typedef struct {
   ro_bf_t wr3deskew4                : ( 7 - 0 + 1 );
   ro_bf_t wr3deskew5                : ( 15 - 8 + 1 );
   ro_bf_t wr3deskew6                : ( 23 - 16 + 1 );
   ro_bf_t wr3deskew7                : ( 31 - 24 + 1 );
} phy_wr_deskew_con10_t;

typedef struct {
   ro_bf_t wr3deskew8                : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_wr_deskew_con11_t;

typedef struct {
   ro_bf_t wr4deskew0                : ( 7 - 0 + 1 );
   ro_bf_t wr4deskew1                : ( 15 - 8 + 1 );
   ro_bf_t wr4deskew2                : ( 23 - 16 + 1 );
   ro_bf_t wr4deskew3                : ( 31 - 24 + 1 );
} phy_wr_deskew_con12_t;

typedef struct {
   ro_bf_t wr4deskew4                : ( 7 - 0 + 1 );
   ro_bf_t wr4deskew5                : ( 15 - 8 + 1 );
   ro_bf_t wr4deskew6                : ( 23 - 16 + 1 );
   ro_bf_t wr4deskew7                : ( 31 - 24 + 1 );
} phy_wr_deskew_con13_t;

typedef struct {
   ro_bf_t wr4deskew8                : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_wr_deskew_con14_t;

typedef struct {
   ro_bf_t wr5deskew0                : ( 7 - 0 + 1 );
   ro_bf_t wr5deskew1                : ( 15 - 8 + 1 );
   ro_bf_t wr5deskew2                : ( 23 - 16 + 1 );
   ro_bf_t wr5deskew3                : ( 31 - 24 + 1 );
} phy_wr_deskew_con15_t;

typedef struct {
   ro_bf_t wr5deskew4                : ( 7 - 0 + 1 );
   ro_bf_t wr5deskew5                : ( 15 - 8 + 1 );
   ro_bf_t wr5deskew6                : ( 23 - 16 + 1 );
   ro_bf_t wr5deskew7                : ( 31 - 24 + 1 );
} phy_wr_deskew_con16_t;

typedef struct {
   ro_bf_t wr5deskew8                : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_wr_deskew_con17_t;

typedef struct {
   ro_bf_t wr6deskew0                : ( 7 - 0 + 1 );
   ro_bf_t wr6deskew1                : ( 15 - 8 + 1 );
   ro_bf_t wr6deskew2                : ( 23 - 16 + 1 );
   ro_bf_t wr6deskew3                : ( 31 - 24 + 1 );
} phy_wr_deskew_con18_t;

typedef struct {
   ro_bf_t wr6deskew4                : ( 7 - 0 + 1 );
   ro_bf_t wr6deskew5                : ( 15 - 8 + 1 );
   ro_bf_t wr6deskew6                : ( 23 - 16 + 1 );
   ro_bf_t wr6deskew7                : ( 31 - 24 + 1 );
} phy_wr_deskew_con19_t;

typedef struct {
   ro_bf_t wr6deskew8                : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_wr_deskew_con20_t;

typedef struct {
   ro_bf_t wr7deskew0                : ( 7 - 0 + 1 );
   ro_bf_t wr7deskew1                : ( 15 - 8 + 1 );
   ro_bf_t wr7deskew2                : ( 23 - 16 + 1 );
   ro_bf_t wr7deskew3                : ( 31 - 24 + 1 );
} phy_wr_deskew_con21_t;

typedef struct {
   ro_bf_t wr7deskew4                : ( 7 - 0 + 1 );
   ro_bf_t wr7deskew5                : ( 15 - 8 + 1 );
   ro_bf_t wr7deskew6                : ( 23 - 16 + 1 );
   ro_bf_t wr7deskew7                : ( 31 - 24 + 1 );
} phy_wr_deskew_con22_t;

typedef struct {
   ro_bf_t wr7deskew8                : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_wr_deskew_con23_t;

typedef struct {
   ro_bf_t dmdeskew0                 : ( 7 - 0 + 1 );
   ro_bf_t dmdeskew1                 : ( 15 - 8 + 1 );
   ro_bf_t dmdeskew2                 : ( 23 - 16 + 1 );
   ro_bf_t dmdeskew3                 : ( 31 - 24 + 1 );
} phy_dm_deskew_con0_t;

typedef struct {
   ro_bf_t dmdeskew4                 : ( 7 - 0 + 1 );
   ro_bf_t dmdeskew5                 : ( 15 - 8 + 1 );
   ro_bf_t dmdeskew6                 : ( 23 - 16 + 1 );
   ro_bf_t dmdeskew7                 : ( 31 - 24 + 1 );
} phy_dm_deskew_con1_t;

typedef struct {
   ro_bf_t dmdeskew8                 : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_dm_deskew_con2_t;

typedef struct {
   ro_bf_t d0_vwmc0                  : ( 7 - 0 + 1 );
   ro_bf_t d0_vwmc1                  : ( 15 - 8 + 1 );
   ro_bf_t d0_vwmc2                  : ( 23 - 16 + 1 );
   ro_bf_t d0_vwmc3                  : ( 31 - 24 + 1 );
} phy_vwmc_stat0_t;

typedef struct {
   ro_bf_t d0_vwmc4                  : ( 7 - 0 + 1 );
   ro_bf_t d0_vwmc5                  : ( 15 - 8 + 1 );
   ro_bf_t d0_vwmc6                  : ( 23 - 16 + 1 );
   ro_bf_t d0_vwmc7                  : ( 31 - 24 + 1 );
} phy_vwmc_stat1_t;

typedef struct {
   ro_bf_t d0_vwmc8                  : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_vwmc_stat2_t;

typedef struct {
   ro_bf_t d1_vwmc0                  : ( 7 - 0 + 1 );
   ro_bf_t d1_vwmc1                  : ( 15 - 8 + 1 );
   ro_bf_t d1_vwmc2                  : ( 23 - 16 + 1 );
   ro_bf_t d1_vwmc3                  : ( 31 - 24 + 1 );
} phy_vwmc_stat3_t;

typedef struct {
   ro_bf_t d1_vwmc4                  : ( 7 - 0 + 1 );
   ro_bf_t d1_vwmc5                  : ( 15 - 8 + 1 );
   ro_bf_t d1_vwmc6                  : ( 23 - 16 + 1 );
   ro_bf_t d1_vwmc7                  : ( 31 - 24 + 1 );
} phy_vwmc_stat4_t;

typedef struct {
   ro_bf_t d1_vwmc8                  : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_vwmc_stat5_t;

typedef struct {
   ro_bf_t d2_vwmc0                  : ( 7 - 0 + 1 );
   ro_bf_t d2_vwmc1                  : ( 15 - 8 + 1 );
   ro_bf_t d2_vwmc2                  : ( 23 - 16 + 1 );
   ro_bf_t d2_vwmc3                  : ( 31 - 24 + 1 );
} phy_vwmc_stat6_t;

typedef struct {
   ro_bf_t d2_vwmc4                  : ( 7 - 0 + 1 );
   ro_bf_t d2_vwmc5                  : ( 15 - 8 + 1 );
   ro_bf_t d2_vwmc6                  : ( 23 - 16 + 1 );
   ro_bf_t d2_vwmc7                  : ( 31 - 24 + 1 );
} phy_vwmc_stat7_t;

typedef struct {
   ro_bf_t d2_vwmc8                  : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_vwmc_stat8_t;

typedef struct {
   ro_bf_t d3_vwmc4                  : ( 7 - 0 + 1 );
   ro_bf_t d3_vwmc5                  : ( 15 - 8 + 1 );
   ro_bf_t d3_vwmc6                  : ( 23 - 16 + 1 );
   ro_bf_t d3_vwmc7                  : ( 31 - 24 + 1 );
} phy_vwmc_stat9_t;

typedef struct {
   ro_bf_t d3_vwmc4                  : ( 7 - 0 + 1 );
   ro_bf_t d3_vwmc5                  : ( 15 - 8 + 1 );
   ro_bf_t d3_vwmc6                  : ( 23 - 16 + 1 );
   ro_bf_t d3_vwmc7                  : ( 31 - 24 + 1 );
} phy_vwmc_stat10_t;

typedef struct {
   ro_bf_t d3_vwmc8                  : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_vwmc_stat11_t;

typedef struct {
   ro_bf_t d4_vwmc0                  : ( 7 - 0 + 1 );
   ro_bf_t d4_vwmc1                  : ( 15 - 8 + 1 );
   ro_bf_t d4_vwmc2                  : ( 23 - 16 + 1 );
   ro_bf_t d4_vwmc3                  : ( 31 - 24 + 1 );
} phy_vwmc_stat12_t;

typedef struct {
   ro_bf_t d4_vwmc4                  : ( 7 - 0 + 1 );
   ro_bf_t d4_vwmc5                  : ( 15 - 8 + 1 );
   ro_bf_t d4_vwmc6                  : ( 23 - 16 + 1 );
   ro_bf_t d4_vwmc7                  : ( 31 - 24 + 1 );
} phy_vwmc_stat13_t;

typedef struct {
   ro_bf_t d4_vwmc8                  : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_vwmc_stat14_t;

typedef struct {
   ro_bf_t d5_vwmc0                  : ( 7 - 0 + 1 );
   ro_bf_t d5_vwmc1                  : ( 15 - 8 + 1 );
   ro_bf_t d5_vwmc2                  : ( 23 - 16 + 1 );
   ro_bf_t d5_vwmc3                  : ( 31 - 24 + 1 );
} phy_vwmc_stat15_t;

typedef struct {
   ro_bf_t d5_vwmc4                  : ( 7 - 0 + 1 );
   ro_bf_t d5_vwmc5                  : ( 15 - 8 + 1 );
   ro_bf_t d5_vwmc6                  : ( 23 - 16 + 1 );
   ro_bf_t d5_vwmc7                  : ( 31 - 24 + 1 );
} phy_vwmc_stat16_t;

typedef struct {
   ro_bf_t d5_vwmc8                  : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_vwmc_stat17_t;

typedef struct {
   ro_bf_t d6_vwmc0                  : ( 7 - 0 + 1 );
   ro_bf_t d6_vwmc1                  : ( 15 - 8 + 1 );
   ro_bf_t d6_vwmc2                  : ( 23 - 16 + 1 );
   ro_bf_t d6_vwmc3                  : ( 31 - 24 + 1 );
} phy_vwmc_stat18_t;

typedef struct {
   ro_bf_t d6_vwmc4                  : ( 7 - 0 + 1 );
   ro_bf_t d6_vwmc5                  : ( 15 - 8 + 1 );
   ro_bf_t d6_vwmc6                  : ( 23 - 16 + 1 );
   ro_bf_t d6_vwmc7                  : ( 31 - 24 + 1 );
} phy_vwmc_stat19_t;

typedef struct {
   ro_bf_t d6_vwmc8                  : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_vwmc_stat20_t;

typedef struct {
   ro_bf_t d7_vwmc0                  : ( 7 - 0 + 1 );
   ro_bf_t d7_vwmc1                  : ( 15 - 8 + 1 );
   ro_bf_t d7_vwmc2                  : ( 23 - 16 + 1 );
   ro_bf_t d7_vwmc3                  : ( 31 - 24 + 1 );
} phy_vwmc_stat21_t;

typedef struct {
   ro_bf_t d7_vwmc4                  : ( 7 - 0 + 1 );
   ro_bf_t d7_vwmc5                  : ( 15 - 8 + 1 );
   ro_bf_t d7_vwmc6                  : ( 23 - 16 + 1 );
   ro_bf_t d7_vwmc7                  : ( 31 - 24 + 1 );
} phy_vwmc_stat22_t;

typedef struct {
   ro_bf_t d7_vwmc8                  : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_vwmc_stat23_t;

typedef struct {
   ro_bf_t dm_vwmc0                  : ( 7 - 0 + 1 );
   ro_bf_t dm_vwmc1                  : ( 15 - 8 + 1 );
   ro_bf_t dm_vwmc2                  : ( 23 - 16 + 1 );
   ro_bf_t dm_vwmc3                  : ( 31 - 24 + 1 );
} phy_dm_vwmc_stat0_t;

typedef struct {
   ro_bf_t dm_vwmc4                  : ( 7 - 0 + 1 );
   ro_bf_t dm_vwmc5                  : ( 15 - 8 + 1 );
   ro_bf_t dm_vwmc6                  : ( 23 - 16 + 1 );
   ro_bf_t dm_vwmc7                  : ( 31 - 24 + 1 );
} phy_dm_vwmc_stat1_t;

typedef struct {
   ro_bf_t dm_vwmc8                  : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_dm_vwmc_stat2_t;

typedef struct {
   ro_bf_t d0_vwml0                  : ( 7 - 0 + 1 );
   ro_bf_t d0_vwml1                  : ( 15 - 8 + 1 );
   ro_bf_t d0_vwml2                  : ( 23 - 16 + 1 );
   ro_bf_t d0_vwml3                  : ( 31 - 24 + 1 );
} phy_vwml_stat0_t;

typedef struct {
   ro_bf_t d0_vwml4                  : ( 7 - 0 + 1 );
   ro_bf_t d0_vwml5                  : ( 15 - 8 + 1 );
   ro_bf_t d0_vwml6                  : ( 23 - 16 + 1 );
   ro_bf_t d0_vwml7                  : ( 31 - 24 + 1 );
} phy_vwml_stat1_t;

typedef struct {
   ro_bf_t d0_vwml8                  : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_vwml_stat2_t;

typedef struct {
   ro_bf_t d1_vwml0                  : ( 7 - 0 + 1 );
   ro_bf_t d1_vwml1                  : ( 15 - 8 + 1 );
   ro_bf_t d1_vwml2                  : ( 23 - 16 + 1 );
   ro_bf_t d1_vwml3                  : ( 31 - 24 + 1 );
} phy_vwml_stat3_t;

typedef struct {
   ro_bf_t d1_vwml4                  : ( 7 - 0 + 1 );
   ro_bf_t d1_vwml5                  : ( 15 - 8 + 1 );
   ro_bf_t d1_vwml6                  : ( 23 - 16 + 1 );
   ro_bf_t d1_vwml7                  : ( 31 - 24 + 1 );
} phy_vwml_stat4_t;

typedef struct {
   ro_bf_t d1_vwml8                  : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_vwml_stat5_t;

typedef struct {
   ro_bf_t d2_vwml0                  : ( 7 - 0 + 1 );
   ro_bf_t d2_vwml1                  : ( 15 - 8 + 1 );
   ro_bf_t d2_vwml2                  : ( 23 - 16 + 1 );
   ro_bf_t d2_vwml3                  : ( 31 - 24 + 1 );
} phy_vwml_stat6_t;

typedef struct {
   ro_bf_t d2_vwml4                  : ( 7 - 0 + 1 );
   ro_bf_t d2_vwml5                  : ( 15 - 8 + 1 );
   ro_bf_t d2_vwml6                  : ( 23 - 16 + 1 );
   ro_bf_t d2_vwml7                  : ( 31 - 24 + 1 );
} phy_vwml_stat7_t;

typedef struct {
   ro_bf_t d2_vwml8                  : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_vwml_stat8_t;

typedef struct {
   ro_bf_t d3_vwml0                  : ( 7 - 0 + 1 );
   ro_bf_t d3_vwml1                  : ( 15 - 8 + 1 );
   ro_bf_t d3_vwml2                  : ( 23 - 16 + 1 );
   ro_bf_t d3_vwml3                  : ( 31 - 24 + 1 );
} phy_vwml_stat9_t;

typedef struct {
   ro_bf_t d3_vwml4                  : ( 7 - 0 + 1 );
   ro_bf_t d3_vwml5                  : ( 15 - 8 + 1 );
   ro_bf_t d3_vwml6                  : ( 23 - 16 + 1 );
   ro_bf_t d3_vwml7                  : ( 31 - 24 + 1 );
} phy_vwml_stat10_t;

typedef struct {
   ro_bf_t d3_vwml8                  : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_vwml_stat11_t;

typedef struct {
   ro_bf_t d4_vwml0                  : ( 7 - 0 + 1 );
   ro_bf_t d4_vwml1                  : ( 15 - 8 + 1 );
   ro_bf_t d4_vwml2                  : ( 23 - 16 + 1 );
   ro_bf_t d4_vwml3                  : ( 31 - 24 + 1 );
} phy_vwml_stat12_t;

typedef struct {
   ro_bf_t d4_vwml4                  : ( 7 - 0 + 1 );
   ro_bf_t d4_vwml5                  : ( 15 - 8 + 1 );
   ro_bf_t d4_vwml6                  : ( 23 - 16 + 1 );
   ro_bf_t d4_vwml7                  : ( 31 - 24 + 1 );
} phy_vwml_stat13_t;

typedef struct {
   ro_bf_t d4_vwml8                  : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_vwml_stat14_t;

typedef struct {
   ro_bf_t d5_vwml0                  : ( 7 - 0 + 1 );
   ro_bf_t d5_vwml1                  : ( 15 - 8 + 1 );
   ro_bf_t d5_vwml2                  : ( 23 - 16 + 1 );
   ro_bf_t d5_vwml3                  : ( 31 - 24 + 1 );
} phy_vwml_stat15_t;

typedef struct {
   ro_bf_t d5_vwml4                  : ( 7 - 0 + 1 );
   ro_bf_t d5_vwml5                  : ( 15 - 8 + 1 );
   ro_bf_t d5_vwml6                  : ( 23 - 16 + 1 );
   ro_bf_t d5_vwml7                  : ( 31 - 24 + 1 );
} phy_vwml_stat16_t;

typedef struct {
   ro_bf_t d5_vwml8                  : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_vwml_stat17_t;

typedef struct {
   ro_bf_t d6_vwml0                  : ( 7 - 0 + 1 );
   ro_bf_t d6_vwml1                  : ( 15 - 8 + 1 );
   ro_bf_t d6_vwml2                  : ( 23 - 16 + 1 );
   ro_bf_t d6_vwml3                  : ( 31 - 24 + 1 );
} phy_vwml_stat18_t;

typedef struct {
   ro_bf_t d6_vwml4                  : ( 7 - 0 + 1 );
   ro_bf_t d6_vwml5                  : ( 15 - 8 + 1 );
   ro_bf_t d6_vwml6                  : ( 23 - 16 + 1 );
   ro_bf_t d6_vwml7                  : ( 31 - 24 + 1 );
} phy_vwml_stat19_t;

typedef struct {
   ro_bf_t d6_vwml8                  : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_vwml_stat20_t;

typedef struct {
   ro_bf_t d7_vwml0                  : ( 7 - 0 + 1 );
   ro_bf_t d7_vwml1                  : ( 15 - 8 + 1 );
   ro_bf_t d7_vwml2                  : ( 23 - 16 + 1 );
   ro_bf_t d7_vwml3                  : ( 31 - 24 + 1 );
} phy_vwml_stat21_t;

typedef struct {
   ro_bf_t d7_vwml4                  : ( 7 - 0 + 1 );
   ro_bf_t d7_vwml5                  : ( 15 - 8 + 1 );
   ro_bf_t d7_vwml6                  : ( 23 - 16 + 1 );
   ro_bf_t d7_vwml7                  : ( 31 - 24 + 1 );
} phy_vwml_stat22_t;

typedef struct {
   ro_bf_t d7_vwml8                  : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_vwml_stat23_t;

typedef struct {
   ro_bf_t dm_vwml0                  : ( 7 - 0 + 1 );
   ro_bf_t dm_vwml1                  : ( 15 - 8 + 1 );
   ro_bf_t dm_vwml2                  : ( 23 - 16 + 1 );
   ro_bf_t dm_vwml3                  : ( 31 - 24 + 1 );
} phy_dm_vwml_stat1_t;

typedef struct {
   ro_bf_t dm_vwml4                  : ( 7 - 0 + 1 );
   ro_bf_t dm_vwml5                  : ( 15 - 8 + 1 );
   ro_bf_t dm_vwml6                  : ( 23 - 16 + 1 );
   ro_bf_t dm_vwml7                  : ( 31 - 24 + 1 );
} phy_dm_vwml_stat2_t;

typedef struct {
   ro_bf_t dm_vwml8                  : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_dm_vwml_stat3_t;

typedef struct {
   ro_bf_t d0_vwmr0                  : ( 7 - 0 + 1 );
   ro_bf_t d0_vwmr1                  : ( 15 - 8 + 1 );
   ro_bf_t d0_vwmr2                  : ( 23 - 16 + 1 );
   ro_bf_t d0_vwmr3                  : ( 31 - 24 + 1 );
} phy_vwmr_stat0_t;

typedef struct {
   ro_bf_t d0_vwmr4                  : ( 7 - 0 + 1 );
   ro_bf_t d0_vwmr5                  : ( 15 - 8 + 1 );
   ro_bf_t d0_vwmr6                  : ( 23 - 16 + 1 );
   ro_bf_t d0_vwmr7                  : ( 31 - 24 + 1 );
} phy_vwmr_stat1_t;

typedef struct {
   ro_bf_t d0_vwmr8                  : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_vwmr_stat2_t;

typedef struct {
   ro_bf_t d1_vwmr0                  : ( 7 - 0 + 1 );
   ro_bf_t d1_vwmr1                  : ( 15 - 8 + 1 );
   ro_bf_t d1_vwmr2                  : ( 23 - 16 + 1 );
   ro_bf_t d1_vwmr3                  : ( 31 - 24 + 1 );
} phy_vwmr_stat3_t;

typedef struct {
   ro_bf_t d1_vwmr4                  : ( 7 - 0 + 1 );
   ro_bf_t d1_vwmr5                  : ( 15 - 8 + 1 );
   ro_bf_t d1_vwmr6                  : ( 23 - 16 + 1 );
   ro_bf_t d1_vwmr7                  : ( 31 - 24 + 1 );
} phy_vwmr_stat4_t;

typedef struct {
   ro_bf_t d1_vwmr8                  : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_vwmr_stat5_t;

typedef struct {
   ro_bf_t d2_vwmr0                  : ( 7 - 0 + 1 );
   ro_bf_t d2_vwmr1                  : ( 15 - 8 + 1 );
   ro_bf_t d2_vwmr2                  : ( 23 - 16 + 1 );
   ro_bf_t d2_vwmr3                  : ( 31 - 24 + 1 );
} phy_vwmr_stat6_t;

typedef struct {
   ro_bf_t d2_vwmr4                  : ( 7 - 0 + 1 );
   ro_bf_t d2_vwmr5                  : ( 15 - 8 + 1 );
   ro_bf_t d2_vwmr6                  : ( 23 - 16 + 1 );
   ro_bf_t d2_vwmr7                  : ( 31 - 24 + 1 );
} phy_vwmr_stat7_t;

typedef struct {
   ro_bf_t d2_vwmr8                  : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_vwmr_stat8_t;

typedef struct {
   ro_bf_t d3_vwmr0                  : ( 7 - 0 + 1 );
   ro_bf_t d3_vwmr1                  : ( 15 - 8 + 1 );
   ro_bf_t d3_vwmr2                  : ( 23 - 16 + 1 );
   ro_bf_t d3_vwmr3                  : ( 31 - 24 + 1 );
} phy_vwmr_stat9_t;

typedef struct {
   ro_bf_t d3_vwmr4                  : ( 7 - 0 + 1 );
   ro_bf_t d3_vwmr5                  : ( 15 - 8 + 1 );
   ro_bf_t d3_vwmr6                  : ( 23 - 16 + 1 );
   ro_bf_t d3_vwmr7                  : ( 31 - 24 + 1 );
} phy_vwmr_stat10_t;

typedef struct {
   ro_bf_t d3_vwmr8                  : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_vwmr_stat11_t;

typedef struct {
   ro_bf_t d4_vwmr0                  : ( 7 - 0 + 1 );
   ro_bf_t d4_vwmr1                  : ( 15 - 8 + 1 );
   ro_bf_t d4_vwmr2                  : ( 23 - 16 + 1 );
   ro_bf_t d4_vwmr3                  : ( 31 - 24 + 1 );
} phy_vwmr_stat12_t;

typedef struct {
   ro_bf_t d4_vwmr4                  : ( 7 - 0 + 1 );
   ro_bf_t d4_vwmr5                  : ( 15 - 8 + 1 );
   ro_bf_t d4_vwmr6                  : ( 23 - 16 + 1 );
   ro_bf_t d4_vwmr7                  : ( 31 - 24 + 1 );
} phy_vwmr_stat13_t;

typedef struct {
   ro_bf_t d4_vwmr8                  : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_vwmr_stat14_t;

typedef struct {
   ro_bf_t d5_vwmr0                  : ( 7 - 0 + 1 );
   ro_bf_t d5_vwmr1                  : ( 15 - 8 + 1 );
   ro_bf_t d5_vwmr2                  : ( 23 - 16 + 1 );
   ro_bf_t d5_vwmr3                  : ( 31 - 24 + 1 );
} phy_vwmr_stat15_t;

typedef struct {
   ro_bf_t d5_vwmr4                  : ( 7 - 0 + 1 );
   ro_bf_t d5_vwmr5                  : ( 15 - 8 + 1 );
   ro_bf_t d5_vwmr6                  : ( 23 - 16 + 1 );
   ro_bf_t d5_vwmr7                  : ( 31 - 24 + 1 );
} phy_vwmr_stat16_t;

typedef struct {
   ro_bf_t d5_vwmr8                  : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_vwmr_stat17_t;

typedef struct {
   ro_bf_t d6_vwmr0                  : ( 7 - 0 + 1 );
   ro_bf_t d6_vwmr1                  : ( 15 - 8 + 1 );
   ro_bf_t d6_vwmr2                  : ( 23 - 16 + 1 );
   ro_bf_t d6_vwmr3                  : ( 31 - 24 + 1 );
} phy_vwmr_stat18_t;

typedef struct {
   ro_bf_t d6_vwmr4                  : ( 7 - 0 + 1 );
   ro_bf_t d6_vwmr5                  : ( 15 - 8 + 1 );
   ro_bf_t d6_vwmr6                  : ( 23 - 16 + 1 );
   ro_bf_t d6_vwmr7                  : ( 31 - 24 + 1 );
} phy_vwmr_stat19_t;

typedef struct {
   ro_bf_t d6_vwmr8                  : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_vwmr_stat20_t;

typedef struct {
   ro_bf_t d7_vwmr0                  : ( 7 - 0 + 1 );
   ro_bf_t d7_vwmr1                  : ( 15 - 8 + 1 );
   ro_bf_t d7_vwmr2                  : ( 23 - 16 + 1 );
   ro_bf_t d7_vwmr3                  : ( 31 - 24 + 1 );
} phy_vwmr_stat21_t;

typedef struct {
   ro_bf_t d7_vwmr4                  : ( 7 - 0 + 1 );
   ro_bf_t d7_vwmr5                  : ( 15 - 8 + 1 );
   ro_bf_t d7_vwmr6                  : ( 23 - 16 + 1 );
   ro_bf_t d7_vwmr7                  : ( 31 - 24 + 1 );
} phy_vwmr_stat22_t;

typedef struct {
   ro_bf_t d7_vwmr8                  : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_vwmr_stat23_t;

typedef struct {
   ro_bf_t dm_vwmr0                  : ( 7 - 0 + 1 );
   ro_bf_t dm_vwmr1                  : ( 15 - 8 + 1 );
   ro_bf_t dm_vwmr2                  : ( 23 - 16 + 1 );
   ro_bf_t dm_vwmr3                  : ( 31 - 24 + 1 );
} phy_dm_vwmr_stat0_t;

typedef struct {
   ro_bf_t dm_vwmr4                  : ( 7 - 0 + 1 );
   ro_bf_t dm_vwmr5                  : ( 15 - 8 + 1 );
   ro_bf_t dm_vwmr6                  : ( 23 - 16 + 1 );
   ro_bf_t dm_vwmr7                  : ( 31 - 24 + 1 );
} phy_dm_vwmr_stat1_t;

typedef struct {
   ro_bf_t dm_vwmr8                  : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_dm_vwmr_stat2_t;

typedef struct {
   ro_bf_t dq_io_rd0                 : ( 7 - 0 + 1 );
   ro_bf_t dq_io_rd1                 : ( 15 - 8 + 1 );
   ro_bf_t dq_io_rd2                 : ( 23 - 16 + 1 );
   ro_bf_t dq_io_rd3                 : ( 31 - 24 + 1 );
} phy_dq_io_rdata0_t;

typedef struct {
   ro_bf_t dq_io_rd4                 : ( 7 - 0 + 1 );
   ro_bf_t dq_io_rd5                 : ( 15 - 8 + 1 );
   ro_bf_t dq_io_rd6                 : ( 23 - 16 + 1 );
   ro_bf_t dq_io_rd7                 : ( 31 - 24 + 1 );
} phy_dq_io_rdata1_t;

typedef struct {
   ro_bf_t dq_io_rd8                 : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} phy_dq_io_rdata2_t;

typedef struct {
   rw_bf_t version_info              : ( 31 - 0 + 1 );
} phy_version_info_stat0_t;

typedef struct {
   rw_bf_t zq_auto_en                : ( 0 - 0 + 1 );
   rw_bf_t zq_manual_str             : ( 1 - 1 + 1 );
   rw_bf_t zq_manual_mode            : ( 3 - 2 + 1 );
   rw_bf_t zq_udt_dly                : ( 11 - 4 + 1 );
   rw_bf_t zq_force_impp             : ( 14 - 12 + 1 );
   rw_bf_t zq_force_impn             : ( 17 - 15 + 1 );
   rw_bf_t zq_clk_div_en             : ( 18 - 18 + 1 );
   rw_bf_t zq_mode_noterm            : ( 19 - 19 + 1 );
   rw_bf_t zq_rgddr3                 : ( 20 - 20 + 1 );
   rw_bf_t zq_mode_term              : ( 23 - 21 + 1 );
   rw_bf_t zq_mode_dds               : ( 26 - 24 + 1 );
   rw_bf_t zq_clk_en                 : ( 27 - 27 + 1 );
   rw_bf_t zq_mode_pdds              : ( 30 - 28 + 1 );
   rw_bf_t zq_mode_lp4               : ( 31 - 31 + 1 );
} phy_zq_con0_t;

typedef struct {
   ro_bf_t zq_done                   : ( 0 - 0 + 1 );
   ro_bf_t zq_pending                : ( 1 - 1 + 1 );
   ro_bf_t zq_error                  : ( 2 - 2 + 1 );
   ro_bf_t zq_nmon                   : ( 5 - 3 + 1 );
   ro_bf_t zq_pmon                   : ( 8 - 6 + 1 );
   ro_bf_t reserved_9_15                : ( 15 - 9 + 1 );
   rw_bf_t zq_vref                   : ( 21 - 16 + 1 );
   ro_bf_t reserved_22_31               : ( 31 - 22 + 1 );
} phy_zq_con1_t;

typedef struct {
   rw_bf_t ctrl_zq_clk_div           : ( 15 - 0 + 1 );
   rw_bf_t ctrl_zq_timer             : ( 31 - 16 + 1 );
} phy_zq_con2_t;

typedef struct {
   rw_bf_t zq_ds0_pdds               : ( 2 - 0 + 1 );
   rw_bf_t zq_ds0_dds                : ( 5 - 3 + 1 );
   ro_bf_t reserved_6_7                 : ( 7 - 6 + 1 );
   rw_bf_t zq_ds1_pdds               : ( 10 - 8 + 1 );
   rw_bf_t zq_ds1_dds                : ( 13 - 11 + 1 );
   ro_bf_t reserved_14_15               : ( 15 - 14 + 1 );
   rw_bf_t zq_ds2_pdds               : ( 18 - 16 + 1 );
   rw_bf_t zq_ds2_dds                : ( 21 - 19 + 1 );
   ro_bf_t reserved_22_23               : ( 23 - 22 + 1 );
   rw_bf_t zq_ds3_pdds               : ( 26 - 24 + 1 );
   rw_bf_t zq_ds3_dds                : ( 29 - 27 + 1 );
   ro_bf_t reserved_30_31               : ( 31 - 30 + 1 );
} phy_zq_con3_t;

typedef struct {
   rw_bf_t zq_ds4_pdds               : ( 2 - 0 + 1 );
   rw_bf_t zq_ds4_dds                : ( 5 - 3 + 1 );
   ro_bf_t reserved_6_7                 : ( 7 - 6 + 1 );
   rw_bf_t zq_ds5_pdds               : ( 10 - 8 + 1 );
   rw_bf_t zq_ds5_dds                : ( 13 - 11 + 1 );
   ro_bf_t reserved_14_15               : ( 15 - 14 + 1 );
   rw_bf_t zq_ds6_pdds               : ( 18 - 16 + 1 );
   rw_bf_t zq_ds6_dds                : ( 21 - 19 + 1 );
   ro_bf_t reserved_22_23               : ( 23 - 22 + 1 );
   rw_bf_t zq_ds7_pdds               : ( 26 - 24 + 1 );
   rw_bf_t zq_ds7_dds                : ( 29 - 27 + 1 );
   ro_bf_t reserved_30_31               : ( 31 - 30 + 1 );
} phy_zq_con4_t;

typedef struct {
   rw_bf_t zq_ds8_pdds               : ( 2 - 0 + 1 );
   rw_bf_t zq_ds8_dds                : ( 5 - 3 + 1 );
   ro_bf_t reserved_6_31                : ( 31 - 6 + 1 );
} phy_zq_con5_t;

typedef struct {
   rw_bf_t zq_ds0_lp4                : ( 0 - 0 + 1 );
   rw_bf_t zq_ds0_noterm             : ( 1 - 1 + 1 );
   rw_bf_t zq_ds0_rgddr3             : ( 2 - 2 + 1 );
   rw_bf_t zq_ds0_term               : ( 5 - 3 + 1 );
   ro_bf_t reserved_6_7                 : ( 7 - 6 + 1 );
   rw_bf_t zq_ds1_lp4                : ( 8 - 8 + 1 );
   rw_bf_t zq_ds1_noterm             : ( 9 - 9 + 1 );
   rw_bf_t zq_ds1_rgddr3             : ( 10 - 10 + 1 );
   rw_bf_t zq_ds1_term               : ( 13 - 11 + 1 );
   ro_bf_t reserved_14_15               : ( 15 - 14 + 1 );
   rw_bf_t zq_ds2_lp4                : ( 16 - 16 + 1 );
   rw_bf_t zq_ds2_noterm             : ( 17 - 17 + 1 );
   rw_bf_t zq_ds2_rgddr3             : ( 18 - 18 + 1 );
   rw_bf_t zq_ds2_term               : ( 21 - 19 + 1 );
   ro_bf_t reserved_22_23               : ( 23 - 22 + 1 );
   rw_bf_t zq_ds3_lp4                : ( 24 - 24 + 1 );
   rw_bf_t zq_ds3_noterm             : ( 25 - 25 + 1 );
   rw_bf_t zq_ds3_rgddr3             : ( 26 - 26 + 1 );
   rw_bf_t zq_ds3_term               : ( 29 - 27 + 1 );
   ro_bf_t reserved_30_31               : ( 31 - 30 + 1 );
} phy_zq_con6_t;

typedef struct {
   rw_bf_t zq_ds4_lp4                : ( 0 - 0 + 1 );
   rw_bf_t zq_ds4_noterm             : ( 1 - 1 + 1 );
   rw_bf_t zq_ds4_rgddr3             : ( 2 - 2 + 1 );
   rw_bf_t zq_ds4_term               : ( 5 - 3 + 1 );
   ro_bf_t reserved_6_7                 : ( 7 - 6 + 1 );
   rw_bf_t zq_ds5_lp4                : ( 8 - 8 + 1 );
   rw_bf_t zq_ds5_noterm             : ( 9 - 9 + 1 );
   rw_bf_t zq_ds5_rgddr3             : ( 10 - 10 + 1 );
   rw_bf_t zq_ds5_term               : ( 13 - 11 + 1 );
   ro_bf_t reserved_14_15               : ( 15 - 14 + 1 );
   rw_bf_t zq_ds6_lp4                : ( 16 - 16 + 1 );
   rw_bf_t zq_ds6_noterm             : ( 17 - 17 + 1 );
   rw_bf_t zq_ds6_rgddr3             : ( 18 - 18 + 1 );
   rw_bf_t zq_ds6_term               : ( 21 - 19 + 1 );
   ro_bf_t reserved_22_23               : ( 23 - 22 + 1 );
   rw_bf_t zq_ds7_lp4                : ( 24 - 24 + 1 );
   rw_bf_t zq_ds7_noterm             : ( 25 - 25 + 1 );
   rw_bf_t zq_ds7_rgddr3             : ( 26 - 26 + 1 );
   rw_bf_t zq_ds7_term               : ( 29 - 27 + 1 );
   ro_bf_t reserved_30_31               : ( 31 - 30 + 1 );
} phy_zq_con7_t;

typedef struct {
   rw_bf_t zq_ds8_lp4                : ( 0 - 0 + 1 );
   rw_bf_t zq_ds8_noterm             : ( 1 - 1 + 1 );
   rw_bf_t zq_ds8_rgddr3             : ( 2 - 2 + 1 );
   rw_bf_t zq_ds8_term               : ( 5 - 3 + 1 );
   ro_bf_t reserved_6_31                : ( 31 - 6 + 1 );
} phy_zq_con8_t;

typedef struct {
   rw_bf_t zq_ds0_vref               : ( 5 - 0 + 1 );
   rw_bf_t zq_ds0_vref_fsbst         : ( 6 - 6 + 1 );
   rw_bf_t zq_ds0_vref_pd            : ( 7 - 7 + 1 );
   rw_bf_t zq_ds1_vref               : ( 13 - 8 + 1 );
   rw_bf_t zq_ds1_vref_fsbst         : ( 14 - 14 + 1 );
   rw_bf_t zq_ds1_vref_pd            : ( 15 - 15 + 1 );
   rw_bf_t zq_ds2_vref               : ( 21 - 16 + 1 );
   rw_bf_t zq_ds2_vref_fsbst         : ( 22 - 22 + 1 );
   rw_bf_t zq_ds2_vref_pd            : ( 23 - 23 + 1 );
   rw_bf_t zq_ds3_vref               : ( 29 - 24 + 1 );
   rw_bf_t zq_ds3_vref_fsbst         : ( 30 - 30 + 1 );
   rw_bf_t zq_ds3_vref_pd            : ( 31 - 31 + 1 );
} phy_zq_con9_t;

typedef struct {
   rw_bf_t zq_ds4_vref               : ( 5 - 0 + 1 );
   rw_bf_t zq_ds4_vref_fsbst         : ( 6 - 6 + 1 );
   rw_bf_t zq_ds4_vref_pd            : ( 7 - 7 + 1 );
   rw_bf_t zq_ds5_vref               : ( 13 - 8 + 1 );
   rw_bf_t zq_ds5_vref_fsbst         : ( 14 - 14 + 1 );
   rw_bf_t zq_ds5_vref_pd            : ( 15 - 15 + 1 );
   rw_bf_t zq_ds6_vref               : ( 21 - 16 + 1 );
   rw_bf_t zq_ds6_vref_fsbst         : ( 22 - 22 + 1 );
   rw_bf_t zq_ds6_vref_pd            : ( 23 - 23 + 1 );
   rw_bf_t zq_ds7_vref               : ( 29 - 24 + 1 );
   rw_bf_t zq_ds7_vref_fsbst         : ( 30 - 30 + 1 );
   rw_bf_t zq_ds7_vref_pd            : ( 31 - 31 + 1 );
} phy_zq_con10_t;

typedef struct {
   rw_bf_t zq_ds8_vref               : ( 5 - 0 + 1 );
   rw_bf_t zq_ds8_vref_fsbst         : ( 6 - 6 + 1 );
   rw_bf_t zq_ds8_vref_pd            : ( 7 - 7 + 1 );
   ro_bf_t reserved_8_31             : ( 31 - 8 + 1 );
} phy_zq_con11_t;

typedef struct {
   rw_bf_t zq_ds0_impp               : ( 2 - 0 + 1 );
   rw_bf_t zq_ds0_impn               : ( 5 - 3 + 1 );
   ro_bf_t reserved_6_7              : ( 7 - 6 + 1 );
   rw_bf_t zq_ds1_impp               : ( 10 - 8 + 1 );
   rw_bf_t zq_ds1_impn               : ( 13 - 11 + 1 );
   ro_bf_t reserved_14_15            : ( 15 - 14 + 1 );
   rw_bf_t zq_ds2_impp               : ( 18 - 16 + 1 );
   rw_bf_t zq_ds2_impn               : ( 21 - 19 + 1 );
   ro_bf_t reserved_22_23            : ( 23 - 22 + 1 );
   rw_bf_t zq_ds3_impp               : ( 26 - 24 + 1 );
   rw_bf_t zq_ds3_impn               : ( 29 - 27 + 1 );
   ro_bf_t reserved_30_31            : ( 31 - 30 + 1 );
} phy_zq_con12_t;

typedef struct {
   rw_bf_t zq_ds4_impp               : ( 2 - 0 + 1 );
   rw_bf_t zq_ds4_impn               : ( 5 - 3 + 1 );
   ro_bf_t reserved_6_7              : ( 7 - 6 + 1 );
   rw_bf_t zq_ds5_impp               : ( 10 - 8 + 1 );
   rw_bf_t zq_ds5_impn               : ( 13 - 11 + 1 );
   ro_bf_t reserved_14_15            : ( 15 - 14 + 1 );
   rw_bf_t zq_ds6_impp               : ( 18 - 16 + 1 );
   rw_bf_t zq_ds6_impn               : ( 21 - 19 + 1 );
   ro_bf_t reserved_22_23            : ( 23 - 22 + 1 );
   rw_bf_t zq_ds7_impp               : ( 26 - 24 + 1 );
   rw_bf_t zq_ds7_impn               : ( 29 - 27 + 1 );
   ro_bf_t reserved_30_31            : ( 31 - 30 + 1 );
} phy_zq_con13_t;

typedef struct {
   rw_bf_t dqs_osc_cnt_init_cs1      : ( 15 - 0 + 1 );
   rw_bf_t dqs_osc_cnt_init_cs0      : ( 31 - 16 + 1 );
} phy_dqs_osc_con0_t;

typedef struct {
   rw_bf_t dqs_osc_cnt_new_cs0       : ( 15 - 0 + 1 );
   ro_bf_t reserved_16_31               : ( 31 - 16 + 1 );
} phy_dqs_osc_con1_t;

typedef struct {
   rw_bf_t dqs_osc_cnt_new_cs1       : ( 15 - 0 + 1 );
   ro_bf_t reserved_16_31               : ( 31 - 16 + 1 );
} phy_dqs_osc_con2_t;

typedef struct {
   rw_bf_t dqs_init_runtime          : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_20                : ( 20 - 8 + 1 );
   ro_bf_t dqs_osc_cnt_applied_cs0   : ( 21 - 21 + 1 );
   ro_bf_t dqs_osc_cnt_applied_cs1   : ( 22 - 22 + 1 );
   ro_bf_t dqs_osc_overflow_cs0      : ( 23 - 23 + 1 );
   ro_bf_t dqs_osc_overflow_cs1      : ( 24 - 24 + 1 );
   rw_bf_t dqs_upd_range             : ( 31 - 25 + 1 );
} phy_dqs_osc_con3_t;

typedef struct {
   ro_bf_t offsetw_osc_cs0           : ( 7 - 0 + 1 );
   ro_bf_t offsetw_osc_cs1           : ( 15 - 8 + 1 );
   ro_bf_t reserved_16_31               : ( 31 - 16 + 1 );
} phy_dqs_osc_con4_t;

typedef struct {
   rw_bf_t cbt_ca_vref               : ( 7 - 0 + 1 );
   rw_bf_t cbt_cmd_ca_vref           : ( 8 - 8 + 1 );
   rw_bf_t cbt_ca_vref_mode_ds_0     : ( 9 - 9 + 1 );
   rw_bf_t cbt_ca_vref_mode_ds_1     : ( 10 - 10 + 1 );
   ro_bf_t reserved_11_13            : ( 13 - 11 + 1 );
   rw_bf_t cbt_cmd_clear             : ( 14 - 14 + 1 );
   rw_bf_t cbt_cmd_ca_vwmr           : ( 15 - 15 + 1 );
   rw_bf_t cbt_cmd_ca_vwml           : ( 16 - 16 + 1 );
   rw_bf_t cbt_cmd_cs_vwmr           : ( 17 - 17 + 1 );
   rw_bf_t cbt_cmd_cs_vwml           : ( 18 - 18 + 1 );
   rw_bf_t cbt_reset                 : ( 19 - 19 + 1 );
   rw_bf_t cbt_odt_ca                : ( 21 - 20 + 1 );
   rw_bf_t cbt_cke                   : ( 23 - 22 + 1 );
   rw_bf_t cbt_ca_pattern            : ( 29 - 24 + 1 );
   rw_bf_t cbt_cs                    : ( 31 - 30 + 1 );
} phy_cbt_con0_t;


typedef struct {
   phy_gnr_con0_t GNR_CON0;
   phy_cal_con0_t CAL_CON0;
   phy_cal_con1_t CAL_CON1;
   phy_cal_con2_t CAL_CON2;
   phy_cal_con3_t CAL_CON3;
   phy_wlat_con0_t WLAT_CON0;
   phy_lp_con0_t LP_CON0;
   phy_gate_con0_t GATE_CON0;
   phy_offsetr_con0_t OFFSETR_CON0;
   phy_offsetr_con1_t OFFSETR_CON1;
   phy_offsetr_con2_t OFFSETR_CON2;
   phy_dqlat_con0_t DQLAT_CON0;
   phy_offsetw_con0_t OFFSETW_CON0;
   phy_offsetw_con1_t OFFSETW_CON1;
   phy_offsetw_con2_t OFFSETW_CON2;
   phy_offset_dq_con0_t OFFSET_DQ_CON0;
   phy_offsetc_con0_t OFFSETC_CON0;
   phy_offsetc_con1_t OFFSETC_CON1;
   phy_offsetc_con2_t OFFSETC_CON2;
   phy_shiftc_con0_t SHIFTC_CON0;
   phy_offsetd_con0_t OFFSETD_CON0;
   ro_bf_t reserved_0x54;
   phy_offseto_con0_t OFFSETO_CON0;
   phy_offseto_con1_t OFFSETO_CON1;
   phy_offseto_con2_t OFFSETO_CON2;
   ro_bf_t reserved_0x64_0x68[2];
   phy_wr_lvl_con0_t WR_LVL_CON0;
   phy_wr_lvl_con1_t WR_LVL_CON1;
   phy_wr_lvl_con2_t WR_LVL_CON2;
   phy_wr_lvl_con3_t WR_LVL_CON3;
   phy_ca_dskew_con0_t CA_DSKEW_CON0;
   phy_ca_dskew_con1_t CA_DSKEW_CON1;
   phy_ca_dskew_con2_t CA_DSKEW_CON2;
   ro_bf_t reserved_0x88_0x90[3];
   phy_ca_dskew_con4_t CA_DSKEW_CON4;
   phy_cal_pattern_con0_t CAL_PATTERN_CON0;
   phy_cal_pattern_con1_t CAL_PATTERN_CON1;
   phy_cal_pattern_con2_t CAL_PATTERN_CON2;
   phy_cal_pattern_con3_t CAL_PATTERN_CON3;
   phy_cal_pattern_con4_t CAL_PATTERN_CON4;
   phy_cal_pattern_con5_t CAL_PATTERN_CON5;
   phy_mdll_con0_t MDLL_CON0;
   phy_mdll_con1_t MDLL_CON1;
   phy_dvfs_con0_t DVFS_CON0;
   phy_dvfs_con1_t DVFS_CON1;
   phy_dvfs_con2_t DVFS_CON2;
   phy_dvfs_con3_t DVFS_CON3;
   phy_dvfs_con4_t DVFS_CON4;
   phy_dvfs_con5_t DVFS_CON5;
   phy_t_rddata_con0_t T_RDDATA_CON0;
   phy_t_rddata_con1_t T_RDDATA_CON1;
   phy_t_rddata_con2_t T_RDDATA_CON2;
   ro_bf_t reserved_0xDC;
   phy_cal_fail_stat0_t CAL_FAIL_STAT0;
   phy_cal_fail_stat1_t CAL_FAIL_STAT1;
   phy_cal_fail_stat2_t CAL_FAIL_STAT2;
   phy_cal_fail_stat3_t CAL_FAIL_STAT3;
   phy_cal_gt_wvmc0_t CAL_GT_WVMC0;
   phy_cal_gt_wvmc1_t CAL_GT_WVMC1;
   phy_cal_gt_wvmc2_t CAL_GT_WVMC2;
   phy_cal_gt_cyc_t CAL_GT_CYC;
   phy_cal_rd_vwmc0_t CAL_RD_VWMC0;
   phy_cal_rd_vwmc1_t CAL_RD_VWMC1;
   phy_cal_rd_vwmc2_t CAL_RD_VWMC2;
   ro_bf_t reserved_0x10C;
   phy_cal_rd_vwml0_t CAL_RD_VWML0;
   phy_cal_rd_vwml1_t CAL_RD_VWML1;
   phy_cal_rd_vwml2_t CAL_RD_VWML2;
   ro_bf_t reserved_0x11C;
   phy_cal_rd_vwmr0_t CAL_RD_VWMR0;
   phy_cal_rd_vwmr1_t CAL_RD_VWMR1;
   phy_cal_rd_vwmr2_t CAL_RD_VWMR2;
   ro_bf_t reserved_0x12C;
   phy_cal_wr_vwmc0_t CAL_WR_VWMC0;
   phy_cal_wr_vwmc1_t CAL_WR_VWMC1;
   phy_cal_wr_vwmc2_t CAL_WR_VWMC2;
   ro_bf_t reserved_0x13C;
   phy_cal_wr_vwml0_t CAL_WR_VWML0;
   phy_cal_wr_vwml1_t CAL_WR_VWML1;
   phy_cal_wr_vwml2_t CAL_WR_VWML2;
   ro_bf_t reserved_0x14C;
   phy_cal_wr_vwmr0_t CAL_WR_VWMR0;
   phy_cal_wr_vwmr1_t CAL_WR_VWMR1;
   phy_cal_wr_vwmr2_t CAL_WR_VWMR2;
   ro_bf_t reserved_0x15C;
   phy_cal_dm_vwmc0_t CAL_DM_VWMC0;
   phy_cal_dm_vwmc1_t CAL_DM_VWMC1;
   phy_cal_dm_vwmc2_t CAL_DM_VWMC2;
   ro_bf_t reserved_0x16C;
   phy_cal_dm_vwml0_t CAL_DM_VWML0;
   phy_cal_dm_vwml1_t CAL_DM_VWML1;
   phy_cal_dm_vwml2_t CAL_DM_VWML2;
   ro_bf_t reserved_0x17C;
   phy_cal_dm_vwmr0_t CAL_DM_VWMR0;
   phy_cal_dm_vwmr1_t CAL_DM_VWMR1;
   phy_cal_dm_vwmr2_t CAL_DM_VWMR2;
   ro_bf_t reserved_0x18C;
   phy_rd_deskew_con0_t RD_DESKEW_CON0;
   phy_rd_deskew_con1_t RD_DESKEW_CON1;
   phy_rd_deskew_con2_t RD_DESKEW_CON2;
   phy_rd_deskew_con3_t RD_DESKEW_CON3;
   phy_rd_deskew_con4_t RD_DESKEW_CON4;
   phy_rd_deskew_con5_t RD_DESKEW_CON5;
   phy_rd_deskew_con6_t RD_DESKEW_CON6;
   phy_rd_deskew_con7_t RD_DESKEW_CON7;
   phy_rd_deskew_con8_t RD_DESKEW_CON8;
   phy_rd_deskew_con9_t RD_DESKEW_CON9;
   phy_rd_deskew_con10_t RD_DESKEW_CON10;
   phy_rd_deskew_con11_t RD_DESKEW_CON11;
   phy_rd_deskew_con12_t RD_DESKEW_CON12;
   phy_rd_deskew_con13_t RD_DESKEW_CON13;
   phy_rd_deskew_con14_t RD_DESKEW_CON14;
   phy_rd_deskew_con15_t RD_DESKEW_CON15;
   phy_rd_deskew_con16_t RD_DESKEW_CON16;
   phy_rd_deskew_con17_t RD_DESKEW_CON17;
   phy_rd_deskew_con18_t RD_DESKEW_CON18;
   phy_rd_deskew_con19_t RD_DESKEW_CON19;
   phy_rd_deskew_con20_t RD_DESKEW_CON20;
   phy_rd_deskew_con21_t RD_DESKEW_CON21;
   phy_rd_deskew_con22_t RD_DESKEW_CON22;
   phy_rd_deskew_con23_t RD_DESKEW_CON23;
   phy_wr_deskew_con0_t WR_DESKEW_CON0;
   phy_wr_deskew_con1_t WR_DESKEW_CON1;
   phy_wr_deskew_con2_t WR_DESKEW_CON2;
   phy_wr_deskew_con3_t WR_DESKEW_CON3;
   phy_wr_deskew_con4_t WR_DESKEW_CON4;
   phy_wr_deskew_con5_t WR_DESKEW_CON5;
   phy_wr_deskew_con6_t WR_DESKEW_CON6;
   phy_wr_deskew_con7_t WR_DESKEW_CON7;
   phy_wr_deskew_con8_t WR_DESKEW_CON8;
   phy_wr_deskew_con9_t WR_DESKEW_CON9;
   phy_wr_deskew_con10_t WR_DESKEW_CON10;
   phy_wr_deskew_con11_t WR_DESKEW_CON11;
   phy_wr_deskew_con12_t WR_DESKEW_CON12;
   phy_wr_deskew_con13_t WR_DESKEW_CON13;
   phy_wr_deskew_con14_t WR_DESKEW_CON14;
   phy_wr_deskew_con15_t WR_DESKEW_CON15;
   phy_wr_deskew_con16_t WR_DESKEW_CON16;
   phy_wr_deskew_con17_t WR_DESKEW_CON17;
   phy_wr_deskew_con18_t WR_DESKEW_CON18;
   phy_wr_deskew_con19_t WR_DESKEW_CON19;
   phy_wr_deskew_con20_t WR_DESKEW_CON20;
   phy_wr_deskew_con21_t WR_DESKEW_CON21;
   phy_wr_deskew_con22_t WR_DESKEW_CON22;
   phy_wr_deskew_con23_t WR_DESKEW_CON23;
   phy_dm_deskew_con0_t DM_DESKEW_CON0;
   phy_dm_deskew_con1_t DM_DESKEW_CON1;
   phy_dm_deskew_con2_t DM_DESKEW_CON2;
   phy_vwmc_stat0_t VWMC_STAT0;
   phy_vwmc_stat1_t VWMC_STAT1;
   phy_vwmc_stat2_t VWMC_STAT2;
   phy_vwmc_stat3_t VWMC_STAT3;
   phy_vwmc_stat4_t VWMC_STAT4;
   phy_vwmc_stat5_t VWMC_STAT5;
   phy_vwmc_stat6_t VWMC_STAT6;
   phy_vwmc_stat7_t VWMC_STAT7;
   phy_vwmc_stat8_t VWMC_STAT8;
   phy_vwmc_stat9_t VWMC_STAT9;
   phy_vwmc_stat10_t VWMC_STAT10;
   phy_vwmc_stat11_t VWMC_STAT11;
   phy_vwmc_stat12_t VWMC_STAT12;
   phy_vwmc_stat13_t VWMC_STAT13;
   phy_vwmc_stat14_t VWMC_STAT14;
   phy_vwmc_stat15_t VWMC_STAT15;
   phy_vwmc_stat16_t VWMC_STAT16;
   phy_vwmc_stat17_t VWMC_STAT17;
   phy_vwmc_stat18_t VWMC_STAT18;
   phy_vwmc_stat19_t VWMC_STAT19;
   phy_vwmc_stat20_t VWMC_STAT20;
   phy_vwmc_stat21_t VWMC_STAT21;
   phy_vwmc_stat22_t VWMC_STAT22;
   phy_vwmc_stat23_t VWMC_STAT23;
   phy_dm_vwmc_stat0_t DM_VWMC_STAT0;
   phy_dm_vwmc_stat1_t DM_VWMC_STAT1;
   phy_dm_vwmc_stat2_t DM_VWMC_STAT2;
   phy_vwml_stat0_t VWML_STAT0;
   phy_vwml_stat1_t VWML_STAT1;
   phy_vwml_stat2_t VWML_STAT2;
   phy_vwml_stat3_t VWML_STAT3;
   phy_vwml_stat4_t VWML_STAT4;
   phy_vwml_stat5_t VWML_STAT5;
   phy_vwml_stat6_t VWML_STAT6;
   phy_vwml_stat7_t VWML_STAT7;
   phy_vwml_stat8_t VWML_STAT8;
   phy_vwml_stat9_t VWML_STAT9;
   phy_vwml_stat10_t VWML_STAT10;
   phy_vwml_stat11_t VWML_STAT11;
   phy_vwml_stat12_t VWML_STAT12;
   phy_vwml_stat13_t VWML_STAT13;
   phy_vwml_stat14_t VWML_STAT14;
   phy_vwml_stat15_t VWML_STAT15;
   phy_vwml_stat16_t VWML_STAT16;
   phy_vwml_stat17_t VWML_STAT17;
   phy_vwml_stat18_t VWML_STAT18;
   phy_vwml_stat19_t VWML_STAT19;
   phy_vwml_stat20_t VWML_STAT20;
   phy_vwml_stat21_t VWML_STAT21;
   phy_vwml_stat22_t VWML_STAT22;
   phy_vwml_stat23_t VWML_STAT23;
   phy_dm_vwml_stat1_t DM_VWML_STAT1;
   phy_dm_vwml_stat2_t DM_VWML_STAT2;
   phy_dm_vwml_stat3_t DM_VWML_STAT3;
   phy_vwmr_stat0_t VWMR_STAT0;
   phy_vwmr_stat1_t VWMR_STAT1;
   phy_vwmr_stat2_t VWMR_STAT2;
   phy_vwmr_stat3_t VWMR_STAT3;
   phy_vwmr_stat4_t VWMR_STAT4;
   phy_vwmr_stat5_t VWMR_STAT5;
   phy_vwmr_stat6_t VWMR_STAT6;
   phy_vwmr_stat7_t VWMR_STAT7;
   phy_vwmr_stat8_t VWMR_STAT8;
   phy_vwmr_stat9_t VWMR_STAT9;
   phy_vwmr_stat10_t VWMR_STAT10;
   phy_vwmr_stat11_t VWMR_STAT11;
   phy_vwmr_stat12_t VWMR_STAT12;
   phy_vwmr_stat13_t VWMR_STAT13;
   phy_vwmr_stat14_t VWMR_STAT14;
   phy_vwmr_stat15_t VWMR_STAT15;
   phy_vwmr_stat16_t VWMR_STAT16;
   phy_vwmr_stat17_t VWMR_STAT17;
   phy_vwmr_stat18_t VWMR_STAT18;
   phy_vwmr_stat19_t VWMR_STAT19;
   phy_vwmr_stat20_t VWMR_STAT20;
   phy_vwmr_stat21_t VWMR_STAT21;
   phy_vwmr_stat22_t VWMR_STAT22;
   phy_vwmr_stat23_t VWMR_STAT23;
   phy_dm_vwmr_stat0_t DM_VWMR_STAT0;
   phy_dm_vwmr_stat1_t DM_VWMR_STAT1;
   phy_dm_vwmr_stat2_t DM_VWMR_STAT2;
   phy_dq_io_rdata0_t DQ_IO_RDATA0;
   phy_dq_io_rdata1_t DQ_IO_RDATA1;
   phy_dq_io_rdata2_t DQ_IO_RDATA2;
   phy_version_info_stat0_t VERSION_INFO_STAT0;
   ro_bf_t reserved_0x3B0_0x3C4[6];
   phy_zq_con0_t ZQ_CON0;
   phy_zq_con1_t ZQ_CON1;
   phy_zq_con2_t ZQ_CON2;
   phy_zq_con3_t ZQ_CON3;
   phy_zq_con4_t ZQ_CON4;
   phy_zq_con5_t ZQ_CON5;
   phy_zq_con6_t ZQ_CON6;
   phy_zq_con7_t ZQ_CON7;
   phy_zq_con8_t ZQ_CON8;
   phy_zq_con9_t ZQ_CON9;
   phy_zq_con10_t ZQ_CON10;
   phy_zq_con11_t ZQ_CON11;
   phy_zq_con12_t ZQ_CON12;
   phy_zq_con13_t ZQ_CON13;
   ro_bf_t reserved_0x400_0x5F0[125];
   phy_dqs_osc_con0_t DQS_OSC_CON0;
   phy_dqs_osc_con1_t DQS_OSC_CON1;
   phy_dqs_osc_con2_t DQS_OSC_CON2;
   phy_dqs_osc_con3_t DQS_OSC_CON3;
   phy_dqs_osc_con4_t DQS_OSC_CON4;
   phy_cbt_con0_t CBT_CON0;
} phy_regs_t;

#endif

#endif
