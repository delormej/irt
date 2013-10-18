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
#include "antPlus_MSM.h"
#include "antplus_common.h"				// Include common pages
#include "types.h"
#include "antdefines.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

public ref class MSMSensor : public System::Windows::Forms::Form, public ISimBase{
public:

		MSMSensor(System::Timers::Timer^ channelTimer, dRequestAckMsg^ channelAckMsg){
			InitializeComponent();
			msmData = gcnew MSM();
			commonData = gcnew CommonData();
			requestAckMsg = channelAckMsg;	
			timerHandle = channelTimer;		
			InitializeSim();
		}

		~MSMSensor(){
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
	virtual UCHAR getDeviceType(){return msmData->DEVICE_TYPE;}
	virtual UCHAR getTransmissionType(){return msmData->TX_TYPE;}
	virtual USHORT getTransmitPeriod(){return msmData->MSG_PERIOD;}
	virtual DOUBLE getTimerInterval(){return (double) ulTimerInterval;}
	virtual System::Windows::Forms::Panel^ getSimSettingsPanel(){return this->panel_Settings;}
	virtual System::Windows::Forms::Panel^ getSimTranslatedDisplay(){return this->panel_Display;}


private:
	void InitializeSim();						  // Initialize simulation data variables
	void HandleTransmit(UCHAR* pucTxBuffer_);     // Handles the data page transmissions 
	void UpdateDisplay();
	void HandleCalibration(UCHAR* pucEventBuffer_);
	void UpdateCalibDisplay();
	void UpdateDisplayAckStatus(UCHAR status_);
	void UpdateGpsFixType();
	void UpdateBatStatus();
	void UpdateDayOfWeek();
	void UpdateTxPattern();
	void UpdateCommonPattern();
	System::Void numericUpDownSpeed_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBoxTxSpeed_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBoxCalibrationSupport_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBoxModeStorageSupport_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBoxSecondaryData_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBoxPage3Enable_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDownLongitude_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDownLatitude_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDownElevation_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDownHeading_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void comboBoxGPSFix_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void button_Glb_GlobalDataUpdate_Click(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_Bat_Status_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_Bat_Voltage_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_Bat_VoltInt_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_Bat_VoltFrac_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void comboBoxBatStatus_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void button_Bat_ElpTimeUpdate_Click(System::Object^  sender, System::EventArgs^  e);
	System::Void radioButton_Bat_Elp2Units_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void radioButton_Bat_Elp16Units_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBoxEnableDateTime_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void comboBoxDayOfWeek_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDownDay_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDownMonth_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDownYear_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDownHours_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDownMinutes_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDownSeconds_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_InvalidSerial_CheckedChanged(System::Object^  sender, System::EventArgs^  e); 

private:
	dRequestAckMsg^ requestAckMsg;
	MSM^ msmData;                       // MSM class variable
	CommonData^ commonData;
	System::Timers::Timer^ timerHandle;	// Timer handle
	ULONG  ulTimerInterval;				   // Timer interval between simulated events
	MSM::CalibrationStatus eCalStatus;  // tracks the status of the calibration procedure
	ULONG ulTotalTime;                  // used to track the on time for the batt page  
	static System::Collections::Generic::List<UCHAR> ucTxPattern = gcnew System::Collections::Generic::List<UCHAR>;
	static System::Collections::Generic::List<UCHAR> ucCommonPattern = gcnew System::Collections::Generic::List<UCHAR>;

	// Simulation Variables
	double dbDispAcumTime;  // Cumulative time (s)
	double dbDispAcumDist;  // Cumulative distance (m)
	USHORT usLastEventTime; // used to help calculate distance on each event

private: System::Windows::Forms::NumericUpDown^  numericUpDownSpeed;
private: System::Windows::Forms::Label^  labelAcumDistance;
private: System::Windows::Forms::Label^  label1;
private: System::Windows::Forms::Label^  labelSpeed;
private: System::Windows::Forms::Label^  labelAcumTime;
private: System::Windows::Forms::Label^  label2;
private: System::Windows::Forms::Panel^  panel_Settings;
private: System::Windows::Forms::Panel^  panel_Display;
private: System::Windows::Forms::Label^  label3;
private: System::Windows::Forms::TabControl^  tabControl1;
private: System::Windows::Forms::TabPage^  tabPage1;
private: System::Windows::Forms::TabPage^  tabPage2;
private: System::Windows::Forms::Label^  label4;
private: System::Windows::Forms::Label^  labelTxDist;
private: System::Windows::Forms::Label^  labelTxSpeed;
private: System::Windows::Forms::Label^  labelTxTime;
private: System::Windows::Forms::Label^  label6;
private: System::Windows::Forms::Label^  label5;
private: System::Windows::Forms::CheckBox^  checkBoxTxSpeed;
private: System::Windows::Forms::Label^  labelCalibrationProgress;
private: System::Windows::Forms::CheckBox^  checkBoxCalibrationSupport;
private: System::Windows::Forms::Label^  label7;
private: System::Windows::Forms::CheckBox^  checkBoxModeStorageSupport;
private: System::Windows::Forms::Label^  labelCurrentScaleFactor;
private: System::Windows::Forms::Label^  labelCurrentMode;
private: System::Windows::Forms::Label^  label8;
private: System::Windows::Forms::TabPage^  tabPage3;
private: System::Windows::Forms::GroupBox^  groupBoxPage3;
private: System::Windows::Forms::Label^  label11;
private: System::Windows::Forms::NumericUpDown^  numericUpDownElevation;
private: System::Windows::Forms::Label^  label10;
private: System::Windows::Forms::Label^  label9;
private: System::Windows::Forms::NumericUpDown^  numericUpDownLongitude;
private: System::Windows::Forms::NumericUpDown^  numericUpDownLatitude;
private: System::Windows::Forms::ComboBox^  comboBoxGPSFix;
private: System::Windows::Forms::Label^  label12;
private: System::Windows::Forms::NumericUpDown^  numericUpDownHeading;
private: System::Windows::Forms::Label^  label13;
private: System::Windows::Forms::CheckBox^  checkBoxPage3Enable;
private: System::Windows::Forms::CheckBox^  checkBoxSecondaryData;
private: System::Windows::Forms::Label^  labelTxLat;
private: System::Windows::Forms::Label^  label14;
private: System::Windows::Forms::Label^  label15;
private: System::Windows::Forms::Label^  lableTxLon;
private: System::Windows::Forms::Label^  label18;
private: System::Windows::Forms::Label^  label17;
private: System::Windows::Forms::Label^  label16;
private: System::Windows::Forms::Label^  labelTxFix;
private: System::Windows::Forms::Label^  labelTxHeading;
private: System::Windows::Forms::Label^  labelTxElev;
private: System::Windows::Forms::TabPage^  tabPage4;
private: System::Windows::Forms::TabControl^  tabControl2;
private: System::Windows::Forms::TabPage^  tabPage5;
private: System::Windows::Forms::TabPage^  tabPage6;
private: System::Windows::Forms::TextBox^  textBox_Glb_ModelNumChange;
private: System::Windows::Forms::Label^  label_Glb_ManfID;
private: System::Windows::Forms::TextBox^  textBox_Glb_ManfIDChange;
private: System::Windows::Forms::Label^  label_Glb_ModelNum;
private: System::Windows::Forms::TextBox^  textBox_Glb_SerialNumChange;
private: System::Windows::Forms::Label^  label_Glb_SerialNum;
private: System::Windows::Forms::Button^  button_Glb_GlobalDataUpdate;
private: System::Windows::Forms::TextBox^  textBox_Glb_HardwareVerChange;
private: System::Windows::Forms::Label^  label_Glb_HardwareVer;
private: System::Windows::Forms::Label^  label_Glb_SoftwareVer;
private: System::Windows::Forms::TextBox^  textBox_Glb_SoftwareVerChange;
private: System::Windows::Forms::GroupBox^  groupBox_Resol;
private: System::Windows::Forms::RadioButton^  radioButton_Bat_Elp2Units;
private: System::Windows::Forms::RadioButton^  radioButton_Bat_Elp16Units;
private: System::Windows::Forms::Button^  button_Bat_ElpTimeUpdate;
private: System::Windows::Forms::Label^  label_Voltage_Display;
private: System::Windows::Forms::TextBox^  textBox_Bat_ElpTimeChange;
private: System::Windows::Forms::Label^  label_Bat_ElpTimeDisplay;
private: System::Windows::Forms::Label^  label_Bat_ElpTime;
private: System::Windows::Forms::CheckBox^  checkBox_Bat_Voltage;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Bat_VoltFrac;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Bat_VoltInt;
private: System::Windows::Forms::CheckBox^  checkBox_Bat_Status;
private: System::Windows::Forms::ComboBox^  comboBoxBatStatus;
private: System::Windows::Forms::Label^  label20;
private: System::Windows::Forms::Label^  label19;
private: System::Windows::Forms::TabPage^  tabPage7;
private: System::Windows::Forms::Label^  label25;
private: System::Windows::Forms::Label^  label24;
private: System::Windows::Forms::Label^  label23;
private: System::Windows::Forms::Label^  label22;
private: System::Windows::Forms::Label^  label21;
private: System::Windows::Forms::CheckBox^  checkBoxEnableDateTime;
private: System::Windows::Forms::ComboBox^  comboBoxDayOfWeek;
private: System::Windows::Forms::Label^  label27;
private: System::Windows::Forms::Label^  label26;
private: System::Windows::Forms::NumericUpDown^  numericUpDownMonth;
private: System::Windows::Forms::NumericUpDown^  numericUpDownYear;
private: System::Windows::Forms::NumericUpDown^  numericUpDownDay;
private: System::Windows::Forms::NumericUpDown^  numericUpDownHours;
private: System::Windows::Forms::NumericUpDown^  numericUpDownMinutes;
private: System::Windows::Forms::NumericUpDown^  numericUpDownSeconds;
private: System::Windows::Forms::Label^  label28;
private: System::Windows::Forms::Label^  label29;
private: System::Windows::Forms::Label^  label30;
private: System::Windows::Forms::Label^  label31;
private: System::Windows::Forms::Label^  label32;
private: System::Windows::Forms::Label^  label33;
private: System::Windows::Forms::Label^  label34;
private: System::Windows::Forms::Panel^  panelTimeDate;
private: System::Windows::Forms::CheckBox^  checkBox_InvalidSerial;
private: System::Windows::Forms::Label^  label_Error;




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
			this->checkBoxTxSpeed = (gcnew System::Windows::Forms::CheckBox());
			this->numericUpDownSpeed = (gcnew System::Windows::Forms::NumericUpDown());
			this->labelAcumTime = (gcnew System::Windows::Forms::Label());
			this->labelAcumDistance = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->labelSpeed = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->tabPage2 = (gcnew System::Windows::Forms::TabPage());
			this->labelCurrentScaleFactor = (gcnew System::Windows::Forms::Label());
			this->labelCurrentMode = (gcnew System::Windows::Forms::Label());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->checkBoxModeStorageSupport = (gcnew System::Windows::Forms::CheckBox());
			this->labelCalibrationProgress = (gcnew System::Windows::Forms::Label());
			this->checkBoxCalibrationSupport = (gcnew System::Windows::Forms::CheckBox());
			this->tabPage3 = (gcnew System::Windows::Forms::TabPage());
			this->checkBoxPage3Enable = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxSecondaryData = (gcnew System::Windows::Forms::CheckBox());
			this->groupBoxPage3 = (gcnew System::Windows::Forms::GroupBox());
			this->label13 = (gcnew System::Windows::Forms::Label());
			this->comboBoxGPSFix = (gcnew System::Windows::Forms::ComboBox());
			this->label12 = (gcnew System::Windows::Forms::Label());
			this->numericUpDownHeading = (gcnew System::Windows::Forms::NumericUpDown());
			this->label11 = (gcnew System::Windows::Forms::Label());
			this->numericUpDownElevation = (gcnew System::Windows::Forms::NumericUpDown());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->numericUpDownLongitude = (gcnew System::Windows::Forms::NumericUpDown());
			this->numericUpDownLatitude = (gcnew System::Windows::Forms::NumericUpDown());
			this->tabPage4 = (gcnew System::Windows::Forms::TabPage());
			this->tabControl2 = (gcnew System::Windows::Forms::TabControl());
			this->tabPage5 = (gcnew System::Windows::Forms::TabPage());
			this->label_Error = (gcnew System::Windows::Forms::Label());
			this->checkBox_InvalidSerial = (gcnew System::Windows::Forms::CheckBox());
			this->button_Glb_GlobalDataUpdate = (gcnew System::Windows::Forms::Button());
			this->textBox_Glb_HardwareVerChange = (gcnew System::Windows::Forms::TextBox());
			this->label_Glb_HardwareVer = (gcnew System::Windows::Forms::Label());
			this->label_Glb_SoftwareVer = (gcnew System::Windows::Forms::Label());
			this->textBox_Glb_SoftwareVerChange = (gcnew System::Windows::Forms::TextBox());
			this->textBox_Glb_ModelNumChange = (gcnew System::Windows::Forms::TextBox());
			this->label_Glb_ManfID = (gcnew System::Windows::Forms::Label());
			this->textBox_Glb_ManfIDChange = (gcnew System::Windows::Forms::TextBox());
			this->label_Glb_ModelNum = (gcnew System::Windows::Forms::Label());
			this->textBox_Glb_SerialNumChange = (gcnew System::Windows::Forms::TextBox());
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
			this->tabPage7 = (gcnew System::Windows::Forms::TabPage());
			this->checkBoxEnableDateTime = (gcnew System::Windows::Forms::CheckBox());
			this->panelTimeDate = (gcnew System::Windows::Forms::Panel());
			this->comboBoxDayOfWeek = (gcnew System::Windows::Forms::ComboBox());
			this->label27 = (gcnew System::Windows::Forms::Label());
			this->numericUpDownMonth = (gcnew System::Windows::Forms::NumericUpDown());
			this->numericUpDownSeconds = (gcnew System::Windows::Forms::NumericUpDown());
			this->numericUpDownYear = (gcnew System::Windows::Forms::NumericUpDown());
			this->label26 = (gcnew System::Windows::Forms::Label());
			this->label21 = (gcnew System::Windows::Forms::Label());
			this->label22 = (gcnew System::Windows::Forms::Label());
			this->numericUpDownHours = (gcnew System::Windows::Forms::NumericUpDown());
			this->label23 = (gcnew System::Windows::Forms::Label());
			this->numericUpDownDay = (gcnew System::Windows::Forms::NumericUpDown());
			this->label24 = (gcnew System::Windows::Forms::Label());
			this->label25 = (gcnew System::Windows::Forms::Label());
			this->numericUpDownMinutes = (gcnew System::Windows::Forms::NumericUpDown());
			this->panel_Display = (gcnew System::Windows::Forms::Panel());
			this->labelTxFix = (gcnew System::Windows::Forms::Label());
			this->labelTxHeading = (gcnew System::Windows::Forms::Label());
			this->labelTxElev = (gcnew System::Windows::Forms::Label());
			this->label18 = (gcnew System::Windows::Forms::Label());
			this->label17 = (gcnew System::Windows::Forms::Label());
			this->label16 = (gcnew System::Windows::Forms::Label());
			this->lableTxLon = (gcnew System::Windows::Forms::Label());
			this->label15 = (gcnew System::Windows::Forms::Label());
			this->labelTxLat = (gcnew System::Windows::Forms::Label());
			this->label14 = (gcnew System::Windows::Forms::Label());
			this->labelTxDist = (gcnew System::Windows::Forms::Label());
			this->labelTxSpeed = (gcnew System::Windows::Forms::Label());
			this->labelTxTime = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label28 = (gcnew System::Windows::Forms::Label());
			this->label29 = (gcnew System::Windows::Forms::Label());
			this->label30 = (gcnew System::Windows::Forms::Label());
			this->label31 = (gcnew System::Windows::Forms::Label());
			this->label32 = (gcnew System::Windows::Forms::Label());
			this->label33 = (gcnew System::Windows::Forms::Label());
			this->label34 = (gcnew System::Windows::Forms::Label());
			this->panel_Settings->SuspendLayout();
			this->tabControl1->SuspendLayout();
			this->tabPage1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownSpeed))->BeginInit();
			this->tabPage2->SuspendLayout();
			this->tabPage3->SuspendLayout();
			this->groupBoxPage3->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownHeading))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownElevation))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownLongitude))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownLatitude))->BeginInit();
			this->tabPage4->SuspendLayout();
			this->tabControl2->SuspendLayout();
			this->tabPage5->SuspendLayout();
			this->tabPage6->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Bat_VoltInt))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Bat_VoltFrac))->BeginInit();
			this->groupBox_Resol->SuspendLayout();
			this->tabPage7->SuspendLayout();
			this->panelTimeDate->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownMonth))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownSeconds))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownYear))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownHours))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownDay))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownMinutes))->BeginInit();
			this->panel_Display->SuspendLayout();
			this->SuspendLayout();
			// 
			// panel_Settings
			// 
			this->panel_Settings->Controls->Add(this->tabControl1);
			this->panel_Settings->Location = System::Drawing::Point(322, 50);
			this->panel_Settings->Name = L"panel_Settings";
			this->panel_Settings->Size = System::Drawing::Size(400, 140);
			this->panel_Settings->TabIndex = 0;
			// 
			// tabControl1
			// 
			this->tabControl1->Controls->Add(this->tabPage1);
			this->tabControl1->Controls->Add(this->tabPage2);
			this->tabControl1->Controls->Add(this->tabPage3);
			this->tabControl1->Controls->Add(this->tabPage4);
			this->tabControl1->Location = System::Drawing::Point(3, 3);
			this->tabControl1->Name = L"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(392, 132);
			this->tabControl1->TabIndex = 6;
			// 
			// tabPage1
			// 
			this->tabPage1->Controls->Add(this->checkBoxTxSpeed);
			this->tabPage1->Controls->Add(this->numericUpDownSpeed);
			this->tabPage1->Controls->Add(this->labelAcumTime);
			this->tabPage1->Controls->Add(this->labelAcumDistance);
			this->tabPage1->Controls->Add(this->label2);
			this->tabPage1->Controls->Add(this->labelSpeed);
			this->tabPage1->Controls->Add(this->label1);
			this->tabPage1->Location = System::Drawing::Point(4, 22);
			this->tabPage1->Name = L"tabPage1";
			this->tabPage1->Padding = System::Windows::Forms::Padding(3);
			this->tabPage1->Size = System::Drawing::Size(384, 106);
			this->tabPage1->TabIndex = 0;
			this->tabPage1->Text = L"Main Data";
			this->tabPage1->UseVisualStyleBackColor = true;
			// 
			// checkBoxTxSpeed
			// 
			this->checkBoxTxSpeed->AutoSize = true;
			this->checkBoxTxSpeed->Checked = true;
			this->checkBoxTxSpeed->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBoxTxSpeed->Location = System::Drawing::Point(6, 43);
			this->checkBoxTxSpeed->Name = L"checkBoxTxSpeed";
			this->checkBoxTxSpeed->Size = System::Drawing::Size(126, 17);
			this->checkBoxTxSpeed->TabIndex = 6;
			this->checkBoxTxSpeed->Text = L"Transmit Speed Data";
			this->checkBoxTxSpeed->UseVisualStyleBackColor = true;
			this->checkBoxTxSpeed->CheckedChanged += gcnew System::EventHandler(this, &MSMSensor::checkBoxTxSpeed_CheckedChanged);
			// 
			// numericUpDownSpeed
			// 
			this->numericUpDownSpeed->DecimalPlaces = 3;
			this->numericUpDownSpeed->Location = System::Drawing::Point(76, 65);
			this->numericUpDownSpeed->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {65534, 0, 0, 196608});
			this->numericUpDownSpeed->Name = L"numericUpDownSpeed";
			this->numericUpDownSpeed->Size = System::Drawing::Size(61, 20);
			this->numericUpDownSpeed->TabIndex = 0;
			this->numericUpDownSpeed->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {5000, 0, 0, 196608});
			this->numericUpDownSpeed->ValueChanged += gcnew System::EventHandler(this, &MSMSensor::numericUpDownSpeed_ValueChanged);
			// 
			// labelAcumTime
			// 
			this->labelAcumTime->AutoSize = true;
			this->labelAcumTime->Location = System::Drawing::Point(121, 3);
			this->labelAcumTime->Name = L"labelAcumTime";
			this->labelAcumTime->Size = System::Drawing::Size(16, 13);
			this->labelAcumTime->TabIndex = 5;
			this->labelAcumTime->Text = L"---";
			// 
			// labelAcumDistance
			// 
			this->labelAcumDistance->AutoSize = true;
			this->labelAcumDistance->Location = System::Drawing::Point(143, 23);
			this->labelAcumDistance->Name = L"labelAcumDistance";
			this->labelAcumDistance->Size = System::Drawing::Size(16, 13);
			this->labelAcumDistance->TabIndex = 3;
			this->labelAcumDistance->Text = L"---";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(3, 3);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(112, 13);
			this->label2->TabIndex = 4;
			this->label2->Text = L"Accumulated Time (s):";
			// 
			// labelSpeed
			// 
			this->labelSpeed->AutoSize = true;
			this->labelSpeed->Location = System::Drawing::Point(2, 67);
			this->labelSpeed->Name = L"labelSpeed";
			this->labelSpeed->Size = System::Drawing::Size(68, 13);
			this->labelSpeed->TabIndex = 1;
			this->labelSpeed->Text = L"Speed (m/s):";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(3, 23);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(134, 13);
			this->label1->TabIndex = 2;
			this->label1->Text = L"Accumulated Distance (m):";
			// 
			// tabPage2
			// 
			this->tabPage2->Controls->Add(this->labelCurrentScaleFactor);
			this->tabPage2->Controls->Add(this->labelCurrentMode);
			this->tabPage2->Controls->Add(this->label8);
			this->tabPage2->Controls->Add(this->label7);
			this->tabPage2->Controls->Add(this->checkBoxModeStorageSupport);
			this->tabPage2->Controls->Add(this->labelCalibrationProgress);
			this->tabPage2->Controls->Add(this->checkBoxCalibrationSupport);
			this->tabPage2->Location = System::Drawing::Point(4, 22);
			this->tabPage2->Name = L"tabPage2";
			this->tabPage2->Padding = System::Windows::Forms::Padding(3);
			this->tabPage2->Size = System::Drawing::Size(384, 106);
			this->tabPage2->TabIndex = 1;
			this->tabPage2->Text = L"Calibration";
			this->tabPage2->UseVisualStyleBackColor = true;
			// 
			// labelCurrentScaleFactor
			// 
			this->labelCurrentScaleFactor->AutoSize = true;
			this->labelCurrentScaleFactor->Location = System::Drawing::Point(119, 81);
			this->labelCurrentScaleFactor->Name = L"labelCurrentScaleFactor";
			this->labelCurrentScaleFactor->Size = System::Drawing::Size(16, 13);
			this->labelCurrentScaleFactor->TabIndex = 7;
			this->labelCurrentScaleFactor->Text = L"---";
			// 
			// labelCurrentMode
			// 
			this->labelCurrentMode->AutoSize = true;
			this->labelCurrentMode->Location = System::Drawing::Point(86, 58);
			this->labelCurrentMode->Name = L"labelCurrentMode";
			this->labelCurrentMode->Size = System::Drawing::Size(16, 13);
			this->labelCurrentMode->TabIndex = 6;
			this->labelCurrentMode->Text = L"---";
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Location = System::Drawing::Point(6, 81);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(107, 13);
			this->label8->TabIndex = 5;
			this->label8->Text = L"Current Scale Factor:";
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(6, 58);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(74, 13);
			this->label7->TabIndex = 4;
			this->label7->Text = L"Current Mode:";
			// 
			// checkBoxModeStorageSupport
			// 
			this->checkBoxModeStorageSupport->AutoSize = true;
			this->checkBoxModeStorageSupport->Checked = true;
			this->checkBoxModeStorageSupport->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBoxModeStorageSupport->Location = System::Drawing::Point(6, 29);
			this->checkBoxModeStorageSupport->Name = L"checkBoxModeStorageSupport";
			this->checkBoxModeStorageSupport->Size = System::Drawing::Size(178, 17);
			this->checkBoxModeStorageSupport->TabIndex = 3;
			this->checkBoxModeStorageSupport->Text = L"Capable of Storing Mode Values";
			this->checkBoxModeStorageSupport->UseVisualStyleBackColor = true;
			this->checkBoxModeStorageSupport->CheckedChanged += gcnew System::EventHandler(this, &MSMSensor::checkBoxModeStorageSupport_CheckedChanged);
			// 
			// labelCalibrationProgress
			// 
			this->labelCalibrationProgress->AutoSize = true;
			this->labelCalibrationProgress->BackColor = System::Drawing::SystemColors::Control;
			this->labelCalibrationProgress->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->labelCalibrationProgress->Location = System::Drawing::Point(193, 45);
			this->labelCalibrationProgress->Name = L"labelCalibrationProgress";
			this->labelCalibrationProgress->Size = System::Drawing::Size(119, 16);
			this->labelCalibrationProgress->TabIndex = 2;
			this->labelCalibrationProgress->Text = L"Uncalibrated MSM";
			// 
			// checkBoxCalibrationSupport
			// 
			this->checkBoxCalibrationSupport->AutoSize = true;
			this->checkBoxCalibrationSupport->Checked = true;
			this->checkBoxCalibrationSupport->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBoxCalibrationSupport->Location = System::Drawing::Point(6, 6);
			this->checkBoxCalibrationSupport->Name = L"checkBoxCalibrationSupport";
			this->checkBoxCalibrationSupport->Size = System::Drawing::Size(127, 17);
			this->checkBoxCalibrationSupport->TabIndex = 1;
			this->checkBoxCalibrationSupport->Text = L"Calibration Supported";
			this->checkBoxCalibrationSupport->UseVisualStyleBackColor = true;
			this->checkBoxCalibrationSupport->CheckedChanged += gcnew System::EventHandler(this, &MSMSensor::checkBoxCalibrationSupport_CheckedChanged);
			// 
			// tabPage3
			// 
			this->tabPage3->Controls->Add(this->checkBoxPage3Enable);
			this->tabPage3->Controls->Add(this->checkBoxSecondaryData);
			this->tabPage3->Controls->Add(this->groupBoxPage3);
			this->tabPage3->Controls->Add(this->label10);
			this->tabPage3->Controls->Add(this->label9);
			this->tabPage3->Controls->Add(this->numericUpDownLongitude);
			this->tabPage3->Controls->Add(this->numericUpDownLatitude);
			this->tabPage3->Location = System::Drawing::Point(4, 22);
			this->tabPage3->Name = L"tabPage3";
			this->tabPage3->Size = System::Drawing::Size(384, 106);
			this->tabPage3->TabIndex = 2;
			this->tabPage3->Text = L"Secondary Data";
			this->tabPage3->UseVisualStyleBackColor = true;
			// 
			// checkBoxPage3Enable
			// 
			this->checkBoxPage3Enable->AutoSize = true;
			this->checkBoxPage3Enable->Location = System::Drawing::Point(8, 83);
			this->checkBoxPage3Enable->Name = L"checkBoxPage3Enable";
			this->checkBoxPage3Enable->Size = System::Drawing::Size(122, 17);
			this->checkBoxPage3Enable->TabIndex = 21;
			this->checkBoxPage3Enable->Text = L"Enable Page 3 Data";
			this->checkBoxPage3Enable->UseVisualStyleBackColor = true;
			this->checkBoxPage3Enable->CheckedChanged += gcnew System::EventHandler(this, &MSMSensor::checkBoxPage3Enable_CheckedChanged);
			// 
			// checkBoxSecondaryData
			// 
			this->checkBoxSecondaryData->AutoSize = true;
			this->checkBoxSecondaryData->Location = System::Drawing::Point(8, 7);
			this->checkBoxSecondaryData->Name = L"checkBoxSecondaryData";
			this->checkBoxSecondaryData->Size = System::Drawing::Size(139, 17);
			this->checkBoxSecondaryData->TabIndex = 20;
			this->checkBoxSecondaryData->Text = L"Enable Secondary Data";
			this->checkBoxSecondaryData->UseVisualStyleBackColor = true;
			this->checkBoxSecondaryData->CheckedChanged += gcnew System::EventHandler(this, &MSMSensor::checkBoxSecondaryData_CheckedChanged);
			// 
			// groupBoxPage3
			// 
			this->groupBoxPage3->Controls->Add(this->label13);
			this->groupBoxPage3->Controls->Add(this->comboBoxGPSFix);
			this->groupBoxPage3->Controls->Add(this->label12);
			this->groupBoxPage3->Controls->Add(this->numericUpDownHeading);
			this->groupBoxPage3->Controls->Add(this->label11);
			this->groupBoxPage3->Controls->Add(this->numericUpDownElevation);
			this->groupBoxPage3->Enabled = false;
			this->groupBoxPage3->Location = System::Drawing::Point(169, 0);
			this->groupBoxPage3->Name = L"groupBoxPage3";
			this->groupBoxPage3->Size = System::Drawing::Size(212, 103);
			this->groupBoxPage3->TabIndex = 19;
			this->groupBoxPage3->TabStop = false;
			this->groupBoxPage3->Text = L"Page 3 Data";
			// 
			// label13
			// 
			this->label13->AutoSize = true;
			this->label13->Location = System::Drawing::Point(6, 69);
			this->label13->Name = L"label13";
			this->label13->Size = System::Drawing::Size(45, 13);
			this->label13->TabIndex = 22;
			this->label13->Text = L"GPS Fix";
			// 
			// comboBoxGPSFix
			// 
			this->comboBoxGPSFix->FormattingEnabled = true;
			this->comboBoxGPSFix->Items->AddRange(gcnew cli::array< System::Object^  >(11) {L"None", L"Searching", L"Propagating", L"Last Known", 
				L"2D", L"2D WAAS", L"2D Differential", L"3D", L"3D WAAS", L"3D Differential", L"Invalid"});
			this->comboBoxGPSFix->Location = System::Drawing::Point(80, 66);
			this->comboBoxGPSFix->Name = L"comboBoxGPSFix";
			this->comboBoxGPSFix->Size = System::Drawing::Size(99, 21);
			this->comboBoxGPSFix->TabIndex = 21;
			this->comboBoxGPSFix->Text = L"None";
			this->comboBoxGPSFix->SelectedIndexChanged += gcnew System::EventHandler(this, &MSMSensor::comboBoxGPSFix_SelectedIndexChanged);
			// 
			// label12
			// 
			this->label12->AutoSize = true;
			this->label12->Location = System::Drawing::Point(6, 42);
			this->label12->Name = L"label12";
			this->label12->Size = System::Drawing::Size(47, 13);
			this->label12->TabIndex = 20;
			this->label12->Text = L"Heading";
			// 
			// numericUpDownHeading
			// 
			this->numericUpDownHeading->DecimalPlaces = 1;
			this->numericUpDownHeading->Location = System::Drawing::Point(80, 40);
			this->numericUpDownHeading->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {3599, 0, 0, 65536});
			this->numericUpDownHeading->Name = L"numericUpDownHeading";
			this->numericUpDownHeading->Size = System::Drawing::Size(61, 20);
			this->numericUpDownHeading->TabIndex = 19;
			this->numericUpDownHeading->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1123, 0, 0, 65536});
			this->numericUpDownHeading->ValueChanged += gcnew System::EventHandler(this, &MSMSensor::numericUpDownHeading_ValueChanged);
			// 
			// label11
			// 
			this->label11->AutoSize = true;
			this->label11->Location = System::Drawing::Point(6, 16);
			this->label11->Name = L"label11";
			this->label11->Size = System::Drawing::Size(68, 13);
			this->label11->TabIndex = 18;
			this->label11->Text = L"Elevation (m)";
			// 
			// numericUpDownElevation
			// 
			this->numericUpDownElevation->DecimalPlaces = 1;
			this->numericUpDownElevation->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {2, 0, 0, 65536});
			this->numericUpDownElevation->Location = System::Drawing::Point(80, 14);
			this->numericUpDownElevation->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {126068, 0, 0, 65536});
			this->numericUpDownElevation->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {500, 0, 0, System::Int32::MinValue});
			this->numericUpDownElevation->Name = L"numericUpDownElevation";
			this->numericUpDownElevation->Size = System::Drawing::Size(61, 20);
			this->numericUpDownElevation->TabIndex = 15;
			this->numericUpDownElevation->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1186, 0, 0, 0});
			this->numericUpDownElevation->ValueChanged += gcnew System::EventHandler(this, &MSMSensor::numericUpDownElevation_ValueChanged);
			// 
			// label10
			// 
			this->label10->AutoSize = true;
			this->label10->Location = System::Drawing::Point(4, 58);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(54, 13);
			this->label10->TabIndex = 17;
			this->label10->Text = L"Longitude";
			// 
			// label9
			// 
			this->label9->AutoSize = true;
			this->label9->Location = System::Drawing::Point(4, 32);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(45, 13);
			this->label9->TabIndex = 16;
			this->label9->Text = L"Latitude";
			// 
			// numericUpDownLongitude
			// 
			this->numericUpDownLongitude->DecimalPlaces = 6;
			this->numericUpDownLongitude->Enabled = false;
			this->numericUpDownLongitude->Location = System::Drawing::Point(64, 56);
			this->numericUpDownLongitude->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {180, 0, 0, 0});
			this->numericUpDownLongitude->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {180, 0, 0, System::Int32::MinValue});
			this->numericUpDownLongitude->Name = L"numericUpDownLongitude";
			this->numericUpDownLongitude->Size = System::Drawing::Size(99, 20);
			this->numericUpDownLongitude->TabIndex = 14;
			this->numericUpDownLongitude->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1145493, 0, 0, -2147221504});
			this->numericUpDownLongitude->ValueChanged += gcnew System::EventHandler(this, &MSMSensor::numericUpDownLongitude_ValueChanged);
			// 
			// numericUpDownLatitude
			// 
			this->numericUpDownLatitude->DecimalPlaces = 6;
			this->numericUpDownLatitude->Enabled = false;
			this->numericUpDownLatitude->Location = System::Drawing::Point(64, 30);
			this->numericUpDownLatitude->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {90, 0, 0, 0});
			this->numericUpDownLatitude->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {90, 0, 0, System::Int32::MinValue});
			this->numericUpDownLatitude->Name = L"numericUpDownLatitude";
			this->numericUpDownLatitude->Size = System::Drawing::Size(99, 20);
			this->numericUpDownLatitude->TabIndex = 13;
			this->numericUpDownLatitude->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {513292, 0, 0, 262144});
			this->numericUpDownLatitude->ValueChanged += gcnew System::EventHandler(this, &MSMSensor::numericUpDownLatitude_ValueChanged);
			// 
			// tabPage4
			// 
			this->tabPage4->Controls->Add(this->tabControl2);
			this->tabPage4->Location = System::Drawing::Point(4, 22);
			this->tabPage4->Name = L"tabPage4";
			this->tabPage4->Size = System::Drawing::Size(384, 106);
			this->tabPage4->TabIndex = 3;
			this->tabPage4->Text = L"Common Data";
			this->tabPage4->UseVisualStyleBackColor = true;
			// 
			// tabControl2
			// 
			this->tabControl2->Controls->Add(this->tabPage5);
			this->tabControl2->Controls->Add(this->tabPage6);
			this->tabControl2->Controls->Add(this->tabPage7);
			this->tabControl2->Location = System::Drawing::Point(0, 3);
			this->tabControl2->Name = L"tabControl2";
			this->tabControl2->SelectedIndex = 0;
			this->tabControl2->Size = System::Drawing::Size(388, 107);
			this->tabControl2->TabIndex = 0;
			// 
			// tabPage5
			// 
			this->tabPage5->Controls->Add(this->label_Error);
			this->tabPage5->Controls->Add(this->checkBox_InvalidSerial);
			this->tabPage5->Controls->Add(this->button_Glb_GlobalDataUpdate);
			this->tabPage5->Controls->Add(this->textBox_Glb_HardwareVerChange);
			this->tabPage5->Controls->Add(this->label_Glb_HardwareVer);
			this->tabPage5->Controls->Add(this->label_Glb_SoftwareVer);
			this->tabPage5->Controls->Add(this->textBox_Glb_SoftwareVerChange);
			this->tabPage5->Controls->Add(this->textBox_Glb_ModelNumChange);
			this->tabPage5->Controls->Add(this->label_Glb_ManfID);
			this->tabPage5->Controls->Add(this->textBox_Glb_ManfIDChange);
			this->tabPage5->Controls->Add(this->label_Glb_ModelNum);
			this->tabPage5->Controls->Add(this->textBox_Glb_SerialNumChange);
			this->tabPage5->Controls->Add(this->label_Glb_SerialNum);
			this->tabPage5->Location = System::Drawing::Point(4, 22);
			this->tabPage5->Name = L"tabPage5";
			this->tabPage5->Padding = System::Windows::Forms::Padding(3);
			this->tabPage5->Size = System::Drawing::Size(380, 81);
			this->tabPage5->TabIndex = 0;
			this->tabPage5->Text = L"Required Pages";
			this->tabPage5->UseVisualStyleBackColor = true;
			// 
			// label_Error
			// 
			this->label_Error->AutoSize = true;
			this->label_Error->ForeColor = System::Drawing::Color::Red;
			this->label_Error->Location = System::Drawing::Point(149, 62);
			this->label_Error->Name = L"label_Error";
			this->label_Error->Size = System::Drawing::Size(92, 13);
			this->label_Error->TabIndex = 61;
			this->label_Error->Text = L"Error: Invalid input";
			this->label_Error->Visible = false;
			// 
			// checkBox_InvalidSerial
			// 
			this->checkBox_InvalidSerial->AutoSize = true;
			this->checkBox_InvalidSerial->Location = System::Drawing::Point(149, 8);
			this->checkBox_InvalidSerial->Name = L"checkBox_InvalidSerial";
			this->checkBox_InvalidSerial->Size = System::Drawing::Size(79, 17);
			this->checkBox_InvalidSerial->TabIndex = 60;
			this->checkBox_InvalidSerial->Text = L"No Serial #";
			this->checkBox_InvalidSerial->UseVisualStyleBackColor = true;
			this->checkBox_InvalidSerial->CheckedChanged += gcnew System::EventHandler(this, &MSMSensor::checkBox_InvalidSerial_CheckedChanged);
			// 
			// button_Glb_GlobalDataUpdate
			// 
			this->button_Glb_GlobalDataUpdate->Location = System::Drawing::Point(250, 55);
			this->button_Glb_GlobalDataUpdate->Name = L"button_Glb_GlobalDataUpdate";
			this->button_Glb_GlobalDataUpdate->Size = System::Drawing::Size(97, 20);
			this->button_Glb_GlobalDataUpdate->TabIndex = 59;
			this->button_Glb_GlobalDataUpdate->Text = L"Update All";
			this->button_Glb_GlobalDataUpdate->UseVisualStyleBackColor = true;
			this->button_Glb_GlobalDataUpdate->Click += gcnew System::EventHandler(this, &MSMSensor::button_Glb_GlobalDataUpdate_Click);
			// 
			// textBox_Glb_HardwareVerChange
			// 
			this->textBox_Glb_HardwareVerChange->Location = System::Drawing::Point(318, 6);
			this->textBox_Glb_HardwareVerChange->MaxLength = 3;
			this->textBox_Glb_HardwareVerChange->Name = L"textBox_Glb_HardwareVerChange";
			this->textBox_Glb_HardwareVerChange->Size = System::Drawing::Size(29, 20);
			this->textBox_Glb_HardwareVerChange->TabIndex = 57;
			this->textBox_Glb_HardwareVerChange->Text = L"1";
			// 
			// label_Glb_HardwareVer
			// 
			this->label_Glb_HardwareVer->AutoSize = true;
			this->label_Glb_HardwareVer->Location = System::Drawing::Point(237, 9);
			this->label_Glb_HardwareVer->Name = L"label_Glb_HardwareVer";
			this->label_Glb_HardwareVer->Size = System::Drawing::Size(75, 13);
			this->label_Glb_HardwareVer->TabIndex = 54;
			this->label_Glb_HardwareVer->Text = L"Hardware Ver:";
			// 
			// label_Glb_SoftwareVer
			// 
			this->label_Glb_SoftwareVer->AutoSize = true;
			this->label_Glb_SoftwareVer->Location = System::Drawing::Point(241, 35);
			this->label_Glb_SoftwareVer->Name = L"label_Glb_SoftwareVer";
			this->label_Glb_SoftwareVer->Size = System::Drawing::Size(71, 13);
			this->label_Glb_SoftwareVer->TabIndex = 55;
			this->label_Glb_SoftwareVer->Text = L"Software Ver:";
			// 
			// textBox_Glb_SoftwareVerChange
			// 
			this->textBox_Glb_SoftwareVerChange->Location = System::Drawing::Point(318, 32);
			this->textBox_Glb_SoftwareVerChange->MaxLength = 3;
			this->textBox_Glb_SoftwareVerChange->Name = L"textBox_Glb_SoftwareVerChange";
			this->textBox_Glb_SoftwareVerChange->Size = System::Drawing::Size(29, 20);
			this->textBox_Glb_SoftwareVerChange->TabIndex = 58;
			this->textBox_Glb_SoftwareVerChange->Text = L"1";
			// 
			// textBox_Glb_ModelNumChange
			// 
			this->textBox_Glb_ModelNumChange->Location = System::Drawing::Point(94, 59);
			this->textBox_Glb_ModelNumChange->MaxLength = 5;
			this->textBox_Glb_ModelNumChange->Name = L"textBox_Glb_ModelNumChange";
			this->textBox_Glb_ModelNumChange->Size = System::Drawing::Size(49, 20);
			this->textBox_Glb_ModelNumChange->TabIndex = 52;
			this->textBox_Glb_ModelNumChange->Text = L"33669";
			// 
			// label_Glb_ManfID
			// 
			this->label_Glb_ManfID->AutoSize = true;
			this->label_Glb_ManfID->Location = System::Drawing::Point(6, 35);
			this->label_Glb_ManfID->Name = L"label_Glb_ManfID";
			this->label_Glb_ManfID->Size = System::Drawing::Size(51, 13);
			this->label_Glb_ManfID->TabIndex = 48;
			this->label_Glb_ManfID->Text = L"Manf. ID:";
			// 
			// textBox_Glb_ManfIDChange
			// 
			this->textBox_Glb_ManfIDChange->Location = System::Drawing::Point(94, 32);
			this->textBox_Glb_ManfIDChange->MaxLength = 5;
			this->textBox_Glb_ManfIDChange->Name = L"textBox_Glb_ManfIDChange";
			this->textBox_Glb_ManfIDChange->Size = System::Drawing::Size(49, 20);
			this->textBox_Glb_ManfIDChange->TabIndex = 51;
			this->textBox_Glb_ManfIDChange->Text = L"2";
			// 
			// label_Glb_ModelNum
			// 
			this->label_Glb_ModelNum->AutoSize = true;
			this->label_Glb_ModelNum->Location = System::Drawing::Point(6, 62);
			this->label_Glb_ModelNum->Name = L"label_Glb_ModelNum";
			this->label_Glb_ModelNum->Size = System::Drawing::Size(49, 13);
			this->label_Glb_ModelNum->TabIndex = 50;
			this->label_Glb_ModelNum->Text = L"Model #:";
			// 
			// textBox_Glb_SerialNumChange
			// 
			this->textBox_Glb_SerialNumChange->Location = System::Drawing::Point(67, 6);
			this->textBox_Glb_SerialNumChange->MaxLength = 10;
			this->textBox_Glb_SerialNumChange->Name = L"textBox_Glb_SerialNumChange";
			this->textBox_Glb_SerialNumChange->Size = System::Drawing::Size(76, 20);
			this->textBox_Glb_SerialNumChange->TabIndex = 53;
			this->textBox_Glb_SerialNumChange->Text = L"1234567890";
			// 
			// label_Glb_SerialNum
			// 
			this->label_Glb_SerialNum->AutoSize = true;
			this->label_Glb_SerialNum->Location = System::Drawing::Point(6, 9);
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
			this->tabPage6->Size = System::Drawing::Size(380, 81);
			this->tabPage6->TabIndex = 1;
			this->tabPage6->Text = L"Battery Page";
			this->tabPage6->UseVisualStyleBackColor = true;
			// 
			// label19
			// 
			this->label19->AutoSize = true;
			this->label19->Location = System::Drawing::Point(6, 24);
			this->label19->Name = L"label19";
			this->label19->Size = System::Drawing::Size(73, 13);
			this->label19->TabIndex = 79;
			this->label19->Text = L"Battery Status";
			// 
			// label20
			// 
			this->label20->AutoSize = true;
			this->label20->Location = System::Drawing::Point(13, 62);
			this->label20->Name = L"label20";
			this->label20->Size = System::Drawing::Size(143, 13);
			this->label20->TabIndex = 78;
			this->label20->Text = L"Fractional Voltage (1/256 V):";
			// 
			// comboBoxBatStatus
			// 
			this->comboBoxBatStatus->Enabled = false;
			this->comboBoxBatStatus->FormattingEnabled = true;
			this->comboBoxBatStatus->Items->AddRange(gcnew cli::array< System::Object^  >(6) {L"New", L"Good", L"Ok", L"Low", L"Critical", 
				L"Invalid"});
			this->comboBoxBatStatus->Location = System::Drawing::Point(85, 21);
			this->comboBoxBatStatus->Name = L"comboBoxBatStatus";
			this->comboBoxBatStatus->Size = System::Drawing::Size(60, 21);
			this->comboBoxBatStatus->TabIndex = 76;
			this->comboBoxBatStatus->Text = L"Ok";
			this->comboBoxBatStatus->SelectedIndexChanged += gcnew System::EventHandler(this, &MSMSensor::comboBoxBatStatus_SelectedIndexChanged);
			// 
			// button_Bat_ElpTimeUpdate
			// 
			this->button_Bat_ElpTimeUpdate->Enabled = false;
			this->button_Bat_ElpTimeUpdate->Location = System::Drawing::Point(226, 58);
			this->button_Bat_ElpTimeUpdate->Name = L"button_Bat_ElpTimeUpdate";
			this->button_Bat_ElpTimeUpdate->Size = System::Drawing::Size(84, 20);
			this->button_Bat_ElpTimeUpdate->TabIndex = 73;
			this->button_Bat_ElpTimeUpdate->Text = L"Update Time";
			this->button_Bat_ElpTimeUpdate->UseVisualStyleBackColor = true;
			this->button_Bat_ElpTimeUpdate->Click += gcnew System::EventHandler(this, &MSMSensor::button_Bat_ElpTimeUpdate_Click);
			// 
			// numericUpDown_Bat_VoltInt
			// 
			this->numericUpDown_Bat_VoltInt->Enabled = false;
			this->numericUpDown_Bat_VoltInt->Location = System::Drawing::Point(165, 41);
			this->numericUpDown_Bat_VoltInt->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {14, 0, 0, 0});
			this->numericUpDown_Bat_VoltInt->Name = L"numericUpDown_Bat_VoltInt";
			this->numericUpDown_Bat_VoltInt->Size = System::Drawing::Size(36, 20);
			this->numericUpDown_Bat_VoltInt->TabIndex = 68;
			this->numericUpDown_Bat_VoltInt->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {3, 0, 0, 0});
			this->numericUpDown_Bat_VoltInt->ValueChanged += gcnew System::EventHandler(this, &MSMSensor::numericUpDown_Bat_VoltInt_ValueChanged);
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
			this->numericUpDown_Bat_VoltFrac->Enabled = false;
			this->numericUpDown_Bat_VoltFrac->Location = System::Drawing::Point(165, 60);
			this->numericUpDown_Bat_VoltFrac->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
			this->numericUpDown_Bat_VoltFrac->Name = L"numericUpDown_Bat_VoltFrac";
			this->numericUpDown_Bat_VoltFrac->Size = System::Drawing::Size(42, 20);
			this->numericUpDown_Bat_VoltFrac->TabIndex = 69;
			this->numericUpDown_Bat_VoltFrac->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {85, 0, 0, 0});
			this->numericUpDown_Bat_VoltFrac->ValueChanged += gcnew System::EventHandler(this, &MSMSensor::numericUpDown_Bat_VoltFrac_ValueChanged);
			// 
			// groupBox_Resol
			// 
			this->groupBox_Resol->Controls->Add(this->radioButton_Bat_Elp2Units);
			this->groupBox_Resol->Controls->Add(this->radioButton_Bat_Elp16Units);
			this->groupBox_Resol->Enabled = false;
			this->groupBox_Resol->Location = System::Drawing::Point(253, 17);
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
			this->radioButton_Bat_Elp2Units->CheckedChanged += gcnew System::EventHandler(this, &MSMSensor::radioButton_Bat_Elp2Units_CheckedChanged);
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
			this->radioButton_Bat_Elp16Units->CheckedChanged += gcnew System::EventHandler(this, &MSMSensor::radioButton_Bat_Elp16Units_CheckedChanged);
			// 
			// checkBox_Bat_Voltage
			// 
			this->checkBox_Bat_Voltage->AutoSize = true;
			this->checkBox_Bat_Voltage->Enabled = false;
			this->checkBox_Bat_Voltage->Location = System::Drawing::Point(6, 42);
			this->checkBox_Bat_Voltage->Name = L"checkBox_Bat_Voltage";
			this->checkBox_Bat_Voltage->Size = System::Drawing::Size(153, 17);
			this->checkBox_Bat_Voltage->TabIndex = 70;
			this->checkBox_Bat_Voltage->Text = L"Enable Battery Voltage (V):";
			this->checkBox_Bat_Voltage->UseVisualStyleBackColor = true;
			this->checkBox_Bat_Voltage->CheckedChanged += gcnew System::EventHandler(this, &MSMSensor::checkBox_Bat_Voltage_CheckedChanged);
			// 
			// textBox_Bat_ElpTimeChange
			// 
			this->textBox_Bat_ElpTimeChange->Enabled = false;
			this->textBox_Bat_ElpTimeChange->Location = System::Drawing::Point(316, 58);
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
			this->label_Bat_ElpTimeDisplay->Location = System::Drawing::Point(317, 2);
			this->label_Bat_ElpTimeDisplay->Name = L"label_Bat_ElpTimeDisplay";
			this->label_Bat_ElpTimeDisplay->Size = System::Drawing::Size(62, 13);
			this->label_Bat_ElpTimeDisplay->TabIndex = 65;
			this->label_Bat_ElpTimeDisplay->Text = L"0";
			this->label_Bat_ElpTimeDisplay->TextAlign = System::Drawing::ContentAlignment::BottomRight;
			// 
			// label_Bat_ElpTime
			// 
			this->label_Bat_ElpTime->AutoSize = true;
			this->label_Bat_ElpTime->Location = System::Drawing::Point(223, 1);
			this->label_Bat_ElpTime->Name = L"label_Bat_ElpTime";
			this->label_Bat_ElpTime->Size = System::Drawing::Size(88, 13);
			this->label_Bat_ElpTime->TabIndex = 64;
			this->label_Bat_ElpTime->Text = L"Elapsed Time (s):";
			// 
			// checkBox_Bat_Status
			// 
			this->checkBox_Bat_Status->AutoSize = true;
			this->checkBox_Bat_Status->Location = System::Drawing::Point(0, 0);
			this->checkBox_Bat_Status->Name = L"checkBox_Bat_Status";
			this->checkBox_Bat_Status->Size = System::Drawing::Size(156, 17);
			this->checkBox_Bat_Status->TabIndex = 67;
			this->checkBox_Bat_Status->Text = L"Enable Battery Status Page";
			this->checkBox_Bat_Status->UseVisualStyleBackColor = true;
			this->checkBox_Bat_Status->CheckedChanged += gcnew System::EventHandler(this, &MSMSensor::checkBox_Bat_Status_CheckedChanged);
			// 
			// tabPage7
			// 
			this->tabPage7->Controls->Add(this->checkBoxEnableDateTime);
			this->tabPage7->Controls->Add(this->panelTimeDate);
			this->tabPage7->Location = System::Drawing::Point(4, 22);
			this->tabPage7->Name = L"tabPage7";
			this->tabPage7->Size = System::Drawing::Size(380, 81);
			this->tabPage7->TabIndex = 2;
			this->tabPage7->Text = L"Date Time";
			this->tabPage7->UseVisualStyleBackColor = true;
			// 
			// checkBoxEnableDateTime
			// 
			this->checkBoxEnableDateTime->AutoSize = true;
			this->checkBoxEnableDateTime->Location = System::Drawing::Point(0, 1);
			this->checkBoxEnableDateTime->Name = L"checkBoxEnableDateTime";
			this->checkBoxEnableDateTime->Size = System::Drawing::Size(139, 17);
			this->checkBoxEnableDateTime->TabIndex = 9;
			this->checkBoxEnableDateTime->Text = L"Enable Date Time Page";
			this->checkBoxEnableDateTime->UseVisualStyleBackColor = true;
			this->checkBoxEnableDateTime->CheckedChanged += gcnew System::EventHandler(this, &MSMSensor::checkBoxEnableDateTime_CheckedChanged);
			// 
			// panelTimeDate
			// 
			this->panelTimeDate->Controls->Add(this->comboBoxDayOfWeek);
			this->panelTimeDate->Controls->Add(this->label27);
			this->panelTimeDate->Controls->Add(this->numericUpDownMonth);
			this->panelTimeDate->Controls->Add(this->numericUpDownSeconds);
			this->panelTimeDate->Controls->Add(this->numericUpDownYear);
			this->panelTimeDate->Controls->Add(this->label26);
			this->panelTimeDate->Controls->Add(this->label21);
			this->panelTimeDate->Controls->Add(this->label22);
			this->panelTimeDate->Controls->Add(this->numericUpDownHours);
			this->panelTimeDate->Controls->Add(this->label23);
			this->panelTimeDate->Controls->Add(this->numericUpDownDay);
			this->panelTimeDate->Controls->Add(this->label24);
			this->panelTimeDate->Controls->Add(this->label25);
			this->panelTimeDate->Controls->Add(this->numericUpDownMinutes);
			this->panelTimeDate->Enabled = false;
			this->panelTimeDate->Location = System::Drawing::Point(15, 3);
			this->panelTimeDate->Name = L"panelTimeDate";
			this->panelTimeDate->Size = System::Drawing::Size(365, 75);
			this->panelTimeDate->TabIndex = 18;
			// 
			// comboBoxDayOfWeek
			// 
			this->comboBoxDayOfWeek->FormattingEnabled = true;
			this->comboBoxDayOfWeek->Items->AddRange(gcnew cli::array< System::Object^  >(8) {L"Sunday", L"Monday", L"Tuesday", L"Wednesday", 
				L"Thursday", L"Friday", L"Saturday", L"Invalid"});
			this->comboBoxDayOfWeek->Location = System::Drawing::Point(77, 27);
			this->comboBoxDayOfWeek->Name = L"comboBoxDayOfWeek";
			this->comboBoxDayOfWeek->Size = System::Drawing::Size(82, 21);
			this->comboBoxDayOfWeek->TabIndex = 8;
			this->comboBoxDayOfWeek->Text = L"Wednesday";
			this->comboBoxDayOfWeek->SelectedIndexChanged += gcnew System::EventHandler(this, &MSMSensor::comboBoxDayOfWeek_SelectedIndexChanged);
			// 
			// label27
			// 
			this->label27->AutoSize = true;
			this->label27->Location = System::Drawing::Point(1, 30);
			this->label27->Name = L"label27";
			this->label27->Size = System::Drawing::Size(70, 13);
			this->label27->TabIndex = 6;
			this->label27->Text = L"Day of Week";
			// 
			// numericUpDownMonth
			// 
			this->numericUpDownMonth->Location = System::Drawing::Point(208, 28);
			this->numericUpDownMonth->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {12, 0, 0, 0});
			this->numericUpDownMonth->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->numericUpDownMonth->Name = L"numericUpDownMonth";
			this->numericUpDownMonth->Size = System::Drawing::Size(37, 20);
			this->numericUpDownMonth->TabIndex = 17;
			this->numericUpDownMonth->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {2, 0, 0, 0});
			this->numericUpDownMonth->ValueChanged += gcnew System::EventHandler(this, &MSMSensor::numericUpDownMonth_ValueChanged);
			// 
			// numericUpDownSeconds
			// 
			this->numericUpDownSeconds->Location = System::Drawing::Point(324, 54);
			this->numericUpDownSeconds->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {59, 0, 0, 0});
			this->numericUpDownSeconds->Name = L"numericUpDownSeconds";
			this->numericUpDownSeconds->Size = System::Drawing::Size(37, 20);
			this->numericUpDownSeconds->TabIndex = 12;
			this->numericUpDownSeconds->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {30, 0, 0, 0});
			this->numericUpDownSeconds->ValueChanged += gcnew System::EventHandler(this, &MSMSensor::numericUpDownSeconds_ValueChanged);
			// 
			// numericUpDownYear
			// 
			this->numericUpDownYear->Location = System::Drawing::Point(208, 54);
			this->numericUpDownYear->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
			this->numericUpDownYear->Name = L"numericUpDownYear";
			this->numericUpDownYear->Size = System::Drawing::Size(45, 20);
			this->numericUpDownYear->TabIndex = 16;
			this->numericUpDownYear->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {9, 0, 0, 0});
			this->numericUpDownYear->ValueChanged += gcnew System::EventHandler(this, &MSMSensor::numericUpDownYear_ValueChanged);
			// 
			// label26
			// 
			this->label26->AutoSize = true;
			this->label26->Location = System::Drawing::Point(176, 4);
			this->label26->Name = L"label26";
			this->label26->Size = System::Drawing::Size(26, 13);
			this->label26->TabIndex = 5;
			this->label26->Text = L"Day";
			// 
			// label21
			// 
			this->label21->AutoSize = true;
			this->label21->Location = System::Drawing::Point(269, 56);
			this->label21->Name = L"label21";
			this->label21->Size = System::Drawing::Size(49, 13);
			this->label21->TabIndex = 0;
			this->label21->Text = L"Seconds";
			// 
			// label22
			// 
			this->label22->AutoSize = true;
			this->label22->Location = System::Drawing::Point(274, 30);
			this->label22->Name = L"label22";
			this->label22->Size = System::Drawing::Size(44, 13);
			this->label22->TabIndex = 1;
			this->label22->Text = L"Minutes";
			// 
			// numericUpDownHours
			// 
			this->numericUpDownHours->Location = System::Drawing::Point(324, 2);
			this->numericUpDownHours->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {23, 0, 0, 0});
			this->numericUpDownHours->Name = L"numericUpDownHours";
			this->numericUpDownHours->Size = System::Drawing::Size(37, 20);
			this->numericUpDownHours->TabIndex = 14;
			this->numericUpDownHours->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {12, 0, 0, 0});
			this->numericUpDownHours->ValueChanged += gcnew System::EventHandler(this, &MSMSensor::numericUpDownHours_ValueChanged);
			// 
			// label23
			// 
			this->label23->AutoSize = true;
			this->label23->Location = System::Drawing::Point(283, 4);
			this->label23->Name = L"label23";
			this->label23->Size = System::Drawing::Size(35, 13);
			this->label23->TabIndex = 2;
			this->label23->Text = L"Hours";
			// 
			// numericUpDownDay
			// 
			this->numericUpDownDay->Location = System::Drawing::Point(208, 2);
			this->numericUpDownDay->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {31, 0, 0, 0});
			this->numericUpDownDay->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->numericUpDownDay->Name = L"numericUpDownDay";
			this->numericUpDownDay->Size = System::Drawing::Size(37, 20);
			this->numericUpDownDay->TabIndex = 15;
			this->numericUpDownDay->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {24, 0, 0, 0});
			this->numericUpDownDay->ValueChanged += gcnew System::EventHandler(this, &MSMSensor::numericUpDownDay_ValueChanged);
			// 
			// label24
			// 
			this->label24->AutoSize = true;
			this->label24->Location = System::Drawing::Point(112, 56);
			this->label24->Name = L"label24";
			this->label24->Size = System::Drawing::Size(90, 13);
			this->label24->TabIndex = 3;
			this->label24->Text = L"Year (since 2000)";
			// 
			// label25
			// 
			this->label25->AutoSize = true;
			this->label25->Location = System::Drawing::Point(165, 30);
			this->label25->Name = L"label25";
			this->label25->Size = System::Drawing::Size(37, 13);
			this->label25->TabIndex = 4;
			this->label25->Text = L"Month";
			// 
			// numericUpDownMinutes
			// 
			this->numericUpDownMinutes->Location = System::Drawing::Point(324, 28);
			this->numericUpDownMinutes->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {59, 0, 0, 0});
			this->numericUpDownMinutes->Name = L"numericUpDownMinutes";
			this->numericUpDownMinutes->Size = System::Drawing::Size(37, 20);
			this->numericUpDownMinutes->TabIndex = 13;
			this->numericUpDownMinutes->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {30, 0, 0, 0});
			this->numericUpDownMinutes->ValueChanged += gcnew System::EventHandler(this, &MSMSensor::numericUpDownMinutes_ValueChanged);
			// 
			// panel_Display
			// 
			this->panel_Display->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->panel_Display->Controls->Add(this->labelTxFix);
			this->panel_Display->Controls->Add(this->labelTxHeading);
			this->panel_Display->Controls->Add(this->labelTxElev);
			this->panel_Display->Controls->Add(this->label18);
			this->panel_Display->Controls->Add(this->label17);
			this->panel_Display->Controls->Add(this->label16);
			this->panel_Display->Controls->Add(this->lableTxLon);
			this->panel_Display->Controls->Add(this->label15);
			this->panel_Display->Controls->Add(this->labelTxLat);
			this->panel_Display->Controls->Add(this->label14);
			this->panel_Display->Controls->Add(this->labelTxDist);
			this->panel_Display->Controls->Add(this->labelTxSpeed);
			this->panel_Display->Controls->Add(this->labelTxTime);
			this->panel_Display->Controls->Add(this->label6);
			this->panel_Display->Controls->Add(this->label5);
			this->panel_Display->Controls->Add(this->label4);
			this->panel_Display->Controls->Add(this->label3);
			this->panel_Display->Location = System::Drawing::Point(58, 188);
			this->panel_Display->Name = L"panel_Display";
			this->panel_Display->Size = System::Drawing::Size(200, 90);
			this->panel_Display->TabIndex = 1;
			// 
			// labelTxFix
			// 
			this->labelTxFix->AutoSize = true;
			this->labelTxFix->Location = System::Drawing::Point(161, 43);
			this->labelTxFix->Name = L"labelTxFix";
			this->labelTxFix->Size = System::Drawing::Size(16, 13);
			this->labelTxFix->TabIndex = 16;
			this->labelTxFix->Text = L"---";
			// 
			// labelTxHeading
			// 
			this->labelTxHeading->AutoSize = true;
			this->labelTxHeading->Location = System::Drawing::Point(161, 28);
			this->labelTxHeading->Name = L"labelTxHeading";
			this->labelTxHeading->Size = System::Drawing::Size(16, 13);
			this->labelTxHeading->TabIndex = 15;
			this->labelTxHeading->Text = L"---";
			// 
			// labelTxElev
			// 
			this->labelTxElev->AutoSize = true;
			this->labelTxElev->Location = System::Drawing::Point(161, 13);
			this->labelTxElev->Name = L"labelTxElev";
			this->labelTxElev->Size = System::Drawing::Size(16, 13);
			this->labelTxElev->TabIndex = 14;
			this->labelTxElev->Text = L"---";
			// 
			// label18
			// 
			this->label18->AutoSize = true;
			this->label18->Location = System::Drawing::Point(109, 43);
			this->label18->Name = L"label18";
			this->label18->Size = System::Drawing::Size(50, 13);
			this->label18->TabIndex = 13;
			this->label18->Text = L"Fix Type:";
			// 
			// label17
			// 
			this->label17->AutoSize = true;
			this->label17->Location = System::Drawing::Point(109, 28);
			this->label17->Name = L"label17";
			this->label17->Size = System::Drawing::Size(53, 13);
			this->label17->TabIndex = 12;
			this->label17->Text = L"Heading: ";
			// 
			// label16
			// 
			this->label16->AutoSize = true;
			this->label16->Location = System::Drawing::Point(109, 13);
			this->label16->Name = L"label16";
			this->label16->Size = System::Drawing::Size(57, 13);
			this->label16->TabIndex = 11;
			this->label16->Text = L"Elevation: ";
			// 
			// lableTxLon
			// 
			this->lableTxLon->AutoSize = true;
			this->lableTxLon->Location = System::Drawing::Point(64, 73);
			this->lableTxLon->Name = L"lableTxLon";
			this->lableTxLon->Size = System::Drawing::Size(16, 13);
			this->lableTxLon->TabIndex = 10;
			this->lableTxLon->Text = L"---";
			// 
			// label15
			// 
			this->label15->AutoSize = true;
			this->label15->Location = System::Drawing::Point(3, 73);
			this->label15->Name = L"label15";
			this->label15->Size = System::Drawing::Size(57, 13);
			this->label15->TabIndex = 9;
			this->label15->Text = L"Longitude:";
			// 
			// labelTxLat
			// 
			this->labelTxLat->AutoSize = true;
			this->labelTxLat->Location = System::Drawing::Point(64, 58);
			this->labelTxLat->Name = L"labelTxLat";
			this->labelTxLat->Size = System::Drawing::Size(16, 13);
			this->labelTxLat->TabIndex = 8;
			this->labelTxLat->Text = L"---";
			// 
			// label14
			// 
			this->label14->AutoSize = true;
			this->label14->Location = System::Drawing::Point(3, 58);
			this->label14->Name = L"label14";
			this->label14->Size = System::Drawing::Size(48, 13);
			this->label14->TabIndex = 7;
			this->label14->Text = L"Latitude:";
			// 
			// labelTxDist
			// 
			this->labelTxDist->AutoSize = true;
			this->labelTxDist->Location = System::Drawing::Point(64, 28);
			this->labelTxDist->Name = L"labelTxDist";
			this->labelTxDist->Size = System::Drawing::Size(16, 13);
			this->labelTxDist->TabIndex = 6;
			this->labelTxDist->Text = L"---";
			// 
			// labelTxSpeed
			// 
			this->labelTxSpeed->AutoSize = true;
			this->labelTxSpeed->Location = System::Drawing::Point(64, 43);
			this->labelTxSpeed->Name = L"labelTxSpeed";
			this->labelTxSpeed->Size = System::Drawing::Size(16, 13);
			this->labelTxSpeed->TabIndex = 5;
			this->labelTxSpeed->Text = L"---";
			// 
			// labelTxTime
			// 
			this->labelTxTime->AutoSize = true;
			this->labelTxTime->Location = System::Drawing::Point(64, 13);
			this->labelTxTime->Name = L"labelTxTime";
			this->labelTxTime->Size = System::Drawing::Size(16, 13);
			this->labelTxTime->TabIndex = 4;
			this->labelTxTime->Text = L"---";
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(3, 43);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(44, 13);
			this->label6->TabIndex = 3;
			this->label6->Text = L"Speed: ";
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(3, 28);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(55, 13);
			this->label5->TabIndex = 2;
			this->label5->Text = L"Distance: ";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(3, 13);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(36, 13);
			this->label4->TabIndex = 1;
			this->label4->Text = L"Time: ";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Underline, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label3->Location = System::Drawing::Point(3, 0);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(136, 13);
			this->label3->TabIndex = 0;
			this->label3->Text = L"Current MSM Transmission:";
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
			// MSMSensor
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(794, 351);
			this->Controls->Add(this->panel_Display);
			this->Controls->Add(this->panel_Settings);
			this->Name = L"MSMSensor";
			this->Text = L"Multi-Sport Speed and Distance";
			this->panel_Settings->ResumeLayout(false);
			this->tabControl1->ResumeLayout(false);
			this->tabPage1->ResumeLayout(false);
			this->tabPage1->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownSpeed))->EndInit();
			this->tabPage2->ResumeLayout(false);
			this->tabPage2->PerformLayout();
			this->tabPage3->ResumeLayout(false);
			this->tabPage3->PerformLayout();
			this->groupBoxPage3->ResumeLayout(false);
			this->groupBoxPage3->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownHeading))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownElevation))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownLongitude))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownLatitude))->EndInit();
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
			this->tabPage7->ResumeLayout(false);
			this->tabPage7->PerformLayout();
			this->panelTimeDate->ResumeLayout(false);
			this->panelTimeDate->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownMonth))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownSeconds))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownYear))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownHours))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownDay))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownMinutes))->EndInit();
			this->panel_Display->ResumeLayout(false);
			this->panel_Display->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
};