/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *
 * DFD for Exynos
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <asm-offsets.h>
#include <config.h>
#include <version.h>
#include <linux/linkage.h>
#include <asm/macro.h>
#include "dfd.h"

ENTRY(dfd_entry_dump_gpr)
	/* Check to allow accessing FP/SIMD register */
	mov	x28, x30
	mov	x0, #3 << 20
	msr	cpacr_el1, x0			/* Enable FP/SIMD */

	/* Get the core address of ramdump by each core */
	bl	dfd_get_core_num
	mov	x7, x0
	ldr	x3, =(CONFIG_RAMDUMP_COREREG)
	mov	x2, x0
	ldr	x4, =(DFD_COREREG_OFFSET)
	mul	x2, x2, x4
	add	x3, x3, x2

	mov	x1, x0				/* x1 is core number */
	mov	x0, x3				/* x0 is target address */
	bl	dfd_dump_gpr
	bl	dfd_dump_pc

	/* Debug for Cache Flush of dumpGPR : Save initial value 0 */
	mov	w3, #0
	bl	dfd_debug_dump_gpr

	/* Flush cache to get the latest data */
	mov	x0, x7
	bl	dfd_check_core_stat

	/*
	 * x0 = 0, power off, skip cache Flush
	 * x0 = 1, needs Level1(L1=>L2) cache Flush
	 * x0 = 2, needs Level2(L2=>DRAM) cache Flush
	 * x0 = 3, needs Level1, Level2 cache Flush
	 */
	cbz	w0, 2f				/* Skip cache flush */

	cmp	w0, #3				/* Needs all flush */
	b.eq	3f

	/* Debug for Cache Flush of dumpGPR : Save the level of cache flush */
	mov	w3, w0
	bl	dfd_debug_dump_gpr

	sub	w0, w0, #1			/* Sync csselr_el1 bits value */
	mov	x1, #0				/* Only Flush */

	bl	__asm_flush_dcache_level
	mov     x0, #0
	msr     csselr_el1, x0
	dsb     sy
	isb
	b	2f
3:
	/* Debug for Cache Flush of dumpGPR : Save the level of cache flush all */
	mov	w3, w0
	bl	dfd_debug_dump_gpr

	/* Run cache flush all to Level1,2 */
	bl	__asm_flush_dcache_all
2:
	/* Write own bit to inform finishing Dump GPR */
	bl	dfd_get_core_num
	ldr	x1, =CONFIG_RAMDUMP_DUMP_GPR_WAIT
	ldr	w2, [x1]
	mov	w3, #1
	lsl	w3, w3, w0
	orr	w2, w2, w3
	str	w2, [x1]

	/* core0 skip into wfi */
	cbz	w0, 1f

	isb
	dsb	sy
	wfi
1:
	mov	x30, x28
	ret
ENDPROC(dfd_entry_dump_gpr)

ENTRY(dfd_dump_gpr)
	/* Store dump gpr register to dram, x0: target address
	 * The offset is following as kernel's struct pt_regs */
	str	d8, [x0], #8	/* x0 */
	str	d9, [x0], #8	/* x1 */
	str	d10, [x0], #8	/* x2 */
	str	d11, [x0], #8	/* x3 */
	str	d12, [x0], #8	/* x4 */
	str	d13, [x0], #8	/* x5 */
	str	d14, [x0], #8	/* x6 */
	str	d15, [x0], #8	/* x7 */
	str	d16, [x0], #8	/* x8 */
	str	d17, [x0], #8	/* x9 */
	str	d18, [x0], #8	/* x10 */
	str	d19, [x0], #8	/* x11 */
	str	d20, [x0], #8	/* x12 */
	str	d21, [x0], #8	/* x13 */
	str	d22, [x0], #8	/* x14 */
	str	d23, [x0], #8	/* x15 */
	str	d24, [x0], #8	/* x16 */
	str	d25, [x0], #8	/* x17 */
	str	d26, [x0], #8	/* x18 */
	str	d27, [x0], #88	/* x19 */
	str	d28, [x0], #8	/* x30 */
	str	d29, [x0], #8	/* sp */
	ret
ENDPROC(dfd_dump_gpr)

ENTRY(dfd_dump_pc)
	/* x0: target address, x1: core number */
	ldr	x2, =DFD_APOLLO_DUMP_PC_ADDRESS
	cmp	x1, #3
	b.ls	1f
	ldr	x2, =DFD_ATLAS_DUMP_PC_ADDRESS
	sub	x1, x1, #4
1:
	ldr	x3, [x2, x1, lsl #3]
	str	x3, [x0]
	ret
ENDPROC(dfd_dump_pc)

ENTRY(dfd_get_core_num)
	mrs     x1, mpidr_el1
	and     x0, x1, #0xf            /* x0 = cpu id */
	lsr     x1, x1, #8
	and     x1, x1, #0xf            /* x1 = cluster id */
	cmp	x1, #1
	b.eq	1f
	add	x0, x0, #4		/* reorder cpu id */
1:
	ret				/* r0(cpu id) is return */
ENDPROC(dfd_get_core_num)

ENTRY(dfd_debug_dump_gpr)
	/* x3 is save data, and this function uses only x3, x4 registers */
	ldr	x4, =(CONFIG_RAMDUMP_DUMP_GPR_DEBUG)
	add     x4, x4, x7, lsl #2
	str	w3, [x4]
	ret
ENDPROC(dfd_debug_dump_gpr)

ENTRY(dfd_check_core_stat)
	/* x0 is core number */
	ldr	x1, =CONFIG_RAMDUMP_GPR_POWER_STAT
	ldr     w2, [x1, x0, lsl #2]
	lsr	w2, w2, #16
	mov 	x0, x2
	ret
ENDPROC(dfd_check_core_stat)
