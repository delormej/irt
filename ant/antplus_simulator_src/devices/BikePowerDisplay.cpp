/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/



#include "StdAfx.h"
#include "BikePowerDisplay.h"

/**************************************************************************
 * BikePowerDisplay::ANT_eventNotification
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
void BikePowerDisplay::ANT_eventNotification(UCHAR ucEventCode_, UCHAR* pucEventBuffer_)
{	
	switch(ucEventCode_)
	{
		case EVENT_RX_BURST_PACKET:
		case EVENT_RX_BROADCAST:
		case EVENT_RX_ACKNOWLEDGED:	// Intentional fall thru
			HandleReceive((UCHAR*) pucEventBuffer_);	// Decode current data
			break;
		case EVENT_TRANSFER_TX_COMPLETED:
			ucAckRetryCount = 0;			// Reset retransmission counter
			ucMsgExpectingAck = 0;			// Clear pending msg code
			UpdateDisplayAckStatus(ACK_SUCCESS);	// Tx successful
			break;
		case EVENT_TRANSFER_TX_FAILED:	
		case EVENT_ACK_TIMEOUT:		// Intentional fall thru
			if(ucMsgExpectingAck)
			{
				if(HandleRetransmit())
					UpdateDisplayAckStatus(ACK_RETRY);	// Data was retransmitted
				else
					UpdateDisplayAckStatus(ACK_FAIL);	// Maximum number of retries exceeded, Tx failed
			}
			break;
		default:
			break;
	}
}


/**************************************************************************
 * BikePowerDisplay::InitializeSim
 * 
 * Initializes simulator variables
 * 
 * returns: N/A
 *
 **************************************************************************/
void BikePowerDisplay::InitializeSim()
{
	bSeenPower = FALSE;					
	bSeenWheelTorque = FALSE;
	bSeenCrankTorque = FALSE;
	bSeenCTF = FALSE;
	ePrevType = BikePower::SensorType::UNKNOWN;

	ulAcumPowerEventCount = 0;

	ulAcumTorqueEventCount = 0;	
	ulAcumTicks = 0;
	ulDistance100 = 0;
	ucWheelCircumference100 = System::Convert::ToByte(this->numericUpDown_Cal_WheelCircum->Value);	// Initial value set on UI (cm)

	usCTFOffset = 0;	// Initial uncalibrated offset
	bAutoZeroInvalid = FALSE;
	bAZOn = (BOOL) this->radioButton_Cal_AZon->Checked;		// Initial value set on UI
	usCalSerialToSend = System::Convert::ToUInt16(this->numericUpDown_Cal_CTFSerial->Value);			
	usCalSlopeToSend = System::Convert::ToUInt16(10 * this->numericUpDown_Cal_CTFSlope->Value);
	ucMsgExpectingAck = 0;
	bCoasting = FALSE;
	bStopped = FALSE;
}


/**************************************************************************
 * BikePowerDisplay::HandleReceive
 * 
 * Decodes received data
 *
 * pucRxBuffer_: pointer to the buffer containing the received data
 * 
 * returns: N/A
 *
 **************************************************************************/
