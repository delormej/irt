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
#include "hrm_rx.h"




// ANT Channel settings
#define ANT_CHANNEL_HRMRX                          ((UCHAR) 0)        // Default ANT Channel

// other defines
#define HRM_PRECISION                              ((ULONG)1000)


extern volatile USHORT usLowPowerMode;                                         // low power mode control

static const UCHAR aucNetworkKey[] = ANTPLUS_NETWORK_KEY;

static void ProcessANTHRMRXEvents(ANTPLUS_EVENT_RETURN* pstEvent_);
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
void main_hrm()
{

   UCHAR* pucRxBuffer;
   ANTPLUS_EVENT_RETURN stEventStruct;



   ANT_Reset();



   // Main loop

   while(TRUE)
   {
      usLowPowerMode = 0;//LPM1_bits;                          // initialize the low power mode -  move to processor specific file and generic function

      pucRxBuffer = ANTInterface_Transaction();                // Check if any data has been recieved from serial

      if(pucRxBuffer)
      {

         if(HRMRX_ChannelEvent(pucRxBuffer, &stEventStruct))
            usLowPowerMode = 0;
         ProcessANTHRMRXEvents(&stEventStruct);

         ProcessAntEvents(pucRxBuffer);
      }

      _BIS_SR(usLowPowerMode);                                 // go to sleep if we can  -  move to processor specific file and generic function
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
void ProcessANTHRMRXEvents(ANTPLUS_EVENT_RETURN* pstEvent_)
{
   static UCHAR ucPreviousBeatCount = 0;

   switch (pstEvent_->eEvent)
   {

      case ANTPLUS_EVENT_CHANNEL_ID:
      {
         // Can store this device number for future pairings so that 
         // wild carding is not necessary.
         printf("Device Number is %d\n", pstEvent_->usParam1);
         printf("Transmission type is %d\n\n", pstEvent_->usParam2);
         break;
      }
      case ANTPLUS_EVENT_PAGE:
      {
         HRMPage0_Data* pstPage0Data = HRMRX_GetPage0(); //common data
         BOOL bCommonPage = FALSE;

         IOBOARD_LED3_OUT &= ~IOBOARD_LED3_BIT;    // TURN ON LED 3

         //print formulated page identifier
         if (pstEvent_->usParam1 <= HRM_PAGE_4)
            printf("Heart Rate Monitor Page %d\n", pstEvent_->usParam1);

         // Get data correspinding to the page. Only get the data you 
         // care about.
         switch(pstEvent_->usParam1)
         {
            case HRM_PAGE_0:
            {
               bCommonPage = TRUE;
               break;
            }
            case HRM_PAGE_1:
            {
               HRMPage1_Data* pstPage1Data = HRMRX_GetPage1();
               ULONG ulMinutes, ulHours, ulDays, ulSeconds;

               ulDays = (ULONG)((pstPage1Data->ulOperatingTime) / 86400);  //1 day == 86400s
               ulHours = (ULONG)((pstPage1Data->ulOperatingTime) % 86400); // half the calculation so far
               ulMinutes = ulHours % (ULONG)3600;
               ulSeconds = ulMinutes % (ULONG)60;
               ulHours /= (ULONG)3600; //finish the calculations: hours = 1hr == 3600s
               ulMinutes /= (ULONG)60; //finish the calculations: minutes = 1min == 60s

               printf("Cumulative operating time: %dd ", ulDays);
               printf("%dh ", ulHours);
               printf("%dm ", ulMinutes);
               printf("%ds\n\n", ulSeconds);
               bCommonPage = TRUE;
               break;
            }
            case HRM_PAGE_2:
            {
               HRMPage2_Data* pstPage2Data = HRMRX_GetPage2();

               printf("Manufacturer ID: %u\n", pstPage2Data->ucManId);
               printf("Serial No (upper 16-bits): 0x%X\n", pstPage2Data->ulSerialNumber);               
               bCommonPage = TRUE;
               break;
            }
            case HRM_PAGE_3:
            {
               HRMPage3_Data* pstPage3Data = HRMRX_GetPage3();

               printf("Hardware Rev ID %u ", pstPage3Data->ucHwVersion);
               printf("Model %u\n", pstPage3Data->ucModelNumber);
               printf("Software Ver ID %u\n", pstPage3Data->ucSwVersion);
               bCommonPage = TRUE;
               break;
            }
            case HRM_PAGE_4:
            {
               HRMPage4_Data* pstPage4Data = HRMRX_GetPage4();

               printf("Previous heart beat event: %u.", (ULONG)(pstPage4Data->usPreviousBeat/1024));
               printf("%03u s\n", (ULONG)((((pstPage4Data->usPreviousBeat % 1024) * HRM_PRECISION) + 512) / 1024));

               if((pstPage0Data->ucBeatCount - ucPreviousBeatCount) == 1)	// ensure that there is only one beat between time intervals
               {
                  USHORT usR_RInterval = pstPage0Data->usBeatTime - pstPage4Data->usPreviousBeat;	// subtracting the event time gives the R-R interval
                  printf("R-R Interval: %u.", (ULONG)(usR_RInterval/1024));
                  printf("%03u s\n", (ULONG)((((usR_RInterval % 1024) * HRM_PRECISION) + 512) / 1024));
               }
               ucPreviousBeatCount = pstPage0Data->ucBeatCount;

               bCommonPage = TRUE;
               break;
            }
            default:
            {
               // ASSUME PAGE 0
               printf("Unknown format\n\n");
               break; 
            }
         }
         if(bCommonPage)
         {
            printf("Time of last heart beat event: %u.", (ULONG)(pstPage0Data->usBeatTime/1024));
            printf("%03u s\n", (ULONG)((((pstPage0Data->usBeatTime % 1024) * HRM_PRECISION) + 512) / 1024));
            printf("Heart beat count: %u\n", pstPage0Data->ucBeatCount);
            printf("Instantaneous heart rate: %u bpm\n\n", pstPage0Data->ucComputedHeartRate);
         }
         IOBOARD_LED3_OUT |= IOBOARD_LED3_BIT;    // TURN OFF LED 3
         break;
      }

      case ANTPLUS_EVENT_UNKNOWN_PAGE:  // Decode unknown page manually
      case ANTPLUS_EVENT_NONE:
      default:
      {
         break;
      }  
   }
}

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

                  if(pucEventBuffer_[3] == MESG_SYSTEM_RESET_ID)  //For modules that do not support the startup message (0x6F)
                  {
                     ANT_NetworkKey(ANTPLUS_NETWORK_NUMBER, aucNetworkKey);
                     printf("Reset is complete.\n");
                  }

                  else if (pucEventBuffer_[3] == MESG_OPEN_CHANNEL_ID)
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
                     HRMRX_Open(ANT_CHANNEL_HRMRX, 0, 0); 
                  }
                  break;
               }
            }
            break;
         }
         case MESG_START_UP:  //Supported by AP2 and later
         {
            ANT_NetworkKey(ANTPLUS_NETWORK_NUMBER, aucNetworkKey);
            printf("Reset is complete.\n");
            break;
         }
      }
   }      
}
