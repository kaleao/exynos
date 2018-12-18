/*
 * Copyright (c) 2010 Samsung Electronics Co., Ltd.
 *              http://www.samsung.com/
 *
 * S5PC220 - LCD Driver for U-Boot
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#pragma message("*********************************** bootmode_decon.c ***********************************")

#include <common.h>

//#include <font_CN16x32.h>

#define LCD_BGCOLOR		0x1428A0

static unsigned int gFgColor = 0xFF;
static unsigned int gLeftColor = LCD_BGCOLOR;
#if 0
#ifdef CONFIG_EDP_SCREEN
#define CFG_LCD_FBUFFER 0x41800800
#define LCD_WIDTH		2560
#define LCD_HEIGHT		1600
#else

#define CFG_LCD_FBUFFER 0x47000000
#define LCD_WIDTH		1280
#define LCD_HEIGHT		800
#endif

#endif
#define CFG_LCD_FBUFFER 0xE2000000 /*-->UBOOT_ON_LCD_START_ADDR*/
#define WORD_BANK_ADDR  0xDE000000	/*The font start address in DRAM*/	

#ifdef CONFIG_DECON_LCD_S6E8AA0
#define LCD_WIDTH		720
#define LCD_HEIGHT		1280
#elif defined(CONFIG_DECON_LCD_S6TNMR7) 
#define LCD_WIDTH		2560
#define LCD_HEIGHT		1600
#else
#define LCD_WIDTH		1080
#define LCD_HEIGHT		1920
#endif

/*
void LCD_setfgcolor(unsigned int color)
{
    gFgColor = color;
}

void LCD_setleftcolor(unsigned int color)
{
    gLeftColor = color;
}*/

//fangcg:  lcd is 4 bytes per pixel, but we only use 24 colors.
#define FONT_HEIGHT  42
#define FONT_WIDTH   24
#define LCD_LINE_LENGTH  LCD_WIDTH
#define ROWVALUE  (LCD_HEIGHT / FONT_HEIGHT)
#define COLVALUE   (LCD_WIDTH / FONT_WIDTH)

struct bFrameBuffer
{
    u32 pixel[LCD_HEIGHT][LCD_WIDTH];
}*g_pFrameBuf;

static char strbuffer[ROWVALUE][COLVALUE];

void LCD_setfgcolor_font(unsigned int color)
{
	gFgColor = color;
}
void LCD_draw_bgcolor_font()
{
    u32 i, j;
    u32 *pBuffer = (u32 *)CFG_LCD_FBUFFER;

    for (i = 0; i < LCD_HEIGHT; i++)
    {
        for (j = 0; j < LCD_WIDTH; j++)
        {
            *pBuffer++ = gFgColor;
        }
    }
}

void exynos_get_MFP(unsigned char *buf)
{
	static char run_cmd[100];

	sprintf(run_cmd, "movi read font 0 %lx", buf);
	printf("run_cmd:%s\n", run_cmd);
	run_command(run_cmd,0);
}

