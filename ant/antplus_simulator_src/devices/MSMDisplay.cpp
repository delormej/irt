/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/



#pragma once
#include "StdAfx.h"
#include "MSMDisplay.h"

/**************************************************************************
* MSMDisplay::InitializeSim
* 
* Initialize the simulator variables
*
* returns: N/A
*
**************************************************************************/
void MSMDisplay::InitializeSim()
{
	dbDispAcumDist = 0;
	usPreviousDist = 0;
	dbDispAcumTime = 0;
	usPreviousTime = 0;
	eCalStatus = MSM::CalibrationStatus::NONE;

	// initialize calibration stuff
	msmData->ucMode = System::Convert::ToByte (this->numericUpDownMode->Value);
	ucPreviousMode = msmData->ucMode;
	msmData->usScaleFactor10000 = System::Convert::ToUInt16 (this->numericUpDownScaleFactor->Value * msmData->CAL_SF_SCALE_FACTOR);
	usPreviousScale = msmData->usScaleFactor10000;
}


/**************************************************************************
* MSMDisplay::ANT_eventNotification
* 
* Process ANT channel event
*
* ucEventCode_: code of ANT channel event
* pucEventBuffer_: pointer to buffer containing data received from ANT,
*                  or a pointer to the transmit buffer in the case of an EVENT_TX
* 
* returns: N/A
*
**************************************************************************/
void MSMDisplay::ANT_eventNotification(UCHAR ucEventCode_, UCHAR* pucEventBuffer_)
{
	UCHAR ucPageNum = pucEventBuffer_[0];

	switch(ucEventCode_)
	{
		case EVENT_RX_ACKNOWLEDGED:
		case EVENT_RX_BURST_PACKET: // intentional fall thru
		case EVENT_RX_BROADCAST:
			// check if this is common pages or msm pages
			if (ucPageNum < commonData->START_COMMON_PAGE)
			{
				msmData->Decode(pucEventBuffer_);
				// check if calibration page
				if (ucPageNum == msmData->PAGE_CALIBRATION)
					UpdateCalibration();
			}
			else
				commonData->Decode(pucEventBuffer_);

			UpdateDisplay();
			break;

		case EVENT_TRANSFER_TX_COMPLETED:
			UpdateDisplayAckStatus(msmData->ACK_SUCCESS);	// Tx successful
			break;

		case EVENT_TRANSFER_TX_FAILED:	
		case EVENT_ACK_TIMEOUT:		// Intentional fall thru
			UpdateDisplayAckStatus(msmData->ACK_FAIL);
         break;
		default:
			break;
	}
}

