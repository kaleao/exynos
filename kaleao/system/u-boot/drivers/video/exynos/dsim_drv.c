/*
 * Samsung SoC MIPI-DSIM driver.
 *
 * Copyright (c) 2014 Samsung Electronics
 *
 * Sewoon Park <seuni.park@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <common.h>
#include <malloc.h>

#include <asm/arch/gpio.h>
#include <asm/arch/pinmux.h>
#include <asm/arch/mipi_dsim.h>

#include "dsim.h"
#include "regs-dsim.h"
#include "decon.h"
#include "decon_config.h"
#include "./panels/lcd_ctrl.h"

#define MIPI_CMD_TIMEOUT 50

int decon_log_level = 5;

struct dsim_device *dsim0_for_decon;
EXPORT_SYMBOL(dsim0_for_decon);

struct dsim_device *dsim1_for_decon;
EXPORT_SYMBOL(dsim1_for_decon);

static int dsim_get_interrupt_src(struct dsim_device *dsim, u32 reg_id, unsigned int timeout)
{
	unsigned long delay_time = 100;
	unsigned int reg = dsim_read(dsim->id, DSIM_INTSRC);

	do {
		reg = dsim_read(dsim->id, DSIM_INTSRC);
		if (reg & reg_id)
			break;
		timeout--;
		udelay(delay_time);
	} while (timeout);

	if (!timeout) {
		dsim_err("timeout!! wait for DSIM interrupt (0x%lx)\n", reg);
		return -EBUSY;
	}

	dsim_reg_clear_int(dsim->id, reg);

	return 0;
}

static void dsim_long_data_wr(struct dsim_device *dsim, unsigned long data0, unsigned int data1)
{
	unsigned int data_cnt = 0, payload = 0;

	/* in case that data count is more then 4 */
	for (data_cnt = 0; data_cnt < data1; data_cnt += 4) {
		/*
		 * after sending 4bytes per one time,
		 * send remainder data less then 4.
		 */
		if ((data1 - data_cnt) < 4) {
			if ((data1 - data_cnt) == 3) {
				payload = *(u8 *)(data0 + data_cnt) |
					(*(u8 *)(data0 + (data_cnt + 1))) << 8 |
					(*(u8 *)(data0 + (data_cnt + 2))) << 16;
				dsim_dbg("count = 3 payload = %x, %x %x %x\n",
						payload, *(u8 *)(data0 + data_cnt),
						*(u8 *)(data0 + (data_cnt + 1)),
						*(u8 *)(data0 + (data_cnt + 2)));
			} else if ((data1 - data_cnt) == 2) {
				payload = *(u8 *)(data0 + data_cnt) |
					(*(u8 *)(data0 + (data_cnt + 1))) << 8;
				dsim_dbg("count = 2 payload = %x, %x %x\n", payload,
						*(u8 *)(data0 + data_cnt),
						*(u8 *)(data0 + (data_cnt + 1)));
			} else if ((data1 - data_cnt) == 1) {
				payload = *(u8 *)(data0 + data_cnt);
			}

			dsim_reg_wr_tx_payload(dsim->id, payload);
			/* send 4bytes per one time. */
		} else {
			payload = *(u8 *)(data0 + data_cnt) |
				(*(u8 *)(data0 + (data_cnt + 1))) << 8 |
				(*(u8 *)(data0 + (data_cnt + 2))) << 16 |
				(*(u8 *)(data0 + (data_cnt + 3))) << 24;

			dsim_dbg("count = 4 payload = %x, %x %x %x %x\n",
					payload, *(u8 *)(data0 + data_cnt),
					*(u8 *)(data0 + (data_cnt + 1)),
					*(u8 *)(data0 + (data_cnt + 2)),
					*(u8 *)(data0 + (data_cnt + 3)));

			dsim_reg_wr_tx_payload(dsim->id, payload);
		}
	}
}

