/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/


#include "StdAfx.h"
#include "TemperatureSensor.h"
#include "antmessage.h"

using namespace System::Runtime::InteropServices; 

static ULONG ulMessageCount = 0;

static BOOL bCloseEvent = TRUE;	

/**************************************************************************
 * TemperatureSensor::InitializeSim
 * 
 * Initializes simulator variables
 * 
 * returns: N/A
 *
 **************************************************************************/
void TemperatureSensor::InitializeSim()
{
   // Set timer period (in ms)
   ulTimerInterval = 250;


   // Take the initial values from the simulator fields
   sMinTemp = System::Convert::ToInt16((this->numericUpDown_Prm_MinTemp->Value)*100);	// Default min temperature value is set on UI
	sCurTemp = System::Convert::ToInt16((this->numericUpDown_Prm_CurTemp->Value)*100);	// Default current temperature value is set on UI
	sMaxTemp = System::Convert::ToInt16((this->numericUpDown_Prm_MaxTemp->Value)*100);	// Default max temperature value is set on UI

  // initialize Temperature
   TemperatureData->sCurrentTemp = sCurTemp;	                              // Set initial temperature value (fixed simulation type)
   TemperatureData->s24HrHigh = System::Convert::ToInt16((this->numericUpDown_Prm_24HrHigh->Value)*10);
   TemperatureData->s24HrLow = System::Convert::ToInt16((this->numericUpDown_Prm_24HrLow->Value)*10);
   TemperatureData->ucTxInfo = TemperatureData->TXINFO_P5HZ;           // Set initial transmission info (5Hz, 1 page)
   TemperatureData->usMessagePeriod = TemperatureData->MESG_P5HZ_PERIOD;   // Defaults to Beacon Rate of 0.5Hz
	TemperatureData->b4HzRotation = FALSE;
   b4HzDefault = FALSE;

   // required common pages
   commonData->usMfgID = System::Convert::ToUInt16(this->textBox_ManfIDChange->Text);			   // Default values set on UI	
   commonData->ucHwVersion = System::Convert::ToByte(this->textBox_HardwareVerChange->Text);	
   commonData->ucSwVersion = System::Convert::ToByte(this->textBox_SoftwareVerChange->Text);	
   commonData->usModelNum = System::Convert::ToUInt16(this->textBox_ModelNumChange->Text);		
   commonData->ulSerialNum = System::Convert::ToUInt32(this->textBox_SerialNumChange->Text);

   //battery common page
	commonData->bBattPageEnabled = TRUE;

	// update the variables
	UpdateBatStatus();
	commonData->eTimeResolution = CommonData::TimeResolution::TWO;
	commonData->ulOpTime = ulTotalTime / (UCHAR) commonData->eTimeResolution;
	commonData->usBatVoltage256 = ((USHORT) System::Convert::ToByte(this->numericUpDown_Bat_VoltInt->Value)) << 8 | (commonData->usBatVoltage256 & 0xFF);	// Integer portion in high byte
	commonData->usBatVoltage256 |= System::Convert::ToByte(this->numericUpDown_Bat_VoltFrac->Value) | (commonData->usBatVoltage256 & 0xFF00);	// Fractional portion in low byte
	commonData->eTimeResolution = CommonData::TimeResolution::TWO;
	commonData->ulOpTime = ulTotalTime;


   ucReserved = TemperatureData->RESERVED;
	//ulRunTime = 0;		
	ucEventCount = 0;	
   ucSimDataType = SIM_FIXED;		      // Generate single fixed temperature value by default
   bSweepAscending = TRUE;             // sweep values will by default			      
}
/*************************************************************************
 * TemperatureSensor::onTimerTock
 * 
 * Simulates a sensor event, updating simulator data based on this event
 * Modifications to the timer interval are applied immediately after this
 * at ANTChannel
 *
 * usEventTime_: current time (ms)
 *
 * returns: N/A
 *
 **************************************************************************/
