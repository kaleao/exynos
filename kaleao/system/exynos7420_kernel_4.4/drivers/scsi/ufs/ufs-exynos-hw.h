/*
 * UFS Host Controller driver for Exynos specific extensions
 *
 * Copyright (C) 2014-2015 Samsung Electronics Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef _UFS_EXYNOS_HW_H_
#define _UFS_EXYNOS_HW_H_

#include "ufs-exynos.h"
#include "unipro.h"

struct exynos_ufs_uic_attr exynos7_uic_attr = {
	.tx_trailingclks		= 0x10,
	.tx_dif_p_nsec			= 3000000,	/* unit: ns */
	.tx_dif_n_nsec			= 1000000,	/* unit: ns */
	.tx_high_z_cnt_nsec		= 20000,	/* unit: ns */
	.tx_base_unit_nsec		= 100000,	/* unit: ns */
	.tx_gran_unit_nsec		= 4000,		/* unit: ns */
	.tx_sleep_cnt			= 1000,		/* unit: ns */
	.tx_min_activatetime		= 0xa,
	.rx_filler_enable		= 0x2,
	.rx_dif_p_nsec			= 1000000,	/* unit: ns */
	.rx_hibern8_wait_nsec		= 4000000,	/* unit: ns */
	.rx_base_unit_nsec		= 100000,	/* unit: ns */
	.rx_gran_unit_nsec		= 4000,		/* unit: ns */
	.rx_sleep_cnt			= 1280,		/* unit: ns */
	.rx_stall_cnt			= 320,		/* unit: ns */
	.rx_hs_g1_sync_len_cap		= SYNC_LEN_COARSE(0xf),
	.rx_hs_g2_sync_len_cap		= SYNC_LEN_COARSE(0xf),
	.rx_hs_g3_sync_len_cap		= SYNC_LEN_COARSE(0xf),
	.rx_hs_g1_prep_sync_len_cap	= PREP_LEN(0xf),
	.rx_hs_g2_prep_sync_len_cap	= PREP_LEN(0xf),
	.rx_hs_g3_prep_sync_len_cap	= PREP_LEN(0xf),
	.pa_dbg_option_suite		= 0x30103,
};

#endif /* _UFS_EXYNOS_HW_H_ */