void BikePowerDisplay::HandleReceive(UCHAR* pucRxBuffer_)
{
	static USHORT usNoEventCount = 0;			// Counter for successive transmissions with no new events
	// Power sensors
	static UCHAR ucPrevPowerEventCount = 0;		// Previous value for the  power event count 
	static USHORT usPrevAcumPower = 0;			// Previous value for the cumulative power (W)
	// Crank/wheel torque sensors
	static UCHAR ucPrevTorqueEventCount = 0;	// Previous value for the torque event count
	static UCHAR ucPrevTicks = 0;				// Previous number of revolutions (wheel or torque)
	static USHORT usPrevPeriod2048 = 0;			// Previous period (wheel or torque), in 1/2048s
	static USHORT usPrevTorque32 = 0;			// Previous torque (1/32 Nm)
	static UCHAR ucCurTorqueEventCount = 0;		// Current torque event count
	static UCHAR ucCurTicks = 0;				// Current number of ticks
	static USHORT usCurPeriod2048 = 0;			// Current period
	static USHORT usCurTorque32 = 0;			// Current torque 
	// Crank torque frequency
	static UCHAR ucPrevCTFEventCount = 0;		// Previous value for the crank torque frequency event count
	static USHORT usPrevTime2000 = 0;			// Previous CTF time stamp (1/2000 s)
	static USHORT usPrevCTFTicks = 0;			// Previous torque tick count

	UCHAR ucPageNum = pucRxBuffer_[0];			// Get page number

	// Decode common pages if available
	if(ucPageNum >= CommonData::PAGE80 && ucPageNum <= CommonData::PAGE82)
	{
		try
		{
			commonPages->Decode(pucRxBuffer_);
		}	
		catch(CommonData::Error^ errorCommon)
		{
		}	
	}


	// Decode bike power pages if available
	if(ucPageNum <= BikePower::PAGE_CTF)
	{
		try
		{
			bpPages->Decode(pucRxBuffer_);
			bAutoZeroInvalid = FALSE;
			bBadCalPage = FALSE;
		}
		catch(BikePower::Error^ errorBikePower)
		{
			if(errorBikePower->bUndefAutoZero)
				bAutoZeroInvalid = TRUE;
			if(ucPageNum == BikePower::PAGE_CALIBRATION)
				bBadCalPage = TRUE;
		}
	}

	// If sensor type changed, reset calculated values
	if(ePrevType != bpPages->eType)	
	{
		ChangeSensorType();		// Update sensor type on interface
		bCoasting = FALSE;		// Reset state variables
		bStopped = FALSE;
		bSeenCTF = FALSE;
		bSeenPower = FALSE;
		bSeenWheelTorque = FALSE;
		bSeenCrankTorque = FALSE;
		bSeenCTF = FALSE;
				
		if(bpPages->eType == BikePower::SensorType::CRANK_TORQUE_FREQ)	// For CTF sensor
			SendMessage(BikePower::CAL_REQUEST);		// Request offset to be able to do calculations
	}

	// Process bike power pages
	switch(ucPageNum)
	{
		case BikePower::PAGE_GET_SET_PARAMETERS:
			if(bpPages->ucCrankLength == 0xFF)
			{
				label_CrankLength->Text = "Invalid";
				checkBox_InvalidCrankLength->Checked = true;
				checkBox_AutoCrank->Checked = false;
			}
			else if(bpPages->ucCrankLength == 0xFE)
			{
				label_CrankLength->Text = "Auto Crank";
				checkBox_InvalidCrankLength->Checked = false;
				checkBox_AutoCrank->Checked = true;
			}
			else
			{
				label_CrankLength->Text = (((float)bpPages->ucCrankLength * (float)0.5) + (110.0)).ToString("F1"); 
				checkBox_InvalidCrankLength->Checked = false;
				checkBox_AutoCrank->Checked = false;
				numericUpDown_CrankLength->Value = (Decimal)(((float)bpPages->ucCrankLength * (float)0.5) + (110.0));
			}
			label_AutoCrankLength->Text = ((bpPages->ucSensorCapabilities & 0x01) == 0) ? "Manually Set Crank Length" : "Automatically Set Crank Length"; 

			switch((bpPages->ucSensorStatus & 0x30) >> 4)
			{
			case BikePower::SensorStatus::UNDEFINED:
				label_SensorStatus->Text = "Undefined";
				break;
			case BikePower::SensorStatus::LEFT_PRESENT:
				label_SensorStatus->Text = "Left Present";
				break;
			case BikePower::SensorStatus::RIGHT_PRESENT:
				label_SensorStatus->Text = "Right Present";
				break;
			case BikePower::SensorStatus::LEFT_RIGHT_PRESENT:
				label_SensorStatus->Text = "Left + Right Present";
				break;
			default:
				label_SensorStatus->Text = "----";
				break;
			}

			switch((bpPages->ucSensorStatus & 0x0C) >> 2)
			{
			case BikePower::SWMistmatchStatus::UNDEFINED:
				label_SWMistmatchStatus->Text = "Undefined";
				break;
			case BikePower::SWMistmatchStatus::RIGHT_SENSOR_OLDER:
				label_SWMistmatchStatus->Text = "Right Sensor Older";
				break;
			case BikePower::SWMistmatchStatus::LEFT_SENSOR_OLDER:
				label_SWMistmatchStatus->Text = "Left Sensor Older";
				break;
			case BikePower::SWMistmatchStatus::SW_SAME:
				label_SWMistmatchStatus->Text = "No mismatch";
				break;
			default:
				label_SWMistmatchStatus->Text = "----";
				break;
			}
			
			switch(bpPages->ucSensorStatus & 0x03)
			{
			case BikePower::CrankLengthStatus::LENGTH_INVALID:
				label_CrankLengthStatus->Text = "Invalid";
				break;
			case BikePower::CrankLengthStatus::DEFAULT_LENGTH_USED:
				label_CrankLengthStatus->Text = "Default Length Used";
				break;
			case BikePower::CrankLengthStatus::LENGTH_MANUALLY_SET:
				label_CrankLengthStatus->Text = "Manually Set";
				break;
			case BikePower::CrankLengthStatus::LENGTH_AUTOMATICALLY_SET:
				label_CrankLengthStatus->Text = "Automatically Set";
				break;
			default:
				label_CrankLengthStatus->Text = "----";
				break;
			}

			break;
		case BikePower::PAGE_POWER: 
		{
			if(!bSeenPower)  // Initialize previous values if first time the page is seen
			{
				ucPrevPowerEventCount = bpPages->ucPowEventCount;
				usPrevAcumPower = bpPages->usAcumPower;
				ulAcumPowerEventCount = 0;
				bSeenPower = TRUE;
			}
			
			// Difference between current and previous event
			UCHAR ucEventDiff = GetDifference(bpPages->ucPowEventCount, ucPrevPowerEventCount);  
			USHORT usPowerDiff = GetDifference(bpPages->usAcumPower, usPrevAcumPower);
			// Update last event
			ucPrevPowerEventCount = bpPages->ucPowEventCount;	
			usPrevAcumPower = bpPages->usAcumPower;
			// Cumulative values
			ulAcumPowerEventCount += ucEventDiff;	// Cumulative power event count
			
			// Update calculation if dealing with a new event
			if(ucEventDiff && bpPages->eType == BikePower::SensorType::POWER_ONLY)	
				ulAveragePower256 = (((ULONG) usPowerDiff) << 8)/ucEventDiff;  // Average power (1/256W)

			break;
		}
		case BikePower::PAGE_WHEEL_TORQUE: 
			ucCurTorqueEventCount = bpPages->ucWTEventCount;
			ucCurTicks = bpPages->ucWheelTicks;
			usCurPeriod2048 = bpPages->usAcumWheelPeriod2048;
			usCurTorque32 = bpPages->usAcumTorque32;			
		case BikePower::PAGE_CRANK_TORQUE:	// intentional fall-thru (most of the calculations for these two sensors are the same)
		{
			if(bpPages->eType == BikePower::SensorType::TORQUE_CRANK)
			{
				ucCurTorqueEventCount = bpPages->ucCTEventCount;
				ucCurTicks = bpPages->ucCrankTicks;
				usCurPeriod2048 = bpPages->usAcumCrankPeriod2048;
				usCurTorque32 = bpPages->usAcumTorque32;				
			}

			// Initialize previous values if first time the wheel or crank torque page is seen
			if((!bSeenCrankTorque && bpPages->eType == BikePower::SensorType::TORQUE_CRANK) || (!bSeenWheelTorque && bpPages->eType == BikePower::SensorType::TORQUE_WHEEL))  
			{	
				if(!bSeenCrankTorque && bpPages->eType == BikePower::SensorType::TORQUE_WHEEL)
					bSeenWheelTorque = TRUE;
				if(!bSeenWheelTorque && bpPages->eType == BikePower::SensorType::TORQUE_CRANK)
					bSeenCrankTorque = TRUE;
				ulAcumTorqueEventCount = 0;	// Reset accumulated values
				ulAcumTicks = 0;
				usNoEventCount = 0;
				ulDistance100 = 0;
				ucPrevTorqueEventCount = ucCurTorqueEventCount;
				ucPrevTicks = ucCurTicks;
				usPrevPeriod2048 = usCurPeriod2048;
				usPrevTorque32 = usCurTorque32;
			}

			// Difference between current and previous event
			UCHAR ucTorqueEventDiff = GetDifference(ucCurTorqueEventCount, ucPrevTorqueEventCount);
			UCHAR ucTicksDiff = GetDifference(ucCurTicks, ucPrevTicks);
			USHORT usPeriodDiff = GetDifference(usCurPeriod2048, usPrevPeriod2048);
			USHORT usTorqueDiff = GetDifference(usCurTorque32, usPrevTorque32);  
			// Update last event
			ucPrevTorqueEventCount =ucCurTorqueEventCount;  
			ucPrevTicks = ucCurTicks;
			usPrevPeriod2048 = usCurPeriod2048;	
			usPrevTorque32 = usCurTorque32;
			// Update cumulative values
			ulAcumTorqueEventCount += ucTorqueEventDiff;	// Cumulative torque event count
			ulAcumTicks += ucTicksDiff;						// Cumulative revolutions

			// Update calculations if dealing with a new event
			if(ucTorqueEventDiff)
			{
				usNoEventCount = 0;
				if(usPeriodDiff)
				{
					if(bpPages->eType == BikePower::SensorType::TORQUE_WHEEL)
					{
						bStopped = FALSE;
						ulSpeed = ((ULONG) 3600 * 2048 * ucWheelCircumference100 * ucTorqueEventDiff)/((ULONG) 100 * usPeriodDiff);	// Average speed in m/h (Circumference in cm, Period in 1/2048s)
					}
					else // Crank Torque 
					{
						bCoasting = FALSE;
						ulAverageCadence = ((ULONG) 60 * 2048 * ucTorqueEventDiff)/(ULONG) usPeriodDiff;	// Average cadence (rpm)
					}
					ulAngularVelocity256 = ((ULONG) 2 * 2048 * PI256 * ucTorqueEventDiff)/(ULONG) usPeriodDiff;	// Angular velocity (1/256 rad/s) -> 2 * 2048 * 256 * pi
				}
				else
				{
					if(bpPages->eType == BikePower::SensorType::TORQUE_WHEEL)
					{
						bStopped = TRUE;	// No changes in wheel period if bike has stopped (time synchronous update)
						ulSpeed = 0;		// If the bike has stopped, its speed and wheel angular velocity are zero
					}
					else  // Crank Torque
					{
						bCoasting = TRUE;	// No changes in crank period if the bike is coasting (time synchronous update)
						ulAverageCadence = 0;		// If the bike is coasting, its cadence and crank angular velocity are zero
					}
					ulAngularVelocity256 = 0;
				}
				ulAverageTorque32 = (ULONG) usTorqueDiff/(ULONG) ucTorqueEventDiff;		// Average torque (1/32 Nm)
				ulAveragePower256 = (ulAverageTorque32 * ulAngularVelocity256)/32;		// Average power => (1/32Nm) * (1/256 rad/s) * 1/32 = 1/256W
				if(bpPages->eType == BikePower::SensorType::TORQUE_WHEEL)	
				{
					ulDistance100 += (ULONG) ucWheelCircumference100 * ucTicksDiff;	// Distance (cm)
					if(usTorqueDiff)   // Check for coasting
						bCoasting = FALSE;	
					else
						bCoasting = TRUE;	// The bike is coasting if there are wheel events but the accumulated torque does not change 
				}
			}
			else
			{	// Event synchronous 
				usNoEventCount++;
				if(usNoEventCount >= BikePower::MAX_NOEVENT)	
				{
					if(bpPages->eType == BikePower::SensorType::TORQUE_WHEEL)
					{				
						bStopped = TRUE;	// Detect stop (no new wheel events before a preset number of messages)
						ulSpeed = 0;		// Bike stopped, so speed and wheel angular velocity is zero
					}
					else	// Crank Torque
					{
						bCoasting = TRUE;	// Detect coasting (no new crank events before a preset number of messages)
						ulAverageCadence = 0;	//  If the bike is coasting, cadence and crank angular velocity are zero
						
					}
					ulAngularVelocity256 = 0;
				}
			}

			break;
		}      
		case BikePower::PAGE_CTF:
		{
			if(!bSeenCTF)	// Initialize previous values if first time the page has been seen
			{
				bSeenCTF = TRUE;
				ulAcumCTFEventCount = 0;
				usNoEventCount = 0;
				ucPrevCTFEventCount = bpPages->ucCTFEventCount;
				usPrevTime2000 = bpPages->usTime2000;
				usPrevCTFTicks = bpPages->usTorqueTicks;
			}

			// Difference between current and previous event
			UCHAR ucCTFEventDiff = GetDifference(bpPages->ucCTFEventCount, ucPrevCTFEventCount);
			USHORT usTimeDiff = GetDifference(bpPages->usTime2000, usPrevTime2000);
			USHORT usCTFTicksDiff = GetDifference(bpPages->usTorqueTicks, usPrevCTFTicks);
			// Update last event
			ucPrevCTFEventCount = bpPages->ucCTFEventCount;
			usPrevTime2000 = bpPages->usTime2000;
			usPrevCTFTicks = bpPages->usTorqueTicks;
			// Update cumulative values
			ulAcumCTFEventCount += ucCTFEventDiff;
			
			// Update calculations if dealing with a new event
			if(ucCTFEventDiff)
			{
				usNoEventCount = 0;
				bCoasting = FALSE;
				if(usTimeDiff)
				{
					ulAverageCadence = ((ULONG) 60 * 2000 * ucCTFEventDiff)/(ULONG) usTimeDiff;		// Cadence (rpm)
					if(!bpPages->IsSlopeInvalid(bpPages->usSlope10))
					{
						ulAverageTorque32 = ((((ULONG) 32 * 10 * 2000 * usCTFTicksDiff)/usTimeDiff) - (32 * 10 * usCTFOffset))/(ULONG)bpPages->usSlope10;	// Torque (1/32 Nm):  time is in 1/2000s, slope in 1/10 Hz/Nm, convert to 1/32 Nm for consistency
						ulAveragePower256 = (ULONG) (PI256 * ulAverageTorque32 * ulAverageCadence)/(30*32);		// Average power (1/256 W) , torque is in 1/32 m
						if(ulAverageTorque32)
							ulAngularVelocity256 = ((ULONG) 32 * ulAveragePower256)/ulAverageTorque32;			// Power (1/256W) / Torque (1/32 Nm) -> 1/256 rad/s
					}
				}
			}
			else
			{
				usNoEventCount++;
				if(usNoEventCount >= BikePower::MAX_NOEVENT)	
				{
					bCoasting = TRUE;	// Detect coasting
					ulAverageCadence = 0;
					ulAveragePower256 = 0;
				}
			}
			break;
		}
		case BikePower::PAGE_CALIBRATION: 
			// Set calibration offset to use on calculations if it received a valid value
			if(bpPages->ucRxCalibrationID == BikePower::CAL_CTF && bpPages->ucRxCTFMsgID == BikePower::CTF_OFFSET)
			{
				if(!bpPages->IsOffsetInvalid(bpPages->usCalOffset))
					usCTFOffset = bpPages->usCalOffset;		
			}
			break;
		default:
			break;
	}

	UpdateDisplay(ucPageNum);
	ePrevType = bpPages->eType;					// Previous detected sensor type
}


