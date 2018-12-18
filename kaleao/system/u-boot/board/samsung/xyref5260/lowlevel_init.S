/*
 * Lowlevel setup for XYREF5260 board based on EXYNOS5260
 *
 * Copyright (C) 2013 Samsung Electronics
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

/* Multi Core Timer */
#define G_TCON_OFFSET		0x0240
#define GLOBAL_FRC_ENABLE	0x100

#define PSHOLD_CONTROL_OFFSET	0x330C
#define GPA2CON_OFFSET		0x0040
#define GPA2DAT_OFFSET		0x0044
#define GPF1CON_OFFSET		0x01E0
#define GPF1DAT_OFFSET		0x01E4
#define GPX2PUD_OFFSET		0x0C48

/* cpu_state flag */
#define RESET			(1 << 0)
#define RESERVED		(1 << 1)
#define HOTPLUG			(1 << 2)
#define C2_STATE		(1 << 3)
#define SWITCH			(1 << 4)

_TEXT_BASE:
	.word	CONFIG_SYS_TEXT_BASE

	.globl lowlevel_init
lowlevel_init:
	/* use iRAM stack in bl2 */
	ldr	sp, =CONFIG_IRAM_STACK
	stmdb	r13!, {ip,lr}

	/* check warm reset status */
	bl	check_warm_reset

	/* check reset status */
	ldr	r0, =(EXYNOS5260_POWER_BASE + INFORM1_OFFSET)
	ldr	r1, [r0]

	/* AFTR wakeup reset */
	ldr	r2, =S5P_CHECK_DIDLE
	cmp	r1, r2
	beq	exit_wakeup

	/* LPA wakeup reset */
	ldr	r2, =S5P_CHECK_LPA
	cmp	r1, r2
	beq	exit_wakeup

	/* Sleep wakeup reset */
	ldr	r2, =S5P_CHECK_SLEEP
	cmp	r1, r2
	beq	wakeup_reset

	/* PS-Hold high */
	ldr	r0, =(EXYNOS5260_POWER_BASE + PSHOLD_CONTROL_OFFSET)
	ldr	r1, =0x5300
	str	r1, [r0]

	bl	pmic_gpio_init

#if defined(CONFIG_PM)
	bl	pmic_enable_peric_dev
#endif

	bl	read_om

	/*
	 * If U-boot is already running in RAM, no need to relocate U-Boot.
	 * Memory controller must be configured before relocating U-Boot
	 * in ram.
	 */
	ldr	r0, =0x0ffffff		/* r0 <- Mask Bits*/
	bic	r1, pc, r0		/* pc <- current addr of code */
					/* r1 <- unmasked bits of pc */
	ldr	r2, _TEXT_BASE		/* r2 <- original base addr in ram */
	bic	r2, r2, r0		/* r2 <- unmasked bits of r2*/
	cmp	r1, r2			/* compare r1, r2 */
	beq	1f			/* r0 == r1 then skip sdram init */

	bl	set_cpu_state

	/* relocate nscode to non-secure region */
	bl	relocate_code

#if defined(CONFIG_PM)
	bl	pmic_init
#endif

	/* init system clock */
	bl	system_clock_init

	/* Memory initialize */
	bl	mem_ctrl_init

#if defined(CONFIG_TZPC)
	bl	tzpc_init
#endif

1:
	ldmia	r13!, {ip,pc}

wakeup_reset:
	bl	start_mct_frc

	bl	read_om

	bl	set_cpu_state

	/* relocate nscode to non-secure region */
	bl	relocate_code

	/* If eMMC booting */
	ldr	r0, =(EXYNOS5260_POWER_BASE + INFORM3_OFFSET)
	ldr	r1, [r0]
	cmp	r1, #BOOT_EMMC_4_4
	bleq	emmc_endbootop

	/* init system clock */
	bl	system_clock_init

	/* Memory initialize */
	bl	mem_ctrl_init