int dsim_write_data(struct dsim_device *dsim, unsigned int data_id,
	unsigned long data0, unsigned int data1)
{
	int ret = 0;

	if (dsim->state != DSIM_STATE_HSCLKEN) {
		dsim_err("DSIM is not ready. state(%d)\n", dsim->state);
		return -EINVAL;
	}

	switch (data_id) {
		/* short packet types of packet types for command. */
		case MIPI_DSI_GENERIC_SHORT_WRITE_0_PARAM:
		case MIPI_DSI_GENERIC_SHORT_WRITE_1_PARAM:
		case MIPI_DSI_GENERIC_SHORT_WRITE_2_PARAM:
		case MIPI_DSI_DCS_SHORT_WRITE:
		case MIPI_DSI_DCS_SHORT_WRITE_PARAM:
		case MIPI_DSI_SET_MAXIMUM_RETURN_PACKET_SIZE:
			/* general command */
		case MIPI_DSI_COLOR_MODE_OFF:
		case MIPI_DSI_COLOR_MODE_ON:
		case MIPI_DSI_SHUTDOWN_PERIPHERAL:
		case MIPI_DSI_TURN_ON_PERIPHERAL:
			dsim_reg_clear_int_all(dsim->id);
			dsim_reg_wr_tx_header(dsim->id, data_id, data0, data1);
			if (dsim_get_interrupt_src(dsim, DSIM_INTSRC_SFR_PH_FIFO_EMPTY,
						MIPI_CMD_TIMEOUT) < 0) {
				dsim_err("MIPI DSIM (id=%d) write Timeout! 0x%lX\n",
						data_id, data0);
				ret = -ETIMEDOUT;
				goto exit;
			}
			break;

			/* packet types for video data */
		case MIPI_DSI_V_SYNC_START:
		case MIPI_DSI_V_SYNC_END:
		case MIPI_DSI_H_SYNC_START:
		case MIPI_DSI_H_SYNC_END:
		case MIPI_DSI_END_OF_TRANSMISSION:
			break;

			/* short and response packet types for command */
		case MIPI_DSI_GENERIC_READ_REQUEST_0_PARAM:
		case MIPI_DSI_GENERIC_READ_REQUEST_1_PARAM:
		case MIPI_DSI_GENERIC_READ_REQUEST_2_PARAM:
		case MIPI_DSI_DCS_READ:
			dsim_reg_clear_int_all(dsim->id);
			dsim_reg_wr_tx_header(dsim->id, data_id, data0, data1);
			if (dsim_get_interrupt_src(dsim, DSIM_INTSRC_SFR_PH_FIFO_EMPTY,
						MIPI_CMD_TIMEOUT) < 0) {
				dsim_err("MIPI DSIM (id=%d) write Timeout! 0x%lX\n",
						data_id, data0);
				ret = -ETIMEDOUT;
				goto exit;
			}
			break;

			/* long packet type and null packet */
		case MIPI_DSI_NULL_PACKET:
		case MIPI_DSI_BLANKING_PACKET:
			break;

		case MIPI_DSI_GENERIC_LONG_WRITE:
		case MIPI_DSI_DCS_LONG_WRITE:
			{
				unsigned int size;

				size = data1 * 4;
				dsim_reg_clear_int(dsim->id, DSIM_INTSRC_SFR_PL_FIFO_EMPTY);
				/* if data count is less then 4, then send 3bytes data.  */
				if (data1 < 4) {
					unsigned int payload = 0;
					payload = *(u8 *)(data0) |
						*(u8 *)(data0 + 1) << 8 |
						*(u8 *)(data0 + 2) << 16;

					dsim_reg_wr_tx_payload(dsim->id, payload);

					dsim_dbg("count = %d payload = %x,%x %x %x\n",
							data1, payload,
							*(u8 *)(data0),
							*(u8 *)(data0 + 1),
							*(u8 *)(data0 + 2));
					/* in case that data count is more then 4 */
				} else
					dsim_long_data_wr(dsim, data0, data1);

				/* put data into header fifo */
				dsim_reg_wr_tx_header(dsim->id, data_id, data1 & 0xff, (data1 & 0xff00) >> 8);
				if (dsim_get_interrupt_src(dsim, DSIM_INTSRC_SFR_PL_FIFO_EMPTY,
							MIPI_CMD_TIMEOUT) < 0) {
					dsim_err("MIPI DSIM (id=%d) write Timeout! 0x%lX\n",
							data_id, data0);
					ret = -ETIMEDOUT;
					goto exit;
				}
				break;
			}

			/* packet typo for video data */
		case MIPI_DSI_PACKED_PIXEL_STREAM_16:
		case MIPI_DSI_PACKED_PIXEL_STREAM_18:
		case MIPI_DSI_PIXEL_STREAM_3BYTE_18:
		case MIPI_DSI_PACKED_PIXEL_STREAM_24:
			break;
		default:
			dsim_info("data id %x is not supported current DSI spec.\n", data_id);
			ret = -EINVAL;
			goto exit;
	}

exit:
	if ((dsim->state == DSIM_STATE_HSCLKEN) && (ret == -ETIMEDOUT)) {
		dsim_err("0x%08X, 0x%08X, 0x%08X, 0x%08X\n",
				readl(dsim->reg_base + DSIM_STATUS),
				readl(dsim->reg_base + DSIM_INTSRC),
				readl(dsim->reg_base + DSIM_FIFOCTRL),
				readl(dsim->reg_base + DSIM_MULTI_PKT));
		dsim_reg_set_fifo_ctrl(dsim->id, DSIM_FIFOCTRL_INIT_SFR);
	}

	return ret;
}

