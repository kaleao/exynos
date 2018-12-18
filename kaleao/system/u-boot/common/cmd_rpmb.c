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
#include <asm/io.h>
#include <common.h>
#include <command.h>
#include <scsi.h>
#include <malloc.h>
#include <rpmb.h>

static void dump_packet(u8 * data, int len)
{
	u8 s[17];
	u32 i, j;
#ifdef RPMB_DBG
	s[16] = '\0';
	for (i = 0; i < len; i += 16) {
		printf("%06X :", i);
		for (j = 0; j < 16; j++) {
			printf(" %02X", data[i + j]);
			s[j] = (data[i + j] < ' ' ? '.' : (data[i + j] > '}' ? '.' : data[i + j]));
		}
		printf(" |%s|\n", s);
	}
	printf("\n");
#endif
}

static void swap_packet(u8 * p, u8 * d)
{
	int i;

	for (i = 0; i < 512; i++)
		d[i] = p[511 - i];
}

/* SMC function only for CryptoManager wrapper function */
static inline uint32_t cm_smc(uint32_t * cmd, uint32_t * arg1, uint32_t * arg2, uint32_t * arg3)
{
	register uint64_t reg0 __asm__("x0") = *cmd;
	register uint64_t reg1 __asm__("x1") = *arg1;
	register uint64_t reg2 __asm__("x2") = *arg2;
	register uint64_t reg3 __asm__("x3") = *arg3;

	__asm__ volatile ("stp	x29, x30, [sp, #-16]!\n"
			  "smc	0\n"
			  "ldp	x29, x30, [sp], #16\n":"+r" (reg0),
			  "+r"(reg1), "+r"(reg2), "+r"(reg3)

	    );

	*cmd = (uint32_t) reg0;
	*arg1 = (uint32_t) reg1;
	*arg2 = (uint32_t) reg2;
	*arg3 = (uint32_t) reg3;

	return (uint32_t) reg0;
}

/* print byte string to hexa values */
static void print_byte_to_hex(uint8_t * byte_array, size_t byte_length)
{
	int i;
	printf("\n");
	for (i = 0; i < byte_length; i++) {
		if ((0 != i) && (0 == (i % 16)))
			printf(" \n");
		printf(" %02X", byte_array[i]);
	}
	printf("\n");
}

/*
 * function to get RPMB raw keys from the REK-based KDF in CryptoManager.
 * @ key_len: RPMB key length in byte (this values is fixed to 32 for now)
 * @ rpmb_key: Pointer of the output buffer to store the RPMB raw key
 */
uint32_t get_RPMB_key(size_t key_len, uint8_t * rpmb_key)
{
	uint32_t r0, r1, r2, r3;
	uint32_t retry_cnt = 0;
	uint32_t ret = RV_SUCCESS;

#ifdef CACHE_ENABLED
	CACHE_CLEAN_INVALIDATE(rpmb_key, key_len);
#endif
	do {
		if (++retry_cnt > MAX_SMC_RETRY_CNT) {
			ret = RV_BOOT_RPMB_EXCEED_SMC_RETRY_CNT;
			printf("[CM] RPMB: exceed maximum SMC retry count\n");
			return ret;
		}

		r0 = SMC_AARCH64_PREFIX | SMC_CM_RPMB;
		r1 = RPMB_GET_KEY;
		r2 = RPMB_KEY_LEN;
		r3 = (uint32_t) rpmb_key;

		ret = cm_smc(&r0, &r1, &r2, &r3);

	} while (ret == RV_SYNC_AES_BUSY);

	if (ret != RV_SUCCESS) {
		printf("[CM] RPMB: error in CryptoManager F/W: 0x%X\n", ret);
		return ret;
	}
#ifdef CACHE_ENABLED
	CACHE_CLEAN_INVALIDATE(rpmb_key, key_len);
#endif
#ifdef CM_DEBUG
	printf("[CM] RPMB: key was derived successfully\n");
#endif
	return ret;
}

/*
 * function to block get_PRMB_key()
 * this function can be used when the caller needs to block use of get_RPMB_key()
 */
