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
 * SCSI support based on the UFS chip.
 *	* cmd_scsi API
 *	int scsi_exec(ccb *pccb) : main entry function
 *	void scsi_bus_reset(void) : reset scsi bus
 *	void scsi_print_error (ccb * pccb) : error decode (optional)
 *
 *	* API to board.c
 *	int ufs_init(int mode)
 *
 *	* API to specific board
 *	int ufs_board_init(int host_index, struct ufs_host *ufs)
 */

#include <common.h>
#include <command.h>
#include <compiler.h>
#include <linux/compat.h>
#include <malloc.h>
#include <asm/dma-mapping.h>
#include <scsi.h>
#include <ufs.h>

#if defined(CONFIG_CMD_SCSI) && defined(CONFIG_SCSI_UFS)
#ifdef	SCSI_UFS_DEBUG
#define	ufs_debug(fmt,args...)	printf (fmt ,##args)
#else
#define ufs_debug(fmt,args...)
#endif

/*
	Multiple UFS host : cmd_scsi should be changed
	Now only one host supported
*/
static ccb tempccb;		/* temporary scsi command buffer */
static struct ufs_host *_ufs[CONFIG_SYS_SCSI_MAX_SCSI_ID] = { NULL, };

static int _ufs_index = 0;
/*********************************************************************************
 * forward declerations
 */

int ufs_link_startup(struct ufs_host *ufs);

/*
	called by common/cmd_scsi.c
*/

struct ufs_host *get_cur_ufs_host(void)
{
	return _ufs[_ufs_index];
}

#define	UFS_LOG_RING_BUFFER	64
static unsigned char _ufs_debug_buf[UFS_LOG_RING_BUFFER][16];
static unsigned long _ufs_debug_time[UFS_LOG_RING_BUFFER];
static int _ufs_head = 0, _ufs_tail = 0;

void push_scsi_cmd_log(unsigned char *cmd)
{
	_ufs_debug_time[_ufs_tail] = get_timer(0);
	memcpy(_ufs_debug_buf[_ufs_tail], cmd, 16);
	if (++_ufs_tail == UFS_LOG_RING_BUFFER)
		_ufs_tail = 0;
	if (_ufs_tail == _ufs_head) {
		if (++_ufs_head == UFS_LOG_RING_BUFFER) {
			_ufs_head = 0;
		}
	}
}

unsigned char *pop_scsi_cmd_log(unsigned long *timestamp)
{
	unsigned char *log = _ufs_debug_buf[_ufs_head];
	if (_ufs_head == _ufs_tail) {
		printf("No more command!! (head=%d, tail=%d)\n", _ufs_head, _ufs_tail);
		return NULL;
	}

	*timestamp = _ufs_debug_time[_ufs_head];
	if (++_ufs_head == UFS_LOG_RING_BUFFER)
		_ufs_head = 0;

	return log;
}

void print_scsi_cmd_log(void)
{
	int i;
	unsigned char *cmd;
	ulong diff, time, ptime;

	cmd = pop_scsi_cmd_log(&time);
	ptime = time;
	while (cmd) {
		diff = (time - ptime) / 2;
		if (diff < 10000)
			printf("%5d usec :", diff);
		else if (diff < (10 * 1000 * 1000))
			printf("%5d msec :", diff / 1000);
		else
			printf("%5d sec :", diff / (1000 * 1000));
		for (i = 0; i < 16; i++)
			printf(" %02x", cmd[i]);
		printf("\n");
		ptime = time;
		cmd = pop_scsi_cmd_log(&time);
	}
}

/********************************************************************************
 * reports SCSI errors to the user
	called by common/cmd_scsi.c
 */
void scsi_print_error(ccb * pccb)
{
	u8 *p = pccb->sense_buf;

	if (p[0] == 0x70) {
		printf("SenseKey (%02x) : ", p[2]);
		switch (p[2] & 0xf) {
		case 0:
			printf("NO SENSE\n");
			break;
		case 1:
			printf("RECOVERED ERROR\n");
			break;
		case 2:
			printf("NOT READY\n");
			break;
		case 3:
			printf("MEDIUM ERROR\n");
			break;
		case 4:
			printf("HARDWARE ERROR\n");
			break;
		case 5:
			printf("ILLEGAL REQUEST\n");
			break;
		case 6:
			printf("UNIT ATTENTION\n");
			break;
		case 7:
			printf("DATA PROTECT\n");
			break;
		case 8:
			printf("BLANK CHECK\n");
			break;
		case 9:
			printf("VENDOR SPECIFIC\n");
			break;
		case 10:
			printf("COPY ABORTED\n");
			break;
		case 11:
			printf("ABORTED COMMAND\n");
			break;
		case 13:
			printf("VOLUME OVERFLOW\n");
			break;
		case 14:
			printf("MISCOMPARE\n");
			break;
		default:
			printf("Obsolute or Reserved\n");
			break;
		}
		printf("ASC(%02x), ASCQ(%02x)\n", p[12], p[13]);
	}
}

/*
	called by common/cmd_scsi.c
*/
void scsi_bus_reset(void)
{
	struct ufs_host *ufs = get_cur_ufs_host();

	ufs_link_startup(ufs);
}

static int ufs_map_sg(struct ufs_host *ufs)
{
	int i, len, sg_segments;

	if (!ufs->scsi_cmd)
		goto no_data;

	len = ufs->scsi_cmd->datalen;
	if (len < 0)
		goto no_data;

	if (len) {
		sg_segments = (len + UFS_SG_BLOCK_SIZE - 1) / UFS_SG_BLOCK_SIZE;
		ufs->utrd_addr->prdt_len =
		    cpu_to_le16((u16) (sg_segments) * sizeof(struct ufs_prdt));
		ufs->utrd_addr->prdt_off = cpu_to_le16(ALIGNED_UPIU_SIZE * 2);
		for (i = 0; i < sg_segments; i++) {
#if (UFS_ACCESS_ONLY_4K==TRUE)
			ufs->cmd_desc_addr->prd_table[i].size =
			    cpu_to_le32((u32) UFS_SG_BLOCK_SIZE - 1);
#else
			if (len > UFS_SG_BLOCK_SIZE) {
				ufs->cmd_desc_addr->prd_table[i].size =
				    cpu_to_le32((u32) UFS_SG_BLOCK_SIZE - 1);
			} else {
				ufs->cmd_desc_addr->prd_table[i].size = cpu_to_le32((u32) len - 1);
			}
#endif
			ufs->cmd_desc_addr->prd_table[i].base_addr =
			    cpu_to_le32((u32) (ufs->scsi_cmd->pdata) + i * UFS_SG_BLOCK_SIZE);
			len -= UFS_SG_BLOCK_SIZE;
		}
		return 0;
	}

 no_data:
	ufs->utrd_addr->prdt_len = 0;
	ufs->utrd_addr->prdt_off = 0;
	return 0;
}

