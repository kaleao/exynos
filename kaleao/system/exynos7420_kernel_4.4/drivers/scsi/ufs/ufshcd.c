/*
 * Universal Flash Storage Host controller driver Core
 *
 * This code is based on drivers/scsi/ufs/ufshcd.c
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

#include <linux/async.h>
#include <linux/kthread.h>
#include <linux/of.h>
#if defined(CONFIG_UFS_FMP_DM_CRYPT)
#include <linux/smc.h>
#endif

#include "ufshcd.h"
#include "unipro.h"
#include "ufs-exynos.h"

#define UFSHCD_ENABLE_INTRS	(UTP_TRANSFER_REQ_COMPL |\
				 UTP_TASK_REQ_COMPL |\
				 UIC_POWER_MODE |\
				 UFSHCD_HIBERNATE_MASK |\
				 UFSHCD_ERROR_MASK)
/* UIC command timeout, unit: ms */
#define UIC_CMD_TIMEOUT	500

/* NOP OUT retries waiting for NOP IN response */
#define NOP_OUT_RETRIES    10
/* Timeout after 30 msecs if NOP OUT hangs without response */
#define NOP_OUT_TIMEOUT    30 /* msecs */
/* Device initialization completion timeout, unit: ms */
#define DEV_INIT_COMPL_TIMEOUT  500

/* Query request retries */
#define QUERY_REQ_RETRIES 10
/* Query request timeout */
#define QUERY_REQ_TIMEOUT 1000 /* msec */

/* Task management command timeout */
#define TM_CMD_TIMEOUT	100 /* msecs */

/* Expose the flag value from utp_upiu_query.value */
#define MASK_QUERY_UPIU_FLAG_LOC 0xFF

/* Interrupt aggregation default timeout, unit: 40us */
#define INT_AGGR_DEF_TO	0x01

/* Link Hibernation delay, msecs */
#define LINK_H8_DELAY  5

/* UFS link setup retries */
#define UFS_LINK_SETUP_RETRIES 10

enum {
	UFSHCD_MAX_CHANNEL	= 0,
	UFSHCD_MAX_ID		= 1,
	UFSHCD_MAX_LUNS		= 8,
	UFSHCD_CMD_PER_LUN	= 32,
	UFSHCD_CAN_QUEUE	= 32,
};

/* UFSHCD states */
enum {
	UFSHCD_STATE_RESET,
	UFSHCD_STATE_ERROR,
	UFSHCD_STATE_OPERATIONAL,
};

/* UFSHCD error handling flags */
enum {
	UFSHCD_EH_IN_PROGRESS = (1 << 0),
};

/* UFSHCD UIC layer error flags */
enum {
	UFSHCD_UIC_DL_PA_INIT_ERROR = (1 << 0), /* Data link layer error */
	UFSHCD_UIC_NL_ERROR = (1 << 1), /* Network layer error */
	UFSHCD_UIC_TL_ERROR = (1 << 2), /* Transport Layer error */
	UFSHCD_UIC_DME_ERROR = (1 << 3), /* DME error */
};

/* Interrupt configuration options */
enum {
	UFSHCD_INT_DISABLE,
	UFSHCD_INT_ENABLE,
	UFSHCD_INT_CLEAR,
};

#define ufshcd_set_eh_in_progress(h) \
	(h->eh_flags |= UFSHCD_EH_IN_PROGRESS)
#define ufshcd_eh_in_progress(h) \
	(h->eh_flags & UFSHCD_EH_IN_PROGRESS)
#define ufshcd_clear_eh_in_progress(h) \
	(h->eh_flags &= ~UFSHCD_EH_IN_PROGRESS)

static void ufshcd_tmc_handler(struct ufs_hba *hba);
static void ufshcd_async_scan(void *data, async_cookie_t cookie);
static int ufshcd_reset_and_restore(struct ufs_hba *hba);
static int ufshcd_clear_tm_cmd(struct ufs_hba *hba, int tag);
static int ufshcd_read_sdev_qdepth(struct ufs_hba *hba,
				struct scsi_device *sdev);
static int ufshcd_host_reset_and_restore(struct ufs_hba *hba);
static int ufshcd_link_init(struct ufs_hba *hba);
static void ufshcd_command_done(struct request *rq);

extern void exynos_ufs_ctrl_hci_core_clk(struct exynos_ufs *ufs, bool en);

/* UFS link power state */
enum {
	LINK_CTRL_DISABLED = 0,
	LINK_CTRL_ENABLED,
	LINK_ACTIVATED,
	LINK_HIBERN8ED,
};

static void ufshcd_link_pm_reset_state(struct ufs_hba *hba)
{
	hba->link_state = LINK_CTRL_DISABLED;
	hba->hibern8_cnt = 0;
}

bool ufshcd_link_check_in_use(struct ufs_hba *hba)
{
	return (hba->lrb_in_use || hba->tm_slots_in_use);
}
EXPORT_SYMBOL_GPL(ufshcd_link_check_in_use);

bool ufshcd_link_is_hibern8ed(struct ufs_hba *hba)
{
	return test_bit(LINK_HIBERN8ED, &hba->link_state);
}
EXPORT_SYMBOL_GPL(ufshcd_link_is_hibern8ed);

#ifdef CONFIG_UFS_DYNAMIC_H8
static void ufshcd_link_pm_enable_ctrl(struct ufs_hba *hba)
{
	set_bit(LINK_CTRL_ENABLED, &hba->link_state);
	set_bit(LINK_ACTIVATED, &hba->link_state);
}

static void ufshcd_link_pm_work(struct work_struct *work)
{
	struct ufs_hba *hba = container_of(work, struct ufs_hba, link_pm.work);

	if (hba->ufshcd_state != UFSHCD_STATE_OPERATIONAL)
		return;

	if (!test_bit(LINK_CTRL_ENABLED, &hba->link_state))
		return;

	mutex_lock(&hba->link_pm_mutex);

	if (ufshcd_link_check_in_use(hba))
		goto out;

	if (test_and_clear_bit(LINK_ACTIVATED, &hba->link_state)) {
		if (!ufshcd_dme_hibern8_enter(hba))
			set_bit(LINK_HIBERN8ED, &hba->link_state);
	}

out:
	mutex_unlock(&hba->link_pm_mutex);
}

static inline void ufshcd_get_link(struct ufs_hba *hba)
{
	if (hba->ufshcd_state != UFSHCD_STATE_OPERATIONAL)
		return;

	if (!test_bit(LINK_CTRL_ENABLED, &hba->link_state))
		return;

	if (delayed_work_pending(&hba->link_pm))
		cancel_delayed_work_sync(&hba->link_pm);

	mutex_lock(&hba->link_pm_mutex);

	if (test_and_clear_bit(LINK_HIBERN8ED, &hba->link_state)) {
		if (!ufshcd_dme_hibern8_exit(hba))
			set_bit(LINK_ACTIVATED, &hba->link_state);
	}

	mutex_unlock(&hba->link_pm_mutex);
}

static inline void ufshcd_put_link(struct ufs_hba *hba)
{
	unsigned int hibern8_enter_delay = LINK_H8_DELAY;

	if (hba->ufshcd_state != UFSHCD_STATE_OPERATIONAL)
		return;
	if (!test_bit(LINK_CTRL_ENABLED, &hba->link_state))
		return;

	if (ufshcd_link_check_in_use(hba))
		return;

	if (hba->ops && hba->ops->ufshcd_get_hibern8_enter_delay)
		hibern8_enter_delay =
			hba->ops->ufshcd_get_hibern8_enter_delay(hba);

	schedule_delayed_work(&hba->link_pm,
			msecs_to_jiffies(hibern8_enter_delay));
}

static inline void ufshcd_flush_delayed_pm(struct ufs_hba *hba)
{
	flush_delayed_work(&hba->link_pm);
}

static inline void ufshcd_cancel_delayed_pm(struct ufs_hba *hba)
{
	cancel_delayed_work_sync(&hba->link_pm);
}

/**
 * ufshcd_do_link_pm  - put the link to hibernation
 * hba: per adapter instance
 * If both two doorbells(transfer and task managemt) are empty,
 * link can be put into hibernation mode. Doorbell shoud be checked
 * with UTRCS and UTMRCS.
 */
static void ufshcd_do_link_pm(struct ufs_hba *hba)
{
	if (!ufshcd_link_check_in_use(hba))
		ufshcd_put_link(hba);
}
#else
static inline void ufshcd_link_pm_enable_ctrl(struct ufs_hba *hba)
{
}

static inline void ufshcd_link_pm_work(struct work_struct *work)
{
}

static inline void ufshcd_get_link(struct ufs_hba *hba)
{
}

static inline void ufshcd_put_link(struct ufs_hba *hba)
{
}

static inline void ufshcd_flush_delayed_pm(struct ufs_hba *hba)
{
}

static inline void ufshcd_cancel_delayed_pm(struct ufs_hba *hba)
{
}

static inline void ufshcd_do_link_pm(struct ufs_hba *hba)
{
}
#endif

/*
 * ufshcd_wait_for_register - wait for register value to change
 * @hba - per-adapter interface
 * @reg - mmio register offset
 * @mask - mask to apply to read register value
 * @val - wait condition
 * @interval_us - polling interval in microsecs
 * @timeout_ms - timeout in millisecs
 *
 * Returns -ETIMEDOUT on error, zero on success
 */
static int ufshcd_wait_for_register(struct ufs_hba *hba, u32 reg, u32 mask,
		u32 val, unsigned long interval_us, unsigned long timeout_ms)
{
	int err = 0;
	unsigned long timeout = jiffies + msecs_to_jiffies(timeout_ms);

	/* ignore bits that we don't intend to wait on */
	val = val & mask;

	while ((ufshcd_readl(hba, reg) & mask) != val) {
		/* wakeup within 50us of expiry */
		usleep_range(interval_us, interval_us + 50);

		if (time_after(jiffies, timeout)) {
			if ((ufshcd_readl(hba, reg) & mask) != val)
				err = -ETIMEDOUT;
			break;
		}
	}

	return err;
}

/**
 * ufshcd_get_intr_mask - Get the interrupt bit mask
 * @hba - Pointer to adapter instance
 *
 * Returns interrupt bit mask per version
 */
static inline u32 ufshcd_get_intr_mask(struct ufs_hba *hba)
{
	if (hba->ufs_version == UFSHCI_VERSION_10)
		return INTERRUPT_MASK_ALL_VER_10;
	else
		return INTERRUPT_MASK_ALL_VER_11;
}

/**
 * ufshcd_get_ufs_version - Get the UFS version supported by the HBA
 * @hba - Pointer to adapter instance
 *
 * Returns UFSHCI version supported by the controller
 */
static inline u32 ufshcd_get_ufs_version(struct ufs_hba *hba)
{
	return ufshcd_readl(hba, REG_UFS_VERSION);
}

/**
 * ufshcd_is_device_present - Check if any device connected to
 *			      the host controller
 * @reg_hcs - host controller status register value
 *
 * Returns 1 if device present, 0 if no device detected
 */
static inline int ufshcd_is_device_present(u32 reg_hcs)
{
	return (DEVICE_PRESENT & reg_hcs) ? 1 : 0;
}

/**
 * ufshcd_get_tr_ocs - Get the UTRD Overall Command Status
 * @lrb: pointer to local command reference block
 *
 * This function is used to get the OCS field from UTRD
 * Returns the OCS field in the UTRD
 */
static inline int ufshcd_get_tr_ocs(struct ufshcd_lrb *lrbp)
{
	return le32_to_cpu(lrbp->utr_descriptor_ptr->header.dword_2) & MASK_OCS;
}

/**
 * ufshcd_get_tmr_ocs - Get the UTMRD Overall Command Status
 * @task_req_descp: pointer to utp_task_req_desc structure
 *
 * This function is used to get the OCS field from UTMRD
 * Returns the OCS field in the UTMRD
 */
static inline int
ufshcd_get_tmr_ocs(struct utp_task_req_desc *task_req_descp)
{
	return le32_to_cpu(task_req_descp->header.dword_2) & MASK_OCS;
}

/**
 * ufshcd_get_tm_free_slot - get a free slot for task management request
 * @hba: per adapter instance
 * @free_slot: pointer to variable with available slot value
 *
 * Get a free tag and lock it until ufshcd_put_tm_slot() is called.
 * Returns 0 if free slot is not available, else return 1 with tag value
 * in @free_slot.
 */
static bool ufshcd_get_tm_free_slot(struct ufs_hba *hba, int *free_slot)
{
	int tag;
	bool ret = false;

	if (!free_slot)
		goto out;

	do {
		tag = find_first_zero_bit(&hba->tm_slots_in_use, hba->nutmrs);
		if (tag >= hba->nutmrs)
			goto out;
	} while (test_and_set_bit_lock(tag, &hba->tm_slots_in_use));

	*free_slot = tag;
	ret = true;
out:
	return ret;
}

static inline void ufshcd_put_tm_slot(struct ufs_hba *hba, int slot)
{
	clear_bit_unlock(slot, &hba->tm_slots_in_use);
}

/**
 * ufshcd_utrl_clear - Clear a bit in UTRLCLR register
 * @hba: per adapter instance
 * @pos: position of the bit to be cleared
 */
static inline void ufshcd_utrl_clear(struct ufs_hba *hba, u32 pos)
{
	u32 clear;

	if (hba->quirks & UFSHCI_QUIRK_BROKEN_REQ_LIST_CLR)
		clear = (1 << pos);
	else
		clear = ~(1 << pos);

	ufshcd_writel(hba, clear, REG_UTP_TRANSFER_REQ_LIST_CLEAR);
}

/**
 * ufshcd_utmrl_clear - Clear a bit in UTRMLCLR register
 * @hba: per adapter instance
 * @pos: position of the bit to be cleared
 */
static inline void ufshcd_utmrl_clear(struct ufs_hba *hba, u32 pos)
{
	u32 clear;

	if (hba->quirks & UFSHCI_QUIRK_BROKEN_REQ_LIST_CLR)
		clear = (1 << pos);
	else
		clear = ~(1 << pos);

	ufshcd_writel(hba, clear, REG_UTP_TASK_REQ_LIST_CLEAR);
}

/**
 * ufshcd_get_lists_status - Check UCRDY, UTRLRDY and UTMRLRDY
 * @reg: Register value of host controller status
 *
 * Returns integer, 0 on Success and positive value if failed
 */
static inline int ufshcd_get_lists_status(u32 reg)
{
	/*
	 * The mask 0xFF is for the following HCS register bits
	 * Bit		Description
	 *  0		Device Present
	 *  1		UTRLRDY
	 *  2		UTMRLRDY
	 *  3		UCRDY
	 *  4		HEI
	 *  5		DEI
	 * 6-7		reserved
	 */
	return (((reg) & (0xFF)) >> 1) ^ (0x07);
}

/**
 * ufshcd_get_uic_cmd_result - Get the UIC command result
 * @hba: Pointer to adapter instance
 *
 * This function gets the result of UIC command completion
 * Returns 0 on success, non zero value on error
 */
static inline int ufshcd_get_uic_cmd_result(struct ufs_hba *hba)
{
#if defined(UFS_EXYNOS_DEBUG)
	return ufshcd_readl2(hba, REG_UIC_COMMAND_ARG_2) &
	       MASK_UIC_COMMAND_RESULT;
#else
	return ufshcd_readl(hba, REG_UIC_COMMAND_ARG_2) &
	       MASK_UIC_COMMAND_RESULT;
#endif
}

/**
 * ufshcd_get_dme_attr_val - Get the value of attribute returned by UIC command
 * @hba: Pointer to adapter instance
 *
 * This function gets UIC command argument3
 * Returns 0 on success, non zero value on error
 */
static inline u32 ufshcd_get_dme_attr_val(struct ufs_hba *hba)
{
#if defined(UFS_EXYNOS_DEBUG)
	return ufshcd_readl2(hba, REG_UIC_COMMAND_ARG_3);
#else
	return ufshcd_readl(hba, REG_UIC_COMMAND_ARG_3);
#endif
}

/**
 * ufshcd_get_req_rsp - returns the TR response transaction type
 * @ucd_rsp_ptr: pointer to response UPIU
 */
static inline int
ufshcd_get_req_rsp(struct utp_upiu_rsp *ucd_rsp_ptr)
{
	return be32_to_cpu(ucd_rsp_ptr->header.dword_0) >> 24;
}

/**
 * ufshcd_get_rsp_upiu_result - Get the result from response UPIU
 * @ucd_rsp_ptr: pointer to response UPIU
 *
 * This function gets the response status and scsi_status from response UPIU
 * Returns the response result code.
 */
static inline int
ufshcd_get_rsp_upiu_result(struct utp_upiu_rsp *ucd_rsp_ptr)
{
	return be32_to_cpu(ucd_rsp_ptr->header.dword_1) & MASK_RSP_UPIU_RESULT;
}

/*
 * ufshcd_get_rsp_upiu_data_seg_len - Get the data segment length
 *				from response UPIU
 * @ucd_rsp_ptr: pointer to response UPIU
 *
 * Return the data segment length.
 */
static inline unsigned int
ufshcd_get_rsp_upiu_data_seg_len(struct utp_upiu_rsp *ucd_rsp_ptr)
{
	return be32_to_cpu(ucd_rsp_ptr->header.dword_2) &
		MASK_RSP_UPIU_DATA_SEG_LEN;
}

