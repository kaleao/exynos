/*
 * Exynos Generic power domain support.
 *
 * Copyright (c) 2013 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * Implementation of Exynos specific power domain control which is used in
 * conjunction with runtime-pm. Support for both device-tree and non-device-tree
 * based power domain support is included.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <mach/tmu.h>
#include <mach/apm-exynos.h>
#include "pm_domains-exynos7420.h"
#include <linux/mfd/samsung/core.h>

struct exynos7420_pd_data *exynos_pd_find_data(struct exynos_pm_domain *pd)
{
	int i;
	struct exynos7420_pd_data *pd_data;

	/* find pd_data for each power domain */
	for (i = 0, pd_data = &pd_data_list[0]; i < ARRAY_SIZE(pd_data_list); i++, pd_data++) {
		if (strcmp(pd_data->name, pd->name))
			continue;

		DEBUG_PRINT_INFO("%s: found pd_data\n", pd->name);
		break;
	}

	return pd_data;
}

int exynos_pd_clk_get(struct exynos_pm_domain *pd)
{
	int i = 0;
	struct exynos7420_pd_data *pd_data = pd->pd_data;
	struct exynos_pd_clk *top_clks = NULL;

	if (!pd_data) {
		pr_err(PM_DOMAIN_PREFIX "pd_data is null\n");
		return -ENODEV;
	}

	top_clks = pd_data->top_clks;

	for (i = 0; i < pd_data->num_top_clks; i++) {
		top_clks[i].clock = samsung_clk_get_by_reg((unsigned long)top_clks[i].reg, top_clks[i].bit_offset);
		if (!top_clks[i].clock) {
			pr_err(PM_DOMAIN_PREFIX "clk_get of %s's top clock has been failed\n", pd->name);
			return -ENODEV;
		}
	}

	return 0;
}

static int exynos5_pd_enable_ccf_clks(struct exynos_pd_clk *ptr, int nr_regs)
{
	unsigned int ret;

	for (; nr_regs > 0; nr_regs--, ptr++) {
		ret = clk_prepare_enable(ptr->clock);
		if (ret)
			return ret;
		DEBUG_PRINT_INFO("clock name : %s, usage_count : %d, SFR : 0x%x\n",
			ptr->clock->name, ptr->clock->enable_count, __raw_readl(ptr->reg));
	}
	return 0;
}

static void exynos5_pd_disable_ccf_clks(struct exynos_pd_clk *ptr, int nr_regs)
{
	for (; nr_regs > 0; nr_regs--, ptr++) {
		if (ptr->clock->enable_count > 0) {
			clk_disable_unprepare(ptr->clock);
			DEBUG_PRINT_INFO("clock name : %s, usage_count : %d, SFR : 0x%x\n",
				ptr->clock->name, ptr->clock->enable_count, __raw_readl(ptr->reg));
		}
	}
}

static void exynos5_pd_enable_clks(struct exynos_pd_clk *ptr, int nr_regs)
{
	unsigned int reg;

	if (!ptr)
		return;

	for (; nr_regs > 0; nr_regs--, ptr++) {
		reg = __raw_readl(ptr->reg);
		reg |= (1 << ptr->bit_offset);
		__raw_writel(reg, ptr->reg);
	}
}

static void exynos5_pd_enable_asyncbridge_clks(struct exynos_pd_clk *ptr, int nr_regs)
{
	unsigned int reg;
	struct exynos_pm_domain *exypd = NULL;

	if (!ptr)
		return;

	for (; nr_regs > 0; nr_regs--, ptr++) {
		exypd = exynos_pd_lookup_name(ptr->domain_name);
		if (!exypd)
			continue;

		mutex_lock(&exypd->access_lock);
		if ((__raw_readl(exypd->base+0x4) & LOCAL_PWR_CFG) == LOCAL_PWR_CFG) {
			reg = __raw_readl(ptr->reg);
			reg |= (1 << ptr->bit_offset);
			__raw_writel(reg, ptr->reg);
		}
		mutex_unlock(&exypd->access_lock);
	}
}

static void exynos5_pd_set_sys_pwr_regs(struct exynos_pd_reg *ptr, int nr_regs)
{
	unsigned int reg;

	if (!ptr)
		return;

	for (; nr_regs > 0; nr_regs--, ptr++) {
		reg = __raw_readl(ptr->reg);
		reg &= ~(1 << ptr->bit_offset);
		__raw_writel(reg, ptr->reg);
	}
}

