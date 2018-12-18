/*
 * Copyright (C) 2014 Samsung Electronics
 *
 * Configuration settings for the SAMSUNG SMDK7580 (EXYNOS7580) board.
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
#define CONFIG_ARM64
#define CONFIG_SAMSUNG			/* in a SAMSUNG core */
#define CONFIG_S5P			/* S5P Family */
#define CONFIG_ARCH_EXYNOS		/* which is in a Exynos Family */
#define CONFIG_ARCH_EXYNOS7		/* which is in a Exynos7 Family */
#define CONFIG_CPU_EXYNOS7580		/* which is in a Exynos7580 */
/* CONFIG_MACH_SMDK7580 is for only SMDK7580. */
#define CONFIG_MACH_SMDK7580		/* which is in a SMDK7580 */
#define CONFIG_EMULATOR			/* which is in a EMULATOR */

#ifdef CONFIG_MACH_SMDK7580
#define CONFIG_BOARD_NAME		"SMDK7580"
#endif

#include <asm/arch/cpu.h>		/* get chip and board defs */

#define CONFIG_ARCH_CPU_INIT
#define CONFIG_DISPLAY_CPUINFO
#define CONFIG_DISPLAY_BOARDINFO
#define CONFIG_DISPLAY_CHIPID

/* TRUSTZONE */
#define CONFIG_TRUSTZONE_ENABLE
#ifdef CONFIG_TRUSTZONE_ENABLE
#undef CONFIG_TZPC
#define CONFIG_SMC_CMD
#define CONFIG_TRUSTZONE_RESERVED_DRAM  0x900000
#else
#define CONFIG_TRUSTZONE_RESERVED_DRAM  0x0
#endif

/* Configuration of bl1 partition size */
#define CONFIG_BL_MONITOR

/* Configuration of secure boot */
#undef CONFIG_UBOOT_SECURE_BOOT
#undef CONFIG_TZSW_SECURE_BOOT
#undef CONFIG_SECURE_BOOT

#ifdef CONFIG_SECURE_BOOT
#define CONFIG_UBOOT_SECURE_BOOT
#define CONFIG_TZSW_SECURE_BOOT
#define CONFIG_SECURE_ROOTFS
#define CONFIG_SECURE_CONTEXT_BASE      0x20003800
#define CONFIG_SECURE_KERNEL_BASE       0x20080000
#define CONFIG_SECURE_KERNEL_SIZE       PART_SIZE_KERNEL
#define CONFIG_SECURE_ROOTFS_BASE       0x22000000
#define CONFIG_SECURE_ROOTFS_SIZE       0x100000
#endif

/* Power Management is enabled */
#define CONFIG_PM
#ifndef CONFIG_EMULATOR
#define CONFIG_PM_VDD_ATLAS		982
#define CONFIG_PM_VDD_APOLLO		1107
#define CONFIG_PM_VDD_MIF		1000
#define CONFIG_PM_VDD_INT		1000
#define CONFIG_PM_VDD_G3D		900
#define CONFIG_PM_VDD_DISP		1000
#endif

/* MUIC device is enabled */
#define CONFIG_MUIC

/* Bootloader Recovery */
#define CONFIG_RECOVERY_MODE

/* ETBDUMP MODE */
#define CONFIG_ETBDUMP_MODE

/* RAMDUMP MODE */
#define CONFIG_RAMDUMP_MODE		0xD
#define CONFIG_RAMDUMP_GPR

/* Keep L2 Cache Disabled */
#define CONFIG_SYS_DCACHE_OFF

#define CONFIG_SYS_SDRAM_BASE		0x20000000
#define CONFIG_SYS_TEXT_BASE		0x6F000000
#define CONFIG_SYS_ROOTFS_BASE		0x22000000
#define CONFIG_SPL_TEXT_BASE		0x02054000

/* input clock of PLL: SMDK7580 has 26MHz input clock */
#define CONFIG_SYS_CLK_FREQ		26000000

#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_CMDLINE_TAG
#define CONFIG_INITRD_TAG
#define CONFIG_REVISION_TAG
#define CONFIG_CMDLINE_EDITING

#define CONFIG_MACH_TYPE		MACH_TYPE_SMDK7580

/* iRAM information */
#define CONFIG_PHY_IRAM_BASE            (0x02020000)
#define CONFIG_PHY_IRAM_NS_BASE         (CONFIG_PHY_IRAM_BASE + 0x4E000)
#define CONFIG_PHY_BL_SYS_INFO		(CONFIG_PHY_IRAM_NS_BASE + 0x800)
#define CONFIG_PHY_BL_SYS_INFO_DRAM_SIZE	(CONFIG_PHY_BL_SYS_INFO + 0x48)
#define CONFIG_PHY_ALIVE_IRAM_BASE         (CONFIG_PHY_IRAM_BASE + 0xA1000)

