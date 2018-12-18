/*
 * PCIe host controller driver for Samsung EXYNOS SoCs
 *
 * Copyright (C) 2013 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * Author: Jingoo Han <jg1.han@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of_gpio.h>
#include <linux/pci.h>
#include <linux/platform_device.h>
#include <linux/resource.h>
#include <linux/types.h>
#include <linux/workqueue.h>

#include "pcie-designware.h"

#define MAX_TIMEOUT		1000

#define to_exynos_pcie(x)	container_of(x, struct exynos_pcie, pp)

struct exynos_pcie {
	struct device_node	*dev_node;
	void __iomem		*elbi_base;
	void __iomem		*phy_base;
	void __iomem		*sysreg_reg;
	void __iomem		*pmu_reg;
	void __iomem		*rc_dbi_base;
	void __iomem		*cmu_reg;
	void __iomem		*phy_pcs_reg;
	int			perst_gpio;
	struct pinctrl		*pinctrl_exynos;
	struct pinctrl_state	*pinctrl_reset;
	struct pinctrl_state	*pinctrl_default;
	bool			use_msi;
	struct delayed_work     work;
	struct workqueue_struct *pcie_wq;
	void (*link_down_cb)(struct pci_dev *pdev);
	struct pcie_port	pp;
};


/* PCIe ELBI registers */
#define PCIE_IRQ_PULSE			0x000
#define IRQ_INTA_ASSERT			(0x1 << 0)
#define IRQ_INTB_ASSERT			(0x1 << 2)
#define IRQ_INTC_ASSERT			(0x1 << 4)
#define IRQ_INTD_ASSERT			(0x1 << 6)
#define PCIE_IRQ_LEVEL			0x004
#define PCIE_IRQ_SPECIAL		0x008
#define PCIE_IRQ_EN_PULSE		0x00c
#define PCIE_IRQ_EN_LEVEL		0x010
//BABIS: Changed 2 -> 1 to enable the irq_en_msi_ctrl_int bit
//#define IRQ_MSI_ENABLE		(0x1 << 2)
#define IRQ_MSI_ENABLE			(0x1 << 1)
#define IRQ_LINKDOWN_ENABLE		(0x1 << 4)
#define PCIE_IRQ_EN_SPECIAL		0x014
#define PCIE_APP_INIT_RESET		0x028
#define PCIE_APP_LTSSM_ENABLE		0x02c
#define PCIE_L1_ENTRY_BUG_FIX_ENABLE	0x038
#define PCIE_ELBI_SLV_AWMISC		0x11c
#define PCIE_ELBI_SLV_ARMISC		0x120
#define PCIE_ELBI_SLV_DBI_ENABLE	(0x1 << 21)
#define PCIE_SW_WAKE			0x018
#define PCIE_BUS_NUM			0x01c
#define PCIE_RADM_MSG_LTR		0x020
#define PCIE_APP_LTR_LATENCY		0x024
#define PCIE_APP_REQ_EXIT_L1		0x040
#define PCIE_ELBI_RDLH_LINKUP		0x074
#define PCIE_ELBI_LTSSM_DISABLE		0x0
#define PCIE_ELBI_LTSSM_ENABLE		0x1
#define PCIE_PM_DSTATE			0x88
#define PCIE_D0_UNINIT_STATE		0x4
#define PCIE_CORE_RESETN_DISABLE	0xF0
#define PCIE_APP_REQ_EXIT_L1_MODE	0xF4

/* PCIe RC registers */
#define PCIE_MISC_CONTROL			0x8BC
#define	PCIE_RC_LCR				0x007C
#define PCIE_RC_LCR_MAX_LINK_SPEEDS_MASK	0xf
#define PCIE_RC_LCR_MAX_LINK_SPEEDS_GEN1	0x1
#define PCIE_RC_LCR_MAX_LINK_SPEEDS_GEN2 	0x2
#define PORT_LOGIC_SPEED_CHANGE			(0x1 << 17)
#define	PCIE_RC_LCTRL2_LSTAT2			0x00A0
#define PCIE_LINK_WIDTH_SPEED_CONTROL   0x80C
#define ID_MASK	0xffff

static void exynos_pcie_phy_init(struct pcie_port *);
static int exynos_pcie_rd_own_conf(struct pcie_port *pp, int where, int size, u32 *val);
static int exynos_pcie_wr_own_conf(struct pcie_port *pp, int where, int size, u32 val);
static void exynos_pcie_clear_irq_level(struct pcie_port *pp);
static irqreturn_t exynos_pcie_msi_irq_handler(int irq, void *arg);
static void exynos_pcie_msi_init(struct pcie_port *pp);
int exynos_pcie_reset(struct pcie_port *pp);
static int exynos_pcie_link_up(struct pcie_port *pp);
static void exynos_pcie_enable_interrupts(struct pcie_port *pp);
static int exynos_pcie_establish_link(struct pcie_port *pp);


