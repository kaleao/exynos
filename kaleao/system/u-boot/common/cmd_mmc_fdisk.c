/*
 * Copyright (c) 2010 Samsung Electronics Co., Ltd.
 *              http://www.samsung.com/
 *
 * fdisk command for U-boot
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <common.h>
#include <command.h>
#include <mmc.h>

static unsigned long gblock_size = 512;
#define		BLOCK_END			0xFFFFFFFF
#define		_10MB				(10*1024*1024)
#define		_100MB				(100*1024*1024)
#define		_8_4GB				(1023*254*63)

#define		SYSTEM_PART_SIZE		(800)
#define		USER_DATA_PART_SIZE		(2000)
#define		CACHE_PART_SIZE			(800)

#define		CHS_MODE			0
#define		LBA_MODE			!(CHS_MODE)

typedef struct
{
	int		C_start;
	int		H_start;
	int		S_start;

	int		C_end;
	int		H_end;
	int		S_end;

	int		available_block;
	int		unit;
	int		total_block_count;
	int		addr_mode;	// LBA_MODE or CHS_MODE
} SDInfo;

typedef struct
{
	unsigned char bootable;
	unsigned char partitionId;

	int		C_start;
	int		H_start;
	int		S_start;

	int		C_end;
	int		H_end;
	int		S_end;

	int		block_start;
	int		block_count;
	int		block_end;
} PartitionInfo;

/////////////////////////////////////////////////////////////////
int calc_unit(unsigned long long length, SDInfo sdInfo)
{
	if (sdInfo.addr_mode == CHS_MODE)
		return ( (length / gblock_size / sdInfo.unit + 1 ) * sdInfo.unit);
	else
		return ( (length / gblock_size) );
}

/////////////////////////////////////////////////////////////////
void encode_chs(int C, int H, int S, unsigned char *result)
{
	*result++ = (unsigned char) H;
	*result++ = (unsigned char) ( S + ((C & 0x00000300) >> 2) );
	*result   = (unsigned char) (C & 0x000000FF);
}

/////////////////////////////////////////////////////////////////
void encode_partitionInfo(PartitionInfo partInfo, unsigned char *result)
{
	*result++ = partInfo.bootable;

	encode_chs(partInfo.C_start, partInfo.H_start, partInfo.S_start, result);
	result +=3;
	*result++ = partInfo.partitionId;

	encode_chs(partInfo.C_end, partInfo.H_end, partInfo.S_end, result);
	result += 3;

	memcpy(result, (unsigned char *)&(partInfo.block_start), 4);
	result += 4;

	memcpy(result, (unsigned char *)&(partInfo.block_count), 4);
}

/////////////////////////////////////////////////////////////////
void decode_partitionInfo(unsigned char *in, PartitionInfo *partInfo)
{
	partInfo->bootable	= *in;
	partInfo->partitionId	= *(in + 4);

	memcpy((unsigned char *)&(partInfo->block_start), (in + 8), 4);
	memcpy((unsigned char *)&(partInfo->block_count), (in +12), 4);
}

/////////////////////////////////////////////////////////////////
void get_SDInfo(int block_count, SDInfo *sdInfo)
{
       int C, H, S;

        int C_max = 1023, H_max = 255, S_max = 63;
        int H_start = 1, S_start = 1;
        int diff_min = 0, diff = 0;

        if(block_count >= _8_4GB)
                sdInfo->addr_mode = LBA_MODE;
        else
                sdInfo->addr_mode = CHS_MODE;

//-----------------------------------------------------
        if (sdInfo->addr_mode == CHS_MODE)
        {
                diff_min = C_max;

                for (H = H_start; H <= H_max; H++)
                        for (S  = S_start; S <= S_max; S++)
                        {
                                C = block_count / (H * S);

                                if ( (C <= C_max) )
                                {
                                        diff = C_max - C;
                                        if (diff <= diff_min)
                                        {
                                                diff_min = diff;
                                                sdInfo->C_end = C;
                                                sdInfo->H_end = H;
                                                sdInfo->S_end = S;
                                        }
                                }
                        }
        }
//-----------------------------------------------------
        else
        {
                sdInfo->C_end = 1023;
                sdInfo->H_end = 254;
                sdInfo->S_end = 63;
        }

//-----------------------------------------------------
        sdInfo->C_start                 = 0;
        sdInfo->H_start                 = 1;
        sdInfo->S_start                 = 1;

        sdInfo->total_block_count       = block_count;
        sdInfo->available_block         = sdInfo->C_end * sdInfo->H_end * sdInfo->S_end;
        sdInfo->unit                    = sdInfo->H_end * sdInfo->S_end;
}

/////////////////////////////////////////////////////////////////
void make_partitionInfo(int LBA_start, int count, SDInfo sdInfo, PartitionInfo *partInfo)
{
        int             temp = 0;
        int             _10MB_unit;

        partInfo->block_start   = LBA_start;

//-----------------------------------------------------
        if (sdInfo.addr_mode == CHS_MODE)
        {
                partInfo->C_start       = partInfo->block_start / (sdInfo.H_end * sdInfo.S_end);
                temp                    = partInfo->block_start % (sdInfo.H_end * sdInfo.S_end);
                partInfo->H_start       = temp / sdInfo.S_end;
                partInfo->S_start       = temp % sdInfo.S_end + 1;

                if (count == BLOCK_END)
                {
                        _10MB_unit = calc_unit(CFG_PARTITION_START, sdInfo);
                        partInfo->block_end     = sdInfo.C_end * sdInfo.H_end * sdInfo.S_end - _10MB_unit - 1;
                        partInfo->block_count   = partInfo->block_end - partInfo->block_start + 1;

                        partInfo->C_end = partInfo->block_end / sdInfo.unit;
                        partInfo->H_end = sdInfo.H_end - 1;
                        partInfo->S_end = sdInfo.S_end;
                }
                else
                {
                        partInfo->block_count   = count;

                        partInfo->block_end     = partInfo->block_start + count - 1;
                        partInfo->C_end         = partInfo->block_end / sdInfo.unit;

                        temp                    = partInfo->block_end % sdInfo.unit;
                        partInfo->H_end         = temp / sdInfo.S_end;
                        partInfo->S_end         = temp % sdInfo.S_end + 1;
                }
        }
//-----------------------------------------------------
        else
        {
                partInfo->C_start       = 0;
                partInfo->H_start       = 1;
                partInfo->S_start       = 1;

                partInfo->C_end         = 1023;
                partInfo->H_end         = 254;
                partInfo->S_end         = 63;

                if (count == BLOCK_END)
                {
                        _10MB_unit = calc_unit(CFG_PARTITION_START, sdInfo);
                        partInfo->block_end     = sdInfo.total_block_count - _10MB_unit - 1;
                        partInfo->block_count   = partInfo->block_end - partInfo->block_start + 1;

                }
                else
                {
                        partInfo->block_count   = count;
                        partInfo->block_end     = partInfo->block_start + count - 1;
                }
        }
}

/////////////////////////////////////////////////////////////////
int make_mmc_partition(int total_block_count, unsigned char *mbr, int sys_part, int user_part, int cache_part)
{
	int		block_start = 0, block_offset;

	SDInfo		sdInfo;
	PartitionInfo	partInfo[4];

///////////////////////////////////////////////////////////
	memset((unsigned char *)&sdInfo, 0x00, sizeof(SDInfo));

///////////////////////////////////////////////////////////
	get_SDInfo(total_block_count, &sdInfo);

///////////////////////////////////////////////////////////
// 반드시 Unit단위로 먼저 계산한다.
	block_start = calc_unit(CFG_PARTITION_START, sdInfo);
	block_offset = calc_unit(((unsigned long long)sys_part)*1024*1024, sdInfo);

	printf("Part : start=%d, offset=%d %d\n", block_start, block_offset);

	partInfo[0].bootable	= 0x00;
	partInfo[0].partitionId	= 0x83;

	make_partitionInfo(block_start, block_offset, sdInfo, &partInfo[0]);

///////////////////////////////////////////////////////////
	block_start += block_offset;
	block_offset = calc_unit(((unsigned long long)user_part)*1024*1024, sdInfo);

	printf("Part : start=%d, offset=%d %d\n", block_start, block_offset);

	partInfo[1].bootable	= 0x00;
	partInfo[1].partitionId	= 0x83;

	make_partitionInfo(block_start, block_offset, sdInfo, &partInfo[1]);

///////////////////////////////////////////////////////////
	block_start += block_offset;
	block_offset = calc_unit(((unsigned long long)cache_part)*1024*1024, sdInfo);

	printf("Part : start=%d, offset=%d %d\n", block_start, block_offset);

	partInfo[2].bootable	= 0x00;
	partInfo[2].partitionId	= 0x83;

	make_partitionInfo(block_start, block_offset, sdInfo, &partInfo[2]);

///////////////////////////////////////////////////////////
	block_start += block_offset;
	block_offset = BLOCK_END;

	partInfo[3].bootable	= 0x00;
	partInfo[3].partitionId	= 0x0C;

	make_partitionInfo(block_start, block_offset, sdInfo, &partInfo[3]);

///////////////////////////////////////////////////////////
	memset(mbr, 0x00, sizeof(mbr));
	mbr[510] = 0x55; mbr[511] = 0xAA;

	encode_partitionInfo(partInfo[0], &mbr[0x1CE]);
	encode_partitionInfo(partInfo[1], &mbr[0x1DE]);
	encode_partitionInfo(partInfo[2], &mbr[0x1EE]);
	encode_partitionInfo(partInfo[3], &mbr[0x1BE]);

	return CMD_RET_SUCCESS;
}

int get_mmc_part_info(block_dev_desc_t *dev_desc, int dev, int part_num, int *block_start, int *block_count, unsigned char *part_Id)
{
	struct mmc *mmc;

	if (!dev_desc || !block_start || !block_count || !part_Id) {
		printf("mmc/sd partition argument invalid.\n");
		return -1;
	}

	if (dev_desc->part_type == PART_TYPE_DOS) {
		return get_mmc_mbr_part_info(dev_desc, dev, part_num, block_start, block_count, part_Id);
	}
#ifdef CONFIG_EFI_PARTITION
	else if (dev_desc->part_type == PART_TYPE_EFI) {
		return get_mmc_gpt_part_info(dev_desc, dev, part_num, block_start, block_count, part_Id);
	}
#endif
	else {
		printf("mmc/sd unknown partition type : 0x%x.\n", dev_desc->part_type);
		return -1;
	}
}

#ifdef CONFIG_EFI_PARTITION
/////////////////////////////////////////////////////////////////
int get_mmc_block_count(char *device_name)
{
	int rv;
	struct mmc *mmc;
	int block_count = 0;
	int dev_num;

	dev_num = simple_strtoul(device_name, NULL, 0);

	mmc = find_mmc_device(dev_num);
	if (!mmc)
	{
		printf("mmc/sd device is NOT founded.\n");
		return -1;
	}

	block_count = mmc->capacity / mmc->read_bl_len;

	return block_count;
}
int get_mmc_gpt_part_info(block_dev_desc_t *blkdev, int dev, int part_num, int *block_start, int *block_count, unsigned char *part_type)
{
	disk_partition_t info;

	if (!blkdev || !block_start || !block_count || !part_type) {
		printf("%s : argument invalid\n", __func__);
	}

	if (get_partition_info_efi(blkdev, part_num, &info))
	{
		printf("mmc/sd failed to get partition info.\n");
		return -1;
	}

	*block_start = info.start;
	*block_count = info.size;
	*part_type = 0xee; /* this is efi part type for mbr */

	return 0;
}
#endif

