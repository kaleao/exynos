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
	int val;
	u32 ret, ret2;
	printf("Core stat at previous(IRAM)\n" );
	for (val = 0; val < DFD_NR_CPUS; val++) {
		ret = readl(DFD_CORE_STAT + (val * DFD_REG_OFFSET));
		printf("Core%d: ", val);
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

	/* APL core is needed for debugging as follow */
	ret = readl(pmu_base + DFD_APL_NONCPU_CONFIG_OFFSET);
	ret |= DFD_APL_NONCPU_INITIATE_WAKEUP;
	writel(ret, pmu_base + DFD_APL_NONCPU_CONFIG_OFFSET);

	/* copy IRAM core stat to DRAM */
	for (val = 0; val < DFD_NR_CPUS; val++) {
		ret = readl(DFD_CORE_STAT + (val * DFD_REG_OFFSET));

		writel(ret, CONFIG_RAMDUMP_GPR_POWER_STAT + (val * DFD_REG_OFFSET));

		/* clear IRAM core stat like cold-booting */
		if (val == 0)
			ret = RESERVED;
		else
			ret = HOTPLUG;
		writel(ret, DFD_CORE_STAT + (val * DFD_REG_OFFSET));
	}

	for (val = 0; val < DFD_NR_CPUS; val++) {
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
			printf("Core %d: Power Off at previous time: 0x%x\n", val, ret);

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
	unsigned int rst_stat = readl(EXYNOS7580_POWER_BASE + 0x404);
	unsigned int dfd_en = readl(EXYNOS7580_POWER_BASE + DFD_RESET_SEQUENCER_OFFSET);

	if (rst_stat & (WRESET | APL_WDTRESET | CPU_WDTRESET) &&
		dfd_en & DFD_EDPCSR_DUMP_EN) {

		printf("smdk7580 is not supported Fake S/W reset, It should be power off\n");

	}

}
