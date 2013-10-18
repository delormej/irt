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
#include "timer.h"
#include "antplus.h"
#include "common_pages.h"
#include "wgt_pages.h"
#include "wgt_tx.h"


// Simulator parameters
#define WGT_SIMULATED_WEIGHT           ((USHORT) 8562)         // 85.62 Kg
#define WGT_SIMULATED_HYDRATION        ((USHORT) 3511)         // 35.11%
#define WGT_SIMULATED_BODY_FAT         ((USHORT) 1927)         // 19.27%
#define WGT_SIMULATED_ACTIVE_MET_RATE  ((USHORT) 3792)         // 948 Kcal
#define WGT_SIMULATED_BASAL_MET_RATE   ((USHORT) 7848)         // 1962 Kcal
#define WGT_SIMULATED_MUSCLE_MASS      ((USHORT) 4420)         // 44.2 Kg
#define WGT_SIMULATED_BONE_MASS        ((UCHAR) 24)            // 2.4 Kg

#define WGT_UP_LOCK_TIMEOUT            ((USHORT) 8)            // 8 seconds

#define WGT_RX_PROFILES                TRUE     // Support profile exchange in scale
#define WGT_SET_PROFILE                FALSE    //  Set to TRUE to set a profile on the scale
#if WGT_SET_PROFILE == TRUE
   // Sample Profile
   #define WGT_SIMULATED_ID               ((USHORT) 5)
   #define WGT_SIMULATED_GENDER           WEIGHT_GENDER_MALE
   #define WGT_SIMULATED_AGE              ((UCHAR) 25)      // years
   #define WGT_SIMULATED_HEIGHT           ((UCHAR) 185)     // cm
   #define WGT_SIMULATED_ATHLETE          WEIGHT_NOT_ATHLETE
   #define WGT_SIMULATED_ACTIVITY_LEVEL   ((UCHAR) 4)   
#else
   // No Profile
   #define WGT_SIMULATED_ID               WEIGHT_PROFILE_UNDEF
   #define WGT_SIMULATED_GENDER           WEIGHT_GENDER_UNDEF
   #define WGT_SIMULATED_AGE              WEIGHT_AGE_UNDEF
   #define WGT_SIMULATED_HEIGHT           WEIGHT_HEIGHT_UNDEF
   #define WGT_SIMULATED_ATHLETE          WEIGHT_NOT_ATHLETE
   #define WGT_SIMULATED_ACTIVITY_LEVEL   WEIGHT_DESCR_UNDEF
#endif

#define WGT_EVENT_QUEUE_SIZE             ((UCHAR) 0x04)  // Event queue size

typedef struct
{
   WGTTX_EVENT_RETURN* astQueue;
   UCHAR ucHead;
   UCHAR ucTail;
} WGT_EventQueue;

/////////////////////////////////////////////////////////////////////////////
// STATIC ANT Configuration Block
/////////////////////////////////////////////////////////////////////////////

static UCHAR aucTxBuf[8];                                // Primary transmit buffer

// Data pages created in the scale
static WGTPage_UserProfile stUserProfileData;
static WGTPage_BodyWeight stBodyWeightData;
static WGTPage_BodyComposition stBodyCompositionData;
static WGTPage_MetabolicInfo stMetabolicInfoData;
static WGTPage_BodyMass stBodyMassData;
static CommonPage80_Data stCommon80Data;
static CommonPage81_Data stCommon81Data;

static WGTPage_UserProfile stReceivedUserProfile;  // profile received from display

// ANT channel parameters
static UCHAR ucAntChannel;    
static USHORT usDeviceNumber;  
static UCHAR ucTransType; 

static UCHAR ucAlarmNumber;                 // Timer alarm

static BOOL bUserProfileLocked;   
static BOOL bTxUserData;
static BOOL bTxUserProfile; 

// Event Queue
static WGTTX_EVENT_RETURN astEventQueue[WGT_EVENT_QUEUE_SIZE];
static WGT_EventQueue stTheEventQueue;    // Event queue

