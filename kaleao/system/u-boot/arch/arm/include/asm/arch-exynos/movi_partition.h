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

#ifndef __MOVI_PARTITION_H__
#define __MOVI_PARTITION_H__

/*
	device 0 : eMMC
	device 1 : SD/MMC
	device 2 : UFS
*/
#define MOVI_DEVICE_MAX_TYPE	3
#define SCSI_TAG	0x40000000
#define IN_BOOT	0x80000000
#define FORCE_UPDATE	0x20000000

#define eFUSE_SIZE		(1 * 512)	// 512 Byte eFuse, 512 Byte reserved

#define MOVI_BLKSIZE		(1<<9) /* 512 bytes */
#define UFS_BLKSIZE		(1<<12) /* 4096 bytes */

#if defined(CONFIG_ARCH_EXYNOS7)

/* partition information for el3_mon */
#define PART_SIZE_BL1		(8 * 1024)
#ifdef CONFIG_ARM64
#if defined(CONFIG_CPU_EXYNOS5433_EVT1)
#define PART_SIZE_EL3_MON	(92 * 1024)
#else
#define PART_SIZE_EL3_MON	(192 * 1024)
#endif
#else
#if defined(CONFIG_CPU_EXYNOS5433_EVT1)
#define PART_SIZE_EL3_MON	(92 * 1024)
#else
#define PART_SIZE_EL3_MON	(64 * 1024)
#endif
#endif
#if defined(CONFIG_SPL_19KB)
#define PART_SIZE_BL2		(20 * 1024)
#define PART_SIZE_BL2_NORMAL	(20 * 1024)
#define PART_SIZE_BL2_SG	(0)
#elif defined(CONFIG_SPL_27KB)
#define PART_SIZE_BL2_NORMAL	(27 * 1024)
#define PART_SIZE_BL2_SG	(20 * 1024)
#define PART_SIZE_BL2_SIGNATURE	(1 * 1024)
#define PART_SIZE_BL2		(PART_SIZE_BL2_NORMAL + PART_SIZE_BL2_SG + PART_SIZE_BL2_SIGNATURE)
#else
#define PART_SIZE_BL2		(16 * 1024)
#endif
#define PART_SIZE_UBOOT		(528 * 1024)
#define PART_SIZE_MOBICORE	(512 * 1024)
#if defined(CONFIG_CPU_EXYNOS5433_EVT1)
#ifdef CONFIG_ARM64
#define PART_SIZE_KERNEL	(20 * 1024 * 1024)
#else
#define PART_SIZE_KERNEL	(5 * 1024 * 1024)
#endif
#else
#define PART_SIZE_KERNEL	(20 * 1024 * 1024)
#endif
#define PART_SIZE_ROOTFS	(26 * 1024 * 1024)
#define PART_SIZE_DTB		(1 * 1024 * 1024)
#define PART_SIZE_BOOTLOADER_SET	(1536 * 1024)
#define PART_SIZE_LOADABLE_FW	(2048 * 1024)

#define MOVI_BL1_BLKCNT		(PART_SIZE_BL1 / MOVI_BLKSIZE)		/* 8KB */
#define MOVI_EL3_MON_BLKCNT	(PART_SIZE_EL3_MON / MOVI_BLKSIZE)
#define MOVI_BL2_BLKCNT		(PART_SIZE_BL2 / MOVI_BLKSIZE)
#define MOVI_UBOOT_BLKCNT	(PART_SIZE_UBOOT / MOVI_BLKSIZE)	/* 328KB */
#define MOVI_MOBICORE_BLKCNT	(PART_SIZE_MOBICORE / MOVI_BLKSIZE)	/* 256KB */
#define MOVI_ENV_BLKCNT		(CONFIG_ENV_SIZE / MOVI_BLKSIZE)	/* 16KB */

