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
#include <mmc.h>
#include <asm/arch/movi_partition.h>
#include <asm/arch/cpu.h>

#ifndef CONFIG_SPL_BUILD
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
	int partition_num = 0;
	static int first_time = 1;
	static int location_before = 0;
#ifdef CONFIG_EFI_PARTITION
	 disk_partition_t info;
	 int tcount = 0;
	 int table_count;
	 block_dev_desc_t *dev_desc=NULL;
	 if (location & SCSI_TAG)
	 {
		 dev_desc=get_dev("scsi", get_userLUN());
	 }
	 else {
		 dev_desc=get_dev("mmc", 0);
	 }
#endif
	if((location_before != location) || (location & FORCE_UPDATE))
		first_time = 1;
	if(!first_time)
		return 0;
	if(!(location & IN_BOOT))
		first_time = 0;
	location_before = location;
	/* init raw_area will be 16MB */
	raw_area_control.start_blk = 16*1024*1024/MOVI_BLKSIZE;
	raw_area_control.next_raw_area = 0;
	strcpy(raw_area_control.description, "initial raw table");

	image = raw_area_control.image;


	/* For eMMC partition BLOCK Change*/
#if  0//CONFIG_COMPLETE_UBOOT_FILE /* yujing 2012-10-29*/
	 image[0].start_blk = (location & 1) ? 1 : 0;
	 image[0].used_blk = MOVI_BOOTLOADER_BLKCNT;
	 image[0].size = PART_SIZE_BOOTLOADER;
	 image[0].attribute = 0x27;
	 strcpy(image[0].description, "bootloader");
	 printf("bootloader: %d  \n", image[0].start_blk);
	 partition_num++;
#else
	/* image 0 should be bl1 */
	image[0].start_blk = (location == 1) ? 1 : 0;
	image[0].used_blk = MOVI_BL1_BLKCNT;
	image[0].size = PART_SIZE_BL1;
	image[0].attribute = 0x0;
	strcpy(image[0].description, "fwbl1");
	dbg("fwbl1: %d\n", image[0].start_blk);
	partition_num++;
	
	/* image 1 should be secure monitor */
	image[1].start_blk = image[0].start_blk + MOVI_BL1_BLKCNT;
	image[1].used_blk = MOVI_EL3_MON_BLKCNT;
	image[1].size = PART_SIZE_EL3_MON;
	image[1].attribute = 0x1;
	strcpy(image[1].description, "el3_mon");
	dbg("el3_mon: %d\n", image[1].start_blk);
	partition_num++;
	
	/* image 2 should be bl2 */
	image[2].start_blk = image[1].start_blk + MOVI_EL3_MON_BLKCNT;
	image[2].used_blk = MOVI_BL2_BLKCNT;
	image[2].size = PART_SIZE_BL2;
	image[2].attribute = 0x2;
	strcpy(image[2].description, "bl2");
	dbg("bl2: %d\n", image[2].start_blk);
	partition_num++;
	
	/* image 3 should be u-boot */
	image[3].start_blk = image[2].start_blk + MOVI_BL2_BLKCNT;
	image[3].used_blk = MOVI_UBOOT_BLKCNT;
	image[3].size = PART_SIZE_UBOOT;
	image[3].attribute = 0x3;
	strcpy(image[3].description, "bootloader");
	dbg("u-boot: %d\n", image[3].start_blk);
	partition_num++;
	
	/* image 4 should be TrustZone S/W */
	image[4].start_blk = image[3].start_blk + MOVI_UBOOT_BLKCNT;
	image[4].used_blk = MOVI_MOBICORE_BLKCNT;
	image[4].size = PART_SIZE_MOBICORE;
	image[4].attribute = 0x4;
	strcpy(image[4].description, "tzsw");
	dbg("TrustZone S/W: %d\n", image[4].start_blk);
	partition_num++;

	image[5].start_blk = (location & 1) ? 1 : 0;
	image[5].used_blk = MOVI_BOOTLOADER_BLKCNT;
	image[5].size = PART_SIZE_BOOTLOADER;
	image[5].attribute = 0x27;
	strcpy(image[5].description, "set");
	dbg("bootloader: %d	\n", image[5].start_blk);
	partition_num++;

	image[6].start_blk = MOVI_ENV_POS;
	image[6].used_blk = MOVI_ENV_BLKCNT;
	image[6].size = CONFIG_ENV_SIZE;
	image[6].attribute = 0x5;
	strcpy(image[6].description, "environment");
	dbg("environment: %d	\n", image[6].start_blk);
	partition_num++;
