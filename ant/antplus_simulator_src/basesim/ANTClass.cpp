/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/

#include "stdafx.h"
#include "ANTClass.h"
#include "antplus.h"

static UCHAR aucResponseBuf[10];
static UCHAR aucRxBuf[17];
static volatile ANT_RESPONSE_FUNC pResponseFunc = NULL;
static volatile ANT_EVENT_RESPONSE_FUNC pEventResponseFunc = NULL;

/**************************************************************************
 * ANTProtocolEventProcess
 * 
 * Callback function for ANT protocol events
 *
 * ucChannel_: channel number
 * ucMessageCode_: message code received from ANT
 *
 * returns: N/A
 *
 **************************************************************************/
void ANTProtocolEventProcess(UCHAR ucChannel_, UCHAR ucMessageCode_)
{

   if(ANTClass::pclMyCommandTimeout->bWaitingForResponse)
   {
   
      if(ucMessageCode_ == MESG_RESPONSE_EVENT_ID)
      {
         if(aucResponseBuf[1] == ANTClass::pclMyCommandTimeout->ucMessageID)
         {
               ANTClass::pclMyCommandTimeout->ClearWait();
         }
      }
      else if(ucMessageCode_ == ANTClass::pclMyCommandTimeout->ucResponseID)
      {
         ANTClass::pclMyCommandTimeout->ClearWait();
      }
      
   }
      
      
   // Pass onto application level
   System::Diagnostics::Debug::Assert(pEventResponseFunc != NULL);
   pEventResponseFunc(ucChannel_, ucMessageCode_, (char*)&aucResponseBuf[0]);
}

/**************************************************************************
 * ANTChannelEventProcess
 * 
 * Callback function for ANT channel events
 *
 * ucChannel_: ANT channel
 * ucMessageCode_: message code returned from ANT
 *
 * returns: N/A
 *
 **************************************************************************/
void ANTChannelEventProcess(UCHAR ucChannel_, UCHAR ucMessageCode_)
{

   
   if(ANTClass::paclMyAckTimeout[ucChannel_]->bWaitingForResponse)
   {
   
   
	   switch(ucMessageCode_)
	   {   
		   case EVENT_TRANSFER_TX_COMPLETED:
		   case EVENT_TRANSFER_TX_FAILED:
		   case EVENT_CHANNEL_CLOSED:
         case CHANNEL_NOT_OPENED:
         case TRANSFER_IN_PROGRESS:
         {
            ANTClass::paclMyAckTimeout[ucChannel_]->ClearWait();
            break;
         }
         default:
            break;
      } 
   
      
   }
   


   pResponseFunc(ucChannel_, ucMessageCode_, &aucRxBuf[0]);

}





/**************************************************************************
 * LibInit
 * 
 * Initializes ANT DLL and assigns response callback function
 *
 * pEventResponseFunc_: Pointer to ANT Response handler function
 *
 * returns: TRUE if ANT DLL loaded and initialized correctly. FALSE otherwise.
 *
 **************************************************************************/
BOOL ANTClass::LibInit(ANT_EVENT_RESPONSE_FUNC pEventResponseFunc_)
{
	if (pEventResponseFunc_ == NULL)
		return FALSE;

	pEventResponseFunc = pEventResponseFunc_;

   // Try loading the ANT DLL
	if (!ANT_Load())
		return FALSE;
      

	// Assign callback function
   ANT_AssignResponseFunction( (RESPONSE_FUNC) ANTProtocolEventProcess, aucResponseBuf);

   // Initialize the timers
   pclMyCommandTimeout = gcnew TimeoutStruct(gcnew ChannelDelegate(&ANTClass::TimerEvent));
   pclMyCommandTimeout->Interval = 500;

   pclMyCommandTimeout->Elapsed += gcnew System::Timers::ElapsedEventHandler(pclMyCommandTimeout, &TimeoutStruct::ProcessTimerEvent);

  
  
	return(TRUE);

}

/**************************************************************************
 * AssignChannelEventFunctions
 * 
 * Assigns a channel event function to required number of channels. 
 *
 * pResponseFunc_: Pointer to channl event handler functiion.
 *
 * returns: TRUE if input parameters make sense.
 *
 **************************************************************************/
BOOL ANTClass::AssignChannelEventFunctions(int iChannels_, ANT_RESPONSE_FUNC pResponseFunc_)
{
   int i;
	if (pResponseFunc_ == NULL)
		return FALSE;         
      

   paclMyAckTimeout = gcnew array<TimeoutStruct^, 1>(iChannels_);
	
   pResponseFunc = pResponseFunc_;
   
	//After the capabilities return we know how many channels are supported and can assign the channel response function to each
	for(i=0; i < iChannels_; ++i)
   {
      paclMyAckTimeout[i] = gcnew TimeoutStruct(gcnew ChannelDelegate(&ANTClass::AckTimerEvent));
      paclMyAckTimeout[i]->Elapsed += gcnew System::Timers::ElapsedEventHandler(paclMyAckTimeout[i], &TimeoutStruct::ProcessTimerEvent);
      ANT_AssignChannelEventFunction((UCHAR)i, (CHANNEL_EVENT_FUNC) ANTChannelEventProcess, aucRxBuf);   
   }
   return(TRUE);
}



/**************************************************************************
 * Disconnect
 * 
 * Disconnects from the ANT USB stick by restting the ANT chip and unloading
 * the ANT DLL. 
 *
 * returns: N/A.
 *
 **************************************************************************/
void ANTClass::Disconnect()
{
	ANT_ResetSystem();
	Sleep(600);
	ANT_Close();
   
}

/**************************************************************************
 * GetLibVersion
 * 
 * Gets ANT Library (ANT DLL) version
 *
 * returns: pointer to a string of characters representing the DLL version, 
 *          if available
 *
 **************************************************************************/   
const UCHAR* ANTClass::GetLibVersion()
{
	if(ANT_LibVersionSupport())
	{
		return (ANT_LibVersion());	// Get version, if supported
	}
	else
		return nullptr;	// Version string not supported or DLL not yet loaded
}

/**************************************************************************
 * Init
 * 
 * Tries to connect to the USB stick. 
 *
 * Params:
 * 
 * ucUSBPort_: USB Port
 * usBaud_: Baud rate to connect at
 * returns: N/A.
 *
 **************************************************************************/
BOOL ANTClass::Init(UCHAR ucUSBPort_, ULONG ulBaud_, UCHAR ucPortType_, UCHAR ucSerialFrameType_)
{
   return(ANT_Init(ucUSBPort_, ulBaud_, ucPortType_, ucSerialFrameType_));
}

/**************************************************************************
 * GetDeviceUSBInfo
 * 
 * Returns USB info about the USB stick. 
 *
 * Params:
 * 
 * ucUSBPort_: USB Port
 * pucStringDesc_: String descriptor
 * pucSerial_: Serial number of device.
 * returns: N/A.
 *
 **************************************************************************/
BOOL ANTClass::GetDeviceUSBInfo(UCHAR ucUSBPort_, UCHAR* pucStringDesc_, UCHAR* pucSerial_)
{
   return(ANT_GetDeviceUSBInfo(ucUSBPort_,pucStringDesc_, pucSerial_));
}

/**************************************************************************
 * GetDeviceUSBPID
 * 
 * Returns USB PID of connected USB device. . 
 *
 * Params:
 * 
 * pusPID_: PID word
 *
 * returns: TRUE if device is connected. FALSE otehrwise.
 *
 **************************************************************************/
BOOL ANTClass::GetDeviceUSBPID(USHORT* pusPID_)
{
   return(ANT_GetDeviceUSBPID(pusPID_));
}



/**************************************************************************
 * Close
 * 
 * Unloads the ANT DLL. 
 *
 * Params:
 * 
 * returns: N/A.
 *
 **************************************************************************/
void ANTClass::Close()
{
   ANT_Close();
}


/**************************************************************************
 * UnAssignChannel
 * 
 * Unassigns ANT channel. 
 * Sets state variables for response or timeout. 
 *
 * Params:
 *
 * ucChannel_: ANT channel number
 * 
 * returns: Returns right aways (does not blowck waiting for response)
 *
 **************************************************************************/
void ANTClass::UnAssignChannel(UCHAR ucChannel_)
{
   if(!ANT_UnAssignChannel(ucChannel_, RESPONSE_TIME)) 
      System::Diagnostics::Debug::Assert(FALSE, "Command sent before response or timeout recieved");
}  
/**************************************************************************
 * AssignChannel
 * 
 * Assigns an ANT channel. 
 * Sets state variables for response or timeout. 
 *
 * Params:
 *
 * ucChannel_: ANT channel number
 * ucType_: Channel type
 * ucNetworkNum_: Network number
 * 
 * returns: Returns right aways (does not blowck waiting for response)
 *
 **************************************************************************/
void ANTClass::AssignChannel(UCHAR ucChannel_, UCHAR ucType_, UCHAR ucNetworkNum_)
{
   if(!ANT_AssignChannel(ucChannel_, ucType_, ucNetworkNum_, RESPONSE_TIME)) 
      System::Diagnostics::Debug::Assert(FALSE, "Command sent before response or timeout recieved");
} 
/**************************************************************************
 * SetChannelId
 * 
 * Sets ANT channel ID. 
 * Sets state variables for response or timeout. 
 *
 * Params:
 *
 * ucChannel_: ANT channel number
 * usDeviceNumber_: Device number
 * ucDeviceType_: Device type
 * ucTransmissionType_: Transmission type
 * 
 * returns: Returns right aways (does not blowck waiting for response)
 *
 **************************************************************************/
void ANTClass::SetChannelId(UCHAR ucChannel_, USHORT usDeviceNumber_, UCHAR ucDeviceType_, UCHAR ucTransmissionType_)
{
   if(!ANT_SetChannelId(ucChannel_, usDeviceNumber_, ucDeviceType_, ucTransmissionType_, RESPONSE_TIME))
      System::Diagnostics::Debug::Assert(FALSE, "Command sent before response or timeout recieved");
}
/**************************************************************************
 * SetChannelPeriod
 * 
 * Sets ANT channel message period. 
 * Sets state variables for response or timeout. 
 *
 * Params:
 *
 * ucChannel_: ANT channel number
 * usPeriod_: Message period
 * 
 * returns: Returns right aways (does not blowck waiting for response)
 *
 **************************************************************************/  
void ANTClass::SetChannelPeriod(UCHAR ucChannel_, USHORT usPeriod_)
{
   if(!ANT_SetChannelPeriod(ucChannel_, usPeriod_, RESPONSE_TIME))
      System::Diagnostics::Debug::Assert(FALSE, "Command sent before response or timeout recieved");
}
/**************************************************************************
 * SetChannelSearchTimeout
 * 
 * Sets ANT channel search timeout. 
 * Sets state variables for response or timeout. 
 *
 * Params:
 *
 * ucChannel_: ANT channel number
 * ucTimeout_: Search timeout
 * 
 * returns: Returns right aways (does not blowck waiting for response)
 *
 **************************************************************************/    
void ANTClass::SetChannelSearchTimeout(UCHAR ucChannel_, UCHAR ucTimeout_)
{
   if(!ANT_SetChannelSearchTimeout(ucChannel_, ucTimeout_, RESPONSE_TIME))
      System::Diagnostics::Debug::Assert(FALSE, "Command sent before response or timeout recieved");
}
/**************************************************************************
 * SetChannelRFFreq
 * 
 * Sets ANT channel RF frequency. 
 * Sets state variables for response or timeout. 
 *
 * Params:
 *
 * ucChannel_: ANT channel number
 * ucRFFreq_: RF frequency
 * 
 * returns: Returns right aways (does not blowck waiting for response)
 *
 **************************************************************************/      
void ANTClass::SetChannelRFFreq(UCHAR ucChannel_, UCHAR ucRFFreq_)
{
   if(!ANT_SetChannelRFFreq(ucChannel_, ucRFFreq_, RESPONSE_TIME))
      System::Diagnostics::Debug::Assert(FALSE, "Command sent before response or timeout recieved");
}   
/**************************************************************************
 * SetNetworkKey
 * 
 * Sets network key. 
 * Sets state variables for response or timeout. 
 *
 * Params:
 *
 * ucChannel_: ANT channel number
 * pucKey_: Pointer to buffer containing network key
 * 
 * returns: Returns right aways (does not blowck waiting for response)
 *
 **************************************************************************/      
void ANTClass::SetNetworkKey(UCHAR ucNetNum_, UCHAR* pucKey_)
{
   if(!ANT_SetNetworkKey(ucNetNum_, pucKey_, RESPONSE_TIME))
      System::Windows::Forms::MessageBox::Show("Failed to set network key. Ensure device is alive and you are connected at the correct baud rate.");
} 
/**************************************************************************
 * SetTransmitPower
 * 
 * Sets transmit power. 
 * Sets state variables for response or timeout. 
 *
 * Params:
 *
 * ucPower_: Transmit power
 * 
 * returns: Returns right aways (does not blowck waiting for response)
 *
 **************************************************************************/      
void ANTClass::SetTransmitPower(UCHAR ucPower_)
{
   if(!ANT_SetTransmitPower(ucPower_, RESPONSE_TIME))
      System::Diagnostics::Debug::Assert(FALSE, "Command sent before response or timeout recieved");
}
/**************************************************************************
 * SetChannelTxPower
 * 
 * Sets transmit power. 
 * Sets state variables for response or timeout. 
 *
 * Params:
 *
 * ucChannel_: ANT channel
 * ucPower_: Transmit power
 * 
 * returns: Returns right aways (does not block waiting for response)
 *
 **************************************************************************/      
void ANTClass::SetChannelTxPower(UCHAR ucChannel_, UCHAR ucPower_)
{
   if(!ANT_SetChannelTxPower(ucChannel_, ucPower_, RESPONSE_TIME))
      System::Diagnostics::Debug::Assert(FALSE, "Command sent before response or timeout recieved");
}
/**************************************************************************
 * ResetSystem
 * 
 * Resets ANT and delays appropriate amount of time before returning. 
 *
 * Params:
 *
 * ucPower_: Transmit power
 * 
 * returns: Returns after ~600ms
 *
 **************************************************************************/      
void ANTClass::ResetSystem()
{
   ANT_ResetSystem();
   Sleep(600);
}

/**************************************************************************
 * OpenChannel
 * 
 * Opens ANT channel. 
 * Sets state variables for response or timeout. 
 *
 * Params:
 *
 * ucChannel_: ANT channel
 * 
 * returns: Returns right aways (does not blowck waiting for response)
 *
 **************************************************************************/      
void ANTClass::OpenChannel(UCHAR ucChannel_)
{
   if(!pclMyCommandTimeout->bWaitingForResponse)
   {
      ANT_OpenChannel(ucChannel_);
      pclMyCommandTimeout->WaitForResponse(ucChannel_, MESG_RESPONSE_EVENT_ID, MESG_OPEN_CHANNEL_ID);
   }
   else
   {
      System::Diagnostics::Debug::Assert(FALSE, "Command sent before response or timeout recieved");
   }    
   
}

/**************************************************************************
 * CloseChannel
 * 
 * Closes ANT channel. 
 * Sets state variables for response or timeout. 
 *
 * Params:
 *
 * ucChannel_: ANT channel
 * 
 * returns: Returns right aways (does not blowck waiting for response)
 *
 **************************************************************************/      
void ANTClass::CloseChannel(UCHAR ucChannel_)
{
   if(!pclMyCommandTimeout->bWaitingForResponse)
   {
      ANT_CloseChannel(ucChannel_);
      pclMyCommandTimeout->WaitForResponse(ucChannel_, MESG_RESPONSE_EVENT_ID, MESG_CLOSE_CHANNEL_ID);
   }
   else
   {
      System::Diagnostics::Debug::Assert(FALSE, "Command sent before response or timeout recieved");
   }    
}
/**************************************************************************
 * RequestMessage
 * 
 * Requests a message from ANT. 
 * Sets state variables for response or timeout. 
 *
 * Params:
 *
 * ucChannel_: ANT channel
 * ucMsgID_: Requested message ID
 * 
 * returns: Returns right aways (does not blowck waiting for response)
 *
 **************************************************************************/      
void ANTClass::RequestMessage(UCHAR ucChannel_, UCHAR ucMsgID_)
{
   if(!pclMyCommandTimeout->bWaitingForResponse)
   {
      ANT_RequestMessage(ucChannel_, ucMsgID_);
      pclMyCommandTimeout->WaitForResponse(ucChannel_, ucMsgID_, MESG_REQUEST_ID);
   }
   else
   {
      System::Diagnostics::Debug::Assert(FALSE, "Command sent before response or timeout recieved");
   }    
} 
/**************************************************************************
 * WriteMessage
 * 
 * Sends a custom message to ANT.
 *
 * Params:
 *
 * ucMessageID: messageID
 * aucData: messsage payload
 * usMessageSize: length of payload
 * 
 * returns: Returns right aways (does not block waiting for response)
 *
 **************************************************************************/
void ANTClass::WriteMessage(UCHAR ucMessageID, UCHAR* aucData, USHORT usMessageSize)
{
   ANT_WriteMessage(ucMessageID, aucData, usMessageSize);
}
/**************************************************************************
 * SendBroadcastData
 * 
 * Sends a broadcast message to ANT
 *
 * Params:
 *
 * ucChannel_: ANT channel
 * pucData_: Pointer to broadcast data buffer
 * 
 * returns: N/A
 *
 **************************************************************************/      
void ANTClass::SendBroadcastData(UCHAR ucChannel_, UCHAR* pucData_)
{
   ANT_SendBroadcastData(ucChannel_, pucData_);
}
/**************************************************************************
 * SendAcknowledgedData
 * 
 * Sends an acknowledged message to ANT
 *
 * Params:
 *
 * ucChannel_: ANT channel
 * pucData_: Pointer to acknowledge data buffer
 * 
 * returns: N/A
 *
 **************************************************************************/      
void ANTClass::SendAcknowledgedData(UCHAR ucChannel_, UCHAR* pucData_)
{
   SendAcknowledgedData(ucChannel_, pucData_, 0);
}
void ANTClass::SendAcknowledgedData(UCHAR ucChannel_, UCHAR* pucData_, USHORT usTimeout_)
{

   System::Diagnostics::Debug::Assert(paclMyAckTimeout[ucChannel_] != nullptr, "ANT Class not initialized properly! ");


   if(usTimeout_)
   {
      paclMyAckTimeout[ucChannel_]->Interval = usTimeout_;
      paclMyAckTimeout[ucChannel_]->WaitForResponse(ucChannel_, 0,0);
      
   }

   ANT_SendAcknowledgedData(ucChannel_, pucData_);
} 
/**************************************************************************
 * SendBurstTransfer
 * 
 * Sends a block of data as burst transfers
 *
 * Params:
 *
 * ucChannel_: ANT channel
 * pucData_: Pointer to burst data buffer
 * usSize_: Number of burst packets that need to be sent.
 * 
 * returns: After last burst packet has been sent to the ANT chip
 *
 **************************************************************************/      
void ANTClass::SendBurstTransfer(UCHAR ucChannel_, UCHAR* pucData_, USHORT usSize_)
{
   ANT_SendBurstTransfer(ucChannel_, pucData_, usSize_);
}
/**************************************************************************
 * AckTimerEvent
 * 
 * Timeout event callback routine called by Acknowledged message timer. This 
 * should only ever fire if no valid response was recieved to an acknowledged
 * message. The EVENT_ACK_TIMEOUT is propagted through the Channel Event 
 * function chain. 
 *
 * Params:
 *
 * ucANTChannel_:  ANT channel
 * 
 * returns: N/A
 *
 **************************************************************************/      
