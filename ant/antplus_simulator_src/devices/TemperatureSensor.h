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
#include "antplus_temperature.h"	
#include "antplus_common.h"
#include "types.h"
#include "antdefines.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

public ref class TemperatureSensor : public System::Windows::Forms::Form, public ISimBase{
public:

		TemperatureSensor(System::Timers::Timer^ channelTimer, dRequestUpdateMesgPeriod^ channelUpdateMesgPeriod){
			InitializeComponent();
			TemperatureData = gcnew Temperature();
			commonData = gcnew CommonData();  
         requestUpdateMesgPeriod = channelUpdateMesgPeriod;
			timerHandle = channelTimer;		
			InitializeSim();
		}

		~TemperatureSensor(){
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
	virtual void onTimerTock(USHORT eventTime);							                     
	virtual void ANT_eventNotification(UCHAR ucEventCode_, UCHAR* pucEventBuffer_);	  
	virtual UCHAR getDeviceType(){return TemperatureData->DEVICE_TYPE;}  	
	virtual UCHAR getTransmissionType(){return TemperatureData->TX_TYPE_SENSOR;} 	
	virtual USHORT getTransmitPeriod(){return TemperatureData->MESG_P5HZ_PERIOD;}	
	virtual DOUBLE getTimerInterval(){return (double) ulTimerInterval;}
	virtual System::Windows::Forms::Panel^ getSimSettingsPanel(){return this->panel_Settings;}
	virtual System::Windows::Forms::Panel^ getSimTranslatedDisplay(){return this->panel_Display;}


private:
	void InitializeSim();						   // Initialize simulation
   void HandleTransmit(UCHAR* pucTxBuffer_); // Handles the data page transmissions
   void HandleReceive(UCHAR* pucRxBuffer_);  // Handles page requests   
	void UpdateBatStatus();
	void UpdateDisplay();
   void ForceUpdate();
	System::Void radioButton_SimTypeChanged (System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_Prm_CurTemp_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_Prm_MinMaxTemp_ValueChanged(System::Object^  sender, System::EventArgs^  e);
   System::Void numericUpDown_Prm_24HrHighLow_ValueChanged(System::Object^  sender, System::EventArgs^  e);
   System::Void button_AdvancedUpdate_Click(System::Object^  sender, System::EventArgs^  e);
   System::Void radioButton_Time_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_Bat_Status_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_Bat_Voltage_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_Bat_VoltInt_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_Bat_VoltFrac_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void comboBoxBatStatus_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void button_Bat_ElpTimeUpdate_Click(System::Object^  sender, System::EventArgs^  e);
	System::Void radioButton_Bat_Elp2Units_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void radioButton_Bat_Elp16Units_CheckedChanged(System::Object^  sender, System::EventArgs^  e);   
   System::Void radioButton_Default_4Hz_CheckedChanged(System::Object^  sender, System::EventArgs^  e);

private:
	//dRequestAckMsg^ requestAckMsg;		
	dRequestUpdateMesgPeriod^ requestUpdateMesgPeriod;
   Temperature^ TemperatureData;                // Temperature class variable
   CommonData^ commonData;
	System::Timers::Timer^ timerHandle;	
	ULONG  ulTimerInterval;	
	ULONG ulTotalTime;                  // used to track the on time for the batt page 

    // Simulator Variables
   ULONG ulRunTime;				// Time in ms
	UCHAR ucReserved;				// Reserved field
	UCHAR ucEventCount;			// Event count
	short sMinTemp;				// Minimum Temperature (hundredths of deg C)
	short sCurTemp;				// Current Temperature (hundredths of deg C)
	short sMaxTemp;				// Maximum Temperature (hundredths of deg C)
   UCHAR ucRequestCount;	   // Used to count the number of times to re-send the request pages
   UCHAR TxInfo;              // Contains Transmission info for Page 0

   // Background Data
	USHORT usMfgID;		// Manufacturing ID
	UCHAR ucHwVersion;	// Hardware version
	UCHAR ucSwVersion;	// Software version
	USHORT usModelNum;	// Model number
	USHORT usSerialNum;	// Serial number

   // Status
	UCHAR ucSimDataType;	   // Method to generate simulated data
	BOOL bSweepAscending;	// Sweep through heart rate values ascending
   BOOL b4HzDefault;

  
private: System::Windows::Forms::Panel^  panel_Settings;
private: System::Windows::Forms::Panel^  panel_Display;
private: System::Windows::Forms::TabControl^  tabControl1;
private: System::Windows::Forms::TabPage^  tabPage1;
private: System::Windows::Forms::TabPage^  tabPage2;
private: System::Windows::Forms::Label^  label_EventCount;
private: System::Windows::Forms::Label^  label_TxEventCount;
private: System::Windows::Forms::Label^  label_TranslatedDisplayLabel;
private: System::Windows::Forms::Label^  label_TempTx;

private: System::Windows::Forms::Label^  label_TxTempOf;
private: System::Windows::Forms::Label^  label_SoftwareVer;
private: System::Windows::Forms::Label^  label_HardwareVer;
private: System::Windows::Forms::Label^  label_ManfID;
private: System::Windows::Forms::Label^  label_ModelNum;
private: System::Windows::Forms::Label^  label_AdvancedError;
private: System::Windows::Forms::TextBox^  textBox_SoftwareVerChange;
private: System::Windows::Forms::Button^  button_AdvancedUpdate;
private: System::Windows::Forms::TextBox^  textBox_HardwareVerChange;
private: System::Windows::Forms::TextBox^  textBox_ModelNumChange;
private: System::Windows::Forms::TextBox^  textBox_SerialNumChange;
private: System::Windows::Forms::TextBox^  textBox_ManfIDChange;
private: System::Windows::Forms::Label^  label_SerialNum;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Prm_MaxTemp;

private: System::Windows::Forms::NumericUpDown^  numericUpDown_Prm_MinTemp;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Prm_CurTemp;
private: System::Windows::Forms::RadioButton^  radioButton_SimTypeSweep;



private: System::Windows::Forms::RadioButton^  radioButton_SimTypeFixed;
private: System::Windows::Forms::Label^  label_MaxTemp;



private: System::Windows::Forms::Label^  label_CurrentTemp;

private: System::Windows::Forms::Label^  label_MinTemp;


private: System::Windows::Forms::NumericUpDown^  numericUpDown_Prm_24HrHigh;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Prm_24HrLow;
private: System::Windows::Forms::Label^  label_Prm_24HrLow;
private: System::Windows::Forms::Label^  label_Prm_24HrHigh;
private: System::Windows::Forms::TabPage^  tabPage3;
private: System::Windows::Forms::RadioButton^  radioButton_UTCTime_Set;

private: System::Windows::Forms::RadioButton^  radioButton_UTCTime_NotSet;

private: System::Windows::Forms::RadioButton^  radioButton_UTCTime_NotSupported;

private: System::Windows::Forms::TabControl^  tabControl2;
private: System::Windows::Forms::TabPage^  tabPage4;
private: System::Windows::Forms::TabPage^  tabPage5;
private: System::Windows::Forms::Label^  label19;
private: System::Windows::Forms::ComboBox^  comboBoxBatStatus;
private: System::Windows::Forms::Label^  label_Voltage_Display;
private: System::Windows::Forms::GroupBox^  groupBox_Resol;
private: System::Windows::Forms::RadioButton^  radioButton_Bat_Elp2Units;
private: System::Windows::Forms::RadioButton^  radioButton_Bat_Elp16Units;
private: System::Windows::Forms::Label^  label_Bat_ElpTimeDisplay;
private: System::Windows::Forms::Label^  label_Bat_ElpTime;
private: System::Windows::Forms::CheckBox^  checkBox_Bat_Status;
private: System::Windows::Forms::Label^  label20;
private: System::Windows::Forms::Button^  button_Bat_ElpTimeUpdate;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Bat_VoltInt;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Bat_VoltFrac;
private: System::Windows::Forms::CheckBox^  checkBox_Bat_Voltage;
private: System::Windows::Forms::TextBox^  textBox_Bat_ElpTimeChange;
private: System::Windows::Forms::GroupBox^  groupBox_defaultPeriod;
private: System::Windows::Forms::RadioButton^  radioButton_Default_4Hz;
private: System::Windows::Forms::RadioButton^  radioButton_Default_P5Hz;
private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel_TxInfo_Time;
private: System::Windows::Forms::GroupBox^  groupBox_UTC_Time;
private: System::Windows::Forms::GroupBox^  groupBox_Local_Time;
private: System::Windows::Forms::RadioButton^  radioButton_LocalTime_Set;
private: System::Windows::Forms::RadioButton^  radioButton_LocalTime_NotSupported;
private: System::Windows::Forms::RadioButton^  radioButton_LocalTime_NotSet;



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
         this->groupBox_defaultPeriod = (gcnew System::Windows::Forms::GroupBox());
         this->radioButton_Default_4Hz = (gcnew System::Windows::Forms::RadioButton());
         this->radioButton_Default_P5Hz = (gcnew System::Windows::Forms::RadioButton());
         this->numericUpDown_Prm_24HrHigh = (gcnew System::Windows::Forms::NumericUpDown());
         this->numericUpDown_Prm_24HrLow = (gcnew System::Windows::Forms::NumericUpDown());
         this->label_Prm_24HrLow = (gcnew System::Windows::Forms::Label());
         this->label_Prm_24HrHigh = (gcnew System::Windows::Forms::Label());
         this->numericUpDown_Prm_MaxTemp = (gcnew System::Windows::Forms::NumericUpDown());
         this->numericUpDown_Prm_MinTemp = (gcnew System::Windows::Forms::NumericUpDown());
         this->numericUpDown_Prm_CurTemp = (gcnew System::Windows::Forms::NumericUpDown());
         this->radioButton_SimTypeSweep = (gcnew System::Windows::Forms::RadioButton());
         this->radioButton_SimTypeFixed = (gcnew System::Windows::Forms::RadioButton());
         this->label_MaxTemp = (gcnew System::Windows::Forms::Label());
         this->label_CurrentTemp = (gcnew System::Windows::Forms::Label());
         this->label_MinTemp = (gcnew System::Windows::Forms::Label());
         this->tabPage3 = (gcnew System::Windows::Forms::TabPage());
         this->tableLayoutPanel_TxInfo_Time = (gcnew System::Windows::Forms::TableLayoutPanel());
         this->groupBox_UTC_Time = (gcnew System::Windows::Forms::GroupBox());
         this->radioButton_UTCTime_Set = (gcnew System::Windows::Forms::RadioButton());
         this->radioButton_UTCTime_NotSupported = (gcnew System::Windows::Forms::RadioButton());
         this->radioButton_UTCTime_NotSet = (gcnew System::Windows::Forms::RadioButton());
         this->groupBox_Local_Time = (gcnew System::Windows::Forms::GroupBox());
         this->radioButton_LocalTime_Set = (gcnew System::Windows::Forms::RadioButton());
         this->radioButton_LocalTime_NotSupported = (gcnew System::Windows::Forms::RadioButton());
         this->radioButton_LocalTime_NotSet = (gcnew System::Windows::Forms::RadioButton());
         this->tabPage2 = (gcnew System::Windows::Forms::TabPage());
         this->tabControl2 = (gcnew System::Windows::Forms::TabControl());
         this->tabPage4 = (gcnew System::Windows::Forms::TabPage());
         this->label_SoftwareVer = (gcnew System::Windows::Forms::Label());
         this->label_ManfID = (gcnew System::Windows::Forms::Label());
         this->label_HardwareVer = (gcnew System::Windows::Forms::Label());
         this->label_SerialNum = (gcnew System::Windows::Forms::Label());
         this->textBox_ManfIDChange = (gcnew System::Windows::Forms::TextBox());
         this->label_ModelNum = (gcnew System::Windows::Forms::Label());
         this->textBox_SerialNumChange = (gcnew System::Windows::Forms::TextBox());
         this->label_AdvancedError = (gcnew System::Windows::Forms::Label());
         this->textBox_ModelNumChange = (gcnew System::Windows::Forms::TextBox());
         this->textBox_SoftwareVerChange = (gcnew System::Windows::Forms::TextBox());
         this->textBox_HardwareVerChange = (gcnew System::Windows::Forms::TextBox());
         this->button_AdvancedUpdate = (gcnew System::Windows::Forms::Button());
         this->tabPage5 = (gcnew System::Windows::Forms::TabPage());
         this->label20 = (gcnew System::Windows::Forms::Label());
         this->button_Bat_ElpTimeUpdate = (gcnew System::Windows::Forms::Button());
         this->numericUpDown_Bat_VoltInt = (gcnew System::Windows::Forms::NumericUpDown());
         this->numericUpDown_Bat_VoltFrac = (gcnew System::Windows::Forms::NumericUpDown());
         this->checkBox_Bat_Voltage = (gcnew System::Windows::Forms::CheckBox());
         this->textBox_Bat_ElpTimeChange = (gcnew System::Windows::Forms::TextBox());
         this->label19 = (gcnew System::Windows::Forms::Label());
         this->comboBoxBatStatus = (gcnew System::Windows::Forms::ComboBox());
         this->label_Voltage_Display = (gcnew System::Windows::Forms::Label());
         this->groupBox_Resol = (gcnew System::Windows::Forms::GroupBox());
         this->radioButton_Bat_Elp2Units = (gcnew System::Windows::Forms::RadioButton());
         this->radioButton_Bat_Elp16Units = (gcnew System::Windows::Forms::RadioButton());
         this->label_Bat_ElpTimeDisplay = (gcnew System::Windows::Forms::Label());
         this->label_Bat_ElpTime = (gcnew System::Windows::Forms::Label());
         this->checkBox_Bat_Status = (gcnew System::Windows::Forms::CheckBox());
         this->panel_Display = (gcnew System::Windows::Forms::Panel());
         this->label_EventCount = (gcnew System::Windows::Forms::Label());
         this->label_TxEventCount = (gcnew System::Windows::Forms::Label());
         this->label_TranslatedDisplayLabel = (gcnew System::Windows::Forms::Label());
         this->label_TempTx = (gcnew System::Windows::Forms::Label());
         this->label_TxTempOf = (gcnew System::Windows::Forms::Label());
         this->panel_Settings->SuspendLayout();
         this->tabControl1->SuspendLayout();
         this->tabPage1->SuspendLayout();
         this->groupBox_defaultPeriod->SuspendLayout();
         (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Prm_24HrHigh))->BeginInit();
         (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Prm_24HrLow))->BeginInit();
         (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Prm_MaxTemp))->BeginInit();
         (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Prm_MinTemp))->BeginInit();
         (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Prm_CurTemp))->BeginInit();
         this->tabPage3->SuspendLayout();
         this->tableLayoutPanel_TxInfo_Time->SuspendLayout();
         this->groupBox_UTC_Time->SuspendLayout();
         this->groupBox_Local_Time->SuspendLayout();
         this->tabPage2->SuspendLayout();
         this->tabControl2->SuspendLayout();
         this->tabPage4->SuspendLayout();
         this->tabPage5->SuspendLayout();
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
         this->tabControl1->Dock = System::Windows::Forms::DockStyle::Fill;
         this->tabControl1->Location = System::Drawing::Point(0, 0);
         this->tabControl1->Name = L"tabControl1";
         this->tabControl1->SelectedIndex = 0;
         this->tabControl1->Size = System::Drawing::Size(400, 166);
         this->tabControl1->TabIndex = 0;
         // 
         // tabPage1
         // 
         this->tabPage1->Controls->Add(this->groupBox_defaultPeriod);
         this->tabPage1->Controls->Add(this->numericUpDown_Prm_24HrHigh);
         this->tabPage1->Controls->Add(this->numericUpDown_Prm_24HrLow);
         this->tabPage1->Controls->Add(this->label_Prm_24HrLow);
         this->tabPage1->Controls->Add(this->label_Prm_24HrHigh);
         this->tabPage1->Controls->Add(this->numericUpDown_Prm_MaxTemp);
         this->tabPage1->Controls->Add(this->numericUpDown_Prm_MinTemp);
         this->tabPage1->Controls->Add(this->numericUpDown_Prm_CurTemp);
         this->tabPage1->Controls->Add(this->radioButton_SimTypeSweep);
         this->tabPage1->Controls->Add(this->radioButton_SimTypeFixed);
         this->tabPage1->Controls->Add(this->label_MaxTemp);
         this->tabPage1->Controls->Add(this->label_CurrentTemp);
         this->tabPage1->Controls->Add(this->label_MinTemp);
         this->tabPage1->Location = System::Drawing::Point(4, 22);
         this->tabPage1->Name = L"tabPage1";
         this->tabPage1->Padding = System::Windows::Forms::Padding(3);
         this->tabPage1->Size = System::Drawing::Size(392, 140);
         this->tabPage1->TabIndex = 0;
         this->tabPage1->Text = L"Parameters";
         this->tabPage1->UseVisualStyleBackColor = true;
         // 
         // groupBox_defaultPeriod
         // 
         this->groupBox_defaultPeriod->Controls->Add(this->radioButton_Default_4Hz);
         this->groupBox_defaultPeriod->Controls->Add(this->radioButton_Default_P5Hz);
         this->groupBox_defaultPeriod->Enabled = false;
         this->groupBox_defaultPeriod->Location = System::Drawing::Point(218, 80);
         this->groupBox_defaultPeriod->Name = L"groupBox_defaultPeriod";
         this->groupBox_defaultPeriod->Size = System::Drawing::Size(114, 44);
         this->groupBox_defaultPeriod->TabIndex = 31;
         this->groupBox_defaultPeriod->TabStop = false;
         this->groupBox_defaultPeriod->Text = L"Default Tx Period";
         // 
         // radioButton_Default_4Hz
         // 
         this->radioButton_Default_4Hz->AutoSize = true;
         this->radioButton_Default_4Hz->Location = System::Drawing::Point(65, 19);
         this->radioButton_Default_4Hz->Name = L"radioButton_Default_4Hz";
         this->radioButton_Default_4Hz->Size = System::Drawing::Size(44, 17);
         this->radioButton_Default_4Hz->TabIndex = 1;
         this->radioButton_Default_4Hz->Text = L"4Hz";
         this->radioButton_Default_4Hz->UseVisualStyleBackColor = true;
         this->radioButton_Default_4Hz->CheckedChanged += gcnew System::EventHandler(this, &TemperatureSensor::radioButton_Default_4Hz_CheckedChanged);
         // 
         // radioButton_Default_P5Hz
         // 
         this->radioButton_Default_P5Hz->AutoSize = true;
         this->radioButton_Default_P5Hz->Checked = true;
         this->radioButton_Default_P5Hz->Location = System::Drawing::Point(6, 19);
         this->radioButton_Default_P5Hz->Name = L"radioButton_Default_P5Hz";
         this->radioButton_Default_P5Hz->Size = System::Drawing::Size(53, 17);
         this->radioButton_Default_P5Hz->TabIndex = 0;
         this->radioButton_Default_P5Hz->TabStop = true;
         this->radioButton_Default_P5Hz->Text = L"0.5Hz";
         this->radioButton_Default_P5Hz->UseVisualStyleBackColor = true;
         // 
         // numericUpDown_Prm_24HrHigh
         // 
         this->numericUpDown_Prm_24HrHigh->DecimalPlaces = 1;
         this->numericUpDown_Prm_24HrHigh->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
         this->numericUpDown_Prm_24HrHigh->Location = System::Drawing::Point(218, 28);
         this->numericUpDown_Prm_24HrHigh->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {2047, 0, 0, 65536});
         this->numericUpDown_Prm_24HrHigh->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {2047, 0, 0, -2147418112});
         this->numericUpDown_Prm_24HrHigh->Name = L"numericUpDown_Prm_24HrHigh";
         this->numericUpDown_Prm_24HrHigh->Size = System::Drawing::Size(60, 20);
         this->numericUpDown_Prm_24HrHigh->TabIndex = 30;
         this->numericUpDown_Prm_24HrHigh->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {45, 0, 0, 0});
         this->numericUpDown_Prm_24HrHigh->ValueChanged += gcnew System::EventHandler(this, &TemperatureSensor::numericUpDown_Prm_24HrHighLow_ValueChanged);
         // 
         // numericUpDown_Prm_24HrLow
         // 
         this->numericUpDown_Prm_24HrLow->DecimalPlaces = 1;
         this->numericUpDown_Prm_24HrLow->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
         this->numericUpDown_Prm_24HrLow->Location = System::Drawing::Point(218, 52);
         this->numericUpDown_Prm_24HrLow->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {2047, 0, 0, 65536});
         this->numericUpDown_Prm_24HrLow->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {2047, 0, 0, -2147418112});
         this->numericUpDown_Prm_24HrLow->Name = L"numericUpDown_Prm_24HrLow";
         this->numericUpDown_Prm_24HrLow->Size = System::Drawing::Size(60, 20);
         this->numericUpDown_Prm_24HrLow->TabIndex = 29;
         this->numericUpDown_Prm_24HrLow->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {5, 0, 0, System::Int32::MinValue});
         this->numericUpDown_Prm_24HrLow->ValueChanged += gcnew System::EventHandler(this, &TemperatureSensor::numericUpDown_Prm_24HrHighLow_ValueChanged);
         // 
         // label_Prm_24HrLow
         // 
         this->label_Prm_24HrLow->AutoSize = true;
         this->label_Prm_24HrLow->Location = System::Drawing::Point(136, 54);
         this->label_Prm_24HrLow->Name = L"label_Prm_24HrLow";
         this->label_Prm_24HrLow->Size = System::Drawing::Size(76, 13);
         this->label_Prm_24HrLow->TabIndex = 28;
         this->label_Prm_24HrLow->Text = L"24 Hr Low (°C)";
         // 
         // label_Prm_24HrHigh
         // 
         this->label_Prm_24HrHigh->AutoSize = true;
         this->label_Prm_24HrHigh->Location = System::Drawing::Point(134, 30);
         this->label_Prm_24HrHigh->Name = L"label_Prm_24HrHigh";
         this->label_Prm_24HrHigh->Size = System::Drawing::Size(78, 13);
         this->label_Prm_24HrHigh->TabIndex = 27;
         this->label_Prm_24HrHigh->Text = L"24 Hr High (°C)";
         // 
         // numericUpDown_Prm_MaxTemp
         // 
         this->numericUpDown_Prm_MaxTemp->DecimalPlaces = 2;
         this->numericUpDown_Prm_MaxTemp->Enabled = false;
         this->numericUpDown_Prm_MaxTemp->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 131072});
         this->numericUpDown_Prm_MaxTemp->Location = System::Drawing::Point(66, 78);
         this->numericUpDown_Prm_MaxTemp->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {32767, 0, 0, 131072});
         this->numericUpDown_Prm_MaxTemp->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {32767, 0, 0, -2147352576});
         this->numericUpDown_Prm_MaxTemp->Name = L"numericUpDown_Prm_MaxTemp";
         this->numericUpDown_Prm_MaxTemp->Size = System::Drawing::Size(60, 20);
         this->numericUpDown_Prm_MaxTemp->TabIndex = 23;
         this->numericUpDown_Prm_MaxTemp->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {35, 0, 0, 0});
         this->numericUpDown_Prm_MaxTemp->ValueChanged += gcnew System::EventHandler(this, &TemperatureSensor::numericUpDown_Prm_MinMaxTemp_ValueChanged);
         // 
         // numericUpDown_Prm_MinTemp
         // 
         this->numericUpDown_Prm_MinTemp->DecimalPlaces = 2;
         this->numericUpDown_Prm_MinTemp->Enabled = false;
         this->numericUpDown_Prm_MinTemp->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 131072});
         this->numericUpDown_Prm_MinTemp->Location = System::Drawing::Point(66, 27);
         this->numericUpDown_Prm_MinTemp->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {32767, 0, 0, 131072});
         this->numericUpDown_Prm_MinTemp->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {32767, 0, 0, -2147352576});
         this->numericUpDown_Prm_MinTemp->Name = L"numericUpDown_Prm_MinTemp";
         this->numericUpDown_Prm_MinTemp->Size = System::Drawing::Size(60, 20);
         this->numericUpDown_Prm_MinTemp->TabIndex = 22;
         this->numericUpDown_Prm_MinTemp->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {10, 0, 0, System::Int32::MinValue});
         this->numericUpDown_Prm_MinTemp->ValueChanged += gcnew System::EventHandler(this, &TemperatureSensor::numericUpDown_Prm_MinMaxTemp_ValueChanged);
         // 
         // numericUpDown_Prm_CurTemp
         // 
         this->numericUpDown_Prm_CurTemp->DecimalPlaces = 2;
         this->numericUpDown_Prm_CurTemp->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 131072});
         this->numericUpDown_Prm_CurTemp->Location = System::Drawing::Point(66, 52);
         this->numericUpDown_Prm_CurTemp->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {32767, 0, 0, 131072});
         this->numericUpDown_Prm_CurTemp->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {32767, 0, 0, -2147352576});
         this->numericUpDown_Prm_CurTemp->Name = L"numericUpDown_Prm_CurTemp";
         this->numericUpDown_Prm_CurTemp->Size = System::Drawing::Size(60, 20);
         this->numericUpDown_Prm_CurTemp->TabIndex = 20;
         this->numericUpDown_Prm_CurTemp->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {23, 0, 0, 0});
         this->numericUpDown_Prm_CurTemp->ValueChanged += gcnew System::EventHandler(this, &TemperatureSensor::numericUpDown_Prm_CurTemp_ValueChanged);
         // 
         // radioButton_SimTypeSweep
         // 
         this->radioButton_SimTypeSweep->AutoSize = true;
         this->radioButton_SimTypeSweep->Location = System::Drawing::Point(296, 52);
         this->radioButton_SimTypeSweep->Name = L"radioButton_SimTypeSweep";
         this->radioButton_SimTypeSweep->Size = System::Drawing::Size(58, 17);
         this->radioButton_SimTypeSweep->TabIndex = 26;
         this->radioButton_SimTypeSweep->Text = L"Sweep";
         this->radioButton_SimTypeSweep->UseVisualStyleBackColor = true;
         this->radioButton_SimTypeSweep->CheckedChanged += gcnew System::EventHandler(this, &TemperatureSensor::radioButton_SimTypeChanged);
         // 
         // radioButton_SimTypeFixed
         // 
         this->radioButton_SimTypeFixed->AutoSize = true;
         this->radioButton_SimTypeFixed->Checked = true;
         this->radioButton_SimTypeFixed->Location = System::Drawing::Point(296, 28);
         this->radioButton_SimTypeFixed->Name = L"radioButton_SimTypeFixed";
         this->radioButton_SimTypeFixed->Size = System::Drawing::Size(50, 17);
         this->radioButton_SimTypeFixed->TabIndex = 25;
         this->radioButton_SimTypeFixed->TabStop = true;
         this->radioButton_SimTypeFixed->Text = L"Fixed";
         this->radioButton_SimTypeFixed->UseVisualStyleBackColor = true;
         this->radioButton_SimTypeFixed->CheckedChanged += gcnew System::EventHandler(this, &TemperatureSensor::radioButton_SimTypeChanged);
         // 
         // label_MaxTemp
         // 
         this->label_MaxTemp->AutoSize = true;
         this->label_MaxTemp->Location = System::Drawing::Point(13, 80);
         this->label_MaxTemp->Name = L"label_MaxTemp";
         this->label_MaxTemp->Size = System::Drawing::Size(47, 13);
         this->label_MaxTemp->TabIndex = 21;
         this->label_MaxTemp->Text = L"Max (°C)";
         // 
         // label_CurrentTemp
         // 
         this->label_CurrentTemp->AutoSize = true;
         this->label_CurrentTemp->Location = System::Drawing::Point(-1, 54);
         this->label_CurrentTemp->Name = L"label_CurrentTemp";
         this->label_CurrentTemp->Size = System::Drawing::Size(61, 13);
         this->label_CurrentTemp->TabIndex = 19;
         this->label_CurrentTemp->Text = L"Current (°C)";
         // 
         // label_MinTemp
         // 
         this->label_MinTemp->AutoSize = true;
         this->label_MinTemp->Location = System::Drawing::Point(16, 30);
         this->label_MinTemp->Name = L"label_MinTemp";
         this->label_MinTemp->Size = System::Drawing::Size(44, 13);
         this->label_MinTemp->TabIndex = 18;
         this->label_MinTemp->Text = L"Min (°C)";
         // 
         // tabPage3
         // 
         this->tabPage3->Controls->Add(this->tableLayoutPanel_TxInfo_Time);
         this->tabPage3->Location = System::Drawing::Point(4, 22);
         this->tabPage3->Name = L"tabPage3";
         this->tabPage3->Size = System::Drawing::Size(392, 140);
         this->tabPage3->TabIndex = 2;
         this->tabPage3->Text = L"Time";
         this->tabPage3->UseVisualStyleBackColor = true;
         // 
         // tableLayoutPanel_TxInfo_Time
         // 
         this->tableLayoutPanel_TxInfo_Time->ColumnCount = 2;
         this->tableLayoutPanel_TxInfo_Time->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
            50)));
         this->tableLayoutPanel_TxInfo_Time->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
            50)));
         this->tableLayoutPanel_TxInfo_Time->Controls->Add(this->groupBox_UTC_Time, 0, 0);
         this->tableLayoutPanel_TxInfo_Time->Controls->Add(this->groupBox_Local_Time, 1, 0);
         this->tableLayoutPanel_TxInfo_Time->Dock = System::Windows::Forms::DockStyle::Fill;
         this->tableLayoutPanel_TxInfo_Time->Location = System::Drawing::Point(0, 0);
         this->tableLayoutPanel_TxInfo_Time->Name = L"tableLayoutPanel_TxInfo_Time";
         this->tableLayoutPanel_TxInfo_Time->RowCount = 1;
         this->tableLayoutPanel_TxInfo_Time->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 
            50)));
         this->tableLayoutPanel_TxInfo_Time->Size = System::Drawing::Size(392, 140);
         this->tableLayoutPanel_TxInfo_Time->TabIndex = 3;
         // 
         // groupBox_UTC_Time
         // 
         this->groupBox_UTC_Time->Controls->Add(this->radioButton_UTCTime_Set);
         this->groupBox_UTC_Time->Controls->Add(this->radioButton_UTCTime_NotSupported);
         this->groupBox_UTC_Time->Controls->Add(this->radioButton_UTCTime_NotSet);
         this->groupBox_UTC_Time->Dock = System::Windows::Forms::DockStyle::Fill;
         this->groupBox_UTC_Time->Location = System::Drawing::Point(3, 3);
         this->groupBox_UTC_Time->Name = L"groupBox_UTC_Time";
         this->groupBox_UTC_Time->Size = System::Drawing::Size(190, 134);
         this->groupBox_UTC_Time->TabIndex = 0;
         this->groupBox_UTC_Time->TabStop = false;
         this->groupBox_UTC_Time->Text = L"UTC Time";
         // 
         // radioButton_UTCTime_Set
         // 
         this->radioButton_UTCTime_Set->AutoSize = true;
         this->radioButton_UTCTime_Set->Location = System::Drawing::Point(35, 90);
         this->radioButton_UTCTime_Set->Name = L"radioButton_UTCTime_Set";
         this->radioButton_UTCTime_Set->Size = System::Drawing::Size(114, 17);
         this->radioButton_UTCTime_Set->TabIndex = 2;
         this->radioButton_UTCTime_Set->Text = L"Supported and Set";
         this->radioButton_UTCTime_Set->UseVisualStyleBackColor = true;
         this->radioButton_UTCTime_Set->CheckedChanged += gcnew System::EventHandler(this, &TemperatureSensor::radioButton_Time_CheckedChanged);
         // 
         // radioButton_UTCTime_NotSupported
         // 
         this->radioButton_UTCTime_NotSupported->AutoSize = true;
         this->radioButton_UTCTime_NotSupported->Checked = true;
         this->radioButton_UTCTime_NotSupported->Location = System::Drawing::Point(35, 28);
         this->radioButton_UTCTime_NotSupported->Name = L"radioButton_UTCTime_NotSupported";
         this->radioButton_UTCTime_NotSupported->Size = System::Drawing::Size(94, 17);
         this->radioButton_UTCTime_NotSupported->TabIndex = 0;
         this->radioButton_UTCTime_NotSupported->TabStop = true;
         this->radioButton_UTCTime_NotSupported->Text = L"Not Supported";
         this->radioButton_UTCTime_NotSupported->UseVisualStyleBackColor = true;
         this->radioButton_UTCTime_NotSupported->CheckedChanged += gcnew System::EventHandler(this, &TemperatureSensor::radioButton_Time_CheckedChanged);
         // 
         // radioButton_UTCTime_NotSet
         // 
         this->radioButton_UTCTime_NotSet->AutoSize = true;
         this->radioButton_UTCTime_NotSet->Location = System::Drawing::Point(35, 59);
         this->radioButton_UTCTime_NotSet->Name = L"radioButton_UTCTime_NotSet";
         this->radioButton_UTCTime_NotSet->Size = System::Drawing::Size(116, 17);
         this->radioButton_UTCTime_NotSet->TabIndex = 1;
         this->radioButton_UTCTime_NotSet->Text = L"Supported, Not Set";
         this->radioButton_UTCTime_NotSet->UseVisualStyleBackColor = true;
         this->radioButton_UTCTime_NotSet->CheckedChanged += gcnew System::EventHandler(this, &TemperatureSensor::radioButton_Time_CheckedChanged);
         // 
         // groupBox_Local_Time
         // 
         this->groupBox_Local_Time->Controls->Add(this->radioButton_LocalTime_Set);
         this->groupBox_Local_Time->Controls->Add(this->radioButton_LocalTime_NotSupported);
         this->groupBox_Local_Time->Controls->Add(this->radioButton_LocalTime_NotSet);
         this->groupBox_Local_Time->Dock = System::Windows::Forms::DockStyle::Fill;
         this->groupBox_Local_Time->Location = System::Drawing::Point(199, 3);
         this->groupBox_Local_Time->Name = L"groupBox_Local_Time";
         this->groupBox_Local_Time->Size = System::Drawing::Size(190, 134);
         this->groupBox_Local_Time->TabIndex = 1;
         this->groupBox_Local_Time->TabStop = false;
         this->groupBox_Local_Time->Text = L"Local Time";
         // 
         // radioButton_LocalTime_Set
         // 
         this->radioButton_LocalTime_Set->AutoSize = true;
         this->radioButton_LocalTime_Set->Location = System::Drawing::Point(37, 90);
         this->radioButton_LocalTime_Set->Name = L"radioButton_LocalTime_Set";
         this->radioButton_LocalTime_Set->Size = System::Drawing::Size(114, 17);
         this->radioButton_LocalTime_Set->TabIndex = 5;
         this->radioButton_LocalTime_Set->Text = L"Supported and Set";
         this->radioButton_LocalTime_Set->UseVisualStyleBackColor = true;
         this->radioButton_LocalTime_Set->CheckedChanged += gcnew System::EventHandler(this, &TemperatureSensor::radioButton_Time_CheckedChanged);
         // 
         // radioButton_LocalTime_NotSupported
         // 
         this->radioButton_LocalTime_NotSupported->AutoSize = true;
         this->radioButton_LocalTime_NotSupported->Checked = true;
         this->radioButton_LocalTime_NotSupported->Location = System::Drawing::Point(37, 28);
         this->radioButton_LocalTime_NotSupported->Name = L"radioButton_LocalTime_NotSupported";
         this->radioButton_LocalTime_NotSupported->Size = System::Drawing::Size(94, 17);
         this->radioButton_LocalTime_NotSupported->TabIndex = 3;
         this->radioButton_LocalTime_NotSupported->TabStop = true;
         this->radioButton_LocalTime_NotSupported->Text = L"Not Supported";
         this->radioButton_LocalTime_NotSupported->UseVisualStyleBackColor = true;
         this->radioButton_LocalTime_NotSupported->CheckedChanged += gcnew System::EventHandler(this, &TemperatureSensor::radioButton_Time_CheckedChanged);
         // 
         // radioButton_LocalTime_NotSet
         // 
         this->radioButton_LocalTime_NotSet->AutoSize = true;
         this->radioButton_LocalTime_NotSet->Location = System::Drawing::Point(37, 59);
         this->radioButton_LocalTime_NotSet->Name = L"radioButton_LocalTime_NotSet";
         this->radioButton_LocalTime_NotSet->Size = System::Drawing::Size(116, 17);
         this->radioButton_LocalTime_NotSet->TabIndex = 4;
         this->radioButton_LocalTime_NotSet->Text = L"Supported, Not Set";
         this->radioButton_LocalTime_NotSet->UseVisualStyleBackColor = true;
         this->radioButton_LocalTime_NotSet->CheckedChanged += gcnew System::EventHandler(this, &TemperatureSensor::radioButton_Time_CheckedChanged);
         // 
         // tabPage2
         // 
         this->tabPage2->Controls->Add(this->tabControl2);
         this->tabPage2->Location = System::Drawing::Point(4, 22);
         this->tabPage2->Name = L"tabPage2";
         this->tabPage2->Padding = System::Windows::Forms::Padding(3);
         this->tabPage2->Size = System::Drawing::Size(392, 140);
         this->tabPage2->TabIndex = 1;
         this->tabPage2->Text = L"Common Data";
         this->tabPage2->UseVisualStyleBackColor = true;
         // 
         // tabControl2
         // 
         this->tabControl2->Controls->Add(this->tabPage4);
         this->tabControl2->Controls->Add(this->tabPage5);
         this->tabControl2->Dock = System::Windows::Forms::DockStyle::Fill;
         this->tabControl2->Location = System::Drawing::Point(3, 3);
         this->tabControl2->Name = L"tabControl2";
         this->tabControl2->SelectedIndex = 0;
         this->tabControl2->Size = System::Drawing::Size(386, 134);
         this->tabControl2->TabIndex = 40;
         // 
         // tabPage4
         // 
         this->tabPage4->Controls->Add(this->label_SoftwareVer);
         this->tabPage4->Controls->Add(this->label_ManfID);
         this->tabPage4->Controls->Add(this->label_HardwareVer);
         this->tabPage4->Controls->Add(this->label_SerialNum);
         this->tabPage4->Controls->Add(this->textBox_ManfIDChange);
         this->tabPage4->Controls->Add(this->label_ModelNum);
         this->tabPage4->Controls->Add(this->textBox_SerialNumChange);
         this->tabPage4->Controls->Add(this->label_AdvancedError);
         this->tabPage4->Controls->Add(this->textBox_ModelNumChange);
         this->tabPage4->Controls->Add(this->textBox_SoftwareVerChange);
         this->tabPage4->Controls->Add(this->textBox_HardwareVerChange);
         this->tabPage4->Controls->Add(this->button_AdvancedUpdate);
         this->tabPage4->Location = System::Drawing::Point(4, 22);
         this->tabPage4->Name = L"tabPage4";
         this->tabPage4->Padding = System::Windows::Forms::Padding(3);
         this->tabPage4->Size = System::Drawing::Size(378, 108);
         this->tabPage4->TabIndex = 0;
         this->tabPage4->Text = L"Manufacturer\'s Info";
         this->tabPage4->UseVisualStyleBackColor = true;
         // 
         // label_SoftwareVer
         // 
         this->label_SoftwareVer->AutoSize = true;
         this->label_SoftwareVer->Location = System::Drawing::Point(171, 15);
         this->label_SoftwareVer->Name = L"label_SoftwareVer";
         this->label_SoftwareVer->Size = System::Drawing::Size(71, 13);
         this->label_SoftwareVer->TabIndex = 31;
         this->label_SoftwareVer->Text = L"Software Ver:";
         // 
         // label_ManfID
         // 
         this->label_ManfID->AutoSize = true;
         this->label_ManfID->Location = System::Drawing::Point(35, 15);
         this->label_ManfID->Name = L"label_ManfID";
         this->label_ManfID->Size = System::Drawing::Size(51, 13);
         this->label_ManfID->TabIndex = 28;
         this->label_ManfID->Text = L"Manf. ID:";
         // 
         // label_HardwareVer
         // 
         this->label_HardwareVer->AutoSize = true;
         this->label_HardwareVer->Location = System::Drawing::Point(11, 59);
         this->label_HardwareVer->Name = L"label_HardwareVer";
         this->label_HardwareVer->Size = System::Drawing::Size(75, 13);
         this->label_HardwareVer->TabIndex = 30;
         this->label_HardwareVer->Text = L"Hardware Ver:";
         // 
         // label_SerialNum
         // 
         this->label_SerialNum->AutoSize = true;
         this->label_SerialNum->Location = System::Drawing::Point(196, 37);
         this->label_SerialNum->Name = L"label_SerialNum";
         this->label_SerialNum->Size = System::Drawing::Size(46, 13);
         this->label_SerialNum->TabIndex = 29;
         this->label_SerialNum->Text = L"Serial #:";
         // 
         // textBox_ManfIDChange
         // 
         this->textBox_ManfIDChange->Location = System::Drawing::Point(101, 11);
         this->textBox_ManfIDChange->MaxLength = 5;
         this->textBox_ManfIDChange->Name = L"textBox_ManfIDChange";
         this->textBox_ManfIDChange->Size = System::Drawing::Size(45, 20);
         this->textBox_ManfIDChange->TabIndex = 34;
         this->textBox_ManfIDChange->Text = L"12345";
         // 
         // label_ModelNum
         // 
         this->label_ModelNum->AutoSize = true;
         this->label_ModelNum->Location = System::Drawing::Point(37, 37);
         this->label_ModelNum->Name = L"label_ModelNum";
         this->label_ModelNum->Size = System::Drawing::Size(49, 13);
         this->label_ModelNum->TabIndex = 32;
         this->label_ModelNum->Text = L"Model #:";
         // 
         // textBox_SerialNumChange
         // 
         this->textBox_SerialNumChange->Location = System::Drawing::Point(248, 33);
         this->textBox_SerialNumChange->MaxLength = 10;
         this->textBox_SerialNumChange->Name = L"textBox_SerialNumChange";
         this->textBox_SerialNumChange->Size = System::Drawing::Size(97, 20);
         this->textBox_SerialNumChange->TabIndex = 38;
         this->textBox_SerialNumChange->Text = L"1234567890";
         // 
         // label_AdvancedError
         // 
         this->label_AdvancedError->AutoSize = true;
         this->label_AdvancedError->Location = System::Drawing::Point(171, 59);
         this->label_AdvancedError->Name = L"label_AdvancedError";
         this->label_AdvancedError->Size = System::Drawing::Size(32, 13);
         this->label_AdvancedError->TabIndex = 33;
         this->label_AdvancedError->Text = L"Error:";
         this->label_AdvancedError->Visible = false;
         // 
         // textBox_ModelNumChange
         // 
         this->textBox_ModelNumChange->Location = System::Drawing::Point(101, 33);
         this->textBox_ModelNumChange->MaxLength = 5;
         this->textBox_ModelNumChange->Name = L"textBox_ModelNumChange";
         this->textBox_ModelNumChange->Size = System::Drawing::Size(45, 20);
         this->textBox_ModelNumChange->TabIndex = 35;
         this->textBox_ModelNumChange->Text = L"01234";
         // 
         // textBox_SoftwareVerChange
         // 
         this->textBox_SoftwareVerChange->Location = System::Drawing::Point(248, 11);
         this->textBox_SoftwareVerChange->MaxLength = 3;
         this->textBox_SoftwareVerChange->Name = L"textBox_SoftwareVerChange";
         this->textBox_SoftwareVerChange->Size = System::Drawing::Size(45, 20);
         this->textBox_SoftwareVerChange->TabIndex = 37;
         this->textBox_SoftwareVerChange->Text = L"1";
         // 
         // textBox_HardwareVerChange
         // 
         this->textBox_HardwareVerChange->Location = System::Drawing::Point(101, 55);
         this->textBox_HardwareVerChange->MaxLength = 3;
         this->textBox_HardwareVerChange->Name = L"textBox_HardwareVerChange";
         this->textBox_HardwareVerChange->Size = System::Drawing::Size(45, 20);
         this->textBox_HardwareVerChange->TabIndex = 36;
         this->textBox_HardwareVerChange->Text = L"1";
         // 
         // button_AdvancedUpdate
         // 
         this->button_AdvancedUpdate->Location = System::Drawing::Point(248, 55);
         this->button_AdvancedUpdate->Name = L"button_AdvancedUpdate";
         this->button_AdvancedUpdate->Size = System::Drawing::Size(97, 20);
         this->button_AdvancedUpdate->TabIndex = 39;
         this->button_AdvancedUpdate->Text = L"Update Prod Info";
         this->button_AdvancedUpdate->UseVisualStyleBackColor = true;
         this->button_AdvancedUpdate->Click += gcnew System::EventHandler(this, &TemperatureSensor::button_AdvancedUpdate_Click);
         // 
         // tabPage5
         // 
         this->tabPage5->Controls->Add(this->label20);
         this->tabPage5->Controls->Add(this->button_Bat_ElpTimeUpdate);
         this->tabPage5->Controls->Add(this->numericUpDown_Bat_VoltInt);
         this->tabPage5->Controls->Add(this->numericUpDown_Bat_VoltFrac);
         this->tabPage5->Controls->Add(this->checkBox_Bat_Voltage);
         this->tabPage5->Controls->Add(this->textBox_Bat_ElpTimeChange);
         this->tabPage5->Controls->Add(this->label19);
         this->tabPage5->Controls->Add(this->comboBoxBatStatus);
         this->tabPage5->Controls->Add(this->label_Voltage_Display);
         this->tabPage5->Controls->Add(this->groupBox_Resol);
         this->tabPage5->Controls->Add(this->label_Bat_ElpTimeDisplay);
         this->tabPage5->Controls->Add(this->label_Bat_ElpTime);
         this->tabPage5->Controls->Add(this->checkBox_Bat_Status);
         this->tabPage5->Location = System::Drawing::Point(4, 22);
         this->tabPage5->Name = L"tabPage5";
         this->tabPage5->Padding = System::Windows::Forms::Padding(3);
         this->tabPage5->Size = System::Drawing::Size(378, 108);
         this->tabPage5->TabIndex = 1;
         this->tabPage5->Text = L"Battery Page";
         this->tabPage5->UseVisualStyleBackColor = true;
         // 
         // label20
         // 
         this->label20->AutoSize = true;
         this->label20->Location = System::Drawing::Point(10, 78);
         this->label20->Name = L"label20";
         this->label20->Size = System::Drawing::Size(143, 13);
         this->label20->TabIndex = 92;
         this->label20->Text = L"Fractional Voltage (1/256 V):";
         // 
         // button_Bat_ElpTimeUpdate
         // 
         this->button_Bat_ElpTimeUpdate->Location = System::Drawing::Point(212, 76);
         this->button_Bat_ElpTimeUpdate->Name = L"button_Bat_ElpTimeUpdate";
         this->button_Bat_ElpTimeUpdate->Size = System::Drawing::Size(84, 20);
         this->button_Bat_ElpTimeUpdate->TabIndex = 91;
         this->button_Bat_ElpTimeUpdate->Text = L"Update Time";
         this->button_Bat_ElpTimeUpdate->UseVisualStyleBackColor = true;
         this->button_Bat_ElpTimeUpdate->Click += gcnew System::EventHandler(this, &TemperatureSensor::button_Bat_ElpTimeUpdate_Click);
         // 
         // numericUpDown_Bat_VoltInt
         // 
         this->numericUpDown_Bat_VoltInt->Location = System::Drawing::Point(165, 50);
         this->numericUpDown_Bat_VoltInt->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {14, 0, 0, 0});
         this->numericUpDown_Bat_VoltInt->Name = L"numericUpDown_Bat_VoltInt";
         this->numericUpDown_Bat_VoltInt->Size = System::Drawing::Size(36, 20);
         this->numericUpDown_Bat_VoltInt->TabIndex = 87;
         this->numericUpDown_Bat_VoltInt->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {3, 0, 0, 0});
         this->numericUpDown_Bat_VoltInt->ValueChanged += gcnew System::EventHandler(this, &TemperatureSensor::numericUpDown_Bat_VoltInt_ValueChanged);
         // 
         // numericUpDown_Bat_VoltFrac
         // 
         this->numericUpDown_Bat_VoltFrac->Location = System::Drawing::Point(159, 76);
         this->numericUpDown_Bat_VoltFrac->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
         this->numericUpDown_Bat_VoltFrac->Name = L"numericUpDown_Bat_VoltFrac";
         this->numericUpDown_Bat_VoltFrac->Size = System::Drawing::Size(42, 20);
         this->numericUpDown_Bat_VoltFrac->TabIndex = 88;
         this->numericUpDown_Bat_VoltFrac->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {85, 0, 0, 0});
         this->numericUpDown_Bat_VoltFrac->ValueChanged += gcnew System::EventHandler(this, &TemperatureSensor::numericUpDown_Bat_VoltFrac_ValueChanged);
         // 
         // checkBox_Bat_Voltage
         // 
         this->checkBox_Bat_Voltage->AutoSize = true;
         this->checkBox_Bat_Voltage->Checked = true;
         this->checkBox_Bat_Voltage->CheckState = System::Windows::Forms::CheckState::Checked;
         this->checkBox_Bat_Voltage->Location = System::Drawing::Point(0, 50);
         this->checkBox_Bat_Voltage->Name = L"checkBox_Bat_Voltage";
         this->checkBox_Bat_Voltage->Size = System::Drawing::Size(153, 17);
         this->checkBox_Bat_Voltage->TabIndex = 89;
         this->checkBox_Bat_Voltage->Text = L"Enable Battery Voltage (V):";
         this->checkBox_Bat_Voltage->UseVisualStyleBackColor = true;
         this->checkBox_Bat_Voltage->CheckedChanged += gcnew System::EventHandler(this, &TemperatureSensor::checkBox_Bat_Voltage_CheckedChanged);
         // 
         // textBox_Bat_ElpTimeChange
         // 
         this->textBox_Bat_ElpTimeChange->Location = System::Drawing::Point(302, 76);
         this->textBox_Bat_ElpTimeChange->MaxLength = 9;
         this->textBox_Bat_ElpTimeChange->Name = L"textBox_Bat_ElpTimeChange";
         this->textBox_Bat_ElpTimeChange->Size = System::Drawing::Size(63, 20);
         this->textBox_Bat_ElpTimeChange->TabIndex = 90;
         this->textBox_Bat_ElpTimeChange->Text = L"0";
         this->textBox_Bat_ElpTimeChange->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
         // 
         // label19
         // 
         this->label19->AutoSize = true;
         this->label19->Location = System::Drawing::Point(62, 27);
         this->label19->Name = L"label19";
         this->label19->Size = System::Drawing::Size(73, 13);
         this->label19->TabIndex = 86;
         this->label19->Text = L"Battery Status";
         // 
         // comboBoxBatStatus
         // 
         this->comboBoxBatStatus->FormattingEnabled = true;
         this->comboBoxBatStatus->Items->AddRange(gcnew cli::array< System::Object^  >(6) {L"New", L"Good", L"Ok", L"Low", L"Critical", 
            L"Invalid"});
         this->comboBoxBatStatus->Location = System::Drawing::Point(141, 23);
         this->comboBoxBatStatus->Name = L"comboBoxBatStatus";
         this->comboBoxBatStatus->Size = System::Drawing::Size(60, 21);
         this->comboBoxBatStatus->TabIndex = 85;
         this->comboBoxBatStatus->Text = L"Ok";
         this->comboBoxBatStatus->SelectedIndexChanged += gcnew System::EventHandler(this, &TemperatureSensor::comboBoxBatStatus_SelectedIndexChanged);
         // 
         // label_Voltage_Display
         // 
         this->label_Voltage_Display->AutoSize = true;
         this->label_Voltage_Display->Location = System::Drawing::Point(186, 7);
         this->label_Voltage_Display->Name = L"label_Voltage_Display";
         this->label_Voltage_Display->Size = System::Drawing::Size(0, 13);
         this->label_Voltage_Display->TabIndex = 84;
         // 
         // groupBox_Resol
         // 
         this->groupBox_Resol->Controls->Add(this->radioButton_Bat_Elp2Units);
         this->groupBox_Resol->Controls->Add(this->radioButton_Bat_Elp16Units);
         this->groupBox_Resol->Location = System::Drawing::Point(241, 34);
         this->groupBox_Resol->Name = L"groupBox_Resol";
         this->groupBox_Resol->Size = System::Drawing::Size(124, 36);
         this->groupBox_Resol->TabIndex = 80;
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
         this->radioButton_Bat_Elp2Units->CheckedChanged += gcnew System::EventHandler(this, &TemperatureSensor::radioButton_Bat_Elp2Units_CheckedChanged);
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
         this->radioButton_Bat_Elp16Units->CheckedChanged += gcnew System::EventHandler(this, &TemperatureSensor::radioButton_Bat_Elp16Units_CheckedChanged);
         // 
         // label_Bat_ElpTimeDisplay
         // 
         this->label_Bat_ElpTimeDisplay->Enabled = false;
         this->label_Bat_ElpTimeDisplay->Location = System::Drawing::Point(306, 9);
         this->label_Bat_ElpTimeDisplay->Name = L"label_Bat_ElpTimeDisplay";
         this->label_Bat_ElpTimeDisplay->Size = System::Drawing::Size(62, 13);
         this->label_Bat_ElpTimeDisplay->TabIndex = 82;
         this->label_Bat_ElpTimeDisplay->Text = L"0";
         this->label_Bat_ElpTimeDisplay->TextAlign = System::Drawing::ContentAlignment::BottomRight;
         // 
         // label_Bat_ElpTime
         // 
         this->label_Bat_ElpTime->AutoSize = true;
         this->label_Bat_ElpTime->Location = System::Drawing::Point(212, 9);
         this->label_Bat_ElpTime->Name = L"label_Bat_ElpTime";
         this->label_Bat_ElpTime->Size = System::Drawing::Size(88, 13);
         this->label_Bat_ElpTime->TabIndex = 81;
         this->label_Bat_ElpTime->Text = L"Elapsed Time (s):";
         // 
         // checkBox_Bat_Status
         // 
         this->checkBox_Bat_Status->AutoSize = true;
         this->checkBox_Bat_Status->Checked = true;
         this->checkBox_Bat_Status->CheckState = System::Windows::Forms::CheckState::Checked;
         this->checkBox_Bat_Status->Location = System::Drawing::Point(3, 8);
         this->checkBox_Bat_Status->Name = L"checkBox_Bat_Status";
         this->checkBox_Bat_Status->Size = System::Drawing::Size(156, 17);
         this->checkBox_Bat_Status->TabIndex = 83;
         this->checkBox_Bat_Status->Text = L"Enable Battery Status Page";
         this->checkBox_Bat_Status->UseVisualStyleBackColor = true;
         this->checkBox_Bat_Status->CheckedChanged += gcnew System::EventHandler(this, &TemperatureSensor::checkBox_Bat_Status_CheckedChanged);
         // 
         // panel_Display
         // 
         this->panel_Display->Controls->Add(this->label_EventCount);
         this->panel_Display->Controls->Add(this->label_TxEventCount);
         this->panel_Display->Controls->Add(this->label_TranslatedDisplayLabel);
         this->panel_Display->Controls->Add(this->label_TempTx);
         this->panel_Display->Controls->Add(this->label_TxTempOf);
         this->panel_Display->Location = System::Drawing::Point(58, 188);
         this->panel_Display->Name = L"panel_Display";
         this->panel_Display->Size = System::Drawing::Size(200, 90);
         this->panel_Display->TabIndex = 1;
         // 
         // label_EventCount
         // 
         this->label_EventCount->AutoSize = true;
         this->label_EventCount->Location = System::Drawing::Point(151, 46);
         this->label_EventCount->Name = L"label_EventCount";
         this->label_EventCount->Size = System::Drawing::Size(16, 13);
         this->label_EventCount->TabIndex = 7;
         this->label_EventCount->Text = L"---";
         // 
         // label_TxEventCount
         // 
         this->label_TxEventCount->AutoSize = true;
         this->label_TxEventCount->Location = System::Drawing::Point(33, 46);
         this->label_TxEventCount->Name = L"label_TxEventCount";
         this->label_TxEventCount->Size = System::Drawing::Size(112, 13);
         this->label_TxEventCount->TabIndex = 5;
         this->label_TxEventCount->Text = L"Transmit Event Count:";
         // 
         // label_TranslatedDisplayLabel
         // 
         this->label_TranslatedDisplayLabel->AutoSize = true;
         this->label_TranslatedDisplayLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Underline, 
            System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
         this->label_TranslatedDisplayLabel->Location = System::Drawing::Point(3, 9);
         this->label_TranslatedDisplayLabel->Name = L"label_TranslatedDisplayLabel";
         this->label_TranslatedDisplayLabel->Size = System::Drawing::Size(82, 13);
         this->label_TranslatedDisplayLabel->TabIndex = 9;
         this->label_TranslatedDisplayLabel->Text = L"Current Tx Data";
         // 
         // label_TempTx
         // 
         this->label_TempTx->AutoSize = true;
         this->label_TempTx->Location = System::Drawing::Point(151, 33);
         this->label_TempTx->Name = L"label_TempTx";
         this->label_TempTx->Size = System::Drawing::Size(16, 13);
         this->label_TempTx->TabIndex = 8;
         this->label_TempTx->Text = L"---";
         // 
         // label_TxTempOf
         // 
         this->label_TxTempOf->AutoSize = true;
         this->label_TxTempOf->Location = System::Drawing::Point(18, 33);
         this->label_TxTempOf->Name = L"label_TxTempOf";
         this->label_TxTempOf->Size = System::Drawing::Size(127, 13);
         this->label_TxTempOf->TabIndex = 6;
         this->label_TxTempOf->Text = L"Current Temperature (°C):";
         // 
         // TemperatureSensor
         // 
         this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
         this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
         this->ClientSize = System::Drawing::Size(794, 351);
         this->Controls->Add(this->panel_Display);
         this->Controls->Add(this->panel_Settings);
         this->Name = L"TemperatureSensor";
         this->Text = L"Temperature Sensor";
         this->panel_Settings->ResumeLayout(false);
         this->tabControl1->ResumeLayout(false);
         this->tabPage1->ResumeLayout(false);
         this->tabPage1->PerformLayout();
         this->groupBox_defaultPeriod->ResumeLayout(false);
         this->groupBox_defaultPeriod->PerformLayout();
         (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Prm_24HrHigh))->EndInit();
         (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Prm_24HrLow))->EndInit();
         (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Prm_MaxTemp))->EndInit();
         (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Prm_MinTemp))->EndInit();
         (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Prm_CurTemp))->EndInit();
         this->tabPage3->ResumeLayout(false);
         this->tableLayoutPanel_TxInfo_Time->ResumeLayout(false);
         this->groupBox_UTC_Time->ResumeLayout(false);
         this->groupBox_UTC_Time->PerformLayout();
         this->groupBox_Local_Time->ResumeLayout(false);
         this->groupBox_Local_Time->PerformLayout();
         this->tabPage2->ResumeLayout(false);
         this->tabControl2->ResumeLayout(false);
         this->tabPage4->ResumeLayout(false);
         this->tabPage4->PerformLayout();
         this->tabPage5->ResumeLayout(false);
         this->tabPage5->PerformLayout();
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