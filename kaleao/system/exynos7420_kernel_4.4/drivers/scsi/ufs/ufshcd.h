/*
 * Universal Flash Storage Host controller driver
 *
 * This code is based on drivers/scsi/ufs/ufshcd.h
 * Copyright (C) 2011-2013 Samsung India Software Operations
 *
 * Authors:
 *	Santosh Yaraganavi <santosh.sy@samsung.com>
 *	Vinayak Holikatti <h.vinayak@samsung.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * See the COPYING file in the top-level directory or visit
 * <http://www.gnu.org/licenses/gpl-2.0.html>
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * This program is provided "AS IS" and "WITH ALL FAULTS" and
 * without warranty of any kind. You are solely responsible for
 * determining the appropriateness of using and distributing
 * the program and assume all risks associated with your exercise
 * of rights with respect to the program, including but not limited
 * to infringement of third party rights, the risks and costs of
 * program errors, damage to or loss of data, programs or equipment,
 * and unavailability or interruption of operations. Under no
 * circumstances will the contributor of this Program be liable for
 * any damages of any kind arising from your use or distribution of
 * this program.
 */

#ifndef _UFSHCD_H
#define _UFSHCD_H

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/wait.h>
#include <linux/bitops.h>
#include <linux/pm_runtime.h>
#include <linux/clk.h>
#include <linux/completion.h>
#include <linux/jiffies.h>

#include <asm/irq.h>
#include <asm/byteorder.h>
#include <scsi/scsi.h>
#include <scsi/scsi_cmnd.h>
#include <scsi/scsi_host.h>
#include <scsi/scsi_tcq.h>
#include <scsi/scsi_dbg.h>
#include <scsi/scsi_eh.h>

#include "ufs.h"
#include "ufshci.h"

#define UFSHCD "ufshcd"
#define UFSHCD_DRIVER_VERSION "0.2"


/*#define UFS_EXYNOS_DEBUG*/

enum dev_cmd_type {
	DEV_CMD_TYPE_NOP		= 0x0,
	DEV_CMD_TYPE_QUERY		= 0x1,
};

struct ufs_hba;

/**
 * struct uic_command - UIC command structure
 * @command: UIC command
 * @argument1: UIC command argument 1
 * @argument2: UIC command argument 2
 * @argument3: UIC command argument 3
 * @cmd_active: Indicate if UIC command is outstanding
 * @result: UIC command result
 * @done: UIC command completion
 */
struct uic_command {
	u32 command;
	u32 argument1;
	u32 argument2;
	u32 argument3;
	int cmd_active;
	int result;
	struct completion done;
};

struct uic_pwr_mode {
	u8 lane;
	u8 gear;
	u8 mode;
	u8 hs_series;
	bool termn;
	u32 local_l2_timer[3];
	u32 remote_l2_timer[3];
};

/**
 * struct ufshcd_lrb - local reference block
 * @utr_descriptor_ptr: UTRD address of the command
 * @ucd_req_ptr: UCD address of the command
 * @ucd_rsp_ptr: Response UPIU address for this command
 * @ucd_prdt_ptr: PRDT address of the command
 * @cmd: pointer to SCSI command
 * @sense_buffer: pointer to sense buffer address of the SCSI command
 * @sense_bufflen: Length of the sense buffer
 * @scsi_status: SCSI status of the command
 * @command_type: SCSI, UFS, Query.
 * @task_tag: Task tag of the command
 * @lun: LUN of the command
 * @intr_cmd: Interrupt command (doesn't participate in interrupt aggregation)
 */
struct ufshcd_lrb {
	struct utp_transfer_req_desc *utr_descriptor_ptr;
	struct utp_upiu_req *ucd_req_ptr;
	struct utp_upiu_rsp *ucd_rsp_ptr;
	struct ufshcd_sg_entry *ucd_prdt_ptr;

	struct list_head list;
	struct scsi_cmnd *cmd;
	u8 *sense_buffer;
	unsigned int sense_bufflen;
	int scsi_status;

