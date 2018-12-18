/*
 * Copyright (c) 2011 Samsung Electronics Co., Ltd.
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

#include <common.h>
#include <config.h>
#include <asm/arch/movi_partition.h>
#include <asm/arch/cpu.h>

#define SMC_CMD_REG			(-101)

#define SMC_CMD_LOAD_UBOOT		(-230)
#define SMC_CMD_COLDBOOT		(-231)
#define SMC_CMD_WARMBOOT		(-232)
#define SMC_CMD_CHECK_SECOND_BOOT	(-233)
#define SMC_CMD_EMMC_ENDBOOTOP          (-234)
#define SMC_CMD_SDMMC_ENUMERATE         (-235)
#define SMC_CMD_SET_SECURE_REG		(-236)
#define SMC_CMD_READ_SECURE_REG		(-237)
#define SMC_CMD_SET_SIGNATURE_SIZE	(-238)
#define SMC_CMD_CHECK_SIGNATURE		(-240)
#define SMC_CMD_HASH			(-241)
#define SMC_CMD_CHECK_SIGNATURE_WITH_HASH	(-242)

/* MACRO for SMC_CMD_REG */
#define SMC_REG_CLASS_SFR_W		(0x1 << 30)
#define SMC_REG_CLASS_SFR_R		(0x3 << 30)
#define SMC_REG_CLASS_MASK		(0x3 << 30)
#define SMC_REG_ID_SFR_W(ADDR)		(SMC_REG_CLASS_SFR_W | ((ADDR) >> 2))
#define SMC_REG_ID_SFR_R(ADDR)		(SMC_REG_CLASS_SFR_R | ((ADDR) >> 2))

#ifndef CONFIG_CPU_EXYNOS5433
#define SMC_CMD_GET_DRAM_SIZE		(-239)
#endif
#define SMC_CMD_LOAD_SECURE_PAYLOAD	(-300)
#define SMC_CMD_GET_EL3_MON_VERSION	(-301)

#define SMC_CMD_LOAD_LDFW		(-0x500)

#ifdef CONFIG_SECURE_BOOT
#define SMC_CM_SECURE_BOOT		(0x8200101D)
#define SB_ERASE_PUBLIC_KEY		(0x5)
#endif

#ifdef CONFIG_UBOOT_SECURE_BOOT
#define SMC_UBOOT_SIGNATURE_SIZE	256
#else
#define SMC_UBOOT_SIGNATURE_SIZE	0
#endif

#ifdef CONFIG_TZSW_SECURE_BOOT
#define SMC_TZSW_SIGNATURE_SIZE         256
#else
#define SMC_TZSW_SIGNATURE_SIZE         0
#endif

#define CONFIG_IMAGE_INFO_BASE		(CONFIG_SYS_SDRAM_BASE)
#define CONFIG_PHY_UBOOT_BASE		(CONFIG_SYS_TEXT_BASE)

#if defined(CONFIG_CPU_EXYNOS5433_EVT1)
#define SMC_SECURE_CONTEXT_BASE		(CONFIG_PHY_IRAM_BASE + 0x3400)
#else
#define SMC_SECURE_CONTEXT_BASE		(CONFIG_PHY_IRAM_BASE + 0x4c00)
#endif

#if defined(CONFIG_CPU_EXYNOS5430) || defined(CONFIG_CPU_EXYNOS5260) || defined(CONFIG_CPU_EXYNOS3250) || defined(CONFIG_CPU_EXYNOS3472)
#define CONFIG_PHY_TZSW_BASE		(CONFIG_PHY_IRAM_BASE + 0xA000)
#elif defined(CONFIG_CPU_EXYNOS5412)
#define CONFIG_PHY_TZSW_BASE		(CONFIG_PHY_IRAM_BASE + 0x10000)
#else
#define CONFIG_PHY_TZSW_BASE		(CONFIG_PHY_IRAM_BASE + 0x8000)
#endif

#define TRUST_MEASUREMENT_KERNEL        (0x10)