/**************************************************************************
* MSMDisplay::UpdateDisplay
* 
* Updates the variables found on the GUI
*
* returns: N/A
*
**************************************************************************/
void MSMDisplay::UpdateDisplay()
{
	// update the acumulated variables
	UpdateAcumValues();

	// update the accumulated vaules on the display
	this->labelDisplayAcumDistance->Text = dbDispAcumDist.ToString("N2");
	this->labelDisplayAcumTime->Text = dbDispAcumTime.ToString("N2");

	// check for invalid speed
	if (msmData->IsSpeedValid(msmData->usInstSpeed1000))
		this->labelDisplayInstSpeed->Text = ((double) msmData->usInstSpeed1000 / msmData->SPEED_SCALE_FACTOR).ToString("N3");
	else
		this->labelDisplayInstSpeed->Text = "INVALID SPEED";

	// check for page 2 data, display it if found
	if (msmData->bPage2Enabled)
	{
		this->labelDisplayLat->Text = (msmData->slLatitude_SC / msmData->SEMI_CIRCLE_CONVERSION).ToString("N6");
		this->labelDisplayLong->Text = (msmData->slLongitude_SC / msmData->SEMI_CIRCLE_CONVERSION).ToString("N6");

		// update the Rx labels
		this->labelRxLat->Text = msmData->slLatitude_SC.ToString();
		this->labelRxLon->Text = msmData->slLongitude_SC.ToString();
	}

	// check if page 3 data has been received if so, show the data
	if (msmData->bPage3Enabled)
	{
		// check that elevation is valid
		if (msmData->IsElevationValid())
			this->labelDisplayElevation->Text = (((double) msmData->usElevation5 / msmData->ELEVATION_SCALE_FACTOR) - msmData->ELEVATION_OFFSET).ToString("N1");
		else
			this->labelDisplayElevation->Text = "Invalid";

		// check that heading data is valid
		if (msmData->IsHeadingValid())
			this->labelDisplayHeading->Text = ((double) msmData->usHeading10 / msmData->HEADING_SCALE_FACTOR).ToString("N1");
		else
			this->labelDisplayHeading->Text = "Invalid";

		// set the GPS Fix stuff
		switch (msmData->ucFixType)
		{
		case MSM::GpsFix::NONE:
			this->labelDisplayFixType->Text = "None";
			break;
		case MSM::GpsFix::INVALID:
			this->labelDisplayFixType->Text = "Invalid";
			break;
		case MSM::GpsFix::PROPAGATING:
			this->labelDisplayFixType->Text = "Propagating";
			break;
      case MSM::GpsFix::LAST_KNOWN:
         this->labelDisplayFixType->Text = "Last Known";
         break;
		case MSM::GpsFix::SEARCHING:
			this->labelDisplayFixType->Text = "Searching";
			break;
		case MSM::GpsFix::THREE_D:
			this->labelDisplayFixType->Text = "3D Fix";
			break;
		case MSM::GpsFix::THREE_D_DIF:
			this->labelDisplayFixType->Text = "3D Differential";
			break;
		case MSM::GpsFix::THREE_D_WAAS:
			this->labelDisplayFixType->Text = "3D WAAS";
			break;
		case MSM::GpsFix::TWO_D:
			this->labelDisplayFixType->Text = "2D Fix";
			break;
		case MSM::GpsFix::TWO_D_DIF:
			this->labelDisplayFixType->Text = "2D Differential";
			break;
		case MSM::GpsFix::TWO_D_WAAS:
			this->labelDisplayFixType->Text = "2D WAAS";
			break;
		default:
			this->labelDisplayFixType->Text = "Error";
			break;
		}

		// update the RX labels
		this->labelRxElev->Text = msmData->usElevation5.ToString();
		this->labelRxHeading->Text = msmData->usHeading10.ToString();
		this->labelRxFix->Text = msmData->ucFixType.ToString();
	}



	// the tx variables will show the raw transmitted data decoded, but not scaled into proper units
	this->labelTxAcumDist->Text = msmData->usAcumDist10.ToString();
	this->labelTxAcumTime->Text = msmData->usAcumTime1024.ToString();
	this->labelTxInstSpeed->Text = msmData->usInstSpeed1000.ToString();	

	// update the values for the HW common data page
	if (commonData->usMfgID != 0)
	{
		this->label_Glb_ManfIDDisplay->Text = commonData->usMfgID.ToString();
		this->label_Glb_HardwareVerDisplay->Text = commonData->ucHwVersion.ToString();
		this->label_Glb_ModelNumDisplay->Text = commonData->usModelNum.ToString();
	}

	// update the values for the SW common data page
	if (commonData->ulSerialNum != 0)
	{
		if(commonData->ulSerialNum == 0xFFFFFFFF)
			this->label_Glb_SerialNumDisplay->Text = "N/A";
		else
			this->label_Glb_SerialNumDisplay->Text = commonData->ulSerialNum.ToString();
		this->label_Glb_SoftwareVerDisplay->Text = commonData->ucSwVersion.ToString();

	}

	// update the values for the battery common data page
	if (commonData->ulOpTime != 0)
	{
		this->labelBattVolt->Text = System::Math::Round((double)commonData->usBatVoltage256/256,4).ToString("N2");
		
		if (commonData->eTimeResolution == CommonData::TimeResolution::SIXTEEN)
			this->labelTimeRes->Text = "16";
		else
			this->labelTimeRes->Text = "2";

		// now that we know the time resolution we can display the run time
		this->labelOpTime->Text = (commonData->ulOpTime * (UCHAR) commonData->eTimeResolution).ToString();

		switch (commonData->eBatStatus)
		{
		case CommonData::BatStatus::CRITICAL:
			this->labelBattStatus->Text = "Critical";
			break;
		case CommonData::BatStatus::GOOD:
			this->labelBattStatus->Text = "Good";
			break;
		case CommonData::BatStatus::INVALID:
			this->labelBattStatus->Text = "Invalid";
			break;
		case CommonData::BatStatus::LOW:
			this->labelBattStatus->Text = "Low";
			break;
		case CommonData::BatStatus::NEW:
			this->labelBattStatus->Text = "New";
			break;
		case CommonData::BatStatus::OK:
			this->labelBattStatus->Text = "Ok";
			break;
		default:
			break;
		}
	}

	// update the date and time info
	if (commonData->ucDays != 0)
	{
		this->labelHours->Text = commonData->ucHours.ToString("D2");
		this->labelMinutes->Text = commonData->ucMinutes.ToString("D2");
		this->labelSeconds->Text = commonData->ucSeconds.ToString("D2");
		this->labelDay->Text = commonData->ucDays.ToString("D2");
		this->labelMonth->Text = commonData->ucMonth.ToString("D2");
		this->labelYear->Text = commonData->ucYears.ToString("D2");

		switch (commonData->eDayOfWeek)
		{
		case CommonData::DayOfWeek::SUNDAY:
			this->labelDayOfWeek->Text = "Sunday";
			break;

		case CommonData::DayOfWeek::MONDAY:
			this->labelDayOfWeek->Text = "Monday";
			break;

		case CommonData::DayOfWeek::TUESDAY:
			this->labelDayOfWeek->Text = "Tuesday";
			break;

		case CommonData::DayOfWeek::WEDNESDAY:
			this->labelDayOfWeek->Text = "Wednesday";
			break;

		case CommonData::DayOfWeek::THURSDAY:
			this->labelDayOfWeek->Text = "Thursday";
			break;

		case CommonData::DayOfWeek::FRIDAY:
			this->labelDayOfWeek->Text = "Friday";
			break;

		case CommonData::DayOfWeek::SATURDAY:
			this->labelDayOfWeek->Text = "Saturday";
			break;

		case CommonData::DayOfWeek::INVALID:
			this->labelDayOfWeek->Text = "Invalid";
			break;

		default:
			break;
		}
	}
}

