/*
 * SAMSUNG SoC DECON driver
 *
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * Sewoon Park <seuni.park@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <malloc.h>
#include <configs/avl7420_aarch64_evt1.h>
#include "decon.h"
#include "dsim.h"
#include "decon_config.h"
#include "./panels/lcd_ctrl.h"
#ifdef CONFIG_DISPLAY_LOGO
#include "ms_bmp.h"
#endif
#include <asm/arch/power.h>

#define false 0
#define true 1

static int drv_video_on = false;
struct decon_device *decon_int_drvdata;
EXPORT_SYMBOL(decon_int_drvdata);
struct decon_device *decon_ext_drvdata;
EXPORT_SYMBOL(decon_ext_drvdata);

extern struct dsim_device *dsim0_for_decon;
extern struct dsim_device *dsim1_for_decon;

/* ---------- CHECK FUNCTIONS ----------- */
static void decon_to_psr_info(struct decon_device *decon,
				struct decon_psr_info *psr)
{
	psr->psr_mode = decon->pdata->psr_mode;
	psr->trig_mode = decon->pdata->trig_mode;
}

static void decon_to_init_param(struct decon_device *decon, struct decon_init_param *p)
{
	struct decon_lcd *lcd_info = decon_get_lcd_info();
	int ret = 0;

	if (decon->id) {
		/* should be control a DECON_EXT */
	} else {
		p->lcd_info = lcd_info;
	}
	p->psr.psr_mode = decon->pdata->psr_mode;
	p->psr.trig_mode = decon->pdata->trig_mode;
	p->nr_windows = decon->pdata->max_win;
}

static u32 vidosd_a(int x, int y)
{
	return VIDOSD_A_TOPLEFT_X(x) |
		VIDOSD_A_TOPLEFT_Y(y);
}

static u32 vidosd_b(int x, int y, u32 xres, u32 yres)
{
	return VIDOSD_B_BOTRIGHT_X(x + xres - 1) |
		VIDOSD_B_BOTRIGHT_Y(y + yres - 1);
}

static u32 vidosd_c(u8 r0, u8 g0, u8 b0)
{
	return VIDOSD_C_ALPHA0_R_F(r0) |
		VIDOSD_C_ALPHA0_G_F(g0) |
		VIDOSD_C_ALPHA0_B_F(b0);
}

static u32 vidosd_d(u8 r1, u8 g1, u8 b1)
{
	return VIDOSD_D_ALPHA1_R_F(r1) |
		VIDOSD_D_ALPHA1_G_F(g1) |
		VIDOSD_D_ALPHA1_B_F(b1);
}

/* ---------- FB_IOCTL INTERFACE ----------- */
static int decon_wait_for_vsync(struct decon_device *decon, u32 timeout)
{
	return decon_reg_wait_for_update_timeout(decon->id, timeout * 1000);
}

static void decon_parse_pdata(struct decon_device *decon, u32 dev_id)
{
	struct exynos_decon_platdata *pdata = get_decon_pdata();

	decon->id = dev_id;
	decon->pdata->ip_ver = pdata->ip_ver;
	decon->pdata->psr_mode = pdata->psr_mode;
	decon->pdata->trig_mode = pdata->trig_mode;
	decon->pdata->dsi_mode = pdata->dsi_mode;
	decon->pdata->max_win = pdata->max_win;
	decon->pdata->default_win = pdata->default_win;

	decon_info("decon-%s: ver%d, max win%d, %s mode, %s trigger\n",
			decon->id ? "ext" : "int", decon->pdata->ip_ver,
			decon->pdata->max_win,
			decon->pdata->psr_mode ? "command" : "video",
			decon->pdata->trig_mode ? "sw" : "hw");
	decon_info("%s dsi mode\n", decon->pdata->dsi_mode ? "dual" : "single");
}

void draw_logo(int width, int height)
{

	int i,j;
	height = LCD_HEIGHT/ 3;

	for (i=0; i<height; i++){
		for(j=0; j< LCD_WIDTH; j++){
			*((unsigned int *)(UBOOT_ON_LCD_START_ADDR + (i * LCD_WIDTH + j)*4)) = 0x0000ff00;
		}
	}
	for (i=height; i<height*2; i++){
		for(j=0; j< LCD_WIDTH; j++){
			*((unsigned int *)(UBOOT_ON_LCD_START_ADDR + (i * LCD_WIDTH + j)*4)) = 0x00ff0000;		
		}
	}
	for (i=height*2; i<height*3; i++){
		for(j=0; j< LCD_WIDTH; j++){
			*((unsigned int *)(UBOOT_ON_LCD_START_ADDR + (i * LCD_WIDTH + j)*4)) = 0x000000ff;
		}
	}
}