void TemperatureSensor::onTimerTock(USHORT eventTime)
 {
   UCHAR tempOffset = 0;				// Temporary variable to calculate sweeping intervals


	// Update temperature value
	switch(ucSimDataType)
	{
		case SIM_FIXED:
			// Transmits the current temperature value
         TemperatureData->sCurrentTemp = sCurTemp;
			break;
		/*case SIM_STEP:
			// Step: Jumps between min and max every 16 beats
			if(ucEventCount & 0x10)
				TemperatureData->sCurrentTemp = sMinTemp;
			else
				TemperatureData->sCurrentTemp = sMaxTemp;
			break;
         */
		case SIM_SWEEP:
			// temperature sweeps between min and max
			// The jump offset is calculated versus position against the max so it won't get stuck on low values for a long time and won't speed through high values too fast
			tempOffset = sMaxTemp - sCurTemp;
			tempOffset = (((tempOffset & 0xC0) >> 6) + ((tempOffset & 0x20) >>5) + ((tempOffset & 0x10) >>4)+1)*100;
			if(bSweepAscending)
				sCurTemp += tempOffset;
			else
				sCurTemp -= tempOffset;
			// Ensure value is not less than min or more than max
			if(sCurTemp >= sMaxTemp)
			{
				sCurTemp = sMaxTemp;
				bSweepAscending = FALSE;
			}
			if(sCurTemp <= sMinTemp)
			{
				sCurTemp = sMinTemp;
				bSweepAscending = TRUE;
			}
			TemperatureData->sCurrentTemp = sCurTemp;	
			break;
		default:
			break;
	}

   // Update 
	UpdateDisplay();
 }
/**************************************************************************
 * TemperatureSensor::ANT_eventNotification
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
void TemperatureSensor::ANT_eventNotification(UCHAR ucEventCode_, UCHAR* pucEventBuffer_)
{
	switch(ucEventCode_)
	{
	case EVENT_TX:
		HandleTransmit((UCHAR*) pucEventBuffer_);
		break;
   case EVENT_RX_BROADCAST:
	case EVENT_RX_ACKNOWLEDGED:
      HandleReceive((UCHAR*) pucEventBuffer_);
		break;
   case MESG_CLOSE_CHANNEL_ID:
		bCloseEvent = TRUE;
      this->groupBox_defaultPeriod->Enabled = false;
		break;
	default:
		break;
	}
}


/**************************************************************************
 * TemperatureSensor::HandleTransmit
 * 
 * Encode data generated by simulator for transmission
 *
 * pucTxBuffer_: pointer to the transmit buffer
 * 
 * returns: N/As
 *
 **************************************************************************/