/////////////////////////////////////////////////////////////////
/* This function return block start address and count by 512 block size unit */
int get_mmc_mbr_part_info(block_dev_desc_t *dev_desc, int dev, int part_num, int *block_start, int *block_count, unsigned char *part_Id)
{
	int rv;
	PartitionInfo	partInfo;
#if defined(CONFIG_MMC_64BIT_BUS) || defined(CONFIG_CPU_EXYNOS5410_EVT2)
	ALLOC_CACHE_ALIGN_BUFFER(unsigned char, mbr, 512);
#else
	unsigned char	mbr[512];
#endif

	if ( part_num>4 || part_num<0 )
		return 0;

	rv = dev_desc->block_read(dev, 0, 1, mbr);
	if(!rv)
		return CMD_RET_FAILURE;

	decode_partitionInfo(&mbr[0x1AE + 0x10*part_num], &partInfo);
			*block_start	= partInfo.block_start * (dev_desc->blksz/512);
			*block_count	= partInfo.block_count * (dev_desc->blksz/512);
			*part_Id 	= partInfo.partitionId;

	return CMD_RET_SUCCESS;
}

int get_boot_part_info(int dev, int part_num, unsigned int *block_start, unsigned int *block_count, unsigned char *part_Id)
{
	block_dev_desc_t *dev_desc;
	char if_name[32]="mmc";

#ifdef CONFIG_SCSI_UFS
	if ( exynos_boot_mode()==BOOT_UFS ) {
		strncpy(if_name, "scsi", 8);
	}
#endif
	dev_desc=get_dev(if_name,dev);
	return get_mmc_part_info(dev_desc, dev, part_num, block_start, block_count, part_Id);
}

