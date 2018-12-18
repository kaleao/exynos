/*
 * Machine Specific Values for EXYNOS7580
 *
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

#ifndef _EXYNOS7580_SETUP_H
#define _EXYNOS7580_SETUP_H

#include <config.h>
#include <version.h>
#include <asm/arch/cpu.h>

void sdelay(unsigned long);
extern void check_pll_unlock(unsigned int addr);
extern void check_mux_stat(unsigned int addr, unsigned int mask_bit);
extern unsigned int second_boot_info_base;
extern unsigned int smc_image_info_base;
#endif
