
typedef unsigned int u32;
typedef int s32;

typedef enum
{
	SYSC_DVFS_L0_A=-1,
	SYSC_DVFS_L0=0,
	SYSC_DVFS_L1,
	SYSC_DVFS_L2,
	SYSC_DVFS_L3,
	SYSC_DVFS_L4,
	SYSC_DVFS_L5,
	SYSC_DVFS_L6,
	SYSC_DVFS_L7,
	SYSC_DVFS_L8,
	SYSC_DVFS_L9,
	SYSC_DVFS_L10,
	SYSC_DVFS_L11,
	SYSC_DVFS_L12,
	SYSC_DVFS_L13,
	SYSC_DVFS_L14,
	SYSC_DVFS_L15,
	SYSC_DVFS_L16,
	SYSC_DVFS_L17,
	SYSC_DVFS_L18,
	SYSC_DVFS_L19,
	SYSC_DVFS_L20,
	SYSC_DVFS_L21,
	SYSC_DVFS_L22,
	SYSC_DVFS_L23,
	SYSC_DVFS_FIN=0x100,
} SYSC_DVFS_LVL;

typedef enum
{
	SYSC_ASV_0=0 ,
	SYSC_ASV_1,
	SYSC_ASV_2,
	SYSC_ASV_3,
	SYSC_ASV_4,
	SYSC_ASV_5=5,
	SYSC_ASV_6,
	SYSC_ASV_7,
	SYSC_ASV_8,
	SYSC_ASV_9,
	SYSC_ASV_10,
	SYSC_ASV_11 = 11,
	SYSC_ASV_12 = 12,
	SYSC_ASV_13,
	SYSC_ASV_14,
	SYSC_ASV_15 = 15,
	SYSC_ASV_MAX
} SYSC_ASV_GROUP;

typedef enum
{
	SYSC_DVFS_EGL,
	SYSC_DVFS_KFC,
	SYSC_DVFS_MIF,
	SYSC_DVFS_INT,
	SYSC_DVFS_G3D,

	SYSC_DVFS_CAM,
	SYSC_DVFS_DISP,

	SYSC_DVFS_NUM
} SYSC_DVFS_SEL;

#define CHIPID_ASV_EGL_BASE 0x10004010
#define CHIPID_ASV_KFC_BASE 0x10004014
#define CHIPID_ASV_G3D_BASE 0x10004018
#define CHIPID_ASV_MIF_BASE 0x10004018
#define CHIPID_ASV_INT_BASE 0x1000401C
#define CHIPID_ASV_CAM_BASE 0x1000401C

const u32 g_uChipidBase[SYSC_DVFS_NUM][4]= {
	[SYSC_DVFS_EGL] = { CHIPID_ASV_EGL_BASE, 0 },
	[SYSC_DVFS_KFC] = { CHIPID_ASV_KFC_BASE, 0 },
	[SYSC_DVFS_G3D] = { CHIPID_ASV_G3D_BASE, 0 },
	[SYSC_DVFS_MIF] = { CHIPID_ASV_MIF_BASE, 16 },
	[SYSC_DVFS_INT] = { CHIPID_ASV_INT_BASE, 0 },
	[SYSC_DVFS_CAM] = { CHIPID_ASV_CAM_BASE, 16 },
};

static volatile s32 g_uSwAsvGroup[SYSC_DVFS_NUM][3] = {
	[SYSC_DVFS_EGL] = { -1,-1,-1 },
	[SYSC_DVFS_KFC] = { -1,-1,-1 },
	[SYSC_DVFS_G3D] = { -1,-1,-1 },
	[SYSC_DVFS_MIF] = { -1,-1,-1 },
	[SYSC_DVFS_INT] = { -1,-1,-1 },
	[SYSC_DVFS_CAM] = { -1,-1,-1 },
};


/* applied DVFS v0.91 */
#define ASV_VOLTAGE_GROUP_NUM	4
const u32 g_AsvGrpVoltage[SYSC_DVFS_NUM][ASV_VOLTAGE_GROUP_NUM]= {
                      // ASV0      ASV1~5   ASV6~10  ASV11~15
	[SYSC_DVFS_EGL] = { 10000000,  9500000, 9000000, 9000000 },
	[SYSC_DVFS_KFC] = { 9750000,  9750000, 9125000, 8500000 },
	[SYSC_DVFS_MIF] = { 10500000, 10250000, 9625000, 9000000 },
	[SYSC_DVFS_INT] = { 10750000, 10500000, 9875000, 9250000 },
	[SYSC_DVFS_G3D] = { 10000000,  9750000, 9125000, 8500000 },
};

const u32 g_AsvGrpVoltage_v1[SYSC_DVFS_NUM][ASV_VOLTAGE_GROUP_NUM]= {
                      // ASV0      ASV1~5   ASV6~10  ASV11~15
	[SYSC_DVFS_EGL] = { 9250000,  9000000, 9000000, 9000000 },
	[SYSC_DVFS_KFC] = { 10250000,  10000000, 9125000, 8500000 },
	[SYSC_DVFS_MIF] = { 10250000, 10000000, 9375000, 8750000 },
	[SYSC_DVFS_INT] = { 9750000, 9500000, 8750000, 8125000 },
	[SYSC_DVFS_G3D] = { 8875000,  8750000, 8188000, 7875000 },
};