#ifdef CONFIG_DISPLAY_LOGO
void s5p_lcd_draw_bootlogo(unsigned char *buf)
{
	unsigned char *pboot_logo_data=buf;
	unsigned int LCD_WIDTH_CHG,LCD_HEIGHT_CHG;

#ifdef CONFIG_DECON_LCD_S6E8AA0
	LCD_WIDTH_CHG = 720;
	LCD_HEIGHT_CHG = 1280;
#elif defined(CONFIG_DECON_LCD_S6TNMR7) 
	LCD_WIDTH_CHG = 2560;
	LCD_HEIGHT_CHG = 1600;
#else
	LCD_WIDTH_CHG = 1080;
	LCD_HEIGHT_CHG = 1920;
#endif

	printf("LCD_WIDTH:%d, LCD_HEIGHT:%d\n", LCD_WIDTH_CHG, LCD_HEIGHT_CHG);	
	unsigned int i, j, sx, sy;
	unsigned int* pBuffer = (unsigned int*)LOGO_KERNEL;
	unsigned int* pBufferEnd = pBuffer + (LCD_HEIGHT_CHG*LCD_WIDTH_CHG);
	BITMAPFILEHEADER *pBithead = (BITMAPFILEHEADER *)pboot_logo_data;
	BITMAPINFOHEADER* pBitinfo = (BITMAPINFOHEADER *)(pboot_logo_data+sizeof(BITMAPFILEHEADER));
	unsigned char* pBitmap = (unsigned char*)pboot_logo_data+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
	unsigned int* pBitmapLong = (unsigned int*)pBitmap;
	unsigned int iBitmapData;
	unsigned int *pcur_fb;
	int width, height;

	printf("------------------\n");
	printf("size FILEHEADER:%d, INFOHEADER:%d\n", sizeof(BITMAPFILEHEADER), sizeof(BITMAPINFOHEADER));
	printf("type:0x%x,size:%d:offset:%d\n", pBithead->bfType, pBithead->bfSize, pBithead->bfOffBits);
	printf("bitcount:%d\n", pBitinfo->biBitCount);
	printf("logo width:%d, height:%d\n", (pBitinfo->biWidth), pBitinfo->biHeight ), pBithead->bfOffBits;
	printf("LCD_WIDTH:%d, LCD_HEIGHT:%d\n", LCD_WIDTH_CHG, LCD_HEIGHT_CHG);
	
	if(pBithead->bfType!=0x4d42 || pBitinfo->biBitCount != 24)
	{
		printf("bfType:0x%x, biBitCount:%d\n", pBithead->bfType, pBitinfo->biBitCount);
		printf("Please run \"fastboot flash charger sd_fuse/battery_low.bmp\"\n");
		return ;
	}

	width = pBitinfo->biWidth;
	height = pBitinfo->biHeight;
	if(width<0 || width>LCD_WIDTH_CHG)
		return ;
	if(height<0 || height>LCD_HEIGHT_CHG)
		return ;
	sx = (LCD_WIDTH_CHG - width)>>1;
	sy = (LCD_HEIGHT_CHG - height)>>1;
	//BMP is flush from bottom to top! 14+50+(width*3+pad_to_4)*height
	for(i=sy+height;i>sy;i--)
	{
		for(j=sx;j<sx+width;j++)
		{
			pcur_fb = pBuffer + (i*LCD_WIDTH_CHG +j);
			iBitmapData = (pBitmap[0]<<0) | (pBitmap[1]<<8) | (pBitmap[2]<<16) | 0xff<<24;
			*pcur_fb = iBitmapData;
			pBitmap += 3;
		}
		j = (((unsigned long)((width*3+3)/4))*4-width*3);
		//printf("padding:%d\n",j);
		pBitmap += j;
	}
}

