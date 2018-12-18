/*
 * (C) Copyright 2013 Samsung Electronics Co. Ltd
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#ifndef __PMIC_H__
#define __PMIC_H__

#define GPM0CON		*(volatile unsigned int *)(0x104E0000)
#define GPM0DAT		*(volatile unsigned int *)(0x104E0004)
#define GPM0PUD		*(volatile unsigned int *)(0x104E0008)

#define IIC0_ESCL_Hi	GPM0DAT |= (0x1<<1)
#define IIC0_ESCL_Lo	GPM0DAT &= ~(0x1<<1)
#define IIC0_ESDA_Hi	GPM0DAT |= (0x1<<0)
#define IIC0_ESDA_Lo	GPM0DAT &= ~(0x1<<0)

#define IIC0_ESCL_INP	GPM0CON &= ~(0xf<<4)
#define IIC0_ESCL_OUTP	GPM0CON = (GPM0CON & ~(0xf<<4))|(0x1<<4)

#define IIC0_ESDA_INP	GPM0CON &= ~(0xf<<0)
#define IIC0_ESDA_OUTP	GPM0CON = (GPM0CON & ~(0xf<<0))|(0x1<<0)

#define GPIO_DAT	GPM0DAT
#define GPIO_DAT_SHIFT	(0)
#define DIS_GPIO_PUD	GPM0PUD &= ~(0xf<<0)

#define DELAY		100

/* S2MPU03 slave address */
#define S2MPU03_ADDR	0xCC
#define S2MPU03_RTC	0x0C

#define VDD_BASE_VOLT	50000
#define VDD_VOLT_STEP	625
#define MIN_VOLT	500
#define MAX_RD_VAL	0xB0
#define RD_BUCK_VOLT(x)	(((x > MAX_RD_VAL) ? 0 : \
			(((x - 0x10) * VDD_VOLT_STEP) + VDD_BASE_VOLT) / 100))
#define WR_BUCK_VOLT(x)	((x < MIN_VOLT) ? 0x10 : \
			((((x) * 100) - VDD_BASE_VOLT) / VDD_VOLT_STEP + 0x10))

/* S2MPU03 Revision ID */
#define REV_00		0x00

/* S2MPU03 Register Address */
#define S2MPU03_PMIC_ID	0x00
#define S2MPU03_INT1	0x01
#define S2MPU03_INT2	0x02
#define S2MPU03_INT3	0x03
#define S2MPU03_INT1M	0x04
#define S2MPU03_INT2M	0x05
#define S2MPU03_INT3M	0x06
#define S2MPU03_STATUS1	0x07
#define S2MPU03_STATUS2	0x08
#define PWRONSRC	0x09
#define OFFSRC		0x0A
#define RTC_BUF		0x0C
#define MRSTB		0x0D
#define WRSTBI_CTRL	0x15
#define BUCK1_CTRL1	0x1A
#define BUCK1_CTRL2	0x1B
#define BUCK2_CTRL1	0x1C
#define BUCK2_CTRL2	0x1D
#define BUCK3_CTRL1	0x1E
#define BUCK3_CTRL2	0x1F
#define BUCK4_CTRL1	0x20
#define BUCK4_CTRL2	0x21
#define BUCK5_CTRL1	0x22
#define BUCK5_CTRL2	0x23
#define BUCK6_CTRL1	0x24
#define BUCK6_CTRL2	0x25
#define BUCK7_CTRL1	0x26
#define BUCK7_CTRL2	0x27
#define BUCK7_CTRL3	0x28

#define LDO_DVS		0x2A
#define LDO1_CTRL	0x2B
#define LDO2_CTRL	0x2C
#define LDO3_CTRL	0x2D
#define LDO4_CTRL	0x2E
#define LDO5_CTRL	0x2F
#define LDO6_CTRL	0x30
#define LDO7_CTRL	0x31
#define LDO8_CTRL	0x32
#define LDO9_CTRL	0x33
#define LDO10_CTRL	0x34
#define LDO11_CTRL1	0x35
#define LDO11_CTRL2	0x36
#define LDO12_CTRL	0x37
#define LDO13_CTRL	0x38
#define LDO14_CTRL	0x39
#define LDO15_CTRL	0x3A
#define LDO16_CTRL	0x3B
#define LDO17_CTRL1	0x3C
#define LDO17_CTRL2	0x3D
#define LDO18_CTRL1	0x3E
#define LDO18_CTRL2	0x3F
#define LDO19_CTRL	0x40
#define LDO20_CTRL	0x41
#define LDO21_CTRL	0x42
#define LDO22_CTRL	0x43
#define LDO23_CTRL	0x44
#define LDO24_CTRL	0x45
#define LDO25_CTRL	0x46
#define LDO26_CTRL	0x47
#define LDO27_CTRL	0x48
#define LDO28_CTRL	0x49
#define LDO29_CTRL	0x4A
#define LDO30_CTRL	0x4B
#define LDO31_CTRL	0x4C
#define LDO32_CTRL	0x4D
#define LDO33_CTRL	0x4E
#define LDO34_CTRL	0x4F
#define LDO35_CTRL	0x50
#define LDO36_CTRL	0x51
#define LDO37_CTRL	0x52
#define LDO38_CTRL	0x53
#define LDO39_CTRL	0x54

/* S2MPU03 RTC Register Address */
#define RTC_WTSR_SMPL	0x01
#define RTC_UPDATE	0x02
#define RTC_SEC		0x04
#define RTC_MIN		0x05
#define RTC_HOUR	0x06
#define RTC_WEEK	0x07
#define RTC_DATE	0x08
#define RTC_MONTH	0x09
#define RTC_YEAR	0x0A

/* RTC Counter Register offsets */
typedef enum {
	PMIC_RTC_SEC = 0,
	PMIC_RTC_MIN,
	PMIC_RTC_HOUR,
	PMIC_RTC_WEEK,
	PMIC_RTC_DATE,
	PMIC_RTC_MONTH,
	PMIC_RTC_YEAR,
	NR_PMIC_RTC_CNT_REGS,
};

/* LDO CTRL bit */
#define OFF			(0x0)
#define ON			(0x1)
#define OUTPUT_OFF		(~(3 << 6))
#define OUTPUT_PWREN_ON		(1 << 6)
#define OUTPUT_LOWPWR_MODE	(2 << 6)
#define OUTPUT_NORMAL_MODE	(3 << 6)

/*
 * RTC_BUF
 */
#define CP32KHZ_EN		(0x1 << 1)
#define AP32KHZ_EN		(0x1 << 0)

/*
 * WRSTBI
 */
#define WRSTBI_EN		(0x1 << 6)
#define WRST_EN			(0x1 << 4)

/*
 * Manual Reset
 */
#define MRSTB_EN		(0x1 << 4)

extern void pmic_init(void);
extern void pmic_lcd_tsp(void);
extern void IIC0_ERead(unsigned char ChipId,
		unsigned char IicAddr, unsigned char *IicData);
extern void IIC0_EWrite(unsigned char ChipId,
		unsigned char IicAddr, unsigned char IicData);
extern void display_pmic_rtc_time(void);
extern void get_asv_efuse(void);
extern void pmic_audio_gps_enable(void);

#endif /*__PMIC_H__*/