/**
 * ufshcd_config_int_aggr - Configure interrupt aggregation values.
 *		Currently there is no use case where we want to configure
 *		interrupt aggregation dynamically. So to configure interrupt
 *		aggregation, #define INT_AGGR_COUNTER_THRESHOLD_VALUE and
 *		INT_AGGR_TIMEOUT_VALUE are used.
 * ufshcd_reset_intr_aggr - Reset interrupt aggregation values.
 * @hba: per adapter instance
 */
static inline void
ufshcd_reset_intr_aggr(struct ufs_hba *hba)
{
	ufshcd_writel(hba, INT_AGGR_ENABLE |
		      INT_AGGR_COUNTER_AND_TIMER_RESET,
		      REG_UTP_TRANSFER_REQ_INT_AGG_CONTROL);
}

/**
 * ufshcd_config_intr_aggr - Configure interrupt aggregation values.
 * @hba: per adapter instance
 * @cnt: Interrupt aggregation counter threshold
 * @tmout: Interrupt aggregation timeout value
 */
static inline void
ufshcd_config_intr_aggr(struct ufs_hba *hba, u8 cnt, u8 tmout)
{
	ufshcd_writel(hba, INT_AGGR_ENABLE | INT_AGGR_PARAM_WRITE |
		      INT_AGGR_COUNTER_THLD_VAL(cnt) |
		      INT_AGGR_TIMEOUT_VAL(tmout),
		      REG_UTP_TRANSFER_REQ_INT_AGG_CONTROL);
}

/**
 * ufshcd_enable_run_stop_reg - Enable run-stop registers,
 *			When run-stop registers are set to 1, it indicates the
 *			host controller that it can process the requests
 * @hba: per adapter instance
 */
static void ufshcd_enable_run_stop_reg(struct ufs_hba *hba)
{
	ufshcd_writel(hba, UTP_TASK_REQ_LIST_RUN_STOP_BIT,
		      REG_UTP_TASK_REQ_LIST_RUN_STOP);
	ufshcd_writel(hba, UTP_TRANSFER_REQ_LIST_RUN_STOP_BIT,
		      REG_UTP_TRANSFER_REQ_LIST_RUN_STOP);
}

/**
 * ufshcd_hba_start - Start controller initialization sequence
 * @hba: per adapter instance
 */
static inline void ufshcd_hba_start(struct ufs_hba *hba)
{
	ufshcd_writel(hba, CONTROLLER_ENABLE, REG_CONTROLLER_ENABLE);
}

/**
 * ufshcd_is_hba_active - Get controller state
 * @hba: per adapter instance
 *
 * Returns zero if controller is active, 1 otherwise
 */
static inline int ufshcd_is_hba_active(struct ufs_hba *hba)
{
	return (ufshcd_readl(hba, REG_CONTROLLER_ENABLE) & 0x1) ? 0 : 1;
}

/**
 * ufshcd_send_command - Send SCSI or device management commands
 * @hba: per adapter instance
 * @task_tag: Task tag of the command
 */
static inline
void ufshcd_send_command(struct ufs_hba *hba, unsigned int task_tag)
{
	__set_bit(task_tag, &hba->outstanding_reqs);
	ufshcd_writel(hba, 1 << task_tag, REG_UTP_TRANSFER_REQ_DOOR_BELL);
}

/**
 * ufshcd_copy_sense_data - Copy sense data in case of check condition
 * @lrb - pointer to local reference block
 */
static inline void ufshcd_copy_sense_data(struct ufshcd_lrb *lrbp)
{
	int len;
	if (lrbp->sense_buffer &&
	    ufshcd_get_rsp_upiu_data_seg_len(lrbp->ucd_rsp_ptr)) {
		len = be16_to_cpu(lrbp->ucd_rsp_ptr->sr.sense_data_len);
		memcpy(lrbp->sense_buffer,
			lrbp->ucd_rsp_ptr->sr.sense_data,
			min_t(int, len, SCSI_SENSE_BUFFERSIZE));
	}
}

/**
 * ufshcd_copy_query_response() - Copy the Query Response and the data
 * descriptor
 * @hba: per adapter instance
 * @lrb - pointer to local reference block
 */
static
int ufshcd_copy_query_response(struct ufs_hba *hba, struct ufshcd_lrb *lrbp)
{
	struct ufs_query_res *query_res = &hba->dev_cmd.query.response;

	memcpy(&query_res->upiu_res, &lrbp->ucd_rsp_ptr->qr, QUERY_OSF_SIZE);

	/* Get the descriptor */
	if (lrbp->ucd_rsp_ptr->qr.opcode == UPIU_QUERY_OPCODE_READ_DESC) {
		u8 *descp = (u8 *)lrbp->ucd_rsp_ptr +
				GENERAL_UPIU_REQUEST_SIZE;
		u16 resp_len;
		u16 buf_len;

		/* data segment length */
		resp_len = be32_to_cpu(lrbp->ucd_rsp_ptr->header.dword_2) &
						MASK_QUERY_DATA_SEG_LEN;
		buf_len = be16_to_cpu(hba->dev_cmd.query.request.upiu_req.length);

		if (likely(buf_len >= resp_len)) {
			memcpy(hba->dev_cmd.query.descriptor, descp, resp_len);
		} else {
			dev_warn(hba->dev,
				"%s: Response size is bigger than buffer",
				__func__);
			return -EINVAL;
		}
	}

	return 0;
}

/**
 * ufshcd_hba_capabilities - Read controller capabilities
 * @hba: per adapter instance
 */
static inline void ufshcd_hba_capabilities(struct ufs_hba *hba)
{
	hba->capabilities = ufshcd_readl(hba, REG_CONTROLLER_CAPABILITIES);

	/* nutrs and nutmrs are 0 based values */
	hba->nutrs = (hba->capabilities & MASK_TRANSFER_REQUESTS_SLOTS) + 1;
	hba->nutmrs =
	((hba->capabilities & MASK_TASK_MANAGEMENT_REQUEST_SLOTS) >> 16) + 1;
}

/**
 * ufshcd_ready_for_uic_cmd - Check if controller is ready
 *                            to accept UIC commands
 * @hba: per adapter instance
 * Return true on success, else false
 */
static inline bool ufshcd_ready_for_uic_cmd(struct ufs_hba *hba)
{
#if defined(UFS_EXYNOS_DEBUG)
	if (ufshcd_readl2(hba, REG_CONTROLLER_STATUS) & UIC_COMMAND_READY)
#else
	if (ufshcd_readl(hba, REG_CONTROLLER_STATUS) & UIC_COMMAND_READY)
#endif
		return true;
	else
		return false;
}

/**
 * ufshcd_get_upmcrs - Get the power mode change request status
 * @hba: Pointer to adapter instance
 *
 * This function gets the UPMCRS field of HCS register
 * Returns value of UPMCRS field
 */
static inline u8 ufshcd_get_upmcrs(struct ufs_hba *hba)
{
	return (ufshcd_readl(hba, REG_CONTROLLER_STATUS) >> 8) & 0x7;
}

/**
 * ufshcd_dispatch_uic_cmd - Dispatch UIC commands to unipro layers
 * @hba: per adapter instance
 * @uic_cmd: UIC command
 *
 * Mutex must be held.
 */
static inline void
ufshcd_dispatch_uic_cmd(struct ufs_hba *hba, struct uic_command *uic_cmd)
{
	WARN_ON(hba->active_uic_cmd);

	hba->active_uic_cmd = uic_cmd;

#if defined(UFS_EXYNOS_DEBUG)
	/* Write Args */
	ufshcd_writel2(hba, uic_cmd->argument1, REG_UIC_COMMAND_ARG_1);
	ufshcd_writel2(hba, uic_cmd->argument2, REG_UIC_COMMAND_ARG_2);
	ufshcd_writel2(hba, uic_cmd->argument3, REG_UIC_COMMAND_ARG_3);

	/* Write UIC Cmd */
	ufshcd_writel2(hba, uic_cmd->command & COMMAND_OPCODE_MASK,
		      REG_UIC_COMMAND);
#else
	/* Write Args */
	ufshcd_writel(hba, uic_cmd->argument1, REG_UIC_COMMAND_ARG_1);
	ufshcd_writel(hba, uic_cmd->argument2, REG_UIC_COMMAND_ARG_2);
	ufshcd_writel(hba, uic_cmd->argument3, REG_UIC_COMMAND_ARG_3);

	/* Write UIC Cmd */
	ufshcd_writel(hba, uic_cmd->command & COMMAND_OPCODE_MASK,
		      REG_UIC_COMMAND);
#endif		      
}

/**
 * ufshcd_wait_for_uic_cmd - Wait complectioin of UIC command
 * @hba: per adapter instance
 * @uic_command: UIC command
 *
 * Must be called with mutex held.
 * Returns 0 only if success.
 */
static int
ufshcd_wait_for_uic_cmd(struct ufs_hba *hba, struct uic_command *uic_cmd)
{
	int ret;
	unsigned long flags;

	if (wait_for_completion_timeout(&uic_cmd->done,
					msecs_to_jiffies(UIC_CMD_TIMEOUT)))
		ret = uic_cmd->argument2 & MASK_UIC_COMMAND_RESULT;
	else
		ret = -ETIMEDOUT;

	spin_lock_irqsave(hba->host->host_lock, flags);
	hba->active_uic_cmd = NULL;
	spin_unlock_irqrestore(hba->host->host_lock, flags);

	return ret;
}

/**
 * __ufshcd_send_uic_cmd - Send UIC commands and retrieve the result
 * @hba: per adapter instance
 * @uic_cmd: UIC command
 *
 * Identical to ufshcd_send_uic_cmd() expect mutex. Must be called
 * with mutex held.
 * Returns 0 only if success.
 */
static int
__ufshcd_send_uic_cmd(struct ufs_hba *hba, struct uic_command *uic_cmd)
{
	int ret;
	unsigned long flags;

	if (!ufshcd_ready_for_uic_cmd(hba)) {
		dev_err(hba->dev,
			"Controller not ready to accept UIC commands\n");
		return -EIO;
	}

	init_completion(&uic_cmd->done);

	spin_lock_irqsave(hba->host->host_lock, flags);
	ufshcd_dispatch_uic_cmd(hba, uic_cmd);
	spin_unlock_irqrestore(hba->host->host_lock, flags);
#if defined(UFS_EXYNOS_DEBUG)
	printk("[UIC:0x%08X 0x%08X 0x%08X 0x%08X]\n", uic_cmd->command, uic_cmd->argument1, uic_cmd->argument2, uic_cmd->argument3);
#endif
	ret = ufshcd_wait_for_uic_cmd(hba, uic_cmd);

	return ret;
}

/**
 * ufshcd_send_uic_cmd - Send UIC commands and retrieve the result
 * @hba: per adapter instance
 * @uic_cmd: UIC command
 *
 * Returns 0 only if success.
 */
static int
ufshcd_send_uic_cmd(struct ufs_hba *hba, struct uic_command *uic_cmd)
{
	int ret;

	mutex_lock(&hba->uic_cmd_mutex);
	ret = __ufshcd_send_uic_cmd(hba, uic_cmd);
	mutex_unlock(&hba->uic_cmd_mutex);

	return ret;
}

#define byte2word(b0, b1, b2, b3) 	\
		((unsigned int)(b0) << 24) | ((unsigned int)(b1) << 16) | ((unsigned int)(b2) << 8) | (b3)
#define word_in(x, c)           byte2word(((unsigned char *)(x) + 4 * c)[0], ((unsigned char *)(x) + 4 * c)[1], \
					((unsigned char *)(x) + 4 * c)[2], ((unsigned char *)(x) + 4 * c)[3])

#define CLEAR		0
#define AES_CBC		1
#define AES_XTS		2

/**
 * ufshcd_map_sg - Map scatter-gather list to prdt
 * @hba: per adapter instance
 * @lrbp - pointer to local reference block
 *
 * Returns 0 in case of success, non-zero value in case of failure
 */
static int ufshcd_map_sg(struct ufs_hba *hba, struct ufshcd_lrb *lrbp)
{
	struct ufshcd_sg_entry *prd_table;
	struct scatterlist *sg;
	struct scsi_cmnd *cmd;
	int sg_segments;
	int i;
#if defined(CONFIG_UFS_FMP_DM_CRYPT) || defined(CONFIG_UFS_FMP_ECRYPT_FS)
	unsigned int sector = 0;
	unsigned int sector_key = UFS_BYPASS_SECTOR_BEGIN;
	static unsigned int fmp_key_flag = 0;
#endif
	cmd = lrbp->cmd;
#if defined(CONFIG_UFS_FMP_DM_CRYPT)
	if (cmd->request->bio) {
		sector_key = (cmd->request->bio->bi_sensitive_data == 1)? UFS_ENCRYPTION_SECTOR_BEGIN : UFS_BYPASS_SECTOR_BEGIN;
		cmd->request->bio->bi_sensitive_data = 0;
		sector = cmd->request->bio->bi_sector;
	}
#endif
	sg_segments = scsi_dma_map(cmd);
	if (sg_segments < 0)
		return sg_segments;

	if (sg_segments) {
		if (hba->quirks & UFSHCI_QUIRK_BROKEN_DWORD_UTRD)
			lrbp->utr_descriptor_ptr->prd_table_length =
				cpu_to_le16((u16)(sg_segments *
					sizeof(struct ufshcd_sg_entry)));
		else
			lrbp->utr_descriptor_ptr->prd_table_length =
				cpu_to_le16((u16) (sg_segments));

		prd_table = (struct ufshcd_sg_entry *)lrbp->ucd_prdt_ptr;

		scsi_for_each_sg(cmd, sg, sg_segments, i) {
			hba->tp_per_period += prd_table[i].size  =
				cpu_to_le32(((u32) sg_dma_len(sg))-1);
			prd_table[i].base_addr =
				cpu_to_le32(lower_32_bits(sg->dma_address));
			prd_table[i].upper_addr =
				cpu_to_le32(upper_32_bits(sg->dma_address));
			hba->transferred_sector += prd_table[i].size;
#if defined(CONFIG_UFS_FMP_DM_CRYPT) || defined(CONFIG_UFS_FMP_ECRYPT_FS)
#if defined(CONFIG_UFS_FMP_ECRYPT_FS)
			if(test_bit(PG_sensitive_data, &sg_page(sg)->flags)) {
				sector_key |= UFS_FILE_ENCRYPTION_SECTOR_BEGIN;
			} else
				sector_key &= ~UFS_FILE_ENCRYPTION_SECTOR_BEGIN;
#endif
			if (sector_key == UFS_BYPASS_SECTOR_BEGIN) {
				SET_DAS(&prd_table[i], CLEAR);
				SET_FAS(&prd_table[i], CLEAR);
			} else {
				if (sector_key & UFS_ENCRYPTION_SECTOR_BEGIN) { /* disk encryption */
					/* disk algorithm selector  */
					SET_DAS(&prd_table[i], AES_XTS);
					prd_table[i].size |= DKL;

					/* Disk IV */
					prd_table[i].disk_iv0 = 0;
					prd_table[i].disk_iv1 = 0;
					prd_table[i].disk_iv2 = 0;
					prd_table[i].disk_iv3 = htonl(sector);

					/* Disk Enc Key, Tweak Key */
					if (!fmp_key_flag) {
						int ret = 0;
						unsigned int i;
						struct exynos_ufs *ufs = dev_get_platdata(hba->dev);

						exynos_ufs_ctrl_hci_core_clk(ufs, false);
						for (i = 0; i < 10; i++) {
							ret = exynos_smc(SMC_CMD_FMP, FMP_KEY_SET, UFS_FMP, 0);
							if (ret < 0)
								panic("failed to load FMP F/W\n");
							else if (ret) {
								dev_err(hba->dev, "failed to smc call for FMP key setting: %x\n", ret);
								return ret;
							}
						}
						exynos_ufs_ctrl_hci_core_clk(ufs, true);
						fmp_key_flag = 1;
					}

				}
				if (sector_key & UFS_FILE_ENCRYPTION_SECTOR_BEGIN) { /* file encryption */
					/* file algorithm selector*/
					SET_FAS(&prd_table[i], AES_XTS);
					prd_table[i].size |= FKL;

					/* File IV */
					prd_table[i].file_iv0 = word_in(sg_page(sg)->iv, 3);;
					prd_table[i].file_iv1 = word_in(sg_page(sg)->iv, 2);;
					prd_table[i].file_iv2 = word_in(sg_page(sg)->iv, 1);
					prd_table[i].file_iv3 = word_in(sg_page(sg)->iv, 0);

					/* File Enc Key - 256bit */
					prd_table[i].file_enckey0 = word_in(sg_page(sg)->key, 7);
					prd_table[i].file_enckey1 = word_in(sg_page(sg)->key, 6);
					prd_table[i].file_enckey2 = word_in(sg_page(sg)->key, 5);
					prd_table[i].file_enckey3 = word_in(sg_page(sg)->key, 4);
					prd_table[i].file_enckey4 = word_in(sg_page(sg)->key, 3);
					prd_table[i].file_enckey5 = word_in(sg_page(sg)->key, 2);
					prd_table[i].file_enckey6 = word_in(sg_page(sg)->key, 1);
					prd_table[i].file_enckey7 = word_in(sg_page(sg)->key, 0);

					/* File Tweak Key - 256bit */
					prd_table[i].file_twkey0 = word_in(sg_page(sg)->key, 15);
					prd_table[i].file_twkey1 = word_in(sg_page(sg)->key, 14);
					prd_table[i].file_twkey2 = word_in(sg_page(sg)->key, 13);
					prd_table[i].file_twkey3 = word_in(sg_page(sg)->key, 12);
					prd_table[i].file_twkey4 = word_in(sg_page(sg)->key, 11);
					prd_table[i].file_twkey5 = word_in(sg_page(sg)->key, 10);
					prd_table[i].file_twkey6 = word_in(sg_page(sg)->key, 9);
					prd_table[i].file_twkey7 = word_in(sg_page(sg)->key, 8);
				}
			}
			sector += UFSHCI_SECTOR_SIZE / MIN_SECTOR_SIZE;
#endif
		}
	} else {
		lrbp->utr_descriptor_ptr->prd_table_length = 0;
	}

	return 0;
}

