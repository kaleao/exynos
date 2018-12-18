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

#ifndef __ASM_ARM_ARCH_CLOCK_H_
#define __ASM_ARM_ARCH_CLOCK_H_

#ifndef __ASSEMBLY__
#if defined(CONFIG_CPU_EXYNOS3250) || defined(CONFIG_CPU_EXYNOS3472)
struct exynos4_clock {
	/*CMU_LEFTBUS*/
	unsigned char	res1[0x4200];			/* 0x1003_4200 */
	unsigned int	src_leftbus;
	unsigned char	res2[0x1fc];
	unsigned int	mux_stat_leftbus;
	unsigned char	res4[0xfc];
	unsigned int	div_leftbus;
	unsigned char	res5[0xfc];
	unsigned int	div_stat_leftbus;
	unsigned char	res6[0xfc];
	unsigned int	gate_bus_leftbus;;
	unsigned char	res7[0xfc];
	unsigned int	gate_ip_leftbus;
	unsigned char	res8[0x1fc];
	unsigned int	clkout_leftbus;			/* 0x1003_4A00 */
	unsigned int	clkout_leftbus_div_stat;
	/*CMU_RIGHTBUS*/
	unsigned char	res9[0x37f8];
	unsigned int	src_rightbus;			/* 0x1003_8200 */
	unsigned char	res10[0x1fc];
	unsigned int	mux_stat_rightbus;
	unsigned char	res11[0xfc];
	unsigned int	div_rightbus;
	unsigned char	res12[0xfc];
	unsigned int	div_stat_rightbus;
	unsigned char	res13[0xfc];
	unsigned int	gate_bus_rightbus;
	unsigned char	res14[0x5c];
	unsigned int	gate_bus_perir;
	unsigned char	res15[0x9c];
	unsigned int	gate_ip_rightbus;
	unsigned char	res16[0x15c];
	unsigned int	gate_ip_perir;
	unsigned char	res17[0x9c];
	unsigned int	clkout_rightbus;		/* 0x1003_8A00 */
	unsigned int	clkout_rightbus_div_stat;
	/*CMU_TOP*/
	unsigned char	res18[0x3608];
	unsigned int	mpll_lock;			/* 0x1003_C010 */
	unsigned char	res20[0xc];
	unsigned int	vpll_lock;
	unsigned char	res101[0xc];
	unsigned int	upll_lock;
	unsigned char	res21[0xdc];
	unsigned int	mpll_con0;			/* 0x1003_C110 */
	unsigned int	mpll_con1;
	unsigned int	mpll_con2;
	unsigned char	res22[0x4];
	unsigned int	vpll_con0;			/* 0x1003_C120 */
	unsigned int	vpll_con1;
	unsigned int	vpll_con2;
	unsigned char	res222[0x4];
	unsigned int	upll_con0;			/* 0x1003_C130 */
	unsigned int	upll_con1;
	unsigned int	upll_con2;
	unsigned char	res23[0xd4];
	unsigned int	src_top0;			/* 0x1003_C210 */
	unsigned int	src_top1;
	unsigned char	res24[0x8];
	unsigned int	src_cam;			/* 0x1003_C220*/
	unsigned char	res105[0x4];
	unsigned int	src_mfc;
	unsigned int	src_g3d;
	unsigned char	res102[0x4];
	unsigned int	src_lcd0;
	unsigned int	src_isp;
	unsigned char	res103[0x4];
	unsigned int	src_fsys;
	unsigned char	res25[0xc];
	unsigned int	src_peril0;
	unsigned int	src_peril1;
	unsigned int	src_cam1;
	unsigned char	res26[0xb4];
	unsigned int	src_mask_top;			/* 0x1003_C310 */
	unsigned char	res27[0xc];
	unsigned int	src_mask_cam;
	unsigned char	res28[0x10];
	unsigned int	src_mask_lcd0;
	unsigned int	src_mask_isp;
	unsigned char	res104[0x4];
	unsigned int	src_mask_fsys;
	unsigned char	res29[0xc];
	unsigned int	src_mask_peril0;
	unsigned int	src_mask_peril1;
	unsigned char	res30[0xb8];
	unsigned int	mux_stat_top;			/* 0x1003_C410 */
	unsigned int	mux_stat_top1;
	unsigned char	res31[0x10];
	unsigned int	mux_stat_mfc;
	unsigned int	mux_stat_g3d;
	unsigned char	res32[0xe0];
	unsigned int	div_top;
	unsigned char	res33[0xc];
	unsigned int	div_cam;
	unsigned char	res106[0x4];
	unsigned int	div_mfc;
	unsigned int	div_g3d;
	unsigned char	res107[0x4];
	unsigned int	div_lcd0;
	unsigned int	div_isp;
	unsigned char	res108[0x4];
	unsigned int	div_fsys0;
	unsigned int	div_fsys1;
	unsigned int	div_fsys2;
	unsigned int	div_fsys3;
	unsigned int	div_peril0;
	unsigned int	div_peril1;
	unsigned int	div_peril2;
	unsigned int	div_peril3;
	unsigned int	div_peril4;
	unsigned int	div_peril5;
	unsigned int	div_cam1;
	unsigned char	res34[0x14];
	unsigned int	div2_ratio;
	unsigned char	res35[0x8c];
	unsigned int	div_stat_top;			/* 0x1003_C610 */
	unsigned char	res36[0xc];
	unsigned int	div_stat_cam;
	unsigned char	res110[0x4];
	unsigned int	div_stat_mfc;
	unsigned int	div_stat_g3d;
	unsigned char	res111[0x4];
	unsigned int	div_stat_lcd0;
	unsigned int	div_stat_isp;
	unsigned char	res112[0x4];
	unsigned int	div_stat_fsys0;
	unsigned int	div_stat_fsys1;
	unsigned int	div_stat_fsys2;
	unsigned char	res113[0x4];
	unsigned int	div_stat_peril0;
	unsigned int	div_stat_peril1;
	unsigned int	div_stat_peril2;
	unsigned int	div_stat_peril3;
	unsigned int	div_stat_peril4;
	unsigned int	div_stat_peril5;
	unsigned int	div_stat_cam1;
	unsigned char	res37[0x14];
	unsigned int	div2_stat;
	unsigned char	res114[0x9c];
	unsigned int	gate_bus_cam;			/* 0x1003_C720 */
	unsigned char	res115[0x4];
	unsigned int	gate_bus_mfc;
	unsigned int	gate_bus_g3d;
	unsigned char	res116[0x4];
	unsigned int	gate_bus_lcd0;
	unsigned int	gate_bus_cam1;
	unsigned char	res117[0x4];
	unsigned int	gate_bus_fsys0;
	unsigned int	gate_bus_fsys1;
	unsigned char	res118[0x8];
	unsigned int	gate_bus_peril;
	unsigned char	res119[0xcc];
	unsigned int	gate_sclk_cam;			/* 0x1003_C820 */
	unsigned char	res120[0x4];
	unsigned int	gate_sclk_mfc;
	unsigned int	gate_sclk_g3d;
	unsigned char	res121[0x4];
	unsigned int	gate_sclk_lcd0;
	unsigned int	gate_sclk_isp;
	unsigned char	res122[0x4];
	unsigned int	gate_sclk_fsys;
	unsigned char	res123[0xc];
	unsigned int	gate_sclk_peril;
	unsigned char	res38[0xcc];
	unsigned int	gate_ip_cam;			/* 0x1003_C920 */
	unsigned char	res1283[0x4];
	unsigned int	gate_ip_mfc;
	unsigned int	gate_ip_g3d;
	unsigned char	res124[0x4];
	unsigned int	gate_ip_lcd0;
	unsigned int	gate_ip_isp;
	unsigned char	res39[0x4];
	unsigned int	gate_ip_fsys;
	unsigned char	res40[0xc];
	unsigned int	gate_ip_peril;
	unsigned char	res42[0x1c];
	unsigned int	gate_block;
	unsigned char	res43[0x8c];
	unsigned int	clkout_cmu_top;			/* 0x1003_CA00 */
	unsigned int	clkout_cmu_top_div_stat;

	/*CMU_CPU	*/
	unsigned char	res44[0x75f8];
	unsigned int	apll_lock;			/* 0x1004_4000 */
	unsigned char	res45[0xfc];
	unsigned int	apll_con0;			/* 0x1004_4100 */
	unsigned int	apll_con1;
	unsigned int	apll_con2;
	unsigned char	res46[0xf4];
	unsigned int	src_cpu;			/* 0x1004_4200 */
	unsigned char	res47[0x1fc];
	unsigned int	mux_stat_cpu;			/* 0x1004_4400 */
	unsigned char	res48[0xfc];
	unsigned int	div_cpu0;			/* 0x1004_4500 */
	unsigned int	div_cpu1;
	unsigned char	res49[0xf8];
	unsigned int	div_stat_cpu0;			/* 0x1004_4600 */
	unsigned int	div_stat_cpu1;
	unsigned char	res50[0xf8];
	unsigned int	gate_bus_cpu;			/* 0x1004_4700 */
	unsigned char	res51[0xfc];
	unsigned int	gate_sclk_cpu;			/* 0x1004_4800 */
	unsigned char	res52[0xfc];
	unsigned int	gate_ip_cpu;			/* 0x1004_4900 */
	unsigned char	res53[0xfc];
	unsigned int	clkout_cmu_cpu;			/* 0x1004_4A00 */
	unsigned int	clkout_cmu_cpu_div_stat;
	unsigned char	res54[0x5f8];
	unsigned int	armclk_stopctrl;		/* 0x1004_5000 */
	unsigned int	atclk_stopctrl;
	unsigned int	arm_ema_ctrl;
	unsigned int	arm_ema_status;
	unsigned char	res855[0x10];
	unsigned int	pwr_ctrl;
	unsigned int	pwr_ctr2;
	unsigned char	res55[0xD8];
	unsigned int	apll_con0_l8;			/* 0x1004_5100 */
	unsigned int	apll_con0_l7;
	unsigned int	apll_con0_l6;
	unsigned int	apll_con0_l5;
	unsigned int	apll_con0_l4;
	unsigned int	apll_con0_l3;
	unsigned int	apll_con0_l2;
	unsigned int	apll_con0_l1;
	unsigned int	iem_control;
	unsigned char	res56[0xdc];
	unsigned int	apll_con1_l8;			/* 0x1004_5200 */
	unsigned int	apll_con1_l7;
	unsigned int	apll_con1_l6;
	unsigned int	apll_con1_l5;
	unsigned int	apll_con1_l4;
	unsigned int	apll_con1_l3;
	unsigned int	apll_con1_l2;
	unsigned int	apll_con1_l1;
	unsigned char	res57[0xe0];
	unsigned int	div_iem_l8;			/* 0x1004_5300 */
	unsigned int	div_iem_l7;
	unsigned int	div_iem_l6;
	unsigned int	div_iem_l5;
	unsigned int	div_iem_l4;
	unsigned int	div_iem_l3;
	unsigned int	div_iem_l2;
	unsigned int	div_iem_l1;
	unsigned char	res58[0xf0];
	unsigned int	cpu_status;			/* 0x1004_5410 */

	/*CMU_ISP*/
	unsigned char	res59[0x2eec];
	unsigned int	div_isp0;			/* 0x1004_8300 */
	unsigned int	div_isp1;
	unsigned char	res60[0xf8];
	unsigned int 	div_stat_isp0;			/* 0x1004_8400 */
	unsigned int 	div_stat_isp1;
	unsigned char	res61[0x2f8];
	unsigned int 	gate_bus_isp0;			/* 0x1004_8700 */
	unsigned int 	gate_bus_isp1;
	unsigned int 	gate_bus_isp2;
	unsigned int 	gate_bus_isp3;
	unsigned char	res62[0xf0];
	unsigned int 	gate_ip_isp0;			/* 0x1004_8800 */
	unsigned int 	gate_ip_isp1;
	unsigned char	res63[0xf8];
	unsigned int 	isp_gate_sclk_isp;		/* 0x1004_8900 */
	unsigned char	res64[0xfc];
	unsigned int 	clkout_cmu_isp;			/* 0x1004_8A00 */
	unsigned int 	clkout_cmu_isp_div_stat;
	unsigned char	res880[0xf8];
	unsigned int	isp_spare0;
	unsigned int	isp_spare1;
	unsigned int	isp_spare2;
	unsigned int	isp_spare3;
	/*CMU_ACP*/
	unsigned char	res65[0x4077f0];
	unsigned int 	src_acp;			/* 0x1045_0300 */
	unsigned int 	src_mask_acp;
	unsigned char	res66[0xf8];
	unsigned int 	mux_stat_acp0;			/* 0x1045_0400 */
	unsigned int 	mux_stat_acp1;
	unsigned char	res67[0xf8];
	unsigned int 	div_acp0;			/* 0x1045_0500 */
	unsigned int 	div_acp1;
	unsigned char	res68[0xf8];
	unsigned int 	div_stat_acp0;			/* 0x1045_0600 */
	unsigned int 	div_stat_acp1;
	unsigned char	res69[0xf8];
	unsigned int 	gate_bus_acp0;			/* 0x1045_0700 */
	unsigned int 	gate_bus_acp1;
	unsigned char	res70[0xf8];
	unsigned int 	gate_sclk_acp;			/* 0x1045_0800 */
	unsigned char	res71[0xfc];
	unsigned int 	gate_ip_acp;			/* 0x1045_0900 */
	unsigned int 	gate_ip_acp1;
	unsigned char	res72[0xf8];
	unsigned int 	clkout_cmu_acp;			/* 0x1045_0A00 */
	unsigned int 	clkout_cmu_acp_div_stat;
	unsigned char	res790[0x5f8];
	unsigned int	dcgidx_map0;
	unsigned int	dcgidx_map1;
	unsigned int	dcgidx_map2;
	unsigned char	res791[0x14];
	unsigned int	dcgperf_map0;
	unsigned int	dcgperf_map1;
	unsigned char	res792[0x18];
	unsigned int	dvcidx_map;
	unsigned char	res793[0x1c];
	unsigned int	freq_cpu;
	unsigned int	freq_dpm;
	unsigned char	res794[0x18];
	unsigned int	dvsemclk_en;
	unsigned int	maxperf;
	unsigned char	res795[0xf78];
	unsigned int	acp_spare0;			/*0x1045_2000*/
	unsigned int	acp_spare1;
	unsigned int	acp_spare2;
	unsigned int	acp_spare3;
	unsigned int	acp_spare4;

