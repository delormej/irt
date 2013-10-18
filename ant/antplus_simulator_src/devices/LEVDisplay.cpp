/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/



#pragma once
#include "StdAfx.h"
#include "LEVDisplay.h"


/**************************************************************************
* LEVDisplay::InitializeSim
* 
* Initialize the simulator variables
*
* returns: N/A
*
**************************************************************************/
void LEVDisplay::InitializeSim()
{
	dbDispAcumDist = 0;
	bPage16Enabled = TRUE;
	
	ucAckRetryCount = 0;
	ucMsgExpectingAck = 0;
	
	ucRxMsgCount = 0;

	ucDisplaySupportedAssist = 7;
	ucDisplaySupportedRegen = 7;

	// inialize values to match UI
	LEVData->usP16ManID = (System::Convert::ToUInt16(this->numericUpDown_P16ManID->Value)); 
	LEVData->usP16WheelCircum = (System::Convert::ToUInt16(this->numericUpDown_P16Circum->Value)); 
	LEVData->usP16WheelCircum |= 0xF000;
	LEVData->ucP70TxTimes = System::Convert::ToByte(this->numericUpDown_RqTxTimes->Value);
	LEVData->ucP70RequestedPage = LEVData->PAGE1_SPEED_SYSTEM1;
	//LEVData->usP16DisplayCommand = 0; 
	LEVData->ucP16TravelMode = 0;

	// required common pages
	commonDataDisplay->ulSerialNum = System::Convert::ToUInt32(this->textBox_SerialNum->Text);
	commonDataDisplay->ucSwVersion = System::Convert::ToByte(this->textBox_SwVersion->Text);
	commonDataDisplay->ucHwVersion = System::Convert::ToByte(this->textBox_HwVersion->Text);
	commonDataDisplay->usMfgID = System::Convert::ToUInt16(this->textBox_MfgID->Text);
	commonDataDisplay->usModelNum = System::Convert::ToUInt16(this->textBox_ModelNum->Text);
}


/**************************************************************************
* LEVDisplay::ANT_eventNotification
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
void LEVDisplay::ANT_eventNotification(UCHAR ucEventCode_, UCHAR* pucEventBuffer_)
{
	UCHAR ucPageNum = pucEventBuffer_[0];

	switch(ucEventCode_)
	{
		case EVENT_RX_ACKNOWLEDGED:
		case EVENT_RX_BURST_PACKET: // intentional fall thru
		case EVENT_RX_BROADCAST:
			// check if this is common page or LEV page
			if (ucPageNum < commonDataSensor->START_COMMON_PAGE)
				LEVData->Decode(pucEventBuffer_);
			else
				commonDataSensor->Decode(pucEventBuffer_);
			// update the UI
			UpdateDisplay(ucPageNum);

			// the first time we receive a page we want to send a common page back
			if (ucRxMsgCount == 0)
				TransmitCommonPage();

			// increment our count every time we receive a page
			ucRxMsgCount++;

			if ((ucRxMsgCount > 19) && (ucMsgExpectingAck != 0x10)) 	
			{
				TransmitCommonPage();		// every 20th message, send a common page back
				ucRxMsgCount = 1;			// reset to 1 so that the about 'first time' is only true once
			}

			break;
		
		case EVENT_TRANSFER_TX_COMPLETED:
			ucAckRetryCount = 0;			// Reset retransmission counter
			ucMsgExpectingAck = 0;			// Clear pending msg code
			UpdateDisplayAckStatus(LEVData->ACK_SUCCESS);	// Tx successful
			break;
		case EVENT_TRANSFER_TX_FAILED:	
		case EVENT_ACK_TIMEOUT:		// Intentional fall thru
			if(ucMsgExpectingAck)
			{
				if(HandleRetransmit())
					UpdateDisplayAckStatus(LEVData->ACK_RETRY);	// Data was retransmitted
				else
					UpdateDisplayAckStatus(LEVData->ACK_FAIL);	// Maximum number of retries exceeded, Tx failed
			}
			break;

		case EVENT_RX_FAIL:
			ucRxMsgCount++;			// also count the rx fails as messages
			break;


		default:
			break;
	}
}


/**************************************************************************
* LEVDisplay::UpdateDisplay
* 
* Updates the variables found on the GUI
*
* returns: N/A
*
**************************************************************************/
void LEVDisplay::UpdateDisplay(UCHAR ucPageNum)
{	
	// update the common P1, P2, and P3 fields if we get any of those
	if ((ucPageNum == LEVData->PAGE1_SPEED_SYSTEM1) || (ucPageNum == LEVData->PAGE2_SPEED_DISTANCE) 
		|| (ucPageNum == LEVData->PAGE3_SPEED_SYSTEM2))
	{
		this->label_P1SpeedValue->Text = (System::Convert::ToDecimal(LEVData->usCurrentLEVSpeed) / 10).ToString("N1") + " km/h";   // speed comes in as 0.1 km / hr so multiply by 10
		this->label_P2SpeedVal->Text = (System::Convert::ToDecimal(LEVData->usCurrentLEVSpeed) / 10).ToString("N1");   // speed comes in as 0.1 km / hr so multiply by 10
		this->label_P3SpeedVal->Text = (System::Convert::ToDecimal(LEVData->usCurrentLEVSpeed) / 10).ToString("N1") + " km/h";   // speed comes in as 0.1 km / hr so multiply by 10
	}

	// update the common P1, and P3 fields if we get any of those
	if ((ucPageNum == LEVData->PAGE1_SPEED_SYSTEM1) || (ucPageNum == LEVData->PAGE3_SPEED_SYSTEM2))
	{
		// throttle
		if ((LEVData->ucSystemState & 0x10) != 0)
		{
			this->label_P1ThrottleVal->Text = "Yes";
			this->label_P3ThrottleVal->Text = "Yes";
		}
		else
		{
			this->label_P1ThrottleVal->Text = "No";
			this->label_P3ThrottleVal->Text = "No";
		}
		
		if ((LEVData->ucSystemState & 0x08) != 0)
		{
			this->label_P1LightValue->Text = "Light On";
			this->label_P3LightVal->Text = "Light On";
		}
		else
		{
			this->label_P1LightValue->Text = "Light Off";
			this->label_P3LightVal->Text = "Light Off";
		}
		
		if ((LEVData->ucSystemState & 0x04) != 0)
		{
			this->label_P1BeamValue->Text = "High Beam";
			this->label_P3BeamVal->Text = "High Beam";
		}
		else
		{
			this->label_P1BeamValue->Text = "Low Beam";
			this->label_P3BeamVal->Text = "Low Beam";
		}

		if ((LEVData->ucSystemState & 0x02) != 0)
		{
			this->label_P1LeftValue->Text = "On";
			this->label_P3LeftVal->Text = "On";
		}
		else
		{
			this->label_P1LeftValue->Text = "Off";
			this->label_P3LeftVal->Text = "Off";
		}

		if ((LEVData->ucSystemState & 0x01) != 0)
		{
			this->label_P1RightValue->Text = "On";
			this->label_P3RightVal->Text = "On";
		}
		else
		{
			this->label_P1RightValue->Text = "Off";
			this->label_P3RightVal->Text = "Off";
		}
		
		// clear everything but bits 3-5, then shift down to get current assist level
		if (((LEVData->ucTravelModeState & 0x38) >> 3) == 0)
		{
			this->label_P1AssistLevelVal->Text = "Off";
			this->label_P3AssistLvlVal->Text = "Off";
		}
		else
			MapAssistMode();

		// clear everything but bits 0-2 to get current regen level
		if ((LEVData->ucTravelModeState & 0x07) == 0)
		{
			this->label_P1RegenLevelVal->Text = "Off";
			this->label_P3RegenVal->Text = "Off";
		}
		else
			MapRegenMode();
		
		// gear exists
		if ((LEVData->ucGearState & 0x80) != 0)
		{
			this->label_P1GearExistVal->Text = "Yes";
			this->label_P3GearsExistVal->Text = "Yes";
		}
		else
		{
			this->label_P1GearExistVal->Text = "No";
			this->label_P3GearsExistVal->Text = "No";
		}
		
		// gear manual or auto
		if ((LEVData->ucGearState & 0x40) != 0)
		{
			this->label_P1ManAutoVal->Text = "Man";
			this->label_P3ManAutoVal->Text = "Man";
		}
		else
		{
			this->label_P1ManAutoVal->Text = "Auto";
			this->label_P3ManAutoVal->Text = "Auto";
		}
		
		// clear everything but bits 2-5, then shift down to get rear gear setting
		if (((LEVData->ucGearState & 0x3C) >> 2) == 0)
		{
			this->label_P1RearGearVal->Text = "None";
			this->label_P3RearGearVal->Text = "None";
		}
		else
		{
			this->label_P1RearGearVal->Text = ((LEVData->ucGearState & 0x3C) >> 2).ToString();
			this->label_P3RearGearVal->Text = ((LEVData->ucGearState & 0x3C) >> 2).ToString();
		}

		// clear everything but bits 0-1 to get front gear setting
		if ((LEVData->ucGearState & 0x03) == 0)
		{
			this->label_P1FrontGearVal->Text = "None";
			this->label_P3FrontGearVal->Text = "None";
		}
		else
		{
			this->label_P1FrontGearVal->Text = (LEVData->ucGearState & 0x03).ToString();
			this->label_P3FrontGearVal->Text = (LEVData->ucGearState & 0x03).ToString();
		}
	}
	
	//*****************************
	// Decode Page 1
	//*****************************
	if (ucPageNum == LEVData->PAGE1_SPEED_SYSTEM1)
	{
		if ((LEVData->ucTemperature & 0x80) != 0)			// if bit 7 is high...
			this->label_P1MotorAlertVal->Text = "Overheating";
		else
			this->label_P1MotorAlertVal->Text = "No Alert";

		if ((LEVData->ucTemperature & 0x08) != 0)			// if bit 3 is high...
			this->label_P1BattAlertVal->Text = "Overheating";
		else
			this->label_P1BattAlertVal->Text = "No Alert";

		// clear everything but bits 0-2 to get battery temp
		if ((LEVData->ucTemperature & 0x07) == 0)
			this->label_P1BattTempVal->Text = "Unknown";
		else if ((LEVData->ucTemperature & 0x07) == 1)
			this->label_P1BattTempVal->Text = "Cold";
		else if ((LEVData->ucTemperature & 0x07) == 2)
			this->label_P1BattTempVal->Text = "Cold/Warm";
		else if ((LEVData->ucTemperature & 0x07) == 3)
			this->label_P1BattTempVal->Text = "Warm";
		else if ((LEVData->ucTemperature & 0x07) == 4)
			this->label_P1BattTempVal->Text = "Warm/Hot";
		else if ((LEVData->ucTemperature & 0x07) == 5)
			this->label_P1BattTempVal->Text = "Hot";

		// clear everything but bits 4-6, then shift down to get motor temp
		if (((LEVData->ucTemperature & 0x70) >> 4) == 0)
			this->label_P1MotorTempVal->Text = "Unknown";
		else if (((LEVData->ucTemperature & 0x70) >> 4) == 1)
			this->label_P1MotorTempVal->Text = "Cold";
		else if (((LEVData->ucTemperature & 0x70) >> 4) == 2)
			this->label_P1MotorTempVal->Text = "Cold/Warm";
		else if (((LEVData->ucTemperature & 0x70) >> 4) == 3)
			this->label_P1MotorTempVal->Text = "Warm";
		else if (((LEVData->ucTemperature & 0x70) >> 4) == 4)
			this->label_P1MotorTempVal->Text = "Warm/Hot";
		else if (((LEVData->ucTemperature & 0x70) >> 4) == 5)
			this->label_P1MotorTempVal->Text = "Hot";
		
		switch (LEVData->ucErrorMessage)
		{
		case 0:
			this->label_P1ErrorValue->Text = "0: No Error";
			break;
		case 1:
			this->label_P1ErrorValue->Text = "1: Batt Error";
			break;
		case 2:
			this->label_P1ErrorValue->Text = "2: Train Error";
			break;                               
		case 3:
			this->label_P1ErrorValue->Text = "3: Batt EOL";
			break;
		case 4:
			this->label_P1ErrorValue->Text = "4: Overheat";
			break;
		default:
			if (LEVData->ucErrorMessage > 15)
				this->label_P1ErrorValue->Text = LEVData->ucErrorMessage.ToString() + ": Custom";
			else
				this->label_P1ErrorValue->Text = "Invalid Code";
		}
	}

	//*****************************
	// Decode Page 2
	//*****************************
	else if (ucPageNum == LEVData->PAGE2_SPEED_DISTANCE)
	{
		this->label_P2OdometerVal->Text = ((float)(LEVData->ulOdometer) / 100).ToString("N2");
		if (LEVData->usRemainingRange == 0)
			this->label_P2RangeVal->Text = "Unused";
		else 
			this->label_P2RangeVal->Text = LEVData->usRemainingRange.ToString();
	}
	
	
	//*****************************
	// Decode Page 3
	//*****************************
	else if (ucPageNum == LEVData->PAGE3_SPEED_SYSTEM2)
	{
		this->label_P3PercentAssistVal->Text = LEVData->ucPercentAssist.ToString() + "%";

		if ((LEVData->ucBatteryCharge & 0x80) != 0)		// check if bit 7 high
		{
			this->label_P3BattEmptyVal->Text = "On";
			this->label_P3ChargeVal->Text = (LEVData->ucBatteryCharge ^ 0x80).ToString() + "%";
		}
		else
		{
			this->label_P3BattEmptyVal->Text = "Off";
			this->label_P3ChargeVal->Text = LEVData->ucBatteryCharge.ToString() + "%";
		}
	}

	//*****************************
	// Decode Page 4
	//*****************************
	else if (ucPageNum == LEVData->PAGE4_BATTERY)
	{
		if (LEVData->ucBatteryVoltage == 0)
			this->label_P4VoltageVal->Text = "Unused";
		else 
			this->label_P4VoltageVal->Text = LEVData->ucBatteryVoltage.ToString();
		
		if ((LEVData->usChargeCycleCount & 0x0FFF) == 0)
			this->label_P4ChargeCycleVal->Text = "Unused";
		else
			this->label_P4ChargeCycleVal->Text = LEVData->usChargeCycleCount.ToString();
		if (LEVData->usFuelConsumption  == 0)
			this->label_P4FuelVal->Text = "Unused";
		else
			this->label_P4FuelVal->Text = (System::Convert::ToDecimal(LEVData->usFuelConsumption) / 10).ToString("N1") + " Wh/km";   // fuel comes in as 0.1 km / hr so multiply by 10
		if (LEVData->usDistanceOnCharge == 0)
			this->label_P4DistanceChargeVal->Text = "Unused";
		else
			this->label_P4DistanceChargeVal->Text = (System::Convert::ToDecimal(LEVData->usDistanceOnCharge) / 10).ToString("N1") + " km";   // dist on charge comes in as 0.1 km / hr so multiply by 10

	}


	//*****************************
	// Decode Page 5
	//*****************************
	else if (ucPageNum == LEVData->PAGE5_CAPABILITES)
	{
		// clear everything but bits 3-5, then shift down to get assist modes
		this->label_P5NumAssistVal->Text = ((LEVData->ucSupportedTravelModes & 0x38) >> 3).ToString();

		// clear everything but bits 0-2 to get regen modes
		this->label_P5NumRegenVal->Text = (LEVData->ucSupportedTravelModes & 0x07).ToString();

		if(LEVData->usWheelCircumference == 0x00)
			this->label_P5CircumVal->Text = "Unused";
		else
			this->label_P5CircumVal->Text = LEVData->usWheelCircumference.ToString();
	}
	else if(ucPageNum == LEVData->PAGE34_SPEED_DISTANCE)
	{
		this->label_P2OdometerVal->Text = ((float)(LEVData->ulP34Odometer) / 100).ToString("N2");
		if(LEVData->usP34FuelConsumption == 0x00)
		{
			this->label_P34FuelValue->Text = "Unused";
			this->label_P4FuelVal->Text = "Unused";
		}
		else
		{
			this->label_P34FuelValue->Text = (System::Convert::ToDecimal(LEVData->usP34FuelConsumption / 10).ToString("N1"));
			this->label_P4FuelVal->Text = (System::Convert::ToDecimal(LEVData->usFuelConsumption) / 10).ToString("N1") + " Wh/km";   // fuel comes in as 0.1 km / hr so multiply by 10
		}
		this->label_P2SpeedVal->Text = (System::Convert::ToDecimal(LEVData->usCurrentLEVSpeed) / 10).ToString("N1"); 
	}


	// update the values for the HW common data page
	if (commonDataSensor->usMfgID != 0)
	{
		this->label_Glb_ManfIDDisplay->Text = commonDataSensor->usMfgID.ToString();
		this->label_Glb_HardwareVerDisplay->Text = commonDataSensor->ucHwVersion.ToString();
		this->label_Glb_ModelNumDisplay->Text = commonDataSensor->usModelNum.ToString();
	}

	// update the values for the SW common data page
	if (commonDataSensor->ulSerialNum != 0)
	{
		if(commonDataSensor->ulSerialNum == 0xFFFFFFFF)
			this->label_Glb_SerialNumDisplay->Text = "N/A";
		else
			this->label_Glb_SerialNumDisplay->Text = commonDataSensor->ulSerialNum.ToString();
		this->label_Glb_SoftwareVerDisplay->Text = commonDataSensor->ucSwVersion.ToString();

	}


	// the tx variables will show the raw transmitted data decoded, but not scaled into proper units
	this->label_StatusSpeedVal->Text = LEVData->usCurrentLEVSpeed.ToString();
	this->label_StatusOdoVal->Text = LEVData->ulOdometer.ToString();
	this->label_StatusAssistVal->Text = LEVData->ucPercentAssist.ToString();


}


void LEVDisplay::MapAssistMode()
{
	UCHAR AssistMode = ((LEVData->ucTravelModeState & 0x38) >> 3);

	if(ucDisplaySupportedAssist == 1)	// Display supports 1 Assist Mode
	{
			this->label_P1AssistLevelVal->Text = "7";
			this->label_P3AssistLvlVal->Text = "7";	
	}
	else if(ucDisplaySupportedAssist == 2)	// Display supports 2 Assist Modes
	{
		if(AssistMode == 1 || AssistMode == 2 || AssistMode == 3)	// map requested assist modes 1,2,3 to Display assist mode 3
		{
			this->label_P1AssistLevelVal->Text = "3";
			this->label_P3AssistLvlVal->Text = "3";
		}
		else	// map requested assist modes 4,5,6,7 to Display assist mode 7
		{
			this->label_P1AssistLevelVal->Text = "7";
			this->label_P3AssistLvlVal->Text = "7";	
		}
	}
	else if(ucDisplaySupportedAssist == 3)	// Display supports 3 Assist Modes
	{
		if(AssistMode == 1 || AssistMode == 2)	// map requeseted assist modes 1,2 to Display assist mode 2
		{
			this->label_P1AssistLevelVal->Text = "2";
			this->label_P3AssistLvlVal->Text = "2";	
		}
		else if(AssistMode == 3 || AssistMode == 4)	// map requested assist modes 3,4 to Display assist mode 4
		{			
			this->label_P1AssistLevelVal->Text = "4";
			this->label_P3AssistLvlVal->Text = "4";	
		}
		else	// map requested assist modes 5,6,7 to Display assist mode 7
		{		
			this->label_P1AssistLevelVal->Text = "7";
			this->label_P3AssistLvlVal->Text = "7";	
		}
	}
	else if(ucDisplaySupportedAssist == 4)	// Display supports 4 Assist Modes
	{
		if(AssistMode == 1)	// No mapping required for incoming assist mode 1
		{
			this->label_P1AssistLevelVal->Text = "1";
			this->label_P3AssistLvlVal->Text = "1";	
		}
		else if(AssistMode == 2 || AssistMode == 3)	// map incoming assist modes 2,3 to Display assist mode 3
		{
			this->label_P1AssistLevelVal->Text = "3";
			this->label_P3AssistLvlVal->Text = "3";	
		}
		else if(AssistMode == 4 || AssistMode == 5) // map incoming assist modes 4, 5 to Display assist mode 5
		{
			this->label_P1AssistLevelVal->Text = "5";
			this->label_P3AssistLvlVal->Text = "5";	
		}
		else	// map incoming assist modes 6,7 to Display assist mode 7
		{
			this->label_P1AssistLevelVal->Text = "7";
			this->label_P3AssistLvlVal->Text = "7";	
		}
	}
	else if(ucDisplaySupportedAssist == 5)	// Display supports 5 Assist Modes
	{
		if(AssistMode == 1 || AssistMode == 2 || AssistMode == 3)	// No mapping required for incoming assist modes 1,2,3
		{
			this->label_P1AssistLevelVal->Text = ((LEVData->ucTravelModeState & 0x38) >> 3).ToString();
			this->label_P3AssistLvlVal->Text = ((LEVData->ucTravelModeState & 0x38) >> 3).ToString();
		}
		else if(AssistMode == 4 || AssistMode == 5)	// map incoming assist modes 4,5 to Display assist mode 5
		{
			this->label_P1AssistLevelVal->Text = "5";
			this->label_P3AssistLvlVal->Text = "5";	
		}
		else	// map incoming assist modes 6,7 to Display assist mode 7
		{
			this->label_P1AssistLevelVal->Text = "7";
			this->label_P3AssistLvlVal->Text = "7";	
		}
	}
	else if(ucDisplaySupportedAssist == 6)	// Display supports 6 Assist Modes
	{
		if(AssistMode == 1 || AssistMode == 2 || AssistMode == 3 || AssistMode == 4 || AssistMode == 5)	// No mapping required for incoming assist modes 1,2,3,4,5
		{
			this->label_P1AssistLevelVal->Text = ((LEVData->ucTravelModeState & 0x38) >> 3).ToString();
			this->label_P3AssistLvlVal->Text = ((LEVData->ucTravelModeState & 0x38) >> 3).ToString();
		}
		else	// map incoming assist modes 6,7 to assist mode 7
		{
			this->label_P1AssistLevelVal->Text = "7";
			this->label_P3AssistLvlVal->Text = "7";	
		}
	}
	else	// No mapping required if all assist modes are supported
	{
			this->label_P1AssistLevelVal->Text = ((LEVData->ucTravelModeState & 0x38) >> 3).ToString();
			this->label_P3AssistLvlVal->Text = ((LEVData->ucTravelModeState & 0x38) >> 3).ToString();
	}
}
void LEVDisplay::MapRegenMode()
{
	UCHAR RegenMode = (LEVData->ucTravelModeState & 0x07);

	if(ucDisplaySupportedRegen == 0)	// Display does not support Regen Modes
	{
			this->label_P1RegenLevelVal->Text = "Off";
			this->label_P3RegenVal->Text = "Off";
	}
	else if(ucDisplaySupportedRegen == 1)	// Display supports 1 Regen Mode
	{
			this->label_P1RegenLevelVal->Text = "7";
			this->label_P3RegenVal->Text = "7";
	}
	else if(ucDisplaySupportedRegen == 2)	// Display supports 2 Regen Modes
	{
		if(RegenMode == 1 || RegenMode == 2 || RegenMode == 3)	// map requested regen modes 1,2,3 to Display regen mode 3
		{
			this->label_P1RegenLevelVal->Text = "3";
			this->label_P3RegenVal->Text = "3";
		}
		else	// map requested regen modes 4,5,6,7 to Display regen mode 7
		{
			this->label_P1RegenLevelVal->Text = "7";
			this->label_P3RegenVal->Text = "7";
		}
	}
	else if(ucDisplaySupportedRegen == 3)	// Display supports 3 Regen Modes
	{
		if(RegenMode == 1 || RegenMode == 2)	// map requeseted regen modes 1,2 to Display regen mode 2
		{
			this->label_P1RegenLevelVal->Text = "2";
			this->label_P3RegenVal->Text = "2";
		}
		else if(RegenMode == 3 || RegenMode == 4)	// map requested regen modes 3,4 to Display regen mode 4
		{			
			this->label_P1RegenLevelVal->Text = "4";
			this->label_P3RegenVal->Text = "4";
		}
		else	// map requested regen modes 5,6,7 to Display regen mode 7
		{		
			this->label_P1RegenLevelVal->Text = "7";
			this->label_P3RegenVal->Text = "7";
		}
	}
	else if(ucDisplaySupportedRegen == 4)	// Display supports 4 Regen Modes
	{
		if(RegenMode == 1)	// No mapping required for incoming regen mode 1
		{
			this->label_P1RegenLevelVal->Text = "1";
			this->label_P3RegenVal->Text = "1";
		}
		else if(RegenMode == 2 || RegenMode == 3)	// map incoming regen modes 2,3 to Display regen mode 3
		{
			this->label_P1RegenLevelVal->Text = "3";
			this->label_P3RegenVal->Text = "3";
		}
		else if(RegenMode == 4 || RegenMode == 5) // map incoming regen modes 4, 5 to Display regen mode 5
		{
			this->label_P1RegenLevelVal->Text = "5";
			this->label_P3RegenVal->Text = "5";
		}
		else	// map incoming regen modes 6,7 to Display regen mode 7
		{
			this->label_P1RegenLevelVal->Text = "7";
			this->label_P3RegenVal->Text = "7";
		}
	}
	else if(ucDisplaySupportedRegen == 5)	// Display supports 5 Regen Modes
	{
		if(RegenMode == 1 || RegenMode == 2 || RegenMode == 3)	// No mapping required for incoming regen modes 1,2,3
		{
			this->label_P1RegenLevelVal->Text = (LEVData->ucTravelModeState & 0x07).ToString();
			this->label_P3RegenVal->Text = (LEVData->ucTravelModeState & 0x07).ToString();
		}
		else if(RegenMode == 4 || RegenMode == 5)	// map incoming regen modes 4,5 to Display regen mode 5
		{
			this->label_P1RegenLevelVal->Text = "5";
			this->label_P3RegenVal->Text = "5";
		}
		else	// map incoming regen modes 6,7 to Display regen mode 7
		{
			this->label_P1RegenLevelVal->Text = "7";
			this->label_P3RegenVal->Text = "7";
		}
	}
	else if(ucDisplaySupportedRegen == 6)	// Display supports 6 Regen Modes
	{
		if(RegenMode == 1 || RegenMode == 2 || RegenMode == 3 || RegenMode == 4 || RegenMode == 5)	// No mapping required for incoming regen modes 1,2,3,4,5
		{
			this->label_P1RegenLevelVal->Text = (LEVData->ucTravelModeState & 0x07).ToString();
			this->label_P3RegenVal->Text = (LEVData->ucTravelModeState & 0x07).ToString();
		}
		else	// map incoming regen modes 6,7 to regen mode 7
		{
			this->label_P1RegenLevelVal->Text = "7";
			this->label_P3RegenVal->Text = "7";
		}
	}
	else	// No mapping required if all regen modes are supported
	{
			this->label_P1RegenLevelVal->Text = (LEVData->ucTravelModeState & 0x07).ToString();
			this->label_P3RegenVal->Text = (LEVData->ucTravelModeState & 0x07).ToString();
	}
}
/**************************************************************************
 * LEVDisplay::checkBox_P16TravelModeNotSupported_CheckedChanged
 * 
 * Enables or Disables the Travel Mode field in Page 16
 *
 * returns: N/A
 *
 **************************************************************************/
