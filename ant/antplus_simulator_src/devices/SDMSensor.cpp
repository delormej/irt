/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/



#include "StdAfx.h"
#include "SDMSensor.h"

/**************************************************************************
 * SDMSensor::ANT_eventNotification
 * 
 * Process ANT channel event
 *
 * ucEventCode_: code of ANT channel event
 * pucEventBuffer_: pointer to buffer containing data received from ANT,
 *		or a pointer to the transmit buffer in the case of an EVENT_TX
 * 
 * returns: N/A
 *
 **************************************************************************/
void SDMSensor::ANT_eventNotification(UCHAR ucEventCode_, UCHAR* pucEventBuffer_)
{
	switch(ucEventCode_)
	{
	case EVENT_TX:
		HandleTransmit((UCHAR*) pucEventBuffer_);
		UpdateDisplay();
		break;		
	case EVENT_RX_ACKNOWLEDGED:						// added for receiving request pages
		HandleReceive((UCHAR*) pucEventBuffer_);	
		break;
	case EVENT_TRANSFER_TX_COMPLETED:	// For sending ACK request replies
		ucAckRetryCount = 0;			// Reset retransmission counter
		ucMsgExpectingAck = 0;			// Clear pending msg code		
		break;
	case EVENT_TRANSFER_TX_FAILED:	
	case EVENT_ACK_TIMEOUT:				// Intentional fall thru
		if(ucMsgExpectingAck)
			HandleRetransmit();			// resend if it failed
		break;
	default:
		break;
	}
}


/**************************************************************************
 * SDMSensor::InitializeSim
 * 
 * Initializes simulator variables
 * 
 * returns: N/A
 *
 **************************************************************************/
void SDMSensor::InitializeSim()
{	
	// Time info
	ulTimerInterval = 667; //cadence of 90
	ulRunTime = 0;
	dwTimeLastEvent = GetTickCount();	// Initialize timers for latency calculation
	dwTimeLastTx = dwTimeLastEvent;

	// Common pages (data initialized on UI)
	commonPages->ulSerialNum = System::Convert::ToUInt32(this->textBox_SerialNum->Text);
	commonPages->usModelNum = System::Convert::ToUInt16(this->textBox_ModelNum->Text);
	commonPages->usMfgID = System::Convert::ToUInt16(this->textBox_MfgID->Text);	
	commonPages->ucHwVersion = System::Convert::ToByte(this->textBox_HwVersion->Text);
	commonPages->ucSwVersion = System::Convert::ToByte(this->textBox_SwVersion->Text);
	// Options
	ucSimDataType = SIM_FIXED;
	bSweepAscending = TRUE;
	bTxCommon = TRUE;	// Enable common pages (required on new designs)
	bTxPage2 = TRUE;	// Enable page 2 and all fields by default
	bTxPage3 = FALSE;	// Disable Page 3 by default
	bTxPage16 = TRUE;	// Enable Request Pages by default
	bTxPage22 = TRUE;	// Enable Request Pages by default	

	bTxPage16Flag =	FALSE;	// This flag is raised when a request to send Page 16 data is received	
	bTxPage22Flag = FALSE;	// This flag is raised when a request to send Page 22 data is received
	ucRequestCount = 0;		
	bP22Ignore = FALSE;
	ucMsgExpectingAck = 0;
	ucMaxAckRetries = 127;	// 127 is the default max number of times to attempt to resend an ack message

	bUseTime = TRUE;	
	bUseDistance = TRUE;
	bUseLatency = TRUE;
	bUseSpeed = TRUE;
	bUseCadence = TRUE;
	eUnit = SDM_SPM;		// user input is cadence (rpm)
	// Simulator data
	ucStrideLength = System::Convert::ToByte(this->numericUpDown_Sim_StrideLength->Value);
	ucCalorieRate = System::Convert::ToByte(this->numericUpDown_Sim_BurnRate->Value);
	this->listBox_SimUnits->SelectedIndex = 3;	// Use rpm by default
	this->listBox_SimUnits->SelectedIndexChanged += gcnew System::EventHandler(this, &SDMSensor::listBox_SimUnitsChanged);
	usMaxCadence16 = ConvertFromInput(this->numericUpDown_Sim_MaxOutput->Value);	// Initial cadence value set on UI
	usMinCadence16 = ConvertFromInput(this->numericUpDown_Sim_MinOutput->Value); // Initial cadence value set on UI
	usCurCadence16 = ConvertFromInput(this->numericUpDown_Sim_CurOutput->Value);	// Initial cadence value set on UI
	usMaxSpeed256 = CadenceToSpeed(usMaxCadence16);
	usMinSpeed256 = CadenceToSpeed(usMinCadence16);
	usCurSpeed256 = CadenceToSpeed(usCurCadence16);
	usCadence16 = usCurCadence16;
	usSpeed256 = usCurSpeed256;
	ucStrideCount = 0;
	ucCalorieCount = 0;
	ulTempCalories = 0;
	ulStridesAccum = 0;
	ulDistanceAccum = 0;	
	usDistance16 = 0;
	usTime200 = 0;
	ucLatency32 = 0;
	// Initialize status flags
	sStatusFlags.ucLocation = (UCHAR) 0;
	sStatusFlags.ucBatteryStatus = (UCHAR) 0;
	sStatusFlags.ucHealth = (UCHAR) 0;
	sStatusFlags.ucUseState = (UCHAR) 1;
	this->listBox_P2Location->SelectedIndex = 0;
	this->listBox_P2Battery->SelectedIndex = 0;
	this->listBox_P2Health->SelectedIndex = 0;
	this->listBox_P2Use->SelectedIndex = 1;
	// Initialize capabilities flags
	sCapabFlags.ucCadence = (UCHAR) 1;
	sCapabFlags.ucCalories = (UCHAR) 0;
	sCapabFlags.ucDistance = (UCHAR) 1;
	sCapabFlags.ucLatency = (UCHAR) 1;
	sCapabFlags.ucSpeed = (UCHAR) 1;
	sCapabFlags.ucTime = (UCHAR) 1;
	ucCapabFlags = SDM_CAPAB_INITIAL;		// initialize flags based on the values above, very important to do this
	this->label_P22Cadence->ForeColor = System::Drawing::Color::Green;
	this->label_P22Calories->ForeColor = System::Drawing::Color::Red;
	this->label_P22Distance->ForeColor = System::Drawing::Color::Green;
	this->label_P22Latency->ForeColor = System::Drawing::Color::Green;
	this->label_P22Speed->ForeColor = System::Drawing::Color::Green;
	this->label_P22Time->ForeColor = System::Drawing::Color::Green;
}

/**************************************************************************
 * SDMSensor::HandleTransmit
 * 
 * Encode data generated by simulator for transmission
 *
 * pucTxBuffer_: pointer to the transmit buffer
 * 
 * returns: N/A
 *
 **************************************************************************/
void SDMSensor::HandleTransmit(UCHAR* pucTxBuffer_)
{
	static UCHAR ucMessageCount = 0;	
	static UCHAR ucExtMesgType = 0;
	UCHAR ucPageNum;

	// Update latency
	// Latency calculation is platform specific
	dwTimeLastTx = GetTickCount();	// Get current time for latency calculation
	ucLatency32 = (UCHAR) (((dwTimeLastTx - dwTimeLastEvent) << 2)/25);		// Convert from ms to 1/32 seconds 

	// Figure out what page to send
	ucPageNum = SDM_PAGE1;
	if(bTxPage2 || bTxPage3)
	{
		// Page 1 and 2 (or 3) alternate every 2 messages
		if(ucMessageCount & 0x02)
			if (bTxPage2)	
				ucPageNum = SDM_PAGE2;
			else
				ucPageNum = SDM_PAGE3;
	}

	// Send common pages every 65th message, 
	// Skip if we are sending request pages as this takes priority
	if(ucMessageCount++ >= (SDM_COMMON_INTERVAL - 1) && bTxCommon 
			&& (bTxPage16Flag == FALSE) && (bTxPage22Flag == FALSE))
	{  
		 // Always send two in a row    
		if(ucExtMesgType++ & 0x02)
			ucPageNum = CommonData::PAGE81;  
		 else
			 ucPageNum = CommonData::PAGE80;				
		ucMessageCount = 0;      
	}

	// Request pages only on request. This takes priority over all other messages.
	if (bTxPage16Flag == TRUE)   
	{
		ucPageNum = SDM_PAGE16;
		ucRequestCount--;		// subtract one from the resend count
		if (ucRequestCount == 0) // if count is zero, then reset 
			bTxPage16Flag = FALSE;
	}	
	if (bTxPage22Flag == TRUE)
	{
		ucPageNum = SDM_PAGE22;	
		ucRequestCount--;		// subtract one from the resend count
		if (ucRequestCount == 0) // if count is zero, then reset 
			bTxPage22Flag = FALSE;
	}	

	// Set page number
	pucTxBuffer_[0] = ucPageNum;

	// Encode each page
	switch(ucPageNum)
	{
		case SDM_PAGE1:
			pucTxBuffer_[1] = (usTime200 % 200) * bUseTime;					// fractional time (1/200 seconds)
			pucTxBuffer_[2] = (UCHAR) (usTime200/200 * bUseTime);			// time of last event (seconds)
			pucTxBuffer_[3] = (usDistance16 * bUseDistance >> 4) & 0xFF;	// cumulative distance (meters)  
			pucTxBuffer_[4] = (usDistance16 * bUseDistance << 4) & 0xF0;	// fractional distance (1/16 meters)
			pucTxBuffer_[4] |= ((usSpeed256 * bUseSpeed >> 8) & 0x0F);		// current speed (m/s)	- if enabled
			pucTxBuffer_[5] = usSpeed256 * bUseSpeed & 0xFF;				// fractional speed (1/256 m/s)	- if enabled
			pucTxBuffer_[6] = ucStrideCount & 0xFF;							// cumulative stride count (number of strides)
			pucTxBuffer_[7] = ucLatency32 * bUseLatency;					// latency (1/32 seconds) - if enabled
			break;
		case SDM_PAGE2:
			pucTxBuffer_[1] = SDM_RESERVED;
			pucTxBuffer_[2] = SDM_RESERVED;
			pucTxBuffer_[3] = (usCadence16 * bUseCadence >> 4) & 0xFF;		// current cadence (strides/min)
			pucTxBuffer_[4] = (usCadence16 * bUseCadence << 4) & 0xF0;		// fractional cadence (1/16 strides/min)
			pucTxBuffer_[4] |= (usSpeed256 * bUseSpeed >> 8) & 0x0F;		// current speed (m/s)
			pucTxBuffer_[5] = usSpeed256 * bUseSpeed & 0xFF;				// fractional speed (1/256 m/s)
			pucTxBuffer_[6] = SDM_RESERVED;
			pucTxBuffer_[7] = ucStatusFlags;								// SDM status flags
			break;
		case SDM_PAGE3:
			pucTxBuffer_[1] = SDM_RESERVED;
			pucTxBuffer_[2] = SDM_RESERVED;
			pucTxBuffer_[3] = (usCadence16 * bUseCadence >> 4) & 0xFF;		// current cadence (strides/min)
			pucTxBuffer_[4] = (usCadence16 * bUseCadence << 4) & 0xF0;		// fractional cadence (1/16 strides/min)
			pucTxBuffer_[4] |= (usSpeed256 * bUseSpeed >> 8) & 0x0F;		// current speed (m/s)
			pucTxBuffer_[5] = usSpeed256 * bUseSpeed & 0xFF;				// fractional speed (1/256 m/s)
			pucTxBuffer_[6] = ucCalorieCount & 0xFF;						// cumulative calorie count
			pucTxBuffer_[7] = ucStatusFlags;								// SDM status flags
			break;
		case SDM_PAGE16:		// on request only			
			pucTxBuffer_[1] = ulStridesAccum & 0xFF;						// Bytes 1-3 = Stride Count
			pucTxBuffer_[2] = (ulStridesAccum >> 8) & 0xFF;
			pucTxBuffer_[3] = (ulStridesAccum >> 16) & 0xFF;
			pucTxBuffer_[4] = ulDistanceAccum & 0xFF;						// Bytes 4-7 = Distance
			pucTxBuffer_[5] = (ulDistanceAccum >> 8) & 0xFF;		
			pucTxBuffer_[6] = (ulDistanceAccum >> 16) & 0xFF;
			pucTxBuffer_[7] = (ulDistanceAccum >> 24) & 0xFF;
			break;
		case SDM_PAGE22:		// on request only
			pucTxBuffer_[1] = ucCapabFlags;									// Capabilities Byte									
			pucTxBuffer_[2] = SDM_RESERVED;									// Bytes 2-7 are reserved
			pucTxBuffer_[3] = SDM_RESERVED;
			pucTxBuffer_[4] = SDM_RESERVED;			
			pucTxBuffer_[5] = SDM_RESERVED;
			pucTxBuffer_[6] = SDM_RESERVED;
			pucTxBuffer_[7] = SDM_RESERVED;
			break;				
		case CommonData::PAGE80:
		case CommonData::PAGE81:
			try
			{
				commonPages->Encode(ucPageNum, pucTxBuffer_);
			}
			catch(CommonData::Error^ errorCommon)
			{
			}
			break;
		default:
			break;
	}	
}


/**************************************************************************
 * SDMSensor::HandleReceive
 * 
 * Decode incoming transmissions, specifically data requests
 *
 * returns: N/A
 *
 **************************************************************************/
void SDMSensor::HandleReceive(UCHAR* pucRxBuffer_)
{
	UCHAR ucPageNum = pucRxBuffer_[0];
	UCHAR ucRequestNum = pucRxBuffer_[6];	
	BOOL bP22Skip = FALSE;

	// if all the flags are on AND we are chosing to ignore the request if this happens, set the flag
	if ((bP22Ignore == TRUE) && (ucCapabFlags == SDM_CAPAB_ALL))   
		bP22Skip = TRUE;

	if(ucPageNum == SDM_PAGE70)		// Only decode request pages
	{		
		if ((ucRequestNum == SDM_PAGE16) && (bTxPage16 == TRUE))  // only process request if page is enabled
		{
			ucRequestCount	= pucRxBuffer_[5];	// save the number of times to re-transmit
			if (ucRequestCount == 0x80)			// 0x80 is a special case where we want to resend ack messages until a response is recieved
			{				
				ucMaxAckRetries = 127;			// reset to default in case this was changed
				SendAckRequestMsg(SDM_PAGE16);  // will automatically attempt to re-send until ack
			}
			else if ((ucRequestCount & 0x80) == 0x80)    // if MSB is set we want to use ACK response, the number in bits 0-6 becomes the maximum number of retries
			{				
				ucMaxAckRetries = ucRequestCount &= ~0x80;	// clear the MSB, save the remaining number as max retries
				SendAckRequestMsg(SDM_PAGE16);				
			}
			else // must be a regular broadcast request
			{
				// set a flag that will insert the page into the regular broadcast stream
				bTxPage16Flag = TRUE;
				bTxPage22Flag = FALSE;  // new requests will overwrite the last request
			}			
		}

		if ((ucRequestNum == SDM_PAGE22) && (bTxPage22 == TRUE) && (bP22Skip == FALSE))  // only process request if page is enabled and we are not skipping
		{
			ucRequestCount	= pucRxBuffer_[5];	// save the number of times to re-transmit
			if (ucRequestCount == 0x80)			// 0x80 is a special case where we want to resend ack messages until a response is recieved
			{				
				ucMaxAckRetries = 127;			// reset to default in case this was changed
				SendAckRequestMsg(SDM_PAGE22);  // will automatically attempt to re-send until ack
			}
			else if ((ucRequestCount & 0x80) == 0x80)    // if MSB is set we want to use ACK response, the number in bits 0-6 becomes the maximum number of retries
			{				
				ucMaxAckRetries = ucRequestCount &= ~0x80;	// clear the MSB, save the remaining number as max retries
				SendAckRequestMsg(SDM_PAGE22);				
			}
			else // must be a regular broadcast request
			{
				// set a flag that will insert the page into the regular broadcast stream
				bTxPage22Flag = TRUE;
				bTxPage16Flag = FALSE;  // new requests will overwrite the last request
			}			
		}
	}
}


/**************************************************************************
 * SDMSensor::HandleRetransmit
 * 
 * Retransmits request message, up to the maximum retransmission number,
 * ucMaxAckRetries, which defaults to 127, but is modified on request
 * 
 * returns: TRUE if message was retransmitted
 *          FALSE if maximum number of retransmissions was reached
 *
 **************************************************************************/
BOOL SDMSensor::HandleRetransmit()
{
	BOOL bSuccess = TRUE;

	if(ucMsgExpectingAck)	// Message still expecting an ack
	{
		if(ucAckRetryCount++ < ucMaxAckRetries)
			SendAckRequestMsg(ucMsgExpectingAck);
		else
			bSuccess = FALSE;
	}
	
	return bSuccess;
}


/**************************************************************************
 * SDMSensor::SendAckRequestMsg
 * 
 * Reply to the request message using an acknoledged message
 *
 * ucMsgCode_: message ID of the calibration message to send
 * Supported messages:  
 * - Page 16 Request (Distance and Strides)
 * - Page 22 Request (Capabilities)
 * 
 * returns: N/A
 *
 **************************************************************************/
void SDMSensor::SendAckRequestMsg(UCHAR ucMsgCode_) 
{
	UCHAR aucAckBuffer[8] = {0,0,0,0,0,0,0,0};

	switch(ucMsgCode_)
	{
		case SDM_PAGE16:	// Page 16 Request (Distance and Strides)
			aucAckBuffer[0] = SDM_PAGE16;
			aucAckBuffer[1] = ulStridesAccum & 0xFF;						// Bytes 1-3 = Stride Count
			aucAckBuffer[2] = (ulStridesAccum >> 8) & 0xFF;
			aucAckBuffer[3] = (ulStridesAccum >> 16) & 0xFF;
			aucAckBuffer[4] = ulDistanceAccum & 0xFF;						// Bytes 4-7 = Distance
			aucAckBuffer[5] = (ulDistanceAccum >> 8) & 0xFF;		
			aucAckBuffer[6] = (ulDistanceAccum >> 16) & 0xFF;
			aucAckBuffer[7] = (ulDistanceAccum >> 24) & 0xFF;
			break;
		case SDM_PAGE22:	// Page 22 Request (Capabilities)
			aucAckBuffer[0] = SDM_PAGE22;
			aucAckBuffer[1] = ucCapabFlags;									// Capabilities Byte									
			aucAckBuffer[2] = SDM_RESERVED;									// Bytes 2-7 are reserved
			aucAckBuffer[3] = SDM_RESERVED;
			aucAckBuffer[4] = SDM_RESERVED;			
			aucAckBuffer[5] = SDM_RESERVED;
			aucAckBuffer[6] = SDM_RESERVED;
			aucAckBuffer[7] = SDM_RESERVED;
			break;		
		default:
			return;		// Do not transmit if unsupported message code
	}

	// Send acknowledged message
	if(!ucMsgExpectingAck)	// If this message is not a retransmission
	{
		ucAckRetryCount = 0;			// Reset retransmission counter
		ucMsgExpectingAck = ucMsgCode_;	// Set code to message for which an ACK wil be expected
	}

	requestAckMsg(aucAckBuffer);	// Send message	
}



/**************************************************************************
 * SDMSensor::onTimerTock
 * 
 * Simulates a device event, updating simulator data based on this event
 * Modifications to the timer interval are applied immediately after this
 * at ANTChannel
 *
 * usEventTime_: current time (ms)
 *
 * returns: N/A
 *
 **************************************************************************/
void SDMSensor::onTimerTock(USHORT eventTime)
{	
	USHORT tempOffset = 0;				// Temporary variable to calculate sweeping intervals
	dwTimeLastEvent = GetTickCount();	// Get current time in ms (since system started)
	
	// Update stride count (required)
	ucStrideCount++;
	
	// Update Accumulative stride count once every two steps	
	ulStridesAccum++;
	if (ulStridesAccum == 16777216)   // 16777215 = 0xFFFFFF
		ulStridesAccum = 0;		

	// Update event time
	ulRunTime += ulTimerInterval;	
	usTime200 = (USHORT) ((ulRunTime%256000)/5);	// ms -> 1/200s,  rollover at 256 seconds

	// Update speed
	switch(ucSimDataType)
	{
	case SIM_FIXED:
		// Speed and cadence values do not change
		break;
	case SIM_SWEEP:
		// Value sweeps between max and min
		// The jump offset is calculated versus position against the max so it won't get stuck on low values for a long time and won't speed through high values too fast
		// Calculate next speed value
		tempOffset = usMaxSpeed256-usCurSpeed256;
		tempOffset = ((tempOffset & 0x600) >> 6) + ((tempOffset & 0x100) >>5) + ((tempOffset & 0x80) >>4)+0xF;
		if(bSweepAscending)
			usCurSpeed256 += tempOffset;
		else
			usCurSpeed256 -= tempOffset;
		// Ensure value is more than min and less than max
		if(usCurSpeed256 >= usMaxSpeed256)
		{
			usCurSpeed256 = usMaxSpeed256;
			bSweepAscending = FALSE;
		}
		if(usCurSpeed256 <= usMinSpeed256)
		{
			usCurSpeed256 = usMinSpeed256;
			bSweepAscending = TRUE;
		}
		// Update cadence according to new speed
		usCurCadence16 = SpeedToCadence(usCurSpeed256);
		break;
	default:
		break;
	}
	usSpeed256 = usCurSpeed256;	
	usCadence16 = usCurCadence16;

	// Calculate distance
	usDistance16 += ((USHORT) ucStrideLength * 16)/100;	// Stride length cm -> 1/16m

	// Update Accumulative distance
	ulDistanceAccum += ((ULONG) ucStrideLength * 256)/100;	// Stride length cm -> 1/256m

	// Update the Calorie count (burn rate (K-kcal/m) * distance (1/16m))		
	ulTempCalories += (ULONG)(ucCalorieRate*((ucStrideLength * 16)/100));  // K-kcal	
	ucCalorieCount = (UCHAR)((ulTempCalories/1000)/16);  // convert to UCHAR and kcal	

	// Update timer interval (in ms)
	if(usCadence16)
		ulTimerInterval = (ULONG) 960000/usCadence16;		// 60 seconds/revolutions per minute (60 * 1000 * 16)
	else
		ulTimerInterval = 600000;	// Cadence & speed = 0, so timer interval is set to a very large value
	
}

/***************************************************************************
 * SDMSensor::UpdateDisplay
 * 
 * Updates displayed simulator data on GUI 
 *
 * returns: N/A
 *
 **************************************************************************/
void SDMSensor::UpdateDisplay()
{
	this->label_TransStrideCountDisplay->Text = ucStrideCount.ToString();		// Display stride count (required)
	this->label_P1StrideCountDisplay->Text = ucStrideCount.ToString();

	if(bUseTime)
		this->label_Trn_TimeDisplay->Text = ((double)usTime200/200).ToString();	// Display time in seconds
	else
		this->label_Trn_TimeDisplay->Text = "--";	// Unused

	if(bUseSpeed)
		this->label_TransInstSpeedDisplay->Text = ((double)(usSpeed256 & 0xFFF)/256).ToString();	// Display speed in m/s
	else
		this->label_TransInstSpeedDisplay->Text = "--"; // Unused

	if(bUseCadence && (bTxPage2 || bTxPage3))
		this->label_TransCadenceDisplay->Text = ((double)usCadence16/16).ToString(); // Display cadence in rpm
	else
		this->label_TransCadenceDisplay->Text = "--"; // Unused

	if(bUseDistance)
	{
		this->label_TransDistDisplay->Text = ((double)(usDistance16 & 0xFFF)/16).ToString();	// Display distance in m
		this->label_P1TotDistDisplay->Text = ((double)(usDistance16 & 0xFFF)/16).ToString();
	}
	else
	{
		this->label_TransDistDisplay->Text = "--";	// Unused
		this->label_P1TotDistDisplay->Text = "--";
	}

	if(bUseLatency)
		this->label_TransLatencyDisplay->Text = System::Convert::ToString((double)ucLatency32*1000/32);	// Display latency in ms
	else
		this->label_TransLatencyDisplay->Text = "--";

	if(ucSimDataType == SIM_SWEEP)
	{
		if(eUnit == SDM_SPM) // Show sweeping values
			this->numericUpDown_Sim_CurOutput->Value = ConvertToInput(usCurCadence16);	// Cadence
		else
			this->numericUpDown_Sim_CurOutput->Value = ConvertToInput(usCurSpeed256 & 0xFFF); // Speed
	}

	if(bTxPage3) // calories
	{
		this->label_P3CalDisplay->Text = ucCalorieCount.ToString();
		this->label_TransCalDisplay->Text = ucCalorieCount.ToString();
	}
	else
	{
		this->label_P3CalDisplay->Text = "--";		
		this->label_TransCalDisplay->Text = "--";	
	}

	if(bTxPage16)
	{
		this->label_P16StridesDisplay->Text = ulStridesAccum.ToString();  //mask off the top byte
		this->label_P16DistanceDisplay->Text = ulDistanceAccum.ToString();
	}
	else
	{
		this->label_P16StridesDisplay->Text = "--";		
		this->label_P16DistanceDisplay->Text = "--";		
	}
}