	/*CMU_DMC*/
	unsigned char	res73[0x16DFF4];
	unsigned char	res74[0x110];
	unsigned int 	bpll_lock;			/* 0x105c_0118 */
	unsigned char	res76[0xfc];
	unsigned int 	bpll_con0;			/* 0x105c_0218 */
	unsigned int 	bpll_con1;
	unsigned int 	bpll_con2;
	unsigned char	res77[0xdc];
	unsigned int 	src_dmc;			/* 0x105c_0300 */
	unsigned char	res78[0xfc];
	unsigned int 	mux_stat_dmc;			/* 0x105c_0400 */
	unsigned char	res79[0xfc];
	unsigned int 	div_dyn_freq_scale;		/* 0x105c_0500 */
	unsigned int 	div_dmc1;
	unsigned char	res80[0xf8];
	unsigned int 	div_stat_dmc0;			/* 0x105c_0600 */
	unsigned int 	div_stat_dmc1;
	unsigned char	res81[0xf8];
	unsigned int 	gate_bus_dmc0;			/* 0x105c_0700 */
	unsigned int 	gate_bus_dmc1;
	unsigned int 	gate_bus_dmc2;
	unsigned int 	gate_bus_dmc3;
	unsigned char	res82[0xf0];
	unsigned int 	gate_clk_dmc;			/* 0x105c_0800 */
	unsigned char	res83[0xfc];
	unsigned int 	gate_ip_dmc0;			/* 0x105c_0900 */
	unsigned int 	gate_ip_dmc1;
	unsigned char	res84[0xf8];
	unsigned int 	clkout_cmu_dmc;			/* 0x105c_0A00 */
	unsigned int 	clkout_cmu_dmc_div_stat;
	unsigned char	res841[0x688];
	unsigned int	dmc_freq_ctrl;			/* 0x105c_1090 */
	unsigned int	dmc_pause_ctrl;
	unsigned int	ddrphy_lock_ctrl;
	unsigned char	res842[0x74];
	unsigned int	epll_lock;
	unsigned int	epll_con0;
	unsigned int 	epll_con1;
	unsigned int 	epll_con2;
	unsigned int	clk_src_epll;
	unsigned int	clk_mux_stat_epll;
	unsigned char	res843[0xED8];
	unsigned int	semaphore0;			/* 0x105c_2000 */
	unsigned int	semaphore1;
	unsigned int	semaphore2;
	unsigned int	cmu_dmc_spare0;
	unsigned int	cmu_dmc_spare1;
	unsigned char	res844[0xFEC];
	unsigned int	divdmc_ratio_dyn_clk_gate_con;	/* 0x105c_3000 */
};
#elif defined(CONFIG_CPU_EXYNOS4415)
struct exynos4_clock {
	unsigned char	res0[0x200];			/* 0x1003_4200 */
	unsigned int	src_cdrex;
	unsigned char	res1[0x3ffc];
	unsigned int	src_leftbus;
	unsigned char	res2[0x1fc];
	unsigned int	mux_stat_leftbus;
	unsigned char	res3[0xfc];
	unsigned int	div_leftbus;
	unsigned char	res4[0xfc];
	unsigned int	div_stat_leftbus;
	unsigned char	res5[0xfc];
	unsigned int	gate_bus_leftbus;
	unsigned char	res6[0x2c];
	unsigned int	gate_bus_image;
	unsigned char	res7[0xcc];
	unsigned int	gate_ip_leftbus;
	unsigned char	res8[0x12c];
	unsigned int	gate_ip_image;
	unsigned char	res9[0xcc];
	unsigned int	clkout_leftbus;			/* 0x1003_4A00 */
	unsigned int	clkout_leftbus_div_stat;
	unsigned char	res10[0x37f8];
	unsigned int	src_rightbus;			/* 0x1003_8200 */
	unsigned char	res11[0x1fc];
	unsigned int	mux_stat_rightbus;
	unsigned char	res12[0xfc];
	unsigned int	div_rightbus;
	unsigned char	res13[0xfc];
	unsigned int	div_stat_rightbus;
	unsigned char	res14[0xfc];
	unsigned int	gate_bus_rightbus;
	unsigned char	res15[0x5c];
	unsigned int	gate_bus_perir;
	unsigned char	res16[0x9c];
	unsigned int	gate_ip_rightbus;
	unsigned char	res17[0x15c];
	unsigned int	gate_ip_perir;
	unsigned char	res18[0x9c];
	unsigned int	clkout_rightbus;		/* 0x1003_8A00 */
	unsigned int	clkout_rightbus_div_stat;
	unsigned char	res19[0x3608];
	unsigned int	epll_lock;			/* 0x1003_C010 */
	unsigned char	res20[0xc];
	unsigned int	vpll_lock;
	unsigned char	res21[0xc];
	unsigned int	mphy_pll_lock;
	unsigned char	res22[0xdc];
	unsigned int	epll_con0;			/* 0x1003_C110 */
	unsigned int	epll_con1;
	unsigned int	epll_con2;
	unsigned char	res23[0x4];
	unsigned int	vpll_con0;			/* 0x1003_C120 */
	unsigned int	vpll_con1;
	unsigned int	vpll_con2;
	unsigned char	res24[0x4];
	unsigned int	mphy_pll_con0;
	unsigned int	mphy_pll_con1;
	unsigned int	mphy_pll_con2;
	unsigned char	res25[0xd4];
	unsigned int	src_top0;			/* 0x1003_C210 */
	unsigned int	src_top1;
	unsigned char	res26[0x8];
	unsigned int	src_cam0;
	unsigned int	src_tv;
	unsigned int	src_mfc;
	unsigned int	src_g3d0;
	unsigned char	res27[0x4];
	unsigned int	src_lcd0;
	unsigned int	src_isp;
	unsigned int	src_maudio;
	unsigned int	src_fsys;
	unsigned char	res28[0xc];
	unsigned int	src_peril0;
	unsigned int	src_peril1;
	unsigned int	src_cam1;
	unsigned char	res29[0xb4];
	unsigned int	src_mask_top;			/* 0x1003_C310 */
	unsigned char	res30[0xc];
	unsigned int	src_mask_cam0;
	unsigned int	src_mask_tv;
	unsigned char	res31[0xc];
	unsigned int	src_mask_lcd0;
	unsigned int	src_mask_isp;
	unsigned int	src_mask_maudio;
	unsigned int	src_mask_fsys;
	unsigned char	res32[0xc];
	unsigned int	src_mask_peril0;
	unsigned int	src_mask_peril1;
	unsigned char	res33[0xb8];
	unsigned int	mux_stat_top0;			/* 0x1003_C410 */
	unsigned int	mux_stat_top1;
	unsigned char	res34[0x10];
	unsigned int	mux_stat_mfc;
	unsigned int	mux_stat_g3d;
	unsigned char	res35[0x28];
	unsigned int	mux_stat_cam1;
	unsigned char	res36[0xb4];
	unsigned int	div_top;
	unsigned char	res37[0xc];
	unsigned int	div_cam0;
	unsigned int	div_tv;
	unsigned int	div_mfc;
	unsigned int	div_g3d;
	unsigned char	res38[0x4];
	unsigned int	div_lcd0;
	unsigned int	div_isp;
	unsigned int	div_maudio;
	unsigned int	div_fsys0;
	unsigned int	div_fsys1;
	unsigned int	div_fsys2;
	unsigned int	div_fsys3;
	unsigned int	div_peril0;
	unsigned int	div_peril1;
	unsigned int	div_peril2;
	unsigned int	div_peril3;
	unsigned int	div_peril4;
	unsigned int	div_peril5;
	unsigned int	div_cam1;
	unsigned int	div_isp1_ts;
	unsigned char	res39[0x10];
	unsigned int	div2_ratio;
	unsigned char	res40[0x8c];
	unsigned int	div_stat_top;			/* 0x1003_C610 */
	unsigned char	res41[0xc];
	unsigned int	div_stat_cam0;
	unsigned int	div_stat_tv;
	unsigned int	div_stat_mfc;
	unsigned int	div_stat_g3d;
	unsigned char	res42[0x4];
	unsigned int	div_stat_lcd0;
	unsigned int	div_stat_isp;
	unsigned int	div_stat_maudio;
	unsigned int	div_stat_fsys0;
	unsigned int	div_stat_fsys1;
	unsigned int	div_stat_fsys2;
	unsigned int	div_stat_fsys3;
	unsigned int	div_stat_peril0;
	unsigned int	div_stat_peril1;
	unsigned int	div_stat_peril2;
	unsigned int	div_stat_peril3;
	unsigned int	div_stat_peril4;
	unsigned int	div_stat_peril5;
	unsigned int	div_stat_cam1;
	unsigned char	res43[0x14];
	unsigned int	div2_stat;
	unsigned char	res44[0x9c];
	unsigned int	gate_bus_cam0;			/* 0x1003_C720 */
	unsigned int	gate_bus_tv;
	unsigned int	gate_bus_mfc;
	unsigned int	gate_bus_g3d;
	unsigned char	res45[0x4];
	unsigned int	gate_bus_lcd0;
	unsigned int	gate_bus_cam1;
	unsigned char	res46[0x4];
	unsigned int	gate_bus_fsys0;
	unsigned int	gate_bus_fsys1;
	unsigned char	res47[0x8];
	unsigned int	gate_bus_peril;
	unsigned char	res48[0xcc];
	unsigned int	gate_sclk_cam;			/* 0x1003_C820 */
	unsigned int	gate_sclk_tv;
	unsigned int	gate_sclk_mfc;
	unsigned int	gate_sclk_g3d;
	unsigned char	res49[0x4];
	unsigned int	gate_sclk_lcd0;
	unsigned int	gate_sclk_isp;
	unsigned int	gate_sclk_maudio;
	unsigned int	gate_sclk_fsys;
	unsigned char	res50[0xc];
	unsigned int	gate_sclk_peril;
	unsigned char	res51[0xcc];
	unsigned int	gate_ip_cam;			/* 0x1003_C920 */
	unsigned int	gate_ip_tv;
	unsigned int	gate_ip_mfc;
	unsigned int	gate_ip_g3d;
	unsigned char	res52[0x4];
	unsigned int	gate_ip_lcd0;
	unsigned int	gate_ip_isp;
	unsigned char	res53[0x4];
	unsigned int	gate_ip_fsys;
	unsigned char	res54[0xc];
	unsigned int	gate_ip_peril;
	unsigned char	res55[0x1c];
	unsigned int	gate_block;
	unsigned char	res56[0x8c];
	unsigned int	clkout_cmu_top;			/* 0x1003_CA00 */
	unsigned int	clkout_cmu_top_div_stat;
	unsigned char	res57[0x75f8];
	unsigned int	apll_lock;			/* 0x1004_4000 */
	unsigned char	res58[0xfc];
	unsigned int	apll_con0;			/* 0x1004_4100 */
	unsigned int	apll_con1;
	unsigned int	apll_con2;
	unsigned char	res59[0xf4];
	unsigned int	src_cpu;
	unsigned char	res60[0x1fc];
	unsigned int	mux_stat_cpu;
	unsigned char	res61[0xfc];
	unsigned int	div_cpu0;
	unsigned int	div_cpu1;
	unsigned char	res62[0xf8];
	unsigned int	div_stat_cpu0;
	unsigned int	div_stat_cpu1;
	unsigned char	res63[0x1f8];
	unsigned int	gate_sclk_cpu;
	unsigned char	res64[0xfc];
	unsigned int	gate_ip_cpu;
	unsigned char	res65[0xfc];
	unsigned int	clkout_cmu_cpu;
	unsigned int	clkout_cmu_cpu_div_stat;
	unsigned char	res66[0x5f8];
	unsigned int	armclk_stopctrl;		/* 0x1004_5000 */
	unsigned int	atclk_stopctrl;
	unsigned int	arm_ema_ctrl;
	unsigned int	arm_ema_status;
	unsigned int	parityfail_status;
	unsigned int	parityfail_clear;
	unsigned char	res67[0x8];
	unsigned int	pwr_ctrl;
	unsigned int	pwr_ctrl2;
	unsigned char	res68[0xd8];
	unsigned int	apll_con0_l8;
	unsigned int	apll_con0_l7;
	unsigned int	apll_con0_l6;
	unsigned int	apll_con0_l5;
	unsigned int	apll_con0_l4;
	unsigned int	apll_con0_l3;
	unsigned int	apll_con0_l2;
	unsigned int	apll_con0_l1;
	unsigned int	iem_control;
	unsigned char	res69[0xdc];
	unsigned int	apll_con1_l8;
	unsigned int	apll_con1_l7;
	unsigned int	apll_con1_l6;
	unsigned int	apll_con1_l5;
	unsigned int	apll_con1_l4;
	unsigned int	apll_con1_l3;
	unsigned int	apll_con1_l2;
	unsigned int	apll_con1_l1;
	unsigned char	res70[0xe0];
	unsigned int	div_iem_l8;
	unsigned int	div_iem_l7;
	unsigned int	div_iem_l6;
	unsigned int	div_iem_l5;
	unsigned int	div_iem_l4;
	unsigned int	div_iem_l3;
	unsigned int	div_iem_l2;
	unsigned int	div_iem_l1;
	unsigned char	res71[0xe0];
	unsigned int	l2_status;
	unsigned char	res72[0xc];
	unsigned int	cpu_status;
	unsigned char	res73[0xc];
	unsigned int	ptm_status;
	unsigned char	res74[0x2edc];
	unsigned int	div_isp0;
	unsigned int	div_isp1;
	unsigned char	res75[0xf8];
	unsigned int	div_stat_isp0;
	unsigned int	div_stat_isp1;
	unsigned char	res76[0x2f8];
	unsigned int	gate_bus_isp0;
	unsigned int	gate_bus_isp1;
	unsigned int	gate_bus_isp2;
	unsigned int	gate_bus_isp3;
	unsigned char	res77[0xf0];
	unsigned int	gate_ip_isp0;
	unsigned int	gate_ip_isp1;
	unsigned char	res78[0xf8];
	unsigned int	gate_sclk_isp0;
	unsigned char	res79[0xfc];
	unsigned int	gate_cmu_isp;
	unsigned int	gate_cmu_isp_div_stat;
	unsigned char	res80[0xf8];
	unsigned int	isp_spare0;
	unsigned int	isp_spare1;
	unsigned int	isp_spare2;
	unsigned int	isp_spare3;
	unsigned char	res81[0x4074f0];
	unsigned char	res82[0x300];                /* 0x1450_0000 */
	unsigned int	src_acp;
	unsigned int	src_mask_acp;
	unsigned char	res83[0xf8];
	unsigned int	mux_stat_acp0;
	unsigned int	mux_stat_acp1;
	unsigned char	res84[0xf8];
	unsigned int	div_acp0;
	unsigned int	div_acp1;
	unsigned char	res85[0xf8];
	unsigned int	div_stat_acp0;
	unsigned int	div_stat_acp1;
	unsigned char	res86[0xf8];
	unsigned int	gate_bus_acp0;
	unsigned int	gate_bus_acp1;
	unsigned char	res87[0xf8];
	unsigned int	gate_sclk_acp;
	unsigned char	res88[0xfc];
	unsigned int	gate_ip_acp0;
	unsigned int	gate_ip_acp1;
	unsigned char	res89[0xf8];
	unsigned int	clkout_cmu_acp;
	unsigned int	clkout_cmu_acp_div_stat;
	unsigned char	res90[0x5f8];
	unsigned int	dcgidx_map0;
	unsigned int	dcgidx_map1;
	unsigned int	dcgidx_map2;
	unsigned char	res91[0x14];
	unsigned int	dcgperf_map0;
	unsigned int	dcgperf_map1;
	unsigned char	res92[0x18];
	unsigned int	dvcidx_map;
	unsigned char	res93[0x1c];
	unsigned int	freq_cpu;
	unsigned int	freq_dpm;
	unsigned char	res94[0x18];
	unsigned int	dvsemclk_en;
	unsigned int	maxperf;
	unsigned char	res95[0xf78];
	unsigned int	acp_spare0;
	unsigned int	acp_spare1;
	unsigned int	acp_spare2;
	unsigned int	acp_spare3;
	unsigned int	acp_spare4;
	unsigned char	res96[0xdfec];
	unsigned char	res97[0x15fffd];
	unsigned char	res98[0x8];             /* 0x105c_0000 */
	unsigned int	mpll_lock;
	unsigned char	res99[0xfc];
	unsigned int	mpll_con0;
	unsigned int	mpll_con1;
	unsigned int	mpll_con2;
	unsigned char	res100[0x4];
	unsigned int	bpll_lock;
	unsigned char	res101[0xfc];
	unsigned int	bpll_con0;
	unsigned int	bpll_con1;
	unsigned int	bpll_con2;
	unsigned char	res102[0xdc];
	unsigned int	src_dmc;
	unsigned char	res103[0xfc];
	unsigned int	mux_stat_dmc;
	unsigned char	res104[0xfc];
	unsigned int	div_dyn_freq_scale;
	unsigned int	div_dmc1;
	unsigned char	res105[0xf8];
	unsigned int	div_stat_dmc0;
	unsigned char	res106[0xfc];
	unsigned int	gate_bus_dmc0;
	unsigned int	gate_bus_dmc1;
	unsigned char	res107[0xf8];
	unsigned int	gate_sclk_dmc;
	unsigned char	res108[0xfc];
	unsigned int	gate_ip_dmc0;
	unsigned int	gate_ip_dmc1;
	unsigned char	res109[0xf8];
	unsigned int	clkout_cmu_dmc;
	unsigned int	clkout_cmu_dmc_div_stat;
	unsigned char	res110[0x688];
	unsigned int	dmc_freq_ctrl;
	unsigned int	dmc_pause_ctrl;
	unsigned int	ddrphy_lock_ctrl;
	unsigned char	res111[0xf64];
	unsigned int	semaphore0;
	unsigned int	semaphore1;
	unsigned int	semaphore2;
	unsigned int	cmu_dmc_spare0;
	unsigned int	cmu_dmc_spare1;
};

#else
struct exynos4_clock {
	unsigned char	res1[0x4200];			/* 0x1003_4200 */
	unsigned int	src_leftbus;
	unsigned char	res2[0x1fc];
	unsigned int	mux_stat_leftbus;
	unsigned char	res4[0xfc];
	unsigned int	div_leftbus;
	unsigned char	res5[0xfc];
	unsigned int	div_stat_leftbus;
	unsigned char	res6[0x1fc];
	unsigned int	gate_ip_leftbus;
	unsigned char	res7[0x1fc];
	unsigned int	clkout_leftbus;			/* 0x1003_4A00 */
	unsigned int	clkout_leftbus_div_stat;
	unsigned char	res8[0x37f8];
	unsigned int	src_rightbus;			/* 0x1003_8200 */
	unsigned char	res9[0x1fc];
	unsigned int	mux_stat_rightbus;
	unsigned char	res10[0xfc];
	unsigned int	div_rightbus;
	unsigned char	res11[0xfc];
	unsigned int	div_stat_rightbus;
	unsigned char	res12[0x1fc];
	unsigned int	gate_ip_rightbus;
	unsigned char	res13[0x1fc];
	unsigned int	clkout_rightbus;		/* 0x1003_8A00 */
	unsigned int	clkout_rightbus_div_stat;
	unsigned char	res14[0x3608];
	unsigned int	epll_lock;			/* 0x1003_C010 */
	unsigned char	res15[0xc];
	unsigned int	vpll_lock;
	unsigned char	res16[0xec];
	unsigned int	epll_con0;			/* 0x1003_C110 */
	unsigned int	epll_con1;
	unsigned char	res17[0x8];
	unsigned int	vpll_con0;			/* 0x1003_C120 */
	unsigned int	vpll_con1;
	unsigned char	res18[0xe8];
	unsigned int	src_top0;			/* 0x1003_C210 */
	unsigned int	src_top1;
	unsigned char	res19[0x8];
	unsigned int	src_cam;
	unsigned int	src_tv;
	unsigned int	src_mfc;
	unsigned int	src_g3d;
	unsigned int	src_image;
	unsigned int	src_lcd0;
	unsigned int	src_lcd1;
	unsigned int	src_maudio;
	unsigned int	src_fsys;
	unsigned char	res20[0xc];
	unsigned int	src_peril0;
	unsigned int	src_peril1;
	unsigned char	res21[0xb8];
	unsigned int	src_mask_top;			/* 0x1003_C310 */
	unsigned char	res22[0xc];
	unsigned int	src_mask_cam;
	unsigned int	src_mask_tv;
	unsigned char	res23[0xc];
	unsigned int	src_mask_lcd0;
	unsigned int	src_mask_lcd1;
	unsigned int	src_mask_maudio;
	unsigned int	src_mask_fsys;
	unsigned char	res24[0xc];
	unsigned int	src_mask_peril0;
	unsigned int	src_mask_peril1;
	unsigned char	res25[0xb8];
	unsigned int	mux_stat_top;			/* 0x1003_C410 */
	unsigned char	res26[0x14];
	unsigned int	mux_stat_mfc;
	unsigned int	mux_stat_g3d;
	unsigned int	mux_stat_image;
	unsigned char	res27[0xdc];
	unsigned int	div_top;
	unsigned char	res28[0xc];
	unsigned int	div_cam;
	unsigned int	div_tv;
	unsigned int	div_mfc;
	unsigned int	div_g3d;
	unsigned int	div_image;
	unsigned int	div_lcd0;
	unsigned int	div_lcd1;
	unsigned int	div_maudio;
	unsigned int	div_fsys0;
	unsigned int	div_fsys1;
	unsigned int	div_fsys2;
	unsigned int	div_fsys3;
	unsigned int	div_peril0;
	unsigned int	div_peril1;
	unsigned int	div_peril2;
	unsigned int	div_peril3;
	unsigned int	div_peril4;
	unsigned int	div_peril5;
	unsigned char	res29[0x18];
	unsigned int	div2_ratio;
	unsigned char	res30[0x8c];
	unsigned int	div_stat_top;			/* 0x1003_C610 */
	unsigned char	res31[0xc];
	unsigned int	div_stat_cam;
	unsigned int	div_stat_tv;
	unsigned int	div_stat_mfc;
	unsigned int	div_stat_g3d;
	unsigned int	div_stat_image;
	unsigned int	div_stat_lcd0;
	unsigned int	div_stat_lcd1;
	unsigned int	div_stat_maudio;
	unsigned int	div_stat_fsys0;
	unsigned int	div_stat_fsys1;
	unsigned int	div_stat_fsys2;
	unsigned int	div_stat_fsys3;
	unsigned int	div_stat_peril0;
	unsigned int	div_stat_peril1;
	unsigned int	div_stat_peril2;
	unsigned int	div_stat_peril3;
	unsigned int	div_stat_peril4;
	unsigned int	div_stat_peril5;
	unsigned char	res32[0x18];
	unsigned int	div2_stat;
	unsigned char	res33[0x29c];
	unsigned int	gate_ip_cam;			/* 0x1003_C920 */
	unsigned int	gate_ip_tv;
	unsigned int	gate_ip_mfc;
	unsigned int	gate_ip_g3d;
	unsigned int	gate_ip_image;
	unsigned int	gate_ip_lcd0;
	unsigned int	gate_ip_lcd1;
	unsigned char	res34[0x4];
	unsigned int	gate_ip_fsys;
	unsigned char	res35[0x8];
	unsigned int	gate_ip_gps;
	unsigned int	gate_ip_peril;
	unsigned char	res36[0xc];
	unsigned int	gate_ip_perir;
	unsigned char	res37[0xc];
	unsigned int	gate_block;
	unsigned char	res38[0x8c];
	unsigned int	clkout_cmu_top;			/* 0x1003_CA00 */
	unsigned int	clkout_cmu_top_div_stat;
#ifdef CONFIG_EXYNOS4X12
	unsigned char	res39[0x3700];
	unsigned int	mpll_con0;			/* 0x1004_0108 */
	unsigned int	mpll_con1;			/* 0x1004_010C */
	unsigned char	res65[0xf0];
#else
	unsigned char	res39[0x37f8];
#endif
	unsigned int	src_dmc;			/* 0x1004_0200 */
	unsigned char	res40[0xfc];
	unsigned int	src_mask_dmc;
	unsigned char	res41[0xfc];
	unsigned int	mux_stat_dmc;
	unsigned char	res42[0xfc];
	unsigned int	div_dmc0;
	unsigned int	div_dmc1;
	unsigned char	res43[0xf8];
	unsigned int	div_stat_dmc0;
	unsigned int	div_stat_dmc1;
	unsigned char	res44[0x2f8];
	unsigned int	gate_ip_dmc;
	unsigned char	res45[0xfc];
	unsigned int	clkout_cmu_dmc;
	unsigned int	clkout_cmu_dmc_div_stat;
	unsigned char	res46[0x5f8];
	unsigned int	dcgidx_map0;			/* 0x1004_1000 */
	unsigned int	dcgidx_map1;
	unsigned int	dcgidx_map2;
	unsigned char	res47[0x14];
	unsigned int	dcgperf_map0;
	unsigned int	dcgperf_map1;
	unsigned char	res48[0x18];
	unsigned int	dvcidx_map;
	unsigned char	res49[0x1c];
	unsigned int	freq_cpu;
	unsigned int	freq_dpm;
	unsigned char	res50[0x18];
	unsigned int	dvsemclk_en;
	unsigned int	maxperf;
	unsigned char	res51[0x2f78];
	unsigned int	apll_lock;			/* 0x1004_4000 */
	unsigned char	res52[0x4];
	unsigned int	mpll_lock;
	unsigned char	res53[0xf4];
	unsigned int	apll_con0;			/* 0x1004_4100 */
	unsigned int	apll_con1;
#ifdef CONFIG_EXYNOS4X12
	unsigned char	res54[0xf8];
#else
	unsigned int	mpll_con0;
	unsigned int	mpll_con1;
	unsigned char	res54[0xf0];
#endif
	unsigned int	src_cpu;
	unsigned char	res55[0x1fc];
	unsigned int	mux_stat_cpu;
	unsigned char	res56[0xfc];
	unsigned int	div_cpu0;
	unsigned int	div_cpu1;
	unsigned char	res57[0xf8];
	unsigned int	div_stat_cpu0;
	unsigned int	div_stat_cpu1;
	unsigned char	res58[0x3f8];
	unsigned int	clkout_cmu_cpu;
	unsigned int	clkout_cmu_cpu_div_stat;
	unsigned char	res59[0x5f8];
	unsigned int	armclk_stopctrl;		/* 0x1004_5000 */
	unsigned int	atclk_stopctrl;
	unsigned char	res60[0x8];
	unsigned int	parityfail_status;
	unsigned int	parityfail_clear;
	unsigned char	res61[0xe8];
	unsigned int	apll_con0_l8;
	unsigned int	apll_con0_l7;
	unsigned int	apll_con0_l6;
	unsigned int	apll_con0_l5;
	unsigned int	apll_con0_l4;
	unsigned int	apll_con0_l3;
	unsigned int	apll_con0_l2;
	unsigned int	apll_con0_l1;
	unsigned int	iem_control;
	unsigned char	res62[0xdc];
	unsigned int	apll_con1_l8;
	unsigned int	apll_con1_l7;
	unsigned int	apll_con1_l6;
	unsigned int	apll_con1_l5;
	unsigned int	apll_con1_l4;
	unsigned int	apll_con1_l3;
	unsigned int	apll_con1_l2;
	unsigned int	apll_con1_l1;
	unsigned char	res63[0xe0];
	unsigned int	div_iem_l8;
	unsigned int	div_iem_l7;
	unsigned int	div_iem_l6;
	unsigned int	div_iem_l5;
	unsigned int	div_iem_l4;
	unsigned int	div_iem_l3;
	unsigned int	div_iem_l2;
	unsigned int	div_iem_l1;
};
#endif

