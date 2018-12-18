/*
 * Copyright (C) 2013 Samsung Electronics
 *
 * Configuration settings for the SAMSUNG XYREF4415 (EXYNOS4415) board.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/* High Level Configuration Options */
#define CONFIG_SAMSUNG			/* in a SAMSUNG core */
#define CONFIG_S5P			/* S5P Family */
#define CONFIG_ARCH_EXYNOS		/* which is in a Exynos Family */
#define CONFIG_ARCH_EXYNOS4	        /* which is in a Exynos4 Family */
#define CONFIG_CPU_EXYNOS4415		/* which is in a Exynos4415 */
#define CONFIG_MACH_XYREF4415		/* which is in a XYREF4415 */

#include <asm/arch/cpu.h>		/* get chip and board defs */

#define CONFIG_ARCH_CPU_INIT
#define CONFIG_DISPLAY_CPUINFO
#define CONFIG_DISPLAY_BOARDINFO

/* Configuration of SDRAM size */
#undef CONFIG_SDRAM_SIZE_1536MB

/* Configuration of bl1 partition size */
#define CONFIG_BL_MONITOR

/* TRUSTZONE */
#define CONFIG_TRUSTZONE_ENABLE
#ifdef CONFIG_TRUSTZONE_ENABLE
#undef CONFIG_TZPC
#define CONFIG_SMC_CMD
#define CONFIG_TRUSTZONE_RESERVED_DRAM  0x400000
#endif

/* Configuration of secure boot */
#undef CONFIG_UBOOT_SECURE_BOOT
#define CONFIG_TZSW_SECURE_BOOT
#undef CONFIG_SECURE_BOOT

#ifdef CONFIG_SECURE_BOOT
#define CONFIG_UBOOT_SECURE_BOOT
#define CONFIG_TZSW_SECURE_BOOT
#define CONFIG_SECURE_ROOTFS
#define CONFIG_SECURE_CONTEXT_BASE      0x40003800
#define CONFIG_SECURE_KERNEL_BASE       0x40008000
#define CONFIG_SECURE_KERNEL_SIZE       0x400000
#define CONFIG_SECURE_ROOTFS_BASE       0x41000000
#define CONFIG_SECURE_ROOTFS_SIZE       0x100000
#endif

/* Keep L2 Cache Disabled */
#define CONFIG_SYS_DCACHE_OFF

#define CONFIG_SYS_SDRAM_BASE		0x40000000
#define CONFIG_SYS_TEXT_BASE		0x43E00000
#define CONFIG_SPL_TEXT_BASE		0x02026000

/* input clock of PLL: XYREF4415 has 24MHz input clock */
#define CONFIG_SYS_CLK_FREQ		24000000

#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_CMDLINE_TAG
#define CONFIG_INITRD_TAG
#define CONFIG_REVISION_TAG
#define CONFIG_CMDLINE_EDITING

#define CONFIG_MACH_TYPE		MACH_TYPE_XYREF4415

/* iRAM information */
#define CONFIG_PHY_IRAM_BASE            (0x02020000)
#define CONFIG_PHY_IRAM_NS_BASE         (CONFIG_PHY_IRAM_BASE + 0x4F000)

/* bootloader system information */
#define CONFIG_BL_SYS_INFO_BASE		(CONFIG_PHY_IRAM_NS_BASE + 0x800)

/* Power Down Modes */
#define S5P_CHECK_SLEEP			0x00000BAD
#define S5P_CHECK_DIDLE			0xBAD00000
#define S5P_CHECK_LPA			0xABAD0000

/* Offset for OM status registers */
#define OM_STATUS_OFFSET                0x0

/* Offset for inform registers */
#define INFORM0_OFFSET			0x800
#define INFORM1_OFFSET			0x804
#define INFORM2_OFFSET			0x808
#define INFORM3_OFFSET			0x80C

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + (1 << 20))

/* select serial console configuration */
#define CONFIG_SERIAL_MULTI
#define CONFIG_SERIAL3			/* use SERIAL 3 */
#define CONFIG_BAUDRATE			115200
#define EXYNOS4_DEFAULT_UART_OFFSET	0x010000

#define TZPC_BASE_OFFSET		0x10000