void print_mmc_blk(unsigned char* buf)
{
    int i;

    for (i = 1; i <= 512; i++) {
        printf("0x%02x ", buf[i-1]);
        if (i % 16 == 0) {
            printf("\n");
        }
    }
}

int dump_mmc(unsigned long start_blk, int end_blk)
{
    struct mmc* mmc;
    lbaint_t blkcnt;
    int rv;
    unsigned char buf[512] = {0,};

    mmc = find_mmc_device(0);
    if (!mmc)
    {
		printf("mmc/sd device is NOT founded.\n");
		return -1;
    }

    blkcnt = end_blk - start_blk + 1;

    while (blkcnt--)
    {
        rv = mmc->block_dev.block_read(0, start_blk++, 1, buf);

        if(rv != 1)
        {
            printf("mmc/sd device read fail.\n");
            return -1;
        }
        print_mmc_blk(buf);
    }

    return 0;
}

/////////////////////////////////////////////////////////////////
int print_mmc_part_info(block_dev_desc_t *dev_desc, int dev)
{
	int		i;

	PartitionInfo	partInfo[4];

	if (!dev_desc->blksz)
		return CMD_RET_FAILURE;

	for ( i=0; i<4; i++) {
		get_mmc_part_info(dev_desc, dev, i+1, &(partInfo[i].block_start),
				&(partInfo[i].block_count), &(partInfo[i].partitionId) );
	}

	printf("\n");
	printf("partion #    size(MB)     block start #    block count    partition_Id (block size=%d)\n", dev_desc->blksz);
	for ( i=0; i<4; i++) {
		if ( (partInfo[i].block_start !=0) && (partInfo[i].block_count != 0) )
			printf("   %d        %6d         %8d        %8d          0x%02X\n",
				i+1, (partInfo[i].block_count /2048),
				partInfo[i].block_start, partInfo[i].block_count, partInfo[i].partitionId);
	}

	return CMD_RET_SUCCESS;
}