struct exynos5_clock {
	unsigned int	apll_lock;
	unsigned char	res1[0xfc];
	unsigned int	apll_con0;
	unsigned int	apll_con1;
	unsigned char	res2[0xf8];
	unsigned int	src_cpu;
	unsigned char	res3[0x1fc];
	unsigned int	mux_stat_cpu;
	unsigned char	res4[0xfc];
	unsigned int	div_cpu0;
	unsigned int	div_cpu1;
	unsigned char	res5[0xf8];
	unsigned int	div_stat_cpu0;
	unsigned int	div_stat_cpu1;
	unsigned char	res6[0x1f8];
	unsigned int	gate_sclk_cpu;
	unsigned char	res7[0x1fc];
	unsigned int	clkout_cmu_cpu;
	unsigned int	clkout_cmu_cpu_div_stat;
	unsigned char	res8[0x5f8];
	unsigned int	armclk_stopctrl;
	unsigned char	res9[0xc];
	unsigned int	parityfail_status;
	unsigned int	parityfail_clear;
	unsigned char	res10[0x8];
	unsigned int	pwr_ctrl;
	unsigned int	pwr_ctr2;
	unsigned char	res11[0xd8];
	unsigned int	apll_con0_l8;
	unsigned int	apll_con0_l7;
	unsigned int	apll_con0_l6;
	unsigned int	apll_con0_l5;
	unsigned int	apll_con0_l4;
	unsigned int	apll_con0_l3;
	unsigned int	apll_con0_l2;
	unsigned int	apll_con0_l1;
	unsigned int	iem_control;
	unsigned char	res12[0xdc];
	unsigned int	apll_con1_l8;
	unsigned int	apll_con1_l7;
	unsigned int	apll_con1_l6;
	unsigned int	apll_con1_l5;
	unsigned int	apll_con1_l4;
	unsigned int	apll_con1_l3;
	unsigned int	apll_con1_l2;
	unsigned int	apll_con1_l1;
	unsigned char	res13[0xe0];
	unsigned int	div_iem_l8;
	unsigned int	div_iem_l7;
	unsigned int	div_iem_l6;
	unsigned int	div_iem_l5;
	unsigned int	div_iem_l4;
	unsigned int	div_iem_l3;
	unsigned int	div_iem_l2;
	unsigned int	div_iem_l1;
	unsigned char	res14[0x2ce0];
	unsigned int	mpll_lock;
	unsigned char	res15[0xfc];
	unsigned int	mpll_con0;
	unsigned int	mpll_con1;
	unsigned char	res16[0xf8];
	unsigned int	src_core0;
	unsigned int	src_core1;
	unsigned char	res17[0xf8];
	unsigned int	src_mask_core;
	unsigned char	res18[0x100];
	unsigned int	mux_stat_core1;
	unsigned char	res19[0xf8];
	unsigned int	div_core0;
	unsigned int	div_core1;
	unsigned int	div_sysrgt;
	unsigned char	res20[0xf4];
	unsigned int	div_stat_core0;
	unsigned int	div_stat_core1;
	unsigned int	div_stat_sysrgt;
	unsigned char	res21[0x2f4];
	unsigned int	gate_ip_core;
	unsigned int	gate_ip_sysrgt;
	unsigned char	res22[0xf8];
	unsigned int	clkout_cmu_core;
	unsigned int	clkout_cmu_core_div_stat;
	unsigned char	res23[0x5f8];
	unsigned int	dcgidx_map0;
	unsigned int	dcgidx_map1;
	unsigned int	dcgidx_map2;
	unsigned char	res24[0x14];
	unsigned int	dcgperf_map0;
	unsigned int	dcgperf_map1;
	unsigned char	res25[0x18];
	unsigned int	dvcidx_map;
	unsigned char	res26[0x1c];
	unsigned int	freq_cpu;
	unsigned int	freq_dpm;
	unsigned char	res27[0x18];
	unsigned int	dvsemclk_en;
	unsigned int	maxperf;
	unsigned char	res28[0xf78];
	unsigned int	c2c_config;
	unsigned char	res29[0x24fc];
	unsigned int	div_acp;
	unsigned char	res30[0xfc];
	unsigned int	div_stat_acp;
	unsigned char	res31[0x1fc];
	unsigned int	gate_ip_acp;
	unsigned char	res32[0xfc];
	unsigned int	div_syslft;
	unsigned char	res33[0xc];
	unsigned int	div_stat_syslft;
	unsigned char	res34[0x1c];
	unsigned int	gate_ip_syslft;
	unsigned char	res35[0xcc];
	unsigned int	clkout_cmu_acp;
	unsigned int	clkout_cmu_acp_div_stat;
	unsigned char	res36[0x8];
	unsigned int	ufmc_config;
	unsigned char	res37[0x38ec];
	unsigned int	div_isp0;
	unsigned int	div_isp1;
	unsigned int	div_isp2;
	unsigned char	res38[0xf4];
	unsigned int	div_stat_isp0;
	unsigned int	div_stat_isp1;
	unsigned int	div_stat_isp2;
	unsigned char	res39[0x3f4];
	unsigned int	gate_ip_isp0;
	unsigned int	gate_ip_isp1;
	unsigned char	res40[0xf8];
	unsigned int	gate_sclk_isp;
	unsigned char	res41[0xc];
	unsigned int	mcuisp_pwr_ctrl;
	unsigned char	res42[0xec];
	unsigned int	clkout_cmu_isp;
	unsigned int	clkout_cmu_isp_div_stat;
	unsigned char	res43[0x3618];
	unsigned int	cpll_lock;
	unsigned char	res44[0xc];
	unsigned int	epll_lock;
	unsigned char	res45[0xc];
	unsigned int	vpll_lock;
	unsigned char	res46[0xc];
	unsigned int	gpll_lock;
	unsigned char	res47[0xcc];
	unsigned int	cpll_con0;
	unsigned int	cpll_con1;
	unsigned char	res48[0x8];
	unsigned int	epll_con0;
	unsigned int	epll_con1;
	unsigned int	epll_con2;
	unsigned char	res49[0x4];
	unsigned int	vpll_con0;
	unsigned int	vpll_con1;
	unsigned int	vpll_con2;
	unsigned char	res50[0x4];
	unsigned int	gpll_con0;
	unsigned int	gpll_con1;
	unsigned char	res51[0xb8];
	unsigned int	src_top0;
	unsigned int	src_top1;
	unsigned int	src_top2;
	unsigned int	src_top3;
	unsigned int	src_gscl;
	unsigned char	res52[0x8];
	unsigned int	src_disp1_0;
	unsigned char	res53[0x10];
	unsigned int	src_mau;
	unsigned int	src_fsys;
	unsigned int	src_gen;
	unsigned char	res54[0x4];
	unsigned int	src_peric0;
	unsigned int	src_peric1;
	unsigned char	res55[0x18];
	unsigned int	sclk_src_isp;
	unsigned char	res56[0x9c];
	unsigned int	src_mask_top;
	unsigned char	res57[0xc];
	unsigned int	src_mask_gscl;
	unsigned char	res58[0x8];
	unsigned int	src_mask_disp1_0;
	unsigned char	res59[0x4];
	unsigned int	src_mask_maudio;
	unsigned char	res60[0x8];
	unsigned int	src_mask_fsys;
	unsigned int	src_mask_gen;
	unsigned char	res61[0x8];
	unsigned int	src_mask_peric0;
	unsigned int	src_mask_peric1;
	unsigned char	res62[0x18];
	unsigned int	src_mask_isp;
	unsigned char	res63[0x9c];
	unsigned int	mux_stat_top0;
	unsigned int	mux_stat_top1;
	unsigned int	mux_stat_top2;
	unsigned int	mux_stat_top3;
	unsigned char	res64[0xf0];
	unsigned int	div_top0;
	unsigned int	div_top1;
	unsigned char	res65[0x8];
	unsigned int	div_gscl;
	unsigned char	res66[0x8];
	unsigned int	div_disp1_0;
	unsigned char	res67[0xc];
	unsigned int	div_gen;
	unsigned char	res68[0x4];
	unsigned int	div_mau;
	unsigned int	div_fsys0;
	unsigned int	div_fsys1;
	unsigned int	div_fsys2;
	unsigned char	res69[0x4];
	unsigned int	div_peric0;
	unsigned int	div_peric1;
	unsigned int	div_peric2;
	unsigned int	div_peric3;
	unsigned int	div_peric4;
	unsigned int	div_peric5;
	unsigned char	res70[0x10];
	unsigned int	sclk_div_isp;
	unsigned char	res71[0xc];
	unsigned int	div2_ratio0;
	unsigned int	div2_ratio1;
	unsigned char	res72[0x8];
	unsigned int	div4_ratio;
	unsigned char	res73[0x6c];
	unsigned int	div_stat_top0;
	unsigned int	div_stat_top1;
	unsigned char	res74[0x8];
	unsigned int	div_stat_gscl;
	unsigned char	res75[0x8];
	unsigned int	div_stat_disp1_0;
	unsigned char	res76[0xc];
	unsigned int	div_stat_gen;
	unsigned char	res77[0x4];
	unsigned int	div_stat_maudio;
	unsigned int	div_stat_fsys0;
	unsigned int	div_stat_fsys1;
	unsigned int	div_stat_fsys2;
	unsigned char	res78[0x4];
	unsigned int	div_stat_peric0;
	unsigned int	div_stat_peric1;
	unsigned int	div_stat_peric2;
	unsigned int	div_stat_peric3;
	unsigned int	div_stat_peric4;
	unsigned int	div_stat_peric5;
	unsigned char	res79[0x10];
	unsigned int	sclk_div_stat_isp;
	unsigned char	res80[0xc];
	unsigned int	div2_stat0;
	unsigned int	div2_stat1;
	unsigned char	res81[0x8];
	unsigned int	div4_stat;
	unsigned char	res82[0x184];
	unsigned int	gate_top_sclk_disp1;
	unsigned int	gate_top_sclk_gen;
	unsigned char	res83[0xc];
	unsigned int	gate_top_sclk_mau;
	unsigned int	gate_top_sclk_fsys;
	unsigned char	res84[0xc];
	unsigned int	gate_top_sclk_peric;
	unsigned char	res85[0x1c];
	unsigned int	gate_top_sclk_isp;
	unsigned char	res86[0xac];
	unsigned int	gate_ip_gscl;
	unsigned char	res87[0x4];
	unsigned int	gate_ip_disp1;
	unsigned int	gate_ip_mfc;
	unsigned int	gate_ip_g3d;
	unsigned int	gate_ip_gen;
	unsigned char	res88[0xc];
	unsigned int	gate_ip_fsys;
	unsigned char	res89[0x8];
	unsigned int	gate_ip_peric;
	unsigned char	res90[0xc];
	unsigned int	gate_ip_peris;
	unsigned char	res91[0x1c];
	unsigned int	gate_block;
	unsigned char	res92[0x1c];
	unsigned int	mcuiop_pwr_ctrl;
	unsigned char	res93[0x5c];
	unsigned int	clkout_cmu_top;
	unsigned int	clkout_cmu_top_div_stat;
	unsigned char	res94[0x37f8];
	unsigned int	src_lex;
	unsigned char	res95[0x1fc];
	unsigned int	mux_stat_lex;
	unsigned char	res96[0xfc];
	unsigned int	div_lex;
	unsigned char	res97[0xfc];
	unsigned int	div_stat_lex;
	unsigned char	res98[0x1fc];
	unsigned int	gate_ip_lex;
	unsigned char	res99[0x1fc];
	unsigned int	clkout_cmu_lex;
	unsigned int	clkout_cmu_lex_div_stat;
	unsigned char	res100[0x3af8];
	unsigned int	div_r0x;
	unsigned char	res101[0xfc];
	unsigned int	div_stat_r0x;
	unsigned char	res102[0x1fc];
	unsigned int	gate_ip_r0x;
	unsigned char	res103[0x1fc];
	unsigned int	clkout_cmu_r0x;
	unsigned int	clkout_cmu_r0x_div_stat;
	unsigned char	res104[0x3af8];
	unsigned int	div_r1x;
	unsigned char	res105[0xfc];
	unsigned int	div_stat_r1x;
	unsigned char	res106[0x1fc];
	unsigned int	gate_ip_r1x;
	unsigned char	res107[0x1fc];
	unsigned int	clkout_cmu_r1x;
	unsigned int	clkout_cmu_r1x_div_stat;
	unsigned char	res108[0x3608];
	unsigned int	bpll_lock;
	unsigned char	res109[0xfc];
	unsigned int	bpll_con0;
	unsigned int	bpll_con1;
	unsigned char	res110[0xe8];
	unsigned int	src_cdrex;
	unsigned char	res111[0x1fc];
	unsigned int	mux_stat_cdrex;
	unsigned char	res112[0xfc];
	unsigned int	div_cdrex;
	unsigned char	res113[0xfc];
	unsigned int	div_stat_cdrex;
	unsigned char	res114[0x2fc];
	unsigned int	gate_ip_cdrex;
	unsigned char	res115[0x10];
	unsigned int	dmc_freq_ctrl;
	unsigned char	res116[0x4];
	unsigned int	drex2_pause;
	unsigned char	res117[0xe0];
	unsigned int	clkout_cmu_cdrex;
	unsigned int	clkout_cmu_cdrex_div_stat;
	unsigned char	res118[0x8];
	unsigned int	lpddr3phy_ctrl;
	unsigned char	res119[0xc];
	unsigned int	lpddr3phy_con3;
	unsigned int	pll_div2_sel;
	unsigned char	res120[0xf5d8];
};

struct exynos5410_clock {
	unsigned int	apll_lock;			/* 0x1001_0000 */
	unsigned char	res1[0xfc];
	unsigned int	apll_con0;			/* 0x1001_0100 */
	unsigned int	apll_con1;			/* 0x1001_0104 */
	unsigned char	res2[0xf8];
	unsigned int	src_cpu;			/* 0x1001_0200 */
	unsigned char	res3[0x2fc];
	unsigned int	div_cpu0;			/* 0x1001_0500 */
	unsigned int	div_cpu1;			/* 0x1001_0504 */
	unsigned char	res4[0x4f8];
	unsigned int	clkout_cmu_cpu;			/* 0x1001_0A00 */
	unsigned char	res5[0x35fc];
	unsigned int	mpll_lock;			/* 0x1001_4000 */
	unsigned char	res6[0xfc];
	unsigned int	mpll_con0;			/* 0x1001_4100 */
	unsigned int	mpll_con1;			/* 0x1001_4104 */
	unsigned char	res7[0xf8];
	unsigned int	src_core0;			/* 0x1001_4200 */
	unsigned int	src_core1;			/* 0x1001_4204 */
	unsigned char	res8[0x2fc];
	unsigned int	div_core1;			/* 0x1001_4504 */
	unsigned char	res9[0x4f8];
	unsigned int	clkout_cmu_core;		/* 0x1001_4A00 */
	unsigned char	res10[0xb71c];
	unsigned int	cpll_con0;			/* 0x1002_0120 */
	unsigned int	cpll_con1;			/* 0x1002_0124 */
	unsigned int	dpll_con0;			/* 0x1002_0128 */
	unsigned int	dpll_con1;			/* 0x1002_012C */
	unsigned int	epll_con0;			/* 0x1002_0130 */
	unsigned int	epll_con1;			/* 0x1002_0134 */
	unsigned int	epll_con2;			/* 0x1002_0138 */
	unsigned char	res11[0x4];
	unsigned int	vpll_con0;			/* 0x1002_0140 */
	unsigned int	vpll_con1;			/* 0x1002_0144 */
	unsigned int	vpll_con2;			/* 0x1002_0148 */
	unsigned char	res12[0xc4];
	unsigned int	src_top0;			/* 0x1002_0210 */
	unsigned int	src_top1;			/* 0x1002_0214 */
	unsigned int	src_top2;			/* 0x1002_0218 */
	unsigned int	src_top3;			/* 0x1002_021C */
	unsigned char	res13[0x20];
	unsigned int	src_mau;			/* 0x1002_0240 */
	unsigned int	src_fsys;			/* 0x1002_0244 */
	unsigned char	res14[0x8];
	unsigned int	src_peric0;			/* 0x1002_0250 */
	unsigned int	src_peric1;			/* 0x1002_0254 */
	unsigned char	res15[0x2b8];
	unsigned int	div_top0;			/* 0x1002_0510 */
	unsigned int	div_top1;			/* 0x1002_0514 */
	unsigned int	div_top2;			/* 0x1002_0518 */
	unsigned int	div_top3;			/* 0x1002_051C */
	unsigned char	res16[0x28];
	unsigned int	div_fsys0;			/* 0x1002_0548 */
	unsigned int	div_fsys1;			/* 0x1002_054C */
	unsigned int	div_fsys2;			/* 0x1002_0550 */
	unsigned int	div_fsys3;			/* 0x1002_0554 */
	unsigned int	div_peric0;			/* 0x1002_0558 */
	unsigned char	res17[0x44];
	unsigned int	div4_ratio;			/* 0x1002_05A0 */
	unsigned char	res18[0x45c];
	unsigned int	clkout_cmu_top;			/* 0x1002_0A00 */
	unsigned char	res19[0xf60c];
	unsigned int	bpll_lock;			/* 0x1003_0010 */
	unsigned char	res20[0xfc];
	unsigned int	bpll_con0;			/* 0x1003_0110 */
	unsigned int	bpll_con1;			/* 0x1003_0114 */
	unsigned char	res21[0xe8];
	unsigned int	src_cdrex;			/* 0x1003_0200 */
	unsigned char	res22[0x1fc];
	unsigned int	mux_stat_cdrex;			/* 0x1003_0400 */
	unsigned char	res23[0xfc];
	unsigned int	div_cdrex0;			/* 0x1003_0500 */
	unsigned int	div_cdrex1;			/* 0x1003_0504 */
	unsigned char	res24[0x4f8];
	unsigned int	clkout_cmu_cdrex;		/* 0x1003_0A00 */
	unsigned char	res25[0x75fc];
	unsigned int	kpll_lock;			/* 0x1003_8000 */
	unsigned char	res26[0xfc];
	unsigned int	kpll_con0;			/* 0x1003_8100 */
	unsigned int	kpll_con1;			/* 0x1003_8104 */
	unsigned char	res27[0xf8];
	unsigned int	src_kfc;			/* 0x1003_8200 */
	unsigned char	res28[0x2fc];
	unsigned int	div_kfc;			/* 0x1003_8500 */
};

struct exynos5260_clock_egl {
	unsigned int	egl_pll_lock;			/* 0x1060_0000 */
	unsigned int	egl_dpll_lock;
	unsigned char	res1[0xf8];
	unsigned int	egl_pll_con0;
	unsigned int	egl_pll_con1;
	unsigned char	res2[0x4];
	unsigned int	egl_pll_freq_det;
	unsigned int	egl_dpll_con0;
	unsigned int	egl_dpll_con1;
	unsigned char	res3[0x4];
	unsigned int	egl_dpll_freq_det;
	unsigned char	res4[0xe0];
	unsigned int	clk_mux_sel_egl;
	unsigned char	res5[0xfc];
	unsigned int	clk_mux_enable_egl;
	unsigned char	res6[0xfc];
	unsigned int	clk_mux_stat_egl;
	unsigned char	res7[0x1fc];
	unsigned int	clk_div_egl;
	unsigned int	clk_div_egl_pll_freq_det;
	unsigned char	res8[0xf8];
	unsigned int	clk_div_stat_egl;
	unsigned int	clk_div_stat_egl_pll_freq_det;
	unsigned char	res9[0xf8];
	unsigned int	clk_en_aclk_egl;
	unsigned char	res10[0xfc];
	unsigned int	clk_en_pclk_egl;
	unsigned char	res11[0xfc];
	unsigned int	clk_en_sclk_egl;
	unsigned char	res12[0xfc];
	unsigned int	clk_en_ip_egl;
	unsigned char	res13[0xfc];
	unsigned int	clkout_cmu_egl;
	unsigned int	clkout_cmu_egl_div_stat;
	unsigned char	res14[0x3f8];
	unsigned int	armclk_stopctrl;
	unsigned char	res15[0x4];
	unsigned int	eagle_ema_ctrl;
	unsigned int	eagle_ema_status;
	unsigned char	res16[0x10];
	unsigned int	pwr_ctrl;
	unsigned int	pwr_ctrl2;
	unsigned int	clkstop_ctrl;
	unsigned char	res17[0x54];
	unsigned int	intr_spread_enable;
	unsigned int	intr_spread_use_standbywfi;
	unsigned char	res18[0xf74];
	unsigned int	cmu_egl_spare0;
	unsigned int	cmu_egl_spare1;
	unsigned int	cmu_egl_spare2;
	unsigned int	cmu_egl_spare3;
	unsigned int	cmu_egl_spare4;		/* 0x1060_2010 */
};

struct exynos5260_clock_fsys {
	unsigned char	res1[0x200];
	unsigned int	clk_mux_sel_fsys0;			/* 0x122E_0200 */
	unsigned int	clk_mux_sel_fsys1;
	unsigned char	res2[0xf8];
	unsigned int	clk_mux_enable_fsys0;
	unsigned int	clk_mux_enable_fsys1;
	unsigned char	res3[0xf8];
	unsigned int	clk_mux_stat_fsys0;
	unsigned int	clk_mux_stat_fsys1;
	unsigned char	res4[0xf8];
	unsigned int	clk_mux_ignore_fsys0;
	unsigned int	clk_mux_ignore_fsys1;
	unsigned char	res5[0x2f8];
	unsigned int	clk_en_aclk_fsys;
	unsigned int	clk_en_aclk_fsys_secure_rtic;
	unsigned int	clk_en_aclk_fsys_secure_smmu_rtic;
	unsigned char	res6[0xf4];
	unsigned int	clk_en_pclk_fsys;
	unsigned char	res7[0xfc];
	unsigned int	clk_en_sclk_fsys;
	unsigned char	res8[0xfc];
	unsigned int	clk_en_ip_fsys;
	unsigned int	clk_en_ip_fsys_secure_rtic;
	unsigned int	clk_en_ip_fsys_secure_smmu_rtic;	/* 0x122E_0B08 */
};