void TemperatureSensor::HandleTransmit(UCHAR* pucTxBuffer_)
{
   // Kill Page Requests and Switch to 0.5Hz Beacon Mode
	if (bCloseEvent)
	{
		bCloseEvent = FALSE;      
      this->groupBox_defaultPeriod->Enabled = true;
		TemperatureData->bPageRequest = FALSE;
      if(!b4HzDefault)
      {
         TemperatureData->b4HzRotation = FALSE;
         TemperatureData->usMessagePeriod = TemperatureData->MESG_P5HZ_PERIOD;
         requestUpdateMesgPeriod(TemperatureData->MESG_P5HZ_PERIOD);
      }
      else
      {
         TemperatureData->b4HzRotation = TRUE;
         TemperatureData->usMessagePeriod = TemperatureData->MESG_4HZ_PERIOD;
         requestUpdateMesgPeriod(TemperatureData->MESG_4HZ_PERIOD);
      }
      
	}

   // Update event count
   TemperatureData->ucEventCount =  ++ucEventCount;

	static UCHAR ucMessageNum = 0;		      // Message count
	static bool bBackgroundToggle = false;		// Page toggle tracker
   static bool bMainPageToggle = false;

   // If a page has been requested
   if(TemperatureData->bPageRequest && ucRequestCount != 0)
   {
      // If all the requested pages have been sent already, uset the flag and continue normally
     /* if(ucRequestCount == 0)
      {
         TemperatureData->bPageRequest = false;
         goto send_normal;
      }*/

      // find out which page needs to be sent and encode it
     switch (TemperatureData->ucRequestedPageNum)	// Get Requested Page # 
		{
         case TemperatureData->PAGE_0 :
            TemperatureData->Encode(TemperatureData->PAGE_0, pucTxBuffer_);
            break;

         case TemperatureData->PAGE_1 :
            TemperatureData->Encode(TemperatureData->PAGE_1, pucTxBuffer_);
            break;

			case commonData->PAGE80 :
				commonData->Encode(commonData->PAGE80, pucTxBuffer_);
				break;

			case commonData->PAGE81 :
				commonData->Encode(commonData->PAGE81, pucTxBuffer_);
				break;

         case commonData->PAGE82:
            if(commonData->bBattPageEnabled)
               commonData->Encode(commonData->PAGE82, pucTxBuffer_);
            break;

			default:
				break;
		}
      ucRequestCount--;
     // return;
   }
//send_normal: 
   else if(ucMessageNum++ == TemperatureData->BACKGROUND_INTERVAL - 1)// Send background pages at defined interval 
		{
         TemperatureData->bPageRequest = false;

			ucMessageNum = 0;
			switch(bBackgroundToggle)
			{
				case false:
					// send Page 80
               commonData->Encode(commonData->PAGE80, pucTxBuffer_);							
					bBackgroundToggle = true;
					break;
				case true:
               // send Page 81
               commonData->Encode(commonData->PAGE81, pucTxBuffer_);
					bBackgroundToggle = false;
					break;
			} 
		}
   else
   { 
      // Interleave Page 0 and Page 1
      switch( bMainPageToggle)
      {
      case false :      // send page 0
         TemperatureData->Encode(TemperatureData->PAGE_0, pucTxBuffer_);
         bMainPageToggle = true;
         break;
      case true :    // send page 1
         TemperatureData->Encode(TemperatureData->PAGE_1, pucTxBuffer_);
         bMainPageToggle = false;
      }
      
   }

   this->label_TempTx->Text = (System::Convert::ToDecimal(sCurTemp)/100).ToString();	// Display temp being transmitted

   ulMessageCount++;

   // Return to 0.5Hz Beacon ONLY after 30 secs of no requests and if on 0.5Hz Default
	if (TemperatureData->b4HzRotation && (ulMessageCount > TemperatureData->MESG_4HZ_TIMEOUT) && !b4HzDefault) 
	{
		TemperatureData->b4HzRotation = FALSE;
		TemperatureData->usMessagePeriod = TemperatureData->MESG_P5HZ_PERIOD;
		requestUpdateMesgPeriod(TemperatureData->MESG_P5HZ_PERIOD);
	}

   static UCHAR ucTimeCount = 0;
	// update the batt op time every second
	if (ucTimeCount == 3)
	{
		ulTotalTime++;
		commonData->ulOpTime = ulTotalTime / (UCHAR) commonData->eTimeResolution;
		ucTimeCount = 0;
	}
	else
		ucTimeCount++;
}
/**************************************************************************
 * TemperatureSensor::HandleReceive
 * 
 * Handle incoming page requests
 *
 * pucTxBuffer_: pointer to the transmit buffer
 * 
 * returns: N/As
 *
 **************************************************************************/
void TemperatureSensor::HandleReceive(UCHAR *pucRxBuffer_)
{
   ulMessageCount = 0;  // Reset Message Counter on Any Reset

	if (pucRxBuffer_[0] == commonData->PAGE70)	// Data Page Request
	{
      // Change to 4Hz and begin Page Rotation on *ANY* RX
      if (!TemperatureData->b4HzRotation || b4HzDefault) // ADD CHECK FOR IF PAGE 70
      {
         TemperatureData->b4HzRotation = TRUE;
         TemperatureData->usMessagePeriod = TemperatureData->MESG_4HZ_PERIOD;		
         requestUpdateMesgPeriod(TemperatureData->MESG_4HZ_PERIOD);
      }

      TemperatureData->bPageRequest = TRUE;
      TemperatureData->ucRequestedPageNum = pucRxBuffer_[6];
      ucRequestCount = (pucRxBuffer_[5] & 0x7F);

   }
}
/**************************************************************************
 * TemperatureSensor::UpdateDisplay
 * 
 * Update the GUI with all the changes
 * 
 * returns: N/A
 *
 **************************************************************************/
void TemperatureSensor::UpdateDisplay()
{
   this->label_EventCount->Text = ucEventCount.ToString(); // Event count
   this->numericUpDown_Prm_CurTemp->Value = System::Convert::ToDecimal(sCurTemp)/100;	// Current temperature generated by simulator

   // update the operating time for the battery common page
	this->label_Bat_ElpTimeDisplay->Text = ulTotalTime.ToString();
}
/**************************************************************************
 * TemperatureSensor::ForceUpdate
 * 
 * Causes a timer event, to force the simulator to update all calculations
 * 
 * returns: N/A
 *
 **************************************************************************/