static void ufs_compose_upiu(struct ufs_host *ufs)
{
	u32 data_direction;
	u32 upiu_flags;
	u32 type;
	u32 function;
	u32 opcode;
	u32 descid;
	u8 *p = ufs->cmd_desc_addr->command_upiu.header.tsf;
	ccb *pccb = (ccb *) & tempccb;
/*
	command_type :
	| descriptor ID | opcode | function | UPIU type |
	|       LUN        |Task Tag| function | UPIU type |

	UPIU type[7:0] : NOP_OUT, COMMAND, DATA_OUT, TASK_REQ, QUERY_REQ
	function[15:8] : Used by QUERY_REQ / TASK_REQ
	opcode[23:16] : Opcode in query / input parameter2=Task Tag (TASK_REQ)
	descriptor ID[31:24] : descriptor ID in query / input parameter1=LUN (TASK_REQ)
*/
	type = (ufs->command_type) & 0xff;
	function = ((ufs->command_type) >> 8) & 0xff;
	opcode = ((ufs->command_type) >> 16) & 0xff;
	descid = ((ufs->command_type) >> 24) & 0xff;
#ifdef	SCSI_UFS_DEBUG
	printf("ufs_compose_upiu : type(%02x) function(%02x) opcode(%02x) descID(%02x)\n", type,
	       function, opcode, descid);
#endif
	switch (type) {
	case UPIU_TRANSACTION_COMMAND:
		if (ufs->scsi_cmd->datalen) {
			switch (ufs->scsi_cmd->cmd[0]) {
			case SCSI_MODE_SEL6:
			case SCSI_MODE_SEL10:
			case SCSI_UNMAP:
			case SCSI_FORMAT:
			case SCSI_WRITE6:
			case SCSI_WRITE10:
			case 0x8a:	/* WRITE16 */
			case SCSI_WRITE_BUFF:
			case SCSI_SECU_PROT_OUT:
				data_direction = UTP_HOST_TO_DEVICE;
				upiu_flags = UPIU_CMD_FLAGS_WRITE;
				break;
			default:
				data_direction = UTP_DEVICE_TO_HOST;
				upiu_flags = UPIU_CMD_FLAGS_READ;
				break;
			}
		} else {
			data_direction = UTP_NO_DATA_TRANSFER;
			upiu_flags = UPIU_CMD_FLAGS_NONE;
		}
		ufs->utrd_addr->dw[0] =
		    cpu_to_le32(data_direction | UTP_SCSI_COMMAND | UTP_REQ_DESC_INT_CMD);
		ufs->utrd_addr->dw[2] = cpu_to_le32(OCS_INVALID_COMMAND_STATUS);

		ufs->cmd_desc_addr->command_upiu.header.type = type;
		ufs->cmd_desc_addr->command_upiu.header.flags = upiu_flags;
		ufs->cmd_desc_addr->command_upiu.header.lun = ufs->lun;
		ufs->cmd_desc_addr->command_upiu.header.tag = 0;	/* queuing is not used */

		ufs->cmd_desc_addr->command_upiu.header.tsf[0] =
		    cpu_to_be32(ufs->scsi_cmd->datalen);
		memcpy(&ufs->cmd_desc_addr->command_upiu.header.tsf[1], ufs->scsi_cmd->cmd,
		       (min_t(unsigned short, ufs->scsi_cmd->cmdlen, MAX_CDB_SIZE)));
		break;
	case UPIU_TRANSACTION_TASK_REQ:
		ufs->utmrd_addr->dw[2] = cpu_to_le32(OCS_INVALID_COMMAND_STATUS);
		ufs->utmrd_addr->task_req_upiu.type = type;
		ufs->utmrd_addr->task_req_upiu.flags = UPIU_CMD_FLAGS_NONE;
		ufs->utmrd_addr->task_req_upiu.lun = ufs->lun;
		ufs->utmrd_addr->task_req_upiu.tag = 0;	/* queuing is not used */
		ufs->utmrd_addr->task_req_upiu.function = function;
		ufs->utmrd_addr->task_req_upiu.tsf[0] = cpu_to_be32(ufs->lun);
		ufs->utmrd_addr->task_req_upiu.tsf[1] = 0;	/* queuing is not used */
		break;
	case UPIU_TRANSACTION_QUERY_REQ:
		/* For query function implementation */
		ufs->utrd_addr->dw[0] = UTP_REQ_DESC_INT_CMD;
		ufs->utrd_addr->dw[2] = cpu_to_le32(OCS_INVALID_COMMAND_STATUS);

		ufs->cmd_desc_addr->command_upiu.header.type = type;
		ufs->cmd_desc_addr->command_upiu.header.flags = UPIU_CMD_FLAGS_NONE;
		ufs->cmd_desc_addr->command_upiu.header.tag = 0;		/* queuing is not used */
		ufs->cmd_desc_addr->command_upiu.header.function = function;
		p[0] = opcode;	/* opcode */
		p[1] = descid;	/* descripot id */
		p[2] = ufs->lun;
		switch (function) {
		case UFS_STD_READ_REQ:
			*(u16 *) (&p[6]) = cpu_to_be16(4 * UPIU_DATA_SIZE);
			break;
		case UFS_STD_WRITE_REQ:
			if (ufs->data_seg_len) {
				*(u16 *) (&p[6]) = cpu_to_be16(ufs->data_seg_len);
				ufs->cmd_desc_addr->command_upiu.header.datalength =
				    cpu_to_be16(ufs->data_seg_len);
				memcpy(ufs->cmd_desc_addr->command_upiu.data, ufs->upiu_data,
				       (min_t
					(unsigned short, ufs->data_seg_len, 4 * UPIU_DATA_SIZE)));
			}
			if (opcode == UPIU_QUERY_OPCODE_WRITE_ATTR) {
				*(u32 *) (&p[8]) = cpu_to_be32(ufs->attributes.arry[descid]);
			}
			break;
		case UFS_VENDOR_FORMAT_REQ:
			if (ufs->data_seg_len) {
				ufs->cmd_desc_addr->command_upiu.header.datalength =
				    cpu_to_be16(ufs->data_seg_len);
				memcpy(ufs->cmd_desc_addr->command_upiu.data, ufs->upiu_data,
				       (min_t
					(unsigned short, ufs->data_seg_len, 4 * UPIU_DATA_SIZE)));
			}
			break;
		}
		break;
	case UPIU_TRANSACTION_NOP_OUT:
	default:
		break;
	}
}

/********************************************************************************
 * ufs minimal interrupt handler
 */
static int handle_ufs_int(struct ufs_host *ufs)
{
	u32 intr_stat;

	if (!ufs->timeout--) {
		//print_ufs_upiu(UFS_DEBUG_UPIU_ALL);
		//print_scsi_cmd_log();

		ufs->ufs_state = UFS_TRANSFER_TIMEOUT | UFS_TRANSFER_COMPLETE;
		return UFS_ERROR_TIMEOUT;
	}

	intr_stat = readl(ufs->ioaddr + REG_INTERRUPT_STATUS);
	if (intr_stat & 1) {
		if (ufs->ufs_state == UFS_TRANSFER_COMMAND) {
			ufs->ufs_state = UFS_TRANSFER_COMM_DONE;
		} else {
			printf
			    ("IS(%x) : UTP Transfer Request Completion Status w/o UFS_TRANSFER_COMMAND\n",
			     intr_stat);
		}
	}
#ifdef	SCSI_UFS_DEBUG
	if (intr_stat & (1 << 1)) {
		printf("IS(%x) : UIC DME_ENDPOINTRESET Indication in status(%x)\n", intr_stat,
		       ufs->ufs_state);
	}

	if (intr_stat & (1 << 2)) {
		printf("IS(%x) : UIC ERROR in status(%x)\n", intr_stat, ufs->ufs_state);
	}

	if (intr_stat & (1 << 3)) {
		printf("IS(%x) : UIC Test Mode Status in status(%x)\n", intr_stat, ufs->ufs_state);
	}

	if (intr_stat & (1 << 4)) {
		printf("IS(%x) : UIC Power Mode Status in status(%x)\n", intr_stat, ufs->ufs_state);
	}

	if (intr_stat & (1 << 5)) {
		printf("IS(%x) : UIC Hibernate Exit Status in status(%x)\n", intr_stat,
		       ufs->ufs_state);
	}

	if (intr_stat & (1 << 6)) {
		printf("IS(%x) : UIC Hibernate Enter Status in status(%x)\n", intr_stat,
		       ufs->ufs_state);
	}
#endif
	if (intr_stat & (1 << 7)) {
		printf("IS(%x) : UIC Link Lost Status in status(%x), clear doorbell and retry\n",
		       intr_stat, ufs->ufs_state);
		if ((readl(ufs->ioaddr + REG_INTERRUPT_STATUS) & 0x7) == 0x7) {
			if (ufs->ufs_state == UPIU_TRANSACTION_QUERY_REQ) {
				writel(1, ufs->ioaddr + REG_UTP_TRANSFER_REQ_LIST_CLEAR);
				while (readl(ufs->ioaddr + REG_UTP_TRANSFER_REQ_DOOR_BELL)) ;
				writel(1, (ufs->ioaddr + REG_UTP_TRANSFER_REQ_DOOR_BELL));
			} else if (ufs->ufs_state == UPIU_TRANSACTION_TASK_REQ) {
				writel(1, ufs->ioaddr + REG_UTP_TASK_REQ_LIST_CLEAR);
				while (readl(ufs->ioaddr + REG_UTP_TASK_REQ_DOOR_BELL)) ;
				writel(1, (ufs->ioaddr + REG_UTP_TASK_REQ_DOOR_BELL));
			} else {
				printf("\tNOT TRANSFER STATE...\n");
			}
		} else {
			ufs->ufs_state =
			    UFS_TRANSFER_COMPLETE | UFS_TRANSFER_ERROR | UFS_TRANSFER_FATAL_ERROR;
		}
	}

	if (intr_stat & (1 << 8)) {
		printf("IS(%x) : UIC Link Startup Status in status(%x)\n", intr_stat,
		       ufs->ufs_state);
		if ((readl(ufs->ioaddr + REG_INTERRUPT_STATUS) & 0x7) == 0x7) {
			if (ufs->ufs_state == UPIU_TRANSACTION_QUERY_REQ) {
				writel(1, ufs->ioaddr + REG_UTP_TRANSFER_REQ_LIST_CLEAR);
				while (readl(ufs->ioaddr + REG_UTP_TRANSFER_REQ_DOOR_BELL)) ;
				writel(1, (ufs->ioaddr + REG_UTP_TRANSFER_REQ_DOOR_BELL));
			} else if (ufs->ufs_state == UPIU_TRANSACTION_TASK_REQ) {
				writel(1, ufs->ioaddr + REG_UTP_TASK_REQ_LIST_CLEAR);
				while (readl(ufs->ioaddr + REG_UTP_TASK_REQ_DOOR_BELL)) ;
				writel(1, (ufs->ioaddr + REG_UTP_TASK_REQ_DOOR_BELL));
			} else {
				printf("\tNOT TRANSFER STATE...\n");
			}
		} else {
			ufs->ufs_state =
			    UFS_TRANSFER_COMPLETE | UFS_TRANSFER_ERROR | UFS_TRANSFER_FATAL_ERROR;
		}
	}

	if (intr_stat & (1 << 9)) {
		if (ufs->ufs_state == UFS_TRANSFER_TASK) {
			ufs->ufs_state = UFS_TRANSFER_TASK_DONE;
		} else {
			printf
			    ("IS(%x) : UTP Task Management Request Completion Status w/o UFS_TRANSFER_TASK\n",
			     intr_stat);
		}
	}

	if (intr_stat & (1 << 10)) {
		if (ufs->ufs_state == UFS_TRANSFER_UIC) {
			ufs->ufs_state = UFS_TRANSFER_COMPLETE;
		} else {
			printf("IS(%x) : UIC Command Completion Status w/o UFS_TRANSFER_UIC\n",
			       intr_stat);
		}
	}

	if (intr_stat & (1 << 11)) {
		printf("IS(%x) : Device Fatal Error Status in status(%x)\n", intr_stat,
		       ufs->ufs_state);
		ufs->ufs_state =
		    UFS_TRANSFER_COMPLETE | UFS_TRANSFER_ERROR | UFS_TRANSFER_FATAL_ERROR;
	}

	if (intr_stat & (1 << 16)) {
		printf("IS(%x) : Host Controller Fatal Error Status in status(%x)\n", intr_stat,
		       ufs->ufs_state);
		ufs->ufs_state =
		    UFS_TRANSFER_COMPLETE | UFS_TRANSFER_ERROR | UFS_TRANSFER_FATAL_ERROR;
	}

	if (intr_stat & (1 << 17)) {
		printf("IS(%x) : System Bus Fatal Error Status in status(%x)\n", intr_stat,
		       ufs->ufs_state);
		ufs->ufs_state =
		    UFS_TRANSFER_COMPLETE | UFS_TRANSFER_ERROR | UFS_TRANSFER_FATAL_ERROR;
	}

	if (intr_stat & 0xFFFCF000) {
		printf("IS(%x) : Reserved Status in status(%x)\n", intr_stat, ufs->ufs_state);
	}

	if (intr_stat)
		writel(intr_stat, ufs->ioaddr + REG_INTERRUPT_STATUS);

	if (ufs->ufs_state == UFS_TRANSFER_COMM_DONE) {
		if (!(readl(ufs->ioaddr + REG_UTP_TRANSFER_REQ_DOOR_BELL) & 1)) {
			ufs->ufs_state = UFS_TRANSFER_COMPLETE;
		}
	}

	if (ufs->ufs_state == UFS_TRANSFER_TASK_DONE) {
		if (!(readl(ufs->ioaddr + REG_UTP_TASK_REQ_DOOR_BELL) & 1)) {
			ufs->ufs_state = UFS_TRANSFER_COMPLETE;
		}
	}

	if (!(ufs->ufs_state & UFS_TRANSFER_COMPLETE))
		udelay(1);

	return UFS_NO_ERROR;
}

void print_scsi_cmd(ccb * pccb)
{
	int i, len;
	if (!pccb)
		return;

	printf("target %d, LUN %d\n\tCMD(%d):", pccb->target, pccb->lun, pccb->cmdlen);
	for (i = 0; i < pccb->cmdlen; i++) {
		printf(" %02x", pccb->cmd[i]);
	}
	printf("\n");
	if (pccb->datalen) {
		len = pccb->datalen;
		if (len > 16)
			len = 16;
		printf("\tData(%d):", pccb->datalen);
		for (i = 0; i < len; i++) {
			printf(" %02x", pccb->pdata[i]);
		}
		printf("\n");
	}
}

void print_ufs_upiu(int print_level)
{
	int i, len;
	u32 level;
	struct ufs_host *ufs = get_cur_ufs_host();
	if (!ufs)
		return;

	printf("\n");

	level = print_level & 0x7;
	if (level) {
		printf("UFS registers\n");
		if (level > 1)
			len = 0x200;
		else
			len = 0x100;
		for (i = 0; i < len / 4; i++) {
			if ((i & 3) == 0)
				printf("%08x :", ((u32) (ufs->ioaddr) + 4 * i));
			printf(" %08x", *((u32 *) (ufs->ioaddr) + i));
			if ((i & 3) == 3)
				printf("\n");
		}
	}

	level = (print_level >> 4) & 0x7;
	if (level) {
		printf("UTP Transfer Request Descriptor (0x%08x)\n", ufs->utrd_addr);
		for (i = 0; i < 8; i++) {
			printf(" %08x", *((u32 *) (ufs->utrd_addr) + i));
			if ((i & 3) == 3)
				printf("\n");
		}
	}

	level = (print_level >> 8) & 0x7;
	if (level++) {
		len = level * 4;
		printf("UTP Command Descriptor\nCommand UPIU (0x%08x)\n",
		       &(ufs->cmd_desc_addr->command_upiu));
		for (i = 0; i < len; i++) {
			printf(" %08x", *((u32 *) (&ufs->cmd_desc_addr->command_upiu) + i));
			if ((i & 3) == 3)
				printf("\n");
		}
	}

	level = (print_level >> 12) & 0x7;
	if (level++) {
		len = level * 4;
		printf("Response UPIU (0x%08x)\n", &(ufs->cmd_desc_addr->response_upiu));
		for (i = 0; i < len; i++) {
			printf(" %08x", *((u32 *) (&ufs->cmd_desc_addr->response_upiu) + i));
			if ((i & 3) == 3)
				printf("\n");
		}
	}

	if (ufs->utrd_addr->prdt_len) {
		level = (print_level >> 16) & 0x7;
		if (level) {
			len = ufs->utrd_addr->prdt_len / 4;
			if (len > 4 * level)
				len = 4 * level;
			printf("PRDT (0x%08x) length %d\n", ufs->cmd_desc_addr->prd_table,
			       ufs->utrd_addr->prdt_len);
			for (i = 0; i < len; i++) {
				printf(" %08x", *((u32 *) (ufs->cmd_desc_addr->prd_table) + i));
				if ((i & 3) == 3)
					printf("\n");
			}
		}

		level = (print_level >> 20) & 0x7;
		if (level++) {
			len = ufs->cmd_desc_addr->prd_table[0].size + 1;
			printf("Data (addr=0x%08x, total length=%d, first prd length=%d)\n",
			       ufs->cmd_desc_addr->prd_table[0].base_addr,
			       be32_to_cpu(ufs->cmd_desc_addr->command_upiu.header.tsf[0]), len);
			if (len > 16 * level)
				len = 16 * level;
			for (i = 0; i < len; i++) {
				printf(" %02x",
				       *((u8 *) (ufs->cmd_desc_addr->prd_table[0].base_addr) + i));
				if ((i & 15) == 15)
					printf("\n");
			}
		}
	}

	level = (print_level >> 24) & 0x7;
	if (level) {
		print_scsi_cmd(ufs->scsi_cmd);
	}
	printf("\n");
}

void print_ufs_desc(u8 * desc)
{
	int i, len, type;
	if (!desc)
		return;

	len = desc[0];
	type = desc[1];

	printf("Descriptor length 0x%02x, type 0x%02x\n", len, type);

	for (i = 0; i < len; i++) {
		if ((i & 0xf) == 0)
			printf("%02x :", i);
		printf(" %02x", desc[i]);
		if ((i & 0xf) == 0xf)
			printf("\n");
	}
}

void print_ufs_device_desc(u8 * desc)
{
	int len, type;
	struct ufs_device_desc *dp = desc;
	if (!desc)
		return;

	len = desc[0];
	type = desc[1];

	printf("Device Descriptor\n");
	printf("------------------------------------------------------\n");
	printf("bLength = 0x%02x (1Fh)\n", dp->bLength);
	printf("bDescriptorType = 0x%02x (00h)\n", dp->bDescriptorType);
	printf("bDevice = 0x%02x (00h device, others reserved)\n", dp->bDevice);
	printf("bDeviceClass = 0x%02x (00h Mass storage)\n", dp->bDeviceClass);
	printf
	    ("bDeviceSubClass = 0x%02x (00h embedded bootable, 01h embedded non-bootable, 02h removable non-bootable)\n",
	     dp->bDeviceSubClass);
	printf("bProtocol = 0x%02x (00h scsi)\n", dp->bProtocol);
	printf("bNumberLU = 0x%02x (01h~08h, default 01h)\n", dp->bNumberLU);
	printf("iNumberWLU = 0x%02x (04h)\n", dp->iNumberWLU);
	printf("bBootEnable = 0x%02x (00h boot disabled, 01h boot enabled)\n", dp->bBootEnable);
	printf("bDescrAccessEn = 0x%02x (00h device desc access disabled, 01h access enabled)\n",
	       dp->bDescrAccessEn);

	printf("bInitPowerMode = 0x%02x (00h UFS sleep mode, 01h active mode)\n",
	       dp->bInitPowerMode);
	printf("bHighPriorityLUN = 0x%02x (00h~07h, 7Fh same priority)\n", dp->bHighPriorityLUN);
	printf("bSecureRemovalType = 0x%02x (00h~03h)\n", dp->bSecureRemovalType);
	printf("bSecurityLU = 0x%02x (00h not support, 01h RPMB)\n", dp->bSecurityLU);
	printf("bInitActiveICCLevel = 0x%02x (00h~0Fh, default 00h)\n", dp->bInitActiveICCLevel);
	printf("wSpecVersion = 0x%04x\n", be16_to_cpu(dp->wSpecVersion));
	printf("wManufactureData = 0x%04x (MM/YY)\n", be16_to_cpu(dp->wManufactureData));
	printf("iManufacturerName = 0x%02x\n", dp->iManufacturerName);
	printf("iProductName = 0x%02x\n", dp->iProductName);
	printf("iSerialNumber = 0x%02x\n", dp->iSerialNumber);

	printf("iOemID = 0x%02x\n", dp->iOemID);
	printf("wManufacturerID = 0x%04x\n", be16_to_cpu(dp->wManufacturerID));
	printf("bUD0BaseOffset = 0x%02x (10h)\n", dp->bUD0BaseOffset);
	printf("bUDConfigPlength = 0x%02x (10h)\n", dp->bUDConfigPlength);
	printf("bDeviceRTTCap = 0x%02x (minimum value 02h)\n", dp->bDeviceRTTCap);
	printf("wPeriodicRTCUpdate = 0x%04x\n", be16_to_cpu(dp->wPeriodicRTCUpdate));
	printf("------------------------------------------------------\n");
}

