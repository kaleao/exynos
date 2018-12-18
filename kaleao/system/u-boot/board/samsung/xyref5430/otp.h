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

#ifndef __OTP_H__
#define __OTP_H__

#ifdef __cplusplus
extern "C" {
#endif

#define OTP_BASE	0x101E0000
#define rOTP_LOCK0							(OTP_BASE+0x000)
#define	rOTP_LOCK1							(OTP_BASE+0x004)
#define	rOTP_SECURE_READ_DATA			(OTP_BASE+0x008)
#define	rOTP_NONSECURE_READ_DATA		(OTP_BASE+0x00C)
#define	rOTP_CON_CONRTOL					(OTP_BASE+0x010)
#define	rOTP_CON_CONFIG					(OTP_BASE+0x014)
#define	rOTP_IF								(OTP_BASE+0x018)
#define	rOTP_INT_STATUS					(OTP_BASE+0x01C)
#define	rOTP_INT_EN						(OTP_BASE+0x020)
#define	rOTP_CON_TIME_PARA_0				(OTP_BASE+0x024)
#define	rOTP_CON_TIME_PARA_1				(OTP_BASE+0x028)
#define	rOTP_CON_TIME_PARA_2				(OTP_BASE+0x02C)
#define	rOTP_CON_TIME_PARA_3				(OTP_BASE+0x030)
#define	rOTP_CON_TIME_PARA_4				(OTP_BASE+0x034)
#define	rOTP_CON_TIME_PARA_5				(OTP_BASE+0x038)
#define	rOTP_CON_TIME_PARA_6				(OTP_BASE+0x03C)
#define	rOTP_CON_TIME_PARA_7				(OTP_BASE+0x040)
#define	rOTP_ADD_LOCK						(OTP_BASE+0x044)
#define	rOTP_CUSTOM_LOCK0					(OTP_BASE+0x048)
#define	rOTP_CUSTOM_LOCK01				(OTP_BASE+0x04C)

#define 	OTP_LOCK_KEY_BASE					0x7C00
#define 	OTP_LOCK_KEY20_AREA_BASE		64

typedef unsigned int u32;
typedef unsigned char u8;

enum
{
	NO_FAIL = 0,
	INIT_FAIL,
	STANDBY_FAIL,
	PROGRAM_FAIL,
	LOCK_PROGRAM_FAIL,
	SECURE_FAIL,
	PROGRAM_LOCK,
	UNACCESSIBLE_REGION,
	TIME_OUT
};

enum
{
	OTP_KEY0 = 0x7C00,
	OTP_KEY1 = 0x7C01,
	OTP_KEY2 = 0x7C02,
	OTP_KEY3 = 0x7C03,
	OTP_KEY4 = 0x7C04,
	OTP_KEY5 = 0x7C05,
	OTP_KEY6 = 0x7C06,
	OTP_KEY7 = 0x7C07,
	OTP_KEY8 = 0x7C08,
	OTP_KEY9 = 0x7C09,
	OTP_KEY10 = 0x7C0A,
	OTP_KEY11 = 0x7C0B,
	OTP_KEY12 = 0x7C0C,
	OTP_KEY13 = 0x7C0D,
	OTP_KEY14 = 0x7C0E,
	OTP_KEY15 = 0x7C0F,
	OTP_KEY16 = 0x7C10,
	OTP_KEY17 = 0x7C11,
	OTP_KEY18 = 0x7C12,
	OTP_KEY19 = 0x7C13,
	OTP_KEY20 = 0x7C14,
	OTP_KEY21 = 0x7C15,
	OTP_KEY22 = 0x7C16,
	OTP_KEY23 = 0x7C17,
	OTP_KEY24 = 0x7C18,
	OTP_KEY25 = 0x7C19,
	OTP_KEY26 = 0x7C1A,
	OTP_KEY27 = 0x7C1B,
	OTP_KEY28 = 0x7C1C,
	OTP_KEY29 = 0x7C1D,
	OTP_KEY30 = 0x7C1E,
	OTP_KEY31 = 0x7C1F,
	OTP_KEY20_0 = 0x7C41,
	OTP_KEY20_1 = 0x7C42,
	OTP_KEY20_2 = 0x7C43,
	OTP_KEY20_3 = 0x7C44,
	OTP_KEY20_4 = 0x7C45,
	OTP_KEY20_5 = 0x7C46,
	OTP_KEY20_6 = 0x7C47,
	OTP_KEY20_7 = 0x7C48,
};
int otp_cmd_init(void);
int otp_cmd_standby(void);
u32 otp_cmd_read(u32 ulAddress, u32 *pulError);
int otp_read(u32 ulAddress, u8 *pucData, u32 ulSize, u32 *pulError);
#endif //__OTP_H__
