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

#ifndef MEMORY_H_
#define MEMORY_H_

#if defined( LPDDR3 )
	#include "lpddr3.h"
#elif defined( LPDDR4 )
	#include "lpddr4.h"
#elif defined( DDR3 )
	#include "ddr3.h"
#elif defined( DDR4 )
	#include "ddr4.h"
#else
	#error
#endif

typedef enum {
	MEM_VECTOR_DMA_MEMCPY = 1,
	MEM_VECTOR_EVEN_MODE,
	MEM_VECTOR_ODD_MODE,
	MEM_VECTOR_STUCK_ADDRESS,
	MEM_VECTOR_RANDOM,
	MEM_VECTOR_XOR,
	MEM_VECTOR_SUB,
	MEM_VECTOR_MUL,
	MEM_VECTOR_DIV,
	MEM_VECTOR_OR,
	MEM_VECTOR_AND,
	MEM_VECTOR_SEQUENTIAL_INCREMENTAL,
	MEM_VECTOR_SOLIDBITS,
	MEM_VECTOR_CHECKERBOARD,
	MEM_VECTOR_BLOCKSEQUENTIAL,
	MEM_VECTOR_WALKBITS0,
	MEM_VECTOR_WALKBITS1,
	MEM_VECTOR_BITSPREAD,
	MEM_VECTOR_BITFLIP,
} mem_vector_id_t;



#endif /* MEMORY_H_ */