static void Init();
static BOOL HandleResponseEvents(UCHAR* pucBuffer_);
static BOOL HandleTransmitMessages(UCHAR* pucBuffer_);
static BOOL HandleReceiveMessages(UCHAR* pucBuffer_);
static void CheckForProfiles();
static BOOL HasProfileChanged(WGTPage_UserProfile* pstNewProfile);
static void ApplyProfile(WGTPage_UserProfile* pstNewProfile);
static void LockProfile();
static void SetMeasurementsInvalid();
static void SetMeasurementsComputing();
static BOOL SetEvent(UCHAR ucEventCode_);
static BOOL SetTimeout(USHORT usTimeout_);
static void DisableTimeout(void);
static void WGTTX_Tick(USHORT usTimeStamp_, UCHAR ucAlarmNumber_); 



/////////////////////////////////////////////////////////////////////////////////////////
// WGTTX_Open
//
// Initlize Weight Scale transmitter, queues up all configuration commands. 
//
// ucDeviceNumber_: ANT channel ID device number
//
// \return: TRUE if init sucesssful. 
/////////////////////////////////////////////////////////////////////////////////////////
BOOL WGTTX_Open( UCHAR ucAntChannel_, USHORT usDeviceNumber_, UCHAR ucTransType_)
{
   Init();
   
   ucAntChannel = ucAntChannel_;
   usDeviceNumber = usDeviceNumber_;
   ucTransType = ucTransType_;
   
   if(usDeviceNumber == 0) // Wildcards not valid in master devices
      usDeviceNumber = 2;
   
   if(ucTransType == 0)    // Wildcards not valid in master devices
      ucTransType = 5;
      
   if (!ANT_AssignChannel(ucAntChannel, WEIGHT_TX_CHANNEL_TYPE,ANTPLUS_NETWORK_NUMBER))
      return FALSE;
           
   return(TRUE);
}


/////////////////////////////////////////////////////////////////////////////////////////
// WGTTX_Close
//
// Description:
// Closes Weight Scale transmit channel. Once the channel has been successfuly closed, 
// an EVENT_CHANNEL_CLOSED event will be generated via the callback function
//
// Params:
// N/A
//
// Returns: TRUE if message was successfully sent to ANT, FALSE otherwise. 
// 
////////////////////////////////////////////////////////////////////////////////////////
BOOL WGTTX_Close()
{
   ANT_CloseChannel(ucAntChannel);
   return(TRUE);
}


/////////////////////////////////////////////////////////////////////////////////////////
// WGTTX_CalculateWeight
//
// Description:
//
// Simulates calculation of weight and advanced data when user stands on scale 
// Params:
// N/A.
//
// returns: N/A. 
/////////////////////////////////////////////////////////////////////////////////////////
void WGTTX_CalculateWeight()
{
   // Only allow user to stand on the scale after the profile has been locked
   if(bUserProfileLocked)
   {
      stBodyWeightData.usBodyWeight100 = WGT_SIMULATED_WEIGHT;
   
      if(bTxUserData)
      {
         // Apply user profile to calculations
         stBodyCompositionData.usHydrationP100 = WGT_SIMULATED_HYDRATION;
         stBodyCompositionData.usBodyFatP100 = WGT_SIMULATED_BODY_FAT;
         stMetabolicInfoData.usActiveMetRate4 = WGT_SIMULATED_ACTIVE_MET_RATE;
         stMetabolicInfoData.usBasalMetRate4 = WGT_SIMULATED_BASAL_MET_RATE;
         stBodyMassData.usMuscleMass100 = WGT_SIMULATED_MUSCLE_MASS;
         stBodyMassData.ucBoneMass10 = WGT_SIMULATED_BONE_MASS;
      }
   }
}

/////////////////////////////////////////////////////////////////////////////////////////
// WGTTX_GetReceivedUserProfile
//
// Description:
//
// Obtains the user profile received from a display device
// Params:
// N/A.
//
// returns: pointer to user profile
/////////////////////////////////////////////////////////////////////////////////////////
WGTPage_UserProfile* WGTTX_GetReceivedUserProfile()
{
   return &stReceivedUserProfile;
}

