/*
 * UFS Host Controller driver for Exynos specific extensions
 *
 * Copyright (C) 2014-2015 Samsung Electronics Co., Ltd.
 * Author: Seungwon Jeon  <essuuj@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "ufshcd.h"
#include "unipro.h"

#include "ufs-exynos.h"
#include "ufs-exynos-hw.h"

static int exynos7_ufs_drv_init(struct device *dev, struct exynos_ufs *ufs)
{
	struct clk *child, *parent;

	child = devm_clk_get(dev, "ref_clk");
	if (IS_ERR(child)) {
		dev_err(dev, "failed to get ref_clk clock\n");
		return -EINVAL;
	}

	parent = devm_clk_get(dev, "ref_clk_parent");
	if (IS_ERR(parent)) {
		dev_err(dev, "failed to get ref_clk_parent clock\n");
		return -EINVAL;
	}
	return clk_set_parent(child, parent);
}

static int exynos7_ufs_pre_link(struct exynos_ufs *ufs)
{
	struct ufs_hba *hba = ufs->hba;
	u32 val = ufs->drv_data->uic_attr->pa_dbg_option_suite;
	int i;

	exynos_ufs_enable_ov_tm(hba);
	for_each_ufs_tx_lane(ufs, i)
		ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x297, i), 0x17);
	for_each_ufs_rx_lane(ufs, i) {
		ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x362, i), 0xff);
		ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x363, i), 0x00);
	}
	exynos_ufs_disable_ov_tm(hba);

	ufshcd_dme_set(hba, UIC_ARG_MIB(PA_DBG_OPTION_SUITE_DYN), 0xf);
	ufshcd_dme_set(hba, UIC_ARG_MIB(PA_DBG_OPTION_SUITE_DYN), 0xf);
	for_each_ufs_tx_lane(ufs, i)
		ufshcd_dme_set(hba,
			UIC_ARG_MIB_SEL(TX_HIBERN8_CONTROL, i), 0x0);
	ufshcd_dme_set(hba, UIC_ARG_MIB(PA_DBG_TXPHY_CFGUPDT), 0x1);
	udelay(1);
	ufshcd_dme_set(hba, UIC_ARG_MIB(PA_DBG_OPTION_SUITE), val | (1 << 12));
	ufshcd_dme_set(hba, UIC_ARG_MIB(PA_DBG_SKIP_RESET_PHY), 0x1);
	ufshcd_dme_set(hba, UIC_ARG_MIB(PA_DBG_SKIP_LINE_RESET), 0x1);
	ufshcd_dme_set(hba, UIC_ARG_MIB(PA_DBG_LINE_RESET_REQ), 0x1);
	udelay(1600);
	ufshcd_dme_set(hba, UIC_ARG_MIB(PA_DBG_OPTION_SUITE), val);

	return 0;
}

static int exynos7_ufs_post_link(struct exynos_ufs *ufs)
{
	struct ufs_hba *hba = ufs->hba;
	int i;

	exynos_ufs_enable_ov_tm(hba);
	for_each_ufs_tx_lane(ufs, i) {
		ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x28b, i), 0x83);
		ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x29a, i), 0x07);
		ufshcd_dme_set(hba, UIC_ARG_MIB_SEL(0x277, i),
			TX_LINERESET_N(exynos_ufs_calc_time_cntr(ufs, 200000)));
	}
	exynos_ufs_disable_ov_tm(hba);

	exynos_ufs_enable_dbg_mode(hba);
	ufshcd_dme_set(hba, UIC_ARG_MIB(PA_SAVECONFIGTIME), 0xbb8);
	exynos_ufs_disable_dbg_mode(hba);

	return 0;
}

static int exynos7_ufs_pre_pwr_change(struct exynos_ufs *ufs,
						struct uic_pwr_mode *pwr)
{
	unipro_writel(ufs, 0x22, UNIPRO_DBG_FORCE_DME_CTRL_STATE);

	return 0;
}

static int exynos7_ufs_post_pwr_change(struct exynos_ufs *ufs,
						struct uic_pwr_mode *pwr)
{
	struct ufs_hba *hba = ufs->hba;

	ufshcd_dme_set(hba, UIC_ARG_MIB(PA_DBG_RXPHY_CFGUPDT), 0x1);

	if (pwr->lane == 1) {
		exynos_ufs_enable_dbg_mode(hba);
		ufshcd_dme_set(hba, UIC_ARG_MIB(PA_CONNECTEDTXDATALANES), 0x1);
		exynos_ufs_disable_dbg_mode(hba);
	}

	return 0;
}

struct exynos_ufs_drv_data exynos_ufs_drvs[] = {
{
	.compatible		= "samsung,exynos7-ufs",
	.uic_attr		= &exynos7_uic_attr,
	.quirks			= UFSHCI_QUIRK_BYTE_ALIGN_UTRD |
				  UFSHCI_QUIRK_BROKEN_REQ_LIST_CLR |
				  UFSHCI_QUIRK_BROKEN_HCE |
				  UFSHCI_QUIRK_SKIP_RESET_INTR_AGGR,
	.opts			= EXYNOS_UFS_OPT_HAS_APB_CLK_CTRL |
				  EXYNOS_UFS_OPT_BROKEN_AUTO_CLK_CTRL |
				  EXYNOS_UFS_OPT_BROKEN_RX_SEL_IDX,
	.drv_init		= exynos7_ufs_drv_init,
	.pre_link		= exynos7_ufs_pre_link,
	.post_link		= exynos7_ufs_post_link,
	.pre_pwr_change		= exynos7_ufs_pre_pwr_change,
	.post_pwr_change	= exynos7_ufs_post_pwr_change,
}, {
}, };
