/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/



#pragma once
#include "StdAfx.h"
#include "BikePowerSensor.h"


/**************************************************************************
* BikePowerSensor::ANT_eventNotification
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
void BikePowerSensor::ANT_eventNotification(UCHAR ucEventCode_, UCHAR* pucEventBuffer_)
{
	switch(ucEventCode_)
	{
	case EVENT_TX:
		HandleTransmit((UCHAR*) pucEventBuffer_);
		UpdateDisplay();
		break;
	case EVENT_RX_ACKNOWLEDGED:
		if(pucEventBuffer_[0] == commonPages->PAGE70)
		{
			commonPages->Decode((UCHAR*)pucEventBuffer_);
			HandleRequest((UCHAR*)pucEventBuffer_);
		}
		else  if(pucEventBuffer_[0] == bpPages->PAGE_GET_SET_PARAMETERS)
		{
			bpPages->ucCrankLength = pucEventBuffer_[4];
			
			HandleReceive((UCHAR*)pucEventBuffer_);
		}
		else
		{
			HandleCalibration((UCHAR*) pucEventBuffer_);
			UpdateCalDisplay();
		}
		break;
	case EVENT_TRANSFER_TX_COMPLETED:
		break;
	case EVENT_TRANSFER_TX_FAILED:
		break;
	default:
		break;
	}
}


/**************************************************************************
 * BikePowerSensor::InitializeSim
 * 
 * Initializes simulator variables
 * 
 * returns: N/A
 *
 **************************************************************************/
void BikePowerSensor::InitializeSim()
{	
	ulTimerInterval = 667; //90rpm
	ulRunTime1000 = 0;

	// Simulation settings
	bSweepAscending = TRUE;
	ucSimDataType = SIM_FIXED;
	bByCadence = TRUE;
	bStop = FALSE;
	bCoast = FALSE;
	bCalSuccess = TRUE;
	ucCurPedalPwrValue = System::Convert::ToByte(this->numericUpDown_PedalPwr_CurrOutput->Value);
	ucNumWheelGearTeeth = System::Convert::ToByte(this->numericUpDown_Sim_WheelGearTeeth->Value);
	ucNumCrankGearTeeth = System::Convert::ToByte(this->numericUpDown_Sim_CrankGearTeeth->Value);
	ucWheelCircumference100 = System::Convert::ToByte(this->numericUpDown_Sim_WheelCircumference->Value);	// Wheel circumference (cm)
	usWheelTorque32 = System::Convert::ToUInt16(32 * this->numericUpDown_Sim_WheelTorque->Value);	// Wheel torque (1/10 Nm)
	usCrankTorque32 = System::Convert::ToUInt16(32 * this->numericUpDown_Sim_CrankTorque->Value);	// Crank torque (1/10 Nm)
	ulMinValue1000 = System::Convert::ToUInt32(1000 * this->numericUpDown_Sim_MinOutput->Value);	// Min speed or cadence (resolution 1/1000)
	ulCurValue1000 = System::Convert::ToUInt32(1000 * this->numericUpDown_Sim_CurOutput->Value);	// Current speed or cadence (resolution 1/1000)
	ulMaxValue1000 = System::Convert::ToUInt32(1000 * this->numericUpDown_Sim_MaxOutput->Value);	// Max speed or cadence	 (resolution 1/1000)
	ulEventTime1000 = 0;
	sCalData = System::Convert::ToInt16(this->numericUpDown_Cal_CalNumber->Value);					// Calibration data
	usCalOffset = System::Convert::ToUInt16(this->numericUpDown_Cal_CalNumber->Value);				// CTF calibration data

	bpPages->usSlope10 = System::Convert::ToUInt16(10* this->numericUpDown_Ppg_SlopeConstant->Value);	// Slope (1/10 Nm/Hz)	
	bpPages->sRawTorque = System::Convert::ToInt16(this->numericUpDown_Cal_RawTorque->Value);		// Raw torque (signed)
	bpPages->sOffsetTorque = System::Convert::ToInt16(this->numericUpDown_Cal_OffsetTorque->Value);	// Offset torque (signed)
	this->listBox_Cal_AZStatus->SelectedIndex = 1;	// Auto Zero On
	bpPages->bAutoZeroEnable = TRUE;
	bpPages->bAutoZeroOn = TRUE;

	// Paging
	bpPages->eType = BikePower::SensorType::POWER_ONLY;		// Power only sensor by default
	bTxPage82 = TRUE;	// Include battery info
	bTxAZ = TRUE;		// Include auto zero support
	bTxCadence = TRUE;	// Include cadence
	bTxPedalPower = TRUE;	//Include pedal power
	bRightPedal = FALSE;	//Unknown pedal side contribution to power
	ucPowerInterleave = System::Convert::ToByte(this->numericUpDown_Ppg_BasicPowerInterlvTime->Value);	// Interleaving interval from UI
	ucEventFreq10 = System::Convert::ToByte(10 * this->numericUpDown_Ppg_TimerEventFreq->Value);		// Event freq from UI
	eUpdateType = BikePower::UpdateType::TIME_SYNCH;	// Event synchronous sensor
	ucMsgExpectingAck = 0;	// No messages pending at initialization

	// Common Data (initial values set on UI)
	commonPages->usBatVoltage256 = (System::Convert::ToByte(this->numericUpDown_Bat_VoltInt->Value) << 8) + (System::Convert::ToByte(this->numericUpDown_Bat_VoltFrac->Value));
	this->label_Voltage_Display->Text = ((double) commonPages->usBatVoltage256/256).ToString();	// Display in V
	commonPages->usMfgID = System::Convert::ToUInt16(this->textBox_Glb_ManfIDChange->Text);
	commonPages->ulSerialNum = System::Convert::ToUInt32(this->textBox_Glb_SerialNumChange->Text);
	commonPages->ucHwVersion = System::Convert::ToByte(this->textBox_Glb_HardwareVerChange->Text);
	commonPages->ucSwVersion = System::Convert::ToByte(this->textBox_Glb_SoftwareVerChange->Text);
	commonPages->usModelNum = System::Convert::ToUInt16(this->textBox_Glb_ModelNumChange->Text);
	commonPages->eTimeResolution = CommonData::TimeResolution::TWO;	// Two second resolution
	commonPages->ulOpTime = 0;
	this->listBox_Bat_Status->SelectedIndex = 2; // OK
	commonPages->eBatStatus = CommonData::BatStatus::OK;	

	ucBcastMessage = 0;
	ucBcastCount = 0;
	ucTransmissionCount = 0;

	bpPages->ucCrankLength = (UCHAR)(((float)numericUpDown_CrankLength->Value - (float)110.0) / (float)0.5);
	bpPages->ucSensorCapabilities = 0;
	bpPages->ucSensorStatus = 0x3E;

   bpPages->ucLeftTorqueEffectiveness = (UCHAR)(numericUpDown_LeftTorqueEffectiveness->Value * 2);
   bpPages->ucRightTorqueEffectiveness = (UCHAR) (numericUpDown_RightTorqueEffectiveness->Value * 2);
   bpPages->ucLeftPedalSmoothness = (UCHAR) (numericUpDown_LeftPedalSmoothness->Value * 2);
   bpPages->ucRightPedalSmoothness = (UCHAR) (numericUpDown_RightPedalSmoothness->Value * 2);
}



/**************************************************************************
 * BikePowerSensor::HandleTransmit
 * 
 * Encode data generated by simulator for transmission
 *
 * pucTxBuffer_: pointer to the transmit buffer
 * 
 * returns: N/A
 *
 **************************************************************************/