/**
 * ufshcd_enable_intr - enable interrupts
 * @hba: per adapter instance
 * @intrs: interrupt bits
 */
static void ufshcd_enable_intr(struct ufs_hba *hba, u32 intrs)
{
	u32 set = ufshcd_readl(hba, REG_INTERRUPT_ENABLE);

	if (hba->ufs_version == UFSHCI_VERSION_10) {
		u32 rw;
		rw = set & INTERRUPT_MASK_RW_VER_10;
		set = rw | ((set ^ intrs) & intrs);
	} else {
		set |= intrs;
	}

	ufshcd_writel(hba, set, REG_INTERRUPT_ENABLE);
}

/**
 * ufshcd_disable_intr - disable interrupts
 * @hba: per adapter instance
 * @intrs: interrupt bits
 */
static void ufshcd_disable_intr(struct ufs_hba *hba, u32 intrs)
{
	u32 set = ufshcd_readl(hba, REG_INTERRUPT_ENABLE);

	if (hba->ufs_version == UFSHCI_VERSION_10) {
		u32 rw;
		rw = (set & INTERRUPT_MASK_RW_VER_10) &
			~(intrs & INTERRUPT_MASK_RW_VER_10);
		set = rw | ((set & intrs) & ~INTERRUPT_MASK_RW_VER_10);

	} else {
		set &= ~intrs;
	}

	ufshcd_writel(hba, set, REG_INTERRUPT_ENABLE);
}

/**
 * ufshcd_prepare_req_desc_hdr() - Fills the requests header
 * descriptor according to request
 * @lrbp: pointer to local reference block
 * @upiu_flags: flags required in the header
 * @cmd_dir: requests data direction
 */
static void ufshcd_prepare_req_desc_hdr(struct ufshcd_lrb *lrbp,
		u32 *upiu_flags, enum dma_data_direction cmd_dir)
{
	struct utp_transfer_req_desc *req_desc = lrbp->utr_descriptor_ptr;
	u32 data_direction;
	u32 dword_0;

	if (cmd_dir == DMA_FROM_DEVICE) {
		data_direction = UTP_DEVICE_TO_HOST;
		*upiu_flags = UPIU_CMD_FLAGS_READ;
	} else if (cmd_dir == DMA_TO_DEVICE) {
		data_direction = UTP_HOST_TO_DEVICE;
		*upiu_flags = UPIU_CMD_FLAGS_WRITE;
	} else {
		data_direction = UTP_NO_DATA_TRANSFER;
		*upiu_flags = UPIU_CMD_FLAGS_NONE;
	}

	dword_0 = data_direction | (lrbp->command_type
				<< UPIU_COMMAND_TYPE_OFFSET);
	if (lrbp->intr_cmd)
		dword_0 |= UTP_REQ_DESC_INT_CMD;

	/* Transfer request descriptor header fields */
	req_desc->header.dword_0 = cpu_to_le32(dword_0);

	/*
	 * assigning invalid value for command status. Controller
	 * updates OCS on command completion, with the command
	 * status
	 */
	req_desc->header.dword_2 =
		cpu_to_le32(OCS_INVALID_COMMAND_STATUS);
}

/**
 * ufshcd_prepare_utp_scsi_cmd_upiu() - fills the utp_transfer_req_desc,
 * for scsi commands
 * @lrbp - local reference block pointer
 * @upiu_flags - flags
 */
static
void ufshcd_prepare_utp_scsi_cmd_upiu(struct ufshcd_lrb *lrbp, u32 upiu_flags)
{
	struct utp_upiu_req *ucd_req_ptr = lrbp->ucd_req_ptr;

	/* command descriptor fields */
	ucd_req_ptr->header.dword_0 = UPIU_HEADER_DWORD(
				UPIU_TRANSACTION_COMMAND, upiu_flags,
				lrbp->lun, lrbp->task_tag);
	ucd_req_ptr->header.dword_1 = UPIU_HEADER_DWORD(
				UPIU_COMMAND_SET_TYPE_SCSI, 0, 0, 0);

	/* Total EHS length and Data segment length will be zero */
	ucd_req_ptr->header.dword_2 = 0;

	ucd_req_ptr->sc.exp_data_transfer_len =
		cpu_to_be32(lrbp->cmd->sdb.length);

	memcpy(ucd_req_ptr->sc.cdb, lrbp->cmd->cmnd,
		(min_t(unsigned short, lrbp->cmd->cmd_len, MAX_CDB_SIZE)));
}

/**
 * ufshcd_prepare_utp_query_req_upiu() - fills the utp_transfer_req_desc,
 * for query requsts
 * @hba: UFS hba
 * @lrbp: local reference block pointer
 * @upiu_flags: flags
 */
static void ufshcd_prepare_utp_query_req_upiu(struct ufs_hba *hba,
				struct ufshcd_lrb *lrbp, u32 upiu_flags)
{
	struct utp_upiu_req *ucd_req_ptr = lrbp->ucd_req_ptr;
	struct ufs_query *query = &hba->dev_cmd.query;
	u16 len = be16_to_cpu(query->request.upiu_req.length);
	u8 *descp = (u8 *)lrbp->ucd_req_ptr + GENERAL_UPIU_REQUEST_SIZE;

	/* Query request header */
	ucd_req_ptr->header.dword_0 = UPIU_HEADER_DWORD(
			UPIU_TRANSACTION_QUERY_REQ, upiu_flags,
			lrbp->lun, lrbp->task_tag);
	ucd_req_ptr->header.dword_1 = UPIU_HEADER_DWORD(
			0, query->request.query_func, 0, 0);

	/* Data segment length */
	ucd_req_ptr->header.dword_2 = UPIU_HEADER_DWORD(
			0, 0, len >> 8, (u8)len);

	/* Copy the Query Request buffer as is */
	memcpy(&ucd_req_ptr->qr, &query->request.upiu_req,
			QUERY_OSF_SIZE);

	/* Copy the Descriptor */
	if ((query->request.upiu_req.opcode == UPIU_QUERY_OPCODE_WRITE_DESC))
		memcpy(descp, query->descriptor, len);
}

static inline void ufshcd_prepare_utp_nop_upiu(struct ufshcd_lrb *lrbp)
{
	struct utp_upiu_req *ucd_req_ptr = lrbp->ucd_req_ptr;

	memset(ucd_req_ptr, 0, sizeof(struct utp_upiu_req));

	/* command descriptor fields */
	ucd_req_ptr->header.dword_0 =
		UPIU_HEADER_DWORD(
			UPIU_TRANSACTION_NOP_OUT, 0, 0, lrbp->task_tag);
}

/**
 * ufshcd_compose_upiu - form UFS Protocol Information Unit(UPIU)
 * @hba - per adapter instance
 * @lrb - pointer to local reference block
 */
static int ufshcd_compose_upiu(struct ufs_hba *hba, struct ufshcd_lrb *lrbp)
{
	u32 upiu_flags;
	int ret = 0;

	switch (lrbp->command_type) {
	case UTP_CMD_TYPE_SCSI:
		if (likely(lrbp->cmd)) {
			ufshcd_prepare_req_desc_hdr(lrbp, &upiu_flags,
					lrbp->cmd->sc_data_direction);
			ufshcd_prepare_utp_scsi_cmd_upiu(lrbp, upiu_flags);
		} else {
			ret = -EINVAL;
		}
		break;
	case UTP_CMD_TYPE_DEV_MANAGE:
		ufshcd_prepare_req_desc_hdr(lrbp, &upiu_flags, DMA_NONE);
		if (hba->dev_cmd.type == DEV_CMD_TYPE_QUERY)
			ufshcd_prepare_utp_query_req_upiu(
					hba, lrbp, upiu_flags);
		else if (hba->dev_cmd.type == DEV_CMD_TYPE_NOP)
			ufshcd_prepare_utp_nop_upiu(lrbp);
		else
			ret = -EINVAL;
		break;
	case UTP_CMD_TYPE_UFS:
		/* For UFS native command implementation */
		ret = -ENOTSUPP;
		dev_err(hba->dev, "%s: UFS native command are not supported\n",
			__func__);
		break;
	default:
		ret = -ENOTSUPP;
		dev_err(hba->dev, "%s: unknown command type: 0x%x\n",
				__func__, lrbp->command_type);
		break;
	} /* end of switch */

	return ret;
}

/**
 * ufshcd_queuecommand - main entry point for SCSI requests
 * @cmd: command from SCSI Midlayer
 * @done: call back function
 *
 * Returns 0 for success, non-zero in case of failure
 */
static int ufshcd_queuecommand(struct Scsi_Host *host, struct scsi_cmnd *cmd)
{
	struct ufshcd_lrb *lrbp;
	struct ufs_hba *hba;
	unsigned long flags;
	int tag;
	int err = 0;

	hba = shost_priv(host);

	tag = cmd->request->tag;

	spin_lock_irqsave(hba->host->host_lock, flags);
	switch (hba->ufshcd_state) {
	case UFSHCD_STATE_OPERATIONAL:
		break;
	case UFSHCD_STATE_RESET:
		err = SCSI_MLQUEUE_HOST_BUSY;
		goto out_unlock;
	case UFSHCD_STATE_ERROR:
		set_host_byte(cmd, DID_ERROR);
		cmd->scsi_done(cmd);
		goto out_unlock;
	default:
		dev_WARN_ONCE(hba->dev, 1, "%s: invalid state %d\n",
				__func__, hba->ufshcd_state);
		set_host_byte(cmd, DID_BAD_TARGET);
		cmd->scsi_done(cmd);
		goto out_unlock;
	}
	spin_unlock_irqrestore(hba->host->host_lock, flags);

	/* acquire the tag to make sure device cmds don't use it */
	if (test_and_set_bit_lock(tag, &hba->lrb_in_use)) {
		/*
		 * Dev manage command in progress, requeue the command.
		 * Requeuing the command helps in cases where the request *may*
		 * find different tag instead of waiting for dev manage command
		 * completion.
		 */
		err = SCSI_MLQUEUE_HOST_BUSY;
		goto out;
	}

	lrbp = &hba->lrb[tag];

	WARN_ON(lrbp->cmd);
	lrbp->cmd = cmd;
	lrbp->sense_bufflen = SCSI_SENSE_BUFFERSIZE;
	lrbp->sense_buffer = cmd->sense_buffer;
	lrbp->task_tag = tag;
	lrbp->lun = cmd->device->lun;
	lrbp->intr_cmd = false;
	lrbp->command_type = UTP_CMD_TYPE_SCSI;

	/* form UPIU before issuing the command */
	ufshcd_compose_upiu(hba, lrbp);
	err = ufshcd_map_sg(hba, lrbp);
	if (err) {
		lrbp->cmd = NULL;
		clear_bit_unlock(tag, &hba->lrb_in_use);
		goto out;
	}

	/* issue command to the controller */
	spin_lock_bh(&hba->lrb_lock);
	list_add_tail(&lrbp->list, &hba->lrb_q);
	spin_unlock_bh(&hba->lrb_lock);
	wake_up_process(hba->thread);
	goto out;

out_unlock:
	spin_unlock_irqrestore(hba->host->host_lock, flags);
out:
	return err;
}

static struct ufshcd_lrb *__get_lrb(struct ufs_hba *hba)
{
	struct ufshcd_lrb *lrbp = NULL;

	if (!list_empty(&hba->lrb_q)) {
		lrbp = list_entry(hba->lrb_q.next, struct ufshcd_lrb, list);
		list_del_init(&lrbp->list);
	}
	return lrbp;
}

static struct ufshcd_lrb *__get_prev_lrb(struct ufs_hba *hba)
{
	struct ufshcd_lrb *lrbp = NULL;

	if (!list_empty(&hba->lrb_q)) {
		lrbp = list_entry(hba->lrb_q.prev, struct ufshcd_lrb, list);
		list_del_init(&lrbp->list);
	}
	return lrbp;
}

static void __get_lrb_and_requeue(struct ufs_hba *hba)
{
	struct ufshcd_lrb *lrbp = NULL;
	struct scsi_cmnd *cmd;

	do {
		spin_lock_bh(&hba->lrb_lock);
		lrbp = __get_prev_lrb(hba);
		spin_unlock_bh(&hba->lrb_lock);

		cmd = lrbp ? lrbp->cmd : NULL;
		if (cmd) {
			scsi_dma_unmap(cmd);
			set_host_byte(cmd, DID_REQUEUE);
			lrbp->cmd = NULL;
			clear_bit_unlock(lrbp->task_tag, &hba->lrb_in_use);
			cmd->scsi_done(cmd);
		}
	} while (lrbp);
}

static int ufshcd_queue_handler(void *data)
{
	struct ufs_hba *hba = (struct ufs_hba *)data;
	struct ufshcd_lrb *lrbp;

	while (!kthread_should_stop()) {
		set_current_state(TASK_INTERRUPTIBLE);

		/* wake the link up */
		ufshcd_get_link(hba);

		spin_lock_bh(&hba->lrb_lock);
		lrbp = __get_lrb(hba);
		spin_unlock_bh(&hba->lrb_lock);

		if (lrbp) {
			struct scsi_cmnd *cmd = lrbp->cmd;
			int tag = cmd->request->tag;
			unsigned long flags;

			/* issue command to the controller */
			spin_lock_irqsave(hba->host->host_lock, flags);
			if (hba->ops && hba->ops->ufshcd_set_nexus_t_xfer_req)
				hba->ops->ufshcd_set_nexus_t_xfer_req(hba,
							tag, lrbp->cmd);
			ufshcd_send_command(hba, tag);
			spin_unlock_irqrestore(hba->host->host_lock, flags);
			continue;
		}
		schedule();
	}
	set_current_state(TASK_RUNNING);

	return 0;
}

static int ufshcd_compose_dev_cmd(struct ufs_hba *hba,
		struct ufshcd_lrb *lrbp, enum dev_cmd_type cmd_type, int tag)
{
	lrbp->cmd = NULL;
	lrbp->sense_bufflen = 0;
	lrbp->sense_buffer = NULL;
	lrbp->task_tag = tag;
	lrbp->lun = 0; /* device management cmd is not specific to any LUN */
	lrbp->command_type = UTP_CMD_TYPE_DEV_MANAGE;
	lrbp->intr_cmd = true; /* No interrupt aggregation */
	hba->dev_cmd.type = cmd_type;

	return ufshcd_compose_upiu(hba, lrbp);
}

static int
ufshcd_clear_cmd(struct ufs_hba *hba, int tag)
{
	int err = 0;
	unsigned long flags;
	u32 mask = 1 << tag;

	/* clear outstanding transaction before retry */
	spin_lock_irqsave(hba->host->host_lock, flags);
	ufshcd_utrl_clear(hba, tag);
	spin_unlock_irqrestore(hba->host->host_lock, flags);

	/*
	 * wait for for h/w to clear corresponding bit in door-bell.
	 * max. wait is 1 sec.
	 */
	err = ufshcd_wait_for_register(hba,
			REG_UTP_TRANSFER_REQ_DOOR_BELL,
			mask, ~mask, 1000, 1000);

	return err;
}

static int
ufshcd_check_query_response(struct ufs_hba *hba, struct ufshcd_lrb *lrbp)
{
	struct ufs_query_res *query_res = &hba->dev_cmd.query.response;

	/* Get the UPIU response */
	query_res->response = ufshcd_get_rsp_upiu_result(lrbp->ucd_rsp_ptr) >>
				UPIU_RSP_CODE_OFFSET;
	return query_res->response;
}

/**
 * ufshcd_dev_cmd_completion() - handles device management command responses
 * @hba: per adapter instance
 * @lrbp: pointer to local reference block
 */
static int
ufshcd_dev_cmd_completion(struct ufs_hba *hba, struct ufshcd_lrb *lrbp)
{
	int resp;
	int err = 0;

	resp = ufshcd_get_req_rsp(lrbp->ucd_rsp_ptr);

	switch (resp) {
	case UPIU_TRANSACTION_NOP_IN:
		if (hba->dev_cmd.type != DEV_CMD_TYPE_NOP) {
			err = -EINVAL;
			dev_err(hba->dev, "%s: unexpected response %x\n",
					__func__, resp);
		}
		break;
	case UPIU_TRANSACTION_QUERY_RSP:
		err = ufshcd_check_query_response(hba, lrbp);
		if (!err)
			err = ufshcd_copy_query_response(hba, lrbp);
		break;
	case UPIU_TRANSACTION_REJECT_UPIU:
		/* TODO: handle Reject UPIU Response */
		err = -EPERM;
		dev_err(hba->dev, "%s: Reject UPIU not fully implemented\n",
				__func__);
		break;
	default:
		err = -EINVAL;
		dev_err(hba->dev, "%s: Invalid device management cmd response: %x\n",
				__func__, resp);
		break;
	}

	return err;
}

