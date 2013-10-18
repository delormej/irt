/*******************************************************************************
 * THE FOLLOWING EXAMPLE CODE IS INTENDED FOR LIMITED CIRCULATION ONLY.
 * 
 * Please forward all questions regarding this code to ANT Technical Support.
 * 
 * Dynastream Innovations Inc.
 * 
 * (P) +1 403 932 9292
 * (F) +1 403 932 4196
 * (E) support@thisisant.com
 * 
 * www.thisisant.com
 *
 * Reference Design Disclaimer
 *
 * The references designs and codes provided may be used with ANT devices only and remain the copyrighted property of 
 * Dynastream Innovations Inc. The reference designs and codes are being provided on an "as-is" basis and as an accommodation, 
 * and therefore all warranties, representations, or guarantees of any kind (whether express, implied or statutory) including, 
 * without limitation, warranties of merchantability, non-infringement,
 * or fitness for a particular purpose, are specifically disclaimed.
 *
 * ©2012 Dynastream Innovations Inc. All Rights Reserved
 * This software may not be reproduced by
 * any means without express written approval of Dynastream
 * Innovations Inc.
 *
 *******************************************************************************/
 


#include "StdAfx.h"
#include "BikeSpdCadDisplay.h"

/**************************************************************************
 * BikeSpdCadDisplay::ANT_eventNotification
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
void BikeSpdCadDisplay::ANT_eventNotification(UCHAR ucEventCode_, UCHAR* pucEventBuffer_)
{
	switch(ucEventCode_)
	{
		case EVENT_RX_ACKNOWLEDGED:
		case EVENT_RX_BURST_PACKET: // intentional fall thru
		case EVENT_RX_BROADCAST:
			HandleReceive((UCHAR*) pucEventBuffer_);
			UpdateDisplay();
			break;
		default:
			break;
	}
}


/**************************************************************************
 * BikeSpdCadDisplay::InitializeSim
 * 
 * Initializes simulator variables
 * 
 * returns: N/A
 *
 **************************************************************************/
void BikeSpdCadDisplay::InitializeSim()
{
	eState = BSC_INIT;
	ucWheelCircumference = System::Convert::ToByte(this->numericUpDown_Sim_WheelCircumference->Value);  // Initial value set on UI
	usCadEventCount = 0;
	usCadPreviousEventCount = 0;
	usCadTime1024 = 0;	
	usCadPreviousTime1024 = 0;
	ulCadAcumEventCount = 0;
	ulCadAcumTime1024 = 0;	
	usSpdEventCount = 0;
	usSpdPreviousEventCount = 0;
	usSpdTime1024 = 0;			
	usSpdPreviousTime1024 = 0;
	ulSpdAcumEventCount = 0;
	ulSpdAcumTime1024 = 0;
	ucCadence = 0;
	ulSpeed = 0;
	ulDistance = 0;
	bStop = FALSE;
	bCoast = FALSE;
}


/**************************************************************************
 * BikeSpdCadDisplay::HandleReceive
 * 
 * Decodes received data
 *
 * pucRxBuffer_: pointer to the buffer containing the received data
 * 
 * returns: N/A
 *
 **************************************************************************/