/**************************************************************************
 * BikePowerDisplay::HandleRetransmit
 * 
 * Retransmits calibration message, up to the maximum retransmission number
 * If values are updated on the GUI while attempting to retransmit, the
 * newest values will be sent
 * 
 * returns: TRUE if message was retransmitted
 *          FALSE if maximum number of retransmissions was reached
 *
 **************************************************************************/
BOOL BikePowerDisplay::HandleRetransmit()
{
	BOOL bSuccess = TRUE;

	if(ucMsgExpectingAck)	// Message still expecting an ack
	{
		if(ucAckRetryCount++ < MAX_RETRIES)
		{
			SendMessage(ucMsgExpectingAck);
		}
		else
			bSuccess = FALSE;
	}
	
	return bSuccess;
}
/**************************************************************************
 * BikePowerDisplay::UpdateDisplay
 * 
 * Shows received decoded data on GUI
 *
 * ucPageNum_: received page
 *
 * returns:  N/A
 *
 **************************************************************************/
void BikePowerDisplay::UpdateDisplay(UCHAR ucPageNum_)
{
	// Display current message received
	switch(ucPageNum_)
	{
		case BikePower::PAGE_POWER:
			this->label_Trn_PageDisplay->Text = "BAS";
			this->label_Trn_UpdateCountDisplay->Text = bpPages->ucPowEventCount.ToString();			// Current power event count
			if(bpPages->IsCadenceInvalid(bpPages->ucCadence))
				this->label_Trn_CadenceDisplay->Text = "Off";
			else
				this->label_Trn_CadenceDisplay->Text = bpPages->ucCadence.ToString();				// Cadence (RPM)
			
			if(bpPages->ucPedalPower != BikePower::RESERVED)
			{
				UCHAR ucTemp = 0x00;
				ucTemp = ((bpPages->ucPedalPower) & 0x7F);	//0x7F is a mask used to remove "Pedal Differentiation Bit" leaving pedal power percent value only

				if(ucTemp >= 0x65)	// Checking pedal power percent value is within the valid range 0%-100%
				{
					this->label_Trn_PedalDisplay->Text = "---";
					this->label_Trn_PedalPwrDisplay->Text = "Invalid";
				}
				else
				{
					if((bpPages->ucPedalPower) & 0x80)	// 0x80 is a mask for right pedal power contribution
						this->label_Trn_PedalDisplay->Text = "Right";	// Power contribution is from the right pedal
					else
						this->label_Trn_PedalDisplay->Text = "Unknown";	// Power contribution is unknown

					this->label_Trn_PedalPwrDisplay->Text = ucTemp.ToString();
				}
			}
			else
			{
				this->label_Trn_PedalDisplay->Text = "---";
				this->label_Trn_PedalPwrDisplay->Text = "Off";
			}
			
			this->label_Trn_AccumPowerDisplay->Text = bpPages->usAcumPower.ToString();				// Current cumulative power (W)
			this->label_Trn_InstPowerDisplay->Text = bpPages->usPower.ToString();					// Power (W)
			this->label_Dat_InstCadenceDisplay->Text = this->label_Trn_CadenceDisplay->Text;		// Cadence (RPM)
			this->label_Dat_InstPowerDisplay->Text = bpPages->usPower.ToString();					// Power (W)
			this->label_Dat_PedalPwrDisplay->Text = this->label_Trn_PedalPwrDisplay->Text;
			this->label_Dat_PedalDisplay->Text = this->label_Trn_PedalDisplay->Text;
			break;
		case BikePower::PAGE_WHEEL_TORQUE:
			this->label_Trn_PageDisplay->Text = "WHL";
			this->label_Trn_UpdateCountDisplay->Text = bpPages->ucWTEventCount.ToString();
			this->label_Trn_EventCountDisplay->Text = bpPages->ucWheelTicks.ToString();
			if(bpPages->IsCadenceInvalid(bpPages->ucCadence))
				this->label_Trn_CadenceDisplay->Text = "Off";
			else
				this->label_Trn_CadenceDisplay->Text = bpPages->ucCadence.ToString();				// Cadence (RPM)
			this->label_Dat_InstCadenceDisplay->Text = this->label_Trn_CadenceDisplay->Text;		// Cadence (RPM)
			this->label_Trn_AccumOneDisplay->Text = bpPages->usAcumWheelPeriod2048.ToString();		// Period (1/2048 s)
			this->label_Trn_AccumTwoDisplay->Text = bpPages->usAcumTorque32.ToString();				// Torque (1/32 Nm)
			break;
		case BikePower::PAGE_CRANK_TORQUE:
			this->label_Trn_PageDisplay->Text = "CRK";
			this->label_Trn_UpdateCountDisplay->Text = bpPages->ucCTEventCount.ToString();
			this->label_Trn_EventCountDisplay->Text = bpPages->ucCrankTicks.ToString();
			if(bpPages->IsCadenceInvalid(bpPages->ucCadence))
				this->label_Trn_CadenceDisplay->Text = "Off";
			else
				this->label_Trn_CadenceDisplay->Text = bpPages->ucCadence.ToString();				// Cadence (RPM)
			this->label_Dat_InstCadenceDisplay->Text = this->label_Trn_CadenceDisplay->Text;		// Cadence (RPM)
			this->label_Trn_AccumOneDisplay->Text = bpPages->usAcumCrankPeriod2048.ToString();
			this->label_Trn_AccumTwoDisplay->Text = (bpPages->usAcumTorque32).ToString();			// Accum torque (1/32 Nm)
			break;
      case BikePower::PAGE_TEPS:
      {
         if(bpPages->ucLeftTorqueEffectiveness == bpPages->INVALID_TEPS)
            label_LeftTorqueEffectiveness->Text = "INVALID";
         else
            label_LeftTorqueEffectiveness->Text = (bpPages->ucLeftTorqueEffectiveness / 2.0).ToString();
         if(bpPages->ucRightTorqueEffectiveness == bpPages->INVALID_TEPS)
            label_RightTorqueEffectiveness->Text = "INVALID";
         else
            label_RightTorqueEffectiveness->Text = (bpPages->ucRightTorqueEffectiveness / 2.0).ToString();
         if(bpPages->ucLeftPedalSmoothness == bpPages->INVALID_TEPS)
            label_LeftPedalSmoothness->Text = "INVALID";
         else
            label_LeftPedalSmoothness->Text = (bpPages->ucLeftPedalSmoothness / 2.0).ToString();
         if(bpPages->ucRightPedalSmoothness == bpPages->INVALID_TEPS)
            label_RightPedalSmoothness->Text = "INVALID";
         else if(bpPages->ucRightPedalSmoothness == bpPages->COMBINED_PEDAL_SMOOTHNESS)
            label_RightPedalSmoothness->Text = "COMBINED PEDAL SMOOTHNESS";
         else
            label_RightPedalSmoothness->Text = (bpPages->ucRightPedalSmoothness / 2.0).ToString();
         break;
      }
		case BikePower::PAGE_CTF:
			this->label_Trn_PageDisplay->Text = "CTF";
			this->label_Trn_UpdateCountDisplay->Text = bpPages->ucCTFEventCount.ToString();
			this->label_Trn_SlopeDisplay->Text = bpPages->usSlope10.ToString();
			this->label_Trn_AccumOneDisplay->Text = bpPages->usTime2000.ToString();
			this->label_Trn_AccumTwoDisplay->Text = bpPages->usTorqueTicks.ToString();
			if(bpPages->IsSlopeInvalid(bpPages->usSlope10))
				this->label_Dat_SlopeDisplay->Text = "---";
			else
				this->label_Dat_SlopeDisplay->Text = ((double)bpPages->usSlope10/10).ToString();
			break;
		case BikePower::PAGE_CALIBRATION:
			this->label_Cal_StatusDisplay->ForeColor = System::Drawing::Color::Green;
			if(bBadCalPage)
			{
				this->label_Cal_StatusDisplay->ForeColor = System::Drawing::Color::Red;
				this->label_Cal_StatusDisplay->Text = "xPAG";
			}

			switch(bpPages->ucRxCalibrationID)
			{
				case BikePower::CAL_SUCCESS:
				case BikePower::CAL_FAIL:	// Intentional fall thru
					if(!bAutoZeroInvalid)
					{
						this->button_Cal_AutoZeroSet->Enabled = (bpPages->bAutoZeroEnable == TRUE);
						this->radioButton_Cal_AZoff->Enabled = (bpPages->bAutoZeroEnable == TRUE);
						this->radioButton_Cal_AZon->Enabled = (bpPages->bAutoZeroEnable == TRUE);
						if(!bpPages->bAutoZeroEnable)
							this->listBox_Cal_AZStatus->SelectedIndex = 2;
						else 
							this->listBox_Cal_AZStatus->SelectedIndex = (UCHAR) bpPages->bAutoZeroOn;
					}
					this->label_Cal_CalNumberDisplay->Text = bpPages->sCalibrationData.ToString();
					if(bpPages->ucRxCalibrationID == BikePower::CAL_SUCCESS)
					{
						this->label_Cal_StatusDisplay->Text = "+CAL";
						this->label_Cal_StatusDisplay->ForeColor = System::Drawing::Color::Green;
					}
					else
					{
						this->label_Cal_StatusDisplay->Text = "xCAL";
						this->label_Cal_StatusDisplay->ForeColor = System::Drawing::Color::Red;
					}
					break;
				case BikePower::CAL_TORQUE_METER_CAPABILITIES:
					this->button_Cal_AutoZeroSet->Enabled = (bpPages->bAutoZeroEnable == TRUE);
					this->radioButton_Cal_AZoff->Enabled = (bpPages->bAutoZeroEnable == TRUE);
					this->radioButton_Cal_AZon->Enabled = (bpPages->bAutoZeroEnable == TRUE);
					if(!bpPages->bAutoZeroEnable)	// Auto zero not supported
						this->listBox_Cal_AZStatus->SelectedIndex = 2;
					else
						this->listBox_Cal_AZStatus->SelectedIndex = (UCHAR) bpPages->bAutoZeroOn;
					if(bpPages->IsRawTorqueInvalid(bpPages->sRawTorque))
						this->label_Cal_RawTorqueDisplay->Text = "---";
					else
						this->label_Cal_RawTorqueDisplay->Text = bpPages->sRawTorque.ToString();

					if(bpPages->IsOffsetTorqueInvalid(bpPages->sOffsetTorque))
						this->label_Cal_OffsetTorqueDisplay->Text = "---";
					else
						this->label_Cal_OffsetTorqueDisplay->Text = bpPages->sOffsetTorque.ToString();
					break;
				case BikePower::CAL_CTF:
					if(bpPages->ucRxCTFMsgID == BikePower::CTF_OFFSET)
					{
						this->label_Cal_CalNumberDisplay->Text = bpPages->usCalOffset.ToString();
						if(!bpPages->usCalOffset)
						{
							this->label_Cal_StatusDisplay->ForeColor = System::Drawing::Color::Yellow;
							this->label_Cal_StatusDisplay->Text = "ZERO";									
						}
						else if(bpPages->IsOffsetInvalid(bpPages->usCalOffset))
						{
							this->label_Cal_StatusDisplay->ForeColor = System::Drawing::Color::Red;
							this->label_Cal_StatusDisplay->Text = "xOFS";
						}
						else
						{
							this->label_Dat_CTFOffsetDisplay->Text = usCTFOffset.ToString();
							this->label_Cal_StatusDisplay->Text = "+OFS";
						}
					}
					if(bpPages->ucRxCTFMsgID == BikePower::CTF_ACK)
					{
						if(bpPages->ucRxCTFMsgID == BikePower::CTF_SLOPE)
							this->label_Cal_StatusDisplay->Text = "+SLP";
						if(bpPages->ucRxCTFMsgID == BikePower::CTF_SERIAL)
							this->label_Cal_StatusDisplay->Text = "+SRL";
					}
					break;
				default:
					break;
			}
			break;
		case CommonData::PAGE80:
			this->label_Glb_HardwareVerDisplay->Text = commonPages->ucHwVersion.ToString();	// Hw version
			this->label_Glb_ManfIDDisplay->Text = commonPages->usMfgID.ToString();			// Mfg ID
			this->label_Glb_ModelNumDisplay->Text = commonPages->usModelNum.ToString();		// Model number
			break;
		case CommonData::PAGE81:
			this->label_Glb_SoftwareVerDisplay->Text = commonPages->ucSwVersion.ToString();	// Sw version
			if(commonPages->ulSerialNum != 0xFFFFFFFF)
				this->label_Glb_SerialNumDisplay->Text = commonPages->ulSerialNum.ToString();	// Serial number
			else
				this->label_Glb_SerialNumDisplay->Text = "N/A";
			break;
		case CommonData::PAGE82:
			if(commonPages->IsBatteryVoltageInvalid(commonPages->usBatVoltage256))		// Battery voltage
				this->label_Bat_VoltsDisplay->Text = "Invalid Voltage";
			else
				this->label_Bat_VoltsDisplay->Text = System::Math::Round((double)commonPages->usBatVoltage256/256,4).ToString();
			if(commonPages->IsBatteryStatusInvalid(commonPages->eBatStatus))	// Battery status
				this->listBox_Bat_Status->SelectedIndex = this->listBox_Bat_Status->Items->Count - 1;
			else
				this->listBox_Bat_Status->SelectedIndex = (UCHAR) (commonPages->eBatStatus) - 1;
			this->label_Bat_ElpTimeDisplay->Text = ((ULONG) (commonPages->ulOpTime & CommonData::OPERATING_TIME_MASK) * (UCHAR) commonPages->eTimeResolution).ToString();	// Operating time (s)
			this->label_Bat_SecPrecis->Text = System::String::Concat(((UCHAR)commonPages->eTimeResolution).ToString(), " Sec Precision");	// Time resolution
			break;
		default:
			break;
	}


	// Calculations are not displayed initially, so they will only be displayed once the sensor type is known
	// A sensor type would change from UNKNOWN to the correct type after initialization
	if(ePrevType == bpPages->eType)	
	{
		switch(bpPages->eType)
		{
			case BikePower::SensorType::POWER_ONLY:
				this->label_Dat_Coasting->Visible = (bCoasting == TRUE);	
				this->label_Dat_Stopped->Visible = (bStopped == TRUE);
				this->label_Dat_CalcPowerDisplay->Text = System::Math::Round((double)ulAveragePower256/256,2).ToString();	// Average power (W)
				this->label_Dat_UpdateCountDisplay->Text = ulAcumPowerEventCount.ToString();		// Cumulative power event count	
				break;
			case BikePower::SensorType::TORQUE_WHEEL:
			case BikePower::SensorType::TORQUE_CRANK: // intentional fall - thru
				this->label_Dat_Coasting->Visible = (bCoasting == TRUE);
				this->label_Dat_Stopped->Visible = (bStopped == TRUE);
				this->label_Dat_CalcPowerDisplay->Text = System::Math::Round((double) ulAveragePower256/256,2).ToString();	// Average power (W)
				this->label_Dat_UpdateCountDisplay->Text = ulAcumTorqueEventCount.ToString();	// Torque event count
				this->label_Dat_EventCountDisplay->Text = ulAcumTicks.ToString();	// Ticks
				this->label_Dat_AngVelDisplay->Text = System::Math::Round((double) ulAngularVelocity256/256,2).ToString();	// Angular velocity (rad/s)
				this->label_Dat_TorqueDisplay->Text = System::Math::Round((double) ulAverageTorque32/32,2).ToString();	// Average torque (Nm)
				if(bpPages->eType == BikePower::SensorType::TORQUE_WHEEL)
				{
					this->label_Dat_SpeedDisplay->Text = System::Math::Round((double)ulSpeed/1000,2).ToString();	// Average speed (meters/h -> km/h)
					this->label_Dat_DistDisplay->Text = System::Math::Round((double)ulDistance100/100,2).ToString();	// Distance (m)	
				}
				else // Crank Torque
				{
					this->label_Dat_CalcRPMDisplay->Text = System::Math::Round(ulAverageCadence,2).ToString();
				}
				break;
			case BikePower::SensorType::CRANK_TORQUE_FREQ:
				if(ucPageNum_ == BikePower::PAGE_CALIBRATION && bpPages->ucRxCTFMsgID == BikePower::CTF_OFFSET)	
				{
					// Blank calculations while receiving CTF offset, rather than displaying erroneous data
					this->label_Dat_CalcPowerDisplay->Text = "---";
					this->label_Dat_UpdateCountDisplay->Text = "---";
					this->label_Dat_AngVelDisplay->Text = "---";
					this->label_Dat_TorqueDisplay->Text = "---";			
					this->label_Dat_CalcRPMDisplay->Text = "---";
				}
				else
				{
					this->label_Dat_Coasting->Visible = (bCoasting == TRUE);
					this->label_Dat_Stopped->Visible = (bStopped == TRUE);
					this->label_Dat_CalcPowerDisplay->Text = System::Math::Round((double)ulAveragePower256/256,2).ToString();
					this->label_Dat_UpdateCountDisplay->Text = ulAcumCTFEventCount.ToString();
					this->label_Dat_AngVelDisplay->Text = System::Math::Round((double)ulAngularVelocity256/256,2).ToString();
					this->label_Dat_TorqueDisplay->Text = System::Math::Round((double)ulAverageTorque32/32,2).ToString();			
					this->label_Dat_CalcRPMDisplay->Text = ulAverageCadence.ToString();
				}
				break;
			default:
				break;
		}
	}

}

