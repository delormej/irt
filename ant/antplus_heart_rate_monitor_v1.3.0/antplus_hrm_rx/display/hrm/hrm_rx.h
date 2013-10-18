/*
This software is subject to the license described in the License.txt file
included with this software distribution. You may not use this file except in compliance
with this license.

Copyright (c) Dynastream Innovations Inc. 2013
All rights reserved.
*/

#ifndef __HRM_RX__
#define __HRM_RX__

#include "config.h"
#include "types.h"
#include "antinterface.h"
#include "antplus.h"                   
#include "antinterface.h"                   


// Page structs
typedef struct
{
   USHORT usBeatTime;                                // All pages
   UCHAR ucBeatCount;                                // All pages
   UCHAR ucComputedHeartRate;                        // All pages
} HRMPage0_Data;

typedef struct
{
   ULONG ulOperatingTime;                            // Page 1
} HRMPage1_Data;

typedef struct
{
   UCHAR ucManId;                                    // Page 2
   ULONG ulSerialNumber;                             // Page 2
} HRMPage2_Data;

typedef struct
{
   UCHAR ucHwVersion;                                // Page 3
   UCHAR ucSwVersion;                                // Page 3
   UCHAR ucModelNumber;                              // Page 3
} HRMPage3_Data;

typedef struct
{
   UCHAR ucManufSpecific;                            // Page 4
   USHORT usPreviousBeat;                            // Page 4
} HRMPage4_Data;

// Public Functions
void 
HRMRX_Init();

BOOL                                                 // TRUE is commands queued succesfully    
HRMRX_Open(
   UCHAR ucAntChannel_,                              // ANT Channel number
   USHORT usSearchDeviceNumber_,                     // ANT Channel device number
   UCHAR ucTransType_);                              // ANT Channel transmission type
   
BOOL                                                 // TRUE if command queued succesfully  
HRMRX_Close();

BOOL                                                 // TRUE if commands queued to transmit (don't sleep transmit thread)
HRMRX_ChannelEvent(
   UCHAR* pucEventBuffer_,                           // Pointer to buffer containing response from ANT
   ANTPLUS_EVENT_RETURN* pstEventStruct_);           // Pointer to event structure set by this function

HRMPage0_Data*                                       // Pointer to page 0 data
HRMRX_GetPage0();

HRMPage1_Data*                                       // Pointer to page 1 data
HRMRX_GetPage1();

HRMPage2_Data*                                       // Pointer to page 2 data
HRMRX_GetPage2();                                                       

HRMPage3_Data*                                       // Pointer to page 3 data
HRMRX_GetPage3();

HRMPage4_Data*                                       // Pointer to page 4 data
HRMRX_GetPage4();

#endif
