/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com/
 *
 * EXYNOS Power mode
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/of.h>
#include <linux/device.h>
#include <linux/tick.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/stat.h>
#include <linux/exynos-ss.h>

#include <asm/smp_plat.h>
#include <asm/cputype.h>

#include <mach/pmu.h>
#include <mach/exynos-pm.h>
#include <mach/exynos-powermode.h>
#include <mach/regs-pmu.h>
#include <mach/regs-clock.h>
#include <mach/pmu_cal_sys.h>

#include <sound/exynos.h>

/* Sysreg base EMA control SFR save/restore list */
#if defined(CONFIG_SOC_EXYNOS7420)
#define APOLLO_EMA_CON		(EXYNOS7420_VA_SYSREG + 0x0038)
#define ATLAS_EMA_CON		(EXYNOS7420_VA_SYSREG + 0x0138)
#define CPU_EMA_REG1		(EXYNOS7420_VA_SYSREG + 0x2908)
#define EMA_RF2_HS_CON		(EXYNOS7420_VA_SYSREG + 0x2718)
#define EMA_RF2_HS_CON_INT	(EXYNOS7420_VA_SYSREG + 0x2758)
#define IMEM_EMA_VROMP_HD_CON	(EXYNOS7420_VA_SYSREG + 0x1424)
#endif

/***************************************************************************
 *                         Functions for low power                         *
 ***************************************************************************/
#ifdef CONFIG_CPU_IDLE_EXYNOS
static void exynos_idle_clock_down(bool on, enum cluster_type cluster)
{
	void __iomem *reg_pwr_ctrl;
	unsigned int tmp;

#if defined(CONFIG_SOC_EXYNOS5433)
	reg_pwr_ctrl = (cluster == LITTLE) ? EXYNOS5430_KFC_PWR_CTRL :
					     EXYNOS5430_EGL_PWR_CTRL;
#elif defined(CONFIG_SOC_EXYNOS7420)
	reg_pwr_ctrl = (cluster == LITTLE) ? EXYNOS7420_APOLLO_PWR_CTRL :
					     EXYNOS7420_ATLAS_PWR_CTRL;
#endif

	tmp = on ? USE_L2QACTIVE : 0;
	__raw_writel(tmp, reg_pwr_ctrl);
}
#else
static void exynos_idle_clock_down(bool on, enum cluster_type cluster) { };
#endif

/***************************************************************************
 *                           Cluster power down                            *
 ***************************************************************************/

/*
 * CPD - Cluster Power Down
 *
 * If cpu has enough idle time which is longer than CPD target_residency,
 * set cpd_state_mask. All cpus in a cluster are set cpd_state_mask, cluster
 * can be powered off. When update cpd_state_mask, coherency must be kept
 * because each cpu update the mask asynchronously. In this driver, use the
 * spinlock, cpd_lock.
 */
static struct cpumask cpd_state_mask;
static void init_cpd_state_mask(void)
{
	cpumask_clear(&cpd_state_mask);
}

static void update_cpd_state(bool down, unsigned int cpu)
{
	if (down)
		cpumask_set_cpu(cpu, &cpd_state_mask);
	else
		cpumask_clear_cpu(cpu, &cpd_state_mask);
}

static s64 get_next_event_time_us(unsigned int cpu)
{
	struct clock_event_device *dev = per_cpu(tick_cpu_device, cpu).evtdev;

	return ktime_to_us(ktime_sub(dev->next_event, ktime_get()));
}

static int is_cpus_busy(unsigned int target_residency,
				const struct cpumask *mask)
{
	int cpu;

	/*
	 * "Busy" means that even one cpu in mask has the smaller idle
	 * time than target_residency.
	 */
	for_each_cpu_and(cpu, cpu_possible_mask, mask) {
		if (!cpumask_test_cpu(cpu, &cpd_state_mask))
			return -EBUSY;

		/*
		 * Compare cpu's next event time and target_residency.
		 * Next event time means idle time.
		 */
		if (get_next_event_time_us(cpu) < target_residency)
			return -EBUSY;
	}

	return 0;
}

static int get_cluster_id(unsigned int cpu)
{
	return MPIDR_AFFINITY_LEVEL(cpu_logical_map(cpu), 1);
}

/*
 * If AP put into LPC, console cannot work normally. For development,
 * support sysfs to enable or disable LPC. Refer below :
 *
 * echo 0/1 > /sys/power/lpc (0:disable, 1:enable)
 */
static int lpc_enabled = 1;

static ssize_t show_lpc_enabled(struct kobject *kobj,
				struct kobj_attribute *attr, char *buf)
{
	return snprintf(buf, 3, "%d\n", lpc_enabled);
}

