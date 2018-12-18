/*
 * Lowlevel setup for EXYNOS7420
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
#include <version.h>
#include <asm/arch/cpu.h>

#include "dfd.h"

/* #define SCRATCH_INFORM_DEBUG */
//#define CUSTOM_PMIC_WRSTI
#define NEW_AVL_PMIC_WRSTI
//#define OLD_AVL_PMIC_WRSTI
/* Multi Core Timer */
#define G_TCON_OFFSET		0x0240
#define GLOBAL_FRC_ENABLE	0x100

#define PSHOLD_CONTROL_OFFSET	0x330C
#define GPV7CON_OFFSET		0x0000
#define GPV7DAT_OFFSET		0x0004
#define GPF0CON_OFFSET		0x0020
#define GPF0DAT_OFFSET		0x0024
#define GPF0PUD_OFFSET		0x0028
#define GPF1CON_OFFSET		0x01E0
#define GPF1DAT_OFFSET		0x01E4
#define GPF3CON_OFFSET		0x0080
#define GPF3DAT_OFFSET		0x0084
#define GPF3PUD_OFFSET		0x0088
#define GPX2PUD_OFFSET		0x0C48
#define GPA2PUD_OFFSET		0x0048
#define GPA3CON_OFFSET		0x0060
#define GPA3DAT_OFFSET		0x0064
#define GPA3PUD_OFFSET		0x0068
#define GPF2CON_OFFSET		0x0060
#define GPF2DAT_OFFSET		0x0064
#define GPF2PUD_OFFSET		0x0068

/* cpu_state flag */
#define RESET			(1 << 0)
#define RESERVED		(1 << 1)
#define HOTPLUG			(1 << 2)
#define C2_STATE		(1 << 3)
#define CLUSTER_OFF		(1 << 4)