int Lcd_read_logo(unsigned char *partition_name,unsigned char *buf)
{
	char run_cmd[100];
	unsigned long long start, count;
	unsigned long offset;
	unsigned int LCD_WIDTH_CHG,LCD_HEIGHT_CHG;
	int ret;
	struct exynos7420_power *pmu = (struct exynos7420_power *)EXYNOS7420_POWER_BASE;
	int OmPin;
#ifdef CONFIG_DECON_LCD_S6E8AA0
	LCD_WIDTH_CHG = 720;
	LCD_HEIGHT_CHG = 1280;
#elif defined(CONFIG_DECON_LCD_S6TNMR7) 
	LCD_WIDTH_CHG = 2560;
	LCD_HEIGHT_CHG = 1600;
#else
	LCD_WIDTH_CHG = 1080;
	LCD_HEIGHT_CHG = 1920;
#endif

	OmPin = readl(&pmu->inform3);
	if(!(strncmp(partition_name,"bootlogo",8)) || !(strncmp(partition_name,"battery",7))){
		if(OmPin == BOOT_UFS)
			sprintf(run_cmd, "movi read scsi %s 0 %lx", partition_name, buf);
		else
			sprintf(run_cmd, "movi read %s 0 %lx", partition_name, buf);

	}

	printf("run_cmd:%s\n", run_cmd);
	run_command(run_cmd,0);
	s5p_lcd_draw_bootlogo(buf);

	return 0;
}

extern void LCD_Poweroff(void);
extern int check_boot_mode(void);
void LCD_Turnoff(void)
{ 
	if(!drv_video_on)
		return; 
	LCD_Poweroff();
	decon_reg_direct_on_off(0, 0);/*Close Decon,Or kernel will panic*/
	decon_reg_reset(0);
	dsim_reg_sw_reset(0);
}

static void decon_init_diplay(struct decon_device *decon,
		struct dsim_device *dsim)
{

	struct decon_regs_data win_regs = {0};
	struct decon_psr_info psr;
	struct decon_init_param p;
	u32 timeout = 1000;
	u32 reg_val;

	decon_to_init_param(decon, &p);
	decon_reg_init(decon->id, decon->pdata->dsi_mode, &p);

	reg_val = readl(DECON_SMMU_BASE_ADDR);
	/* stop smmu before proceeding for buffer rendering */
	writel(0, DECON_SMMU_BASE_ADDR);

	win_regs.wincon = WINCON_BPPMODE_ARGB8888 | WINCON_ENWIN;
	win_regs.winmap = 0x0;
	win_regs.vidosd_a = vidosd_a(0, 0);
	win_regs.vidosd_b = vidosd_b(0, 0,
			p.lcd_info->xres, p.lcd_info->yres);
	win_regs.vidosd_c = vidosd_c(0, 0, 0);
	win_regs.vidosd_d = vidosd_d(0xff, 0xff, 0xff);
	win_regs.vidw_buf_start = UBOOT_ON_LCD_START_ADDR;
	win_regs.vidw_whole_w = p.lcd_info->xres;
	win_regs.vidw_whole_h = p.lcd_info->yres;
	win_regs.vidw_offset_x = 0;
	win_regs.vidw_offset_y = 0;
	win_regs.type = IDMA_G0;

	decon_reg_shadow_protect_win(decon->id, 0, 1);
	decon_reg_set_winmap(decon->id, 0, 0, 0);
	decon_reg_set_regs_data(decon->id, 0, &win_regs);
	decon_reg_shadow_protect_win(decon->id, 0, 0);
	decon_to_psr_info(decon, &psr);
	decon_reg_start(decon->id, decon->pdata->dsi_mode, &psr);

	decon_reg_set_trigger(decon->id, decon->pdata->dsi_mode,decon->pdata->trig_mode, DECON_TRIG_ENABLE);    
 
      	decon_reg_clear_int(decon->id);    
        decon_reg_set_int(decon->id, &psr, decon->pdata->dsi_mode, 1);    
        while (!(decon_read(decon->id, VIDINTCON1) & VIDINTCON1_INT_I80) && --timeout)    
       		mdelay(10);    
  
        if (!timeout)    
       		printf("decon: frame buffer done interrupt timeout.\n");    
        if (decon->pdata->trig_mode == DECON_HW_TRIG)    
       		decon_reg_set_trigger(decon->id, decon->pdata->dsi_mode, decon->pdata->trig_mode, DECON_TRIG_DISABLE);  
	
	return 0;
}

#else
static void decon_show_color_map(struct decon_device *decon,
		struct dsim_device *dsim)
{
	struct decon_regs_data win_regs = {0};
	struct decon_psr_info psr;
	struct decon_init_param p;

	decon_reg_shadow_protect_win(decon->id, 0, 1);

	decon_to_init_param(decon, &p);
	decon_reg_init(decon->id, decon->pdata->dsi_mode, &p);

