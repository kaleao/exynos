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

static inline u32 exynos_smc(u32 cmd, u32 arg1, u32 arg2, u32 arg3)
{
	register u32 reg0 __asm__("r0") = cmd;
	register u32 reg1 __asm__("r1") = arg1;
	register u32 reg2 __asm__("r2") = arg2;
	register u32 reg3 __asm__("r3") = arg3;

	__asm__ volatile (
		"smc	0\n"
		: "+r"(reg0), "+r"(reg1), "+r"(reg2), "+r"(reg3)

	);

	return reg0;
}

#if defined(CONFIG_CPU_EXYNOS5433_EVT1)
void security_check(void)
{
	int ret;
	CHECK_IMAGE_INFO check_info_image;

	/* Verify Kernel */
	check_info_image.context = 0x0;
	check_info_image.data    = CONFIG_SECURE_KERNEL_BASE;
	check_info_image.dataLen = CONFIG_SECURE_KERNEL_SIZE - 256;
	check_info_image.signature = CONFIG_SECURE_KERNEL_BASE + CONFIG_SECURE_KERNEL_SIZE - 256;
	check_info_image.signatureLen = 256;

	ret = exynos_smc(SMC_CMD_CHECK_SIGNATURE, 0, &check_info_image, TRUST_MEASUREMENT_KERNEL);

	if (ret){
		printf("Kernel Integrity check fail\nSystem Halt....");
		while (1);
	}

	printf("Kernel Integirty check success.\n");

#ifdef CONFIG_SECURE_ROOTFS

	check_info_image.context = 0x0;
	check_info_image.data 	 = CONFIG_SECURE_ROOTFS_BASE;
	check_info_image.dataLen = CONFIG_SECURE_ROOTFS_SIZE - 256;
	check_info_image.signature = CONFIG_SECURE_ROOTFS_BASE + CONFIG_SECURE_ROOTFS_SIZE - 256;
	check_info_image.signatureLen = 256;

	ret = exynos_smc(SMC_CMD_CHECK_SIGNATURE, 0, &check_info_image, 0);

	if (ret) {
		printf("rootfs Integrity check fail\nSystem Halt....");
		while (1);
	}
	printf("rootfs Integirty check success.\n");
#endif

}
#else
void security_check(void)
{
	unsigned int secure_context_base;
	int ret;

	/* Set Secure Context Base Address*/
	secure_context_base = CONFIG_SYS_SDRAM_BASE + PART_SIZE_BL1 - 1024;

	/* Verify Kernel */
	ret = check_signature((SB20_CONTEXT *)secure_context_base,
			(unsigned char *)CONFIG_SECURE_KERNEL_BASE, CONFIG_SECURE_KERNEL_SIZE - 256,
			(unsigned char *)(CONFIG_SECURE_KERNEL_BASE +
			CONFIG_SECURE_KERNEL_SIZE - 256), 256);
	if (ret) {
		printf("Kernel Integrity check fail\nSystem Halt....");
		while (1);
	}
	printf("Kernel Integirty check success.\n");

#ifdef CONFIG_SECURE_ROOTFS
	/* Verify rootfs */
	ret = check_signature((SB20_CONTEXT *)secure_context_base,
			(unsigned char *)CONFIG_SECURE_ROOTFS_BASE, CONFIG_SECURE_ROOTFS_SIZE - 256,
			(unsigned char *)(CONFIG_SECURE_ROOTFS_BASE +
			CONFIG_SECURE_ROOTFS_SIZE - 256), 256);
	if (ret) {
		printf("rootfs Integrity check fail\nSystem Halt....");
		while (1);
	}
	printf("rootfs Integirty check success.\n");
#endif
}
#endif