struct exynos5260_clock_kfc {
	unsigned int	kfc_pll_lock;			/* 0x1070_0000 */
	unsigned char	res1[0xfc];
	unsigned int	kfc_pll_con0;
	unsigned int	kfc_pll_con1;
	unsigned char	res2[0x4];
	unsigned int	kfc_pll_freq_det;
	unsigned char	res3[0xf0];
	unsigned int	clk_mux_sel_kfc0;
	unsigned char	res4[0x4];
	unsigned int	clk_mux_sel_kfc2;
	unsigned char	res5[0xf4];
	unsigned int	clk_mux_enable_kfc0;
	unsigned char	res6[0x4];
	unsigned int	clk_mux_enable_kfc2;
	unsigned char	res7[0xf4];
	unsigned int	clk_mux_stat_kfc0;
	unsigned char	res8[0x4];
	unsigned int	clk_mux_stat_kfc2;
	unsigned char	res9[0x1f4];
	unsigned int	clk_div_kfc;
	unsigned int	clk_div_kfc_pll_freq_det;
	unsigned char	res10[0xf8];
	unsigned int	clk_div_stat_kfc;
	unsigned int	clk_div_stat_kfc_pll_freq_det;
	unsigned char	res11[0xf8];
	unsigned int	clk_en_aclk_kfc;
	unsigned char	res12[0xfc];
	unsigned int	clk_en_pclk_kfc;
	unsigned char	res13[0xfc];
	unsigned int	clk_en_sclk_kfc;
	unsigned char	res14[0xfc];
	unsigned int	clk_en_ip_kfc;
	unsigned char	res15[0xfc];
	unsigned int	clkout_cmu_kfc;
	unsigned int	clkout_cmu_kfc_div_stat;
	unsigned char	res16[0x3f8];
	unsigned int	armclk_stopctrl_kfc;
	unsigned char	res17[0x4];
	unsigned int	arm_ema_ctrl;
	unsigned int	arm_ema_status;
	unsigned char	res18[0x10];
	unsigned int	pwr_ctrl_kfc;
	unsigned int	pwr_ctrl2_kfc;
	unsigned int	clkstop_ctrl_kfc;
	unsigned char	res19[0x54];
	unsigned int	intr_spread_enable_kfc;
	unsigned int	intr_spread_use_standbywfi_kfc;
	unsigned int	intr_spread_blocking_duration_kfc;
	unsigned char	res20[0xf74];
	unsigned int	cmu_kfc_spare0;
	unsigned int	cmu_kfc_spare1;
	unsigned int	cmu_kfc_spare2;
	unsigned int	cmu_kfc_spare3;
	unsigned int	cmu_kfc_spare4;		/* 0x1070_2010 */
};

struct exynos5260_clock_mif {
	unsigned int	mem_pll_lock;			/* 0x10CE_0000 */
	unsigned int	bus_pll_lock;
	unsigned int	media_pll_lock;
	unsigned char	res1[0xf4];
	unsigned int	mem_pll_con0;
	unsigned int	mem_pll_con1;
	unsigned char	res2[0x4];
	unsigned int	mem_pll_freq_det;
	unsigned int	bus_pll_con0;
	unsigned int	bus_pll_con1;
	unsigned char	res3[0x4];
	unsigned int	bus_pll_freq_det;
	unsigned int	media_pll_con0;
	unsigned int	media_pll_con1;
	unsigned char	res4[0x4];
	unsigned int	media_pll_freq_det;
	unsigned char	res5[0xd0];
	unsigned int	clk_mux_sel_mif;
	unsigned char	res6[0xfc];
	unsigned int	clk_mux_enable_mif;
	unsigned char	res7[0xfc];
	unsigned int	clk_mux_stat_mif;
	unsigned char	res8[0xfc];
	unsigned int	clk_mux_ignore_mif;
	unsigned char	res9[0xfc];
	unsigned int	clk_div_mif;
	unsigned int	clk_div_mif_pll_freq_det;
	unsigned char	res10[0xf8];
	unsigned int	clk_div_stat_mif;
	unsigned int	clk_div_stat_mif_pll_freq_det;
	unsigned char	res11[0xf8];
	unsigned int	clk_en_aclk_mif;
	unsigned int	clk_en_aclk_mif_secure_drex1_tz;
	unsigned int	clk_en_aclk_mif_secure_drex0_tz;
	unsigned int	clk_en_aclk_mif_secure_intmem;
	unsigned char	res12[0xf0];
	unsigned int	clk_en_pclk_mif;
	unsigned int	clk_en_pclk_mif_secure_monocnt;
	unsigned int	clk_en_pclk_mif_secure_rtc_apbif;
	unsigned int	clk_en_pclk_mif_secure_drex1_tz;
	unsigned int	clk_en_pclk_mif_secure_drex0_tz;
	unsigned char	res13[0xec];
	unsigned int	clk_en_sclk_mif;
	unsigned char	res14[0xfc];
	unsigned int	clk_en_ip_mif;
	unsigned int	clk_en_ip_mif_secure_monocnt;
	unsigned int	clk_en_ip_mif_secure_rtc_apbif;
	unsigned int	clk_en_ip_mif_secure_drex1_tz;
	unsigned int	clk_en_ip_mif_secure_drex0_tz;
	unsigned int	clk_en_ip_mif_secure_intmem;
	unsigned char	res15[0xec];
	unsigned int	clkout_cmu_mif_div_stat;
	unsigned char	res16[0x3f8];
	unsigned int	drex_freq_ctrl;
	unsigned int	pause;
	unsigned int	ddrphy_lock_ctrl;
	unsigned char	res17[0xbaf4];
	unsigned int	clkout_cmu_mif;			/* 0x10CE_CB00 */
};

struct exynos5260_clock_peri {
	unsigned int	clk_mux_sel_peri;		/* 0x1020_0200 */
	unsigned int	clk_mux_sel_peri1;
	unsigned char	res1[0xf8];
	unsigned int	clk_mux_enable_peri;
	unsigned int	clk_mux_enable_peri1;
	unsigned char	res2[0xf8];
	unsigned int	clk_mux_stat_peri;
	unsigned int	clk_mux_stat_peri1;
	unsigned char	res3[0xf8];
	unsigned int	clk_mux_ignore_peri;
	unsigned int	clk_mux_ignore_peri1;
	unsigned char	res4[0xf8];
	unsigned int	clk_div_peri;
	unsigned char	res5[0xfc];
	unsigned int	clk_div_stat_peri;
	unsigned char	res6[0xfc];
	unsigned int	clk_en_pclk_peri0;
	unsigned int	clk_en_pclk_peri1;
	unsigned int	clk_en_pclk_peri2;
	unsigned int	clk_en_pclk_peri3;
	unsigned int	clk_en_pclk_peri_secure_chipid;
	unsigned int	clk_en_pclk_peri_secure_provkey0;
	unsigned int	clk_en_pclk_peri_secure_provkey1;
	unsigned int	clk_en_pclk_peri_secure_seckey;
	unsigned int	clk_en_pclk_peri_secure_antirbkcnt;
	unsigned int	clk_en_pclk_peri_secure_top_rtc;
	unsigned int	clk_en_pclk_peri_secure_tzpc;
	unsigned char	res7[0x1d4];
	unsigned int	clk_en_sclk_peri;
	unsigned int	clk_en_sclk_peri_secure_top_rtc;
	unsigned char	res8[0xf8];
	unsigned int	clk_en_ip_peri0;
	unsigned int	clk_en_ip_peri1;
	unsigned int	clk_en_ip_peri2;
	unsigned int	clk_en_ip_peri_secure_chipid;
	unsigned int	clk_en_ip_peri_secure_provkey0;
	unsigned int	clk_en_ip_peri_secure_provkey1;
	unsigned int	clk_en_ip_peri_secure_seckey;
	unsigned int	clk_en_ip_peri_secure_antirbkcnt;
	unsigned int	clk_en_ip_peri_secure_top_rtc;
	unsigned int	clk_en_ip_peri_secure_tzpc;	/* 0x1020_0B24 */
};

struct exynos5260_clock_top {
	unsigned int	disp_pll_lock;		/* 0x1001_0000 */
	unsigned int	aud_pll_lock;
	unsigned char	res1[0xf8];
	unsigned int	disp_pll_con0;
	unsigned int	disp_pll_con1;
	unsigned int	disp_pll_freq_det;
	unsigned char	res2[0x4];
	unsigned int	aud_pll_con0;
	unsigned int	aud_pll_con1;
	unsigned int	aud_pll_con2;
	unsigned int	aud_pll_freq_det;
	unsigned char	res3[0xe0];
	unsigned int	clk_mux_sel_top_pll0;
	unsigned int	clk_mux_sel_top_mfc;
	unsigned int	clk_mux_sel_top_g2d;
	unsigned int	clk_mux_sel_top_gscl;
	unsigned char	res4[0x4];
	unsigned int	clk_mux_sel_top_isp10;
	unsigned int	clk_mux_sel_top_isp11;
	unsigned int	clk_mux_sel_top_disp0;
	unsigned int	clk_mux_sel_top_disp1;
	unsigned int	clk_mux_sel_top_bus;
	unsigned int	clk_mux_sel_top_peri0;
	unsigned int	clk_mux_sel_top_peri1;
	unsigned int	clk_mux_sel_top_fsys;
	unsigned char	res5[0xcc];
	unsigned int	clk_mux_enable_top_pll0;
	unsigned int	clk_mux_enable_top_mfc;
	unsigned int	clk_mux_enable_top_g2d;
	unsigned int	clk_mux_enable_top_gscl;
	unsigned char	res6[0x4];
	unsigned int	clk_mux_enable_top_isp10;
	unsigned int	clk_mux_enable_top_isp11;
	unsigned int	clk_mux_enable_top_disp0;
	unsigned int	clk_mux_enable_top_disp1;
	unsigned int	clk_mux_enable_top_bus;
	unsigned int	clk_mux_enable_top_peri0;
	unsigned int	clk_mux_enable_top_peri1;
	unsigned int	clk_mux_enable_top_fsys;
	unsigned char	res7[0xcc];
	unsigned int	clk_mux_stat_top_pll0;
	unsigned int	clk_mux_stat_top_mfc;
	unsigned int	clk_mux_stat_top_g2d;
	unsigned int	clk_mux_stat_top_gscl;
	unsigned char	res8[0x4];
	unsigned int	clk_mux_stat_top_isp10;
	unsigned int	clk_mux_stat_top_isp11;
	unsigned int	clk_mux_stat_top_disp0;
	unsigned int	clk_mux_stat_top_disp1;
	unsigned int	clk_mux_stat_top_bus;
	unsigned int	clk_mux_stat_top_peri0;
	unsigned int	clk_mux_stat_top_peri1;
	unsigned int	clk_mux_stat_top_fsys;
	unsigned char	res9[0xcc];
	unsigned int	clk_mux_ignore_top_pll0;
	unsigned int	clk_mux_ignore_top_mfc;
	unsigned int	clk_mux_ignore_top_g2d;
	unsigned int	clk_mux_ignore_top_gscl;
	unsigned char	res10[0x4];
	unsigned int	clk_mux_ignore_top_isp10;
	unsigned int	clk_mux_ignore_top_isp11;
	unsigned int	clk_mux_ignore_top_disp0;
	unsigned int	clk_mux_ignore_top_disp1;
	unsigned int	clk_mux_ignore_top_bus;
	unsigned int	clk_mux_ignore_top_peri0;
	unsigned int	clk_mux_ignore_top_peri1;
	unsigned int	clk_mux_ignore_top_fsys;
	unsigned char	res11[0xcc];
	unsigned int	clk_div_top_g2d_mfc;
	unsigned int	clk_div_top_gscl_isp0;
	unsigned int	clk_div_top_isp10;
	unsigned int	clk_div_top_isp11;
	unsigned int	clk_div_top_disp;
	unsigned int	clk_div_top_bus;
	unsigned int	clk_div_top_peri0;
	unsigned int	clk_div_top_peri1;
	unsigned int	clk_div_top_peri2;
	unsigned int	clk_div_top_fsys0;
	unsigned int	clk_div_top_fsys1;
	unsigned int	clk_div_top_hpm;
	unsigned int	clk_div_top_pll_freq_det;
	unsigned char	res12[0xcc];
	unsigned int	clk_div_stat_top_g2d_mfc;
	unsigned int	clk_div_stat_top_gscl_isp0;
	unsigned int	clk_div_stat_top_isp10;
	unsigned int	clk_div_stat_top_isp11;
	unsigned int	clk_div_stat_top_disp;
	unsigned int	clk_div_stat_top_bus;
	unsigned int	clk_div_stat_top_peri0;
	unsigned int	clk_div_stat_top_peri1;
	unsigned int	clk_div_stat_top_peri2;
	unsigned int	clk_div_stat_top_fsys0;
	unsigned int	clk_div_stat_top_fsys1;
	unsigned int	clk_div_stat_top_hpm;
	unsigned int	clk_div_stat_top_pll_freq_det;
	unsigned char	res13[0xcc];
	unsigned int	clk_en_aclk_top;
	unsigned char	res14[0x1fc];
	unsigned int	clk_en_sclk_top;
	unsigned char	res15[0xfc];
	unsigned int	clk_en_ip_top;
	unsigned char	res16[0xfc];
	unsigned int	clkout_cmu_top;
	unsigned int	clkout_cmu_top_div_stat;	/* 0x1001_0C04 */
};

/* EXYNOS5430 CMU */
struct exynos5430_clock_egl {
	unsigned int	egl_pll_lock;			/* 0x1180_0000 */
	unsigned int	egl_dpll_lock;
	unsigned char	res1[0xf8];
	unsigned int	egl_pll_con0;
	unsigned int	egl_pll_con1;
	unsigned char	res2[0x4];
	unsigned int	egl_pll_freq_det;
	unsigned int	egl_dpll_con0;
	unsigned int	egl_dpll_con1;
	unsigned int	egl_dpll_con2;
	unsigned int	egl_dpll_freq_det;
	unsigned char	res3[0xe0];
	unsigned int	clk_mux_sel_egl0;
	unsigned int	clk_mux_sel_egl1;
	unsigned int	clk_mux_sel_egl2;
	unsigned char	res4[0xf4];
	unsigned int	clk_mux_enable_egl0;
	unsigned int	clk_mux_enable_egl1;
	unsigned int	clk_mux_enable_egl2;
	unsigned char	res5[0xf4];
	unsigned int	clk_mux_stat_egl0;
	unsigned int	clk_mux_stat_egl1;
	unsigned int	clk_mux_stat_egl2;
	unsigned char	res6[0x1f4];
	unsigned int	clk_div_egl0;
	unsigned int	clk_div_egl1;
	unsigned int	clk_div_egl_pll_freq_det;
	unsigned char	res7[0xf4];
	unsigned int	clk_div_stat_egl0;
	unsigned int	clk_div_stat_egl1;
	unsigned int	clk_div_stat_egl_pll_freq_det;
	unsigned char	res8[0xf4];
	unsigned int	clk_enable_aclk_egl;
	unsigned char	res9[0xfc];
	unsigned int	clk_enable_pclk_egl;
	unsigned char	res10[0xfc];
	unsigned int	clk_enable_sclk_egl;
	unsigned char	res11[0xfc];
	unsigned int	clk_enable_ip_egl0;
	unsigned int	clk_enable_ip_egl1;
	unsigned char	res12[0xf8];
	unsigned int	clkout_cmu_egl;
	unsigned char	res13[0x3fc];
	unsigned int	armclk_stopctrl;
	unsigned char	res14[0x1c];
	unsigned int	pwr_ctrl;
	unsigned int	pwr_ctrl2;
	unsigned char	res15[0x58];
	unsigned int	intr_spread_enable;
	unsigned int	intr_spread_use_standbywfi;
	unsigned int	intr_spread_blocking_duration;	/* 0x1180_1088 */
};

struct exynos5430_clock_fsys {
	unsigned char	res1[0x200];			/* 0x156E_0000 */
	unsigned int	clk_mux_sel_fsys0;
	unsigned int	clk_mux_sel_fsys1;
	unsigned int	clk_mux_sel_fsys2;
	unsigned int	clk_mux_sel_fsys3;
	unsigned int	clk_mux_sel_fsys4;
	unsigned char	res2[0xec];
	unsigned int	clk_mux_enable_fsys0;
	unsigned int	clk_mux_enable_fsys1;
	unsigned int	clk_mux_enable_fsys2;
	unsigned int	clk_mux_enable_fsys3;
	unsigned int	clk_mux_enable_fsys4;
	unsigned char	res3[0xec];
	unsigned int	clk_mux_stat_fsys0;
	unsigned int	clk_mux_stat_fsys1;
	unsigned int	clk_mux_stat_fsys2;
	unsigned int	clk_mux_stat_fsys3;
	unsigned int	clk_mux_stat_fsys4;
	unsigned char	res4[0xf4];
	unsigned int	clk_mux_ignore_fsys2;
	unsigned int	clk_mux_ignore_fsys3;
	unsigned char	res5[0x2f0];
	unsigned int	clk_enable_aclk_fsys0;
	unsigned int	clk_enable_aclk_fsys1;
	unsigned char	res6[0xf8];
	unsigned int	clk_enable_pclk_fsys;
	unsigned char	res7[0xfc];
	unsigned int	clk_enable_sclk_fsys;
	unsigned char	res8[0xfc];
	unsigned int	clk_enable_ip_fsys0;
	unsigned int	clk_enable_ip_fsys1;		/* 0x156E_0B04 */
};

struct exynos5430_clock_kfc {
	unsigned int	kfc_pll_lock;			/* 0x1190_0000 */
	unsigned char	res1[0xfc];
	unsigned int	kfc_pll_con0;
	unsigned int	kfc_pll_con1;
	unsigned char	res2[0x4];
	unsigned int	kfc_pll_freq_det;
	unsigned char	res3[0xf0];
	unsigned int	clk_mux_sel_kfc0;
	unsigned int	clk_mux_sel_kfc1;
	unsigned int	clk_mux_sel_kfc2;
	unsigned char	res5[0xf4];
	unsigned int	clk_mux_enable_kfc0;
	unsigned int	clk_mux_enable_kfc1;
	unsigned int	clk_mux_enable_kfc2;
	unsigned char	res6[0xf4];
	unsigned int	clk_mux_stat_kfc0;
	unsigned int	clk_mux_stat_kfc1;
	unsigned int	clk_mux_stat_kfc2;
	unsigned char	res7[0x1f4];
	unsigned int	clk_div_kfc0;
	unsigned int	clk_div_kfc1;
	unsigned int	clk_div_kfc_pll_freq_det;
	unsigned char	res8[0xf4];
	unsigned int	clk_div_stat_kfc0;
	unsigned int	clk_div_stat_kfc1;
	unsigned int	clk_div_stat_kfc_pll_freq_det;
	unsigned char	res9[0xf4];
	unsigned int	clk_enable_aclk_kfc;
	unsigned char	res10[0xfc];
	unsigned int	clk_enable_pclk_kfc;
	unsigned char	res11[0xfc];
	unsigned int	clk_enable_sclk_kfc;
	unsigned char	res12[0xfc];
	unsigned int	clk_enable_ip_kfc0;
	unsigned int	clk_enable_ip_kfc1;
	unsigned char	res13[0xf8];
	unsigned int	clkout_cmu_kfc;
	unsigned char	res14[0x3fc];
	unsigned int	armclk_stopctrl_kfc;
	unsigned char	res15[0x1c];
	unsigned int	pwr_ctrl;
	unsigned int	pwr_ctrl2;
	unsigned char	res16[0x58];
	unsigned int	intr_spread_enable_kfc;
	unsigned int	intr_spread_use_standbywfi_kfc;
	unsigned int	intr_spread_blocking_duration_kfc;	/* 0x1190_1088 */
};