#define MOVI_KERNEL_BLKCNT	(PART_SIZE_KERNEL / MOVI_BLKSIZE)	/* 20MB */
#define MOVI_ROOTFS_BLKCNT	(PART_SIZE_ROOTFS / MOVI_BLKSIZE)	/* 26MB */
#define MOVI_DTB_BLKCNT		(PART_SIZE_DTB / MOVI_BLKSIZE)		/* 1MB */
#define MOVI_LOADABLE_FW_BLKCNT	(PART_SIZE_LOADABLE_FW / MOVI_BLKSIZE)	/* 2MB */
#define MOVI_BOOTLOADER_SET_BLKCNT	\
	(PART_SIZE_BOOTLOADER_SET / MOVI_BLKSIZE)		/* 1.5MB */

#define MOVI_UBOOT_POS		((eFUSE_SIZE / MOVI_BLKSIZE) + \
				MOVI_BL1_BLKCNT + \
				MOVI_EL3_MON_BLKCNT + \
				MOVI_BL2_BLKCNT)
#define MOVI_TZSW_POS          (MOVI_UBOOT_POS + MOVI_UBOOT_BLKCNT)
#define MOVI_ENV_POS           (MOVI_TZSW_POS + MOVI_MOBICORE_BLKCNT ) 
#define CONFIG_ENV_OFFSET	(MOVI_ENV_POS * MOVI_BLKSIZE)

/* UFS support */
#define UFS_eFUSE_BLKCNT	0	/* (eFUSE_SIZE / UFS_BLKSIZE) */
#define UFS_BL1_BLKCNT	       	(PART_SIZE_BL1 / UFS_BLKSIZE)
#define UFS_EL3_MON_BLKCNT	(PART_SIZE_EL3_MON / UFS_BLKSIZE)
#define UFS_BL2_BLKCNT		(PART_SIZE_BL2 / UFS_BLKSIZE)
#define UFS_ENV_BLKCNT		(CONFIG_ENV_SIZE / UFS_BLKSIZE)		/* 16KB */
#define UFS_UBOOT_BLKCNT	(PART_SIZE_UBOOT / UFS_BLKSIZE)		/* 328KB */
#define UFS_KERNEL_BLKCNT	(PART_SIZE_KERNEL / UFS_BLKSIZE)	/* 4MB */
#define UFS_ROOTFS_BLKCNT	(PART_SIZE_ROOTFS / UFS_BLKSIZE)	/* 26MB */
#define UFS_DTB_BLKCNT		(PART_SIZE_DTB / UFS_BLKSIZE)		/* 1MB */
#define UFS_MOBICORE_BLKCNT	(PART_SIZE_MOBICORE / UFS_BLKSIZE)		/* 160KB */
#define UFS_LOADABLE_FW_BLKCNT	(PART_SIZE_LOADABL_FW / UFS_BLKSIZE)	/* 2MB */
#define UFS_UBOOT_POS		(UFS_eFUSE_BLKCNT + \
				UFS_BL1_BLKCNT + \
				UFS_EL3_MON_BLKCNT + \
				UFS_BL2_BLKCNT)
#define UFS_TZSW_POS           	(UFS_UBOOT_POS + UFS_UBOOT_BLKCNT)
#define UFS_ENV_POS		        (UFS_TZSW_POS + UFS_MOBICORE_BLKCNT)
#define CONFIG_UFS_ENV_OFFSET	(UFS_ENV_POS * UFS_BLKSIZE)
#define PART_SIZE_BOOTLOADER 	(PART_SIZE_EL3_MON + PART_SIZE_BL2 + PART_SIZE_BL1 + PART_SIZE_UBOOT + PART_SIZE_MOBICORE)
#define MOVI_BOOTLOADER_BLKCNT	(PART_SIZE_BOOTLOADER / MOVI_BLKSIZE)
#else /* CONFIG_ARCH_EXYNOS7 */
/* partition information */
#if defined(CONFIG_BL_MONITOR)
#ifdef CONFIG_CPU_EXYNOS5433
#define PART_SIZE_BL1		(14 * 1024)
#else
#define PART_SIZE_BL1		(15 * 1024)
#endif
#else
#define PART_SIZE_BL1		(8 * 1024)
#endif

