/*
 * Copyright (C) 2014 Samsung Electronics
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

#include <common.h>
#include <config.h>
#include <asm/arch/pinmux.h>
#include <asm/arch/mmc.h>
#include <asm/arch/clock.h>
#include <asm/arch/power.h>
#include <asm/arch/clk.h>
#include <asm/arch/smc.h>

/* 1st_dev: eMMC, 2nd_dev: SD/MMC */
#define SDMMC_SECOND_DEV	0x10
#define USB_SECOND_DEV		0x0
#define USB_SECOND_DEV_SD_MMC       0b100
#define USB_SECOND_DEV_RESERVED     0b110
#define USB_SECOND_DEV_EMMC         0b1000
#define USB_SECOND_DEV_UFS          0b1010
#define USB_SECOND_DEV_SPI          0b1100
#define SECOND_BOOT_MODE	0xFEED0002
#define UBOOT			0x1
#define TZSW			0x2
#define SIGNATURE_CHECK_FAIL	-1
#define SIGNATURE_CHECK_SUCCESS	(0)

extern unsigned int second_boot_info_base;

#ifndef CONFIG_SG_BINARY
/* find boot device for secondary booting */
static int find_second_boot_dev(void)
{
	struct exynos7420_power *pmu = (struct exynos7420_power *)EXYNOS7420_POWER_BASE;
	unsigned int om_status =  readl(&pmu->om_stat) & 0x14;
    int ret = -1;

    switch(om_status) {
        case SDMMC_SECOND_DEV:
            writel(0x1, second_boot_info_base);
            ret = BOOT_SEC_DEV;
            break;
        case USB_SECOND_DEV:
        case USB_SECOND_DEV_SD_MMC:
        case USB_SECOND_DEV_RESERVED:
        case USB_SECOND_DEV_EMMC:
		case USB_SECOND_DEV_UFS:
        case USB_SECOND_DEV_SPI:
            writel(0x2, second_boot_info_base);
            ret = BOOT_USB;
            break;
        default:
            ret = -1;
    }

    return ret;

#if 0
	if (om_status == SDMMC_SECOND_DEV) {
		writel(0x1, second_boot_info_base);
		return BOOT_SEC_DEV;
	} else if (om_status == USB_SECOND_DEV) {
		writel(0x2, second_boot_info_base);
		return BOOT_USB;
	} else
	    return -1;
#endif
}

static unsigned int device(unsigned int boot_dev)
{
	switch(boot_dev) {
	case BOOT_MMCSD:
	case BOOT_SEC_DEV:
		boot_dev = SDMMC_CH2;
		break;
	case BOOT_EMMC_4_4:
		boot_dev = EMMC;
		break;
	case BOOT_USB:
		boot_dev = USB;
		break;
	case BOOT_UFS:
		boot_dev = UFS;
		break;
	default:
		while(1);
	}
	return boot_dev;
}

static int ld_image_from_2nd_dev(int image)
{
	int ret = SIGNATURE_CHECK_FAIL;
	unsigned int boot_dev = 0;

	boot_dev = find_second_boot_dev();

	/* sdmmc enumerate */
	if (device(boot_dev) == SDMMC_CH2)
		sdmmc_enumerate();

	switch (image) {
	case UBOOT:
		/* load uboot from 2nd dev */
		ret = load_uboot_image(device(boot_dev));
		break;
	case TZSW:
		/* load uboot from 2nd dev */
		ret = coldboot(device(boot_dev));
		break;
	}

	if (ret != SIGNATURE_CHECK_SUCCESS)
		while(1);

	return boot_dev;
}

void jump_to_uboot(void)
{
	unsigned int om_status = readl(EXYNOS7420_POWER_BASE + INFORM3_OFFSET);
	unsigned int boot_dev = 0;
	int ret = 0;

	/* TODO : find second boot function */
	if (find_second_boot() == SECOND_BOOT_MODE)
		boot_dev = find_second_boot_dev();

	if (!boot_dev)
		boot_dev = om_status;

	/* Load u-boot image to ram */
	ret = load_uboot_image(device(boot_dev));
	if (ret != SIGNATURE_CHECK_SUCCESS)
		boot_dev = ld_image_from_2nd_dev(UBOOT);

	/* Load tzsw image & U-Boot boot */
	ret = coldboot(device(boot_dev));
	if (ret != SIGNATURE_CHECK_SUCCESS)
		/*
		 * SMC for Coldboot doesn't load any images.
		 * So, it doesn't have to try to load the image again.
		 */
		while (1);

}
#endif

void board_init_f(unsigned long bootflag)
{
	/* Jump to U-Boot image */
#ifndef CONFIG_SG_BINARY
	jump_to_uboot();
#endif

	/* Never returns Here */
}

/* Place Holders */
void board_init_r(gd_t *id, ulong dest_addr)
{
	/* Function attribute is no-return */
	/* This Function never executes */
	while (1)
		;
}

void save_boot_params(u32 r0, u32 r1, u32 r2, u32 r3) {}
