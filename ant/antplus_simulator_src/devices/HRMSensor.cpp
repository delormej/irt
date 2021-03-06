/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/


#include "StdAfx.h"
#include "HRMSensor.h"

/**************************************************************************
 * HRMSensor::ANT_eventNotification
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
void HRMSensor::ANT_eventNotification(UCHAR ucEventCode_, UCHAR* pucEventBuffer_)
{
	switch(ucEventCode_)
	{
	case EVENT_TX:
		HandleTransmit((UCHAR*) pucEventBuffer_);
		break;
	default:
		break;
	}
}


/**************************************************************************
 * HRMSensor::InitializeSim
 * 
 * Initializes simulator variables
 * 
 * returns: N/A
 *
 **************************************************************************/
void HRMSensor::InitializeSim()
{
	ulTimerInterval = 833;			//	72 bpm
	ucReserved = HRM_RESERVED;
	ulRunTime = 0;		
	ulRunTime16000 = 0;	
	ucBackgroundCount = 0;
	ucNextBackgroundPage = 1;		// By default, support all defined pages
	ucEventCount = 0;	
	ucMinPulse = System::Convert::ToByte(this->numericUpDown_Prm_MinPulse->Text);	// Default min HR value is set on UI
	ucCurPulse = System::Convert::ToByte(this->numericUpDown_Prm_CurPulse->Text);	// Default current HR value is set on UI
	ucMaxPulse = System::Convert::ToByte(this->numericUpDown_Prm_MaxPulse->Text);	// Default max HR value is set on UI
	ucBPM = ucCurPulse;				// Initial heart rate value (fixed simulation type)
	ulElapsedTime2 = 0;	
	usTime1024 = 0;
	usPreviousTime1024 = 0;
	ucMfgID = System::Convert::ToByte(this->textBox_ManfIDChange->Text);			// Default values set on UI	
	ucHwVersion = System::Convert::ToByte(this->textBox_HardwareVerChange->Text);	
	ucSwVersion = System::Convert::ToByte(this->textBox_SoftwareVerChange->Text);	
	ucModelNum = System::Convert::ToByte(this->textBox_ModelNumChange->Text);		
	usSerialNum = System::Convert::ToUInt16(this->textBox_SerialNumChange->Text);			
	ucSimDataType = SIM_FIXED;		// Generate single fixed heart rate value by default
	bLegacy = FALSE;				// Disable legacy mode by default
	bTxMinimum = FALSE;				// Disable minimum data set by default
	bSweepAscending = TRUE;			// If sweeping, start with ascending values by default
}


/**************************************************************************
 * HRMSensor::HandleTransmit
 * 
 * Encode data generated by simulator for transmission
 *
 * pucTxBuffer_: pointer to the transmit buffer
 * 
 * returns: N/A
 *
 **************************************************************************/
