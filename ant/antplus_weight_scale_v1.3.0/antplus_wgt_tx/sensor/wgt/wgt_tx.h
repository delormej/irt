/*
This software is subject to the license described in the License.txt file
included with this software distribution. You may not use this file except in compliance
with this license.

Copyright (c) Dynastream Innovations Inc. 2013
All rights reserved.
*/

#ifndef WGT_TX_H_
#define WGT_TX_H_

#include "config.h"
#include "types.h"
#include "antinterface.h"
#include "antplus.h"  
#include "wgt_pages.h" 

///////////////////////////////
// Manufacturer Specific Settings
///////////////////////////////
#define WGTTX_MFG_ID                        ((UCHAR) 1)              // Manufacturer ID
#define WGTTX_SERIAL_NUMBER                 ((ULONG) 0x00ABCDEF)     // Serial Number
#define WGTTX_HW_VERSION                    ((UCHAR) 5)              // HW Version
#define WGTTX_SW_VERSION                    ((UCHAR) 1)              // SW Version
#define WGTTX_MODEL_NUMBER                  ((UCHAR) 67)             // Model Number

///////////////////////////////
// Weight Scale Device Profile
///////////////////////////////

#define WEIGHT_TX_CHANNEL_TYPE            CHANNEL_TYPE_MASTER
#define WEIGHT_DEVICE_TYPE                ((UCHAR) 0x77)
#define WEIGHT_TX_TYPE                    ((UCHAR) 0x05)
#define WEIGHT_MSG_PERIOD                 ((USHORT) 8192) // 4Hz

// Body weight
#define WEIGHT_RESERVED                   ((UCHAR) 0xFF)
#define WEIGHT_INVALID                    ((USHORT) 0xFFFF)
#define WEIGHT_COMPUTING                  ((USHORT) 0xFFFE)
#define WEIGHT_MASS_INVALID               ((UCHAR) 0xFF)
#define WEIGHT_MASS_COMPUTING             ((UCHAR) 0xFE)

// Interleaving requirements
#define WEIGHT_INTERVAL_COMMON            ((UCHAR) 21)  // Pages 80, 81 required every 21 messages

///////////////////////////////
// User Profile Definitions
///////////////////////////////
#define WEIGHT_PROFILE_UNDEF              ((USHORT) 0xFFFF)
#define WEIGHT_PROFILE_SCALE_MIN          ((USHORT) 0x0000)
#define WEIGHT_PROFILE_SCALE_MAX          ((USHORT) 0x000F)
#define WEIGHT_PROFILE_DISPLAY_STAT_MIN   ((USHORT) 0x0010)
#define WEIGHT_PROFILE_DISPLAY_STAT_MAX   ((USHORT) 0x00FF)
#define WEIGHT_PROFILE_DISPLAY_PERS_MIN   ((USHORT) 0x0100)
#define WEIGHT_PROFILE_DISPLAY_PERS_MAX   ((USHORT) 0xFFFE)
#define WEIGHT_PROFILE_DISPLAY_MIN        ((USHORT) 0x0010)
#define WEIGHT_PROFILE_DISPLAY_MAX        ((USHORT) 0xFFFE)

#define WEIGHT_GENDER_SHIFT               ((UCHAR) 7)
#define WEIGHT_GENDER_FEMALE              ((UCHAR) 0x00)
#define WEIGHT_GENDER_MALE                ((UCHAR) 0x01)
#define WEIGHT_GENDER_UNDEF               ((UCHAR) 0x00)
#define WEIGHT_AGE_MASK                   ((UCHAR) 0x7F)
#define WEIGHT_AGE_UNDEF                  ((UCHAR) 0x00)
#define WEIGHT_HEIGHT_UNDEF               ((UCHAR) 0x00)
#define WEIGHT_IS_ATHLETE                 (TRUE)
#define WEIGHT_NOT_ATHLETE                (FALSE)

// Descriptive Bit Field
#define WEIGHT_DESCR_UNDEF                ((UCHAR) 0x00)
#define WEIGHT_DESCR_RESERVED             ((UCHAR) 0x00)
#define WEIGHT_DESCR_MASK                 ((UCHAR) 0x07)
#define WEIGHT_ACTIVITY_ATHLETE_MASK      ((UCHAR) 0x080
#define WEIGHT_ACTIVITY_ATHLETE_SHIFT     ((UCHAR) 7)
#define WEIGHT_ACTIVITY_SHIFT             ((UCHAR) 0)
#define WEIGHT_ACTIVITY_RESERVED          ((UCHAR) 0x07 << ACTIVITY_SHIFT)