exit_wakeup:
	/* W/A for abnormal MMC interrupt */
	ldr	r0, =0x1214009c
	ldr	r1, [r0]
	orr	r1, r1, #(1 << 11)
	str	r1, [r0]

	ldr	r0, =0x1215009c
	ldr	r1, [r0]
	orr	r1, r1, #(1 << 11)
	str	r1, [r0]

	ldr	r0, =0x1216009c
	ldr	r1, [r0]
	orr	r1, r1, #(1 << 11)
	str	r1, [r0]

	b       warmboot

pmic_gpio_init:
	/* Disable EINT GPIO Pull-up/down */
	ldr	r0, =(EXYNOS5260_GPIO_PART1_BASE + GPX2PUD_OFFSET)
	mov	r1, #0x0
	str	r1, [r0]

	/* Set PMIC WRSTBI(GPA2_0) to Output */
	ldr	r0, =(EXYNOS5260_GPIO_PART1_BASE + GPA2CON_OFFSET)
	ldr	r1, [r0]
	bic	r2, r1, #0xF
	and	r1, r1, r2
	orr	r1, r1, #0x1
	str	r1, [r0]

	/* Set PMIC WRSTBI(GPA2_0) to High */
	ldr	r0, =(EXYNOS5260_GPIO_PART1_BASE + GPA2DAT_OFFSET)
	ldr	r1, [r0]
	orr	r1, r1, #0x1
	str	r1, [r0]
	mov	pc, lr

read_om:
	/* Read booting information */
	ldr	r0, =(EXYNOS5260_POWER_BASE + OM_STATUS_OFFSET)
	ldr	r1, [r0]
	bic	r2, r1, #0xffffffc1

	/* SD/MMC BOOT */
	cmp     r2, #0x4
	moveq   r3, #BOOT_MMCSD

	/* eMMC BOOT */
	cmp	r2, #0x6
	moveq	r3, #BOOT_EMMC

	/* eMMC 4.4 BOOT */
	cmp	r2, #0x8
	moveq	r3, #BOOT_EMMC_4_4
	cmp	r2, #0x10
	moveq	r3, #BOOT_EMMC_4_4
	cmp	r2, #0x28
	moveq	r3, #BOOT_EMMC_4_4

	ldr	r0, =(EXYNOS5260_POWER_BASE + INFORM3_OFFSET)
	str	r3, [r0]

	mov	pc, lr

