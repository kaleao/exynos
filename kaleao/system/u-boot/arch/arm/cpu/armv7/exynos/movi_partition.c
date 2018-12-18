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
#include <asm/arch/movi_partition.h>
#include <asm/arch/cpu.h>

#ifdef DEBUG_MOVI_PARTITION
#define dbg(x...)       printf(x)
#else
#define dbg(x...)       do { } while (0)
#endif

raw_area_t raw_area_control;

int init_raw_area_table(int location)
{
	int i;
	member_t *image;
#if defined(CONFIG_CPU_EXYNOS7420) || defined(CONFIG_CPU_EXYNOS5433_EVT1)
	unsigned int each_blkcnt[9];
#endif

	/* init raw_area will be 16MB */
	raw_area_control.start_blk = 16*1024*1024/MOVI_BLKSIZE;
	raw_area_control.next_raw_area = 0;
	strcpy(raw_area_control.description, "initial raw table");

	image = raw_area_control.image;

	/* For eMMC partition BLOCK Change*/
#if defined(CONFIG_CPU_EXYNOS7420) || defined(CONFIG_CPU_EXYNOS5433_EVT1)
	if (location == 0 || location == 1) {
		each_blkcnt[0] = MOVI_BL1_BLKCNT;
		each_blkcnt[1] = MOVI_EL3_MON_BLKCNT;
		each_blkcnt[2] = MOVI_BL2_BLKCNT;
		each_blkcnt[3] = MOVI_UBOOT_BLKCNT;
		each_blkcnt[4] = MOVI_MOBICORE_BLKCNT;
		each_blkcnt[5] = MOVI_ENV_BLKCNT;
		each_blkcnt[6] = MOVI_KERNEL_BLKCNT;
		each_blkcnt[7] = MOVI_ROOTFS_BLKCNT;
		each_blkcnt[8] = MOVI_DTB_BLKCNT;
	} else if (location == 2) {
		each_blkcnt[0] = UFS_BL1_BLKCNT * 8;
		each_blkcnt[1] = UFS_EL3_MON_BLKCNT * 8;
		each_blkcnt[2] = UFS_BL2_BLKCNT * 8;
		each_blkcnt[3] = UFS_UBOOT_BLKCNT * 8;
		each_blkcnt[4] = UFS_MOBICORE_BLKCNT * 8;
		each_blkcnt[5] = UFS_ENV_BLKCNT * 8;
		each_blkcnt[6] = UFS_KERNEL_BLKCNT * 8;
		each_blkcnt[7] = UFS_ROOTFS_BLKCNT * 8;
		each_blkcnt[8] = UFS_DTB_BLKCNT * 8;
	} else {
		printf("unkown location(%d)\n", location);
		return -1;
	}

	/* image 0 should be bl1 */
	image[0].start_blk = (location == 1) ? 1 : 0;
	image[0].used_blk = each_blkcnt[0];
	image[0].size = PART_SIZE_BL1;
	image[0].attribute = 0x0;
	strcpy(image[0].description, "fwbl1");
	dbg("fwbl1: %d\n", image[0].start_blk);

	/* image 1 should be secure monitor */
	image[1].start_blk = image[0].start_blk + each_blkcnt[0];
	image[1].used_blk = each_blkcnt[1];
	image[1].size = PART_SIZE_EL3_MON;
	image[1].attribute = 0x1;
	strcpy(image[1].description, "el3_mon");
	dbg("el3_mon: %d\n", image[1].start_blk);

	/* image 2 should be bl2 */
	image[2].start_blk = image[1].start_blk + each_blkcnt[1];
	image[2].used_blk = each_blkcnt[2];
	image[2].size = PART_SIZE_BL2;
	image[2].attribute = 0x2;
	strcpy(image[2].description, "bl2");
	dbg("bl2: %d\n", image[2].start_blk);

	/* image 3 should be u-boot */
	image[3].start_blk = image[2].start_blk + each_blkcnt[2];
	image[3].used_blk = each_blkcnt[3];
	image[3].size = PART_SIZE_UBOOT;
	image[3].attribute = 0x3;
	strcpy(image[3].description, "u-boot");
	dbg("u-boot: %d\n", image[3].start_blk);

	/* image 4 should be TrustZone S/W */
	image[4].start_blk = image[3].start_blk + each_blkcnt[3];
	image[4].used_blk = each_blkcnt[4];
	image[4].size = PART_SIZE_MOBICORE;
	image[4].attribute = 0x4;
	strcpy(image[4].description, "TrustZone S/W");
	dbg("TrustZone S/W: %d\n", image[4].start_blk);

	/* image 5 should be environment */
	image[5].start_blk = image[4].start_blk + each_blkcnt[4];
	image[5].used_blk = each_blkcnt[5];
	image[5].size = CONFIG_ENV_SIZE;
	image[5].attribute = 0x5;
	strcpy(image[5].description, "environment");
	dbg("env: %d\n", image[5].start_blk);


	/* For eMMC partition BLOCK Change*/
	if (location == 0)
		image[5].start_blk = image[5].start_blk + 1;

	/* image 6 should be kernel */
	image[6].start_blk = image[5].start_blk + each_blkcnt[5];
	image[6].used_blk = each_blkcnt[6];
	image[6].size = PART_SIZE_KERNEL;
	image[6].attribute = 0x6;
	strcpy(image[6].description, "kernel");
	dbg("knl: %d\n", image[6].start_blk);

	/* image 7 should be RFS */
	image[7].start_blk = image[6].start_blk + each_blkcnt[6];
	image[7].used_blk = each_blkcnt[7];
	image[7].size = PART_SIZE_ROOTFS;
	image[7].attribute = 0x7;
	strcpy(image[7].description, "rfs");
	dbg("rfs: %d\n", image[7].start_blk);

	/* image 8 should be DTB */
	image[8].start_blk = image[7].start_blk + each_blkcnt[7];
	image[8].used_blk = each_blkcnt[8];
	image[8].size = PART_SIZE_DTB;
	image[8].attribute = 0x8;
	strcpy(image[8].description, "dtb");
	dbg("dtb: %d\n", image[8].start_blk);

#else /* CONFIG_CPU_EXYNOS7420 */

	/* image 0 should be fwbl1 */
	image[0].start_blk = location;
	image[0].used_blk = MOVI_BL1_BLKCNT;
	image[0].size = PART_SIZE_BL1;
	image[0].attribute = 0x0;
	strcpy(image[0].description, "bl1");
	dbg("fwbl1: %d\n", image[0].start_blk);

	/* image 1 should be bl2 */
	image[1].start_blk = image[0].start_blk + MOVI_BL1_BLKCNT;
	image[1].used_blk = MOVI_BL2_BLKCNT;
	image[1].size = PART_SIZE_BL2;
	image[1].attribute = 0x1;
	strcpy(image[1].description, "bl2");
	dbg("bl2: %d\n", image[1].start_blk);

	/* image 2 should be u-boot */
	image[2].start_blk = image[1].start_blk + MOVI_BL2_BLKCNT;
	image[2].used_blk = MOVI_UBOOT_BLKCNT;
	image[2].size = PART_SIZE_UBOOT;
	image[2].attribute = 0x2;
	strcpy(image[2].description, "u-boot");
	dbg("u-boot: %d\n", image[2].start_blk);

	/* image 3 should be TrustZone S/W */
	image[3].start_blk = image[2].start_blk + MOVI_UBOOT_BLKCNT;
	image[3].used_blk = MOVI_TZSW_BLKCNT;
	image[3].size = PART_SIZE_TZSW;
	image[3].attribute = 0x3;
	strcpy(image[3].description, "TrustZone S/W");
	dbg("TrustZone S/W: %d\n", image[3].start_blk);

	/* image 4 should be environment */
	image[4].start_blk = image[3].start_blk + MOVI_TZSW_BLKCNT;
	image[4].used_blk = MOVI_ENV_BLKCNT;
	image[4].size = CONFIG_ENV_SIZE;
	image[4].attribute = 0x4;
	strcpy(image[4].description, "environment");
	dbg("env: %d\n", image[4].start_blk);


	/* For eMMC partition BLOCK Change*/
	if (location == 0)
		image[4].start_blk = image[4].start_blk + 1;

	/* image 5 should be kernel */
	image[5].start_blk = image[4].start_blk + MOVI_ENV_BLKCNT;
	image[5].used_blk = MOVI_ZIMAGE_BLKCNT;
	image[5].size = PART_SIZE_KERNEL;
	image[5].attribute = 0x5;
	strcpy(image[5].description, "kernel");
	dbg("knl: %d\n", image[5].start_blk);

	/* image 6 should be RFS */
	image[6].start_blk = image[5].start_blk + MOVI_ZIMAGE_BLKCNT;
	image[6].used_blk = MOVI_ROOTFS_BLKCNT;
	image[6].size = PART_SIZE_ROOTFS;
	image[6].attribute = 0x6;
	strcpy(image[6].description, "rfs");
	dbg("rfs: %d\n", image[6].start_blk);

#endif /* CONFIG_CPU_EXYNOS7420 */

	for (i=9; i<15; i++) {
		raw_area_control.image[i].start_blk = 0;
		raw_area_control.image[i].used_blk = 0;
	}

	return 0;
}