void BikePowerSensor::HandleTransmit(UCHAR* pucTxBuffer_)
{
	static ULONG ulMessageCount = 0;	
	static UCHAR ucExtMesgType = 0;
	UCHAR ucPageNum;
	
	if(ucBcastMessage)
	{
		switch(ucBcastMessage)
		{
		case bpPages->PAGE_GET_SET_PARAMETERS:
			ucBcastCount++;
			bpPages->EncodeMainData(ucBcastMessage, pucTxBuffer_);
			if(ucBcastCount > ucTransmissionCount)
				ucBcastMessage = 0;
		break;
		default:
			if((bpPages->eType == BikePower::SensorType::CRANK_TORQUE_FREQ) && (ucBcastMessage == BikePower::CAL_REQUEST))
			{
				bpPages->EncodeCTFCalibrationPage(BikePower::CTF_OFFSET, usCalOffset, pucTxBuffer_);	// CTF sensors, send calibration offset
				ucBcastCount++;
				if(ucBcastCount > BikePower::MAX_RETRY_CTF)
				{
					ucBcastMessage = 0;
					ucBcastCount = 0;
				}
			}
			else if(ucBcastMessage)	
			{
				switch(ucBcastMessage)
				{
				case BikePower::CAL_REQUEST:	// Manual calibration request, send calibration response
					if(bCalSuccess)		// Other sensors, report success or failure
						bpPages->EncodeCalibrationResponse(BikePower::CAL_SUCCESS, bpPages->bAutoZeroEnable, bpPages->bAutoZeroOn, sCalData, pucTxBuffer_);	
					else
						bpPages->EncodeCalibrationResponse(BikePower::CAL_FAIL, bpPages->bAutoZeroEnable, bpPages->bAutoZeroOn, sCalData, pucTxBuffer_);	
					break;
				case BikePower::CAL_AUTOZERO_CONFIG:	// Auto zero calibration request, send response (which includes auto zero status)
					if(bCalSuccess)		
						bpPages->EncodeCalibrationResponse(BikePower::CAL_SUCCESS, bpPages->bAutoZeroEnable, bpPages->bAutoZeroOn, sCalData, pucTxBuffer_);	
					else
						bpPages->EncodeCalibrationResponse(BikePower::CAL_FAIL, bpPages->bAutoZeroEnable, bpPages->bAutoZeroOn, sCalData, pucTxBuffer_);	
					break;
				case BikePower::CTF_SLOPE:	
					bpPages->EncodeCTFCalibrationPage(BikePower::CTF_ACK, BikePower::CTF_SLOPE, pucTxBuffer_);
					break;
				case BikePower::CTF_SERIAL:	// ACK save serial to flash
					bpPages->EncodeCTFCalibrationPage(BikePower::CTF_ACK, BikePower::CTF_SERIAL, pucTxBuffer_);
					break;
				default:
					return;
				}
				ucBcastMessage = 0;		
				ucBcastCount = 0;
			}
			break;
		}
		return;
	}

	

	// Figure out what page to send
	// Set first to primary page according to sensor type
	switch(bpPages->eType)
	{
	case BikePower::SensorType::POWER_ONLY:
		ucPageNum = BikePower::PAGE_POWER;
		break;
	case BikePower::SensorType::TORQUE_WHEEL:
		ucPageNum = BikePower::PAGE_WHEEL_TORQUE;
		break;
	case BikePower::SensorType::TORQUE_CRANK:
		ucPageNum = BikePower::PAGE_CRANK_TORQUE;
		break;
	case BikePower::SensorType::CRANK_TORQUE_FREQ:
		ucPageNum = BikePower::PAGE_CTF;
		break;
	default:
		break;
	}
	
	// Handle interleaving:
	// The page with lowest frequency takes priority when there is a conflict
	// Every 121st message - one of the two global data pages, or the autozero support page (except CTF)
	if(bpPages->eType !=  BikePower::SensorType::CRANK_TORQUE_FREQ)
	{
		if(ulMessageCount % BikePower::INTERVAL_COMMON_AND_AZ == 0 && ulMessageCount != 0)
		{
			if(ucExtMesgType == 0)
			{
				ucPageNum = CommonData::PAGE80;
				ucExtMesgType++;
			}
			else if(ucExtMesgType == 1)
			{
				ucPageNum = CommonData::PAGE81;
				if(bTxAZ)
					ucExtMesgType++;
				else
					ucExtMesgType = 0;
			}
			else if(ucExtMesgType == 2)
			{
				ucPageNum = BikePower::PAGE_CALIBRATION;
				ucExtMesgType = 0;
			}
		}
		// Every 61st message - battery status (except CTF)
		else if((ulMessageCount % BikePower::INTERVAL_BATTERY == 0) && bTxPage82 && ulMessageCount != 0)
		{
			ucPageNum = CommonData::PAGE82;
		}
		// Every 2-9 messages (user defined) - basic power (wheel/crank torque)
		else if((ulMessageCount % ucPowerInterleave == 0) && bpPages->eType !=  BikePower::SensorType::POWER_ONLY && ulMessageCount != 0)
		{
			ucPageNum = BikePower::PAGE_POWER;
		}
      // Every 5 messages, if enabled, toque effectiveness & pedal smoothness page
      else if((ulMessageCount % BikePower::INTERVAL_TEPS == 1) && bTxPage19) // offset by 1, so it doesn't overlap with basic power if sent every 5 messages
      {
         ucPageNum = BikePower::PAGE_TEPS;
      }
		ulMessageCount++;
	}

	// Main Bike Power Data Pages
	if(ucPageNum >= BikePower::PAGE_POWER && ucPageNum <= BikePower::PAGE_CTF)
	{
		try
			{
				bpPages->EncodeMainData(ucPageNum, pucTxBuffer_);
			}
			catch(BikePower::Error^ errorMain)
			{
			}
	}

	// Calibration (Auto Zero Support)
	if(ucPageNum == BikePower::PAGE_CALIBRATION)
		bpPages->EncodeAutoZeroSupport(pucTxBuffer_);

	// Common Pages
	if(ucPageNum >= CommonData::PAGE80 && ucPageNum <= CommonData::PAGE82)
	{
		try
			{
				commonPages->Encode(ucPageNum, pucTxBuffer_);
			}
			catch(CommonData::Error^ errorCommon)
			{
			}
	}

}


/**************************************************************************
 * BikePowerSensor::HandleCalibration
 * 
 * Decodes recived calibration messages and sends a response if required
 *
 * pucRxBuffer_: pointer to the receive buffer
 * 
 * returns: N/A
 *
 **************************************************************************/
void BikePowerSensor::HandleCalibration(UCHAR* pucRxBuffer_)
{
	BOOL bAutoZeroInvalid = FALSE;

	UCHAR ucPageNum = pucRxBuffer_[0];
	if(ucPageNum == BikePower::PAGE_CALIBRATION)
	{
		try
		{
			bpPages->Decode(pucRxBuffer_);
		}
		catch(BikePower::Error^ errorBikePower)
		{
			if(errorBikePower->bUndefAutoZero)
				bAutoZeroInvalid = TRUE;
		}

		if(bpPages->ucRxCalibrationID == BikePower::CAL_REQUEST)
			SendCalibrationResponse(BikePower::CAL_REQUEST);
		
		if(bpPages->ucRxCalibrationID == BikePower::CAL_AUTOZERO_CONFIG)
		{
			// The received auto zero status (if valid) is updated directly when decoding
			BOOL bPrevCalStatus = bCalSuccess;
			if(bAutoZeroInvalid)				
				bCalSuccess = FALSE;	// Report as failure if an invalid status was received, regardless of how it is set on interface
			SendCalibrationResponse(BikePower::CAL_AUTOZERO_CONFIG);
			bCalSuccess = bPrevCalStatus;
		}
		
		if(bpPages->ucRxCalibrationID == BikePower::CAL_CTF)
		{
			if(bpPages->ucRxCTFMsgID == BikePower::CTF_SLOPE)
			{
				// Validate and send response
				if(bpPages->usCalSlope > BikePower::MAX_SLOPE)
					bpPages->usCalSlope = BikePower::MAX_SLOPE;
				else if(bpPages->usCalSlope < BikePower::MIN_SLOPE)
					bpPages->usCalSlope = BikePower::MIN_SLOPE;
				bpPages->usSlope10 = bpPages->usCalSlope;			// Save on flash
				SendCalibrationResponse(BikePower::CTF_SLOPE);
			}
			else if(bpPages->ucRxCTFMsgID == BikePower::CTF_SERIAL)
			{
				// The received serial num is also already updated by the Bike Power decoding
				this->commonPages->ulSerialNum = bpPages->usCalSerialNum;	// Save on flash
				SendCalibrationResponse(BikePower::CTF_SERIAL);
			}
		}
	}
}

void BikePowerSensor::HandleRequest(UCHAR* pucRxBuffer_)
{
	//Request pages supported so far
	if(commonPages->ucReqPageNum != bpPages->PAGE_GET_SET_PARAMETERS)
		return;

	if(!ucBcastMessage)
	{
		ucTransmissionCount = (commonPages->ucReqTransResp & 0x7F);
		ucBcastCount = 1;				// Reset transmission counter
		ucBcastMessage = commonPages->ucReqPageNum;	// Set code to message for which to transmit
	}
}
void BikePowerSensor::HandleReceive(UCHAR *pucRxBuffer_)
{
	switch(pucRxBuffer_[0])
	{
	case BikePower::PAGE_GET_SET_PARAMETERS:
		if(bpPages->ucCrankLength < 0xFE)
		{
			numericUpDown_CrankLength->Value = (Decimal)(((float)bpPages->ucCrankLength * (float)0.5) + (float)110.0);
			checkBox_InvalidCrankLength->Checked = false;
		}
		else if(bpPages->ucCrankLength == 0xFE)
		{
			numericUpDown_CrankLength->Value = (Decimal)172.5;
			bpPages->ucCrankLength = 0x7D;
			checkBox_InvalidCrankLength->Checked = false;
		}
		else if(bpPages->ucCrankLength == 0xFF)
			checkBox_InvalidCrankLength->Checked = true;
		break;
	default:
		break;
	}
}
/**************************************************************************
 * BikePowerSensor::SendCalibrationResponse
 * 
 * Sends calibration message according to the message code, using the values
 * for the parameters previously set on the GUI
 *
 * ucMsgCode_: message ID of the calibration message requiring a response
 * Supported messages:  
 * - calibration response (success or fail, or for CTF sensors, calibration offset)
 * - auto zero calibration response
 * - serial CTF ack
 * - slope CTF ack
 * 
 * returns: N/A
 *
 **************************************************************************/
void BikePowerSensor::SendCalibrationResponse(UCHAR ucMsgCode_)
{
	// Send acknowledged message
	if(!ucBcastMessage)	
	{
		ucBcastCount = 1;				// Reset retransmission counter
		ucBcastMessage = ucMsgCode_;	
	}
}


