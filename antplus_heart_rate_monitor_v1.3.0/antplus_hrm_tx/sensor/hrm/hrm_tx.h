/*
This software is subject to the license described in the License.txt file
included with this software distribution. You may not use this file except in compliance
with this license.

Copyright (c) Dynastream Innovations Inc. 2013
All rights reserved.
*/

#ifndef __HRM_TX__
#define __HRM_TX__

#include "config.h"
#include "types.h"
#include "antinterface.h"
#include "antplus.h"                   


// Global Page Settings
#define HRMTX_MFG_ID								  ((UCHAR) 2)			// Manufacturer ID
#define HRMTX_SERIAL_NUMBER					  ((UCHAR) 0xABCD)	// Serial Number
#define HRMTX_HW_VERSION						  ((UCHAR) 5)			// HW Version
#define HRMTX_SW_VERSION						  ((UCHAR) 0)        // SW Version
#define HRMTX_MODEL_NUMBER						  ((UCHAR) 2)			// Model Number

// Publc Functions
void 
HRMTX_Init();

BOOL                                                           // TRUE is commands queued succesfully 
HRMTX_Open(
   UCHAR ucAntChannel_,                                        // ANT Channel number
   USHORT usDeviceNumber_,                                     // ANT Channel device number
   UCHAR ucTransType_);                                        // ANT Channel transmission type
   
BOOL                                                           // TRUE if command queued succesfully  
HRMTX_Close();

BOOL                                                           // TRUE if commands queued to transmit (don't sleep transmit thread)
HRMTX_ChannelEvent(
   UCHAR* pucEventBuffer_,                                     // Pointer to buffer containing response from ANT
   ANTPLUS_EVENT_RETURN* pstEventStruct_);                     // Pointer to event structure set by this function
   

#endif
