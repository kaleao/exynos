/*
 * Copyright@ Samsung Electronics Co. LTD
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.


 * Alternatively, this program is free software in case of open source projec;
 * you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.

 */

typedef unsigned int u32;

#define Inp32(addr) (*(volatile u32 *)(addr))
#define MAX_ASV_GROUP 16

u32 uEglAsvGrpVolt;
u32 uKfcAsvGrpVolt;
u32 uG3dAsvGrpVolt;
u32 uMifAsvGrpVolt;
u32 uIntAsvGrpVolt;
u32 uCamAsvGrpVolt;

/* L20 */
const u32 g_uVtableEgl[MAX_ASV_GROUP] = {
	900000,	900000,	900000,	900000,	900000,
	900000,	900000,	900000,	900000,	900000,
	900000,	900000,	900000,	900000,	900000,
	900000,
};

/* L12 */
const u32 g_uVtableKfc[MAX_ASV_GROUP] = {
	1012500, 1000000, 975000, 950000, 937500,
	925000, 912500, 900000, 887500, 875000,
	862500, 850000, 837500, 825000, 812500,
	800000,
};

/* L5 */
const u32 g_uVtableG3d[MAX_ASV_GROUP] = {
	1000000, 987500, 975000, 962500, 950000,
	937500, 925000, 912500, 912500, 900000,
	900000, 887500, 887500, 875000, 875000,
	862500,
};

/* L0 */
const u32 g_uVtableMif[MAX_ASV_GROUP] = {
	1062500, 1050000, 1037500, 1025000, 1012500,
	1000000, 987500, 975000, 962500, 950000,
	950000, 937500, 937500, 925000, 925000,
	925000,
};

/* L0 */
const u32 g_uVtableInt[MAX_ASV_GROUP] = {
	987500, 975000, 962500, 950000, 937500,
	925000, 912500, 900000, 887500, 875000,
	862500, 850000, 837500, 825000, 812500,
	800000,
};

/* L0 */
const u32 g_uVtableCam[MAX_ASV_GROUP] = {
	1012500, 1000000, 987500, 975000, 962500,
	950000, 937500, 925000, 925000, 912500,
	912500, 900000, 900000, 887500, 887500,
	875000,
};

void Get_ASV_Group(void)
{
	u32 ASV;
	u32 IDS_EGL; /* mA */

	/* get ASV value from IDS_EGL */
	IDS_EGL = Inp32(0x105d0000) & 0xff;
	if ((IDS_EGL >= 1) && (IDS_EGL <= 20))
		ASV = 1;
	else if ((IDS_EGL >= 21) && (IDS_EGL <= 25))
		ASV = 2;
	else if ((IDS_EGL >= 26) && (IDS_EGL <= 30))
		ASV = 3;
	else if ((IDS_EGL >= 31) && (IDS_EGL <= 36))
		ASV = 4;
	else if ((IDS_EGL >= 37) && (IDS_EGL <= 43))
		ASV = 5;
	else if ((IDS_EGL >= 44) && (IDS_EGL <= 51))
		ASV = 6;
	else if ((IDS_EGL >= 52) && (IDS_EGL <= 60))
		ASV = 7;
	else if ((IDS_EGL >= 61) && (IDS_EGL <= 70))
		ASV = 8;
	else if ((IDS_EGL >= 71) && (IDS_EGL <= 81))
		ASV = 9;
	else if ((IDS_EGL >= 82) && (IDS_EGL <= 93))
		ASV = 10;
	else if ((IDS_EGL >= 94) && (IDS_EGL <= 106))
		ASV = 11;
	else if ((IDS_EGL >= 107) && (IDS_EGL <= 120))
		ASV = 12;
	else if ((IDS_EGL >= 121) && (IDS_EGL <= 135))
		ASV = 13;
	else if ((IDS_EGL >= 136) && (IDS_EGL <= 140))
		ASV = 14;
	else
		ASV = 0;

	/* get voltage from table */
	uEglAsvGrpVolt = g_uVtableEgl[ASV];
	uKfcAsvGrpVolt = g_uVtableKfc[ASV];
	uG3dAsvGrpVolt = g_uVtableG3d[ASV];
	uMifAsvGrpVolt = g_uVtableMif[ASV];
	uIntAsvGrpVolt = g_uVtableInt[ASV];
	uCamAsvGrpVolt = g_uVtableCam[ASV];
}
