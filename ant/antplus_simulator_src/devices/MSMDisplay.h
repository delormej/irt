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

public ref class MSMDisplay : public System::Windows::Forms::Form, public ISimBase{
public:
		MSMDisplay(dRequestAckMsg^ channelAckMsg){
			InitializeComponent();
			msmData = gcnew MSM();
         commonData = gcnew CommonData();
			requestAckMsg = channelAckMsg;	
			InitializeSim();
		}

		~MSMDisplay(){
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
	virtual void onTimerTock(USHORT eventTime){} // Do nothing
	virtual void ANT_eventNotification(UCHAR ucEventCode_, UCHAR* pucEventBuffer_);	 // Channel event state machine
	virtual UCHAR getDeviceType(){return msmData->DEVICE_TYPE;}
	virtual UCHAR getTransmissionType(){return 0;}
	virtual USHORT getTransmitPeriod(){return msmData->MSG_PERIOD;}
	virtual DOUBLE getTimerInterval(){return 1000;} // Set to any value, as receiver does not use the timer
	virtual System::Windows::Forms::Panel^ getSimSettingsPanel(){return this->panel_Settings;}
	virtual System::Windows::Forms::Panel^ getSimTranslatedDisplay(){return this->panel_Display;}


private:
	void InitializeSim();   // Initialize simulation
	void UpdateDisplay();   // Updates the display after an ANT message is received
	void UpdateAcumValues();
	void UpdateCalibration();
	void SendCalibrationMsg(MSM::CalibrationStatus pageType_);
	void UpdateDisplayAckStatus(UCHAR ackStatus_);
	System::Void numericUpDownMode_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDownScaleFactor_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void buttonCalibrationRequest_Click(System::Object^  sender, System::EventArgs^  e);
	System::Void buttonSetScaleFactor_Click(System::Object^  sender, System::EventArgs^  e);

private:
	dRequestAckMsg^ requestAckMsg;
	MSM^ msmData;
	CommonData^ commonData;
	MSM::CalibrationStatus eCalStatus;

   // Previous values
	UCHAR ucPreviousMode;   // mode
	USHORT usPreviousScale; // scale
	USHORT usPreviousDist;  // distance
	USHORT usPreviousTime;  // time

   // Calculated values for display
	double dbDispAcumDist;  // Cumulative distance (m)
	double dbDispAcumTime;  // Cumulative time (s)
	double dbDispInstSpeed; // Instantaneous speed (m/s)
	double dbDispLat;       // Latitude (deg)
	double dbDispLong;      // Longitude (deg)
	double dbDispElev;      // Elevation (m)
	double dbDispHeading;   // Heading (deg)

private: System::Windows::Forms::Panel^  panel_Settings;
private: System::Windows::Forms::Panel^  panel_Display;
private: System::Windows::Forms::Label^  label1;
private: System::Windows::Forms::Label^  label4;
private: System::Windows::Forms::Label^  label3;
private: System::Windows::Forms::TabControl^  tabControl1;
private: System::Windows::Forms::TabPage^  tabPage1;
private: System::Windows::Forms::TabPage^  tabPage2;
private: System::Windows::Forms::Label^  labelDisplayAcumTime;
private: System::Windows::Forms::Label^  labelDisplayAcumDistance;
private: System::Windows::Forms::Label^  labelDisplayInstSpeed;
private: System::Windows::Forms::Label^  label9;
private: System::Windows::Forms::Label^  label13;
private: System::Windows::Forms::Label^  labelTxAcumTime;
private: System::Windows::Forms::Label^  label11;
private: System::Windows::Forms::Label^  labelTxAcumDist;
private: System::Windows::Forms::Label^  label10;
private: System::Windows::Forms::Label^  labelTxInstSpeed;
private: System::Windows::Forms::Label^  label14;
private: System::Windows::Forms::Label^  label12;
private: System::Windows::Forms::NumericUpDown^  numericUpDownScaleFactor;
private: System::Windows::Forms::NumericUpDown^  numericUpDownMode;
private: System::Windows::Forms::Button^  buttonSetScaleFactor;
private: System::Windows::Forms::Button^  buttonCalibrationRequest;
private: System::Windows::Forms::Label^  labelCalibrationProgress;
private: System::Windows::Forms::Label^  labelDisplayFixType;
private: System::Windows::Forms::Label^  labelDisplayHeading;
private: System::Windows::Forms::Label^  labelDisplayElevation;
private: System::Windows::Forms::Label^  labelDisplayLat;
private: System::Windows::Forms::Label^  labelDisplayLong;
private: System::Windows::Forms::Label^  labelFix;
private: System::Windows::Forms::Label^  labelHead;
private: System::Windows::Forms::Label^  labelElev;
private: System::Windows::Forms::Label^  labelLon;
private: System::Windows::Forms::Label^  labelLat;
private: System::Windows::Forms::Label^  label2;
private: System::Windows::Forms::Label^  labelRxLon;
private: System::Windows::Forms::Label^  labelRxLat;
private: System::Windows::Forms::Label^  label5;
private: System::Windows::Forms::Label^  labelRxFix;
private: System::Windows::Forms::Label^  labelRxHeading;
private: System::Windows::Forms::Label^  labelRxElev;
private: System::Windows::Forms::Label^  label8;
private: System::Windows::Forms::Label^  label7;
private: System::Windows::Forms::Label^  label6;
private: System::Windows::Forms::TabPage^  tabPage3;
private: System::Windows::Forms::Label^  label_Glb_SoftwareVer;
private: System::Windows::Forms::Label^  label_Glb_HardwareVer;
private: System::Windows::Forms::Label^  label_Glb_SoftwareVerDisplay;
private: System::Windows::Forms::Label^  label_Glb_SerialNumDisplay;
private: System::Windows::Forms::Label^  label_Glb_ModelNum;
private: System::Windows::Forms::Label^  label_Glb_ManfIDDisplay;
private: System::Windows::Forms::Label^  label_Glb_SerialNum;
private: System::Windows::Forms::Label^  label_Glb_ModelNumDisplay;
private: System::Windows::Forms::Label^  label_Glb_HardwareVerDisplay;
private: System::Windows::Forms::Label^  label_Glb_ManfID;
private: System::Windows::Forms::Label^  label16;
private: System::Windows::Forms::Label^  label15;
private: System::Windows::Forms::Label^  label21;
private: System::Windows::Forms::Label^  label20;
private: System::Windows::Forms::Label^  labelBattStatus;
private: System::Windows::Forms::Label^  labelBattVolt;
private: System::Windows::Forms::Label^  labelTimeRes;
private: System::Windows::Forms::Label^  labelOpTime;
private: System::Windows::Forms::Label^  labelDay;
private: System::Windows::Forms::Label^  labelSeconds;
private: System::Windows::Forms::Label^  labelMinutes;
private: System::Windows::Forms::Label^  labelHours;
private: System::Windows::Forms::Label^  label27;
private: System::Windows::Forms::Label^  label30;
private: System::Windows::Forms::Label^  label37;
private: System::Windows::Forms::Label^  labelDayOfWeek;
private: System::Windows::Forms::Label^  labelYear;
private: System::Windows::Forms::Label^  labelMonth;
private: System::Windows::Forms::Label^  label25;
private: System::Windows::Forms::Label^  label24;
private: System::Windows::Forms::Label^  label28;
private: System::Windows::Forms::Label^  label26;
private: System::Windows::Forms::Label^  label29;
private: System::Windows::Forms::Label^  label23;
private: System::Windows::Forms::TabControl^  tabControl2;
private: System::Windows::Forms::TabPage^  tabPage5;
private: System::Windows::Forms::TabPage^  tabPage6;
private: System::Windows::Forms::TabPage^  tabPage7;


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
         this->labelDisplayFixType = (gcnew System::Windows::Forms::Label());
         this->labelDisplayHeading = (gcnew System::Windows::Forms::Label());
         this->labelDisplayElevation = (gcnew System::Windows::Forms::Label());
         this->labelDisplayLat = (gcnew System::Windows::Forms::Label());
         this->labelDisplayLong = (gcnew System::Windows::Forms::Label());
         this->labelFix = (gcnew System::Windows::Forms::Label());
         this->labelHead = (gcnew System::Windows::Forms::Label());
         this->labelElev = (gcnew System::Windows::Forms::Label());
         this->labelLon = (gcnew System::Windows::Forms::Label());
         this->labelLat = (gcnew System::Windows::Forms::Label());
         this->labelDisplayInstSpeed = (gcnew System::Windows::Forms::Label());
         this->labelDisplayAcumTime = (gcnew System::Windows::Forms::Label());
         this->labelDisplayAcumDistance = (gcnew System::Windows::Forms::Label());
         this->label4 = (gcnew System::Windows::Forms::Label());
         this->label1 = (gcnew System::Windows::Forms::Label());
         this->label3 = (gcnew System::Windows::Forms::Label());
         this->tabPage2 = (gcnew System::Windows::Forms::TabPage());
         this->labelCalibrationProgress = (gcnew System::Windows::Forms::Label());
         this->buttonSetScaleFactor = (gcnew System::Windows::Forms::Button());
         this->buttonCalibrationRequest = (gcnew System::Windows::Forms::Button());
         this->label14 = (gcnew System::Windows::Forms::Label());
         this->label12 = (gcnew System::Windows::Forms::Label());
         this->numericUpDownScaleFactor = (gcnew System::Windows::Forms::NumericUpDown());
         this->numericUpDownMode = (gcnew System::Windows::Forms::NumericUpDown());
         this->tabPage3 = (gcnew System::Windows::Forms::TabPage());
         this->tabControl2 = (gcnew System::Windows::Forms::TabControl());
         this->tabPage5 = (gcnew System::Windows::Forms::TabPage());
         this->label_Glb_ManfID = (gcnew System::Windows::Forms::Label());
         this->label_Glb_ManfIDDisplay = (gcnew System::Windows::Forms::Label());
         this->label_Glb_ModelNum = (gcnew System::Windows::Forms::Label());
         this->label_Glb_ModelNumDisplay = (gcnew System::Windows::Forms::Label());
         this->label_Glb_HardwareVer = (gcnew System::Windows::Forms::Label());
         this->label_Glb_HardwareVerDisplay = (gcnew System::Windows::Forms::Label());
         this->label_Glb_SerialNumDisplay = (gcnew System::Windows::Forms::Label());
         this->label_Glb_SerialNum = (gcnew System::Windows::Forms::Label());
         this->label_Glb_SoftwareVerDisplay = (gcnew System::Windows::Forms::Label());
         this->label_Glb_SoftwareVer = (gcnew System::Windows::Forms::Label());
         this->tabPage6 = (gcnew System::Windows::Forms::TabPage());
         this->label21 = (gcnew System::Windows::Forms::Label());
         this->labelTimeRes = (gcnew System::Windows::Forms::Label());
         this->label15 = (gcnew System::Windows::Forms::Label());
         this->labelOpTime = (gcnew System::Windows::Forms::Label());
         this->label16 = (gcnew System::Windows::Forms::Label());
         this->labelBattVolt = (gcnew System::Windows::Forms::Label());
         this->label20 = (gcnew System::Windows::Forms::Label());
         this->labelBattStatus = (gcnew System::Windows::Forms::Label());
         this->tabPage7 = (gcnew System::Windows::Forms::TabPage());
         this->label29 = (gcnew System::Windows::Forms::Label());
         this->label30 = (gcnew System::Windows::Forms::Label());
         this->labelDayOfWeek = (gcnew System::Windows::Forms::Label());
         this->label23 = (gcnew System::Windows::Forms::Label());
         this->label27 = (gcnew System::Windows::Forms::Label());
         this->label28 = (gcnew System::Windows::Forms::Label());
         this->labelHours = (gcnew System::Windows::Forms::Label());
         this->label26 = (gcnew System::Windows::Forms::Label());
         this->labelMinutes = (gcnew System::Windows::Forms::Label());
         this->label25 = (gcnew System::Windows::Forms::Label());
         this->labelSeconds = (gcnew System::Windows::Forms::Label());
         this->labelYear = (gcnew System::Windows::Forms::Label());
         this->label24 = (gcnew System::Windows::Forms::Label());
         this->labelMonth = (gcnew System::Windows::Forms::Label());
         this->label37 = (gcnew System::Windows::Forms::Label());
         this->labelDay = (gcnew System::Windows::Forms::Label());
         this->panel_Display = (gcnew System::Windows::Forms::Panel());
         this->labelRxFix = (gcnew System::Windows::Forms::Label());
         this->labelRxHeading = (gcnew System::Windows::Forms::Label());
         this->labelRxElev = (gcnew System::Windows::Forms::Label());
         this->label8 = (gcnew System::Windows::Forms::Label());
         this->label7 = (gcnew System::Windows::Forms::Label());
         this->label6 = (gcnew System::Windows::Forms::Label());
         this->labelRxLon = (gcnew System::Windows::Forms::Label());
         this->labelRxLat = (gcnew System::Windows::Forms::Label());
         this->label5 = (gcnew System::Windows::Forms::Label());
         this->label2 = (gcnew System::Windows::Forms::Label());
         this->labelTxInstSpeed = (gcnew System::Windows::Forms::Label());
         this->label13 = (gcnew System::Windows::Forms::Label());
         this->labelTxAcumTime = (gcnew System::Windows::Forms::Label());
         this->label11 = (gcnew System::Windows::Forms::Label());
         this->labelTxAcumDist = (gcnew System::Windows::Forms::Label());
         this->label10 = (gcnew System::Windows::Forms::Label());
         this->label9 = (gcnew System::Windows::Forms::Label());
         this->panel_Settings->SuspendLayout();
         this->tabControl1->SuspendLayout();
         this->tabPage1->SuspendLayout();
         this->tabPage2->SuspendLayout();
         (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownScaleFactor))->BeginInit();
         (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownMode))->BeginInit();
         this->tabPage3->SuspendLayout();
         this->tabControl2->SuspendLayout();
         this->tabPage5->SuspendLayout();
         this->tabPage6->SuspendLayout();
         this->tabPage7->SuspendLayout();
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
         this->tabControl1->Location = System::Drawing::Point(3, 0);
         this->tabControl1->Name = L"tabControl1";
         this->tabControl1->SelectedIndex = 0;
         this->tabControl1->Size = System::Drawing::Size(397, 140);
         this->tabControl1->TabIndex = 8;
         // 
         // tabPage1
         // 
         this->tabPage1->Controls->Add(this->labelDisplayFixType);
         this->tabPage1->Controls->Add(this->labelDisplayHeading);
         this->tabPage1->Controls->Add(this->labelDisplayElevation);
         this->tabPage1->Controls->Add(this->labelDisplayLat);
         this->tabPage1->Controls->Add(this->labelDisplayLong);
         this->tabPage1->Controls->Add(this->labelFix);
         this->tabPage1->Controls->Add(this->labelHead);
         this->tabPage1->Controls->Add(this->labelElev);
         this->tabPage1->Controls->Add(this->labelLon);
         this->tabPage1->Controls->Add(this->labelLat);
         this->tabPage1->Controls->Add(this->labelDisplayInstSpeed);
         this->tabPage1->Controls->Add(this->labelDisplayAcumTime);
         this->tabPage1->Controls->Add(this->labelDisplayAcumDistance);
         this->tabPage1->Controls->Add(this->label4);
         this->tabPage1->Controls->Add(this->label1);
         this->tabPage1->Controls->Add(this->label3);
         this->tabPage1->Location = System::Drawing::Point(4, 22);
         this->tabPage1->Name = L"tabPage1";
         this->tabPage1->Padding = System::Windows::Forms::Padding(3);
         this->tabPage1->Size = System::Drawing::Size(389, 114);
         this->tabPage1->TabIndex = 0;
         this->tabPage1->Text = L"MSM Data";
         this->tabPage1->UseVisualStyleBackColor = true;
         // 
         // labelDisplayFixType
         // 
         this->labelDisplayFixType->AutoSize = true;
         this->labelDisplayFixType->Location = System::Drawing::Point(306, 98);
         this->labelDisplayFixType->Name = L"labelDisplayFixType";
         this->labelDisplayFixType->Size = System::Drawing::Size(16, 13);
         this->labelDisplayFixType->TabIndex = 35;
         this->labelDisplayFixType->Text = L"---";
         // 
         // labelDisplayHeading
         // 
         this->labelDisplayHeading->AutoSize = true;
         this->labelDisplayHeading->Location = System::Drawing::Point(306, 73);
         this->labelDisplayHeading->Name = L"labelDisplayHeading";
         this->labelDisplayHeading->Size = System::Drawing::Size(16, 13);
         this->labelDisplayHeading->TabIndex = 34;
         this->labelDisplayHeading->Text = L"---";
         // 
         // labelDisplayElevation
         // 
         this->labelDisplayElevation->AutoSize = true;
         this->labelDisplayElevation->Location = System::Drawing::Point(306, 50);
         this->labelDisplayElevation->Name = L"labelDisplayElevation";
         this->labelDisplayElevation->Size = System::Drawing::Size(16, 13);
         this->labelDisplayElevation->TabIndex = 33;
         this->labelDisplayElevation->Text = L"---";
         // 
         // labelDisplayLat
         // 
         this->labelDisplayLat->AutoSize = true;
         this->labelDisplayLat->Location = System::Drawing::Point(306, 4);
         this->labelDisplayLat->Name = L"labelDisplayLat";
         this->labelDisplayLat->Size = System::Drawing::Size(16, 13);
         this->labelDisplayLat->TabIndex = 32;
         this->labelDisplayLat->Text = L"---";
         // 
         // labelDisplayLong
         // 
         this->labelDisplayLong->AutoSize = true;
         this->labelDisplayLong->Location = System::Drawing::Point(306, 27);
         this->labelDisplayLong->Name = L"labelDisplayLong";
         this->labelDisplayLong->Size = System::Drawing::Size(16, 13);
         this->labelDisplayLong->TabIndex = 31;
         this->labelDisplayLong->Text = L"---";
         // 
         // labelFix
         // 
         this->labelFix->AutoSize = true;
         this->labelFix->Location = System::Drawing::Point(216, 96);
         this->labelFix->Name = L"labelFix";
         this->labelFix->Size = System::Drawing::Size(50, 13);
         this->labelFix->TabIndex = 30;
         this->labelFix->Text = L"Fix Type:";
         // 
         // labelHead
         // 
         this->labelHead->AutoSize = true;
         this->labelHead->Location = System::Drawing::Point(216, 73);
         this->labelHead->Name = L"labelHead";
         this->labelHead->Size = System::Drawing::Size(77, 13);
         this->labelHead->TabIndex = 29;
         this->labelHead->Text = L"Heading (deg):";
         // 
         // labelElev
         // 
         this->labelElev->AutoSize = true;
         this->labelElev->Location = System::Drawing::Point(216, 50);
         this->labelElev->Name = L"labelElev";
         this->labelElev->Size = System::Drawing::Size(71, 13);
         this->labelElev->TabIndex = 28;
         this->labelElev->Text = L"Elevation (m):";
         // 
         // labelLon
         // 
         this->labelLon->AutoSize = true;
         this->labelLon->Location = System::Drawing::Point(216, 27);
         this->labelLon->Name = L"labelLon";
         this->labelLon->Size = System::Drawing::Size(84, 13);
         this->labelLon->TabIndex = 27;
         this->labelLon->Text = L"Longitude (deg):";
         // 
         // labelLat
         // 
         this->labelLat->AutoSize = true;
         this->labelLat->Location = System::Drawing::Point(216, 4);
         this->labelLat->Name = L"labelLat";
         this->labelLat->Size = System::Drawing::Size(75, 13);
         this->labelLat->TabIndex = 26;
         this->labelLat->Text = L"Latitude (deg):";
         // 
         // labelDisplayInstSpeed
         // 
         this->labelDisplayInstSpeed->AutoSize = true;
         this->labelDisplayInstSpeed->Location = System::Drawing::Point(146, 50);
         this->labelDisplayInstSpeed->Name = L"labelDisplayInstSpeed";
         this->labelDisplayInstSpeed->Size = System::Drawing::Size(16, 13);
         this->labelDisplayInstSpeed->TabIndex = 10;
         this->labelDisplayInstSpeed->Text = L"---";
         // 
         // labelDisplayAcumTime
         // 
         this->labelDisplayAcumTime->AutoSize = true;
         this->labelDisplayAcumTime->Location = System::Drawing::Point(146, 4);
         this->labelDisplayAcumTime->Name = L"labelDisplayAcumTime";
         this->labelDisplayAcumTime->Size = System::Drawing::Size(16, 13);
         this->labelDisplayAcumTime->TabIndex = 9;
         this->labelDisplayAcumTime->Text = L"---";
         // 
         // labelDisplayAcumDistance
         // 
         this->labelDisplayAcumDistance->AutoSize = true;
         this->labelDisplayAcumDistance->Location = System::Drawing::Point(146, 27);
         this->labelDisplayAcumDistance->Name = L"labelDisplayAcumDistance";
         this->labelDisplayAcumDistance->Size = System::Drawing::Size(16, 13);
         this->labelDisplayAcumDistance->TabIndex = 8;
         this->labelDisplayAcumDistance->Text = L"---";
         // 
         // label4
         // 
         this->label4->AutoSize = true;
         this->label4->Location = System::Drawing::Point(6, 4);
         this->label4->Name = L"label4";
         this->label4->Size = System::Drawing::Size(112, 13);
         this->label4->TabIndex = 3;
         this->label4->Text = L"Accumulated Time (s):";
         // 
         // label1
         // 
         this->label1->AutoSize = true;
         this->label1->Location = System::Drawing::Point(6, 27);
         this->label1->Name = L"label1";
         this->label1->Size = System::Drawing::Size(134, 13);
         this->label1->TabIndex = 0;
         this->label1->Text = L"Accumulated Distance (m):";
         // 
         // label3
         // 
         this->label3->AutoSize = true;
         this->label3->Location = System::Drawing::Point(6, 50);
         this->label3->Name = L"label3";
         this->label3->Size = System::Drawing::Size(138, 13);
         this->label3->TabIndex = 2;
         this->label3->Text = L"Instantaneous Speed (m/s):";
         // 
         // tabPage2
         // 
         this->tabPage2->Controls->Add(this->labelCalibrationProgress);
         this->tabPage2->Controls->Add(this->buttonSetScaleFactor);
         this->tabPage2->Controls->Add(this->buttonCalibrationRequest);
         this->tabPage2->Controls->Add(this->label14);
         this->tabPage2->Controls->Add(this->label12);
         this->tabPage2->Controls->Add(this->numericUpDownScaleFactor);
         this->tabPage2->Controls->Add(this->numericUpDownMode);
         this->tabPage2->Location = System::Drawing::Point(4, 22);
         this->tabPage2->Name = L"tabPage2";
         this->tabPage2->Padding = System::Windows::Forms::Padding(3);
         this->tabPage2->Size = System::Drawing::Size(389, 114);
         this->tabPage2->TabIndex = 1;
         this->tabPage2->Text = L"Calibration";
         this->tabPage2->UseVisualStyleBackColor = true;
         // 
         // labelCalibrationProgress
         // 
         this->labelCalibrationProgress->AutoSize = true;
         this->labelCalibrationProgress->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, 
            System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
         this->labelCalibrationProgress->Location = System::Drawing::Point(188, 64);
         this->labelCalibrationProgress->Name = L"labelCalibrationProgress";
         this->labelCalibrationProgress->Size = System::Drawing::Size(146, 17);
         this->labelCalibrationProgress->TabIndex = 6;
         this->labelCalibrationProgress->Text = L"Calibration Uninitiated";
         // 
         // buttonSetScaleFactor
         // 
         this->buttonSetScaleFactor->Location = System::Drawing::Point(9, 85);
         this->buttonSetScaleFactor->Name = L"buttonSetScaleFactor";
         this->buttonSetScaleFactor->Size = System::Drawing::Size(153, 20);
         this->buttonSetScaleFactor->TabIndex = 5;
         this->buttonSetScaleFactor->Text = L"Set Mode and Scale Factor";
         this->buttonSetScaleFactor->UseVisualStyleBackColor = true;
         this->buttonSetScaleFactor->Click += gcnew System::EventHandler(this, &MSMDisplay::buttonSetScaleFactor_Click);
         // 
         // buttonCalibrationRequest
         // 
         this->buttonCalibrationRequest->Location = System::Drawing::Point(9, 58);
         this->buttonCalibrationRequest->Name = L"buttonCalibrationRequest";
         this->buttonCalibrationRequest->Size = System::Drawing::Size(153, 20);
         this->buttonCalibrationRequest->TabIndex = 4;
         this->buttonCalibrationRequest->Text = L"Send Calibration Request";
         this->buttonCalibrationRequest->UseVisualStyleBackColor = true;
         this->buttonCalibrationRequest->Click += gcnew System::EventHandler(this, &MSMDisplay::buttonCalibrationRequest_Click);
         // 
         // label14
         // 
         this->label14->AutoSize = true;
         this->label14->Location = System::Drawing::Point(6, 34);
         this->label14->Name = L"label14";
         this->label14->Size = System::Drawing::Size(67, 13);
         this->label14->TabIndex = 3;
         this->label14->Text = L"Scale Factor";
         // 
         // label12
         // 
         this->label12->AutoSize = true;
         this->label12->Location = System::Drawing::Point(6, 5);
         this->label12->Name = L"label12";
         this->label12->Size = System::Drawing::Size(68, 13);
         this->label12->TabIndex = 2;
         this->label12->Text = L"Mode (km/h)";
         // 
         // numericUpDownScaleFactor
         // 
         this->numericUpDownScaleFactor->DecimalPlaces = 4;
         this->numericUpDownScaleFactor->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 262144});
         this->numericUpDownScaleFactor->Location = System::Drawing::Point(79, 32);
         this->numericUpDownScaleFactor->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {65534, 0, 0, 262144});
         this->numericUpDownScaleFactor->Name = L"numericUpDownScaleFactor";
         this->numericUpDownScaleFactor->Size = System::Drawing::Size(60, 20);
         this->numericUpDownScaleFactor->TabIndex = 1;
         this->numericUpDownScaleFactor->ValueChanged += gcnew System::EventHandler(this, &MSMDisplay::numericUpDownScaleFactor_ValueChanged);
         // 
         // numericUpDownMode
         // 
         this->numericUpDownMode->Location = System::Drawing::Point(79, 3);
         this->numericUpDownMode->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {235, 0, 0, 0});
         this->numericUpDownMode->Name = L"numericUpDownMode";
         this->numericUpDownMode->Size = System::Drawing::Size(49, 20);
         this->numericUpDownMode->TabIndex = 0;
         this->numericUpDownMode->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {10, 0, 0, 0});
         this->numericUpDownMode->ValueChanged += gcnew System::EventHandler(this, &MSMDisplay::numericUpDownMode_ValueChanged);
         // 
         // tabPage3
         // 
         this->tabPage3->Controls->Add(this->tabControl2);
         this->tabPage3->Location = System::Drawing::Point(4, 22);
         this->tabPage3->Name = L"tabPage3";
         this->tabPage3->Size = System::Drawing::Size(389, 114);
         this->tabPage3->TabIndex = 2;
         this->tabPage3->Text = L"Common Data";
         this->tabPage3->UseVisualStyleBackColor = true;
         // 
         // tabControl2
         // 
         this->tabControl2->Controls->Add(this->tabPage5);
         this->tabControl2->Controls->Add(this->tabPage6);
         this->tabControl2->Controls->Add(this->tabPage7);
         this->tabControl2->Location = System::Drawing::Point(0, 3);
         this->tabControl2->Name = L"tabControl2";
         this->tabControl2->SelectedIndex = 0;
         this->tabControl2->Size = System::Drawing::Size(389, 111);
         this->tabControl2->TabIndex = 0;
         // 
         // tabPage5
         // 
         this->tabPage5->Controls->Add(this->label_Glb_ManfID);
         this->tabPage5->Controls->Add(this->label_Glb_ManfIDDisplay);
         this->tabPage5->Controls->Add(this->label_Glb_ModelNum);
         this->tabPage5->Controls->Add(this->label_Glb_ModelNumDisplay);
         this->tabPage5->Controls->Add(this->label_Glb_HardwareVer);
         this->tabPage5->Controls->Add(this->label_Glb_HardwareVerDisplay);
         this->tabPage5->Controls->Add(this->label_Glb_SerialNumDisplay);
         this->tabPage5->Controls->Add(this->label_Glb_SerialNum);
         this->tabPage5->Controls->Add(this->label_Glb_SoftwareVerDisplay);
         this->tabPage5->Controls->Add(this->label_Glb_SoftwareVer);
         this->tabPage5->Location = System::Drawing::Point(4, 22);
         this->tabPage5->Name = L"tabPage5";
         this->tabPage5->Padding = System::Windows::Forms::Padding(3);
         this->tabPage5->Size = System::Drawing::Size(381, 85);
         this->tabPage5->TabIndex = 0;
         this->tabPage5->Text = L"Manufacturer\'s Info";
         this->tabPage5->UseVisualStyleBackColor = true;
         // 
         // label_Glb_ManfID
         // 
         this->label_Glb_ManfID->AutoSize = true;
         this->label_Glb_ManfID->Location = System::Drawing::Point(43, 14);
         this->label_Glb_ManfID->Name = L"label_Glb_ManfID";
         this->label_Glb_ManfID->Size = System::Drawing::Size(51, 13);
         this->label_Glb_ManfID->TabIndex = 17;
         this->label_Glb_ManfID->Text = L"Manf. ID:";
         // 
         // label_Glb_ManfIDDisplay
         // 
         this->label_Glb_ManfIDDisplay->Location = System::Drawing::Point(100, 14);
         this->label_Glb_ManfIDDisplay->Name = L"label_Glb_ManfIDDisplay";
         this->label_Glb_ManfIDDisplay->Size = System::Drawing::Size(27, 13);
         this->label_Glb_ManfIDDisplay->TabIndex = 22;
         this->label_Glb_ManfIDDisplay->Text = L"---";
         // 
         // label_Glb_ModelNum
         // 
         this->label_Glb_ModelNum->AutoSize = true;
         this->label_Glb_ModelNum->Location = System::Drawing::Point(45, 37);
         this->label_Glb_ModelNum->Name = L"label_Glb_ModelNum";
         this->label_Glb_ModelNum->Size = System::Drawing::Size(49, 13);
         this->label_Glb_ModelNum->TabIndex = 21;
         this->label_Glb_ModelNum->Text = L"Model #:";
         // 
         // label_Glb_ModelNumDisplay
         // 
         this->label_Glb_ModelNumDisplay->Location = System::Drawing::Point(100, 37);
         this->label_Glb_ModelNumDisplay->Name = L"label_Glb_ModelNumDisplay";
         this->label_Glb_ModelNumDisplay->Size = System::Drawing::Size(42, 13);
         this->label_Glb_ModelNumDisplay->TabIndex = 23;
         this->label_Glb_ModelNumDisplay->Text = L"---";
         // 
         // label_Glb_HardwareVer
         // 
         this->label_Glb_HardwareVer->AutoSize = true;
         this->label_Glb_HardwareVer->Location = System::Drawing::Point(182, 14);
         this->label_Glb_HardwareVer->Name = L"label_Glb_HardwareVer";
         this->label_Glb_HardwareVer->Size = System::Drawing::Size(64, 13);
         this->label_Glb_HardwareVer->TabIndex = 19;
         this->label_Glb_HardwareVer->Text = L"Hw Version:";
         // 
         // label_Glb_HardwareVerDisplay
         // 
         this->label_Glb_HardwareVerDisplay->Location = System::Drawing::Point(247, 14);
         this->label_Glb_HardwareVerDisplay->Name = L"label_Glb_HardwareVerDisplay";
         this->label_Glb_HardwareVerDisplay->Size = System::Drawing::Size(27, 13);
         this->label_Glb_HardwareVerDisplay->TabIndex = 25;
         this->label_Glb_HardwareVerDisplay->Text = L"---";
         // 
         // label_Glb_SerialNumDisplay
         // 
         this->label_Glb_SerialNumDisplay->AutoSize = true;
         this->label_Glb_SerialNumDisplay->Location = System::Drawing::Point(100, 59);
         this->label_Glb_SerialNumDisplay->Name = L"label_Glb_SerialNumDisplay";
         this->label_Glb_SerialNumDisplay->Size = System::Drawing::Size(16, 13);
         this->label_Glb_SerialNumDisplay->TabIndex = 24;
         this->label_Glb_SerialNumDisplay->Text = L"---";
         // 
         // label_Glb_SerialNum
         // 
         this->label_Glb_SerialNum->AutoSize = true;
         this->label_Glb_SerialNum->Location = System::Drawing::Point(48, 59);
         this->label_Glb_SerialNum->Name = L"label_Glb_SerialNum";
         this->label_Glb_SerialNum->Size = System::Drawing::Size(46, 13);
         this->label_Glb_SerialNum->TabIndex = 18;
         this->label_Glb_SerialNum->Text = L"Serial #:";
         // 
         // label_Glb_SoftwareVerDisplay
         // 
         this->label_Glb_SoftwareVerDisplay->AutoSize = true;
         this->label_Glb_SoftwareVerDisplay->Location = System::Drawing::Point(247, 37);
         this->label_Glb_SoftwareVerDisplay->Name = L"label_Glb_SoftwareVerDisplay";
         this->label_Glb_SoftwareVerDisplay->Size = System::Drawing::Size(16, 13);
         this->label_Glb_SoftwareVerDisplay->TabIndex = 26;
         this->label_Glb_SoftwareVerDisplay->Text = L"---";
         // 
         // label_Glb_SoftwareVer
         // 
         this->label_Glb_SoftwareVer->AutoSize = true;
         this->label_Glb_SoftwareVer->Location = System::Drawing::Point(183, 37);
         this->label_Glb_SoftwareVer->Name = L"label_Glb_SoftwareVer";
         this->label_Glb_SoftwareVer->Size = System::Drawing::Size(63, 13);
         this->label_Glb_SoftwareVer->TabIndex = 20;
         this->label_Glb_SoftwareVer->Text = L"Sw Version:";
         // 
         // tabPage6
         // 
         this->tabPage6->Controls->Add(this->label21);
         this->tabPage6->Controls->Add(this->labelTimeRes);
         this->tabPage6->Controls->Add(this->label15);
         this->tabPage6->Controls->Add(this->labelOpTime);
         this->tabPage6->Controls->Add(this->label16);
         this->tabPage6->Controls->Add(this->labelBattVolt);
         this->tabPage6->Controls->Add(this->label20);
         this->tabPage6->Controls->Add(this->labelBattStatus);
         this->tabPage6->Location = System::Drawing::Point(4, 22);
         this->tabPage6->Name = L"tabPage6";
         this->tabPage6->Padding = System::Windows::Forms::Padding(3);
         this->tabPage6->Size = System::Drawing::Size(381, 85);
         this->tabPage6->TabIndex = 1;
         this->tabPage6->Text = L"Battery Status";
         this->tabPage6->UseVisualStyleBackColor = true;
         // 
         // label21
         // 
         this->label21->AutoSize = true;
         this->label21->Location = System::Drawing::Point(49, 12);
         this->label21->Name = L"label21";
         this->label21->Size = System::Drawing::Size(86, 13);
         this->label21->TabIndex = 33;
         this->label21->Text = L"Time Resolution:";
         // 
         // labelTimeRes
         // 
         this->labelTimeRes->AutoSize = true;
         this->labelTimeRes->Location = System::Drawing::Point(141, 12);
         this->labelTimeRes->Name = L"labelTimeRes";
         this->labelTimeRes->Size = System::Drawing::Size(16, 13);
         this->labelTimeRes->TabIndex = 35;
         this->labelTimeRes->Text = L"---";
         // 
         // label15
         // 
         this->label15->AutoSize = true;
         this->label15->Location = System::Drawing::Point(53, 34);
         this->label15->Name = L"label15";
         this->label15->Size = System::Drawing::Size(82, 13);
         this->label15->TabIndex = 27;
         this->label15->Text = L"Operating Time:";
         // 
         // labelOpTime
         // 
         this->labelOpTime->AutoSize = true;
         this->labelOpTime->Location = System::Drawing::Point(141, 34);
         this->labelOpTime->Name = L"labelOpTime";
         this->labelOpTime->Size = System::Drawing::Size(16, 13);
         this->labelOpTime->TabIndex = 34;
         this->labelOpTime->Text = L"---";
         // 
         // label16
         // 
         this->label16->AutoSize = true;
         this->label16->Location = System::Drawing::Point(189, 12);
         this->label16->Name = L"label16";
         this->label16->Size = System::Drawing::Size(82, 13);
         this->label16->TabIndex = 28;
         this->label16->Text = L"Battery Voltage:";
         // 
         // labelBattVolt
         // 
         this->labelBattVolt->AutoSize = true;
         this->labelBattVolt->Location = System::Drawing::Point(277, 12);
         this->labelBattVolt->Name = L"labelBattVolt";
         this->labelBattVolt->Size = System::Drawing::Size(16, 13);
         this->labelBattVolt->TabIndex = 36;
         this->labelBattVolt->Text = L"---";
         // 
         // label20
         // 
         this->label20->AutoSize = true;
         this->label20->Location = System::Drawing::Point(195, 34);
         this->label20->Name = L"label20";
         this->label20->Size = System::Drawing::Size(76, 13);
         this->label20->TabIndex = 32;
         this->label20->Text = L"Battery Status:";
         // 
         // labelBattStatus
         // 
         this->labelBattStatus->AutoSize = true;
         this->labelBattStatus->Location = System::Drawing::Point(277, 34);
         this->labelBattStatus->Name = L"labelBattStatus";
         this->labelBattStatus->Size = System::Drawing::Size(16, 13);
         this->labelBattStatus->TabIndex = 37;
         this->labelBattStatus->Text = L"---";
         // 
         // tabPage7
         // 
         this->tabPage7->Controls->Add(this->label29);
         this->tabPage7->Controls->Add(this->label30);
         this->tabPage7->Controls->Add(this->labelDayOfWeek);
         this->tabPage7->Controls->Add(this->label23);
         this->tabPage7->Controls->Add(this->label27);
         this->tabPage7->Controls->Add(this->label28);
         this->tabPage7->Controls->Add(this->labelHours);
         this->tabPage7->Controls->Add(this->label26);
         this->tabPage7->Controls->Add(this->labelMinutes);
         this->tabPage7->Controls->Add(this->label25);
         this->tabPage7->Controls->Add(this->labelSeconds);
         this->tabPage7->Controls->Add(this->labelYear);
         this->tabPage7->Controls->Add(this->label24);
         this->tabPage7->Controls->Add(this->labelMonth);
         this->tabPage7->Controls->Add(this->label37);
         this->tabPage7->Controls->Add(this->labelDay);
         this->tabPage7->Location = System::Drawing::Point(4, 22);
         this->tabPage7->Name = L"tabPage7";
         this->tabPage7->Size = System::Drawing::Size(381, 85);
         this->tabPage7->TabIndex = 2;
         this->tabPage7->Text = L"Time and Data";
         this->tabPage7->UseVisualStyleBackColor = true;
         // 
         // label29
         // 
         this->label29->AutoSize = true;
         this->label29->Location = System::Drawing::Point(194, 61);
         this->label29->Name = L"label29";
         this->label29->Size = System::Drawing::Size(56, 13);
         this->label29->TabIndex = 59;
         this->label29->Text = L"Weekday:";
         // 
         // label30
         // 
         this->label30->AutoSize = true;
         this->label30->Location = System::Drawing::Point(3, 19);
         this->label30->Name = L"label30";
         this->label30->Size = System::Drawing::Size(70, 13);
         this->label30->TabIndex = 39;
         this->label30->Text = L"Current Time:";
         // 
         // labelDayOfWeek
         // 
         this->labelDayOfWeek->AutoSize = true;
         this->labelDayOfWeek->Location = System::Drawing::Point(256, 61);
         this->labelDayOfWeek->Name = L"labelDayOfWeek";
         this->labelDayOfWeek->Size = System::Drawing::Size(16, 13);
         this->labelDayOfWeek->TabIndex = 52;
         this->labelDayOfWeek->Text = L"---";
         // 
         // label23
         // 
         this->label23->AutoSize = true;
         this->label23->Location = System::Drawing::Point(79, 32);
         this->label23->Name = L"label23";
         this->label23->Size = System::Drawing::Size(63, 13);
         this->label23->TabIndex = 58;
         this->label23->Text = L"hh : mm : ss";
         // 
         // label27
         // 
         this->label27->AutoSize = true;
         this->label27->Location = System::Drawing::Point(94, 19);
         this->label27->Name = L"label27";
         this->label27->Size = System::Drawing::Size(10, 13);
         this->label27->TabIndex = 42;
         this->label27->Text = L":";
         // 
         // label28
         // 
         this->label28->AutoSize = true;
         this->label28->Location = System::Drawing::Point(297, 19);
         this->label28->Name = L"label28";
         this->label28->Size = System::Drawing::Size(12, 13);
         this->label28->TabIndex = 57;
         this->label28->Text = L"/";
         // 
         // labelHours
         // 
         this->labelHours->AutoSize = true;
         this->labelHours->Location = System::Drawing::Point(77, 19);
         this->labelHours->Name = L"labelHours";
         this->labelHours->Size = System::Drawing::Size(19, 13);
         this->labelHours->TabIndex = 43;
         this->labelHours->Text = L"00";
         // 
         // label26
         // 
         this->label26->AutoSize = true;
         this->label26->Location = System::Drawing::Point(258, 32);
         this->label26->Name = L"label26";
         this->label26->Size = System::Drawing::Size(67, 13);
         this->label26->TabIndex = 56;
         this->label26->Text = L"dd / mm / yy";
         // 
         // labelMinutes
         // 
         this->labelMinutes->AutoSize = true;
         this->labelMinutes->Location = System::Drawing::Point(102, 19);
         this->labelMinutes->Name = L"labelMinutes";
         this->labelMinutes->Size = System::Drawing::Size(19, 13);
         this->labelMinutes->TabIndex = 44;
         this->labelMinutes->Text = L"00";
         // 
         // label25
         // 
         this->label25->AutoSize = true;
         this->label25->Location = System::Drawing::Point(271, 19);
         this->label25->Name = L"label25";
         this->label25->Size = System::Drawing::Size(12, 13);
         this->label25->TabIndex = 55;
         this->label25->Text = L"/";
         // 
         // labelSeconds
         // 
         this->labelSeconds->AutoSize = true;
         this->labelSeconds->Location = System::Drawing::Point(126, 19);
         this->labelSeconds->Name = L"labelSeconds";
         this->labelSeconds->Size = System::Drawing::Size(19, 13);
         this->labelSeconds->TabIndex = 45;
         this->labelSeconds->Text = L"00";
         // 
         // labelYear
         // 
         this->labelYear->AutoSize = true;
         this->labelYear->Location = System::Drawing::Point(306, 19);
         this->labelYear->Name = L"labelYear";
         this->labelYear->Size = System::Drawing::Size(19, 13);
         this->labelYear->TabIndex = 51;
         this->labelYear->Text = L"00";
         // 
         // label24
         // 
         this->label24->AutoSize = true;
         this->label24->Location = System::Drawing::Point(180, 19);
         this->label24->Name = L"label24";
         this->label24->Size = System::Drawing::Size(70, 13);
         this->label24->TabIndex = 54;
         this->label24->Text = L"Current Date:";
         // 
         // labelMonth
         // 
         this->labelMonth->AutoSize = true;
         this->labelMonth->Location = System::Drawing::Point(281, 19);
         this->labelMonth->Name = L"labelMonth";
         this->labelMonth->Size = System::Drawing::Size(19, 13);
         this->labelMonth->TabIndex = 50;
         this->labelMonth->Text = L"00";
         // 
         // label37
         // 
         this->label37->AutoSize = true;
         this->label37->Location = System::Drawing::Point(118, 19);
         this->label37->Name = L"label37";
         this->label37->Size = System::Drawing::Size(10, 13);
         this->label37->TabIndex = 53;
         this->label37->Text = L":";
         // 
         // labelDay
         // 
         this->labelDay->AutoSize = true;
         this->labelDay->Location = System::Drawing::Point(256, 18);
         this->labelDay->Name = L"labelDay";
         this->labelDay->Size = System::Drawing::Size(19, 13);
         this->labelDay->TabIndex = 46;
         this->labelDay->Text = L"00";
         // 
         // panel_Display
         // 
         this->panel_Display->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
         this->panel_Display->Controls->Add(this->labelRxFix);
         this->panel_Display->Controls->Add(this->labelRxHeading);
         this->panel_Display->Controls->Add(this->labelRxElev);
         this->panel_Display->Controls->Add(this->label8);
         this->panel_Display->Controls->Add(this->label7);
         this->panel_Display->Controls->Add(this->label6);
         this->panel_Display->Controls->Add(this->labelRxLon);
         this->panel_Display->Controls->Add(this->labelRxLat);
         this->panel_Display->Controls->Add(this->label5);
         this->panel_Display->Controls->Add(this->label2);
         this->panel_Display->Controls->Add(this->labelTxInstSpeed);
         this->panel_Display->Controls->Add(this->label13);
         this->panel_Display->Controls->Add(this->labelTxAcumTime);
         this->panel_Display->Controls->Add(this->label11);
         this->panel_Display->Controls->Add(this->labelTxAcumDist);
         this->panel_Display->Controls->Add(this->label10);
         this->panel_Display->Controls->Add(this->label9);
         this->panel_Display->Location = System::Drawing::Point(58, 188);
         this->panel_Display->Name = L"panel_Display";
         this->panel_Display->Size = System::Drawing::Size(200, 90);
         this->panel_Display->TabIndex = 1;
         // 
         // labelRxFix
         // 
         this->labelRxFix->AutoSize = true;
         this->labelRxFix->Location = System::Drawing::Point(160, 43);
         this->labelRxFix->Name = L"labelRxFix";
         this->labelRxFix->Size = System::Drawing::Size(16, 13);
         this->labelRxFix->TabIndex = 16;
         this->labelRxFix->Text = L"---";
         // 
         // labelRxHeading
         // 
         this->labelRxHeading->AutoSize = true;
         this->labelRxHeading->Location = System::Drawing::Point(160, 28);
         this->labelRxHeading->Name = L"labelRxHeading";
         this->labelRxHeading->Size = System::Drawing::Size(16, 13);
         this->labelRxHeading->TabIndex = 15;
         this->labelRxHeading->Text = L"---";
         // 
         // labelRxElev
         // 
         this->labelRxElev->AutoSize = true;
         this->labelRxElev->Location = System::Drawing::Point(160, 13);
         this->labelRxElev->Name = L"labelRxElev";
         this->labelRxElev->Size = System::Drawing::Size(16, 13);
         this->labelRxElev->TabIndex = 14;
         this->labelRxElev->Text = L"---";
         // 
         // label8
         // 
         this->label8->AutoSize = true;
         this->label8->Location = System::Drawing::Point(100, 43);
         this->label8->Name = L"label8";
         this->label8->Size = System::Drawing::Size(50, 13);
         this->label8->TabIndex = 13;
         this->label8->Text = L"Fix Type:";
         // 
         // label7
         // 
         this->label7->AutoSize = true;
         this->label7->Location = System::Drawing::Point(100, 28);
         this->label7->Name = L"label7";
         this->label7->Size = System::Drawing::Size(50, 13);
         this->label7->TabIndex = 12;
         this->label7->Text = L"Heading:";
         // 
         // label6
         // 
         this->label6->AutoSize = true;
         this->label6->Location = System::Drawing::Point(100, 13);
         this->label6->Name = L"label6";
         this->label6->Size = System::Drawing::Size(54, 13);
         this->label6->TabIndex = 11;
         this->label6->Text = L"Elevation:";
         // 
         // labelRxLon
         // 
         this->labelRxLon->AutoSize = true;
         this->labelRxLon->Location = System::Drawing::Point(61, 73);
         this->labelRxLon->Name = L"labelRxLon";
         this->labelRxLon->Size = System::Drawing::Size(16, 13);
         this->labelRxLon->TabIndex = 10;
         this->labelRxLon->Text = L"---";
         // 
         // labelRxLat
         // 
         this->labelRxLat->AutoSize = true;
         this->labelRxLat->Location = System::Drawing::Point(61, 60);
         this->labelRxLat->Name = L"labelRxLat";
         this->labelRxLat->Size = System::Drawing::Size(16, 13);
         this->labelRxLat->TabIndex = 9;
         this->labelRxLat->Text = L"---";
         // 
         // label5
         // 
         this->label5->AutoSize = true;
         this->label5->Location = System::Drawing::Point(3, 58);
         this->label5->Name = L"label5";
         this->label5->Size = System::Drawing::Size(48, 13);
         this->label5->TabIndex = 8;
         this->label5->Text = L"Latitude:";
         // 
         // label2
         // 
         this->label2->AutoSize = true;
         this->label2->Location = System::Drawing::Point(3, 73);
         this->label2->Name = L"label2";
         this->label2->Size = System::Drawing::Size(57, 13);
         this->label2->TabIndex = 7;
         this->label2->Text = L"Longitude:";
         // 
         // labelTxInstSpeed
         // 
         this->labelTxInstSpeed->AutoSize = true;
         this->labelTxInstSpeed->Location = System::Drawing::Point(61, 43);
         this->labelTxInstSpeed->Name = L"labelTxInstSpeed";
         this->labelTxInstSpeed->Size = System::Drawing::Size(16, 13);
         this->labelTxInstSpeed->TabIndex = 6;
         this->labelTxInstSpeed->Text = L"---";
         // 
         // label13
         // 
         this->label13->AutoSize = true;
         this->label13->Location = System::Drawing::Point(3, 43);
         this->label13->Name = L"label13";
         this->label13->Size = System::Drawing::Size(41, 13);
         this->label13->TabIndex = 5;
         this->label13->Text = L"Speed:";
         // 
         // labelTxAcumTime
         // 
         this->labelTxAcumTime->AutoSize = true;
         this->labelTxAcumTime->Location = System::Drawing::Point(61, 13);
         this->labelTxAcumTime->Name = L"labelTxAcumTime";
         this->labelTxAcumTime->Size = System::Drawing::Size(16, 13);
         this->labelTxAcumTime->TabIndex = 4;
         this->labelTxAcumTime->Text = L"---";
         // 
         // label11
         // 
         this->label11->AutoSize = true;
         this->label11->Location = System::Drawing::Point(3, 13);
         this->label11->Name = L"label11";
         this->label11->Size = System::Drawing::Size(33, 13);
         this->label11->TabIndex = 3;
         this->label11->Text = L"Time:";
         // 
         // labelTxAcumDist
         // 
         this->labelTxAcumDist->AutoSize = true;
         this->labelTxAcumDist->Location = System::Drawing::Point(61, 28);
         this->labelTxAcumDist->Name = L"labelTxAcumDist";
         this->labelTxAcumDist->Size = System::Drawing::Size(16, 13);
         this->labelTxAcumDist->TabIndex = 2;
         this->labelTxAcumDist->Text = L"---";
         // 
         // label10
         // 
         this->label10->AutoSize = true;
         this->label10->Location = System::Drawing::Point(3, 28);
         this->label10->Name = L"label10";
         this->label10->Size = System::Drawing::Size(52, 13);
         this->label10->TabIndex = 1;
         this->label10->Text = L"Distance:";
         // 
         // label9
         // 
         this->label9->AutoSize = true;
         this->label9->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Underline, System::Drawing::GraphicsUnit::Point, 
            static_cast<System::Byte>(0)));
         this->label9->Location = System::Drawing::Point(3, 0);
         this->label9->Name = L"label9";
         this->label9->Size = System::Drawing::Size(114, 13);
         this->label9->TabIndex = 0;
         this->label9->Text = L"Current MSM Rx Data:";
         // 
         // MSMDisplay
         // 
         this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
         this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
         this->ClientSize = System::Drawing::Size(794, 351);
         this->Controls->Add(this->panel_Display);
         this->Controls->Add(this->panel_Settings);
         this->Name = L"MSMDisplay";
         this->Text = L"Multi-Sport Speed and Distance Display";
         this->panel_Settings->ResumeLayout(false);
         this->tabControl1->ResumeLayout(false);
         this->tabPage1->ResumeLayout(false);
         this->tabPage1->PerformLayout();
         this->tabPage2->ResumeLayout(false);
         this->tabPage2->PerformLayout();
         (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownScaleFactor))->EndInit();
         (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownMode))->EndInit();
         this->tabPage3->ResumeLayout(false);
         this->tabControl2->ResumeLayout(false);
         this->tabPage5->ResumeLayout(false);
         this->tabPage5->PerformLayout();
         this->tabPage6->ResumeLayout(false);
         this->tabPage6->PerformLayout();
         this->tabPage7->ResumeLayout(false);
         this->tabPage7->PerformLayout();
         this->panel_Display->ResumeLayout(false);
         this->panel_Display->PerformLayout();
         this->ResumeLayout(false);

      }
#pragma endregion
};