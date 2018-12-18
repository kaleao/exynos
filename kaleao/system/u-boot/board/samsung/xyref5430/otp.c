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

#include "otp.h"

#define HW_REG32(base, offset)	(*(volatile u32 *)(base + (offset)))
#define Outp32(addr, data) (*(volatile u32 *)(addr) = (data))
#define Inp32(addr) (*(volatile u32 *)(addr))
#define DBG_MSG 0
#define ERROR_MSG 1
#define false 0
#define true 1

int otp_cmd_init(void)
{
	u32 ulreg;
	u32 uErrorCnt = 0;
	int bResult = false;

	Outp32(rOTP_CON_CONRTOL, 0x01);
	while(1)
	{
		if(Inp32(rOTP_INT_STATUS) & 0x01)
		{
			bResult = true;
			break;
		}
		uErrorCnt ++;
		if(uErrorCnt > 0xffffff)
		{
			bResult = false;
			break;
		}
	}
	ulreg = Inp32(rOTP_INT_STATUS);
	Outp32(rOTP_INT_STATUS, (ulreg |0x01));
	return bResult;
}
int otp_cmd_standby(void)
{
	u32 ulreg;
	u32 uErrorCnt = 0;
	int bResult = false;
	// 1. set standby command
	ulreg = Inp32(rOTP_CON_CONRTOL);
	Outp32(rOTP_CON_CONRTOL, (ulreg & 0xfffffff7)|0x08);
	while(1)
	{
		if(Inp32(rOTP_INT_STATUS) & 0x08)
		{
			bResult = true;
			break;
		}
		uErrorCnt ++;
		if(uErrorCnt > 0xffffff)
		{
			bResult = false;
			break;
		}
	}
	ulreg = Inp32(rOTP_INT_STATUS);
		Outp32(rOTP_INT_STATUS, (ulreg |0x08));
	return bResult;
}

u32 otp_cmd_read(u32 ulAddress, u32 *pulError)
{
	//DEBUGMSG(DBG_MSG, (" %s\n", __FUNCTION__));
	volatile u32 ulreg;
	u32 ulErrorCnt = 0;
	u32 ulReadData = 0;
	//DEBUGMSG(DBG_MSG, ("ulAddress: 0x%X\n", ulAddress));
	*pulError = NO_FAIL;
	// 1. set address
	// OTP_IF: program data[31],address [14:0]
	ulreg = Inp32(rOTP_IF);
	ulreg = (ulreg & 0xFFFF0000) | (ulAddress & 0x7FFF);
	Outp32(rOTP_IF, ulreg);
	// 2. set read command
	ulreg = Inp32(rOTP_CON_CONRTOL);
	Outp32(rOTP_CON_CONRTOL, (ulreg & 0xFFFFFFFD)|0x02);
	// 3. check read status
	while(1)
	{
		ulreg = Inp32(rOTP_INT_STATUS);
		// check read done
		if(ulreg & 0x02)
		{
			//DEBUGMSG(DBG_MSG, ("pulError = NO_FAIL (0x%x)\n",*pulError));
			break;
		}
		// Check secure fail
		if(ulreg & 0x80)
		{
			*pulError = SECURE_FAIL;
			//DEBUGMSG(ERROR_MSG, ("pulError = SECURE_FAIL (0x%x)\n",*pulError));
			break;
		}
		ulErrorCnt ++;
		if(ulErrorCnt > 0xffffff)
		{
			*pulError = TIME_OUT;
			//DEBUGMSG(ERROR_MSG, ("pulError = SECURE_FAIL (0x%x)\n",*pulError));
			break;
		}
	}
	if(*pulError == NO_FAIL)
	{
		// checking bit [14:13]
		ulreg = (Inp32(rOTP_IF) & 0x6000) >> 13;
		// 4-1 read SECURE DATA [bit [14:13]= 1:0 or 1:1]
		if(ulreg & 0x2)
		{
			ulReadData = Inp32(rOTP_SECURE_READ_DATA);
			//DEBUGMSG(DBG_MSG, ("read SECURE DATA= 0x%x\n",ulReadData));
		}
		// 4-2 Hardware only accessible [bit [14:13]= 0:1]
		else if(ulreg & 0x1)
		{
			*pulError = UNACCESSIBLE_REGION;
			//DEBUGMSG(ERROR_MSG, ("UNACCESSIBLE_REGION\n"));
		}
		// 4-3 read NON SECURE DATA [bit [14:13]= 0:0]
		else if(!(ulreg & 0x3))
		{
			ulReadData = Inp32(rOTP_NONSECURE_READ_DATA);
			//DEBUGMSG(DBG_MSG, ("read NON SECURE DATA= 0x%x\n",ulReadData));
		}
		// 4-3 Hardware only accessible [bit [14:13]= 0:1]
		else if(ulreg == 1)
		{
			*pulError = UNACCESSIBLE_REGION;
		}
	}
	if(*pulError == NO_FAIL)
	{
		ulreg = Inp32(rOTP_INT_STATUS);
		Outp32(rOTP_INT_STATUS, (ulreg |0x02));
	}
	return ulReadData;
}
int otp_read(u32 ulAddress, u8 *pucData, u32 ulSize, u32 *pulError)
{
	u32 ulCount;
	u32 ulDataShiftCount;
	u32 ulDataCount = 0;
	u32 ulData;
	*pulError = NO_FAIL;
	if(otp_cmd_init() == false)
	{
		*pulError = INIT_FAIL;
	}
	if(*pulError != INIT_FAIL)
	{
		for(ulCount = 0 ; ulCount < (ulSize/32) ; ulCount++)
		{
			ulData = otp_cmd_read(ulAddress+(ulCount*32),pulError);
			if(*pulError != NO_FAIL)
				break;
			for(ulDataShiftCount = 0 ; ulDataShiftCount < 32 ; ulDataShiftCount ++)
			{
				pucData[ulDataCount++] = (unsigned char)((ulData >> ulDataShiftCount) & 0x1);
			}
		}
	}
	if(otp_cmd_standby() == false)
	{
		*pulError = STANDBY_FAIL;
	}
	if(*pulError != NO_FAIL)
		return false;
	else
		return true;
}
// example code
void OPT_Bank3_read_test(void)
{
	// 0x400: bit numbers in each bank
	// 3: Bank number
	u8	READ_ARRAY[1024] = {0};
	u32 ulErrorNo = NO_FAIL;
	u32	ulReadSize = 32;// (32 aline 0,32,64,96....)
	u32	uloffset = 0;// (32 aline 0,32,64,96....)
	u32 ulBaseAddress = 0x400 * 3;
	u32	ulAddress = ulBaseAddress + uloffset;
	if(otp_read(ulAddress,READ_ARRAY,ulReadSize,&ulErrorNo)== false)
	{
		// fail
		printf("Test fail(Error no:%d)\n",ulErrorNo);
	}else
	{
		//pass
		printf("Test Pass\n");
	}

}

int otp_tmu_parse(u32 ulOffset)
{
	int ulSize = 32;
	int ret = 0;
	u32 ErrorCnt = 0;
	u8 pucData[32]={0,};
	u32 i=0;
	u32 ulBaseAddress = 0x400 * 3;

	if (otp_read((ulBaseAddress+(32*ulOffset)), pucData, ulSize, &ErrorCnt)) {
		for (i = 0; i < 8; i++) {
			if (pucData[i*2] == 1) {
				ret = ret | (0x1 << i);
			}
		}
	}
 return ret;
}
