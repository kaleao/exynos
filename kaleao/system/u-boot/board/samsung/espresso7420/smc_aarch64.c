/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *
 * "SMC CALL COMMAND"
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <asm/arch/smc.h>
#include "setup.h"

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

inline u64 exynos_smc_read(u64 cmd, u64 arg1)
{
	register u64 reg0 __asm__("x0") = cmd;
	register u64 reg1 __asm__("x1") = arg1;
	register u64 reg2 __asm__("x2") = 0;

	__asm__ volatile (
		"dsb	sy\n"
		"smc	0\n"
		: "+r"(reg0), "+r"(reg1), "+r"(reg2)

	);

	return reg2;
}

int init_ldfw(u64 addr, u64 size)
{
#if defined(CONFIG_SMC_CMD)
	return exynos_smc(SMC_CMD_LOAD_LDFW,
			addr, size, 0);
#else
	return 0;
#endif

}

unsigned long load_sp_image(u32 boot_device)
{
#if defined(CONFIG_SMC_CMD)
	if (boot_device == SDMMC_CH2)
		return exynos_smc(SMC_CMD_LOAD_SECURE_PAYLOAD,
			(u64)boot_device, MOVI_TZSW_POS, 0);
	else if (boot_device == UFS)
		return exynos_smc(SMC_CMD_LOAD_SECURE_PAYLOAD,
				(u64)boot_device, UFS_TZSW_POS, 0);
	else
		return exynos_smc(SMC_CMD_LOAD_SECURE_PAYLOAD,
			(u64)boot_device, 0, 0);
#else
	return 0;
#endif
}

#ifdef CONFIG_EXYNOS_DA
unsigned int bl2_mdelay(u32 mdelay){
    int i=0;
    int j=0;

#define DEF_CLK 800000000

    for(i=0; i < mdelay* (DEF_CLK / 1000)  ;i++){
        asm volatile("": : :"memory");
        j++;
    }
}
#endif /* CONFIG_EXYNOS_DA */

unsigned int load_uboot_image(u32 boot_device)
{
#if defined(CONFIG_SMC_CMD)
	image_info *info_image;

	info_image = (image_info *) ((u32)smc_image_info_base);

	info_image->image_base_addr = CONFIG_PHY_UBOOT_BASE;
	info_image->size = (MOVI_UBOOT_BLKCNT * MOVI_BLKSIZE);
	info_image->secure_context_base = SMC_SECURE_CONTEXT_BASE;
	info_image->signature_size = SMC_UBOOT_SIGNATURE_SIZE;

	if (boot_device == SDMMC_CH2) {

		info_image->bootdev.sdmmc.image_pos = MOVI_UBOOT_POS;
		info_image->bootdev.sdmmc.blkcnt =  MOVI_UBOOT_BLKCNT;
		info_image->bootdev.sdmmc.base_addr = CONFIG_PHY_UBOOT_BASE;

	} else if (boot_device == EMMC) {

		info_image->bootdev.emmc.blkcnt =  MOVI_UBOOT_BLKCNT;
		info_image->bootdev.emmc.base_addr = CONFIG_PHY_UBOOT_BASE;

	} else if (boot_device == UFS) {
		info_image->bootdev.ufs.start_blk = UFS_UBOOT_POS;
		info_image->bootdev.ufs.blkcnt =  UFS_UBOOT_BLKCNT;
		info_image->bootdev.ufs.dst_addr = CONFIG_SYS_TEXT_BASE;
		info_image->size = (UFS_UBOOT_BLKCNT * UFS_BLKSIZE);

	} else if (boot_device == USB) {
        /* USB buffer, under 3 KB size, non-secure region */
//        info_image->bootdev.usb.read_buffer = CONFIG_PHY_IRAM_NS_BASE + 0x800;
#ifdef CONFIG_EXYNOS_DA
//        bl2_mdelay(500);
#endif
    }

#if defined(CONFIG_UBOOT_SECURE_BOOT)
	exynos_smc(SMC_CMD_SET_SIGNATURE_SIZE,
			0, (u32)info_image, 0);
#endif
	return exynos_smc(SMC_CMD_LOAD_UBOOT,
			(u64)boot_device, (u64)info_image, (u64)boot_device);
#else
	if (boot_device == SDMMC_CH2) {

		u32 (*copy_uboot)(u32, u32, u32) = (void *)
			*(u32 *)(IROM_FNPTR_BASE + SDMMC_DEV_OFFSET);

		copy_uboot(MOVI_UBOOT_POS,
				MOVI_UBOOT_BLKCNT, CONFIG_SYS_TEXT_BASE);

	} else if (boot_device == EMMC) {

		u32 (*copy_uboot)(u32, u32) = (void *)
				*(u32 *)(IROM_FNPTR_BASE + EMMC_DEV_OFFSET );

		copy_uboot(MOVI_UBOOT_BLKCNT, CONFIG_SYS_TEXT_BASE);

	}

#endif
}

unsigned int coldboot(u32 boot_device)
{
#ifdef CONFIG_EXYNOS_DA
//    bl2_mdelay(500);
#endif
#if defined(CONFIG_SMC_CMD)
	return exynos_smc(SMC_CMD_COLDBOOT, 0, 0, CONFIG_SYS_TEXT_BASE);
#else
	__attribute__((noreturn)) void (*uboot)(void);

	/* Jump to U-Boot image */
	uboot = (void *)CONFIG_SYS_TEXT_BASE;
	(*uboot)();
#endif
	/* Never returns Here */
	return 0;
}

void warmboot(void)
{
#if defined(CONFIG_SMC_CMD)
	exynos_smc(SMC_CMD_WARMBOOT, 0, 0, CONFIG_PHY_IRAM_NS_BASE);
#else
	__attribute__((noreturn)) void (*wakeup_kernel)(void);

	/* Jump to kernel for wakeup */
	wakeup_kernel = (void *)readl(EXYNOS7420_POWER_BASE + INFORM0_OFFSET);
	(*wakeup_kernel)();
	/* Never returns Here */
#endif
}

unsigned int find_second_boot(void)
{
#if defined(CONFIG_SMC_CMD)
	return exynos_smc_read(SMC_CMD_CHECK_SECOND_BOOT, 0);
#else
	return readl(IROM_FNPTR_BASE + SECCOND_BOOT_INFORM_OFFSET);
#endif
}

void emmc_endbootop(void)
{
#if defined(CONFIG_SMC_CMD)
	exynos_smc(SMC_CMD_EMMC_ENDBOOTOP, 0, 0, 0);
#else
	void (*emmc_endboot_op)(void) = (void *)
		*(u32 *)(IROM_FNPTR_BASE + EMMC_ENDBOOTOP_OFFSET);

	emmc_endboot_op();
#endif
}

void sdmmc_enumerate(void)
{
	exynos_smc(SMC_CMD_SDMMC_ENUMERATE, 0, 0, 0);
}

void set_secure_reg(u64 reg_val, u32 num)
{
#if defined(CONFIG_SMC_CMD)
	exynos_smc(SMC_CMD_SET_SECURE_REG, reg_val, (u64)num, reg_val);
#else

#endif
}

#if defined(CONFIG_RAMDUMP_GPR)
void cpu_boot(u64 id, u64 cpu, u64 fn)
{
	exynos_smc(id, cpu, fn, 0);
}
#endif

unsigned int get_el3_mon_version(char *ptr, u32 string_size)
{
#if defined(CONFIG_SMC_CMD)
	return exynos_smc(SMC_CMD_GET_EL3_MON_VERSION,
			ptr, string_size, 0);
#else
	return 0;
#endif
}
