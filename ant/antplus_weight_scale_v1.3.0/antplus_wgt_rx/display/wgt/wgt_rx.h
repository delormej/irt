/*
This software is subject to the license described in the License.txt file
included with this software distribution. You may not use this file except in compliance
with this license.

Copyright (c) Dynastream Innovations Inc. 2013
All rights reserved.
*/

/////////////////////////////////////////////////////////////////////////////////////////
//Vipin Bali Bakshi
//Date:June 9, 2010
/////////////////////////////////////////////////////////////////////////////////////////
#ifndef WGT_RX_H_
#define WGT_RX_H_

#include "config.h"
#include "types.h"
#include "antinterface.h"
#include "antplus.h"                   
#include "antinterface.h"
#include "common_pages.h"

typedef struct
{
	USHORT usUserProfileInformation;
	UCHAR ucCapabilitiesField;
	USHORT usWeightValue;
}WGT_DataPage1;

typedef struct
{
	USHORT usUserProfileInformation;
	UCHAR ucReserved;
	USHORT usHydration;
	USHORT usBodyFat;
}WGT_DataPage2;

typedef struct
{
	USHORT usUserProfileInformation;
	UCHAR ucReserved;
	USHORT usActiveRate;
	USHORT usBasalRate;
}WGT_DataPage3;

typedef struct
{
	USHORT usUserProfileInformation;
	UCHAR ucReserved1;
	UCHAR ucReserved2;
	USHORT usMuscleMass;
	UCHAR ucBoneMass;
}WGT_DataPage4;

//User Profile Page
typedef struct
{
	USHORT usUserProfileIdentification;
	UCHAR ucCapabilitiesBitField;
	UCHAR ucReserved;
	UCHAR ucGender;
	UCHAR ucAge;
	UCHAR ucHeight;
	UCHAR ucDescriptiveBitField;
}WGT_UserProfilePage58;

typedef struct
{
	USHORT usUserProfileIdentification;
	UCHAR ucCapabilitiesBitField;
	UCHAR ucReserved;
	UCHAR ucGender;
	UCHAR ucAge;
	UCHAR ucHeight;
	UCHAR ucDescriptiveBitField;
}Display_UserProfilePage;

//Public Functions	
WGT_DataPage1* WGT_GetDataPage1();
WGT_DataPage2* WGT_GetDataPage2();
WGT_DataPage3* WGT_GetDataPage3();
WGT_DataPage4* WGT_GetDataPage4();
WGT_UserProfilePage58* WGT_GetUserProfilePage58();
//CommonPage70_Data* WGT_GetCommonDataPage70();					ANT_FS capabilities not in use
CommonPage80_Data* WGT_GetCommonDataPage80();
CommonPage81_Data* WGT_GetCommonDataPage81();
CommonPage82_Data* WGT_GetCommonDataPage82();
Display_UserProfilePage* Display_GetUserProfilePage();

void WGTRX_Init();
BOOL WGTRX_Open(UCHAR ucAntChannel_, USHORT usSearchDeviceNumber_, UCHAR ucTransType_);
BOOL WGTRX_Close();
BOOL WGTRX_ChannelEvent(UCHAR* pucEventBuffer_, ANTPLUS_EVENT_RETURN* pstEventStruct_);

BOOL Display_UserInformationInit();

//MOBILE DISPLAY DECISION TREE FUNCTIONS
BOOL Branch_Main_Page();
BOOL ProcessScaleCapabilities();
BOOL TransmitUserProfile();
void Branch_User_Page();
BOOL ScalePriorityTransmit();
BOOL ScalePriorityRecieve();
static BOOL ggbBranchUserPage;
#endif /*WGT_RX_H_*/

		