void exynos_pcie_link_restored(struct pci_dev *pdev)
{
	struct pcie_port *pp;

	pp = pci_get_drvdata(pdev);

	pp->pci_link_up = 1;
	wmb();
}
EXPORT_SYMBOL(exynos_pcie_link_restored);

int exynos_pcie_is_linkup(struct pci_dev *pdev)
{
	struct pcie_port *pp;

	pp = pci_get_drvdata(pdev);

	wmb();
	return /*exynos_pcie_link_up(pp)*/pp->pci_link_up;
}
EXPORT_SYMBOL(exynos_pcie_is_linkup);

int exynos_pcie_register_link_down_cb(struct pci_dev *pdev, void (*link_down_cb)(struct pci_dev *pdev))
{
	struct pcie_port *pp;
	struct exynos_pcie *exynos_pcie;

	pp = pci_get_drvdata(pdev);

	if(pp==NULL) {return 1;}

	exynos_pcie = to_exynos_pcie(pp);

	exynos_pcie->link_down_cb = link_down_cb;

	return 0;		
}
EXPORT_SYMBOL(exynos_pcie_register_link_down_cb);

int exynos_pcie_reset_rc(struct pci_dev *pdev)
{
	struct pcie_port *pp;
	int status = 0;

	pp = pci_get_drvdata(pdev);

	if(pp==NULL) {return 1;}

	pp->pci_link_up = 0;
	wmb();

	mdelay(100);
/* JVELEG: disabled the following print */
/*	dev_info(pp->dev, "Reset Called from outside...\n"); */
	//dev_info(pp->dev, "Establishing link...\n");
	status = exynos_pcie_establish_link(pp);
	if(status!=0)
	{
		return status;
	}
	//dev_info(pp->dev, "loading saved state...\n");
	pci_load_saved_state(pp->pci_dev, pp->pci_state);
	//dev_info(pp->dev, "restoring saved state...\n");
	pci_restore_state(pp->pci_dev);
	//dev_info(pp->dev, "restoring msi...\n");
	dw_pcie_restore_msi(pp);

	return status;
}
EXPORT_SYMBOL(exynos_pcie_reset_rc);

unsigned long exynos_pcie_get_msi_addr(struct pci_dev *pdev)
{
	struct pcie_port *pp;

	pp = pci_get_drvdata(pdev);

	if(pp==NULL) {return 0x0;}

	return pp->msi_data;	
}
EXPORT_SYMBOL(exynos_pcie_get_msi_addr);


void exynos_pcie_link_retrain(struct pci_dev *pdev)
{
	struct pcie_port *pp;

	pp = pci_get_drvdata(pdev);

	if(pp==NULL) {return;}
	dw_pcie_link_retrain(pp);
	mdelay(10);

}
EXPORT_SYMBOL(exynos_pcie_link_retrain);


static void exynos_pcie_ep_reset(struct pcie_port *pp)
{
	if(pp==NULL){ return; }
	
	if(pp->full_reset_gpio > 0)
    	{
        	gpio_set_value(pp->full_reset_gpio, 1);
        	mdelay(300);
        	gpio_set_value(pp->full_reset_gpio, 0);
    	}
	
	if(pp->reset_ep_gpio > 0 && pp->partial_reset_gpio > 0)
	{	
		gpio_set_value(pp->reset_ep_gpio, 1);
		gpio_set_value(pp->partial_reset_gpio, 1);
		udelay(10);
		gpio_set_value(pp->reset_ep_gpio, 0);
		udelay(10);
		gpio_set_value(pp->partial_reset_gpio, 0);
	}
}

static void exynos_pcie_host_init(struct pcie_port *pp);

static void exynos_pcie_clock_enable(struct pcie_port *pp, int enable)
{
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pp);
	struct clk *clk;
	int i;

	if (enable) {
		for (i = 0; i < 14; i++)
		{			
			clk = of_clk_get(exynos_pcie->dev_node, i);
			if (IS_ERR(clk))
			{
				dev_err(pp->dev, "Enable clocks: Error in clk %d\n", i);
			}
			clk_prepare_enable(clk);
		}
	} else {
		for (i = 0; i < 14; i++)
		{
			clk = of_clk_get(exynos_pcie->dev_node, i);
			if (IS_ERR(clk))
			{
				dev_err(pp->dev, "Enable clocks: Error in clk %d\n", i);
			}
			clk_disable_unprepare(clk);
		}
	}
}

