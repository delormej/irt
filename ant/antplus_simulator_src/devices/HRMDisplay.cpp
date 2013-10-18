/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/


#include "StdAfx.h"
#include "HRMDisplay.h"

/**************************************************************************
 * HRMDisplay::ANT_eventNotification
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
void HRMDisplay::ANT_eventNotification(UCHAR ucEventCode_, UCHAR* pucEventBuffer_)
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
 * HRMDisplay::InitializeSim
 * 
 * Initializes simulator variables
 * 
 * returns: N/A
 *
 **************************************************************************/
void HRMDisplay::InitializeSim()
{
	ucBPM = 0;					
	ucEventCount = 0;			
	ucPreviousEventCount = 0;	
	usTime1024 = 0;				
	usPreviousTime1024 = 0;		
	ulElapsedTime2 = 0;
	ucStatePage = HRM_INIT_PAGE;
	ulAcumEventCount = 0;
	ulAcumTime1024 = 0;
	usR_RInterval1024 = 0;		
	ucCalcBPM = 0;
	ucMfgID = 0;
	ucHwVersion = 0;
	ucSwVersion = 0;
	ucModelNum = 0;	
	usSerialNum = 0;
}

/**************************************************************************
 * HRMDisplay::HandleReceive
 * 
 * Decodes received data
 *
 * pucRxBuffer_: pointer to the buffer containing the received data
 * 
 * returns: N/A
 *
 **************************************************************************/
void HRMDisplay::HandleReceive(UCHAR* pucRxBuffer_)
{
	static UCHAR ucNoEventCount = 0;	// Successive transmissions with no new HR events
	UCHAR ucPageNum = 0;
	UCHAR ucEventDiff = 0;
	USHORT usTimeDiff1024 = 0;

	//Remove page toggle bit
	ucPageNum = pucRxBuffer_[0] & ~HRM_TOGGLE_MASK;

   // Do not process undefined pages
   if(!( ucPageNum == HRM_PAGE0 || ucPageNum == HRM_PAGE1 
      || ucPageNum == HRM_PAGE2 || ucPageNum == HRM_PAGE3 
      || ucPageNum == HRM_PAGE4))
      return;

	// Monitor page toggle bit
	if(ucStatePage != HRM_EXT_PAGE)
	{
		if(ucStatePage == HRM_INIT_PAGE)
		{ 
			ucStatePage = (pucRxBuffer_[0] & HRM_TOGGLE_MASK) >>7;
			// Initialize previous values to correctly get the cumulative values
			ucPreviousEventCount = pucRxBuffer_[6];
			usPreviousTime1024 = pucRxBuffer_[4];
			usPreviousTime1024 += pucRxBuffer_[5]<<8;
		}
		else if(ucStatePage != ((pucRxBuffer_[0] & HRM_TOGGLE_MASK) >>7))
		{ 
			// First page toggle was seen, enable advanced data
			ucStatePage = HRM_EXT_PAGE;
		}
	}
	

	// Handle basic data
	ucEventCount = pucRxBuffer_[6];
	usTime1024 = pucRxBuffer_[4];
	usTime1024 += pucRxBuffer_[5]<<8;
	ucBPM = pucRxBuffer_[7];

	// Handle background data, if supported
	if(ucStatePage == HRM_EXT_PAGE)
	{
		switch(ucPageNum)
		{
			case HRM_PAGE1:
				ulElapsedTime2 = pucRxBuffer_[1];
				ulElapsedTime2 += pucRxBuffer_[2] << 8;
				ulElapsedTime2 += pucRxBuffer_[3] << 16;
				break;
			case HRM_PAGE2:
				ucMfgID = pucRxBuffer_[1];
				usSerialNum = pucRxBuffer_[2];
				usSerialNum += pucRxBuffer_[3] << 8;
				break;
			case HRM_PAGE3:
				ucHwVersion = pucRxBuffer_[1];
				ucSwVersion = pucRxBuffer_[2];
				ucModelNum = pucRxBuffer_[3];
				break;
			case HRM_PAGE4:
				usPreviousTime1024 = pucRxBuffer_[2];
				usPreviousTime1024 += pucRxBuffer_[3] << 8;
				break;
			default:
				break;
		}
	}


	// Only need to do calculations if dealing with a new event
	if(ucEventCount != ucPreviousEventCount) 
	{
		ucNoEventCount = 0;
		// Update cumulative event count
		if(ucEventCount > ucPreviousEventCount)
			ucEventDiff = ucEventCount - ucPreviousEventCount;
		else
			ucEventDiff = (UCHAR) 0xFF - ucPreviousEventCount + ucEventCount + 1;

		ulAcumEventCount += ucEventDiff;

		// Update cumulative time
		if(usTime1024 > usPreviousTime1024)
			usTimeDiff1024 = usTime1024 - usPreviousTime1024;
		else
			usTimeDiff1024 = (USHORT) (0xFFFF - usPreviousTime1024 + usTime1024 + 1);
		ulAcumTime1024 += usTimeDiff1024;


		// Calculate R-R Interval
		if(ucEventDiff == 1) 
			usR_RInterval1024 = usTimeDiff1024;


		// Calculate heart rate (from timing data), in bpm
		if(usTimeDiff1024 > 0)
			ucCalcBPM = (UCHAR) ( ((USHORT) ucEventDiff * 0xF000) / usTimeDiff1024 );	// 1 min = 0xF000 = 60 * 1024 
	}
	else
	{
		ucNoEventCount++;
		if(ucNoEventCount >= MAX_NO_EVENTS)
		{
			ucCalcBPM = HRM_INVALID_BPM;	// No new valid HR data has been received
		}
	}
		
	// Display data
	UpdateDisplay(ucPageNum);

	// Update previous time and event count
	if(ucPageNum != HRM_PAGE4)
		usPreviousTime1024 = usTime1024;  // only if not previously obtained from HRM message
	ucPreviousEventCount = ucEventCount;
}