struct exynos5430_clock_mif {
	unsigned int	mem_pll_lock;			/* 0x105B_0000 */
	unsigned int	bus_pll_lock;
	unsigned int	mfc_pll_lock;
	unsigned int	bus_dpll_lock;
	unsigned char	res1[0xf0];
	unsigned int	bus_pll_con0;
	unsigned int	bus_pll_con1;
	unsigned char	res2[0x4];
	unsigned int	bus_pll_freq_det;
	unsigned int	mem_pll_con0;
	unsigned int	mem_pll_con1;
	unsigned char	res3[0x4];
	unsigned int	mem_pll_freq_det;
	unsigned int	mfc_pll_con0;
	unsigned int	mfc_pll_con1;
	unsigned char	res4[0x4];
	unsigned int	mfc_pll_freq_det;
	unsigned int	bus_dpll_con0;
	unsigned int	bus_dpll_con1;
	unsigned int	bus_dpll_con2;
	unsigned int	bus_dpll_freq_det;
	unsigned char	res5[0xc0];
	unsigned int	clk_mux_sel_mif0;
	unsigned int	clk_mux_sel_mif1;
	unsigned int	clk_mux_sel_mif2;
	unsigned int	clk_mux_sel_mif3;
	unsigned int	clk_mux_sel_mif4;
	unsigned int	clk_mux_sel_mif5;
	unsigned char	res6[0xe8];
	unsigned int	clk_mux_enable_mif0;
	unsigned int	clk_mux_enable_mif1;
	unsigned int	clk_mux_enable_mif2;
	unsigned int	clk_mux_enable_mif3;
	unsigned int	clk_mux_enable_mif4;
	unsigned int	clk_mux_enable_mif5;
	unsigned char	res7[0xe8];
	unsigned int	clk_mux_stat_mif0;
	unsigned int	clk_mux_stat_mif1;
	unsigned int	clk_mux_stat_mif2;
	unsigned int	clk_mux_stat_mif3;
	unsigned int	clk_mux_stat_mif4;
	unsigned int	clk_mux_stat_mif5;
	unsigned char	res8[0x1e8];
	unsigned int	clk_div_mif0;
	unsigned int	clk_div_mif1;
	unsigned int	clk_div_mif2;
	unsigned int	clk_div_mif3;
	unsigned int	clk_div_mif4;
	unsigned int	clk_div_mif5;
	unsigned int	clk_div_mif_pll_freq_det;
	unsigned char	res9[0xe4];
	unsigned int	clk_div_stat_mif0;
	unsigned int	clk_div_stat_mif1;
	unsigned int	clk_div_stat_mif2;
	unsigned int	clk_div_stat_mif3;
	unsigned int	clk_div_stat_mif4;
	unsigned int	clk_div_stat_mif5;
	unsigned int	clk_div_stat_mif_pll_freq_det;
	unsigned char	res10[0xe4];
	unsigned int	clk_enable_aclk_mif0;
	unsigned int	clk_enable_aclk_mif1;
	unsigned int	clk_enable_aclk_mif2;
	unsigned int	clk_enable_aclk_mif3;
	unsigned char	res11[0xf0];
	unsigned int	clk_enable_pclk_mif;
	unsigned int	clk_enable_pclk_mif_secure_drex0_tz;
	unsigned int	clk_enable_pclk_mif_secure_drex1_tz;
	unsigned int	clk_enable_pclk_mif_secure_monotonic_cnt;
	unsigned int	clk_enable_pclk_mif_secure_rtc;
	unsigned char	res12[0xec];
	unsigned int	clk_enable_sclk_mif;
	unsigned char	res13[0xfc];
	unsigned int	clk_enable_ip_mif0;
	unsigned int	clk_enable_ip_mif1;
	unsigned int	clk_enable_ip_mif2;
	unsigned int	clk_enable_ip_mif3;
	unsigned int	clk_enable_ip_mif_secure_drex0_tz;
	unsigned int	clk_enable_ip_mif_secure_drex1_tz;
	unsigned int	clk_enable_ip_mif_secure_monotonic_cnt;
	unsigned int	clk_enable_ip_mif_secure_rtc;
	unsigned char	res14[0xe4];
	unsigned int	clkout_cmu_mif;
	unsigned char	res15[0x3f8];
	unsigned int	drex_freq_ctrl0;
	unsigned int	drex_freq_ctrl1;
	unsigned int	pause;
	unsigned int	ddrphy_lock_ctrl;	/* 0x105B_100C */
};

struct exynos5430_clock_peric {
	unsigned char	res1[0x800];			/* 0x14C8_0000 */
	unsigned int	clk_enable_aclk_peric;
	unsigned char	res2[0xfc];
	unsigned int	clk_enable_pclk_peric;
	unsigned char	res3[0xfc];
	unsigned int	clk_enable_sclk_peric;
	unsigned char	res4[0xfc];
	unsigned int	clk_enable_ip_peric0;
	unsigned int	clk_enable_ip_peric1;
};

struct exynos5430_clock_peris {
	unsigned char	res1[0x800];				/* 0x1004_0000 */
	unsigned int	clk_enable_aclk_peris;
	unsigned char	res2[0xfc];
	unsigned int	clk_enable_pclk_peris;
	unsigned int	clk_enable_pclk_peris_secure_tzpc;
	unsigned int	clk_enable_pclk_peris_secure_seckey_apbif;
	unsigned int	clk_enable_pclk_peris_secure_chipid_apbif;
	unsigned int	clk_enable_pclk_peris_secure_toprtc;
	unsigned int	clk_enable_pclk_peris_secure_custom_efuse_apbif;
	unsigned int	clk_enable_pclk_peris_secure_antirbk_cnt_apbif;
	unsigned char	res3[0xe4];
	unsigned int	clk_enable_sclk_peris;
	unsigned int	clk_enable_sclk_peris_secure_seckey;
	unsigned int	clk_enable_sclk_peris_secure_chipid;
	unsigned int	clk_enable_sclk_peris_secure_toprtc;
	unsigned int	clk_enable_sclk_peris_secure_custom_efuse_apbif;
	unsigned int	clk_enable_sclk_peris_secure_antirbk_cnt;
	unsigned char	res4[0xe8];
	unsigned int	clk_enable_ip_peris0;
	unsigned int	clk_enable_ip_peris1;
	unsigned int	clk_enable_ip_peris_secure_tzpc;
	unsigned int	clk_enable_ip_peris_secure_seckey;
	unsigned int	clk_enable_ip_peris_secure_chipid;
	unsigned int	clk_enable_ip_peris_secure_toprtc;
	unsigned int	clk_enable_ip_peris_secure_custom_efuse;
	unsigned int	clk_enable_ip_peris_secure_antirbk_cnt;	/* 0x1004_0B1C */
};

struct exynos5430_clock_top {
	unsigned int	isp_pll_lock;			/* 0x1003_0000 */
	unsigned int	aud_pll_lock;
	unsigned int	aud_dpll_lock;
	unsigned char	res1[0xf4];
	unsigned int	isp_pll_con0;
	unsigned int	isp_pll_con1;
	unsigned int	isp_pll_freq_det;
	unsigned char	res2[0x4];
	unsigned int	aud_pll_con0;
	unsigned int	aud_pll_con1;
	unsigned int	aud_pll_con2;
	unsigned int	aud_pll_freq_det;
	unsigned int	aud_dpll_con0;
	unsigned int	aud_dpll_con1;
	unsigned int	aud_dpll_con2;
	unsigned int	aud_dpll_freq_det;
	unsigned char	res3[0xd0];
	unsigned int	clk_mux_sel_top0;
	unsigned int	clk_mux_sel_top1;
	unsigned int	clk_mux_sel_top2;
	unsigned int	clk_mux_sel_top3;
	unsigned int	clk_mux_sel_top4;
	unsigned char	res4[0xc];
	unsigned int	clk_mux_sel_top_mscl;
	unsigned int	clk_mux_sel_top_cam1;
	unsigned int	clk_mux_sel_top_disp;
	unsigned char	res5[0x4];
	unsigned int	clk_mux_sel_top_fsys0;
	unsigned int	clk_mux_sel_top_fsys1;
	unsigned int	clk_mux_sel_top_peric0;
	unsigned int	clk_mux_sel_top_peric1;
	unsigned char	res6[0xc0];
	unsigned int	clk_mux_enable_top_top0;
	unsigned int	clk_mux_enable_top_top1;
	unsigned int	clk_mux_enable_top_top2;
	unsigned int	clk_mux_enable_top_top3;
	unsigned int	clk_mux_enable_top_top4;
	unsigned char	res7[0xc];
	unsigned int	clk_mux_enable_top_top_mscl;
	unsigned int	clk_mux_enable_top_top_cam1;
	unsigned int	clk_mux_enable_top_top_disp;
	unsigned char	res8[0x4];
	unsigned int	clk_mux_enable_top_fsys0;
	unsigned int	clk_mux_enable_top_fsys1;
	unsigned int	clk_mux_enable_top_peric0;
	unsigned int	clk_mux_enable_top_peric1;
	unsigned char	res9[0xc0];
	unsigned int	clk_mux_stat_top_top0;
	unsigned int	clk_mux_stat_top_top1;
	unsigned int	clk_mux_stat_top_top2;
	unsigned int	clk_mux_stat_top_top3;
	unsigned int	clk_mux_stat_top_top4;
	unsigned char	res10[0xc];
	unsigned int	clk_mux_stat_top_top_mscl;
	unsigned int	clk_mux_stat_top_top_cam1;
	unsigned char	res11[0x8];
	unsigned int	clk_mux_stat_top_fsys0;
	unsigned int	clk_mux_stat_top_fsys1;
	unsigned int	clk_mux_stat_top_peric0;
	unsigned int	clk_mux_stat_top_peric1;
	unsigned char	res12[0x1c0];
	unsigned int	clk_div_top0;
	unsigned int	clk_div_top1;
	unsigned int	clk_div_top2;
	unsigned int	clk_div_top3;
	unsigned int	clk_div_top4;
	unsigned char	res13[0x4];
	unsigned int	clk_div_top_mscl;
	unsigned int	clk_div_top_cam10;
	unsigned int	clk_div_top_cam11;
	unsigned char	res14[0x8];
	unsigned int	clk_div_top_fsys0;
	unsigned int	clk_div_top_fsys1;
	unsigned int	clk_div_top_fsys2;
	unsigned int	clk_div_top_peric0;
	unsigned int	clk_div_top_peric1;
	unsigned int	clk_div_top_peric2;
	unsigned int	clk_div_top_peric3;
	unsigned char	res15[0x4];
	unsigned int	clk_div_top_pll_freq_det;
	unsigned char	res16[0xb0];
	unsigned int	clk_div_stat_top0;
	unsigned int	clk_div_stat_top1;
	unsigned int	clk_div_stat_top2;
	unsigned int	clk_div_stat_top3;
	unsigned int	clk_div_stat_top4;
	unsigned char	res17[0x4];
	unsigned int	clk_div_stat_top_mscl;
	unsigned int	clk_div_stat_top_cam10;
	unsigned int	clk_div_stat_top_cam11;
	unsigned int	clk_div_stat_top_fsys0;
	unsigned int	clk_div_stat_top_fsys1;
	unsigned int	clk_div_stat_top_fsys2;
	unsigned int	clk_div_stat_top_peric0;
	unsigned int	clk_div_stat_top_peric1;
	unsigned int	clk_div_stat_top_peric2;
	unsigned int	clk_div_stat_top_peric3;
	unsigned char	res19[0x8];
	unsigned int	clk_div_stat_top_pll_freq_det;
	unsigned char	res20[0xb0];
	unsigned int	clk_enable_aclk_top;
	unsigned char	res21[0x1fc];
	unsigned int	clk_enable_sclk_top;
	unsigned int	clk_enable_sclk_top_mscl;
	unsigned int	clk_enable_sclk_top_cam1;
	unsigned int	clk_enable_sclk_top_disp;
	unsigned int	clk_enable_sclk_top_fsys;
	unsigned int	clk_enable_sclk_top_peric;
	unsigned char	res22[0xe8];
	unsigned int	clk_enable_ip_top;
	unsigned char	res23[0xfc];
	unsigned int	clkout_cmu_top;			/* 0x1003_0C00 */
};

struct exynos5430_clock_bus1 {
	unsigned char	res1[0x200];			/* 0x1480_0000 */
	unsigned int	clk_mux_sel_bus1;
	unsigned int	clk_mux_enable_bus1;
	unsigned int	clk_mux_stat_bus1;
	unsigned char	res2[0x3f4];
	unsigned int	clk_div_bus1;
	unsigned char	res3[0xfc];
	unsigned int	clk_div_stat_bus1;
	unsigned char	res4[0xfc];
	unsigned int	clk_enable_aclk_bus1;
	unsigned char	res5[0xfc];
	unsigned int	clk_enable_pclk_bus1;
	unsigned char	res6[0x1fc];
	unsigned int	clk_enable_ip_bus10;
	unsigned int	clk_enable_ip_bus11;		/* 0x1480_0B04 */
};

struct exynos5430_clock_bus2 {
	unsigned char	res1[0x200];			/* 0x1340_0000 */
	unsigned int	clk_mux_sel_bus2;
	unsigned int	clk_mux_enable_bus2;
	unsigned int	clk_mux_stat_bus2;
	unsigned char	res2[0x3f4];
	unsigned int	clk_div_bus2;
	unsigned char	res3[0xfc];
	unsigned int	clk_div_stat_bus2;
	unsigned char	res4[0xfc];
	unsigned int	clk_enable_aclk_bus2;
	unsigned char	res5[0xfc];
	unsigned int	clk_enable_pclk_bus2;
	unsigned char	res6[0x1fc];
	unsigned int	clk_enable_ip_bus20;
	unsigned int	clk_enable_ip_bus21;		/* 0x1340_0B04 */
};

struct exynos5430_clock_imem {
	unsigned char	res1[0x800];				/* 0x1106_0000 */
	unsigned int	clk_enable_aclk_imem;
	unsigned int	clk_enable_aclk_imem_secure_int_mem;
	unsigned int	clk_enable_aclk_imem_secure_sss;
	unsigned int	clk_enable_aclk_imem_secure_slimsss;
	unsigned int	clk_enable_aclk_imem_secure_rtic;
	unsigned int	clk_enable_aclk_imem_secure_smmu_sss;
	unsigned int	clk_enable_aclk_imem_secure_smmu_slimsss;
	unsigned int	clk_enable_aclk_imem_secure_smmu_rtic;
	unsigned char	res2[0xe0];
	unsigned int	clk_enable_pclk_imem;
	unsigned int	clk_enable_pclk_imem_secure_sss;
	unsigned int	clk_enable_pclk_imem_secure_slimsss;
	unsigned int	clk_enable_pclk_imem_secure_rtic;
	unsigned int	clk_enable_pclk_imem_secure_smmu_sss;
	unsigned int	clk_enable_pclk_imem_secure_smmu_slimsss;
	unsigned int	clk_enable_pclk_imem_secure_smmu_rtic;
	unsigned char	res3[0x1e4];
	unsigned int	clk_enable_ip_imem0;
	unsigned int	clk_enable_ip_imem1;
	unsigned int	clk_enable_ip_imem_secure_int_mem;
	unsigned int	clk_enable_ip_imem_secure_sss;
	unsigned int	clk_enable_ip_imem_secure_slimsss;
	unsigned int	clk_enable_ip_imem_secure_rtic;
	unsigned int	clk_enable_ip_imem_secure_smmu_sss;
	unsigned int	clk_enable_ip_imem_secure_smmu_slimsss;
	unsigned int	clk_enable_ip_imem_secure_smmu_rtic;	/* 0x1106_0B18 */
};

struct exynos5430_clock_cpif {
	unsigned int	mphy_pll_lock;				/* 0x13B9_0000 */
	unsigned char	res1[0xfc];
	unsigned int	mphy_pll_con0;
	unsigned int	mphy_pll_con1;
	unsigned char	res2[0x4];
	unsigned int	mphy_pll_freq_det;
	unsigned char	res3[0xf0];
	unsigned int	clk_mux_sel_cpif0;
	unsigned int	clk_mux_sel_cpif1;
	unsigned int	clk_mux_sel_cpif2;
	unsigned char	res4[0xf4];
	unsigned int	clk_mux_enable_cpif0;
	unsigned int	clk_mux_enable_cpif1;
	unsigned int	clk_mux_enable_cpif2;
	unsigned char	res5[0xf4];
	unsigned int	clk_mux_stat_cpif0;
	unsigned int	clk_mux_stat_cpif1;
	unsigned int	clk_mux_stat_cpif2;
	unsigned char	res6[0xf8];
	unsigned int	clk_mux_ignore_cpif1;
	unsigned char	res7[0xf8];
	unsigned int	clk_div_cpif;
	unsigned char	res8[0x4];
	unsigned int	clk_div_cpif_pll_freq_det;
	unsigned char	res9[0xf4];
	unsigned int	clk_div_stat_cpif;
	unsigned char	res10[0x4];
	unsigned int	clk_div_stat_cpif_pll_freq_det;
	unsigned char	res11[0xf4];
	unsigned int	clk_enable_aclk_cpif;
	unsigned char	res12[0xfc];
	unsigned int	clk_enable_pclk_cpif;
	unsigned char	res13[0xfc];
	unsigned int	clk_enable_sclk_cpif;
	unsigned char	res14[0xfc];
	unsigned int	clk_enable_ip_cpif0;
	unsigned int	clk_enable_ip_cpif1;
	unsigned char	res15[0xf8];
	unsigned int	clkout_cmu_cpif;			/* 0x13B9_0C00 */
	unsigned int	clkout_div_stat_cmu_cpif;
};

/* EXYNOS7420 CMU */
struct exynos7420_clock_topc {
	unsigned int	cci_pll_lock;			/* 0x1057_0000 */
	unsigned int	bus0_pll_lock;
	unsigned int	bus1_pll_lock;
	unsigned int	mfc_pll_lock;
	unsigned int	aud_pll_lock;
	unsigned int	mif_pll_lock;
	unsigned char	res1[0xe8];
	unsigned int	cci_pll_con0;			/* 0x1057_0100 */
	unsigned int	cci_pll_con1;
	unsigned char	res2[0x8];
	unsigned int	bus0_pll_con0;			/* 0x1057_0110 */
	unsigned int	bus0_pll_con1;
	unsigned char	res3[0x8];
	unsigned int	bus1_pll_con0;			/* 0x1057_0120 */
	unsigned int	bus1_pll_con1;
	unsigned char	res4[0x8];
	unsigned int	mfc_pll_con0;			/* 0x1057_0130 */
	unsigned int	mfc_pll_con1;
	unsigned char	res5[0x8];
	unsigned int	aud_pll_con0;			/* 0x1057_0140 */
	unsigned int	aud_pll_con1;
	unsigned int	aud_pll_con2;
	unsigned char	res6[0x4];
	unsigned int	mif_pll_con0;			/* 0x1057_0150 */
	unsigned int	mif_pll_con1;
	unsigned int	mif_pll_con2;
	unsigned char	res7[0xa4];
	unsigned int	clk_mux_sel_topc0;		/* 0x1057_0200 */
	unsigned int	clk_mux_sel_topc1;
	unsigned int	clk_mux_sel_topc2;
	unsigned int	clk_mux_sel_topc3;
	unsigned int	clk_mux_sel_topc4;
	unsigned int	clk_mux_sel_topc5;
	unsigned char	res8[0xe8];
	unsigned int	clk_mux_enable_topc0;		/* 0x1057_0300 */
	unsigned int	clk_mux_enable_topc1;
	unsigned int	clk_mux_enable_topc2;
	unsigned int	clk_mux_enable_topc3;
	unsigned int	clk_mux_enable_topc4;
	unsigned int	clk_mux_enable_topc5;
	unsigned char	res9[0xe8];
	unsigned int	clk_mux_stat_topc0;		/* 0x1057_0400 */
	unsigned int	clk_mux_stat_topc1;
	unsigned int	clk_mux_stat_topc2;
	unsigned int	clk_mux_stat_topc3;
	unsigned int	clk_mux_stat_topc4;
	unsigned int	clk_mux_stat_topc5;
	unsigned char	res10[0x1e8];
	unsigned int	clk_div_topc0;			/* 0x1057_0600 */
	unsigned int	clk_div_topc1;
	unsigned int	clk_div_topc2;
	unsigned int	clk_div_topc3;
	unsigned char	res11[0xf0];
	unsigned int	clk_div_stat_topc0;		/* 0x1057_0700 */
	unsigned int	clk_div_stat_topc1;
	unsigned int	clk_div_stat_topc2;
	unsigned int	clk_div_stat_topc3;
	unsigned char	res12[0xf0];
	unsigned int	clk_enable_aclk_topc0;		/* 0x1057_0800 */
	unsigned int	clk_enable_aclk_topc1;
	unsigned int	clk_enable_aclk_topc2;
	unsigned char	res13[0x1f4];
	unsigned int	clk_enable_sclk_topc0;		/* 0x1057_0A00 */
	unsigned int	clk_enable_sclk_topc1;
	unsigned char	res14[0xf8];
	unsigned int	clk_enable_ip_topc;		/* 0x1057_0B00 */
	unsigned char	res15[0xfc];
	unsigned int	clkout_cmu_topc;		/* 0x1057_0C00 */
	unsigned int	clkout_all_blk_cmu;
	unsigned char	res16[0x8];
	unsigned int	clkout_cmu_topc_div_stat;	/* 0x1057_0C10 */
	unsigned int	clkout_all_blk_cmu_div_stat;
	unsigned char	res17[0xe8];
	unsigned int	cmu_topc_low_power_mode;	/* 0x1057_0D00 */
	unsigned int	cmu_topc_lp4_phy_div_sel;
	unsigned int	cmu_topc_rootclken_on;
	unsigned char	res18[0x2f8];
	unsigned int	drex_freq_ctrl1;		/* 0x1057_1004 */
	unsigned int	pause;
	unsigned char	res19[0xf4];
	unsigned int	dvfs_ctrl_vdd_int;		/* 0x1057_1100 */
	unsigned char	res20[0xc];
	unsigned int	dvfs_ctrl_vdd_disp_cam0;	/* 0x1057_1110 */
	unsigned char	res21[0xf4];
	unsigned int	l0_clk_mux_sel_topc2;		/* 0x1057_1208 */
	unsigned int	l0_clk_mux_sel_topc3;
	unsigned char	res22[0x3f0];
	unsigned int	l0_clk_div_topc0;		/* 0x1057_1600 */
	unsigned int	l0_clk_div_topc1;
	unsigned char	res23[0xc00];
	unsigned int	l1_clk_mux_sel_topc2;		/* 0x1057_2208 */
	unsigned int	l1_clk_mux_sel_topc3;
	unsigned char	res24[0x3f0];
	unsigned int	l1_clk_div_topc0;		/* 0x1057_2600 */
	unsigned int	l1_clk_div_topc1;
	unsigned char	res25[0xc00];
	unsigned int	l2_clk_mux_sel_topc2;		/* 0x1057_3208 */
	unsigned int	l2_clk_mux_sel_topc3;
	unsigned char	res26[0x3f0];
	unsigned int	l2_clk_div_topc0;		/* 0x1057_3600 */
	unsigned int	l2_clk_div_topc1;
	unsigned char	res27[0xc00];
	unsigned int	l3_clk_mux_sel_topc2;		/* 0x1057_4208 */
	unsigned int	l3_clk_mux_sel_topc3;
	unsigned char	res28[0x3f0];
	unsigned int	l3_clk_div_topc0;		/* 0x1057_4600 */
	unsigned int	l3_clk_div_topc1;
	unsigned char	res29[0xc00];
	unsigned int	l4_clk_mux_sel_topc2;		/* 0x1057_5208 */
	unsigned int	l4_clk_mux_sel_topc3;
	unsigned char	res30[0x3f0];
	unsigned int	l4_clk_div_topc0;		/* 0x1057_5600 */
	unsigned int	l4_clk_div_topc1;
	unsigned char	res31[0xc00];
	unsigned int	l5_clk_mux_sel_topc2;		/* 0x1057_6208 */
	unsigned int	l5_clk_mux_sel_topc3;
	unsigned char	res32[0x3f0];
	unsigned int	l5_clk_div_topc0;		/* 0x1057_6600 */
	unsigned int	l5_clk_div_topc1;
	unsigned char	res33[0xc00];
	unsigned int	l6_clk_mux_sel_topc2;		/* 0x1057_7208 */
	unsigned int	l6_clk_mux_sel_topc3;
	unsigned char	res34[0x3f0];
	unsigned int	l6_clk_div_topc0;		/* 0x1057_7600 */
	unsigned int	l6_clk_div_topc1;
	unsigned char	res35[0xc00];
	unsigned int	l7_clk_mux_sel_topc2;		/* 0x1057_8208 */
	unsigned int	l7_clk_mux_sel_topc3;
	unsigned char	res36[0x3f0];
	unsigned int	l7_clk_div_topc0;		/* 0x1057_8600 */
	unsigned int	l7_clk_div_topc1;
	unsigned char	res37[0xc00];
	unsigned int	l8_clk_mux_sel_topc2;		/* 0x1057_9208 */
	unsigned int	l8_clk_mux_sel_topc3;
	unsigned char	res38[0x3f0];
	unsigned int	l8_clk_div_topc0;		/* 0x1057_9600 */
	unsigned int	l8_clk_div_topc1;
	unsigned char	res39[0xc00];
	unsigned int	l9_clk_mux_sel_topc2;		/* 0x1057_A208 */
	unsigned int	l9_clk_mux_sel_topc3;
	unsigned char	res40[0x3f0];
	unsigned int	l9_clk_div_topc0;		/* 0x1057_A600 */
	unsigned int	l9_clk_div_topc1;
};

