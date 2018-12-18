/*
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

#include <common.h>
#include <asm/arch/cpu.h>
#include <asm/arch/clk.h>
#include <asm/arch/clock.h>
#include <asm/arch/power.h>
#include <asm/arch/smc.h>
#include <asm/types.h>
#include <asm/arch/movi_partition.h>

#define EXYNOS7420_LDFW_NWD_ADDR	(0x48000000)
#define EXYNOS7420_LDFW_PARTITION_SIZE	(PART_SIZE_LOADABLE_FW)
#define EXYNOS_LDFW_MAGIC		(0x10adab1e)

#define LDFW_DEBUG

#define CHECK_SIGNATURE_FAIL		(0x2)
#define CHECK_ROLL_BACK_COUNT_FAIL	(0x20)

#ifdef LDFW_DEBUG
#define LDFW_INFO(format, arg...)	printf(format, ##arg)
#define LDFW_WARN(format, arg...)	printf(format, ##arg)
#define LDFW_ERR(format, arg...)	printf(format, ##arg)
#else
#define LDFW_INFO(msg)
#define LDFW_WARN(msg)
#define LDFW_ERR(format, arg...)	printf(format, ##arg)
#endif

int get_userLUN(void);
int do_movi(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[]);
int init_ldfw(u64 addr, u64 size);

struct fw_header {
	unsigned int magic;  /* Should be ‘0x10adab1e’ */
	unsigned int size;
	unsigned int init_entry;
	unsigned int entry_point;
	unsigned int suspend_entry;
	unsigned int resume_entry;
	unsigned int start_smc_id;
	unsigned int version;
	unsigned int set_runtime_entry;
	unsigned int reserved1;
	unsigned int reserved2;
	unsigned int reserved3;
	char fw_name[16];
};

int load_ldfw_parition(u64 addr)
{
	char cmd[32], part[32], part2[32];
	char buffer[32], dev_num[32];
	char *argv[7]  = { NULL, NULL, NULL, NULL, NULL, NULL, NULL};
	int argc = 0;
	struct exynos7420_power *pmu = (struct exynos7420_power *)EXYNOS7420_POWER_BASE;
	int OmPin;
	int ret;

	OmPin = readl(&pmu->inform3);

        argv[1] = cmd;
        sprintf(cmd, "read");

	if (OmPin == BOOT_UFS) {
		LDFW_INFO("ldfw: booting device is UFS.\n");
		argv[2] = part2;
		argv[3] = part;
		argv[4] = dev_num;
		argv[5] = buffer;
		strncpy(part2, "scsi", 7);
		sprintf(dev_num, "%u", get_userLUN());
		argc = 6;
	} else if (OmPin == BOOT_MMCSD || OmPin == BOOT_EMMC_4_4 || \
			OmPin == BOOT_EMMC) {
		LDFW_INFO("ldfw: booting device is eMMC.\n");
		argv[2] = part;
		argv[3] = dev_num;
		argv[4] = buffer;
		sprintf(dev_num, "%d", 0);
		argc = 5;
	} else {
		/* LDFW should be on eMMC or UFS */
		LDFW_INFO("ldfw: This booting device is not supported.\n");
		return -1;
	}

	sprintf(buffer, "0x%x", (u32)addr);
	sprintf(part, "%s", "ldfw");

	LDFW_INFO("ldfw: read whole ldfw partition from the storage\n");
	ret = do_movi(NULL, 0, argc, argv);
	if (ret != 1)
		LDFW_ERR("ldfw: there is no ldfw partition\n");

	return ret == 1 ? 0:-1;
}

int init_ldfws(void)
{
	u64 addr = EXYNOS7420_LDFW_NWD_ADDR;
	u64 size = 0;
	struct fw_header *fwh;
	s64 ret;
	u32 try, try_fail, i;
	char name[17] = {0,};

#ifndef CONFIG_ARM64
	return -1;
#endif

	if (load_ldfw_parition(addr)) {
		LDFW_ERR("ldfw: can not read ldfw from the storage\n");
		return -1;
	}

	fwh = (struct fw_header *)addr;
	if (fwh->magic != EXYNOS_LDFW_MAGIC) {
		/* there are no ldfw */
		LDFW_ERR("ldfw: there is no ldfw at ldfw partition\n");
		return -1;
	} else {
		for(i=0;;i++) {
			if (fwh->magic != EXYNOS_LDFW_MAGIC)
				break;
			strncpy(name, fwh->fw_name, 16);
			LDFW_INFO("ldfw: %dth ldfw's version 0x%x name : %s\n",
				i, fwh->version, name);
			size += (u64)fwh->size;
			fwh = (struct fw_header *)((u64)fwh + (u64)fwh->size);
		}
	}

	LDFW_INFO("ldfw: init ldfw(s). whole ldfws size 0x%x\n", size);
	ret = init_ldfw(addr, size);

	if ((ret <0) && (ret & CHECK_SIGNATURE_FAIL))
		LDFW_ERR("ldfw: signature of ldfw is corrupted.!\n");
	else if ((ret < 0) && (ret & CHECK_ROLL_BACK_COUNT_FAIL))
		LDFW_ERR("ldfw: roll back count of ldfw is corrupted.!\n");
	else if (!ret)
		LDFW_WARN("ldfw: No ldfw is inited\n");
	else {
		try = ret & 0xffff;
		try_fail = (ret >> 16) & 0xffff;
		LDFW_INFO("ldfw: try to init %d ldfw(s). except %d ldfw " \
			"%d ldfw(s) have been inited done.\n", \
				try, try_fail, try - try_fail);
	}

	return 0;
}