	int command_type;
	int task_tag;
	unsigned int lun;
	bool intr_cmd;
};

/**
 * struct ufs_query - holds relevent data structures for query request
 * @request: request upiu and function
 * @descriptor: buffer for sending/receiving descriptor
 * @response: response upiu and response
 */
struct ufs_query {
	struct ufs_query_req request;
	u8 *descriptor;
	struct ufs_query_res response;
};

/**
 * struct ufs_dev_cmd - all assosiated fields with device management commands
 * @type: device management command type - Query, NOP OUT
 * @lock: lock to allow one command at a time
 * @complete: internal commands completion
 * @tag_wq: wait queue until free command slot is available
 */
struct ufs_dev_cmd {
	enum dev_cmd_type type;
	struct mutex lock;
	struct completion *complete;
	wait_queue_head_t tag_wq;
	struct ufs_query query;
};

/*
 * ufshcd_ops: host specific operations
 * ufshcd_pre_link_startup: configre and contorl host's link before link startup
 * ufshcd_post_link_startup: configure and control host's link after link startup
 * ufshcd_host_reset: reset host for specific vendor
 * ufshcd_dev_hw_reset: reset device hardware via rst_n signal
 */
struct ufshcd_ops {
	int (*ufshcd_pre_link_startup)(struct ufs_hba *hba);
	int (*ufshcd_post_link_startup)(struct ufs_hba *hba);
	void (*ufshcd_host_reset)(struct ufs_hba *hba);
	void (*ufshcd_dev_hw_reset)(struct ufs_hba *hba);
	int (*ufshcd_pre_prep_pmc)(struct ufs_hba *hba,
				struct uic_pwr_mode *pmd);
	int (*ufshcd_post_prep_pmc)(struct ufs_hba *hba,
			struct uic_pwr_mode *pmd);
	void (*ufshcd_set_nexus_t_xfer_req)(struct ufs_hba *hba,
				int tag, struct scsi_cmnd *cmd);
	void (*ufshcd_set_nexus_t_task_mgmt)(struct ufs_hba *hba,
				int tag, u8 tm_func);
	void (*ufshcd_pre_hibern8)(struct ufs_hba *hba, u8 enter);
	void (*ufshcd_post_hibern8)(struct ufs_hba *hba, u8 enter);
	int (*ufshcd_get_hibern8_enter_delay)(struct ufs_hba *hba);
};

/**
 * ufshcd_drv_data: host specific data
 */
struct ufshcd_drv_data {
	const struct ufshcd_ops *ops;
	u32 quirks;
	const void *vs_data;
};

/**
 * struct ufs_hba - per adapter private structure
 * @mmio_base: UFSHCI base register address
 * @ucdl_base_addr: UFS Command Descriptor base address
 * @utrdl_base_addr: UTP Transfer Request Descriptor base address
 * @utmrdl_base_addr: UTP Task Management Descriptor base address
 * @ucdl_dma_addr: UFS Command Descriptor DMA address
 * @utrdl_dma_addr: UTRDL DMA address
 * @utmrdl_dma_addr: UTMRDL DMA address
 * @host: Scsi_Host instance of the driver
 * @dev: device handle
 * @lrb: local reference block
 * @lrb_in_use: lrb in use
 * @outstanding_tasks: Bits representing outstanding task requests
 * @outstanding_reqs: Bits representing outstanding transfer requests
 * @capabilities: UFS Controller Capabilities
 * @nutrs: Transfer Request Queue depth supported by controller
 * @nutmrs: Task Management Queue depth supported by controller
 * @ufs_version: UFS Version to which controller complies
 * @irq: Irq number of the controller
 * @active_uic_cmd: handle of active UIC command
 * @uic_cmd_mutex: mutex for uic command
 * @tm_wq: wait queue for task management
 * @tm_tag_wq: wait queue for free task management slots
 * @tm_condition: condition variable for task management
 * @tm_slots_in_use: bit map of task management request slots in use
 * @pwr_done: completion for power mode change
 * @ufshcd_state: UFSHCD states
 * @eh_flags: Error handling flags
 * @intr_mask: Interrupt Mask Bits
 * @ee_ctrl_mask: Exception event control mask
 * @eh_work: Worker to handle UFS errors that require s/w attention
 * @errors: HBA errors
 * @uic_error: UFS interconnect layer error status
 * @saved_err: sticky error mask
 * @saved_uic_err: sticky UIC error mask
 * @dev_cmd: ufs device management command information
 */
