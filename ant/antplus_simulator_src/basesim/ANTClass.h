/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/


#pragma once
#include "StdAfx.h"
#include <stdio.h>
#include "ant_dll.h"
#include "types.h"
#include "config.h"

#define     USB_ARCT_PID ((USHORT) 0x1007)
#define     RESPONSE_TIME ((ULONG) 500)

//Port Types: these defines are used to decide what type of connection to connect over
#define PORT_TYPE_USB      0

//Framer Types: These are used to define which framing type to use
#define FRAMER_TYPE_BASIC            0

typedef void(*ANT_RESPONSE_FUNC)(UCHAR ucChannel_, UCHAR ucMessageCode_, UCHAR* pucBuffer_);
typedef void(*ANT_EVENT_RESPONSE_FUNC)(UCHAR ucChannel_, UCHAR ucEvent_, char* pcBuffer_);
public delegate void ChannelDelegate(UCHAR ucChannel_);

public ref class TimeoutStruct : System::Timers::Timer 
{
public:

   TimeoutStruct(ChannelDelegate^ pclChannelDelegate_);

   UCHAR ucChannel;
   UCHAR ucMessageID;
   UCHAR ucResponseID;
   BOOL bWaitingForResponse;
   ChannelDelegate^ ChannelCallback;   
   
   
   void ProcessTimerEvent(
      System::Object^  sender, 
      System::Timers::ElapsedEventArgs^ e);
      
      
   void 
   WaitForResponse( 
      UCHAR ucChannel_, 
      UCHAR ucResponseID_, 
      UCHAR ucMessageID_);

   void 
   ClearWait();
      
} ;

public ref class ANTClass
{
public:
   ANTClass(){};
   ~ANTClass(){};
   
   static BOOL 
   LibInit(
      ANT_EVENT_RESPONSE_FUNC pEventResponseFunc_);
      
   static BOOL 
   AssignChannelEventFunctions(
      int iChannels_, 
      ANT_RESPONSE_FUNC pResponseFunc_);
      
   static void 
   Disconnect();
   
   static const UCHAR*
   GetLibVersion();
   
   static BOOL 
   Init(
      UCHAR ucUSBPort_, 
      ULONG ulBaud_, 
      UCHAR ucPortType_, 
      UCHAR ucSerialFrameType_);
      
   static BOOL 
   GetDeviceUSBInfo(
      UCHAR ucUSBPort_, 
      UCHAR* pucPID_, 
      UCHAR* pucSerial_);
      
   static BOOL 
   GetDeviceUSBPID(
      USHORT* pusPID_);
   
   static void 
   Close();
   
   static void 
   UnAssignChannel(
      UCHAR ucChannel_);
      
   static void 
   AssignChannel(
      UCHAR ucChannel_, 
      UCHAR ucType_, 
      UCHAR ucNetworkNum_);
      
   static void 
   SetChannelId(
      UCHAR ucChannel_, 
      USHORT usDeviceNumber_, 
      UCHAR ucDeviceType_, 
      UCHAR ucTransmissionType_);
      
   static void 
   SetChannelPeriod(
      UCHAR ucChannel_, 
      USHORT usPeriod_);
      
   static void 
   SetChannelSearchTimeout(
      UCHAR ucChannel_, 
      UCHAR ucTimeout_);
      
   static void 
   SetChannelRFFreq(
      UCHAR ucChannel_, 
      UCHAR ucRFFreq_);
      
   static void 
   SetNetworkKey(
      UCHAR ucNetNum_, 
      UCHAR* pucKey_);
      
   static void 
   SetTransmitPower(
      UCHAR ucPower_);
   
   static void 
   SetChannelTxPower(
      UCHAR ucChannel_, 
      UCHAR ucPower_);

   static void 
   ResetSystem();
   
   static void 
   OpenChannel(
      UCHAR ucChannel_);
      
   static void 
   CloseChannel(
      UCHAR ucChannel_);
      
   static void 
   RequestMessage(
      UCHAR ucChannel_, 
      UCHAR ucMsgID_);

   static void
   WriteMessage(
      UCHAR ucMessageID, 
      UCHAR* aucData, 
      USHORT usMessageSize);
      
   static void 
   SendBroadcastData(
      UCHAR ucChannel_, 
      UCHAR* pucData_);
      
   static void 
   SendAcknowledgedData(
      UCHAR ucChannel_, 
      UCHAR* pucData_);
      
   static void 
   SendAcknowledgedData(
      UCHAR ucChannel_, 
      UCHAR* pucData_,
      USHORT usTimeout_);


   static void 
   SendBurstTransfer(
      UCHAR ucChannel_, 
      UCHAR* pucData_, 
      USHORT);
      
   
   static void 
   TimerEvent(
      UCHAR ucChannel_);

   static void 
   AckTimerEvent(UCHAR ucChannel_);

   static TimeoutStruct^ pclMyCommandTimeout;
   static array<TimeoutStruct^>^ paclMyAckTimeout; 
};