typedef enum
{
	ASV_GROUP0	= 0x0,
	ASV_GROUP1_5	= 0x1,
	ASV_GROUP6_10	= 0x2,
	ASV_GROUP11_15	= 0x3
} ASV_GROUP;

#define Inp32(addr) (*(volatile u32 *)(addr))
#define Outp32(addr, data)      (*(volatile u32 *)(addr) = (data))
#define GetBits(uAddr, uBaseBit, uMaskValue) ((Inp32(uAddr)>>(uBaseBit))&(uMaskValue))

#define CHIPID_ASV_TABLE_VERSION 0x10004020
#define FS_ABB_CONTROL_REG	 0x10004028

#define FS_ABB_EGL_OFFSET		(0)
#define FS_ABB_KFC_OFFSET		(4)
#define FS_ABB_G3D_OFFSET		(8)
#define FS_ABB_MIF_OFFSET		(12)
#define FS_ABB_INT_OFFSET		(16)
#define FS_ABB_MASK			(0xF)
#define FS_ABB_MASTER_BIT_OFFSET	(28)
#define FS_ABB_MASTER_BIT_MASK		(0x1)

#define FBB_EGL_BASE 0x105C0780
#define FBB_KFC_BASE 0x105C0784
#define FBB_G3D_BASE 0x105C0790
#define FBB_MIF_BASE 0x105C078C
#define FBB_INT_BASE 0x105C0788

u32 uEglAsvGrpVolt;
u32 uKfcAsvGrpVolt;
u32 uMifAsvGrpVolt;
u32 uIntAsvGrpVolt;
u32 uG3dAsvGrpVolt;

u32 DVFS_GetAsvTableVersion(void)
{
	return Inp32(CHIPID_ASV_TABLE_VERSION)&0x3;
}

u32 DVFS_GetFbbinfo(void)
{
	return Inp32(FS_ABB_CONTROL_REG);
}

u32 DVFS_GetSubGroup(SYSC_DVFS_SEL eSel, SYSC_DVFS_LVL eLvl)
{
	u32 subgrp = 0; // version 0

	if (DVFS_GetAsvTableVersion() == 1)
	{
		switch(eSel)
		{
			case SYSC_DVFS_EGL:
				subgrp = (eLvl <= SYSC_DVFS_L8) ? 0 :
					(eLvl <= SYSC_DVFS_L13) ? 1 : 2;
				break;
			case SYSC_DVFS_KFC:
				subgrp = (eLvl <= SYSC_DVFS_L8) ? 0 :
					(eLvl <= SYSC_DVFS_L13) ? 1 : 2;
				break;
			case SYSC_DVFS_G3D:
				subgrp = (eLvl <= SYSC_DVFS_L2) ? 0 :
					(eLvl <= SYSC_DVFS_L4) ? 1 : 2;
				break;
			case SYSC_DVFS_MIF:
				subgrp = (eLvl <= SYSC_DVFS_L0) ? 0 :
					(eLvl <= SYSC_DVFS_L3) ? 1 : 2;
				break;
			case SYSC_DVFS_INT:
				subgrp = (eLvl <= SYSC_DVFS_L0) ? 0 :
					(eLvl <= SYSC_DVFS_L3) ? 1 : 2;
				break;
			case SYSC_DVFS_CAM:
				subgrp = (eLvl <= SYSC_DVFS_L0) ? 0 :
					(eLvl <= SYSC_DVFS_L2) ? 1 : 2;
				break;
			default:
				while(1);
		}
	}

	return subgrp;
}

const char * DVFS_GetDvfsName(SYSC_DVFS_SEL eSel)
{
	return (eSel == SYSC_DVFS_EGL) ? "EGL" :
		(eSel == SYSC_DVFS_KFC) ? "KFC" :
		(eSel == SYSC_DVFS_G3D) ? "G3D" :
		(eSel == SYSC_DVFS_MIF) ? "MIF" :
		(eSel == SYSC_DVFS_INT) ? "INT" :
		(eSel == SYSC_DVFS_CAM) ? "CAM" : "DISP";
}

SYSC_ASV_GROUP DVFS_GetAsvGroup(SYSC_DVFS_SEL eSel, SYSC_DVFS_LVL eLvl)
{
	u32 subgrp, uAsvGroup=7;

	subgrp = DVFS_GetSubGroup(eSel, eLvl);

	if (DVFS_GetAsvTableVersion() == 0 && eSel == SYSC_DVFS_CAM)
	{
		eSel = SYSC_DVFS_INT;
	}

	uAsvGroup = GetBits(g_uChipidBase[eSel][0], g_uChipidBase[eSel][1] + subgrp*4, 0xf);

	if (g_uSwAsvGroup[eSel][subgrp]>=0)
	{
		uAsvGroup = g_uSwAsvGroup[eSel][subgrp];
	}
	else;

	return (SYSC_ASV_GROUP)uAsvGroup;
}