/* BL1 version information */
#define CONFIG_BL1_VERSION_INFORM       (CONFIG_PHY_IRAM_NS_BASE + 0x810)

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
#define CONFIG_SERIAL1			/* use SERIAL 1 */
#define CONFIG_BAUDRATE			115200

#define TZPC_BASE_OFFSET		0x10000

/* SD/MMC configuration */
#define CONFIG_GENERIC_MMC
#define CONFIG_MMC
#define CONFIG_SDHCI
#define CONFIG_S5P_MSHC
#define CONFIG_S5P_SDHCI
#define CONFIG_MMC_64BIT_BUS

#if defined(CONFIG_S5P_MSHC)
#define CONFIG_MMC_SMU_INIT
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

#ifndef CONFIG_EMULATOR
/* UFS configuration */
#define CONFIG_SCSI_UFS
#if defined(CONFIG_SCSI_UFS)
#define CONFIG_CMD_SCSI
#define CONFIG_CMD_UFS
#define CONFIG_SYS_SCSI_MAX_SCSI_ID	1
#define CONFIG_SYS_SCSI_MAX_LUN		8
#define CONFIG_SYS_SCSI_MAX_DEVICE	(CONFIG_SYS_SCSI_MAX_SCSI_ID * \
					CONFIG_SYS_SCSI_MAX_LUN)
#endif
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
#define BOOT_UFS		0x200

/*
 *  Boot device
 */
#define SDMMC_CH2               0x0
#define SDMMC_CH0               0x4
#define EMMC                    0x14
#define SATA                    0x18
#define UFS			0x20
#define SPI_SF                  0x28
#define SFMC                    0x34
#define USB                     0x40

#define CONFIG_BOARD_EARLY_INIT_F
#define CONFIG_BOARD_LATE_INIT

/* CHIP ID */
#define CHIPID0_OFFSET		0x14
#define CHIPID1_OFFSET		0x18

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
#define CONFIG_PARTITION_UUIDS

/* FAT filesystem */
#define CONFIG_FAT_WRITE

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

#define USBD_DOWN_ADDR                  0x20000000
#define EXYNOS_SYSREG_BASE              EXYNOS7580_SYSREG_BASE
#define EXYNOS_POWER_BASE               EXYNOS7580_POWER_BASE

/*
 * FASTBOOT
 */
#define CONFIG_FASTBOOT
#define CFG_FASTBOOT_SDMMCBSP
#if defined(CONFIG_SCSI_UFS)
#define CFG_FASTBOOT_UFSBSP
#endif
/* Fastboot multiple access */
#define CFG_FASTBOOT_MULTI_ACESS
/* Fastboot variables */
#define CFG_FASTBOOT_TRANSFER_BUFFER            (0x28000000)
#define CFG_FASTBOOT_TRANSFER_BUFFER_SIZE       (0x30000000)   /* 768MB */
#define CFG_FASTBOOT_ADDR_KERNEL                (0x20008000)
#define CFG_FASTBOOT_ADDR_RAMDISK               (0x21000000)
#define CFG_FASTBOOT_MMC_BUFFER                 (0x78000000)
#define CFG_FASTBOOT_PAGESIZE                   (2048)  // Page size of booting device
#define CFG_FASTBOOT_SDMMC_BLOCKSIZE            (512)   // Block size of sdmmc

/* MMC SPL */
#define CONFIG_SPL
#define CONFIG_SPL_19KB
#define IROM_FNPTR_BASE                         0x020200A0
#define SECCOND_BOOT_INFORM_OFFSET              0x00000028
#define SDMMC_DEV_OFFSET                        0x00000000
#define EMMC_DEV_OFFSET                         0x00000014
#define EMMC_ENDBOOTOP_OFFSET                   0x00000018

#ifdef CONFIG_SECURE_BOOT
#define CONFIG_BOOTCOMMAND	"emmc open 0;movi r z f 0 20000000;emmc close 0;"       \
                                "movi read kernel 0 20080000;movi read rootfs 0 22000000 A00000;movi r d 0 28000000;bootm 20080000 22000000 28000000"
#define CONFIG_BOOTCOMMAND_UFS	"movi read scsi kernel %u 40080000;movi read scsi rootfs %u 42000000 A00000;movi r s d 0 48000000;bootm 40080000 42000000 48000000"
#else
#define CONFIG_BOOTCOMMAND	"movi read kernel 0 20080000;movi read rootfs 0 22000000 A00000;movi r d 0 28000000;bootm 20080000 22000000 28000000"
#define CONFIG_BOOTCOMMAND_UFS	"movi read scsi kernel %u 40080000;movi read scsi rootfs %u 42000000 A00000;movi r s d 0 48000000;bootm 40080000 42000000 48000000"
#endif

