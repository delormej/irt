/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/


/*
For every available channel of a connected device, an instance of this class is created. 
The main program will then call for the created tab page and add it to the main page tab control. 
All the ANT communication in a channel is driven by this class, along with the opening, 
closing, and setting parameters of the ANT channel. The channel holds an event timer which is
used to open the channel after all parameters are set and then on every tick calls the simulator class
for an update to the transmit buffer and the timer interval. 
This class also receives and handles the channel event messages. 
*/


#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "StdAfx.h"
#include "types.h"
#include "antmessage.h"
#include "antdefines.h"
#include "ISimBase.h"
#include "SimTypes.h"

typedef enum
{
   STATE_CHANNEL_CLOSED,
   STATE_CHANNEL_OPENING,
   STATE_CHANNEL_OPEN
} AntChannelState;

public value struct CapabilitiesStruct
{
   UCHAR ucStandardOptions;
   UCHAR ucAdvancedOptions;
   UCHAR ucAdvancedOptions2;
};

typedef enum
{
   PLATFORM_AP1_ANT11TR,   
   PLATFORM_AP2_USB2_AT3,
   PLATFORM_C7,
   PLATFORM_UNKNOWN,
} ANTPlatform;

// List of TxPower Level Settings
#define TXPOWER_SETTINGS1 {L"-20 dBm", L"-10 dBm", L"-5 dBm", L"0 dBm", L"N/A"}
#define TXPOWER_SETTINGS2 {L"-18 dBm", L"-12 dBm", L"-6 dBm", L"0 dBm", L"N/A"}
#define TXPOWER_SETTINGS3 {L"-20 dBm", L"-10 dBm", L"-4 dBm", L"0 dBm", L"+4 dBm"}
#define TXPOWER_SETTINGS_UNKNOWN {L"0", L"1", L"2", L"3", L"4"}