static ssize_t store_lpc_enabled(struct kobject *kobj, struct kobj_attribute *attr,
					const char *buf, size_t count)
{
	int input;

	if (!sscanf(buf, "%1d", &input))
		return -EINVAL;

	lpc_enabled = !!input;

	return count;
}

static struct kobj_attribute lpc_attribute =
	__ATTR(lpc, S_IRUGO | S_IWUSR, show_lpc_enabled, store_lpc_enabled);

#define REG_LPC_STATE_ADDR	(S5P_VA_SYSRAM_NS + 0x50)
#define ENTER_LPC		(0x1)

static inline void write_lpc_flag(int value)
{
	__raw_writel(value, REG_LPC_STATE_ADDR);
}

struct check_reg check_reg_lpc[LIST_MAX_LENGTH];

static int is_lpc_available(unsigned int target_residency)
{
	if (!lpc_enabled)
		return false;

	if (is_cpus_busy(target_residency, cpu_possible_mask))
		return false;

	if (check_reg_status(check_reg_lpc, ARRAY_SIZE(check_reg_lpc)))
		return false;

	if (exynos_lpc_prepare())
		return false;

	if (is_dll_on())
		return false;

	if (exynos_check_aud_pwr() > AUD_PWR_LPC)
		return false;

	if (pwm_check_enable_cnt())
		return false;

	return true;
}

static DEFINE_SPINLOCK(cpd_lock);

int determine_cpd(int index, int c2_index, unsigned int cpu,
				unsigned int target_residency)
{
	exynos_cpu.power_down(cpu);

	/*
	 * CPU enters low power state with index of C2, it just indicates local
	 * cpu power gating. In case of index of CPD, this state implements similiar
	 * execution as C2, furthermore, it also indicates that cluster can be
	 * powered down. If all cpus in a cluster enter low power state with index
	 * of CPD and idle time is longer than target residency, cluster can be
	 * powered down using cpu_sequencer.
	 *
	 * This function determines whether to power down the cluster or not.
	 * If index is C2, skip this routine.
	 */

	if (index == c2_index)
		goto skip_determine;

	spin_lock(&cpd_lock);
	update_cpd_state(true, cpu);

	if (is_lpc_available(target_residency)) {
#ifdef CONFIG_SOC_EXYNOS7420
		write_lpc_flag(ENTER_LPC);
#endif
		s3c24xx_serial_fifo_wait();
	}

	/*
	 * Power down of LITTLE cluster have nothing to gain power consumption,
	 * so does not support. For you reference, cluster id "1" indicates LITTLE.
	 */
	if (get_cluster_id(cpu)) {
		index = c2_index;
		goto unlock;
	}

	/*
	 * If cluster is busy, return index with IDLE_C2.
	 * Otherwise, enable cpu sequcner to shutdown cluster.
	 */
	if (is_cpus_busy(target_residency, cpu_coregroup_mask(cpu)))
		index = c2_index;
	else
		exynos_cpu_sequencer_ctrl(true);

unlock:
	spin_unlock(&cpd_lock);

skip_determine:
	return index;
}

void wakeup_from_c2(unsigned int cpu)
{
	exynos_cpu.power_up(cpu);

	update_cpd_state(false, cpu);

	exynos_cpu_sequencer_ctrl(false);
}


/***************************************************************************
 *                              Low power mode                             *
 ***************************************************************************/
static void exynos_ctrl_alpa(bool enter)
{
	unsigned int tmp;

	tmp = __raw_readl(EXYNOS_PMU_PMU_SYNC_CTRL);
	tmp = (enter ? tmp | ENABLE_ASYNC_PMU : tmp & ~ENABLE_ASYNC_PMU);
	__raw_writel(tmp, EXYNOS_PMU_PMU_SYNC_CTRL);

	tmp = __raw_readl(EXYNOS_PMU_CENTRAL_SEQ_MIF_OPTION);
	tmp = (enter ? tmp | USE_AUD_NOT_ACCESS_MIF : tmp & ~USE_AUD_NOT_ACCESS_MIF);
	__raw_writel(tmp, EXYNOS_PMU_CENTRAL_SEQ_MIF_OPTION);

	tmp = __raw_readl(EXYNOS_PMU_WAKEUP_MASK_MIF);
	tmp = (enter ? tmp & ~WAKEUP_MASK_AUD : tmp | WAKEUP_MASK_AUD);
	__raw_writel(tmp, EXYNOS_PMU_WAKEUP_MASK_MIF);
}