void HRMSensor::HandleTransmit(UCHAR* pucTxBuffer_)
{
	static UCHAR ucMessageNum = 0;		// Message count
	static UCHAR ucPageToggle = 0;		// Page toggle tracker
	UCHAR ucPageNum;					// Page number

	if(bLegacy || bTxMinimum)
		ucPageNum = HRM_PAGE0;			// Send page 0 if using the minimum data set or legacy sensors
	else
		ucPageNum = HRM_PAGE4;			// Send page 4 otherwise
	
	// Send background pages at defined interval (not available in legacy sensors)
	if(!bLegacy)
	{	
		if(ucMessageNum++ == HRM_BACKGROUND_INTERVAL - 1)
		{
			ucMessageNum = 0;
			switch(ucNextBackgroundPage)
			{
				case 1:
					// Page 1 is not sent with minimum data set
					ucPageNum = HRM_PAGE1;							
					ucNextBackgroundPage++;
					break;
				case 2:
					ucPageNum = HRM_PAGE2;
					if(bTxMinimum)
					{
						// If using minimum data set, background pages are sent three times to ensure they are received by displays with lower message rate
						if(++ucBackgroundCount > 2)
						{
							ucBackgroundCount = 0;
							ucNextBackgroundPage ++;
						}
					}
					else
					{
						// Otherwise, send only once
						ucNextBackgroundPage++;
					}
					break;
				case 3:
					ucPageNum = HRM_PAGE3;
					if(bTxMinimum)
					{
						// If using minimum data set, background pages are sent three times to ensure they are received by displays with lower message rate
						if(++ucBackgroundCount > 2)
						{
							ucBackgroundCount = 0;
							ucNextBackgroundPage = HRM_PAGE2;
						}
					}
					else
					{
						// Otherwise, send only once
						ucNextBackgroundPage = HRM_PAGE1;
					}
					break;
					default:
						break;
			}
		}
	}

	// Fill in common info
	pucTxBuffer_[4] = (UCHAR) (usTime1024 & 0xFF);		// Low byte of time of last valid heart beat event (1/1024s)
	pucTxBuffer_[5] = (UCHAR) (usTime1024 >> 8) & 0xFF;	// High byte of time of last valid heart beat event (1/1024s)
	pucTxBuffer_[6] = ucEventCount;						// Heart beat count (counts)
	pucTxBuffer_[7] = ucBPM;							// Instantaneous heart rate (bpm)

	// Format the required page
	switch(ucPageNum)
	{
		case HRM_PAGE0:
			// Minimum data set: Page 0
			if(bLegacy)
				pucTxBuffer_[0] = ucReserved;			// Legacy sensors only, do not interpret at receiver, do not use as reference for new sensor designs
			else
				pucTxBuffer_[0] = HRM_PAGE0;
	
			pucTxBuffer_[1] = ucReserved;				// Reserved, do not interpret at the receiver
			pucTxBuffer_[2] = ucReserved;				// Reserved, do not interpret at the receiver
			pucTxBuffer_[3] = ucReserved;				// Reserved, do not interpret at the receiver
			break;
		case HRM_PAGE1:
			pucTxBuffer_[0] = HRM_PAGE1;				
			pucTxBuffer_[1] = (ulElapsedTime2 & 0xFF);		// Cumulative operating time, bits 0-7 (intervals of 2s)
			pucTxBuffer_[2] = (ulElapsedTime2 >> 8) & 0xFF;	// Cumulative operating time, bits 8-15 (intervals of 2s)
			pucTxBuffer_[3] = (ulElapsedTime2 >> 16) & 0xFF;	// Cumulative operating time, bits 16-23 (intervals of 2s)
			break;
		case HRM_PAGE2:
			pucTxBuffer_[0] = HRM_PAGE2;				
			pucTxBuffer_[1] = ucMfgID;				// Manufacturing ID
			pucTxBuffer_[2] = usSerialNum & 0x00FF;	// Low byte of serial number
			pucTxBuffer_[3] = (usSerialNum & 0xFF00) >>8;	// High byte of serial number
			break;
		case HRM_PAGE3:
			pucTxBuffer_[0] = HRM_PAGE3;
			pucTxBuffer_[1] = ucHwVersion;			// Hardware version
			pucTxBuffer_[2] = ucSwVersion;			// Software version
			pucTxBuffer_[3] = ucModelNum;			// Model number
			break;
		case HRM_PAGE4:
			pucTxBuffer_[0] = HRM_PAGE4;				
			pucTxBuffer_[1] = ucReserved;						// Reserved, do not interpret at receiver
			pucTxBuffer_[2] = usPreviousTime1024 & 0xFF;		// Low byte of previous heart beat event (1/1024s)
			pucTxBuffer_[3] = (usPreviousTime1024 >> 8) & 0xFF;	// High byte of previous heart beat event (1/1024s)
			break;
		default:
			break;
	}

	// Handle page toggle bit: toggle every four messages
	if(!bLegacy)
	{
		ucPageToggle += 0x20;
		pucTxBuffer_[0] += (ucPageToggle & HRM_TOGGLE_MASK);
	}
}