static int ufshcd_wait_for_dev_cmd(struct ufs_hba *hba,
		struct ufshcd_lrb *lrbp, int max_timeout)
{
	int err = 0;
	unsigned long time_left;
	unsigned long flags;

	time_left = wait_for_completion_timeout(hba->dev_cmd.complete,
			msecs_to_jiffies(max_timeout));

	spin_lock_irqsave(hba->host->host_lock, flags);
	hba->dev_cmd.complete = NULL;
	if (likely(time_left)) {
		err = ufshcd_get_tr_ocs(lrbp);
		if (!err)
			err = ufshcd_dev_cmd_completion(hba, lrbp);
	}
	spin_unlock_irqrestore(hba->host->host_lock, flags);

	if (!time_left) {
		err = -ETIMEDOUT;
		if (!ufshcd_clear_cmd(hba, lrbp->task_tag)) {
			spin_lock_irqsave(hba->host->host_lock, flags);
			__clear_bit(lrbp->task_tag, &hba->outstanding_reqs);
			spin_unlock_irqrestore(hba->host->host_lock, flags);

			/* sucessfully cleared the command, retry if needed */
			err = -EAGAIN;
		}
	}

	return err;
}

/**
 * ufshcd_get_dev_cmd_tag - Get device management command tag
 * @hba: per-adapter instance
 * @tag: pointer to variable with available slot value
 *
 * Get a free slot and lock it until device management command
 * completes.
 *
 * Returns false if free slot is unavailable for locking, else
 * return true with tag value in @tag.
 */
static bool ufshcd_get_dev_cmd_tag(struct ufs_hba *hba, int *tag_out)
{
	int tag;
	bool ret = false;
	unsigned long tmp;

	if (!tag_out)
		goto out;

	do {
		tmp = ~hba->lrb_in_use;
		tag = find_last_bit(&tmp, hba->nutrs);
		if (tag >= hba->nutrs)
			goto out;
	} while (test_and_set_bit_lock(tag, &hba->lrb_in_use));

	*tag_out = tag;
	ret = true;
out:
	return ret;
}

static inline void ufshcd_put_dev_cmd_tag(struct ufs_hba *hba, int tag)
{
	clear_bit_unlock(tag, &hba->lrb_in_use);
}

/**
 * ufshcd_exec_dev_cmd - API for sending device management requests
 * @hba - UFS hba
 * @cmd_type - specifies the type (NOP, Query...)
 * @timeout - time in seconds
 *
 * NOTE: Since there is only one available tag for device management commands,
 * it is expected you hold the hba->dev_cmd.lock mutex.
 */
static int ufshcd_exec_dev_cmd(struct ufs_hba *hba,
		enum dev_cmd_type cmd_type, int timeout)
{
	struct ufshcd_lrb *lrbp;
	int err;
	int tag;
	struct completion wait;
	unsigned long flags;

	ufshcd_get_link(hba);

	/*
	 * Get free slot, sleep if slots are unavailable.
	 * Even though we use wait_event() which sleeps indefinitely,
	 * the maximum wait time is bounded by SCSI request timeout.
	 */
	wait_event(hba->dev_cmd.tag_wq, ufshcd_get_dev_cmd_tag(hba, &tag));

	init_completion(&wait);
	lrbp = &hba->lrb[tag];
	WARN_ON(lrbp->cmd);
	err = ufshcd_compose_dev_cmd(hba, lrbp, cmd_type, tag);
	if (unlikely(err))
		goto out_put_tag;

	hba->dev_cmd.complete = &wait;

	spin_lock_irqsave(hba->host->host_lock, flags);
	if (hba->ops && hba->ops->ufshcd_set_nexus_t_xfer_req)
		hba->ops->ufshcd_set_nexus_t_xfer_req(hba, tag, lrbp->cmd);
	ufshcd_send_command(hba, tag);
	spin_unlock_irqrestore(hba->host->host_lock, flags);

	err = ufshcd_wait_for_dev_cmd(hba, lrbp, timeout);

out_put_tag:
	ufshcd_put_dev_cmd_tag(hba, tag);
	wake_up(&hba->dev_cmd.tag_wq);
	ufshcd_do_link_pm(hba);
	return err;
}

int ufshcd_prep_query(struct ufs_hba *hba, enum query_opcode op,
		u8 idn, u8 idx, u8 sel, u32 *qry_val)
{
	struct ufs_query_req *request;
	struct ufs_query_res *response;

	if ((op == UPIU_QUERY_OPCODE_WRITE_ATTR) && !qry_val) {
		dev_err(hba->dev,
			"%s: query value required for opcode 0x%x\n",
			__func__, op);
		return -EINVAL;
	}

	request = &hba->dev_cmd.query.request;
	response = &hba->dev_cmd.query.response;
	memset(request, 0, sizeof(struct ufs_query_req));
	memset(response, 0, sizeof(struct ufs_query_res));

	switch (op) {
	case UPIU_QUERY_OPCODE_WRITE_ATTR:
		request->upiu_req.value = *qry_val;
	case UPIU_QUERY_OPCODE_SET_FLAG:
	case UPIU_QUERY_OPCODE_CLEAR_FLAG:
	case UPIU_QUERY_OPCODE_TOGGLE_FLAG:
	case UPIU_QUERY_OPCODE_WRITE_DESC:
		request->query_func = UPIU_QUERY_FUNC_STANDARD_WRITE_REQUEST;
		break;
	case UPIU_QUERY_OPCODE_READ_ATTR:
	case UPIU_QUERY_OPCODE_READ_FLAG:
	case UPIU_QUERY_OPCODE_READ_DESC:
		request->query_func = UPIU_QUERY_FUNC_STANDARD_READ_REQUEST;
		break;
	default:
		dev_err(hba->dev,
			"%s: unexpected query opcode 0x%.2x\n",
			__func__, op);
		return -EINVAL;
	}

	request->upiu_req.opcode = op;
	request->upiu_req.idn = idn;
	request->upiu_req.index = idx;
	request->upiu_req.selector = sel;

	return 0;
}

/**
 * ufshcd_query_flag() - API function for sending flag query requests
 * hba: per-adapter instance
 * query_opcode: flag query to perform
 * idn: flag idn to access
 * flag_res: the flag value after the query request completes
 *
 * Returns 0 for success, non-zero in case of failure
 */
static int ufshcd_query_flag(struct ufs_hba *hba, enum query_opcode opcode,
			enum flag_idn idn, bool *flag_res)
{
	struct ufs_query_res *response = &hba->dev_cmd.query.response;
	int err;

	BUG_ON(!hba);

	mutex_lock(&hba->dev_cmd.lock);

	err = ufshcd_prep_query(hba, opcode, idn, 0, 0, NULL);
	if (err)
		goto out_unlock;

	/* Send query request */
	err = ufshcd_exec_dev_cmd(hba, DEV_CMD_TYPE_QUERY,
			QUERY_REQ_TIMEOUT);
	if (err) {
		dev_err(hba->dev,
			"%s: Sending flag query for idn %d failed, err = %d\n",
			__func__, idn, err);
		goto out_unlock;
	}

	if (flag_res)
		*flag_res = (be32_to_cpu(response->upiu_res.value) &
				MASK_QUERY_UPIU_FLAG_LOC) & 0x1;

out_unlock:
	mutex_unlock(&hba->dev_cmd.lock);
	return err;
}

/*
 * ufshcd_query_attr - API function for sending attribute requests
 * hba: per-adapter instance
 * opcode: attribute opcode
 * idn: attribute idn to access
 * index: index field
 * selector: selector field
 * attr_val: the attribute value after the query request completes
 *
 * Returns 0 for success, non-zero in case of failure
 */
int ufshcd_query_attr(struct ufs_hba *hba, enum query_opcode opcode,
		u8 idn, u8 index, u8 selector, u32 *attr_val)
{
	struct ufs_query_res *response = &hba->dev_cmd.query.response;
	int err;

	BUG_ON(!hba);

	mutex_lock(&hba->dev_cmd.lock);

	err = ufshcd_prep_query(hba, opcode, idn, index, selector, attr_val);
	if (err)
		goto out_unlock;

	/* Send query request */
	err = ufshcd_exec_dev_cmd(hba, DEV_CMD_TYPE_QUERY,
			QUERY_REQ_TIMEOUT);
	if (err) {
		dev_err(hba->dev,
			"%s: opcode 0x%.2x for idn %d failed, err = %d\n",
			__func__, opcode, idn, err);
		goto out_unlock;
	}

	if (attr_val)
		*attr_val = response->upiu_res.value;

out_unlock:
	mutex_unlock(&hba->dev_cmd.lock);
	return err;
}

int ufshcd_query_descriptor(struct ufs_hba *hba,
		enum query_opcode opcode, u8 idn, u8 index,
		u8 selector, u8 *desc_buf, int *buf_len)
{
	struct ufs_query_req *request;
	struct ufs_query_res *response;
	int err;

	BUG_ON(!hba);

	if (!desc_buf || !buf_len) {
		dev_err(hba->dev,
			"%s: descriptor buffer required for opcode 0x%x\n",
			__func__, opcode);
		err = -EINVAL;
		goto out;
	}

	if (*buf_len <= QUERY_DESC_MIN_SIZE || *buf_len > QUERY_DESC_MAX_SIZE) {
		dev_err(hba->dev,
			"%s: descriptor buffer size (%d) is out of range\n",
			__func__, *buf_len);
		err = -EINVAL;
		goto out;
	}

	mutex_lock(&hba->dev_cmd.lock);

	err = ufshcd_prep_query(hba, opcode, idn, index, selector, NULL);
	if (err)
		goto out_unlock;

	request = &hba->dev_cmd.query.request;
	response = &hba->dev_cmd.query.response;

	hba->dev_cmd.query.descriptor = desc_buf;
	request->upiu_req.length = cpu_to_be16(*buf_len);

	err = ufshcd_exec_dev_cmd(hba, DEV_CMD_TYPE_QUERY, QUERY_REQ_TIMEOUT);
	if (err) {
		dev_err(hba->dev,
			"%s: opcode 0x%.2x for idn %d failed, err = %d\n",
			__func__, opcode, idn, err);
		goto out_unlock;
	}

	hba->dev_cmd.query.descriptor = NULL;
	*buf_len = be16_to_cpu(response->upiu_res.length);

out_unlock:
	mutex_unlock(&hba->dev_cmd.lock);
out:
	return err;
}

/**
 * ufshcd_memory_alloc - allocate memory for host memory space data structures
 * @hba: per adapter instance
 *
 * 1. Allocate DMA memory for Command Descriptor array
 *	Each command descriptor consist of Command UPIU, Response UPIU and PRDT
 * 2. Allocate DMA memory for UTP Transfer Request Descriptor List (UTRDL).
 * 3. Allocate DMA memory for UTP Task Management Request Descriptor List
 *	(UTMRDL)
 * 4. Allocate memory for local reference block(lrb).
 *
 * Returns 0 for success, non-zero in case of failure
 */
static int ufshcd_memory_alloc(struct ufs_hba *hba)
{
	size_t utmrdl_size, utrdl_size, ucdl_size;

	/* Allocate memory for UTP command descriptors */
	ucdl_size = (sizeof(struct utp_transfer_cmd_desc) * hba->nutrs);
	hba->ucdl_base_addr = dmam_alloc_coherent(hba->dev,
						  ucdl_size,
						  &hba->ucdl_dma_addr,
						  GFP_KERNEL);

	/*
	 * UFSHCI requires UTP command descriptor to be 128 byte aligned.
	 * make sure hba->ucdl_dma_addr is aligned to PAGE_SIZE
	 * if hba->ucdl_dma_addr is aligned to PAGE_SIZE, then it will
	 * be aligned to 128 bytes as well
	 */
	if (!hba->ucdl_base_addr ||
	    WARN_ON(hba->ucdl_dma_addr & (PAGE_SIZE - 1))) {
		dev_err(hba->dev,
			"Command Descriptor Memory allocation failed\n");
		goto out;
	}

	/*
	 * Allocate memory for UTP Transfer descriptors
	 * UFSHCI requires 1024 byte alignment of UTRD
	 */
	utrdl_size = (sizeof(struct utp_transfer_req_desc) * hba->nutrs);
	hba->utrdl_base_addr = dmam_alloc_coherent(hba->dev,
						   utrdl_size,
						   &hba->utrdl_dma_addr,
						   GFP_KERNEL);
	if (!hba->utrdl_base_addr ||
	    WARN_ON(hba->utrdl_dma_addr & (PAGE_SIZE - 1))) {
		dev_err(hba->dev,
			"Transfer Descriptor Memory allocation failed\n");
		goto out;
	}

	/*
	 * Allocate memory for UTP Task Management descriptors
	 * UFSHCI requires 1024 byte alignment of UTMRD
	 */
	utmrdl_size = sizeof(struct utp_task_req_desc) * hba->nutmrs;
	hba->utmrdl_base_addr = dmam_alloc_coherent(hba->dev,
						    utmrdl_size,
						    &hba->utmrdl_dma_addr,
						    GFP_KERNEL);
	if (!hba->utmrdl_base_addr ||
	    WARN_ON(hba->utmrdl_dma_addr & (PAGE_SIZE - 1))) {
		dev_err(hba->dev,
		"Task Management Descriptor Memory allocation failed\n");
		goto out;
	}

	/* Allocate memory for local reference block */
	hba->lrb = devm_kzalloc(hba->dev,
				hba->nutrs * sizeof(struct ufshcd_lrb),
				GFP_KERNEL);
	if (!hba->lrb) {
		dev_err(hba->dev, "LRB Memory allocation failed\n");
		goto out;
	}
	return 0;
out:
	return -ENOMEM;
}

/**
 * ufshcd_host_memory_configure - configure local reference block with
 *				memory offsets
 * @hba: per adapter instance
 *
 * Configure Host memory space
 * 1. Update Corresponding UTRD.UCDBA and UTRD.UCDBAU with UCD DMA
 * address.
 * 2. Update each UTRD with Response UPIU offset, Response UPIU length
 * and PRDT offset.
 * 3. Save the corresponding addresses of UTRD, UCD.CMD, UCD.RSP and UCD.PRDT
 * into local reference block.
 */
static void ufshcd_host_memory_configure(struct ufs_hba *hba)
{
	struct utp_transfer_cmd_desc *cmd_descp;
	struct utp_transfer_req_desc *utrdlp;
	dma_addr_t cmd_desc_dma_addr;
	dma_addr_t cmd_desc_element_addr;
	u16 response_offset;
	u16 prdt_offset;
	int cmd_desc_size;
	int i;

	utrdlp = hba->utrdl_base_addr;
	cmd_descp = hba->ucdl_base_addr;

	response_offset =
		offsetof(struct utp_transfer_cmd_desc, response_upiu);
	prdt_offset =
		offsetof(struct utp_transfer_cmd_desc, prd_table);

	cmd_desc_size = sizeof(struct utp_transfer_cmd_desc);
	cmd_desc_dma_addr = hba->ucdl_dma_addr;

	for (i = 0; i < hba->nutrs; i++) {
		/* Configure UTRD with command descriptor base address */
		cmd_desc_element_addr =
				(cmd_desc_dma_addr + (cmd_desc_size * i));
		utrdlp[i].command_desc_base_addr_lo =
				cpu_to_le32(lower_32_bits(cmd_desc_element_addr));
		utrdlp[i].command_desc_base_addr_hi =
				cpu_to_le32(upper_32_bits(cmd_desc_element_addr));

		/* Response upiu and prdt offset should be in double words */
		if (hba->quirks & UFSHCI_QUIRK_BROKEN_DWORD_UTRD) {
			utrdlp[i].response_upiu_offset =
				cpu_to_le16(response_offset);
			utrdlp[i].prd_table_offset =
				cpu_to_le16(prdt_offset);
			utrdlp[i].response_upiu_length =
				cpu_to_le16(ALIGNED_UPIU_SIZE);
		} else {
			utrdlp[i].response_upiu_offset =
				cpu_to_le16((response_offset >> 2));
			utrdlp[i].prd_table_offset =
				cpu_to_le16((prdt_offset >> 2));
			utrdlp[i].response_upiu_length =
				cpu_to_le16(ALIGNED_UPIU_SIZE >> 2);
		}

		hba->lrb[i].utr_descriptor_ptr = (utrdlp + i);
		hba->lrb[i].ucd_req_ptr =
			(struct utp_upiu_req *)(cmd_descp + i);
		hba->lrb[i].ucd_rsp_ptr =
			(struct utp_upiu_rsp *)cmd_descp[i].response_upiu;
		hba->lrb[i].ucd_prdt_ptr =
			(struct ufshcd_sg_entry *)cmd_descp[i].prd_table;
	}
}

/**
 * ufshcd_dme_link_startup - Notify Unipro to perform link startup
 * @hba: per adapter instance
 *
 * UIC_CMD_DME_LINK_STARTUP command must be issued to Unipro layer,
 * in order to initialize the Unipro link startup procedure.
 * Once the Unipro links are up, the device connected to the controller
 * is detected.
 *
 * Returns 0 on success, non-zero value on failure
 */