#define CONFIG_RECOVERYCOMMAND	\
		"emmc partition 0 10 0;"	\
		"mmc erase user 0 1072 1;"	\
		"movi r f 1 20000000;emmc open 0;movi w z f 0 20000000;emmc close 0;"	\
		"movi r b 1 20000000;emmc open 0;movi w z b 0 20000000;emmc close 0;"	\
		"movi r u 1 20000000;emmc open 0;movi w z u 0 20000000;emmc close 0;"	\
		"movi r e 1 20000000;emmc open 0;movi w z e 0 20000000;emmc close 0;"	\
		"movi r t 1 20000000;emmc open 0;movi w z t 0 20000000;emmc close 0;"	\
		"reset"

#define CONFIG_RECOVERYCOMMAND_USB	\
		""

#define CONFIG_FUSING_COMMAND_EMMC	\
		"emmc open 0;"			\
		"movi w z f 0 20000000;"	\
		"movi w z b 0 20008000;"	\
		"movi w z u 0 22000000;"	\
		"movi w z e 0 22100000;"	\
		"movi w z t 0 22200000;"	\
		"movi w l 0   22300000;"	\
		"emmc close 0;"			\
		"reset"

#define CONFIG_FUSING_COMMAND_SDMMC	\
		"movi w f 0 20000000;"	\
		"movi w b 0 20008000;"	\
		"movi w u 0 22000000;"	\
		"movi w e 0 22100000;"	\
		"movi w t 0 22200000;"	\
		"movi w l 0 22300000;"	\
		"reset"

#define CONFIG_FUSING_COMMAND_UFS	\
		"movi w s f 1 40000000;"	\
		"movi w s b 1 40008000;"	\
		"movi w s u 1 42000000;"	\
		"movi w s e 1 42100000;"	\
		"movi w s t 1 42200000;"	\
		"movi w s l 0 42300000;"	\
		"reset"

#define CONFIG_FDT_LOAD_COMMAND_EMMC	\
		"movi r d 0 28000000;"	\
		"fdt addr 28000000;"	\

#define CONFIG_FDT_LOAD_COMMAND_SDMMC	\
		"movi r d 0 28000000;"	\
		"fdt addr 28000000;"	\

#define CONFIG_FDT_LOAD_COMMAND_UFS	\
		"movi r s d 0 28000000;"	\
		"fdt addr 28000000;"	\

#define CONFIG_FDT_SAVE_COMMAND_EMMC	\
		"movi w d 0 28000000;"

#define CONFIG_FDT_SAVE_COMMAND_SDMMC	\
		"movi w d 0 28000000;"

#define CONFIG_FDT_SAVE_COMMAND_UFS	\
		"movi w s d 0 28000000;"

#ifdef CONFIG_MACH_SMDK7580
#define CONFIG_GPT_ENV_SMDK7580 \
		"uuid_disk=12345678-1234-1234-1234-123456789012;"\
		"name=fat,size=13000MiB,uuid=EBD0A0A2-B9E5-4433-87C0-68B6B72699C7;"\
		"name=system,size=600MiB,uuid=0FC63DAF-8483-4772-8E79-3D69D8477DE4;"\
		"name=userdata,size=14300MiB,uuid=0FC63DAF-8483-4772-8E79-3D69D8477DE4;"\
		"name=cache,size=300MiB,uuid=0FC63DAF-8483-4772-8E79-3D69D8477DE4;"\
		"name=modem,size=30MiB,uuid=0FC63DAF-8483-4772-8E79-3D69D8477DE4;"\
		"name=efs,size=5MiB,uuid=0FC63DAF-8483-4772-8E79-3D69D8477DE4"
#define CONFIG_GPT_ENV CONFIG_GPT_ENV_SMDK7580
#endif

/* Configuration for factory reset mode */
#define CONFIG_FACTORY_RESET_MODE       0xf
#define CONFIG_FACTORY_RESET_BOOTCOMMAND        \
                "ext3format mmc 0:3;ext3format mmc 0:4;"		\
                "movi read kernel 0 20080000;"                          \
                "movi read rootfs 0 22000000 A00000;"                   \
				"movi r d 0 28000000;"									\
                "bootm 20080000 22000000 28000000"