/**************************************************************************
 * HRMSensor::onTimerTock
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
void HRMSensor::onTimerTock(USHORT usEventTime_)
{
	UCHAR tempOffset = 0;				// Temporary variable to calculate sweeping intervals

	// Update event count
	++ucEventCount;

	// Save previous event time
	usPreviousTime1024 = usTime1024;
	
	// Update event time
	ulRunTime16000 += (ulTimerInterval << 4);	// Multiply by 16 to convert from ms to 1/16000s
	usTime1024 = (USHORT) ((ulRunTime16000 << 3) / 125);	// Convert to 1/1024s - multiply by 1024/16000

	ulRunTime += ulTimerInterval;
	while(ulRunTime/2000)   // 2000 ms
	{
		++ulElapsedTime2;  // elapsed time is updated every 2 seconds
		ulRunTime -=2000;
	}

	// Update heart rate value
	switch(ucSimDataType)
	{
		case SIM_FIXED:
			// Transmits the current pulse value
			ucBPM = ucCurPulse;
			break;
		case SIM_STEP:
			// Step: Jumps between min and max every 16 beats
			if(ucEventCount & 0x10)
				ucBPM = ucMinPulse;
			else
				ucBPM = ucMaxPulse;
			break;
		case SIM_SWEEP:
			// Heart rate sweeps between min and max
			// The jump offset is calculated versus position against the max so it won't get stuck on low values for a long time and won't speed through high values too fast
			tempOffset = ucMaxPulse - ucCurPulse;
			tempOffset = ((tempOffset & 0xC0) >> 6) + ((tempOffset & 0x20) >>5) + ((tempOffset & 0x10) >>4)+1;
			if(bSweepAscending)
				ucCurPulse += tempOffset;
			else
				ucCurPulse -= tempOffset;
			// Ensure value is not less than min or more than max
			if(ucCurPulse >= ucMaxPulse)
			{
				ucCurPulse = ucMaxPulse;
				bSweepAscending = FALSE;
			}
			if(ucCurPulse <= ucMinPulse)
			{
				ucCurPulse = ucMinPulse;
				bSweepAscending = TRUE;
			}
			ucBPM = ucCurPulse;	
			break;
		default:
			break;
	}
	
	// Update timer interval (in ms)
	if(ucBPM)
		ulTimerInterval = (ULONG) 60000/ucBPM;			// 60 seconds/beats per minute
	
	// Update display
	UpdateDisplay();

}


/**************************************************************************
 * HRMSensor::UpdateDisplay
 * 
 * Updates displayed simulator data on GUI
 *
 * returns: N/A
 *
 **************************************************************************/
void HRMSensor::UpdateDisplay()
{
	label_EventCount->Text = ucEventCount.ToString(); // Event count
	this->label_ElapsedSecsDisplay->Text = ((ulElapsedTime2 & 0x00FFFFFF) << 1).ToString();  // Operating time (s)
	this->numericUpDown_Prm_CurPulse->Value = ucCurPulse;	// Current BPM generated by simulator
	label_PulseTxd->Text = ucBPM.ToString();	// Current BPM transmitting
	this->label_CurrentTimeDisplay->Text = usTime1024.ToString(); // Time of last event (1/1024s)
	if(bTxMinimum || bLegacy)	// Time of previous event (1/1024s), only when using advanced data pages
		this->label_LastTimeDisplay->Text = "---";  
	else
		this->label_LastTimeDisplay->Text = usPreviousTime1024.ToString();
}


/**************************************************************************
 * HRMSensor::radioButton_SimTypeChanged
 * 
 * Select method to generate simulator data, from user input (GUI)
 *
 * returns: N/A
 *
 **************************************************************************/
void HRMSensor::radioButton_SimTypeChanged (System::Object^  sender, System::EventArgs^  e)
{
	if(radioButton_HRFixed->Checked)
	{
		this->numericUpDown_Prm_CurPulse->Enabled = true;
		this->numericUpDown_Prm_MinPulse->Enabled = false;
		this->numericUpDown_Prm_MaxPulse->Enabled = false;	
		ucSimDataType = SIM_FIXED;
	}
	else if(this->radioButton_HRSweep->Checked)
	{
		this->numericUpDown_Prm_CurPulse->Enabled = false;
		this->numericUpDown_Prm_MinPulse->Enabled = true;
		this->numericUpDown_Prm_MaxPulse->Enabled = true;
		ucSimDataType = SIM_SWEEP;
		bSweepAscending = TRUE;
	}
	else if(this->radioButton_HRStep->Checked)
	{
		this->numericUpDown_Prm_CurPulse->Enabled = false;
		this->numericUpDown_Prm_MinPulse->Enabled = true;
		this->numericUpDown_Prm_MaxPulse->Enabled = true;
		ucSimDataType = SIM_STEP;
	}
}



