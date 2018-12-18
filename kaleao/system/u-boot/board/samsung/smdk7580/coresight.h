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

#ifndef __CORESIGHT_H
#define __CORESIGHT_H

#include <asm/io.h>

#define CORESIGHT_LAR           (0xFB0)
#define CORESIGHT_UNLOCK        (0xC5ACCE55)

#define etb_writel(base, val, off)   writel((val), base + off)
#define etb_readl(base, off)         readl(base + off)

#define ETB_LOCK(base)                                                 \
do {                                                                   \
       /* wait for things to settle */                                 \
       mb();                                                           \
       etb_writel(base, 0x0, CORESIGHT_LAR);                           \
} while (0)

#define ETB_UNLOCK(base)                                               \
do {                                                                   \
       etb_writel(base, CORESIGHT_UNLOCK, CORESIGHT_LAR);              \
       /* make sure everyone has seen this */                           \
       mb();                                                           \
} while (0)

#define ETB_STATUS_REG          (0x00C)
#define ETB_RAM_READ_DATA_REG   (0x010)
#define ETB_RAM_READ_POINTER    (0x014)
#define ETB_RAM_WRITE_POINTER   (0x018)

extern void etb_dump(void);

#endif