static int ufshcd_dme_link_startup(struct ufs_hba *hba)
{
	struct uic_command uic_cmd = {0};
	int ret;

	uic_cmd.command = UIC_CMD_DME_LINK_STARTUP;

	ret = ufshcd_send_uic_cmd(hba, &uic_cmd);
	if (ret)
		dev_err(hba->dev,
			"dme-link-startup: error code %d\n", ret);

	return ret;
}

static int ufshcd_dme_reset(struct ufs_hba *hba)
{
	struct uic_command uic_cmd = {0};
	int ret;

	uic_cmd.command = UIC_CMD_DME_RESET;
	uic_cmd.argument1 = 0x1;

	ret = ufshcd_send_uic_cmd(hba, &uic_cmd);
	if (ret)
		dev_err(hba->dev,
			"dme-reset: error code %d\n", ret);

	return ret;
}

static int ufshcd_dme_enable(struct ufs_hba *hba)
{
	struct uic_command uic_cmd = {0};
	int ret;

	uic_cmd.command = UIC_CMD_DME_ENABLE;

	ret = ufshcd_send_uic_cmd(hba, &uic_cmd);
	if (ret)
		dev_err(hba->dev,
			"dme-reset: error code %d\n", ret);

	return ret;
}

/**
 * ufshcd_dme_set_attr - UIC command for DME_SET, DME_PEER_SET
 * @hba: per adapter instance
 * @attr_sel: uic command argument1
 * @attr_set: attribute set type as uic command argument2
 * @mib_val: setting value as uic command argument3
 * @peer: indicate whether peer or local
 *
 * Returns 0 on success, non-zero value on failure
 */
int ufshcd_dme_set_attr(struct ufs_hba *hba, u32 attr_sel,
			u8 attr_set, u32 mib_val, u8 peer)
{
	struct uic_command uic_cmd = {0};
	static const char *const action[] = {
		"dme-set",
		"dme-peer-set"
	};
	const char *set = action[!!peer];
	int ret;

	uic_cmd.command = peer ?
		UIC_CMD_DME_PEER_SET : UIC_CMD_DME_SET;
	uic_cmd.argument1 = attr_sel;
	uic_cmd.argument2 = UIC_ARG_ATTR_TYPE(attr_set);
	uic_cmd.argument3 = mib_val;

	ret = ufshcd_send_uic_cmd(hba, &uic_cmd);
	if (ret)
		dev_err(hba->dev, "%s: attr-id 0x%x val 0x%x error code %d\n",
			set, UIC_GET_ATTR_ID(attr_sel), mib_val, ret);

	return ret;
}
EXPORT_SYMBOL_GPL(ufshcd_dme_set_attr);

/**
 * ufshcd_dme_get_attr - UIC command for DME_GET, DME_PEER_GET
 * @hba: per adapter instance
 * @attr_sel: uic command argument1
 * @mib_val: the value of the attribute as returned by the UIC command
 * @peer: indicate whether peer or local
 *
 * Returns 0 on success, non-zero value on failure
 */
int ufshcd_dme_get_attr(struct ufs_hba *hba, u32 attr_sel,
			u32 *mib_val, u8 peer)
{
	struct uic_command uic_cmd = {0};
	static const char *const action[] = {
		"dme-get",
		"dme-peer-get"
	};
	const char *get = action[!!peer];
	int ret;

	uic_cmd.command = peer ?
		UIC_CMD_DME_PEER_GET : UIC_CMD_DME_GET;
	uic_cmd.argument1 = attr_sel;

	ret = ufshcd_send_uic_cmd(hba, &uic_cmd);
	if (ret) {
		dev_err(hba->dev, "%s: attr-id 0x%x error code %d\n",
			get, UIC_GET_ATTR_ID(attr_sel), ret);
		goto out;
	}

	if (mib_val)
		*mib_val = uic_cmd.argument3;
out:
	return ret;
}
EXPORT_SYMBOL_GPL(ufshcd_dme_get_attr);

static int __ufshcd_dme_hibern8_ctrl(struct ufs_hba *hba, u8 enter)
{
	struct uic_command uic_cmd = {0};
	struct completion hibern8_done;
	static const char *const action[] = {
		"dme-hibernate-exit",
		"dme-hibernate-enter"
	};
	const char *hibern8 = action[!!enter];
	unsigned long flags;
	u8 status;
	int ret;

	uic_cmd.command = enter ?
		UIC_CMD_DME_HIBER_ENTER : UIC_CMD_DME_HIBER_EXIT;
	init_completion(&hibern8_done);

	spin_lock_irqsave(hba->host->host_lock, flags);
	hba->hibern8_done = &hibern8_done;
	spin_unlock_irqrestore(hba->host->host_lock, flags);

	ret = ufshcd_send_uic_cmd(hba, &uic_cmd);
	if (ret) {
		dev_err(hba->dev, "%s: error code %d\n", hibern8, ret);
		goto out;
	}

	if (!wait_for_completion_timeout(hba->hibern8_done,
					 msecs_to_jiffies(UIC_CMD_TIMEOUT))) {
		dev_err(hba->dev, "%s: timeout\n", hibern8);
		ret = -ETIMEDOUT;
		goto out;
	}

	status = ufshcd_get_upmcrs(hba);
	if (status != PWR_LOCAL) {
		dev_err(hba->dev, "%s: failed, upmcrs:0x%x\n",
			hibern8, status);
		ret = (status != PWR_OK) ? status : -1;
	}
out:
	spin_lock_irqsave(hba->host->host_lock, flags);
	hba->hibern8_done = NULL;
	spin_unlock_irqrestore(hba->host->host_lock, flags);
	return ret;
}

/**
 * ufshcd_dme_hibern8_ctrl - Wrapper function for ufshcd_dme_hibern8_ctrl
 * @hba: per adapter instance
 * @enter: indicate wherter hibernation enter or exit
 *
 * Returns 0 on success, non-zero value on failure
 */
int ufshcd_dme_hibern8_ctrl(struct ufs_hba *hba, u8 enter)
{
	int ret = 0;

	mutex_lock(&hba->hibern8_mutex);

	if (enter) {
		if (hba->hibern8_cnt++ != 0)
			goto out;
	} else {
		if (WARN_ON(!hba->hibern8_cnt))
			goto out;

		if (--hba->hibern8_cnt > 0)
			goto out;
	}

	if (hba->ops && hba->ops->ufshcd_pre_hibern8)
		hba->ops->ufshcd_pre_hibern8(hba, enter);

	ret = __ufshcd_dme_hibern8_ctrl(hba, enter);
	if (ret)
		goto out;

	if (hba->ops && hba->ops->ufshcd_post_hibern8)
		hba->ops->ufshcd_post_hibern8(hba, enter);
out:
	mutex_unlock(&hba->hibern8_mutex);

	if (ret) {
		unsigned long flags;

		spin_lock_irqsave(hba->host->host_lock, flags);
		hba->ufshcd_state = UFSHCD_STATE_RESET;
		ufshcd_set_eh_in_progress(hba);
		spin_unlock_irqrestore(hba->host->host_lock, flags);

		ufshcd_host_reset_and_restore(hba);

		spin_lock_irqsave(hba->host->host_lock, flags);
		ufshcd_clear_eh_in_progress(hba);
		spin_unlock_irqrestore(hba->host->host_lock, flags);
	}

	return ret;
}
EXPORT_SYMBOL_GPL(ufshcd_dme_hibern8_ctrl);


/**
 * ufshcd_uic_change_pwr_mode - Perform the UIC power mode chage
 *				using DME_SET primitives.
 * @hba: per adapter instance
 * @mode: powr mode value
 *
 * Returns 0 on success, non-zero value on failure
 */
int ufshcd_uic_change_pwr_mode(struct ufs_hba *hba, u8 mode)
{
	struct uic_command uic_cmd = {0};
	struct completion pwr_done;
	unsigned long flags;
	u8 status;
	int ret;

	uic_cmd.command = UIC_CMD_DME_SET;
	uic_cmd.argument1 = UIC_ARG_MIB(PA_PWRMODE);
	uic_cmd.argument3 = mode;
	init_completion(&pwr_done);

	mutex_lock(&hba->uic_cmd_mutex);

	spin_lock_irqsave(hba->host->host_lock, flags);
	hba->pwr_done = &pwr_done;
	spin_unlock_irqrestore(hba->host->host_lock, flags);
	ret = __ufshcd_send_uic_cmd(hba, &uic_cmd);
	if (ret) {
		dev_err(hba->dev,
			"pwr mode change with mode 0x%x uic error %d\n",
			mode, ret);
		goto out;
	}

	if (!wait_for_completion_timeout(hba->pwr_done,
					 msecs_to_jiffies(UIC_CMD_TIMEOUT))) {
		dev_err(hba->dev,
			"pwr mode change with mode 0x%x completion timeout\n",
			mode);
		ret = -ETIMEDOUT;
		goto out;
	}

	status = ufshcd_get_upmcrs(hba);
	if (status != PWR_LOCAL) {
		dev_err(hba->dev,
			"pwr mode change failed, host umpcrs:0x%x\n",
			status);
		ret = (status != PWR_OK) ? status : -1;
	}

	printk("%s : power mode changed - 0x%02X <<<==========================\n", __func__, mode);
out:
	spin_lock_irqsave(hba->host->host_lock, flags);
	hba->pwr_done = NULL;
	spin_unlock_irqrestore(hba->host->host_lock, flags);
	mutex_unlock(&hba->uic_cmd_mutex);
	return ret;
}

/**
 * ufshcd_config_max_pwr_mode - Set & Change power mode with
 *	maximum capability attribute information.
 * @hba: per adapter instance
 *
 * Returns 0 on success, non-zero value on failure
 */
int ufshcd_config_max_pwr_mode(struct ufs_hba *hba)
{
	struct uic_pwr_mode *req_pmd = &hba->req_pmd_parm;
	struct uic_pwr_mode *act_pmd = &hba->act_pmd_parm;
	u32 lane, l_rx = 1, l_tx = 1, gear = 1, series = 0, mode = FAST_MODE;
	bool termn = FALSE;
	int ret;

	memcpy(act_pmd, req_pmd, sizeof(struct uic_pwr_mode));

	/* Get the connected lane count */
	ufshcd_dme_get(hba, UIC_ARG_MIB(PA_CONNECTEDRXDATALANES), &l_rx);
	ufshcd_dme_get(hba, UIC_ARG_MIB(PA_CONNECTEDTXDATALANES), &l_tx);

	/* Get the power mode and gear count */
	ufshcd_dme_get(hba, UIC_ARG_MIB(PA_MAXRXHSGEAR), &gear);
	if (IS_PWR_MODE_HS(req_pmd->mode) && gear) {
		mode= req_pmd->mode;
	} else {
		ufshcd_dme_get(hba, UIC_ARG_MIB(PA_MAXRXPWMGEAR), &gear);
		mode = IS_PWR_MODE_PWM(req_pmd->mode) ?
				req_pmd->mode : SLOW_MODE;
	}

	printk("%s: <<============================\n", __func__);
	printk("%s: Read ConnectedDataLanes Tx = %d, Rx = %d\n", __func__, l_tx, l_rx);
	printk("%s: Read MaxRxHSGear = %d\n", __func__, gear);

	printk("%s: req_pmd->lane = %d\n", __func__, req_pmd->lane);
	printk("%s: req_pmd->gear = %d\n", __func__, req_pmd->gear);

	gear = min_t(u8, req_pmd->gear, gear);
	lane = min_t(u8, req_pmd->lane, min_t(u8, l_rx, l_tx));

	printk("%s: set lane = %d\n", __func__, lane);
	printk("%s: set gear = %d\n", __func__, gear);

	/* Set the count for lane */
	ufshcd_dme_set(hba, UIC_ARG_MIB(PA_ACTIVERXDATALANES), lane);
	ufshcd_dme_set(hba, UIC_ARG_MIB(PA_ACTIVETXDATALANES), lane);

	/* Set the count for gear */
	ufshcd_dme_set(hba, UIC_ARG_MIB(PA_RXGEAR), gear);
	ufshcd_dme_set(hba, UIC_ARG_MIB(PA_TXGEAR), gear);

	if (IS_PWR_MODE_HS(mode)) {
		/* Set HS_Series */
		series = (req_pmd->hs_series == 0) ? PA_HS_MODE_B : req_pmd->hs_series;
		ufshcd_dme_set(hba, UIC_ARG_MIB(PA_HSSERIES), series);

		/* Termination enable */
		termn = TRUE;
	}

	/* Termination */
	ufshcd_dme_set(hba, UIC_ARG_MIB(PA_RXTERMINATION), termn);
	ufshcd_dme_set(hba, UIC_ARG_MIB(PA_TXTERMINATION), termn);

	/* Set l2 timer */
	ufshcd_dme_set(hba,
		UIC_ARG_MIB(DL_FC0PROTTIMEOUTVAL), req_pmd->local_l2_timer[0]);
	ufshcd_dme_set(hba,
		UIC_ARG_MIB(DL_TC0REPLAYTIMEOUTVAL), req_pmd->local_l2_timer[1]);
	ufshcd_dme_set(hba,
		UIC_ARG_MIB(DL_AFC0REQTIMEOUTVAL), req_pmd->local_l2_timer[2]);
	ufshcd_dme_set(hba,
		UIC_ARG_MIB(PA_PWRMODEUSERDATA0), req_pmd->remote_l2_timer[0]);
	ufshcd_dme_set(hba,
		UIC_ARG_MIB(PA_PWRMODEUSERDATA1), req_pmd->remote_l2_timer[1]);
	ufshcd_dme_set(hba,
		UIC_ARG_MIB(PA_PWRMODEUSERDATA2), req_pmd->remote_l2_timer[2]);

	/* Save power mode parameters */
	act_pmd->lane = lane;
	act_pmd->gear = gear;
	act_pmd->mode = mode;
	act_pmd->hs_series = series;
	act_pmd->termn = termn;

	if (hba->ops && hba->ops->ufshcd_pre_prep_pmc) {
		ret = hba->ops->ufshcd_pre_prep_pmc(hba, act_pmd);
		if (ret)
			goto out;
	}

	/* Power mode change */
	ret = ufshcd_uic_change_pwr_mode(hba, mode << 4 | mode);
	if (ret)
		goto out;

	if (hba->ops && hba->ops->ufshcd_post_prep_pmc) {
		ret = hba->ops->ufshcd_post_prep_pmc(hba, act_pmd);
		if (ret)
			goto out;
	}

out:
	dev_info(hba->dev,
		"Power mode change(%d):M(%d)G(%d)L(%d)HS-series(%d)\n",
		 ret, mode, gear, lane, series);

	return ret;
}
EXPORT_SYMBOL_GPL(ufshcd_config_max_pwr_mode);

/**
 * ufshcd_complete_dev_init() - checks device readiness
 * hba: per-adapter instance
 *
 * Set fDeviceInit flag and poll until device toggles it.
 */
static int ufshcd_complete_dev_init(struct ufs_hba *hba)
{
	int retries, err = 0;
	bool flag_res = 1;
	unsigned long timeout;

	for (retries = QUERY_REQ_RETRIES; retries > 0; retries--) {
		/* Set the fDeviceInit flag */
		err = ufshcd_query_flag(hba, UPIU_QUERY_OPCODE_SET_FLAG,
					QUERY_FLAG_IDN_FDEVICEINIT, NULL);
		if (!err || err == -ETIMEDOUT)
			break;
		dev_dbg(hba->dev, "%s: error %d retrying\n", __func__, err);
	}
	if (err) {
		dev_err(hba->dev,
			"%s setting fDeviceInit flag failed with error %d\n",
			__func__, err);
		goto out;
	}

	/* Poll fDeviceInit flag to be cleared */
	timeout = jiffies + msecs_to_jiffies(DEV_INIT_COMPL_TIMEOUT);

	do {
		err = ufshcd_query_flag(hba, UPIU_QUERY_OPCODE_READ_FLAG,
					QUERY_FLAG_IDN_FDEVICEINIT, &flag_res);
		if (!flag_res)
			break;
		usleep_range(1000, 1000);
	} while (time_before(jiffies, timeout));

	if (err) {
		dev_err(hba->dev,
			"%s reading fDeviceInit flag failed with error %d\n",
			__func__, err);
		goto out;
	}

	if (flag_res) {
		dev_err(hba->dev,
			"%s fDeviceInit was not cleared by the device\n",
			__func__);
		err = -EBUSY;
	}

out:
	return err;
}

/**
 * ufshcd_make_hba_operational - Make UFS controller operational
 * @hba: per adapter instance
 *
 * To bring UFS host controller to operational state,
 * 1. Check if device is present
 * 2. Enable required interrupts
 * 3. Configure interrupt aggregation
 * 4. Program UTRL and UTMRL base addres
 * 5. Configure run-stop-registers
 *
 * Returns 0 on success, non-zero value on failure
 */