/**************************************************************************
 * BikePowerDisplay::UpdateDisplayAckStatus
 * 
 * Updates display to show if acknowledged calibration messages were
 * transmitted successfully
 *
 * returns:  N/A
 *
 **************************************************************************/
void BikePowerDisplay::UpdateDisplayAckStatus(UCHAR ucStatus_)
{
	switch(ucStatus_)
	{
		case ACK_SUCCESS:
			this->label_Cal_StatusDisplay->ForeColor = System::Drawing::Color::Green;
			this->label_Cal_StatusDisplay->Text = "SENT";
			break;
		case ACK_RETRY:
			this->label_Cal_StatusDisplay->ForeColor = System::Drawing::Color::Blue;
			this->label_Cal_StatusDisplay->Text = "RTRY";
			break;
		case ACK_FAIL:
			this->label_Cal_StatusDisplay->ForeColor = System::Drawing::Color::Red;
			this->label_Cal_StatusDisplay->Text = "xSNT";
			break;
		default:
			break;
	}
}


/**************************************************************************
 * BikePowerDisplay::ChangeSensorType
 * 
 * Updates display to enable/disable elements available only on certain
 * type of sensors, after a change in sensor type is detected
 *
 * returns:  N/A
 *
 **************************************************************************/
void BikePowerDisplay::ChangeSensorType()
{
	switch(bpPages->eType)
		{
			case BikePower::SensorType::POWER_ONLY:
				this->label_Dat_PageRecDisplay->Text = "Power Only Sensor";
				this->label_Trn_EventCountDisplay->Text = "---";
				this->label_Trn_AccumOneDisplay->Text = "---";
				this->label_Trn_AccumTwoDisplay->Text = "---";
				this->label_Trn_Slope->Visible = false;
				this->label_Trn_SlopeDisplay->Visible = false;
				this->label_Dat_SlopeDisplay->Text = "---";
				this->label_Dat_AngVelDisplay->Text = "---";
				this->label_Dat_TorqueDisplay->Text = "---";
				this->label_Dat_CalcRPMDisplay->Text = "---";
				this->label_Dat_SpeedDisplay->Text = "---";
				this->label_Dat_DistDisplay->Text = "---";
				this->label_Dat_CTFOffsetDisplay->Text = "---";
				this->numericUpDown_Cal_CTFSlope->Enabled = false;
				this->numericUpDown_Cal_CTFSerial->Enabled = false;
				this->button_Cal_CTFSerialSet->Enabled = false;
				this->button_Cal_CTFSlopeSet->Enabled = false;
				break;
			case BikePower::SensorType::TORQUE_WHEEL:
				this->label_Dat_PageRecDisplay->Text = "Wheel Torque Sensor";
				this->label_Trn_AccumOne->Text = "Wheel P:";
				this->label_Trn_Slope->Visible = false;
				this->label_Trn_SlopeDisplay->Visible = false;
				this->label_Trn_InstPowerDisplay->Text = "---";
				this->label_Trn_AccumPowerDisplay->Text = "---";
				this->label_Dat_InstPowerDisplay->Text = "---";
				this->label_Dat_CTFOffsetDisplay->Text = "---";
				this->label_Dat_SlopeDisplay->Text = "---";
				this->numericUpDown_Cal_CTFSlope->Enabled = false;
				this->numericUpDown_Cal_CTFSerial->Enabled = false;
				this->button_Cal_CTFSerialSet->Enabled = false;
				this->button_Cal_CTFSlopeSet->Enabled = false;
				break;
			case BikePower::SensorType::TORQUE_CRANK:
				this->label_Dat_PageRecDisplay->Text = "Crank Torque Sensor";
				this->label_Trn_AccumOne->Text = "Crank P:";
				this->label_Trn_Slope->Visible = false;
				this->label_Trn_SlopeDisplay->Visible = false;
				this->label_Trn_InstPowerDisplay->Text = "---";
				this->label_Trn_AccumPowerDisplay->Text = "---";
				this->label_Dat_SlopeDisplay->Text = "---";
				this->label_Dat_SpeedDisplay->Text = "---";
				this->label_Dat_DistDisplay->Text = "---";
				this->label_Dat_InstPowerDisplay->Text = "---";
				this->label_Dat_CTFOffsetDisplay->Text = "---";
				this->numericUpDown_Cal_CTFSlope->Enabled = false;
				this->numericUpDown_Cal_CTFSerial->Enabled = false;
				this->button_Cal_CTFSerialSet->Enabled = false;
				this->button_Cal_CTFSlopeSet->Enabled = false;
				break;
			case BikePower::SensorType::CRANK_TORQUE_FREQ:
				this->label_Dat_PageRecDisplay->Text = "Crank Torque Frequency Sensor";
				this->label_Trn_AccumOne->Text = "   Time:";
				this->label_Trn_EventCountDisplay->Text = "---";
				this->label_Trn_Slope->Visible = true;
				this->label_Trn_SlopeDisplay->Visible = true;
				this->label_Trn_InstPowerDisplay->Text = "---";
				this->label_Trn_AccumPowerDisplay->Text = "---";
				this->label_Trn_CadenceDisplay->Text = "---";
				this->label_Trn_PedalDisplay->Text = "---";
				this->label_Trn_PedalPwrDisplay->Text = "---";
				this->label_Dat_SpeedDisplay->Text = "---";
				this->label_Dat_DistDisplay->Text = "---";				
				this->label_Dat_InstPowerDisplay->Text = "---";				
				this->label_Dat_InstCadenceDisplay->Text = "---";
				this->label_Dat_CTFOffsetDisplay->Text = "Unknown";
				this->numericUpDown_Cal_CTFSlope->Enabled = true;
				this->numericUpDown_Cal_CTFSerial->Enabled = true;
				this->button_Cal_CTFSerialSet->Enabled = true;
				this->button_Cal_CTFSlopeSet->Enabled = true;
				this->button_Cal_AutoZeroSet->Enabled = false;
				this->radioButton_Cal_AZoff->Enabled = false;
				this->radioButton_Cal_AZon->Enabled = false;
				this->listBox_Cal_AZStatus->SelectedIndex = -1;
				break;
			case BikePower::SensorType::UNKNOWN:
				this->label_Dat_PageRecDisplay->Text = "Updating...";
				this->label_Trn_EventCountDisplay->Text = "---";
				this->label_Trn_AccumOneDisplay->Text = "---";
				this->label_Trn_AccumTwoDisplay->Text = "---";
				this->label_Trn_Slope->Visible = false;
				this->label_Trn_SlopeDisplay->Visible = false;
				this->label_Dat_SlopeDisplay->Text = "---";
				this->label_Dat_AngVelDisplay->Text = "---";
				this->label_Dat_TorqueDisplay->Text = "---";
				this->label_Dat_CalcRPMDisplay->Text = "---";
				this->label_Dat_SpeedDisplay->Text = "---";
				this->label_Dat_DistDisplay->Text = "---";
				this->label_Dat_CTFOffsetDisplay->Text = "---";
				this->numericUpDown_Cal_CTFSlope->Enabled = false;
				this->numericUpDown_Cal_CTFSerial->Enabled = false;
				this->button_Cal_CTFSerialSet->Enabled = false;
				this->button_Cal_CTFSlopeSet->Enabled = false;
				break;
			default:
				break;
		}
}