/* The setting clock list to enter system power mode */
static struct sfr_save save_clk_regs[] = {
#if defined(CONFIG_SOC_EXYNOS5433)
	SFR_SAVE(EXYNOS5430_ISP_PLL_CON0),
	SFR_SAVE(EXYNOS5430_ISP_PLL_CON1),
	SFR_SAVE(EXYNOS5430_AUD_PLL_CON0),
	SFR_SAVE(EXYNOS5430_AUD_PLL_CON1),
	SFR_SAVE(EXYNOS5430_AUD_PLL_CON2),
	SFR_SAVE(EXYNOS5430_MEM0_PLL_CON0),
	SFR_SAVE(EXYNOS5430_MEM0_PLL_CON1),
	SFR_SAVE(EXYNOS5430_MEM1_PLL_CON0),
	SFR_SAVE(EXYNOS5430_MEM1_PLL_CON1),
	SFR_SAVE(EXYNOS5430_BUS_PLL_CON0),
	SFR_SAVE(EXYNOS5430_BUS_PLL_CON1),
	SFR_SAVE(EXYNOS5430_MFC_PLL_CON0),
	SFR_SAVE(EXYNOS5430_MFC_PLL_CON1),

	SFR_SAVE(EXYNOS5430_SRC_SEL_EGL0),
	SFR_SAVE(EXYNOS5430_SRC_SEL_EGL1),
	SFR_SAVE(EXYNOS5430_SRC_SEL_EGL2),
	SFR_SAVE(EXYNOS5430_SRC_SEL_KFC0),
	SFR_SAVE(EXYNOS5430_SRC_SEL_KFC1),
	SFR_SAVE(EXYNOS5430_SRC_SEL_KFC2),
	SFR_SAVE(EXYNOS5430_SRC_SEL_TOP0),
	SFR_SAVE(EXYNOS5430_SRC_SEL_TOP1),
	SFR_SAVE(EXYNOS5430_SRC_SEL_TOP2),
	SFR_SAVE(EXYNOS5430_SRC_SEL_TOP3),
	SFR_SAVE(EXYNOS5430_SRC_SEL_TOP_MSCL),
	SFR_SAVE(EXYNOS5430_SRC_SEL_TOP_CAM1),
	SFR_SAVE(EXYNOS5430_SRC_SEL_TOP_DISP),
	SFR_SAVE(EXYNOS5430_SRC_SEL_TOP_FSYS0),
	SFR_SAVE(EXYNOS5430_SRC_SEL_TOP_FSYS1),
	SFR_SAVE(EXYNOS5430_SRC_SEL_TOP_PERIC0),
	SFR_SAVE(EXYNOS5430_SRC_SEL_TOP_PERIC1),
	SFR_SAVE(EXYNOS5430_SRC_SEL_MIF0),
	SFR_SAVE(EXYNOS5430_SRC_SEL_MIF1),
	SFR_SAVE(EXYNOS5430_SRC_SEL_MIF2),
	SFR_SAVE(EXYNOS5430_SRC_SEL_MIF3),
	SFR_SAVE(EXYNOS5430_SRC_SEL_MIF4),
	SFR_SAVE(EXYNOS5430_SRC_SEL_MIF5),

	SFR_SAVE(EXYNOS5430_DIV_EGL0),
	SFR_SAVE(EXYNOS5430_DIV_EGL1),
	SFR_SAVE(EXYNOS5430_DIV_KFC0),
	SFR_SAVE(EXYNOS5430_DIV_KFC1),
	SFR_SAVE(EXYNOS5430_DIV_TOP0),
	SFR_SAVE(EXYNOS5430_DIV_TOP1),
	SFR_SAVE(EXYNOS5430_DIV_TOP2),
	SFR_SAVE(EXYNOS5430_DIV_TOP3),
	SFR_SAVE(EXYNOS5430_DIV_TOP_MSCL),
	SFR_SAVE(EXYNOS5430_DIV_TOP_CAM10),
	SFR_SAVE(EXYNOS5430_DIV_TOP_CAM11),
	SFR_SAVE(EXYNOS5430_DIV_TOP_FSYS0),
	SFR_SAVE(EXYNOS5430_DIV_TOP_FSYS1),
	SFR_SAVE(EXYNOS5430_DIV_TOP_FSYS2),
	SFR_SAVE(EXYNOS5430_DIV_TOP_PERIC0),
	SFR_SAVE(EXYNOS5430_DIV_TOP_PERIC1),
	SFR_SAVE(EXYNOS5430_DIV_TOP_PERIC2),
	SFR_SAVE(EXYNOS5430_DIV_TOP_PERIC3),
	SFR_SAVE(EXYNOS5430_DIV_MIF1),
	SFR_SAVE(EXYNOS5430_DIV_MIF2),
	SFR_SAVE(EXYNOS5430_DIV_MIF3),
	SFR_SAVE(EXYNOS5430_DIV_MIF4),
	SFR_SAVE(EXYNOS5430_DIV_MIF5),
	SFR_SAVE(EXYNOS5430_DIV_BUS1),
	SFR_SAVE(EXYNOS5430_DIV_BUS2),