/**************************************************************************
 * HRMSensor::button_UpdateTime_Click
 * 
 * Validates and updates cumulative operating time, from user input (GUI)
 *
 * returns: N/A
 *
 **************************************************************************/
System::Void HRMSensor::button_UpdateTime_Click(System::Object^  sender, System::EventArgs^  e)
{
	ULONG ulCumulativeTime = 0;
	label_AdvancedError->Visible = false;

	try{
		ulCumulativeTime = System::Convert::ToUInt32(this->textBox_ElpTimeChange->Text);
		if(ulCumulativeTime > 33554430)			// Cumulative Operating Time rollover: 33554430 seconds
			throw "Cumulative operating time exceeds rollover value";
		ulElapsedTime2 = ulCumulativeTime >> 1;		// Cumulative time is stored in intervals of 2 seconds
		// Update display
		label_ElapsedSecsDisplay->Text = ((ulElapsedTime2 & 0x00FFFFFF) << 1).ToString();
	}

	catch(...){
		label_AdvancedError->Text = "Error:  Time";
		label_AdvancedError->Visible = true;
	}
}



/**************************************************************************
 * HRMSensor::button_AdvancedUpdate_Click
 * 
 * Validates and updates product information, from user input (GUI)
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void HRMSensor::button_AdvancedUpdate_Click(System::Object^  sender, System::EventArgs^  e) 
{
	this->label_AdvancedError->Visible = false;
	this->label_AdvancedError->Text = "Error: ";

	//convert and catch failed conversions
	try{
		ucMfgID = System::Convert::ToByte(this->textBox_ManfIDChange->Text);
	}
	catch(...){
		this->label_AdvancedError->Text = System::String::Concat(label_AdvancedError->Text, " MFID");
		this->label_AdvancedError->Visible = true;
	}

	try{
		usSerialNum = System::Convert::ToUInt16(this->textBox_SerialNumChange->Text);
		}
	catch(...){
		label_AdvancedError->Text = System::String::Concat(label_AdvancedError->Text, " Ser#");
		label_AdvancedError->Visible = true;
	}

	try{
		ucHwVersion = System::Convert::ToByte(this->textBox_HardwareVerChange->Text);
	}
	catch(...){
		label_AdvancedError->Text = System::String::Concat(label_AdvancedError->Text, " HWVr");
		label_AdvancedError->Visible = true;
	}

	try{
		ucSwVersion = System::Convert::ToByte(this->textBox_SoftwareVerChange->Text);
	}
	catch(...){
		label_AdvancedError->Text = System::String::Concat(label_AdvancedError->Text, " SWVr");
		label_AdvancedError->Visible = true;
	}

	try{
		ucModelNum = System::Convert::ToByte(this->textBox_ModelNumChange->Text);
	}
	catch(...){
		label_AdvancedError->Text = System::String::Concat(label_AdvancedError->Text, " Mdl#");
		label_AdvancedError->Visible = true;
	}
}


/**************************************************************************
 * HRMSensor::numericUpDown_Prm_CurPulse_ValueChanged
 * 
 * Validates and updates the current heart rate value, from user input (GUI)
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void HRMSensor::numericUpDown_Prm_CurPulse_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	// This value is raised whenever the value changes, even if internally
	// Only update the current pulse if set by the user (not sweeping)
	if(this->numericUpDown_Prm_CurPulse->Enabled)
	{
		ucCurPulse = System::Convert::ToByte(this->numericUpDown_Prm_CurPulse->Value);
		ForceUpdate();
	}
}



/**************************************************************************
 * HRMSensor::numericUpDown_Prm_MinMaxPulse_ValueChanged
 * 
 * If the user has changed the min or max heart rate, validate that
 * minimum < current <  maximum
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void HRMSensor::numericUpDown_Prm_MinMaxPulse_ValueChanged(System::Object^  sender, System::EventArgs^  e) 
{
	// This event is raised whenever the min and max value change, even if internally
	// Check min<max if in min/max mode, and force min<cur<max
	if(this->numericUpDown_Prm_MinPulse->Value < this->numericUpDown_Prm_MaxPulse->Value)
	{
		ucMinPulse = System::Convert::ToByte(this->numericUpDown_Prm_MinPulse->Value);
		ucMaxPulse = System::Convert::ToByte(this->numericUpDown_Prm_MaxPulse->Value);
		if(ucCurPulse > ucMaxPulse)
		{
			ucCurPulse = ucMaxPulse;
			this->numericUpDown_Prm_CurPulse->Value = ucCurPulse;
			ForceUpdate();
		}
		else if(ucCurPulse < ucMinPulse)
		{
			ucCurPulse = ucMinPulse;
			this->numericUpDown_Prm_CurPulse->Value = ucCurPulse;
			ForceUpdate();
		}
		
	}
	else
	{ 
		// If the values were invalid, set numeric values to last valid values
		this->numericUpDown_Prm_MinPulse->Value = ucMinPulse;
		this->numericUpDown_Prm_MaxPulse->Value = ucMaxPulse;
	}
	
}



/**************************************************************************
 * HRMSensor::checkBox_Legacy_CheckedChanged
 * 
 * Enable simulation of legacy HRM for testing backward compatibility of receivers
 *
 * returns: N/A
 *
 **************************************************************************/
