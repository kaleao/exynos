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

#ifndef ASP_H_
#define ASP_H_

#ifndef __ASSEMBLY__

typedef struct {
   ro_bf_t no_of_regions             : ( 3 - 0 + 1 );
   ro_bf_t reserved_4_7                 : ( 7 - 4 + 1 );
   ro_bf_t address_width             : ( 13 - 8 + 1 );
   ro_bf_t reserved_14_31               : ( 31 - 14 + 1 );
} asp_tzconfig_t;

typedef struct {
   rw_bf_t reaction_value            : ( 1 - 0 + 1 );
   ro_bf_t reserved_2_31                : ( 31 - 2 + 1 );
} asp_tzaction_t;

typedef struct {
   rw_bf_t lockdown_regions          : ( 8 - 0 + 1 );
   ro_bf_t reserved_9_30                : ( 30 - 9 + 1 );
   rw_bf_t enable                    : ( 31 - 31 + 1 );
} asp_tzldrange_t;

typedef struct {
   rw_bf_t region_register           : ( 0 - 0 + 1 );
   ro_bf_t reserved_1_31                : ( 31 - 1 + 1 );
} asp_tzldselect_t;

typedef struct {
   ro_bf_t status_r0                 : ( 0 - 0 + 1 );
   ro_bf_t overrun_r0                : ( 1 - 1 + 1 );
   ro_bf_t status_w0                 : ( 2 - 2 + 1 );
   ro_bf_t overrun_w0                : ( 3 - 3 + 1 );
   ro_bf_t status_r1                 : ( 4 - 4 + 1 );
   ro_bf_t overrun_r1                : ( 5 - 5 + 1 );
   ro_bf_t status_w1                 : ( 6 - 6 + 1 );
   ro_bf_t overrun_w1                : ( 7 - 7 + 1 );
   ro_bf_t status_r2                 : ( 8 - 8 + 1 );
   ro_bf_t overrun_r2                : ( 9 - 9 + 1 );
   ro_bf_t status_w2                 : ( 10 - 10 + 1 );
   ro_bf_t overrun_w2                : ( 11 - 11 + 1 );
   ro_bf_t status_r3                 : ( 12 - 12 + 1 );
   ro_bf_t overrun_r3                : ( 13 - 13 + 1 );
   ro_bf_t status_w3                 : ( 14 - 14 + 1 );
   ro_bf_t overrun_w3                : ( 15 - 15 + 1 );
   ro_bf_t reserved_16_31               : ( 31 - 16 + 1 );
} asp_tzinitstatus_t;

typedef struct {
   ro_bf_t int_clear                 : ( 31 - 0 + 1 );
} asp_tzinitclear_t;

typedef struct {
   ro_bf_t fail_addr_low             : ( 31 - 0 + 1 );
} asp_tzfailaddrlowr_t;

typedef struct {
   ro_bf_t fail_addr_high            : ( 2 - 0 + 1 );
   ro_bf_t reserved_3_31                : ( 31 - 3 + 1 );
} asp_tzfailaddrhighr_t;

typedef struct {
   ro_bf_t reserved_0_19                : ( 19 - 0 + 1 );
   ro_bf_t privileged                : ( 20 - 20 + 1 );
   ro_bf_t nonsecure                 : ( 21 - 21 + 1 );
   ro_bf_t reserved_22_23               : ( 23 - 22 + 1 );
   ro_bf_t write                     : ( 24 - 24 + 1 );
   ro_bf_t reserved_25_31               : ( 31 - 25 + 1 );
} asp_tzfailctrlr_t;

typedef struct {
   ro_bf_t axid                      : ( 15 - 0 + 1 );
   ro_bf_t reserved_16_31               : ( 31 - 16 + 1 );
} asp_tzfailidr_t;

typedef struct {
   ro_bf_t fail_addr_low             : ( 31 - 0 + 1 );
} asp_tzfailaddrloww_t;

typedef struct {
   ro_bf_t fail_addr_high            : ( 2 - 0 + 1 );
   ro_bf_t reserved_3_31                : ( 31 - 3 + 1 );
} asp_tzfailaddrhighw_t;

typedef struct {
   ro_bf_t reserved_0_19                : ( 19 - 0 + 1 );
   ro_bf_t privileged                : ( 20 - 20 + 1 );
   ro_bf_t nonsecure                 : ( 21 - 21 + 1 );
   ro_bf_t reserved_22_23               : ( 23 - 22 + 1 );
   ro_bf_t write                     : ( 24 - 24 + 1 );
   ro_bf_t reserved_25_31               : ( 31 - 25 + 1 );
} asp_tzfailctrlw_t;

typedef struct {
   ro_bf_t axid                      : ( 15 - 0 + 1 );
   ro_bf_t rsvd0                     : ( 31 - 16 + 1 );
} asp_tzfailidw_t;

typedef struct {
   ro_bf_t reserved_0_15                : ( 15 - 0 + 1 );
   rw_bf_t base_address_low          : ( 31 - 16 + 1 );
} asp_tzrslow_t;

typedef struct {
   rw_bf_t base_address_high         : ( 2 - 0 + 1 );
   ro_bf_t reserved_3_31                 : ( 31 - 3 + 1 );
} asp_tzrshigh_t;

typedef struct {
   ro_bf_t en                        : ( 0 - 0 + 1 );
   ro_bf_t reserved_1_3                 : ( 3 - 1 + 1 );
   ro_bf_t size                      : ( 22 - 4 + 1 );
   ro_bf_t reserved_23_27               : ( 27 - 23 + 1 );
   rw_bf_t sp                        : ( 31 - 28 + 1 );
} asp_tzrsattr_t;