int dsim_read_data(struct dsim_device *dsim, u32 data_id,
	 unsigned long addr, u32 count, u8 *buf)
{
	u32 rx_fifo, rx_size = 0;
	int i, j, ret = 0;

	if (dsim->state != DSIM_STATE_HSCLKEN) {
		dsim_err("DSIM is not ready. state(%d)\n", dsim->state);
		return -EINVAL;
	}

	/* Set the maximum packet size returned */
	dsim_write_data(dsim,
			MIPI_DSI_SET_MAXIMUM_RETURN_PACKET_SIZE, count, 0);

	/* Read request */
	dsim_write_data(dsim, data_id, addr, 0);
	if (dsim_get_interrupt_src(dsim, DSIM_INTSRC_RX_DAT_DONE,
				MIPI_CMD_TIMEOUT) < 0) {
		dsim_err("MIPI DSIM (id=%d) read Timeout! 0x%lX\n",
				data_id, addr);
		ret = -ETIMEDOUT;
		goto exit;
	}

	rx_fifo = readl(dsim->reg_base + DSIM_RXFIFO);

	/* Parse the RX packet data types */
	switch (rx_fifo & 0xff) {
		case MIPI_DSI_RX_END_OF_TRANSMISSION:
			dsim_dbg("EoTp was received from LCD module.\n");
			break;
		case MIPI_DSI_RX_DCS_SHORT_READ_RESPONSE_1BYTE:
		case MIPI_DSI_RX_DCS_SHORT_READ_RESPONSE_2BYTE:
		case MIPI_DSI_RX_GENERIC_SHORT_READ_RESPONSE_1BYTE:
		case MIPI_DSI_RX_GENERIC_SHORT_READ_RESPONSE_2BYTE:
			dsim_dbg("Short Packet was received from LCD module.\n");
			for (i = 0; i <= count; i++)
				buf[i] = (rx_fifo >> (8 + i * 8)) & 0xff;
			break;
		case MIPI_DSI_RX_DCS_LONG_READ_RESPONSE:
		case MIPI_DSI_RX_GENERIC_LONG_READ_RESPONSE:
			dsim_dbg("Long Packet was received from LCD module.\n");
			rx_size = (rx_fifo & 0x00ffff00) >> 8;
			dsim_dbg("rx fifo : %8x, response : %x, rx_size : %d\n",
					rx_fifo, rx_fifo & 0xff, rx_size);
			/* Read data from RX packet payload */
			for (i = 0; i < rx_size >> 2; i++) {
				rx_fifo = readl(dsim->reg_base + DSIM_RXFIFO);
				for (j = 0; j < 4; j++)
					buf[(i*4)+j] = (u8)(rx_fifo >> (j * 8)) & 0xff;
			}
			if (rx_size % 4) {
				rx_fifo = readl(dsim->reg_base + DSIM_RXFIFO);
				for (j = 0; j < rx_size % 4; j++)
					buf[4 * i + j] =
						(u8)(rx_fifo >> (j * 8)) & 0xff;
			}
			break;
		default:
			dsim_err("Packet format is invaild.\n");
			goto rx_error;
	}

	rx_fifo = readl(dsim->reg_base + DSIM_RXFIFO);
	if (rx_fifo != DSIM_RX_FIFO_READ_DONE) {
		dsim_info("%s Can't find RX FIFO READ DONE FLAG : %x\n",
				__func__, rx_fifo);
		goto clear_rx_fifo;
	}
	ret = rx_size;
	goto exit;

clear_rx_fifo:
	i = 0;
	while (1) {
		rx_fifo = readl(dsim->reg_base + DSIM_RXFIFO);
		if ((rx_fifo == DSIM_RX_FIFO_READ_DONE) ||
				(i > DSIM_MAX_RX_FIFO))
			break;
		dsim_info("%s clear rx fifo : %08x\n", __func__, rx_fifo);
		i++;
	}
	ret = 0;
	goto exit;

rx_error:
	dsim_reg_force_dphy_stop_state(dsim->id, 1);
	dsim_reg_force_dphy_stop_state(dsim->id, 0);
	ret = -EPERM;
	goto exit;

exit:
	return ret;
}

