/*
 * (C) Copyright 2010 Samsung Electronics
 * Minkyu Kang <mk7.kang@samsung.com>
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
 *
 */

#ifndef __ASM_ARM_ARCH_CLK_H_
#define __ASM_ARM_ARCH_CLK_H_

#define APLL	0
#define MPLL	1
#define EPLL	2
#define HPLL	3
#define VPLL	4
#define KPLL	5
#define BPLL	6
#define CPLL	7
#define SPLL	8
#define UPLL	9

/* EXYNOS5260, EXYNOS5430 PLL */
#define EGL_PLL		0
#define EGL_DPLL	1
#define KFC_PLL		2
#define MEM_PLL		3
#define BUS_PLL		4
#define MEDIA_PLL	5

/* EXYNOS5430 PLL */
#define MFC_PLL		5
#define ISP_PLL		6
#define MPHY_PLL	7

/* EXYNOS7420 PLL */
#define ATLAS_PLL	0
#define APOLLO_PLL	2
#define MIF_PLL		3
#define BUS0_PLL	6
#define BUS1_PLL	7
#define CCI_PLL		8

#define MPLL_PRE_DIV	2

/* EXYNOS7580 PLL */
#define PLL_CPU        0
#define PLL_APL        1
#define PLL_MEM0       2
#define PLL_MEDIA      3
#define PLL_BUS        4
#define PLL_USB        8

unsigned long get_pll_clk(int pllreg);
unsigned long get_arm_clk(void);
unsigned long get_pwm_clk(void);
unsigned long get_uart_clk(int dev_index);
unsigned long get_mmc_clk(int dev_index);
unsigned long get_usbdrd_clk(void);
void set_mmc_clk(int dev_index, unsigned int div);
unsigned long get_lcd_clk(void);
void set_lcd_clk(void);
void set_mipi_clk(void);

void set_ufs_clk(int dev_index);
unsigned long get_ufs_clk(int dev_index);
#endif
