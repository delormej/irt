/*
This software is subject to the license described in the License.txt file
included with this software distribution. You may not use this file except in compliance
with this license.

Copyright (c) Dynastream Innovations Inc. 2013
All rights reserved.
*/

#include "types.h"
#include "antinterface.h"
#include "antdefines.h"
#include "timer.h"
#include "printf.h"

#include "ioboard.h"
#include "hrm_tx.h"
#include "antplus.h"



extern volatile USHORT usLowPowerMode;                          // low power mode control

// ANT Channel Settings
#define HRMTX_ANT_CHANNEL                   ((UCHAR) 1)        // Default ANT Channel

// other defines
#define HRM_PRECISION                              ((ULONG)1000)

static const UCHAR aucNetworkKey[] = ANTPLUS_NETWORK_KEY;


static void ProcessANTHRMTXEvents(ANTPLUS_EVENT_RETURN* pstEvent_);
static void ProcessAntEvents(UCHAR* pucEventBuffer_);

//----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////
// main
//
// main function  
//
// Configures device simulator and HRM TX channel.
//
// \return: This function does not return. 
////////////////////////////////////////////////////////////////////////////
void main_hrmtx()
{

   UCHAR* pucRxBuffer;
   ANTPLUS_EVENT_RETURN stHRMEventStruct;

//   Device_SetSimTIck((60*16)/150);                          // Initialize heart beat simulator (timer)
   
   
   

   // Set network key. Do not send other commands
   // until after a response is recieved to this one.
   ANT_NetworkKey(ANTPLUS_NETWORK_NUMBER, aucNetworkKey);

   // Main loop
   while(TRUE)
   {
      usLowPowerMode = 0;//LPM1_bits;                       // nitialize the low power mode -  move to processor specific file and generic function
      
      pucRxBuffer = ANTInterface_Transaction();                // Check if any data has been recieved from serial
      
      if(pucRxBuffer)
      {
         if(HRMTX_ChannelEvent(pucRxBuffer, &stHRMEventStruct))
            usLowPowerMode = 0;
         
         ProcessANTHRMTXEvents(&stHRMEventStruct);               
         
         ProcessAntEvents(pucRxBuffer);
      }
      
      _BIS_SR(usLowPowerMode);                              // Go to sleep if we can  -  move to processor specific file and generic function
   }   
   
   
} 


////////////////////////////////////////////////////////////////////////////
// ProcessANTHRMRXEvents
//
// HRM Reciever event processor  
//
// Processes events recieved from HRM module.
//
// \return: N/A 
///////////////////////////////////////////////////////////////////////////
void ProcessANTHRMTXEvents(ANTPLUS_EVENT_RETURN* pstEvent_)
{

   switch(pstEvent_->eEvent)
   {

       case ANTPLUS_EVENT_TRANSMIT:
      {
         IOBOARD_LED3_OUT &= ~IOBOARD_LED3_BIT;                // Turn ON LED
         printf("Transmit.\n");
         IOBOARD_LED3_OUT |= IOBOARD_LED3_BIT;                 // Turn OFF LED
         break;
      } 
      case ANTPLUS_EVENT_NONE:
      default:
      {
         break;
      }
      
   }
}

////////////////////////////////////////////////////////////////////////////
// ProcessAntEvents
//
// Generic ANT Event handling  
//
// \return: N/A 
///////////////////////////////////////////////////////////////////////////
void ProcessAntEvents(UCHAR* pucEventBuffer_)
{
   
   if(pucEventBuffer_)
   {
      UCHAR ucANTEvent = pucEventBuffer_[BUFFER_INDEX_MESG_ID];   
      
      switch( ucANTEvent )
      {
         case MESG_RESPONSE_EVENT_ID:
         {
            switch(pucEventBuffer_[BUFFER_INDEX_RESPONSE_CODE])
            {
               case EVENT_RX_SEARCH_TIMEOUT:
               {
                  IOBOARD_LED0_OUT |= IOBOARD_LED0_BIT;         
                  break;
               }
               case EVENT_TX:
               {
                  break;
               }
               
               case RESPONSE_NO_ERROR:
               {   
                  if (pucEventBuffer_[3] == MESG_OPEN_CHANNEL_ID)
                  {
                     IOBOARD_LED0_OUT &= ~IOBOARD_LED0_BIT;         
                     printf("initialization is complete.\n");
                  }
                  else if (pucEventBuffer_[3] == MESG_CLOSE_CHANNEL_ID)
                  {
                  }
                  else if (pucEventBuffer_[3] == MESG_NETWORK_KEY_ID)
                  {
                     //Once we get a response to the set network key
                     //command, start opening the HRM channel
                     HRMTX_Open(HRMTX_ANT_CHANNEL, 49, 5); 
                  }
                  break;
               }
            }
         }
      }
   }      
}