void LEVDisplay::checkBox_P16TravelModeNotSupported_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	if (this->checkBox_P16TravelModeNotSupported->Checked == TRUE)
	{
		LEVData->ucP16TravelMode = 0xFF;			 // 0xFF = not supported   		
		this->comboBox_P16Assist->Enabled = FALSE;
		this->comboBox_P16Regen->Enabled = FALSE;
	}
	else
	{
		LEVData->ucP16TravelMode = LEVData->ucP16TravelMode & 0x3F; // Clear bits 6 and 7	
		this->comboBox_P16Assist->Enabled = TRUE;
		this->comboBox_P16Regen->Enabled = TRUE;
		UpdateAssistMode();						    // refresh values in the ucP16TravelMode variable
		UpdateRegenMode();
	}
}


/**************************************************************************
 * LEVDisplay::comboBox_P16Assist_SelectedIndexChanged
 * 
 * Updates the user selected Assist Level to send to the sensor
 *
 * returns: N/A
 *
 **************************************************************************/
void LEVDisplay::comboBox_P16Assist_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
{
	CheckAssistMapping();
}


/**************************************************************************
 * LEVDisplay::CheckAssistMapping
 * 
 * Make sure the selected Assist Level is consistent with the supported 
 * assist levels
 *
 * returns: N/A
 *
 **************************************************************************/