u32 GetAsvGroupbyAsv(u32 uAsv)
{
	if(uAsv >= 1 && uAsv <= 5)
		return ASV_GROUP1_5;
	else if(uAsv >= 6 && uAsv <= 10)
		return ASV_GROUP6_10;
	else if(uAsv >= 11 && uAsv <= 15)
		return ASV_GROUP11_15;
	else
		return ASV_GROUP0;
}


u32 GetVoltagebyAsvGroup(ASV_GROUP eAsvGroup, SYSC_DVFS_SEL eDvfsSel)
{
	if (DVFS_GetAsvTableVersion() == 1)
		return g_AsvGrpVoltage_v1[eDvfsSel][eAsvGroup];
	else
		return g_AsvGrpVoltage[eDvfsSel][eAsvGroup];

}

void Get_ASV_Group(void)
{
	u32 uEglAsv;
	u32 uKfcAsv;
	u32 uMifAsv;
	u32 uIntAsv;
	u32 uG3dAsv;

	u32 uEglAsvGrp;
	u32 uKfcAsvGrp;
	u32 uMifAsvGrp;
	u32 uIntAsvGrp;
	u32 uG3dAsvGrp;

	u32 fs_abb_con_reg;
	u32 fs_abb_egl;
	u32 fs_abb_kfc;
	u32 fs_abb_g3d;
	u32 fs_abb_mif;
	u32 fs_abb_int;
	u32 is_fs_abb;
	u32 tmp;

	fs_abb_con_reg = DVFS_GetFbbinfo();
	is_fs_abb = (fs_abb_con_reg >> FS_ABB_MASTER_BIT_OFFSET) & FS_ABB_MASTER_BIT_MASK;

	if (is_fs_abb) {
		tmp = (fs_abb_con_reg >> FS_ABB_EGL_OFFSET) & FS_ABB_MASK;
		fs_abb_egl = (1 << 31) | (1 << 7) | (tmp);
		Outp32(FBB_EGL_BASE, fs_abb_egl);

		tmp = (fs_abb_con_reg >> FS_ABB_KFC_OFFSET) & FS_ABB_MASK;
		fs_abb_kfc = (1 << 31) | (1 << 7) | (tmp);
		Outp32(FBB_KFC_BASE, fs_abb_kfc);

		tmp = (fs_abb_con_reg >> FS_ABB_G3D_OFFSET) & FS_ABB_MASK;
		fs_abb_g3d = (1 << 31) | (1 << 7) | (tmp);
		Outp32(FBB_G3D_BASE, fs_abb_g3d);

		tmp = (fs_abb_con_reg >> FS_ABB_MIF_OFFSET) & FS_ABB_MASK;
		fs_abb_mif = (1 << 31) | (1 << 7) | (tmp);
		Outp32(FBB_MIF_BASE, fs_abb_mif);

		tmp = (fs_abb_con_reg >> FS_ABB_INT_OFFSET) & FS_ABB_MASK;
		fs_abb_int = (1 << 31) | (1 << 7) | (tmp);
		Outp32(FBB_INT_BASE, fs_abb_int);
	}

	// GET ASV VALUE
	uEglAsv = DVFS_GetAsvGroup(SYSC_DVFS_EGL, 20); /* 500Mhz level 20 */
	uKfcAsv = DVFS_GetAsvGroup(SYSC_DVFS_KFC, 12); /* 800Mhz level 12 */
	uMifAsv = DVFS_GetAsvGroup(SYSC_DVFS_MIF, 0); /* 825Mhz level 0 */
	uIntAsv = DVFS_GetAsvGroup(SYSC_DVFS_INT, 0); /* 400Mhz level 0 */
	uG3dAsv = DVFS_GetAsvGroup(SYSC_DVFS_G3D, 5); /* 266Mhz level 5 */

	// GET ASV VOLTAGE GROUP
	uEglAsvGrp = GetAsvGroupbyAsv(uEglAsv);
	uKfcAsvGrp = GetAsvGroupbyAsv(uKfcAsv);
	uMifAsvGrp = GetAsvGroupbyAsv(uMifAsv);
	uIntAsvGrp = GetAsvGroupbyAsv(uIntAsv);
	uG3dAsvGrp = GetAsvGroupbyAsv(uG3dAsv);

	// GET VOLTAGE BY ASV VOLTAGE GROUP
	uEglAsvGrpVolt = GetVoltagebyAsvGroup(uEglAsvGrp,SYSC_DVFS_EGL);
	uKfcAsvGrpVolt = GetVoltagebyAsvGroup(uKfcAsvGrp,SYSC_DVFS_KFC);
	uMifAsvGrpVolt = GetVoltagebyAsvGroup(uMifAsvGrp,SYSC_DVFS_MIF);
	uIntAsvGrpVolt = GetVoltagebyAsvGroup(uIntAsvGrp,SYSC_DVFS_INT);
	uG3dAsvGrpVolt = GetVoltagebyAsvGroup(uG3dAsvGrp,SYSC_DVFS_G3D);
}