set_cpu_state:
	/* read boot cluster */
	adr	r0, _cpu_state
	mrc	p15, 0, r1, c0, c0, 5	@ read MPIDR
	ubfx	r1, r1, #8, #4		@ r1 = cluster id

	/* set reset flag at _cpu_state of boot cpu */
	ldr	r2, =RESET
	str	r2, [r0, r1, lsl #4]

	mov	pc, lr

check_warm_reset:
	/* check reset status */
	ldr	r0, =(EXYNOS5260_POWER_BASE + RST_STAT_OFFSET)
	ldr	r1, [r0]
	and	r1, r1, #WRESET
	cmp	r1, #WRESET		@ check warm reset
	/* clear reset_status */
	ldreq	r0, =(EXYNOS5260_POWER_BASE + INFORM1_OFFSET)
	moveq	r1, #0x0
	streq	r1, [r0]

	mov	pc, lr

start_mct_frc:
	ldr	r0, =(EXYNOS5260_MCT_BASE + G_TCON_OFFSET)
	ldr	r1, [r0]
	orr	r1, r1, #GLOBAL_FRC_ENABLE
	str	r1, [r0]

	mov	pc, lr


/*
 * Relocate code
 */
relocate_code:
	adr	r0, nscode_base			@ r0: source address (start)
	adr	r1, nscode_end			@ r1: source address (end)
	ldr	r2, =CONFIG_PHY_IRAM_NS_BASE	@ r2: target address

1:
	ldmia	r0!, {r3-r6}
	stmia	r2!, {r3-r6}
	cmp	r0, r1
	blt	1b

	.word	0xF57FF04F			@dsb	sy
	.word	0xF57FF06F			@isb	sy

	mov	pc, lr

/******************************************************************************/
/*
 * CPU1, 2, 3, 4, 5 waits here until CPU0 wake it up.
 * - below code is copied to CONFIG_PHY_IRAM_NS_BASE, which is non-secure memory.
 */
nscode_base:
	b	1f
	nop				@ for backward compatibility

	.word	0x0			@ REG0: RESUME_ADDR
	.word	0x0			@ REG1: RESUME_FLAG
	.word	0x0			@ REG2
	.word	0x0			@ REG3
_switch_addr:
	.word	0x0			@ REG4: SWITCH_ADDR
_hotplug_addr:
	.word	0x0			@ REG5: CPU1_BOOT_REG
	.word	0x0			@ REG6
_c2_addr:
	.word	0x0			@ REG7: REG_C2_ADDR
_cpu_state:
	.word	HOTPLUG			@ CLUSTER0_CORE0_STATE
	.word	HOTPLUG			@ CLUSTER0_CORE1_STATE
	.word	RESERVED		@ CLUSTER0_CORE2_STATE
	.word	RESERVED		@ CLUSTER0_CORE3_STATE
	.word	HOTPLUG			@ CLUSTER1_CORE0_STATE
	.word	HOTPLUG			@ CLUSTER1_CORE1_STATE
	.word	HOTPLUG			@ CLUSTER1_CORE2_STATE
	.word	HOTPLUG			@ CLUSTER1_CORE3_STATE

_gic_state:
	.word	0x0			@ CPU0 - GICD_IGROUPR0
	.word	0x0			@ CPU1 - GICD_IGROUPR0
	.word	0x0			@ CPU2 - GICD_IGROUPR0
	.word	0x0			@ CPU3 - GICD_IGROUPR0
	.word	0x0			@ CPU4 - GICD_IGROUPR0
	.word	0x0			@ CPU5 - GICD_IGROUPR0
	.word	0x0			@ RESERVED
	.word	0x0			@ RESERVED

#define RESET			(1 << 0)
#define SECONDARY_RESET		(1 << 1)
#define HOTPLUG			(1 << 2)
#define C2_STATE		(1 << 3)
#define SWITCH			(1 << 4)

1:
	adr	r0, _cpu_state

	mrc	p15, 0, r7, c0, c0, 5	@ read MPIDR
	ubfx	r8, r7, #8, #4		@ r8 = cluster id
	ubfx	r7, r7, #0, #4		@ r7 = cpu id
	add	r7, r7, r8, lsl #2	@ cpuid + (clusterid * 4)

	/* read the current cpu state */
	ldr	r10, [r0, r7, lsl #2]

	/* HYP entry */

	/*
	 * Set the HYP spsr to itself, so that the entry point
	 * does not see the difference between a function call
	 * and an exception return.
	 */
	mrs	r4, cpsr
	msr     spsr_cxsf, r4

	bic	r6, r4, #0x1f
	orr	r6, r6, #0x13
	msr	spsr_cxsf, r6	/* Setup SPSR to jump to NS SVC mode */
	add	r4, pc, #4
	.word	0xe12ef304	/* msr    elr_hyp, r4 */
	.word	0xe160006e	/* ERET */
ns_svc_entry:
	tst	r10, #SWITCH
	adrne	r0, _switch_addr
	bne	wait_for_addr
	tst	r10, #C2_STATE
	adrne	r0, _c2_addr
	bne	wait_for_addr

	/* clear INFORM1 for security reason  */
	ldr	r0, =(EXYNOS5260_POWER_BASE + INFORM1_OFFSET)
	ldr	r1, [r0]
	cmp	r1, #0x0
	movne	r1, #0x0
	strne	r1, [r0]
	ldrne	r1, =(EXYNOS5260_POWER_BASE + INFORM0_OFFSET)
	ldrne	pc, [r1]

	tst	r10, #RESET
	ldrne	pc, =CONFIG_SYS_TEXT_BASE

	adr	r0, _hotplug_addr
wait_for_addr:
	ldr	r1, [r0]
	cmp	r1, #0x0
	bxne	r1
	wfe
	b	wait_for_addr
	.ltorg
nscode_end:
