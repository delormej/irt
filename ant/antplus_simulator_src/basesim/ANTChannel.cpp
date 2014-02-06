/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/

#include "StdAfx.h"
#include "ANTChannel.h"
#include "ANTPlus.h"
#include "ANTClass.h"


/**************************************************************************
 * ANTChannel::ANTChannel
 * 
 * ANT CHannel constructor - called for each channel allocated to a 
 * device.
 *
 * Params:
 *
 * ucInstanceNum_:   Corressponds to channel number. 
 * ucSimType_:       Type of simulator this channel will represent - either sensor or display. 
 *          
 * returns: N/A
 *
 **************************************************************************/
ANTChannel::ANTChannel(UCHAR ucInstanceNum_, UCHAR ucSimType_, ANTPlatform eHWType_, CapabilitiesStruct sCapabilities_)
{
	System::Windows::Forms::Control::CheckForIllegalCrossThreadCalls = FALSE;

	InitializeComponent();
   bTXPowerUIEnabled = FALSE;
	ucSimType = ucSimType_;
	ucMyChannelNum = ucInstanceNum_;
	ucMyTxType = 1;
	ucMyDeviceType = 1;				                        // Transmission Type and Device Type will be set by simulator once loaded
	usMyMsgPeriod = 8192;				                     // 4Hz, set to specific simulator value once loaded
	dCurEventTime = 0;
	bPairingBit = FALSE;			                           // Pairing bit is disabled by default
	bDisplayHex = TRUE;				                        // Display raw data in hex by default
	eMyChannelState = STATE_CHANNEL_CLOSED;					// Channel is initially closed
	bMyCheckInternalRaise = FALSE;
   bAlreadyUnAssigned = FALSE;								   // For dealing with differences in how AP2 search timeouts

	this->tabPage_ChannelTab->Text = String::Concat("Channel ", System::Convert::ToString(ucMyChannelNum));
	
	// Create timer to handle event simulation
	this->timer_SimEvent = (gcnew System::Timers::Timer(500));
	this->timer_SimEvent->Elapsed += gcnew System::Timers::ElapsedEventHandler(this, &ANTChannel::timer_SimEvent_Tick);
	
   // Store the capabilities bytes
   sMyCapabilities.ucStandardOptions = sCapabilities_.ucStandardOptions;
   sMyCapabilities.ucAdvancedOptions = sCapabilities_.ucAdvancedOptions;
   sMyCapabilities.ucAdvancedOptions2 = sCapabilities_.ucAdvancedOptions2;

   
   switch(ucSimType)
	{
		case SIM_SENSOR:
		{
			// Display simulator list on combo box
			this->comboBox_SimSelect->Items->AddRange(gcnew cli::array< System::Object^  >(UNSELECTED) SUPPORTED_SENSORS);
			// Create a device ID based on channelNum to prevent duplicates
			usMyDeviceNum = (ucMyChannelNum*137)+100;
			this->textBox_DeviceID->Text = System::Convert::ToString(usMyDeviceNum);
			// Simulator is transmitter
			this->checkBox_TxEnable->Text = L"Transmitting";
			// Disable functionality for getting Channel ID of paired device
			this->button_getChannelID->Visible = false;
			// Keep timer alive
			System::GC::KeepAlive(timer_SimEvent);
			break;
		}
		case SIM_DISPLAY:
		{
			// Display simulator list on combo box
			this->comboBox_SimSelect->Items->AddRange(gcnew cli::array< System::Object^  >(UNSELECTED) SUPPORTED_DISPLAYS);
			// Wildcard Channel ID
			usMyDeviceNum = 0;
			this->textBox_DeviceID->Text = System::Convert::ToString(usMyDeviceNum);
			// Simulator is receiver
			this->checkBox_TxEnable->Text = L"Receiving";
			// Enable functionality for getting Channel ID of paired device
			this->button_getChannelID->Visible = true;
			// Ensure timer is disabled
			this->timer_SimEvent->Stop();
			// Disable display of simulated time (display simulation is not timer-driven)
			this->label_TxTime->Visible = false;
			this->label_TxTimeDisplay->Visible = false;
			break;
		}
	default:
		{
			// Ensure timer is disabled
			this->timer_SimEvent->Stop();
			break;
		}
	}
   // Init the TX Power Select Dropdown based on which HW platform was found
   switch (eHWType_)
   {
      case PLATFORM_AP1_ANT11TR:      
      {       
         this->comboBox_TxPowerSelect->Items->AddRange(gcnew cli::array< System::Object^ > TXPOWER_SETTINGS1);
         break;
      }
      case PLATFORM_AP2_USB2_AT3:
      {         
         this->comboBox_TxPowerSelect->Items->AddRange(gcnew cli::array< System::Object^  > TXPOWER_SETTINGS2);
         break;
      }      
      case PLATFORM_C7:         
      {       
         this->comboBox_TxPowerSelect->Items->AddRange(gcnew cli::array< System::Object^  > TXPOWER_SETTINGS3);
         break;
      }
      case PLATFORM_UNKNOWN: 
      // fall through
      default:
      {         
         this->comboBox_TxPowerSelect->Items->AddRange(gcnew cli::array< System::Object^  > TXPOWER_SETTINGS_UNKNOWN);
         break;        
      }   
   }
   // Make sure the dropdown starts showing the default power 
   this->comboBox_TxPowerSelect->SelectedIndex = DEFAULT_RADIO_TX_POWER;   
   bTXPowerUIEnabled = TRUE;
}


/**************************************************************************
 * ANTChannel::~ANTChannel
 * 
 * ANT CHannel deconstructor - called after channels are destroyed.
 *
 * returns: N/A
 *
 **************************************************************************/
ANTChannel::~ANTChannel()
{
	if(curSimulator)
	{
		delete curSimulator;
	}

	delete timer_SimEvent;
	this->tabPage_ChannelTab->Controls->Clear();
	delete this->tabPage_ChannelTab;
	delete this->tabControl1;
	//clean up floating resources with the garbage collector
	GC::Collect(2);
	if (components)
	{
		delete components;
	}
}


/**************************************************************************
 * ANTChannel::ANTProtocolEvent
 * 
 * MainForm ANT Protocol Event handler - Handles ANT responses to ANT commands and
 * requested messages from ANT 
 *
 * Params:
 *
 * ucMessageCode_:   Message ID of packet recieved from ANT. 
 * pcBuffer_:   Pointer to message buffer recieved from ANT. 
 *          
 * returns: N/A
 *
 **************************************************************************/
void ANTChannel::ANTProtocolEvent(UCHAR ucMessageCode_, UCHAR* pcBuffer_)
{
   if(ucMessageCode_ == MESG_CHANNEL_ID_ID)
   {
      // Requesting the Channel ID is only available on the receiver
      if(ucSimType == SIM_DISPLAY)
      {
         // Update Channel Parameters
         usMyDeviceNum = (USHORT) pcBuffer_[1] + ((USHORT) pcBuffer_[2] << 8);
         ucMyDeviceType = (UCHAR) pcBuffer_[3];
         ucMyTxType = (UCHAR) pcBuffer_[4];
         // Display Channel ID
         this->textBox_DeviceID->ReadOnly = FALSE;
         this->textBox_DeviceID->Text = System::Convert::ToString(usMyDeviceNum);
         this->textBox_DeviceID->ReadOnly = TRUE;
         this->textBox_DeviceType->ReadOnly = FALSE;
         this->textBox_DeviceType->Text = System::Convert::ToString(ucMyDeviceType);
         this->textBox_DeviceType->ReadOnly = TRUE;
         this->textBox_TransmissionType->ReadOnly = FALSE;
         this->textBox_TransmissionType->Text = System::Convert::ToString(ucMyTxType);
         this->textBox_TransmissionType->ReadOnly = TRUE;
      }
   }
   else if(ucMessageCode_ == MESG_RESPONSE_EVENT_ID)
   {
      // Check if this was a successful response. 
      // Otherwise handle the error condition.
      if(pcBuffer_[2] == RESPONSE_NO_ERROR)
      {
         switch (pcBuffer_[1])
         {
            case MESG_ASSIGN_CHANNEL_ID:
            {
         
               if(eMyChannelState == STATE_CHANNEL_OPENING)
               {
                  // Set RF Freq. 
                  ANTClass::SetChannelRFFreq(ucMyChannelNum, ANTPLUS_RF_FREQ);
               }
               else
               {
                  System::Diagnostics::Debug::Assert(FALSE, "Invalid State");
               }
               break;
            }
            case MESG_UNASSIGN_CHANNEL_ID:
            {
	            this->checkBox_TxEnable->Enabled = TRUE;
               break;
            }
         
            case MESG_CHANNEL_ID_ID:
            {
               if(eMyChannelState == STATE_CHANNEL_OPENING)
               {
	               ANTClass::SetChannelPeriod(ucMyChannelNum, usMyMsgPeriod);
               }
               else
               {
                  System::Diagnostics::Debug::Assert(FALSE, "Invalid State");
               }
               break;
            }
            case MESG_CHANNEL_MESG_PERIOD_ID:
            {
               if(eMyChannelState == STATE_CHANNEL_OPENING)
               {
	                     ANTClass::OpenChannel(ucMyChannelNum);
                     
                  
               }
#ifdef INC_GEOCACHE
               else if(eMyChannelState == STATE_CHANNEL_OPEN)
			   {
					// Do Nothing - this is the Response from the Dynamic Mesg Period change
					// specific to Geocache Profile ...
			   }
#endif	// INC_GEOCACHE
               else
               {
                  System::Diagnostics::Debug::Assert(FALSE, "Invalid State");
               }
               break;
            }
         
         
            case MESG_CHANNEL_RADIO_FREQ_ID:   
            {
               if(eMyChannelState == STATE_CHANNEL_OPENING)
               {
               
   	                  ANTClass::SetChannelId(ucMyChannelNum, usMyDeviceNum, ucMyDeviceType, ucMyTxType);
               }
               else
               {
                  System::Diagnostics::Debug::Assert(FALSE, "Invalid State");
               }
               break;
            }
            case MESG_OPEN_CHANNEL_ID:        
            {
               if(eMyChannelState == STATE_CHANNEL_OPENING)
               {
                  eMyChannelState = STATE_CHANNEL_OPEN;
			         UpdateRawTxDisplay("\nChannel Opened...", NULL);
               
	               this->checkBox_TxEnable->Enabled = TRUE;				  
               
			         if(ucSimType == SIM_SENSOR)
			         {
				         // Enable timer for sensor simulation
				         this->timer_SimEvent->Enabled = TRUE;
#ifdef INC_WEIGHT_SCALE
						 if(comboBox_SimSelect->SelectedIndex == WEIGHT_SCALE)
							 curSimulator->ANT_eventNotification(MESG_OPEN_CHANNEL_ID, NULL);
#endif // INC_WEIGHT_SCALE
			         }
			         if(ucSimType == SIM_DISPLAY) 
			         {
				         // Enable capability of obtaining channel ID of paired device on display devices
				         this->button_getChannelID->Enabled = TRUE;
#ifdef INC_GEOCACHE
						 if(this->comboBox_SimSelect->SelectedIndex == GEOCACHE)
						 {
							 ANTClass::SetChannelSearchTimeout(ucMyChannelNum, 45);		// !!! 45sec Search Timeout for Geocache Display !!!
						 }
#endif	// INC_GEOCACHE
			         }
#ifdef INC_TEMPERATURE
						 if(comboBox_SimSelect->SelectedIndex == TEMPERATURE)
							 curSimulator->ANT_eventNotification(MESG_OPEN_CHANNEL_ID, NULL);
#endif // INC_TEMPERATURE
               
			         //Don't allow changes to channel configuration while channel is open
			         this->checkBox_PairingOn->Enabled = FALSE;
			         textBox_DeviceID->ReadOnly = TRUE;
                  textBox_TransmissionType->ReadOnly = TRUE;
               
#ifdef INC_CUSTOM
			         if(this->comboBox_SimSelect->SelectedIndex == CUSTOM)
					  {
				         textBox_DeviceType->ReadOnly = TRUE;
				         textBox_TransmissionType->ReadOnly = TRUE;
				         textBox_TransmitPeriod->ReadOnly = TRUE;
			         }
#endif // INC_CUSTOM
               }
               else
               {
                  System::Diagnostics::Debug::Assert(FALSE, "Invalid State");
               }
               break;
            }
            case MESG_CLOSE_CHANNEL_ID:
            {
				 eMyChannelState = STATE_CHANNEL_CLOSED;
				 timer_SimEvent->Enabled = FALSE;
		         // Re-enable changes to the configuration
		         textBox_DeviceID->ReadOnly = FALSE;
               textBox_TransmissionType->ReadOnly = FALSE;
		         this->checkBox_PairingOn->Enabled = TRUE; // Start by default without setting the pairing bit
				   this->checkBox_PairingOn->Checked = FALSE;
		         bPairingBit = FALSE;
#ifdef INC_CUSTOM
		         if(this->comboBox_SimSelect->SelectedIndex == CUSTOM)
		         {
			         this->textBox_TransmissionType->ReadOnly = FALSE;
			         this->textBox_TransmitPeriod->ReadOnly = FALSE;
		         }
#endif // INC_CUSTOM
#ifdef INC_WEIGHT_SCALE
				 if(this->comboBox_SimSelect->SelectedIndex == WEIGHT_SCALE)
					curSimulator->ANT_eventNotification(MESG_CLOSE_CHANNEL_ID, NULL);
#endif // INC_WEIGHT_SCALE         
#ifdef INC_GEOCACHE
				 if(this->comboBox_SimSelect->SelectedIndex == GEOCACHE)
					curSimulator->ANT_eventNotification(MESG_CLOSE_CHANNEL_ID, NULL);
#endif	// INC_GEOCACHE
#ifdef INC_TEMPERATURE
				 if(this->comboBox_SimSelect->SelectedIndex == TEMPERATURE)
					curSimulator->ANT_eventNotification(MESG_CLOSE_CHANNEL_ID, NULL);
#endif	// INC_TEMPERATURE
               break;
            }        
         }
      }
      else
      {
         if(eMyChannelState == STATE_CHANNEL_OPENING)
         {
            ChannelOpenFailed();
         }

         if( pcBuffer_[2] == EVENT_COMMAND_TIMEOUT)
         {
            UpdateRawTxDisplay("\nCOMMAND TIMEOUT!!!!!", NULL);
         }      
      }
   }
}

/**************************************************************************
 * ANTChannel::ANTChannelEvent
 * 
 * Process ANT event
 * The display is updated accordingly, and the event is forwarded to 
 * the simulator for device specific handling
 * !! IMPORTANT: only events defined here are processed by simulators
 *
 * ucEvent_: event code
 * pcBuffer_: pointer to buffer containing data received from ANT
 *
 * returns: N/A
 *
 **************************************************************************/
void ANTChannel::ANTChannelEvent(UCHAR ucEvent_, UCHAR* pcBuffer_)
{
   UCHAR ucBufferIndex = 1;

	switch(ucEvent_)
	{   
		case EVENT_TX:
			{
				// Simulated sensors update the transmit buffer every message period
				if(ucSimType == SIM_SENSOR)
				{
					sendSimMsg();
				}
				break;
			}
		case EVENT_RX_BROADCAST:
      {
		   UpdateRawTxDisplay("\nRX:", (UCHAR*) &pcBuffer_[ucBufferIndex]);
			
         // Forward to simulator for further processing
			curSimulator->ANT_eventNotification(ucEvent_, (UCHAR*) &pcBuffer_[ucBufferIndex]);
 			break;
	   }
		case EVENT_RX_SEARCH_TIMEOUT:
		{
			// This event should only take place on a receiver
			if(ucSimType == SIM_DISPLAY) 
			{
				eMyChannelState = STATE_CHANNEL_CLOSED;
				this->checkBox_TxEnable->Checked = FALSE;
				this->checkBox_PairingOn->Enabled = TRUE;
				this->textBox_DeviceID->Enabled = TRUE;
				UpdateRawTxDisplay("\nReceiver Search Timeout", NULL);
				ANTClass::UnAssignChannel(ucMyChannelNum); // Necessary for ARCT functionality            
				// the AP2 chip sends 2 commands on timeout: EVENT_RX_SEARCH_TIMEOUT AND EVENT_CHANNEL_CLOSED
				// beacuse the preceding line just unassigned the channel, for AP2, the EVENT_CHANNEL_CLOSED 
				// will happen next and try to close / unassign an already unassigned channel. This flag prevents this.
				bAlreadyUnAssigned = TRUE;
			}
			break;
		}
		case EVENT_RX_ACKNOWLEDGED:
		{
			UpdateRawTxDisplay("\nRX Acknowledged Msg:\n  ", (UCHAR*) &pcBuffer_[1]);
			// Forward to simulator for further processing
			curSimulator->ANT_eventNotification(ucEvent_, (UCHAR*) &pcBuffer_[1]);
			break;
		}
		case EVENT_RX_FAIL:
		{
			UpdateRawTxDisplay("\nRX Failure!", NULL);
			// Forward to simulator for further processing
			curSimulator->ANT_eventNotification(ucEvent_, pcBuffer_);
			break;
		}
		case EVENT_TRANSFER_TX_COMPLETED:
		{
			UpdateRawTxDisplay("\nTX Success!", NULL);
			// Forward to simulator for further processing
			curSimulator->ANT_eventNotification(ucEvent_, pcBuffer_);
			break;
		}
		case EVENT_TRANSFER_TX_FAILED:
		{
			UpdateRawTxDisplay("\nTX Failure!", NULL);
			// Forward to simulator for further processing
			curSimulator->ANT_eventNotification(ucEvent_, pcBuffer_);
			break;
		}
		case EVENT_CHANNEL_CLOSED:
		{
			if (bAlreadyUnAssigned == FALSE)				// only unassign the channel if it is not already done
				ANTClass::UnAssignChannel(ucMyChannelNum);
  			UpdateRawTxDisplay("\n...Channel Closed", NULL);
			bAlreadyUnAssigned = FALSE;						// reset the flag
			break;
		}
		case EVENT_RX_FAIL_GO_TO_SEARCH:
		{
			// This event should only take place on a receiver
			if(ucSimType == SIM_DISPLAY)
			{
				UpdateRawTxDisplay("\nRepeated RX Failure,\nSearching for Signal...", NULL);
			}
			break;
		}
      
      case EVENT_ACK_TIMEOUT:
      {
			UpdateRawTxDisplay("Ack timeout...", NULL);
			// Forward to simulator for further processing
			// Only forward to receivers, as a master would get an EVENT_TRANSFER_TX_FAILED instead
			if(ucSimType == SIM_DISPLAY)
			{
				curSimulator->ANT_eventNotification(ucEvent_, NULL);	
			}
			break;
         
      }
	  case EVENT_RX_BURST_PACKET:
	  {
			UpdateRawTxDisplay("\nRX BURST Msg:\n  ", (UCHAR*) &pcBuffer_[1]);
			break;
	  }
	 default:
	 {
		  // Look up event in table
		  int iIndex = Array::IndexOf(ANTEventTable::aucCode, ucEvent_);
		  if(iIndex >=0 && iIndex < ANTEventTable::aucCode->Length)
			UpdateRawTxDisplay(String::Concat("\nEvent: ", ANTEventTable::aucDescr[iIndex]), NULL);
		  else
			UpdateRawTxDisplay(String::Concat("\nUnknown Event:", System::Convert::ToString(ucEvent_)), NULL);
		  break;
	  }
	}
   
   
 
   
}


/**************************************************************************
 * ANTChannel::sendSimMsg
 * 
 * Handle update of transmit buffer every message period, and 
 * transmission of broadcast message
 * Only performed in sensor simulators
 * The pointer to the transmit buffer is provided to the simulator
 * specific ANT event handler so that it can be updated
 *
 * returns: N/A
 *
 **************************************************************************/
void ANTChannel::sendSimMsg()
{
	UCHAR aucTxBuffer[8] = {0,0,0,0,0,0,0,0};

	if(eMyChannelState == STATE_CHANNEL_OPEN && ucSimType == SIM_SENSOR)
	{
		// Call simulator to update transmit buffer
		curSimulator->ANT_eventNotification(EVENT_TX, aucTxBuffer);
		
      {
         // Update display
		   UpdateRawTxDisplay("\nTX:", (UCHAR*) aucTxBuffer);
		   // Send broadcast data
		   ANTClass::SendBroadcastData(ucMyChannelNum, (UCHAR*) aucTxBuffer);
      
      }
      
      
      
   }
}

/**************************************************************************
 * ANTChannel::updateMesgPeriod
 * 
 * Allows simulators to update message period on demand
 *
 * usMesgRate_: new message period
 *
 * returns: N/A
 *
 **************************************************************************/
void ANTChannel::updateMesgPeriod(USHORT usMesgPeriod_)
{
	// Update display
	UpdateRawTxDisplay(String::Concat("\nMessage Period Changed: ", usMesgPeriod_.ToString()), NULL);

	// Update Message Period
	usMyMsgPeriod = usMesgPeriod_;
	ANTClass::SetChannelPeriod(ucMyChannelNum, usMyMsgPeriod);
}

/**************************************************************************
 * ANTChannel::sendAckMsg
 * 
 * Allows simulators to send acknowledged messages on demand
 *
 * pucTxBuffer_: pointer to buffer containing data to send
 *
 * returns: N/A
 *
 **************************************************************************/
void ANTChannel::sendAckMsg(UCHAR* pucTxBuffer_)
{
	USHORT usAckTimeout = (USHORT) (((ULONG) usMyMsgPeriod * 1000)/32768);	// Timeout = 1 message period
	if(pucTxBuffer_)
	{
		// Update display
		UpdateRawTxDisplay("\nTX Acknowledged Msg:\n  ", (UCHAR*) pucTxBuffer_);
		// Send acknowledged data
		if(ucSimType == SIM_DISPLAY)
			ANTClass::SendAcknowledgedData(ucMyChannelNum, (UCHAR*) pucTxBuffer_, usAckTimeout);	// Timeout only required for receiver
		else if(ucSimType == SIM_SENSOR)
			ANTClass::SendAcknowledgedData(ucMyChannelNum, (UCHAR*) pucTxBuffer_);
	}
}



/**************************************************************************
 * ANTChannel::sendBroadcastMsg
 * 
 * Allows simulators to send broadcast messages on demand
 * Used in display simulators when sending responses to received messages
 *
 * pucTxBuffer_: pointer to buffer containing data to send
 *
 * returns: N/A
 *
 **************************************************************************/
void ANTChannel::sendBroadcastMsg(UCHAR* pucTxBuffer_)
{
	if(pucTxBuffer_)
	{
		// Update display
		UpdateRawTxDisplay("\nTX:", (UCHAR*) pucTxBuffer_);
		// Send broadcast data
		ANTClass::SendBroadcastData(ucMyChannelNum, (UCHAR*) pucTxBuffer_);
	}
}



/**************************************************************************
 * ANTChannel::timer_SimEvent_Tick
 * 
 * Called every timer event, for event driven simulators (sensors)
 * The timer interval is updated after updating device specific simulated
 * data
 *
 * returns: N/A
 *
 **************************************************************************/
System::Void ANTChannel::timer_SimEvent_Tick(System::Object^  sender, System::Timers::ElapsedEventArgs^ e) 
{
	if(eMyChannelState == STATE_CHANNEL_OPEN && ucSimType == SIM_SENSOR)
	{
		// Update current event time (for display purposes)
		dCurEventTime += timer_SimEvent->Interval;
		this->label_TxTimeDisplay->Text = System::TimeSpan::FromSeconds(System::Math::Round(dCurEventTime/1000)).ToString();
		// Simulator specific event-driven behavior
		curSimulator->onTimerTock((USHORT) dCurEventTime);
		// Update timer interval (as per simulator)
		this->timer_SimEvent->Interval = curSimulator->getTimerInterval();
	}
}

/**************************************************************************
 * ANTChannel::UpdateRawTxDisplay
 * 
 * Displays current event in raw box
 *
 * strLabel_: label identifying the event
 * pucBuffer_: pointer to buffer with data to display
 *
 * returns: N/A
 *
 **************************************************************************/
System::Void ANTChannel::UpdateRawTxDisplay(System::String^ strLabel_, UCHAR* pucBuffer_)
{
	UCHAR i;

	richTextBox_RawTxDisplay->AppendText(strLabel_);
	
	if(pucBuffer_)
	{
		for(i=0; i<8; ++i)
		{
			if(bDisplayHex)
			{
				System::String^ strTemp = System::Convert::ToString(pucBuffer_[i], 16);
				if(strTemp->Length == 1)
					richTextBox_RawTxDisplay->AppendText(System::String::Concat("[","0",strTemp, "]"));
				else
					richTextBox_RawTxDisplay->AppendText(System::String::Concat("[",strTemp, "]"));
			}			
			else
			{
				richTextBox_RawTxDisplay->AppendText(System::String::Concat("[", pucBuffer_[i], "]"));
			}
		}
	}
}


/**************************************************************************
 * ANTChannel::button_Clear_Click
 * 
 * Clears raw text box
 *
 * returns: N/A
 *
 **************************************************************************/
System::Void ANTChannel::button_Clear_Click(System::Object^  sender, System::EventArgs^  e)
{
	richTextBox_RawTxDisplay->Text ="";
}


/**************************************************************************
 * ANTChannel::checkBox_TxEnable_CheckedChanged
 * 
 * Opens and closes the channel, as selected by the user
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void ANTChannel::checkBox_TxEnable_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{

	// Return if value of checkbox was changed internally and not by the user
	if(bMyCheckInternalRaise)
   {
		bMyCheckInternalRaise = FALSE;
		return;
	}
	
	// Disable changes while performing checks
	this->checkBox_TxEnable->Enabled = FALSE;
	
   if(!curSimulator)
   {
		// Disable opening the channel if no simulator was loaded
		bMyCheckInternalRaise=TRUE;
		this->checkBox_TxEnable->Checked = FALSE;
	    this->checkBox_TxEnable->Enabled = TRUE;
      
		UpdateRawTxDisplay("\nChoose Simulator First ...", NULL);
      
	}
	else if(checkBox_TxEnable->Checked && curSimulator)
   {
		// Check channel parameters are valid (in case they were modified by the user)
		if(!ValidateChannelID())
		{
			// Return if channel parameters are incorrect
			UpdateRawTxDisplay("\nChannel Setup Failed: Bad Channel Parameters", NULL);
			bMyCheckInternalRaise = TRUE;
			this->checkBox_TxEnable->Checked = FALSE;
			this->checkBox_TxEnable->Enabled = TRUE; 
			eMyChannelState = STATE_CHANNEL_CLOSED;
			return;
		}
      

		switch(ucSimType)
      {
			// Open Channel
			case SIM_SENSOR:
				OpenChannel(PARAMETER_TX_NOT_RX);
				break;
			case SIM_DISPLAY:
				OpenChannel(PARAMETER_RX_NOT_TX);
				break;
			default:
            eMyChannelState = STATE_CHANNEL_CLOSED;
				break;
		}
      // Reassert the TX Power level in case the user selected 
      comboBox_TxPowerSelect_SelectedIndexChanged(sender, e);
	}
	else
	{
		if(eMyChannelState == STATE_CHANNEL_OPEN)
		{	
			// Only close if channel was previously open
			ANTClass::CloseChannel(ucMyChannelNum);         
		}
	}
}


/**************************************************************************
 * ANTChannel::comboBox_SimSelect_SelectedIndexChanged
 * 
 * Handles selection of simulator to load.
 * Selecting the same simulator will cause it to reload, resetting to 
 * initial values
 *
 * returns: N/A
 *
 **************************************************************************/
System::Void ANTChannel::comboBox_SimSelect_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
	//Delete last simulator
	if(curSimulator)
		delete curSimulator;
	// By default, unset pairing bit
	this->checkBox_PairingOn->Checked = FALSE;
	bPairingBit = FALSE;
   
	// Reset simulation timer
	dCurEventTime = 0;
	if(SIM_SENSOR)
	{
		this->label_TxTimeDisplay->Text = "0";
	}
	// Load simulator
	switch(comboBox_SimSelect->SelectedIndex)
	{
#ifdef INC_HRM
	case HEART_RATE_MONITOR:
		switch(ucSimType)
		{
#ifdef SIM_TX
			case SIM_SENSOR:
				curSimulator = gcnew HRMSensor(timer_SimEvent);
				break;
#endif // SIM_TX
#ifdef SIM_RX
			case SIM_DISPLAY:
				curSimulator = gcnew HRMDisplay();
				break;
#endif // SIM_RX
			default:
				break;
		}
		break;
#endif // INC_HRM
#ifdef INC_SDM
	case SPEED_DISTANCE_MONITOR:
		switch(ucSimType)
		{
#ifdef SIM_TX
			case SIM_SENSOR:
				curSimulator = gcnew SDMSensor(timer_SimEvent,gcnew dRequestAckMsg(this,&ANTChannel::sendAckMsg));
				break;
#endif // SIM_TX
#ifdef SIM_RX
			case SIM_DISPLAY:
				curSimulator = gcnew SDMDisplay(gcnew dRequestAckMsg(this,&ANTChannel::sendAckMsg));  // modified to add Tx
				break;
#endif // SIM_RX
			default:
				break;
		}
		break;
#endif // INC_HRM
#ifdef INC_BIKE_POWER
	case BIKE_POWER:
		switch(ucSimType)
		{
#ifdef SIM_TX
			case SIM_SENSOR:
				curSimulator = gcnew BikePowerSensor(timer_SimEvent, gcnew dRequestAckMsg(this,&ANTChannel::sendAckMsg));
				break;
#endif // SIM_TX
#ifdef SIM_RX
			case SIM_DISPLAY:
				curSimulator = gcnew BikePowerDisplay(gcnew dRequestAckMsg(this,&ANTChannel::sendAckMsg));
				break;
#endif // SIM_RX
			default:
				break;
		}
		break;
#endif // INC_BIKE_POWER
#ifdef INC_BIKE_SPEED_CADENCE
	case BIKE_SPDCAD:
		switch(ucSimType)
		{
#ifdef SIM_TX
			case SIM_SENSOR:
				curSimulator = gcnew BikeSpdCadSensor(timer_SimEvent);
				break;
#endif // SIM_TX
#ifdef SIM_RX
			case SIM_DISPLAY:
				curSimulator = gcnew BikeSpdCadDisplay();
				break;
#endif // SIM_RX
			default:
				break;
		}
		break;
#endif // INC_BIKE_SPEED_CADENCE
#ifdef INC_BIKE_CADENCE
	case BIKE_CADENCE:
		switch(ucSimType)
		{
#ifdef SIM_TX
			case SIM_SENSOR:
				curSimulator = gcnew BikeCadenceSensor(timer_SimEvent);
				break;
#endif // SIM_TX
#ifdef SIM_RX
			case SIM_DISPLAY:
				curSimulator = gcnew BikeCadenceDisplay();
#endif // SIM_RX
			default:
				break;
		}
		break;
#endif // BIKE_CADENCE
#ifdef INC_BIKE_SPEED
	case BIKE_SPEED:
		switch(ucSimType)
		{
#ifdef SIM_TX
			case SIM_SENSOR:
				curSimulator = gcnew BikeSpeedSensor(timer_SimEvent);
				break;
#endif // SIM_TX
#ifdef SIM_RX
			case SIM_DISPLAY:
				curSimulator = gcnew BikeSpeedDisplay();
				break;
#endif // SIM_RX
			default:
				break;
		}
		break;
#endif // INC_BIKE_SPEED
#ifdef INC_WEIGHT_SCALE
	case WEIGHT_SCALE:
		switch(ucSimType)
		{
#ifdef SIM_TX
			case SIM_SENSOR:
				curSimulator = gcnew WeightScaleSensor(timer_SimEvent);
				break;
#endif // SIM_TX
#ifdef SIM_RX
			case SIM_DISPLAY:
				curSimulator = gcnew WeightScaleDisplay(gcnew dRequestBcastMsg(this,&ANTChannel::sendBroadcastMsg));
				break;
#endif // SIM_RX
			default:
				break;
		}
		break;
#endif //INC_WEIGHT_SCALE
#ifdef INC_MSM
	case MULTISPORT_SPEED_DISTANCE:
		switch(ucSimType)
		{
#ifdef SIM_TX
			case SIM_SENSOR:
				curSimulator = gcnew MSMSensor(timer_SimEvent, gcnew dRequestAckMsg(this,&ANTChannel::sendAckMsg));
				break;
#endif // SIM_TX
#ifdef SIM_RX
			case SIM_DISPLAY:
				curSimulator = gcnew MSMDisplay(gcnew dRequestAckMsg(this,&ANTChannel::sendAckMsg));
				break;
#endif // SIM_RX
			default:
				break;
		}
		break;
#endif //INC_MSM
#ifdef INC_LEV
	case LIGHT_ELECTRIC_VEHICLE:
		switch(ucSimType)
		{
#ifdef SIM_TX
			case SIM_SENSOR:
				curSimulator = gcnew LEVSensor(timer_SimEvent, gcnew dRequestAckMsg(this,&ANTChannel::sendAckMsg));
				break;
#endif // SIM_TX
#ifdef SIM_RX
			case SIM_DISPLAY:
				curSimulator = gcnew LEVDisplay(gcnew dRequestAckMsg(this,&ANTChannel::sendAckMsg), gcnew dRequestBcastMsg(this,&ANTChannel::sendBroadcastMsg));
				break;
#endif // SIM_RX
			default:
				break;
		}
		break;
#endif //INC_LEV
#ifdef INC_GEOCACHE
	case GEOCACHE:
		switch(ucSimType)
		{
#ifdef SIM_TX
			case SIM_SENSOR:
				curSimulator = gcnew GeocacheSensor(timer_SimEvent, gcnew dRequestAckMsg(this, &ANTChannel::sendAckMsg), gcnew dRequestUpdateMesgPeriod(this, &ANTChannel::updateMesgPeriod));
				break;
#endif	// SIM_TX
#ifdef SIM_RX
			case SIM_DISPLAY:
				curSimulator = gcnew GeocacheDisplay(timer_SimEvent, gcnew dRequestAckMsg(this, &ANTChannel::sendAckMsg), gcnew dRequestBcastMsg(this, &ANTChannel::sendBroadcastMsg));
				break;
#endif	// SIM_RX
			default:
				break;
		}
		break;
#endif	//INC_GEOCACHE
	
#ifdef INC_TEMPERATURE
	case TEMPERATURE:
		switch(ucSimType)
		{
#ifdef SIM_TX
			case SIM_SENSOR:
				curSimulator = gcnew TemperatureSensor(timer_SimEvent, gcnew dRequestUpdateMesgPeriod(this, &ANTChannel::updateMesgPeriod));
				break;
#endif // SIM_TX
#ifdef SIM_RX
			case SIM_DISPLAY:
				curSimulator = gcnew TemperatureDisplay(timer_SimEvent, gcnew dRequestAckMsg(this, &ANTChannel::sendAckMsg), gcnew dRequestUpdateMesgPeriod(this, &ANTChannel::updateMesgPeriod));
				break;
#endif // SIM_RX
			default:
				break;
		}
		break;
#endif // INC_TEMPERATURE
   
#ifdef INC_AUDIO
   case AUDIO:
      switch(ucSimType)
      {
#ifdef SIM_TX
   case SIM_SENSOR:
      curSimulator = gcnew AudioDevice(timer_SimEvent);
      break;
#endif // SIM_TX
#ifdef SIM_RX
   case SIM_DISPLAY:
      curSimulator = gcnew AudioRemote(timer_SimEvent, gcnew dRequestAckMsg(this, &ANTChannel::sendAckMsg));
      break;
#endif // SIM_RX
   default:
      break;
      }
      break;
#endif // INC_AUDIO
  
#ifdef INC_CUSTOM
	case CUSTOM:
		switch(ucSimType)
		{
#ifdef SIM_TX
			case SIM_SENSOR:
				curSimulator = gcnew CustomSensor(timer_SimEvent, gcnew dRequestAckMsg(this,&ANTChannel::sendAckMsg));
				break;
#endif // SIM_TX
#ifdef SIM_RX
			case SIM_DISPLAY:
				curSimulator = gcnew CustomDisplay(gcnew dRequestAckMsg(this,&ANTChannel::sendAckMsg));
				break;
#endif // SIM_RX
			default:
				break;
		}
		break;
#endif // INC_CUSTOM
	default:
		return;
	}

	// Obtain channel parameters from simulator
	ucMyDeviceType = curSimulator->getDeviceType();
	ucMyTxType = curSimulator->getTransmissionType();
	usMyMsgPeriod = curSimulator->getTransmitPeriod();
	
   // Update display of channel parameters
	this->textBox_DeviceType->Text = ucMyDeviceType.ToString();
	this->textBox_DeviceType->ReadOnly = TRUE;
	this->textBox_TransmissionType->Text = ucMyTxType.ToString();
	this->textBox_TransmitPeriod->Text = usMyMsgPeriod.ToString();
	this->textBox_TransmitPeriod->ReadOnly = TRUE;
	this->tabPage_ChannelTab->Controls->Add(curSimulator->getSimSettingsPanel());
	this->tabPage_ChannelTab->Controls->Add(curSimulator->getSimTranslatedDisplay());
	
   // Ensure channel is closed
	this->checkBox_TxEnable->Checked = FALSE;
	eMyChannelState = STATE_CHANNEL_CLOSED;
	if(ucSimType == SIM_SENSOR)
	{
		this->timer_SimEvent->Interval = curSimulator->getTimerInterval();
	}
}



/**************************************************************************
 * ANTChannel::checkBox_PairingOn_CheckedChange
 * 
 * Enables/disables pairing bit, per user input (GUI)
 *
 * returns: N/A
 *
 **************************************************************************/
System::Void ANTChannel::checkBox_PairingOn_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(this->checkBox_PairingOn->Checked)
	{
		bPairingBit = TRUE;
	}
	else
	{
		bPairingBit = FALSE;
	}

   SetPairingBit();
}


/**************************************************************************
 * ANTChannel::checkBox_DisplayAsHex_CheckedChanged
 * 
 * Selects whether data is displayed as hex or decimal, per user input (GUI)
 *
 * returns: N/A
 *
 **************************************************************************/
System::Void ANTChannel::checkBox_DisplayAsHex_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	if(this->checkBox_DisplayAsHex->Checked)
	{
		bDisplayHex = TRUE;
	}
	else
	{
		bDisplayHex = FALSE;
	}
}


/**************************************************************************
 * ANTChannel::button_getChannelID_Click
 * 
 * Requests channel ID
 *
 * returns: N/A
 *
 **************************************************************************/
System::Void ANTChannel::button_getChannelID_Click(System::Object^  sender, System::EventArgs^  e) {
	// Request Device Number of device we are pairing to, response is handled in main function that handles incoming events
	if(ucSimType == SIM_DISPLAY)
   { 
      ANTClass::RequestMessage(ucMyChannelNum, MESG_CHANNEL_ID_ID);
   }
}



/**************************************************************************
 * ValidateChannelID
 * 
 * Validates input boxes for Channel ID and Message Period
 * 
 * returns: TRUE if the parameters are valid
 *
 **************************************************************************/
BOOL ANTChannel::ValidateChannelID()
{
	BOOL bSuccess = TRUE;

	try
	{
		ucMyDeviceType = System::Convert::ToByte(this->textBox_DeviceType->Text);
		ucMyTxType = System::Convert::ToByte(this->textBox_TransmissionType->Text);
		usMyDeviceNum = System::Convert::ToUInt16(this->textBox_DeviceID->Text);
		usMyMsgPeriod = System::Convert::ToUInt16(this->textBox_TransmitPeriod->Text);
      SetPairingBit();
	}
	catch(...)
	{
		bSuccess = FALSE;
		//MessageBox::Show("Invalid Input", "Invalid Channel Parameters", MessageBoxButtons::OK,MessageBoxIcon::Error, MessageBoxDefaultButton::Button1);
	}

	return bSuccess;
}


/**************************************************************************
 * SetPairingBit
 * 
 * Sets/unsets pairing bit in Channel ID
 * 
 * returns: N/A
 *
 **************************************************************************/
void ANTChannel::SetPairingBit()
{
	if(bPairingBit)
      ucMyDeviceType  |= 0x80;	// set pairing bit
   else
      ucMyDeviceType &= ~0x80;   // clear pairing bit

   // Update text box
	//this->textBox_DeviceType->Text = System::Convert::ToString(ucMyDeviceType);
}

/**************************************************************************
 * OpenChannel
 * 
 * Initiates the opening of a channel by setting the proper state and sending
 * an assign channel command.
 *
 * Params:
 * 
 * ucChannelType_: ANT Channel type as either MASTER (0x10) or SLAVE (0x00)
 * 
 * returns: TRUE if the parameters are valid
 *
 **************************************************************************/
BOOL ANTChannel::OpenChannel(UCHAR ucChannelType_)
{	
   //!! Verify input parameters
   eMyChannelState = STATE_CHANNEL_OPENING;
	ANTClass::AssignChannel(ucMyChannelNum, ucChannelType_, ANTPLUS_NETWORK_NUMBER);
  
   return TRUE;
}


/**************************************************************************
 * ChannelOpenFailed
 * 
 * Called when opening of the ANT channel fails. Resets the proper state 
 * variables and displays error message.
 * 
 * returns: N/A
 *
 **************************************************************************/
void ANTChannel::ChannelOpenFailed()
{
   UpdateRawTxDisplay("\nChannel Setup Failed", NULL);
   bMyCheckInternalRaise = TRUE;
   checkBox_TxEnable->Checked = FALSE;
   checkBox_TxEnable->Enabled = TRUE;
   eMyChannelState = STATE_CHANNEL_CLOSED;    
}


/**************************************************************************
 * comboBox_TxPowerSelect_SelectedIndexChanged
 * 
 * User may change TxPower level if desired before opening channel.  If
 * device reports PER_CHANNEL_TX_POWER the SetChannelTXPower() command
 * is used affecting   .  Otherwise SetTransmitPower command is used and
 * all channel tabs must be updated.  
 *
 * returns: N/A
 *
 **************************************************************************/
System::Void ANTChannel::comboBox_TxPowerSelect_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
{
   UCHAR ucNewPower;

   ucNewPower = this->comboBox_TxPowerSelect->SelectedIndex;
   // Only generate commands in response to user events, not init or other programmatic changes to index
   if (bTXPowerUIEnabled == TRUE)
   {
      // Assert the new power
      //  Use Per Channel TX Power command if possible 
      if (sMyCapabilities.ucAdvancedOptions & CAPABILITIES_PER_CHANNEL_TX_POWER_ENABLED)
      {         
         ANTClass::SetChannelTxPower(ucMyChannelNum, ucNewPower);    
      }
      else
      {   
         ANTClass::SetTransmitPower(ucNewPower);         
         ANTChannel::iTheTXPowerLevel = ucNewPower;
      }
   }
}


/**************************************************************************
 * tabPage_ChannelTab_Enter
 * 
 * Update the TX Power dropdown if needed.  This could be caused if both the 
 * Channel TX Power Command is not supported and the user may have adjusted
 * the TX Power on another channel. 
 *
 * returns: N/A
 *
 **************************************************************************/

System::Void ANTChannel::tabPage_ChannelTab_Enter(System::Object^  sender, System::EventArgs^  e)
{
   // Are we using the global TX Power Command (=>per channel not supported)?
   if (~sMyCapabilities.ucAdvancedOptions & CAPABILITIES_PER_CHANNEL_TX_POWER_ENABLED)
   {
      // Has another channel changed the power level?
      if (this->comboBox_TxPowerSelect->SelectedIndex != ANTChannel::iTheTXPowerLevel)
      {
         // Disable the TX Power Select Dropdown handler and update the index
         bTXPowerUIEnabled = FALSE;
         this->comboBox_TxPowerSelect->SelectedIndex = ANTChannel::iTheTXPowerLevel;
         bTXPowerUIEnabled = TRUE;
      }
   }
}