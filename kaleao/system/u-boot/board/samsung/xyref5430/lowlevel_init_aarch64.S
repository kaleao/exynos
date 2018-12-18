/*
 * Lowlevel setup for EXYNOS5433
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

	/* check warm reset status */
	bl	check_warm_reset

	/* check reset status */
	ldr	x0, =(EXYNOS5430_POWER_BASE + INFORM1_OFFSET)
	ldr	w1, [x0]

	/* AFTR wakeup reset */
	ldr	w2, =S5P_CHECK_DIDLE
	cmp	w1, w2
	beq	exit_wakeup

	/* LPA wakeup reset */
	ldr	w2, =S5P_CHECK_LPA
	cmp	w1, w2
	beq	exit_wakeup

	/* Sleep wakeup reset */
	ldr	w2, =S5P_CHECK_SLEEP
	cmp	w1, w2
	beq	wakeup_reset

	/* PS-Hold high */
	ldr	x0, =(EXYNOS5430_POWER_BASE + PSHOLD_CONTROL_OFFSET)
	ldr	w1, =0x5300
	str	w1, [x0]

#ifdef CONFIG_CPU_EXYNOS5433
	/* enable pull-down GPA2_PUD[0:6] for avoiding board reset */
	ldr	x0, =0x10580048
	ldr	w1, =0x00001555
	str	w1, [x0]
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
	ldr	w0, =0xffffff		/* r0 <- Mask Bits*/
	ldr	w3, =lowlevel_init
	bic	w1, w3, #0x0ffffff	/* pc <- current addr of code */
					/* r1 <- unmasked bits of pc */
	ldr	w2, _TEXT_BASE		/* r2 <- original base addr in ram */
	bic	w2, w2, #0x0ffffff	/* r2 <- unmasked bits of r2*/
	cmp	w1, w2			/* compare r1, r2 */
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
	mov	w2, #0
	bl	enable_noc_timeout