static void exynos_pd_notify_power_state(struct exynos_pm_domain *pd, unsigned int turn_on)
{
#if defined(CONFIG_ARM_EXYNOS7420_BUS_DEVFREQ) && defined(CONFIG_PM_RUNTIME)
	exynos7_int_notify_power_status(pd->genpd.name, true);
	exynos7_isp_notify_power_status(pd->genpd.name, true);
	exynos7_disp_notify_power_status(pd->genpd.name, true);
#endif
}

static int exynos_pd_power_init(struct exynos_pm_domain *pd)
{
	int ret;
	struct exynos7420_pd_data *pd_data = pd->pd_data;

	DEBUG_PRINT_INFO("%s's init callback start\n", pd->name);

	/* Enabling Top-to-Local clocks first (due to power-off unused)*/
	ret = exynos5_pd_enable_ccf_clks(pd_data->top_clks, pd_data->num_top_clks);
	if (ret) {
		pr_err("PM DOMAIN : enable_ccf_clks of %s has been failed\n", pd->name);
		return ret;
	}

	if (!strcmp(pd->name, "pd-g3d")) {
		gpu_dvs_ctrl = ioremap(0x105c6100, SZ_4K);
		if (IS_ERR_OR_NULL(gpu_dvs_ctrl))
			pr_err("PM DOMAIN : ioremap of gpu_dvs_ctrl failed\n");
	}

	/* BLK_CAM1 only : ioremap for LPI_MASK_CAM1_ASYNCBRIDGE */
	if (!strcmp(pd->name, "pd-cam1")) {
		lpi_mask_cam1_asyncbridge = ioremap(0x145e0020, SZ_4K);
		if (IS_ERR_OR_NULL(lpi_mask_cam1_asyncbridge))
			pr_err("PM DOMAIN : ioremap of lpi_mask_cam1_asyncbridge fail\n");
	}

	DEBUG_PRINT_INFO("%s's init callback end\n", pd->name);

	return 0;
}

static int exynos_pd_power_on_pre(struct exynos_pm_domain *pd)
{
	int ret;
	struct exynos7420_pd_data *pd_data = pd->pd_data;

	DEBUG_PRINT_INFO("%s's on_pre callback start\n", pd->name);

	/* 1. Enabling Top-to-Local clocks */
	ret = exynos5_pd_enable_ccf_clks(pd_data->top_clks, pd_data->num_top_clks);
	if (ret) {
		pr_err("PM DOMAIN : enable_ccf_clks of %s has been failed\n", pd->name);
		return ret;
	}

	/* 2. Enabling related Async-bridge clocks */
	exynos5_pd_enable_asyncbridge_clks(pd_data->asyncbridge_clks, pd_data->num_asyncbridge_clks);

	/* 3. Setting sys pwr regs */
	exynos5_pd_set_sys_pwr_regs(pd_data->sys_pwr_regs, pd_data->num_sys_pwr_regs);

	if (!strcmp(pd->name, "pd-g3d")) {
		/* BLK_G3D only : WA for tmu noise at G3D power transition */
		exynos_tmu_core_control(false, EXYNOS_TMU_CORE_GPU);
		/* BUCK6EN gpio pin set to SCALL__G3D__ALO */
		g3d_pin_config_set();
	}

	DEBUG_PRINT_INFO("%s's on_pre callback end\n", pd->name);

	return 0;
}

