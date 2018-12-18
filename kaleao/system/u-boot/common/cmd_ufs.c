/*
 * (C) Copyright 2013 SAMSUNG Electronics
 * Mingyu Lee <coollmg.lee@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * UFS specific special functions
 */

 /*
  * SCSI support.
  */
#include <common.h>
#include <command.h>
#include <asm/processor.h>
#include <scsi.h>
#include <image.h>
#include <ufs.h>

static ccb tempccb;		/* temporary scsi command buffer */
static unsigned char data_format_query_request[512]
    __attribute__ ((aligned(ARCH_DMA_MINALIGN))) = {
0x02, 0x00, 0x00, 0x00, 0x4a, 0xe1, 0x3d, 0x6c,
	    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	    0x01, 0xc8, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00,};

static unsigned char data_configuration_desc[256]
    __attribute__ ((aligned(ARCH_DMA_MINALIGN))) = {
0x90, 0x01, 0x00, 0x01, 0x01, 0x01, 0x03, 0x03,
	    0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00,
	    0x00, 0x0C, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
	    0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	    0x00, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
	    0x00, 0x0C, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
	    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
	    0x00, 0x0C, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
	    0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x08, 0xC0,
	    0x01, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	    0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
	    0x00, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
	    0x00, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	    0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	    0x00, 0x0C, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,};

static unsigned int y = 1U;

static unsigned int my_rand_r(unsigned int *seedp)
{
	*seedp ^= (*seedp << 13);
	*seedp ^= (*seedp >> 17);
	*seedp ^= (*seedp << 5);

	return *seedp;
}

static unsigned int my_rand(void)
{
	return my_rand_r(&y);
}

static void my_srand(unsigned int seed)
{
	y = seed;
}

void scsi_write_read_test(int device, ulong blknr, ulong blkcnt, void *buf, int mode)
{
	ulong i, r, res, start;
	u8 *buffer = buf;

	switch (mode) {
	case 0:
		printf("UFS write / read / compare test\n");
		r = (get_timer(0) & 0xff) << 24;
		printf("\nscsi_write pattern: buffer %lx, length %lx\n", (unsigned long)buffer,
		       UFS_LOGICAL_BLOCK_SIZE * blkcnt);
		for (i = 0; i < UFS_LOGICAL_BLOCK_SIZE * blkcnt; i += 4)
			*(u32 *) (buffer + i) = i | r;
		scsi_write(device, blknr, blkcnt, buffer);
		printf("clear buffer & scsi_read: buffer %lx, length %lx\n", (unsigned long)buffer,
		       UFS_LOGICAL_BLOCK_SIZE * blkcnt);
		memset(buffer, 0x00, UFS_LOGICAL_BLOCK_SIZE * blkcnt);
		scsi_read(device, blknr, blkcnt, buffer);
		printf("check buffer: buffer %lx, length %lx\n", (unsigned long)buffer,
		       UFS_LOGICAL_BLOCK_SIZE * blkcnt);
		for (i = 0; i < UFS_LOGICAL_BLOCK_SIZE * blkcnt; i += 4) {
			if (*(u32 *) (buffer + i) != (i | r)) {
				printf("Read Data Offset 0x%08x : 0x%08x != 0x%08x\n", buffer + i,
				       *(u32 *) (buffer + i), i | r);
				break;
			}
		}
		printf("check done\n");
		break;
	case 1:
		printf("UFS read command looping test\n");
		i = 0;
		r = 1;
		while (r) {
			i++;
			res = scsi_read(device, blknr, blkcnt, buffer);
			if (res != blkcnt) {
				printf("%d read try FAIL (%d block read)\n", i, res);
				r = 0;
			}
			if (!(i % 4096))
				printf("%d read done.\n", i);
		}
		break;
	case 2:
		printf("UFS read/write looping test\n");
		i = 0;
		r = 1;
		while (r) {
			i++;
			res = scsi_read(device, blknr, blkcnt, buffer);
			if (res != blkcnt) {
				printf("%d read try FAIL (%d block read)\n", i, res);
				r = 0;
			}
			res = scsi_write(device, blknr, blkcnt, buffer);
			if (res != blkcnt) {
				printf("%d write try FAIL (%d block write)\n", i, res);
				r = 0;
			}
			if (!(i % 4096))
				printf("%d read/write done.\n", i);
		}
		break;
	case 3:
		printf("UFS random read/write looping test\n");
		i = 0;
		r = 1;
		my_srand(get_timer(0));
		while (r) {
			i++;
			start = my_rand();
			if (start > blknr)
				start = start % blknr;
			if (my_rand() & 1) {
				res = scsi_read(device, start * 8, blkcnt, buffer);
				if (res != blkcnt) {
					printf("%d read try FAIL (%d block read)\n", i, res);
					r = 0;
				}
			} else {
				res = scsi_write(device, start * 8, blkcnt, buffer);
				if (res != blkcnt) {
					printf("%d write try FAIL (%d block write)\n", i, res);
					r = 0;
				}
			}
			if (!(i % 4096))
				printf("%d read or write done.\n", i);
		}
		break;
	case 4:
		printf("UFS random read looping test\n");
		i = 0;
		r = 1;
		my_srand(get_timer(0));
		while (r) {
			i++;
			start = my_rand();
			if (start > blknr)
				start = start % blknr;
			res = scsi_read(device, start * 8, blkcnt, buffer);
			if (res != blkcnt) {
				printf("%d read try FAIL (%d block read)\n", i, res);
				r = 0;
			}
			if (!(i % 4096))
				printf("%d read done.\n", i);
		}
		break;
	case 10:
		printf("get_timer test\n");
		for (i = 0; i < 100; i++) {
			r = get_timer(0);
			while (get_timer(0) - r < 2000000) ;
			printf("%d\n", i);
		}
		break;
	default:
		break;
	}
}