/**************************************************************************
 * BikePowerDisplay::button_Cal_Calibrate_Click
 * 
 * Sends a calibration request
 *
 * returns:  N/A
 *
 **************************************************************************/
System::Void BikePowerDisplay::button_Cal_Calibrate_Click(System::Object^  sender, System::EventArgs^  e) 
{
	bRequestCalibration = true;

	SendMessage(BikePower::CAL_REQUEST);
}


/**************************************************************************
 * BikePowerDisplay::button_Cal_AutoZeroSet_Click
 * 
 * Sends an Auto Zero Configuration request
 *
 * returns:  N/A
 *
 **************************************************************************/
System::Void BikePowerDisplay::button_Cal_AutoZeroSet_Click(System::Object^  sender, System::EventArgs^  e) 
{
	bRequestCalibration = true;
	SendMessage(BikePower::CAL_AUTOZERO_CONFIG);
}


/**************************************************************************
 * BikePowerDisplay::button_Cal_CTFSerialSet_Click
 * 
 * Sends a CTF defined message for serial number, to save a new serial
 * number to the power sensor flash
 *
 * returns:  N/A
 *
 **************************************************************************/
System::Void BikePowerDisplay::button_Cal_CTFSerialSet_Click(System::Object^  sender, System::EventArgs^  e) 
{
	bRequestCalibration = true;
	SendMessage(BikePower::CTF_SERIAL);
}