////////////////////////////////////////////////////////////////////////////
// WGTTX_ChannelEvent
//
// Process ANT channel event. 
//
// pucEventBuffer_: pointer to buffer containg data recieved from ANT
//
// !!IMPORTANT: If this function returns true, it means that there is 
// a pending message to transmit to ANT. Do not sleep thread.
//
// \return: TRUE if a packet is to be transmitted 
////////////////////////////////////////////////////////////////////////////
BOOL WGTTX_ChannelEvent(UCHAR* pucEventBuffer_)
{
   BOOL bTransmit = FALSE;

   UCHAR ucChannel = pucEventBuffer_[BUFFER_INDEX_CHANNEL_NUM] & 0x1F;
   
   if(ucChannel == ucAntChannel)
   {   
      if(pucEventBuffer_)
      {
         UCHAR ucANTEvent = pucEventBuffer_[BUFFER_INDEX_MESG_ID];   
      
         switch(ucANTEvent)
         {
            case MESG_ACKNOWLEDGED_DATA_ID:
            case MESG_BROADCAST_DATA_ID:
            {
               bTransmit = HandleReceiveMessages(pucEventBuffer_);
               break;
            }
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
                     bTransmit = HandleTransmitMessages(pucEventBuffer_);
                     break;
                  }
                  default:
                  {
                     // Handle other events at application level
                     break;
                  }
               }
               break;
            }   
         }
      }
   }
   return(bTransmit);
}