/**************************************************************************
 * BikePowerSensor::onTimerTock
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
void BikePowerSensor::onTimerTock(USHORT eventTime_)
{
	bpPages->usTime2000 = 2 * eventTime_;	// Time in intervals of 1/2000s 

	if(bTxPage82) // Update battery page if enabled
	{
		// Update operating time
		ulRunTime1000 += ulTimerInterval;	// in ms
		while(ulRunTime1000/((USHORT) 1000 * (UCHAR) commonPages->eTimeResolution))   
		{
			(commonPages->ulOpTime)++;  // elapsed time is updated every 2 or 16 seconds, depending on resolution
			ulRunTime1000 -= ((USHORT) 1000 * (UCHAR) commonPages->eTimeResolution);
		}
	}

	// Calculate next value, if sweeping
	if(ucSimDataType == SIM_SWEEP && !bStop && !bCoast)
	{
		// Value sweeps between max and min
		// The jump offset is calculated versus position against the max so it won't get stuck on low values for a long time and won't speed through high values too fast
		ULONG tempOffset = ulMaxValue1000 - ulMinValue1000;
		tempOffset = ((tempOffset & 0x800) >> 6) + ((tempOffset & 0x400) >> 5) + ((tempOffset & 0x200) >> 4) + 0xFF;
		if(bSweepAscending)
			ulCurValue1000 += tempOffset;
		else
			ulCurValue1000 -= tempOffset;
		// Ensure value is more than min and less than max
		if(ulCurValue1000 > ulMaxValue1000)
		{
			ulMaxValue1000 = ulMaxValue1000;
			bSweepAscending = FALSE;
		}
		if(ulCurValue1000 < ulMinValue1000)
		{
			ulCurValue1000 = ulMinValue1000;
			bSweepAscending = TRUE;
		}		
	}

	// Update cadence 
	if(bByCadence)
	{
		bpPages->ucCadence = (UCHAR) (ulCurValue1000/1000);	// We already have cadence
		ulSpeed1000 = CadenceToSpeed(ulCurValue1000);	// Get speed from cadence
	}
	else
	{
		bpPages->ucCadence = (UCHAR) (SpeedToCadence(ulCurValue1000)/1000);	// Get cadence from speed
		ulSpeed1000 = ulCurValue1000;	// We already have speed
	}

	if(bTxPedalPower)
	{
		if(bRightPedal)
		{
			UCHAR ucTemp = 0x00;
			ucTemp = ucCurPedalPwrValue | 0x80;	// 0x80 mask for right pedal power contribution
			bpPages->ucPedalPower = ucTemp;
		}
		else
			bpPages->ucPedalPower = ucCurPedalPwrValue;
	}

	// Calculate sensor event interval (in ms) based on the location of the sensor, wheel or crank
	ULONG ulEventPeriod1000;
	if(bpPages->eType == BikePower::SensorType::TORQUE_WHEEL)
	{
		if(bpPages->ucCadence)
			ulEventPeriod1000 = ((ULONG) 60000 * ucNumWheelGearTeeth)/((ULONG) bpPages->ucCadence * ucNumCrankGearTeeth); // rpm -> ms
		else
			ulEventPeriod1000 = 600000;	// No events, set interval to a very large value
	}
	else	// crank sensor
	{
		if(bpPages->ucCadence)
			ulEventPeriod1000 = (ULONG) 60000/bpPages->ucCadence; // rpm -> ms
		else
			ulEventPeriod1000 = 600000;	// No events, set interval to a very large value
	}

	// Update values, depending on type of updates (event or time synchronous)
	if(eUpdateType == BikePower::UpdateType::EVENT_SYNCH)
	{
		ulTimerInterval = ulEventPeriod1000;	// Event synchronous, the next interval corresponds to a wheel or crank rotation
		
		//If stopped and event driven, there is no update at all, because the sensor never fires
		//If coasting, event driven, and a crank mounted sensor there is no update at all, because the sensor never fires
		if(bStop || (bCoast && 
			(bpPages->eType == BikePower::SensorType::TORQUE_CRANK ||
			 bpPages->eType == BikePower::SensorType::CRANK_TORQUE_FREQ)))
			return;

		// If there is an event, update event count
		switch(bpPages->eType)
		{
		case BikePower::SensorType::TORQUE_WHEEL:
			bpPages->ucWTEventCount++;
			bpPages->ucWheelTicks++;
			break;
		case BikePower::SensorType::TORQUE_CRANK:
			bpPages->ucCTEventCount++;
			bpPages->ucCrankTicks++;
			break;
		case BikePower::SensorType::CRANK_TORQUE_FREQ:
			bpPages->ucCTFEventCount++;
			break;
		default:	// no event synch Power Only sensor
			break;
		}
	}
	else
	{ 
		ulTimerInterval = (ULONG) 10000/ucEventFreq10;		// Time syncrhonous, fixed interval.  Freq in 1/10 Hz -> ms
		
		// The data page is updated every time for time synchronous sensors, so the event count is incremented
		if(bpPages->eType == BikePower::SensorType::TORQUE_WHEEL)
			bpPages->ucWTEventCount++;
		else if(bpPages->eType == BikePower::SensorType::TORQUE_CRANK)
			bpPages->ucCTEventCount++;

		// If there are wheel or crank revolutions, the ticks also increment
		if(!bStop && (!bCoast || (bCoast && bpPages->eType == BikePower::SensorType::TORQUE_WHEEL)))
		{
			ulEventTime1000 += ulTimerInterval;
			UCHAR ucTicks = (UCHAR) (ulEventTime1000/ulEventPeriod1000);	// Number of ticks since last sensor event
			ulEventTime1000 = ulEventTime1000%ulEventPeriod1000;	// Update last sensor event time for next calculation
			
			switch(bpPages->eType)
			{
			case BikePower::SensorType::TORQUE_WHEEL:
				bpPages->ucWheelTicks += ucTicks;
				break;
			case BikePower::SensorType::TORQUE_CRANK:
				bpPages->ucCrankTicks += ucTicks;
				break;
			default:	// no time synch CTF
				break;
			}
		}
		
	}

	//If we are not pedalling there is, theoretically, no torque and thus no power; also cadence will be zero
	if(bStop || bCoast)
	{
		bpPages->usPower = 0;
		bpPages->ucCadence = 0;
		bpPages->ucPedalPower = 0;
	}
	else
	{
		bpPages->usPower = (USHORT) ( (((ULONG) PI256 * bpPages->ucCadence * usCrankTorque32)/((ULONG) 30 * 32) +  128) >> 8);	// 2*pi*Cadence*CrankTorque/60; convert CrankTorque from 1/32Nm to Nm, and constant PI256 = pi * 256.  Adding 0.5 (128/256) for rounding.
	}

	// Basic power page is updated every time (except if CTF)
	if(bpPages->eType != BikePower::SensorType::CRANK_TORQUE_FREQ)
	{
		bpPages->ucPowEventCount++;
		bpPages->usAcumPower += bpPages->usPower;
	}

	if(bpPages->eType == BikePower::SensorType::TORQUE_WHEEL)
	{
		//Don't update the wheel period if we stopped
		if(!bStop)
			bpPages->usAcumWheelPeriod2048 += (USHORT) ((ulEventPeriod1000*2048)/1000);
		// If we are coasting the wheel event is happening but there is, theoretically, no torque
		if(!bCoast)
			bpPages->usAcumTorque32 += usWheelTorque32;	// Update cumulative torque
	}
	else if(bpPages->eType == BikePower::SensorType::TORQUE_CRANK)
	{
		//Don't update the crank period if it is not spinning
		if(!bStop && !bCoast)
			bpPages->usAcumCrankPeriod2048 += (USHORT) ((ulEventPeriod1000*2048)/1000);
		// If we are coasting there is no torque
		if(!bCoast)
			bpPages->usAcumTorque32 += usCrankTorque32;	// Update cumulative torque
	}
	else if(bpPages->eType == BikePower::SensorType::CRANK_TORQUE_FREQ)
	{
		//If we are coasting there is no torque
		if(!bCoast)
			bpPages->usTorqueTicks += (USHORT) ((((ULONG) usCrankTorque32 * bpPages->usSlope10)/(32 * 10) + (ULONG) usCalOffset) * ulTimerInterval/1000);
	}

	// If transmission of cadence is disabled, blank it
	if(!bTxCadence)
		bpPages->ucCadence = BikePower::INVALID_CADENCE;

	// If transmission of pedal power is disabled, blank it
	if(!bTxPedalPower)
		bpPages->ucPedalPower = BikePower::RESERVED;
}


/***************************************************************************
 * BikePowerSensor::UpdateDisplay
 * 
 * Updates displayed simulator data on GUI
 *
 * returns: N/A
 *
 **************************************************************************/