struct exynos7420_clock_top0 {
	unsigned char	res1[0x4];
	unsigned int	isp_pll_lock;			/* 0x105D_0004 */
	unsigned int	cam_pll_lock;
	unsigned char	res2[0x104];
	unsigned int	isp_pll_con0;			/* 0x105D_0110 */
	unsigned int	isp_pll_con1;
	unsigned char	res3[0x8];
	unsigned int	cam_pll_con0;			/* 0x105D_0120 */
	unsigned int	cam_pll_con1;
	unsigned char	res4[0xd8];
	unsigned int	clk_mux_sel_top00;		/* 0x105D_0200 */
	unsigned int	clk_mux_sel_top01;
	unsigned char	res5[0x4];
	unsigned int	clk_mux_sel_top03;		/* 0x105D_020C */
	unsigned int	clk_mux_sel_top04;
	unsigned int	clk_mux_sel_top05;
	unsigned int	clk_mux_sel_top06;
	unsigned int	clk_mux_sel_top07;
	unsigned int	clk_mux_sel_top0_disp;
	unsigned int	clk_mux_sel_top0_cam10;
	unsigned int	clk_mux_sel_top0_cam11;
	unsigned char	res7[0x4];
	unsigned int	clk_mux_sel_top0_peric0;	/* 0x105D_0230 */
	unsigned int	clk_mux_sel_top0_peric1;
	unsigned int	clk_mux_sel_top0_peric2;
	unsigned int	clk_mux_sel_top0_peric3;
	unsigned int	clk_mux_sel_top0_peric4;
	unsigned char	res8[0xbc];
	unsigned int	clk_mux_enable_top00;		/* 0x105D_0300 */
	unsigned int	clk_mux_enable_top01;
	unsigned char	res9[0x4];
	unsigned int	clk_mux_enable_top03;		/* 0x105D_030C */
	unsigned int	clk_mux_enable_top04;
	unsigned int	clk_mux_enable_top05;
	unsigned int	clk_mux_enable_top06;
	unsigned int	clk_mux_enable_top07;
	unsigned int	clk_mux_enable_top0_disp;
	unsigned int	clk_mux_enable_top0_cam10;
	unsigned int	clk_mux_enable_top0_cam11;
	unsigned char	res10[0x4];
	unsigned int	clk_mux_enable_top0_peric0;	/* 0x105D_0330 */
	unsigned int	clk_mux_enable_top0_peric1;
	unsigned int	clk_mux_enable_top0_peric2;
	unsigned int	clk_mux_enable_top0_peric3;
	unsigned int	clk_mux_enable_top0_peric4;
	unsigned char	res11[0xbc];
	unsigned int	clk_mux_stat_top00;		/* 0x105D_0400 */
	unsigned int	clk_mux_stat_top01;
	unsigned char	res12[0x4];
	unsigned int	clk_mux_stat_top03;		/* 0x105D_040C */
	unsigned int	clk_mux_stat_top04;
	unsigned int	clk_mux_stat_top05;
	unsigned int	clk_mux_stat_top06;
	unsigned int	clk_mux_stat_top07;
	unsigned int	clk_mux_stat_top0_disp;
	unsigned int	clk_mux_stat_top0_cam10;
	unsigned int	clk_mux_stat_top0_cam11;
	unsigned char	res13[0x8];
	unsigned int	clk_mux_stat_top0_peric1;	/* 0x105D_0434 */
	unsigned int	clk_mux_stat_top0_peric2;
	unsigned int	clk_mux_stat_top0_peric3;
	unsigned int	clk_mux_stat_top0_peric4;
	unsigned char	res14[0x1c8];
	unsigned int	clk_div_top03;			/* 0x105D_060C */
	unsigned int	clk_div_top04;
	unsigned int	clk_div_top05;
	unsigned int	clk_div_top06;
	unsigned int	clk_div_top07;
	unsigned int	clk_div_top0_disp;
	unsigned int	clk_div_top0_cam10;
	unsigned int	clk_div_top0_cam11;
	unsigned char	res15[0x4];
	unsigned int	clk_div_top0_peric0;		/* 0x105D_0630 */
	unsigned int	clk_div_top0_peric1;
	unsigned int	clk_div_top0_peric2;
	unsigned int	clk_div_top0_peric3;
	unsigned int	clk_div_top0_peric4;
	unsigned char	res16[0xc8];
	unsigned int	clk_div_stat_top03;		/* 0x105D_070C */
	unsigned int	clk_div_stat_top04;
	unsigned int	clk_div_stat_top05;
	unsigned int	clk_div_stat_top06;
	unsigned int	clk_div_stat_top07;
	unsigned int	clk_div_stat_top0_disp;
	unsigned int	clk_div_stat_top0_cam10;
	unsigned int	clk_div_stat_top0_cam11;
	unsigned char	res17[0x4];
	unsigned int	clk_div_stat_top0_peric0;	/* 0x105D_0730 */
	unsigned int	clk_div_stat_top0_peric1;
	unsigned int	clk_div_stat_top0_peric2;
	unsigned int	clk_div_stat_top0_peric3;
	unsigned int	clk_div_stat_top0_peric4;
	unsigned char	res18[0xc4];
	unsigned int	clk_enable_aclk_top02;		/* 0x105D_0808 */
	unsigned int	clk_enable_aclk_top03;
	unsigned int	clk_enable_aclk_top04;
	unsigned int	clk_enable_aclk_top05;
	unsigned int	clk_enable_aclk_top06;
	unsigned int	clk_enable_aclk_top07;
	unsigned char	res19[0x200];
	unsigned int	clk_enable_sclk_top0_disp;	/* 0x105D_0A20 */
	unsigned int	clk_enable_sclk_top0_cam10;
	unsigned int	clk_enable_sclk_top0_cam11;
	unsigned char	res191[0x4];
	unsigned int	clk_enable_sclk_top0_peric0;
	unsigned int	clk_enable_sclk_top0_peric1;
	unsigned int	clk_enable_sclk_top0_peric2;
	unsigned int	clk_enable_sclk_top0_peric3;
	unsigned int	clk_enable_sclk_top0_peric4;
	unsigned char	res20[0xbc];
	unsigned int	clk_enable_ip_top0;		/* 0x105D_0B00 */
	unsigned char	res21[0xfc];
	unsigned int	clkout_cmu_top0;		/* 0x105D_0C00 */
	unsigned int	clkout_cmu_top0_div_stat;
	unsigned char	res22[0xf8];
	unsigned int	mode_clock_enable;		/* 0x105D_0D00 */
	unsigned int	cmu_top0_rootclken_on0;
	unsigned int	cmu_top0_rootclken_on1;
	unsigned int	spdif_bypass;
	unsigned char	res23[0xf0];
	unsigned int	cmu_top0_dvfs_ctrl_stat;	/* 0x105D_0E00 */
	unsigned char	res24[0x408];
	unsigned int	l0_clk_mux_sel_top03;		/* 0x105D_120C */
	unsigned int	l0_clk_mux_sel_top04;
	unsigned int	l0_clk_mux_sel_top05;
	unsigned int	l0_clk_mux_sel_top06;
	unsigned int	l0_clk_mux_sel_top07;
	unsigned char	res25[0x3ec];
	unsigned int	l0_clk_div_top03;		/* 0x105D_160C */
	unsigned int	l0_clk_div_top04;
	unsigned int	l0_clk_div_top05;
	unsigned int	l0_clk_div_top06;
	unsigned int	l0_clk_div_top07;
	unsigned char	res26[0xbec];
	unsigned int	l1_clk_mux_sel_top03;		/* 0x105D_220C */
	unsigned int	l1_clk_mux_sel_top04;
	unsigned int	l1_clk_mux_sel_top05;
	unsigned int	l1_clk_mux_sel_top06;
	unsigned int	l1_clk_mux_sel_top07;
	unsigned char	res27[0x3ec];
	unsigned int	l1_clk_div_top03;		/* 0x105D_260C */
	unsigned int	l1_clk_div_top04;
	unsigned int	l1_clk_div_top05;
	unsigned int	l1_clk_div_top06;
	unsigned int	l1_clk_div_top07;
	unsigned char	res28[0xbec];
	unsigned int	l2_clk_mux_sel_top03;		/* 0x105D_320C */
	unsigned int	l2_clk_mux_sel_top04;
	unsigned int	l2_clk_mux_sel_top05;
	unsigned int	l2_clk_mux_sel_top06;
	unsigned int	l2_clk_mux_sel_top07;
	unsigned char	res29[0x3ec];
	unsigned int	l2_clk_div_top03;		/* 0x105D_360C */
	unsigned int	l2_clk_div_top04;
	unsigned int	l2_clk_div_top05;
	unsigned int	l2_clk_div_top06;
	unsigned int	l2_clk_div_top07;
	unsigned char	res30[0xbec];
	unsigned int	l3_clk_mux_sel_top03;		/* 0x105D_420C */
	unsigned int	l3_clk_mux_sel_top04;
	unsigned int	l3_clk_mux_sel_top05;
	unsigned int	l3_clk_mux_sel_top06;
	unsigned int	l3_clk_mux_sel_top07;
	unsigned char	res31[0x3ec];
	unsigned int	l3_clk_div_top03;		/* 0x105D_460C */
	unsigned int	l3_clk_div_top04;
	unsigned int	l3_clk_div_top05;
	unsigned int	l3_clk_div_top06;
	unsigned int	l3_clk_div_top07;
	unsigned char	res32[0xbec];
	unsigned int	l4_clk_mux_sel_top03;		/* 0x105D_520C */
	unsigned int	l4_clk_mux_sel_top04;
	unsigned int	l4_clk_mux_sel_top05;
	unsigned int	l4_clk_mux_sel_top06;
	unsigned int	l4_clk_mux_sel_top07;
	unsigned char	res33[0x3ec];
	unsigned int	l4_clk_div_top03;		/* 0x105D_560C */
	unsigned int	l4_clk_div_top04;
	unsigned int	l4_clk_div_top05;
	unsigned int	l4_clk_div_top06;
	unsigned int	l4_clk_div_top07;
	unsigned char	res34[0xbec];
	unsigned int	l5_clk_mux_sel_top03;		/* 0x105D_620C */
	unsigned int	l5_clk_mux_sel_top04;
	unsigned int	l5_clk_mux_sel_top05;
	unsigned int	l5_clk_mux_sel_top06;
	unsigned int	l5_clk_mux_sel_top07;
	unsigned char	res35[0x3ec];
	unsigned int	l5_clk_div_top03;		/* 0x105D_660C */
	unsigned int	l5_clk_div_top04;
	unsigned int	l5_clk_div_top05;
	unsigned int	l5_clk_div_top06;
	unsigned int	l5_clk_div_top07;
	unsigned char	res36[0xbec];
	unsigned int	l6_clk_mux_sel_top03;		/* 0x105D_720C */
	unsigned int	l6_clk_mux_sel_top04;
	unsigned int	l6_clk_mux_sel_top05;
	unsigned int	l6_clk_mux_sel_top06;
	unsigned int	l6_clk_mux_sel_top07;
	unsigned char	res37[0x3ec];
	unsigned int	l6_clk_div_top03;		/* 0x105D_760C */
	unsigned int	l6_clk_div_top04;
	unsigned int	l6_clk_div_top05;
	unsigned int	l6_clk_div_top06;
	unsigned int	l6_clk_div_top07;
	unsigned char	res38[0xbec];
	unsigned int	l7_clk_mux_sel_top03;		/* 0x105D_820C */
	unsigned int	l7_clk_mux_sel_top04;
	unsigned int	l7_clk_mux_sel_top05;
	unsigned int	l7_clk_mux_sel_top06;
	unsigned int	l7_clk_mux_sel_top07;
	unsigned char	res39[0x3ec];
	unsigned int	l7_clk_div_top03;		/* 0x105D_860C */
	unsigned int	l7_clk_div_top04;
	unsigned int	l7_clk_div_top05;
	unsigned int	l7_clk_div_top06;
	unsigned int	l7_clk_div_top07;
	unsigned char	res40[0xbec];
	unsigned int	l8_clk_mux_sel_top03;		/* 0x105D_920C */
	unsigned int	l8_clk_mux_sel_top04;
	unsigned int	l8_clk_mux_sel_top05;
	unsigned int	l8_clk_mux_sel_top06;
	unsigned int	l8_clk_mux_sel_top07;
	unsigned char	res41[0x3ec];
	unsigned int	l8_clk_div_top03;		/* 0x105D_960C */
	unsigned int	l8_clk_div_top04;
	unsigned int	l8_clk_div_top05;
	unsigned int	l8_clk_div_top06;
	unsigned int	l8_clk_div_top07;
	unsigned char	res42[0xbec];
	unsigned int	l9_clk_mux_sel_top03;		/* 0x105D_A20C */
	unsigned int	l9_clk_mux_sel_top04;
	unsigned int	l9_clk_mux_sel_top05;
	unsigned int	l9_clk_mux_sel_top06;
	unsigned int	l9_clk_mux_sel_top07;
	unsigned char	res43[0x3ec];
	unsigned int	l9_clk_div_top03;		/* 0x105D_A60C */
	unsigned int	l9_clk_div_top04;
	unsigned int	l9_clk_div_top05;
	unsigned int	l9_clk_div_top06;
	unsigned int	l9_clk_div_top07;
};

struct exynos7420_clock_top1 {
	unsigned char	res1[0x200];
	unsigned int	clk_mux_sel_top10;		/* 0x105E_0200 */
	unsigned int	clk_mux_sel_top11;
	unsigned char	res2[0x4];
	unsigned int	clk_mux_sel_top13;		/* 0x105E_020C */
	unsigned char	res3[0x14];
	unsigned int	clk_mux_sel_top1_fsys0;		/* 0x105E_0224 */
	unsigned int	clk_mux_sel_top1_fsys1;
	unsigned int	clk_mux_sel_top1_fsys11;
	unsigned char	res4[0xd0];
	unsigned int	clk_mux_enable_top10;		/* 0x105E_0300 */
	unsigned int	clk_mux_enable_top11;
	unsigned char	res5[0x4];
	unsigned int	clk_mux_enable_top13;		/* 0x105E_030C */
	unsigned char	res6[0x14];
	unsigned int	clk_mux_enable_top1_fsys0;	/* 0x105E_0324 */
	unsigned int	clk_mux_enable_top1_fsys1;
	unsigned int	clk_mux_enable_top1_fsys11;
	unsigned char	res7[0xd0];
	unsigned int	clk_mux_stat_top10;		/* 0x105E_0400 */
	unsigned int	clk_mux_stat_top11;
	unsigned char	res8[0x4];
	unsigned int	clk_mux_stat_top13;		/* 0x105E_040C */
	unsigned char	res9[0x14];
	unsigned int	clk_mux_stat_top1_fsys0;	/* 0x105E_0424 */
	unsigned int	clk_mux_stat_top1_fsys1;
	unsigned int	clk_mux_stat_top1_fsys11;
	unsigned char	res10[0x1dc];
	unsigned int	clk_div_top13;			/* 0x105E_060C */
	unsigned char	res11[0x14];
	unsigned int	clk_div_top1_fsys0;		/* 0x105E_0624 */
	unsigned int	clk_div_top1_fsys1;
	unsigned int	clk_div_top1_fsys11;
	unsigned char	res12[0xdc];
	unsigned int	clk_div_stat_top13;		/* 0x105E_070C */
	unsigned char	res13[0x14];
	unsigned int	clk_div_stat_top1_fsys0;	/* 0x105E_0724 */
	unsigned int	clk_div_stat_top1_fsys1;
	unsigned int	clk_div_stat_top1_fsys11;
	unsigned char	res14[0xd8];
	unsigned int	clk_enable_aclk_top12;		/* 0x105E_0808 */
	unsigned int	clk_enable_aclk_top13;
	unsigned char	res15[0x214];
	unsigned int	clk_enable_sclk_top1_fsys0;	/* 0x105E_0A24 */
	unsigned int	clk_enable_sclk_top1_fsys1;
	unsigned int	clk_enable_sclk_top1_fsys11;
	unsigned char	res16[0xd0];
	unsigned int	clk_enable_ip_top1;		/* 0x105E_0B00 */
	unsigned char	res17[0xfc];
	unsigned int	clkout_cmu_top1;		/* 0x105E_0C00 */
	unsigned int	clkout_cmu_top1_div_stat;
	unsigned char	res18[0xfc];
	unsigned int	cmu_top1_rootclken_on;		/* 0x105E_0D04 */
	unsigned char	res19[0xf8];
	unsigned int	cmu_top1_dvfs_ctrl_stat;	/* 0x105E_0E00 */
	unsigned char	res20[0x408];
	unsigned int	l0_clk_mux_sel_top13;		/* 0x105E_120C */
	unsigned char	res21[0x3fc];
	unsigned int	l0_clk_div_top13;		/* 0x105E_160C */
	unsigned char	res22[0xbfc];
	unsigned int	l1_clk_mux_sel_top13;		/* 0x105E_220C */
	unsigned char	res23[0x3fc];
	unsigned int	l1_clk_div_top13;		/* 0x105E_260C */
	unsigned char	res24[0xbfc];
	unsigned int	l2_clk_mux_sel_top13;		/* 0x105E_320C */
	unsigned char	res25[0x3fc];
	unsigned int	l2_clk_div_top13;		/* 0x105E_360C */
	unsigned char	res26[0xbfc];
	unsigned int	l3_clk_mux_sel_top13;		/* 0x105E_420C */
	unsigned char	res27[0x3fc];
	unsigned int	l3_clk_div_top13;		/* 0x105E_460C */
	unsigned char	res28[0xbfc];
	unsigned int	l4_clk_mux_sel_top13;		/* 0x105E_520C */
	unsigned char	res29[0x3fc];
	unsigned int	l4_clk_div_top13;		/* 0x105E_560C */
	unsigned char	res30[0xbfc];
	unsigned int	l5_clk_mux_sel_top13;		/* 0x105E_620C */
	unsigned char	res31[0x3fc];
	unsigned int	l5_clk_div_top13;		/* 0x105E_660C */
	unsigned char	res32[0xbfc];
	unsigned int	l6_clk_mux_sel_top13;		/* 0x105E_720C */
	unsigned char	res33[0x3fc];
	unsigned int	l6_clk_div_top13;		/* 0x105E_760C */
	unsigned char	res34[0xbfc];
	unsigned int	l7_clk_mux_sel_top13;		/* 0x105E_820C */
	unsigned char	res35[0x3fc];
	unsigned int	l7_clk_div_top13;		/* 0x105E_860C */
	unsigned char	res36[0xbfc];
	unsigned int	l8_clk_mux_sel_top13;		/* 0x105E_920C */
	unsigned char	res37[0x3fc];
	unsigned int	l8_clk_div_top13;		/* 0x105E_960C */
	unsigned char	res38[0xbfc];
	unsigned int	l9_clk_mux_sel_top13;		/* 0x105E_A20C */
	unsigned char	res39[0x3fc];
	unsigned int	l9_clk_div_top13;		/* 0x105E_A60C */
};

