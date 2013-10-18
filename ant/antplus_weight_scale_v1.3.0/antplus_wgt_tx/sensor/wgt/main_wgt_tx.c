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
#include "wgt_pages.h"
#include "wgt_tx.h"


#define WGTTX_CHANNEL   ((UCHAR) 0)

extern volatile USHORT usLowPowerMode;       // low power mode control

static const UCHAR aucNetworkKey[] = ANTPLUS_NETWORK_KEY;
static WGTTX_EVENT_RETURN stWGTEventStruct;
static BOOL bMayStepOnScale;

static void ProcessANTWGTTXEvents(WGTTX_EVENT_RETURN* pstEvent_);
static void ProcessAntEvents(UCHAR* pucEventBuffer_);
static void ProcessButtonState();                     // Simulates usage of the scale:
                                                      // '1' - User stands on scale
                                                      // '3' - User stands off scale
                                                      
                                                      
////////////////////////////////////////////////////////////////////////////
// main
//
// main function  
// Configures device simulator and Weight Scale TX channel.
//
// \return: This function does not return. 
////////////////////////////////////////////////////////////////////////////
void main_wgttx()
{
   UCHAR* pucRxBuffer;
   
   bMayStepOnScale = FALSE;
   
   // Main loop
   // Set network key. Do not send other commands
   // until after a response is received to this one.
   ANT_NetworkKey(ANTPLUS_NETWORK_NUMBER, aucNetworkKey);

   // Main loop
   while(TRUE)
   {
      usLowPowerMode = 0;//LPM1_bits;                 // Initialize the low power mode -  move to processor specific file and generic function
      
      // Check the state of the buttons
      IOBoard_Check_Button();                         // Check the button state
      ProcessButtonState();  
      
      if(WGTTX_CheckEvents(&stWGTEventStruct))        // Check for weight scale events
        ProcessANTWGTTXEvents(&stWGTEventStruct);     // Process weight scale Events    
   
      // Try to send or recieve a message to/from ANT
      // If there is a message to recieve, the message will be
      // recieved and buffered up
      // If there is a message to transmit, this function
      // will block until the message is sent.
      pucRxBuffer = ANTInterface_Transaction();       // Check if any data has been recieved from serial
             
      if(pucRxBuffer)
      {
         if(WGTTX_ChannelEvent(pucRxBuffer))
            usLowPowerMode = 0;
    
         ProcessAntEvents(pucRxBuffer);
      }
      _BIS_SR(usLowPowerMode);                        // Go to sleep if we can  -  move to processor specific file and generic function
   } 
} 


////////////////////////////////////////////////////////////////////////////
// ProcessANTWGTTXEvents
//
// Weight Scale event processor  
//
// Processes events recieved from Weight Scale module.
//
// \return: N/A 
///////////////////////////////////////////////////////////////////////////
void ProcessANTWGTTXEvents(WGTTX_EVENT_RETURN* pstEvent_)
{
   WGTPage_UserProfile* pstProfile;

   switch(pstEvent_->eEvent)
   {
      case WGTTX_EVENT_NEW_PROFILE:
         printf("Received new user profile.\n");            
         // Application can retrieve and store new profile, if desired
         pstProfile = WGTTX_GetReceivedUserProfile();
         printf("Profile ID is %u.\n", (USHORT) pstProfile->usID);
         break;
      case WGTTX_EVENT_UPDATED_PROFILE:
         printf("Received matching profile, with changes.\n");         
         // Application can retrieve and store updated profile, if desired  
         pstProfile = WGTTX_GetReceivedUserProfile(); 
         printf("Profile ID is %u.\n", (USHORT) pstProfile->usID);          
         break;
      case WGTTX_EVENT_INVALID_PROFILE:
         printf("Received a profile from the scale, but it is not valid.\n");
         break;
      case WGTTX_EVENT_USER_PROFILE_LOCKED:
         printf("User profile is locked.\n");
         printf("Please stand on scale.\n");
         IOBOARD_LED0_OUT &= ~IOBOARD_LED0_BIT;    // Turn ON LED
         bMayStepOnScale = TRUE;
         break;
      case WGTTX_EVENT_SENT_PROFILE:
         printf("Sent profile to display.\n");
         break;
      case WGTTX_EVENT_ANTFS_REQUEST:
         printf("ANT-FS Session requested, but not supported.\n");
         break;
      case WGTTX_EVENT_NONE:
      default:
         break;
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
               case EVENT_TX:
               {
                  break;
               }
               case EVENT_CHANNEL_CLOSED:
               {
                  IOBOARD_LED3_OUT |= IOBOARD_LED3_BIT;                 // Turn OFF LED
                  printf("Channel is closed.\n");
                  break;
               }
               case RESPONSE_NO_ERROR:
               {   
                  if (pucEventBuffer_[3] == MESG_OPEN_CHANNEL_ID)
                  {
                     IOBOARD_LED3_OUT &= ~IOBOARD_LED3_BIT;               // Turn ON LED  
                     printf("Initialization is complete.\n");
                  }
                  else if (pucEventBuffer_[3] == MESG_NETWORK_KEY_ID)
                  {
                     //Once we get a response to the set network key
                     //command, start opening the Weight Scale channel
                     WGTTX_Open(WGTTX_CHANNEL, (USHORT) WGTTX_SERIAL_NUMBER, WEIGHT_TX_TYPE); 
                  }
                  break;
               }
            }
         }
      }
   }      
}

////////////////////////////////////////////////////////////////////////////
// ProcessButtonState
//
// IO Board button state processing
//
// Processes state of buttons in IO board
// Pressing Button 1: user stands on Scale
// Pressing Button 3: user stands off Scale
//
// \return: N/A 
///////////////////////////////////////////////////////////////////////////
void ProcessButtonState()
{
   UCHAR ucButtonStates = IOBoard_Button_Pressed();

   if(ucButtonStates & BUTTON3_STATE_PRESSED)
   {
      if(bMayStepOnScale)
      {
         printf("User stepped off the scale.\n");
         // Continue transmitting while scale display is active, then close channel
         WGTTX_Close();        
      }
      IOBoard_Button_Clear(BUTTON3_STATE_OFFSET);
   }
   else if(ucButtonStates & BUTTON1_STATE_PRESSED)
   {
      if(bMayStepOnScale)
      {
         printf("User stepped on the scale.\n");
         IOBOARD_LED0_OUT |= IOBOARD_LED0_BIT;        // Turn OFF LED 
         // Obtain weight and apply user calculations
         WGTTX_CalculateWeight();
      }
      IOBoard_Button_Clear(BUTTON1_STATE_OFFSET);
   }
}