/**************************************************************************
 * SDMSensor::button_UpdateCommon_Click
 * 
 * Validates and updates product information, from user input (GUI)
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void SDMSensor::button_UpdateCommon_Click(System::Object^  sender, System::EventArgs^  e) 
{
	label_ErrorCommon->Visible = false;
	label_ErrorCommon->Text = "Error: ";
	// Convert and catch failed conversions
	try{
		commonPages->usMfgID = System::Convert::ToUInt16(this->textBox_MfgID->Text);
	}
	catch(...){
		label_ErrorCommon->Text = System::String::Concat(label_ErrorCommon->Text, " MFID");
		label_ErrorCommon->Visible = true;
	}
	try{
		if(!this->checkBox_InvalidSerial->Checked)
			commonPages->ulSerialNum = System::Convert::ToUInt32(this->textBox_SerialNum->Text);
		else
			commonPages->ulSerialNum = 0xFFFFFFFF;
	}
	catch(...){
		label_ErrorCommon->Text = System::String::Concat(label_ErrorCommon->Text, " Ser#");
		label_ErrorCommon->Visible = true;
	}
	try{
		commonPages->ucHwVersion = System::Convert::ToByte(this->textBox_HwVersion->Text);
	}
	catch(...){
		label_ErrorCommon->Text = System::String::Concat(label_ErrorCommon->Text, " HWVr");
		label_ErrorCommon->Visible = true;
	}
	try{
		commonPages->ucSwVersion = System::Convert::ToByte(this->textBox_SwVersion->Text);
	}
	catch(...){
		label_ErrorCommon->Text = System::String::Concat(label_ErrorCommon->Text, " SWVr");
		label_ErrorCommon->Visible = true;
	}
	try{
		commonPages->usModelNum = System::Convert::ToUInt16(this->textBox_ModelNum->Text);
	}
	catch(...){
		label_ErrorCommon->Text = System::String::Concat(label_ErrorCommon->Text, " Mdl#");
		label_ErrorCommon->Visible = true;
	}
}


/**************************************************************************
 * SDMSensor::checkBox_EnableCommon_CheckedChanged
 * 
 * Enable/disable transmission of common pages
 *
 * returns: N/A
 *
 **************************************************************************/
System::Void SDMSensor::checkBox_EnableCommon_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	// Enable disabling common data pages for testing backward compatibility of receivers
	// Do NOT use as a reference for new designs
	if(!checkBox_EnableCommon->Checked)
	{
		System::Windows::Forms::DialogResult result = MessageBox::Show(L"This option is available for backward compatibility testing.\nIt should not be used as a reference in the development of new sensors.", L"Warning", MessageBoxButtons::OKCancel);
		if( result == ::DialogResult::OK )
		{
			bTxCommon = FALSE;
		}
		else
		{
			bTxCommon = TRUE;
			checkBox_EnableCommon->Checked = TRUE;
		}
	}
	else
	{
		// Reenable common pages
		bTxCommon = TRUE;
	}
}


/**************************************************************************
 * SDMSensor::listBox_SimUnitsChanged
 * 
 * Select simulation units
 *
 * returns: N/A
 *
 **************************************************************************/
System::Void SDMSensor::listBox_SimUnitsChanged(System::Object^  sender, System::EventArgs^  e)
{
	System::Decimal decMaxValue;

	eUnit = (SDMUnit) this->listBox_SimUnits->SelectedIndex;

	// Remove limit, so that conversion can take place successfully (changed from 1000 to 2000)
	this->numericUpDown_Sim_MaxOutput->Maximum = 2000;
	this->numericUpDown_Sim_MinOutput->Maximum = 2000;
	this->numericUpDown_Sim_CurOutput->Maximum = 2000;
	// Disable boxes momentarily, to keep values from being checked and modified again as they change
	this->numericUpDown_Sim_CurOutput->Enabled = FALSE;
	this->numericUpDown_Sim_MaxOutput->Enabled = FALSE;
	this->numericUpDown_Sim_MinOutput->Enabled = FALSE;

	// Update input data
	if(eUnit == SDM_SPM)
	{
		// Selected cadence (switching from speed to cadence)
		// Latest speed and cadence values are maintained, just displayed in proper unit
		// Display values
		this->numericUpDown_Sim_MinOutput->Value = ConvertToInput(usMinCadence16);
		this->numericUpDown_Sim_CurOutput->Value = ConvertToInput(usCurCadence16);
		this->numericUpDown_Sim_MaxOutput->Value = ConvertToInput(usMaxCadence16);
		decMaxValue = ConvertToInput(SDM_MAX_CADENCE16);
		//MessageBox::Show("Current Cadence = " + usCurCadence16.ToString() + " Current dexMaxValue = " + decMaxValue.ToString(), "Speed to Cadence", MessageBoxButtons::OK,MessageBoxIcon::Error, MessageBoxDefaultButton::Button1);	
	}
	else
	{
		// Selected a speed unit (switching between speed units or from cadence)
		// Latest speed value is maintained, just displayed in the selected unit
		this->numericUpDown_Sim_MinOutput->Value = ConvertToInput(usMinSpeed256);
		this->numericUpDown_Sim_CurOutput->Value = ConvertToInput(usCurSpeed256);
		this->numericUpDown_Sim_MaxOutput->Value = ConvertToInput(usMaxSpeed256);
		decMaxValue = ConvertToInput(SDM_MAX_SPEED256);
		//MessageBox::Show("Current Cadence = " + usCurCadence16.ToString() + " Current dexMaxValue = " + decMaxValue.ToString(), "Cadence to Speed", MessageBoxButtons::OK,MessageBoxIcon::Error, MessageBoxDefaultButton::Button1);	
	}

	// Adjust maximum value of input boxes to prevent overflow of the speed/cadence fields
	this->numericUpDown_Sim_MaxOutput->Maximum = decMaxValue + (System::Decimal) 0.001;
	this->numericUpDown_Sim_MinOutput->Maximum = decMaxValue + (System::Decimal) 0.001;
	this->numericUpDown_Sim_CurOutput->Maximum = decMaxValue + (System::Decimal) 0.001;

	// Enable input boxes again so that they can be modified
	if(ucSimDataType == SIM_SWEEP)
	{
		this->numericUpDown_Sim_MaxOutput->Enabled = TRUE;
		this->numericUpDown_Sim_MinOutput->Enabled = TRUE;
	}
	else
	{
		this->numericUpDown_Sim_CurOutput->Enabled = TRUE;
	}

}


/**************************************************************************
 * SDMSensor::numericUpDown_Sim_CurOutput_ValueChanged
 * 
 * Updates and converts units of the current speed or cadence value, when it changes
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void SDMSensor::numericUpDown_Sim_CurOutput_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	// Ignore changes when sweeping
	if(this->numericUpDown_Sim_CurOutput->Enabled)
	{
		if(eUnit == SDM_SPM)
		{
			// User input is cadence
			// Update cadence value
			usCurCadence16 = ConvertFromInput(this->numericUpDown_Sim_CurOutput->Value);
			// Update speed from cadence
			usCurSpeed256 = CadenceToSpeed(usCurCadence16);
		}
		else
		{
			// User input is speed
			// Update speed value
			usCurSpeed256 = ConvertFromInput(this->numericUpDown_Sim_CurOutput->Value);
			// Update cadence from speed
			usCurCadence16 = SpeedToCadence(usCurSpeed256);
		}
		ForceUpdate();
	}	
}


/**************************************************************************
 * SDMSensor::numericUpDown_Sim_MinMaxOutput_ValueChanged
 * 
 * If the user has changed the min or max speed, validate that
 * minimum < current <  maximum
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void SDMSensor::numericUpDown_Sim_MinMaxOutput_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	USHORT usPrevCadence16 = usCurCadence16;
	USHORT usPrevSpeed256 = usCurSpeed256;

	// Check only if user changes values
	if(this->numericUpDown_Sim_MinOutput->Enabled && this->numericUpDown_Sim_MaxOutput->Enabled)
	{
		// Check min<max if in min/max mode, and force min<cur<max
		if(this->numericUpDown_Sim_MinOutput->Value < this->numericUpDown_Sim_MaxOutput->Value)
		{
			if(eUnit == SDM_SPM)
			{
				// User input is cadence
				usMinCadence16 = ConvertFromInput(this->numericUpDown_Sim_MinOutput->Value);
				usMaxCadence16 = ConvertFromInput(this->numericUpDown_Sim_MaxOutput->Value);
				if(usCurCadence16 > usMaxCadence16)
					usCurCadence16 = usMaxCadence16;
				else if(usCurCadence16 < usMinCadence16)
					usCurCadence16= usMinCadence16;
				// Update speed
				usMinSpeed256 = CadenceToSpeed(usMinCadence16);
				usMaxSpeed256 = CadenceToSpeed(usMaxCadence16);
				if(usCurCadence16 != usPrevCadence16)
				{
					this->numericUpDown_Sim_CurOutput->Value = ConvertToInput(usCurCadence16);
					usCurSpeed256 = CadenceToSpeed(usCurCadence16);
					ForceUpdate();					
				}
			}
			else
			{
				// User input is speed
				usMinSpeed256 = ConvertFromInput(this->numericUpDown_Sim_MinOutput->Value);
				usMaxSpeed256 = ConvertFromInput(this->numericUpDown_Sim_MaxOutput->Value);
				if(usCurSpeed256 > usMaxSpeed256)
					usCurSpeed256 = usMaxSpeed256;
				else if(usCurSpeed256 < usMinSpeed256)
					usCurSpeed256 = usMinSpeed256;
				// Update cadence
				usMinCadence16 = SpeedToCadence(usMinSpeed256);
				usMaxCadence16 = SpeedToCadence(usMaxSpeed256);
				if(usCurSpeed256 != usPrevSpeed256)
				{
					this->numericUpDown_Sim_CurOutput->Value = ConvertToInput(usCurSpeed256);				
					usCurCadence16 = SpeedToCadence(usCurSpeed256);
					ForceUpdate();
				}
			}
		}
		else
		{
			// If the values were invalid, set numeric values to last valid values
			if(eUnit == SDM_SPM)
			{
				// Cadence
				this->numericUpDown_Sim_MinOutput->Value = ConvertToInput(usMinCadence16);
				this->numericUpDown_Sim_MaxOutput->Value = ConvertToInput(usMaxCadence16);
			}
			else
			{
				// Speed
				this->numericUpDown_Sim_MinOutput->Value = ConvertToInput(usMinSpeed256);
				this->numericUpDown_Sim_MaxOutput->Value = ConvertToInput(usMaxSpeed256);
			}
		}
	}
}


/**************************************************************************
 * SDMSensor::checkBox_Sweeping_CheckedChanged
 * 
 * Select method to generate simulator data, from user input (GUI)
 *
 * returns: N/A
 *
 **************************************************************************/
System::Void SDMSensor::checkBox_Sweeping_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	this->numericUpDown_Sim_CurOutput->Enabled = !this->numericUpDown_Sim_CurOutput->Enabled;
	this->numericUpDown_Sim_MinOutput->Enabled = !this->numericUpDown_Sim_MinOutput->Enabled;
	this->numericUpDown_Sim_MaxOutput->Enabled = !this->numericUpDown_Sim_MaxOutput->Enabled;
	// If we allow the units to be changed while the transmission is sweeping it gets really messy
	this->listBox_SimUnits->Enabled = !this->listBox_SimUnits->Enabled;

	if(this->checkBox_Sweeping->Checked)
	{
		ucSimDataType = SIM_SWEEP;
		bSweepAscending = TRUE;
	}
	else
	{
		ucSimDataType = SIM_FIXED;
	}
}


/**************************************************************************
 * SDMSensor::button_UpdateDistance_Click
 * 
 * Validates and updates cumulative distance, from user input (GUI)
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void SDMSensor::button_UpdateDistance_Click(System::Object^  sender, System::EventArgs^  e) 
{
	this->label_P1UpdateError->Visible = false;
	//convert and catch failed conversions
	try{
		usDistance16 = 16 * (USHORT) System::Convert::ToByte(this->textBox_P1TotDistChange->Text);
		this->label_P1TotDistDisplay->Text = this->textBox_P1TotDistChange->Text;
	}
	catch(...){
		this->label_P1UpdateError->Text = "Error: Dist";
		this->label_P1UpdateError->Visible = true;
	}
}


/**************************************************************************
 * SDMSensor::button_UpdateStrides_Click
 * 
 * Validates and updates cumulative distance, from user input (GUI)
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void SDMSensor::button_UpdateStrides_Click(System::Object^  sender, System::EventArgs^  e)
{
	this->label_P1UpdateError->Visible = false;
	//convert and catch failed conversions
	try{
		ucStrideCount = System::Convert::ToByte(this->textBox_P1StrideCountChange->Text);
		this->label_P1StrideCountDisplay->Text = this->textBox_P1StrideCountChange->Text;
	}
	catch(...){
		this->label_P1UpdateError->Text = "Error: Stride";
		this->label_P1UpdateError->Visible = true;
	}
}


/**************************************************************************
 * SDMSensor::checkBox_P1InstSpeed_CheckedChanged
 * 
 * Enable/disable transmission of speed (optional field), and synchronizes 
 * option in Page 1, 2, and 3
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void SDMSensor::checkBox_P1InstSpeed_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	// Synchronize the three pages' inst speed options to avoid confusion 
	this->checkBox_P2InstSpeed->Checked = this->checkBox_P1InstSpeed->Checked;			
	this->checkBox_P3InstSpeed->Checked = this->checkBox_P1InstSpeed->Checked;				

	bUseSpeed = this->checkBox_P1InstSpeed->Checked;

	sCapabFlags.ucSpeed = (UCHAR) this->checkBox_P1InstSpeed->Checked;
	if (sCapabFlags.ucSpeed == 1)				// if latency is selected
	{
		ucCapabFlags |= SDM_SPEED_MASK;			// set the bit to 1
		this->label_P22Speed->ForeColor = System::Drawing::Color::Green;
	}
	else										// distance not selected
	{
		ucCapabFlags &= ~SDM_SPEED_MASK;		// set the bit to 0
		this->label_P22Speed->ForeColor = System::Drawing::Color::Red;
	}
}


/**************************************************************************
 * SDMSensor::checkBox_P2InstSpeed_CheckedChanged
 * 
 * Enable/disable transmission of speed (optional field), and synchronizes 
 * option in Page 1, 2, and 3
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void SDMSensor::checkBox_P2InstSpeed_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	// Synchronize the three pages' inst speed options to avoid confusion	
	this->checkBox_P1InstSpeed->Checked = this->checkBox_P2InstSpeed->Checked;			
	this->checkBox_P3InstSpeed->Checked = this->checkBox_P2InstSpeed->Checked;	

	bUseSpeed = this->checkBox_P2InstSpeed->Checked;

	sCapabFlags.ucSpeed = (UCHAR) this->checkBox_P2InstSpeed->Checked;
	if (sCapabFlags.ucSpeed == 1)				// if latency is selected
	{
		ucCapabFlags |= SDM_SPEED_MASK;			// set the bit to 1
		this->label_P22Speed->ForeColor = System::Drawing::Color::Green;
	}
	else										// distance not selected
	{
		ucCapabFlags &= ~SDM_SPEED_MASK;		// set the bit to 0
		this->label_P22Speed->ForeColor = System::Drawing::Color::Red;
	}
}


/**************************************************************************
 * SDMSensor::checkBox_P3InstSpeed_CheckedChanged
 * 
 * Enable/disable transmission of speed (optional field), and synchronizes 
 * option in Page 1, 2, and 3
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void SDMSensor::checkBox_P3InstSpeed_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	// Synchronize the three pages' inst speed options to avoid confusion		
	this->checkBox_P1InstSpeed->Checked = this->checkBox_P3InstSpeed->Checked;			
	this->checkBox_P2InstSpeed->Checked = this->checkBox_P3InstSpeed->Checked;	

	bUseSpeed = this->checkBox_P3InstSpeed->Checked;

	sCapabFlags.ucSpeed = (UCHAR) this->checkBox_P3InstSpeed->Checked;
	if (sCapabFlags.ucSpeed == 1)				// if latency is selected
	{
		ucCapabFlags |= SDM_SPEED_MASK;			// set the bit to 1
		this->label_P22Speed->ForeColor = System::Drawing::Color::Green;
	}
	else										// distance not selected
	{
		ucCapabFlags &= ~SDM_SPEED_MASK;		// set the bit to 0
		this->label_P22Speed->ForeColor = System::Drawing::Color::Red;
	}
}


/**************************************************************************
 * SDMSensor::checkBox_P1Time_CheckedChanged
 * 
 * Enable/disable transmission of time in Page 1 (optional field)
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void SDMSensor::checkBox_P1Time_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	bUseTime = !bUseTime;

	sCapabFlags.ucTime = (UCHAR) this->checkBox_P1Time->Checked;
	if (sCapabFlags.ucTime == 1)			// if time is selected
	{
		ucCapabFlags |= SDM_TIME_MASK;	// set the bit to 1
		this->label_P22Time->ForeColor = System::Drawing::Color::Green;
	}
	else									// time not selected
	{
		ucCapabFlags &= ~SDM_TIME_MASK;	// set the bit to 0
		this->label_P22Time->ForeColor = System::Drawing::Color::Red;
	}
}


/**************************************************************************
 * SDMSensor::checkBox_P1Latency_CheckedChanged
 * 
 * Enable/disable transmission of latency in Page 1 (optional field)
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void SDMSensor::checkBox_P1Latency_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	bUseLatency = !bUseLatency;

	sCapabFlags.ucLatency = (UCHAR) this->checkBox_P1Latency->Checked;
	if (sCapabFlags.ucLatency == 1)				// if latency is selected
	{
		ucCapabFlags |= SDM_LATENCY_MASK;		// set the bit to 1
		this->label_P22Latency->ForeColor = System::Drawing::Color::Green;
	}
	else										// distance not selected
	{
		ucCapabFlags &= ~SDM_LATENCY_MASK;		// set the bit to 0
		this->label_P22Latency->ForeColor = System::Drawing::Color::Red;
	}
}


/**************************************************************************
 * SDMSensor::checkBox_P1Distance_CheckedChanged
 * 
 * Enable/disable transmission of distance in Page 1 (optional field)
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void SDMSensor::checkBox_P1Distance_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	bUseDistance = !bUseDistance;

	sCapabFlags.ucDistance = (UCHAR) this->checkBox_P1Distance->Checked;
	if (sCapabFlags.ucDistance == 1)			// if distance is selected
	{
		ucCapabFlags |= SDM_DISTANCE_MASK;	// set the bit to 1
		this->label_P22Distance->ForeColor = System::Drawing::Color::Green;
	}
	else										// distance not selected
	{
		ucCapabFlags &= ~SDM_DISTANCE_MASK;	// set the bit to 0
		this->label_P22Distance->ForeColor = System::Drawing::Color::Red;
	}
}


/**************************************************************************
 * SDMSensor::checkBox_P2Cadence_CheckedChanged
 * 
 * Enable/disable transmission of cadence in Page 2 (optional field)
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void SDMSensor::checkBox_P2Cadence_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{		
	this->checkBox_P3Cadence->Checked = this->checkBox_P2Cadence->Checked;			
	bUseCadence = this->checkBox_P2Cadence->Checked;
	//bUseCadence = !bUseCadence; 

	sCapabFlags.ucCadence = (UCHAR) this->checkBox_P2Cadence->Checked;
	if (sCapabFlags.ucCadence == 1)				// if latency is selected
	{
		ucCapabFlags |= SDM_CADENCE_MASK;		// set the bit to 1
		this->label_P22Cadence->ForeColor = System::Drawing::Color::Green;
	}
	else										// distance not selected
	{
		ucCapabFlags &= ~SDM_CADENCE_MASK;		// set the bit to 0
		this->label_P22Cadence->ForeColor = System::Drawing::Color::Red;
	}
}


/**************************************************************************
 * SDMSensor::checkBox_P3Cadence_CheckedChanged
 * 
 * Enable/disable transmission of cadence in Page 3 (optional field)
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void SDMSensor::checkBox_P3Cadence_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	this->checkBox_P2Cadence->Checked = this->checkBox_P3Cadence->Checked;			
	bUseCadence = this->checkBox_P3Cadence->Checked;
	//bUseCadence = !bUseCadence;

	sCapabFlags.ucCadence = (UCHAR) this->checkBox_P3Cadence->Checked;
	if (sCapabFlags.ucCadence == 1)				// if latency is selected
	{
		ucCapabFlags |= SDM_CADENCE_MASK;		// set the bit to 1
		this->label_P22Cadence->ForeColor = System::Drawing::Color::Green;
	}
	else										// distance not selected
	{
		ucCapabFlags &= ~SDM_CADENCE_MASK;		// set the bit to 0
		this->label_P22Cadence->ForeColor = System::Drawing::Color::Red;
	}
}


/**************************************************************************
 * SDMSensor::numericUpDown_Sim_StrideLength_ValueChanged
 * 
 * Updates stride length when modified in UI
 *
 * returns: N/A
 *
 **************************************************************************/
System::Void SDMSensor::numericUpDown_Sim_StrideLength_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	ucStrideLength = System::Convert::ToByte(this->numericUpDown_Sim_StrideLength->Value);
	usMaxSpeed256 = CadenceToSpeed(usMaxCadence16);		// update the speed since it is based on stride length
	usMinSpeed256 = CadenceToSpeed(usMinCadence16);
	usCurSpeed256 = CadenceToSpeed(usCurCadence16);
}


/**************************************************************************
 * SDMSensor::numericUpDown_Sim_BurnRate_ValueChanged
 * 
 * Updates Calories Burn Rate when modified in UI
 *
 * returns: N/A
 *
 **************************************************************************/
System::Void SDMSensor::numericUpDown_Sim_BurnRate_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	ucCalorieRate = System::Convert::ToByte(this->numericUpDown_Sim_BurnRate->Value);
}


/**************************************************************************
 * SDMSensor::listBox_P2Location_SelectedIndexChanged
 * 
 * Updates location field in status flags
 *
 * returns: N/A
 *
 **************************************************************************/
System::Void SDMSensor::listBox_P2Location_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
{
	sStatusFlags.ucLocation = (UCHAR) this->listBox_P2Location->SelectedIndex;
	ucStatusFlags &= ~SDM_LOCATION_MASK;	// Clear previous status
	ucStatusFlags |= (sStatusFlags.ucLocation << SDM_LOCATION_SHIFT);	// Set updated status
}

/**************************************************************************
 * SDMSensor::listBox_P2Battery_SelectedIndexChanged
 * 
 * Updates battery status field in status flags
 *
 * returns: N/A
 *
 **************************************************************************/
System::Void SDMSensor::listBox_P2Battery_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
{
	sStatusFlags.ucBatteryStatus = (UCHAR) this->listBox_P2Battery->SelectedIndex;
	ucStatusFlags &= ~SDM_BAT_MASK;	// Clear previous status
	ucStatusFlags |= (sStatusFlags.ucBatteryStatus << SDM_BAT_SHIFT);	// Set updated status
}

/**************************************************************************
 * SDMSensor::listBox_P2Health_SelectedIndexChanged
 * 
 * Updates health field in status flags
 *
 * returns: N/A
 *
 **************************************************************************/
System::Void SDMSensor::listBox_P2Health_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
{
	sStatusFlags.ucHealth = (UCHAR) this->listBox_P2Health->SelectedIndex;
	ucStatusFlags &= ~SDM_HEALTH_MASK;	// Clear previous status
	ucStatusFlags |= (sStatusFlags.ucHealth << SDM_HEALTH_SHIFT);	// Set updated status
}

/**************************************************************************
 * SDMSensor::listBox_P2Use_SelectedIndexChanged
 * 
 * Updates use state field in status flags
 *
 * returns: N/A
 *
 **************************************************************************/
System::Void SDMSensor::listBox_P2Use_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
{
	sStatusFlags.ucUseState = (UCHAR) this->listBox_P2Use->SelectedIndex;
	ucStatusFlags &= ~SDM_STATE_MASK;	// Clear previous status
	ucStatusFlags |= (sStatusFlags.ucUseState << SDM_STATE_SHIFT);	// Set updated status
}


/**************************************************************************
 * SDMSensor::listBox_P3Location_SelectedIndexChanged
 * 
 * Updates location field in status flags
 *
 * returns: N/A
 *
 **************************************************************************/
System::Void SDMSensor::listBox_P3Location_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
{
	sStatusFlags.ucLocation = (UCHAR) this->listBox_P3Location->SelectedIndex;
	ucStatusFlags &= ~SDM_LOCATION_MASK;	// Clear previous status
	ucStatusFlags |= (sStatusFlags.ucLocation << SDM_LOCATION_SHIFT);	// Set updated status
}

/**************************************************************************
 * SDMSensor::listBox_P3Battery_SelectedIndexChanged
 * 
 * Updates battery status field in status flags
 *
 * returns: N/A
 *
 **************************************************************************/
System::Void SDMSensor::listBox_P3Battery_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
{
	sStatusFlags.ucBatteryStatus = (UCHAR) this->listBox_P3Battery->SelectedIndex;
	ucStatusFlags &= ~SDM_BAT_MASK;	// Clear previous status
	ucStatusFlags |= (sStatusFlags.ucBatteryStatus << SDM_BAT_SHIFT);	// Set updated status
}

/**************************************************************************
 * SDMSensor::listBox_P3Health_SelectedIndexChanged
 * 
 * Updates health field in status flags
 *
 * returns: N/A
 *
 **************************************************************************/
System::Void SDMSensor::listBox_P3Health_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
{
	sStatusFlags.ucHealth = (UCHAR) this->listBox_P3Health->SelectedIndex;
	ucStatusFlags &= ~SDM_HEALTH_MASK;	// Clear previous status
	ucStatusFlags |= (sStatusFlags.ucHealth << SDM_HEALTH_SHIFT);	// Set updated status
}

/**************************************************************************
 * SDMSensor::listBox_P3Use_SelectedIndexChanged
 * 
 * Updates use state field in status flags
 *
 * returns: N/A
 *
 **************************************************************************/
System::Void SDMSensor::listBox_P3Use_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
{
	sStatusFlags.ucUseState = (UCHAR) this->listBox_P3Use->SelectedIndex;
	ucStatusFlags &= ~SDM_STATE_MASK;	// Clear previous status
	ucStatusFlags |= (sStatusFlags.ucUseState << SDM_STATE_SHIFT);	// Set updated status
}


/**************************************************************************
 * SDMSensor::ConvertFromInput
 * 
 * Converts from user selected units for speed and cadence to units
 * defined in the device profile
 *
 * decValue_: Value to convert
 *
 * returns: speed (1/256 s) or cadence (strides/min)
 *
 **************************************************************************/
USHORT SDMSensor::ConvertFromInput(System::Decimal decValue_)
{
	USHORT usOutput;

	switch(eUnit)
	{
		case SDM_MS:	
			usOutput = (USHORT) (256 * decValue_);	// Convert speed from m/s to 1/256 m/s
			break;
		case SDM_KPH:
			usOutput = (USHORT) (2560 * decValue_/36);	// Convert speed from km/h to 1/256 m/s
			break;
		case SDM_MPH:
			usOutput = (USHORT) (411991 * decValue_/3600);	// Convert speed from mi/h to 1/256 m/s
			break;
		case SDM_SPM:
			usOutput = (USHORT) (16 * decValue_);	// Convert cadence from spm to 1/16 spm
			break;
		default:
			usOutput = SDM_UNUSED;
			break;
	}

	return usOutput;
}


/**************************************************************************
 * SDMSensor::ConvertToInput
 * 
 * Converts from units defined in device profile for speed and cadence
 * to the user selected units for display purposes (on text box)
 *
 * usValue_: Value to convert
 *
 * returns: speed or cadence, in selected unit
 *
 **************************************************************************/
System::Decimal SDMSensor::ConvertToInput(USHORT usValue_)
{	
	System::Decimal decInput;

	switch(eUnit)
	{
		case SDM_MS:	
			decInput = System::Convert::ToDecimal(usValue_ /256.0);	// Convert speed from 1/256 m/s to m/s
			break;
		case SDM_KPH:
			decInput = System::Convert::ToDecimal(usValue_ * 36/2560.0);	// Convert speed from 1/256 m/s to km/h
			break;
		case SDM_MPH:
			decInput = System::Convert::ToDecimal(usValue_ * 3600/411991.0);	// Convert speed from 1/256 m/s to mi/h
			break;
		case SDM_SPM:
			decInput = System::Convert::ToDecimal(usValue_/16.0);	// Convert cadence from 1/16 spm to spm
			break;
		default:
			decInput = System::Convert::ToDecimal(SDM_UNUSED);
			break;
	}

	return decInput;
}

/**************************************************************************
 * SDMSensor::CadenceToSpeed
 * 
 * Converts from cadence to speed
 *
 * usCadence16_: Cadence (1/16 rpm)
 *
 * returns: Speed (1/256 m/s)
 *
 **************************************************************************/
USHORT SDMSensor::CadenceToSpeed(USHORT usCadence16_)
{
	return (USHORT) (16 * (ULONG) ucStrideLength * (ULONG) usCadence16_  / (ULONG) 6000);  // 1/16 spm -> 1/256 m/s (stride length is in cm)
}


/**************************************************************************
 * SDMSensor::SpeedToCadence
 * 
 * Converts from cadence to speed
 *
 * usCadence16_: Cadence (1/16 rpm)
 *
 * returns: Speed (1/256 m/s)
 *
 **************************************************************************/
USHORT SDMSensor::SpeedToCadence(USHORT usSpeed256_)
{
	return (USHORT) ((ULONG) 6000 * usSpeed256_ / (16 * (ULONG) ucStrideLength));	// 1/256 m/s -> 1/16 spm (stride length is in cm)
}

/**************************************************************************
 * SDMSensor::ForceUpdate
 * 
 * Causes a timer event, to force the simulator to update all calculations
 * 
 * returns: N/A
 *
 **************************************************************************/
void SDMSensor::ForceUpdate()
{
	timerHandle->Interval = 250;
}

/**************************************************************************
 * SDMSensor::checkBox_Page2_CheckedChanged
 * 
 * Select whether to include Page 2 in transmission (Page 1 is always on)
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void SDMSensor::checkBox_Page2_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	if(this->checkBox_Page2->Checked)  // If checked, Enable Page 2, Disable Page 3
	{
		Page2_Enable();
		Page3_Disable();
		this->checkBox_Page3->Checked = FALSE;
	}
	else   // If not checked, Disable Page 2
	{
		Page2_Disable();
	}
}

/**************************************************************************
 * SDMSensor::checkBox_Page3_CheckedChanged
 * 
 * Select whether to include Page 3 in transmission
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void SDMSensor::checkBox_Page3_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	// Calories capability dependent on whether Page 3 is on, so store that capability
	sCapabFlags.ucCalories = (UCHAR) this->checkBox_Page3->Checked;

	if(this->checkBox_Page3->Checked)  // If checked, Enable Page 3, Disable Page 2
	{
		Page3_Enable();
		Page2_Disable();
		this->checkBox_Page2->Checked = FALSE;
		
		ucCapabFlags |= SDM_CALORIES_MASK;		// set the bit to 1
		this->label_P22Calories->ForeColor = System::Drawing::Color::Green;
	}
	else   // If not checked, Disable Page 3
	{		
		Page3_Disable();

		ucCapabFlags &= ~SDM_CALORIES_MASK;		// set the bit to 0
		this->label_P22Calories->ForeColor = System::Drawing::Color::Red;
	}
}


/**************************************************************************
 * SDMSensor::Page2_Enable
 * 
 * Enables Page 2 GUI
 * 
 * returns: N/A
 *
 **************************************************************************/
void SDMSensor::Page2_Enable()
{
	this->listBox_P2Battery->Enabled = TRUE;
	this->listBox_P2Health->Enabled = TRUE;
	this->listBox_P2Location->Enabled = TRUE;
	this->listBox_P2Use->Enabled = TRUE;
	this->checkBox_P2Cadence->Enabled = TRUE;
	this->checkBox_P2InstSpeed->Enabled = TRUE;	
	// synchronize Instant Speed and Flags in case anything was changed while the page was disabled	
	this->checkBox_P2InstSpeed->Checked = this->checkBox_P1InstSpeed->Checked;	
	this->listBox_P2Location->SelectedIndex = (int) sStatusFlags.ucLocation; 
	this->listBox_P2Battery->SelectedIndex = (int) sStatusFlags.ucBatteryStatus; 
	this->listBox_P2Health->SelectedIndex = (int) sStatusFlags.ucHealth; 
	this->listBox_P2Use->SelectedIndex = (int) sStatusFlags.ucUseState; 
	bTxPage2 = TRUE;		
}

/**************************************************************************
 * SDMSensor::Page2_Disable
 * 
 * Disables Page 2 GUI
 * 
 * returns: N/A
 *
 **************************************************************************/
