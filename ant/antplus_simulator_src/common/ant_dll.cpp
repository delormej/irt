/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/
 

#include "stdafx.h"
#include "types.h"
#include "antdefines.h"
#include "antmessage.h"
#include "ant_dll.h"


//////////////////////////////////////////////////////////////////////////////////
// Public Variables
//////////////////////////////////////////////////////////////////////////////////

// Function Pointers
P_ANT_INIT                       ANT_Init;
P_ANT_GETDEVICEUSBINFO           ANT_GetDeviceUSBInfo;
P_ANT_GETDEVICEUSBPID            ANT_GetDeviceUSBPID;
P_ANT_CLOSE                      ANT_Close;
P_ANT_ARF                        ANT_AssignResponseFunction;
P_ANT_AEF                        ANT_AssignChannelEventFunction;
P_ANT_UNASSIGN                   ANT_UnAssignChannel;
P_ANT_ASSIGN                     ANT_AssignChannel;
P_ANT_SETID                      ANT_SetChannelId;
P_ANT_SETPER                     ANT_SetChannelPeriod;
P_ANT_SETTIME                    ANT_SetChannelSearchTimeout;
P_ANT_SETFREQ                    ANT_SetChannelRFFreq;
P_ANT_SETKEY                     ANT_SetNetworkKey;
P_ANT_SETPWR                     ANT_SetTransmitPower;
P_ANT_SETCHNLPWR                 ANT_SetChannelTxPower;
P_ANT_RST                        ANT_ResetSystem;
P_ANT_OPENCHNL                   ANT_OpenChannel;
P_ANT_CLOSECHNL                  ANT_CloseChannel;
P_ANT_REQMSG                     ANT_RequestMessage;
P_ANT_WRITEMSG                   ANT_WriteMessage;
P_ANT_TX                         ANT_SendBroadcastData;
P_ANT_TXA                        ANT_SendAcknowledgedData;
P_ANT_TXB                        ANT_SendBurstTransfer;
P_ANT_LIBVER					      ANT_LibVersion;

//////////////////////////////////////////////////////////////////////////////////
// Private Variables
//////////////////////////////////////////////////////////////////////////////////

static HMODULE hANTdll;
static BOOL bLoaded = FALSE;


