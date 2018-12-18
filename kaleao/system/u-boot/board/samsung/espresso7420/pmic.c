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
#include <asm/arch/cpu.h>
#include <asm/arch/smc.h>
#include "pmic.h"
#include "asv-exynos_cal.h"

#define ASV_TBL_ADDR_BASE	(0x101E0160)
#define ASV_TBL_ADDR_CNT	(5)
#define MAX_ASV_GROUP		(16)

#ifndef CONFIG_SPL_BUILD
u32 uBigAsvGrpVolt;
u32 uLitAsvGrpVolt;
u32 uG3dAsvGrpVolt;
u32 uMifAsvGrpVolt;
u32 uIntAsvGrpVolt;
u32 uDisAsvGrpVolt;
#endif
u32 int_m_vthr = 0;
u32 int_m_delta = 0;

struct _asv_tbl_info /*little endian*/
{
	unsigned bigcpu_asv_group:12;			//(ASV_TBL_BASE+0x00)[11:0]
	unsigned bigcpu_ssa0:4;					//(ASV_TBL_BASE+0x00)[15:12]
	unsigned littlecpu_asv_group:12;		//(ASV_TBL_BASE+0x00)[27:16]
	unsigned littlecpu_ssa0:4;				//(ASV_TBL_BASE+0x00)[31:28]

	unsigned g3d_asv_group:12;				//(ASV_TBL_BASE+0x04)[11:0]
	unsigned g3d_ssa0:4;					//(ASV_TBL_BASE+0x04)[15:12]
	unsigned mif_asv_group:12;				//(ASV_TBL_BASE+0x04)[27:16]
	unsigned mif_ssa0:4;					//(ASV_TBL_BASE+0x04)[31:28]

	unsigned int_asv_group:12;				//(ASV_TBL_BASE+0x08)[11:0]
	unsigned int_ssa0:4;					//(ASV_TBL_BASE+0x08)[15:12]
	unsigned cam_disp_asv_group:12;			//(ASV_TBL_BASE+0x08)[27:16]
	unsigned cam_disp_ssa0:4;				//(ASV_TBL_BASE+0x08)[31:28]

	unsigned dvfs_asv_table_version:4;		//(ASV_TBL_BASE+0x0C)[3:0]
	unsigned asv_group_type:1;				//(ASV_TBL_BASE+0x0C)[4]
	unsigned reserved01:3;					//(ASV_TBL_BASE+0x0C)[7:5]
	unsigned shift_type:1;					//(ASV_TBL_BASE+0x0C)[8]
	unsigned ssa1_enable:1;					//(ASV_TBL_BASE+0x0C)[9]
	unsigned ssa0_enable:1;				//(ASV_TBL_BASE+0x0C)[10]
	unsigned reserved02:5;					//(ASV_TBL_BASE+0x0C)[15:11]
	unsigned asv_method:1;					//(ASV_TBL_BASE+0x0C)[16]
	unsigned reserved03:15;					//(ASV_TBL_BASE+0x0C)[31:17]

	unsigned main_asv_group:4;				//(ASV_TBL_BASE+0x10)[3:0]
	unsigned main_asv_ssa:3;				//(ASV_TBL_BASE+0x10)[6:4]
	unsigned main_asv_ssa_minus_sign:1;			//(ASV_TBL_BASE+0x10)[7]
	unsigned bigcpu_ssa1:4;					//(ASV_TBL_BASE+0x10)[11:8]
	unsigned littlecpu_ssa1:4;				//(ASV_TBL_BASE+0x10)[15:12]
	unsigned g3d_ssa1:4;					//(ASV_TBL_BASE+0x10)[19:16]
	unsigned mif_ssa1:4;					//(ASV_TBL_BASE+0x10)[23:20]
	unsigned int_ssa1:4;					//(ASV_TBL_BASE+0x10)[27:24]
	unsigned cam_disp_ssa1:4;				//(ASV_TBL_BASE+0x10)[31:28]

	unsigned bigcpu_ssa_ema:9;				//(ASV_TBL_BASE+0x14)[8:0]
	unsigned littlecpu_ssa_ema:9;			//(ASV_TBL_BASE+0x14)[17:9]
	unsigned g3d_ssa_ema:9;					//(ASV_TBL_BASE+0x14)[26:18]
	unsigned reserved04:5;					//(ASV_TBL_BASE+0x14)[31:27]
};

static struct _asv_tbl_info gasv_table_info;

static unsigned char pmic_id;


#ifndef CONFIG_SG_BINARY
void Delay(void)
{
	unsigned long i;
	for (i = 0; i < DELAY; i++)
		;
}

void IIC0_SCLH_SDAH(void)
{
	IIC0_ESCL_Hi;
	IIC0_ESDA_Hi;
	Delay();
}

void IIC0_SCLH_SDAL(void)
{
	IIC0_ESCL_Hi;
	IIC0_ESDA_Lo;
	Delay();
}

void IIC0_SCLL_SDAH(void)
{
	IIC0_ESCL_Lo;
	IIC0_ESDA_Hi;
	Delay();
}

void IIC0_SCLL_SDAL(void)
{
	IIC0_ESCL_Lo;
	IIC0_ESDA_Lo;
	Delay();
}