void SDMSensor::Page2_Disable()
{
	//this->label_TransCadence->ForeColor = System::Drawing::SystemColors::GrayText;
	//this->label_TransCadenceDisplay->ForeColor = System::Drawing::SystemColors::GrayText;
	this->listBox_P2Battery->Enabled = FALSE;
	this->listBox_P2Health->Enabled = FALSE;
	this->listBox_P2Location->Enabled = FALSE;
	this->listBox_P2Use->Enabled = FALSE;
	this->checkBox_P2Cadence->Enabled = FALSE;
	this->checkBox_P2InstSpeed->Enabled = FALSE;
	bTxPage2 = FALSE;		
}

/**************************************************************************
 * SDMSensor::Page3_Enable
 * 
 * Enables Page 3 GUI
 * 
 * returns: N/A
 *
 **************************************************************************/
void SDMSensor::Page3_Enable()
{
	this->listBox_P3Battery->Enabled = TRUE;
	this->listBox_P3Health->Enabled = TRUE;
	this->listBox_P3Location->Enabled = TRUE;
	this->listBox_P3Use->Enabled = TRUE;
	this->checkBox_P3Cadence->Enabled = TRUE;
	this->checkBox_P3InstSpeed->Enabled = TRUE;	
	this->textBox_P3Calories->Enabled = TRUE;
	this->button_P3UpdateCalories->Enabled = TRUE;
	this->label_P3CalDisplay->Enabled = TRUE;
	// synchronize Instant Speed and Flags in case anything was changed while the page was disabled	
	this->checkBox_P3InstSpeed->Checked = this->checkBox_P1InstSpeed->Checked;
	this->listBox_P3Location->SelectedIndex = (int) sStatusFlags.ucLocation; 
	this->listBox_P3Battery->SelectedIndex = (int) sStatusFlags.ucBatteryStatus; 
	this->listBox_P3Health->SelectedIndex = (int) sStatusFlags.ucHealth; 
	this->listBox_P3Use->SelectedIndex = (int) sStatusFlags.ucUseState; 	
	bTxPage3 = TRUE;
}

/**************************************************************************
 * SDMSensor::Page3_Disable
 * 
 * Disables Page 3 GUI
 * 
 * returns: N/A
 *
 **************************************************************************/
void SDMSensor::Page3_Disable()
{
	this->listBox_P3Battery->Enabled = FALSE;
	this->listBox_P3Health->Enabled = FALSE;
	this->listBox_P3Location->Enabled = FALSE;
	this->listBox_P3Use->Enabled = FALSE;
	this->checkBox_P3Cadence->Enabled = FALSE;
	this->checkBox_P3InstSpeed->Enabled = FALSE;
	this->textBox_P3Calories->Enabled = FALSE;
	this->button_P3UpdateCalories->Enabled = FALSE;
	this->label_P3CalDisplay->Enabled = FALSE;
	bTxPage3 = FALSE;	
}


/**************************************************************************
 * SDMSensor::checkBox_P22Enabled_CheckedChanged
 * 
 * Select whether to allow Pagge 22 to be requestable
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void SDMSensor::checkBox_P22Enabled_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	if(this->checkBox_P22Enabled->Checked)		// If checked, Enable 
	{
		bTxPage22 = TRUE;
		// disable UI
		this->checkBox_P22Ignore->Enabled = TRUE;
		this->groupBox_P22Supported->Enabled = TRUE;	
	}
	else   // If not checked, Disable
	{		
		bTxPage22 = FALSE;
		// enable UI
		this->checkBox_P22Ignore->Enabled = FALSE;
		this->groupBox_P22Supported->Enabled = FALSE;			
	}
}


/**************************************************************************
 * SDMSensor::checkBox_P22Ignore_CheckedChanged
 * 
 * The device profile allows for sensors to ignore a Page 22 (Capabilities) Request
 * if the device supports all fields. This allows the user to either use this feature or not.
 *
 * returns: N/A
 *
 **************************************************************************/
System::Void SDMSensor::checkBox_P22Ignore_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{	
	if(this->checkBox_P22Ignore->Checked)		// If checked, Enable 
	{
		bP22Ignore = TRUE;		
	}
	else   // If not checked, Disable
	{		
		bP22Ignore = FALSE;
	}
}


/**************************************************************************
 * SDMSensor::checkBox_P16Enabled_CheckedChanged
 * 
 * Select whether to allow Pagge 16 to be requestable
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void SDMSensor::checkBox_P16Enabled_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	if(this->checkBox_P16Enabled->Checked)		// If checked, Enable 
	{
		bTxPage16 = TRUE;
		// disable UI
		this->label_P16Strides->Enabled = TRUE;
		this->label_P16Distance->Enabled = TRUE;
		this->label_P16StridesDisplay->Enabled = TRUE;
		this->label_P16DistanceDisplay->Enabled = TRUE;
		this->label_P16_UnitsDist->Enabled = TRUE;
		this->button_P16UpdateStrides->Enabled = TRUE;
		this->button_P16UpdateDistance->Enabled = TRUE;
		this->textBox_P16StridesDisplayChange->Enabled = TRUE;
		this->textBox_P16DistDisplayChange->Enabled = TRUE;
	}
	else   // If not checked, Disable
	{		
		bTxPage16 = FALSE;
		// enable UI
		this->label_P16Strides->Enabled = FALSE;
		this->label_P16Distance->Enabled = FALSE;
		this->label_P16StridesDisplay->Enabled = FALSE;
		this->label_P16DistanceDisplay->Enabled = FALSE;
		this->label_P16_UnitsDist->Enabled = FALSE;
		this->button_P16UpdateStrides->Enabled = FALSE;
		this->button_P16UpdateDistance->Enabled = FALSE;
		this->textBox_P16StridesDisplayChange->Enabled = FALSE;
		this->textBox_P16DistDisplayChange->Enabled = FALSE;					
	}
}


/**************************************************************************
 * SDMSensor::button_P3UpdateCalories_Click
 * 
 * Validates and updates cumulative Calories on Page 3, from user input (GUI)
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void SDMSensor::button_P3UpdateCalories_Click(System::Object^  sender, System::EventArgs^  e) 
{	    
	//convert and catch failed conversions
	try{
		ucCalorieCount = System::Convert::ToByte(this->textBox_P3Calories->Text);
		this->label_P3CalDisplay->Text = this->textBox_P3Calories->Text;
		ulTempCalories = ucCalorieCount * 1000 * 16;
	}
	catch(...){
		MessageBox::Show("Invalid Input. Please enter a value from 0 to 255.", "Error", MessageBoxButtons::OK,MessageBoxIcon::Error, MessageBoxDefaultButton::Button1);	
	}	
}


/**************************************************************************
 * SDMSensor::button_P16UpdateStrides_Click
 * 
 * Validates and updates accumulated strides, from user input (GUI)
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void SDMSensor::button_P16UpdateStrides_Click(System::Object^  sender, System::EventArgs^  e)
{
	this->label_P16UpdateError->Visible = false;
	//convert and catch failed conversions
	try{
		ulStridesAccum = System::Convert::ToUInt32(this->textBox_P16StridesDisplayChange->Text);
		if (ulStridesAccum >= 16777216)
		{
			ulStridesAccum = 16777215;
			this->label_P16StridesDisplay->Text = "16777215";
		}
		else
			this->label_P16StridesDisplay->Text = this->textBox_P16StridesDisplayChange->Text;
	}
	catch(...){
		this->label_P16UpdateError->Text = "Error: Stride";
		this->label_P16UpdateError->Visible = true;
	}
}


/**************************************************************************
 * SDMSensor::button_P16UpdateDistance_Click
 * 
 * Validates and updates accumlated distance, from user input (GUI)
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void SDMSensor::button_P16UpdateDistance_Click(System::Object^  sender, System::EventArgs^  e) 
{
	this->label_P16UpdateError->Visible = false;
	//convert and catch failed conversions
	try{
		ulDistanceAccum = System::Convert::ToUInt32(this->textBox_P16DistDisplayChange->Text);
		this->label_P16DistanceDisplay->Text = this->textBox_P16DistDisplayChange->Text;
	}
	catch(...){
		this->label_P16UpdateError->Text = "Error: Dist";
		this->label_P16UpdateError->Visible = true;
	}
}


System::Void SDMSensor::checkBox_InvalidSerial_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
{
	if(this->checkBox_InvalidSerial->Checked)
		this->textBox_SerialNum->Enabled = false;
	else
		this->textBox_SerialNum->Enabled = true;
}