void BikePowerSensor::UpdateDisplay()
{
	if(bTxPage82)
	{
		this->label_Bat_ElpTimeDisplay->Text = ((ULONG) (commonPages->ulOpTime & CommonData::OPERATING_TIME_MASK) * (UCHAR) commonPages->eTimeResolution).ToString();
	}

	if(ucSimDataType == SIM_SWEEP)
		this->numericUpDown_Sim_CurOutput->Value = System::Convert::ToDecimal((double)ulCurValue1000/1000);

	if(bStop)
	{
		// The speed is just a display, so we zero it so the user knows the bike is stopped
		this->label_Trn_InstSpeedDisplay->Text = "0";
		// We display zero cadence to avoid user confusion, but the value transmitted will remain the same
		this->label_Trn_CadenceDisplay->Text = "0";
		// No power can be generated with no movement
		this->label_Trn_PowerDisplay->Text = "0";
		if(bpPages->ucPedalPower != BikePower::RESERVED)
		{
			this->label_Trn_PedalPwrDisplay->Text = "0";
			this->label_Trn_PedalDisplay->Text = "---";
		}
		return;	// no other sensor data updated
	}
	else if(bCoast && bpPages->eType == BikePower::SensorType::TORQUE_CRANK)
	{
		// We display zero cadence to avoid user confusion, but the value transmitted will remain the same
		this->label_Trn_CadenceDisplay->Text = "0";
		//No power can be generated with no torque
		this->label_Trn_PowerDisplay->Text = "0";
		return;	// no other sensor data updated
	}
	
	if(bCoast)
	{
		if(bpPages->ucPedalPower != BikePower::RESERVED)
		{
			this->label_Trn_PedalPwrDisplay->Text = "0";
			this->label_Trn_PedalDisplay->Text = "---";
		}
	}

	// Not all values are directly transmitted by all sensor types, but they are calculated at the receiver
	if(!bpPages->IsCadenceInvalid(bpPages->ucCadence))
		this->label_Trn_CadenceDisplay->Text = bpPages->ucCadence.ToString();
	else
		this->label_Trn_CadenceDisplay->Text = "Off";
	this->label_Trn_InstSpeedDisplay->Text = ((double) ulSpeed1000/1000).ToString();
	this->label_Trn_PowerDisplay->Text = bpPages->usPower.ToString();

	if(bpPages->eType != BikePower::SensorType::CRANK_TORQUE_FREQ)
	{
		if(bpPages->ucPedalPower != BikePower::RESERVED)	//Pedal power byte is used
		{
			UCHAR ucTemp = 0x00;
			
			if(0x80 & bpPages->ucPedalPower)
				this->label_Trn_PedalDisplay->Text = "Right";	// Right pedal power contribution
			else
				this->label_Trn_PedalDisplay->Text = "Unknown";	// Unknown pedal power contribution
			
			ucTemp = ((bpPages->ucPedalPower) & 0x7F);	// 0x7F is a mask to remove "Pedal Differentiation Bit" and find
														// out pedal power percent value
			this->label_Trn_PedalPwrDisplay->Text = ucTemp.ToString();
		}
		else
		{
			this->label_Trn_PedalPwrDisplay->Text = "Off";
			this->label_Trn_PedalDisplay->Text = "---";
		}
	}

	// Sensor specific data
	switch(bpPages->eType)
	{
	case BikePower::SensorType::POWER_ONLY:
		this->label_Trn_UpdateCountDisplay->Text = bpPages->ucPowEventCount.ToString();
		this->label_Trn_AccumOneDisplay->Text = bpPages->usAcumPower.ToString();
		break;
	case BikePower::SensorType::TORQUE_WHEEL:
		this->label_Trn_UpdateCountDisplay->Text = bpPages->ucWTEventCount.ToString();
		this->label_Trn_EventCountDisplay->Text = bpPages->ucWheelTicks.ToString();
		this->label_Trn_AccumTwoDisplay->Text = bpPages->usAcumTorque32.ToString();
		this->label_Trn_AccumOneDisplay->Text = bpPages->usAcumWheelPeriod2048.ToString();
		break;
	case BikePower::SensorType::TORQUE_CRANK:
		this->label_Trn_UpdateCountDisplay->Text = bpPages->ucCTEventCount.ToString();
		this->label_Trn_EventCountDisplay->Text = bpPages->ucCrankTicks.ToString();
		this->label_Trn_AccumTwoDisplay->Text = bpPages->usAcumTorque32.ToString();
		this->label_Trn_AccumOneDisplay->Text = bpPages->usAcumCrankPeriod2048.ToString();
		break;
	case BikePower::SensorType::CRANK_TORQUE_FREQ:
		this->label_Trn_UpdateCountDisplay->Text = bpPages->ucCTFEventCount.ToString();
		this->label_Trn_AccumTwoDisplay->Text = bpPages->usTorqueTicks.ToString();
		break;
	default:
		break;
	}
}


/***************************************************************************
 * BikePowerSensor::UpdateCalDisplay
 * 
 * Updates received calibration data
 *
 * returns: N/A
 *
 **************************************************************************/
void BikePowerSensor::UpdateCalDisplay()
{
	// Update Auto Zero status
	if(!bpPages->bAutoZeroEnable)
		this->listBox_Cal_AZStatus->SelectedIndex = 2;	// Not supported
	else if(bpPages->bAutoZeroOn)
		this->listBox_Cal_AZStatus->SelectedIndex = 1;	// On
	else
		this->listBox_Cal_AZStatus->SelectedIndex = 0;	// Off

	// Update slope
	this->numericUpDown_Ppg_SlopeConstant->Value = (System::Decimal)bpPages->usSlope10/10;
	
	// Update serial number
	this->textBox_Glb_SerialNumChange->Text = commonPages->ulSerialNum.ToString();
}


/***************************************************************************
 * BikePowerSensor::radioButton_SensorType_CheckedChanged
 * 
 * Sets simulator type and updates interface to enable sensor specific features
 *
 * returns: N/A
 *
 **************************************************************************/
System::Void BikePowerSensor::radioButton_SensorType_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	// Power Only Sensor
	if(this->radioButton_Power->Checked)
	{	
		bpPages->eType = BikePower::SensorType::POWER_ONLY;
		this->label_Trn_AccumOne->Text = "Acc.Power:";
		this->label_Trn_AccumOne->Visible = true;
		this->label_Trn_AccumOneDisplay->Visible = true;
		this->label_Trn_AccumTwo->Visible = false;
		this->label_Trn_AccumTwoDisplay->Visible = false;
		this->checkBox_Cal_TorqAZMesg->Enabled = true;
		this->label_Cal_CalNum->Text = "Calibration Number To Send:";
		this->numericUpDown_Cal_CalNumber->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1000, 0, 0, 0});
		this->numericUpDown_Cal_CalNumber->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {32767, 0, 0, 0});
		this->numericUpDown_Cal_CalNumber->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {32768, 0, 0, System::Int32::MinValue});
		this->radioButton_Ppg_EventOnTime->Enabled = true;
		this->numericUpDown_Ppg_TimerEventFreq->Enabled = true;
		this->radioButton_Ppg_EventOnTime->Checked = true;
		this->radioButton_Ppg_EventOnRotation->Enabled = false;
		this->numericUpDown_Ppg_BasicPowerInterlvTime->Enabled = false;
		this->numericUpDown_Ppg_SlopeConstant->Enabled = false;
		this->checkBox_Bat_Status->Enabled = true;
		this->checkBox_Bat_Status->Checked = true;
		this->numericUpDown_Cal_CalNumber->Maximum = BikePower::MAX_CALDATA;
		this->numericUpDown_Cal_CalNumber->Minimum = BikePower::MIN_CALDATA;
		this->checkBox_Ppg_TxCadence->Enabled = true;
		this->checkBox_Ppg_TxPedalPwr->Enabled = true;
		this->numericUpDown_PedalPwr_CurrOutput->Enabled = true;
		this->checkBox_RightPedal->Enabled = true;
      this->tabControl_TEPS->Enabled = true;
	}
	// Wheel Torque Sensor
	else if(this->radioButton_WheelTorque->Checked)
	{
		bpPages->eType = BikePower::SensorType::TORQUE_WHEEL;
		this->label_Trn_AccumOne->Text = "Acc.Wheel P:";
		this->label_Trn_AccumTwo->Text = "Acc.Torque:";
		this->label_Trn_AccumOne->Visible = true;
		this->label_Trn_AccumOneDisplay->Visible = true;
		this->label_Trn_AccumTwo->Visible = true;
		this->label_Trn_AccumTwoDisplay->Visible = true;
		this->checkBox_Cal_TorqAZMesg->Enabled = true;
		this->label_Cal_CalNum->Text = "Calibration Number To Send:";
		this->numericUpDown_Cal_CalNumber->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1000, 0, 0, 0});
		this->numericUpDown_Cal_CalNumber->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {32767, 0, 0, 0});
		this->numericUpDown_Cal_CalNumber->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {32768, 0, 0, System::Int32::MinValue});
		this->radioButton_Ppg_EventOnTime->Enabled = true;
		this->numericUpDown_Ppg_TimerEventFreq->Enabled = true;
		this->radioButton_Ppg_EventOnRotation->Enabled = true;
		this->numericUpDown_Ppg_BasicPowerInterlvTime->Enabled = true;
		this->numericUpDown_Ppg_SlopeConstant->Enabled = false;
		this->checkBox_Bat_Status->Enabled = true;
		this->checkBox_Bat_Status->Checked = true;
		this->numericUpDown_Cal_CalNumber->Maximum = BikePower::MAX_CALDATA;
		this->numericUpDown_Cal_CalNumber->Minimum = BikePower::MIN_CALDATA;
		this->checkBox_Ppg_TxCadence->Enabled = true;
		this->checkBox_Ppg_TxPedalPwr->Enabled = true;
		this->numericUpDown_PedalPwr_CurrOutput->Enabled = true;
		this->checkBox_RightPedal->Enabled = true;
      this->tabControl_TEPS->Enabled = true;

	}
	// Crank Torque Sensor
	else if(this->radioButton_CrankTorque->Checked)
	{
		bpPages->eType = BikePower::SensorType::TORQUE_CRANK;
		this->label_Trn_AccumOne->Text = "Acc.Crank P:";
		this->label_Trn_AccumTwo->Text = "Acc.Torque:";
		this->label_Trn_AccumOne->Visible = true;
		this->label_Trn_AccumOneDisplay->Visible = true;
		this->label_Trn_AccumTwo->Visible = true;
		this->label_Trn_AccumTwoDisplay->Visible = true;
		this->checkBox_Cal_TorqAZMesg->Enabled = true;
		this->label_Cal_CalNum->Text = "Calibration Number To Send:";
		this->numericUpDown_Cal_CalNumber->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1000, 0, 0, 0});
		this->numericUpDown_Cal_CalNumber->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {32767, 0, 0, 0});
		this->numericUpDown_Cal_CalNumber->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {32768, 0, 0, System::Int32::MinValue});
		this->radioButton_Ppg_EventOnTime->Enabled = true;
		this->numericUpDown_Ppg_TimerEventFreq->Enabled = true;
		this->radioButton_Ppg_EventOnRotation->Enabled = true;
		this->numericUpDown_Ppg_BasicPowerInterlvTime->Enabled = true;
		this->numericUpDown_Ppg_SlopeConstant->Enabled = false;
		this->checkBox_Bat_Status->Enabled = true;
		this->checkBox_Bat_Status->Checked = true;
		this->numericUpDown_Cal_CalNumber->Maximum = BikePower::MAX_CALDATA;
		this->numericUpDown_Cal_CalNumber->Minimum = BikePower::MIN_CALDATA;
		this->checkBox_Ppg_TxCadence->Enabled = true;
		this->checkBox_Ppg_TxPedalPwr->Enabled = true;
		this->numericUpDown_PedalPwr_CurrOutput->Enabled = true;
		this->checkBox_RightPedal->Enabled = true;
      this->tabControl_TEPS->Enabled = true;
	}
	// Crank Torque Frequency Sensor
	else
	{ 
		bpPages->eType = BikePower::SensorType::CRANK_TORQUE_FREQ;
		this->label_Trn_AccumTwo->Text = "Acc.Torque:";
		this->label_Trn_AccumOne->Visible = false;
		this->label_Trn_AccumOneDisplay->Visible = false;
		this->label_Trn_AccumTwo->Visible = true;
		this->label_Trn_AccumTwoDisplay->Visible = true;
		this->checkBox_Cal_TorqAZMesg->Checked = false;
		this->checkBox_Cal_TorqAZMesg->Enabled = false;
		this->label_Cal_CalNum->Text = "Current Torque Ticks Offset:";
		this->numericUpDown_Cal_CalNumber->Value = 100;
		this->numericUpDown_Cal_CalNumber->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {100, 0, 0, 0});
		this->numericUpDown_Cal_CalNumber->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {65535, 0, 0, 0});
		this->numericUpDown_Cal_CalNumber->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {0, 0, 0, 0});
		this->radioButton_Ppg_EventOnRotation->Enabled = true;
		this->numericUpDown_Ppg_BasicPowerInterlvTime->Enabled = false;
		this->numericUpDown_Ppg_SlopeConstant->Enabled = true;
		this->radioButton_Ppg_EventOnRotation->Checked = true;
		this->radioButton_Ppg_EventOnTime->Enabled = false;
		this->numericUpDown_Ppg_TimerEventFreq->Enabled = false;
		this->checkBox_Bat_Status->Enabled = false;
		this->checkBox_Bat_Status->Checked = false;
		// Validate calibration offset
		if(this->numericUpDown_Cal_CalNumber->Value < BikePower::MIN_OFFSET)
			this->numericUpDown_Cal_CalNumber->Value = BikePower::MIN_OFFSET;
		else if(this->numericUpDown_Cal_CalNumber-> Value > BikePower::MAX_OFFSET)
			this->numericUpDown_Cal_CalNumber->Value = BikePower::MIN_OFFSET;
		this->numericUpDown_Cal_CalNumber->Maximum = BikePower::MAX_OFFSET;
		this->numericUpDown_Cal_CalNumber->Minimum = BikePower::MIN_OFFSET;
		this->checkBox_Ppg_TxCadence->Enabled = false;
		this->checkBox_Ppg_TxPedalPwr->Enabled = false;
		this->numericUpDown_PedalPwr_CurrOutput->Enabled = false;
		this->checkBox_RightPedal->Enabled = false;
		this->label_Trn_PedalDisplay->Text = "---";
		this->label_Trn_PedalPwrDisplay->Text = "---";
      this->tabControl_TEPS->Enabled = false;
      this->checkBox_EnableTEPS->Checked = false;
	}
}