/* Miscellaneous configurable options */
#define CONFIG_SYS_LONGHELP		/* undef to save memory */
#define CONFIG_SYS_HUSH_PARSER		/* use "hush" command parser	*/
#define CONFIG_SYS_PROMPT		CONFIG_BOARD_NAME " # "
#define CONFIG_SYS_CBSIZE		256	/* Console I/O Buffer Size */
#define CONFIG_SYS_PBSIZE		384	/* Print Buffer Size */
#define CONFIG_SYS_MAXARGS		16	/* max number of command args */
#define CONFIG_DEFAULT_CONSOLE		"console=ttySAC1,115200n8\0"
/* Boot Argument Buffer Size */
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE
/* memtest works on */
#define CONFIG_SYS_MEMTEST_START	CONFIG_SYS_SDRAM_BASE
#define CONFIG_SYS_MEMTEST_END		(CONFIG_SYS_SDRAM_BASE + 0x5E00000)
#define CONFIG_SYS_LOAD_ADDR		CONFIG_SYS_TEXT_BASE

#define CONFIG_SYS_HZ			1000

/* Stack sizes */
#define CONFIG_STACKSIZE		(256 << 10)	/* 256KB */

#define MEMCLK			825
/* SMDK7580 has 2GB DRAM */
#define CONFIG_NR_DRAM_BANKS	8
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
#define PHYS_SDRAM_6_SIZE	SDRAM_BANK_SIZE
#define PHYS_SDRAM_7		(CONFIG_SYS_SDRAM_BASE + (6 * SDRAM_BANK_SIZE))
#define PHYS_SDRAM_7_SIZE	SDRAM_BANK_SIZE
#define PHYS_SDRAM_8		(CONFIG_SYS_SDRAM_BASE + (7 * SDRAM_BANK_SIZE))
#define PHYS_SDRAM_8_SIZE	(SDRAM_BANK_SIZE - \
				CONFIG_TRUSTZONE_RESERVED_DRAM)

#define CONFIG_SYS_MONITOR_BASE	0x00000000

/* FLASH and environment organization */
#define CONFIG_SYS_NO_FLASH
#undef CONFIG_CMD_IMLS
#define CONFIG_IDENT_STRING		" for " CONFIG_BOARD_NAME

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
#define CONFIG_EFI_PARTITION

#define CONFIG_NVDATA_PARTITION
#define CFG_PARTITION_START     0x6400000
#define CONFIG_IRAM_STACK	(CONFIG_PHY_IRAM_NS_BASE + 0x1000)

#define CONFIG_SYS_INIT_SP_ADDR	(CONFIG_SYS_LOAD_ADDR - 0x100000)

#if defined(CONFIG_EFI_PARTITION) && defined(CONFIG_DOS_PARTITION)
#error Choose MBR(DOS) or GPT(EFI) partition support
#endif

#if !defined(CONFIG_EFI_PARTITION) && !defined(CONFIG_DOS_PARTITION)
#error MBR(DOS) or GPT(EFI) partition support must be selected
#endif

#ifdef CONFIG_EFI_PARTITION
#define CONFIG_CMD_GPT
#define HAVE_BLOCK_DEVICE
#define CONFIG_LIB_UUID
#define CONFIG_SYS_64BIT_LBA
#endif

/* devicetree support */
#define CONFIG_OF_LIBFDT

/* secondary boot information */
#define CONFIG_SECOND_BOOT_INFORM_SIZE	(0x4)
#define CONFIG_SECOND_BOOT_INFORM_BASE	(CONFIG_PHY_IRAM_NS_BASE + \
						nscode_end - nscode_base)

/* image inforamtion base address for SMC */
#define CONFIG_SMC_IMAGE_INFORM_BASE	(CONFIG_PHY_IRAM_NS_BASE + \
						nscode_end - nscode_base + \
						CONFIG_SECOND_BOOT_INFORM_SIZE)
/* Offset for pmu reset status */
#define RST_STAT_OFFSET			0x404

/* RST_STAT */
#define SWRESET			(1 << 29)
#define WRESET			(1 << 28)
#define A5IS_WDTRESET		(1 << 27)
#define SSS_WDTRESET		(1 << 26)
#define APL_WDTRESET		(1 << 24)
#define CPU_WDTRESET		(1 << 23)
#define PINRESET		(1 << 16)
#define APL_WRESET3		(1 << 7)
#define APL_WRESET2		(1 << 6)
#define APL_WRESET1		(1 << 5)
#define APL_WRESET0		(1 << 4)
#define CPU_WRESET3		(1 << 3)
#define CPU_WRESET2		(1 << 2)
#define CPU_WRESET1		(1 << 1)
#define CPU_WRESET0		(1 << 0)

/* LCD on U-BOOT */
#define CONFIG_VIDEO
#define CONFIG_EXYNOS_LCD_ON_UBOOT
#define CONFIG_DECON_LCD_S6E3FA0
#define CONFIG_DISPLAY_LOGO
#define DISP_RESOL 3 /* 2=INT_WQHD_EXT_FHD(1/2 MIC) */

#endif	/* __CONFIG_H */
