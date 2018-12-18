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
#include <asm/arch/smc.h>
#include "uboot_sb21.h"

static inline u64 exynos_smc(u64 cmd, u64 arg1, u64 arg2, u64 arg3)
{
	register u64 reg0 __asm__("x0") = cmd;
	register u64 reg1 __asm__("x1") = arg1;
	register u64 reg2 __asm__("x2") = arg2;
	register u64 reg3 __asm__("x3") = arg3;

	__asm__ volatile (
		"dsb	sy\n"
		"smc	0\n"
		: "+r"(reg0), "+r"(reg1), "+r"(reg2), "+r"(reg3)

	);

	return reg0;
}

void security_check(void)
{
	int ret;
	CHECK_IMAGE_INFO check_info_image;

	/* Verify Kernel */
	check_info_image.context = 0x0;
	check_info_image.data    = CONFIG_SECURE_KERNEL_BASE;
	check_info_image.dataLen = CONFIG_SECURE_KERNEL_SIZE - SB_MAX_SIGN_LEN;
	check_info_image.signature = CONFIG_SECURE_KERNEL_BASE + CONFIG_SECURE_KERNEL_SIZE - SB_MAX_SIGN_LEN;
	check_info_image.signatureLen = SB_MAX_SIGN_LEN;

	ret = exynos_smc(SMC_CMD_CHECK_SIGNATURE, 0, &check_info_image, TRUST_MEASUREMENT_KERNEL);

	if (ret){
		printf("Kernel Integrity check fail\nSystem Halt....");
		exynos_smc(SMC_CM_SECURE_BOOT, SB_ERASE_PUBLIC_KEY, 0, 0);
		printf("Public key is deleted...");
		while (1);
	}

	printf("Kernel Integirty check success.\n");

#ifdef CONFIG_SECURE_ROOTFS

	check_info_image.context = 0x0;
	check_info_image.data 	 = CONFIG_SECURE_ROOTFS_BASE;
	check_info_image.dataLen = CONFIG_SECURE_ROOTFS_SIZE - SB_MAX_SIGN_LEN;
	check_info_image.signature = CONFIG_SECURE_ROOTFS_BASE + CONFIG_SECURE_ROOTFS_SIZE - SB_MAX_SIGN_LEN;
	check_info_image.signatureLen = SB_MAX_SIGN_LEN;

	ret = exynos_smc(SMC_CMD_CHECK_SIGNATURE, 0, &check_info_image, 0);

	if (ret) {
		printf("rootfs Integrity check fail\nSystem Halt....");
		exynos_smc(SMC_CM_SECURE_BOOT, SB_ERASE_PUBLIC_KEY, 0, 0);
		printf("Public key is deleted...");
		while (1);
	}
	printf("rootfs Integirty check success.\n");
#endif
}