/**************************************************************************
 * HRMDisplay::UpdateDisplay
 * 
 * Shows received decoded data on GUI
 *
 * ucPageNum_: received page
 *
 * returns:  N/A
 *
 **************************************************************************/
void HRMDisplay::UpdateDisplay(UCHAR ucPageNum_)
{	
	// Display basic data
	this->label_Trn_TimeDisplay->Text = usTime1024.ToString();
	this->label_Trn_EventCountDisplay->Text = ucEventCount.ToString();
	if(ucBPM)
		this->label_Trn_RawPulseDisplay->Text = ucBPM.ToString();
	else
		this->label_Trn_RawPulseDisplay->Text = "---";	// Invalid heart rate
	

	// Display background data, if available
	if(ucStatePage == HRM_EXT_PAGE)
	{
		this->label_Calc_waitToggle->Visible = false;	// Page toggle was seen, so show in GUI
		switch(ucPageNum_)
		{
		case HRM_PAGE1:
			this->label_Glb_BattTimeDisplay->Text = (ulElapsedTime2*2).ToString();
			break;
		case HRM_PAGE2:
			this->label_Glb_ManfIDDisplay->Text = ucMfgID.ToString();
			this->label_Glb_SerialNumDisplay->Text = usSerialNum.ToString();
			break;
		case HRM_PAGE3:
			this->label_Glb_HardwareVerDisplay->Text = ucHwVersion.ToString();
			this->label_Glb_SoftwareVerDisplay->Text = ucSwVersion.ToString();
			this->label_Glb_ModelNumDisplay->Text = ucModelNum.ToString();
			break;
		case HRM_PAGE4:
			this->label_Trn_LastEventTimeDisplay->Text = usPreviousTime1024.ToString();
			break;
		default:
			break;
		}
	}
	
	// Display calculated data
	// Cumulative time (in s)
	this->label_Calc_ElapsedSecsDisplay->Text = System::Math::Round((DOUBLE) ulAcumTime1024/1024,3).ToString();
	
	// R-R interval (in ms) 
	if(ucStatePage == HRM_EXT_PAGE)
		this->label_Calc_RRIntervalDisplay->Text = System::Math::Round(((DOUBLE) usR_RInterval1024*1000/1024),3).ToString(); // Convert to ms
	else
		this->label_Calc_RRIntervalDisplay->Text = "---";

	// Cumulative event count
	this->label_Calc_TotEventCountDisplay->Text = ulAcumEventCount.ToString();

	// Heart rate (from timing data)
	if(ucCalcBPM)
	{
		this->label_Calc_PulseDisplay->Text = ucCalcBPM.ToString();
		this->label_Calc_InstPulseDisplay->Text = ucBPM.ToString();
	}
	else
	{
		this->label_Calc_PulseDisplay->Text = "---";	// Invalid heart rate
		this->label_Calc_InstPulseDisplay->Text = "---";
	}

}