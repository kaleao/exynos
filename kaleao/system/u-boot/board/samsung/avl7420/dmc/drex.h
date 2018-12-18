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

#ifndef DREX_H_
#define DREX_H_

#if defined( DREX1_V2_0_3 )
	#include "drex1_v2_0_3.h"
	#include "tzasc.h"
#elif defined( DREX1_V2_0_4 )
	#include "drex1_v2_0_4.h"
	#include "tzasc.h"
#elif defined( DREX1_V2_0_5 )
	#include "drex1_v2_0_5.h"
	#include "asp.h"
#else
	#error
#endif

#ifndef __ASSEMBLY__

typedef enum {
	DREX_DFI_UPDATE_MC = 0,
	DREX_DFI_UPDATE_PHY = 1,
	DREX_DFI_UPDATE_IGNORE = 2,
} drex_dfi_update_mode_t;

typedef enum {
	DREX_MRW_COMMAND  	= 0x0,
	DREX_PALL_COMMAND 	= 0x1,
	DREX_PRE_COMMAND  	= 0x2,
	DREX_MPC_COMMAND	= 0x3,
	DREX_SRE_COMMAND 	= 0x4,
	DREX_REFA_COMMAND  	= 0x5,
	DREX_PDE_COMMAND	= 0x6,
	DREX_PDX_COMMAND	= 0x7,
	DREX_SRX_COMMAND	= 0x8,
	DREX_MRR_COMMAND	= 0x9,
	DREX_ACT_COMMAND	= 0xe,
} drex_direct_command_t;

#endif

#endif /* DREX_H_ */