	SFR_SAVE(EXYNOS5430_SRC_ENABLE_TOP0),
	SFR_SAVE(EXYNOS5430_SRC_ENABLE_TOP2),
	SFR_SAVE(EXYNOS5430_SRC_ENABLE_TOP3),
	SFR_SAVE(EXYNOS5430_SRC_ENABLE_TOP4),
	SFR_SAVE(EXYNOS5430_SRC_ENABLE_TOP_MSCL),
	SFR_SAVE(EXYNOS5430_SRC_ENABLE_TOP_CAM1),
	SFR_SAVE(EXYNOS5430_SRC_ENABLE_TOP_DISP),

	SFR_SAVE(EXYNOS5430_ENABLE_IP_EGL1),
	SFR_SAVE(EXYNOS5430_ENABLE_IP_KFC1),
	SFR_SAVE(EXYNOS5430_ENABLE_IP_TOP),
	SFR_SAVE(EXYNOS5430_ENABLE_IP_FSYS0),
	SFR_SAVE(EXYNOS5430_ENABLE_IP_PERIC0),
	SFR_SAVE(EXYNOS5430_ENABLE_IP_MIF1),
	SFR_SAVE(EXYNOS5430_ENABLE_IP_CPIF0),
#elif defined(CONFIG_SOC_EXYNOS7420)
	SFR_SAVE(EXYNOS7420_MUX_ENABLE_TOPC0),
	SFR_SAVE(EXYNOS7420_MUX_ENABLE_TOPC1),
	SFR_SAVE(EXYNOS7420_MUX_ENABLE_TOPC2),
	SFR_SAVE(EXYNOS7420_MUX_ENABLE_TOPC3),
	SFR_SAVE(EXYNOS7420_MUX_ENABLE_TOPC4),
	SFR_SAVE(EXYNOS7420_MUX_ENABLE_TOPC5),
	SFR_SAVE(EXYNOS7420_ENABLE_ACLK_TOPC0),
	SFR_SAVE(EXYNOS7420_ENABLE_ACLK_TOPC1),
	SFR_SAVE(EXYNOS7420_ENABLE_ACLK_TOPC2),
	SFR_SAVE(EXYNOS7420_ENABLE_SCLK_TOPC0),
	SFR_SAVE(EXYNOS7420_ENABLE_SCLK_TOPC1),
	SFR_SAVE(EXYNOS7420_MUX_ENABLE_TOP00),
	SFR_SAVE(EXYNOS7420_MUX_ENABLE_TOP03),
	SFR_SAVE(EXYNOS7420_MUX_ENABLE_TOP04),
	SFR_SAVE(EXYNOS7420_MUX_ENABLE_TOP05),
	SFR_SAVE(EXYNOS7420_MUX_ENABLE_TOP06),
	SFR_SAVE(EXYNOS7420_MUX_ENABLE_TOP07),
	SFR_SAVE(EXYNOS7420_MUX_ENABLE_TOP0_DISP),
	SFR_SAVE(EXYNOS7420_MUX_ENABLE_TOP0_CAM10),
	SFR_SAVE(EXYNOS7420_MUX_ENABLE_TOP0_CAM11),
	SFR_SAVE(EXYNOS7420_MUX_ENABLE_TOP0_PERIC0),
	SFR_SAVE(EXYNOS7420_MUX_ENABLE_TOP0_PERIC1),
	SFR_SAVE(EXYNOS7420_MUX_ENABLE_TOP0_PERIC2),
	SFR_SAVE(EXYNOS7420_MUX_ENABLE_TOP0_PERIC3),
	SFR_SAVE(EXYNOS7420_ENABLE_ACLK_TOP02),
	SFR_SAVE(EXYNOS7420_ENABLE_ACLK_TOP03),
	SFR_SAVE(EXYNOS7420_ENABLE_ACLK_TOP04),
	SFR_SAVE(EXYNOS7420_ENABLE_ACLK_TOP05),
	SFR_SAVE(EXYNOS7420_ENABLE_ACLK_TOP06),
	SFR_SAVE(EXYNOS7420_ENABLE_ACLK_TOP07),
	SFR_SAVE(EXYNOS7420_ENABLE_SCLK_TOP0_DISP),
	SFR_SAVE(EXYNOS7420_ENABLE_SCLK_TOP0_CAM10),
	SFR_SAVE(EXYNOS7420_ENABLE_SCLK_TOP0_CAM11),
	SFR_SAVE(EXYNOS7420_ENABLE_SCLK_TOP0_PERIC0),
	SFR_SAVE(EXYNOS7420_ENABLE_SCLK_TOP0_PERIC1),
	SFR_SAVE(EXYNOS7420_ENABLE_SCLK_TOP0_PERIC2),
	SFR_SAVE(EXYNOS7420_ENABLE_SCLK_TOP0_PERIC3),
	SFR_SAVE(EXYNOS7420_MUX_ENABLE_TOP10),
	SFR_SAVE(EXYNOS7420_MUX_ENABLE_TOP13),
	SFR_SAVE(EXYNOS7420_MUX_ENABLE_TOP1_FSYS0),
	SFR_SAVE(EXYNOS7420_MUX_ENABLE_TOP1_FSYS1),
	SFR_SAVE(EXYNOS7420_MUX_ENABLE_TOP1_FSYS11),
	SFR_SAVE(EXYNOS7420_ENABLE_ACLK_TOP12),
	SFR_SAVE(EXYNOS7420_ENABLE_ACLK_TOP13),
	SFR_SAVE(EXYNOS7420_ENABLE_SCLK_TOP1_FSYS0),
	SFR_SAVE(EXYNOS7420_ENABLE_SCLK_TOP1_FSYS1),
	SFR_SAVE(EXYNOS7420_ENABLE_SCLK_TOP1_FSYS11),
	SFR_SAVE(EXYNOS7420_MUX_SEL_TOP00),
	SFR_SAVE(EXYNOS7420_MUX_SEL_TOP0_DISP),
	SFR_SAVE(EXYNOS7420_MUX_SEL_TOP0_PERIC0),