/* Power Management is enabled */
#define CONFIG_PM
#define CONFIG_PM_VDD_MIF	        1100
#define CONFIG_PM_VDD_ARM	        1000
#define CONFIG_PM_VDD_INT	        1000
#define CONFIG_PM_VDD_G3D	        1100

/*  RAMDUMP MODE */
#define CONFIG_RAMDUMP_MODE		0xD

/* SD/MMC configuration */
#define CONFIG_GENERIC_MMC
#define CONFIG_MMC
#define CONFIG_SDHCI
#define CONFIG_S5P_MSHC
#define CONFIG_S5P_SDHCI

#if defined(CONFIG_S5P_MSHC)
#define CONFIG_MMC_EARLY_INIT
#define MMC_MAX_CHANNEL         4
#define USE_MMC0
#define USE_MMC2

#define PHASE_DEVIDER           4

#define SDR_CH0                 0x03020001
#define DDR_CH0                 0x03030002

#define SDR_CH2                 0x03020001
#define DDR_CH2                 0x03030002

#define SDR_CH4			0x00010001
#define DDR_CH4			0x00010001
#endif

/*
 * Boot configuration
 */
#define BOOT_ONENAND		0x1
#define BOOT_NAND		0x40000
#define BOOT_MMCSD		0x3
#define BOOT_NOR		0x4
#define BOOT_SEC_DEV		0x5
#define BOOT_EMMC		0x6
#define BOOT_EMMC_4_4		0x7
#define BOOT_USB                0x100

/*
 *  Boot device
 */
#define SDMMC_CH2               0x0
#define SDMMC_CH0               0x4
#define EMMC                    0x14
#define SATA                    0x18
#define SPI_SF                  0x28
#define SFMC                    0x34
#define USB                     0x40

#define CONFIG_BOARD_EARLY_INIT_F
#define CONFIG_BOARD_LATE_INIT

/* PWM */
#define CONFIG_PWM

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE

/* Command definition*/
#include <config_cmd_default.h>

#define CONFIG_CMD_PING
#define CONFIG_CMD_ELF
#define CONFIG_CMD_MMC
#define CONFIG_CMD_EXT2
#define CONFIG_CMD_FAT
#define CONFIG_CMD_NET
#define CONFIG_CMD_BOOTZ

#define CONFIG_BOOTDELAY		3
#define CONFIG_ZERO_BOOTDELAY_CHECK

/* USB */
#undef CONFIG_CMD_USB
/* EHCI : 2.0 Host */
#undef CONFIG_USB_EHCI
#undef CONFIG_USB_EHCI_EXYNOS
#undef CONFIG_USB_STORAGE

#define CONFIG_S3C_USBD
#undef CONFIG_USB_CPUMODE

#define USBD_DOWN_ADDR                  0x40000000
#define EXYNOS_SYSREG_BASE              EXYNOS4_SYSREG_BASE
#define EXYNOS_POWER_BASE               EXYNOS4_POWER_BASE

/*
 * FASTBOOT
 */
#define CONFIG_FASTBOOT
#define CFG_FASTBOOT_SDMMCBSP
/* Fastboot variables */
#define CFG_FASTBOOT_TRANSFER_BUFFER            (0x48000000)
#define CFG_FASTBOOT_TRANSFER_BUFFER_SIZE       (0x30000000)   /* 768MB */
#define CFG_FASTBOOT_ADDR_KERNEL                (0x40008000)
#define CFG_FASTBOOT_ADDR_RAMDISK               (0x41000000)
#define CFG_FASTBOOT_PAGESIZE                   (2048)  // Page size of booting device
#define CFG_FASTBOOT_SDMMC_BLOCKSIZE            (512)   // Block size of sdmmc

/* MMC SPL */
#define CONFIG_SPL
#define CONFIG_SPL_15KB
#define IROM_FNPTR_BASE                         0x020200A0
#define SECCOND_BOOT_INFORM_OFFSET              0x00000040
#define SDMMC_DEV_OFFSET                        0x00000010
#define EMMC_DEV_OFFSET                         0x00000014

#ifdef CONFIG_SECURE_BOOT
#define CONFIG_BOOTCOMMAND	"emmc open 0;movi r z f 0 40000000;emmc close 0;"       \
                                "movi read kernel 0 40008000;movi read rootfs 0 41000000 100000;bootz 40008000 41000000"
#else
#define CONFIG_BOOTCOMMAND	"movi read kernel 0 40008000;movi read rootfs 0 41000000 100000;bootz 40008000 41000000"
#endif