/**************************************************************************
 * BikePowerDisplay::button_Cal_CTFSlopeSet_Click
 * 
 * Sends a CTF defined message for slope to the power sensor, to save a new
 * value for the slope to the  sensor flash
 *
 * returns:  N/A
 *
 **************************************************************************/
System::Void BikePowerDisplay::button_Cal_CTFSlopeSet_Click(System::Object^  sender, System::EventArgs^  e) 
{
	bRequestCalibration = true;
	SendMessage(BikePower::CTF_SLOPE);
}


/**************************************************************************
 * BikePowerDisplay::numericUpDown_Cal_WheelCircum_ValueChanged
 * 
 * Updates value of wheel circumference
 *
 * returns:  N/A
 *
 **************************************************************************/
System::Void BikePowerDisplay::numericUpDown_Cal_WheelCircum_ValueChanged(System::Object^  sender, System::EventArgs^  e) 
{
	ucWheelCircumference100 = System::Convert::ToByte(this->numericUpDown_Cal_WheelCircum->Value);
}

/**************************************************************************
 * BikePowerDisplay::radioButton_Cal_AZ_CheckedChanged
 * 
 * Updates auto zero status set on display
 *
 * returns:  N/A
 *
 **************************************************************************/
System::Void BikePowerDisplay::radioButton_Cal_AZ_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	bAZOn = (BOOL) this->radioButton_Cal_AZon->Checked;
}


