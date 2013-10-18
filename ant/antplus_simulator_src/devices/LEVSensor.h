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
#include "antplus_lev.h"	            // Device specific class
#include "antplus_common.h"				// Include common pages (example usage on SDM and Bike Power)
#include "types.h"
#include "antdefines.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

public ref class LEVSensor : public System::Windows::Forms::Form, public ISimBase{
public:
	// The constructor may include the following parameters:
	// System::Timers::Timer^ channelTimer:  Timer handle, if using a timer (not required in most cases, since getTimerInterval gets called after onTimerTock on each tock)
	// dRequestAckMsg^ channelAckMsg:  Handle to delegate function to send acknowledged messages (only needed if the device needs to send acknowledged messages)
	// These parameters may or may not be included in the constructor, as needed
		LEVSensor(System::Timers::Timer^ channelTimer, dRequestAckMsg^ channelAckMsg){
			InitializeComponent();			
			LEVData = gcnew LEV();
			commonDataSensor = gcnew CommonData();
			commonDataDisplay = gcnew CommonData();
			requestAckMsg = channelAckMsg;	
			timerHandle = channelTimer;		
			InitializeSim();;
		}

		~LEVSensor(){
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
	virtual void onTimerTock(USHORT eventTime);							// Called every simulator event.  Empty function for most receivers (timer is disabled on receivers unless explicitly enabled)
	virtual void ANT_eventNotification(UCHAR ucEventCode_, UCHAR* pucEventBuffer_);	 // Channel event state machine
	virtual UCHAR getDeviceType(){return LEV::DEVICE_TYPE;}  	
	virtual UCHAR getTransmissionType(){return LEV::TX_TYPE;} 	
	virtual USHORT getTransmitPeriod(){return LEV::MSG_PERIOD;}	
	virtual DOUBLE getTimerInterval(){return (double) ulTimerInterval;}	// Set to any value for receivers not using the timer.
	virtual System::Windows::Forms::Panel^ getSimSettingsPanel(){return this->panel_Settings;}
	virtual System::Windows::Forms::Panel^ getSimTranslatedDisplay(){return this->panel_Display;}

// Optional methods (not required by interface class)
private:
	void InitializeSim();						  // Initialize simulation
	void HandleTransmit(UCHAR* pucTxBuffer_);     // Handles the data page transmissions 
	void UpdateDisplay();						  // Updates the UI
	void HandleReceive(UCHAR* pucRxBuffer_);      // Handles incoming data pages
	//BOOL HandleRetransmit();					  // Re-transmits an ACK
	void UpdateAssistModeState();				  // Updates the Assist Mode
	void UpdateRegenModeState();				  // Updates the Regen Mode
	void UpdateFrontGearState();				  // Updates the front gear
	void UpdateRearGearState();					  // Updates the rear gear
	void UpdateBatteryTemp();					  // Updates the battery temp
	void UpdateMotorTemp();						  // Updates the motor temp
	void MapAssistModes();						  // Maps incoming Page 16 data to appropriate Assist Mode based on number of LEV supported modes 	
	void MapRegenModes();						  // Maps incoming Page 16 data to appropriate Regen Mode based on number of LEV supported modes
	void CheckAssistMapping();
	void CheckRegenMapping();
	void UpdateP1Assist();
	void UpdateP1Regen();

	// Add other GUI specific functions here
	System::Void numericUpDwn_P5AssistModes_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_P5RegenModes_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void comboBox_P1AssistLevel_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void comboBox_P1RegenLevel_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_P1HighBeam_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_P1LightOn_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_P1LeftSignalOn_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_P1RightSignalOn_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_P1Throttle_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void comboBox_P1FrontGear_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void comboBox_P1RearGear_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_P1GearsExist_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_P1ManualGears_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void radioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_P1CustomError_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_P1Speed_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_P2Speed_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_P3PercentAssist_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void button_P2UpdateDist_Click(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_P1TempUnused_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_P3Charge_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_P3BattEmpty_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_P4Voltage_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_P4VoltUnused_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_P2Range_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_P2RangeUnused_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_P16Supported_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void button_CommonUpdate_Click(System::Object^  sender, System::EventArgs^  e);
	System::Void comboBox_P1BattTemp_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void comboBox_P1MotorTemp_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_P1BattOverheat_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_P1MotorOverheat_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_P4ChargeCycle_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_P4FuelConsumption_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_P4DistanceOnCharge_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_P5Circum_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_P5WheelCircum_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_P4ChargeCycleUnused_CheckedChanged(System::Object^  sender, System::EventArgs^  e); 
	System::Void checkBox_P4FuelConsumptionUnused_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_P4DistanceOnChargeUnused_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_UsePage34_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_NoSerial_CheckedChanged(System::Object^  sender, System::EventArgs^  e);


private:
	dRequestAckMsg^ requestAckMsg;		// Delegate handle (ACK messages, only if used)
	System::Timers::Timer^ timerHandle;	// Timer handle (only if there is a timer)
	ULONG  ulTimerInterval;				// Timer interval between simulated events (only if there is a timer)
	LEV^ LEVData;                       // LEV class variable
	CommonData^ commonDataSensor;		// Hold the data for the sensor (what we Tx)
	CommonData^ commonDataDisplay;		// Hold the data for the display (what we Rx)

	// Simulator Variables
	double dbDispAcumDist;      // Cumulative distance (m)
	BOOL bPage16Supported;		// Flag for handling or ignoring incoming Page 16

	// make sure you are actually using these
	USHORT usLastEventTime;     // used to help calculate distance on each event
	UCHAR ucMsgExpectingAck;	// Message pending to be acknowledged
	UCHAR ucAckRetryCount;		// Counts the number of retries attempted for sending an ACK message
	UCHAR ucMaxAckRetries;	    // Maximum number of times for the sensor to attempt to resend an ack message if it does not rx an ack

	UCHAR ucRequestPage;	 // the page to transmit
	UCHAR ucRequestCount;    // the number of times to re-transmit
	BOOL bTxRequestFlag;	
	BOOL bTxPage34;

private: System::Windows::Forms::Panel^  panel_Settings;
private: System::Windows::Forms::Panel^  panel_Display;
private: System::Windows::Forms::TabPage^  tabPage_P16TravelMode;
private: System::Windows::Forms::CheckBox^  checkBox_P16TravelMode;
private: System::Windows::Forms::Label^  label_P16Regen;
private: System::Windows::Forms::Label^  label_P16Assist;
private: System::Windows::Forms::ComboBox^  comboBox_P16Assist;
private: System::Windows::Forms::ComboBox^  comboBox_P16Regen;
private: System::Windows::Forms::TabPage^  tabPage_P16Command;
private: System::Windows::Forms::GroupBox^  groupBox_P16Gear;
private: System::Windows::Forms::Label^  label_P16FrontGear;
private: System::Windows::Forms::ComboBox^  comboBox_P16FrontGear;
private: System::Windows::Forms::Label^  label_P16RearGear;
private: System::Windows::Forms::ComboBox^  comboBox_P16RearGear;
private: System::Windows::Forms::CheckBox^  checkBox_P16RightTurnOn;
private: System::Windows::Forms::CheckBox^  checkBox_P16LeftTurnOn;
private: System::Windows::Forms::CheckBox^  checkBox_P16HighBeamOn;
private: System::Windows::Forms::CheckBox^  checkBox_P16LightOn;
private: System::Windows::Forms::TabPage^  tabPage_P16ManID;
private: System::Windows::Forms::Label^  label_P16ManID;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_P16ManID;
private: System::Windows::Forms::Label^  label_StatusAssistVal;
private: System::Windows::Forms::Label^  label_StatusAssist;
private: System::Windows::Forms::Label^  label_StatusSpeedVal;
private: System::Windows::Forms::Label^  label_StatusSpeed;
private: System::Windows::Forms::Label^  label_StatusOdoVal;
private: System::Windows::Forms::Label^  label_StatusOdometer;
private: System::Windows::Forms::Label^  label_TxStatusBox;
private: System::Windows::Forms::TabControl^  tabControl_LEV;
private: System::Windows::Forms::TabPage^  tabPage_P1;
private: System::Windows::Forms::TabControl^  tabControl_P1;
private: System::Windows::Forms::TabPage^  tabPage_P1Mode;
private: System::Windows::Forms::Label^  label_P1RegenLevel;
private: System::Windows::Forms::Label^  label_P1AssistLevel;
private: System::Windows::Forms::ComboBox^  comboBox_P1AssistLevel;
private: System::Windows::Forms::ComboBox^  comboBox_P1RegenLevel;
private: System::Windows::Forms::TabPage^  tabPage_P1State;
private: System::Windows::Forms::CheckBox^  checkBox_P1RightSignalOn;
private: System::Windows::Forms::CheckBox^  checkBox_P1LeftSignalOn;
private: System::Windows::Forms::CheckBox^  checkBox_P1HighBeam;
private: System::Windows::Forms::CheckBox^  checkBox_P1LightOn;
private: System::Windows::Forms::TabPage^  tabPage_P1Gear;
private: System::Windows::Forms::CheckBox^  checkBox_P1ManualGears;
private: System::Windows::Forms::CheckBox^  checkBox_P1GearsExist;
private: System::Windows::Forms::GroupBox^  groupBox_P1CurrentGear;
private: System::Windows::Forms::Label^  label_P1FrontGear;
private: System::Windows::Forms::ComboBox^  comboBox_P1FrontGear;
private: System::Windows::Forms::Label^  label_P1RearGear;
private: System::Windows::Forms::ComboBox^  comboBox_P1RearGear;
private: System::Windows::Forms::TabPage^  tabPage_P1Error;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_P1CustomError;
private: System::Windows::Forms::RadioButton^  radioButton_P1CustomError;
private: System::Windows::Forms::RadioButton^  radioButton_P1Overheat;
private: System::Windows::Forms::RadioButton^  radioButton_P1EndLife;
private: System::Windows::Forms::RadioButton^  radioButton_P1TrainError;
private: System::Windows::Forms::RadioButton^  radioButton_P1BatteryError;
private: System::Windows::Forms::RadioButton^  radioButton_P1NoError;
private: System::Windows::Forms::TabPage^  tabPage_P1Speed;
private: System::Windows::Forms::Label^  label_P1Speed;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_P1Speed;
private: System::Windows::Forms::TabPage^  tabPage_P1Temp;
private: System::Windows::Forms::CheckBox^  checkBox_P1TempUnused;
private: System::Windows::Forms::CheckBox^  checkBox_P1MotorOverheat;
private: System::Windows::Forms::CheckBox^  checkBox_P1BattOverheat;
private: System::Windows::Forms::Label^  label_P1MotorTemp;
private: System::Windows::Forms::Label^  label_P1BattTemp;
private: System::Windows::Forms::ComboBox^  comboBox_P1MotorTemp;
private: System::Windows::Forms::ComboBox^  comboBox_P1BattTemp;
private: System::Windows::Forms::TabPage^  tabPage_P2;
private: System::Windows::Forms::Label^  label_P2Range;
private: System::Windows::Forms::Label^  label_P2Speed;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_P2Speed;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_P2Range;
private: System::Windows::Forms::CheckBox^  checkBox_P2RangeUnused;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_P2Distance;
private: System::Windows::Forms::Label^  label_P2TotalDistance;
private: System::Windows::Forms::Label^  label_P2TotDistDisplay;
private: System::Windows::Forms::Button^  button_P2UpdateDist;
private: System::Windows::Forms::Label^  label_P2UpdateError;
private: System::Windows::Forms::TabPage^  tabPage_P3;
private: System::Windows::Forms::TabControl^  tabControl_Page3;
private: System::Windows::Forms::TabPage^  tabPage_P3BatterySOC;
private: System::Windows::Forms::GroupBox^  groupBox_P3Charge;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_P3Charge;
private: System::Windows::Forms::CheckBox^  checkBox_P3BattEmpty;
private: System::Windows::Forms::Label^  label_P3Note2;
private: System::Windows::Forms::Label^  label_P3Note1;
private: System::Windows::Forms::TabPage^  tabPage_P3Assist;
private: System::Windows::Forms::Label^  label_P3PercentAssist;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_P3PercentAssist;
private: System::Windows::Forms::TabPage^  tabPage4;
private: System::Windows::Forms::TabControl^  tabControl_Page45;
private: System::Windows::Forms::TabPage^  tabPage_Page4;
private: System::Windows::Forms::Label^  label_P4DistanceOnCharge;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_P4DistanceOnCharge;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_P4FuelConsumption;
private: System::Windows::Forms::Label^  label_P4FuelConsumption;
private: System::Windows::Forms::Label^  label_P4ChargeCycle;
private: System::Windows::Forms::TabPage^  tabPage_Page5;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_P5Circum;
private: System::Windows::Forms::Label^  label_P5Circum;
private: System::Windows::Forms::Label^  label_P5RegenModes;
private: System::Windows::Forms::Label^  label_P5AssistModes;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_P5RegenModes;
private: System::Windows::Forms::NumericUpDown^  numericUpDwn_P5AssistModes;
private: System::Windows::Forms::TabPage^  tabPage_P16;
private: System::Windows::Forms::Label^  label_P16ManIDValue;
private: System::Windows::Forms::Label^  label_P16ManuID;
private: System::Windows::Forms::CheckBox^  checkBox_P16Supported;
private: System::Windows::Forms::TabPage^  tabPage_Common1;
private: System::Windows::Forms::Label^  label_Common1;
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
private: System::Windows::Forms::TabPage^  tabPage_Common2;
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
private: System::Windows::Forms::Label^  label_Common2;
private: System::Windows::Forms::CheckBox^  checkBox_P1Throttle;
private: System::Windows::Forms::CheckBox^  checkBox_P5WheelCircum;
private: System::Windows::Forms::TabPage^  tabPage_Page4Cont;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_P4Voltage;
private: System::Windows::Forms::CheckBox^  checkBox_P4VoltUnused;
private: System::Windows::Forms::CheckBox^  checkBox_P4ChargeCycleUnused;
private: System::Windows::Forms::CheckBox^  checkBox_P4FuelConsumptionUnused;
private: System::Windows::Forms::CheckBox^  checkBox_P4DistanceOnChargeUnused;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_P4ChargeCycle;
private: System::Windows::Forms::Label^  label_P5;
private: System::Windows::Forms::GroupBox^  groupBox_P4Voltage;
private: System::Windows::Forms::CheckBox^  checkBox_UsePage34;
private: System::Windows::Forms::CheckBox^  checkBox_NoSerial;


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
			this->tabControl_LEV = (gcnew System::Windows::Forms::TabControl());
			this->tabPage_P1 = (gcnew System::Windows::Forms::TabPage());
			this->tabControl_P1 = (gcnew System::Windows::Forms::TabControl());
			this->tabPage_P1Mode = (gcnew System::Windows::Forms::TabPage());
			this->label_P1RegenLevel = (gcnew System::Windows::Forms::Label());
			this->label_P1AssistLevel = (gcnew System::Windows::Forms::Label());
			this->comboBox_P1AssistLevel = (gcnew System::Windows::Forms::ComboBox());
			this->comboBox_P1RegenLevel = (gcnew System::Windows::Forms::ComboBox());
			this->tabPage_P1State = (gcnew System::Windows::Forms::TabPage());
			this->checkBox_P1Throttle = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox_P1RightSignalOn = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox_P1LeftSignalOn = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox_P1HighBeam = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox_P1LightOn = (gcnew System::Windows::Forms::CheckBox());
			this->tabPage_P1Gear = (gcnew System::Windows::Forms::TabPage());
			this->checkBox_P1ManualGears = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox_P1GearsExist = (gcnew System::Windows::Forms::CheckBox());
			this->groupBox_P1CurrentGear = (gcnew System::Windows::Forms::GroupBox());
			this->label_P1FrontGear = (gcnew System::Windows::Forms::Label());
			this->comboBox_P1FrontGear = (gcnew System::Windows::Forms::ComboBox());
			this->label_P1RearGear = (gcnew System::Windows::Forms::Label());
			this->comboBox_P1RearGear = (gcnew System::Windows::Forms::ComboBox());
			this->tabPage_P1Error = (gcnew System::Windows::Forms::TabPage());
			this->numericUpDown_P1CustomError = (gcnew System::Windows::Forms::NumericUpDown());
			this->radioButton_P1CustomError = (gcnew System::Windows::Forms::RadioButton());
			this->radioButton_P1Overheat = (gcnew System::Windows::Forms::RadioButton());
			this->radioButton_P1EndLife = (gcnew System::Windows::Forms::RadioButton());
			this->radioButton_P1TrainError = (gcnew System::Windows::Forms::RadioButton());
			this->radioButton_P1BatteryError = (gcnew System::Windows::Forms::RadioButton());
			this->radioButton_P1NoError = (gcnew System::Windows::Forms::RadioButton());
			this->tabPage_P1Speed = (gcnew System::Windows::Forms::TabPage());
			this->label_P1Speed = (gcnew System::Windows::Forms::Label());
			this->numericUpDown_P1Speed = (gcnew System::Windows::Forms::NumericUpDown());
			this->tabPage_P1Temp = (gcnew System::Windows::Forms::TabPage());
			this->checkBox_P1TempUnused = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox_P1MotorOverheat = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox_P1BattOverheat = (gcnew System::Windows::Forms::CheckBox());
			this->label_P1MotorTemp = (gcnew System::Windows::Forms::Label());
			this->label_P1BattTemp = (gcnew System::Windows::Forms::Label());
			this->comboBox_P1MotorTemp = (gcnew System::Windows::Forms::ComboBox());
			this->comboBox_P1BattTemp = (gcnew System::Windows::Forms::ComboBox());
			this->tabPage_P2 = (gcnew System::Windows::Forms::TabPage());
			this->checkBox_UsePage34 = (gcnew System::Windows::Forms::CheckBox());
			this->label_P2Range = (gcnew System::Windows::Forms::Label());
			this->label_P2Speed = (gcnew System::Windows::Forms::Label());
			this->numericUpDown_P2Speed = (gcnew System::Windows::Forms::NumericUpDown());
			this->numericUpDown_P2Range = (gcnew System::Windows::Forms::NumericUpDown());
			this->checkBox_P2RangeUnused = (gcnew System::Windows::Forms::CheckBox());
			this->numericUpDown_P2Distance = (gcnew System::Windows::Forms::NumericUpDown());
			this->label_P2TotalDistance = (gcnew System::Windows::Forms::Label());
			this->label_P2TotDistDisplay = (gcnew System::Windows::Forms::Label());
			this->button_P2UpdateDist = (gcnew System::Windows::Forms::Button());
			this->label_P2UpdateError = (gcnew System::Windows::Forms::Label());
			this->tabPage_P3 = (gcnew System::Windows::Forms::TabPage());
			this->tabControl_Page3 = (gcnew System::Windows::Forms::TabControl());
			this->tabPage_P3BatterySOC = (gcnew System::Windows::Forms::TabPage());
			this->groupBox_P3Charge = (gcnew System::Windows::Forms::GroupBox());
			this->numericUpDown_P3Charge = (gcnew System::Windows::Forms::NumericUpDown());
			this->checkBox_P3BattEmpty = (gcnew System::Windows::Forms::CheckBox());
			this->label_P3Note2 = (gcnew System::Windows::Forms::Label());
			this->label_P3Note1 = (gcnew System::Windows::Forms::Label());
			this->tabPage_P3Assist = (gcnew System::Windows::Forms::TabPage());
			this->label_P3PercentAssist = (gcnew System::Windows::Forms::Label());
			this->numericUpDown_P3PercentAssist = (gcnew System::Windows::Forms::NumericUpDown());
			this->tabPage4 = (gcnew System::Windows::Forms::TabPage());
			this->tabControl_Page45 = (gcnew System::Windows::Forms::TabControl());
			this->tabPage_Page4 = (gcnew System::Windows::Forms::TabPage());
			this->numericUpDown_P4ChargeCycle = (gcnew System::Windows::Forms::NumericUpDown());
			this->checkBox_P4DistanceOnChargeUnused = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox_P4FuelConsumptionUnused = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox_P4ChargeCycleUnused = (gcnew System::Windows::Forms::CheckBox());
			this->label_P4DistanceOnCharge = (gcnew System::Windows::Forms::Label());
			this->numericUpDown_P4DistanceOnCharge = (gcnew System::Windows::Forms::NumericUpDown());
			this->numericUpDown_P4FuelConsumption = (gcnew System::Windows::Forms::NumericUpDown());
			this->label_P4FuelConsumption = (gcnew System::Windows::Forms::Label());
			this->label_P4ChargeCycle = (gcnew System::Windows::Forms::Label());
			this->tabPage_Page4Cont = (gcnew System::Windows::Forms::TabPage());
			this->groupBox_P4Voltage = (gcnew System::Windows::Forms::GroupBox());
			this->numericUpDown_P4Voltage = (gcnew System::Windows::Forms::NumericUpDown());
			this->checkBox_P4VoltUnused = (gcnew System::Windows::Forms::CheckBox());
			this->tabPage_Page5 = (gcnew System::Windows::Forms::TabPage());
			this->label_P5 = (gcnew System::Windows::Forms::Label());
			this->checkBox_P5WheelCircum = (gcnew System::Windows::Forms::CheckBox());
			this->numericUpDown_P5Circum = (gcnew System::Windows::Forms::NumericUpDown());
			this->label_P5Circum = (gcnew System::Windows::Forms::Label());
			this->label_P5RegenModes = (gcnew System::Windows::Forms::Label());
			this->label_P5AssistModes = (gcnew System::Windows::Forms::Label());
			this->numericUpDown_P5RegenModes = (gcnew System::Windows::Forms::NumericUpDown());
			this->numericUpDwn_P5AssistModes = (gcnew System::Windows::Forms::NumericUpDown());
			this->tabPage_P16 = (gcnew System::Windows::Forms::TabPage());
			this->label_P16ManIDValue = (gcnew System::Windows::Forms::Label());
			this->label_P16ManuID = (gcnew System::Windows::Forms::Label());
			this->checkBox_P16Supported = (gcnew System::Windows::Forms::CheckBox());
			this->tabPage_Common1 = (gcnew System::Windows::Forms::TabPage());
			this->checkBox_NoSerial = (gcnew System::Windows::Forms::CheckBox());
			this->label_Common1 = (gcnew System::Windows::Forms::Label());
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
			this->tabPage_Common2 = (gcnew System::Windows::Forms::TabPage());
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
			this->label_Common2 = (gcnew System::Windows::Forms::Label());
			this->panel_Display = (gcnew System::Windows::Forms::Panel());
			this->label_StatusAssistVal = (gcnew System::Windows::Forms::Label());
			this->label_StatusAssist = (gcnew System::Windows::Forms::Label());
			this->label_StatusSpeedVal = (gcnew System::Windows::Forms::Label());
			this->label_StatusSpeed = (gcnew System::Windows::Forms::Label());
			this->label_StatusOdoVal = (gcnew System::Windows::Forms::Label());
			this->label_StatusOdometer = (gcnew System::Windows::Forms::Label());
			this->label_TxStatusBox = (gcnew System::Windows::Forms::Label());
			this->tabPage_P16TravelMode = (gcnew System::Windows::Forms::TabPage());
			this->checkBox_P16TravelMode = (gcnew System::Windows::Forms::CheckBox());
			this->label_P16Regen = (gcnew System::Windows::Forms::Label());
			this->label_P16Assist = (gcnew System::Windows::Forms::Label());
			this->comboBox_P16Assist = (gcnew System::Windows::Forms::ComboBox());
			this->comboBox_P16Regen = (gcnew System::Windows::Forms::ComboBox());
			this->tabPage_P16Command = (gcnew System::Windows::Forms::TabPage());
			this->groupBox_P16Gear = (gcnew System::Windows::Forms::GroupBox());
			this->label_P16FrontGear = (gcnew System::Windows::Forms::Label());
			this->comboBox_P16FrontGear = (gcnew System::Windows::Forms::ComboBox());
			this->label_P16RearGear = (gcnew System::Windows::Forms::Label());
			this->comboBox_P16RearGear = (gcnew System::Windows::Forms::ComboBox());
			this->checkBox_P16RightTurnOn = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox_P16LeftTurnOn = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox_P16HighBeamOn = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox_P16LightOn = (gcnew System::Windows::Forms::CheckBox());
			this->tabPage_P16ManID = (gcnew System::Windows::Forms::TabPage());
			this->label_P16ManID = (gcnew System::Windows::Forms::Label());
			this->numericUpDown_P16ManID = (gcnew System::Windows::Forms::NumericUpDown());
			this->panel_Settings->SuspendLayout();
			this->tabControl_LEV->SuspendLayout();
			this->tabPage_P1->SuspendLayout();
			this->tabControl_P1->SuspendLayout();
			this->tabPage_P1Mode->SuspendLayout();
			this->tabPage_P1State->SuspendLayout();
			this->tabPage_P1Gear->SuspendLayout();
			this->groupBox_P1CurrentGear->SuspendLayout();
			this->tabPage_P1Error->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_P1CustomError))->BeginInit();
			this->tabPage_P1Speed->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_P1Speed))->BeginInit();
			this->tabPage_P1Temp->SuspendLayout();
			this->tabPage_P2->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_P2Speed))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_P2Range))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_P2Distance))->BeginInit();
			this->tabPage_P3->SuspendLayout();
			this->tabControl_Page3->SuspendLayout();
			this->tabPage_P3BatterySOC->SuspendLayout();
			this->groupBox_P3Charge->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_P3Charge))->BeginInit();
			this->tabPage_P3Assist->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_P3PercentAssist))->BeginInit();
			this->tabPage4->SuspendLayout();
			this->tabControl_Page45->SuspendLayout();
			this->tabPage_Page4->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_P4ChargeCycle))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_P4DistanceOnCharge))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_P4FuelConsumption))->BeginInit();
			this->tabPage_Page4Cont->SuspendLayout();
			this->groupBox_P4Voltage->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_P4Voltage))->BeginInit();
			this->tabPage_Page5->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_P5Circum))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_P5RegenModes))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDwn_P5AssistModes))->BeginInit();
			this->tabPage_P16->SuspendLayout();
			this->tabPage_Common1->SuspendLayout();
			this->tabPage_Common2->SuspendLayout();
			this->groupBox_P81Rx->SuspendLayout();
			this->groupBox_P80Rx->SuspendLayout();
			this->panel_Display->SuspendLayout();
			this->tabPage_P16TravelMode->SuspendLayout();
			this->tabPage_P16Command->SuspendLayout();
			this->groupBox_P16Gear->SuspendLayout();
			this->tabPage_P16ManID->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_P16ManID))->BeginInit();
			this->SuspendLayout();
			// 
			// panel_Settings
			// 
			this->panel_Settings->Controls->Add(this->tabControl_LEV);
			this->panel_Settings->Location = System::Drawing::Point(322, 50);
			this->panel_Settings->Name = L"panel_Settings";
			this->panel_Settings->Size = System::Drawing::Size(400, 140);
			this->panel_Settings->TabIndex = 0;
			// 
			// tabControl_LEV
			// 
			this->tabControl_LEV->Controls->Add(this->tabPage_P1);
			this->tabControl_LEV->Controls->Add(this->tabPage_P2);
			this->tabControl_LEV->Controls->Add(this->tabPage_P3);
			this->tabControl_LEV->Controls->Add(this->tabPage4);
			this->tabControl_LEV->Controls->Add(this->tabPage_P16);
			this->tabControl_LEV->Controls->Add(this->tabPage_Common1);
			this->tabControl_LEV->Controls->Add(this->tabPage_Common2);
			this->tabControl_LEV->Location = System::Drawing::Point(0, 3);
			this->tabControl_LEV->Name = L"tabControl_LEV";
			this->tabControl_LEV->SelectedIndex = 0;
			this->tabControl_LEV->Size = System::Drawing::Size(397, 137);
			this->tabControl_LEV->TabIndex = 0;
			// 
			// tabPage_P1
			// 
			this->tabPage_P1->Controls->Add(this->tabControl_P1);
			this->tabPage_P1->Location = System::Drawing::Point(4, 22);
			this->tabPage_P1->Name = L"tabPage_P1";
			this->tabPage_P1->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_P1->Size = System::Drawing::Size(389, 111);
			this->tabPage_P1->TabIndex = 1;
			this->tabPage_P1->Text = L"Page 1";
			this->tabPage_P1->UseVisualStyleBackColor = true;
			// 
			// tabControl_P1
			// 
			this->tabControl_P1->Controls->Add(this->tabPage_P1Mode);
			this->tabControl_P1->Controls->Add(this->tabPage_P1State);
			this->tabControl_P1->Controls->Add(this->tabPage_P1Gear);
			this->tabControl_P1->Controls->Add(this->tabPage_P1Error);
			this->tabControl_P1->Controls->Add(this->tabPage_P1Speed);
			this->tabControl_P1->Controls->Add(this->tabPage_P1Temp);
			this->tabControl_P1->Location = System::Drawing::Point(3, 5);
			this->tabControl_P1->Name = L"tabControl_P1";
			this->tabControl_P1->SelectedIndex = 0;
			this->tabControl_P1->Size = System::Drawing::Size(383, 103);
			this->tabControl_P1->TabIndex = 0;
			// 
			// tabPage_P1Mode
			// 
			this->tabPage_P1Mode->Controls->Add(this->label_P1RegenLevel);
			this->tabPage_P1Mode->Controls->Add(this->label_P1AssistLevel);
			this->tabPage_P1Mode->Controls->Add(this->comboBox_P1AssistLevel);
			this->tabPage_P1Mode->Controls->Add(this->comboBox_P1RegenLevel);
			this->tabPage_P1Mode->Location = System::Drawing::Point(4, 22);
			this->tabPage_P1Mode->Name = L"tabPage_P1Mode";
			this->tabPage_P1Mode->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_P1Mode->Size = System::Drawing::Size(375, 77);
			this->tabPage_P1Mode->TabIndex = 0;
			this->tabPage_P1Mode->Text = L"Travel Mode";
			this->tabPage_P1Mode->UseVisualStyleBackColor = true;
			// 
			// label_P1RegenLevel
			// 
			this->label_P1RegenLevel->AutoSize = true;
			this->label_P1RegenLevel->Location = System::Drawing::Point(196, 19);
			this->label_P1RegenLevel->Name = L"label_P1RegenLevel";
			this->label_P1RegenLevel->Size = System::Drawing::Size(108, 13);
			this->label_P1RegenLevel->TabIndex = 26;
			this->label_P1RegenLevel->Text = L"Current Regen Level:";
			// 
			// label_P1AssistLevel
			// 
			this->label_P1AssistLevel->AutoSize = true;
			this->label_P1AssistLevel->Location = System::Drawing::Point(65, 19);
			this->label_P1AssistLevel->Name = L"label_P1AssistLevel";
			this->label_P1AssistLevel->Size = System::Drawing::Size(103, 13);
			this->label_P1AssistLevel->TabIndex = 25;
			this->label_P1AssistLevel->Text = L"Current Assist Level:";
			// 
			// comboBox_P1AssistLevel
			// 
			this->comboBox_P1AssistLevel->FormattingEnabled = true;
			this->comboBox_P1AssistLevel->Items->AddRange(gcnew cli::array< System::Object^  >(8) {L"Assist Off", L"Assist 1", L"Assist 2", 
				L"Assist 3", L"Assist 4", L"Assist 5", L"Assist 6", L"Assist 7"});
			this->comboBox_P1AssistLevel->Location = System::Drawing::Point(68, 35);
			this->comboBox_P1AssistLevel->Name = L"comboBox_P1AssistLevel";
			this->comboBox_P1AssistLevel->Size = System::Drawing::Size(110, 21);
			this->comboBox_P1AssistLevel->TabIndex = 24;
			this->comboBox_P1AssistLevel->Text = L"Assist Off";
			this->comboBox_P1AssistLevel->SelectedIndexChanged += gcnew System::EventHandler(this, &LEVSensor::comboBox_P1AssistLevel_SelectedIndexChanged);
			// 
			// comboBox_P1RegenLevel
			// 
			this->comboBox_P1RegenLevel->FormattingEnabled = true;
			this->comboBox_P1RegenLevel->Items->AddRange(gcnew cli::array< System::Object^  >(8) {L"Regenerative Off", L"Regenerative 1", 
				L"Regenerative 2", L"Regenerative 3", L"Regenerative 4", L"Regenerative 5", L"Regenerative 6", L"Regenerative 7"});
			this->comboBox_P1RegenLevel->Location = System::Drawing::Point(199, 35);
			this->comboBox_P1RegenLevel->Name = L"comboBox_P1RegenLevel";
			this->comboBox_P1RegenLevel->Size = System::Drawing::Size(110, 21);
			this->comboBox_P1RegenLevel->TabIndex = 23;
			this->comboBox_P1RegenLevel->Text = L"Regenerative Off";
			this->comboBox_P1RegenLevel->SelectedIndexChanged += gcnew System::EventHandler(this, &LEVSensor::comboBox_P1RegenLevel_SelectedIndexChanged);
			// 
			// tabPage_P1State
			// 
			this->tabPage_P1State->Controls->Add(this->checkBox_P1Throttle);
			this->tabPage_P1State->Controls->Add(this->checkBox_P1RightSignalOn);
			this->tabPage_P1State->Controls->Add(this->checkBox_P1LeftSignalOn);
			this->tabPage_P1State->Controls->Add(this->checkBox_P1HighBeam);
			this->tabPage_P1State->Controls->Add(this->checkBox_P1LightOn);
			this->tabPage_P1State->Location = System::Drawing::Point(4, 22);
			this->tabPage_P1State->Name = L"tabPage_P1State";
			this->tabPage_P1State->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_P1State->Size = System::Drawing::Size(375, 77);
			this->tabPage_P1State->TabIndex = 1;
			this->tabPage_P1State->Text = L"System State";
			this->tabPage_P1State->UseVisualStyleBackColor = true;
			// 
			// checkBox_P1Throttle
			// 
			this->checkBox_P1Throttle->AutoSize = true;
			this->checkBox_P1Throttle->Location = System::Drawing::Point(76, 54);
			this->checkBox_P1Throttle->Name = L"checkBox_P1Throttle";
			this->checkBox_P1Throttle->Size = System::Drawing::Size(117, 17);
			this->checkBox_P1Throttle->TabIndex = 31;
			this->checkBox_P1Throttle->Text = L"Manual Throttle On";
			this->checkBox_P1Throttle->UseVisualStyleBackColor = true;
			this->checkBox_P1Throttle->CheckedChanged += gcnew System::EventHandler(this, &LEVSensor::checkBox_P1Throttle_CheckedChanged);
			// 
			// checkBox_P1RightSignalOn
			// 
			this->checkBox_P1RightSignalOn->AutoSize = true;
			this->checkBox_P1RightSignalOn->Location = System::Drawing::Point(174, 32);
			this->checkBox_P1RightSignalOn->Name = L"checkBox_P1RightSignalOn";
			this->checkBox_P1RightSignalOn->Size = System::Drawing::Size(125, 17);
			this->checkBox_P1RightSignalOn->TabIndex = 30;
			this->checkBox_P1RightSignalOn->Text = L"Right Turn Signal On";
			this->checkBox_P1RightSignalOn->UseVisualStyleBackColor = true;
			this->checkBox_P1RightSignalOn->CheckedChanged += gcnew System::EventHandler(this, &LEVSensor::checkBox_P1RightSignalOn_CheckedChanged);
			// 
			// checkBox_P1LeftSignalOn
			// 
			this->checkBox_P1LeftSignalOn->AutoSize = true;
			this->checkBox_P1LeftSignalOn->Location = System::Drawing::Point(174, 9);
			this->checkBox_P1LeftSignalOn->Name = L"checkBox_P1LeftSignalOn";
			this->checkBox_P1LeftSignalOn->Size = System::Drawing::Size(118, 17);
			this->checkBox_P1LeftSignalOn->TabIndex = 29;
			this->checkBox_P1LeftSignalOn->Text = L"Left Turn Signal On";
			this->checkBox_P1LeftSignalOn->UseVisualStyleBackColor = true;
			this->checkBox_P1LeftSignalOn->CheckedChanged += gcnew System::EventHandler(this, &LEVSensor::checkBox_P1LeftSignalOn_CheckedChanged);
			// 
			// checkBox_P1HighBeam
			// 
			this->checkBox_P1HighBeam->AutoSize = true;
			this->checkBox_P1HighBeam->Location = System::Drawing::Point(76, 9);
			this->checkBox_P1HighBeam->Name = L"checkBox_P1HighBeam";
			this->checkBox_P1HighBeam->Size = System::Drawing::Size(95, 17);
			this->checkBox_P1HighBeam->TabIndex = 28;
			this->checkBox_P1HighBeam->Text = L"High Beam On";
			this->checkBox_P1HighBeam->UseVisualStyleBackColor = true;
			this->checkBox_P1HighBeam->CheckedChanged += gcnew System::EventHandler(this, &LEVSensor::checkBox_P1HighBeam_CheckedChanged);
			// 
			// checkBox_P1LightOn
			// 
			this->checkBox_P1LightOn->AutoSize = true;
			this->checkBox_P1LightOn->Location = System::Drawing::Point(76, 32);
			this->checkBox_P1LightOn->Name = L"checkBox_P1LightOn";
			this->checkBox_P1LightOn->Size = System::Drawing::Size(66, 17);
			this->checkBox_P1LightOn->TabIndex = 27;
			this->checkBox_P1LightOn->Text = L"Light On";
			this->checkBox_P1LightOn->UseVisualStyleBackColor = true;
			this->checkBox_P1LightOn->CheckedChanged += gcnew System::EventHandler(this, &LEVSensor::checkBox_P1LightOn_CheckedChanged);
			// 
			// tabPage_P1Gear
			// 
			this->tabPage_P1Gear->Controls->Add(this->checkBox_P1ManualGears);
			this->tabPage_P1Gear->Controls->Add(this->checkBox_P1GearsExist);
			this->tabPage_P1Gear->Controls->Add(this->groupBox_P1CurrentGear);
			this->tabPage_P1Gear->Location = System::Drawing::Point(4, 22);
			this->tabPage_P1Gear->Name = L"tabPage_P1Gear";
			this->tabPage_P1Gear->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_P1Gear->Size = System::Drawing::Size(375, 77);
			this->tabPage_P1Gear->TabIndex = 2;
			this->tabPage_P1Gear->Text = L"Gear State";
			this->tabPage_P1Gear->UseVisualStyleBackColor = true;
			// 
			// checkBox_P1ManualGears
			// 
			this->checkBox_P1ManualGears->AutoSize = true;
			this->checkBox_P1ManualGears->Location = System::Drawing::Point(210, 42);
			this->checkBox_P1ManualGears->Name = L"checkBox_P1ManualGears";
			this->checkBox_P1ManualGears->Size = System::Drawing::Size(123, 17);
			this->checkBox_P1ManualGears->TabIndex = 30;
			this->checkBox_P1ManualGears->Text = L"Manual Gear Control";
			this->checkBox_P1ManualGears->UseVisualStyleBackColor = true;
			this->checkBox_P1ManualGears->CheckedChanged += gcnew System::EventHandler(this, &LEVSensor::checkBox_P1ManualGears_CheckedChanged);
			// 
			// checkBox_P1GearsExist
			// 
			this->checkBox_P1GearsExist->AutoSize = true;
			this->checkBox_P1GearsExist->Location = System::Drawing::Point(210, 18);
			this->checkBox_P1GearsExist->Name = L"checkBox_P1GearsExist";
			this->checkBox_P1GearsExist->Size = System::Drawing::Size(79, 17);
			this->checkBox_P1GearsExist->TabIndex = 29;
			this->checkBox_P1GearsExist->Text = L"Gears Exist";
			this->checkBox_P1GearsExist->UseVisualStyleBackColor = true;
			this->checkBox_P1GearsExist->CheckedChanged += gcnew System::EventHandler(this, &LEVSensor::checkBox_P1GearsExist_CheckedChanged);
			// 
			// groupBox_P1CurrentGear
			// 
			this->groupBox_P1CurrentGear->Controls->Add(this->label_P1FrontGear);
			this->groupBox_P1CurrentGear->Controls->Add(this->comboBox_P1FrontGear);
			this->groupBox_P1CurrentGear->Controls->Add(this->label_P1RearGear);
			this->groupBox_P1CurrentGear->Controls->Add(this->comboBox_P1RearGear);
			this->groupBox_P1CurrentGear->Location = System::Drawing::Point(42, 5);
			this->groupBox_P1CurrentGear->Name = L"groupBox_P1CurrentGear";
			this->groupBox_P1CurrentGear->Size = System::Drawing::Size(133, 67);
			this->groupBox_P1CurrentGear->TabIndex = 28;
			this->groupBox_P1CurrentGear->TabStop = false;
			this->groupBox_P1CurrentGear->Text = L"Current Gear";
			// 
			// label_P1FrontGear
			// 
			this->label_P1FrontGear->AutoSize = true;
			this->label_P1FrontGear->Location = System::Drawing::Point(7, 18);
			this->label_P1FrontGear->Name = L"label_P1FrontGear";
			this->label_P1FrontGear->Size = System::Drawing::Size(60, 13);
			this->label_P1FrontGear->TabIndex = 21;
			this->label_P1FrontGear->Text = L"Front Gear:";
			// 
			// comboBox_P1FrontGear
			// 
			this->comboBox_P1FrontGear->FormattingEnabled = true;
			this->comboBox_P1FrontGear->Items->AddRange(gcnew cli::array< System::Object^  >(4) {L"none", L"1", L"2", L"3"});
			this->comboBox_P1FrontGear->Location = System::Drawing::Point(73, 15);
			this->comboBox_P1FrontGear->Name = L"comboBox_P1FrontGear";
			this->comboBox_P1FrontGear->Size = System::Drawing::Size(47, 21);
			this->comboBox_P1FrontGear->TabIndex = 19;
			this->comboBox_P1FrontGear->Text = L"none";
			this->comboBox_P1FrontGear->SelectedIndexChanged += gcnew System::EventHandler(this, &LEVSensor::comboBox_P1FrontGear_SelectedIndexChanged);
			// 
			// label_P1RearGear
			// 
			this->label_P1RearGear->AutoSize = true;
			this->label_P1RearGear->Location = System::Drawing::Point(8, 43);
			this->label_P1RearGear->Name = L"label_P1RearGear";
			this->label_P1RearGear->Size = System::Drawing::Size(59, 13);
			this->label_P1RearGear->TabIndex = 22;
			this->label_P1RearGear->Text = L"Rear Gear:";
			// 
			// comboBox_P1RearGear
			// 
			this->comboBox_P1RearGear->FormattingEnabled = true;
			this->comboBox_P1RearGear->Items->AddRange(gcnew cli::array< System::Object^  >(16) {L"none", L"1", L"2", L"3", L"4", L"5", 
				L"6", L"7", L"8", L"9", L"10", L"11", L"12", L"13", L"14", L"15"});
			this->comboBox_P1RearGear->Location = System::Drawing::Point(73, 40);
			this->comboBox_P1RearGear->Name = L"comboBox_P1RearGear";
			this->comboBox_P1RearGear->Size = System::Drawing::Size(47, 21);
			this->comboBox_P1RearGear->TabIndex = 20;
			this->comboBox_P1RearGear->Text = L"none";
			this->comboBox_P1RearGear->SelectedIndexChanged += gcnew System::EventHandler(this, &LEVSensor::comboBox_P1RearGear_SelectedIndexChanged);
			// 
			// tabPage_P1Error
			// 
			this->tabPage_P1Error->Controls->Add(this->numericUpDown_P1CustomError);
			this->tabPage_P1Error->Controls->Add(this->radioButton_P1CustomError);
			this->tabPage_P1Error->Controls->Add(this->radioButton_P1Overheat);
			this->tabPage_P1Error->Controls->Add(this->radioButton_P1EndLife);
			this->tabPage_P1Error->Controls->Add(this->radioButton_P1TrainError);
			this->tabPage_P1Error->Controls->Add(this->radioButton_P1BatteryError);
			this->tabPage_P1Error->Controls->Add(this->radioButton_P1NoError);
			this->tabPage_P1Error->Location = System::Drawing::Point(4, 22);
			this->tabPage_P1Error->Name = L"tabPage_P1Error";
			this->tabPage_P1Error->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_P1Error->Size = System::Drawing::Size(375, 77);
			this->tabPage_P1Error->TabIndex = 3;
			this->tabPage_P1Error->Text = L"Error";
			this->tabPage_P1Error->UseVisualStyleBackColor = true;
			// 
			// numericUpDown_P1CustomError
			// 
			this->numericUpDown_P1CustomError->Enabled = false;
			this->numericUpDown_P1CustomError->Location = System::Drawing::Point(310, 51);
			this->numericUpDown_P1CustomError->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
			this->numericUpDown_P1CustomError->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {16, 0, 0, 0});
			this->numericUpDown_P1CustomError->Name = L"numericUpDown_P1CustomError";
			this->numericUpDown_P1CustomError->Size = System::Drawing::Size(42, 20);
			this->numericUpDown_P1CustomError->TabIndex = 6;
			this->numericUpDown_P1CustomError->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
			this->numericUpDown_P1CustomError->ValueChanged += gcnew System::EventHandler(this, &LEVSensor::numericUpDown_P1CustomError_ValueChanged);
			// 
			// radioButton_P1CustomError
			// 
			this->radioButton_P1CustomError->AutoSize = true;
			this->radioButton_P1CustomError->Location = System::Drawing::Point(176, 54);
			this->radioButton_P1CustomError->Name = L"radioButton_P1CustomError";
			this->radioButton_P1CustomError->Size = System::Drawing::Size(128, 17);
			this->radioButton_P1CustomError->TabIndex = 5;
			this->radioButton_P1CustomError->Text = L"Custom Error Code   --";
			this->radioButton_P1CustomError->UseVisualStyleBackColor = true;
			this->radioButton_P1CustomError->CheckedChanged += gcnew System::EventHandler(this, &LEVSensor::radioButton_CheckedChanged);
			// 
			// radioButton_P1Overheat
			// 
			this->radioButton_P1Overheat->AutoSize = true;
			this->radioButton_P1Overheat->Location = System::Drawing::Point(176, 30);
			this->radioButton_P1Overheat->Name = L"radioButton_P1Overheat";
			this->radioButton_P1Overheat->Size = System::Drawing::Size(83, 17);
			this->radioButton_P1Overheat->TabIndex = 4;
			this->radioButton_P1Overheat->Text = L"Overheating";
			this->radioButton_P1Overheat->UseVisualStyleBackColor = true;
			this->radioButton_P1Overheat->CheckedChanged += gcnew System::EventHandler(this, &LEVSensor::radioButton_CheckedChanged);
			// 
			// radioButton_P1EndLife
			// 
			this->radioButton_P1EndLife->AutoSize = true;
			this->radioButton_P1EndLife->Location = System::Drawing::Point(176, 6);
			this->radioButton_P1EndLife->Name = L"radioButton_P1EndLife";
			this->radioButton_P1EndLife->Size = System::Drawing::Size(112, 17);
			this->radioButton_P1EndLife->TabIndex = 3;
			this->radioButton_P1EndLife->Text = L"Battery End of Life";
			this->radioButton_P1EndLife->UseVisualStyleBackColor = true;
			this->radioButton_P1EndLife->CheckedChanged += gcnew System::EventHandler(this, &LEVSensor::radioButton_CheckedChanged);
			// 
			// radioButton_P1TrainError
			// 
			this->radioButton_P1TrainError->AutoSize = true;
			this->radioButton_P1TrainError->Location = System::Drawing::Point(50, 54);
			this->radioButton_P1TrainError->Name = L"radioButton_P1TrainError";
			this->radioButton_P1TrainError->Size = System::Drawing::Size(102, 17);
			this->radioButton_P1TrainError->TabIndex = 2;
			this->radioButton_P1TrainError->Text = L"Drive Train Error";
			this->radioButton_P1TrainError->UseVisualStyleBackColor = true;
			this->radioButton_P1TrainError->CheckedChanged += gcnew System::EventHandler(this, &LEVSensor::radioButton_CheckedChanged);
			// 
			// radioButton_P1BatteryError
			// 
			this->radioButton_P1BatteryError->AutoSize = true;
			this->radioButton_P1BatteryError->Location = System::Drawing::Point(50, 31);
			this->radioButton_P1BatteryError->Name = L"radioButton_P1BatteryError";
			this->radioButton_P1BatteryError->Size = System::Drawing::Size(83, 17);
			this->radioButton_P1BatteryError->TabIndex = 1;
			this->radioButton_P1BatteryError->Text = L"Battery Error";
			this->radioButton_P1BatteryError->UseVisualStyleBackColor = true;
			this->radioButton_P1BatteryError->CheckedChanged += gcnew System::EventHandler(this, &LEVSensor::radioButton_CheckedChanged);
			// 
			// radioButton_P1NoError
			// 
			this->radioButton_P1NoError->AutoSize = true;
			this->radioButton_P1NoError->Checked = true;
			this->radioButton_P1NoError->Location = System::Drawing::Point(50, 7);
			this->radioButton_P1NoError->Name = L"radioButton_P1NoError";
			this->radioButton_P1NoError->Size = System::Drawing::Size(64, 17);
			this->radioButton_P1NoError->TabIndex = 0;
			this->radioButton_P1NoError->TabStop = true;
			this->radioButton_P1NoError->Text = L"No Error";
			this->radioButton_P1NoError->UseVisualStyleBackColor = true;
			this->radioButton_P1NoError->CheckedChanged += gcnew System::EventHandler(this, &LEVSensor::radioButton_CheckedChanged);
			// 
			// tabPage_P1Speed
			// 
			this->tabPage_P1Speed->Controls->Add(this->label_P1Speed);
			this->tabPage_P1Speed->Controls->Add(this->numericUpDown_P1Speed);
			this->tabPage_P1Speed->Location = System::Drawing::Point(4, 22);
			this->tabPage_P1Speed->Name = L"tabPage_P1Speed";
			this->tabPage_P1Speed->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_P1Speed->Size = System::Drawing::Size(375, 77);
			this->tabPage_P1Speed->TabIndex = 4;
			this->tabPage_P1Speed->Text = L"LEV Speed";
			this->tabPage_P1Speed->UseVisualStyleBackColor = true;
			// 
			// label_P1Speed
			// 
			this->label_P1Speed->AutoSize = true;
			this->label_P1Speed->Location = System::Drawing::Point(107, 30);
			this->label_P1Speed->Name = L"label_P1Speed";
			this->label_P1Speed->Size = System::Drawing::Size(98, 13);
			this->label_P1Speed->TabIndex = 3;
			this->label_P1Speed->Text = L"LEV Speed (km/h):";
			// 
			// numericUpDown_P1Speed
			// 
			this->numericUpDown_P1Speed->DecimalPlaces = 1;
			this->numericUpDown_P1Speed->Location = System::Drawing::Point(211, 28);
			this->numericUpDown_P1Speed->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {4095, 0, 0, 65536});
			this->numericUpDown_P1Speed->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
			this->numericUpDown_P1Speed->Name = L"numericUpDown_P1Speed";
			this->numericUpDown_P1Speed->Size = System::Drawing::Size(65, 20);
			this->numericUpDown_P1Speed->TabIndex = 2;
			this->numericUpDown_P1Speed->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {205, 0, 0, 65536});
			this->numericUpDown_P1Speed->ValueChanged += gcnew System::EventHandler(this, &LEVSensor::numericUpDown_P1Speed_ValueChanged);
			// 
			// tabPage_P1Temp
			// 
			this->tabPage_P1Temp->Controls->Add(this->checkBox_P1TempUnused);
			this->tabPage_P1Temp->Controls->Add(this->checkBox_P1MotorOverheat);
			this->tabPage_P1Temp->Controls->Add(this->checkBox_P1BattOverheat);
			this->tabPage_P1Temp->Controls->Add(this->label_P1MotorTemp);
			this->tabPage_P1Temp->Controls->Add(this->label_P1BattTemp);
			this->tabPage_P1Temp->Controls->Add(this->comboBox_P1MotorTemp);
			this->tabPage_P1Temp->Controls->Add(this->comboBox_P1BattTemp);
			this->tabPage_P1Temp->Location = System::Drawing::Point(4, 22);
			this->tabPage_P1Temp->Name = L"tabPage_P1Temp";
			this->tabPage_P1Temp->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_P1Temp->Size = System::Drawing::Size(375, 77);
			this->tabPage_P1Temp->TabIndex = 5;
			this->tabPage_P1Temp->Text = L"Temp";
			this->tabPage_P1Temp->UseVisualStyleBackColor = true;
			// 
			// checkBox_P1TempUnused
			// 
			this->checkBox_P1TempUnused->AutoSize = true;
			this->checkBox_P1TempUnused->Location = System::Drawing::Point(220, 51);
			this->checkBox_P1TempUnused->Name = L"checkBox_P1TempUnused";
			this->checkBox_P1TempUnused->Size = System::Drawing::Size(63, 17);
			this->checkBox_P1TempUnused->TabIndex = 35;
			this->checkBox_P1TempUnused->Text = L"Unused";
			this->checkBox_P1TempUnused->UseVisualStyleBackColor = true;
			this->checkBox_P1TempUnused->CheckedChanged += gcnew System::EventHandler(this, &LEVSensor::checkBox_P1TempUnused_CheckedChanged);
			// 
			// checkBox_P1MotorOverheat
			// 
			this->checkBox_P1MotorOverheat->AutoSize = true;
			this->checkBox_P1MotorOverheat->Location = System::Drawing::Point(220, 32);
			this->checkBox_P1MotorOverheat->Name = L"checkBox_P1MotorOverheat";
			this->checkBox_P1MotorOverheat->Size = System::Drawing::Size(124, 17);
			this->checkBox_P1MotorOverheat->TabIndex = 37;
			this->checkBox_P1MotorOverheat->Text = L"Motor Overheat Alert";
			this->checkBox_P1MotorOverheat->UseVisualStyleBackColor = true;
			this->checkBox_P1MotorOverheat->CheckedChanged += gcnew System::EventHandler(this, &LEVSensor::checkBox_P1MotorOverheat_CheckedChanged);
			// 
			// checkBox_P1BattOverheat
			// 
			this->checkBox_P1BattOverheat->AutoSize = true;
			this->checkBox_P1BattOverheat->Location = System::Drawing::Point(220, 12);
			this->checkBox_P1BattOverheat->Name = L"checkBox_P1BattOverheat";
			this->checkBox_P1BattOverheat->Size = System::Drawing::Size(130, 17);
			this->checkBox_P1BattOverheat->TabIndex = 36;
			this->checkBox_P1BattOverheat->Text = L"Battery Overheat Alert";
			this->checkBox_P1BattOverheat->UseVisualStyleBackColor = true;
			this->checkBox_P1BattOverheat->CheckedChanged += gcnew System::EventHandler(this, &LEVSensor::checkBox_P1BattOverheat_CheckedChanged);
			// 
			// label_P1MotorTemp
			// 
			this->label_P1MotorTemp->AutoSize = true;
			this->label_P1MotorTemp->Location = System::Drawing::Point(109, 20);
			this->label_P1MotorTemp->Name = L"label_P1MotorTemp";
			this->label_P1MotorTemp->Size = System::Drawing::Size(67, 13);
			this->label_P1MotorTemp->TabIndex = 34;
			this->label_P1MotorTemp->Text = L"Motor Temp:";
			// 
			// label_P1BattTemp
			// 
			this->label_P1BattTemp->AutoSize = true;
			this->label_P1BattTemp->Location = System::Drawing::Point(14, 20);
			this->label_P1BattTemp->Name = L"label_P1BattTemp";
			this->label_P1BattTemp->Size = System::Drawing::Size(73, 13);
			this->label_P1BattTemp->TabIndex = 33;
			this->label_P1BattTemp->Text = L"Battery Temp:";
			// 
			// comboBox_P1MotorTemp
			// 
			this->comboBox_P1MotorTemp->FormattingEnabled = true;
			this->comboBox_P1MotorTemp->Items->AddRange(gcnew cli::array< System::Object^  >(6) {L"Unknown", L"Cold", L"Cold/Warm", L"Warm", 
				L"Warm/Hot", L"Hot"});
			this->comboBox_P1MotorTemp->Location = System::Drawing::Point(112, 36);
			this->comboBox_P1MotorTemp->Name = L"comboBox_P1MotorTemp";
			this->comboBox_P1MotorTemp->Size = System::Drawing::Size(82, 21);
			this->comboBox_P1MotorTemp->TabIndex = 32;
			this->comboBox_P1MotorTemp->Text = L"Unknown";
			this->comboBox_P1MotorTemp->SelectedIndexChanged += gcnew System::EventHandler(this, &LEVSensor::comboBox_P1MotorTemp_SelectedIndexChanged);
			// 
			// comboBox_P1BattTemp
			// 
			this->comboBox_P1BattTemp->FormattingEnabled = true;
			this->comboBox_P1BattTemp->Items->AddRange(gcnew cli::array< System::Object^  >(6) {L"Unknown", L"Cold", L"Cold/Warm", L"Warm", 
				L"Warm/Hot", L"Hot"});
			this->comboBox_P1BattTemp->Location = System::Drawing::Point(17, 36);
			this->comboBox_P1BattTemp->Name = L"comboBox_P1BattTemp";
			this->comboBox_P1BattTemp->Size = System::Drawing::Size(82, 21);
			this->comboBox_P1BattTemp->TabIndex = 31;
			this->comboBox_P1BattTemp->Text = L"Unknown";
			this->comboBox_P1BattTemp->SelectedIndexChanged += gcnew System::EventHandler(this, &LEVSensor::comboBox_P1BattTemp_SelectedIndexChanged);
			// 
			// tabPage_P2
			// 
			this->tabPage_P2->Controls->Add(this->checkBox_UsePage34);
			this->tabPage_P2->Controls->Add(this->label_P2Range);
			this->tabPage_P2->Controls->Add(this->label_P2Speed);
			this->tabPage_P2->Controls->Add(this->numericUpDown_P2Speed);
			this->tabPage_P2->Controls->Add(this->numericUpDown_P2Range);
			this->tabPage_P2->Controls->Add(this->checkBox_P2RangeUnused);
			this->tabPage_P2->Controls->Add(this->numericUpDown_P2Distance);
			this->tabPage_P2->Controls->Add(this->label_P2TotalDistance);
			this->tabPage_P2->Controls->Add(this->label_P2TotDistDisplay);
			this->tabPage_P2->Controls->Add(this->button_P2UpdateDist);
			this->tabPage_P2->Controls->Add(this->label_P2UpdateError);
			this->tabPage_P2->Location = System::Drawing::Point(4, 22);
			this->tabPage_P2->Name = L"tabPage_P2";
			this->tabPage_P2->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_P2->Size = System::Drawing::Size(389, 111);
			this->tabPage_P2->TabIndex = 2;
			this->tabPage_P2->Text = L"Page 2/34";
			this->tabPage_P2->UseVisualStyleBackColor = true;
			// 
			// checkBox_UsePage34
			// 
			this->checkBox_UsePage34->AutoSize = true;
			this->checkBox_UsePage34->Location = System::Drawing::Point(206, 88);
			this->checkBox_UsePage34->Name = L"checkBox_UsePage34";
			this->checkBox_UsePage34->Size = System::Drawing::Size(94, 17);
			this->checkBox_UsePage34->TabIndex = 45;
			this->checkBox_UsePage34->Text = L"Use Page 34\?";
			this->checkBox_UsePage34->UseVisualStyleBackColor = true;
			this->checkBox_UsePage34->CheckedChanged += gcnew System::EventHandler(this, &LEVSensor::checkBox_UsePage34_CheckedChanged);
			// 
			// label_P2Range
			// 
			this->label_P2Range->AutoSize = true;
			this->label_P2Range->Location = System::Drawing::Point(22, 36);
			this->label_P2Range->Name = L"label_P2Range";
			this->label_P2Range->Size = System::Drawing::Size(118, 13);
			this->label_P2Range->TabIndex = 44;
			this->label_P2Range->Text = L"Remaining Range (km):";
			// 
			// label_P2Speed
			// 
			this->label_P2Speed->AutoSize = true;
			this->label_P2Speed->Location = System::Drawing::Point(42, 62);
			this->label_P2Speed->Name = L"label_P2Speed";
			this->label_P2Speed->Size = System::Drawing::Size(98, 13);
			this->label_P2Speed->TabIndex = 43;
			this->label_P2Speed->Text = L"LEV Speed (km/h):";
			// 
			// numericUpDown_P2Speed
			// 
			this->numericUpDown_P2Speed->DecimalPlaces = 1;
			this->numericUpDown_P2Speed->Location = System::Drawing::Point(146, 60);
			this->numericUpDown_P2Speed->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {4095, 0, 0, 65536});
			this->numericUpDown_P2Speed->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
			this->numericUpDown_P2Speed->Name = L"numericUpDown_P2Speed";
			this->numericUpDown_P2Speed->Size = System::Drawing::Size(54, 20);
			this->numericUpDown_P2Speed->TabIndex = 42;
			this->numericUpDown_P2Speed->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {205, 0, 0, 65536});
			this->numericUpDown_P2Speed->ValueChanged += gcnew System::EventHandler(this, &LEVSensor::numericUpDown_P2Speed_ValueChanged);
			// 
			// numericUpDown_P2Range
			// 
			this->numericUpDown_P2Range->Location = System::Drawing::Point(146, 34);
			this->numericUpDown_P2Range->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {4095, 0, 0, 0});
			this->numericUpDown_P2Range->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->numericUpDown_P2Range->Name = L"numericUpDown_P2Range";
			this->numericUpDown_P2Range->Size = System::Drawing::Size(54, 20);
			this->numericUpDown_P2Range->TabIndex = 6;
			this->numericUpDown_P2Range->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {4095, 0, 0, 0});
			this->numericUpDown_P2Range->ValueChanged += gcnew System::EventHandler(this, &LEVSensor::numericUpDown_P2Range_ValueChanged);
			// 
			// checkBox_P2RangeUnused
			// 
			this->checkBox_P2RangeUnused->AutoSize = true;
			this->checkBox_P2RangeUnused->Location = System::Drawing::Point(206, 35);
			this->checkBox_P2RangeUnused->Name = L"checkBox_P2RangeUnused";
			this->checkBox_P2RangeUnused->Size = System::Drawing::Size(63, 17);
			this->checkBox_P2RangeUnused->TabIndex = 7;
			this->checkBox_P2RangeUnused->Text = L"Unused";
			this->checkBox_P2RangeUnused->UseVisualStyleBackColor = true;
			this->checkBox_P2RangeUnused->CheckedChanged += gcnew System::EventHandler(this, &LEVSensor::checkBox_P2RangeUnused_CheckedChanged);
			// 
			// numericUpDown_P2Distance
			// 
			this->numericUpDown_P2Distance->Location = System::Drawing::Point(204, 9);
			this->numericUpDown_P2Distance->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {167772, 0, 0, 0});
			this->numericUpDown_P2Distance->Name = L"numericUpDown_P2Distance";
			this->numericUpDown_P2Distance->Size = System::Drawing::Size(68, 20);
			this->numericUpDown_P2Distance->TabIndex = 38;
			// 
			// label_P2TotalDistance
			// 
			this->label_P2TotalDistance->AutoSize = true;
			this->label_P2TotalDistance->Location = System::Drawing::Point(16, 11);
			this->label_P2TotalDistance->Name = L"label_P2TotalDistance";
			this->label_P2TotalDistance->Size = System::Drawing::Size(102, 13);
			this->label_P2TotalDistance->TabIndex = 31;
			this->label_P2TotalDistance->Text = L"Total Distance (km):";
			// 
			// label_P2TotDistDisplay
			// 
			this->label_P2TotDistDisplay->Location = System::Drawing::Point(124, 11);
			this->label_P2TotDistDisplay->Name = L"label_P2TotDistDisplay";
			this->label_P2TotDistDisplay->Size = System::Drawing::Size(74, 13);
			this->label_P2TotDistDisplay->TabIndex = 32;
			this->label_P2TotDistDisplay->Text = L"0";
			// 
			// button_P2UpdateDist
			// 
			this->button_P2UpdateDist->Location = System::Drawing::Point(278, 7);
			this->button_P2UpdateDist->Name = L"button_P2UpdateDist";
			this->button_P2UpdateDist->Size = System::Drawing::Size(97, 20);
			this->button_P2UpdateDist->TabIndex = 35;
			this->button_P2UpdateDist->Text = L"Update Distance";
			this->button_P2UpdateDist->UseVisualStyleBackColor = true;
			this->button_P2UpdateDist->Click += gcnew System::EventHandler(this, &LEVSensor::button_P2UpdateDist_Click);
			// 
			// label_P2UpdateError
			// 
			this->label_P2UpdateError->AutoSize = true;
			this->label_P2UpdateError->Location = System::Drawing::Point(275, 36);
			this->label_P2UpdateError->Name = L"label_P2UpdateError";
			this->label_P2UpdateError->Size = System::Drawing::Size(32, 13);
			this->label_P2UpdateError->TabIndex = 33;
			this->label_P2UpdateError->Text = L"Error:";
			this->label_P2UpdateError->Visible = false;
			// 
			// tabPage_P3
			// 
			this->tabPage_P3->Controls->Add(this->tabControl_Page3);
			this->tabPage_P3->Location = System::Drawing::Point(4, 22);
			this->tabPage_P3->Name = L"tabPage_P3";
			this->tabPage_P3->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_P3->Size = System::Drawing::Size(389, 111);
			this->tabPage_P3->TabIndex = 3;
			this->tabPage_P3->Text = L"Page 3";
			this->tabPage_P3->UseVisualStyleBackColor = true;
			// 
			// tabControl_Page3
			// 
			this->tabControl_Page3->Controls->Add(this->tabPage_P3BatterySOC);
			this->tabControl_Page3->Controls->Add(this->tabPage_P3Assist);
			this->tabControl_Page3->Location = System::Drawing::Point(3, 5);
			this->tabControl_Page3->Name = L"tabControl_Page3";
			this->tabControl_Page3->SelectedIndex = 0;
			this->tabControl_Page3->Size = System::Drawing::Size(383, 103);
			this->tabControl_Page3->TabIndex = 1;
			// 
			// tabPage_P3BatterySOC
			// 
			this->tabPage_P3BatterySOC->Controls->Add(this->groupBox_P3Charge);
			this->tabPage_P3BatterySOC->Controls->Add(this->label_P3Note2);
			this->tabPage_P3BatterySOC->Controls->Add(this->label_P3Note1);
			this->tabPage_P3BatterySOC->Location = System::Drawing::Point(4, 22);
			this->tabPage_P3BatterySOC->Name = L"tabPage_P3BatterySOC";
			this->tabPage_P3BatterySOC->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_P3BatterySOC->Size = System::Drawing::Size(375, 77);
			this->tabPage_P3BatterySOC->TabIndex = 5;
			this->tabPage_P3BatterySOC->Text = L"Battery SOC";
			this->tabPage_P3BatterySOC->UseVisualStyleBackColor = true;
			// 
			// groupBox_P3Charge
			// 
			this->groupBox_P3Charge->Controls->Add(this->numericUpDown_P3Charge);
			this->groupBox_P3Charge->Controls->Add(this->checkBox_P3BattEmpty);
			this->groupBox_P3Charge->Location = System::Drawing::Point(154, 25);
			this->groupBox_P3Charge->Name = L"groupBox_P3Charge";
			this->groupBox_P3Charge->Size = System::Drawing::Size(182, 46);
			this->groupBox_P3Charge->TabIndex = 13;
			this->groupBox_P3Charge->TabStop = false;
			this->groupBox_P3Charge->Text = L"Battery Charge (%)";
			// 
			// numericUpDown_P3Charge
			// 
			this->numericUpDown_P3Charge->Location = System::Drawing::Point(20, 19);
			this->numericUpDown_P3Charge->Name = L"numericUpDown_P3Charge";
			this->numericUpDown_P3Charge->Size = System::Drawing::Size(45, 20);
			this->numericUpDown_P3Charge->TabIndex = 4;
			this->numericUpDown_P3Charge->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {100, 0, 0, 0});
			this->numericUpDown_P3Charge->ValueChanged += gcnew System::EventHandler(this, &LEVSensor::numericUpDown_P3Charge_ValueChanged);
			// 
			// checkBox_P3BattEmpty
			// 
			this->checkBox_P3BattEmpty->AutoSize = true;
			this->checkBox_P3BattEmpty->Location = System::Drawing::Point(71, 22);
			this->checkBox_P3BattEmpty->Name = L"checkBox_P3BattEmpty";
			this->checkBox_P3BattEmpty->Size = System::Drawing::Size(91, 17);
			this->checkBox_P3BattEmpty->TabIndex = 3;
			this->checkBox_P3BattEmpty->Text = L"Battery Empty";
			this->checkBox_P3BattEmpty->UseVisualStyleBackColor = true;
			this->checkBox_P3BattEmpty->CheckedChanged += gcnew System::EventHandler(this, &LEVSensor::checkBox_P3BattEmpty_CheckedChanged);
			// 
			// label_P3Note2
			// 
			this->label_P3Note2->AutoSize = true;
			this->label_P3Note2->Location = System::Drawing::Point(7, 20);
			this->label_P3Note2->Name = L"label_P3Note2";
			this->label_P3Note2->Size = System::Drawing::Size(110, 13);
			this->label_P3Note2->TabIndex = 1;
			this->label_P3Note2->Text = L"under the Page 1 tab.";
			// 
			// label_P3Note1
			// 
			this->label_P3Note1->AutoSize = true;
			this->label_P3Note1->Location = System::Drawing::Point(7, 7);
			this->label_P3Note1->Name = L"label_P3Note1";
			this->label_P3Note1->Size = System::Drawing::Size(360, 13);
			this->label_P3Note1->TabIndex = 0;
			this->label_P3Note1->Text = L"Please note that fields shared between Page 1 and Page 3 must be altered";
			// 
			// tabPage_P3Assist
			// 
			this->tabPage_P3Assist->Controls->Add(this->label_P3PercentAssist);
			this->tabPage_P3Assist->Controls->Add(this->numericUpDown_P3PercentAssist);
			this->tabPage_P3Assist->Location = System::Drawing::Point(4, 22);
			this->tabPage_P3Assist->Name = L"tabPage_P3Assist";
			this->tabPage_P3Assist->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_P3Assist->Size = System::Drawing::Size(375, 77);
			this->tabPage_P3Assist->TabIndex = 6;
			this->tabPage_P3Assist->Text = L"% Assist";
			this->tabPage_P3Assist->UseVisualStyleBackColor = true;
			// 
			// label_P3PercentAssist
			// 
			this->label_P3PercentAssist->AutoSize = true;
			this->label_P3PercentAssist->Location = System::Drawing::Point(123, 30);
			this->label_P3PercentAssist->Name = L"label_P3PercentAssist";
			this->label_P3PercentAssist->Size = System::Drawing::Size(77, 13);
			this->label_P3PercentAssist->TabIndex = 43;
			this->label_P3PercentAssist->Text = L"Percent Assist:";
			// 
			// numericUpDown_P3PercentAssist
			// 
			this->numericUpDown_P3PercentAssist->Location = System::Drawing::Point(206, 28);
			this->numericUpDown_P3PercentAssist->Name = L"numericUpDown_P3PercentAssist";
			this->numericUpDown_P3PercentAssist->Size = System::Drawing::Size(45, 20);
			this->numericUpDown_P3PercentAssist->TabIndex = 42;
			this->numericUpDown_P3PercentAssist->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {100, 0, 0, 0});
			this->numericUpDown_P3PercentAssist->ValueChanged += gcnew System::EventHandler(this, &LEVSensor::numericUpDown_P3PercentAssist_ValueChanged);
			// 
			// tabPage4
			// 
			this->tabPage4->Controls->Add(this->tabControl_Page45);
			this->tabPage4->Location = System::Drawing::Point(4, 22);
			this->tabPage4->Name = L"tabPage4";
			this->tabPage4->Padding = System::Windows::Forms::Padding(3);
			this->tabPage4->Size = System::Drawing::Size(389, 111);
			this->tabPage4->TabIndex = 8;
			this->tabPage4->Text = L"Page 4/5";
			this->tabPage4->UseVisualStyleBackColor = true;
			// 
			// tabControl_Page45
			// 
			this->tabControl_Page45->Controls->Add(this->tabPage_Page4);
			this->tabControl_Page45->Controls->Add(this->tabPage_Page4Cont);
			this->tabControl_Page45->Controls->Add(this->tabPage_Page5);
			this->tabControl_Page45->Location = System::Drawing::Point(3, 5);
			this->tabControl_Page45->Name = L"tabControl_Page45";
			this->tabControl_Page45->SelectedIndex = 0;
			this->tabControl_Page45->Size = System::Drawing::Size(383, 103);
			this->tabControl_Page45->TabIndex = 2;
			// 
			// tabPage_Page4
			// 
			this->tabPage_Page4->Controls->Add(this->numericUpDown_P4ChargeCycle);
			this->tabPage_Page4->Controls->Add(this->checkBox_P4DistanceOnChargeUnused);
			this->tabPage_Page4->Controls->Add(this->checkBox_P4FuelConsumptionUnused);
			this->tabPage_Page4->Controls->Add(this->checkBox_P4ChargeCycleUnused);
			this->tabPage_Page4->Controls->Add(this->label_P4DistanceOnCharge);
			this->tabPage_Page4->Controls->Add(this->numericUpDown_P4DistanceOnCharge);
			this->tabPage_Page4->Controls->Add(this->numericUpDown_P4FuelConsumption);
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
			// numericUpDown_P4ChargeCycle
			// 
			this->numericUpDown_P4ChargeCycle->Location = System::Drawing::Point(153, 6);
			this->numericUpDown_P4ChargeCycle->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {4095, 0, 0, 0});
			this->numericUpDown_P4ChargeCycle->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->numericUpDown_P4ChargeCycle->Name = L"numericUpDown_P4ChargeCycle";
			this->numericUpDown_P4ChargeCycle->Size = System::Drawing::Size(54, 20);
			this->numericUpDown_P4ChargeCycle->TabIndex = 15;
			this->numericUpDown_P4ChargeCycle->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {56, 0, 0, 0});
			this->numericUpDown_P4ChargeCycle->ValueChanged += gcnew System::EventHandler(this, &LEVSensor::numericUpDown_P4ChargeCycle_ValueChanged);
			// 
			// checkBox_P4DistanceOnChargeUnused
			// 
			this->checkBox_P4DistanceOnChargeUnused->AutoSize = true;
			this->checkBox_P4DistanceOnChargeUnused->Location = System::Drawing::Point(237, 54);
			this->checkBox_P4DistanceOnChargeUnused->Name = L"checkBox_P4DistanceOnChargeUnused";
			this->checkBox_P4DistanceOnChargeUnused->Size = System::Drawing::Size(63, 17);
			this->checkBox_P4DistanceOnChargeUnused->TabIndex = 14;
			this->checkBox_P4DistanceOnChargeUnused->Text = L"Unused";
			this->checkBox_P4DistanceOnChargeUnused->UseVisualStyleBackColor = true;
			this->checkBox_P4DistanceOnChargeUnused->CheckedChanged += gcnew System::EventHandler(this, &LEVSensor::checkBox_P4DistanceOnChargeUnused_CheckedChanged);
			// 
			// checkBox_P4FuelConsumptionUnused
			// 
			this->checkBox_P4FuelConsumptionUnused->AutoSize = true;
			this->checkBox_P4FuelConsumptionUnused->Location = System::Drawing::Point(237, 31);
			this->checkBox_P4FuelConsumptionUnused->Name = L"checkBox_P4FuelConsumptionUnused";
			this->checkBox_P4FuelConsumptionUnused->Size = System::Drawing::Size(63, 17);
			this->checkBox_P4FuelConsumptionUnused->TabIndex = 13;
			this->checkBox_P4FuelConsumptionUnused->Text = L"Unused";
			this->checkBox_P4FuelConsumptionUnused->UseVisualStyleBackColor = true;
			this->checkBox_P4FuelConsumptionUnused->CheckedChanged += gcnew System::EventHandler(this, &LEVSensor::checkBox_P4FuelConsumptionUnused_CheckedChanged);
			// 
			// checkBox_P4ChargeCycleUnused
			// 
			this->checkBox_P4ChargeCycleUnused->AutoSize = true;
			this->checkBox_P4ChargeCycleUnused->Location = System::Drawing::Point(237, 7);
			this->checkBox_P4ChargeCycleUnused->Name = L"checkBox_P4ChargeCycleUnused";
			this->checkBox_P4ChargeCycleUnused->Size = System::Drawing::Size(63, 17);
			this->checkBox_P4ChargeCycleUnused->TabIndex = 12;
			this->checkBox_P4ChargeCycleUnused->Text = L"Unused";
			this->checkBox_P4ChargeCycleUnused->UseVisualStyleBackColor = true;
			this->checkBox_P4ChargeCycleUnused->CheckedChanged += gcnew System::EventHandler(this, &LEVSensor::checkBox_P4ChargeCycleUnused_CheckedChanged);
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
			// numericUpDown_P4DistanceOnCharge
			// 
			this->numericUpDown_P4DistanceOnCharge->DecimalPlaces = 1;
			this->numericUpDown_P4DistanceOnCharge->Location = System::Drawing::Point(153, 53);
			this->numericUpDown_P4DistanceOnCharge->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {65535, 0, 0, 65536});
			this->numericUpDown_P4DistanceOnCharge->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
			this->numericUpDown_P4DistanceOnCharge->Name = L"numericUpDown_P4DistanceOnCharge";
			this->numericUpDown_P4DistanceOnCharge->Size = System::Drawing::Size(54, 20);
			this->numericUpDown_P4DistanceOnCharge->TabIndex = 10;
			this->numericUpDown_P4DistanceOnCharge->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {4014, 0, 0, 65536});
			this->numericUpDown_P4DistanceOnCharge->ValueChanged += gcnew System::EventHandler(this, &LEVSensor::numericUpDown_P4DistanceOnCharge_ValueChanged);
			// 
			// numericUpDown_P4FuelConsumption
			// 
			this->numericUpDown_P4FuelConsumption->DecimalPlaces = 1;
			this->numericUpDown_P4FuelConsumption->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
			this->numericUpDown_P4FuelConsumption->Location = System::Drawing::Point(153, 30);
			this->numericUpDown_P4FuelConsumption->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {4095, 0, 0, 65536});
			this->numericUpDown_P4FuelConsumption->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
			this->numericUpDown_P4FuelConsumption->Name = L"numericUpDown_P4FuelConsumption";
			this->numericUpDown_P4FuelConsumption->Size = System::Drawing::Size(54, 20);
			this->numericUpDown_P4FuelConsumption->TabIndex = 9;
			this->numericUpDown_P4FuelConsumption->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {105, 0, 0, 65536});
			this->numericUpDown_P4FuelConsumption->ValueChanged += gcnew System::EventHandler(this, &LEVSensor::numericUpDown_P4FuelConsumption_ValueChanged);
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
			// tabPage_Page4Cont
			// 
			this->tabPage_Page4Cont->Controls->Add(this->groupBox_P4Voltage);
			this->tabPage_Page4Cont->Location = System::Drawing::Point(4, 22);
			this->tabPage_Page4Cont->Name = L"tabPage_Page4Cont";
			this->tabPage_Page4Cont->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_Page4Cont->Size = System::Drawing::Size(375, 77);
			this->tabPage_Page4Cont->TabIndex = 7;
			this->tabPage_Page4Cont->Text = L"Page 4 Cont\'d";
			this->tabPage_Page4Cont->UseVisualStyleBackColor = true;
			// 
			// groupBox_P4Voltage
			// 
			this->groupBox_P4Voltage->Controls->Add(this->numericUpDown_P4Voltage);
			this->groupBox_P4Voltage->Controls->Add(this->checkBox_P4VoltUnused);
			this->groupBox_P4Voltage->Location = System::Drawing::Point(125, 14);
			this->groupBox_P4Voltage->Name = L"groupBox_P4Voltage";
			this->groupBox_P4Voltage->Size = System::Drawing::Size(125, 48);
			this->groupBox_P4Voltage->TabIndex = 16;
			this->groupBox_P4Voltage->TabStop = false;
			this->groupBox_P4Voltage->Text = L"Batt Voltage (1/4V)";
			// 
			// numericUpDown_P4Voltage
			// 
			this->numericUpDown_P4Voltage->Location = System::Drawing::Point(6, 19);
			this->numericUpDown_P4Voltage->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
			this->numericUpDown_P4Voltage->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->numericUpDown_P4Voltage->Name = L"numericUpDown_P4Voltage";
			this->numericUpDown_P4Voltage->Size = System::Drawing::Size(45, 20);
			this->numericUpDown_P4Voltage->TabIndex = 5;
			this->numericUpDown_P4Voltage->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
			this->numericUpDown_P4Voltage->ValueChanged += gcnew System::EventHandler(this, &LEVSensor::numericUpDown_P4Voltage_ValueChanged);
			// 
			// checkBox_P4VoltUnused
			// 
			this->checkBox_P4VoltUnused->AutoSize = true;
			this->checkBox_P4VoltUnused->Location = System::Drawing::Point(57, 20);
			this->checkBox_P4VoltUnused->Name = L"checkBox_P4VoltUnused";
			this->checkBox_P4VoltUnused->Size = System::Drawing::Size(63, 17);
			this->checkBox_P4VoltUnused->TabIndex = 8;
			this->checkBox_P4VoltUnused->Text = L"Unused";
			this->checkBox_P4VoltUnused->UseVisualStyleBackColor = true;
			this->checkBox_P4VoltUnused->CheckedChanged += gcnew System::EventHandler(this, &LEVSensor::checkBox_P4VoltUnused_CheckedChanged);
			// 
			// tabPage_Page5
			// 
			this->tabPage_Page5->Controls->Add(this->label_P5);
			this->tabPage_Page5->Controls->Add(this->checkBox_P5WheelCircum);
			this->tabPage_Page5->Controls->Add(this->numericUpDown_P5Circum);
			this->tabPage_Page5->Controls->Add(this->label_P5Circum);
			this->tabPage_Page5->Controls->Add(this->label_P5RegenModes);
			this->tabPage_Page5->Controls->Add(this->label_P5AssistModes);
			this->tabPage_Page5->Controls->Add(this->numericUpDown_P5RegenModes);
			this->tabPage_Page5->Controls->Add(this->numericUpDwn_P5AssistModes);
			this->tabPage_Page5->Location = System::Drawing::Point(4, 22);
			this->tabPage_Page5->Name = L"tabPage_Page5";
			this->tabPage_Page5->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_Page5->Size = System::Drawing::Size(375, 77);
			this->tabPage_Page5->TabIndex = 6;
			this->tabPage_Page5->Text = L"Page 5";
			this->tabPage_Page5->UseVisualStyleBackColor = true;
			// 
			// label_P5
			// 
			this->label_P5->AutoSize = true;
			this->label_P5->Location = System::Drawing::Point(6, 9);
			this->label_P5->Name = L"label_P5";
			this->label_P5->Size = System::Drawing::Size(164, 13);
			this->label_P5->TabIndex = 19;
			this->label_P5->Text = L"Travel Modes Supported by LEV:";
			// 
			// checkBox_P5WheelCircum
			// 
			this->checkBox_P5WheelCircum->AutoSize = true;
			this->checkBox_P5WheelCircum->Location = System::Drawing::Point(277, 38);
			this->checkBox_P5WheelCircum->Name = L"checkBox_P5WheelCircum";
			this->checkBox_P5WheelCircum->Size = System::Drawing::Size(63, 17);
			this->checkBox_P5WheelCircum->TabIndex = 18;
			this->checkBox_P5WheelCircum->Text = L"Unused";
			this->checkBox_P5WheelCircum->UseVisualStyleBackColor = true;
			this->checkBox_P5WheelCircum->CheckedChanged += gcnew System::EventHandler(this, &LEVSensor::checkBox_P5WheelCircum_CheckedChanged);
			// 
			// numericUpDown_P5Circum
			// 
			this->numericUpDown_P5Circum->Location = System::Drawing::Point(206, 35);
			this->numericUpDown_P5Circum->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {4095, 0, 0, 0});
			this->numericUpDown_P5Circum->Name = L"numericUpDown_P5Circum";
			this->numericUpDown_P5Circum->Size = System::Drawing::Size(54, 20);
			this->numericUpDown_P5Circum->TabIndex = 17;
			this->numericUpDown_P5Circum->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {2000, 0, 0, 0});
			this->numericUpDown_P5Circum->ValueChanged += gcnew System::EventHandler(this, &LEVSensor::numericUpDown_P5Circum_ValueChanged);
			// 
			// label_P5Circum
			// 
			this->label_P5Circum->AutoSize = true;
			this->label_P5Circum->Location = System::Drawing::Point(203, 19);
			this->label_P5Circum->Name = L"label_P5Circum";
			this->label_P5Circum->Size = System::Drawing::Size(137, 13);
			this->label_P5Circum->TabIndex = 16;
			this->label_P5Circum->Text = L"Wheel Circumference (mm):";
			// 
			// label_P5RegenModes
			// 
			this->label_P5RegenModes->AutoSize = true;
			this->label_P5RegenModes->Location = System::Drawing::Point(13, 53);
			this->label_P5RegenModes->Name = L"label_P5RegenModes";
			this->label_P5RegenModes->Size = System::Drawing::Size(87, 13);
			this->label_P5RegenModes->TabIndex = 15;
			this->label_P5RegenModes->Text = L"# Regen Modes:";
			// 
			// label_P5AssistModes
			// 
			this->label_P5AssistModes->AutoSize = true;
			this->label_P5AssistModes->Location = System::Drawing::Point(13, 30);
			this->label_P5AssistModes->Name = L"label_P5AssistModes";
			this->label_P5AssistModes->Size = System::Drawing::Size(82, 13);
			this->label_P5AssistModes->TabIndex = 14;
			this->label_P5AssistModes->Text = L"# Assist Modes:";
			// 
			// numericUpDown_P5RegenModes
			// 
			this->numericUpDown_P5RegenModes->Location = System::Drawing::Point(106, 51);
			this->numericUpDown_P5RegenModes->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {7, 0, 0, 0});
			this->numericUpDown_P5RegenModes->Name = L"numericUpDown_P5RegenModes";
			this->numericUpDown_P5RegenModes->Size = System::Drawing::Size(35, 20);
			this->numericUpDown_P5RegenModes->TabIndex = 13;
			this->numericUpDown_P5RegenModes->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {7, 0, 0, 0});
			this->numericUpDown_P5RegenModes->ValueChanged += gcnew System::EventHandler(this, &LEVSensor::numericUpDown_P5RegenModes_ValueChanged);
			// 
			// numericUpDwn_P5AssistModes
			// 
			this->numericUpDwn_P5AssistModes->Location = System::Drawing::Point(106, 28);
			this->numericUpDwn_P5AssistModes->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {7, 0, 0, 0});
			this->numericUpDwn_P5AssistModes->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->numericUpDwn_P5AssistModes->Name = L"numericUpDwn_P5AssistModes";
			this->numericUpDwn_P5AssistModes->Size = System::Drawing::Size(35, 20);
			this->numericUpDwn_P5AssistModes->TabIndex = 12;
			this->numericUpDwn_P5AssistModes->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {7, 0, 0, 0});
			this->numericUpDwn_P5AssistModes->ValueChanged += gcnew System::EventHandler(this, &LEVSensor::numericUpDwn_P5AssistModes_ValueChanged);
			// 
			// tabPage_P16
			// 
			this->tabPage_P16->Controls->Add(this->label_P16ManIDValue);
			this->tabPage_P16->Controls->Add(this->label_P16ManuID);
			this->tabPage_P16->Controls->Add(this->checkBox_P16Supported);
			this->tabPage_P16->Location = System::Drawing::Point(4, 22);
			this->tabPage_P16->Name = L"tabPage_P16";
			this->tabPage_P16->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_P16->Size = System::Drawing::Size(389, 111);
			this->tabPage_P16->TabIndex = 10;
			this->tabPage_P16->Text = L"Page 16";
			this->tabPage_P16->UseVisualStyleBackColor = true;
			// 
			// label_P16ManIDValue
			// 
			this->label_P16ManIDValue->AutoSize = true;
			this->label_P16ManIDValue->Location = System::Drawing::Point(244, 69);
			this->label_P16ManIDValue->Name = L"label_P16ManIDValue";
			this->label_P16ManIDValue->Size = System::Drawing::Size(31, 13);
			this->label_P16ManIDValue->TabIndex = 4;
			this->label_P16ManIDValue->Text = L"-- -- --";
			// 
			// label_P16ManuID
			// 
			this->label_P16ManuID->AutoSize = true;
			this->label_P16ManuID->Location = System::Drawing::Point(114, 69);
			this->label_P16ManuID->Name = L"label_P16ManuID";
			this->label_P16ManuID->Size = System::Drawing::Size(124, 13);
			this->label_P16ManuID->TabIndex = 3;
			this->label_P16ManuID->Text = L"Display Manufacturer ID:";
			// 
			// checkBox_P16Supported
			// 
			this->checkBox_P16Supported->AutoSize = true;
			this->checkBox_P16Supported->Checked = true;
			this->checkBox_P16Supported->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBox_P16Supported->Location = System::Drawing::Point(117, 29);
			this->checkBox_P16Supported->Name = L"checkBox_P16Supported";
			this->checkBox_P16Supported->Size = System::Drawing::Size(134, 17);
			this->checkBox_P16Supported->TabIndex = 1;
			this->checkBox_P16Supported->Text = L"Rx Page 16 Supported";
			this->checkBox_P16Supported->UseVisualStyleBackColor = true;
			this->checkBox_P16Supported->CheckedChanged += gcnew System::EventHandler(this, &LEVSensor::checkBox_P16Supported_CheckedChanged);
			// 
			// tabPage_Common1
			// 
			this->tabPage_Common1->Controls->Add(this->checkBox_NoSerial);
			this->tabPage_Common1->Controls->Add(this->label_Common1);
			this->tabPage_Common1->Controls->Add(this->label_SoftwareVer);
			this->tabPage_Common1->Controls->Add(this->label_HardwareVer);
			this->tabPage_Common1->Controls->Add(this->button_CommonUpdate);
			this->tabPage_Common1->Controls->Add(this->label_ErrorCommon);
			this->tabPage_Common1->Controls->Add(this->label_ManfID);
			this->tabPage_Common1->Controls->Add(this->label_SerialNum);
			this->tabPage_Common1->Controls->Add(this->textBox_SerialNum);
			this->tabPage_Common1->Controls->Add(this->textBox_SwVersion);
			this->tabPage_Common1->Controls->Add(this->textBox_HwVersion);
			this->tabPage_Common1->Controls->Add(this->textBox_ModelNum);
			this->tabPage_Common1->Controls->Add(this->textBox_MfgID);
			this->tabPage_Common1->Controls->Add(this->label_ModelNum);
			this->tabPage_Common1->Location = System::Drawing::Point(4, 22);
			this->tabPage_Common1->Name = L"tabPage_Common1";
			this->tabPage_Common1->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_Common1->Size = System::Drawing::Size(389, 111);
			this->tabPage_Common1->TabIndex = 11;
			this->tabPage_Common1->Text = L"Common Tx";
			this->tabPage_Common1->UseVisualStyleBackColor = true;
			// 
			// checkBox_NoSerial
			// 
			this->checkBox_NoSerial->AutoSize = true;
			this->checkBox_NoSerial->Location = System::Drawing::Point(172, 31);
			this->checkBox_NoSerial->Name = L"checkBox_NoSerial";
			this->checkBox_NoSerial->Size = System::Drawing::Size(79, 17);
			this->checkBox_NoSerial->TabIndex = 54;
			this->checkBox_NoSerial->Text = L"No Serial #";
			this->checkBox_NoSerial->UseVisualStyleBackColor = true;
			this->checkBox_NoSerial->CheckedChanged += gcnew System::EventHandler(this, &LEVSensor::checkBox_NoSerial_CheckedChanged);
			// 
			// label_Common1
			// 
			this->label_Common1->AutoSize = true;
			this->label_Common1->Location = System::Drawing::Point(6, 8);
			this->label_Common1->Name = L"label_Common1";
			this->label_Common1->Size = System::Drawing::Size(136, 13);
			this->label_Common1->TabIndex = 53;
			this->label_Common1->Text = L"LEV Sensor Common Data:";
			// 
			// label_SoftwareVer
			// 
			this->label_SoftwareVer->AutoSize = true;
			this->label_SoftwareVer->Location = System::Drawing::Point(261, 55);
			this->label_SoftwareVer->Name = L"label_SoftwareVer";
			this->label_SoftwareVer->Size = System::Drawing::Size(71, 13);
			this->label_SoftwareVer->TabIndex = 44;
			this->label_SoftwareVer->Text = L"Software Ver:";
			// 
			// label_HardwareVer
			// 
			this->label_HardwareVer->AutoSize = true;
			this->label_HardwareVer->Location = System::Drawing::Point(257, 32);
			this->label_HardwareVer->Name = L"label_HardwareVer";
			this->label_HardwareVer->Size = System::Drawing::Size(75, 13);
			this->label_HardwareVer->TabIndex = 43;
			this->label_HardwareVer->Text = L"Hardware Ver:";
			// 
			// button_CommonUpdate
			// 
			this->button_CommonUpdate->Location = System::Drawing::Point(260, 82);
			this->button_CommonUpdate->Name = L"button_CommonUpdate";
			this->button_CommonUpdate->Size = System::Drawing::Size(97, 20);
			this->button_CommonUpdate->TabIndex = 52;
			this->button_CommonUpdate->Text = L"Update All";
			this->button_CommonUpdate->UseVisualStyleBackColor = true;
			this->button_CommonUpdate->Click += gcnew System::EventHandler(this, &LEVSensor::button_CommonUpdate_Click);
			// 
			// label_ErrorCommon
			// 
			this->label_ErrorCommon->AutoSize = true;
			this->label_ErrorCommon->ForeColor = System::Drawing::Color::Red;
			this->label_ErrorCommon->Location = System::Drawing::Point(172, 86);
			this->label_ErrorCommon->Name = L"label_ErrorCommon";
			this->label_ErrorCommon->Size = System::Drawing::Size(32, 13);
			this->label_ErrorCommon->TabIndex = 46;
			this->label_ErrorCommon->Text = L"Error:";
			this->label_ErrorCommon->Visible = false;
			// 
			// label_ManfID
			// 
			this->label_ManfID->AutoSize = true;
			this->label_ManfID->Location = System::Drawing::Point(38, 55);
			this->label_ManfID->Name = L"label_ManfID";
			this->label_ManfID->Size = System::Drawing::Size(51, 13);
			this->label_ManfID->TabIndex = 41;
			this->label_ManfID->Text = L"Manf. ID:";
			// 
			// label_SerialNum
			// 
			this->label_SerialNum->AutoSize = true;
			this->label_SerialNum->Location = System::Drawing::Point(38, 32);
			this->label_SerialNum->Name = L"label_SerialNum";
			this->label_SerialNum->Size = System::Drawing::Size(46, 13);
			this->label_SerialNum->TabIndex = 42;
			this->label_SerialNum->Text = L"Serial #:";
			// 
			// textBox_SerialNum
			// 
			this->textBox_SerialNum->Location = System::Drawing::Point(90, 29);
			this->textBox_SerialNum->MaxLength = 10;
			this->textBox_SerialNum->Name = L"textBox_SerialNum";
			this->textBox_SerialNum->Size = System::Drawing::Size(76, 20);
			this->textBox_SerialNum->TabIndex = 49;
			this->textBox_SerialNum->Text = L"987654321";
			// 
			// textBox_SwVersion
			// 
			this->textBox_SwVersion->Location = System::Drawing::Point(338, 52);
			this->textBox_SwVersion->MaxLength = 3;
			this->textBox_SwVersion->Name = L"textBox_SwVersion";
			this->textBox_SwVersion->Size = System::Drawing::Size(29, 20);
			this->textBox_SwVersion->TabIndex = 51;
			this->textBox_SwVersion->Text = L"2";
			// 
			// textBox_HwVersion
			// 
			this->textBox_HwVersion->Location = System::Drawing::Point(338, 29);
			this->textBox_HwVersion->MaxLength = 3;
			this->textBox_HwVersion->Name = L"textBox_HwVersion";
			this->textBox_HwVersion->Size = System::Drawing::Size(29, 20);
			this->textBox_HwVersion->TabIndex = 50;
			this->textBox_HwVersion->Text = L"2";
			// 
			// textBox_ModelNum
			// 
			this->textBox_ModelNum->Location = System::Drawing::Point(117, 76);
			this->textBox_ModelNum->MaxLength = 5;
			this->textBox_ModelNum->Name = L"textBox_ModelNum";
			this->textBox_ModelNum->Size = System::Drawing::Size(49, 20);
			this->textBox_ModelNum->TabIndex = 48;
			this->textBox_ModelNum->Text = L"22448";
			// 
			// textBox_MfgID
			// 
			this->textBox_MfgID->Location = System::Drawing::Point(117, 52);
			this->textBox_MfgID->MaxLength = 5;
			this->textBox_MfgID->Name = L"textBox_MfgID";
			this->textBox_MfgID->Size = System::Drawing::Size(49, 20);
			this->textBox_MfgID->TabIndex = 47;
			this->textBox_MfgID->Text = L"12345";
			// 
			// label_ModelNum
			// 
			this->label_ModelNum->AutoSize = true;
			this->label_ModelNum->Location = System::Drawing::Point(38, 79);
			this->label_ModelNum->Name = L"label_ModelNum";
			this->label_ModelNum->Size = System::Drawing::Size(49, 13);
			this->label_ModelNum->TabIndex = 45;
			this->label_ModelNum->Text = L"Model #:";
			// 
			// tabPage_Common2
			// 
			this->tabPage_Common2->Controls->Add(this->groupBox_P81Rx);
			this->tabPage_Common2->Controls->Add(this->groupBox_P80Rx);
			this->tabPage_Common2->Controls->Add(this->label_Common2);
			this->tabPage_Common2->Location = System::Drawing::Point(4, 22);
			this->tabPage_Common2->Name = L"tabPage_Common2";
			this->tabPage_Common2->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_Common2->Size = System::Drawing::Size(389, 111);
			this->tabPage_Common2->TabIndex = 12;
			this->tabPage_Common2->Text = L"Common Rx";
			this->tabPage_Common2->UseVisualStyleBackColor = true;
			// 
			// groupBox_P81Rx
			// 
			this->groupBox_P81Rx->Controls->Add(this->label_Glb_SoftwareVer);
			this->groupBox_P81Rx->Controls->Add(this->label_Glb_SoftwareVerDisplay);
			this->groupBox_P81Rx->Controls->Add(this->label_Glb_SerialNumDisplay);
			this->groupBox_P81Rx->Controls->Add(this->label_Glb_SerialNum);
			this->groupBox_P81Rx->Location = System::Drawing::Point(197, 26);
			this->groupBox_P81Rx->Name = L"groupBox_P81Rx";
			this->groupBox_P81Rx->Size = System::Drawing::Size(162, 79);
			this->groupBox_P81Rx->TabIndex = 56;
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
			this->groupBox_P80Rx->Location = System::Drawing::Point(27, 26);
			this->groupBox_P80Rx->Name = L"groupBox_P80Rx";
			this->groupBox_P80Rx->Size = System::Drawing::Size(164, 79);
			this->groupBox_P80Rx->TabIndex = 55;
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
			// label_Common2
			// 
			this->label_Common2->AutoSize = true;
			this->label_Common2->Location = System::Drawing::Point(6, 8);
			this->label_Common2->Name = L"label_Common2";
			this->label_Common2->Size = System::Drawing::Size(137, 13);
			this->label_Common2->TabIndex = 54;
			this->label_Common2->Text = L"LEV Display Common Data:";
			// 
			// panel_Display
			// 
			this->panel_Display->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->panel_Display->Controls->Add(this->label_StatusAssistVal);
			this->panel_Display->Controls->Add(this->label_StatusAssist);
			this->panel_Display->Controls->Add(this->label_StatusSpeedVal);
			this->panel_Display->Controls->Add(this->label_StatusSpeed);
			this->panel_Display->Controls->Add(this->label_StatusOdoVal);
			this->panel_Display->Controls->Add(this->label_StatusOdometer);
			this->panel_Display->Controls->Add(this->label_TxStatusBox);
			this->panel_Display->Location = System::Drawing::Point(58, 188);
			this->panel_Display->Name = L"panel_Display";
			this->panel_Display->Size = System::Drawing::Size(200, 90);
			this->panel_Display->TabIndex = 1;
			// 
			// label_StatusAssistVal
			// 
			this->label_StatusAssistVal->AutoSize = true;
			this->label_StatusAssistVal->Location = System::Drawing::Point(126, 61);
			this->label_StatusAssistVal->Name = L"label_StatusAssistVal";
			this->label_StatusAssistVal->Size = System::Drawing::Size(16, 13);
			this->label_StatusAssistVal->TabIndex = 29;
			this->label_StatusAssistVal->Text = L"---";
			// 
			// label_StatusAssist
			// 
			this->label_StatusAssist->AutoSize = true;
			this->label_StatusAssist->Location = System::Drawing::Point(6, 61);
			this->label_StatusAssist->Name = L"label_StatusAssist";
			this->label_StatusAssist->Size = System::Drawing::Size(48, 13);
			this->label_StatusAssist->TabIndex = 28;
			this->label_StatusAssist->Text = L"% Assist:";
			// 
			// label_StatusSpeedVal
			// 
			this->label_StatusSpeedVal->AutoSize = true;
			this->label_StatusSpeedVal->Location = System::Drawing::Point(126, 31);
			this->label_StatusSpeedVal->Name = L"label_StatusSpeedVal";
			this->label_StatusSpeedVal->Size = System::Drawing::Size(16, 13);
			this->label_StatusSpeedVal->TabIndex = 27;
			this->label_StatusSpeedVal->Text = L"---";
			// 
			// label_StatusSpeed
			// 
			this->label_StatusSpeed->AutoSize = true;
			this->label_StatusSpeed->Location = System::Drawing::Point(6, 31);
			this->label_StatusSpeed->Name = L"label_StatusSpeed";
			this->label_StatusSpeed->Size = System::Drawing::Size(93, 13);
			this->label_StatusSpeed->TabIndex = 26;
			this->label_StatusSpeed->Text = L"Speed (0.1 km/h):";
			// 
			// label_StatusOdoVal
			// 
			this->label_StatusOdoVal->AutoSize = true;
			this->label_StatusOdoVal->Location = System::Drawing::Point(126, 46);
			this->label_StatusOdoVal->Name = L"label_StatusOdoVal";
			this->label_StatusOdoVal->Size = System::Drawing::Size(16, 13);
			this->label_StatusOdoVal->TabIndex = 25;
			this->label_StatusOdoVal->Text = L"---";
			// 
			// label_StatusOdometer
			// 
			this->label_StatusOdometer->AutoSize = true;
			this->label_StatusOdometer->Location = System::Drawing::Point(6, 46);
			this->label_StatusOdometer->Name = L"label_StatusOdometer";
			this->label_StatusOdometer->Size = System::Drawing::Size(103, 13);
			this->label_StatusOdometer->TabIndex = 24;
			this->label_StatusOdometer->Text = L"Odometer (0.01 km):";
			// 
			// label_TxStatusBox
			// 
			this->label_TxStatusBox->AutoSize = true;
			this->label_TxStatusBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Underline, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->label_TxStatusBox->Location = System::Drawing::Point(3, 8);
			this->label_TxStatusBox->Name = L"label_TxStatusBox";
			this->label_TxStatusBox->Size = System::Drawing::Size(139, 13);
			this->label_TxStatusBox->TabIndex = 23;
			this->label_TxStatusBox->Text = L"Current LEV Tx Data (Raw):";
			// 
			// tabPage_P16TravelMode
			// 
			this->tabPage_P16TravelMode->Controls->Add(this->checkBox_P16TravelMode);
			this->tabPage_P16TravelMode->Controls->Add(this->label_P16Regen);
			this->tabPage_P16TravelMode->Controls->Add(this->label_P16Assist);
			this->tabPage_P16TravelMode->Controls->Add(this->comboBox_P16Assist);
			this->tabPage_P16TravelMode->Controls->Add(this->comboBox_P16Regen);
			this->tabPage_P16TravelMode->Location = System::Drawing::Point(4, 22);
			this->tabPage_P16TravelMode->Name = L"tabPage_P16TravelMode";
			this->tabPage_P16TravelMode->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_P16TravelMode->Size = System::Drawing::Size(375, 77);
			this->tabPage_P16TravelMode->TabIndex = 0;
			this->tabPage_P16TravelMode->Text = L"Travel Mode";
			this->tabPage_P16TravelMode->UseVisualStyleBackColor = true;
			// 
			// checkBox_P16TravelMode
			// 
			this->checkBox_P16TravelMode->AutoSize = true;
			this->checkBox_P16TravelMode->Location = System::Drawing::Point(6, 6);
			this->checkBox_P16TravelMode->Name = L"checkBox_P16TravelMode";
			this->checkBox_P16TravelMode->Size = System::Drawing::Size(102, 17);
			this->checkBox_P16TravelMode->TabIndex = 23;
			this->checkBox_P16TravelMode->Text = L"Mode Unknown";
			this->checkBox_P16TravelMode->UseVisualStyleBackColor = true;
			// 
			// label_P16Regen
			// 
			this->label_P16Regen->AutoSize = true;
			this->label_P16Regen->Location = System::Drawing::Point(183, 26);
			this->label_P16Regen->Name = L"label_P16Regen";
			this->label_P16Regen->Size = System::Drawing::Size(140, 13);
			this->label_P16Regen->TabIndex = 22;
			this->label_P16Regen->Text = L"Current Regenerative Level:";
			// 
			// label_P16Assist
			// 
			this->label_P16Assist->AutoSize = true;
			this->label_P16Assist->Location = System::Drawing::Point(51, 26);
			this->label_P16Assist->Name = L"label_P16Assist";
			this->label_P16Assist->Size = System::Drawing::Size(103, 13);
			this->label_P16Assist->TabIndex = 21;
			this->label_P16Assist->Text = L"Current Assist Level:";
			// 
			// comboBox_P16Assist
			// 
			this->comboBox_P16Assist->FormattingEnabled = true;
			this->comboBox_P16Assist->Items->AddRange(gcnew cli::array< System::Object^  >(8) {L"Assist Off", L"Assist 1", L"Assist 2", 
				L"Assist 3", L"Assist 4", L"Assist 5", L"Assist 6", L"Assist 7"});
			this->comboBox_P16Assist->Location = System::Drawing::Point(54, 42);
			this->comboBox_P16Assist->Name = L"comboBox_P16Assist";
			this->comboBox_P16Assist->Size = System::Drawing::Size(110, 21);
			this->comboBox_P16Assist->TabIndex = 20;
			this->comboBox_P16Assist->Text = L"Assist Off";
			// 
			// comboBox_P16Regen
			// 
			this->comboBox_P16Regen->FormattingEnabled = true;
			this->comboBox_P16Regen->Items->AddRange(gcnew cli::array< System::Object^  >(8) {L"Regenerative Off", L"Regenerative 1", 
				L"Regenerative 2", L"Regenerative 3", L"Regenerative 4", L"Regenerative 5", L"Regenerative 6", L"Regenerative 7"});
			this->comboBox_P16Regen->Location = System::Drawing::Point(186, 42);
			this->comboBox_P16Regen->Name = L"comboBox_P16Regen";
			this->comboBox_P16Regen->Size = System::Drawing::Size(110, 21);
			this->comboBox_P16Regen->TabIndex = 19;
			this->comboBox_P16Regen->Text = L"Regenerative Off";
			// 
			// tabPage_P16Command
			// 
			this->tabPage_P16Command->Controls->Add(this->groupBox_P16Gear);
			this->tabPage_P16Command->Controls->Add(this->checkBox_P16RightTurnOn);
			this->tabPage_P16Command->Controls->Add(this->checkBox_P16LeftTurnOn);
			this->tabPage_P16Command->Controls->Add(this->checkBox_P16HighBeamOn);
			this->tabPage_P16Command->Controls->Add(this->checkBox_P16LightOn);
			this->tabPage_P16Command->Location = System::Drawing::Point(4, 22);
			this->tabPage_P16Command->Name = L"tabPage_P16Command";
			this->tabPage_P16Command->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_P16Command->Size = System::Drawing::Size(375, 77);
			this->tabPage_P16Command->TabIndex = 1;
			this->tabPage_P16Command->Text = L"Display Command";
			this->tabPage_P16Command->UseVisualStyleBackColor = true;
			// 
			// groupBox_P16Gear
			// 
			this->groupBox_P16Gear->Controls->Add(this->label_P16FrontGear);
			this->groupBox_P16Gear->Controls->Add(this->comboBox_P16FrontGear);
			this->groupBox_P16Gear->Controls->Add(this->label_P16RearGear);
			this->groupBox_P16Gear->Controls->Add(this->comboBox_P16RearGear);
			this->groupBox_P16Gear->Location = System::Drawing::Point(6, 5);
			this->groupBox_P16Gear->Name = L"groupBox_P16Gear";
			this->groupBox_P16Gear->Size = System::Drawing::Size(133, 67);
			this->groupBox_P16Gear->TabIndex = 27;
			this->groupBox_P16Gear->TabStop = false;
			this->groupBox_P16Gear->Text = L"Current Gear";
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
			// comboBox_P16FrontGear
			// 
			this->comboBox_P16FrontGear->FormattingEnabled = true;
			this->comboBox_P16FrontGear->Items->AddRange(gcnew cli::array< System::Object^  >(4) {L"none", L"1", L"2", L"3"});
			this->comboBox_P16FrontGear->Location = System::Drawing::Point(73, 15);
			this->comboBox_P16FrontGear->Name = L"comboBox_P16FrontGear";
			this->comboBox_P16FrontGear->Size = System::Drawing::Size(47, 21);
			this->comboBox_P16FrontGear->TabIndex = 19;
			this->comboBox_P16FrontGear->Text = L"none";
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
			// 
			// checkBox_P16RightTurnOn
			// 
			this->checkBox_P16RightTurnOn->AutoSize = true;
			this->checkBox_P16RightTurnOn->Location = System::Drawing::Point(247, 41);
			this->checkBox_P16RightTurnOn->Name = L"checkBox_P16RightTurnOn";
			this->checkBox_P16RightTurnOn->Size = System::Drawing::Size(125, 17);
			this->checkBox_P16RightTurnOn->TabIndex = 26;
			this->checkBox_P16RightTurnOn->Text = L"Right Turn Signal On";
			this->checkBox_P16RightTurnOn->UseVisualStyleBackColor = true;
			// 
			// checkBox_P16LeftTurnOn
			// 
			this->checkBox_P16LeftTurnOn->AutoSize = true;
			this->checkBox_P16LeftTurnOn->Location = System::Drawing::Point(247, 18);
			this->checkBox_P16LeftTurnOn->Name = L"checkBox_P16LeftTurnOn";
			this->checkBox_P16LeftTurnOn->Size = System::Drawing::Size(118, 17);
			this->checkBox_P16LeftTurnOn->TabIndex = 25;
			this->checkBox_P16LeftTurnOn->Text = L"Left Turn Signal On";
			this->checkBox_P16LeftTurnOn->UseVisualStyleBackColor = true;
			// 
			// checkBox_P16HighBeamOn
			// 
			this->checkBox_P16HighBeamOn->AutoSize = true;
			this->checkBox_P16HighBeamOn->Location = System::Drawing::Point(149, 18);
			this->checkBox_P16HighBeamOn->Name = L"checkBox_P16HighBeamOn";
			this->checkBox_P16HighBeamOn->Size = System::Drawing::Size(95, 17);
			this->checkBox_P16HighBeamOn->TabIndex = 24;
			this->checkBox_P16HighBeamOn->Text = L"High Beam On";
			this->checkBox_P16HighBeamOn->UseVisualStyleBackColor = true;
			// 
			// checkBox_P16LightOn
			// 
			this->checkBox_P16LightOn->AutoSize = true;
			this->checkBox_P16LightOn->Location = System::Drawing::Point(149, 41);
			this->checkBox_P16LightOn->Name = L"checkBox_P16LightOn";
			this->checkBox_P16LightOn->Size = System::Drawing::Size(66, 17);
			this->checkBox_P16LightOn->TabIndex = 23;
			this->checkBox_P16LightOn->Text = L"Light On";
			this->checkBox_P16LightOn->UseVisualStyleBackColor = true;
			// 
			// tabPage_P16ManID
			// 
			this->tabPage_P16ManID->Controls->Add(this->label_P16ManID);
			this->tabPage_P16ManID->Controls->Add(this->numericUpDown_P16ManID);
			this->tabPage_P16ManID->Location = System::Drawing::Point(4, 22);
			this->tabPage_P16ManID->Name = L"tabPage_P16ManID";
			this->tabPage_P16ManID->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_P16ManID->Size = System::Drawing::Size(375, 77);
			this->tabPage_P16ManID->TabIndex = 2;
			this->tabPage_P16ManID->Text = L"Manufacturer ID";
			this->tabPage_P16ManID->UseVisualStyleBackColor = true;
			// 
			// label_P16ManID
			// 
			this->label_P16ManID->AutoSize = true;
			this->label_P16ManID->Location = System::Drawing::Point(114, 31);
			this->label_P16ManID->Name = L"label_P16ManID";
			this->label_P16ManID->Size = System::Drawing::Size(87, 13);
			this->label_P16ManID->TabIndex = 1;
			this->label_P16ManID->Text = L"Manufacturer ID:";
			// 
			// numericUpDown_P16ManID
			// 
			this->numericUpDown_P16ManID->Location = System::Drawing::Point(207, 29);
			this->numericUpDown_P16ManID->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {65535, 0, 0, 0});
			this->numericUpDown_P16ManID->Name = L"numericUpDown_P16ManID";
			this->numericUpDown_P16ManID->Size = System::Drawing::Size(54, 20);
			this->numericUpDown_P16ManID->TabIndex = 0;
			this->numericUpDown_P16ManID->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {65535, 0, 0, 0});
			// 
			// LEVSensor
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(794, 351);
			this->Controls->Add(this->panel_Display);
			this->Controls->Add(this->panel_Settings);
			this->Name = L"LEVSensor";
			this->Text = L"Light Electric Vehicle (LEV)";
			this->panel_Settings->ResumeLayout(false);
			this->tabControl_LEV->ResumeLayout(false);
			this->tabPage_P1->ResumeLayout(false);
			this->tabControl_P1->ResumeLayout(false);
			this->tabPage_P1Mode->ResumeLayout(false);
			this->tabPage_P1Mode->PerformLayout();
			this->tabPage_P1State->ResumeLayout(false);
			this->tabPage_P1State->PerformLayout();
			this->tabPage_P1Gear->ResumeLayout(false);
			this->tabPage_P1Gear->PerformLayout();
			this->groupBox_P1CurrentGear->ResumeLayout(false);
			this->groupBox_P1CurrentGear->PerformLayout();
			this->tabPage_P1Error->ResumeLayout(false);
			this->tabPage_P1Error->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_P1CustomError))->EndInit();
			this->tabPage_P1Speed->ResumeLayout(false);
			this->tabPage_P1Speed->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_P1Speed))->EndInit();
			this->tabPage_P1Temp->ResumeLayout(false);
			this->tabPage_P1Temp->PerformLayout();
			this->tabPage_P2->ResumeLayout(false);
			this->tabPage_P2->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_P2Speed))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_P2Range))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_P2Distance))->EndInit();
			this->tabPage_P3->ResumeLayout(false);
			this->tabControl_Page3->ResumeLayout(false);
			this->tabPage_P3BatterySOC->ResumeLayout(false);
			this->tabPage_P3BatterySOC->PerformLayout();
			this->groupBox_P3Charge->ResumeLayout(false);
			this->groupBox_P3Charge->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_P3Charge))->EndInit();
			this->tabPage_P3Assist->ResumeLayout(false);
			this->tabPage_P3Assist->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_P3PercentAssist))->EndInit();
			this->tabPage4->ResumeLayout(false);
			this->tabControl_Page45->ResumeLayout(false);
			this->tabPage_Page4->ResumeLayout(false);
			this->tabPage_Page4->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_P4ChargeCycle))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_P4DistanceOnCharge))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_P4FuelConsumption))->EndInit();
			this->tabPage_Page4Cont->ResumeLayout(false);
			this->groupBox_P4Voltage->ResumeLayout(false);
			this->groupBox_P4Voltage->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_P4Voltage))->EndInit();
			this->tabPage_Page5->ResumeLayout(false);
			this->tabPage_Page5->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_P5Circum))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_P5RegenModes))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDwn_P5AssistModes))->EndInit();
			this->tabPage_P16->ResumeLayout(false);
			this->tabPage_P16->PerformLayout();
			this->tabPage_Common1->ResumeLayout(false);
			this->tabPage_Common1->PerformLayout();
			this->tabPage_Common2->ResumeLayout(false);
			this->tabPage_Common2->PerformLayout();
			this->groupBox_P81Rx->ResumeLayout(false);
			this->groupBox_P81Rx->PerformLayout();
			this->groupBox_P80Rx->ResumeLayout(false);
			this->groupBox_P80Rx->PerformLayout();
			this->panel_Display->ResumeLayout(false);
			this->panel_Display->PerformLayout();
			this->tabPage_P16TravelMode->ResumeLayout(false);
			this->tabPage_P16TravelMode->PerformLayout();
			this->tabPage_P16Command->ResumeLayout(false);
			this->tabPage_P16Command->PerformLayout();
			this->groupBox_P16Gear->ResumeLayout(false);
			this->groupBox_P16Gear->PerformLayout();
			this->tabPage_P16ManID->ResumeLayout(false);
			this->tabPage_P16ManID->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_P16ManID))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion
};