.macro	push, xreg1, xreg2
stp	\xreg1, \xreg2, [sp, #-16]!
.endm

.macro	pop, xreg1, xreg2
ldp	\xreg1, \xreg2, [sp], #16
.endm

_TEXT_BASE:
	.word	CONFIG_SYS_TEXT_BASE

	.globl lowlevel_init
lowlevel_init:
	/* use iRAM stack in bl2 */
	ldr	x0, =(CONFIG_IRAM_STACK)
	bic	sp, x0, #0xf
	push	x30, x29

#if defined(CONFIG_SPL_BUILD) && defined(SCRATCH_INFORM_DEBUG)
	ldr	x0, =0x10000
	bl	scratch_inform
#endif
	/* check warm reset status */
	bl	check_warm_reset

	/* check reset status */
	ldr	x0, =(EXYNOS7420_POWER_BASE + INFORM1_OFFSET)
	ldr	w1, [x0]

#ifndef CONFIG_SG_BINARY
	/* AFTR wakeup reset */
	ldr	w2, =S5P_CHECK_DIDLE
	cmp	w1, w2
	beq	exit_wakeup

	/* LPA wakeup reset */
	ldr	w2, =S5P_CHECK_LPA
	cmp	w1, w2
	beq	exit_wakeup
#endif

	/* Sleep wakeup reset */
	ldr	w2, =S5P_CHECK_SLEEP
	cmp	w1, w2
	beq	wakeup_reset

#ifndef CONFIG_SG_BINARY
	/* PS-Hold high */
	ldr	x0, =(EXYNOS7420_POWER_BASE + PSHOLD_CONTROL_OFFSET)
	ldr	w1, =0x5300
	str	w1, [x0]

	bl	pmic_gpio_init

#if defined(CONFIG_SPL_BUILD) && defined(SCRATCH_INFORM_DEBUG)
	ldr	x0, =0x20000
	bl	scratch_inform
#endif

#if defined(CONFIG_PM)
	bl	pmic_enable_peric_dev
#endif

	bl	read_om

	/*
	 * If U-boot is already running in RAM, no need to relocate U-Boot.
	 * Memory controller must be configured before relocating U-Boot
	 * in ram.
	 */
	ldr	w0, =0xffffff		/* r0 <- Mask Bits*/
	ldr	w3, =lowlevel_init
	bic	w1, w3, #0x0ffffff	/* pc <- current addr of code */
					/* r1 <- unmasked bits of pc */
	ldr	w2, _TEXT_BASE		/* r2 <- original base addr in ram */
	bic	w2, w2, #0x0ffffff	/* r2 <- unmasked bits of r2*/
	cmp	w1, w2			/* compare r1, r2 */
	beq	1f			/* r0 == r1 then skip sdram init */

	/* init asv table */
	bl	_asv_table_init

	/* if watchdog or warm && dump GPR */
	ldr	x1, =(EXYNOS7420_POWER_BASE + DFD_RESET_SEQUENCER_OFFSET)
	ldr	w2, [x1]
	and	w2, w2, #(DFD_EDPCSR_DUMP_EN)
	cmp	w2, #0
	b.eq	skip_dump_gpr

	/* check reset status */
	ldr	x1, =(EXYNOS7420_POWER_BASE + RST_STAT_OFFSET)
	ldr	w2, [x1]
	ldr	w3, =(WRESET | APOLLO_WDTRESET | ATLAS_WDTRESET)
	and	w2, w2, w3
	cmp	w2, #0
	b.ne	run_dump_gpr

skip_dump_gpr:
	bl	set_cpu_state

	/* relocate nscode to non-secure region */
	bl	relocate_code

	/* copy bl2 to alive area */
	bl	copy_to_alive

run_dump_gpr:
#if defined(CONFIG_PM)
	bl	pmic_init
#endif

#if defined(CONFIG_SPL_BUILD) && defined(SCRATCH_INFORM_DEBUG)
	ldr	x0, =0x30000
	bl	scratch_inform
#endif

	/* init system clock */
	bl	system_clock_init

#if defined(CONFIG_SPL_BUILD) && defined(SCRATCH_INFORM_DEBUG)
	ldr	x0, =0x40000
	bl	scratch_inform
#endif

	/* Memory initialize */
	bl	mem_ctrl_init

#if defined(CONFIG_SPL_BUILD) && defined(SCRATCH_INFORM_DEBUG)
	ldr	x0, =0x50000
	bl	scratch_inform
#endif

	/* mark that bl2 is executed */
	ldr	x1, =CONFIG_SYS_SDRAM_BASE
	ldr	w0, =0xabcdef
	str	w0, [x1]

#if defined(CONFIG_TZPC)
	bl	tzpc_init
#endif

#if defined(CONFIG_SPL_BUILD) && defined(SCRATCH_INFORM_DEBUG)
	ldr	x0, =0x60000
	bl	scratch_inform
#endif
1:
	pop	x30, x29
	ret
#endif
wakeup_reset:
	/* init asv table */
	bl	_asv_table_init

	bl	start_mct_frc

	bl	read_om

	bl	set_cpu_state

	/* relocate nscode to non-secure region */
	bl	relocate_code

	/* If eMMC booting */
	ldr	x0, =(EXYNOS7420_POWER_BASE + INFORM3_OFFSET)
	ldr	w1, [x0]
	cmp	w1, #BOOT_EMMC_4_4
	bne	1f
	bl	emmc_endbootop
1:
#if defined(CONFIG_SPL_BUILD) && defined(SCRATCH_INFORM_DEBUG)
	ldr	x0, =0x70000
	bl	scratch_inform
#endif

	/* init system clock */
	bl	system_clock_init

#if defined(CONFIG_SPL_BUILD) && defined(SCRATCH_INFORM_DEBUG)
	ldr	x0, =0x80000
	bl	scratch_inform
#endif

	/* Memory initialize */
	bl	mem_ctrl_init

#if defined(CONFIG_SPL_BUILD) && defined(SCRATCH_INFORM_DEBUG)
	ldr	x0, =0x90000
	bl	scratch_inform
#endif

exit_wakeup:
	bl	msh2_fifo_reset

	b       warmboot

pmic_gpio_init:
	/* Disable EINT GPIO(nPower/GPA2_7) Pull-up/down */
	ldr	x0, =(EXYNOS7420_GPIO_ALIVE_BASE + GPA2PUD_OFFSET)
	ldr	w1, [x0]
	bic	w1, w1, #0xC000
	str	w1, [x0]
#ifdef CUSTOM_PMIC_WRSTI
	/* Set PMIC WRSTI(GPA3_7) to Pull-up/down disable */
	ldr	x0, =(EXYNOS7420_GPIO_ALIVE_BASE + GPA3PUD_OFFSET)
	ldr	w1, [x0]
	bic	w1, w1, #0xC000
	str	w1, [x0]

	/* Set PMIC WRSTI(GPA3_7) to Output */
	ldr	x0, =(EXYNOS7420_GPIO_ALIVE_BASE + GPA3CON_OFFSET)
	ldr	w1, [x0]
	bic	w1, w1, #0xF0000000
	orr	w1, w1, #0x10000000
	str	w1, [x0]

	/* Set PMIC WRSTI(GPA3_7) to High */
	ldr	x0, =(EXYNOS7420_GPIO_ALIVE_BASE + GPA3DAT_OFFSET)
	ldr	w1, [x0]
	orr	w1, w1, #0x80
	str	w1, [x0]
	ret	
#endif
#ifdef OLD_AVL_PMIC_WRSTI
	/* Set PMIC WRSTI(GPF0_2) to Pull-up/down disable */
	ldr	x0, =(EXYNOS7420_GPIO_BUS1_BASE + GPF0PUD_OFFSET)
	ldr	w1, [x0]
	bic	w1, w1, #0x30
	str	w1, [x0]

	/* Set PMIC WRSTI(GPF0_2) to Output */
	ldr	x0, =(EXYNOS7420_GPIO_BUS1_BASE + GPF0CON_OFFSET)
	ldr	w1, [x0]
	bic	w1, w1, #0xF00
	orr	w1, w1, #0x100
	str	w1, [x0]

	/* Set PMIC WRSTI(GPF0_2) to High */
	ldr	x0, =(EXYNOS7420_GPIO_BUS1_BASE + GPF0DAT_OFFSET)
	ldr	w1, [x0]
	orr	w1, w1, #0x4
	str	w1, [x0]
	ret
#endif
#ifdef NEW_AVL_PMIC_WRSTI
	/* Set PMIC WRSTI(GPF2_3) to Pull-up/down disable */
	ldr	x0, =(EXYNOS7420_GPIO_BUS1_BASE + GPF2PUD_OFFSET)
	ldr	w1, [x0]
	bic	w1, w1, #0xc0
	str	w1, [x0]

	/* Set PMIC WRSTI(GPF2_3) to Output */
	ldr	x0, =(EXYNOS7420_GPIO_BUS1_BASE + GPF2CON_OFFSET)
	ldr	w1, [x0]
	bic	w1, w1, #0xF000
	orr	w1, w1, #0x1000
	str	w1, [x0]

	/* Set PMIC WRSTI(GPF2_3) to High */
	ldr	x0, =(EXYNOS7420_GPIO_BUS1_BASE + GPF2DAT_OFFSET)
	ldr	w1, [x0]
	orr	w1, w1, #0x8
	str	w1, [x0]
	ret
#endif
read_om:
	/* Read booting information */
	ldr	x0, =(EXYNOS7420_POWER_BASE + OM_STATUS_OFFSET)
	ldr	w1, [x0]
	bic	w2, w1, #0xffffffc1

	/* SD/MMC BOOT */
	cmp     w2, #0x4
	beq	1f

	/* eMMC 4.4 BOOT */
	cmp	w2, #0x0
	beq	2f
	cmp	w2, #0x8
	beq	2f
	cmp	w2, #0x10
	beq	2f
	cmp	w2, #0x18
	beq	2f

	/* UFS BOOT */
	cmp	w2, #0xA
	beq	3f
	cmp	w2, #0x1A
	beq	3f
1:
	mov	w3, #BOOT_MMCSD
	b	10f
2:
	mov	w3, #BOOT_EMMC_4_4
	b	10f
3:
	mov	w3, #BOOT_UFS
10:
	ldr	x0, =(EXYNOS7420_POWER_BASE + INFORM3_OFFSET)
	str	w3, [x0]

	ret

set_cpu_state:
	/* read boot cluster */
	adr	x0, _cpu_state
	mrs     x1, mpidr_el1		/* read MPIDR */
	ubfx	w1, w1, #8, #4		/* w1 = cluster id */
	lsl     x1, x1, #4

	/* set reset flag at _cpu_state of boot cpu */
	ldr	w2, =RESET
	str	w2, [x0, x1]

	ret

check_warm_reset:
	/* check reset status */
	ldr	x0, =(EXYNOS7420_POWER_BASE + RST_STAT_OFFSET)
	ldr	w1, [x0]
	and	w1, w1, #WRESET
	tst	w1, #WRESET		/* check warm reset */
	beq	1f
	/* clear reset_status */
	ldr	x0, =(EXYNOS7420_POWER_BASE + INFORM1_OFFSET)
	mov	w1, #0x0
	str	w1, [x0]
1:
	ret

msh2_fifo_reset:
	/* Clear FIFO */
	ldr	x0, =EXYNOS7420_MMC_BASE
	ldr	w1, [x0]
	orr	w1, w1, #0x2
	str	w1, [x0]

	ret

start_mct_frc:
	ldr	x0, =(EXYNOS7420_MCT_BASE + G_TCON_OFFSET)
	ldr	w1, [x0]
	orr	w1, w1, #GLOBAL_FRC_ENABLE
	str	w1, [x0]

	ret

#if defined(CONFIG_SPL_BUILD) && defined(SCRATCH_INFORM_DEBUG)
scratch_inform:
	ldr	x1, =(EXYNOS7420_POWER_BASE + 0x814)
	ldr	w2, [x1]
	ldr	w3, =0x0000FFFF
	and	w2, w2, w3
	orr	w0, w0, w2
	str	w0, [x1]

	ret
#endif

.globl smc_image_info_base
smc_image_info_base:
	.word CONFIG_SMC_IMAGE_INFORM_BASE

.globl second_boot_info_base
second_boot_info_base:
	.word CONFIG_SECOND_BOOT_INFORM_BASE

/*
 * Relocate code
 */
relocate_code:
	adr	x0, nscode_base			/* r0: source address (start) */
	adr	x1, nscode_end			/* r1: source address (end) */
	ldr	x2, =CONFIG_PHY_IRAM_NS_BASE	/* r2: target address */

1:
	ldp     x3, x4, [x0], #16
	stp     x3, x4, [x2], #16
	cmp	x0, x1
	b.lt	1b

	.word	0xd5033f9f			/* dsb	sy */
	.word	0xd5033fdf			/* isb	sy */

	ret

#ifndef CONFIG_SG_BINARY
/* copy bl2 to alive area */
copy_to_alive:
	ldr	x0, =(CONFIG_SPL_TEXT_BASE+PART_SIZE_BL2_NORMAL)		/* start */
	ldr	x1, =(CONFIG_SPL_TEXT_BASE+PART_SIZE_BL2_NORMAL+PART_SIZE_BL2_SG)	/* end */
	ldr	x2, =CONFIG_PHY_ALIVE_IRAM_BASE			/* target address */

1:
	ldp     x3, x4, [x0], #16
	stp     x3, x4, [x2], #16
	cmp	x0, x1
	b.lt	1b

	.word	0xd5033f9f			/* dsb	sy */
	.word	0xd5033fdf			/* isb	sy */

	ret
#endif

/******************************************************************************/
/*
 * CPU1, 2, 3, 4, 5 waits here until CPU0 wake it up.
 * - below code is copied to CONFIG_PHY_IRAM_NS_BASE, which is non-secure memory.
 */
	.align 3
nscode_base:
	b	1f
	nop				/* for backward compatibility */

	.align 3
	.word	0x0			/* REG0: RESUME_ADDR */
	.word	0x0			/* REG1: RESUME_FLAG */
	.word	0x0			/* REG2 */
	.word	0x0			/* REG3 */
_switch_addr:
	.word	0x0			/* REG4: SWITCH_ADDR */
_hotplug_addr:
	.word	0x0			/* REG5: CPU1_BOOT_REG */
	.word	0x0			/* REG6 */
_c2_addr:
	.word	0x0			/* REG7: REG_C2_ADDR */
_cpu_state:
	.word	HOTPLUG			/* CLUSTER0_CORE0_STATE */
	.word	HOTPLUG			/* CLUSTER0_CORE1_STATE */
	.word	HOTPLUG			/* CLUSTER0_CORE2_STATE */
	.word	HOTPLUG			/* CLUSTER0_CORE3_STATE */
	.word	HOTPLUG			/* CLUSTER1_CORE0_STATE */
	.word	HOTPLUG			/* CLUSTER1_CORE1_STATE */
	.word	HOTPLUG			/* CLUSTER1_CORE2_STATE */
	.word	HOTPLUG			/* CLUSTER1_CORE3_STATE */
	.word	0x0			/* CLUSTER0_STATE */
	.word	0x0			/* CLUSTER1_STATE */
_lpc_state:
	.word	0x0			/* LPC FLAG */
	.word	0x0			/* LPC STATE */

1:
	adr	x0, _cpu_state

	mrs     x7, mpidr_el1
	ubfx    w8, w7, #8, #4          /* w8 = cluster id */
	and     w7, w7, #0xf            /* w7 = cpu id */
	lsl	w8, w8, #2
	add 	w7, w7, w8

	/* read the current cpu state */
	ldr	w10, [x0, x7, lsl #2]

	/* clear cpu state */
	mov	x1, xzr
	str	w1, [x0, x7, lsl #2]

ns_svc_entry:
	nop
	mov	x0, #C2_STATE
	orr	x0, x0, #CLUSTER_OFF
	tst	x10, x0
	b.eq	2f
	adr	x0, _c2_addr
	b	wait_for_addr
2:
	/* clear INFORM1 for security reason  */
	ldr	x0, =(EXYNOS7420_POWER_BASE + INFORM1_OFFSET)
	ldr	w1, [x0]
	cmp	w1, #0x0
	b.eq	3f
	mov	w1, #0x0
	str	w1, [x0]
	ldr	x1, =(EXYNOS7420_POWER_BASE + INFORM0_OFFSET)
	ldr	w2, [x1]
	br	x2
3:
	tst	x10, #RESET
	b.eq	4f
	ldr	x1, =CONFIG_SYS_TEXT_BASE
	br	x1
4:
	adr	x0, _hotplug_addr
wait_for_addr:
	ldr	w1, [x0]
	cmp	w1, #0x0
	b.eq	5f
	br	x1
5:
	b	wait_for_addr
	.ltorg
nscode_end:
