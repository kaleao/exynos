/*
 * (C) Copyright 2011 Samsung Electronics Co. Ltd
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <common.h>
#include <command.h>
#include <mmc.h>
#include <asm/arch/movi_partition.h>

int do_movi(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	char *cmd;
	ulong addr, start_blk, blkcnt;
	uint rfs_size;
	char run_cmd[100];
	char cmd_type[2][8]=
		{
			"mmc",
#ifdef CONFIG_SCSI_UFS
			"scsi",
#endif
		};
	uint rw = 0, attribute = -1;
	int i;
	member_t *image;
	int dev_num = 0;
	int location = 1;
	int ctype = 0;	/* use mmc command */
	int arg_offset;

	cmd = argv[1];

	if (!cmd) goto usage;

	switch (cmd[0]) {
	case 'i':
		if(argv[2]==NULL)
		{
			dev_num = 0;
		} else {
			dev_num = simple_strtoul(argv[2], NULL, 10);
		}

		sprintf(run_cmd,"mmcinfo %d", dev_num);
		run_command(run_cmd, 0);
		return 1;
	case 'r':
		rw = 0;	/* read case */
		break;
	case 'w':
		rw = 1; /* write case */
		break;
	default:
		goto usage;
	}

	cmd = argv[2];

	switch (cmd[0]) {
#ifdef CONFIG_SCSI_UFS
	case 's':
		location = 2;
		ctype = 1;	/* use scsi command */
		arg_offset = 3;
		break;
#endif
	case 'z':
		location = 0;
		arg_offset = 3;
		break;
	default:
		location = 1;
		arg_offset = 2;
		break;
	}

	dev_num = simple_strtoul(argv[arg_offset+1], NULL, 10);
	cmd = argv[arg_offset];

	switch (cmd[0]) {
#ifdef CONFIG_ARCH_EXYNOS7

	case 'f':
		/*
		 * eMMC / UFS : argument 6
		 * SD/MMC : argument 5
		 */
		if (argc != (arg_offset+3))
			goto usage;
		attribute = 0x0;
		break;
	case 'e':
		if (argc != (arg_offset+3))
			goto usage;
		attribute = 0x1;
		break;
	case 'b':
		if (argc != (arg_offset+3))
			goto usage;
		attribute = 0x2;
		break;
	case 'u':
		if (argc != (arg_offset+3))
			goto usage;
		attribute = 0x3;
		break;
	case 't':
		if (argc != (arg_offset+3))
			goto usage;
		attribute = 0x4;
		break;
	case 'k':
		if (argc != (5 + ctype))
			goto usage;
		attribute = 0x6;
		break;
	case 'r':
		if (argc != (6 + ctype))
			goto usage;
		attribute = 0x7;
		break;
	case 'd':
		if (argc != (5 + ctype))
			goto usage;
		attribute = 0x8;
		break;
	case 's':
		if (argc != (arg_offset+3))
			goto usage;
		attribute = 0x9;
		break;
	case 'l':
		if (argc != (5 + ctype))
			goto usage;
		attribute = 0xa;
		break;

#else /* CONFIG_ARCH_EXYNOS7 */

	case 'f':
		if (argc != (arg_offset+3))
			goto usage;
		attribute = 0x0;
		break;
	case 'b':
		if (argc != (arg_offset+3))
			goto usage;
		attribute = 0x1;
		break;
	case 'u':
		if (argc != (arg_offset+3))
			goto usage;
		attribute = 0x2;
		break;
	case 't':
		if (argc != (arg_offset+3))
			goto usage;
		attribute = 0x3;
		break;
	case 'k':
		if (argc != (5 + ctype))
			goto usage;
		attribute = 0x5;
		break;
	case 'r':
		if (argc != (6 + ctype))
			goto usage;
		attribute = 0x6;
		break;

#endif /* CONFIG_ARCH_EXYNOS7 */
	default:
		goto usage;
	}

	addr = simple_strtoul(argv[arg_offset+2], NULL, 16);

	init_raw_area_table(location);

	/* firmware BL1 r/w */
	if (attribute == 0x0) {
		for (i=0, image = raw_area_control.image; i<15; i++) {
			if (image[i].attribute == attribute)
				break;
		}

		start_blk = image[i].start_blk;
		blkcnt = image[i].used_blk;
		printf("%s FWBL1 ..device %d Start %ld, Count %ld ", rw ? "writing":"reading",
			dev_num, start_blk, blkcnt);
		sprintf(run_cmd,"%s %s %d 0x%lx 0x%lx 0x%lx",
			cmd_type[ctype], rw ? "write":"read", dev_num,
			addr, start_blk, blkcnt);
		run_command(run_cmd, 0);
		printf("completed\n");
		return 1;
	}
	/* BL2 r/w */
	/* el3 monitor r/w (exynos7) */
	if (attribute == 0x1) {
		for (i=0, image = raw_area_control.image; i<15; i++) {
			if (image[i].attribute == attribute)
				break;
		}
		start_blk = image[i].start_blk;
		blkcnt = image[i].used_blk;
#ifdef CONFIG_ARCH_EXYNOS7
		printf("%s el3 monitor ..device %d Start %ld, Count %ld ", rw ? "writing":"reading",
			dev_num, start_blk, blkcnt);
#else /* CONFIG_ARCH_EXYNOS7 */
		printf("%s BL2 ..device %d Start %ld, Count %ld ", rw ? "writing":"reading",
			dev_num, start_blk, blkcnt);
#endif /* CONFIG_ARCH_EXYNOS7 */
		sprintf(run_cmd,"%s %s %d 0x%lx 0x%lx 0x%lx",
			cmd_type[ctype], rw ? "write":"read", dev_num,
			addr, start_blk, blkcnt);
		run_command(run_cmd, 0);
		printf("completed\n");
		return 1;
	}
	/* u-boot r/w */
	/* BL2 r/w (exynos7) */
	if (attribute == 0x2) {
		for (i=0, image = raw_area_control.image; i<15; i++) {
			if (image[i].attribute == attribute)
				break;
		}
		start_blk = image[i].start_blk;
		blkcnt = image[i].used_blk;
#ifdef CONFIG_ARCH_EXYNOS7
		printf("%s bl2 ..device %d Start %ld, Count %ld ", rw ? "writing":"reading",
			dev_num, start_blk, blkcnt);
#else /* CONFIG_ARCH_EXYNOS7 */
		printf("%s bootloader..device %d Start %ld, Count %ld ", rw ? "writing":"reading",
			dev_num, start_blk, blkcnt);
#endif /* CONFIG_ARCH_EXYNOS7 */
		sprintf(run_cmd,"%s %s %d 0x%lx 0x%lx 0x%lx",
			cmd_type[ctype], rw ? "write":"read", dev_num,
			addr, start_blk, blkcnt);
		run_command(run_cmd, 0);
		printf("completed\n");
		return 1;
	}

	/* TrustZone S/W */
	/* u-boot r/w (exynos7) */
	if (attribute == 0x3) {
		for (i=0, image = raw_area_control.image; i<15; i++) {
			if (image[i].attribute == attribute)
				break;
		}
		start_blk = image[i].start_blk;
		blkcnt = image[i].used_blk;
#ifdef CONFIG_ARCH_EXYNOS7
		printf("%s bootloader ..device %d Start %ld, Count %ld ", rw ? "writing":"reading",
			dev_num, start_blk, blkcnt);
#else /* CONFIG_ARCH_EXYNOS7 */
		printf("%s %d TrustZone S/W.. Start %ld, Count %ld ", rw ? "writing" : "reading",
		       dev_num, start_blk, blkcnt);
#endif /* CONFIG_ARCH_EXYNOS7 */
		sprintf(run_cmd, "%s %s %d 0x%lx 0x%lx 0x%lx",
			cmd_type[ctype], rw ? "write" : "read", dev_num,
			addr, start_blk, blkcnt);
		run_command(run_cmd, 0);
		printf("completed\n");
		return 1;
	}

	/* TrustZone S/W r/w (exynos7) */
	if (attribute == 0x4) {
		for (i=0, image = raw_area_control.image; i<15; i++) {
			if (image[i].attribute == attribute)
				break;
		}
		start_blk = image[i].start_blk;
		blkcnt = image[i].used_blk;
		printf("%s TrustZone S/W ..device %d Start %ld, Count %ld ", rw ? "writing":"reading",
			dev_num, start_blk, blkcnt);
		sprintf(run_cmd, "%s %s %d 0x%lx 0x%lx 0x%lx",
			cmd_type[ctype], rw ? "write" : "read", dev_num,
			addr, start_blk, blkcnt);
		run_command(run_cmd, 0);
		printf("completed\n");
		return 1;
	}

	/* kernel r/w */
	if (attribute == 0x5) {
		for (i=0, image = raw_area_control.image; i<15; i++) {
			if (image[i].attribute == attribute)
				break;
		}
		start_blk = 4096;
		blkcnt = image[i].used_blk;
		printf("%s kernel..device %d Start %ld, Count %ld ", rw ? "writing" : "reading",
			dev_num, start_blk, blkcnt);
		sprintf(run_cmd, "%s %s %d 0x%lx 0x%lx 0x%lx",
			cmd_type[ctype], rw ? "write" : "read", dev_num,
			addr, start_blk, blkcnt);
		run_command(run_cmd, 0);
		printf("completed\n");
		return 1;
	}

#ifdef CONFIG_ARCH_EXYNOS7
	/* kernel r/w (armv8) */
	if (attribute == 0x6) {
		for (i=0, image = raw_area_control.image; i<15; i++) {
			if (image[i].attribute == attribute)
				break;
		}
		start_blk = 4096;
		blkcnt = 40960;
		printf("%s kernel..device %d Start %ld, Count %ld ", rw ? "writing":"reading",
			dev_num, start_blk, blkcnt);
		sprintf(run_cmd,"%s %s %d 0x%lx 0x%lx 0x%lx",
			cmd_type[ctype], rw ? "write":"read", dev_num,
			addr, start_blk, blkcnt);
		run_command(run_cmd, 0);
		printf("completed\n");
		return 1;
	}

#else /* CONFIG_ARCH_EXYNOS7 */
	/* root file system r/w */
	if (attribute == 0x6) {
		rfs_size = simple_strtoul(argv[5+ctype], NULL, 16);

		for (i=0, image = raw_area_control.image; i<15; i++) {
			if (image[i].attribute == attribute)
				break;
		}
		start_blk = 43513;
		blkcnt = rfs_size/MOVI_BLKSIZE +
			((rfs_size&(MOVI_BLKSIZE-1)) ? 1 : 0);
		image[i].used_blk = blkcnt;
		printf("%s RFS..device %d Start %ld, Count %ld ", rw ? "writing":"reading",
			dev_num, start_blk, blkcnt);
		sprintf(run_cmd,"%s %s %d 0x%lx 0x%lx 0x%lx",
			cmd_type[ctype], rw ? "write":"read", dev_num,
			addr, start_blk, blkcnt);
		run_command(run_cmd, 0);
		printf("completed\n");
		return 1;
	}