void IIC0_ELow(void)
{
	IIC0_SCLL_SDAL();
	IIC0_SCLH_SDAL();
	IIC0_SCLH_SDAL();
	IIC0_SCLL_SDAL();
}

void IIC0_EHigh(void)
{
	IIC0_SCLL_SDAH();
	IIC0_SCLH_SDAH();
	IIC0_SCLH_SDAH();
	IIC0_SCLL_SDAH();
}

void IIC0_EStart(void)
{
	IIC0_SCLH_SDAH();
	IIC0_SCLH_SDAL();
	Delay();
	IIC0_SCLL_SDAL();
}

void IIC0_EEnd(void)
{
	IIC0_SCLL_SDAL();
	IIC0_SCLH_SDAL();
	Delay();
	IIC0_SCLH_SDAH();
}

void IIC0_EAck_write(void)
{
	unsigned long ack;

	/* Function <- Input */
	IIC0_ESDA_INP;

	IIC0_ESCL_Lo;
	Delay();
	IIC0_ESCL_Hi;
	Delay();
	ack = GPIO_DAT;
	IIC0_ESCL_Hi;
	Delay();
	IIC0_ESCL_Hi;
	Delay();

	/* Function <- Output (SDA) */
	IIC0_ESDA_OUTP;

	ack = (ack >> GPIO_DAT_SHIFT) & 0x1;

	IIC0_SCLL_SDAL();
}

void IIC0_EAck_read(void)
{
	/* Function <- Output */
	IIC0_ESDA_OUTP;

	IIC0_ESCL_Lo;
	IIC0_ESCL_Lo;
	IIC0_ESDA_Hi;
	IIC0_ESCL_Hi;
	IIC0_ESCL_Hi;
	/* Function <- Input (SDA) */
	IIC0_ESDA_INP;

	IIC0_SCLL_SDAL();
}

void IIC0_ESetport(void)
{
	/* Pull Up/Down Disable	SCL, SDA */
	DIS_GPIO_PUD;

	IIC0_ESCL_Hi;
	IIC0_ESDA_Hi;

	/* Function <- Output (SCL) */
	IIC0_ESCL_OUTP;
	/* Function <- Output (SDA) */
	IIC0_ESDA_OUTP;

	Delay();
}

void IIC0_EWrite(unsigned char ChipId,
		unsigned char IicAddr, unsigned char IicData)
{
	unsigned long i;

	IIC0_EStart();

	/* write chip id */
	for (i = 7; i > 0; i--) {
		if ((ChipId >> i) & 0x0001)
			IIC0_EHigh();
		else
			IIC0_ELow();
	}

	/* write */
	IIC0_ELow();

	/* ACK */
	IIC0_EAck_write();

	/* write reg. addr. */
	for (i = 8; i > 0; i--) {
		if ((IicAddr >> (i-1)) & 0x0001)
			IIC0_EHigh();
		else
			IIC0_ELow();
	}

	/* ACK */
	IIC0_EAck_write();

	/* write reg. data. */
	for (i = 8; i > 0; i--) {
		if ((IicData >> (i-1)) & 0x0001)
			IIC0_EHigh();
		else
			IIC0_ELow();
	}

	/* ACK */
	IIC0_EAck_write();

	IIC0_EEnd();
}

void IIC0_ERead(unsigned char ChipId,
		unsigned char IicAddr, unsigned char *IicData)
{
	unsigned long i, reg;
	unsigned char data = 0;

	IIC0_EStart();

	/* write chip id */
	for (i = 7; i > 0; i--) {
		if ((ChipId >> i) & 0x0001)
			IIC0_EHigh();
		else
			IIC0_ELow();
	}

	/* write */
	IIC0_ELow();

	/* ACK */
	IIC0_EAck_write();

	/* write reg. addr. */
	for (i = 8; i > 0; i--) {
		if ((IicAddr >> (i-1)) & 0x0001)
			IIC0_EHigh();
		else
			IIC0_ELow();
	}

	/* ACK */
	IIC0_EAck_write();

	IIC0_EStart();

	/* write chip id */
	for (i = 7; i > 0; i--) {
		if ((ChipId >> i) & 0x0001)
			IIC0_EHigh();
		else
			IIC0_ELow();
	}

	/* read */
	IIC0_EHigh();
	/* ACK */
	IIC0_EAck_write();

	/* read reg. data. */
	IIC0_ESDA_INP;

	IIC0_ESCL_Lo;
	IIC0_ESCL_Lo;
	Delay();

	for (i = 8; i > 0; i--) {
		IIC0_ESCL_Lo;
		IIC0_ESCL_Lo;
		Delay();
		IIC0_ESCL_Hi;
		IIC0_ESCL_Hi;
		Delay();
		reg = GPIO_DAT;
		IIC0_ESCL_Hi;
		IIC0_ESCL_Hi;
		Delay();
		IIC0_ESCL_Lo;
		IIC0_ESCL_Lo;
		Delay();

		reg = (reg >> GPIO_DAT_SHIFT) & 0x1;

		data |= reg << (i-1);
	}

	/* ACK */
	IIC0_EAck_read();
	IIC0_ESDA_OUTP;

	IIC0_EEnd();

	*IicData = data;
}

static u32 get_ema_efuse(unsigned int offset)
{
	u64 tmp = 0;

	tmp = exynos_smc_read(SMC_CMD_REG, SMC_REG_ID_SFR_R((unsigned long) ASV_TBL_ADDR_BASE + offset));
	return (u32)tmp;
}