uint32_t block_RPMB_key(void)
{
	uint32_t r0, r1, r2, r3;
	uint32_t ret = RV_SUCCESS;

	r0 = SMC_AARCH64_PREFIX | SMC_CM_RPMB;
	r1 = RPMB_BLOCK_KEY;

	ret = cm_smc(&r0, &r1, &r2, &r3);

	if (ret != RV_SUCCESS) {
		printf("[CM] RPMB: failed to block key: 0x%X\n", ret);
		return ret;
	}
#ifdef CM_DEBUG
	printf("[CM] RPMB: key was blocked successfully\n");
#endif
	return ret;
}

/*
 * function to get hmac values from CryptoManager F/W with RPMB frame data.
 * @ input_data: pointer of the Input buffer storing RPMB frame data
 * @ input_len: input length in bytes (Maximum is 320)
 * @ output_data: pointer of the output buffer to store hmac-sha256 value
 */
uint32_t get_RPMB_hmac(uint8_t * input_data, size_t input_len, uint8_t * output_data)
{
	uint32_t r0, r1, r2, r3;
	uint32_t retry_cnt = 0;
	uint32_t ret = RV_SUCCESS;

	rpmb_param rpmb_data;

	rpmb_data.input_data = (uint32_t) input_data;
	rpmb_data.input_len = input_len;
	rpmb_data.output_data = (uint32_t) output_data;

#ifdef CACHE_ENABLED
	CACHE_CLEAN(input_data, input_len);
	CACHE_CLEAN_INVALIDATE(output_data, RPMB_HMAC_LEN);
#endif

	/* call REK-based KBKDF in CryptoManager F/W with smc */
	do {
		if (++retry_cnt > MAX_SMC_RETRY_CNT) {
			ret = RV_BOOT_RPMB_EXCEED_SMC_RETRY_CNT;
			printf("[CM] RPMB: exceed maximum SMC retry count\n");
			return ret;
		}

		r0 = SMC_AARCH64_PREFIX | SMC_CM_RPMB;
		r1 = RPMB_GET_HMAC;
		r2 = (uint32_t) & rpmb_data;

		ret = cm_smc(&r0, &r1, &r2, &r3);

	} while (ret == RV_SYNC_AES_BUSY);

	if (ret != RV_SUCCESS) {
		printf("[CM] RPMB: error in CryptoManager F/W: 0x%X\n", ret);
		return ret;
	}
#ifdef CACHE_ENABLED
	CACHE_CLEAN_INVALIDATE(output_data, RPMB_HMAC_LEN);
#endif
#ifdef CM_DEBUG
	printf("[CM] RPMB: hmac calculation was finished successfully\n");
#endif
	return ret;
}

/*
 * function to block get_PRMB_hmac()
 * this function can be used when the caller needs to block use of get_RPMB_hmac()
 */
uint32_t block_RPMB_hmac(void)
{
	uint32_t r0, r1, r2, r3;
	uint32_t ret = RV_SUCCESS;

	r0 = SMC_AARCH64_PREFIX | SMC_CM_RPMB;
	r1 = RPMB_BLOCK_HMAC;

	ret = cm_smc(&r0, &r1, &r2, &r3);

	if (ret != RV_SUCCESS) {
		printf("[CM] RPMB: failed to block hmac: 0x%X\n", ret);
		return ret;
	}
#ifdef CM_DEBUG
	printf("[CM] RPMB: hmac was blocked successfully\n");
#endif
	return ret;
}