#define CONFIG_RECOVERYCOMMAND	\
		"emmc partition 0 10 0;"	\
		"mmc erase user 0 1072 1;"	\
		"movi r f 1 40000000;emmc open 0;movi w z f 0 40000000;emmc close 0;"	\
		"movi r b 1 40000000;emmc open 0;movi w z b 0 40000000;emmc close 0;"	\
		"movi r u 1 40000000;emmc open 0;movi w z u 0 40000000;emmc close 0;"	\
		"movi r t 1 40000000;emmc open 0;movi w z t 0 40000000;emmc close 0;"	\
		"reset"

/* Configuration for factory reset mode */
#define CONFIG_FACTORY_RESET_MODE       0xf
#define CONFIG_FACTORY_RESET_BOOTCOMMAND        \
                "ext3format mmc 0:3;ext3format mmc 0:4;"		\
                "movi read kernel 0 40008000;"                          \
                "movi read rootfs 0 41000000 100000;"                   \
                "bootz 40008000 41000000"

/* Miscellaneous configurable options */
#define CONFIG_SYS_LONGHELP		/* undef to save memory */
#define CONFIG_SYS_HUSH_PARSER		/* use "hush" command parser	*/
#define CONFIG_SYS_PROMPT		"XYREF4415 # "
#define CONFIG_SYS_CBSIZE		256	/* Console I/O Buffer Size */
#define CONFIG_SYS_PBSIZE		384	/* Print Buffer Size */
#define CONFIG_SYS_MAXARGS		16	/* max number of command args */
#define CONFIG_DEFAULT_CONSOLE		"console=ttySAC1,115200n8\0"
/* Boot Argument Buffer Size */
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE
/* memtest works on */
#define CONFIG_SYS_MEMTEST_START	CONFIG_SYS_SDRAM_BASE
#define CONFIG_SYS_MEMTEST_END		(CONFIG_SYS_SDRAM_BASE + 0x5E00000)
#define CONFIG_SYS_LOAD_ADDR		(CONFIG_SYS_SDRAM_BASE + 0x3E00000)

#define CONFIG_SYS_HZ			1000

#define CONFIG_RD_LVL

/* Stack sizes */
#define CONFIG_STACKSIZE		(256 << 10)	/* 256KB */

#if defined(CONFIG_SDRAM_SIZE_1536MB)
#define CONFIG_NR_DRAM_BANKS	6
#define SDRAM_BANK_SIZE		(256UL << 20UL)	/* 256 MB */
#define PHYS_SDRAM_1		CONFIG_SYS_SDRAM_BASE
#define PHYS_SDRAM_1_SIZE	SDRAM_BANK_SIZE
#define PHYS_SDRAM_2		(CONFIG_SYS_SDRAM_BASE + SDRAM_BANK_SIZE)
#define PHYS_SDRAM_2_SIZE	SDRAM_BANK_SIZE
#define PHYS_SDRAM_3		(CONFIG_SYS_SDRAM_BASE + (2 * SDRAM_BANK_SIZE))
#define PHYS_SDRAM_3_SIZE	SDRAM_BANK_SIZE
#define PHYS_SDRAM_4		(CONFIG_SYS_SDRAM_BASE + (3 * SDRAM_BANK_SIZE))
#define PHYS_SDRAM_4_SIZE	SDRAM_BANK_SIZE
#define PHYS_SDRAM_5		(CONFIG_SYS_SDRAM_BASE + (4 * SDRAM_BANK_SIZE))
#define PHYS_SDRAM_5_SIZE	SDRAM_BANK_SIZE
#define PHYS_SDRAM_6		(CONFIG_SYS_SDRAM_BASE + (5 * SDRAM_BANK_SIZE))
#define PHYS_SDRAM_6_SIZE       (SDRAM_BANK_SIZE -                            \
                                               CONFIG_TRUSTZONE_RESERVED_DRAM)