static void set_ema_atlas(u32 ema_con, u32 ema_reg, u32 offset_l1, u32 offset_l2, u32 default_l1, u32 default_l2)
{
	u32 fuse = 0;
	u32 mask = 0xF;

	fuse = get_ema_efuse(ASV3_2_ATLAS_L1);
	fuse = (fuse & (mask << offset_l1)) >> offset_l1;
	if (fuse == 0)
		ema_con = (ema_con & ~(mask << 4)) | (default_l1 << 4);
	else
		ema_con = (ema_con & ~(mask << 4)) | (fuse << 4);

	fuse = get_ema_efuse(ASV3_3_ATLAS_L2_INT_CAM);
	fuse = (fuse & (mask << offset_l2)) >> offset_l2;
	if (fuse == 0)
		ema_reg = (ema_con & ~(mask << 8)) | (default_l2 << 8);
	else
		ema_reg = (ema_con & ~(mask << 8)) | (fuse << 8);

	writel(ema_con, EXYNOS7420_SYSREG_BASE + ATLAS_EMA_CON);
	writel(ema_reg, EXYNOS7420_SYSREG_BASE + ATLAS_EMA_REG1);
}

static void set_ema_apollo(u32 ema_con, u32 offset, u32 value)
{
	u32 fuse = 0;
	u32 mask = 0x7;

	fuse = get_ema_efuse(ASV1_1_APOLLO_G3D);
	fuse = (fuse & (mask << offset)) >> offset;
	if (fuse == 0)
		ema_con = (ema_con & ~(mask << 20) & ~(mask << 8)) | (value << 20) | (value << 8);
	else
		ema_con = (ema_con & ~(mask << 20) & ~(mask << 8)) | (fuse << 20) | (fuse << 8);

	writel(ema_con, EXYNOS7420_SYSREG_BASE + APOLLO_EMA_CON);
}

static void set_ema_g3d(u32 ema_con, u32 offset, u32 value)
{
	u32 fuse = 0;
	u32 mask = 0x3;

	fuse = get_ema_efuse(ASV1_1_APOLLO_G3D);
	fuse = (fuse & (mask << offset)) >> offset;
	if (fuse == 0)
		ema_con = (ema_con & ~(mask << 4)) | (value << 4);
	else
		ema_con = (ema_con & ~(mask << 4)) | (fuse << 4);

	writel(ema_con, EXYNOS7420_SYSREG_BASE + G3D_EMA_RF2_UHD_CON);
}

static void set_ema_int_cam_disp(u32 ema_con, u32 offset, u32 value)
{
	u32 fuse = 0;
	u32 mask = 0x7;

	fuse = get_ema_efuse(ASV3_3_ATLAS_L2_INT_CAM);
	fuse = (fuse & (mask << offset)) >> offset;
	if (fuse == 0)
		ema_con = (ema_con & ~(mask << 0)) | (value << 0);
	else
		ema_con = (ema_con & ~(mask << 0)) | (fuse << 0);
	if (offset == 16) /* INT */
		writel(ema_con, EXYNOS7420_SYSREG_BASE + EMA_RF2_HS_CON_INT);
	else /* CAM_DISP */
		writel(ema_con, EXYNOS7420_SYSREG_BASE + EMA_RF2_HS_CON);
}

static void set_mcs_int_cam_disp(u32 mcs_con, u32 offset, u32 value)
{
	u32 fuse = 0;
	u32 mask = 0x3;

	fuse = get_ema_efuse(ASV3_3_ATLAS_L2_INT_CAM);
	fuse = (fuse & (mask << offset)) >> offset;
	if (fuse == 0)
		mcs_con = (mcs_con & ~(mask << 4)) | (value << 4);
	else
		mcs_con = (mcs_con & ~(mask << 4)) | (fuse << 4);
	if (offset == 28) /* INT */
		writel(mcs_con, EXYNOS7420_SYSREG_BASE + EMA_RF2_UHD_CON_INT);
	else /* CAM_DISP */
		writel(mcs_con, EXYNOS7420_SYSREG_BASE + EMA_RF2_UHD_CON);
}