/**************************************************************************
* MSMDisplay::UpdateAcumValues
* 
* Updates the accumulated values 
*
* returns: N/A
*
**************************************************************************/
void MSMDisplay::UpdateAcumValues()
{
	USHORT usRollover;  // this is to help protect against rollover using integer math for accumulated values

	if (usPreviousDist == 0 && usPreviousTime == 0)  // check if this is the first transmission
	{
		// update the previous values to be current next time
		usPreviousDist = msmData->usAcumDist10;
		usPreviousTime = msmData->usAcumTime1024;
	}
	else
	{
		// update the internal display value for distance
		usRollover = msmData->usAcumDist10 - usPreviousDist;
		dbDispAcumDist += (double) usRollover / msmData->DIST_SCALE_FACTOR;

		// update the internal display value for time
		usRollover = msmData->usAcumTime1024 - usPreviousTime;
		dbDispAcumTime += (double) usRollover / msmData->TIME_SCALE_FACTOR;

		// update the previous values to be current next time
		usPreviousDist = msmData->usAcumDist10;
		usPreviousTime = msmData->usAcumTime1024;
	}
}

/**************************************************************************
* MSMDisplay::UpdateCalibration
* 
* Updates the calibration values and progress
*
* returns: N/A
*
**************************************************************************/