/**************************************************************************
 * BikePowerDisplay::numericUpDown_Cal_CTFSerial_ValueChanged
 * 
 * Updates serial number set on display, for CTF Serial message
 *
 * returns:  N/A
 *
 **************************************************************************/
System::Void BikePowerDisplay::numericUpDown_Cal_CTFSerial_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	usCalSerialToSend = System::Convert::ToUInt16(this->numericUpDown_Cal_CTFSerial->Value);	 // serial number
}


/**************************************************************************
 * BikePowerDisplay::numericUpDown_Cal_CTFSlope_ValueChanged
 * 
 * Updates slope number set on display, for CTF Slope message
 *
 * returns:  N/A
 *
 **************************************************************************/
System::Void BikePowerDisplay::numericUpDown_Cal_CTFSlope_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	usCalSlopeToSend = System::Convert::ToUInt16(10*(this->numericUpDown_Cal_CTFSlope->Value));	 // slope in 1/10 Nm/Hz
}

System::Void BikePowerDisplay::button_SetCrankStatus_Click(System::Object ^sender, System::EventArgs ^e)
{
	bpPages->ucSubpageNumber = BikePower::SUBPAGE_CRANK_PARAMETERS;
	if(checkBox_InvalidCrankLength->Checked)
		bpPages->ucCrankLength = 0xFF; // Invalid
	else if(checkBox_AutoCrank->Checked)
		bpPages->ucCrankLength = 0xFE; // Request Auto Crank
	else
		bpPages->ucCrankLength = (UCHAR)(((float)numericUpDown_CrankLength->Value - (float)110.0) / (float)0.5); // Send Crank Length value
	
	bpPages->ucSensorStatus = 0x00; // Readonly byte
	bpPages->ucSensorCapabilities = 0x00; // Readonly byte

	bRequestCalibration = false;
	SendMessage(bpPages->PAGE_GET_SET_PARAMETERS);
}