void BikeSpdCadDisplay::HandleReceive(UCHAR* pucRxBuffer_)
{
	static UCHAR ucNoSpdEventCount = 0;	// Counter for successive transmissions with no new speed events
	static UCHAR ucNoCadEventCount = 0;	// Counter for successive transmissions with no new cadence events
	USHORT usEventDiff = 0;
	USHORT usTimeDiff1024 = 0;

	// Decode data page
	usCadTime1024 = pucRxBuffer_[0];
	usCadTime1024 += pucRxBuffer_[1] <<8;
	usCadEventCount = pucRxBuffer_[2];
	usCadEventCount += pucRxBuffer_[3] <<8;
	usSpdTime1024 = pucRxBuffer_[4];
	usSpdTime1024 += pucRxBuffer_[5] <<8;
	usSpdEventCount = pucRxBuffer_[6];
	usSpdEventCount += pucRxBuffer_[7] <<8;

	// Initialize previous values on first message received
	if(eState == BSC_INIT)
	{
		usCadPreviousTime1024 = usCadTime1024;
		usCadPreviousEventCount = usCadEventCount;
		usSpdPreviousTime1024 = usSpdTime1024;
		usSpdPreviousEventCount = usSpdEventCount;
		eState = BSC_ACTIVE;
	}

	// Update cadence calculations on new cadence event
	if(usCadEventCount != usCadPreviousEventCount)
	{
		ucNoCadEventCount = 0;
		bCoast = FALSE;
		// Update cumulative event count
		if(usCadEventCount > usCadPreviousEventCount)
			usEventDiff = usCadEventCount - usCadPreviousEventCount;
		else
			usEventDiff = (USHORT) (0xFFFF - usCadPreviousEventCount + usCadEventCount + 1);
		ulCadAcumEventCount += usEventDiff;

		// Update cumulative time (1/1024s)
		if(usCadTime1024 > usCadPreviousTime1024)
			usTimeDiff1024 = usCadTime1024 - usCadPreviousTime1024;
		else
			usTimeDiff1024 = (USHORT) (0xFFFF - usCadPreviousTime1024 + usCadTime1024 + 1);
		ulCadAcumTime1024 += usTimeDiff1024;

		// Calculate cadence (rpm)
		if(usTimeDiff1024 > 0)
			ucCadence = (UCHAR) ( ((ULONG) usEventDiff * 0xF000) / (ULONG) usTimeDiff1024 );	// 1 min = 0xF000 = 60 * 1024 		
	}
	else
	{
		ucNoCadEventCount++;
		if(ucNoCadEventCount >= MAX_NO_EVENTS)
			bCoast = TRUE;	// Coasting
	}

	// Update speed calculations on new speed event
	if(usSpdEventCount != usSpdPreviousEventCount)
	{
		ucNoSpdEventCount = 0;
		bStop = FALSE;
		// Update cumulative event count
		if(usSpdEventCount > usSpdPreviousEventCount)
			usEventDiff = usSpdEventCount - usSpdPreviousEventCount;
		else
			usEventDiff = (USHORT) (0xFFFF - usSpdPreviousEventCount + usSpdEventCount + 1);
		ulSpdAcumEventCount += usEventDiff;

		// Update cumulative time (1/1024s)
		if(usSpdTime1024 > usSpdPreviousTime1024)
			usTimeDiff1024 = usSpdTime1024 - usSpdPreviousTime1024;
		else
			usTimeDiff1024 = (USHORT) (0xFFFF - usSpdPreviousTime1024 + usSpdTime1024 + 1);
		ulSpdAcumTime1024 += usTimeDiff1024;

		// Calculate speed (meters/h)
		if(ucWheelCircumference)
			ulSpeed = (ucWheelCircumference * 0x9000 * (ULONG) usEventDiff) / (ULONG) usTimeDiff1024;	// 1024 * 36 = 0x9000

		// Calculate distance (cm)
		ulDistance = (ULONG) ucWheelCircumference * ulSpdAcumEventCount;
	}
	else
	{
		ucNoSpdEventCount++;
		if(ucNoSpdEventCount >= MAX_NO_EVENTS)
			bStop = TRUE;	// Stopping
	}

	// Update previous values
	usCadPreviousTime1024 = usCadTime1024;
	usCadPreviousEventCount = usCadEventCount;
	usSpdPreviousTime1024 = usSpdTime1024;
	usSpdPreviousEventCount = usSpdEventCount;
}

/**************************************************************************
 * BikeSpdCadDisplay::UpdateDisplay
 * 
 * Shows received decoded data on GUI
 *
 * ucPageNum_: received page
 *
 * returns:  N/A
 *
 **************************************************************************/
void BikeSpdCadDisplay::UpdateDisplay()
{	
	// Display received data
	this->label_Trn_CadenceTimeDisplay->Text = System::Convert::ToString(usCadTime1024);
	this->label_Trn_CadCountDisplay->Text = System::Convert::ToString(usCadEventCount);
	this->label_Trn_SpeedTimeDisplay->Text = System::Convert::ToString(usSpdTime1024);
	this->label_Trn_SpdCountDisplay->Text = System::Convert::ToString(usSpdEventCount);

	// Display calculations
	this->label_Calc_CadenceDisplay->Text = System::Convert::ToString(ucCadence);
	this->label_Calc_CadEventCountDisplay->Text = System::Convert::ToString((unsigned int) ulCadAcumEventCount);
	this->label_Calc_SpdEventCountDisplay->Text = System::Convert::ToString((unsigned int) ulSpdAcumEventCount);
	this->label_Calc_SpeedDisplay->Text = System::Convert::ToString((double) ulSpeed/1000);	 // meters/h -> km/h
	this->label_Calc_DistanceDisplay->Text = System::Convert::ToString(System::Math::Round((double) ulDistance/100000, 3));	// cm -> km
	// Most recent available time
	if(ulSpdAcumTime1024 < ulCadAcumTime1024) 
		this->label_Calc_ElapsedSecsDisplay->Text = System::Convert::ToString(System::Math::Round((double) ulCadAcumTime1024/1024,3));	// 1/1024 s -> s
	else
		this->label_Calc_ElapsedSecsDisplay->Text = System::Convert::ToString(System::Math::Round((double) ulSpdAcumTime1024/1024,3));	// 1/1024 s -> s
	// Stopping and coasting detected
	this->label_Coasting->Visible = (bCoast == TRUE && bStop == FALSE);	// Coasting not displayed if bike has stopped
	this->label_Stopped->Visible = (bStop == TRUE);
}


/**************************************************************************
 * BikeSpdCadDisplay::numericUpDown_Sim_WheelCircumference_ValueChanged
 * 
 * Updates wheel circumference value, if updated (either by the user or internally)
 * Validation is already performed by the numericUpDown control
 *
 * returns:  N/A
 *
 **************************************************************************/
System::Void BikeSpdCadDisplay::numericUpDown_Sim_WheelCircumference_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	ucWheelCircumference = System::Convert::ToByte(this->numericUpDown_Sim_WheelCircumference->Value);
}