void MSMDisplay::UpdateCalibration()
{
	if (eCalStatus == MSM::CalibrationStatus::REQUEST_IN_PROGRESS)
	{
		// update the scale factor if it is valid
		if (msmData->usScaleFactor10000 == msmData->CAL_SCALE_CONF)
		{
			// update the scale factor box
			this->numericUpDownScaleFactor->Value = System::Convert::ToDecimal(msmData->usScaleFactor10000 / msmData->CAL_SF_SCALE_FACTOR);

			if (msmData->ucMode == ucPreviousMode)
			{
				this->labelCalibrationProgress->Text = "Calibration Request Confirmed \nMode Data Supported";
				this->labelCalibrationProgress->BackColor = System::Drawing::Color::Green;
			}

			else if (!msmData->IsCalModeValid(msmData->ucMode))
			{
				this->labelCalibrationProgress->Text = "Calibration Request Confirmed \nMode Data Not Supported";
				this->labelCalibrationProgress->BackColor = System::Drawing::Color::CadetBlue;
			}

			else
			{
				this->labelCalibrationProgress->Text = "Calibration Request Confirmed \nMode Data Invaild";
				this->labelCalibrationProgress->BackColor = System::Drawing::Color::Red;
			}
		}

		// calibration not supported response is valid
		else if (!msmData->IsCalScaleValid(msmData->usScaleFactor10000))
		{
			eCalStatus = MSM::CalibrationStatus::NONE;

			this->labelCalibrationProgress->Text = "Calibration Not Supported";
			this->labelCalibrationProgress->BackColor = System::Drawing::Color::CadetBlue;

			if (msmData->IsCalModeValid(msmData->ucMode))
			{
				this->labelCalibrationProgress->Text = "Calibration Not Supported \nMode Data Invaild: " + msmData->ucMode.ToString();
				this->labelCalibrationProgress->BackColor = System::Drawing::Color::Red;
			}
		}

		// any other respone is invalid
		else
		{
			this->labelCalibrationProgress->Text = "Scale Factor Invalid: " + msmData->usScaleFactor10000.ToString();
			this->labelCalibrationProgress->BackColor = System::Drawing::Color::Red;
		}
	}

	if (eCalStatus == MSM::CalibrationStatus::SET_SCALE_IN_PROGRESS)
	{
		// confirmation is given if both the scale factor and mode are the same as was previously sent
		if (msmData->usScaleFactor10000 == usPreviousScale)
		{
			// set the status
			eCalStatus = MSM::CalibrationStatus::COMPLETE;

			if (msmData->ucMode == ucPreviousMode)
			{
				this->labelCalibrationProgress->Text = "Set Scale Factor Confirmed \nMode Data Supported";
				this->labelCalibrationProgress->BackColor = System::Drawing::Color::Green;
			}

			else if (!msmData->IsCalModeValid(msmData->ucMode))
			{
				this->labelCalibrationProgress->Text = "Set Scale Factor Confirmed \nMode Data Not Supported";
				this->labelCalibrationProgress->BackColor = System::Drawing::Color::CadetBlue;
			}

			else
			{
				this->labelCalibrationProgress->Text = "Set Scale Factor Confirmed \nMode Data Invalid" + msmData->ucMode.ToString();
				this->labelCalibrationProgress->BackColor = System::Drawing::Color::Red;
			}
		}

		// sensor does not support calibration
		else if (!msmData->IsCalScaleValid(msmData->usScaleFactor10000))
		{
			// set the status
			eCalStatus = MSM::CalibrationStatus::NONE;

			this->labelCalibrationProgress->Text = "Set Scale Factor Confirmed \nCalibration Not Supported";
			this->labelCalibrationProgress->BackColor = System::Drawing::Color::CadetBlue;

			// invalid response for the mode
			if (msmData->IsCalModeValid(msmData->ucMode))
			{
				this->labelCalibrationProgress->Text = "Calibration Not Supported \nMode Data Invalid" + msmData->ucMode.ToString();
				this->labelCalibrationProgress->BackColor = System::Drawing::Color::Red;
			}
		}

		// any other response is invalid
		else
		{
			this->labelCalibrationProgress->Text = "Scale Factor Invalid: " + msmData->usScaleFactor10000.ToString();
			this->labelCalibrationProgress->BackColor = System::Drawing::Color::Red;
		}
	}

	if(eCalStatus == MSM::CalibrationStatus::COMPLETE)
	{
		this->labelCalibrationProgress->Text = "Calibration Process Complete";
		this->labelCalibrationProgress->BackColor = System::Drawing::SystemColors::Control;
	}
}

/**************************************************************************
* MSMDisplay::sendCalibrationMsg
*
* Sends calibration data pages from the display device to the sensor
*
* MSM::CalibartionStatus calType_: Determines what type of calibration message to send to the sensor
*
* returns: N/A
*
**************************************************************************/
void MSMDisplay::SendCalibrationMsg(MSM::CalibrationStatus calType_)
{
	// declare buffer for the acknowledged messages
	UCHAR aucAckBuffer[8] = {0,0,0,0,0,0,0,0};

	switch (calType_)
	{
	   case MSM::CalibrationStatus::REQUEST_IN_PROGRESS:
		   // set the scale factor to 0
		   msmData->usScaleFactor10000 = msmData->CAL_SCALE_REQUEST;
		   break;

	   case MSM::CalibrationStatus::SET_SCALE_IN_PROGRESS:
		   // set the scale factor according to the GUI
		   msmData->usScaleFactor10000 = USHORT (this->numericUpDownScaleFactor->Value * msmData->CAL_SF_SCALE_FACTOR);
		   break;

	   default:
		   break;
	}

	// update the previous variables to enable checking on return
	ucPreviousMode = msmData->ucMode;
	usPreviousScale = msmData->usScaleFactor10000;

	// get the data loaded into the buffer
	msmData->EncodeData(msmData->PAGE_CALIBRATION, aucAckBuffer);

	// make sure the buffer has something in it
	if (aucAckBuffer[0])
	{
		// send the acknowledged message
		requestAckMsg(aucAckBuffer);
	}
}