#endif /* CONFIG_ARCH_EXYNOS7 */

#ifdef CONFIG_ARCH_EXYNOS7
	/* root file system r/w (armv8) */
	if (attribute == 0x7) {
		rfs_size = simple_strtoul(argv[5+ctype], NULL, 16);

		for (i=0, image = raw_area_control.image; i<15; i++) {
			if (image[i].attribute == attribute)
				break;
		}
		start_blk = 43513;
		blkcnt = rfs_size/MOVI_BLKSIZE +
			((rfs_size&(MOVI_BLKSIZE-1)) ? 1 : 0);
		image[i].used_blk = blkcnt;
		printf("%s RFS..device %d Start %ld, Count %ld ", rw ? "writing":"reading",
			dev_num, start_blk, blkcnt);
		sprintf(run_cmd,"%s %s %d 0x%lx 0x%lx 0x%lx",
			cmd_type[ctype], rw ? "write":"read", dev_num,
			addr, start_blk, blkcnt);
		run_command(run_cmd, 0);
		printf("completed\n");
		return 1;
	}

	/* dtb r/w (armv8) */
	if (attribute == 0x8) {
		for (i=0, image = raw_area_control.image; i<15; i++) {
			if (image[i].attribute == attribute)
				break;
		}
		start_blk = 151552;
		blkcnt = image[i].used_blk;
		printf("%s dtb..device %d Start %ld, Count %ld ", rw ? "writing":"reading",
			dev_num, start_blk, blkcnt);
		sprintf(run_cmd,"%s %s %d 0x%lx 0x%lx 0x%lx",
			cmd_type[ctype], rw ? "write":"read", dev_num,
			addr, start_blk, blkcnt);
		run_command(run_cmd, 0);
		printf("completed\n");
		return 1;
	}

	/* bootloader set r/w */
	if (attribute == 0x9) {
		printf("%s bootloader set..device %d Start %ld, Count %ld ", rw ? "writing":"reading",
			dev_num, location, MOVI_BOOTLOADER_SET_BLKCNT);
		sprintf(run_cmd,"%s %s %d 0x%lx 0x%lx 0x%lx",
			cmd_type[ctype], rw ? "write":"read", dev_num,
			addr, location, MOVI_BOOTLOADER_SET_BLKCNT);
		run_command(run_cmd, 0);
		printf("completed\n");
		return 1;
	}

	/* Loadable F/W r/w */
	if (attribute == 0xa) {
		for (i=0, image = raw_area_control.image; i<15; i++) {
			if (image[i].attribute == attribute)
				break;
		}
		start_blk = image[i].start_blk;
		blkcnt = image[i].used_blk;
		printf("%s Loadable F/W..device %d Start %ld, Count %ld ", rw ? "writing":"reading",
			dev_num, start_blk, blkcnt);
		sprintf(run_cmd,"%s %s %d 0x%lx 0x%lx 0x%lx",
			cmd_type[ctype], rw ? "write":"read", dev_num,
			addr, start_blk, blkcnt);
		run_command(run_cmd, 0);
		printf("completed\n");
		return 1;
	}


