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

public ref class GeocacheDisplay : public System::Windows::Forms::Form, public ISimBase{
public:

		GeocacheDisplay(System::Timers::Timer^ channelTimer, dRequestAckMsg^ channelAckMsg,  dRequestBcastMsg^ channelBcastMsg){
			InitializeComponent();
			GeocacheData = gcnew Geocache();
			commonData = gcnew CommonData();
			requestAckMsg = channelAckMsg;
			requestBcastMsg = channelBcastMsg;
			timerHandle = channelTimer;		
			InitializeSim();
		}

		~GeocacheDisplay(){
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
	virtual UCHAR getTransmissionType(){return GeocacheData->TX_TYPE_DISPLAY;}
	virtual USHORT getTransmitPeriod(){return GeocacheData->MESG_4HZ_PERIOD;}
	virtual DOUBLE getTimerInterval(){return (double) ulTimerInterval;}
	virtual System::Windows::Forms::Panel^ getSimSettingsPanel(){return this->panel_Settings;}
	virtual System::Windows::Forms::Panel^ getSimTranslatedDisplay(){return this->panel_Display;}


private:
	void InitializeSim();							// Initialize simulation data variables
	void HandleTransmit(UCHAR* pucTxBuffer_);		// Handles the data page transmissions 
	void HandleReceive(UCHAR* pucRxBuffer_);		// Handles page requests 
	void Authentication(UCHAR* pucRxBuffer_);		// Handle Authentication request
	UCHAR GeocacheDisplay::NextUnReadPage();
	void UpdateLoggedVisit();
	void UpdateDisplay();
	void UpdateAckStatus(UCHAR* pucRxBuffer_);
	void UpdateDisplayAckStatus(UCHAR status_);
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
	System::Void numericUpDownDay_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDownMonth_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDownYear_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDownHours_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDownMinutes_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void numericUpDownSeconds_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void buttonProgram_Click(System::Object^  sender, System::EventArgs^  e);
	System::Void buttonErase_Click(System::Object^  sender, System::EventArgs^  e);
	System::Void buttonForget_Click(System::Object^  sender, System::EventArgs^  e);
private:
	dRequestAckMsg^ requestAckMsg;
	dRequestBcastMsg^ requestBcastMsg;
	Geocache^ GeocacheData;                       // Geocache class variable
	CommonData^ commonData;
	System::Timers::Timer^ timerHandle;	// Timer handle
	ULONG  ulTimerInterval;				   // Timer interval between simulated events
	ULONG ulTotalTime;                  // used to track the on time for the batt page  
	
	// Simulation Variables
	double dbDispAcumTime;  // Cumulative time (s)
	double dbDispAcumDist;  // Cumulative distance (m)
	USHORT usLastEventTime; // used to help calculate distance on each event

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
private: System::Windows::Forms::GroupBox^  groupBox3;
private: System::Windows::Forms::GroupBox^  groupBox4;
private: System::Windows::Forms::GroupBox^  groupBox5;
private: System::Windows::Forms::GroupBox^  groupBox6;
private: System::Windows::Forms::TabPage^  tabPage10;
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
private: System::Windows::Forms::TabPage^  tabPage7;
private: System::Windows::Forms::CheckBox^  checkBox1;
private: System::Windows::Forms::GroupBox^  groupBox7;
private: System::Windows::Forms::CheckBox^  checkBox2;
private: System::Windows::Forms::Label^  label6;
private: System::Windows::Forms::TextBox^  textBox1;
private: System::Windows::Forms::Label^  label18;
private: System::Windows::Forms::Label^  label23;
private: System::Windows::Forms::GroupBox^  groupBox8;
private: System::Windows::Forms::CheckBox^  checkBox3;
private: System::Windows::Forms::TextBox^  textBox2;
private: System::Windows::Forms::Label^  label24;
private: System::Windows::Forms::TabPage^  tabPage8;
private: System::Windows::Forms::TabControl^  tabControl5;
private: System::Windows::Forms::TabPage^  tabPage9;
private: System::Windows::Forms::Button^  button1;
private: System::Windows::Forms::TabPage^  tabPage15;
private: System::Windows::Forms::GroupBox^  groupBox9;
private: System::Windows::Forms::CheckBox^  checkBox4;
private: System::Windows::Forms::NumericUpDown^  numericUpDown1;
private: System::Windows::Forms::TabPage^  tabPage16;
private: System::Windows::Forms::GroupBox^  groupBox10;
private: System::Windows::Forms::CheckBox^  checkBox5;
private: System::Windows::Forms::NumericUpDown^  numericUpDown2;
private: System::Windows::Forms::TabPage^  tabPage17;
private: System::Windows::Forms::GroupBox^  groupBox11;
private: System::Windows::Forms::TextBox^  textBox3;
private: System::Windows::Forms::CheckBox^  checkBox6;
private: System::Windows::Forms::TabPage^  tabPage18;
private: System::Windows::Forms::GroupBox^  groupBox12;
private: System::Windows::Forms::Label^  label25;
private: System::Windows::Forms::Label^  label26;
private: System::Windows::Forms::Label^  label27;
private: System::Windows::Forms::NumericUpDown^  numericUpDown3;
private: System::Windows::Forms::NumericUpDown^  numericUpDown4;
private: System::Windows::Forms::Label^  label37;
private: System::Windows::Forms::NumericUpDown^  numericUpDown5;
private: System::Windows::Forms::Label^  label38;
private: System::Windows::Forms::NumericUpDown^  numericUpDown6;
private: System::Windows::Forms::NumericUpDown^  numericUpDown7;
private: System::Windows::Forms::NumericUpDown^  numericUpDown8;
private: System::Windows::Forms::Label^  label39;
private: System::Windows::Forms::NumericUpDown^  numericUpDown9;
private: System::Windows::Forms::Label^  label40;
private: System::Windows::Forms::Label^  label41;
private: System::Windows::Forms::CheckBox^  checkBox7;
private: System::Windows::Forms::TabPage^  tabPage19;
private: System::Windows::Forms::Label^  label42;
private: System::Windows::Forms::Label^  label43;
private: System::Windows::Forms::TabPage^  tabPage20;
private: System::Windows::Forms::TabControl^  tabControl6;
private: System::Windows::Forms::TabPage^  tabPage21;
private: System::Windows::Forms::Button^  button2;
private: System::Windows::Forms::TextBox^  textBox4;
private: System::Windows::Forms::TextBox^  textBox5;
private: System::Windows::Forms::TextBox^  textBox6;
private: System::Windows::Forms::TextBox^  textBox7;
private: System::Windows::Forms::TextBox^  textBox8;
private: System::Windows::Forms::Label^  label44;
private: System::Windows::Forms::Label^  label45;
private: System::Windows::Forms::Label^  label46;
private: System::Windows::Forms::Label^  label47;
private: System::Windows::Forms::Label^  label48;
private: System::Windows::Forms::TabPage^  tabPage22;
private: System::Windows::Forms::Label^  label49;
private: System::Windows::Forms::Label^  label50;
private: System::Windows::Forms::ComboBox^  comboBox1;
private: System::Windows::Forms::Button^  button3;
private: System::Windows::Forms::NumericUpDown^  numericUpDown10;
private: System::Windows::Forms::Label^  label51;
private: System::Windows::Forms::NumericUpDown^  numericUpDown11;
private: System::Windows::Forms::GroupBox^  groupBox13;
private: System::Windows::Forms::RadioButton^  radioButton1;
private: System::Windows::Forms::RadioButton^  radioButton2;
private: System::Windows::Forms::CheckBox^  checkBox8;
private: System::Windows::Forms::TextBox^  textBox9;
private: System::Windows::Forms::Label^  label52;
private: System::Windows::Forms::Label^  label53;
private: System::Windows::Forms::CheckBox^  checkBox9;
private: System::Windows::Forms::TabPage^  tabPage23;
private: System::Windows::Forms::GroupBox^  groupBox2;
private: System::Windows::Forms::CheckBox^  checkBoxPIN;
private: System::Windows::Forms::TextBox^  textBoxPIN;
private: System::Windows::Forms::Label^  label2;
private: System::Windows::Forms::Label^  label22;
private: System::Windows::Forms::GroupBox^  groupBox1;
private: System::Windows::Forms::CheckBox^  checkBoxID;
private: System::Windows::Forms::TextBox^  textBoxID;
private: System::Windows::Forms::Label^  label1;
private: System::Windows::Forms::Label^  labelBattStatus;
private: System::Windows::Forms::Label^  labelBattVolt;
private: System::Windows::Forms::Label^  labelTimeRes;
private: System::Windows::Forms::Label^  labelOpTime;
private: System::Windows::Forms::Label^  label62;
private: System::Windows::Forms::Label^  label63;
private: System::Windows::Forms::Label^  label67;
private: System::Windows::Forms::Label^  label68;
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
private: System::Windows::Forms::Label^  labelGCdHint;
private: System::Windows::Forms::Label^  label70;
private: System::Windows::Forms::Label^  labelGCdPIN;
private: System::Windows::Forms::Label^  label72;
private: System::Windows::Forms::Label^  labelGCdID;
private: System::Windows::Forms::Label^  label74;
private: System::Windows::Forms::Label^  labelGCdNumVisits;
private: System::Windows::Forms::Label^  label76;
private: System::Windows::Forms::Label^  labelGCdLon;
private: System::Windows::Forms::Label^  label78;
private: System::Windows::Forms::Label^  labelGCdLat;
private: System::Windows::Forms::Label^  label80;
private: System::Windows::Forms::Label^  labelGCdTimestamp;
private: System::Windows::Forms::Label^  label82;
private: System::Windows::Forms::Label^  labelGCdTotNumPages;
private: System::Windows::Forms::Label^  label71;
private: System::Windows::Forms::Label^  labelProgStatus;
private: System::Windows::Forms::Button^  buttonProgram;
private: System::Windows::Forms::Label^  label19;
private: System::Windows::Forms::Button^  buttonErase;
private: System::Windows::Forms::Label^  labelGCdPagesRecvd;
private: System::Windows::Forms::Label^  label54;
private: System::Windows::Forms::Label^  label56;
private: System::Windows::Forms::Label^  label20;
private: System::Windows::Forms::Label^  labelProgTotalPages;
private: System::Windows::Forms::Label^  labelCurrProgPage;
private: System::Windows::Forms::Button^  buttonForget;
private: System::Windows::Forms::Label^  label61;
private: System::Windows::Forms::Label^  label60;
private: System::Windows::Forms::Label^  labelAuthNonce;
private: System::Windows::Forms::Label^  label59;
private: System::Windows::Forms::Label^  labelAuthSerialNum;
private: System::Windows::Forms::Label^  label57;
private: System::Windows::Forms::TabControl^  tabControl2;
private: System::Windows::Forms::TabPage^  tabPage5;
private: System::Windows::Forms::TabPage^  tabPage6;


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
			this->buttonForget = (gcnew System::Windows::Forms::Button());
			this->label54 = (gcnew System::Windows::Forms::Label());
			this->labelGCdPagesRecvd = (gcnew System::Windows::Forms::Label());
			this->labelGCdTotNumPages = (gcnew System::Windows::Forms::Label());
			this->label71 = (gcnew System::Windows::Forms::Label());
			this->labelGCdHint = (gcnew System::Windows::Forms::Label());
			this->label70 = (gcnew System::Windows::Forms::Label());
			this->labelGCdPIN = (gcnew System::Windows::Forms::Label());
			this->label72 = (gcnew System::Windows::Forms::Label());
			this->labelGCdID = (gcnew System::Windows::Forms::Label());
			this->label74 = (gcnew System::Windows::Forms::Label());
			this->labelGCdNumVisits = (gcnew System::Windows::Forms::Label());
			this->label76 = (gcnew System::Windows::Forms::Label());
			this->labelGCdLon = (gcnew System::Windows::Forms::Label());
			this->label78 = (gcnew System::Windows::Forms::Label());
			this->labelGCdLat = (gcnew System::Windows::Forms::Label());
			this->label80 = (gcnew System::Windows::Forms::Label());
			this->labelGCdTimestamp = (gcnew System::Windows::Forms::Label());
			this->label82 = (gcnew System::Windows::Forms::Label());
			this->tabPage2 = (gcnew System::Windows::Forms::TabPage());
			this->label61 = (gcnew System::Windows::Forms::Label());
			this->label60 = (gcnew System::Windows::Forms::Label());
			this->labelAuthNonce = (gcnew System::Windows::Forms::Label());
			this->label59 = (gcnew System::Windows::Forms::Label());
			this->labelAuthSerialNum = (gcnew System::Windows::Forms::Label());
			this->label57 = (gcnew System::Windows::Forms::Label());
			this->labelAuthToken = (gcnew System::Windows::Forms::Label());
			this->label36 = (gcnew System::Windows::Forms::Label());
			this->tabPage4 = (gcnew System::Windows::Forms::TabPage());
			this->labelBattStatus = (gcnew System::Windows::Forms::Label());
			this->labelBattVolt = (gcnew System::Windows::Forms::Label());
			this->labelTimeRes = (gcnew System::Windows::Forms::Label());
			this->labelOpTime = (gcnew System::Windows::Forms::Label());
			this->label62 = (gcnew System::Windows::Forms::Label());
			this->label63 = (gcnew System::Windows::Forms::Label());
			this->label67 = (gcnew System::Windows::Forms::Label());
			this->label68 = (gcnew System::Windows::Forms::Label());
			this->label_Glb_SoftwareVer = (gcnew System::Windows::Forms::Label());
			this->label_Glb_HardwareVer = (gcnew System::Windows::Forms::Label());
			this->label_Glb_SoftwareVerDisplay = (gcnew System::Windows::Forms::Label());
			this->label_Glb_SerialNumDisplay = (gcnew System::Windows::Forms::Label());
			this->label_Glb_ModelNum = (gcnew System::Windows::Forms::Label());
			this->label_Glb_ManfIDDisplay = (gcnew System::Windows::Forms::Label());
			this->label_Glb_SerialNum = (gcnew System::Windows::Forms::Label());
			this->label_Glb_ModelNumDisplay = (gcnew System::Windows::Forms::Label());
			this->label_Glb_HardwareVerDisplay = (gcnew System::Windows::Forms::Label());
			this->label_Glb_ManfID = (gcnew System::Windows::Forms::Label());
			this->tabPage3 = (gcnew System::Windows::Forms::TabPage());
			this->tabControl3 = (gcnew System::Windows::Forms::TabControl());
			this->tabPage10 = (gcnew System::Windows::Forms::TabPage());
			this->labelCurrProgPage = (gcnew System::Windows::Forms::Label());
			this->label56 = (gcnew System::Windows::Forms::Label());
			this->label20 = (gcnew System::Windows::Forms::Label());
			this->labelProgTotalPages = (gcnew System::Windows::Forms::Label());
			this->label19 = (gcnew System::Windows::Forms::Label());
			this->buttonErase = (gcnew System::Windows::Forms::Button());
			this->labelProgStatus = (gcnew System::Windows::Forms::Label());
			this->buttonProgram = (gcnew System::Windows::Forms::Button());
			this->tabPage23 = (gcnew System::Windows::Forms::TabPage());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->checkBoxPIN = (gcnew System::Windows::Forms::CheckBox());
			this->textBoxPIN = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label22 = (gcnew System::Windows::Forms::Label());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->checkBoxID = (gcnew System::Windows::Forms::CheckBox());
			this->textBoxID = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
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
			this->tabPage7 = (gcnew System::Windows::Forms::TabPage());
			this->checkBox1 = (gcnew System::Windows::Forms::CheckBox());
			this->groupBox7 = (gcnew System::Windows::Forms::GroupBox());
			this->checkBox2 = (gcnew System::Windows::Forms::CheckBox());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->label18 = (gcnew System::Windows::Forms::Label());
			this->label23 = (gcnew System::Windows::Forms::Label());
			this->groupBox8 = (gcnew System::Windows::Forms::GroupBox());
			this->checkBox3 = (gcnew System::Windows::Forms::CheckBox());
			this->textBox2 = (gcnew System::Windows::Forms::TextBox());
			this->label24 = (gcnew System::Windows::Forms::Label());
			this->tabPage8 = (gcnew System::Windows::Forms::TabPage());
			this->tabControl5 = (gcnew System::Windows::Forms::TabControl());
			this->tabPage9 = (gcnew System::Windows::Forms::TabPage());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->tabPage15 = (gcnew System::Windows::Forms::TabPage());
			this->groupBox9 = (gcnew System::Windows::Forms::GroupBox());
			this->checkBox4 = (gcnew System::Windows::Forms::CheckBox());
			this->numericUpDown1 = (gcnew System::Windows::Forms::NumericUpDown());
			this->tabPage16 = (gcnew System::Windows::Forms::TabPage());
			this->groupBox10 = (gcnew System::Windows::Forms::GroupBox());
			this->checkBox5 = (gcnew System::Windows::Forms::CheckBox());
			this->numericUpDown2 = (gcnew System::Windows::Forms::NumericUpDown());
			this->tabPage17 = (gcnew System::Windows::Forms::TabPage());
			this->groupBox11 = (gcnew System::Windows::Forms::GroupBox());
			this->textBox3 = (gcnew System::Windows::Forms::TextBox());
			this->checkBox6 = (gcnew System::Windows::Forms::CheckBox());
			this->tabPage18 = (gcnew System::Windows::Forms::TabPage());
			this->groupBox12 = (gcnew System::Windows::Forms::GroupBox());
			this->label25 = (gcnew System::Windows::Forms::Label());
			this->label26 = (gcnew System::Windows::Forms::Label());
			this->label27 = (gcnew System::Windows::Forms::Label());
			this->numericUpDown3 = (gcnew System::Windows::Forms::NumericUpDown());
			this->numericUpDown4 = (gcnew System::Windows::Forms::NumericUpDown());
			this->label37 = (gcnew System::Windows::Forms::Label());
			this->numericUpDown5 = (gcnew System::Windows::Forms::NumericUpDown());
			this->label38 = (gcnew System::Windows::Forms::Label());
			this->numericUpDown6 = (gcnew System::Windows::Forms::NumericUpDown());
			this->numericUpDown7 = (gcnew System::Windows::Forms::NumericUpDown());
			this->numericUpDown8 = (gcnew System::Windows::Forms::NumericUpDown());
			this->label39 = (gcnew System::Windows::Forms::Label());
			this->numericUpDown9 = (gcnew System::Windows::Forms::NumericUpDown());
			this->label40 = (gcnew System::Windows::Forms::Label());
			this->label41 = (gcnew System::Windows::Forms::Label());
			this->checkBox7 = (gcnew System::Windows::Forms::CheckBox());
			this->tabPage19 = (gcnew System::Windows::Forms::TabPage());
			this->label42 = (gcnew System::Windows::Forms::Label());
			this->label43 = (gcnew System::Windows::Forms::Label());
			this->tabPage20 = (gcnew System::Windows::Forms::TabPage());
			this->tabControl6 = (gcnew System::Windows::Forms::TabControl());
			this->tabPage21 = (gcnew System::Windows::Forms::TabPage());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->textBox4 = (gcnew System::Windows::Forms::TextBox());
			this->textBox5 = (gcnew System::Windows::Forms::TextBox());
			this->textBox6 = (gcnew System::Windows::Forms::TextBox());
			this->textBox7 = (gcnew System::Windows::Forms::TextBox());
			this->textBox8 = (gcnew System::Windows::Forms::TextBox());
			this->label44 = (gcnew System::Windows::Forms::Label());
			this->label45 = (gcnew System::Windows::Forms::Label());
			this->label46 = (gcnew System::Windows::Forms::Label());
			this->label47 = (gcnew System::Windows::Forms::Label());
			this->label48 = (gcnew System::Windows::Forms::Label());
			this->tabPage22 = (gcnew System::Windows::Forms::TabPage());
			this->label49 = (gcnew System::Windows::Forms::Label());
			this->label50 = (gcnew System::Windows::Forms::Label());
			this->comboBox1 = (gcnew System::Windows::Forms::ComboBox());
			this->button3 = (gcnew System::Windows::Forms::Button());
			this->numericUpDown10 = (gcnew System::Windows::Forms::NumericUpDown());
			this->label51 = (gcnew System::Windows::Forms::Label());
			this->numericUpDown11 = (gcnew System::Windows::Forms::NumericUpDown());
			this->groupBox13 = (gcnew System::Windows::Forms::GroupBox());
			this->radioButton1 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButton2 = (gcnew System::Windows::Forms::RadioButton());
			this->checkBox8 = (gcnew System::Windows::Forms::CheckBox());
			this->textBox9 = (gcnew System::Windows::Forms::TextBox());
			this->label52 = (gcnew System::Windows::Forms::Label());
			this->label53 = (gcnew System::Windows::Forms::Label());
			this->checkBox9 = (gcnew System::Windows::Forms::CheckBox());
			this->tabControl2 = (gcnew System::Windows::Forms::TabControl());
			this->tabPage5 = (gcnew System::Windows::Forms::TabPage());
			this->tabPage6 = (gcnew System::Windows::Forms::TabPage());
			this->panel_Settings->SuspendLayout();
			this->tabControl1->SuspendLayout();
			this->tabPage1->SuspendLayout();
			this->tabPage2->SuspendLayout();
			this->tabPage4->SuspendLayout();
			this->tabPage3->SuspendLayout();
			this->tabControl3->SuspendLayout();
			this->tabPage10->SuspendLayout();
			this->tabPage23->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->groupBox1->SuspendLayout();
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
			this->panel_Display->SuspendLayout();
			this->tabPage7->SuspendLayout();
			this->groupBox7->SuspendLayout();
			this->groupBox8->SuspendLayout();
			this->tabPage8->SuspendLayout();
			this->tabControl5->SuspendLayout();
			this->tabPage9->SuspendLayout();
			this->tabPage15->SuspendLayout();
			this->groupBox9->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown1))->BeginInit();
			this->tabPage16->SuspendLayout();
			this->groupBox10->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown2))->BeginInit();
			this->tabPage17->SuspendLayout();
			this->groupBox11->SuspendLayout();
			this->tabPage18->SuspendLayout();
			this->groupBox12->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown3))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown4))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown5))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown6))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown7))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown8))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown9))->BeginInit();
			this->tabPage19->SuspendLayout();
			this->tabPage20->SuspendLayout();
			this->tabControl6->SuspendLayout();
			this->tabPage21->SuspendLayout();
			this->tabPage22->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown10))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown11))->BeginInit();
			this->groupBox13->SuspendLayout();
			this->tabControl2->SuspendLayout();
			this->tabPage5->SuspendLayout();
			this->tabPage6->SuspendLayout();
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
			this->tabControl1->Controls->Add(this->tabPage2);
			this->tabControl1->Controls->Add(this->tabPage4);
			this->tabControl1->Controls->Add(this->tabPage3);
			this->tabControl1->Location = System::Drawing::Point(5, 3);
			this->tabControl1->Name = L"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(392, 160);
			this->tabControl1->TabIndex = 6;
			// 
			// tabPage1
			// 
			this->tabPage1->Controls->Add(this->buttonForget);
			this->tabPage1->Controls->Add(this->label54);
			this->tabPage1->Controls->Add(this->labelGCdPagesRecvd);
			this->tabPage1->Controls->Add(this->labelGCdTotNumPages);
			this->tabPage1->Controls->Add(this->label71);
			this->tabPage1->Controls->Add(this->labelGCdHint);
			this->tabPage1->Controls->Add(this->label70);
			this->tabPage1->Controls->Add(this->labelGCdPIN);
			this->tabPage1->Controls->Add(this->label72);
			this->tabPage1->Controls->Add(this->labelGCdID);
			this->tabPage1->Controls->Add(this->label74);
			this->tabPage1->Controls->Add(this->labelGCdNumVisits);
			this->tabPage1->Controls->Add(this->label76);
			this->tabPage1->Controls->Add(this->labelGCdLon);
			this->tabPage1->Controls->Add(this->label78);
			this->tabPage1->Controls->Add(this->labelGCdLat);
			this->tabPage1->Controls->Add(this->label80);
			this->tabPage1->Controls->Add(this->labelGCdTimestamp);
			this->tabPage1->Controls->Add(this->label82);
			this->tabPage1->Location = System::Drawing::Point(4, 22);
			this->tabPage1->Name = L"tabPage1";
			this->tabPage1->Padding = System::Windows::Forms::Padding(3);
			this->tabPage1->Size = System::Drawing::Size(384, 134);
			this->tabPage1->TabIndex = 0;
			this->tabPage1->Text = L"Geocache Data";
			this->tabPage1->UseVisualStyleBackColor = true;
			// 
			// buttonForget
			// 
			this->buttonForget->Location = System::Drawing::Point(185, 11);
			this->buttonForget->Name = L"buttonForget";
			this->buttonForget->Size = System::Drawing::Size(126, 23);
			this->buttonForget->TabIndex = 64;
			this->buttonForget->Text = L"Forget this Geocache";
			this->buttonForget->UseVisualStyleBackColor = true;
			this->buttonForget->Click += gcnew System::EventHandler(this, &GeocacheDisplay::buttonForget_Click);
			// 
			// label54
			// 
			this->label54->AutoSize = true;
			this->label54->Location = System::Drawing::Point(123, 44);
			this->label54->Name = L"label54";
			this->label54->Size = System::Drawing::Size(16, 13);
			this->label54->TabIndex = 63;
			this->label54->Text = L"of";
			// 
			// labelGCdPagesRecvd
			// 
			this->labelGCdPagesRecvd->AutoSize = true;
			this->labelGCdPagesRecvd->Location = System::Drawing::Point(108, 44);
			this->labelGCdPagesRecvd->Name = L"labelGCdPagesRecvd";
			this->labelGCdPagesRecvd->Size = System::Drawing::Size(16, 13);
			this->labelGCdPagesRecvd->TabIndex = 62;
			this->labelGCdPagesRecvd->Text = L"---";
			// 
			// labelGCdTotNumPages
			// 
			this->labelGCdTotNumPages->AutoSize = true;
			this->labelGCdTotNumPages->Location = System::Drawing::Point(141, 44);
			this->labelGCdTotNumPages->Name = L"labelGCdTotNumPages";
			this->labelGCdTotNumPages->Size = System::Drawing::Size(16, 13);
			this->labelGCdTotNumPages->TabIndex = 61;
			this->labelGCdTotNumPages->Text = L"---";
			// 
			// label71
			// 
			this->label71->AutoSize = true;
			this->label71->Location = System::Drawing::Point(16, 44);
			this->label71->Name = L"label71";
			this->label71->Size = System::Drawing::Size(86, 13);
			this->label71->TabIndex = 60;
			this->label71->Text = L"Pages (Recv\'d) :";
			// 
			// labelGCdHint
			// 
			this->labelGCdHint->AutoSize = true;
			this->labelGCdHint->Location = System::Drawing::Point(49, 104);
			this->labelGCdHint->Name = L"labelGCdHint";
			this->labelGCdHint->Size = System::Drawing::Size(16, 13);
			this->labelGCdHint->TabIndex = 30;
			this->labelGCdHint->Text = L"---";
			// 
			// label70
			// 
			this->label70->AutoSize = true;
			this->label70->Location = System::Drawing::Point(16, 104);
			this->label70->Name = L"label70";
			this->label70->Size = System::Drawing::Size(35, 13);
			this->label70->TabIndex = 29;
			this->label70->Text = L"Hint : ";
			// 
			// labelGCdPIN
			// 
			this->labelGCdPIN->AutoSize = true;
			this->labelGCdPIN->Location = System::Drawing::Point(256, 104);
			this->labelGCdPIN->Name = L"labelGCdPIN";
			this->labelGCdPIN->Size = System::Drawing::Size(16, 13);
			this->labelGCdPIN->TabIndex = 28;
			this->labelGCdPIN->Text = L"---";
			// 
			// label72
			// 
			this->label72->AutoSize = true;
			this->label72->Location = System::Drawing::Point(223, 104);
			this->label72->Name = L"label72";
			this->label72->Size = System::Drawing::Size(34, 13);
			this->label72->TabIndex = 27;
			this->label72->Text = L"PIN : ";
			// 
			// labelGCdID
			// 
			this->labelGCdID->AutoSize = true;
			this->labelGCdID->Location = System::Drawing::Point(49, 16);
			this->labelGCdID->Name = L"labelGCdID";
			this->labelGCdID->Size = System::Drawing::Size(16, 13);
			this->labelGCdID->TabIndex = 25;
			this->labelGCdID->Text = L"---";
			// 
			// label74
			// 
			this->label74->AutoSize = true;
			this->label74->Location = System::Drawing::Point(16, 16);
			this->label74->Name = L"label74";
			this->label74->Size = System::Drawing::Size(27, 13);
			this->label74->TabIndex = 26;
			this->label74->Text = L"ID : ";
			// 
			// labelGCdNumVisits
			// 
			this->labelGCdNumVisits->AutoSize = true;
			this->labelGCdNumVisits->Location = System::Drawing::Point(243, 44);
			this->labelGCdNumVisits->Name = L"labelGCdNumVisits";
			this->labelGCdNumVisits->Size = System::Drawing::Size(16, 13);
			this->labelGCdNumVisits->TabIndex = 24;
			this->labelGCdNumVisits->Text = L"---";
			// 
			// label76
			// 
			this->label76->AutoSize = true;
			this->label76->Location = System::Drawing::Point(182, 44);
			this->label76->Name = L"label76";
			this->label76->Size = System::Drawing::Size(62, 13);
			this->label76->TabIndex = 18;
			this->label76->Text = L"# of Visits : ";
			// 
			// labelGCdLon
			// 
			this->labelGCdLon->AutoSize = true;
			this->labelGCdLon->Location = System::Drawing::Point(47, 79);
			this->labelGCdLon->Name = L"labelGCdLon";
			this->labelGCdLon->Size = System::Drawing::Size(16, 13);
			this->labelGCdLon->TabIndex = 23;
			this->labelGCdLon->Text = L"---";
			// 
			// label78
			// 
			this->label78->AutoSize = true;
			this->label78->Location = System::Drawing::Point(16, 79);
			this->label78->Name = L"label78";
			this->label78->Size = System::Drawing::Size(31, 13);
			this->label78->TabIndex = 22;
			this->label78->Text = L"Lon :";
			// 
			// labelGCdLat
			// 
			this->labelGCdLat->AutoSize = true;
			this->labelGCdLat->Location = System::Drawing::Point(47, 66);
			this->labelGCdLat->Name = L"labelGCdLat";
			this->labelGCdLat->Size = System::Drawing::Size(16, 13);
			this->labelGCdLat->TabIndex = 21;
			this->labelGCdLat->Text = L"---";
			// 
			// label80
			// 
			this->label80->AutoSize = true;
			this->label80->Location = System::Drawing::Point(16, 66);
			this->label80->Name = L"label80";
			this->label80->Size = System::Drawing::Size(28, 13);
			this->label80->TabIndex = 20;
			this->label80->Text = L"Lat :";
			// 
			// labelGCdTimestamp
			// 
			this->labelGCdTimestamp->AutoSize = true;
			this->labelGCdTimestamp->Location = System::Drawing::Point(243, 62);
			this->labelGCdTimestamp->Name = L"labelGCdTimestamp";
			this->labelGCdTimestamp->Size = System::Drawing::Size(16, 13);
			this->labelGCdTimestamp->TabIndex = 19;
			this->labelGCdTimestamp->Text = L"---";
			// 
			// label82
			// 
			this->label82->AutoSize = true;
			this->label82->Location = System::Drawing::Point(182, 62);
			this->label82->Name = L"label82";
			this->label82->Size = System::Drawing::Size(58, 13);
			this->label82->TabIndex = 17;
			this->label82->Text = L"Last Visit : ";
			// 
			// tabPage2
			// 
			this->tabPage2->Controls->Add(this->label61);
			this->tabPage2->Controls->Add(this->label60);
			this->tabPage2->Controls->Add(this->labelAuthNonce);
			this->tabPage2->Controls->Add(this->label59);
			this->tabPage2->Controls->Add(this->labelAuthSerialNum);
			this->tabPage2->Controls->Add(this->label57);
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
			// label61
			// 
			this->label61->AutoSize = true;
			this->label61->Location = System::Drawing::Point(36, 78);
			this->label61->Name = L"label61";
			this->label61->Size = System::Drawing::Size(59, 13);
			this->label61->TabIndex = 13;
			this->label61->Text = L"Received :";
			// 
			// label60
			// 
			this->label60->AutoSize = true;
			this->label60->Location = System::Drawing::Point(36, 27);
			this->label60->Name = L"label60";
			this->label60->Size = System::Drawing::Size(35, 13);
			this->label60->TabIndex = 12;
			this->label60->Text = L"Sent :";
			// 
			// labelAuthNonce
			// 
			this->labelAuthNonce->AutoSize = true;
			this->labelAuthNonce->Location = System::Drawing::Point(171, 43);
			this->labelAuthNonce->Name = L"labelAuthNonce";
			this->labelAuthNonce->Size = System::Drawing::Size(16, 13);
			this->labelAuthNonce->TabIndex = 11;
			this->labelAuthNonce->Text = L"---";
			// 
			// label59
			// 
			this->label59->AutoSize = true;
			this->label59->Location = System::Drawing::Point(116, 43);
			this->label59->Name = L"label59";
			this->label59->Size = System::Drawing::Size(45, 13);
			this->label59->TabIndex = 10;
			this->label59->Text = L"Nonce :";
			// 
			// labelAuthSerialNum
			// 
			this->labelAuthSerialNum->AutoSize = true;
			this->labelAuthSerialNum->Location = System::Drawing::Point(171, 27);
			this->labelAuthSerialNum->Name = L"labelAuthSerialNum";
			this->labelAuthSerialNum->Size = System::Drawing::Size(16, 13);
			this->labelAuthSerialNum->TabIndex = 9;
			this->labelAuthSerialNum->Text = L"---";
			// 
			// label57
			// 
			this->label57->AutoSize = true;
			this->label57->Location = System::Drawing::Point(116, 27);
			this->label57->Name = L"label57";
			this->label57->Size = System::Drawing::Size(49, 13);
			this->label57->TabIndex = 8;
			this->label57->Text = L"Serial # :";
			// 
			// labelAuthToken
			// 
			this->labelAuthToken->AutoSize = true;
			this->labelAuthToken->Location = System::Drawing::Point(234, 78);
			this->labelAuthToken->Name = L"labelAuthToken";
			this->labelAuthToken->Size = System::Drawing::Size(16, 13);
			this->labelAuthToken->TabIndex = 7;
			this->labelAuthToken->Text = L"---";
			// 
			// label36
			// 
			this->label36->AutoSize = true;
			this->label36->Location = System::Drawing::Point(116, 78);
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
			// labelBattStatus
			// 
			this->labelBattStatus->AutoSize = true;
			this->labelBattStatus->Location = System::Drawing::Point(282, 65);
			this->labelBattStatus->Name = L"labelBattStatus";
			this->labelBattStatus->Size = System::Drawing::Size(16, 13);
			this->labelBattStatus->TabIndex = 80;
			this->labelBattStatus->Text = L"---";
			// 
			// labelBattVolt
			// 
			this->labelBattVolt->AutoSize = true;
			this->labelBattVolt->Location = System::Drawing::Point(282, 39);
			this->labelBattVolt->Name = L"labelBattVolt";
			this->labelBattVolt->Size = System::Drawing::Size(16, 13);
			this->labelBattVolt->TabIndex = 79;
			this->labelBattVolt->Text = L"---";
			// 
			// labelTimeRes
			// 
			this->labelTimeRes->AutoSize = true;
			this->labelTimeRes->Location = System::Drawing::Point(126, 39);
			this->labelTimeRes->Name = L"labelTimeRes";
			this->labelTimeRes->Size = System::Drawing::Size(16, 13);
			this->labelTimeRes->TabIndex = 78;
			this->labelTimeRes->Text = L"---";
			// 
			// labelOpTime
			// 
			this->labelOpTime->AutoSize = true;
			this->labelOpTime->Location = System::Drawing::Point(126, 65);
			this->labelOpTime->Name = L"labelOpTime";
			this->labelOpTime->Size = System::Drawing::Size(16, 13);
			this->labelOpTime->TabIndex = 77;
			this->labelOpTime->Text = L"---";
			// 
			// label62
			// 
			this->label62->AutoSize = true;
			this->label62->Location = System::Drawing::Point(34, 39);
			this->label62->Name = L"label62";
			this->label62->Size = System::Drawing::Size(86, 13);
			this->label62->TabIndex = 76;
			this->label62->Text = L"Time Resolution:";
			// 
			// label63
			// 
			this->label63->AutoSize = true;
			this->label63->Location = System::Drawing::Point(200, 65);
			this->label63->Name = L"label63";
			this->label63->Size = System::Drawing::Size(76, 13);
			this->label63->TabIndex = 75;
			this->label63->Text = L"Battery Status:";
			// 
			// label67
			// 
			this->label67->AutoSize = true;
			this->label67->Location = System::Drawing::Point(194, 39);
			this->label67->Name = L"label67";
			this->label67->Size = System::Drawing::Size(82, 13);
			this->label67->TabIndex = 71;
			this->label67->Text = L"Battery Voltage:";
			// 
			// label68
			// 
			this->label68->AutoSize = true;
			this->label68->Location = System::Drawing::Point(38, 65);
			this->label68->Name = L"label68";
			this->label68->Size = System::Drawing::Size(82, 13);
			this->label68->TabIndex = 70;
			this->label68->Text = L"Operating Time:";
			// 
			// label_Glb_SoftwareVer
			// 
			this->label_Glb_SoftwareVer->AutoSize = true;
			this->label_Glb_SoftwareVer->Location = System::Drawing::Point(174, 52);
			this->label_Glb_SoftwareVer->Name = L"label_Glb_SoftwareVer";
			this->label_Glb_SoftwareVer->Size = System::Drawing::Size(63, 13);
			this->label_Glb_SoftwareVer->TabIndex = 63;
			this->label_Glb_SoftwareVer->Text = L"Sw Version:";
			// 
			// label_Glb_HardwareVer
			// 
			this->label_Glb_HardwareVer->AutoSize = true;
			this->label_Glb_HardwareVer->Location = System::Drawing::Point(33, 83);
			this->label_Glb_HardwareVer->Name = L"label_Glb_HardwareVer";
			this->label_Glb_HardwareVer->Size = System::Drawing::Size(64, 13);
			this->label_Glb_HardwareVer->TabIndex = 62;
			this->label_Glb_HardwareVer->Text = L"Hw Version:";
			// 
			// label_Glb_SoftwareVerDisplay
			// 
			this->label_Glb_SoftwareVerDisplay->AutoSize = true;
			this->label_Glb_SoftwareVerDisplay->Location = System::Drawing::Point(243, 52);
			this->label_Glb_SoftwareVerDisplay->Name = L"label_Glb_SoftwareVerDisplay";
			this->label_Glb_SoftwareVerDisplay->Size = System::Drawing::Size(16, 13);
			this->label_Glb_SoftwareVerDisplay->TabIndex = 69;
			this->label_Glb_SoftwareVerDisplay->Text = L"---";
			// 
			// label_Glb_SerialNumDisplay
			// 
			this->label_Glb_SerialNumDisplay->AutoSize = true;
			this->label_Glb_SerialNumDisplay->Location = System::Drawing::Point(243, 26);
			this->label_Glb_SerialNumDisplay->Name = L"label_Glb_SerialNumDisplay";
			this->label_Glb_SerialNumDisplay->Size = System::Drawing::Size(16, 13);
			this->label_Glb_SerialNumDisplay->TabIndex = 67;
			this->label_Glb_SerialNumDisplay->Text = L"---";
			// 
			// label_Glb_ModelNum
			// 
			this->label_Glb_ModelNum->AutoSize = true;
			this->label_Glb_ModelNum->Location = System::Drawing::Point(48, 52);
			this->label_Glb_ModelNum->Name = L"label_Glb_ModelNum";
			this->label_Glb_ModelNum->Size = System::Drawing::Size(49, 13);
			this->label_Glb_ModelNum->TabIndex = 64;
			this->label_Glb_ModelNum->Text = L"Model #:";
			// 
			// label_Glb_ManfIDDisplay
			// 
			this->label_Glb_ManfIDDisplay->Location = System::Drawing::Point(103, 26);
			this->label_Glb_ManfIDDisplay->Name = L"label_Glb_ManfIDDisplay";
			this->label_Glb_ManfIDDisplay->Size = System::Drawing::Size(27, 13);
			this->label_Glb_ManfIDDisplay->TabIndex = 65;
			this->label_Glb_ManfIDDisplay->Text = L"---";
			// 
			// label_Glb_SerialNum
			// 
			this->label_Glb_SerialNum->AutoSize = true;
			this->label_Glb_SerialNum->Location = System::Drawing::Point(191, 26);
			this->label_Glb_SerialNum->Name = L"label_Glb_SerialNum";
			this->label_Glb_SerialNum->Size = System::Drawing::Size(46, 13);
			this->label_Glb_SerialNum->TabIndex = 61;
			this->label_Glb_SerialNum->Text = L"Serial #:";
			// 
			// label_Glb_ModelNumDisplay
			// 
			this->label_Glb_ModelNumDisplay->Location = System::Drawing::Point(103, 52);
			this->label_Glb_ModelNumDisplay->Name = L"label_Glb_ModelNumDisplay";
			this->label_Glb_ModelNumDisplay->Size = System::Drawing::Size(42, 13);
			this->label_Glb_ModelNumDisplay->TabIndex = 66;
			this->label_Glb_ModelNumDisplay->Text = L"---";
			// 
			// label_Glb_HardwareVerDisplay
			// 
			this->label_Glb_HardwareVerDisplay->Location = System::Drawing::Point(103, 83);
			this->label_Glb_HardwareVerDisplay->Name = L"label_Glb_HardwareVerDisplay";
			this->label_Glb_HardwareVerDisplay->Size = System::Drawing::Size(27, 13);
			this->label_Glb_HardwareVerDisplay->TabIndex = 68;
			this->label_Glb_HardwareVerDisplay->Text = L"---";
			// 
			// label_Glb_ManfID
			// 
			this->label_Glb_ManfID->AutoSize = true;
			this->label_Glb_ManfID->Location = System::Drawing::Point(46, 26);
			this->label_Glb_ManfID->Name = L"label_Glb_ManfID";
			this->label_Glb_ManfID->Size = System::Drawing::Size(51, 13);
			this->label_Glb_ManfID->TabIndex = 60;
			this->label_Glb_ManfID->Text = L"Manf. ID:";
			// 
			// tabPage3
			// 
			this->tabPage3->Controls->Add(this->tabControl3);
			this->tabPage3->Location = System::Drawing::Point(4, 22);
			this->tabPage3->Name = L"tabPage3";
			this->tabPage3->Size = System::Drawing::Size(384, 134);
			this->tabPage3->TabIndex = 2;
			this->tabPage3->Text = L"Programming";
			this->tabPage3->UseVisualStyleBackColor = true;
			// 
			// tabControl3
			// 
			this->tabControl3->Controls->Add(this->tabPage10);
			this->tabControl3->Controls->Add(this->tabPage23);
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
			this->tabPage10->Controls->Add(this->labelCurrProgPage);
			this->tabPage10->Controls->Add(this->label56);
			this->tabPage10->Controls->Add(this->label20);
			this->tabPage10->Controls->Add(this->labelProgTotalPages);
			this->tabPage10->Controls->Add(this->label19);
			this->tabPage10->Controls->Add(this->buttonErase);
			this->tabPage10->Controls->Add(this->labelProgStatus);
			this->tabPage10->Controls->Add(this->buttonProgram);
			this->tabPage10->Location = System::Drawing::Point(4, 22);
			this->tabPage10->Name = L"tabPage10";
			this->tabPage10->Padding = System::Windows::Forms::Padding(3);
			this->tabPage10->Size = System::Drawing::Size(370, 105);
			this->tabPage10->TabIndex = 4;
			this->tabPage10->Text = L"Program";
			this->tabPage10->UseVisualStyleBackColor = true;
			// 
			// labelCurrProgPage
			// 
			this->labelCurrProgPage->AutoSize = true;
			this->labelCurrProgPage->Location = System::Drawing::Point(197, 76);
			this->labelCurrProgPage->Name = L"labelCurrProgPage";
			this->labelCurrProgPage->Size = System::Drawing::Size(16, 13);
			this->labelCurrProgPage->TabIndex = 67;
			this->labelCurrProgPage->Text = L"---";
			// 
			// label56
			// 
			this->label56->AutoSize = true;
			this->label56->Location = System::Drawing::Point(106, 76);
			this->label56->Name = L"label56";
			this->label56->Size = System::Drawing::Size(82, 13);
			this->label56->TabIndex = 66;
			this->label56->Text = L"Pages (Prog\'d) :";
			// 
			// label20
			// 
			this->label20->AutoSize = true;
			this->label20->Location = System::Drawing::Point(215, 76);
			this->label20->Name = L"label20";
			this->label20->Size = System::Drawing::Size(16, 13);
			this->label20->TabIndex = 65;
			this->label20->Text = L"of";
			// 
			// labelProgTotalPages
			// 
			this->labelProgTotalPages->AutoSize = true;
			this->labelProgTotalPages->Location = System::Drawing::Point(232, 76);
			this->labelProgTotalPages->Name = L"labelProgTotalPages";
			this->labelProgTotalPages->Size = System::Drawing::Size(16, 13);
			this->labelProgTotalPages->TabIndex = 64;
			this->labelProgTotalPages->Text = L"---";
			// 
			// label19
			// 
			this->label19->AutoSize = true;
			this->label19->Location = System::Drawing::Point(113, 12);
			this->label19->Name = L"label19";
			this->label19->Size = System::Drawing::Size(43, 13);
			this->label19->TabIndex = 9;
			this->label19->Text = L"Status :";
			// 
			// buttonErase
			// 
			this->buttonErase->Enabled = false;
			this->buttonErase->Location = System::Drawing::Point(200, 37);
			this->buttonErase->Name = L"buttonErase";
			this->buttonErase->Size = System::Drawing::Size(102, 23);
			this->buttonErase->TabIndex = 2;
			this->buttonErase->Text = L"Erase Geocache";
			this->buttonErase->UseVisualStyleBackColor = true;
			this->buttonErase->Click += gcnew System::EventHandler(this, &GeocacheDisplay::buttonErase_Click);
			// 
			// labelProgStatus
			// 
			this->labelProgStatus->AutoSize = true;
			this->labelProgStatus->Location = System::Drawing::Point(160, 12);
			this->labelProgStatus->Name = L"labelProgStatus";
			this->labelProgStatus->Size = System::Drawing::Size(88, 13);
			this->labelProgStatus->TabIndex = 8;
			this->labelProgStatus->Text = L"---------------------------";
			// 
			// buttonProgram
			// 
			this->buttonProgram->Enabled = false;
			this->buttonProgram->Location = System::Drawing::Point(46, 37);
			this->buttonProgram->Name = L"buttonProgram";
			this->buttonProgram->Size = System::Drawing::Size(112, 23);
			this->buttonProgram->TabIndex = 0;
			this->buttonProgram->Text = L"Program Geocache";
			this->buttonProgram->UseVisualStyleBackColor = true;
			this->buttonProgram->Click += gcnew System::EventHandler(this, &GeocacheDisplay::buttonProgram_Click);
			// 
			// tabPage23
			// 
			this->tabPage23->Controls->Add(this->groupBox2);
			this->tabPage23->Controls->Add(this->groupBox1);
			this->tabPage23->Location = System::Drawing::Point(4, 22);
			this->tabPage23->Name = L"tabPage23";
			this->tabPage23->Size = System::Drawing::Size(370, 105);
			this->tabPage23->TabIndex = 5;
			this->tabPage23->Text = L"Main Data";
			this->tabPage23->UseVisualStyleBackColor = true;
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->checkBoxPIN);
			this->groupBox2->Controls->Add(this->textBoxPIN);
			this->groupBox2->Controls->Add(this->label2);
			this->groupBox2->Controls->Add(this->label22);
			this->groupBox2->Location = System::Drawing::Point(197, 3);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(160, 82);
			this->groupBox2->TabIndex = 60;
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
			this->checkBoxPIN->CheckedChanged += gcnew System::EventHandler(this, &GeocacheDisplay::checkBoxPIN_CheckedChanged);
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
			this->textBoxPIN->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &GeocacheDisplay::textBoxPIN_KeyDown);
			this->textBoxPIN->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &GeocacheDisplay::textBoxPIN_KeyPress);
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
			this->label22->Size = System::Drawing::Size(0, 13);
			this->label22->TabIndex = 58;
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->checkBoxID);
			this->groupBox1->Controls->Add(this->textBoxID);
			this->groupBox1->Controls->Add(this->label1);
			this->groupBox1->Location = System::Drawing::Point(17, 6);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(160, 79);
			this->groupBox1->TabIndex = 59;
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
			this->checkBoxID->CheckedChanged += gcnew System::EventHandler(this, &GeocacheDisplay::checkBoxID_CheckedChanged);
			// 
			// textBoxID
			// 
			this->textBoxID->Enabled = false;
			this->textBoxID->Location = System::Drawing::Point(64, 43);
			this->textBoxID->MaxLength = 9;
			this->textBoxID->Name = L"textBoxID";
			this->textBoxID->Size = System::Drawing::Size(76, 20);
			this->textBoxID->TabIndex = 54;
			this->textBoxID->Text = L"ABCD12345";
			this->textBoxID->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &GeocacheDisplay::textBoxID_KeyDown);
			this->textBoxID->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &GeocacheDisplay::textBoxID_KeyPress);
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
			this->checkBoxLatitude->CheckedChanged += gcnew System::EventHandler(this, &GeocacheDisplay::checkBoxLatitude_CheckedChanged);
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
			this->numericUpDownLatitude->ValueChanged += gcnew System::EventHandler(this, &GeocacheDisplay::numericUpDownLatitude_ValueChanged);
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
			this->checkBoxLongitude->CheckedChanged += gcnew System::EventHandler(this, &GeocacheDisplay::checkBoxLongitude_CheckedChanged);
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
			this->numericUpDownLongitude->ValueChanged += gcnew System::EventHandler(this, &GeocacheDisplay::numericUpDownLongitude_ValueChanged);
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
			this->checkBoxHint->CheckedChanged += gcnew System::EventHandler(this, &GeocacheDisplay::checkBoxHint_CheckedChanged);
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
			this->numericUpDownMinutes->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {30, 0, 0, 0});
			// 
			// numericUpDownNumVisits
			// 
			this->numericUpDownNumVisits->Enabled = false;
			this->numericUpDownNumVisits->Location = System::Drawing::Point(74, 41);
			this->numericUpDownNumVisits->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
			this->numericUpDownNumVisits->Name = L"numericUpDownNumVisits";
			this->numericUpDownNumVisits->Size = System::Drawing::Size(45, 20);
			this->numericUpDownNumVisits->TabIndex = 31;
			this->numericUpDownNumVisits->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {9, 0, 0, 0});
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
			this->checkBoxLoggedVisits->CheckedChanged += gcnew System::EventHandler(this, &GeocacheDisplay::checkBoxLoggedVisits_CheckedChanged);
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
			// tabPage7
			// 
			this->tabPage7->Controls->Add(this->checkBox1);
			this->tabPage7->Controls->Add(this->groupBox7);
			this->tabPage7->Controls->Add(this->groupBox8);
			this->tabPage7->Location = System::Drawing::Point(4, 22);
			this->tabPage7->Name = L"tabPage7";
			this->tabPage7->Padding = System::Windows::Forms::Padding(3);
			this->tabPage7->Size = System::Drawing::Size(384, 134);
			this->tabPage7->TabIndex = 0;
			this->tabPage7->Text = L"Main Data";
			this->tabPage7->UseVisualStyleBackColor = true;
			// 
			// checkBox1
			// 
			this->checkBox1->AutoSize = true;
			this->checkBox1->Checked = true;
			this->checkBox1->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBox1->Location = System::Drawing::Point(38, 101);
			this->checkBox1->Name = L"checkBox1";
			this->checkBox1->Size = System::Drawing::Size(116, 17);
			this->checkBox1->TabIndex = 55;
			this->checkBox1->Text = L"4Hz Page Rotation";
			this->checkBox1->UseVisualStyleBackColor = true;
			// 
			// groupBox7
			// 
			this->groupBox7->Controls->Add(this->checkBox2);
			this->groupBox7->Controls->Add(this->label6);
			this->groupBox7->Controls->Add(this->textBox1);
			this->groupBox7->Controls->Add(this->label18);
			this->groupBox7->Controls->Add(this->label23);
			this->groupBox7->Location = System::Drawing::Point(196, 6);
			this->groupBox7->Name = L"groupBox7";
			this->groupBox7->Size = System::Drawing::Size(160, 98);
			this->groupBox7->TabIndex = 57;
			this->groupBox7->TabStop = false;
			this->groupBox7->Text = L"Page 1 Data";
			// 
			// checkBox2
			// 
			this->checkBox2->AutoSize = true;
			this->checkBox2->Location = System::Drawing::Point(17, 19);
			this->checkBox2->Name = L"checkBox2";
			this->checkBox2->Size = System::Drawing::Size(69, 17);
			this->checkBox2->TabIndex = 55;
			this->checkBox2->Text = L"Set PIN :";
			this->checkBox2->UseVisualStyleBackColor = true;
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(132, 78);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(16, 13);
			this->label6->TabIndex = 59;
			this->label6->Text = L"---";
			// 
			// textBox1
			// 
			this->textBox1->Cursor = System::Windows::Forms::Cursors::IBeam;
			this->textBox1->Enabled = false;
			this->textBox1->Location = System::Drawing::Point(64, 43);
			this->textBox1->MaxLength = 10;
			this->textBox1->Name = L"textBox1";
			this->textBox1->Size = System::Drawing::Size(76, 20);
			this->textBox1->TabIndex = 54;
			this->textBox1->Text = L"12345";
			// 
			// label18
			// 
			this->label18->AutoSize = true;
			this->label18->Location = System::Drawing::Point(16, 46);
			this->label18->Name = L"label18";
			this->label18->Size = System::Drawing::Size(31, 13);
			this->label18->TabIndex = 4;
			this->label18->Text = L"PIN :";
			// 
			// label23
			// 
			this->label23->AutoSize = true;
			this->label23->Location = System::Drawing::Point(14, 78);
			this->label23->Name = L"label23";
			this->label23->Size = System::Drawing::Size(92, 13);
			this->label23->TabIndex = 58;
			this->label23->Text = L"Total # of Pages :";
			// 
			// groupBox8
			// 
			this->groupBox8->Controls->Add(this->checkBox3);
			this->groupBox8->Controls->Add(this->textBox2);
			this->groupBox8->Controls->Add(this->label24);
			this->groupBox8->Location = System::Drawing::Point(19, 6);
			this->groupBox8->Name = L"groupBox8";
			this->groupBox8->Size = System::Drawing::Size(160, 79);
			this->groupBox8->TabIndex = 56;
			this->groupBox8->TabStop = false;
			this->groupBox8->Text = L"Page 0 Data";
			// 
			// checkBox3
			// 
			this->checkBox3->AutoSize = true;
			this->checkBox3->Location = System::Drawing::Point(19, 19);
			this->checkBox3->Name = L"checkBox3";
			this->checkBox3->Size = System::Drawing::Size(62, 17);
			this->checkBox3->TabIndex = 21;
			this->checkBox3->Text = L"Set ID :";
			this->checkBox3->UseVisualStyleBackColor = true;
			// 
			// textBox2
			// 
			this->textBox2->Enabled = false;
			this->textBox2->Location = System::Drawing::Point(64, 43);
			this->textBox2->MaxLength = 9;
			this->textBox2->Name = L"textBox2";
			this->textBox2->Size = System::Drawing::Size(76, 20);
			this->textBox2->TabIndex = 54;
			this->textBox2->Text = L"ABCD12345";
			// 
			// label24
			// 
			this->label24->AutoSize = true;
			this->label24->Location = System::Drawing::Point(16, 46);
			this->label24->Name = L"label24";
			this->label24->Size = System::Drawing::Size(24, 13);
			this->label24->TabIndex = 4;
			this->label24->Text = L"ID :";
			// 
			// tabPage8
			// 
			this->tabPage8->Controls->Add(this->tabControl5);
			this->tabPage8->Location = System::Drawing::Point(4, 22);
			this->tabPage8->Name = L"tabPage8";
			this->tabPage8->Size = System::Drawing::Size(384, 134);
			this->tabPage8->TabIndex = 2;
			this->tabPage8->Text = L"Programmable Page(s)";
			this->tabPage8->UseVisualStyleBackColor = true;
			// 
			// tabControl5
			// 
			this->tabControl5->Controls->Add(this->tabPage9);
			this->tabControl5->Controls->Add(this->tabPage15);
			this->tabControl5->Controls->Add(this->tabPage16);
			this->tabControl5->Controls->Add(this->tabPage17);
			this->tabControl5->Controls->Add(this->tabPage18);
			this->tabControl5->Location = System::Drawing::Point(3, 3);
			this->tabControl5->Name = L"tabControl5";
			this->tabControl5->SelectedIndex = 0;
			this->tabControl5->Size = System::Drawing::Size(378, 131);
			this->tabControl5->TabIndex = 2;
			// 
			// tabPage9
			// 
			this->tabPage9->Controls->Add(this->button1);
			this->tabPage9->Location = System::Drawing::Point(4, 22);
			this->tabPage9->Name = L"tabPage9";
			this->tabPage9->Padding = System::Windows::Forms::Padding(3);
			this->tabPage9->Size = System::Drawing::Size(370, 105);
			this->tabPage9->TabIndex = 4;
			this->tabPage9->Text = L"Program";
			this->tabPage9->UseVisualStyleBackColor = true;
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(126, 35);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(112, 23);
			this->button1->TabIndex = 0;
			this->button1->Text = L"Program Geocache";
			this->button1->UseVisualStyleBackColor = true;
			// 
			// tabPage15
			// 
			this->tabPage15->Controls->Add(this->groupBox9);
			this->tabPage15->Location = System::Drawing::Point(4, 22);
			this->tabPage15->Name = L"tabPage15";
			this->tabPage15->Padding = System::Windows::Forms::Padding(3);
			this->tabPage15->Size = System::Drawing::Size(370, 105);
			this->tabPage15->TabIndex = 0;
			this->tabPage15->Text = L"Lattitude";
			this->tabPage15->UseVisualStyleBackColor = true;
			// 
			// groupBox9
			// 
			this->groupBox9->Controls->Add(this->checkBox4);
			this->groupBox9->Controls->Add(this->numericUpDown1);
			this->groupBox9->Location = System::Drawing::Point(6, 6);
			this->groupBox9->Name = L"groupBox9";
			this->groupBox9->Size = System::Drawing::Size(244, 79);
			this->groupBox9->TabIndex = 2;
			this->groupBox9->TabStop = false;
			this->groupBox9->Text = L"Data ID 0";
			// 
			// checkBox4
			// 
			this->checkBox4->AutoSize = true;
			this->checkBox4->Location = System::Drawing::Point(30, 19);
			this->checkBox4->Name = L"checkBox4";
			this->checkBox4->Size = System::Drawing::Size(89, 17);
			this->checkBox4->TabIndex = 20;
			this->checkBox4->Text = L"Set Latitude :";
			this->checkBox4->UseVisualStyleBackColor = true;
			// 
			// numericUpDown1
			// 
			this->numericUpDown1->DecimalPlaces = 6;
			this->numericUpDown1->Enabled = false;
			this->numericUpDown1->Location = System::Drawing::Point(73, 42);
			this->numericUpDown1->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {90, 0, 0, 0});
			this->numericUpDown1->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {90, 0, 0, System::Int32::MinValue});
			this->numericUpDown1->Name = L"numericUpDown1";
			this->numericUpDown1->Size = System::Drawing::Size(99, 20);
			this->numericUpDown1->TabIndex = 13;
			this->numericUpDown1->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {513292, 0, 0, 262144});
			// 
			// tabPage16
			// 
			this->tabPage16->Controls->Add(this->groupBox10);
			this->tabPage16->Location = System::Drawing::Point(4, 22);
			this->tabPage16->Name = L"tabPage16";
			this->tabPage16->Padding = System::Windows::Forms::Padding(3);
			this->tabPage16->Size = System::Drawing::Size(370, 105);
			this->tabPage16->TabIndex = 1;
			this->tabPage16->Text = L"Longitude";
			this->tabPage16->UseVisualStyleBackColor = true;
			// 
			// groupBox10
			// 
			this->groupBox10->Controls->Add(this->checkBox5);
			this->groupBox10->Controls->Add(this->numericUpDown2);
			this->groupBox10->Location = System::Drawing::Point(6, 6);
			this->groupBox10->Name = L"groupBox10";
			this->groupBox10->Size = System::Drawing::Size(244, 79);
			this->groupBox10->TabIndex = 3;
			this->groupBox10->TabStop = false;
			this->groupBox10->Text = L"Data ID 1";
			// 
			// checkBox5
			// 
			this->checkBox5->AutoSize = true;
			this->checkBox5->Location = System::Drawing::Point(29, 19);
			this->checkBox5->Name = L"checkBox5";
			this->checkBox5->Size = System::Drawing::Size(98, 17);
			this->checkBox5->TabIndex = 21;
			this->checkBox5->Text = L"Set Longitude :";
			this->checkBox5->UseVisualStyleBackColor = true;
			// 
			// numericUpDown2
			// 
			this->numericUpDown2->DecimalPlaces = 6;
			this->numericUpDown2->Enabled = false;
			this->numericUpDown2->Location = System::Drawing::Point(74, 42);
			this->numericUpDown2->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {180, 0, 0, 0});
			this->numericUpDown2->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {180, 0, 0, System::Int32::MinValue});
			this->numericUpDown2->Name = L"numericUpDown2";
			this->numericUpDown2->Size = System::Drawing::Size(99, 20);
			this->numericUpDown2->TabIndex = 14;
			this->numericUpDown2->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1145493, 0, 0, -2147221504});
			// 
			// tabPage17
			// 
			this->tabPage17->Controls->Add(this->groupBox11);
			this->tabPage17->Location = System::Drawing::Point(4, 22);
			this->tabPage17->Name = L"tabPage17";
			this->tabPage17->Size = System::Drawing::Size(370, 105);
			this->tabPage17->TabIndex = 2;
			this->tabPage17->Text = L"Hint";
			this->tabPage17->UseVisualStyleBackColor = true;
			// 
			// groupBox11
			// 
			this->groupBox11->Controls->Add(this->textBox3);
			this->groupBox11->Controls->Add(this->checkBox6);
			this->groupBox11->Location = System::Drawing::Point(3, 3);
			this->groupBox11->Name = L"groupBox11";
			this->groupBox11->Size = System::Drawing::Size(364, 93);
			this->groupBox11->TabIndex = 4;
			this->groupBox11->TabStop = false;
			this->groupBox11->Text = L"Data ID 2";
			// 
			// textBox3
			// 
			this->textBox3->Enabled = false;
			this->textBox3->Location = System::Drawing::Point(6, 53);
			this->textBox3->MaxLength = 80;
			this->textBox3->Name = L"textBox3";
			this->textBox3->Size = System::Drawing::Size(352, 20);
			this->textBox3->TabIndex = 56;
			this->textBox3->Text = L"HINT HINT";
			// 
			// checkBox6
			// 
			this->checkBox6->AutoSize = true;
			this->checkBox6->Location = System::Drawing::Point(27, 19);
			this->checkBox6->Name = L"checkBox6";
			this->checkBox6->Size = System::Drawing::Size(70, 17);
			this->checkBox6->TabIndex = 57;
			this->checkBox6->Text = L"Set Hint :";
			this->checkBox6->UseVisualStyleBackColor = true;
			// 
			// tabPage18
			// 
			this->tabPage18->Controls->Add(this->groupBox12);
			this->tabPage18->Location = System::Drawing::Point(4, 22);
			this->tabPage18->Name = L"tabPage18";
			this->tabPage18->Size = System::Drawing::Size(370, 105);
			this->tabPage18->TabIndex = 3;
			this->tabPage18->Text = L"LoggedVisits";
			this->tabPage18->UseVisualStyleBackColor = true;
			// 
			// groupBox12
			// 
			this->groupBox12->Controls->Add(this->label25);
			this->groupBox12->Controls->Add(this->label26);
			this->groupBox12->Controls->Add(this->label27);
			this->groupBox12->Controls->Add(this->numericUpDown3);
			this->groupBox12->Controls->Add(this->numericUpDown4);
			this->groupBox12->Controls->Add(this->label37);
			this->groupBox12->Controls->Add(this->numericUpDown5);
			this->groupBox12->Controls->Add(this->label38);
			this->groupBox12->Controls->Add(this->numericUpDown6);
			this->groupBox12->Controls->Add(this->numericUpDown7);
			this->groupBox12->Controls->Add(this->numericUpDown8);
			this->groupBox12->Controls->Add(this->label39);
			this->groupBox12->Controls->Add(this->numericUpDown9);
			this->groupBox12->Controls->Add(this->label40);
			this->groupBox12->Controls->Add(this->label41);
			this->groupBox12->Controls->Add(this->checkBox7);
			this->groupBox12->Location = System::Drawing::Point(3, 4);
			this->groupBox12->Name = L"groupBox12";
			this->groupBox12->Size = System::Drawing::Size(364, 99);
			this->groupBox12->TabIndex = 2;
			this->groupBox12->TabStop = false;
			this->groupBox12->Text = L"Data ID 4";
			// 
			// label25
			// 
			this->label25->AutoSize = true;
			this->label25->Location = System::Drawing::Point(37, 72);
			this->label25->Name = L"label25";
			this->label25->Size = System::Drawing::Size(103, 13);
			this->label25->TabIndex = 33;
			this->label25->Text = L"Last Visit Timestamp";
			// 
			// label26
			// 
			this->label26->AutoSize = true;
			this->label26->Location = System::Drawing::Point(164, 15);
			this->label26->Name = L"label26";
			this->label26->Size = System::Drawing::Size(26, 13);
			this->label26->TabIndex = 23;
			this->label26->Text = L"Day";
			// 
			// label27
			// 
			this->label27->AutoSize = true;
			this->label27->Location = System::Drawing::Point(9, 43);
			this->label27->Name = L"label27";
			this->label27->Size = System::Drawing::Size(59, 13);
			this->label27->TabIndex = 32;
			this->label27->Text = L"# of Visits :";
			// 
			// numericUpDown3
			// 
			this->numericUpDown3->Enabled = false;
			this->numericUpDown3->Location = System::Drawing::Point(316, 39);
			this->numericUpDown3->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {59, 0, 0, 0});
			this->numericUpDown3->Name = L"numericUpDown3";
			this->numericUpDown3->Size = System::Drawing::Size(37, 20);
			this->numericUpDown3->TabIndex = 26;
			this->numericUpDown3->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {30, 0, 0, 0});
			// 
			// numericUpDown4
			// 
			this->numericUpDown4->Enabled = false;
			this->numericUpDown4->Location = System::Drawing::Point(74, 41);
			this->numericUpDown4->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
			this->numericUpDown4->Name = L"numericUpDown4";
			this->numericUpDown4->Size = System::Drawing::Size(45, 20);
			this->numericUpDown4->TabIndex = 31;
			this->numericUpDown4->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {9, 0, 0, 0});
			// 
			// label37
			// 
			this->label37->AutoSize = true;
			this->label37->Location = System::Drawing::Point(153, 41);
			this->label37->Name = L"label37";
			this->label37->Size = System::Drawing::Size(37, 13);
			this->label37->TabIndex = 22;
			this->label37->Text = L"Month";
			// 
			// numericUpDown5
			// 
			this->numericUpDown5->Enabled = false;
			this->numericUpDown5->Location = System::Drawing::Point(196, 39);
			this->numericUpDown5->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {12, 0, 0, 0});
			this->numericUpDown5->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->numericUpDown5->Name = L"numericUpDown5";
			this->numericUpDown5->Size = System::Drawing::Size(37, 20);
			this->numericUpDown5->TabIndex = 30;
			this->numericUpDown5->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {5, 0, 0, 0});
			// 
			// label38
			// 
			this->label38->AutoSize = true;
			this->label38->Location = System::Drawing::Point(161, 69);
			this->label38->Name = L"label38";
			this->label38->Size = System::Drawing::Size(29, 13);
			this->label38->TabIndex = 21;
			this->label38->Text = L"Year";
			// 
			// numericUpDown6
			// 
			this->numericUpDown6->Enabled = false;
			this->numericUpDown6->Location = System::Drawing::Point(316, 65);
			this->numericUpDown6->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {59, 0, 0, 0});
			this->numericUpDown6->Name = L"numericUpDown6";
			this->numericUpDown6->Size = System::Drawing::Size(37, 20);
			this->numericUpDown6->TabIndex = 25;
			this->numericUpDown6->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {30, 0, 0, 0});
			// 
			// numericUpDown7
			// 
			this->numericUpDown7->Enabled = false;
			this->numericUpDown7->Location = System::Drawing::Point(196, 13);
			this->numericUpDown7->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {31, 0, 0, 0});
			this->numericUpDown7->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->numericUpDown7->Name = L"numericUpDown7";
			this->numericUpDown7->Size = System::Drawing::Size(37, 20);
			this->numericUpDown7->TabIndex = 28;
			this->numericUpDown7->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {13, 0, 0, 0});
			// 
			// numericUpDown8
			// 
			this->numericUpDown8->Enabled = false;
			this->numericUpDown8->Location = System::Drawing::Point(196, 65);
			this->numericUpDown8->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {3000, 0, 0, 0});
			this->numericUpDown8->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1900, 0, 0, 0});
			this->numericUpDown8->Name = L"numericUpDown8";
			this->numericUpDown8->Size = System::Drawing::Size(55, 20);
			this->numericUpDown8->TabIndex = 29;
			this->numericUpDown8->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {2011, 0, 0, 0});
			// 
			// label39
			// 
			this->label39->AutoSize = true;
			this->label39->Location = System::Drawing::Point(275, 15);
			this->label39->Name = L"label39";
			this->label39->Size = System::Drawing::Size(35, 13);
			this->label39->TabIndex = 20;
			this->label39->Text = L"Hours";
			// 
			// numericUpDown9
			// 
			this->numericUpDown9->Enabled = false;
			this->numericUpDown9->Location = System::Drawing::Point(316, 13);
			this->numericUpDown9->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {23, 0, 0, 0});
			this->numericUpDown9->Name = L"numericUpDown9";
			this->numericUpDown9->Size = System::Drawing::Size(37, 20);
			this->numericUpDown9->TabIndex = 27;
			this->numericUpDown9->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {12, 0, 0, 0});
			// 
			// label40
			// 
			this->label40->AutoSize = true;
			this->label40->Location = System::Drawing::Point(266, 41);
			this->label40->Name = L"label40";
			this->label40->Size = System::Drawing::Size(44, 13);
			this->label40->TabIndex = 19;
			this->label40->Text = L"Minutes";
			// 
			// label41
			// 
			this->label41->AutoSize = true;
			this->label41->Location = System::Drawing::Point(261, 69);
			this->label41->Name = L"label41";
			this->label41->Size = System::Drawing::Size(49, 13);
			this->label41->TabIndex = 18;
			this->label41->Text = L"Seconds";
			// 
			// checkBox7
			// 
			this->checkBox7->AutoSize = true;
			this->checkBox7->Location = System::Drawing::Point(15, 18);
			this->checkBox7->Name = L"checkBox7";
			this->checkBox7->Size = System::Drawing::Size(114, 17);
			this->checkBox7->TabIndex = 24;
			this->checkBox7->Text = L"Set Logged Visits :";
			this->checkBox7->UseVisualStyleBackColor = true;
			// 
			// tabPage19
			// 
			this->tabPage19->Controls->Add(this->label42);
			this->tabPage19->Controls->Add(this->label43);
			this->tabPage19->Location = System::Drawing::Point(4, 22);
			this->tabPage19->Name = L"tabPage19";
			this->tabPage19->Padding = System::Windows::Forms::Padding(3);
			this->tabPage19->Size = System::Drawing::Size(384, 134);
			this->tabPage19->TabIndex = 4;
			this->tabPage19->Text = L"Authentication";
			this->tabPage19->UseVisualStyleBackColor = true;
			// 
			// label42
			// 
			this->label42->AutoSize = true;
			this->label42->Location = System::Drawing::Point(143, 31);
			this->label42->Name = L"label42";
			this->label42->Size = System::Drawing::Size(16, 13);
			this->label42->TabIndex = 7;
			this->label42->Text = L"---";
			// 
			// label43
			// 
			this->label43->AutoSize = true;
			this->label43->Location = System::Drawing::Point(25, 31);
			this->label43->Name = L"label43";
			this->label43->Size = System::Drawing::Size(115, 13);
			this->label43->TabIndex = 6;
			this->label43->Text = L"Authentication Token :";
			// 
			// tabPage20
			// 
			this->tabPage20->Controls->Add(this->tabControl6);
			this->tabPage20->Location = System::Drawing::Point(4, 22);
			this->tabPage20->Name = L"tabPage20";
			this->tabPage20->Size = System::Drawing::Size(384, 134);
			this->tabPage20->TabIndex = 3;
			this->tabPage20->Text = L"Common Data";
			this->tabPage20->UseVisualStyleBackColor = true;
			// 
			// tabControl6
			// 
			this->tabControl6->Controls->Add(this->tabPage21);
			this->tabControl6->Controls->Add(this->tabPage22);
			this->tabControl6->Location = System::Drawing::Point(0, 3);
			this->tabControl6->Name = L"tabControl6";
			this->tabControl6->SelectedIndex = 0;
			this->tabControl6->Size = System::Drawing::Size(388, 115);
			this->tabControl6->TabIndex = 0;
			// 
			// tabPage21
			// 
			this->tabPage21->Controls->Add(this->button2);
			this->tabPage21->Controls->Add(this->textBox4);
			this->tabPage21->Controls->Add(this->textBox5);
			this->tabPage21->Controls->Add(this->textBox6);
			this->tabPage21->Controls->Add(this->textBox7);
			this->tabPage21->Controls->Add(this->textBox8);
			this->tabPage21->Controls->Add(this->label44);
			this->tabPage21->Controls->Add(this->label45);
			this->tabPage21->Controls->Add(this->label46);
			this->tabPage21->Controls->Add(this->label47);
			this->tabPage21->Controls->Add(this->label48);
			this->tabPage21->Location = System::Drawing::Point(4, 22);
			this->tabPage21->Name = L"tabPage21";
			this->tabPage21->Padding = System::Windows::Forms::Padding(3);
			this->tabPage21->Size = System::Drawing::Size(380, 89);
			this->tabPage21->TabIndex = 0;
			this->tabPage21->Text = L"Required Pages";
			this->tabPage21->UseVisualStyleBackColor = true;
			// 
			// button2
			// 
			this->button2->Location = System::Drawing::Point(185, 58);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(97, 20);
			this->button2->TabIndex = 59;
			this->button2->Text = L"Update All";
			this->button2->UseVisualStyleBackColor = true;
			// 
			// textBox4
			// 
			this->textBox4->Location = System::Drawing::Point(263, 6);
			this->textBox4->MaxLength = 3;
			this->textBox4->Name = L"textBox4";
			this->textBox4->Size = System::Drawing::Size(29, 20);
			this->textBox4->TabIndex = 57;
			this->textBox4->Text = L"1";
			// 
			// textBox5
			// 
			this->textBox5->Location = System::Drawing::Point(263, 32);
			this->textBox5->MaxLength = 3;
			this->textBox5->Name = L"textBox5";
			this->textBox5->Size = System::Drawing::Size(29, 20);
			this->textBox5->TabIndex = 58;
			this->textBox5->Text = L"1";
			// 
			// textBox6
			// 
			this->textBox6->Location = System::Drawing::Point(94, 59);
			this->textBox6->MaxLength = 5;
			this->textBox6->Name = L"textBox6";
			this->textBox6->Size = System::Drawing::Size(49, 20);
			this->textBox6->TabIndex = 52;
			this->textBox6->Text = L"33669";
			// 
			// textBox7
			// 
			this->textBox7->Location = System::Drawing::Point(94, 32);
			this->textBox7->MaxLength = 5;
			this->textBox7->Name = L"textBox7";
			this->textBox7->Size = System::Drawing::Size(49, 20);
			this->textBox7->TabIndex = 51;
			this->textBox7->Text = L"2";
			// 
			// textBox8
			// 
			this->textBox8->Location = System::Drawing::Point(67, 6);
			this->textBox8->MaxLength = 10;
			this->textBox8->Name = L"textBox8";
			this->textBox8->Size = System::Drawing::Size(76, 20);
			this->textBox8->TabIndex = 53;
			this->textBox8->Text = L"1234567890";
			// 
			// label44
			// 
			this->label44->AutoSize = true;
			this->label44->Location = System::Drawing::Point(182, 9);
			this->label44->Name = L"label44";
			this->label44->Size = System::Drawing::Size(75, 13);
			this->label44->TabIndex = 54;
			this->label44->Text = L"Hardware Ver:";
			// 
			// label45
			// 
			this->label45->AutoSize = true;
			this->label45->Location = System::Drawing::Point(182, 35);
			this->label45->Name = L"label45";
			this->label45->Size = System::Drawing::Size(71, 13);
			this->label45->TabIndex = 55;
			this->label45->Text = L"Software Ver:";
			// 
			// label46
			// 
			this->label46->AutoSize = true;
			this->label46->Location = System::Drawing::Point(6, 35);
			this->label46->Name = L"label46";
			this->label46->Size = System::Drawing::Size(51, 13);
			this->label46->TabIndex = 48;
			this->label46->Text = L"Manf. ID:";
			// 
			// label47
			// 
			this->label47->AutoSize = true;
			this->label47->Location = System::Drawing::Point(6, 62);
			this->label47->Name = L"label47";
			this->label47->Size = System::Drawing::Size(49, 13);
			this->label47->TabIndex = 50;
			this->label47->Text = L"Model #:";
			// 
			// label48
			// 
			this->label48->AutoSize = true;
			this->label48->Location = System::Drawing::Point(6, 9);
			this->label48->Name = L"label48";
			this->label48->Size = System::Drawing::Size(46, 13);
			this->label48->TabIndex = 49;
			this->label48->Text = L"Serial #:";
			// 
			// tabPage22
			// 
			this->tabPage22->Controls->Add(this->label49);
			this->tabPage22->Controls->Add(this->label50);
			this->tabPage22->Controls->Add(this->comboBox1);
			this->tabPage22->Controls->Add(this->button3);
			this->tabPage22->Controls->Add(this->numericUpDown10);
			this->tabPage22->Controls->Add(this->label51);
			this->tabPage22->Controls->Add(this->numericUpDown11);
			this->tabPage22->Controls->Add(this->groupBox13);
			this->tabPage22->Controls->Add(this->checkBox8);
			this->tabPage22->Controls->Add(this->textBox9);
			this->tabPage22->Controls->Add(this->label52);
			this->tabPage22->Controls->Add(this->label53);
			this->tabPage22->Controls->Add(this->checkBox9);
			this->tabPage22->Location = System::Drawing::Point(4, 22);
			this->tabPage22->Name = L"tabPage22";
			this->tabPage22->Padding = System::Windows::Forms::Padding(3);
			this->tabPage22->Size = System::Drawing::Size(380, 89);
			this->tabPage22->TabIndex = 1;
			this->tabPage22->Text = L"Battery Page";
			this->tabPage22->UseVisualStyleBackColor = true;
			// 
			// label49
			// 
			this->label49->AutoSize = true;
			this->label49->Location = System::Drawing::Point(6, 24);
			this->label49->Name = L"label49";
			this->label49->Size = System::Drawing::Size(73, 13);
			this->label49->TabIndex = 79;
			this->label49->Text = L"Battery Status";
			// 
			// label50
			// 
			this->label50->AutoSize = true;
			this->label50->Location = System::Drawing::Point(13, 62);
			this->label50->Name = L"label50";
			this->label50->Size = System::Drawing::Size(143, 13);
			this->label50->TabIndex = 78;
			this->label50->Text = L"Fractional Voltage (1/256 V):";
			// 
			// comboBox1
			// 
			this->comboBox1->FormattingEnabled = true;
			this->comboBox1->Items->AddRange(gcnew cli::array< System::Object^  >(6) {L"New", L"Good", L"Ok", L"Low", L"Critical", L"Invalid"});
			this->comboBox1->Location = System::Drawing::Point(85, 21);
			this->comboBox1->Name = L"comboBox1";
			this->comboBox1->Size = System::Drawing::Size(60, 21);
			this->comboBox1->TabIndex = 76;
			this->comboBox1->Text = L"Ok";
			// 
			// button3
			// 
			this->button3->Location = System::Drawing::Point(226, 58);
			this->button3->Name = L"button3";
			this->button3->Size = System::Drawing::Size(84, 20);
			this->button3->TabIndex = 73;
			this->button3->Text = L"Update Time";
			this->button3->UseVisualStyleBackColor = true;
			// 
			// numericUpDown10
			// 
			this->numericUpDown10->Location = System::Drawing::Point(165, 41);
			this->numericUpDown10->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {14, 0, 0, 0});
			this->numericUpDown10->Name = L"numericUpDown10";
			this->numericUpDown10->Size = System::Drawing::Size(36, 20);
			this->numericUpDown10->TabIndex = 68;
			this->numericUpDown10->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {3, 0, 0, 0});
			// 
			// label51
			// 
			this->label51->AutoSize = true;
			this->label51->Location = System::Drawing::Point(186, 7);
			this->label51->Name = L"label51";
			this->label51->Size = System::Drawing::Size(0, 13);
			this->label51->TabIndex = 75;
			// 
			// numericUpDown11
			// 
			this->numericUpDown11->Location = System::Drawing::Point(165, 60);
			this->numericUpDown11->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
			this->numericUpDown11->Name = L"numericUpDown11";
			this->numericUpDown11->Size = System::Drawing::Size(42, 20);
			this->numericUpDown11->TabIndex = 69;
			this->numericUpDown11->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {85, 0, 0, 0});
			// 
			// groupBox13
			// 
			this->groupBox13->Controls->Add(this->radioButton1);
			this->groupBox13->Controls->Add(this->radioButton2);
			this->groupBox13->Enabled = false;
			this->groupBox13->Location = System::Drawing::Point(253, 17);
			this->groupBox13->Name = L"groupBox13";
			this->groupBox13->Size = System::Drawing::Size(124, 36);
			this->groupBox13->TabIndex = 63;
			this->groupBox13->TabStop = false;
			this->groupBox13->Text = L"Time Resolution";
			// 
			// radioButton1
			// 
			this->radioButton1->AutoSize = true;
			this->radioButton1->Checked = true;
			this->radioButton1->Location = System::Drawing::Point(6, 15);
			this->radioButton1->Name = L"radioButton1";
			this->radioButton1->Size = System::Drawing::Size(51, 17);
			this->radioButton1->TabIndex = 58;
			this->radioButton1->TabStop = true;
			this->radioButton1->Text = L"2 sec";
			this->radioButton1->UseVisualStyleBackColor = true;
			// 
			// radioButton2
			// 
			this->radioButton2->AutoSize = true;
			this->radioButton2->Location = System::Drawing::Point(63, 15);
			this->radioButton2->Name = L"radioButton2";
			this->radioButton2->Size = System::Drawing::Size(57, 17);
			this->radioButton2->TabIndex = 59;
			this->radioButton2->Text = L"16 sec";
			this->radioButton2->UseVisualStyleBackColor = true;
			// 
			// checkBox8
			// 
			this->checkBox8->AutoSize = true;
			this->checkBox8->Checked = true;
			this->checkBox8->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBox8->Location = System::Drawing::Point(6, 42);
			this->checkBox8->Name = L"checkBox8";
			this->checkBox8->Size = System::Drawing::Size(153, 17);
			this->checkBox8->TabIndex = 70;
			this->checkBox8->Text = L"Enable Battery Voltage (V):";
			this->checkBox8->UseVisualStyleBackColor = true;
			// 
			// textBox9
			// 
			this->textBox9->Location = System::Drawing::Point(316, 58);
			this->textBox9->MaxLength = 9;
			this->textBox9->Name = L"textBox9";
			this->textBox9->Size = System::Drawing::Size(63, 20);
			this->textBox9->TabIndex = 72;
			this->textBox9->Text = L"0";
			this->textBox9->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			// 
			// label52
			// 
			this->label52->Enabled = false;
			this->label52->Location = System::Drawing::Point(317, 2);
			this->label52->Name = L"label52";
			this->label52->Size = System::Drawing::Size(62, 13);
			this->label52->TabIndex = 65;
			this->label52->Text = L"0";
			this->label52->TextAlign = System::Drawing::ContentAlignment::BottomRight;
			// 
			// label53
			// 
			this->label53->AutoSize = true;
			this->label53->Location = System::Drawing::Point(223, 1);
			this->label53->Name = L"label53";
			this->label53->Size = System::Drawing::Size(88, 13);
			this->label53->TabIndex = 64;
			this->label53->Text = L"Elapsed Time (s):";
			// 
			// checkBox9
			// 
			this->checkBox9->AutoSize = true;
			this->checkBox9->Checked = true;
			this->checkBox9->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBox9->Location = System::Drawing::Point(0, 0);
			this->checkBox9->Name = L"checkBox9";
			this->checkBox9->Size = System::Drawing::Size(156, 17);
			this->checkBox9->TabIndex = 67;
			this->checkBox9->Text = L"Enable Battery Status Page";
			this->checkBox9->UseVisualStyleBackColor = true;
			// 
			// tabControl2
			// 
			this->tabControl2->Controls->Add(this->tabPage5);
			this->tabControl2->Controls->Add(this->tabPage6);
			this->tabControl2->Location = System::Drawing::Point(0, 0);
			this->tabControl2->Name = L"tabControl2";
			this->tabControl2->SelectedIndex = 0;
			this->tabControl2->Size = System::Drawing::Size(384, 134);
			this->tabControl2->TabIndex = 0;
			// 
			// tabPage5
			// 
			this->tabPage5->Controls->Add(this->label_Glb_ModelNumDisplay);
			this->tabPage5->Controls->Add(this->label_Glb_HardwareVer);
			this->tabPage5->Controls->Add(this->label_Glb_ManfID);
			this->tabPage5->Controls->Add(this->label_Glb_SoftwareVer);
			this->tabPage5->Controls->Add(this->label_Glb_HardwareVerDisplay);
			this->tabPage5->Controls->Add(this->label_Glb_SoftwareVerDisplay);
			this->tabPage5->Controls->Add(this->label_Glb_SerialNumDisplay);
			this->tabPage5->Controls->Add(this->label_Glb_SerialNum);
			this->tabPage5->Controls->Add(this->label_Glb_ModelNum);
			this->tabPage5->Controls->Add(this->label_Glb_ManfIDDisplay);
			this->tabPage5->Location = System::Drawing::Point(4, 22);
			this->tabPage5->Name = L"tabPage5";
			this->tabPage5->Padding = System::Windows::Forms::Padding(3);
			this->tabPage5->Size = System::Drawing::Size(376, 108);
			this->tabPage5->TabIndex = 0;
			this->tabPage5->Text = L"Manufacturer\'s Info";
			this->tabPage5->UseVisualStyleBackColor = true;
			// 
			// tabPage6
			// 
			this->tabPage6->Controls->Add(this->labelBattStatus);
			this->tabPage6->Controls->Add(this->label68);
			this->tabPage6->Controls->Add(this->label67);
			this->tabPage6->Controls->Add(this->labelBattVolt);
			this->tabPage6->Controls->Add(this->label63);
			this->tabPage6->Controls->Add(this->label62);
			this->tabPage6->Controls->Add(this->labelTimeRes);
			this->tabPage6->Controls->Add(this->labelOpTime);
			this->tabPage6->Location = System::Drawing::Point(4, 22);
			this->tabPage6->Name = L"tabPage6";
			this->tabPage6->Padding = System::Windows::Forms::Padding(3);
			this->tabPage6->Size = System::Drawing::Size(376, 108);
			this->tabPage6->TabIndex = 1;
			this->tabPage6->Text = L"Battery Status";
			this->tabPage6->UseVisualStyleBackColor = true;
			// 
			// GeocacheDisplay
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(794, 351);
			this->Controls->Add(this->panel_Display);
			this->Controls->Add(this->panel_Settings);
			this->Name = L"GeocacheDisplay";
			this->Text = L"Geocache Display";
			this->panel_Settings->ResumeLayout(false);
			this->tabControl1->ResumeLayout(false);
			this->tabPage1->ResumeLayout(false);
			this->tabPage1->PerformLayout();
			this->tabPage2->ResumeLayout(false);
			this->tabPage2->PerformLayout();
			this->tabPage4->ResumeLayout(false);
			this->tabPage3->ResumeLayout(false);
			this->tabControl3->ResumeLayout(false);
			this->tabPage10->ResumeLayout(false);
			this->tabPage10->PerformLayout();
			this->tabPage23->ResumeLayout(false);
			this->groupBox2->ResumeLayout(false);
			this->groupBox2->PerformLayout();
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
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
			this->panel_Display->ResumeLayout(false);
			this->panel_Display->PerformLayout();
			this->tabPage7->ResumeLayout(false);
			this->tabPage7->PerformLayout();
			this->groupBox7->ResumeLayout(false);
			this->groupBox7->PerformLayout();
			this->groupBox8->ResumeLayout(false);
			this->groupBox8->PerformLayout();
			this->tabPage8->ResumeLayout(false);
			this->tabControl5->ResumeLayout(false);
			this->tabPage9->ResumeLayout(false);
			this->tabPage15->ResumeLayout(false);
			this->groupBox9->ResumeLayout(false);
			this->groupBox9->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown1))->EndInit();
			this->tabPage16->ResumeLayout(false);
			this->groupBox10->ResumeLayout(false);
			this->groupBox10->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown2))->EndInit();
			this->tabPage17->ResumeLayout(false);
			this->groupBox11->ResumeLayout(false);
			this->groupBox11->PerformLayout();
			this->tabPage18->ResumeLayout(false);
			this->groupBox12->ResumeLayout(false);
			this->groupBox12->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown3))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown4))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown5))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown6))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown7))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown8))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown9))->EndInit();
			this->tabPage19->ResumeLayout(false);
			this->tabPage19->PerformLayout();
			this->tabPage20->ResumeLayout(false);
			this->tabControl6->ResumeLayout(false);
			this->tabPage21->ResumeLayout(false);
			this->tabPage21->PerformLayout();
			this->tabPage22->ResumeLayout(false);
			this->tabPage22->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown10))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown11))->EndInit();
			this->groupBox13->ResumeLayout(false);
			this->groupBox13->PerformLayout();
			this->tabControl2->ResumeLayout(false);
			this->tabPage5->ResumeLayout(false);
			this->tabPage5->PerformLayout();
			this->tabPage6->ResumeLayout(false);
			this->tabPage6->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion

};