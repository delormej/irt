/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/


#pragma once
#include "StdAfx.h"
#include "ISimBase.h"
#include "antplus_lev.h"				// Device specific class
#include "antplus_common.h"				// Include common pages (example usage on SDM and Bike Power)
#include "types.h"
#include "antdefines.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

public ref class LEVDisplay : public System::Windows::Forms::Form, public ISimBase{
public:
	// The constructor may include the following parameters:
	// System::Timers::Timer^ channelTimer:  Timer handle, if using a timer (not required in most cases, since getTimerInterval gets called after onTimerTock on each tock)
	// dRequestAckMsg^ channelAckMsg:  Handle to delegate function to send acknowledged messages (only needed if the device needs to send acknowledged messages)
	// These parameters may or may not be included in the constructor, as needed
		LEVDisplay(dRequestAckMsg^ channelAckMsg, dRequestBcastMsg^ channelBcastMsg){
			InitializeComponent();
			LEVData = gcnew LEV();
			commonDataDisplay = gcnew CommonData();
			commonDataSensor = gcnew CommonData();
			requestAckMsg = channelAckMsg;		
			requestBcastMsg = channelBcastMsg;
			InitializeSim();
		}

		~LEVDisplay(){
			this->panel_Display->Controls->Clear();
			this->panel_Settings->Controls->Clear();
			delete this->panel_Display;
			delete this->panel_Settings;
			//clean up floating resources with the garbage collector
			GC::Collect(2);

			if (components)
			{
				delete components;
			}
		}

// Methods required by the interface class
public:
	virtual void onTimerTock(USHORT eventTime){}	// Do nothing
	virtual void ANT_eventNotification(UCHAR ucEventCode_, UCHAR* pucEventBuffer_);	 // Channel event state machine
	virtual UCHAR getDeviceType(){return LEV::DEVICE_TYPE;}  	
	virtual UCHAR getTransmissionType(){return 0;} 	
	virtual USHORT getTransmitPeriod(){return LEV::MSG_PERIOD;}	
	virtual DOUBLE getTimerInterval(){return 1000;} // Set to any value, as receiver does not use the timer
	virtual System::Windows::Forms::Panel^ getSimSettingsPanel(){return this->panel_Settings;}
	virtual System::Windows::Forms::Panel^ getSimTranslatedDisplay(){return this->panel_Display;}

// Optional methods (not required by interface class)
private:
	void InitializeSim();				   // Initialize simulation
	void UpdateDisplay(UCHAR ucPageNum);   // Updates the display after an ANT message is received
	void UpdateAssistMode();			   // Updates the Assist Mode
	void UpdateRegenMode();				   // Updates the Regen Mode
	void UpdateFrontGearState();		   // Updates the front gear
	void UpdateRearGear();
	void SendRequestMsg(UCHAR ucMsgCode_); // Sends an Acknowledged message
	BOOL HandleRetransmit();
	void UpdateDisplayAckStatus(UCHAR ucStatus_);
	void TransmitCommonPage();			   // Handles sending bcast common pages on the reverse channel
	void MapAssistMode();
	void MapRegenMode();
	void CheckAssistMapping();
	void CheckRegenMapping();
	void UpdateP16Assist();
	void UpdateP16Regen();

	// Add other GUI specific functions here
	System::Void checkBox_P16TravelModeNotSupported_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void comboBox_P16Assist_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void comboBox_P16Regen_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void comboBox_P16FrontGear_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void comboBox_P16RearGear_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_P16HighBeamOn_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_P16LightOn_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_P16LeftSignalOn_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_P16RightSignalOn_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_P16ManID_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void button_P16Send_Click(System::Object^  sender, System::EventArgs^  e);
	System::Void button_CommonUpdate_Click(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_P16Circum_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_RqTxTimes_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void comboBox_PageToRequest_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void button_SendRequest_Click(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_DisplayAssistModes_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_DisplayRegenModes_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_NoSerial_CheckedChanged(System::Object^  sender, System::EventArgs^  e);


private:
	dRequestAckMsg^ requestAckMsg;		// Delegate handle (ACK messages)
	dRequestBcastMsg^ requestBcastMsg;  // Delgate handle (broadcast message)
	LEV^ LEVData;
	CommonData^ commonDataDisplay;		// stores the common data that is Tx (from display)
	CommonData^ commonDataSensor;		// stores the common data that is Rx (from sensor)

	// Calculated values for display
	double dbDispAcumDist;			// Cumulative distance (km)
	BOOL bPage16Enabled;			// Holds whether Page 16 is sent by the display or not

	UCHAR ucRxMsgCount;				// Counts the number of messages received so that we can send back a common page on the 20th message
	UCHAR ucMsgExpectingAck;		// Holds the message that is pending acknowledgement
	UCHAR ucAckRetryCount;			// Holds the number of times an ack has been retried	

	// Display Travel Modes
	UCHAR ucDisplaySupportedAssist;	 // number of supported assist levels for the display
	UCHAR ucDisplaySupportedRegen;	 // number of supported regen levels for the display

private: System::Windows::Forms::Panel^  panel_Settings;
private: System::Windows::Forms::Panel^  panel_Display;
private: System::Windows::Forms::Label^  label_P16FrontGear;
private: System::Windows::Forms::Label^  label_P16RearGear;
private: System::Windows::Forms::Label^  label_RxStatusBox;
private: System::Windows::Forms::Label^  label_StatusAssistVal;
private: System::Windows::Forms::Label^  label_StatusAssist;
private: System::Windows::Forms::Label^  label_StatusSpeedVal;
private: System::Windows::Forms::Label^  label_StatusSpeed;
private: System::Windows::Forms::Label^  label_StatusOdoVal;
private: System::Windows::Forms::Label^  label_StatusOdometer;
private: System::Windows::Forms::TabControl^  tabControl_LEVDisplay;
private: System::Windows::Forms::TabPage^  tabPage_P1;
private: System::Windows::Forms::Label^  label_P1RightValue;
private: System::Windows::Forms::Label^  label_P1LeftValue;
private: System::Windows::Forms::Label^  label_P1BeamValue;
private: System::Windows::Forms::Label^  label_P1LightValue;
private: System::Windows::Forms::Label^  label_P1SpeedValue;
private: System::Windows::Forms::Label^  label_P1Right;
private: System::Windows::Forms::Label^  label_P1Left;
private: System::Windows::Forms::Label^  label_P1Beam;
private: System::Windows::Forms::Label^  label_P1Light;
private: System::Windows::Forms::Label^  label_P1Speed;
private: System::Windows::Forms::Label^  label_P1RegenLevelVal;
private: System::Windows::Forms::Label^  label_P1AssistLevelVal;
private: System::Windows::Forms::Label^  label_P1RegenLevel;
private: System::Windows::Forms::Label^  label_P1AssistLevel;
private: System::Windows::Forms::TabPage^  tabPage_P2;
private: System::Windows::Forms::Label^  label_P2SpeedVal;
private: System::Windows::Forms::Label^  label_P2Speed;
private: System::Windows::Forms::Label^  label_P2OdometerVal;
private: System::Windows::Forms::Label^  label_P2Odometer;
private: System::Windows::Forms::TabPage^  tabPage_P3;
private: System::Windows::Forms::TabPage^  tabPage_Page45;
private: System::Windows::Forms::TabPage^  tabPage_P16;
private: System::Windows::Forms::TabControl^  tabControl_P16;
private: System::Windows::Forms::TabPage^  tabPage_P16TravelMode;
private: System::Windows::Forms::Label^  label_AckMsgStatus;
private: System::Windows::Forms::Button^  button_P16Send;
private: System::Windows::Forms::TabPage^  tabPage_P16Display;
private: System::Windows::Forms::CheckBox^  checkBox_P16TravelModeNotSupported;
private: System::Windows::Forms::Label^  label_P16CurrentRegen;
private: System::Windows::Forms::Label^  label_P16CurrentAssist;
private: System::Windows::Forms::ComboBox^  comboBox_P16Assist;
private: System::Windows::Forms::ComboBox^  comboBox_P16Regen;
private: System::Windows::Forms::TabPage^  tabPage_P16Command;
private: System::Windows::Forms::GroupBox^  groupBox_P16CurrentGear;
private: System::Windows::Forms::Label^  label_P16Front;
private: System::Windows::Forms::ComboBox^  comboBox_P16FrontGear;
private: System::Windows::Forms::Label^  label_P16Rear;
private: System::Windows::Forms::ComboBox^  comboBox_P16RearGear;
private: System::Windows::Forms::CheckBox^  checkBox_P16RightSignalOn;
private: System::Windows::Forms::CheckBox^  checkBox_P16LeftSignalOn;
private: System::Windows::Forms::CheckBox^  checkBox_P16HighBeamOn;
private: System::Windows::Forms::CheckBox^  checkBox_P16LightOn;
private: System::Windows::Forms::TabPage^  tabPage_P16ManID;
private: System::Windows::Forms::Label^  label_P16ManID;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_P16ManID;
private: System::Windows::Forms::TabPage^  tabPage_Common1;
private: System::Windows::Forms::Label^  label_Common1;
private: System::Windows::Forms::GroupBox^  groupBox_P81Rx;
private: System::Windows::Forms::Label^  label_Glb_SoftwareVer;
private: System::Windows::Forms::Label^  label_Glb_SoftwareVerDisplay;
private: System::Windows::Forms::Label^  label_Glb_SerialNumDisplay;
private: System::Windows::Forms::Label^  label_Glb_SerialNum;
private: System::Windows::Forms::GroupBox^  groupBox_P80Rx;
private: System::Windows::Forms::Label^  label_Glb_HardwareVer;
private: System::Windows::Forms::Label^  label_Glb_ModelNum;
private: System::Windows::Forms::Label^  label_Glb_ManfIDDisplay;
private: System::Windows::Forms::Label^  label_Glb_ModelNumDisplay;
private: System::Windows::Forms::Label^  label_Glb_HardwareVerDisplay;
private: System::Windows::Forms::Label^  label_Glb_ManfID;
private: System::Windows::Forms::TabPage^  tabPage_Common2;
private: System::Windows::Forms::Label^  label_SoftwareVer;
private: System::Windows::Forms::Label^  label_HardwareVer;
private: System::Windows::Forms::Button^  button_CommonUpdate;
private: System::Windows::Forms::Label^  label_ErrorCommon;
private: System::Windows::Forms::Label^  label_ManfID;
private: System::Windows::Forms::Label^  label_SerialNum;
private: System::Windows::Forms::TextBox^  textBox_SerialNum;
private: System::Windows::Forms::TextBox^  textBox_SwVersion;
private: System::Windows::Forms::TextBox^  textBox_HwVersion;
private: System::Windows::Forms::TextBox^  textBox_ModelNum;
private: System::Windows::Forms::TextBox^  textBox_MfgID;
private: System::Windows::Forms::Label^  label_ModelNum;
private: System::Windows::Forms::Label^  label_P1RearGearVal;
private: System::Windows::Forms::Label^  label_P1FrontGearVal;
private: System::Windows::Forms::Label^  label_P1ManAutoVal;
private: System::Windows::Forms::Label^  label_P1GearExistVal;
private: System::Windows::Forms::Label^  label_P1RearGear;
private: System::Windows::Forms::Label^  label_P1FrontGear;
private: System::Windows::Forms::Label^  label_P1ManAuto;
private: System::Windows::Forms::Label^  label_P1GearExist;
private: System::Windows::Forms::Label^  label_P3RearGearVal;
private: System::Windows::Forms::Label^  label_P3FrontGearVal;
private: System::Windows::Forms::Label^  label_P3ManAutoVal;
private: System::Windows::Forms::Label^  label_P3GearsExistVal;
private: System::Windows::Forms::Label^  label_P3RearGear;
private: System::Windows::Forms::Label^  label_P3FrontGear;
private: System::Windows::Forms::Label^  label_P3ManAuto;
private: System::Windows::Forms::Label^  label_P3GearsExist;
private: System::Windows::Forms::Label^  label_P3RegenVal;
private: System::Windows::Forms::Label^  label_P3RightVal;
private: System::Windows::Forms::Label^  label_P3AssistLvlVal;
private: System::Windows::Forms::Label^  label_P3LeftVal;
private: System::Windows::Forms::Label^  label_P3Regen;
private: System::Windows::Forms::Label^  label_P3BeamVal;
private: System::Windows::Forms::Label^  label_P3AssistLvl;
private: System::Windows::Forms::Label^  label_P3LightVal;
private: System::Windows::Forms::Label^  label_P3SpeedVal;
private: System::Windows::Forms::Label^  label_P3Right;
private: System::Windows::Forms::Label^  label_P3Left;
private: System::Windows::Forms::Label^  label_P3Beam;
private: System::Windows::Forms::Label^  label_P3Light;
private: System::Windows::Forms::Label^  label_P3Speed;
private: System::Windows::Forms::Label^  label_P1ThrottleVal;
private: System::Windows::Forms::Label^  label_P1Throttle;
private: System::Windows::Forms::Label^  label_P1MotorAlertVal;
private: System::Windows::Forms::Label^  label_P1BattAlertVal;
private: System::Windows::Forms::Label^  label_P1MotorTempVal;
private: System::Windows::Forms::Label^  label_P1MotorAlert;
private: System::Windows::Forms::Label^  label_P1BattAlert;
private: System::Windows::Forms::Label^  label_P1MotorTemp;
private: System::Windows::Forms::Label^  label_P1BattTempVal;
private: System::Windows::Forms::Label^  label_P1BattTemp;
private: System::Windows::Forms::Label^  label_P1ErrorValue;
private: System::Windows::Forms::Label^  label_P1Error;
private: System::Windows::Forms::Label^  label_P2RangeVal;
private: System::Windows::Forms::Label^  label_P2Range;
private: System::Windows::Forms::Label^  label_P3PercentAssist;
private: System::Windows::Forms::Label^  label_P3BattSOC;
private: System::Windows::Forms::Label^  label_P3ThrottleVal;
private: System::Windows::Forms::Label^  label_P3Throttle;
private: System::Windows::Forms::Label^  label_P3PercentAssistVal;
private: System::Windows::Forms::Label^  label_P3BattEmpty;
private: System::Windows::Forms::Label^  label_P3BattEmptyVal;
private: System::Windows::Forms::Label^  label_P3ChargeVal;
private: System::Windows::Forms::TabControl^  tabControl_Page45;
private: System::Windows::Forms::TabPage^  tabPage_Page4;
private: System::Windows::Forms::Label^  label_P4VoltageVal;
private: System::Windows::Forms::Label^  label_P4Voltage;
private: System::Windows::Forms::Label^  label_P4DistanceOnCharge;
private: System::Windows::Forms::Label^  label_P4FuelConsumption;
private: System::Windows::Forms::Label^  label_P4ChargeCycle;
private: System::Windows::Forms::TabPage^  tabPage_Page5;
private: System::Windows::Forms::Label^  label_P5NumRegenVal;
private: System::Windows::Forms::Label^  label_P5NumAssistVal;
private: System::Windows::Forms::Label^  label_P5NumRegen;
private: System::Windows::Forms::Label^  label_P5NumAssist;
private: System::Windows::Forms::Label^  label_P5Circum;
private: System::Windows::Forms::Label^  label_P4DistanceChargeVal;
private: System::Windows::Forms::Label^  label_P4FuelVal;
private: System::Windows::Forms::Label^  label_P4ChargeCycleVal;
private: System::Windows::Forms::Label^  label_P5CircumVal;
private: System::Windows::Forms::TabPage^  tabPage_P16Wheel;
private: System::Windows::Forms::Label^  label_P16Circum;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_P16Circum;
private: System::Windows::Forms::Label^  label_RqTxTimes;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_RqTxTimes;
private: System::Windows::Forms::TabPage^  tabPage_RqPage;
private: System::Windows::Forms::ComboBox^  comboBox_PageToRequest;
private: System::Windows::Forms::GroupBox^  groupBox_Request;
private: System::Windows::Forms::Button^  button_SendRequest;
private: System::Windows::Forms::Label^  label_PageToRequest;
private: System::Windows::Forms::Label^  label_AckStatus;
private: System::Windows::Forms::TabPage^  tabPage_TravelModes;
private: System::Windows::Forms::Label^  label_SupportedTravelModes;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_DisplayRegenModes;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_DisplayAssistModes;
private: System::Windows::Forms::Label^  label_RegenModesSupported;
private: System::Windows::Forms::Label^  label_AssistModesSupported;
private: System::Windows::Forms::Label^  label_P34FuelValue;
private: System::Windows::Forms::Label^  labelP34_Fuel;
private: System::Windows::Forms::CheckBox^  checkBox_NoSerial;
private: System::Windows::Forms::Label^  label_Common2;


		/// <summary>
		/// Required designer variables
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->panel_Settings = (gcnew System::Windows::Forms::Panel());
			this->tabControl_LEVDisplay = (gcnew System::Windows::Forms::TabControl());
			this->tabPage_P1 = (gcnew System::Windows::Forms::TabPage());
			this->label_P1ThrottleVal = (gcnew System::Windows::Forms::Label());
			this->label_P1Throttle = (gcnew System::Windows::Forms::Label());
			this->label_P1RearGearVal = (gcnew System::Windows::Forms::Label());
			this->label_P1FrontGearVal = (gcnew System::Windows::Forms::Label());
			this->label_P1ManAutoVal = (gcnew System::Windows::Forms::Label());
			this->label_P1GearExistVal = (gcnew System::Windows::Forms::Label());
			this->label_P1RearGear = (gcnew System::Windows::Forms::Label());
			this->label_P1FrontGear = (gcnew System::Windows::Forms::Label());
			this->label_P1ManAuto = (gcnew System::Windows::Forms::Label());
			this->label_P1GearExist = (gcnew System::Windows::Forms::Label());
			this->label_P1MotorAlertVal = (gcnew System::Windows::Forms::Label());
			this->label_P1BattAlertVal = (gcnew System::Windows::Forms::Label());
			this->label_P1MotorTempVal = (gcnew System::Windows::Forms::Label());
			this->label_P1MotorAlert = (gcnew System::Windows::Forms::Label());
			this->label_P1BattAlert = (gcnew System::Windows::Forms::Label());
			this->label_P1MotorTemp = (gcnew System::Windows::Forms::Label());
			this->label_P1BattTempVal = (gcnew System::Windows::Forms::Label());
			this->label_P1BattTemp = (gcnew System::Windows::Forms::Label());
			this->label_P1RegenLevelVal = (gcnew System::Windows::Forms::Label());
			this->label_P1RightValue = (gcnew System::Windows::Forms::Label());
			this->label_P1AssistLevelVal = (gcnew System::Windows::Forms::Label());
			this->label_P1LeftValue = (gcnew System::Windows::Forms::Label());
			this->label_P1RegenLevel = (gcnew System::Windows::Forms::Label());
			this->label_P1BeamValue = (gcnew System::Windows::Forms::Label());
			this->label_P1AssistLevel = (gcnew System::Windows::Forms::Label());
			this->label_P1LightValue = (gcnew System::Windows::Forms::Label());
			this->label_P1SpeedValue = (gcnew System::Windows::Forms::Label());
			this->label_P1ErrorValue = (gcnew System::Windows::Forms::Label());
			this->label_P1Right = (gcnew System::Windows::Forms::Label());
			this->label_P1Left = (gcnew System::Windows::Forms::Label());
			this->label_P1Beam = (gcnew System::Windows::Forms::Label());
			this->label_P1Light = (gcnew System::Windows::Forms::Label());
			this->label_P1Speed = (gcnew System::Windows::Forms::Label());
			this->label_P1Error = (gcnew System::Windows::Forms::Label());
			this->tabPage_P2 = (gcnew System::Windows::Forms::TabPage());
			this->label_P34FuelValue = (gcnew System::Windows::Forms::Label());
			this->labelP34_Fuel = (gcnew System::Windows::Forms::Label());
			this->label_P2RangeVal = (gcnew System::Windows::Forms::Label());
			this->label_P2Range = (gcnew System::Windows::Forms::Label());
			this->label_P2SpeedVal = (gcnew System::Windows::Forms::Label());
			this->label_P2Speed = (gcnew System::Windows::Forms::Label());
			this->label_P2OdometerVal = (gcnew System::Windows::Forms::Label());
			this->label_P2Odometer = (gcnew System::Windows::Forms::Label());
			this->tabPage_P3 = (gcnew System::Windows::Forms::TabPage());
			this->label_P3PercentAssistVal = (gcnew System::Windows::Forms::Label());
			this->label_P3BattEmpty = (gcnew System::Windows::Forms::Label());
			this->label_P3BattEmptyVal = (gcnew System::Windows::Forms::Label());
			this->label_P3ChargeVal = (gcnew System::Windows::Forms::Label());
			this->label_P3ThrottleVal = (gcnew System::Windows::Forms::Label());
			this->label_P3Throttle = (gcnew System::Windows::Forms::Label());
			this->label_P3PercentAssist = (gcnew System::Windows::Forms::Label());
			this->label_P3BattSOC = (gcnew System::Windows::Forms::Label());
			this->label_P3RearGearVal = (gcnew System::Windows::Forms::Label());
			this->label_P3FrontGearVal = (gcnew System::Windows::Forms::Label());
			this->label_P3ManAutoVal = (gcnew System::Windows::Forms::Label());
			this->label_P3GearsExistVal = (gcnew System::Windows::Forms::Label());
			this->label_P3RearGear = (gcnew System::Windows::Forms::Label());
			this->label_P3FrontGear = (gcnew System::Windows::Forms::Label());
			this->label_P3ManAuto = (gcnew System::Windows::Forms::Label());
			this->label_P3GearsExist = (gcnew System::Windows::Forms::Label());
			this->label_P3RegenVal = (gcnew System::Windows::Forms::Label());
			this->label_P3RightVal = (gcnew System::Windows::Forms::Label());
			this->label_P3AssistLvlVal = (gcnew System::Windows::Forms::Label());
			this->label_P3LeftVal = (gcnew System::Windows::Forms::Label());
			this->label_P3Regen = (gcnew System::Windows::Forms::Label());
			this->label_P3BeamVal = (gcnew System::Windows::Forms::Label());
			this->label_P3AssistLvl = (gcnew System::Windows::Forms::Label());
			this->label_P3LightVal = (gcnew System::Windows::Forms::Label());
			this->label_P3SpeedVal = (gcnew System::Windows::Forms::Label());
			this->label_P3Right = (gcnew System::Windows::Forms::Label());
			this->label_P3Left = (gcnew System::Windows::Forms::Label());
			this->label_P3Beam = (gcnew System::Windows::Forms::Label());
			this->label_P3Light = (gcnew System::Windows::Forms::Label());
			this->label_P3Speed = (gcnew System::Windows::Forms::Label());
			this->tabPage_Page45 = (gcnew System::Windows::Forms::TabPage());
			this->tabControl_Page45 = (gcnew System::Windows::Forms::TabControl());
			this->tabPage_Page4 = (gcnew System::Windows::Forms::TabPage());
			this->label_P4DistanceChargeVal = (gcnew System::Windows::Forms::Label());
			this->label_P4FuelVal = (gcnew System::Windows::Forms::Label());
			this->label_P4ChargeCycleVal = (gcnew System::Windows::Forms::Label());
			this->label_P4VoltageVal = (gcnew System::Windows::Forms::Label());
			this->label_P4Voltage = (gcnew System::Windows::Forms::Label());
			this->label_P4DistanceOnCharge = (gcnew System::Windows::Forms::Label());
			this->label_P4FuelConsumption = (gcnew System::Windows::Forms::Label());
			this->label_P4ChargeCycle = (gcnew System::Windows::Forms::Label());
			this->tabPage_Page5 = (gcnew System::Windows::Forms::TabPage());
			this->label_P5CircumVal = (gcnew System::Windows::Forms::Label());
			this->label_P5NumRegenVal = (gcnew System::Windows::Forms::Label());
			this->label_P5NumAssistVal = (gcnew System::Windows::Forms::Label());
			this->label_P5NumRegen = (gcnew System::Windows::Forms::Label());
			this->label_P5NumAssist = (gcnew System::Windows::Forms::Label());
			this->label_P5Circum = (gcnew System::Windows::Forms::Label());
			this->tabPage_RqPage = (gcnew System::Windows::Forms::TabPage());
			this->button_SendRequest = (gcnew System::Windows::Forms::Button());
			this->label_PageToRequest = (gcnew System::Windows::Forms::Label());
			this->groupBox_Request = (gcnew System::Windows::Forms::GroupBox());
			this->label_RqTxTimes = (gcnew System::Windows::Forms::Label());
			this->numericUpDown_RqTxTimes = (gcnew System::Windows::Forms::NumericUpDown());
			this->comboBox_PageToRequest = (gcnew System::Windows::Forms::ComboBox());
			this->tabPage_P16 = (gcnew System::Windows::Forms::TabPage());
			this->tabControl_P16 = (gcnew System::Windows::Forms::TabControl());
			this->tabPage_P16TravelMode = (gcnew System::Windows::Forms::TabPage());
			this->button_P16Send = (gcnew System::Windows::Forms::Button());
			this->tabPage_P16Display = (gcnew System::Windows::Forms::TabPage());
			this->checkBox_P16TravelModeNotSupported = (gcnew System::Windows::Forms::CheckBox());
			this->label_P16CurrentRegen = (gcnew System::Windows::Forms::Label());
			this->label_P16CurrentAssist = (gcnew System::Windows::Forms::Label());
			this->comboBox_P16Assist = (gcnew System::Windows::Forms::ComboBox());
			this->comboBox_P16Regen = (gcnew System::Windows::Forms::ComboBox());
			this->tabPage_P16Command = (gcnew System::Windows::Forms::TabPage());
			this->groupBox_P16CurrentGear = (gcnew System::Windows::Forms::GroupBox());
			this->label_P16Front = (gcnew System::Windows::Forms::Label());
			this->comboBox_P16FrontGear = (gcnew System::Windows::Forms::ComboBox());
			this->label_P16Rear = (gcnew System::Windows::Forms::Label());
			this->comboBox_P16RearGear = (gcnew System::Windows::Forms::ComboBox());
			this->checkBox_P16RightSignalOn = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox_P16LeftSignalOn = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox_P16HighBeamOn = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox_P16LightOn = (gcnew System::Windows::Forms::CheckBox());
			this->tabPage_P16ManID = (gcnew System::Windows::Forms::TabPage());
			this->label_P16ManID = (gcnew System::Windows::Forms::Label());
			this->numericUpDown_P16ManID = (gcnew System::Windows::Forms::NumericUpDown());
			this->tabPage_P16Wheel = (gcnew System::Windows::Forms::TabPage());
			this->numericUpDown_P16Circum = (gcnew System::Windows::Forms::NumericUpDown());
			this->label_P16Circum = (gcnew System::Windows::Forms::Label());
			this->tabPage_Common1 = (gcnew System::Windows::Forms::TabPage());
			this->label_Common1 = (gcnew System::Windows::Forms::Label());
			this->groupBox_P81Rx = (gcnew System::Windows::Forms::GroupBox());
			this->label_Glb_SoftwareVer = (gcnew System::Windows::Forms::Label());
			this->label_Glb_SoftwareVerDisplay = (gcnew System::Windows::Forms::Label());
			this->label_Glb_SerialNumDisplay = (gcnew System::Windows::Forms::Label());
			this->label_Glb_SerialNum = (gcnew System::Windows::Forms::Label());
			this->groupBox_P80Rx = (gcnew System::Windows::Forms::GroupBox());
			this->label_Glb_HardwareVer = (gcnew System::Windows::Forms::Label());
			this->label_Glb_ModelNum = (gcnew System::Windows::Forms::Label());
			this->label_Glb_ManfIDDisplay = (gcnew System::Windows::Forms::Label());
			this->label_Glb_ModelNumDisplay = (gcnew System::Windows::Forms::Label());
			this->label_Glb_HardwareVerDisplay = (gcnew System::Windows::Forms::Label());
			this->label_Glb_ManfID = (gcnew System::Windows::Forms::Label());
			this->tabPage_Common2 = (gcnew System::Windows::Forms::TabPage());
			this->label_SoftwareVer = (gcnew System::Windows::Forms::Label());
			this->label_HardwareVer = (gcnew System::Windows::Forms::Label());
			this->button_CommonUpdate = (gcnew System::Windows::Forms::Button());
			this->label_ErrorCommon = (gcnew System::Windows::Forms::Label());
			this->label_ManfID = (gcnew System::Windows::Forms::Label());
			this->label_SerialNum = (gcnew System::Windows::Forms::Label());
			this->textBox_SerialNum = (gcnew System::Windows::Forms::TextBox());
			this->textBox_SwVersion = (gcnew System::Windows::Forms::TextBox());
			this->textBox_HwVersion = (gcnew System::Windows::Forms::TextBox());
			this->textBox_ModelNum = (gcnew System::Windows::Forms::TextBox());
			this->textBox_MfgID = (gcnew System::Windows::Forms::TextBox());
			this->label_ModelNum = (gcnew System::Windows::Forms::Label());
			this->tabPage_TravelModes = (gcnew System::Windows::Forms::TabPage());
			this->numericUpDown_DisplayRegenModes = (gcnew System::Windows::Forms::NumericUpDown());
			this->numericUpDown_DisplayAssistModes = (gcnew System::Windows::Forms::NumericUpDown());
			this->label_RegenModesSupported = (gcnew System::Windows::Forms::Label());
			this->label_AssistModesSupported = (gcnew System::Windows::Forms::Label());
			this->label_SupportedTravelModes = (gcnew System::Windows::Forms::Label());
			this->label_AckMsgStatus = (gcnew System::Windows::Forms::Label());
			this->panel_Display = (gcnew System::Windows::Forms::Panel());
			this->label_AckStatus = (gcnew System::Windows::Forms::Label());
			this->label_StatusAssistVal = (gcnew System::Windows::Forms::Label());
			this->label_StatusAssist = (gcnew System::Windows::Forms::Label());
			this->label_StatusSpeedVal = (gcnew System::Windows::Forms::Label());
			this->label_StatusSpeed = (gcnew System::Windows::Forms::Label());
			this->label_StatusOdoVal = (gcnew System::Windows::Forms::Label());
			this->label_StatusOdometer = (gcnew System::Windows::Forms::Label());
			this->label_RxStatusBox = (gcnew System::Windows::Forms::Label());
			this->label_P16FrontGear = (gcnew System::Windows::Forms::Label());
			this->label_P16RearGear = (gcnew System::Windows::Forms::Label());
			this->label_Common2 = (gcnew System::Windows::Forms::Label());
			this->checkBox_NoSerial = (gcnew System::Windows::Forms::CheckBox());
			this->panel_Settings->SuspendLayout();
			this->tabControl_LEVDisplay->SuspendLayout();
			this->tabPage_P1->SuspendLayout();
			this->tabPage_P2->SuspendLayout();
			this->tabPage_P3->SuspendLayout();
			this->tabPage_Page45->SuspendLayout();
			this->tabControl_Page45->SuspendLayout();
			this->tabPage_Page4->SuspendLayout();
			this->tabPage_Page5->SuspendLayout();
			this->tabPage_RqPage->SuspendLayout();
			this->groupBox_Request->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_RqTxTimes))->BeginInit();
			this->tabPage_P16->SuspendLayout();
			this->tabControl_P16->SuspendLayout();
			this->tabPage_P16TravelMode->SuspendLayout();
			this->tabPage_P16Display->SuspendLayout();
			this->tabPage_P16Command->SuspendLayout();
			this->groupBox_P16CurrentGear->SuspendLayout();
			this->tabPage_P16ManID->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_P16ManID))->BeginInit();
			this->tabPage_P16Wheel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_P16Circum))->BeginInit();
			this->tabPage_Common1->SuspendLayout();
			this->groupBox_P81Rx->SuspendLayout();
			this->groupBox_P80Rx->SuspendLayout();
			this->tabPage_Common2->SuspendLayout();
			this->tabPage_TravelModes->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_DisplayRegenModes))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_DisplayAssistModes))->BeginInit();
			this->panel_Display->SuspendLayout();
			this->SuspendLayout();
			// 
			// panel_Settings
			// 
			this->panel_Settings->Controls->Add(this->tabControl_LEVDisplay);
			this->panel_Settings->Location = System::Drawing::Point(322, 50);
			this->panel_Settings->Name = L"panel_Settings";
			this->panel_Settings->Size = System::Drawing::Size(400, 140);
			this->panel_Settings->TabIndex = 0;
			// 
			// tabControl_LEVDisplay
			// 
			this->tabControl_LEVDisplay->Controls->Add(this->tabPage_P1);
			this->tabControl_LEVDisplay->Controls->Add(this->tabPage_P2);
			this->tabControl_LEVDisplay->Controls->Add(this->tabPage_P3);
			this->tabControl_LEVDisplay->Controls->Add(this->tabPage_Page45);
			this->tabControl_LEVDisplay->Controls->Add(this->tabPage_P16);
			this->tabControl_LEVDisplay->Controls->Add(this->tabPage_Common1);
			this->tabControl_LEVDisplay->Controls->Add(this->tabPage_Common2);
			this->tabControl_LEVDisplay->Controls->Add(this->tabPage_TravelModes);
			this->tabControl_LEVDisplay->Location = System::Drawing::Point(0, 3);
			this->tabControl_LEVDisplay->Name = L"tabControl_LEVDisplay";
			this->tabControl_LEVDisplay->SelectedIndex = 0;
			this->tabControl_LEVDisplay->Size = System::Drawing::Size(397, 137);
			this->tabControl_LEVDisplay->TabIndex = 0;
			// 
			// tabPage_P1
			// 
			this->tabPage_P1->Controls->Add(this->label_P1ThrottleVal);
			this->tabPage_P1->Controls->Add(this->label_P1Throttle);
			this->tabPage_P1->Controls->Add(this->label_P1RearGearVal);
			this->tabPage_P1->Controls->Add(this->label_P1FrontGearVal);
			this->tabPage_P1->Controls->Add(this->label_P1ManAutoVal);
			this->tabPage_P1->Controls->Add(this->label_P1GearExistVal);
			this->tabPage_P1->Controls->Add(this->label_P1RearGear);
			this->tabPage_P1->Controls->Add(this->label_P1FrontGear);
			this->tabPage_P1->Controls->Add(this->label_P1ManAuto);
			this->tabPage_P1->Controls->Add(this->label_P1GearExist);
			this->tabPage_P1->Controls->Add(this->label_P1MotorAlertVal);
			this->tabPage_P1->Controls->Add(this->label_P1BattAlertVal);
			this->tabPage_P1->Controls->Add(this->label_P1MotorTempVal);
			this->tabPage_P1->Controls->Add(this->label_P1MotorAlert);
			this->tabPage_P1->Controls->Add(this->label_P1BattAlert);
			this->tabPage_P1->Controls->Add(this->label_P1MotorTemp);
			this->tabPage_P1->Controls->Add(this->label_P1BattTempVal);
			this->tabPage_P1->Controls->Add(this->label_P1BattTemp);
			this->tabPage_P1->Controls->Add(this->label_P1RegenLevelVal);
			this->tabPage_P1->Controls->Add(this->label_P1RightValue);
			this->tabPage_P1->Controls->Add(this->label_P1AssistLevelVal);
			this->tabPage_P1->Controls->Add(this->label_P1LeftValue);
			this->tabPage_P1->Controls->Add(this->label_P1RegenLevel);
			this->tabPage_P1->Controls->Add(this->label_P1BeamValue);
			this->tabPage_P1->Controls->Add(this->label_P1AssistLevel);
			this->tabPage_P1->Controls->Add(this->label_P1LightValue);
			this->tabPage_P1->Controls->Add(this->label_P1SpeedValue);
			this->tabPage_P1->Controls->Add(this->label_P1ErrorValue);
			this->tabPage_P1->Controls->Add(this->label_P1Right);
			this->tabPage_P1->Controls->Add(this->label_P1Left);
			this->tabPage_P1->Controls->Add(this->label_P1Beam);
			this->tabPage_P1->Controls->Add(this->label_P1Light);
			this->tabPage_P1->Controls->Add(this->label_P1Speed);
			this->tabPage_P1->Controls->Add(this->label_P1Error);
			this->tabPage_P1->Location = System::Drawing::Point(4, 22);
			this->tabPage_P1->Name = L"tabPage_P1";
			this->tabPage_P1->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_P1->Size = System::Drawing::Size(389, 111);
			this->tabPage_P1->TabIndex = 0;
			this->tabPage_P1->Text = L"Page 1";
			this->tabPage_P1->UseVisualStyleBackColor = true;
			// 
			// label_P1ThrottleVal
			// 
			this->label_P1ThrottleVal->AutoSize = true;
			this->label_P1ThrottleVal->Location = System::Drawing::Point(72, 23);
			this->label_P1ThrottleVal->Name = L"label_P1ThrottleVal";
			this->label_P1ThrottleVal->Size = System::Drawing::Size(16, 13);
			this->label_P1ThrottleVal->TabIndex = 60;
			this->label_P1ThrottleVal->Text = L"---";
			// 
			// label_P1Throttle
			// 
			this->label_P1Throttle->AutoSize = true;
			this->label_P1Throttle->Location = System::Drawing::Point(2, 23);
			this->label_P1Throttle->Name = L"label_P1Throttle";
			this->label_P1Throttle->Size = System::Drawing::Size(46, 13);
			this->label_P1Throttle->TabIndex = 59;
			this->label_P1Throttle->Text = L"Throttle:";
			// 
			// label_P1RearGearVal
			// 
			this->label_P1RearGearVal->AutoSize = true;
			this->label_P1RearGearVal->Location = System::Drawing::Point(349, 57);
			this->label_P1RearGearVal->Name = L"label_P1RearGearVal";
			this->label_P1RearGearVal->Size = System::Drawing::Size(16, 13);
			this->label_P1RearGearVal->TabIndex = 58;
			this->label_P1RearGearVal->Text = L"---";
			// 
			// label_P1FrontGearVal
			// 
			this->label_P1FrontGearVal->AutoSize = true;
			this->label_P1FrontGearVal->Location = System::Drawing::Point(349, 40);
			this->label_P1FrontGearVal->Name = L"label_P1FrontGearVal";
			this->label_P1FrontGearVal->Size = System::Drawing::Size(16, 13);
			this->label_P1FrontGearVal->TabIndex = 57;
			this->label_P1FrontGearVal->Text = L"---";
			// 
			// label_P1ManAutoVal
			// 
			this->label_P1ManAutoVal->AutoSize = true;
			this->label_P1ManAutoVal->Location = System::Drawing::Point(361, 23);
			this->label_P1ManAutoVal->Name = L"label_P1ManAutoVal";
			this->label_P1ManAutoVal->Size = System::Drawing::Size(16, 13);
			this->label_P1ManAutoVal->TabIndex = 56;
			this->label_P1ManAutoVal->Text = L"---";
			// 
			// label_P1GearExistVal
			// 
			this->label_P1GearExistVal->AutoSize = true;
			this->label_P1GearExistVal->Location = System::Drawing::Point(361, 6);
			this->label_P1GearExistVal->Name = L"label_P1GearExistVal";
			this->label_P1GearExistVal->Size = System::Drawing::Size(16, 13);
			this->label_P1GearExistVal->TabIndex = 55;
			this->label_P1GearExistVal->Text = L"---";
			// 
			// label_P1RearGear
			// 
			this->label_P1RearGear->AutoSize = true;
			this->label_P1RearGear->Location = System::Drawing::Point(283, 57);
			this->label_P1RearGear->Name = L"label_P1RearGear";
			this->label_P1RearGear->Size = System::Drawing::Size(59, 13);
			this->label_P1RearGear->TabIndex = 54;
			this->label_P1RearGear->Text = L"Rear Gear:";
			// 
			// label_P1FrontGear
			// 
			this->label_P1FrontGear->AutoSize = true;
			this->label_P1FrontGear->Location = System::Drawing::Point(283, 40);
			this->label_P1FrontGear->Name = L"label_P1FrontGear";
			this->label_P1FrontGear->Size = System::Drawing::Size(60, 13);
			this->label_P1FrontGear->TabIndex = 53;
			this->label_P1FrontGear->Text = L"Front Gear:";
			// 
			// label_P1ManAuto
			// 
			this->label_P1ManAuto->AutoSize = true;
			this->label_P1ManAuto->Location = System::Drawing::Point(283, 23);
			this->label_P1ManAuto->Name = L"label_P1ManAuto";
			this->label_P1ManAuto->Size = System::Drawing::Size(72, 13);
			this->label_P1ManAuto->TabIndex = 52;
			this->label_P1ManAuto->Text = L"Manual/Auto:";
			// 
			// label_P1GearExist
			// 
			this->label_P1GearExist->AutoSize = true;
			this->label_P1GearExist->Location = System::Drawing::Point(283, 6);
			this->label_P1GearExist->Name = L"label_P1GearExist";
			this->label_P1GearExist->Size = System::Drawing::Size(63, 13);
			this->label_P1GearExist->TabIndex = 51;
			this->label_P1GearExist->Text = L"Gears Exist:";
			// 
			// label_P1MotorAlertVal
			// 
			this->label_P1MotorAlertVal->AutoSize = true;
			this->label_P1MotorAlertVal->Location = System::Drawing::Point(212, 57);
			this->label_P1MotorAlertVal->Name = L"label_P1MotorAlertVal";
			this->label_P1MotorAlertVal->Size = System::Drawing::Size(16, 13);
			this->label_P1MotorAlertVal->TabIndex = 50;
			this->label_P1MotorAlertVal->Text = L"---";
			// 
			// label_P1BattAlertVal
			// 
			this->label_P1BattAlertVal->AutoSize = true;
			this->label_P1BattAlertVal->Location = System::Drawing::Point(212, 40);
			this->label_P1BattAlertVal->Name = L"label_P1BattAlertVal";
			this->label_P1BattAlertVal->Size = System::Drawing::Size(16, 13);
			this->label_P1BattAlertVal->TabIndex = 49;
			this->label_P1BattAlertVal->Text = L"---";
			// 
			// label_P1MotorTempVal
			// 
			this->label_P1MotorTempVal->AutoSize = true;
			this->label_P1MotorTempVal->Location = System::Drawing::Point(212, 23);
			this->label_P1MotorTempVal->Name = L"label_P1MotorTempVal";
			this->label_P1MotorTempVal->Size = System::Drawing::Size(16, 13);
			this->label_P1MotorTempVal->TabIndex = 48;
			this->label_P1MotorTempVal->Text = L"---";
			// 
			// label_P1MotorAlert
			// 
			this->label_P1MotorAlert->AutoSize = true;
			this->label_P1MotorAlert->Location = System::Drawing::Point(133, 57);
			this->label_P1MotorAlert->Name = L"label_P1MotorAlert";
			this->label_P1MotorAlert->Size = System::Drawing::Size(61, 13);
			this->label_P1MotorAlert->TabIndex = 47;
			this->label_P1MotorAlert->Text = L"Motor Alert:";
			// 
			// label_P1BattAlert
			// 
			this->label_P1BattAlert->AutoSize = true;
			this->label_P1BattAlert->Location = System::Drawing::Point(133, 40);
			this->label_P1BattAlert->Name = L"label_P1BattAlert";
			this->label_P1BattAlert->Size = System::Drawing::Size(67, 13);
			this->label_P1BattAlert->TabIndex = 46;
			this->label_P1BattAlert->Text = L"Battery Alert:";
			// 
			// label_P1MotorTemp
			// 
			this->label_P1MotorTemp->AutoSize = true;
			this->label_P1MotorTemp->Location = System::Drawing::Point(133, 23);
			this->label_P1MotorTemp->Name = L"label_P1MotorTemp";
			this->label_P1MotorTemp->Size = System::Drawing::Size(67, 13);
			this->label_P1MotorTemp->TabIndex = 45;
			this->label_P1MotorTemp->Text = L"Motor Temp:";
			// 
			// label_P1BattTempVal
			// 
			this->label_P1BattTempVal->AutoSize = true;
			this->label_P1BattTempVal->Location = System::Drawing::Point(212, 6);
			this->label_P1BattTempVal->Name = L"label_P1BattTempVal";
			this->label_P1BattTempVal->Size = System::Drawing::Size(16, 13);
			this->label_P1BattTempVal->TabIndex = 44;
			this->label_P1BattTempVal->Text = L"---";
			// 
			// label_P1BattTemp
			// 
			this->label_P1BattTemp->AutoSize = true;
			this->label_P1BattTemp->Location = System::Drawing::Point(133, 6);
			this->label_P1BattTemp->Name = L"label_P1BattTemp";
			this->label_P1BattTemp->Size = System::Drawing::Size(73, 13);
			this->label_P1BattTemp->TabIndex = 43;
			this->label_P1BattTemp->Text = L"Battery Temp:";
			// 
			// label_P1RegenLevelVal
			// 
			this->label_P1RegenLevelVal->AutoSize = true;
			this->label_P1RegenLevelVal->Location = System::Drawing::Point(212, 91);
			this->label_P1RegenLevelVal->Name = L"label_P1RegenLevelVal";
			this->label_P1RegenLevelVal->Size = System::Drawing::Size(16, 13);
			this->label_P1RegenLevelVal->TabIndex = 18;
			this->label_P1RegenLevelVal->Text = L"---";
			// 
			// label_P1RightValue
			// 
			this->label_P1RightValue->AutoSize = true;
			this->label_P1RightValue->Location = System::Drawing::Point(72, 91);
			this->label_P1RightValue->Name = L"label_P1RightValue";
			this->label_P1RightValue->Size = System::Drawing::Size(16, 13);
			this->label_P1RightValue->TabIndex = 14;
			this->label_P1RightValue->Text = L"---";
			// 
			// label_P1AssistLevelVal
			// 
			this->label_P1AssistLevelVal->AutoSize = true;
			this->label_P1AssistLevelVal->Location = System::Drawing::Point(212, 74);
			this->label_P1AssistLevelVal->Name = L"label_P1AssistLevelVal";
			this->label_P1AssistLevelVal->Size = System::Drawing::Size(16, 13);
			this->label_P1AssistLevelVal->TabIndex = 17;
			this->label_P1AssistLevelVal->Text = L"---";
			// 
			// label_P1LeftValue
			// 
			this->label_P1LeftValue->AutoSize = true;
			this->label_P1LeftValue->Location = System::Drawing::Point(72, 74);
			this->label_P1LeftValue->Name = L"label_P1LeftValue";
			this->label_P1LeftValue->Size = System::Drawing::Size(16, 13);
			this->label_P1LeftValue->TabIndex = 13;
			this->label_P1LeftValue->Text = L"---";
			// 
			// label_P1RegenLevel
			// 
			this->label_P1RegenLevel->AutoSize = true;
			this->label_P1RegenLevel->Location = System::Drawing::Point(133, 91);
			this->label_P1RegenLevel->Name = L"label_P1RegenLevel";
			this->label_P1RegenLevel->Size = System::Drawing::Size(71, 13);
			this->label_P1RegenLevel->TabIndex = 3;
			this->label_P1RegenLevel->Text = L"Regen Level:";
			// 
			// label_P1BeamValue
			// 
			this->label_P1BeamValue->AutoSize = true;
			this->label_P1BeamValue->Location = System::Drawing::Point(72, 57);
			this->label_P1BeamValue->Name = L"label_P1BeamValue";
			this->label_P1BeamValue->Size = System::Drawing::Size(16, 13);
			this->label_P1BeamValue->TabIndex = 12;
			this->label_P1BeamValue->Text = L"---";
			// 
			// label_P1AssistLevel
			// 
			this->label_P1AssistLevel->AutoSize = true;
			this->label_P1AssistLevel->Location = System::Drawing::Point(133, 74);
			this->label_P1AssistLevel->Name = L"label_P1AssistLevel";
			this->label_P1AssistLevel->Size = System::Drawing::Size(66, 13);
			this->label_P1AssistLevel->TabIndex = 2;
			this->label_P1AssistLevel->Text = L"Assist Level:";
			// 
			// label_P1LightValue
			// 
			this->label_P1LightValue->AutoSize = true;
			this->label_P1LightValue->Location = System::Drawing::Point(72, 40);
			this->label_P1LightValue->Name = L"label_P1LightValue";
			this->label_P1LightValue->Size = System::Drawing::Size(16, 13);
			this->label_P1LightValue->TabIndex = 11;
			this->label_P1LightValue->Text = L"---";
			// 
			// label_P1SpeedValue
			// 
			this->label_P1SpeedValue->AutoSize = true;
			this->label_P1SpeedValue->Location = System::Drawing::Point(72, 6);
			this->label_P1SpeedValue->Name = L"label_P1SpeedValue";
			this->label_P1SpeedValue->Size = System::Drawing::Size(16, 13);
			this->label_P1SpeedValue->TabIndex = 10;
			this->label_P1SpeedValue->Text = L"---";
			// 
			// label_P1ErrorValue
			// 
			this->label_P1ErrorValue->AutoSize = true;
			this->label_P1ErrorValue->Location = System::Drawing::Point(321, 91);
			this->label_P1ErrorValue->Name = L"label_P1ErrorValue";
			this->label_P1ErrorValue->Size = System::Drawing::Size(16, 13);
			this->label_P1ErrorValue->TabIndex = 9;
			this->label_P1ErrorValue->Text = L"---";
			// 
			// label_P1Right
			// 
			this->label_P1Right->AutoSize = true;
			this->label_P1Right->Location = System::Drawing::Point(2, 91);
			this->label_P1Right->Name = L"label_P1Right";
			this->label_P1Right->Size = System::Drawing::Size(67, 13);
			this->label_P1Right->TabIndex = 8;
			this->label_P1Right->Text = L"Right Signal:";
			// 
			// label_P1Left
			// 
			this->label_P1Left->AutoSize = true;
			this->label_P1Left->Location = System::Drawing::Point(2, 74);
			this->label_P1Left->Name = L"label_P1Left";
			this->label_P1Left->Size = System::Drawing::Size(60, 13);
			this->label_P1Left->TabIndex = 7;
			this->label_P1Left->Text = L"Left Signal:";
			// 
			// label_P1Beam
			// 
			this->label_P1Beam->AutoSize = true;
			this->label_P1Beam->Location = System::Drawing::Point(2, 57);
			this->label_P1Beam->Name = L"label_P1Beam";
			this->label_P1Beam->Size = System::Drawing::Size(65, 13);
			this->label_P1Beam->TabIndex = 6;
			this->label_P1Beam->Text = L"Beam State:";
			// 
			// label_P1Light
			// 
			this->label_P1Light->AutoSize = true;
			this->label_P1Light->Location = System::Drawing::Point(2, 40);
			this->label_P1Light->Name = L"label_P1Light";
			this->label_P1Light->Size = System::Drawing::Size(61, 13);
			this->label_P1Light->TabIndex = 5;
			this->label_P1Light->Text = L"Light State:";
			// 
			// label_P1Speed
			// 
			this->label_P1Speed->AutoSize = true;
			this->label_P1Speed->Location = System::Drawing::Point(2, 6);
			this->label_P1Speed->Name = L"label_P1Speed";
			this->label_P1Speed->Size = System::Drawing::Size(64, 13);
			this->label_P1Speed->TabIndex = 4;
			this->label_P1Speed->Text = L"LEV Speed:";
			// 
			// label_P1Error
			// 
			this->label_P1Error->AutoSize = true;
			this->label_P1Error->Location = System::Drawing::Point(255, 91);
			this->label_P1Error->Name = L"label_P1Error";
			this->label_P1Error->Size = System::Drawing::Size(60, 13);
			this->label_P1Error->TabIndex = 3;
			this->label_P1Error->Text = L"Error Code:";
			// 
			// tabPage_P2
			// 
			this->tabPage_P2->Controls->Add(this->label_P34FuelValue);
			this->tabPage_P2->Controls->Add(this->labelP34_Fuel);
			this->tabPage_P2->Controls->Add(this->label_P2RangeVal);
			this->tabPage_P2->Controls->Add(this->label_P2Range);
			this->tabPage_P2->Controls->Add(this->label_P2SpeedVal);
			this->tabPage_P2->Controls->Add(this->label_P2Speed);
			this->tabPage_P2->Controls->Add(this->label_P2OdometerVal);
			this->tabPage_P2->Controls->Add(this->label_P2Odometer);
			this->tabPage_P2->Location = System::Drawing::Point(4, 22);
			this->tabPage_P2->Name = L"tabPage_P2";
			this->tabPage_P2->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_P2->Size = System::Drawing::Size(389, 111);
			this->tabPage_P2->TabIndex = 1;
			this->tabPage_P2->Text = L"Page 2/34";
			this->tabPage_P2->UseVisualStyleBackColor = true;
			// 
			// label_P34FuelValue
			// 
			this->label_P34FuelValue->AutoSize = true;
			this->label_P34FuelValue->Location = System::Drawing::Point(207, 86);
			this->label_P34FuelValue->Name = L"label_P34FuelValue";
			this->label_P34FuelValue->Size = System::Drawing::Size(16, 13);
			this->label_P34FuelValue->TabIndex = 40;
			this->label_P34FuelValue->Text = L"---";
			// 
			// labelP34_Fuel
			// 
			this->labelP34_Fuel->AutoSize = true;
			this->labelP34_Fuel->Location = System::Drawing::Point(52, 86);
			this->labelP34_Fuel->Name = L"labelP34_Fuel";
			this->labelP34_Fuel->Size = System::Drawing::Size(136, 13);
			this->labelP34_Fuel->TabIndex = 39;
			this->labelP34_Fuel->Text = L"Fuel Consumption(Wh/km):";
			// 
			// label_P2RangeVal
			// 
			this->label_P2RangeVal->AutoSize = true;
			this->label_P2RangeVal->Location = System::Drawing::Point(207, 14);
			this->label_P2RangeVal->Name = L"label_P2RangeVal";
			this->label_P2RangeVal->Size = System::Drawing::Size(16, 13);
			this->label_P2RangeVal->TabIndex = 38;
			this->label_P2RangeVal->Text = L"---";
			// 
			// label_P2Range
			// 
			this->label_P2Range->AutoSize = true;
			this->label_P2Range->Location = System::Drawing::Point(70, 14);
			this->label_P2Range->Name = L"label_P2Range";
			this->label_P2Range->Size = System::Drawing::Size(118, 13);
			this->label_P2Range->TabIndex = 37;
			this->label_P2Range->Text = L"Remaining Range (km):";
			// 
			// label_P2SpeedVal
			// 
			this->label_P2SpeedVal->AutoSize = true;
			this->label_P2SpeedVal->Location = System::Drawing::Point(207, 62);
			this->label_P2SpeedVal->Name = L"label_P2SpeedVal";
			this->label_P2SpeedVal->Size = System::Drawing::Size(16, 13);
			this->label_P2SpeedVal->TabIndex = 16;
			this->label_P2SpeedVal->Text = L"---";
			// 
			// label_P2Speed
			// 
			this->label_P2Speed->AutoSize = true;
			this->label_P2Speed->Location = System::Drawing::Point(93, 62);
			this->label_P2Speed->Name = L"label_P2Speed";
			this->label_P2Speed->Size = System::Drawing::Size(95, 13);
			this->label_P2Speed->TabIndex = 15;
			this->label_P2Speed->Text = L"LEV Speed(km/h):";
			// 
			// label_P2OdometerVal
			// 
			this->label_P2OdometerVal->AutoSize = true;
			this->label_P2OdometerVal->Location = System::Drawing::Point(207, 37);
			this->label_P2OdometerVal->Name = L"label_P2OdometerVal";
			this->label_P2OdometerVal->Size = System::Drawing::Size(16, 13);
			this->label_P2OdometerVal->TabIndex = 14;
			this->label_P2OdometerVal->Text = L"---";
			// 
			// label_P2Odometer
			// 
			this->label_P2Odometer->AutoSize = true;
			this->label_P2Odometer->Location = System::Drawing::Point(112, 37);
			this->label_P2Odometer->Name = L"label_P2Odometer";
			this->label_P2Odometer->Size = System::Drawing::Size(76, 13);
			this->label_P2Odometer->TabIndex = 13;
			this->label_P2Odometer->Text = L"Odometer(km):";
			// 
			// tabPage_P3
			// 
			this->tabPage_P3->Controls->Add(this->label_P3PercentAssistVal);
			this->tabPage_P3->Controls->Add(this->label_P3BattEmpty);
			this->tabPage_P3->Controls->Add(this->label_P3BattEmptyVal);
			this->tabPage_P3->Controls->Add(this->label_P3ChargeVal);
			this->tabPage_P3->Controls->Add(this->label_P3ThrottleVal);
			this->tabPage_P3->Controls->Add(this->label_P3Throttle);
			this->tabPage_P3->Controls->Add(this->label_P3PercentAssist);
			this->tabPage_P3->Controls->Add(this->label_P3BattSOC);
			this->tabPage_P3->Controls->Add(this->label_P3RearGearVal);
			this->tabPage_P3->Controls->Add(this->label_P3FrontGearVal);
			this->tabPage_P3->Controls->Add(this->label_P3ManAutoVal);
			this->tabPage_P3->Controls->Add(this->label_P3GearsExistVal);
			this->tabPage_P3->Controls->Add(this->label_P3RearGear);
			this->tabPage_P3->Controls->Add(this->label_P3FrontGear);
			this->tabPage_P3->Controls->Add(this->label_P3ManAuto);
			this->tabPage_P3->Controls->Add(this->label_P3GearsExist);
			this->tabPage_P3->Controls->Add(this->label_P3RegenVal);
			this->tabPage_P3->Controls->Add(this->label_P3RightVal);
			this->tabPage_P3->Controls->Add(this->label_P3AssistLvlVal);
			this->tabPage_P3->Controls->Add(this->label_P3LeftVal);
			this->tabPage_P3->Controls->Add(this->label_P3Regen);
			this->tabPage_P3->Controls->Add(this->label_P3BeamVal);
			this->tabPage_P3->Controls->Add(this->label_P3AssistLvl);
			this->tabPage_P3->Controls->Add(this->label_P3LightVal);
			this->tabPage_P3->Controls->Add(this->label_P3SpeedVal);
			this->tabPage_P3->Controls->Add(this->label_P3Right);
			this->tabPage_P3->Controls->Add(this->label_P3Left);
			this->tabPage_P3->Controls->Add(this->label_P3Beam);
			this->tabPage_P3->Controls->Add(this->label_P3Light);
			this->tabPage_P3->Controls->Add(this->label_P3Speed);
			this->tabPage_P3->Location = System::Drawing::Point(4, 22);
			this->tabPage_P3->Name = L"tabPage_P3";
			this->tabPage_P3->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_P3->Size = System::Drawing::Size(389, 111);
			this->tabPage_P3->TabIndex = 2;
			this->tabPage_P3->Text = L"Page 3";
			this->tabPage_P3->UseVisualStyleBackColor = true;
			// 
			// label_P3PercentAssistVal
			// 
			this->label_P3PercentAssistVal->AutoSize = true;
			this->label_P3PercentAssistVal->Location = System::Drawing::Point(224, 57);
			this->label_P3PercentAssistVal->Name = L"label_P3PercentAssistVal";
			this->label_P3PercentAssistVal->Size = System::Drawing::Size(16, 13);
			this->label_P3PercentAssistVal->TabIndex = 102;
			this->label_P3PercentAssistVal->Text = L"---";
			// 
			// label_P3BattEmpty
			// 
			this->label_P3BattEmpty->AutoSize = true;
			this->label_P3BattEmpty->Location = System::Drawing::Point(133, 23);
			this->label_P3BattEmpty->Name = L"label_P3BattEmpty";
			this->label_P3BattEmpty->Size = System::Drawing::Size(118, 13);
			this->label_P3BattEmpty->TabIndex = 100;
			this->label_P3BattEmpty->Text = L"Battery Empty Warning:";
			// 
			// label_P3BattEmptyVal
			// 
			this->label_P3BattEmptyVal->AutoSize = true;
			this->label_P3BattEmptyVal->Location = System::Drawing::Point(257, 23);
			this->label_P3BattEmptyVal->Name = L"label_P3BattEmptyVal";
			this->label_P3BattEmptyVal->Size = System::Drawing::Size(16, 13);
			this->label_P3BattEmptyVal->TabIndex = 99;
			this->label_P3BattEmptyVal->Text = L"---";
			// 
			// label_P3ChargeVal
			// 
			this->label_P3ChargeVal->AutoSize = true;
			this->label_P3ChargeVal->Location = System::Drawing::Point(207, 6);
			this->label_P3ChargeVal->Name = L"label_P3ChargeVal";
			this->label_P3ChargeVal->Size = System::Drawing::Size(16, 13);
			this->label_P3ChargeVal->TabIndex = 98;
			this->label_P3ChargeVal->Text = L"---";
			// 
			// label_P3ThrottleVal
			// 
			this->label_P3ThrottleVal->AutoSize = true;
			this->label_P3ThrottleVal->Location = System::Drawing::Point(72, 23);
			this->label_P3ThrottleVal->Name = L"label_P3ThrottleVal";
			this->label_P3ThrottleVal->Size = System::Drawing::Size(16, 13);
			this->label_P3ThrottleVal->TabIndex = 96;
			this->label_P3ThrottleVal->Text = L"---";
			// 
			// label_P3Throttle
			// 
			this->label_P3Throttle->AutoSize = true;
			this->label_P3Throttle->Location = System::Drawing::Point(2, 23);
			this->label_P3Throttle->Name = L"label_P3Throttle";
			this->label_P3Throttle->Size = System::Drawing::Size(46, 13);
			this->label_P3Throttle->TabIndex = 95;
			this->label_P3Throttle->Text = L"Throttle:";
			// 
			// label_P3PercentAssist
			// 
			this->label_P3PercentAssist->AutoSize = true;
			this->label_P3PercentAssist->Location = System::Drawing::Point(133, 57);
			this->label_P3PercentAssist->Name = L"label_P3PercentAssist";
			this->label_P3PercentAssist->Size = System::Drawing::Size(85, 13);
			this->label_P3PercentAssist->TabIndex = 93;
			this->label_P3PercentAssist->Text = L"Current % Assist:";
			// 
			// label_P3BattSOC
			// 
			this->label_P3BattSOC->AutoSize = true;
			this->label_P3BattSOC->Location = System::Drawing::Point(133, 6);
			this->label_P3BattSOC->Name = L"label_P3BattSOC";
			this->label_P3BattSOC->Size = System::Drawing::Size(68, 13);
			this->label_P3BattSOC->TabIndex = 91;
			this->label_P3BattSOC->Text = L"Battery SOC:";
			// 
			// label_P3RearGearVal
			// 
			this->label_P3RearGearVal->AutoSize = true;
			this->label_P3RearGearVal->Location = System::Drawing::Point(352, 57);
			this->label_P3RearGearVal->Name = L"label_P3RearGearVal";
			this->label_P3RearGearVal->Size = System::Drawing::Size(16, 13);
			this->label_P3RearGearVal->TabIndex = 90;
			this->label_P3RearGearVal->Text = L"---";
			// 
			// label_P3FrontGearVal
			// 
			this->label_P3FrontGearVal->AutoSize = true;
			this->label_P3FrontGearVal->Location = System::Drawing::Point(352, 40);
			this->label_P3FrontGearVal->Name = L"label_P3FrontGearVal";
			this->label_P3FrontGearVal->Size = System::Drawing::Size(16, 13);
			this->label_P3FrontGearVal->TabIndex = 89;
			this->label_P3FrontGearVal->Text = L"---";
			// 
			// label_P3ManAutoVal
			// 
			this->label_P3ManAutoVal->AutoSize = true;
			this->label_P3ManAutoVal->Location = System::Drawing::Point(364, 23);
			this->label_P3ManAutoVal->Name = L"label_P3ManAutoVal";
			this->label_P3ManAutoVal->Size = System::Drawing::Size(16, 13);
			this->label_P3ManAutoVal->TabIndex = 88;
			this->label_P3ManAutoVal->Text = L"---";
			// 
			// label_P3GearsExistVal
			// 
			this->label_P3GearsExistVal->AutoSize = true;
			this->label_P3GearsExistVal->Location = System::Drawing::Point(364, 6);
			this->label_P3GearsExistVal->Name = L"label_P3GearsExistVal";
			this->label_P3GearsExistVal->Size = System::Drawing::Size(16, 13);
			this->label_P3GearsExistVal->TabIndex = 87;
			this->label_P3GearsExistVal->Text = L"---";
			// 
			// label_P3RearGear
			// 
			this->label_P3RearGear->AutoSize = true;
			this->label_P3RearGear->Location = System::Drawing::Point(286, 57);
			this->label_P3RearGear->Name = L"label_P3RearGear";
			this->label_P3RearGear->Size = System::Drawing::Size(59, 13);
			this->label_P3RearGear->TabIndex = 86;
			this->label_P3RearGear->Text = L"Rear Gear:";
			// 
			// label_P3FrontGear
			// 
			this->label_P3FrontGear->AutoSize = true;
			this->label_P3FrontGear->Location = System::Drawing::Point(286, 40);
			this->label_P3FrontGear->Name = L"label_P3FrontGear";
			this->label_P3FrontGear->Size = System::Drawing::Size(60, 13);
			this->label_P3FrontGear->TabIndex = 85;
			this->label_P3FrontGear->Text = L"Front Gear:";
			// 
			// label_P3ManAuto
			// 
			this->label_P3ManAuto->AutoSize = true;
			this->label_P3ManAuto->Location = System::Drawing::Point(286, 23);
			this->label_P3ManAuto->Name = L"label_P3ManAuto";
			this->label_P3ManAuto->Size = System::Drawing::Size(72, 13);
			this->label_P3ManAuto->TabIndex = 84;
			this->label_P3ManAuto->Text = L"Manual/Auto:";
			// 
			// label_P3GearsExist
			// 
			this->label_P3GearsExist->AutoSize = true;
			this->label_P3GearsExist->Location = System::Drawing::Point(286, 6);
			this->label_P3GearsExist->Name = L"label_P3GearsExist";
			this->label_P3GearsExist->Size = System::Drawing::Size(63, 13);
			this->label_P3GearsExist->TabIndex = 83;
			this->label_P3GearsExist->Text = L"Gears Exist:";
			// 
			// label_P3RegenVal
			// 
			this->label_P3RegenVal->AutoSize = true;
			this->label_P3RegenVal->Location = System::Drawing::Point(224, 91);
			this->label_P3RegenVal->Name = L"label_P3RegenVal";
			this->label_P3RegenVal->Size = System::Drawing::Size(16, 13);
			this->label_P3RegenVal->TabIndex = 74;
			this->label_P3RegenVal->Text = L"---";
			// 
			// label_P3RightVal
			// 
			this->label_P3RightVal->AutoSize = true;
			this->label_P3RightVal->Location = System::Drawing::Point(72, 91);
			this->label_P3RightVal->Name = L"label_P3RightVal";
			this->label_P3RightVal->Size = System::Drawing::Size(16, 13);
			this->label_P3RightVal->TabIndex = 72;
			this->label_P3RightVal->Text = L"---";
			// 
			// label_P3AssistLvlVal
			// 
			this->label_P3AssistLvlVal->AutoSize = true;
			this->label_P3AssistLvlVal->Location = System::Drawing::Point(224, 74);
			this->label_P3AssistLvlVal->Name = L"label_P3AssistLvlVal";
			this->label_P3AssistLvlVal->Size = System::Drawing::Size(16, 13);
			this->label_P3AssistLvlVal->TabIndex = 73;
			this->label_P3AssistLvlVal->Text = L"---";
			// 
			// label_P3LeftVal
			// 
			this->label_P3LeftVal->AutoSize = true;
			this->label_P3LeftVal->Location = System::Drawing::Point(72, 74);
			this->label_P3LeftVal->Name = L"label_P3LeftVal";
			this->label_P3LeftVal->Size = System::Drawing::Size(16, 13);
			this->label_P3LeftVal->TabIndex = 71;
			this->label_P3LeftVal->Text = L"---";
			// 
			// label_P3Regen
			// 
			this->label_P3Regen->AutoSize = true;
			this->label_P3Regen->Location = System::Drawing::Point(133, 91);
			this->label_P3Regen->Name = L"label_P3Regen";
			this->label_P3Regen->Size = System::Drawing::Size(71, 13);
			this->label_P3Regen->TabIndex = 61;
			this->label_P3Regen->Text = L"Regen Level:";
			// 
			// label_P3BeamVal
			// 
			this->label_P3BeamVal->AutoSize = true;
			this->label_P3BeamVal->Location = System::Drawing::Point(72, 57);
			this->label_P3BeamVal->Name = L"label_P3BeamVal";
			this->label_P3BeamVal->Size = System::Drawing::Size(16, 13);
			this->label_P3BeamVal->TabIndex = 70;
			this->label_P3BeamVal->Text = L"---";
			// 
			// label_P3AssistLvl
			// 
			this->label_P3AssistLvl->AutoSize = true;
			this->label_P3AssistLvl->Location = System::Drawing::Point(133, 74);
			this->label_P3AssistLvl->Name = L"label_P3AssistLvl";
			this->label_P3AssistLvl->Size = System::Drawing::Size(66, 13);
			this->label_P3AssistLvl->TabIndex = 59;
			this->label_P3AssistLvl->Text = L"Assist Level:";
			// 
			// label_P3LightVal
			// 
			this->label_P3LightVal->AutoSize = true;
			this->label_P3LightVal->Location = System::Drawing::Point(72, 40);
			this->label_P3LightVal->Name = L"label_P3LightVal";
			this->label_P3LightVal->Size = System::Drawing::Size(16, 13);
			this->label_P3LightVal->TabIndex = 69;
			this->label_P3LightVal->Text = L"---";
			// 
			// label_P3SpeedVal
			// 
			this->label_P3SpeedVal->AutoSize = true;
			this->label_P3SpeedVal->Location = System::Drawing::Point(72, 6);
			this->label_P3SpeedVal->Name = L"label_P3SpeedVal";
			this->label_P3SpeedVal->Size = System::Drawing::Size(16, 13);
			this->label_P3SpeedVal->TabIndex = 68;
			this->label_P3SpeedVal->Text = L"---";
			// 
			// label_P3Right
			// 
			this->label_P3Right->AutoSize = true;
			this->label_P3Right->Location = System::Drawing::Point(2, 91);
			this->label_P3Right->Name = L"label_P3Right";
			this->label_P3Right->Size = System::Drawing::Size(67, 13);
			this->label_P3Right->TabIndex = 66;
			this->label_P3Right->Text = L"Right Signal:";
			// 
			// label_P3Left
			// 
			this->label_P3Left->AutoSize = true;
			this->label_P3Left->Location = System::Drawing::Point(2, 74);
			this->label_P3Left->Name = L"label_P3Left";
			this->label_P3Left->Size = System::Drawing::Size(60, 13);
			this->label_P3Left->TabIndex = 65;
			this->label_P3Left->Text = L"Left Signal:";
			// 
			// label_P3Beam
			// 
			this->label_P3Beam->AutoSize = true;
			this->label_P3Beam->Location = System::Drawing::Point(2, 57);
			this->label_P3Beam->Name = L"label_P3Beam";
			this->label_P3Beam->Size = System::Drawing::Size(65, 13);
			this->label_P3Beam->TabIndex = 64;
			this->label_P3Beam->Text = L"Beam State:";
			// 
			// label_P3Light
			// 
			this->label_P3Light->AutoSize = true;
			this->label_P3Light->Location = System::Drawing::Point(2, 40);
			this->label_P3Light->Name = L"label_P3Light";
			this->label_P3Light->Size = System::Drawing::Size(61, 13);
			this->label_P3Light->TabIndex = 63;
			this->label_P3Light->Text = L"Light State:";
			// 
			// label_P3Speed
			// 
			this->label_P3Speed->AutoSize = true;
			this->label_P3Speed->Location = System::Drawing::Point(2, 6);
			this->label_P3Speed->Name = L"label_P3Speed";
			this->label_P3Speed->Size = System::Drawing::Size(64, 13);
			this->label_P3Speed->TabIndex = 62;
			this->label_P3Speed->Text = L"LEV Speed:";
			// 
			// tabPage_Page45
			// 
			this->tabPage_Page45->Controls->Add(this->tabControl_Page45);
			this->tabPage_Page45->Location = System::Drawing::Point(4, 22);
			this->tabPage_Page45->Name = L"tabPage_Page45";
			this->tabPage_Page45->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_Page45->Size = System::Drawing::Size(389, 111);
			this->tabPage_Page45->TabIndex = 6;
			this->tabPage_Page45->Text = L"Page 4/5";
			this->tabPage_Page45->UseVisualStyleBackColor = true;
			// 
			// tabControl_Page45
			// 
			this->tabControl_Page45->Controls->Add(this->tabPage_Page4);
			this->tabControl_Page45->Controls->Add(this->tabPage_Page5);
			this->tabControl_Page45->Controls->Add(this->tabPage_RqPage);
			this->tabControl_Page45->Location = System::Drawing::Point(3, 5);
			this->tabControl_Page45->Name = L"tabControl_Page45";
			this->tabControl_Page45->SelectedIndex = 0;
			this->tabControl_Page45->Size = System::Drawing::Size(383, 103);
			this->tabControl_Page45->TabIndex = 3;
			// 
			// tabPage_Page4
			// 
			this->tabPage_Page4->Controls->Add(this->label_P4DistanceChargeVal);
			this->tabPage_Page4->Controls->Add(this->label_P4FuelVal);
			this->tabPage_Page4->Controls->Add(this->label_P4ChargeCycleVal);
			this->tabPage_Page4->Controls->Add(this->label_P4VoltageVal);
			this->tabPage_Page4->Controls->Add(this->label_P4Voltage);
			this->tabPage_Page4->Controls->Add(this->label_P4DistanceOnCharge);
			this->tabPage_Page4->Controls->Add(this->label_P4FuelConsumption);
			this->tabPage_Page4->Controls->Add(this->label_P4ChargeCycle);
			this->tabPage_Page4->Location = System::Drawing::Point(4, 22);
			this->tabPage_Page4->Name = L"tabPage_Page4";
			this->tabPage_Page4->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_Page4->Size = System::Drawing::Size(375, 77);
			this->tabPage_Page4->TabIndex = 5;
			this->tabPage_Page4->Text = L"Page 4";
			this->tabPage_Page4->UseVisualStyleBackColor = true;
			// 
			// label_P4DistanceChargeVal
			// 
			this->label_P4DistanceChargeVal->AutoSize = true;
			this->label_P4DistanceChargeVal->Location = System::Drawing::Point(141, 55);
			this->label_P4DistanceChargeVal->Name = L"label_P4DistanceChargeVal";
			this->label_P4DistanceChargeVal->Size = System::Drawing::Size(16, 13);
			this->label_P4DistanceChargeVal->TabIndex = 39;
			this->label_P4DistanceChargeVal->Text = L"---";
			// 
			// label_P4FuelVal
			// 
			this->label_P4FuelVal->AutoSize = true;
			this->label_P4FuelVal->Location = System::Drawing::Point(153, 32);
			this->label_P4FuelVal->Name = L"label_P4FuelVal";
			this->label_P4FuelVal->Size = System::Drawing::Size(16, 13);
			this->label_P4FuelVal->TabIndex = 38;
			this->label_P4FuelVal->Text = L"---";
			// 
			// label_P4ChargeCycleVal
			// 
			this->label_P4ChargeCycleVal->AutoSize = true;
			this->label_P4ChargeCycleVal->Location = System::Drawing::Point(126, 8);
			this->label_P4ChargeCycleVal->Name = L"label_P4ChargeCycleVal";
			this->label_P4ChargeCycleVal->Size = System::Drawing::Size(16, 13);
			this->label_P4ChargeCycleVal->TabIndex = 37;
			this->label_P4ChargeCycleVal->Text = L"---";
			// 
			// label_P4VoltageVal
			// 
			this->label_P4VoltageVal->AutoSize = true;
			this->label_P4VoltageVal->Location = System::Drawing::Point(325, 8);
			this->label_P4VoltageVal->Name = L"label_P4VoltageVal";
			this->label_P4VoltageVal->Size = System::Drawing::Size(16, 13);
			this->label_P4VoltageVal->TabIndex = 36;
			this->label_P4VoltageVal->Text = L"---";
			// 
			// label_P4Voltage
			// 
			this->label_P4Voltage->AutoSize = true;
			this->label_P4Voltage->Location = System::Drawing::Point(201, 8);
			this->label_P4Voltage->Name = L"label_P4Voltage";
			this->label_P4Voltage->Size = System::Drawing::Size(118, 13);
			this->label_P4Voltage->TabIndex = 35;
			this->label_P4Voltage->Text = L"Battery Voltage (1/4 V):";
			// 
			// label_P4DistanceOnCharge
			// 
			this->label_P4DistanceOnCharge->AutoSize = true;
			this->label_P4DistanceOnCharge->Location = System::Drawing::Point(8, 55);
			this->label_P4DistanceOnCharge->Name = L"label_P4DistanceOnCharge";
			this->label_P4DistanceOnCharge->Size = System::Drawing::Size(127, 13);
			this->label_P4DistanceOnCharge->TabIndex = 11;
			this->label_P4DistanceOnCharge->Text = L"Distance on Charge (km):";
			// 
			// label_P4FuelConsumption
			// 
			this->label_P4FuelConsumption->AutoSize = true;
			this->label_P4FuelConsumption->Location = System::Drawing::Point(8, 32);
			this->label_P4FuelConsumption->Name = L"label_P4FuelConsumption";
			this->label_P4FuelConsumption->Size = System::Drawing::Size(139, 13);
			this->label_P4FuelConsumption->TabIndex = 8;
			this->label_P4FuelConsumption->Text = L"Fuel Consumption (Wh/km):";
			// 
			// label_P4ChargeCycle
			// 
			this->label_P4ChargeCycle->AutoSize = true;
			this->label_P4ChargeCycle->Location = System::Drawing::Point(8, 8);
			this->label_P4ChargeCycle->Name = L"label_P4ChargeCycle";
			this->label_P4ChargeCycle->Size = System::Drawing::Size(112, 13);
			this->label_P4ChargeCycle->TabIndex = 0;
			this->label_P4ChargeCycle->Text = L"Charging Cycle Count:";
			// 
			// tabPage_Page5
			// 
			this->tabPage_Page5->Controls->Add(this->label_P5CircumVal);
			this->tabPage_Page5->Controls->Add(this->label_P5NumRegenVal);
			this->tabPage_Page5->Controls->Add(this->label_P5NumAssistVal);
			this->tabPage_Page5->Controls->Add(this->label_P5NumRegen);
			this->tabPage_Page5->Controls->Add(this->label_P5NumAssist);
			this->tabPage_Page5->Controls->Add(this->label_P5Circum);
			this->tabPage_Page5->Location = System::Drawing::Point(4, 22);
			this->tabPage_Page5->Name = L"tabPage_Page5";
			this->tabPage_Page5->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_Page5->Size = System::Drawing::Size(375, 77);
			this->tabPage_Page5->TabIndex = 6;
			this->tabPage_Page5->Text = L"Page 5";
			this->tabPage_Page5->UseVisualStyleBackColor = true;
			// 
			// label_P5CircumVal
			// 
			this->label_P5CircumVal->AutoSize = true;
			this->label_P5CircumVal->Location = System::Drawing::Point(314, 14);
			this->label_P5CircumVal->Name = L"label_P5CircumVal";
			this->label_P5CircumVal->Size = System::Drawing::Size(16, 13);
			this->label_P5CircumVal->TabIndex = 53;
			this->label_P5CircumVal->Text = L"---";
			// 
			// label_P5NumRegenVal
			// 
			this->label_P5NumRegenVal->AutoSize = true;
			this->label_P5NumRegenVal->Location = System::Drawing::Point(99, 42);
			this->label_P5NumRegenVal->Name = L"label_P5NumRegenVal";
			this->label_P5NumRegenVal->Size = System::Drawing::Size(16, 13);
			this->label_P5NumRegenVal->TabIndex = 52;
			this->label_P5NumRegenVal->Text = L"---";
			// 
			// label_P5NumAssistVal
			// 
			this->label_P5NumAssistVal->AutoSize = true;
			this->label_P5NumAssistVal->Location = System::Drawing::Point(99, 14);
			this->label_P5NumAssistVal->Name = L"label_P5NumAssistVal";
			this->label_P5NumAssistVal->Size = System::Drawing::Size(16, 13);
			this->label_P5NumAssistVal->TabIndex = 51;
			this->label_P5NumAssistVal->Text = L"---";
			// 
			// label_P5NumRegen
			// 
			this->label_P5NumRegen->AutoSize = true;
			this->label_P5NumRegen->Location = System::Drawing::Point(6, 42);
			this->label_P5NumRegen->Name = L"label_P5NumRegen";
			this->label_P5NumRegen->Size = System::Drawing::Size(87, 13);
			this->label_P5NumRegen->TabIndex = 50;
			this->label_P5NumRegen->Text = L"# Regen Modes:";
			// 
			// label_P5NumAssist
			// 
			this->label_P5NumAssist->AutoSize = true;
			this->label_P5NumAssist->Location = System::Drawing::Point(6, 14);
			this->label_P5NumAssist->Name = L"label_P5NumAssist";
			this->label_P5NumAssist->Size = System::Drawing::Size(82, 13);
			this->label_P5NumAssist->TabIndex = 49;
			this->label_P5NumAssist->Text = L"# Assist Modes:";
			// 
			// label_P5Circum
			// 
			this->label_P5Circum->AutoSize = true;
			this->label_P5Circum->Location = System::Drawing::Point(171, 14);
			this->label_P5Circum->Name = L"label_P5Circum";
			this->label_P5Circum->Size = System::Drawing::Size(137, 13);
			this->label_P5Circum->TabIndex = 16;
			this->label_P5Circum->Text = L"Wheel Circumference (mm):";
			// 
			// tabPage_RqPage
			// 
			this->tabPage_RqPage->Controls->Add(this->button_SendRequest);
			this->tabPage_RqPage->Controls->Add(this->label_PageToRequest);
			this->tabPage_RqPage->Controls->Add(this->groupBox_Request);
			this->tabPage_RqPage->Controls->Add(this->comboBox_PageToRequest);
			this->tabPage_RqPage->Location = System::Drawing::Point(4, 22);
			this->tabPage_RqPage->Name = L"tabPage_RqPage";
			this->tabPage_RqPage->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_RqPage->Size = System::Drawing::Size(375, 77);
			this->tabPage_RqPage->TabIndex = 7;
			this->tabPage_RqPage->Text = L"Request";
			this->tabPage_RqPage->UseVisualStyleBackColor = true;
			// 
			// button_SendRequest
			// 
			this->button_SendRequest->Location = System::Drawing::Point(18, 31);
			this->button_SendRequest->Name = L"button_SendRequest";
			this->button_SendRequest->Size = System::Drawing::Size(86, 23);
			this->button_SendRequest->TabIndex = 28;
			this->button_SendRequest->Text = L"Send Request";
			this->button_SendRequest->UseVisualStyleBackColor = true;
			this->button_SendRequest->Click += gcnew System::EventHandler(this, &LEVDisplay::button_SendRequest_Click);
			// 
			// label_PageToRequest
			// 
			this->label_PageToRequest->AutoSize = true;
			this->label_PageToRequest->Location = System::Drawing::Point(125, 14);
			this->label_PageToRequest->Name = L"label_PageToRequest";
			this->label_PageToRequest->Size = System::Drawing::Size(90, 13);
			this->label_PageToRequest->TabIndex = 27;
			this->label_PageToRequest->Text = L"Page to Request:";
			// 
			// groupBox_Request
			// 
			this->groupBox_Request->Controls->Add(this->label_RqTxTimes);
			this->groupBox_Request->Controls->Add(this->numericUpDown_RqTxTimes);
			this->groupBox_Request->Location = System::Drawing::Point(233, 7);
			this->groupBox_Request->Name = L"groupBox_Request";
			this->groupBox_Request->Size = System::Drawing::Size(136, 64);
			this->groupBox_Request->TabIndex = 26;
			this->groupBox_Request->TabStop = false;
			this->groupBox_Request->Text = L"Sensor Reply";
			// 
			// label_RqTxTimes
			// 
			this->label_RqTxTimes->AutoSize = true;
			this->label_RqTxTimes->Location = System::Drawing::Point(18, 29);
			this->label_RqTxTimes->Name = L"label_RqTxTimes";
			this->label_RqTxTimes->Size = System::Drawing::Size(53, 13);
			this->label_RqTxTimes->TabIndex = 18;
			this->label_RqTxTimes->Text = L"Tx Times:";
			// 
			// numericUpDown_RqTxTimes
			// 
			this->numericUpDown_RqTxTimes->Location = System::Drawing::Point(77, 27);
			this->numericUpDown_RqTxTimes->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {4, 0, 0, 0});
			this->numericUpDown_RqTxTimes->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->numericUpDown_RqTxTimes->Name = L"numericUpDown_RqTxTimes";
			this->numericUpDown_RqTxTimes->Size = System::Drawing::Size(47, 20);
			this->numericUpDown_RqTxTimes->TabIndex = 17;
			this->numericUpDown_RqTxTimes->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {4, 0, 0, 0});
			this->numericUpDown_RqTxTimes->ValueChanged += gcnew System::EventHandler(this, &LEVDisplay::numericUpDown_RqTxTimes_ValueChanged);
			// 
			// comboBox_PageToRequest
			// 
			this->comboBox_PageToRequest->FormattingEnabled = true;
			this->comboBox_PageToRequest->Items->AddRange(gcnew cli::array< System::Object^  >(5) {L"Data Page 1", L"Data Page 2", L"Data Page 3", 
				L"Data Page 4", L"Data Page 5"});
			this->comboBox_PageToRequest->Location = System::Drawing::Point(125, 33);
			this->comboBox_PageToRequest->Name = L"comboBox_PageToRequest";
			this->comboBox_PageToRequest->Size = System::Drawing::Size(91, 21);
			this->comboBox_PageToRequest->TabIndex = 25;
			this->comboBox_PageToRequest->Text = L"Data Page 1";
			this->comboBox_PageToRequest->SelectedIndexChanged += gcnew System::EventHandler(this, &LEVDisplay::comboBox_PageToRequest_SelectedIndexChanged);
			// 
			// tabPage_P16
			// 
			this->tabPage_P16->Controls->Add(this->tabControl_P16);
			this->tabPage_P16->Location = System::Drawing::Point(4, 22);
			this->tabPage_P16->Name = L"tabPage_P16";
			this->tabPage_P16->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_P16->Size = System::Drawing::Size(389, 111);
			this->tabPage_P16->TabIndex = 7;
			this->tabPage_P16->Text = L"Page 16";
			this->tabPage_P16->UseVisualStyleBackColor = true;
			// 
			// tabControl_P16
			// 
			this->tabControl_P16->Controls->Add(this->tabPage_P16TravelMode);
			this->tabControl_P16->Controls->Add(this->tabPage_P16Display);
			this->tabControl_P16->Controls->Add(this->tabPage_P16Command);
			this->tabControl_P16->Controls->Add(this->tabPage_P16ManID);
			this->tabControl_P16->Controls->Add(this->tabPage_P16Wheel);
			this->tabControl_P16->Location = System::Drawing::Point(3, 5);
			this->tabControl_P16->Name = L"tabControl_P16";
			this->tabControl_P16->SelectedIndex = 0;
			this->tabControl_P16->Size = System::Drawing::Size(383, 103);
			this->tabControl_P16->TabIndex = 2;
			// 
			// tabPage_P16TravelMode
			// 
			this->tabPage_P16TravelMode->Controls->Add(this->button_P16Send);
			this->tabPage_P16TravelMode->Location = System::Drawing::Point(4, 22);
			this->tabPage_P16TravelMode->Name = L"tabPage_P16TravelMode";
			this->tabPage_P16TravelMode->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_P16TravelMode->Size = System::Drawing::Size(375, 77);
			this->tabPage_P16TravelMode->TabIndex = 0;
			this->tabPage_P16TravelMode->Text = L"Send";
			this->tabPage_P16TravelMode->UseVisualStyleBackColor = true;
			// 
			// button_P16Send
			// 
			this->button_P16Send->Location = System::Drawing::Point(97, 27);
			this->button_P16Send->Name = L"button_P16Send";
			this->button_P16Send->Size = System::Drawing::Size(180, 23);
			this->button_P16Send->TabIndex = 23;
			this->button_P16Send->Text = L"Send Page 16 to the Sensor";
			this->button_P16Send->UseVisualStyleBackColor = true;
			this->button_P16Send->Click += gcnew System::EventHandler(this, &LEVDisplay::button_P16Send_Click);
			// 
			// tabPage_P16Display
			// 
			this->tabPage_P16Display->Controls->Add(this->checkBox_P16TravelModeNotSupported);
			this->tabPage_P16Display->Controls->Add(this->label_P16CurrentRegen);
			this->tabPage_P16Display->Controls->Add(this->label_P16CurrentAssist);
			this->tabPage_P16Display->Controls->Add(this->comboBox_P16Assist);
			this->tabPage_P16Display->Controls->Add(this->comboBox_P16Regen);
			this->tabPage_P16Display->Location = System::Drawing::Point(4, 22);
			this->tabPage_P16Display->Name = L"tabPage_P16Display";
			this->tabPage_P16Display->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_P16Display->Size = System::Drawing::Size(375, 77);
			this->tabPage_P16Display->TabIndex = 1;
			this->tabPage_P16Display->Text = L"Travel Mode";
			this->tabPage_P16Display->UseVisualStyleBackColor = true;
			// 
			// checkBox_P16TravelModeNotSupported
			// 
			this->checkBox_P16TravelModeNotSupported->AutoSize = true;
			this->checkBox_P16TravelModeNotSupported->Location = System::Drawing::Point(6, 10);
			this->checkBox_P16TravelModeNotSupported->Name = L"checkBox_P16TravelModeNotSupported";
			this->checkBox_P16TravelModeNotSupported->Size = System::Drawing::Size(95, 17);
			this->checkBox_P16TravelModeNotSupported->TabIndex = 27;
			this->checkBox_P16TravelModeNotSupported->Text = L"Not Supported";
			this->checkBox_P16TravelModeNotSupported->UseVisualStyleBackColor = true;
			this->checkBox_P16TravelModeNotSupported->CheckedChanged += gcnew System::EventHandler(this, &LEVDisplay::checkBox_P16TravelModeNotSupported_CheckedChanged);
			// 
			// label_P16CurrentRegen
			// 
			this->label_P16CurrentRegen->AutoSize = true;
			this->label_P16CurrentRegen->Location = System::Drawing::Point(195, 30);
			this->label_P16CurrentRegen->Name = L"label_P16CurrentRegen";
			this->label_P16CurrentRegen->Size = System::Drawing::Size(140, 13);
			this->label_P16CurrentRegen->TabIndex = 26;
			this->label_P16CurrentRegen->Text = L"Current Regenerative Level:";
			// 
			// label_P16CurrentAssist
			// 
			this->label_P16CurrentAssist->AutoSize = true;
			this->label_P16CurrentAssist->Location = System::Drawing::Point(63, 30);
			this->label_P16CurrentAssist->Name = L"label_P16CurrentAssist";
			this->label_P16CurrentAssist->Size = System::Drawing::Size(103, 13);
			this->label_P16CurrentAssist->TabIndex = 25;
			this->label_P16CurrentAssist->Text = L"Current Assist Level:";
			// 
			// comboBox_P16Assist
			// 
			this->comboBox_P16Assist->FormattingEnabled = true;
			this->comboBox_P16Assist->Items->AddRange(gcnew cli::array< System::Object^  >(8) {L"Assist Off", L"Assist 1", L"Assist 2", 
				L"Assist 3", L"Assist 4", L"Assist 5", L"Assist 6", L"Assist 7"});
			this->comboBox_P16Assist->Location = System::Drawing::Point(66, 46);
			this->comboBox_P16Assist->Name = L"comboBox_P16Assist";
			this->comboBox_P16Assist->Size = System::Drawing::Size(110, 21);
			this->comboBox_P16Assist->TabIndex = 24;
			this->comboBox_P16Assist->Text = L"Assist Off";
			this->comboBox_P16Assist->SelectedIndexChanged += gcnew System::EventHandler(this, &LEVDisplay::comboBox_P16Assist_SelectedIndexChanged);
			// 
			// comboBox_P16Regen
			// 
			this->comboBox_P16Regen->FormattingEnabled = true;
			this->comboBox_P16Regen->Items->AddRange(gcnew cli::array< System::Object^  >(8) {L"Regenerative Off", L"Regenerative 1", 
				L"Regenerative 2", L"Regenerative 3", L"Regenerative 4", L"Regenerative 5", L"Regenerative 6", L"Regenerative 7"});
			this->comboBox_P16Regen->Location = System::Drawing::Point(198, 46);
			this->comboBox_P16Regen->Name = L"comboBox_P16Regen";
			this->comboBox_P16Regen->Size = System::Drawing::Size(110, 21);
			this->comboBox_P16Regen->TabIndex = 23;
			this->comboBox_P16Regen->Text = L"Regenerative Off";
			this->comboBox_P16Regen->SelectedIndexChanged += gcnew System::EventHandler(this, &LEVDisplay::comboBox_P16Regen_SelectedIndexChanged);
			// 
			// tabPage_P16Command
			// 
			this->tabPage_P16Command->Controls->Add(this->groupBox_P16CurrentGear);
			this->tabPage_P16Command->Controls->Add(this->checkBox_P16RightSignalOn);
			this->tabPage_P16Command->Controls->Add(this->checkBox_P16LeftSignalOn);
			this->tabPage_P16Command->Controls->Add(this->checkBox_P16HighBeamOn);
			this->tabPage_P16Command->Controls->Add(this->checkBox_P16LightOn);
			this->tabPage_P16Command->Location = System::Drawing::Point(4, 22);
			this->tabPage_P16Command->Name = L"tabPage_P16Command";
			this->tabPage_P16Command->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_P16Command->Size = System::Drawing::Size(375, 77);
			this->tabPage_P16Command->TabIndex = 2;
			this->tabPage_P16Command->Text = L"Display Command";
			this->tabPage_P16Command->UseVisualStyleBackColor = true;
			// 
			// groupBox_P16CurrentGear
			// 
			this->groupBox_P16CurrentGear->Controls->Add(this->label_P16Front);
			this->groupBox_P16CurrentGear->Controls->Add(this->comboBox_P16FrontGear);
			this->groupBox_P16CurrentGear->Controls->Add(this->label_P16Rear);
			this->groupBox_P16CurrentGear->Controls->Add(this->comboBox_P16RearGear);
			this->groupBox_P16CurrentGear->Location = System::Drawing::Point(4, 5);
			this->groupBox_P16CurrentGear->Name = L"groupBox_P16CurrentGear";
			this->groupBox_P16CurrentGear->Size = System::Drawing::Size(133, 67);
			this->groupBox_P16CurrentGear->TabIndex = 32;
			this->groupBox_P16CurrentGear->TabStop = false;
			this->groupBox_P16CurrentGear->Text = L"Current Gear";
			// 
			// label_P16Front
			// 
			this->label_P16Front->AutoSize = true;
			this->label_P16Front->Location = System::Drawing::Point(7, 18);
			this->label_P16Front->Name = L"label_P16Front";
			this->label_P16Front->Size = System::Drawing::Size(60, 13);
			this->label_P16Front->TabIndex = 21;
			this->label_P16Front->Text = L"Front Gear:";
			// 
			// comboBox_P16FrontGear
			// 
			this->comboBox_P16FrontGear->FormattingEnabled = true;
			this->comboBox_P16FrontGear->Items->AddRange(gcnew cli::array< System::Object^  >(4) {L"none", L"1", L"2", L"3"});
			this->comboBox_P16FrontGear->Location = System::Drawing::Point(73, 15);
			this->comboBox_P16FrontGear->Name = L"comboBox_P16FrontGear";
			this->comboBox_P16FrontGear->Size = System::Drawing::Size(47, 21);
			this->comboBox_P16FrontGear->TabIndex = 19;
			this->comboBox_P16FrontGear->Text = L"none";
			this->comboBox_P16FrontGear->SelectedIndexChanged += gcnew System::EventHandler(this, &LEVDisplay::comboBox_P16FrontGear_SelectedIndexChanged);
			// 
			// label_P16Rear
			// 
			this->label_P16Rear->AutoSize = true;
			this->label_P16Rear->Location = System::Drawing::Point(8, 43);
			this->label_P16Rear->Name = L"label_P16Rear";
			this->label_P16Rear->Size = System::Drawing::Size(59, 13);
			this->label_P16Rear->TabIndex = 22;
			this->label_P16Rear->Text = L"Rear Gear:";
			// 
			// comboBox_P16RearGear
			// 
			this->comboBox_P16RearGear->FormattingEnabled = true;
			this->comboBox_P16RearGear->Items->AddRange(gcnew cli::array< System::Object^  >(16) {L"none", L"1", L"2", L"3", L"4", L"5", 
				L"6", L"7", L"8", L"9", L"10", L"11", L"12", L"13", L"14", L"15"});
			this->comboBox_P16RearGear->Location = System::Drawing::Point(73, 40);
			this->comboBox_P16RearGear->Name = L"comboBox_P16RearGear";
			this->comboBox_P16RearGear->Size = System::Drawing::Size(47, 21);
			this->comboBox_P16RearGear->TabIndex = 20;
			this->comboBox_P16RearGear->Text = L"none";
			this->comboBox_P16RearGear->SelectedIndexChanged += gcnew System::EventHandler(this, &LEVDisplay::comboBox_P16RearGear_SelectedIndexChanged);
			// 
			// checkBox_P16RightSignalOn
			// 
			this->checkBox_P16RightSignalOn->AutoSize = true;
			this->checkBox_P16RightSignalOn->Location = System::Drawing::Point(245, 43);
			this->checkBox_P16RightSignalOn->Name = L"checkBox_P16RightSignalOn";
			this->checkBox_P16RightSignalOn->Size = System::Drawing::Size(125, 17);
			this->checkBox_P16RightSignalOn->TabIndex = 31;
			this->checkBox_P16RightSignalOn->Text = L"Right Turn Signal On";
			this->checkBox_P16RightSignalOn->UseVisualStyleBackColor = true;
			this->checkBox_P16RightSignalOn->CheckedChanged += gcnew System::EventHandler(this, &LEVDisplay::checkBox_P16RightSignalOn_CheckedChanged);
			// 
			// checkBox_P16LeftSignalOn
			// 
			this->checkBox_P16LeftSignalOn->AutoSize = true;
			this->checkBox_P16LeftSignalOn->Location = System::Drawing::Point(245, 20);
			this->checkBox_P16LeftSignalOn->Name = L"checkBox_P16LeftSignalOn";
			this->checkBox_P16LeftSignalOn->Size = System::Drawing::Size(118, 17);
			this->checkBox_P16LeftSignalOn->TabIndex = 30;
			this->checkBox_P16LeftSignalOn->Text = L"Left Turn Signal On";
			this->checkBox_P16LeftSignalOn->UseVisualStyleBackColor = true;
			this->checkBox_P16LeftSignalOn->CheckedChanged += gcnew System::EventHandler(this, &LEVDisplay::checkBox_P16LeftSignalOn_CheckedChanged);
			// 
			// checkBox_P16HighBeamOn
			// 
			this->checkBox_P16HighBeamOn->AutoSize = true;
			this->checkBox_P16HighBeamOn->Location = System::Drawing::Point(147, 20);
			this->checkBox_P16HighBeamOn->Name = L"checkBox_P16HighBeamOn";
			this->checkBox_P16HighBeamOn->Size = System::Drawing::Size(95, 17);
			this->checkBox_P16HighBeamOn->TabIndex = 29;
			this->checkBox_P16HighBeamOn->Text = L"High Beam On";
			this->checkBox_P16HighBeamOn->UseVisualStyleBackColor = true;
			this->checkBox_P16HighBeamOn->CheckedChanged += gcnew System::EventHandler(this, &LEVDisplay::checkBox_P16HighBeamOn_CheckedChanged);
			// 
			// checkBox_P16LightOn
			// 
			this->checkBox_P16LightOn->AutoSize = true;
			this->checkBox_P16LightOn->Location = System::Drawing::Point(147, 43);
			this->checkBox_P16LightOn->Name = L"checkBox_P16LightOn";
			this->checkBox_P16LightOn->Size = System::Drawing::Size(66, 17);
			this->checkBox_P16LightOn->TabIndex = 28;
			this->checkBox_P16LightOn->Text = L"Light On";
			this->checkBox_P16LightOn->UseVisualStyleBackColor = true;
			this->checkBox_P16LightOn->CheckedChanged += gcnew System::EventHandler(this, &LEVDisplay::checkBox_P16LightOn_CheckedChanged);
			// 
			// tabPage_P16ManID
			// 
			this->tabPage_P16ManID->Controls->Add(this->label_P16ManID);
			this->tabPage_P16ManID->Controls->Add(this->numericUpDown_P16ManID);
			this->tabPage_P16ManID->Location = System::Drawing::Point(4, 22);
			this->tabPage_P16ManID->Name = L"tabPage_P16ManID";
			this->tabPage_P16ManID->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_P16ManID->Size = System::Drawing::Size(375, 77);
			this->tabPage_P16ManID->TabIndex = 3;
			this->tabPage_P16ManID->Text = L"Manufacturer ID";
			this->tabPage_P16ManID->UseVisualStyleBackColor = true;
			// 
			// label_P16ManID
			// 
			this->label_P16ManID->AutoSize = true;
			this->label_P16ManID->Location = System::Drawing::Point(95, 30);
			this->label_P16ManID->Name = L"label_P16ManID";
			this->label_P16ManID->Size = System::Drawing::Size(124, 13);
			this->label_P16ManID->TabIndex = 3;
			this->label_P16ManID->Text = L"Display Manufacturer ID:";
			// 
			// numericUpDown_P16ManID
			// 
			this->numericUpDown_P16ManID->Location = System::Drawing::Point(225, 28);
			this->numericUpDown_P16ManID->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {65535, 0, 0, 0});
			this->numericUpDown_P16ManID->Name = L"numericUpDown_P16ManID";
			this->numericUpDown_P16ManID->Size = System::Drawing::Size(54, 20);
			this->numericUpDown_P16ManID->TabIndex = 2;
			this->numericUpDown_P16ManID->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {65535, 0, 0, 0});
			this->numericUpDown_P16ManID->ValueChanged += gcnew System::EventHandler(this, &LEVDisplay::numericUpDown_P16ManID_ValueChanged);
			// 
			// tabPage_P16Wheel
			// 
			this->tabPage_P16Wheel->Controls->Add(this->numericUpDown_P16Circum);
			this->tabPage_P16Wheel->Controls->Add(this->label_P16Circum);
			this->tabPage_P16Wheel->Location = System::Drawing::Point(4, 22);
			this->tabPage_P16Wheel->Name = L"tabPage_P16Wheel";
			this->tabPage_P16Wheel->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_P16Wheel->Size = System::Drawing::Size(375, 77);
			this->tabPage_P16Wheel->TabIndex = 4;
			this->tabPage_P16Wheel->Text = L"Wheel";
			this->tabPage_P16Wheel->UseVisualStyleBackColor = true;
			// 
			// numericUpDown_P16Circum
			// 
			this->numericUpDown_P16Circum->Location = System::Drawing::Point(232, 28);
			this->numericUpDown_P16Circum->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {4095, 0, 0, 0});
			this->numericUpDown_P16Circum->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->numericUpDown_P16Circum->Name = L"numericUpDown_P16Circum";
			this->numericUpDown_P16Circum->Size = System::Drawing::Size(54, 20);
			this->numericUpDown_P16Circum->TabIndex = 18;
			this->numericUpDown_P16Circum->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1500, 0, 0, 0});
			this->numericUpDown_P16Circum->ValueChanged += gcnew System::EventHandler(this, &LEVDisplay::numericUpDown_P16Circum_ValueChanged);
			// 
			// label_P16Circum
			// 
			this->label_P16Circum->AutoSize = true;
			this->label_P16Circum->Location = System::Drawing::Point(89, 30);
			this->label_P16Circum->Name = L"label_P16Circum";
			this->label_P16Circum->Size = System::Drawing::Size(137, 13);
			this->label_P16Circum->TabIndex = 17;
			this->label_P16Circum->Text = L"Wheel Circumference (mm):";
			// 
			// tabPage_Common1
			// 
			this->tabPage_Common1->Controls->Add(this->label_Common1);
			this->tabPage_Common1->Controls->Add(this->groupBox_P81Rx);
			this->tabPage_Common1->Controls->Add(this->groupBox_P80Rx);
			this->tabPage_Common1->Location = System::Drawing::Point(4, 22);
			this->tabPage_Common1->Name = L"tabPage_Common1";
			this->tabPage_Common1->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_Common1->Size = System::Drawing::Size(389, 111);
			this->tabPage_Common1->TabIndex = 8;
			this->tabPage_Common1->Text = L"Common Rx";
			this->tabPage_Common1->UseVisualStyleBackColor = true;
			// 
			// label_Common1
			// 
			this->label_Common1->AutoSize = true;
			this->label_Common1->Location = System::Drawing::Point(6, 8);
			this->label_Common1->Name = L"label_Common1";
			this->label_Common1->Size = System::Drawing::Size(136, 13);
			this->label_Common1->TabIndex = 55;
			this->label_Common1->Text = L"LEV Sensor Common Data:";
			// 
			// groupBox_P81Rx
			// 
			this->groupBox_P81Rx->Controls->Add(this->label_Glb_SoftwareVer);
			this->groupBox_P81Rx->Controls->Add(this->label_Glb_SoftwareVerDisplay);
			this->groupBox_P81Rx->Controls->Add(this->label_Glb_SerialNumDisplay);
			this->groupBox_P81Rx->Controls->Add(this->label_Glb_SerialNum);
			this->groupBox_P81Rx->Location = System::Drawing::Point(197, 26);
			this->groupBox_P81Rx->Name = L"groupBox_P81Rx";
			this->groupBox_P81Rx->Size = System::Drawing::Size(173, 79);
			this->groupBox_P81Rx->TabIndex = 1;
			this->groupBox_P81Rx->TabStop = false;
			this->groupBox_P81Rx->Text = L"Page 81";
			// 
			// label_Glb_SoftwareVer
			// 
			this->label_Glb_SoftwareVer->AutoSize = true;
			this->label_Glb_SoftwareVer->Location = System::Drawing::Point(15, 42);
			this->label_Glb_SoftwareVer->Name = L"label_Glb_SoftwareVer";
			this->label_Glb_SoftwareVer->Size = System::Drawing::Size(63, 13);
			this->label_Glb_SoftwareVer->TabIndex = 28;
			this->label_Glb_SoftwareVer->Text = L"Sw Version:";
			// 
			// label_Glb_SoftwareVerDisplay
			// 
			this->label_Glb_SoftwareVerDisplay->AutoSize = true;
			this->label_Glb_SoftwareVerDisplay->Location = System::Drawing::Point(85, 42);
			this->label_Glb_SoftwareVerDisplay->Name = L"label_Glb_SoftwareVerDisplay";
			this->label_Glb_SoftwareVerDisplay->Size = System::Drawing::Size(16, 13);
			this->label_Glb_SoftwareVerDisplay->TabIndex = 30;
			this->label_Glb_SoftwareVerDisplay->Text = L"---";
			// 
			// label_Glb_SerialNumDisplay
			// 
			this->label_Glb_SerialNumDisplay->AutoSize = true;
			this->label_Glb_SerialNumDisplay->Location = System::Drawing::Point(85, 29);
			this->label_Glb_SerialNumDisplay->Name = L"label_Glb_SerialNumDisplay";
			this->label_Glb_SerialNumDisplay->Size = System::Drawing::Size(16, 13);
			this->label_Glb_SerialNumDisplay->TabIndex = 29;
			this->label_Glb_SerialNumDisplay->Text = L"---";
			// 
			// label_Glb_SerialNum
			// 
			this->label_Glb_SerialNum->AutoSize = true;
			this->label_Glb_SerialNum->Location = System::Drawing::Point(32, 29);
			this->label_Glb_SerialNum->Name = L"label_Glb_SerialNum";
			this->label_Glb_SerialNum->Size = System::Drawing::Size(46, 13);
			this->label_Glb_SerialNum->TabIndex = 27;
			this->label_Glb_SerialNum->Text = L"Serial #:";
			// 
			// groupBox_P80Rx
			// 
			this->groupBox_P80Rx->Controls->Add(this->label_Glb_HardwareVer);
			this->groupBox_P80Rx->Controls->Add(this->label_Glb_ModelNum);
			this->groupBox_P80Rx->Controls->Add(this->label_Glb_ManfIDDisplay);
			this->groupBox_P80Rx->Controls->Add(this->label_Glb_ModelNumDisplay);
			this->groupBox_P80Rx->Controls->Add(this->label_Glb_HardwareVerDisplay);
			this->groupBox_P80Rx->Controls->Add(this->label_Glb_ManfID);
			this->groupBox_P80Rx->Location = System::Drawing::Point(21, 26);
			this->groupBox_P80Rx->Name = L"groupBox_P80Rx";
			this->groupBox_P80Rx->Size = System::Drawing::Size(170, 79);
			this->groupBox_P80Rx->TabIndex = 0;
			this->groupBox_P80Rx->TabStop = false;
			this->groupBox_P80Rx->Text = L"Page 80";
			// 
			// label_Glb_HardwareVer
			// 
			this->label_Glb_HardwareVer->AutoSize = true;
			this->label_Glb_HardwareVer->Location = System::Drawing::Point(13, 49);
			this->label_Glb_HardwareVer->Name = L"label_Glb_HardwareVer";
			this->label_Glb_HardwareVer->Size = System::Drawing::Size(64, 13);
			this->label_Glb_HardwareVer->TabIndex = 27;
			this->label_Glb_HardwareVer->Text = L"Hw Version:";
			// 
			// label_Glb_ModelNum
			// 
			this->label_Glb_ModelNum->AutoSize = true;
			this->label_Glb_ModelNum->Location = System::Drawing::Point(28, 36);
			this->label_Glb_ModelNum->Name = L"label_Glb_ModelNum";
			this->label_Glb_ModelNum->Size = System::Drawing::Size(49, 13);
			this->label_Glb_ModelNum->TabIndex = 28;
			this->label_Glb_ModelNum->Text = L"Model #:";
			// 
			// label_Glb_ManfIDDisplay
			// 
			this->label_Glb_ManfIDDisplay->Location = System::Drawing::Point(83, 23);
			this->label_Glb_ManfIDDisplay->Name = L"label_Glb_ManfIDDisplay";
			this->label_Glb_ManfIDDisplay->Size = System::Drawing::Size(50, 13);
			this->label_Glb_ManfIDDisplay->TabIndex = 29;
			this->label_Glb_ManfIDDisplay->Text = L"---";
			// 
			// label_Glb_ModelNumDisplay
			// 
			this->label_Glb_ModelNumDisplay->Location = System::Drawing::Point(83, 36);
			this->label_Glb_ModelNumDisplay->Name = L"label_Glb_ModelNumDisplay";
			this->label_Glb_ModelNumDisplay->Size = System::Drawing::Size(50, 13);
			this->label_Glb_ModelNumDisplay->TabIndex = 30;
			this->label_Glb_ModelNumDisplay->Text = L"---";
			// 
			// label_Glb_HardwareVerDisplay
			// 
			this->label_Glb_HardwareVerDisplay->Location = System::Drawing::Point(83, 49);
			this->label_Glb_HardwareVerDisplay->Name = L"label_Glb_HardwareVerDisplay";
			this->label_Glb_HardwareVerDisplay->Size = System::Drawing::Size(27, 13);
			this->label_Glb_HardwareVerDisplay->TabIndex = 31;
			this->label_Glb_HardwareVerDisplay->Text = L"---";
			// 
			// label_Glb_ManfID
			// 
			this->label_Glb_ManfID->AutoSize = true;
			this->label_Glb_ManfID->Location = System::Drawing::Point(26, 23);
			this->label_Glb_ManfID->Name = L"label_Glb_ManfID";
			this->label_Glb_ManfID->Size = System::Drawing::Size(51, 13);
			this->label_Glb_ManfID->TabIndex = 26;
			this->label_Glb_ManfID->Text = L"Manf. ID:";
			// 
			// tabPage_Common2
			// 
			this->tabPage_Common2->Controls->Add(this->checkBox_NoSerial);
			this->tabPage_Common2->Controls->Add(this->label_Common2);
			this->tabPage_Common2->Controls->Add(this->label_SoftwareVer);
			this->tabPage_Common2->Controls->Add(this->label_HardwareVer);
			this->tabPage_Common2->Controls->Add(this->button_CommonUpdate);
			this->tabPage_Common2->Controls->Add(this->label_ErrorCommon);
			this->tabPage_Common2->Controls->Add(this->label_ManfID);
			this->tabPage_Common2->Controls->Add(this->label_SerialNum);
			this->tabPage_Common2->Controls->Add(this->textBox_SerialNum);
			this->tabPage_Common2->Controls->Add(this->textBox_SwVersion);
			this->tabPage_Common2->Controls->Add(this->textBox_HwVersion);
			this->tabPage_Common2->Controls->Add(this->textBox_ModelNum);
			this->tabPage_Common2->Controls->Add(this->textBox_MfgID);
			this->tabPage_Common2->Controls->Add(this->label_ModelNum);
			this->tabPage_Common2->Location = System::Drawing::Point(4, 22);
			this->tabPage_Common2->Name = L"tabPage_Common2";
			this->tabPage_Common2->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_Common2->Size = System::Drawing::Size(389, 111);
			this->tabPage_Common2->TabIndex = 9;
			this->tabPage_Common2->Text = L"Common Tx";
			this->tabPage_Common2->UseVisualStyleBackColor = true;
			// 
			// label_SoftwareVer
			// 
			this->label_SoftwareVer->AutoSize = true;
			this->label_SoftwareVer->Location = System::Drawing::Point(257, 54);
			this->label_SoftwareVer->Name = L"label_SoftwareVer";
			this->label_SoftwareVer->Size = System::Drawing::Size(71, 13);
			this->label_SoftwareVer->TabIndex = 57;
			this->label_SoftwareVer->Text = L"Software Ver:";
			// 
			// label_HardwareVer
			// 
			this->label_HardwareVer->AutoSize = true;
			this->label_HardwareVer->Location = System::Drawing::Point(253, 28);
			this->label_HardwareVer->Name = L"label_HardwareVer";
			this->label_HardwareVer->Size = System::Drawing::Size(75, 13);
			this->label_HardwareVer->TabIndex = 56;
			this->label_HardwareVer->Text = L"Hardware Ver:";
			// 
			// button_CommonUpdate
			// 
			this->button_CommonUpdate->Location = System::Drawing::Point(266, 77);
			this->button_CommonUpdate->Name = L"button_CommonUpdate";
			this->button_CommonUpdate->Size = System::Drawing::Size(97, 20);
			this->button_CommonUpdate->TabIndex = 65;
			this->button_CommonUpdate->Text = L"Update All";
			this->button_CommonUpdate->UseVisualStyleBackColor = true;
			this->button_CommonUpdate->Click += gcnew System::EventHandler(this, &LEVDisplay::button_CommonUpdate_Click);
			// 
			// label_ErrorCommon
			// 
			this->label_ErrorCommon->AutoSize = true;
			this->label_ErrorCommon->Location = System::Drawing::Point(153, 78);
			this->label_ErrorCommon->Name = L"label_ErrorCommon";
			this->label_ErrorCommon->Size = System::Drawing::Size(32, 13);
			this->label_ErrorCommon->TabIndex = 59;
			this->label_ErrorCommon->Text = L"Error:";
			this->label_ErrorCommon->Visible = false;
			// 
			// label_ManfID
			// 
			this->label_ManfID->AutoSize = true;
			this->label_ManfID->Location = System::Drawing::Point(14, 54);
			this->label_ManfID->Name = L"label_ManfID";
			this->label_ManfID->Size = System::Drawing::Size(51, 13);
			this->label_ManfID->TabIndex = 54;
			this->label_ManfID->Text = L"Manf. ID:";
			// 
			// label_SerialNum
			// 
			this->label_SerialNum->AutoSize = true;
			this->label_SerialNum->Location = System::Drawing::Point(19, 28);
			this->label_SerialNum->Name = L"label_SerialNum";
			this->label_SerialNum->Size = System::Drawing::Size(46, 13);
			this->label_SerialNum->TabIndex = 55;
			this->label_SerialNum->Text = L"Serial #:";
			// 
			// textBox_SerialNum
			// 
			this->textBox_SerialNum->Location = System::Drawing::Point(71, 25);
			this->textBox_SerialNum->MaxLength = 10;
			this->textBox_SerialNum->Name = L"textBox_SerialNum";
			this->textBox_SerialNum->Size = System::Drawing::Size(76, 20);
			this->textBox_SerialNum->TabIndex = 62;
			this->textBox_SerialNum->Text = L"1234567890";
			// 
			// textBox_SwVersion
			// 
			this->textBox_SwVersion->Location = System::Drawing::Point(334, 51);
			this->textBox_SwVersion->MaxLength = 3;
			this->textBox_SwVersion->Name = L"textBox_SwVersion";
			this->textBox_SwVersion->Size = System::Drawing::Size(29, 20);
			this->textBox_SwVersion->TabIndex = 64;
			this->textBox_SwVersion->Text = L"1";
			// 
			// textBox_HwVersion
			// 
			this->textBox_HwVersion->Location = System::Drawing::Point(334, 25);
			this->textBox_HwVersion->MaxLength = 3;
			this->textBox_HwVersion->Name = L"textBox_HwVersion";
			this->textBox_HwVersion->Size = System::Drawing::Size(29, 20);
			this->textBox_HwVersion->TabIndex = 63;
			this->textBox_HwVersion->Text = L"1";
			// 
			// textBox_ModelNum
			// 
			this->textBox_ModelNum->Location = System::Drawing::Point(98, 75);
			this->textBox_ModelNum->MaxLength = 5;
			this->textBox_ModelNum->Name = L"textBox_ModelNum";
			this->textBox_ModelNum->Size = System::Drawing::Size(49, 20);
			this->textBox_ModelNum->TabIndex = 61;
			this->textBox_ModelNum->Text = L"33669";
			// 
			// textBox_MfgID
			// 
			this->textBox_MfgID->Location = System::Drawing::Point(98, 51);
			this->textBox_MfgID->MaxLength = 5;
			this->textBox_MfgID->Name = L"textBox_MfgID";
			this->textBox_MfgID->Size = System::Drawing::Size(49, 20);
			this->textBox_MfgID->TabIndex = 60;
			this->textBox_MfgID->Text = L"54321";
			// 
			// label_ModelNum
			// 
			this->label_ModelNum->AutoSize = true;
			this->label_ModelNum->Location = System::Drawing::Point(16, 78);
			this->label_ModelNum->Name = L"label_ModelNum";
			this->label_ModelNum->Size = System::Drawing::Size(49, 13);
			this->label_ModelNum->TabIndex = 58;
			this->label_ModelNum->Text = L"Model #:";
			// 
			// tabPage_TravelModes
			// 
			this->tabPage_TravelModes->Controls->Add(this->numericUpDown_DisplayRegenModes);
			this->tabPage_TravelModes->Controls->Add(this->numericUpDown_DisplayAssistModes);
			this->tabPage_TravelModes->Controls->Add(this->label_RegenModesSupported);
			this->tabPage_TravelModes->Controls->Add(this->label_AssistModesSupported);
			this->tabPage_TravelModes->Controls->Add(this->label_SupportedTravelModes);
			this->tabPage_TravelModes->Location = System::Drawing::Point(4, 22);
			this->tabPage_TravelModes->Name = L"tabPage_TravelModes";
			this->tabPage_TravelModes->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_TravelModes->Size = System::Drawing::Size(389, 111);
			this->tabPage_TravelModes->TabIndex = 10;
			this->tabPage_TravelModes->Text = L"Travel Modes";
			this->tabPage_TravelModes->UseVisualStyleBackColor = true;
			// 
			// numericUpDown_DisplayRegenModes
			// 
			this->numericUpDown_DisplayRegenModes->Location = System::Drawing::Point(282, 57);
			this->numericUpDown_DisplayRegenModes->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {7, 0, 0, 0});
			this->numericUpDown_DisplayRegenModes->Name = L"numericUpDown_DisplayRegenModes";
			this->numericUpDown_DisplayRegenModes->Size = System::Drawing::Size(36, 20);
			this->numericUpDown_DisplayRegenModes->TabIndex = 4;
			this->numericUpDown_DisplayRegenModes->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {7, 0, 0, 0});
			this->numericUpDown_DisplayRegenModes->ValueChanged += gcnew System::EventHandler(this, &LEVDisplay::numericUpDown_DisplayRegenModes_ValueChanged);
			// 
			// numericUpDown_DisplayAssistModes
			// 
			this->numericUpDown_DisplayAssistModes->Location = System::Drawing::Point(131, 57);
			this->numericUpDown_DisplayAssistModes->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {7, 0, 0, 0});
			this->numericUpDown_DisplayAssistModes->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->numericUpDown_DisplayAssistModes->Name = L"numericUpDown_DisplayAssistModes";
			this->numericUpDown_DisplayAssistModes->Size = System::Drawing::Size(36, 20);
			this->numericUpDown_DisplayAssistModes->TabIndex = 3;
			this->numericUpDown_DisplayAssistModes->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {7, 0, 0, 0});
			this->numericUpDown_DisplayAssistModes->ValueChanged += gcnew System::EventHandler(this, &LEVDisplay::numericUpDown_DisplayAssistModes_ValueChanged);
			// 
			// label_RegenModesSupported
			// 
			this->label_RegenModesSupported->AutoSize = true;
			this->label_RegenModesSupported->Location = System::Drawing::Point(189, 59);
			this->label_RegenModesSupported->Name = L"label_RegenModesSupported";
			this->label_RegenModesSupported->Size = System::Drawing::Size(87, 13);
			this->label_RegenModesSupported->TabIndex = 2;
			this->label_RegenModesSupported->Text = L"# Regen Modes:";
			// 
			// label_AssistModesSupported
			// 
			this->label_AssistModesSupported->AutoSize = true;
			this->label_AssistModesSupported->Location = System::Drawing::Point(43, 59);
			this->label_AssistModesSupported->Name = L"label_AssistModesSupported";
			this->label_AssistModesSupported->Size = System::Drawing::Size(82, 13);
			this->label_AssistModesSupported->TabIndex = 1;
			this->label_AssistModesSupported->Text = L"# Assist Modes:";
			// 
			// label_SupportedTravelModes
			// 
			this->label_SupportedTravelModes->AutoSize = true;
			this->label_SupportedTravelModes->Location = System::Drawing::Point(98, 19);
			this->label_SupportedTravelModes->Name = L"label_SupportedTravelModes";
			this->label_SupportedTravelModes->Size = System::Drawing::Size(178, 13);
			this->label_SupportedTravelModes->TabIndex = 0;
			this->label_SupportedTravelModes->Text = L"Travel Modes Supported by Display:";
			// 
			// label_AckMsgStatus
			// 
			this->label_AckMsgStatus->AutoSize = true;
			this->label_AckMsgStatus->Location = System::Drawing::Point(127, 67);
			this->label_AckMsgStatus->Name = L"label_AckMsgStatus";
			this->label_AckMsgStatus->Size = System::Drawing::Size(31, 13);
			this->label_AckMsgStatus->TabIndex = 0;
			this->label_AckMsgStatus->Text = L"-- -- --";
			// 
			// panel_Display
			// 
			this->panel_Display->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->panel_Display->Controls->Add(this->label_AckMsgStatus);
			this->panel_Display->Controls->Add(this->label_AckStatus);
			this->panel_Display->Controls->Add(this->label_StatusAssistVal);
			this->panel_Display->Controls->Add(this->label_StatusAssist);
			this->panel_Display->Controls->Add(this->label_StatusSpeedVal);
			this->panel_Display->Controls->Add(this->label_StatusSpeed);
			this->panel_Display->Controls->Add(this->label_StatusOdoVal);
			this->panel_Display->Controls->Add(this->label_StatusOdometer);
			this->panel_Display->Controls->Add(this->label_RxStatusBox);
			this->panel_Display->Location = System::Drawing::Point(58, 188);
			this->panel_Display->Name = L"panel_Display";
			this->panel_Display->Size = System::Drawing::Size(200, 90);
			this->panel_Display->TabIndex = 1;
			// 
			// label_AckStatus
			// 
			this->label_AckStatus->AutoSize = true;
			this->label_AckStatus->Location = System::Drawing::Point(6, 67);
			this->label_AckStatus->Name = L"label_AckStatus";
			this->label_AckStatus->Size = System::Drawing::Size(85, 13);
			this->label_AckStatus->TabIndex = 23;
			this->label_AckStatus->Text = L"Ack Msg Status:";
			// 
			// label_StatusAssistVal
			// 
			this->label_StatusAssistVal->AutoSize = true;
			this->label_StatusAssistVal->Location = System::Drawing::Point(127, 52);
			this->label_StatusAssistVal->Name = L"label_StatusAssistVal";
			this->label_StatusAssistVal->Size = System::Drawing::Size(16, 13);
			this->label_StatusAssistVal->TabIndex = 22;
			this->label_StatusAssistVal->Text = L"---";
			// 
			// label_StatusAssist
			// 
			this->label_StatusAssist->AutoSize = true;
			this->label_StatusAssist->Location = System::Drawing::Point(6, 52);
			this->label_StatusAssist->Name = L"label_StatusAssist";
			this->label_StatusAssist->Size = System::Drawing::Size(48, 13);
			this->label_StatusAssist->TabIndex = 21;
			this->label_StatusAssist->Text = L"% Assist:";
			// 
			// label_StatusSpeedVal
			// 
			this->label_StatusSpeedVal->AutoSize = true;
			this->label_StatusSpeedVal->Location = System::Drawing::Point(127, 22);
			this->label_StatusSpeedVal->Name = L"label_StatusSpeedVal";
			this->label_StatusSpeedVal->Size = System::Drawing::Size(16, 13);
			this->label_StatusSpeedVal->TabIndex = 20;
			this->label_StatusSpeedVal->Text = L"---";
			// 
			// label_StatusSpeed
			// 
			this->label_StatusSpeed->AutoSize = true;
			this->label_StatusSpeed->Location = System::Drawing::Point(6, 22);
			this->label_StatusSpeed->Name = L"label_StatusSpeed";
			this->label_StatusSpeed->Size = System::Drawing::Size(93, 13);
			this->label_StatusSpeed->TabIndex = 19;
			this->label_StatusSpeed->Text = L"Speed (0.1 km/h):";
			// 
			// label_StatusOdoVal
			// 
			this->label_StatusOdoVal->AutoSize = true;
			this->label_StatusOdoVal->Location = System::Drawing::Point(127, 37);
			this->label_StatusOdoVal->Name = L"label_StatusOdoVal";
			this->label_StatusOdoVal->Size = System::Drawing::Size(16, 13);
			this->label_StatusOdoVal->TabIndex = 18;
			this->label_StatusOdoVal->Text = L"---";
			// 
			// label_StatusOdometer
			// 
			this->label_StatusOdometer->AutoSize = true;
			this->label_StatusOdometer->Location = System::Drawing::Point(6, 37);
			this->label_StatusOdometer->Name = L"label_StatusOdometer";
			this->label_StatusOdometer->Size = System::Drawing::Size(103, 13);
			this->label_StatusOdometer->TabIndex = 17;
			this->label_StatusOdometer->Text = L"Odometer (0.01 km):";
			// 
			// label_RxStatusBox
			// 
			this->label_RxStatusBox->AutoSize = true;
			this->label_RxStatusBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Underline, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->label_RxStatusBox->Location = System::Drawing::Point(3, 3);
			this->label_RxStatusBox->Name = L"label_RxStatusBox";
			this->label_RxStatusBox->Size = System::Drawing::Size(140, 13);
			this->label_RxStatusBox->TabIndex = 1;
			this->label_RxStatusBox->Text = L"Current LEV Rx Data (Raw):";
			// 
			// label_P16FrontGear
			// 
			this->label_P16FrontGear->AutoSize = true;
			this->label_P16FrontGear->Location = System::Drawing::Point(7, 18);
			this->label_P16FrontGear->Name = L"label_P16FrontGear";
			this->label_P16FrontGear->Size = System::Drawing::Size(60, 13);
			this->label_P16FrontGear->TabIndex = 21;
			this->label_P16FrontGear->Text = L"Front Gear:";
			// 
			// label_P16RearGear
			// 
			this->label_P16RearGear->AutoSize = true;
			this->label_P16RearGear->Location = System::Drawing::Point(8, 43);
			this->label_P16RearGear->Name = L"label_P16RearGear";
			this->label_P16RearGear->Size = System::Drawing::Size(59, 13);
			this->label_P16RearGear->TabIndex = 22;
			this->label_P16RearGear->Text = L"Rear Gear:";
			// 
			// label_Common2
			// 
			this->label_Common2->AutoSize = true;
			this->label_Common2->Location = System::Drawing::Point(6, 8);
			this->label_Common2->Name = L"label_Common2";
			this->label_Common2->Size = System::Drawing::Size(137, 13);
			this->label_Common2->TabIndex = 66;
			this->label_Common2->Text = L"LEV Display Common Data:";
			// 
			// checkBox_NoSerial
			// 
			this->checkBox_NoSerial->AutoSize = true;
			this->checkBox_NoSerial->Location = System::Drawing::Point(153, 27);
			this->checkBox_NoSerial->Name = L"checkBox_NoSerial";
			this->checkBox_NoSerial->Size = System::Drawing::Size(79, 17);
			this->checkBox_NoSerial->TabIndex = 67;
			this->checkBox_NoSerial->Text = L"No Serial #";
			this->checkBox_NoSerial->UseVisualStyleBackColor = true;
			this->checkBox_NoSerial->CheckedChanged += gcnew System::EventHandler(this, &LEVDisplay::checkBox_NoSerial_CheckedChanged);
			// 
			// LEVDisplay
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(794, 351);
			this->Controls->Add(this->panel_Display);
			this->Controls->Add(this->panel_Settings);
			this->Name = L"LEVDisplay";
			this->Text = L"Light Electric Vehicle (LEV)";
			this->panel_Settings->ResumeLayout(false);
			this->tabControl_LEVDisplay->ResumeLayout(false);
			this->tabPage_P1->ResumeLayout(false);
			this->tabPage_P1->PerformLayout();
			this->tabPage_P2->ResumeLayout(false);
			this->tabPage_P2->PerformLayout();
			this->tabPage_P3->ResumeLayout(false);
			this->tabPage_P3->PerformLayout();
			this->tabPage_Page45->ResumeLayout(false);
			this->tabControl_Page45->ResumeLayout(false);
			this->tabPage_Page4->ResumeLayout(false);
			this->tabPage_Page4->PerformLayout();
			this->tabPage_Page5->ResumeLayout(false);
			this->tabPage_Page5->PerformLayout();
			this->tabPage_RqPage->ResumeLayout(false);
			this->tabPage_RqPage->PerformLayout();
			this->groupBox_Request->ResumeLayout(false);
			this->groupBox_Request->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_RqTxTimes))->EndInit();
			this->tabPage_P16->ResumeLayout(false);
			this->tabControl_P16->ResumeLayout(false);
			this->tabPage_P16TravelMode->ResumeLayout(false);
			this->tabPage_P16Display->ResumeLayout(false);
			this->tabPage_P16Display->PerformLayout();
			this->tabPage_P16Command->ResumeLayout(false);
			this->tabPage_P16Command->PerformLayout();
			this->groupBox_P16CurrentGear->ResumeLayout(false);
			this->groupBox_P16CurrentGear->PerformLayout();
			this->tabPage_P16ManID->ResumeLayout(false);
			this->tabPage_P16ManID->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_P16ManID))->EndInit();
			this->tabPage_P16Wheel->ResumeLayout(false);
			this->tabPage_P16Wheel->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_P16Circum))->EndInit();
			this->tabPage_Common1->ResumeLayout(false);
			this->tabPage_Common1->PerformLayout();
			this->groupBox_P81Rx->ResumeLayout(false);
			this->groupBox_P81Rx->PerformLayout();
			this->groupBox_P80Rx->ResumeLayout(false);
			this->groupBox_P80Rx->PerformLayout();
			this->tabPage_Common2->ResumeLayout(false);
			this->tabPage_Common2->PerformLayout();
			this->tabPage_TravelModes->ResumeLayout(false);
			this->tabPage_TravelModes->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_DisplayRegenModes))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_DisplayAssistModes))->EndInit();
			this->panel_Display->ResumeLayout(false);
			this->panel_Display->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
};