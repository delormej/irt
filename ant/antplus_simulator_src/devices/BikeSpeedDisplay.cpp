/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/



#include "StdAfx.h"
#include "BikeSpeedDisplay.h"

/**************************************************************************
 * BikeSpeedeDisplay::ANT_eventNotification
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
void BikeSpeedDisplay::ANT_eventNotification(UCHAR ucEventCode_, UCHAR* pucEventBuffer_)
{
	switch(ucEventCode_)
	{
		case EVENT_RX_ACKNOWLEDGED:
		case EVENT_RX_BURST_PACKET: // intentional fall thru
		case EVENT_RX_BROADCAST:   
			HandleReceive((UCHAR*) pucEventBuffer_);
			break;
		default:
			break;
	}
}

/**************************************************************************
 * BikeSpeedDisplay::InitializeSim
 * 
 * Initializes simulator variables
 * 
 * returns: N/A
 *
 **************************************************************************/
void BikeSpeedDisplay::InitializeSim()
{
	ucStatePage = BS_INIT_PAGE;
	ucWheelCircumference = System::Convert::ToByte(this->numericUpDown_Sim_WheelCircumference->Value);  // Initial value set on UI
	usEventCount = 0;			
	usPreviousEventCount = 0;	
	usTime1024 = 0;				
	usPreviousTime1024 = 0;		
	ulElapsedTime2 = 0;
	bStop = FALSE;

	ulAcumEventCount = 0;
	ulAcumTime1024 = 0;
	ulSpeed = 0;
	ulDistance = 0;

	ucMfgID = 0;
	ucHwVersion = 0;
	ucSwVersion = 0;
	ucModelNum = 0;	
	usSerialNum = 0;
}

/**************************************************************************
 * BikeSpeedDisplay::HandleReceive
 * 
 * Decodes received data
 *
 * pucRxBuffer_: pointer to the buffer containing the received data
 * 
 * returns: N/A
 *
 **************************************************************************/
void BikeSpeedDisplay::HandleReceive(UCHAR* pucRxBuffer_)
{
	static UCHAR ucNoEventCount = 0;	// Counter for successive transmissions with no new speed events
	UCHAR ucPageNum = 0;
	USHORT usEventDiff = 0;
	USHORT usTimeDiff1024 = 0;

	// Monitor page toggle bit
	if(ucStatePage != BS_EXT_PAGE)
	{
		if(ucStatePage == BS_INIT_PAGE)
		{ 
			ucStatePage = (pucRxBuffer_[0] & BS_TOGGLE_MASK) >>7;
			// Initialize previous values to correctly get the cumulative values
			usPreviousTime1024 = pucRxBuffer_[4];
			usPreviousTime1024 += pucRxBuffer_[5]<<8;
			usPreviousEventCount = pucRxBuffer_[6];
			usPreviousEventCount += pucRxBuffer_[7]<<8;
		}
		else if(ucStatePage != ((pucRxBuffer_[0] & BS_TOGGLE_MASK) >>7))
		{ 
			// First page toggle was seen, enable advanced data
			ucStatePage = BS_EXT_PAGE;
			
		}
	}

	// Remove page toggle bit
	ucPageNum = pucRxBuffer_[0] & ~BS_TOGGLE_MASK;

	// Handle basic data
   if (ucPageNum <= BS_PAGE3)
   {
	usEventCount = pucRxBuffer_[6];
	usEventCount += pucRxBuffer_[7]<<8;
	usTime1024 = pucRxBuffer_[4];
	usTime1024 += pucRxBuffer_[5]<<8;
   }

	// Handle advanced data, if supported
	if(ucStatePage == BS_EXT_PAGE)
	{
		switch(ucPageNum)
		{
		case BS_PAGE1:
			//Battery Time Page
			ulElapsedTime2 = pucRxBuffer_[1];
			ulElapsedTime2 += pucRxBuffer_[2] << 8;
			ulElapsedTime2+= pucRxBuffer_[3] << 16;
			break;
		case BS_PAGE2:
			//Manf Info Page
			ucMfgID = pucRxBuffer_[1];
			usSerialNum = pucRxBuffer_[2];
			usSerialNum += pucRxBuffer_[3] << 8;
			break;
		case BS_PAGE3:
			//Version Info Page
			ucHwVersion = pucRxBuffer_[1];
			ucSwVersion = pucRxBuffer_[2];
			ucModelNum = pucRxBuffer_[3];
			break;
		default:
			break;
		}
	}
	
	// Only need to do calculations if dealing with a new event
	if(usEventCount != usPreviousEventCount)
	{
		ucNoEventCount = 0;
		bStop = FALSE;
		// Update cumulative event count
		if(usEventCount > usPreviousEventCount)
			usEventDiff = usEventCount - usPreviousEventCount;
		else
			usEventDiff = (USHORT) (0xFFFF - usPreviousEventCount + usEventCount + 1);
		ulAcumEventCount += usEventDiff;

		// Update cumulative time (1/1024s)
		if(usTime1024 > usPreviousTime1024)
			usTimeDiff1024 = usTime1024 - usPreviousTime1024;
		else
			usTimeDiff1024 = (USHORT) (0xFFFF - usPreviousTime1024 + usTime1024 + 1);
		ulAcumTime1024 += usTimeDiff1024;

		// Calculate speed (meters/h)
		if(ucWheelCircumference)
			ulSpeed = (ucWheelCircumference * 36864 * (ULONG) usEventDiff) / (ULONG) usTimeDiff1024;	// 1024 * 36 = 36864  (cm/(1/1024)s -> meters/h)

		// Calculate distance (cm)
		ulDistance = (ULONG) ucWheelCircumference * ulAcumEventCount;
	}
	else
	{
		ucNoEventCount++;
		if(ucNoEventCount >= MAX_NO_EVENTS)
			bStop = TRUE;	// Stopping
	}

	// Display data
	UpdateDisplay(ucPageNum);

	// Update previous time and event count
	usPreviousEventCount = usEventCount;
	usPreviousTime1024 = usTime1024;
}