struct exynos7420_clock_atlas {
	unsigned int	atlas_pll_lock;			/* 0x1180_0000 */
	unsigned char	res1[0xfc];
	unsigned int	atlas_pll_con0;			/* 0x1180_0100 */
	unsigned int	atlas_pll_con1;
	unsigned int	atlas_pll_con2;
	unsigned char	res2[0xf4];
	unsigned int	clk_mux_sel_atlas0;		/* 0x1180_0200 */
	unsigned int	clk_mux_sel_atlas1;
	unsigned int	clk_mux_sel_atlas2;
	unsigned char	res3[0xf4];
	unsigned int	clk_mux_enable_atlas0;		/* 0x1180_0300 */
	unsigned int	clk_mux_enable_atlas1;
	unsigned int	clk_mux_enable_atlas2;
	unsigned char	res4[0xf4];
	unsigned int	clk_mux_stat_atlas0;		/* 0x1180_0400 */
	unsigned int	clk_mux_stat_atlas1;
	unsigned int	clk_mux_stat_atlas2;
	unsigned char	res5[0x1f4];
	unsigned int	clk_div_atlas0;			/* 0x1180_0600 */
	unsigned int	clk_div_atlas1;
	unsigned char	res6[0xf8];
	unsigned int	clk_div_stat_atlas0;		/* 0x1180_0700 */
	unsigned int	clk_div_stat_atlas1;
	unsigned char	res7[0x3f8];
	unsigned int	clk_enable_ip_atlas0;		/* 0x1180_0B00 */
	unsigned int	clk_enable_ip_atlas1;
	unsigned char	res8[0xf8];
	unsigned int	clkout_cmu_atlas;		/* 0x1180_0C00 */
	unsigned int	clkout_cmu_atlas_div_stat;
	unsigned char	res9[0x3f8];
	unsigned int	atlas_armclk_stopctrl;		/* 0x1180_1000 */
	unsigned char	res10[0x1c];
	unsigned int	atlas_pwr_ctrl;			/* 0x1180_1020 */
	unsigned int	atlas_pwr_ctrl2;
	unsigned char	res11[0x58];
	unsigned int	intr_spread_enable;		/* 0x1180_1080 */
	unsigned int	intr_spread_use_standbywfi;
	unsigned int	intr_spread_blocking_duration;
};

struct exynos7420_clock_apollo {
	unsigned int	apollo_pll_lock;		/* 0x1190_0000 */
	unsigned char	res1[0xfc];
	unsigned int	apollo_pll_con0;		/* 0x1190_0100 */
	unsigned int	apollo_pll_con1;		/* 0x1190_0104 */
	unsigned int	apollo_pll_con2;		/* 0x1190_0108 */
	unsigned char	res2[0xf4];
	unsigned int	clk_mux_sel_apollo0;		/* 0x1190_0200 */
	unsigned int	clk_mux_sel_apollo1;		/* 0x1190_0204 */
	unsigned int	clk_mux_sel_apollo2;		/* 0x1190_0208 */
	unsigned char	res3[0xf4];
	unsigned int	clk_mux_enable_apollo0;		/* 0x1190_0300 */
	unsigned int	clk_mux_enable_apollo1;		/* 0x1190_0304 */
	unsigned int	clk_mux_enable_apollo2;		/* 0x1190_0308 */
	unsigned char	res4[0xf4];
	unsigned int	clk_mux_stat_apollo0;		/* 0x1190_0400 */
	unsigned int	clk_mux_stat_apollo1;		/* 0x1190_0404 */
	unsigned int	clk_mux_stat_apollo2;		/* 0x1190_0408 */
	unsigned char	res5[0x1f4];
	unsigned int	clk_div_apollo0;		/* 0x1190_0600 */
	unsigned int	clk_div_apollo1;		/* 0x1190_0604 */
	unsigned char	res6[0xf8];
	unsigned int	clk_div_stat_apollo0;		/* 0x1190_0700 */
	unsigned int	clk_div_stat_apollo1;		/* 0x1190_0704 */
	unsigned char	res7[0x3f8];
	unsigned int	clk_enable_ip_apollo0;		/* 0x1190_0B00 */
	unsigned int	clk_enable_ip_apollo1;		/* 0x1190_0B04 */
	unsigned char	res8[0xf8];
	unsigned int	clkout_cmu_apollo;		/* 0x1190_0C00 */
	unsigned int	clkout_cmu_apollo_div_stat;	/* 0x1190_0C04 */
	unsigned char	res9[0x3f8];
	unsigned int	apollo_armclk_stopctrl;		/* 0x1190_1000 */
	unsigned char	res10[0x1c];
	unsigned int	apollo_pwr_ctrl;		/* 0x1190_1020 */
	unsigned int	apollo_pwr_ctrl2;		/* 0x1190_1024 */
	unsigned char	res11[0x58];
	unsigned int	intr_spread_enable;		/* 0x1190_1080 */
	unsigned int	intr_spread_use_standbywfi;	/* 0x1190_1084 */
	unsigned int	intr_spread_blocking_duration;	/* 0x1190_1088 */
};

struct exynos7420_clock_fsys0 {
	unsigned int	hsic_pll_lock;			/* 0x10E9_0000 */
	unsigned char	res1[0xfc];
	unsigned int	hsic_pll_con0;			/* 0x10E9_0100 */
	unsigned int	hsic_pll_con1;
	unsigned char	res2[0xf8];
	unsigned int	clk_mux_sel_fsys00;		/* 0x10E9_0200 */
	unsigned int	clk_mux_sel_fsys01;
	unsigned int	clk_mux_sel_fsys02;
	unsigned int	clk_mux_sel_fsys03;
	unsigned char	res3[0xf0];
	unsigned int	clk_mux_enable_fsys00;		/* 0x10E9_0300 */
	unsigned int	clk_mux_enable_fsys01;
	unsigned int	clk_mux_enable_fsys02;
	unsigned int	clk_mux_enable_fsys03;
	unsigned char	res4[0xf0];
	unsigned int	clk_mux_stat_fsys00;		/* 0x10E9_0400 */
	unsigned int	clk_mux_stat_fsys01;
	unsigned int	clk_mux_stat_fsys02;
	unsigned int	clk_mux_stat_fsys03;
	unsigned char	res5[0xf8];
	unsigned int	clk_mux_ignore_fsys02;		/* 0x10E9_0508 */
	unsigned int	clk_mux_ignore_fsys03;
	unsigned char	res6[0xf0];
	unsigned int	clk_div_fsys0;			/* 0x10E9_0600 */
	unsigned char	res7[0xfc];
	unsigned int	clk_div_stat_fsys0;		/* 0x10E9_0700 */
	unsigned char	res8[0x3fc];
	unsigned int	clk_enable_ip_fsys00;		/* 0x10E9_0B00 */
	unsigned int	clk_enable_ip_fsys01;
	unsigned char	res9[0xf8];
	unsigned int	clkout_cmu_fsys0;		/* 0x10E9_0C00 */
	unsigned int	clkout_cmu_fsys0_div_stat;
};

struct exynos7420_clock_fsys1 {
	unsigned char	res1[0x200];
	unsigned int	clk_mux_sel_fsys10;		/* 0x156E_0200 */
	unsigned int	clk_mux_sel_fsys11;		/* 0x156E_0204 */
	unsigned int	clk_mux_sel_fsys12;		/* 0x156E_0208 */
	unsigned char	res2[0xf4];
	unsigned int	clk_mux_enable_fsys10;		/* 0x156E_0300 */
	unsigned int	clk_mux_enable_fsys11;		/* 0x156E_0304 */
	unsigned int	clk_mux_enable_fsys12;		/* 0x156E_0308 */
	unsigned char	res3[0xf4];
	unsigned int	clk_mux_stat_fsys10;		/* 0x156E_0400 */
	unsigned int	clk_mux_stat_fsys11;		/* 0x156E_0404 */
	unsigned int	clk_mux_stat_fsys12;		/* 0x156E_0408 */
	unsigned char	res4[0xfc];
	unsigned int	clk_mux_ignore_fsys12;		/* 0x156E_0508 */
	unsigned char	res5[0xf4];
	unsigned int	clk_div_fsys1;			/* 0x156E_0600 */
	unsigned char	res6[0xfc];
	unsigned int	clk_div_stat_fsys1;		/* 0x156E_0700 */
	unsigned char	res7[0xfc];
	unsigned int	clk_enable_aclk_fsys1;		/* 0x156E_0800 */
	unsigned char	res8[0xfc];
	unsigned int	clk_enable_pclk_fsys1;		/* 0x156E_0900 */
	unsigned char	res9[0x100];
	unsigned int	clk_enable_sclk_fsys11;		/* 0x156E_0A04 */
	unsigned int	clk_enable_sclk_fsys12;		/* 0x156E_0A08 */
	unsigned int	clk_enable_sclk_fsys13;		/* 0x156E_0A0C */
	unsigned char	res10[0x1f0];
	unsigned int	clkout_cmu_fsys1;		/* 0x156E_0C00 */
	unsigned int	clkout_cmu_fsys1_div_stat;	/* 0x156E_0C04 */
};

/* EXYNOS7580 CMU */
struct exynos7580_clock_apl {
        unsigned int    apl_pll_lock;                   /* 0x1080_0000 */
        unsigned char   res1[0xfc];
        unsigned int    apl_pll_con0;                   /* 0x1080_0100 */
        unsigned int    apl_pll_con1;                   /* 0x1080_0104 */
        unsigned int    apl_pll_freq_det;               /* 0x1080_0108 */
        unsigned char   res2[0xf4];
        unsigned int    clk_mux_sel_apl0;               /* 0x1080_0200 */
        unsigned int    clk_mux_sel_apl1;               /* 0x1080_0204 */
        unsigned int    clk_mux_sel_apl2;               /* 0x1080_0208 */
        unsigned char   res3[0xf4];
        unsigned int    clk_mux_enable_apl0;            /* 0x1080_0300 */
        unsigned int    clk_mux_enable_apl1;            /* 0x1080_0304 */
        unsigned int    clk_mux_enable_apl2;            /* 0x1080_0308 */
        unsigned char   res4[0xf4];
        unsigned int    clk_mux_stat_apl0;              /* 0x1080_0400 */
        unsigned int    clk_mux_stat_apl1;              /* 0x1080_0404 */
        unsigned int    clk_mux_stat_apl2;              /* 0x1080_0408 */
        unsigned char   res5[0x1f4];
        unsigned int    clk_div_apl0;                   /* 0x1080_0600 */
        unsigned int    clk_div_apl1;                   /* 0x1080_0604 */
        unsigned char   res6[0xf8];
        unsigned int    clk_div_stat_apl0;              /* 0x1080_0700 */
        unsigned int    clk_div_stat_apl1;              /* 0x1080_0704 */
        unsigned char   res7[0xf8];
        unsigned int    clk_enable_aclk_apl;            /* 0x1080_0800 */
        unsigned char   res8[0xfc];
        unsigned int    clk_enable_pclk_apl;            /* 0x1080_0900 */
        unsigned char   res9[0xfc];
        unsigned int    clk_enable_sclk_apl;            /* 0x1080_0A00 */
        unsigned char   res10[0x1fc];
        unsigned int    clkout_cmu_apl;                 /* 0x1080_0C00 */
        unsigned int    clkout_cmu_apl_div_stat;        /* 0x1080_0C04 */
        unsigned char   res11[0xf8];
        unsigned int    clk_enable_pdn_apl;             /* 0x1080_0D00 */
        unsigned char   res12[0x2fc];
        unsigned int    armclk_stropctrl;               /* 0x1080_1000 */
        unsigned char   res13[0x1c];
        unsigned int    pwr_ctrl;                       /* 0x1080_1020 */
        unsigned int    pwr_ctrl2;                      /* 0x1080_1024 */
        unsigned int    pwr_ctrl3;                      /* 0x1080_1028 */
        unsigned char   res14[0x4];
        unsigned int    smpl_ctrl;                      /* 0x1080_1030 */
        unsigned char   res15[0x4c];
        unsigned int    intr_spread_enable;             /* 0x1080_1080 */
        unsigned int    intr_spread_use_standbywfi;     /* 0x1080_1084 */
};

struct exynos7580_clock_bus0 {
        unsigned char   res1[0x5fc];
        unsigned int    clk_div_bus0;                   /* 0x1180_0600 */
        unsigned char   res2[0xfc];
        unsigned int    clk_div_stat_bus0;              /* 0x1180_0700 */
        unsigned char   res3[0xfc];
        unsigned int    clk_enable_aclk_bus0;           /* 0x1180_0800 */
        unsigned char   res4[0xfc];
        unsigned int    clk_enable_pclk_bus0;           /* 0x1180_0900 */
};

struct exynos7580_clock_bus1 {
        unsigned char   res1[0x5fc];
        unsigned int    clk_div_bus1;                   /* 0x1240_0600 */
        unsigned char   res2[0xfc];
        unsigned int    clk_div_stat_bus1;              /* 0x1240_0700 */
        unsigned char   res3[0xfc];
        unsigned int    clk_enable_aclk_bus1;           /* 0x1240_0800 */
        unsigned char   res4[0xfc];
        unsigned int    clk_enable_pclk_bus1;           /* 0x1240_0900 */
};

struct exynos7580_clock_bus2 {
        unsigned char   res1[0x5fc];
        unsigned int    clk_div_bus2;                   /* 0x1300_0600 */
        unsigned char   res2[0xfc];
        unsigned int    clk_div_stat_bus2;              /* 0x1300_0700 */
        unsigned char   res3[0xfc];
        unsigned int    clk_enable_aclk_bus2;           /* 0x1300_0800 */
        unsigned char   res4[0xfc];
        unsigned int    clk_enable_pclk_bus2;           /* 0x1300_0900 */
};

struct exynos7580_clock_cpu {
        unsigned int    cpu_pll_lock;                   /* 0x1090_0000 */
        unsigned char   res1[0xfc];
        unsigned int    cpu_pll_con0;                   /* 0x1090_0100 */
        unsigned int    cpu_pll_con1;                   /* 0x1090_0104 */
        unsigned int    cpu_pll_freq_det;               /* 0x1090_0108 */
        unsigned char   res2[0xf4];
        unsigned int    clk_mux_sel_cpu0;               /* 0x1090_0200 */
        unsigned int    clk_mux_sel_cpu1;               /* 0x1090_0204 */
        unsigned int    clk_mux_sel_cpu2;               /* 0x1090_0208 */
        unsigned char   res3[0xf4];
        unsigned int    clk_mux_enable_cpu0;            /* 0x1090_0300 */
        unsigned int    clk_mux_enable_cpu1;            /* 0x1090_0304 */
        unsigned int    clk_mux_enable_cpu2;            /* 0x1090_0308 */
        unsigned char   res4[0xf4];
        unsigned int    clk_mux_stat_cpu0;              /* 0x1090_0400 */
        unsigned int    clk_mux_stat_cpu1;              /* 0x1090_0404 */
        unsigned int    clk_mux_stat_cpu2;              /* 0x1090_0408 */
        unsigned char   res5[0x1f4];
        unsigned int    clk_div_cpu0;                   /* 0x1090_0600 */
        unsigned int    clk_div_cpu1;                   /* 0x1090_0604 */
        unsigned char   res6[0xf8];
        unsigned int    clk_div_stat_cpu0;              /* 0x1090_0700 */
        unsigned int    clk_div_stat_cpu1;              /* 0x1090_0704 */
        unsigned char   res7[0xf8];
        unsigned int    clk_enable_aclk_cpu;            /* 0x1090_0800 */
        unsigned char   res8[0xfc];
        unsigned int    clk_enable_pclk_cpu;            /* 0x1090_0900 */
        unsigned char   res9[0xfc];
        unsigned int    clk_enable_sclk_cpu;            /* 0x1090_0A00 */
        unsigned char   res10[0x1fc];
        unsigned int    clkout_cmu_cpu;                 /* 0x1090_0C00 */
        unsigned int    clkout_cmu_cpu_div_stat;        /* 0x1090_0C04 */
        unsigned char   res11[0xf8];
        unsigned int    clk_enable_pdn_cpu;             /* 0x1090_0D00 */
        unsigned char   res12[0x2fc];
        unsigned int    armclk_stropctrl;               /* 0x1090_1000 */
        unsigned char   res13[0x1c];
        unsigned int    pwr_ctrl;                       /* 0x1090_1020 */
        unsigned int    pwr_ctrl2;                      /* 0x1090_1024 */
        unsigned int    pwr_ctrl3;                      /* 0x1090_1028 */
        unsigned char   res14[0x4];
        unsigned int    smpl_ctrl;                      /* 0x1090_1030 */
        unsigned char   res15[0x4c];
        unsigned int    intr_spread_enable;             /* 0x1090_1080 */
        unsigned int    intr_spread_use_standbywfi;     /* 0x1090_1084 */
};

struct exynos7580_clock_fsys {
        unsigned int    usb_pll_lock;                   /* 0x1373_0000 */
        unsigned char   res1[0xfc];
        unsigned int    usb_pll_con0;                   /* 0x1373_0100 */
        unsigned int    usb_pll_con1;                   /* 0x1373_0104 */
        unsigned char   res2[0x4];
        unsigned int    usb_pll_freq_det;               /* 0x1373_010C */
        unsigned char   res3[0xf0];
        unsigned int    clk_mux_sel_fsys0;              /* 0x1373_0200 */
        unsigned int    clk_mux_sel_fsys1;              /* 0x1373_0204 */
        unsigned int    clk_mux_sel_fsys2;              /* 0x1373_0208 */
        unsigned char   res4[0xf4];
        unsigned int    clk_mux_enable_fsys0;           /* 0x1373_0300 */
        unsigned int    clk_mux_enable_fsys1;           /* 0x1373_0304 */
        unsigned int    clk_mux_enable_fsys2;           /* 0x1373_0308 */
        unsigned char   res5[0xf4];
        unsigned int    clk_mux_stat_fsys0;             /* 0x1373_0400 */
        unsigned int    clk_mux_stat_fsys1;             /* 0x1373_0404 */
        unsigned int    clk_mux_stat_fsys2;             /* 0x1373_0408 */
        unsigned char   res6[0xf8];
        unsigned int    clk_mux_ignore_fsys1;           /* 0x1373_0504 */
        unsigned int    clk_mux_ignore_fsys2;           /* 0x1373_0508 */
        unsigned char   res7[0x2f4];
        unsigned int    clk_enable_aclk_fsys0;          /* 0x1373_0800 */
        unsigned int    clk_enable_aclk_fsys1;          /* 0x1373_0804 */
        unsigned int    clk_enable_aclk_fsys2;          /* 0x1373_0808 */
        unsigned char   res8[0xf4];
        unsigned int    clk_enable_pclk_fsys;           /* 0x1373_0900 */
        unsigned char   res9[0xfc];
        unsigned int    clk_enable_sclk_fsys;           /* 0x1373_0A00 */
        unsigned char   res10[0x1fc];
        unsigned int    clkout_cmu_fsys;                /* 0x1373_0C00 */
        unsigned int    clkout_cmu_fsys_div_stat;       /* 0x1373_0C04 */
};

struct exynos7580_clock_imem {
        unsigned char   res1[0x7fc];
        unsigned int    clk_enable_aclk_imem;                   /* 0x10C6_0800 */
        unsigned int    clk_enable_aclk_imem_secure_iramc_top;  /* 0x10C6_0804 */
        unsigned int    clk_enable_aclk_imem_secure_sss;        /* 0x10C6_0808 */
        unsigned int    clk_enable_aclk_imem_secure_rtic;       /* 0x10C6_080C */
        unsigned char   res2[0xf0];
        unsigned int    clk_enable_pclk_imem;                   /* 0x10C6_0900 */
        unsigned int    clk_enable_pclk_imem_secure_sss;        /* 0x10C6_0904 */
        unsigned int    clk_enable_pclk_imem_secure_rtic;       /* 0x10C6_0908 */
};

