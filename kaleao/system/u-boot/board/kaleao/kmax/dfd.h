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

#ifndef _EXYNOS7420_DFD_H
#define _EXYNOS7420_DFD_H

#define DFD_NR_CPUS			(8)
#define DFD_EDPCSR_DUMP_EN		(1 << 0)
#define DFD_CPU_ON_PSCI_ID		(0xC4000003)
#define DFD_ATLAS_DBG_CONFIG_OFFSET	(0x2440)
#define DFD_ATLAS_DBG_INITIATE_WAKEUP	(3 << 16)
#define DFD_RESET_SEQUENCER_OFFSET	(0x500)
#define DFD_COREREG_OFFSET		(0x200)
#define DFD_REG_OFFSET			(0x4)

#define DFD_ATLAS_DUMP_PC_ADDRESS	(0x11840000)
#define DFD_APOLLO_DUMP_PC_ADDRESS	(0x11930000)

#define DFD_ATLAS_LAST_CORE		(7)
#define DFD_APOLLO_LAST_CORE		(3)

#define CLEAR				(0 << 0)
#define RESERVED			(1 << 0)
#define HOTPLUG				(1 << 2)
#define C2_STATE			(1 << 3)
#define CLUSTER_OFF			(1 << 4)
#define DFD_CORE_STAT			(CONFIG_PHY_IRAM_NS_BASE + 0x28)

#define CPU0_LOGICAL_MAP		0x100
#define CPU1_LOGICAL_MAP		0x101
#define CPU2_LOGICAL_MAP		0x102
#define CPU3_LOGICAL_MAP		0x103
#define CPU4_LOGICAL_MAP		0x0
#define CPU5_LOGICAL_MAP		0x1
#define CPU6_LOGICAL_MAP		0x2
#define CPU7_LOGICAL_MAP		0x3

#define FLUSH_SKIP			0x0
#define FLUSH_LEVEL1			0x1
#define FLUSH_LEVEL2			0x2
#define FLUSH_LEVEL3			0x3

#ifndef __ASSEMBLY__
#ifdef CONFIG_RAMDUMP_GPR
void dfd_run_dump_gpr(unsigned long pmu_base);
void dfd_set_dump_grp(unsigned long pmu_base, int en);
void dfd_entry_dump_gpr(void);
void dfd_display_reboot_reason(void);
void dfd_display_core_stat(void);
#else
inline static void dfd_display_reboot_reason(void)
{
}
inline static void dfd_display_core_stat(void)
{
}
inline static void dfd_run_dump_gpr(unsigned long pmu_base)
{
}
inline static void dfd_set_dump_gpr(unsigned long pmu_base, int en)
{
}
inline static void dfd_entry_dump_gpr(void)
{
}
#endif
#endif
#endif
