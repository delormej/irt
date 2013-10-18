/*
This software is subject to the license described in the License.txt file
included with this software distribution. You may not use this file except in compliance
with this license.

Copyright (c) Dynastream Innovations Inc. 2013
All rights reserved.
*/

#include "types.h"
#include "antmessage.h"
#include "antdefines.h"
#include "ANTInterface.h"
#include "ioboard.h"
#include "hrm_tx.h"
#include "antplus.h"
#include "timer.h"



// ANT Channel
#define HRMTX_CHANNEL_TYPE            ((UCHAR) 0x10)     // Master

// Channel ID
#define HRMTX_DEVICE_TYPE             ((UCHAR) 0x78)	

// Message Periods
#define HRMTX_MSG_PERIOD              ((USHORT) 0x1F86)	// decimal 8070 (4.06Hz)

// Simulator HRM
#define HRM_BPM                        ((UCHAR) 150)     // Beats per minute


/////////////////////////////////////////////////////////////////////////////
// STATIC ANT Configuration Block
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// ANT Network Setup definitions
///////////////////////////////////////////////////////////////////////////////
static UCHAR aucTxBuf[8];                                // Primary transmit buffer
static UCHAR ucPageChange;											// Page change bit
static UCHAR ucMessageCount;										// Message counter	
static ULONG ulRunTime;												// Accumulated runtime.
static ULONG ulElapsedTime2;                             // Elapsed time in 2 second resolution.
static UCHAR ucExtMesgType;                              // Extended page number tracker


UCHAR ucHrmBpm;
UCHAR ucAlarmNumber;
UCHAR ucAntChannel;    
USHORT usDeviceNumber;  
UCHAR ucTransType;     

static BOOL HandleResponseEvents(UCHAR* pucBuffer_);
static BOOL HandleDataMessages(UCHAR* pucBuffer_, ANTPLUS_EVENT_RETURN* pstEventStruct_);
static void HRMTX_PulseEvent(USHORT usTime1024, UCHAR ucAlarmNumber_);                                         // Time of pulse event in (1/1024)s


/////////////////////////////////////////////////////////////////////////////////////////
// Functon: HRMTX_Init
//
// Descrption:
//
// Initialize all state variables. 
// Params:
// N/A.
//
// returns: N/A. 
/////////////////////////////////////////////////////////////////////////////////////////
void HRMTX_Init()
{
   // Intialize app variables
   ucPageChange = 0;	  
   ucMessageCount = 0;  
   ulRunTime = 0;	
   ucExtMesgType = 0;
   ucHrmBpm = HRM_BPM; 
   
}


//----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////
// HRMTX_Open
//
// Initlize HRM transmitter, queues up all configuration commands. 
//
// ucDeviceNumber_: ANT channel ID device number
//
// \return: TRUE if init sucesssful. 
////////////////////////////////////////////////////////////////////////////
BOOL HRMTX_Open( UCHAR ucAntChannel_, USHORT usDeviceNumber_, UCHAR ucTransType_)
{
   HRMTX_Init();
   
   ucAntChannel = ucAntChannel_;
   usDeviceNumber = usDeviceNumber_;
   ucTransType = ucTransType_;
   
   if(usDeviceNumber == 0)
      usDeviceNumber = 2;
   
   if(ucTransType == 0)
      ucTransType = 5;
      
   if (!ANT_AssignChannel(ucAntChannel,HRMTX_CHANNEL_TYPE,ANTPLUS_NETWORK_NUMBER ))
      return FALSE;
         
   // Register the timer for HRM simulator
   ucAlarmNumber = Timer_RegisterAlarm( HRMTX_PulseEvent, ALARM_FLAG_CONTINUOUS);
         
   if( ucAlarmNumber == 0)
      return(FALSE);
           
   return(TRUE);
}
/////////////////////////////////////////////////////////////////////////////////////////
// Functon: HRMRX_Close
//
// Description:
// Closes HRM recieve channel and initializes all state variables. Once the channel has 
// been successfuly closed, an HRMRX_EVENT_CHANNEL_CLOSED event will be generated via the
// callback function/
//
// Params:
// N/A
//
// Returns: TRUE if message was successfully sent to ANT, FALSE otherwise. 
// 
////////////////////////////////////////////////////////////////////////////////////////
BOOL HRMTX_Close()
{
   ANT_CloseChannel(ucAntChannel);
   return(TRUE);
}

////////////////////////////////////////////////////////////////////////////
// HR<TX_ChannelEvent
//
// Process ANT channel event. 
//
// pucEventBuffer_: pointer to buffer containg data recieved from ANT
// pstEventStruct_: Pointer to event return structure
//
// !!IMPORTANT: If this function returns true, it means that there is 
// a pending message to transmit to ANT. Do not sleep thread.
//
// \return: TRUE if a packet is to be transmitted 
////////////////////////////////////////////////////////////////////////////
BOOL HRMTX_ChannelEvent(UCHAR* pucEventBuffer_, ANTPLUS_EVENT_RETURN* pstEventStruct_)
{
   BOOL bTransmit = FALSE;

   UCHAR ucChannel = pucEventBuffer_[BUFFER_INDEX_CHANNEL_NUM] & 0x1F;
   pstEventStruct_->eEvent = ANTPLUS_EVENT_NONE;
   
   if(ucChannel == ucAntChannel)
   {
   
   
      if(pucEventBuffer_)
      {
         UCHAR ucANTEvent = pucEventBuffer_[BUFFER_INDEX_MESG_ID];   
      
         switch(ucANTEvent)
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
         
                  case EVENT_TX:
                  {
                     bTransmit = HandleDataMessages(pucEventBuffer_, pstEventStruct_);
                     break;
                  }
                  default:
                  {
                     // Handle other events at application level
                     break;
                  }
               }
            }   
         }
      }
   }
   return(bTransmit);
}



