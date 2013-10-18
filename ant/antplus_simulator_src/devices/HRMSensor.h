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
#include "hrm.h"
#include "types.h"
#include "antdefines.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

public ref class HRMSensor : public System::Windows::Forms::Form, public ISimBase{
public:
	HRMSensor(System::Timers::Timer^ channelTimer){
		InitializeComponent();
		timerHandle = channelTimer;		// Get timer handle
		InitializeSim();	

	}

	~HRMSensor(){
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
	virtual UCHAR getDeviceType(){return HRM_DEVICE_TYPE;}
	virtual UCHAR getTransmissionType(){return HRM_TX_TYPE;}
	virtual USHORT getTransmitPeriod(){return HRM_MSG_PERIOD;}
	virtual DOUBLE getTimerInterval(){return (double) ulTimerInterval;}
	virtual System::Windows::Forms::Panel^ getSimSettingsPanel(){return this->panel_Settings;}
	virtual System::Windows::Forms::Panel^ getSimTranslatedDisplay(){return this->panel_Display;}

private:
	void InitializeSim();
	void HandleTransmit(UCHAR* pucTxBuffer_);
	void UpdateDisplay();
	void ForceUpdate();
	System::Void radioButton_SimTypeChanged (System::Object^  sender, System::EventArgs^  e);
	System::Void button_AdvancedUpdate_Click(System::Object^  sender, System::EventArgs^  e);
	System::Void button_UpdateTime_Click(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_Prm_CurPulse_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_Legacy_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_SendBasicPage_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_Prm_MinMaxPulse_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	

private:
	// Simulator timer
	System::Timers::Timer^ timerHandle;	// Timer handle
	ULONG ulTimerInterval;				// Timer interval between simulated events
	
	// Simulator variables
	ULONG ulRunTime;				// Time in ms
	ULONG ulRunTime16000;			// Time in 1/16000s, for conversion between time base of ms to 1/1024s
	UCHAR ucReserved;				// Reserved field
	UCHAR ucBackgroundCount;		// To send multiple times the same extended message
	UCHAR ucNextBackgroundPage;		// Next extended page to send
	UCHAR ucBPM;					// Heart rate (bpm)
	UCHAR ucEventCount;				// Heart beat count
	UCHAR ucMinPulse;				// Minimum heart rate (bpm)
	UCHAR ucCurPulse;				// Current heart rate (bpm)
	UCHAR ucMaxPulse;				// Maximum heart rate (bpm)
	ULONG ulElapsedTime2;			// Cumulative operating time (elapsed time) in 2 second resolution
	USHORT usTime1024;				// Time of last heart beat event (1/1024 seconds)
	USHORT usPreviousTime1024;		// Time of previous heart beat event (1/1024 seconds)

	// Background Data
	UCHAR ucMfgID;			// Manufacturing ID
	UCHAR ucHwVersion;		// Hardware version
	UCHAR ucSwVersion;		// Software version
	UCHAR ucModelNum;		// Model number
	USHORT usSerialNum;		// Serial number

	// Status
	UCHAR ucSimDataType;	// Method to generate simulated data
	BOOL bLegacy;			// Enable simulation of legacy sensors
	BOOL bTxMinimum;		// Enable transmission of basic data set
	BOOL bSweepAscending;	// Sweep through heart rate values ascending



private: System::Windows::Forms::Button^  button_AdvancedUpdate;
private: System::Windows::Forms::Button^  button_UpdateTime;
private: System::Windows::Forms::CheckBox^  checkBox_Legacy;
private: System::Windows::Forms::CheckBox^  checkBox_SendBasicPage;
private: System::Windows::Forms::Label^  label_AdvancedError;
private: System::Windows::Forms::Label^  label_CurrentPulse;
private: System::Windows::Forms::Label^  label_CurrentTime;
private: System::Windows::Forms::Label^  label_CurrentTimeDisplay;
private: System::Windows::Forms::Label^  label_ElapsedSecsDisplay;
private: System::Windows::Forms::Label^  label_ElpTime;
private: System::Windows::Forms::Label^  label_EventCount;
private: System::Windows::Forms::Label^  label_HardwareVer;
private: System::Windows::Forms::Label^  label_HRSimType;
private: System::Windows::Forms::Label^  label_LastTime;
private: System::Windows::Forms::Label^  label_LastTimeDisplay;
private: System::Windows::Forms::Label^  label_ManfID;
private: System::Windows::Forms::Label^  label_MaxPulse;
private: System::Windows::Forms::Label^  label_MinPulse;
private: System::Windows::Forms::Label^  label_ModelNum;
private: System::Windows::Forms::Label^  label_Pulse;
private: System::Windows::Forms::Label^  label_PulseTxd;
private: System::Windows::Forms::Label^  label_SerialNum;
private: System::Windows::Forms::Label^  label_SoftwareVer;
private: System::Windows::Forms::Label^  label_TranslatedDisplayLabel;
private: System::Windows::Forms::Label^  label_TxEventCount;
private: System::Windows::Forms::Label^  label_TxPulseOf;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Prm_CurPulse;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Prm_MaxPulse;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Prm_MinPulse;
private: System::Windows::Forms::Panel^  panel_Display;
private: System::Windows::Forms::Panel^  panel_Settings;
private: System::Windows::Forms::RadioButton^  radioButton_HRFixed;
private: System::Windows::Forms::RadioButton^  radioButton_HRStep;
private: System::Windows::Forms::RadioButton^  radioButton_HRSweep;
private: System::Windows::Forms::TabControl^  tabControl_Settings;
private: System::Windows::Forms::TabPage^  tabPage_Advanced;
private: System::Windows::Forms::TabPage^  tabPage_Parameters;
private: System::Windows::Forms::TextBox^  textBox_ElpTimeChange;
private: System::Windows::Forms::TextBox^  textBox_HardwareVerChange;
private: System::Windows::Forms::TextBox^  textBox_ManfIDChange;
private: System::Windows::Forms::TextBox^  textBox_ModelNumChange;
private: System::Windows::Forms::TextBox^  textBox_SerialNumChange;
private: System::Windows::Forms::TextBox^  textBox_SoftwareVerChange;




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
		this->button_AdvancedUpdate = (gcnew System::Windows::Forms::Button());
		this->button_UpdateTime = (gcnew System::Windows::Forms::Button());
		this->checkBox_Legacy = (gcnew System::Windows::Forms::CheckBox());
		this->checkBox_SendBasicPage = (gcnew System::Windows::Forms::CheckBox());
		this->label_AdvancedError = (gcnew System::Windows::Forms::Label());
		this->label_CurrentPulse = (gcnew System::Windows::Forms::Label());
		this->label_CurrentTime = (gcnew System::Windows::Forms::Label());
		this->label_CurrentTimeDisplay = (gcnew System::Windows::Forms::Label());
		this->label_ElapsedSecsDisplay = (gcnew System::Windows::Forms::Label());
		this->label_ElpTime = (gcnew System::Windows::Forms::Label());
		this->label_EventCount = (gcnew System::Windows::Forms::Label());
		this->label_HardwareVer = (gcnew System::Windows::Forms::Label());
		this->label_HRSimType = (gcnew System::Windows::Forms::Label());
		this->label_LastTime = (gcnew System::Windows::Forms::Label());
		this->label_LastTimeDisplay = (gcnew System::Windows::Forms::Label());
		this->label_ManfID = (gcnew System::Windows::Forms::Label());
		this->label_MaxPulse = (gcnew System::Windows::Forms::Label());
		this->label_MinPulse = (gcnew System::Windows::Forms::Label());
		this->label_ModelNum = (gcnew System::Windows::Forms::Label());
		this->label_Pulse = (gcnew System::Windows::Forms::Label());
		this->label_PulseTxd = (gcnew System::Windows::Forms::Label());
		this->label_SerialNum = (gcnew System::Windows::Forms::Label());
		this->label_SoftwareVer = (gcnew System::Windows::Forms::Label());
		this->label_TranslatedDisplayLabel = (gcnew System::Windows::Forms::Label());
		this->label_TxEventCount = (gcnew System::Windows::Forms::Label());
		this->label_TxPulseOf = (gcnew System::Windows::Forms::Label());
		this->numericUpDown_Prm_CurPulse = (gcnew System::Windows::Forms::NumericUpDown());
		this->numericUpDown_Prm_MaxPulse = (gcnew System::Windows::Forms::NumericUpDown());
		this->numericUpDown_Prm_MinPulse = (gcnew System::Windows::Forms::NumericUpDown());
		this->panel_Settings = (gcnew System::Windows::Forms::Panel());
		this->panel_Display = (gcnew System::Windows::Forms::Panel());
		this->radioButton_HRFixed = (gcnew System::Windows::Forms::RadioButton());
		this->radioButton_HRStep = (gcnew System::Windows::Forms::RadioButton());
		this->radioButton_HRSweep = (gcnew System::Windows::Forms::RadioButton());
		this->tabControl_Settings = (gcnew System::Windows::Forms::TabControl());
		this->tabPage_Advanced = (gcnew System::Windows::Forms::TabPage());
		this->tabPage_Parameters = (gcnew System::Windows::Forms::TabPage());
		this->textBox_ElpTimeChange = (gcnew System::Windows::Forms::TextBox());
		this->textBox_HardwareVerChange = (gcnew System::Windows::Forms::TextBox());
		this->textBox_ManfIDChange = (gcnew System::Windows::Forms::TextBox());
		this->textBox_ModelNumChange = (gcnew System::Windows::Forms::TextBox());
		this->textBox_SerialNumChange = (gcnew System::Windows::Forms::TextBox());
		this->textBox_SoftwareVerChange = (gcnew System::Windows::Forms::TextBox());
		this->panel_Settings->SuspendLayout();
		this->tabControl_Settings->SuspendLayout();
		this->tabPage_Parameters->SuspendLayout();
		(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Prm_MaxPulse))->BeginInit();
		(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Prm_MinPulse))->BeginInit();
		(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Prm_CurPulse))->BeginInit();
		this->tabPage_Advanced->SuspendLayout();
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
		this->tabControl_Settings->Controls->Add(this->tabPage_Advanced);
		this->tabControl_Settings->Location = System::Drawing::Point(0, 3);
		this->tabControl_Settings->Name = L"tabControl_Settings";
		this->tabControl_Settings->SelectedIndex = 0;
		this->tabControl_Settings->Size = System::Drawing::Size(397, 137);
		this->tabControl_Settings->TabIndex = 10;
		// 
		// tabPage_Parameters
		// 
		this->tabPage_Parameters->Controls->Add(this->checkBox_Legacy);
		this->tabPage_Parameters->Controls->Add(this->numericUpDown_Prm_MaxPulse);
		this->tabPage_Parameters->Controls->Add(this->checkBox_SendBasicPage);
		this->tabPage_Parameters->Controls->Add(this->numericUpDown_Prm_MinPulse);
		this->tabPage_Parameters->Controls->Add(this->numericUpDown_Prm_CurPulse);
		this->tabPage_Parameters->Controls->Add(this->label_HRSimType);
		this->tabPage_Parameters->Controls->Add(this->radioButton_HRStep);
		this->tabPage_Parameters->Controls->Add(this->radioButton_HRSweep);
		this->tabPage_Parameters->Controls->Add(this->radioButton_HRFixed);
		this->tabPage_Parameters->Controls->Add(this->label_MaxPulse);
		this->tabPage_Parameters->Controls->Add(this->label_CurrentPulse);
		this->tabPage_Parameters->Controls->Add(this->label_MinPulse);
		this->tabPage_Parameters->Controls->Add(this->label_Pulse);
		this->tabPage_Parameters->Location = System::Drawing::Point(4, 22);
		this->tabPage_Parameters->Name = L"tabPage_Parameters";
		this->tabPage_Parameters->Padding = System::Windows::Forms::Padding(3);
		this->tabPage_Parameters->Size = System::Drawing::Size(389, 111);
		this->tabPage_Parameters->TabIndex = 0;
		this->tabPage_Parameters->Text = L"Parameters";
		this->tabPage_Parameters->UseVisualStyleBackColor = true;
		// 
		// checkBox_Legacy
		// 
		this->checkBox_Legacy->AutoSize = true;
		this->checkBox_Legacy->Location = System::Drawing::Point(170, 91);
		this->checkBox_Legacy->Name = L"checkBox_Legacy";
		this->checkBox_Legacy->Size = System::Drawing::Size(61, 17);
		this->checkBox_Legacy->TabIndex = 20;
		this->checkBox_Legacy->Text = L"Legacy";
		this->checkBox_Legacy->UseVisualStyleBackColor = true;
		this->checkBox_Legacy->CheckedChanged += gcnew System::EventHandler(this, &HRMSensor::checkBox_Legacy_CheckedChanged);
		// 
		// numericUpDown_Prm_MaxPulse
		// 
		this->numericUpDown_Prm_MaxPulse->Enabled = false;
		this->numericUpDown_Prm_MaxPulse->Location = System::Drawing::Point(108, 60);
		this->numericUpDown_Prm_MaxPulse->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
		this->numericUpDown_Prm_MaxPulse->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
		this->numericUpDown_Prm_MaxPulse->Name = L"numericUpDown_Prm_MaxPulse";
		this->numericUpDown_Prm_MaxPulse->Size = System::Drawing::Size(45, 20);
		this->numericUpDown_Prm_MaxPulse->TabIndex = 4;
		this->numericUpDown_Prm_MaxPulse->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {180, 0, 0, 0});
      this->numericUpDown_Prm_MaxPulse->ValueChanged += gcnew System::EventHandler(this, &HRMSensor::numericUpDown_Prm_MinMaxPulse_ValueChanged);
		// 
		// checkBox_SendBasicPage
		// 
		this->checkBox_SendBasicPage->AutoSize = true;
		this->checkBox_SendBasicPage->Location = System::Drawing::Point(25, 91);
		this->checkBox_SendBasicPage->Name = L"checkBox_SendBasicPage";
		this->checkBox_SendBasicPage->Size = System::Drawing::Size(112, 17);
		this->checkBox_SendBasicPage->TabIndex = 19;
		this->checkBox_SendBasicPage->Text = L"Minimum Data Set";
		this->checkBox_SendBasicPage->UseVisualStyleBackColor = true;
		this->checkBox_SendBasicPage->CheckedChanged += gcnew System::EventHandler(this, &HRMSensor::checkBox_SendBasicPage_CheckedChanged);
		// 
		// numericUpDown_Prm_MinPulse
		// 
		this->numericUpDown_Prm_MinPulse->Enabled = false;
		this->numericUpDown_Prm_MinPulse->Location = System::Drawing::Point(108, 9);
		this->numericUpDown_Prm_MinPulse->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
		this->numericUpDown_Prm_MinPulse->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
		this->numericUpDown_Prm_MinPulse->Name = L"numericUpDown_Prm_MinPulse";
		this->numericUpDown_Prm_MinPulse->Size = System::Drawing::Size(45, 20);
		this->numericUpDown_Prm_MinPulse->TabIndex = 3;
		this->numericUpDown_Prm_MinPulse->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {45, 0, 0, 0});
		this->numericUpDown_Prm_MinPulse->ValueChanged += gcnew System::EventHandler(this, &HRMSensor::numericUpDown_Prm_MinMaxPulse_ValueChanged);
		// 
		// numericUpDown_Prm_CurPulse
		// 
		this->numericUpDown_Prm_CurPulse->Location = System::Drawing::Point(108, 34);
		this->numericUpDown_Prm_CurPulse->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
		this->numericUpDown_Prm_CurPulse->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
		this->numericUpDown_Prm_CurPulse->Name = L"numericUpDown_Prm_CurPulse";
		this->numericUpDown_Prm_CurPulse->Size = System::Drawing::Size(45, 20);
		this->numericUpDown_Prm_CurPulse->TabIndex = 2;
		this->numericUpDown_Prm_CurPulse->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {72, 0, 0, 0});
		this->numericUpDown_Prm_CurPulse->ValueChanged += gcnew System::EventHandler(this, &HRMSensor::numericUpDown_Prm_CurPulse_ValueChanged);
		// 
		// label_HRSimType
		// 
		this->label_HRSimType->AutoSize = true;
		this->label_HRSimType->Location = System::Drawing::Point(193, 12);
		this->label_HRSimType->Name = L"label_HRSimType";
		this->label_HRSimType->Size = System::Drawing::Size(77, 39);
		this->label_HRSimType->TabIndex = 12;
		this->label_HRSimType->Text = L"Heart Rate\r\nSimulator Type\r\n------------------";
		this->label_HRSimType->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
		// 
		// radioButton_HRStep
		// 
		this->radioButton_HRStep->AutoSize = true;
		this->radioButton_HRStep->Location = System::Drawing::Point(283, 59);
		this->radioButton_HRStep->Name = L"radioButton_HRStep";
		this->radioButton_HRStep->Size = System::Drawing::Size(47, 17);
		this->radioButton_HRStep->TabIndex = 18;
		this->radioButton_HRStep->Text = L"Step";
		this->radioButton_HRStep->UseVisualStyleBackColor = true;
		this->radioButton_HRStep->CheckedChanged += gcnew System::EventHandler(this, &HRMSensor::radioButton_SimTypeChanged);
		// 
		// radioButton_HRSweep
		// 
		this->radioButton_HRSweep->AutoSize = true;
		this->radioButton_HRSweep->Location = System::Drawing::Point(283, 37);
		this->radioButton_HRSweep->Name = L"radioButton_HRSweep";
		this->radioButton_HRSweep->Size = System::Drawing::Size(58, 17);
		this->radioButton_HRSweep->TabIndex = 16;
		this->radioButton_HRSweep->Text = L"Sweep";
		this->radioButton_HRSweep->UseVisualStyleBackColor = true;
		this->radioButton_HRSweep->CheckedChanged += gcnew System::EventHandler(this, &HRMSensor::radioButton_SimTypeChanged);
		// 
		// radioButton_HRFixed
		// 
		this->radioButton_HRFixed->AutoSize = true;
		this->radioButton_HRFixed->Checked = true;
		this->radioButton_HRFixed->Location = System::Drawing::Point(283, 14);
		this->radioButton_HRFixed->Name = L"radioButton_HRFixed";
		this->radioButton_HRFixed->Size = System::Drawing::Size(50, 17);
		this->radioButton_HRFixed->TabIndex = 15;
		this->radioButton_HRFixed->TabStop = true;
		this->radioButton_HRFixed->Text = L"Fixed";
		this->radioButton_HRFixed->UseVisualStyleBackColor = true;
		this->radioButton_HRFixed->CheckedChanged += gcnew System::EventHandler(this, &HRMSensor::radioButton_SimTypeChanged);
		// 
		// label_MaxPulse
		// 
		this->label_MaxPulse->AutoSize = true;
		this->label_MaxPulse->Location = System::Drawing::Point(75, 63);
		this->label_MaxPulse->Name = L"label_MaxPulse";
		this->label_MaxPulse->Size = System::Drawing::Size(27, 13);
		this->label_MaxPulse->TabIndex = 3;
		this->label_MaxPulse->Text = L"Max";
		// 
		// label_CurrentPulse
		// 
		this->label_CurrentPulse->AutoSize = true;
		this->label_CurrentPulse->Location = System::Drawing::Point(61, 37);
		this->label_CurrentPulse->Name = L"label_CurrentPulse";
		this->label_CurrentPulse->Size = System::Drawing::Size(41, 13);
		this->label_CurrentPulse->TabIndex = 2;
		this->label_CurrentPulse->Text = L"Current";
		// 
		// label_MinPulse
		// 
		this->label_MinPulse->AutoSize = true;
		this->label_MinPulse->Location = System::Drawing::Point(78, 12);
		this->label_MinPulse->Name = L"label_MinPulse";
		this->label_MinPulse->Size = System::Drawing::Size(24, 13);
		this->label_MinPulse->TabIndex = 1;
		this->label_MinPulse->Text = L"Min";
		// 
		// label_Pulse
		// 
		this->label_Pulse->AutoSize = true;
		this->label_Pulse->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Underline, System::Drawing::GraphicsUnit::Point, 
			static_cast<System::Byte>(0)));
		this->label_Pulse->Location = System::Drawing::Point(22, 11);
		this->label_Pulse->Name = L"label_Pulse";
		this->label_Pulse->Size = System::Drawing::Size(36, 13);
		this->label_Pulse->TabIndex = 0;
		this->label_Pulse->Text = L"Pulse:";
		// 
		// tabPage_Advanced
		// 
		this->tabPage_Advanced->Controls->Add(this->button_UpdateTime);
		this->tabPage_Advanced->Controls->Add(this->label_SoftwareVer);
		this->tabPage_Advanced->Controls->Add(this->label_HardwareVer);
		this->tabPage_Advanced->Controls->Add(this->label_ManfID);
		this->tabPage_Advanced->Controls->Add(this->label_ModelNum);
		this->tabPage_Advanced->Controls->Add(this->label_AdvancedError);
		this->tabPage_Advanced->Controls->Add(this->textBox_ElpTimeChange);
		this->tabPage_Advanced->Controls->Add(this->textBox_SoftwareVerChange);
		this->tabPage_Advanced->Controls->Add(this->button_AdvancedUpdate);
		this->tabPage_Advanced->Controls->Add(this->textBox_HardwareVerChange);
		this->tabPage_Advanced->Controls->Add(this->textBox_ModelNumChange);
		this->tabPage_Advanced->Controls->Add(this->textBox_SerialNumChange);
		this->tabPage_Advanced->Controls->Add(this->textBox_ManfIDChange);
		this->tabPage_Advanced->Controls->Add(this->label_ElapsedSecsDisplay);
		this->tabPage_Advanced->Controls->Add(this->label_SerialNum);
		this->tabPage_Advanced->Controls->Add(this->label_ElpTime);
		this->tabPage_Advanced->Location = System::Drawing::Point(4, 22);
		this->tabPage_Advanced->Name = L"tabPage_Advanced";
		this->tabPage_Advanced->Padding = System::Windows::Forms::Padding(3);
		this->tabPage_Advanced->Size = System::Drawing::Size(389, 111);
		this->tabPage_Advanced->TabIndex = 1;
		this->tabPage_Advanced->Text = L"Advanced";
		this->tabPage_Advanced->UseVisualStyleBackColor = true;
		// 
		// button_UpdateTime
		// 
		this->button_UpdateTime->Location = System::Drawing::Point(9, 76);
		this->button_UpdateTime->Name = L"button_UpdateTime";
		this->button_UpdateTime->Size = System::Drawing::Size(97, 20);
		this->button_UpdateTime->TabIndex = 16;
		this->button_UpdateTime->Text = L"Update Time";
		this->button_UpdateTime->UseVisualStyleBackColor = true;
		this->button_UpdateTime->Click += gcnew System::EventHandler(this, &HRMSensor::button_UpdateTime_Click);
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
		this->textBox_ElpTimeChange->TabIndex = 21;
		this->textBox_ElpTimeChange->Text = L"0";
		this->textBox_ElpTimeChange->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
		// 
		// textBox_SoftwareVerChange
		// 
		this->textBox_SoftwareVerChange->Location = System::Drawing::Point(233, 81);
		this->textBox_SoftwareVerChange->MaxLength = 3;
		this->textBox_SoftwareVerChange->Name = L"textBox_SoftwareVerChange";
		this->textBox_SoftwareVerChange->Size = System::Drawing::Size(29, 20);
		this->textBox_SoftwareVerChange->TabIndex = 25;
		this->textBox_SoftwareVerChange->Text = L"1";
		// 
		// button_AdvancedUpdate
		// 
		this->button_AdvancedUpdate->Location = System::Drawing::Point(283, 41);
		this->button_AdvancedUpdate->Name = L"button_AdvancedUpdate";
		this->button_AdvancedUpdate->Size = System::Drawing::Size(97, 20);
		this->button_AdvancedUpdate->TabIndex = 27;
		this->button_AdvancedUpdate->Text = L"Update Prod Info";
		this->button_AdvancedUpdate->UseVisualStyleBackColor = true;
		this->button_AdvancedUpdate->Click += gcnew System::EventHandler(this, &HRMSensor::button_AdvancedUpdate_Click);
		// 
		// textBox_HardwareVerChange
		// 
		this->textBox_HardwareVerChange->Location = System::Drawing::Point(233, 59);
		this->textBox_HardwareVerChange->MaxLength = 3;
		this->textBox_HardwareVerChange->Name = L"textBox_HardwareVerChange";
		this->textBox_HardwareVerChange->Size = System::Drawing::Size(29, 20);
		this->textBox_HardwareVerChange->TabIndex = 24;
		this->textBox_HardwareVerChange->Text = L"1";
		// 
		// textBox_ModelNumChange
		// 
		this->textBox_ModelNumChange->Location = System::Drawing::Point(233, 35);
		this->textBox_ModelNumChange->MaxLength = 3;
		this->textBox_ModelNumChange->Name = L"textBox_ModelNumChange";
		this->textBox_ModelNumChange->Size = System::Drawing::Size(29, 20);
		this->textBox_ModelNumChange->TabIndex = 23;
		this->textBox_ModelNumChange->Text = L"1";
		// 
		// textBox_SerialNumChange
		// 
		this->textBox_SerialNumChange->Location = System::Drawing::Point(332, 9);
		this->textBox_SerialNumChange->MaxLength = 5;
		this->textBox_SerialNumChange->Name = L"textBox_SerialNumChange";
		this->textBox_SerialNumChange->Size = System::Drawing::Size(49, 20);
		this->textBox_SerialNumChange->TabIndex = 26;
		this->textBox_SerialNumChange->Text = L"12345";
		// 
		// textBox_ManfIDChange
		// 
		this->textBox_ManfIDChange->Location = System::Drawing::Point(233, 10);
		this->textBox_ManfIDChange->MaxLength = 3;
		this->textBox_ManfIDChange->Name = L"textBox_ManfIDChange";
		this->textBox_ManfIDChange->Size = System::Drawing::Size(29, 20);
		this->textBox_ManfIDChange->TabIndex = 22;
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
		this->panel_Display->Controls->Add(this->label_LastTimeDisplay);
		this->panel_Display->Controls->Add(this->label_LastTime);
		this->panel_Display->Controls->Add(this->label_CurrentTimeDisplay);
		this->panel_Display->Controls->Add(this->label_CurrentTime);
		this->panel_Display->Controls->Add(this->label_EventCount);
		this->panel_Display->Controls->Add(this->label_TxEventCount);
		this->panel_Display->Controls->Add(this->label_TranslatedDisplayLabel);
		this->panel_Display->Controls->Add(this->label_PulseTxd);
		this->panel_Display->Controls->Add(this->label_TxPulseOf);
		this->panel_Display->Location = System::Drawing::Point(58, 188);
		this->panel_Display->Name = L"panel_Display";
		this->panel_Display->Size = System::Drawing::Size(200, 90);
		this->panel_Display->TabIndex = 1;
		// 
		// label_LastTimeDisplay
		// 
		this->label_LastTimeDisplay->AutoSize = true;
		this->label_LastTimeDisplay->Location = System::Drawing::Point(140, 64);
		this->label_LastTimeDisplay->Name = L"label_LastTimeDisplay";
		this->label_LastTimeDisplay->Size = System::Drawing::Size(16, 13);
		this->label_LastTimeDisplay->TabIndex = 7;
		this->label_LastTimeDisplay->Text = L"---";
		// 
		// label_LastTime
		// 
		this->label_LastTime->AutoSize = true;
		this->label_LastTime->Location = System::Drawing::Point(40, 64);
		this->label_LastTime->Name = L"label_LastTime";
		this->label_LastTime->Size = System::Drawing::Size(94, 13);
		this->label_LastTime->TabIndex = 5;
		this->label_LastTime->Text = L"Previous 1/1024s:";
		// 
		// label_CurrentTimeDisplay
		// 
		this->label_CurrentTimeDisplay->AutoSize = true;
		this->label_CurrentTimeDisplay->Location = System::Drawing::Point(140, 51);
		this->label_CurrentTimeDisplay->Name = L"label_CurrentTimeDisplay";
		this->label_CurrentTimeDisplay->Size = System::Drawing::Size(16, 13);
		this->label_CurrentTimeDisplay->TabIndex = 8;
		this->label_CurrentTimeDisplay->Text = L"---";
		// 
		// label_CurrentTime
		// 
		this->label_CurrentTime->AutoSize = true;
		this->label_CurrentTime->Location = System::Drawing::Point(47, 51);
		this->label_CurrentTime->Name = L"label_CurrentTime";
		this->label_CurrentTime->Size = System::Drawing::Size(87, 13);
		this->label_CurrentTime->TabIndex = 6;
		this->label_CurrentTime->Text = L"Current 1/1024s:";
		// 
		// label_EventCount
		// 
		this->label_EventCount->AutoSize = true;
		this->label_EventCount->Location = System::Drawing::Point(140, 38);
		this->label_EventCount->Name = L"label_EventCount";
		this->label_EventCount->Size = System::Drawing::Size(16, 13);
		this->label_EventCount->TabIndex = 2;
		this->label_EventCount->Text = L"---";
		// 
		// label_TxEventCount
		// 
		this->label_TxEventCount->AutoSize = true;
		this->label_TxEventCount->Location = System::Drawing::Point(22, 38);
		this->label_TxEventCount->Name = L"label_TxEventCount";
		this->label_TxEventCount->Size = System::Drawing::Size(112, 13);
		this->label_TxEventCount->TabIndex = 0;
		this->label_TxEventCount->Text = L"Transmit Event Count:";
		// 
		// label_TranslatedDisplayLabel
		// 
		this->label_TranslatedDisplayLabel->AutoSize = true;
		this->label_TranslatedDisplayLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Underline, 
			System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
		this->label_TranslatedDisplayLabel->Location = System::Drawing::Point(2, 2);
		this->label_TranslatedDisplayLabel->Name = L"label_TranslatedDisplayLabel";
		this->label_TranslatedDisplayLabel->Size = System::Drawing::Size(139, 13);
		this->label_TranslatedDisplayLabel->TabIndex = 4;
		this->label_TranslatedDisplayLabel->Text = L"Current HRM Transmission :";
		// 
		// label_PulseTxd
		// 
		this->label_PulseTxd->AutoSize = true;
		this->label_PulseTxd->Location = System::Drawing::Point(140, 25);
		this->label_PulseTxd->Name = L"label_PulseTxd";
		this->label_PulseTxd->Size = System::Drawing::Size(16, 13);
		this->label_PulseTxd->TabIndex = 3;
		this->label_PulseTxd->Text = L"---";
		// 
		// label_TxPulseOf
		// 
		this->label_TxPulseOf->AutoSize = true;
		this->label_TxPulseOf->Location = System::Drawing::Point(26, 25);
		this->label_TxPulseOf->Name = L"label_TxPulseOf";
		this->label_TxPulseOf->Size = System::Drawing::Size(108, 13);
		this->label_TxPulseOf->TabIndex = 1;
		this->label_TxPulseOf->Text = L"Transmitting Pulse of:";
		// 
		// HRMSensor
		// 
		this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
		this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
		this->ClientSize = System::Drawing::Size(794, 351);
		this->Controls->Add(this->panel_Display);
		this->Controls->Add(this->panel_Settings);
		this->Name = L"HRMSensor";
		this->Text = L"HRMSensor";
		this->panel_Settings->ResumeLayout(false);
		this->tabControl_Settings->ResumeLayout(false);
		this->tabPage_Parameters->ResumeLayout(false);
		this->tabPage_Parameters->PerformLayout();
		(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Prm_MaxPulse))->EndInit();
		(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Prm_MinPulse))->EndInit();
		(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Prm_CurPulse))->EndInit();
		this->tabPage_Advanced->ResumeLayout(false);
		this->tabPage_Advanced->PerformLayout();
		this->panel_Display->ResumeLayout(false);
		this->panel_Display->PerformLayout();
		this->ResumeLayout(false);

	}
#pragma endregion


};