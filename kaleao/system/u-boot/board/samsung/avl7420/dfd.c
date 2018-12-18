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

#include <asm/io.h>
#include <asm/arch/cpu.h>
#include <asm/arch/power.h>
#include <asm/arch/gpio.h>
#include <asm/arch/smc.h>
#include "pmic.h"
#include "dfd.h"

void dfd_display_reboot_reason(void)
{
	u32 ret;
	ret = readl(CONFIG_RAMDUMP_REASON);
	printf("reboot reason: ");

	switch (ret) {
		case RAMDUMP_SIGN_PANIC:
			printf("0x%x - Kernel PANIC\n", ret);
			break;
		case RAMDUMP_SIGN_NORMAL_REBOOT:
			printf("0x%x - User Reboot(S/W Reboot)\n", ret);
			break;
		case RAMDUMP_SIGN_FORCE_REBOOT:
			printf("0x%x - Forced Reboot(S/W Reboot)\n", ret);
			break;
		case RAMDUMP_SIGN_RESET:
		default:
			printf("0x%x - Power/Emergency Reset\n", ret);
			break;
	}
}

void dfd_display_core_stat(void)
{
	int val, right_val;
	u32 ret, ret2;
	printf("Core stat at previous(IRAM)\n" );
	for (val = 0; val < DFD_NR_CPUS; val++) {
		ret = readl(DFD_CORE_STAT + (val * DFD_REG_OFFSET));
		if (val < 4)
			right_val = val + 4;
		else
			right_val = val - 4;

		printf("Core%d: ", right_val);
		switch(ret) {
			case CLEAR:
				printf("Running\n");
				break;
			case RESERVED:
				printf("Reserved\n");
				break;
			case HOTPLUG:
				printf("Hotplug\n");
				break;
			case C2_STATE:
				printf("C2\n");
				break;
			case CLUSTER_OFF:
				printf("CLUSTER_OFF\n");
				break;
			default:
				printf("Unknown: 0x%x\n", ret);
				break;
		}
	}
	printf("Core stat at previous(KERNEL)\n" );
	for (val = 0; val < DFD_NR_CPUS; val++) {
		ret = readl(CONFIG_RAMDUMP_CORE_POWER_STAT + (val * DFD_REG_OFFSET));
		printf("Core%d: ", val);
		switch(ret) {
			case RAMDUMP_SIGN_ALIVE:
				printf("Alive");
				break;
			case RAMDUMP_SIGN_DEAD:
				printf("Dead");
				break;
			case RAMDUMP_SIGN_RESET:
			default:
				printf("Power/Emergency Reset: 0x%x", ret);
				break;
		}

		ret2 = readl(CONFIG_RAMDUMP_CORE_PANIC_STAT + (val * DFD_REG_OFFSET));
		switch(ret2) {
			case RAMDUMP_SIGN_PANIC:
				printf("/PANIC\n");
				break;
			case RAMDUMP_SIGN_RESET:
			default:
				printf("\n");
				break;
		}
	}
}

void dfd_set_dump_gpr(unsigned long pmu_base, int en)
{
	writel(en & DFD_EDPCSR_DUMP_EN, pmu_base + DFD_RESET_SEQUENCER_OFFSET);
}

static int dfd_wait_complete(unsigned int core)
{
	u32 ret;
	u32 loop = 1000;

	do {
		ret = readl(CONFIG_RAMDUMP_DUMP_GPR_WAIT);
		if (ret & core)
			return 0;
		udelay(1000);
	} while(loop-- > 0);

	printf("Failed to wait complete - ret:%x core:%x\n", ret, core);
	return -1;
}

void dfd_determine_cache_flush(void)
{
	u32 val, stat, ret;
	int little_on = -1, big_on = -1;

	/* copy IRAM core stat to DRAM */
	for (val = 0; val < DFD_NR_CPUS; val++) {
		ret = readl(DFD_CORE_STAT + (val * DFD_REG_OFFSET));
		if (val < 4) {
			/* 0 ~ 3 are Big core in only DFD_CORE_STAT */
			if (ret & (HOTPLUG | C2_STATE | CLUSTER_OFF)) {
				stat = (FLUSH_SKIP << 16) | ret;
			} else {
				stat = (FLUSH_LEVEL1 << 16) | ret;
				big_on = (val + 4);
			}
			writel(stat, CONFIG_RAMDUMP_GPR_POWER_STAT + ((val + 4) * DFD_REG_OFFSET));
		} else {
			/* 4 ~ 7 are Little core in only DFD_CORE_STAT */
			if (ret & (HOTPLUG | C2_STATE | CLUSTER_OFF)) {
				stat = (FLUSH_SKIP << 16) | ret;
			} else {
				stat = (FLUSH_LEVEL1 << 16) | ret;
				little_on = (val - 4);
			}
			writel(stat, CONFIG_RAMDUMP_GPR_POWER_STAT + ((val - 4) * DFD_REG_OFFSET));
		}

		/* clear IRAM core stat like cold-booting */
		if (val == 4)
			ret = RESERVED;
		else
			ret = HOTPLUG;
		writel(ret, DFD_CORE_STAT + (val * DFD_REG_OFFSET));
	}

	/* conclude core which runs cache flush level 2 in little cluster */
	if (little_on < 0) {
		/* core0 runs cache flush level 2 */
		stat = (FLUSH_LEVEL2 << 16);
		writel(stat, CONFIG_RAMDUMP_GPR_POWER_STAT);
		printf("[Little] any core is not running, core0 runs Cache Flush 2\n");
	} else {
		stat = readl(CONFIG_RAMDUMP_GPR_POWER_STAT + (little_on * DFD_REG_OFFSET));
		stat = (FLUSH_LEVEL3 << 16) | (stat & 0xFFFF);
		writel(stat, CONFIG_RAMDUMP_GPR_POWER_STAT + (little_on * DFD_REG_OFFSET));
		printf("[Little] core%d is running, it runs Cache Flush 3\n", little_on);
	}

	/* conclude core which runs cache flush level 2 in big cluster */
	if (big_on > 0) {
		stat = readl(CONFIG_RAMDUMP_GPR_POWER_STAT + (big_on * DFD_REG_OFFSET));
		stat = (FLUSH_LEVEL3 << 16) | (stat & 0xFFFF);
		writel(stat, CONFIG_RAMDUMP_GPR_POWER_STAT + (big_on * DFD_REG_OFFSET));
		printf("[BIG] core%d is running, it runs Cache Flush 3\n", big_on);
	}
}

