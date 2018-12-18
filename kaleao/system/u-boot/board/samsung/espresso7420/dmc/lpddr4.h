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

#ifndef LPDDR4_H_
#define LPDDR4_H_


#ifndef __ASSEMBLY__

typedef enum {
	LPDDR4_MPC1_NOP = 0,
	LPDDR4_MPC1_RD_FIFO = 0x41,
	LPDDR4_MPC1_RD_DQ_CALIBRATION = 0x43,
	LPDDR4_MPC1_WR_FIFO = 0x47,
	LPDDR4_MPC1_START_DQS_OSC = 0x4b,
	LPDDR4_MPC1_STOP_DQS_OSC = 0x4d,
	LPDDR4_MPC1_ZQCAL_START = 0x4f,
	LPDDR4_MPC1_ZQCAL_LATCH = 0x51,
} lpddr4_mpc_mode_t;

typedef struct {
	unsigned char training;
	unsigned char mr14;
} lpddr4_vref_training_t;

typedef struct {
	unsigned char pattern;
	unsigned char invert;
} lppdr4_cal_pattern_t;

typedef enum {
	LPDDR4_COMMAND_BUS_TRAINING_ENABLE = ( 1 << 0 ),
	LPDDR4_CA_VREF_TRAINING_ENABLE = (1 << 1 ),
	LPDDR4_READ_CALIBRATION_ENABLE = ( 1 << 2 ),
	LPDDR4_READ_VREF_TRAINING_ENABLE = (1 << 3 ),
	LPDDR4_WRITE_CALIBRATION_ENABLE = ( 1 << 4 ),
	LPDDR4_WRITE_VREF_TRAINING_ENABLE = (1 << 5 ),
	LPDDR4_WRITE_LEVELING_ENABLE = ( 1 << 8 ),
	LPDDR4_GATE_TRAINING_ENABLE = ( 1 << 9 ),
} lpddr4_training_t;

typedef struct {
	const unsigned char reserved_0_2	: 3;
	const unsigned char RZQI			: 2;
	const unsigned char PPP			: 1;
	const unsigned char reserved_6	: 1;
	const unsigned char CATR			: 1;
} lp4_mr0_op_t;

typedef struct {
	unsigned char BL					: 2;
	unsigned char WR_PRE				: 1;
	unsigned char RD_PRE				: 1;
	unsigned char nWR					: 3;
	unsigned char PST					: 1;
} lp4_mr1_op_t;

typedef struct {
	unsigned char RL					: 3;
	unsigned char WL					: 3;
	unsigned char WLS					: 1;
	unsigned char WR_Leveling			: 1;
} lp4_mr2_op_t;

typedef struct {
	unsigned char PU_CAL				: 1;
	const unsigned char reserved_1_2	: 2;
	unsigned char PDDS					: 3;
	unsigned char DBI_RD				: 1;
	unsigned char DBI_WR				: 1;
} lp4_mr3_op_t;

typedef struct {
	unsigned char Refresh_Rate			: 3;
	const unsigned char reserved_3	: 1;
	unsigned char PPRE					: 1;
	const unsigned char reserved_5_6	: 2;
	unsigned char TUF					: 1;
} lp4_mr4_op_t;

typedef struct {
	unsigned char ZQ_Reset				: 1;
	const unsigned char reserved_1_7	: 7;
} lp4_mr10_op_t;

typedef struct {
	unsigned char DQ_ODT				: 3;
	unsigned char CA_ODT				: 3;
	const unsigned char reserved_6_7	: 2;
} lp4_mr11_op_t;

typedef struct {
	unsigned char VREF_CA				: 6;
	unsigned char VR_CA				: 1;
	const unsigned char reserved_7	: 2;
} lp4_mr12_op_t;

typedef struct {
	unsigned char CBT					: 1;
	const unsigned char reserved_1	: 1;
	unsigned char VRO					: 1;
	unsigned char VRCG					: 1;
	const unsigned char reserved_4	: 1;
	unsigned char DM_DIS				: 1;
	unsigned char FSP_WR				: 1;
	unsigned char FSP_OP				: 1;
} lp4_mr13_op_t;

typedef struct {
	unsigned char VREF_DQ				: 6;
	unsigned char VR_DQ				: 1;
	const unsigned char reserved_7	: 2;
} lp4_mr14_op_t;

typedef struct {
	unsigned char CODT					: 3;
	unsigned char ODTE_CK				: 2;
	unsigned char ODTE_CS				: 1;
	unsigned char ODTD_CA				: 1;
	const unsigned char reserved_7	: 1;
} lp4_mr22_op_t;

typedef struct {
	lp4_mr1_op_t MR1;
	lp4_mr2_op_t MR2;
	lp4_mr3_op_t MR3;
	lp4_mr11_op_t MR11;
	lp4_mr12_op_t MR12;
	lp4_mr14_op_t MR14;
	lp4_mr22_op_t MR22;
	const unsigned char dummy;
} lp4_fsp_t;

typedef struct {
	int freq;
	int read_preamble;
	int read_postamble;
	int write_preamble;
	int pu_cal;
	int pdds;
	int dbi_rd;
	int dbi_wr;
	int dq_odt;
	int ca_odt;
	int vref_dq;
	int vref_ca;
	int command_bus;
	int gate_training;
	int read_dq_cal;
	int write_dq_cal;
} lp4_conf_t;

extern int Lpddr4Init(void);

#endif

#endif /* LPDDR4_H_ */