/*#define EMA_DEBUG*/
static void set_ema(u8 buck, u16 vdd)
{
	switch(buck) {
	case BUCK2_OUT:
		/* Atlas */
#if !defined(CONFIG_SPL_BUILD) && defined(EMA_DEBUG)
		printf("[EMA] set ATLAS\n");
#endif
		if (vdd >= 1000) {
			set_ema_atlas(0x62, 0x530, 16, 0, 0x6, 0x5);
		} else if ((vdd >= 900) && (vdd < 1000)) {
			set_ema_atlas(0x62, 0x530, 20, 4, 0x6, 0x5);
		} else if ((vdd >= 800) && (vdd < 900)) {
			set_ema_atlas(0x82, 0xb30, 24, 8, 0x8, 0xB);
		} else {
			set_ema_atlas(0x72, 0x930, 24, 12, 0x7, 0x9);
		}
		break;
	case BUCK3_OUT:
		/* Apollo */
#if !defined(CONFIG_SPL_BUILD) && defined(EMA_DEBUG)
		printf("[EMA] set APOLLO\n");
#endif
		if (vdd >= 1000) {
			set_ema_apollo(0x210211, 9, 0x2);
		} else if ((vdd >= 900) && (vdd < 1000)) {
			set_ema_apollo(0x210211, 12, 0x2);
		} else if ((vdd >= 800) && (vdd < 900)) {
			set_ema_apollo(0x210211, 15, 0x2);
		} else {
			set_ema_apollo(0x210211, 18, 0x2);
		}
		break;
	case BUCK6_OUT:
		/* G3D */
#if !defined(CONFIG_SPL_BUILD) && defined(EMA_DEBUG)
		printf("[EMA] set G3D\n");
#endif
		if (vdd >= 900) {
			set_ema_g3d(0x11, 21, 0x1);
		} else if ((vdd >= 800) && (vdd < 900)) {
			set_ema_g3d(0x11, 23, 0x1);
		} else if ((vdd >= 700) && (vdd < 800)) {
			set_ema_g3d(0x11, 25, 0x1);
		} else {
			set_ema_g3d(0x31, 27, 0x3);
		}
		break;
	case BUCK4_OUT:
		/* INT */
#if !defined(CONFIG_SPL_BUILD) && defined(EMA_DEBUG)
		printf("[EMA] set INT\n");
#endif
		set_ema_int_cam_disp(0x12, 16, 0x4);
		set_mcs_int_cam_disp(0x11, 28, 0x3);
		break;
	case BUCK5_OUT:
		/* CAM_DISP */
#if !defined(CONFIG_SPL_BUILD) && defined(EMA_DEBUG)
		printf("[EMA] set CAM_DISP\n");
#endif
		set_ema_int_cam_disp(0x12, 19, 0x4);
		set_mcs_int_cam_disp(0x11, 30, 0x3);
		break;
	default:
		while(1);
	}
}

#ifdef CONFIG_SPL_BUILD
static void set_vdd_with_ema(u8 buck, float vdd)
{
	/* make high vdd only */
	u32 max_val = 0;
	u32 int_val = 0;
	u32 cam_disp_val = 0;

	switch(buck) {
	case BUCK4_OUT:
		/* INT */
		int_val = (u32)vdd;
		IIC0_ERead(S2MPS15_ADDR, BUCK5_OUT, &cam_disp_val);
		cam_disp_val = RD_BUCK_VOLT_EVT1((unsigned int)cam_disp_val);
		max_val = MAX(int_val, cam_disp_val);
		max_val = MAX(max_val, int_m_vthr) + int_m_delta;
		IIC0_EWrite(S2MPS15_ADDR, LDO8_CTRL,
			(WR_LDO_VOLT(max_val) | (0x3 << 6)));
		break;
	case BUCK5_OUT:
		/* DISP_CAM */
		IIC0_ERead(S2MPS15_ADDR, BUCK4_OUT, &int_val);
		int_val = RD_BUCK_VOLT_EVT1((unsigned int)int_val);
		cam_disp_val = (u32)vdd;
		max_val = MAX(int_val, cam_disp_val);
		max_val = MAX(max_val, int_m_vthr) + int_m_delta;
		IIC0_EWrite(S2MPS15_ADDR, LDO8_CTRL,
			(WR_LDO_VOLT(max_val) | (0x3 << 6)));
		break;
	}

	IIC0_EWrite(S2MPS15_ADDR, buck, WR_BUCK_VOLT_EVT1(vdd));
	set_ema(buck, (u16)vdd);
}
#else
static void set_vdd_with_ema(u8 buck, float vdd)
{
	u16 old_vdd = 0;
	u32 max_val = 0;
	u32 int_val = 0;
	u32 cam_disp_val = 0;

	switch(buck) {
	case BUCK4_OUT:
		/* INT */
		int_val = (u32)vdd;
		IIC0_ERead(S2MPS15_ADDR, BUCK5_OUT, &cam_disp_val);
		cam_disp_val = RD_BUCK_VOLT_EVT1((unsigned int)cam_disp_val);
		max_val = MAX(int_val, cam_disp_val);
		max_val = MAX(max_val, int_m_vthr) + int_m_delta;
#if defined(EMA_DEBUG)
	printf("\n");
	printf("[EMA] int_val : %d, cam_disp_val : %d\n", int_val, cam_disp_val);
	printf("[EMA] max_val : %d, int_m_vthr : %d\n", max_val, int_m_vthr);
	printf("\n");
#endif
		break;
	case BUCK5_OUT:
		/* DISP_CAM */
		IIC0_ERead(S2MPS15_ADDR, BUCK4_OUT, &int_val);
		int_val = RD_BUCK_VOLT_EVT1((unsigned int)int_val);
		cam_disp_val = (u32)vdd;
		max_val = MAX(int_val, cam_disp_val);
		max_val = MAX(max_val, int_m_vthr) + int_m_delta;
#if defined(EMA_DEBUG)
	printf("\n");
	printf("[EMA] int_val : %d, cam_disp_val : %d\n", int_val, cam_disp_val);
	printf("[EMA] max_val : %d, int_m_vthr : %d\n", max_val, int_m_vthr);
	printf("\n");
#endif
		break;
	}

	IIC0_ERead(S2MPS15_ADDR, buck, &old_vdd);
#if defined(EMA_DEBUG)
	printf("[EMA] old : %d, new : %d\n", RD_BUCK_VOLT_EVT1(old_vdd), (u16)vdd);
#endif
	if (RD_BUCK_VOLT_EVT1(old_vdd) == (u16)vdd) {
		return ;
	} else if (RD_BUCK_VOLT_EVT1(old_vdd) < (u16)vdd) {
		/* make high vdd */
#if defined(EMA_DEBUG)
		printf("[EMA] set high\n");
#endif
		if ((buck == BUCK4_OUT) || (buck == BUCK5_OUT))
			IIC0_EWrite(S2MPS15_ADDR, LDO8_CTRL,
				(WR_LDO_VOLT(max_val) | (0x3 << 6)));
		IIC0_EWrite(S2MPS15_ADDR, buck, WR_BUCK_VOLT_EVT1(vdd));
		set_ema(buck, (u16)vdd);
	} else {
		/* make low vdd */
#if defined(EMA_DEBUG)
		printf("[EMA] set low\n");
#endif
		set_ema(buck, (u16)vdd);
		IIC0_EWrite(S2MPS15_ADDR, buck, WR_BUCK_VOLT_EVT1(vdd));
		if ((buck == BUCK4_OUT) || (buck == BUCK5_OUT)) {
			IIC0_EWrite(S2MPS15_ADDR, LDO8_CTRL,
				(WR_LDO_VOLT(max_val) | (0x3 << 6)));
		}
	}
}
#endif

static void set_ldo_dvs1(void)
{
	u32 fuse = 0;
	u32 mask = 0x3;
	u32 value = 0;

	/* vthr */
	fuse = get_ema_efuse(ASV3_2_ATLAS_L1);
	fuse = (fuse & (mask << 0)) >> 0;
	switch(fuse) {
	case 0b00:
		/* 800mV */
		value = (value & ~(mask << 0)) | (0b100 << 5);
		break;
	case 0b01:
		/* 750mV */
		value = (value & ~(mask << 0)) | (0b011 << 5);
		break;
	case 0b10:
		/* 700mV */
		value = (value & ~(mask << 0)) | (0b010 << 5);
		break;
	case 0b11:
		/* 900mV */
		value = (value & ~(mask << 0)) | (0b110 << 5);
		break;
	default:
		while(1);
	}

	/* delta */
	fuse = get_ema_efuse(ASV3_2_ATLAS_L1);
	fuse = (fuse & (mask << 2)) >> 2;
	if (fuse == 0)
		value = (value & ~(mask << 3)) | (0b01 << 3);
	else
		value = (value & ~(mask << 3)) | (fuse << 3);

	/* sync_en */
	value = value | (0x1 << 2);

	IIC0_EWrite(S2MPS15_ADDR, LDO_DVS1, value);
}

static void set_ldo_dvs2(void)
{
	u32 fuse = 0;
	u32 mask = 0x3;
	u32 value = 0;

	/* vthr */
	fuse = get_ema_efuse(ASV3_2_ATLAS_L1);
	fuse = (fuse & (mask << 12)) >> 12;
	switch(fuse) {
	case 0b00:
		/* 750mV */
		value = (value & ~(mask << 0)) | (0b011 << 5);
		int_m_vthr = 750;
		break;
	case 0b01:
		/* 800mV */
		value = (value & ~(mask << 0)) | (0b100 << 5);
		int_m_vthr = 800;
		break;
	case 0b10:
		/* 700mV */
		value = (value & ~(mask << 0)) | (0b010 << 5);
		int_m_vthr = 700;
		break;
	case 0b11:
		/* 850mV */
		value = (value & ~(mask << 0)) | (0b101 << 5);
		int_m_vthr = 850;
		break;
	default:
		while(1);
	}

	/* delta */
	fuse = get_ema_efuse(ASV3_2_ATLAS_L1);
	fuse = (fuse & (mask << 14)) >> 14;
	int_m_delta = 25 + 25 * fuse;
	value = (value & ~(mask << 3)) | (fuse << 3);

	/* sync_en : disable */
	value = value | (0x0 << 2);

	IIC0_EWrite(S2MPS15_ADDR, LDO_DVS2, value);
}

static void set_ldo_dvs3(void)
{
	u32 fuse = 0;
	u32 mask = 0x3;
	u32 value = 0;

	/* vthr */
	fuse = get_ema_efuse(ASV3_2_ATLAS_L1);
	fuse = (fuse & (mask << 4)) >> 4;
	switch(fuse) {
	case 0b00:
		/* 800mV */
		value = (value & ~(mask << 0)) | (0b100 << 5);
		break;
	case 0b01:
		/* 750mV */
		value = (value & ~(mask << 0)) | (0b011 << 5);
		break;
	case 0b10:
		/* 700mV */
		value = (value & ~(mask << 0)) | (0b010 << 5);
		break;
	case 0b11:
		/* 900mV */
		value = (value & ~(mask << 0)) | (0b110 << 5);
		break;
	default:
		while(1);
	}

	/* delta */
	fuse = get_ema_efuse(ASV3_2_ATLAS_L1);
	fuse = (fuse & (mask << 6)) >> 6;
	value = (value & ~(mask << 3)) | (fuse << 3);

	/* sync_en */
	value = value | (0x1 << 2);

	IIC0_EWrite(S2MPS15_ADDR, LDO_DVS3, value);
}