// Priorities
#define WEIGHT_PRIORITY_SCALE             ((UCHAR) 1)
#define WEIGHT_PRIORITY_DISPLAY           ((UCHAR) 2)
#define WEIGHT_PRIORITY_WATCH             ((UCHAR) 3)
#define WEIGHT_PRIORITY_UNDEF             ((UCHAR) 0) 

///////////////////////////////
// Capabilities Definitions
///////////////////////////////

#define WEIGHT_CAPAB_RESERVED             ((UCHAR) 0x00)

#define WEIGHT_SCALE_SELECTED_SHIFT       ((UCHAR) 0)   // bit 0
#define WEIGHT_SCALE_SELECTED             ((UCHAR) 0x01 << SCALE_SELECTED_SHIFT)
#define WEIGHT_SCALE_UNSELECTED           ((UCHAR) 0x00 << SCALE_SELECTED_SHIFT)
#define WEIGHT_SCALE_SELECTED_MASK        ((UCHAR) 0x01)

#define WEIGHT_SCALE_EXCHANGE_SHIFT       ((UCHAR) 1)
#define WEIGHT_SCALE_EXCHANGE             ((UCHAR) 0x01 << SCALE_EXCHANGE_SHIFT)
#define WEIGHT_SCALE_NO_EXCHANGE          ((UCHAR) 0x00 << SCALE_EXCHANGE_SHIFT)
#define WEIGHT_SCALE_EXCHANGE_MASK        ((UCHAR) 0x02)

#define WEIGHT_ANTFS_SHIFT                ((UCHAR) 2)          // bit 2
#define WEIGHT_ANTFS                      ((UCHAR) 0x01 << ANTFS_SHIFT)
#define WEIGHT_NO_ANTFS                   ((UCHAR) 0x00 << ANTFS_SHIFT)
#define WEIGHT_ANTFS_MASK                 ((UCHAR) 0x04)

#define WEIGHT_DISPLAY_EXCHANGE_SHIFT     ((UCHAR) 7)     // bit 7
#define WEIGHT_DISPLAY_EXCHANGE           ((UCHAR) 0x00 << DISPLAY_EXCHANGE_SHIFT)  
#define WEIGHT_DISPLAY_NO_EXCHANGE        ((UCHAR) 0x01 << DISPLAY_EXCHANGE_SHIFT)
#define WEIGHT_DISPLAY_EXCHANGE_MASK      ((UCHAR) 0x80)

///////////////////////////////
// Public enums
///////////////////////////////

typedef enum
{
   WGTTX_EVENT_NONE,
   WGTTX_EVENT_NEW_PROFILE,
   WGTTX_EVENT_UPDATED_PROFILE,
   WGTTX_EVENT_INVALID_PROFILE,
   WGTTX_EVENT_USER_PROFILE_LOCKED,
   WGTTX_EVENT_UNKNOWN_PAGE,
   WGTTX_EVENT_SENT_PROFILE,
   WGTTX_EVENT_ANTFS_REQUEST
} WGTTX_EVENT;

typedef struct
{
   WGTTX_EVENT eEvent;                 // Event
} WGTTX_EVENT_RETURN;

///////////////////////////////
// Public functions
///////////////////////////////
BOOL                                         // TRUE if commands queued succesfully 
WGTTX_Open(                               
   UCHAR ucAntChannel_,                      // ANT Channel number
   USHORT usDeviceNumber_,                   // ANT Channel device number
   UCHAR ucTransType_);                      // ANT Channel transmission type
   
BOOL                                         // TRUE if command queued succesfully
WGTTX_Close();                         

void 
WGTTX_CalculateWeight();

BOOL                                         // TRUE if commands queued to transmit (don't sleep transmit thread)
WGTTX_ChannelEvent(
   UCHAR* pucEventBuffer_);                  // Pointer to buffer containing response from ANT 
   
WGTPage_UserProfile*                         // Pointer to user profile struct received from another device
WGTTX_GetReceivedUserProfile();

BOOL                                         // TRUE if there is an event
WGTTX_CheckEvents(
   WGTTX_EVENT_RETURN* pstEventStruct);      // Pointer to the event struct to read from

#endif /*WGT_TX_H_*/