void LEVDisplay::CheckAssistMapping()
{
	UCHAR CurrentAssist = (this->LEVData->ucP16TravelMode & 0x38) >> 3;
	// if Assist Off is selected, set the assist level to 000
	if(this->comboBox_P16Assist->Text->Equals("Assist Off"))
		LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xC7);
	else if(ucDisplaySupportedAssist == 7)
		UpdateAssistMode();
	else 
	{
		// switch number of supported assist modes
		switch(ucDisplaySupportedAssist)
		{
		case 0x01:	// 1 assist modes supported, selecting "Assist 7" acceptable
			// if anything other than "Assist Off" is selected, Send a messagebox with an error message
			if(!(this->comboBox_P16Assist->Text->Equals("Assist 7")))
			{
				MessageBox::Show("The Display supports 1 Assist Mode. Please select Assist 7 or Off");
				switch(CurrentAssist)
				{
				case 0x07:
					this->comboBox_P16Assist->Text = "Assist 7";
					break;
				default:
					this->comboBox_P16Assist->Text = "Assist Off";
					break;
				}
			}
			// if "Assist Off" was selected, it would have been caught in the above if statement
			else
				UpdateAssistMode();
			break;
		case 0x02:	// 2 assist modes supported, selecting "Assist 3" or "Assist 7" acceptable
			// if anything other than "Assist 3" or "Assist 7" is selected, send messagebox with error message
			if(!((this->comboBox_P16Assist->Text->Equals("Assist 3")) || (this->comboBox_P16Assist->Text->Equals("Assist 7"))))
			{
				MessageBox::Show("The Display supports 2 Assist Modes. Please select Assist 3, 7 or Off");
				switch(CurrentAssist)
				{
				case 0x03:
					this->comboBox_P16Assist->Text = "Assist 3";
					break;
				case 0x07:
					this->comboBox_P16Assist->Text = "Assist 7";
					break;
				default:
					this->comboBox_P16Assist->Text = "Assist Off";
					break;
				}
			}
			// if "Assist Off" was selected, it would have been caught in the above if statement
			else
				UpdateAssistMode();
			break;
		case 0x03:	// 3 Assist modes supported, selecting "Assist 2", "Assist 4" or "Assist 7" acceptable
			// if anything other than "Assist 2", "Assist 4", or "Assist 7" is selected, show messagebox with an error message
			if(!((this->comboBox_P16Assist->Text->Equals("Assist 2")) || (this->comboBox_P16Assist->Text->Equals("Assist 4")) || (this->comboBox_P16Assist->Text->Equals("Assist 7"))))
			{
				MessageBox::Show("The Display supports 3 Assist Modes. Please select Assist 2, 4, 7, or Off");
				switch(CurrentAssist)
				{
				case 0x02:
					this->comboBox_P16Assist->Text = "Assist 2";
					break;
				case 0x04:
					this->comboBox_P16Assist->Text = "Assist 4";
					break;
				case 0x07:
					this->comboBox_P16Assist->Text = "Assist 7";
					break;
				default:
					this->comboBox_P16Assist->Text = "Assist Off";
					break;
				}
			}
			// if "Assist Off" was selected, it would have been caught in the above if statement
			else
				UpdateAssistMode();
			break;
		case 0x04:	// 4 assist modes supported, selecting "Assist 2", "Assist 4" or "Assist 6" NOT acceptable
			// if "Assist 2", "Assist 4" or "Assist 6" selected, show messagebox with error message
			if((this->comboBox_P16Assist->Text->Equals("Assist 2")) || (this->comboBox_P16Assist->Text->Equals("Assist 4")) || (this->comboBox_P16Assist->Text->Equals("Assist 6")))
			{
				MessageBox::Show("The Display supports 4 Assist Modes. Please select Assist 1, 3, 5, 7, or Off");
				switch(CurrentAssist)
				{
				case 0x01:
					this->comboBox_P16Assist->Text = "Assist 1";
					break;
				case 0x03:
					this->comboBox_P16Assist->Text = "Assist 3";
					break;
				case 0x05:
					this->comboBox_P16Assist->Text = "Assist 5";
					break;
				case 0x07:
					this->comboBox_P16Assist->Text = "Assist 7";
					break;
				default:
					this->comboBox_P16Assist->Text = "Assist Off";
					break;
				}
			}
			// if "Assist Off" selected, it would have been caught in the above if statement
			else
				UpdateAssistMode();
			break;
		case 0x05:	// 5 assist modes supported, selecting "Assist 4" or "Assist 6" NOT acceptable
			// if "Assist 4" or "Assist 6" selected, show messagebox with error message
			if((this->comboBox_P16Assist->Text->Equals("Assist 4")) || (this->comboBox_P16Assist->Text->Equals("Assist 6")))
			{
				MessageBox::Show("The Display supports 5 Assist Modes. Please select Assist 1, 2, 3, 5, 7, or Off");
				switch(CurrentAssist)
				{
				case 0x01:
					this->comboBox_P16Assist->Text = "Assist 1";
					break;
				case 0x02:
					this->comboBox_P16Assist->Text = "Assist 2";
					break;
				case 0x03:
					this->comboBox_P16Assist->Text = "Assist 3";
					break;
				case 0x05:
					this->comboBox_P16Assist->Text = "Assist 5";
					break;
				case 0x07:
					this->comboBox_P16Assist->Text = "Assist 7";
					break;
				default:
					this->comboBox_P16Assist->Text = "Assist Off";
					break;
				}
			}
			// if "Assist Off" selected, it would have been caught in the above if statement
			else
				UpdateAssistMode();
			break;
		case 0x06:	// 6 assist modes supported, selecting "Assist 6" NOT acceptable
			// if "Assist 6" selected, show messagebox with error message
			if((this->comboBox_P16Assist->Text->Equals("Assist 6")))
			{
				MessageBox::Show("The Display supports 6 Assist Modes. Please select Assist 1, 2,3, 4, 5, 7 or Off");
				switch(CurrentAssist)
				{
				case 0x01:
					this->comboBox_P16Assist->Text = "Assist 1";
					break;
				case 0x02:
					this->comboBox_P16Assist->Text = "Assist 2";
					break;
				case 0x03:
					this->comboBox_P16Assist->Text = "Assist 3";
					break;
				case 0x04:
					this->comboBox_P16Assist->Text = "Assist 4";
					break;
				case 0x05:
					this->comboBox_P16Assist->Text = "Assist 5";
					break;
				case 0x07:
					this->comboBox_P16Assist->Text = "Assist 7";
					break;
				default:
					this->comboBox_P16Assist->Text = "Assist Off";
					break;
				}
			}
			// if "Assist Off" selected, it would have been caught in the above if statement
			else
				UpdateAssistMode();
			break;
		default:
			// set Assist mode to off on default
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode& 0xC7);
		}
	}
}
/**************************************************************************
 * LEVDisplay::UpdateAssistMode
 * 
 * Sets the Assist Level, Page 16, Byte 3, bits 3-5
 *
 * returns: N/A
 *
 **************************************************************************/
void LEVDisplay::UpdateAssistMode()
{
	if (this->comboBox_P16Assist->Text->Equals("Assist 1"))		// 001 = Assist 1
	{
		LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xC7);	// clear the bits 3-5 only 
		LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x08);	// set bit 3 high
	}
	else if (this->comboBox_P16Assist->Text->Equals("Assist 2"))		// 010 = Assist 2
	{
		LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xC7);	// clear the bits 3-5 only 
		LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x10);	// set bit 4 high
	}
	else if (this->comboBox_P16Assist->Text->Equals("Assist 3"))		// 011 = Assist 3
	{
		LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xC7);	// clear the bits 3-5 only 
		LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x18);	// set bits 3,4 high
	}
	else if (this->comboBox_P16Assist->Text->Equals("Assist 4"))		// 100 = Assist 4
	{
		LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xC7);	// clear the bits 3-5 only 
		LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x20);	// set bit 5 high
	}
	else if (this->comboBox_P16Assist->Text->Equals("Assist 5"))		// 101 = Assist 5
	{
		LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xC7);	// clear the bits 3-5 only 
		LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x28);	// set bits 5,3 high
	}
	else if (this->comboBox_P16Assist->Text->Equals("Assist 6"))		// 110 = Assist 6
	{
		LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xC7);	// clear the bits 3-5 only 
		LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x30);	// set bits 5,4 high
	}
	else if (this->comboBox_P16Assist->Text->Equals("Assist 7"))		// 111 = Assist 7
	{
		LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xC7);	// clear the bits 3-5 only 
		LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x38);	// set bits 5,4,3 high
	}
}


/**************************************************************************
 * LEVDisplay::comboBox_P16Regen_SelectedIndexChanged
 * 
 * Updates the user selected Current Regen Level
 *
 * returns: N/A
 *
 **************************************************************************/
void LEVDisplay::comboBox_P16Regen_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
{
	CheckRegenMapping();
}


/**************************************************************************
 * LEVDisplay::CheckRegenMapping
 * 
 * Make sure the selected Regen Level is consistent with the supported 
 * regen levels
 *
 * returns: N/A
 *
 **************************************************************************/