/* example function to test check above functions work correctly */
int do_rpmb_test(cmd_tbl_t * cmdtp, int flag, int argc, char *const argv[])
{
	uint8_t rpmb_key[RPMB_KEY_LEN];
	uint32_t ret = RV_SUCCESS;

	static uint8_t input_data[32] = {
		0xA2, 0x8C, 0xF4, 0x31, 0x30, 0xEE, 0x69, 0x6A, 0x98, 0xF1,
		0x4A, 0x37, 0x67, 0x8B, 0x56, 0xBC, 0xFC, 0xBD, 0xD9, 0xE5,
		0xCF, 0x69, 0x71, 0x7F, 0xEC, 0xF5, 0x48, 0x0F, 0x0E, 0xBD,
		0xF7, 0x90
	};

	uint8_t output_data[32];

	/* Test 1: RPMB key derivation */
	printf("[CM] RPMB: Test1:: key derivation\n");

	ret = get_RPMB_key(RPMB_KEY_LEN, rpmb_key);
	if (ret != RV_SUCCESS) {
		printf("[CM] RPMB: key derivation: fail: 0x%X\n", ret);
		return ret;
	}
	printf("[CM] RPMB: key derivation: success\n");
	printf("[CM] RPMB: key: ");
	print_byte_to_hex(rpmb_key, RPMB_KEY_LEN);
	printf("\n");

	/* Test 2: RPMB key blocking */
	printf("[CM] RPMB: Test2:: key blocking\n");

	ret = block_RPMB_key();
	if (ret != RV_SUCCESS) {
		printf("[CM] RPMB: key blocking: fail: 0x%X\n", ret);
		return ret;
	}
	printf("[CM] RPMB: key blocking: success\n\n");

	/* Test 3: RPMB key derivation after key blocking */
	printf("[CM] RPMB: Test3:: key derivation after key blocking\n");

	ret = get_RPMB_key(RPMB_KEY_LEN, rpmb_key);
	if (ret == RV_RPMB_KEY_BLOCKED)
		printf("[CM] RPMB: key was blocked already: 0x%X\n\n", ret);
	else
		printf("[CM] RPMB: key wasn't blocked correctly: 0x%X\n\n", ret);

	/* Test 4: RPMB get hamc with input data */
	printf("[CM] RPMB: Test4:: get hmac value\n");

	memset(output_data, 0, sizeof(output_data));
	ret = get_RPMB_hmac(input_data, sizeof(input_data), output_data);
	if (ret != RV_SUCCESS) {
		printf("[CM] RPMB: get hamc value: fail: 0x%X\n", ret);
		return ret;
	}
	printf("[CM] RPMB: get hmac value: success\n");
	printf("[CM] RPMB: HMAC: ");
	print_byte_to_hex(output_data, RPMB_HMAC_LEN);
	printf("\n");

	/* Test 5: RPMB hmac blocking */
	printf("[CM] RPMB: Test5:: hmac blocking\n");

	ret = block_RPMB_hmac();
	if (ret != RV_SUCCESS) {
		printf("[CM] RPMB: hmac blocking: fail: 0x%X\n", ret);
		return ret;
	}
	printf("[CM] RPMB: hmac blocking: success\n\n");

	/* Test 6: RPMB hmac after hmac blocking */
	printf("[CM] RPMB: Test6:: get hmac after hmac blocking\n");

	memset(output_data, 0, sizeof(output_data));
	ret = get_RPMB_hmac(input_data, sizeof(input_data), output_data);

	if (ret == RV_RPMB_HMAC_BLOCKED)
		printf("[CM] RPMB: hmac was blocked already: 0x%X\n", ret);
	else
		printf("[CM] RPMB: hmac wasn't blocked correctly: 0x%X\n", ret);

	return ret;
}

U_BOOT_CMD(rpmb_test, 1, 0, do_rpmb_test, "Test for RPMB", "");

static void ufs_upiu_report(struct rpmb_packet *packet, int sp_in_out)
{
	char *in_out;

	in_out = sp_in_out ? "SECURITY PROTOCOL OUT" : "SECURITY PROTOCOL IN";

	printf("RPMB: Auth. %s counter %d\n", in_out, packet->write_counter);
	printf("RPMB: Auth. %s address %x\n", in_out, packet->address);
	printf("RPMB: Auth. %s block count %d\n", in_out, packet->count);
	printf("RPMB: Auth. %s result %d\n", in_out, packet->result);
	printf("RPMB: Auth. %s response %x\n", in_out, packet->request);
}

