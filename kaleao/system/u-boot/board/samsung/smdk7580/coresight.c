/*
 * (C) Copyright 2014 Samsung Electronics Co. Ltd
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <common.h>
#include <asm/io.h>
#include "coresight.h"

void etb_dump(void)
{
       int i;
       char *buf_ptr;
       unsigned int read_data, depth;
       unsigned int read_ptr, write_ptr;
       unsigned int frame_off, frame_endoff;

       ETB_UNLOCK(EXYNOS_ETB_BASE);

       read_ptr = etb_readl(EXYNOS_ETB_BASE, ETB_RAM_READ_POINTER);
       write_ptr = etb_readl(EXYNOS_ETB_BASE, ETB_RAM_WRITE_POINTER);

       frame_off = write_ptr % 4;
       frame_endoff = 4 - frame_off;
       if (frame_off) {
               printf("write_ptr: %lu not aligned to formatter frame size\n",
                               (unsigned long)write_ptr);
               printf("frameoff: %lu, frame_endoff: %lu\n",
                               (unsigned long)frame_off, (unsigned long)frame_endoff);
               write_ptr += frame_endoff;
       }

       if ((etb_readl(EXYNOS_ETB_BASE, ETB_STATUS_REG) & 0x1) == 0)
               etb_writel(EXYNOS_ETB_BASE, 0x0, ETB_RAM_READ_POINTER);
       else
               etb_writel(EXYNOS_ETB_BASE, write_ptr, ETB_RAM_READ_POINTER);

       /* RAM depth == 4096, 4096 * 4Byte = 16KB */
       depth = 4096;
       buf_ptr = (char *)EXYNOS_ETBDUMP_SDRAM_BASE;
       for (i = 0; i < depth; i++) {
               read_data = etb_readl(EXYNOS_ETB_BASE, ETB_RAM_READ_DATA_REG);
               *buf_ptr++ = read_data >> 0;
               *buf_ptr++ = read_data >> 8;
               *buf_ptr++ = read_data >> 16;
               *buf_ptr++ = read_data >> 24;
       }

       if (frame_off) {
               buf_ptr -= (frame_endoff * 4);
               for (i = 0; i < frame_endoff; i++) {
                       *buf_ptr++ = 0x0;
                       *buf_ptr++ = 0x0;
                       *buf_ptr++ = 0x0;
                       *buf_ptr++ = 0x0;
               }
       }

       etb_writel(EXYNOS_ETB_BASE, read_ptr, ETB_RAM_READ_POINTER);

       ETB_LOCK(EXYNOS_ETB_BASE);

       printf("ETB(16KB) dumped from %#x to %#x !!!!\n", EXYNOS_ETB_BASE, EXYNOS_ETBDUMP_SDRAM_BASE);
}
