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

public ref class AudioRemote : public System::Windows::Forms::Form, public ISimBase{
public:
	// The constructor may include the following parameters:
	// System::Timers::Timer^ channelTimer:  Timer handle, if using a timer (not required in most cases, since getTimerInterval gets called after onTimerTock on each tock)
	// dRequestAckMsg^ channelAckMsg:  Handle to delegate function to send acknowledged messages (only needed if the device needs to send acknowledged messages)
	// These parameters may or may not be included in the constructor, as needed
		AudioRemote(System::Timers::Timer^ channelTimer, dRequestAckMsg^ channelAckMsg){
			InitializeComponent();
			requestAckMsg = channelAckMsg;	
            AudioData = gcnew AntPlusControls();
            CommonPages = gcnew CommonData();
			InitializeSim();
		}

		~AudioRemote(){
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
    virtual UCHAR getDeviceType(){return AntPlusControls::DEVICE_TYPE;} // Controls device type
    virtual UCHAR getTransmissionType(){return 0;} // 0 for wildcard
    virtual USHORT getTransmitPeriod(){return AntPlusControls::MSG_PERIOD;} // Controls message period
	virtual DOUBLE getTimerInterval(){return 3600000;}	// Set to any value (receiver does not use timer)
	virtual System::Windows::Forms::Panel^ getSimSettingsPanel(){return this->panel_Settings;}
	virtual System::Windows::Forms::Panel^ getSimTranslatedDisplay(){return this->panel_Display;}

// Optional methods (not required by interface class)
private:
	void InitializeSim();						// Initialize simulation
   void UpdateUI(UCHAR ucPageNumber_);
   BOOL HandleRetransmit();
   void UpdateDisplayAckStatus(UCHAR ucStatus_);
   void SendCommand(UCHAR ucPageNumber_);
   System::String^ ConvertToMinSec(USHORT usTime_);

   System::Void btAudioCommands_Click(System::Object^  sender, System::EventArgs^  e);
   System::Void cbInvalidVolInc_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
   System::Void button_UpdateSerial_Click(System::Object^  sender, System::EventArgs^  e);
   System::Void checkBox_UnknownSerialNumber_CheckedChanged(System::Object^  sender, System::EventArgs^  e);

private:
   dRequestAckMsg^ requestAckMsg;		// Delegate handle (ACK messages)
	
   CommonData^ CommonPages;            // Handle to CommonPages class
   AntPlusControls^ AudioData;         // Handle to AntPlusControls class

   UCHAR ucAckRetryCount;              // Holds number of retried Ack messages
   UCHAR ucMsgExpectingAck;            // Holds the message that is expecting an Ack


private: System::Windows::Forms::Panel^  panel_Settings;
private: System::Windows::Forms::Panel^  panel_Display;
private: System::Windows::Forms::TabControl^  tabControl2;
private: System::Windows::Forms::TabPage^  tabPage1;
private: System::Windows::Forms::Label^  lbShuffleState;
private: System::Windows::Forms::ProgressBar^  pbAudio;
private: System::Windows::Forms::Label^  lbTotalTrackTime;
private: System::Windows::Forms::Label^  lbRepeatState;
private: System::Windows::Forms::Label^  lbCurrentTrackTime;
private: System::Windows::Forms::Label^  label7;
private: System::Windows::Forms::Label^  label11;
private: System::Windows::Forms::Label^  lbAudioState;
private: System::Windows::Forms::Label^  lbVolume;
private: System::Windows::Forms::Label^  label5;
private: System::Windows::Forms::Label^  label10;
private: System::Windows::Forms::TabPage^  tabPage2;
private: System::Windows::Forms::TabControl^  tabControl3;
private: System::Windows::Forms::TabPage^  tabPage5;
private: System::Windows::Forms::Label^  label_Glb_ManfID;
private: System::Windows::Forms::Label^  label_Glb_ModelNum;
private: System::Windows::Forms::Label^  label_SoftwareVer;
private: System::Windows::Forms::Label^  label_Glb_HardwareVer;
private: System::Windows::Forms::Label^  label_ManuID;
private: System::Windows::Forms::Label^  label_Glb_SerialNum;
private: System::Windows::Forms::Label^  label_HardwareVer;
private: System::Windows::Forms::Label^  label_SerialNumber;
private: System::Windows::Forms::Label^  label_ModelNumber;
private: System::Windows::Forms::Label^  label_Glb_SoftwareVer;
private: System::Windows::Forms::TabPage^  tabPage6;
private: System::Windows::Forms::Label^  label21;
private: System::Windows::Forms::Label^  labelTimeRes;
private: System::Windows::Forms::Label^  label15;
private: System::Windows::Forms::Label^  labelOpTime;
private: System::Windows::Forms::Label^  label16;
private: System::Windows::Forms::Label^  labelBattVolt;
private: System::Windows::Forms::Label^  label20;
private: System::Windows::Forms::Label^  labelBattStatus;
private: System::Windows::Forms::TabPage^  tabPage7;
private: System::Windows::Forms::Label^  lbSizeUnit;
private: System::Windows::Forms::Label^  lbSizeExponent;
private: System::Windows::Forms::Label^  label9;
private: System::Windows::Forms::Label^  label8;
private: System::Windows::Forms::Label^  lbPercentUsed;
private: System::Windows::Forms::Label^  label6;
private: System::Windows::Forms::TabPage^  tabPage4;
private: System::Windows::Forms::Label^  lbNetworkKey;
private: System::Windows::Forms::Label^  lbConnStat;
private: System::Windows::Forms::Label^  lbPairingStat;
private: System::Windows::Forms::Label^  lbDeviceId;
private: System::Windows::Forms::Label^  lbTotalDevices;
private: System::Windows::Forms::Label^  lbDeviceIndex;
private: System::Windows::Forms::Label^  label19;
private: System::Windows::Forms::Label^  label18;
private: System::Windows::Forms::Label^  label17;
private: System::Windows::Forms::Label^  label14;
private: System::Windows::Forms::Label^  label13;
private: System::Windows::Forms::Label^  label12;
private: System::Windows::Forms::Label^  lbAckStatus;
private: System::Windows::Forms::Label^  label1;
private: System::Windows::Forms::CheckBox^  cbInvalidVolInc;
private: System::Windows::Forms::Label^  label3;
private: System::Windows::Forms::Button^  btAudioCommands;
private: System::Windows::Forms::ComboBox^  cbAudioCommands;
private: System::Windows::Forms::NumericUpDown^  nmAudioVolInc;
private: System::Windows::Forms::Label^  label2;
private: System::Windows::Forms::Label^  lbCustomRepeat;
private: System::Windows::Forms::Label^  lbCustomShuffle;
private: System::Windows::Forms::Label^  label22;
private: System::Windows::Forms::Label^  label4;
private: System::Windows::Forms::TabPage^  tabPage3;
private: System::Windows::Forms::Button^  button_UpdateSerial;
private: System::Windows::Forms::Label^  label_Serial;
private: System::Windows::Forms::Label^  label_SerialNumberError;
private: System::Windows::Forms::TextBox^  textBox_Serial;
private: System::Windows::Forms::CheckBox^  checkBox_UnknownSerialNumber;
private: System::Windows::Forms::Label^  label23;
private: System::Windows::Forms::Label^  lbSongTitle;


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
         this->tabControl2 = (gcnew System::Windows::Forms::TabControl());
         this->tabPage1 = (gcnew System::Windows::Forms::TabPage());
         this->lbCustomRepeat = (gcnew System::Windows::Forms::Label());
         this->lbCustomShuffle = (gcnew System::Windows::Forms::Label());
         this->label22 = (gcnew System::Windows::Forms::Label());
         this->label4 = (gcnew System::Windows::Forms::Label());
         this->lbShuffleState = (gcnew System::Windows::Forms::Label());
         this->pbAudio = (gcnew System::Windows::Forms::ProgressBar());
         this->lbTotalTrackTime = (gcnew System::Windows::Forms::Label());
         this->lbRepeatState = (gcnew System::Windows::Forms::Label());
         this->lbCurrentTrackTime = (gcnew System::Windows::Forms::Label());
         this->label7 = (gcnew System::Windows::Forms::Label());
         this->label11 = (gcnew System::Windows::Forms::Label());
         this->lbAudioState = (gcnew System::Windows::Forms::Label());
         this->lbVolume = (gcnew System::Windows::Forms::Label());
         this->label5 = (gcnew System::Windows::Forms::Label());
         this->label10 = (gcnew System::Windows::Forms::Label());
         this->tabPage3 = (gcnew System::Windows::Forms::TabPage());
         this->button_UpdateSerial = (gcnew System::Windows::Forms::Button());
         this->label_Serial = (gcnew System::Windows::Forms::Label());
         this->label_SerialNumberError = (gcnew System::Windows::Forms::Label());
         this->textBox_Serial = (gcnew System::Windows::Forms::TextBox());
         this->checkBox_UnknownSerialNumber = (gcnew System::Windows::Forms::CheckBox());
         this->tabPage2 = (gcnew System::Windows::Forms::TabPage());
         this->tabControl3 = (gcnew System::Windows::Forms::TabControl());
         this->tabPage5 = (gcnew System::Windows::Forms::TabPage());
         this->label_Glb_ManfID = (gcnew System::Windows::Forms::Label());
         this->label_Glb_ModelNum = (gcnew System::Windows::Forms::Label());
         this->label_SoftwareVer = (gcnew System::Windows::Forms::Label());
         this->label_Glb_HardwareVer = (gcnew System::Windows::Forms::Label());
         this->label_ManuID = (gcnew System::Windows::Forms::Label());
         this->label_Glb_SerialNum = (gcnew System::Windows::Forms::Label());
         this->label_HardwareVer = (gcnew System::Windows::Forms::Label());
         this->label_SerialNumber = (gcnew System::Windows::Forms::Label());
         this->label_ModelNumber = (gcnew System::Windows::Forms::Label());
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
         this->lbSizeUnit = (gcnew System::Windows::Forms::Label());
         this->lbSizeExponent = (gcnew System::Windows::Forms::Label());
         this->label9 = (gcnew System::Windows::Forms::Label());
         this->label8 = (gcnew System::Windows::Forms::Label());
         this->lbPercentUsed = (gcnew System::Windows::Forms::Label());
         this->label6 = (gcnew System::Windows::Forms::Label());
         this->tabPage4 = (gcnew System::Windows::Forms::TabPage());
         this->lbNetworkKey = (gcnew System::Windows::Forms::Label());
         this->lbConnStat = (gcnew System::Windows::Forms::Label());
         this->lbPairingStat = (gcnew System::Windows::Forms::Label());
         this->lbDeviceId = (gcnew System::Windows::Forms::Label());
         this->lbTotalDevices = (gcnew System::Windows::Forms::Label());
         this->lbDeviceIndex = (gcnew System::Windows::Forms::Label());
         this->label19 = (gcnew System::Windows::Forms::Label());
         this->label18 = (gcnew System::Windows::Forms::Label());
         this->label17 = (gcnew System::Windows::Forms::Label());
         this->label14 = (gcnew System::Windows::Forms::Label());
         this->label13 = (gcnew System::Windows::Forms::Label());
         this->label12 = (gcnew System::Windows::Forms::Label());
         this->panel_Display = (gcnew System::Windows::Forms::Panel());
         this->lbAckStatus = (gcnew System::Windows::Forms::Label());
         this->label1 = (gcnew System::Windows::Forms::Label());
         this->cbInvalidVolInc = (gcnew System::Windows::Forms::CheckBox());
         this->label3 = (gcnew System::Windows::Forms::Label());
         this->btAudioCommands = (gcnew System::Windows::Forms::Button());
         this->cbAudioCommands = (gcnew System::Windows::Forms::ComboBox());
         this->nmAudioVolInc = (gcnew System::Windows::Forms::NumericUpDown());
         this->label2 = (gcnew System::Windows::Forms::Label());
         this->label23 = (gcnew System::Windows::Forms::Label());
         this->lbSongTitle = (gcnew System::Windows::Forms::Label());
         this->panel_Settings->SuspendLayout();
         this->tabControl2->SuspendLayout();
         this->tabPage1->SuspendLayout();
         this->tabPage3->SuspendLayout();
         this->tabPage2->SuspendLayout();
         this->tabControl3->SuspendLayout();
         this->tabPage5->SuspendLayout();
         this->tabPage6->SuspendLayout();
         this->tabPage7->SuspendLayout();
         this->tabPage4->SuspendLayout();
         this->panel_Display->SuspendLayout();
         (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->nmAudioVolInc))->BeginInit();
         this->SuspendLayout();
         // 
         // panel_Settings
         // 
         this->panel_Settings->Controls->Add(this->tabControl2);
         this->panel_Settings->Location = System::Drawing::Point(322, 50);
         this->panel_Settings->Name = L"panel_Settings";
         this->panel_Settings->Size = System::Drawing::Size(400, 140);
         this->panel_Settings->TabIndex = 0;
         // 
         // tabControl2
         // 
         this->tabControl2->Controls->Add(this->tabPage1);
         this->tabControl2->Controls->Add(this->tabPage3);
         this->tabControl2->Controls->Add(this->tabPage2);
         this->tabControl2->Location = System::Drawing::Point(0, 2);
         this->tabControl2->Name = L"tabControl2";
         this->tabControl2->SelectedIndex = 0;
         this->tabControl2->Size = System::Drawing::Size(400, 137);
         this->tabControl2->TabIndex = 1;
         // 
         // tabPage1
         // 
         this->tabPage1->Controls->Add(this->lbSongTitle);
         this->tabPage1->Controls->Add(this->label23);
         this->tabPage1->Controls->Add(this->lbCustomRepeat);
         this->tabPage1->Controls->Add(this->lbCustomShuffle);
         this->tabPage1->Controls->Add(this->label22);
         this->tabPage1->Controls->Add(this->label4);
         this->tabPage1->Controls->Add(this->lbShuffleState);
         this->tabPage1->Controls->Add(this->pbAudio);
         this->tabPage1->Controls->Add(this->lbTotalTrackTime);
         this->tabPage1->Controls->Add(this->lbRepeatState);
         this->tabPage1->Controls->Add(this->lbCurrentTrackTime);
         this->tabPage1->Controls->Add(this->label7);
         this->tabPage1->Controls->Add(this->label11);
         this->tabPage1->Controls->Add(this->lbAudioState);
         this->tabPage1->Controls->Add(this->lbVolume);
         this->tabPage1->Controls->Add(this->label5);
         this->tabPage1->Controls->Add(this->label10);
         this->tabPage1->Location = System::Drawing::Point(4, 22);
         this->tabPage1->Name = L"tabPage1";
         this->tabPage1->Padding = System::Windows::Forms::Padding(3);
         this->tabPage1->Size = System::Drawing::Size(392, 111);
         this->tabPage1->TabIndex = 0;
         this->tabPage1->Text = L"Audio";
         this->tabPage1->UseVisualStyleBackColor = true;
         // 
         // lbCustomRepeat
         // 
         this->lbCustomRepeat->AutoSize = true;
         this->lbCustomRepeat->Location = System::Drawing::Point(95, 42);
         this->lbCustomRepeat->Name = L"lbCustomRepeat";
         this->lbCustomRepeat->Size = System::Drawing::Size(19, 13);
         this->lbCustomRepeat->TabIndex = 57;
         this->lbCustomRepeat->Text = L"----";
         // 
         // lbCustomShuffle
         // 
         this->lbCustomShuffle->AutoSize = true;
         this->lbCustomShuffle->Location = System::Drawing::Point(265, 42);
         this->lbCustomShuffle->Name = L"lbCustomShuffle";
         this->lbCustomShuffle->Size = System::Drawing::Size(19, 13);
         this->lbCustomShuffle->TabIndex = 56;
         this->lbCustomShuffle->Text = L"----";
         // 
         // label22
         // 
         this->label22->AutoSize = true;
         this->label22->Location = System::Drawing::Point(181, 42);
         this->label22->Margin = System::Windows::Forms::Padding(3);
         this->label22->Name = L"label22";
         this->label22->Size = System::Drawing::Size(81, 13);
         this->label22->TabIndex = 55;
         this->label22->Text = L"Custom Shuffle:";
         // 
         // label4
         // 
         this->label4->AutoSize = true;
         this->label4->Location = System::Drawing::Point(6, 42);
         this->label4->Margin = System::Windows::Forms::Padding(3);
         this->label4->Name = L"label4";
         this->label4->Size = System::Drawing::Size(83, 13);
         this->label4->TabIndex = 54;
         this->label4->Text = L"Custom Repeat:";
         // 
         // lbShuffleState
         // 
         this->lbShuffleState->AutoSize = true;
         this->lbShuffleState->Location = System::Drawing::Point(265, 23);
         this->lbShuffleState->Name = L"lbShuffleState";
         this->lbShuffleState->Size = System::Drawing::Size(19, 13);
         this->lbShuffleState->TabIndex = 53;
         this->lbShuffleState->Text = L"----";
         // 
         // pbAudio
         // 
         this->pbAudio->Location = System::Drawing::Point(64, 78);
         this->pbAudio->Margin = System::Windows::Forms::Padding(1);
         this->pbAudio->Maximum = 65535;
         this->pbAudio->Name = L"pbAudio";
         this->pbAudio->Size = System::Drawing::Size(264, 27);
         this->pbAudio->Step = 1;
         this->pbAudio->Style = System::Windows::Forms::ProgressBarStyle::Continuous;
         this->pbAudio->TabIndex = 45;
         // 
         // lbTotalTrackTime
         // 
         this->lbTotalTrackTime->AutoSize = true;
         this->lbTotalTrackTime->Location = System::Drawing::Point(332, 85);
         this->lbTotalTrackTime->Margin = System::Windows::Forms::Padding(3);
         this->lbTotalTrackTime->Name = L"lbTotalTrackTime";
         this->lbTotalTrackTime->Size = System::Drawing::Size(49, 13);
         this->lbTotalTrackTime->TabIndex = 47;
         this->lbTotalTrackTime->Text = L"00:00:00";
         // 
         // lbRepeatState
         // 
         this->lbRepeatState->AutoSize = true;
         this->lbRepeatState->Location = System::Drawing::Point(95, 23);
         this->lbRepeatState->Name = L"lbRepeatState";
         this->lbRepeatState->Size = System::Drawing::Size(19, 13);
         this->lbRepeatState->TabIndex = 52;
         this->lbRepeatState->Text = L"----";
         // 
         // lbCurrentTrackTime
         // 
         this->lbCurrentTrackTime->AutoSize = true;
         this->lbCurrentTrackTime->Location = System::Drawing::Point(11, 85);
         this->lbCurrentTrackTime->Margin = System::Windows::Forms::Padding(3);
         this->lbCurrentTrackTime->Name = L"lbCurrentTrackTime";
         this->lbCurrentTrackTime->Size = System::Drawing::Size(49, 13);
         this->lbCurrentTrackTime->TabIndex = 46;
         this->lbCurrentTrackTime->Text = L"00:00:00";
         // 
         // label7
         // 
         this->label7->AutoSize = true;
         this->label7->Location = System::Drawing::Point(203, 4);
         this->label7->Margin = System::Windows::Forms::Padding(0, 1, 0, 1);
         this->label7->Name = L"label7";
         this->label7->Size = System::Drawing::Size(59, 13);
         this->label7->TabIndex = 48;
         this->label7->Text = L"Volume(%):";
         // 
         // label11
         // 
         this->label11->AutoSize = true;
         this->label11->Location = System::Drawing::Point(191, 23);
         this->label11->Name = L"label11";
         this->label11->Size = System::Drawing::Size(71, 13);
         this->label11->TabIndex = 51;
         this->label11->Text = L"Shuffle State:";
         // 
         // lbAudioState
         // 
         this->lbAudioState->AutoSize = true;
         this->lbAudioState->Location = System::Drawing::Point(95, 4);
         this->lbAudioState->Name = L"lbAudioState";
         this->lbAudioState->Size = System::Drawing::Size(19, 13);
         this->lbAudioState->TabIndex = 44;
         this->lbAudioState->Text = L"----";
         // 
         // lbVolume
         // 
         this->lbVolume->AutoSize = true;
         this->lbVolume->Location = System::Drawing::Point(265, 4);
         this->lbVolume->Name = L"lbVolume";
         this->lbVolume->Size = System::Drawing::Size(19, 13);
         this->lbVolume->TabIndex = 49;
         this->lbVolume->Text = L"----";
         // 
         // label5
         // 
         this->label5->AutoSize = true;
         this->label5->Location = System::Drawing::Point(24, 4);
         this->label5->Margin = System::Windows::Forms::Padding(3);
         this->label5->Name = L"label5";
         this->label5->Size = System::Drawing::Size(65, 13);
         this->label5->TabIndex = 43;
         this->label5->Text = L"Audio State:";
         // 
         // label10
         // 
         this->label10->AutoSize = true;
         this->label10->Location = System::Drawing::Point(16, 23);
         this->label10->Margin = System::Windows::Forms::Padding(3);
         this->label10->Name = L"label10";
         this->label10->Size = System::Drawing::Size(73, 13);
         this->label10->TabIndex = 50;
         this->label10->Text = L"Repeat State:";
         // 
         // tabPage3
         // 
         this->tabPage3->Controls->Add(this->button_UpdateSerial);
         this->tabPage3->Controls->Add(this->label_Serial);
         this->tabPage3->Controls->Add(this->label_SerialNumberError);
         this->tabPage3->Controls->Add(this->textBox_Serial);
         this->tabPage3->Controls->Add(this->checkBox_UnknownSerialNumber);
         this->tabPage3->Location = System::Drawing::Point(4, 22);
         this->tabPage3->Name = L"tabPage3";
         this->tabPage3->Size = System::Drawing::Size(392, 111);
         this->tabPage3->TabIndex = 2;
         this->tabPage3->Text = L"Command Data";
         this->tabPage3->UseVisualStyleBackColor = true;
         // 
         // button_UpdateSerial
         // 
         this->button_UpdateSerial->Location = System::Drawing::Point(248, 42);
         this->button_UpdateSerial->Name = L"button_UpdateSerial";
         this->button_UpdateSerial->Size = System::Drawing::Size(69, 23);
         this->button_UpdateSerial->TabIndex = 9;
         this->button_UpdateSerial->Text = L"Update ";
         this->button_UpdateSerial->UseVisualStyleBackColor = true;
         this->button_UpdateSerial->Click += gcnew System::EventHandler(this, &AudioRemote::button_UpdateSerial_Click);
         // 
         // label_Serial
         // 
         this->label_Serial->AutoSize = true;
         this->label_Serial->Location = System::Drawing::Point(40, 34);
         this->label_Serial->Name = L"label_Serial";
         this->label_Serial->Size = System::Drawing::Size(76, 13);
         this->label_Serial->TabIndex = 5;
         this->label_Serial->Text = L"Serial Number:";
         // 
         // label_SerialNumberError
         // 
         this->label_SerialNumberError->AutoSize = true;
         this->label_SerialNumberError->ForeColor = System::Drawing::Color::Red;
         this->label_SerialNumberError->Location = System::Drawing::Point(20, 84);
         this->label_SerialNumberError->Name = L"label_SerialNumberError";
         this->label_SerialNumberError->Size = System::Drawing::Size(93, 13);
         this->label_SerialNumberError->TabIndex = 8;
         this->label_SerialNumberError->Text = L"Error: Invalid Input";
         this->label_SerialNumberError->Visible = false;
         // 
         // textBox_Serial
         // 
         this->textBox_Serial->Location = System::Drawing::Point(122, 31);
         this->textBox_Serial->Name = L"textBox_Serial";
         this->textBox_Serial->Size = System::Drawing::Size(72, 20);
         this->textBox_Serial->TabIndex = 6;
         this->textBox_Serial->Text = L"19633";
         // 
         // checkBox_UnknownSerialNumber
         // 
         this->checkBox_UnknownSerialNumber->AutoSize = true;
         this->checkBox_UnknownSerialNumber->Location = System::Drawing::Point(43, 56);
         this->checkBox_UnknownSerialNumber->Name = L"checkBox_UnknownSerialNumber";
         this->checkBox_UnknownSerialNumber->Size = System::Drawing::Size(141, 17);
         this->checkBox_UnknownSerialNumber->TabIndex = 7;
         this->checkBox_UnknownSerialNumber->Text = L"Unknown Serial Number";
         this->checkBox_UnknownSerialNumber->UseVisualStyleBackColor = true;
         this->checkBox_UnknownSerialNumber->CheckedChanged += gcnew System::EventHandler(this, &AudioRemote::checkBox_UnknownSerialNumber_CheckedChanged);
         // 
         // tabPage2
         // 
         this->tabPage2->Controls->Add(this->tabControl3);
         this->tabPage2->Location = System::Drawing::Point(4, 22);
         this->tabPage2->Name = L"tabPage2";
         this->tabPage2->Padding = System::Windows::Forms::Padding(3);
         this->tabPage2->Size = System::Drawing::Size(392, 111);
         this->tabPage2->TabIndex = 1;
         this->tabPage2->Text = L"Common Data";
         this->tabPage2->UseVisualStyleBackColor = true;
         // 
         // tabControl3
         // 
         this->tabControl3->Controls->Add(this->tabPage5);
         this->tabControl3->Controls->Add(this->tabPage6);
         this->tabControl3->Controls->Add(this->tabPage7);
         this->tabControl3->Controls->Add(this->tabPage4);
         this->tabControl3->Location = System::Drawing::Point(2, 0);
         this->tabControl3->Name = L"tabControl3";
         this->tabControl3->SelectedIndex = 0;
         this->tabControl3->Size = System::Drawing::Size(389, 111);
         this->tabControl3->TabIndex = 21;
         // 
         // tabPage5
         // 
         this->tabPage5->Controls->Add(this->label_Glb_ManfID);
         this->tabPage5->Controls->Add(this->label_Glb_ModelNum);
         this->tabPage5->Controls->Add(this->label_SoftwareVer);
         this->tabPage5->Controls->Add(this->label_Glb_HardwareVer);
         this->tabPage5->Controls->Add(this->label_ManuID);
         this->tabPage5->Controls->Add(this->label_Glb_SerialNum);
         this->tabPage5->Controls->Add(this->label_HardwareVer);
         this->tabPage5->Controls->Add(this->label_SerialNumber);
         this->tabPage5->Controls->Add(this->label_ModelNumber);
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
         // label_Glb_ModelNum
         // 
         this->label_Glb_ModelNum->AutoSize = true;
         this->label_Glb_ModelNum->Location = System::Drawing::Point(45, 37);
         this->label_Glb_ModelNum->Name = L"label_Glb_ModelNum";
         this->label_Glb_ModelNum->Size = System::Drawing::Size(49, 13);
         this->label_Glb_ModelNum->TabIndex = 21;
         this->label_Glb_ModelNum->Text = L"Model #:";
         // 
         // label_SoftwareVer
         // 
         this->label_SoftwareVer->AutoSize = true;
         this->label_SoftwareVer->Location = System::Drawing::Point(252, 37);
         this->label_SoftwareVer->Name = L"label_SoftwareVer";
         this->label_SoftwareVer->Size = System::Drawing::Size(19, 13);
         this->label_SoftwareVer->TabIndex = 19;
         this->label_SoftwareVer->Text = L"----";
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
         // label_ManuID
         // 
         this->label_ManuID->AutoSize = true;
         this->label_ManuID->Location = System::Drawing::Point(100, 14);
         this->label_ManuID->Name = L"label_ManuID";
         this->label_ManuID->Size = System::Drawing::Size(19, 13);
         this->label_ManuID->TabIndex = 16;
         this->label_ManuID->Text = L"----";
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
         // label_HardwareVer
         // 
         this->label_HardwareVer->AutoSize = true;
         this->label_HardwareVer->Location = System::Drawing::Point(252, 14);
         this->label_HardwareVer->Name = L"label_HardwareVer";
         this->label_HardwareVer->Size = System::Drawing::Size(19, 13);
         this->label_HardwareVer->TabIndex = 18;
         this->label_HardwareVer->Text = L"----";
         // 
         // label_SerialNumber
         // 
         this->label_SerialNumber->AutoSize = true;
         this->label_SerialNumber->Location = System::Drawing::Point(100, 59);
         this->label_SerialNumber->Name = L"label_SerialNumber";
         this->label_SerialNumber->Size = System::Drawing::Size(19, 13);
         this->label_SerialNumber->TabIndex = 15;
         this->label_SerialNumber->Text = L"----";
         // 
         // label_ModelNumber
         // 
         this->label_ModelNumber->AutoSize = true;
         this->label_ModelNumber->Location = System::Drawing::Point(100, 37);
         this->label_ModelNumber->Name = L"label_ModelNumber";
         this->label_ModelNumber->Size = System::Drawing::Size(19, 13);
         this->label_ModelNumber->TabIndex = 17;
         this->label_ModelNumber->Text = L"----";
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
         this->tabPage7->Controls->Add(this->lbSizeUnit);
         this->tabPage7->Controls->Add(this->lbSizeExponent);
         this->tabPage7->Controls->Add(this->label9);
         this->tabPage7->Controls->Add(this->label8);
         this->tabPage7->Controls->Add(this->lbPercentUsed);
         this->tabPage7->Controls->Add(this->label6);
         this->tabPage7->Location = System::Drawing::Point(4, 22);
         this->tabPage7->Name = L"tabPage7";
         this->tabPage7->Size = System::Drawing::Size(381, 85);
         this->tabPage7->TabIndex = 2;
         this->tabPage7->Text = L"Memory Level";
         this->tabPage7->UseVisualStyleBackColor = true;
         // 
         // lbSizeUnit
         // 
         this->lbSizeUnit->AutoSize = true;
         this->lbSizeUnit->Location = System::Drawing::Point(133, 58);
         this->lbSizeUnit->Name = L"lbSizeUnit";
         this->lbSizeUnit->Size = System::Drawing::Size(19, 13);
         this->lbSizeUnit->TabIndex = 5;
         this->lbSizeUnit->Text = L"----";
         // 
         // lbSizeExponent
         // 
         this->lbSizeExponent->AutoSize = true;
         this->lbSizeExponent->Location = System::Drawing::Point(133, 35);
         this->lbSizeExponent->Name = L"lbSizeExponent";
         this->lbSizeExponent->Size = System::Drawing::Size(19, 13);
         this->lbSizeExponent->TabIndex = 4;
         this->lbSizeExponent->Text = L"----";
         // 
         // label9
         // 
         this->label9->AutoSize = true;
         this->label9->Location = System::Drawing::Point(73, 58);
         this->label9->Margin = System::Windows::Forms::Padding(5);
         this->label9->Name = L"label9";
         this->label9->Size = System::Drawing::Size(52, 13);
         this->label9->TabIndex = 3;
         this->label9->Text = L"Size Unit:";
         // 
         // label8
         // 
         this->label8->AutoSize = true;
         this->label8->Location = System::Drawing::Point(47, 35);
         this->label8->Margin = System::Windows::Forms::Padding(5);
         this->label8->Name = L"label8";
         this->label8->Size = System::Drawing::Size(78, 13);
         this->label8->TabIndex = 2;
         this->label8->Text = L"Size Exponent:";
         // 
         // lbPercentUsed
         // 
         this->lbPercentUsed->AutoSize = true;
         this->lbPercentUsed->Location = System::Drawing::Point(133, 12);
         this->lbPercentUsed->Name = L"lbPercentUsed";
         this->lbPercentUsed->Size = System::Drawing::Size(19, 13);
         this->lbPercentUsed->TabIndex = 1;
         this->lbPercentUsed->Text = L"----";
         // 
         // label6
         // 
         this->label6->AutoSize = true;
         this->label6->Location = System::Drawing::Point(79, 12);
         this->label6->Margin = System::Windows::Forms::Padding(5);
         this->label6->Name = L"label6";
         this->label6->Size = System::Drawing::Size(46, 13);
         this->label6->TabIndex = 0;
         this->label6->Text = L"% Used:";
         // 
         // tabPage4
         // 
         this->tabPage4->Controls->Add(this->lbNetworkKey);
         this->tabPage4->Controls->Add(this->lbConnStat);
         this->tabPage4->Controls->Add(this->lbPairingStat);
         this->tabPage4->Controls->Add(this->lbDeviceId);
         this->tabPage4->Controls->Add(this->lbTotalDevices);
         this->tabPage4->Controls->Add(this->lbDeviceIndex);
         this->tabPage4->Controls->Add(this->label19);
         this->tabPage4->Controls->Add(this->label18);
         this->tabPage4->Controls->Add(this->label17);
         this->tabPage4->Controls->Add(this->label14);
         this->tabPage4->Controls->Add(this->label13);
         this->tabPage4->Controls->Add(this->label12);
         this->tabPage4->Location = System::Drawing::Point(4, 22);
         this->tabPage4->Name = L"tabPage4";
         this->tabPage4->Size = System::Drawing::Size(381, 85);
         this->tabPage4->TabIndex = 3;
         this->tabPage4->Text = L"Peripheral Devices";
         this->tabPage4->UseVisualStyleBackColor = true;
         // 
         // lbNetworkKey
         // 
         this->lbNetworkKey->AutoSize = true;
         this->lbNetworkKey->Location = System::Drawing::Point(318, 61);
         this->lbNetworkKey->Name = L"lbNetworkKey";
         this->lbNetworkKey->Size = System::Drawing::Size(19, 13);
         this->lbNetworkKey->TabIndex = 9;
         this->lbNetworkKey->Text = L"----";
         // 
         // lbConnStat
         // 
         this->lbConnStat->AutoSize = true;
         this->lbConnStat->Location = System::Drawing::Point(318, 38);
         this->lbConnStat->Name = L"lbConnStat";
         this->lbConnStat->Size = System::Drawing::Size(19, 13);
         this->lbConnStat->TabIndex = 8;
         this->lbConnStat->Text = L"----";
         // 
         // lbPairingStat
         // 
         this->lbPairingStat->AutoSize = true;
         this->lbPairingStat->Location = System::Drawing::Point(318, 15);
         this->lbPairingStat->Name = L"lbPairingStat";
         this->lbPairingStat->Size = System::Drawing::Size(19, 13);
         this->lbPairingStat->TabIndex = 7;
         this->lbPairingStat->Text = L"----";
         // 
         // lbDeviceId
         // 
         this->lbDeviceId->AutoSize = true;
         this->lbDeviceId->Location = System::Drawing::Point(144, 61);
         this->lbDeviceId->Name = L"lbDeviceId";
         this->lbDeviceId->Size = System::Drawing::Size(19, 13);
         this->lbDeviceId->TabIndex = 6;
         this->lbDeviceId->Text = L"----";
         // 
         // lbTotalDevices
         // 
         this->lbTotalDevices->AutoSize = true;
         this->lbTotalDevices->Location = System::Drawing::Point(144, 38);
         this->lbTotalDevices->Name = L"lbTotalDevices";
         this->lbTotalDevices->Size = System::Drawing::Size(19, 13);
         this->lbTotalDevices->TabIndex = 5;
         this->lbTotalDevices->Text = L"----";
         // 
         // lbDeviceIndex
         // 
         this->lbDeviceIndex->AutoSize = true;
         this->lbDeviceIndex->Location = System::Drawing::Point(144, 15);
         this->lbDeviceIndex->Name = L"lbDeviceIndex";
         this->lbDeviceIndex->Size = System::Drawing::Size(19, 13);
         this->lbDeviceIndex->TabIndex = 4;
         this->lbDeviceIndex->Text = L"----";
         // 
         // label19
         // 
         this->label19->AutoSize = true;
         this->label19->Location = System::Drawing::Point(78, 61);
         this->label19->Margin = System::Windows::Forms::Padding(5);
         this->label19->Name = L"label19";
         this->label19->Size = System::Drawing::Size(58, 13);
         this->label19->TabIndex = 2;
         this->label19->Text = L"Device ID:";
         // 
         // label18
         // 
         this->label18->AutoSize = true;
         this->label18->Location = System::Drawing::Point(5, 38);
         this->label18->Margin = System::Windows::Forms::Padding(5);
         this->label18->Name = L"label18";
         this->label18->Size = System::Drawing::Size(131, 13);
         this->label18->TabIndex = 2;
         this->label18->Text = L"Total Connected Devices:";
         // 
         // label17
         // 
         this->label17->AutoSize = true;
         this->label17->Location = System::Drawing::Point(13, 15);
         this->label17->Margin = System::Windows::Forms::Padding(5);
         this->label17->Name = L"label17";
         this->label17->Size = System::Drawing::Size(123, 13);
         this->label17->TabIndex = 3;
         this->label17->Text = L"Peripheral Device Index:";
         // 
         // label14
         // 
         this->label14->AutoSize = true;
         this->label14->Location = System::Drawing::Point(239, 61);
         this->label14->Margin = System::Windows::Forms::Padding(5);
         this->label14->Name = L"label14";
         this->label14->Size = System::Drawing::Size(71, 13);
         this->label14->TabIndex = 2;
         this->label14->Text = L"Network Key:";
         // 
         // label13
         // 
         this->label13->AutoSize = true;
         this->label13->Location = System::Drawing::Point(213, 38);
         this->label13->Margin = System::Windows::Forms::Padding(5);
         this->label13->Name = L"label13";
         this->label13->Size = System::Drawing::Size(97, 13);
         this->label13->TabIndex = 2;
         this->label13->Text = L"Connection Status:";
         // 
         // label12
         // 
         this->label12->AutoSize = true;
         this->label12->Location = System::Drawing::Point(235, 15);
         this->label12->Margin = System::Windows::Forms::Padding(5);
         this->label12->Name = L"label12";
         this->label12->Size = System::Drawing::Size(75, 13);
         this->label12->TabIndex = 1;
         this->label12->Text = L"Pairing Status:";
         // 
         // panel_Display
         // 
         this->panel_Display->Controls->Add(this->lbAckStatus);
         this->panel_Display->Controls->Add(this->label1);
         this->panel_Display->Controls->Add(this->cbInvalidVolInc);
         this->panel_Display->Controls->Add(this->label3);
         this->panel_Display->Controls->Add(this->btAudioCommands);
         this->panel_Display->Controls->Add(this->cbAudioCommands);
         this->panel_Display->Controls->Add(this->nmAudioVolInc);
         this->panel_Display->Controls->Add(this->label2);
         this->panel_Display->Location = System::Drawing::Point(58, 188);
         this->panel_Display->Name = L"panel_Display";
         this->panel_Display->Size = System::Drawing::Size(200, 90);
         this->panel_Display->TabIndex = 1;
         // 
         // lbAckStatus
         // 
         this->lbAckStatus->AutoSize = true;
         this->lbAckStatus->Location = System::Drawing::Point(41, 24);
         this->lbAckStatus->Name = L"lbAckStatus";
         this->lbAckStatus->Size = System::Drawing::Size(19, 13);
         this->lbAckStatus->TabIndex = 30;
         this->lbAckStatus->Text = L"----";
         // 
         // label1
         // 
         this->label1->AutoSize = true;
         this->label1->Location = System::Drawing::Point(6, 24);
         this->label1->Name = L"label1";
         this->label1->Size = System::Drawing::Size(29, 13);
         this->label1->TabIndex = 29;
         this->label1->Text = L"Ack:";
         // 
         // cbInvalidVolInc
         // 
         this->cbInvalidVolInc->AutoSize = true;
         this->cbInvalidVolInc->Location = System::Drawing::Point(132, 69);
         this->cbInvalidVolInc->Name = L"cbInvalidVolInc";
         this->cbInvalidVolInc->Size = System::Drawing::Size(60, 17);
         this->cbInvalidVolInc->TabIndex = 26;
         this->cbInvalidVolInc->Text = L"Default";
         this->cbInvalidVolInc->UseVisualStyleBackColor = true;
         this->cbInvalidVolInc->CheckedChanged += gcnew System::EventHandler(this, &AudioRemote::cbInvalidVolInc_CheckedChanged);
         // 
         // label3
         // 
         this->label3->AutoSize = true;
         this->label3->Location = System::Drawing::Point(2, 2);
         this->label3->Name = L"label3";
         this->label3->Size = System::Drawing::Size(92, 13);
         this->label3->TabIndex = 27;
         this->label3->Text = L"Audio Commands:";
         // 
         // btAudioCommands
         // 
         this->btAudioCommands->Location = System::Drawing::Point(3, 69);
         this->btAudioCommands->Name = L"btAudioCommands";
         this->btAudioCommands->Size = System::Drawing::Size(105, 20);
         this->btAudioCommands->TabIndex = 28;
         this->btAudioCommands->Text = L"Send Command";
         this->btAudioCommands->UseVisualStyleBackColor = true;
         this->btAudioCommands->Click += gcnew System::EventHandler(this, &AudioRemote::btAudioCommands_Click);
         // 
         // cbAudioCommands
         // 
         this->cbAudioCommands->FormattingEnabled = true;
         this->cbAudioCommands->Items->AddRange(gcnew cli::array< System::Object^  >(18) {L"Play", L"Pause", L"Stop", L"Volume +", 
            L"Volume -", L"Mute/Unmute", L"Track Ahead", L"Track Back", L"Repeat Track", L"Repeat All", L"Repeat Off", L"Shuffle Songs", 
            L"Shuffle Albums", L"Shuffle Off", L"FFWD", L"FRWD", L"Custom Repeat", L"Custom Shuffle"});
         this->cbAudioCommands->Location = System::Drawing::Point(6, 42);
         this->cbAudioCommands->Name = L"cbAudioCommands";
         this->cbAudioCommands->Size = System::Drawing::Size(102, 21);
         this->cbAudioCommands->TabIndex = 23;
         this->cbAudioCommands->Text = L"Custom Repeat";
         // 
         // nmAudioVolInc
         // 
         this->nmAudioVolInc->Location = System::Drawing::Point(132, 35);
         this->nmAudioVolInc->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
         this->nmAudioVolInc->Name = L"nmAudioVolInc";
         this->nmAudioVolInc->Size = System::Drawing::Size(51, 20);
         this->nmAudioVolInc->TabIndex = 25;
         this->nmAudioVolInc->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
         // 
         // label2
         // 
         this->label2->AutoSize = true;
         this->label2->Location = System::Drawing::Point(129, 19);
         this->label2->Name = L"label2";
         this->label2->Size = System::Drawing::Size(69, 13);
         this->label2->TabIndex = 24;
         this->label2->Text = L"Vol. Incr. (%):";
         // 
         // label23
         // 
         this->label23->AutoSize = true;
         this->label23->Location = System::Drawing::Point(31, 61);
         this->label23->Margin = System::Windows::Forms::Padding(3);
         this->label23->Name = L"label23";
         this->label23->Size = System::Drawing::Size(58, 13);
         this->label23->TabIndex = 58;
         this->label23->Text = L"Song Title:";
         // 
         // lbSongTitle
         // 
         this->lbSongTitle->AutoSize = true;
         this->lbSongTitle->Location = System::Drawing::Point(95, 61);
         this->lbSongTitle->Name = L"lbSongTitle";
         this->lbSongTitle->Size = System::Drawing::Size(19, 13);
         this->lbSongTitle->TabIndex = 59;
         this->lbSongTitle->Text = L"----";
         // 
         // AudioRemote
         // 
         this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
         this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
         this->ClientSize = System::Drawing::Size(794, 351);
         this->Controls->Add(this->panel_Display);
         this->Controls->Add(this->panel_Settings);
         this->Name = L"AudioRemote";
         this->Text = L"Audio Remote";
         this->panel_Settings->ResumeLayout(false);
         this->tabControl2->ResumeLayout(false);
         this->tabPage1->ResumeLayout(false);
         this->tabPage1->PerformLayout();
         this->tabPage3->ResumeLayout(false);
         this->tabPage3->PerformLayout();
         this->tabPage2->ResumeLayout(false);
         this->tabControl3->ResumeLayout(false);
         this->tabPage5->ResumeLayout(false);
         this->tabPage5->PerformLayout();
         this->tabPage6->ResumeLayout(false);
         this->tabPage6->PerformLayout();
         this->tabPage7->ResumeLayout(false);
         this->tabPage7->PerformLayout();
         this->tabPage4->ResumeLayout(false);
         this->tabPage4->PerformLayout();
         this->panel_Display->ResumeLayout(false);
         this->panel_Display->PerformLayout();
         (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->nmAudioVolInc))->EndInit();
         this->ResumeLayout(false);

      }
#pragma endregion
};