//////////////////////////////////////////////////////////////////////////////////
// Public Functions
//////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
BOOL ANT_Load(void)
{
   if (bLoaded)
      return TRUE;




   hANTdll = LoadLibraryA("ANT_DLL.dll");


   if (hANTdll == NULL)
      return FALSE;

   ANT_Init = (P_ANT_INIT) GetProcAddress(hANTdll, "ANT_Init");
   ANT_GetDeviceUSBInfo = (P_ANT_GETDEVICEUSBINFO) GetProcAddress(hANTdll, "ANT_GetDeviceUSBInfo");
   ANT_GetDeviceUSBPID = (P_ANT_GETDEVICEUSBPID) GetProcAddress(hANTdll, "ANT_GetDeviceUSBPID");
   ANT_Close = (P_ANT_CLOSE) GetProcAddress(hANTdll, "ANT_Close");
   ANT_AssignResponseFunction = (P_ANT_ARF) GetProcAddress(hANTdll, "ANT_AssignResponseFunction");
   ANT_AssignChannelEventFunction = (P_ANT_AEF) GetProcAddress(hANTdll, "ANT_AssignChannelEventFunction");
   ANT_UnAssignChannel = (P_ANT_UNASSIGN) GetProcAddress(hANTdll, "ANT_UnAssignChannel_RTO");
   ANT_AssignChannel = (P_ANT_ASSIGN) GetProcAddress(hANTdll, "ANT_AssignChannel_RTO");
   ANT_WriteMessage = (P_ANT_WRITEMSG) GetProcAddress(hANTdll, "ANT_WriteMessage");
   ANT_SetChannelId =(P_ANT_SETID) GetProcAddress(hANTdll, "ANT_SetChannelId_RTO");
   ANT_SetChannelPeriod = (P_ANT_SETPER) GetProcAddress(hANTdll, "ANT_SetChannelPeriod_RTO");
   ANT_SetChannelSearchTimeout = (P_ANT_SETTIME) GetProcAddress(hANTdll, "ANT_SetChannelSearchTimeout_RTO");
   ANT_SetChannelRFFreq = (P_ANT_SETFREQ) GetProcAddress(hANTdll, "ANT_SetChannelRFFreq_RTO");
   ANT_SetNetworkKey = (P_ANT_SETKEY) GetProcAddress(hANTdll, "ANT_SetNetworkKey_RTO");
   ANT_SetTransmitPower = (P_ANT_SETPWR) GetProcAddress(hANTdll, "ANT_SetTransmitPower_RTO");
   ANT_SetChannelTxPower = (P_ANT_SETCHNLPWR) GetProcAddress(hANTdll, "ANT_SetChannelTxPower_RTO");
   ANT_ResetSystem = (P_ANT_RST) GetProcAddress(hANTdll, "ANT_ResetSystem");
   ANT_OpenChannel = (P_ANT_OPENCHNL) GetProcAddress(hANTdll, "ANT_OpenChannel");
   ANT_CloseChannel = (P_ANT_CLOSECHNL) GetProcAddress(hANTdll, "ANT_CloseChannel");
   ANT_RequestMessage = (P_ANT_REQMSG) GetProcAddress(hANTdll, "ANT_RequestMessage");
   ANT_SendBroadcastData = (P_ANT_TX) GetProcAddress(hANTdll, "ANT_SendBroadcastData");
   ANT_SendAcknowledgedData = (P_ANT_TXA) GetProcAddress(hANTdll, "ANT_SendAcknowledgedData");
   ANT_SendBurstTransfer = (P_ANT_TXB) GetProcAddress(hANTdll, "ANT_SendBurstTransfer");
   ANT_LibVersion = (P_ANT_LIBVER) GetProcAddress(hANTdll, "ANT_LibVersion");
   if (ANT_Init == NULL)
      return FALSE;
   if (ANT_GetDeviceUSBInfo == NULL)
      return FALSE;
   if (ANT_GetDeviceUSBPID == NULL)
      return FALSE;
   if (ANT_Close == NULL)
      return FALSE;
   if (ANT_AssignResponseFunction == NULL)
      return FALSE;
   if (ANT_AssignChannelEventFunction == NULL)
      return FALSE;
   if (ANT_UnAssignChannel == NULL)
      return FALSE;
   if (ANT_AssignChannel == NULL)
      return FALSE;
   if (ANT_WriteMessage == NULL)
      return FALSE;
   if (ANT_SetChannelId == NULL)
      return FALSE;
   if (ANT_SetChannelPeriod == NULL)
      return FALSE;
   if (ANT_SetChannelSearchTimeout == NULL)
      return FALSE;
   if (ANT_SetChannelRFFreq == NULL)
      return FALSE;
   if (ANT_SetNetworkKey == NULL)
      return FALSE;
   if (ANT_SetTransmitPower == NULL)
      return FALSE;
   if (ANT_SetChannelTxPower == NULL)
      return FALSE;
   if (ANT_ResetSystem == NULL)
      return FALSE;
   if (ANT_OpenChannel == NULL)
      return FALSE;
   if (ANT_CloseChannel == NULL)
      return FALSE;
   if (ANT_RequestMessage == NULL)
      return FALSE;
   if (ANT_SendBroadcastData == NULL)
      return FALSE;
   if (ANT_SendAcknowledgedData == NULL)
      return FALSE;
   if (ANT_SendBurstTransfer == NULL)
      return FALSE;
   bLoaded = TRUE;
   return TRUE;
}

///////////////////////////////////////////////////////////////////////
void ANT_UnLoad(void)
{
   if (hANTdll != NULL)
   {
      FreeLibrary(hANTdll);
   }
   bLoaded = FALSE;
}

///////////////////////////////////////////////////////////////////////
BOOL ANT_LibVersionSupport(void)
{
	BOOL bLibVer = FALSE;

	if(hANTdll != NULL)
	{
		if(ANT_LibVersion != NULL)
			bLibVer = TRUE;
	}

	return bLibVer;
}