void print_ufs_configuration_desc(u8 * desc)
{
	int i, len, type;
	struct ufs_config_desc *dp = desc;
	if (!desc)
		return;

	len = desc[0];
	type = desc[1];

	printf("Configuration Descriptor\n");
	printf("------------------------------------------------------\n");
	printf("Configuration Descriptor Header\n");
	printf("bLength = 0x%02x (90h)\n", dp->header.bLength);
	printf("bDescriptorType = 0x%02x (01h)\n", dp->header.bDescriptorType);
	printf("bNumberLU = 0x%02x\n", dp->header.bNumberLU);
	printf("bBootEnable = 0x%02x\n", dp->header.bBootEnable);
	printf("bDescrAccessEn = 0x%02x\n", dp->header.bDescrAccessEn);
	printf("bInitPowerMode = 0x%02x\n", dp->header.bInitPowerMode);
	printf("bHighPriorityLUN = 0x%02x\n", dp->header.bHighPriorityLUN);
	printf("bSecureRemovalType = 0x%02x\n", dp->header.bSecureRemovalType);
	printf("bInitActiveICCLevel = 0x%02x\n", dp->header.bInitActiveICCLevel);
	printf("wPeriodicRTCUpdate = 0x%04x\n", be16_to_cpu(dp->header.wPeriodicRTCUpdate));

	for (i = 0; i < 8; i++) {
		printf("Unit Descriptor configurable parameters (%02xh)\n", i);
		printf("\tbLUEnable = 0x%02x\n", dp->unit[i].bLUEnable);
		printf("\tbBootLunID = 0x%02x\n", dp->unit[i].bBootLunID);
		printf("\tbLUWriteProtect = 0x%02x\n", dp->unit[i].bLUWriteProtect);
		printf("\tbMemoryType = 0x%02x\n", dp->unit[i].bMemoryType);
		printf("\tdNumAllocUnits = 0x%08x\n", be32_to_cpu(dp->unit[i].dNumAllocUnits));
		printf("\tbDataReliability = 0x%02x\n", dp->unit[i].bDataReliability);
		printf("\tbLogicalBlockSize = 0x%02x\n", dp->unit[i].bLogicalBlockSize);
		printf("\tbProvisioningType = 0x%02x\n", dp->unit[i].bProvisioningType);
		printf("\twContextCapabilities = 0x%04x\n",
		       be16_to_cpu(dp->unit[i].wContextCapabilities));
	}
	printf("------------------------------------------------------\n");
}

void print_ufs_geometry_desc(u8 * desc)
{
	int len, type;
	struct ufs_geometry_desc *dp = desc;
	if (!desc)
		return;

	len = desc[0];
	type = desc[1];

	printf("Geometry Descriptor\n");
	printf("------------------------------------------------------\n");
	printf("bLength = 0x%02x (44h)\n", dp->bLength);
	printf("bDescriptorType = 0x%02x (07h)\n", dp->bDescriptorType);
	printf("bMediaTechnology = 0x%02x (00h reserved)\n", dp->bMediaTechnology);
	printf("qTotalRawDeviceCapacity = 0x%08x%08x (512bytes)\n",
	       be32_to_cpu(dp->qTotalRawDeviceCapacity_h),
	       be32_to_cpu(dp->qTotalRawDeviceCapacity_l));
	printf("dSegmentSize = 0x%08x (512bytes)\n", be32_to_cpu(dp->dSegmentSize));
	printf("bAllocationUnitSize = 0x%02x (segments)\n", dp->bAllocationUnitSize);
	printf("bMinAddrBlockSize = 0x%02x (512bytes, minimum value 08h=4KB)\n",
	       dp->bMinAddrBlockSize);
	printf("bOptimalReadBlockSize = 0x%02x (512bytes, 0=not available)\n",
	       dp->bOptimalReadBlockSize);
	printf("bOptimalWriteBlockSize = 0x%02x (512bytes)\n", dp->bOptimalWriteBlockSize);
	printf("bMaxInBufferSize = 0x%02x (512bytes, minimum value 08h=4KB)\n",
	       dp->bMaxInBufferSize);
	printf("bMaxOutBufferSize = 0x%02x (512bytes, minimum value 08h=4KB)\n",
	       dp->bMaxOutBufferSize);
	printf("bRPMB_ReadWriteSize = 0x%02x\n", dp->bRPMB_ReadWriteSize);
	printf("bDataOrdering = 0x%02x (00h not support, 01h support)\n", dp->bDataOrdering);
	printf("bMaxContexIDNumber = 0x%02x (minimum value 05h)\n", dp->bMaxContexIDNumber);
	printf("bSysDataTagUnitSize = 0x%02x\n", dp->bSysDataTagUnitSize);
	printf("bSysDataTagResSize = 0x%02x\n", dp->bSysDataTagResSize);
	printf("bSupportedSecRTypes = 0x%02x\n", dp->bSupportedSecRTypes);
	printf("wSupportedMemoryTypes = 0x%04x\n", be16_to_cpu(dp->wSupportedMemoryTypes));
	printf("dSystemCodeMaxNAllocU = 0x%08x\n", be32_to_cpu(dp->dSystemCodeMaxNAllocU));
	printf("wSystemCodeCapAdjFac = 0x%04x\n", be16_to_cpu(dp->wSystemCodeCapAdjFac));
	printf("dNonPersistMaxNAllocU = 0x%08x\n", be32_to_cpu(dp->dNonPersistMaxNAllocU));
	printf("wNonPersistCapAdjFac = 0x%04x\n", be16_to_cpu(dp->wNonPersistCapAdjFac));
	printf("dEnhanced1MaxNAllocU = 0x%08x\n", be32_to_cpu(dp->dEnhanced1MaxNAllocU));
	printf("wEnhanced1CapAdjFac = 0x%0x\n", be16_to_cpu(dp->wEnhanced1CapAdjFac));
	printf("dEnhanced2MaxNAllocU = 0x%08x\n", be32_to_cpu(dp->dEnhanced2MaxNAllocU));
	printf("wEnhanced2CapAdjFac = 0x%04x\n", be16_to_cpu(dp->wEnhanced2CapAdjFac));
	printf("dEnhanced3MaxNAllocU = 0x%08x\n", be32_to_cpu(dp->dEnhanced3MaxNAllocU));
	printf("wEnhanced3CapAdjFac = 0x%04x\n", be16_to_cpu(dp->wEnhanced3CapAdjFac));
	printf("dEnhanced4MaxNAllocU = 0x%08x\n", be32_to_cpu(dp->dEnhanced4MaxNAllocU));
	printf("wEnhanced4CapAdjFac = 0x%04x\n", be16_to_cpu(dp->wEnhanced4CapAdjFac));
	printf("------------------------------------------------------\n");
}

void print_ufs_information(void)
{
	int i;
	u32 capacity, alloc_unit, value;
	struct ufs_host *ufs = get_cur_ufs_host();
	if (!ufs)
		return;

	printf("----------------------------------------------------------------------\n");
	printf("UFS device information\n");
	printf("----------------------------------------------------------------------\n");
	capacity = (2048 * 1024) * be32_to_cpu(ufs->geometry_desc.qTotalRawDeviceCapacity_h)
	    + be32_to_cpu(ufs->geometry_desc.qTotalRawDeviceCapacity_l) / (2 * 1024);
	printf("Capacity\t\t\t%d Gbytes (%dMbytes)\n", capacity / 1024, capacity);
	printf("Erase block size\t\t%d Kbytes\n", be32_to_cpu(ufs->geometry_desc.dSegmentSize) / 2);
	alloc_unit =
	    ufs->geometry_desc.bAllocationUnitSize * be32_to_cpu(ufs->geometry_desc.dSegmentSize) /
	    2;
	printf("Allocation unit size\t\t%d Kbytes\n", alloc_unit);
	printf("Address block size\t\t%d Kbytes\n", ufs->geometry_desc.bMinAddrBlockSize / 2);
	printf("Optimal read block size\t\t%d Kbytes\n",
	       ufs->geometry_desc.bOptimalReadBlockSize / 2);
	printf("Optimal write block size\t%d Kbytes\n",
	       ufs->geometry_desc.bOptimalReadBlockSize / 2);
	printf("Supported memory type\n");
	value = be16_to_cpu(ufs->geometry_desc.wSupportedMemoryTypes);
	if (value & (1 << 0))
		printf("\tNormal memory\n");
	if (value & (1 << 1))
		printf("\tSystem code memory\n");
	if (value & (1 << 2))
		printf("\tNon-Persistent memory\n");
	if (value & (1 << 3))
		printf("\tEnhanced memory memory type 1\n");
	if (value & (1 << 4))
		printf("\tEnhanced memory memory type 2\n");
	if (value & (1 << 5))
		printf("\tEnhanced memory memory type 3\n");
	if (value & (1 << 6))
		printf("\tEnhanced memory memory type 4\n");
	if (value & (1 << 15))
		printf("\tRPMB memory\n");
	printf("Availble LUNs\t\t\t%d ea\n", ufs->config_desc.header.bNumberLU);
	if (ufs->config_desc.header.bBootEnable == 1)
		printf("Boot feature\t\t\tenabled\n");
	else
		printf("Boot feature\t\t\tdisabled\n");
	if (ufs->config_desc.header.bDescrAccessEn == 1)
		printf("Descriptor access\t\tenabled\n");
	else
		printf("Descriptor access\t\tdisabled\n");
	if (ufs->config_desc.header.bInitPowerMode == 1)
		printf("Initial Power Mode\t\tActive Mode\n");
	else
		printf("Initial Power Mode\t\tUFS-Sleep Mode\n");
	if (ufs->config_desc.header.bHighPriorityLUN == 0x7f)
		printf("All logical unit have the same priority\n");
	else
		printf("High priority logical unit\t%d\n",
		       ufs->config_desc.header.bHighPriorityLUN);

	printf("----------------------------------------------------------------------\n");
	printf("\t\t\tLUN0\tLUN1\tLUN2\tLUN3\tLUN4\tLUN5\tLUN6\tLUN7\n");
	printf("LU en");
	for (i = 0; i < 8; i++) {
		printf("\t\t");
		if (ufs->config_desc.unit[i].bLUEnable == 1)
			printf("en");
		else
			printf("dis");
		if (i == 7)
			printf("\n");
	}
	printf("Boot\t");
	for (i = 0; i < 8; i++) {
		printf("\t");
		switch (ufs->config_desc.unit[i].bBootLunID) {
		case 1:
			printf("BootA");
			break;
		case 2:
			printf("BootB");
			break;
		default:
			printf("\t");
			break;
		}
		if (i == 7)
			printf("\n");
	}
	printf("WP\t");
	for (i = 0; i < 8; i++) {
		printf("\t\t");
		switch (ufs->config_desc.unit[i].bLUWriteProtect) {
		case 1:
			printf("WP");
			break;
		case 2:
			printf("perWP");
			break;
		}
		if (i == 7)
			printf("\n");
	}
	printf("Type\t");
	for (i = 0; i < 8; i++) {
		printf("\t");
		switch (ufs->config_desc.unit[i].bMemoryType) {
		case 0:
			printf("Normal");
			break;
		case 1:
			printf("SysCode");
			break;
		case 2:
			printf("NonPer");
			break;
		case 3:
		case 4:
		case 5:
		case 6:
			printf("Type%d", ufs->config_desc.unit[i].bMemoryType - 2);
			break;
		}
		if (i == 7)
			printf("\n");
	}
	printf("Capa");
	for (i = 0; i < 8; i++) {
		printf("\t\t");
		value = be32_to_cpu(ufs->config_desc.unit[i].dNumAllocUnits) * alloc_unit;
		if (value < 1024)
			printf("%dK", value);
		else if (value < 1024 * 1024)
			printf("%dM", value / 1024);
		else
			printf("%dG", value / (1024 * 1024));
		if (i == 7)
			printf("\n");
	}
	printf("BlSize");
	for (i = 0; i < 8; i++) {
		printf("\t\t");
		value = 1 << ufs->config_desc.unit[i].bLogicalBlockSize;
		if (value < 1024)
			printf("%dK", value);
		else if (value < 1024 * 1024)
			printf("%dM", value / 1024);
		else
			printf("%dG", value / (1024 * 1024));
		if (i == 7)
			printf("\n");
	}
	printf("----------------------------------------------------------------------\n");
}

