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


public ref class BikePowerSensor : public System::Windows::Forms::Form, public ISimBase{
public:
	BikePowerSensor(System::Timers::Timer^ channelTimer, dRequestAckMsg^ channelAckMsg){
		requestAckMsg = channelAckMsg;
		timerHandle = channelTimer;
		commonPages = gcnew CommonData();
		bpPages = gcnew BikePower();
		InitializeComponent();
		InitializeSim();
	}

	~BikePowerSensor(){
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
	virtual UCHAR getDeviceType(){return BikePower::DEVICE_TYPE;}
	virtual UCHAR getTransmissionType(){return BikePower::TX_TYPE;}
	virtual USHORT getTransmitPeriod(){return BikePower::MSG_PERIOD;}
	virtual DOUBLE getTimerInterval(){return (double) ulTimerInterval;}
	virtual System::Windows::Forms::Panel^ getSimSettingsPanel(){return this->panel_Settings;}
	virtual System::Windows::Forms::Panel^ getSimTranslatedDisplay(){return this->panel_Display;}

private:
	void InitializeSim();
	void HandleTransmit(UCHAR* pucTxBuffer_);
	void HandleRequest(UCHAR* pucRxBuffer_);
	void HandleReceive(UCHAR* pucRxBuffer_);
	void HandleCalibration(UCHAR* pucRxBuffer_);
	void SendCalibrationResponse(UCHAR ucMsgCode_);
	void UpdateDisplay();
	void UpdateCalDisplay();
	void ForceUpdate();
	System::Void radioButton_SensorType_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_Ppg_BasicPowerInterlvTime_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void radioButton_Ppg_UpdateType_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_Ppg_TimerEventFreq_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void button_GlobalDataUpdate_Click(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_Bat_Status_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void radioButton_Bat_ElpUnits_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void button_ElpTimeUpdate_Click(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_Bat_VoltInt_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_Bat_VoltFrac_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_Bat_Voltage_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_Bat_FracVolt_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void listBox_Bat_Status_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_Sim_Sweeping_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_Sim_CrankTorque_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_Sim_WheelTorque_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_Sim_CrankGearTeeth_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_Sim_WheelGearTeeth_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_Ppg_SlopeConstant_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_Sim_WheelCircumference_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_Sim_MinMaxOutput_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_Sim_CurOutput_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_Sim_Coast_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_Sim_Stop_CheckedChanged(System::Object^  sender, System::EventArgs^  e);	
	System::Void radioButton_SimByChanged(System::Object^  sender, System::EventArgs^  e);	
	System::Void checkBox_Cal_TorqAZMesg_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void listBox_Cal_AZStatus_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_Cal_RawTorque_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_Cal_OffsetTorque_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_Cal_CalNumber_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void radioButton_Cal_Success_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_Ppg_TxCadence_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_Ppg_TxPedalPwr_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_PedalPwr_CurrOutput_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_RightPedal_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_InvalidSerial_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void button_UpdateGetSet_Click(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_InvalidCrankLength_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
   System::Void checkBox_LeftTorqueEffectivenessInvalid_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
   System::Void checkBox_RightTorqueEffectivenessInvalid_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
   System::Void checkBox_LeftPedalSmoothnessInvalid_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
   System::Void checkBox_RightPedalSmoothnessInvalid_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
   System::Void checkBox_Combined_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
   System::Void checkBox_EnableTEPS_CheckedChanged(System::Object^  sender, System::EventArgs^  e); 
   System::Void numericUpDown_LeftTorqueEffectiveness_ValueChanged(System::Object^  sender, System::EventArgs^  e);
   System::Void numericUpDown_RightTorqueEffectiveness_ValueChanged(System::Object^  sender, System::EventArgs^  e);
   System::Void numericUpDown_LeftPedalSmoothness_ValueChanged(System::Object^  sender, System::EventArgs^  e);
   System::Void numericUpDown_RightPedalSmoothness_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	void CalculateWheelTorque();
	void CalculateCrankTorque();
	ULONG SpeedToCadence(ULONG ulSpeed_);
	ULONG CadenceToSpeed(ULONG ulCadence_);


private:
	static const USHORT PI256 = 804;		// Pi * 256
	static const UCHAR MAX_RETRY = 40;		// Maximum retransmissions for ack'ed messages

	dRequestAckMsg^ requestAckMsg;		// Delegate for handling Ack data
	CommonData^ commonPages;			// Common data pages handle
	BikePower^ bpPages;					// Bike power data pages handle
	System::Timers::Timer^ timerHandle;	// Timer handle
	ULONG ulTimerInterval;				// Timer interval

	// Simulation settings
	UCHAR ucSimDataType;				// Type of simulation (fixed/sweeping)
	BOOL bSweepAscending;				// Sweep ascending (if enabled)
	BOOL bStop;							// Bike stopped
	BOOL bCoast;						// Bike is coasting
	BOOL bByCadence;					// Base simulation on cadence, in rpm
	UCHAR ucCurPedalPwrValue;			// Currnet pedal power depending on settings
	UCHAR ucNumCrankGearTeeth;			// Crank gear-teeth
	UCHAR ucNumWheelGearTeeth;			// Wheel gear-teeth
	UCHAR ucWheelCircumference100;		// Wheel circumference (cm)
	USHORT usCrankTorque32;				// Crank torque (1/32 Nm)
	USHORT usWheelTorque32;				// Wheel torque (1/32 Nm)
	ULONG ulCurValue1000;				// Current speed (1/1000 km/h, that is, m/h), or current cadence (1/1000 rpm), depending on settings
	ULONG ulMinValue1000;				// Min speed or cadence (m/h or rpm)
	ULONG ulMaxValue1000;				// Max speed or cadence (m/h or rpm)
	ULONG ulSpeed1000;					// Speed (m/h)
	ULONG ulEventTime1000;				// Event time (in ms)
	USHORT usCalOffset;					// Calibration offset to send (CTF sensors)
	SHORT sCalData;						// Calibration data to send (other sensors)
	BOOL bCalSuccess;					   // Report next calibration as success or failure
	BOOL bUpdateTicks;
	BOOL bUpdate;
	

	// Paging
	BOOL bTxPage82;						// Transmit battery global page
	BOOL bTxAZ;							   // Transmit auto zero support message
	BOOL bTxCadence;					   // Include instantaneous crank cadence in transmission
	BOOL bTxPedalPower;					// Include pedal power in transmission
	BOOL bRightPedal;					   // Include pedal side contribution to pedal power
   BOOL bTxPage19;                  // Transmit torque effectiveness & pedal smoothness page
	UCHAR ucPowerInterleave;			// Interleaving interval for basic power page
	UCHAR ucEventFreq10;				   // Event frequency (time synchronous) in 1/10Hz
	BikePower::UpdateType eUpdateType;	// Type of updates (event/time synch)
	UCHAR ucAckRetryCount;				// Number of retries for an acknowledged message
	UCHAR ucMsgExpectingAck;			// Message pending for retransmission
	UCHAR ucBcastMessage;
	UCHAR ucTransmissionCount;
	UCHAR ucBcastCount;

	// Common Pages
	ULONG ulRunTime1000;				// Run time (in ms)


private: System::Windows::Forms::Panel^  panel_Settings;
private: System::Windows::Forms::TabControl^  tabControl_Settings;
private: System::Windows::Forms::TabPage^  tabPage_Simulation;
private: System::Windows::Forms::TabPage^  tabPage_GlobalData;
private: System::Windows::Forms::Panel^  panel_Display;
private: System::Windows::Forms::Label^  label_Sim_Max;
private: System::Windows::Forms::Label^  label_Sim_Current;
private: System::Windows::Forms::Label^  label_Sim_Min;
private: System::Windows::Forms::Label^  label_Trn_EventCount;
private: System::Windows::Forms::Label^  label_Trn_InstSpeed;
private: System::Windows::Forms::Label^  label_Trn_InstSpeedDisplay;
private: System::Windows::Forms::Label^  label_Trn_EventCountDisplay;
private: System::Windows::Forms::Label^  label_Glb_HardwareVer;
private: System::Windows::Forms::Label^  label_Glb_SerialNum;
private: System::Windows::Forms::Label^  label_Glb_ManfID;
private: System::Windows::Forms::Label^  label_Glb_SoftwareVer;
private: System::Windows::Forms::Label^  label_Glb_ModelNum;
private: System::Windows::Forms::Button^  button_Glb_GlobalDataUpdate;
private: System::Windows::Forms::TextBox^  textBox_Glb_ModelNumChange;
private: System::Windows::Forms::TextBox^  textBox_Glb_SerialNumChange;
private: System::Windows::Forms::TextBox^  textBox_Glb_ManfIDChange;
private: System::Windows::Forms::TextBox^  textBox_Glb_SoftwareVerChange;
private: System::Windows::Forms::TextBox^  textBox_Glb_HardwareVerChange;
private: System::Windows::Forms::Label^  label_Trn_DisplayTitle;
private: System::Windows::Forms::Label^  label_Glb_GlobalDataError;
private: System::Windows::Forms::RadioButton^  radioButton_Sim_SimByCadence;
private: System::Windows::Forms::CheckBox^  checkBox_Sim_Sweeping;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Sim_WheelCircumference;
private: System::Windows::Forms::Label^  label_Sim_WheelCircumferenceConst;
private: System::Windows::Forms::Label^  label_Trn_AccumOneDisplay;
private: System::Windows::Forms::Label^  label_Trn_AccumOne;
private: System::Windows::Forms::Label^  label_Trn_CadenceDisplay;
private: System::Windows::Forms::Label^  label_Trn_Cadence;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Sim_CrankGearTeeth;
private: System::Windows::Forms::Label^  label_Sim_Wheel;
private: System::Windows::Forms::Label^  label_Sim_Crank;
private: System::Windows::Forms::Label^  label_Sim_GearTeeth;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Sim_WheelGearTeeth;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Sim_CrankTorque;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Sim_WheelTorque;
private: System::Windows::Forms::Label^  label_Sim_Torque;
private: System::Windows::Forms::RadioButton^  radioButton_Sim_SimBySpeed;
private: System::Windows::Forms::TabPage^  tabPage_PowerPaging;
private: System::Windows::Forms::TabPage^  tabPage_Calibration;
private: System::Windows::Forms::TabPage^  tabPage_GlobalBattery;
private: System::Windows::Forms::GroupBox^  groupBox_Ppg_RaiseEventsSelect;
private: System::Windows::Forms::RadioButton^  radioButton_Ppg_EventOnTime;
private: System::Windows::Forms::RadioButton^  radioButton_Ppg_EventOnRotation;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Ppg_BasicPowerInterlvTime;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Ppg_TimerEventFreq;
private: System::Windows::Forms::CheckBox^  checkBox_Bat_Status;
private: System::Windows::Forms::TextBox^  textBox_Bat_ElpTimeChange;
private: System::Windows::Forms::Button^  button_Bat_ElpTimeUpdate;
private: System::Windows::Forms::Label^  label_Bat_ElpTimeDisplay;
private: System::Windows::Forms::Label^  label_Bat_ElpTime;
private: System::Windows::Forms::RadioButton^  radioButton_Bat_Elp16Units;
private: System::Windows::Forms::RadioButton^ radioButton_Bat_Elp2Units;
private: System::Windows::Forms::Label^  label_Bat_Status;
private: System::Windows::Forms::ListBox^  listBox_Bat_Status;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Bat_VoltFrac;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Bat_VoltInt;
private: System::Windows::Forms::Label^  label_Bat_Volts;
private: System::Windows::Forms::CheckBox^  checkBox_Bat_FracVolt;
private: System::Windows::Forms::CheckBox^  checkBox_Bat_Voltage;
private: System::Windows::Forms::Label^  label_Cal_CalSuccess;
private: System::Windows::Forms::RadioButton^  radioButton_Cal_Failure;
private: System::Windows::Forms::RadioButton^  radioButton_Cal_Success;
private: System::Windows::Forms::Label^  label_Cal_CalNum;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Cal_CalNumber;
private: System::Windows::Forms::Label^  label_Cal_AZStatus;
private: System::Windows::Forms::ListBox^  listBox_Cal_AZStatus;
private: System::Windows::Forms::CheckBox^  checkBox_Cal_TorqAZMesg;
private: System::Windows::Forms::Label^  label_Cal_TorqOffset;
private: System::Windows::Forms::Label^  label_Cal_RawTorq;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Cal_OffsetTorque;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Cal_RawTorque;
private: System::Windows::Forms::Label^  label_Trn_PowerDisplay;
private: System::Windows::Forms::Label^  label_Trn_Power;
private: System::Windows::Forms::Label^  label_Trn_UpdateCountDisplay;
private: System::Windows::Forms::Label^  label_Trn_UpdateCount;
private: System::Windows::Forms::Label^  label_Ppg_SlopeConstant;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Ppg_SlopeConstant;
private: System::Windows::Forms::Label^  label_Trn_AccumTitle;
private: System::Windows::Forms::Label^  label_Trn_AccumTwo;
private: System::Windows::Forms::Label^  label_Trn_AccumTwoDisplay;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Sim_MinOutput;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Sim_MaxOutput;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Sim_CurOutput;
private: System::Windows::Forms::CheckBox^  checkBox_Sim_Stop;
private: System::Windows::Forms::CheckBox^  checkBox_Sim_Coast;
private: System::Windows::Forms::GroupBox^  groupBox_Ppg_SendPageSelect;
private: System::Windows::Forms::RadioButton^  radioButton_WheelTorque;
private: System::Windows::Forms::RadioButton^  radioButton_CTF;
private: System::Windows::Forms::RadioButton^  radioButton_Power;
private: System::Windows::Forms::RadioButton^  radioButton_CrankTorque;
private: System::Windows::Forms::Label^  label1;
private: System::Windows::Forms::Label^  label_Volt_Coarse;
private: System::Windows::Forms::Label^  label_Voltage_Display;
private: System::Windows::Forms::Label^  label_Volts;
private: System::Windows::Forms::GroupBox^  groupBox_Resol;
private: System::Windows::Forms::CheckBox^  checkBox_Ppg_TxCadence;
private: System::Windows::Forms::CheckBox^  checkBox_Ppg_TxPedalPwr;
private: System::Windows::Forms::Label^  label_Trn_PedalPwr;
private: System::Windows::Forms::Label^  label_Trn_PedalPwrDisplay;
private: System::Windows::Forms::Label^  label_PedalPwr;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_PedalPwr_CurrOutput;
private: System::Windows::Forms::CheckBox^  checkBox_RightPedal;
private: System::Windows::Forms::Label^  label_Trn_PedalDisplay;
private: System::Windows::Forms::CheckBox^  checkBox_InvalidSerial;
private: System::Windows::Forms::TabPage^  tabPage_GetSet;
private: System::Windows::Forms::GroupBox^  groupBox3;
private: System::Windows::Forms::RadioButton^  radioButton_CrankAuto;
private: System::Windows::Forms::RadioButton^  radioButton_CrankManual;
private: System::Windows::Forms::RadioButton^  radioButton_CrankDefault;
private: System::Windows::Forms::RadioButton^  radioButton_CrankInvalid;
private: System::Windows::Forms::GroupBox^  groupBox2;
private: System::Windows::Forms::RadioButton^  radioButton_MismatchNone;
private: System::Windows::Forms::RadioButton^  radioButton_MismatchLeft;
private: System::Windows::Forms::RadioButton^  radioButton_MismatchRight;
private: System::Windows::Forms::RadioButton^  radioButton_MismatchUndefined;
private: System::Windows::Forms::GroupBox^  groupBox1;
private: System::Windows::Forms::RadioButton^  radioButton_SensorBoth;
private: System::Windows::Forms::RadioButton^  radioButton_SensorRight;
private: System::Windows::Forms::RadioButton^  radioButton_SensorLeft;
private: System::Windows::Forms::RadioButton^  radioButton_SensorUndefined;
private: System::Windows::Forms::CheckBox^  checkBox_AutoCrank;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_CrankLength;
private: System::Windows::Forms::CheckBox^  checkBox_InvalidCrankLength;
private: System::Windows::Forms::Label^  label2;
private: System::Windows::Forms::Button^  button_UpdateGetSet;
private: System::Windows::Forms::TabPage^  tabControl_TEPS;

private: System::Windows::Forms::CheckBox^  checkBox_EnableTEPS;
private: System::Windows::Forms::CheckBox^  checkBox_Combined;
private: System::Windows::Forms::GroupBox^  groupBox4;
private: System::Windows::Forms::CheckBox^  checkBox_LeftTorqueEffectivenessInvalid;

private: System::Windows::Forms::NumericUpDown^  numericUpDown_LeftTorqueEffectiveness;
private: System::Windows::Forms::Label^  label3;
private: System::Windows::Forms::CheckBox^  checkBox_RightTorqueEffectivenessInvalid;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_RightTorqueEffectiveness;
private: System::Windows::Forms::Label^  label4;
private: System::Windows::Forms::GroupBox^  groupBox5;
private: System::Windows::Forms::CheckBox^  checkBox_RightPedalSmoothnessInvalid;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_RightPedalSmoothness;


private: System::Windows::Forms::Label^  label5;
private: System::Windows::Forms::CheckBox^  checkBox_LeftPedalSmoothnessInvalid;

private: System::Windows::Forms::NumericUpDown^  numericUpDown_LeftPedalSmoothness;

private: System::Windows::Forms::Label^  label6;


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
      this->tabPage_Simulation = (gcnew System::Windows::Forms::TabPage());
      this->checkBox_RightPedal = (gcnew System::Windows::Forms::CheckBox());
      this->numericUpDown_PedalPwr_CurrOutput = (gcnew System::Windows::Forms::NumericUpDown());
      this->label_PedalPwr = (gcnew System::Windows::Forms::Label());
      this->checkBox_Sim_Coast = (gcnew System::Windows::Forms::CheckBox());
      this->label_Sim_GearTeeth = (gcnew System::Windows::Forms::Label());
      this->label_Sim_Torque = (gcnew System::Windows::Forms::Label());
      this->checkBox_Sim_Stop = (gcnew System::Windows::Forms::CheckBox());
      this->label_Sim_WheelCircumferenceConst = (gcnew System::Windows::Forms::Label());
      this->checkBox_Sim_Sweeping = (gcnew System::Windows::Forms::CheckBox());
      this->numericUpDown_Sim_WheelCircumference = (gcnew System::Windows::Forms::NumericUpDown());
      this->numericUpDown_Sim_MaxOutput = (gcnew System::Windows::Forms::NumericUpDown());
      this->radioButton_Sim_SimBySpeed = (gcnew System::Windows::Forms::RadioButton());
      this->radioButton_Sim_SimByCadence = (gcnew System::Windows::Forms::RadioButton());
      this->numericUpDown_Sim_CurOutput = (gcnew System::Windows::Forms::NumericUpDown());
      this->label_Sim_Crank = (gcnew System::Windows::Forms::Label());
      this->numericUpDown_Sim_MinOutput = (gcnew System::Windows::Forms::NumericUpDown());
      this->label_Sim_Max = (gcnew System::Windows::Forms::Label());
      this->numericUpDown_Sim_WheelGearTeeth = (gcnew System::Windows::Forms::NumericUpDown());
      this->label_Sim_Current = (gcnew System::Windows::Forms::Label());
      this->label_Sim_Min = (gcnew System::Windows::Forms::Label());
      this->numericUpDown_Sim_CrankTorque = (gcnew System::Windows::Forms::NumericUpDown());
      this->label_Sim_Wheel = (gcnew System::Windows::Forms::Label());
      this->numericUpDown_Sim_CrankGearTeeth = (gcnew System::Windows::Forms::NumericUpDown());
      this->numericUpDown_Sim_WheelTorque = (gcnew System::Windows::Forms::NumericUpDown());
      this->tabPage_PowerPaging = (gcnew System::Windows::Forms::TabPage());
      this->checkBox_Ppg_TxPedalPwr = (gcnew System::Windows::Forms::CheckBox());
      this->checkBox_Ppg_TxCadence = (gcnew System::Windows::Forms::CheckBox());
      this->numericUpDown_Ppg_SlopeConstant = (gcnew System::Windows::Forms::NumericUpDown());
      this->groupBox_Ppg_SendPageSelect = (gcnew System::Windows::Forms::GroupBox());
      this->radioButton_WheelTorque = (gcnew System::Windows::Forms::RadioButton());
      this->radioButton_CTF = (gcnew System::Windows::Forms::RadioButton());
      this->radioButton_Power = (gcnew System::Windows::Forms::RadioButton());
      this->radioButton_CrankTorque = (gcnew System::Windows::Forms::RadioButton());
      this->label1 = (gcnew System::Windows::Forms::Label());
      this->numericUpDown_Ppg_BasicPowerInterlvTime = (gcnew System::Windows::Forms::NumericUpDown());
      this->label_Ppg_SlopeConstant = (gcnew System::Windows::Forms::Label());
      this->groupBox_Ppg_RaiseEventsSelect = (gcnew System::Windows::Forms::GroupBox());
      this->numericUpDown_Ppg_TimerEventFreq = (gcnew System::Windows::Forms::NumericUpDown());
      this->radioButton_Ppg_EventOnTime = (gcnew System::Windows::Forms::RadioButton());
      this->radioButton_Ppg_EventOnRotation = (gcnew System::Windows::Forms::RadioButton());
      this->tabPage_Calibration = (gcnew System::Windows::Forms::TabPage());
      this->checkBox_Cal_TorqAZMesg = (gcnew System::Windows::Forms::CheckBox());
      this->label_Cal_TorqOffset = (gcnew System::Windows::Forms::Label());
      this->label_Cal_RawTorq = (gcnew System::Windows::Forms::Label());
      this->label_Cal_AZStatus = (gcnew System::Windows::Forms::Label());
      this->numericUpDown_Cal_RawTorque = (gcnew System::Windows::Forms::NumericUpDown());
      this->listBox_Cal_AZStatus = (gcnew System::Windows::Forms::ListBox());
      this->label_Cal_CalNum = (gcnew System::Windows::Forms::Label());
      this->numericUpDown_Cal_OffsetTorque = (gcnew System::Windows::Forms::NumericUpDown());
      this->numericUpDown_Cal_CalNumber = (gcnew System::Windows::Forms::NumericUpDown());
      this->label_Cal_CalSuccess = (gcnew System::Windows::Forms::Label());
      this->radioButton_Cal_Failure = (gcnew System::Windows::Forms::RadioButton());
      this->radioButton_Cal_Success = (gcnew System::Windows::Forms::RadioButton());
      this->tabPage_GlobalData = (gcnew System::Windows::Forms::TabPage());
      this->checkBox_InvalidSerial = (gcnew System::Windows::Forms::CheckBox());
      this->button_Glb_GlobalDataUpdate = (gcnew System::Windows::Forms::Button());
      this->label_Glb_GlobalDataError = (gcnew System::Windows::Forms::Label());
      this->textBox_Glb_HardwareVerChange = (gcnew System::Windows::Forms::TextBox());
      this->textBox_Glb_ModelNumChange = (gcnew System::Windows::Forms::TextBox());
      this->label_Glb_HardwareVer = (gcnew System::Windows::Forms::Label());
      this->label_Glb_ManfID = (gcnew System::Windows::Forms::Label());
      this->textBox_Glb_ManfIDChange = (gcnew System::Windows::Forms::TextBox());
      this->label_Glb_SoftwareVer = (gcnew System::Windows::Forms::Label());
      this->label_Glb_ModelNum = (gcnew System::Windows::Forms::Label());
      this->textBox_Glb_SerialNumChange = (gcnew System::Windows::Forms::TextBox());
      this->textBox_Glb_SoftwareVerChange = (gcnew System::Windows::Forms::TextBox());
      this->label_Glb_SerialNum = (gcnew System::Windows::Forms::Label());
      this->tabPage_GlobalBattery = (gcnew System::Windows::Forms::TabPage());
      this->groupBox_Resol = (gcnew System::Windows::Forms::GroupBox());
      this->radioButton_Bat_Elp2Units = (gcnew System::Windows::Forms::RadioButton());
      this->radioButton_Bat_Elp16Units = (gcnew System::Windows::Forms::RadioButton());
      this->label_Volt_Coarse = (gcnew System::Windows::Forms::Label());
      this->button_Bat_ElpTimeUpdate = (gcnew System::Windows::Forms::Button());
      this->label_Voltage_Display = (gcnew System::Windows::Forms::Label());
      this->textBox_Bat_ElpTimeChange = (gcnew System::Windows::Forms::TextBox());
      this->checkBox_Bat_FracVolt = (gcnew System::Windows::Forms::CheckBox());
      this->label_Bat_ElpTimeDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Bat_Volts = (gcnew System::Windows::Forms::Label());
      this->label_Bat_ElpTime = (gcnew System::Windows::Forms::Label());
      this->label_Volts = (gcnew System::Windows::Forms::Label());
      this->checkBox_Bat_Voltage = (gcnew System::Windows::Forms::CheckBox());
      this->numericUpDown_Bat_VoltFrac = (gcnew System::Windows::Forms::NumericUpDown());
      this->numericUpDown_Bat_VoltInt = (gcnew System::Windows::Forms::NumericUpDown());
      this->label_Bat_Status = (gcnew System::Windows::Forms::Label());
      this->listBox_Bat_Status = (gcnew System::Windows::Forms::ListBox());
      this->checkBox_Bat_Status = (gcnew System::Windows::Forms::CheckBox());
      this->tabPage_GetSet = (gcnew System::Windows::Forms::TabPage());
      this->button_UpdateGetSet = (gcnew System::Windows::Forms::Button());
      this->checkBox_InvalidCrankLength = (gcnew System::Windows::Forms::CheckBox());
      this->label2 = (gcnew System::Windows::Forms::Label());
      this->numericUpDown_CrankLength = (gcnew System::Windows::Forms::NumericUpDown());
      this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
      this->radioButton_CrankAuto = (gcnew System::Windows::Forms::RadioButton());
      this->radioButton_CrankDefault = (gcnew System::Windows::Forms::RadioButton());
      this->radioButton_CrankInvalid = (gcnew System::Windows::Forms::RadioButton());
      this->radioButton_CrankManual = (gcnew System::Windows::Forms::RadioButton());
      this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
      this->radioButton_MismatchLeft = (gcnew System::Windows::Forms::RadioButton());
      this->radioButton_MismatchRight = (gcnew System::Windows::Forms::RadioButton());
      this->radioButton_MismatchUndefined = (gcnew System::Windows::Forms::RadioButton());
      this->radioButton_MismatchNone = (gcnew System::Windows::Forms::RadioButton());
      this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
      this->radioButton_SensorBoth = (gcnew System::Windows::Forms::RadioButton());
      this->radioButton_SensorLeft = (gcnew System::Windows::Forms::RadioButton());
      this->radioButton_SensorUndefined = (gcnew System::Windows::Forms::RadioButton());
      this->radioButton_SensorRight = (gcnew System::Windows::Forms::RadioButton());
      this->checkBox_AutoCrank = (gcnew System::Windows::Forms::CheckBox());
      this->tabControl_TEPS = (gcnew System::Windows::Forms::TabPage());
      this->groupBox5 = (gcnew System::Windows::Forms::GroupBox());
      this->checkBox_RightPedalSmoothnessInvalid = (gcnew System::Windows::Forms::CheckBox());
      this->numericUpDown_RightPedalSmoothness = (gcnew System::Windows::Forms::NumericUpDown());
      this->label5 = (gcnew System::Windows::Forms::Label());
      this->checkBox_LeftPedalSmoothnessInvalid = (gcnew System::Windows::Forms::CheckBox());
      this->numericUpDown_LeftPedalSmoothness = (gcnew System::Windows::Forms::NumericUpDown());
      this->label6 = (gcnew System::Windows::Forms::Label());
      this->groupBox4 = (gcnew System::Windows::Forms::GroupBox());
      this->checkBox_RightTorqueEffectivenessInvalid = (gcnew System::Windows::Forms::CheckBox());
      this->numericUpDown_RightTorqueEffectiveness = (gcnew System::Windows::Forms::NumericUpDown());
      this->label4 = (gcnew System::Windows::Forms::Label());
      this->checkBox_LeftTorqueEffectivenessInvalid = (gcnew System::Windows::Forms::CheckBox());
      this->numericUpDown_LeftTorqueEffectiveness = (gcnew System::Windows::Forms::NumericUpDown());
      this->label3 = (gcnew System::Windows::Forms::Label());
      this->checkBox_Combined = (gcnew System::Windows::Forms::CheckBox());
      this->checkBox_EnableTEPS = (gcnew System::Windows::Forms::CheckBox());
      this->panel_Display = (gcnew System::Windows::Forms::Panel());
      this->label_Trn_PedalDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Trn_PedalPwrDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Trn_PedalPwr = (gcnew System::Windows::Forms::Label());
      this->label_Trn_UpdateCountDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Trn_UpdateCount = (gcnew System::Windows::Forms::Label());
      this->label_Trn_EventCount = (gcnew System::Windows::Forms::Label());
      this->label_Trn_AccumOneDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Trn_AccumTwo = (gcnew System::Windows::Forms::Label());
      this->label_Trn_PowerDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Trn_Power = (gcnew System::Windows::Forms::Label());
      this->label_Trn_AccumTwoDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Trn_CadenceDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Trn_Cadence = (gcnew System::Windows::Forms::Label());
      this->label_Trn_InstSpeedDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Trn_EventCountDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Trn_InstSpeed = (gcnew System::Windows::Forms::Label());
      this->label_Trn_DisplayTitle = (gcnew System::Windows::Forms::Label());
      this->label_Trn_AccumOne = (gcnew System::Windows::Forms::Label());
      this->label_Trn_AccumTitle = (gcnew System::Windows::Forms::Label());
      this->panel_Settings->SuspendLayout();
      this->tabControl_Settings->SuspendLayout();
      this->tabPage_Simulation->SuspendLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_PedalPwr_CurrOutput))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_WheelCircumference))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_MaxOutput))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_CurOutput))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_MinOutput))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_WheelGearTeeth))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_CrankTorque))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_CrankGearTeeth))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_WheelTorque))->BeginInit();
      this->tabPage_PowerPaging->SuspendLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Ppg_SlopeConstant))->BeginInit();
      this->groupBox_Ppg_SendPageSelect->SuspendLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Ppg_BasicPowerInterlvTime))->BeginInit();
      this->groupBox_Ppg_RaiseEventsSelect->SuspendLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Ppg_TimerEventFreq))->BeginInit();
      this->tabPage_Calibration->SuspendLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Cal_RawTorque))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Cal_OffsetTorque))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Cal_CalNumber))->BeginInit();
      this->tabPage_GlobalData->SuspendLayout();
      this->tabPage_GlobalBattery->SuspendLayout();
      this->groupBox_Resol->SuspendLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Bat_VoltFrac))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Bat_VoltInt))->BeginInit();
      this->tabPage_GetSet->SuspendLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_CrankLength))->BeginInit();
      this->groupBox3->SuspendLayout();
      this->groupBox2->SuspendLayout();
      this->groupBox1->SuspendLayout();
      this->tabControl_TEPS->SuspendLayout();
      this->groupBox5->SuspendLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_RightPedalSmoothness))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_LeftPedalSmoothness))->BeginInit();
      this->groupBox4->SuspendLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_RightTorqueEffectiveness))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_LeftTorqueEffectiveness))->BeginInit();
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
      this->tabControl_Settings->Controls->Add(this->tabPage_Simulation);
      this->tabControl_Settings->Controls->Add(this->tabPage_PowerPaging);
      this->tabControl_Settings->Controls->Add(this->tabPage_Calibration);
      this->tabControl_Settings->Controls->Add(this->tabPage_GlobalData);
      this->tabControl_Settings->Controls->Add(this->tabPage_GlobalBattery);
      this->tabControl_Settings->Controls->Add(this->tabPage_GetSet);
      this->tabControl_Settings->Controls->Add(this->tabControl_TEPS);
      this->tabControl_Settings->Location = System::Drawing::Point(0, 3);
      this->tabControl_Settings->Name = L"tabControl_Settings";
      this->tabControl_Settings->SelectedIndex = 0;
      this->tabControl_Settings->Size = System::Drawing::Size(397, 137);
      this->tabControl_Settings->TabIndex = 10;
      // 
      // tabPage_Simulation
      // 
      this->tabPage_Simulation->Controls->Add(this->checkBox_RightPedal);
      this->tabPage_Simulation->Controls->Add(this->numericUpDown_PedalPwr_CurrOutput);
      this->tabPage_Simulation->Controls->Add(this->label_PedalPwr);
      this->tabPage_Simulation->Controls->Add(this->checkBox_Sim_Coast);
      this->tabPage_Simulation->Controls->Add(this->label_Sim_GearTeeth);
      this->tabPage_Simulation->Controls->Add(this->label_Sim_Torque);
      this->tabPage_Simulation->Controls->Add(this->checkBox_Sim_Stop);
      this->tabPage_Simulation->Controls->Add(this->label_Sim_WheelCircumferenceConst);
      this->tabPage_Simulation->Controls->Add(this->checkBox_Sim_Sweeping);
      this->tabPage_Simulation->Controls->Add(this->numericUpDown_Sim_WheelCircumference);
      this->tabPage_Simulation->Controls->Add(this->numericUpDown_Sim_MaxOutput);
      this->tabPage_Simulation->Controls->Add(this->radioButton_Sim_SimBySpeed);
      this->tabPage_Simulation->Controls->Add(this->radioButton_Sim_SimByCadence);
      this->tabPage_Simulation->Controls->Add(this->numericUpDown_Sim_CurOutput);
      this->tabPage_Simulation->Controls->Add(this->label_Sim_Crank);
      this->tabPage_Simulation->Controls->Add(this->numericUpDown_Sim_MinOutput);
      this->tabPage_Simulation->Controls->Add(this->label_Sim_Max);
      this->tabPage_Simulation->Controls->Add(this->numericUpDown_Sim_WheelGearTeeth);
      this->tabPage_Simulation->Controls->Add(this->label_Sim_Current);
      this->tabPage_Simulation->Controls->Add(this->label_Sim_Min);
      this->tabPage_Simulation->Controls->Add(this->numericUpDown_Sim_CrankTorque);
      this->tabPage_Simulation->Controls->Add(this->label_Sim_Wheel);
      this->tabPage_Simulation->Controls->Add(this->numericUpDown_Sim_CrankGearTeeth);
      this->tabPage_Simulation->Controls->Add(this->numericUpDown_Sim_WheelTorque);
      this->tabPage_Simulation->Location = System::Drawing::Point(4, 22);
      this->tabPage_Simulation->Name = L"tabPage_Simulation";
      this->tabPage_Simulation->Padding = System::Windows::Forms::Padding(3);
      this->tabPage_Simulation->Size = System::Drawing::Size(389, 111);
      this->tabPage_Simulation->TabIndex = 0;
      this->tabPage_Simulation->Text = L"Simulation";
      this->tabPage_Simulation->UseVisualStyleBackColor = true;
      // 
      // checkBox_RightPedal
      // 
      this->checkBox_RightPedal->AutoSize = true;
      this->checkBox_RightPedal->Location = System::Drawing::Point(202, 54);
      this->checkBox_RightPedal->Name = L"checkBox_RightPedal";
      this->checkBox_RightPedal->Size = System::Drawing::Size(81, 17);
      this->checkBox_RightPedal->TabIndex = 34;
      this->checkBox_RightPedal->Text = L"Right Pedal";
      this->checkBox_RightPedal->UseVisualStyleBackColor = true;
      this->checkBox_RightPedal->CheckedChanged += gcnew System::EventHandler(this, &BikePowerSensor::checkBox_RightPedal_CheckedChanged);
      // 
      // numericUpDown_PedalPwr_CurrOutput
      // 
      this->numericUpDown_PedalPwr_CurrOutput->Location = System::Drawing::Point(202, 25);
      this->numericUpDown_PedalPwr_CurrOutput->Name = L"numericUpDown_PedalPwr_CurrOutput";
      this->numericUpDown_PedalPwr_CurrOutput->Size = System::Drawing::Size(50, 20);
      this->numericUpDown_PedalPwr_CurrOutput->TabIndex = 31;
      this->numericUpDown_PedalPwr_CurrOutput->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {50, 0, 0, 0});
      this->numericUpDown_PedalPwr_CurrOutput->ValueChanged += gcnew System::EventHandler(this, &BikePowerSensor::numericUpDown_PedalPwr_CurrOutput_ValueChanged);
      // 
      // label_PedalPwr
      // 
      this->label_PedalPwr->AutoSize = true;
      this->label_PedalPwr->Location = System::Drawing::Point(198, 9);
      this->label_PedalPwr->Name = L"label_PedalPwr";
      this->label_PedalPwr->Size = System::Drawing::Size(81, 13);
      this->label_PedalPwr->TabIndex = 29;
      this->label_PedalPwr->Text = L"Pedal Power(%)";
      // 
      // checkBox_Sim_Coast
      // 
      this->checkBox_Sim_Coast->AutoSize = true;
      this->checkBox_Sim_Coast->Location = System::Drawing::Point(74, 71);
      this->checkBox_Sim_Coast->Name = L"checkBox_Sim_Coast";
      this->checkBox_Sim_Coast->Size = System::Drawing::Size(53, 17);
      this->checkBox_Sim_Coast->TabIndex = 20;
      this->checkBox_Sim_Coast->Text = L"Coast";
      this->checkBox_Sim_Coast->UseVisualStyleBackColor = true;
      this->checkBox_Sim_Coast->CheckedChanged += gcnew System::EventHandler(this, &BikePowerSensor::checkBox_Sim_Coast_CheckedChanged);
      // 
      // label_Sim_GearTeeth
      // 
      this->label_Sim_GearTeeth->AutoSize = true;
      this->label_Sim_GearTeeth->Location = System::Drawing::Point(293, 32);
      this->label_Sim_GearTeeth->Name = L"label_Sim_GearTeeth";
      this->label_Sim_GearTeeth->Size = System::Drawing::Size(84, 13);
      this->label_Sim_GearTeeth->TabIndex = 20;
      this->label_Sim_GearTeeth->Text = L"# Gear   #Teeth";
      // 
      // label_Sim_Torque
      // 
      this->label_Sim_Torque->AutoSize = true;
      this->label_Sim_Torque->Location = System::Drawing::Point(301, 70);
      this->label_Sim_Torque->Name = L"label_Sim_Torque";
      this->label_Sim_Torque->Size = System::Drawing::Size(66, 13);
      this->label_Sim_Torque->TabIndex = 26;
      this->label_Sim_Torque->Text = L"Torque (Nm)";
      // 
      // checkBox_Sim_Stop
      // 
      this->checkBox_Sim_Stop->AutoSize = true;
      this->checkBox_Sim_Stop->Location = System::Drawing::Point(6, 71);
      this->checkBox_Sim_Stop->Name = L"checkBox_Sim_Stop";
      this->checkBox_Sim_Stop->Size = System::Drawing::Size(48, 17);
      this->checkBox_Sim_Stop->TabIndex = 19;
      this->checkBox_Sim_Stop->Text = L"Stop";
      this->checkBox_Sim_Stop->UseVisualStyleBackColor = true;
      this->checkBox_Sim_Stop->CheckedChanged += gcnew System::EventHandler(this, &BikePowerSensor::checkBox_Sim_Stop_CheckedChanged);
      // 
      // label_Sim_WheelCircumferenceConst
      // 
      this->label_Sim_WheelCircumferenceConst->AutoSize = true;
      this->label_Sim_WheelCircumferenceConst->Location = System::Drawing::Point(100, 88);
      this->label_Sim_WheelCircumferenceConst->Name = L"label_Sim_WheelCircumferenceConst";
      this->label_Sim_WheelCircumferenceConst->Size = System::Drawing::Size(132, 13);
      this->label_Sim_WheelCircumferenceConst->TabIndex = 18;
      this->label_Sim_WheelCircumferenceConst->Text = L"Wheel Circumference (cm)";
      this->label_Sim_WheelCircumferenceConst->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
      // 
      // checkBox_Sim_Sweeping
      // 
      this->checkBox_Sim_Sweeping->AutoSize = true;
      this->checkBox_Sim_Sweeping->Location = System::Drawing::Point(6, 88);
      this->checkBox_Sim_Sweeping->Name = L"checkBox_Sim_Sweeping";
      this->checkBox_Sim_Sweeping->Size = System::Drawing::Size(73, 17);
      this->checkBox_Sim_Sweeping->TabIndex = 17;
      this->checkBox_Sim_Sweeping->Text = L"Sweeping";
      this->checkBox_Sim_Sweeping->UseVisualStyleBackColor = true;
      this->checkBox_Sim_Sweeping->CheckedChanged += gcnew System::EventHandler(this, &BikePowerSensor::checkBox_Sim_Sweeping_CheckedChanged);
      // 
      // numericUpDown_Sim_WheelCircumference
      // 
      this->numericUpDown_Sim_WheelCircumference->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {5, 0, 0, 0});
      this->numericUpDown_Sim_WheelCircumference->Location = System::Drawing::Point(238, 86);
      this->numericUpDown_Sim_WheelCircumference->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
      this->numericUpDown_Sim_WheelCircumference->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
      this->numericUpDown_Sim_WheelCircumference->Name = L"numericUpDown_Sim_WheelCircumference";
      this->numericUpDown_Sim_WheelCircumference->Size = System::Drawing::Size(47, 20);
      this->numericUpDown_Sim_WheelCircumference->TabIndex = 13;
      this->numericUpDown_Sim_WheelCircumference->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {207, 0, 0, 0});
      this->numericUpDown_Sim_WheelCircumference->ValueChanged += gcnew System::EventHandler(this, &BikePowerSensor::numericUpDown_Sim_WheelCircumference_ValueChanged);
      // 
      // numericUpDown_Sim_MaxOutput
      // 
      this->numericUpDown_Sim_MaxOutput->DecimalPlaces = 3;
      this->numericUpDown_Sim_MaxOutput->Enabled = false;
      this->numericUpDown_Sim_MaxOutput->Location = System::Drawing::Point(42, 48);
      this->numericUpDown_Sim_MaxOutput->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
      this->numericUpDown_Sim_MaxOutput->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 196608});
      this->numericUpDown_Sim_MaxOutput->Name = L"numericUpDown_Sim_MaxOutput";
      this->numericUpDown_Sim_MaxOutput->Size = System::Drawing::Size(64, 20);
      this->numericUpDown_Sim_MaxOutput->TabIndex = 16;
      this->numericUpDown_Sim_MaxOutput->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
      this->numericUpDown_Sim_MaxOutput->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {120, 0, 0, 0});
      this->numericUpDown_Sim_MaxOutput->ValueChanged += gcnew System::EventHandler(this, &BikePowerSensor::numericUpDown_Sim_MinMaxOutput_ValueChanged);
      // 
      // radioButton_Sim_SimBySpeed
      // 
      this->radioButton_Sim_SimBySpeed->AutoSize = true;
      this->radioButton_Sim_SimBySpeed->Location = System::Drawing::Point(112, 36);
      this->radioButton_Sim_SimBySpeed->Name = L"radioButton_Sim_SimBySpeed";
      this->radioButton_Sim_SimBySpeed->Size = System::Drawing::Size(56, 30);
      this->radioButton_Sim_SimBySpeed->TabIndex = 12;
      this->radioButton_Sim_SimBySpeed->Text = L"Speed\r\n(km/h)";
      this->radioButton_Sim_SimBySpeed->UseVisualStyleBackColor = true;
      // 
      // radioButton_Sim_SimByCadence
      // 
      this->radioButton_Sim_SimByCadence->AutoSize = true;
      this->radioButton_Sim_SimByCadence->Checked = true;
      this->radioButton_Sim_SimByCadence->Location = System::Drawing::Point(112, 8);
      this->radioButton_Sim_SimByCadence->Name = L"radioButton_Sim_SimByCadence";
      this->radioButton_Sim_SimByCadence->Size = System::Drawing::Size(71, 30);
      this->radioButton_Sim_SimByCadence->TabIndex = 11;
      this->radioButton_Sim_SimByCadence->TabStop = true;
      this->radioButton_Sim_SimByCadence->Text = L"Cadence \r\n(rpm)";
      this->radioButton_Sim_SimByCadence->UseVisualStyleBackColor = true;
      this->radioButton_Sim_SimByCadence->CheckedChanged += gcnew System::EventHandler(this, &BikePowerSensor::radioButton_SimByChanged);
      // 
      // numericUpDown_Sim_CurOutput
      // 
      this->numericUpDown_Sim_CurOutput->DecimalPlaces = 3;
      this->numericUpDown_Sim_CurOutput->Location = System::Drawing::Point(42, 27);
      this->numericUpDown_Sim_CurOutput->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
      this->numericUpDown_Sim_CurOutput->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 196608});
      this->numericUpDown_Sim_CurOutput->Name = L"numericUpDown_Sim_CurOutput";
      this->numericUpDown_Sim_CurOutput->Size = System::Drawing::Size(64, 20);
      this->numericUpDown_Sim_CurOutput->TabIndex = 15;
      this->numericUpDown_Sim_CurOutput->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
      this->numericUpDown_Sim_CurOutput->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {90, 0, 0, 0});
      this->numericUpDown_Sim_CurOutput->ValueChanged += gcnew System::EventHandler(this, &BikePowerSensor::numericUpDown_Sim_CurOutput_ValueChanged);
      // 
      // label_Sim_Crank
      // 
      this->label_Sim_Crank->AutoSize = true;
      this->label_Sim_Crank->Location = System::Drawing::Point(342, 52);
      this->label_Sim_Crank->Name = L"label_Sim_Crank";
      this->label_Sim_Crank->Size = System::Drawing::Size(35, 13);
      this->label_Sim_Crank->TabIndex = 21;
      this->label_Sim_Crank->Text = L"Crank";
      // 
      // numericUpDown_Sim_MinOutput
      // 
      this->numericUpDown_Sim_MinOutput->DecimalPlaces = 3;
      this->numericUpDown_Sim_MinOutput->Enabled = false;
      this->numericUpDown_Sim_MinOutput->Location = System::Drawing::Point(42, 6);
      this->numericUpDown_Sim_MinOutput->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
      this->numericUpDown_Sim_MinOutput->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 196608});
      this->numericUpDown_Sim_MinOutput->Name = L"numericUpDown_Sim_MinOutput";
      this->numericUpDown_Sim_MinOutput->Size = System::Drawing::Size(64, 20);
      this->numericUpDown_Sim_MinOutput->TabIndex = 14;
      this->numericUpDown_Sim_MinOutput->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
      this->numericUpDown_Sim_MinOutput->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {60, 0, 0, 0});
      this->numericUpDown_Sim_MinOutput->ValueChanged += gcnew System::EventHandler(this, &BikePowerSensor::numericUpDown_Sim_MinMaxOutput_ValueChanged);
      // 
      // label_Sim_Max
      // 
      this->label_Sim_Max->AutoSize = true;
      this->label_Sim_Max->Location = System::Drawing::Point(16, 50);
      this->label_Sim_Max->Name = L"label_Sim_Max";
      this->label_Sim_Max->Size = System::Drawing::Size(27, 13);
      this->label_Sim_Max->TabIndex = 3;
      this->label_Sim_Max->Text = L"Max";
      // 
      // numericUpDown_Sim_WheelGearTeeth
      // 
      this->numericUpDown_Sim_WheelGearTeeth->Location = System::Drawing::Point(291, 8);
      this->numericUpDown_Sim_WheelGearTeeth->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {99, 0, 0, 0});
      this->numericUpDown_Sim_WheelGearTeeth->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
      this->numericUpDown_Sim_WheelGearTeeth->Name = L"numericUpDown_Sim_WheelGearTeeth";
      this->numericUpDown_Sim_WheelGearTeeth->Size = System::Drawing::Size(45, 20);
      this->numericUpDown_Sim_WheelGearTeeth->TabIndex = 21;
      this->numericUpDown_Sim_WheelGearTeeth->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
      this->numericUpDown_Sim_WheelGearTeeth->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {21, 0, 0, 0});
      this->numericUpDown_Sim_WheelGearTeeth->ValueChanged += gcnew System::EventHandler(this, &BikePowerSensor::numericUpDown_Sim_WheelGearTeeth_ValueChanged);
      // 
      // label_Sim_Current
      // 
      this->label_Sim_Current->AutoSize = true;
      this->label_Sim_Current->Location = System::Drawing::Point(2, 29);
      this->label_Sim_Current->Name = L"label_Sim_Current";
      this->label_Sim_Current->Size = System::Drawing::Size(41, 13);
      this->label_Sim_Current->TabIndex = 2;
      this->label_Sim_Current->Text = L"Current";
      // 
      // label_Sim_Min
      // 
      this->label_Sim_Min->AutoSize = true;
      this->label_Sim_Min->Location = System::Drawing::Point(19, 8);
      this->label_Sim_Min->Name = L"label_Sim_Min";
      this->label_Sim_Min->Size = System::Drawing::Size(24, 13);
      this->label_Sim_Min->TabIndex = 1;
      this->label_Sim_Min->Text = L"Min";
      // 
      // numericUpDown_Sim_CrankTorque
      // 
      this->numericUpDown_Sim_CrankTorque->DecimalPlaces = 1;
      this->numericUpDown_Sim_CrankTorque->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
      this->numericUpDown_Sim_CrankTorque->Location = System::Drawing::Point(340, 86);
      this->numericUpDown_Sim_CrankTorque->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {999, 0, 0, 65536});
      this->numericUpDown_Sim_CrankTorque->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
      this->numericUpDown_Sim_CrankTorque->Name = L"numericUpDown_Sim_CrankTorque";
      this->numericUpDown_Sim_CrankTorque->Size = System::Drawing::Size(45, 20);
      this->numericUpDown_Sim_CrankTorque->TabIndex = 24;
      this->numericUpDown_Sim_CrankTorque->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
      this->numericUpDown_Sim_CrankTorque->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {300, 0, 0, 65536});
      this->numericUpDown_Sim_CrankTorque->ValueChanged += gcnew System::EventHandler(this, &BikePowerSensor::numericUpDown_Sim_CrankTorque_ValueChanged);
      // 
      // label_Sim_Wheel
      // 
      this->label_Sim_Wheel->AutoSize = true;
      this->label_Sim_Wheel->Location = System::Drawing::Point(294, 52);
      this->label_Sim_Wheel->Name = L"label_Sim_Wheel";
      this->label_Sim_Wheel->Size = System::Drawing::Size(38, 13);
      this->label_Sim_Wheel->TabIndex = 22;
      this->label_Sim_Wheel->Text = L"Wheel";
      // 
      // numericUpDown_Sim_CrankGearTeeth
      // 
      this->numericUpDown_Sim_CrankGearTeeth->Location = System::Drawing::Point(338, 8);
      this->numericUpDown_Sim_CrankGearTeeth->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {99, 0, 0, 0});
      this->numericUpDown_Sim_CrankGearTeeth->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
      this->numericUpDown_Sim_CrankGearTeeth->Name = L"numericUpDown_Sim_CrankGearTeeth";
      this->numericUpDown_Sim_CrankGearTeeth->Size = System::Drawing::Size(45, 20);
      this->numericUpDown_Sim_CrankGearTeeth->TabIndex = 22;
      this->numericUpDown_Sim_CrankGearTeeth->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
      this->numericUpDown_Sim_CrankGearTeeth->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {42, 0, 0, 0});
      this->numericUpDown_Sim_CrankGearTeeth->ValueChanged += gcnew System::EventHandler(this, &BikePowerSensor::numericUpDown_Sim_CrankGearTeeth_ValueChanged);
      // 
      // numericUpDown_Sim_WheelTorque
      // 
      this->numericUpDown_Sim_WheelTorque->DecimalPlaces = 1;
      this->numericUpDown_Sim_WheelTorque->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
      this->numericUpDown_Sim_WheelTorque->Location = System::Drawing::Point(291, 86);
      this->numericUpDown_Sim_WheelTorque->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {999, 0, 0, 65536});
      this->numericUpDown_Sim_WheelTorque->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
      this->numericUpDown_Sim_WheelTorque->Name = L"numericUpDown_Sim_WheelTorque";
      this->numericUpDown_Sim_WheelTorque->Size = System::Drawing::Size(45, 20);
      this->numericUpDown_Sim_WheelTorque->TabIndex = 23;
      this->numericUpDown_Sim_WheelTorque->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
      this->numericUpDown_Sim_WheelTorque->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {150, 0, 0, 65536});
      this->numericUpDown_Sim_WheelTorque->ValueChanged += gcnew System::EventHandler(this, &BikePowerSensor::numericUpDown_Sim_WheelTorque_ValueChanged);
      // 
      // tabPage_PowerPaging
      // 
      this->tabPage_PowerPaging->Controls->Add(this->checkBox_Ppg_TxPedalPwr);
      this->tabPage_PowerPaging->Controls->Add(this->checkBox_Ppg_TxCadence);
      this->tabPage_PowerPaging->Controls->Add(this->numericUpDown_Ppg_SlopeConstant);
      this->tabPage_PowerPaging->Controls->Add(this->groupBox_Ppg_SendPageSelect);
      this->tabPage_PowerPaging->Controls->Add(this->label1);
      this->tabPage_PowerPaging->Controls->Add(this->numericUpDown_Ppg_BasicPowerInterlvTime);
      this->tabPage_PowerPaging->Controls->Add(this->label_Ppg_SlopeConstant);
      this->tabPage_PowerPaging->Controls->Add(this->groupBox_Ppg_RaiseEventsSelect);
      this->tabPage_PowerPaging->Location = System::Drawing::Point(4, 22);
      this->tabPage_PowerPaging->Name = L"tabPage_PowerPaging";
      this->tabPage_PowerPaging->Size = System::Drawing::Size(389, 111);
      this->tabPage_PowerPaging->TabIndex = 3;
      this->tabPage_PowerPaging->Text = L"Settings";
      this->tabPage_PowerPaging->UseVisualStyleBackColor = true;
      // 
      // checkBox_Ppg_TxPedalPwr
      // 
      this->checkBox_Ppg_TxPedalPwr->AutoSize = true;
      this->checkBox_Ppg_TxPedalPwr->Checked = true;
      this->checkBox_Ppg_TxPedalPwr->CheckState = System::Windows::Forms::CheckState::Checked;
      this->checkBox_Ppg_TxPedalPwr->Location = System::Drawing::Point(129, 57);
      this->checkBox_Ppg_TxPedalPwr->Name = L"checkBox_Ppg_TxPedalPwr";
      this->checkBox_Ppg_TxPedalPwr->Size = System::Drawing::Size(114, 17);
      this->checkBox_Ppg_TxPedalPwr->TabIndex = 33;
      this->checkBox_Ppg_TxPedalPwr->Text = L"Send Pedal Power";
      this->checkBox_Ppg_TxPedalPwr->UseVisualStyleBackColor = true;
      this->checkBox_Ppg_TxPedalPwr->CheckedChanged += gcnew System::EventHandler(this, &BikePowerSensor::checkBox_Ppg_TxPedalPwr_CheckedChanged);
      // 
      // checkBox_Ppg_TxCadence
      // 
      this->checkBox_Ppg_TxCadence->AutoSize = true;
      this->checkBox_Ppg_TxCadence->Checked = true;
      this->checkBox_Ppg_TxCadence->CheckState = System::Windows::Forms::CheckState::Checked;
      this->checkBox_Ppg_TxCadence->Location = System::Drawing::Point(129, 41);
      this->checkBox_Ppg_TxCadence->Name = L"checkBox_Ppg_TxCadence";
      this->checkBox_Ppg_TxCadence->Size = System::Drawing::Size(97, 17);
      this->checkBox_Ppg_TxCadence->TabIndex = 32;
      this->checkBox_Ppg_TxCadence->Text = L"Send Cadence";
      this->checkBox_Ppg_TxCadence->UseVisualStyleBackColor = true;
      this->checkBox_Ppg_TxCadence->CheckedChanged += gcnew System::EventHandler(this, &BikePowerSensor::checkBox_Ppg_TxCadence_CheckedChanged);
      // 
      // numericUpDown_Ppg_SlopeConstant
      // 
      this->numericUpDown_Ppg_SlopeConstant->DecimalPlaces = 1;
      this->numericUpDown_Ppg_SlopeConstant->Enabled = false;
      this->numericUpDown_Ppg_SlopeConstant->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {5, 0, 0, 0});
      this->numericUpDown_Ppg_SlopeConstant->Location = System::Drawing::Point(213, 79);
      this->numericUpDown_Ppg_SlopeConstant->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {50, 0, 0, 0});
      this->numericUpDown_Ppg_SlopeConstant->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {10, 0, 0, 0});
      this->numericUpDown_Ppg_SlopeConstant->Name = L"numericUpDown_Ppg_SlopeConstant";
      this->numericUpDown_Ppg_SlopeConstant->Size = System::Drawing::Size(45, 20);
      this->numericUpDown_Ppg_SlopeConstant->TabIndex = 31;
      this->numericUpDown_Ppg_SlopeConstant->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {10, 0, 0, 0});
      this->numericUpDown_Ppg_SlopeConstant->ValueChanged += gcnew System::EventHandler(this, &BikePowerSensor::numericUpDown_Ppg_SlopeConstant_ValueChanged);
      // 
      // groupBox_Ppg_SendPageSelect
      // 
      this->groupBox_Ppg_SendPageSelect->Controls->Add(this->radioButton_WheelTorque);
      this->groupBox_Ppg_SendPageSelect->Controls->Add(this->radioButton_CTF);
      this->groupBox_Ppg_SendPageSelect->Controls->Add(this->radioButton_Power);
      this->groupBox_Ppg_SendPageSelect->Controls->Add(this->radioButton_CrankTorque);
      this->groupBox_Ppg_SendPageSelect->Location = System::Drawing::Point(3, 2);
      this->groupBox_Ppg_SendPageSelect->Name = L"groupBox_Ppg_SendPageSelect";
      this->groupBox_Ppg_SendPageSelect->Size = System::Drawing::Size(117, 106);
      this->groupBox_Ppg_SendPageSelect->TabIndex = 27;
      this->groupBox_Ppg_SendPageSelect->TabStop = false;
      this->groupBox_Ppg_SendPageSelect->Text = L"Sensor Type:";
      // 
      // radioButton_WheelTorque
      // 
      this->radioButton_WheelTorque->AutoSize = true;
      this->radioButton_WheelTorque->Location = System::Drawing::Point(6, 34);
      this->radioButton_WheelTorque->Name = L"radioButton_WheelTorque";
      this->radioButton_WheelTorque->Size = System::Drawing::Size(93, 17);
      this->radioButton_WheelTorque->TabIndex = 26;
      this->radioButton_WheelTorque->Text = L"Wheel Torque";
      this->radioButton_WheelTorque->UseVisualStyleBackColor = true;
      this->radioButton_WheelTorque->CheckedChanged += gcnew System::EventHandler(this, &BikePowerSensor::radioButton_SensorType_CheckedChanged);
      // 
      // radioButton_CTF
      // 
      this->radioButton_CTF->AutoSize = true;
      this->radioButton_CTF->Location = System::Drawing::Point(6, 70);
      this->radioButton_CTF->Name = L"radioButton_CTF";
      this->radioButton_CTF->Size = System::Drawing::Size(93, 30);
      this->radioButton_CTF->TabIndex = 28;
      this->radioButton_CTF->Text = L"Crank Torque \r\nFrequency";
      this->radioButton_CTF->UseVisualStyleBackColor = true;
      this->radioButton_CTF->CheckedChanged += gcnew System::EventHandler(this, &BikePowerSensor::radioButton_SensorType_CheckedChanged);
      // 
      // radioButton_Power
      // 
      this->radioButton_Power->AutoSize = true;
      this->radioButton_Power->Checked = true;
      this->radioButton_Power->Location = System::Drawing::Point(6, 16);
      this->radioButton_Power->Name = L"radioButton_Power";
      this->radioButton_Power->Size = System::Drawing::Size(79, 17);
      this->radioButton_Power->TabIndex = 25;
      this->radioButton_Power->TabStop = true;
      this->radioButton_Power->Text = L"Power Only";
      this->radioButton_Power->UseVisualStyleBackColor = true;
      this->radioButton_Power->CheckedChanged += gcnew System::EventHandler(this, &BikePowerSensor::radioButton_SensorType_CheckedChanged);
      // 
      // radioButton_CrankTorque
      // 
      this->radioButton_CrankTorque->AutoSize = true;
      this->radioButton_CrankTorque->Location = System::Drawing::Point(6, 52);
      this->radioButton_CrankTorque->Name = L"radioButton_CrankTorque";
      this->radioButton_CrankTorque->Size = System::Drawing::Size(90, 17);
      this->radioButton_CrankTorque->TabIndex = 27;
      this->radioButton_CrankTorque->Text = L"Crank Torque";
      this->radioButton_CrankTorque->UseVisualStyleBackColor = true;
      this->radioButton_CrankTorque->CheckedChanged += gcnew System::EventHandler(this, &BikePowerSensor::radioButton_SensorType_CheckedChanged);
      // 
      // label1
      // 
      this->label1->AutoSize = true;
      this->label1->Location = System::Drawing::Point(126, 10);
      this->label1->Name = L"label1";
      this->label1->Size = System::Drawing::Size(95, 26);
      this->label1->TabIndex = 31;
      this->label1->Text = L"Send basic power\r\nevery ... messages";
      // 
      // numericUpDown_Ppg_BasicPowerInterlvTime
      // 
      this->numericUpDown_Ppg_BasicPowerInterlvTime->Enabled = false;
      this->numericUpDown_Ppg_BasicPowerInterlvTime->Location = System::Drawing::Point(227, 15);
      this->numericUpDown_Ppg_BasicPowerInterlvTime->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {9, 0, 0, 0});
      this->numericUpDown_Ppg_BasicPowerInterlvTime->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {2, 0, 0, 0});
      this->numericUpDown_Ppg_BasicPowerInterlvTime->Name = L"numericUpDown_Ppg_BasicPowerInterlvTime";
      this->numericUpDown_Ppg_BasicPowerInterlvTime->Size = System::Drawing::Size(31, 20);
      this->numericUpDown_Ppg_BasicPowerInterlvTime->TabIndex = 30;
      this->numericUpDown_Ppg_BasicPowerInterlvTime->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {5, 0, 0, 0});
      this->numericUpDown_Ppg_BasicPowerInterlvTime->ValueChanged += gcnew System::EventHandler(this, &BikePowerSensor::numericUpDown_Ppg_BasicPowerInterlvTime_ValueChanged);
      // 
      // label_Ppg_SlopeConstant
      // 
      this->label_Ppg_SlopeConstant->AutoSize = true;
      this->label_Ppg_SlopeConstant->Location = System::Drawing::Point(160, 76);
      this->label_Ppg_SlopeConstant->Name = L"label_Ppg_SlopeConstant";
      this->label_Ppg_SlopeConstant->Size = System::Drawing::Size(47, 26);
      this->label_Ppg_SlopeConstant->TabIndex = 10;
      this->label_Ppg_SlopeConstant->Text = L"Slope \r\n(Nm/Hz)";
      // 
      // groupBox_Ppg_RaiseEventsSelect
      // 
      this->groupBox_Ppg_RaiseEventsSelect->Controls->Add(this->numericUpDown_Ppg_TimerEventFreq);
      this->groupBox_Ppg_RaiseEventsSelect->Controls->Add(this->radioButton_Ppg_EventOnTime);
      this->groupBox_Ppg_RaiseEventsSelect->Controls->Add(this->radioButton_Ppg_EventOnRotation);
      this->groupBox_Ppg_RaiseEventsSelect->Location = System::Drawing::Point(272, 3);
      this->groupBox_Ppg_RaiseEventsSelect->Name = L"groupBox_Ppg_RaiseEventsSelect";
      this->groupBox_Ppg_RaiseEventsSelect->Size = System::Drawing::Size(114, 72);
      this->groupBox_Ppg_RaiseEventsSelect->TabIndex = 5;
      this->groupBox_Ppg_RaiseEventsSelect->TabStop = false;
      this->groupBox_Ppg_RaiseEventsSelect->Text = L"Events Raised On:";
      // 
      // numericUpDown_Ppg_TimerEventFreq
      // 
      this->numericUpDown_Ppg_TimerEventFreq->DecimalPlaces = 1;
      this->numericUpDown_Ppg_TimerEventFreq->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
      this->numericUpDown_Ppg_TimerEventFreq->Location = System::Drawing::Point(56, 49);
      this->numericUpDown_Ppg_TimerEventFreq->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {4, 0, 0, 0});
      this->numericUpDown_Ppg_TimerEventFreq->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
      this->numericUpDown_Ppg_TimerEventFreq->Name = L"numericUpDown_Ppg_TimerEventFreq";
      this->numericUpDown_Ppg_TimerEventFreq->Size = System::Drawing::Size(39, 20);
      this->numericUpDown_Ppg_TimerEventFreq->TabIndex = 34;
      this->numericUpDown_Ppg_TimerEventFreq->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {2, 0, 0, 0});
      this->numericUpDown_Ppg_TimerEventFreq->ValueChanged += gcnew System::EventHandler(this, &BikePowerSensor::numericUpDown_Ppg_TimerEventFreq_ValueChanged);
      // 
      // radioButton_Ppg_EventOnTime
      // 
      this->radioButton_Ppg_EventOnTime->AutoSize = true;
      this->radioButton_Ppg_EventOnTime->Checked = true;
      this->radioButton_Ppg_EventOnTime->Location = System::Drawing::Point(4, 32);
      this->radioButton_Ppg_EventOnTime->Name = L"radioButton_Ppg_EventOnTime";
      this->radioButton_Ppg_EventOnTime->Size = System::Drawing::Size(94, 30);
      this->radioButton_Ppg_EventOnTime->TabIndex = 33;
      this->radioButton_Ppg_EventOnTime->TabStop = true;
      this->radioButton_Ppg_EventOnTime->Text = L"Timer Intervals\r\nin Hz";
      this->radioButton_Ppg_EventOnTime->UseVisualStyleBackColor = true;
      this->radioButton_Ppg_EventOnTime->CheckedChanged += gcnew System::EventHandler(this, &BikePowerSensor::radioButton_Ppg_UpdateType_CheckedChanged);
      // 
      // radioButton_Ppg_EventOnRotation
      // 
      this->radioButton_Ppg_EventOnRotation->AutoSize = true;
      this->radioButton_Ppg_EventOnRotation->Enabled = false;
      this->radioButton_Ppg_EventOnRotation->Location = System::Drawing::Point(4, 16);
      this->radioButton_Ppg_EventOnRotation->Name = L"radioButton_Ppg_EventOnRotation";
      this->radioButton_Ppg_EventOnRotation->Size = System::Drawing::Size(65, 17);
      this->radioButton_Ppg_EventOnRotation->TabIndex = 32;
      this->radioButton_Ppg_EventOnRotation->Text = L"Rotation";
      this->radioButton_Ppg_EventOnRotation->UseVisualStyleBackColor = true;
      this->radioButton_Ppg_EventOnRotation->CheckedChanged += gcnew System::EventHandler(this, &BikePowerSensor::radioButton_Ppg_UpdateType_CheckedChanged);
      // 
      // tabPage_Calibration
      // 
      this->tabPage_Calibration->Controls->Add(this->checkBox_Cal_TorqAZMesg);
      this->tabPage_Calibration->Controls->Add(this->label_Cal_TorqOffset);
      this->tabPage_Calibration->Controls->Add(this->label_Cal_RawTorq);
      this->tabPage_Calibration->Controls->Add(this->label_Cal_AZStatus);
      this->tabPage_Calibration->Controls->Add(this->numericUpDown_Cal_RawTorque);
      this->tabPage_Calibration->Controls->Add(this->listBox_Cal_AZStatus);
      this->tabPage_Calibration->Controls->Add(this->label_Cal_CalNum);
      this->tabPage_Calibration->Controls->Add(this->numericUpDown_Cal_OffsetTorque);
      this->tabPage_Calibration->Controls->Add(this->numericUpDown_Cal_CalNumber);
      this->tabPage_Calibration->Controls->Add(this->label_Cal_CalSuccess);
      this->tabPage_Calibration->Controls->Add(this->radioButton_Cal_Failure);
      this->tabPage_Calibration->Controls->Add(this->radioButton_Cal_Success);
      this->tabPage_Calibration->Location = System::Drawing::Point(4, 22);
      this->tabPage_Calibration->Name = L"tabPage_Calibration";
      this->tabPage_Calibration->Size = System::Drawing::Size(389, 111);
      this->tabPage_Calibration->TabIndex = 2;
      this->tabPage_Calibration->Text = L"Calibration";
      this->tabPage_Calibration->UseVisualStyleBackColor = true;
      // 
      // checkBox_Cal_TorqAZMesg
      // 
      this->checkBox_Cal_TorqAZMesg->AutoSize = true;
      this->checkBox_Cal_TorqAZMesg->Checked = true;
      this->checkBox_Cal_TorqAZMesg->CheckState = System::Windows::Forms::CheckState::Checked;
      this->checkBox_Cal_TorqAZMesg->Location = System::Drawing::Point(206, 3);
      this->checkBox_Cal_TorqAZMesg->Name = L"checkBox_Cal_TorqAZMesg";
      this->checkBox_Cal_TorqAZMesg->Size = System::Drawing::Size(146, 30);
      this->checkBox_Cal_TorqAZMesg->TabIndex = 38;
      this->checkBox_Cal_TorqAZMesg->Text = L"Enable Torque/Autozero \r\nSupport Message";
      this->checkBox_Cal_TorqAZMesg->UseVisualStyleBackColor = true;
      this->checkBox_Cal_TorqAZMesg->CheckedChanged += gcnew System::EventHandler(this, &BikePowerSensor::checkBox_Cal_TorqAZMesg_CheckedChanged);
      // 
      // label_Cal_TorqOffset
      // 
      this->label_Cal_TorqOffset->AutoSize = true;
      this->label_Cal_TorqOffset->Location = System::Drawing::Point(311, 72);
      this->label_Cal_TorqOffset->Name = L"label_Cal_TorqOffset";
      this->label_Cal_TorqOffset->Size = System::Drawing::Size(75, 13);
      this->label_Cal_TorqOffset->TabIndex = 33;
      this->label_Cal_TorqOffset->Text = L"Offset Torque:";
      // 
      // label_Cal_RawTorq
      // 
      this->label_Cal_RawTorq->AutoSize = true;
      this->label_Cal_RawTorq->Location = System::Drawing::Point(317, 34);
      this->label_Cal_RawTorq->Name = L"label_Cal_RawTorq";
      this->label_Cal_RawTorq->Size = System::Drawing::Size(69, 13);
      this->label_Cal_RawTorq->TabIndex = 32;
      this->label_Cal_RawTorq->Text = L"Raw Torque:";
      // 
      // label_Cal_AZStatus
      // 
      this->label_Cal_AZStatus->AutoSize = true;
      this->label_Cal_AZStatus->Location = System::Drawing::Point(203, 42);
      this->label_Cal_AZStatus->Name = L"label_Cal_AZStatus";
      this->label_Cal_AZStatus->Size = System::Drawing::Size(87, 13);
      this->label_Cal_AZStatus->TabIndex = 29;
      this->label_Cal_AZStatus->Text = L"Auto Zero Status";
      // 
      // numericUpDown_Cal_RawTorque
      // 
      this->numericUpDown_Cal_RawTorque->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1000, 0, 0, 0});
      this->numericUpDown_Cal_RawTorque->Location = System::Drawing::Point(321, 50);
      this->numericUpDown_Cal_RawTorque->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {32767, 0, 0, 0});
      this->numericUpDown_Cal_RawTorque->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {32767, 0, 0, System::Int32::MinValue});
      this->numericUpDown_Cal_RawTorque->Name = L"numericUpDown_Cal_RawTorque";
      this->numericUpDown_Cal_RawTorque->Size = System::Drawing::Size(61, 20);
      this->numericUpDown_Cal_RawTorque->TabIndex = 40;
      this->numericUpDown_Cal_RawTorque->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
      this->numericUpDown_Cal_RawTorque->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {12345, 0, 0, 0});
      this->numericUpDown_Cal_RawTorque->ValueChanged += gcnew System::EventHandler(this, &BikePowerSensor::numericUpDown_Cal_RawTorque_ValueChanged);
      // 
      // listBox_Cal_AZStatus
      // 
      this->listBox_Cal_AZStatus->FormattingEnabled = true;
      this->listBox_Cal_AZStatus->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"0 - Off", L"1 - On", L"FF - Not Supported"});
      this->listBox_Cal_AZStatus->Location = System::Drawing::Point(206, 59);
      this->listBox_Cal_AZStatus->Name = L"listBox_Cal_AZStatus";
      this->listBox_Cal_AZStatus->Size = System::Drawing::Size(99, 43);
      this->listBox_Cal_AZStatus->TabIndex = 39;
      this->listBox_Cal_AZStatus->SelectedIndexChanged += gcnew System::EventHandler(this, &BikePowerSensor::listBox_Cal_AZStatus_SelectedIndexChanged);
      // 
      // label_Cal_CalNum
      // 
      this->label_Cal_CalNum->AutoSize = true;
      this->label_Cal_CalNum->Location = System::Drawing::Point(12, 10);
      this->label_Cal_CalNum->Name = L"label_Cal_CalNum";
      this->label_Cal_CalNum->Size = System::Drawing::Size(143, 13);
      this->label_Cal_CalNum->TabIndex = 4;
      this->label_Cal_CalNum->Text = L"Calibration Number To Send:";
      // 
      // numericUpDown_Cal_OffsetTorque
      // 
      this->numericUpDown_Cal_OffsetTorque->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1000, 0, 0, 0});
      this->numericUpDown_Cal_OffsetTorque->Location = System::Drawing::Point(321, 88);
      this->numericUpDown_Cal_OffsetTorque->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {32767, 0, 0, 0});
      this->numericUpDown_Cal_OffsetTorque->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {32767, 0, 0, System::Int32::MinValue});
      this->numericUpDown_Cal_OffsetTorque->Name = L"numericUpDown_Cal_OffsetTorque";
      this->numericUpDown_Cal_OffsetTorque->Size = System::Drawing::Size(61, 20);
      this->numericUpDown_Cal_OffsetTorque->TabIndex = 41;
      this->numericUpDown_Cal_OffsetTorque->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
      this->numericUpDown_Cal_OffsetTorque->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {12345, 0, 0, 0});
      this->numericUpDown_Cal_OffsetTorque->ValueChanged += gcnew System::EventHandler(this, &BikePowerSensor::numericUpDown_Cal_OffsetTorque_ValueChanged);
      // 
      // numericUpDown_Cal_CalNumber
      // 
      this->numericUpDown_Cal_CalNumber->Location = System::Drawing::Point(90, 27);
      this->numericUpDown_Cal_CalNumber->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {32767, 0, 0, 0});
      this->numericUpDown_Cal_CalNumber->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {32768, 0, 0, System::Int32::MinValue});
      this->numericUpDown_Cal_CalNumber->Name = L"numericUpDown_Cal_CalNumber";
      this->numericUpDown_Cal_CalNumber->Size = System::Drawing::Size(61, 20);
      this->numericUpDown_Cal_CalNumber->TabIndex = 35;
      this->numericUpDown_Cal_CalNumber->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
      this->numericUpDown_Cal_CalNumber->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {150, 0, 0, 0});
      this->numericUpDown_Cal_CalNumber->ValueChanged += gcnew System::EventHandler(this, &BikePowerSensor::numericUpDown_Cal_CalNumber_ValueChanged);
      // 
      // label_Cal_CalSuccess
      // 
      this->label_Cal_CalSuccess->AutoSize = true;
      this->label_Cal_CalSuccess->Location = System::Drawing::Point(17, 60);
      this->label_Cal_CalSuccess->Name = L"label_Cal_CalSuccess";
      this->label_Cal_CalSuccess->Size = System::Drawing::Size(134, 13);
      this->label_Cal_CalSuccess->TabIndex = 2;
      this->label_Cal_CalSuccess->Text = L"Report Next Calibration As:";
      // 
      // radioButton_Cal_Failure
      // 
      this->radioButton_Cal_Failure->AutoSize = true;
      this->radioButton_Cal_Failure->Location = System::Drawing::Point(92, 76);
      this->radioButton_Cal_Failure->Name = L"radioButton_Cal_Failure";
      this->radioButton_Cal_Failure->Size = System::Drawing::Size(56, 17);
      this->radioButton_Cal_Failure->TabIndex = 37;
      this->radioButton_Cal_Failure->Text = L"Failure";
      this->radioButton_Cal_Failure->UseVisualStyleBackColor = true;
      // 
      // radioButton_Cal_Success
      // 
      this->radioButton_Cal_Success->AutoSize = true;
      this->radioButton_Cal_Success->Checked = true;
      this->radioButton_Cal_Success->Location = System::Drawing::Point(20, 76);
      this->radioButton_Cal_Success->Name = L"radioButton_Cal_Success";
      this->radioButton_Cal_Success->Size = System::Drawing::Size(66, 17);
      this->radioButton_Cal_Success->TabIndex = 36;
      this->radioButton_Cal_Success->TabStop = true;
      this->radioButton_Cal_Success->Text = L"Success";
      this->radioButton_Cal_Success->UseVisualStyleBackColor = true;
      this->radioButton_Cal_Success->CheckedChanged += gcnew System::EventHandler(this, &BikePowerSensor::radioButton_Cal_Success_CheckedChanged);
      // 
      // tabPage_GlobalData
      // 
      this->tabPage_GlobalData->Controls->Add(this->checkBox_InvalidSerial);
      this->tabPage_GlobalData->Controls->Add(this->button_Glb_GlobalDataUpdate);
      this->tabPage_GlobalData->Controls->Add(this->label_Glb_GlobalDataError);
      this->tabPage_GlobalData->Controls->Add(this->textBox_Glb_HardwareVerChange);
      this->tabPage_GlobalData->Controls->Add(this->textBox_Glb_ModelNumChange);
      this->tabPage_GlobalData->Controls->Add(this->label_Glb_HardwareVer);
      this->tabPage_GlobalData->Controls->Add(this->label_Glb_ManfID);
      this->tabPage_GlobalData->Controls->Add(this->textBox_Glb_ManfIDChange);
      this->tabPage_GlobalData->Controls->Add(this->label_Glb_SoftwareVer);
      this->tabPage_GlobalData->Controls->Add(this->label_Glb_ModelNum);
      this->tabPage_GlobalData->Controls->Add(this->textBox_Glb_SerialNumChange);
      this->tabPage_GlobalData->Controls->Add(this->textBox_Glb_SoftwareVerChange);
      this->tabPage_GlobalData->Controls->Add(this->label_Glb_SerialNum);
      this->tabPage_GlobalData->Location = System::Drawing::Point(4, 22);
      this->tabPage_GlobalData->Name = L"tabPage_GlobalData";
      this->tabPage_GlobalData->Padding = System::Windows::Forms::Padding(3);
      this->tabPage_GlobalData->Size = System::Drawing::Size(389, 111);
      this->tabPage_GlobalData->TabIndex = 1;
      this->tabPage_GlobalData->Text = L"Global Data";
      this->tabPage_GlobalData->UseVisualStyleBackColor = true;
      // 
      // checkBox_InvalidSerial
      // 
      this->checkBox_InvalidSerial->AutoSize = true;
      this->checkBox_InvalidSerial->Location = System::Drawing::Point(145, 15);
      this->checkBox_InvalidSerial->Name = L"checkBox_InvalidSerial";
      this->checkBox_InvalidSerial->Size = System::Drawing::Size(79, 17);
      this->checkBox_InvalidSerial->TabIndex = 50;
      this->checkBox_InvalidSerial->Text = L"No Serial #";
      this->checkBox_InvalidSerial->UseVisualStyleBackColor = true;
      this->checkBox_InvalidSerial->CheckedChanged += gcnew System::EventHandler(this, &BikePowerSensor::checkBox_InvalidSerial_CheckedChanged);
      // 
      // button_Glb_GlobalDataUpdate
      // 
      this->button_Glb_GlobalDataUpdate->Location = System::Drawing::Point(252, 67);
      this->button_Glb_GlobalDataUpdate->Name = L"button_Glb_GlobalDataUpdate";
      this->button_Glb_GlobalDataUpdate->Size = System::Drawing::Size(97, 20);
      this->button_Glb_GlobalDataUpdate->TabIndex = 49;
      this->button_Glb_GlobalDataUpdate->Text = L"Update All";
      this->button_Glb_GlobalDataUpdate->UseVisualStyleBackColor = true;
      this->button_Glb_GlobalDataUpdate->Click += gcnew System::EventHandler(this, &BikePowerSensor::button_GlobalDataUpdate_Click);
      // 
      // label_Glb_GlobalDataError
      // 
      this->label_Glb_GlobalDataError->AutoSize = true;
      this->label_Glb_GlobalDataError->Location = System::Drawing::Point(220, 90);
      this->label_Glb_GlobalDataError->Name = L"label_Glb_GlobalDataError";
      this->label_Glb_GlobalDataError->Size = System::Drawing::Size(32, 13);
      this->label_Glb_GlobalDataError->TabIndex = 20;
      this->label_Glb_GlobalDataError->Text = L"Error:";
      this->label_Glb_GlobalDataError->Visible = false;
      // 
      // textBox_Glb_HardwareVerChange
      // 
      this->textBox_Glb_HardwareVerChange->Location = System::Drawing::Point(320, 13);
      this->textBox_Glb_HardwareVerChange->MaxLength = 3;
      this->textBox_Glb_HardwareVerChange->Name = L"textBox_Glb_HardwareVerChange";
      this->textBox_Glb_HardwareVerChange->Size = System::Drawing::Size(29, 20);
      this->textBox_Glb_HardwareVerChange->TabIndex = 45;
      this->textBox_Glb_HardwareVerChange->Text = L"1";
      // 
      // textBox_Glb_ModelNumChange
      // 
      this->textBox_Glb_ModelNumChange->Location = System::Drawing::Point(90, 67);
      this->textBox_Glb_ModelNumChange->MaxLength = 5;
      this->textBox_Glb_ModelNumChange->Name = L"textBox_Glb_ModelNumChange";
      this->textBox_Glb_ModelNumChange->Size = System::Drawing::Size(49, 20);
      this->textBox_Glb_ModelNumChange->TabIndex = 44;
      this->textBox_Glb_ModelNumChange->Text = L"33669";
      // 
      // label_Glb_HardwareVer
      // 
      this->label_Glb_HardwareVer->AutoSize = true;
      this->label_Glb_HardwareVer->Location = System::Drawing::Point(239, 16);
      this->label_Glb_HardwareVer->Name = L"label_Glb_HardwareVer";
      this->label_Glb_HardwareVer->Size = System::Drawing::Size(75, 13);
      this->label_Glb_HardwareVer->TabIndex = 3;
      this->label_Glb_HardwareVer->Text = L"Hardware Ver:";
      // 
      // label_Glb_ManfID
      // 
      this->label_Glb_ManfID->AutoSize = true;
      this->label_Glb_ManfID->Location = System::Drawing::Point(6, 42);
      this->label_Glb_ManfID->Name = L"label_Glb_ManfID";
      this->label_Glb_ManfID->Size = System::Drawing::Size(51, 13);
      this->label_Glb_ManfID->TabIndex = 1;
      this->label_Glb_ManfID->Text = L"Manf. ID:";
      // 
      // textBox_Glb_ManfIDChange
      // 
      this->textBox_Glb_ManfIDChange->Location = System::Drawing::Point(90, 39);
      this->textBox_Glb_ManfIDChange->MaxLength = 5;
      this->textBox_Glb_ManfIDChange->Name = L"textBox_Glb_ManfIDChange";
      this->textBox_Glb_ManfIDChange->Size = System::Drawing::Size(49, 20);
      this->textBox_Glb_ManfIDChange->TabIndex = 43;
      this->textBox_Glb_ManfIDChange->Text = L"2";
      // 
      // label_Glb_SoftwareVer
      // 
      this->label_Glb_SoftwareVer->AutoSize = true;
      this->label_Glb_SoftwareVer->Location = System::Drawing::Point(243, 42);
      this->label_Glb_SoftwareVer->Name = L"label_Glb_SoftwareVer";
      this->label_Glb_SoftwareVer->Size = System::Drawing::Size(71, 13);
      this->label_Glb_SoftwareVer->TabIndex = 4;
      this->label_Glb_SoftwareVer->Text = L"Software Ver:";
      // 
      // label_Glb_ModelNum
      // 
      this->label_Glb_ModelNum->AutoSize = true;
      this->label_Glb_ModelNum->Location = System::Drawing::Point(8, 70);
      this->label_Glb_ModelNum->Name = L"label_Glb_ModelNum";
      this->label_Glb_ModelNum->Size = System::Drawing::Size(49, 13);
      this->label_Glb_ModelNum->TabIndex = 5;
      this->label_Glb_ModelNum->Text = L"Model #:";
      // 
      // textBox_Glb_SerialNumChange
      // 
      this->textBox_Glb_SerialNumChange->Location = System::Drawing::Point(63, 13);
      this->textBox_Glb_SerialNumChange->MaxLength = 10;
      this->textBox_Glb_SerialNumChange->Name = L"textBox_Glb_SerialNumChange";
      this->textBox_Glb_SerialNumChange->Size = System::Drawing::Size(76, 20);
      this->textBox_Glb_SerialNumChange->TabIndex = 47;
      this->textBox_Glb_SerialNumChange->Text = L"1234567890";
      // 
      // textBox_Glb_SoftwareVerChange
      // 
      this->textBox_Glb_SoftwareVerChange->Location = System::Drawing::Point(320, 42);
      this->textBox_Glb_SoftwareVerChange->MaxLength = 3;
      this->textBox_Glb_SoftwareVerChange->Name = L"textBox_Glb_SoftwareVerChange";
      this->textBox_Glb_SoftwareVerChange->Size = System::Drawing::Size(29, 20);
      this->textBox_Glb_SoftwareVerChange->TabIndex = 48;
      this->textBox_Glb_SoftwareVerChange->Text = L"1";
      // 
      // label_Glb_SerialNum
      // 
      this->label_Glb_SerialNum->AutoSize = true;
      this->label_Glb_SerialNum->Location = System::Drawing::Point(11, 16);
      this->label_Glb_SerialNum->Name = L"label_Glb_SerialNum";
      this->label_Glb_SerialNum->Size = System::Drawing::Size(46, 13);
      this->label_Glb_SerialNum->TabIndex = 2;
      this->label_Glb_SerialNum->Text = L"Serial #:";
      // 
      // tabPage_GlobalBattery
      // 
      this->tabPage_GlobalBattery->Controls->Add(this->groupBox_Resol);
      this->tabPage_GlobalBattery->Controls->Add(this->label_Volt_Coarse);
      this->tabPage_GlobalBattery->Controls->Add(this->button_Bat_ElpTimeUpdate);
      this->tabPage_GlobalBattery->Controls->Add(this->label_Voltage_Display);
      this->tabPage_GlobalBattery->Controls->Add(this->textBox_Bat_ElpTimeChange);
      this->tabPage_GlobalBattery->Controls->Add(this->checkBox_Bat_FracVolt);
      this->tabPage_GlobalBattery->Controls->Add(this->label_Bat_ElpTimeDisplay);
      this->tabPage_GlobalBattery->Controls->Add(this->label_Bat_Volts);
      this->tabPage_GlobalBattery->Controls->Add(this->label_Bat_ElpTime);
      this->tabPage_GlobalBattery->Controls->Add(this->label_Volts);
      this->tabPage_GlobalBattery->Controls->Add(this->checkBox_Bat_Voltage);
      this->tabPage_GlobalBattery->Controls->Add(this->numericUpDown_Bat_VoltFrac);
      this->tabPage_GlobalBattery->Controls->Add(this->numericUpDown_Bat_VoltInt);
      this->tabPage_GlobalBattery->Controls->Add(this->label_Bat_Status);
      this->tabPage_GlobalBattery->Controls->Add(this->listBox_Bat_Status);
      this->tabPage_GlobalBattery->Controls->Add(this->checkBox_Bat_Status);
      this->tabPage_GlobalBattery->Location = System::Drawing::Point(4, 22);
      this->tabPage_GlobalBattery->Name = L"tabPage_GlobalBattery";
      this->tabPage_GlobalBattery->Size = System::Drawing::Size(389, 111);
      this->tabPage_GlobalBattery->TabIndex = 4;
      this->tabPage_GlobalBattery->Text = L"Global Battery Data";
      this->tabPage_GlobalBattery->UseVisualStyleBackColor = true;
      // 
      // groupBox_Resol
      // 
      this->groupBox_Resol->Controls->Add(this->radioButton_Bat_Elp2Units);
      this->groupBox_Resol->Controls->Add(this->radioButton_Bat_Elp16Units);
      this->groupBox_Resol->Location = System::Drawing::Point(181, 5);
      this->groupBox_Resol->Name = L"groupBox_Resol";
      this->groupBox_Resol->Size = System::Drawing::Size(123, 40);
      this->groupBox_Resol->TabIndex = 2;
      this->groupBox_Resol->TabStop = false;
      this->groupBox_Resol->Text = L"Time Resolution";
      // 
      // radioButton_Bat_Elp2Units
      // 
      this->radioButton_Bat_Elp2Units->AutoSize = true;
      this->radioButton_Bat_Elp2Units->Checked = true;
      this->radioButton_Bat_Elp2Units->Location = System::Drawing::Point(6, 15);
      this->radioButton_Bat_Elp2Units->Name = L"radioButton_Bat_Elp2Units";
      this->radioButton_Bat_Elp2Units->Size = System::Drawing::Size(51, 17);
      this->radioButton_Bat_Elp2Units->TabIndex = 58;
      this->radioButton_Bat_Elp2Units->TabStop = true;
      this->radioButton_Bat_Elp2Units->Text = L"2 sec";
      this->radioButton_Bat_Elp2Units->UseVisualStyleBackColor = true;
      this->radioButton_Bat_Elp2Units->CheckedChanged += gcnew System::EventHandler(this, &BikePowerSensor::radioButton_Bat_ElpUnits_CheckedChanged);
      // 
      // radioButton_Bat_Elp16Units
      // 
      this->radioButton_Bat_Elp16Units->AutoSize = true;
      this->radioButton_Bat_Elp16Units->Location = System::Drawing::Point(63, 15);
      this->radioButton_Bat_Elp16Units->Name = L"radioButton_Bat_Elp16Units";
      this->radioButton_Bat_Elp16Units->Size = System::Drawing::Size(57, 17);
      this->radioButton_Bat_Elp16Units->TabIndex = 59;
      this->radioButton_Bat_Elp16Units->Text = L"16 sec";
      this->radioButton_Bat_Elp16Units->UseVisualStyleBackColor = true;
      this->radioButton_Bat_Elp16Units->CheckedChanged += gcnew System::EventHandler(this, &BikePowerSensor::radioButton_Bat_ElpUnits_CheckedChanged);
      // 
      // label_Volt_Coarse
      // 
      this->label_Volt_Coarse->AutoSize = true;
      this->label_Volt_Coarse->Location = System::Drawing::Point(6, 37);
      this->label_Volt_Coarse->Name = L"label_Volt_Coarse";
      this->label_Volt_Coarse->Size = System::Drawing::Size(56, 13);
      this->label_Volt_Coarse->TabIndex = 2;
      this->label_Volt_Coarse->Text = L"Coarse (V)";
      // 
      // button_Bat_ElpTimeUpdate
      // 
      this->button_Bat_ElpTimeUpdate->Location = System::Drawing::Point(250, 84);
      this->button_Bat_ElpTimeUpdate->Name = L"button_Bat_ElpTimeUpdate";
      this->button_Bat_ElpTimeUpdate->Size = System::Drawing::Size(51, 20);
      this->button_Bat_ElpTimeUpdate->TabIndex = 57;
      this->button_Bat_ElpTimeUpdate->Text = L"Update";
      this->button_Bat_ElpTimeUpdate->UseVisualStyleBackColor = true;
      this->button_Bat_ElpTimeUpdate->Click += gcnew System::EventHandler(this, &BikePowerSensor::button_ElpTimeUpdate_Click);
      // 
      // label_Voltage_Display
      // 
      this->label_Voltage_Display->AutoSize = true;
      this->label_Voltage_Display->Location = System::Drawing::Point(110, 21);
      this->label_Voltage_Display->Name = L"label_Voltage_Display";
      this->label_Voltage_Display->Size = System::Drawing::Size(0, 13);
      this->label_Voltage_Display->TabIndex = 61;
      // 
      // textBox_Bat_ElpTimeChange
      // 
      this->textBox_Bat_ElpTimeChange->Location = System::Drawing::Point(238, 62);
      this->textBox_Bat_ElpTimeChange->MaxLength = 9;
      this->textBox_Bat_ElpTimeChange->Name = L"textBox_Bat_ElpTimeChange";
      this->textBox_Bat_ElpTimeChange->Size = System::Drawing::Size(63, 20);
      this->textBox_Bat_ElpTimeChange->TabIndex = 56;
      this->textBox_Bat_ElpTimeChange->Text = L"0";
      this->textBox_Bat_ElpTimeChange->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
      // 
      // checkBox_Bat_FracVolt
      // 
      this->checkBox_Bat_FracVolt->AutoSize = true;
      this->checkBox_Bat_FracVolt->Checked = true;
      this->checkBox_Bat_FracVolt->CheckState = System::Windows::Forms::CheckState::Checked;
      this->checkBox_Bat_FracVolt->Location = System::Drawing::Point(92, 89);
      this->checkBox_Bat_FracVolt->Name = L"checkBox_Bat_FracVolt";
      this->checkBox_Bat_FracVolt->Size = System::Drawing::Size(110, 17);
      this->checkBox_Bat_FracVolt->TabIndex = 54;
      this->checkBox_Bat_FracVolt->Text = L"Enable Frac. Volt.";
      this->checkBox_Bat_FracVolt->UseVisualStyleBackColor = true;
      this->checkBox_Bat_FracVolt->CheckedChanged += gcnew System::EventHandler(this, &BikePowerSensor::checkBox_Bat_FracVolt_CheckedChanged);
      // 
      // label_Bat_ElpTimeDisplay
      // 
      this->label_Bat_ElpTimeDisplay->Location = System::Drawing::Point(239, 49);
      this->label_Bat_ElpTimeDisplay->Name = L"label_Bat_ElpTimeDisplay";
      this->label_Bat_ElpTimeDisplay->Size = System::Drawing::Size(62, 13);
      this->label_Bat_ElpTimeDisplay->TabIndex = 21;
      this->label_Bat_ElpTimeDisplay->Text = L"0";
      this->label_Bat_ElpTimeDisplay->TextAlign = System::Drawing::ContentAlignment::BottomRight;
      // 
      // label_Bat_Volts
      // 
      this->label_Bat_Volts->AutoSize = true;
      this->label_Bat_Volts->Location = System::Drawing::Point(9, 57);
      this->label_Bat_Volts->Name = L"label_Bat_Volts";
      this->label_Bat_Volts->Size = System::Drawing::Size(56, 26);
      this->label_Bat_Volts->TabIndex = 30;
      this->label_Bat_Volts->Text = L"Fractional \r\n(1/256V)";
      // 
      // label_Bat_ElpTime
      // 
      this->label_Bat_ElpTime->AutoSize = true;
      this->label_Bat_ElpTime->Location = System::Drawing::Point(148, 48);
      this->label_Bat_ElpTime->Name = L"label_Bat_ElpTime";
      this->label_Bat_ElpTime->Size = System::Drawing::Size(88, 13);
      this->label_Bat_ElpTime->TabIndex = 19;
      this->label_Bat_ElpTime->Text = L"Elapsed Time (s):";
      // 
      // label_Volts
      // 
      this->label_Volts->AutoSize = true;
      this->label_Volts->Location = System::Drawing::Point(6, 21);
      this->label_Volts->Name = L"label_Volts";
      this->label_Volts->Size = System::Drawing::Size(98, 13);
      this->label_Volts->TabIndex = 60;
      this->label_Volts->Text = L"Battery Voltage (V):";
      // 
      // checkBox_Bat_Voltage
      // 
      this->checkBox_Bat_Voltage->AutoSize = true;
      this->checkBox_Bat_Voltage->Checked = true;
      this->checkBox_Bat_Voltage->CheckState = System::Windows::Forms::CheckState::Checked;
      this->checkBox_Bat_Voltage->Location = System::Drawing::Point(6, 89);
      this->checkBox_Bat_Voltage->Name = L"checkBox_Bat_Voltage";
      this->checkBox_Bat_Voltage->Size = System::Drawing::Size(83, 17);
      this->checkBox_Bat_Voltage->TabIndex = 53;
      this->checkBox_Bat_Voltage->Text = L"Enable Volt.";
      this->checkBox_Bat_Voltage->UseVisualStyleBackColor = true;
      this->checkBox_Bat_Voltage->CheckedChanged += gcnew System::EventHandler(this, &BikePowerSensor::checkBox_Bat_Voltage_CheckedChanged);
      // 
      // numericUpDown_Bat_VoltFrac
      // 
      this->numericUpDown_Bat_VoltFrac->Location = System::Drawing::Point(74, 63);
      this->numericUpDown_Bat_VoltFrac->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
      this->numericUpDown_Bat_VoltFrac->Name = L"numericUpDown_Bat_VoltFrac";
      this->numericUpDown_Bat_VoltFrac->Size = System::Drawing::Size(42, 20);
      this->numericUpDown_Bat_VoltFrac->TabIndex = 52;
      this->numericUpDown_Bat_VoltFrac->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {85, 0, 0, 0});
      this->numericUpDown_Bat_VoltFrac->ValueChanged += gcnew System::EventHandler(this, &BikePowerSensor::numericUpDown_Bat_VoltFrac_ValueChanged);
      // 
      // numericUpDown_Bat_VoltInt
      // 
      this->numericUpDown_Bat_VoltInt->Location = System::Drawing::Point(74, 37);
      this->numericUpDown_Bat_VoltInt->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {14, 0, 0, 0});
      this->numericUpDown_Bat_VoltInt->Name = L"numericUpDown_Bat_VoltInt";
      this->numericUpDown_Bat_VoltInt->Size = System::Drawing::Size(36, 20);
      this->numericUpDown_Bat_VoltInt->TabIndex = 51;
      this->numericUpDown_Bat_VoltInt->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {3, 0, 0, 0});
      this->numericUpDown_Bat_VoltInt->ValueChanged += gcnew System::EventHandler(this, &BikePowerSensor::numericUpDown_Bat_VoltInt_ValueChanged);
      // 
      // label_Bat_Status
      // 
      this->label_Bat_Status->AutoSize = true;
      this->label_Bat_Status->Location = System::Drawing::Point(310, 5);
      this->label_Bat_Status->Name = L"label_Bat_Status";
      this->label_Bat_Status->Size = System::Drawing::Size(73, 13);
      this->label_Bat_Status->TabIndex = 27;
      this->label_Bat_Status->Text = L"Battery Status";
      // 
      // listBox_Bat_Status
      // 
      this->listBox_Bat_Status->FormattingEnabled = true;
      this->listBox_Bat_Status->Items->AddRange(gcnew cli::array< System::Object^  >(6) {L"1 - New", L"2 - Good", L"3 - OK", L"4 - Low", 
         L"5 - Critical", L"7 - Disabled"});
      this->listBox_Bat_Status->Location = System::Drawing::Point(313, 22);
      this->listBox_Bat_Status->Name = L"listBox_Bat_Status";
      this->listBox_Bat_Status->Size = System::Drawing::Size(72, 82);
      this->listBox_Bat_Status->TabIndex = 55;
      this->listBox_Bat_Status->SelectedIndexChanged += gcnew System::EventHandler(this, &BikePowerSensor::listBox_Bat_Status_SelectedIndexChanged);
      // 
      // checkBox_Bat_Status
      // 
      this->checkBox_Bat_Status->AutoSize = true;
      this->checkBox_Bat_Status->Checked = true;
      this->checkBox_Bat_Status->CheckState = System::Windows::Forms::CheckState::Checked;
      this->checkBox_Bat_Status->Location = System::Drawing::Point(6, 4);
      this->checkBox_Bat_Status->Name = L"checkBox_Bat_Status";
      this->checkBox_Bat_Status->Size = System::Drawing::Size(156, 17);
      this->checkBox_Bat_Status->TabIndex = 50;
      this->checkBox_Bat_Status->Text = L"Enable Battery Status Page";
      this->checkBox_Bat_Status->UseVisualStyleBackColor = true;
      this->checkBox_Bat_Status->CheckedChanged += gcnew System::EventHandler(this, &BikePowerSensor::checkBox_Bat_Status_CheckedChanged);
      // 
      // tabPage_GetSet
      // 
      this->tabPage_GetSet->Controls->Add(this->button_UpdateGetSet);
      this->tabPage_GetSet->Controls->Add(this->checkBox_InvalidCrankLength);
      this->tabPage_GetSet->Controls->Add(this->label2);
      this->tabPage_GetSet->Controls->Add(this->numericUpDown_CrankLength);
      this->tabPage_GetSet->Controls->Add(this->groupBox3);
      this->tabPage_GetSet->Controls->Add(this->groupBox2);
      this->tabPage_GetSet->Controls->Add(this->groupBox1);
      this->tabPage_GetSet->Controls->Add(this->checkBox_AutoCrank);
      this->tabPage_GetSet->Location = System::Drawing::Point(4, 22);
      this->tabPage_GetSet->Name = L"tabPage_GetSet";
      this->tabPage_GetSet->Size = System::Drawing::Size(389, 111);
      this->tabPage_GetSet->TabIndex = 5;
      this->tabPage_GetSet->Text = L"Get/Set";
      this->tabPage_GetSet->UseVisualStyleBackColor = true;
      // 
      // button_UpdateGetSet
      // 
      this->button_UpdateGetSet->Location = System::Drawing::Point(6, 88);
      this->button_UpdateGetSet->Name = L"button_UpdateGetSet";
      this->button_UpdateGetSet->Size = System::Drawing::Size(75, 20);
      this->button_UpdateGetSet->TabIndex = 7;
      this->button_UpdateGetSet->Text = L"Update";
      this->button_UpdateGetSet->UseVisualStyleBackColor = true;
      this->button_UpdateGetSet->Click += gcnew System::EventHandler(this, &BikePowerSensor::button_UpdateGetSet_Click);
      // 
      // checkBox_InvalidCrankLength
      // 
      this->checkBox_InvalidCrankLength->AutoSize = true;
      this->checkBox_InvalidCrankLength->Location = System::Drawing::Point(6, 43);
      this->checkBox_InvalidCrankLength->Name = L"checkBox_InvalidCrankLength";
      this->checkBox_InvalidCrankLength->Size = System::Drawing::Size(57, 17);
      this->checkBox_InvalidCrankLength->TabIndex = 6;
      this->checkBox_InvalidCrankLength->Text = L"Invalid";
      this->checkBox_InvalidCrankLength->UseVisualStyleBackColor = true;
      this->checkBox_InvalidCrankLength->CheckedChanged += gcnew System::EventHandler(this, &BikePowerSensor::checkBox_InvalidCrankLength_CheckedChanged);
      // 
      // label2
      // 
      this->label2->AutoSize = true;
      this->label2->Location = System::Drawing::Point(3, 4);
      this->label2->Name = L"label2";
      this->label2->Size = System::Drawing::Size(74, 13);
      this->label2->TabIndex = 5;
      this->label2->Text = L"Crank Length:";
      // 
      // numericUpDown_CrankLength
      // 
      this->numericUpDown_CrankLength->DecimalPlaces = 1;
      this->numericUpDown_CrankLength->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {5, 0, 0, 65536});
      this->numericUpDown_CrankLength->Location = System::Drawing::Point(6, 20);
      this->numericUpDown_CrankLength->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {2365, 0, 0, 65536});
      this->numericUpDown_CrankLength->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1100, 0, 0, 65536});
      this->numericUpDown_CrankLength->Name = L"numericUpDown_CrankLength";
      this->numericUpDown_CrankLength->Size = System::Drawing::Size(68, 20);
      this->numericUpDown_CrankLength->TabIndex = 4;
      this->numericUpDown_CrankLength->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1725, 0, 0, 65536});
      // 
      // groupBox3
      // 
      this->groupBox3->Controls->Add(this->radioButton_CrankAuto);
      this->groupBox3->Controls->Add(this->radioButton_CrankDefault);
      this->groupBox3->Controls->Add(this->radioButton_CrankInvalid);
      this->groupBox3->Controls->Add(this->radioButton_CrankManual);
      this->groupBox3->Location = System::Drawing::Point(86, 2);
      this->groupBox3->Name = L"groupBox3";
      this->groupBox3->Size = System::Drawing::Size(98, 106);
      this->groupBox3->TabIndex = 2;
      this->groupBox3->TabStop = false;
      this->groupBox3->Text = L"Crank Length";
      // 
      // radioButton_CrankAuto
      // 
      this->radioButton_CrankAuto->AutoSize = true;
      this->radioButton_CrankAuto->Location = System::Drawing::Point(6, 74);
      this->radioButton_CrankAuto->Margin = System::Windows::Forms::Padding(1);
      this->radioButton_CrankAuto->Name = L"radioButton_CrankAuto";
      this->radioButton_CrankAuto->Size = System::Drawing::Size(69, 17);
      this->radioButton_CrankAuto->TabIndex = 3;
      this->radioButton_CrankAuto->Text = L"Auto. Set";
      this->radioButton_CrankAuto->UseVisualStyleBackColor = true;
      // 
      // radioButton_CrankDefault
      // 
      this->radioButton_CrankDefault->AutoSize = true;
      this->radioButton_CrankDefault->Location = System::Drawing::Point(6, 36);
      this->radioButton_CrankDefault->Margin = System::Windows::Forms::Padding(1);
      this->radioButton_CrankDefault->Name = L"radioButton_CrankDefault";
      this->radioButton_CrankDefault->Size = System::Drawing::Size(59, 17);
      this->radioButton_CrankDefault->TabIndex = 1;
      this->radioButton_CrankDefault->Text = L"Default";
      this->radioButton_CrankDefault->UseVisualStyleBackColor = true;
      // 
      // radioButton_CrankInvalid
      // 
      this->radioButton_CrankInvalid->AutoSize = true;
      this->radioButton_CrankInvalid->Location = System::Drawing::Point(6, 17);
      this->radioButton_CrankInvalid->Margin = System::Windows::Forms::Padding(1);
      this->radioButton_CrankInvalid->Name = L"radioButton_CrankInvalid";
      this->radioButton_CrankInvalid->Size = System::Drawing::Size(56, 17);
      this->radioButton_CrankInvalid->TabIndex = 0;
      this->radioButton_CrankInvalid->Text = L"Invalid";
      this->radioButton_CrankInvalid->UseVisualStyleBackColor = true;
      // 
      // radioButton_CrankManual
      // 
      this->radioButton_CrankManual->AutoSize = true;
      this->radioButton_CrankManual->Checked = true;
      this->radioButton_CrankManual->Location = System::Drawing::Point(6, 55);
      this->radioButton_CrankManual->Margin = System::Windows::Forms::Padding(1);
      this->radioButton_CrankManual->Name = L"radioButton_CrankManual";
      this->radioButton_CrankManual->Size = System::Drawing::Size(86, 17);
      this->radioButton_CrankManual->TabIndex = 2;
      this->radioButton_CrankManual->TabStop = true;
      this->radioButton_CrankManual->Text = L"Manually Set";
      this->radioButton_CrankManual->UseVisualStyleBackColor = true;
      // 
      // groupBox2
      // 
      this->groupBox2->Controls->Add(this->radioButton_MismatchLeft);
      this->groupBox2->Controls->Add(this->radioButton_MismatchRight);
      this->groupBox2->Controls->Add(this->radioButton_MismatchUndefined);
      this->groupBox2->Controls->Add(this->radioButton_MismatchNone);
      this->groupBox2->Location = System::Drawing::Point(187, 3);
      this->groupBox2->Name = L"groupBox2";
      this->groupBox2->Size = System::Drawing::Size(99, 105);
      this->groupBox2->TabIndex = 1;
      this->groupBox2->TabStop = false;
      this->groupBox2->Text = L"SW Mismatch";
      // 
      // radioButton_MismatchLeft
      // 
      this->radioButton_MismatchLeft->AutoSize = true;
      this->radioButton_MismatchLeft->Location = System::Drawing::Point(6, 55);
      this->radioButton_MismatchLeft->Margin = System::Windows::Forms::Padding(1);
      this->radioButton_MismatchLeft->Name = L"radioButton_MismatchLeft";
      this->radioButton_MismatchLeft->Size = System::Drawing::Size(71, 17);
      this->radioButton_MismatchLeft->TabIndex = 2;
      this->radioButton_MismatchLeft->Text = L"Left Older";
      this->radioButton_MismatchLeft->UseVisualStyleBackColor = true;
      // 
      // radioButton_MismatchRight
      // 
      this->radioButton_MismatchRight->AutoSize = true;
      this->radioButton_MismatchRight->Location = System::Drawing::Point(6, 36);
      this->radioButton_MismatchRight->Margin = System::Windows::Forms::Padding(1);
      this->radioButton_MismatchRight->Name = L"radioButton_MismatchRight";
      this->radioButton_MismatchRight->Size = System::Drawing::Size(78, 17);
      this->radioButton_MismatchRight->TabIndex = 1;
      this->radioButton_MismatchRight->Text = L"Right Older";
      this->radioButton_MismatchRight->UseVisualStyleBackColor = true;
      // 
      // radioButton_MismatchUndefined
      // 
      this->radioButton_MismatchUndefined->AutoSize = true;
      this->radioButton_MismatchUndefined->Location = System::Drawing::Point(6, 17);
      this->radioButton_MismatchUndefined->Margin = System::Windows::Forms::Padding(1);
      this->radioButton_MismatchUndefined->Name = L"radioButton_MismatchUndefined";
      this->radioButton_MismatchUndefined->Size = System::Drawing::Size(74, 17);
      this->radioButton_MismatchUndefined->TabIndex = 0;
      this->radioButton_MismatchUndefined->Text = L"Undefined";
      this->radioButton_MismatchUndefined->UseVisualStyleBackColor = true;
      // 
      // radioButton_MismatchNone
      // 
      this->radioButton_MismatchNone->AutoSize = true;
      this->radioButton_MismatchNone->Checked = true;
      this->radioButton_MismatchNone->Location = System::Drawing::Point(6, 74);
      this->radioButton_MismatchNone->Margin = System::Windows::Forms::Padding(1);
      this->radioButton_MismatchNone->Name = L"radioButton_MismatchNone";
      this->radioButton_MismatchNone->Size = System::Drawing::Size(87, 17);
      this->radioButton_MismatchNone->TabIndex = 3;
      this->radioButton_MismatchNone->TabStop = true;
      this->radioButton_MismatchNone->Text = L"No Mismatch";
      this->radioButton_MismatchNone->UseVisualStyleBackColor = true;
      // 
      // groupBox1
      // 
      this->groupBox1->Controls->Add(this->radioButton_SensorBoth);
      this->groupBox1->Controls->Add(this->radioButton_SensorLeft);
      this->groupBox1->Controls->Add(this->radioButton_SensorUndefined);
      this->groupBox1->Controls->Add(this->radioButton_SensorRight);
      this->groupBox1->Location = System::Drawing::Point(289, 3);
      this->groupBox1->Name = L"groupBox1";
      this->groupBox1->Size = System::Drawing::Size(100, 105);
      this->groupBox1->TabIndex = 0;
      this->groupBox1->TabStop = false;
      this->groupBox1->Text = L"Sensor Status";
      // 
      // radioButton_SensorBoth
      // 
      this->radioButton_SensorBoth->AutoSize = true;
      this->radioButton_SensorBoth->Checked = true;
      this->radioButton_SensorBoth->Location = System::Drawing::Point(6, 74);
      this->radioButton_SensorBoth->Margin = System::Windows::Forms::Padding(1);
      this->radioButton_SensorBoth->Name = L"radioButton_SensorBoth";
      this->radioButton_SensorBoth->Size = System::Drawing::Size(86, 17);
      this->radioButton_SensorBoth->TabIndex = 3;
      this->radioButton_SensorBoth->TabStop = true;
      this->radioButton_SensorBoth->Text = L"Both Present";
      this->radioButton_SensorBoth->UseVisualStyleBackColor = true;
      // 
      // radioButton_SensorLeft
      // 
      this->radioButton_SensorLeft->AutoSize = true;
      this->radioButton_SensorLeft->Location = System::Drawing::Point(6, 36);
      this->radioButton_SensorLeft->Margin = System::Windows::Forms::Padding(1);
      this->radioButton_SensorLeft->Name = L"radioButton_SensorLeft";
      this->radioButton_SensorLeft->Size = System::Drawing::Size(82, 17);
      this->radioButton_SensorLeft->TabIndex = 1;
      this->radioButton_SensorLeft->Text = L"Left Present";
      this->radioButton_SensorLeft->UseVisualStyleBackColor = true;
      // 
      // radioButton_SensorUndefined
      // 
      this->radioButton_SensorUndefined->AutoSize = true;
      this->radioButton_SensorUndefined->Location = System::Drawing::Point(6, 17);
      this->radioButton_SensorUndefined->Margin = System::Windows::Forms::Padding(1);
      this->radioButton_SensorUndefined->Name = L"radioButton_SensorUndefined";
      this->radioButton_SensorUndefined->Size = System::Drawing::Size(74, 17);
      this->radioButton_SensorUndefined->TabIndex = 0;
      this->radioButton_SensorUndefined->Text = L"Undefined";
      this->radioButton_SensorUndefined->UseVisualStyleBackColor = true;
      // 
      // radioButton_SensorRight
      // 
      this->radioButton_SensorRight->AutoSize = true;
      this->radioButton_SensorRight->Location = System::Drawing::Point(6, 55);
      this->radioButton_SensorRight->Margin = System::Windows::Forms::Padding(1);
      this->radioButton_SensorRight->Name = L"radioButton_SensorRight";
      this->radioButton_SensorRight->Size = System::Drawing::Size(89, 17);
      this->radioButton_SensorRight->TabIndex = 2;
      this->radioButton_SensorRight->Text = L"Right Present";
      this->radioButton_SensorRight->UseVisualStyleBackColor = true;
      // 
      // checkBox_AutoCrank
      // 
      this->checkBox_AutoCrank->Location = System::Drawing::Point(6, 58);
      this->checkBox_AutoCrank->Name = L"checkBox_AutoCrank";
      this->checkBox_AutoCrank->Size = System::Drawing::Size(88, 34);
      this->checkBox_AutoCrank->TabIndex = 3;
      this->checkBox_AutoCrank->Text = L"Auto Crank Length";
      this->checkBox_AutoCrank->UseVisualStyleBackColor = true;
      // 
      // tabControl_TEPS
      // 
      this->tabControl_TEPS->Controls->Add(this->groupBox5);
      this->tabControl_TEPS->Controls->Add(this->groupBox4);
      this->tabControl_TEPS->Controls->Add(this->checkBox_Combined);
      this->tabControl_TEPS->Controls->Add(this->checkBox_EnableTEPS);
      this->tabControl_TEPS->Location = System::Drawing::Point(4, 22);
      this->tabControl_TEPS->Name = L"tabControl_TEPS";
      this->tabControl_TEPS->Padding = System::Windows::Forms::Padding(3);
      this->tabControl_TEPS->Size = System::Drawing::Size(389, 111);
      this->tabControl_TEPS->TabIndex = 6;
      this->tabControl_TEPS->Text = L"TE&PS";
      this->tabControl_TEPS->UseVisualStyleBackColor = true;
      // 
      // groupBox5
      // 
      this->groupBox5->Controls->Add(this->checkBox_RightPedalSmoothnessInvalid);
      this->groupBox5->Controls->Add(this->numericUpDown_RightPedalSmoothness);
      this->groupBox5->Controls->Add(this->label5);
      this->groupBox5->Controls->Add(this->checkBox_LeftPedalSmoothnessInvalid);
      this->groupBox5->Controls->Add(this->numericUpDown_LeftPedalSmoothness);
      this->groupBox5->Controls->Add(this->label6);
      this->groupBox5->Location = System::Drawing::Point(198, 30);
      this->groupBox5->Name = L"groupBox5";
      this->groupBox5->Size = System::Drawing::Size(174, 75);
      this->groupBox5->TabIndex = 6;
      this->groupBox5->TabStop = false;
      this->groupBox5->Text = L"Pedal Smoothness (%)";
      // 
      // checkBox_RightPedalSmoothnessInvalid
      // 
      this->checkBox_RightPedalSmoothnessInvalid->AutoSize = true;
      this->checkBox_RightPedalSmoothnessInvalid->Location = System::Drawing::Point(112, 45);
      this->checkBox_RightPedalSmoothnessInvalid->Name = L"checkBox_RightPedalSmoothnessInvalid";
      this->checkBox_RightPedalSmoothnessInvalid->Size = System::Drawing::Size(57, 17);
      this->checkBox_RightPedalSmoothnessInvalid->TabIndex = 5;
      this->checkBox_RightPedalSmoothnessInvalid->Text = L"Invalid";
      this->checkBox_RightPedalSmoothnessInvalid->UseVisualStyleBackColor = true;
      this->checkBox_RightPedalSmoothnessInvalid->CheckedChanged += gcnew System::EventHandler(this, &BikePowerSensor::checkBox_RightPedalSmoothnessInvalid_CheckedChanged);
      // 
      // numericUpDown_RightPedalSmoothness
      // 
      this->numericUpDown_RightPedalSmoothness->DecimalPlaces = 1;
      this->numericUpDown_RightPedalSmoothness->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {5, 0, 0, 65536});
      this->numericUpDown_RightPedalSmoothness->Location = System::Drawing::Point(42, 44);
      this->numericUpDown_RightPedalSmoothness->Name = L"numericUpDown_RightPedalSmoothness";
      this->numericUpDown_RightPedalSmoothness->Size = System::Drawing::Size(64, 20);
      this->numericUpDown_RightPedalSmoothness->TabIndex = 4;
      this->numericUpDown_RightPedalSmoothness->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {420, 0, 0, 65536});
      this->numericUpDown_RightPedalSmoothness->ValueChanged += gcnew System::EventHandler(this, &BikePowerSensor::numericUpDown_RightPedalSmoothness_ValueChanged);
      // 
      // label5
      // 
      this->label5->AutoSize = true;
      this->label5->Location = System::Drawing::Point(8, 46);
      this->label5->Name = L"label5";
      this->label5->Size = System::Drawing::Size(32, 13);
      this->label5->TabIndex = 3;
      this->label5->Text = L"Right";
      // 
      // checkBox_LeftPedalSmoothnessInvalid
      // 
      this->checkBox_LeftPedalSmoothnessInvalid->AutoSize = true;
      this->checkBox_LeftPedalSmoothnessInvalid->Location = System::Drawing::Point(112, 21);
      this->checkBox_LeftPedalSmoothnessInvalid->Name = L"checkBox_LeftPedalSmoothnessInvalid";
      this->checkBox_LeftPedalSmoothnessInvalid->Size = System::Drawing::Size(57, 17);
      this->checkBox_LeftPedalSmoothnessInvalid->TabIndex = 2;
      this->checkBox_LeftPedalSmoothnessInvalid->Text = L"Invalid";
      this->checkBox_LeftPedalSmoothnessInvalid->UseVisualStyleBackColor = true;
      this->checkBox_LeftPedalSmoothnessInvalid->CheckedChanged += gcnew System::EventHandler(this, &BikePowerSensor::checkBox_LeftPedalSmoothnessInvalid_CheckedChanged);
      // 
      // numericUpDown_LeftPedalSmoothness
      // 
      this->numericUpDown_LeftPedalSmoothness->DecimalPlaces = 1;
      this->numericUpDown_LeftPedalSmoothness->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {5, 0, 0, 65536});
      this->numericUpDown_LeftPedalSmoothness->Location = System::Drawing::Point(42, 20);
      this->numericUpDown_LeftPedalSmoothness->Name = L"numericUpDown_LeftPedalSmoothness";
      this->numericUpDown_LeftPedalSmoothness->Size = System::Drawing::Size(64, 20);
      this->numericUpDown_LeftPedalSmoothness->TabIndex = 1;
      this->numericUpDown_LeftPedalSmoothness->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {355, 0, 0, 65536});
      this->numericUpDown_LeftPedalSmoothness->ValueChanged += gcnew System::EventHandler(this, &BikePowerSensor::numericUpDown_LeftPedalSmoothness_ValueChanged);
      // 
      // label6
      // 
      this->label6->AutoSize = true;
      this->label6->Location = System::Drawing::Point(8, 22);
      this->label6->Name = L"label6";
      this->label6->Size = System::Drawing::Size(28, 13);
      this->label6->TabIndex = 0;
      this->label6->Text = L"Left:";
      // 
      // groupBox4
      // 
      this->groupBox4->Controls->Add(this->checkBox_RightTorqueEffectivenessInvalid);
      this->groupBox4->Controls->Add(this->numericUpDown_RightTorqueEffectiveness);
      this->groupBox4->Controls->Add(this->label4);
      this->groupBox4->Controls->Add(this->checkBox_LeftTorqueEffectivenessInvalid);
      this->groupBox4->Controls->Add(this->numericUpDown_LeftTorqueEffectiveness);
      this->groupBox4->Controls->Add(this->label3);
      this->groupBox4->Location = System::Drawing::Point(7, 30);
      this->groupBox4->Name = L"groupBox4";
      this->groupBox4->Size = System::Drawing::Size(174, 75);
      this->groupBox4->TabIndex = 3;
      this->groupBox4->TabStop = false;
      this->groupBox4->Text = L"Torque Effectiveness (%)";
      // 
      // checkBox_RightTorqueEffectivenessInvalid
      // 
      this->checkBox_RightTorqueEffectivenessInvalid->AutoSize = true;
      this->checkBox_RightTorqueEffectivenessInvalid->Location = System::Drawing::Point(112, 45);
      this->checkBox_RightTorqueEffectivenessInvalid->Name = L"checkBox_RightTorqueEffectivenessInvalid";
      this->checkBox_RightTorqueEffectivenessInvalid->Size = System::Drawing::Size(57, 17);
      this->checkBox_RightTorqueEffectivenessInvalid->TabIndex = 5;
      this->checkBox_RightTorqueEffectivenessInvalid->Text = L"Invalid";
      this->checkBox_RightTorqueEffectivenessInvalid->UseVisualStyleBackColor = true;
      this->checkBox_RightTorqueEffectivenessInvalid->CheckedChanged += gcnew System::EventHandler(this, &BikePowerSensor::checkBox_RightTorqueEffectivenessInvalid_CheckedChanged);
      // 
      // numericUpDown_RightTorqueEffectiveness
      // 
      this->numericUpDown_RightTorqueEffectiveness->DecimalPlaces = 1;
      this->numericUpDown_RightTorqueEffectiveness->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {5, 0, 0, 
         65536});
      this->numericUpDown_RightTorqueEffectiveness->Location = System::Drawing::Point(42, 44);
      this->numericUpDown_RightTorqueEffectiveness->Name = L"numericUpDown_RightTorqueEffectiveness";
      this->numericUpDown_RightTorqueEffectiveness->Size = System::Drawing::Size(64, 20);
      this->numericUpDown_RightTorqueEffectiveness->TabIndex = 4;
      this->numericUpDown_RightTorqueEffectiveness->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {420, 0, 0, 65536});
      this->numericUpDown_RightTorqueEffectiveness->ValueChanged += gcnew System::EventHandler(this, &BikePowerSensor::numericUpDown_RightTorqueEffectiveness_ValueChanged);
      // 
      // label4
      // 
      this->label4->AutoSize = true;
      this->label4->Location = System::Drawing::Point(8, 46);
      this->label4->Name = L"label4";
      this->label4->Size = System::Drawing::Size(32, 13);
      this->label4->TabIndex = 3;
      this->label4->Text = L"Right";
      // 
      // checkBox_LeftTorqueEffectivenessInvalid
      // 
      this->checkBox_LeftTorqueEffectivenessInvalid->AutoSize = true;
      this->checkBox_LeftTorqueEffectivenessInvalid->Location = System::Drawing::Point(112, 21);
      this->checkBox_LeftTorqueEffectivenessInvalid->Name = L"checkBox_LeftTorqueEffectivenessInvalid";
      this->checkBox_LeftTorqueEffectivenessInvalid->Size = System::Drawing::Size(57, 17);
      this->checkBox_LeftTorqueEffectivenessInvalid->TabIndex = 2;
      this->checkBox_LeftTorqueEffectivenessInvalid->Text = L"Invalid";
      this->checkBox_LeftTorqueEffectivenessInvalid->UseVisualStyleBackColor = true;
      this->checkBox_LeftTorqueEffectivenessInvalid->CheckedChanged += gcnew System::EventHandler(this, &BikePowerSensor::checkBox_LeftTorqueEffectivenessInvalid_CheckedChanged);
      // 
      // numericUpDown_LeftTorqueEffectiveness
      // 
      this->numericUpDown_LeftTorqueEffectiveness->DecimalPlaces = 1;
      this->numericUpDown_LeftTorqueEffectiveness->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {5, 0, 0, 65536});
      this->numericUpDown_LeftTorqueEffectiveness->Location = System::Drawing::Point(42, 20);
      this->numericUpDown_LeftTorqueEffectiveness->Name = L"numericUpDown_LeftTorqueEffectiveness";
      this->numericUpDown_LeftTorqueEffectiveness->Size = System::Drawing::Size(64, 20);
      this->numericUpDown_LeftTorqueEffectiveness->TabIndex = 1;
      this->numericUpDown_LeftTorqueEffectiveness->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {355, 0, 0, 65536});
      this->numericUpDown_LeftTorqueEffectiveness->ValueChanged += gcnew System::EventHandler(this, &BikePowerSensor::numericUpDown_LeftTorqueEffectiveness_ValueChanged);
      // 
      // label3
      // 
      this->label3->AutoSize = true;
      this->label3->Location = System::Drawing::Point(8, 22);
      this->label3->Name = L"label3";
      this->label3->Size = System::Drawing::Size(28, 13);
      this->label3->TabIndex = 0;
      this->label3->Text = L"Left:";
      // 
      // checkBox_Combined
      // 
      this->checkBox_Combined->AutoSize = true;
      this->checkBox_Combined->Location = System::Drawing::Point(262, 7);
      this->checkBox_Combined->Name = L"checkBox_Combined";
      this->checkBox_Combined->Size = System::Drawing::Size(110, 17);
      this->checkBox_Combined->TabIndex = 2;
      this->checkBox_Combined->Text = L"Combined System";
      this->checkBox_Combined->UseVisualStyleBackColor = true;
      this->checkBox_Combined->CheckedChanged += gcnew System::EventHandler(this, &BikePowerSensor::checkBox_Combined_CheckedChanged);
      // 
      // checkBox_EnableTEPS
      // 
      this->checkBox_EnableTEPS->AutoSize = true;
      this->checkBox_EnableTEPS->Location = System::Drawing::Point(7, 7);
      this->checkBox_EnableTEPS->Name = L"checkBox_EnableTEPS";
      this->checkBox_EnableTEPS->Size = System::Drawing::Size(87, 17);
      this->checkBox_EnableTEPS->TabIndex = 0;
      this->checkBox_EnableTEPS->Text = L"Enable Page";
      this->checkBox_EnableTEPS->UseVisualStyleBackColor = true;
      this->checkBox_EnableTEPS->CheckedChanged += gcnew System::EventHandler(this, &BikePowerSensor::checkBox_EnableTEPS_CheckedChanged);
      // 
      // panel_Display
      // 
      this->panel_Display->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
      this->panel_Display->Controls->Add(this->label_Trn_PedalDisplay);
      this->panel_Display->Controls->Add(this->label_Trn_PedalPwrDisplay);
      this->panel_Display->Controls->Add(this->label_Trn_PedalPwr);
      this->panel_Display->Controls->Add(this->label_Trn_UpdateCountDisplay);
      this->panel_Display->Controls->Add(this->label_Trn_UpdateCount);
      this->panel_Display->Controls->Add(this->label_Trn_EventCount);
      this->panel_Display->Controls->Add(this->label_Trn_AccumOneDisplay);
      this->panel_Display->Controls->Add(this->label_Trn_AccumTwo);
      this->panel_Display->Controls->Add(this->label_Trn_PowerDisplay);
      this->panel_Display->Controls->Add(this->label_Trn_Power);
      this->panel_Display->Controls->Add(this->label_Trn_AccumTwoDisplay);
      this->panel_Display->Controls->Add(this->label_Trn_CadenceDisplay);
      this->panel_Display->Controls->Add(this->label_Trn_Cadence);
      this->panel_Display->Controls->Add(this->label_Trn_InstSpeedDisplay);
      this->panel_Display->Controls->Add(this->label_Trn_EventCountDisplay);
      this->panel_Display->Controls->Add(this->label_Trn_InstSpeed);
      this->panel_Display->Controls->Add(this->label_Trn_DisplayTitle);
      this->panel_Display->Controls->Add(this->label_Trn_AccumOne);
      this->panel_Display->Controls->Add(this->label_Trn_AccumTitle);
      this->panel_Display->Location = System::Drawing::Point(58, 188);
      this->panel_Display->Name = L"panel_Display";
      this->panel_Display->Size = System::Drawing::Size(242, 108);
      this->panel_Display->TabIndex = 1;
      // 
      // label_Trn_PedalDisplay
      // 
      this->label_Trn_PedalDisplay->AutoSize = true;
      this->label_Trn_PedalDisplay->Location = System::Drawing::Point(186, 88);
      this->label_Trn_PedalDisplay->Name = L"label_Trn_PedalDisplay";
      this->label_Trn_PedalDisplay->Size = System::Drawing::Size(16, 13);
      this->label_Trn_PedalDisplay->TabIndex = 21;
      this->label_Trn_PedalDisplay->Text = L"---";
      // 
      // label_Trn_PedalPwrDisplay
      // 
      this->label_Trn_PedalPwrDisplay->AutoSize = true;
      this->label_Trn_PedalPwrDisplay->Location = System::Drawing::Point(164, 88);
      this->label_Trn_PedalPwrDisplay->Name = L"label_Trn_PedalPwrDisplay";
      this->label_Trn_PedalPwrDisplay->Size = System::Drawing::Size(16, 13);
      this->label_Trn_PedalPwrDisplay->TabIndex = 19;
      this->label_Trn_PedalPwrDisplay->Text = L"---";
      // 
      // label_Trn_PedalPwr
      // 
      this->label_Trn_PedalPwr->AutoSize = true;
      this->label_Trn_PedalPwr->Location = System::Drawing::Point(79, 88);
      this->label_Trn_PedalPwr->Name = L"label_Trn_PedalPwr";
      this->label_Trn_PedalPwr->Size = System::Drawing::Size(87, 13);
      this->label_Trn_PedalPwr->TabIndex = 18;
      this->label_Trn_PedalPwr->Text = L"Pedal Power (%):";
      // 
      // label_Trn_UpdateCountDisplay
      // 
      this->label_Trn_UpdateCountDisplay->BackColor = System::Drawing::SystemColors::Control;
      this->label_Trn_UpdateCountDisplay->Location = System::Drawing::Point(164, 73);
      this->label_Trn_UpdateCountDisplay->Name = L"label_Trn_UpdateCountDisplay";
      this->label_Trn_UpdateCountDisplay->Size = System::Drawing::Size(34, 13);
      this->label_Trn_UpdateCountDisplay->TabIndex = 12;
      this->label_Trn_UpdateCountDisplay->Text = L"---";
      // 
      // label_Trn_UpdateCount
      // 
      this->label_Trn_UpdateCount->AutoSize = true;
      this->label_Trn_UpdateCount->BackColor = System::Drawing::SystemColors::Control;
      this->label_Trn_UpdateCount->Location = System::Drawing::Point(115, 72);
      this->label_Trn_UpdateCount->Name = L"label_Trn_UpdateCount";
      this->label_Trn_UpdateCount->Size = System::Drawing::Size(50, 13);
      this->label_Trn_UpdateCount->TabIndex = 11;
      this->label_Trn_UpdateCount->Text = L"Updates:";
      // 
      // label_Trn_EventCount
      // 
      this->label_Trn_EventCount->AutoSize = true;
      this->label_Trn_EventCount->BackColor = System::Drawing::SystemColors::Control;
      this->label_Trn_EventCount->Location = System::Drawing::Point(122, 58);
      this->label_Trn_EventCount->Name = L"label_Trn_EventCount";
      this->label_Trn_EventCount->Size = System::Drawing::Size(43, 13);
      this->label_Trn_EventCount->TabIndex = 0;
      this->label_Trn_EventCount->Text = L"Events:";
      // 
      // label_Trn_AccumOneDisplay
      // 
      this->label_Trn_AccumOneDisplay->BackColor = System::Drawing::SystemColors::Control;
      this->label_Trn_AccumOneDisplay->Location = System::Drawing::Point(70, 58);
      this->label_Trn_AccumOneDisplay->Name = L"label_Trn_AccumOneDisplay";
      this->label_Trn_AccumOneDisplay->Size = System::Drawing::Size(40, 13);
      this->label_Trn_AccumOneDisplay->TabIndex = 8;
      this->label_Trn_AccumOneDisplay->Text = L"---";
      // 
      // label_Trn_AccumTwo
      // 
      this->label_Trn_AccumTwo->AutoSize = true;
      this->label_Trn_AccumTwo->BackColor = System::Drawing::SystemColors::Control;
      this->label_Trn_AccumTwo->Location = System::Drawing::Point(6, 72);
      this->label_Trn_AccumTwo->Name = L"label_Trn_AccumTwo";
      this->label_Trn_AccumTwo->Size = System::Drawing::Size(66, 13);
      this->label_Trn_AccumTwo->TabIndex = 15;
      this->label_Trn_AccumTwo->Text = L"Acc.Torque:";
      this->label_Trn_AccumTwo->Visible = false;
      // 
      // label_Trn_PowerDisplay
      // 
      this->label_Trn_PowerDisplay->BackColor = System::Drawing::SystemColors::Control;
      this->label_Trn_PowerDisplay->Location = System::Drawing::Point(164, 45);
      this->label_Trn_PowerDisplay->Name = L"label_Trn_PowerDisplay";
      this->label_Trn_PowerDisplay->Size = System::Drawing::Size(34, 13);
      this->label_Trn_PowerDisplay->TabIndex = 14;
      this->label_Trn_PowerDisplay->Text = L"---";
      // 
      // label_Trn_Power
      // 
      this->label_Trn_Power->AutoSize = true;
      this->label_Trn_Power->BackColor = System::Drawing::SystemColors::Control;
      this->label_Trn_Power->Location = System::Drawing::Point(105, 45);
      this->label_Trn_Power->Name = L"label_Trn_Power";
      this->label_Trn_Power->Size = System::Drawing::Size(60, 13);
      this->label_Trn_Power->TabIndex = 13;
      this->label_Trn_Power->Text = L"Power (W):";
      // 
      // label_Trn_AccumTwoDisplay
      // 
      this->label_Trn_AccumTwoDisplay->BackColor = System::Drawing::SystemColors::Control;
      this->label_Trn_AccumTwoDisplay->Location = System::Drawing::Point(70, 72);
      this->label_Trn_AccumTwoDisplay->Name = L"label_Trn_AccumTwoDisplay";
      this->label_Trn_AccumTwoDisplay->Size = System::Drawing::Size(40, 13);
      this->label_Trn_AccumTwoDisplay->TabIndex = 16;
      this->label_Trn_AccumTwoDisplay->Text = L"---";
      this->label_Trn_AccumTwoDisplay->Visible = false;
      // 
      // label_Trn_CadenceDisplay
      // 
      this->label_Trn_CadenceDisplay->BackColor = System::Drawing::SystemColors::Control;
      this->label_Trn_CadenceDisplay->Location = System::Drawing::Point(164, 31);
      this->label_Trn_CadenceDisplay->Name = L"label_Trn_CadenceDisplay";
      this->label_Trn_CadenceDisplay->Size = System::Drawing::Size(34, 13);
      this->label_Trn_CadenceDisplay->TabIndex = 10;
      this->label_Trn_CadenceDisplay->Text = L"---";
      // 
      // label_Trn_Cadence
      // 
      this->label_Trn_Cadence->AutoSize = true;
      this->label_Trn_Cadence->BackColor = System::Drawing::SystemColors::Control;
      this->label_Trn_Cadence->Location = System::Drawing::Point(86, 30);
      this->label_Trn_Cadence->Name = L"label_Trn_Cadence";
      this->label_Trn_Cadence->Size = System::Drawing::Size(79, 13);
      this->label_Trn_Cadence->TabIndex = 9;
      this->label_Trn_Cadence->Text = L"Cadence (rpm):";
      // 
      // label_Trn_InstSpeedDisplay
      // 
      this->label_Trn_InstSpeedDisplay->BackColor = System::Drawing::SystemColors::Control;
      this->label_Trn_InstSpeedDisplay->Location = System::Drawing::Point(164, 17);
      this->label_Trn_InstSpeedDisplay->Name = L"label_Trn_InstSpeedDisplay";
      this->label_Trn_InstSpeedDisplay->Size = System::Drawing::Size(34, 13);
      this->label_Trn_InstSpeedDisplay->TabIndex = 3;
      this->label_Trn_InstSpeedDisplay->Text = L"---";
      // 
      // label_Trn_EventCountDisplay
      // 
      this->label_Trn_EventCountDisplay->BackColor = System::Drawing::SystemColors::Control;
      this->label_Trn_EventCountDisplay->Location = System::Drawing::Point(164, 59);
      this->label_Trn_EventCountDisplay->Name = L"label_Trn_EventCountDisplay";
      this->label_Trn_EventCountDisplay->Size = System::Drawing::Size(34, 13);
      this->label_Trn_EventCountDisplay->TabIndex = 2;
      this->label_Trn_EventCountDisplay->Text = L"---";
      // 
      // label_Trn_InstSpeed
      // 
      this->label_Trn_InstSpeed->AutoSize = true;
      this->label_Trn_InstSpeed->BackColor = System::Drawing::SystemColors::Control;
      this->label_Trn_InstSpeed->Location = System::Drawing::Point(27, 16);
      this->label_Trn_InstSpeed->Name = L"label_Trn_InstSpeed";
      this->label_Trn_InstSpeed->Size = System::Drawing::Size(138, 13);
      this->label_Trn_InstSpeed->TabIndex = 1;
      this->label_Trn_InstSpeed->Text = L"Instantaneous Speed (kph):";
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
      // label_Trn_AccumOne
      // 
      this->label_Trn_AccumOne->AutoSize = true;
      this->label_Trn_AccumOne->BackColor = System::Drawing::SystemColors::Control;
      this->label_Trn_AccumOne->Location = System::Drawing::Point(2, 58);
      this->label_Trn_AccumOne->Name = L"label_Trn_AccumOne";
      this->label_Trn_AccumOne->Size = System::Drawing::Size(62, 13);
      this->label_Trn_AccumOne->TabIndex = 7;
      this->label_Trn_AccumOne->Text = L"Acc.Power:";
      // 
      // label_Trn_AccumTitle
      // 
      this->label_Trn_AccumTitle->AutoSize = true;
      this->label_Trn_AccumTitle->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Italic, 
         System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
      this->label_Trn_AccumTitle->Location = System::Drawing::Point(1, 44);
      this->label_Trn_AccumTitle->Name = L"label_Trn_AccumTitle";
      this->label_Trn_AccumTitle->Size = System::Drawing::Size(71, 13);
      this->label_Trn_AccumTitle->TabIndex = 17;
      this->label_Trn_AccumTitle->Text = L"Accumulates:";
      // 
      // BikePowerSensor
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(794, 351);
      this->Controls->Add(this->panel_Display);
      this->Controls->Add(this->panel_Settings);
      this->Name = L"BikePowerSensor";
      this->Text = L"BikePowerSensor";
      this->panel_Settings->ResumeLayout(false);
      this->tabControl_Settings->ResumeLayout(false);
      this->tabPage_Simulation->ResumeLayout(false);
      this->tabPage_Simulation->PerformLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_PedalPwr_CurrOutput))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_WheelCircumference))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_MaxOutput))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_CurOutput))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_MinOutput))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_WheelGearTeeth))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_CrankTorque))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_CrankGearTeeth))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_WheelTorque))->EndInit();
      this->tabPage_PowerPaging->ResumeLayout(false);
      this->tabPage_PowerPaging->PerformLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Ppg_SlopeConstant))->EndInit();
      this->groupBox_Ppg_SendPageSelect->ResumeLayout(false);
      this->groupBox_Ppg_SendPageSelect->PerformLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Ppg_BasicPowerInterlvTime))->EndInit();
      this->groupBox_Ppg_RaiseEventsSelect->ResumeLayout(false);
      this->groupBox_Ppg_RaiseEventsSelect->PerformLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Ppg_TimerEventFreq))->EndInit();
      this->tabPage_Calibration->ResumeLayout(false);
      this->tabPage_Calibration->PerformLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Cal_RawTorque))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Cal_OffsetTorque))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Cal_CalNumber))->EndInit();
      this->tabPage_GlobalData->ResumeLayout(false);
      this->tabPage_GlobalData->PerformLayout();
      this->tabPage_GlobalBattery->ResumeLayout(false);
      this->tabPage_GlobalBattery->PerformLayout();
      this->groupBox_Resol->ResumeLayout(false);
      this->groupBox_Resol->PerformLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Bat_VoltFrac))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Bat_VoltInt))->EndInit();
      this->tabPage_GetSet->ResumeLayout(false);
      this->tabPage_GetSet->PerformLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_CrankLength))->EndInit();
      this->groupBox3->ResumeLayout(false);
      this->groupBox3->PerformLayout();
      this->groupBox2->ResumeLayout(false);
      this->groupBox2->PerformLayout();
      this->groupBox1->ResumeLayout(false);
      this->groupBox1->PerformLayout();
      this->tabControl_TEPS->ResumeLayout(false);
      this->tabControl_TEPS->PerformLayout();
      this->groupBox5->ResumeLayout(false);
      this->groupBox5->PerformLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_RightPedalSmoothness))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_LeftPedalSmoothness))->EndInit();
      this->groupBox4->ResumeLayout(false);
      this->groupBox4->PerformLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_RightTorqueEffectiveness))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_LeftTorqueEffectiveness))->EndInit();
      this->panel_Display->ResumeLayout(false);
      this->panel_Display->PerformLayout();
      this->ResumeLayout(false);

   }
#pragma endregion


};