#define SHA_INIT				0x0
#define SHA_UPDATE_UNIT_SIZE			0x1
#define SHA_UPDATE_REMAIN_SIZE			0x2
#define SHA_FINAL				0x3
#define SHA_DIGEST				0x4

#define SB_MAX_SIGN_LEN			(256)

typedef struct sdmmc_dev {
	/* for SDMMC */
	u32	image_pos;
	u32	blkcnt;
	u32	base_addr;
} sdmmc_t;

typedef struct emmc_dev {
	/* for eMMC */
	u32	blkcnt;
	u32	base_addr;
} emmc_t;

typedef struct sata_dev {
	/* for SATA */
	u64	read_sector_of_hdd;
	u32	trans_byte;
	u32	*read_buffer;
	u32	position_of_mem;
} sata_t;

typedef struct sfmc_dev {
	/* for SFMC */
	u32	cs;
	u32	byte_offset;
	u32	byte_size;
	void	*dest_addr;
} sfmc_t;

typedef struct spi_sf_dev {
	/* for SPI SERIAL FLASH */
	u32	flash_read_addr;
	u32	read_size;
	u8	*read_buff;
} spi_sf_t;

typedef struct usb_dev {
	/* for USB */
	u32	base_addr;
	u32	*read_buffer;
	u32	secure;
} usb_t;

typedef struct ufs_dev {
	/* for UFS */
	u32	start_blk;
	u32	blkcnt;
	u32	dst_addr;
} ufs_t;

/* boot device */
typedef union boot_device_u {
	sdmmc_t 	sdmmc;
	emmc_t		emmc;
	sata_t		sata;
	sfmc_t		sfmc;
	spi_sf_t	spi_sf;
	usb_t		usb;
	ufs_t		ufs;
} boot_device_t;

typedef struct ld_image_info {
	/* for Signature */
	u32	image_base_addr;
	u32	size;
	u32	secure_context_base;
	u32	signature_size;
	boot_device_t bootdev;

} image_info;

typedef struct {
	unsigned int		hash_addr;
	unsigned int		result_hash;
	unsigned int		total_size;
	unsigned int		unit_size;
	unsigned int		remain_size;
	unsigned int		kindofhash;
} HASH_INFO;

typedef struct {
	unsigned int		hashCode_addr;
	unsigned int		hashCode_Len;
	unsigned int 		pubKey_addr;
	unsigned int		pubKey_Len;
	unsigned int		signature;
	unsigned int		signatureLen;
	unsigned int		reserved1;
	unsigned int		reserved2;
} SIGNATURE_WITH_HASH;

typedef struct {
	unsigned int		context;
	unsigned int		data;
	unsigned int		dataLen;
	unsigned int		signature;
	unsigned int		signatureLen;
	unsigned int		reserved;
} CHECK_IMAGE_INFO;

typedef struct reg_val {
	u32	addr;
	u32	val;
} reg_val_t;

typedef struct reg_arr {
	reg_val_t	set0;
	reg_val_t	set1;
	reg_val_t	set2;
	reg_val_t	set3;
	reg_val_t	set4;
	reg_val_t	set5;
	reg_val_t	set6;
	reg_val_t	set7;
	reg_val_t	set8;
	reg_val_t	set9;
} reg_arr_t;

#if defined(CONFIG_CPU_EXYNOS7420) && defined(CONFIG_ARM64)
inline u64 exynos_smc_read(u64 cmd, u64 arg1);
#endif
#if defined(CONFIG_ARM64)
unsigned long load_sp_image(u32 boot_device);
#else
unsigned int load_sp_image(u32 boot_device);
#endif
unsigned int load_uboot_image(u32 boot_device);
unsigned int coldboot(u32 boot_device);
void warmboot(void);
unsigned int find_second_boot(void);
void emmc_endbootop(void);
#if defined(CONFIG_ARM64)
void set_secure_reg(u64 reg_val, u32 num);
#else
void set_secure_reg(u32 reg_val, u32 num);
#endif
unsigned int read_secure_reg(u32 addr);