#endif /* CONFIG_ARCH_EXYNOS7 */

	return 1;

usage:
	printf("Usage:\n%s\n", cmdtp->usage);
	return -1;
}

#ifdef CONFIG_ARCH_EXYNOS7
U_BOOT_CMD(
	movi,	7,	0,	do_movi,
	"movi\t- sd/mmc r/w sub system for SMDK board",
	"init - Initialize moviNAND and show card info\n"
#ifdef CONFIG_SCSI_UFS
	"#UFS#\n"
	"movi read scsi {fwbl1 | bl2 | u-boot | el3_mon | tzsw} {device_number} {addr} - Read data from UFS\n"
	"movi write scsi {fwbl1 | bl2 | u-boot | el3_mon | tzsw} {device_number} {addr} - Write data to UFS\n"
	"movi read scsi kernel {device_number} {addr} - Read Kernel from UFS\n"
	"movi write scsi kernel {device_number} {addr} - Write Kernel to UFS\n"
	"movi read scsi rootfs {device_number} {addr} [bytes(hex)] - Read rootfs data from UFS by size\n"
	"movi write scsi rootfs {device_number} {addr} [bytes(hex)] - Write rootfs data to UFS by size\n"
	"movi read scsi dtb {device_number} {addr} - Read dtb data from UFS\n"
	"movi write scsi dtb {device_number} {addr} - Write dtb data to UFS\n"
	"movi read scsi ldfw {device_number} {addr} - Read Loadable F/W data from UFS\n"
	"movi write scsi ldfw {device_number} {addr} - Write Loadable F/W data to UFS\n"
#endif
	"#eMMC#\n"
	"movi read zero {fwbl1 | bl2 | u-boot | el3_mon | tzsw | set} {device_number} {addr} - Read data from emmc\n"
	"movi write zero {fwbl1 | bl2 | u-boot | el3_mon | tzsw | set} {device_number} {addr} - write data to emmc\n"
	"#SD/MMC#\n"
	"movi read {fwbl1 | bl2 | u-boot | el3_mon | tzsw | set} {device_number} {addr} - Read data from sd/mmc\n"
	"movi write {fwbl1 | bl2 | u-boot | el3_mon | tzsw | set} {device_number} {addr} - write data to sd/mmc\n"
	"#COMMON#\n"
	"movi read kernel {device_number} {addr} - Read kernel data from device\n"
	"movi write kernel {device_number} {addr} - Write kernel data to device\n"
	"movi read rootfs {device_number} {addr} [bytes(hex)] - Read rootfs data from device by size\n"
	"movi write rootfs {device_number} {addr} [bytes(hex)] - Write rootfs data to device by size\n"
	"movi read dtb {device_number} {addr} - Read dtb data from device\n"
	"movi write dtb {device_number} {addr} - Write dtb data to device\n"
	"movi read ldfw {device_number} {addr} - Read Loadable F/W data from device\n"
	"movi write ldfw {device_number} {addr} - Write Loadable F/W data to device\n"
	"movi read {sector#} {device_number} {bytes(hex)} {addr} - instead of this, you can use \"mmc read\"\n"
	"movi write {sector#} {device_number} {bytes(hex)} {addr} - instead of this, you can use \"mmc write\"\n"
);
#else /* CONFIG_ARCH_EXYNOS7 */
U_BOOT_CMD(
	movi,	7,	0,	do_movi,
	"movi\t- sd/mmc r/w sub system for SMDK board",
	"init - Initialize moviNAND and show card info\n"
	"#eMMC#\n"
	"movi read zero {fwbl1 | bl2 | u-boot | tzsw} {device_number} {addr} - Read data from emmc\n"
	"movi write zero {fwbl1 | bl2 | u-boot | tzsw} {device_number} {addr} - Write data to emmc\n"
	"#SD/MMC#\n"
	"movi read {fwbl1 | bl2 | u-boot | tzsw} {device_number} {addr} - Read data from sd/mmc\n"
	"movi write {fwbl1 | bl2 | u-boot | tzsw} {device_number} {addr} - Write data to sd/mmc\n"
	"#COMMON in eMMC or SD/MMC#\n"
	"movi read kernel {device_number} {addr} - Read data from device\n"
	"movi write kernel {device_number} {addr} - Write data to device\n"
	"movi read rootfs {device_number} {addr} [bytes(hex)] - Read rootfs data from device by size\n"
	"movi write rootfs {device_number} {addr} [bytes(hex)] - Write rootfs data to device by size\n"
	"movi read {sector#} {device_number} {bytes(hex)} {addr} - instead of this, you can use \"mmc read\"\n"
	"movi write {sector#} {device_number} {bytes(hex)} {addr} - instead of this, you can use \"mmc write\"\n"
);
#endif /* CONFIG_ARCH_EXYNOS7 */