void print_ufs_flags(void)
{
	struct ufs_host *ufs = get_cur_ufs_host();
	if (!ufs)
		return;

	printf("----------------------------------------------------------------------\n");
	printf("UFS device flags\n");
	printf("----------------------------------------------------------------------\n");
	printf("UFS flag : fDeviceInit: %d\n", ufs->flags.flag.fDeviceInit);
	printf("UFS flag : fPermanentWPEn: %d\n", ufs->flags.flag.fPermanentWPEn);
	printf("UFS flag : fPowerOnWPEn: %d\n", ufs->flags.flag.fPowerOnWPEn);
	printf("UFS flag : fBackgroundOpsEn: %d\n", ufs->flags.flag.fBackgroundOpsEn);
}

int send_uic_cmd(struct ufs_host *ufs)
{
	int err = 0, error_code;

//	while(!(readl(ufs->ioaddr + REG_CONTROLLER_STATUS) & (1<<3)))
//	{
//	};

	writel(ufs->uic_cmd->uiccmdarg1, (ufs->ioaddr + REG_UIC_COMMAND_ARG_1));
	writel(ufs->uic_cmd->uiccmdarg2, (ufs->ioaddr + REG_UIC_COMMAND_ARG_2));
	writel(ufs->uic_cmd->uiccmdarg3, (ufs->ioaddr + REG_UIC_COMMAND_ARG_3));
	writel(ufs->uic_cmd->uiccmdr, (ufs->ioaddr + REG_UIC_COMMAND));

	ufs->ufs_state = UFS_TRANSFER_UIC;
	ufs->timeout = ufs->uic_cmd_timeout;	/* usec unit */
	while (!(ufs->ufs_state & UFS_TRANSFER_COMPLETE) && err==0)
		err = handle_ufs_int(ufs);
	error_code = readl(ufs->ioaddr + REG_UIC_COMMAND_ARG_2);
	if (err || error_code) {
#ifdef SCSI_UFS_DEBUG

		printf("UFS(%d) UIC command error!\n\t%08x %08x %08x %08x\n\t%08x %08x %08x %08x\n",
		       ufs->host_index, ufs->uic_cmd->uiccmdr, ufs->uic_cmd->uiccmdarg1,
		       ufs->uic_cmd->uiccmdarg2, ufs->uic_cmd->uiccmdarg3,
		       readl(ufs->ioaddr + REG_UIC_COMMAND),
		       readl(ufs->ioaddr + REG_UIC_COMMAND_ARG_1)
		       , readl(ufs->ioaddr + REG_UIC_COMMAND_ARG_2),
		       readl(ufs->ioaddr + REG_UIC_COMMAND_ARG_3));
		//printf("err = 0x%08x, error_code = 0x%08x\n", err, error_code);
#endif

	}
	if (ufs->uic_cmd->uiccmdr == UIC_CMD_DME_GET
	    || ufs->uic_cmd->uiccmdr == UIC_CMD_DME_PEER_GET) {
		ufs->uic_cmd->uiccmdarg3 = readl(ufs->ioaddr + REG_UIC_COMMAND_ARG_3);
#ifdef SCSI_UFS_DEBUG
	//	if (ufs->uic_cmd->uiccmdarg1 = 0x15710000)
		printf
		    ("UFS(%d) UIC_CMD_DME_(PEER)_GET command\n\t%08x %08x %08x %08x\n\t%08x %08x %08x %08x\n",
		     ufs->host_index, ufs->uic_cmd->uiccmdr, ufs->uic_cmd->uiccmdarg1,
		     ufs->uic_cmd->uiccmdarg2, ufs->uic_cmd->uiccmdarg3,
		     readl(ufs->ioaddr + REG_UIC_COMMAND),
		     readl(ufs->ioaddr + REG_UIC_COMMAND_ARG_1)
		     , readl(ufs->ioaddr + REG_UIC_COMMAND_ARG_2),
		     readl(ufs->ioaddr + REG_UIC_COMMAND_ARG_3));
#endif
	}

	return error_code | err;
}

static int send_cmd(struct ufs_host *ufs)
{
	int err;
#ifdef UFS_ERROR_INJECTION
	if (!(get_timer(0) & 0xff)) {
		printf("UFS(%d) ERROR INJECTION!!!\n", ufs->host_index);
		ufs->ufs_state = UFS_TRANSFER_ERROR_RETRY;
		return UFS_ERROR_TIMEOUT;
	}
#endif
	switch (ufs->command_type & 0xff) {
	case UPIU_TRANSACTION_NOP_OUT:
	case UPIU_TRANSACTION_QUERY_REQ:
		writel((readl(ufs->ioaddr + 0x140) & ~0x01), (ufs->ioaddr + 0x140));
		break;
	case UPIU_TRANSACTION_COMMAND:
	case UPIU_TRANSACTION_TASK_REQ:
	default:
		break;
	}

	switch (ufs->command_type & 0xff) {
	case UPIU_TRANSACTION_NOP_OUT:
	case UPIU_TRANSACTION_COMMAND:
	case UPIU_TRANSACTION_QUERY_REQ:
		writel(1, (ufs->ioaddr + REG_UTP_TRANSFER_REQ_DOOR_BELL));
		ufs->ufs_state = UFS_TRANSFER_COMMAND;
		break;
	case UPIU_TRANSACTION_TASK_REQ:
		writel(1, (ufs->ioaddr + REG_UTP_TASK_REQ_DOOR_BELL));
		ufs->ufs_state = UFS_TRANSFER_TASK;
		break;
	}

	ufs->timeout = ufs->ufs_cmd_timeout;	/* usec unit */
	while (!(ufs->ufs_state & UFS_TRANSFER_COMPLETE))
		err = handle_ufs_int(ufs);

	switch (ufs->command_type & 0xff) {
	case UPIU_TRANSACTION_NOP_OUT:
	case UPIU_TRANSACTION_QUERY_REQ:
		writel((readl(ufs->ioaddr + 0x140) | 0x01), (ufs->ioaddr + 0x140));
		break;
	case UPIU_TRANSACTION_COMMAND:
		memcpy(ufs->scsi_cmd->sense_buf,
		       (u8 *) (ufs->cmd_desc_addr->response_upiu.data) + 2, 18);
		break;
	case UPIU_TRANSACTION_TASK_REQ:
	default:
		break;
	}

	if (err) {
		printf("UFS(%d) command timeout!\n", ufs->host_index);
	}
	return err;
}

static int ufs_parse_respnse(struct ufs_host *ufs)
{
	ccb *pccb = ufs->scsi_cmd;

	if (!ufs || !pccb)
		return FALSE;

	if (ufs->utrd_addr->dw[2]) {
		printf("SCSI CMD : %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
		       pccb->cmd[0], pccb->cmd[1], pccb->cmd[2], pccb->cmd[3]
		       , pccb->cmd[4], pccb->cmd[5], pccb->cmd[6], pccb->cmd[7]
		       , pccb->cmd[8], pccb->cmd[9]);
		pccb->contr_stat = SCSI_SEL_TIME_OUT;
		printf("SCSI Response(%02x) : ", ufs->cmd_desc_addr->response_upiu.header.response);
		if (ufs->cmd_desc_addr->response_upiu.header.response) {
			printf("Target Failure\n");
		} else {
			printf("Target success\n");
		}
		printf("SCSI Status(%02x) : ", ufs->cmd_desc_addr->response_upiu.header.status);
		switch (ufs->cmd_desc_addr->response_upiu.header.status) {
		case 0:
			printf("GOOD\n");
			break;
		case 2:
			printf("CHECK CONDITION\n");
			break;
		case 4:
			printf("CONDITION MET\n");
			break;
		case 8:
			printf("BUSY\n");
			break;
		case 0x18:
			printf("RESERVATION CONFLICT\n");
			break;
		case 0x28:
			printf("TASK SET FULL\n");
			break;
		case 0x30:
			printf("ACA ACTIVE\n");
			break;
		case 0x40:
			printf("TASK ABORTED\n");
			break;
		default:
			printf("reserved\n");
			break;
		}
		return FALSE;
	}
	return TRUE;
}