System::Void BikePowerDisplay::button_GetCrankStatus_Click(System::Object ^sender, System::EventArgs ^e)
{
	commonPages->ucDescriptorByte1 = BikePower::SUBPAGE_CRANK_PARAMETERS; // for requesting subpages
	commonPages->ucDescriptorByte2 = 0xFF; // Invalid byte
	commonPages->ucReqTransResp = System::Convert::ToByte(numericUpDown_TransResponse->Value); //Number of times to transmit, don't use ack messages
	commonPages->ucReqPageNum = BikePower::PAGE_GET_SET_PARAMETERS; // request Get/Set page
	commonPages->ucCommandType = 0x01; // For requesting a data page

	bRequestCalibration = false;
	SendMessage(commonPages->PAGE70);
}
System::Void BikePowerDisplay::SendMessage(UCHAR ucMsgCode_)
{
	UCHAR aucAckBuffer[8] = {0,0,0,0,0,0,0,0};

	// If a calibration request should be sent
	if(bRequestCalibration)
	{
		switch(ucMsgCode_)
		{
			case BikePower::CAL_REQUEST:	// Manual calibration request
				bpPages->EncodeManualCalibrationRequest(aucAckBuffer);
				break;
			case BikePower::CAL_AUTOZERO_CONFIG:	// Auto zero calibration request
				bpPages->EncodeAZCalibrationRequest(TRUE, bAZOn, aucAckBuffer);
				break;
			case BikePower::CTF_SLOPE:	// Save slope to flash
				bpPages->EncodeCTFCalibrationPage(ucMsgCode_, usCalSlopeToSend, aucAckBuffer);
				break;
			case BikePower::CTF_SERIAL:	// Save serial to flash
				bpPages->EncodeCTFCalibrationPage(ucMsgCode_, usCalSerialToSend, aucAckBuffer);
				break;
			default:
				break;
		}
	}
	// If a non calibration request should be sent
	else
	{
		switch(ucMsgCode_)
		{
			case BikePower::PAGE_GET_SET_PARAMETERS:
				bpPages->EncodeMainData(ucMsgCode_, aucAckBuffer);
				break;
			case CommonData::PAGE70:
				commonPages->Encode(commonPages->PAGE70, aucAckBuffer);
				break;
			default:
				return;
		}
	}

	if(!ucMsgExpectingAck)
	{
		ucAckRetryCount = 0;
		ucMsgExpectingAck = ucMsgCode_;
	}

	requestAckMsg(aucAckBuffer);
}
/**************************************************************************
 * BikePowerDisplay::GetDifference
 * 
 * Gets the difference between the current and previous value, considering
 * rollover
 * Template allows using the function for UCHAR, USHORT and ULONG types
 *
 * CurrentVal_: Current value
 * PreviousVal_: Previous value
 *
 * returns:  difference between the two values
 *
 **************************************************************************/
template< typename T>
T BikePowerDisplay::GetDifference(T CurrentVal_, T PreviousVal_)
{
	T MaxVal = (T) 0xFFFFFF;
	T Diff = CurrentVal_ - PreviousVal_;
	if(PreviousVal_ > CurrentVal_)
		Diff = MaxVal + Diff + 1;		// handle rollover
	return Diff;
}

void BikePowerDisplay::checkBox_InvalidCrankLength_CheckedChanged(System::Object ^sender, System::EventArgs ^e)
{
	if(checkBox_InvalidCrankLength->Checked)
	{
		checkBox_AutoCrank->Checked = false;
		numericUpDown_CrankLength->Enabled = false;
	}
	else
	{
		numericUpDown_CrankLength->Enabled = true;
	}
}

void BikePowerDisplay::checkBox_AutoCrank_CheckedChanged(System::Object ^sender, System::EventArgs ^e)
{
	if(checkBox_AutoCrank->Checked)
	{
		checkBox_InvalidCrankLength->Checked = false;
		numericUpDown_CrankLength->Enabled = false;
	}
	else
		numericUpDown_CrankLength->Enabled = true;
}