/**************************************************************************
 * BikePowerSensor::numericUpDown_Ppg_BasicPowerInterlvTime_ValueChanged
 * 
 * Updates interleaving period for basic power page
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void BikePowerSensor::numericUpDown_Ppg_BasicPowerInterlvTime_ValueChanged(System::Object^  sender, System::EventArgs^  e) 
{
	ucPowerInterleave = System::Convert::ToByte(this->numericUpDown_Ppg_BasicPowerInterlvTime->Value);
}


/**************************************************************************
 * BikePowerSensor::numericUpDown_Ppg_TimerEventFreq_ValueChanged
 * 
 * Updates event frequency for time synchronous simulation
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void BikePowerSensor::numericUpDown_Ppg_TimerEventFreq_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	ucEventFreq10 = System::Convert::ToByte(10 * this->numericUpDown_Ppg_TimerEventFreq->Value);
}


/**************************************************************************
 * BikePowerSensor::radioButton_Ppg_UpdateType_CheckedChanged
 * 
 * Updates type of simulation (event/time synch)
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void BikePowerSensor::radioButton_Ppg_UpdateType_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	if(this->radioButton_Ppg_EventOnRotation->Checked)
		eUpdateType = BikePower::UpdateType::EVENT_SYNCH;
	else
		eUpdateType = BikePower::UpdateType::TIME_SYNCH;
}

/**************************************************************************
 * BikePowerSensor::button_GlobalDataUpdate_Click
 * 
 * Validates and updates product information, from user input (GUI)
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void BikePowerSensor::button_GlobalDataUpdate_Click(System::Object^  sender, System::EventArgs^  e) 
{
	this->label_Glb_GlobalDataError->Visible = false;
	this->label_Glb_GlobalDataError->Text = "Error: ";
	// Convert and catch failed conversions
	try{
		commonPages->usMfgID = System::Convert::ToUInt16(this->textBox_Glb_ManfIDChange->Text);
	}
	catch(...){
		this->label_Glb_GlobalDataError->Text = System::String::Concat(this->label_Glb_GlobalDataError->Text, " MFID");
		this->label_Glb_GlobalDataError->Visible = true;
	}
	try{
		commonPages->ulSerialNum = System::Convert::ToUInt32(this->textBox_Glb_SerialNumChange->Text);
	}
	catch(...){
		this->label_Glb_GlobalDataError->Text = System::String::Concat(this->label_Glb_GlobalDataError->Text, " Ser#");
		this->label_Glb_GlobalDataError->Visible = true;
	}
	try{
		commonPages->ucHwVersion = System::Convert::ToByte(this->textBox_Glb_HardwareVerChange->Text);
	}
	catch(...){
		this->label_Glb_GlobalDataError->Text = System::String::Concat(this->label_Glb_GlobalDataError->Text, " HWVr");
		this->label_Glb_GlobalDataError->Visible = true;
	}
	try{
		commonPages->ucSwVersion = System::Convert::ToByte(this->textBox_Glb_SoftwareVerChange->Text);
	}
	catch(...){
		this->label_Glb_GlobalDataError->Text = System::String::Concat(this->label_Glb_GlobalDataError->Text, " SWVr");
		this->label_Glb_GlobalDataError->Visible = true;
	}
	try{
		commonPages->usModelNum = System::Convert::ToUInt16(this->textBox_Glb_ModelNumChange->Text);
	}
	catch(...){
		this->label_Glb_GlobalDataError->Text = System::String::Concat(this->label_Glb_GlobalDataError->Text, " Mdl#");
		this->label_Glb_GlobalDataError->Visible = true;
	}
}

/**************************************************************************
 * BikePowerSensor::bcheckBox_Bat_Status_CheckedChanged
 * 
 * Enables/disables transmission of battery page
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void BikePowerSensor::checkBox_Bat_Status_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	bTxPage82 = this->checkBox_Bat_Status->Checked;
}


/**************************************************************************
 * BikePowerSensor::radioButton_Bat_ElpUnits_CheckedChanged
 * 
 * Selects time resolution (two or sixteen seconds)
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void BikePowerSensor::radioButton_Bat_ElpUnits_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(this->radioButton_Bat_Elp2Units->Checked && commonPages->eTimeResolution == CommonData::TimeResolution::SIXTEEN)
	{
		commonPages->eTimeResolution = CommonData::TimeResolution::TWO;
		commonPages->ulOpTime  = commonPages->ulOpTime << 3; // OpTime*8: Convert from interval of 16 seconds to interval of 2 seconds
	}
	else if(this->radioButton_Bat_Elp16Units->Checked && commonPages->eTimeResolution == CommonData::TimeResolution::TWO) 
	{
		commonPages->eTimeResolution = CommonData::TimeResolution::SIXTEEN;
		commonPages->ulOpTime = commonPages->ulOpTime >> 3;		// OpTime/8: Convert from interval of 2 seconds to interval of 16 seconds
	}
	this->label_Bat_ElpTimeDisplay->Text = ((ULONG) (commonPages->ulOpTime & CommonData::OPERATING_TIME_MASK) * (UCHAR) commonPages->eTimeResolution).ToString();
	ulRunTime1000 = 0;	// reset run time value so that next update happens in 2 or 16 seconds
}


/**************************************************************************
 * BikePowerSensor::button_ElpTimeUpdate_Click
 * 
 * Updates cumulative operating time (in seconds)
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void BikePowerSensor::button_ElpTimeUpdate_Click(System::Object^  sender, System::EventArgs^  e) 
{
	this->button_Bat_ElpTimeUpdate->Text = "Update";
	try{
		ULONG ulTimeS = System::Convert::ToUInt32(this->textBox_Bat_ElpTimeChange->Text);	// Get time in seconds
		if(commonPages->eTimeResolution == CommonData::TimeResolution::TWO)	// Conver to two second resolution
		{
			if(ulTimeS > CommonData::MAX_2SEC)
				ulTimeS = CommonData::MAX_2SEC;
			commonPages->ulOpTime = ulTimeS >> 1;  // Time/2
		}
		else
		{	// Convert to sixteen second resolution
			if(ulTimeS > CommonData::MAX_16SEC)
				ulTimeS = CommonData::MAX_16SEC;
			commonPages->ulOpTime = ulTimeS >> 4;	// Time/16
		}
		this->label_Bat_ElpTimeDisplay->Text = ulTimeS.ToString();
	}
	catch(...){
		this->button_Bat_ElpTimeUpdate->Text = "Retry";	// Invalid input, try again
	}
}


/**************************************************************************
 * BikePowerSensor::numericUpDown_Bat_VoltInt_ValueChanged
 * 
 * Updates coarse (integer) battery voltage
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void BikePowerSensor::numericUpDown_Bat_VoltInt_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	commonPages->usBatVoltage256 = ((USHORT) System::Convert::ToByte(this->numericUpDown_Bat_VoltInt->Value)) << 8 | (commonPages->usBatVoltage256 & 0xFF);	// Integer portion in high byte
	this->label_Voltage_Display->Text = ((double) commonPages->usBatVoltage256/256).ToString();	// Display in V
}

/**************************************************************************
 * BikePowerSensor::numericUpDown_Bat_VoltInt_ValueChanged
 * 
 * Updates fractional battery voltage
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void BikePowerSensor::numericUpDown_Bat_VoltFrac_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	commonPages->usBatVoltage256 = System::Convert::ToByte(this->numericUpDown_Bat_VoltFrac->Value) | (commonPages->usBatVoltage256 & 0xFF00);	// Fractional portion in low byte
	this->label_Voltage_Display->Text = ((double) commonPages->usBatVoltage256/256).ToString();	// Display in V
}

/**************************************************************************
 * BikePowerSensor::checkBox_Bat_Voltage_CheckedChanged
 * 
 * Enables/disables coarse battery voltage
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void BikePowerSensor::checkBox_Bat_Voltage_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	this->numericUpDown_Bat_VoltInt->Enabled = this->checkBox_Bat_Voltage->Checked;
	this->numericUpDown_Bat_VoltFrac->Enabled = this->checkBox_Bat_Voltage->Checked;
	this->checkBox_Bat_FracVolt->Enabled = this->checkBox_Bat_Voltage->Checked;
	if(!this->checkBox_Bat_Voltage->Checked)
	{
		commonPages->usBatVoltage256 = 	(USHORT) CommonData::BATTERY_VOLTAGE_INVALID << 8 | (commonPages->usBatVoltage256 & 0xFF);	// Integer portion in high byte
		this->label_Voltage_Display->Text = "Off";
	}
	else
	{
		commonPages->usBatVoltage256 = ((USHORT) System::Convert::ToByte(this->numericUpDown_Bat_VoltInt->Value)) << 8 | (commonPages->usBatVoltage256 & 0xFF);	// Integer portion in high byte
		this->label_Voltage_Display->Text = ((double) commonPages->usBatVoltage256/256).ToString();	// Display in V
	}
}


/**************************************************************************
 * BikePowerSensor::checkBox_Bat_FracVolt_CheckedChanged
 * 
 * Enables/disables fractional battery voltage
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void BikePowerSensor::checkBox_Bat_FracVolt_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	this->numericUpDown_Bat_VoltFrac->Enabled = this->checkBox_Bat_FracVolt->Checked;
	if(!this->checkBox_Bat_FracVolt->Checked)
		commonPages->usBatVoltage256 &= 0xFF00;	// Fractional portion in low byte, set to zero
	else
		commonPages->usBatVoltage256 = System::Convert::ToByte(this->numericUpDown_Bat_VoltFrac->Value) | (commonPages->usBatVoltage256 & 0xFF00);	// Fractional portion in low byte, from GUI
	this->label_Voltage_Display->Text = ((double) commonPages->usBatVoltage256/256).ToString();	// Display in V
}


/**************************************************************************
 * BikePowerSensor::listBox_Bat_Status_SelectedIndexChanged
 * 
 * Sets battery status
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void BikePowerSensor::listBox_Bat_Status_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
{
	if(this->listBox_Bat_Status->SelectedIndex == 5) // Set manually as indexes don't match enum (due to reserved bits)
		commonPages->eBatStatus = CommonData::BatStatus::INVALID;
	else
		commonPages->eBatStatus = (CommonData::BatStatus) (this->listBox_Bat_Status->SelectedIndex + 1);

}

/**************************************************************************
 * BikePowerSensor::numericUpDown_Ppg_SlopeConstant_ValueChanged
 * 
 * Updates slope
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void BikePowerSensor::numericUpDown_Ppg_SlopeConstant_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	bpPages->usSlope10 = System::Convert::ToUInt16(10 * this->numericUpDown_Ppg_SlopeConstant->Value);		
}


/**************************************************************************
 * BikePowerSensor::numericUpDown_Sim_WheelCircumference_ValueChanged
 * 
 * Updates wheel circumference (cm)
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void BikePowerSensor::numericUpDown_Sim_WheelCircumference_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	ucWheelCircumference100 = System::Convert::ToByte(this->numericUpDown_Sim_WheelCircumference->Value);
}


/**************************************************************************
 * BikePowerSensor::checkBox_Sim_Coast_CheckedChanged
 * 
 * Enables/disables coasting
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void BikePowerSensor::checkBox_Sim_Coast_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	bCoast = this->checkBox_Sim_Coast->Checked;
	this->checkBox_Sim_Stop->Enabled = !bCoast;
}


/**************************************************************************
 * BikePowerSensor::checkBox_Sim_Stop_CheckedChanged
 * 
 * Enables/disables stopping
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void BikePowerSensor::checkBox_Sim_Stop_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	bStop = this->checkBox_Sim_Stop->Checked;
	this->checkBox_Sim_Coast->Enabled = !bStop;
}


/**************************************************************************
 * BikePowerSensor::checkBox_Sim_Sweeping_CheckedChanged
 * 
 * Enables/disables sweeping
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void BikePowerSensor::checkBox_Sim_Sweeping_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	this->numericUpDown_Sim_CurOutput->Enabled = !this->numericUpDown_Sim_CurOutput->Enabled;
	this->numericUpDown_Sim_MinOutput->Enabled = !this->numericUpDown_Sim_MinOutput->Enabled;
	this->numericUpDown_Sim_MaxOutput->Enabled = !this->numericUpDown_Sim_MaxOutput->Enabled;

	if(this->checkBox_Sim_Sweeping->Checked)
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
 * BikePowerSensor::numericUpDown_Sim_WheelTorque_ValueChanged
 * 
 * Updates wheel and crank torque
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void BikePowerSensor::numericUpDown_Sim_WheelTorque_ValueChanged(System::Object^  sender, System::EventArgs^  e) 
{
	usWheelTorque32 = System::Convert::ToUInt16(32 * this->numericUpDown_Sim_WheelTorque->Value);	// Update wheel torque from UI
	CalculateCrankTorque();
	this->numericUpDown_Sim_WheelTorque->Value = (System::Decimal) usWheelTorque32/32;	// Update values displayed
	this->numericUpDown_Sim_CrankTorque->Value = (System::Decimal) usCrankTorque32/32;
}



/**************************************************************************
 * BikePowerSensor::numericUpDown_Sim_CrankTorque_ValueChanged
 * 
 * Updates wheel and crank torque
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void BikePowerSensor::numericUpDown_Sim_CrankTorque_ValueChanged(System::Object^  sender, System::EventArgs^  e) 
{
	usCrankTorque32 = System::Convert::ToUInt16(32 * this->numericUpDown_Sim_CrankTorque->Value);	// Update crank torque from UI
	CalculateWheelTorque();
	this->numericUpDown_Sim_CrankTorque->Value = (System::Decimal) usCrankTorque32/32;	// Update values displayed
	this->numericUpDown_Sim_WheelTorque->Value = (System::Decimal) usWheelTorque32/32;
}


/**************************************************************************
 * BikePowerSensor::numericUpDown_Sim_CrankGearTeeth_ValueChanged
 * 
 * Updates crank gear teeth, and wheel/crank torque
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void BikePowerSensor::numericUpDown_Sim_CrankGearTeeth_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	ucNumCrankGearTeeth = System::Convert::ToByte(this->numericUpDown_Sim_CrankGearTeeth->Value);	// Update from UI
	CalculateWheelTorque();
	this->numericUpDown_Sim_CrankTorque->Value = (System::Decimal) usCrankTorque32/32;	// Update values displayed
	this->numericUpDown_Sim_WheelTorque->Value = (System::Decimal) usWheelTorque32/32;
}

/**************************************************************************
 * BikePowerSensor::numericUpDown_Sim_WheelGearTeeth_ValueChanged
 * 
 * Updates wheel gear teeth, and wheel/crank torque
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void BikePowerSensor::numericUpDown_Sim_WheelGearTeeth_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	ucNumWheelGearTeeth = System::Convert::ToByte(this->numericUpDown_Sim_WheelGearTeeth->Value);	// Update from UI
	CalculateCrankTorque();
	this->numericUpDown_Sim_WheelTorque->Value = (System::Decimal) usWheelTorque32/32;	// Update values displayed
	this->numericUpDown_Sim_CrankTorque->Value = (System::Decimal) usCrankTorque32/32;
}


/**************************************************************************
 * BikePowerSensor::numericUpDown_PedalPwr_CurOutput_ValueChanged
 * 
 * Updates the current power pedal value, when it changes
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void BikePowerSensor::numericUpDown_PedalPwr_CurrOutput_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
		ucCurPedalPwrValue = System::Convert::ToByte(this->numericUpDown_PedalPwr_CurrOutput->Value);	//Update from UI
}


/**************************************************************************
 * BikePowerSensor::numericUpDown_Sim_CurOutput_ValueChanged
 * 
 * Updates and converts units of the current speed or cadence value, when it changes
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void BikePowerSensor::numericUpDown_Sim_CurOutput_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	// Ignore changes when sweeping
	if(this->numericUpDown_Sim_CurOutput->Enabled)
	{
		ulCurValue1000 = System::Convert::ToUInt32(1000 * this->numericUpDown_Sim_CurOutput->Value);	// Convert to 1/1000 resolution
		ForceUpdate();
	}
}


/**************************************************************************
 * BikePowerSensor::numericUpDown_Sim_MinMaxOutput_ValueChanged
 * 
 * If the user has changed the min or max speed, validate that
 * minimum < current <  maximum
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void BikePowerSensor::numericUpDown_Sim_MinMaxOutput_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	ULONG ulPrevValue1000 = ulCurValue1000;

	// Check only if user changes values (not sweeping)
	if(this->numericUpDown_Sim_MinOutput->Enabled && this->numericUpDown_Sim_MaxOutput->Enabled)
	{
		// Check min<max, and force min<cur<max
		if(this->numericUpDown_Sim_MinOutput->Value < this->numericUpDown_Sim_MaxOutput->Value)
		{
			ulMinValue1000 = System::Convert::ToUInt32(1000 * this->numericUpDown_Sim_MinOutput->Value);
			ulMaxValue1000 = System::Convert::ToUInt32(1000 * this->numericUpDown_Sim_MaxOutput->Value);
			if(ulCurValue1000 > ulMaxValue1000)
				ulCurValue1000 = ulMaxValue1000;
			else if(ulCurValue1000 < ulMinValue1000)
				ulCurValue1000 = ulMinValue1000;
			if(ulCurValue1000 != ulPrevValue1000)
			{
				this->numericUpDown_Sim_CurOutput->Value = System::Convert::ToDecimal((double)ulCurValue1000/1000);
				ForceUpdate();
			}
		}
		else
		{
			// If the values were invalid, set numeric values to last valid values
			this->numericUpDown_Sim_MinOutput->Value = System::Convert::ToDecimal((double)ulMinValue1000/1000);
			this->numericUpDown_Sim_MaxOutput->Value = System::Convert::ToDecimal((double)ulMaxValue1000/1000);
		}
	}
}


/**************************************************************************
 * BikePowerSensor::radioButton_SimByChanged
 * 
 * Selects simulation by cadence or speed, and converts last value to appropiate
 * unit for consistency
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void BikePowerSensor::radioButton_SimByChanged(System::Object^  sender, System::EventArgs^  e) 
{
	bByCadence = (BOOL) this->radioButton_Sim_SimByCadence->Checked;

	// Disable boxes momentarily, to keep values from being checked and modified again as they change
	this->numericUpDown_Sim_CurOutput->Enabled = FALSE;
	this->numericUpDown_Sim_MaxOutput->Enabled = FALSE;
	this->numericUpDown_Sim_MinOutput->Enabled = FALSE;

	// Update input data
	if(bByCadence)
	{
		// Selected cadence (switching from speed to cadence)
		// Latest speed and cadence values are maintained, so the values that represented speed are converted to cadence
		ulCurValue1000 = SpeedToCadence(ulCurValue1000);	
		ulMinValue1000 = SpeedToCadence(ulMinValue1000);
		ulMaxValue1000 = SpeedToCadence(ulMaxValue1000);
	}
	else
	{
		// Selected speed (switching from cadence to speed)
		// Latest speed and cadence value are maintained, so values representing cadence are converted to speed
		ulCurValue1000 = CadenceToSpeed(ulCurValue1000);
		ulMinValue1000 = CadenceToSpeed(ulMinValue1000);
		ulMaxValue1000 = CadenceToSpeed(ulMaxValue1000);
	}

	// Validate values do not exceed max
	if(ulCurValue1000 > 255000)
		ulCurValue1000 = 255000;
	if(ulMinValue1000 > 255000)
		ulMinValue1000 = 255000;
	if(ulMaxValue1000 > 255000)
		ulMaxValue1000 = 255000;

	// Update output
	this->numericUpDown_Sim_CurOutput->Value = System::Convert::ToDecimal((double)ulCurValue1000/1000);
	this->numericUpDown_Sim_MinOutput->Value = System::Convert::ToDecimal((double)ulMinValue1000/1000);
	this->numericUpDown_Sim_MaxOutput->Value = System::Convert::ToDecimal((double)ulMaxValue1000/1000);

	// Enable input boxes again so that they can be modified by user
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
 * BikePowerSensor::checkBox_Cal_TorqAZMesg_CheckedChanged
 * 
 * Enables/disables transmission of auto zero support page
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void BikePowerSensor::checkBox_Cal_TorqAZMesg_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	bTxAZ = (BOOL) this->checkBox_Cal_TorqAZMesg->Checked;
}


/**************************************************************************
 * BikePowerSensor::listBox_Cal_AZStatus_SelectedIndexChanged
 * 
 * Sets auto zero status
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void BikePowerSensor::listBox_Cal_AZStatus_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
{
	switch(this->listBox_Cal_AZStatus->SelectedIndex)
	{
	case 0:
		bpPages->bAutoZeroEnable = TRUE;
		bpPages->bAutoZeroOn = FALSE;
		break;
	case 1:
		bpPages->bAutoZeroEnable = TRUE;
		bpPages->bAutoZeroOn = TRUE;
		break;
	case 2:
		bpPages->bAutoZeroEnable = FALSE;
		bpPages->bAutoZeroOn = FALSE;
		break;
	default:
		break;
	}
}


/**************************************************************************
 * BikePowerSensor::numericUpDown_Cal_RawTorque_ValueChanged
 * 
 * Updates raw torque for auto zero support page
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void BikePowerSensor::numericUpDown_Cal_RawTorque_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	bpPages->sRawTorque = System::Convert::ToInt16(this->numericUpDown_Cal_RawTorque->Value);
}


/**************************************************************************
 * BikePowerSensor::numericUpDown_Cal_OffsetTorque_ValueChanged
 * 
 * Updates offset torque for auto zero support page
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void BikePowerSensor::numericUpDown_Cal_OffsetTorque_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	bpPages->sOffsetTorque = System::Convert::ToInt16(this->numericUpDown_Cal_OffsetTorque->Value);
}


/**************************************************************************
 * BikePowerSensor::numericUpDown_Cal_CalNumber_ValueChanged
 * 
 * Updates the calibration number to send, depending on the sensor type
 * When dealing with CTF sensors, the input is converted to an unsigned short,
 * and when dealing with other sensors, the input is converted to a signed
 * short
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void BikePowerSensor::numericUpDown_Cal_CalNumber_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	if(bpPages->eType == BikePower::SensorType::CRANK_TORQUE_FREQ)
		usCalOffset = System::Convert::ToUInt16(this->numericUpDown_Cal_CalNumber->Value);
	else
		sCalData = System::Convert::ToInt16(this->numericUpDown_Cal_CalNumber->Value);
}


/**************************************************************************
 * BikePowerSensor::radioButton_Cal_Success_CheckedChanged
 * 
 * Sets next calibration response to report a success or failure
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void BikePowerSensor::radioButton_Cal_Success_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	bCalSuccess = (BOOL) this->radioButton_Cal_Success->Checked;
}


/**************************************************************************
 * BikePowerSensor::checkBox_Ppg_TxCadence_CheckedChanged
 * 
 *  Enables/disables transmission of cadence in power and torque data pages
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void BikePowerSensor::checkBox_Ppg_TxCadence_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	bTxCadence = (BOOL) this->checkBox_Ppg_TxCadence->Checked;
}


/**************************************************************************
 * BikePowerSensor::checkBox_Ppg_TxPedalPwr_CheckedChanged
 * 
 *  Enables/disables transmission of pedal power in power data page
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void BikePowerSensor::checkBox_Ppg_TxPedalPwr_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	bTxPedalPower = (BOOL) this->checkBox_Ppg_TxPedalPwr->Checked;

	if(bTxPedalPower)
	{
		this->numericUpDown_PedalPwr_CurrOutput->Enabled = true;
		this->checkBox_RightPedal->Enabled = true;
	}
	else
	{
		this->numericUpDown_PedalPwr_CurrOutput->Enabled = false;
		this->checkBox_RightPedal->Enabled = false;
	}
}


/**************************************************************************
 * BikePowerSensor::checkBox_RightPedal_CheckedChanged
 * 
 *  Enables/disables right pedal % power contribution
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void BikePowerSensor::checkBox_RightPedal_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	bRightPedal = (BOOL) this->checkBox_RightPedal->Checked;
}


/**************************************************************************
 * BikePowerSensor::CalculateWheelTorque
 * 
 * Calculates wheel torque from crank torque, and updates the crank torque
 * if the value is invalid
 * 
 * returns: N/A
 *
 **************************************************************************/