struct ufs_hba {
	void __iomem *mmio_base;

	/* Virtual memory reference */
	struct utp_transfer_cmd_desc *ucdl_base_addr;
	struct utp_transfer_req_desc *utrdl_base_addr;
	struct utp_task_req_desc *utmrdl_base_addr;

	/* DMA memory reference */
	dma_addr_t ucdl_dma_addr;
	dma_addr_t utrdl_dma_addr;
	dma_addr_t utmrdl_dma_addr;

	struct Scsi_Host *host;
	struct device *dev;

	struct ufshcd_lrb *lrb;
	volatile unsigned long lrb_in_use;

	unsigned long outstanding_tasks;
	unsigned long outstanding_reqs;

	u32 capabilities;
	int nutrs;
	int nutmrs;
	u32 ufs_version;
	unsigned int irq;

	struct uic_command *active_uic_cmd;
	struct mutex uic_cmd_mutex;
	struct mutex hibern8_mutex;

	wait_queue_head_t tm_wq;
	wait_queue_head_t tm_tag_wq;
	unsigned long tm_condition;
	unsigned long tm_slots_in_use;

	struct completion *pwr_done;
	struct completion *hibern8_done;
	unsigned int hibern8_cnt;

	u32 ufshcd_state;
	u32 eh_flags;
	u32 intr_mask;
	u32 transferred_sector;

	/* Task thread */
	struct task_struct *thread;
	struct list_head lrb_q;
	spinlock_t lrb_lock;

	/* Work Queues */
	struct work_struct eh_work;

	/* Link power management */
	struct delayed_work link_pm;
	struct mutex link_pm_mutex;
	unsigned long link_state;

	/* Performance */
	u32 tp_per_period;

	/* HBA Errors */
	u32 errors;
	u32 uic_error;
	u32 saved_err;
	u32 saved_uic_err;

	/* Device management request data */
	struct ufs_dev_cmd dev_cmd;

	/* power mode parameter */
	struct uic_pwr_mode req_pmd_parm;
	struct uic_pwr_mode act_pmd_parm;

	/* Vendor specific interface */
	const struct ufshcd_ops *ops;
	u32 quirks;

/* UFSHCI doesn't support DWORD size in UTRD */
#define UFSHCI_QUIRK_BROKEN_DWORD_UTRD		BIT(0)
#define UFSHCI_QUIRK_BROKEN_REQ_LIST_CLR	BIT(1)
#define UFSHCI_QUIRK_USE_OF_HCE			BIT(2)
};

#if defined(UFS_EXYNOS_DEBUG)
#define ufshcd_writel(hba, val, reg)	\
	writel((val), (hba)->mmio_base + (reg)); \
	printk("[hfshcd_w: 0x%04X 0x%08X]\n", (unsigned int)(reg), (unsigned int)(val))

static inline u32 ufshcd_readl(struct ufs_hba *hba, u32 reg)			\
{										\
	u32 val = readl((hba)->mmio_base + (reg)); \
	printk("[ufshcd_r:0x%04X 0x%08X]\n", (unsigned int)reg, val);\
	return val;					\
}

#define ufshcd_readl2(hba, reg)	\
	readl((hba)->mmio_base + (reg))
#define ufshcd_writel2(hba, val, reg)	\
	writel((val), (hba)->mmio_base + (reg))
	