static void set_ldo_dvs4(void)
{
	u32 fuse = 0;
	u32 mask = 0x3;
	u32 value = 0;

	/* vthr */
	fuse = get_ema_efuse(ASV3_2_ATLAS_L1);
	fuse = (fuse & (mask << 8)) >> 8;
	switch(fuse) {
	case 0b00:
		/* 750mV */
		value = (value & ~(mask << 0)) | (0b011 << 5);
		break;
	case 0b01:
		/* 800mV */
		value = (value & ~(mask << 0)) | (0b100 << 5);
		break;
	case 0b10:
		/* 700mV */
		value = (value & ~(mask << 0)) | (0b010 << 5);
		break;
	case 0b11:
		/* 850mV */
		value = (value & ~(mask << 0)) | (0b101 << 5);
		break;
	default:
		while(1);
	}

	/* delta */
	fuse = get_ema_efuse(ASV3_2_ATLAS_L1);
	fuse = (fuse & (mask << 10)) >> 10;
	value = (value & ~(mask << 3)) | (fuse << 3);

	/* sync_en */
	value = value | (0x1 << 2);

	IIC0_EWrite(S2MPS15_ADDR, LDO_DVS4, value);
}
#endif

void _asv_table_init(void)
{
	int i;
	u32 *pasv_table;
	u64 tmp;

	pasv_table = (u32 *)&gasv_table_info;
	for(i = 0; i < ASV_TBL_ADDR_CNT; i++) {
		tmp = exynos_smc_read(SMC_CMD_REG, SMC_REG_ID_SFR_R((unsigned long) ASV_TBL_ADDR_BASE + 0x4 * i));
		*(pasv_table +i) = (u32)tmp;
	}

}

static u32 _cal_is_fused_speed_grp(void)
{
	return gasv_table_info.asv_method;
}

#ifndef CONFIG_SG_BINARY
static u32 _cal_get_match_subgrp(u32 id, s32 level)
{
	u32 subgrp = 0;	/*  version 0 */

	switch (id) {
	case SYSC_DVFS_BIG:
		subgrp = (level <= SYSC_DVFS_L9) ? 0 :
			(level <= SYSC_DVFS_L15) ? 1 : 2;
		break;
	case SYSC_DVFS_LIT:
		if (gasv_table_info.dvfs_asv_table_version >= 9) {
			subgrp = (level <= SYSC_DVFS_L9) ? 0 :
				(level <= SYSC_DVFS_L13) ? 1 : 2;
		} else {
			subgrp = (level <= SYSC_DVFS_L9) ? 0 :
				(level <= SYSC_DVFS_L14) ? 1 : 2;
		}
		break;
	case SYSC_DVFS_G3D:
		if(gasv_table_info.dvfs_asv_table_version <= 6){
			subgrp = (level <= SYSC_DVFS_L3) ? 0 :
				(level <= SYSC_DVFS_L6) ? 1 : 2;
		} else{
			subgrp = (level <= SYSC_DVFS_L1) ? 0 :
				(level <= SYSC_DVFS_L3) ? 1 : 2;
		}
		break;
	case SYSC_DVFS_MIF:
		if(gasv_table_info.dvfs_asv_table_version <= 6){
			subgrp = (level <= SYSC_DVFS_L3) ? 0 :
				(level <= SYSC_DVFS_L7) ? 1 : 2;
		} else{
			subgrp = (level <= SYSC_DVFS_L1) ? 0 :
				(level <= SYSC_DVFS_L4) ? 1 : 2;
		}
		break;
	case SYSC_DVFS_INT:
		if(gasv_table_info.dvfs_asv_table_version <= 2) {
			subgrp = (level <= SYSC_DVFS_L6) ? 0 :
				(level <= SYSC_DVFS_L8) ? 1 : 2;
		} else if(gasv_table_info.dvfs_asv_table_version >= 7) {
			if(level <= SYSC_DVFS_L0)
				subgrp = 0;
			else if(level >= SYSC_DVFS_L2)
				subgrp = 2;
			else
				subgrp = 1;
		} else{
			if(level <= SYSC_DVFS_L2)
				subgrp = 0;
			else if(level >= SYSC_DVFS_L9)
				subgrp = 2;
			else
				subgrp = 1;
		}
		break;
	case SYSC_DVFS_CAM:
		if (gasv_table_info.dvfs_asv_table_version <= 2) {
			subgrp = (level <= SYSC_DVFS_L6) ? 0 :
				(level <= SYSC_DVFS_L8) ? 1 : 2;
		} else if ((gasv_table_info.dvfs_asv_table_version == 7) ||
				(gasv_table_info.dvfs_asv_table_version == 8)) {
			if ((level ==SYSC_DVFS_L0) || (level == SYSC_DVFS_L6))
				subgrp = 0;
			else if ((level == SYSC_DVFS_L1) || (level == SYSC_DVFS_L2) || (level == SYSC_DVFS_L7))
				subgrp = 1;
			else
				subgrp = 2;
		} else if (gasv_table_info.dvfs_asv_table_version >= 9) {
			if ((level ==SYSC_DVFS_L0) || (level == SYSC_DVFS_L6))
				subgrp = 0;
			else if((level == SYSC_DVFS_L1) || (level == SYSC_DVFS_L7))
				subgrp = 1;
			else
				subgrp = 2;
		} else {
			if ((level == SYSC_DVFS_L0) || (level == SYSC_DVFS_L6))
				subgrp = 0;
			else if (level >= SYSC_DVFS_L9)
				subgrp = 2;
			else
				subgrp = 1;
		}
		break;
	default:
		Assert(0);
	}

	return subgrp;
}

