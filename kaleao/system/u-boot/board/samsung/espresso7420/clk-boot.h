/*
 * Clock setup for EXYNOS7420
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

#ifndef __CLK_BOOT_H__
#define __CLK_BOOT_H__

int clk_init(unsigned int cmu_resume);
unsigned int clk_get_atlas_pll_freq(void);
unsigned int clk_get_apollo_pll_freq(void);
unsigned int clk_get_mif_pll_freq(void);


int clk_init_big_cpu(void);
int clk_init_little_cpu(void);
int clk_init_mif(void);
int clk_init_g3d(void);
int clk_init_cam0(void);
int clk_init_cam1(void);
int clk_init_isp0(void);
int clk_init_isp1(void);
int clk_init_vpp(void);
int clk_init_disp(void);
int clk_init_aud(void);
//FSYS0 is Retention Block. No need to reconfigure
int clk_init_fsys0(void);
//FSYS1 is Retention Block. No need to reconfigure
int clk_init_fsys1(void);
int clk_init_g2d(void);
int clk_init_mscl(void);
int clk_init_mfc(void);
int clk_init_hevc(void);

#endif /* __CLK_BOOT_H__ */