static void exynos_pcie_phy_clock_enable(struct pcie_port *pp, int enable)
{
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pp);
	struct clk *clk;
	int i;

	if (enable) {
		for (i = 0; i < 14; i++)
		{			
			clk = of_clk_get(exynos_pcie->dev_node, i);
			if (IS_ERR(clk))
			{
				dev_err(pp->dev, "Enable clocks: Error in clk %d\n", i);
			}
			clk_prepare_enable(clk);
		}
	} else {
		for (i = 0; i < 14; i++)
		{
			clk = of_clk_get(exynos_pcie->dev_node, i);
			if (IS_ERR(clk))
			{
				dev_err(pp->dev, "Enable clocks: Error in clk %d\n", i);
			}
			clk_disable_unprepare(clk);
		}
	}
}


static void exynos_pcie_phy_init(struct pcie_port *pp)
{
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pp);
	void __iomem *elbi_base = exynos_pcie->elbi_base;
	void __iomem *phy_base = exynos_pcie->phy_base;
	void __iomem *sysreg_reg = exynos_pcie->sysreg_reg;
	void __iomem *phy_pcs_reg = exynos_pcie->phy_pcs_reg;
	int i = 0;
	u32 val = 0;

	/* 24MHz gen1 */
	/*	u32 cmn_config_val[26] = {0x01, 0x0F, 0xa6, 0x71, 0x90, 0x62, 0x20,
		0x00, 0x00, 0xa7, 0x0a, 0x37, 0x20, 0x08,
		0xEF, 0xfc, 0x96, 0x14, 0x00, 0x10, 0x68,
		0x01, 0x00, 0x00, 0x04, 0x20};
		u32 trsv_config_val[41] = {0x31, 0xF4, 0xF4, 0x80, 0x25, 0x40, 0xC0,
		0x03, 0x35, 0x55, 0x4c, 0xc3, 0x10, 0x54,
		0x70, 0xc5, 0x00, 0x2f, 0x44, 0xa4, 0x00,
		0x3b, 0x34, 0xa7, 0x64, 0x00, 0x1f, 0x83,
		0x1b, 0x01, 0xE0, 0x00, 0x00, 0x02, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x1d, 0x00};
	 */



	//Settings for Gen2!!
	u32 cmn_config_val[26] = {0x01, 0x04, 0xa6, 0x34, 0x90, 0x40, 0x00, 0x00, 0x00, 0x80, 0x00,
		0x33, 0x20, 0x08, 0x7e, 0xfa, 0x82, 0x10, 0x80, 0x20, 0x32, 0x00,
		0x00, 0x00, 0x04, 0x10};

	u32 trsv_config_val[41] = {0x31, 0xF4, 0xF4, 0x80, 0x25, 0x40, 0xb0, 0x03, 0x35, 0x55, 0x4c,
		0xc3, 0x10, 0x54, 0x70, 0xc5, 0x00, 0x2f, 0x38, 0xa4, 0x00, 0x1b,
		0x64, 0x40, 0x69, 0x00, 0x1F, 0x81, 0x1b, 0x01, 0x80, 0x00, 0x00,
		0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x15, 0x00};


	//BABIS: Commented Gen2
	/* set high speed for Gen2 */
	writel(readl(sysreg_reg) | (0x1 << 2), sysreg_reg);

	/* Clock Gating */
	val = readl(sysreg_reg + 0xC);
	val &= ~(0x3 << 4);
	val |= (0x2 << 4);
	writel(val, sysreg_reg + 0xC);

	val = readl(sysreg_reg + 0xC);
	val &= ~(0x3 << 2);
	val |= (0x1 << 2);
	writel(val, sysreg_reg + 0xC);

	val = readl(sysreg_reg);

	val = readl(sysreg_reg + 0xC);

	/* pcs_g_rst */
	writel(0x1, elbi_base + 0x130);
	udelay(10);
	writel(0x0, elbi_base + 0x130);
	udelay(10);

	/* pma_init_rst */
	writel(0x1, elbi_base + 0x134);
	udelay(10);
	writel(0x0, elbi_base + 0x134);
	udelay(10);

	/* PHY Common block Setting */
	for (i = 0; i < 26; i++)
		writel(cmn_config_val[i], phy_base + (i * 4));

	/* PHY Tranceiver/Receiver block Setting */
	for (i = 0; i < 41; i++)
		writel(trsv_config_val[i], phy_base + ((0x30 + i) * 4));

	writel(0x2, phy_pcs_reg);

	/* PCIE_PHY CMN_RST */
	val = readl(sysreg_reg + 0xC);
	val &= ~(0x1 << 7);
	writel(val, sysreg_reg + 0xC);

	val = readl(sysreg_reg + 0xC);
	val |= (0x1 << 7);
	writel(val, sysreg_reg + 0xC);

	udelay(20);

	val = readl(sysreg_reg + 0xC);
	val &= ~(0x1 << 7);
	writel(val, sysreg_reg + 0xC);

	val = readl(sysreg_reg + 0xC);
}