void TemperatureSensor::ForceUpdate()
{
	   timerHandle->Interval = 250;
}
/**************************************************************************
 * TemperatureSensor::radioButton_SimTypeChanged
 * 
 * Select method to generate simulator data, from user input (GUI)
 *
 * returns: N/A
 *
 **************************************************************************/
void TemperatureSensor::radioButton_SimTypeChanged (System::Object^  sender, System::EventArgs^  e)
{
   if(this->radioButton_SimTypeFixed->Checked)
	{
		this->numericUpDown_Prm_CurTemp->Enabled = true;
		this->numericUpDown_Prm_MinTemp->Enabled = false;
		this->numericUpDown_Prm_MaxTemp->Enabled = false;	
		ucSimDataType = SIM_FIXED;
	}
	else if(this->radioButton_SimTypeSweep->Checked)
	{
		this->numericUpDown_Prm_CurTemp->Enabled = false;
		this->numericUpDown_Prm_MinTemp->Enabled = true;
		this->numericUpDown_Prm_MaxTemp->Enabled = true;
		ucSimDataType = SIM_SWEEP;
		bSweepAscending = TRUE;
	}
}
/**************************************************************************
 * TemperatureSensor::button_AdvancedUpdate_Click
 * 
 * Validates and updates product information, from user input (GUI)
 * 
 * returns: N/A
 *
 **************************************************************************/
void TemperatureSensor::button_AdvancedUpdate_Click(System::Object^  sender, System::EventArgs^  e)
{
   this->label_AdvancedError->Visible = false;
	this->label_AdvancedError->Text = "Error: ";

	//convert and catch failed conversions
	try{
      commonData->usMfgID = System::Convert::ToUInt16(this->textBox_ManfIDChange->Text);
	}
	catch(...){
		this->label_AdvancedError->Text = System::String::Concat(label_AdvancedError->Text, " MFID");
		this->label_AdvancedError->Visible = true;
	}

	try{
		commonData->ulSerialNum = System::Convert::ToUInt32(this->textBox_SerialNumChange->Text);
		}
	catch(...){
		label_AdvancedError->Text = System::String::Concat(label_AdvancedError->Text, " Ser#");
		label_AdvancedError->Visible = true;
	}

	try{
		commonData->ucHwVersion = System::Convert::ToByte(this->textBox_HardwareVerChange->Text);
	}
	catch(...){
		label_AdvancedError->Text = System::String::Concat(label_AdvancedError->Text, " HWVr");
		label_AdvancedError->Visible = true;
	}

	try{
		commonData->ucSwVersion = System::Convert::ToByte(this->textBox_SoftwareVerChange->Text);
	}
	catch(...){
		label_AdvancedError->Text = System::String::Concat(label_AdvancedError->Text, " SWVr");
		label_AdvancedError->Visible = true;
	}

	try{
      commonData->usModelNum = System::Convert::ToUInt16(this->textBox_ModelNumChange->Text);
	}
	catch(...){
		label_AdvancedError->Text = System::String::Concat(label_AdvancedError->Text, " Mdl#");
		label_AdvancedError->Visible = true;
	}
}

/**************************************************************************
 * TemperatureSensor::numericUpDown_Prm_CurTemp_ValueChanged
 * 
 * Validates and updates the current temperature value, from user input (GUI)
 * 
 * returns: N/A
 *
 **************************************************************************/
void TemperatureSensor::numericUpDown_Prm_CurTemp_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
   // This value is raised whenever the value changes, even if internally
	// Only update the current temp if set by the user (not sweeping)
	if(this->numericUpDown_Prm_CurTemp->Enabled)
	{
      sCurTemp = System::Convert::ToInt16((this->numericUpDown_Prm_CurTemp->Value)*100);
      TemperatureData->sCurrentTemp = sCurTemp;
		ForceUpdate();
	}
}
/**************************************************************************
 * TemperatureSensor::numericUpDown_Prm_MinMaxTemp_ValueChanged
 * 
 * If the user has changed the min or max temperature, validate that
 * minimum < current <  maximum
 * 
 * returns: N/A
 *
 **************************************************************************/