void LEVDisplay::CheckRegenMapping()
{
	UCHAR CurrentRegen = this->LEVData->ucP16TravelMode & 0x07;

	// if Regen Off is selected, set the regen level to 000
	if(this->comboBox_P16Regen->Text->Equals("Regenerative Off"))
		LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xF8);
	else if(ucDisplaySupportedRegen == 7)
		UpdateRegenMode();
	else 
	{
		// switch number of supported regen modes
		switch(ucDisplaySupportedRegen)
		{
		case 0x00:
			if((!this->comboBox_P16Regen->Text->Equals("Regenerative Off")))
			{
				MessageBox::Show("The Display does not support Regenerative Modes. Please select Regenerative Off");
				this->comboBox_P16Regen->Text = "Regenerative Off";
			}
			break;
		case 0x01:	// 1 regen modes supported, selecting "Regenerative 7" acceptable
			// if anything other than "Regenerative  Off" is selected, Send a messagebox with an error message
			if(!(this->comboBox_P16Regen->Text->Equals("Regenerative 7")))
			{
				MessageBox::Show("The Display supports 1 Regenerative Mode. Please select Regenerative 7 or Off");
				switch(CurrentRegen)
				{
				case 0x07:
					this->comboBox_P16Regen->Text = "Regenerative 7";
					break;
				default:
					this->comboBox_P16Regen->Text = "Regenerative Off";
					break;
				}
			}
			// if "Regenerative Off" was selected, it would have been caught in the above if statement
			else
				UpdateRegenMode();
			break;
		case 0x02:	// 2 regen modes supported, selecting "Regenerative 3" or "Regenerative 7" acceptable
			// if anything other than "Regenerative 3" or "Regenerative 7" is selected, send messagebox with error message
			if(!((this->comboBox_P16Regen->Text->Equals("Regenerative 3")) || (this->comboBox_P16Regen->Text->Equals("Regenerative 7"))))
			{
				MessageBox::Show("The Display supports 2 Regenerative Modes. Please select Regenerative 3, 7 or Off");
				switch(CurrentRegen)
				{
				case 0x03:
					this->comboBox_P16Regen->Text = "Regenerative 3";
					break;
				case 0x07:
					this->comboBox_P16Regen->Text = "Regenerative 7";
					break;
				default:
					this->comboBox_P16Regen->Text = "Regenerative Off";
					break;
				}
			}
			// if "Regenerative Off" was selected, it would have been caught in the above if statement
			else
				UpdateRegenMode();
			break;
		case 0x03:	// 3 Regenerative modes supported, selecting "Regenerative 2", "Regenerative 4" or "Regenerative 7" acceptable
			// if anything other than "Regenerative 2", "Regenerative 4", or "Regenerative 7" is selected, show messagebox with an error message
			if(!((this->comboBox_P16Regen->Text->Equals("Regenerative 2")) || (this->comboBox_P16Regen->Text->Equals("Regenerative 4")) || (this->comboBox_P16Regen->Text->Equals("Regenerative 7"))))
			{
				MessageBox::Show("The Display supports 3 Regenerative Modes. Please select Regenerative 2, 4, 7, or Off");
				switch(CurrentRegen)
				{
				case 0x02:
					this->comboBox_P16Regen->Text = "Regenerative 2";
					break;
				case 0x04:
					this->comboBox_P16Regen->Text = "Regenerative 4";
					break;
				case 0x07:
					this->comboBox_P16Regen->Text = "Regenerative 7";
					break;
				default:
					this->comboBox_P16Regen->Text = "Regenerative Off";
					break;
				}
			}
			// if "Regenerative Off" was selected, it would have been caught in the above if statement
			else
				UpdateRegenMode();
			break;
		case 0x04:	// 4 regen modes supported, selecting "Regenerative 2", "Regenerative 4" or "Regenerative 6" NOT acceptable
			// if "Regenerative 2", "Regenerative 4" or "Regenerative 6" selected, show messagebox with error message
			if((this->comboBox_P16Regen->Text->Equals("Regenerative 2")) || (this->comboBox_P16Regen->Text->Equals("Regenerative 4")) || (this->comboBox_P16Regen->Text->Equals("Regenerative 6")))
			{
				MessageBox::Show("The Display supports 4 Regenerative Modes. Please select Regenerative 1, 3, 5, 7, or Off");
				switch(CurrentRegen)
				{
				case 0x01:
					this->comboBox_P16Regen->Text = "Regenerative 1";
					break;
				case 0x03:
					this->comboBox_P16Regen->Text = "Regenerative 3";
					break;
				case 0x05:
					this->comboBox_P16Regen->Text = "Regenerative 5";
					break;
				case 0x07:
					this->comboBox_P16Regen->Text = "Regenerative 7";
					break;
				default:
					this->comboBox_P16Regen->Text = "Regenerative Off";
					break;
				}
			}
			// if "Regenerative Off" selected, it would have been caught in the above if statement
			else
				UpdateRegenMode();
			break;
		case 0x05:	// 5 regen modes supported, selecting "Regenerative 4" or "Regenerative 6" NOT acceptable
			// if "Regenerative 4" or "Regenerative 6" selected, show messagebox with error message
			if((this->comboBox_P16Regen->Text->Equals("Regenerative 4")) || (this->comboBox_P16Regen->Text->Equals("Regenerative 6")))
			{
				MessageBox::Show("The Display supports 5 Regenerative Modes. Please select Regenerative 1, 2, 3, 5, 7, or Off");
				switch(CurrentRegen)
				{
				case 0x01:
					this->comboBox_P16Regen->Text = "Regenerative 1";
					break;
				case 0x02:
					this->comboBox_P16Regen->Text = "Regenerative 2";
					break;
				case 0x03:
					this->comboBox_P16Regen->Text = "Regenerative 3";
					break;
				case 0x05:
					this->comboBox_P16Regen->Text = "Regenerative 5";
					break;
				case 0x07:
					this->comboBox_P16Regen->Text = "Regenerative 7";
					break;
				default:
					this->comboBox_P16Regen->Text = "Regenerative Off";
					break;
				}
			}
			// if "Regenerative Off" selected, it would have been caught in the above if statement
			else
				UpdateRegenMode();
			break;
		case 0x06:	// 6 regen modes supported, selecting "Regenerative 6" NOT acceptable
			// if "Regenerative 6" selected, show messagebox with error message
			if((this->comboBox_P16Regen->Text->Equals("Regenerative 6")))
			{
				MessageBox::Show("The Display supports 6 Regenerative Modes. Please select Regenerative 1, 2,3, 4, 5, 7 or Off");
				switch(CurrentRegen)
				{
				case 0x01:
					this->comboBox_P16Regen->Text = "Regenerative 1";
					break;
				case 0x02:
					this->comboBox_P16Regen->Text = "Regenerative 2";
					break;
				case 0x03:
					this->comboBox_P16Regen->Text = "Regenerative 3";
					break;
				case 0x04:
					this->comboBox_P16Regen->Text = "Regenerative 4";
					break;
				case 0x05:
					this->comboBox_P16Regen->Text = "Regenerative 5";
					break;
				case 0x07:
					this->comboBox_P16Regen->Text = "Regenerative 7";
					break;
				default:
					this->comboBox_P16Regen->Text = "Regenerative Off";
					break;
				}
			}
			// if "Regenerative Off" selected, it would have been caught in the above if statement
			else
				UpdateRegenMode();
			break;
		default:
			// set Regen mode to off on default
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode& 0xF8);
		}
	}
}
/**************************************************************************
 * LEVDisplay::UpdateRegenMode
 * 
 * Sets the Current Regen Level, Page 16, Byte 3, bits 0-2
 *
 * returns: N/A
 *
 **************************************************************************/
void LEVDisplay::UpdateRegenMode()
{
	if (this->comboBox_P16Regen->Text->Equals("Regenerative Off"))		// 000 = Regen Off
	{
		LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xF8);	// clear the bits 0-2 only   
	}
	else if (this->comboBox_P16Regen->Text->Equals("Regenerative 1"))	// 001 = Regen 1
	{
		LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xF8);	// clear the bits 0-2 only 
		LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x01);	// set bit 0 high
	}
	else if (this->comboBox_P16Regen->Text->Equals("Regenerative 2"))	// 010 = Regen 2
	{
		LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xF8);	// clear the bits 0-2 only 
		LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x02);	// set bit 1 high
	}
	else if (this->comboBox_P16Regen->Text->Equals("Regenerative 3"))	// 011 = Regen 3
	{
		LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xF8);	// clear the bits 0-2 only 
		LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x03);	// set bits 0,1 high
	}
	else if (this->comboBox_P16Regen->Text->Equals("Regenerative 4"))	// 100 = Regen 4
	{
		LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xF8);	// clear the bits 0-2 only 
		LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x04);	// set bit 2 high
	}
	else if (this->comboBox_P16Regen->Text->Equals("Regenerative 5"))	// 101 = Regen 5
	{
		LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xF8);	// clear the bits 0-2 only 
		LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x05);	// set bits 0,2 high
	}
	else if (this->comboBox_P16Regen->Text->Equals("Regenerative 6"))	// 110 = Regen 6
	{
		LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xF8);	// clear the bits 0-2 only 
		LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x06);	// set bits 2,1 high
	}
	else if (this->comboBox_P16Regen->Text->Equals("Regenerative 7"))	// 111 = Regen 7
	{
		LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xF8);	// clear the bits 0-2 only 
		LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x07);	// set bits 0,1,2 high
	}
}


/**************************************************************************
 * LEVDisplay::comboBox_P16FrontGear_SelectedIndexChanged
 * 
 * Updates the user selected Front Gear Setting for P16
 *
 * returns: N/A
 *
 **************************************************************************/
void LEVDisplay::comboBox_P16FrontGear_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
{
	UpdateFrontGearState();
}


/**************************************************************************
 * LEVDisplay::UpdateFrontGearState
 * 
 * Sets the P16 Front Gear Setting, Page 16, Byte 4-5, bits 4-5
 *
 * returns: N/A
 *
 **************************************************************************/
void LEVDisplay::UpdateFrontGearState()
{
	if (this->comboBox_P16FrontGear->Text->Equals("none"))			// 00 = no gear
	{
		LEVData->usP16DisplayCommand = (LEVData->usP16DisplayCommand & 0xFFCF);	// clear the bits 4-5 only   
	}
	else if (this->comboBox_P16FrontGear->Text->Equals("1"))			// 01 = gear 1
	{
		LEVData->usP16DisplayCommand = (LEVData->usP16DisplayCommand & 0xFFCF);	// clear the bits 4-5 only   
		LEVData->usP16DisplayCommand = (LEVData->usP16DisplayCommand | 0x10);	// set bit 4 high
	}
	else if (this->comboBox_P16FrontGear->Text->Equals("2"))		// 10 = gear 2
	{
		LEVData->usP16DisplayCommand = (LEVData->usP16DisplayCommand & 0xFFCF);	// clear the bits 4-5 only   
		LEVData->usP16DisplayCommand = (LEVData->usP16DisplayCommand | 0x20);	// set bit 5 high
	}
	else if (this->comboBox_P16FrontGear->Text->Equals("3"))		// 11 = gear 3
	{
		LEVData->usP16DisplayCommand = (LEVData->usP16DisplayCommand & 0xFFCF);	// clear the bits 4-5 only   
		LEVData->usP16DisplayCommand = (LEVData->usP16DisplayCommand | 0x30);	// set bits 4,5 high
	}
}


/**************************************************************************
 * LEVDisplay::comboBox_P16RearGear_SelectedIndexChanged
 * 
 * Updates the user selected Rear Gear Setting for P16
 *
 * returns: N/A
 *
 **************************************************************************/
void LEVDisplay::comboBox_P16RearGear_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
{
	UpdateRearGear();
}


/**************************************************************************
 * LEVDisplay::UpdateRearGear
 * 
 * Sets the Front Rear Setting, Page 16, Byte 4-5, bits 6-9
 *
 * returns: N/A
 *
 **************************************************************************/