int exynos_pcie_reset(struct pcie_port *pp)
{
	struct device *dev = pp->dev;
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pp);
	void __iomem *elbi_base = exynos_pcie->elbi_base;
	void __iomem *phy_base = exynos_pcie->phy_base;
	//void __iomem *dbi_base = exynos_pcie->rc_dbi_base;
	u32 val = 0;
	u32 tmp0 = 0, tmp1 = 0;
	int count = 0, try_cnt = 0;
	unsigned long flags;
	int status;

	spin_lock_irqsave(&pp->conf_lock, flags);
retry:
	//exynos_pcie_rd_own_conf(pp, PCI_COMMAND, 4,  &val);
	//printk(KERN_INFO "PCI_COMMAND: %X\n", val);
	//Reset PCI Endpoint
	exynos_pcie_ep_reset(pp);

	//printk(KERN_INFO "BABIS: RxElecIdle\n");
	/* set RxElecIdle = 0 to access DBI area */
	tmp0 = readl(phy_base + 0x4A*4);
	tmp1 = readl(phy_base + 0x5C*4);
	writel(0xDF, phy_base + 0x4A*4);
	writel(0x54, phy_base + 0x5C*4);

	//printk(KERN_INFO "BABIS: Link reset\n");
	
	/* link reset by using wake pin */
	if(exynos_pcie->pinctrl_exynos!=NULL && exynos_pcie->pinctrl_default!=NULL && exynos_pcie->pinctrl_reset!=NULL)
	{	
		status = pinctrl_select_state(exynos_pcie->pinctrl_exynos, exynos_pcie->pinctrl_reset);
		if (status!=0)
			dev_err(dev, "failed pcie link reset\n");

		//printk(KERN_INFO "BABIS: Link reset asserted\n");
		udelay(20);

		status = pinctrl_select_state(exynos_pcie->pinctrl_exynos, exynos_pcie->pinctrl_default);
		if (status!=0)
			dev_err(dev, "failed pcie link reset\n");
	}
	else
	{
		dev_err(dev, "pinctrl states NULL");
	}
	//printk(KERN_INFO "BABIS: Perst gpio\n");
	/* set #PERST high */
	gpio_set_value(exynos_pcie->perst_gpio, 1);
	mdelay(20);

	//exynos_pcie_rd_own_conf(pp, PCI_COMMAND, 4,  &val);
	//printk(KERN_INFO "PCI_COMMAND: %X\n", val);
	exynos_pcie_phy_init(pp);

	/* Bus number enable */
	val = readl(elbi_base + PCIE_SW_WAKE);
	val &= ~(0x1 << 1);
	writel(val, elbi_base + PCIE_SW_WAKE);

	writel(0x1, elbi_base + PCIE_L1_ENTRY_BUG_FIX_ENABLE);


	/* DBI_RO_WR_EN */
	exynos_pcie_wr_own_conf(pp, PCIE_MISC_CONTROL, 4,  1);

	/* setup root complex */
	dw_pcie_setup_rc(pp);

	/* Rollback PHY Changes for clearing RxElecIdle */
	writel(tmp1, phy_base + 0x5C*4);
	writel(tmp0, phy_base + 0x4A*4);

	//dev_info(dev, "D state: %x, %x\n",
	//		readl(elbi_base + PCIE_PM_DSTATE) & 0x7,
	//		readl(elbi_base + PCIE_ELBI_RDLH_LINKUP) & 0x1f); 

	/* assert LTSSM enable */
	writel(PCIE_ELBI_LTSSM_ENABLE, elbi_base + PCIE_APP_LTSSM_ENABLE);
	
	//dev_info(dev, "After LTSSM enable\n"); 
	count = 0;
	while (count < MAX_TIMEOUT) {
		val = readl(elbi_base + PCIE_IRQ_LEVEL) & 0x10;
		if (val)
			break;

		count++;

		udelay(10);
	}

	/* wait to check whether link down again(D0 UNINIT) or not for retry */
	mdelay(1);
	val = readl(elbi_base + PCIE_PM_DSTATE) & 0x7;
	if (count >= MAX_TIMEOUT || val == PCIE_D0_UNINIT_STATE) {
		try_cnt++;
		//dev_err(dev, "%s: Link is not up, try count: %d, %x\n", __func__, try_cnt,
		//		readl(elbi_base + PCIE_ELBI_RDLH_LINKUP));

		if (try_cnt < 10) {
			gpio_set_value(exynos_pcie->perst_gpio, 0);
			/* LTSSM disable */
			//writel(PCIE_ELBI_LTSSM_DISABLE,
			//		elbi_base + PCIE_APP_LTSSM_ENABLE);

			mdelay(15);
			goto retry;
		} else {			
			spin_unlock_irqrestore(&pp->conf_lock, flags);
			return -EPIPE;
		}
	} else {
		//dev_info(dev, "%s: Link up:%x, enabling interrupt\n", __func__,
                //                readl(elbi_base + PCIE_ELBI_RDLH_LINKUP));
		writel(readl(exynos_pcie->elbi_base + PCIE_IRQ_SPECIAL),
                                exynos_pcie->elbi_base + PCIE_IRQ_SPECIAL);
		writel(readl(exynos_pcie->elbi_base + PCIE_IRQ_EN_LEVEL)
                                | IRQ_LINKDOWN_ENABLE,
                                exynos_pcie->elbi_base + PCIE_IRQ_EN_LEVEL);
		//dev_info(dev, "%s: Link up:%x\n", __func__,
		//		readl(elbi_base + PCIE_ELBI_RDLH_LINKUP));

	}
	/* setup ATU for cfg/mem outbound */
        dw_pcie_prog_viewport_cfg0(pp, 0x1000000);
        dw_pcie_prog_viewport_mem_outbound(pp);

	spin_unlock_irqrestore(&pp->conf_lock, flags);
	return 0;
}

