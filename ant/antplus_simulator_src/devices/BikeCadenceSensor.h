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
#include "bc.h"
#include "types.h"
#include "antdefines.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

public ref class BikeCadenceSensor : public System::Windows::Forms::Form, public ISimBase{
public:
	BikeCadenceSensor(System::Timers::Timer^ channelTimer){
		InitializeComponent();
		timerHandle = channelTimer;		// Get timer handle
		InitializeSim();	

	}

	~BikeCadenceSensor(){
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
	virtual UCHAR getDeviceType(){return BC_DEVICE_TYPE;}
	virtual UCHAR getTransmissionType(){return BC_TX_TYPE;}
	virtual USHORT getTransmitPeriod(){return BC_MSG_PERIOD;}
	virtual DOUBLE getTimerInterval(){return (double) ulTimerInterval;}
	virtual System::Windows::Forms::Panel^ getSimSettingsPanel(){return this->panel_Settings;}
	virtual System::Windows::Forms::Panel^ getSimTranslatedDisplay(){return this->panel_Display;}

private:
	void InitializeSim();
	void HandleTransmit(UCHAR* pucTxBuffer_);
	void UpdateDisplay();
	void ForceUpdate();
	System::Void numericUpDown_Sim_CurCadence_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_Sim_MinMaxCadence_ValueChanged(System::Object^  sender, System::EventArgs^  e);
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
	USHORT usEventCount;			// Bike cadence (pedal revolutions) event count
	UCHAR ucMinCadence;				// Minimum bike cadence (rpm)
	UCHAR ucCurCadence;				// Current bike cadence (rpm)
	UCHAR ucMaxCadence;				// Maximum bike cadence (rpm)
	ULONG ulElapsedTime2;			// Cumulative operating time (elapsed time) in 2 second resolution
	USHORT usTime1024;				// Time of last bike cadence event (1/1024 seconds)
	UCHAR ucBackgroundCount;		// To send multiple times the same extended message
	UCHAR ucNextBackgroundPage;		// Next extended page to send

	// Common Pages
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
private: System::Windows::Forms::Label^  label_CurrentCadence;
private: System::Windows::Forms::Label^  label_ElapsedSecsDisplay;
private: System::Windows::Forms::Label^  label_ElpTime;
private: System::Windows::Forms::Label^  label_HardwareVer;
private: System::Windows::Forms::Label^  label_ManfID;
private: System::Windows::Forms::Label^  label_MaxCadence;
private: System::Windows::Forms::Label^  label_MinCadence;
private: System::Windows::Forms::Label^  label_ModelNum;
private: System::Windows::Forms::Label^  label_SerialNum;
private: System::Windows::Forms::Label^  label_Sim_Cadence;
private: System::Windows::Forms::Label^  label_Sim_SimType;
private: System::Windows::Forms::Label^  label_SoftwareVer;
private: System::Windows::Forms::Label^  label_Trn_EventCount;
private: System::Windows::Forms::Label^  label_Trn_EventCountDisplay;
private: System::Windows::Forms::Label^  label_Trn_Time;
private: System::Windows::Forms::Label^  label_Trn_TimeDisplay;
private: System::Windows::Forms::Label^  label_Trn_TranslatedDisplayLabel;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Sim_CurrentCadence;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Sim_MaxCadence;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Sim_MinCadence;
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
		this->numericUpDown_Sim_MaxCadence = (gcnew System::Windows::Forms::NumericUpDown());
		this->numericUpDown_Sim_MinCadence = (gcnew System::Windows::Forms::NumericUpDown());
		this->numericUpDown_Sim_CurrentCadence = (gcnew System::Windows::Forms::NumericUpDown());
		this->label_MaxCadence = (gcnew System::Windows::Forms::Label());
		this->label_CurrentCadence = (gcnew System::Windows::Forms::Label());
		this->label_MinCadence = (gcnew System::Windows::Forms::Label());
		this->label_Sim_SimType = (gcnew System::Windows::Forms::Label());
		this->radioButton_Sim_Sweep = (gcnew System::Windows::Forms::RadioButton());
		this->radioButton_Sim_Fixed = (gcnew System::Windows::Forms::RadioButton());
		this->label_Sim_Cadence = (gcnew System::Windows::Forms::Label());
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
		(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_MaxCadence))->BeginInit();
		(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_MinCadence))->BeginInit();
		(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_CurrentCadence))->BeginInit();
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
		this->tabPage_Simulation->Controls->Add(this->numericUpDown_Sim_MaxCadence);
		this->tabPage_Simulation->Controls->Add(this->numericUpDown_Sim_MinCadence);
		this->tabPage_Simulation->Controls->Add(this->numericUpDown_Sim_CurrentCadence);
		this->tabPage_Simulation->Controls->Add(this->label_MaxCadence);
		this->tabPage_Simulation->Controls->Add(this->label_CurrentCadence);
		this->tabPage_Simulation->Controls->Add(this->label_MinCadence);
		this->tabPage_Simulation->Controls->Add(this->label_Sim_SimType);
		this->tabPage_Simulation->Controls->Add(this->radioButton_Sim_Sweep);
		this->tabPage_Simulation->Controls->Add(this->radioButton_Sim_Fixed);
		this->tabPage_Simulation->Controls->Add(this->label_Sim_Cadence);
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
		this->checkBox_SendBasicPage->Location = System::Drawing::Point(258, 76);
		this->checkBox_SendBasicPage->Name = L"checkBox_SendBasicPage";
		this->checkBox_SendBasicPage->Size = System::Drawing::Size(112, 17);
		this->checkBox_SendBasicPage->TabIndex = 62;
		this->checkBox_SendBasicPage->Text = L"Minimum Data Set";
		this->checkBox_SendBasicPage->UseVisualStyleBackColor = true;
		this->checkBox_SendBasicPage->CheckedChanged += gcnew System::EventHandler(this, &BikeCadenceSensor::checkBox_SendBasicPage_CheckedChanged);
		// 
		// checkBox_Legacy
		// 
		this->checkBox_Legacy->AutoSize = true;
		this->checkBox_Legacy->Location = System::Drawing::Point(180, 76);
		this->checkBox_Legacy->Name = L"checkBox_Legacy";
		this->checkBox_Legacy->Size = System::Drawing::Size(61, 17);
		this->checkBox_Legacy->TabIndex = 19;
		this->checkBox_Legacy->Text = L"Legacy";
		this->checkBox_Legacy->UseVisualStyleBackColor = true;
		this->checkBox_Legacy->CheckedChanged += gcnew System::EventHandler(this, &BikeCadenceSensor::checkBox_Legacy_CheckedChanged);
		// 
		// numericUpDown_Sim_MaxCadence
		// 
		this->numericUpDown_Sim_MaxCadence->Enabled = false;
		this->numericUpDown_Sim_MaxCadence->Location = System::Drawing::Point(78, 77);
		this->numericUpDown_Sim_MaxCadence->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
		this->numericUpDown_Sim_MaxCadence->Name = L"numericUpDown_Sim_MaxCadence";
		this->numericUpDown_Sim_MaxCadence->Size = System::Drawing::Size(45, 20);
		this->numericUpDown_Sim_MaxCadence->TabIndex = 18;
		this->numericUpDown_Sim_MaxCadence->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {120, 0, 0, 0});
		this->numericUpDown_Sim_MaxCadence->ValueChanged += gcnew System::EventHandler(this, &BikeCadenceSensor::numericUpDown_Sim_MinMaxCadence_ValueChanged);
		// 
		// numericUpDown_Sim_MinCadence
		// 
		this->numericUpDown_Sim_MinCadence->Enabled = false;
		this->numericUpDown_Sim_MinCadence->Location = System::Drawing::Point(78, 26);
		this->numericUpDown_Sim_MinCadence->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
		this->numericUpDown_Sim_MinCadence->Name = L"numericUpDown_Sim_MinCadence";
		this->numericUpDown_Sim_MinCadence->Size = System::Drawing::Size(45, 20);
		this->numericUpDown_Sim_MinCadence->TabIndex = 16;
		this->numericUpDown_Sim_MinCadence->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {60, 0, 0, 0});
		this->numericUpDown_Sim_MinCadence->ValueChanged += gcnew System::EventHandler(this, &BikeCadenceSensor::numericUpDown_Sim_MinMaxCadence_ValueChanged);
		// 
		// numericUpDown_Sim_CurrentCadence
		// 
		this->numericUpDown_Sim_CurrentCadence->Location = System::Drawing::Point(78, 51);
		this->numericUpDown_Sim_CurrentCadence->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
		this->numericUpDown_Sim_CurrentCadence->Name = L"numericUpDown_Sim_CurrentCadence";
		this->numericUpDown_Sim_CurrentCadence->Size = System::Drawing::Size(45, 20);
		this->numericUpDown_Sim_CurrentCadence->TabIndex = 15;
		this->numericUpDown_Sim_CurrentCadence->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {90, 0, 0, 0});
		this->numericUpDown_Sim_CurrentCadence->ValueChanged += gcnew System::EventHandler(this, &BikeCadenceSensor::numericUpDown_Sim_CurCadence_ValueChanged);
		// 
		// label_MaxCadence
		// 
		this->label_MaxCadence->AutoSize = true;
		this->label_MaxCadence->Location = System::Drawing::Point(47, 80);
		this->label_MaxCadence->Name = L"label_MaxCadence";
		this->label_MaxCadence->Size = System::Drawing::Size(27, 13);
		this->label_MaxCadence->TabIndex = 17;
		this->label_MaxCadence->Text = L"Max";
		// 
		// label_CurrentCadence
		// 
		this->label_CurrentCadence->AutoSize = true;
		this->label_CurrentCadence->Location = System::Drawing::Point(33, 54);
		this->label_CurrentCadence->Name = L"label_CurrentCadence";
		this->label_CurrentCadence->Size = System::Drawing::Size(41, 13);
		this->label_CurrentCadence->TabIndex = 14;
		this->label_CurrentCadence->Text = L"Current";
		// 
		// label_MinCadence
		// 
		this->label_MinCadence->AutoSize = true;
		this->label_MinCadence->Location = System::Drawing::Point(50, 29);
		this->label_MinCadence->Name = L"label_MinCadence";
		this->label_MinCadence->Size = System::Drawing::Size(24, 13);
		this->label_MinCadence->TabIndex = 13;
		this->label_MinCadence->Text = L"Min";
		// 
		// label_Sim_SimType
		// 
		this->label_Sim_SimType->AutoSize = true;
		this->label_Sim_SimType->Location = System::Drawing::Point(177, 28);
		this->label_Sim_SimType->Name = L"label_Sim_SimType";
		this->label_Sim_SimType->Size = System::Drawing::Size(85, 13);
		this->label_Sim_SimType->TabIndex = 12;
		this->label_Sim_SimType->Text = L"Simulation Type:";
		this->label_Sim_SimType->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
		// 
		// radioButton_Sim_Sweep
		// 
		this->radioButton_Sim_Sweep->AutoSize = true;
		this->radioButton_Sim_Sweep->Location = System::Drawing::Point(268, 51);
		this->radioButton_Sim_Sweep->Name = L"radioButton_Sim_Sweep";
		this->radioButton_Sim_Sweep->Size = System::Drawing::Size(58, 17);
		this->radioButton_Sim_Sweep->TabIndex = 12;
		this->radioButton_Sim_Sweep->Text = L"Sweep";
		this->radioButton_Sim_Sweep->UseVisualStyleBackColor = true;
		this->radioButton_Sim_Sweep->CheckedChanged += gcnew System::EventHandler(this, &BikeCadenceSensor::radioButton_SimTypeChanged);
		// 
		// radioButton_Sim_Fixed
		// 
		this->radioButton_Sim_Fixed->AutoSize = true;
		this->radioButton_Sim_Fixed->Checked = true;
		this->radioButton_Sim_Fixed->Location = System::Drawing::Point(268, 28);
		this->radioButton_Sim_Fixed->Name = L"radioButton_Sim_Fixed";
		this->radioButton_Sim_Fixed->Size = System::Drawing::Size(50, 17);
		this->radioButton_Sim_Fixed->TabIndex = 11;
		this->radioButton_Sim_Fixed->TabStop = true;
		this->radioButton_Sim_Fixed->Text = L"Fixed";
		this->radioButton_Sim_Fixed->UseVisualStyleBackColor = true;
		// 
		// label_Sim_Cadence
		// 
		this->label_Sim_Cadence->AutoSize = true;
		this->label_Sim_Cadence->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Underline, 
			System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
		this->label_Sim_Cadence->Location = System::Drawing::Point(44, 10);
		this->label_Sim_Cadence->Name = L"label_Sim_Cadence";
		this->label_Sim_Cadence->Size = System::Drawing::Size(79, 13);
		this->label_Sim_Cadence->TabIndex = 0;
		this->label_Sim_Cadence->Text = L"Cadence (rpm):";
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
		this->button_UpdateTime->Click += gcnew System::EventHandler(this, &BikeCadenceSensor::button_UpdateTime_Click);
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
		this->button_AdvancedUpdate->Click += gcnew System::EventHandler(this, &BikeCadenceSensor::button_AdvancedUpdate_Click);
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
		this->label_Trn_TimeDisplay->Location = System::Drawing::Point(141, 53);
		this->label_Trn_TimeDisplay->Name = L"label_Trn_TimeDisplay";
		this->label_Trn_TimeDisplay->Size = System::Drawing::Size(16, 13);
		this->label_Trn_TimeDisplay->TabIndex = 6;
		this->label_Trn_TimeDisplay->Text = L"---";
		this->label_Trn_TimeDisplay->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
		// 
		// label_Trn_Time
		// 
		this->label_Trn_Time->AutoSize = true;
		this->label_Trn_Time->Location = System::Drawing::Point(22, 53);
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
		this->label_Trn_TranslatedDisplayLabel->Size = System::Drawing::Size(181, 13);
		this->label_Trn_TranslatedDisplayLabel->TabIndex = 4;
		this->label_Trn_TranslatedDisplayLabel->Text = L"Current Bike Cadence Transmission :";
		// 
		// label_Trn_EventCountDisplay
		// 
		this->label_Trn_EventCountDisplay->AutoSize = true;
		this->label_Trn_EventCountDisplay->Location = System::Drawing::Point(141, 30);
		this->label_Trn_EventCountDisplay->Name = L"label_Trn_EventCountDisplay";
		this->label_Trn_EventCountDisplay->Size = System::Drawing::Size(16, 13);
		this->label_Trn_EventCountDisplay->TabIndex = 2;
		this->label_Trn_EventCountDisplay->Text = L"---";
		this->label_Trn_EventCountDisplay->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
		// 
		// label_Trn_EventCount
		// 
		this->label_Trn_EventCount->AutoSize = true;
		this->label_Trn_EventCount->Location = System::Drawing::Point(66, 30);
		this->label_Trn_EventCount->Name = L"label_Trn_EventCount";
		this->label_Trn_EventCount->Size = System::Drawing::Size(69, 13);
		this->label_Trn_EventCount->TabIndex = 0;
		this->label_Trn_EventCount->Text = L"Event Count:";
		this->label_Trn_EventCount->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
		// 
		// BikeCadenceSensor
		// 
		this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
		this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
		this->ClientSize = System::Drawing::Size(794, 351);
		this->Controls->Add(this->panel_Display);
		this->Controls->Add(this->panel_Settings);
		this->Name = L"BikeCadenceSensor";
		this->Text = L"BikeCadenceSensor";
		this->panel_Settings->ResumeLayout(false);
		this->tabControl_Settings->ResumeLayout(false);
		this->tabPage_Simulation->ResumeLayout(false);
		this->tabPage_Simulation->PerformLayout();
		(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_MaxCadence))->EndInit();
		(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_MinCadence))->EndInit();
		(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_CurrentCadence))->EndInit();
		this->tabPage_GlobalData->ResumeLayout(false);
		this->tabPage_GlobalData->PerformLayout();
		this->panel_Display->ResumeLayout(false);
		this->panel_Display->PerformLayout();
		this->ResumeLayout(false);

	}
#pragma endregion

};