void ANTClass::AckTimerEvent(UCHAR ucChannel_)
{

   aucRxBuf[0] = ucChannel_;
   aucRxBuf[1] = 1;
   aucRxBuf[2] = EVENT_ACK_TIMEOUT;

   ANTChannelEventProcess(ucChannel_, EVENT_ACK_TIMEOUT);
}

/**************************************************************************
 * TimerEvent
 * 
 * Timeout event callback routine called by command message timer. This 
 * should only ever fire if no valid response was recieved to a command that 
 * requires a response. The EVENT_ACK_TIMEOUT is propagted through the Protocol 
 * Event function chain. 
 *
 * Params:
 *
 * ucANTChannel_:  ANT channel
 * 
 * returns: N/A
 *
 **************************************************************************/      
void ANTClass::TimerEvent(UCHAR ucChannel_)
{

   // Assert ucChannel_ == pclMyCommandTimeout->ucChannel

   // If this is (for example) a REQUEST message,
   // then need to flip the message ID to make the 
   // response work properly.
   if(pclMyCommandTimeout->ucResponseID != MESG_RESPONSE_EVENT_ID)
   {
      pclMyCommandTimeout->ucResponseID = MESG_RESPONSE_EVENT_ID;
   }

   aucResponseBuf[0] = pclMyCommandTimeout->ucChannel;
   aucResponseBuf[1] = pclMyCommandTimeout->ucMessageID;
   aucResponseBuf[2] = EVENT_COMMAND_TIMEOUT;

 
   ANTProtocolEventProcess(0, pclMyCommandTimeout->ucResponseID);
   pclMyCommandTimeout->ClearWait();
  
}  


/**************************************************************************
 * TimeoutStruct
 * 
 * Constructor for timer command timer. 
 * 
 * Params:
 *
 * ChannelCallbackFunc_: Reference to delegate which is called when timer 
 *                       timeout elapses. 
 * 
 * returns: N/A
 *
 **************************************************************************/      
TimeoutStruct::TimeoutStruct(ChannelDelegate^ ChannelCallbackFunc_)
{
   ChannelCallback = ChannelCallbackFunc_;
}

/**************************************************************************
 * ProcessTimerEvent
 * 
 * Callback function passed as delegate to timer. Called when timeout time
 * elaspes. This function handles the timeout and calls it delegate function 
 * to propagate the timeout event to the ANT Class. 
 * 
 * Params: N/A (required for compatibility of timer "elapsed" delegate)
 * 
 * returns: N/A
 *
 **************************************************************************/      
void TimeoutStruct::ProcessTimerEvent(System::Object^  sender, System::Timers::ElapsedEventArgs^ e)
{
   Stop();

   if(bWaitingForResponse)
   {
      ChannelCallback(ucChannel);
   }
}


/**************************************************************************
 * WaitForResponse
 * 
 * Sets state variables and starts timer. Called when a command is sent
 * that requires some response. 
 *
 * Params: 
 *
 * ucChannel_: ANT Channel
 * ucResponseID_: Message ID of the response we are waiting for
 * ucMessageID_: The message ID of the message just sent (the one istigating the response)
 *
 * 
 * returns: Right away
 *
 **************************************************************************/      
void TimeoutStruct::WaitForResponse(UCHAR ucChannel_, UCHAR ucResponseID_, UCHAR ucMessageID_)
{  
   ucMessageID = ucMessageID_;
   ucResponseID = ucResponseID_;
   ucChannel = ucChannel_;
   bWaitingForResponse = TRUE;
   Start();
}


/**************************************************************************
 * ClearWait
 * 
 * Clears the waiting state machines variables. This should be called if waiting 
 * for a response to some command and it is recieved, or if the request times out.
 * 
 *
 * Params: N/A
 * 
 * returns: N/A
 *
 **************************************************************************/      
void TimeoutStruct::ClearWait()
{  
   Stop();
   ucMessageID = 0;
   ucResponseID = 0;
   ucChannel = 0;
   bWaitingForResponse = FALSE;

}




