static int exynos_pcie_establish_link(struct pcie_port *pp)
{
	int status = 0;
	
	//Link is assumed down
	pp->pci_link_up = 0;
	wmb();	
	if(pp->initialized==0){
		status = exynos_pcie_reset(pp);
	}
	else
	{
		//Reset PCI Endpoint
		exynos_pcie_ep_reset(pp);
		mdelay(100);
		dw_pcie_setup_rc(pp);
		mdelay(10);
		if(exynos_pcie_link_up(pp))
		{
			status = 0;
		}
		else
		{
			//status = -1;
			status = exynos_pcie_reset(pp);
		}
	}

	return status;	
}

static void exynos_pcie_clear_irq_pulse(struct pcie_port *pp)
{
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pp);
	void __iomem *elbi_base = exynos_pcie->elbi_base;
	u32 val = 0;

	val = readl(elbi_base + PCIE_IRQ_PULSE);
	writel(val, elbi_base + PCIE_IRQ_PULSE);

	val = readl(elbi_base + PCIE_IRQ_LEVEL);
	writel(val, elbi_base + PCIE_IRQ_LEVEL);

	val = readl(elbi_base + PCIE_IRQ_SPECIAL);
	if (val & (0x1 << 2))
	{		
		//dev_info(pp->dev, "link down interrupt! : %X\n", val);
		//pp->pci_link_up = 0;
		//wmb();
	}
	writel(val, elbi_base + PCIE_IRQ_SPECIAL);

	return;
}

static void exynos_pcie_enable_irq_pulse(struct pcie_port *pp)
{
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pp);
	void __iomem *elbi_base = exynos_pcie->elbi_base;
	u32 val = 0;

	/* enable INTX interrupt */
	val = IRQ_INTA_ASSERT | IRQ_INTB_ASSERT |
		IRQ_INTC_ASSERT | IRQ_INTD_ASSERT,
				writel(val, elbi_base + PCIE_IRQ_EN_PULSE);

	writel(0x0, elbi_base + PCIE_IRQ_EN_PULSE);

	/* disable LEVEL interrupt */
	writel(0x0, elbi_base + PCIE_IRQ_EN_LEVEL);

	/* disable SPECIAL interrupt */
	writel(0x0, elbi_base + PCIE_IRQ_EN_SPECIAL);
	

	return;
}

static irqreturn_t exynos_pcie_irq_handler(int irq, void *arg)
{
	struct pcie_port *pp = arg;

	if (IS_ENABLED(CONFIG_PCI_MSI))
		exynos_pcie_msi_irq_handler(irq, arg);

	exynos_pcie_clear_irq_pulse(pp);
	return IRQ_HANDLED;
}

static void exynos_pcie_clear_irq_level(struct pcie_port *pp)
{
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pp);
	void __iomem *elbi_base = exynos_pcie->elbi_base;
	u32 val;

	val = readl(elbi_base + PCIE_IRQ_LEVEL);
	writel(val, elbi_base + PCIE_IRQ_LEVEL);
	return;
}

static irqreturn_t exynos_pcie_msi_irq_handler(int irq, void *arg)
{
	struct pcie_port *pp = arg;

	/* handle msi irq */
	dw_handle_msi_irq(pp);
	exynos_pcie_clear_irq_level(pp);

	return IRQ_HANDLED;
}

static void exynos_pcie_msi_init(struct pcie_port *pp)
{
	u32 val;
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pp);
	void __iomem *elbi_base = exynos_pcie->elbi_base;

	if (!exynos_pcie->use_msi)
		return;

	dw_pcie_msi_init(pp);

	/* enable MSI interrupt */
	val = readl(elbi_base + PCIE_IRQ_EN_LEVEL);
	val |= IRQ_MSI_ENABLE;
	writel(val, elbi_base + PCIE_IRQ_EN_LEVEL);
	return;
}