static int ufs_power_on_reset(struct ufs_host *ufs)
{
	ufs->ufs_hw_reset(ufs, 0);
	ufs->ufs_device_power(ufs, 0);
	udelay(2000);

	return 0;
}

static int ufs_initialization(struct ufs_host *ufs)
{
	u32 reg;
	printf("UFS cmd_ufs.c : ufs_initialization()\n");
	/* VS_SW_RST */
	writel(1, (ufs->ioaddr + VS_SW_RST));
	/* VENDOR_SPECIFIC_IS[20] : clear UFS_IDLE_Indicator bit (if UFS_LINK is reset, this bit is asserted) */
	writel(1 << 20, (ufs->ioaddr + VS_IS));
	/* Set Unipro PCLK Divide value to 1 */
	writel((1 << 4), (ufs->ioaddr + VS_UNIPRO_APB_CLK_CTRL));

	reg = readl(ufs->ioaddr + UFSP_UPRSECURITY);
	writel(ufs->ioaddr + UFSP_UPRSECURITY, reg & (~0x00020012));
	reg = readl(ufs->ioaddr + UFSP_UPWSECURITY);
	writel(ufs->ioaddr + UFSP_UPWSECURITY, reg & (~0x00000012));

	memset(ufs->cmd_desc_addr, 0x00, UFS_NUTRS * sizeof(struct ufs_cmd_desc));
	memset(ufs->utrd_addr, 0x00, UFS_NUTRS * sizeof(struct ufs_utrd));
	memset(ufs->utmrd_addr, 0x00, UFS_NUTMRS * sizeof(struct ufs_utmrd));
	ufs->utrd_addr->cmd_desc_addr_l = cpu_to_le32(ufs->cmd_desc_addr);
	ufs->utrd_addr->rsp_upiu_off = cpu_to_le16(offsetof(struct ufs_cmd_desc, response_upiu));
	ufs->utrd_addr->rsp_upiu_len = cpu_to_le16(ALIGNED_UPIU_SIZE);

	writel(ufs->utmrd_addr, (ufs->ioaddr + REG_UTP_TASK_REQ_LIST_BASE_L));
	writel(0, (ufs->ioaddr + REG_UTP_TASK_REQ_LIST_BASE_H));

	writel(ufs->utrd_addr, (ufs->ioaddr + REG_UTP_TRANSFER_REQ_LIST_BASE_L));
	writel(0, (ufs->ioaddr + REG_UTP_TRANSFER_REQ_LIST_BASE_H));

#if 1
	ufs->utmrd_addr[0].dw[2] = cpu_to_le32(OCS_INVALID_COMMAND_STATUS);
	ufs->utmrd_addr[0].task_req_upiu.type = UPIU_TRANSACTION_TASK_REQ;
	ufs->utmrd_addr[0].task_req_upiu.flags = UPIU_CMD_FLAGS_NONE;
	ufs->utmrd_addr[0].task_req_upiu.lun = 0;
	ufs->utmrd_addr[0].task_req_upiu.tag = 0;
	ufs->utmrd_addr[0].task_req_upiu.function = UFS_QUERY_TASK;
	ufs->utmrd_addr[0].task_req_upiu.tsf[0] = 0;
	ufs->utmrd_addr[0].task_req_upiu.tsf[1] = 0;

	ufs->utmrd_addr[1].dw[2] = cpu_to_le32(OCS_INVALID_COMMAND_STATUS);
	ufs->utmrd_addr[1].task_req_upiu.type = UPIU_TRANSACTION_TASK_REQ;
	ufs->utmrd_addr[1].task_req_upiu.flags = UPIU_CMD_FLAGS_NONE;
	ufs->utmrd_addr[1].task_req_upiu.lun = 0;
	ufs->utmrd_addr[1].task_req_upiu.tag = 1;
	ufs->utmrd_addr[1].task_req_upiu.function = UFS_QUERY_TASK;
	ufs->utmrd_addr[1].task_req_upiu.tsf[0] = 0;
	ufs->utmrd_addr[1].task_req_upiu.tsf[1] = 1;
#endif

	return ufs_link_startup(ufs);
}