void TemperatureSensor::numericUpDown_Prm_MinMaxTemp_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
   // This event is raised whenever the min and max value change, even if internally
	// Check min<max if in min/max mode, and force min<cur<max
	if(this->numericUpDown_Prm_MinTemp->Value < this->numericUpDown_Prm_MaxTemp->Value)
	{
		sMinTemp = System::Convert::ToByte((this->numericUpDown_Prm_MinTemp->Value)*100);
		sMaxTemp = System::Convert::ToByte((this->numericUpDown_Prm_MaxTemp->Value)*100);
		if(sCurTemp > sMaxTemp)
		{
			sCurTemp = sMaxTemp;
         this->numericUpDown_Prm_CurTemp->Value = System::Convert::ToDecimal(sCurTemp)/100;
			ForceUpdate();
		}
		else if(sCurTemp < sMinTemp)
		{
			sCurTemp = sMinTemp;
			this->numericUpDown_Prm_CurTemp->Value = System::Convert::ToDecimal(sCurTemp)/100;
			ForceUpdate();
		}
		
	}
	else
	{ 
		// If the values were invalid, set numeric values to last valid values
		this->numericUpDown_Prm_MinTemp->Value = System::Convert::ToDecimal(sMinTemp)/100;
		this->numericUpDown_Prm_MaxTemp->Value = System::Convert::ToDecimal(sMaxTemp)/100;
	}
}

/**************************************************************************
 * TemperatureSensor::numericUpDown_Prm_24HrHighLow_ValueChanged
 * 
 * Updates the 24Hr Low and 24Hr High variable when the user
 * changes the value in the box
 * 
 * returns: N/A
 *
 **************************************************************************/
void TemperatureSensor::numericUpDown_Prm_24HrHighLow_ValueChanged(System::Object^  sender, System::EventArgs^  e) 
{
   TemperatureData->s24HrHigh = System::Convert::ToInt16((this->numericUpDown_Prm_24HrHigh->Value)*10);
   TemperatureData->s24HrLow = System::Convert::ToInt16((this->numericUpDown_Prm_24HrLow->Value)*10);
   ForceUpdate();
}
/**************************************************************************
 * TemperatureSensor::radioButton_Time_CheckedChanged
 * 
 * Updates TxInfo (only 4hz version currently) bitfield regarding time capabilities
 * for use in data page 0
 * 
 * returns: N/A
 *
 **************************************************************************/
void TemperatureSensor::radioButton_Time_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
   if(this->radioButton_UTCTime_NotSupported->Checked)
   {
      TemperatureData->ucTxInfo = TemperatureData->TXINFO_P5HZ | TemperatureData->UTC_TIME_NOT_SUPPORTED;
   }
   else if(this->radioButton_UTCTime_NotSet->Checked)
   {
      TemperatureData->ucTxInfo = TemperatureData->TXINFO_P5HZ | TemperatureData->UTC_TIME_NOT_SET;
   }
   else
   {
      TemperatureData->ucTxInfo = TemperatureData->TXINFO_P5HZ | TemperatureData->UTC_TIME_SET;
   }

   if(this->radioButton_LocalTime_NotSupported->Checked)
   {
      TemperatureData->ucTxInfo |= TemperatureData->LOCAL_TIME_NOT_SUPPORTED;
   }
   else if(this->radioButton_LocalTime_NotSet->Checked)
   {
      TemperatureData->ucTxInfo |= TemperatureData->LOCAL_TIME_NOT_SET;
   }
   else
   {
      TemperatureData->ucTxInfo |= TemperatureData->LOCAL_TIME_SET;
   }

}

/**************************************************************************
 * TemperatureSensor::checkBox_Bat_Status_CheckedChanged
 * 
 * Enables and disables the battery voltage GUI elements
 *
 * returns: N/A
 *
 **************************************************************************/
