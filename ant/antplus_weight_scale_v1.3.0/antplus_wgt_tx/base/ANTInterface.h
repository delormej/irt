/*
   Copyright 2012 Dynastream Innovations, Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#ifndef __ANTINTERFACE_H__
#define __ANTINTERFACE_H__

#include "types.h"
#include "antmessage.h"

typedef struct
{
   UCHAR ucControl;
   UCHAR aucData[ANT_DATA_SIZE];
} ANT_MESG_STRUCT;

typedef union
{
   ANT_MESG_STRUCT stMessageStruct;
   UCHAR aucMesg[sizeof(ANT_MESG_STRUCT)];
} ANT_MESG;

void 
ANTInterface_Init();

UCHAR* 
ANTInterface_Transaction();

BOOL 
ANT_Reset();

BOOL 
ANT_UnAssignChannel(
   UCHAR ucChannelNumber_);

BOOL 
ANT_AssignChannel(
   UCHAR ucChannelNumber_, 
   UCHAR ucChannelType_, 
   UCHAR ucNetworkNumber_);

BOOL 
ANT_SearchTimeout(
   UCHAR ucChannelNumber_, 
   UCHAR ucTimeout_);


BOOL 
ANT_NetworkKey(
   UCHAR ucNetworkNumber_, 
   const UCHAR* pucKey_);
   
BOOL 
ANT_ChannelId(
   UCHAR ucANTChannel_, 
   USHORT usDeviceNumber_, 
   UCHAR ucDeviceType_, 
   UCHAR ucTransmitType_);
   
BOOL 
ANT_ChannelPower(
   UCHAR ucANTChannel_, 
   UCHAR ucPower_);
   
BOOL 
ANT_ChannelRFFreq(
   UCHAR ucANTChannel_,
   UCHAR ucFreq_);
   
BOOL 
ANT_ChannelPeriod(
   UCHAR ucANTChannel_, 
   USHORT usPeriod_);
   
BOOL 
ANT_Broadcast(
   UCHAR ucANTChannel_, 
   UCHAR* pucBuffer_);
      
BOOL 
ANT_AcknowledgedTimeout(
   UCHAR ucChannel_, 
   UCHAR* pucData_, 
   USHORT usTimeout_);
   
BOOL 
ANT_Acknowledged(
   UCHAR ucANTChannel_, 
   UCHAR* pucBuffer_);
   
BOOL 
ANT_BurstPacket(
   UCHAR ucControl_, 
   UCHAR* pucBuffer_);
   
USHORT 
ANT_SendBurstTransfer(
   UCHAR ucAntChannel_, 
   UCHAR* pucBuffer_, 
   USHORT usPackets_);
   
USHORT
ANT_SendPartialBurst(
   UCHAR ucAntChannel_,
   UCHAR* pucBuffer_,
   USHORT usPackets_,
   ULONG ulInitialPacket_,
   BOOL bIncludeLast_);

BOOL 
ANT_OpenChannel(
   UCHAR ucANTChannel_);
   
BOOL 
ANT_CloseChannel(
   UCHAR ucANTChannel_);
   
BOOL 
ANT_RequestMessage(
   UCHAR ucANTChannel_, 
   UCHAR ucRequestedMessage_);
   
BOOL 
ANT_RunScript(
   UCHAR ucPageNum_);
   
   
#endif