static int ufs_rpmb_commands(int dev_num, struct rpmb_packet *packet)
{
	u8 *buf, *hmac;
	int i, result = -1;
	u32 addr, start_blk, blk_cnt, w_counter;
	ulong test_key = 0;
	uint8_t rpmb_key[RPMB_KEY_LEN];
	uint8_t output_data[HMAC_SIZE];
	uint32_t ret = RV_SUCCESS;

	/* RPMB LUN number */
	dev_num = 0xC4;

	switch (packet->request) {
	case 1:
		/* Authentication key programming request */
		printf("Write authentication KEY\n");
		dump_packet(packet->Key_MAC, HMAC_SIZE);

		buf = malloc(RPMB_SIZE);
		swap_packet((u8 *) packet, buf);

		/* Key restore */
		memcpy((void *)(buf + HMAC_START_BYTE), packet->Key_MAC, HMAC_SIZE);

		printf("Authentication key programming request Packet\n");
		dump_packet((u8 *) packet, RPMB_SIZE);

		printf("Authentication key programming request Packet (Swapped)\n");
		dump_packet(buf, RPMB_SIZE);

		/* Send UPIU */
		scsi_rpmb_out(dev_num, 0, 1, buf);
		ufs_upiu_report(packet, SECU_PROT_OUT);

		memset((void *)packet, 0, RPMB_SIZE);
		packet->request = 0x05;
		swap_packet((u8 *) packet, buf);

		printf("Result read request\n");
		dump_packet((u8 *) packet, RPMB_SIZE);

		printf("Result read request (Swapped)\n");
		dump_packet(buf, RPMB_SIZE);

		/* Send UPIU */
		scsi_rpmb_out(dev_num, 0, 1, buf);
		ufs_upiu_report(packet, SECU_PROT_OUT);

		memset((void *)buf, 0, RPMB_SIZE);

		/* Send UPIU */
		scsi_rpmb_in(dev_num, 0, 1, buf);

		swap_packet(buf, (u8 *) packet);
		printf("Authentication key programming response\n");
		dump_packet(buf, RPMB_SIZE);

		printf("Authentication key programming response (Swapped)\n");
		dump_packet((u8 *) packet, RPMB_SIZE);
		ufs_upiu_report(packet, SECU_PROT_IN);
		break;

	case 2:
		/* Reading of the Write Counter value request */
		buf = malloc(RPMB_SIZE);
		swap_packet((u8 *) packet, buf);

		printf("Reading of the Write Counter value request\n");
		dump_packet((u8 *) packet, RPMB_SIZE);

		printf("Reading of the Write Counter value request (Swapped)\n");
		dump_packet(buf, RPMB_SIZE);

		/* Send UPIU */
		scsi_rpmb_out(dev_num, 0, 1, buf);
		ufs_upiu_report(packet, SECU_PROT_OUT);

		memset((void *)buf, 0, RPMB_SIZE);

		/* Send UPIU */
		scsi_rpmb_in(dev_num, 0, 1, buf);

		swap_packet(buf, (u8 *) packet);

		printf("Reading of the Write Counter value response\n");
		dump_packet(buf, RPMB_SIZE);

		printf("Reading of the Write Counter value response (Swapped)\n");
		dump_packet((u8 *) packet, RPMB_SIZE);

		printf("RPMB: MAC\n");
		dump_packet(packet->Key_MAC, HMAC_SIZE);

		ufs_upiu_report(packet, SECU_PROT_IN);
		break;

	case 3:
		/* Authenticated data write request */
		addr = *(u32 *) (packet->data);
		blk_cnt = packet->count;
		start_blk = packet->address;
		w_counter = packet->write_counter;
		buf = malloc(RPMB_SIZE * blk_cnt);
		hmac = malloc(HMAC_CALC_SIZE * blk_cnt);

		printf("Authenticated data write quest (Data only)\n");
		dump_packet((u8 *) addr, DATA_SIZE * blk_cnt);

		printf("Authenticated data write quest (Swapped)\n");
		printf("HMAC calculatation\n");
		memset(output_data, 0, sizeof(output_data));

		/* Write Data reordering */
		for (i = 0; i < blk_cnt; i++) {
			/* Copy to packet data from buffer */
			memcpy(packet->data, (void *)(addr + (i * DATA_SIZE)), DATA_SIZE);

			/* Swap Copy to buffer from packet */
			swap_packet((u8 *) packet, (u8 *) (buf + (i * RPMB_SIZE)));

			/* Multiple block Data merge for hmac calculation. */
			memcpy((void *)(hmac + (i * HMAC_CALC_SIZE)),
			       (void *)(buf + DATA_START_BYTE + (i * RPMB_SIZE)), HMAC_CALC_SIZE);
		}

		/* hmac calculation include all block data. */
		ret = get_RPMB_hmac(hmac, blk_cnt * HMAC_CALC_SIZE, output_data);
		dump_packet((u8 *) (buf + (blk_cnt - 1) * RPMB_SIZE), RPMB_SIZE);

		/* Write hmac to last block */
		memcpy((void *)(buf + HMAC_START_BYTE + (blk_cnt - 1) * RPMB_SIZE),
		       (void *)(output_data), HMAC_SIZE);
		memcpy((void *)(output_data), (void *)(packet->Key_MAC), HMAC_SIZE);
		dump_packet((u8 *) (buf + (blk_cnt - 1) * RPMB_SIZE), RPMB_SIZE);

		if (ret != RV_SUCCESS)
			printf("[CM] RPMB: get hamc value: fail: 0x%X\n", ret);
		else {
			printf("[CM] RPMB: get hmac value: success\n");
			printf("[CM] RPMB: HMAC: ");
			print_byte_to_hex(output_data, RPMB_HMAC_LEN);
			printf("\n");
		}

		printf("Authenticated data write quest (Swapped & HMAC included)\n");
		dump_packet((u8 *) (buf + ((blk_cnt - 1) * RPMB_SIZE)), RPMB_SIZE);

		/* HMAC calculation here */
		printf("Send authenticated data write quest\n");

		/* Send UPIU */
		scsi_rpmb_out(dev_num, start_blk, blk_cnt, buf);
		ufs_upiu_report(packet, SECU_PROT_OUT);

		memset((void *)packet, 0, RPMB_SIZE);
		packet->request = 0x05;
		swap_packet((u8 *) packet, buf);

		printf("Result write request\n");
		dump_packet((u8 *) packet, RPMB_SIZE);

		printf("Result write request (Swapped)\n");
		dump_packet((u8 *) buf, RPMB_SIZE);

		/* Send UPIU */
		scsi_rpmb_out(dev_num, 0, 1, buf);
		ufs_upiu_report(packet, SECU_PROT_OUT);
		memset((void *)buf, 0, RPMB_SIZE);

		/* Send UPIU */
		scsi_rpmb_in(dev_num, 0, 1, buf);

		swap_packet(buf, (u8 *) packet);
		printf("Authenticated data write response\n");

		dump_packet(buf, RPMB_SIZE);
		printf("Authenticated data write response (Swapped)\n");

		dump_packet((u8 *) packet, RPMB_SIZE);
		ufs_upiu_report(packet, SECU_PROT_IN);

		printf("RPMB: MAC\n");
		dump_packet(packet->Key_MAC, HMAC_SIZE);
		free(hmac);
		break;

	case 4:
		/* Authenticated data read request */
		addr = *(u32 *) (packet->data);
		blk_cnt = *((u32 *) (packet->data) + 1);
		start_blk = packet->address;
		*(u32 *) (packet->data) = 0;
		*((u32 *) (packet->data) + 1) = 0;

		buf = malloc(RPMB_SIZE * blk_cnt);
		hmac = malloc(HMAC_CALC_SIZE * blk_cnt);
		swap_packet((u8 *) packet, buf);

		printf("Authenticated data read request\n");
		dump_packet((u8 *) packet, RPMB_SIZE);

		printf("Authenticated data read request (Swapped)\n");
		dump_packet(buf, RPMB_SIZE);

		/* Send UPIU */
		scsi_rpmb_out(dev_num, 0, 1, buf);
		ufs_upiu_report(packet, SECU_PROT_OUT);

		memset((void *)buf, 0, RPMB_SIZE * blk_cnt);

		/* Send UPIU */
		scsi_rpmb_in(dev_num, start_blk, blk_cnt, buf);

		printf("Authenticated data read response (Swapped)\n");
		printf("HMAC calculatation\n");
		memset(output_data, 0, sizeof(output_data));

		/* Read hmac Data reordering */
		for (i = 0; i < blk_cnt; i++) {
			memcpy((void *)(hmac + (i * HMAC_CALC_SIZE)),
			       (void *)(buf + DATA_START_BYTE + (i * RPMB_SIZE)), HMAC_CALC_SIZE);
		}

		ret = get_RPMB_hmac(hmac, blk_cnt * HMAC_CALC_SIZE, output_data);
		result = memcmp((void *)(output_data),
				(void *)(buf + HMAC_START_BYTE + ((blk_cnt - 1) * RPMB_SIZE)),
				HMAC_SIZE);

		/* Read Data reordering */
		for (i = 0; i < blk_cnt; i++) {
			swap_packet((u8 *) (buf + (i * RPMB_SIZE)), (u8 *) packet);
			memcpy((void *)(addr + (i * DATA_SIZE)), packet->data, DATA_SIZE);
		}

		if (ret != RV_SUCCESS)
			printf("RPMB: get hamc value: fail: 0x%X\n", ret);
		else
			printf("RPMB: get hmac value: success\n");

		if (result != 0) {
			printf("HMAC compare fail !!\n");
			printf("HMAC Host value\n");
			dump_packet(output_data, HMAC_SIZE);

			printf("HMAC Device value blk_cnt %d i %d\n", blk_cnt, i);
			dump_packet((void *)(buf + HMAC_START_BYTE + ((blk_cnt - 1) * RPMB_SIZE)),
				    HMAC_SIZE);

			printf("Authenticated data read response (Not Swapped)\n");
			dump_packet((void *)(buf + (blk_cnt - 1) * RPMB_SIZE), RPMB_SIZE);
			printf("Authenticated data read response (Swapped)\n");
			dump_packet((u8 *) packet, RPMB_SIZE);
		} else {
			printf("HMAC compare success !!\n");
			printf("RPMB: HMAC: ");
			print_byte_to_hex(output_data, RPMB_HMAC_LEN);
			printf("\n");
			printf("Authenticated data read response (Not Swapped)\n");
			dump_packet((void *)(buf + ((blk_cnt - 1) * RPMB_SIZE)), RPMB_SIZE);
			printf("Authenticated data read response (Swapped)\n");
			dump_packet((u8 *) packet, RPMB_SIZE);
		}

		ufs_upiu_report(packet, SECU_PROT_IN);

		printf("RPMB: MAC\n");
		dump_packet(packet->Key_MAC, HMAC_SIZE);
		free(hmac);
		break;
	}

	free(buf);
	return ret;
}