/*
	called by common/cmd_scsi.c
*/
int scsi_exec(ccb * pccb)
{
	struct ufs_host *ufs;
	int err, retry = 0;

	do {
		ufs = get_cur_ufs_host();
		ufs->scsi_cmd = pccb;
		ufs->sense_buffer = pccb->sense_buf;
		ufs->sense_buflen = 64;	/* defined in include/scsi.h */
		ufs->lun = pccb->lun;
		ufs->command_type = UFS_SEND_CMD;

		pccb->trans_bytes = 0;	/* no bytes transfered yet */

		memset(ufs->cmd_desc_addr, 0x00, sizeof(struct ufs_cmd_desc));

		ufs_compose_upiu(ufs);
		err = ufs_map_sg(ufs);
		if (err)
			return err;

		push_scsi_cmd_log(ufs->scsi_cmd);

		err = send_cmd(ufs);

#ifdef	SCSI_UFS_DEBUG
		print_ufs_upiu(UFS_DEBUG_UPIU);
#endif

		if (err) {
			/* Re-init UFS & retry transection */
			ufs->ufs_state = 0;
			ufs_link_startup(ufs);
			printf("%s: error %d, retrying %d ... \n", __func__, err, ++retry);
		} else {
			retry = 0;
		}
	} while (retry);

	return ufs_parse_respnse(ufs);
}

int ufs_edit_config_desc(u32 lun, u32 enable, u32 bootlun, u32 writeprotect, u32 type, u32 capacity)
{
	int i, luns = 0, boot = 0, flag = 0;
	u32 sum = 0;
	int res = CMD_RET_FAILURE;
	struct ufs_host *ufs = get_cur_ufs_host();

	/* For W-LUN */
	if (!ufs || (lun > 7 && (lun & (0x1 << 7))))
		return res;

	if (enable) {
		ufs->config_desc.unit[lun].bLUEnable = 1;
		ufs->config_desc.unit[lun].bBootLunID = bootlun;
		ufs->config_desc.unit[lun].bLUWriteProtect = writeprotect;
		ufs->config_desc.unit[lun].bMemoryType = type;
		if (bootlun)
			ufs->config_desc.unit[lun].bMemoryType = 0x3;
		if (capacity) {
			ufs->config_desc.unit[lun].dNumAllocUnits
			    =
			    cpu_to_be32((2048 * capacity) /
					(ufs->geometry_desc.bAllocationUnitSize *
					 be32_to_cpu(ufs->geometry_desc.dSegmentSize)));
		} else {
			ufs->config_desc.unit[lun].dNumAllocUnits = 0;
			flag = 1;
		}
	} else {
		ufs->config_desc.unit[lun].bLUEnable = 0;
		ufs->config_desc.unit[lun].bBootLunID = 0;
		ufs->config_desc.unit[lun].dNumAllocUnits = 0;
		ufs->config_desc.unit[lun].bLUWriteProtect = 0;
		ufs->config_desc.unit[lun].bMemoryType = 0;
	}

	for (i = 0; i < 8; i++) {
		if (ufs->config_desc.unit[i].bLUEnable)
			luns++;
		if (ufs->config_desc.unit[i].bBootLunID)
			boot = 1;
		sum += be32_to_cpu(ufs->config_desc.unit[i].dNumAllocUnits);
	}
	ufs->config_desc.header.bNumberLU = (luns ? luns : 1);
	ufs->config_desc.header.bBootEnable = boot;
	ufs->config_desc.header.bDescrAccessEn = 1;
	ufs->config_desc.header.bInitPowerMode = 1;
	ufs->config_desc.header.bSecureRemovalType = 0;	// this value may be different
	ufs->config_desc.header.bInitActiveICCLevel = 0xf;	// this value may be different

	if (flag)
		ufs->config_desc.unit[lun].dNumAllocUnits
		    =
		    cpu_to_be32((be32_to_cpu(ufs->geometry_desc.qTotalRawDeviceCapacity_l) /
				 be32_to_cpu(ufs->geometry_desc.dSegmentSize) - sum));

	return res;
}

static void ufs_setup_cmd(ccb * pccb, u8 scsi_cmd)
{
	pccb->cmd[0] = scsi_cmd;
	if (scsi_cmd < 0x20)
		pccb->cmdlen = 6;
	if (scsi_cmd >= 0x20 && scsi_cmd < 0x60)
		pccb->cmdlen = 10;
	if (scsi_cmd >= 0x80 && scsi_cmd < 0xa0)
		pccb->cmdlen = 16;
	if (scsi_cmd >= 0xa0 && scsi_cmd < 0xc0)
		pccb->cmdlen = 12;
}

int ufs_send_upiu(ufs_upiu_cmd cmd)
{
	int res = CMD_RET_FAILURE;
	struct ufs_host *ufs = get_cur_ufs_host();
	u8 *p;
	u8 index;
	u32 value;
	ccb tccb;

	if (!ufs)
		return res;

	ufs->command_type = cmd;

	switch (cmd & 0xff) {
	case UPIU_TRANSACTION_QUERY_REQ:
		p = ufs->cmd_desc_addr->response_upiu.header.tsf;
		memset(ufs->cmd_desc_addr, 0x00, sizeof(struct ufs_cmd_desc));
		ufs->scsi_cmd = 0;
		switch ((cmd >> 16) & 0xff) {
		case UPIU_QUERY_OPCODE_WRITE_DESC:
			switch ((cmd >> 24) & 0xff) {
			case UPIU_DESC_ID_CONFIGURATION:
				ufs->data_seg_len = sizeof(struct ufs_config_desc);
				memcpy(ufs->upiu_data, &ufs->config_desc, ufs->data_seg_len);
				printf("UPIU_QUERY_OPCODE_WRITE_DESC : len%02x\n",
				       ufs->data_seg_len);
				break;
			}
			break;
		case UPIU_QUERY_OPCODE_WRITE_ATTR:
			if (((cmd >> 24) & 0xff) == UPIU_ATTR_ID_BOOTLUNEN) {
				ufs->attributes.arry[UPIU_ATTR_ID_BOOTLUNEN] = 0x01;
				printf("UPIU_ATTR_ID_BOOTLUNEN : %08x\n",
				       ufs->attributes.arry[UPIU_ATTR_ID_BOOTLUNEN]);
			} else if (((cmd >> 24) & 0xff) == UPIU_ATTR_ID_CONFIGDESCLOCK) {
				ufs->attributes.arry[UPIU_ATTR_ID_CONFIGDESCLOCK] = 0x01;
				printf("UPIU_ATTR_ID_CONFIGDESCLOCK : %08x\n",
				       ufs->attributes.arry[UPIU_ATTR_ID_CONFIGDESCLOCK]);
			}
			break;
		}
		break;
	case UPIU_TRANSACTION_TASK_REQ:
		p = ufs->utmrd_addr->task_rsp_upiu.tsf;
		memset(ufs->utmrd_addr, 0x00, sizeof(struct ufs_utmrd));
		ufs->lun = (cmd >> 24) & 0xff;
		ufs->scsi_cmd = 0;
		break;
	case UPIU_TRANSACTION_COMMAND:
		memset(&tccb, 0x00, sizeof(ccb));
		memset(ufs->cmd_desc_addr, 0x00, sizeof(struct ufs_cmd_desc));
		ufs_setup_cmd(&tccb, (cmd >> 16) & 0xff);
		p = ufs->cmd_desc_addr->response_upiu.header.tsf;
		ufs->lun = (cmd >> 24) & 0xff;
		ufs->scsi_cmd = &tccb;
		ufs->sense_buffer = tccb.sense_buf;
		ufs->sense_buflen = 64;	/* defined in include/scsi.h */
		tccb.lun = ufs->lun;
		tccb.trans_bytes = 0;	/* no bytes transfered yet */
		break;
	default:
		break;
	}

	ufs_compose_upiu(ufs);
	ufs_map_sg(ufs);

#ifdef	SCSI_UFS_DEBUG
	print_ufs_upiu(UFS_DEBUG_UPIU_ALL2);
#endif

	res = send_cmd(ufs);

#ifdef	SCSI_UFS_DEBUG
	print_ufs_upiu(UFS_DEBUG_UPIU_ALL2);
#endif

/*
	command_type :
	| descriptor ID | opcode | function | UPIU type |

	UPIU type[7:0] : NOP_OUT, COMMAND, DATA_OUT, TASK_REQ, QUERY_REQ
	function[15:8] : Used by QUERY_REQ / TASK_REQ
	opcode[23:16] : Opcode in query / input parameter1 (TASK_REQ)
	descriptor ID[31:24] : descriptor ID in query / input parameter2 (TASK_REQ)

*/
	switch (cmd & 0xff) {
	case UPIU_TRANSACTION_QUERY_REQ:
		if (ufs->cmd_desc_addr->response_upiu.header.response) {
			printf("lun:%d Query Response : 0x%02x\n", ufs->lun,
			       ufs->cmd_desc_addr->response_upiu.header.response);
			break;
		}
		switch ((cmd >> 16) & 0xff) {
		case UPIU_QUERY_OPCODE_READ_DESC:
			switch ((cmd >> 24) & 0xff) {
			case UPIU_DESC_ID_UNIT:
				index = *((u8 *) (ufs->cmd_desc_addr->response_upiu.data) + 2);
				if (index > 7) {
					printf("bUnitIndex : 0x%02x\n", index);
					break;
				}
				memcpy(&ufs->unit_desc[index], ufs->cmd_desc_addr->response_upiu.data,
					min_t(u8, *(u8 *)(ufs->cmd_desc_addr->response_upiu.data),
					     sizeof(struct ufs_unit_desc)));
				break;
			case UPIU_DESC_ID_DEVICE:
				memcpy(&ufs->device_desc, ufs->cmd_desc_addr->response_upiu.data,
				       min_t(u8, *(u8 *) (ufs->cmd_desc_addr->response_upiu.data),
					     sizeof(struct ufs_device_desc)));
				//print_ufs_device_desc(&ufs->device_desc);
				break;
			case UPIU_DESC_ID_CONFIGURATION:
				memcpy(&ufs->config_desc, ufs->cmd_desc_addr->response_upiu.data,
				       min_t(u8, *(u8 *) (ufs->cmd_desc_addr->response_upiu.data),
					     sizeof(struct ufs_config_desc)));
				//print_ufs_configuration_desc(&ufs->config_desc);
				break;
			case UPIU_DESC_ID_GEOMETRY:
				memcpy(&ufs->geometry_desc, ufs->cmd_desc_addr->response_upiu.data,
				       min_t(u8, *(u8 *) (ufs->cmd_desc_addr->response_upiu.data),
					     sizeof(struct ufs_geometry_desc)));
				//print_ufs_geometry_desc(&ufs->geometry_desc);
				break;
			}
			//print_ufs_desc(ufs->cmd_desc_addr->response_upiu.data);
			break;
		case UPIU_QUERY_OPCODE_READ_ATTR:
			value = UPIU_HEADER_DWORD((u32) p[8], (u32) p[9], (u32) p[10], (u32) p[11]);
			index = p[1];
#ifdef SCSI_UFS_DEBUG
			printf("ATTRIBUTE IDN(0x%02x) = 0x%08x\n", index, value);
#endif
			ufs->attributes.arry[index] = value;
			break;
		case UPIU_QUERY_OPCODE_READ_FLAG:
			value = p[11];
			index = p[1];
#ifdef SCSI_UFS_DEBUG
			printf("FLAG IDN(0x%02x) = %x\n", index, value);
#endif
			ufs->flags.arry[index] = value;
			break;
		}
		break;
	case UPIU_TRANSACTION_TASK_REQ:
		if (ufs->utmrd_addr->task_rsp_upiu.response) {
			printf("Task Management Request Fail : 0x%02x 0x%02x\n",
			       ufs->utmrd_addr->task_rsp_upiu.response, p[3]);
			break;
		}
		break;
	case UPIU_TRANSACTION_COMMAND:
		ufs_parse_respnse(ufs);
		break;
	default:
		break;
	}

	return res;
}

