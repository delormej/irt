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


public ref class BikeSpdCadDisplay : public System::Windows::Forms::Form, public ISimBase{
public:
	BikeSpdCadDisplay(){
		InitializeComponent();
		InitializeSim();
	}

	~BikeSpdCadDisplay(){
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
	virtual void ANT_eventNotification(UCHAR ucEventCode_, UCHAR* pucEventBuffer_);
	virtual UCHAR getDeviceType(){return BSC_DEVICE_TYPE;}
	virtual UCHAR getTransmissionType(){return 0;}		// Transmission type wildcarded to ensure future compatibility
	virtual USHORT getTransmitPeriod(){return BSC_MSG_PERIOD;}
	virtual DOUBLE getTimerInterval(){return 3600000;} // Set interval to one hour, so timer events are not frequent (timer should be disabled tho)
	virtual void onTimerTock(USHORT eventTime){} // Do nothing
	virtual System::Windows::Forms::Panel^ getSimSettingsPanel(){return this->panel_Settings;}
	virtual System::Windows::Forms::Panel^ getSimTranslatedDisplay(){return this->panel_Display;}

private:
	void InitializeSim();
	void HandleReceive(UCHAR* pucRxBuffer_);
	void UpdateDisplay();
	System::Void numericUpDown_Sim_WheelCircumference_ValueChanged(System::Object^  sender, System::EventArgs^  e);

private:
	// Implementation specific constants
	static const UCHAR MAX_NO_EVENTS = 12;	// Maximum number of messages with no new events to detect stopping/coasting

	// Simulator variables
	BSCState eState;				// Simulator state
	BOOL bCoast;					// Coasting flag
	BOOL bStop;						// Stopping flag

	// Cadence
	USHORT usCadEventCount;			// Bike cadence event count (pedal revolutions)
	USHORT usCadPreviousEventCount;	// Bike cadence previous event count
	USHORT usCadTime1024;			// Time of last bike cadence event (1/1024s)
	USHORT usCadPreviousTime1024;	// Time of previous bike cadence event (1/1024s)
	ULONG ulCadAcumEventCount;		// Cumulative bike cadence event count (pedal revolutions)
	ULONG ulCadAcumTime1024;		// Cumulative time (1/1024 seconds)
	UCHAR ucCadence;				// Calculated cadence (rpm)
	
	// Speed
	USHORT usSpdEventCount;			// Bike speed event count (wheel revolutions)
	USHORT usSpdPreviousEventCount;	// Bike speed previous event count
	USHORT usSpdTime1024;			// Time of last bike speed event (1/1024s)
	USHORT usSpdPreviousTime1024;	// Time of previous bike speed event (1/1024s)
	ULONG ulSpdAcumEventCount;		// Cumulative bike speed event count (wheel revolutions)
	ULONG ulSpdAcumTime1024;		// Cumulative time (1/1024 seconds)
	ULONG ulSpeed;					// Instantaneous speed (meters/h)
	ULONG ulDistance;				// Cumulative distance (cm)
	UCHAR ucWheelCircumference;		// Wheel circumference (cm)


private: System::Windows::Forms::Panel^  panel_Display;
private: System::Windows::Forms::Label^  label_Trn_Title;
private: System::Windows::Forms::Label^  label_Trn_Speed;
private: System::Windows::Forms::Label^  label_Trn_Cadence;
private: System::Windows::Forms::Label^  label_Trn_SpeedTimeDisplay;
private: System::Windows::Forms::Label^  label_Trn_CadenceTimeDisplay;
private: System::Windows::Forms::Label^  label_Trn_Time;
private: System::Windows::Forms::Label^  label_Trn_SpdCountDisplay;
private: System::Windows::Forms::Label^  label_Trn_CadCountDisplay;
private: System::Windows::Forms::Label^  label_Trn_Count;
private: System::Windows::Forms::Label^  label_Trn_TimeUnit;
private: System::Windows::Forms::Panel^  panel_Settings;
private: System::Windows::Forms::GroupBox^  groupBox_CalculatedData;
private: System::Windows::Forms::Label^  label_Calc_Cadence;
private: System::Windows::Forms::Label^  label_Calc_CadenceDisplay;
private: System::Windows::Forms::Label^  label_Calc_CadEventCountDisplay;
private: System::Windows::Forms::Label^  label_Calc_CadEventCount;
private: System::Windows::Forms::Label^  label_Calc_ElapsedSecsDisplay;
private: System::Windows::Forms::Label^  label_Calc_ElpTime;
private: System::Windows::Forms::Label^  label_Calc_Speed;
private: System::Windows::Forms::Label^  label_Calc_SpeedDisplay;
private: System::Windows::Forms::Label^  label_Calc_SpdEventCountDisplay;
private: System::Windows::Forms::Label^  label_Calc_SpdEventCount;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Sim_WheelCircumference;
private: System::Windows::Forms::Label^  label_Sim_WheelCircumferenceConst;
private: System::Windows::Forms::Label^  label_Calc_DistanceDisplay;
private: System::Windows::Forms::Label^  label2;
private: System::Windows::Forms::Label^  label_Coasting;
private: System::Windows::Forms::Label^  label_Stopped;

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
			this->panel_Display = (gcnew System::Windows::Forms::Panel());
			this->label_Trn_TimeUnit = (gcnew System::Windows::Forms::Label());
			this->label_Trn_SpdCountDisplay = (gcnew System::Windows::Forms::Label());
			this->label_Trn_CadCountDisplay = (gcnew System::Windows::Forms::Label());
			this->label_Trn_Cadence = (gcnew System::Windows::Forms::Label());
			this->label_Trn_Count = (gcnew System::Windows::Forms::Label());
			this->label_Trn_Speed = (gcnew System::Windows::Forms::Label());
			this->label_Trn_SpeedTimeDisplay = (gcnew System::Windows::Forms::Label());
			this->label_Trn_CadenceTimeDisplay = (gcnew System::Windows::Forms::Label());
			this->label_Trn_Time = (gcnew System::Windows::Forms::Label());
			this->label_Trn_Title = (gcnew System::Windows::Forms::Label());
			this->panel_Settings = (gcnew System::Windows::Forms::Panel());
			this->numericUpDown_Sim_WheelCircumference = (gcnew System::Windows::Forms::NumericUpDown());
			this->label_Sim_WheelCircumferenceConst = (gcnew System::Windows::Forms::Label());
			this->groupBox_CalculatedData = (gcnew System::Windows::Forms::GroupBox());
			this->label_Stopped = (gcnew System::Windows::Forms::Label());
			this->label_Coasting = (gcnew System::Windows::Forms::Label());
			this->label_Calc_DistanceDisplay = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label_Calc_Speed = (gcnew System::Windows::Forms::Label());
			this->label_Calc_SpeedDisplay = (gcnew System::Windows::Forms::Label());
			this->label_Calc_SpdEventCountDisplay = (gcnew System::Windows::Forms::Label());
			this->label_Calc_SpdEventCount = (gcnew System::Windows::Forms::Label());
			this->label_Calc_Cadence = (gcnew System::Windows::Forms::Label());
			this->label_Calc_CadenceDisplay = (gcnew System::Windows::Forms::Label());
			this->label_Calc_CadEventCountDisplay = (gcnew System::Windows::Forms::Label());
			this->label_Calc_CadEventCount = (gcnew System::Windows::Forms::Label());
			this->label_Calc_ElapsedSecsDisplay = (gcnew System::Windows::Forms::Label());
			this->label_Calc_ElpTime = (gcnew System::Windows::Forms::Label());
			this->panel_Display->SuspendLayout();
			this->panel_Settings->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_WheelCircumference))->BeginInit();
			this->groupBox_CalculatedData->SuspendLayout();
			this->SuspendLayout();
			// 
			// panel_Display
			// 
			this->panel_Display->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->panel_Display->Controls->Add(this->label_Trn_TimeUnit);
			this->panel_Display->Controls->Add(this->label_Trn_SpdCountDisplay);
			this->panel_Display->Controls->Add(this->label_Trn_CadCountDisplay);
			this->panel_Display->Controls->Add(this->label_Trn_Cadence);
			this->panel_Display->Controls->Add(this->label_Trn_Count);
			this->panel_Display->Controls->Add(this->label_Trn_Speed);
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
			this->label_Trn_TimeUnit->Location = System::Drawing::Point(142, 26);
			this->label_Trn_TimeUnit->Name = L"label_Trn_TimeUnit";
			this->label_Trn_TimeUnit->Size = System::Drawing::Size(44, 12);
			this->label_Trn_TimeUnit->TabIndex = 11;
			this->label_Trn_TimeUnit->Text = L"(1/1024s)";
			// 
			// label_Trn_SpdCountDisplay
			// 
			this->label_Trn_SpdCountDisplay->AutoSize = true;
			this->label_Trn_SpdCountDisplay->Location = System::Drawing::Point(72, 62);
			this->label_Trn_SpdCountDisplay->Name = L"label_Trn_SpdCountDisplay";
			this->label_Trn_SpdCountDisplay->Size = System::Drawing::Size(16, 13);
			this->label_Trn_SpdCountDisplay->TabIndex = 10;
			this->label_Trn_SpdCountDisplay->Text = L"---";
			// 
			// label_Trn_CadCountDisplay
			// 
			this->label_Trn_CadCountDisplay->AutoSize = true;
			this->label_Trn_CadCountDisplay->Location = System::Drawing::Point(72, 44);
			this->label_Trn_CadCountDisplay->Name = L"label_Trn_CadCountDisplay";
			this->label_Trn_CadCountDisplay->Size = System::Drawing::Size(16, 13);
			this->label_Trn_CadCountDisplay->TabIndex = 9;
			this->label_Trn_CadCountDisplay->Text = L"---";
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
			// label_Trn_Count
			// 
			this->label_Trn_Count->AutoSize = true;
			this->label_Trn_Count->Location = System::Drawing::Point(67, 26);
			this->label_Trn_Count->Name = L"label_Trn_Count";
			this->label_Trn_Count->Size = System::Drawing::Size(38, 13);
			this->label_Trn_Count->TabIndex = 8;
			this->label_Trn_Count->Text = L"Count:";
			// 
			// label_Trn_Speed
			// 
			this->label_Trn_Speed->AutoSize = true;
			this->label_Trn_Speed->Location = System::Drawing::Point(25, 62);
			this->label_Trn_Speed->Name = L"label_Trn_Speed";
			this->label_Trn_Speed->Size = System::Drawing::Size(41, 13);
			this->label_Trn_Speed->TabIndex = 2;
			this->label_Trn_Speed->Text = L"Speed:";
			// 
			// label_Trn_SpeedTimeDisplay
			// 
			this->label_Trn_SpeedTimeDisplay->AutoSize = true;
			this->label_Trn_SpeedTimeDisplay->Location = System::Drawing::Point(114, 62);
			this->label_Trn_SpeedTimeDisplay->Name = L"label_Trn_SpeedTimeDisplay";
			this->label_Trn_SpeedTimeDisplay->Size = System::Drawing::Size(16, 13);
			this->label_Trn_SpeedTimeDisplay->TabIndex = 7;
			this->label_Trn_SpeedTimeDisplay->Text = L"---";
			// 
			// label_Trn_CadenceTimeDisplay
			// 
			this->label_Trn_CadenceTimeDisplay->AutoSize = true;
			this->label_Trn_CadenceTimeDisplay->Location = System::Drawing::Point(114, 44);
			this->label_Trn_CadenceTimeDisplay->Name = L"label_Trn_CadenceTimeDisplay";
			this->label_Trn_CadenceTimeDisplay->Size = System::Drawing::Size(16, 13);
			this->label_Trn_CadenceTimeDisplay->TabIndex = 6;
			this->label_Trn_CadenceTimeDisplay->Text = L"---";
			// 
			// label_Trn_Time
			// 
			this->label_Trn_Time->AutoSize = true;
			this->label_Trn_Time->Location = System::Drawing::Point(110, 26);
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
			// panel_Settings
			// 
			this->panel_Settings->Controls->Add(this->numericUpDown_Sim_WheelCircumference);
			this->panel_Settings->Controls->Add(this->label_Sim_WheelCircumferenceConst);
			this->panel_Settings->Controls->Add(this->groupBox_CalculatedData);
			this->panel_Settings->Location = System::Drawing::Point(322, 50);
			this->panel_Settings->Name = L"panel_Settings";
			this->panel_Settings->Size = System::Drawing::Size(400, 140);
			this->panel_Settings->TabIndex = 2;
			// 
			// numericUpDown_Sim_WheelCircumference
			// 
			this->numericUpDown_Sim_WheelCircumference->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {5, 0, 0, 0});
			this->numericUpDown_Sim_WheelCircumference->Location = System::Drawing::Point(332, 115);
			this->numericUpDown_Sim_WheelCircumference->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {500, 0, 0, 0});
			this->numericUpDown_Sim_WheelCircumference->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->numericUpDown_Sim_WheelCircumference->Name = L"numericUpDown_Sim_WheelCircumference";
			this->numericUpDown_Sim_WheelCircumference->Size = System::Drawing::Size(47, 20);
			this->numericUpDown_Sim_WheelCircumference->TabIndex = 56;
			this->numericUpDown_Sim_WheelCircumference->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {207, 0, 0, 0});
			this->numericUpDown_Sim_WheelCircumference->ValueChanged += gcnew System::EventHandler(this, &BikeSpdCadDisplay::numericUpDown_Sim_WheelCircumference_ValueChanged);
			// 
			// label_Sim_WheelCircumferenceConst
			// 
			this->label_Sim_WheelCircumferenceConst->AutoSize = true;
			this->label_Sim_WheelCircumferenceConst->Location = System::Drawing::Point(197, 117);
			this->label_Sim_WheelCircumferenceConst->Name = L"label_Sim_WheelCircumferenceConst";
			this->label_Sim_WheelCircumferenceConst->Size = System::Drawing::Size(132, 13);
			this->label_Sim_WheelCircumferenceConst->TabIndex = 57;
			this->label_Sim_WheelCircumferenceConst->Text = L"Wheel Circumference (cm)";
			// 
			// groupBox_CalculatedData
			// 
			this->groupBox_CalculatedData->Controls->Add(this->label_Stopped);
			this->groupBox_CalculatedData->Controls->Add(this->label_Coasting);
			this->groupBox_CalculatedData->Controls->Add(this->label_Calc_DistanceDisplay);
			this->groupBox_CalculatedData->Controls->Add(this->label2);
			this->groupBox_CalculatedData->Controls->Add(this->label_Calc_Speed);
			this->groupBox_CalculatedData->Controls->Add(this->label_Calc_SpeedDisplay);
			this->groupBox_CalculatedData->Controls->Add(this->label_Calc_SpdEventCountDisplay);
			this->groupBox_CalculatedData->Controls->Add(this->label_Calc_SpdEventCount);
			this->groupBox_CalculatedData->Controls->Add(this->label_Calc_Cadence);
			this->groupBox_CalculatedData->Controls->Add(this->label_Calc_CadenceDisplay);
			this->groupBox_CalculatedData->Controls->Add(this->label_Calc_CadEventCountDisplay);
			this->groupBox_CalculatedData->Controls->Add(this->label_Calc_CadEventCount);
			this->groupBox_CalculatedData->Controls->Add(this->label_Calc_ElapsedSecsDisplay);
			this->groupBox_CalculatedData->Controls->Add(this->label_Calc_ElpTime);
			this->groupBox_CalculatedData->Location = System::Drawing::Point(3, 3);
			this->groupBox_CalculatedData->Name = L"groupBox_CalculatedData";
			this->groupBox_CalculatedData->Size = System::Drawing::Size(394, 109);
			this->groupBox_CalculatedData->TabIndex = 17;
			this->groupBox_CalculatedData->TabStop = false;
			this->groupBox_CalculatedData->Text = L"Calculated Data";
			// 
			// label_Stopped
			// 
			this->label_Stopped->AutoSize = true;
			this->label_Stopped->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label_Stopped->ForeColor = System::Drawing::Color::Red;
			this->label_Stopped->Location = System::Drawing::Point(322, 16);
			this->label_Stopped->Name = L"label_Stopped";
			this->label_Stopped->Size = System::Drawing::Size(54, 13);
			this->label_Stopped->TabIndex = 20;
			this->label_Stopped->Text = L"Stopped";
			// 
			// label_Coasting
			// 
			this->label_Coasting->AutoSize = true;
			this->label_Coasting->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label_Coasting->ForeColor = System::Drawing::Color::Red;
			this->label_Coasting->Location = System::Drawing::Point(129, 16);
			this->label_Coasting->Name = L"label_Coasting";
			this->label_Coasting->Size = System::Drawing::Size(56, 13);
			this->label_Coasting->TabIndex = 22;
			this->label_Coasting->Text = L"Coasting";
			// 
			// label_Calc_DistanceDisplay
			// 
			this->label_Calc_DistanceDisplay->AutoSize = true;
			this->label_Calc_DistanceDisplay->Location = System::Drawing::Point(322, 83);
			this->label_Calc_DistanceDisplay->Name = L"label_Calc_DistanceDisplay";
			this->label_Calc_DistanceDisplay->Size = System::Drawing::Size(16, 13);
			this->label_Calc_DistanceDisplay->TabIndex = 15;
			this->label_Calc_DistanceDisplay->Text = L"---";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(241, 83);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(75, 13);
			this->label2->TabIndex = 14;
			this->label2->Text = L"Distance (km):";
			// 
			// label_Calc_Speed
			// 
			this->label_Calc_Speed->AutoSize = true;
			this->label_Calc_Speed->Location = System::Drawing::Point(248, 37);
			this->label_Calc_Speed->Name = L"label_Calc_Speed";
			this->label_Calc_Speed->Size = System::Drawing::Size(68, 13);
			this->label_Calc_Speed->TabIndex = 11;
			this->label_Calc_Speed->Text = L"Speed (kph):";
			// 
			// label_Calc_SpeedDisplay
			// 
			this->label_Calc_SpeedDisplay->AutoSize = true;
			this->label_Calc_SpeedDisplay->Location = System::Drawing::Point(322, 37);
			this->label_Calc_SpeedDisplay->Name = L"label_Calc_SpeedDisplay";
			this->label_Calc_SpeedDisplay->Size = System::Drawing::Size(16, 13);
			this->label_Calc_SpeedDisplay->TabIndex = 13;
			this->label_Calc_SpeedDisplay->Text = L"---";
			// 
			// label_Calc_SpdEventCountDisplay
			// 
			this->label_Calc_SpdEventCountDisplay->AutoSize = true;
			this->label_Calc_SpdEventCountDisplay->Location = System::Drawing::Point(322, 62);
			this->label_Calc_SpdEventCountDisplay->Name = L"label_Calc_SpdEventCountDisplay";
			this->label_Calc_SpdEventCountDisplay->Size = System::Drawing::Size(16, 13);
			this->label_Calc_SpdEventCountDisplay->TabIndex = 12;
			this->label_Calc_SpdEventCountDisplay->Text = L"---";
			// 
			// label_Calc_SpdEventCount
			// 
			this->label_Calc_SpdEventCount->AutoSize = true;
			this->label_Calc_SpdEventCount->Location = System::Drawing::Point(213, 62);
			this->label_Calc_SpdEventCount->Name = L"label_Calc_SpdEventCount";
			this->label_Calc_SpdEventCount->Size = System::Drawing::Size(103, 13);
			this->label_Calc_SpdEventCount->TabIndex = 9;
			this->label_Calc_SpdEventCount->Text = L"Speed Event Count:";
			// 
			// label_Calc_Cadence
			// 
			this->label_Calc_Cadence->AutoSize = true;
			this->label_Calc_Cadence->Location = System::Drawing::Point(37, 37);
			this->label_Calc_Cadence->Name = L"label_Calc_Cadence";
			this->label_Calc_Cadence->Size = System::Drawing::Size(86, 13);
			this->label_Calc_Cadence->TabIndex = 1;
			this->label_Calc_Cadence->Text = L"Cadence (RPM):";
			// 
			// label_Calc_CadenceDisplay
			// 
			this->label_Calc_CadenceDisplay->AutoSize = true;
			this->label_Calc_CadenceDisplay->Location = System::Drawing::Point(129, 37);
			this->label_Calc_CadenceDisplay->Name = L"label_Calc_CadenceDisplay";
			this->label_Calc_CadenceDisplay->Size = System::Drawing::Size(16, 13);
			this->label_Calc_CadenceDisplay->TabIndex = 3;
			this->label_Calc_CadenceDisplay->Text = L"---";
			// 
			// label_Calc_CadEventCountDisplay
			// 
			this->label_Calc_CadEventCountDisplay->AutoSize = true;
			this->label_Calc_CadEventCountDisplay->Location = System::Drawing::Point(129, 62);
			this->label_Calc_CadEventCountDisplay->Name = L"label_Calc_CadEventCountDisplay";
			this->label_Calc_CadEventCountDisplay->Size = System::Drawing::Size(16, 13);
			this->label_Calc_CadEventCountDisplay->TabIndex = 2;
			this->label_Calc_CadEventCountDisplay->Text = L"---";
			// 
			// label_Calc_CadEventCount
			// 
			this->label_Calc_CadEventCount->AutoSize = true;
			this->label_Calc_CadEventCount->Location = System::Drawing::Point(8, 62);
			this->label_Calc_CadEventCount->Name = L"label_Calc_CadEventCount";
			this->label_Calc_CadEventCount->Size = System::Drawing::Size(115, 13);
			this->label_Calc_CadEventCount->TabIndex = 0;
			this->label_Calc_CadEventCount->Text = L"Cadence Event Count:";
			// 
			// label_Calc_ElapsedSecsDisplay
			// 
			this->label_Calc_ElapsedSecsDisplay->AutoSize = true;
			this->label_Calc_ElapsedSecsDisplay->Location = System::Drawing::Point(129, 83);
			this->label_Calc_ElapsedSecsDisplay->Name = L"label_Calc_ElapsedSecsDisplay";
			this->label_Calc_ElapsedSecsDisplay->Size = System::Drawing::Size(16, 13);
			this->label_Calc_ElapsedSecsDisplay->TabIndex = 7;
			this->label_Calc_ElapsedSecsDisplay->Text = L"---";
			// 
			// label_Calc_ElpTime
			// 
			this->label_Calc_ElpTime->AutoSize = true;
			this->label_Calc_ElpTime->Location = System::Drawing::Point(8, 83);
			this->label_Calc_ElpTime->Name = L"label_Calc_ElpTime";
			this->label_Calc_ElpTime->Size = System::Drawing::Size(115, 13);
			this->label_Calc_ElpTime->TabIndex = 0;
			this->label_Calc_ElpTime->Text = L"Total Elapsed Time (s):";
			// 
			// BikeSpdCadDisplay
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(786, 317);
			this->Controls->Add(this->panel_Settings);
			this->Controls->Add(this->panel_Display);
			this->Name = L"BikeSpdCadDisplay";
			this->Text = L"Bike Speed and Cadence Simulator";
			this->panel_Display->ResumeLayout(false);
			this->panel_Display->PerformLayout();
			this->panel_Settings->ResumeLayout(false);
			this->panel_Settings->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Sim_WheelCircumference))->EndInit();
			this->groupBox_CalculatedData->ResumeLayout(false);
			this->groupBox_CalculatedData->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion

};