	SFR_SAVE(EXYNOS7420_MUX_ENABLE_MIF0),
	SFR_SAVE(EXYNOS7420_MUX_ENABLE_MIF1),
	SFR_SAVE(EXYNOS7420_MUX_ENABLE_MIF2),
	SFR_SAVE(EXYNOS7420_MUX_ENABLE_MIF3),
	SFR_SAVE(EXYNOS7420_ENABLE_ACLK_CCORE0),
	SFR_SAVE(EXYNOS7420_ENABLE_ACLK_CCORE1),
	SFR_SAVE(EXYNOS7420_MUX_ENABLE_IMEM),
	SFR_SAVE(EXYNOS7420_ENABLE_ACLK_IMEM0),
	SFR_SAVE(EXYNOS7420_ENABLE_ACLK_IMEM1),
	SFR_SAVE(EXYNOS7420_ENABLE_PCLK_IMEM),

	SFR_SAVE(EXYNOS7420_MUX_ENABLE_PERIC0),
	SFR_SAVE(EXYNOS7420_ENABLE_SCLK_PERIC0),
	SFR_SAVE(EXYNOS7420_MUX_ENABLE_PERIC11),
	SFR_SAVE(EXYNOS7420_ENABLE_SCLK_PERIC10),
	SFR_SAVE(EXYNOS7420_ENABLE_ACLK_BUS0),
	SFR_SAVE(EXYNOS7420_MUX_ENABLE_BUS1),
	SFR_SAVE(EXYNOS7420_ENABLE_ACLK_BUS1),

	SFR_SAVE(EXYNOS7420_MUX_ENABLE_FSYS00),
	SFR_SAVE(EXYNOS7420_MUX_ENABLE_FSYS01),
	SFR_SAVE(EXYNOS7420_ENABLE_ACLK_FSYS00),
	SFR_SAVE(EXYNOS7420_ENABLE_ACLK_FSYS01),
	SFR_SAVE(EXYNOS7420_ENABLE_SCLK_FSYS01),
	SFR_SAVE(EXYNOS7420_ENABLE_SCLK_FSYS04),
	SFR_SAVE(EXYNOS7420_MUX_ENABLE_FSYS11),
	SFR_SAVE(EXYNOS7420_ENABLE_ACLK_FSYS1),

	SFR_SAVE(APOLLO_EMA_CON),
	SFR_SAVE(ATLAS_EMA_CON),
	SFR_SAVE(CPU_EMA_REG1),
	SFR_SAVE(EMA_RF2_HS_CON),
	SFR_SAVE(EMA_RF2_HS_CON_INT),
	SFR_SAVE(IMEM_EMA_VROMP_HD_CON),
#endif
};