	win_regs.wincon = WINCON_BPPMODE_ARGB8888;
	win_regs.winmap = 0x0;
	win_regs.vidosd_a = vidosd_a(0, 0);
	win_regs.vidosd_b = vidosd_b(0, 0,
			p.lcd_info->xres, p.lcd_info->yres);
	win_regs.vidosd_c = vidosd_c(0x0, 0x0, 0x0);
	win_regs.vidosd_d = vidosd_d(0xff, 0xff, 0xff);
	win_regs.vidw_buf_start = UBOOT_ON_LCD_START_ADDR;
	win_regs.vidw_whole_w = p.lcd_info->xres;
	win_regs.vidw_whole_h = p.lcd_info->yres;
	win_regs.vidw_offset_x = 0;
	win_regs.vidw_offset_y = 0;
	win_regs.type = IDMA_G0;

	decon_reg_set_regs_data(decon->id, 0, &win_regs);

	decon_reg_shadow_protect_win(decon->id, 0, 0);

	decon_to_psr_info(decon, &psr);
	decon_reg_start(decon->id, decon->pdata->dsi_mode, &psr);

	decon_reg_activate_window(decon->id, 0);

	decon_reg_set_winmap(decon->id, 0, 0x00FF33 /* black */, 1);

	decon_reg_set_trigger(decon->id, decon->pdata->dsi_mode,
			decon->pdata->trig_mode, DECON_TRIG_ENABLE);

	/*call_panel_ops(dsim, displayon, dsim);*/

	/* Sent first frame to LCD */
	decon_wait_for_vsync(decon, VSYNC_TIMEOUT_MSEC);

	if (decon->pdata->trig_mode == DECON_HW_TRIG)
		decon_reg_set_trigger(decon->id, decon->pdata->dsi_mode,
				decon->pdata->trig_mode, DECON_TRIG_DISABLE);
}
#endif

/* --------- DRIVER INITIALIZATION ---------- */
static int decon_probe(u32 dev_id)
{
	int times=100;
	struct decon_device *decon;
	int ret = 0;
	struct decon_psr_info psr;
	struct decon_init_param p;
	struct decon_regs_data win_regs;

	if (dev_id > MAX_DECON_ID) {
		decon_err("does not support (%d) decon device\n", dev_id);
		return -ERANGE;
	}

	decon = kzalloc(sizeof(struct decon_device), GFP_KERNEL);
	if (!decon) {
		decon_err("no memory for decon device\n");
		return -ENOMEM;
	}

	/* setup pointer to master device */
	decon->pdata = kzalloc(sizeof(struct exynos_decon_platdata),
			GFP_KERNEL);
	if (!decon->pdata) {
		decon_err("no memory for DECON platdata\n");
		return -ENOMEM;
	}

	/* store platform data ptr to decon_tv context */
	decon_parse_pdata(decon, dev_id);    
	decon_info("decon->id : %d\n",decon->id);    
	/* init clock setting for decon */
	if (!decon->id) {
		decon_int_drvdata = decon;
		decon->regs = DECON_INT_BASE_ADDR;
	} else {
		decon_ext_drvdata = decon;
		decon->regs = DECON_EXT_BASE_ADDR;
	}

	decon_info("decon%d registered successfully\n", decon->id);

	return 0;
}

/* --------- DRIVER DISPLAY START ---------- */
int drv_video_probe(void)
{
	struct exynos_decon_platdata *pdata = get_decon_pdata();
	int ret = 0;
	int i = 0;
#ifdef CONFIG_DECON_LCD_S6TNMR7
	ret = decon_probe(i);
	if (ret < 0) {
		decon_err("decon%d probe was failed\n", i);
		return ret;
	}
	
	for (i = 0; i <= pdata->dsi_mode; i++) {
		ret = dsim_probe(i);
		if (ret < 0) {
			decon_err("dsim%d probe was failed\n", i);
			return ret;
		}
	}
	memcpy(UBOOT_ON_LCD_START_ADDR, LOGO_KERNEL , LCD_WIDTH * LCD_HEIGHT *4);
	decon_init_diplay(decon_int_drvdata, dsim0_for_decon);
#else
	for (i = 0; i <= pdata->dsi_mode; i++) {
		ret = dsim_probe(i);
		if (ret < 0) {
			decon_err("dsim%d probe was failed\n", i);
			return ret;
		}
		ret = decon_probe(i);
		if (ret < 0) {
			decon_err("decon%d probe was failed\n", i);
			return ret;
		}
	}
#endif
	drv_video_on = true;
}

int drv_video_init(void)
{
	int ret = 0;
	Lcd_read_logo("bootlogo", LOGO_KERNEL +  LCD_WIDTH * LCD_HEIGHT *4);
	check_boot_mode();/*20150324 xufei:add boot menu function*/
	return ret;
}