static int ufshcd_make_hba_operational(struct ufs_hba *hba)
{
	int err = 0;
	u32 reg;

	/* check if device present */
	reg = ufshcd_readl(hba, REG_CONTROLLER_STATUS);
	if (!ufshcd_is_device_present(reg)) {
		dev_err(hba->dev, "cc: Device not present\n");
		err = -ENXIO;
		goto out;
	}

	/* Enable required interrupts */
	ufshcd_enable_intr(hba, UFSHCD_ENABLE_INTRS);

	/* Configure interrupt aggregation */
	ufshcd_config_intr_aggr(hba, hba->nutrs - 1, INT_AGGR_DEF_TO);

	/* Configure UTRL and UTMRL base address registers */
	ufshcd_writel(hba, lower_32_bits(hba->utrdl_dma_addr),
			REG_UTP_TRANSFER_REQ_LIST_BASE_L);
	ufshcd_writel(hba, upper_32_bits(hba->utrdl_dma_addr),
			REG_UTP_TRANSFER_REQ_LIST_BASE_H);
	ufshcd_writel(hba, lower_32_bits(hba->utmrdl_dma_addr),
			REG_UTP_TASK_REQ_LIST_BASE_L);
	ufshcd_writel(hba, upper_32_bits(hba->utmrdl_dma_addr),
			REG_UTP_TASK_REQ_LIST_BASE_H);

	/*
	 * UCRDY, UTMRLDY and UTRLRDY bits must be 1
	 * DEI, HEI bits must be 0
	 */
	if (!(ufshcd_get_lists_status(reg))) {
		ufshcd_enable_run_stop_reg(hba);
	} else {
		dev_err(hba->dev,
			"Host controller not ready to process requests");
		err = -EIO;
		goto out;
	}

out:
	return err;
}

/**
 * __ufshcd_hba_enable - initialize the controller
 * @hba: per adapter instance
 *
 * The controller resets itself and controller firmware initialization
 * sequence kicks off. When controller is ready it will set
 * the Host Controller Enable bit to 1.
 *
 * Returns 0 on success, non-zero value on failure
 */
static int __ufshcd_hba_enable(struct ufs_hba *hba)
{
	int retry;

	/*
	 * msleep of 1 and 5 used in this function might result in msleep(20),
	 * but it was necessary to send the UFS FPGA to reset mode during
	 * development and testing of this driver. msleep can be changed to
	 * mdelay and retry count can be reduced based on the controller.
	 */
	if (!ufshcd_is_hba_active(hba)) {

		/* change controller state to "reset state" */
		ufshcd_hba_stop(hba);

		/*
		 * This delay is based on the testing done with UFS host
		 * controller FPGA. The delay can be changed based on the
		 * host controller used.
		 */
		msleep(5);
	}

	/* start controller initialization sequence */
	ufshcd_hba_start(hba);

	/*
	 * To initialize a UFS host controller HCE bit must be set to 1.
	 * During initialization the HCE bit value changes from 1->0->1.
	 * When the host controller completes initialization sequence
	 * it sets the value of HCE bit to 1. The same HCE bit is read back
	 * to check if the controller has completed initialization sequence.
	 * So without this delay the value HCE = 1, set in the previous
	 * instruction might be read back.
	 * This delay can be changed based on the controller.
	 */
	msleep(1);

	/* wait for the host controller to complete initialization */
	retry = 10;
	while (ufshcd_is_hba_active(hba)) {
		if (retry) {
			retry--;
		} else {
			dev_err(hba->dev,
				"Controller enable failed\n");
			return -EIO;
		}
		msleep(5);
	}
	return 0;
}

static int ufshcd_hba_enable(struct ufs_hba *hba)
{
	int ret;

	/* Reset the count of hibernation enter/exit */
	ufshcd_link_pm_reset_state(hba);

	if (hba->ops && hba->ops->ufshcd_host_reset)
		hba->ops->ufshcd_host_reset(hba);

	if (hba->quirks & UFSHCI_QUIRK_USE_OF_HCE) {
		/* enable UIC related interrupts */
		ufshcd_enable_intr(hba, UIC_COMMAND_COMPL);

		ret = ufshcd_dme_reset(hba);
		if (!ret)
			ret = ufshcd_dme_enable(hba);
	} else {
		ret = __ufshcd_hba_enable(hba);
	}

	if (ret)
		dev_err(hba->dev, "Host controller enable failed\n");

	return ret;
}

/**
 * ufshcd_link_startup - Initialize unipro link startup
 * @hba: per adapter instance
 *
 * Returns 0 for success, non-zero in case of failure
 */
static int ufshcd_link_startup(struct ufs_hba *hba)
{
	int ret;

	/* enable UIC related interrupts */
	ufshcd_enable_intr(hba, UIC_COMMAND_COMPL);

	if (hba->ops && hba->ops->ufshcd_dev_hw_reset)
		hba->ops->ufshcd_dev_hw_reset(hba);

	if (hba->ops && hba->ops->ufshcd_pre_link_startup)
		hba->ops->ufshcd_pre_link_startup(hba);

	ret = ufshcd_dme_link_startup(hba);
	if (ret)
		goto out;

	if (hba->ops && hba->ops->ufshcd_post_link_startup)
		hba->ops->ufshcd_post_link_startup(hba);

	ret = ufshcd_make_hba_operational(hba);

out:
	if (ret)
		dev_err(hba->dev, "link startup failed %d\n", ret);
	return ret;
}

/**
 * ufshcd_verify_dev_init() - Verify device initialization
 * @hba: per-adapter instance
 *
 * Send NOP OUT UPIU and wait for NOP IN response to check whether the
 * device Transport Protocol (UTP) layer is ready after a reset.
 * If the UTP layer at the device side is not initialized, it may
 * not respond with NOP IN UPIU within timeout of %NOP_OUT_TIMEOUT
 * and we retry sending NOP OUT for %NOP_OUT_RETRIES iterations.
 */
static int ufshcd_verify_dev_init(struct ufs_hba *hba)
{
	int err = 0;
	int retries;

	mutex_lock(&hba->dev_cmd.lock);
	for (retries = NOP_OUT_RETRIES; retries > 0; retries--) {
		err = ufshcd_exec_dev_cmd(hba, DEV_CMD_TYPE_NOP,
					       NOP_OUT_TIMEOUT);
		if (!err || err == -ETIMEDOUT)
			break;

		dev_dbg(hba->dev, "%s: error %d retrying\n", __func__, err);
	}
	mutex_unlock(&hba->dev_cmd.lock);

	if (err)
		dev_err(hba->dev, "%s: NOP OUT failed %d\n", __func__, err);
	return err;
}

/**
 * ufshcd_slave_alloc - handle initial SCSI device configurations
 * @sdev: pointer to SCSI device
 *
 * Returns success
 */
static int ufshcd_slave_alloc(struct scsi_device *sdev)
{
	struct ufs_hba *hba;
	int lun_qdepth;

	hba = shost_priv(sdev->host);
	sdev->tagged_supported = 1;

	/* Mode sense(6) is not supported by UFS, so use Mode sense(10) */
	sdev->use_10_for_ms = 1;
	scsi_set_tag_type(sdev, MSG_SIMPLE_TAG);

	/* allow SCSI layer to restart the device in case of errors */
	sdev->allow_restart = 1;

	lun_qdepth = ufshcd_read_sdev_qdepth(hba, sdev);
	if (lun_qdepth == 0 || lun_qdepth > hba->nutrs) {
		dev_info(hba->dev,
			"lun %d queue depth is %d\n", sdev->lun, lun_qdepth);
		lun_qdepth = hba->nutrs;
	} else if (lun_qdepth < 0) {
		lun_qdepth = 1;
	}

	/*
	 * Inform SCSI Midlayer that the LUN queue depth is same as the
	 * controller queue depth. If a LUN queue depth is less than the
	 * controller queue depth and if the LUN reports
	 * SAM_STAT_TASK_SET_FULL, the LUN queue depth will be adjusted
	 * with scsi_adjust_queue_depth.
	 */
	scsi_activate_tcq(sdev, lun_qdepth);

	blk_queue_softirq_done(sdev->request_queue, ufshcd_command_done);

	return 0;
}

/**
 * ufshcd_slave_destroy - remove SCSI device configurations
 * @sdev: pointer to SCSI device
 */
static void ufshcd_slave_destroy(struct scsi_device *sdev)
{
	struct ufs_hba *hba;

	hba = shost_priv(sdev->host);
	scsi_deactivate_tcq(sdev, hba->nutrs);
}

/**
 * ufshcd_task_req_compl - handle task management request completion
 * @hba: per adapter instance
 * @index: index of the completed request
 * @resp: task management service response
 *
 * Returns non-zero value on error, zero on success
 */
static int ufshcd_task_req_compl(struct ufs_hba *hba, u32 index, u8 *resp)
{
	struct utp_task_req_desc *task_req_descp;
	struct utp_upiu_task_rsp *task_rsp_upiup;
	unsigned long flags;
	int ocs_value;
	int task_result;

	spin_lock_irqsave(hba->host->host_lock, flags);

	/* Clear completed tasks from outstanding_tasks */
	__clear_bit(index, &hba->outstanding_tasks);

	task_req_descp = hba->utmrdl_base_addr;
	ocs_value = ufshcd_get_tmr_ocs(&task_req_descp[index]);

	if (ocs_value == OCS_SUCCESS) {
		task_rsp_upiup = (struct utp_upiu_task_rsp *)
				task_req_descp[index].task_rsp_upiu;
		task_result = be32_to_cpu(task_rsp_upiup->header.dword_1);
		task_result = ((task_result & MASK_TASK_RESPONSE) >> 8);
		if (resp)
			*resp = (u8)task_result;
	} else {
		dev_err(hba->dev, "%s: failed, ocs = 0x%x\n",
				__func__, ocs_value);
	}
	spin_unlock_irqrestore(hba->host->host_lock, flags);

	return ocs_value;
}

/**
 * ufshcd_adjust_lun_qdepth - Update LUN queue depth if device responds with
 *			      SAM_STAT_TASK_SET_FULL SCSI command status.
 * @cmd: pointer to SCSI command
 */
static void ufshcd_adjust_lun_qdepth(struct scsi_cmnd *cmd)
{
	struct ufs_hba *hba;
	int i;
	int lun_qdepth = 0;

	hba = shost_priv(cmd->device->host);

	/*
	 * LUN queue depth can be obtained by counting outstanding commands
	 * on the LUN.
	 */
	for (i = 0; i < hba->nutrs; i++) {
		if (test_bit(i, &hba->outstanding_reqs)) {

			/*
			 * Check if the outstanding command belongs
			 * to the LUN which reported SAM_STAT_TASK_SET_FULL.
			 */
			if (cmd->device->lun == hba->lrb[i].lun)
				lun_qdepth++;
		}
	}

	/*
	 * LUN queue depth will be total outstanding commands, except the
	 * command for which the LUN reported SAM_STAT_TASK_SET_FULL.
	 */
	scsi_adjust_queue_depth(cmd->device, MSG_SIMPLE_TAG, lun_qdepth - 1);
}

/**
 * ufshcd_scsi_cmd_status - Update SCSI command result based on SCSI status
 * @lrb: pointer to local reference block of completed command
 * @scsi_status: SCSI command status
 *
 * Returns value base on SCSI command status
 */
static inline int
ufshcd_scsi_cmd_status(struct ufshcd_lrb *lrbp, int scsi_status)
{
	int result = 0;

	switch (scsi_status) {
	case SAM_STAT_CHECK_CONDITION:
		ufshcd_copy_sense_data(lrbp);
	case SAM_STAT_GOOD:
		result |= DID_OK << 16 |
			  COMMAND_COMPLETE << 8 |
			  scsi_status;
		break;
	case SAM_STAT_TASK_SET_FULL:
		/*
		 * If a LUN reports SAM_STAT_TASK_SET_FULL, then the LUN queue
		 * depth needs to be adjusted to the exact number of
		 * outstanding commands the LUN can handle at any given time.
		 */
		ufshcd_adjust_lun_qdepth(lrbp->cmd);
	case SAM_STAT_BUSY:
	case SAM_STAT_TASK_ABORTED:
		ufshcd_copy_sense_data(lrbp);
		result |= scsi_status;
		break;
	default:
		result |= DID_ERROR << 16;
		break;
	} /* end of switch */

	return result;
}

/**
 * ufshcd_transfer_rsp_status - Get overall status of the response
 * @hba: per adapter instance
 * @lrb: pointer to local reference block of completed command
 *
 * Returns result of the command to notify SCSI midlayer
 */
static inline int
ufshcd_transfer_rsp_status(struct ufs_hba *hba, struct ufshcd_lrb *lrbp)
{
	int result = 0;
	int scsi_status;
	int ocs;

	/* overall command status of utrd */
	ocs = ufshcd_get_tr_ocs(lrbp);

	switch (ocs) {
	case OCS_SUCCESS:
	case OCS_FATAL_ERROR:
		result = ufshcd_get_req_rsp(lrbp->ucd_rsp_ptr);

		switch (result) {
		case UPIU_TRANSACTION_RESPONSE:
			/*
			 * get the response UPIU result to extract
			 * the SCSI command status
			 */
			result = ufshcd_get_rsp_upiu_result(lrbp->ucd_rsp_ptr);

			/*
			 * get the result based on SCSI status response
			 * to notify the SCSI midlayer of the command status
			 */
			scsi_status = result & MASK_SCSI_STATUS;
			result = ufshcd_scsi_cmd_status(lrbp, scsi_status);
			break;
		case UPIU_TRANSACTION_REJECT_UPIU:
			/* TODO: handle Reject UPIU Response */
			result = DID_ERROR << 16;
			dev_err(hba->dev,
				"Reject UPIU not fully implemented\n");
			break;
		default:
			result = DID_ERROR << 16;
			dev_err(hba->dev,
				"Unexpected request response code = %x\n",
				result);
			break;
		}
		break;
	case OCS_ABORTED:
		result |= DID_ABORT << 16;
		break;
	case OCS_INVALID_COMMAND_STATUS:
		result |= DID_REQUEUE << 16;
		break;
	case OCS_INVALID_CMD_TABLE_ATTR:
	case OCS_INVALID_PRDT_ATTR:
	case OCS_MISMATCH_DATA_BUF_SIZE:
	case OCS_MISMATCH_RESP_UPIU_SIZE:
	case OCS_PEER_COMM_FAILURE:
	default:
		result |= DID_ERROR << 16;
		dev_err(hba->dev,
		"OCS error from controller = %x\n", ocs);
		break;
	} /* end of switch */

	return result;
}

/**
 * ufshcd_uic_cmd_compl - handle completion of uic command
 * @hba: per adapter instance
 * @intr_status: interrupt status generated by the controller
 */
static void ufshcd_uic_cmd_compl(struct ufs_hba *hba, u32 intr_status)
{
	if ((intr_status & UIC_COMMAND_COMPL) && hba->active_uic_cmd) {
		hba->active_uic_cmd->argument2 |=
			ufshcd_get_uic_cmd_result(hba);
		hba->active_uic_cmd->argument3 =
			ufshcd_get_dme_attr_val(hba);
		complete(&hba->active_uic_cmd->done);
	}

	if ((intr_status & UIC_POWER_MODE) && hba->pwr_done)
		complete(hba->pwr_done);

	if ((intr_status & UFSHCD_HIBERNATE_MASK) && hba->hibern8_done)
		complete(hba->hibern8_done);
}

/**
 * __ufshcd_transfer_req_compl - handle SCSI and query command completion
 * @hba: per adapter instance
 */
static void __ufshcd_transfer_req_compl(struct ufs_hba *hba, int reason)
{
	struct ufshcd_lrb *lrbp;
	struct scsi_cmnd *cmd;
	unsigned long completed_reqs;
	u32 tr_doorbell;
	int result;
	int index;
	bool int_aggr_reset = false;

	tr_doorbell = ufshcd_readl(hba, REG_UTP_TRANSFER_REQ_DOOR_BELL);
	completed_reqs = tr_doorbell ^ hba->outstanding_reqs;

	for (index = 0; index < hba->nutrs; index++) {
		if (test_bit(index, &completed_reqs)) {
			lrbp = &hba->lrb[index];
			cmd = lrbp->cmd;
			/*
			 * Don't skip resetting interrupt aggregation counters
			 * if a regular command is present.
			 */
			int_aggr_reset |= !lrbp->intr_cmd;

			if (cmd) {
				result = ufshcd_transfer_rsp_status(hba, lrbp);
				cmd->result = result;
				if (reason)
					set_host_byte(cmd, reason);
				/* Mark completed command as NULL in LRB */
				lrbp->cmd = NULL;
				clear_bit_unlock(index, &hba->lrb_in_use);
				/* Do not touch lrbp after scsi done */
				cmd->scsi_done(cmd);
			} else if (lrbp->command_type ==
					UTP_CMD_TYPE_DEV_MANAGE) {
				if (hba->dev_cmd.complete)
					complete(hba->dev_cmd.complete);
			}
		} /* end of if */
	} /* end of for */

	/* clear corresponding bits of completed commands */
	hba->outstanding_reqs ^= completed_reqs;

	/* we might have free'd some tags above */
	wake_up(&hba->dev_cmd.tag_wq);
}


static inline void ufshcd_transfer_req_compl(struct ufs_hba *hba)
{
	__ufshcd_transfer_req_compl(hba, 0);
}

static void ufshcd_command_done(struct request *rq)
{
	struct scsi_cmnd *cmd = rq->special;
	scsi_dma_unmap(cmd);
	scsi_softirq_done(rq);
}

/**
 * ufshcd_err_handler - handle UFS errors that require s/w attention
 * @work: pointer to work structure
 */