static void exynos_pcie_enable_interrupts(struct pcie_port *pp)
{
	exynos_pcie_enable_irq_pulse(pp);

	if (IS_ENABLED(CONFIG_PCI_MSI))
		exynos_pcie_msi_init(pp);
	
	return;
}

static inline void exynos_pcie_readl_rc(struct pcie_port *pp,
		void __iomem *dbi_base, u32 *val)
{
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pp);

	/* Switch to SW mode and back */
	writel(0x1, exynos_pcie->elbi_base + PCIE_APP_REQ_EXIT_L1);
	writel(0x0, exynos_pcie->elbi_base + PCIE_APP_REQ_EXIT_L1_MODE);
	*val = readl(dbi_base);
	writel(0x1, exynos_pcie->elbi_base + PCIE_APP_REQ_EXIT_L1_MODE);
	writel(0x0, exynos_pcie->elbi_base + PCIE_APP_REQ_EXIT_L1);
}

static inline void exynos_pcie_writel_rc(struct pcie_port *pp,
		u32 val, void __iomem *dbi_base)
{
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pp);

	/* Switch to SW mode and back */
	writel(0x1, exynos_pcie->elbi_base + PCIE_APP_REQ_EXIT_L1);
	writel(0x0, exynos_pcie->elbi_base + PCIE_APP_REQ_EXIT_L1_MODE);
	writel(val, dbi_base);
	writel(0x1, exynos_pcie->elbi_base + PCIE_APP_REQ_EXIT_L1_MODE);
	writel(0x0, exynos_pcie->elbi_base + PCIE_APP_REQ_EXIT_L1);
}

static int exynos_pcie_rd_own_conf(struct pcie_port *pp, int where, int size,
		u32 *val)
{
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pp);
	int ret = 0;
	
	writel(0x1, exynos_pcie->elbi_base + PCIE_APP_REQ_EXIT_L1);
	writel(0x0, exynos_pcie->elbi_base + PCIE_APP_REQ_EXIT_L1_MODE);
	ret = dw_pcie_cfg_read(exynos_pcie->rc_dbi_base + where, size, val);
	writel(0x1, exynos_pcie->elbi_base + PCIE_APP_REQ_EXIT_L1_MODE);
	writel(0x0, exynos_pcie->elbi_base + PCIE_APP_REQ_EXIT_L1);

	return ret;
}

static int exynos_pcie_wr_own_conf(struct pcie_port *pp, int where, int size,
		u32 val)
{
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pp);
	int ret = 0;

	//if(!(pp->pci_link_up))
	//	return PCIBIOS_DEVICE_NOT_FOUND;
	/* Switch to SW mode and back */
	writel(0x1, exynos_pcie->elbi_base + PCIE_APP_REQ_EXIT_L1);
	writel(0x0, exynos_pcie->elbi_base + PCIE_APP_REQ_EXIT_L1_MODE);
	ret = dw_pcie_cfg_write(exynos_pcie->rc_dbi_base + where, size, val);
	writel(0x1, exynos_pcie->elbi_base + PCIE_APP_REQ_EXIT_L1_MODE);
	writel(0x0, exynos_pcie->elbi_base + PCIE_APP_REQ_EXIT_L1);

	return ret;
}

static inline int exynos_pcie_link_up(struct pcie_port *pp)
{
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pp);
	u32 val;

	val = readl(exynos_pcie->elbi_base + PCIE_ELBI_RDLH_LINKUP) & 0x1f;

	if (val >= 0x0d && val <= 0x15)
		return 1;

	return 0;
}

static void exynos_pcie_host_init(struct pcie_port *pp)
{
	struct exynos_pcie *exynos_pcie = to_exynos_pcie(pp);
	int retries = 0;
	int status = 0;

	/* Enable the needed clocks */
	exynos_pcie_clock_enable(pp, 1);
	exynos_pcie_phy_clock_enable(pp, 1);

	/* Power up the block */
	writel(1, exynos_pcie->pmu_reg);

	exynos_pcie_enable_interrupts(pp);

	while(retries<10)
	{
		status = exynos_pcie_establish_link(pp);
		if(status==0)
		{
			break;
		}

		dev_warn(pp->dev, "Link not established! Retrying...\n");
		retries++;		
	}

	if(status!=0)
	{
		dev_err(pp->dev, "Cannot initialize link!\n");
		return;
	}	
	pp->pci_link_up = 1;	
	pp->initialized = 1;
	wmb();
}


