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


public ref class SDMDisplay : public System::Windows::Forms::Form, public ISimBase{

public:
	SDMDisplay(dRequestAckMsg^ channelAckMsg){
		requestAckMsg = channelAckMsg;
		commonPages = gcnew CommonData();
		InitializeComponent();
		InitializeSim();
	}

	~SDMDisplay(){
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
	virtual void ANT_eventNotification(UCHAR ucEventCode_, UCHAR* pucEventBuffer_);
	virtual UCHAR getDeviceType(){return SDM_DEVICE_TYPE;}
	virtual UCHAR getTransmissionType(){return 0;}  // Set transmission type to 0 for future compatibility
	virtual USHORT getTransmitPeriod(){return SDM_MSG_PERIOD;}
	virtual DOUBLE getTimerInterval(){return 3600000;} // Set interval to one hour, so timer events are not frequent (timer should be disabled tho)
	virtual void onTimerTock(USHORT eventTime){} // Do nothing
	virtual System::Windows::Forms::Panel^ getSimSettingsPanel(){return this->panel_Settings;}
	virtual System::Windows::Forms::Panel^ getSimTranslatedDisplay(){return this->panel_Display;}

private:
	void InitializeSim();
	void HandleReceive(UCHAR* pucRxBuffer_);
	void UpdateDisplay(UCHAR ucPageNum_);
	void SendRequestMsg(UCHAR ucMsgCode_);
	BOOL HandleRetransmit();
	void UpdateDisplayAckStatus(UCHAR ucStatus_);
	void EncodeRequestMsg(UCHAR ucPageID_, UCHAR* pucTxBuffer_);
	
	System::Void button_RequestPage16_Click(System::Object^  sender, System::EventArgs^  e);
	System::Void button_RequestPage22_Click(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_RqTxTimes_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_RqAckReply_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_RqTxUntilAck_CheckedChanged(System::Object^  sender, System::EventArgs^  e);

private:	// Implementation specific constants
	static const UCHAR ACK_SUCCESS = 0;
	static const UCHAR ACK_RETRY = 1;
	static const UCHAR ACK_FAIL = 2;
	static const UCHAR MAX_RETRIES = 40;	// Maximum number of retransmissions for each message

private:
	dRequestAckMsg^ requestAckMsg;		// Delegate for handling Ack data
	CommonData^ commonPages;			// Handle to common data pages

	// Simulator data
	UCHAR ucLatency32;		// Latency (1/32 seconds)
	USHORT usSpeed256;		// Instantaneous speed (1/256 m/s)
	USHORT usCadence16;		// Instantaneous cadence (1/16 m/s)
	SDMStatus sStatusFlags;	// SDM status flags (individual fields)
	UCHAR ucPreviousStrideCount;	// Previous stride count (strides)
	UCHAR ucStrideCount;	// Cumulative stride count (strides)
	USHORT usPreviousTime200;	// Previous time (1/256 seconds)
	USHORT usTime200;		// Time of last speed/distance computation (1/200 seconds)
	USHORT usPreviousDistance16;	// Previous distance (1/16 seconds)
	USHORT usDistance16;	// Distance (1/16 m)	
	UCHAR ucCapabFlags;		// Capabilities (Page 22) status flags (encoded)
	UCHAR ucPreviousCalCount;	// Previous calorie count (kcal)
	UCHAR ucCalCount;		// Cumulative calorie count (kcal)
	ULONG ulStridesAccum;	// Total accumulated strides, one increment for 2 steps, rollover at 16777216
	ULONG ulDistanceAccum;	// Total accumulated distance (1/256 m)

	// Calculated data
	ULONG ulAcumStrideCount; // Cumulative stride count
	ULONG ulAcumDistance16;	 // Cumulative distance (1/16 m)
	ULONG ulAcumTime200;	 // Cumulative time (1/256 seconds)
	ULONG ulAcumCalCount;	 // Cumulative calories count

	// Tx Request Messages
	UCHAR ucMsgExpectingAck;	// Message pending to be acknowledged
	UCHAR ucAckRetryCount;		// Number of retries for an acknowledged message
	UCHAR ucRqTxTimes;			// Number of times for the sensor to send response
	BOOL bRqAckReply;			// Flag for whether the sensor should use ACK messagess to reply
	BOOL bRqTxUntilAck;			// Flag for whether the sensor should repeat ACK messages until it gets a response

	BOOL bInitialized;		// Flag to only set previous values at the beginning
	BOOL bCalInitialized;	// Flag to only set previous values at the beginning


private: System::Windows::Forms::Panel^  panel_Settings;
private: System::Windows::Forms::TabControl^  tabControl_Settings;
private: System::Windows::Forms::Panel^  panel_Display;
private: System::Windows::Forms::Label^  label_TransStrideCount;
private: System::Windows::Forms::Label^  label_TransInstSpeed;
private: System::Windows::Forms::Label^  label_TransInstSpeedDisplay;
private: System::Windows::Forms::Label^  label_TransStrideCountDisplay;
private: System::Windows::Forms::Label^  label_TransDisplayTitle;
private: System::Windows::Forms::TabPage^  tabPage_Page1Settings;
private: System::Windows::Forms::TabPage^  tabPage_Page2Settings;
private: System::Windows::Forms::Label^  label_P1StrideCount;
private: System::Windows::Forms::Label^  label_P1TotalDistance;
private: System::Windows::Forms::Label^  label_P1StrideCountDisplay;
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
private: System::Windows::Forms::Label^  label_TransLatencyDisplay;
private: System::Windows::Forms::Label^  label_TransLatency;
private: System::Windows::Forms::Label^  label_TransDistDisplay;
private: System::Windows::Forms::Label^  label_TransDist;
private: System::Windows::Forms::Label^  label_TransCadenceDisplay;
private: System::Windows::Forms::Label^  label_TransCadence;
private: System::Windows::Forms::GroupBox^  groupBox2;
private: System::Windows::Forms::Label^  label_Glb_ModelNum;
private: System::Windows::Forms::Label^  label_Glb_SoftwareVer;
private: System::Windows::Forms::Label^  label_Glb_ModelNumDisplay;
private: System::Windows::Forms::Label^  label_Glb_ManfIDDisplay;
private: System::Windows::Forms::Label^  label_Glb_SerialNumDisplay;
private: System::Windows::Forms::Label^  label_Glb_HardwareVerDisplay;
private: System::Windows::Forms::Label^  label_Glb_HardwareVer;
private: System::Windows::Forms::Label^  label_Glb_SerialNum;
private: System::Windows::Forms::Label^  label_Glb_SoftwareVerDisplay;
private: System::Windows::Forms::Label^  label_Glb_ManfID;
private: System::Windows::Forms::Label^  label_Trn_EventTimeDisplay;
private: System::Windows::Forms::Label^  label_Trn_EventTime;
private: System::Windows::Forms::Label^  label_Calc_TimeDisplay;
private: System::Windows::Forms::Label^  label_Calc_Time;
private: System::Windows::Forms::Label^  label_Calc_Pace;
private: System::Windows::Forms::Label^  label_Calc_CadenceDisplay;
private: System::Windows::Forms::Label^  label_Calc_Cadence;
private: System::Windows::Forms::Label^  label_Calc_SpeedDisplay;
private: System::Windows::Forms::Label^  label_Calc_Speed;
private: System::Windows::Forms::Label^  label_Calc_PaceDisplay;
private: System::Windows::Forms::Label^  label_Calc_AvgSpdDisplay;
private: System::Windows::Forms::Label^  label_Calc_AvgSpd;
private: System::Windows::Forms::GroupBox^  groupBox1;
private: System::Windows::Forms::GroupBox^  groupBox3;
private: System::Windows::Forms::GroupBox^  groupBox4;
private: System::Windows::Forms::TabPage^  tabPage_RequestPages;
private: System::Windows::Forms::Label^  label_AckMsgStatus;
private: System::Windows::Forms::Button^  button_RequestPage22;
private: System::Windows::Forms::CheckBox^  checkBox_RqTxUntilAck;
private: System::Windows::Forms::CheckBox^  checkBox_RqAckReply;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_RqTxTimes;
private: System::Windows::Forms::Label^  label_RqTxTimes;
private: System::Windows::Forms::GroupBox^  groupBox_RqSensorReply;
private: System::Windows::Forms::GroupBox^  groupBox_RqAccumulated;
private: System::Windows::Forms::GroupBox^  groupBox_RqCapabilities;
private: System::Windows::Forms::Label^  label_RqTime;
private: System::Windows::Forms::Label^  label_RqCalories;
private: System::Windows::Forms::Label^  label_RqLatency;
private: System::Windows::Forms::Label^  label_RqDistance;
private: System::Windows::Forms::Label^  label_RqSpeed;
private: System::Windows::Forms::Label^  label_RqCadence;
private: System::Windows::Forms::Label^  label_RqStridesDisplay;
private: System::Windows::Forms::Label^  label_RqDistanceDisplay;
private: System::Windows::Forms::Label^  label_RqAccumStrides;
private: System::Windows::Forms::Label^  label_RqAccumDistance;
private: System::Windows::Forms::Button^  button_RequestPage16;
private: System::Windows::Forms::GroupBox^  groupBox_P2Calories;
private: System::Windows::Forms::Label^  label_P2kcal;
private: System::Windows::Forms::Label^  label_P2Calories;
private: System::Windows::Forms::Label^  label_TransCal;
private: System::Windows::Forms::Label^  label_TransCalDisplay;

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
		this->tabPage_Page1Settings = (gcnew System::Windows::Forms::TabPage());
		this->groupBox4 = (gcnew System::Windows::Forms::GroupBox());
		this->label_Calc_Cadence = (gcnew System::Windows::Forms::Label());
		this->label_Calc_CadenceDisplay = (gcnew System::Windows::Forms::Label());
		this->label_P1StrideCount = (gcnew System::Windows::Forms::Label());
		this->label_P1StrideCountDisplay = (gcnew System::Windows::Forms::Label());
		this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
		this->label_Calc_PaceDisplay = (gcnew System::Windows::Forms::Label());
		this->label_Calc_AvgSpdDisplay = (gcnew System::Windows::Forms::Label());
		this->label_Calc_AvgSpd = (gcnew System::Windows::Forms::Label());
		this->label_Calc_Speed = (gcnew System::Windows::Forms::Label());
		this->label_Calc_SpeedDisplay = (gcnew System::Windows::Forms::Label());
		this->label_Calc_Pace = (gcnew System::Windows::Forms::Label());
		this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
		this->label_P1TotalDistance = (gcnew System::Windows::Forms::Label());
		this->label_P1TotDistDisplay = (gcnew System::Windows::Forms::Label());
		this->label_Calc_Time = (gcnew System::Windows::Forms::Label());
		this->label_Calc_TimeDisplay = (gcnew System::Windows::Forms::Label());
		this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
		this->label_Glb_ModelNumDisplay = (gcnew System::Windows::Forms::Label());
		this->label_Glb_ManfIDDisplay = (gcnew System::Windows::Forms::Label());
		this->label_Glb_SerialNumDisplay = (gcnew System::Windows::Forms::Label());
		this->label_Glb_HardwareVerDisplay = (gcnew System::Windows::Forms::Label());
		this->label_Glb_SoftwareVerDisplay = (gcnew System::Windows::Forms::Label());
		this->label_Glb_ModelNum = (gcnew System::Windows::Forms::Label());
		this->label_Glb_SoftwareVer = (gcnew System::Windows::Forms::Label());
		this->label_Glb_HardwareVer = (gcnew System::Windows::Forms::Label());
		this->label_Glb_SerialNum = (gcnew System::Windows::Forms::Label());
		this->label_Glb_ManfID = (gcnew System::Windows::Forms::Label());
		this->tabPage_Page2Settings = (gcnew System::Windows::Forms::TabPage());
		this->groupBox_P2Calories = (gcnew System::Windows::Forms::GroupBox());
		this->label_P2kcal = (gcnew System::Windows::Forms::Label());
		this->label_P2Calories = (gcnew System::Windows::Forms::Label());
		this->label_P2StatusFlags = (gcnew System::Windows::Forms::Label());
		this->label_P2Use = (gcnew System::Windows::Forms::Label());
		this->label_P2Health = (gcnew System::Windows::Forms::Label());
		this->label_P2Battery = (gcnew System::Windows::Forms::Label());
		this->label_P2Location = (gcnew System::Windows::Forms::Label());
		this->listBox_P2Use = (gcnew System::Windows::Forms::ListBox());
		this->listBox_P2Health = (gcnew System::Windows::Forms::ListBox());
		this->listBox_P2Battery = (gcnew System::Windows::Forms::ListBox());
		this->listBox_P2Location = (gcnew System::Windows::Forms::ListBox());
		this->tabPage_RequestPages = (gcnew System::Windows::Forms::TabPage());
		this->button_RequestPage16 = (gcnew System::Windows::Forms::Button());
		this->groupBox_RqAccumulated = (gcnew System::Windows::Forms::GroupBox());
		this->label_RqAccumDistance = (gcnew System::Windows::Forms::Label());
		this->label_RqAccumStrides = (gcnew System::Windows::Forms::Label());
		this->label_RqStridesDisplay = (gcnew System::Windows::Forms::Label());
		this->label_RqDistanceDisplay = (gcnew System::Windows::Forms::Label());
		this->groupBox_RqCapabilities = (gcnew System::Windows::Forms::GroupBox());
		this->label_RqTime = (gcnew System::Windows::Forms::Label());
		this->label_RqCalories = (gcnew System::Windows::Forms::Label());
		this->label_RqLatency = (gcnew System::Windows::Forms::Label());
		this->label_RqDistance = (gcnew System::Windows::Forms::Label());
		this->label_RqSpeed = (gcnew System::Windows::Forms::Label());
		this->label_RqCadence = (gcnew System::Windows::Forms::Label());
		this->groupBox_RqSensorReply = (gcnew System::Windows::Forms::GroupBox());
		this->label_RqTxTimes = (gcnew System::Windows::Forms::Label());
		this->checkBox_RqTxUntilAck = (gcnew System::Windows::Forms::CheckBox());
		this->numericUpDown_RqTxTimes = (gcnew System::Windows::Forms::NumericUpDown());
		this->checkBox_RqAckReply = (gcnew System::Windows::Forms::CheckBox());
		this->button_RequestPage22 = (gcnew System::Windows::Forms::Button());
		this->label_AckMsgStatus = (gcnew System::Windows::Forms::Label());
		this->panel_Display = (gcnew System::Windows::Forms::Panel());
		this->label_TransCal = (gcnew System::Windows::Forms::Label());
		this->label_TransCalDisplay = (gcnew System::Windows::Forms::Label());
		this->label_TransDistDisplay = (gcnew System::Windows::Forms::Label());
		this->label_TransDist = (gcnew System::Windows::Forms::Label());
		this->label_TransInstSpeedDisplay = (gcnew System::Windows::Forms::Label());
		this->label_TransInstSpeed = (gcnew System::Windows::Forms::Label());
		this->label_TransCadenceDisplay = (gcnew System::Windows::Forms::Label());
		this->label_TransCadence = (gcnew System::Windows::Forms::Label());
		this->label_Trn_EventTimeDisplay = (gcnew System::Windows::Forms::Label());
		this->label_Trn_EventTime = (gcnew System::Windows::Forms::Label());
		this->label_TransDisplayTitle = (gcnew System::Windows::Forms::Label());
		this->label_TransLatency = (gcnew System::Windows::Forms::Label());
		this->label_TransLatencyDisplay = (gcnew System::Windows::Forms::Label());
		this->label_TransStrideCountDisplay = (gcnew System::Windows::Forms::Label());
		this->label_TransStrideCount = (gcnew System::Windows::Forms::Label());
		this->panel_Settings->SuspendLayout();
		this->tabControl_Settings->SuspendLayout();
		this->tabPage_Page1Settings->SuspendLayout();
		this->groupBox4->SuspendLayout();
		this->groupBox3->SuspendLayout();
		this->groupBox1->SuspendLayout();
		this->groupBox2->SuspendLayout();
		this->tabPage_Page2Settings->SuspendLayout();
		this->groupBox_P2Calories->SuspendLayout();
		this->tabPage_RequestPages->SuspendLayout();
		this->groupBox_RqAccumulated->SuspendLayout();
		this->groupBox_RqCapabilities->SuspendLayout();
		this->groupBox_RqSensorReply->SuspendLayout();
		(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_RqTxTimes))->BeginInit();
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
		this->tabControl_Settings->Controls->Add(this->tabPage_Page1Settings);
		this->tabControl_Settings->Controls->Add(this->tabPage_Page2Settings);
		this->tabControl_Settings->Controls->Add(this->tabPage_RequestPages);
		this->tabControl_Settings->Location = System::Drawing::Point(0, 3);
		this->tabControl_Settings->Name = L"tabControl_Settings";
		this->tabControl_Settings->SelectedIndex = 0;
		this->tabControl_Settings->Size = System::Drawing::Size(397, 137);
		this->tabControl_Settings->TabIndex = 10;
		// 
		// tabPage_Page1Settings
		// 
		this->tabPage_Page1Settings->Controls->Add(this->groupBox4);
		this->tabPage_Page1Settings->Controls->Add(this->groupBox3);
		this->tabPage_Page1Settings->Controls->Add(this->groupBox1);
		this->tabPage_Page1Settings->Controls->Add(this->groupBox2);
		this->tabPage_Page1Settings->Location = System::Drawing::Point(4, 22);
		this->tabPage_Page1Settings->Name = L"tabPage_Page1Settings";
		this->tabPage_Page1Settings->Size = System::Drawing::Size(389, 111);
		this->tabPage_Page1Settings->TabIndex = 2;
		this->tabPage_Page1Settings->Text = L"Data";
		this->tabPage_Page1Settings->UseVisualStyleBackColor = true;
		// 
		// groupBox4
		// 
		this->groupBox4->Controls->Add(this->label_Calc_Cadence);
		this->groupBox4->Controls->Add(this->label_Calc_CadenceDisplay);
		this->groupBox4->Controls->Add(this->label_P1StrideCount);
		this->groupBox4->Controls->Add(this->label_P1StrideCountDisplay);
		this->groupBox4->Location = System::Drawing::Point(3, 33);
		this->groupBox4->Name = L"groupBox4";
		this->groupBox4->Size = System::Drawing::Size(231, 31);
		this->groupBox4->TabIndex = 2;
		this->groupBox4->TabStop = false;
		this->groupBox4->Text = L"Cadence";
		// 
		// label_Calc_Cadence
		// 
		this->label_Calc_Cadence->AutoSize = true;
		this->label_Calc_Cadence->Location = System::Drawing::Point(6, 12);
		this->label_Calc_Cadence->Name = L"label_Calc_Cadence";
		this->label_Calc_Cadence->Size = System::Drawing::Size(63, 13);
		this->label_Calc_Cadence->TabIndex = 31;
		this->label_Calc_Cadence->Text = L"Strides/min:";
		// 
		// label_Calc_CadenceDisplay
		// 
		this->label_Calc_CadenceDisplay->AutoSize = true;
		this->label_Calc_CadenceDisplay->Location = System::Drawing::Point(69, 12);
		this->label_Calc_CadenceDisplay->Name = L"label_Calc_CadenceDisplay";
		this->label_Calc_CadenceDisplay->Size = System::Drawing::Size(16, 13);
		this->label_Calc_CadenceDisplay->TabIndex = 32;
		this->label_Calc_CadenceDisplay->Text = L"---";
		// 
		// label_P1StrideCount
		// 
		this->label_P1StrideCount->AutoSize = true;
		this->label_P1StrideCount->Location = System::Drawing::Point(123, 12);
		this->label_P1StrideCount->Name = L"label_P1StrideCount";
		this->label_P1StrideCount->Size = System::Drawing::Size(42, 13);
		this->label_P1StrideCount->TabIndex = 22;
		this->label_P1StrideCount->Text = L"Strides:";
		// 
		// label_P1StrideCountDisplay
		// 
		this->label_P1StrideCountDisplay->AutoSize = true;
		this->label_P1StrideCountDisplay->Location = System::Drawing::Point(165, 12);
		this->label_P1StrideCountDisplay->Name = L"label_P1StrideCountDisplay";
		this->label_P1StrideCountDisplay->Size = System::Drawing::Size(16, 13);
		this->label_P1StrideCountDisplay->TabIndex = 26;
		this->label_P1StrideCountDisplay->Text = L"---";
		// 
		// groupBox3
		// 
		this->groupBox3->Controls->Add(this->label_Calc_PaceDisplay);
		this->groupBox3->Controls->Add(this->label_Calc_AvgSpdDisplay);
		this->groupBox3->Controls->Add(this->label_Calc_AvgSpd);
		this->groupBox3->Controls->Add(this->label_Calc_Speed);
		this->groupBox3->Controls->Add(this->label_Calc_SpeedDisplay);
		this->groupBox3->Controls->Add(this->label_Calc_Pace);
		this->groupBox3->Location = System::Drawing::Point(3, 64);
		this->groupBox3->Name = L"groupBox3";
		this->groupBox3->Size = System::Drawing::Size(230, 48);
		this->groupBox3->TabIndex = 2;
		this->groupBox3->TabStop = false;
		this->groupBox3->Text = L"Speed";
		// 
		// label_Calc_PaceDisplay
		// 
		this->label_Calc_PaceDisplay->AutoSize = true;
		this->label_Calc_PaceDisplay->Location = System::Drawing::Point(190, 14);
		this->label_Calc_PaceDisplay->Name = L"label_Calc_PaceDisplay";
		this->label_Calc_PaceDisplay->Size = System::Drawing::Size(16, 13);
		this->label_Calc_PaceDisplay->TabIndex = 34;
		this->label_Calc_PaceDisplay->Text = L"---";
		// 
		// label_Calc_AvgSpdDisplay
		// 
		this->label_Calc_AvgSpdDisplay->AutoSize = true;
		this->label_Calc_AvgSpdDisplay->Location = System::Drawing::Point(120, 28);
		this->label_Calc_AvgSpdDisplay->Name = L"label_Calc_AvgSpdDisplay";
		this->label_Calc_AvgSpdDisplay->Size = System::Drawing::Size(16, 13);
		this->label_Calc_AvgSpdDisplay->TabIndex = 36;
		this->label_Calc_AvgSpdDisplay->Text = L"---";
		// 
		// label_Calc_AvgSpd
		// 
		this->label_Calc_AvgSpd->AutoSize = true;
		this->label_Calc_AvgSpd->Location = System::Drawing::Point(7, 28);
		this->label_Calc_AvgSpd->Name = L"label_Calc_AvgSpd";
		this->label_Calc_AvgSpd->Size = System::Drawing::Size(111, 13);
		this->label_Calc_AvgSpd->TabIndex = 35;
		this->label_Calc_AvgSpd->Text = L"Average Speed (m/s):";
		// 
		// label_Calc_Speed
		// 
		this->label_Calc_Speed->AutoSize = true;
		this->label_Calc_Speed->Location = System::Drawing::Point(7, 14);
		this->label_Calc_Speed->Name = L"label_Calc_Speed";
		this->label_Calc_Speed->Size = System::Drawing::Size(68, 13);
		this->label_Calc_Speed->TabIndex = 29;
		this->label_Calc_Speed->Text = L"Speed (m/s):";
		// 
		// label_Calc_SpeedDisplay
		// 
		this->label_Calc_SpeedDisplay->AutoSize = true;
		this->label_Calc_SpeedDisplay->Location = System::Drawing::Point(76, 14);
		this->label_Calc_SpeedDisplay->Name = L"label_Calc_SpeedDisplay";
		this->label_Calc_SpeedDisplay->Size = System::Drawing::Size(16, 13);
		this->label_Calc_SpeedDisplay->TabIndex = 30;
		this->label_Calc_SpeedDisplay->Text = L"---";
		// 
		// label_Calc_Pace
		// 
		this->label_Calc_Pace->AutoSize = true;
		this->label_Calc_Pace->Location = System::Drawing::Point(114, 14);
		this->label_Calc_Pace->Name = L"label_Calc_Pace";
		this->label_Calc_Pace->Size = System::Drawing::Size(79, 13);
		this->label_Calc_Pace->TabIndex = 33;
		this->label_Calc_Pace->Text = L"Pace (min/km):";
		// 
		// groupBox1
		// 
		this->groupBox1->Controls->Add(this->label_P1TotalDistance);
		this->groupBox1->Controls->Add(this->label_P1TotDistDisplay);
		this->groupBox1->Controls->Add(this->label_Calc_Time);
		this->groupBox1->Controls->Add(this->label_Calc_TimeDisplay);
		this->groupBox1->Location = System::Drawing::Point(3, 2);
		this->groupBox1->Name = L"groupBox1";
		this->groupBox1->Size = System::Drawing::Size(231, 31);
		this->groupBox1->TabIndex = 2;
		this->groupBox1->TabStop = false;
		this->groupBox1->Text = L"Totals";
		// 
		// label_P1TotalDistance
		// 
		this->label_P1TotalDistance->AutoSize = true;
		this->label_P1TotalDistance->Location = System::Drawing::Point(6, 11);
		this->label_P1TotalDistance->Name = L"label_P1TotalDistance";
		this->label_P1TotalDistance->Size = System::Drawing::Size(69, 13);
		this->label_P1TotalDistance->TabIndex = 21;
		this->label_P1TotalDistance->Text = L"Distance (m):";
		// 
		// label_P1TotDistDisplay
		// 
		this->label_P1TotDistDisplay->AutoSize = true;
		this->label_P1TotDistDisplay->Location = System::Drawing::Point(74, 11);
		this->label_P1TotDistDisplay->Name = L"label_P1TotDistDisplay";
		this->label_P1TotDistDisplay->Size = System::Drawing::Size(16, 13);
		this->label_P1TotDistDisplay->TabIndex = 24;
		this->label_P1TotDistDisplay->Text = L"---";
		// 
		// label_Calc_Time
		// 
		this->label_Calc_Time->AutoSize = true;
		this->label_Calc_Time->Location = System::Drawing::Point(123, 11);
		this->label_Calc_Time->Name = L"label_Calc_Time";
		this->label_Calc_Time->Size = System::Drawing::Size(47, 13);
		this->label_Calc_Time->TabIndex = 27;
		this->label_Calc_Time->Text = L"Time (s):";
		// 
		// label_Calc_TimeDisplay
		// 
		this->label_Calc_TimeDisplay->AutoSize = true;
		this->label_Calc_TimeDisplay->Location = System::Drawing::Point(169, 11);
		this->label_Calc_TimeDisplay->Name = L"label_Calc_TimeDisplay";
		this->label_Calc_TimeDisplay->Size = System::Drawing::Size(16, 13);
		this->label_Calc_TimeDisplay->TabIndex = 28;
		this->label_Calc_TimeDisplay->Text = L"---";
		// 
		// groupBox2
		// 
		this->groupBox2->Controls->Add(this->label_Glb_ModelNumDisplay);
		this->groupBox2->Controls->Add(this->label_Glb_ManfIDDisplay);
		this->groupBox2->Controls->Add(this->label_Glb_SerialNumDisplay);
		this->groupBox2->Controls->Add(this->label_Glb_HardwareVerDisplay);
		this->groupBox2->Controls->Add(this->label_Glb_SoftwareVerDisplay);
		this->groupBox2->Controls->Add(this->label_Glb_ModelNum);
		this->groupBox2->Controls->Add(this->label_Glb_SoftwareVer);
		this->groupBox2->Controls->Add(this->label_Glb_HardwareVer);
		this->groupBox2->Controls->Add(this->label_Glb_SerialNum);
		this->groupBox2->Controls->Add(this->label_Glb_ManfID);
		this->groupBox2->Location = System::Drawing::Point(240, 2);
		this->groupBox2->Name = L"groupBox2";
		this->groupBox2->Size = System::Drawing::Size(146, 109);
		this->groupBox2->TabIndex = 19;
		this->groupBox2->TabStop = false;
		this->groupBox2->Text = L"Global Data";
		// 
		// label_Glb_ModelNumDisplay
		// 
		this->label_Glb_ModelNumDisplay->AutoSize = true;
		this->label_Glb_ModelNumDisplay->Location = System::Drawing::Point(76, 51);
		this->label_Glb_ModelNumDisplay->Name = L"label_Glb_ModelNumDisplay";
		this->label_Glb_ModelNumDisplay->Size = System::Drawing::Size(16, 13);
		this->label_Glb_ModelNumDisplay->TabIndex = 9;
		this->label_Glb_ModelNumDisplay->Text = L"---";
		// 
		// label_Glb_ManfIDDisplay
		// 
		this->label_Glb_ManfIDDisplay->AutoSize = true;
		this->label_Glb_ManfIDDisplay->Location = System::Drawing::Point(76, 34);
		this->label_Glb_ManfIDDisplay->Name = L"label_Glb_ManfIDDisplay";
		this->label_Glb_ManfIDDisplay->Size = System::Drawing::Size(16, 13);
		this->label_Glb_ManfIDDisplay->TabIndex = 8;
		this->label_Glb_ManfIDDisplay->Text = L"---";
		// 
		// label_Glb_SerialNumDisplay
		// 
		this->label_Glb_SerialNumDisplay->AutoSize = true;
		this->label_Glb_SerialNumDisplay->Location = System::Drawing::Point(76, 15);
		this->label_Glb_SerialNumDisplay->Name = L"label_Glb_SerialNumDisplay";
		this->label_Glb_SerialNumDisplay->Size = System::Drawing::Size(16, 13);
		this->label_Glb_SerialNumDisplay->TabIndex = 11;
		this->label_Glb_SerialNumDisplay->Text = L"---";
		// 
		// label_Glb_HardwareVerDisplay
		// 
		this->label_Glb_HardwareVerDisplay->AutoSize = true;
		this->label_Glb_HardwareVerDisplay->Location = System::Drawing::Point(76, 69);
		this->label_Glb_HardwareVerDisplay->Name = L"label_Glb_HardwareVerDisplay";
		this->label_Glb_HardwareVerDisplay->Size = System::Drawing::Size(16, 13);
		this->label_Glb_HardwareVerDisplay->TabIndex = 14;
		this->label_Glb_HardwareVerDisplay->Text = L"---";
		// 
		// label_Glb_SoftwareVerDisplay
		// 
		this->label_Glb_SoftwareVerDisplay->AutoSize = true;
		this->label_Glb_SoftwareVerDisplay->Location = System::Drawing::Point(76, 87);
		this->label_Glb_SoftwareVerDisplay->Name = L"label_Glb_SoftwareVerDisplay";
		this->label_Glb_SoftwareVerDisplay->Size = System::Drawing::Size(16, 13);
		this->label_Glb_SoftwareVerDisplay->TabIndex = 16;
		this->label_Glb_SoftwareVerDisplay->Text = L"---";
		// 
		// label_Glb_ModelNum
		// 
		this->label_Glb_ModelNum->AutoSize = true;
		this->label_Glb_ModelNum->Location = System::Drawing::Point(29, 51);
		this->label_Glb_ModelNum->Name = L"label_Glb_ModelNum";
		this->label_Glb_ModelNum->Size = System::Drawing::Size(49, 13);
		this->label_Glb_ModelNum->TabIndex = 5;
		this->label_Glb_ModelNum->Text = L"Model #:";
		// 
		// label_Glb_SoftwareVer
		// 
		this->label_Glb_SoftwareVer->AutoSize = true;
		this->label_Glb_SoftwareVer->Location = System::Drawing::Point(7, 87);
		this->label_Glb_SoftwareVer->Name = L"label_Glb_SoftwareVer";
		this->label_Glb_SoftwareVer->Size = System::Drawing::Size(71, 13);
		this->label_Glb_SoftwareVer->TabIndex = 4;
		this->label_Glb_SoftwareVer->Text = L"Software Ver:";
		// 
		// label_Glb_HardwareVer
		// 
		this->label_Glb_HardwareVer->AutoSize = true;
		this->label_Glb_HardwareVer->Location = System::Drawing::Point(3, 69);
		this->label_Glb_HardwareVer->Name = L"label_Glb_HardwareVer";
		this->label_Glb_HardwareVer->Size = System::Drawing::Size(75, 13);
		this->label_Glb_HardwareVer->TabIndex = 3;
		this->label_Glb_HardwareVer->Text = L"Hardware Ver:";
		// 
		// label_Glb_SerialNum
		// 
		this->label_Glb_SerialNum->AutoSize = true;
		this->label_Glb_SerialNum->Location = System::Drawing::Point(32, 15);
		this->label_Glb_SerialNum->Name = L"label_Glb_SerialNum";
		this->label_Glb_SerialNum->Size = System::Drawing::Size(46, 13);
		this->label_Glb_SerialNum->TabIndex = 2;
		this->label_Glb_SerialNum->Text = L"Serial #:";
		// 
		// label_Glb_ManfID
		// 
		this->label_Glb_ManfID->AutoSize = true;
		this->label_Glb_ManfID->Location = System::Drawing::Point(27, 33);
		this->label_Glb_ManfID->Name = L"label_Glb_ManfID";
		this->label_Glb_ManfID->Size = System::Drawing::Size(51, 13);
		this->label_Glb_ManfID->TabIndex = 1;
		this->label_Glb_ManfID->Text = L"Manf. ID:";
		// 
		// tabPage_Page2Settings
		// 
		this->tabPage_Page2Settings->Controls->Add(this->groupBox_P2Calories);
		this->tabPage_Page2Settings->Controls->Add(this->label_P2StatusFlags);
		this->tabPage_Page2Settings->Controls->Add(this->label_P2Use);
		this->tabPage_Page2Settings->Controls->Add(this->label_P2Health);
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
		this->tabPage_Page2Settings->Text = L"Status";
		this->tabPage_Page2Settings->UseVisualStyleBackColor = true;
		// 
		// groupBox_P2Calories
		// 
		this->groupBox_P2Calories->Controls->Add(this->label_P2kcal);
		this->groupBox_P2Calories->Controls->Add(this->label_P2Calories);
		this->groupBox_P2Calories->Location = System::Drawing::Point(294, 69);
		this->groupBox_P2Calories->Name = L"groupBox_P2Calories";
		this->groupBox_P2Calories->Size = System::Drawing::Size(91, 35);
		this->groupBox_P2Calories->TabIndex = 39;
		this->groupBox_P2Calories->TabStop = false;
		this->groupBox_P2Calories->Text = L"Total Calories";
		// 
		// label_P2kcal
		// 
		this->label_P2kcal->AutoSize = true;
		this->label_P2kcal->Location = System::Drawing::Point(7, 16);
		this->label_P2kcal->Name = L"label_P2kcal";
		this->label_P2kcal->Size = System::Drawing::Size(36, 13);
		this->label_P2kcal->TabIndex = 1;
		this->label_P2kcal->Text = L"(kcal):";
		// 
		// label_P2Calories
		// 
		this->label_P2Calories->AutoSize = true;
		this->label_P2Calories->Location = System::Drawing::Point(49, 16);
		this->label_P2Calories->Name = L"label_P2Calories";
		this->label_P2Calories->Size = System::Drawing::Size(16, 13);
		this->label_P2Calories->TabIndex = 0;
		this->label_P2Calories->Text = L"---";
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
		// label_P2Use
		// 
		this->label_P2Use->AutoSize = true;
		this->label_P2Use->Location = System::Drawing::Point(313, 15);
		this->label_P2Use->Name = L"label_P2Use";
		this->label_P2Use->Size = System::Drawing::Size(54, 13);
		this->label_P2Use->TabIndex = 7;
		this->label_P2Use->Text = L"Use State";
		// 
		// label_P2Health
		// 
		this->label_P2Health->AutoSize = true;
		this->label_P2Health->Location = System::Drawing::Point(209, 15);
		this->label_P2Health->Name = L"label_P2Health";
		this->label_P2Health->Size = System::Drawing::Size(65, 13);
		this->label_P2Health->TabIndex = 6;
		this->label_P2Health->Text = L"SDM Health";
		// 
		// label_P2Battery
		// 
		this->label_P2Battery->AutoSize = true;
		this->label_P2Battery->Location = System::Drawing::Point(107, 23);
		this->label_P2Battery->Name = L"label_P2Battery";
		this->label_P2Battery->Size = System::Drawing::Size(73, 13);
		this->label_P2Battery->TabIndex = 5;
		this->label_P2Battery->Text = L"Battery Status";
		// 
		// label_P2Location
		// 
		this->label_P2Location->AutoSize = true;
		this->label_P2Location->Location = System::Drawing::Point(9, 23);
		this->label_P2Location->Name = L"label_P2Location";
		this->label_P2Location->Size = System::Drawing::Size(75, 13);
		this->label_P2Location->TabIndex = 4;
		this->label_P2Location->Text = L"SDM Location";
		// 
		// listBox_P2Use
		// 
		this->listBox_P2Use->Enabled = false;
		this->listBox_P2Use->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
			static_cast<System::Byte>(0)));
		this->listBox_P2Use->FormattingEnabled = true;
		this->listBox_P2Use->Items->AddRange(gcnew cli::array< System::Object^  >(2) {L"00 - Inactive", L"01 - Active"});
		this->listBox_P2Use->Location = System::Drawing::Point(297, 32);
		this->listBox_P2Use->Name = L"listBox_P2Use";
		this->listBox_P2Use->Size = System::Drawing::Size(88, 30);
		this->listBox_P2Use->TabIndex = 38;
		// 
		// listBox_P2Health
		// 
		this->listBox_P2Health->Enabled = false;
		this->listBox_P2Health->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
			static_cast<System::Byte>(0)));
		this->listBox_P2Health->FormattingEnabled = true;
		this->listBox_P2Health->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"00 - OK", L"01 - Error", L"10 - Warning"});
		this->listBox_P2Health->Location = System::Drawing::Point(200, 32);
		this->listBox_P2Health->Name = L"listBox_P2Health";
		this->listBox_P2Health->Size = System::Drawing::Size(87, 43);
		this->listBox_P2Health->TabIndex = 37;
		// 
		// listBox_P2Battery
		// 
		this->listBox_P2Battery->Enabled = false;
		this->listBox_P2Battery->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
			static_cast<System::Byte>(0)));
		this->listBox_P2Battery->FormattingEnabled = true;
		this->listBox_P2Battery->Items->AddRange(gcnew cli::array< System::Object^  >(4) {L"00 - OK New", L"01 - OK Good", L"10 - OK", 
			L"11 - Low"});
		this->listBox_P2Battery->Location = System::Drawing::Point(99, 40);
		this->listBox_P2Battery->Name = L"listBox_P2Battery";
		this->listBox_P2Battery->Size = System::Drawing::Size(89, 56);
		this->listBox_P2Battery->TabIndex = 36;
		// 
		// listBox_P2Location
		// 
		this->listBox_P2Location->Enabled = false;
		this->listBox_P2Location->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
			static_cast<System::Byte>(0)));
		this->listBox_P2Location->FormattingEnabled = true;
		this->listBox_P2Location->Items->AddRange(gcnew cli::array< System::Object^  >(4) {L"00 - Laces", L"01 - Midsole", L"10 - Chest", 
			L"11 - Other"});
		this->listBox_P2Location->Location = System::Drawing::Point(6, 40);
		this->listBox_P2Location->Name = L"listBox_P2Location";
		this->listBox_P2Location->Size = System::Drawing::Size(82, 56);
		this->listBox_P2Location->TabIndex = 35;
		// 
		// tabPage_RequestPages
		// 
		this->tabPage_RequestPages->Controls->Add(this->button_RequestPage16);
		this->tabPage_RequestPages->Controls->Add(this->groupBox_RqAccumulated);
		this->tabPage_RequestPages->Controls->Add(this->groupBox_RqCapabilities);
		this->tabPage_RequestPages->Controls->Add(this->groupBox_RqSensorReply);
		this->tabPage_RequestPages->Controls->Add(this->button_RequestPage22);
		this->tabPage_RequestPages->Controls->Add(this->label_AckMsgStatus);
		this->tabPage_RequestPages->Location = System::Drawing::Point(4, 22);
		this->tabPage_RequestPages->Name = L"tabPage_RequestPages";
		this->tabPage_RequestPages->Padding = System::Windows::Forms::Padding(3);
		this->tabPage_RequestPages->Size = System::Drawing::Size(389, 111);
		this->tabPage_RequestPages->TabIndex = 4;
		this->tabPage_RequestPages->Text = L"Request";
		this->tabPage_RequestPages->UseVisualStyleBackColor = true;
		// 
		// button_RequestPage16
		// 
		this->button_RequestPage16->Location = System::Drawing::Point(6, 9);
		this->button_RequestPage16->Name = L"button_RequestPage16";
		this->button_RequestPage16->Size = System::Drawing::Size(53, 23);
		this->button_RequestPage16->TabIndex = 20;
		this->button_RequestPage16->Text = L"Pg 16";
		this->button_RequestPage16->UseVisualStyleBackColor = true;
		this->button_RequestPage16->Click += gcnew System::EventHandler(this, &SDMDisplay::button_RequestPage16_Click);
		// 
		// groupBox_RqAccumulated
		// 
		this->groupBox_RqAccumulated->Controls->Add(this->label_RqAccumDistance);
		this->groupBox_RqAccumulated->Controls->Add(this->label_RqAccumStrides);
		this->groupBox_RqAccumulated->Controls->Add(this->label_RqStridesDisplay);
		this->groupBox_RqAccumulated->Controls->Add(this->label_RqDistanceDisplay);
		this->groupBox_RqAccumulated->Location = System::Drawing::Point(248, 9);
		this->groupBox_RqAccumulated->Name = L"groupBox_RqAccumulated";
		this->groupBox_RqAccumulated->Size = System::Drawing::Size(135, 97);
		this->groupBox_RqAccumulated->TabIndex = 19;
		this->groupBox_RqAccumulated->TabStop = false;
		this->groupBox_RqAccumulated->Text = L"P16 - Accumulated";
		// 
		// label_RqAccumDistance
		// 
		this->label_RqAccumDistance->AutoSize = true;
		this->label_RqAccumDistance->Location = System::Drawing::Point(19, 59);
		this->label_RqAccumDistance->Name = L"label_RqAccumDistance";
		this->label_RqAccumDistance->Size = System::Drawing::Size(98, 13);
		this->label_RqAccumDistance->TabIndex = 42;
		this->label_RqAccumDistance->Text = L"Distance (1/256m):";
		// 
		// label_RqAccumStrides
		// 
		this->label_RqAccumStrides->AutoSize = true;
		this->label_RqAccumStrides->Location = System::Drawing::Point(19, 20);
		this->label_RqAccumStrides->Name = L"label_RqAccumStrides";
		this->label_RqAccumStrides->Size = System::Drawing::Size(42, 13);
		this->label_RqAccumStrides->TabIndex = 37;
		this->label_RqAccumStrides->Text = L"Strides:";
		// 
		// label_RqStridesDisplay
		// 
		this->label_RqStridesDisplay->Location = System::Drawing::Point(38, 37);
		this->label_RqStridesDisplay->Name = L"label_RqStridesDisplay";
		this->label_RqStridesDisplay->Size = System::Drawing::Size(70, 13);
		this->label_RqStridesDisplay->TabIndex = 40;
		this->label_RqStridesDisplay->Text = L"0";
		// 
		// label_RqDistanceDisplay
		// 
		this->label_RqDistanceDisplay->Location = System::Drawing::Point(38, 76);
		this->label_RqDistanceDisplay->Name = L"label_RqDistanceDisplay";
		this->label_RqDistanceDisplay->Size = System::Drawing::Size(70, 13);
		this->label_RqDistanceDisplay->TabIndex = 41;
		this->label_RqDistanceDisplay->Text = L"0";
		// 
		// groupBox_RqCapabilities
		// 
		this->groupBox_RqCapabilities->Controls->Add(this->label_RqTime);
		this->groupBox_RqCapabilities->Controls->Add(this->label_RqCalories);
		this->groupBox_RqCapabilities->Controls->Add(this->label_RqLatency);
		this->groupBox_RqCapabilities->Controls->Add(this->label_RqDistance);
		this->groupBox_RqCapabilities->Controls->Add(this->label_RqSpeed);
		this->groupBox_RqCapabilities->Controls->Add(this->label_RqCadence);
		this->groupBox_RqCapabilities->Location = System::Drawing::Point(124, 35);
		this->groupBox_RqCapabilities->Name = L"groupBox_RqCapabilities";
		this->groupBox_RqCapabilities->Size = System::Drawing::Size(118, 71);
		this->groupBox_RqCapabilities->TabIndex = 18;
		this->groupBox_RqCapabilities->TabStop = false;
		this->groupBox_RqCapabilities->Text = L"P22 - Capabilities";
		// 
		// label_RqTime
		// 
		this->label_RqTime->AutoSize = true;
		this->label_RqTime->Location = System::Drawing::Point(8, 17);
		this->label_RqTime->Name = L"label_RqTime";
		this->label_RqTime->Size = System::Drawing::Size(30, 13);
		this->label_RqTime->TabIndex = 20;
		this->label_RqTime->Text = L"Time";
		// 
		// label_RqCalories
		// 
		this->label_RqCalories->AutoSize = true;
		this->label_RqCalories->Location = System::Drawing::Point(58, 51);
		this->label_RqCalories->Name = L"label_RqCalories";
		this->label_RqCalories->Size = System::Drawing::Size(44, 13);
		this->label_RqCalories->TabIndex = 25;
		this->label_RqCalories->Text = L"Calories";
		// 
		// label_RqLatency
		// 
		this->label_RqLatency->AutoSize = true;
		this->label_RqLatency->Location = System::Drawing::Point(58, 17);
		this->label_RqLatency->Name = L"label_RqLatency";
		this->label_RqLatency->Size = System::Drawing::Size(45, 13);
		this->label_RqLatency->TabIndex = 23;
		this->label_RqLatency->Text = L"Latency";
		// 
		// label_RqDistance
		// 
		this->label_RqDistance->AutoSize = true;
		this->label_RqDistance->Location = System::Drawing::Point(8, 34);
		this->label_RqDistance->Name = L"label_RqDistance";
		this->label_RqDistance->Size = System::Drawing::Size(49, 13);
		this->label_RqDistance->TabIndex = 21;
		this->label_RqDistance->Text = L"Distance";
		// 
		// label_RqSpeed
		// 
		this->label_RqSpeed->AutoSize = true;
		this->label_RqSpeed->Location = System::Drawing::Point(8, 51);
		this->label_RqSpeed->Name = L"label_RqSpeed";
		this->label_RqSpeed->Size = System::Drawing::Size(38, 13);
		this->label_RqSpeed->TabIndex = 22;
		this->label_RqSpeed->Text = L"Speed";
		// 
		// label_RqCadence
		// 
		this->label_RqCadence->AutoSize = true;
		this->label_RqCadence->Location = System::Drawing::Point(58, 34);
		this->label_RqCadence->Name = L"label_RqCadence";
		this->label_RqCadence->Size = System::Drawing::Size(50, 13);
		this->label_RqCadence->TabIndex = 24;
		this->label_RqCadence->Text = L"Cadence";
		// 
		// groupBox_RqSensorReply
		// 
		this->groupBox_RqSensorReply->Controls->Add(this->label_RqTxTimes);
		this->groupBox_RqSensorReply->Controls->Add(this->checkBox_RqTxUntilAck);
		this->groupBox_RqSensorReply->Controls->Add(this->numericUpDown_RqTxTimes);
		this->groupBox_RqSensorReply->Controls->Add(this->checkBox_RqAckReply);
		this->groupBox_RqSensorReply->Location = System::Drawing::Point(6, 35);
		this->groupBox_RqSensorReply->Name = L"groupBox_RqSensorReply";
		this->groupBox_RqSensorReply->Size = System::Drawing::Size(112, 71);
		this->groupBox_RqSensorReply->TabIndex = 17;
		this->groupBox_RqSensorReply->TabStop = false;
		this->groupBox_RqSensorReply->Text = L"Sensor Reply";
		// 
		// label_RqTxTimes
		// 
		this->label_RqTxTimes->AutoSize = true;
		this->label_RqTxTimes->Location = System::Drawing::Point(5, 19);
		this->label_RqTxTimes->Name = L"label_RqTxTimes";
		this->label_RqTxTimes->Size = System::Drawing::Size(53, 13);
		this->label_RqTxTimes->TabIndex = 16;
		this->label_RqTxTimes->Text = L"Tx Times:";
		// 
		// checkBox_RqTxUntilAck
		// 
		this->checkBox_RqTxUntilAck->AutoSize = true;
		this->checkBox_RqTxUntilAck->Location = System::Drawing::Point(8, 51);
		this->checkBox_RqTxUntilAck->Name = L"checkBox_RqTxUntilAck";
		this->checkBox_RqTxUntilAck->Size = System::Drawing::Size(82, 17);
		this->checkBox_RqTxUntilAck->TabIndex = 15;
		this->checkBox_RqTxUntilAck->Text = L"Tx until Ack";
		this->checkBox_RqTxUntilAck->UseVisualStyleBackColor = true;
		this->checkBox_RqTxUntilAck->CheckedChanged += gcnew System::EventHandler(this, &SDMDisplay::checkBox_RqTxUntilAck_CheckedChanged);
		// 
		// numericUpDown_RqTxTimes
		// 
		this->numericUpDown_RqTxTimes->Location = System::Drawing::Point(59, 17);
		this->numericUpDown_RqTxTimes->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {127, 0, 0, 0});
		this->numericUpDown_RqTxTimes->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
		this->numericUpDown_RqTxTimes->Name = L"numericUpDown_RqTxTimes";
		this->numericUpDown_RqTxTimes->Size = System::Drawing::Size(47, 20);
		this->numericUpDown_RqTxTimes->TabIndex = 13;
		this->numericUpDown_RqTxTimes->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {4, 0, 0, 0});
		this->numericUpDown_RqTxTimes->ValueChanged += gcnew System::EventHandler(this, &SDMDisplay::numericUpDown_RqTxTimes_ValueChanged);
		// 
		// checkBox_RqAckReply
		// 
		this->checkBox_RqAckReply->AutoSize = true;
		this->checkBox_RqAckReply->Location = System::Drawing::Point(8, 35);
		this->checkBox_RqAckReply->Name = L"checkBox_RqAckReply";
		this->checkBox_RqAckReply->Size = System::Drawing::Size(75, 17);
		this->checkBox_RqAckReply->TabIndex = 14;
		this->checkBox_RqAckReply->Text = L"Ack Reply";
		this->checkBox_RqAckReply->UseVisualStyleBackColor = true;
		this->checkBox_RqAckReply->CheckedChanged += gcnew System::EventHandler(this, &SDMDisplay::checkBox_RqAckReply_CheckedChanged);
		// 
		// button_RequestPage22
		// 
		this->button_RequestPage22->Location = System::Drawing::Point(65, 9);
		this->button_RequestPage22->Name = L"button_RequestPage22";
		this->button_RequestPage22->Size = System::Drawing::Size(53, 23);
		this->button_RequestPage22->TabIndex = 2;
		this->button_RequestPage22->Text = L"Pg 22";
		this->button_RequestPage22->UseVisualStyleBackColor = true;
		this->button_RequestPage22->Click += gcnew System::EventHandler(this, &SDMDisplay::button_RequestPage22_Click);
		// 
		// label_AckMsgStatus
		// 
		this->label_AckMsgStatus->AutoSize = true;
		this->label_AckMsgStatus->Location = System::Drawing::Point(130, 14);
		this->label_AckMsgStatus->Name = L"label_AckMsgStatus";
		this->label_AckMsgStatus->Size = System::Drawing::Size(89, 13);
		this->label_AckMsgStatus->TabIndex = 1;
		this->label_AckMsgStatus->Text = L"Request Status...";
		// 
		// panel_Display
		// 
		this->panel_Display->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
		this->panel_Display->Controls->Add(this->label_TransCal);
		this->panel_Display->Controls->Add(this->label_TransCalDisplay);
		this->panel_Display->Controls->Add(this->label_TransDistDisplay);
		this->panel_Display->Controls->Add(this->label_TransDist);
		this->panel_Display->Controls->Add(this->label_TransInstSpeedDisplay);
		this->panel_Display->Controls->Add(this->label_TransInstSpeed);
		this->panel_Display->Controls->Add(this->label_TransCadenceDisplay);
		this->panel_Display->Controls->Add(this->label_TransCadence);
		this->panel_Display->Controls->Add(this->label_Trn_EventTimeDisplay);
		this->panel_Display->Controls->Add(this->label_Trn_EventTime);
		this->panel_Display->Controls->Add(this->label_TransDisplayTitle);
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
		this->label_TransCal->Location = System::Drawing::Point(-1, 69);
		this->label_TransCal->Name = L"label_TransCal";
		this->label_TransCal->Size = System::Drawing::Size(36, 13);
		this->label_TransCal->TabIndex = 15;
		this->label_TransCal->Text = L"(kcal):";
		// 
		// label_TransCalDisplay
		// 
		this->label_TransCalDisplay->AutoSize = true;
		this->label_TransCalDisplay->BackColor = System::Drawing::Color::Transparent;
		this->label_TransCalDisplay->Location = System::Drawing::Point(41, 69);
		this->label_TransCalDisplay->Name = L"label_TransCalDisplay";
		this->label_TransCalDisplay->Size = System::Drawing::Size(16, 13);
		this->label_TransCalDisplay->TabIndex = 14;
		this->label_TransCalDisplay->Text = L"---";
		// 
		// label_TransDistDisplay
		// 
		this->label_TransDistDisplay->AutoSize = true;
		this->label_TransDistDisplay->BackColor = System::Drawing::Color::Transparent;
		this->label_TransDistDisplay->Location = System::Drawing::Point(143, 68);
		this->label_TransDistDisplay->Name = L"label_TransDistDisplay";
		this->label_TransDistDisplay->Size = System::Drawing::Size(16, 13);
		this->label_TransDistDisplay->TabIndex = 6;
		this->label_TransDistDisplay->Text = L"---";
		// 
		// label_TransDist
		// 
		this->label_TransDist->AutoSize = true;
		this->label_TransDist->BackColor = System::Drawing::Color::Transparent;
		this->label_TransDist->Location = System::Drawing::Point(68, 68);
		this->label_TransDist->Name = L"label_TransDist";
		this->label_TransDist->Size = System::Drawing::Size(69, 13);
		this->label_TransDist->TabIndex = 5;
		this->label_TransDist->Text = L"Distance (m):";
		// 
		// label_TransInstSpeedDisplay
		// 
		this->label_TransInstSpeedDisplay->AutoSize = true;
		this->label_TransInstSpeedDisplay->BackColor = System::Drawing::Color::Transparent;
		this->label_TransInstSpeedDisplay->Location = System::Drawing::Point(143, 56);
		this->label_TransInstSpeedDisplay->Name = L"label_TransInstSpeedDisplay";
		this->label_TransInstSpeedDisplay->Size = System::Drawing::Size(16, 13);
		this->label_TransInstSpeedDisplay->TabIndex = 3;
		this->label_TransInstSpeedDisplay->Text = L"---";
		// 
		// label_TransInstSpeed
		// 
		this->label_TransInstSpeed->AutoSize = true;
		this->label_TransInstSpeed->BackColor = System::Drawing::Color::Transparent;
		this->label_TransInstSpeed->Location = System::Drawing::Point(-1, 56);
		this->label_TransInstSpeed->Name = L"label_TransInstSpeed";
		this->label_TransInstSpeed->Size = System::Drawing::Size(138, 13);
		this->label_TransInstSpeed->TabIndex = 1;
		this->label_TransInstSpeed->Text = L"Instantaneous Speed (m/s):";
		// 
		// label_TransCadenceDisplay
		// 
		this->label_TransCadenceDisplay->AutoSize = true;
		this->label_TransCadenceDisplay->BackColor = System::Drawing::Color::Transparent;
		this->label_TransCadenceDisplay->Location = System::Drawing::Point(143, 43);
		this->label_TransCadenceDisplay->Name = L"label_TransCadenceDisplay";
		this->label_TransCadenceDisplay->Size = System::Drawing::Size(16, 13);
		this->label_TransCadenceDisplay->TabIndex = 10;
		this->label_TransCadenceDisplay->Text = L"---";
		// 
		// label_TransCadence
		// 
		this->label_TransCadence->AutoSize = true;
		this->label_TransCadence->BackColor = System::Drawing::Color::Transparent;
		this->label_TransCadence->Location = System::Drawing::Point(1, 43);
		this->label_TransCadence->Name = L"label_TransCadence";
		this->label_TransCadence->Size = System::Drawing::Size(136, 13);
		this->label_TransCadence->TabIndex = 9;
		this->label_TransCadence->Text = L"Inst. Cadence (strides/min):";
		// 
		// label_Trn_EventTimeDisplay
		// 
		this->label_Trn_EventTimeDisplay->AutoSize = true;
		this->label_Trn_EventTimeDisplay->BackColor = System::Drawing::Color::Transparent;
		this->label_Trn_EventTimeDisplay->Location = System::Drawing::Point(143, 31);
		this->label_Trn_EventTimeDisplay->Name = L"label_Trn_EventTimeDisplay";
		this->label_Trn_EventTimeDisplay->Size = System::Drawing::Size(16, 13);
		this->label_Trn_EventTimeDisplay->TabIndex = 12;
		this->label_Trn_EventTimeDisplay->Text = L"---";
		// 
		// label_Trn_EventTime
		// 
		this->label_Trn_EventTime->AutoSize = true;
		this->label_Trn_EventTime->BackColor = System::Drawing::Color::Transparent;
		this->label_Trn_EventTime->Location = System::Drawing::Point(59, 30);
		this->label_Trn_EventTime->Name = L"label_Trn_EventTime";
		this->label_Trn_EventTime->Size = System::Drawing::Size(78, 13);
		this->label_Trn_EventTime->TabIndex = 11;
		this->label_Trn_EventTime->Text = L"Event Time (s):";
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
		// label_TransLatency
		// 
		this->label_TransLatency->AutoSize = true;
		this->label_TransLatency->BackColor = System::Drawing::Color::Transparent;
		this->label_TransLatency->Location = System::Drawing::Point(3, 17);
		this->label_TransLatency->Name = L"label_TransLatency";
		this->label_TransLatency->Size = System::Drawing::Size(70, 13);
		this->label_TransLatency->TabIndex = 7;
		this->label_TransLatency->Text = L"Latency (ms):";
		// 
		// label_TransLatencyDisplay
		// 
		this->label_TransLatencyDisplay->AutoSize = true;
		this->label_TransLatencyDisplay->BackColor = System::Drawing::Color::Transparent;
		this->label_TransLatencyDisplay->Location = System::Drawing::Point(79, 17);
		this->label_TransLatencyDisplay->Name = L"label_TransLatencyDisplay";
		this->label_TransLatencyDisplay->Size = System::Drawing::Size(16, 13);
		this->label_TransLatencyDisplay->TabIndex = 8;
		this->label_TransLatencyDisplay->Text = L"---";
		// 
		// label_TransStrideCountDisplay
		// 
		this->label_TransStrideCountDisplay->AutoSize = true;
		this->label_TransStrideCountDisplay->BackColor = System::Drawing::Color::Transparent;
		this->label_TransStrideCountDisplay->Location = System::Drawing::Point(175, 17);
		this->label_TransStrideCountDisplay->Name = L"label_TransStrideCountDisplay";
		this->label_TransStrideCountDisplay->Size = System::Drawing::Size(16, 13);
		this->label_TransStrideCountDisplay->TabIndex = 2;
		this->label_TransStrideCountDisplay->Text = L"---";
		// 
		// label_TransStrideCount
		// 
		this->label_TransStrideCount->AutoSize = true;
		this->label_TransStrideCount->BackColor = System::Drawing::Color::Transparent;
		this->label_TransStrideCount->Location = System::Drawing::Point(127, 17);
		this->label_TransStrideCount->Name = L"label_TransStrideCount";
		this->label_TransStrideCount->Size = System::Drawing::Size(42, 13);
		this->label_TransStrideCount->TabIndex = 0;
		this->label_TransStrideCount->Text = L"Strides:";
		// 
		// SDMDisplay
		// 
		this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
		this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
		this->ClientSize = System::Drawing::Size(794, 351);
		this->Controls->Add(this->panel_Display);
		this->Controls->Add(this->panel_Settings);
		this->Name = L"SDMDisplay";
		this->Text = L"SDMDisplay";
		this->panel_Settings->ResumeLayout(false);
		this->tabControl_Settings->ResumeLayout(false);
		this->tabPage_Page1Settings->ResumeLayout(false);
		this->groupBox4->ResumeLayout(false);
		this->groupBox4->PerformLayout();
		this->groupBox3->ResumeLayout(false);
		this->groupBox3->PerformLayout();
		this->groupBox1->ResumeLayout(false);
		this->groupBox1->PerformLayout();
		this->groupBox2->ResumeLayout(false);
		this->groupBox2->PerformLayout();
		this->tabPage_Page2Settings->ResumeLayout(false);
		this->tabPage_Page2Settings->PerformLayout();
		this->groupBox_P2Calories->ResumeLayout(false);
		this->groupBox_P2Calories->PerformLayout();
		this->tabPage_RequestPages->ResumeLayout(false);
		this->tabPage_RequestPages->PerformLayout();
		this->groupBox_RqAccumulated->ResumeLayout(false);
		this->groupBox_RqAccumulated->PerformLayout();
		this->groupBox_RqCapabilities->ResumeLayout(false);
		this->groupBox_RqCapabilities->PerformLayout();
		this->groupBox_RqSensorReply->ResumeLayout(false);
		this->groupBox_RqSensorReply->PerformLayout();
		(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_RqTxTimes))->EndInit();
		this->panel_Display->ResumeLayout(false);
		this->panel_Display->PerformLayout();
		this->ResumeLayout(false);

	}
#pragma endregion

};