static void ufshcd_err_handler(struct work_struct *work)
{
	struct ufs_hba *hba;
	unsigned long flags;
	u32 err_xfer = 0;
	u32 err_tm = 0;
	int err = 0;
	int tag;

	hba = container_of(work, struct ufs_hba, eh_work);

	spin_lock_irqsave(hba->host->host_lock, flags);
	if (hba->ufshcd_state == UFSHCD_STATE_RESET) {
		spin_unlock_irqrestore(hba->host->host_lock, flags);
		goto out;
	}

	hba->ufshcd_state = UFSHCD_STATE_RESET;
	ufshcd_set_eh_in_progress(hba);

	/* Complete requests that have door-bell cleared by h/w */
	ufshcd_transfer_req_compl(hba);
	ufshcd_tmc_handler(hba);
	spin_unlock_irqrestore(hba->host->host_lock, flags);

	/* Clear pending transfer requests */
	for_each_set_bit(tag, &hba->outstanding_reqs, hba->nutrs)
		if (ufshcd_clear_cmd(hba, tag))
			err_xfer |= 1 << tag;

	/* Clear pending task management requests */
	for_each_set_bit(tag, &hba->outstanding_tasks, hba->nutmrs)
		if (ufshcd_clear_tm_cmd(hba, tag))
			err_tm |= 1 << tag;

	/* Complete the requests that are cleared by s/w */
	spin_lock_irqsave(hba->host->host_lock, flags);
	ufshcd_transfer_req_compl(hba);
	ufshcd_tmc_handler(hba);
	spin_unlock_irqrestore(hba->host->host_lock, flags);

	/* Fatal errors need reset */
	if (err_xfer || err_tm || (hba->saved_err & INT_FATAL_ERRORS) ||
			((hba->saved_err & UIC_ERROR) &&
			 (hba->saved_uic_err & UFSHCD_UIC_DL_PA_INIT_ERROR))) {
		dev_err(hba->dev,
			"%s: saved_err:0x%x, saved_uic_err:0x%x\n",
			__func__, hba->saved_err, hba->saved_uic_err);

		err = ufshcd_reset_and_restore(hba);
		if (err) {
			dev_err(hba->dev, "%s: reset and restore failed\n",
					__func__);
			hba->ufshcd_state = UFSHCD_STATE_ERROR;
		}
		/*
		 * Inform scsi mid-layer that we did reset and allow to handle
		 * Unit Attention properly.
		 */
		scsi_report_bus_reset(hba->host, 0);
		hba->saved_err = 0;
		hba->saved_uic_err = 0;
	}
	ufshcd_clear_eh_in_progress(hba);

out:
	scsi_unblock_requests(hba->host);
}

/**
 * ufshcd_update_uic_error - check and set fatal UIC error flags.
 * @hba: per-adapter instance
 */
static void ufshcd_update_uic_error(struct ufs_hba *hba)
{
	u32 reg;

	/* PA_INIT_ERROR is fatal and needs UIC reset */
	reg = ufshcd_readl(hba, REG_UIC_ERROR_CODE_DATA_LINK_LAYER);
	if (reg & UIC_DATA_LINK_LAYER_ERROR_PA_INIT)
		hba->uic_error |= UFSHCD_UIC_DL_PA_INIT_ERROR;

	/* UIC NL/TL/DME errors needs software retry */
	reg = ufshcd_readl(hba, REG_UIC_ERROR_CODE_NETWORK_LAYER);
	if (reg)
		hba->uic_error |= UFSHCD_UIC_NL_ERROR;

	reg = ufshcd_readl(hba, REG_UIC_ERROR_CODE_TRANSPORT_LAYER);
	if (reg)
		hba->uic_error |= UFSHCD_UIC_TL_ERROR;

	reg = ufshcd_readl(hba, REG_UIC_ERROR_CODE_DME);
	if (reg)
		hba->uic_error |= UFSHCD_UIC_DME_ERROR;

	dev_dbg(hba->dev, "%s: UIC error flags = 0x%08x\n",
			__func__, hba->uic_error);
}

/**
 * ufshcd_check_errors - Check for errors that need s/w attention
 * @hba: per-adapter instance
 */
static void ufshcd_check_errors(struct ufs_hba *hba)
{
	bool queue_eh_work = false;

	if (hba->errors & INT_FATAL_ERRORS)
		queue_eh_work = true;

	if (hba->errors & UIC_ERROR) {
		hba->uic_error = 0;
		ufshcd_update_uic_error(hba);
		if (hba->uic_error)
			queue_eh_work = true;
	}

	if (queue_eh_work) {
		/* handle fatal errors only when link is functional */
		if (hba->ufshcd_state == UFSHCD_STATE_OPERATIONAL) {
			/* block commands from scsi mid-layer */
			scsi_block_requests(hba->host);

			/* transfer error masks to sticky bits */
			hba->saved_err |= hba->errors;
			hba->saved_uic_err |= hba->uic_error;

			hba->ufshcd_state = UFSHCD_STATE_ERROR;
			schedule_work(&hba->eh_work);
		}
	}
	/*
	 * if (!queue_eh_work) -
	 * Other errors are either non-fatal where host recovers
	 * itself without s/w intervention or errors that will be
	 * handled by the SCSI core layer.
	 */
}

/**
 * ufshcd_tmc_handler - handle task management function completion
 * @hba: per adapter instance
 */
static void ufshcd_tmc_handler(struct ufs_hba *hba)
{
	u32 tm_doorbell;

	tm_doorbell = ufshcd_readl(hba, REG_UTP_TASK_REQ_DOOR_BELL);
	hba->tm_condition = tm_doorbell ^ hba->outstanding_tasks;
	wake_up(&hba->tm_wq);
}

/**
 * ufshcd_sl_intr - Interrupt service routine
 * @hba: per adapter instance
 * @intr_status: contains interrupts generated by the controller
 */
static void ufshcd_sl_intr(struct ufs_hba *hba, u32 intr_status)
{
	hba->errors = UFSHCD_ERROR_MASK & intr_status;
	if (hba->errors)
		ufshcd_check_errors(hba);

	if (intr_status & UFSHCD_UIC_MASK)
		ufshcd_uic_cmd_compl(hba, intr_status);

	if (intr_status & UTP_TASK_REQ_COMPL)
		ufshcd_tmc_handler(hba);

	if (intr_status & UTP_TRANSFER_REQ_COMPL)
		ufshcd_transfer_req_compl(hba);

	if (intr_status & UFSHCD_REQ_COMPL)
		ufshcd_do_link_pm(hba);
}

/**
 * ufshcd_intr - Main interrupt service routine
 * @irq: irq number
 * @__hba: pointer to adapter instance
 *
 * Returns IRQ_HANDLED - If interrupt is valid
 *		IRQ_NONE - If invalid interrupt
 */
static irqreturn_t ufshcd_intr(int irq, void *__hba)
{
	u32 intr_status;
	irqreturn_t retval = IRQ_NONE;
	struct ufs_hba *hba = __hba;

	spin_lock(hba->host->host_lock);

#if defined(UFS_EXYNOS_DEBUG)	
	intr_status = ufshcd_readl2(hba, REG_INTERRUPT_STATUS);
#else
	intr_status = ufshcd_readl(hba, REG_INTERRUPT_STATUS);
#endif
	if (intr_status) {
#if defined(UFS_EXYNOS_DEBUG)		
		ufshcd_writel2(hba, intr_status, REG_INTERRUPT_STATUS);
#else
		ufshcd_writel(hba, intr_status, REG_INTERRUPT_STATUS);
#endif
		ufshcd_sl_intr(hba, intr_status);
		retval = IRQ_HANDLED;
	}
	spin_unlock(hba->host->host_lock);
	return retval;
}

static int ufshcd_clear_tm_cmd(struct ufs_hba *hba, int tag)
{
	int err = 0;
	u32 mask = 1 << tag;
	unsigned long flags;

	if (!test_bit(tag, &hba->outstanding_tasks))
		goto out;

	spin_lock_irqsave(hba->host->host_lock, flags);
	ufshcd_utmrl_clear(hba, tag);
	spin_unlock_irqrestore(hba->host->host_lock, flags);

	/* poll for max. 1 sec to clear door bell register by h/w */
	err = ufshcd_wait_for_register(hba,
			REG_UTP_TASK_REQ_DOOR_BELL,
			mask, 0, 1000, 1000);
out:
	return err;
}

/**
 * ufshcd_issue_tm_cmd - issues task management commands to controller
 * @hba: per adapter instance
 * @lun_id: LUN ID to which TM command is sent
 * @task_id: task ID to which the TM command is applicable
 * @tm_function: task management function opcode
 * @tm_response: task management service response return value
 *
 * Returns non-zero value on error, zero on success.
 */
static int ufshcd_issue_tm_cmd(struct ufs_hba *hba, int lun_id, int task_id,
		u8 tm_function, u8 *tm_response)
{
	struct utp_task_req_desc *task_req_descp;
	struct utp_upiu_task_req *task_req_upiup;
	struct Scsi_Host *host;
	unsigned long flags;
	int free_slot;
	int err;
	int task_tag;

	host = hba->host;

	ufshcd_get_link(hba);

	/*
	 * Get free slot, sleep if slots are unavailable.
	 * Even though we use wait_event() which sleeps indefinitely,
	 * the maximum wait time is bounded by %TM_CMD_TIMEOUT.
	 */
	wait_event(hba->tm_tag_wq, ufshcd_get_tm_free_slot(hba, &free_slot));

	spin_lock_irqsave(host->host_lock, flags);
	task_req_descp = hba->utmrdl_base_addr;
	task_req_descp += free_slot;

	/* Configure task request descriptor */
	task_req_descp->header.dword_0 = cpu_to_le32(UTP_REQ_DESC_INT_CMD);
	task_req_descp->header.dword_2 =
			cpu_to_le32(OCS_INVALID_COMMAND_STATUS);

	/* Configure task request UPIU */
	task_req_upiup =
		(struct utp_upiu_task_req *) task_req_descp->task_req_upiu;
	task_tag = hba->nutrs + free_slot;
	task_req_upiup->header.dword_0 =
		UPIU_HEADER_DWORD(UPIU_TRANSACTION_TASK_REQ, 0,
				lun_id, task_tag);
	task_req_upiup->header.dword_1 =
		UPIU_HEADER_DWORD(0, tm_function, 0, 0);

	task_req_upiup->input_param1 = cpu_to_be32(lun_id);
	task_req_upiup->input_param2 = cpu_to_be32(task_id);

	/* send command to the controller */
	if (hba->ops && hba->ops->ufshcd_set_nexus_t_task_mgmt)
		hba->ops->ufshcd_set_nexus_t_task_mgmt(hba,
					free_slot, tm_function);
	__set_bit(free_slot, &hba->outstanding_tasks);
	ufshcd_writel(hba, 1 << free_slot, REG_UTP_TASK_REQ_DOOR_BELL);

	spin_unlock_irqrestore(host->host_lock, flags);

	/* wait until the task management command is completed */
	err = wait_event_timeout(hba->tm_wq,
			test_bit(free_slot, &hba->tm_condition),
			msecs_to_jiffies(TM_CMD_TIMEOUT));
	if (!err) {
		dev_err(hba->dev, "%s: task management cmd 0x%.2x timed-out\n",
				__func__, tm_function);
		if (ufshcd_clear_tm_cmd(hba, free_slot))
			dev_WARN(hba->dev, "%s: unable clear tm cmd (slot %d) after timeout\n",
					__func__, free_slot);
		err = -ETIMEDOUT;
	} else {
		err = ufshcd_task_req_compl(hba, free_slot, tm_response);
	}

	clear_bit(free_slot, &hba->tm_condition);
	ufshcd_put_tm_slot(hba, free_slot);
	wake_up(&hba->tm_tag_wq);

	return err;
}

/**
 * ufshcd_eh_device_reset_handler - device reset handler registered to
 *                                    scsi layer.
 * @cmd: SCSI command pointer
 *
 * Returns SUCCESS/FAILED
 */
static int ufshcd_eh_device_reset_handler(struct scsi_cmnd *cmd)
{
	struct Scsi_Host *host;
	struct ufs_hba *hba;
	unsigned int tag;
	u32 pos;
	int err;
	u8 resp = 0xF;
	struct ufshcd_lrb *lrbp;
	unsigned long flags;

	host = cmd->device->host;
	hba = shost_priv(host);
	tag = cmd->request->tag;

	lrbp = &hba->lrb[tag];
	err = ufshcd_issue_tm_cmd(hba, lrbp->lun, 0, UFS_LOGICAL_RESET, &resp);
	if (err || resp != UPIU_TASK_MANAGEMENT_FUNC_COMPL) {
		if (!err)
			err = resp;
		goto out;
	}

	/* clear the commands that were pending for corresponding LUN */
	for_each_set_bit(pos, &hba->outstanding_reqs, hba->nutrs) {
		if (hba->lrb[pos].lun == lrbp->lun) {
			err = ufshcd_clear_cmd(hba, pos);
			if (err)
				break;
		}
	}
	spin_lock_irqsave(host->host_lock, flags);
	__ufshcd_transfer_req_compl(hba, DID_RESET);
	spin_unlock_irqrestore(host->host_lock, flags);
out:
	if (!err) {
		err = SUCCESS;
	} else {
		dev_err(hba->dev, "%s: failed with err %d\n", __func__, err);
		err = FAILED;
	}
	return err;
}

/**
 * ufshcd_abort - abort a specific command
 * @cmd: SCSI command pointer
 *
 * Abort the pending command in device by sending UFS_ABORT_TASK task management
 * command, and in host controller by clearing the door-bell register. There can
 * be race between controller sending the command to the device while abort is
 * issued. To avoid that, first issue UFS_QUERY_TASK to check if the command is
 * really issued and then try to abort it.
 *
 * Returns SUCCESS/FAILED
 */
static int ufshcd_abort(struct scsi_cmnd *cmd)
{
	struct Scsi_Host *host;
	struct ufs_hba *hba;
	unsigned long flags;
	unsigned int tag;
	int err = 0;
	int poll_cnt;
	u8 resp = 0xF;
	struct ufshcd_lrb *lrbp;

	host = cmd->device->host;
	hba = shost_priv(host);
	tag = cmd->request->tag;

	dev_err(hba->dev, "%s: tag:%d, cmd:0x%x\n", __func__, tag, cmd->cmnd[0]);

	/* If command is already aborted/completed, return SUCCESS */
	if (!(test_bit(tag, &hba->outstanding_reqs)))
		goto out;

	lrbp = &hba->lrb[tag];
	for (poll_cnt = 100; poll_cnt; poll_cnt--) {
		err = ufshcd_issue_tm_cmd(hba, lrbp->lun, lrbp->task_tag,
				UFS_QUERY_TASK, &resp);
		if (!err && resp == UPIU_TASK_MANAGEMENT_FUNC_SUCCEEDED) {
			/* cmd pending in the device */
			break;
		} else if (!err && resp == UPIU_TASK_MANAGEMENT_FUNC_COMPL) {
			u32 reg;

			/*
			 * cmd not pending in the device, check if it is
			 * in transition.
			 */
			reg = ufshcd_readl(hba, REG_UTP_TRANSFER_REQ_DOOR_BELL);
			if (reg & (1 << tag)) {
				/* sleep for max. 200us to stabilize */
				usleep_range(100, 200);
				continue;
			}
			/* command completed already */
			goto out;
		} else {
			if (!err)
				err = resp; /* service response error */
			goto out;
		}
	}

	if (!poll_cnt) {
		err = -EBUSY;
		goto out;
	}

	err = ufshcd_issue_tm_cmd(hba, lrbp->lun, lrbp->task_tag,
			UFS_ABORT_TASK, &resp);
	if (err || resp != UPIU_TASK_MANAGEMENT_FUNC_COMPL) {
		if (!err)
			err = resp; /* service response error */
		goto out;
	}

	err = ufshcd_clear_cmd(hba, tag);
	if (err)
		goto out;

	scsi_dma_unmap(cmd);

	spin_lock_irqsave(host->host_lock, flags);
	__clear_bit(tag, &hba->outstanding_reqs);
	hba->lrb[tag].cmd = NULL;
	spin_unlock_irqrestore(host->host_lock, flags);

	clear_bit_unlock(tag, &hba->lrb_in_use);
	wake_up(&hba->dev_cmd.tag_wq);
out:
	if (!err) {
		err = SUCCESS;
	} else {
		dev_err(hba->dev, "%s: failed with err %d\n", __func__, err);
		err = FAILED;
	}

	return err;
}

/**
 * ufshcd_host_reset_and_restore - reset and restore host controller
 * @hba: per-adapter instance
 *
 * Note that host controller reset may issue DME_RESET to
 * local and remote (device) Uni-Pro stack and the attributes
 * are reset to default state.
 *
 * Returns zero on success, non-zero on failure
 */
static int ufshcd_host_reset_and_restore(struct ufs_hba *hba)
{
	int err = 0;
	unsigned long flags;

	scsi_block_requests(hba->host);
	__get_lrb_and_requeue(hba);

	/* Reset the host controller */
	spin_lock_irqsave(hba->host->host_lock, flags);
	ufshcd_hba_stop(hba);
	spin_unlock_irqrestore(hba->host->host_lock, flags);

	/* Establish the link again and restore the device */
	err = ufshcd_link_init(hba);
	if (err)
		dev_err(hba->dev, "%s: failed with err %d\n", __func__, err);

	scsi_unblock_requests(hba->host);

	return err;
}

