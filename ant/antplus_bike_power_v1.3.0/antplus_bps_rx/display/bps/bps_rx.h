/*
This software is subject to the license described in the License.txt file
included with this software distribution. You may not use this file except in compliance
with this license.

Copyright (c) Dynastream Innovations Inc. 2013
All rights reserved.
*/

#ifndef __BPS_RX__
#define __BPS_RX__

#include "config.h"
#include "types.h"
#include "antinterface.h"
#include "antplus.h"                                      
#include "common_pages.h"
#include "bps_pages.h"
      
// BPS RX constants for calculations
#define BPSRX_WHEEL_CIRCUMFERENCE            ((USHORT)  2)     // Default Bike wheel circumference (m)
#define BPSRX_MAX_REPEATS                    ((USHORT) 12)     // Set Max # of no updates before zero speed set  
#define BPSRX_INITIAL_OFFSET	             ((USHORT)100)
#define BPSRX_ZERO_SPEED		             ((UCHAR)   0)

// BPS RX constants for calibration
#define BPSRX_DEFINED_SLOPE		             ((USHORT) 200)    // Default Bike wheel circumference (m)
#define BPSRX_DEFINED_SERIAL                 ((USHORT) 67890)  // Set Max # of no updates (event count for event sync or no increase in  
#define BPSRX_CALIBRATION_TIMEOUT            ((USHORT) 480)

// BPS Subevent defines
#define BPSRX_SUBEVENT_UPDATE         ((USHORT) 0)
#define BPSRX_SUBEVENT_NOUPDATE       ((USHORT) 1)
#define BPSRX_SUBEVENT_ZERO_SPEED     ((USHORT) 2)
#define BPSRX_SUBEVENT_COASTING       ((USHORT) 3)
#define BPSRX_SUBEVENT_FIRSTPAGE      ((USHORT) 4)


// global pages define in common_pages.h file, PBS in pbs_pages.h

//calculated data structures
typedef struct
{
	ULONG ulAvgStandardPower;
}CalculatedStdPower;

typedef struct
{
	ULONG ulWTAvgSpeed;
	ULONG ulWTDistance;
	ULONG ulWTAngularVelocity;
	ULONG ulWTAverageTorque;
	ULONG ulWTAveragePower;
}CalculatedStdWheelTorque;

typedef struct
{
	ULONG ulCTAverageCadence;
	ULONG ulCTAngularVelocity;
	ULONG ulCTAverageTorque;
	ULONG ulCTAveragePower;
}CalculatedStdCrankTorque;

typedef struct
{
	ULONG ulCTFAverageCadence;
	ULONG ulCTFTorqueFrequency;
	ULONG ulCTFAverageTorque;
	ULONG ulCTFAveragePower;
}CalculatedStdCTF;

typedef enum
{
   CALIBRATION_TYPE_MANUAL,                                
   CALIBRATION_TYPE_AUTO,                                  
   CALIBRATION_TYPE_SLOPE,                             
   CALIBRATION_TYPE_SERIAL                             
} CalibrationType;

// Public Functions
void 
BPSRX_Init();

BOOL                                                 // TRUE is commands queued succesfully    
BPSRX_Open(
   UCHAR ucAntChannel_,                              // ANT Channel Number
   USHORT usSearchDeviceNumber_,                     // ANT Channel device number
   UCHAR ucTransType_);                              // ANT Channel transmission type
   
BOOL                                                 // TRUE if command queued succesfully  
BPSRX_Close();

void 												// Returns TRUE
BPSRX_PulseEvent(                                    // Time of pulse event in (1/1024)s
	USHORT usTime1024, 
	UCHAR ucAlarmNumber_);

BOOL                                                 // TRUE if commands queued to transmit (don't sleep transmit thread)
BPSRX_ChannelEvent(
   UCHAR* pucEventBuffer_,                           // Pointer to buffer containing response from ANT
   ANTPLUS_EVENT_RETURN* pstEventStruct_);           // Pointer to event structure set by this function

BPSPage1_Data*                                       // Pointer to page 1 data
BPSRX_GetPage1();

UCHAR 												  // Returns CTF ID
BPSRX_GetCTF_ID();

BPSPage16_Data*                             // Pointer to page 16 data
BPSRX_GetPage16();

BPSPage17_Data*                             // Pointer to page 17 data
BPSRX_GetPage17();                                                       

BPSPage18_Data*                             // Pointer to page 18 data
BPSRX_GetPage18();

BPSPage32_Data*                             // Pointer to page 32 data
BPSRX_GetPage32();

CommonPage80_Data*                          // Pointer to Page 80 data
Common_GetPage80();

CommonPage81_Data*                          // Pointer to Page 81 data
Common_GetPage81();

CommonPage82_Data*                          // Pointer to Page 81 data
Common_GetPage82();

CalculatedStdPower*                         // Pointer to Speed/Distance calculations
BPSRX_GetCalcStdPwr();

CalculatedStdWheelTorque*
BPSRX_GetCalcWT();

CalculatedStdCrankTorque*
BPSRX_GetCalcCT();

CalculatedStdCTF*
BPSRX_GetCalcCTF();

BOOL 
CheckCalibrationTimeout(
	ANTPLUS_EVENT_RETURN* pstEventStruct_);

BOOL CalibrationRequest();
#endif
