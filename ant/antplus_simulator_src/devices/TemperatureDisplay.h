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


public ref class TemperatureDisplay : public System::Windows::Forms::Form, public ISimBase{
public:
   TemperatureDisplay(System::Timers::Timer^ channelTimer, dRequestAckMsg^ channelAckMsg,dRequestUpdateMesgPeriod^ channelUpdateMesgPeriod){
		InitializeComponent();
      TemperatureData = gcnew Temperature();
      commonData = gcnew CommonData();
      requestAckMsg = channelAckMsg;
      requestUpdateMesgPeriod = channelUpdateMesgPeriod;
		InitializeSim();
	}

	~TemperatureDisplay(){
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
   virtual UCHAR getDeviceType(){return TemperatureData->DEVICE_TYPE;}
	virtual UCHAR getTransmissionType(){return 0;}	// Transmission type on receiver must be set to 0 for pairing search and future compatibility
   virtual USHORT getTransmitPeriod(){return TemperatureData->MESG_P5HZ_PERIOD;}
	virtual DOUBLE getTimerInterval(){return 3600000;} // Set interval to one hour, so timer events are not frequent (timer should be disabled tho)
	virtual void onTimerTock(USHORT eventTime){} // Do nothing
	virtual System::Windows::Forms::Panel^ getSimSettingsPanel(){return this->panel_Settings;}
	virtual System::Windows::Forms::Panel^ getSimTranslatedDisplay(){return this->panel_Display;}

private:
	void InitializeSim();
	void HandleReceive(UCHAR* pucRxBuffer_);
	void UpdateDisplay(UCHAR ucPageNum_);
   void SendRequestMsg(UCHAR ucMsgCode_);
	BOOL HandleRetransmit();
	void UpdateDisplayAckStatus(UCHAR ucStatus_);
	void EncodeRequestMsg(UCHAR ucPageID_, UCHAR* pucTxBuffer_);   

   System::Void button_Req_Page80_Click(System::Object^ sender, System::EventArgs^ e);
   System::Void button_Req_Page81_Click(System::Object^ sender, System::EventArgs^ e);
   System::Void button_Req_Page82_Click(System::Object^  sender, System::EventArgs^  e);
   System::Void numericUpDown_Req_Copies_ValueChaned(System::Object^ sender, System::EventArgs^ e);
   System::Void radioButton_RxPeriod_Changed(System::Object^  sender, System::EventArgs^  e);

private:	// Implementation specific constants
	static const UCHAR ACK_SUCCESS = 0;
	static const UCHAR ACK_RETRY = 1;
	static const UCHAR ACK_FAIL = 2;
	static const UCHAR MAX_RETRIES = 40;	// Maximum number of retransmissions for each message

private:
   // Handles
   dRequestAckMsg^ requestAckMsg;   
	dRequestUpdateMesgPeriod^ requestUpdateMesgPeriod;
   CommonData^ commonData;              // CommonData handle
   Temperature^ TemperatureData;        // Temperature class handle

	// Simulator Variables
	short sTemp;					   // Transmitted temperature (hundredths of deg C)
   short s24HrHigh;
   short s24HrLow;               
	UCHAR ucEventCount;				// Temperature event count
	ULONG ulElapsedTime2;			// Cumulative operating time in 2 second resolution	

	ULONG ulAcumEventCount;			// Cumulative temperature event count

	// Background Data
	USHORT usMfgID;					// Manufacturing ID
	UCHAR ucHwVersion;				// Hardware version
	UCHAR ucSwVersion;				// Software version
	USHORT usModelNum;				// Model number
	ULONG ulSerialNum;				// Serial number
   UCHAR ucSupportedPages;       // Supported Pages Data
   UCHAR ucTxInfo;               // Message period

   // Tx Request Messages
	UCHAR ucMsgExpectingAck;	   // Message pending to be acknowledged
	UCHAR ucAckRetryCount;		   // Number of retries for an acknowledged message
	UCHAR ucRqTxTimes;			   // Number of times for the sensor to send response
	
private: System::Windows::Forms::Panel^  panel_Display;
private: System::Windows::Forms::Label^  label_Trn_EventCountDisplay;
private: System::Windows::Forms::Label^  label_Trn_EventCount;


private: System::Windows::Forms::Label^  label_Trn_TranslatedDisplayLabel;
private: System::Windows::Forms::Panel^  panel_Settings;
private: System::Windows::Forms::GroupBox^  groupBox_GlobalData;



private: System::Windows::Forms::Label^  label_Glb_ModelNum;
private: System::Windows::Forms::Label^  label_Glb_SoftwareVer;
private: System::Windows::Forms::Label^  label_Glb_ModelNumDisplay;
private: System::Windows::Forms::Label^  label_Glb_MfgIDDisplay;

private: System::Windows::Forms::Label^  label_Glb_SerialNumDisplay;
private: System::Windows::Forms::Label^  label_Glb_HardwareVerDisplay;
private: System::Windows::Forms::Label^  label_Glb_HardwareVer;
private: System::Windows::Forms::Label^  label_Glb_SerialNum;
private: System::Windows::Forms::Label^  label_Glb_SoftwareVerDisplay;
private: System::Windows::Forms::Label^  label_Glb_MfgID;







private: System::Windows::Forms::Label^  label_Calc_TotEventCountDisplay;
private: System::Windows::Forms::Label^  label_Calc_TotEventCount;


private: System::Windows::Forms::Label^  label_Trn_RawTemperature;
private: System::Windows::Forms::Label^  label_Trn_RawTempDisplay;









private: System::Windows::Forms::Label^  label_Trn_24HrLowDisplay;

private: System::Windows::Forms::Label^  label_Trn_24HrHighDisplay;
private: System::Windows::Forms::Label^  label_Trn_24HrLow;
private: System::Windows::Forms::Label^  label_Trn_24hrHigh;

private: System::Windows::Forms::TabPage^  tabPage1;
private: System::Windows::Forms::TabPage^  tabPage2;



private: System::Windows::Forms::Label^  label_Req_Copies;
private: System::Windows::Forms::Button^  button_Req_Page81;
private: System::Windows::Forms::Button^  button_Req_Page80;

private: System::Windows::Forms::NumericUpDown^  numericUpDown_Req_Copies;
private: System::Windows::Forms::Label^  label_AckMsgStatus;
private: System::Windows::Forms::Label^  label_Glb_SpprtdPgs;
private: System::Windows::Forms::Label^  label_Glb_MsgPeriod_Display;

private: System::Windows::Forms::Label^  label_Glb_MsgPeriod;


private: System::Windows::Forms::Label^  label_Glb_SpprtdPgs_Display;
private: System::Windows::Forms::Label^  label_UTCTimeDisplay;

private: System::Windows::Forms::Label^  label_UTCTime;

private: System::Windows::Forms::TabPage^  tabPage3;
private: System::Windows::Forms::Label^  labelBattStatus;
private: System::Windows::Forms::Label^  label68;
private: System::Windows::Forms::Label^  label67;
private: System::Windows::Forms::Label^  labelBattVolt;
private: System::Windows::Forms::Label^  label63;
private: System::Windows::Forms::Label^  label62;
private: System::Windows::Forms::Label^  labelTimeRes;
private: System::Windows::Forms::Label^  labelOpTime;
private: System::Windows::Forms::Button^  button_Req_Page82;
private: System::Windows::Forms::TabControl^  tabControl1;

public: System::Windows::Forms::RadioButton^  radioButton_RxP5Hz;
private: 
public: System::Windows::Forms::RadioButton^  radioButton_Rx4Hz;
private: System::Windows::Forms::GroupBox^  groupBox_RxPeriod;
private: System::Windows::Forms::Label^  label_LocalTime_Display;
private: System::Windows::Forms::Label^  label_LocalTime;













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
      this->label_Trn_24HrLowDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Trn_24HrHighDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Trn_24HrLow = (gcnew System::Windows::Forms::Label());
      this->label_Trn_24hrHigh = (gcnew System::Windows::Forms::Label());
      this->label_Trn_RawTempDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Trn_RawTemperature = (gcnew System::Windows::Forms::Label());
      this->label_Trn_EventCountDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Trn_EventCount = (gcnew System::Windows::Forms::Label());
      this->label_Trn_TranslatedDisplayLabel = (gcnew System::Windows::Forms::Label());
      this->panel_Settings = (gcnew System::Windows::Forms::Panel());
      this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
      this->tabPage1 = (gcnew System::Windows::Forms::TabPage());
      this->groupBox_RxPeriod = (gcnew System::Windows::Forms::GroupBox());
      this->radioButton_Rx4Hz = (gcnew System::Windows::Forms::RadioButton());
      this->radioButton_RxP5Hz = (gcnew System::Windows::Forms::RadioButton());
      this->label_UTCTimeDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Calc_TotEventCountDisplay = (gcnew System::Windows::Forms::Label());
      this->label_UTCTime = (gcnew System::Windows::Forms::Label());
      this->groupBox_GlobalData = (gcnew System::Windows::Forms::GroupBox());
      this->label_Glb_MsgPeriod_Display = (gcnew System::Windows::Forms::Label());
      this->label_Glb_MsgPeriod = (gcnew System::Windows::Forms::Label());
      this->label_Glb_SpprtdPgs_Display = (gcnew System::Windows::Forms::Label());
      this->label_Glb_SpprtdPgs = (gcnew System::Windows::Forms::Label());
      this->label_Glb_ModelNum = (gcnew System::Windows::Forms::Label());
      this->label_Glb_SerialNum = (gcnew System::Windows::Forms::Label());
      this->label_Glb_SoftwareVerDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Glb_MfgID = (gcnew System::Windows::Forms::Label());
      this->label_Glb_SoftwareVer = (gcnew System::Windows::Forms::Label());
      this->label_Glb_HardwareVer = (gcnew System::Windows::Forms::Label());
      this->label_Glb_HardwareVerDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Glb_ModelNumDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Glb_SerialNumDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Glb_MfgIDDisplay = (gcnew System::Windows::Forms::Label());
      this->label_Calc_TotEventCount = (gcnew System::Windows::Forms::Label());
      this->tabPage2 = (gcnew System::Windows::Forms::TabPage());
      this->button_Req_Page82 = (gcnew System::Windows::Forms::Button());
      this->label_AckMsgStatus = (gcnew System::Windows::Forms::Label());
      this->button_Req_Page81 = (gcnew System::Windows::Forms::Button());
      this->button_Req_Page80 = (gcnew System::Windows::Forms::Button());
      this->numericUpDown_Req_Copies = (gcnew System::Windows::Forms::NumericUpDown());
      this->label_Req_Copies = (gcnew System::Windows::Forms::Label());
      this->tabPage3 = (gcnew System::Windows::Forms::TabPage());
      this->labelBattStatus = (gcnew System::Windows::Forms::Label());
      this->label68 = (gcnew System::Windows::Forms::Label());
      this->label67 = (gcnew System::Windows::Forms::Label());
      this->labelBattVolt = (gcnew System::Windows::Forms::Label());
      this->label63 = (gcnew System::Windows::Forms::Label());
      this->label62 = (gcnew System::Windows::Forms::Label());
      this->labelTimeRes = (gcnew System::Windows::Forms::Label());
      this->labelOpTime = (gcnew System::Windows::Forms::Label());
      this->label_LocalTime_Display = (gcnew System::Windows::Forms::Label());
      this->label_LocalTime = (gcnew System::Windows::Forms::Label());
      this->panel_Display->SuspendLayout();
      this->panel_Settings->SuspendLayout();
      this->tabControl1->SuspendLayout();
      this->tabPage1->SuspendLayout();
      this->groupBox_RxPeriod->SuspendLayout();
      this->groupBox_GlobalData->SuspendLayout();
      this->tabPage2->SuspendLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Req_Copies))->BeginInit();
      this->tabPage3->SuspendLayout();
      this->SuspendLayout();
      // 
      // panel_Display
      // 
      this->panel_Display->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
      this->panel_Display->Controls->Add(this->label_Trn_24HrLowDisplay);
      this->panel_Display->Controls->Add(this->label_Trn_24HrHighDisplay);
      this->panel_Display->Controls->Add(this->label_Trn_24HrLow);
      this->panel_Display->Controls->Add(this->label_Trn_24hrHigh);
      this->panel_Display->Controls->Add(this->label_Trn_RawTempDisplay);
      this->panel_Display->Controls->Add(this->label_Trn_RawTemperature);
      this->panel_Display->Controls->Add(this->label_Trn_EventCountDisplay);
      this->panel_Display->Controls->Add(this->label_Trn_EventCount);
      this->panel_Display->Controls->Add(this->label_Trn_TranslatedDisplayLabel);
      this->panel_Display->Location = System::Drawing::Point(65, 199);
      this->panel_Display->Name = L"panel_Display";
      this->panel_Display->Size = System::Drawing::Size(200, 90);
      this->panel_Display->TabIndex = 3;
      // 
      // label_Trn_24HrLowDisplay
      // 
      this->label_Trn_24HrLowDisplay->AutoSize = true;
      this->label_Trn_24HrLowDisplay->Location = System::Drawing::Point(146, 63);
      this->label_Trn_24HrLowDisplay->Name = L"label_Trn_24HrLowDisplay";
      this->label_Trn_24HrLowDisplay->Size = System::Drawing::Size(16, 13);
      this->label_Trn_24HrLowDisplay->TabIndex = 14;
      this->label_Trn_24HrLowDisplay->Text = L"---";
      // 
      // label_Trn_24HrHighDisplay
      // 
      this->label_Trn_24HrHighDisplay->AutoSize = true;
      this->label_Trn_24HrHighDisplay->Location = System::Drawing::Point(146, 50);
      this->label_Trn_24HrHighDisplay->Name = L"label_Trn_24HrHighDisplay";
      this->label_Trn_24HrHighDisplay->Size = System::Drawing::Size(16, 13);
      this->label_Trn_24HrHighDisplay->TabIndex = 13;
      this->label_Trn_24HrHighDisplay->Text = L"---";
      // 
      // label_Trn_24HrLow
      // 
      this->label_Trn_24HrLow->AutoSize = true;
      this->label_Trn_24HrLow->Location = System::Drawing::Point(61, 63);
      this->label_Trn_24HrLow->Name = L"label_Trn_24HrLow";
      this->label_Trn_24HrLow->Size = System::Drawing::Size(79, 13);
      this->label_Trn_24HrLow->TabIndex = 12;
      this->label_Trn_24HrLow->Text = L"24 Hr Low (°C):";
      // 
      // label_Trn_24hrHigh
      // 
      this->label_Trn_24hrHigh->AutoSize = true;
      this->label_Trn_24hrHigh->Location = System::Drawing::Point(59, 50);
      this->label_Trn_24hrHigh->Name = L"label_Trn_24hrHigh";
      this->label_Trn_24hrHigh->Size = System::Drawing::Size(81, 13);
      this->label_Trn_24hrHigh->TabIndex = 11;
      this->label_Trn_24hrHigh->Text = L"24 Hr High (°C):";
      // 
      // label_Trn_RawTempDisplay
      // 
      this->label_Trn_RawTempDisplay->AutoSize = true;
      this->label_Trn_RawTempDisplay->Location = System::Drawing::Point(146, 37);
      this->label_Trn_RawTempDisplay->Name = L"label_Trn_RawTempDisplay";
      this->label_Trn_RawTempDisplay->Size = System::Drawing::Size(16, 13);
      this->label_Trn_RawTempDisplay->TabIndex = 10;
      this->label_Trn_RawTempDisplay->Text = L"---";
      // 
      // label_Trn_RawTemperature
      // 
      this->label_Trn_RawTemperature->AutoSize = true;
      this->label_Trn_RawTemperature->Location = System::Drawing::Point(13, 37);
      this->label_Trn_RawTemperature->Name = L"label_Trn_RawTemperature";
      this->label_Trn_RawTemperature->Size = System::Drawing::Size(127, 13);
      this->label_Trn_RawTemperature->TabIndex = 9;
      this->label_Trn_RawTemperature->Text = L"Current Temperature (°C):";
      // 
      // label_Trn_EventCountDisplay
      // 
      this->label_Trn_EventCountDisplay->AutoSize = true;
      this->label_Trn_EventCountDisplay->Location = System::Drawing::Point(146, 24);
      this->label_Trn_EventCountDisplay->Name = L"label_Trn_EventCountDisplay";
      this->label_Trn_EventCountDisplay->Size = System::Drawing::Size(16, 13);
      this->label_Trn_EventCountDisplay->TabIndex = 8;
      this->label_Trn_EventCountDisplay->Text = L"---";
      // 
      // label_Trn_EventCount
      // 
      this->label_Trn_EventCount->AutoSize = true;
      this->label_Trn_EventCount->Location = System::Drawing::Point(71, 24);
      this->label_Trn_EventCount->Name = L"label_Trn_EventCount";
      this->label_Trn_EventCount->Size = System::Drawing::Size(69, 13);
      this->label_Trn_EventCount->TabIndex = 7;
      this->label_Trn_EventCount->Text = L"Event Count:";
      // 
      // label_Trn_TranslatedDisplayLabel
      // 
      this->label_Trn_TranslatedDisplayLabel->AutoSize = true;
      this->label_Trn_TranslatedDisplayLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Underline, 
         System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
      this->label_Trn_TranslatedDisplayLabel->Location = System::Drawing::Point(3, 0);
      this->label_Trn_TranslatedDisplayLabel->Name = L"label_Trn_TranslatedDisplayLabel";
      this->label_Trn_TranslatedDisplayLabel->Size = System::Drawing::Size(83, 13);
      this->label_Trn_TranslatedDisplayLabel->TabIndex = 4;
      this->label_Trn_TranslatedDisplayLabel->Text = L"Current Rx Data";
      // 
      // panel_Settings
      // 
      this->panel_Settings->Controls->Add(this->tabControl1);
      this->panel_Settings->Location = System::Drawing::Point(322, 40);
      this->panel_Settings->Name = L"panel_Settings";
      this->panel_Settings->Size = System::Drawing::Size(400, 166);
      this->panel_Settings->TabIndex = 2;
      // 
      // tabControl1
      // 
      this->tabControl1->Controls->Add(this->tabPage1);
      this->tabControl1->Controls->Add(this->tabPage2);
      this->tabControl1->Controls->Add(this->tabPage3);
      this->tabControl1->Dock = System::Windows::Forms::DockStyle::Fill;
      this->tabControl1->Location = System::Drawing::Point(0, 0);
      this->tabControl1->Name = L"tabControl1";
      this->tabControl1->SelectedIndex = 0;
      this->tabControl1->Size = System::Drawing::Size(400, 166);
      this->tabControl1->TabIndex = 19;
      // 
      // tabPage1
      // 
      this->tabPage1->Controls->Add(this->label_LocalTime_Display);
      this->tabPage1->Controls->Add(this->label_LocalTime);
      this->tabPage1->Controls->Add(this->groupBox_RxPeriod);
      this->tabPage1->Controls->Add(this->label_UTCTimeDisplay);
      this->tabPage1->Controls->Add(this->label_Calc_TotEventCountDisplay);
      this->tabPage1->Controls->Add(this->label_UTCTime);
      this->tabPage1->Controls->Add(this->groupBox_GlobalData);
      this->tabPage1->Controls->Add(this->label_Calc_TotEventCount);
      this->tabPage1->Location = System::Drawing::Point(4, 22);
      this->tabPage1->Name = L"tabPage1";
      this->tabPage1->Padding = System::Windows::Forms::Padding(3);
      this->tabPage1->Size = System::Drawing::Size(392, 140);
      this->tabPage1->TabIndex = 0;
      this->tabPage1->Text = L"Receive";
      this->tabPage1->UseVisualStyleBackColor = true;
      // 
      // groupBox_RxPeriod
      // 
      this->groupBox_RxPeriod->Controls->Add(this->radioButton_Rx4Hz);
      this->groupBox_RxPeriod->Controls->Add(this->radioButton_RxP5Hz);
      this->groupBox_RxPeriod->Enabled = false;
      this->groupBox_RxPeriod->Location = System::Drawing::Point(258, 89);
      this->groupBox_RxPeriod->Name = L"groupBox_RxPeriod";
      this->groupBox_RxPeriod->Size = System::Drawing::Size(111, 41);
      this->groupBox_RxPeriod->TabIndex = 26;
      this->groupBox_RxPeriod->TabStop = false;
      this->groupBox_RxPeriod->Text = L"Rx Period";
      // 
      // radioButton_Rx4Hz
      // 
      this->radioButton_Rx4Hz->AutoSize = true;
      this->radioButton_Rx4Hz->Location = System::Drawing::Point(65, 19);
      this->radioButton_Rx4Hz->Name = L"radioButton_Rx4Hz";
      this->radioButton_Rx4Hz->Size = System::Drawing::Size(44, 17);
      this->radioButton_Rx4Hz->TabIndex = 23;
      this->radioButton_Rx4Hz->Text = L"4Hz";
      this->radioButton_Rx4Hz->UseVisualStyleBackColor = true;
      this->radioButton_Rx4Hz->CheckedChanged += gcnew System::EventHandler(this, &TemperatureDisplay::radioButton_RxPeriod_Changed);
      // 
      // radioButton_RxP5Hz
      // 
      this->radioButton_RxP5Hz->AutoSize = true;
      this->radioButton_RxP5Hz->Checked = true;
      this->radioButton_RxP5Hz->Location = System::Drawing::Point(6, 19);
      this->radioButton_RxP5Hz->Name = L"radioButton_RxP5Hz";
      this->radioButton_RxP5Hz->Size = System::Drawing::Size(53, 17);
      this->radioButton_RxP5Hz->TabIndex = 24;
      this->radioButton_RxP5Hz->TabStop = true;
      this->radioButton_RxP5Hz->Text = L"0.5Hz";
      this->radioButton_RxP5Hz->UseVisualStyleBackColor = true;
      // 
      // label_UTCTimeDisplay
      // 
      this->label_UTCTimeDisplay->AutoSize = true;
      this->label_UTCTimeDisplay->Location = System::Drawing::Point(163, 106);
      this->label_UTCTimeDisplay->Name = L"label_UTCTimeDisplay";
      this->label_UTCTimeDisplay->Size = System::Drawing::Size(16, 13);
      this->label_UTCTimeDisplay->TabIndex = 22;
      this->label_UTCTimeDisplay->Text = L"---";
      // 
      // label_Calc_TotEventCountDisplay
      // 
      this->label_Calc_TotEventCountDisplay->AutoSize = true;
      this->label_Calc_TotEventCountDisplay->Location = System::Drawing::Point(163, 86);
      this->label_Calc_TotEventCountDisplay->Name = L"label_Calc_TotEventCountDisplay";
      this->label_Calc_TotEventCountDisplay->Size = System::Drawing::Size(16, 13);
      this->label_Calc_TotEventCountDisplay->TabIndex = 2;
      this->label_Calc_TotEventCountDisplay->Text = L"---";
      // 
      // label_UTCTime
      // 
      this->label_UTCTime->AutoSize = true;
      this->label_UTCTime->Location = System::Drawing::Point(99, 106);
      this->label_UTCTime->Name = L"label_UTCTime";
      this->label_UTCTime->Size = System::Drawing::Size(58, 13);
      this->label_UTCTime->TabIndex = 21;
      this->label_UTCTime->Text = L"UTC Time:";
      // 
      // groupBox_GlobalData
      // 
      this->groupBox_GlobalData->Controls->Add(this->label_Glb_MsgPeriod_Display);
      this->groupBox_GlobalData->Controls->Add(this->label_Glb_MsgPeriod);
      this->groupBox_GlobalData->Controls->Add(this->label_Glb_SpprtdPgs_Display);
      this->groupBox_GlobalData->Controls->Add(this->label_Glb_SpprtdPgs);
      this->groupBox_GlobalData->Controls->Add(this->label_Glb_ModelNum);
      this->groupBox_GlobalData->Controls->Add(this->label_Glb_SerialNum);
      this->groupBox_GlobalData->Controls->Add(this->label_Glb_SoftwareVerDisplay);
      this->groupBox_GlobalData->Controls->Add(this->label_Glb_MfgID);
      this->groupBox_GlobalData->Controls->Add(this->label_Glb_SoftwareVer);
      this->groupBox_GlobalData->Controls->Add(this->label_Glb_HardwareVer);
      this->groupBox_GlobalData->Controls->Add(this->label_Glb_HardwareVerDisplay);
      this->groupBox_GlobalData->Controls->Add(this->label_Glb_ModelNumDisplay);
      this->groupBox_GlobalData->Controls->Add(this->label_Glb_SerialNumDisplay);
      this->groupBox_GlobalData->Controls->Add(this->label_Glb_MfgIDDisplay);
      this->groupBox_GlobalData->Location = System::Drawing::Point(3, 0);
      this->groupBox_GlobalData->Name = L"groupBox_GlobalData";
      this->groupBox_GlobalData->Size = System::Drawing::Size(386, 83);
      this->groupBox_GlobalData->TabIndex = 18;
      this->groupBox_GlobalData->TabStop = false;
      this->groupBox_GlobalData->Text = L"Global Data";
      // 
      // label_Glb_MsgPeriod_Display
      // 
      this->label_Glb_MsgPeriod_Display->AutoSize = true;
      this->label_Glb_MsgPeriod_Display->Location = System::Drawing::Point(309, 43);
      this->label_Glb_MsgPeriod_Display->Name = L"label_Glb_MsgPeriod_Display";
      this->label_Glb_MsgPeriod_Display->Size = System::Drawing::Size(16, 13);
      this->label_Glb_MsgPeriod_Display->TabIndex = 20;
      this->label_Glb_MsgPeriod_Display->Text = L"---";
      // 
      // label_Glb_MsgPeriod
      // 
      this->label_Glb_MsgPeriod->AutoSize = true;
      this->label_Glb_MsgPeriod->Location = System::Drawing::Point(217, 43);
      this->label_Glb_MsgPeriod->Name = L"label_Glb_MsgPeriod";
      this->label_Glb_MsgPeriod->Size = System::Drawing::Size(86, 13);
      this->label_Glb_MsgPeriod->TabIndex = 19;
      this->label_Glb_MsgPeriod->Text = L"Message Period:";
      // 
      // label_Glb_SpprtdPgs_Display
      // 
      this->label_Glb_SpprtdPgs_Display->AutoSize = true;
      this->label_Glb_SpprtdPgs_Display->Location = System::Drawing::Point(160, 60);
      this->label_Glb_SpprtdPgs_Display->Name = L"label_Glb_SpprtdPgs_Display";
      this->label_Glb_SpprtdPgs_Display->Size = System::Drawing::Size(16, 13);
      this->label_Glb_SpprtdPgs_Display->TabIndex = 18;
      this->label_Glb_SpprtdPgs_Display->Text = L"---";
      // 
      // label_Glb_SpprtdPgs
      // 
      this->label_Glb_SpprtdPgs->AutoSize = true;
      this->label_Glb_SpprtdPgs->Location = System::Drawing::Point(62, 60);
      this->label_Glb_SpprtdPgs->Name = L"label_Glb_SpprtdPgs";
      this->label_Glb_SpprtdPgs->Size = System::Drawing::Size(92, 13);
      this->label_Glb_SpprtdPgs->TabIndex = 17;
      this->label_Glb_SpprtdPgs->Text = L"Supported Pages:";
      // 
      // label_Glb_ModelNum
      // 
      this->label_Glb_ModelNum->AutoSize = true;
      this->label_Glb_ModelNum->Location = System::Drawing::Point(254, 26);
      this->label_Glb_ModelNum->Name = L"label_Glb_ModelNum";
      this->label_Glb_ModelNum->Size = System::Drawing::Size(49, 13);
      this->label_Glb_ModelNum->TabIndex = 5;
      this->label_Glb_ModelNum->Text = L"Model #:";
      // 
      // label_Glb_SerialNum
      // 
      this->label_Glb_SerialNum->AutoSize = true;
      this->label_Glb_SerialNum->Location = System::Drawing::Point(159, 9);
      this->label_Glb_SerialNum->Name = L"label_Glb_SerialNum";
      this->label_Glb_SerialNum->Size = System::Drawing::Size(46, 13);
      this->label_Glb_SerialNum->TabIndex = 2;
      this->label_Glb_SerialNum->Text = L"Serial #:";
      // 
      // label_Glb_SoftwareVerDisplay
      // 
      this->label_Glb_SoftwareVerDisplay->AutoSize = true;
      this->label_Glb_SoftwareVerDisplay->Location = System::Drawing::Point(160, 43);
      this->label_Glb_SoftwareVerDisplay->Name = L"label_Glb_SoftwareVerDisplay";
      this->label_Glb_SoftwareVerDisplay->Size = System::Drawing::Size(16, 13);
      this->label_Glb_SoftwareVerDisplay->TabIndex = 16;
      this->label_Glb_SoftwareVerDisplay->Text = L"---";
      // 
      // label_Glb_MfgID
      // 
      this->label_Glb_MfgID->AutoSize = true;
      this->label_Glb_MfgID->Location = System::Drawing::Point(252, 60);
      this->label_Glb_MfgID->Name = L"label_Glb_MfgID";
      this->label_Glb_MfgID->Size = System::Drawing::Size(51, 13);
      this->label_Glb_MfgID->TabIndex = 1;
      this->label_Glb_MfgID->Text = L"Manf. ID:";
      // 
      // label_Glb_SoftwareVer
      // 
      this->label_Glb_SoftwareVer->AutoSize = true;
      this->label_Glb_SoftwareVer->Location = System::Drawing::Point(83, 43);
      this->label_Glb_SoftwareVer->Name = L"label_Glb_SoftwareVer";
      this->label_Glb_SoftwareVer->Size = System::Drawing::Size(71, 13);
      this->label_Glb_SoftwareVer->TabIndex = 4;
      this->label_Glb_SoftwareVer->Text = L"Software Ver:";
      // 
      // label_Glb_HardwareVer
      // 
      this->label_Glb_HardwareVer->AutoSize = true;
      this->label_Glb_HardwareVer->Location = System::Drawing::Point(79, 26);
      this->label_Glb_HardwareVer->Name = L"label_Glb_HardwareVer";
      this->label_Glb_HardwareVer->Size = System::Drawing::Size(75, 13);
      this->label_Glb_HardwareVer->TabIndex = 3;
      this->label_Glb_HardwareVer->Text = L"Hardware Ver:";
      // 
      // label_Glb_HardwareVerDisplay
      // 
      this->label_Glb_HardwareVerDisplay->AutoSize = true;
      this->label_Glb_HardwareVerDisplay->Location = System::Drawing::Point(160, 26);
      this->label_Glb_HardwareVerDisplay->Name = L"label_Glb_HardwareVerDisplay";
      this->label_Glb_HardwareVerDisplay->Size = System::Drawing::Size(16, 13);
      this->label_Glb_HardwareVerDisplay->TabIndex = 14;
      this->label_Glb_HardwareVerDisplay->Text = L"---";
      // 
      // label_Glb_ModelNumDisplay
      // 
      this->label_Glb_ModelNumDisplay->AutoSize = true;
      this->label_Glb_ModelNumDisplay->Location = System::Drawing::Point(309, 26);
      this->label_Glb_ModelNumDisplay->Name = L"label_Glb_ModelNumDisplay";
      this->label_Glb_ModelNumDisplay->Size = System::Drawing::Size(16, 13);
      this->label_Glb_ModelNumDisplay->TabIndex = 9;
      this->label_Glb_ModelNumDisplay->Text = L"---";
      // 
      // label_Glb_SerialNumDisplay
      // 
      this->label_Glb_SerialNumDisplay->AutoSize = true;
      this->label_Glb_SerialNumDisplay->Location = System::Drawing::Point(211, 9);
      this->label_Glb_SerialNumDisplay->Name = L"label_Glb_SerialNumDisplay";
      this->label_Glb_SerialNumDisplay->Size = System::Drawing::Size(16, 13);
      this->label_Glb_SerialNumDisplay->TabIndex = 11;
      this->label_Glb_SerialNumDisplay->Text = L"---";
      // 
      // label_Glb_MfgIDDisplay
      // 
      this->label_Glb_MfgIDDisplay->AutoSize = true;
      this->label_Glb_MfgIDDisplay->Location = System::Drawing::Point(309, 60);
      this->label_Glb_MfgIDDisplay->Name = L"label_Glb_MfgIDDisplay";
      this->label_Glb_MfgIDDisplay->Size = System::Drawing::Size(16, 13);
      this->label_Glb_MfgIDDisplay->TabIndex = 8;
      this->label_Glb_MfgIDDisplay->Text = L"---";
      // 
      // label_Calc_TotEventCount
      // 
      this->label_Calc_TotEventCount->AutoSize = true;
      this->label_Calc_TotEventCount->Location = System::Drawing::Point(61, 86);
      this->label_Calc_TotEventCount->Name = L"label_Calc_TotEventCount";
      this->label_Calc_TotEventCount->Size = System::Drawing::Size(96, 13);
      this->label_Calc_TotEventCount->TabIndex = 0;
      this->label_Calc_TotEventCount->Text = L"Total Event Count:";
      // 
      // tabPage2
      // 
      this->tabPage2->Controls->Add(this->button_Req_Page82);
      this->tabPage2->Controls->Add(this->label_AckMsgStatus);
      this->tabPage2->Controls->Add(this->button_Req_Page81);
      this->tabPage2->Controls->Add(this->button_Req_Page80);
      this->tabPage2->Controls->Add(this->numericUpDown_Req_Copies);
      this->tabPage2->Controls->Add(this->label_Req_Copies);
      this->tabPage2->Location = System::Drawing::Point(4, 22);
      this->tabPage2->Name = L"tabPage2";
      this->tabPage2->Padding = System::Windows::Forms::Padding(3);
      this->tabPage2->Size = System::Drawing::Size(392, 140);
      this->tabPage2->TabIndex = 1;
      this->tabPage2->Text = L"Request";
      this->tabPage2->UseVisualStyleBackColor = true;
      // 
      // button_Req_Page82
      // 
      this->button_Req_Page82->Location = System::Drawing::Point(95, 75);
      this->button_Req_Page82->Name = L"button_Req_Page82";
      this->button_Req_Page82->Size = System::Drawing::Size(75, 23);
      this->button_Req_Page82->TabIndex = 8;
      this->button_Req_Page82->Text = L"Page 82";
      this->button_Req_Page82->UseVisualStyleBackColor = true;
      this->button_Req_Page82->Click += gcnew System::EventHandler(this, &TemperatureDisplay::button_Req_Page82_Click);
      // 
      // label_AckMsgStatus
      // 
      this->label_AckMsgStatus->AutoSize = true;
      this->label_AckMsgStatus->Location = System::Drawing::Point(198, 85);
      this->label_AckMsgStatus->Name = L"label_AckMsgStatus";
      this->label_AckMsgStatus->Size = System::Drawing::Size(89, 13);
      this->label_AckMsgStatus->TabIndex = 7;
      this->label_AckMsgStatus->Text = L"Request Status...";
      // 
      // button_Req_Page81
      // 
      this->button_Req_Page81->Location = System::Drawing::Point(95, 46);
      this->button_Req_Page81->Name = L"button_Req_Page81";
      this->button_Req_Page81->Size = System::Drawing::Size(75, 23);
      this->button_Req_Page81->TabIndex = 6;
      this->button_Req_Page81->Text = L"Page 81";
      this->button_Req_Page81->UseVisualStyleBackColor = true;
      this->button_Req_Page81->Click += gcnew System::EventHandler(this, &TemperatureDisplay::button_Req_Page81_Click);
      // 
      // button_Req_Page80
      // 
      this->button_Req_Page80->Location = System::Drawing::Point(95, 18);
      this->button_Req_Page80->Name = L"button_Req_Page80";
      this->button_Req_Page80->Size = System::Drawing::Size(75, 23);
      this->button_Req_Page80->TabIndex = 5;
      this->button_Req_Page80->Text = L"Page 80";
      this->button_Req_Page80->UseVisualStyleBackColor = true;
      this->button_Req_Page80->Click += gcnew System::EventHandler(this, &TemperatureDisplay::button_Req_Page80_Click);
      // 
      // numericUpDown_Req_Copies
      // 
      this->numericUpDown_Req_Copies->Location = System::Drawing::Point(212, 46);
      this->numericUpDown_Req_Copies->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {127, 0, 0, 0});
      this->numericUpDown_Req_Copies->Name = L"numericUpDown_Req_Copies";
      this->numericUpDown_Req_Copies->Size = System::Drawing::Size(75, 20);
      this->numericUpDown_Req_Copies->TabIndex = 4;
      this->numericUpDown_Req_Copies->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
      this->numericUpDown_Req_Copies->ValueChanged += gcnew System::EventHandler(this, &TemperatureDisplay::numericUpDown_Req_Copies_ValueChaned);
      // 
      // label_Req_Copies
      // 
      this->label_Req_Copies->AutoSize = true;
      this->label_Req_Copies->Location = System::Drawing::Point(200, 23);
      this->label_Req_Copies->Name = L"label_Req_Copies";
      this->label_Req_Copies->Size = System::Drawing::Size(97, 13);
      this->label_Req_Copies->TabIndex = 3;
      this->label_Req_Copies->Text = L"Copies to Request:";
      // 
      // tabPage3
      // 
      this->tabPage3->Controls->Add(this->labelBattStatus);
      this->tabPage3->Controls->Add(this->label68);
      this->tabPage3->Controls->Add(this->label67);
      this->tabPage3->Controls->Add(this->labelBattVolt);
      this->tabPage3->Controls->Add(this->label63);
      this->tabPage3->Controls->Add(this->label62);
      this->tabPage3->Controls->Add(this->labelTimeRes);
      this->tabPage3->Controls->Add(this->labelOpTime);
      this->tabPage3->Location = System::Drawing::Point(4, 22);
      this->tabPage3->Name = L"tabPage3";
      this->tabPage3->Size = System::Drawing::Size(392, 140);
      this->tabPage3->TabIndex = 2;
      this->tabPage3->Text = L"Battery Status";
      this->tabPage3->UseVisualStyleBackColor = true;
      // 
      // labelBattStatus
      // 
      this->labelBattStatus->AutoSize = true;
      this->labelBattStatus->Location = System::Drawing::Point(312, 64);
      this->labelBattStatus->Name = L"labelBattStatus";
      this->labelBattStatus->Size = System::Drawing::Size(16, 13);
      this->labelBattStatus->TabIndex = 88;
      this->labelBattStatus->Text = L"---";
      // 
      // label68
      // 
      this->label68->AutoSize = true;
      this->label68->Location = System::Drawing::Point(68, 64);
      this->label68->Name = L"label68";
      this->label68->Size = System::Drawing::Size(82, 13);
      this->label68->TabIndex = 81;
      this->label68->Text = L"Operating Time:";
      // 
      // label67
      // 
      this->label67->AutoSize = true;
      this->label67->Location = System::Drawing::Point(224, 38);
      this->label67->Name = L"label67";
      this->label67->Size = System::Drawing::Size(82, 13);
      this->label67->TabIndex = 82;
      this->label67->Text = L"Battery Voltage:";
      // 
      // labelBattVolt
      // 
      this->labelBattVolt->AutoSize = true;
      this->labelBattVolt->Location = System::Drawing::Point(312, 38);
      this->labelBattVolt->Name = L"labelBattVolt";
      this->labelBattVolt->Size = System::Drawing::Size(16, 13);
      this->labelBattVolt->TabIndex = 87;
      this->labelBattVolt->Text = L"---";
      // 
      // label63
      // 
      this->label63->AutoSize = true;
      this->label63->Location = System::Drawing::Point(230, 64);
      this->label63->Name = L"label63";
      this->label63->Size = System::Drawing::Size(76, 13);
      this->label63->TabIndex = 83;
      this->label63->Text = L"Battery Status:";
      // 
      // label62
      // 
      this->label62->AutoSize = true;
      this->label62->Location = System::Drawing::Point(64, 38);
      this->label62->Name = L"label62";
      this->label62->Size = System::Drawing::Size(86, 13);
      this->label62->TabIndex = 84;
      this->label62->Text = L"Time Resolution:";
      // 
      // labelTimeRes
      // 
      this->labelTimeRes->AutoSize = true;
      this->labelTimeRes->Location = System::Drawing::Point(156, 38);
      this->labelTimeRes->Name = L"labelTimeRes";
      this->labelTimeRes->Size = System::Drawing::Size(16, 13);
      this->labelTimeRes->TabIndex = 86;
      this->labelTimeRes->Text = L"---";
      // 
      // labelOpTime
      // 
      this->labelOpTime->AutoSize = true;
      this->labelOpTime->Location = System::Drawing::Point(156, 64);
      this->labelOpTime->Name = L"labelOpTime";
      this->labelOpTime->Size = System::Drawing::Size(16, 13);
      this->labelOpTime->TabIndex = 85;
      this->labelOpTime->Text = L"---";
      // 
      // label_LocalTime_Display
      // 
      this->label_LocalTime_Display->AutoSize = true;
      this->label_LocalTime_Display->Location = System::Drawing::Point(163, 124);
      this->label_LocalTime_Display->Name = L"label_LocalTime_Display";
      this->label_LocalTime_Display->Size = System::Drawing::Size(16, 13);
      this->label_LocalTime_Display->TabIndex = 28;
      this->label_LocalTime_Display->Text = L"---";
      // 
      // label_LocalTime
      // 
      this->label_LocalTime->AutoSize = true;
      this->label_LocalTime->Location = System::Drawing::Point(95, 124);
      this->label_LocalTime->Name = L"label_LocalTime";
      this->label_LocalTime->Size = System::Drawing::Size(62, 13);
      this->label_LocalTime->TabIndex = 27;
      this->label_LocalTime->Text = L"Local Time:";
      // 
      // TemperatureDisplay
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(794, 351);
      this->Controls->Add(this->panel_Display);
      this->Controls->Add(this->panel_Settings);
      this->Name = L"TemperatureDisplay";
      this->Text = L"TemperatureDisplay";
      this->panel_Display->ResumeLayout(false);
      this->panel_Display->PerformLayout();
      this->panel_Settings->ResumeLayout(false);
      this->tabControl1->ResumeLayout(false);
      this->tabPage1->ResumeLayout(false);
      this->tabPage1->PerformLayout();
      this->groupBox_RxPeriod->ResumeLayout(false);
      this->groupBox_RxPeriod->PerformLayout();
      this->groupBox_GlobalData->ResumeLayout(false);
      this->groupBox_GlobalData->PerformLayout();
      this->tabPage2->ResumeLayout(false);
      this->tabPage2->PerformLayout();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Req_Copies))->EndInit();
      this->tabPage3->ResumeLayout(false);
      this->tabPage3->PerformLayout();
      this->ResumeLayout(false);

   }
#pragma endregion
};