static int ufs_req_sense(struct ufs_host *ufs)
{
	unsigned char *buf;

	ufs->data_seg_len = 0;
	buf = scsi_sense(0);

	printf("SenseData\n%02x %02x %02x %02x | %02x %02x %02x %02x\n"
	       "%02x %02x %02x %02x | %02x %02x %02x %02x\n%02x %02x\n", buf[0], buf[1], buf[2],
	       buf[3], buf[4], buf[5], buf[6], buf[7]
	       , buf[8], buf[9], buf[10], buf[11], buf[12], buf[13], buf[14], buf[15]
	       , buf[16], buf[17]);

	printf("SenseKey(%02x), inf(%02x %02x %02x %02x), ASC(%02x), ASCQ(%02x)\n", buf[2], buf[3],
	       buf[4], buf[5], buf[6], buf[12], buf[13]);

	if (buf[0] == 0x70 && buf[2] == 0x06) {
		printf("Clear Unit Attention condition\n");
		return 0;
	}

	return 1;
}

static int ufs_req_format(struct ufs_host *ufs)
{
	int res;
	u32 t;

	ufs->data_seg_len = 512;
	t = ufs->ufs_cmd_timeout;
	ufs->ufs_cmd_timeout = 0xffffffff;
	memcpy(ufs->upiu_data, data_format_query_request, ufs->data_seg_len);
	res = ufs_send_upiu(UFS_SEND_REQUEST_FORMAT_QUERY_REQ);
	ufs->ufs_cmd_timeout = t;
	if (res) {
		printf("UFS%d REQUEST_QUERY with format error!\n", ufs->host_index);
	}

	return res;
}

static int ufs_req_config(struct ufs_host *ufs)
{
	int res;

	ufs->data_seg_len = 0x90;
	memcpy(ufs->upiu_data, data_configuration_desc, ufs->data_seg_len);
	res = ufs_send_upiu(UFS_SEND_WRITE_CONFIG_DESC);
	ufs_send_upiu(UFS_SEND_WRITE_BOOTLUNEN_ATTR);

	if (res) {
		printf("UFS%d UFS_SEND_WRITE_CONFIG_DESC error!\n", ufs->host_index);
	}

	return res;
}

#define TORONTO_FIRMWARE_OFFSET01	0x00000200
#define TORONTO_FIRMWARE_OFFSET02	0x0005b000
#define TORONTO_FIRMWARE_OFFSET03	0x00000200
#define TORONTO_FIRMWARE_OFFSET04	0x00001000

