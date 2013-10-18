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


public ref class HRMDisplay : public System::Windows::Forms::Form, public ISimBase{
public:
	HRMDisplay(){
		InitializeComponent();
		InitializeSim();
	}

	~HRMDisplay(){
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
	virtual UCHAR getDeviceType(){return HRM_DEVICE_TYPE;}
	virtual UCHAR getTransmissionType(){return 0;}	// Transmission type on receiver must be set to 0 for pairing search and future compatibility
	virtual USHORT getTransmitPeriod(){return HRM_MSG_PERIOD;}
	virtual DOUBLE getTimerInterval(){return 3600000;} // Set interval to one hour, so timer events are not frequent (timer should be disabled tho)
	virtual void onTimerTock(USHORT eventTime){} // Do nothing
	virtual System::Windows::Forms::Panel^ getSimSettingsPanel(){return this->panel_Settings;}
	virtual System::Windows::Forms::Panel^ getSimTranslatedDisplay(){return this->panel_Display;}

private:
	void InitializeSim();
	void HandleReceive(UCHAR* pucRxBuffer_);
	void UpdateDisplay(UCHAR ucPageNum_);

private:
	// Implementation specific constants
	static const UCHAR MAX_NO_EVENTS = 12;	// Maximum number of messages with no new events (3s)

	// Simulator Variables
	UCHAR ucBPM;					   // Heart rate (bpm)
	UCHAR ucEventCount;				// Heart beat event count
	UCHAR ucPreviousEventCount;	// Previous heart beat event count	
	USHORT usTime1024;				// Time of last heart beat event (1/1024 seconds)
	USHORT usPreviousTime1024;		// Time of previous heart beat event (1/1024 seconds)
	ULONG ulElapsedTime2;			// Cumulative operating time (elapsed time since battery insertion) in 2 second resolution	
	UCHAR ucStatePage;				// Track if advanced data is supported

	// Calculated data
	ULONG ulAcumEventCount;			// Cumulative heart beat event count
	ULONG ulAcumTime1024;			// Cumulative time (1/1024 seconds), conversion to s is performed for data display
	USHORT usR_RInterval1024;		// R-R interval (1/1024 seconds), conversion to ms is performed for data display
	UCHAR ucCalcBPM;				   // Calculated heart rate (bpm), from event time

	// Background Data
	UCHAR ucMfgID;					   // Manufacturing ID
	UCHAR ucHwVersion;				// Hardware version
	UCHAR ucSwVersion;				// Software version
	UCHAR ucModelNum;				   // Model number
	USHORT usSerialNum;				// Serial number

private: System::Windows::Forms::Panel^  panel_Display;
private: System::Windows::Forms::Label^  label_Trn_EventCountDisplay;
private: System::Windows::Forms::Label^  label_Trn_EventCount;
private: System::Windows::Forms::Label^  label_Trn_TimeDisplay;
private: System::Windows::Forms::Label^  label_Trn_Time;
private: System::Windows::Forms::Label^  label_Trn_TranslatedDisplayLabel;
private: System::Windows::Forms::Panel^  panel_Settings;
private: System::Windows::Forms::GroupBox^  groupBox2;
private: System::Windows::Forms::Label^  label_Glb_BattTimeDisplay;
private: System::Windows::Forms::Label^  label_Glb_BattTime;
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
private: System::Windows::Forms::GroupBox^  groupBox_CalculatedData;
private: System::Windows::Forms::Label^  label_Calc_waitToggle;
private: System::Windows::Forms::Label^  label_Calc_Pulse;
private: System::Windows::Forms::Label^  label_Calc_PulseDisplay;
private: System::Windows::Forms::Label^  label_Calc_TotEventCountDisplay;
private: System::Windows::Forms::Label^  label_Calc_TotEventCount;
private: System::Windows::Forms::Label^  label_Calc_ElapsedSecsDisplay;
private: System::Windows::Forms::Label^  label_Calc_ElpTime;
private: System::Windows::Forms::Label^  label_Trn_RawPulse;
private: System::Windows::Forms::Label^  label_Trn_RawPulseDisplay;
private: System::Windows::Forms::Label^  label_Trn_LastEventTimeDisplay;
private: System::Windows::Forms::Label^  label_Trn_LastEventTime;
private: System::Windows::Forms::Label^  label_Calc_RRInterval;
private: System::Windows::Forms::Label^  label_Calc_RRIntervalDisplay;
private: System::Windows::Forms::Label^  label_Calc_InstPulseDisplay;
private: System::Windows::Forms::Label^  label_Calc_InstPulse;

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
		this->label_Trn_LastEventTimeDisplay = (gcnew System::Windows::Forms::Label());
		this->label_Trn_LastEventTime = (gcnew System::Windows::Forms::Label());
		this->label_Trn_RawPulseDisplay = (gcnew System::Windows::Forms::Label());
		this->label_Trn_RawPulse = (gcnew System::Windows::Forms::Label());
		this->label_Trn_EventCountDisplay = (gcnew System::Windows::Forms::Label());
		this->label_Trn_EventCount = (gcnew System::Windows::Forms::Label());
		this->label_Trn_TimeDisplay = (gcnew System::Windows::Forms::Label());
		this->label_Trn_Time = (gcnew System::Windows::Forms::Label());
		this->label_Trn_TranslatedDisplayLabel = (gcnew System::Windows::Forms::Label());
		this->panel_Settings = (gcnew System::Windows::Forms::Panel());
		this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
		this->label_Glb_BattTimeDisplay = (gcnew System::Windows::Forms::Label());
		this->label_Glb_BattTime = (gcnew System::Windows::Forms::Label());
		this->label_Glb_ModelNum = (gcnew System::Windows::Forms::Label());
		this->label_Glb_SoftwareVer = (gcnew System::Windows::Forms::Label());
		this->label_Glb_ModelNumDisplay = (gcnew System::Windows::Forms::Label());
		this->label_Glb_ManfIDDisplay = (gcnew System::Windows::Forms::Label());
		this->label_Glb_SerialNumDisplay = (gcnew System::Windows::Forms::Label());
		this->label_Glb_HardwareVerDisplay = (gcnew System::Windows::Forms::Label());
		this->label_Glb_HardwareVer = (gcnew System::Windows::Forms::Label());
		this->label_Glb_SerialNum = (gcnew System::Windows::Forms::Label());
		this->label_Glb_SoftwareVerDisplay = (gcnew System::Windows::Forms::Label());
		this->label_Glb_ManfID = (gcnew System::Windows::Forms::Label());
		this->groupBox_CalculatedData = (gcnew System::Windows::Forms::GroupBox());
		this->label_Calc_InstPulseDisplay = (gcnew System::Windows::Forms::Label());
		this->label_Calc_InstPulse = (gcnew System::Windows::Forms::Label());
		this->label_Calc_RRInterval = (gcnew System::Windows::Forms::Label());
		this->label_Calc_RRIntervalDisplay = (gcnew System::Windows::Forms::Label());
		this->label_Calc_waitToggle = (gcnew System::Windows::Forms::Label());
		this->label_Calc_Pulse = (gcnew System::Windows::Forms::Label());
		this->label_Calc_PulseDisplay = (gcnew System::Windows::Forms::Label());
		this->label_Calc_TotEventCountDisplay = (gcnew System::Windows::Forms::Label());
		this->label_Calc_TotEventCount = (gcnew System::Windows::Forms::Label());
		this->label_Calc_ElapsedSecsDisplay = (gcnew System::Windows::Forms::Label());
		this->label_Calc_ElpTime = (gcnew System::Windows::Forms::Label());
		this->panel_Display->SuspendLayout();
		this->panel_Settings->SuspendLayout();
		this->groupBox2->SuspendLayout();
		this->groupBox_CalculatedData->SuspendLayout();
		this->SuspendLayout();
		// 
		// panel_Display
		// 
		this->panel_Display->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
		this->panel_Display->Controls->Add(this->label_Trn_LastEventTimeDisplay);
		this->panel_Display->Controls->Add(this->label_Trn_LastEventTime);
		this->panel_Display->Controls->Add(this->label_Trn_RawPulseDisplay);
		this->panel_Display->Controls->Add(this->label_Trn_RawPulse);
		this->panel_Display->Controls->Add(this->label_Trn_EventCountDisplay);
		this->panel_Display->Controls->Add(this->label_Trn_EventCount);
		this->panel_Display->Controls->Add(this->label_Trn_TimeDisplay);
		this->panel_Display->Controls->Add(this->label_Trn_Time);
		this->panel_Display->Controls->Add(this->label_Trn_TranslatedDisplayLabel);
		this->panel_Display->Location = System::Drawing::Point(65, 199);
		this->panel_Display->Name = L"panel_Display";
		this->panel_Display->Size = System::Drawing::Size(200, 90);
		this->panel_Display->TabIndex = 3;
		// 
		// label_Trn_LastEventTimeDisplay
		// 
		this->label_Trn_LastEventTimeDisplay->AutoSize = true;
		this->label_Trn_LastEventTimeDisplay->Location = System::Drawing::Point(144, 70);
		this->label_Trn_LastEventTimeDisplay->Name = L"label_Trn_LastEventTimeDisplay";
		this->label_Trn_LastEventTimeDisplay->Size = System::Drawing::Size(16, 13);
		this->label_Trn_LastEventTimeDisplay->TabIndex = 12;
		this->label_Trn_LastEventTimeDisplay->Text = L"---";
		// 
		// label_Trn_LastEventTime
		// 
		this->label_Trn_LastEventTime->AutoSize = true;
		this->label_Trn_LastEventTime->Location = System::Drawing::Point(2, 70);
		this->label_Trn_LastEventTime->Name = L"label_Trn_LastEventTime";
		this->label_Trn_LastEventTime->Size = System::Drawing::Size(136, 13);
		this->label_Trn_LastEventTime->TabIndex = 11;
		this->label_Trn_LastEventTime->Text = L"Last Event Time (1/1024s):";
		// 
		// label_Trn_RawPulseDisplay
		// 
		this->label_Trn_RawPulseDisplay->AutoSize = true;
		this->label_Trn_RawPulseDisplay->Location = System::Drawing::Point(144, 38);
		this->label_Trn_RawPulseDisplay->Name = L"label_Trn_RawPulseDisplay";
		this->label_Trn_RawPulseDisplay->Size = System::Drawing::Size(16, 13);
		this->label_Trn_RawPulseDisplay->TabIndex = 10;
		this->label_Trn_RawPulseDisplay->Text = L"---";
		// 
		// label_Trn_RawPulse
		// 
		this->label_Trn_RawPulse->AutoSize = true;
		this->label_Trn_RawPulse->Location = System::Drawing::Point(32, 38);
		this->label_Trn_RawPulse->Name = L"label_Trn_RawPulse";
		this->label_Trn_RawPulse->Size = System::Drawing::Size(106, 13);
		this->label_Trn_RawPulse->TabIndex = 9;
		this->label_Trn_RawPulse->Text = L"Instantaneous Pulse:";
		// 
		// label_Trn_EventCountDisplay
		// 
		this->label_Trn_EventCountDisplay->AutoSize = true;
		this->label_Trn_EventCountDisplay->Location = System::Drawing::Point(144, 22);
		this->label_Trn_EventCountDisplay->Name = L"label_Trn_EventCountDisplay";
		this->label_Trn_EventCountDisplay->Size = System::Drawing::Size(16, 13);
		this->label_Trn_EventCountDisplay->TabIndex = 8;
		this->label_Trn_EventCountDisplay->Text = L"---";
		// 
		// label_Trn_EventCount
		// 
		this->label_Trn_EventCount->AutoSize = true;
		this->label_Trn_EventCount->Location = System::Drawing::Point(69, 22);
		this->label_Trn_EventCount->Name = L"label_Trn_EventCount";
		this->label_Trn_EventCount->Size = System::Drawing::Size(69, 13);
		this->label_Trn_EventCount->TabIndex = 7;
		this->label_Trn_EventCount->Text = L"Event Count:";
		// 
		// label_Trn_TimeDisplay
		// 
		this->label_Trn_TimeDisplay->AutoSize = true;
		this->label_Trn_TimeDisplay->Location = System::Drawing::Point(144, 54);
		this->label_Trn_TimeDisplay->Name = L"label_Trn_TimeDisplay";
		this->label_Trn_TimeDisplay->Size = System::Drawing::Size(16, 13);
		this->label_Trn_TimeDisplay->TabIndex = 6;
		this->label_Trn_TimeDisplay->Text = L"---";
		// 
		// label_Trn_Time
		// 
		this->label_Trn_Time->AutoSize = true;
		this->label_Trn_Time->Location = System::Drawing::Point(25, 54);
		this->label_Trn_Time->Name = L"label_Trn_Time";
		this->label_Trn_Time->Size = System::Drawing::Size(113, 13);
		this->label_Trn_Time->TabIndex = 5;
		this->label_Trn_Time->Text = L"Event Time (1/1024s):";
		// 
		// label_Trn_TranslatedDisplayLabel
		// 
		this->label_Trn_TranslatedDisplayLabel->AutoSize = true;
		this->label_Trn_TranslatedDisplayLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Underline, 
			System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
		this->label_Trn_TranslatedDisplayLabel->Location = System::Drawing::Point(2, 2);
		this->label_Trn_TranslatedDisplayLabel->Name = L"label_Trn_TranslatedDisplayLabel";
		this->label_Trn_TranslatedDisplayLabel->Size = System::Drawing::Size(166, 13);
		this->label_Trn_TranslatedDisplayLabel->TabIndex = 4;
		this->label_Trn_TranslatedDisplayLabel->Text = L"Current Heart Rate Transmission :";
		// 
		// panel_Settings
		// 
		this->panel_Settings->Controls->Add(this->groupBox2);
		this->panel_Settings->Controls->Add(this->groupBox_CalculatedData);
		this->panel_Settings->Location = System::Drawing::Point(329, 61);
		this->panel_Settings->Name = L"panel_Settings";
		this->panel_Settings->Size = System::Drawing::Size(400, 140);
		this->panel_Settings->TabIndex = 2;
		// 
		// groupBox2
		// 
		this->groupBox2->Controls->Add(this->label_Glb_BattTimeDisplay);
		this->groupBox2->Controls->Add(this->label_Glb_BattTime);
		this->groupBox2->Controls->Add(this->label_Glb_ModelNum);
		this->groupBox2->Controls->Add(this->label_Glb_SoftwareVer);
		this->groupBox2->Controls->Add(this->label_Glb_ModelNumDisplay);
		this->groupBox2->Controls->Add(this->label_Glb_ManfIDDisplay);
		this->groupBox2->Controls->Add(this->label_Glb_SerialNumDisplay);
		this->groupBox2->Controls->Add(this->label_Glb_HardwareVerDisplay);
		this->groupBox2->Controls->Add(this->label_Glb_HardwareVer);
		this->groupBox2->Controls->Add(this->label_Glb_SerialNum);
		this->groupBox2->Controls->Add(this->label_Glb_SoftwareVerDisplay);
		this->groupBox2->Controls->Add(this->label_Glb_ManfID);
		this->groupBox2->Location = System::Drawing::Point(241, 3);
		this->groupBox2->Name = L"groupBox2";
		this->groupBox2->Size = System::Drawing::Size(156, 130);
		this->groupBox2->TabIndex = 18;
		this->groupBox2->TabStop = false;
		this->groupBox2->Text = L"Global Data";
		// 
		// label_Glb_BattTimeDisplay
		// 
		this->label_Glb_BattTimeDisplay->AutoSize = true;
		this->label_Glb_BattTimeDisplay->Location = System::Drawing::Point(78, 109);
		this->label_Glb_BattTimeDisplay->Name = L"label_Glb_BattTimeDisplay";
		this->label_Glb_BattTimeDisplay->Size = System::Drawing::Size(16, 13);
		this->label_Glb_BattTimeDisplay->TabIndex = 18;
		this->label_Glb_BattTimeDisplay->Text = L"---";
		// 
		// label_Glb_BattTime
		// 
		this->label_Glb_BattTime->AutoSize = true;
		this->label_Glb_BattTime->Location = System::Drawing::Point(11, 109);
		this->label_Glb_BattTime->Name = L"label_Glb_BattTime";
		this->label_Glb_BattTime->Size = System::Drawing::Size(69, 13);
		this->label_Glb_BattTime->TabIndex = 17;
		this->label_Glb_BattTime->Text = L"Battery Time:";
		// 
		// label_Glb_ModelNum
		// 
		this->label_Glb_ModelNum->AutoSize = true;
		this->label_Glb_ModelNum->Location = System::Drawing::Point(31, 55);
		this->label_Glb_ModelNum->Name = L"label_Glb_ModelNum";
		this->label_Glb_ModelNum->Size = System::Drawing::Size(49, 13);
		this->label_Glb_ModelNum->TabIndex = 5;
		this->label_Glb_ModelNum->Text = L"Model #:";
		// 
		// label_Glb_SoftwareVer
		// 
		this->label_Glb_SoftwareVer->AutoSize = true;
		this->label_Glb_SoftwareVer->Location = System::Drawing::Point(9, 91);
		this->label_Glb_SoftwareVer->Name = L"label_Glb_SoftwareVer";
		this->label_Glb_SoftwareVer->Size = System::Drawing::Size(71, 13);
		this->label_Glb_SoftwareVer->TabIndex = 4;
		this->label_Glb_SoftwareVer->Text = L"Software Ver:";
		// 
		// label_Glb_ModelNumDisplay
		// 
		this->label_Glb_ModelNumDisplay->AutoSize = true;
		this->label_Glb_ModelNumDisplay->Location = System::Drawing::Point(78, 55);
		this->label_Glb_ModelNumDisplay->Name = L"label_Glb_ModelNumDisplay";
		this->label_Glb_ModelNumDisplay->Size = System::Drawing::Size(16, 13);
		this->label_Glb_ModelNumDisplay->TabIndex = 9;
		this->label_Glb_ModelNumDisplay->Text = L"---";
		// 
		// label_Glb_ManfIDDisplay
		// 
		this->label_Glb_ManfIDDisplay->AutoSize = true;
		this->label_Glb_ManfIDDisplay->Location = System::Drawing::Point(78, 38);
		this->label_Glb_ManfIDDisplay->Name = L"label_Glb_ManfIDDisplay";
		this->label_Glb_ManfIDDisplay->Size = System::Drawing::Size(16, 13);
		this->label_Glb_ManfIDDisplay->TabIndex = 8;
		this->label_Glb_ManfIDDisplay->Text = L"---";
		// 
		// label_Glb_SerialNumDisplay
		// 
		this->label_Glb_SerialNumDisplay->AutoSize = true;
		this->label_Glb_SerialNumDisplay->Location = System::Drawing::Point(78, 19);
		this->label_Glb_SerialNumDisplay->Name = L"label_Glb_SerialNumDisplay";
		this->label_Glb_SerialNumDisplay->Size = System::Drawing::Size(16, 13);
		this->label_Glb_SerialNumDisplay->TabIndex = 11;
		this->label_Glb_SerialNumDisplay->Text = L"---";
		// 
		// label_Glb_HardwareVerDisplay
		// 
		this->label_Glb_HardwareVerDisplay->AutoSize = true;
		this->label_Glb_HardwareVerDisplay->Location = System::Drawing::Point(78, 73);
		this->label_Glb_HardwareVerDisplay->Name = L"label_Glb_HardwareVerDisplay";
		this->label_Glb_HardwareVerDisplay->Size = System::Drawing::Size(16, 13);
		this->label_Glb_HardwareVerDisplay->TabIndex = 14;
		this->label_Glb_HardwareVerDisplay->Text = L"---";
		// 
		// label_Glb_HardwareVer
		// 
		this->label_Glb_HardwareVer->AutoSize = true;
		this->label_Glb_HardwareVer->Location = System::Drawing::Point(5, 73);
		this->label_Glb_HardwareVer->Name = L"label_Glb_HardwareVer";
		this->label_Glb_HardwareVer->Size = System::Drawing::Size(75, 13);
		this->label_Glb_HardwareVer->TabIndex = 3;
		this->label_Glb_HardwareVer->Text = L"Hardware Ver:";
		// 
		// label_Glb_SerialNum
		// 
		this->label_Glb_SerialNum->AutoSize = true;
		this->label_Glb_SerialNum->Location = System::Drawing::Point(34, 19);
		this->label_Glb_SerialNum->Name = L"label_Glb_SerialNum";
		this->label_Glb_SerialNum->Size = System::Drawing::Size(46, 13);
		this->label_Glb_SerialNum->TabIndex = 2;
		this->label_Glb_SerialNum->Text = L"Serial #:";
		// 
		// label_Glb_SoftwareVerDisplay
		// 
		this->label_Glb_SoftwareVerDisplay->AutoSize = true;
		this->label_Glb_SoftwareVerDisplay->Location = System::Drawing::Point(78, 91);
		this->label_Glb_SoftwareVerDisplay->Name = L"label_Glb_SoftwareVerDisplay";
		this->label_Glb_SoftwareVerDisplay->Size = System::Drawing::Size(16, 13);
		this->label_Glb_SoftwareVerDisplay->TabIndex = 16;
		this->label_Glb_SoftwareVerDisplay->Text = L"---";
		// 
		// label_Glb_ManfID
		// 
		this->label_Glb_ManfID->AutoSize = true;
		this->label_Glb_ManfID->Location = System::Drawing::Point(29, 37);
		this->label_Glb_ManfID->Name = L"label_Glb_ManfID";
		this->label_Glb_ManfID->Size = System::Drawing::Size(51, 13);
		this->label_Glb_ManfID->TabIndex = 1;
		this->label_Glb_ManfID->Text = L"Manf. ID:";
		// 
		// groupBox_CalculatedData
		// 
		this->groupBox_CalculatedData->Controls->Add(this->label_Calc_InstPulseDisplay);
		this->groupBox_CalculatedData->Controls->Add(this->label_Calc_InstPulse);
		this->groupBox_CalculatedData->Controls->Add(this->label_Calc_RRInterval);
		this->groupBox_CalculatedData->Controls->Add(this->label_Calc_RRIntervalDisplay);
		this->groupBox_CalculatedData->Controls->Add(this->label_Calc_waitToggle);
		this->groupBox_CalculatedData->Controls->Add(this->label_Calc_Pulse);
		this->groupBox_CalculatedData->Controls->Add(this->label_Calc_PulseDisplay);
		this->groupBox_CalculatedData->Controls->Add(this->label_Calc_TotEventCountDisplay);
		this->groupBox_CalculatedData->Controls->Add(this->label_Calc_TotEventCount);
		this->groupBox_CalculatedData->Controls->Add(this->label_Calc_ElapsedSecsDisplay);
		this->groupBox_CalculatedData->Controls->Add(this->label_Calc_ElpTime);
		this->groupBox_CalculatedData->Location = System::Drawing::Point(3, 3);
		this->groupBox_CalculatedData->Name = L"groupBox_CalculatedData";
		this->groupBox_CalculatedData->Size = System::Drawing::Size(232, 130);
		this->groupBox_CalculatedData->TabIndex = 17;
		this->groupBox_CalculatedData->TabStop = false;
		this->groupBox_CalculatedData->Text = L"Calculated Data";
		// 
		// label_Calc_InstPulseDisplay
		// 
		this->label_Calc_InstPulseDisplay->AutoSize = true;
		this->label_Calc_InstPulseDisplay->Location = System::Drawing::Point(141, 41);
		this->label_Calc_InstPulseDisplay->Name = L"label_Calc_InstPulseDisplay";
		this->label_Calc_InstPulseDisplay->Size = System::Drawing::Size(16, 13);
		this->label_Calc_InstPulseDisplay->TabIndex = 13;
		this->label_Calc_InstPulseDisplay->Text = L"---";
		// 
		// label_Calc_InstPulse
		// 
		this->label_Calc_InstPulse->AutoSize = true;
		this->label_Calc_InstPulse->Location = System::Drawing::Point(62, 41);
		this->label_Calc_InstPulse->Name = L"label_Calc_InstPulse";
		this->label_Calc_InstPulse->Size = System::Drawing::Size(73, 13);
		this->label_Calc_InstPulse->TabIndex = 12;
		this->label_Calc_InstPulse->Text = L"Current Pulse:";
		// 
		// label_Calc_RRInterval
		// 
		this->label_Calc_RRInterval->AutoSize = true;
		this->label_Calc_RRInterval->Location = System::Drawing::Point(47, 57);
		this->label_Calc_RRInterval->Name = L"label_Calc_RRInterval";
		this->label_Calc_RRInterval->Size = System::Drawing::Size(89, 13);
		this->label_Calc_RRInterval->TabIndex = 10;
		this->label_Calc_RRInterval->Text = L"R-R Interval (ms):";
		// 
		// label_Calc_RRIntervalDisplay
		// 
		this->label_Calc_RRIntervalDisplay->AutoSize = true;
		this->label_Calc_RRIntervalDisplay->Location = System::Drawing::Point(141, 57);
		this->label_Calc_RRIntervalDisplay->Name = L"label_Calc_RRIntervalDisplay";
		this->label_Calc_RRIntervalDisplay->Size = System::Drawing::Size(16, 13);
		this->label_Calc_RRIntervalDisplay->TabIndex = 11;
		this->label_Calc_RRIntervalDisplay->Text = L"---";
		// 
		// label_Calc_waitToggle
		// 
		this->label_Calc_waitToggle->AutoSize = true;
		this->label_Calc_waitToggle->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Italic, 
			System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
		this->label_Calc_waitToggle->Location = System::Drawing::Point(37, 16);
		this->label_Calc_waitToggle->Name = L"label_Calc_waitToggle";
		this->label_Calc_waitToggle->Size = System::Drawing::Size(177, 13);
		this->label_Calc_waitToggle->TabIndex = 8;
		this->label_Calc_waitToggle->Text = L"Waiting for page toggle to change...";
		// 
		// label_Calc_Pulse
		// 
		this->label_Calc_Pulse->AutoSize = true;
		this->label_Calc_Pulse->Location = System::Drawing::Point(48, 105);
		this->label_Calc_Pulse->Name = L"label_Calc_Pulse";
		this->label_Calc_Pulse->Size = System::Drawing::Size(87, 13);
		this->label_Calc_Pulse->TabIndex = 1;
		this->label_Calc_Pulse->Text = L"Pulse (from time):";
		// 
		// label_Calc_PulseDisplay
		// 
		this->label_Calc_PulseDisplay->AutoSize = true;
		this->label_Calc_PulseDisplay->Location = System::Drawing::Point(141, 105);
		this->label_Calc_PulseDisplay->Name = L"label_Calc_PulseDisplay";
		this->label_Calc_PulseDisplay->Size = System::Drawing::Size(16, 13);
		this->label_Calc_PulseDisplay->TabIndex = 3;
		this->label_Calc_PulseDisplay->Text = L"---";
		// 
		// label_Calc_TotEventCountDisplay
		// 
		this->label_Calc_TotEventCountDisplay->AutoSize = true;
		this->label_Calc_TotEventCountDisplay->Location = System::Drawing::Point(141, 73);
		this->label_Calc_TotEventCountDisplay->Name = L"label_Calc_TotEventCountDisplay";
		this->label_Calc_TotEventCountDisplay->Size = System::Drawing::Size(16, 13);
		this->label_Calc_TotEventCountDisplay->TabIndex = 2;
		this->label_Calc_TotEventCountDisplay->Text = L"---";
		// 
		// label_Calc_TotEventCount
		// 
		this->label_Calc_TotEventCount->AutoSize = true;
		this->label_Calc_TotEventCount->Location = System::Drawing::Point(39, 73);
		this->label_Calc_TotEventCount->Name = L"label_Calc_TotEventCount";
		this->label_Calc_TotEventCount->Size = System::Drawing::Size(96, 13);
		this->label_Calc_TotEventCount->TabIndex = 0;
		this->label_Calc_TotEventCount->Text = L"Total Event Count:";
		// 
		// label_Calc_ElapsedSecsDisplay
		// 
		this->label_Calc_ElapsedSecsDisplay->AutoSize = true;
		this->label_Calc_ElapsedSecsDisplay->Location = System::Drawing::Point(141, 89);
		this->label_Calc_ElapsedSecsDisplay->Name = L"label_Calc_ElapsedSecsDisplay";
		this->label_Calc_ElapsedSecsDisplay->Size = System::Drawing::Size(16, 13);
		this->label_Calc_ElapsedSecsDisplay->TabIndex = 7;
		this->label_Calc_ElapsedSecsDisplay->Text = L"---";
		// 
		// label_Calc_ElpTime
		// 
		this->label_Calc_ElpTime->AutoSize = true;
		this->label_Calc_ElpTime->Location = System::Drawing::Point(20, 89);
		this->label_Calc_ElpTime->Name = L"label_Calc_ElpTime";
		this->label_Calc_ElpTime->Size = System::Drawing::Size(115, 13);
		this->label_Calc_ElpTime->TabIndex = 0;
		this->label_Calc_ElpTime->Text = L"Total Elapsed Time (s):";
		// 
		// HRMDisplay
		// 
		this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
		this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
		this->ClientSize = System::Drawing::Size(794, 351);
		this->Controls->Add(this->panel_Display);
		this->Controls->Add(this->panel_Settings);
		this->Name = L"HRMDisplay";
		this->Text = L"HRMDisplay";
		this->panel_Display->ResumeLayout(false);
		this->panel_Display->PerformLayout();
		this->panel_Settings->ResumeLayout(false);
		this->groupBox2->ResumeLayout(false);
		this->groupBox2->PerformLayout();
		this->groupBox_CalculatedData->ResumeLayout(false);
		this->groupBox_CalculatedData->PerformLayout();
		this->ResumeLayout(false);

	}
#pragma endregion

};