#else
#define ufshcd_writel(hba, val, reg)	\
	writel((val), (hba)->mmio_base + (reg))
	
#define ufshcd_readl(hba, reg)	\
	readl((hba)->mmio_base + (reg))
#endif

int ufshcd_init(struct device *, struct ufs_hba ** ,
		void __iomem *, unsigned int,
		const struct ufshcd_drv_data *);
void ufshcd_remove(struct ufs_hba *);


#ifdef CONFIG_PM_SLEEP
extern int ufshcd_suspend(struct ufs_hba *hba);
extern int ufshcd_resume(struct ufs_hba *hba);
#endif

/**
 * ufshcd_hba_stop - Send controller to reset state
 * @hba: per adapter instance
 */
static inline void ufshcd_hba_stop(struct ufs_hba *hba)
{
	ufshcd_writel(hba, CONTROLLER_DISABLE,  REG_CONTROLLER_ENABLE);
}

static inline void check_upiu_size(void)
{
	BUILD_BUG_ON(ALIGNED_UPIU_SIZE <
		GENERAL_UPIU_REQUEST_SIZE + QUERY_DESC_MAX_SIZE);
}

extern int ufshcd_dme_set_attr(struct ufs_hba *hba, u32 attr_sel,
			       u8 attr_set, u32 mib_val, u8 peer);
extern int ufshcd_dme_get_attr(struct ufs_hba *hba, u32 attr_sel,
			       u32 *mib_val, u8 peer);
extern int ufshcd_dme_hibern8_ctrl(struct ufs_hba *hba, u8 enter);

extern bool ufshcd_link_check_in_use(struct ufs_hba *hba);
extern bool ufshcd_link_is_hibern8ed(struct ufs_hba *hba);
extern int ufshcd_config_max_pwr_mode(struct ufs_hba *hba);
extern void scsi_softirq_done(struct request *rq);

/* UIC command interfaces for DME primitives */
#define DME_LOCAL	0
#define DME_PEER	1
#define ATTR_SET_NOR	0	/* NORMAL */
#define ATTR_SET_ST	1	/* STATIC */

static inline int ufshcd_dme_set(struct ufs_hba *hba, u32 attr_sel,
				 u32 mib_val)
{
	return ufshcd_dme_set_attr(hba, attr_sel, ATTR_SET_NOR,
				   mib_val, DME_LOCAL);
}

static inline int ufshcd_dme_st_set(struct ufs_hba *hba, u32 attr_sel,
				    u32 mib_val)
{
	return ufshcd_dme_set_attr(hba, attr_sel, ATTR_SET_ST,
				   mib_val, DME_LOCAL);
}

static inline int ufshcd_dme_peer_set(struct ufs_hba *hba, u32 attr_sel,
				      u32 mib_val)
{
	return ufshcd_dme_set_attr(hba, attr_sel, ATTR_SET_NOR,
				   mib_val, DME_PEER);
}

static inline int ufshcd_dme_peer_st_set(struct ufs_hba *hba, u32 attr_sel,
					 u32 mib_val)
{
	return ufshcd_dme_set_attr(hba, attr_sel, ATTR_SET_ST,
				   mib_val, DME_PEER);
}

static inline int ufshcd_dme_get(struct ufs_hba *hba,
				 u32 attr_sel, u32 *mib_val)
{
	return ufshcd_dme_get_attr(hba, attr_sel, mib_val, DME_LOCAL);
}

static inline int ufshcd_dme_peer_get(struct ufs_hba *hba,
				      u32 attr_sel, u32 *mib_val)
{
	return ufshcd_dme_get_attr(hba, attr_sel, mib_val, DME_PEER);
}

static inline int ufshcd_dme_hibern8_enter(struct ufs_hba *hba)
{
	return ufshcd_dme_hibern8_ctrl(hba, true);
}

static inline int ufshcd_dme_hibern8_exit(struct ufs_hba *hba)
{
	return ufshcd_dme_hibern8_ctrl(hba, false);
}

#endif /* End of Header */