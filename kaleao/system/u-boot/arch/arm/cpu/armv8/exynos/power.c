/*
 * Copyright (C) 2012 Samsung Electronics
 * Donghwa Lee <dh09.lee@samsung.com>
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
#include <asm/io.h>
#include <asm/arch/power.h>

void exynos5_set_usbhost_phy_ctrl(unsigned int enable)
{
	struct exynos5_power *power =
		(struct exynos5_power *)samsung_get_base_power();

	if (enable) {
		/* Enabling USBHOST_PHY */
		setbits_le32(&power->usbhost_phy_control,
				POWER_USB_HOST_PHY_CTRL_EN);
	} else {
		/* Disabling USBHOST_PHY */
		clrbits_le32(&power->usbhost_phy_control,
				POWER_USB_HOST_PHY_CTRL_EN);
	}
}

void set_usbhost_phy_ctrl(unsigned int enable)
{
	if (cpu_is_exynos5())
		exynos5_set_usbhost_phy_ctrl(enable);
	else if (cpu_is_exynos7())
		exynos5_set_usbhost_phy_ctrl(enable);
}