void BikePowerSensor::CalculateWheelTorque()
{
	usWheelTorque32 = (ucNumWheelGearTeeth * usCrankTorque32)/ucNumCrankGearTeeth;	// Calculate wheel torque
	if(usWheelTorque32 > 3196)	// 99.9 * 32
	{
		usWheelTorque32 = 3196;
		usCrankTorque32 = (ucNumCrankGearTeeth * usWheelTorque32)/ucNumWheelGearTeeth;	// Calculate crank torque from max wheel torque
	}
}


/**************************************************************************
 * BikePowerSensor::CalculateCrankTorque
 * 
 * Calculates crank torque from wheel torque, and updates the wheel torque
 * if the value is invalid
 * 
 * returns: N/A
 *
 **************************************************************************/
void BikePowerSensor::CalculateCrankTorque()
{
	usCrankTorque32 = (ucNumCrankGearTeeth * usWheelTorque32)/ucNumWheelGearTeeth;		// Calculate crank torque
	if(usCrankTorque32 > 3196)	// 99.9 * 32
	{
		usCrankTorque32 = 3196;
		usWheelTorque32 = (ucNumWheelGearTeeth * usCrankTorque32)/ucNumCrankGearTeeth;	// Calculate wheel torque from max crank torque
	}
}


/**************************************************************************
 * BikePowerSensor::SpeedToCadence
 * 
 * Converts from speed (m/h) to cadence (1/1000 rpm)
 *
 * ulSpeed1000_ : Speed (m/h)
 * 
 * returns: Cadence (1/1000 rpm)
 *
 **************************************************************************/