#define PART_SIZE_BL2		(16 * 1024)
#define PART_SIZE_UBOOT		(328 * 1024)
#if defined(CONFIG_CPU_EXYNOS5260) || defined(CONFIG_CPU_EXYNOS3250) || defined(CONFIG_CPU_EXYNOS3472)
#define PART_SIZE_KERNEL	(8 * 1024 * 1024)
#elif defined(CONFIG_CPU_EXYNOS5430) || defined(CONFIG_CPU_EXYNOS5412)
#define PART_SIZE_KERNEL	(5 * 1024 * 1024)
#else
#define PART_SIZE_KERNEL	(4 * 1024 * 1024)
#endif
#define PART_SIZE_ROOTFS	(26 * 1024 * 1024)
#if defined(CONFIG_EXYNOS4X12) || defined(CONFIG_CPU_EXYNOS5250) || defined(CONFIG_CPU_EXYNOS3250)
#define PART_SIZE_TZSW		(156 * 1024)
#elif defined(CONFIG_CPU_EXYNOS3472)
#define PART_SIZE_TZSW		(208 * 1024)
#else
#define PART_SIZE_TZSW		(256 * 1024)
#endif

#define MOVI_BL1_BLKCNT	        (PART_SIZE_BL1 / MOVI_BLKSIZE)
#define MOVI_BL2_BLKCNT		(PART_SIZE_BL2 / MOVI_BLKSIZE)
#define MOVI_ENV_BLKCNT		(CONFIG_ENV_SIZE / MOVI_BLKSIZE)	/* 16KB */
#define MOVI_UBOOT_BLKCNT	(PART_SIZE_UBOOT / MOVI_BLKSIZE)	/* 328KB */
#define MOVI_ZIMAGE_BLKCNT	(PART_SIZE_KERNEL / MOVI_BLKSIZE)	/* 4MB */
#define MOVI_ROOTFS_BLKCNT	(PART_SIZE_ROOTFS / MOVI_BLKSIZE)	/* 26MB */
#define MOVI_TZSW_BLKCNT	(PART_SIZE_TZSW / MOVI_BLKSIZE)		/* 160KB */

#define MOVI_UBOOT_POS		((eFUSE_SIZE / MOVI_BLKSIZE) + MOVI_BL1_BLKCNT + MOVI_BL2_BLKCNT)
#define MOVI_TZSW_POS           ((eFUSE_SIZE / MOVI_BLKSIZE) + MOVI_BL1_BLKCNT \
                                  + MOVI_BL2_BLKCNT + MOVI_UBOOT_BLKCNT)
#define MOVI_ENV_POS            (MOVI_TZSW_POS + MOVI_TZSW_BLKCNT)

#endif /* CONFIG_ARCH_EXYNOS7 */

#ifndef __ASSEMBLY__
#include <asm/io.h>

/*
 *
 * start_blk: start block number for image
 * used_blk: blocks occupied by image
 * size: image size in bytes
 * attribute: attributes of image
 *            0x0: BL1
 *            0x1: u-boot parted (BL2)
 *            0x2: u-boot
 *            0x4: kernel
 *            0x8: root file system
 *            0x10: environment area
 *            0x20: reserved
 * description: description for image
 * by scsuh
 */
typedef struct member {
	uint start_blk;
	uint used_blk;
	uint size;
	uint attribute; /* attribute of image */
	char description[16];
} member_t; /* 32 bytes */

/*
 * start_blk: start block number for raw area
 * total_blk: total block number of card
 * next_raw_area: add next raw_area structure
 * description: description for raw_area
 * image: several image that is controlled by raw_area structure
 * by scsuh
 */
typedef struct raw_area {
	uint start_blk; /* compare with PT on coherency test */
	uint total_blk;
	uint next_raw_area; /* should be sector number */
	char description[16];
	member_t image[MAX_PARTITION];
} raw_area_t; /* 512 bytes */

extern raw_area_t raw_area_control;
#endif
#endif /*__MOVI_PARTITION_H__*/