int toshiba_firmware_update(struct ufs_host *ufs, int argc, char **argv)
{
	unsigned char *buf;
	unsigned int addr = simple_strtoul(argv[2], NULL, 16);
	int res;

	printf("UFS firmware update start\n");

	printf("Step 1 : Power on reset\n");
	ufs_power_on_reset(ufs);

	printf("Step 2 : Initialization\n");
	if (ufs_initialization(ufs))
		goto err_exit;

	printf("Step 3 : Request Sense\n");
	if (ufs_req_sense(ufs))
		goto err_exit;

	printf("Step 4 : Firmware Update (Authorization) from 0x%08x (len0x%08x)\n", addr,
	       TORONTO_FIRMWARE_OFFSET01);
	buf = scsi_write_buffer(0, 1, 0, 0, TORONTO_FIRMWARE_OFFSET01, addr);
	addr += TORONTO_FIRMWARE_OFFSET01;

	printf("Step 5 : Firmware Update (Transfer and Update Firmware) from 0x%08x (len0x%08x)\n",
	       addr, TORONTO_FIRMWARE_OFFSET02);
	buf = scsi_write_buffer(0, 5, 0, 0, TORONTO_FIRMWARE_OFFSET02, addr);
	addr += TORONTO_FIRMWARE_OFFSET02;

	printf("Step 6 : Power on reset\n");
	ufs_power_on_reset(ufs);

	printf("Step 7 : Initialization\n");
	if (ufs_initialization(ufs))
		goto err_exit;

	printf("Step 8 : Request Sense\n");
	if (ufs_req_sense(ufs))
		goto err_exit;

	printf("Step 9 : Write System Data (Authorization) from 0x%08x (len0x%08x)\n", addr,
	       TORONTO_FIRMWARE_OFFSET03);
	buf = scsi_write_buffer(0, 1, 0, 0, TORONTO_FIRMWARE_OFFSET03, addr);
	addr += TORONTO_FIRMWARE_OFFSET03;

	printf("Step 10 : Write System Data (Transfer data) from 0x%08x (len0x%08x)\n", addr,
	       TORONTO_FIRMWARE_OFFSET04);
	buf = scsi_write_buffer(0, 5, 0, 0, TORONTO_FIRMWARE_OFFSET04, addr);

	printf("Step 11 : Request format with QUERY REQUEST\n");
	res = ufs_req_format(ufs);

	printf("Step 12 : Power on reset\n");
	ufs_power_on_reset(ufs);

	printf("Step 13 : Initialization\n");
	if (ufs_initialization(ufs))
		goto err_exit;

	printf("Step 14 : Request Sense\n");
	if (ufs_req_sense(ufs))
		goto err_exit;

	printf("Step 15 : Write Configuration Descriptor\n");
	if (ufs_req_config(ufs))
		goto err_exit;

	return 0;

 err_exit:
	return 1;
}

int do_ufsupdate(cmd_tbl_t * cmdtp, int flag, int argc, char *const argv[])
{
	struct ufs_host *ufs = get_cur_ufs_host();

	if (argc < 3) {
		return CMD_RET_USAGE;
	}

	if (ufs == NULL) {
		printf("UFS device not found.\n");
		return 1;
	}

	if (ufs->ufs_device_power == NULL) {
		printf("UFS device power control not supported.\n");
		return 1;
	}

	switch (argv[1][0]) {
	case 't':
		return toshiba_firmware_update(ufs, argc, argv);
		break;
	default:
		break;
	}

	return CMD_RET_USAGE;
}

U_BOOT_CMD(ufs_update, 3, 1, do_ufsupdate,
	   "UFS device firmware update",
	   "- Update UFS firmware\n"
	   "ufs_update vendor[mem/toshiba] address - update UFS firmware with image in address\n");

int do_ufstest(cmd_tbl_t * cmdtp, int flag, int argc, char *const argv[])
{
	ulong dev;
	ulong addr;
	ulong blk;
	ulong cnt;
	ulong mode;
	struct ufs_host *ufs = get_cur_ufs_host();

	if (ufs == NULL) {
		printf("UFS device not found.\n");
		return 1;
	}

	switch (argv[1][0]) {
	case 'l':
		printf("link start function call\n");
		ufs_link_startup(ufs);
		//ufs_req_sense(ufs);
		return 0;
		break;
	case 'c':
		printf("Write Configuration Descriptor\n");
		ufs_req_sense(ufs);
		ufs_req_config(ufs);
		return 0;
		break;
	case '1':
		printf("Step 1 : Power on reset\n");
		ufs_power_on_reset(ufs);

		printf("Step 2 : Initialization\n");
		ufs_initialization(ufs);

		printf("Step 3 : Request Sense\n");
		ufs_req_sense(ufs);

		printf("Request Sense\n");
		ufs_req_sense(ufs);
		print_ufs_upiu(UFS_DEBUG_UPIU_ALL);

		ufs_req_sense(ufs);
		return 0;
		break;
	case 's':
		printf("Request Sense\n");
		ufs_req_sense(ufs);
		print_ufs_upiu(UFS_DEBUG_UPIU_ALL);
		return 0;
		break;
	case 'w':
		printf("Write buffser\n");
		scsi_write_buffer(0, 1, 0, 0, TORONTO_FIRMWARE_OFFSET01, 0x40000000);
		print_ufs_upiu(UFS_DEBUG_UPIU_ALL);
		return 0;
		break;
	case 'p':
		if (ufs->ufs_device_power == NULL) {
			printf("UFS device power control not supported.\n");
			return 1;
		}
		if (argv[2][0] == 'h') {
			printf("UFS device power on.\n");
			return ufs->ufs_device_power(ufs, 1);
		} else {
			printf("UFS device power off.\n");
			return ufs->ufs_device_power(ufs, 0);
		}
		break;
	case 'r':
		if (ufs->ufs_hw_reset == NULL) {
			printf("UFS device reset control not supported.\n");
			return 1;
		}
		if (argv[2][0] == 'h') {
			return ufs->ufs_hw_reset(ufs, 1);
		} else {
			return ufs->ufs_hw_reset(ufs, 0);
		}
		break;
	case 't':
		printf("Write buffser\n");
		dev = simple_strtoul(argv[2], NULL, 16);
		addr = simple_strtoul(argv[3], NULL, 16);
		blk = simple_strtoul(argv[4], NULL, 16);
		cnt = simple_strtoul(argv[5], NULL, 16);
		if (argc == 7)
			mode = simple_strtoul(argv[6], NULL, 10);
		else
			mode = 0;
		printf("\nSCSI test mode[%d]: device %d block # %ld, count %ld ...\n",
		       mode, dev, blk, cnt);
		scsi_write_read_test(dev, blk, cnt, (ulong *) addr, mode);
		return 0;
		break;
	default:
		break;
	}

	return CMD_RET_USAGE;
}