void LEVDisplay::UpdateRearGear()
{
	if (this->comboBox_P16RearGear->Text->Equals("none"))		// 0000 = no gear
	{
		LEVData->usP16DisplayCommand = (LEVData->usP16DisplayCommand & 0xFC3F);	// clear the bits 6-9 only   
	}
	else if (this->comboBox_P16RearGear->Text->Equals("1"))		// 0001 = gear 1
	{
		LEVData->usP16DisplayCommand = (LEVData->usP16DisplayCommand & 0xFC3F);	// clear the bits 6-9 only 
		LEVData->usP16DisplayCommand = (LEVData->usP16DisplayCommand | (1 << 6));	// shift value up 6 to get into bits 6-9
	}
	else if (this->comboBox_P16RearGear->Text->Equals("2"))		// 0010 = gear 2
	{
		LEVData->usP16DisplayCommand = (LEVData->usP16DisplayCommand & 0xFC3F);	// clear the bits 6-9 only 
		LEVData->usP16DisplayCommand = (LEVData->usP16DisplayCommand | (2 << 6));	// shift value up 6 to get into bits 6-9
	}
	else if (this->comboBox_P16RearGear->Text->Equals("3"))		// 0011 = gear 3
	{
		LEVData->usP16DisplayCommand = (LEVData->usP16DisplayCommand & 0xFC3F);	// clear the bits 6-9 only 
		LEVData->usP16DisplayCommand = (LEVData->usP16DisplayCommand | (3 << 6));	// shift value up 6 to get into bits 6-9
	}
	else if (this->comboBox_P16RearGear->Text->Equals("4"))		// 0100 = gear 4
	{
		LEVData->usP16DisplayCommand = (LEVData->usP16DisplayCommand & 0xFC3F);	// clear the bits 6-9 only 
		LEVData->usP16DisplayCommand = (LEVData->usP16DisplayCommand | (4 << 6));	// shift value up 6 to get into bits 6-9
	}
	else if (this->comboBox_P16RearGear->Text->Equals("5"))		// 0101 = gear 5
	{
		LEVData->usP16DisplayCommand = (LEVData->usP16DisplayCommand & 0xFC3F);	// clear the bits 6-9 only 
		LEVData->usP16DisplayCommand = (LEVData->usP16DisplayCommand | (5 << 6));	// shift value up 6 to get into bits 6-9
	}
	else if (this->comboBox_P16RearGear->Text->Equals("6"))		// 0110 = gear 6
	{
		LEVData->usP16DisplayCommand = (LEVData->usP16DisplayCommand & 0xFC3F);	// clear the bits 6-9 only 
		LEVData->usP16DisplayCommand = (LEVData->usP16DisplayCommand | (6 << 6));	// shift value up 6 to get into bits 6-9
	}
	else if (this->comboBox_P16RearGear->Text->Equals("7"))		// 0111 = gear 7
	{
		LEVData->usP16DisplayCommand = (LEVData->usP16DisplayCommand & 0xFC3F);	// clear the bits 6-9 only 
		LEVData->usP16DisplayCommand = (LEVData->usP16DisplayCommand | (7 << 6));	// shift value up 6 to get into bits 6-9
	}
	else if (this->comboBox_P16RearGear->Text->Equals("8"))		// 1000 = gear 8
	{
		LEVData->usP16DisplayCommand = (LEVData->usP16DisplayCommand & 0xFC3F);	// clear the bits 6-9 only 
		LEVData->usP16DisplayCommand = (LEVData->usP16DisplayCommand | (8 << 6));	// shift value up 6 to get into bits 6-9
	}
	else if (this->comboBox_P16RearGear->Text->Equals("9"))		// 1001 = gear 9 
	{
		LEVData->usP16DisplayCommand = (LEVData->usP16DisplayCommand & 0xFC3F);	// clear the bits 6-9 only 
		LEVData->usP16DisplayCommand = (LEVData->usP16DisplayCommand | (9 << 6));	// shift value up 6 to get into bits 6-9
	}
	else if (this->comboBox_P16RearGear->Text->Equals("10"))		// 1010 = gear 10
	{
		LEVData->usP16DisplayCommand = (LEVData->usP16DisplayCommand & 0xFC3F);	// clear the bits 6-9 only 
		LEVData->usP16DisplayCommand = (LEVData->usP16DisplayCommand | (10 << 6));	// shift value up 6 to get into bits 6-9
	}
	else if (this->comboBox_P16RearGear->Text->Equals("11"))		// 1011 = gear 11
	{
		LEVData->usP16DisplayCommand = (LEVData->usP16DisplayCommand & 0xFC3F);	// clear the bits 6-9 only 
		LEVData->usP16DisplayCommand = (LEVData->usP16DisplayCommand | (11 << 6));	// shift value up 6 to get into bits 6-9
	}
	else if (this->comboBox_P16RearGear->Text->Equals("12"))		// 1100 = gear 12
	{
		LEVData->usP16DisplayCommand = (LEVData->usP16DisplayCommand & 0xFC3F);	// clear the bits 6-9 only 
		LEVData->usP16DisplayCommand = (LEVData->usP16DisplayCommand | (12 << 6));	// shift value up 6 to get into bits 6-9
	}
	else if (this->comboBox_P16RearGear->Text->Equals("13"))		// 1101 = gear 13
	{
		LEVData->usP16DisplayCommand = (LEVData->usP16DisplayCommand & 0xFC3F);	// clear the bits 6-9 only 
		LEVData->usP16DisplayCommand = (LEVData->usP16DisplayCommand | (13 << 6));	// shift value up 6 to get into bits 6-9
	}
	else if (this->comboBox_P16RearGear->Text->Equals("14"))		// 1110 = gear 14
	{
		LEVData->usP16DisplayCommand = (LEVData->usP16DisplayCommand & 0xFC3F);	// clear the bits 6-9 only 
		LEVData->usP16DisplayCommand = (LEVData->usP16DisplayCommand | (14 << 6));	// shift value up 6 to get into bits 6-9
	}
	else if (this->comboBox_P16RearGear->Text->Equals("15"))		// 1111 = gear 15
	{
		LEVData->usP16DisplayCommand = (LEVData->usP16DisplayCommand & 0xFC3F);	// clear the bits 6-9 only 
		LEVData->usP16DisplayCommand = (LEVData->usP16DisplayCommand | (15 << 6));	// shift value up 6 to get into bits 6-9
	}
}


/**************************************************************************
 * LEVDisplay::checkBox_P16HighBeamOn_CheckedChanged
 * 
 * Toggles the bit in the command bitfield for high/low beam, page 16, byte 4-5, bit 2
 *
 * returns: N/A
 *
 **************************************************************************/
void LEVDisplay::checkBox_P16HighBeamOn_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	if (this->checkBox_P16HighBeamOn->Checked == TRUE)
		LEVData->usP16DisplayCommand |= 0x04;            // set bit 2 to 1   		
	else
		LEVData->usP16DisplayCommand &= (UCHAR)~0x04;    // set bit 2 to 0
}


/**************************************************************************
 * LEVDisplay::checkBox_P16LightOn_CheckedChanged
 * 
 * Toggles the bit in the system state bitfield for Light on/off, page 16, byte 4-5, bit 3
 *
 * returns: N/A
 *
 **************************************************************************/
void LEVDisplay::checkBox_P16LightOn_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	if (this->checkBox_P16LightOn->Checked == TRUE)
		LEVData->usP16DisplayCommand |= 0x08;            // set bit 3 to 1   		
	else
		LEVData->usP16DisplayCommand &= (UCHAR)~0x08;    // set bit 3 to 0
}


/**************************************************************************
 * LEVDisplay::checkBox_P16LeftSignalOn_CheckedChanged
 * 
 * Toggles the bit in the system state bitfield for left signal on/off, page 16, byte 4-5, bit 1
 *
 * returns: N/A
 *
 **************************************************************************/
void LEVDisplay::checkBox_P16LeftSignalOn_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	if (this->checkBox_P16LeftSignalOn->Checked == TRUE)
		LEVData->usP16DisplayCommand |= 0x02;            // set bit 1 to 1   		
	else
		LEVData->usP16DisplayCommand &= (UCHAR)~0x02;    // set bit 1 to 0
}


/**************************************************************************
 * LEVDisplay::checkBox_P16RightSignalOn_CheckedChanged
 * 
 * Toggles the bit in the system state bitfield for right signal on/off, page 16, byte 4-5, bit 0
 *
 * returns: N/A
 *
 **************************************************************************/
void LEVDisplay::checkBox_P16RightSignalOn_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	if (this->checkBox_P16RightSignalOn->Checked == TRUE)
		LEVData->usP16DisplayCommand |= 0x01;            // set bit 0 to 1   		
	else
		LEVData->usP16DisplayCommand &= (UCHAR)~0x01;    // set bit 0 to 0
}


/**************************************************************************
 * LEVDisplay::numericUpDown_P16ManID_ValueChanged
 * 
 * Page 16, Byte 6-7, Selects the manufacturer ID to use. UI locks the range
 *
 * returns: N/A
 *
 **************************************************************************/
void LEVDisplay::numericUpDown_P16ManID_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	LEVData->usP16ManID = (System::Convert::ToUInt16(this->numericUpDown_P16ManID->Value)); 
}


/**************************************************************************
 * LEVDisplay::numericUpDown_P16Circum_ValueChanged
 * 
 * Page 16, Byte 2-3.5, Selects the wheel circumference to use. UI locks the range
 *
 * returns: N/A
 *
 **************************************************************************/
void LEVDisplay::numericUpDown_P16Circum_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	LEVData->usP16WheelCircum = (System::Convert::ToUInt16(this->numericUpDown_P16Circum->Value));
	LEVData->usP16WheelCircum |= 0xF000;
}


/**************************************************************************
 * LEVDisplay::button_P16Send_Click
 * 
 * Sends Page 16 as an acknowledged message (only done on user request)
 *
 * returns: N/A
 *
 **************************************************************************/
void LEVDisplay::button_P16Send_Click(System::Object^  sender, System::EventArgs^  e)
{
	this->label_AckMsgStatus->Text = "";
	SendRequestMsg(LEVData->PAGE16_DISPLAY_DATA);	
}


/**************************************************************************
 * LEVDisplay::SendRequestMsg
 * 
 * Sends an acknowledged Page Requset message according to the message code 
 *
 * ucMsgCode_: message ID of the message to send
 * Supported messages:  
 * - Page 16 Display Data
 * 
 * returns: N/A
 *
 **************************************************************************/