/**************************************************************************
* MSMDisplay::UpdateDisplayAckStatus
*
* Updates the display relative to the calibration process after the ACK is received
*
* UCHAR ackStatus_: this determines if the ACK was a success or a fail
*
* returns: N/A
*
**************************************************************************/
void MSMDisplay::UpdateDisplayAckStatus(UCHAR ackStatus_)
{
	switch (ackStatus_)
	{
	case msmData->ACK_SUCCESS:

		if (eCalStatus == MSM::CalibrationStatus::REQUEST_IN_PROGRESS)
		{
			this->labelCalibrationProgress->Text = "Calibration Request \nAcknowledgement Recieved";
			this->labelCalibrationProgress->BackColor = System::Drawing::Color::Orange;
		}

		if (eCalStatus == MSM::CalibrationStatus::SET_SCALE_IN_PROGRESS)
		{
			this->labelCalibrationProgress->Text = "Set Scale \nAcknowledgement Recieved";
			this->labelCalibrationProgress->BackColor = System::Drawing::Color::Orange;
		}
		break;

	case msmData->ACK_FAIL:
		if (eCalStatus == MSM::CalibrationStatus::REQUEST_IN_PROGRESS)
		{
			this->labelCalibrationProgress->Text = "Calibration Request \nAcknowledgement Failed";
			this->labelCalibrationProgress->BackColor = System::Drawing::Color::Red;
		}

		if (eCalStatus == MSM::CalibrationStatus::SET_SCALE_IN_PROGRESS)
		{
			this->labelCalibrationProgress->Text = "Set Scale \nAcknowledgement Failed";
			this->labelCalibrationProgress->BackColor = System::Drawing::Color::Red;
		}
		break;
	
	default:
		break;
	}
}

/**************************************************************************
* MSMDisplay::numericUpDownMode_ValueChanged
* 
* Handles the change to the calibration Mode value 
*
* returns: N/A
*
**************************************************************************/
void MSMDisplay::numericUpDownMode_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	msmData->ucMode = System::Convert::ToByte(this->numericUpDownMode->Value);

}

/**************************************************************************
* MSMDisplay::numericUpDownScaleFactor_ValueChanged
* 
* Handles the change to the calibration Scale Factor value 
*
* returns: N/A
*
**************************************************************************/
void  MSMDisplay::numericUpDownScaleFactor_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	msmData->usScaleFactor10000 = System::Convert::ToUInt16(this->numericUpDownScaleFactor->Value * msmData->CAL_SF_SCALE_FACTOR);
}

/**************************************************************************
* MSMDisplay::buttonCalibrationRequest_Click
* 
* Initiates the Calibration request to the MSM sensor
*
* returns: N/A
*
**************************************************************************/
void MSMDisplay::buttonCalibrationRequest_Click(System::Object^  sender, System::EventArgs^  e)
{
	eCalStatus = MSM::CalibrationStatus::REQUEST_IN_PROGRESS;
	SendCalibrationMsg(eCalStatus); 

	// update the display
	this->numericUpDownScaleFactor->Value = System::Convert::ToDecimal(msmData->usScaleFactor10000 / msmData->CAL_SF_SCALE_FACTOR);
	this->labelCalibrationProgress->Text = "Calibration Request Sent";
	this->labelCalibrationProgress->BackColor = System::Drawing::Color::Yellow;
}

/**************************************************************************
* MSMDisplay::buttonSetScaleFactor_Click
* 
* Sends a Set Mode/Scale Factor message to the sensor
*
* returns: N/A
*
**************************************************************************/
void MSMDisplay::buttonSetScaleFactor_Click(System::Object^  sender, System::EventArgs^  e)
{
	eCalStatus = MSM::CalibrationStatus::SET_SCALE_IN_PROGRESS;
	SendCalibrationMsg(eCalStatus);

	// update the display
	this->labelCalibrationProgress->Text = "Set Scale Factor Sent";
	this->labelCalibrationProgress->BackColor = System::Drawing::Color::Yellow;
}