int ufs_send_upiu_with_lun(ufs_upiu_cmd cmd, u8 lun)
{
	struct ufs_host *ufs = get_cur_ufs_host();
	ufs->lun = lun;

	return ufs_send_upiu(cmd);
}

static int ufs_mphy_unipro_setting(struct ufs_host *ufs, struct ufs_uic_cmd *uic_cmd_list)
{
	int res = 0;
	u32 timeout;

	if (!uic_cmd_list) {
		printf("%s: cmd list is empty\n", __func__);
		return res;
	}

	while (uic_cmd_list->uiccmdr) {
		//printf("[0x%08X 0x%08X]\n", uic_cmd_list->uiccmdr, uic_cmd_list->uiccmdarg1);
		ufs->uic_cmd = uic_cmd_list++;
		switch (ufs->uic_cmd->uiccmdr) {
		case UIC_CMD_WAIT:
			udelay(ufs->uic_cmd->uiccmdarg2);
			break;
		case UIC_CMD_WAIT_ISR:
			timeout = ufs->uic_cmd_timeout;
			while ((readl(ufs->ioaddr + ufs->uic_cmd->uiccmdarg1) &
				ufs->uic_cmd->uiccmdarg2) != ufs->uic_cmd->uiccmdarg2) {
				if (!timeout--) {
					res = CMD_RET_FAILURE;
					goto out;
				}
#if defined(CONFIG_UFS_LOW_SPEED)
				if ((ufs->uic_cmd->uiccmdr == 0x90)&&(ufs->uic_cmd->uiccmdarg1 == 0x20) && (ufs->uic_cmd->uiccmdarg2 == 0x10))
				{
					udelay(1000);
					printf("0x90 ignored !!\n");
					break;
				}
				else
					udelay(30);
#else
#if defined(CONFIG_UFS_ADD_WAITING_DELAY)
				udelay(30);
#else
				udelay(1);
#endif
#endif					
			}
			writel(ufs->uic_cmd->uiccmdarg2, (ufs->ioaddr + ufs->uic_cmd->uiccmdarg1));		
			break;
		case UIC_CMD_REGISTER_SET:
			writel(ufs->uic_cmd->uiccmdarg2, (ufs->ioaddr + ufs->uic_cmd->uiccmdarg1));
			break;
		case PHY_PMA_COMN_SET:
		case PHY_PMA_TRSV_SET:
			writel(ufs->uic_cmd->uiccmdarg2, (ufs->phy_pma + ufs->uic_cmd->uiccmdarg1));
			break;
		case PHY_PMA_COMN_WAIT:
		case PHY_PMA_TRSV_WAIT:
			timeout = ufs->uic_cmd_timeout;
			while ((readl(ufs->phy_pma + ufs->uic_cmd->uiccmdarg1) &
				ufs->uic_cmd->uiccmdarg2) != ufs->uic_cmd->uiccmdarg2) {
				if (!timeout--) {
					res = CMD_RET_FAILURE;
					goto out;
				}
#if defined(CONFIG_UFS_LOW_SPEED)				
				if ((ufs->uic_cmd->uiccmdr == 0xF2) && (ufs->uic_cmd->uiccmdarg1 == 0x78) && (ufs->uic_cmd->uiccmdarg2 == 0x20))
				{
					udelay(1000);
					printf("0xf2 ignored !!\n");
					break;
//					printf(".");
				}
				else if ((ufs->uic_cmd->uiccmdr == 0xF3) && (ufs->uic_cmd->uiccmdarg1 == 0x178) && (ufs->uic_cmd->uiccmdarg2 == 0x10))
				{
					udelay(1000);	
					printf("0xf3 ignored !!\n");
					break;
//					printf(".");
				}
				else
					udelay(30);
#else
#if defined(CONFIG_UFS_ADD_WAITING_DELAY)
				udelay(3000);
#else
				udelay(1);
#endif
#endif
			}
			break;
		default:
			//printf("%(in) <<====\n", __func__);
			res = send_uic_cmd(ufs);
			//printf("%(out) <<====\n", __func__);
			break;
		}
	}

out:
	if (res)
		printf("%s: failed cmd:0x%x arg1:0x%x arg2:0x%x with err %d\n",
			__func__, ufs->uic_cmd->uiccmdr,
			ufs->uic_cmd->uiccmdarg1,
			ufs->uic_cmd->uiccmdarg2, res);
	return res;
}

static int ufs_check_2lane(struct ufs_host *ufs)
{
	int res = CMD_RET_FAILURE;
	int tx, rx;
	struct ufs_uic_cmd tx_cmd = { UIC_CMD_DME_GET, (0x1561 << 16), 0, 0 };	/* PA_ConnectedTxDataLane */
	struct ufs_uic_cmd rx_cmd = { UIC_CMD_DME_GET, (0x1581 << 16), 0, 0 };	/* PA_ConnectedRxDataLane */
	struct ufs_uic_cmd ufs_2lane_cmd[] = {
		{UIC_CMD_DME_SET, (0x1560 << 16), 0, 2}	/* PA_ActiveTxDataLanes */
		, {UIC_CMD_DME_SET, (0x1580 << 16), 0, 2}	/* PA_ActiveRxDataLanes */
		, {UIC_CMD_DME_SET, (0x1571 << 16), 0, 0x55}	/* PA_PWRMode */
		/* NULL TERMINATION */
		, {0, 0, 0, 0}
	};
	if (!ufs)
		return res;

	if (ufs->quirks & UFS_QUIRK_USE_1LANE)
		return res;

	ufs->uic_cmd = &tx_cmd;
	res = send_uic_cmd(ufs);
	if (res)
		goto out;
	tx = ufs->uic_cmd->uiccmdarg3;

	ufs->uic_cmd = &rx_cmd;
	send_uic_cmd(ufs);
	if (res)
		goto out;
	rx = ufs->uic_cmd->uiccmdarg3;

	if (tx == 2 && rx == 2) {
		res = ufs_mphy_unipro_setting(ufs, ufs_2lane_cmd);
		if (res) {
			printf("trying to use 2 lane connection, but fail...\n");
			goto out;
		}
		while ((readl(ufs->ioaddr + REG_INTERRUPT_STATUS) & (1 << 4)) == 0) ;
		writel((1 << 4), ufs->ioaddr + REG_INTERRUPT_STATUS);
		while ((readl(ufs->ioaddr + REG_INTERRUPT_STATUS) & (1 << 4))) ;
		printf("Use 2 lane connection\n");
	}

 out:
	return res;
}

static int ufs_end_boot_mode(struct ufs_host *ufs)
{
	int flag = 1, retry = 1000;
	int res;

	res = ufs_send_upiu(UFS_SEND_NOP);
	if (res) {
		printf("UFS: NOP OUT failed\n");
		return res;
	}

	ufs->flags.arry[UPIU_FLAG_ID_DEVICEINIT] = flag;
	res = ufs_send_upiu_with_lun(UFS_SEND_SET_DEVICEINIT_FLAG, 0);
	if (res)
		return res;

	do {
		ufs_send_upiu_with_lun(UFS_SEND_READ_DEVICEINIT_FLAG, 0);
		flag = ufs->flags.arry[UPIU_FLAG_ID_DEVICEINIT];
		retry--;
	} while (flag && retry > 0);

	if (flag) {
		printf("UFS: fdeviceinit faild\n");
		return CMD_RET_FAILURE;
	}

	return CMD_RET_SUCCESS;
}