void dfd_run_dump_gpr(unsigned long pmu_base)
{
	u32 cpu_logical_map[DFD_NR_CPUS] =
			{CPU0_LOGICAL_MAP,
			 CPU1_LOGICAL_MAP,
			 CPU2_LOGICAL_MAP,
			 CPU3_LOGICAL_MAP,
			 CPU4_LOGICAL_MAP,
			 CPU5_LOGICAL_MAP,
			 CPU6_LOGICAL_MAP,
			 CPU7_LOGICAL_MAP};
	u32 val;
	int ret;

	/* Check reset_sequencer_configuration register */
	if (readl(pmu_base + DFD_RESET_SEQUENCER_OFFSET) != DFD_EDPCSR_DUMP_EN) {
		return;
	}

	printf("---------------------------------------------------------\n");
	printf("Dump GPR_EN & Watchdog or Warm Reset Detected\n");

	/* Initialization to use waiting for complete Dump GPR of other cores */
	writel(0, CONFIG_RAMDUMP_DUMP_GPR_WAIT);

	/* Atlas core is needed for debugging as follow */
	ret = readl(pmu_base + DFD_ATLAS_DBG_CONFIG_OFFSET);
	ret |= DFD_ATLAS_DBG_INITIATE_WAKEUP;
	writel(ret, pmu_base + DFD_ATLAS_DBG_CONFIG_OFFSET);

	dfd_determine_cache_flush();

	for (val = 0; val < DFD_NR_CPUS; val++) {
		if(val==3)
			continue;
		/* Condition check : Core Power off / Panic at previous time */
		ret = readl(CONFIG_RAMDUMP_CORE_PANIC_STAT + (val * DFD_REG_OFFSET));
		if (ret == RAMDUMP_SIGN_PANIC) {
			printf("Core %d: already dump core in panic, Skip Dump GPR: 0x%x\n",
				val, ret);
			ret = readl(CONFIG_RAMDUMP_DUMP_GPR_WAIT);
			writel((ret | (1 << val)), CONFIG_RAMDUMP_DUMP_GPR_WAIT);
			continue;
		}
		ret = readl(CONFIG_RAMDUMP_GPR_POWER_STAT + (val * DFD_REG_OFFSET));
		if (ret & (HOTPLUG | C2_STATE | CLUSTER_OFF))
			printf("Core %d: Power Off at previous: 0x%x\n", val, ret);

		if (val == 0) {
			/* CPU 0 jumps just calling function */
			dfd_entry_dump_gpr();
			ret = 0;
		} else {
			/* ON other cpus */
			ret = cpu_boot(DFD_CPU_ON_PSCI_ID,
					cpu_logical_map[val],
					dfd_entry_dump_gpr);
			if (ret) {
				printf("ERROR: Core %d: failed to power on : 0x%x\n", val, ret);
				ret = readl(CONFIG_RAMDUMP_DUMP_GPR_WAIT);
				writel((ret | (1 << val)), CONFIG_RAMDUMP_DUMP_GPR_WAIT);
				continue;
			}
			ret = dfd_wait_complete(1 << val);
		}
		if (!ret)
			printf("Core %d: finished to dump GPR, Cache Flush level:%d\n",
				val, readl(CONFIG_RAMDUMP_DUMP_GPR_DEBUG + (val * DFD_REG_OFFSET)));
	}
	printf("---------------------------------------------------------\n");
}

void reset_prepare_board(void)
{
#ifndef CONFIG_SG_BINARY
	unsigned int rst_stat = readl(EXYNOS7420_POWER_BASE + 0x404);
	unsigned int dfd_en = readl(EXYNOS7420_POWER_BASE + DFD_RESET_SEQUENCER_OFFSET);

	if (rst_stat & (WRESET | APOLLO_WDTRESET | ATLAS_WDTRESET) &&
		dfd_en & DFD_EDPCSR_DUMP_EN) {
		printf("xyref7420 is not supported Fake S/W reset, It should be power off\n");
	}
#endif

}