static struct pcie_host_ops exynos_pcie_host_ops = {
	.readl_rc = exynos_pcie_readl_rc,
	.writel_rc = exynos_pcie_writel_rc,
	.rd_own_conf = exynos_pcie_rd_own_conf,
	.wr_own_conf = exynos_pcie_wr_own_conf,
	.link_up = exynos_pcie_link_up,
	.host_init = exynos_pcie_host_init,
};

static int add_pcie_port(struct pcie_port *pp, struct platform_device *pdev)
{
	int ret = 0;

	pp->irq = platform_get_irq(pdev, 0);
	if (!pp->irq) {
		dev_err(&pdev->dev, "failed to get irq\n");
		return -ENODEV;
	}

	ret = devm_request_irq(&pdev->dev, pp->irq, exynos_pcie_irq_handler,
			IRQF_SHARED, "exynos7-pcie", pp);
	if (ret) {
		dev_err(&pdev->dev, "failed to request irq\n");
		return ret;
	}

	pp->root_bus_nr = -1;
	pp->ops = &exynos_pcie_host_ops;
	spin_lock_init(&pp->conf_lock);
	ret = dw_pcie_host_init(pp);
	if (ret) {
		dev_err(&pdev->dev, "failed to initialize host\n");
		return ret;
	}

	return 0;
}