static struct sfr_bit_ctrl set_clk_regs[] = {
#if defined(CONFIG_SOC_EXYNOS5433)
	SFR_CTRL(EXYNOS5430_ENABLE_IP_FSYS0,		0xffffffff, 0x00007dfb),
	SFR_CTRL(EXYNOS5430_ENABLE_IP_PERIC0,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS5430_SRC_SEL_TOP_PERIC1,		0xffffffff, 0x00000011),
	SFR_CTRL(EXYNOS5430_ENABLE_IP_EGL1,		0xffffffff, 0x00000fff),
	SFR_CTRL(EXYNOS5430_ENABLE_IP_KFC1,		0xffffffff, 0x00000fff),
	SFR_CTRL(EXYNOS5430_ENABLE_IP_MIF1,		0xffffffff, 0x01ffffff),
	SFR_CTRL(EXYNOS5430_ENABLE_IP_CPIF0,		0xffffffff, 0x000FF000),
#elif defined(CONFIG_SOC_EXYNOS7420)
	SFR_CTRL(EXYNOS7420_MUX_ENABLE_TOPC0,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_MUX_ENABLE_TOPC1,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_MUX_ENABLE_TOPC2,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_MUX_ENABLE_TOPC3,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_MUX_ENABLE_TOPC4,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_MUX_ENABLE_TOPC5,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_ENABLE_ACLK_TOPC0,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_ENABLE_ACLK_TOPC1,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_ENABLE_ACLK_TOPC2,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_ENABLE_SCLK_TOPC0,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_ENABLE_SCLK_TOPC1,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_MUX_ENABLE_TOP00,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_MUX_ENABLE_TOP03,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_MUX_ENABLE_TOP04,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_MUX_ENABLE_TOP05,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_MUX_ENABLE_TOP06,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_MUX_ENABLE_TOP07,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_MUX_ENABLE_TOP0_DISP,	0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_MUX_ENABLE_TOP0_CAM10,	0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_MUX_ENABLE_TOP0_CAM11,	0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_MUX_ENABLE_TOP0_PERIC0,	0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_MUX_ENABLE_TOP0_PERIC1,	0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_MUX_ENABLE_TOP0_PERIC2,	0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_MUX_ENABLE_TOP0_PERIC3,	0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_ENABLE_ACLK_TOP02,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_ENABLE_ACLK_TOP03,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_ENABLE_ACLK_TOP04,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_ENABLE_ACLK_TOP05,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_ENABLE_ACLK_TOP06,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_ENABLE_ACLK_TOP07,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_ENABLE_SCLK_TOP0_DISP,	0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_ENABLE_SCLK_TOP0_CAM10,	0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_ENABLE_SCLK_TOP0_CAM11,	0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_ENABLE_SCLK_TOP0_PERIC0,	0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_ENABLE_SCLK_TOP0_PERIC1,	0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_ENABLE_SCLK_TOP0_PERIC2,	0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_ENABLE_SCLK_TOP0_PERIC3,	0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_MUX_ENABLE_TOP10,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_MUX_ENABLE_TOP13,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_MUX_ENABLE_TOP1_FSYS0,	0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_MUX_ENABLE_TOP1_FSYS1,	0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_MUX_ENABLE_TOP1_FSYS11,	0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_ENABLE_ACLK_TOP12,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_ENABLE_ACLK_TOP13,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_ENABLE_SCLK_TOP1_FSYS0,	0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_ENABLE_SCLK_TOP1_FSYS1,	0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_ENABLE_SCLK_TOP1_FSYS11,	0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_MUX_SEL_TOP00,		0x00000001, 0x00000000),
	SFR_CTRL(EXYNOS7420_MUX_SEL_TOP0_DISP,		0x00007000, 0x00003000),
	SFR_CTRL(EXYNOS7420_MUX_SEL_TOP0_PERIC0,	0x00300370, 0x00100130),

	SFR_CTRL(EXYNOS7420_MUX_ENABLE_MIF0,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_MUX_ENABLE_MIF1,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_MUX_ENABLE_MIF2,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_MUX_ENABLE_MIF3,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_ENABLE_ACLK_CCORE0,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_ENABLE_ACLK_CCORE1,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_MUX_ENABLE_IMEM,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_ENABLE_ACLK_IMEM0,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_ENABLE_ACLK_IMEM1,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_ENABLE_PCLK_IMEM,		0xffffffff, 0xffffffff),

	SFR_CTRL(EXYNOS7420_MUX_ENABLE_PERIC0,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_ENABLE_SCLK_PERIC0,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_MUX_ENABLE_PERIC11,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_ENABLE_SCLK_PERIC10,	0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_ENABLE_ACLK_BUS0,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_MUX_ENABLE_BUS1,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_ENABLE_ACLK_BUS1,		0xffffffff, 0xffffffff),