static u32 _cal_get_ids_hpm_group(void)
{
	return 0;
}

static u32 _cal_get_asv_grp(u32 id, s32 level)
{
	u32 subgrp, asv_group = 0;

	if (_cal_is_fused_speed_grp()) {
		if (gasv_table_info.asv_group_type == 1) {
			asv_group = gasv_table_info.main_asv_group;

			/* ASV group shift */
			if (gasv_table_info.main_asv_ssa_minus_sign == 1) {
				asv_group -= (int)gasv_table_info.main_asv_ssa;
			} else {
				asv_group += gasv_table_info.main_asv_ssa;
			}

			if(asv_group < 0)
				asv_group = 0;
			if(asv_group >= MAX_ASV_GROUP)
				asv_group = MAX_ASV_GROUP - 1;

		} else {
			subgrp = _cal_get_match_subgrp(id, level);
			subgrp = subgrp * 4;

			if (id == SYSC_DVFS_BIG)
				asv_group = (gasv_table_info.bigcpu_asv_group >> subgrp) & 0xF;
			else if (id == SYSC_DVFS_LIT)
				asv_group = (gasv_table_info.littlecpu_asv_group >> subgrp) & 0xF;
			else if (id == SYSC_DVFS_G3D)
				asv_group = (gasv_table_info.g3d_asv_group >> subgrp) & 0xF;
			else if (id == SYSC_DVFS_MIF)
				asv_group = (gasv_table_info.mif_asv_group >> subgrp) & 0xF;
			else if (id == SYSC_DVFS_INT)
				asv_group = (gasv_table_info.int_asv_group >> subgrp) & 0xF;
			else if (id == SYSC_DVFS_CAM)
				asv_group = (gasv_table_info.cam_disp_asv_group >> subgrp) & 0xF;
			else
				asv_group = 0;
		}
	} else {
		asv_group = _cal_get_ids_hpm_group();
	}

	if (asv_group > MAX_ASV_GROUP)
		return 0;

	return asv_group;
}
#endif

u32 _cal_get_mif(void)
{
	u32 chip_status;
	u32 bin2_efuse = (readl(EXYNOS7420_PRO_ID + BIN2_OFFSET) >> 16) & 0x3;

	chip_status = (gasv_table_info.dvfs_asv_table_version & 0xF);

	switch (chip_status) {
	case 5:
		while(1);		// MEMCLK = 828 MHz
		break;
	case 0:
	case 1:
	case 4:
		return MEMCLK_1264;	// MEMCLK = 1264 MHz
		break;
	case 2:
	case 3:
	case 6:
	case 7:
	case 8:
	case 9:
	default:
		switch (bin2_efuse) {
		case 0b00:
			return MEMCLK_1552;	// MEMCLK = 1552 MHz
			break;
		case 0b01:
			return MEMCLK_1456;	// MEMCLK = 1456 MHz
			break;
		case 0b11:
			return MEMCLK_1264;	// MEMCLK = 1264 MHz
			break;
		default:
			while(1);
			break;
		}
	}
}

#ifndef CONFIG_SG_BINARY
const u16 _volt_table_mif_asv_v9[16] =
 /*	ASV0 ASV1 ASV2 ASV3 ASV4 ASV5 ASV6 ASV7 ASV8 ASV9 ASV10 ASV11 ASV12 ASV13 ASV14 ASV15 */
	{938, 925, 913, 900, 888, 875, 863, 850, 838, 825, 813, 800, 794, 788, 782, 775,};

static void set_vdd_mif_buck1(void) {
	u32 bin2_efuse = (readl(EXYNOS7420_PRO_ID + BIN2_OFFSET) >> 16) & 0x3;
	u32 asv_group = _cal_get_asv_grp(SYSC_DVFS_MIF, 0);
	u32 vdd_mif = 0;

	switch (bin2_efuse) {
	case 0b00: // MEMCLK = 1552 MHz
		vdd_mif = _volt_table_mif_asv_v9[asv_group];
		break;
	case 0b01: // MEMCLK = 1456 MHz
		vdd_mif = _volt_table_mif_asv_v9[asv_group] - 37;
		break;
	case 0b11: // MEMCLK = 1264 MHz
		vdd_mif = _volt_table_mif_asv_v9[asv_group] - 68;
		break;
	default:
		while(1);
	}
	IIC0_EWrite(S2MPS15_ADDR, BUCK1_OUT, WR_BUCK_VOLT_EVT1(vdd_mif));
}

void pmic_init(void)
{
	IIC0_ESetport();

	set_ldo_dvs1();
	set_ldo_dvs2();
	set_ldo_dvs3();
	set_ldo_dvs4();

	if (pmic_id >= 0x11) {
		/* BUCK Control (EVT1) */
		set_vdd_mif_buck1();
		set_vdd_with_ema(BUCK2_OUT, (float)CONFIG_PM_VDD_ATLAS);
		set_vdd_with_ema(BUCK3_OUT, (float)CONFIG_PM_VDD_APOLLO);
		set_vdd_with_ema(BUCK4_OUT, (float)CONFIG_PM_VDD_INT);
		set_vdd_with_ema(BUCK5_OUT, (float)CONFIG_PM_VDD_DISP);
		set_vdd_with_ema(BUCK6_OUT, (float)CONFIG_PM_VDD_G3D);
		writel(0x7, EXYNOS7420_SYSREG_BASE + IMEM_EMA_VROMP_HD_CON);
	}
}