void lcd_draw_char(u32 row, u32 col, uchar c/*a char*/, u32 color,unsigned char *buf)
{
    uchar *font;
    u32 *pix;
    u32 f;
    int r;
    unsigned int offset = 0;
    u32 *pFramebuf = (u32 *)CFG_LCD_FBUFFER;

    if ((row > ROWVALUE) || (col >COLVALUE))
    {
        return -1;
    };

    if (c < 32 || c >= 127)
	return -1;

    g_pFrameBuf = (struct bFrameBuffer *)CFG_LCD_FBUFFER;
    pix = &g_pFrameBuf->pixel[((row * FONT_HEIGHT))][col * FONT_WIDTH];
    font = &buf[((c - 32) * (FONT_HEIGHT * 3))];
    for (r = 0; r < FONT_HEIGHT; r++)
    {
        f = *font++;

        offset = (row * FONT_HEIGHT + r) * LCD_WIDTH + (col * FONT_WIDTH);
        *pix++ = (((f>>7) & 0x1)? color: pFramebuf[offset + 0]);
        *pix++ = (((f>>6) & 0x1)? color: pFramebuf[offset + 1]);
        *pix++ = (((f>>5) & 0x1)? color: pFramebuf[offset + 2]);
        *pix++ = (((f>>4) & 0x1)? color: pFramebuf[offset + 3]);
        *pix++ = (((f>>3) & 0x1)? color: pFramebuf[offset + 4]);
        *pix++ = (((f>>2) & 0x1)? color: pFramebuf[offset + 5]);
        *pix++ = (((f>>1) & 0x1)? color: pFramebuf[offset + 6]);
        *pix++ = (((f>>0) & 0x1)? color: pFramebuf[offset + 7]);

        f = *font++;
        *pix++ = (((f>>7) & 0x1)? color: pFramebuf[offset + 8]);
        *pix++ = (((f>>6) & 0x1)? color: pFramebuf[offset + 9]);
        *pix++ = (((f>>5) & 0x1)? color: pFramebuf[offset + 10]);
        *pix++ = (((f>>4) & 0x1)? color: pFramebuf[offset + 11]);
        *pix++ = (((f>>3) & 0x1)? color: pFramebuf[offset + 12]);
        *pix++ = (((f>>2) & 0x1)? color: pFramebuf[offset + 13]);
        *pix++ = (((f>>1) & 0x1)? color: pFramebuf[offset + 14]);
        *pix++ = (((f>>0) & 0x1)? color: pFramebuf[offset + 15]);

        f = *font++;
        *pix++ = (((f>>7) & 0x1)? color: pFramebuf[offset + 16]);
        *pix++ = (((f>>6) & 0x1)? color: pFramebuf[offset + 17]);
        *pix++ = (((f>>5) & 0x1)? color: pFramebuf[offset + 18]);
        *pix++ = (((f>>4) & 0x1)? color: pFramebuf[offset + 19]);
        *pix++ = (((f>>3) & 0x1)? color: pFramebuf[offset + 20]);
        *pix++ = (((f>>2) & 0x1)? color: pFramebuf[offset + 21]);
        *pix++ = (((f>>1) & 0x1)? color: pFramebuf[offset + 22]);
        *pix++ = (((f>>0) & 0x1)? color: pFramebuf[offset + 23]);

        pix += (LCD_WIDTH - 24);
    }

    return 0;
}

void lcd_draw_string(u32 row, u32 col, char *c, u32 color)
{
    u32  i,j, x,nrow,ncol;

    nrow = row;
    ncol  = col;

    if (nrow > ROWVALUE ||ncol>COLVALUE)
    {
        return;
    }

    if (c[0] == 0) return;

    if (c[1] == 0)
    {
        x=1;
    }
    else
        x = strlen(c);

    for (i = 0; i < x; i++)
    {
        if (c[i] == '\n')
        {
            for (j = ncol; j < COLVALUE; j++)
            {
                strbuffer[nrow][ncol] = ' ';
                lcd_draw_char(nrow, ncol, strbuffer[nrow][ncol], color, WORD_BANK_ADDR);
            }
            nrow += 1;
            if (nrow > ROWVALUE)
            {
                //scroll();
                nrow = ROWVALUE;
            }
            ncol = 0;
        }
        else
        {
            strbuffer[nrow][ncol] = c[i];
            lcd_draw_char(nrow, ncol, strbuffer[nrow][ncol], color , WORD_BANK_ADDR );
            ncol += 1;
            if (ncol > COLVALUE-1)
            {
                nrow += 1;
                if (nrow > ROWVALUE)
                {
                    //scroll();
                    nrow = ROWVALUE;
                }
                ncol = 0;
            }
        }
    }
}

u32 get_lcd_framebuf(void)
{
	return (u32*)CFG_LCD_FBUFFER;
}

int get_lcd_width(void)
{
	return LCD_WIDTH;
}

int get_lcd_height(void)
{
	return LCD_HEIGHT;
}