void TemperatureSensor::checkBox_Bat_Status_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	if (this->checkBox_Bat_Status->Checked == TRUE)
	{
		commonData->bBattPageEnabled = TRUE;
		this->comboBoxBatStatus->Enabled = TRUE;
		this->label_Bat_ElpTimeDisplay->Enabled = TRUE;
		this->textBox_Bat_ElpTimeChange->Enabled = TRUE;
		this->checkBox_Bat_Voltage->Enabled = TRUE;
		this->checkBox_Bat_Voltage->Checked = TRUE;
		this->numericUpDown_Bat_VoltFrac->Enabled = TRUE;
		this->numericUpDown_Bat_VoltInt->Enabled = TRUE;
		this->groupBox_Resol->Enabled = TRUE;
		this->button_Bat_ElpTimeUpdate->Enabled = TRUE;

		// update the variables
		UpdateBatStatus();
		if (this->radioButton_Bat_Elp16Units->Checked)
			commonData->eTimeResolution = CommonData::TimeResolution::SIXTEEN;
		
		if (this->radioButton_Bat_Elp2Units->Checked)
			commonData->eTimeResolution = CommonData::TimeResolution::TWO;

		commonData->ulOpTime = ulTotalTime / (UCHAR) commonData->eTimeResolution;
		commonData->usBatVoltage256 = ((USHORT) System::Convert::ToByte(this->numericUpDown_Bat_VoltInt->Value)) << 8 | (commonData->usBatVoltage256 & 0xFF);	// Integer portion in high byte
		commonData->usBatVoltage256 |= System::Convert::ToByte(this->numericUpDown_Bat_VoltFrac->Value) | (commonData->usBatVoltage256 & 0xFF00);	// Fractional portion in low byte
	}

	else
	{
		commonData->bBattPageEnabled = FALSE;
		this->comboBoxBatStatus->Enabled = FALSE;
		this->label_Bat_ElpTimeDisplay->Enabled = FALSE;
		this->textBox_Bat_ElpTimeChange->Enabled = FALSE;
		this->checkBox_Bat_Voltage->Enabled = FALSE;
		this->checkBox_Bat_Voltage->Checked = FALSE;
		this->numericUpDown_Bat_VoltFrac->Enabled = FALSE;
		this->numericUpDown_Bat_VoltInt->Enabled = FALSE;
		this->groupBox_Resol->Enabled = FALSE;
		this->button_Bat_ElpTimeUpdate->Enabled = FALSE;
	}
}

/**************************************************************************
 * TemperatureSensor::checkBox_Bat_Voltage_CheckedChanged
 * 
 * Enables and disables the battery voltage numeric upDown
 *
 * returns: N/A
 *
 **************************************************************************/
void TemperatureSensor::checkBox_Bat_Voltage_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	if (this->checkBox_Bat_Voltage->Checked == TRUE)
	{
		this->numericUpDown_Bat_VoltInt->Enabled = TRUE;
		this->numericUpDown_Bat_VoltFrac->Enabled = TRUE;
		commonData->usBatVoltage256 = ((USHORT) System::Convert::ToByte(this->numericUpDown_Bat_VoltInt->Value)) << 8 | (commonData->usBatVoltage256 & 0xFF);	// Integer portion in high byte
		commonData->usBatVoltage256 |= System::Convert::ToByte(this->numericUpDown_Bat_VoltFrac->Value) | (commonData->usBatVoltage256 & 0xFF00);	// Fractional portion in low byte
	}
	else
	{
		this->numericUpDown_Bat_VoltInt->Enabled = FALSE;
		this->numericUpDown_Bat_VoltFrac->Enabled = FALSE;
		commonData->usBatVoltage256 = commonData->BATTERY_VOLTAGE_INVALID;
	}
}

/**************************************************************************
 * TemperatureSensor::numericUpDown_Bat_VoltInt_ValueChanged
 * 
 * changes the coarse battery voltage
 *
 * returns: N/A
 *
 **************************************************************************/
void TemperatureSensor::numericUpDown_Bat_VoltInt_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	commonData->usBatVoltage256 = ((USHORT) System::Convert::ToByte(this->numericUpDown_Bat_VoltInt->Value)) << 8 | (commonData->usBatVoltage256 & 0xFF);	// Integer portion in high byte
}

/**************************************************************************
 * TemperatureSensor::numericUpDown_Bat_VoltFrac_ValueChanged
 * 
 * changes the fractional battery voltage
 *
 * returns: N/A
 *
 **************************************************************************/
void TemperatureSensor::numericUpDown_Bat_VoltFrac_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	commonData->usBatVoltage256 = System::Convert::ToByte(this->numericUpDown_Bat_VoltFrac->Value) | (commonData->usBatVoltage256 & 0xFF00);	// Fractional portion in low byte
}