	SFR_CTRL(EXYNOS7420_MUX_ENABLE_FSYS00,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_MUX_ENABLE_FSYS01,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_ENABLE_ACLK_FSYS00,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_ENABLE_ACLK_FSYS01,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_ENABLE_SCLK_FSYS01,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_ENABLE_SCLK_FSYS04,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_MUX_ENABLE_FSYS11,		0xffffffff, 0xffffffff),
	SFR_CTRL(EXYNOS7420_ENABLE_ACLK_FSYS1,		0xffffffff, 0xffffffff),
#endif
};

/* The setting clock list to enter LPA mode */
static struct sfr_save save_clk_regs_lpa[] = {
#if defined(CONFIG_SOC_EXYNOS5433)
	SFR_SAVE(EXYNOS5430_ENABLE_ACLK_MIF3),
	SFR_SAVE(EXYNOS5430_ENABLE_ACLK_TOP),
	SFR_SAVE(EXYNOS5430_ENABLE_SCLK_FSYS),
	SFR_SAVE(EXYNOS5430_SRC_SEL_BUS2),
	SFR_SAVE(EXYNOS5430_SRC_SEL_FSYS0),
#elif defined(CONFIG_SOC_EXYNOS7420)
	SFR_SAVE(EXYNOS7420_ENABLE_SCLK_FSYS11),
#endif
};

static struct sfr_bit_ctrl set_clk_regs_lpa[] = {
#if defined(CONFIG_SOC_EXYNOS5433)
	SFR_CTRL(EXYNOS5430_MPHY_PLL_CON0,		0x80000000, 0x80000000),
	SFR_CTRL(EXYNOS5430_SRC_SEL_FSYS0,		0x00000001, 0x00000000),
	SFR_CTRL(EXYNOS5430_SRC_SEL_BUS2,		0x00000001, 0x00000000),
	SFR_CTRL(EXYNOS5430_ENABLE_SCLK_FSYS,		0xffffffff, 0x00000000),
	SFR_CTRL(EXYNOS5430_ENABLE_ACLK_TOP,		0x00040000, 0x00000000),
	SFR_CTRL(EXYNOS5430_ENABLE_ACLK_MIF3,		0x00001000, 0x00000000),
#elif defined(CONFIG_SOC_EXYNOS7420)
	SFR_CTRL(EXYNOS7420_ENABLE_SCLK_FSYS11,		0xffffffff, 0xfffeffff),
#endif
};

/* The setting clock list to enter sleep mode */
static struct sfr_save save_clk_regs_sleep[] = {
#if defined(CONFIG_SOC_EXYNOS7420)
	SFR_SAVE(EXYNOS7420_ENABLE_SCLK_FSYS11),
#endif
};

static struct sfr_bit_ctrl set_clk_regs_sleep[] = {
#if defined(CONFIG_SOC_EXYNOS7420)
	SFR_CTRL(EXYNOS7420_ENABLE_SCLK_FSYS11,		0xffffffff, 0xffffffff),
#endif
};

static void exynos_sys_powerdown_set_clk(enum sys_powerdown mode)
{
	switch (mode) {
	case SYS_AFTR:
		break;
	case SYS_ALPA:
	case SYS_LPA:
		exynos_save_sfr(save_clk_regs_lpa, ARRAY_SIZE(save_clk_regs_lpa));
		exynos_set_sfr(set_clk_regs_lpa, ARRAY_SIZE(set_clk_regs_lpa));

		exynos_save_sfr(save_clk_regs, ARRAY_SIZE(save_clk_regs));
		exynos_set_sfr(set_clk_regs, ARRAY_SIZE(set_clk_regs));
		break;
	case SYS_SLEEP:
		exynos_save_sfr(save_clk_regs_sleep, ARRAY_SIZE(save_clk_regs_sleep));
		exynos_set_sfr(set_clk_regs_sleep, ARRAY_SIZE(set_clk_regs_sleep));

		exynos_save_sfr(save_clk_regs, ARRAY_SIZE(save_clk_regs));
		exynos_set_sfr(set_clk_regs, ARRAY_SIZE(set_clk_regs));
		break;
	default:
		break;
	}
}

static void exynos_sys_powerdown_restore_clk(enum sys_powerdown mode)
{
	switch (mode) {
	case SYS_AFTR:
		break;
	case SYS_ALPA:
	case SYS_LPA:
		exynos_restore_sfr(save_clk_regs_lpa, ARRAY_SIZE(save_clk_regs_lpa));
		exynos_restore_sfr(save_clk_regs, ARRAY_SIZE(save_clk_regs));
		break;
	case SYS_SLEEP:
		exynos_restore_sfr(save_clk_regs_sleep, ARRAY_SIZE(save_clk_regs_sleep));
		exynos_restore_sfr(save_clk_regs, ARRAY_SIZE(save_clk_regs));
		break;
	default:
		break;
	}
}