#ifndef CONFIG_SPL_BUILD
void pmic_asv_init(void)
{
	unsigned char read_vol_int = 0;

	set_ldo_dvs1();
	set_ldo_dvs2();
	set_ldo_dvs3();
	set_ldo_dvs4();

	/* MIF */
	IIC0_EWrite(S2MPS15_ADDR, BUCK1_OUT,
		WR_BUCK_VOLT_EVT1((float)uMifAsvGrpVolt/1000));
	/* EGL */
	set_vdd_with_ema(BUCK2_OUT, ((float)uBigAsvGrpVolt/1000));
	/* KFC */
	set_vdd_with_ema(BUCK3_OUT, ((float)uLitAsvGrpVolt/1000));
	/* INT */
	set_vdd_with_ema(BUCK4_OUT, ((float)uIntAsvGrpVolt/1000));
	/* CAM0_DISP */
	set_vdd_with_ema(BUCK5_OUT, (float)uDisAsvGrpVolt/1000);
	/* G3D */
	set_vdd_with_ema(BUCK6_OUT, ((float)uG3dAsvGrpVolt/1000));
}

void Get_ASV_Group(void)
{
	cal_init();

	/* get voltage from table */
	uBigAsvGrpVolt = cal_get_volt(SYSC_DVFS_BIG, 13);
	uLitAsvGrpVolt = cal_get_volt(SYSC_DVFS_LIT, 8);
	uG3dAsvGrpVolt = cal_get_volt(SYSC_DVFS_G3D, 7);
	switch(cal_get_mif()) {
	case MEMCLK_1552:
		uMifAsvGrpVolt = cal_get_volt(SYSC_DVFS_MIF, 0);
		break;
	case MEMCLK_1456:
		uMifAsvGrpVolt = cal_get_volt(SYSC_DVFS_MIF, 1);
		break;
	case MEMCLK_1264:
		uMifAsvGrpVolt = cal_get_volt(SYSC_DVFS_MIF, 2);
		break;
	default:
		while(1);
		break;
	}
	uIntAsvGrpVolt = cal_get_volt(SYSC_DVFS_INT, 7);
	uDisAsvGrpVolt = cal_get_volt(SYSC_DVFS_CAM, 0);
}
#endif

void pmic_enable_peric_dev(void)
{
	unsigned char ldo_ctrl;
	unsigned char rtc_ctrl;
	unsigned char wrstbi_ctrl;
	unsigned char wrstbi_addr = 0;
	unsigned char wrstbi_en = 0;

	IIC0_ESetport();

	/* read pmic revision number */
	IIC0_ERead(S2MPS15_ADDR, S2MPS15_PMIC_ID, &pmic_id);

	if (pmic_id >= 0x11) {
		/* BUCK Control (EVT1) */
		wrstbi_addr = WRSTBI_CTRL_EVT1;
		wrstbi_en = WRSTBI_EN_EVT1;
	} else {
		/* BUCK Control (EVT0) */
		wrstbi_addr = WRSTBI_CTRL_EVT0;
		wrstbi_en = WRSTBI_EN_EVT0;
	}

	/* enable WRSTBI detection */
	IIC0_ERead(S2MPS15_ADDR, wrstbi_addr, &wrstbi_ctrl);
	wrstbi_ctrl |= wrstbi_en;
	IIC0_EWrite(S2MPS15_ADDR, wrstbi_addr, wrstbi_ctrl);

	/* BT 32KHz On, CP 32KHz On, AP 32KHz On */
	IIC0_ERead(S2MPS15_ADDR, RTC_BUF, &rtc_ctrl);
	rtc_ctrl |= (BT32KHZ_EN | CP32KHZ_EN | AP32KHZ_EN);
	IIC0_EWrite(S2MPS15_ADDR, RTC_BUF, rtc_ctrl);

	/* power control for SERIAL */
	/* enable LDO14 : VCC_2.8V_PERI */
	IIC0_ERead(S2MPS15_ADDR, LDO14_CTRL, &ldo_ctrl);
	ldo_ctrl |= OUTPUT_PWREN_ON;
	IIC0_EWrite(S2MPS15_ADDR, LDO14_CTRL, ldo_ctrl);

	/* enable LDO15 : VCC_1.8V_PERI */
	IIC0_ERead(S2MPS15_ADDR, LDO15_CTRL, &ldo_ctrl);
	ldo_ctrl |= OUTPUT_PWREN_ON;
	IIC0_EWrite(S2MPS15_ADDR, LDO15_CTRL, ldo_ctrl);

	/* enable LDO17 : VCC_2.8V_PERI */
	IIC0_ERead(S2MPS15_ADDR, LDO17_CTRL, &ldo_ctrl);
	ldo_ctrl |= OUTPUT_PWREN_ON;
	IIC0_EWrite(S2MPS15_ADDR, LDO17_CTRL, ldo_ctrl);
}
#endif