typedef struct {
   rw_bf_t int_test_en               : ( 0 - 0 + 1 );
   ro_bf_t reserved_1_31                : ( 31 - 1 + 1 );
} asp_tzitcrg_t;

typedef struct {
   ro_bf_t secure_boot_lock          : ( 0 - 0 + 1 );
   ro_bf_t reserved_1_31                : ( 31 - 1 + 1 );
} asp_tzitip_t;

typedef struct {
   rw_bf_t itop_int                  : ( 0 - 0 + 1 );
   ro_bf_t reserved_1_31                : ( 31 - 1 + 1 );
} asp_tzitop_t;

typedef struct {
   rw_bf_t chunk_end                 : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_15                : ( 15 - 8 + 1 );
   rw_bf_t chunk_start               : ( 23 - 16 + 1 );
   ro_bf_t reserved_24_31               : ( 31 - 24 + 1 );
} asp_membaseconfig0_t;

typedef struct {
   rw_bf_t chunk_end                 : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_15                : ( 15 - 8 + 1 );
   rw_bf_t chunk_start               : ( 23 - 16 + 1 );
   ro_bf_t reserved_24_31               : ( 31 - 24 + 1 );
} asp_membaseconfig1_t;

typedef struct {
   rw_bf_t chip_bank                 : ( 3 - 0 + 1 );
   rw_bf_t chip_row                  : ( 7 - 4 + 1 );
   rw_bf_t chip_col                  : ( 11 - 8 + 1 );
   rw_bf_t chip_map                  : ( 15 - 12 + 1 );
   rw_bf_t bit_sel0                  : ( 16 - 16 + 1 );
   rw_bf_t bit_sel1                  : ( 17 - 17 + 1 );
   rw_bf_t bit_sel_en_0              : ( 18 - 18 + 1 );
   rw_bf_t bit_sel_en_1              : ( 19 - 19 + 1 );
   rw_bf_t bank_lsb                  : ( 22 - 20 + 1 );
   ro_bf_t reserved_23                  : ( 23 - 23 + 1 );
   rw_bf_t rank_inter_en             : ( 24 - 24 + 1 );
   ro_bf_t reserved_25_31               : ( 31 - 25 + 1 );
} asp_memconfig0_t;

typedef struct {
   rw_bf_t chip_bank                 : ( 3 - 0 + 1 );
   rw_bf_t chip_row                  : ( 7 - 4 + 1 );
   rw_bf_t chip_col                  : ( 11 - 8 + 1 );
   rw_bf_t chip_map                  : ( 15 - 12 + 1 );
   rw_bf_t bit_sel0                  : ( 16 - 16 + 1 );
   rw_bf_t bit_sel1                  : ( 17 - 17 + 1 );
   rw_bf_t bit_sel_en_0              : ( 18 - 18 + 1 );
   rw_bf_t bit_sel_en_1              : ( 19 - 19 + 1 );
   rw_bf_t bank_lsb                  : ( 22 - 20 + 1 );
   ro_bf_t reserved_23                  : ( 23 - 23 + 1 );
   rw_bf_t rank_inter_en             : ( 24 - 24 + 1 );
   ro_bf_t reserved_25_31               : ( 31 - 25 + 1 );
} asp_memconfig1_t;

typedef struct {
   rw_bf_t chip0_size                : ( 7 - 0 + 1 );
   ro_bf_t reserved_8_31                : ( 31 - 8 + 1 );
} asp_memsize0_t;


typedef struct {
	asp_tzfailaddrlowr_t TZFAILADDRLOWR;
	asp_tzfailaddrhighr_t TZFAILADDRHIGHR;
	asp_tzfailctrlr_t TZFAILCTRLR;
	asp_tzfailidr_t TZFAILIDR;
	asp_tzfailaddrloww_t TZFAILADDRLOWW;
	asp_tzfailaddrhighw_t TZFAILADDRHIGHW;
	asp_tzfailctrlw_t TZFAILCTRLW;
	asp_tzfailidw_t TZFAILIDW;
} asp_tzfail_set_t;

typedef struct {
	asp_tzrslow_t TZRSLOW;
	asp_tzrshigh_t TZRSHIGH;
	asp_tzrsattr_t TZRSATTR;
	ro_bf_t reserved_0_31;
} asp_tzrs_set_t;

typedef struct {
#if 0
	asp_tzconfig_t TZCONFIG;
	asp_tzaction_t TZACTION;
	asp_tzldrange_t TZLDRANGE;
	asp_tzldselect_t TZLDSELECT;
	asp_tzinitstatus_t TZINITSTATUS;
	asp_tzinitclear_t TZINITCLEAR;
	ro_bf_t reserved_0x18_0x3C[10];
	asp_tzfail_set_t TZFAIL[4];
	ro_bf_t reserved_0xc0_0xfc[16];
	asp_tzrs_set_t TZRS[8];
	ro_bf_t reserved_0x17c_0xdfc[801];
	asp_tzitcrg_t TZITCRG;
	asp_tzitip_t TZITIP;
	asp_tzitop_t TZITOP;
	ro_bf_t reserved_0xe0c_0xefc[61];
#endif
	asp_membaseconfig0_t MEMBASECONFIG0;
	asp_membaseconfig1_t MEMBASECONFIG1;
	ro_bf_t reserved_0x8_0xc[2];
	asp_memconfig0_t MEMCONFIG0;
	asp_memconfig1_t MEMCONFIG1;
	ro_bf_t reserved_0x18_0x1c[2];
	asp_memsize0_t MEMSIZE0;
} asp_regs_t;

#endif

#endif
