/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/
 

#pragma once
#include "stdafx.h"
#include "ISimBase.h"
#include "antPlus_Geocache.h"
#include "antplus_common.h"				// Include common pages
#include "types.h"
#include "antdefines.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

public ref class GeocacheSensor : public System::Windows::Forms::Form, public ISimBase{
public:

		GeocacheSensor(System::Timers::Timer^ channelTimer, dRequestAckMsg^ channelAckMsg,  dRequestUpdateMesgPeriod^ channelUpdateMesgPeriod){
			InitializeComponent();
			GeocacheData = gcnew Geocache();
			commonData = gcnew CommonData();
			requestAckMsg = channelAckMsg;
			requestUpdateMesgPeriod = channelUpdateMesgPeriod;
			timerHandle = channelTimer;		
			InitializeSim();
		}

		~GeocacheSensor(){
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


public:
	virtual void onTimerTock(USHORT eventTime);							// Called every simulator event
	virtual void ANT_eventNotification(UCHAR ucEventCode_, UCHAR* pucEventBuffer_);	 // Channel event state machine
	virtual UCHAR getDeviceType(){return GeocacheData->DEVICE_TYPE;}
	virtual UCHAR getTransmissionType(){return GeocacheData->TX_TYPE_SENSOR;}
	virtual USHORT getTransmitPeriod(){return GeocacheData->MESG_P5HZ_PERIOD;}
	virtual DOUBLE getTimerInterval(){return (double) ulTimerInterval;}
	virtual System::Windows::Forms::Panel^ getSimSettingsPanel(){return this->panel_Settings;}
	virtual System::Windows::Forms::Panel^ getSimTranslatedDisplay(){return this->panel_Display;}


private:
	void InitializeSim();							// Initialize simulation data variables
	void HandleTransmit(UCHAR* pucTxBuffer_);		// Handles the data page transmissions 
	void HandleReceive(UCHAR* pucRxBuffer_);		// Handles page requests 
	void Authentication(UCHAR* pucRxBuffer_);		// Handle Authentication request
	void UpdateDisplay();
	void UpdateDisplayAckStatus(UCHAR status_);
	void UpdateBatStatus();
	System::Void checkBoxID_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void textBoxID_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e);
	System::Void textBoxID_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);
	System::Void checkBoxPIN_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void textBoxPIN_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e);
	System::Void textBoxPIN_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);
	System::Void checkBoxLatitude_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBoxLongitude_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBoxHint_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBoxLoggedVisits_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDownNumVisits_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDownLongitude_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDownLatitude_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void button_Glb_GlobalDataUpdate_Click(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_Bat_Status_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_Bat_Voltage_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_Bat_VoltInt_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_Bat_VoltFrac_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void comboBoxBatStatus_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void button_Bat_ElpTimeUpdate_Click(System::Object^  sender, System::EventArgs^  e);
	System::Void radioButton_Bat_Elp2Units_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void radioButton_Bat_Elp16Units_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDownDay_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDownMonth_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDownYear_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDownHours_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDownMinutes_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDownSeconds_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void buttonProgram_Click(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_NoSerial_CheckedChanged(System::Object^  sender, System::EventArgs^  e);

private:
	dRequestAckMsg^ requestAckMsg;
	dRequestUpdateMesgPeriod^ requestUpdateMesgPeriod;
	Geocache^ GeocacheData;                       // Geocache class variable
	CommonData^ commonData;
	System::Timers::Timer^ timerHandle;	// Timer handle
	ULONG  ulTimerInterval;				   // Timer interval between simulated events
	ULONG ulTotalTime;                  // used to track the on time for the batt page  

private: System::Windows::Forms::Panel^  panel_Settings;
private: System::Windows::Forms::Panel^  panel_Display;
private: System::Windows::Forms::Label^  label3;
private: System::Windows::Forms::Label^  labelLon;
private: System::Windows::Forms::Label^  label28;
private: System::Windows::Forms::Label^  label29;
private: System::Windows::Forms::Label^  label30;
private: System::Windows::Forms::Label^  label31;
private: System::Windows::Forms::Label^  label32;
private: System::Windows::Forms::Label^  label33;
private: System::Windows::Forms::Label^  label34;
private: System::Windows::Forms::TabControl^  tabControl1;
private: System::Windows::Forms::TabPage^  tabPage1;
private: System::Windows::Forms::GroupBox^  groupBox2;
private: System::Windows::Forms::TextBox^  textBoxPIN;
private: System::Windows::Forms::Label^  label2;
private: System::Windows::Forms::GroupBox^  groupBox1;
private: System::Windows::Forms::TextBox^  textBoxID;
private: System::Windows::Forms::Label^  label1;
private: System::Windows::Forms::TabPage^  tabPage3;
private: System::Windows::Forms::CheckBox^  checkBoxLongitude;
private: System::Windows::Forms::CheckBox^  checkBoxLatitude;
private: System::Windows::Forms::NumericUpDown^  numericUpDownLongitude;
private: System::Windows::Forms::NumericUpDown^  numericUpDownLatitude;
private: System::Windows::Forms::TabPage^  tabPage4;
private: System::Windows::Forms::TabControl^  tabControl3;
private: System::Windows::Forms::TabPage^  tabPage11;
private: System::Windows::Forms::TabPage^  tabPage12;
private: System::Windows::Forms::TabPage^  tabPage13;
private: System::Windows::Forms::TabPage^  tabPage14;
private: System::Windows::Forms::CheckBox^  checkBoxHint;
private: System::Windows::Forms::TextBox^  textBoxHint;
private: System::Windows::Forms::Label^  label13;
private: System::Windows::Forms::NumericUpDown^  numericUpDownNumVisits;
private: System::Windows::Forms::NumericUpDown^  numericUpDownMonth;
private: System::Windows::Forms::NumericUpDown^  numericUpDownSeconds;
private: System::Windows::Forms::NumericUpDown^  numericUpDownYear;
private: System::Windows::Forms::Label^  label7;
private: System::Windows::Forms::Label^  label8;
private: System::Windows::Forms::CheckBox^  checkBoxLoggedVisits;
private: System::Windows::Forms::Label^  label9;
private: System::Windows::Forms::NumericUpDown^  numericUpDownHours;
private: System::Windows::Forms::Label^  label10;
private: System::Windows::Forms::NumericUpDown^  numericUpDownDay;
private: System::Windows::Forms::Label^  label11;
private: System::Windows::Forms::Label^  label12;
private: System::Windows::Forms::NumericUpDown^  numericUpDownMinutes;
private: System::Windows::Forms::Label^  label35;
private: System::Windows::Forms::TabPage^  tabPage2;
private: System::Windows::Forms::Label^  labelAuthToken;
private: System::Windows::Forms::Label^  label36;
private: System::Windows::Forms::TabControl^  tabControl2;
private: System::Windows::Forms::TabPage^  tabPage5;
private: System::Windows::Forms::Button^  button_Glb_GlobalDataUpdate;
private: System::Windows::Forms::TextBox^  textBox_Glb_HardwareVerChange;
private: System::Windows::Forms::TextBox^  textBox_Glb_SoftwareVerChange;
private: System::Windows::Forms::TextBox^  textBox_Glb_ModelNumChange;
private: System::Windows::Forms::TextBox^  textBox_Glb_ManfIDChange;
private: System::Windows::Forms::TextBox^  textBox_Glb_SerialNumChange;
private: System::Windows::Forms::Label^  label_Glb_HardwareVer;
private: System::Windows::Forms::Label^  label_Glb_SoftwareVer;
private: System::Windows::Forms::Label^  label_Glb_ManfID;
private: System::Windows::Forms::Label^  label_Glb_ModelNum;
private: System::Windows::Forms::Label^  label_Glb_SerialNum;
private: System::Windows::Forms::TabPage^  tabPage6;
private: System::Windows::Forms::Label^  label19;
private: System::Windows::Forms::Label^  label20;
private: System::Windows::Forms::ComboBox^  comboBoxBatStatus;
private: System::Windows::Forms::Button^  button_Bat_ElpTimeUpdate;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Bat_VoltInt;
private: System::Windows::Forms::Label^  label_Voltage_Display;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Bat_VoltFrac;
private: System::Windows::Forms::GroupBox^  groupBox_Resol;
private: System::Windows::Forms::RadioButton^  radioButton_Bat_Elp2Units;
private: System::Windows::Forms::RadioButton^  radioButton_Bat_Elp16Units;
private: System::Windows::Forms::CheckBox^  checkBox_Bat_Voltage;
private: System::Windows::Forms::TextBox^  textBox_Bat_ElpTimeChange;
private: System::Windows::Forms::Label^  label_Bat_ElpTimeDisplay;
private: System::Windows::Forms::Label^  label_Bat_ElpTime;
private: System::Windows::Forms::CheckBox^  checkBox_Bat_Status;
private: System::Windows::Forms::Label^  labelTotNumPages;
private: System::Windows::Forms::Label^  label22;
private: System::Windows::Forms::GroupBox^  groupBox3;
private: System::Windows::Forms::GroupBox^  groupBox4;
private: System::Windows::Forms::GroupBox^  groupBox5;
private: System::Windows::Forms::GroupBox^  groupBox6;
private: System::Windows::Forms::TabPage^  tabPage10;
private: System::Windows::Forms::Button^  buttonProgram;
private: System::Windows::Forms::Label^  label15;
private: System::Windows::Forms::Label^  labelLat;
private: System::Windows::Forms::Label^  label14;
private: System::Windows::Forms::Label^  labelTimestamp;
private: System::Windows::Forms::Label^  label4;
private: System::Windows::Forms::Label^  labelNumVisits;
private: System::Windows::Forms::Label^  label5;
private: System::Windows::Forms::Label^  label16;
private: System::Windows::Forms::Label^  labelHint;
private: System::Windows::Forms::Label^  label21;
private: System::Windows::Forms::Label^  labelPIN;
private: System::Windows::Forms::Label^  label17;
private: System::Windows::Forms::Label^  labelID;
private: System::Windows::Forms::CheckBox^  checkBoxPIN;
private: System::Windows::Forms::CheckBox^  checkBoxID;
private: System::Windows::Forms::CheckBox^  checkBox_NoSerial;
private: System::Windows::Forms::Label^  labelError;

		 /// <summary>
		/// Required designer variable.a
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
			this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
			this->tabPage1 = (gcnew System::Windows::Forms::TabPage());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->checkBoxPIN = (gcnew System::Windows::Forms::CheckBox());
			this->labelTotNumPages = (gcnew System::Windows::Forms::Label());
			this->textBoxPIN = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label22 = (gcnew System::Windows::Forms::Label());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->checkBoxID = (gcnew System::Windows::Forms::CheckBox());
			this->textBoxID = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->tabPage3 = (gcnew System::Windows::Forms::TabPage());
			this->tabControl3 = (gcnew System::Windows::Forms::TabControl());
			this->tabPage10 = (gcnew System::Windows::Forms::TabPage());
			this->buttonProgram = (gcnew System::Windows::Forms::Button());
			this->tabPage11 = (gcnew System::Windows::Forms::TabPage());
			this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
			this->checkBoxLatitude = (gcnew System::Windows::Forms::CheckBox());
			this->numericUpDownLatitude = (gcnew System::Windows::Forms::NumericUpDown());
			this->tabPage12 = (gcnew System::Windows::Forms::TabPage());
			this->groupBox4 = (gcnew System::Windows::Forms::GroupBox());
			this->checkBoxLongitude = (gcnew System::Windows::Forms::CheckBox());
			this->numericUpDownLongitude = (gcnew System::Windows::Forms::NumericUpDown());
			this->tabPage13 = (gcnew System::Windows::Forms::TabPage());
			this->groupBox5 = (gcnew System::Windows::Forms::GroupBox());
			this->textBoxHint = (gcnew System::Windows::Forms::TextBox());
			this->checkBoxHint = (gcnew System::Windows::Forms::CheckBox());
			this->tabPage14 = (gcnew System::Windows::Forms::TabPage());
			this->groupBox6 = (gcnew System::Windows::Forms::GroupBox());
			this->label35 = (gcnew System::Windows::Forms::Label());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->label13 = (gcnew System::Windows::Forms::Label());
			this->numericUpDownMinutes = (gcnew System::Windows::Forms::NumericUpDown());
			this->numericUpDownNumVisits = (gcnew System::Windows::Forms::NumericUpDown());
			this->label12 = (gcnew System::Windows::Forms::Label());
			this->numericUpDownMonth = (gcnew System::Windows::Forms::NumericUpDown());
			this->label11 = (gcnew System::Windows::Forms::Label());
			this->numericUpDownSeconds = (gcnew System::Windows::Forms::NumericUpDown());
			this->numericUpDownDay = (gcnew System::Windows::Forms::NumericUpDown());
			this->numericUpDownYear = (gcnew System::Windows::Forms::NumericUpDown());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->numericUpDownHours = (gcnew System::Windows::Forms::NumericUpDown());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->checkBoxLoggedVisits = (gcnew System::Windows::Forms::CheckBox());
			this->tabPage2 = (gcnew System::Windows::Forms::TabPage());
			this->labelAuthToken = (gcnew System::Windows::Forms::Label());
			this->label36 = (gcnew System::Windows::Forms::Label());
			this->tabPage4 = (gcnew System::Windows::Forms::TabPage());
			this->tabControl2 = (gcnew System::Windows::Forms::TabControl());
			this->tabPage5 = (gcnew System::Windows::Forms::TabPage());
			this->checkBox_NoSerial = (gcnew System::Windows::Forms::CheckBox());
			this->button_Glb_GlobalDataUpdate = (gcnew System::Windows::Forms::Button());
			this->textBox_Glb_HardwareVerChange = (gcnew System::Windows::Forms::TextBox());
			this->textBox_Glb_SoftwareVerChange = (gcnew System::Windows::Forms::TextBox());
			this->textBox_Glb_ModelNumChange = (gcnew System::Windows::Forms::TextBox());
			this->textBox_Glb_ManfIDChange = (gcnew System::Windows::Forms::TextBox());
			this->textBox_Glb_SerialNumChange = (gcnew System::Windows::Forms::TextBox());
			this->label_Glb_HardwareVer = (gcnew System::Windows::Forms::Label());
			this->label_Glb_SoftwareVer = (gcnew System::Windows::Forms::Label());
			this->label_Glb_ManfID = (gcnew System::Windows::Forms::Label());
			this->label_Glb_ModelNum = (gcnew System::Windows::Forms::Label());
			this->label_Glb_SerialNum = (gcnew System::Windows::Forms::Label());
			this->tabPage6 = (gcnew System::Windows::Forms::TabPage());
			this->label19 = (gcnew System::Windows::Forms::Label());
			this->label20 = (gcnew System::Windows::Forms::Label());
			this->comboBoxBatStatus = (gcnew System::Windows::Forms::ComboBox());
			this->button_Bat_ElpTimeUpdate = (gcnew System::Windows::Forms::Button());
			this->numericUpDown_Bat_VoltInt = (gcnew System::Windows::Forms::NumericUpDown());
			this->label_Voltage_Display = (gcnew System::Windows::Forms::Label());
			this->numericUpDown_Bat_VoltFrac = (gcnew System::Windows::Forms::NumericUpDown());
			this->groupBox_Resol = (gcnew System::Windows::Forms::GroupBox());
			this->radioButton_Bat_Elp2Units = (gcnew System::Windows::Forms::RadioButton());
			this->radioButton_Bat_Elp16Units = (gcnew System::Windows::Forms::RadioButton());
			this->checkBox_Bat_Voltage = (gcnew System::Windows::Forms::CheckBox());
			this->textBox_Bat_ElpTimeChange = (gcnew System::Windows::Forms::TextBox());
			this->label_Bat_ElpTimeDisplay = (gcnew System::Windows::Forms::Label());
			this->label_Bat_ElpTime = (gcnew System::Windows::Forms::Label());
			this->checkBox_Bat_Status = (gcnew System::Windows::Forms::CheckBox());
			this->panel_Display = (gcnew System::Windows::Forms::Panel());
			this->labelHint = (gcnew System::Windows::Forms::Label());
			this->label21 = (gcnew System::Windows::Forms::Label());
			this->labelPIN = (gcnew System::Windows::Forms::Label());
			this->label17 = (gcnew System::Windows::Forms::Label());
			this->labelID = (gcnew System::Windows::Forms::Label());
			this->label16 = (gcnew System::Windows::Forms::Label());
			this->labelNumVisits = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->labelLon = (gcnew System::Windows::Forms::Label());
			this->label15 = (gcnew System::Windows::Forms::Label());
			this->labelLat = (gcnew System::Windows::Forms::Label());
			this->label14 = (gcnew System::Windows::Forms::Label());
			this->labelTimestamp = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label28 = (gcnew System::Windows::Forms::Label());
			this->label29 = (gcnew System::Windows::Forms::Label());
			this->label30 = (gcnew System::Windows::Forms::Label());
			this->label31 = (gcnew System::Windows::Forms::Label());
			this->label32 = (gcnew System::Windows::Forms::Label());
			this->label33 = (gcnew System::Windows::Forms::Label());
			this->label34 = (gcnew System::Windows::Forms::Label());
			this->labelError = (gcnew System::Windows::Forms::Label());
			this->panel_Settings->SuspendLayout();
			this->tabControl1->SuspendLayout();
			this->tabPage1->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->groupBox1->SuspendLayout();
			this->tabPage3->SuspendLayout();
			this->tabControl3->SuspendLayout();
			this->tabPage10->SuspendLayout();
			this->tabPage11->SuspendLayout();
			this->groupBox3->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownLatitude))->BeginInit();
			this->tabPage12->SuspendLayout();
			this->groupBox4->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownLongitude))->BeginInit();
			this->tabPage13->SuspendLayout();
			this->groupBox5->SuspendLayout();
			this->tabPage14->SuspendLayout();
			this->groupBox6->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownMinutes))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownNumVisits))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownMonth))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownSeconds))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownDay))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownYear))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownHours))->BeginInit();
			this->tabPage2->SuspendLayout();
			this->tabPage4->SuspendLayout();
			this->tabControl2->SuspendLayout();
			this->tabPage5->SuspendLayout();
			this->tabPage6->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Bat_VoltInt))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Bat_VoltFrac))->BeginInit();
			this->groupBox_Resol->SuspendLayout();
			this->panel_Display->SuspendLayout();
			this->SuspendLayout();
			// 
			// panel_Settings
			// 
			this->panel_Settings->Controls->Add(this->tabControl1);
			this->panel_Settings->Location = System::Drawing::Point(322, 40);
			this->panel_Settings->Name = L"panel_Settings";
			this->panel_Settings->Size = System::Drawing::Size(400, 166);
			this->panel_Settings->TabIndex = 0;
			// 
			// tabControl1
			// 
			this->tabControl1->Controls->Add(this->tabPage1);
			this->tabControl1->Controls->Add(this->tabPage3);
			this->tabControl1->Controls->Add(this->tabPage2);
			this->tabControl1->Controls->Add(this->tabPage4);
			this->tabControl1->Location = System::Drawing::Point(3, 3);
			this->tabControl1->Name = L"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(392, 160);
			this->tabControl1->TabIndex = 6;
			// 
			// tabPage1
			// 
			this->tabPage1->Controls->Add(this->groupBox2);
			this->tabPage1->Controls->Add(this->groupBox1);
			this->tabPage1->Location = System::Drawing::Point(4, 22);
			this->tabPage1->Name = L"tabPage1";
			this->tabPage1->Padding = System::Windows::Forms::Padding(3);
			this->tabPage1->Size = System::Drawing::Size(384, 134);
			this->tabPage1->TabIndex = 0;
			this->tabPage1->Text = L"Main Data";
			this->tabPage1->UseVisualStyleBackColor = true;
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->checkBoxPIN);
			this->groupBox2->Controls->Add(this->labelTotNumPages);
			this->groupBox2->Controls->Add(this->textBoxPIN);
			this->groupBox2->Controls->Add(this->label2);
			this->groupBox2->Controls->Add(this->label22);
			this->groupBox2->Location = System::Drawing::Point(196, 6);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(160, 98);
			this->groupBox2->TabIndex = 57;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Page 1 Data";
			// 
			// checkBoxPIN
			// 
			this->checkBoxPIN->AutoSize = true;
			this->checkBoxPIN->Location = System::Drawing::Point(17, 19);
			this->checkBoxPIN->Name = L"checkBoxPIN";
			this->checkBoxPIN->Size = System::Drawing::Size(69, 17);
			this->checkBoxPIN->TabIndex = 55;
			this->checkBoxPIN->Text = L"Set PIN :";
			this->checkBoxPIN->UseVisualStyleBackColor = true;
			this->checkBoxPIN->CheckedChanged += gcnew System::EventHandler(this, &GeocacheSensor::checkBoxPIN_CheckedChanged);
			// 
			// labelTotNumPages
			// 
			this->labelTotNumPages->AutoSize = true;
			this->labelTotNumPages->Location = System::Drawing::Point(132, 78);
			this->labelTotNumPages->Name = L"labelTotNumPages";
			this->labelTotNumPages->Size = System::Drawing::Size(16, 13);
			this->labelTotNumPages->TabIndex = 59;
			this->labelTotNumPages->Text = L"---";
			// 
			// textBoxPIN
			// 
			this->textBoxPIN->Cursor = System::Windows::Forms::Cursors::IBeam;
			this->textBoxPIN->Enabled = false;
			this->textBoxPIN->Location = System::Drawing::Point(64, 43);
			this->textBoxPIN->MaxLength = 10;
			this->textBoxPIN->Name = L"textBoxPIN";
			this->textBoxPIN->Size = System::Drawing::Size(76, 20);
			this->textBoxPIN->TabIndex = 54;
			this->textBoxPIN->Text = L"12345";
			this->textBoxPIN->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &GeocacheSensor::textBoxPIN_KeyDown);
			this->textBoxPIN->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &GeocacheSensor::textBoxPIN_KeyPress);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(16, 46);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(31, 13);
			this->label2->TabIndex = 4;
			this->label2->Text = L"PIN :";
			// 
			// label22
			// 
			this->label22->AutoSize = true;
			this->label22->Location = System::Drawing::Point(14, 78);
			this->label22->Name = L"label22";
			this->label22->Size = System::Drawing::Size(92, 13);
			this->label22->TabIndex = 58;
			this->label22->Text = L"Total # of Pages :";
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->checkBoxID);
			this->groupBox1->Controls->Add(this->textBoxID);
			this->groupBox1->Controls->Add(this->label1);
			this->groupBox1->Location = System::Drawing::Point(19, 6);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(160, 79);
			this->groupBox1->TabIndex = 56;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Page 0 Data";
			// 
			// checkBoxID
			// 
			this->checkBoxID->AutoSize = true;
			this->checkBoxID->Location = System::Drawing::Point(19, 19);
			this->checkBoxID->Name = L"checkBoxID";
			this->checkBoxID->Size = System::Drawing::Size(62, 17);
			this->checkBoxID->TabIndex = 21;
			this->checkBoxID->Text = L"Set ID :";
			this->checkBoxID->UseVisualStyleBackColor = true;
			this->checkBoxID->CheckedChanged += gcnew System::EventHandler(this, &GeocacheSensor::checkBoxID_CheckedChanged);
			// 
			// textBoxID
			// 
			this->textBoxID->CharacterCasing = System::Windows::Forms::CharacterCasing::Upper;
			this->textBoxID->Enabled = false;
			this->textBoxID->Location = System::Drawing::Point(64, 43);
			this->textBoxID->MaxLength = 9;
			this->textBoxID->Name = L"textBoxID";
			this->textBoxID->Size = System::Drawing::Size(76, 20);
			this->textBoxID->TabIndex = 54;
			this->textBoxID->Text = L"ABCD12345";
			this->textBoxID->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &GeocacheSensor::textBoxID_KeyDown);
			this->textBoxID->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &GeocacheSensor::textBoxID_KeyPress);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(16, 46);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(24, 13);
			this->label1->TabIndex = 4;
			this->label1->Text = L"ID :";
			// 
			// tabPage3
			// 
			this->tabPage3->Controls->Add(this->tabControl3);
			this->tabPage3->Location = System::Drawing::Point(4, 22);
			this->tabPage3->Name = L"tabPage3";
			this->tabPage3->Size = System::Drawing::Size(384, 134);
			this->tabPage3->TabIndex = 2;
			this->tabPage3->Text = L"Programmable Page(s)";
			this->tabPage3->UseVisualStyleBackColor = true;
			// 
			// tabControl3
			// 
			this->tabControl3->Controls->Add(this->tabPage10);
			this->tabControl3->Controls->Add(this->tabPage11);
			this->tabControl3->Controls->Add(this->tabPage12);
			this->tabControl3->Controls->Add(this->tabPage13);
			this->tabControl3->Controls->Add(this->tabPage14);
			this->tabControl3->Location = System::Drawing::Point(3, 3);
			this->tabControl3->Name = L"tabControl3";
			this->tabControl3->SelectedIndex = 0;
			this->tabControl3->Size = System::Drawing::Size(378, 131);
			this->tabControl3->TabIndex = 2;
			// 
			// tabPage10
			// 
			this->tabPage10->Controls->Add(this->buttonProgram);
			this->tabPage10->Location = System::Drawing::Point(4, 22);
			this->tabPage10->Name = L"tabPage10";
			this->tabPage10->Padding = System::Windows::Forms::Padding(3);
			this->tabPage10->Size = System::Drawing::Size(370, 105);
			this->tabPage10->TabIndex = 4;
			this->tabPage10->Text = L"Program";
			this->tabPage10->UseVisualStyleBackColor = true;
			// 
			// buttonProgram
			// 
			this->buttonProgram->Location = System::Drawing::Point(126, 35);
			this->buttonProgram->Name = L"buttonProgram";
			this->buttonProgram->Size = System::Drawing::Size(112, 23);
			this->buttonProgram->TabIndex = 0;
			this->buttonProgram->Text = L"Program Geocache";
			this->buttonProgram->UseVisualStyleBackColor = true;
			this->buttonProgram->Click += gcnew System::EventHandler(this, &GeocacheSensor::buttonProgram_Click);
			// 
			// tabPage11
			// 
			this->tabPage11->Controls->Add(this->groupBox3);
			this->tabPage11->Location = System::Drawing::Point(4, 22);
			this->tabPage11->Name = L"tabPage11";
			this->tabPage11->Padding = System::Windows::Forms::Padding(3);
			this->tabPage11->Size = System::Drawing::Size(370, 105);
			this->tabPage11->TabIndex = 0;
			this->tabPage11->Text = L"Lattitude";
			this->tabPage11->UseVisualStyleBackColor = true;
			// 
			// groupBox3
			// 
			this->groupBox3->Controls->Add(this->checkBoxLatitude);
			this->groupBox3->Controls->Add(this->numericUpDownLatitude);
			this->groupBox3->Location = System::Drawing::Point(6, 6);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(244, 79);
			this->groupBox3->TabIndex = 2;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = L"Data ID 0";
			// 
			// checkBoxLatitude
			// 
			this->checkBoxLatitude->AutoSize = true;
			this->checkBoxLatitude->Location = System::Drawing::Point(30, 19);
			this->checkBoxLatitude->Name = L"checkBoxLatitude";
			this->checkBoxLatitude->Size = System::Drawing::Size(89, 17);
			this->checkBoxLatitude->TabIndex = 20;
			this->checkBoxLatitude->Text = L"Set Latitude :";
			this->checkBoxLatitude->UseVisualStyleBackColor = true;
			this->checkBoxLatitude->CheckedChanged += gcnew System::EventHandler(this, &GeocacheSensor::checkBoxLatitude_CheckedChanged);
			// 
			// numericUpDownLatitude
			// 
			this->numericUpDownLatitude->DecimalPlaces = 6;
			this->numericUpDownLatitude->Enabled = false;
			this->numericUpDownLatitude->Location = System::Drawing::Point(73, 42);
			this->numericUpDownLatitude->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {90, 0, 0, 0});
			this->numericUpDownLatitude->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {90, 0, 0, System::Int32::MinValue});
			this->numericUpDownLatitude->Name = L"numericUpDownLatitude";
			this->numericUpDownLatitude->Size = System::Drawing::Size(99, 20);
			this->numericUpDownLatitude->TabIndex = 13;
			this->numericUpDownLatitude->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {513292, 0, 0, 262144});
			this->numericUpDownLatitude->ValueChanged += gcnew System::EventHandler(this, &GeocacheSensor::numericUpDownLatitude_ValueChanged);
			// 
			// tabPage12
			// 
			this->tabPage12->Controls->Add(this->groupBox4);
			this->tabPage12->Location = System::Drawing::Point(4, 22);
			this->tabPage12->Name = L"tabPage12";
			this->tabPage12->Padding = System::Windows::Forms::Padding(3);
			this->tabPage12->Size = System::Drawing::Size(370, 105);
			this->tabPage12->TabIndex = 1;
			this->tabPage12->Text = L"Longitude";
			this->tabPage12->UseVisualStyleBackColor = true;
			// 
			// groupBox4
			// 
			this->groupBox4->Controls->Add(this->checkBoxLongitude);
			this->groupBox4->Controls->Add(this->numericUpDownLongitude);
			this->groupBox4->Location = System::Drawing::Point(6, 6);
			this->groupBox4->Name = L"groupBox4";
			this->groupBox4->Size = System::Drawing::Size(244, 79);
			this->groupBox4->TabIndex = 3;
			this->groupBox4->TabStop = false;
			this->groupBox4->Text = L"Data ID 1";
			// 
			// checkBoxLongitude
			// 
			this->checkBoxLongitude->AutoSize = true;
			this->checkBoxLongitude->Location = System::Drawing::Point(29, 19);
			this->checkBoxLongitude->Name = L"checkBoxLongitude";
			this->checkBoxLongitude->Size = System::Drawing::Size(98, 17);
			this->checkBoxLongitude->TabIndex = 21;
			this->checkBoxLongitude->Text = L"Set Longitude :";
			this->checkBoxLongitude->UseVisualStyleBackColor = true;
			this->checkBoxLongitude->CheckedChanged += gcnew System::EventHandler(this, &GeocacheSensor::checkBoxLongitude_CheckedChanged);
			// 
			// numericUpDownLongitude
			// 
			this->numericUpDownLongitude->DecimalPlaces = 6;
			this->numericUpDownLongitude->Enabled = false;
			this->numericUpDownLongitude->Location = System::Drawing::Point(74, 42);
			this->numericUpDownLongitude->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {180, 0, 0, 0});
			this->numericUpDownLongitude->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {180, 0, 0, System::Int32::MinValue});
			this->numericUpDownLongitude->Name = L"numericUpDownLongitude";
			this->numericUpDownLongitude->Size = System::Drawing::Size(99, 20);
			this->numericUpDownLongitude->TabIndex = 14;
			this->numericUpDownLongitude->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1145493, 0, 0, -2147221504});
			this->numericUpDownLongitude->ValueChanged += gcnew System::EventHandler(this, &GeocacheSensor::numericUpDownLongitude_ValueChanged);
			// 
			// tabPage13
			// 
			this->tabPage13->Controls->Add(this->groupBox5);
			this->tabPage13->Location = System::Drawing::Point(4, 22);
			this->tabPage13->Name = L"tabPage13";
			this->tabPage13->Size = System::Drawing::Size(370, 105);
			this->tabPage13->TabIndex = 2;
			this->tabPage13->Text = L"Hint";
			this->tabPage13->UseVisualStyleBackColor = true;
			// 
			// groupBox5
			// 
			this->groupBox5->Controls->Add(this->textBoxHint);
			this->groupBox5->Controls->Add(this->checkBoxHint);
			this->groupBox5->Location = System::Drawing::Point(3, 3);
			this->groupBox5->Name = L"groupBox5";
			this->groupBox5->Size = System::Drawing::Size(364, 93);
			this->groupBox5->TabIndex = 4;
			this->groupBox5->TabStop = false;
			this->groupBox5->Text = L"Data ID 2";
			// 
			// textBoxHint
			// 
			this->textBoxHint->Enabled = false;
			this->textBoxHint->Location = System::Drawing::Point(6, 53);
			this->textBoxHint->MaxLength = 80;
			this->textBoxHint->Name = L"textBoxHint";
			this->textBoxHint->Size = System::Drawing::Size(352, 20);
			this->textBoxHint->TabIndex = 56;
			this->textBoxHint->Text = L"HINT HINT";
			// 
			// checkBoxHint
			// 
			this->checkBoxHint->AutoSize = true;
			this->checkBoxHint->Location = System::Drawing::Point(27, 19);
			this->checkBoxHint->Name = L"checkBoxHint";
			this->checkBoxHint->Size = System::Drawing::Size(70, 17);
			this->checkBoxHint->TabIndex = 57;
			this->checkBoxHint->Text = L"Set Hint :";
			this->checkBoxHint->UseVisualStyleBackColor = true;
			this->checkBoxHint->CheckedChanged += gcnew System::EventHandler(this, &GeocacheSensor::checkBoxHint_CheckedChanged);
			// 
			// tabPage14
			// 
			this->tabPage14->Controls->Add(this->groupBox6);
			this->tabPage14->Location = System::Drawing::Point(4, 22);
			this->tabPage14->Name = L"tabPage14";
			this->tabPage14->Size = System::Drawing::Size(370, 105);
			this->tabPage14->TabIndex = 3;
			this->tabPage14->Text = L"LoggedVisits";
			this->tabPage14->UseVisualStyleBackColor = true;
			// 
			// groupBox6
			// 
			this->groupBox6->Controls->Add(this->label35);
			this->groupBox6->Controls->Add(this->label7);
			this->groupBox6->Controls->Add(this->label13);
			this->groupBox6->Controls->Add(this->numericUpDownMinutes);
			this->groupBox6->Controls->Add(this->numericUpDownNumVisits);
			this->groupBox6->Controls->Add(this->label12);
			this->groupBox6->Controls->Add(this->numericUpDownMonth);
			this->groupBox6->Controls->Add(this->label11);
			this->groupBox6->Controls->Add(this->numericUpDownSeconds);
			this->groupBox6->Controls->Add(this->numericUpDownDay);
			this->groupBox6->Controls->Add(this->numericUpDownYear);
			this->groupBox6->Controls->Add(this->label10);
			this->groupBox6->Controls->Add(this->numericUpDownHours);
			this->groupBox6->Controls->Add(this->label8);
			this->groupBox6->Controls->Add(this->label9);
			this->groupBox6->Controls->Add(this->checkBoxLoggedVisits);
			this->groupBox6->Location = System::Drawing::Point(3, 4);
			this->groupBox6->Name = L"groupBox6";
			this->groupBox6->Size = System::Drawing::Size(364, 99);
			this->groupBox6->TabIndex = 2;
			this->groupBox6->TabStop = false;
			this->groupBox6->Text = L"Data ID 4";
			// 
			// label35
			// 
			this->label35->AutoSize = true;
			this->label35->Location = System::Drawing::Point(37, 72);
			this->label35->Name = L"label35";
			this->label35->Size = System::Drawing::Size(103, 13);
			this->label35->TabIndex = 33;
			this->label35->Text = L"Last Visit Timestamp";
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(164, 15);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(26, 13);
			this->label7->TabIndex = 23;
			this->label7->Text = L"Day";
			// 
			// label13
			// 
			this->label13->AutoSize = true;
			this->label13->Location = System::Drawing::Point(9, 43);
			this->label13->Name = L"label13";
			this->label13->Size = System::Drawing::Size(59, 13);
			this->label13->TabIndex = 32;
			this->label13->Text = L"# of Visits :";
			// 
			// numericUpDownMinutes
			// 
			this->numericUpDownMinutes->Enabled = false;
			this->numericUpDownMinutes->Location = System::Drawing::Point(316, 39);
			this->numericUpDownMinutes->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {59, 0, 0, 0});
			this->numericUpDownMinutes->Name = L"numericUpDownMinutes";
			this->numericUpDownMinutes->Size = System::Drawing::Size(37, 20);
			this->numericUpDownMinutes->TabIndex = 26;
			this->numericUpDownMinutes->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->numericUpDownMinutes->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {30, 0, 0, 0});
			// 
			// numericUpDownNumVisits
			// 
			this->numericUpDownNumVisits->Enabled = false;
			this->numericUpDownNumVisits->Location = System::Drawing::Point(74, 41);
			this->numericUpDownNumVisits->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {65535, 0, 0, 0});
			this->numericUpDownNumVisits->Name = L"numericUpDownNumVisits";
			this->numericUpDownNumVisits->Size = System::Drawing::Size(66, 20);
			this->numericUpDownNumVisits->TabIndex = 31;
			this->numericUpDownNumVisits->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->numericUpDownNumVisits->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			// 
			// label12
			// 
			this->label12->AutoSize = true;
			this->label12->Location = System::Drawing::Point(153, 41);
			this->label12->Name = L"label12";
			this->label12->Size = System::Drawing::Size(37, 13);
			this->label12->TabIndex = 22;
			this->label12->Text = L"Month";
			// 
			// numericUpDownMonth
			// 
			this->numericUpDownMonth->Enabled = false;
			this->numericUpDownMonth->Location = System::Drawing::Point(196, 39);
			this->numericUpDownMonth->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {12, 0, 0, 0});
			this->numericUpDownMonth->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->numericUpDownMonth->Name = L"numericUpDownMonth";
			this->numericUpDownMonth->Size = System::Drawing::Size(37, 20);
			this->numericUpDownMonth->TabIndex = 30;
			this->numericUpDownMonth->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->numericUpDownMonth->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {5, 0, 0, 0});
			// 
			// label11
			// 
			this->label11->AutoSize = true;
			this->label11->Location = System::Drawing::Point(161, 69);
			this->label11->Name = L"label11";
			this->label11->Size = System::Drawing::Size(29, 13);
			this->label11->TabIndex = 21;
			this->label11->Text = L"Year";
			// 
			// numericUpDownSeconds
			// 
			this->numericUpDownSeconds->Enabled = false;
			this->numericUpDownSeconds->Location = System::Drawing::Point(316, 65);
			this->numericUpDownSeconds->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {59, 0, 0, 0});
			this->numericUpDownSeconds->Name = L"numericUpDownSeconds";
			this->numericUpDownSeconds->Size = System::Drawing::Size(37, 20);
			this->numericUpDownSeconds->TabIndex = 25;
			this->numericUpDownSeconds->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->numericUpDownSeconds->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {30, 0, 0, 0});
			// 
			// numericUpDownDay
			// 
			this->numericUpDownDay->Enabled = false;
			this->numericUpDownDay->Location = System::Drawing::Point(196, 13);
			this->numericUpDownDay->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {31, 0, 0, 0});
			this->numericUpDownDay->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->numericUpDownDay->Name = L"numericUpDownDay";
			this->numericUpDownDay->Size = System::Drawing::Size(37, 20);
			this->numericUpDownDay->TabIndex = 28;
			this->numericUpDownDay->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->numericUpDownDay->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {13, 0, 0, 0});
			// 
			// numericUpDownYear
			// 
			this->numericUpDownYear->Enabled = false;
			this->numericUpDownYear->Location = System::Drawing::Point(196, 65);
			this->numericUpDownYear->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {3000, 0, 0, 0});
			this->numericUpDownYear->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1900, 0, 0, 0});
			this->numericUpDownYear->Name = L"numericUpDownYear";
			this->numericUpDownYear->Size = System::Drawing::Size(55, 20);
			this->numericUpDownYear->TabIndex = 29;
			this->numericUpDownYear->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->numericUpDownYear->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {2011, 0, 0, 0});
			// 
			// label10
			// 
			this->label10->AutoSize = true;
			this->label10->Location = System::Drawing::Point(275, 15);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(35, 13);
			this->label10->TabIndex = 20;
			this->label10->Text = L"Hours";
			// 
			// numericUpDownHours
			// 
			this->numericUpDownHours->Enabled = false;
			this->numericUpDownHours->Location = System::Drawing::Point(316, 13);
			this->numericUpDownHours->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {23, 0, 0, 0});
			this->numericUpDownHours->Name = L"numericUpDownHours";
			this->numericUpDownHours->Size = System::Drawing::Size(37, 20);
			this->numericUpDownHours->TabIndex = 27;
			this->numericUpDownHours->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->numericUpDownHours->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {12, 0, 0, 0});
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Location = System::Drawing::Point(266, 41);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(44, 13);
			this->label8->TabIndex = 19;
			this->label8->Text = L"Minutes";
			// 
			// label9
			// 
			this->label9->AutoSize = true;
			this->label9->Location = System::Drawing::Point(261, 69);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(49, 13);
			this->label9->TabIndex = 18;
			this->label9->Text = L"Seconds";
			// 
			// checkBoxLoggedVisits
			// 
			this->checkBoxLoggedVisits->AutoSize = true;
			this->checkBoxLoggedVisits->Location = System::Drawing::Point(15, 18);
			this->checkBoxLoggedVisits->Name = L"checkBoxLoggedVisits";
			this->checkBoxLoggedVisits->Size = System::Drawing::Size(114, 17);
			this->checkBoxLoggedVisits->TabIndex = 24;
			this->checkBoxLoggedVisits->Text = L"Set Logged Visits :";
			this->checkBoxLoggedVisits->UseVisualStyleBackColor = true;
			this->checkBoxLoggedVisits->CheckedChanged += gcnew System::EventHandler(this, &GeocacheSensor::checkBoxLoggedVisits_CheckedChanged);
			// 
			// tabPage2
			// 
			this->tabPage2->Controls->Add(this->labelAuthToken);
			this->tabPage2->Controls->Add(this->label36);
			this->tabPage2->Location = System::Drawing::Point(4, 22);
			this->tabPage2->Name = L"tabPage2";
			this->tabPage2->Padding = System::Windows::Forms::Padding(3);
			this->tabPage2->Size = System::Drawing::Size(384, 134);
			this->tabPage2->TabIndex = 4;
			this->tabPage2->Text = L"Authentication";
			this->tabPage2->UseVisualStyleBackColor = true;
			// 
			// labelAuthToken
			// 
			this->labelAuthToken->AutoSize = true;
			this->labelAuthToken->Location = System::Drawing::Point(143, 31);
			this->labelAuthToken->Name = L"labelAuthToken";
			this->labelAuthToken->Size = System::Drawing::Size(16, 13);
			this->labelAuthToken->TabIndex = 7;
			this->labelAuthToken->Text = L"---";
			// 
			// label36
			// 
			this->label36->AutoSize = true;
			this->label36->Location = System::Drawing::Point(25, 31);
			this->label36->Name = L"label36";
			this->label36->Size = System::Drawing::Size(115, 13);
			this->label36->TabIndex = 6;
			this->label36->Text = L"Authentication Token :";
			// 
			// tabPage4
			// 
			this->tabPage4->Controls->Add(this->tabControl2);
			this->tabPage4->Location = System::Drawing::Point(4, 22);
			this->tabPage4->Name = L"tabPage4";
			this->tabPage4->Size = System::Drawing::Size(384, 134);
			this->tabPage4->TabIndex = 3;
			this->tabPage4->Text = L"Common Data";
			this->tabPage4->UseVisualStyleBackColor = true;
			// 
			// tabControl2
			// 
			this->tabControl2->Controls->Add(this->tabPage5);
			this->tabControl2->Controls->Add(this->tabPage6);
			this->tabControl2->Location = System::Drawing::Point(0, 3);
			this->tabControl2->Name = L"tabControl2";
			this->tabControl2->SelectedIndex = 0;
			this->tabControl2->Size = System::Drawing::Size(388, 131);
			this->tabControl2->TabIndex = 0;
			// 
			// tabPage5
			// 
			this->tabPage5->Controls->Add(this->labelError);
			this->tabPage5->Controls->Add(this->checkBox_NoSerial);
			this->tabPage5->Controls->Add(this->button_Glb_GlobalDataUpdate);
			this->tabPage5->Controls->Add(this->textBox_Glb_HardwareVerChange);
			this->tabPage5->Controls->Add(this->textBox_Glb_SoftwareVerChange);
			this->tabPage5->Controls->Add(this->textBox_Glb_ModelNumChange);
			this->tabPage5->Controls->Add(this->textBox_Glb_ManfIDChange);
			this->tabPage5->Controls->Add(this->textBox_Glb_SerialNumChange);
			this->tabPage5->Controls->Add(this->label_Glb_HardwareVer);
			this->tabPage5->Controls->Add(this->label_Glb_SoftwareVer);
			this->tabPage5->Controls->Add(this->label_Glb_ManfID);
			this->tabPage5->Controls->Add(this->label_Glb_ModelNum);
			this->tabPage5->Controls->Add(this->label_Glb_SerialNum);
			this->tabPage5->Location = System::Drawing::Point(4, 22);
			this->tabPage5->Name = L"tabPage5";
			this->tabPage5->Padding = System::Windows::Forms::Padding(3);
			this->tabPage5->Size = System::Drawing::Size(380, 105);
			this->tabPage5->TabIndex = 0;
			this->tabPage5->Text = L"Manufacturer\'s Info";
			this->tabPage5->UseVisualStyleBackColor = true;
			// 
			// checkBox_NoSerial
			// 
			this->checkBox_NoSerial->AutoSize = true;
			this->checkBox_NoSerial->Location = System::Drawing::Point(149, 29);
			this->checkBox_NoSerial->Name = L"checkBox_NoSerial";
			this->checkBox_NoSerial->Size = System::Drawing::Size(79, 17);
			this->checkBox_NoSerial->TabIndex = 60;
			this->checkBox_NoSerial->Text = L"No Serial #";
			this->checkBox_NoSerial->UseVisualStyleBackColor = true;
			this->checkBox_NoSerial->CheckedChanged += gcnew System::EventHandler(this, &GeocacheSensor::checkBox_NoSerial_CheckedChanged);
			// 
			// button_Glb_GlobalDataUpdate
			// 
			this->button_Glb_GlobalDataUpdate->Location = System::Drawing::Point(247, 79);
			this->button_Glb_GlobalDataUpdate->Name = L"button_Glb_GlobalDataUpdate";
			this->button_Glb_GlobalDataUpdate->Size = System::Drawing::Size(97, 20);
			this->button_Glb_GlobalDataUpdate->TabIndex = 59;
			this->button_Glb_GlobalDataUpdate->Text = L"Update All";
			this->button_Glb_GlobalDataUpdate->UseVisualStyleBackColor = true;
			this->button_Glb_GlobalDataUpdate->Click += gcnew System::EventHandler(this, &GeocacheSensor::button_Glb_GlobalDataUpdate_Click);
			// 
			// textBox_Glb_HardwareVerChange
			// 
			this->textBox_Glb_HardwareVerChange->Location = System::Drawing::Point(321, 27);
			this->textBox_Glb_HardwareVerChange->MaxLength = 3;
			this->textBox_Glb_HardwareVerChange->Name = L"textBox_Glb_HardwareVerChange";
			this->textBox_Glb_HardwareVerChange->Size = System::Drawing::Size(29, 20);
			this->textBox_Glb_HardwareVerChange->TabIndex = 57;
			this->textBox_Glb_HardwareVerChange->Text = L"1";
			// 
			// textBox_Glb_SoftwareVerChange
			// 
			this->textBox_Glb_SoftwareVerChange->Location = System::Drawing::Point(321, 53);
			this->textBox_Glb_SoftwareVerChange->MaxLength = 3;
			this->textBox_Glb_SoftwareVerChange->Name = L"textBox_Glb_SoftwareVerChange";
			this->textBox_Glb_SoftwareVerChange->Size = System::Drawing::Size(29, 20);
			this->textBox_Glb_SoftwareVerChange->TabIndex = 58;
			this->textBox_Glb_SoftwareVerChange->Text = L"1";
			// 
			// textBox_Glb_ModelNumChange
			// 
			this->textBox_Glb_ModelNumChange->Location = System::Drawing::Point(94, 79);
			this->textBox_Glb_ModelNumChange->MaxLength = 5;
			this->textBox_Glb_ModelNumChange->Name = L"textBox_Glb_ModelNumChange";
			this->textBox_Glb_ModelNumChange->Size = System::Drawing::Size(49, 20);
			this->textBox_Glb_ModelNumChange->TabIndex = 52;
			this->textBox_Glb_ModelNumChange->Text = L"33669";
			// 
			// textBox_Glb_ManfIDChange
			// 
			this->textBox_Glb_ManfIDChange->Location = System::Drawing::Point(94, 53);
			this->textBox_Glb_ManfIDChange->MaxLength = 5;
			this->textBox_Glb_ManfIDChange->Name = L"textBox_Glb_ManfIDChange";
			this->textBox_Glb_ManfIDChange->Size = System::Drawing::Size(49, 20);
			this->textBox_Glb_ManfIDChange->TabIndex = 51;
			this->textBox_Glb_ManfIDChange->Text = L"2";
			// 
			// textBox_Glb_SerialNumChange
			// 
			this->textBox_Glb_SerialNumChange->Location = System::Drawing::Point(67, 27);
			this->textBox_Glb_SerialNumChange->MaxLength = 10;
			this->textBox_Glb_SerialNumChange->Name = L"textBox_Glb_SerialNumChange";
			this->textBox_Glb_SerialNumChange->Size = System::Drawing::Size(76, 20);
			this->textBox_Glb_SerialNumChange->TabIndex = 53;
			this->textBox_Glb_SerialNumChange->Text = L"1234567890";
			// 
			// label_Glb_HardwareVer
			// 
			this->label_Glb_HardwareVer->AutoSize = true;
			this->label_Glb_HardwareVer->Location = System::Drawing::Point(240, 29);
			this->label_Glb_HardwareVer->Name = L"label_Glb_HardwareVer";
			this->label_Glb_HardwareVer->Size = System::Drawing::Size(75, 13);
			this->label_Glb_HardwareVer->TabIndex = 54;
			this->label_Glb_HardwareVer->Text = L"Hardware Ver:";
			// 
			// label_Glb_SoftwareVer
			// 
			this->label_Glb_SoftwareVer->AutoSize = true;
			this->label_Glb_SoftwareVer->Location = System::Drawing::Point(244, 56);
			this->label_Glb_SoftwareVer->Name = L"label_Glb_SoftwareVer";
			this->label_Glb_SoftwareVer->Size = System::Drawing::Size(71, 13);
			this->label_Glb_SoftwareVer->TabIndex = 55;
			this->label_Glb_SoftwareVer->Text = L"Software Ver:";
			// 
			// label_Glb_ManfID
			// 
			this->label_Glb_ManfID->AutoSize = true;
			this->label_Glb_ManfID->Location = System::Drawing::Point(6, 56);
			this->label_Glb_ManfID->Name = L"label_Glb_ManfID";
			this->label_Glb_ManfID->Size = System::Drawing::Size(51, 13);
			this->label_Glb_ManfID->TabIndex = 48;
			this->label_Glb_ManfID->Text = L"Manf. ID:";
			// 
			// label_Glb_ModelNum
			// 
			this->label_Glb_ModelNum->AutoSize = true;
			this->label_Glb_ModelNum->Location = System::Drawing::Point(8, 82);
			this->label_Glb_ModelNum->Name = L"label_Glb_ModelNum";
			this->label_Glb_ModelNum->Size = System::Drawing::Size(49, 13);
			this->label_Glb_ModelNum->TabIndex = 50;
			this->label_Glb_ModelNum->Text = L"Model #:";
			// 
			// label_Glb_SerialNum
			// 
			this->label_Glb_SerialNum->AutoSize = true;
			this->label_Glb_SerialNum->Location = System::Drawing::Point(6, 30);
			this->label_Glb_SerialNum->Name = L"label_Glb_SerialNum";
			this->label_Glb_SerialNum->Size = System::Drawing::Size(46, 13);
			this->label_Glb_SerialNum->TabIndex = 49;
			this->label_Glb_SerialNum->Text = L"Serial #:";
			// 
			// tabPage6
			// 
			this->tabPage6->Controls->Add(this->label19);
			this->tabPage6->Controls->Add(this->label20);
			this->tabPage6->Controls->Add(this->comboBoxBatStatus);
			this->tabPage6->Controls->Add(this->button_Bat_ElpTimeUpdate);
			this->tabPage6->Controls->Add(this->numericUpDown_Bat_VoltInt);
			this->tabPage6->Controls->Add(this->label_Voltage_Display);
			this->tabPage6->Controls->Add(this->numericUpDown_Bat_VoltFrac);
			this->tabPage6->Controls->Add(this->groupBox_Resol);
			this->tabPage6->Controls->Add(this->checkBox_Bat_Voltage);
			this->tabPage6->Controls->Add(this->textBox_Bat_ElpTimeChange);
			this->tabPage6->Controls->Add(this->label_Bat_ElpTimeDisplay);
			this->tabPage6->Controls->Add(this->label_Bat_ElpTime);
			this->tabPage6->Controls->Add(this->checkBox_Bat_Status);
			this->tabPage6->Location = System::Drawing::Point(4, 22);
			this->tabPage6->Name = L"tabPage6";
			this->tabPage6->Padding = System::Windows::Forms::Padding(3);
			this->tabPage6->Size = System::Drawing::Size(380, 105);
			this->tabPage6->TabIndex = 1;
			this->tabPage6->Text = L"Battery Page";
			this->tabPage6->UseVisualStyleBackColor = true;
			// 
			// label19
			// 
			this->label19->AutoSize = true;
			this->label19->Location = System::Drawing::Point(65, 27);
			this->label19->Name = L"label19";
			this->label19->Size = System::Drawing::Size(73, 13);
			this->label19->TabIndex = 79;
			this->label19->Text = L"Battery Status";
			// 
			// label20
			// 
			this->label20->AutoSize = true;
			this->label20->Location = System::Drawing::Point(13, 81);
			this->label20->Name = L"label20";
			this->label20->Size = System::Drawing::Size(143, 13);
			this->label20->TabIndex = 78;
			this->label20->Text = L"Fractional Voltage (1/256 V):";
			// 
			// comboBoxBatStatus
			// 
			this->comboBoxBatStatus->FormattingEnabled = true;
			this->comboBoxBatStatus->Items->AddRange(gcnew cli::array< System::Object^  >(6) {L"New", L"Good", L"Ok", L"Low", L"Critical", 
				L"Invalid"});
			this->comboBoxBatStatus->Location = System::Drawing::Point(144, 24);
			this->comboBoxBatStatus->Name = L"comboBoxBatStatus";
			this->comboBoxBatStatus->Size = System::Drawing::Size(60, 21);
			this->comboBoxBatStatus->TabIndex = 76;
			this->comboBoxBatStatus->Text = L"Ok";
			this->comboBoxBatStatus->SelectedIndexChanged += gcnew System::EventHandler(this, &GeocacheSensor::comboBoxBatStatus_SelectedIndexChanged);
			// 
			// button_Bat_ElpTimeUpdate
			// 
			this->button_Bat_ElpTimeUpdate->Location = System::Drawing::Point(221, 77);
			this->button_Bat_ElpTimeUpdate->Name = L"button_Bat_ElpTimeUpdate";
			this->button_Bat_ElpTimeUpdate->Size = System::Drawing::Size(84, 20);
			this->button_Bat_ElpTimeUpdate->TabIndex = 73;
			this->button_Bat_ElpTimeUpdate->Text = L"Update Time";
			this->button_Bat_ElpTimeUpdate->UseVisualStyleBackColor = true;
			this->button_Bat_ElpTimeUpdate->Click += gcnew System::EventHandler(this, &GeocacheSensor::button_Bat_ElpTimeUpdate_Click);
			// 
			// numericUpDown_Bat_VoltInt
			// 
			this->numericUpDown_Bat_VoltInt->Location = System::Drawing::Point(168, 52);
			this->numericUpDown_Bat_VoltInt->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {14, 0, 0, 0});
			this->numericUpDown_Bat_VoltInt->Name = L"numericUpDown_Bat_VoltInt";
			this->numericUpDown_Bat_VoltInt->Size = System::Drawing::Size(36, 20);
			this->numericUpDown_Bat_VoltInt->TabIndex = 68;
			this->numericUpDown_Bat_VoltInt->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {3, 0, 0, 0});
			this->numericUpDown_Bat_VoltInt->ValueChanged += gcnew System::EventHandler(this, &GeocacheSensor::numericUpDown_Bat_VoltInt_ValueChanged);
			// 
			// label_Voltage_Display
			// 
			this->label_Voltage_Display->AutoSize = true;
			this->label_Voltage_Display->Location = System::Drawing::Point(186, 7);
			this->label_Voltage_Display->Name = L"label_Voltage_Display";
			this->label_Voltage_Display->Size = System::Drawing::Size(0, 13);
			this->label_Voltage_Display->TabIndex = 75;
			// 
			// numericUpDown_Bat_VoltFrac
			// 
			this->numericUpDown_Bat_VoltFrac->Location = System::Drawing::Point(162, 79);
			this->numericUpDown_Bat_VoltFrac->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
			this->numericUpDown_Bat_VoltFrac->Name = L"numericUpDown_Bat_VoltFrac";
			this->numericUpDown_Bat_VoltFrac->Size = System::Drawing::Size(42, 20);
			this->numericUpDown_Bat_VoltFrac->TabIndex = 69;
			this->numericUpDown_Bat_VoltFrac->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {85, 0, 0, 0});
			this->numericUpDown_Bat_VoltFrac->ValueChanged += gcnew System::EventHandler(this, &GeocacheSensor::numericUpDown_Bat_VoltFrac_ValueChanged);
			// 
			// groupBox_Resol
			// 
			this->groupBox_Resol->Controls->Add(this->radioButton_Bat_Elp2Units);
			this->groupBox_Resol->Controls->Add(this->radioButton_Bat_Elp16Units);
			this->groupBox_Resol->Enabled = false;
			this->groupBox_Resol->Location = System::Drawing::Point(239, 27);
			this->groupBox_Resol->Name = L"groupBox_Resol";
			this->groupBox_Resol->Size = System::Drawing::Size(124, 36);
			this->groupBox_Resol->TabIndex = 63;
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
			this->radioButton_Bat_Elp2Units->CheckedChanged += gcnew System::EventHandler(this, &GeocacheSensor::radioButton_Bat_Elp2Units_CheckedChanged);
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
			this->radioButton_Bat_Elp16Units->CheckedChanged += gcnew System::EventHandler(this, &GeocacheSensor::radioButton_Bat_Elp16Units_CheckedChanged);
			// 
			// checkBox_Bat_Voltage
			// 
			this->checkBox_Bat_Voltage->AutoSize = true;
			this->checkBox_Bat_Voltage->Checked = true;
			this->checkBox_Bat_Voltage->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBox_Bat_Voltage->Location = System::Drawing::Point(6, 53);
			this->checkBox_Bat_Voltage->Name = L"checkBox_Bat_Voltage";
			this->checkBox_Bat_Voltage->Size = System::Drawing::Size(153, 17);
			this->checkBox_Bat_Voltage->TabIndex = 70;
			this->checkBox_Bat_Voltage->Text = L"Enable Battery Voltage (V):";
			this->checkBox_Bat_Voltage->UseVisualStyleBackColor = true;
			this->checkBox_Bat_Voltage->CheckedChanged += gcnew System::EventHandler(this, &GeocacheSensor::checkBox_Bat_Voltage_CheckedChanged);
			// 
			// textBox_Bat_ElpTimeChange
			// 
			this->textBox_Bat_ElpTimeChange->Location = System::Drawing::Point(311, 77);
			this->textBox_Bat_ElpTimeChange->MaxLength = 9;
			this->textBox_Bat_ElpTimeChange->Name = L"textBox_Bat_ElpTimeChange";
			this->textBox_Bat_ElpTimeChange->Size = System::Drawing::Size(63, 20);
			this->textBox_Bat_ElpTimeChange->TabIndex = 72;
			this->textBox_Bat_ElpTimeChange->Text = L"0";
			this->textBox_Bat_ElpTimeChange->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			// 
			// label_Bat_ElpTimeDisplay
			// 
			this->label_Bat_ElpTimeDisplay->Enabled = false;
			this->label_Bat_ElpTimeDisplay->Location = System::Drawing::Point(312, 8);
			this->label_Bat_ElpTimeDisplay->Name = L"label_Bat_ElpTimeDisplay";
			this->label_Bat_ElpTimeDisplay->Size = System::Drawing::Size(62, 13);
			this->label_Bat_ElpTimeDisplay->TabIndex = 65;
			this->label_Bat_ElpTimeDisplay->Text = L"0";
			this->label_Bat_ElpTimeDisplay->TextAlign = System::Drawing::ContentAlignment::BottomRight;
			// 
			// label_Bat_ElpTime
			// 
			this->label_Bat_ElpTime->AutoSize = true;
			this->label_Bat_ElpTime->Location = System::Drawing::Point(229, 8);
			this->label_Bat_ElpTime->Name = L"label_Bat_ElpTime";
			this->label_Bat_ElpTime->Size = System::Drawing::Size(88, 13);
			this->label_Bat_ElpTime->TabIndex = 64;
			this->label_Bat_ElpTime->Text = L"Elapsed Time (s):";
			// 
			// checkBox_Bat_Status
			// 
			this->checkBox_Bat_Status->AutoSize = true;
			this->checkBox_Bat_Status->Checked = true;
			this->checkBox_Bat_Status->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBox_Bat_Status->Location = System::Drawing::Point(3, 3);
			this->checkBox_Bat_Status->Name = L"checkBox_Bat_Status";
			this->checkBox_Bat_Status->Size = System::Drawing::Size(156, 17);
			this->checkBox_Bat_Status->TabIndex = 67;
			this->checkBox_Bat_Status->Text = L"Enable Battery Status Page";
			this->checkBox_Bat_Status->UseVisualStyleBackColor = true;
			this->checkBox_Bat_Status->CheckedChanged += gcnew System::EventHandler(this, &GeocacheSensor::checkBox_Bat_Status_CheckedChanged);
			// 
			// panel_Display
			// 
			this->panel_Display->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->panel_Display->Controls->Add(this->labelHint);
			this->panel_Display->Controls->Add(this->label21);
			this->panel_Display->Controls->Add(this->labelPIN);
			this->panel_Display->Controls->Add(this->label17);
			this->panel_Display->Controls->Add(this->labelID);
			this->panel_Display->Controls->Add(this->label16);
			this->panel_Display->Controls->Add(this->labelNumVisits);
			this->panel_Display->Controls->Add(this->label5);
			this->panel_Display->Controls->Add(this->labelLon);
			this->panel_Display->Controls->Add(this->label15);
			this->panel_Display->Controls->Add(this->labelLat);
			this->panel_Display->Controls->Add(this->label14);
			this->panel_Display->Controls->Add(this->labelTimestamp);
			this->panel_Display->Controls->Add(this->label4);
			this->panel_Display->Controls->Add(this->label3);
			this->panel_Display->Location = System::Drawing::Point(58, 188);
			this->panel_Display->Name = L"panel_Display";
			this->panel_Display->Size = System::Drawing::Size(200, 90);
			this->panel_Display->TabIndex = 1;
			// 
			// labelHint
			// 
			this->labelHint->AutoSize = true;
			this->labelHint->Location = System::Drawing::Point(36, 73);
			this->labelHint->Name = L"labelHint";
			this->labelHint->Size = System::Drawing::Size(16, 13);
			this->labelHint->TabIndex = 16;
			this->labelHint->Text = L"---";
			// 
			// label21
			// 
			this->label21->AutoSize = true;
			this->label21->Location = System::Drawing::Point(3, 73);
			this->label21->Name = L"label21";
			this->label21->Size = System::Drawing::Size(35, 13);
			this->label21->TabIndex = 15;
			this->label21->Text = L"Hint : ";
			// 
			// labelPIN
			// 
			this->labelPIN->AutoSize = true;
			this->labelPIN->Location = System::Drawing::Point(36, 28);
			this->labelPIN->Name = L"labelPIN";
			this->labelPIN->Size = System::Drawing::Size(16, 13);
			this->labelPIN->TabIndex = 14;
			this->labelPIN->Text = L"---";
			// 
			// label17
			// 
			this->label17->AutoSize = true;
			this->label17->Location = System::Drawing::Point(3, 28);
			this->label17->Name = L"label17";
			this->label17->Size = System::Drawing::Size(34, 13);
			this->label17->TabIndex = 13;
			this->label17->Text = L"PIN : ";
			// 
			// labelID
			// 
			this->labelID->AutoSize = true;
			this->labelID->Location = System::Drawing::Point(36, 15);
			this->labelID->Name = L"labelID";
			this->labelID->Size = System::Drawing::Size(16, 13);
			this->labelID->TabIndex = 12;
			this->labelID->Text = L"---";
			// 
			// label16
			// 
			this->label16->AutoSize = true;
			this->label16->Location = System::Drawing::Point(3, 15);
			this->label16->Name = L"label16";
			this->label16->Size = System::Drawing::Size(27, 13);
			this->label16->TabIndex = 12;
			this->label16->Text = L"ID : ";
			// 
			// labelNumVisits
			// 
			this->labelNumVisits->AutoSize = true;
			this->labelNumVisits->Location = System::Drawing::Point(64, 43);
			this->labelNumVisits->Name = L"labelNumVisits";
			this->labelNumVisits->Size = System::Drawing::Size(16, 13);
			this->labelNumVisits->TabIndex = 11;
			this->labelNumVisits->Text = L"---";
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(3, 43);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(62, 13);
			this->label5->TabIndex = 2;
			this->label5->Text = L"# of Visits : ";
			// 
			// labelLon
			// 
			this->labelLon->AutoSize = true;
			this->labelLon->Location = System::Drawing::Point(142, 39);
			this->labelLon->Name = L"labelLon";
			this->labelLon->Size = System::Drawing::Size(16, 13);
			this->labelLon->TabIndex = 10;
			this->labelLon->Text = L"---";
			// 
			// label15
			// 
			this->label15->AutoSize = true;
			this->label15->Location = System::Drawing::Point(111, 39);
			this->label15->Name = L"label15";
			this->label15->Size = System::Drawing::Size(31, 13);
			this->label15->TabIndex = 9;
			this->label15->Text = L"Lon :";
			// 
			// labelLat
			// 
			this->labelLat->AutoSize = true;
			this->labelLat->Location = System::Drawing::Point(142, 26);
			this->labelLat->Name = L"labelLat";
			this->labelLat->Size = System::Drawing::Size(16, 13);
			this->labelLat->TabIndex = 8;
			this->labelLat->Text = L"---";
			// 
			// label14
			// 
			this->label14->AutoSize = true;
			this->label14->Location = System::Drawing::Point(111, 26);
			this->label14->Name = L"label14";
			this->label14->Size = System::Drawing::Size(28, 13);
			this->label14->TabIndex = 7;
			this->label14->Text = L"Lat :";
			// 
			// labelTimestamp
			// 
			this->labelTimestamp->AutoSize = true;
			this->labelTimestamp->Location = System::Drawing::Point(36, 58);
			this->labelTimestamp->Name = L"labelTimestamp";
			this->labelTimestamp->Size = System::Drawing::Size(16, 13);
			this->labelTimestamp->TabIndex = 4;
			this->labelTimestamp->Text = L"---";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(3, 58);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(39, 13);
			this->label4->TabIndex = 1;
			this->label4->Text = L"Time : ";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Underline, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label3->Location = System::Drawing::Point(3, 0);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(145, 13);
			this->label3->TabIndex = 0;
			this->label3->Text = L"Current Geocache Contents :";
			// 
			// label28
			// 
			this->label28->AutoSize = true;
			this->label28->Location = System::Drawing::Point(3, 58);
			this->label28->Name = L"label28";
			this->label28->Size = System::Drawing::Size(70, 13);
			this->label28->TabIndex = 6;
			this->label28->Text = L"Day of Week";
			// 
			// label29
			// 
			this->label29->AutoSize = true;
			this->label29->Location = System::Drawing::Point(166, 34);
			this->label29->Name = L"label29";
			this->label29->Size = System::Drawing::Size(26, 13);
			this->label29->TabIndex = 5;
			this->label29->Text = L"Day";
			// 
			// label30
			// 
			this->label30->AutoSize = true;
			this->label30->Location = System::Drawing::Point(199, 40);
			this->label30->Name = L"label30";
			this->label30->Size = System::Drawing::Size(37, 13);
			this->label30->TabIndex = 4;
			this->label30->Text = L"Month";
			// 
			// label31
			// 
			this->label31->AutoSize = true;
			this->label31->Location = System::Drawing::Point(271, 40);
			this->label31->Name = L"label31";
			this->label31->Size = System::Drawing::Size(29, 13);
			this->label31->TabIndex = 3;
			this->label31->Text = L"Year";
			// 
			// label32
			// 
			this->label32->AutoSize = true;
			this->label32->Location = System::Drawing::Point(3, 21);
			this->label32->Name = L"label32";
			this->label32->Size = System::Drawing::Size(35, 13);
			this->label32->TabIndex = 2;
			this->label32->Text = L"Hours";
			// 
			// label33
			// 
			this->label33->AutoSize = true;
			this->label33->Location = System::Drawing::Point(74, 21);
			this->label33->Name = L"label33";
			this->label33->Size = System::Drawing::Size(44, 13);
			this->label33->TabIndex = 1;
			this->label33->Text = L"Minutes";
			// 
			// label34
			// 
			this->label34->AutoSize = true;
			this->label34->Location = System::Drawing::Point(213, 5);
			this->label34->Name = L"label34";
			this->label34->Size = System::Drawing::Size(49, 13);
			this->label34->TabIndex = 0;
			this->label34->Text = L"Seconds";
			// 
			// labelError
			// 
			this->labelError->AutoSize = true;
			this->labelError->ForeColor = System::Drawing::Color::Red;
			this->labelError->Location = System::Drawing::Point(149, 9);
			this->labelError->Name = L"labelError";
			this->labelError->Size = System::Drawing::Size(90, 13);
			this->labelError->TabIndex = 61;
			this->labelError->Text = L"Error: Invalid data";
			this->labelError->Visible = false;
			// 
			// GeocacheSensor
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(794, 351);
			this->Controls->Add(this->panel_Display);
			this->Controls->Add(this->panel_Settings);
			this->Name = L"GeocacheSensor";
			this->Text = L"Geocache Sensor";
			this->panel_Settings->ResumeLayout(false);
			this->tabControl1->ResumeLayout(false);
			this->tabPage1->ResumeLayout(false);
			this->groupBox2->ResumeLayout(false);
			this->groupBox2->PerformLayout();
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->tabPage3->ResumeLayout(false);
			this->tabControl3->ResumeLayout(false);
			this->tabPage10->ResumeLayout(false);
			this->tabPage11->ResumeLayout(false);
			this->groupBox3->ResumeLayout(false);
			this->groupBox3->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownLatitude))->EndInit();
			this->tabPage12->ResumeLayout(false);
			this->groupBox4->ResumeLayout(false);
			this->groupBox4->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownLongitude))->EndInit();
			this->tabPage13->ResumeLayout(false);
			this->groupBox5->ResumeLayout(false);
			this->groupBox5->PerformLayout();
			this->tabPage14->ResumeLayout(false);
			this->groupBox6->ResumeLayout(false);
			this->groupBox6->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownMinutes))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownNumVisits))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownMonth))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownSeconds))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownDay))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownYear))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownHours))->EndInit();
			this->tabPage2->ResumeLayout(false);
			this->tabPage2->PerformLayout();
			this->tabPage4->ResumeLayout(false);
			this->tabControl2->ResumeLayout(false);
			this->tabPage5->ResumeLayout(false);
			this->tabPage5->PerformLayout();
			this->tabPage6->ResumeLayout(false);
			this->tabPage6->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Bat_VoltInt))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Bat_VoltFrac))->EndInit();
			this->groupBox_Resol->ResumeLayout(false);
			this->groupBox_Resol->PerformLayout();
			this->panel_Display->ResumeLayout(false);
			this->panel_Display->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
};