static int exynos_pd_power_on_post(struct exynos_pm_domain *pd)
{
	struct exynos7420_pd_data *pd_data = pd->pd_data;
	unsigned int temp;

	DEBUG_PRINT_INFO("%s's on_post callback start\n", pd->name);

	/* 1. Restoring resetted SFRs belonged to this block */
	exynos_restore_sfr(pd_data->save_list, pd_data->num_save_list);

	/* BLK_G3D only : WA for tmu noise at G3D power transition */
	if (!strcmp(pd->name, "pd-g3d"))
		exynos_tmu_core_control(true, EXYNOS_TMU_CORE_GPU);

	/* Except for G3D, AUD : notify state of individual domain to DEVFreq */
	if (!strcmp(pd->name, "pd-cam0") || !strcmp(pd->name, "pd-cam1") || !strcmp(pd->name, "pd-isp0") ||
		!strcmp(pd->name, "pd-isp1") || !strcmp(pd->name, "pd-vpp") || !strcmp(pd->name, "pd-disp") ||
		!strcmp(pd->name, "pd-g2d") || !strcmp(pd->name, "pd-mscl") || !strcmp(pd->name, "pd-mfc") ||
		!strcmp(pd->name, "pd-hevc"))
		exynos_pd_notify_power_state(pd, true);

#ifdef CONFIG_EXYNOS_MBOX
	if (!strcmp(pd->name, "pd-g3d")) {
		exynos7420_g3d_power_on_noti_apm();
	}
#endif
	/* BLK_ISP0 only : LPI setting for autoclock gating */
	if (!strcmp(pd->name, "pd-isp0")) {
		temp = __raw_readl(EXYNOS_PMU_LPI_AUTOMATIC_CLKGATE_ISP0_BUSMASTER);
		temp |= 0x3;
		__raw_writel(temp, EXYNOS_PMU_LPI_AUTOMATIC_CLKGATE_ISP0_BUSMASTER);
	}

	DEBUG_PRINT_INFO("%s's on_post callback end\n", pd->name);

	return 0;
}

static int exynos_pd_power_off_pre(struct exynos_pm_domain *pd)
{
	struct exynos7420_pd_data *pd_data = pd->pd_data;

	DEBUG_PRINT_INFO("%s's off_pre callback start\n", pd->name);

#ifdef CONFIG_EXYNOS_MBOX
	if (!strcmp(pd->name, "pd-g3d")) {
		exynos7420_g3d_power_down_noti_apm();
	}
#endif

	if (!strcmp(pd->name, "pd-g3d")) {
		/* BUCK6EN gpio pin set to SCALL__G3D__ALO */
		g3d_pin_config_set();

		if (s2m_get_dvs_is_on()) {
			if (s2m_set_dvs_pin(false) < 0) {
				pr_err(PM_DOMAIN_PREFIX "timed out during dvs status check\n");
			}
		}
	}

	/* 1. Save SFRs belonged to this block */
	exynos_save_sfr(pd_data->save_list, pd_data->num_save_list);

	/* 2. Enable local clocks to prevent LPI stuck */
	exynos5_pd_enable_clks(pd_data->local_clks, pd_data->num_local_clks);

	/* BLK_AUD only : CLKMUX switch from IOCLK to PLL source */
	if (!strcmp(pd->name, "pd-aud")) {
		unsigned int reg;
		reg = __raw_readl(EXYNOS7420_MUX_SEL_AUD);
		reg &= ~((1 << 16) | (1 << 12));
		__raw_writel(reg, EXYNOS7420_MUX_SEL_AUD);
	}

	/* 3. Enabling related Async-bridge clocks */
	exynos5_pd_enable_asyncbridge_clks(pd_data->asyncbridge_clks, pd_data->num_asyncbridge_clks);

	/* 4. Setting sys pwr regs */
	exynos5_pd_set_sys_pwr_regs(pd_data->sys_pwr_regs, pd_data->num_sys_pwr_regs);


	DEBUG_PRINT_INFO("%s's off_pre callback end\n", pd->name);

	return 0;
}

static int exynos_pd_power_forced_off_pre(struct exynos_pm_domain *pd)
{
	/* BLK_CAM1 only : LPI_MASK_CAM1_ASYNCBRIDGE. */
	if (!strcmp(pd->name, "pd-cam1")) {
		unsigned int reg;
		reg = __raw_readl(lpi_mask_cam1_asyncbridge);
		reg |= (1 << 5) | (1 << 4);
		__raw_writel(reg, lpi_mask_cam1_asyncbridge);
	}

	return 0;
}

static int exynos_pd_power_off_post(struct exynos_pm_domain *pd)
{
	struct exynos7420_pd_data *pd_data = pd->pd_data;

	DEBUG_PRINT_INFO("%s's off_post callback start\n", pd->name);

	/* 1. Disabling Top-to-Local clocks */
	exynos5_pd_disable_ccf_clks(pd_data->top_clks, pd_data->num_top_clks);

	DEBUG_PRINT_INFO("%s's off_post callback end\n", pd->name);

	return 0;
}