ULONG BikePowerSensor::SpeedToCadence(ULONG ulSpeed1000_)
{
	ULONG ulCadence1000;

	ulCadence1000 = 100 * ((ulSpeed1000_ * 1000 * ucNumWheelGearTeeth)/((ULONG) 60 * ucWheelCircumference100 * ucNumCrankGearTeeth));	// m/h to rpm,  circumference is in cm
	
	return ulCadence1000;	
}


/**************************************************************************
 * BikePowerSensor::CadenceToSpeed
 * 
 * Converts from cadence (1/1000 rpm) to speed (m/h)
 *
 * ulCadence1000_ : Cadence (1/1000 rpm)
 * 
 * returns: Speed (m/h)
 *
 **************************************************************************/
ULONG BikePowerSensor::CadenceToSpeed(ULONG ulCadence1000_)
{
	ULONG ulSpeed1000;

	ulSpeed1000 = (ulCadence1000_/100 * 60 * ucWheelCircumference100 * ucNumCrankGearTeeth)/(1000* ucNumWheelGearTeeth);	// rpm to m/h, circumference is in cm

	return ulSpeed1000;
}


/**************************************************************************
 * BikePowerSensor::ForceUpdate
 * 
 * Causes a timer event, to force the simulator to update all calculations
  * 
 * returns: N/A
 *
 **************************************************************************/
