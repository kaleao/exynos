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

#define GPG3CON		*(volatile unsigned int *)(0x134701C0)
#define GPG3DAT		*(volatile unsigned int *)(0x134701C4)
#define GPG3PUD		*(volatile unsigned int *)(0x134701C8)

#define IIC0_ESCL_Hi	GPG3DAT |= (0x1<<1)
#define IIC0_ESCL_Lo	GPG3DAT &= ~(0x1<<1)
#define IIC0_ESDA_Hi	GPG3DAT |= (0x1<<0)
#define IIC0_ESDA_Lo	GPG3DAT &= ~(0x1<<0)

#define IIC0_ESCL_INP	GPG3CON &= ~(0xf<<4)
#define IIC0_ESCL_OUTP	GPG3CON = (GPG3CON & ~(0xf<<4))|(0x1<<4)

#define IIC0_ESDA_INP	GPG3CON &= ~(0xf<<0)
#define IIC0_ESDA_OUTP	GPG3CON = (GPG3CON & ~(0xf<<0))|(0x1<<0)

#define GPIO_DAT	GPG3DAT
#define GPIO_DAT_SHIFT	(0)
#define DIS_GPIO_PUD	GPG3PUD &= ~(0xf<<0)

#define DELAY		100

/* S2MPS15 slave address */
#define S2MPS15_ADDR	0xCC
#define S2MPS15_RTC	0x0C

#define VDD_BASE_VOLT	50000
#define VDD_VOLT_STEP	625
#define MIN_VOLT	500
#define MAX_RD_VAL	0xB0
#define RD_BUCK_VOLT(x)	(((x > MAX_RD_VAL) ? 0 : \
			(((x - 0x10) * VDD_VOLT_STEP) + VDD_BASE_VOLT) / 100))
#define WR_BUCK_VOLT(x)	((x < MIN_VOLT) ? 0x10 : \
			((((x) * 100) - VDD_BASE_VOLT) / VDD_VOLT_STEP + 0x10))
#define RD_BUCK_VOLT_EVT1(x)	(((x > (MAX_RD_VAL + 0x10)) ? 0 : \
			(((x - 0x20) * VDD_VOLT_STEP) + VDD_BASE_VOLT) / 100))
#define WR_BUCK_VOLT_EVT1(x)	((x < MIN_VOLT) ? 0x20 : \
			((((x) * 100) - VDD_BASE_VOLT) / VDD_VOLT_STEP + 0x20))

#define LDO_BASE_VOLT	700
#define LDO_VOLT_STEP	25
#define LDO_MIN_VOLT	700
#define LDO_MAX_VOLT	1000
#define LDO_MAX_RD_VAL	0x1C
#define RD_LDO_VOLT(x)	((((x & ~(0x3 << 6)) > LDO_MAX_RD_VAL) ? 0 : \
			((((x & ~(0x3 << 6)) - 0x10) * LDO_VOLT_STEP) + LDO_BASE_VOLT)))
#define WR_LDO_VOLT(x)	((x > LDO_MAX_VOLT) ? 0x1C : \
			((x - LDO_BASE_VOLT) / LDO_VOLT_STEP + 0x10))

/* S2MPS15 Revision ID */
#define REV_00		0x00

/* S2MPS15 Register Address */
#define S2MPS15_PMIC_ID	0x00
#define S2MPS15_INT1	0x01
#define S2MPS15_INT2	0x02
#define S2MPS15_INT3	0x03
#define S2MPS15_INT1M	0x04
#define S2MPS15_INT2M	0x05
#define S2MPS15_INT3M	0x06
#define S2MPS15_STATUS1	0x07
#define S2MPS15_STATUS2	0x08
#define PWRONSRC	0x09
#define OFFSRC		0x0A
#define RTC_BUF		0x0C
#define CTRL1		0x0D
#define WRSTBI_CTRL_EVT0	0x18
#define WRSTBI_CTRL_EVT1	0x15
#define BUCK1_CTRL	0x19
#define BUCK1_OUT	0x1A
#define BUCK2_CTRL	0x1B
#define BUCK2_OUT	0x1C
#define BUCK3_CTRL	0x1D
#define BUCK3_OUT	0x1E
#define BUCK4_CTRL	0x1F
#define BUCK4_OUT	0x20
#define BUCK5_CTRL	0x21
#define BUCK5_OUT	0x22
#define BUCK6_CTRL	0x23
#define BUCK6_OUT	0x24
#define BUCK7_CTRL	0x25
#define BUCK7_OUT	0x26
#define BUCK8_CTRL	0x27
#define BUCK8_OUT	0x28
#define BUCK9_CTRL	0x29
#define BUCK9_OUT	0x2A
#define BUCK10_CTRL	0x2B
#define BUCK10_OUT	0x2C

#define LDO_DVS1	0x30
#define LDO_DVS2	0x31
#define LDO_DVS3	0x32
#define LDO_DVS4	0x33
#define LDO8_CTRL	0x3B
#define LDO14_CTRL	0x41
#define LDO15_CTRL	0x42
#define LDO16_CTRL	0x43
#define LDO17_CTRL	0x44
#define LDO18_CTRL	0x45
#define LDO19_CTRL	0x46
#define LDO20_CTRL	0x47
#define LDO21_CTRL	0x48
#define LDO22_CTRL	0x49
#define LDO23_CTRL	0x4A
#define LDO24_CTRL	0x4B
#define LDO25_CTRL	0x4D
#define LDO26_CTRL	0x4C
#define LDO27_CTRL	0x4E

/* S2MPS15 RTC Register Address */
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
#define LOW_JITTER_EN		(0x1 << 4)
#define BT32KHZ_EN		(0x1 << 2)
#define CP32KHZ_EN		(0x1 << 1)
#define AP32KHZ_EN		(0x1 << 0)

/*
 * WRSTBI
 */
#define WRSTBI_EN_EVT0		(0x1 << 5)
#define WRSTBI_EN_EVT1		(0x1 << 6)

/*
 * EMA
 */
#define ASV1_1_APOLLO_G3D	0x14
#define ASV3_2_ATLAS_L1		0x38
#define ASV3_3_ATLAS_L2_INT_CAM	0x3C

#define APOLLO_EMA_CON		0x0038
#define ATLAS_EMA_CON		0x0138
#define G3D_EMA_RF2_UHD_CON	0x1224
#define IMEM_EMA_VROMP_HD_CON	0x1424
#define EMA_RF2_HS_CON		0x2718
#define EMA_RF2_UHD_CON		0x271C
#define EMA_RF2_HS_CON_INT	0x2758
#define EMA_RF2_UHD_CON_INT	0x275C
#define ATLAS_EMA_REG1		0x2908

extern void pmic_init(void);
extern void IIC0_ERead(unsigned char ChipId,
		unsigned char IicAddr, unsigned char *IicData);
extern void IIC0_EWrite(unsigned char ChipId,
		unsigned char IicAddr, unsigned char IicData);
extern void display_pmic_rtc_time(void);
extern void get_asv_efuse(void);

#endif /*__PMIC_H__*/

