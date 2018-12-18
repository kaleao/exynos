/*
 * Copyright (C) 2012 Samsung Electronics
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

#include<common.h>
#include<config.h>
#include <asm/arch/power.h>


#define FIRST_EMMC_SECOND_SDMMC_DEV_0	0x20
#define FIRST_EMMC_SECOND_SDMMC_DEV_1	0x24
#define FIRST_EMMC_SECOND_SDMMC_DEV_2	0x26

#define FIRST_EMMC_SECOND_USB_DEV_0	0x0
#define FIRST_EMMC_SECOND_USB_DEV_1	0x6
#define FIRST_EMMC_SECOND_USB_DEV_2	0x8
#define FIRST_EMMC_SECOND_USB_DEV_3	0x28

#define FIRST_SD_SECOND_USB_DEV	0x4

#define SIGNATURE_CHECK_FAIL	-1
#define SECOND_BOOT_MODE	0xFEED0002
#define SPI_SDMMC_DEV 0x2C
#define SDMMC_SECOND_DEV	0x101
#define USB_SECOND_DEV1		0x007
#define USB_SECOND_DEV2		0x001

/*
* Copy U-boot from mmc to RAM:
* COPY_BL2_FNPTR_ADDR: Address in iRAM, which Contains
* Pointer to API (Data transfer from mmc to ram)
*/

static int find_second_boot_dev(void)
{
	struct exynos4_power *pmu = (struct exynos4_power *)EXYNOS4_POWER_BASE;
	unsigned int om_status =  readl(&pmu->om_stat) & 0x3f;

	if (om_status == SDMMC_SECOND_DEV) {
		writel(0x1, CONFIG_SECONDARY_BOOT_INFORM_BASE);
		return BOOT_SEC_DEV;
	} else if ((om_status == USB_SECOND_DEV1) || (om_status == USB_SECOND_DEV2)) {
		writel(0x2, CONFIG_SECONDARY_BOOT_INFORM_BASE);
		return BOOT_USB;
	} else
		return -1;
}

void copy_uboot_to_ram(unsigned int boot_dev)
{
	int ret = 0;

	switch (boot_dev) {
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
	}
	/* Load u-boot image to ram */
	ret = load_uboot_image(boot_dev);
	if (ret == SIGNATURE_CHECK_FAIL) {
		sdmmc_enumerate();
		if (find_second_boot_dev() == BOOT_SEC_DEV)
			boot_dev = SDMMC_CH2;
		ret = load_uboot_image(boot_dev);
		if (ret == SIGNATURE_CHECK_FAIL)
			while (1);
	}

	/* Load tzsw image & U-Boot boot */
	ret = coldboot(boot_dev);
	if (ret == SIGNATURE_CHECK_FAIL) {
		sdmmc_enumerate();
		if (find_second_boot_dev() == BOOT_SEC_DEV)
			boot_dev = SDMMC_CH2;
		ret = coldboot(boot_dev);
		if (ret == SIGNATURE_CHECK_FAIL)
			while (1);
	}
}

void load_uboot(void)
{
	unsigned int om_status = readl(EXYNOS4_POWER_BASE + INFORM3_OFFSET);
	unsigned int boot_dev = 0;

	/* TODO : find second boot function */
	if (find_second_boot() == SECOND_BOOT_MODE)
		boot_dev = find_second_boot_dev();

	if (!boot_dev)
		boot_dev = om_status;

	copy_uboot_to_ram(boot_dev);
}

void board_init_f(unsigned long bootflag)
{
	__attribute__((noreturn)) void (*uboot)(void);
	load_uboot();

	/* Jump to U-Boot image */
	/*
	uboot = (void *)CONFIG_SYS_TEXT_BASE;
	(*uboot)();
	*/
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
