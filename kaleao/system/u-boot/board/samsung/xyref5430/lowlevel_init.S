/*
 * Lowlevel setup for XYREF5430 board based on EXYNOS5430
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
#define GPA2_PUD		0x0048
#define GPF5_CON		0x0280
#define GPF5_DAT		0x0284

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
	ldr	r0, =(EXYNOS5430_POWER_BASE + INFORM1_OFFSET)
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
	ldr	r0, =(EXYNOS5430_POWER_BASE + PSHOLD_CONTROL_OFFSET)
	ldr	r1, =0x5300
	str	r1, [r0]

#ifdef CONFIG_CPU_EXYNOS5433
	/* enable pull-down GPA2_PUD[0:6] for avoiding board reset */
	ldr	r0, =0x10580048
	ldr	r1, =0x00001555
	str	r1, [r0]
#endif

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

	/*
	 * Enable NOC-BUS Timeout function for debugging
	 * R2 register shows the cold booting or wakeup
	 */
	mov	r2, #0
	bl	enable_noc_timeout

#ifndef CONFIG_CPU_EXYNOS5433
	bl get_dram_size
	ldr r1, =CONFIG_SYS_SDRAM_BASE
	str r0, [r1, #4]
#endif
	/* mark that bl2 is executed */
	ldr r1, =CONFIG_SYS_SDRAM_BASE
	ldr r0, =0xabcdef
	str r0, [r1]

#if defined(CONFIG_PM)
	bl	pmic_late_init
#endif

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
	ldr	r0, =(EXYNOS5430_POWER_BASE + INFORM3_OFFSET)
	ldr	r1, [r0]
	cmp	r1, #BOOT_EMMC_4_4
	bleq	emmc_endbootop

	/* init system clock */
	bl	system_clock_init

	/* Memory initialize */
	bl	mem_ctrl_init

	/*
	 * Enable NOC-BUS Timeout function for debugging
	 * R2 register shows the cold booting or wakeup
	 */
	mov	r2, #1
	bl	enable_noc_timeout

exit_wakeup:
	/* W/A for abnormal MMC interrupt */
	ldr	r0, =0x1554009c
	ldr	r1, [r0]
	orr	r1, r1, #(1 << 11)
	str	r1, [r0]

	ldr	r0, =0x1555009c
	ldr	r1, [r0]
	orr	r1, r1, #(1 << 11)
	str	r1, [r0]

	ldr	r0, =0x1556009c
	ldr	r1, [r0]
	orr	r1, r1, #(1 << 11)
	str	r1, [r0]

	b       warmboot

pmic_gpio_init:
	/* nPOWER(EINT23) pull-up-down disable */
	ldr	r0, =(EXYNOS5430_GPIO_ALIVE_BASE + GPA2_PUD)
	ldr	r1, [r0]
	bic	r1, r1, #0x4000
	str	r1, [r0]

	/* Set PMIC WRSTBI(GPF5_4) to Output */
#ifdef CONFIG_CPU_EXYNOS5433_EVT1
	ldr	r0, =0x110900a0 /* GPF5_CON */
#else
	ldr	r0, =(EXYNOS5430_GPIO_PERIC_BASE + GPF5_CON)
#endif
	ldr	r1, [r0]
	bic	r2, r1, #0xF0000
	and	r1, r1, r2
	orr	r1, r1, #0x10000
	str	r1, [r0]

	/* Set PMIC WRSTBI(GPF5_4) to High */
#ifdef CONFIG_CPU_EXYNOS5433_EVT1
	ldr	r0, =0x110900a4 /* GPF5_DAT */
#else
	ldr	r0, =(EXYNOS5430_GPIO_PERIC_BASE + GPF5_DAT)
#endif
	ldr	r1, [r0]
	orr	r1, r1, #0x10
	str	r1, [r0]

	mov	pc, lr

read_om:
	/* Read booting information */
	ldr	r0, =(EXYNOS5430_POWER_BASE + OM_STATUS_OFFSET)
	ldr	r1, [r0]
	bic	r2, r1, #0xffffffc1

	/* SD/MMC BOOT */
	cmp     r2, #0x4
	moveq   r3, #BOOT_MMCSD

	/* eMMC BOOT */
	cmp	r2, #0x0
	moveq	r3, #BOOT_EMMC_4_4
	cmp	r2, #0x8
	moveq	r3, #BOOT_EMMC_4_4
	cmp	r2, #0x10
	moveq	r3, #BOOT_EMMC_4_4
	cmp	r2, #0x18
	moveq	r3, #BOOT_EMMC_4_4

	/* UFS BOOT */
	cmp	r2, #0xA
	moveq	r3, #BOOT_UFS
	cmp	r2, #0x1A
	moveq	r3, #BOOT_UFS

	ldr	r0, =(EXYNOS5430_POWER_BASE + INFORM3_OFFSET)
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
	ldr	r0, =(EXYNOS5430_POWER_BASE + RST_STAT_OFFSET)
	ldr	r1, [r0]
	and	r1, r1, #WRESET
	cmp	r1, #WRESET		@ check warm reset
	/* clear reset_status */
	ldreq	r0, =(EXYNOS5430_POWER_BASE + INFORM1_OFFSET)
	moveq	r1, #0x0
	streq	r1, [r0]

	mov	pc, lr

.globl smc_image_info_base
smc_image_info_base:
	.word CONFIG_SMC_IMAGE_INFORM_BASE

.globl second_boot_info_base
second_boot_info_base:
	.word CONFIG_SECOND_BOOT_INFORM_BASE

start_mct_frc:
	ldr	r0, =(EXYNOS5430_MCT_BASE + G_TCON_OFFSET)
	ldr	r1, [r0]
	orr	r1, r1, #GLOBAL_FRC_ENABLE
	str	r1, [r0]

	mov	pc, lr

enable_noc_timeout:
#ifdef CONFIG_CPU_EXYNOS5433_EVT1
	/* FSYS NOC-BUS timeout is removed : 0x156f0700, 0x3 */
	mov	r1, #0x1
	ldr	r0, =0x14c60700
	str	r1, [r0]
	ldr	r0, =0x10050700
	str	r1, [r0]
	ldr	r0, =0x105e0700
	str	r1, [r0]
	ldr	r0, =0x10fd0700
	str	r1, [r0]
	ldr	r0, =0x13430700
	str	r1, [r0]
	ldr	r0, =0x14830700
	str	r1, [r0]
	ldr	r0, =0x13630700
	str	r1, [r0]
	ldr	r0, =0x11810700
	str	r1, [r0]
	ldr	r0, =0x11910700
	str	r1, [r0]

	/* Only run at cold booting */
	cmp	r2, #0
	ldreq	r0, =0x150e0700 /* MSCL */
	streq	r1, [r0]
	ldreq	r0, =0x152a0700 /* MFC */
	streq	r1, [r0]
	ldreq	r0, =0x14fa0700 /* HEVC */
	streq	r1, [r0]
	ldreq	r0, =0x13cb0700 /* GSCL */
	streq	r1, [r0]
	ldreq	r0, =0x14a90700 /* G3D */
	streq	r1, [r0]
	ldreq	r0, =0x124c0700 /* G2D */
	streq	r1, [r0]
	ldreq	r0, =0x13b80700 /* DISP */
	streq	r1, [r0]
	ldreq	r0, =0x145f0700 /* CAM1 */
	streq	r1, [r0]
	ldreq	r0, =0x120f0700 /* CAM0 */
	streq	r1, [r0]
	ldreq	r0, =0x114f0700 /* AUD */
	streq	r1, [r0]
#endif
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
	.word	HOTPLUG			@ CLUSTER0_CORE2_STATE
	.word	HOTPLUG			@ CLUSTER0_CORE3_STATE
	.word	HOTPLUG			@ CLUSTER1_CORE0_STATE
	.word	HOTPLUG			@ CLUSTER1_CORE1_STATE
	.word	HOTPLUG			@ CLUSTER1_CORE2_STATE
	.word	HOTPLUG			@ CLUSTER1_CORE3_STATE
	.word	0x0			@ CLUSTER0_STATE
	.word	0x0			@ CLUSTER1_STATE

1:
	adr	r0, _cpu_state

	mrc	p15, 0, r7, c0, c0, 5	@ read MPIDR
	ubfx	r8, r7, #8, #4		@ r8 = cluster id
	and	r7, r7, #0xf		@ r7 = cpu id
	add	r7, r7, r8, lsl #2

	/* read the current cpu state */
	ldr	r10, [r0, r7, lsl #2]
#ifndef CONFIG_CPU_EXYNOS5433_EVT1
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
	add	r4, pc, #12
	.word	0xe12ef304	/* msr    elr_hyp, r4 */
	.word	0xF57FF04F	/* dsb sy */
	.word	0xF57FF06F	/* isb sy */
	.word	0xe160006e	/* ERET */
#endif
ns_svc_entry:
	nop
	tst	r10, #SWITCH
	adrne	r0, _switch_addr
	bne	wait_for_addr
	tst	r10, #C2_STATE
	adrne	r0, _c2_addr
	bne	wait_for_addr

	/* clear INFORM1 for security reason  */
	ldr	r0, =(EXYNOS5430_POWER_BASE + INFORM1_OFFSET)
	ldr	r1, [r0]
	cmp	r1, #0x0
	movne	r1, #0x0
	strne	r1, [r0]
	ldrne	r1, =(EXYNOS5430_POWER_BASE + INFORM0_OFFSET)
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
