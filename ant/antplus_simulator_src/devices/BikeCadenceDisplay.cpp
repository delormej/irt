/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/



#include "StdAfx.h"
#include "BikeCadenceDisplay.h"


/**************************************************************************
 * BikeCadenceDisplay::ANT_eventNotification
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
void BikeCadenceDisplay::ANT_eventNotification(UCHAR ucEventCode_, UCHAR* pucEventBuffer_)
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
 * BikeCadenceDisplay::InitializeSim
 * 
 * Initializes simulator variables
 * 
 * returns: N/A
 *
 **************************************************************************/
void BikeCadenceDisplay::InitializeSim()
{
	ucStatePage = BC_INIT_PAGE;
	usEventCount = 0;			
	usPreviousEventCount = 0;	
	usTime1024 = 0;				
	usPreviousTime1024 = 0;		
	ulElapsedTime2 = 0;
	bCoast = FALSE;

	ulAcumEventCount = 0;
	ulAcumTime1024 = 0;
	ucCadence = 0;

	ucMfgID = 0;
	ucHwVersion = 0;
	ucSwVersion = 0;
	ucModelNum = 0;	
	usSerialNum = 0;
}

/**************************************************************************
 * BikeCadenceDisplay::HandleReceive
 * 
 * Decodes received data
 *
 * pucRxBuffer_: pointer to the buffer containing the received data
 * 
 * returns: N/A
 *
 **************************************************************************/
void BikeCadenceDisplay::HandleReceive(UCHAR* pucRxBuffer_)
{
	static UCHAR ucNoEventCount = 0;	// Counter for successive transmissions with no new cadence events
	UCHAR ucPageNum = 0;
	USHORT usEventDiff = 0;
	USHORT usTimeDiff1024 = 0;

	// Monitor page toggle bit
	if(ucStatePage != BC_EXT_PAGE)
	{
		if(ucStatePage == BC_INIT_PAGE)
		{ 
			ucStatePage = (pucRxBuffer_[0] & BC_TOGGLE_MASK) >>7;
			// Initialize previous values to correctly get the cumulative values
			usPreviousTime1024 = pucRxBuffer_[4];
			usPreviousTime1024 += pucRxBuffer_[5]<<8;
			usPreviousEventCount = pucRxBuffer_[6];
			usPreviousEventCount += pucRxBuffer_[7]<<8;
		}
		else if(ucStatePage != ((pucRxBuffer_[0] & BC_TOGGLE_MASK) >>7))
		{ 
			// First page toggle was seen, enable advanced data
			ucStatePage = BC_EXT_PAGE;
		}
	}

	// Remove page toggle bit
	ucPageNum = pucRxBuffer_[0] & ~BC_TOGGLE_MASK;
	
	// Handle basic data
	usEventCount = pucRxBuffer_[6];
	usEventCount += pucRxBuffer_[7]<<8;
	usTime1024 = pucRxBuffer_[4];
	usTime1024 += pucRxBuffer_[5]<<8;

	// Handle advanced data, if supported
	if(ucStatePage == BC_EXT_PAGE)
	{
		switch(ucPageNum)
		{
		case BC_PAGE1:
			// Battery Time Page
			ulElapsedTime2 = pucRxBuffer_[1];
			ulElapsedTime2 += pucRxBuffer_[2] << 8;
			ulElapsedTime2 += pucRxBuffer_[3] << 16;
			break;
		case BC_PAGE2:
			// Manf Info Page
			ucMfgID = pucRxBuffer_[1];
			usSerialNum = pucRxBuffer_[2];
			usSerialNum += pucRxBuffer_[3] << 8;
			break;
		case BC_PAGE3:
			// Version Info Page
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
		bCoast = FALSE;
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

		// Calculate cadence (rpm)
		if(usTimeDiff1024 > 0)
			ucCadence = (UCHAR) ( ((ULONG) usEventDiff * 0xF000) / (ULONG) usTimeDiff1024 );	// 1 min = 0xF000 = 60 * 1024 
	}
	else
	{
		ucNoEventCount++;
		if(ucNoEventCount >= MAX_NO_EVENTS)
			bCoast = TRUE;	// Coasting
	}
	
	// Display data
	UpdateDisplay(ucPageNum);
	
	// Update previous time and event count
	usPreviousEventCount = usEventCount;
	usPreviousTime1024 = usTime1024;

}


/**************************************************************************
 * BikeCadenceDisplay::UpdateDisplay
 * 
 * Shows received decoded data on GUI
 *
 * ucPageNum_: received page
 *
 * returns:  N/A
 *
 **************************************************************************/
void BikeCadenceDisplay::UpdateDisplay(UCHAR ucPageNum_)
{	
	// Display basic data
	this->label_Trn_TimeDisplay->Text = usTime1024.ToString();
	this->label_Trn_EventCountDisplay->Text = usEventCount.ToString();
	this->label_Coasting->Visible = (bCoast == TRUE);

	// Display advanced data, if available
	if(ucStatePage == BC_EXT_PAGE)
	{
		this->label_Calc_waitToggle->Visible = false; // Page toggle already seen
		switch(ucPageNum_)
		{
		case BC_PAGE1:
			this->label_Glb_BattTimeDisplay->Text = (ulElapsedTime2*2).ToString();
			break;
		case BC_PAGE2:
			this->label_Glb_ManfIDDisplay->Text = ucMfgID.ToString();
			this->label_Glb_SerialNumDisplay->Text = usSerialNum.ToString();
			break;
		case BC_PAGE3:
			this->label_Glb_HardwareVerDisplay->Text = ucHwVersion.ToString();
			this->label_Glb_SoftwareVerDisplay->Text = ucSwVersion.ToString();
			this->label_Glb_ModelNumDisplay->Text = ucModelNum.ToString();
			break;
		default:
			break;
		}
	}
		
	// Display calculations
	// Cumulative time (in s)
	this->label_Calc_ElapsedSecsDisplay->Text = System::Math::Round((DOUBLE) ulAcumTime1024/1024,3).ToString();
	// Cumulative event count
	this->label_Calc_TotEventCountDisplay->Text = ulAcumEventCount.ToString();
	// Calculated cadence
	this->label_Calc_CadenceDisplay->Text = ucCadence.ToString();

}