/////////////////////////////////////////////////////////////////
int create_mmc_fdisk(block_dev_desc_t *dev_desc, int dev, unsigned int sys_part, unsigned int user_part, unsigned int cache_part)
{
	int		rv;
#if defined(CONFIG_MMC_64BIT_BUS) || defined(CONFIG_CPU_EXYNOS5410_EVT2)
	ALLOC_CACHE_ALIGN_BUFFER(unsigned char, mbr, 512);
#else
	unsigned char	mbr[512];
#endif

	if (!dev_desc)
		return CMD_RET_FAILURE;

	if ((((dev_desc->lba/1024)*dev_desc->blksz)/1024)<(sys_part+user_part+cache_part)) {
		printf("Partition size too small (%d < %d + %d + %d)\n", (((dev_desc->lba/1024)*dev_desc->blksz)/1024), sys_part, user_part, cache_part);
		return CMD_RET_FAILURE;
	}


	memset(mbr, 0x00, 512);

	printf("device LBA = %d (blsize %d), %d %d %d\n", dev_desc->lba, dev_desc->blksz, sys_part, user_part, cache_part);

	gblock_size = dev_desc->blksz;

	make_mmc_partition(dev_desc->lba, mbr, sys_part, user_part, cache_part);

	rv = dev_desc->block_write(dev, 0, 1, mbr);
	if (!rv)
		return CMD_RET_FAILURE;

	printf("fdisk is completed\n");

	print_mmc_part_info(dev_desc, dev);
	return CMD_RET_SUCCESS;
}