#endif
#ifdef CONFIG_EFI_PARTITION 
	/* image 4 should be TrustZone S/W */
	image[partition_num].start_blk = CONFIG_PARTITION_START/MOVI_BLKSIZE;
	image[partition_num].used_blk = CONFIG_PRI_GPT_SIZE/MOVI_BLKSIZE;
	image[partition_num].size = CONFIG_PRI_GPT_SIZE;
	strcpy(image[partition_num].description, "partition_table");
	dbg("partition_table : %d %d \n", image[partition_num].start_blk,partition_num);
	partition_num++;
	if(!(location & IN_BOOT))
		for(i=1;i<MAX_PARTITION;i++)
		{
			if (get_partition_info(dev_desc, i, &info) !=0)
				break;
			strcpy(image[partition_num].description, info.name);
			image[partition_num].start_blk = info.start;
			image[partition_num].used_blk = info.size ;
			image[partition_num].size = (unsigned long long) (info.size)*MOVI_BLKSIZE;
			partition_num++;
		}
#else
	/* image 5 should be environment */
	image[partition_num].start_blk = image[partition_num-1].start_blk + image[partition_num-1].used_blk;
	image[partition_num].used_blk = MOVI_ENV_BLKCNT;
	image[partition_num].size = CONFIG_ENV_SIZE;
	image[partition_num].attribute = 0x5;
	strcpy(image[partition_num].description, "environment");
	dbg("env: %d\n", image[partition_num].start_blk);
	partition_num++;


	/* For eMMC partition BLOCK Change*/
	if (location == 0)
		image[partition_num-1].start_blk = image[partition_num-1].start_blk + 1;

	/* image 6 should be kernel */
	image[partition_num].start_blk = image[partition_num-1].start_blk + MOVI_ENV_BLKCNT;
	image[partition_num].used_blk = MOVI_KERNEL_BLKCNT;
	image[partition_num].size = PART_SIZE_KERNEL;
	image[partition_num].attribute = 0x6;
	strcpy(image[partition_num].description, "kernel");
	dbg("knl: %d\n", image[partition_num].start_blk);
	partition_num++;

	/* image 7 should be RFS */
	image[partition_num].start_blk = image[partition_num-1].start_blk + MOVI_KERNEL_BLKCNT;
	image[partition_num].used_blk = MOVI_ROOTFS_BLKCNT;
	image[partition_num].size = PART_SIZE_ROOTFS;
	image[partition_num].attribute = 0x7;
	strcpy(image[partition_num].description, "rfs");
	dbg("rfs: %d\n", image[partition_num].start_blk);
	partition_num++;

	/* image 8 should be DTB */
	image[partition_num].start_blk = image[partition_num-1].start_blk + MOVI_ROOTFS_BLKCNT;
	image[partition_num].used_blk = MOVI_DTB_BLKCNT;
	image[partition_num].size = PART_SIZE_DTB;
	image[partition_num].attribute = 0x8;
	strcpy(image[partition_num].description, "dtb");
	dbg("dtb: %d\n", image[partition_num].start_blk);
	partition_num++;

	/* image 9 should be Loadable F/W */
	image[partition_num].start_blk = image[partition_num-1].start_blk + MOVI_DTB_BLKCNT;
	image[partition_num].used_blk = MOVI_LOADABLE_FW_BLKCNT;
	image[partition_num].size = PART_SIZE_LOADABLE_FW;
	image[partition_num].attribute = 0xa;
	strcpy(image[partition_num].description, "ldfw");
	dbg("Loadable F/W: %d\n", image[partition_num].start_blk);
	partition_num++;

	for (i=partition_num; i<MAX_PARTITION; i++) {
		raw_area_control.image[i].start_blk = 0;
		raw_area_control.image[i].used_blk = 0;
	}
#endif
	return 0;
part_type_error:
	printf("Error: No MBR is found at SD/MMC.\n");
	printf("Hint: please fuse primary&second gpt (GUID parition table) first********\n");
	return 1;
}
#endif /* CONFIG_SPL_BUILD */
int get_raw_area_info(char *name, unsigned long long *start, unsigned long long *size)
{
	int	i;
	member_t	*image;
	image = raw_area_control.image;
	for (i=0; i<MAX_PARTITION; i++) {
		if (!strcmp(image[i].description, name))
			break;
	}
	if (i == MAX_PARTITION)
		return -1;
	/* start and size will be zero
	* if there is no partition named $name.
	* becasue the image[15].size/start is zero
	*/
	*start = (unsigned long long)image[i].start_blk * (unsigned int)MOVI_BLKSIZE;
	*size = image[i].size;
	return (i == MAX_PARTITION) ? -1 : i;
}