#ifndef CONFIG_CPU_EXYNOS5433
	bl get_dram_size
	ldr x1, =CONFIG_SYS_SDRAM_BASE
	str w0, [x1, #4]
#endif
	/* mark that bl2 is executed */
	ldr x1, =CONFIG_SYS_SDRAM_BASE
	ldr w0, =0xabcdef
	str w0, [x1]

#if defined(CONFIG_PM)
	bl	pmic_late_init
#endif

#if defined(CONFIG_TZPC)
	bl	tzpc_init
#endif

1:
	pop	x30, x29
	ret

wakeup_reset:
	bl	start_mct_frc

	bl	read_om

	bl	set_cpu_state

	/* relocate nscode to non-secure region */
	bl	relocate_code

	/* If eMMC booting */
	ldr	x0, =(EXYNOS5430_POWER_BASE + INFORM3_OFFSET)
	ldr	w1, [x0]
	cmp	w1, #BOOT_EMMC_4_4
	bne	1f
	bl	emmc_endbootop
1:
	/* init system clock */
	bl	system_clock_init

	/* Memory initialize */
	bl	mem_ctrl_init

	/*
	 * Enable NOC-BUS Timeout function for debugging
	 * R2 register shows the cold booting or wakeup
	 */
	mov	w2, #1
	bl	enable_noc_timeout

exit_wakeup:
	/* W/A for abnormal MMC interrupt */
	ldr	x0, =0x1554009c
	ldr	w1, [x0]
	orr	w1, w1, #(1 << 11)
	str	w1, [x0]

	ldr	x0, =0x1555009c
	ldr	w1, [x0]
	orr	w1, w1, #(1 << 11)
	str	w1, [x0]

	ldr	x0, =0x1556009c
	ldr	w1, [x0]
	orr	w1, w1, #(1 << 11)
	str	w1, [x0]

	b       warmboot

pmic_gpio_init:
	/* nPOWER(EINT23) pull-up-down disable */
	ldr	x0, =(EXYNOS5430_GPIO_ALIVE_BASE + GPA2_PUD)
	ldr	w1, [x0]
	bic	w2, w1, #0x4000
	str	w2, [x0]

	/* Set PMIC WRSTBI(GPF5_4) to Output */
	ldr	x0, =(EXYNOS5430_GPIO_PERIC_BASE + GPF5_CON)
	ldr	w1, [x0]
	bic	w2, w1, #0xF0000
	and	w1, w1, w2
	orr	w1, w1, #0x10000
	str	w1, [x0]

	/* Set PMIC WRSTBI(GPF5_4) to High */
	ldr	x0, =(EXYNOS5430_GPIO_PERIC_BASE + GPF5_DAT)
	ldr	w1, [x0]
	orr	w1, w1, #0x10
	str	w1, [x0]

	ret

read_om:
	/* Read booting information */
	ldr	x0, =(EXYNOS5430_POWER_BASE + OM_STATUS_OFFSET)
	ldr	w1, [x0]
	bic	w2, w1, #0xffffffc1

	/* SD/MMC BOOT */
	cmp	w2, #0x4
	beq	1f

	/* eMMC BOOT */
	cmp	w2, #0x0
	beq	2f
	cmp	w2, #0x8
	beq	2f
	cmp	w2, #0x10
	beq	2f
	cmp	w2, #0x18
	beq	2f
1:
	mov	w3, #BOOT_MMCSD
	b	10f
2:
	mov	w3, #BOOT_EMMC_4_4
10:
	ldr	x0, =(EXYNOS5430_POWER_BASE + INFORM3_OFFSET)
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
	ldr	x0, =(EXYNOS5430_POWER_BASE + RST_STAT_OFFSET)
	ldr	w1, [x0]
	and	w1, w1, #WRESET
	tst	w1, #WRESET		/* check warm reset */
	beq	1f
	/* clear reset_status */
	ldr	x0, =(EXYNOS5430_POWER_BASE + INFORM1_OFFSET)
	mov	w1, #0x0
	str	w1, [x0]
1:
	ret

.globl smc_image_info_base
smc_image_info_base:
	.word CONFIG_SMC_IMAGE_INFORM_BASE

.globl second_boot_info_base
second_boot_info_base:
	.word CONFIG_SECOND_BOOT_INFORM_BASE

start_mct_frc:
	ldr	x0, =(EXYNOS5430_MCT_BASE + G_TCON_OFFSET)
	ldr	w1, [x0]
	orr	w1, w1, #GLOBAL_FRC_ENABLE
	str	w1, [x0]

	ret

enable_noc_timeout:
#ifdef CONFIG_CPU_EXYNOS5433_EVT1
	/* FSYS NOC-BUS timeout is removed : 0x156f0700, 0x3 */
	mov	w1, #0x1
	ldr	x0, =0x14c60700
	str	w1, [x0]
	ldr	x0, =0x10050700
	str	w1, [x0]
	ldr	x0, =0x105e0700
	str	w1, [x0]
	ldr	x0, =0x10fd0700
	str	w1, [x0]
	ldr	x0, =0x13430700
	str	w1, [x0]
	ldr	x0, =0x14830700
	str	w1, [x0]
	ldr	x0, =0x13630700
	str	w1, [x0]
	ldr	x0, =0x11810700
	str	w1, [x0]
	ldr	x0, =0x11910700
	str	w1, [x0]

	/* Only run at cold booting */
	cmp	w2, #0
	bne	1f
	ldr	x0, =0x150e0700 /* MSCL */
	str	w1, [x0]
	ldr	x0, =0x152a0700 /* MFC */
	str	w1, [x0]
	ldr	x0, =0x14fa0700 /* HEVC */
	str	w1, [x0]
	ldr	x0, =0x13cb0700 /* GSCL */
	str	w1, [x0]
	ldr	x0, =0x14a90700 /* G3D */
	str	w1, [x0]
	ldr	x0, =0x124c0700 /* G2D */
	str	w1, [x0]
	ldr	x0, =0x13b80700 /* DISP */
	str	w1, [x0]
	ldr	x0, =0x145f0700 /* CAM1 */
	str	w1, [x0]
	ldr	x0, =0x120f0700 /* CAM0 */
	str	w1, [x0]
	ldr	x0, =0x114f0700 /* AUD */
	str	w1, [x0]
1:
#endif
	ret

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
	.word	0x0				/* CLUSTER0_STATE */
	.word	0x0				/* CLUSTER1_STATE */
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
	ldr	x0, =(EXYNOS5430_POWER_BASE + INFORM1_OFFSET)
	ldr	w1, [x0]
	cmp	w1, #0x0
	b.eq	3f
	mov	w1, #0x0
	str	w1, [x0]
	ldr	x1, =(EXYNOS5430_POWER_BASE + INFORM0_OFFSET)
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
	wfe
	b	wait_for_addr
	.ltorg
nscode_end:
