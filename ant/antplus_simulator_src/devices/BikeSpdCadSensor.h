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
#include "bsc.h"
#include "types.h"
#include "antdefines.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

public ref class BikeSpdCadSensor : public System::Windows::Forms::Form, public ISimBase{
	public:
		BikeSpdCadSensor(System::Timers::Timer^ channelTimer){
			InitializeComponent();
			timerHandle = channelTimer;		// Get timer handle
			InitializeSim();
			
		}

		~BikeSpdCadSensor(){
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
	virtual void onTimerTock(USHORT eventTime);
	virtual void ANT_eventNotification(UCHAR ucEventCode_, UCHAR* pucEventBuffer_);
	virtual UCHAR getDeviceType(){return BSC_DEVICE_TYPE;}
	virtual UCHAR getTransmissionType(){return BSC_TX_TYPE;}
	virtual USHORT getTransmitPeriod(){return BSC_MSG_PERIOD;}
	virtual DOUBLE getTimerInterval(){return (double) ulTimerInterval;}
	virtual System::Windows::Forms::Panel^ getSimSettingsPanel(){return this->panel_Settings;}
	virtual System::Windows::Forms::Panel^ getSimTranslatedDisplay(){return this->panel_Display;}

private:
	void InitializeSim();
	void HandleTransmit(UCHAR* pucTxBuffer_);
	void UpdateDisplay();	
	void ForceUpdate();
	System::Void numericUpDown_Sim_WheelCircumference_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_Sim_CadCurOutput_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_Sim_CadMinMaxOutput_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_Sim_SpdCurOutput_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDown_Sim_SpdMinMaxOutput_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_Sim_SpeedSweeping_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_Sim_CadenceSweeping_CheckedChanged(System::Object^  sender, System::EventArgs^  e);

private:
	// Simulator timer
	System::Timers::Timer^ timerHandle;	// Timer handle
	ULONG ulRunTime16000;				// Time in 1/16000s, for conversion between time base of ms to 1/1024s
	ULONG ulTimerInterval;				// Timer interval between simulated events (ms)
	ULONG ulNextCadInterval;			// Time interval until the next cadence event (ms)
	ULONG ulNextSpdInterval;			// Time interval until the next speed event (ms)

	// Cadence
	USHORT usCadEventCount;			// Bike cadence event count (pedal revolutions)
	USHORT usCadTime1024;			// Time of last bike cadence event (1/1024s)
	UCHAR ucMinCadence;				// Minimum bike cadence (rpm)
	UCHAR ucCurCadence;				// Current bike cadence (rpm)
	UCHAR ucMaxCadence;				// Maximum bike cadence (rpm)
	UCHAR ucCadSimDataType;			// Method to generate simulated data
	BOOL bCadSweepAscending;		// Sweep through values in an ascending manner
	
	// Speed
	USHORT usSpdEventCount;			// Bike speed event count (wheel revolutions)
	USHORT usSpdTime1024;			// Time of last bike speed event (1/1024s)
	ULONG ulMinSpeed;				// Minimum bike speed (meters/h)
	ULONG ulCurSpeed;				// Current bike speed (meters/h)
	ULONG ulMaxSpeed;				// Maximum bike speed (meters/h)
	UCHAR ucWheelCircumference;		// Wheel circumference (cm)
	UCHAR ucSpdSimDataType;			// Method to generate simulated data
	BOOL bSpdSweepAscending;		// Sweep through values in an ascending manner

private: System::Windows::Forms::Panel^  panel_Settings;
private: System::Windows::Forms::Panel^  panel_Display;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Sim_CadMaxOutput;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Sim_CadCurOutput;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Sim_CadMinOutput;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Sim_WheelCircumference;
private: System::Windows::Forms::Label^  label_Sim_WheelCircumferenceConst;
private: System::Windows::Forms::CheckBox^  checkBox_Sim_CadSweeping;
private: System::Windows::Forms::Label^  label_Sim_CadMax;
private: System::Windows::Forms::Label^  label_Sim_CadCurrent;
private: System::Windows::Forms::Label^  label_Sim_CadMin;
private: System::Windows::Forms::Label^  label_Sim_CadLabel;
private: System::Windows::Forms::Label^  label_Trn_Title;
private: System::Windows::Forms::Label^  label_Trn_Speed;
private: System::Windows::Forms::Label^  label_Trn_SpeedTimeDisplay;
private: System::Windows::Forms::Label^  label_Trn_CadenceTimeDisplay;
private: System::Windows::Forms::Label^  label_Trn_Time;
private: System::Windows::Forms::Label^  label_Sim_SpdMin;
private: System::Windows::Forms::Label^  label_Sim_SpdCur;
private: System::Windows::Forms::Label^  label_Sim_SpdMax;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Sim_SpdMaxOutput;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Sim_SpdMinOutput;
private: System::Windows::Forms::CheckBox^  checkBox_Sim_SpdSweeping;
private: System::Windows::Forms::Label^  label_Sim_SpdLabel;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Sim_SpdCurOutput;
private: System::Windows::Forms::Label^  label_Trn_SpdCountDisplay;
private: System::Windows::Forms::Label^  label_Trn_CadCountDisplay;
private: System::Windows::Forms::Label^  label_Trn_Count;
private: System::Windows::Forms::Label^  label_Trn_TimeUnit;
private: System::Windows::Forms::Label^  label_Trn_Cadence;

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
			this->label_Sim_SpdMin = (gcnew System::Windows::Forms::Label());
			this->label_Sim_SpdCur = (gcnew System::Windows::Forms::Label());
			this->checkBox_Sim_SpdSweeping = (gcnew System::Windows::Forms::CheckBox());
			this->label_Sim_SpdMax = (gcnew System::Windows::Forms::Label());
			this->numericUpDown_Sim_SpdMaxOutput = (gcnew System::Windows::Forms::NumericUpDown());
			this->numericUpDown_Sim_SpdMinOutput = (gcnew System::Windows::Forms::NumericUpDown());
			this->label_Sim_SpdLabel = (gcnew System::Windows::Forms::Label());
			this->numericUpDown_Sim_SpdCurOutput = (gcnew System::Windows::Forms::NumericUpDown());
			this->numericUpDown_Sim_WheelCircumference = (gcnew System::Windows::Forms::NumericUpDown());
			this->label_Sim_CadMin = (gcnew System::Windows::Forms::Label());
			this->numericUpDown_Sim_CadMaxOutput = (gcnew System::Windows::Forms::NumericUpDown());
			this->numericUpDown_Sim_CadMinOutput = (gcnew System::Windows::Forms::NumericUpDown());
			this->label_Sim_WheelCircumferenceConst = (gcnew System::Windows::Forms::Label());
			this->label_Sim_CadCurrent = (gcnew System::Windows::Forms::Label());
			this->checkBox_Sim_CadSweeping = (gcnew System::Windows::Forms::CheckBox());
			this->label_Sim_CadMax = (gcnew System::Windows::Forms::Label());
			this->label_Sim_CadLabel = (gcnew System::Windows::Forms::Label());
			this->numericUpDown_Sim_CadCurOutput = (gcnew System::Windows::Forms::NumericUpDown());
			this->panel_Display = (gcnew System::Windows::Forms::Panel());
			this->label_Trn_TimeUnit = (gcnew System::Windows::Forms::Label());
			this->label_Trn_SpdCountDisplay = (gcnew System::Windows::Forms::Label());
			this->label_Trn_Speed = (gcnew System::Windows::Forms::Label());
			this->label_Trn_Cadence = (gcnew System::Windows::Forms::Label());
			this->label_Trn_CadCountDisplay = (gcnew System::Windows::Forms::Label());
			this->label_Trn_Count = (gcnew System::Windows::Forms::Label());
			this->label_Trn_SpeedTimeDisplay = (gcnew System::Windows::Forms::Label());
			this->label_Trn_CadenceTimeDisplay = (gcnew System::Windows::Forms::Label());
			this->label_Trn_Time = (gcnew System::Windows::Forms::Label());
			this->label_Trn_Title = (gcnew System::Windows::Forms::Label());
			this->panel_Settings->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_SpdMaxOutput))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_SpdMinOutput))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_SpdCurOutput))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_WheelCircumference))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_CadMaxOutput))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_CadMinOutput))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_CadCurOutput))->BeginInit();
			this->panel_Display->SuspendLayout();
			this->SuspendLayout();
			// 
			// panel_Settings
			// 
			this->panel_Settings->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->panel_Settings->Controls->Add(this->label_Sim_SpdMin);
			this->panel_Settings->Controls->Add(this->label_Sim_SpdCur);
			this->panel_Settings->Controls->Add(this->checkBox_Sim_SpdSweeping);
			this->panel_Settings->Controls->Add(this->label_Sim_SpdMax);
			this->panel_Settings->Controls->Add(this->numericUpDown_Sim_SpdMaxOutput);
			this->panel_Settings->Controls->Add(this->numericUpDown_Sim_SpdMinOutput);
			this->panel_Settings->Controls->Add(this->label_Sim_SpdLabel);
			this->panel_Settings->Controls->Add(this->numericUpDown_Sim_SpdCurOutput);
			this->panel_Settings->Controls->Add(this->numericUpDown_Sim_WheelCircumference);
			this->panel_Settings->Controls->Add(this->label_Sim_CadMin);
			this->panel_Settings->Controls->Add(this->numericUpDown_Sim_CadMaxOutput);
			this->panel_Settings->Controls->Add(this->numericUpDown_Sim_CadMinOutput);
			this->panel_Settings->Controls->Add(this->label_Sim_WheelCircumferenceConst);
			this->panel_Settings->Controls->Add(this->label_Sim_CadCurrent);
			this->panel_Settings->Controls->Add(this->checkBox_Sim_CadSweeping);
			this->panel_Settings->Controls->Add(this->label_Sim_CadMax);
			this->panel_Settings->Controls->Add(this->label_Sim_CadLabel);
			this->panel_Settings->Controls->Add(this->numericUpDown_Sim_CadCurOutput);
			this->panel_Settings->Location = System::Drawing::Point(322, 50);
			this->panel_Settings->Name = L"panel_Settings";
			this->panel_Settings->Size = System::Drawing::Size(400, 140);
			this->panel_Settings->TabIndex = 0;
			// 
			// label_Sim_SpdMin
			// 
			this->label_Sim_SpdMin->AutoSize = true;
			this->label_Sim_SpdMin->Location = System::Drawing::Point(171, 38);
			this->label_Sim_SpdMin->Name = L"label_Sim_SpdMin";
			this->label_Sim_SpdMin->Size = System::Drawing::Size(24, 13);
			this->label_Sim_SpdMin->TabIndex = 49;
			this->label_Sim_SpdMin->Text = L"Min";
			// 
			// label_Sim_SpdCur
			// 
			this->label_Sim_SpdCur->AutoSize = true;
			this->label_Sim_SpdCur->Location = System::Drawing::Point(154, 62);
			this->label_Sim_SpdCur->Name = L"label_Sim_SpdCur";
			this->label_Sim_SpdCur->Size = System::Drawing::Size(41, 13);
			this->label_Sim_SpdCur->TabIndex = 50;
			this->label_Sim_SpdCur->Text = L"Current";
			// 
			// checkBox_Sim_SpdSweeping
			// 
			this->checkBox_Sim_SpdSweeping->AutoSize = true;
			this->checkBox_Sim_SpdSweeping->Location = System::Drawing::Point(189, 109);
			this->checkBox_Sim_SpdSweeping->Name = L"checkBox_Sim_SpdSweeping";
			this->checkBox_Sim_SpdSweeping->Size = System::Drawing::Size(73, 17);
			this->checkBox_Sim_SpdSweeping->TabIndex = 19;
			this->checkBox_Sim_SpdSweeping->Text = L"Sweeping";
			this->checkBox_Sim_SpdSweeping->UseVisualStyleBackColor = true;
			this->checkBox_Sim_SpdSweeping->CheckedChanged += gcnew System::EventHandler(this, &BikeSpdCadSensor::checkBox_Sim_SpeedSweeping_CheckedChanged);
			// 
			// label_Sim_SpdMax
			// 
			this->label_Sim_SpdMax->AutoSize = true;
			this->label_Sim_SpdMax->Location = System::Drawing::Point(168, 86);
			this->label_Sim_SpdMax->Name = L"label_Sim_SpdMax";
			this->label_Sim_SpdMax->Size = System::Drawing::Size(27, 13);
			this->label_Sim_SpdMax->TabIndex = 51;
			this->label_Sim_SpdMax->Text = L"Max";
			// 
			// numericUpDown_Sim_SpdMaxOutput
			// 
			this->numericUpDown_Sim_SpdMaxOutput->DecimalPlaces = 3;
			this->numericUpDown_Sim_SpdMaxOutput->Enabled = false;
			this->numericUpDown_Sim_SpdMaxOutput->Location = System::Drawing::Point(198, 83);
			this->numericUpDown_Sim_SpdMaxOutput->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
			this->numericUpDown_Sim_SpdMaxOutput->Name = L"numericUpDown_Sim_SpdMaxOutput";
			this->numericUpDown_Sim_SpdMaxOutput->Size = System::Drawing::Size(64, 20);
			this->numericUpDown_Sim_SpdMaxOutput->TabIndex = 17;
			this->numericUpDown_Sim_SpdMaxOutput->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->numericUpDown_Sim_SpdMaxOutput->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {80, 0, 0, 0});
			this->numericUpDown_Sim_SpdMaxOutput->ValueChanged += gcnew System::EventHandler(this, &BikeSpdCadSensor::numericUpDown_Sim_SpdMinMaxOutput_ValueChanged);
			// 
			// numericUpDown_Sim_SpdMinOutput
			// 
			this->numericUpDown_Sim_SpdMinOutput->DecimalPlaces = 3;
			this->numericUpDown_Sim_SpdMinOutput->Enabled = false;
			this->numericUpDown_Sim_SpdMinOutput->Location = System::Drawing::Point(198, 35);
			this->numericUpDown_Sim_SpdMinOutput->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
			this->numericUpDown_Sim_SpdMinOutput->Name = L"numericUpDown_Sim_SpdMinOutput";
			this->numericUpDown_Sim_SpdMinOutput->Size = System::Drawing::Size(64, 20);
			this->numericUpDown_Sim_SpdMinOutput->TabIndex = 15;
			this->numericUpDown_Sim_SpdMinOutput->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->numericUpDown_Sim_SpdMinOutput->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {10, 0, 0, 0});
			this->numericUpDown_Sim_SpdMinOutput->ValueChanged += gcnew System::EventHandler(this, &BikeSpdCadSensor::numericUpDown_Sim_SpdMinMaxOutput_ValueChanged);
			// 
			// label_Sim_SpdLabel
			// 
			this->label_Sim_SpdLabel->AutoSize = true;
			this->label_Sim_SpdLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Underline, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->label_Sim_SpdLabel->Location = System::Drawing::Point(199, 14);
			this->label_Sim_SpdLabel->Name = L"label_Sim_SpdLabel";
			this->label_Sim_SpdLabel->Size = System::Drawing::Size(68, 13);
			this->label_Sim_SpdLabel->TabIndex = 43;
			this->label_Sim_SpdLabel->Text = L"Speed (kph):";
			// 
			// numericUpDown_Sim_SpdCurOutput
			// 
			this->numericUpDown_Sim_SpdCurOutput->DecimalPlaces = 3;
			this->numericUpDown_Sim_SpdCurOutput->Location = System::Drawing::Point(198, 59);
			this->numericUpDown_Sim_SpdCurOutput->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
			this->numericUpDown_Sim_SpdCurOutput->Name = L"numericUpDown_Sim_SpdCurOutput";
			this->numericUpDown_Sim_SpdCurOutput->Size = System::Drawing::Size(64, 20);
			this->numericUpDown_Sim_SpdCurOutput->TabIndex = 16;
			this->numericUpDown_Sim_SpdCurOutput->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->numericUpDown_Sim_SpdCurOutput->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {30, 0, 0, 0});
			this->numericUpDown_Sim_SpdCurOutput->ValueChanged += gcnew System::EventHandler(this, &BikeSpdCadSensor::numericUpDown_Sim_SpdCurOutput_ValueChanged);
			// 
			// numericUpDown_Sim_WheelCircumference
			// 
			this->numericUpDown_Sim_WheelCircumference->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {5, 0, 0, 0});
			this->numericUpDown_Sim_WheelCircumference->Location = System::Drawing::Point(311, 55);
			this->numericUpDown_Sim_WheelCircumference->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
			this->numericUpDown_Sim_WheelCircumference->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->numericUpDown_Sim_WheelCircumference->Name = L"numericUpDown_Sim_WheelCircumference";
			this->numericUpDown_Sim_WheelCircumference->Size = System::Drawing::Size(47, 20);
			this->numericUpDown_Sim_WheelCircumference->TabIndex = 20;
			this->numericUpDown_Sim_WheelCircumference->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {207, 0, 0, 0});
			this->numericUpDown_Sim_WheelCircumference->ValueChanged += gcnew System::EventHandler(this, &BikeSpdCadSensor::numericUpDown_Sim_WheelCircumference_ValueChanged);
			// 
			// label_Sim_CadMin
			// 
			this->label_Sim_CadMin->AutoSize = true;
			this->label_Sim_CadMin->Location = System::Drawing::Point(40, 38);
			this->label_Sim_CadMin->Name = L"label_Sim_CadMin";
			this->label_Sim_CadMin->Size = System::Drawing::Size(24, 13);
			this->label_Sim_CadMin->TabIndex = 31;
			this->label_Sim_CadMin->Text = L"Min";
			// 
			// numericUpDown_Sim_CadMaxOutput
			// 
			this->numericUpDown_Sim_CadMaxOutput->Enabled = false;
			this->numericUpDown_Sim_CadMaxOutput->Location = System::Drawing::Point(67, 83);
			this->numericUpDown_Sim_CadMaxOutput->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
			this->numericUpDown_Sim_CadMaxOutput->Name = L"numericUpDown_Sim_CadMaxOutput";
			this->numericUpDown_Sim_CadMaxOutput->Size = System::Drawing::Size(64, 20);
			this->numericUpDown_Sim_CadMaxOutput->TabIndex = 12;
			this->numericUpDown_Sim_CadMaxOutput->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->numericUpDown_Sim_CadMaxOutput->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {120, 0, 0, 0});
			this->numericUpDown_Sim_CadMaxOutput->ValueChanged += gcnew System::EventHandler(this, &BikeSpdCadSensor::numericUpDown_Sim_CadMinMaxOutput_ValueChanged);
			// 
			// numericUpDown_Sim_CadMinOutput
			// 
			this->numericUpDown_Sim_CadMinOutput->Enabled = false;
			this->numericUpDown_Sim_CadMinOutput->Location = System::Drawing::Point(67, 35);
			this->numericUpDown_Sim_CadMinOutput->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
			this->numericUpDown_Sim_CadMinOutput->Name = L"numericUpDown_Sim_CadMinOutput";
			this->numericUpDown_Sim_CadMinOutput->Size = System::Drawing::Size(64, 20);
			this->numericUpDown_Sim_CadMinOutput->TabIndex = 10;
			this->numericUpDown_Sim_CadMinOutput->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->numericUpDown_Sim_CadMinOutput->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {60, 0, 0, 0});
			this->numericUpDown_Sim_CadMinOutput->ValueChanged += gcnew System::EventHandler(this, &BikeSpdCadSensor::numericUpDown_Sim_CadMinMaxOutput_ValueChanged);
			// 
			// label_Sim_WheelCircumferenceConst
			// 
			this->label_Sim_WheelCircumferenceConst->AutoSize = true;
			this->label_Sim_WheelCircumferenceConst->Location = System::Drawing::Point(293, 12);
			this->label_Sim_WheelCircumferenceConst->Name = L"label_Sim_WheelCircumferenceConst";
			this->label_Sim_WheelCircumferenceConst->Size = System::Drawing::Size(78, 39);
			this->label_Sim_WheelCircumferenceConst->TabIndex = 40;
			this->label_Sim_WheelCircumferenceConst->Text = L"Wheel \r\nCircumference \r\n(cm)";
			this->label_Sim_WheelCircumferenceConst->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// label_Sim_CadCurrent
			// 
			this->label_Sim_CadCurrent->AutoSize = true;
			this->label_Sim_CadCurrent->Location = System::Drawing::Point(23, 62);
			this->label_Sim_CadCurrent->Name = L"label_Sim_CadCurrent";
			this->label_Sim_CadCurrent->Size = System::Drawing::Size(41, 13);
			this->label_Sim_CadCurrent->TabIndex = 32;
			this->label_Sim_CadCurrent->Text = L"Current";
			// 
			// checkBox_Sim_CadSweeping
			// 
			this->checkBox_Sim_CadSweeping->AutoSize = true;
			this->checkBox_Sim_CadSweeping->Location = System::Drawing::Point(58, 109);
			this->checkBox_Sim_CadSweeping->Name = L"checkBox_Sim_CadSweeping";
			this->checkBox_Sim_CadSweeping->Size = System::Drawing::Size(73, 17);
			this->checkBox_Sim_CadSweeping->TabIndex = 14;
			this->checkBox_Sim_CadSweeping->Text = L"Sweeping";
			this->checkBox_Sim_CadSweeping->UseVisualStyleBackColor = true;
			this->checkBox_Sim_CadSweeping->CheckedChanged += gcnew System::EventHandler(this, &BikeSpdCadSensor::checkBox_Sim_CadenceSweeping_CheckedChanged);
			// 
			// label_Sim_CadMax
			// 
			this->label_Sim_CadMax->AutoSize = true;
			this->label_Sim_CadMax->Location = System::Drawing::Point(37, 86);
			this->label_Sim_CadMax->Name = L"label_Sim_CadMax";
			this->label_Sim_CadMax->Size = System::Drawing::Size(27, 13);
			this->label_Sim_CadMax->TabIndex = 35;
			this->label_Sim_CadMax->Text = L"Max";
			// 
			// label_Sim_CadLabel
			// 
			this->label_Sim_CadLabel->AutoSize = true;
			this->label_Sim_CadLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Underline, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->label_Sim_CadLabel->Location = System::Drawing::Point(52, 14);
			this->label_Sim_CadLabel->Name = L"label_Sim_CadLabel";
			this->label_Sim_CadLabel->Size = System::Drawing::Size(79, 13);
			this->label_Sim_CadLabel->TabIndex = 29;
			this->label_Sim_CadLabel->Text = L"Cadence (rpm):";
			// 
			// numericUpDown_Sim_CadCurOutput
			// 
			this->numericUpDown_Sim_CadCurOutput->Location = System::Drawing::Point(67, 59);
			this->numericUpDown_Sim_CadCurOutput->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
			this->numericUpDown_Sim_CadCurOutput->Name = L"numericUpDown_Sim_CadCurOutput";
			this->numericUpDown_Sim_CadCurOutput->Size = System::Drawing::Size(64, 20);
			this->numericUpDown_Sim_CadCurOutput->TabIndex = 11;
			this->numericUpDown_Sim_CadCurOutput->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->numericUpDown_Sim_CadCurOutput->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {90, 0, 0, 0});
			this->numericUpDown_Sim_CadCurOutput->ValueChanged += gcnew System::EventHandler(this, &BikeSpdCadSensor::numericUpDown_Sim_CadCurOutput_ValueChanged);
			// 
			// panel_Display
			// 
			this->panel_Display->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->panel_Display->Controls->Add(this->label_Trn_TimeUnit);
			this->panel_Display->Controls->Add(this->label_Trn_SpdCountDisplay);
			this->panel_Display->Controls->Add(this->label_Trn_Speed);
			this->panel_Display->Controls->Add(this->label_Trn_Cadence);
			this->panel_Display->Controls->Add(this->label_Trn_CadCountDisplay);
			this->panel_Display->Controls->Add(this->label_Trn_Count);
			this->panel_Display->Controls->Add(this->label_Trn_SpeedTimeDisplay);
			this->panel_Display->Controls->Add(this->label_Trn_CadenceTimeDisplay);
			this->panel_Display->Controls->Add(this->label_Trn_Time);
			this->panel_Display->Controls->Add(this->label_Trn_Title);
			this->panel_Display->Location = System::Drawing::Point(58, 188);
			this->panel_Display->Name = L"panel_Display";
			this->panel_Display->Size = System::Drawing::Size(200, 90);
			this->panel_Display->TabIndex = 1;
			// 
			// label_Trn_TimeUnit
			// 
			this->label_Trn_TimeUnit->AutoSize = true;
			this->label_Trn_TimeUnit->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 6.25F, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->label_Trn_TimeUnit->Location = System::Drawing::Point(143, 26);
			this->label_Trn_TimeUnit->Name = L"label_Trn_TimeUnit";
			this->label_Trn_TimeUnit->Size = System::Drawing::Size(44, 12);
			this->label_Trn_TimeUnit->TabIndex = 11;
			this->label_Trn_TimeUnit->Text = L"(1/1024s)";
			// 
			// label_Trn_SpdCountDisplay
			// 
			this->label_Trn_SpdCountDisplay->AutoSize = true;
			this->label_Trn_SpdCountDisplay->Location = System::Drawing::Point(71, 62);
			this->label_Trn_SpdCountDisplay->Name = L"label_Trn_SpdCountDisplay";
			this->label_Trn_SpdCountDisplay->Size = System::Drawing::Size(16, 13);
			this->label_Trn_SpdCountDisplay->TabIndex = 10;
			this->label_Trn_SpdCountDisplay->Text = L"---";
			// 
			// label_Trn_Speed
			// 
			this->label_Trn_Speed->AutoSize = true;
			this->label_Trn_Speed->Location = System::Drawing::Point(13, 62);
			this->label_Trn_Speed->Name = L"label_Trn_Speed";
			this->label_Trn_Speed->Size = System::Drawing::Size(41, 13);
			this->label_Trn_Speed->TabIndex = 2;
			this->label_Trn_Speed->Text = L"Speed:";
			// 
			// label_Trn_Cadence
			// 
			this->label_Trn_Cadence->AutoSize = true;
			this->label_Trn_Cadence->Location = System::Drawing::Point(13, 44);
			this->label_Trn_Cadence->Name = L"label_Trn_Cadence";
			this->label_Trn_Cadence->Size = System::Drawing::Size(53, 13);
			this->label_Trn_Cadence->TabIndex = 0;
			this->label_Trn_Cadence->Text = L"Cadence:";
			// 
			// label_Trn_CadCountDisplay
			// 
			this->label_Trn_CadCountDisplay->AutoSize = true;
			this->label_Trn_CadCountDisplay->Location = System::Drawing::Point(71, 44);
			this->label_Trn_CadCountDisplay->Name = L"label_Trn_CadCountDisplay";
			this->label_Trn_CadCountDisplay->Size = System::Drawing::Size(16, 13);
			this->label_Trn_CadCountDisplay->TabIndex = 9;
			this->label_Trn_CadCountDisplay->Text = L"---";
			// 
			// label_Trn_Count
			// 
			this->label_Trn_Count->AutoSize = true;
			this->label_Trn_Count->Location = System::Drawing::Point(66, 26);
			this->label_Trn_Count->Name = L"label_Trn_Count";
			this->label_Trn_Count->Size = System::Drawing::Size(38, 13);
			this->label_Trn_Count->TabIndex = 8;
			this->label_Trn_Count->Text = L"Count:";
			// 
			// label_Trn_SpeedTimeDisplay
			// 
			this->label_Trn_SpeedTimeDisplay->AutoSize = true;
			this->label_Trn_SpeedTimeDisplay->Location = System::Drawing::Point(113, 62);
			this->label_Trn_SpeedTimeDisplay->Name = L"label_Trn_SpeedTimeDisplay";
			this->label_Trn_SpeedTimeDisplay->Size = System::Drawing::Size(16, 13);
			this->label_Trn_SpeedTimeDisplay->TabIndex = 7;
			this->label_Trn_SpeedTimeDisplay->Text = L"---";
			// 
			// label_Trn_CadenceTimeDisplay
			// 
			this->label_Trn_CadenceTimeDisplay->AutoSize = true;
			this->label_Trn_CadenceTimeDisplay->Location = System::Drawing::Point(113, 44);
			this->label_Trn_CadenceTimeDisplay->Name = L"label_Trn_CadenceTimeDisplay";
			this->label_Trn_CadenceTimeDisplay->Size = System::Drawing::Size(16, 13);
			this->label_Trn_CadenceTimeDisplay->TabIndex = 6;
			this->label_Trn_CadenceTimeDisplay->Text = L"---";
			// 
			// label_Trn_Time
			// 
			this->label_Trn_Time->AutoSize = true;
			this->label_Trn_Time->Location = System::Drawing::Point(109, 26);
			this->label_Trn_Time->Name = L"label_Trn_Time";
			this->label_Trn_Time->Size = System::Drawing::Size(33, 13);
			this->label_Trn_Time->TabIndex = 5;
			this->label_Trn_Time->Text = L"Time:";
			// 
			// label_Trn_Title
			// 
			this->label_Trn_Title->AutoSize = true;
			this->label_Trn_Title->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Underline, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->label_Trn_Title->Location = System::Drawing::Point(2, 2);
			this->label_Trn_Title->Name = L"label_Trn_Title";
			this->label_Trn_Title->Size = System::Drawing::Size(160, 13);
			this->label_Trn_Title->TabIndex = 4;
			this->label_Trn_Title->Text = L"Current Speed and Cadence Tx:";
			// 
			// BikeSpdCadSensor
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(786, 317);
			this->Controls->Add(this->panel_Display);
			this->Controls->Add(this->panel_Settings);
			this->Name = L"BikeSpdCadSensor";
			this->Text = L"Bike Speed and Cadence Simulator";
			this->panel_Settings->ResumeLayout(false);
			this->panel_Settings->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_SpdMaxOutput))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_SpdMinOutput))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_SpdCurOutput))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_WheelCircumference))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_CadMaxOutput))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_CadMinOutput))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_CadCurOutput))->EndInit();
			this->panel_Display->ResumeLayout(false);
			this->panel_Display->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion

};