static int __init exynos_pcie_probe(struct platform_device *pdev)
{
	struct exynos_pcie *exynos_pcie;
	struct pcie_port *pp;
	struct resource *elbi_base;
	struct resource *phy_base;
	struct resource *sysreg_reg;
	struct resource *pmu_reg;
	struct resource *rc_dbi_base;
	struct resource *cmu_reg;
	struct resource *phy_pcs_reg;
	struct device_node *root_node = pdev->dev.of_node;
	int ret = 0;
	int status;


	exynos_pcie = devm_kzalloc(&pdev->dev, sizeof(*exynos_pcie),
			GFP_KERNEL);
	if (!exynos_pcie)
		return -ENOMEM;

	pp = &exynos_pcie->pp;
	pp->dev = &pdev->dev;

	exynos_pcie->dev_node = root_node;

	exynos_pcie->use_msi = of_property_read_bool(root_node, "use-msi");

	exynos_pcie->perst_gpio = of_get_named_gpio(root_node, "perst-gpio", 0);
	if (exynos_pcie->perst_gpio < 0)
		dev_err(&pdev->dev, "cannot get perst-gpio\n");
	else
		devm_gpio_request_one(pp->dev, exynos_pcie->perst_gpio,
				GPIOF_OUT_INIT_LOW, "PERST");

	elbi_base = platform_get_resource_byname(pdev, IORESOURCE_MEM, "elbi");
	exynos_pcie->elbi_base = devm_ioremap_resource(&pdev->dev, elbi_base);
	if (IS_ERR(exynos_pcie->elbi_base)) {
		ret = PTR_ERR(exynos_pcie->elbi_base);
		goto probe_fail;
	}

	phy_base = platform_get_resource_byname(pdev, IORESOURCE_MEM, "phy");
	exynos_pcie->phy_base = devm_ioremap_resource(&pdev->dev, phy_base);
	if (IS_ERR(exynos_pcie->phy_base)) {
		ret = PTR_ERR(exynos_pcie->phy_base);
		goto probe_fail;
	}

	sysreg_reg = platform_get_resource_byname(pdev, IORESOURCE_MEM, "sysreg-reg");
	exynos_pcie->sysreg_reg = devm_ioremap_resource(&pdev->dev, sysreg_reg);
	if (IS_ERR(exynos_pcie->sysreg_reg)) {
		ret = PTR_ERR(exynos_pcie->sysreg_reg);
		goto probe_fail;
	}

	pmu_reg = platform_get_resource_byname(pdev, IORESOURCE_MEM, "pmu-reg");
	exynos_pcie->pmu_reg = devm_ioremap_resource(&pdev->dev, pmu_reg);
	if (IS_ERR(exynos_pcie->pmu_reg)) {
		ret = PTR_ERR(exynos_pcie->pmu_reg);
		goto probe_fail;
	}

	rc_dbi_base = platform_get_resource_byname(pdev, IORESOURCE_MEM, "config");
	exynos_pcie->rc_dbi_base = devm_ioremap_resource(&pdev->dev, rc_dbi_base);
	if (IS_ERR(exynos_pcie->rc_dbi_base)) {
		ret = PTR_ERR(exynos_pcie->rc_dbi_base);
		goto probe_fail;
	}

	pp->dbi_base = exynos_pcie->rc_dbi_base;

	cmu_reg = platform_get_resource_byname(pdev, IORESOURCE_MEM, "cmu-reg");
	exynos_pcie->cmu_reg = devm_ioremap_resource(&pdev->dev, cmu_reg);
	if (IS_ERR(exynos_pcie->cmu_reg)) {
		ret = PTR_ERR(exynos_pcie->cmu_reg);
		goto probe_fail;
	}

	phy_pcs_reg = platform_get_resource_byname(pdev, IORESOURCE_MEM, "phy-pcs-reg");
	exynos_pcie->phy_pcs_reg = devm_ioremap_resource(&pdev->dev, phy_pcs_reg);
	if (IS_ERR(exynos_pcie->phy_pcs_reg)) {
		ret = PTR_ERR(exynos_pcie->phy_pcs_reg);
		goto probe_fail;
	}

	pp->reset_ep_gpio = of_get_named_gpio(root_node, "ep_reset_gpio", 0);
	if(pp->reset_ep_gpio < 0)
	{
		dev_err(&pdev->dev, "cannot get reset_ep_gpio\n");
	}
	else
	{
		status = gpio_request(pp->reset_ep_gpio, "pci_ep_reset");
	
		if(status!=0)
		{ 
			dev_err(&pdev->dev, "cannot request reset_ep_gpio\n");
			pp->reset_ep_gpio = 0;
		}
		else
		{
			gpio_direction_output(pp->reset_ep_gpio, 0);
		}
	}

	pp->partial_reset_gpio = of_get_named_gpio(root_node, "partial_reset_gpio", 0);
	if(pp->partial_reset_gpio < 0)
	{
		dev_err(&pdev->dev, "cannot get partial_reset_gpio\n");
	}
	else
	{
		status = gpio_request(pp->partial_reset_gpio, "partial_reset");
	
		if(status!=0)
		{ 
			dev_err(&pdev->dev, "cannot request partial_reset_gpio\n");
			pp->reset_ep_gpio = 0;
		}
		else
		{
			gpio_direction_output(pp->partial_reset_gpio, 0);
		}
	}
	
	pp->full_reset_gpio = of_get_named_gpio(root_node, "full_reset_gpio", 0);
        if(pp->full_reset_gpio < 0)
        {
                dev_err(&pdev->dev, "cannot get full_reset_gpio\n");
        }
        else
        {
                status = gpio_request(pp->full_reset_gpio, "full_reset");

                if(status!=0)
                {
                        dev_err(&pdev->dev, "cannot request full_reset_gpio\n");
                        pp->full_reset_gpio = 0;
                }
                else
                {
                        gpio_direction_output(pp->full_reset_gpio, 0);
                }
        }

	
	exynos_pcie->pinctrl_exynos = devm_pinctrl_get_select_default(pp->dev);
	if(IS_ERR(exynos_pcie->pinctrl_exynos))
	{
		dev_err(&pdev->dev, "Cannot get pinctrl!!\n");
		exynos_pcie->pinctrl_reset = NULL;
		exynos_pcie->pinctrl_default = NULL;
	}
	else
	{
		exynos_pcie->pinctrl_reset = pinctrl_lookup_state(exynos_pcie->pinctrl_exynos, "pcie_reset");
		exynos_pcie->pinctrl_default = pinctrl_lookup_state(exynos_pcie->pinctrl_exynos, "default");
		if(IS_ERR(exynos_pcie->pinctrl_reset))
		{
			dev_err(&pdev->dev, "Cannot get pinctrl_reset state\n");
		}
		
		if(IS_ERR(exynos_pcie->pinctrl_default))
		{
			dev_err(&pdev->dev, "Cannot get pinctrl_deafult state\n");
		}
	}

	pp->initialized = 0;
	ret = add_pcie_port(pp, pdev);	
	if (ret)
		goto probe_fail;

	if(pp->pci_dev!=NULL)
	{
		pci_save_state(pp->pci_dev);
		pp->pci_state = pci_store_saved_state(pp->pci_dev);
	}

	platform_set_drvdata(pdev, exynos_pcie);


probe_fail:
	if (ret)
		dev_err(&pdev->dev, "%s: pcie probe failed\n", __func__);
	else
		dev_info(&pdev->dev, "%s: pcie probe success\n", __func__);

	return ret;
}

#ifdef CONFIG_OF
static const struct of_device_id exynos_pcie_of_match[] = {
	{ .compatible = "samsung,exynos7420-pcie", },
	{},
};
MODULE_DEVICE_TABLE(of, exynos_pcie_of_match);
#endif

static struct platform_driver exynos_pcie_driver = {
	.driver = {
		.name	= "exynos7-pcie",
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(exynos_pcie_of_match)
	},
};

/* Exynos PCIe driver does not allow module unload */
static int __init pcie_init(void)
{
	return platform_driver_probe(&exynos_pcie_driver, exynos_pcie_probe);
}
device_initcall(pcie_init);

MODULE_AUTHOR("Jingoo Han <jg1.han@samsung.com>");
MODULE_DESCRIPTION("Samsung Exynos 7420 PCIe host controller driver");
MODULE_LICENSE("GPL v2");