/**
 * ufshcd_reset_and_restore - reset and re-initialize host/device
 * @hba: per-adapter instance
 *
 * Reset and recover device, host and re-establish link. This
 * is helpful to recover the communication in fatal error conditions.
 *
 * Returns zero on success, non-zero on failure
 */
static int ufshcd_reset_and_restore(struct ufs_hba *hba)
{
	int err = 0;
	unsigned long flags;
	int tag;

	for_each_set_bit(tag, &hba->outstanding_reqs, hba->nutrs)
		ufshcd_clear_cmd(hba, tag);

	spin_lock_irqsave(hba->host->host_lock, flags);
	__ufshcd_transfer_req_compl(hba, DID_RESET);
	spin_unlock_irqrestore(hba->host->host_lock, flags);

	err = ufshcd_host_reset_and_restore(hba);

	/*
	 * After reset the door-bell might be cleared, complete
	 * outstanding requests in s/w here.
	 */
	spin_lock_irqsave(hba->host->host_lock, flags);
	__ufshcd_transfer_req_compl(hba, DID_RESET);
	ufshcd_tmc_handler(hba);
	spin_unlock_irqrestore(hba->host->host_lock, flags);

	return err;
}

/**
 * ufshcd_eh_host_reset_handler - host reset handler registered to scsi layer
 * @cmd - SCSI command pointer
 *
 * Returns SUCCESS/FAILED
 */
static int ufshcd_eh_host_reset_handler(struct scsi_cmnd *cmd)
{
	int err;
	unsigned long flags;
	struct ufs_hba *hba;

	hba = shost_priv(cmd->device->host);

	/*
	 * Check if there is any race with fatal error handling.
	 * If so, wait for it to complete. Even though fatal error
	 * handling does reset and restore in some cases, don't assume
	 * anything out of it. We are just avoiding race here.
	 */
	do {
		spin_lock_irqsave(hba->host->host_lock, flags);
		if (!(work_pending(&hba->eh_work) ||
				hba->ufshcd_state == UFSHCD_STATE_RESET))
			break;
		spin_unlock_irqrestore(hba->host->host_lock, flags);
		dev_dbg(hba->dev, "%s: reset in progress\n", __func__);
		flush_work(&hba->eh_work);
	} while (1);

	hba->ufshcd_state = UFSHCD_STATE_RESET;
	ufshcd_set_eh_in_progress(hba);
	spin_unlock_irqrestore(hba->host->host_lock, flags);

	err = ufshcd_reset_and_restore(hba);

	spin_lock_irqsave(hba->host->host_lock, flags);
	if (!err) {
		err = SUCCESS;
		hba->ufshcd_state = UFSHCD_STATE_OPERATIONAL;
	} else {
		err = FAILED;
		hba->ufshcd_state = UFSHCD_STATE_ERROR;
	}
	ufshcd_clear_eh_in_progress(hba);
	spin_unlock_irqrestore(hba->host->host_lock, flags);

	return err;
}

/**
 * ufshcd_read_sdev_qdepth - read the lun command queue depth
 * @hba: Pointer to adapter instance
 * @sdev: pointer to SCSI device
 *
 * Return in case of success the lun's queue depth else error.
 */
static int ufshcd_read_sdev_qdepth(struct ufs_hba *hba,
				struct scsi_device *sdev)
{
	int ret;
	int buff_len = UNIT_DESC_MAX_SIZE;
	u8 desc_buf[UNIT_DESC_MAX_SIZE];

	ret = ufshcd_query_descriptor(hba, UPIU_QUERY_OPCODE_READ_DESC,
			QUERY_DESC_IDN_UNIT, sdev->lun, 0, desc_buf, &buff_len);

	if (ret || (buff_len < UNIT_DESC_PARAM_LU_Q_DEPTH)) {
		dev_err(hba->dev,
			"%s:Failed reading unit descriptor. len = %d ret = %d",
			 __func__, buff_len, ret);
		if (!ret)
			ret = -EINVAL;

		goto out;
	}

	ret = desc_buf[UNIT_DESC_PARAM_LU_Q_DEPTH] & 0xFF;
out:
	return ret;
}

#include "ufs-exynos.h"

void dumpio(unsigned int addr, void __iomem *reg, int range)
{
	ulong dispcount;
	void __iomem *start_addr;
	void __iomem *max_addr;
	u32 val;
	int i;	

	start_addr = reg;	
	dispcount = 8;
	
	printk("start address : 0x%08X range = 0x%X\n", addr, range);

	addr += 96;
	start_addr = reg + 96;
	max_addr = reg + (range - 96);
	i = 0;
	printk("0x%08X: ", (unsigned int)addr);
	while(start_addr < max_addr)
	{
		val = readl(start_addr);
		i++;
		printk("0x%08X ", val);
		if (i == dispcount)
		{
			if (start_addr+4 < max_addr)
				printk("\n0x%08X: ", addr+4);
			i=0;
		}
		start_addr+=4;
		addr += 4;
	}

	printk("\n");		
}


static int ufshcd_link_init(struct ufs_hba *hba)
{
	int re_cnt = 0;
	int ret;

	printk("%s(in) \n", __func__);

retry:
	ret = ufshcd_hba_enable(hba);
	if (ret)
		goto out;

	ret = ufshcd_link_startup(hba);
	if (ret)
		goto out;

	ret = ufshcd_verify_dev_init(hba);
	if (ret)
		goto out;

	ret = ufshcd_complete_dev_init(hba);
	if (ret)
		goto out;

	ret = ufshcd_config_max_pwr_mode(hba);
	if (ret)
		goto out;

	hba->ufshcd_state = UFSHCD_STATE_OPERATIONAL;

	ufshcd_link_pm_enable_ctrl(hba);
#if 0
	{
		void __iomem *mem;
		mem = devm_ioremap_nocache(hba->dev, 0x15570000, 0x3000);
		if (mem == NULL)
		{
			printk("error mem!<<====================\n");
			return 0;
		}
		dumpio(0x15570000, mem, 0x3000);
	}
#endif
	printk("%s(out)\n", __func__);

	return 0;

out:
	if (re_cnt++ < UFS_LINK_SETUP_RETRIES) {
		struct uic_pwr_mode *req_pmd = &hba->req_pmd_parm;
		dev_err(hba->dev, "%s failed with err %d, retrying:%d\n",
			__func__, ret, re_cnt);
	
		if(re_cnt > (UFS_LINK_SETUP_RETRIES/2))
		{
			req_pmd->gear = 1;
		}
		ssleep(1);
		
		goto retry;
	}

	return ret;
}

/**
 * ufshcd_async_scan - asynchronous execution for link startup
 * @data: data pointer to pass to this function
 * @cookie: cookie data
 */
static void ufshcd_async_scan(void *data, async_cookie_t cookie)
{
	struct ufs_hba *hba = (struct ufs_hba *)data;

	if (ufshcd_link_init(hba))
		return;

	/* If we are in error handling context no need to scan the host */
	if (!ufshcd_eh_in_progress(hba)) {
		scsi_unblock_requests(hba->host);
		scsi_scan_host(hba->host);
	}
}

static struct scsi_host_template ufshcd_driver_template = {
	.module			= THIS_MODULE,
	.name			= UFSHCD,
	.proc_name		= UFSHCD,
	.queuecommand		= ufshcd_queuecommand,
	.slave_alloc		= ufshcd_slave_alloc,
	.slave_destroy		= ufshcd_slave_destroy,
	.eh_abort_handler	= ufshcd_abort,
	.eh_device_reset_handler = ufshcd_eh_device_reset_handler,
	.eh_host_reset_handler   = ufshcd_eh_host_reset_handler,
	.this_id		= -1,
	.sg_tablesize		= SG_ALL,
	.cmd_per_lun		= UFSHCD_CMD_PER_LUN,
	.can_queue		= UFSHCD_CAN_QUEUE,
};

/**
 * ufshcd_suspend - suspend power management function
 * @hba: per adapter instance
 * @state: power state
 *
 * Returns -ENOSYS
 */
int ufshcd_suspend(struct ufs_hba *hba)
{
	scsi_block_requests(hba->host);

	ufshcd_flush_delayed_pm(hba);
	ufshcd_hba_stop(hba);
	ufshcd_disable_intr(hba, hba->intr_mask);

	return 0;
}
EXPORT_SYMBOL_GPL(ufshcd_suspend);

/**
 * ufshcd_resume - resume power management function
 * @hba: per adapter instance
 *
 * Returns -ENOSYS
 */
int ufshcd_resume(struct ufs_hba *hba)
{
	async_schedule(ufshcd_async_scan, hba);

	return 0;
}
EXPORT_SYMBOL_GPL(ufshcd_resume);

/**
 * ufshcd_remove - de-allocate SCSI host and host memory space
 *		data structure memory
 * @hba - per adapter instance
 */
void ufshcd_remove(struct ufs_hba *hba)
{
	kthread_stop(hba->thread);
	scsi_remove_host(hba->host);

	ufshcd_cancel_delayed_pm(hba);

	/* disable interrupts */
	ufshcd_disable_intr(hba, hba->intr_mask);
	ufshcd_hba_stop(hba);

	scsi_host_put(hba->host);
}
EXPORT_SYMBOL_GPL(ufshcd_remove);

/**
 * ufshcd_set_dma_mask - Set dma mask based on the controller
 *			addressing capability
 * @hba: per adapter instance
 *
 * Returns 0 for success, non-zero for failure
 */
static int ufshcd_set_dma_mask(struct ufs_hba *hba)
{
	if (hba->capabilities & MASK_64_ADDRESSING_SUPPORT) {
		if (!dma_set_mask_and_coherent(hba->dev, DMA_BIT_MASK(64)))
			return 0;
	}
	return dma_set_mask_and_coherent(hba->dev, DMA_BIT_MASK(32));
}

static int ufshcd_parse_dt(struct ufs_hba *hba)
{
	struct device *dev = hba->dev;
	struct device_node *np = dev->of_node;
	struct uic_pwr_mode *pmd = &hba->req_pmd_parm;
	const char *str = NULL;

	if (!of_property_read_string(np, "ufs,pmd-attr-mode", &str)) {
		if (!strncmp(str, "FAST", sizeof("FAST")))
			pmd->mode = FAST_MODE;
		else if (!strncmp(str, "SLOW", sizeof("SLOW")))
			pmd->mode = SLOW_MODE;
		else if (!strncmp(str, "FAST_auto", sizeof("FAST_auto")))
			pmd->mode = FASTAUTO_MODE;
		else if (!strncmp(str, "SLOW_auto", sizeof("SLOW_auto")))
			pmd->mode = SLOWAUTO_MODE;
		else
			pmd->mode = FAST_MODE;
	} else {
		pmd->mode = FAST_MODE;
	}

	if (of_property_read_u8(np, "ufs,pmd-attr-lane", &pmd->lane))
		pmd->lane = 1;

	if (of_property_read_u8(np, "ufs,pmd-attr-gear", &pmd->gear))
		pmd->gear = 1;

	if (IS_PWR_MODE_HS(pmd->mode)) {
		if (!of_property_read_string(np, "ufs,pmd-attr-hs-series", &str)) {
			if (!strncmp(str, "HS_rate_b", sizeof("HS_rate_b")))
				pmd->hs_series = PA_HS_MODE_B;
			else if (!strncmp(str, "HS_rate_a", sizeof("HS_rate_a")))
				pmd->hs_series = PA_HS_MODE_A;
			else
				pmd->hs_series = PA_HS_MODE_A;
		} else {
			pmd->hs_series = PA_HS_MODE_A;
		}
	}

	if (of_property_read_u32_array(
		np, "ufs,pmd-local-l2-timer", pmd->local_l2_timer, 3)) {
		pmd->local_l2_timer[0] = FC0PROTTIMEOUTVAL;
		pmd->local_l2_timer[1] = TC0REPLAYTIMEOUTVAL;
		pmd->local_l2_timer[2] = AFC0REQTIMEOUTVAL;
	}

	if (of_property_read_u32_array(
		np, "ufs,pmd-remote-l2-timer", pmd->remote_l2_timer, 3)) {
		pmd->remote_l2_timer[0] = FC0PROTTIMEOUTVAL;
		pmd->remote_l2_timer[1] = TC0REPLAYTIMEOUTVAL;
		pmd->remote_l2_timer[2] = AFC0REQTIMEOUTVAL;
	}

	return 0;
}

/**
 * ufshcd_init - Driver initialization routine
 * @dev: pointer to device handle
 * @hba_handle: driver private handle
 * @mmio_base: base register address
 * @irq: Interrupt line of device
 * Returns 0 on success, non-zero value on failure
 */
int ufshcd_init(struct device *dev, struct ufs_hba **hba_handle,
		 void __iomem *mmio_base, unsigned int irq,
		 const struct ufshcd_drv_data *drv_data)
{
	struct Scsi_Host *host;
	struct ufs_hba *hba;
	int err;

	if (!dev) {
		dev_err(dev,
		"Invalid memory reference for dev is NULL\n");
		err = -ENODEV;
		goto out_error;
	}

	if (!mmio_base) {
		dev_err(dev,
		"Invalid memory reference for mmio_base is NULL\n");
		err = -ENODEV;
		goto out_error;
	}

	host = scsi_host_alloc(&ufshcd_driver_template,
				sizeof(struct ufs_hba));
	if (!host) {
		dev_err(dev, "scsi_host_alloc failed\n");
		err = -ENOMEM;
		goto out_error;
	}
	hba = shost_priv(host);
	hba->host = host;
	hba->dev = dev;
	hba->mmio_base = mmio_base;
	hba->irq = irq;

	if (drv_data) {
		if (drv_data->ops)
			hba->ops = drv_data->ops;
		hba->quirks = drv_data->quirks;
	}

	/* Read capabilities registers */
	ufshcd_hba_capabilities(hba);

	/* Get UFS version supported by the controller */
	hba->ufs_version = ufshcd_get_ufs_version(hba);

	/* Get Interrupt bit mask per version */
	hba->intr_mask = ufshcd_get_intr_mask(hba);

	err = ufshcd_set_dma_mask(hba);
	if (err) {
		dev_err(hba->dev, "set dma mask failed\n");
		goto out_disable;
	}

	/* Allocate memory for host memory space */
	err = ufshcd_memory_alloc(hba);
	if (err) {
		dev_err(hba->dev, "Memory allocation failed\n");
		goto out_disable;
	}

	/* Skip pre-CI */
#ifdef CONFIG_SCSI_SKIP_CACHE_OP
	scsi_dma_set_skip_cpu_sync();
#endif

	/* Configure LRB */
	ufshcd_host_memory_configure(hba);

	host->can_queue = hba->nutrs;
	host->cmd_per_lun = hba->nutrs;
	host->max_id = UFSHCD_MAX_ID;
	host->max_lun = UFSHCD_MAX_LUNS;
	host->max_channel = UFSHCD_MAX_CHANNEL;
	host->unique_id = host->host_no;
	host->max_cmd_len = MAX_CDB_SIZE;

	/* Parse DT */
	ufshcd_parse_dt(hba);

	/* Initailize wait queue for task management */
	init_waitqueue_head(&hba->tm_wq);
	init_waitqueue_head(&hba->tm_tag_wq);

	/* Initialize work queues */
	INIT_WORK(&hba->eh_work, ufshcd_err_handler);
	INIT_DELAYED_WORK(&hba->link_pm, ufshcd_link_pm_work);

	/* Initialize UIC command mutex */
	mutex_init(&hba->uic_cmd_mutex);

	mutex_init(&hba->hibern8_mutex);

	/* Initialize mutex for device management commands */
	mutex_init(&hba->dev_cmd.lock);

	/* Initialize mutex for link power management */
	mutex_init(&hba->link_pm_mutex);

	/* Initialize device management tag acquire wait queue */
	init_waitqueue_head(&hba->dev_cmd.tag_wq);

	/* IRQ registration */
	err = devm_request_irq(dev, irq, ufshcd_intr, IRQF_SHARED, UFSHCD, hba);
	if (err) {
		dev_err(hba->dev, "request irq failed\n");
		goto out_disable;
	}

	/* Enable SCSI tag mapping */
	err = scsi_init_shared_tag_map(host, host->can_queue);
	if (err) {
		dev_err(hba->dev, "init shared queue failed\n");
		goto out_disable;
	}

	err = scsi_add_host(host, hba->dev);
	if (err) {
		dev_err(hba->dev, "scsi_add_host failed\n");
		goto out_disable;
	}

	INIT_LIST_HEAD(&hba->lrb_q);
	spin_lock_init(&hba->lrb_lock);

	hba->thread = kthread_run(ufshcd_queue_handler, hba, "ufshcdq");
	if (IS_ERR(hba->thread)) {
		err = PTR_ERR(hba->thread);
		goto out_remove_scsi_host;
	}

	*hba_handle = hba;

	async_schedule(ufshcd_async_scan, hba);

	return 0;

out_remove_scsi_host:
	scsi_remove_host(hba->host);
out_disable:
	scsi_host_put(host);
out_error:
	return err;
}
EXPORT_SYMBOL_GPL(ufshcd_init);

MODULE_AUTHOR("Santosh Yaragnavi <santosh.sy@samsung.com>");
MODULE_AUTHOR("Vinayak Holikatti <h.vinayak@samsung.com>");
MODULE_DESCRIPTION("Generic UFS host controller driver Core");
MODULE_LICENSE("GPL");
MODULE_VERSION(UFSHCD_DRIVER_VERSION);