void LEVDisplay::SendRequestMsg(UCHAR ucMsgCode_) 
{
	UCHAR aucAckBuffer[8] = {0,0,0,0,0,0,0,0};


	//SendRequestMsg(LEVData->PAGE70_REQUEST);	

	switch(ucMsgCode_)
	{
		case LEVData->PAGE16_DISPLAY_DATA:	  // Page 16 
			LEVData->EncodeData(LEVData->PAGE16_DISPLAY_DATA, aucAckBuffer);
			break;
		case LEVData->PAGE70_REQUEST:		  // Page 70
			LEVData->EncodeData(LEVData->PAGE70_REQUEST, aucAckBuffer);
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
 * LEVDisplay::HandleRetransmit
 * 
 * Retransmits request message, up to the maximum retransmission number 
 * 
 * returns: TRUE if message was retransmitted
 *          FALSE if maximum number of retransmissions was reached
 *
 **************************************************************************/
BOOL LEVDisplay::HandleRetransmit()
{
	BOOL bSuccess = TRUE;

	if(ucMsgExpectingAck)	// Message still expecting an ack
	{
		if(ucAckRetryCount++ < LEVData->MAX_RETRIES)
			SendRequestMsg(ucMsgExpectingAck);
		else
			bSuccess = FALSE;
	}
	
	return bSuccess;
}


/**************************************************************************
 * LEVDisplay::UpdateDisplayAckStatus
 * 
 * Updates display to show if acknowledged request messages were
 * transmitted successfully
 *
 * returns:  N/A
 *
 **************************************************************************/
void LEVDisplay::UpdateDisplayAckStatus(UCHAR ucStatus_)
{
	switch(ucStatus_)
	{
		case LEVData->ACK_SUCCESS:
			this->label_AckMsgStatus->ForeColor = System::Drawing::Color::Green;
			this->label_AckMsgStatus->Text = "SENT";
			break;
		case LEVData->ACK_RETRY:
			this->label_AckMsgStatus->ForeColor = System::Drawing::Color::Blue;
			this->label_AckMsgStatus->Text = "RTRY";
			break;
		case LEVData->ACK_FAIL:
			this->label_AckMsgStatus->ForeColor = System::Drawing::Color::Red;
			this->label_AckMsgStatus->Text = "FAIL";
			break;
		default:
			break;
	}
}


/**************************************************************************
 * LEVDisplay::numericUpDown_numericUpDown_RqTxTimes
 * 
 * Request Pages, Selects the number of times to Request the sensor to broadcast requested page for
 *
 * returns: N/A
 *
 **************************************************************************/
void LEVDisplay::numericUpDown_RqTxTimes_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	LEVData->ucP70TxTimes = System::Convert::ToByte(this->numericUpDown_RqTxTimes->Value);
}

/**************************************************************************
 * LEVDisplay::comboBox_PageToRequest_SelectedIndexChanged
 * 
 * Request Pages, Selects the page to Request
 *
 * returns: N/A
 *
 **************************************************************************/
void LEVDisplay::comboBox_PageToRequest_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
{
	if (this->comboBox_PageToRequest->Text->Equals("Data Page 1"))		
	{
		LEVData->ucP70RequestedPage = LEVData->PAGE1_SPEED_SYSTEM1;	
	}
	else if (this->comboBox_PageToRequest->Text->Equals("Data Page 2"))		
	{
		LEVData->ucP70RequestedPage = LEVData->PAGE2_SPEED_DISTANCE;
	}
	else if (this->comboBox_PageToRequest->Text->Equals("Data Page 3"))		
	{
		LEVData->ucP70RequestedPage = LEVData->PAGE3_SPEED_SYSTEM2;	
	}
	else if (this->comboBox_PageToRequest->Text->Equals("Data Page 4"))		
	{
		LEVData->ucP70RequestedPage = LEVData->PAGE4_BATTERY;	
	}
	else if (this->comboBox_PageToRequest->Text->Equals("Data Page 5"))		
	{
		LEVData->ucP70RequestedPage = LEVData->PAGE5_CAPABILITES;	
	}
}


/**************************************************************************
 * LEVDisplay::button_SendRequestClick
 * 
 * Sends a Request Page 70 as an acknowledged message (only done on user request)
 *
 * returns: N/A
 *
 **************************************************************************/
void LEVDisplay::button_SendRequest_Click(System::Object^  sender, System::EventArgs^  e)
{
	this->label_AckMsgStatus->Text = "";			//*** fix this, wrong place
	SendRequestMsg(LEVData->PAGE70_REQUEST);	
}









/**************************************************************************
 * LEVDisplay::TransmitCommonPage
 * 
 * Handles sending bcast common pages on the reverse channel
 *
 * returns: N/A
 *
 **************************************************************************/
void LEVDisplay::TransmitCommonPage() 
{
	static UCHAR ucReqCommonPatternCount = 0;

	UCHAR aucTxBuffer[8];
	UCHAR aucReqCommonTxPattern[4] = {commonDataDisplay->PAGE80, commonDataDisplay->PAGE81};
	memset(aucTxBuffer, 8, 0x00);
	
	// send common data pages 80 and 81
	commonDataDisplay->Encode(aucReqCommonTxPattern[ucReqCommonPatternCount], aucTxBuffer);
	ucReqCommonPatternCount++;

	if (ucReqCommonPatternCount >= 2)
		ucReqCommonPatternCount = 0;

	requestBcastMsg(aucTxBuffer);	// Send as broadcast message
}


/**************************************************************************
 * LEVDisplay::button_CommonUpdate_Click
 * 
 * Adjusts the global pages 80 and 81 via the GUI
 *
 * returns: N/A
 *
 **************************************************************************/
void LEVDisplay::button_CommonUpdate_Click(System::Object^  sender, System::EventArgs^  e)
{
	// required common pages
	//commonData->ulSerialNum = System::Convert::ToUInt32(this->textBox_SerialNum->Text);
	//commonData->ucSwVersion = System::Convert::ToByte(this->textBox_SwVersion->Text);
	//commonData->ucHwVersion = System::Convert::ToByte(this->textBox_HwVersion->Text);
	//commonData->usMfgID = System::Convert::ToUInt16(this->textBox_MfgID->Text);
	//commonData->usModelNum = System::Convert::ToUInt16(this->textBox_ModelNum->Text);

	label_ErrorCommon->Visible = false;
	label_ErrorCommon->Text = "Error: ";
	// Convert and catch failed conversions
	try{
		commonDataDisplay->usMfgID = System::Convert::ToUInt16(this->textBox_MfgID->Text);
	}
	catch(...){
		label_ErrorCommon->Text = System::String::Concat(label_ErrorCommon->Text, " MFID");
		label_ErrorCommon->Visible = true;
	}
	try{
		if(this->checkBox_NoSerial->Checked)
			commonDataDisplay->ulSerialNum = 0xFFFFFFFF;
		else
			commonDataDisplay->ulSerialNum = System::Convert::ToUInt32(this->textBox_SerialNum->Text);
	}
	catch(...){
		label_ErrorCommon->Text = System::String::Concat(label_ErrorCommon->Text, " Ser#");
		label_ErrorCommon->Visible = true;
	}
	try{
		commonDataDisplay->ucHwVersion = System::Convert::ToByte(this->textBox_HwVersion->Text);
	}
	catch(...){
		label_ErrorCommon->Text = System::String::Concat(label_ErrorCommon->Text, " HWVr");
		label_ErrorCommon->Visible = true;
	}
	try{
		commonDataDisplay->ucSwVersion = System::Convert::ToByte(this->textBox_SwVersion->Text);
	}
	catch(...){
		label_ErrorCommon->Text = System::String::Concat(label_ErrorCommon->Text, " SWVr");
		label_ErrorCommon->Visible = true;
	}
	try{
		commonDataDisplay->usModelNum = System::Convert::ToUInt16(this->textBox_ModelNum->Text);
	}
	catch(...){
		label_ErrorCommon->Text = System::String::Concat(label_ErrorCommon->Text, " Mdl#");
		label_ErrorCommon->Visible = true;
	}
}





/**************************************************************************
 * LEVDisplay::numericUpDown_DisplayAssistModes_ValueChanged
 * 
 * Handles change in value for supported assist modes
 *
 * returns: N/A
 *
 **************************************************************************/
void LEVDisplay::numericUpDown_DisplayAssistModes_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	ucDisplaySupportedAssist = (System::Convert::ToByte(this->numericUpDown_DisplayAssistModes->Value));
	UpdateP16Assist();
}
/**************************************************************************
 * LEVDisplay::UpdateP16Assist
 * 
 * Handles Assist Mode mapping when the number of supported assist modes is changed
 *
 * returns: N/A
 *
 **************************************************************************/
void LEVDisplay::UpdateP16Assist()
{
	// get the number of assist modes currently in Page 16
	UCHAR AssistMode;
	if(this->comboBox_P16Assist->Text->Equals("Assist Off"))
		AssistMode = 0;
	else if(this->comboBox_P16Assist->Text->Equals("Assist 1"))
		AssistMode = 1;
	else if(this->comboBox_P16Assist->Text->Equals("Assist 2"))
		AssistMode = 2;
	else if(this->comboBox_P16Assist->Text->Equals("Assist 3"))
		AssistMode = 3;
	else if(this->comboBox_P16Assist->Text->Equals("Assist 4"))
		AssistMode = 4;
	else if(this->comboBox_P16Assist->Text->Equals("Assist 5"))
		AssistMode = 5;
	else if(this->comboBox_P16Assist->Text->Equals("Assist 6"))
		AssistMode = 6;
	else
		AssistMode = 7;

	if(ucDisplaySupportedAssist == 1)	// Display supports 1 AssistMode
	{
		switch(AssistMode)
		{
		case 0:	// "Assist Off" is common to all mapping, nothing needs to be done 
			break;
		default:	// if the current Assist mode is not Off, change the page 16 data to Assist mode 7 and change the combobox index
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xC7);
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x38);
			this->comboBox_P16Assist->Text = "Assist 7";
			break;
		}
	}
	else if(ucDisplaySupportedAssist == 2)	// Display supports 2 AssistModes
	{
		switch(AssistMode)
		{
		case 1:
		case 2:	
		case 3:	// if Assist mode is 1, 2, or 3 ensure page 16 data is Assist mode 3 and change combobox index
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xC7);
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x18);
			this->comboBox_P16Assist->Text = "Assist 3";
			break;
		case 4:
		case 5:
		case 6:	
		case 7:	// if Assist mode is 4, 5, 6, or 7 ensure page 16 data is Assist mode 7 and change combobox index
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xC7);
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x38);
			this->comboBox_P16Assist->Text = "Assist 7";
			break;
		default:	// "Assist Off" is common to all mapping, nothing needs to be done
			break;
		}
	}
	else if(ucDisplaySupportedAssist == 3)	// Display supports 3 Assist Modes
	{
		switch(AssistMode)
		{
		case 1:	
		case 2:	// if Assist mode is 1, or 2, ensure page 16 data is Assist mode 2 and change combobox index
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xC7);
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x10);
			this->comboBox_P16Assist->Text = "Assist 2";
			break;
		case 3:	
		case 4:	// if the current Assist mode is 3 or 4, ensure page 16 data is Assist mode 4 and change combobox index
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xC7);
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x20);
			this->comboBox_P16Assist->Text = "Assist 4";
			break;
		case 5:
		case 6:	
		case 7:	// if the current Assist mode is 5, 6, or 7, ensure page 16 data is Assist mode 7 and change combobox index
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xC7);
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x38);
			this->comboBox_P16Assist->Text = "Assist 7";
			break;
		default:	// "Assist Off" is common to all mapping, nothing needs to be done
			break;
		}
	}
	else if(ucDisplaySupportedAssist == 4)	// Display supports 4 Assist Modes
	{
		switch(AssistMode)
		{
		case 1:	// if the current Assist mode is 1, ensure page 16 data is Assist mode 1 
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xC7);
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x08);
			break;
		case 2:	
		case 3:	// if the current Assist mode is 2 or 3, ensure page 16 data is Assist mode 3 and change combobox index
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xC7);
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x18);
			this->comboBox_P16Assist->Text = "Assist 3";
			break;
		case 4:	
		case 5:	// if the current Assist mode is 4 or 5, ensure page 16 data is Assist mode 5 and change combobox index
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xC7);
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x28);
			this->comboBox_P16Assist->Text = "Assist 5";
			break;
		case 6:	
		case 7:	// if the current Assist mode is 6 or 7, ensure page 16 data is Assist mode 7 and change combobox index
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xC7);
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x38);
			this->comboBox_P16Assist->Text = "Assist 7";
			break;
		default:	// "Assist Off" is common to all mapping, nothing needs to be done
			break;
		}
	}
	else if(ucDisplaySupportedAssist== 5)	// Display supports 5 Assist Modes
	{
		switch(AssistMode)
		{
		case 1:	// if the current Assist mode is 1, ensure page 16 data is Assist mode 1 
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xC7);
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x08);
			break;
		case 2:	// if the current Assist mode is 2, ensure page 16 data is Assist mode 2 
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xC7);
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x10);
			break;
		case 3:	// if the current Assist mode is 3, ensure page 16 data is Assist mode 3 
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xC7);
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x18);
			break;
		case 4:
		case 5:	// if the current Assist mode is 4 or 5, ensure page 16 data is Assist mode 5 and change combobox index
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xC7);
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x28);
			this->comboBox_P16Assist->Text = "Assist 5";
			break;
		case 6:
		case 7:	// if the current Assist mode is 6 or 7, ensure page 16 data is Assist mode 7 and change combobox index
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xC7);
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x38);
			this->comboBox_P16Assist->Text = "Assist 7";
			break;
		default:	// "Assist Off" is common to call mapping, nothing needs to be done
			break;
		}
	}
	else if(ucDisplaySupportedAssist == 6)	// Display supports 6 Assist Modes
	{
		switch(AssistMode)
		{
		case 1:	// if the currentAssistmode is 1, ensure page 16 data is Assist mode 1 
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xC7);
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x08);
			break;
		case 2:	// if the current Assist mode is 2, ensure page 16 data is Assist mode 2 
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xC7);
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x10);
			break;
		case 3:	// if the current Assist mode is 3, ensure page 16 data is Assist mode 3 
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xC7);
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x18);
			break;
		case 4: // if the current Assist mode is 4, ensure page 16 data is Assist mode 4 
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xC7);
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x20);
			break;
		case 5: // if the current Assist mode is 5, ensure page 16 data is Assist mode 5 
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xC7);
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x28);
			break;
		case 6:
		case 7:	// if the current Assist mode is 6 or 7, ensure page 16 data is Assist mode 7 and change combobox index
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xC7);
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x38);
			this->comboBox_P16Assist->Text = "Assist 7";
			break;
		default:	// "Assist Off" is common to all mapping, nothing needs to be done
			break;
		}
	}
}
/**************************************************************************
 * LEVDisplay::numericUpDown_DisplayRegenModes_ValueChanged
 * 
 * Handles change in value for supported Regen modes
 *
 * returns: N/A
 *
 **************************************************************************/
