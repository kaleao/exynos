/*
 * KMAX Local Power Control
 *
 * Copyright (C) 2016 ICS FORTH
 *		http://ics.forth.gr
 *
 * Author: Nick Kossifidis <mick@ics.forth.gr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_address.h>

/* Max number of checks on the status register */
#define	MAX_CHECKS	10

/* Offsets of CONFIGURATION registers
 * for the various blocks, on the PMU */
#define AUD_CONFIGURATION	0x4000
#define CAM0_CONFIGURATION	0x4040
#define CAM1_CONFIGURATION	0x4060
#define DISP_CONFIGURATION	0x4080
#define G3D_CONFIGURATION	0x4100
#define ISP0_CONFIGURATION	0x4140
#define ISP1_CONFIGURATION	0x4180
#define MFC_CONFIGURATION	0x41A0
#define MSCL_CONFIGURATION	0x41C0
#define VPP_CONFIGURATION	0x41E0

/* Note: STATUS registers are just after
 * the CONFIGURATION registers (that are 32bit long
 * so it's xxx_STATUS = xxx_CONFIGURATION + 4) */

struct block {
	u32		cfg_reg;
	const char*	name;
};

const static struct block blocks[] __initconst = {
	{AUD_CONFIGURATION, "AUD"},
	{CAM0_CONFIGURATION, "CAM0"},
	{CAM1_CONFIGURATION, "CAM1"},
	{DISP_CONFIGURATION, "DISP"},
	{G3D_CONFIGURATION, "G3D"},
	{ISP0_CONFIGURATION, "ISP0"},
	{ISP1_CONFIGURATION, "ISP1"},
	{MFC_CONFIGURATION, "MFC"},
	{MSCL_CONFIGURATION, "MSCL"},
//	{VPP_CONFIGURATION, "VPP"},
};

static u8
kmax_lpc_read_status(const struct block* blk, void __iomem* base_addr)
{
	void __iomem *addr = base_addr + blk->cfg_reg + 4;
	return readb(addr);
}

static void
kmax_lpc_write_config(const struct block* blk, void __iomem* base_addr, u8 conf)
{
	u32 *addr = base_addr + blk->cfg_reg;
	return writeb(conf & 0x0F, addr);
}

static void
kmax_lpc_power_down_block(const struct block* blk, void __iomem* base_addr)
{
	int i = 0;
	u8 status = 0;

	pr_info("[LPC] Shutting down %s\n",blk->name);

	/* Set LOCAL_PWR_CFG to 0 */
	kmax_lpc_write_config(blk, base_addr, 0);

	for (i = 0; i < MAX_CHECKS; i++) {
		usleep_range(100, 200);
		status = kmax_lpc_read_status(blk, base_addr);
		if (status == 0)
			break;
	}

	if (i >= MAX_CHECKS)
		pr_err("[LPC] timeout reached for %s\n",blk->name);
	else
		pr_info("[LPC] %s is off\n",blk->name);
}

static const struct of_device_id exynos7_pmu_match[] = {
        { .compatible = "samsung,exynos7-pmu" }
};

static int __init
kmax_lpc_init(void)
{
        struct device_node *np;
	void __iomem *base_addr = NULL;
	int i = 0;

        np = of_find_matching_node(NULL, exynos7_pmu_match);
        if (np)
               	base_addr = of_iomap(np, 0);

	if(!base_addr) {
		pr_err("[LPC] Unable to map PMU memory region\n");
		return -EIO;
	}

	for (i = 0; i < ARRAY_SIZE(blocks); i++)
		kmax_lpc_power_down_block(&blocks[i], base_addr);

	iounmap(base_addr);
	return 0;
}

/* Register an arch initcall (before subsys) */
arch_initcall(kmax_lpc_init);