struct exynos7580_clock_mif {
        unsigned int    mem0_pll_lock;                  /* 0x1043_0000 */
        unsigned char   res1[0x4];
        unsigned int    media_pll_lock;                 /* 0x1043_0008 */
        unsigned int    bus_pll_lock;                   /* 0x1043_000C */
        unsigned char   res2[0xf0];
        unsigned int    mem0_pll_con0;                  /* 0x1043_0100 */
        unsigned int    mem0_pll_con1;                  /* 0x1043_0104 */
        unsigned char   res3[0x4];
        unsigned int    mem0_pll_freq_det;              /* 0x1043_010C */
        unsigned char   res4[0x10];
        unsigned int    media_pll_con0;                 /* 0x1043_0120 */
        unsigned int    media_pll_con1;                 /* 0x1043_0124 */
        unsigned char   res5[0x4];
        unsigned int    media_pll_freq_det;             /* 0x1043_012C */
        unsigned int    bus_pll_con0;                   /* 0x1043_0130 */
        unsigned int    bus_pll_con1;                   /* 0x1043_0134 */
        unsigned char   res6[0x4];
        unsigned int    bus_pll_freq_det;               /* 0x1043_013C */
        unsigned char   res7[0xc0];
        unsigned int    clk_mux_sel_mif0;               /* 0x1043_0200 */
        unsigned int    clk_mux_sel_mif1;               /* 0x1043_0204 */
        unsigned int    clk_mux_sel_mif2;               /* 0x1043_0208 */
        unsigned int    clk_mux_sel_mif3;               /* 0x1043_020C */
        unsigned int    clk_mux_sel_mif4;               /* 0x1043_0210 */
        unsigned int    clk_mux_sel_mif5;               /* 0x1043_0214 */
        unsigned int    clk_mux_sel_top_disp;           /* 0x1043_0218 */
        unsigned char   res8[0xe4];
        unsigned int    clk_mux_enable_mif0;            /* 0x1043_0300 */
        unsigned int    clk_mux_enable_mif1;            /* 0x1043_0304 */
        unsigned int    clk_mux_enable_mif2;            /* 0x1043_0308 */
        unsigned int    clk_mux_enable_mif3;            /* 0x1043_030C */
        unsigned int    clk_mux_enable_mif4;            /* 0x1043_0310 */
        unsigned int    clk_mux_enable_mif5;            /* 0x1043_0314 */
        unsigned int    clk_mux_enable_top_disp;        /* 0x1043_0318 */
        unsigned char   res9[0xe4];
        unsigned int    clk_mux_stat_mif0;              /* 0x1043_0400 */
        unsigned int    clk_mux_stat_mif1;              /* 0x1043_0404 */
        unsigned int    clk_mux_stat_mif2;              /* 0x1043_0408 */
        unsigned int    clk_mux_stat_mif3;              /* 0x1043_040C */
        unsigned int    clk_mux_stat_mif4;              /* 0x1043_0410 */
        unsigned int    clk_mux_stat_mif5;              /* 0x1043_0414 */
        unsigned int    clk_mux_stat_top_disp;          /* 0x1043_0418 */
        unsigned char   res10[0x4];
        unsigned int    clk_mux_stat_cpmux;             /* 0x1043_0420 */
        unsigned char   res11[0x1dc];
        unsigned int    clk_div_mif0;                   /* 0x1043_0600 */
        unsigned int    clk_div_mif1;                   /* 0x1043_0604 */
        unsigned char   res12[0x8];
        unsigned int    clk_div_top_disp;               /* 0x1043_0610 */
        unsigned char   res13[0xec];
        unsigned int    clk_div_stat_mif0;              /* 0x1043_0700 */
        unsigned int    clk_div_stat_mif1;              /* 0x1043_0704 */
        unsigned char   res14[0x8];
        unsigned int    clk_div_stat_top_disp;          /* 0x1043_0710 */
        unsigned char   res15[0xec];
        unsigned int    clk_enable_aclk_mif0;           /* 0x1043_0800 */
        unsigned int    clk_enable_aclk_mif1;           /* 0x1043_0804 */
        unsigned int    clk_enable_aclk_mif2;           /* 0x1043_0808 */
        unsigned char   res16[0x8];
        unsigned int    clk_enable_aclk_top_disp;       /* 0x1043_0810 */
        unsigned char   res17[0xec];
        unsigned int    clk_enable_pclk_mif;            /* 0x1043_0900 */
        unsigned int    clk_enable_pclk_mif_secure_drex0_tz;            /* 0x1043_0904 */
        unsigned char   res18[0xf8];
        unsigned int    clk_enable_sclk_mif0;           /* 0x1043_0A00 */
        unsigned char   res19[0xc];
        unsigned int    clk_enable_sclk_top;            /* 0x1043_0A10 */
        unsigned int    clk_enable_sclk_top_disp;       /* 0x1043_0A14 */
        unsigned char   res20[0x1e8];
        unsigned int    clkout_cmu_mif;                 /* 0x1043_0C00 */
        unsigned int    clkout_cmu_mif_div_stat;        /* 0x1043_0C04 */
        unsigned char   res21[0xf8];
        unsigned int    clk_enable_pdn_mif;             /* 0x1043_0D00 */
        unsigned char   res22[0xfc];
        unsigned int    auto_root_clk_gate_mif;         /* 0x1043_0E00 */
        unsigned char   res23[0x1fc];
        unsigned int    drex_freq_ctrl0;                /* 0x1043_1000 */
        unsigned int    drex_freq_ctrl1;                /* 0x1043_1004 */
        unsigned int    pause;                          /* 0x1043_1008 */
        unsigned int    ddrphy_lock_ctrl;               /* 0x1043_100C */
        unsigned int    acg_enable;                     /* 0x1043_1010 */
};


struct exynos7580_clock_peri {
        unsigned char   res1[0x1fc];
        unsigned int    clk_mux_sel_peri;               /* 0x139F_0200 */
        unsigned char   res2[0xfc];
        unsigned int    clk_mux_enable_peri;            /* 0x139F_0300 */
        unsigned char   res3[0x4fc];
        unsigned int    clk_enable_aclk_peri;           /* 0x139F_0800 */
        unsigned char   res4[0xfc];
        unsigned int    clk_enable_pclk_peris;          /* 0x139F_0900 */
        unsigned int    clk_enable_pclk_peric;          /* 0x139F_0904 */
        unsigned char   res5[0x8];
        unsigned int    clk_enable_pclk_peris_secure_tzpc;              /* 0x139F_0910 */
        unsigned int    clk_enable_pclk_peris_secure_seckey_apbif;      /* 0x139F_0914 */
        unsigned int    clk_enable_pclk_peris_secure_chipid_apbif;      /* 0x139F_0918 */
        unsigned int    clk_enable_pclk_peris_secure_rtc_top;           /* 0x139F_091C */
        unsigned char   res6[0x8];
        unsigned int    clk_enable_pclk_peris_secure_antirbk_cnt_apbif; /* 0x139F_0928 */
        unsigned int    clk_enable_pclk_peris_secure_monotonic_cnt_apbif;       /* 0x139F_092C */
        unsigned int    clk_enable_pclk_peris_secure_rtc_apbif;         /* 0x139F_0930 */
        unsigned char   res7[0x1cc];
        unsigned int    clk_enable_sclk_peris;          /* 0x139F_0A00 */
        unsigned int    clk_enable_sclk_peric;          /* 0x139F_0A04 */
        unsigned char   res8[0x8];
        unsigned int    clk_enable_sclk_peris_secure_seckey;            /* 0x139F_0A10 */
        unsigned int    clk_enable_sclk_peris_secure_chipid;            /* 0x139F_0A14 */
        unsigned char   res9[0x8];
        unsigned int    clk_enable_sclk_peris_secure_antirbk_cnt;       /* 0x139F_0A20 */
};

struct exynos7580_clock_top {
        unsigned int    aud_pll_lock;                   /* 0x1001_0000 */
        unsigned char   res1[0xfc];
        unsigned int    aud_pll_con0;                   /* 0x1001_0100 */
        unsigned int    aud_pll_con1;                   /* 0x1001_0104 */
        unsigned int    aud_pll_con2;                   /* 0x1001_0108 */
        unsigned int    aud_pll_freq_det;               /* 0x1001_010C */
        unsigned char   res2[0xf0];
        unsigned int    clk_mux_sel_top0;               /* 0x1001_0200 */
        unsigned int    clk_mux_sel_top1;               /* 0x1001_0204 */
        unsigned int    clk_mux_sel_top2;               /* 0x1001_0208 */
        unsigned char   res3[0x14];
        unsigned int    clk_mux_sel_top_fsys0;          /* 0x1001_0220 */
        unsigned int    clk_mux_sel_top_fsys1;          /* 0x1001_0224 */
        unsigned int    clk_mux_sel_top_fsys2;          /* 0x1001_0228 */
        unsigned char   res4[0x4];
        unsigned int    clk_mux_sel_top_peri;           /* 0x1001_0230 */
        unsigned int    clk_mux_sel_top_isp;            /* 0x1001_0234 */
        unsigned char   res5[0xc8];
        unsigned int    clk_mux_enable_top0;            /* 0x1001_0300 */
        unsigned int    clk_mux_enable_top1;            /* 0x1001_0304 */
        unsigned int    clk_mux_enable_top2;            /* 0x1001_0308 */
        unsigned char   res6[0x14];
        unsigned int    clk_mux_enable_top_fsys0;       /* 0x1001_0320 */
        unsigned int    clk_mux_enable_top_fsys1;       /* 0x1001_0324 */
        unsigned int    clk_mux_enable_top_fsys2;       /* 0x1001_0328 */
        unsigned char   res7[0x4];
        unsigned int    clk_mux_enable_top_peri;        /* 0x1001_0330 */
        unsigned int    clk_mux_enable_top_isp;         /* 0x1001_0334 */
        unsigned char   res8[0xc8];
        unsigned int    clk_mux_stat_top0;              /* 0x1001_0400 */
        unsigned int    clk_mux_stat_top1;              /* 0x1001_0404 */
        unsigned int    clk_mux_stat_top2;              /* 0x1001_0408 */
        unsigned char   res9[0x14];
        unsigned int    clk_mux_stat_top_fsys0;         /* 0x1001_0420 */
        unsigned int    clk_mux_stat_top_fsys1;         /* 0x1001_0424 */
        unsigned int    clk_mux_stat_top_fsys2;         /* 0x1001_0428 */
        unsigned char   res10[0x4];
        unsigned int    clk_mux_stat_top_peri;          /* 0x1001_0430 */
        unsigned int    clk_mux_stat_top_isp;           /* 0x1001_0434 */
        unsigned char   res11[0x1c8];
        unsigned int    clk_div_top0;                   /* 0x1001_0600 */
        unsigned int    clk_div_top1;                   /* 0x1001_0604 */
        unsigned char   res12[0x18];
        unsigned int    clk_div_top_fsys0;              /* 0x1001_0620 */
        unsigned int    clk_div_top_fsys1;              /* 0x1001_0624 */
        unsigned int    clk_div_top_fsys2;              /* 0x1001_0628 */
        unsigned char   res13[0x4];
        unsigned int    clk_div_top_peri0;              /* 0x1001_0630 */
        unsigned int    clk_div_top_peri1;              /* 0x1001_0634 */
        unsigned int    clk_div_top_peri2;              /* 0x1001_0638 */
        unsigned int    clk_div_top_peri3;              /* 0x1001_063C */
        unsigned int    clk_div_top_isp0;               /* 0x1001_0640 */
        unsigned int    clk_div_top_isp1;               /* 0x1001_0644 */
        unsigned char   res14[0xb8];
        unsigned int    clk_div_stat_top0;              /* 0x1001_0700 */
        unsigned int    clk_div_stat_top1;              /* 0x1001_0704 */
        unsigned char   res15[0x18];
        unsigned int    clk_div_stat_top_fsys0;         /* 0x1001_0720 */
        unsigned int    clk_div_stat_top_fsys1;         /* 0x1001_0724 */
        unsigned int    clk_div_stat_top_fsys2;         /* 0x1001_0728 */
        unsigned char   res16[0x4];
        unsigned int    clk_div_stat_top_peri0;         /* 0x1001_0730 */
        unsigned int    clk_div_stat_top_peri1;         /* 0x1001_0734 */
        unsigned int    clk_div_stat_top_peri2;         /* 0x1001_0738 */
        unsigned int    clk_div_stat_top_peri3;         /* 0x1001_073C */
        unsigned int    clk_div_stat_top_isp0;          /* 0x1001_0740 */
        unsigned int    clk_div_stat_top_isp1;          /* 0x1001_0744 */
        unsigned char   res17[0xb8];
        unsigned int    clk_enable_aclk_top;            /* 0x1001_0800 */
        unsigned char   res18[0x1fc];
        unsigned int    clk_enable_sclk_top_fsys;       /* 0x1001_0A00 */
        unsigned int    clk_enable_sclk_top_peri;       /* 0x1001_0A04 */
        unsigned char   res19[0x4];
        unsigned int    clk_enable_sclk_top_isp;        /* 0x1001_0A0C */
        unsigned char   res20[0x1f0];
        unsigned int    clkout_cmu_top;                 /* 0x1001_0C00 */
        unsigned int    clkout_cmu_top_div_stat;        /* 0x1001_0C04 */
        unsigned char   res21[0xf8];
        unsigned int    clk_enable_pdn_top;             /* 0x1001_0D00 */
};

struct exynos5412_clock {
	unsigned int	apll_lock;			/* 0x1001_0000 */
	unsigned char	res1[0xfc];
	unsigned int	apll_con0;			/* 0x1001_0100 */
	unsigned int	apll_con1;			/* 0x1001_0104 */
	unsigned char	res2[0xf8];
	unsigned int	src_cpu;			/* 0x1001_0200 */
	unsigned char	res3[0x2fc];
	unsigned int	div_cpu0;			/* 0x1001_0500 */
	unsigned int	div_cpu1;			/* 0x1001_0504 */
	unsigned char	res4[0x4f8];
	unsigned int	clkout_cmu_cpu;			/* 0x1001_0A00 */
	unsigned char	res5[0x37fc];
	unsigned int	src_core0;			/* 0x1001_4200 */
	unsigned int	src_core1;			/* 0x1001_4204 */
	unsigned char	res6[0x2fc];
	unsigned int	div_cperi1;			/* 0x1001_4504 */
	unsigned char	res7[0x4f8];
	unsigned int	clkout_cmu_core;		/* 0x1001_4A00 */
	unsigned char	res8[0x3afc];
	unsigned int	div_g2d;			/* 0x1001_8500 */
	unsigned char	res9[0x3DFC];
	unsigned int	div_isp0;			/* 0x1001_C300 */
	unsigned int	div_isp1;			/* 0x1001_C304 */
	unsigned int	div_isp2;			/* 0x1001_C308 */
	unsigned char	res91[0x3D14];
	unsigned int	cpll_lock;			/* 0x1002_0020 */
	unsigned char	res10[0xc];
	unsigned int	dpll_lock;			/* 0x1002_0030 */
	unsigned char	res11[0xc];
	unsigned int	epll_lock;			/* 0x1002_0040 */
	unsigned char	res12[0xc];
	unsigned int	rpll_lock;			/* 0x1002_0050 */
	unsigned char	res13[0xc];
	unsigned int	ipll_lock;			/* 0x1002_0060 */
	unsigned char	res14[0xc];
	unsigned int	spll_lock;			/* 0x1002_0070 */
	unsigned char	res15[0xc];
	unsigned int	vpll_lock;			/* 0x1002_0080 */
	unsigned char	res16[0xc];
	unsigned int	mpll_lock;			/* 0x1002_0090 */
	unsigned int	tpll_lock;			/* 0x1002_0094 */
	unsigned char	res17[0x88];
	unsigned int	cpll_con0;			/* 0x1002_0120 */
	unsigned int	cpll_con1;			/* 0x1002_0124 */
	unsigned int	dpll_con0;			/* 0x1002_0128 */
	unsigned int	dpll_con1;			/* 0x1002_012C */
	unsigned int	epll_con0;			/* 0x1002_0130 */
	unsigned int	epll_con1;			/* 0x1002_0134 */
	unsigned int	epll_con2;			/* 0x1002_0138 */
	unsigned char	res18[0x4];
	unsigned int	rpll_con0;			/* 0x1002_0140 */
	unsigned int	rpll_con1;			/* 0x1002_0144 */
	unsigned int	rpll_con2;			/* 0x1002_0148 */
	unsigned char	res19[0x4];
	unsigned int	ipll_con0;			/* 0x1002_0150 */
	unsigned int	ipll_con1;			/* 0x1002_0154 */
	unsigned char	res20[0x8];
	unsigned int	spll_con0;			/* 0x1002_0160 */
	unsigned int	spll_con1;			/* 0x1002_0164 */
	unsigned char	res21[0x8];
	unsigned int	vpll_con0;			/* 0x1002_0170 */
	unsigned int	vpll_con1;			/* 0x1002_0174 */
	unsigned char	res22[0x8];
	unsigned int	mpll_con0;			/* 0x1002_0180 */
	unsigned int	mpll_con1;			/* 0x1002_0184 */
	unsigned char	res231[0x8];
	unsigned int	tpll_con0;			/* 0x1002_0190 */
	unsigned int	tpll_con1;			/* 0x1002_0194 */
	unsigned char	res232[0x68];
	unsigned int	src_top0;			/* 0x1002_0200 */
	unsigned int	src_top1;			/* 0x1002_0204 */
	unsigned int	src_top2;			/* 0x1002_0208 */
	unsigned int	src_top3;			/* 0x1002_020C */
	unsigned int	src_top4;			/* 0x1002_0210 */
	unsigned int	src_top5;			/* 0x1002_0214 */
	unsigned int	src_top6;			/* 0x1002_0218 */
	unsigned int	src_top7;			/* 0x1002_021C */
	unsigned int	src_top8;			/* 0x1002_0220 */
	unsigned int	src_top9;			/* 0x1002_0224 */
	unsigned char	res24[0x4];
	unsigned int	src_disp10;			/* 0x1002_022C */
	unsigned char	res25[0x10];
	unsigned int	src_mau;			/* 0x1002_0240 */
	unsigned int	src_fsys;			/* 0x1002_0244 */
	unsigned char	res26[0x8];
	unsigned int	src_peric0;			/* 0x1002_0250 */
	unsigned int	src_peric1;			/* 0x1002_0254 */
	unsigned char	res27[0x18];
	unsigned int	src_isp;			/* 0x1002_0270 */
	unsigned char	res28[0xC];
	unsigned int	src_top10;			/* 0x1002_0280 */
	unsigned int	src_top11;			/* 0x1002_0284 */
	unsigned int	src_top12;			/* 0x1002_0288 */
	unsigned char	res29[0x274];
	unsigned int	div_top0;			/* 0x1002_0500 */
	unsigned int	div_top1;			/* 0x1002_0504 */
	unsigned int	div_top2;			/* 0x1002_0508 */
	unsigned int	div_top3;			/* 0x1002_050C */
	unsigned char	res30[0x1C];
	unsigned int	div_disp10;			/* 0x1002_052C */
	unsigned char	res31[0x14];
	unsigned int	div_mau;			/* 0x1002_0544 */
	unsigned int	div_fsys0;			/* 0x1002_0548 */
	unsigned int	div_fsys1;			/* 0x1002_054C */
	unsigned int	div_fsys2;			/* 0x1002_0550 */
	unsigned int	div_fsys3;			/* 0x1002_0554 */
	unsigned int	div_peric0;			/* 0x1002_0558 */
	unsigned int	div_peric1;			/* 0x1002_055C */
	unsigned int	div_peric2;			/* 0x1002_0560 */
	unsigned int	div_peric3;			/* 0x1002_0564 */
	unsigned int	div_peric4;			/* 0x1002_0568 */
	unsigned char	res32[0x14];
	unsigned int	div_s_isp0;			/* 0x1002_0580 */
	unsigned int	div_s_isp1;			/* 0x1002_0584 */
	unsigned char	res33[0x8];
	unsigned int	div2_ratio0;			/* 0x1002_0590 */
	unsigned char	res34[0xc];
	unsigned int	div4_ratio;			/* 0x1002_05A0 */
	unsigned char	res35[0x45c];
	unsigned int	clkout_cmu_top;			/* 0x1002_0A00 */
	unsigned char	res36[0xfc];
	unsigned int	wdrex0_pause;			/* 0x1002_0B00 */
	unsigned int	wdrex1_pause;			/* 0x1002_0B04 */
	unsigned int	wdrex2_pause;			/* 0x1002_0B08 */
	unsigned int	wdrex3_pause;			/* 0x1002_0B0C */
	unsigned char	res361[0xf500];
	unsigned int	xpll_lock;			/* 0x1003_0010 */
	unsigned char	res37[0xfc];
	unsigned int	xpll_con0;			/* 0x1003_0110 */
	unsigned int	xpll_con1;			/* 0x1003_0114 */
	unsigned char	res38[0xe8];
	unsigned int	src_cdrex;			/* 0x1003_0200 */
	unsigned char	res39[0x1fc];
	unsigned int	mux_stat_cdrex;			/* 0x1003_0400 */
	unsigned char	res40[0xfc];
	unsigned int	div_cdrex0;			/* 0x1003_0500 */
	unsigned int	div_cdrex1;			/* 0x1003_0504 */
	unsigned char	res41[0x414];
	unsigned int	pause;				/* 0x1003_091C */
	unsigned char	res412[0xe0];
	unsigned int	clkout_cmu_cdrex;		/* 0x1003_0A00 */
        unsigned char	res42[0xc];
	unsigned int	lpddr3phy_ctrl;			/* 0x1003_0A10 */
	unsigned int	lpddr3phy_con0;			/* 0x1003_0A14 */
	unsigned int	lpddr3phy_con1;			/* 0x1003_0A18 */
	unsigned int	lpddr3phy_con2;			/* 0x1003_0A1C */
	unsigned int	lpddr3phy_con3;			/* 0x1003_0A20 */
	unsigned int	lpddr3phy_con4;			/* 0x1003_0A24 */
	unsigned int	lpddr3phy_con5;			/* 0x1003_0A28 */
	unsigned char	res43[0x35E4];
	unsigned int	bpll_lock;			/* 0x1003_4010 */
	unsigned char	res44[0xfc];
	unsigned int	bpll_con0;			/* 0x1003_4110 */
	unsigned int	bpll_con1;			/* 0x1003_4114 */
	unsigned char	res45[0xe8];
	unsigned int	src_cci;			/* 0x1003_4200 */
	unsigned char	res46[0x2fc];
	unsigned int	div_cci0;			/* 0x1003_4500 */
	unsigned char	res461[0x3afc];
	unsigned int	kpll_lock;			/* 0x1003_8000 */
	unsigned char	res47[0xfc];
	unsigned int	kpll_con0;			/* 0x1003_8100 */
	unsigned int	kpll_con1;			/* 0x1003_8104 */
	unsigned char	res48[0xf8];
	unsigned int	src_kfc;			/* 0x1003_8200 */
	unsigned char	res49[0x2fc];
	unsigned int	div_kfc0;			/* 0x1003_8500 */
};
#endif

#endif
