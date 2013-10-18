/*
This software is subject to the license described in the License.txt file
included with this software distribution. You may not use this file except in compliance
with this license.

Copyright (c) Dynastream Innovations Inc. 2013
All rights reserved.
*/

#include "types.h"
#include "config.h"
#include "antinterface.h"
#include "antmessage.h"
#include "antdefines.h"
#include "bps_rx.h"
#include "antplus.h"
#include "common_pages.h"
#include "bps_pages.h"
#include "ioboard.h"
#include "timer.h"



///////////////////////////////////////////////////////////////////////////////
// ANT Network Setup definitions
///////////////////////////////////////////////////////////////////////////////
// ANT Channel
#define BPSRX_CHANNEL_TYPE            ((UCHAR) 0x00)      // Slave

// Channel ID
#define BPSRX_DEVICE_TYPE             ((UCHAR) 0x0B)      // Bike Power	

// Message Periods
#define BPSRX_MSG_PERIOD              ((USHORT) 0x1FF6)	 // decimal 8182 (~4.00Hz)
#define BPSRX_TRANSMIT_POWER          ((UCHAR) 2)	       // ANT Transmit Power (-10dBm)

#define BPSRX_MAX_ACK_ATTEMPTS    ((UCHAR)12)         

static UCHAR  aucTxBuf[8];			  // Primary transmit buffer
/////////////////////////////////////////////////////////////////////////////
// STATIC ANT Configuration Block
/////////////////////////////////////////////////////////////////////////////

static BPSPage1_Data stPage1Data;		// calibration
static BPSPage16_Data stPage16Data;		// std power only
static BPSPage17_Data stPage17Data;		// torque at wheel
static BPSPage18_Data stPage18Data;		// torque at crank
static BPSPage32_Data stPage32Data;		// CTF
static CommonPage80_Data stPage80Data;	// manufacturer info
static CommonPage81_Data stPage81Data;	// product info
static CommonPage82_Data stPage82Data;	// optional battery info page

CalculatedStdPower stCalculatedStdPowerData;			       // Standard Power (Page 16) calculations
CalculatedStdWheelTorque stCalculatedStdWheelTorqueData;  // Wheel Torque (Page 17) calculations
CalculatedStdCrankTorque stCalculatedStdCrankTorqueData;  // Crank Torque (Page 17) calculations
CalculatedStdCTF stCalculatedStdCTFData;				       // CTF (Page 32) calculations

//Standard Power Only Variables
static BOOL   bPOFirstPageCheck;
static UCHAR  ucPOEventCountPrev;
static USHORT usPOAccumulatedPowerPrev;
static ULONG  ulPOEventCountDiff;
static ULONG  ulPOAccumulatedPowerDiff;

//WHEEL TORQUE SENSOR VARIABLES
static BOOL   bWTFirstPageCheck;
static UCHAR  ucWTEventCountPrev;
static ULONG  ulWTEventCountDiff;
static USHORT usWTAccumulatedWheelPeriodPrev;
static ULONG  ulWTAccumulatedWheelPeriodDiff;
static USHORT usWTAccumulatedWheelTorquePrev;
static ULONG  ulWTAccumulatedWheelTorqueDiff;
static UCHAR  ucWTWheelTicksPrev;
static ULONG  ulWTWheelTicksDiff;
static UCHAR  ucWTZeroSpeedCheck;

//CRANK TORQUE SENSOR VARIABLES
static BOOL   bCTFirstPageCheck;
static UCHAR  ucCTEventCountPrev;
static ULONG  ulCTEventCountDiff;
static USHORT usCTAccumulatedCrankPeriodPrev;
static ULONG  ulCTAccumulatedCrankPeriodDiff;
static USHORT usCTAccumulatedCrankTorquePrev;
static ULONG  ulCTAccumulatedCrankTorqueDiff;
static UCHAR  ucCTZeroSpeedCheck;

// CRANK TORQUE FREQUENCY SENSOR VARIABLES
static BOOL   bCTFFirstPageCheck;
static UCHAR  ucCTFEventCountPrev;
static ULONG  ulCTFEventCountDiff;
static USHORT usCTFTimeStampPrev;
static ULONG  ulCTFTimeStampDiff;
static USHORT usCTFTorqueTickStampPrev;
static ULONG  ulCTFTorqueTickStampDiff;
static UCHAR  ucCTFZeroSpeedCheck;

// Calibration Response Page variables
static USHORT usCTFOffset;
static BOOL bCalibrationTimeout;
static UCHAR ucAckFailCount;
static CalibrationType eLocalCalibrationType;
static UCHAR ucAlarmNumber;

// Page 1 variables/bit masks
static UCHAR ucLocalCTF_ID;
static UCHAR ucLocalAutoZeroStatus;
static UCHAR ucLocalSpeedFactor;

#define PAGE1_CID18_AutoZero_ENABLE(x)				  (x & 0x01)
#define PAGE1_CID18_AutoZero_STATUS(x)				  ((x >> 1) & 0x01)

// Page 82 bit masks
#define COMMON82_COARSE_BATT_VOLTAGE(x)           (x & 0x0F)
#define COMMON82_BATT_STATUS(x)                   ((x >> 4) & 0x07)
#define COMMON82_CUM_TIME_RESOLUTION(x)           ((x >> 7) & 0x01)

static UCHAR ucAntChannel;
static USHORT usDeviceNumber;
static UCHAR ucTransType;

static BOOL HandleResponseEvents(UCHAR* pucBuffer_);
static BOOL HandleDataMessages(UCHAR* pucEventBuffer_, ANTPLUS_EVENT_RETURN* pstEventStruct_);