System::Void HRMSensor::checkBox_Legacy_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(checkBox_Legacy->Checked)
	{
		System::Windows::Forms::DialogResult result = MessageBox::Show(L"This option is available for backward compatibility testing.\nIt should not be used as a reference in the development of new sensors.", L"Warning", MessageBoxButtons::OKCancel);
		if( result == ::DialogResult::OK )
		{
			bLegacy = TRUE;
			// Generate random data for reserved field to simulate legacy receivers
			// This should not be interpreted by the receiver, and by no means should be used as a reference for new designs
			// This feature is avaialable only for testing backwards compatibility
			srand(ulElapsedTime2);
			ucReserved = (UCHAR) ((rand() % 240) + 16);	// Generate a random number between 16 and 255
			checkBox_SendBasicPage->Checked = FALSE;
			checkBox_SendBasicPage->Enabled = FALSE;
		}
		else
		{
			bLegacy = FALSE;
			ucReserved = HRM_RESERVED;
			checkBox_Legacy->Checked = FALSE;
			checkBox_SendBasicPage->Enabled = TRUE;
		}
	}
	else
	{
		bLegacy = FALSE;
		ucReserved = HRM_RESERVED;
		checkBox_SendBasicPage->Enabled = TRUE;
	}
		 
}



/**************************************************************************
 * HRMSensor::checkBox_SendBasicPage_CheckedChanged
 * 
 * Enable sending basic data set: Page 0 and Background pages 2 and 3
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void HRMSensor::checkBox_SendBasicPage_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(checkBox_SendBasicPage->Checked)
	{
		bTxMinimum = TRUE;
		ucNextBackgroundPage = 2;	// Page 1 is disabled
		ucBackgroundCount = 0;		// Reset count so that pages are sent twice from the beginning
	}
	else
	{
		bTxMinimum = FALSE;
		ucNextBackgroundPage = 1;	// Enable Page 1
		ucBackgroundCount = 0;		// Reset count so that pages are sent twice from the beginning
	}
		 
}

/**************************************************************************
 * HRMSensor::ForceUpdate
 * 
 * Causes a timer event, to force the simulator to update all calculations
 * 
 * returns: N/A
 *
 **************************************************************************/
void HRMSensor::ForceUpdate()
{
	timerHandle->Interval = 250;
}
