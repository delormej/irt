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
#include "sdm.h"
#include "antplus_common.h"
#include "types.h"
#include "antdefines.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


public ref class SDMSensor : public System::Windows::Forms::Form, public ISimBase{
public:
	SDMSensor(System::Timers::Timer^ channelTimer, dRequestAckMsg^ channelAckMsg){
		requestAckMsg = channelAckMsg;
		commonPages = gcnew CommonData();
		timerHandle = channelTimer;		// Get timer handle		
		InitializeComponent();
		InitializeSim();		
	}

	~SDMSensor(){
		this->panel_Display->Controls->Clear();
		this->panel_Settings->Controls->Clear();
		delete this->panel_Display;
		delete this->panel_Settings;
		//clean up floating resources with the garbage collector
		GC::Collect(2);

		//Deletion of designer component
		if (components)
		{
			delete components;
		}
	}

public:	
	virtual void onTimerTock(USHORT eventTime);
	virtual void ANT_eventNotification(UCHAR ucEventCode_, UCHAR* pucEventBuffer_);
	virtual UCHAR getDeviceType(){return SDM_DEVICE_TYPE;}
	virtual UCHAR getTransmissionType(){return SDM_TX_TYPE;}
	virtual USHORT getTransmitPeriod(){return SDM_MSG_PERIOD;}
	virtual DOUBLE getTimerInterval(){return (double) ulTimerInterval;}
	virtual System::Windows::Forms::Panel^ getSimSettingsPanel(){return this->panel_Settings;}
	virtual System::Windows::Forms::Panel^ getSimTranslatedDisplay(){return this->panel_Display;}

private:
	void InitializeSim();
	void HandleTransmit(UCHAR* pucTxBuffer_);
	void UpdateDisplay();
	void ForceUpdate();
	void Page2_Enable();
	void Page2_Disable();
	void Page3_Enable();
	void Page3_Disable();
	void HandleReceive(UCHAR* pucRxBuffer_);
	void SendAckRequestMsg(UCHAR ucMsgCode_);
	BOOL HandleRetransmit();

	System::Void checkBox_EnableCommon_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void button_UpdateCommon_Click(System::Object^  sender, System::EventArgs^  e);
	System::Void listBox_SimUnitsChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_Sim_StrideLength_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_Sim_CurOutput_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_Sim_MinMaxOutput_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_Sim_BurnRate_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_Sweeping_CheckedChanged(System::Object^  sender, System::EventArgs^  e);	
	System::Void button_UpdateDistance_Click(System::Object^  sender, System::EventArgs^  e);
	System::Void button_UpdateStrides_Click(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_P1InstSpeed_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_P2InstSpeed_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_P3InstSpeed_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_P1Time_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_P1Latency_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_P1Distance_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_P2Cadence_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_P3Cadence_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void listBox_P2Location_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void listBox_P2Battery_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void listBox_P2Health_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void listBox_P2Use_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);	
	System::Void listBox_P3Location_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void listBox_P3Battery_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void listBox_P3Health_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void listBox_P3Use_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_Page2_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_Page3_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void button_P3UpdateCalories_Click(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_P22Enabled_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_P16Enabled_CheckedChanged(System::Object^  sender, System::EventArgs^  e);	
	System::Void button_P16UpdateStrides_Click(System::Object^  sender, System::EventArgs^  e);
	System::Void button_P16UpdateDistance_Click(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_P22Ignore_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_InvalidSerial_CheckedChanged(System::Object^  sender, System::EventArgs^  e);

	USHORT ConvertFromInput(System::Decimal decValue_);
	System::Decimal ConvertToInput(USHORT usValue_);
	USHORT CadenceToSpeed(USHORT usCadence16_);
	USHORT SpeedToCadence(USHORT usSpeed256_);

private:
	dRequestAckMsg^ requestAckMsg;		// Delegate for handling Ack data
	CommonData^ commonPages;			// Handle to common data pages 

	// Simulation timer
	System::Timers::Timer^ timerHandle;	// Timer handle
	ULONG ulTimerInterval;				// Timer interval (ms)
	ULONG ulRunTime;					// Run time at latest simulation event (ms)
	DWORD dwTimeLastEvent;				// Time of last event (ms), since system started
	DWORD dwTimeLastTx;					// Time of last Tx (ms), since system started

	// Simulator variables
	USHORT usSpeed256;		// Instantaneous speed (1/256 m/s)
	USHORT usMaxSpeed256;	// Maximum speed (1/256 m/s)
	USHORT usCurSpeed256;	// Current speed (1/256 m/s)
	USHORT usMinSpeed256;	// Minimum speed (1/256 m/s)
	USHORT usCadence16;		// Instantaneous cadence (1/16 m/s)
	USHORT usMaxCadence16;	// Maximum cadence (1/16 m/s)
	USHORT usCurCadence16;	// Current cadence (1/16 m/s)
	USHORT usMinCadence16;	// Minimum cadence (1/16 m/s)
	UCHAR ucStrideLength;	// Stride length (cm)
	USHORT usDistance16;	// Distance (1/16 m)
	UCHAR ucStrideCount;	// Cumulative stride count (strides)
	UCHAR ucStatusFlags;	// SDM status flags (encoded)
	SDMStatus sStatusFlags;	// SDM status flags (individual fields)
	UCHAR ucCapabFlags;		// Capabilities (Page 22) status flags (encoded)
	SDMCapab sCapabFlags;	// Capabilities status flags (individual fields, see sdm.h)
	USHORT usTime200;		// Time of last speed/distance computation (1/200 seconds)
	UCHAR ucLatency32;		// Time between last computation and transmission of this message (1/32 seconds)
	UCHAR ucCalorieCount;	// Calorie Count (kcal)
	ULONG ulTempCalories;	// Temporary variable to calculate the calories
	UCHAR ucCalorieRate;	// Calorie Burn Rate (K-kcal/m) (values from 1-200)
	ULONG ulStridesAccum;	// Total accumulated strides, one increment for 2 steps, rollover at 16777216
	ULONG ulDistanceAccum;	// Total accumulated distance (1/256 m)
	
	// Simulator options	
	BOOL bTxCommon;			// Enable/disable common pages
	BOOL bTxPage2;			// Enable/disable page 2
	BOOL bTxPage3;			// Enable/disable page 3
	BOOL bTxPage22;			// Enable/disable page 22
	BOOL bTxPage16;			// Enable/disable page 16
	BOOL bTxPage16Flag;		// Indicates Page 16 has been requested
	BOOL bTxPage22Flag;		// Indicates Page 22 has been requested
	BOOL bUseTime;			// Include time in transmission (Page 1)
	BOOL bUseDistance;		// Include distance in transmission (Page 1)
	BOOL bUseLatency;		// Include latency in transmission  (Page 1)
	BOOL bUseSpeed;			// Include speed in transmission (Page 1 and 2)
	BOOL bUseCadence;		// Include cadence in transmission (Page 2)
	BOOL bP22Ignore;		// Ignore P22 request if all fields are supported
	SDMUnit eUnit;			// Units of speed/cadence input
	UCHAR ucSimDataType;	// Method to generate simulated data
	UCHAR ucRequestCount;	// Used to count the number of times to re-send the request pages
	BOOL bSweepAscending;	// Sweep through values in ascending manner
	UCHAR ucMsgExpectingAck;	// Message pending to be acknowledged
	UCHAR ucAckRetryCount;		// Counts the number of retries attempted for sending an ACK message
	UCHAR ucMaxAckRetries;	// Maximum number of times for the sensor to attempt to resend an ack message if it does not rx an ack

private: System::Windows::Forms::Panel^  panel_Settings;
private: System::Windows::Forms::TabControl^  tabControl_Settings;
private: System::Windows::Forms::TabPage^  tabPage_Parameters;
private: System::Windows::Forms::TabPage^  tabPage_DeviceInfo;
private: System::Windows::Forms::Panel^  panel_Display;
private: System::Windows::Forms::Label^  label_Sim_ParamLabel;
private: System::Windows::Forms::Label^  label_PageType;
private: System::Windows::Forms::Label^  label_TransStrideCount;
private: System::Windows::Forms::Label^  label_TransInstSpeed;
private: System::Windows::Forms::Label^  label_TransInstSpeedDisplay;
private: System::Windows::Forms::Label^  label_TransStrideCountDisplay;
private: System::Windows::Forms::Label^  label_HardwareVer;
private: System::Windows::Forms::Label^  label_SerialNum;
private: System::Windows::Forms::Label^  label_ManfID;
private: System::Windows::Forms::Label^  label_SoftwareVer;
private: System::Windows::Forms::Label^  label_ModelNum;
private: System::Windows::Forms::Button^  button_UpdateCommon;
private: System::Windows::Forms::TextBox^  textBox_ModelNum;
private: System::Windows::Forms::TextBox^  textBox_SerialNum;
private: System::Windows::Forms::TextBox^  textBox_MfgID;
private: System::Windows::Forms::TextBox^  textBox_SwVersion;
private: System::Windows::Forms::TextBox^  textBox_HwVersion;
private: System::Windows::Forms::Label^  label_TransDisplayTitle;
private: System::Windows::Forms::Label^  label_ErrorCommon;
private: System::Windows::Forms::TabPage^  tabPage_Page1Settings;
private: System::Windows::Forms::TabPage^  tabPage_Page2Settings;
private: System::Windows::Forms::Panel^  panel_PageType;
private: System::Windows::Forms::CheckBox^  checkBox_Sweeping;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Sim_StrideLength;
private: System::Windows::Forms::Label^  label_StrideConvConst;
private: System::Windows::Forms::Label^  label_P1StrideCount;
private: System::Windows::Forms::Label^  label_P1TotalDistance;
private: System::Windows::Forms::Button^  button_UpdateDistance;
private: System::Windows::Forms::Label^  label_P1UpdateError;
private: System::Windows::Forms::TextBox^  textBox_P1StrideCountChange;
private: System::Windows::Forms::Label^  label_P1StrideCountDisplay;
private: System::Windows::Forms::TextBox^  textBox_P1TotDistChange;
private: System::Windows::Forms::Label^  label_P1TotDistDisplay;
private: System::Windows::Forms::ListBox^  listBox_P2Health;
private: System::Windows::Forms::ListBox^  listBox_P2Battery;
private: System::Windows::Forms::ListBox^  listBox_P2Location;
private: System::Windows::Forms::Label^  label_P2StatusFlags;
private: System::Windows::Forms::Label^  label_P2Use;
private: System::Windows::Forms::Label^  label_P2Health;
private: System::Windows::Forms::Label^  label_P2Battery;
private: System::Windows::Forms::Label^  label_P2Location;
private: System::Windows::Forms::ListBox^  listBox_P2Use;
private: System::Windows::Forms::Label^  label_P1IntReminder;
private: System::Windows::Forms::Label^  label_TransLatencyDisplay;
private: System::Windows::Forms::Label^  label_TransLatency;
private: System::Windows::Forms::Label^  label_TransDistDisplay;
private: System::Windows::Forms::Label^  label_TransDist;
private: System::Windows::Forms::Label^  label_TransCadenceDisplay;
private: System::Windows::Forms::Label^  label_TransCadence;
private: System::Windows::Forms::CheckBox^  checkBox_P1Time;
private: System::Windows::Forms::Label^  label_P1TxFields;
private: System::Windows::Forms::CheckBox^  checkBox_P1Latency;
private: System::Windows::Forms::CheckBox^  checkBox_P1InstSpeed;
private: System::Windows::Forms::CheckBox^  checkBox_P1Distance;
private: System::Windows::Forms::Label^  label_P2TxFields;
private: System::Windows::Forms::CheckBox^  checkBox_P2Cadence;
private: System::Windows::Forms::CheckBox^  checkBox_P2InstSpeed;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Sim_MaxOutput;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Sim_CurOutput;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Sim_MinOutput;
private: System::Windows::Forms::Label^  label_Sim_Max;
private: System::Windows::Forms::Label^  label_Sim_Current;
private: System::Windows::Forms::Label^  label_Sim_Min;
private: System::Windows::Forms::ListBox^  listBox_SimUnits;
private: System::Windows::Forms::Label^  label_Trn_TimeDisplay;
private: System::Windows::Forms::Label^  label_Trn_Time;
private: System::Windows::Forms::CheckBox^  checkBox_EnableCommon;
private: System::Windows::Forms::Button^  button_UpdateStrides;
private: System::Windows::Forms::TabPage^  tabPage_Page3Settings;
private: System::Windows::Forms::Label^  label_P3TxFields;
private: System::Windows::Forms::Label^  label_P3StatusFlags;
private: System::Windows::Forms::CheckBox^  checkBox_P3Cadence;
private: System::Windows::Forms::Label^  label_P3Use;
private: System::Windows::Forms::Label^  label_P3Health;
private: System::Windows::Forms::CheckBox^  checkBox_P3InstSpeed;
private: System::Windows::Forms::Label^  label_P3Battery;
private: System::Windows::Forms::Label^  label_P3Location;
private: System::Windows::Forms::ListBox^  listBox_P3Use;
private: System::Windows::Forms::ListBox^  listBox_P3Health;
private: System::Windows::Forms::ListBox^  listBox_P3Battery;
private: System::Windows::Forms::ListBox^  listBox_P3Location;
private: System::Windows::Forms::GroupBox^  groupBox_P3Calories;
private: System::Windows::Forms::Label^  label_P3CalDisplay;
private: System::Windows::Forms::Label^  label_P3CalUnits;
private: System::Windows::Forms::TextBox^  textBox_P3Calories;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Sim_BurnRate;
private: System::Windows::Forms::Label^  label_BurnRate;
private: System::Windows::Forms::TabPage^  tabPage1;
private: System::Windows::Forms::CheckBox^  checkBox_Page3;
private: System::Windows::Forms::CheckBox^  checkBox_Page2;
private: System::Windows::Forms::CheckBox^  checkBox_Page1;
private: System::Windows::Forms::Button^  button_P3UpdateCalories;
private: System::Windows::Forms::Label^  label_P16Title;
private: System::Windows::Forms::TabPage^  tabPage2;
private: System::Windows::Forms::Label^  label_P22Title;
private: System::Windows::Forms::Button^  button_P16UpdateDistance;
private: System::Windows::Forms::Label^  label_P16Distance;
private: System::Windows::Forms::Label^  label_P16Strides;
private: System::Windows::Forms::Label^  label_P16StridesDisplay;
private: System::Windows::Forms::Button^  button_P16UpdateStrides;
private: System::Windows::Forms::TextBox^  textBox_P16DistDisplayChange;
private: System::Windows::Forms::Label^  label_P16DistanceDisplay;
private: System::Windows::Forms::TextBox^  textBox_P16StridesDisplayChange;
private: System::Windows::Forms::CheckBox^  checkBox2;
private: System::Windows::Forms::CheckBox^  checkBox_P16Enabled;
private: System::Windows::Forms::CheckBox^  checkBox_P22Enabled;
private: System::Windows::Forms::GroupBox^  groupBox_P22Supported;
private: System::Windows::Forms::Label^  label_P22Time;
private: System::Windows::Forms::Label^  label_P22Calories;
private: System::Windows::Forms::Label^  label_P22Distance;
private: System::Windows::Forms::Label^  label_P22Cadence;
private: System::Windows::Forms::Label^  label_P22Speed;
private: System::Windows::Forms::Label^  label_P22Latency;
private: System::Windows::Forms::CheckBox^  checkBox_P22Ignore;
private: System::Windows::Forms::Label^  label_P16_UnitsDist;
private: System::Windows::Forms::Label^  label_P16UpdateError;
private: System::Windows::Forms::Label^  label_TransCal;
private: System::Windows::Forms::Label^  label_TransCalDisplay;
private: System::Windows::Forms::CheckBox^  checkBox_InvalidSerial;


private:
	/// <summary>
	/// Required designer variable.
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
		this->tabControl_Settings = (gcnew System::Windows::Forms::TabControl());
		this->tabPage_Parameters = (gcnew System::Windows::Forms::TabPage());
		this->numericUpDown_Sim_BurnRate = (gcnew System::Windows::Forms::NumericUpDown());
		this->label_BurnRate = (gcnew System::Windows::Forms::Label());
		this->numericUpDown_Sim_StrideLength = (gcnew System::Windows::Forms::NumericUpDown());
		this->numericUpDown_Sim_MaxOutput = (gcnew System::Windows::Forms::NumericUpDown());
		this->label_StrideConvConst = (gcnew System::Windows::Forms::Label());
		this->listBox_SimUnits = (gcnew System::Windows::Forms::ListBox());
		this->numericUpDown_Sim_CurOutput = (gcnew System::Windows::Forms::NumericUpDown());
		this->numericUpDown_Sim_MinOutput = (gcnew System::Windows::Forms::NumericUpDown());
		this->label_Sim_Max = (gcnew System::Windows::Forms::Label());
		this->label_Sim_Current = (gcnew System::Windows::Forms::Label());
		this->label_Sim_Min = (gcnew System::Windows::Forms::Label());
		this->checkBox_Sweeping = (gcnew System::Windows::Forms::CheckBox());
		this->panel_PageType = (gcnew System::Windows::Forms::Panel());
		this->checkBox_Page3 = (gcnew System::Windows::Forms::CheckBox());
		this->checkBox_Page2 = (gcnew System::Windows::Forms::CheckBox());
		this->checkBox_Page1 = (gcnew System::Windows::Forms::CheckBox());
		this->label_PageType = (gcnew System::Windows::Forms::Label());
		this->label_Sim_ParamLabel = (gcnew System::Windows::Forms::Label());
		this->tabPage_DeviceInfo = (gcnew System::Windows::Forms::TabPage());
		this->checkBox_InvalidSerial = (gcnew System::Windows::Forms::CheckBox());
		this->checkBox_EnableCommon = (gcnew System::Windows::Forms::CheckBox());
		this->label_SoftwareVer = (gcnew System::Windows::Forms::Label());
		this->label_HardwareVer = (gcnew System::Windows::Forms::Label());
		this->button_UpdateCommon = (gcnew System::Windows::Forms::Button());
		this->label_ErrorCommon = (gcnew System::Windows::Forms::Label());
		this->label_ManfID = (gcnew System::Windows::Forms::Label());
		this->label_SerialNum = (gcnew System::Windows::Forms::Label());
		this->textBox_SerialNum = (gcnew System::Windows::Forms::TextBox());
		this->label_ModelNum = (gcnew System::Windows::Forms::Label());
		this->textBox_SwVersion = (gcnew System::Windows::Forms::TextBox());
		this->textBox_HwVersion = (gcnew System::Windows::Forms::TextBox());
		this->textBox_ModelNum = (gcnew System::Windows::Forms::TextBox());
		this->textBox_MfgID = (gcnew System::Windows::Forms::TextBox());
		this->tabPage_Page1Settings = (gcnew System::Windows::Forms::TabPage());
		this->button_UpdateStrides = (gcnew System::Windows::Forms::Button());
		this->label_P1TxFields = (gcnew System::Windows::Forms::Label());
		this->checkBox_P1Latency = (gcnew System::Windows::Forms::CheckBox());
		this->checkBox_P1InstSpeed = (gcnew System::Windows::Forms::CheckBox());
		this->checkBox_P1Distance = (gcnew System::Windows::Forms::CheckBox());
		this->checkBox_P1Time = (gcnew System::Windows::Forms::CheckBox());
		this->label_P1StrideCount = (gcnew System::Windows::Forms::Label());
		this->label_P1TotalDistance = (gcnew System::Windows::Forms::Label());
		this->label_P1TotDistDisplay = (gcnew System::Windows::Forms::Label());
		this->button_UpdateDistance = (gcnew System::Windows::Forms::Button());
		this->label_P1UpdateError = (gcnew System::Windows::Forms::Label());
		this->textBox_P1StrideCountChange = (gcnew System::Windows::Forms::TextBox());
		this->label_P1StrideCountDisplay = (gcnew System::Windows::Forms::Label());
		this->textBox_P1TotDistChange = (gcnew System::Windows::Forms::TextBox());
		this->label_P1IntReminder = (gcnew System::Windows::Forms::Label());
		this->tabPage_Page2Settings = (gcnew System::Windows::Forms::TabPage());
		this->label_P2TxFields = (gcnew System::Windows::Forms::Label());
		this->label_P2StatusFlags = (gcnew System::Windows::Forms::Label());
		this->checkBox_P2Cadence = (gcnew System::Windows::Forms::CheckBox());
		this->label_P2Use = (gcnew System::Windows::Forms::Label());
		this->label_P2Health = (gcnew System::Windows::Forms::Label());
		this->checkBox_P2InstSpeed = (gcnew System::Windows::Forms::CheckBox());
		this->label_P2Battery = (gcnew System::Windows::Forms::Label());
		this->label_P2Location = (gcnew System::Windows::Forms::Label());
		this->listBox_P2Use = (gcnew System::Windows::Forms::ListBox());
		this->listBox_P2Health = (gcnew System::Windows::Forms::ListBox());
		this->listBox_P2Battery = (gcnew System::Windows::Forms::ListBox());
		this->listBox_P2Location = (gcnew System::Windows::Forms::ListBox());
		this->tabPage_Page3Settings = (gcnew System::Windows::Forms::TabPage());
		this->groupBox_P3Calories = (gcnew System::Windows::Forms::GroupBox());
		this->textBox_P3Calories = (gcnew System::Windows::Forms::TextBox());
		this->button_P3UpdateCalories = (gcnew System::Windows::Forms::Button());
		this->label_P3CalDisplay = (gcnew System::Windows::Forms::Label());
		this->label_P3CalUnits = (gcnew System::Windows::Forms::Label());
		this->label_P3TxFields = (gcnew System::Windows::Forms::Label());
		this->label_P3StatusFlags = (gcnew System::Windows::Forms::Label());
		this->checkBox_P3Cadence = (gcnew System::Windows::Forms::CheckBox());
		this->label_P3Use = (gcnew System::Windows::Forms::Label());
		this->label_P3Health = (gcnew System::Windows::Forms::Label());
		this->checkBox_P3InstSpeed = (gcnew System::Windows::Forms::CheckBox());
		this->label_P3Battery = (gcnew System::Windows::Forms::Label());
		this->label_P3Location = (gcnew System::Windows::Forms::Label());
		this->listBox_P3Use = (gcnew System::Windows::Forms::ListBox());
		this->listBox_P3Health = (gcnew System::Windows::Forms::ListBox());
		this->listBox_P3Battery = (gcnew System::Windows::Forms::ListBox());
		this->listBox_P3Location = (gcnew System::Windows::Forms::ListBox());
		this->tabPage1 = (gcnew System::Windows::Forms::TabPage());
		this->label_P16UpdateError = (gcnew System::Windows::Forms::Label());
		this->label_P16_UnitsDist = (gcnew System::Windows::Forms::Label());
		this->checkBox_P16Enabled = (gcnew System::Windows::Forms::CheckBox());
		this->button_P16UpdateDistance = (gcnew System::Windows::Forms::Button());
		this->label_P16Distance = (gcnew System::Windows::Forms::Label());
		this->label_P16Strides = (gcnew System::Windows::Forms::Label());
		this->label_P16StridesDisplay = (gcnew System::Windows::Forms::Label());
		this->button_P16UpdateStrides = (gcnew System::Windows::Forms::Button());
		this->textBox_P16DistDisplayChange = (gcnew System::Windows::Forms::TextBox());
		this->label_P16DistanceDisplay = (gcnew System::Windows::Forms::Label());
		this->textBox_P16StridesDisplayChange = (gcnew System::Windows::Forms::TextBox());
		this->label_P16Title = (gcnew System::Windows::Forms::Label());
		this->tabPage2 = (gcnew System::Windows::Forms::TabPage());
		this->checkBox_P22Ignore = (gcnew System::Windows::Forms::CheckBox());
		this->groupBox_P22Supported = (gcnew System::Windows::Forms::GroupBox());
		this->label_P22Time = (gcnew System::Windows::Forms::Label());
		this->label_P22Calories = (gcnew System::Windows::Forms::Label());
		this->label_P22Distance = (gcnew System::Windows::Forms::Label());
		this->label_P22Cadence = (gcnew System::Windows::Forms::Label());
		this->label_P22Speed = (gcnew System::Windows::Forms::Label());
		this->label_P22Latency = (gcnew System::Windows::Forms::Label());
		this->checkBox_P22Enabled = (gcnew System::Windows::Forms::CheckBox());
		this->label_P22Title = (gcnew System::Windows::Forms::Label());
		this->panel_Display = (gcnew System::Windows::Forms::Panel());
		this->label_TransCal = (gcnew System::Windows::Forms::Label());
		this->label_TransCalDisplay = (gcnew System::Windows::Forms::Label());
		this->label_TransDist = (gcnew System::Windows::Forms::Label());
		this->label_TransInstSpeed = (gcnew System::Windows::Forms::Label());
		this->label_TransCadence = (gcnew System::Windows::Forms::Label());
		this->label_Trn_Time = (gcnew System::Windows::Forms::Label());
		this->label_TransDistDisplay = (gcnew System::Windows::Forms::Label());
		this->label_TransInstSpeedDisplay = (gcnew System::Windows::Forms::Label());
		this->label_TransCadenceDisplay = (gcnew System::Windows::Forms::Label());
		this->label_TransDisplayTitle = (gcnew System::Windows::Forms::Label());
		this->label_Trn_TimeDisplay = (gcnew System::Windows::Forms::Label());
		this->label_TransLatency = (gcnew System::Windows::Forms::Label());
		this->label_TransLatencyDisplay = (gcnew System::Windows::Forms::Label());
		this->label_TransStrideCountDisplay = (gcnew System::Windows::Forms::Label());
		this->label_TransStrideCount = (gcnew System::Windows::Forms::Label());
		this->checkBox2 = (gcnew System::Windows::Forms::CheckBox());
		this->panel_Settings->SuspendLayout();
		this->tabControl_Settings->SuspendLayout();
		this->tabPage_Parameters->SuspendLayout();
		(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_BurnRate))->BeginInit();
		(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_StrideLength))->BeginInit();
		(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_MaxOutput))->BeginInit();
		(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_CurOutput))->BeginInit();
		(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_MinOutput))->BeginInit();
		this->panel_PageType->SuspendLayout();
		this->tabPage_DeviceInfo->SuspendLayout();
		this->tabPage_Page1Settings->SuspendLayout();
		this->tabPage_Page2Settings->SuspendLayout();
		this->tabPage_Page3Settings->SuspendLayout();
		this->groupBox_P3Calories->SuspendLayout();
		this->tabPage1->SuspendLayout();
		this->tabPage2->SuspendLayout();
		this->groupBox_P22Supported->SuspendLayout();
		this->panel_Display->SuspendLayout();
		this->SuspendLayout();
		// 
		// panel_Settings
		// 
		this->panel_Settings->Controls->Add(this->tabControl_Settings);
		this->panel_Settings->Location = System::Drawing::Point(322, 50);
		this->panel_Settings->Name = L"panel_Settings";
		this->panel_Settings->Size = System::Drawing::Size(400, 140);
		this->panel_Settings->TabIndex = 0;
		// 
		// tabControl_Settings
		// 
		this->tabControl_Settings->Controls->Add(this->tabPage_Parameters);
		this->tabControl_Settings->Controls->Add(this->tabPage_DeviceInfo);
		this->tabControl_Settings->Controls->Add(this->tabPage_Page1Settings);
		this->tabControl_Settings->Controls->Add(this->tabPage_Page2Settings);
		this->tabControl_Settings->Controls->Add(this->tabPage_Page3Settings);
		this->tabControl_Settings->Controls->Add(this->tabPage1);
		this->tabControl_Settings->Controls->Add(this->tabPage2);
		this->tabControl_Settings->Location = System::Drawing::Point(0, 3);
		this->tabControl_Settings->Name = L"tabControl_Settings";
		this->tabControl_Settings->SelectedIndex = 0;
		this->tabControl_Settings->Size = System::Drawing::Size(397, 137);
		this->tabControl_Settings->TabIndex = 10;
		// 
		// tabPage_Parameters
		// 
		this->tabPage_Parameters->Controls->Add(this->numericUpDown_Sim_BurnRate);
		this->tabPage_Parameters->Controls->Add(this->label_BurnRate);
		this->tabPage_Parameters->Controls->Add(this->numericUpDown_Sim_StrideLength);
		this->tabPage_Parameters->Controls->Add(this->numericUpDown_Sim_MaxOutput);
		this->tabPage_Parameters->Controls->Add(this->label_StrideConvConst);
		this->tabPage_Parameters->Controls->Add(this->listBox_SimUnits);
		this->tabPage_Parameters->Controls->Add(this->numericUpDown_Sim_CurOutput);
		this->tabPage_Parameters->Controls->Add(this->numericUpDown_Sim_MinOutput);
		this->tabPage_Parameters->Controls->Add(this->label_Sim_Max);
		this->tabPage_Parameters->Controls->Add(this->label_Sim_Current);
		this->tabPage_Parameters->Controls->Add(this->label_Sim_Min);
		this->tabPage_Parameters->Controls->Add(this->checkBox_Sweeping);
		this->tabPage_Parameters->Controls->Add(this->panel_PageType);
		this->tabPage_Parameters->Controls->Add(this->label_Sim_ParamLabel);
		this->tabPage_Parameters->Location = System::Drawing::Point(4, 22);
		this->tabPage_Parameters->Name = L"tabPage_Parameters";
		this->tabPage_Parameters->Padding = System::Windows::Forms::Padding(3);
		this->tabPage_Parameters->Size = System::Drawing::Size(389, 111);
		this->tabPage_Parameters->TabIndex = 0;
		this->tabPage_Parameters->Text = L"Parameters";
		this->tabPage_Parameters->UseVisualStyleBackColor = true;
		// 
		// numericUpDown_Sim_BurnRate
		// 
		this->numericUpDown_Sim_BurnRate->Location = System::Drawing::Point(223, 76);
		this->numericUpDown_Sim_BurnRate->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {200, 0, 0, 0});
		this->numericUpDown_Sim_BurnRate->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
		this->numericUpDown_Sim_BurnRate->Name = L"numericUpDown_Sim_BurnRate";
		this->numericUpDown_Sim_BurnRate->Size = System::Drawing::Size(47, 20);
		this->numericUpDown_Sim_BurnRate->TabIndex = 28;
		this->numericUpDown_Sim_BurnRate->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {62, 0, 0, 0});
		this->numericUpDown_Sim_BurnRate->ValueChanged += gcnew System::EventHandler(this, &SDMSensor::numericUpDown_Sim_BurnRate_ValueChanged);
		// 
		// label_BurnRate
		// 
		this->label_BurnRate->AutoSize = true;
		this->label_BurnRate->Location = System::Drawing::Point(196, 60);
		this->label_BurnRate->Name = L"label_BurnRate";
		this->label_BurnRate->Size = System::Drawing::Size(103, 13);
		this->label_BurnRate->TabIndex = 27;
		this->label_BurnRate->Text = L"Burn Rate (kcal/km)";
		// 
		// numericUpDown_Sim_StrideLength
		// 
		this->numericUpDown_Sim_StrideLength->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {5, 0, 0, 0});
		this->numericUpDown_Sim_StrideLength->Location = System::Drawing::Point(223, 31);
		this->numericUpDown_Sim_StrideLength->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
		this->numericUpDown_Sim_StrideLength->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
		this->numericUpDown_Sim_StrideLength->Name = L"numericUpDown_Sim_StrideLength";
		this->numericUpDown_Sim_StrideLength->Size = System::Drawing::Size(47, 20);
		this->numericUpDown_Sim_StrideLength->TabIndex = 13;
		this->numericUpDown_Sim_StrideLength->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {75, 0, 0, 0});
		this->numericUpDown_Sim_StrideLength->ValueChanged += gcnew System::EventHandler(this, &SDMSensor::numericUpDown_Sim_StrideLength_ValueChanged);
		// 
		// numericUpDown_Sim_MaxOutput
		// 
		this->numericUpDown_Sim_MaxOutput->DecimalPlaces = 3;
		this->numericUpDown_Sim_MaxOutput->Enabled = false;
		this->numericUpDown_Sim_MaxOutput->Location = System::Drawing::Point(50, 66);
		this->numericUpDown_Sim_MaxOutput->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255940, 0, 0, 196608});
		this->numericUpDown_Sim_MaxOutput->Name = L"numericUpDown_Sim_MaxOutput";
		this->numericUpDown_Sim_MaxOutput->Size = System::Drawing::Size(64, 20);
		this->numericUpDown_Sim_MaxOutput->TabIndex = 24;
		this->numericUpDown_Sim_MaxOutput->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
		this->numericUpDown_Sim_MaxOutput->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {120, 0, 0, 0});
		this->numericUpDown_Sim_MaxOutput->ValueChanged += gcnew System::EventHandler(this, &SDMSensor::numericUpDown_Sim_MinMaxOutput_ValueChanged);
		// 
		// label_StrideConvConst
		// 
		this->label_StrideConvConst->AutoSize = true;
		this->label_StrideConvConst->Location = System::Drawing::Point(196, 12);
		this->label_StrideConvConst->Name = L"label_StrideConvConst";
		this->label_StrideConvConst->Size = System::Drawing::Size(93, 13);
		this->label_StrideConvConst->TabIndex = 18;
		this->label_StrideConvConst->Text = L"Stride Length (cm)";
		this->label_StrideConvConst->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
		// 
		// listBox_SimUnits
		// 
		this->listBox_SimUnits->FormattingEnabled = true;
		this->listBox_SimUnits->Items->AddRange(gcnew cli::array< System::Object^  >(4) {L"m/s", L"km/h", L"mile/h", L"stride/min"});
		this->listBox_SimUnits->Location = System::Drawing::Point(125, 27);
		this->listBox_SimUnits->Name = L"listBox_SimUnits";
		this->listBox_SimUnits->Size = System::Drawing::Size(65, 56);
		this->listBox_SimUnits->TabIndex = 26;
		// 
		// numericUpDown_Sim_CurOutput
		// 
		this->numericUpDown_Sim_CurOutput->DecimalPlaces = 3;
		this->numericUpDown_Sim_CurOutput->Location = System::Drawing::Point(50, 45);
		this->numericUpDown_Sim_CurOutput->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255940, 0, 0, 196608});
		this->numericUpDown_Sim_CurOutput->Name = L"numericUpDown_Sim_CurOutput";
		this->numericUpDown_Sim_CurOutput->Size = System::Drawing::Size(64, 20);
		this->numericUpDown_Sim_CurOutput->TabIndex = 23;
		this->numericUpDown_Sim_CurOutput->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
		this->numericUpDown_Sim_CurOutput->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {90, 0, 0, 0});
		this->numericUpDown_Sim_CurOutput->ValueChanged += gcnew System::EventHandler(this, &SDMSensor::numericUpDown_Sim_CurOutput_ValueChanged);
		// 
		// numericUpDown_Sim_MinOutput
		// 
		this->numericUpDown_Sim_MinOutput->DecimalPlaces = 3;
		this->numericUpDown_Sim_MinOutput->Enabled = false;
		this->numericUpDown_Sim_MinOutput->Location = System::Drawing::Point(50, 24);
		this->numericUpDown_Sim_MinOutput->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255940, 0, 0, 196608});
		this->numericUpDown_Sim_MinOutput->Name = L"numericUpDown_Sim_MinOutput";
		this->numericUpDown_Sim_MinOutput->Size = System::Drawing::Size(64, 20);
		this->numericUpDown_Sim_MinOutput->TabIndex = 22;
		this->numericUpDown_Sim_MinOutput->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
		this->numericUpDown_Sim_MinOutput->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {60, 0, 0, 0});
		this->numericUpDown_Sim_MinOutput->ValueChanged += gcnew System::EventHandler(this, &SDMSensor::numericUpDown_Sim_MinMaxOutput_ValueChanged);
		// 
		// label_Sim_Max
		// 
		this->label_Sim_Max->AutoSize = true;
		this->label_Sim_Max->Location = System::Drawing::Point(19, 68);
		this->label_Sim_Max->Name = L"label_Sim_Max";
		this->label_Sim_Max->Size = System::Drawing::Size(27, 13);
		this->label_Sim_Max->TabIndex = 21;
		this->label_Sim_Max->Text = L"Max";
		// 
		// label_Sim_Current
		// 
		this->label_Sim_Current->AutoSize = true;
		this->label_Sim_Current->Location = System::Drawing::Point(5, 47);
		this->label_Sim_Current->Name = L"label_Sim_Current";
		this->label_Sim_Current->Size = System::Drawing::Size(41, 13);
		this->label_Sim_Current->TabIndex = 20;
		this->label_Sim_Current->Text = L"Current";
		// 
		// label_Sim_Min
		// 
		this->label_Sim_Min->AutoSize = true;
		this->label_Sim_Min->Location = System::Drawing::Point(22, 26);
		this->label_Sim_Min->Name = L"label_Sim_Min";
		this->label_Sim_Min->Size = System::Drawing::Size(24, 13);
		this->label_Sim_Min->TabIndex = 19;
		this->label_Sim_Min->Text = L"Min";
		// 
		// checkBox_Sweeping
		// 
		this->checkBox_Sweeping->AutoSize = true;
		this->checkBox_Sweeping->Location = System::Drawing::Point(22, 89);
		this->checkBox_Sweeping->Name = L"checkBox_Sweeping";
		this->checkBox_Sweeping->Size = System::Drawing::Size(73, 17);
		this->checkBox_Sweeping->TabIndex = 18;
		this->checkBox_Sweeping->Text = L"Sweeping";
		this->checkBox_Sweeping->UseVisualStyleBackColor = true;
		this->checkBox_Sweeping->CheckedChanged += gcnew System::EventHandler(this, &SDMSensor::checkBox_Sweeping_CheckedChanged);
		// 
		// panel_PageType
		// 
		this->panel_PageType->Controls->Add(this->checkBox_Page3);
		this->panel_PageType->Controls->Add(this->checkBox_Page2);
		this->panel_PageType->Controls->Add(this->checkBox_Page1);
		this->panel_PageType->Controls->Add(this->label_PageType);
		this->panel_PageType->Location = System::Drawing::Point(300, 0);
		this->panel_PageType->Name = L"panel_PageType";
		this->panel_PageType->Size = System::Drawing::Size(89, 111);
		this->panel_PageType->TabIndex = 2;
		// 
		// checkBox_Page3
		// 
		this->checkBox_Page3->AutoSize = true;
		this->checkBox_Page3->Location = System::Drawing::Point(11, 70);
		this->checkBox_Page3->Name = L"checkBox_Page3";
		this->checkBox_Page3->Size = System::Drawing::Size(60, 17);
		this->checkBox_Page3->TabIndex = 24;
		this->checkBox_Page3->Text = L"Page 3";
		this->checkBox_Page3->UseVisualStyleBackColor = true;
		this->checkBox_Page3->CheckedChanged += gcnew System::EventHandler(this, &SDMSensor::checkBox_Page3_CheckedChanged);
		// 
		// checkBox_Page2
		// 
		this->checkBox_Page2->AutoSize = true;
		this->checkBox_Page2->Checked = true;
		this->checkBox_Page2->CheckState = System::Windows::Forms::CheckState::Checked;
		this->checkBox_Page2->Location = System::Drawing::Point(11, 51);
		this->checkBox_Page2->Name = L"checkBox_Page2";
		this->checkBox_Page2->Size = System::Drawing::Size(60, 17);
		this->checkBox_Page2->TabIndex = 23;
		this->checkBox_Page2->Text = L"Page 2";
		this->checkBox_Page2->UseVisualStyleBackColor = true;
		this->checkBox_Page2->CheckedChanged += gcnew System::EventHandler(this, &SDMSensor::checkBox_Page2_CheckedChanged);
		// 
		// checkBox_Page1
		// 
		this->checkBox_Page1->AutoSize = true;
		this->checkBox_Page1->Checked = true;
		this->checkBox_Page1->CheckState = System::Windows::Forms::CheckState::Checked;
		this->checkBox_Page1->Enabled = false;
		this->checkBox_Page1->Location = System::Drawing::Point(11, 32);
		this->checkBox_Page1->Name = L"checkBox_Page1";
		this->checkBox_Page1->Size = System::Drawing::Size(60, 17);
		this->checkBox_Page1->TabIndex = 22;
		this->checkBox_Page1->Text = L"Page 1";
		this->checkBox_Page1->UseVisualStyleBackColor = true;
		// 
		// label_PageType
		// 
		this->label_PageType->AutoSize = true;
		this->label_PageType->Location = System::Drawing::Point(8, 12);
		this->label_PageType->Name = L"label_PageType";
		this->label_PageType->Size = System::Drawing::Size(72, 13);
		this->label_PageType->TabIndex = 12;
		this->label_PageType->Text = L"Broadcasting:";
		this->label_PageType->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
		// 
		// label_Sim_ParamLabel
		// 
		this->label_Sim_ParamLabel->AutoSize = true;
		this->label_Sim_ParamLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Underline, 
			System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
		this->label_Sim_ParamLabel->Location = System::Drawing::Point(26, 4);
		this->label_Sim_ParamLabel->Name = L"label_Sim_ParamLabel";
		this->label_Sim_ParamLabel->Size = System::Drawing::Size(114, 13);
		this->label_Sim_ParamLabel->TabIndex = 0;
		this->label_Sim_ParamLabel->Text = L"Simulation Parameters:";
		// 
		// tabPage_DeviceInfo
		// 
		this->tabPage_DeviceInfo->Controls->Add(this->checkBox_InvalidSerial);
		this->tabPage_DeviceInfo->Controls->Add(this->checkBox_EnableCommon);
		this->tabPage_DeviceInfo->Controls->Add(this->label_SoftwareVer);
		this->tabPage_DeviceInfo->Controls->Add(this->label_HardwareVer);
		this->tabPage_DeviceInfo->Controls->Add(this->button_UpdateCommon);
		this->tabPage_DeviceInfo->Controls->Add(this->label_ErrorCommon);
		this->tabPage_DeviceInfo->Controls->Add(this->label_ManfID);
		this->tabPage_DeviceInfo->Controls->Add(this->label_SerialNum);
		this->tabPage_DeviceInfo->Controls->Add(this->textBox_SerialNum);
		this->tabPage_DeviceInfo->Controls->Add(this->label_ModelNum);
		this->tabPage_DeviceInfo->Controls->Add(this->textBox_SwVersion);
		this->tabPage_DeviceInfo->Controls->Add(this->textBox_HwVersion);
		this->tabPage_DeviceInfo->Controls->Add(this->textBox_ModelNum);
		this->tabPage_DeviceInfo->Controls->Add(this->textBox_MfgID);
		this->tabPage_DeviceInfo->Location = System::Drawing::Point(4, 22);
		this->tabPage_DeviceInfo->Name = L"tabPage_DeviceInfo";
		this->tabPage_DeviceInfo->Padding = System::Windows::Forms::Padding(3);
		this->tabPage_DeviceInfo->Size = System::Drawing::Size(389, 111);
		this->tabPage_DeviceInfo->TabIndex = 1;
		this->tabPage_DeviceInfo->Text = L"Device Info";
		this->tabPage_DeviceInfo->UseVisualStyleBackColor = true;
		// 
		// checkBox_InvalidSerial
		// 
		this->checkBox_InvalidSerial->AutoSize = true;
		this->checkBox_InvalidSerial->Location = System::Drawing::Point(154, 14);
		this->checkBox_InvalidSerial->Name = L"checkBox_InvalidSerial";
		this->checkBox_InvalidSerial->Size = System::Drawing::Size(79, 17);
		this->checkBox_InvalidSerial->TabIndex = 29;
		this->checkBox_InvalidSerial->Text = L"No Serial #";
		this->checkBox_InvalidSerial->UseVisualStyleBackColor = true;
		this->checkBox_InvalidSerial->CheckedChanged += gcnew System::EventHandler(this, &SDMSensor::checkBox_InvalidSerial_CheckedChanged);
		// 
		// checkBox_EnableCommon
		// 
		this->checkBox_EnableCommon->AutoSize = true;
		this->checkBox_EnableCommon->Checked = true;
		this->checkBox_EnableCommon->CheckState = System::Windows::Forms::CheckState::Checked;
		this->checkBox_EnableCommon->Location = System::Drawing::Point(20, 86);
		this->checkBox_EnableCommon->Name = L"checkBox_EnableCommon";
		this->checkBox_EnableCommon->Size = System::Drawing::Size(162, 17);
		this->checkBox_EnableCommon->TabIndex = 28;
		this->checkBox_EnableCommon->Text = L"Enable Common Data Pages";
		this->checkBox_EnableCommon->UseVisualStyleBackColor = true;
		this->checkBox_EnableCommon->CheckedChanged += gcnew System::EventHandler(this, &SDMSensor::checkBox_EnableCommon_CheckedChanged);
		// 
		// label_SoftwareVer
		// 
		this->label_SoftwareVer->AutoSize = true;
		this->label_SoftwareVer->Location = System::Drawing::Point(245, 36);
		this->label_SoftwareVer->Name = L"label_SoftwareVer";
		this->label_SoftwareVer->Size = System::Drawing::Size(71, 13);
		this->label_SoftwareVer->TabIndex = 4;
		this->label_SoftwareVer->Text = L"Software Ver:";
		// 
		// label_HardwareVer
		// 
		this->label_HardwareVer->AutoSize = true;
		this->label_HardwareVer->Location = System::Drawing::Point(241, 15);
		this->label_HardwareVer->Name = L"label_HardwareVer";
		this->label_HardwareVer->Size = System::Drawing::Size(75, 13);
		this->label_HardwareVer->TabIndex = 3;
		this->label_HardwareVer->Text = L"Hardware Ver:";
		// 
		// button_UpdateCommon
		// 
		this->button_UpdateCommon->Location = System::Drawing::Point(254, 60);
		this->button_UpdateCommon->Name = L"button_UpdateCommon";
		this->button_UpdateCommon->Size = System::Drawing::Size(97, 20);
		this->button_UpdateCommon->TabIndex = 27;
		this->button_UpdateCommon->Text = L"Update All";
		this->button_UpdateCommon->UseVisualStyleBackColor = true;
		this->button_UpdateCommon->Click += gcnew System::EventHandler(this, &SDMSensor::button_UpdateCommon_Click);
		// 
		// label_ErrorCommon
		// 
		this->label_ErrorCommon->AutoSize = true;
		this->label_ErrorCommon->Location = System::Drawing::Point(241, 86);
		this->label_ErrorCommon->Name = L"label_ErrorCommon";
		this->label_ErrorCommon->Size = System::Drawing::Size(32, 13);
		this->label_ErrorCommon->TabIndex = 20;
		this->label_ErrorCommon->Text = L"Error:";
		this->label_ErrorCommon->Visible = false;
		// 
		// label_ManfID
		// 
		this->label_ManfID->AutoSize = true;
		this->label_ManfID->Location = System::Drawing::Point(15, 39);
		this->label_ManfID->Name = L"label_ManfID";
		this->label_ManfID->Size = System::Drawing::Size(51, 13);
		this->label_ManfID->TabIndex = 1;
		this->label_ManfID->Text = L"Manf. ID:";
		// 
		// label_SerialNum
		// 
		this->label_SerialNum->AutoSize = true;
		this->label_SerialNum->Location = System::Drawing::Point(20, 15);
		this->label_SerialNum->Name = L"label_SerialNum";
		this->label_SerialNum->Size = System::Drawing::Size(46, 13);
		this->label_SerialNum->TabIndex = 2;
		this->label_SerialNum->Text = L"Serial #:";
		// 
		// textBox_SerialNum
		// 
		this->textBox_SerialNum->Location = System::Drawing::Point(72, 12);
		this->textBox_SerialNum->MaxLength = 10;
		this->textBox_SerialNum->Name = L"textBox_SerialNum";
		this->textBox_SerialNum->Size = System::Drawing::Size(76, 20);
		this->textBox_SerialNum->TabIndex = 24;
		this->textBox_SerialNum->Text = L"1234567890";
		// 
		// label_ModelNum
		// 
		this->label_ModelNum->AutoSize = true;
		this->label_ModelNum->Location = System::Drawing::Point(17, 63);
		this->label_ModelNum->Name = L"label_ModelNum";
		this->label_ModelNum->Size = System::Drawing::Size(49, 13);
		this->label_ModelNum->TabIndex = 5;
		this->label_ModelNum->Text = L"Model #:";
		// 
		// textBox_SwVersion
		// 
		this->textBox_SwVersion->Location = System::Drawing::Point(322, 36);
		this->textBox_SwVersion->MaxLength = 3;
		this->textBox_SwVersion->Name = L"textBox_SwVersion";
		this->textBox_SwVersion->Size = System::Drawing::Size(29, 20);
		this->textBox_SwVersion->TabIndex = 26;
		this->textBox_SwVersion->Text = L"1";
		// 
		// textBox_HwVersion
		// 
		this->textBox_HwVersion->Location = System::Drawing::Point(322, 12);
		this->textBox_HwVersion->MaxLength = 3;
		this->textBox_HwVersion->Name = L"textBox_HwVersion";
		this->textBox_HwVersion->Size = System::Drawing::Size(29, 20);
		this->textBox_HwVersion->TabIndex = 25;
		this->textBox_HwVersion->Text = L"1";
		// 
		// textBox_ModelNum
		// 
		this->textBox_ModelNum->Location = System::Drawing::Point(99, 60);
		this->textBox_ModelNum->MaxLength = 5;
		this->textBox_ModelNum->Name = L"textBox_ModelNum";
		this->textBox_ModelNum->Size = System::Drawing::Size(49, 20);
		this->textBox_ModelNum->TabIndex = 23;
		this->textBox_ModelNum->Text = L"33669";
		// 
		// textBox_MfgID
		// 
		this->textBox_MfgID->Location = System::Drawing::Point(99, 36);
		this->textBox_MfgID->MaxLength = 5;
		this->textBox_MfgID->Name = L"textBox_MfgID";
		this->textBox_MfgID->Size = System::Drawing::Size(49, 20);
		this->textBox_MfgID->TabIndex = 22;
		this->textBox_MfgID->Text = L"54321";
		// 
		// tabPage_Page1Settings
		// 
		this->tabPage_Page1Settings->Controls->Add(this->button_UpdateStrides);
		this->tabPage_Page1Settings->Controls->Add(this->label_P1TxFields);
		this->tabPage_Page1Settings->Controls->Add(this->checkBox_P1Latency);
		this->tabPage_Page1Settings->Controls->Add(this->checkBox_P1InstSpeed);
		this->tabPage_Page1Settings->Controls->Add(this->checkBox_P1Distance);
		this->tabPage_Page1Settings->Controls->Add(this->checkBox_P1Time);
		this->tabPage_Page1Settings->Controls->Add(this->label_P1StrideCount);
		this->tabPage_Page1Settings->Controls->Add(this->label_P1TotalDistance);
		this->tabPage_Page1Settings->Controls->Add(this->label_P1TotDistDisplay);
		this->tabPage_Page1Settings->Controls->Add(this->button_UpdateDistance);
		this->tabPage_Page1Settings->Controls->Add(this->label_P1UpdateError);
		this->tabPage_Page1Settings->Controls->Add(this->textBox_P1StrideCountChange);
		this->tabPage_Page1Settings->Controls->Add(this->label_P1StrideCountDisplay);
		this->tabPage_Page1Settings->Controls->Add(this->textBox_P1TotDistChange);
		this->tabPage_Page1Settings->Controls->Add(this->label_P1IntReminder);
		this->tabPage_Page1Settings->Location = System::Drawing::Point(4, 22);
		this->tabPage_Page1Settings->Name = L"tabPage_Page1Settings";
		this->tabPage_Page1Settings->Size = System::Drawing::Size(389, 111);
		this->tabPage_Page1Settings->TabIndex = 2;
		this->tabPage_Page1Settings->Text = L"Page 1";
		this->tabPage_Page1Settings->UseVisualStyleBackColor = true;
		// 
		// button_UpdateStrides
		// 
		this->button_UpdateStrides->Location = System::Drawing::Point(195, 41);
		this->button_UpdateStrides->Name = L"button_UpdateStrides";
		this->button_UpdateStrides->Size = System::Drawing::Size(97, 20);
		this->button_UpdateStrides->TabIndex = 35;
		this->button_UpdateStrides->Text = L"Update Count";
		this->button_UpdateStrides->UseVisualStyleBackColor = true;
		this->button_UpdateStrides->Click += gcnew System::EventHandler(this, &SDMSensor::button_UpdateStrides_Click);
		// 
		// label_P1TxFields
		// 
		this->label_P1TxFields->AutoSize = true;
		this->label_P1TxFields->Location = System::Drawing::Point(8, 74);
		this->label_P1TxFields->Name = L"label_P1TxFields";
		this->label_P1TxFields->Size = System::Drawing::Size(88, 13);
		this->label_P1TxFields->TabIndex = 34;
		this->label_P1TxFields->Text = L"Broadcast Fields:";
		// 
		// checkBox_P1Latency
		// 
		this->checkBox_P1Latency->AutoSize = true;
		this->checkBox_P1Latency->Checked = true;
		this->checkBox_P1Latency->CheckState = System::Windows::Forms::CheckState::Checked;
		this->checkBox_P1Latency->Location = System::Drawing::Point(173, 73);
		this->checkBox_P1Latency->Name = L"checkBox_P1Latency";
		this->checkBox_P1Latency->Size = System::Drawing::Size(64, 17);
		this->checkBox_P1Latency->TabIndex = 33;
		this->checkBox_P1Latency->Text = L"Latency";
		this->checkBox_P1Latency->UseVisualStyleBackColor = true;
		this->checkBox_P1Latency->CheckedChanged += gcnew System::EventHandler(this, &SDMSensor::checkBox_P1Latency_CheckedChanged);
		// 
		// checkBox_P1InstSpeed
		// 
		this->checkBox_P1InstSpeed->AutoSize = true;
		this->checkBox_P1InstSpeed->Checked = true;
		this->checkBox_P1InstSpeed->CheckState = System::Windows::Forms::CheckState::Checked;
		this->checkBox_P1InstSpeed->Location = System::Drawing::Point(173, 91);
		this->checkBox_P1InstSpeed->Name = L"checkBox_P1InstSpeed";
		this->checkBox_P1InstSpeed->Size = System::Drawing::Size(127, 17);
		this->checkBox_P1InstSpeed->TabIndex = 34;
		this->checkBox_P1InstSpeed->Text = L"Instantaneous Speed";
		this->checkBox_P1InstSpeed->UseVisualStyleBackColor = true;
		this->checkBox_P1InstSpeed->CheckedChanged += gcnew System::EventHandler(this, &SDMSensor::checkBox_P1InstSpeed_CheckedChanged);
		// 
		// checkBox_P1Distance
		// 
		this->checkBox_P1Distance->AutoSize = true;
		this->checkBox_P1Distance->Checked = true;
		this->checkBox_P1Distance->CheckState = System::Windows::Forms::CheckState::Checked;
		this->checkBox_P1Distance->Location = System::Drawing::Point(102, 91);
		this->checkBox_P1Distance->Name = L"checkBox_P1Distance";
		this->checkBox_P1Distance->Size = System::Drawing::Size(68, 17);
		this->checkBox_P1Distance->TabIndex = 32;
		this->checkBox_P1Distance->Text = L"Distance";
		this->checkBox_P1Distance->UseVisualStyleBackColor = true;
		this->checkBox_P1Distance->CheckedChanged += gcnew System::EventHandler(this, &SDMSensor::checkBox_P1Distance_CheckedChanged);
		// 
		// checkBox_P1Time
		// 
		this->checkBox_P1Time->AutoSize = true;
		this->checkBox_P1Time->Checked = true;
		this->checkBox_P1Time->CheckState = System::Windows::Forms::CheckState::Checked;
		this->checkBox_P1Time->Location = System::Drawing::Point(102, 73);
		this->checkBox_P1Time->Name = L"checkBox_P1Time";
		this->checkBox_P1Time->Size = System::Drawing::Size(49, 17);
		this->checkBox_P1Time->TabIndex = 31;
		this->checkBox_P1Time->Text = L"Time";
		this->checkBox_P1Time->UseVisualStyleBackColor = true;
		this->checkBox_P1Time->CheckedChanged += gcnew System::EventHandler(this, &SDMSensor::checkBox_P1Time_CheckedChanged);
		// 
		// label_P1StrideCount
		// 
		this->label_P1StrideCount->AutoSize = true;
		this->label_P1StrideCount->Location = System::Drawing::Point(31, 45);
		this->label_P1StrideCount->Name = L"label_P1StrideCount";
		this->label_P1StrideCount->Size = System::Drawing::Size(68, 13);
		this->label_P1StrideCount->TabIndex = 22;
		this->label_P1StrideCount->Text = L"Stride Count:";
		// 
		// label_P1TotalDistance
		// 
		this->label_P1TotalDistance->AutoSize = true;
		this->label_P1TotalDistance->Location = System::Drawing::Point(3, 23);
		this->label_P1TotalDistance->Name = L"label_P1TotalDistance";
		this->label_P1TotalDistance->Size = System::Drawing::Size(96, 13);
		this->label_P1TotalDistance->TabIndex = 21;
		this->label_P1TotalDistance->Text = L"Total Distance (m):";
		// 
		// label_P1TotDistDisplay
		// 
		this->label_P1TotDistDisplay->Location = System::Drawing::Point(101, 23);
		this->label_P1TotDistDisplay->Name = L"label_P1TotDistDisplay";
		this->label_P1TotDistDisplay->Size = System::Drawing::Size(57, 13);
		this->label_P1TotDistDisplay->TabIndex = 24;
		this->label_P1TotDistDisplay->Text = L"0";
		// 
		// button_UpdateDistance
		// 
		this->button_UpdateDistance->Location = System::Drawing::Point(195, 19);
		this->button_UpdateDistance->Name = L"button_UpdateDistance";
		this->button_UpdateDistance->Size = System::Drawing::Size(97, 20);
		this->button_UpdateDistance->TabIndex = 30;
		this->button_UpdateDistance->Text = L"Update Distance";
		this->button_UpdateDistance->UseVisualStyleBackColor = true;
		this->button_UpdateDistance->Click += gcnew System::EventHandler(this, &SDMSensor::button_UpdateDistance_Click);
		// 
		// label_P1UpdateError
		// 
		this->label_P1UpdateError->AutoSize = true;
		this->label_P1UpdateError->Location = System::Drawing::Point(298, 26);
		this->label_P1UpdateError->Name = L"label_P1UpdateError";
		this->label_P1UpdateError->Size = System::Drawing::Size(32, 13);
		this->label_P1UpdateError->TabIndex = 28;
		this->label_P1UpdateError->Text = L"Error:";
		this->label_P1UpdateError->Visible = false;
		// 
		// textBox_P1StrideCountChange
		// 
		this->textBox_P1StrideCountChange->Location = System::Drawing::Point(160, 42);
		this->textBox_P1StrideCountChange->MaxLength = 3;
		this->textBox_P1StrideCountChange->Name = L"textBox_P1StrideCountChange";
		this->textBox_P1StrideCountChange->Size = System::Drawing::Size(29, 20);
		this->textBox_P1StrideCountChange->TabIndex = 29;
		this->textBox_P1StrideCountChange->Text = L"0";
		// 
		// label_P1StrideCountDisplay
		// 
		this->label_P1StrideCountDisplay->Location = System::Drawing::Point(101, 45);
		this->label_P1StrideCountDisplay->Name = L"label_P1StrideCountDisplay";
		this->label_P1StrideCountDisplay->Size = System::Drawing::Size(54, 13);
		this->label_P1StrideCountDisplay->TabIndex = 26;
		this->label_P1StrideCountDisplay->Text = L"0";
		// 
		// textBox_P1TotDistChange
		// 
		this->textBox_P1TotDistChange->Location = System::Drawing::Point(160, 19);
		this->textBox_P1TotDistChange->MaxLength = 3;
		this->textBox_P1TotDistChange->Name = L"textBox_P1TotDistChange";
		this->textBox_P1TotDistChange->Size = System::Drawing::Size(29, 20);
		this->textBox_P1TotDistChange->TabIndex = 28;
		this->textBox_P1TotDistChange->Text = L"0";
		// 
		// label_P1IntReminder
		// 
		this->label_P1IntReminder->AutoSize = true;
		this->label_P1IntReminder->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Italic, 
			System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
		this->label_P1IntReminder->Location = System::Drawing::Point(139, 3);
		this->label_P1IntReminder->Name = L"label_P1IntReminder";
		this->label_P1IntReminder->Size = System::Drawing::Size(98, 13);
		this->label_P1IntReminder->TabIndex = 29;
		this->label_P1IntReminder->Text = L"Note: Integers Only";
		// 
		// tabPage_Page2Settings
		// 
		this->tabPage_Page2Settings->Controls->Add(this->label_P2TxFields);
		this->tabPage_Page2Settings->Controls->Add(this->label_P2StatusFlags);
		this->tabPage_Page2Settings->Controls->Add(this->checkBox_P2Cadence);
		this->tabPage_Page2Settings->Controls->Add(this->label_P2Use);
		this->tabPage_Page2Settings->Controls->Add(this->label_P2Health);
		this->tabPage_Page2Settings->Controls->Add(this->checkBox_P2InstSpeed);
		this->tabPage_Page2Settings->Controls->Add(this->label_P2Battery);
		this->tabPage_Page2Settings->Controls->Add(this->label_P2Location);
		this->tabPage_Page2Settings->Controls->Add(this->listBox_P2Use);
		this->tabPage_Page2Settings->Controls->Add(this->listBox_P2Health);
		this->tabPage_Page2Settings->Controls->Add(this->listBox_P2Battery);
		this->tabPage_Page2Settings->Controls->Add(this->listBox_P2Location);
		this->tabPage_Page2Settings->Location = System::Drawing::Point(4, 22);
		this->tabPage_Page2Settings->Name = L"tabPage_Page2Settings";
		this->tabPage_Page2Settings->Size = System::Drawing::Size(389, 111);
		this->tabPage_Page2Settings->TabIndex = 3;
		this->tabPage_Page2Settings->Text = L"Page 2";
		this->tabPage_Page2Settings->UseVisualStyleBackColor = true;
		// 
		// label_P2TxFields
		// 
		this->label_P2TxFields->AutoSize = true;
		this->label_P2TxFields->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Underline, 
			System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
		this->label_P2TxFields->Location = System::Drawing::Point(158, 83);
		this->label_P2TxFields->Name = L"label_P2TxFields";
		this->label_P2TxFields->Size = System::Drawing::Size(88, 13);
		this->label_P2TxFields->TabIndex = 37;
		this->label_P2TxFields->Text = L"Broadcast Fields:";
		// 
		// label_P2StatusFlags
		// 
		this->label_P2StatusFlags->AutoSize = true;
		this->label_P2StatusFlags->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Underline, 
			System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
		this->label_P2StatusFlags->Location = System::Drawing::Point(10, 6);
		this->label_P2StatusFlags->Name = L"label_P2StatusFlags";
		this->label_P2StatusFlags->Size = System::Drawing::Size(68, 13);
		this->label_P2StatusFlags->TabIndex = 8;
		this->label_P2StatusFlags->Text = L"Status Flags:";
		// 
		// checkBox_P2Cadence
		// 
		this->checkBox_P2Cadence->AutoSize = true;
		this->checkBox_P2Cadence->Checked = true;
		this->checkBox_P2Cadence->CheckState = System::Windows::Forms::CheckState::Checked;
		this->checkBox_P2Cadence->Location = System::Drawing::Point(252, 74);
		this->checkBox_P2Cadence->Name = L"checkBox_P2Cadence";
		this->checkBox_P2Cadence->Size = System::Drawing::Size(69, 17);
		this->checkBox_P2Cadence->TabIndex = 39;
		this->checkBox_P2Cadence->Text = L"Cadence";
		this->checkBox_P2Cadence->UseVisualStyleBackColor = true;
		this->checkBox_P2Cadence->CheckedChanged += gcnew System::EventHandler(this, &SDMSensor::checkBox_P2Cadence_CheckedChanged);
		// 
		// label_P2Use
		// 
		this->label_P2Use->AutoSize = true;
		this->label_P2Use->Location = System::Drawing::Point(311, 6);
		this->label_P2Use->Name = L"label_P2Use";
		this->label_P2Use->Size = System::Drawing::Size(54, 13);
		this->label_P2Use->TabIndex = 7;
		this->label_P2Use->Text = L"Use State";
		// 
		// label_P2Health
		// 
		this->label_P2Health->AutoSize = true;
		this->label_P2Health->Location = System::Drawing::Point(213, 6);
		this->label_P2Health->Name = L"label_P2Health";
		this->label_P2Health->Size = System::Drawing::Size(65, 13);
		this->label_P2Health->TabIndex = 6;
		this->label_P2Health->Text = L"SDM Health";
		// 
		// checkBox_P2InstSpeed
		// 
		this->checkBox_P2InstSpeed->AutoSize = true;
		this->checkBox_P2InstSpeed->Checked = true;
		this->checkBox_P2InstSpeed->CheckState = System::Windows::Forms::CheckState::Checked;
		this->checkBox_P2InstSpeed->Location = System::Drawing::Point(252, 91);
		this->checkBox_P2InstSpeed->Name = L"checkBox_P2InstSpeed";
		this->checkBox_P2InstSpeed->Size = System::Drawing::Size(127, 17);
		this->checkBox_P2InstSpeed->TabIndex = 40;
		this->checkBox_P2InstSpeed->Text = L"Instantaneous Speed";
		this->checkBox_P2InstSpeed->UseVisualStyleBackColor = true;
		this->checkBox_P2InstSpeed->CheckedChanged += gcnew System::EventHandler(this, &SDMSensor::checkBox_P2InstSpeed_CheckedChanged);
		// 
		// label_P2Battery
		// 
		this->label_P2Battery->AutoSize = true;
		this->label_P2Battery->Location = System::Drawing::Point(111, 6);
		this->label_P2Battery->Name = L"label_P2Battery";
		this->label_P2Battery->Size = System::Drawing::Size(73, 13);
		this->label_P2Battery->TabIndex = 5;
		this->label_P2Battery->Text = L"Battery Status";
		// 
		// label_P2Location
		// 
		this->label_P2Location->AutoSize = true;
		this->label_P2Location->Location = System::Drawing::Point(12, 23);
		this->label_P2Location->Name = L"label_P2Location";
		this->label_P2Location->Size = System::Drawing::Size(75, 13);
		this->label_P2Location->TabIndex = 4;
		this->label_P2Location->Text = L"SDM Location";
		// 
		// listBox_P2Use
		// 
		this->listBox_P2Use->FormattingEnabled = true;
		this->listBox_P2Use->Items->AddRange(gcnew cli::array< System::Object^  >(2) {L"00 - Inactive", L"01 - Active"});
		this->listBox_P2Use->Location = System::Drawing::Point(304, 23);
		this->listBox_P2Use->Name = L"listBox_P2Use";
		this->listBox_P2Use->Size = System::Drawing::Size(75, 30);
		this->listBox_P2Use->TabIndex = 38;
		this->listBox_P2Use->SelectedIndexChanged += gcnew System::EventHandler(this, &SDMSensor::listBox_P2Use_SelectedIndexChanged);
		// 
		// listBox_P2Health
		// 
		this->listBox_P2Health->FormattingEnabled = true;
		this->listBox_P2Health->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"00 - OK", L"01 - Error", L"10 - Warning"});
		this->listBox_P2Health->Location = System::Drawing::Point(210, 23);
		this->listBox_P2Health->Name = L"listBox_P2Health";
		this->listBox_P2Health->Size = System::Drawing::Size(75, 43);
		this->listBox_P2Health->TabIndex = 37;
		this->listBox_P2Health->SelectedIndexChanged += gcnew System::EventHandler(this, &SDMSensor::listBox_P2Health_SelectedIndexChanged);
		// 
		// listBox_P2Battery
		// 
		this->listBox_P2Battery->FormattingEnabled = true;
		this->listBox_P2Battery->Items->AddRange(gcnew cli::array< System::Object^  >(4) {L"00 - OK New", L"01 - OK Good", L"10 - OK", 
			L"11 - Low"});
		this->listBox_P2Battery->Location = System::Drawing::Point(111, 23);
		this->listBox_P2Battery->Name = L"listBox_P2Battery";
		this->listBox_P2Battery->Size = System::Drawing::Size(75, 56);
		this->listBox_P2Battery->TabIndex = 36;
		this->listBox_P2Battery->SelectedIndexChanged += gcnew System::EventHandler(this, &SDMSensor::listBox_P2Battery_SelectedIndexChanged);
		// 
		// listBox_P2Location
		// 
		this->listBox_P2Location->FormattingEnabled = true;
		this->listBox_P2Location->Items->AddRange(gcnew cli::array< System::Object^  >(4) {L"00 - Laces", L"01 - Midsole", L"10 - Chest", 
			L"11 - Other"});
		this->listBox_P2Location->Location = System::Drawing::Point(13, 40);
		this->listBox_P2Location->Name = L"listBox_P2Location";
		this->listBox_P2Location->Size = System::Drawing::Size(75, 56);
		this->listBox_P2Location->TabIndex = 35;
		this->listBox_P2Location->SelectedIndexChanged += gcnew System::EventHandler(this, &SDMSensor::listBox_P2Location_SelectedIndexChanged);
		// 
		// tabPage_Page3Settings
		// 
		this->tabPage_Page3Settings->Controls->Add(this->groupBox_P3Calories);
		this->tabPage_Page3Settings->Controls->Add(this->label_P3TxFields);
		this->tabPage_Page3Settings->Controls->Add(this->label_P3StatusFlags);
		this->tabPage_Page3Settings->Controls->Add(this->checkBox_P3Cadence);
		this->tabPage_Page3Settings->Controls->Add(this->label_P3Use);
		this->tabPage_Page3Settings->Controls->Add(this->label_P3Health);
		this->tabPage_Page3Settings->Controls->Add(this->checkBox_P3InstSpeed);
		this->tabPage_Page3Settings->Controls->Add(this->label_P3Battery);
		this->tabPage_Page3Settings->Controls->Add(this->label_P3Location);
		this->tabPage_Page3Settings->Controls->Add(this->listBox_P3Use);
		this->tabPage_Page3Settings->Controls->Add(this->listBox_P3Health);
		this->tabPage_Page3Settings->Controls->Add(this->listBox_P3Battery);
		this->tabPage_Page3Settings->Controls->Add(this->listBox_P3Location);
		this->tabPage_Page3Settings->Location = System::Drawing::Point(4, 22);
		this->tabPage_Page3Settings->Name = L"tabPage_Page3Settings";
		this->tabPage_Page3Settings->Padding = System::Windows::Forms::Padding(3);
		this->tabPage_Page3Settings->Size = System::Drawing::Size(389, 111);
		this->tabPage_Page3Settings->TabIndex = 4;
		this->tabPage_Page3Settings->Text = L"Page 3";
		this->tabPage_Page3Settings->UseVisualStyleBackColor = true;
		// 
		// groupBox_P3Calories
		// 
		this->groupBox_P3Calories->Controls->Add(this->textBox_P3Calories);
		this->groupBox_P3Calories->Controls->Add(this->button_P3UpdateCalories);
		this->groupBox_P3Calories->Controls->Add(this->label_P3CalDisplay);
		this->groupBox_P3Calories->Controls->Add(this->label_P3CalUnits);
		this->groupBox_P3Calories->ForeColor = System::Drawing::SystemColors::WindowText;
		this->groupBox_P3Calories->Location = System::Drawing::Point(289, 55);
		this->groupBox_P3Calories->Name = L"groupBox_P3Calories";
		this->groupBox_P3Calories->Size = System::Drawing::Size(94, 53);
		this->groupBox_P3Calories->TabIndex = 54;
		this->groupBox_P3Calories->TabStop = false;
		this->groupBox_P3Calories->Text = L"Total Calories";
		// 
		// textBox_P3Calories
		// 
		this->textBox_P3Calories->Enabled = false;
		this->textBox_P3Calories->Location = System::Drawing::Point(6, 29);
		this->textBox_P3Calories->MaxLength = 3;
		this->textBox_P3Calories->Name = L"textBox_P3Calories";
		this->textBox_P3Calories->Size = System::Drawing::Size(29, 20);
		this->textBox_P3Calories->TabIndex = 4;
		// 
		// button_P3UpdateCalories
		// 
		this->button_P3UpdateCalories->Enabled = false;
		this->button_P3UpdateCalories->Location = System::Drawing::Point(40, 29);
		this->button_P3UpdateCalories->Name = L"button_P3UpdateCalories";
		this->button_P3UpdateCalories->Size = System::Drawing::Size(50, 20);
		this->button_P3UpdateCalories->TabIndex = 3;
		this->button_P3UpdateCalories->Text = L"Update";
		this->button_P3UpdateCalories->UseVisualStyleBackColor = true;
		this->button_P3UpdateCalories->Click += gcnew System::EventHandler(this, &SDMSensor::button_P3UpdateCalories_Click);
		// 
		// label_P3CalDisplay
		// 
		this->label_P3CalDisplay->AutoSize = true;
		this->label_P3CalDisplay->Location = System::Drawing::Point(10, 14);
		this->label_P3CalDisplay->Name = L"label_P3CalDisplay";
		this->label_P3CalDisplay->Size = System::Drawing::Size(13, 13);
		this->label_P3CalDisplay->TabIndex = 2;
		this->label_P3CalDisplay->Text = L"0";
		// 
		// label_P3CalUnits
		// 
		this->label_P3CalUnits->AutoSize = true;
		this->label_P3CalUnits->Location = System::Drawing::Point(52, 14);
		this->label_P3CalUnits->Name = L"label_P3CalUnits";
		this->label_P3CalUnits->Size = System::Drawing::Size(33, 13);
		this->label_P3CalUnits->TabIndex = 1;
		this->label_P3CalUnits->Text = L"(kcal)";
		// 
		// label_P3TxFields
		// 
		this->label_P3TxFields->AutoSize = true;
		this->label_P3TxFields->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Underline, 
			System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
		this->label_P3TxFields->Location = System::Drawing::Point(98, 81);
		this->label_P3TxFields->Name = L"label_P3TxFields";
		this->label_P3TxFields->Size = System::Drawing::Size(88, 13);
		this->label_P3TxFields->TabIndex = 49;
		this->label_P3TxFields->Text = L"Broadcast Fields:";
		// 
		// label_P3StatusFlags
		// 
		this->label_P3StatusFlags->AutoSize = true;
		this->label_P3StatusFlags->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Underline, 
			System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
		this->label_P3StatusFlags->Location = System::Drawing::Point(10, 4);
		this->label_P3StatusFlags->Name = L"label_P3StatusFlags";
		this->label_P3StatusFlags->Size = System::Drawing::Size(68, 13);
		this->label_P3StatusFlags->TabIndex = 45;
		this->label_P3StatusFlags->Text = L"Status Flags:";
		// 
		// checkBox_P3Cadence
		// 
		this->checkBox_P3Cadence->AutoSize = true;
		this->checkBox_P3Cadence->Checked = true;
		this->checkBox_P3Cadence->CheckState = System::Windows::Forms::CheckState::Checked;
		this->checkBox_P3Cadence->Enabled = false;
		this->checkBox_P3Cadence->Location = System::Drawing::Point(192, 70);
		this->checkBox_P3Cadence->Name = L"checkBox_P3Cadence";
		this->checkBox_P3Cadence->Size = System::Drawing::Size(69, 17);
		this->checkBox_P3Cadence->TabIndex = 51;
		this->checkBox_P3Cadence->Text = L"Cadence";
		this->checkBox_P3Cadence->UseVisualStyleBackColor = true;
		this->checkBox_P3Cadence->CheckedChanged += gcnew System::EventHandler(this, &SDMSensor::checkBox_P3Cadence_CheckedChanged);
		// 
		// label_P3Use
		// 
		this->label_P3Use->AutoSize = true;
		this->label_P3Use->Location = System::Drawing::Point(311, 4);
		this->label_P3Use->Name = L"label_P3Use";
		this->label_P3Use->Size = System::Drawing::Size(54, 13);
		this->label_P3Use->TabIndex = 44;
		this->label_P3Use->Text = L"Use State";
		// 
		// label_P3Health
		// 
		this->label_P3Health->AutoSize = true;
		this->label_P3Health->Location = System::Drawing::Point(213, 4);
		this->label_P3Health->Name = L"label_P3Health";
		this->label_P3Health->Size = System::Drawing::Size(65, 13);
		this->label_P3Health->TabIndex = 43;
		this->label_P3Health->Text = L"SDM Health";
		// 
		// checkBox_P3InstSpeed
		// 
		this->checkBox_P3InstSpeed->AutoSize = true;
		this->checkBox_P3InstSpeed->Checked = true;
		this->checkBox_P3InstSpeed->CheckState = System::Windows::Forms::CheckState::Checked;
		this->checkBox_P3InstSpeed->Enabled = false;
		this->checkBox_P3InstSpeed->Location = System::Drawing::Point(192, 87);
		this->checkBox_P3InstSpeed->Name = L"checkBox_P3InstSpeed";
		this->checkBox_P3InstSpeed->Size = System::Drawing::Size(95, 17);
		this->checkBox_P3InstSpeed->TabIndex = 52;
		this->checkBox_P3InstSpeed->Text = L"Instant. Speed";
		this->checkBox_P3InstSpeed->UseVisualStyleBackColor = true;
		this->checkBox_P3InstSpeed->CheckedChanged += gcnew System::EventHandler(this, &SDMSensor::checkBox_P3InstSpeed_CheckedChanged);
		// 
		// label_P3Battery
		// 
		this->label_P3Battery->AutoSize = true;
		this->label_P3Battery->Location = System::Drawing::Point(111, 4);
		this->label_P3Battery->Name = L"label_P3Battery";
		this->label_P3Battery->Size = System::Drawing::Size(73, 13);
		this->label_P3Battery->TabIndex = 42;
		this->label_P3Battery->Text = L"Battery Status";
		// 
		// label_P3Location
		// 
		this->label_P3Location->AutoSize = true;
		this->label_P3Location->Location = System::Drawing::Point(12, 21);
		this->label_P3Location->Name = L"label_P3Location";
		this->label_P3Location->Size = System::Drawing::Size(75, 13);
		this->label_P3Location->TabIndex = 41;
		this->label_P3Location->Text = L"SDM Location";
		// 
		// listBox_P3Use
		// 
		this->listBox_P3Use->Enabled = false;
		this->listBox_P3Use->FormattingEnabled = true;
		this->listBox_P3Use->Items->AddRange(gcnew cli::array< System::Object^  >(2) {L"00 - Inactive", L"01 - Active"});
		this->listBox_P3Use->Location = System::Drawing::Point(304, 21);
		this->listBox_P3Use->Name = L"listBox_P3Use";
		this->listBox_P3Use->Size = System::Drawing::Size(75, 30);
		this->listBox_P3Use->TabIndex = 50;
		this->listBox_P3Use->SelectedIndexChanged += gcnew System::EventHandler(this, &SDMSensor::listBox_P3Use_SelectedIndexChanged);
		// 
		// listBox_P3Health
		// 
		this->listBox_P3Health->Enabled = false;
		this->listBox_P3Health->FormattingEnabled = true;
		this->listBox_P3Health->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"00 - OK", L"01 - Error", L"10 - Warning"});
		this->listBox_P3Health->Location = System::Drawing::Point(210, 21);
		this->listBox_P3Health->Name = L"listBox_P3Health";
		this->listBox_P3Health->Size = System::Drawing::Size(75, 43);
		this->listBox_P3Health->TabIndex = 48;
		this->listBox_P3Health->SelectedIndexChanged += gcnew System::EventHandler(this, &SDMSensor::listBox_P3Health_SelectedIndexChanged);
		// 
		// listBox_P3Battery
		// 
		this->listBox_P3Battery->Enabled = false;
		this->listBox_P3Battery->FormattingEnabled = true;
		this->listBox_P3Battery->Items->AddRange(gcnew cli::array< System::Object^  >(4) {L"00 - OK New", L"01 - OK Good", L"10 - OK", 
			L"11 - Low"});
		this->listBox_P3Battery->Location = System::Drawing::Point(111, 21);
		this->listBox_P3Battery->Name = L"listBox_P3Battery";
		this->listBox_P3Battery->Size = System::Drawing::Size(75, 56);
		this->listBox_P3Battery->TabIndex = 47;
		this->listBox_P3Battery->SelectedIndexChanged += gcnew System::EventHandler(this, &SDMSensor::listBox_P3Battery_SelectedIndexChanged);
		// 
		// listBox_P3Location
		// 
		this->listBox_P3Location->Enabled = false;
		this->listBox_P3Location->FormattingEnabled = true;
		this->listBox_P3Location->Items->AddRange(gcnew cli::array< System::Object^  >(4) {L"00 - Laces", L"01 - Midsole", L"10 - Chest", 
			L"11 - Other"});
		this->listBox_P3Location->Location = System::Drawing::Point(13, 38);
		this->listBox_P3Location->Name = L"listBox_P3Location";
		this->listBox_P3Location->Size = System::Drawing::Size(75, 56);
		this->listBox_P3Location->TabIndex = 46;
		this->listBox_P3Location->SelectedIndexChanged += gcnew System::EventHandler(this, &SDMSensor::listBox_P3Location_SelectedIndexChanged);
		// 
		// tabPage1
		// 
		this->tabPage1->Controls->Add(this->label_P16UpdateError);
		this->tabPage1->Controls->Add(this->label_P16_UnitsDist);
		this->tabPage1->Controls->Add(this->checkBox_P16Enabled);
		this->tabPage1->Controls->Add(this->button_P16UpdateDistance);
		this->tabPage1->Controls->Add(this->label_P16Distance);
		this->tabPage1->Controls->Add(this->label_P16Strides);
		this->tabPage1->Controls->Add(this->label_P16StridesDisplay);
		this->tabPage1->Controls->Add(this->button_P16UpdateStrides);
		this->tabPage1->Controls->Add(this->textBox_P16DistDisplayChange);
		this->tabPage1->Controls->Add(this->label_P16DistanceDisplay);
		this->tabPage1->Controls->Add(this->textBox_P16StridesDisplayChange);
		this->tabPage1->Controls->Add(this->label_P16Title);
		this->tabPage1->Location = System::Drawing::Point(4, 22);
		this->tabPage1->Name = L"tabPage1";
		this->tabPage1->Padding = System::Windows::Forms::Padding(3);
		this->tabPage1->Size = System::Drawing::Size(389, 111);
		this->tabPage1->TabIndex = 5;
		this->tabPage1->Text = L"Page 16";
		this->tabPage1->UseVisualStyleBackColor = true;
		// 
		// label_P16UpdateError
		// 
		this->label_P16UpdateError->AutoSize = true;
		this->label_P16UpdateError->Location = System::Drawing::Point(207, 85);
		this->label_P16UpdateError->Name = L"label_P16UpdateError";
		this->label_P16UpdateError->Size = System::Drawing::Size(32, 13);
		this->label_P16UpdateError->TabIndex = 48;
		this->label_P16UpdateError->Text = L"Error:";
		this->label_P16UpdateError->Visible = false;
		// 
		// label_P16_UnitsDist
		// 
		this->label_P16_UnitsDist->AutoSize = true;
		this->label_P16_UnitsDist->Location = System::Drawing::Point(54, 78);
		this->label_P16_UnitsDist->Name = L"label_P16_UnitsDist";
		this->label_P16_UnitsDist->Size = System::Drawing::Size(53, 13);
		this->label_P16_UnitsDist->TabIndex = 47;
		this->label_P16_UnitsDist->Text = L"(1/256 m)";
		// 
		// checkBox_P16Enabled
		// 
		this->checkBox_P16Enabled->AutoSize = true;
		this->checkBox_P16Enabled->Checked = true;
		this->checkBox_P16Enabled->CheckState = System::Windows::Forms::CheckState::Checked;
		this->checkBox_P16Enabled->Location = System::Drawing::Point(247, 7);
		this->checkBox_P16Enabled->Name = L"checkBox_P16Enabled";
		this->checkBox_P16Enabled->Size = System::Drawing::Size(65, 17);
		this->checkBox_P16Enabled->TabIndex = 46;
		this->checkBox_P16Enabled->Text = L"Enabled";
		this->checkBox_P16Enabled->UseVisualStyleBackColor = true;
		this->checkBox_P16Enabled->CheckedChanged += gcnew System::EventHandler(this, &SDMSensor::checkBox_P16Enabled_CheckedChanged);
		// 
		// button_P16UpdateDistance
		// 
		this->button_P16UpdateDistance->Location = System::Drawing::Point(286, 60);
		this->button_P16UpdateDistance->Name = L"button_P16UpdateDistance";
		this->button_P16UpdateDistance->Size = System::Drawing::Size(97, 20);
		this->button_P16UpdateDistance->TabIndex = 45;
		this->button_P16UpdateDistance->Text = L"Update Distance";
		this->button_P16UpdateDistance->UseVisualStyleBackColor = true;
		this->button_P16UpdateDistance->Click += gcnew System::EventHandler(this, &SDMSensor::button_P16UpdateDistance_Click);
		// 
		// label_P16Distance
		// 
		this->label_P16Distance->AutoSize = true;
		this->label_P16Distance->Location = System::Drawing::Point(19, 65);
		this->label_P16Distance->Name = L"label_P16Distance";
		this->label_P16Distance->Size = System::Drawing::Size(117, 13);
		this->label_P16Distance->TabIndex = 37;
		this->label_P16Distance->Text = L"Accumulated Distance:";
		// 
		// label_P16Strides
		// 
		this->label_P16Strides->AutoSize = true;
		this->label_P16Strides->Location = System::Drawing::Point(29, 43);
		this->label_P16Strides->Name = L"label_P16Strides";
		this->label_P16Strides->Size = System::Drawing::Size(107, 13);
		this->label_P16Strides->TabIndex = 36;
		this->label_P16Strides->Text = L"Accumulated Strides:";
		// 
		// label_P16StridesDisplay
		// 
		this->label_P16StridesDisplay->Location = System::Drawing::Point(140, 43);
		this->label_P16StridesDisplay->Name = L"label_P16StridesDisplay";
		this->label_P16StridesDisplay->Size = System::Drawing::Size(70, 13);
		this->label_P16StridesDisplay->TabIndex = 38;
		this->label_P16StridesDisplay->Text = L"0";
		// 
		// button_P16UpdateStrides
		// 
		this->button_P16UpdateStrides->Location = System::Drawing::Point(286, 38);
		this->button_P16UpdateStrides->Name = L"button_P16UpdateStrides";
		this->button_P16UpdateStrides->Size = System::Drawing::Size(97, 20);
		this->button_P16UpdateStrides->TabIndex = 44;
		this->button_P16UpdateStrides->Text = L"Update Strides";
		this->button_P16UpdateStrides->UseVisualStyleBackColor = true;
		this->button_P16UpdateStrides->Click += gcnew System::EventHandler(this, &SDMSensor::button_P16UpdateStrides_Click);
		// 
		// textBox_P16DistDisplayChange
		// 
		this->textBox_P16DistDisplayChange->Location = System::Drawing::Point(210, 62);
		this->textBox_P16DistDisplayChange->MaxLength = 10;
		this->textBox_P16DistDisplayChange->Name = L"textBox_P16DistDisplayChange";
		this->textBox_P16DistDisplayChange->Size = System::Drawing::Size(70, 20);
		this->textBox_P16DistDisplayChange->TabIndex = 43;
		this->textBox_P16DistDisplayChange->Text = L"0";
		// 
		// label_P16DistanceDisplay
		// 
		this->label_P16DistanceDisplay->Location = System::Drawing::Point(140, 65);
		this->label_P16DistanceDisplay->Name = L"label_P16DistanceDisplay";
		this->label_P16DistanceDisplay->Size = System::Drawing::Size(70, 13);
		this->label_P16DistanceDisplay->TabIndex = 39;
		this->label_P16DistanceDisplay->Text = L"0";
		// 
		// textBox_P16StridesDisplayChange
		// 
		this->textBox_P16StridesDisplayChange->Location = System::Drawing::Point(210, 39);
		this->textBox_P16StridesDisplayChange->MaxLength = 8;
		this->textBox_P16StridesDisplayChange->Name = L"textBox_P16StridesDisplayChange";
		this->textBox_P16StridesDisplayChange->Size = System::Drawing::Size(70, 20);
		this->textBox_P16StridesDisplayChange->TabIndex = 41;
		this->textBox_P16StridesDisplayChange->Text = L"0";
		// 
		// label_P16Title
		// 
		this->label_P16Title->AutoSize = true;
		this->label_P16Title->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
			static_cast<System::Byte>(0)));
		this->label_P16Title->Location = System::Drawing::Point(6, 7);
		this->label_P16Title->Name = L"label_P16Title";
		this->label_P16Title->Size = System::Drawing::Size(235, 13);
		this->label_P16Title->TabIndex = 1;
		this->label_P16Title->Text = L"Request Page 16 - Distance and Strides";
		// 
		// tabPage2
		// 
		this->tabPage2->Controls->Add(this->checkBox_P22Ignore);
		this->tabPage2->Controls->Add(this->groupBox_P22Supported);
		this->tabPage2->Controls->Add(this->checkBox_P22Enabled);
		this->tabPage2->Controls->Add(this->label_P22Title);
		this->tabPage2->Location = System::Drawing::Point(4, 22);
		this->tabPage2->Name = L"tabPage2";
		this->tabPage2->Padding = System::Windows::Forms::Padding(3);
		this->tabPage2->Size = System::Drawing::Size(389, 111);
		this->tabPage2->TabIndex = 6;
		this->tabPage2->Text = L"Page 22";
		this->tabPage2->UseVisualStyleBackColor = true;
		// 
		// checkBox_P22Ignore
		// 
		this->checkBox_P22Ignore->AutoSize = true;
		this->checkBox_P22Ignore->Location = System::Drawing::Point(9, 88);
		this->checkBox_P22Ignore->Name = L"checkBox_P22Ignore";
		this->checkBox_P22Ignore->Size = System::Drawing::Size(202, 17);
		this->checkBox_P22Ignore->TabIndex = 12;
		this->checkBox_P22Ignore->Text = L"Ignore Request if all Fields Supported";
		this->checkBox_P22Ignore->UseVisualStyleBackColor = true;
		this->checkBox_P22Ignore->CheckedChanged += gcnew System::EventHandler(this, &SDMSensor::checkBox_P22Ignore_CheckedChanged);
		// 
		// groupBox_P22Supported
		// 
		this->groupBox_P22Supported->Controls->Add(this->label_P22Time);
		this->groupBox_P22Supported->Controls->Add(this->label_P22Calories);
		this->groupBox_P22Supported->Controls->Add(this->label_P22Distance);
		this->groupBox_P22Supported->Controls->Add(this->label_P22Cadence);
		this->groupBox_P22Supported->Controls->Add(this->label_P22Speed);
		this->groupBox_P22Supported->Controls->Add(this->label_P22Latency);
		this->groupBox_P22Supported->ForeColor = System::Drawing::SystemColors::WindowText;
		this->groupBox_P22Supported->Location = System::Drawing::Point(231, 23);
		this->groupBox_P22Supported->Name = L"groupBox_P22Supported";
		this->groupBox_P22Supported->Size = System::Drawing::Size(152, 82);
		this->groupBox_P22Supported->TabIndex = 11;
		this->groupBox_P22Supported->TabStop = false;
		this->groupBox_P22Supported->Text = L"Supported Capabilities";
		// 
		// label_P22Time
		// 
		this->label_P22Time->AutoSize = true;
		this->label_P22Time->Location = System::Drawing::Point(20, 23);
		this->label_P22Time->Name = L"label_P22Time";
		this->label_P22Time->Size = System::Drawing::Size(30, 13);
		this->label_P22Time->TabIndex = 5;
		this->label_P22Time->Text = L"Time";
		// 
		// label_P22Calories
		// 
		this->label_P22Calories->AutoSize = true;
		this->label_P22Calories->Location = System::Drawing::Point(83, 57);
		this->label_P22Calories->Name = L"label_P22Calories";
		this->label_P22Calories->Size = System::Drawing::Size(44, 13);
		this->label_P22Calories->TabIndex = 10;
		this->label_P22Calories->Text = L"Calories";
		// 
		// label_P22Distance
		// 
		this->label_P22Distance->AutoSize = true;
		this->label_P22Distance->Location = System::Drawing::Point(20, 40);
		this->label_P22Distance->Name = L"label_P22Distance";
		this->label_P22Distance->Size = System::Drawing::Size(49, 13);
		this->label_P22Distance->TabIndex = 6;
		this->label_P22Distance->Text = L"Distance";
		// 
		// label_P22Cadence
		// 
		this->label_P22Cadence->AutoSize = true;
		this->label_P22Cadence->Location = System::Drawing::Point(82, 40);
		this->label_P22Cadence->Name = L"label_P22Cadence";
		this->label_P22Cadence->Size = System::Drawing::Size(50, 13);
		this->label_P22Cadence->TabIndex = 9;
		this->label_P22Cadence->Text = L"Cadence";
		// 
		// label_P22Speed
		// 
		this->label_P22Speed->AutoSize = true;
		this->label_P22Speed->Location = System::Drawing::Point(20, 57);
		this->label_P22Speed->Name = L"label_P22Speed";
		this->label_P22Speed->Size = System::Drawing::Size(38, 13);
		this->label_P22Speed->TabIndex = 7;
		this->label_P22Speed->Text = L"Speed";
		// 
		// label_P22Latency
		// 
		this->label_P22Latency->AutoSize = true;
		this->label_P22Latency->Location = System::Drawing::Point(82, 23);
		this->label_P22Latency->Name = L"label_P22Latency";
		this->label_P22Latency->Size = System::Drawing::Size(45, 13);
		this->label_P22Latency->TabIndex = 8;
		this->label_P22Latency->Text = L"Latency";
		// 
		// checkBox_P22Enabled
		// 
		this->checkBox_P22Enabled->AutoSize = true;
		this->checkBox_P22Enabled->Checked = true;
		this->checkBox_P22Enabled->CheckState = System::Windows::Forms::CheckState::Checked;
		this->checkBox_P22Enabled->Location = System::Drawing::Point(194, 7);
		this->checkBox_P22Enabled->Name = L"checkBox_P22Enabled";
		this->checkBox_P22Enabled->Size = System::Drawing::Size(65, 17);
		this->checkBox_P22Enabled->TabIndex = 4;
		this->checkBox_P22Enabled->Text = L"Enabled";
		this->checkBox_P22Enabled->UseVisualStyleBackColor = true;
		this->checkBox_P22Enabled->CheckedChanged += gcnew System::EventHandler(this, &SDMSensor::checkBox_P22Enabled_CheckedChanged);
		// 
		// label_P22Title
		// 
		this->label_P22Title->AutoSize = true;
		this->label_P22Title->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
			static_cast<System::Byte>(0)));
		this->label_P22Title->Location = System::Drawing::Point(6, 7);
		this->label_P22Title->Name = L"label_P22Title";
		this->label_P22Title->Size = System::Drawing::Size(182, 13);
		this->label_P22Title->TabIndex = 2;
		this->label_P22Title->Text = L"Request Page 22 - Capabilities";
		// 
		// panel_Display
		// 
		this->panel_Display->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
		this->panel_Display->Controls->Add(this->label_TransCal);
		this->panel_Display->Controls->Add(this->label_TransCalDisplay);
		this->panel_Display->Controls->Add(this->label_TransDist);
		this->panel_Display->Controls->Add(this->label_TransInstSpeed);
		this->panel_Display->Controls->Add(this->label_TransCadence);
		this->panel_Display->Controls->Add(this->label_Trn_Time);
		this->panel_Display->Controls->Add(this->label_TransDistDisplay);
		this->panel_Display->Controls->Add(this->label_TransInstSpeedDisplay);
		this->panel_Display->Controls->Add(this->label_TransCadenceDisplay);
		this->panel_Display->Controls->Add(this->label_TransDisplayTitle);
		this->panel_Display->Controls->Add(this->label_Trn_TimeDisplay);
		this->panel_Display->Controls->Add(this->label_TransLatency);
		this->panel_Display->Controls->Add(this->label_TransLatencyDisplay);
		this->panel_Display->Controls->Add(this->label_TransStrideCountDisplay);
		this->panel_Display->Controls->Add(this->label_TransStrideCount);
		this->panel_Display->Location = System::Drawing::Point(58, 188);
		this->panel_Display->Name = L"panel_Display";
		this->panel_Display->Size = System::Drawing::Size(200, 90);
		this->panel_Display->TabIndex = 1;
		// 
		// label_TransCal
		// 
		this->label_TransCal->AutoSize = true;
		this->label_TransCal->BackColor = System::Drawing::Color::Transparent;
		this->label_TransCal->Location = System::Drawing::Point(-1, 71);
		this->label_TransCal->Name = L"label_TransCal";
		this->label_TransCal->Size = System::Drawing::Size(36, 13);
		this->label_TransCal->TabIndex = 17;
		this->label_TransCal->Text = L"(kcal):";
		// 
		// label_TransCalDisplay
		// 
		this->label_TransCalDisplay->AutoSize = true;
		this->label_TransCalDisplay->BackColor = System::Drawing::Color::Transparent;
		this->label_TransCalDisplay->Location = System::Drawing::Point(41, 71);
		this->label_TransCalDisplay->Name = L"label_TransCalDisplay";
		this->label_TransCalDisplay->Size = System::Drawing::Size(16, 13);
		this->label_TransCalDisplay->TabIndex = 16;
		this->label_TransCalDisplay->Text = L"---";
		// 
		// label_TransDist
		// 
		this->label_TransDist->AutoSize = true;
		this->label_TransDist->BackColor = System::Drawing::SystemColors::Control;
		this->label_TransDist->Location = System::Drawing::Point(68, 71);
		this->label_TransDist->Name = L"label_TransDist";
		this->label_TransDist->Size = System::Drawing::Size(69, 13);
		this->label_TransDist->TabIndex = 5;
		this->label_TransDist->Text = L"Distance (m):";
		// 
		// label_TransInstSpeed
		// 
		this->label_TransInstSpeed->AutoSize = true;
		this->label_TransInstSpeed->BackColor = System::Drawing::SystemColors::Control;
		this->label_TransInstSpeed->Location = System::Drawing::Point(-1, 58);
		this->label_TransInstSpeed->Name = L"label_TransInstSpeed";
		this->label_TransInstSpeed->Size = System::Drawing::Size(138, 13);
		this->label_TransInstSpeed->TabIndex = 1;
		this->label_TransInstSpeed->Text = L"Instantaneous Speed (m/s):";
		// 
		// label_TransCadence
		// 
		this->label_TransCadence->AutoSize = true;
		this->label_TransCadence->BackColor = System::Drawing::SystemColors::Control;
		this->label_TransCadence->Location = System::Drawing::Point(24, 45);
		this->label_TransCadence->Name = L"label_TransCadence";
		this->label_TransCadence->Size = System::Drawing::Size(113, 13);
		this->label_TransCadence->TabIndex = 9;
		this->label_TransCadence->Text = L"Cadence (strides/min):";
		// 
		// label_Trn_Time
		// 
		this->label_Trn_Time->AutoSize = true;
		this->label_Trn_Time->Location = System::Drawing::Point(90, 32);
		this->label_Trn_Time->Name = L"label_Trn_Time";
		this->label_Trn_Time->Size = System::Drawing::Size(47, 13);
		this->label_Trn_Time->TabIndex = 11;
		this->label_Trn_Time->Text = L"Time (s):";
		// 
		// label_TransDistDisplay
		// 
		this->label_TransDistDisplay->AutoSize = true;
		this->label_TransDistDisplay->BackColor = System::Drawing::SystemColors::Control;
		this->label_TransDistDisplay->Location = System::Drawing::Point(143, 71);
		this->label_TransDistDisplay->Name = L"label_TransDistDisplay";
		this->label_TransDistDisplay->Size = System::Drawing::Size(16, 13);
		this->label_TransDistDisplay->TabIndex = 6;
		this->label_TransDistDisplay->Text = L"---";
		// 
		// label_TransInstSpeedDisplay
		// 
		this->label_TransInstSpeedDisplay->AutoSize = true;
		this->label_TransInstSpeedDisplay->BackColor = System::Drawing::SystemColors::Control;
		this->label_TransInstSpeedDisplay->Location = System::Drawing::Point(143, 58);
		this->label_TransInstSpeedDisplay->Name = L"label_TransInstSpeedDisplay";
		this->label_TransInstSpeedDisplay->Size = System::Drawing::Size(16, 13);
		this->label_TransInstSpeedDisplay->TabIndex = 3;
		this->label_TransInstSpeedDisplay->Text = L"---";
		// 
		// label_TransCadenceDisplay
		// 
		this->label_TransCadenceDisplay->AutoSize = true;
		this->label_TransCadenceDisplay->BackColor = System::Drawing::SystemColors::Control;
		this->label_TransCadenceDisplay->Location = System::Drawing::Point(143, 45);
		this->label_TransCadenceDisplay->Name = L"label_TransCadenceDisplay";
		this->label_TransCadenceDisplay->Size = System::Drawing::Size(16, 13);
		this->label_TransCadenceDisplay->TabIndex = 10;
		this->label_TransCadenceDisplay->Text = L"---";
		// 
		// label_TransDisplayTitle
		// 
		this->label_TransDisplayTitle->AutoSize = true;
		this->label_TransDisplayTitle->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Underline, 
			System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
		this->label_TransDisplayTitle->Location = System::Drawing::Point(2, 2);
		this->label_TransDisplayTitle->Name = L"label_TransDisplayTitle";
		this->label_TransDisplayTitle->Size = System::Drawing::Size(135, 13);
		this->label_TransDisplayTitle->TabIndex = 4;
		this->label_TransDisplayTitle->Text = L"Current SDM Transmission:";
		// 
		// label_Trn_TimeDisplay
		// 
		this->label_Trn_TimeDisplay->AutoSize = true;
		this->label_Trn_TimeDisplay->BackColor = System::Drawing::SystemColors::Control;
		this->label_Trn_TimeDisplay->Location = System::Drawing::Point(143, 32);
		this->label_Trn_TimeDisplay->Name = L"label_Trn_TimeDisplay";
		this->label_Trn_TimeDisplay->Size = System::Drawing::Size(16, 13);
		this->label_Trn_TimeDisplay->TabIndex = 12;
		this->label_Trn_TimeDisplay->Text = L"---";
		// 
		// label_TransLatency
		// 
		this->label_TransLatency->AutoSize = true;
		this->label_TransLatency->BackColor = System::Drawing::SystemColors::Control;
		this->label_TransLatency->Location = System::Drawing::Point(1, 18);
		this->label_TransLatency->Name = L"label_TransLatency";
		this->label_TransLatency->Size = System::Drawing::Size(70, 13);
		this->label_TransLatency->TabIndex = 7;
		this->label_TransLatency->Text = L"Latency (ms):";
		// 
		// label_TransLatencyDisplay
		// 
		this->label_TransLatencyDisplay->AutoSize = true;
		this->label_TransLatencyDisplay->BackColor = System::Drawing::SystemColors::Control;
		this->label_TransLatencyDisplay->Location = System::Drawing::Point(77, 18);
		this->label_TransLatencyDisplay->Name = L"label_TransLatencyDisplay";
		this->label_TransLatencyDisplay->Size = System::Drawing::Size(16, 13);
		this->label_TransLatencyDisplay->TabIndex = 8;
		this->label_TransLatencyDisplay->Text = L"---";
		// 
		// label_TransStrideCountDisplay
		// 
		this->label_TransStrideCountDisplay->AutoSize = true;
		this->label_TransStrideCountDisplay->BackColor = System::Drawing::SystemColors::Control;
		this->label_TransStrideCountDisplay->Location = System::Drawing::Point(173, 18);
		this->label_TransStrideCountDisplay->Name = L"label_TransStrideCountDisplay";
		this->label_TransStrideCountDisplay->Size = System::Drawing::Size(16, 13);
		this->label_TransStrideCountDisplay->TabIndex = 2;
		this->label_TransStrideCountDisplay->Text = L"---";
		// 
		// label_TransStrideCount
		// 
		this->label_TransStrideCount->AutoSize = true;
		this->label_TransStrideCount->BackColor = System::Drawing::SystemColors::Control;
		this->label_TransStrideCount->Location = System::Drawing::Point(125, 18);
		this->label_TransStrideCount->Name = L"label_TransStrideCount";
		this->label_TransStrideCount->Size = System::Drawing::Size(42, 13);
		this->label_TransStrideCount->TabIndex = 0;
		this->label_TransStrideCount->Text = L"Strides:";
		// 
		// checkBox2
		// 
		this->checkBox2->AutoSize = true;
		this->checkBox2->Location = System::Drawing::Point(16, 19);
		this->checkBox2->Name = L"checkBox2";
		this->checkBox2->Size = System::Drawing::Size(65, 17);
		this->checkBox2->TabIndex = 0;
		this->checkBox2->Text = L"Enabled";
		this->checkBox2->UseVisualStyleBackColor = true;
		// 
		// SDMSensor
		// 
		this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
		this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
		this->ClientSize = System::Drawing::Size(794, 351);
		this->Controls->Add(this->panel_Display);
		this->Controls->Add(this->panel_Settings);
		this->Name = L"SDMSensor";
		this->Text = L"SDMSensor";
		this->panel_Settings->ResumeLayout(false);
		this->tabControl_Settings->ResumeLayout(false);
		this->tabPage_Parameters->ResumeLayout(false);
		this->tabPage_Parameters->PerformLayout();
		(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_BurnRate))->EndInit();
		(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_StrideLength))->EndInit();
		(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_MaxOutput))->EndInit();
		(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_CurOutput))->EndInit();
		(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_MinOutput))->EndInit();
		this->panel_PageType->ResumeLayout(false);
		this->panel_PageType->PerformLayout();
		this->tabPage_DeviceInfo->ResumeLayout(false);
		this->tabPage_DeviceInfo->PerformLayout();
		this->tabPage_Page1Settings->ResumeLayout(false);
		this->tabPage_Page1Settings->PerformLayout();
		this->tabPage_Page2Settings->ResumeLayout(false);
		this->tabPage_Page2Settings->PerformLayout();
		this->tabPage_Page3Settings->ResumeLayout(false);
		this->tabPage_Page3Settings->PerformLayout();
		this->groupBox_P3Calories->ResumeLayout(false);
		this->groupBox_P3Calories->PerformLayout();
		this->tabPage1->ResumeLayout(false);
		this->tabPage1->PerformLayout();
		this->tabPage2->ResumeLayout(false);
		this->tabPage2->PerformLayout();
		this->groupBox_P22Supported->ResumeLayout(false);
		this->groupBox_P22Supported->PerformLayout();
		this->panel_Display->ResumeLayout(false);
		this->panel_Display->PerformLayout();
		this->ResumeLayout(false);

	}
#pragma endregion
};