/**************************************************************************
 * TemperatureSensor::comboBoxBatStatus_SelectedIndexChanged
 * 
 * changes the battery status
 *
 * returns: N/A
 *
 **************************************************************************/
void TemperatureSensor::comboBoxBatStatus_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
{
	UpdateBatStatus();
}

/**************************************************************************
 * TemperatureSensor::UpdateBatStatus
 * 
 * Updates the battery status
 *
 * returns: N/A
 *
 **************************************************************************/
void TemperatureSensor::UpdateBatStatus()
{
	if (this->comboBoxBatStatus->Text->Equals("New"))
	{
		commonData->eBatStatus = CommonData::BatStatus::NEW;
	}

	else if (this->comboBoxBatStatus->Text->Equals("Good"))
	{
		commonData->eBatStatus = CommonData::BatStatus::GOOD;
	}

	else if (this->comboBoxBatStatus->Text->Equals("Ok"))
	{
		commonData->eBatStatus = CommonData::BatStatus::OK;
	}

	else if (this->comboBoxBatStatus->Text->Equals("Low"))
	{
		commonData->eBatStatus = CommonData::BatStatus::LOW;
	}

	else if (this->comboBoxBatStatus->Text->Equals("Critical"))
	{
		commonData->eBatStatus = CommonData::BatStatus::CRITICAL;
	}

	else if (this->comboBoxBatStatus->Text->Equals("Invalid"))
	{
		commonData->eBatStatus = CommonData::BatStatus::INVALID;
	}
}

/**************************************************************************
 * TemperatureSensor::button_Bat_ElpTimeUpdate_Click
 * 
 * changes the elapsed battery time
 *
 * returns: N/A
 *
 **************************************************************************/
void TemperatureSensor::button_Bat_ElpTimeUpdate_Click(System::Object^  sender, System::EventArgs^  e)
{
	ulTotalTime = System::Convert::ToUInt32(this->textBox_Bat_ElpTimeChange->Text);
	commonData->ulOpTime = ulTotalTime / (UCHAR) commonData->eTimeResolution;
}

/**************************************************************************
 * TemperatureSensor::radioButton_Bat_Elp2Units_CheckedChanged
 * 
 * changes the elapsed battery time units
 *
 * returns: N/A
 *
 **************************************************************************/
void TemperatureSensor::radioButton_Bat_Elp2Units_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	if (this->radioButton_Bat_Elp2Units->Checked)
		commonData->eTimeResolution = CommonData::TimeResolution::TWO;
}

/**************************************************************************
 * TemperatureSensor::radioButton_Bat_Elp2Units_CheckedChanged
 * 
 * changes the elapsed battery time units
 *
 * returns: N/A
 *
 **************************************************************************/
void TemperatureSensor::radioButton_Bat_Elp16Units_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	if (this->radioButton_Bat_Elp16Units->Checked)
		commonData->eTimeResolution = CommonData::TimeResolution::SIXTEEN;
}



   /**************************************************************************
 * TemperatureSensor::radioButton_Default_4Hz_CheckedChanged
 * 
 * changes the default Tx rate
 *
 * returns: N/A
 *
 **************************************************************************/
void TemperatureSensor::radioButton_Default_4Hz_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
   if(this->radioButton_Default_4Hz->Checked)
   {
      b4HzDefault = true;
      TemperatureData->b4HzRotation = true;TemperatureData->ucTxInfo = TemperatureData->TXINFO_4HZ;           // Set initial transmission info (5Hz, 1 page)
      TemperatureData->usMessagePeriod = TemperatureData->MESG_4HZ_PERIOD;
      requestUpdateMesgPeriod(TemperatureData->MESG_4HZ_PERIOD);
   }
   else
   {
      b4HzDefault = false;      
      TemperatureData->b4HzRotation = false;
      TemperatureData->ucTxInfo = TemperatureData->TXINFO_P5HZ;           // Set initial transmission info (5Hz, 1 page)
      TemperatureData->usMessagePeriod = TemperatureData->MESG_P5HZ_PERIOD;
      requestUpdateMesgPeriod(TemperatureData->MESG_P5HZ_PERIOD);
   }

}