int ufs_link_startup(struct ufs_host *ufs)
{
	struct ufs_uic_cmd uic_cmd = { UIC_CMD_DME_LINK_STARTUP, };
	int res = CMD_RET_FAILURE;
#ifndef UFS_FIRMWARE_UPDATE
	if ((readl(ufs->ioaddr + REG_CONTROLLER_STATUS) & 0xf) != 0xf) {
#endif
		if (ufs->ufs_board_pre_setup) {
			res = ufs->ufs_board_pre_setup(ufs);
			if (res)
				goto out;
		}
		res = ufs_mphy_unipro_setting(ufs, ufs->pre_uic_cmd);
		if (res)
			goto out;
		ufs->uic_cmd = &uic_cmd;
		res = send_uic_cmd(ufs);
		if (res) {
			printf("UFS%d linkstartup error!\n", ufs->host_index);
			goto out;
		}

		printf("link startup success!!!\n");

		res = ufs_mphy_unipro_setting(ufs, ufs->post_uic_cmd);
		if (res)
			goto out;
		printf("wait ufs to be initialized.......\n");
		if (ufs->ufs_board_post_setup) {
			res = ufs->ufs_board_post_setup(ufs);
			if (res)
				goto out;
		}
		res = ufs_end_boot_mode(ufs);
		if (res)
			goto out;
		//ufs->ufs_board_pre_setup(ufs);
		res = ufs_mphy_unipro_setting(ufs, ufs->pwr_mode_change);

#ifndef UFS_FIRMWARE_UPDATE
	}
#endif
 out:
	return res;
}

int ufs_mem_init(struct ufs_host *ufs)
{
	if (!ufs->cmd_desc_addr) {
		ufs->cmd_desc_addr = memalign(0x1000, UFS_NUTRS * sizeof(struct ufs_cmd_desc));
		if (!ufs->cmd_desc_addr) {
			printf("cmd_desc_addr memory alloc error!!!\n");
			goto out;
		}
		if ((u32) (ufs->cmd_desc_addr) & 0xfff) {
			printf("allocated cmd_desc_addr memory align error!!!\n");
			goto free_ucd_out;
		}
	}


	ufs_debug("cmd_desc_addr : %x\n", ufs->cmd_desc_addr);
	ufs_debug("\tresponse_upiu : %x\n", &ufs->cmd_desc_addr->response_upiu);
	ufs_debug("\tprd_table : %x (size=%x)\n", ufs->cmd_desc_addr->prd_table,
		  sizeof(ufs->cmd_desc_addr->prd_table));
	ufs_debug("\tsizeof upiu : %x\n", sizeof(struct ufs_upiu));

	memset(ufs->cmd_desc_addr, 0x00, UFS_NUTRS * sizeof(struct ufs_cmd_desc));

	if (!ufs->utrd_addr) {
		ufs->utrd_addr = memalign(0x1000, UFS_NUTRS * sizeof(struct ufs_utrd));
		if (!ufs->utrd_addr) {
			printf("utrd_addr memory alloc error!!!\n");
			goto free_ucd_out;
		}
		if ((u32) (ufs->utrd_addr) & 0xfff) {
			printf("allocated utrd_addr memory align error!!!\n");
			goto free_utrd_out;
		}
	}
	ufs_debug("utrd_addr : %x\n", ufs->utrd_addr);
	memset(ufs->utrd_addr, 0x00, UFS_NUTRS * sizeof(struct ufs_utrd));

	if (!ufs->utmrd_addr) {
		ufs->utmrd_addr = memalign(0x1000, UFS_NUTMRS * sizeof(struct ufs_utmrd));
		if (!ufs->utmrd_addr) {
			printf("utmrd_addr memory alloc error!!!\n");
			goto free_utrd_out;
		}
		if ((u32) (ufs->utmrd_addr) & 0xfff) {
			printf("allocated utmrd_addr memory align error!!!\n");
			goto free_all_out;
		}
	}
	ufs_debug("utmrd_addr : %x\n", ufs->utmrd_addr);
	memset(ufs->utmrd_addr, 0x00, UFS_NUTMRS * sizeof(struct ufs_utmrd));

	ufs->utrd_addr->cmd_desc_addr_l = cpu_to_le32(ufs->cmd_desc_addr);
	ufs->utrd_addr->rsp_upiu_off = cpu_to_le16(offsetof(struct ufs_cmd_desc, response_upiu));
	ufs->utrd_addr->rsp_upiu_len = cpu_to_le16(ALIGNED_UPIU_SIZE);

	writel(ufs->utmrd_addr, (ufs->ioaddr + REG_UTP_TASK_REQ_LIST_BASE_L));
	writel(0, (ufs->ioaddr + REG_UTP_TASK_REQ_LIST_BASE_H));

	writel(ufs->utrd_addr, (ufs->ioaddr + REG_UTP_TRANSFER_REQ_LIST_BASE_L));
	writel(0, (ufs->ioaddr + REG_UTP_TRANSFER_REQ_LIST_BASE_H));

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

	return CMD_RET_SUCCESS;

 free_all_out:
	free(ufs->utmrd_addr);

 free_utrd_out:
	free(ufs->utrd_addr);

 free_ucd_out:
	free(ufs->cmd_desc_addr);

 out:
	return CMD_RET_FAILURE;

}

static void ufs_mem_clear(struct ufs_host *ufs)
{
	free(ufs->utmrd_addr);
	free(ufs->utrd_addr);
	free(ufs->cmd_desc_addr);
}

static void ufs_identify_bootlun()
{
	struct ufs_host *ufs = get_cur_ufs_host();
	int boot_lun_en;
	int i;

	ufs_send_upiu(UFS_SEND_READ_BOOTLUNEN_ATTR);
	boot_lun_en = ufs->attributes.arry[UPIU_ATTR_ID_BOOTLUNEN];
	if (boot_lun_en == 0) {
		printf("Boot LUN is disabled\n");
		return;
	}

	for (i = 0; i < 8; i++) {
		ufs_send_upiu_with_lun(UFS_SEND_READ_UNIT_DESC, i);
		if (boot_lun_en == ufs->unit_desc[i].bBootLunID) {
			set_bootLUN(i);
			printf("Boot LUN is #%d, bBootLunID:%d\n", i, ufs->unit_desc[i].bBootLunID);
		}
	}
}

static void ufs_disable_ufsp(struct ufs_host *ufs)
{
        writel(0x0, ufs->ioaddr + UFSP_UPSBEGIN0);
        writel(0xffffffff, ufs->ioaddr + UFSP_UPSEND0);
        writel(0xff, ufs->ioaddr + UFSP_UPLUN0);
        writel(0xf1, ufs->ioaddr + UFSP_UPSCTRL0);

}

/*
	2. host & memory init
*/
static int ufs_host_init(int host_index, struct ufs_host *ufs)
{
	int rst_cnt = 0;
	int res;
	/* command timeout may be redefined in ufs_board_init()  */
	ufs->ufs_cmd_timeout = 50000;	/* 100msec */
	ufs->uic_cmd_timeout = 10000;	/* 100msec */

	/* AP specific UFS host init */
	if (ufs_board_init(host_index, ufs))
		goto out;
	/* Read capabilities registers */
	ufs->capabilities = readl(ufs->ioaddr + REG_CONTROLLER_CAPABILITIES);
	ufs->ufs_version = readl(ufs->ioaddr + REG_UFS_VERSION);
	ufs_debug
	    ("%s\n\tcaps(0x%X) 0x%08X\n\tver(0x%X)  0x%08X\n\tPID(0x%X)  0x%08X\n\tMID(0x%X)  0x%08X\n",
	     ufs->host_name, ufs->ioaddr + REG_CONTROLLER_CAPABILITIES, ufs->capabilities,
	     ufs->ioaddr + REG_UFS_VERSION, ufs->ufs_version, ufs->ioaddr + REG_CONTROLLER_PID,
	     readl(ufs->ioaddr + REG_CONTROLLER_PID)
	     , ufs->ioaddr + REG_CONTROLLER_MID, readl(ufs->ioaddr + REG_CONTROLLER_MID));

	if (ufs_mem_init(ufs))
		goto out;
	/* Encryption by-passed */
	ufs_disable_ufsp(ufs);
	do {
		res = ufs_link_startup(ufs);
		if (!res)
			break;
		rst_cnt++;
		printf(" Retry Link Startup CNT : %d\n", rst_cnt);
	} while (rst_cnt < 10);

	if (res)
	{
		printf("Link Startup failed !!!!\n");
		goto freeout;
	}
	
	return CMD_RET_SUCCESS;
 freeout:
	ufs_mem_clear(ufs);

 out:
	return CMD_RET_FAILURE;
}

/*
	1. called by arch/lib/board.c
*/
int ufs_init(int mode)
{
	int res = 0, i;
	if ((mode == 2) && (exynos_boot_mode() != BOOT_UFS)) {
		printf(" Not UFS boot mode. Init UFS manually.\n");
		return res;
	}
	
	for (i = 0; i < CONFIG_SYS_SCSI_MAX_SCSI_ID; i++) {
		if (!_ufs[i]) {
			_ufs[i] = malloc(sizeof(struct ufs_host));
			if (_ufs[i])
				memset(_ufs[i], 0x00, sizeof(struct ufs_host));
			else
				goto errout;
		}
		res = ufs_host_init(i, _ufs[i]);
		if (res)
			goto errfree;
	}
	ufs_identify_bootlun();

	scsi_scan(mode);

	return res;

 errfree:
	do {
		free(_ufs[i]);
	} while (i--);

 errout:
	return res;
}

#endif				/* #if defined(CONFIG_CMD_SCSI) && defined(CONFIG_SCSI_UFS) */