U_BOOT_CMD(ufs_test, 7, 1, do_ufstest, "UFS test function", "ufs_test [power/reset] [high/low]\n");

int v_cmd_lock_free()
{
	ccb *pccb = (ccb *) & tempccb;
	pccb->target = 0;
	pccb->lun = 0;
	pccb->datalen = 0;
	pccb->cmdlen = 16;
	memset(pccb->cmd, 0x00, sizeof(pccb->cmd));
	pccb->cmd[0] = 0xc0;
	pccb->cmd[1] = 0x43;

	if (scsi_exec(pccb) != TRUE)
		return -1;

	return 0;
}

void get_ufs_flags(void)
{
	ufs_send_upiu_with_lun(UFS_SEND_READ_DEVICEINIT_FLAG, 0);
	ufs_send_upiu_with_lun(UFS_SEND_GET_PERM_WP_FLAG, 0);
	ufs_send_upiu_with_lun(UFS_SEND_GET_POW_WP_FLAG, 0);
	ufs_send_upiu_with_lun(UFS_SEND_GET_BGO_EN_FLAG, 0);
	print_ufs_flags();
}

void set_ufs_flags(u32 flag_name, u32 en)
{
	switch (flag_name) {
	case 1:
		ufs_send_upiu_with_lun(UFS_SEND_SET_DEVICEINIT_FLAG, 0);
		break;
	case 2:
		//      ufs_send_upiu_with_lun(UFS_SEND_SET_PERM_WP_FLAG, 0);
		break;
	case 3:
		ufs_send_upiu_with_lun(UFS_SEND_SET_POW_WP_FLAG, 0);
		break;
	case 4:
		if (en == 1)
			ufs_send_upiu_with_lun(UFS_SEND_SET_BGO_EN_FLAG, 0);
		else
			ufs_send_upiu_with_lun(UFS_SEND_CLN_BGO_EN_FLAG, 0);
		break;
	case 6:
		ufs_send_upiu_with_lun(UFS_SEND_SET_PERGE_EN_FLAG, 0);
		break;
	default:
		break;
	}

	get_ufs_flags();
}