void BikePowerSensor::ForceUpdate()
{
	// Only performed in event synchronous simulations, to prevent simulator from 
	// getting stuck on low speed/cadence values
	if(eUpdateType == BikePower::UpdateType::EVENT_SYNCH)
		timerHandle->Interval = 250;
}
/**************************************************************************
 * BikePowerSensor::checkBox_InvalidSerial_CheckedChanged
 * 
 * Handles the CheckedChanged event 
 * 
 * returns: N/A
 *
 **************************************************************************/
void BikePowerSensor::checkBox_InvalidSerial_CheckedChanged(System::Object ^sender, System::EventArgs ^e)
{
	if(this->checkBox_InvalidSerial->Checked)
		this->textBox_Glb_SerialNumChange->Enabled = false;
	else
		this->textBox_Glb_SerialNumChange->Enabled = true;
}
void BikePowerSensor::button_UpdateGetSet_Click(System::Object ^sender, System::EventArgs ^e)
{
	bpPages->ucSubpageNumber = 0x01;

	if(checkBox_InvalidCrankLength->Checked)
		bpPages->ucCrankLength = BikePower::INVALID_CRANK_LENGTH;
	else
		bpPages->ucCrankLength = (UCHAR)(((float)numericUpDown_CrankLength->Value - (float)110.0) / (float)0.5);

	//clear the sensor status;
	bpPages->ucSensorStatus = 0;

	if(radioButton_SensorUndefined->Checked)
		bpPages->ucSensorStatus |= ((UCHAR)BikePower::SensorStatus::UNDEFINED << 4);
	else if(radioButton_SensorLeft->Checked)
		bpPages->ucSensorStatus |= ((UCHAR)BikePower::SensorStatus::LEFT_PRESENT << 4);
	else if(radioButton_SensorRight->Checked)
		bpPages->ucSensorStatus |= ((UCHAR)BikePower::SensorStatus::RIGHT_PRESENT << 4);
	else if(radioButton_SensorBoth->Checked)
		bpPages->ucSensorStatus |= ((UCHAR)BikePower::SensorStatus::LEFT_RIGHT_PRESENT << 4);

	if(radioButton_MismatchUndefined->Checked)
		bpPages->ucSensorStatus |= ((UCHAR)BikePower::SWMistmatchStatus::UNDEFINED << 2);
	else if(radioButton_MismatchRight->Checked)
		bpPages->ucSensorStatus |= ((UCHAR)BikePower::SWMistmatchStatus::RIGHT_SENSOR_OLDER << 2);
	else if(radioButton_MismatchLeft->Checked)
		bpPages->ucSensorStatus |= ((UCHAR)BikePower::SWMistmatchStatus::LEFT_SENSOR_OLDER << 2);
	else if(radioButton_MismatchNone->Checked)
		bpPages->ucSensorStatus |= ((UCHAR)BikePower::SWMistmatchStatus::SW_SAME << 2);

	if(radioButton_CrankInvalid->Checked)
		bpPages->ucSensorStatus |= ((UCHAR)BikePower::CrankLengthStatus::LENGTH_INVALID);
	else if(radioButton_CrankDefault->Checked)
		bpPages->ucSensorStatus |= ((UCHAR)BikePower::CrankLengthStatus::DEFAULT_LENGTH_USED);
	else if(radioButton_CrankManual->Checked)
		bpPages->ucSensorStatus |= ((UCHAR)BikePower::CrankLengthStatus::LENGTH_MANUALLY_SET);
	else if(radioButton_CrankAuto->Checked)
		bpPages->ucSensorStatus |= ((UCHAR)BikePower::CrankLengthStatus::LENGTH_AUTOMATICALLY_SET);

	//Clear auto crank bit
	bpPages->ucSensorCapabilities &= 0xFE;

	// Set auto crank bit
	if(checkBox_AutoCrank->Checked)
		bpPages->ucSensorCapabilities |= 0x01; 
}
void BikePowerSensor::checkBox_InvalidCrankLength_CheckedChanged(System::Object ^sender, System::EventArgs ^e)
{
	if(checkBox_InvalidCrankLength->Checked)
		numericUpDown_CrankLength->Enabled = false;
	else
		numericUpDown_CrankLength->Enabled = true;
}

System::Void BikePowerSensor::checkBox_LeftTorqueEffectivenessInvalid_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
   numericUpDown_LeftTorqueEffectiveness->Enabled = !checkBox_LeftTorqueEffectivenessInvalid->Checked;  
   if(checkBox_LeftTorqueEffectivenessInvalid->Checked)
      bpPages->ucLeftTorqueEffectiveness = bpPages->INVALID_TEPS;
   else
      bpPages->ucLeftTorqueEffectiveness = (UCHAR) (numericUpDown_LeftTorqueEffectiveness->Value * 2);
}

System::Void BikePowerSensor::checkBox_RightTorqueEffectivenessInvalid_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
   numericUpDown_RightTorqueEffectiveness->Enabled = !checkBox_RightTorqueEffectivenessInvalid->Checked;
   if(checkBox_RightTorqueEffectivenessInvalid->Checked)
      bpPages->ucRightTorqueEffectiveness = bpPages->INVALID_TEPS;
   else
      bpPages->ucRightTorqueEffectiveness = (UCHAR) (numericUpDown_RightTorqueEffectiveness->Value *2);
}

System::Void BikePowerSensor::checkBox_LeftPedalSmoothnessInvalid_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
   numericUpDown_LeftPedalSmoothness->Enabled = !checkBox_LeftPedalSmoothnessInvalid->Checked;
   if(checkBox_LeftPedalSmoothnessInvalid->Checked)
      bpPages->ucLeftPedalSmoothness = bpPages->INVALID_TEPS;
   else
      bpPages->ucLeftPedalSmoothness = (UCHAR) (numericUpDown_LeftPedalSmoothness->Value * 2);
}

System::Void BikePowerSensor::checkBox_RightPedalSmoothnessInvalid_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
   numericUpDown_RightPedalSmoothness->Enabled = !checkBox_RightPedalSmoothnessInvalid->Checked;
   if(checkBox_RightPedalSmoothnessInvalid->Checked)
      bpPages->ucRightPedalSmoothness = bpPages->INVALID_TEPS;
   else
      bpPages->ucRightPedalSmoothness = (UCHAR) (numericUpDown_RightPedalSmoothness->Value * 2);
}

System::Void BikePowerSensor::checkBox_Combined_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
   // If this is a combined system, torque effectiveness doesn't apply
   checkBox_LeftTorqueEffectivenessInvalid->Checked = checkBox_Combined->Checked;
   checkBox_RightTorqueEffectivenessInvalid->Checked = checkBox_Combined->Checked;

   // Pedal Smoothness is a little trickier...
   numericUpDown_RightPedalSmoothness->Enabled = !checkBox_Combined->Checked;
   checkBox_RightPedalSmoothnessInvalid->Enabled = !checkBox_Combined->Checked;
   if(checkBox_Combined->Checked)
      bpPages->ucRightPedalSmoothness = bpPages->COMBINED_PEDAL_SMOOTHNESS;
   else
      checkBox_RightPedalSmoothnessInvalid_CheckedChanged(this, nullptr);  // trigger an update to check for invalid values and read numeric box
}

System::Void BikePowerSensor::checkBox_EnableTEPS_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
   bTxPage19 = checkBox_EnableTEPS->Checked;
}

System::Void BikePowerSensor::numericUpDown_LeftTorqueEffectiveness_ValueChanged(System::Object^  sender, System::EventArgs^  e) 
{
   if(!checkBox_LeftTorqueEffectivenessInvalid->Checked)    
      bpPages->ucLeftTorqueEffectiveness = (UCHAR) (numericUpDown_LeftTorqueEffectiveness->Value * 2);
}

System::Void BikePowerSensor::numericUpDown_RightTorqueEffectiveness_ValueChanged(System::Object^  sender, System::EventArgs^  e) 
{
   if(!checkBox_RightTorqueEffectivenessInvalid->Checked)    
      bpPages->ucRightTorqueEffectiveness = (UCHAR) (numericUpDown_RightTorqueEffectiveness->Value * 2);         
}

System::Void BikePowerSensor::numericUpDown_LeftPedalSmoothness_ValueChanged(System::Object^  sender, System::EventArgs^  e) 
{
   if(!checkBox_LeftPedalSmoothnessInvalid->Checked)
      bpPages->ucLeftPedalSmoothness = (UCHAR) (numericUpDown_LeftPedalSmoothness->Value *2);      
}

System::Void BikePowerSensor::numericUpDown_RightPedalSmoothness_ValueChanged(System::Object^  sender, System::EventArgs^  e) 
{
   if(!checkBox_RightPedalSmoothnessInvalid->Checked)
      bpPages->ucRightPedalSmoothness = (UCHAR) (numericUpDown_RightPedalSmoothness->Value * 2);
}