#else
#define CONFIG_NR_DRAM_BANKS	6
#define SDRAM_BANK_SIZE		(256UL << 20UL)	/* 256 MB */
#define PHYS_SDRAM_1		CONFIG_SYS_SDRAM_BASE
#define PHYS_SDRAM_1_SIZE	SDRAM_BANK_SIZE
#define PHYS_SDRAM_2		(CONFIG_SYS_SDRAM_BASE + SDRAM_BANK_SIZE)
#define PHYS_SDRAM_2_SIZE	SDRAM_BANK_SIZE
#define PHYS_SDRAM_3		(CONFIG_SYS_SDRAM_BASE + (2 * SDRAM_BANK_SIZE))
#define PHYS_SDRAM_3_SIZE	SDRAM_BANK_SIZE
#define PHYS_SDRAM_4		(CONFIG_SYS_SDRAM_BASE + (3 * SDRAM_BANK_SIZE))
#define PHYS_SDRAM_4_SIZE	SDRAM_BANK_SIZE
#define PHYS_SDRAM_4_END_SIZE	(SDRAM_BANK_SIZE -                            \
						CONFIG_TRUSTZONE_RESERVED_DRAM)
#define PHYS_SDRAM_5		(CONFIG_SYS_SDRAM_BASE + (4 * SDRAM_BANK_SIZE))
#define PHYS_SDRAM_5_SIZE	SDRAM_BANK_SIZE
#define PHYS_SDRAM_6		(CONFIG_SYS_SDRAM_BASE + (5 * SDRAM_BANK_SIZE))
#define PHYS_SDRAM_6_SIZE	(SDRAM_BANK_SIZE -                            \
						CONFIG_TRUSTZONE_RESERVED_DRAM)
#endif

#define CONFIG_SYS_SDRAM_SIZE		(CONFIG_NR_DRAM_BANKS * SDRAM_BANK_SIZE)
#define CONFIG_SYS_MONITOR_BASE	0x00000000

/* FLASH and environment organization */
#define CONFIG_SYS_NO_FLASH
#undef CONFIG_CMD_IMLS
#define CONFIG_IDENT_STRING		" for XYREF4415"

#define CONFIG_ENV_IS_IN_MMC
#define CONFIG_SYS_MMC_ENV_DEV		0

/* Configuration of ENV size on mmc */
#define CONFIG_ENV_SIZE		(16 << 10)	/* 16 KB */
#include <asm/arch/movi_partition.h>

/* Configuration of ROOTFS_ATAGS */
#define CONFIG_ROOTFS_ATAGS
#ifdef CONFIG_ROOTFS_ATAGS
#define CONFIG_EXTRA_ENV_SETTINGS       "rootfslen= 100000"
#endif

/* Configuration for Partition */
#define CONFIG_DOS_PARTITION
#define CONFIG_NVDATA_PARTITION
#define CFG_PARTITION_START     0x6400000
#define CONFIG_IRAM_STACK	0x02080000

#define CONFIG_SYS_INIT_SP_ADDR	(CONFIG_SYS_LOAD_ADDR - 0x1000000)

/* Ethernet Controllor Driver */
#ifdef CONFIG_CMD_NET
#define CONFIG_SMC911X
#define CONFIG_SMC911X_BASE		0x5000000
#define CONFIG_SMC911X_16_BIT
#define CONFIG_ENV_SROM_BANK		1
#endif /*CONFIG_CMD_NET*/

/* Disable devicetree support */
/* #define CONFIG_OF_LIBFDT */

/* Base address for secondary boot information */
#define CONFIG_SECONDARY_BOOT_INFORM_BASE       (CONFIG_SYS_TEXT_BASE - 0x8)

/* Configurateion of LCD */
#ifdef	CONFIG_EXYNOS_FB
#define CONFIG_SAMSUNG_LOGO
#define CONFIG_FB_EXYNOS_FIMD_V8
#define CONFIG_BMP_32BPP
#define CONFIG_FB_ADDR	0x50000000
#define CONFIG_S6E8AA0
#define CONFIG_EXYNOS_MIPI_DSIM

#define CONFIG_VIDEO_BMP_GZIP
#define CONFIG_SYS_VIDEO_LOGO_MAX_SIZE ((588 * 120 * 4) + (1 << 12))
#endif

/* Offset for pmu reset status */
#define RST_STAT_OFFSET			0x404

/* RST_STAT */
#define SWRESET			(1 << 29)
#define WRESET			(1 << 28)
#define CP_WDTRESET		(1 << 27)
#define ISP_ARM_WDTRESET	(1 << 25)
#define SYS_WDTRESET		(1 << 20)
#define PINRESET		(1 << 16)

#endif	/* __CONFIG_H */
