/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/
 

#ifndef ANT_DLL_H
#define ANT_DLL_H

#include "types.h"

#include "antdefines.h"
#include "antmessage.h"


#if defined(__cplusplus)
   extern "C" {
#endif


//////////////////////////////////////////////////////////////////////////////////
// Public Definitions
//////////////////////////////////////////////////////////////////////////////////

// Application callback function pointers
typedef BOOL (*RESPONSE_FUNC)(UCHAR ucChannel, UCHAR ucResponseMsgID);
typedef BOOL (*CHANNEL_EVENT_FUNC)(UCHAR ucChannel, UCHAR ucEvent);
typedef BOOL (*P_ANT_INIT)(UCHAR, ULONG, UCHAR, UCHAR);
typedef BOOL (*P_ANT_GETDEVICEUSBINFO)(UCHAR ,UCHAR*, UCHAR*);
typedef BOOL (*P_ANT_GETDEVICEUSBPID)(USHORT* pusPID_);
typedef void (*P_ANT_CLOSE)();
typedef void (*P_ANT_ARF)(RESPONSE_FUNC, UCHAR*);
typedef void (*P_ANT_AEF)(UCHAR, CHANNEL_EVENT_FUNC, UCHAR*);
typedef BOOL (*P_ANT_UNASSIGN)(UCHAR, ULONG);
typedef BOOL (*P_ANT_ASSIGN)(UCHAR, UCHAR, UCHAR, ULONG);
typedef BOOL (*P_ANT_SETID)(UCHAR, USHORT, UCHAR, UCHAR, ULONG);
typedef BOOL (*P_ANT_SETPER)(UCHAR, USHORT, ULONG);
typedef BOOL (*P_ANT_SETTIME)(UCHAR, UCHAR, ULONG);
typedef BOOL (*P_ANT_SETFREQ)(UCHAR, UCHAR, ULONG);
typedef BOOL (*P_ANT_SETKEY)(UCHAR, UCHAR*, ULONG);
typedef BOOL (*P_ANT_SETPWR)(UCHAR, ULONG);
typedef BOOL (*P_ANT_SETCHNLPWR)(UCHAR, UCHAR, ULONG);
typedef void (*P_ANT_RST)(void);
typedef void (*P_ANT_OPENCHNL)(UCHAR);
typedef void (*P_ANT_CLOSECHNL)(UCHAR);
typedef void (*P_ANT_REQMSG)(UCHAR, UCHAR);
typedef BOOL (*P_ANT_WRITEMSG)(UCHAR ucMessageID, UCHAR* aucData, USHORT usMessageSize);
typedef void (*P_ANT_TX)(UCHAR, UCHAR*);
typedef void (*P_ANT_TXA)(UCHAR, UCHAR*);
typedef void (*P_ANT_TXB)(UCHAR, UCHAR*, USHORT);
typedef const UCHAR* (*P_ANT_LIBVER)(void);



//////////////////////////////////////////////////////////////////////////////////
// Public Variable Prototypes
//////////////////////////////////////////////////////////////////////////////////

// Function Pointers
extern P_ANT_INIT                      ANT_Init;
extern P_ANT_GETDEVICEUSBINFO          ANT_GetDeviceUSBInfo;
extern P_ANT_GETDEVICEUSBPID           ANT_GetDeviceUSBPID;
extern P_ANT_CLOSE                     ANT_Close;
extern P_ANT_ARF                       ANT_AssignResponseFunction;
extern P_ANT_AEF                       ANT_AssignChannelEventFunction;
extern P_ANT_UNASSIGN                  ANT_UnAssignChannel;
extern P_ANT_ASSIGN                    ANT_AssignChannel;
extern P_ANT_SETID                     ANT_SetChannelId;
extern P_ANT_SETPER                    ANT_SetChannelPeriod;
extern P_ANT_SETTIME                   ANT_SetChannelSearchTimeout;
extern P_ANT_SETFREQ                   ANT_SetChannelRFFreq;
extern P_ANT_SETKEY                    ANT_SetNetworkKey;
extern P_ANT_SETPWR                    ANT_SetTransmitPower;
extern P_ANT_SETCHNLPWR                ANT_SetChannelTxPower;
extern P_ANT_RST                       ANT_ResetSystem;
extern P_ANT_OPENCHNL                  ANT_OpenChannel;
extern P_ANT_CLOSECHNL                 ANT_CloseChannel;
extern P_ANT_REQMSG                    ANT_RequestMessage;
extern P_ANT_WRITEMSG                  ANT_WriteMessage;
extern P_ANT_TX                        ANT_SendBroadcastData;
extern P_ANT_TXA                       ANT_SendAcknowledgedData;
extern P_ANT_TXB                       ANT_SendBurstTransfer;
extern P_ANT_LIBVER					   ANT_LibVersion;

//////////////////////////////////////////////////////////////////////////////////
// Public Function Prototypes
//////////////////////////////////////////////////////////////////////////////////

extern BOOL ANT_Load(void);
extern void ANT_UnLoad(void);
extern BOOL ANT_LibVersionSupport();

#if defined(__cplusplus)
   }
#endif

#endif // !ANT_DLL_H