/////////////////////////////////////////////////////////////////
#define PART_CREATE	1
#define PART_PRINT	2

void set_boot_env(int bootlun, int userlun)
{
#ifdef CONFIG_SCSI_UFS
	char userlun_num[32];
	char bootlun_num[32];

	sprintf(userlun_num, "%u", userlun);
	setenv("lunUser", userlun_num);
	sprintf(bootlun_num, "%u", bootlun);
	setenv("lunBoot", bootlun_num);

	if (exynos_boot_mode() == BOOT_UFS) {
		char bootcmd[150];
		sprintf(bootcmd, CONFIG_BOOTCOMMAND_UFS, userlun, userlun);
		setenv("bootcmd", bootcmd);
	}
	saveenv();
#endif
}

int do_fdisk(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int dev, cmd=0;
	unsigned int sys_part, user_part, cache_part;
	char if_name[32]="mmc";
	block_dev_desc_t	*dev_desc;
	int ret;

	if (argc<3)
		goto err_out;

		if ( strcmp(argv[1], "-c") == 0 )
		cmd = PART_CREATE;
	if ( strcmp(argv[1], "-p") == 0 )
		cmd = PART_PRINT;
	if ( !cmd )
		goto err_out;


	switch (argc) {
	case 4:
		strncpy(if_name, argv[2], 8);
	case 3:
		dev = simple_strtoul(argv[argc-1], NULL, 10);
		dev_desc=get_dev(if_name,dev);
		if (!dev_desc)
			goto err_out;

		printf("%s %s %d partition\n", (cmd==PART_PRINT?"print":"create"), if_name, dev);

		if (cmd==PART_PRINT) {
			return print_mmc_part_info(dev_desc, dev);
		} else if (cmd==PART_CREATE) {
			ret = create_mmc_fdisk(dev_desc, dev,SYSTEM_PART_SIZE,
					USER_DATA_PART_SIZE, CACHE_PART_SIZE);
			return ret;

		}
		break;
	case 7:
		strncpy(if_name, argv[2], 8);
	case 6:
		if (cmd!=PART_CREATE)
			goto err_out;

		dev = simple_strtoul(argv[argc-4], NULL, 10);
		dev_desc=get_dev(if_name,dev);
		if (!dev_desc)
			goto err_out;

		sys_part = simple_strtoul(argv[argc-3], NULL, 10);
		user_part = simple_strtoul(argv[argc-2], NULL, 10);
		cache_part = simple_strtoul(argv[argc-1], NULL, 10);

		printf("%s %s %d partition %d, %d, %d\n",(cmd==PART_PRINT?"print":"create"), if_name, dev,
				sys_part, user_part, cache_part);

		ret = create_mmc_fdisk(dev_desc, dev, sys_part, user_part, cache_part);
		return ret;
		break;
	}
err_out:
	return CMD_RET_USAGE;
}

U_BOOT_CMD(
	fdisk, 7, 0, do_fdisk,
	"fdisk for sd/mmc.",
	"-p <interface> <device_num> - print partition information\n"
	"fdisk -c <interface> <device_num> [<sys. part size(MB)> <user part size> <cache part size>] - create partition.\n"
);