////////////////////////////////////////////////////////////////////////////
// HRMTX_PulseEvent
//
// Simulates a device event. Based on this event, the transmitter 
// data is simulated.  
//
// ulTime1024: Time of event in (1/1024)s
//
// \return: N/A. 
////////////////////////////////////////////////////////////////////////////
void HRMTX_PulseEvent(USHORT usTime1024, UCHAR ucAlarmNumber_)
{
   // Time is in (1/1024) seconds
   static USHORT usLastTime = 0;
   ULONG ulHR = 0;
   USHORT usBeatCount;

	usBeatCount = usTime1024 - usLastTime;                 // Latest time minus last time
   
   usLastTime = usTime1024; 

   aucTxBuf[0] = HRM_PAGE_4;                                 // This is page 4 

   // Previous event time
   aucTxBuf[2] = aucTxBuf[4];
   aucTxBuf[3] = aucTxBuf[5];

   // Current event time
   aucTxBuf[4] = (UCHAR)(usTime1024 & 0xFF);
   aucTxBuf[5] = (UCHAR)((usTime1024 >> 8) & 0xFF);

   // Event count
   aucTxBuf[6]++;

   ulHR = (ULONG)(0x0000F000);   // 60s/min and 1024 counts per second, so 60*1024/beatcount = BPM
   ulHR /= usBeatCount;

   aucTxBuf[7] = (UCHAR)(ulHR);
   

}

////////////////////////////////////////////////////////////////////////////
// HandleResponseEvents
//
// \return: N/A. 
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
            ANT_ChannelId(ucAntChannel, usDeviceNumber,HRMTX_DEVICE_TYPE, ucTransType );
            break;
         }
         case MESG_CHANNEL_ID_ID:
         {
            ANT_ChannelRFFreq(ucAntChannel, ANTPLUS_RF_FREQ);
            break;
         }
         case MESG_CHANNEL_RADIO_FREQ_ID:
         {
            ANT_ChannelPeriod(ucAntChannel, HRMTX_MSG_PERIOD);
            break;
         }
         case MESG_CHANNEL_MESG_PERIOD_ID:
         {
            USHORT usCounts = 60*ALARM_TIMER_PERIOD;
            
         
            ANT_OpenChannel(ucAntChannel);
            Timer_Start( ucAlarmNumber, usCounts/(USHORT)ucHrmBpm);    // Start the HRM PULSE timer         
            
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
BOOL HandleDataMessages(UCHAR* pucBuffer_, ANTPLUS_EVENT_RETURN* pstEventStruct_)
{
   BOOL bTransmit = FALSE;
   
   ulRunTime += ((ULONG)HRMTX_MSG_PERIOD);

   if(aucTxBuf[0] != 0)
   {
      ucPageChange += 0x20;
      aucTxBuf[0] &= ~0x80;					   // Clear the bit
      aucTxBuf[0] |= ucPageChange & 0x80;	   // Set bit if required. 
   }

   if(++ucMessageCount >= 65)
   {
      ucMessageCount = 0;
      ucExtMesgType++;

      if(ucExtMesgType >= 4)
      {
         ucExtMesgType = 1;

      }

      switch (ucExtMesgType)
      {
         case HRM_PAGE_1:
         {
            // Running time increases every 2 seconds. 
            // Since the period count is releative to a 32k clock,
            // need to divide by 65k to get 2 second resolution.
            if(ulRunTime / 0xFFFF)
            {
               ulRunTime -= 0xFFFF;   // Reset but keep the overflow
               ulElapsedTime2++;
            }
            
            aucTxBuf[0] = HRM_PAGE_1;
            aucTxBuf[1] = (ulElapsedTime2 >> 8) & 0xFF;
            aucTxBuf[2] = (ulElapsedTime2 >> 16) & 0xFF;
            aucTxBuf[3] = (ulElapsedTime2 >> 24) & 0xFF;
            break;
         }
         case HRM_PAGE_2:
         {
            aucTxBuf[0] = HRM_PAGE_2;
            aucTxBuf[1] = HRMTX_MFG_ID;            // Mfg ID
            aucTxBuf[2] = LOW_BYTE(HRMTX_SERIAL_NUMBER); // low byte of high serial #
            aucTxBuf[3] = HIGH_BYTE(HRMTX_SERIAL_NUMBER);// high byte of high serial #
            break;
         }
         case HRM_PAGE_3:
         {
            aucTxBuf[0] = HRM_PAGE_3;
            aucTxBuf[1] = HRMTX_HW_VERSION;        // HW version
            aucTxBuf[2] = HRMTX_SW_VERSION;        // SW version
            aucTxBuf[3] = HRMTX_MODEL_NUMBER;      // Model Number
            break;
         }
      }
   }

   if(ANT_Broadcast(ucAntChannel, aucTxBuf))
   {
      pstEventStruct_->eEvent = ANTPLUS_EVENT_TRANSMIT;
      bTransmit = TRUE;
   }
   
   return(bTransmit);
}