static void dsim_d_phy_onoff(struct dsim_device *dsim,
	unsigned int enable)
{
	call_config_ops(dsim, set_mipi_phy, enable);
}

static void dsim_device_to_clks(struct dsim_device *dsim,
		struct dsim_clks *clks)
{
	clks->hs_clk = dsim->lcd_info->hs_clk;
	clks->esc_clk = dsim->lcd_info->esc_clk;
}

static void clks_to_dsim_device(struct dsim_clks *clks,
		struct dsim_device *dsim)
{
	dsim->hs_clk = clks->hs_clk;
	dsim->escape_clk = clks->esc_clk;
	dsim->byte_clk = clks->byte_clk;
}

static int dsim_get_gpios(struct dsim_device *dsim)
{
	call_config_ops(dsim, set_gpio_hw_te);

	return 0;
}

static int dsim_reset_panel(struct dsim_device *dsim)
{
	call_config_ops(dsim, set_gpio_lcd_reset, dsim->board_type);

	return 0;
}

static int dsim_set_panel_power(struct dsim_device *dsim, u32 on)
{
	call_config_ops(dsim, set_gpio_lcd_power, dsim->board_type);

	return 0;
}

int dsim_probe(u32 dev_id)
{
	int ret = 0;
	struct dsim_clks clks;
	struct dsim_device *dsim = NULL;

	dsim = kzalloc(sizeof(struct dsim_device), GFP_KERNEL);
	if (!dsim) {
		dsim_err("failed to allocate dsim device.\n");
		return -ENOMEM;
	}

	/* Display configuration on U-BOOT */
	dsim->board_type = get_exynos_board_type();
	dsim->config_ops = get_exynos_display_config();
	dsim->id = dev_id;
	if (!dsim->id)
		dsim0_for_decon = dsim;
	else
		dsim1_for_decon = dsim;

	dsim_get_gpios(dsim);
	dsim->data_lane_cnt = 4; /* Fixed */
	dsim->lcd_info = decon_get_lcd_info();//get LCD parameter
	if (!dev_id)
		dsim->reg_base = DSIM_1ST_BASE_ADDR;
	else
		dsim->reg_base = DSIM_2ND_BASE_ADDR;
	dsim->panel_ops = decon_get_panel_info();//get LCD MIPI driver --->displayon
	dsim->timing.bps = 0;
	/* set using lanes */
	switch (dsim->data_lane_cnt) {
		case 1:
			dsim->data_lane = DSIM_LANE_DATA0;
			break;
		case 2:
			dsim->data_lane = DSIM_LANE_DATA0 | DSIM_LANE_DATA1;
			break;
		case 3:
			dsim->data_lane = DSIM_LANE_DATA0 | DSIM_LANE_DATA1 |
				DSIM_LANE_DATA2;
			break;
		case 4:
			dsim->data_lane = DSIM_LANE_DATA0 | DSIM_LANE_DATA1 |
				DSIM_LANE_DATA2 | DSIM_LANE_DATA3;
			break;
		default:
			dsim_info("data lane is invalid.\n");
			return -EINVAL;
	};

	dsim->state = DSIM_STATE_SUSPEND;
	if (dsim->id == 0)  
		dsim_set_panel_power(dsim, 1);
	if (dsim->id == 0)  
		dsim_d_phy_onoff(dsim, 1);
	dsim_reg_init(dsim->id, dsim->lcd_info, dsim->data_lane_cnt);
	dsim_device_to_clks(dsim, &clks);
	dsim_reg_set_clocks(dsim->id, &clks, DSIM_LANE_CLOCK | dsim->data_lane, 1);
	clks_to_dsim_device(&clks, dsim);
	dsim_reg_set_lanes(dsim->id, DSIM_LANE_CLOCK | dsim->data_lane, 1);
	if (dsim->id == 0)
		call_panel_ops(dsim, probe, dsim);
	if (dsim->id == 0)  
		dsim_reset_panel(dsim);
	dsim_reg_set_hs_clock(dsim->id, 1);
	dsim->state = DSIM_STATE_HSCLKEN;
	if (dsim->id == 0)   
		call_panel_ops(dsim, displayon, dsim);
	dsim_info("mipi-dsi driver(%s mode), resolution(%d x %d) has been probed.\n",
			dsim->lcd_info->mode == DECON_MIPI_COMMAND_MODE ? "CMD" : "VIDEO", dsim->lcd_info->xres, dsim->lcd_info->yres);

	return 0;
}