int do_rpmb(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	int i;
	char *cmd;
	ulong addr;
	uint rw;
	int ret, start_blk, block_num, w_counter;
	uint8_t rpmb_key[RPMB_KEY_LEN];
	struct rpmb_packet packet;
	u8 *buf;
	int dev_num = simple_strtoul(argv[2], NULL, 10);

	cmd = argv[1];
	switch (cmd[0]) {
	case 'r':
		rw = 0;		/* read case */
		break;
	case 'w':
		rw = 1;		/* write case */
		break;
	default:
		goto usage;
	}

	memset((void *)&packet, 0, 512);
	cmd = argv[3];

	switch (cmd[0]) {
	case 'b':
		if (argc == 7) {
			addr = simple_strtoul(argv[6], NULL, 16);
			start_blk = simple_strtoul(argv[4], NULL, 16);
			block_num = simple_strtoul(argv[5], NULL, 16);

			/* RPMB block size is 512 Byte,
			 *  But Data packet size is 256 Byte.
			 */
			block_num *= 2;
		} else
			goto usage;

		if (rw) {
			printf("Authentication write (addr=%08lx, %d blocks)\n", addr, block_num);
			printf("--> Read write counter first <--\n");
			packet.request = 0x02;

			for (i = 0; i < 16; i++)
				packet.nonce[i] = i;
			ufs_rpmb_commands(dev_num, &packet);
			printf("--> Write counter : %x\n", packet.write_counter);
			w_counter = packet.write_counter;
			memset((void *)&packet, 0, 512);

			packet.request = 0x03;
			packet.count = block_num;
			packet.write_counter = w_counter;
		} else {
			printf("Authentication read (addr=%08lx, %d blocks)\n", addr, block_num);
			packet.request = 0x04;
			packet.count = block_num;
			*((u32 *) (packet.data) + 1) = block_num;
			for (i = 0; i < 16; i++)
				packet.nonce[i] = i;
		}
		*(u32 *) (packet.data) = addr;
		packet.address = start_blk;
		ufs_rpmb_commands(dev_num, &packet);
		if (packet.result != 0)
			return -1;
		break;

	case 'c':
		if (rw)
			goto usage;
		packet.request = 0x02;
		for (i = 0; i < 16; i++)
			packet.nonce[i] = i;
		ufs_rpmb_commands(dev_num, &packet);
		if (packet.result != 0)
			return -1;
		break;

	case 'k':
		if (!rw)
			goto usage;

		if (argc == 5)
			addr = simple_strtoul(argv[4], NULL, 16);
		else {
			/*  RPMB key derivation */
			printf("RPMB key derivation\n");

			ret = get_RPMB_key(RPMB_KEY_LEN, rpmb_key);
			if (ret != RV_SUCCESS) {
				printf("key derivation: fail: 0x%X\n", ret);
				return ret;
			}
			printf("key derivation: success\n");
			printf("key: ");
			print_byte_to_hex(rpmb_key, RPMB_KEY_LEN);
			printf("\n");
			addr = (ulong) rpmb_key;
		}
		memcpy(packet.Key_MAC, (void *)addr, 32);
		packet.request = 0x01;
		ufs_rpmb_commands(dev_num, &packet);
		if (packet.result != 0) {
			memset(rpmb_key, 0x0, RPMB_KEY_LEN);
			memset(packet.Key_MAC, 0x0, RPMB_KEY_LEN);
			return -1;
		}
		break;

	default:
		goto usage;
	}
	return 1;

 usage:
	printf("Usage:\n%s\n", cmdtp->usage);
	return -1;
}

