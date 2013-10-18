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
#include "antplus_control.h"	// Device specific class, if used
#include "antplus_common.h"				// Include common pages (example usage on SDM and Bike Power)
#include "types.h"
#include "antdefines.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

public ref class AudioDevice : public System::Windows::Forms::Form, public ISimBase{
public:
	// The constructor may include the following parameters:
	// System::Timers::Timer^ channelTimer:  Timer handle, if using a timer (not required in most cases, since getTimerInterval gets called after onTimerTock on each tock)
	// dRequestAckMsg^ channelAckMsg:  Handle to delegate function to send acknowledged messages (only needed if the device needs to send acknowledged messages)
	// These parameters may or may not be included in the constructor, as needed
		AudioDevice(System::Timers::Timer^ channelTimer){
			InitializeComponent();	
            AudioData = gcnew AntPlusControls();
            CommonPages = gcnew CommonData();
			InitializeSim();
		}

		~AudioDevice(){
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
	virtual void onTimerTock(USHORT eventTime);							// Called every simulator event.  Empty function for most receivers (timer is disabled on receivers unless explicitly enabled)
	virtual void ANT_eventNotification(UCHAR ucEventCode_, UCHAR* pucEventBuffer_);	 // Channel event state machine
    virtual UCHAR getDeviceType(){return AntPlusControls::DEVICE_TYPE;} // Controls Device Type
    virtual UCHAR getTransmissionType(){return AntPlusControls::TX_TYPE;} // Controls Tx Type
    virtual USHORT getTransmitPeriod(){return AntPlusControls::MSG_PERIOD;} // Controls Message Period
	virtual DOUBLE getTimerInterval(){return 1000;}	// Set to any value, timer not used
	virtual System::Windows::Forms::Panel^ getSimSettingsPanel(){return this->panel_Settings;}
	virtual System::Windows::Forms::Panel^ getSimTranslatedDisplay(){return this->panel_Display;}

// Optional methods (not required by interface class)
private:
	void InitializeSim();						// Initialize simulation
   void HandleTransmit(UCHAR* pucTxBuffer_);
   void UpdateUI();
   void HandleReceive(UCHAR* pucRxBuffer_);
   void ReceiveAudioCommand();
   void UpdateCommonPattern();
   void UpdateBatStatus();
   void UpdateAudioTime();

   System::Void cbAudioState_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
   System::Void cbRepeatState_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
   System::Void cbShuffleState_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
   System::Void cbCustomRepeat_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
   System::Void cbCustomShuffle_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
   System::Void nmVolume_ValueChanged(System::Object^  sender, System::EventArgs^  e);
   System::Void cbInvalidVolume_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
   System::Void nmTotalTime_ValueChanged(System::Object^  sender, System::EventArgs^  e);
   System::Void nmCurrentTime_ValueChanged(System::Object^  sender, System::EventArgs^  e);
   System::Void cbInvalidTotal_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
   System::Void cbInvalidCurrent_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
   System::Void button_UpdateCommon_Click(System::Object^  sender, System::EventArgs^  e);
   System::Void checkBox_BatStatus_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
   System::Void comboBoxBatStatus_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
   System::Void checkBox_Bat_Voltage_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
   System::Void numericUpDown_Bat_VoltInt_ValueChanged(System::Object^  sender, System::EventArgs^  e);
   System::Void numericUpDown_Bat_VoltFrac_ValueChanged(System::Object^  sender, System::EventArgs^  e);
   System::Void radioButton_Bat_Elp2Units_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
   System::Void button_Bat_ElpTimeUpdate_Click(System::Object^  sender, System::EventArgs^  e);
   System::Void button_UpdateMemLevel_Click(System::Object^  sender, System::EventArgs^  e);
   System::Void checkBox_MemoryLevel_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
   System::Void checkBox_PairedDevices_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
   System::Void button_UpdatePeripheral_Click(System::Object^  sender, System::EventArgs^  e);
   System::Void checkBox_NoSerial_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
   System::Void checkBoxSongTitle_CheckedChanged(System::Object^  sender, System::EventArgs^  e);

private:
   AntPlusControls^ AudioData;
   CommonData^ CommonPages;

   ULONG ulTotalTime;

   BOOL bFastForward;
   BOOL bRewind;

   static System::Collections::Generic::List<UCHAR> aucCommonPattern = gcnew System::Collections::Generic::List<UCHAR>;


private: System::Windows::Forms::Panel^  panel_Settings;
private: System::Windows::Forms::Panel^  panel_Display;
private: System::Windows::Forms::TabControl^  tabControl1;
private: System::Windows::Forms::TabPage^  tabPage1;
private: System::Windows::Forms::Label^  label7;
private: System::Windows::Forms::Label^  label6;
private: System::Windows::Forms::CheckBox^  cbInvalidCurrent;
private: System::Windows::Forms::CheckBox^  cbInvalidTotal;
private: System::Windows::Forms::GroupBox^  groupBox1;
private: System::Windows::Forms::CheckBox^  cbInvalidVolume;
private: System::Windows::Forms::NumericUpDown^  nmVolume;
private: System::Windows::Forms::TabPage^  tabPage2;
private: System::Windows::Forms::TabControl^  tabControl2;
private: System::Windows::Forms::TabPage^  tabPage5;
private: System::Windows::Forms::Label^  label_ErrorCommon;
private: System::Windows::Forms::CheckBox^  checkBox_NoSerial;
private: System::Windows::Forms::Button^  button_UpdateCommon;
private: System::Windows::Forms::TextBox^  textBox_HardwareVer;
private: System::Windows::Forms::Label^  label_Glb_HardwareVer;
private: System::Windows::Forms::Label^  label_Glb_SoftwareVer;
private: System::Windows::Forms::TextBox^  textBox_SoftwareVer;
private: System::Windows::Forms::TextBox^  textBox_ManfID;
private: System::Windows::Forms::Label^  label_Glb_ManfID;
private: System::Windows::Forms::TextBox^  textBox_ModelNumber;
private: System::Windows::Forms::Label^  label_Glb_ModelNum;
private: System::Windows::Forms::TextBox^  textBox_SerialNumber;
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
private: System::Windows::Forms::CheckBox^  checkBox_BatStatus;
private: System::Windows::Forms::TabPage^  tabPage7;
private: System::Windows::Forms::Button^  button_UpdateMemLevel;
private: System::Windows::Forms::RadioButton^  rbByteUnit;
private: System::Windows::Forms::RadioButton^  rbBitUnit;
private: System::Windows::Forms::ComboBox^  cbSizeUnit;
private: System::Windows::Forms::Label^  label12;
private: System::Windows::Forms::Label^  label11;
private: System::Windows::Forms::NumericUpDown^  nmSizeExponent;
private: System::Windows::Forms::NumericUpDown^  nmPercentUsed;
private: System::Windows::Forms::Label^  label10;
private: System::Windows::Forms::CheckBox^  checkBox_MemoryLevel;
private: System::Windows::Forms::TabPage^  tabPage3;
private: System::Windows::Forms::Label^  label_PairedError;
private: System::Windows::Forms::TextBox^  textBox_TotalDevices;
private: System::Windows::Forms::Label^  label21;
private: System::Windows::Forms::Label^  label18;
private: System::Windows::Forms::TextBox^  textBox_DeviceIndex;
private: System::Windows::Forms::TextBox^  textBox_DeviceType;
private: System::Windows::Forms::TextBox^  textBox_TxType;
private: System::Windows::Forms::TextBox^  textBox_DeviceNumber;
private: System::Windows::Forms::Label^  label17;
private: System::Windows::Forms::Label^  label16;
private: System::Windows::Forms::Label^  label15;
private: System::Windows::Forms::Label^  label14;
private: System::Windows::Forms::Label^  label13;
private: System::Windows::Forms::ComboBox^  cbNetworkKey;
private: System::Windows::Forms::CheckBox^  cbPaired;
private: System::Windows::Forms::ComboBox^  cbConnectionState;
private: System::Windows::Forms::Button^  button_UpdatePeripheral;
private: System::Windows::Forms::CheckBox^  checkBox_PairedDevices;
private: System::Windows::Forms::Label^  label2;
private: System::Windows::Forms::Label^  label1;
private: System::Windows::Forms::NumericUpDown^  nmCurrentTime;
private: System::Windows::Forms::NumericUpDown^  nmTotalTime;
private: System::Windows::Forms::ComboBox^  cbAudioState;
private: System::Windows::Forms::ComboBox^  cbShuffleState;
private: System::Windows::Forms::ComboBox^  cbRepeatState;
private: System::Windows::Forms::Label^  label5;
private: System::Windows::Forms::Label^  label3;
private: System::Windows::Forms::Label^  label8;
private: System::Windows::Forms::CheckBox^  cbCustomRepeat;
private: System::Windows::Forms::CheckBox^  cbCustomShuffle;
private: System::Windows::Forms::TabControl^  tabControl3;
private: System::Windows::Forms::TabPage^  tabPage4;
private: System::Windows::Forms::TabPage^  tabPage8;
private: System::Windows::Forms::Label^  lbCommand;
private: System::Windows::Forms::Label^  label4;
private: System::Windows::Forms::TextBox^  textBoxSongTitle;
private: System::Windows::Forms::CheckBox^  checkBoxSongTitle;


private: System::Windows::Forms::Label^  label9;


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
         this->tabControl3 = (gcnew System::Windows::Forms::TabControl());
         this->tabPage4 = (gcnew System::Windows::Forms::TabPage());
         this->lbCommand = (gcnew System::Windows::Forms::Label());
         this->label4 = (gcnew System::Windows::Forms::Label());
         this->label3 = (gcnew System::Windows::Forms::Label());
         this->cbCustomShuffle = (gcnew System::Windows::Forms::CheckBox());
         this->cbAudioState = (gcnew System::Windows::Forms::ComboBox());
         this->cbRepeatState = (gcnew System::Windows::Forms::ComboBox());
         this->cbShuffleState = (gcnew System::Windows::Forms::ComboBox());
         this->label5 = (gcnew System::Windows::Forms::Label());
         this->label8 = (gcnew System::Windows::Forms::Label());
         this->cbCustomRepeat = (gcnew System::Windows::Forms::CheckBox());
         this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
         this->cbInvalidVolume = (gcnew System::Windows::Forms::CheckBox());
         this->nmVolume = (gcnew System::Windows::Forms::NumericUpDown());
         this->tabPage8 = (gcnew System::Windows::Forms::TabPage());
         this->checkBoxSongTitle = (gcnew System::Windows::Forms::CheckBox());
         this->label9 = (gcnew System::Windows::Forms::Label());
         this->textBoxSongTitle = (gcnew System::Windows::Forms::TextBox());
         this->label2 = (gcnew System::Windows::Forms::Label());
         this->label1 = (gcnew System::Windows::Forms::Label());
         this->cbInvalidTotal = (gcnew System::Windows::Forms::CheckBox());
         this->nmCurrentTime = (gcnew System::Windows::Forms::NumericUpDown());
         this->cbInvalidCurrent = (gcnew System::Windows::Forms::CheckBox());
         this->label6 = (gcnew System::Windows::Forms::Label());
         this->nmTotalTime = (gcnew System::Windows::Forms::NumericUpDown());
         this->label7 = (gcnew System::Windows::Forms::Label());
         this->tabPage2 = (gcnew System::Windows::Forms::TabPage());
         this->tabControl2 = (gcnew System::Windows::Forms::TabControl());
         this->tabPage5 = (gcnew System::Windows::Forms::TabPage());
         this->label_ErrorCommon = (gcnew System::Windows::Forms::Label());
         this->checkBox_NoSerial = (gcnew System::Windows::Forms::CheckBox());
         this->button_UpdateCommon = (gcnew System::Windows::Forms::Button());
         this->textBox_HardwareVer = (gcnew System::Windows::Forms::TextBox());
         this->label_Glb_HardwareVer = (gcnew System::Windows::Forms::Label());
         this->label_Glb_SoftwareVer = (gcnew System::Windows::Forms::Label());
         this->textBox_SoftwareVer = (gcnew System::Windows::Forms::TextBox());
         this->textBox_ManfID = (gcnew System::Windows::Forms::TextBox());
         this->label_Glb_ManfID = (gcnew System::Windows::Forms::Label());
         this->textBox_ModelNumber = (gcnew System::Windows::Forms::TextBox());
         this->label_Glb_ModelNum = (gcnew System::Windows::Forms::Label());
         this->textBox_SerialNumber = (gcnew System::Windows::Forms::TextBox());
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
         this->checkBox_BatStatus = (gcnew System::Windows::Forms::CheckBox());
         this->tabPage7 = (gcnew System::Windows::Forms::TabPage());
         this->button_UpdateMemLevel = (gcnew System::Windows::Forms::Button());
         this->rbByteUnit = (gcnew System::Windows::Forms::RadioButton());
         this->rbBitUnit = (gcnew System::Windows::Forms::RadioButton());
         this->cbSizeUnit = (gcnew System::Windows::Forms::ComboBox());
         this->label12 = (gcnew System::Windows::Forms::Label());
         this->label11 = (gcnew System::Windows::Forms::Label());
         this->nmSizeExponent = (gcnew System::Windows::Forms::NumericUpDown());
         this->nmPercentUsed = (gcnew System::Windows::Forms::NumericUpDown());
         this->label10 = (gcnew System::Windows::Forms::Label());
         this->checkBox_MemoryLevel = (gcnew System::Windows::Forms::CheckBox());
         this->tabPage3 = (gcnew System::Windows::Forms::TabPage());
         this->label_PairedError = (gcnew System::Windows::Forms::Label());
         this->textBox_TotalDevices = (gcnew System::Windows::Forms::TextBox());
         this->label21 = (gcnew System::Windows::Forms::Label());
         this->label18 = (gcnew System::Windows::Forms::Label());
         this->textBox_DeviceIndex = (gcnew System::Windows::Forms::TextBox());
         this->textBox_DeviceType = (gcnew System::Windows::Forms::TextBox());
         this->textBox_TxType = (gcnew System::Windows::Forms::TextBox());
         this->textBox_DeviceNumber = (gcnew System::Windows::Forms::TextBox());
         this->label17 = (gcnew System::Windows::Forms::Label());
         this->label16 = (gcnew System::Windows::Forms::Label());
         this->label15 = (gcnew System::Windows::Forms::Label());
         this->label14 = (gcnew System::Windows::Forms::Label());
         this->label13 = (gcnew System::Windows::Forms::Label());
         this->cbNetworkKey = (gcnew System::Windows::Forms::ComboBox());
         this->cbPaired = (gcnew System::Windows::Forms::CheckBox());
         this->cbConnectionState = (gcnew System::Windows::Forms::ComboBox());
         this->button_UpdatePeripheral = (gcnew System::Windows::Forms::Button());
         this->checkBox_PairedDevices = (gcnew System::Windows::Forms::CheckBox());
         this->panel_Display = (gcnew System::Windows::Forms::Panel());
         this->panel_Settings->SuspendLayout();
         this->tabControl1->SuspendLayout();
         this->tabPage1->SuspendLayout();
         this->tabControl3->SuspendLayout();
         this->tabPage4->SuspendLayout();
         this->groupBox1->SuspendLayout();
         (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->nmVolume))->BeginInit();
         this->tabPage8->SuspendLayout();
         (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->nmCurrentTime))->BeginInit();
         (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->nmTotalTime))->BeginInit();
         this->tabPage2->SuspendLayout();
         this->tabControl2->SuspendLayout();
         this->tabPage5->SuspendLayout();
         this->tabPage6->SuspendLayout();
         (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Bat_VoltInt))->BeginInit();
         (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Bat_VoltFrac))->BeginInit();
         this->groupBox_Resol->SuspendLayout();
         this->tabPage7->SuspendLayout();
         (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->nmSizeExponent))->BeginInit();
         (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->nmPercentUsed))->BeginInit();
         this->tabPage3->SuspendLayout();
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
         this->tabControl1->Location = System::Drawing::Point(0, 0);
         this->tabControl1->Name = L"tabControl1";
         this->tabControl1->SelectedIndex = 0;
         this->tabControl1->Size = System::Drawing::Size(400, 140);
         this->tabControl1->TabIndex = 1;
         // 
         // tabPage1
         // 
         this->tabPage1->Controls->Add(this->tabControl3);
         this->tabPage1->Location = System::Drawing::Point(4, 22);
         this->tabPage1->Name = L"tabPage1";
         this->tabPage1->Padding = System::Windows::Forms::Padding(3);
         this->tabPage1->Size = System::Drawing::Size(392, 114);
         this->tabPage1->TabIndex = 0;
         this->tabPage1->Text = L"Audio";
         this->tabPage1->UseVisualStyleBackColor = true;
         // 
         // tabControl3
         // 
         this->tabControl3->Controls->Add(this->tabPage4);
         this->tabControl3->Controls->Add(this->tabPage8);
         this->tabControl3->Location = System::Drawing::Point(0, 0);
         this->tabControl3->Name = L"tabControl3";
         this->tabControl3->SelectedIndex = 0;
         this->tabControl3->Size = System::Drawing::Size(396, 115);
         this->tabControl3->TabIndex = 0;
         // 
         // tabPage4
         // 
         this->tabPage4->Controls->Add(this->lbCommand);
         this->tabPage4->Controls->Add(this->label4);
         this->tabPage4->Controls->Add(this->label3);
         this->tabPage4->Controls->Add(this->cbCustomShuffle);
         this->tabPage4->Controls->Add(this->cbAudioState);
         this->tabPage4->Controls->Add(this->cbRepeatState);
         this->tabPage4->Controls->Add(this->cbShuffleState);
         this->tabPage4->Controls->Add(this->label5);
         this->tabPage4->Controls->Add(this->label8);
         this->tabPage4->Controls->Add(this->cbCustomRepeat);
         this->tabPage4->Controls->Add(this->groupBox1);
         this->tabPage4->Location = System::Drawing::Point(4, 22);
         this->tabPage4->Name = L"tabPage4";
         this->tabPage4->Padding = System::Windows::Forms::Padding(3);
         this->tabPage4->Size = System::Drawing::Size(388, 89);
         this->tabPage4->TabIndex = 0;
         this->tabPage4->Text = L"States";
         this->tabPage4->UseVisualStyleBackColor = true;
         // 
         // lbCommand
         // 
         this->lbCommand->AutoSize = true;
         this->lbCommand->Location = System::Drawing::Point(192, 9);
         this->lbCommand->Name = L"lbCommand";
         this->lbCommand->Size = System::Drawing::Size(19, 13);
         this->lbCommand->TabIndex = 53;
         this->lbCommand->Text = L"----";
         // 
         // label4
         // 
         this->label4->AutoSize = true;
         this->label4->Location = System::Drawing::Point(131, 9);
         this->label4->Name = L"label4";
         this->label4->Size = System::Drawing::Size(55, 13);
         this->label4->TabIndex = 52;
         this->label4->Text = L"Cmd Rx\'d:";
         // 
         // label3
         // 
         this->label3->AutoSize = true;
         this->label3->Location = System::Drawing::Point(6, 9);
         this->label3->Name = L"label3";
         this->label3->Size = System::Drawing::Size(34, 13);
         this->label3->TabIndex = 46;
         this->label3->Text = L"Audio";
         // 
         // cbCustomShuffle
         // 
         this->cbCustomShuffle->AutoSize = true;
         this->cbCustomShuffle->Location = System::Drawing::Point(131, 62);
         this->cbCustomShuffle->Name = L"cbCustomShuffle";
         this->cbCustomShuffle->Size = System::Drawing::Size(149, 17);
         this->cbCustomShuffle->TabIndex = 51;
         this->cbCustomShuffle->Text = L"Custom Shuffle Supported";
         this->cbCustomShuffle->UseVisualStyleBackColor = true;
         this->cbCustomShuffle->CheckedChanged += gcnew System::EventHandler(this, &AudioDevice::cbCustomShuffle_CheckedChanged);
         // 
         // cbAudioState
         // 
         this->cbAudioState->FormattingEnabled = true;
         this->cbAudioState->Items->AddRange(gcnew cli::array< System::Object^  >(6) {L"Off", L"Play", L"Pause", L"Stop", L"Busy", 
            L"Unknown"});
         this->cbAudioState->Location = System::Drawing::Point(51, 6);
         this->cbAudioState->Name = L"cbAudioState";
         this->cbAudioState->Size = System::Drawing::Size(74, 21);
         this->cbAudioState->TabIndex = 43;
         this->cbAudioState->Text = L"Off";
         this->cbAudioState->SelectedIndexChanged += gcnew System::EventHandler(this, &AudioDevice::cbAudioState_SelectedIndexChanged);
         // 
         // cbRepeatState
         // 
         this->cbRepeatState->FormattingEnabled = true;
         this->cbRepeatState->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"Off", L"Track", L"All"});
         this->cbRepeatState->Location = System::Drawing::Point(51, 33);
         this->cbRepeatState->Name = L"cbRepeatState";
         this->cbRepeatState->Size = System::Drawing::Size(74, 21);
         this->cbRepeatState->TabIndex = 44;
         this->cbRepeatState->Text = L"Off";
         this->cbRepeatState->SelectedIndexChanged += gcnew System::EventHandler(this, &AudioDevice::cbRepeatState_SelectedIndexChanged);
         // 
         // cbShuffleState
         // 
         this->cbShuffleState->FormattingEnabled = true;
         this->cbShuffleState->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"Off", L"Songs", L"Album"});
         this->cbShuffleState->Location = System::Drawing::Point(51, 60);
         this->cbShuffleState->Name = L"cbShuffleState";
         this->cbShuffleState->Size = System::Drawing::Size(74, 21);
         this->cbShuffleState->TabIndex = 45;
         this->cbShuffleState->Text = L"Off";
         this->cbShuffleState->SelectedIndexChanged += gcnew System::EventHandler(this, &AudioDevice::cbShuffleState_SelectedIndexChanged);
         // 
         // label5
         // 
         this->label5->AutoSize = true;
         this->label5->Location = System::Drawing::Point(3, 36);
         this->label5->Name = L"label5";
         this->label5->Size = System::Drawing::Size(42, 13);
         this->label5->TabIndex = 48;
         this->label5->Text = L"Repeat";
         // 
         // label8
         // 
         this->label8->AutoSize = true;
         this->label8->Location = System::Drawing::Point(5, 63);
         this->label8->Name = L"label8";
         this->label8->Size = System::Drawing::Size(40, 13);
         this->label8->TabIndex = 49;
         this->label8->Text = L"Shuffle";
         // 
         // cbCustomRepeat
         // 
         this->cbCustomRepeat->AutoSize = true;
         this->cbCustomRepeat->Location = System::Drawing::Point(131, 35);
         this->cbCustomRepeat->Name = L"cbCustomRepeat";
         this->cbCustomRepeat->Size = System::Drawing::Size(151, 17);
         this->cbCustomRepeat->TabIndex = 50;
         this->cbCustomRepeat->Text = L"Custom Repeat Supported";
         this->cbCustomRepeat->UseVisualStyleBackColor = true;
         this->cbCustomRepeat->CheckedChanged += gcnew System::EventHandler(this, &AudioDevice::cbCustomRepeat_CheckedChanged);
         // 
         // groupBox1
         // 
         this->groupBox1->Controls->Add(this->cbInvalidVolume);
         this->groupBox1->Controls->Add(this->nmVolume);
         this->groupBox1->Location = System::Drawing::Point(295, 13);
         this->groupBox1->Name = L"groupBox1";
         this->groupBox1->Size = System::Drawing::Size(87, 57);
         this->groupBox1->TabIndex = 12;
         this->groupBox1->TabStop = false;
         this->groupBox1->Text = L"Volume(%)";
         // 
         // cbInvalidVolume
         // 
         this->cbInvalidVolume->AutoSize = true;
         this->cbInvalidVolume->Location = System::Drawing::Point(6, 37);
         this->cbInvalidVolume->Name = L"cbInvalidVolume";
         this->cbInvalidVolume->Size = System::Drawing::Size(72, 17);
         this->cbInvalidVolume->TabIndex = 6;
         this->cbInvalidVolume->Text = L"Unknown";
         this->cbInvalidVolume->UseVisualStyleBackColor = true;
         this->cbInvalidVolume->CheckedChanged += gcnew System::EventHandler(this, &AudioDevice::cbInvalidVolume_CheckedChanged);
         // 
         // nmVolume
         // 
         this->nmVolume->Location = System::Drawing::Point(6, 14);
         this->nmVolume->Name = L"nmVolume";
         this->nmVolume->Size = System::Drawing::Size(43, 20);
         this->nmVolume->TabIndex = 0;
         this->nmVolume->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {50, 0, 0, 0});
         this->nmVolume->ValueChanged += gcnew System::EventHandler(this, &AudioDevice::nmVolume_ValueChanged);
         // 
         // tabPage8
         // 
         this->tabPage8->Controls->Add(this->checkBoxSongTitle);
         this->tabPage8->Controls->Add(this->label9);
         this->tabPage8->Controls->Add(this->textBoxSongTitle);
         this->tabPage8->Controls->Add(this->label2);
         this->tabPage8->Controls->Add(this->label1);
         this->tabPage8->Controls->Add(this->cbInvalidTotal);
         this->tabPage8->Controls->Add(this->nmCurrentTime);
         this->tabPage8->Controls->Add(this->cbInvalidCurrent);
         this->tabPage8->Controls->Add(this->label6);
         this->tabPage8->Controls->Add(this->nmTotalTime);
         this->tabPage8->Controls->Add(this->label7);
         this->tabPage8->Location = System::Drawing::Point(4, 22);
         this->tabPage8->Name = L"tabPage8";
         this->tabPage8->Padding = System::Windows::Forms::Padding(3);
         this->tabPage8->Size = System::Drawing::Size(388, 89);
         this->tabPage8->TabIndex = 1;
         this->tabPage8->Text = L"Track Info";
         this->tabPage8->UseVisualStyleBackColor = true;
         // 
         // checkBoxSongTitle
         // 
         this->checkBoxSongTitle->AutoSize = true;
         this->checkBoxSongTitle->Location = System::Drawing::Point(297, 65);
         this->checkBoxSongTitle->Name = L"checkBoxSongTitle";
         this->checkBoxSongTitle->Size = System::Drawing::Size(75, 17);
         this->checkBoxSongTitle->TabIndex = 44;
         this->checkBoxSongTitle->Text = L"Supported";
         this->checkBoxSongTitle->UseVisualStyleBackColor = true;
         this->checkBoxSongTitle->CheckedChanged += gcnew System::EventHandler(this, &AudioDevice::checkBoxSongTitle_CheckedChanged);
         // 
         // label9
         // 
         this->label9->AutoSize = true;
         this->label9->Location = System::Drawing::Point(111, 66);
         this->label9->Name = L"label9";
         this->label9->Size = System::Drawing::Size(55, 13);
         this->label9->TabIndex = 43;
         this->label9->Text = L"Song Title";
         // 
         // textBoxSongTitle
         // 
         this->textBoxSongTitle->Location = System::Drawing::Point(172, 63);
         this->textBoxSongTitle->Name = L"textBoxSongTitle";
         this->textBoxSongTitle->Size = System::Drawing::Size(119, 20);
         this->textBoxSongTitle->TabIndex = 2;
         // 
         // label2
         // 
         this->label2->AutoSize = true;
         this->label2->Location = System::Drawing::Point(68, 37);
         this->label2->Name = L"label2";
         this->label2->Size = System::Drawing::Size(98, 13);
         this->label2->TabIndex = 42;
         this->label2->Text = L"Current Track Time";
         // 
         // label1
         // 
         this->label1->AutoSize = true;
         this->label1->Location = System::Drawing::Point(78, 11);
         this->label1->Name = L"label1";
         this->label1->Size = System::Drawing::Size(88, 13);
         this->label1->TabIndex = 41;
         this->label1->Text = L"Total Track Time";
         // 
         // cbInvalidTotal
         // 
         this->cbInvalidTotal->AutoSize = true;
         this->cbInvalidTotal->Location = System::Drawing::Point(250, 10);
         this->cbInvalidTotal->Name = L"cbInvalidTotal";
         this->cbInvalidTotal->Size = System::Drawing::Size(57, 17);
         this->cbInvalidTotal->TabIndex = 35;
         this->cbInvalidTotal->Text = L"Invalid";
         this->cbInvalidTotal->UseVisualStyleBackColor = true;
         this->cbInvalidTotal->CheckedChanged += gcnew System::EventHandler(this, &AudioDevice::cbInvalidTotal_CheckedChanged);
         // 
         // nmCurrentTime
         // 
         this->nmCurrentTime->Location = System::Drawing::Point(172, 35);
         this->nmCurrentTime->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {65534, 0, 0, 0});
         this->nmCurrentTime->Name = L"nmCurrentTime";
         this->nmCurrentTime->Size = System::Drawing::Size(54, 20);
         this->nmCurrentTime->TabIndex = 40;
         this->nmCurrentTime->ValueChanged += gcnew System::EventHandler(this, &AudioDevice::nmCurrentTime_ValueChanged);
         // 
         // cbInvalidCurrent
         // 
         this->cbInvalidCurrent->AutoSize = true;
         this->cbInvalidCurrent->Location = System::Drawing::Point(250, 36);
         this->cbInvalidCurrent->Name = L"cbInvalidCurrent";
         this->cbInvalidCurrent->Size = System::Drawing::Size(57, 17);
         this->cbInvalidCurrent->TabIndex = 36;
         this->cbInvalidCurrent->Text = L"Invalid";
         this->cbInvalidCurrent->UseVisualStyleBackColor = true;
         this->cbInvalidCurrent->CheckedChanged += gcnew System::EventHandler(this, &AudioDevice::cbInvalidCurrent_CheckedChanged);
         // 
         // label6
         // 
         this->label6->AutoSize = true;
         this->label6->Location = System::Drawing::Point(232, 11);
         this->label6->Name = L"label6";
         this->label6->Size = System::Drawing::Size(12, 13);
         this->label6->TabIndex = 37;
         this->label6->Text = L"s";
         // 
         // nmTotalTime
         // 
         this->nmTotalTime->Location = System::Drawing::Point(172, 9);
         this->nmTotalTime->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {65534, 0, 0, 0});
         this->nmTotalTime->Name = L"nmTotalTime";
         this->nmTotalTime->Size = System::Drawing::Size(54, 20);
         this->nmTotalTime->TabIndex = 39;
         this->nmTotalTime->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {180, 0, 0, 0});
         this->nmTotalTime->ValueChanged += gcnew System::EventHandler(this, &AudioDevice::nmTotalTime_ValueChanged);
         // 
         // label7
         // 
         this->label7->AutoSize = true;
         this->label7->Location = System::Drawing::Point(232, 37);
         this->label7->Name = L"label7";
         this->label7->Size = System::Drawing::Size(12, 13);
         this->label7->TabIndex = 38;
         this->label7->Text = L"s";
         // 
         // tabPage2
         // 
         this->tabPage2->Controls->Add(this->tabControl2);
         this->tabPage2->Location = System::Drawing::Point(4, 22);
         this->tabPage2->Name = L"tabPage2";
         this->tabPage2->Padding = System::Windows::Forms::Padding(3);
         this->tabPage2->Size = System::Drawing::Size(392, 114);
         this->tabPage2->TabIndex = 1;
         this->tabPage2->Text = L"Common Data";
         this->tabPage2->UseVisualStyleBackColor = true;
         // 
         // tabControl2
         // 
         this->tabControl2->Controls->Add(this->tabPage5);
         this->tabControl2->Controls->Add(this->tabPage6);
         this->tabControl2->Controls->Add(this->tabPage7);
         this->tabControl2->Controls->Add(this->tabPage3);
         this->tabControl2->Location = System::Drawing::Point(2, 4);
         this->tabControl2->Name = L"tabControl2";
         this->tabControl2->SelectedIndex = 0;
         this->tabControl2->Size = System::Drawing::Size(388, 107);
         this->tabControl2->TabIndex = 3;
         // 
         // tabPage5
         // 
         this->tabPage5->Controls->Add(this->label_ErrorCommon);
         this->tabPage5->Controls->Add(this->checkBox_NoSerial);
         this->tabPage5->Controls->Add(this->button_UpdateCommon);
         this->tabPage5->Controls->Add(this->textBox_HardwareVer);
         this->tabPage5->Controls->Add(this->label_Glb_HardwareVer);
         this->tabPage5->Controls->Add(this->label_Glb_SoftwareVer);
         this->tabPage5->Controls->Add(this->textBox_SoftwareVer);
         this->tabPage5->Controls->Add(this->textBox_ManfID);
         this->tabPage5->Controls->Add(this->label_Glb_ManfID);
         this->tabPage5->Controls->Add(this->textBox_ModelNumber);
         this->tabPage5->Controls->Add(this->label_Glb_ModelNum);
         this->tabPage5->Controls->Add(this->textBox_SerialNumber);
         this->tabPage5->Controls->Add(this->label_Glb_SerialNum);
         this->tabPage5->Location = System::Drawing::Point(4, 22);
         this->tabPage5->Name = L"tabPage5";
         this->tabPage5->Padding = System::Windows::Forms::Padding(3);
         this->tabPage5->Size = System::Drawing::Size(380, 81);
         this->tabPage5->TabIndex = 0;
         this->tabPage5->Text = L"Required Pages";
         this->tabPage5->UseVisualStyleBackColor = true;
         // 
         // label_ErrorCommon
         // 
         this->label_ErrorCommon->AutoSize = true;
         this->label_ErrorCommon->ForeColor = System::Drawing::Color::Red;
         this->label_ErrorCommon->Location = System::Drawing::Point(149, 62);
         this->label_ErrorCommon->Name = L"label_ErrorCommon";
         this->label_ErrorCommon->Size = System::Drawing::Size(35, 13);
         this->label_ErrorCommon->TabIndex = 61;
         this->label_ErrorCommon->Text = L"Error: ";
         this->label_ErrorCommon->Visible = false;
         // 
         // checkBox_NoSerial
         // 
         this->checkBox_NoSerial->AutoSize = true;
         this->checkBox_NoSerial->Location = System::Drawing::Point(149, 8);
         this->checkBox_NoSerial->Name = L"checkBox_NoSerial";
         this->checkBox_NoSerial->Size = System::Drawing::Size(79, 17);
         this->checkBox_NoSerial->TabIndex = 60;
         this->checkBox_NoSerial->Text = L"No Serial #";
         this->checkBox_NoSerial->UseVisualStyleBackColor = true;
         this->checkBox_NoSerial->CheckedChanged += gcnew System::EventHandler(this, &AudioDevice::checkBox_NoSerial_CheckedChanged);
         // 
         // button_UpdateCommon
         // 
         this->button_UpdateCommon->Location = System::Drawing::Point(250, 55);
         this->button_UpdateCommon->Name = L"button_UpdateCommon";
         this->button_UpdateCommon->Size = System::Drawing::Size(97, 20);
         this->button_UpdateCommon->TabIndex = 59;
         this->button_UpdateCommon->Text = L"Update All";
         this->button_UpdateCommon->UseVisualStyleBackColor = true;
         this->button_UpdateCommon->Click += gcnew System::EventHandler(this, &AudioDevice::button_UpdateCommon_Click);
         // 
         // textBox_HardwareVer
         // 
         this->textBox_HardwareVer->Location = System::Drawing::Point(318, 6);
         this->textBox_HardwareVer->MaxLength = 3;
         this->textBox_HardwareVer->Name = L"textBox_HardwareVer";
         this->textBox_HardwareVer->Size = System::Drawing::Size(29, 20);
         this->textBox_HardwareVer->TabIndex = 57;
         this->textBox_HardwareVer->Text = L"1";
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
         // textBox_SoftwareVer
         // 
         this->textBox_SoftwareVer->Location = System::Drawing::Point(318, 32);
         this->textBox_SoftwareVer->MaxLength = 3;
         this->textBox_SoftwareVer->Name = L"textBox_SoftwareVer";
         this->textBox_SoftwareVer->Size = System::Drawing::Size(29, 20);
         this->textBox_SoftwareVer->TabIndex = 58;
         this->textBox_SoftwareVer->Text = L"1";
         // 
         // textBox_ManfID
         // 
         this->textBox_ManfID->Location = System::Drawing::Point(94, 32);
         this->textBox_ManfID->MaxLength = 5;
         this->textBox_ManfID->Name = L"textBox_ManfID";
         this->textBox_ManfID->Size = System::Drawing::Size(49, 20);
         this->textBox_ManfID->TabIndex = 52;
         this->textBox_ManfID->Text = L"15";
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
         // textBox_ModelNumber
         // 
         this->textBox_ModelNumber->Location = System::Drawing::Point(94, 59);
         this->textBox_ModelNumber->MaxLength = 5;
         this->textBox_ModelNumber->Name = L"textBox_ModelNumber";
         this->textBox_ModelNumber->Size = System::Drawing::Size(49, 20);
         this->textBox_ModelNumber->TabIndex = 51;
         this->textBox_ModelNumber->Text = L"2";
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
         // textBox_SerialNumber
         // 
         this->textBox_SerialNumber->Location = System::Drawing::Point(67, 6);
         this->textBox_SerialNumber->MaxLength = 10;
         this->textBox_SerialNumber->Name = L"textBox_SerialNumber";
         this->textBox_SerialNumber->Size = System::Drawing::Size(76, 20);
         this->textBox_SerialNumber->TabIndex = 53;
         this->textBox_SerialNumber->Text = L"1234567890";
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
         this->tabPage6->Controls->Add(this->checkBox_BatStatus);
         this->tabPage6->Location = System::Drawing::Point(4, 22);
         this->tabPage6->Name = L"tabPage6";
         this->tabPage6->Padding = System::Windows::Forms::Padding(3);
         this->tabPage6->Size = System::Drawing::Size(380, 81);
         this->tabPage6->TabIndex = 1;
         this->tabPage6->Text = L"Battery Status";
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
         this->comboBoxBatStatus->SelectedIndexChanged += gcnew System::EventHandler(this, &AudioDevice::comboBoxBatStatus_SelectedIndexChanged);
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
         this->button_Bat_ElpTimeUpdate->Click += gcnew System::EventHandler(this, &AudioDevice::button_Bat_ElpTimeUpdate_Click);
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
         this->numericUpDown_Bat_VoltInt->ValueChanged += gcnew System::EventHandler(this, &AudioDevice::numericUpDown_Bat_VoltInt_ValueChanged);
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
         this->numericUpDown_Bat_VoltFrac->ValueChanged += gcnew System::EventHandler(this, &AudioDevice::numericUpDown_Bat_VoltFrac_ValueChanged);
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
         this->radioButton_Bat_Elp2Units->CheckedChanged += gcnew System::EventHandler(this, &AudioDevice::radioButton_Bat_Elp2Units_CheckedChanged);
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
         this->checkBox_Bat_Voltage->CheckedChanged += gcnew System::EventHandler(this, &AudioDevice::checkBox_Bat_Voltage_CheckedChanged);
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
         // checkBox_BatStatus
         // 
         this->checkBox_BatStatus->AutoSize = true;
         this->checkBox_BatStatus->Location = System::Drawing::Point(0, 0);
         this->checkBox_BatStatus->Name = L"checkBox_BatStatus";
         this->checkBox_BatStatus->Size = System::Drawing::Size(156, 17);
         this->checkBox_BatStatus->TabIndex = 67;
         this->checkBox_BatStatus->Text = L"Enable Battery Status Page";
         this->checkBox_BatStatus->UseVisualStyleBackColor = true;
         this->checkBox_BatStatus->CheckedChanged += gcnew System::EventHandler(this, &AudioDevice::checkBox_BatStatus_CheckedChanged);
         // 
         // tabPage7
         // 
         this->tabPage7->Controls->Add(this->button_UpdateMemLevel);
         this->tabPage7->Controls->Add(this->rbByteUnit);
         this->tabPage7->Controls->Add(this->rbBitUnit);
         this->tabPage7->Controls->Add(this->cbSizeUnit);
         this->tabPage7->Controls->Add(this->label12);
         this->tabPage7->Controls->Add(this->label11);
         this->tabPage7->Controls->Add(this->nmSizeExponent);
         this->tabPage7->Controls->Add(this->nmPercentUsed);
         this->tabPage7->Controls->Add(this->label10);
         this->tabPage7->Controls->Add(this->checkBox_MemoryLevel);
         this->tabPage7->Location = System::Drawing::Point(4, 22);
         this->tabPage7->Name = L"tabPage7";
         this->tabPage7->Size = System::Drawing::Size(380, 81);
         this->tabPage7->TabIndex = 2;
         this->tabPage7->Text = L"Memory Level";
         this->tabPage7->UseVisualStyleBackColor = true;
         // 
         // button_UpdateMemLevel
         // 
         this->button_UpdateMemLevel->Enabled = false;
         this->button_UpdateMemLevel->Location = System::Drawing::Point(256, 54);
         this->button_UpdateMemLevel->Name = L"button_UpdateMemLevel";
         this->button_UpdateMemLevel->Size = System::Drawing::Size(71, 23);
         this->button_UpdateMemLevel->TabIndex = 18;
         this->button_UpdateMemLevel->Text = L"Update All";
         this->button_UpdateMemLevel->UseVisualStyleBackColor = true;
         this->button_UpdateMemLevel->Click += gcnew System::EventHandler(this, &AudioDevice::button_UpdateMemLevel_Click);
         // 
         // rbByteUnit
         // 
         this->rbByteUnit->AutoSize = true;
         this->rbByteUnit->Checked = true;
         this->rbByteUnit->Enabled = false;
         this->rbByteUnit->Location = System::Drawing::Point(163, 55);
         this->rbByteUnit->Name = L"rbByteUnit";
         this->rbByteUnit->Size = System::Drawing::Size(45, 17);
         this->rbByteUnit->TabIndex = 17;
         this->rbByteUnit->TabStop = true;
         this->rbByteUnit->Text = L"byte";
         this->rbByteUnit->UseVisualStyleBackColor = true;
         // 
         // rbBitUnit
         // 
         this->rbBitUnit->AutoSize = true;
         this->rbBitUnit->Enabled = false;
         this->rbBitUnit->Location = System::Drawing::Point(116, 55);
         this->rbBitUnit->Name = L"rbBitUnit";
         this->rbBitUnit->Size = System::Drawing::Size(36, 17);
         this->rbBitUnit->TabIndex = 16;
         this->rbBitUnit->Text = L"bit";
         this->rbBitUnit->UseVisualStyleBackColor = true;
         // 
         // cbSizeUnit
         // 
         this->cbSizeUnit->Enabled = false;
         this->cbSizeUnit->FormattingEnabled = true;
         this->cbSizeUnit->Items->AddRange(gcnew cli::array< System::Object^  >(4) {L"Base", L"Kilo", L"Mega", L"Tera"});
         this->cbSizeUnit->Location = System::Drawing::Point(58, 54);
         this->cbSizeUnit->Name = L"cbSizeUnit";
         this->cbSizeUnit->Size = System::Drawing::Size(52, 21);
         this->cbSizeUnit->TabIndex = 15;
         this->cbSizeUnit->Text = L"Mega";
         // 
         // label12
         // 
         this->label12->AutoSize = true;
         this->label12->Location = System::Drawing::Point(3, 57);
         this->label12->Name = L"label12";
         this->label12->Size = System::Drawing::Size(49, 13);
         this->label12->TabIndex = 14;
         this->label12->Text = L"Size Unit";
         // 
         // label11
         // 
         this->label11->AutoSize = true;
         this->label11->Location = System::Drawing::Point(148, 31);
         this->label11->Name = L"label11";
         this->label11->Size = System::Drawing::Size(102, 13);
         this->label11->TabIndex = 13;
         this->label11->Text = L"Total Size Exponent";
         // 
         // nmSizeExponent
         // 
         this->nmSizeExponent->Enabled = false;
         this->nmSizeExponent->Location = System::Drawing::Point(256, 29);
         this->nmSizeExponent->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
         this->nmSizeExponent->Name = L"nmSizeExponent";
         this->nmSizeExponent->Size = System::Drawing::Size(71, 20);
         this->nmSizeExponent->TabIndex = 12;
         this->nmSizeExponent->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {10, 0, 0, 0});
         // 
         // nmPercentUsed
         // 
         this->nmPercentUsed->Enabled = false;
         this->nmPercentUsed->Location = System::Drawing::Point(256, 3);
         this->nmPercentUsed->Name = L"nmPercentUsed";
         this->nmPercentUsed->Size = System::Drawing::Size(71, 20);
         this->nmPercentUsed->TabIndex = 11;
         this->nmPercentUsed->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {30, 0, 0, 0});
         // 
         // label10
         // 
         this->label10->AutoSize = true;
         this->label10->Location = System::Drawing::Point(207, 5);
         this->label10->Name = L"label10";
         this->label10->Size = System::Drawing::Size(43, 13);
         this->label10->TabIndex = 10;
         this->label10->Text = L"% Used";
         // 
         // checkBox_MemoryLevel
         // 
         this->checkBox_MemoryLevel->AutoSize = true;
         this->checkBox_MemoryLevel->Location = System::Drawing::Point(0, 0);
         this->checkBox_MemoryLevel->Name = L"checkBox_MemoryLevel";
         this->checkBox_MemoryLevel->Size = System::Drawing::Size(156, 17);
         this->checkBox_MemoryLevel->TabIndex = 9;
         this->checkBox_MemoryLevel->Text = L"Enable Memory Level Page";
         this->checkBox_MemoryLevel->UseVisualStyleBackColor = true;
         this->checkBox_MemoryLevel->CheckedChanged += gcnew System::EventHandler(this, &AudioDevice::checkBox_MemoryLevel_CheckedChanged);
         // 
         // tabPage3
         // 
         this->tabPage3->Controls->Add(this->label_PairedError);
         this->tabPage3->Controls->Add(this->textBox_TotalDevices);
         this->tabPage3->Controls->Add(this->label21);
         this->tabPage3->Controls->Add(this->label18);
         this->tabPage3->Controls->Add(this->textBox_DeviceIndex);
         this->tabPage3->Controls->Add(this->textBox_DeviceType);
         this->tabPage3->Controls->Add(this->textBox_TxType);
         this->tabPage3->Controls->Add(this->textBox_DeviceNumber);
         this->tabPage3->Controls->Add(this->label17);
         this->tabPage3->Controls->Add(this->label16);
         this->tabPage3->Controls->Add(this->label15);
         this->tabPage3->Controls->Add(this->label14);
         this->tabPage3->Controls->Add(this->label13);
         this->tabPage3->Controls->Add(this->cbNetworkKey);
         this->tabPage3->Controls->Add(this->cbPaired);
         this->tabPage3->Controls->Add(this->cbConnectionState);
         this->tabPage3->Controls->Add(this->button_UpdatePeripheral);
         this->tabPage3->Controls->Add(this->checkBox_PairedDevices);
         this->tabPage3->Location = System::Drawing::Point(4, 22);
         this->tabPage3->Name = L"tabPage3";
         this->tabPage3->Size = System::Drawing::Size(380, 81);
         this->tabPage3->TabIndex = 3;
         this->tabPage3->Text = L"Peripheral Devices";
         this->tabPage3->UseVisualStyleBackColor = true;
         // 
         // label_PairedError
         // 
         this->label_PairedError->ForeColor = System::Drawing::Color::Red;
         this->label_PairedError->Location = System::Drawing::Point(129, 64);
         this->label_PairedError->Name = L"label_PairedError";
         this->label_PairedError->Size = System::Drawing::Size(102, 26);
         this->label_PairedError->TabIndex = 23;
         this->label_PairedError->Text = L"Error: Invalid Input";
         this->label_PairedError->Visible = false;
         // 
         // textBox_TotalDevices
         // 
         this->textBox_TotalDevices->Enabled = false;
         this->textBox_TotalDevices->Location = System::Drawing::Point(171, 39);
         this->textBox_TotalDevices->Name = L"textBox_TotalDevices";
         this->textBox_TotalDevices->Size = System::Drawing::Size(27, 20);
         this->textBox_TotalDevices->TabIndex = 27;
         this->textBox_TotalDevices->Text = L"1";
         // 
         // label21
         // 
         this->label21->Location = System::Drawing::Point(123, 38);
         this->label21->Name = L"label21";
         this->label21->Size = System::Drawing::Size(54, 33);
         this->label21->TabIndex = 26;
         this->label21->Text = L"# of Devices";
         // 
         // label18
         // 
         this->label18->AutoSize = true;
         this->label18->Location = System::Drawing::Point(123, 19);
         this->label18->Name = L"label18";
         this->label18->Size = System::Drawing::Size(33, 13);
         this->label18->TabIndex = 25;
         this->label18->Text = L"Index";
         // 
         // textBox_DeviceIndex
         // 
         this->textBox_DeviceIndex->Enabled = false;
         this->textBox_DeviceIndex->Location = System::Drawing::Point(171, 16);
         this->textBox_DeviceIndex->Name = L"textBox_DeviceIndex";
         this->textBox_DeviceIndex->Size = System::Drawing::Size(27, 20);
         this->textBox_DeviceIndex->TabIndex = 24;
         this->textBox_DeviceIndex->Text = L"0";
         // 
         // textBox_DeviceType
         // 
         this->textBox_DeviceType->Enabled = false;
         this->textBox_DeviceType->Location = System::Drawing::Point(76, 60);
         this->textBox_DeviceType->Name = L"textBox_DeviceType";
         this->textBox_DeviceType->Size = System::Drawing::Size(41, 20);
         this->textBox_DeviceType->TabIndex = 22;
         this->textBox_DeviceType->Text = L"16";
         // 
         // textBox_TxType
         // 
         this->textBox_TxType->Enabled = false;
         this->textBox_TxType->Location = System::Drawing::Point(76, 38);
         this->textBox_TxType->Name = L"textBox_TxType";
         this->textBox_TxType->Size = System::Drawing::Size(41, 20);
         this->textBox_TxType->TabIndex = 21;
         this->textBox_TxType->Text = L"5";
         // 
         // textBox_DeviceNumber
         // 
         this->textBox_DeviceNumber->Enabled = false;
         this->textBox_DeviceNumber->Location = System::Drawing::Point(76, 16);
         this->textBox_DeviceNumber->Name = L"textBox_DeviceNumber";
         this->textBox_DeviceNumber->Size = System::Drawing::Size(41, 20);
         this->textBox_DeviceNumber->TabIndex = 20;
         this->textBox_DeviceNumber->Text = L"4567";
         // 
         // label17
         // 
         this->label17->AutoSize = true;
         this->label17->Location = System::Drawing::Point(2, 63);
         this->label17->Name = L"label17";
         this->label17->Size = System::Drawing::Size(68, 13);
         this->label17->TabIndex = 19;
         this->label17->Text = L"Device Type";
         // 
         // label16
         // 
         this->label16->AutoSize = true;
         this->label16->Location = System::Drawing::Point(24, 41);
         this->label16->Name = L"label16";
         this->label16->Size = System::Drawing::Size(46, 13);
         this->label16->TabIndex = 18;
         this->label16->Text = L"Tx Type";
         // 
         // label15
         // 
         this->label15->AutoSize = true;
         this->label15->Location = System::Drawing::Point(19, 19);
         this->label15->Name = L"label15";
         this->label15->Size = System::Drawing::Size(51, 13);
         this->label15->TabIndex = 17;
         this->label15->Text = L"Device #";
         // 
         // label14
         // 
         this->label14->AutoSize = true;
         this->label14->Location = System::Drawing::Point(204, 6);
         this->label14->Name = L"label14";
         this->label14->Size = System::Drawing::Size(89, 13);
         this->label14->TabIndex = 16;
         this->label14->Text = L"Connection State";
         // 
         // label13
         // 
         this->label13->AutoSize = true;
         this->label13->Location = System::Drawing::Point(225, 33);
         this->label13->Name = L"label13";
         this->label13->Size = System::Drawing::Size(68, 13);
         this->label13->TabIndex = 15;
         this->label13->Text = L"Network Key";
         // 
         // cbNetworkKey
         // 
         this->cbNetworkKey->Enabled = false;
         this->cbNetworkKey->FormattingEnabled = true;
         this->cbNetworkKey->Items->AddRange(gcnew cli::array< System::Object^  >(4) {L"Public", L"Private", L"ANT+", L"ANT-FS"});
         this->cbNetworkKey->Location = System::Drawing::Point(299, 30);
         this->cbNetworkKey->Name = L"cbNetworkKey";
         this->cbNetworkKey->Size = System::Drawing::Size(77, 21);
         this->cbNetworkKey->TabIndex = 14;
         this->cbNetworkKey->Text = L"ANT+";
         // 
         // cbPaired
         // 
         this->cbPaired->AutoSize = true;
         this->cbPaired->Enabled = false;
         this->cbPaired->Location = System::Drawing::Point(237, 61);
         this->cbPaired->Name = L"cbPaired";
         this->cbPaired->Size = System::Drawing::Size(56, 17);
         this->cbPaired->TabIndex = 13;
         this->cbPaired->Text = L"Paired";
         this->cbPaired->UseVisualStyleBackColor = true;
         // 
         // cbConnectionState
         // 
         this->cbConnectionState->Enabled = false;
         this->cbConnectionState->FormattingEnabled = true;
         this->cbConnectionState->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"Closed", L"Searching", L"Tracking"});
         this->cbConnectionState->Location = System::Drawing::Point(299, 3);
         this->cbConnectionState->Name = L"cbConnectionState";
         this->cbConnectionState->Size = System::Drawing::Size(77, 21);
         this->cbConnectionState->TabIndex = 12;
         this->cbConnectionState->Text = L"Searching";
         // 
         // button_UpdatePeripheral
         // 
         this->button_UpdatePeripheral->Enabled = false;
         this->button_UpdatePeripheral->Location = System::Drawing::Point(299, 58);
         this->button_UpdatePeripheral->Name = L"button_UpdatePeripheral";
         this->button_UpdatePeripheral->Size = System::Drawing::Size(77, 20);
         this->button_UpdatePeripheral->TabIndex = 11;
         this->button_UpdatePeripheral->Text = L"Update All";
         this->button_UpdatePeripheral->UseVisualStyleBackColor = true;
         this->button_UpdatePeripheral->Click += gcnew System::EventHandler(this, &AudioDevice::button_UpdatePeripheral_Click);
         // 
         // checkBox_PairedDevices
         // 
         this->checkBox_PairedDevices->AutoSize = true;
         this->checkBox_PairedDevices->Location = System::Drawing::Point(0, 0);
         this->checkBox_PairedDevices->Name = L"checkBox_PairedDevices";
         this->checkBox_PairedDevices->Size = System::Drawing::Size(174, 17);
         this->checkBox_PairedDevices->TabIndex = 10;
         this->checkBox_PairedDevices->Text = L"Enable Peripheral Device Page";
         this->checkBox_PairedDevices->UseVisualStyleBackColor = true;
         this->checkBox_PairedDevices->CheckedChanged += gcnew System::EventHandler(this, &AudioDevice::checkBox_PairedDevices_CheckedChanged);
         // 
         // panel_Display
         // 
         this->panel_Display->Location = System::Drawing::Point(58, 188);
         this->panel_Display->Name = L"panel_Display";
         this->panel_Display->Size = System::Drawing::Size(200, 90);
         this->panel_Display->TabIndex = 1;
         // 
         // AudioDevice
         // 
         this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
         this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
         this->ClientSize = System::Drawing::Size(794, 351);
         this->Controls->Add(this->panel_Display);
         this->Controls->Add(this->panel_Settings);
         this->Name = L"AudioDevice";
         this->Text = L"Audio Device";
         this->panel_Settings->ResumeLayout(false);
         this->tabControl1->ResumeLayout(false);
         this->tabPage1->ResumeLayout(false);
         this->tabControl3->ResumeLayout(false);
         this->tabPage4->ResumeLayout(false);
         this->tabPage4->PerformLayout();
         this->groupBox1->ResumeLayout(false);
         this->groupBox1->PerformLayout();
         (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->nmVolume))->EndInit();
         this->tabPage8->ResumeLayout(false);
         this->tabPage8->PerformLayout();
         (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->nmCurrentTime))->EndInit();
         (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->nmTotalTime))->EndInit();
         this->tabPage2->ResumeLayout(false);
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
         (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->nmSizeExponent))->EndInit();
         (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->nmPercentUsed))->EndInit();
         this->tabPage3->ResumeLayout(false);
         this->tabPage3->PerformLayout();
         this->ResumeLayout(false);

      }
#pragma endregion
};