static struct exynos_pd_callback pd_callback_list[] = {
	{
		.name = "pd-aud",
		.init = exynos_pd_power_init,
		.on_pre = exynos_pd_power_on_pre,
		.on_post = exynos_pd_power_on_post,
		.off_pre = exynos_pd_power_off_pre,
		.forced_off_pre = exynos_pd_power_forced_off_pre,
	}, {
		.name = "pd-mfc",
		.init = exynos_pd_power_init,
		.on_pre = exynos_pd_power_on_pre,
		.on_post = exynos_pd_power_on_post,
		.off_pre = exynos_pd_power_off_pre,
		.forced_off_pre = exynos_pd_power_forced_off_pre,
		.off_post = exynos_pd_power_off_post,
	}, {
		.name = "pd-vpp",
		.init = exynos_pd_power_init,
		.on_pre = exynos_pd_power_on_pre,
		.on_post = exynos_pd_power_on_post,
		.off_pre = exynos_pd_power_off_pre,
		.forced_off_pre = exynos_pd_power_forced_off_pre,
		.off_post = exynos_pd_power_off_post,
	}, {
		.name = "pd-g3d",
		.init = exynos_pd_power_init,
		.on_pre = exynos_pd_power_on_pre,
		.on_post = exynos_pd_power_on_post,
		.off_pre = exynos_pd_power_off_pre,
		.forced_off_pre = exynos_pd_power_forced_off_pre,
		.off_post = exynos_pd_power_off_post,
	}, {
		.name = "pd-disp",
		.init = exynos_pd_power_init,
		.on_pre = exynos_pd_power_on_pre,
		.on_post = exynos_pd_power_on_post,
		.off_pre = exynos_pd_power_off_pre,
		.forced_off_pre = exynos_pd_power_forced_off_pre,
		.off_post = exynos_pd_power_off_post,
	}, {
		.name = "pd-mscl",
		.init = exynos_pd_power_init,
		.on_pre = exynos_pd_power_on_pre,
		.on_post = exynos_pd_power_on_post,
		.off_pre = exynos_pd_power_off_pre,
		.forced_off_pre = exynos_pd_power_forced_off_pre,
		.off_post = exynos_pd_power_off_post,
	}, {
		.name = "pd-isp0",
		.init = exynos_pd_power_init,
		.on_pre = exynos_pd_power_on_pre,
		.on_post = exynos_pd_power_on_post,
		.off_pre = exynos_pd_power_off_pre,
		.forced_off_pre = exynos_pd_power_forced_off_pre,
		.off_post = exynos_pd_power_off_post,
	}, {
		.name = "pd-isp1",
		.init = exynos_pd_power_init,
		.on_pre = exynos_pd_power_on_pre,
		.on_post = exynos_pd_power_on_post,
		.off_pre = exynos_pd_power_off_pre,
		.forced_off_pre = exynos_pd_power_forced_off_pre,
		.off_post = exynos_pd_power_off_post,
	}, {
		.name = "pd-cam0",
		.init = exynos_pd_power_init,
		.on_pre = exynos_pd_power_on_pre,
		.on_post = exynos_pd_power_on_post,
		.off_pre = exynos_pd_power_off_pre,
		.forced_off_pre = exynos_pd_power_forced_off_pre,
		.off_post = exynos_pd_power_off_post,
	}, {
		.name = "pd-cam1",
		.init = exynos_pd_power_init,
		.on_pre = exynos_pd_power_on_pre,
		.on_post = exynos_pd_power_on_post,
		.off_pre = exynos_pd_power_off_pre,
		.forced_off_pre = exynos_pd_power_forced_off_pre,
		.off_post = exynos_pd_power_off_post,
	},
};

struct exynos_pd_callback *exynos_pd_find_callback(struct exynos_pm_domain *pd)
{
	struct exynos_pd_callback *cb = NULL;
	int i;

	/* find callback function for power domain */
	for (i = 0, cb = &pd_callback_list[0]; i < ARRAY_SIZE(pd_callback_list); i++, cb++) {
		if (strcmp(cb->name, pd->name))
			continue;

		DEBUG_PRINT_INFO("%s: found callback function\n", pd->name);
		break;
	}

	pd->cb = cb;
	return cb;
}
