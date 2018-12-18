/*
 * SAMSUNG EXYNOS5430 Flattened Device Tree enabled machine
 *
 * Copyright (c) 2012 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <linux/of_platform.h>
#include <linux/of_fdt.h>
#include <linux/memblock.h>
#include <linux/io.h>
#include <linux/exynos_ion.h>

#include <asm/mach/arch.h>
#include <mach/regs-pmu.h>

#include <plat/cpu.h>

#include "common.h"

const struct of_device_id of_iommu_bus_match_table[] = {
	{ .compatible = "samsung,exynos-iommu-bus", },
	{} /* Empty terminated list */
};

static void __init exsom7420_dt_map_io(void)
{
	exynos_init_io(NULL, 0);
}

static void __init exsom7420_dt_machine_init(void)
{
	of_platform_bus_probe(NULL, of_iommu_bus_match_table, NULL);
	exynos_pmu_init();
}

static char const *exsom7420_dt_compat[] __initdata = {
	"samsung,exynos7420",
	NULL
};

DT_MACHINE_START(EXSOM7420, "SAMSUNG Exynos7420")
	.map_io		= exsom7420_dt_map_io,
	.init_machine	= exsom7420_dt_machine_init,
	.dt_compat	= exsom7420_dt_compat,
	.restart        = exynos_restart,
MACHINE_END
