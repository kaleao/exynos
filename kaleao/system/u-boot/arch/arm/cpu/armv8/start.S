/*
 * (C) Copyright 2013
 * David Feng <fenghua@phytium.com.cn>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <asm-offsets.h>
#include <config.h>
#include <version.h>
#include <linux/linkage.h>
#include <asm/macro.h>
#include <asm/armv8/mmu.h>

/*************************************************************************
 *
 * Startup Code (reset vector)
 *
 *************************************************************************/

.globl	_start
_start:
	b	reset

	.align 3

.globl	_TEXT_BASE
_TEXT_BASE:
	.quad	CONFIG_SYS_TEXT_BASE

/*
 * These are defined in the linker script.
 */
.globl	_end_ofs
_end_ofs:
	.quad	_end - _start

.globl	_bss_start_ofs
_bss_start_ofs:
	.quad	__bss_start - _start

.globl	_bss_end_ofs
_bss_end_ofs:
	.quad	__bss_end - _start

#ifdef CONFIG_SPL_BUILD
/*
 * Exception vectors.
 */
	.align	3
vectors:
bad_sync:
	b	.		/* Current EL Synchronous Thread */
bad_irq:
	b	.		/* Current EL IRQ Thread */
bad_fiq:
	b	.		/* Current EL FIQ Thread */
bad_error:
	b	.		/* Current EL Error Thread */
do_sync:
	b	.		/* Current EL Synchronous Handler */
do_irq:
	b	.		/* Current EL IRQ Handler */
do_fiq:
	b	.		/* Current EL FIQ Handler */
do_error:
	b	.		/* Current EL Error Handler */
#endif
reset:
	/*
	 * Could be EL3/EL2/EL1, Initial State:
	 * Little Endian, MMU Disabled, i/dCache Disabled
	 */
	adr	x0, vectors
	switch_el x1, 3f, 2f, 1f
3:	mrs	x0, scr_el3
	orr	x0, x0, #0xf			/* SCR_EL3.NS|IRQ|FIQ|EA */
	msr	scr_el3, x0
	msr	vbar_el3, x0
	msr	cptr_el3, xzr			/* Enable FP/SIMD */
	b	0f
2:	msr	vbar_el2, x0
	mov	x0, #0x33ff
	msr	cptr_el2, x0			/* Enable FP/SIMD */
	b	0f
1:	msr	vbar_el1, x0
	mov	x0, #3 << 20
	msr	cpacr_el1, x0			/* Enable FP/SIMD */
0:

	/*
	 * Cache/BPB/TLB Invalidate
	 * i-cache is invalidated before enabled in icache_enable()
	 * tlb is invalidated before mmu is enabled in dcache_enable()
	 * d-cache is invalidated before enabled in dcache_enable()
	 */

	/* Processor specific initialization */
	bl	lowlevel_init

/* Set stackpointer in internal RAM to call board_init_f */
call_board_init_f:
	ldr	x0, =(CONFIG_SYS_INIT_SP_ADDR)
	bic	sp, x0, #0xf    /* 16-byte alignment for ABI compliance */
	mov	x0, #0
	bl	board_init_f

/*------------------------------------------------------------------------------*/

/*
 * void relocate_code (addr_sp, gd, addr_moni)
 *
 * This "function" does not return, instead it continues in RAM
 * after relocating the monitor code.
 *
 */
ENTRY(relocate_code)
	mov	x4, x0	/* save addr_sp */
	mov	x5, x1	/* save addr of gd */
	mov	x6, x2	/* save addr of destination */

	/* Set up the stack						    */
stack_setup:
	mov	sp, x4
	mov	x9, #0			/* no relocation. relocation offset(x9) = 0 */

clear_bss:
#ifdef CONFIG_SPL_BUILD
	/* No relocation for SPL */
	ldr	x0, =__bss_start
	ldr	x1, =__bss_end
#else
	ldr	x0, _bss_start_ofs
	ldr	x1, _bss_end_ofs
	mov	x4, x6			/* reloc addr */
	add	x0, x0, x4
	add	x1, x1, x4
#endif
	mov	x2, #0x00000000		/* clear */

clbss_l:cmp	x0, x1			/* clear loop... */
	bhs	clbss_e			/* if reached end of bss, exit */
	str	x2, [x0]
	add	x0, x0, #8
	b	clbss_l
clbss_e:

/*
 * We are done. Do not return, instead branch to second part of board
 * initialization, now running from RAM.
 */
jump_2_ram:
/*
 * Move vector table
 */
	ldr	x0, _board_init_r_ofs
	adr	x1, _start
	add	x2, x0, x1
	add	x2, x2, x9
	/* setup parameters for board_init_r */
	mov	x0, x5		/* gd_t */
	mov	x1, x6		/* dest_addr */
	/* jump to it ... */
	br	x2

	.align 3
_board_init_r_ofs:
	.quad board_init_r - _start
ENDPROC(relocate_code)