/**************************************************************************
 * BikeSpeedDisplay::UpdateDisplay
 * 
 * Shows received decoded data on GUI
 *
 * ucPageNum_: received page
 *
 * returns:  N/A
 *
 **************************************************************************/
void BikeSpeedDisplay::UpdateDisplay(UCHAR ucPageNum_)
{	
	// Display basic data
	this->label_Trn_TimeDisplay->Text = System::Convert::ToString(usTime1024);
	this->label_Trn_EventCountDisplay->Text = System::Convert::ToString(usEventCount);
	this->label_Stopped->Visible = (bStop == TRUE);

	// Display advanced data, if available
	if(ucStatePage == BS_EXT_PAGE)
	{
		this->label_Calc_waitToggle->Visible = false; // Page toggle already seen
		switch(ucPageNum_)
		{
		case BS_PAGE1:
			this->label_Glb_BattTimeDisplay->Text = System::Convert::ToString((unsigned int)ulElapsedTime2*2);
			break;
		case BS_PAGE2:
			this->label_Glb_ManfIDDisplay->Text = System::Convert::ToString(ucMfgID);
			this->label_Glb_SerialNumDisplay->Text = System::Convert::ToString(usSerialNum);
			break;
		case BS_PAGE3:
			this->label_Glb_HardwareVerDisplay->Text = System::Convert::ToString(ucHwVersion);
			this->label_Glb_SoftwareVerDisplay->Text = System::Convert::ToString(ucSwVersion);
			this->label_Glb_ModelNumDisplay->Text = System::Convert::ToString(ucModelNum);
			break;
		default:
			break;
		}
	}
		
	// Display calculations
	// Cumulative time (in s)
	this->label_Calc_ElapsedSecsDisplay->Text = System::Convert::ToString(System::Math::Round((DOUBLE) ulAcumTime1024/1024,3));
	// Cumulative event count
	this->label_Calc_TotEventCountDisplay->Text = System::Convert::ToString((unsigned int) ulAcumEventCount);
	// Speed (km/h)
	this->label_Calc_SpeedDisplay->Text = System::Convert::ToString(System::Math::Round((DOUBLE) ulSpeed/1000,3));	 // meters/h -> km/h
	// Distance (km)
	this->label_Calc_DistanceDisplay->Text = System::Convert::ToString(System::Math::Round((DOUBLE) ulDistance/100000, 3));	// cm -> km
}


/**************************************************************************
 * BikeSpeedDisplay::numericUpDown_Sim_WheelCircumference_ValueChanged
 * 
 * Updates wheel circumference value, if updated (either by the user or internally)
 * Validation is already performed by the numericUpDown control
 *
 * returns:  N/A
 *
 **************************************************************************/
System::Void BikeSpeedDisplay::numericUpDown_Sim_WheelCircumference_ValueChanged(System::Object^  sender, System::EventArgs^  e) 
{
	ucWheelCircumference = System::Convert::ToByte(this->numericUpDown_Sim_WheelCircumference->Value);		 
}