///////////////////////////////////////////////////////////////////////
// WGTTX_CheckEvents
//
// Check for pending weight scale events
//
// Returns TRUE if there is an event to process, and copies the event
// parameters in the return event
//
///////////////////////////////////////////////////////////////////////
BOOL WGTTX_CheckEvents(WGTTX_EVENT_RETURN* pstEvent_)
{
   if(stTheEventQueue.ucHead != stTheEventQueue.ucTail)    // Check for pending events
   {
      // Copy event parameters into return event
      pstEvent_->eEvent = stTheEventQueue.astQueue[stTheEventQueue.ucTail].eEvent;

      // Release event queue
      stTheEventQueue.ucTail = ((stTheEventQueue.ucTail + 1) & (WGT_EVENT_QUEUE_SIZE - 1));    
      return TRUE;
   }

   return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Init
//
// \return: N/A. 
/////////////////////////////////////////////////////////////////////////////////////////
void Init()
{
   // Intialize app variables
   bUserProfileLocked = FALSE;
   bTxUserProfile = FALSE;
   bTxUserData = FALSE;
   
   stUserProfileData.usID = WGT_SIMULATED_ID;
   
   stUserProfileData.stCapabilities.stCapabilitiesBitField.ucReserved = WEIGHT_CAPAB_RESERVED;
   stUserProfileData.stCapabilities.stCapabilitiesBitField.bScaleSet = WGT_SET_PROFILE;
   stUserProfileData.stCapabilities.stCapabilitiesBitField.bScaleExchange = WGT_RX_PROFILES;
   stUserProfileData.stCapabilities.stCapabilitiesBitField.bAntfs = FALSE;
   stUserProfileData.stCapabilities.stCapabilitiesBitField.bNoDisplayExchange = TRUE;   // Initially, we do not know if the display supports profile exchange

   stUserProfileData.stUserInfo.stAgeGender.ucAge = WGT_SIMULATED_AGE;
   stUserProfileData.stUserInfo.stAgeGender.bGender = WGT_SIMULATED_GENDER;
   stUserProfileData.ucHeight = WGT_SIMULATED_HEIGHT;
   stUserProfileData.stDescr.stDescriptiveBitField.ucActivityLevel = WGT_SIMULATED_ACTIVITY_LEVEL;
   stUserProfileData.stDescr.stDescriptiveBitField.ucReserved = WEIGHT_DESCR_RESERVED;
   stUserProfileData.stDescr.stDescriptiveBitField.bAthlete = WGT_SIMULATED_ATHLETE;
      
   SetMeasurementsComputing();
   
   stCommon80Data.ucHwVersion = WGTTX_HW_VERSION;
   stCommon80Data.usManId = WGTTX_MFG_ID;
   stCommon80Data.usModelNumber = WGTTX_MODEL_NUMBER;
   
   stCommon81Data.ucSwVersion = WGTTX_SW_VERSION;
   stCommon81Data.ulSerialNumber = WGTTX_SERIAL_NUMBER;
   
   // Initialize timer
   ucAlarmNumber = Timer_RegisterAlarm(WGTTX_Tick, ALARM_FLAG_ONESHOT);      // Register timer alarm 

   // Initialize event queue
   stTheEventQueue.astQueue = astEventQueue;
   stTheEventQueue.ucHead = 0;  
   stTheEventQueue.ucTail = 0;   
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
            ANT_ChannelId(ucAntChannel, usDeviceNumber, WEIGHT_DEVICE_TYPE, ucTransType );
            break;
         }
         case MESG_CHANNEL_ID_ID:
         {
            ANT_ChannelRFFreq(ucAntChannel, ANTPLUS_RF_FREQ);
            break;
         }
         case MESG_CHANNEL_RADIO_FREQ_ID:
         {
            ANT_ChannelPeriod(ucAntChannel, WEIGHT_MSG_PERIOD);
            break;
         }
         case MESG_CHANNEL_MESG_PERIOD_ID:
         {
            ANT_OpenChannel(ucAntChannel);
            break;
         }
         case MESG_OPEN_CHANNEL_ID:
         {
#if !defined(REFERENCE_DESIGN)
            ANT_ChannelPower(ucAntChannel, 2);
#endif
            CheckForProfiles();
            break;
         }
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
// HandleTransmitMessages
//
// \return: N/A. 
////////////////////////////////////////////////////////////////////////////
BOOL HandleTransmitMessages(UCHAR* pucBuffer_)
{
   BOOL bTransmit = FALSE;
   
   static UCHAR ucMsgCount = 0;
   UCHAR ucPageNum = WEIGHT_PAGE_BODY_WEIGHT; 
   
   if(bTxUserData && (stBodyWeightData.usBodyWeight100 < WEIGHT_COMPUTING))   // Make sure we have a valid profile, and body weight is calculated
   {
      if(ucMsgCount % 4 == 1 && bTxUserData)    // Interleaving advanced data pages
         ucPageNum = WEIGHT_PAGE_BODY_COMPOSITION;
      if(ucMsgCount % 4 == 2 && bTxUserData)
         ucPageNum = WEIGHT_PAGE_METABOLIC_INFO;
      if(ucMsgCount % 4 == 3 && bTxUserData)
         ucPageNum = WEIGHT_PAGE_BODY_MASS;
   }
   
   if(ucMsgCount == (WEIGHT_INTERVAL_COMMON-1))    // Interleaving common pages
      ucPageNum = GLOBAL_PAGE_80;
   if(++ucMsgCount >= (WEIGHT_INTERVAL_COMMON*2))
   {
      ucPageNum = GLOBAL_PAGE_81;
      ucMsgCount = 0;
   }

   if(bTxUserProfile)         
   {
      ucPageNum = WEIGHT_PAGE_USER_PROFILE;     // Transmit user profile 
      bTxUserProfile = FALSE;                   // Only once
      SetEvent(WGTTX_EVENT_SENT_PROFILE);
   }
   
   switch(ucPageNum)
   {
      case WEIGHT_PAGE_BODY_WEIGHT:
         aucTxBuf[0] = ucPageNum;
         aucTxBuf[1] = (UCHAR) stUserProfileData.usID;     // Active user profile ID (low byte)
         aucTxBuf[2] = (UCHAR) (stUserProfileData.usID >> 8);    // Active user profile ID (high byte)
         aucTxBuf[3] = stUserProfileData.stCapabilities.ucCapabilities;    // Scale capabilities
         aucTxBuf[4] = WEIGHT_RESERVED;
         aucTxBuf[5] = WEIGHT_RESERVED;
         aucTxBuf[6] = (UCHAR) stBodyWeightData.usBodyWeight100;          // Body weight in 1/100 kg (low byte)
         aucTxBuf[7] = (UCHAR) (stBodyWeightData.usBodyWeight100 >> 8);      // Body weight in 1/100 kg (high byte)
         break;
      case WEIGHT_PAGE_BODY_COMPOSITION:
         aucTxBuf[0] = ucPageNum;
         aucTxBuf[1] = (UCHAR) stUserProfileData.usID;        // Active user profile ID (low byte)
         aucTxBuf[2] = (UCHAR) (stUserProfileData.usID >> 8);    // Active user profile ID (high byte)
         aucTxBuf[3] = WEIGHT_RESERVED;
         aucTxBuf[4] = (UCHAR) stBodyCompositionData.usHydrationP100;          // % Hydration in intervals of 0.01% (low byte)
         aucTxBuf[5] = (UCHAR) (stBodyCompositionData.usHydrationP100 >> 8);      // % Hydration in intervals of 0.01% (high byte)
         aucTxBuf[6] = (UCHAR) stBodyCompositionData.usBodyFatP100;            // % Body fat in intervals of 0.01% (low byte)
         aucTxBuf[7] = (UCHAR) (stBodyCompositionData.usBodyFatP100 >> 8);        // % Body fat in intervals of 0.01% (high byte)
         break;
      case WEIGHT_PAGE_METABOLIC_INFO:
         aucTxBuf[0] = ucPageNum;
         aucTxBuf[1] = (UCHAR) stUserProfileData.usID;        // Active user profile ID (low byte)
         aucTxBuf[2] = (UCHAR) (stUserProfileData.usID >> 8); // Active user profile ID (high byte)
         aucTxBuf[3] = WEIGHT_RESERVED;
         aucTxBuf[4] = (UCHAR) stMetabolicInfoData.usActiveMetRate4;            // Active metabolic rate in kJ (low byte)
         aucTxBuf[5] = (UCHAR) (stMetabolicInfoData.usActiveMetRate4 >> 8);     // Active metabolic rate in kJ (high byte)
         aucTxBuf[6] = (UCHAR) stMetabolicInfoData.usBasalMetRate4;          // Basal metabolic rate in kJ (low byte)
         aucTxBuf[7] = (UCHAR) (stMetabolicInfoData.usBasalMetRate4 >> 8);      // Basal metabolic rate in kJ (high byte)
         break;
      case WEIGHT_PAGE_BODY_MASS:
         aucTxBuf[0] = ucPageNum;
         aucTxBuf[1] = (UCHAR) stUserProfileData.usID;        // Active user profile ID (low byte)
         aucTxBuf[2] = (UCHAR) (stUserProfileData.usID >> 8);   // Active user profile ID (high byte)
         aucTxBuf[3] = WEIGHT_RESERVED;
         aucTxBuf[4] = WEIGHT_RESERVED;
         aucTxBuf[5] = (UCHAR) stBodyMassData.usMuscleMass100;         // Muscle mass in 1/100 kg (low byte)
         aucTxBuf[6] = (UCHAR) (stBodyMassData.usMuscleMass100 >> 8);  // Muscle mass in 1/100 kg (high byte)
         aucTxBuf[7] = stBodyMassData.ucBoneMass10;                    // Bone mass in 1/10kg
         break;
      case WEIGHT_PAGE_USER_PROFILE:
         aucTxBuf[0] = ucPageNum;
         aucTxBuf[1] = (UCHAR) stUserProfileData.usID;       // Active user profile ID (low byte)
         aucTxBuf[2] = (UCHAR) (stUserProfileData.usID >> 8);    // Active user profile ID (high byte)
         aucTxBuf[3] = stUserProfileData.stCapabilities.ucCapabilities;   // Scale capabilities
         aucTxBuf[4] = WEIGHT_RESERVED;
         aucTxBuf[5] = stUserProfileData.stUserInfo.ucAgeGender;    // Age and gender
         aucTxBuf[6] = stUserProfileData.ucHeight;                  // Height
         aucTxBuf[7] = stUserProfileData.stDescr.ucDescriptiveBitField;  // Descriptive bit field
         break;
      case GLOBAL_PAGE_80:
         aucTxBuf[0] = ucPageNum;                 // Page number
         aucTxBuf[1] = WEIGHT_RESERVED;
         aucTxBuf[2] = WEIGHT_RESERVED;                  
         aucTxBuf[3] = stCommon80Data.ucHwVersion;                // Hw revision
         aucTxBuf[4] = stCommon80Data.usManId & 0xFF;             // Low byte of Mfg ID
         aucTxBuf[5] = (stCommon80Data.usManId >> 8) & 0xFF;         // High byte of Mfg ID
         aucTxBuf[6] = stCommon80Data.usModelNumber & 0xFF;          // Low byte of Model Number
         aucTxBuf[7] = (stCommon80Data.usModelNumber >> 8) & 0xFF;         // High byte of Model Number
         break;
      case GLOBAL_PAGE_81:
         aucTxBuf[0] = ucPageNum;                 // Page number
         aucTxBuf[1] = WEIGHT_RESERVED;
         aucTxBuf[2] = WEIGHT_RESERVED;
         aucTxBuf[3] = stCommon81Data.ucSwVersion;                // Sw revision
         aucTxBuf[4] = stCommon81Data.ulSerialNumber & 0xFF;            // Serial number (bits 0-7)
         aucTxBuf[5] = (stCommon81Data.ulSerialNumber >> 8) & 0xFF;     // Serial number (bits 8-15)
         aucTxBuf[6] = (stCommon81Data.ulSerialNumber >> 16) & 0xFF;    // Serial number (bits 16-23)
         aucTxBuf[7] = (stCommon81Data.ulSerialNumber >> 24) & 0xFF;    // Serial number (bits 24-31)
         break;
   }

   if(ANT_Broadcast(ucAntChannel, aucTxBuf))
   {
      bTransmit = TRUE;
   }
   
   return(bTransmit);
}

////////////////////////////////////////////////////////////////////////////
// HandleReceiveMessages
//
// \return: N/A. 
////////////////////////////////////////////////////////////////////////////
BOOL HandleReceiveMessages(UCHAR* pucBuffer_)
{
   BOOL bTransmit = FALSE;
   BOOL bNoProfileExchange = TRUE;
   
   UCHAR ucPageNum = pucBuffer_[BUFFER_INDEX_MESG_DATA];
   
   if(ucPageNum == WEIGHT_PAGE_ANTFS_REQUEST)
   {
      if(!stUserProfileData.stCapabilities.stCapabilitiesBitField.bAntfs)
      {
         SetEvent(WGTTX_EVENT_ANTFS_REQUEST);
         return bTransmit;
      }
   }
   
   if(ucPageNum == WEIGHT_PAGE_USER_PROFILE) // Only decode user profile
   {
      DisableTimeout(); // got user profile, so we clear the timeout
      
      // Decode received profile
      stReceivedUserProfile.usID = (USHORT) pucBuffer_[BUFFER_INDEX_MESG_DATA + 1];            // Profile ID (low byte)
      stReceivedUserProfile.usID |= (((USHORT) pucBuffer_[BUFFER_INDEX_MESG_DATA + 2]) << 8);     // Profile ID (high byte)
      stReceivedUserProfile.stCapabilities.ucCapabilities = pucBuffer_[BUFFER_INDEX_MESG_DATA + 3];          // Capabilities
      stReceivedUserProfile.stUserInfo.ucAgeGender = pucBuffer_[BUFFER_INDEX_MESG_DATA + 5];     // Gender & Age
      stReceivedUserProfile.ucHeight = pucBuffer_[BUFFER_INDEX_MESG_DATA + 6];             // Height
      stReceivedUserProfile.stDescr.ucDescriptiveBitField = pucBuffer_[BUFFER_INDEX_MESG_DATA + 7]; // Descriptive bit field
   
      // Update display exchange support field as received from display
      bNoProfileExchange = stReceivedUserProfile.stCapabilities.stCapabilitiesBitField.bNoDisplayExchange;
      stUserProfileData.stCapabilities.stCapabilitiesBitField.bNoDisplayExchange = bNoProfileExchange;

      // Do both devices support profile exchange?
      if(!bNoProfileExchange && stUserProfileData.stCapabilities.stCapabilitiesBitField.bScaleExchange)
      {
         // Is user profile ID received from display valid?
         if(stReceivedUserProfile.usID != WEIGHT_PROFILE_UNDEF)
         {
            // User profile ID match?
            if(stUserProfileData.usID == stReceivedUserProfile.usID)
            {
               // Profile ID match: Check for changes to user profile for this ID
               if(!HasProfileChanged(&stReceivedUserProfile))
               {
                  ApplyProfile(&stReceivedUserProfile);
                  // Dispatch event to UI so that it can store the changed profile
                  SetEvent(WGTTX_EVENT_UPDATED_PROFILE);                  
               }               
            }
            else
            {
               ApplyProfile(&stReceivedUserProfile);
               // Dispatch event to UI so that it can store the new profile
               SetEvent(WGTTX_EVENT_NEW_PROFILE);
            }
            bTxUserData = TRUE;
            LockProfile();
         }
         else
         {
            // No valid user profile in display
            SetEvent(WGTTX_EVENT_INVALID_PROFILE);
                        
            // Is user profile set on scale?
            if((stUserProfileData.usID != WEIGHT_PROFILE_UNDEF) && (stUserProfileData.stCapabilities.stCapabilitiesBitField.bScaleSet == TRUE))
            {
               // Transmit Scale User Profile
               bTransmit = TRUE;
               bTxUserProfile = TRUE;
            }
            else
            {
               LockProfile();
            }
         }
      }
      else
      {
         LockProfile();
      }
   }
   else
   {
      // Received some other message
      SetEvent(WGTTX_EVENT_UNKNOWN_PAGE);
   }
   
   return(bTransmit);
}

/////////////////////////////////////////////////////////////////////////////////////////
// CheckForProfiles
// Initial set up of user profile lock timeout
/////////////////////////////////////////////////////////////////////////////////////////
void CheckForProfiles()
{
   // Scale UP Exchange?
   if(!stUserProfileData.stCapabilities.stCapabilitiesBitField.bScaleExchange)
   {
      LockProfile();
   }
   else
   {
      SetTimeout(WGT_UP_LOCK_TIMEOUT);
   }
}

/////////////////////////////////////////////////////////////////////////////////////////
// HasProfileChanged
// Compares a profile with the active profile on the scale
// 
// pstNewProfile: pointer to the new profile
// returns: TRUE if the profile has changed
/////////////////////////////////////////////////////////////////////////////////////////
BOOL HasProfileChanged(WGTPage_UserProfile* pstNewProfile)
{
   BOOL bComparison = FALSE;
   
   if(stUserProfileData.usID !=  pstNewProfile->usID)
      bComparison = TRUE;
      
   if(stUserProfileData.stCapabilities.ucCapabilities != pstNewProfile->stCapabilities.ucCapabilities)
      bComparison = TRUE;
      
   if(stUserProfileData.stUserInfo.ucAgeGender !=  pstNewProfile->stUserInfo.ucAgeGender)
      bComparison = TRUE;
      
   if(stUserProfileData.ucHeight != pstNewProfile->ucHeight)
      bComparison = TRUE;
      
   if(stUserProfileData.stDescr.ucDescriptiveBitField != pstNewProfile->stDescr.ucDescriptiveBitField)
      bComparison = TRUE;
   
   return bComparison;
}

/////////////////////////////////////////////////////////////////////////////////////////
// ApplyProfile
// Copies parameters of the user profile into the active profile
// 
// pstNewProfile: pointer to the new profile
/////////////////////////////////////////////////////////////////////////////////////////
void ApplyProfile(WGTPage_UserProfile* pstNewProfile)
{
   stUserProfileData.usID = pstNewProfile->usID;
   stUserProfileData.stCapabilities.ucCapabilities = pstNewProfile->stCapabilities.ucCapabilities;
   stUserProfileData.stUserInfo.ucAgeGender = pstNewProfile->stUserInfo.ucAgeGender;
   stUserProfileData.ucHeight = pstNewProfile->ucHeight;
   stUserProfileData.stDescr.ucDescriptiveBitField = pstNewProfile->stDescr.ucDescriptiveBitField;
   
   stUserProfileData.stCapabilities.stCapabilitiesBitField.bScaleSet  = TRUE;   
}

/////////////////////////////////////////////////////////////////////////
// LockProfile
// Locks the user profile, and sends an event to the UI so that it can
// allow the user to stand on the scale
/////////////////////////////////////////////////////////////////////////
void LockProfile()
{
   // Lock user profile
   bUserProfileLocked = TRUE;
   SetEvent(WGTTX_EVENT_USER_PROFILE_LOCKED);
}

/////////////////////////////////////////////////////////////////////////
// SetMeasurementsInvalid
/////////////////////////////////////////////////////////////////////////
void SetMeasurementsInvalid()
{
   stBodyWeightData.usBodyWeight100 = WEIGHT_INVALID;
   stBodyCompositionData.usHydrationP100 = WEIGHT_INVALID;
   stBodyCompositionData.usBodyFatP100 = WEIGHT_INVALID;
   stMetabolicInfoData.usActiveMetRate4 = WEIGHT_INVALID;
   stMetabolicInfoData.usBasalMetRate4 = WEIGHT_INVALID;
   stBodyMassData.usMuscleMass100 = WEIGHT_INVALID;
   stBodyMassData.ucBoneMass10 = WEIGHT_MASS_INVALID;
}

/////////////////////////////////////////////////////////////////////////
// SetMeasurementsComputing 
/////////////////////////////////////////////////////////////////////////
void SetMeasurementsComputing()
{
   stBodyWeightData.usBodyWeight100 = WEIGHT_COMPUTING;
   stBodyCompositionData.usHydrationP100 = WEIGHT_COMPUTING;
   stBodyCompositionData.usBodyFatP100 = WEIGHT_COMPUTING;
   stMetabolicInfoData.usActiveMetRate4 = WEIGHT_COMPUTING;
   stMetabolicInfoData.usBasalMetRate4 = WEIGHT_COMPUTING;
   stBodyMassData.usMuscleMass100 = WEIGHT_COMPUTING;
   stBodyMassData.ucBoneMass10 = WEIGHT_MASS_COMPUTING;
}


/////////////////////////////////////////////////////////////////////////
// SetEvent
// Adds an WGTTX event to the queue
// Returns TRUE if the event was successfully added, and FALSE if the 
// queue was full
/////////////////////////////////////////////////////////////////////////
BOOL SetEvent(UCHAR ucEventCode_)
{
   WGTTX_EVENT_RETURN* pstEvent = NULL;
   
   // Check if there is room in the queue for a new event
   if(((stTheEventQueue.ucHead+1) & (WGT_EVENT_QUEUE_SIZE -1)) != stTheEventQueue.ucTail)
      pstEvent = &(stTheEventQueue.astQueue[stTheEventQueue.ucHead]);

   if(pstEvent)
   {
      // Initialize event parameters
      pstEvent->eEvent = (WGTTX_EVENT) ucEventCode_;
      
      // Put the event in the queue
      stTheEventQueue.ucHead = ((stTheEventQueue.ucHead + 1) & (WGT_EVENT_QUEUE_SIZE - 1));
      return TRUE;
   }

   return FALSE;
}

/////////////////////////////////////////////////////////////////////////
// SetTimeout
// usTimeout: timeout, in seconds
/////////////////////////////////////////////////////////////////////////
BOOL SetTimeout(USHORT usTimeout)
{
   USHORT usTicks = usTimeout * ALARM_TIMER_PERIOD;
   if(Timer_Start(ucAlarmNumber, usTicks))
      return TRUE;
   
   return FALSE;
}

/////////////////////////////////////////////////////////////////////////
// Clear Timeout
/////////////////////////////////////////////////////////////////////////
void DisableTimeout()
{
   Timer_Stop(ucAlarmNumber);
}

/////////////////////////////////////////////////////////////////////////
// WGTTX_Tick
// usTimeStamp is time of event in 1/1024 s
/////////////////////////////////////////////////////////////////////////
void WGTTX_Tick(USHORT usTimeStamp_, UCHAR ucAlarmNumber_)      
{
   LockProfile(); 
}



