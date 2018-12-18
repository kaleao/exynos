/*
 * Copyright (c) 2012 Samsung Electronics Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef __LCD_CTRL_H__
#define __LCD_CTRL_H__

#include <asm/arch/cpu.h>

#include <config.h>
#include <common.h>

#include "decon_lcd.h"

#define LDI_ID_REG			0x04
#define LDI_ID_LEN			3

void lcd_init(u32 id, struct decon_lcd *lcd);
void lcd_enable_exynos(u32 id);
void lcd_disable_exynos(u32 id);
//int lcd_gamma_ctrl(u32 backlightlevel);
//int lcd_gamma_update(void);

struct decon_lcd *decon_get_lcd_info(void);
struct mipidsim_lcd_driver *decon_get_panel_info(void);

#endif /* __LCD_CTRL_H__ */