void LEVDisplay::numericUpDown_DisplayRegenModes_ValueChanged(System::Object ^sender, System::EventArgs ^e)
{
	ucDisplaySupportedRegen = (System::Convert::ToByte(this->numericUpDown_DisplayRegenModes->Value));
	UpdateP16Regen();
}
/**************************************************************************
 * LEVDisplay::UpdateP16Regen
 * 
 * Handles Regen Mode mapping when the number of supported regen modes is changed
 *
 * returns: N/A
 *
 **************************************************************************/
void LEVDisplay::UpdateP16Regen()
{
	// get the number of regen modes currently in Page 16
	UCHAR RegenMode;
	if(this->comboBox_P16Regen->Text->Equals("Regenerative Off"))
		RegenMode = 0;
	else if(this->comboBox_P16Regen->Text->Equals("Regenerative 1"))
		RegenMode = 1;
	else if(this->comboBox_P16Regen->Text->Equals("Regenerative 2"))
		RegenMode = 2;
	else if(this->comboBox_P16Regen->Text->Equals("Regenerative 3"))
		RegenMode = 3;
	else if(this->comboBox_P16Regen->Text->Equals("Regenerative 4"))
		RegenMode = 4;
	else if(this->comboBox_P16Regen->Text->Equals("Regenerative 5"))
		RegenMode = 5;
	else if(this->comboBox_P16Regen->Text->Equals("Regenerative 6"))
		RegenMode = 6;
	else
		RegenMode = 7;

	if(ucDisplaySupportedRegen == 0)
	{	// if the Display doesn't support Regen Modes, set Page 16 data to regen mode 0 and update the combobox
		LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xF8);
		this->comboBox_P16Regen->Text = "Regenerative Off";
	}
	else if(ucDisplaySupportedRegen == 1)	// Display supports 1 Regen Mode
	{
		switch(RegenMode)
		{
		case 0:	// "Regenerative Off" is common to all mapping, nothing needs to be done 
			break;
		default:	// if the current regen mode is not Off, change the page 16 data to regen mode 7 and change the combobox index
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xF8);
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x07);
			this->comboBox_P16Regen->Text = "Regenerative 7";
			break;
		}
	}
	else if(ucDisplaySupportedRegen == 2)	// Display supports 2 Regen Modes
	{
		switch(RegenMode)
		{
		case 1:
		case 2:	
		case 3:	// if regen mode is 1, 2, or 3 ensure page 16 data is regen mode 3 and change combobox index
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xF8);
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x03);
			this->comboBox_P16Regen->Text = "Regenerative 3";
			break;
		case 4:
		case 5:
		case 6:	
		case 7:	// if regen mode is 4, 5, 6, or 7 ensure page 16 data is regen mode 7 and change combobox index
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xF8);
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x07);
			this->comboBox_P16Regen->Text = "Regenerative 7";
			break;
		default:	// "Regenerative Off" is common to all mapping, nothing needs to be done
			break;
		}
	}
	else if(ucDisplaySupportedRegen == 3)	// Display supports 3 Regen Modes
	{
		switch(RegenMode)
		{
		case 1:	
		case 2:	// if regen mode is 1, or 2, ensure page 16 data is regen mode 2 and change combobox index
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xF8);
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x02);
			this->comboBox_P16Regen->Text = "Regenerative 2";
			break;
		case 3:	
		case 4:	// if the current regen mode is 3 or 4, ensure page 16 data is regen mode 4 and change combobox index
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xF8);
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x04);
			this->comboBox_P16Regen->Text = "Regenerative 4";
			break;
		case 5:
		case 6:	
		case 7:	// if the current regen mode is 5, 6, or 7, ensure page 16 data is regen mode 7 and change combobox index
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xF8);
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x07);
			this->comboBox_P16Regen->Text = "Regenerative 7";
			break;
		default:	// "Regenerative Off" is common to all mapping, nothing needs to be done
			break;
		}
	}
	else if(ucDisplaySupportedRegen == 4)	// Display supports 4 Regen Modes
	{
		switch(RegenMode)
		{
		case 1:	// if the current regen mode is 1, ensure page 16 data is regen mode 1 
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xF8);
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x01);
			break;
		case 2:	
		case 3:	// if the current regen mode is 2 or 3, ensure page 16 data is regen mode 3 and change combobox index
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xF8);
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x03);
			this->comboBox_P16Regen->Text = "Regenerative 3";
			break;
		case 4:	
		case 5:	// if the current regen mode is 4 or 5, ensure page 16 data is regen mode 5 and change combobox index
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xF8);
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x05);
			this->comboBox_P16Regen->Text = "Regenerative 5";
			break;
		case 6:	
		case 7:	// if the current regen mode is 6 or 7, ensure page 16 data is regen mode 7 and change combobox index
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xF8);
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x07);
			this->comboBox_P16Regen->Text = "Regenerative 7";
			break;
		default:	// "Regenerative Off" is common to all mapping, nothing needs to be done
			break;
		}
	}
	else if(ucDisplaySupportedRegen == 5)	// Display supports 5 Regen Modes
	{
		switch(RegenMode)
		{
		case 1:	// if the current regen mode is 1, ensure page 16 data is regen mode 1 
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xF8);
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x01);
			break;
		case 2:	// if the current regen mode is 2, ensure page 16 data is regen mode 2 
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xF8);
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x02);
			break;
		case 3:	// if the current regen mode is 3, ensure page 16 data is regen mode 3 
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xF8);
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x03);
			break;
		case 4:
		case 5:	// if the current regen mode is 4 or 5, ensure page 16 data is regen mode 5 and change combobox index
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xF8);
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x05);
			this->comboBox_P16Regen->Text = "Regenerative 5";
			break;
		case 6:
		case 7:	// if the current regen mode is 6 or 7, ensure page 16 data is regen mode 7 and change combobox index
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xF8);
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x07);
			this->comboBox_P16Regen->Text = "Regenerative 7";
			break;
		default:	// "Regenerative Off" is common to call mapping, nothing needs to be done
			break;
		}
	}
	else if(ucDisplaySupportedRegen == 6)	// Display supports 6 Regen Modes
	{
		switch(RegenMode)
		{
		case 1:	// if the current regen mode is 1, ensure page 16 data is regen mode 1 
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xF8);
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x01);
			break;
		case 2:	// if the current regen mode is 2, ensure page 16 data is regen mode 2 
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xF8);
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x02);
			break;
		case 3:	// if the current regen mode is 3, ensure page 16 data is regen mode 3 
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xF8);
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x03);
			break;
		case 4: // if the current regen mode is 4, ensure page 16 data is regen mode 4 
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xF8);
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x04);
			break;
		case 5: // if the current regen mode is 5, ensure page 16 data is regen mode 5 
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xF8);
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x05);
			break;
		case 6:
		case 7:	// if the current regen mode is 6 or 7, ensure page 16 data is regen mode 7 and change combobox index
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode & 0xF8);
			LEVData->ucP16TravelMode = (LEVData->ucP16TravelMode | 0x07);
			this->comboBox_P16Regen->Text = "Regenerative 7";
			break;
		default:	// "Regenerative Off" is common to all mapping, nothing needs to be done
			break;
		}
	}
}
/**************************************************************************
 * LEVDisplay::checkBox_NoSerial_CheckedChanged
 * 
 * Handles CheckedChanged event of the checkBox
 *
 * returns: N/A
 *
 **************************************************************************/
void LEVDisplay::checkBox_NoSerial_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
{
	if(this->checkBox_NoSerial->Checked)
		this->textBox_SerialNum->Enabled = false;
	else
		this->textBox_SerialNum->Enabled = true;
}