int do_setufs(cmd_tbl_t * cmdtp, int flag, int argc, char *const argv[])
{
	int bootlun = 0;

	if (argc > 3 && argv[1][0] == 'e') {
		u32 lun = simple_strtoul(argv[2], NULL, 16);
		if (lun > 7) {
			printf("LUN%d is not supported (0~7)\n", lun);
			return CMD_RET_USAGE;
		}
		switch (argv[3][0]) {
		case 'd':
			ufs_edit_config_desc(lun, 0, 0, 0, 0, 0);
			print_ufs_information();
			return 0;
			break;
		case 'e':
			if (argc != 8)
				return CMD_RET_USAGE;

			if (argv[4][0] == 'a' || argv[4][0] == 'b')
				bootlun = argv[4][0] - 'a' + 1;
			if (argv[4][0] == 'A' || argv[4][0] == 'B')
				bootlun = argv[4][0] - 'A' + 1;
			ufs_edit_config_desc(lun, 1, bootlun,
					     simple_strtoul(argv[5], NULL, 10),
					     simple_strtoul(argv[6], NULL, 10),
					     simple_strtoul(argv[7], NULL, 10));
			print_ufs_information();
			return 0;
			break;
		}
		return CMD_RET_USAGE;
	}

	if (argv[1][0] == 'l') {
		u32 set = simple_strtoul(argv[2], NULL, 16);
		if (set == 0) {
			if (v_cmd_lock_free())
				printf("[vendor] failed configuration lock free\n");
			return 0;
		}

		if (set == 1) {
			ufs_send_upiu_with_lun(UFS_SEND_WRITE_LOCK_ATTR, 0);
			return 0;
		}

		return CMD_RET_USAGE;
	}

	/* setufs flag (get/set) flag_name (enable/disable) */
	if ((strncmp(argv[1], "flag", 4) == 0)) {
		u32 en = 0, rw = 0;
		u32 flag_name = 0;

		if (strncmp(argv[2], "get", 3) == 0) {
			get_ufs_flags();
			return 0;
		} else if (strncmp(argv[2], "set", 3) == 0)
			flag_name = simple_strtoul(argv[3], NULL, 16);
		else
			return CMD_RET_USAGE;

		if ((flag_name == 2) || (argc != 5))
			return CMD_RET_USAGE;

		if (strncmp(argv[4], "ena", 3) == 0)
			en = 1;
		else if (strncmp(argv[4], "dis", 3) == 0)
			en = 0;
		else
			return CMD_RET_USAGE;

		set_ufs_flags(flag_name, en);
	}

	switch (argc) {
	case 2:
	case 3:
		switch (argv[1][0]) {
		case 'r':
			ufs_send_upiu_with_lun(UFS_SEND_READ_GEOMETRY_DESC, 0);
			ufs_send_upiu_with_lun(UFS_SEND_READ_CONFIG_DESC, 0);
			ufs_send_upiu_with_lun(UFS_SEND_READ_BOOTLUNEN_ATTR, 0);
			ufs_send_upiu_with_lun(UFS_SEND_READ_POWERMODE_ATTR, 0);
			/* fall through */
		case 's':
			print_ufs_information();
			return 0;
			break;
		case 'w':
			ufs_send_upiu_with_lun(UFS_SEND_WRITE_CONFIG_DESC, 0);
			ufs_send_upiu_with_lun(UFS_SEND_WRITE_BOOTLUNEN_ATTR, 0);
			ufs_send_upiu_with_lun(UFS_SEND_READ_BOOTLUNEN_ATTR, 0);
			return 0;
			break;
		}
		break;
	case 4:
		if (argv[1][0] == 'g') {
			ulong blun = simple_strtoul(argv[2], NULL, 16);
			ulong ulun = simple_strtoul(argv[3], NULL, 16);
			printf("Setup UFS boot LUN(0x%x) and user LUN(0x%x)\n", blun, ulun);
			set_bootLUN(blun);
			set_userLUN(ulun);
			return 0;
		}
		break;
	default:
		printf("Current UFS boot LUN(0x%x) and user LUN(0x%x)\n", get_bootLUN(), get_userLUN());
		return 0;
		break;
	}
	return CMD_RET_USAGE;
}

U_BOOT_CMD(
	setufs, 8, 1, do_setufs,
	"Setup UFS device info",
	"- Print current UFS device boot and user data LUN informations\n"
	"setufs geo bootLUN(0) userLUN(2) - Specify UFS device boot and user data LUN (default value 0 and 2)\n"
	"setufs read info - Get UFS device infomation (Configuration and geometry descriptor)\n"
	"setufs show info - Show UFS device infomation\n"
	"setufs edit LUN# {enable/disable} {0/a/b} write protect(0:no/1:fpoweronwpen/2:permanent) type(0/1/2/15:rpmb) capacity(Mbytes) \
			- Edit UFS device infomation (LUN:0~7, capa:0->all remainded capacity)\n"
	"setufs write info - Write UFS device infomation\n"
	"setufs lock 0 - Configuration lock free\n"
	"setufs lock 1 - Configuration lock\n"
	"setufs flag {get / set} flags {enable / disable}\n"
	" -- Flags list --\n"
	"{1:Device Initialization /\n"
	" 2:Permanent Write Protection Enable (Write Once) /\n"
	"  (!! Warning !! -> It cannot be toggled or cleared once it is set.)\n"
	" 3:Power On Write Protection Enable /\n"
	" 4:Background Operations Enable /\n"
	" 6:Purge Enable }\n");