void exynos_prepare_sys_powerdown(enum sys_powerdown mode)
{
	exynos_set_wakeupmask(mode);
	exynos_pmu_cal_sys_prepare(mode);

	exynos_idle_clock_down(false, LITTLE);
	exynos_idle_clock_down(false, BIG);

	switch (mode) {
	case SYS_AFTR:
		break;
	case SYS_ALPA:
		exynos_ctrl_alpa(true);
	case SYS_LPA:
		exynos_lpa_enter();
		break;
	case SYS_SLEEP:
		break;
	default:
		break;
	}

	exynos_sys_powerdown_set_clk(mode);
}

void exynos_wakeup_sys_powerdown(enum sys_powerdown mode, bool early_wakeup)
{

	if (early_wakeup)
		exynos_pmu_cal_sys_earlywake(mode);
	else
		exynos_pmu_cal_sys_post(mode);

	exynos_clear_wakeupmask();

	exynos_idle_clock_down(true, LITTLE);
	exynos_idle_clock_down(true, BIG);

	exynos_sys_powerdown_restore_clk(mode);

	switch (mode) {
	case SYS_AFTR:
		break;
	case SYS_ALPA:
		exynos_ctrl_alpa(false);
	case SYS_LPA:
		exynos_lpa_exit();
		break;
	case SYS_SLEEP:
#if defined(CONFIG_SOC_EXYNOS7420)
		/* Set Apollo SRAM control sysreg to Dual rail mode*/
		__raw_writel(0x0, EXYNOS7420_VA_SYSREG + 0x0054);
#endif
		break;
	default:
		break;
	}
}

bool exynos_sys_powerdown_enabled(void)
{
	/*
	 * When system enters low power mode,
	 * this bit changes automatically to high
	 */
	return !((__raw_readl(EXYNOS_PMU_CENTRAL_SEQ_CONFIGURATION) &
			CENTRALSEQ_PWR_CFG));
}

struct check_reg check_reg_lpm[LIST_MAX_LENGTH];

/*
 * To determine which power mode system enter, check clock or power
 * registers and other devices by notifier.
 */
int determine_lpm(void)
{
	if (exynos_lpa_prepare())
		return SYS_AFTR;

	if (check_reg_status(check_reg_lpm, ARRAY_SIZE(check_reg_lpm)))
		return SYS_AFTR;

#ifdef CONFIG_SND_SAMSUNG_AUDSS
	if (exynos_check_aud_pwr() == AUD_PWR_ALPA)
#if defined(CONFIG_SOC_EXYNOS5433)
		return SYS_AFTR;
#else
		return SYS_ALPA;
#endif
#endif
	return SYS_LPA;
}

/***************************************************************************
 *                   Initialize exynos-powermode driver                    *
 ***************************************************************************/
static void parsing_dt_reg_check_list(struct device_node *np, const char *reg_name,
					const char *val_name, struct check_reg *list)
{
	const __be32 *reg_list, *val_list;
	unsigned int length, count, i;

	/*
	 * Register and value list should be same length on device tree.
	 * So get length of the list once.
	 */
	reg_list = of_get_property(np, reg_name, &length);
	val_list = of_get_property(np, val_name, NULL);

	if (!reg_list || !val_list) {
		pr_warn("No matching property or poperty unnecessary\n");
		return;
	}

	count = length / sizeof(unsigned int);

	for (i = 0; i < count; i++) {
		list[i].reg = ioremap(be32_to_cpup(reg_list++), SZ_32);
		list[i].check_bit = be32_to_cpup(val_list++);
	}
}

static void exynos_lpm_dt_init(void)
{
	struct device_node *np = of_find_node_by_name(NULL, "low-power-mode");

	parsing_dt_reg_check_list(np, "lpm-reg", "lpm-val", check_reg_lpm);
	parsing_dt_reg_check_list(np, "lpc-reg", "lpc-val", check_reg_lpc);
}

int __init exynos_powermode_init(void)
{
	exynos_idle_clock_down(true, LITTLE);
	exynos_idle_clock_down(true, BIG);

	init_cpd_state_mask();

	exynos_pmu_cal_sys_init();

	exynos_lpm_dt_init();

	if (sysfs_create_file(power_kobj, &lpc_attribute.attr))
		pr_err("%s: failed to create sysfs to control LPC\n", __func__);

	return 0;
}
arch_initcall(exynos_powermode_init);