/////////////////////////////////////////////////////////////////////////////////////////
// Functon: BPSRX_Init
//
// Descrption:
//
// Initialize all state variables. 
// Params:
// N/A.
//
// returns: N/A. 
/////////////////////////////////////////////////////////////////////////////////////////
void BPSRX_Init()
{
   BPSPage1_Data* pstPage1Data = BPSRX_GetPage1();
   BPSPage16_Data* pstPage16Data = BPSRX_GetPage16();
   BPSPage17_Data* pstPage17Data = BPSRX_GetPage17();
   BPSPage18_Data* pstPage18Data = BPSRX_GetPage18();
   BPSPage32_Data* pstPage32Data = BPSRX_GetPage32();
   CommonPage80_Data* pstPage80Data = Common_GetPage80();
   CommonPage81_Data* pstPage81Data = Common_GetPage81();
   CommonPage82_Data* pstPage82Data = Common_GetPage82();
   
   // Intialize app variables
   pstPage1Data->ucCalibrationID = 0;    
   pstPage1Data->stBPSCalibrationData.stBPSPage1_CID18_Data.ucCID18_SensorConfiguration_EN = 0;     //not supported
   pstPage1Data->stBPSCalibrationData.stBPSPage1_CID18_Data.ucCID18_SensorConfiguration_STATUS = 0; //off
   pstPage1Data->stBPSCalibrationData.stBPSPage1_CID18_Data.ssCID18_RawTorqueCount = 0;
   pstPage1Data->stBPSCalibrationData.stBPSPage1_CID18_Data.ssCID18_OffsetTorque = 0;

   pstPage1Data->stBPSCalibrationData.stBPSPage1_CID172_Data.ucCID172_AutoZeroStatus = 0xFF;
   pstPage1Data->stBPSCalibrationData.stBPSPage1_CID172_Data.ucCID172_ReserveByte3 = BPS_PAGE_RESERVE_BYTE;
   pstPage1Data->stBPSCalibrationData.stBPSPage1_CID172_Data.ucCID172_ReserveByte4 = BPS_PAGE_RESERVE_BYTE;
   pstPage1Data->stBPSCalibrationData.stBPSPage1_CID172_Data.ucCID172_ReserveByte5 = BPS_PAGE_RESERVE_BYTE;
   pstPage1Data->stBPSCalibrationData.stBPSPage1_CID172_Data.usCID172_CalibrationData = 0;
   						
   pstPage1Data->stBPSCalibrationData.stBPSPage1_CID175_Data.ucCID175_AutoZeroStatus = 0xFF;
   pstPage1Data->stBPSCalibrationData.stBPSPage1_CID175_Data.ucCID175_ReserveByte3 = BPS_PAGE_RESERVE_BYTE;
   pstPage1Data->stBPSCalibrationData.stBPSPage1_CID175_Data.ucCID175_ReserveByte4 = BPS_PAGE_RESERVE_BYTE;
   pstPage1Data->stBPSCalibrationData.stBPSPage1_CID175_Data.ucCID175_ReserveByte5 = BPS_PAGE_RESERVE_BYTE;
   pstPage1Data->stBPSCalibrationData.stBPSPage1_CID175_Data.usCID175_CalibrationData = 0;

   pstPage1Data->stBPSCalibrationData.stBPSPage1_CID16_CTFID1_Data.ucCID16_CTFID1_ReserveByte3 = BPS_PAGE_RESERVE_BYTE;
   pstPage1Data->stBPSCalibrationData.stBPSPage1_CID16_CTFID1_Data.ucCID16_CTFID1_ReserveByte4 = BPS_PAGE_RESERVE_BYTE;
   pstPage1Data->stBPSCalibrationData.stBPSPage1_CID16_CTFID1_Data.ucCID16_CTFID1_ReserveByte5 = BPS_PAGE_RESERVE_BYTE;
   pstPage1Data->stBPSCalibrationData.stBPSPage1_CID16_CTFID1_Data.usCID16_CTFID1_OffsetData = 0;
									
   pstPage1Data->stBPSCalibrationData.stBPSPage1_CID16_CTFID172_Data.ucCID16_CTFID172_CTFAckMessage = 0;
   pstPage1Data->stBPSCalibrationData.stBPSPage1_CID16_CTFID172_Data.ucCID16_CTFID172_ReserveByte4 = BPS_PAGE_RESERVE_BYTE;
   pstPage1Data->stBPSCalibrationData.stBPSPage1_CID16_CTFID172_Data.ucCID16_CTFID172_ReserveByte5 = BPS_PAGE_RESERVE_BYTE;
   pstPage1Data->stBPSCalibrationData.stBPSPage1_CID16_CTFID172_Data.ucCID16_CTFID172_ReserveByte6 = BPS_PAGE_RESERVE_BYTE;
   pstPage1Data->stBPSCalibrationData.stBPSPage1_CID16_CTFID172_Data.ucCID16_CTFID172_ReserveByte7 = BPS_PAGE_RESERVE_BYTE;

   // pstPage1Data->stBPSCalibrationData.stBPSPage1_CID18_Data.ucCID18_ReserveByte7 = pucEventBuffer_[BUFFER_INDEX_MESG_DATA+7]

   // Power Only (PO) Main Data Page
   pstPage16Data->ucPOEventCount = 0;                                  
   pstPage16Data->ucPOInstantCadence = 0;
   pstPage16Data->ucReserveByte2 = BPS_PAGE_RESERVE_BYTE;                              
   pstPage16Data->usPOAccumulatedPower = 0; 
   pstPage16Data->usPOInstantPower = 0;

   // Wheel Torque (WT) Main Data Page
   pstPage17Data->ucWTEventCount = 0;                                  
   pstPage17Data->ucWTWheelTicks = 0;
   pstPage17Data->ucWTInstantCadence = 0;                                 
   pstPage17Data->usWTAccumulatedWheelPeriod = 0; 
   pstPage17Data->usWTAccumulatedTorque = 0;

   // Standard Crank Torque (CT) Main Data Page
   pstPage18Data->ucCTEventCount = 0;                                  
   pstPage18Data->ucCTCrankTicks = 0;
   pstPage18Data->ucCTInstantCadence = 0;                                 
   pstPage18Data->usCTAccumulatedCrankPeriod = 0; 
   pstPage18Data->usCTAccumulatedTorque = 0;

   // Crank Torque Frequency (CTF) Main Data Page
   pstPage32Data->ucCTFEventCount = 0;                                  
   pstPage32Data->usCTFSlope = 0;
   pstPage32Data->usCTFTimeStamp = 0; 
   pstPage32Data->usCTFTorqueTickStamp = 0;

   // Global Common Pages
   pstPage80Data->ucPg80ReserveByte1 = COMMON_PAGE_RESERVE_BYTE;
   pstPage80Data->ucPg80ReserveByte1 = COMMON_PAGE_RESERVE_BYTE;
   pstPage80Data->ucHwVersion = 0;
   pstPage80Data->usManId = 0;
   pstPage80Data->usModelNumber = 0;

   pstPage81Data->ucPg81ReserveByte1 = COMMON_PAGE_RESERVE_BYTE;
   pstPage81Data->ucPg81ReserveByte1 = COMMON_PAGE_RESERVE_BYTE;
   pstPage81Data->ucSwVersion = 0;
   pstPage81Data->ulSerialNumber = 0;

   pstPage82Data->ucPg82ReserveByte1 = COMMON_PAGE_RESERVE_BYTE;
   pstPage82Data->ucPg82ReserveByte1 = COMMON_PAGE_RESERVE_BYTE;
   pstPage82Data->ulCumOperatingTime = 0;
   pstPage82Data->ucBattVoltage256 = 0;
   pstPage82Data->ucBattVoltage = 0;
   pstPage82Data->ucBattStatus = 0;
   pstPage82Data->ucCumOperatingTimeRes = 0;
   
   ucLocalAutoZeroStatus = 0xFF; //Not supported
   ucLocalCTF_ID = 0xFF;
   ucLocalSpeedFactor = 1;
   
   // Basic Power Variables
   stCalculatedStdPowerData.ulAvgStandardPower = 0; 
   bPOFirstPageCheck = 1; 
   ucPOEventCountPrev = 0;
   usPOAccumulatedPowerPrev = 0;
   ulPOEventCountDiff = 0;
   ulPOAccumulatedPowerDiff=0;
   
   // Wheel Torque Variables
   stCalculatedStdWheelTorqueData.ulWTAvgSpeed = 0;
   stCalculatedStdWheelTorqueData.ulWTDistance = 0;
   stCalculatedStdWheelTorqueData.ulWTAngularVelocity = 0;
   stCalculatedStdWheelTorqueData.ulWTAveragePower = 0;
   stCalculatedStdWheelTorqueData.ulWTAverageTorque = 0;
   bWTFirstPageCheck = 1;
   ucWTEventCountPrev = 0;
   ulWTEventCountDiff = 0;
   usWTAccumulatedWheelPeriodPrev=0;
   ulWTAccumulatedWheelPeriodDiff=0;
   usWTAccumulatedWheelTorquePrev = 0;
   ulWTAccumulatedWheelTorqueDiff = 0;
   ucWTWheelTicksPrev=0;
   ulWTWheelTicksDiff=0;
   ucWTZeroSpeedCheck=0;
   
   // Crank Torque Variables
   stCalculatedStdCrankTorqueData.ulCTAverageCadence=0;
   stCalculatedStdCrankTorqueData.ulCTAngularVelocity = 0;
   stCalculatedStdCrankTorqueData.ulCTAveragePower = 0;
   stCalculatedStdCrankTorqueData.ulCTAverageTorque = 0;
   bCTFirstPageCheck = 1;
   ucCTEventCountPrev=0;
   ulCTEventCountDiff=0;
   usCTAccumulatedCrankPeriodPrev=0;
   ulCTAccumulatedCrankPeriodDiff=0;
   usCTAccumulatedCrankTorquePrev=0;
   ulCTAccumulatedCrankTorqueDiff=0;
   ucCTZeroSpeedCheck=0;

   // Crank Torque Frequency Variables
   stCalculatedStdCTFData.ulCTFAverageCadence = 0;
   stCalculatedStdCTFData.ulCTFAveragePower = 0;
   stCalculatedStdCTFData.ulCTFAverageTorque = 0;
   stCalculatedStdCTFData.ulCTFTorqueFrequency = 0;
   bCTFFirstPageCheck = 1;
   ucCTFEventCountPrev = 0;
   ulCTFEventCountDiff = 0;
   usCTFTimeStampPrev = 0;
   ulCTFTimeStampDiff = 0;
   usCTFTorqueTickStampPrev =0;
   ulCTFTorqueTickStampDiff =0;
   ucCTFZeroSpeedCheck=0;

   // Calibration data
   usCTFOffset = BPSRX_INITIAL_OFFSET;
   bCalibrationTimeout = 0;
   ucAckFailCount = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Functon: BPSRX_Open
//
// Descrption:
// Opens BPS recieve channel. Once the channel has been properly initialized a BPSRX_EVENT_INIT_COMPLETE
// event will be generated via the callback. A positive response from this function does not
// indicate that the channel successfully opened. 
//
// Params:
// usDeviceNumber_: Device number to pair to. 0 for wild-card.
//
// returns: TRUE if all configuration messages were correctly setup and queued. FALSE otherwise. 
/////////////////////////////////////////////////////////////////////////////////////////
BOOL BPSRX_Open(UCHAR ucAntChannel_, USHORT usSearchDeviceNumber_, UCHAR ucTransType_)
{

   BPSRX_Init();

   ucAntChannel = ucAntChannel_;
   usDeviceNumber = usSearchDeviceNumber_;
   ucTransType = ucTransType_;
      
   if (!ANT_AssignChannel(ucAntChannel,BPSRX_CHANNEL_TYPE,ANTPLUS_NETWORK_NUMBER ))
      return FALSE;

   // Register the timer for Calibration Timeout
   ucAlarmNumber = Timer_RegisterAlarm( BPSRX_PulseEvent, ALARM_FLAG_ONESHOT);

   if( ucAlarmNumber == 0)
      return(FALSE);
      
   return(TRUE);
} 

/////////////////////////////////////////////////////////////////////////////////////////
// Functon: BPSRX_Close
//
// Description:
// Closes BPS recieve channel and initializes all state variables. Once the channel has 
// been successfuly closed, an BPSRX_EVENT_CHANNEL_CLOSED event will be generated via the
// callback function/
//
// Params:
// N/A
//
// Returns: TRUE if message was successfully sent to ANT, FALSE otherwise. 
// 
/////////////////////////////////////////////////////////////////////////////////////////
BOOL BPSRX_Close()
{
   return(ANT_CloseChannel(ucAntChannel));
}

///////////////////////////////////////////////////////////////////////////
// BPSRX_PulseEvent
//
// On the Alarm pulse -> Sets Calibration timeout variable 
//
//
// \return: N/A. 
////////////////////////////////////////////////////////////////////////////
void BPSRX_PulseEvent(USHORT usTime1024, UCHAR ucAlarmNumber_)
{ 
   bCalibrationTimeout = 1;
   return;  
}

/////////////////////////////////////////////////////////////////////////////////////////
// Functon: BPSRX_GetPage1
//
// Descrption:
// Returns a pointer to the page 1 (calibration) buffer. This function should be called following 
// an BPSRX_EVENT_PAGE 1 event to extract the latest data from page 1. DTF ID is also returned 
//
// Params:
// N/A
//
// returns: Pointer to Page 1 buffer, NULL if data not valid.  
/////////////////////////////////////////////////////////////////////////////////////////
BPSPage1_Data* BPSRX_GetPage1()
{
   return &stPage1Data;
}

UCHAR BPSRX_GetCTF_ID()
{
   return ucLocalCTF_ID;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Functon: BPSRX_GetPage16
//
// Descrption:
// Returns a pointer to the page 16 buffer. This function should be called following 
// an BPSRX_EVENT_PAGE 16 event to extract the latest data from page 16. 
//
// Params:
// N/A
//
// returns: Pointer to Page 16 buffer, NULL if data not valid.  
/////////////////////////////////////////////////////////////////////////////////////////
BPSPage16_Data* BPSRX_GetPage16()
{
   return &stPage16Data;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Functon: BPSRX_GetPage17
//
// Descrption:
// Returns a pointer to the page 17 buffer. This function should be called following 
// an BPSRX_EVENT_PAGE 17 event to extract the latest data from page 17. 
//
// Params:
// N/A
//
// returns: Pointer to Page 17 buffer, NULL if data not valid.  
/////////////////////////////////////////////////////////////////////////////////////////
BPSPage17_Data* BPSRX_GetPage17()
{
   return &stPage17Data;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Functon: BPSRX_GetPage18
//
// Descrption:
// Returns a pointer to the page 18 buffer. This function should be called following 
// an BPSRX_EVENT_PAGE 18 event to extract the latest data from page 18. 
//
// Params:
// N/A
//
// returns: Pointer to Page 18 buffer, NULL if data not valid.  
/////////////////////////////////////////////////////////////////////////////////////////
BPSPage18_Data* BPSRX_GetPage18()
{
   return &stPage18Data;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Functon: BPSRX_GetPage32
//
// Descrption:
// Returns a pointer to the page 32 buffer. This function should be called following 
// an BPSRX_EVENT_PAGE 32 event to extract the latest data from page 32. 
//
// Params:
// N/A
//
// returns: Pointer to Page 32 buffer, NULL if data not valid.  
/////////////////////////////////////////////////////////////////////////////////////////
BPSPage32_Data* BPSRX_GetPage32()
{
   return &stPage32Data;
}
////////////////////////////////////////////////////////////////////////////////////////
// Functon: CommonPage80_Data
//
// Descrption:
// Returns a pointer to the page 80 buffer. This function should be called following 
// an BPSRX_EVENT_PAGE 80 event to extract the latest data from page 80. 
//
// Params:
// N/A
//
// returns: Pointer to Page 80 buffer, NULL if data not valid.  
/////////////////////////////////////////////////////////////////////////////////////////
CommonPage80_Data* Common_GetPage80()
{
   return &stPage80Data;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Functon: CommonPage81_Data
//
// Descrption:
// Returns a pointer to the page 81 buffer. This function should be called following 
// an BPSRX_EVENT_PAGE 81 event to extract the latest data from page 81. 
//
// Params:
// N/A
//
// returns: Pointer to Page 81 buffer, NULL if data not valid.  
/////////////////////////////////////////////////////////////////////////////////////////
CommonPage81_Data* Common_GetPage81()
{
   return &stPage81Data;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Functon: CommonPage82_Data
//
// Descrption:
// Returns a pointer to the page 81 buffer. This function should be called following 
// an BPSRX_EVENT_PAGE 82 event to extract the latest data from page 82. 
//
// Params:
// N/A
//
// returns: Pointer to Page 81 buffer, NULL if data not valid.  
/////////////////////////////////////////////////////////////////////////////////////////
CommonPage82_Data* Common_GetPage82()
{
   return &stPage82Data;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Functon: BPSRX_GetCalcStdPwr
//
// Descrption:
// Returns a pointer to the calculated standard power data. This function should be called following 
// an BPSRX_EVENT_PAGE 16 event to extract the latest calculated power. 
//
// Params:
// N/A
//
// returns: Pointer to Calculated Power, NULL if data not valid.  
/////////////////////////////////////////////////////////////////////////////////////////
CalculatedStdPower* BPSRX_GetCalcStdPwr()
{
	return (&stCalculatedStdPowerData);
}

/////////////////////////////////////////////////////////////////////////////////////////
// Functon: BPSRX_GetCalcWT
//
// Descrption:
// Returns a pointer to the calculated standard power data. This function should be called following 
// an BPSRX_EVENT_PAGE 16 event to extract the latest calculated power. 
//
// Params:
// N/A
//
// returns: Pointer to Calculated Power, NULL if data not valid.  
/////////////////////////////////////////////////////////////////////////////////////////
CalculatedStdWheelTorque* BPSRX_GetCalcWT()
{
	return (&stCalculatedStdWheelTorqueData);
}

/////////////////////////////////////////////////////////////////////////////////////////
// Functon: BPSRX_GetCalcCT
//
// Descrption:
// Returns a pointer to the calculated standard power data. This function should be called following 
// an BPSRX_EVENT_PAGE 16 event to extract the latest calculated power. 
//
// Params:
// N/A
//
// returns: Pointer to Calculated Power, NULL if data not valid.  
/////////////////////////////////////////////////////////////////////////////////////////
CalculatedStdCrankTorque* BPSRX_GetCalcCT()
{
	return (&stCalculatedStdCrankTorqueData);
}

/////////////////////////////////////////////////////////////////////////////////////////
// Functon: BPSRX_GetCalcCTF
//
// Descrption:
// Returns a pointer to the calculated standard power data. This function should be called following 
// an BPSRX_EVENT_PAGE 16 event to extract the latest calculated power. 
//
// Params:
// N/A
//
// returns: Pointer to Calculated Power, NULL if data not valid.  
/////////////////////////////////////////////////////////////////////////////////////////
CalculatedStdCTF* BPSRX_GetCalcCTF()
{
	return (&stCalculatedStdCTFData);
}

//----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////
// BPS_ChannelEvent
//
// Process channel event messages for the BPS 
//
// pucEventBuffer_: Pointer to ANT message buffer.
//
// \return: TRUE if buffer handled. 
////////////////////////////////////////////////////////////////////////////
BOOL BPSRX_ChannelEvent(UCHAR* pucEventBuffer_, ANTPLUS_EVENT_RETURN* pstEventStruct_)
{
   BOOL bTransmit = TRUE;
   
   UCHAR ucChannel = pucEventBuffer_[BUFFER_INDEX_CHANNEL_NUM] & 0x1F;
   pstEventStruct_->eEvent = ANTPLUS_EVENT_NONE;

   if((ucChannel == ucAntChannel) && pucEventBuffer_)
   {
      UCHAR ucANTEvent = pucEventBuffer_[BUFFER_INDEX_MESG_ID];   
      switch( ucANTEvent )
      {
         case MESG_RESPONSE_EVENT_ID:
         {
            switch(pucEventBuffer_[BUFFER_INDEX_RESPONSE_CODE])
            {
               case RESPONSE_NO_ERROR:
               {   
                  bTransmit = HandleResponseEvents(pucEventBuffer_);
                  break;
               }
               case EVENT_TRANSFER_TX_FAILED:
               {
                  ucAckFailCount++;
                  if (ucAckFailCount<=BPSRX_MAX_ACK_ATTEMPTS)
                  {
                     CalibrationRequest(eLocalCalibrationType);    // calibration failed to transmit, retry
                  }
                  else  // maximum number of retries exceeded
                  {
                     // reset count, set calibration timeout, stop timer
                     ucAckFailCount = 0;           
                     bCalibrationTimeout = 1;      
                     Timer_Stop( ucAlarmNumber);   
                  }
                  break;
               }
               case EVENT_TRANSFER_TX_COMPLETED:
               {
                  ucAckFailCount = 0; // reset
               }
               case EVENT_RX_FAIL:
               default:
               {
                  // Other processing should be handled at the application level
                  break;
               }
            } 
            break;
         }
         case MESG_BROADCAST_DATA_ID:      // Handle all BROADCAST, ACKNOWLEDGED and BURST data the same
         case MESG_ACKNOWLEDGED_DATA_ID:
         case MESG_BURST_DATA_ID:    
         {
            bTransmit = HandleDataMessages(pucEventBuffer_, pstEventStruct_);
            break;
         } 
         case MESG_CHANNEL_ID_ID:
         {
            usDeviceNumber = (USHORT)pucEventBuffer_[BUFFER_INDEX_MESG_DATA]; 
            usDeviceNumber |= (USHORT)pucEventBuffer_[BUFFER_INDEX_MESG_DATA+1] << 8;
            ucTransType    = (UCHAR)pucEventBuffer_[BUFFER_INDEX_MESG_DATA+3];

            pstEventStruct_->eEvent = ANTPLUS_EVENT_CHANNEL_ID;
            pstEventStruct_->usParam1 = usDeviceNumber;
            pstEventStruct_->usParam2 = ucTransType;
            break;
         }
      }
   } 
   return (bTransmit);
}

//----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////
// Calibration Timeout
//
// Checks for a calibration timeout -> sets ANT subevent and returns true if calibration has timed out
// either through repeated failure to send ack message, or failure to receive a calibration 
// response within the timeout period (set by alarm)
//
// \return: N/A. 
////////////////////////////////////////////////////////////////////////////
BOOL CheckCalibrationTimeout(ANTPLUS_EVENT_RETURN* pstEventStruct_)
{
	if(!bCalibrationTimeout) // No time out
		return (FALSE);
	pstEventStruct_->eEvent = ANTPLUS_EVENT_CALIBRATION_TIMEOUT;
	bCalibrationTimeout = 0;
	return (TRUE);
}

////////////////////////////////////////////////////////////////////////////
// Calibration Request
//
// Process Calibration request messages for BPSRX 
//
// eButtonNumber - button pressed
//
// \return: N/A. 
////////////////////////////////////////////////////////////////////////////
BOOL CalibrationRequest(CalibrationType eCalibrationType)
{
   BOOL bNoTransmit=FALSE;
   eLocalCalibrationType = eCalibrationType;
   switch(eLocalCalibrationType)
   {
      case CALIBRATION_TYPE_MANUAL:
      {
         // Manual Calibration request
         // format page
         aucTxBuf[0] = BPS_PAGE_1;
         aucTxBuf[1] = PBS_CID_170;
         aucTxBuf[2] = BPS_PAGE_RESERVE_BYTE;
         aucTxBuf[3] = BPS_PAGE_RESERVE_BYTE;                     
         aucTxBuf[4] = BPS_PAGE_RESERVE_BYTE;
         aucTxBuf[5] = BPS_PAGE_RESERVE_BYTE;
         aucTxBuf[6] = BPS_PAGE_RESERVE_BYTE; 
         aucTxBuf[7] = BPS_PAGE_RESERVE_BYTE;
         break;
      }                                
      case CALIBRATION_TYPE_AUTO:
      {
         // AutoZero Calibration request
         // format page
         if (ucLocalAutoZeroStatus==0xFF)
         {
            bNoTransmit=TRUE;
         }
         else
         {
            aucTxBuf[0] = BPS_PAGE_1;
            aucTxBuf[1] = PBS_CID_171;
            aucTxBuf[2] = ucLocalAutoZeroStatus;
            aucTxBuf[3] = BPS_PAGE_RESERVE_BYTE;                     
            aucTxBuf[4] = BPS_PAGE_RESERVE_BYTE;
            aucTxBuf[5] = BPS_PAGE_RESERVE_BYTE;
            aucTxBuf[6] = BPS_PAGE_RESERVE_BYTE; 
            aucTxBuf[7] = BPS_PAGE_RESERVE_BYTE;
         }
         break;
      }                                                                  
      case CALIBRATION_TYPE_SLOPE:
      {
         // SLOPE TO PWR SENSOR FLASH
         // format page
         aucTxBuf[0] = BPS_PAGE_1;
         aucTxBuf[1] = PBS_CID_16;
         aucTxBuf[2] = PBS_CID_16_CTFID_2;
         aucTxBuf[3] = BPS_PAGE_RESERVE_BYTE;                     
         aucTxBuf[4] = BPS_PAGE_RESERVE_BYTE;
         aucTxBuf[5] = BPS_PAGE_RESERVE_BYTE;
         aucTxBuf[6] = (UCHAR)(BPSRX_DEFINED_SLOPE>>8); 
         aucTxBuf[7] = (UCHAR)(BPSRX_DEFINED_SLOPE & MAX_UCHAR);
         break;
      }                                                             
      case CALIBRATION_TYPE_SERIAL:
      {
         // SERIAL# TO PWR SENSOR FLASH
         // format page
         aucTxBuf[0] = BPS_PAGE_1;
         aucTxBuf[1] = PBS_CID_16;
         aucTxBuf[2] = PBS_CID_16_CTFID_3;
         aucTxBuf[3] = BPS_PAGE_RESERVE_BYTE;                     
         aucTxBuf[4] = BPS_PAGE_RESERVE_BYTE;
         aucTxBuf[5] = BPS_PAGE_RESERVE_BYTE;
         aucTxBuf[6] = (UCHAR)(BPSRX_DEFINED_SERIAL>>8); 
         aucTxBuf[7] = (UCHAR)(BPSRX_DEFINED_SERIAL & MAX_UCHAR);
         break;
      }
   }
   
   if(!bNoTransmit) // Send Calibration Request, Start Timer
   {
      if(ANT_Acknowledged(ucAntChannel, aucTxBuf));
          Timer_Start( ucAlarmNumber, 30*ALARM_TIMER_PERIOD); // 30 second time out                 
   }

   return bNoTransmit;
}


////////////////////////////////////////////////////////////////////////////
// HandleResponseEvents
//
// \return: N/A.                                                      -
////////////////////////////////////////////////////////////////////////////
BOOL HandleResponseEvents(UCHAR* pucBuffer_)
{

   BOOL bTransmit = TRUE;
   
   if(pucBuffer_ && pucBuffer_[BUFFER_INDEX_RESPONSE_CODE] == RESPONSE_NO_ERROR)
   {
      switch(pucBuffer_[BUFFER_INDEX_RESPONSE_MESG_ID])
      {
         case MESG_ASSIGN_CHANNEL_ID:
         {
            ANT_ChannelId(ucAntChannel, usDeviceNumber,BPSRX_DEVICE_TYPE, ucTransType );
            break;
         }
         case MESG_CHANNEL_ID_ID:
         {
            ANT_ChannelRFFreq(ucAntChannel, ANTPLUS_RF_FREQ);
            break;
         }
         case MESG_CHANNEL_RADIO_FREQ_ID:
         {
            ANT_ChannelPeriod(ucAntChannel, BPSRX_MSG_PERIOD);
            break;
         }
         case MESG_CHANNEL_MESG_PERIOD_ID:
         {
            ANT_OpenChannel(ucAntChannel);
            break;
         }
         case MESG_OPEN_CHANNEL_ID:
         case MESG_CLOSE_CHANNEL_ID:            // Fallthrough
         default:
         {
            bTransmit = FALSE;                  // Can go back to sleep   
         }
      }
   }
   return(bTransmit);
}

////////////////////////////////////////////////////////////////////////////
// HandleDataMessages
//
// \return: N/A. 
////////////////////////////////////////////////////////////////////////////
BOOL HandleDataMessages(UCHAR* pucEventBuffer_, ANTPLUS_EVENT_RETURN* pstEventStruct_)
{
   BOOL bTransmit = FALSE;
   
   UCHAR ucPage = pucEventBuffer_[BUFFER_INDEX_MESG_DATA];
   pstEventStruct_->eEvent = ANTPLUS_EVENT_PAGE;
   pstEventStruct_->usParam1 = ucPage;
   pstEventStruct_->usParam2 = BPSRX_SUBEVENT_UPDATE; // initialise to update

   switch(ucPage)
   {
      case BPS_PAGE_1:
      {
         BPSPage1_Data* pstPage1Data = BPSRX_GetPage1();
         pstPage1Data->ucCalibrationID = pucEventBuffer_[BUFFER_INDEX_MESG_DATA+1];   

         switch(pstPage1Data->ucCalibrationID)
         {
            case PBS_CID_18: // CID 0x12 Torque Sensor Capabilities Main Data Page
            {
               pstPage1Data->stBPSCalibrationData.stBPSPage1_CID18_Data.ucCID18_SensorConfiguration_EN = PAGE1_CID18_AutoZero_ENABLE(pucEventBuffer_[BUFFER_INDEX_MESG_DATA+2]);
               pstPage1Data->stBPSCalibrationData.stBPSPage1_CID18_Data.ucCID18_SensorConfiguration_STATUS = PAGE1_CID18_AutoZero_STATUS(pucEventBuffer_[BUFFER_INDEX_MESG_DATA+2]);
               pstPage1Data->stBPSCalibrationData.stBPSPage1_CID18_Data.ssCID18_RawTorqueCount = (SSHORT)pucEventBuffer_[BUFFER_INDEX_MESG_DATA+3];
               pstPage1Data->stBPSCalibrationData.stBPSPage1_CID18_Data.ssCID18_RawTorqueCount |= (SSHORT)(pucEventBuffer_[BUFFER_INDEX_MESG_DATA+4]<<8);
               pstPage1Data->stBPSCalibrationData.stBPSPage1_CID18_Data.ssCID18_OffsetTorque = (SSHORT)pucEventBuffer_[BUFFER_INDEX_MESG_DATA+5];
               pstPage1Data->stBPSCalibrationData.stBPSPage1_CID18_Data.ssCID18_OffsetTorque |= (SSHORT)(pucEventBuffer_[BUFFER_INDEX_MESG_DATA+6]<<8);
	           // pstPage1Data->stBPSCalibrationData.stBPSPage1_CID18_Data.ucCID18_ReserveByte7 = pucEventBuffer_[BUFFER_INDEX_MESG_DATA+7]
	
	            if (pstPage1Data->stBPSCalibrationData.stBPSPage1_CID18_Data.ucCID18_SensorConfiguration_EN==1)
	            {  
		            if (pstPage1Data->stBPSCalibrationData.stBPSPage1_CID18_Data.ucCID18_SensorConfiguration_STATUS==1)
		            {
			            ucLocalAutoZeroStatus = 0x01;  
		            } // auto zero on
		            else
		            {
			            ucLocalAutoZeroStatus = 0x00;  
		            } // auto zero off
	            }
	            else
	            { 
		            ucLocalAutoZeroStatus = 0xFF;  
	            } // auto zero not supported   
               break;
            }
            case PBS_CID_172:  // Calibration Successful 0xAC
            {
                pstPage1Data->stBPSCalibrationData.stBPSPage1_CID172_Data.ucCID172_AutoZeroStatus = pucEventBuffer_[BUFFER_INDEX_MESG_DATA+2];
	            pstPage1Data->stBPSCalibrationData.stBPSPage1_CID172_Data.ucCID172_ReserveByte3 = BPS_PAGE_RESERVE_BYTE;
	            pstPage1Data->stBPSCalibrationData.stBPSPage1_CID172_Data.ucCID172_ReserveByte4 = BPS_PAGE_RESERVE_BYTE;
	            pstPage1Data->stBPSCalibrationData.stBPSPage1_CID172_Data.ucCID172_ReserveByte5 = BPS_PAGE_RESERVE_BYTE;
	            pstPage1Data->stBPSCalibrationData.stBPSPage1_CID172_Data.usCID172_CalibrationData = (USHORT)pucEventBuffer_[BUFFER_INDEX_MESG_DATA+6];
	            pstPage1Data->stBPSCalibrationData.stBPSPage1_CID172_Data.usCID172_CalibrationData |= (USHORT)(pucEventBuffer_[BUFFER_INDEX_MESG_DATA+7]<<8);
   
	            ucLocalAutoZeroStatus = pstPage1Data->stBPSCalibrationData.stBPSPage1_CID172_Data.ucCID172_AutoZeroStatus;
               break;
            }
            case PBS_CID_175:  // Failed response 0xAF
            {
                pstPage1Data->stBPSCalibrationData.stBPSPage1_CID175_Data.ucCID175_AutoZeroStatus = pucEventBuffer_[BUFFER_INDEX_MESG_DATA+2];
	            pstPage1Data->stBPSCalibrationData.stBPSPage1_CID175_Data.ucCID175_ReserveByte3 = BPS_PAGE_RESERVE_BYTE;
	            pstPage1Data->stBPSCalibrationData.stBPSPage1_CID175_Data.ucCID175_ReserveByte4 = BPS_PAGE_RESERVE_BYTE;
	            pstPage1Data->stBPSCalibrationData.stBPSPage1_CID175_Data.ucCID175_ReserveByte5 = BPS_PAGE_RESERVE_BYTE;
	            pstPage1Data->stBPSCalibrationData.stBPSPage1_CID175_Data.usCID175_CalibrationData = (USHORT)pucEventBuffer_[BUFFER_INDEX_MESG_DATA+6];
	            pstPage1Data->stBPSCalibrationData.stBPSPage1_CID175_Data.usCID175_CalibrationData |= (USHORT)(pucEventBuffer_[BUFFER_INDEX_MESG_DATA+7]<<8);
   
	            ucLocalAutoZeroStatus = pstPage1Data->stBPSCalibrationData.stBPSPage1_CID175_Data.ucCID175_AutoZeroStatus;
               break;
            }
            case PBS_CID_16:  // CTF defined msgs 0x10
            {
               ucLocalCTF_ID = pucEventBuffer_[BUFFER_INDEX_MESG_DATA+2];
               
               switch(ucLocalCTF_ID)
               {
                  case PBS_CID_16_CTFID_1:  // Offset Msg
                  {
                     //NB BIG ENDIAN
                     pstPage1Data->stBPSCalibrationData.stBPSPage1_CID16_CTFID1_Data.ucCID16_CTFID1_ReserveByte3 = pucEventBuffer_[BUFFER_INDEX_MESG_DATA+3];
                     pstPage1Data->stBPSCalibrationData.stBPSPage1_CID16_CTFID1_Data.ucCID16_CTFID1_ReserveByte4 = pucEventBuffer_[BUFFER_INDEX_MESG_DATA+4];
                     pstPage1Data->stBPSCalibrationData.stBPSPage1_CID16_CTFID1_Data.ucCID16_CTFID1_ReserveByte5 = pucEventBuffer_[BUFFER_INDEX_MESG_DATA+5];
			            pstPage1Data->stBPSCalibrationData.stBPSPage1_CID16_CTFID1_Data.usCID16_CTFID1_OffsetData |= (USHORT)(pucEventBuffer_[BUFFER_INDEX_MESG_DATA+6]<<8);
			            pstPage1Data->stBPSCalibrationData.stBPSPage1_CID16_CTFID1_Data.usCID16_CTFID1_OffsetData = (USHORT)pucEventBuffer_[BUFFER_INDEX_MESG_DATA+7];
			
			         usCTFOffset = pstPage1Data->stBPSCalibrationData.stBPSPage1_CID16_CTFID1_Data.usCID16_CTFID1_OffsetData;
			         break;
                  }
                  case PBS_CID_16_CTFID_172:  // Slope/Serial acknowledgement
                  {
                     pstPage1Data->stBPSCalibrationData.stBPSPage1_CID16_CTFID172_Data.ucCID16_CTFID172_CTFAckMessage = pucEventBuffer_[BUFFER_INDEX_MESG_DATA+3];
                     pstPage1Data->stBPSCalibrationData.stBPSPage1_CID16_CTFID172_Data.ucCID16_CTFID172_ReserveByte4 = pucEventBuffer_[BUFFER_INDEX_MESG_DATA+4];
                     pstPage1Data->stBPSCalibrationData.stBPSPage1_CID16_CTFID172_Data.ucCID16_CTFID172_ReserveByte5 = pucEventBuffer_[BUFFER_INDEX_MESG_DATA+5];
			            pstPage1Data->stBPSCalibrationData.stBPSPage1_CID16_CTFID172_Data.ucCID16_CTFID172_ReserveByte6 = pucEventBuffer_[BUFFER_INDEX_MESG_DATA+6];
			            pstPage1Data->stBPSCalibrationData.stBPSPage1_CID16_CTFID172_Data.ucCID16_CTFID172_ReserveByte7 = pucEventBuffer_[BUFFER_INDEX_MESG_DATA+7];

	                 break;
                  }
                  default:
                  {
                     //
                  }
               }
               break;
            }	// CTF defined pages break
         }
         break; // Calibration Page1 Break
      }
      case BPS_PAGE_16: // Standard Power Only
      {
         BPSPage16_Data* pstPage16Data = BPSRX_GetPage16();

         pstPage16Data->ucPOEventCount       = pucEventBuffer_[BUFFER_INDEX_MESG_DATA+1];
         pstPage16Data->ucPOInstantCadence   = pucEventBuffer_[BUFFER_INDEX_MESG_DATA+3];                                
         pstPage16Data->usPOAccumulatedPower = (USHORT)pucEventBuffer_[BUFFER_INDEX_MESG_DATA+4];
         pstPage16Data->usPOAccumulatedPower |= (USHORT)(pucEventBuffer_[BUFFER_INDEX_MESG_DATA+5]<<8);
         pstPage16Data->usPOInstantPower     = (USHORT)pucEventBuffer_[BUFFER_INDEX_MESG_DATA+6];
         pstPage16Data->usPOInstantPower     |= (USHORT)(pucEventBuffer_[BUFFER_INDEX_MESG_DATA+7]<<8);
   
         if (bPOFirstPageCheck==1)
         {
            bPOFirstPageCheck=0;
            pstEventStruct_->usParam2 = BPSRX_SUBEVENT_FIRSTPAGE;
            ucPOEventCountPrev = pstPage16Data->ucPOEventCount;
            usPOAccumulatedPowerPrev = pstPage16Data->usPOAccumulatedPower;
         }
         else
         {
            //Calculate Average Power
            ulPOEventCountDiff = (ULONG)((pstPage16Data->ucPOEventCount - ucPOEventCountPrev) & MAX_UCHAR);
            ulPOAccumulatedPowerDiff = (ULONG)((pstPage16Data->usPOAccumulatedPower - usPOAccumulatedPowerPrev) & MAX_USHORT);
            if (ulPOEventCountDiff==0)
            {
               pstEventStruct_->usParam2 = BPSRX_SUBEVENT_NOUPDATE;
            }
            else
            {	
               pstEventStruct_->usParam2 = BPSRX_SUBEVENT_UPDATE;
               stCalculatedStdPowerData.ulAvgStandardPower = ulPOAccumulatedPowerDiff/ulPOEventCountDiff;
               ucPOEventCountPrev = pstPage16Data->ucPOEventCount;
               usPOAccumulatedPowerPrev = pstPage16Data->usPOAccumulatedPower;
            }
         }
         break;                        
      }
      case BPS_PAGE_17:
      {
         BPSPage17_Data* pstPage17Data = BPSRX_GetPage17();
         
         // Wheel Torque (WT) Main Data Page
         pstPage17Data->ucWTEventCount = pucEventBuffer_[BUFFER_INDEX_MESG_DATA+1];
         pstPage17Data->ucWTWheelTicks = pucEventBuffer_[BUFFER_INDEX_MESG_DATA+2];
         pstPage17Data->ucWTInstantCadence = pucEventBuffer_[BUFFER_INDEX_MESG_DATA+3];
         pstPage17Data->usWTAccumulatedWheelPeriod = (USHORT)pucEventBuffer_[BUFFER_INDEX_MESG_DATA+4];
         pstPage17Data->usWTAccumulatedWheelPeriod |= (USHORT)(pucEventBuffer_[BUFFER_INDEX_MESG_DATA+5]<<8);
         pstPage17Data->usWTAccumulatedTorque = (USHORT)pucEventBuffer_[BUFFER_INDEX_MESG_DATA+6];
         pstPage17Data->usWTAccumulatedTorque |= (USHORT)(pucEventBuffer_[BUFFER_INDEX_MESG_DATA+7]<<8);
         
         if (bWTFirstPageCheck==1)
         {
            bWTFirstPageCheck=0;
            pstEventStruct_->usParam2 = BPSRX_SUBEVENT_FIRSTPAGE;            

            ucWTEventCountPrev = pstPage17Data->ucWTEventCount;
            usWTAccumulatedWheelPeriodPrev =  (pstPage17Data->usWTAccumulatedWheelPeriod*ucLocalSpeedFactor);
            ucWTWheelTicksPrev = (pstPage17Data->ucWTWheelTicks*ucLocalSpeedFactor);
            usWTAccumulatedWheelTorquePrev = (pstPage17Data->usWTAccumulatedTorque*ucLocalSpeedFactor);
         }
         else
         {	

            ulWTEventCountDiff = (ULONG)((pstPage17Data->ucWTEventCount - ucWTEventCountPrev) & MAX_UCHAR);
            ulWTAccumulatedWheelPeriodDiff = (ULONG)((pstPage17Data->usWTAccumulatedWheelPeriod - usWTAccumulatedWheelPeriodPrev) & MAX_USHORT);
            ulWTWheelTicksDiff = (ULONG)((pstPage17Data->ucWTWheelTicks - ucWTWheelTicksPrev) & MAX_UCHAR);
            ulWTAccumulatedWheelTorqueDiff = (ULONG)((pstPage17Data->usWTAccumulatedTorque - usWTAccumulatedWheelTorquePrev) & MAX_USHORT);
         
            if (ulWTEventCountDiff==0)
            {
               pstEventStruct_->usParam2 = BPSRX_SUBEVENT_NOUPDATE;
               ucWTZeroSpeedCheck++;
         
               if(ucWTZeroSpeedCheck >= BPSRX_MAX_REPEATS)  // Event sync zero speed condition
               {
                  pstEventStruct_->usParam2 = BPSRX_SUBEVENT_ZERO_SPEED;
                  //ucWTZeroSpeedCheck = 0;
                  stCalculatedStdWheelTorqueData.ulWTAvgSpeed = ((ULONG)0);
                  stCalculatedStdWheelTorqueData.ulWTDistance = ((ULONG)0);
                  stCalculatedStdWheelTorqueData.ulWTAngularVelocity = ((ULONG)0);
                  stCalculatedStdWheelTorqueData.ulWTAverageTorque = ((ULONG)0);
                  stCalculatedStdWheelTorqueData.ulWTAveragePower = ((ULONG)0);
               }
            }
            else
            {
               ucWTZeroSpeedCheck = 0;
               ucLocalSpeedFactor = 1; // assume moving on update.
            
               if(ulWTAccumulatedWheelPeriodDiff == 0)  // Time sync zero speed condition
               {
                  stCalculatedStdWheelTorqueData.ulWTAvgSpeed = ((ULONG)0);
                  stCalculatedStdWheelTorqueData.ulWTDistance = ((ULONG)0);
                  stCalculatedStdWheelTorqueData.ulWTAngularVelocity = ((ULONG)0);
                  stCalculatedStdWheelTorqueData.ulWTAverageTorque = ((ULONG)0);
                  stCalculatedStdWheelTorqueData.ulWTAveragePower = ((ULONG)0);
               
                  pstEventStruct_->usParam2 = BPSRX_SUBEVENT_ZERO_SPEED;
 	              ucLocalSpeedFactor = 0; 
               }
               else
               {
                  stCalculatedStdWheelTorqueData.ulWTAvgSpeed = (ULONG)(((ulWTEventCountDiff<<11)*BPSRX_WHEEL_CIRCUMFERENCE*36)/((ulWTAccumulatedWheelPeriodDiff)*10)*ucLocalSpeedFactor);
                  stCalculatedStdWheelTorqueData.ulWTDistance = (ULONG)((BPSRX_WHEEL_CIRCUMFERENCE*ulWTWheelTicksDiff)*ucLocalSpeedFactor);
                  stCalculatedStdWheelTorqueData.ulWTAngularVelocity = (ULONG)((BPS_PI_CONSTANT*(ulWTEventCountDiff<<4)/ulWTAccumulatedWheelPeriodDiff)*ucLocalSpeedFactor);
                  stCalculatedStdWheelTorqueData.ulWTAverageTorque = (ULONG)((ulWTAccumulatedWheelTorqueDiff/(32*ulWTEventCountDiff))*ucLocalSpeedFactor);
                  stCalculatedStdWheelTorqueData.ulWTAveragePower = (ULONG)((BPS_PI_CONSTANT*(ulWTAccumulatedWheelTorqueDiff>>1)/ulWTAccumulatedWheelPeriodDiff)*ucLocalSpeedFactor);

                  pstEventStruct_->usParam2 = BPSRX_SUBEVENT_UPDATE;
               }
         
               if (stCalculatedStdWheelTorqueData.ulWTAverageTorque==0 && ucLocalSpeedFactor==1)
               {  
                  pstEventStruct_->usParam2 = BPSRX_SUBEVENT_COASTING;
               }
            
               ucWTEventCountPrev = pstPage17Data->ucWTEventCount;
               usWTAccumulatedWheelPeriodPrev =  (pstPage17Data->usWTAccumulatedWheelPeriod);
               ucWTWheelTicksPrev = (pstPage17Data->ucWTWheelTicks);
               usWTAccumulatedWheelTorquePrev = (pstPage17Data->usWTAccumulatedTorque);
            }
         }			 
         break;                        
      }
      case BPS_PAGE_18:
      {
         BPSPage18_Data* pstPage18Data = BPSRX_GetPage18();
         // Standard Crank Torque (CT) Main Data Page
         pstPage18Data->ucCTEventCount = pucEventBuffer_[BUFFER_INDEX_MESG_DATA+1];                                  
         pstPage18Data->ucCTCrankTicks = pucEventBuffer_[BUFFER_INDEX_MESG_DATA+2];
         pstPage18Data->ucCTInstantCadence = pucEventBuffer_[BUFFER_INDEX_MESG_DATA+3];                                 
         pstPage18Data->usCTAccumulatedCrankPeriod = (USHORT)pucEventBuffer_[BUFFER_INDEX_MESG_DATA+4]; 
         pstPage18Data->usCTAccumulatedCrankPeriod |= (USHORT)(pucEventBuffer_[BUFFER_INDEX_MESG_DATA+5]<<8);
         pstPage18Data->usCTAccumulatedTorque = (USHORT)pucEventBuffer_[BUFFER_INDEX_MESG_DATA+6];
         pstPage18Data->usCTAccumulatedTorque |= (USHORT)(pucEventBuffer_[BUFFER_INDEX_MESG_DATA+7]<<8);

         if (bCTFirstPageCheck==1)
         {
            bCTFirstPageCheck=0;
            pstEventStruct_->usParam2 = BPSRX_SUBEVENT_FIRSTPAGE;              
            
            ucCTEventCountPrev = pstPage18Data->ucCTEventCount;
            usCTAccumulatedCrankPeriodPrev = (pstPage18Data->usCTAccumulatedCrankPeriod*ucLocalSpeedFactor);
            usCTAccumulatedCrankTorquePrev = (pstPage18Data->usCTAccumulatedTorque*ucLocalSpeedFactor);
         }
         else
         {
            ulCTEventCountDiff = (ULONG)((pstPage18Data->ucCTEventCount - ucCTEventCountPrev) & MAX_UCHAR);
            ulCTAccumulatedCrankPeriodDiff= (ULONG)((pstPage18Data->usCTAccumulatedCrankPeriod - usCTAccumulatedCrankPeriodPrev) & MAX_USHORT);
            ulCTAccumulatedCrankTorqueDiff= (ULONG)((pstPage18Data->usCTAccumulatedTorque - usCTAccumulatedCrankTorquePrev) & MAX_USHORT);

            if (ulCTEventCountDiff==0)
            {
               pstEventStruct_->usParam2 = BPSRX_SUBEVENT_NOUPDATE;
               ucCTZeroSpeedCheck++;
            
               if(ucCTZeroSpeedCheck>=BPSRX_MAX_REPEATS) // Event sync zero speed condition
               {
                  stCalculatedStdCrankTorqueData.ulCTAverageCadence = 0;
                  stCalculatedStdCrankTorqueData.ulCTAngularVelocity = 0;
                  stCalculatedStdCrankTorqueData.ulCTAverageTorque = 0;
                  stCalculatedStdCrankTorqueData.ulCTAveragePower = 0;

                  pstEventStruct_->usParam2 = BPSRX_SUBEVENT_ZERO_SPEED;
               }
            }
            else
            {
               pstEventStruct_->usParam2 = BPSRX_SUBEVENT_UPDATE;
               ucCTZeroSpeedCheck=0;         // reset
               ucLocalSpeedFactor=1;         // assume moving

               if(ulCTAccumulatedCrankPeriodDiff==0) // Time sync zero speed condition
               {
                  pstEventStruct_->usParam2 = BPSRX_SUBEVENT_ZERO_SPEED;
                  ucLocalSpeedFactor=0;
                  stCalculatedStdCrankTorqueData.ulCTAverageCadence = 0;
                  stCalculatedStdCrankTorqueData.ulCTAngularVelocity = 0;
                  stCalculatedStdCrankTorqueData.ulCTAverageTorque = 0;
                  stCalculatedStdCrankTorqueData.ulCTAveragePower = 0;
               }
               else
               {
                  stCalculatedStdCrankTorqueData.ulCTAverageCadence = (ULONG)((((ulCTEventCountDiff*60)<<11)/(ulCTAccumulatedCrankPeriodDiff))*ucLocalSpeedFactor);
                  stCalculatedStdCrankTorqueData.ulCTAngularVelocity = (ULONG)((BPS_PI_CONSTANT*(ulCTEventCountDiff<<4)/ulCTAccumulatedCrankPeriodDiff)*ucLocalSpeedFactor);
                  stCalculatedStdCrankTorqueData.ulCTAverageTorque = (ULONG)((ulCTAccumulatedCrankTorqueDiff/(32*ulCTEventCountDiff))*ucLocalSpeedFactor);
                  stCalculatedStdCrankTorqueData.ulCTAveragePower = (ULONG)((BPS_PI_CONSTANT*(ulCTAccumulatedCrankTorqueDiff>>1)/ulCTAccumulatedCrankPeriodDiff)*ucLocalSpeedFactor);
               }
               ucCTEventCountPrev = pstPage18Data->ucCTEventCount;
               usCTAccumulatedCrankPeriodPrev = pstPage18Data->usCTAccumulatedCrankPeriod;
               usCTAccumulatedCrankTorquePrev = pstPage18Data->usCTAccumulatedTorque;
            }
         }
         break;                        
      }
      case BPS_PAGE_32:
      {
         BPSPage32_Data* pstPage32Data = BPSRX_GetPage32();
         // Standard Crank Torque (CT) Main Data Page 
         // NOTE: Big Endian Data
         pstPage32Data->ucCTFEventCount = pucEventBuffer_[BUFFER_INDEX_MESG_DATA+1];                                 
         pstPage32Data->usCTFSlope = (USHORT)(pucEventBuffer_[BUFFER_INDEX_MESG_DATA+2]<<8);
         pstPage32Data->usCTFSlope |= (USHORT)pucEventBuffer_[BUFFER_INDEX_MESG_DATA+3];
         pstPage32Data->usCTFTimeStamp = (USHORT)(pucEventBuffer_[BUFFER_INDEX_MESG_DATA+4]<<8); 
         pstPage32Data->usCTFTimeStamp |= (USHORT)pucEventBuffer_[BUFFER_INDEX_MESG_DATA+5];
         pstPage32Data->usCTFTorqueTickStamp = (USHORT)(pucEventBuffer_[BUFFER_INDEX_MESG_DATA+6]<<8);
         pstPage32Data->usCTFTorqueTickStamp |= (USHORT)pucEventBuffer_[BUFFER_INDEX_MESG_DATA+7];

         ulCTFEventCountDiff = (ULONG)((pstPage32Data->ucCTFEventCount - ucCTFEventCountPrev) & MAX_UCHAR);
         ulCTFTimeStampDiff = (ULONG)((pstPage32Data->usCTFTimeStamp - usCTFTimeStampPrev) & MAX_USHORT);
         ulCTFTorqueTickStampDiff = (ULONG)((pstPage32Data->usCTFTorqueTickStamp - usCTFTorqueTickStampPrev) & MAX_USHORT);

         if (bCTFFirstPageCheck==1)
         {
            bCTFFirstPageCheck=0;
            pstEventStruct_->usParam2 = BPSRX_SUBEVENT_FIRSTPAGE;              

            ucCTFEventCountPrev = pstPage32Data->ucCTFEventCount;
            usCTFTimeStampPrev = pstPage32Data->usCTFTimeStamp;
            usCTFTorqueTickStampPrev = pstPage32Data->usCTFTorqueTickStamp;
         }
         else
         {
            if (ulCTFEventCountDiff==0)
            {
               pstEventStruct_->usParam2 = BPSRX_SUBEVENT_NOUPDATE;
               ucCTFZeroSpeedCheck++;
            
               if(ucCTFZeroSpeedCheck>=BPSRX_MAX_REPEATS)  // Event sync zero speed condition
               {
                  pstEventStruct_->usParam2 = BPSRX_SUBEVENT_ZERO_SPEED;
                  stCalculatedStdCTFData.ulCTFAverageCadence = (ULONG)0;
                  stCalculatedStdCTFData.ulCTFTorqueFrequency = (ULONG)0;
                  stCalculatedStdCTFData.ulCTFAverageTorque = (ULONG)0;
                  stCalculatedStdCTFData.ulCTFAveragePower = (ULONG)0;
               }
            }
            else
            {
               pstEventStruct_->usParam2 = BPSRX_SUBEVENT_UPDATE;
               ucCTFZeroSpeedCheck = 0;
 
               stCalculatedStdCTFData.ulCTFAverageCadence = (ULONG)(((ulCTFEventCountDiff<<6)*1875)/ulCTFTimeStampDiff);
               stCalculatedStdCTFData.ulCTFTorqueFrequency = (ULONG)((((ulCTFTorqueTickStampDiff<<4)*125)/ulCTFTimeStampDiff) - usCTFOffset);
               stCalculatedStdCTFData.ulCTFAverageTorque = (ULONG)((10*stCalculatedStdCTFData.ulCTFTorqueFrequency)/pstPage32Data->usCTFSlope);
               stCalculatedStdCTFData.ulCTFAveragePower = (ULONG)(((stCalculatedStdCTFData.ulCTFAverageTorque*stCalculatedStdCTFData.ulCTFAverageCadence*BPS_PI_CONSTANT)/15)>>9);
               ucCTFEventCountPrev = pstPage32Data->ucCTFEventCount;
               usCTFTimeStampPrev = pstPage32Data->usCTFTimeStamp;
               usCTFTorqueTickStampPrev = pstPage32Data->usCTFTorqueTickStamp;
            }
         }
         break;                        
      }
      case GLOBAL_PAGE_80:
      {
         stPage80Data.ucHwVersion    = pucEventBuffer_[BUFFER_INDEX_MESG_DATA+3];
         stPage80Data.usManId        = (USHORT)pucEventBuffer_[BUFFER_INDEX_MESG_DATA+4];
         stPage80Data.usManId       |= (USHORT)(pucEventBuffer_[BUFFER_INDEX_MESG_DATA+5] << 8);
         stPage80Data.usModelNumber  = (USHORT)pucEventBuffer_[BUFFER_INDEX_MESG_DATA+6];
         stPage80Data.usModelNumber |= (USHORT)(pucEventBuffer_[BUFFER_INDEX_MESG_DATA+7] << 8);
         break;
      }
      case GLOBAL_PAGE_81:
      {
         stPage81Data.ucSwVersion     = pucEventBuffer_[BUFFER_INDEX_MESG_DATA+3];
         stPage81Data.ulSerialNumber  = (ULONG)pucEventBuffer_[BUFFER_INDEX_MESG_DATA+4];
         stPage81Data.ulSerialNumber |= (ULONG)pucEventBuffer_[BUFFER_INDEX_MESG_DATA+5] << 8;
         stPage81Data.ulSerialNumber |= (ULONG)pucEventBuffer_[BUFFER_INDEX_MESG_DATA+6] << 16;
         stPage81Data.ulSerialNumber |= (ULONG)pucEventBuffer_[BUFFER_INDEX_MESG_DATA+7] << 24;
         break;
      }
      case GLOBAL_PAGE_82:
      {
         stPage82Data.ulCumOperatingTime    = (ULONG)pucEventBuffer_[BUFFER_INDEX_MESG_DATA+3];
         stPage82Data.ulCumOperatingTime   |= (ULONG)pucEventBuffer_[BUFFER_INDEX_MESG_DATA+4] << 8;
         stPage82Data.ulCumOperatingTime   |= (ULONG)pucEventBuffer_[BUFFER_INDEX_MESG_DATA+5] << 16;
         stPage82Data.ucBattVoltage256      = pucEventBuffer_[BUFFER_INDEX_MESG_DATA+6];
         stPage82Data.ucBattVoltage         = COMMON82_COARSE_BATT_VOLTAGE(pucEventBuffer_[BUFFER_INDEX_MESG_DATA+7]);
         stPage82Data.ucBattStatus          = COMMON82_BATT_STATUS(pucEventBuffer_[BUFFER_INDEX_MESG_DATA+7]);
         stPage82Data.ucCumOperatingTimeRes = COMMON82_CUM_TIME_RESOLUTION(pucEventBuffer_[BUFFER_INDEX_MESG_DATA+7]);
         break;
      }
      default:
      {
         pstEventStruct_->eEvent = ANTPLUS_EVENT_UNKNOWN_PAGE;
         break;
      }
   }

   if(usDeviceNumber == 0)
   {
      if(ANT_RequestMessage(ucAntChannel, MESG_CHANNEL_ID_ID))
         bTransmit = TRUE;
   }
   
   return(bTransmit);
}

