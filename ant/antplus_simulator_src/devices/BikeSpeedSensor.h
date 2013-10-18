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
#include "bs.h"
#include "types.h"
#include "antdefines.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

public ref class BikeSpeedSensor : public System::Windows::Forms::Form, public ISimBase{
public:
	BikeSpeedSensor(System::Timers::Timer^ channelTimer){
		InitializeComponent();
		timerHandle = channelTimer;		// Get timer handle
		InitializeSim();
	}

	~BikeSpeedSensor(){
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
	virtual UCHAR getDeviceType(){return BS_DEVICE_TYPE;}
	virtual UCHAR getTransmissionType(){return BS_TX_TYPE;}
	virtual USHORT getTransmitPeriod(){return BS_MSG_PERIOD;}
	virtual DOUBLE getTimerInterval(){return (double) ulTimerInterval;}
	virtual System::Windows::Forms::Panel^ getSimSettingsPanel(){return this->panel_Settings;}
	virtual System::Windows::Forms::Panel^ getSimTranslatedDisplay(){return this->panel_Display;}

private:
	void InitializeSim();
	void HandleTransmit(UCHAR* pucTxBuffer_);
	void UpdateDisplay();
	void ForceUpdate();
	System::Void numericUpDown_Sim_SpdCurOutput_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_Sim_SpdMinMaxOutput_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_Sim_WheelCircumference_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void radioButton_SimTypeChanged (System::Object^  sender, System::EventArgs^  e);
	System::Void button_AdvancedUpdate_Click(System::Object^  sender, System::EventArgs^  e);
	System::Void button_UpdateTime_Click(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_Legacy_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_SendBasicPage_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	

private:
	// Simulator timer
	System::Timers::Timer^ timerHandle;	// Timer handle
	ULONG  ulTimerInterval;				// Timer interval between simulated events

	// Simulator variables
	ULONG ulRunTime;				// Time in ms
	ULONG ulRunTime16000;			// Time in 1/16000s, for conversion between time base of ms to 1/1024s
	UCHAR ucReserved;				// Reserved field
	SHORT usEventCount;				// Bike speed (wheel revolutions) event count
	UCHAR ucWheelCircumference;		// Wheel circumference (in cm)
	ULONG ulMinSpeed;				// Minimum bike speed (meters/h)
	ULONG ulCurSpeed;				// Current bike speed (meters/h)
	ULONG ulMaxSpeed;				// Maximum bike speed (meters/h)
	ULONG ulElapsedTime2;			// Cumulative operating time (elapsed time) in 2 second resolution
	USHORT usTime1024;				// Time of last bike speed event (1/1024 seconds)
	UCHAR ucBackgroundCount;		// To send multiple times the same extended message
	UCHAR ucNextBackgroundPage;		// Next extended page to send

	// Background Data
	UCHAR ucMfgID;			// Manufacturing ID
	UCHAR ucHwVersion;		// Hardware version
	UCHAR ucSwVersion;		// Software version
	UCHAR ucModelNum;		// Model number
	USHORT usSerialNum;		// Serial number

	// Status
	UCHAR ucSimDataType;	// Method to generate simulated data
	BOOL bLegacy;			// Enable simulation of legacy sensors
	BOOL bSweepAscending;	// Sweep through values in an ascending manner
	BOOL bTxMinimum;		// Use minimum data set (do not transmit optional Page 1)


private: System::Windows::Forms::Button^  button_AdvancedUpdate;
private: System::Windows::Forms::Button^  button_UpdateTime;
private: System::Windows::Forms::CheckBox^  checkBox_Legacy;
private: System::Windows::Forms::Label^  label_AdvancedError;
private: System::Windows::Forms::Label^  label_ElapsedSecsDisplay;
private: System::Windows::Forms::Label^  label_ElpTime;
private: System::Windows::Forms::Label^  label_HardwareVer;
private: System::Windows::Forms::Label^  label_ManfID;
private: System::Windows::Forms::Label^  label_ModelNum;
private: System::Windows::Forms::Label^  label_SerialNum;
private: System::Windows::Forms::Label^  label_Sim_SimType;
private: System::Windows::Forms::Label^  label_Sim_SpdCur;
private: System::Windows::Forms::Label^  label_Sim_SpdMax;
private: System::Windows::Forms::Label^  label_Sim_SpdMin;
private: System::Windows::Forms::Label^  label_Sim_Speed;
private: System::Windows::Forms::Label^  label_Sim_WheelCircumferenceConst;
private: System::Windows::Forms::Label^  label_SoftwareVer;
private: System::Windows::Forms::Label^  label_Trn_EventCount;
private: System::Windows::Forms::Label^  label_Trn_EventCountDisplay;


private: System::Windows::Forms::Label^  label_Trn_Time;
private: System::Windows::Forms::Label^  label_Trn_TimeDisplay;
private: System::Windows::Forms::Label^  label_Trn_TranslatedDisplayLabel;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Sim_SpdCurOutput;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Sim_SpdMaxOutput;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Sim_SpdMinOutput;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Sim_WheelCircumference;
private: System::Windows::Forms::Panel^  panel_Display;
private: System::Windows::Forms::Panel^  panel_Settings;
private: System::Windows::Forms::RadioButton^  radioButton_Sim_Fixed;
private: System::Windows::Forms::RadioButton^  radioButton_Sim_Sweep;
private: System::Windows::Forms::TabControl^  tabControl_Settings;
private: System::Windows::Forms::TabPage^  tabPage_GlobalData;
private: System::Windows::Forms::TabPage^  tabPage_Simulation;
private: System::Windows::Forms::TextBox^  textBox_ElpTimeChange;
private: System::Windows::Forms::TextBox^  textBox_HardwareVerChange;
private: System::Windows::Forms::TextBox^  textBox_ManfIDChange;
private: System::Windows::Forms::TextBox^  textBox_ModelNumChange;
private: System::Windows::Forms::TextBox^  textBox_SerialNumChange;
private: System::Windows::Forms::TextBox^  textBox_SoftwareVerChange;
private: System::Windows::Forms::CheckBox^  checkBox_SendBasicPage;


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
		this->checkBox_SendBasicPage = (gcnew System::Windows::Forms::CheckBox());
		this->checkBox_Legacy = (gcnew System::Windows::Forms::CheckBox());
		this->numericUpDown_Sim_WheelCircumference = (gcnew System::Windows::Forms::NumericUpDown());
		this->label_Sim_SpdMin = (gcnew System::Windows::Forms::Label());
		this->label_Sim_SpdCur = (gcnew System::Windows::Forms::Label());
		this->label_Sim_SpdMax = (gcnew System::Windows::Forms::Label());
		this->label_Sim_WheelCircumferenceConst = (gcnew System::Windows::Forms::Label());
		this->numericUpDown_Sim_SpdMaxOutput = (gcnew System::Windows::Forms::NumericUpDown());
		this->label_Sim_SimType = (gcnew System::Windows::Forms::Label());
		this->radioButton_Sim_Fixed = (gcnew System::Windows::Forms::RadioButton());
		this->numericUpDown_Sim_SpdMinOutput = (gcnew System::Windows::Forms::NumericUpDown());
		this->radioButton_Sim_Sweep = (gcnew System::Windows::Forms::RadioButton());
		this->numericUpDown_Sim_SpdCurOutput = (gcnew System::Windows::Forms::NumericUpDown());
		this->label_Sim_Speed = (gcnew System::Windows::Forms::Label());
		this->tabPage_GlobalData = (gcnew System::Windows::Forms::TabPage());
		this->button_UpdateTime = (gcnew System::Windows::Forms::Button());
		this->label_SoftwareVer = (gcnew System::Windows::Forms::Label());
		this->label_HardwareVer = (gcnew System::Windows::Forms::Label());
		this->label_ManfID = (gcnew System::Windows::Forms::Label());
		this->label_ModelNum = (gcnew System::Windows::Forms::Label());
		this->label_AdvancedError = (gcnew System::Windows::Forms::Label());
		this->textBox_ElpTimeChange = (gcnew System::Windows::Forms::TextBox());
		this->textBox_SoftwareVerChange = (gcnew System::Windows::Forms::TextBox());
		this->button_AdvancedUpdate = (gcnew System::Windows::Forms::Button());
		this->textBox_HardwareVerChange = (gcnew System::Windows::Forms::TextBox());
		this->textBox_ModelNumChange = (gcnew System::Windows::Forms::TextBox());
		this->textBox_SerialNumChange = (gcnew System::Windows::Forms::TextBox());
		this->textBox_ManfIDChange = (gcnew System::Windows::Forms::TextBox());
		this->label_ElapsedSecsDisplay = (gcnew System::Windows::Forms::Label());
		this->label_SerialNum = (gcnew System::Windows::Forms::Label());
		this->label_ElpTime = (gcnew System::Windows::Forms::Label());
		this->panel_Display = (gcnew System::Windows::Forms::Panel());
		this->label_Trn_TimeDisplay = (gcnew System::Windows::Forms::Label());
		this->label_Trn_Time = (gcnew System::Windows::Forms::Label());
		this->label_Trn_TranslatedDisplayLabel = (gcnew System::Windows::Forms::Label());
		this->label_Trn_EventCountDisplay = (gcnew System::Windows::Forms::Label());
		this->label_Trn_EventCount = (gcnew System::Windows::Forms::Label());
		this->panel_Settings->SuspendLayout();
		this->tabControl_Settings->SuspendLayout();
		this->tabPage_Simulation->SuspendLayout();
		(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_WheelCircumference))->BeginInit();
		(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_SpdMaxOutput))->BeginInit();
		(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_SpdMinOutput))->BeginInit();
		(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_SpdCurOutput))->BeginInit();
		this->tabPage_GlobalData->SuspendLayout();
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
		this->tabControl_Settings->Controls->Add(this->tabPage_GlobalData);
		this->tabControl_Settings->Location = System::Drawing::Point(0, 3);
		this->tabControl_Settings->Name = L"tabControl_Settings";
		this->tabControl_Settings->SelectedIndex = 0;
		this->tabControl_Settings->Size = System::Drawing::Size(397, 137);
		this->tabControl_Settings->TabIndex = 10;
		// 
		// tabPage_Simulation
		// 
		this->tabPage_Simulation->Controls->Add(this->checkBox_SendBasicPage);
		this->tabPage_Simulation->Controls->Add(this->checkBox_Legacy);
		this->tabPage_Simulation->Controls->Add(this->numericUpDown_Sim_WheelCircumference);
		this->tabPage_Simulation->Controls->Add(this->label_Sim_SpdMin);
		this->tabPage_Simulation->Controls->Add(this->label_Sim_SpdCur);
		this->tabPage_Simulation->Controls->Add(this->label_Sim_SpdMax);
		this->tabPage_Simulation->Controls->Add(this->label_Sim_WheelCircumferenceConst);
		this->tabPage_Simulation->Controls->Add(this->numericUpDown_Sim_SpdMaxOutput);
		this->tabPage_Simulation->Controls->Add(this->label_Sim_SimType);
		this->tabPage_Simulation->Controls->Add(this->radioButton_Sim_Fixed);
		this->tabPage_Simulation->Controls->Add(this->numericUpDown_Sim_SpdMinOutput);
		this->tabPage_Simulation->Controls->Add(this->radioButton_Sim_Sweep);
		this->tabPage_Simulation->Controls->Add(this->numericUpDown_Sim_SpdCurOutput);
		this->tabPage_Simulation->Controls->Add(this->label_Sim_Speed);
		this->tabPage_Simulation->Location = System::Drawing::Point(4, 22);
		this->tabPage_Simulation->Name = L"tabPage_Simulation";
		this->tabPage_Simulation->Padding = System::Windows::Forms::Padding(3);
		this->tabPage_Simulation->Size = System::Drawing::Size(389, 111);
		this->tabPage_Simulation->TabIndex = 0;
		this->tabPage_Simulation->Text = L"Simulation";
		this->tabPage_Simulation->UseVisualStyleBackColor = true;
		// 
		// checkBox_SendBasicPage
		// 
		this->checkBox_SendBasicPage->AutoSize = true;
		this->checkBox_SendBasicPage->Location = System::Drawing::Point(261, 50);
		this->checkBox_SendBasicPage->Name = L"checkBox_SendBasicPage";
		this->checkBox_SendBasicPage->Size = System::Drawing::Size(112, 17);
		this->checkBox_SendBasicPage->TabIndex = 61;
		this->checkBox_SendBasicPage->Text = L"Minimum Data Set";
		this->checkBox_SendBasicPage->UseVisualStyleBackColor = true;
		this->checkBox_SendBasicPage->CheckedChanged += gcnew System::EventHandler(this, &BikeSpeedSensor::checkBox_SendBasicPage_CheckedChanged);
		// 
		// checkBox_Legacy
		// 
		this->checkBox_Legacy->AutoSize = true;
		this->checkBox_Legacy->Location = System::Drawing::Point(261, 27);
		this->checkBox_Legacy->Name = L"checkBox_Legacy";
		this->checkBox_Legacy->Size = System::Drawing::Size(61, 17);
		this->checkBox_Legacy->TabIndex = 60;
		this->checkBox_Legacy->Text = L"Legacy";
		this->checkBox_Legacy->UseVisualStyleBackColor = true;
		this->checkBox_Legacy->CheckedChanged += gcnew System::EventHandler(this, &BikeSpeedSensor::checkBox_Legacy_CheckedChanged);
		// 
		// numericUpDown_Sim_WheelCircumference
		// 
		this->numericUpDown_Sim_WheelCircumference->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {5, 0, 0, 0});
		this->numericUpDown_Sim_WheelCircumference->Location = System::Drawing::Point(297, 78);
		this->numericUpDown_Sim_WheelCircumference->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
		this->numericUpDown_Sim_WheelCircumference->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
		this->numericUpDown_Sim_WheelCircumference->Name = L"numericUpDown_Sim_WheelCircumference";
		this->numericUpDown_Sim_WheelCircumference->Size = System::Drawing::Size(47, 20);
		this->numericUpDown_Sim_WheelCircumference->TabIndex = 17;
		this->numericUpDown_Sim_WheelCircumference->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {207, 0, 0, 0});
		this->numericUpDown_Sim_WheelCircumference->ValueChanged += gcnew System::EventHandler(this, &BikeSpeedSensor::numericUpDown_Sim_WheelCircumference_ValueChanged);
		// 
		// label_Sim_SpdMin
		// 
		this->label_Sim_SpdMin->AutoSize = true;
		this->label_Sim_SpdMin->Location = System::Drawing::Point(36, 33);
		this->label_Sim_SpdMin->Name = L"label_Sim_SpdMin";
		this->label_Sim_SpdMin->Size = System::Drawing::Size(24, 13);
		this->label_Sim_SpdMin->TabIndex = 57;
		this->label_Sim_SpdMin->Text = L"Min";
		// 
		// label_Sim_SpdCur
		// 
		this->label_Sim_SpdCur->AutoSize = true;
		this->label_Sim_SpdCur->Location = System::Drawing::Point(19, 57);
		this->label_Sim_SpdCur->Name = L"label_Sim_SpdCur";
		this->label_Sim_SpdCur->Size = System::Drawing::Size(41, 13);
		this->label_Sim_SpdCur->TabIndex = 58;
		this->label_Sim_SpdCur->Text = L"Current";
		// 
		// label_Sim_SpdMax
		// 
		this->label_Sim_SpdMax->AutoSize = true;
		this->label_Sim_SpdMax->Location = System::Drawing::Point(33, 81);
		this->label_Sim_SpdMax->Name = L"label_Sim_SpdMax";
		this->label_Sim_SpdMax->Size = System::Drawing::Size(27, 13);
		this->label_Sim_SpdMax->TabIndex = 59;
		this->label_Sim_SpdMax->Text = L"Max";
		// 
		// label_Sim_WheelCircumferenceConst
		// 
		this->label_Sim_WheelCircumferenceConst->AutoSize = true;
		this->label_Sim_WheelCircumferenceConst->Location = System::Drawing::Point(162, 80);
		this->label_Sim_WheelCircumferenceConst->Name = L"label_Sim_WheelCircumferenceConst";
		this->label_Sim_WheelCircumferenceConst->Size = System::Drawing::Size(132, 13);
		this->label_Sim_WheelCircumferenceConst->TabIndex = 53;
		this->label_Sim_WheelCircumferenceConst->Text = L"Wheel Circumference (cm)";
		// 
		// numericUpDown_Sim_SpdMaxOutput
		// 
		this->numericUpDown_Sim_SpdMaxOutput->DecimalPlaces = 3;
		this->numericUpDown_Sim_SpdMaxOutput->Enabled = false;
		this->numericUpDown_Sim_SpdMaxOutput->Location = System::Drawing::Point(63, 78);
		this->numericUpDown_Sim_SpdMaxOutput->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
		this->numericUpDown_Sim_SpdMaxOutput->Name = L"numericUpDown_Sim_SpdMaxOutput";
		this->numericUpDown_Sim_SpdMaxOutput->Size = System::Drawing::Size(64, 20);
		this->numericUpDown_Sim_SpdMaxOutput->TabIndex = 15;
		this->numericUpDown_Sim_SpdMaxOutput->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
		this->numericUpDown_Sim_SpdMaxOutput->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {80, 0, 0, 0});
		this->numericUpDown_Sim_SpdMaxOutput->ValueChanged += gcnew System::EventHandler(this, &BikeSpeedSensor::numericUpDown_Sim_SpdMinMaxOutput_ValueChanged);
		// 
		// label_Sim_SimType
		// 
		this->label_Sim_SimType->AutoSize = true;
		this->label_Sim_SimType->Location = System::Drawing::Point(162, 11);
		this->label_Sim_SimType->Name = L"label_Sim_SimType";
		this->label_Sim_SimType->Size = System::Drawing::Size(85, 13);
		this->label_Sim_SimType->TabIndex = 12;
		this->label_Sim_SimType->Text = L"Simulation Type:";
		this->label_Sim_SimType->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
		// 
		// radioButton_Sim_Fixed
		// 
		this->radioButton_Sim_Fixed->AutoSize = true;
		this->radioButton_Sim_Fixed->Checked = true;
		this->radioButton_Sim_Fixed->Location = System::Drawing::Point(176, 27);
		this->radioButton_Sim_Fixed->Name = L"radioButton_Sim_Fixed";
		this->radioButton_Sim_Fixed->Size = System::Drawing::Size(50, 17);
		this->radioButton_Sim_Fixed->TabIndex = 11;
		this->radioButton_Sim_Fixed->TabStop = true;
		this->radioButton_Sim_Fixed->Text = L"Fixed";
		this->radioButton_Sim_Fixed->UseVisualStyleBackColor = true;
		// 
		// numericUpDown_Sim_SpdMinOutput
		// 
		this->numericUpDown_Sim_SpdMinOutput->DecimalPlaces = 3;
		this->numericUpDown_Sim_SpdMinOutput->Enabled = false;
		this->numericUpDown_Sim_SpdMinOutput->Location = System::Drawing::Point(63, 30);
		this->numericUpDown_Sim_SpdMinOutput->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
		this->numericUpDown_Sim_SpdMinOutput->Name = L"numericUpDown_Sim_SpdMinOutput";
		this->numericUpDown_Sim_SpdMinOutput->Size = System::Drawing::Size(64, 20);
		this->numericUpDown_Sim_SpdMinOutput->TabIndex = 13;
		this->numericUpDown_Sim_SpdMinOutput->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
		this->numericUpDown_Sim_SpdMinOutput->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {10, 0, 0, 0});
		this->numericUpDown_Sim_SpdMinOutput->ValueChanged += gcnew System::EventHandler(this, &BikeSpeedSensor::numericUpDown_Sim_SpdMinMaxOutput_ValueChanged);
		// 
		// radioButton_Sim_Sweep
		// 
		this->radioButton_Sim_Sweep->AutoSize = true;
		this->radioButton_Sim_Sweep->Location = System::Drawing::Point(176, 50);
		this->radioButton_Sim_Sweep->Name = L"radioButton_Sim_Sweep";
		this->radioButton_Sim_Sweep->Size = System::Drawing::Size(58, 17);
		this->radioButton_Sim_Sweep->TabIndex = 12;
		this->radioButton_Sim_Sweep->Text = L"Sweep";
		this->radioButton_Sim_Sweep->UseVisualStyleBackColor = true;
		this->radioButton_Sim_Sweep->CheckedChanged += gcnew System::EventHandler(this, &BikeSpeedSensor::radioButton_SimTypeChanged);
		// 
		// numericUpDown_Sim_SpdCurOutput
		// 
		this->numericUpDown_Sim_SpdCurOutput->DecimalPlaces = 3;
		this->numericUpDown_Sim_SpdCurOutput->Location = System::Drawing::Point(63, 54);
		this->numericUpDown_Sim_SpdCurOutput->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
		this->numericUpDown_Sim_SpdCurOutput->Name = L"numericUpDown_Sim_SpdCurOutput";
		this->numericUpDown_Sim_SpdCurOutput->Size = System::Drawing::Size(64, 20);
		this->numericUpDown_Sim_SpdCurOutput->TabIndex = 14;
		this->numericUpDown_Sim_SpdCurOutput->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
		this->numericUpDown_Sim_SpdCurOutput->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {30, 0, 0, 0});
		this->numericUpDown_Sim_SpdCurOutput->ValueChanged += gcnew System::EventHandler(this, &BikeSpeedSensor::numericUpDown_Sim_SpdCurOutput_ValueChanged);
		// 
		// label_Sim_Speed
		// 
		this->label_Sim_Speed->AutoSize = true;
		this->label_Sim_Speed->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Underline, 
			System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
		this->label_Sim_Speed->Location = System::Drawing::Point(49, 11);
		this->label_Sim_Speed->Name = L"label_Sim_Speed";
		this->label_Sim_Speed->Size = System::Drawing::Size(68, 13);
		this->label_Sim_Speed->TabIndex = 0;
		this->label_Sim_Speed->Text = L"Speed (kph):";
		// 
		// tabPage_GlobalData
		// 
		this->tabPage_GlobalData->Controls->Add(this->button_UpdateTime);
		this->tabPage_GlobalData->Controls->Add(this->label_SoftwareVer);
		this->tabPage_GlobalData->Controls->Add(this->label_HardwareVer);
		this->tabPage_GlobalData->Controls->Add(this->label_ManfID);
		this->tabPage_GlobalData->Controls->Add(this->label_ModelNum);
		this->tabPage_GlobalData->Controls->Add(this->label_AdvancedError);
		this->tabPage_GlobalData->Controls->Add(this->textBox_ElpTimeChange);
		this->tabPage_GlobalData->Controls->Add(this->textBox_SoftwareVerChange);
		this->tabPage_GlobalData->Controls->Add(this->button_AdvancedUpdate);
		this->tabPage_GlobalData->Controls->Add(this->textBox_HardwareVerChange);
		this->tabPage_GlobalData->Controls->Add(this->textBox_ModelNumChange);
		this->tabPage_GlobalData->Controls->Add(this->textBox_SerialNumChange);
		this->tabPage_GlobalData->Controls->Add(this->textBox_ManfIDChange);
		this->tabPage_GlobalData->Controls->Add(this->label_ElapsedSecsDisplay);
		this->tabPage_GlobalData->Controls->Add(this->label_SerialNum);
		this->tabPage_GlobalData->Controls->Add(this->label_ElpTime);
		this->tabPage_GlobalData->Location = System::Drawing::Point(4, 22);
		this->tabPage_GlobalData->Name = L"tabPage_GlobalData";
		this->tabPage_GlobalData->Padding = System::Windows::Forms::Padding(3);
		this->tabPage_GlobalData->Size = System::Drawing::Size(389, 111);
		this->tabPage_GlobalData->TabIndex = 1;
		this->tabPage_GlobalData->Text = L"Global Data";
		this->tabPage_GlobalData->UseVisualStyleBackColor = true;
		// 
		// button_UpdateTime
		// 
		this->button_UpdateTime->Location = System::Drawing::Point(9, 76);
		this->button_UpdateTime->Name = L"button_UpdateTime";
		this->button_UpdateTime->Size = System::Drawing::Size(97, 20);
		this->button_UpdateTime->TabIndex = 24;
		this->button_UpdateTime->Text = L"Update Time";
		this->button_UpdateTime->UseVisualStyleBackColor = true;
		this->button_UpdateTime->Click += gcnew System::EventHandler(this, &BikeSpeedSensor::button_UpdateTime_Click);
		// 
		// label_SoftwareVer
		// 
		this->label_SoftwareVer->AutoSize = true;
		this->label_SoftwareVer->Location = System::Drawing::Point(147, 83);
		this->label_SoftwareVer->Name = L"label_SoftwareVer";
		this->label_SoftwareVer->Size = System::Drawing::Size(71, 13);
		this->label_SoftwareVer->TabIndex = 4;
		this->label_SoftwareVer->Text = L"Software Ver:";
		// 
		// label_HardwareVer
		// 
		this->label_HardwareVer->AutoSize = true;
		this->label_HardwareVer->Location = System::Drawing::Point(143, 61);
		this->label_HardwareVer->Name = L"label_HardwareVer";
		this->label_HardwareVer->Size = System::Drawing::Size(75, 13);
		this->label_HardwareVer->TabIndex = 3;
		this->label_HardwareVer->Text = L"Hardware Ver:";
		// 
		// label_ManfID
		// 
		this->label_ManfID->AutoSize = true;
		this->label_ManfID->Location = System::Drawing::Point(167, 13);
		this->label_ManfID->Name = L"label_ManfID";
		this->label_ManfID->Size = System::Drawing::Size(51, 13);
		this->label_ManfID->TabIndex = 1;
		this->label_ManfID->Text = L"Manf. ID:";
		// 
		// label_ModelNum
		// 
		this->label_ModelNum->AutoSize = true;
		this->label_ModelNum->Location = System::Drawing::Point(169, 37);
		this->label_ModelNum->Name = L"label_ModelNum";
		this->label_ModelNum->Size = System::Drawing::Size(49, 13);
		this->label_ModelNum->TabIndex = 5;
		this->label_ModelNum->Text = L"Model #:";
		// 
		// label_AdvancedError
		// 
		this->label_AdvancedError->AutoSize = true;
		this->label_AdvancedError->Location = System::Drawing::Point(280, 81);
		this->label_AdvancedError->Name = L"label_AdvancedError";
		this->label_AdvancedError->Size = System::Drawing::Size(32, 13);
		this->label_AdvancedError->TabIndex = 20;
		this->label_AdvancedError->Text = L"Error:";
		this->label_AdvancedError->Visible = false;
		// 
		// textBox_ElpTimeChange
		// 
		this->textBox_ElpTimeChange->Location = System::Drawing::Point(28, 41);
		this->textBox_ElpTimeChange->MaxLength = 8;
		this->textBox_ElpTimeChange->Name = L"textBox_ElpTimeChange";
		this->textBox_ElpTimeChange->Size = System::Drawing::Size(63, 20);
		this->textBox_ElpTimeChange->TabIndex = 17;
		this->textBox_ElpTimeChange->Text = L"0";
		this->textBox_ElpTimeChange->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
		// 
		// textBox_SoftwareVerChange
		// 
		this->textBox_SoftwareVerChange->Location = System::Drawing::Point(233, 81);
		this->textBox_SoftwareVerChange->MaxLength = 3;
		this->textBox_SoftwareVerChange->Name = L"textBox_SoftwareVerChange";
		this->textBox_SoftwareVerChange->Size = System::Drawing::Size(29, 20);
		this->textBox_SoftwareVerChange->TabIndex = 21;
		this->textBox_SoftwareVerChange->Text = L"1";
		// 
		// button_AdvancedUpdate
		// 
		this->button_AdvancedUpdate->Location = System::Drawing::Point(283, 41);
		this->button_AdvancedUpdate->Name = L"button_AdvancedUpdate";
		this->button_AdvancedUpdate->Size = System::Drawing::Size(97, 20);
		this->button_AdvancedUpdate->TabIndex = 23;
		this->button_AdvancedUpdate->Text = L"Update Prod Info";
		this->button_AdvancedUpdate->UseVisualStyleBackColor = true;
		this->button_AdvancedUpdate->Click += gcnew System::EventHandler(this, &BikeSpeedSensor::button_AdvancedUpdate_Click);
		// 
		// textBox_HardwareVerChange
		// 
		this->textBox_HardwareVerChange->Location = System::Drawing::Point(233, 59);
		this->textBox_HardwareVerChange->MaxLength = 3;
		this->textBox_HardwareVerChange->Name = L"textBox_HardwareVerChange";
		this->textBox_HardwareVerChange->Size = System::Drawing::Size(29, 20);
		this->textBox_HardwareVerChange->TabIndex = 20;
		this->textBox_HardwareVerChange->Text = L"1";
		// 
		// textBox_ModelNumChange
		// 
		this->textBox_ModelNumChange->Location = System::Drawing::Point(233, 35);
		this->textBox_ModelNumChange->MaxLength = 3;
		this->textBox_ModelNumChange->Name = L"textBox_ModelNumChange";
		this->textBox_ModelNumChange->Size = System::Drawing::Size(29, 20);
		this->textBox_ModelNumChange->TabIndex = 19;
		this->textBox_ModelNumChange->Text = L"1";
		// 
		// textBox_SerialNumChange
		// 
		this->textBox_SerialNumChange->Location = System::Drawing::Point(332, 9);
		this->textBox_SerialNumChange->MaxLength = 5;
		this->textBox_SerialNumChange->Name = L"textBox_SerialNumChange";
		this->textBox_SerialNumChange->Size = System::Drawing::Size(49, 20);
		this->textBox_SerialNumChange->TabIndex = 22;
		this->textBox_SerialNumChange->Text = L"12345";
		// 
		// textBox_ManfIDChange
		// 
		this->textBox_ManfIDChange->Location = System::Drawing::Point(233, 10);
		this->textBox_ManfIDChange->MaxLength = 3;
		this->textBox_ManfIDChange->Name = L"textBox_ManfIDChange";
		this->textBox_ManfIDChange->Size = System::Drawing::Size(29, 20);
		this->textBox_ManfIDChange->TabIndex = 18;
		this->textBox_ManfIDChange->Text = L"123";
		// 
		// label_ElapsedSecsDisplay
		// 
		this->label_ElapsedSecsDisplay->Location = System::Drawing::Point(34, 23);
		this->label_ElapsedSecsDisplay->Name = L"label_ElapsedSecsDisplay";
		this->label_ElapsedSecsDisplay->Size = System::Drawing::Size(55, 13);
		this->label_ElapsedSecsDisplay->TabIndex = 7;
		this->label_ElapsedSecsDisplay->Text = L"0";
		this->label_ElapsedSecsDisplay->TextAlign = System::Drawing::ContentAlignment::BottomRight;
		// 
		// label_SerialNum
		// 
		this->label_SerialNum->AutoSize = true;
		this->label_SerialNum->Location = System::Drawing::Point(280, 13);
		this->label_SerialNum->Name = L"label_SerialNum";
		this->label_SerialNum->Size = System::Drawing::Size(46, 13);
		this->label_SerialNum->TabIndex = 2;
		this->label_SerialNum->Text = L"Serial #:";
		// 
		// label_ElpTime
		// 
		this->label_ElpTime->AutoSize = true;
		this->label_ElpTime->Location = System::Drawing::Point(6, 6);
		this->label_ElpTime->Name = L"label_ElpTime";
		this->label_ElpTime->Size = System::Drawing::Size(88, 13);
		this->label_ElpTime->TabIndex = 0;
		this->label_ElpTime->Text = L"Elapsed Time (s):";
		// 
		// panel_Display
		// 
		this->panel_Display->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
		this->panel_Display->Controls->Add(this->label_Trn_TimeDisplay);
		this->panel_Display->Controls->Add(this->label_Trn_Time);
		this->panel_Display->Controls->Add(this->label_Trn_TranslatedDisplayLabel);
		this->panel_Display->Controls->Add(this->label_Trn_EventCountDisplay);
		this->panel_Display->Controls->Add(this->label_Trn_EventCount);
		this->panel_Display->Location = System::Drawing::Point(58, 188);
		this->panel_Display->Name = L"panel_Display";
		this->panel_Display->Size = System::Drawing::Size(200, 90);
		this->panel_Display->TabIndex = 1;
		// 
		// label_Trn_TimeDisplay
		// 
		this->label_Trn_TimeDisplay->AutoSize = true;
		this->label_Trn_TimeDisplay->Location = System::Drawing::Point(141, 51);
		this->label_Trn_TimeDisplay->Name = L"label_Trn_TimeDisplay";
		this->label_Trn_TimeDisplay->Size = System::Drawing::Size(16, 13);
		this->label_Trn_TimeDisplay->TabIndex = 6;
		this->label_Trn_TimeDisplay->Text = L"---";
		this->label_Trn_TimeDisplay->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
		// 
		// label_Trn_Time
		// 
		this->label_Trn_Time->AutoSize = true;
		this->label_Trn_Time->Location = System::Drawing::Point(22, 51);
		this->label_Trn_Time->Name = L"label_Trn_Time";
		this->label_Trn_Time->Size = System::Drawing::Size(113, 13);
		this->label_Trn_Time->TabIndex = 5;
		this->label_Trn_Time->Text = L"Event Time (1/1024s):";
		this->label_Trn_Time->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
		// 
		// label_Trn_TranslatedDisplayLabel
		// 
		this->label_Trn_TranslatedDisplayLabel->AutoSize = true;
		this->label_Trn_TranslatedDisplayLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Underline, 
			System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
		this->label_Trn_TranslatedDisplayLabel->Location = System::Drawing::Point(2, 2);
		this->label_Trn_TranslatedDisplayLabel->Name = L"label_Trn_TranslatedDisplayLabel";
		this->label_Trn_TranslatedDisplayLabel->Size = System::Drawing::Size(169, 13);
		this->label_Trn_TranslatedDisplayLabel->TabIndex = 4;
		this->label_Trn_TranslatedDisplayLabel->Text = L"Current Bike Speed Transmission :";
		// 
		// label_Trn_EventCountDisplay
		// 
		this->label_Trn_EventCountDisplay->AutoSize = true;
		this->label_Trn_EventCountDisplay->Location = System::Drawing::Point(141, 28);
		this->label_Trn_EventCountDisplay->Name = L"label_Trn_EventCountDisplay";
		this->label_Trn_EventCountDisplay->Size = System::Drawing::Size(16, 13);
		this->label_Trn_EventCountDisplay->TabIndex = 2;
		this->label_Trn_EventCountDisplay->Text = L"---";
		this->label_Trn_EventCountDisplay->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
		// 
		// label_Trn_EventCount
		// 
		this->label_Trn_EventCount->AutoSize = true;
		this->label_Trn_EventCount->Location = System::Drawing::Point(66, 28);
		this->label_Trn_EventCount->Name = L"label_Trn_EventCount";
		this->label_Trn_EventCount->Size = System::Drawing::Size(69, 13);
		this->label_Trn_EventCount->TabIndex = 0;
		this->label_Trn_EventCount->Text = L"Event Count:";
		this->label_Trn_EventCount->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
		// 
		// BikeSpeedSensor
		// 
		this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
		this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
		this->ClientSize = System::Drawing::Size(794, 351);
		this->Controls->Add(this->panel_Display);
		this->Controls->Add(this->panel_Settings);
		this->Name = L"BikeSpeedSensor";
		this->Text = L"BikeSpeedSensor";
		this->panel_Settings->ResumeLayout(false);
		this->tabControl_Settings->ResumeLayout(false);
		this->tabPage_Simulation->ResumeLayout(false);
		this->tabPage_Simulation->PerformLayout();
		(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_WheelCircumference))->EndInit();
		(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_SpdMaxOutput))->EndInit();
		(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_SpdMinOutput))->EndInit();
		(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_SpdCurOutput))->EndInit();
		this->tabPage_GlobalData->ResumeLayout(false);
		this->tabPage_GlobalData->PerformLayout();
		this->panel_Display->ResumeLayout(false);
		this->panel_Display->PerformLayout();
		this->ResumeLayout(false);

	}
#pragma endregion

};