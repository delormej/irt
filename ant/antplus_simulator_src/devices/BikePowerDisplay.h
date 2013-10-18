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
#include "antplus_bikepower.h"
#include "antplus_common.h"
#include "types.h"
#include "antdefines.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

public ref class BikePowerDisplay : public System::Windows::Forms::Form, public ISimBase{

public:
	BikePowerDisplay(dRequestAckMsg^ channelAckMsg){
		requestAckMsg = channelAckMsg;
		commonPages = gcnew CommonData();
		bpPages = gcnew BikePower();
		InitializeComponent();
		InitializeSim();
	}

	~BikePowerDisplay(){
		delete this->panel_Display;
		delete this->panel_Settings;
		//clean up floating resources with the garbage collector
		GC::Collect(2);

		//Deletion of designer component
		if (components)
		{
			delete components;
		}
		if(bpPages)
			delete bpPages;
		if(commonPages)
			delete commonPages;
	}

public:	
	virtual void ANT_eventNotification(UCHAR ucEventCode_, UCHAR* pucEventBuffer_);
	virtual UCHAR getDeviceType(){return BikePower::DEVICE_TYPE;}
	virtual UCHAR getTransmissionType(){return 0;}	// Set to 0 for a pairing search and future compatibility
	virtual USHORT getTransmitPeriod(){return BikePower::MSG_PERIOD;}
	virtual DOUBLE getTimerInterval(){return 3600000;} // Set interval to one hour, as timer is not used in the simulation
	virtual void onTimerTock(USHORT eventTime){} // Do nothing
	virtual System::Windows::Forms::Panel^ getSimSettingsPanel(){return this->panel_Settings;}
	virtual System::Windows::Forms::Panel^ getSimTranslatedDisplay(){return this->panel_Display;}
	
private:
	void InitializeSim();
	void HandleReceive(UCHAR* pucRxBuffer_);
	BOOL HandleRetransmit();
	void SendMessage(UCHAR ucMsgCode_);
	void UpdateDisplay(UCHAR ucPageNum_);
	void UpdateDisplayAckStatus(UCHAR ucStatus_);
	void ChangeSensorType();
	System::Void button_Cal_Calibrate_Click(System::Object^  sender, System::EventArgs^  e);
	System::Void button_Cal_AutoZeroSet_Click(System::Object^  sender, System::EventArgs^  e);
	System::Void button_Cal_CTFSerialSet_Click(System::Object^  sender, System::EventArgs^  e);
	System::Void button_Cal_CTFSlopeSet_Click(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_Cal_WheelCircum_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void radioButton_Cal_AZ_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_Cal_CTFSerial_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_Cal_CTFSlope_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void button_GetCrankStatus_Click(System::Object^  sender, System::EventArgs^  e);
	System::Void button_SetCrankStatus_Click(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_InvalidCrankLength_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_AutoCrank_CheckedChanged(System::Object^  sender, System::EventArgs^  e);

	template< typename T>
	T GetDifference(T CurrentVal, T PreviousVal);


private:	// Implementation specific constants
	static const UCHAR ACK_SUCCESS = 0;
	static const UCHAR ACK_RETRY = 1;
	static const UCHAR ACK_FAIL = 2;
	static const UCHAR MAX_RETRIES = 40;	// Maximum number of retransmissions for each message
	static const USHORT PI256 = 804;		// Pi * 256

private:
	dRequestAckMsg^ requestAckMsg;		// Delegate for handling Ack data
	CommonData^ commonPages;			// Handle to common data pages
	BikePower^ bpPages;					// Handle to bike power data pages
	
	// Pages received so far
	BOOL bSeenPower;					// Standard power
	BOOL bSeenWheelTorque;				// Wheel torque
	BOOL bSeenCrankTorque;				// Crank torque
	BOOL bSeenCTF;						// Crank torque frequency
	BikePower::SensorType ePrevType;	// Previous sensor type
	BOOL bRequestCalibration;			// Used to determine which ack message to send
	
	ULONG ulAveragePower256;			// Average power

	// Power-only sensors
	ULONG ulAcumPowerEventCount;		// Cumulative power event count 

	// Torque sensors (Wheel or Crank)
	ULONG ulAcumTorqueEventCount;		// Cumulative torque event count
	ULONG ulAcumTicks;					// Cumulative revolutions (wheel or torque)	
	ULONG ulSpeed;						// Average speed (meters/h)
	ULONG ulDistance100;				// Distance (cm)
	ULONG ulAngularVelocity256;			// Angular velocity (1/100 rad/s)
	ULONG ulAverageTorque32;			// Average torque (1/32 Nm)
	ULONG ulAverageCadence;				// Average cadence (rpm)
	UCHAR ucWheelCircumference100;		// Wheel circumference (cm)

	// Crank Torque Frequency Sensors
	ULONG ulAcumCTFEventCount;			// Cumulative crank torque frequency event count
	USHORT usCTFOffset;					// Calibration offset

	// Calibration
	BOOL bAZOn;							// Auto Zero Status (set on display)
	USHORT usCalSerialToSend;			// Serial number to send on CTF message (set on display)
	USHORT usCalSlopeToSend;			// Slope to send on CTF message (set on display)
	BOOL bAutoZeroInvalid;				// Bad auto zero status received
	BOOL bBadCalPage;					// Bad calibration page received
	UCHAR ucMsgExpectingAck;			// Message pending to be acknowledged
	UCHAR ucAckRetryCount;				// Number of retries for an acknowledged message

	// Special conditions
	BOOL bCoasting;						// Bike is coasting
	BOOL bStopped;						// Bike stopped

private: System::Windows::Forms::Panel^  panel_Settings;
private: System::Windows::Forms::Panel^  panel_Display;
private: System::Windows::Forms::Label^  label_Trn_EventCount;
private: System::Windows::Forms::Label^  label_Trn_EventCountDisplay;
private: System::Windows::Forms::Label^  label_Trn_DisplayTitle;
private: System::Windows::Forms::Label^  label_Trn_AccumOneDisplay;
private: System::Windows::Forms::Label^  label_Trn_AccumOne;
private: System::Windows::Forms::Label^  label_Trn_CadenceDisplay;
private: System::Windows::Forms::Label^  label_Trn_Cadence;
private: System::Windows::Forms::Label^  label_Trn_PageDisplay;
private: System::Windows::Forms::Label^  label_Trn_Page;
private: System::Windows::Forms::Label^  label_Trn_UpdateCountDisplay;
private: System::Windows::Forms::Label^  label_Trn_UpdateCount;
private: System::Windows::Forms::Label^  label_Trn_AccumTitle;
private: System::Windows::Forms::Label^  label_Trn_AccumTwo;
private: System::Windows::Forms::Label^  label_Trn_AccumTwoDisplay;
private: System::Windows::Forms::TabControl^  tabControl_Settings;
private: System::Windows::Forms::Label^  label_Cal_TorqOffset;
private: System::Windows::Forms::Label^  label_Cal_RawTorq;
private: System::Windows::Forms::Label^  label_Cal_AZStatus;
private: System::Windows::Forms::ListBox^  listBox_Cal_AZStatus;
private: System::Windows::Forms::TabPage^  tabPage_GlobalBattery;
private: System::Windows::Forms::Label^  label_Bat_Volts;
private: System::Windows::Forms::Label^  label_Bat_Status;
private: System::Windows::Forms::ListBox^  listBox_Bat_Status;
private: System::Windows::Forms::Label^  label_Bat_ElpTimeDisplay;
private: System::Windows::Forms::Label^  label_Bat_ElpTime;
private: System::Windows::Forms::Label^  label_Dat_Stopped;
private: System::Windows::Forms::Label^  label_Dat_PageRec;
private: System::Windows::Forms::Label^  label_Cal_CalNum;
private: System::Windows::Forms::Label^  label_Glb_SerialNum;
private: System::Windows::Forms::Label^  label_Glb_SerialNumDisplay;
private: System::Windows::Forms::Label^  label_Glb_SoftwareVerDisplay;
private: System::Windows::Forms::Label^  label_Glb_SoftwareVer;
private: System::Windows::Forms::Label^  label_Glb_ManfID;
private: System::Windows::Forms::Label^  label_Glb_ManfIDDisplay;
private: System::Windows::Forms::Label^  label_Glb_ModelNumDisplay;
private: System::Windows::Forms::Label^  label_Glb_HardwareVerDisplay;
private: System::Windows::Forms::Label^  label_Glb_HardwareVer;
private: System::Windows::Forms::Label^  label_Glb_ModelNum;
private: System::Windows::Forms::Label^  label_Bat_SecPrecis;
private: System::Windows::Forms::TabPage^  tabPage_Calibration;
private: System::Windows::Forms::TabPage^  tabPage_Data;
private: System::Windows::Forms::Button^  button_Cal_Calibrate;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Cal_WheelCircum;
private: System::Windows::Forms::Label^  label_Cal_WheelCircum;
private: System::Windows::Forms::Label^  label_Dat_Coasting;
private: System::Windows::Forms::Label^  label_Cal_CalNumberDisplay;
private: System::Windows::Forms::Label^  label_Bat_VoltsDisplay;
private: System::Windows::Forms::Label^  label_Dat_PageRecDisplay;
private: System::Windows::Forms::Label^  label_Dat_CalcPower;
private: System::Windows::Forms::Label^  label_Dat_EventCount;
private: System::Windows::Forms::Label^  label_Dat_UpdateCount;
private: System::Windows::Forms::Label^  label_Dat_InstCadence;
private: System::Windows::Forms::Label^  label_Dat_CalcRPM;
private: System::Windows::Forms::Label^  label_Dat_EventCountDisplay;
private: System::Windows::Forms::Label^  label_Dat_UpdateCountDisplay;
private: System::Windows::Forms::Label^  label_Dat_Dist;
private: System::Windows::Forms::Label^  label_Dat_Speed;
private: System::Windows::Forms::Label^  label_Dat_Torque;
private: System::Windows::Forms::Label^  label_Dat_AngVel;
private: System::Windows::Forms::Label^  label_Dat_DistDisplay;
private: System::Windows::Forms::Label^  label_Dat_SpeedDisplay;
private: System::Windows::Forms::Label^  label_Dat_CalcRPMDisplay;
private: System::Windows::Forms::Label^  label_Dat_CalcPowerDisplay;
private: System::Windows::Forms::Label^  label_Dat_TorqueDisplay;
private: System::Windows::Forms::Label^  label_Dat_AngVelDisplay;
private: System::Windows::Forms::Label^  label_Dat_InstCadenceDisplay;
private: System::Windows::Forms::Label^  label_Dat_InstPower;
private: System::Windows::Forms::Label^  label_Dat_InstPowerDisplay;
private: System::Windows::Forms::Label^  label_Trn_AccumPowerDisplay;
private: System::Windows::Forms::Label^  label_Trn_AccumPower;
private: System::Windows::Forms::Label^  label_InstPower;
private: System::Windows::Forms::Label^  label_Trn_InstPowerDisplay;
private: System::Windows::Forms::Label^  label_Cal_RawTorqueDisplay;
private: System::Windows::Forms::Label^  label_Cal_OffsetTorqueDisplay;
private: System::Windows::Forms::Label^  label_Trn_Slope;
private: System::Windows::Forms::Label^  label_Trn_SlopeDisplay;
private: System::Windows::Forms::Label^  label_Dat_Slope;
private: System::Windows::Forms::Label^  label_Dat_SlopeDisplay;
private: System::Windows::Forms::RadioButton^  radioButton_Cal_AZoff;
private: System::Windows::Forms::Label^  label_Dat_CTFOffsetDisplay;
private: System::Windows::Forms::Label^  label_Dat_CTFOffset;
private: System::Windows::Forms::RadioButton^  radioButton_Cal_AZon;
private: System::Windows::Forms::Button^  button_Cal_AutoZeroSet;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Cal_CTFSerial;
private: System::Windows::Forms::Button^  button_Cal_CTFSlopeSet;
private: System::Windows::Forms::Button^  button_Cal_CTFSerialSet;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Cal_CTFSlope;
private: System::Windows::Forms::Label^  label_Cal_SetLabel;
private: System::Windows::Forms::Panel^  panel1;
private: System::Windows::Forms::Label^  label_Cal_Status;
private: System::Windows::Forms::Label^  label_Cal_StatusDisplay;
private: System::Windows::Forms::Label^  label_Trn_PedalPwr;
private: System::Windows::Forms::Label^  label_Trn_PedalDisplay;
private: System::Windows::Forms::Label^  label_Trn_PedalPwrDisplay;
private: System::Windows::Forms::Label^  label_Dat_PedalDisplay;
private: System::Windows::Forms::Label^  label_Dat_PedalPwrDisplay;
private: System::Windows::Forms::Label^  label_Dat_PedalPwr;
private: System::Windows::Forms::TabPage^  tabPage1;
private: System::Windows::Forms::Button^  button_SetCrankStatus;
private: System::Windows::Forms::Button^  button_GetCrankStatus;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_CrankLength;
private: System::Windows::Forms::Label^  label1;
private: System::Windows::Forms::CheckBox^  checkBox_AutoCrank;
private: System::Windows::Forms::CheckBox^  checkBox_InvalidCrankLength;
private: System::Windows::Forms::Label^  label_CrankLengthStatus;
private: System::Windows::Forms::Label^  label_CrankLength;
private: System::Windows::Forms::Label^  label_SensorStatus;
private: System::Windows::Forms::Label^  label6;
private: System::Windows::Forms::Label^  label5;
private: System::Windows::Forms::Label^  label4;
private: System::Windows::Forms::Label^  label_SWMistmatchStatus;
private: System::Windows::Forms::Label^  label2;
private: System::Windows::Forms::Label^  label_AutoCrankLength;
private: System::Windows::Forms::Label^  label10;
private: System::Windows::Forms::Label^  label3;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_TransResponse;
private: System::Windows::Forms::Label^  label7;
private: System::Windows::Forms::TabPage^  tabControl_TEPS;
private: System::Windows::Forms::Label^  label11;
private: System::Windows::Forms::Label^  label12;
private: System::Windows::Forms::Label^  label9;
private: System::Windows::Forms::Label^  label8;
private: System::Windows::Forms::Label^  label_RightPedalSmoothness;
private: System::Windows::Forms::Label^  label_LeftPedalSmoothness;
private: System::Windows::Forms::Label^  label_RightTorqueEffectiveness;
private: System::Windows::Forms::Label^  label_LeftTorqueEffectiveness;

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
      this->tabPage_Data = (gcnew System::Windows::Forms::TabPage());
      this->label_Dat_PedalDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Dat_PedalPwrDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Dat_PedalPwr = (gcnew System::Windows::Forms::Label());
      this->label_Dat_CTFOffsetDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Dat_CTFOffset = (gcnew System::Windows::Forms::Label());
      this->label_Dat_SlopeDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Dat_Slope = (gcnew System::Windows::Forms::Label());
      this->label_Dat_InstCadenceDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Dat_InstCadence = (gcnew System::Windows::Forms::Label());
      this->label_Dat_DistDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Dat_Dist = (gcnew System::Windows::Forms::Label());
      this->label_Dat_SpeedDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Dat_Speed = (gcnew System::Windows::Forms::Label());
      this->label_Dat_CalcRPMDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Dat_CalcRPM = (gcnew System::Windows::Forms::Label());
      this->label_Dat_InstPowerDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Dat_InstPower = (gcnew System::Windows::Forms::Label());
      this->label_Dat_CalcPowerDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Dat_EventCountDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Dat_UpdateCountDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Dat_CalcPower = (gcnew System::Windows::Forms::Label());
      this->label_Dat_EventCount = (gcnew System::Windows::Forms::Label());
      this->label_Dat_UpdateCount = (gcnew System::Windows::Forms::Label());
      this->label_Dat_Coasting = (gcnew System::Windows::Forms::Label());
      this->label_Dat_Stopped = (gcnew System::Windows::Forms::Label());
      this->label_Dat_PageRec = (gcnew System::Windows::Forms::Label());
      this->label_Dat_TorqueDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Dat_Torque = (gcnew System::Windows::Forms::Label());
      this->label_Dat_AngVelDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Dat_AngVel = (gcnew System::Windows::Forms::Label());
      this->label_Dat_PageRecDisplay = (gcnew System::Windows::Forms::Label());
      this->tabPage_Calibration = (gcnew System::Windows::Forms::TabPage());
      this->label_Cal_StatusDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Cal_Status = (gcnew System::Windows::Forms::Label());
      this->panel1 = (gcnew System::Windows::Forms::Panel());
      this->button_Cal_CTFSlopeSet = (gcnew System::Windows::Forms::Button());
      this->radioButton_Cal_AZoff = (gcnew System::Windows::Forms::RadioButton());
      this->button_Cal_AutoZeroSet = (gcnew System::Windows::Forms::Button());
      this->numericUpDown_Cal_CTFSlope = (gcnew System::Windows::Forms::NumericUpDown());
      this->radioButton_Cal_AZon = (gcnew System::Windows::Forms::RadioButton());
      this->button_Cal_CTFSerialSet = (gcnew System::Windows::Forms::Button());
      this->numericUpDown_Cal_CTFSerial = (gcnew System::Windows::Forms::NumericUpDown());
      this->label_Cal_RawTorqueDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Cal_OffsetTorqueDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Cal_CalNumberDisplay = (gcnew System::Windows::Forms::Label());
      this->numericUpDown_Cal_WheelCircum = (gcnew System::Windows::Forms::NumericUpDown());
      this->button_Cal_Calibrate = (gcnew System::Windows::Forms::Button());
      this->label_Cal_WheelCircum = (gcnew System::Windows::Forms::Label());
      this->label_Cal_TorqOffset = (gcnew System::Windows::Forms::Label());
      this->label_Cal_AZStatus = (gcnew System::Windows::Forms::Label());
      this->listBox_Cal_AZStatus = (gcnew System::Windows::Forms::ListBox());
      this->label_Cal_CalNum = (gcnew System::Windows::Forms::Label());
      this->label_Cal_RawTorq = (gcnew System::Windows::Forms::Label());
      this->label_Cal_SetLabel = (gcnew System::Windows::Forms::Label());
      this->tabPage_GlobalBattery = (gcnew System::Windows::Forms::TabPage());
      this->label_Bat_VoltsDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Glb_SoftwareVer = (gcnew System::Windows::Forms::Label());
      this->label_Bat_SecPrecis = (gcnew System::Windows::Forms::Label());
      this->label_Bat_Volts = (gcnew System::Windows::Forms::Label());
      this->label_Glb_HardwareVer = (gcnew System::Windows::Forms::Label());
      this->label_Bat_Status = (gcnew System::Windows::Forms::Label());
      this->listBox_Bat_Status = (gcnew System::Windows::Forms::ListBox());
      this->label_Glb_SoftwareVerDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Bat_ElpTimeDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Bat_ElpTime = (gcnew System::Windows::Forms::Label());
      this->label_Glb_SerialNumDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Glb_ModelNum = (gcnew System::Windows::Forms::Label());
      this->label_Glb_ManfIDDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Glb_SerialNum = (gcnew System::Windows::Forms::Label());
      this->label_Glb_ModelNumDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Glb_HardwareVerDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Glb_ManfID = (gcnew System::Windows::Forms::Label());
      this->tabPage1 = (gcnew System::Windows::Forms::TabPage());
      this->label7 = (gcnew System::Windows::Forms::Label());
      this->label3 = (gcnew System::Windows::Forms::Label());
      this->numericUpDown_TransResponse = (gcnew System::Windows::Forms::NumericUpDown());
      this->label_AutoCrankLength = (gcnew System::Windows::Forms::Label());
      this->label10 = (gcnew System::Windows::Forms::Label());
      this->label_CrankLengthStatus = (gcnew System::Windows::Forms::Label());
      this->label_CrankLength = (gcnew System::Windows::Forms::Label());
      this->label_SensorStatus = (gcnew System::Windows::Forms::Label());
      this->label6 = (gcnew System::Windows::Forms::Label());
      this->label5 = (gcnew System::Windows::Forms::Label());
      this->label4 = (gcnew System::Windows::Forms::Label());
      this->label_SWMistmatchStatus = (gcnew System::Windows::Forms::Label());
      this->label2 = (gcnew System::Windows::Forms::Label());
      this->checkBox_AutoCrank = (gcnew System::Windows::Forms::CheckBox());
      this->checkBox_InvalidCrankLength = (gcnew System::Windows::Forms::CheckBox());
      this->label1 = (gcnew System::Windows::Forms::Label());
      this->numericUpDown_CrankLength = (gcnew System::Windows::Forms::NumericUpDown());
      this->button_SetCrankStatus = (gcnew System::Windows::Forms::Button());
      this->button_GetCrankStatus = (gcnew System::Windows::Forms::Button());
      this->panel_Display = (gcnew System::Windows::Forms::Panel());
      this->label_Trn_PedalDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Trn_PedalPwrDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Trn_PedalPwr = (gcnew System::Windows::Forms::Label());
      this->label_Trn_AccumPowerDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Trn_AccumPower = (gcnew System::Windows::Forms::Label());
      this->label_Trn_AccumTwo = (gcnew System::Windows::Forms::Label());
      this->label_Trn_AccumOneDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Trn_AccumOne = (gcnew System::Windows::Forms::Label());
      this->label_Trn_AccumTwoDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Trn_AccumTitle = (gcnew System::Windows::Forms::Label());
      this->label_Trn_Slope = (gcnew System::Windows::Forms::Label());
      this->label_Trn_SlopeDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Trn_UpdateCountDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Trn_UpdateCount = (gcnew System::Windows::Forms::Label());
      this->label_Trn_EventCountDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Trn_EventCount = (gcnew System::Windows::Forms::Label());
      this->label_Trn_InstPowerDisplay = (gcnew System::Windows::Forms::Label());
      this->label_InstPower = (gcnew System::Windows::Forms::Label());
      this->label_Trn_CadenceDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Trn_Cadence = (gcnew System::Windows::Forms::Label());
      this->label_Trn_PageDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Trn_Page = (gcnew System::Windows::Forms::Label());
      this->label_Trn_DisplayTitle = (gcnew System::Windows::Forms::Label());
      this->tabControl_TEPS = (gcnew System::Windows::Forms::TabPage());
      this->label8 = (gcnew System::Windows::Forms::Label());
      this->label9 = (gcnew System::Windows::Forms::Label());
      this->label11 = (gcnew System::Windows::Forms::Label());
      this->label12 = (gcnew System::Windows::Forms::Label());
      this->label_LeftTorqueEffectiveness = (gcnew System::Windows::Forms::Label());
      this->label_RightTorqueEffectiveness = (gcnew System::Windows::Forms::Label());
      this->label_LeftPedalSmoothness = (gcnew System::Windows::Forms::Label());
      this->label_RightPedalSmoothness = (gcnew System::Windows::Forms::Label());
      this->panel_Settings->SuspendLayout();
      this->tabControl_Settings->SuspendLayout();
      this->tabPage_Data->SuspendLayout();
      this->tabPage_Calibration->SuspendLayout();
      this->panel1->SuspendLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Cal_CTFSlope))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Cal_CTFSerial))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Cal_WheelCircum))->BeginInit();
      this->tabPage_GlobalBattery->SuspendLayout();
      this->tabPage1->SuspendLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_TransResponse))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_CrankLength))->BeginInit();
      this->panel_Display->SuspendLayout();
      this->tabControl_TEPS->SuspendLayout();
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
      this->tabControl_Settings->Controls->Add(this->tabPage_Data);
      this->tabControl_Settings->Controls->Add(this->tabPage_Calibration);
      this->tabControl_Settings->Controls->Add(this->tabPage_GlobalBattery);
      this->tabControl_Settings->Controls->Add(this->tabPage1);
      this->tabControl_Settings->Controls->Add(this->tabControl_TEPS);
      this->tabControl_Settings->Location = System::Drawing::Point(0, 3);
      this->tabControl_Settings->Name = L"tabControl_Settings";
      this->tabControl_Settings->SelectedIndex = 0;
      this->tabControl_Settings->Size = System::Drawing::Size(397, 137);
      this->tabControl_Settings->TabIndex = 10;
      // 
      // tabPage_Data
      // 
      this->tabPage_Data->Controls->Add(this->label_Dat_PedalDisplay);
      this->tabPage_Data->Controls->Add(this->label_Dat_PedalPwrDisplay);
      this->tabPage_Data->Controls->Add(this->label_Dat_PedalPwr);
      this->tabPage_Data->Controls->Add(this->label_Dat_CTFOffsetDisplay);
      this->tabPage_Data->Controls->Add(this->label_Dat_CTFOffset);
      this->tabPage_Data->Controls->Add(this->label_Dat_SlopeDisplay);
      this->tabPage_Data->Controls->Add(this->label_Dat_Slope);
      this->tabPage_Data->Controls->Add(this->label_Dat_InstCadenceDisplay);
      this->tabPage_Data->Controls->Add(this->label_Dat_InstCadence);
      this->tabPage_Data->Controls->Add(this->label_Dat_DistDisplay);
      this->tabPage_Data->Controls->Add(this->label_Dat_Dist);
      this->tabPage_Data->Controls->Add(this->label_Dat_SpeedDisplay);
      this->tabPage_Data->Controls->Add(this->label_Dat_Speed);
      this->tabPage_Data->Controls->Add(this->label_Dat_CalcRPMDisplay);
      this->tabPage_Data->Controls->Add(this->label_Dat_CalcRPM);
      this->tabPage_Data->Controls->Add(this->label_Dat_InstPowerDisplay);
      this->tabPage_Data->Controls->Add(this->label_Dat_InstPower);
      this->tabPage_Data->Controls->Add(this->label_Dat_CalcPowerDisplay);
      this->tabPage_Data->Controls->Add(this->label_Dat_EventCountDisplay);
      this->tabPage_Data->Controls->Add(this->label_Dat_UpdateCountDisplay);
      this->tabPage_Data->Controls->Add(this->label_Dat_CalcPower);
      this->tabPage_Data->Controls->Add(this->label_Dat_EventCount);
      this->tabPage_Data->Controls->Add(this->label_Dat_UpdateCount);
      this->tabPage_Data->Controls->Add(this->label_Dat_Coasting);
      this->tabPage_Data->Controls->Add(this->label_Dat_Stopped);
      this->tabPage_Data->Controls->Add(this->label_Dat_PageRec);
      this->tabPage_Data->Controls->Add(this->label_Dat_TorqueDisplay);
      this->tabPage_Data->Controls->Add(this->label_Dat_Torque);
      this->tabPage_Data->Controls->Add(this->label_Dat_AngVelDisplay);
      this->tabPage_Data->Controls->Add(this->label_Dat_AngVel);
      this->tabPage_Data->Controls->Add(this->label_Dat_PageRecDisplay);
      this->tabPage_Data->Location = System::Drawing::Point(4, 22);
      this->tabPage_Data->Name = L"tabPage_Data";
      this->tabPage_Data->Size = System::Drawing::Size(389, 111);
      this->tabPage_Data->TabIndex = 6;
      this->tabPage_Data->Text = L"Data";
      this->tabPage_Data->UseVisualStyleBackColor = true;
      // 
      // label_Dat_PedalDisplay
      // 
      this->label_Dat_PedalDisplay->AutoSize = true;
      this->label_Dat_PedalDisplay->Location = System::Drawing::Point(139, 17);
      this->label_Dat_PedalDisplay->Name = L"label_Dat_PedalDisplay";
      this->label_Dat_PedalDisplay->Size = System::Drawing::Size(16, 13);
      this->label_Dat_PedalDisplay->TabIndex = 55;
      this->label_Dat_PedalDisplay->Text = L"---";
      // 
      // label_Dat_PedalPwrDisplay
      // 
      this->label_Dat_PedalPwrDisplay->AutoSize = true;
      this->label_Dat_PedalPwrDisplay->Location = System::Drawing::Point(110, 17);
      this->label_Dat_PedalPwrDisplay->Name = L"label_Dat_PedalPwrDisplay";
      this->label_Dat_PedalPwrDisplay->Size = System::Drawing::Size(16, 13);
      this->label_Dat_PedalPwrDisplay->TabIndex = 54;
      this->label_Dat_PedalPwrDisplay->Text = L"---";
      // 
      // label_Dat_PedalPwr
      // 
      this->label_Dat_PedalPwr->AutoSize = true;
      this->label_Dat_PedalPwr->Location = System::Drawing::Point(27, 17);
      this->label_Dat_PedalPwr->Name = L"label_Dat_PedalPwr";
      this->label_Dat_PedalPwr->Size = System::Drawing::Size(84, 13);
      this->label_Dat_PedalPwr->TabIndex = 53;
      this->label_Dat_PedalPwr->Text = L"Pedal Power(%):";
      // 
      // label_Dat_CTFOffsetDisplay
      // 
      this->label_Dat_CTFOffsetDisplay->AutoSize = true;
      this->label_Dat_CTFOffsetDisplay->Location = System::Drawing::Point(111, 98);
      this->label_Dat_CTFOffsetDisplay->Name = L"label_Dat_CTFOffsetDisplay";
      this->label_Dat_CTFOffsetDisplay->Size = System::Drawing::Size(16, 13);
      this->label_Dat_CTFOffsetDisplay->TabIndex = 52;
      this->label_Dat_CTFOffsetDisplay->Text = L"---";
      // 
      // label_Dat_CTFOffset
      // 
      this->label_Dat_CTFOffset->AutoSize = true;
      this->label_Dat_CTFOffset->Location = System::Drawing::Point(44, 97);
      this->label_Dat_CTFOffset->Name = L"label_Dat_CTFOffset";
      this->label_Dat_CTFOffset->Size = System::Drawing::Size(61, 13);
      this->label_Dat_CTFOffset->TabIndex = 51;
      this->label_Dat_CTFOffset->Text = L"CTF Offset:";
      // 
      // label_Dat_SlopeDisplay
      // 
      this->label_Dat_SlopeDisplay->AutoSize = true;
      this->label_Dat_SlopeDisplay->Location = System::Drawing::Point(111, 85);
      this->label_Dat_SlopeDisplay->Name = L"label_Dat_SlopeDisplay";
      this->label_Dat_SlopeDisplay->Size = System::Drawing::Size(16, 13);
      this->label_Dat_SlopeDisplay->TabIndex = 50;
      this->label_Dat_SlopeDisplay->Text = L"---";
      // 
      // label_Dat_Slope
      // 
      this->label_Dat_Slope->AutoSize = true;
      this->label_Dat_Slope->Location = System::Drawing::Point(25, 84);
      this->label_Dat_Slope->Name = L"label_Dat_Slope";
      this->label_Dat_Slope->Size = System::Drawing::Size(80, 13);
      this->label_Dat_Slope->TabIndex = 49;
      this->label_Dat_Slope->Text = L"Slope (Nm/Hz):";
      // 
      // label_Dat_InstCadenceDisplay
      // 
      this->label_Dat_InstCadenceDisplay->AutoSize = true;
      this->label_Dat_InstCadenceDisplay->Location = System::Drawing::Point(111, 72);
      this->label_Dat_InstCadenceDisplay->Name = L"label_Dat_InstCadenceDisplay";
      this->label_Dat_InstCadenceDisplay->Size = System::Drawing::Size(16, 13);
      this->label_Dat_InstCadenceDisplay->TabIndex = 36;
      this->label_Dat_InstCadenceDisplay->Text = L"---";
      // 
      // label_Dat_InstCadence
      // 
      this->label_Dat_InstCadence->AutoSize = true;
      this->label_Dat_InstCadence->Location = System::Drawing::Point(29, 71);
      this->label_Dat_InstCadence->Name = L"label_Dat_InstCadence";
      this->label_Dat_InstCadence->Size = System::Drawing::Size(76, 13);
      this->label_Dat_InstCadence->TabIndex = 26;
      this->label_Dat_InstCadence->Text = L"Inst. Cadence:";
      // 
      // label_Dat_DistDisplay
      // 
      this->label_Dat_DistDisplay->AutoSize = true;
      this->label_Dat_DistDisplay->Location = System::Drawing::Point(288, 92);
      this->label_Dat_DistDisplay->Name = L"label_Dat_DistDisplay";
      this->label_Dat_DistDisplay->Size = System::Drawing::Size(16, 13);
      this->label_Dat_DistDisplay->TabIndex = 44;
      this->label_Dat_DistDisplay->Text = L"---";
      // 
      // label_Dat_Dist
      // 
      this->label_Dat_Dist->AutoSize = true;
      this->label_Dat_Dist->Location = System::Drawing::Point(230, 92);
      this->label_Dat_Dist->Name = L"label_Dat_Dist";
      this->label_Dat_Dist->Size = System::Drawing::Size(52, 13);
      this->label_Dat_Dist->TabIndex = 33;
      this->label_Dat_Dist->Text = L"Distance:";
      // 
      // label_Dat_SpeedDisplay
      // 
      this->label_Dat_SpeedDisplay->AutoSize = true;
      this->label_Dat_SpeedDisplay->Location = System::Drawing::Point(288, 79);
      this->label_Dat_SpeedDisplay->Name = L"label_Dat_SpeedDisplay";
      this->label_Dat_SpeedDisplay->Size = System::Drawing::Size(16, 13);
      this->label_Dat_SpeedDisplay->TabIndex = 43;
      this->label_Dat_SpeedDisplay->Text = L"---";
      // 
      // label_Dat_Speed
      // 
      this->label_Dat_Speed->AutoSize = true;
      this->label_Dat_Speed->Location = System::Drawing::Point(214, 79);
      this->label_Dat_Speed->Name = L"label_Dat_Speed";
      this->label_Dat_Speed->Size = System::Drawing::Size(68, 13);
      this->label_Dat_Speed->TabIndex = 32;
      this->label_Dat_Speed->Text = L"Speed (kph):";
      // 
      // label_Dat_CalcRPMDisplay
      // 
      this->label_Dat_CalcRPMDisplay->AutoSize = true;
      this->label_Dat_CalcRPMDisplay->Location = System::Drawing::Point(288, 66);
      this->label_Dat_CalcRPMDisplay->Name = L"label_Dat_CalcRPMDisplay";
      this->label_Dat_CalcRPMDisplay->Size = System::Drawing::Size(16, 13);
      this->label_Dat_CalcRPMDisplay->TabIndex = 42;
      this->label_Dat_CalcRPMDisplay->Text = L"---";
      // 
      // label_Dat_CalcRPM
      // 
      this->label_Dat_CalcRPM->AutoSize = true;
      this->label_Dat_CalcRPM->Location = System::Drawing::Point(176, 66);
      this->label_Dat_CalcRPM->Name = L"label_Dat_CalcRPM";
      this->label_Dat_CalcRPM->Size = System::Drawing::Size(105, 13);
      this->label_Dat_CalcRPM->TabIndex = 29;
      this->label_Dat_CalcRPM->Text = L"Calc. RPM@Sensor:";
      // 
      // label_Dat_InstPowerDisplay
      // 
      this->label_Dat_InstPowerDisplay->AutoSize = true;
      this->label_Dat_InstPowerDisplay->Location = System::Drawing::Point(111, 59);
      this->label_Dat_InstPowerDisplay->Name = L"label_Dat_InstPowerDisplay";
      this->label_Dat_InstPowerDisplay->Size = System::Drawing::Size(16, 13);
      this->label_Dat_InstPowerDisplay->TabIndex = 46;
      this->label_Dat_InstPowerDisplay->Text = L"---";
      // 
      // label_Dat_InstPower
      // 
      this->label_Dat_InstPower->AutoSize = true;
      this->label_Dat_InstPower->Location = System::Drawing::Point(42, 58);
      this->label_Dat_InstPower->Name = L"label_Dat_InstPower";
      this->label_Dat_InstPower->Size = System::Drawing::Size(63, 13);
      this->label_Dat_InstPower->TabIndex = 45;
      this->label_Dat_InstPower->Text = L"Inst. Power:";
      // 
      // label_Dat_CalcPowerDisplay
      // 
      this->label_Dat_CalcPowerDisplay->AutoSize = true;
      this->label_Dat_CalcPowerDisplay->Location = System::Drawing::Point(288, 46);
      this->label_Dat_CalcPowerDisplay->Name = L"label_Dat_CalcPowerDisplay";
      this->label_Dat_CalcPowerDisplay->Size = System::Drawing::Size(16, 13);
      this->label_Dat_CalcPowerDisplay->TabIndex = 41;
      this->label_Dat_CalcPowerDisplay->Text = L"---";
      // 
      // label_Dat_EventCountDisplay
      // 
      this->label_Dat_EventCountDisplay->AutoSize = true;
      this->label_Dat_EventCountDisplay->Location = System::Drawing::Point(111, 46);
      this->label_Dat_EventCountDisplay->Name = L"label_Dat_EventCountDisplay";
      this->label_Dat_EventCountDisplay->Size = System::Drawing::Size(16, 13);
      this->label_Dat_EventCountDisplay->TabIndex = 35;
      this->label_Dat_EventCountDisplay->Text = L"---";
      // 
      // label_Dat_UpdateCountDisplay
      // 
      this->label_Dat_UpdateCountDisplay->AutoSize = true;
      this->label_Dat_UpdateCountDisplay->Location = System::Drawing::Point(111, 33);
      this->label_Dat_UpdateCountDisplay->Name = L"label_Dat_UpdateCountDisplay";
      this->label_Dat_UpdateCountDisplay->Size = System::Drawing::Size(16, 13);
      this->label_Dat_UpdateCountDisplay->TabIndex = 34;
      this->label_Dat_UpdateCountDisplay->Text = L"---";
      // 
      // label_Dat_CalcPower
      // 
      this->label_Dat_CalcPower->AutoSize = true;
      this->label_Dat_CalcPower->Location = System::Drawing::Point(189, 46);
      this->label_Dat_CalcPower->Name = L"label_Dat_CalcPower";
      this->label_Dat_CalcPower->Size = System::Drawing::Size(93, 13);
      this->label_Dat_CalcPower->TabIndex = 25;
      this->label_Dat_CalcPower->Text = L"Calculated Power:";
      // 
      // label_Dat_EventCount
      // 
      this->label_Dat_EventCount->AutoSize = true;
      this->label_Dat_EventCount->Location = System::Drawing::Point(36, 45);
      this->label_Dat_EventCount->Name = L"label_Dat_EventCount";
      this->label_Dat_EventCount->Size = System::Drawing::Size(69, 13);
      this->label_Dat_EventCount->TabIndex = 24;
      this->label_Dat_EventCount->Text = L"Event Count:";
      // 
      // label_Dat_UpdateCount
      // 
      this->label_Dat_UpdateCount->AutoSize = true;
      this->label_Dat_UpdateCount->Location = System::Drawing::Point(29, 32);
      this->label_Dat_UpdateCount->Name = L"label_Dat_UpdateCount";
      this->label_Dat_UpdateCount->Size = System::Drawing::Size(76, 13);
      this->label_Dat_UpdateCount->TabIndex = 23;
      this->label_Dat_UpdateCount->Text = L"Update Count:";
      // 
      // label_Dat_Coasting
      // 
      this->label_Dat_Coasting->AutoSize = true;
      this->label_Dat_Coasting->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
         static_cast<System::Byte>(0)));
      this->label_Dat_Coasting->ForeColor = System::Drawing::Color::Red;
      this->label_Dat_Coasting->Location = System::Drawing::Point(329, 30);
      this->label_Dat_Coasting->Name = L"label_Dat_Coasting";
      this->label_Dat_Coasting->Size = System::Drawing::Size(56, 13);
      this->label_Dat_Coasting->TabIndex = 21;
      this->label_Dat_Coasting->Text = L"Coasting";
      // 
      // label_Dat_Stopped
      // 
      this->label_Dat_Stopped->AutoSize = true;
      this->label_Dat_Stopped->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
         static_cast<System::Byte>(0)));
      this->label_Dat_Stopped->ForeColor = System::Drawing::Color::Red;
      this->label_Dat_Stopped->Location = System::Drawing::Point(329, 82);
      this->label_Dat_Stopped->Name = L"label_Dat_Stopped";
      this->label_Dat_Stopped->Size = System::Drawing::Size(54, 13);
      this->label_Dat_Stopped->TabIndex = 19;
      this->label_Dat_Stopped->Text = L"Stopped";
      // 
      // label_Dat_PageRec
      // 
      this->label_Dat_PageRec->AutoSize = true;
      this->label_Dat_PageRec->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
         static_cast<System::Byte>(0)));
      this->label_Dat_PageRec->Location = System::Drawing::Point(4, 3);
      this->label_Dat_PageRec->Name = L"label_Dat_PageRec";
      this->label_Dat_PageRec->Size = System::Drawing::Size(82, 13);
      this->label_Dat_PageRec->TabIndex = 20;
      this->label_Dat_PageRec->Text = L"Sensor Type:";
      // 
      // label_Dat_TorqueDisplay
      // 
      this->label_Dat_TorqueDisplay->AutoSize = true;
      this->label_Dat_TorqueDisplay->Location = System::Drawing::Point(288, 33);
      this->label_Dat_TorqueDisplay->Name = L"label_Dat_TorqueDisplay";
      this->label_Dat_TorqueDisplay->Size = System::Drawing::Size(16, 13);
      this->label_Dat_TorqueDisplay->TabIndex = 40;
      this->label_Dat_TorqueDisplay->Text = L"---";
      // 
      // label_Dat_Torque
      // 
      this->label_Dat_Torque->AutoSize = true;
      this->label_Dat_Torque->Location = System::Drawing::Point(238, 33);
      this->label_Dat_Torque->Name = L"label_Dat_Torque";
      this->label_Dat_Torque->Size = System::Drawing::Size(44, 13);
      this->label_Dat_Torque->TabIndex = 31;
      this->label_Dat_Torque->Text = L"Torque:";
      // 
      // label_Dat_AngVelDisplay
      // 
      this->label_Dat_AngVelDisplay->AutoSize = true;
      this->label_Dat_AngVelDisplay->Location = System::Drawing::Point(288, 20);
      this->label_Dat_AngVelDisplay->Name = L"label_Dat_AngVelDisplay";
      this->label_Dat_AngVelDisplay->Size = System::Drawing::Size(16, 13);
      this->label_Dat_AngVelDisplay->TabIndex = 39;
      this->label_Dat_AngVelDisplay->Text = L"---";
      // 
      // label_Dat_AngVel
      // 
      this->label_Dat_AngVel->AutoSize = true;
      this->label_Dat_AngVel->Location = System::Drawing::Point(196, 20);
      this->label_Dat_AngVel->Name = L"label_Dat_AngVel";
      this->label_Dat_AngVel->Size = System::Drawing::Size(86, 13);
      this->label_Dat_AngVel->TabIndex = 30;
      this->label_Dat_AngVel->Text = L"Angular Velocity:";
      // 
      // label_Dat_PageRecDisplay
      // 
      this->label_Dat_PageRecDisplay->AutoSize = true;
      this->label_Dat_PageRecDisplay->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, 
         System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
      this->label_Dat_PageRecDisplay->Location = System::Drawing::Point(104, 3);
      this->label_Dat_PageRecDisplay->Name = L"label_Dat_PageRecDisplay";
      this->label_Dat_PageRecDisplay->Size = System::Drawing::Size(19, 13);
      this->label_Dat_PageRecDisplay->TabIndex = 22;
      this->label_Dat_PageRecDisplay->Text = L"---";
      // 
      // tabPage_Calibration
      // 
      this->tabPage_Calibration->Controls->Add(this->label_Cal_StatusDisplay);
      this->tabPage_Calibration->Controls->Add(this->label_Cal_Status);
      this->tabPage_Calibration->Controls->Add(this->panel1);
      this->tabPage_Calibration->Controls->Add(this->label_Cal_RawTorqueDisplay);
      this->tabPage_Calibration->Controls->Add(this->label_Cal_OffsetTorqueDisplay);
      this->tabPage_Calibration->Controls->Add(this->label_Cal_CalNumberDisplay);
      this->tabPage_Calibration->Controls->Add(this->numericUpDown_Cal_WheelCircum);
      this->tabPage_Calibration->Controls->Add(this->button_Cal_Calibrate);
      this->tabPage_Calibration->Controls->Add(this->label_Cal_WheelCircum);
      this->tabPage_Calibration->Controls->Add(this->label_Cal_TorqOffset);
      this->tabPage_Calibration->Controls->Add(this->label_Cal_AZStatus);
      this->tabPage_Calibration->Controls->Add(this->listBox_Cal_AZStatus);
      this->tabPage_Calibration->Controls->Add(this->label_Cal_CalNum);
      this->tabPage_Calibration->Controls->Add(this->label_Cal_RawTorq);
      this->tabPage_Calibration->Controls->Add(this->label_Cal_SetLabel);
      this->tabPage_Calibration->Location = System::Drawing::Point(4, 22);
      this->tabPage_Calibration->Name = L"tabPage_Calibration";
      this->tabPage_Calibration->Size = System::Drawing::Size(389, 111);
      this->tabPage_Calibration->TabIndex = 5;
      this->tabPage_Calibration->Text = L"Calibration";
      this->tabPage_Calibration->UseVisualStyleBackColor = true;
      // 
      // label_Cal_StatusDisplay
      // 
      this->label_Cal_StatusDisplay->AutoSize = true;
      this->label_Cal_StatusDisplay->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, 
         System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
      this->label_Cal_StatusDisplay->Location = System::Drawing::Point(4, 91);
      this->label_Cal_StatusDisplay->Name = L"label_Cal_StatusDisplay";
      this->label_Cal_StatusDisplay->Size = System::Drawing::Size(35, 13);
      this->label_Cal_StatusDisplay->TabIndex = 70;
      this->label_Cal_StatusDisplay->Text = L"none";
      // 
      // label_Cal_Status
      // 
      this->label_Cal_Status->AutoSize = true;
      this->label_Cal_Status->Location = System::Drawing::Point(4, 78);
      this->label_Cal_Status->Name = L"label_Cal_Status";
      this->label_Cal_Status->Size = System::Drawing::Size(37, 13);
      this->label_Cal_Status->TabIndex = 71;
      this->label_Cal_Status->Text = L"Status";
      // 
      // panel1
      // 
      this->panel1->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
      this->panel1->Controls->Add(this->button_Cal_CTFSlopeSet);
      this->panel1->Controls->Add(this->radioButton_Cal_AZoff);
      this->panel1->Controls->Add(this->button_Cal_AutoZeroSet);
      this->panel1->Controls->Add(this->numericUpDown_Cal_CTFSlope);
      this->panel1->Controls->Add(this->radioButton_Cal_AZon);
      this->panel1->Controls->Add(this->button_Cal_CTFSerialSet);
      this->panel1->Controls->Add(this->numericUpDown_Cal_CTFSerial);
      this->panel1->Location = System::Drawing::Point(49, 32);
      this->panel1->Margin = System::Windows::Forms::Padding(1);
      this->panel1->Name = L"panel1";
      this->panel1->Size = System::Drawing::Size(184, 56);
      this->panel1->TabIndex = 69;
      // 
      // button_Cal_CTFSlopeSet
      // 
      this->button_Cal_CTFSlopeSet->Enabled = false;
      this->button_Cal_CTFSlopeSet->Location = System::Drawing::Point(129, 1);
      this->button_Cal_CTFSlopeSet->Name = L"button_Cal_CTFSlopeSet";
      this->button_Cal_CTFSlopeSet->Size = System::Drawing::Size(48, 23);
      this->button_Cal_CTFSlopeSet->TabIndex = 67;
      this->button_Cal_CTFSlopeSet->Text = L"Slope";
      this->button_Cal_CTFSlopeSet->UseVisualStyleBackColor = true;
      this->button_Cal_CTFSlopeSet->Click += gcnew System::EventHandler(this, &BikePowerDisplay::button_Cal_CTFSlopeSet_Click);
      // 
      // radioButton_Cal_AZoff
      // 
      this->radioButton_Cal_AZoff->AutoSize = true;
      this->radioButton_Cal_AZoff->Enabled = false;
      this->radioButton_Cal_AZoff->Location = System::Drawing::Point(14, 38);
      this->radioButton_Cal_AZoff->Name = L"radioButton_Cal_AZoff";
      this->radioButton_Cal_AZoff->Size = System::Drawing::Size(39, 17);
      this->radioButton_Cal_AZoff->TabIndex = 61;
      this->radioButton_Cal_AZoff->Text = L"Off";
      this->radioButton_Cal_AZoff->UseVisualStyleBackColor = true;
      this->radioButton_Cal_AZoff->CheckedChanged += gcnew System::EventHandler(this, &BikePowerDisplay::radioButton_Cal_AZ_CheckedChanged);
      // 
      // button_Cal_AutoZeroSet
      // 
      this->button_Cal_AutoZeroSet->Enabled = false;
      this->button_Cal_AutoZeroSet->Location = System::Drawing::Point(4, 1);
      this->button_Cal_AutoZeroSet->Name = L"button_Cal_AutoZeroSet";
      this->button_Cal_AutoZeroSet->Size = System::Drawing::Size(63, 23);
      this->button_Cal_AutoZeroSet->TabIndex = 63;
      this->button_Cal_AutoZeroSet->Text = L"AutoZero";
      this->button_Cal_AutoZeroSet->UseVisualStyleBackColor = true;
      this->button_Cal_AutoZeroSet->Click += gcnew System::EventHandler(this, &BikePowerDisplay::button_Cal_AutoZeroSet_Click);
      // 
      // numericUpDown_Cal_CTFSlope
      // 
      this->numericUpDown_Cal_CTFSlope->DecimalPlaces = 1;
      this->numericUpDown_Cal_CTFSlope->Enabled = false;
      this->numericUpDown_Cal_CTFSlope->Location = System::Drawing::Point(128, 24);
      this->numericUpDown_Cal_CTFSlope->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {50, 0, 0, 0});
      this->numericUpDown_Cal_CTFSlope->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {10, 0, 0, 0});
      this->numericUpDown_Cal_CTFSlope->Name = L"numericUpDown_Cal_CTFSlope";
      this->numericUpDown_Cal_CTFSlope->Size = System::Drawing::Size(49, 20);
      this->numericUpDown_Cal_CTFSlope->TabIndex = 65;
      this->numericUpDown_Cal_CTFSlope->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {15, 0, 0, 0});
      this->numericUpDown_Cal_CTFSlope->ValueChanged += gcnew System::EventHandler(this, &BikePowerDisplay::numericUpDown_Cal_CTFSlope_ValueChanged);
      // 
      // radioButton_Cal_AZon
      // 
      this->radioButton_Cal_AZon->AutoSize = true;
      this->radioButton_Cal_AZon->Checked = true;
      this->radioButton_Cal_AZon->Enabled = false;
      this->radioButton_Cal_AZon->Location = System::Drawing::Point(14, 23);
      this->radioButton_Cal_AZon->Name = L"radioButton_Cal_AZon";
      this->radioButton_Cal_AZon->Size = System::Drawing::Size(39, 17);
      this->radioButton_Cal_AZon->TabIndex = 62;
      this->radioButton_Cal_AZon->TabStop = true;
      this->radioButton_Cal_AZon->Text = L"On";
      this->radioButton_Cal_AZon->UseVisualStyleBackColor = true;
      this->radioButton_Cal_AZon->CheckedChanged += gcnew System::EventHandler(this, &BikePowerDisplay::radioButton_Cal_AZ_CheckedChanged);
      // 
      // button_Cal_CTFSerialSet
      // 
      this->button_Cal_CTFSerialSet->Enabled = false;
      this->button_Cal_CTFSerialSet->Location = System::Drawing::Point(73, 1);
      this->button_Cal_CTFSerialSet->Name = L"button_Cal_CTFSerialSet";
      this->button_Cal_CTFSerialSet->Size = System::Drawing::Size(46, 23);
      this->button_Cal_CTFSerialSet->TabIndex = 66;
      this->button_Cal_CTFSerialSet->Text = L"Serial";
      this->button_Cal_CTFSerialSet->UseVisualStyleBackColor = true;
      this->button_Cal_CTFSerialSet->Click += gcnew System::EventHandler(this, &BikePowerDisplay::button_Cal_CTFSerialSet_Click);
      // 
      // numericUpDown_Cal_CTFSerial
      // 
      this->numericUpDown_Cal_CTFSerial->Enabled = false;
      this->numericUpDown_Cal_CTFSerial->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {123, 0, 0, 0});
      this->numericUpDown_Cal_CTFSerial->Location = System::Drawing::Point(69, 24);
      this->numericUpDown_Cal_CTFSerial->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {65535, 0, 0, 0});
      this->numericUpDown_Cal_CTFSerial->Name = L"numericUpDown_Cal_CTFSerial";
      this->numericUpDown_Cal_CTFSerial->Size = System::Drawing::Size(55, 20);
      this->numericUpDown_Cal_CTFSerial->TabIndex = 64;
      this->numericUpDown_Cal_CTFSerial->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {54321, 0, 0, 0});
      this->numericUpDown_Cal_CTFSerial->ValueChanged += gcnew System::EventHandler(this, &BikePowerDisplay::numericUpDown_Cal_CTFSerial_ValueChanged);
      // 
      // label_Cal_RawTorqueDisplay
      // 
      this->label_Cal_RawTorqueDisplay->AutoSize = true;
      this->label_Cal_RawTorqueDisplay->Location = System::Drawing::Point(325, 88);
      this->label_Cal_RawTorqueDisplay->Name = L"label_Cal_RawTorqueDisplay";
      this->label_Cal_RawTorqueDisplay->Size = System::Drawing::Size(16, 13);
      this->label_Cal_RawTorqueDisplay->TabIndex = 60;
      this->label_Cal_RawTorqueDisplay->Text = L"---";
      // 
      // label_Cal_OffsetTorqueDisplay
      // 
      this->label_Cal_OffsetTorqueDisplay->AutoSize = true;
      this->label_Cal_OffsetTorqueDisplay->Location = System::Drawing::Point(250, 88);
      this->label_Cal_OffsetTorqueDisplay->Name = L"label_Cal_OffsetTorqueDisplay";
      this->label_Cal_OffsetTorqueDisplay->Size = System::Drawing::Size(16, 13);
      this->label_Cal_OffsetTorqueDisplay->TabIndex = 59;
      this->label_Cal_OffsetTorqueDisplay->Text = L"---";
      // 
      // label_Cal_CalNumberDisplay
      // 
      this->label_Cal_CalNumberDisplay->AutoSize = true;
      this->label_Cal_CalNumberDisplay->Location = System::Drawing::Point(202, 13);
      this->label_Cal_CalNumberDisplay->Name = L"label_Cal_CalNumberDisplay";
      this->label_Cal_CalNumberDisplay->Size = System::Drawing::Size(16, 13);
      this->label_Cal_CalNumberDisplay->TabIndex = 58;
      this->label_Cal_CalNumberDisplay->Text = L"---";
      // 
      // numericUpDown_Cal_WheelCircum
      // 
      this->numericUpDown_Cal_WheelCircum->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {5, 0, 0, 0});
      this->numericUpDown_Cal_WheelCircum->Location = System::Drawing::Point(184, 89);
      this->numericUpDown_Cal_WheelCircum->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
      this->numericUpDown_Cal_WheelCircum->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
      this->numericUpDown_Cal_WheelCircum->Name = L"numericUpDown_Cal_WheelCircum";
      this->numericUpDown_Cal_WheelCircum->Size = System::Drawing::Size(47, 20);
      this->numericUpDown_Cal_WheelCircum->TabIndex = 56;
      this->numericUpDown_Cal_WheelCircum->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {207, 0, 0, 0});
      this->numericUpDown_Cal_WheelCircum->ValueChanged += gcnew System::EventHandler(this, &BikePowerDisplay::numericUpDown_Cal_WheelCircum_ValueChanged);
      // 
      // button_Cal_Calibrate
      // 
      this->button_Cal_Calibrate->Location = System::Drawing::Point(27, 8);
      this->button_Cal_Calibrate->Name = L"button_Cal_Calibrate";
      this->button_Cal_Calibrate->Size = System::Drawing::Size(141, 23);
      this->button_Cal_Calibrate->TabIndex = 42;
      this->button_Cal_Calibrate->Text = L"Send Calibration Request";
      this->button_Cal_Calibrate->UseVisualStyleBackColor = true;
      this->button_Cal_Calibrate->Click += gcnew System::EventHandler(this, &BikePowerDisplay::button_Cal_Calibrate_Click);
      // 
      // label_Cal_WheelCircum
      // 
      this->label_Cal_WheelCircum->AutoSize = true;
      this->label_Cal_WheelCircum->Location = System::Drawing::Point(49, 91);
      this->label_Cal_WheelCircum->Name = L"label_Cal_WheelCircum";
      this->label_Cal_WheelCircum->Size = System::Drawing::Size(135, 13);
      this->label_Cal_WheelCircum->TabIndex = 57;
      this->label_Cal_WheelCircum->Text = L"Wheel Circumference (cm):";
      // 
      // label_Cal_TorqOffset
      // 
      this->label_Cal_TorqOffset->AutoSize = true;
      this->label_Cal_TorqOffset->Location = System::Drawing::Point(239, 72);
      this->label_Cal_TorqOffset->Name = L"label_Cal_TorqOffset";
      this->label_Cal_TorqOffset->Size = System::Drawing::Size(75, 13);
      this->label_Cal_TorqOffset->TabIndex = 33;
      this->label_Cal_TorqOffset->Text = L"Offset Torque:";
      // 
      // label_Cal_AZStatus
      // 
      this->label_Cal_AZStatus->AutoSize = true;
      this->label_Cal_AZStatus->Location = System::Drawing::Point(250, 8);
      this->label_Cal_AZStatus->Name = L"label_Cal_AZStatus";
      this->label_Cal_AZStatus->Size = System::Drawing::Size(124, 13);
      this->label_Cal_AZStatus->TabIndex = 29;
      this->label_Cal_AZStatus->Text = L"Current Auto Zero Status";
      // 
      // listBox_Cal_AZStatus
      // 
      this->listBox_Cal_AZStatus->Enabled = false;
      this->listBox_Cal_AZStatus->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, 
         System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
      this->listBox_Cal_AZStatus->FormattingEnabled = true;
      this->listBox_Cal_AZStatus->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"0 - Off", L"1 - On", L"FF- Not Supported"});
      this->listBox_Cal_AZStatus->Location = System::Drawing::Point(253, 25);
      this->listBox_Cal_AZStatus->Name = L"listBox_Cal_AZStatus";
      this->listBox_Cal_AZStatus->Size = System::Drawing::Size(120, 43);
      this->listBox_Cal_AZStatus->TabIndex = 39;
      // 
      // label_Cal_CalNum
      // 
      this->label_Cal_CalNum->AutoSize = true;
      this->label_Cal_CalNum->Location = System::Drawing::Point(169, 13);
      this->label_Cal_CalNum->Name = L"label_Cal_CalNum";
      this->label_Cal_CalNum->Size = System::Drawing::Size(35, 13);
      this->label_Cal_CalNum->TabIndex = 4;
      this->label_Cal_CalNum->Text = L"Cal #:";
      // 
      // label_Cal_RawTorq
      // 
      this->label_Cal_RawTorq->AutoSize = true;
      this->label_Cal_RawTorq->Location = System::Drawing::Point(316, 72);
      this->label_Cal_RawTorq->Name = L"label_Cal_RawTorq";
      this->label_Cal_RawTorq->Size = System::Drawing::Size(69, 13);
      this->label_Cal_RawTorq->TabIndex = 32;
      this->label_Cal_RawTorq->Text = L"Raw Torque:";
      // 
      // label_Cal_SetLabel
      // 
      this->label_Cal_SetLabel->AutoSize = true;
      this->label_Cal_SetLabel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
      this->label_Cal_SetLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
         static_cast<System::Byte>(0)));
      this->label_Cal_SetLabel->Location = System::Drawing::Point(26, 51);
      this->label_Cal_SetLabel->Margin = System::Windows::Forms::Padding(0);
      this->label_Cal_SetLabel->Name = L"label_Cal_SetLabel";
      this->label_Cal_SetLabel->Size = System::Drawing::Size(28, 15);
      this->label_Cal_SetLabel->TabIndex = 68;
      this->label_Cal_SetLabel->Text = L"Set";
      this->label_Cal_SetLabel->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
      // 
      // tabPage_GlobalBattery
      // 
      this->tabPage_GlobalBattery->Controls->Add(this->label_Bat_VoltsDisplay);
      this->tabPage_GlobalBattery->Controls->Add(this->label_Glb_SoftwareVer);
      this->tabPage_GlobalBattery->Controls->Add(this->label_Bat_SecPrecis);
      this->tabPage_GlobalBattery->Controls->Add(this->label_Bat_Volts);
      this->tabPage_GlobalBattery->Controls->Add(this->label_Glb_HardwareVer);
      this->tabPage_GlobalBattery->Controls->Add(this->label_Bat_Status);
      this->tabPage_GlobalBattery->Controls->Add(this->listBox_Bat_Status);
      this->tabPage_GlobalBattery->Controls->Add(this->label_Glb_SoftwareVerDisplay);
      this->tabPage_GlobalBattery->Controls->Add(this->label_Bat_ElpTimeDisplay);
      this->tabPage_GlobalBattery->Controls->Add(this->label_Bat_ElpTime);
      this->tabPage_GlobalBattery->Controls->Add(this->label_Glb_SerialNumDisplay);
      this->tabPage_GlobalBattery->Controls->Add(this->label_Glb_ModelNum);
      this->tabPage_GlobalBattery->Controls->Add(this->label_Glb_ManfIDDisplay);
      this->tabPage_GlobalBattery->Controls->Add(this->label_Glb_SerialNum);
      this->tabPage_GlobalBattery->Controls->Add(this->label_Glb_ModelNumDisplay);
      this->tabPage_GlobalBattery->Controls->Add(this->label_Glb_HardwareVerDisplay);
      this->tabPage_GlobalBattery->Controls->Add(this->label_Glb_ManfID);
      this->tabPage_GlobalBattery->Location = System::Drawing::Point(4, 22);
      this->tabPage_GlobalBattery->Name = L"tabPage_GlobalBattery";
      this->tabPage_GlobalBattery->Size = System::Drawing::Size(389, 111);
      this->tabPage_GlobalBattery->TabIndex = 4;
      this->tabPage_GlobalBattery->Text = L"Global Data";
      this->tabPage_GlobalBattery->UseVisualStyleBackColor = true;
      // 
      // label_Bat_VoltsDisplay
      // 
      this->label_Bat_VoltsDisplay->AutoSize = true;
      this->label_Bat_VoltsDisplay->Location = System::Drawing::Point(338, 87);
      this->label_Bat_VoltsDisplay->Name = L"label_Bat_VoltsDisplay";
      this->label_Bat_VoltsDisplay->Size = System::Drawing::Size(16, 13);
      this->label_Bat_VoltsDisplay->TabIndex = 61;
      this->label_Bat_VoltsDisplay->Text = L"---";
      // 
      // label_Glb_SoftwareVer
      // 
      this->label_Glb_SoftwareVer->AutoSize = true;
      this->label_Glb_SoftwareVer->Location = System::Drawing::Point(38, 34);
      this->label_Glb_SoftwareVer->Name = L"label_Glb_SoftwareVer";
      this->label_Glb_SoftwareVer->Size = System::Drawing::Size(71, 13);
      this->label_Glb_SoftwareVer->TabIndex = 4;
      this->label_Glb_SoftwareVer->Text = L"Software Ver:";
      // 
      // label_Bat_SecPrecis
      // 
      this->label_Bat_SecPrecis->AutoSize = true;
      this->label_Bat_SecPrecis->Location = System::Drawing::Point(301, 55);
      this->label_Bat_SecPrecis->Name = L"label_Bat_SecPrecis";
      this->label_Bat_SecPrecis->Size = System::Drawing::Size(87, 13);
      this->label_Bat_SecPrecis->TabIndex = 60;
      this->label_Bat_SecPrecis->Text = L"\?\? Sec Precision";
      // 
      // label_Bat_Volts
      // 
      this->label_Bat_Volts->AutoSize = true;
      this->label_Bat_Volts->Location = System::Drawing::Point(305, 87);
      this->label_Bat_Volts->Name = L"label_Bat_Volts";
      this->label_Bat_Volts->Size = System::Drawing::Size(36, 13);
      this->label_Bat_Volts->TabIndex = 30;
      this->label_Bat_Volts->Text = L"Volts: ";
      // 
      // label_Glb_HardwareVer
      // 
      this->label_Glb_HardwareVer->AutoSize = true;
      this->label_Glb_HardwareVer->Location = System::Drawing::Point(34, 91);
      this->label_Glb_HardwareVer->Name = L"label_Glb_HardwareVer";
      this->label_Glb_HardwareVer->Size = System::Drawing::Size(75, 13);
      this->label_Glb_HardwareVer->TabIndex = 3;
      this->label_Glb_HardwareVer->Text = L"Hardware Ver:";
      // 
      // label_Bat_Status
      // 
      this->label_Bat_Status->AutoSize = true;
      this->label_Bat_Status->Location = System::Drawing::Point(221, 7);
      this->label_Bat_Status->Name = L"label_Bat_Status";
      this->label_Bat_Status->Size = System::Drawing::Size(73, 13);
      this->label_Bat_Status->TabIndex = 27;
      this->label_Bat_Status->Text = L"Battery Status";
      // 
      // listBox_Bat_Status
      // 
      this->listBox_Bat_Status->Enabled = false;
      this->listBox_Bat_Status->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
         static_cast<System::Byte>(0)));
      this->listBox_Bat_Status->FormattingEnabled = true;
      this->listBox_Bat_Status->Items->AddRange(gcnew cli::array< System::Object^  >(6) {L"1 - New", L"2 - Good", L"3 - OK", L"4 - Low", 
         L"5 - Critical", L"7 - disabled"});
      this->listBox_Bat_Status->Location = System::Drawing::Point(218, 23);
      this->listBox_Bat_Status->Name = L"listBox_Bat_Status";
      this->listBox_Bat_Status->Size = System::Drawing::Size(79, 82);
      this->listBox_Bat_Status->TabIndex = 55;
      // 
      // label_Glb_SoftwareVerDisplay
      // 
      this->label_Glb_SoftwareVerDisplay->Location = System::Drawing::Point(115, 34);
      this->label_Glb_SoftwareVerDisplay->Name = L"label_Glb_SoftwareVerDisplay";
      this->label_Glb_SoftwareVerDisplay->Size = System::Drawing::Size(27, 13);
      this->label_Glb_SoftwareVerDisplay->TabIndex = 16;
      this->label_Glb_SoftwareVerDisplay->Text = L"---";
      // 
      // label_Bat_ElpTimeDisplay
      // 
      this->label_Bat_ElpTimeDisplay->Location = System::Drawing::Point(318, 40);
      this->label_Bat_ElpTimeDisplay->Name = L"label_Bat_ElpTimeDisplay";
      this->label_Bat_ElpTimeDisplay->Size = System::Drawing::Size(62, 13);
      this->label_Bat_ElpTimeDisplay->TabIndex = 21;
      this->label_Bat_ElpTimeDisplay->Text = L"---";
      this->label_Bat_ElpTimeDisplay->TextAlign = System::Drawing::ContentAlignment::BottomRight;
      // 
      // label_Bat_ElpTime
      // 
      this->label_Bat_ElpTime->AutoSize = true;
      this->label_Bat_ElpTime->Location = System::Drawing::Point(303, 24);
      this->label_Bat_ElpTime->Name = L"label_Bat_ElpTime";
      this->label_Bat_ElpTime->Size = System::Drawing::Size(83, 13);
      this->label_Bat_ElpTime->TabIndex = 19;
      this->label_Bat_ElpTime->Text = L"Battery Time (s):";
      // 
      // label_Glb_SerialNumDisplay
      // 
      this->label_Glb_SerialNumDisplay->Location = System::Drawing::Point(115, 15);
      this->label_Glb_SerialNumDisplay->Name = L"label_Glb_SerialNumDisplay";
      this->label_Glb_SerialNumDisplay->Size = System::Drawing::Size(71, 13);
      this->label_Glb_SerialNumDisplay->TabIndex = 11;
      this->label_Glb_SerialNumDisplay->Text = L"---";
      // 
      // label_Glb_ModelNum
      // 
      this->label_Glb_ModelNum->AutoSize = true;
      this->label_Glb_ModelNum->Location = System::Drawing::Point(60, 72);
      this->label_Glb_ModelNum->Name = L"label_Glb_ModelNum";
      this->label_Glb_ModelNum->Size = System::Drawing::Size(49, 13);
      this->label_Glb_ModelNum->TabIndex = 5;
      this->label_Glb_ModelNum->Text = L"Model #:";
      // 
      // label_Glb_ManfIDDisplay
      // 
      this->label_Glb_ManfIDDisplay->Location = System::Drawing::Point(115, 53);
      this->label_Glb_ManfIDDisplay->Name = L"label_Glb_ManfIDDisplay";
      this->label_Glb_ManfIDDisplay->Size = System::Drawing::Size(27, 13);
      this->label_Glb_ManfIDDisplay->TabIndex = 8;
      this->label_Glb_ManfIDDisplay->Text = L"---";
      // 
      // label_Glb_SerialNum
      // 
      this->label_Glb_SerialNum->AutoSize = true;
      this->label_Glb_SerialNum->Location = System::Drawing::Point(63, 15);
      this->label_Glb_SerialNum->Name = L"label_Glb_SerialNum";
      this->label_Glb_SerialNum->Size = System::Drawing::Size(46, 13);
      this->label_Glb_SerialNum->TabIndex = 2;
      this->label_Glb_SerialNum->Text = L"Serial #:";
      // 
      // label_Glb_ModelNumDisplay
      // 
      this->label_Glb_ModelNumDisplay->Location = System::Drawing::Point(115, 72);
      this->label_Glb_ModelNumDisplay->Name = L"label_Glb_ModelNumDisplay";
      this->label_Glb_ModelNumDisplay->Size = System::Drawing::Size(42, 13);
      this->label_Glb_ModelNumDisplay->TabIndex = 9;
      this->label_Glb_ModelNumDisplay->Text = L"---";
      // 
      // label_Glb_HardwareVerDisplay
      // 
      this->label_Glb_HardwareVerDisplay->Location = System::Drawing::Point(115, 91);
      this->label_Glb_HardwareVerDisplay->Name = L"label_Glb_HardwareVerDisplay";
      this->label_Glb_HardwareVerDisplay->Size = System::Drawing::Size(27, 13);
      this->label_Glb_HardwareVerDisplay->TabIndex = 14;
      this->label_Glb_HardwareVerDisplay->Text = L"---";
      // 
      // label_Glb_ManfID
      // 
      this->label_Glb_ManfID->AutoSize = true;
      this->label_Glb_ManfID->Location = System::Drawing::Point(58, 53);
      this->label_Glb_ManfID->Name = L"label_Glb_ManfID";
      this->label_Glb_ManfID->Size = System::Drawing::Size(51, 13);
      this->label_Glb_ManfID->TabIndex = 1;
      this->label_Glb_ManfID->Text = L"Manf. ID:";
      // 
      // tabPage1
      // 
      this->tabPage1->Controls->Add(this->label7);
      this->tabPage1->Controls->Add(this->label3);
      this->tabPage1->Controls->Add(this->numericUpDown_TransResponse);
      this->tabPage1->Controls->Add(this->label_AutoCrankLength);
      this->tabPage1->Controls->Add(this->label10);
      this->tabPage1->Controls->Add(this->label_CrankLengthStatus);
      this->tabPage1->Controls->Add(this->label_CrankLength);
      this->tabPage1->Controls->Add(this->label_SensorStatus);
      this->tabPage1->Controls->Add(this->label6);
      this->tabPage1->Controls->Add(this->label5);
      this->tabPage1->Controls->Add(this->label4);
      this->tabPage1->Controls->Add(this->label_SWMistmatchStatus);
      this->tabPage1->Controls->Add(this->label2);
      this->tabPage1->Controls->Add(this->checkBox_AutoCrank);
      this->tabPage1->Controls->Add(this->checkBox_InvalidCrankLength);
      this->tabPage1->Controls->Add(this->label1);
      this->tabPage1->Controls->Add(this->numericUpDown_CrankLength);
      this->tabPage1->Controls->Add(this->button_SetCrankStatus);
      this->tabPage1->Controls->Add(this->button_GetCrankStatus);
      this->tabPage1->Location = System::Drawing::Point(4, 22);
      this->tabPage1->Margin = System::Windows::Forms::Padding(1);
      this->tabPage1->Name = L"tabPage1";
      this->tabPage1->Size = System::Drawing::Size(389, 111);
      this->tabPage1->TabIndex = 7;
      this->tabPage1->Text = L"Get/Set";
      this->tabPage1->UseVisualStyleBackColor = true;
      // 
      // label7
      // 
      this->label7->AutoSize = true;
      this->label7->Location = System::Drawing::Point(358, 12);
      this->label7->Name = L"label7";
      this->label7->Size = System::Drawing::Size(23, 13);
      this->label7->TabIndex = 18;
      this->label7->Text = L"mm";
      // 
      // label3
      // 
      this->label3->AutoSize = true;
      this->label3->Location = System::Drawing::Point(197, 88);
      this->label3->Name = L"label3";
      this->label3->Size = System::Drawing::Size(69, 13);
      this->label3->TabIndex = 17;
      this->label3->Text = L"transmissions";
      // 
      // numericUpDown_TransResponse
      // 
      this->numericUpDown_TransResponse->Location = System::Drawing::Point(147, 86);
      this->numericUpDown_TransResponse->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
      this->numericUpDown_TransResponse->Name = L"numericUpDown_TransResponse";
      this->numericUpDown_TransResponse->Size = System::Drawing::Size(44, 20);
      this->numericUpDown_TransResponse->TabIndex = 16;
      this->numericUpDown_TransResponse->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {4, 0, 0, 0});
      // 
      // label_AutoCrankLength
      // 
      this->label_AutoCrankLength->AutoSize = true;
      this->label_AutoCrankLength->Location = System::Drawing::Point(121, 66);
      this->label_AutoCrankLength->Name = L"label_AutoCrankLength";
      this->label_AutoCrankLength->Size = System::Drawing::Size(19, 13);
      this->label_AutoCrankLength->TabIndex = 15;
      this->label_AutoCrankLength->Text = L"----";
      // 
      // label10
      // 
      this->label10->AutoSize = true;
      this->label10->Location = System::Drawing::Point(18, 66);
      this->label10->Margin = System::Windows::Forms::Padding(1);
      this->label10->Name = L"label10";
      this->label10->Size = System::Drawing::Size(99, 13);
      this->label10->TabIndex = 14;
      this->label10->Text = L"Auto Crank Length:";
      // 
      // label_CrankLengthStatus
      // 
      this->label_CrankLengthStatus->AutoSize = true;
      this->label_CrankLengthStatus->Location = System::Drawing::Point(121, 51);
      this->label_CrankLengthStatus->Name = L"label_CrankLengthStatus";
      this->label_CrankLengthStatus->Size = System::Drawing::Size(19, 13);
      this->label_CrankLengthStatus->TabIndex = 13;
      this->label_CrankLengthStatus->Text = L"----";
      // 
      // label_CrankLength
      // 
      this->label_CrankLength->AutoSize = true;
      this->label_CrankLength->Location = System::Drawing::Point(121, 6);
      this->label_CrankLength->Name = L"label_CrankLength";
      this->label_CrankLength->Size = System::Drawing::Size(19, 13);
      this->label_CrankLength->TabIndex = 12;
      this->label_CrankLength->Text = L"----";
      // 
      // label_SensorStatus
      // 
      this->label_SensorStatus->AutoSize = true;
      this->label_SensorStatus->Location = System::Drawing::Point(121, 21);
      this->label_SensorStatus->Name = L"label_SensorStatus";
      this->label_SensorStatus->Size = System::Drawing::Size(19, 13);
      this->label_SensorStatus->TabIndex = 11;
      this->label_SensorStatus->Text = L"----";
      // 
      // label6
      // 
      this->label6->AutoSize = true;
      this->label6->Location = System::Drawing::Point(5, 36);
      this->label6->Margin = System::Windows::Forms::Padding(1);
      this->label6->Name = L"label6";
      this->label6->Size = System::Drawing::Size(112, 13);
      this->label6->TabIndex = 10;
      this->label6->Text = L"SW Mistmatch Status:";
      // 
      // label5
      // 
      this->label5->AutoSize = true;
      this->label5->Location = System::Drawing::Point(10, 51);
      this->label5->Margin = System::Windows::Forms::Padding(1);
      this->label5->Name = L"label5";
      this->label5->Size = System::Drawing::Size(107, 13);
      this->label5->TabIndex = 9;
      this->label5->Text = L"Crank Length Status:";
      // 
      // label4
      // 
      this->label4->AutoSize = true;
      this->label4->Location = System::Drawing::Point(41, 21);
      this->label4->Margin = System::Windows::Forms::Padding(1);
      this->label4->Name = L"label4";
      this->label4->Size = System::Drawing::Size(76, 13);
      this->label4->TabIndex = 8;
      this->label4->Text = L"Sensor Status:";
      // 
      // label_SWMistmatchStatus
      // 
      this->label_SWMistmatchStatus->AutoSize = true;
      this->label_SWMistmatchStatus->Location = System::Drawing::Point(121, 36);
      this->label_SWMistmatchStatus->Name = L"label_SWMistmatchStatus";
      this->label_SWMistmatchStatus->Size = System::Drawing::Size(19, 13);
      this->label_SWMistmatchStatus->TabIndex = 7;
      this->label_SWMistmatchStatus->Text = L"----";
      // 
      // label2
      // 
      this->label2->AutoSize = true;
      this->label2->Location = System::Drawing::Point(43, 6);
      this->label2->Margin = System::Windows::Forms::Padding(1);
      this->label2->Name = L"label2";
      this->label2->Size = System::Drawing::Size(74, 13);
      this->label2->TabIndex = 6;
      this->label2->Text = L"Crank Length:";
      // 
      // checkBox_AutoCrank
      // 
      this->checkBox_AutoCrank->AutoSize = true;
      this->checkBox_AutoCrank->Location = System::Drawing::Point(297, 59);
      this->checkBox_AutoCrank->Name = L"checkBox_AutoCrank";
      this->checkBox_AutoCrank->Size = System::Drawing::Size(79, 17);
      this->checkBox_AutoCrank->TabIndex = 5;
      this->checkBox_AutoCrank->Text = L"Auto Crank";
      this->checkBox_AutoCrank->UseVisualStyleBackColor = true;
      this->checkBox_AutoCrank->CheckedChanged += gcnew System::EventHandler(this, &BikePowerDisplay::checkBox_AutoCrank_CheckedChanged);
      // 
      // checkBox_InvalidCrankLength
      // 
      this->checkBox_InvalidCrankLength->AutoSize = true;
      this->checkBox_InvalidCrankLength->Location = System::Drawing::Point(297, 36);
      this->checkBox_InvalidCrankLength->Name = L"checkBox_InvalidCrankLength";
      this->checkBox_InvalidCrankLength->Size = System::Drawing::Size(57, 17);
      this->checkBox_InvalidCrankLength->TabIndex = 4;
      this->checkBox_InvalidCrankLength->Text = L"Invalid";
      this->checkBox_InvalidCrankLength->UseVisualStyleBackColor = true;
      this->checkBox_InvalidCrankLength->CheckedChanged += gcnew System::EventHandler(this, &BikePowerDisplay::checkBox_InvalidCrankLength_CheckedChanged);
      // 
      // label1
      // 
      this->label1->AutoSize = true;
      this->label1->Location = System::Drawing::Point(222, 12);
      this->label1->Name = L"label1";
      this->label1->Size = System::Drawing::Size(71, 13);
      this->label1->TabIndex = 3;
      this->label1->Text = L"Crank Length";
      // 
      // numericUpDown_CrankLength
      // 
      this->numericUpDown_CrankLength->DecimalPlaces = 1;
      this->numericUpDown_CrankLength->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {5, 0, 0, 65536});
      this->numericUpDown_CrankLength->Location = System::Drawing::Point(297, 10);
      this->numericUpDown_CrankLength->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {2365, 0, 0, 65536});
      this->numericUpDown_CrankLength->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1100, 0, 0, 65536});
      this->numericUpDown_CrankLength->Name = L"numericUpDown_CrankLength";
      this->numericUpDown_CrankLength->Size = System::Drawing::Size(60, 20);
      this->numericUpDown_CrankLength->TabIndex = 2;
      this->numericUpDown_CrankLength->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1725, 0, 0, 65536});
      // 
      // button_SetCrankStatus
      // 
      this->button_SetCrankStatus->Location = System::Drawing::Point(272, 85);
      this->button_SetCrankStatus->Name = L"button_SetCrankStatus";
      this->button_SetCrankStatus->Size = System::Drawing::Size(114, 23);
      this->button_SetCrankStatus->TabIndex = 1;
      this->button_SetCrankStatus->Text = L"Set Crank Status";
      this->button_SetCrankStatus->UseVisualStyleBackColor = true;
      this->button_SetCrankStatus->Click += gcnew System::EventHandler(this, &BikePowerDisplay::button_SetCrankStatus_Click);
      // 
      // button_GetCrankStatus
      // 
      this->button_GetCrankStatus->Location = System::Drawing::Point(8, 85);
      this->button_GetCrankStatus->Name = L"button_GetCrankStatus";
      this->button_GetCrankStatus->Size = System::Drawing::Size(132, 23);
      this->button_GetCrankStatus->TabIndex = 0;
      this->button_GetCrankStatus->Text = L"Request Crank Status";
      this->button_GetCrankStatus->UseVisualStyleBackColor = true;
      this->button_GetCrankStatus->Click += gcnew System::EventHandler(this, &BikePowerDisplay::button_GetCrankStatus_Click);
      // 
      // panel_Display
      // 
      this->panel_Display->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
      this->panel_Display->Controls->Add(this->label_Trn_PedalDisplay);
      this->panel_Display->Controls->Add(this->label_Trn_PedalPwrDisplay);
      this->panel_Display->Controls->Add(this->label_Trn_PedalPwr);
      this->panel_Display->Controls->Add(this->label_Trn_AccumPowerDisplay);
      this->panel_Display->Controls->Add(this->label_Trn_AccumPower);
      this->panel_Display->Controls->Add(this->label_Trn_AccumTwo);
      this->panel_Display->Controls->Add(this->label_Trn_AccumOneDisplay);
      this->panel_Display->Controls->Add(this->label_Trn_AccumOne);
      this->panel_Display->Controls->Add(this->label_Trn_AccumTwoDisplay);
      this->panel_Display->Controls->Add(this->label_Trn_AccumTitle);
      this->panel_Display->Controls->Add(this->label_Trn_Slope);
      this->panel_Display->Controls->Add(this->label_Trn_SlopeDisplay);
      this->panel_Display->Controls->Add(this->label_Trn_UpdateCountDisplay);
      this->panel_Display->Controls->Add(this->label_Trn_UpdateCount);
      this->panel_Display->Controls->Add(this->label_Trn_EventCountDisplay);
      this->panel_Display->Controls->Add(this->label_Trn_EventCount);
      this->panel_Display->Controls->Add(this->label_Trn_InstPowerDisplay);
      this->panel_Display->Controls->Add(this->label_InstPower);
      this->panel_Display->Controls->Add(this->label_Trn_CadenceDisplay);
      this->panel_Display->Controls->Add(this->label_Trn_Cadence);
      this->panel_Display->Controls->Add(this->label_Trn_PageDisplay);
      this->panel_Display->Controls->Add(this->label_Trn_Page);
      this->panel_Display->Controls->Add(this->label_Trn_DisplayTitle);
      this->panel_Display->Location = System::Drawing::Point(53, 188);
      this->panel_Display->Name = L"panel_Display";
      this->panel_Display->Size = System::Drawing::Size(205, 103);
      this->panel_Display->TabIndex = 1;
      // 
      // label_Trn_PedalDisplay
      // 
      this->label_Trn_PedalDisplay->AutoSize = true;
      this->label_Trn_PedalDisplay->Location = System::Drawing::Point(119, 86);
      this->label_Trn_PedalDisplay->Name = L"label_Trn_PedalDisplay";
      this->label_Trn_PedalDisplay->Size = System::Drawing::Size(16, 13);
      this->label_Trn_PedalDisplay->TabIndex = 26;
      this->label_Trn_PedalDisplay->Text = L"---";
      // 
      // label_Trn_PedalPwrDisplay
      // 
      this->label_Trn_PedalPwrDisplay->AutoSize = true;
      this->label_Trn_PedalPwrDisplay->Location = System::Drawing::Point(91, 86);
      this->label_Trn_PedalPwrDisplay->Name = L"label_Trn_PedalPwrDisplay";
      this->label_Trn_PedalPwrDisplay->Size = System::Drawing::Size(16, 13);
      this->label_Trn_PedalPwrDisplay->TabIndex = 25;
      this->label_Trn_PedalPwrDisplay->Text = L"---";
      // 
      // label_Trn_PedalPwr
      // 
      this->label_Trn_PedalPwr->AutoSize = true;
      this->label_Trn_PedalPwr->Location = System::Drawing::Point(1, 86);
      this->label_Trn_PedalPwr->Name = L"label_Trn_PedalPwr";
      this->label_Trn_PedalPwr->Size = System::Drawing::Size(84, 13);
      this->label_Trn_PedalPwr->TabIndex = 24;
      this->label_Trn_PedalPwr->Text = L"Pedal Power(%):";
      // 
      // label_Trn_AccumPowerDisplay
      // 
      this->label_Trn_AccumPowerDisplay->BackColor = System::Drawing::SystemColors::Control;
      this->label_Trn_AccumPowerDisplay->Location = System::Drawing::Point(157, 71);
      this->label_Trn_AccumPowerDisplay->Name = L"label_Trn_AccumPowerDisplay";
      this->label_Trn_AccumPowerDisplay->Size = System::Drawing::Size(41, 13);
      this->label_Trn_AccumPowerDisplay->TabIndex = 19;
      this->label_Trn_AccumPowerDisplay->Text = L"---";
      // 
      // label_Trn_AccumPower
      // 
      this->label_Trn_AccumPower->AutoSize = true;
      this->label_Trn_AccumPower->BackColor = System::Drawing::SystemColors::Control;
      this->label_Trn_AccumPower->Location = System::Drawing::Point(118, 71);
      this->label_Trn_AccumPower->Name = L"label_Trn_AccumPower";
      this->label_Trn_AccumPower->Size = System::Drawing::Size(40, 13);
      this->label_Trn_AccumPower->TabIndex = 18;
      this->label_Trn_AccumPower->Text = L"Power:";
      // 
      // label_Trn_AccumTwo
      // 
      this->label_Trn_AccumTwo->AutoSize = true;
      this->label_Trn_AccumTwo->BackColor = System::Drawing::SystemColors::Control;
      this->label_Trn_AccumTwo->Location = System::Drawing::Point(114, 57);
      this->label_Trn_AccumTwo->Name = L"label_Trn_AccumTwo";
      this->label_Trn_AccumTwo->Size = System::Drawing::Size(44, 13);
      this->label_Trn_AccumTwo->TabIndex = 15;
      this->label_Trn_AccumTwo->Text = L"Torque:";
      // 
      // label_Trn_AccumOneDisplay
      // 
      this->label_Trn_AccumOneDisplay->BackColor = System::Drawing::SystemColors::Control;
      this->label_Trn_AccumOneDisplay->Location = System::Drawing::Point(157, 43);
      this->label_Trn_AccumOneDisplay->Name = L"label_Trn_AccumOneDisplay";
      this->label_Trn_AccumOneDisplay->Size = System::Drawing::Size(41, 13);
      this->label_Trn_AccumOneDisplay->TabIndex = 8;
      this->label_Trn_AccumOneDisplay->Text = L"---";
      // 
      // label_Trn_AccumOne
      // 
      this->label_Trn_AccumOne->AutoSize = true;
      this->label_Trn_AccumOne->BackColor = System::Drawing::SystemColors::Control;
      this->label_Trn_AccumOne->Location = System::Drawing::Point(110, 43);
      this->label_Trn_AccumOne->Name = L"label_Trn_AccumOne";
      this->label_Trn_AccumOne->Size = System::Drawing::Size(48, 13);
      this->label_Trn_AccumOne->TabIndex = 7;
      this->label_Trn_AccumOne->Text = L"Crank P:";
      // 
      // label_Trn_AccumTwoDisplay
      // 
      this->label_Trn_AccumTwoDisplay->BackColor = System::Drawing::SystemColors::Control;
      this->label_Trn_AccumTwoDisplay->Location = System::Drawing::Point(157, 57);
      this->label_Trn_AccumTwoDisplay->Name = L"label_Trn_AccumTwoDisplay";
      this->label_Trn_AccumTwoDisplay->Size = System::Drawing::Size(41, 13);
      this->label_Trn_AccumTwoDisplay->TabIndex = 16;
      this->label_Trn_AccumTwoDisplay->Text = L"---";
      // 
      // label_Trn_AccumTitle
      // 
      this->label_Trn_AccumTitle->AutoSize = true;
      this->label_Trn_AccumTitle->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Italic, 
         System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
      this->label_Trn_AccumTitle->Location = System::Drawing::Point(123, 29);
      this->label_Trn_AccumTitle->Name = L"label_Trn_AccumTitle";
      this->label_Trn_AccumTitle->Size = System::Drawing::Size(71, 13);
      this->label_Trn_AccumTitle->TabIndex = 17;
      this->label_Trn_AccumTitle->Text = L"Accumulates:";
      // 
      // label_Trn_Slope
      // 
      this->label_Trn_Slope->AutoSize = true;
      this->label_Trn_Slope->Location = System::Drawing::Point(121, 16);
      this->label_Trn_Slope->Name = L"label_Trn_Slope";
      this->label_Trn_Slope->Size = System::Drawing::Size(37, 13);
      this->label_Trn_Slope->TabIndex = 22;
      this->label_Trn_Slope->Text = L"Slope:";
      // 
      // label_Trn_SlopeDisplay
      // 
      this->label_Trn_SlopeDisplay->BackColor = System::Drawing::SystemColors::Control;
      this->label_Trn_SlopeDisplay->Location = System::Drawing::Point(157, 16);
      this->label_Trn_SlopeDisplay->Name = L"label_Trn_SlopeDisplay";
      this->label_Trn_SlopeDisplay->Size = System::Drawing::Size(41, 13);
      this->label_Trn_SlopeDisplay->TabIndex = 23;
      this->label_Trn_SlopeDisplay->Text = L"---";
      // 
      // label_Trn_UpdateCountDisplay
      // 
      this->label_Trn_UpdateCountDisplay->BackColor = System::Drawing::Color::Transparent;
      this->label_Trn_UpdateCountDisplay->Location = System::Drawing::Point(73, 71);
      this->label_Trn_UpdateCountDisplay->Name = L"label_Trn_UpdateCountDisplay";
      this->label_Trn_UpdateCountDisplay->Size = System::Drawing::Size(34, 13);
      this->label_Trn_UpdateCountDisplay->TabIndex = 12;
      this->label_Trn_UpdateCountDisplay->Text = L"---";
      // 
      // label_Trn_UpdateCount
      // 
      this->label_Trn_UpdateCount->AutoSize = true;
      this->label_Trn_UpdateCount->BackColor = System::Drawing::Color::Transparent;
      this->label_Trn_UpdateCount->Location = System::Drawing::Point(24, 71);
      this->label_Trn_UpdateCount->Name = L"label_Trn_UpdateCount";
      this->label_Trn_UpdateCount->Size = System::Drawing::Size(50, 13);
      this->label_Trn_UpdateCount->TabIndex = 11;
      this->label_Trn_UpdateCount->Text = L"Updates:";
      // 
      // label_Trn_EventCountDisplay
      // 
      this->label_Trn_EventCountDisplay->BackColor = System::Drawing::Color::Transparent;
      this->label_Trn_EventCountDisplay->Location = System::Drawing::Point(73, 57);
      this->label_Trn_EventCountDisplay->Name = L"label_Trn_EventCountDisplay";
      this->label_Trn_EventCountDisplay->Size = System::Drawing::Size(34, 13);
      this->label_Trn_EventCountDisplay->TabIndex = 2;
      this->label_Trn_EventCountDisplay->Text = L"---";
      // 
      // label_Trn_EventCount
      // 
      this->label_Trn_EventCount->AutoSize = true;
      this->label_Trn_EventCount->BackColor = System::Drawing::Color::Transparent;
      this->label_Trn_EventCount->Location = System::Drawing::Point(31, 57);
      this->label_Trn_EventCount->Name = L"label_Trn_EventCount";
      this->label_Trn_EventCount->Size = System::Drawing::Size(43, 13);
      this->label_Trn_EventCount->TabIndex = 0;
      this->label_Trn_EventCount->Text = L"Events:";
      // 
      // label_Trn_InstPowerDisplay
      // 
      this->label_Trn_InstPowerDisplay->BackColor = System::Drawing::Color::Transparent;
      this->label_Trn_InstPowerDisplay->Location = System::Drawing::Point(73, 44);
      this->label_Trn_InstPowerDisplay->Name = L"label_Trn_InstPowerDisplay";
      this->label_Trn_InstPowerDisplay->Size = System::Drawing::Size(34, 13);
      this->label_Trn_InstPowerDisplay->TabIndex = 21;
      this->label_Trn_InstPowerDisplay->Text = L"---";
      // 
      // label_InstPower
      // 
      this->label_InstPower->AutoSize = true;
      this->label_InstPower->BackColor = System::Drawing::Color::Transparent;
      this->label_InstPower->Location = System::Drawing::Point(11, 43);
      this->label_InstPower->Name = L"label_InstPower";
      this->label_InstPower->Size = System::Drawing::Size(63, 13);
      this->label_InstPower->TabIndex = 20;
      this->label_InstPower->Text = L"Inst. Power:";
      // 
      // label_Trn_CadenceDisplay
      // 
      this->label_Trn_CadenceDisplay->BackColor = System::Drawing::SystemColors::Control;
      this->label_Trn_CadenceDisplay->Location = System::Drawing::Point(73, 30);
      this->label_Trn_CadenceDisplay->Name = L"label_Trn_CadenceDisplay";
      this->label_Trn_CadenceDisplay->Size = System::Drawing::Size(34, 13);
      this->label_Trn_CadenceDisplay->TabIndex = 10;
      this->label_Trn_CadenceDisplay->Text = L"---";
      // 
      // label_Trn_Cadence
      // 
      this->label_Trn_Cadence->AutoSize = true;
      this->label_Trn_Cadence->BackColor = System::Drawing::SystemColors::Control;
      this->label_Trn_Cadence->Location = System::Drawing::Point(1, 30);
      this->label_Trn_Cadence->Name = L"label_Trn_Cadence";
      this->label_Trn_Cadence->Size = System::Drawing::Size(73, 13);
      this->label_Trn_Cadence->TabIndex = 9;
      this->label_Trn_Cadence->Text = L"Inst.Cadence:";
      // 
      // label_Trn_PageDisplay
      // 
      this->label_Trn_PageDisplay->BackColor = System::Drawing::SystemColors::Control;
      this->label_Trn_PageDisplay->Location = System::Drawing::Point(73, 16);
      this->label_Trn_PageDisplay->Name = L"label_Trn_PageDisplay";
      this->label_Trn_PageDisplay->Size = System::Drawing::Size(34, 13);
      this->label_Trn_PageDisplay->TabIndex = 14;
      this->label_Trn_PageDisplay->Text = L"---";
      // 
      // label_Trn_Page
      // 
      this->label_Trn_Page->AutoSize = true;
      this->label_Trn_Page->BackColor = System::Drawing::SystemColors::Control;
      this->label_Trn_Page->Location = System::Drawing::Point(2, 16);
      this->label_Trn_Page->Name = L"label_Trn_Page";
      this->label_Trn_Page->Size = System::Drawing::Size(72, 13);
      this->label_Trn_Page->TabIndex = 13;
      this->label_Trn_Page->Text = L"Current Page:";
      // 
      // label_Trn_DisplayTitle
      // 
      this->label_Trn_DisplayTitle->AutoSize = true;
      this->label_Trn_DisplayTitle->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Underline, 
         System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
      this->label_Trn_DisplayTitle->Location = System::Drawing::Point(2, 2);
      this->label_Trn_DisplayTitle->Name = L"label_Trn_DisplayTitle";
      this->label_Trn_DisplayTitle->Size = System::Drawing::Size(165, 13);
      this->label_Trn_DisplayTitle->TabIndex = 4;
      this->label_Trn_DisplayTitle->Text = L"Current Bike Power Transmission:";
      // 
      // tabControl_TEPS
      // 
      this->tabControl_TEPS->Controls->Add(this->label_RightPedalSmoothness);
      this->tabControl_TEPS->Controls->Add(this->label_LeftPedalSmoothness);
      this->tabControl_TEPS->Controls->Add(this->label_RightTorqueEffectiveness);
      this->tabControl_TEPS->Controls->Add(this->label_LeftTorqueEffectiveness);
      this->tabControl_TEPS->Controls->Add(this->label11);
      this->tabControl_TEPS->Controls->Add(this->label12);
      this->tabControl_TEPS->Controls->Add(this->label9);
      this->tabControl_TEPS->Controls->Add(this->label8);
      this->tabControl_TEPS->Location = System::Drawing::Point(4, 22);
      this->tabControl_TEPS->Name = L"tabControl_TEPS";
      this->tabControl_TEPS->Padding = System::Windows::Forms::Padding(3);
      this->tabControl_TEPS->Size = System::Drawing::Size(389, 111);
      this->tabControl_TEPS->TabIndex = 8;
      this->tabControl_TEPS->Text = L"TE&PS";
      this->tabControl_TEPS->UseVisualStyleBackColor = true;
      // 
      // label8
      // 
      this->label8->AutoSize = true;
      this->label8->Location = System::Drawing::Point(43, 15);
      this->label8->Name = L"label8";
      this->label8->Size = System::Drawing::Size(152, 13);
      this->label8->TabIndex = 0;
      this->label8->Text = L"Left Torque Effectiveness (%): ";
      // 
      // label9
      // 
      this->label9->AutoSize = true;
      this->label9->Location = System::Drawing::Point(36, 35);
      this->label9->Name = L"label9";
      this->label9->Size = System::Drawing::Size(159, 13);
      this->label9->TabIndex = 1;
      this->label9->Text = L"Right Torque Effectiveness (%): ";
      // 
      // label11
      // 
      this->label11->AutoSize = true;
      this->label11->Location = System::Drawing::Point(52, 75);
      this->label11->Name = L"label11";
      this->label11->Size = System::Drawing::Size(143, 13);
      this->label11->TabIndex = 3;
      this->label11->Text = L"Right Pedal Smoothness (%):";
      // 
      // label12
      // 
      this->label12->AutoSize = true;
      this->label12->Location = System::Drawing::Point(7, 55);
      this->label12->Name = L"label12";
      this->label12->Size = System::Drawing::Size(188, 13);
      this->label12->TabIndex = 2;
      this->label12->Text = L"Left/Combined Pedal Smoothness (%):";
      // 
      // label_LeftTorqueEffectiveness
      // 
      this->label_LeftTorqueEffectiveness->AutoSize = true;
      this->label_LeftTorqueEffectiveness->Location = System::Drawing::Point(202, 14);
      this->label_LeftTorqueEffectiveness->Name = L"label_LeftTorqueEffectiveness";
      this->label_LeftTorqueEffectiveness->Size = System::Drawing::Size(13, 13);
      this->label_LeftTorqueEffectiveness->TabIndex = 4;
      this->label_LeftTorqueEffectiveness->Text = L"--";
      // 
      // label_RightTorqueEffectiveness
      // 
      this->label_RightTorqueEffectiveness->AutoSize = true;
      this->label_RightTorqueEffectiveness->Location = System::Drawing::Point(202, 35);
      this->label_RightTorqueEffectiveness->Name = L"label_RightTorqueEffectiveness";
      this->label_RightTorqueEffectiveness->Size = System::Drawing::Size(13, 13);
      this->label_RightTorqueEffectiveness->TabIndex = 5;
      this->label_RightTorqueEffectiveness->Text = L"--";
      // 
      // label_LeftPedalSmoothness
      // 
      this->label_LeftPedalSmoothness->AutoSize = true;
      this->label_LeftPedalSmoothness->Location = System::Drawing::Point(202, 55);
      this->label_LeftPedalSmoothness->Name = L"label_LeftPedalSmoothness";
      this->label_LeftPedalSmoothness->Size = System::Drawing::Size(13, 13);
      this->label_LeftPedalSmoothness->TabIndex = 6;
      this->label_LeftPedalSmoothness->Text = L"--";
      // 
      // label_RightPedalSmoothness
      // 
      this->label_RightPedalSmoothness->AutoSize = true;
      this->label_RightPedalSmoothness->Location = System::Drawing::Point(202, 75);
      this->label_RightPedalSmoothness->Name = L"label_RightPedalSmoothness";
      this->label_RightPedalSmoothness->Size = System::Drawing::Size(13, 13);
      this->label_RightPedalSmoothness->TabIndex = 7;
      this->label_RightPedalSmoothness->Text = L"--";
      // 
      // BikePowerDisplay
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(794, 351);
      this->Controls->Add(this->panel_Display);
      this->Controls->Add(this->panel_Settings);
      this->Name = L"BikePowerDisplay";
      this->Text = L"BikePowerDisplay";
      this->panel_Settings->ResumeLayout(false);
      this->tabControl_Settings->ResumeLayout(false);
      this->tabPage_Data->ResumeLayout(false);
      this->tabPage_Data->PerformLayout();
      this->tabPage_Calibration->ResumeLayout(false);
      this->tabPage_Calibration->PerformLayout();
      this->panel1->ResumeLayout(false);
      this->panel1->PerformLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Cal_CTFSlope))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Cal_CTFSerial))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Cal_WheelCircum))->EndInit();
      this->tabPage_GlobalBattery->ResumeLayout(false);
      this->tabPage_GlobalBattery->PerformLayout();
      this->tabPage1->ResumeLayout(false);
      this->tabPage1->PerformLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_TransResponse))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_CrankLength))->EndInit();
      this->panel_Display->ResumeLayout(false);
      this->panel_Display->PerformLayout();
      this->tabControl_TEPS->ResumeLayout(false);
      this->tabControl_TEPS->PerformLayout();
      this->ResumeLayout(false);

   }
#pragma endregion
};