public ref class ANTChannel : public System::Windows::Forms::Form{
public:
	ANTChannel(UCHAR ucInstanceNum_, UCHAR ucSimType_, ANTPlatform eHWType_, CapabilitiesStruct sCapabilities_);
	~ANTChannel(); 

ref struct ANTEventTable abstract sealed
{
    static initonly array<UCHAR>^ aucCode =
    {
        RESPONSE_NO_ERROR,
        EVENT_RX_SEARCH_TIMEOUT,
		EVENT_RX_FAIL,
		EVENT_TX,
		EVENT_TRANSFER_RX_FAILED,
		EVENT_TRANSFER_TX_COMPLETED,
		EVENT_TRANSFER_TX_FAILED,
		EVENT_CHANNEL_CLOSED,
		EVENT_RX_FAIL_GO_TO_SEARCH,
		EVENT_CHANNEL_COLLISION,
		EVENT_TRANSFER_TX_START,
		CHANNEL_IN_WRONG_STATE,
		CHANNEL_NOT_OPENED,
		CHANNEL_ID_NOT_SET,
		CLOSE_ALL_CHANNELS,
		TRANSFER_IN_PROGRESS,
		TRANSFER_SEQUENCE_NUMBER_ERROR,
		TRANSFER_IN_ERROR,
		INVALID_MESSAGE,
		INVALID_NETWORK_NUMBER,
		INVALID_LIST_ID,
		INVALID_SCAN_TX_CHANNEL,
		NVM_FULL_ERROR,
		NVM_WRITE_ERROR
    };

	static initonly array<String^>^ aucDescr =
	{
		"RESPONSE_NO_ERROR",
		"EVENT_RX_SEARCH_TIMEOUT",
		"EVENT_RX_FAIL",
		"EVENT_TX",
		"EVENT_TRANSFER_RX_FAILED",
		"EVENT_TRANSFER_TX_COMPLETED",
		"EVENT_TRANSFER_TX_FAILED",
		"EVENT_CHANNEL_CLOSED",
		"EVENT_RX_FAIL_GO_TO_SEARCH",
		"EVENT_CHANNEL_COLLISION",
		"EVENT_TRANSFER_TX_START",
		"CHANNEL_IN_WRONG_STATE",
		"CHANNEL_NOT_OPENED",
		"CHANNEL_ID_NOT_SET",
		"CLOSE_ALL_CHANNELS",
		"TRANSFER_IN_PROGRESS",
		"TRANSFER_SEQUENCE_NUMBER_ERROR",
		"TRANSFER_IN_ERROR",
		"INVALID_MESSAGE",
		"INVALID_NETWORK_NUMBER",
		"INVALID_LIST_ID",
		"INVALID_SCAN_TX_CHANNEL",
		"NVM_FULL_ERROR",
		"NVM_WRITE_ERROR"
	};
};

   
public: 
   UCHAR ucMyChannelNum;

public:
   void ANTChannelEvent(UCHAR ucEvent_, UCHAR* pcBuffer_);
   void ANTProtocolEvent(UCHAR ucMessageCode_, UCHAR* pcBuffer_);
   System::Windows::Forms::TabPage^ getTab(){return this->tabPage_ChannelTab;}

private:
   BOOL OpenChannel(UCHAR ucChannelType_);
   void ChannelOpenFailed();   
	void sendSimMsg();
	void updateMesgPeriod(USHORT usMesgPeriod_);
	void sendAckMsg(UCHAR* pucTxBuffer_);
	void sendBroadcastMsg(UCHAR* pucTxBuffer_);	
	BOOL ValidateChannelID();
   void SetPairingBit();

#pragma region Windows Form Designer Control Events
   System::Void UpdateRawTxDisplay(System::String^ strLabel_, UCHAR* pucBuffer_);
	System::Void timer_SimEvent_Tick(System::Object^  sender, System::Timers::ElapsedEventArgs^ e);
	System::Void comboBox_SimSelect_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_TxEnable_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_PairingOn_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void button_getChannelID_Click(System::Object^  sender, System::EventArgs^  e);
	System::Void checkBox_DisplayAsHex_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void button_Clear_Click(System::Object^  sender, System::EventArgs^  e);   
   System::Void comboBox_TxPowerSelect_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
   System::Void tabPage_ChannelTab_Enter(System::Object^  sender, System::EventArgs^  e);           
#pragma endregion

private:
	ISimBase^ curSimulator;						// Current (active) simulator handle

	System::Timers::Timer^  timer_SimEvent;		// Timer, for event-driven simulations
	UCHAR ucSimType;							   // Simulation type
	USHORT usMyDeviceNum;						// Device number
	UCHAR ucMyDeviceType;						// Device type
	UCHAR ucMyTxType;							   // Transmission type
	USHORT usMyMsgPeriod;						// Message period
	DOUBLE dCurEventTime;						// Current time (ms), for event-driven simulations
	BOOL bPairingBit;							   // Use pairing bit
	BOOL bDisplayHex;							   // Display in hex values (otherwise, use decimal values)
	AntChannelState eMyChannelState;			// ANT Channel State
	BOOL bMyCheckInternalRaise;				// Indicates whther checkbox was set by user or internally
	BOOL bAlreadyUnAssigned;					// For dealing with differences in how AP2 search timeouts
   CapabilitiesStruct sMyCapabilities;    // Copy of the Capabilities bytes
   BOOL bTXPowerUIEnabled;                // Indicates if control ready to respond to user events
   static INT iTheTXPowerLevel = DEFAULT_RADIO_TX_POWER;
   
#pragma region Windows Form Designer Control Declarations
private: System::Windows::Forms::Panel^  panelPanelTestControl;   
private: System::Windows::Forms::TabControl^  tabControl1;
private: System::Windows::Forms::TabPage^  tabPage_ChannelTab;
private: System::Windows::Forms::RichTextBox^  richTextBox_RawTxDisplay;
private: System::Windows::Forms::CheckBox^ checkBox_TxEnable;
private: System::Windows::Forms::ComboBox^ comboBox_SimSelect;
private: System::Windows::Forms::Panel^ panel_channelSettings;
private: System::Windows::Forms::TextBox^ textBox_DeviceID;
private: System::Windows::Forms::Label^ label_DeviceId;
private: System::Windows::Forms::Label^ label_DeviceType;
private: System::Windows::Forms::TextBox^ textBox_DeviceType;
private: System::Windows::Forms::Label^ label_TransmitPeriod;
private: System::Windows::Forms::TextBox^ textBox_TransmitPeriod;
private: System::Windows::Forms::Label^ label_TransmissionType;
private: System::Windows::Forms::TextBox^ textBox_TransmissionType;
private: System::Windows::Forms::CheckBox^ checkBox_DisplayAsHex;
private: System::Windows::Forms::Label^  label_TxTime;
private: System::Windows::Forms::Label^  label_RawBox;
private: System::Windows::Forms::Label^  label_TxTimeDisplay;
private: System::Windows::Forms::Label^  label_ChannelConfig;
private: System::Windows::Forms::Panel^  panel_Sim_PLACEHOLDER;
private: System::Windows::Forms::Panel^  panel_SimDisplay_PLACEHOLDER;
private: System::Windows::Forms::Button^  button_getChannelID;
private: System::Windows::Forms::CheckBox^  checkBox_PairingOn;
private: System::ComponentModel::IContainer^  components;
private: System::Windows::Forms::Button^  button_Clear;
private: System::Windows::Forms::ComboBox^  comboBox_TxPowerSelect;
private: System::Windows::Forms::Label^  label_TxPower;


private:
	/// <summary>
	/// Required designer variable.
	/// </summary>
#pragma endregion

#pragma region Windows Form Designer generated code
	/// <summary>
	/// Required method for Designer support - do not modify
	/// the contents of this method with the code editor.
	/// </summary>
	void InitializeComponent(void)
	{
      this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
      this->tabPage_ChannelTab = (gcnew System::Windows::Forms::TabPage());
      this->button_Clear = (gcnew System::Windows::Forms::Button());
      this->label_RawBox = (gcnew System::Windows::Forms::Label());
      this->label_TxTime = (gcnew System::Windows::Forms::Label());
      this->panel_SimDisplay_PLACEHOLDER = (gcnew System::Windows::Forms::Panel());
      this->panel_Sim_PLACEHOLDER = (gcnew System::Windows::Forms::Panel());
      this->label_TxTimeDisplay = (gcnew System::Windows::Forms::Label());
      this->checkBox_DisplayAsHex = (gcnew System::Windows::Forms::CheckBox());
      this->richTextBox_RawTxDisplay = (gcnew System::Windows::Forms::RichTextBox());
      this->panel_channelSettings = (gcnew System::Windows::Forms::Panel());
      this->label_TxPower = (gcnew System::Windows::Forms::Label());
      this->comboBox_TxPowerSelect = (gcnew System::Windows::Forms::ComboBox());
      this->button_getChannelID = (gcnew System::Windows::Forms::Button());
      this->checkBox_PairingOn = (gcnew System::Windows::Forms::CheckBox());
      this->label_ChannelConfig = (gcnew System::Windows::Forms::Label());
      this->label_TransmissionType = (gcnew System::Windows::Forms::Label());
      this->textBox_TransmissionType = (gcnew System::Windows::Forms::TextBox());
      this->label_TransmitPeriod = (gcnew System::Windows::Forms::Label());
      this->textBox_TransmitPeriod = (gcnew System::Windows::Forms::TextBox());
      this->label_DeviceType = (gcnew System::Windows::Forms::Label());
      this->textBox_DeviceType = (gcnew System::Windows::Forms::TextBox());
      this->label_DeviceId = (gcnew System::Windows::Forms::Label());
      this->textBox_DeviceID = (gcnew System::Windows::Forms::TextBox());
      this->checkBox_TxEnable = (gcnew System::Windows::Forms::CheckBox());
      this->comboBox_SimSelect = (gcnew System::Windows::Forms::ComboBox());
      this->tabControl1->SuspendLayout();
      this->tabPage_ChannelTab->SuspendLayout();
      this->panel_channelSettings->SuspendLayout();
      this->SuspendLayout();
      // 
      // tabControl1
      // 
      this->tabControl1->Controls->Add(this->tabPage_ChannelTab);
      this->tabControl1->Location = System::Drawing::Point(48, 39);
      this->tabControl1->Name = L"tabControl1";
      this->tabControl1->SelectedIndex = 0;
      this->tabControl1->Size = System::Drawing::Size(740, 325);
      this->tabControl1->TabIndex = 0;
      // 
      // tabPage_ChannelTab
      // 
      this->tabPage_ChannelTab->Controls->Add(this->button_Clear);
      this->tabPage_ChannelTab->Controls->Add(this->label_RawBox);
      this->tabPage_ChannelTab->Controls->Add(this->label_TxTime);
      this->tabPage_ChannelTab->Controls->Add(this->panel_SimDisplay_PLACEHOLDER);
      this->tabPage_ChannelTab->Controls->Add(this->panel_Sim_PLACEHOLDER);
      this->tabPage_ChannelTab->Controls->Add(this->label_TxTimeDisplay);
      this->tabPage_ChannelTab->Controls->Add(this->checkBox_DisplayAsHex);
      this->tabPage_ChannelTab->Controls->Add(this->richTextBox_RawTxDisplay);
      this->tabPage_ChannelTab->Controls->Add(this->panel_channelSettings);
      this->tabPage_ChannelTab->Controls->Add(this->checkBox_TxEnable);
      this->tabPage_ChannelTab->Controls->Add(this->comboBox_SimSelect);
      this->tabPage_ChannelTab->Location = System::Drawing::Point(4, 22);
      this->tabPage_ChannelTab->Name = L"tabPage_ChannelTab";
      this->tabPage_ChannelTab->Padding = System::Windows::Forms::Padding(3);
      this->tabPage_ChannelTab->Size = System::Drawing::Size(732, 299);
      this->tabPage_ChannelTab->TabIndex = 0;
      this->tabPage_ChannelTab->Text = L"Channel #";
      this->tabPage_ChannelTab->UseVisualStyleBackColor = true;
      this->tabPage_ChannelTab->Enter += gcnew System::EventHandler(this, &ANTChannel::tabPage_ChannelTab_Enter);
      // 
      // button_Clear
      // 
      this->button_Clear->Location = System::Drawing::Point(238, 6);
      this->button_Clear->Name = L"button_Clear";
      this->button_Clear->Size = System::Drawing::Size(56, 23);
      this->button_Clear->TabIndex = 12;
      this->button_Clear->Text = L"Clear";
      this->button_Clear->UseVisualStyleBackColor = true;
      this->button_Clear->Click += gcnew System::EventHandler(this, &ANTChannel::button_Clear_Click);
      // 
      // label_RawBox
      // 
      this->label_RawBox->AutoSize = true;
      this->label_RawBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Underline, System::Drawing::GraphicsUnit::Point, 
         static_cast<System::Byte>(0)));
      this->label_RawBox->Location = System::Drawing::Point(13, 14);
      this->label_RawBox->Name = L"label_RawBox";
      this->label_RawBox->Size = System::Drawing::Size(106, 13);
      this->label_RawBox->TabIndex = 11;
      this->label_RawBox->Text = L"ANT Channel Output";
      // 
      // label_TxTime
      // 
      this->label_TxTime->AutoSize = true;
      this->label_TxTime->Location = System::Drawing::Point(13, 155);
      this->label_TxTime->Name = L"label_TxTime";
      this->label_TxTime->Size = System::Drawing::Size(75, 13);
      this->label_TxTime->TabIndex = 10;
      this->label_TxTime->Text = L"Total Tx Time:";
      // 
      // panel_SimDisplay_PLACEHOLDER
      // 
      this->panel_SimDisplay_PLACEHOLDER->Location = System::Drawing::Point(58, 188);
      this->panel_SimDisplay_PLACEHOLDER->Name = L"panel_SimDisplay_PLACEHOLDER";
      this->panel_SimDisplay_PLACEHOLDER->Size = System::Drawing::Size(200, 90);
      this->panel_SimDisplay_PLACEHOLDER->TabIndex = 9;
      this->panel_SimDisplay_PLACEHOLDER->Visible = false;
      // 
      // panel_Sim_PLACEHOLDER
      // 
      this->panel_Sim_PLACEHOLDER->Location = System::Drawing::Point(322, 50);
      this->panel_Sim_PLACEHOLDER->Name = L"panel_Sim_PLACEHOLDER";
      this->panel_Sim_PLACEHOLDER->Size = System::Drawing::Size(400, 140);
      this->panel_Sim_PLACEHOLDER->TabIndex = 8;
      this->panel_Sim_PLACEHOLDER->Visible = false;
      // 
      // label_TxTimeDisplay
      // 
      this->label_TxTimeDisplay->Location = System::Drawing::Point(113, 156);
      this->label_TxTimeDisplay->Name = L"label_TxTimeDisplay";
      this->label_TxTimeDisplay->Size = System::Drawing::Size(103, 12);
      this->label_TxTimeDisplay->TabIndex = 7;
      this->label_TxTimeDisplay->Text = L"0";
      // 
      // checkBox_DisplayAsHex
      // 
      this->checkBox_DisplayAsHex->AutoSize = true;
      this->checkBox_DisplayAsHex->Checked = true;
      this->checkBox_DisplayAsHex->CheckState = System::Windows::Forms::CheckState::Checked;
      this->checkBox_DisplayAsHex->Location = System::Drawing::Point(125, 12);
      this->checkBox_DisplayAsHex->Name = L"checkBox_DisplayAsHex";
      this->checkBox_DisplayAsHex->Size = System::Drawing::Size(91, 17);
      this->checkBox_DisplayAsHex->TabIndex = 8;
      this->checkBox_DisplayAsHex->Text = L"Display in hex";
      this->checkBox_DisplayAsHex->UseVisualStyleBackColor = true;
      this->checkBox_DisplayAsHex->CheckedChanged += gcnew System::EventHandler(this, &ANTChannel::checkBox_DisplayAsHex_CheckedChanged);
      // 
      // richTextBox_RawTxDisplay
      // 
      this->richTextBox_RawTxDisplay->BackColor = System::Drawing::SystemColors::Window;
      this->richTextBox_RawTxDisplay->Font = (gcnew System::Drawing::Font(L"Lucida Console", 8.25F, System::Drawing::FontStyle::Regular, 
         System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
      this->richTextBox_RawTxDisplay->HideSelection = false;
      this->richTextBox_RawTxDisplay->Location = System::Drawing::Point(16, 30);
      this->richTextBox_RawTxDisplay->Name = L"richTextBox_RawTxDisplay";
      this->richTextBox_RawTxDisplay->ReadOnly = true;
      this->richTextBox_RawTxDisplay->Size = System::Drawing::Size(278, 122);
      this->richTextBox_RawTxDisplay->TabIndex = 0;
      this->richTextBox_RawTxDisplay->TabStop = false;
      this->richTextBox_RawTxDisplay->Text = L"Raw Data...";
      // 
      // panel_channelSettings
      // 
      this->panel_channelSettings->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
      this->panel_channelSettings->Controls->Add(this->label_TxPower);
      this->panel_channelSettings->Controls->Add(this->comboBox_TxPowerSelect);
      this->panel_channelSettings->Controls->Add(this->button_getChannelID);
      this->panel_channelSettings->Controls->Add(this->checkBox_PairingOn);
      this->panel_channelSettings->Controls->Add(this->label_ChannelConfig);
      this->panel_channelSettings->Controls->Add(this->label_TransmissionType);
      this->panel_channelSettings->Controls->Add(this->textBox_TransmissionType);
      this->panel_channelSettings->Controls->Add(this->label_TransmitPeriod);
      this->panel_channelSettings->Controls->Add(this->textBox_TransmitPeriod);
      this->panel_channelSettings->Controls->Add(this->label_DeviceType);
      this->panel_channelSettings->Controls->Add(this->textBox_DeviceType);
      this->panel_channelSettings->Controls->Add(this->label_DeviceId);
      this->panel_channelSettings->Controls->Add(this->textBox_DeviceID);
      this->panel_channelSettings->Location = System::Drawing::Point(322, 211);
      this->panel_channelSettings->Name = L"panel_channelSettings";
      this->panel_channelSettings->Size = System::Drawing::Size(400, 67);
      this->panel_channelSettings->TabIndex = 6;
      // 
      // label_TxPower
      // 
      this->label_TxPower->Location = System::Drawing::Point(333, 24);
      this->label_TxPower->Name = L"label_TxPower";
      this->label_TxPower->Size = System::Drawing::Size(52, 13);
      this->label_TxPower->TabIndex = 14;
      this->label_TxPower->Text = L"Tx Power";
      // 
      // comboBox_TxPowerSelect
      // 
      this->comboBox_TxPowerSelect->ForeColor = System::Drawing::SystemColors::WindowText;
      this->comboBox_TxPowerSelect->FormattingEnabled = true;
      this->comboBox_TxPowerSelect->Location = System::Drawing::Point(327, 40);
      this->comboBox_TxPowerSelect->Margin = System::Windows::Forms::Padding(3, 0, 3, 3);
      this->comboBox_TxPowerSelect->Name = L"comboBox_TxPowerSelect";
      this->comboBox_TxPowerSelect->Size = System::Drawing::Size(66, 21);
      this->comboBox_TxPowerSelect->TabIndex = 13;
      this->comboBox_TxPowerSelect->Text = L"Tx Power";
      this->comboBox_TxPowerSelect->SelectedIndexChanged += gcnew System::EventHandler(this, &ANTChannel::comboBox_TxPowerSelect_SelectedIndexChanged);
      // 
      // button_getChannelID
      // 
      this->button_getChannelID->Enabled = false;
      this->button_getChannelID->Location = System::Drawing::Point(267, 3);
      this->button_getChannelID->Name = L"button_getChannelID";
      this->button_getChannelID->Size = System::Drawing::Size(122, 19);
      this->button_getChannelID->TabIndex = 9;
      this->button_getChannelID->Text = L"Get Channel ID";
      this->button_getChannelID->UseVisualStyleBackColor = true;
      this->button_getChannelID->Visible = false;
      this->button_getChannelID->Click += gcnew System::EventHandler(this, &ANTChannel::button_getChannelID_Click);
      // 
      // checkBox_PairingOn
      // 
      this->checkBox_PairingOn->AutoSize = true;
      this->checkBox_PairingOn->Location = System::Drawing::Point(173, 5);
      this->checkBox_PairingOn->Name = L"checkBox_PairingOn";
      this->checkBox_PairingOn->Size = System::Drawing::Size(88, 17);
      this->checkBox_PairingOn->TabIndex = 7;
      this->checkBox_PairingOn->Text = L"Pairing Mode";
      this->checkBox_PairingOn->UseVisualStyleBackColor = true;
      this->checkBox_PairingOn->CheckedChanged += gcnew System::EventHandler(this, &ANTChannel::checkBox_PairingOn_CheckedChanged);
      // 
      // label_ChannelConfig
      // 
      this->label_ChannelConfig->AutoSize = true;
      this->label_ChannelConfig->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Underline, 
         System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
      this->label_ChannelConfig->Location = System::Drawing::Point(5, 5);
      this->label_ChannelConfig->Name = L"label_ChannelConfig";
      this->label_ChannelConfig->Size = System::Drawing::Size(139, 13);
      this->label_ChannelConfig->TabIndex = 8;
      this->label_ChannelConfig->Text = L"ANT Channel Configuration:";
      // 
      // label_TransmissionType
      // 
      this->label_TransmissionType->AutoSize = true;
      this->label_TransmissionType->Location = System::Drawing::Point(60, 24);
      this->label_TransmissionType->Name = L"label_TransmissionType";
      this->label_TransmissionType->Size = System::Drawing::Size(95, 13);
      this->label_TransmissionType->TabIndex = 5;
      this->label_TransmissionType->Text = L"Transmission Type";
      // 
      // textBox_TransmissionType
      // 
      this->textBox_TransmissionType->Location = System::Drawing::Point(80, 40);
      this->textBox_TransmissionType->MaxLength = 10;
      this->textBox_TransmissionType->Name = L"textBox_TransmissionType";
      this->textBox_TransmissionType->Size = System::Drawing::Size(49, 20);
      this->textBox_TransmissionType->TabIndex = 4;
      this->textBox_TransmissionType->Text = L"1";
      // 
      // label_TransmitPeriod
      // 
      this->label_TransmitPeriod->AutoSize = true;
      this->label_TransmitPeriod->Location = System::Drawing::Point(233, 24);
      this->label_TransmitPeriod->Name = L"label_TransmitPeriod";
      this->label_TransmitPeriod->Size = System::Drawing::Size(93, 13);
      this->label_TransmitPeriod->TabIndex = 7;
      this->label_TransmitPeriod->Text = L"Tx Period (counts)";
      // 
      // textBox_TransmitPeriod
      // 
      this->textBox_TransmitPeriod->Location = System::Drawing::Point(251, 40);
      this->textBox_TransmitPeriod->MaxLength = 10;
      this->textBox_TransmitPeriod->Name = L"textBox_TransmitPeriod";
      this->textBox_TransmitPeriod->Size = System::Drawing::Size(49, 20);
      this->textBox_TransmitPeriod->TabIndex = 6;
      this->textBox_TransmitPeriod->Text = L"8192";
      // 
      // label_DeviceType
      // 
      this->label_DeviceType->AutoSize = true;
      this->label_DeviceType->Location = System::Drawing::Point(160, 24);
      this->label_DeviceType->Name = L"label_DeviceType";
      this->label_DeviceType->Size = System::Drawing::Size(68, 13);
      this->label_DeviceType->TabIndex = 3;
      this->label_DeviceType->Text = L"Device Type";
      // 
      // textBox_DeviceType
      // 
      this->textBox_DeviceType->Location = System::Drawing::Point(166, 40);
      this->textBox_DeviceType->MaxLength = 10;
      this->textBox_DeviceType->Name = L"textBox_DeviceType";
      this->textBox_DeviceType->Size = System::Drawing::Size(49, 20);
      this->textBox_DeviceType->TabIndex = 5;
      this->textBox_DeviceType->Text = L"1";
      // 
      // label_DeviceId
      // 
      this->label_DeviceId->AutoSize = true;
      this->label_DeviceId->Location = System::Drawing::Point(6, 24);
      this->label_DeviceId->Name = L"label_DeviceId";
      this->label_DeviceId->Size = System::Drawing::Size(55, 13);
      this->label_DeviceId->TabIndex = 1;
      this->label_DeviceId->Text = L"Device ID";
      // 
      // textBox_DeviceID
      // 
      this->textBox_DeviceID->Location = System::Drawing::Point(9, 40);
      this->textBox_DeviceID->MaxLength = 10;
      this->textBox_DeviceID->Name = L"textBox_DeviceID";
      this->textBox_DeviceID->Size = System::Drawing::Size(49, 20);
      this->textBox_DeviceID->TabIndex = 3;
      this->textBox_DeviceID->Text = L"33";
      // 
      // checkBox_TxEnable
      // 
      this->checkBox_TxEnable->AutoSize = true;
      this->checkBox_TxEnable->Location = System::Drawing::Point(545, 14);
      this->checkBox_TxEnable->Name = L"checkBox_TxEnable";
      this->checkBox_TxEnable->Size = System::Drawing::Size(94, 17);
      this->checkBox_TxEnable->TabIndex = 2;
      this->checkBox_TxEnable->Text = L"Open Channel";
      this->checkBox_TxEnable->UseVisualStyleBackColor = true;
      this->checkBox_TxEnable->CheckedChanged += gcnew System::EventHandler(this, &ANTChannel::checkBox_TxEnable_CheckedChanged);
      // 
      // comboBox_SimSelect
      // 
      this->comboBox_SimSelect->FormattingEnabled = true;
      this->comboBox_SimSelect->Location = System::Drawing::Point(322, 10);
      this->comboBox_SimSelect->Name = L"comboBox_SimSelect";
      this->comboBox_SimSelect->Size = System::Drawing::Size(205, 21);
      this->comboBox_SimSelect->TabIndex = 1;
      this->comboBox_SimSelect->Text = L"Select Simulator...";
      this->comboBox_SimSelect->SelectedIndexChanged += gcnew System::EventHandler(this, &ANTChannel::comboBox_SimSelect_SelectedIndexChanged);
      // 
      // ANTChannel
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(830, 412);
      this->Controls->Add(this->tabControl1);
      this->Name = L"ANTChannel";
      this->Text = L"ANTChannel";
      this->tabControl1->ResumeLayout(false);
      this->tabPage_ChannelTab->ResumeLayout(false);
      this->tabPage_ChannelTab->PerformLayout();
      this->panel_channelSettings->ResumeLayout(false);
      this->panel_channelSettings->PerformLayout();
      this->ResumeLayout(false);

   }
#pragma endregion

};