void rpmb_key_programming(void)
{
	char *argv[7] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL};
	int ret;

	/* Read write counter */
	argv[1] = "read";
	argv[2] = "C4";
	argv[3] = "counter";
	ret = do_rpmb(NULL, 0, 7, argv);
	if (ret < 0) {
		/* key programming */
		argv[1] = "write";
		argv[3] = "key";
		ret = do_rpmb(NULL, 0, 7, argv);
		if (ret < 0)
			printf("RPMB: ERR: key programming fail: 0x%x\n", ret);
	} else {
		printf("RPMB: key already programmed\n");
	}

	ret = block_RPMB_key();
	if (ret != RV_SUCCESS)
		printf("RPMB: key blocking: fail: 0x%X\n", ret);
	else
		printf("RPMB: key blocking: success\n");

	ret = block_RPMB_hmac();
	if (ret != RV_SUCCESS)
		printf("RPMB: hmac blocking: fail: 0x%X\n", ret);
	else
		printf("RPMB: hmac blocking: success\n");
}

U_BOOT_CMD(rpmb, 7, 0, do_rpmb,
	   "RPMB test commands for SMDK board",
	   "sub commands list\n"
	   "\trpmb write {device_number} key - Write authentication KEY\n"
	   "\trpmb write {device_number} block {start_block} {number_of_block} {addr} - Authentication write\n"
	   "\trpmb read {device_number} block {start_block} {number_of_block} {addr} - Authentication read\n"
	   "\trpmb read {device_number} counter - Write counter read\n");
