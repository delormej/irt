/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/


/* The ANT+ simulator was created to aid with the development and testing of ANT+ sensors and display devices.
This is the main class, it controls opening the ANT library, connecting the ANT devices, 
creating the appropriate number of ANTChannel classes, adding the channel controls to the form and 
handling and forwarding appropriate ANT messages to the respective channels.
*/


#pragma once

#include <stdio.h>
#include "stdafx.h"
#include "config.h"
#include "types.h"
#include "antclass.h"
#include "ANTChannel.h"
#include "AboutSim.h"


void UnmanagedSender(UCHAR ucChannel_, UCHAR ucMessageCode_, UCHAR* pucBuffer_);
void UnmanagedEventSender(UCHAR ucChannel_, UCHAR ucMessageCode_, UCHAR* pucBuffer_);


namespace ANTPlusSim {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	struct capabilityDefs{
		UCHAR ucBitMask;
		char* flagDefinition;
	};

	const short int NUM_BASIC_CAPS =6;
	const short int NUM_EXTD_CAPS1 =6;
	const short int NUM_EXTD_CAPS2 =7;

	/// <summary>
	/// Summary for MainForm
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class MainForm : public System::Windows::Forms::Form
	{
	public:
		MainForm(UCHAR ucSimType_);
		
		~MainForm(); 
            
      void ProcessChannelEvent(UCHAR ucChannel_, UCHAR ucMessageCode_, UCHAR* pucBuffer_);
      void ProcessProtocolEvent(UCHAR ucChannel_, UCHAR ucMessageCode_, UCHAR* pucBuffer_);
      
   private:
		array<ANTChannel^> ^channelList;
		int numChannels;
		System::String^ strTitle;
		System::String^ strAbout;
		System::String^ strLibVersion;
		UCHAR ucSimType;
		BOOL bMyArct;
		BOOL bMyTimerStarted;
		BOOL bMyConnected;
      ANTPlatform eMyANTPlatform;
      CapabilitiesStruct sMyCapabilities;


	private: System::Windows::Forms::Panel^ panel_devicePanel_1;
	private: System::Windows::Forms::RichTextBox^ richTextBox_deviceReport_1;	
	private: System::Windows::Forms::Button^  button_Connect;
	private: System::Windows::Forms::Label^  label_DeviceProfile;
	private: System::Windows::Forms::ComboBox^  comboBox_DeviceSelect;
	private: System::Windows::Forms::FlowLayoutPanel^  flowLayoutPanel_ActiveDeviceList;
	private: System::Windows::Forms::TabControl^  tabControl_Channels;
	private: System::Windows::Forms::RichTextBox^  richTextBox_MainStatus;
	private: System::Windows::Forms::Label^  label_ConnectedDevices;
	private: System::Windows::Forms::Label^  label_USBPort;
	private: System::Windows::Forms::NumericUpDown^  numericUpDown_USBSelect;
	private: System::Windows::Forms::Timer^  timerGeneric;
   private: System::Windows::Forms::TextBox^  textBox_baudRate;

   private: System::Windows::Forms::Label^  label1;
   private: System::Windows::Forms::LinkLabel^  linkLabel_About;

	private:
		/// <summary>
      void CreateDevicePanel();
      void RemoveDevicePanel();
      void InterpretCapabilities(UCHAR* pcBuffer_);      
      void CreateChannels();
      void Connect();     
      
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::IContainer^  components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
         this->components = (gcnew System::ComponentModel::Container());
         System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(MainForm::typeid));
         this->button_Connect = (gcnew System::Windows::Forms::Button());
         this->label_DeviceProfile = (gcnew System::Windows::Forms::Label());
         this->comboBox_DeviceSelect = (gcnew System::Windows::Forms::ComboBox());
         this->flowLayoutPanel_ActiveDeviceList = (gcnew System::Windows::Forms::FlowLayoutPanel());
         this->tabControl_Channels = (gcnew System::Windows::Forms::TabControl());
         this->richTextBox_MainStatus = (gcnew System::Windows::Forms::RichTextBox());
         this->label_ConnectedDevices = (gcnew System::Windows::Forms::Label());
         this->label_USBPort = (gcnew System::Windows::Forms::Label());
         this->numericUpDown_USBSelect = (gcnew System::Windows::Forms::NumericUpDown());
         this->linkLabel_About = (gcnew System::Windows::Forms::LinkLabel());
         this->timerGeneric = (gcnew System::Windows::Forms::Timer(this->components));
         this->textBox_baudRate = (gcnew System::Windows::Forms::TextBox());
         this->label1 = (gcnew System::Windows::Forms::Label());
         (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_USBSelect))->BeginInit();
         this->SuspendLayout();
         // 
         // button_Connect
         // 
         this->button_Connect->Location = System::Drawing::Point(243, 12);
         this->button_Connect->Name = L"button_Connect";
         this->button_Connect->Size = System::Drawing::Size(74, 23);
         this->button_Connect->TabIndex = 1;
         this->button_Connect->Text = L"Connect";
         this->button_Connect->UseVisualStyleBackColor = true;
         this->button_Connect->Click += gcnew System::EventHandler(this, &MainForm::button_Connect_Click);
         // 
         // label_DeviceProfile
         // 
         this->label_DeviceProfile->AutoSize = true;
         this->label_DeviceProfile->Location = System::Drawing::Point(42, 15);
         this->label_DeviceProfile->Name = L"label_DeviceProfile";
         this->label_DeviceProfile->Size = System::Drawing::Size(44, 13);
         this->label_DeviceProfile->TabIndex = 1;
         this->label_DeviceProfile->Text = L"Device:";
         // 
         // comboBox_DeviceSelect
         // 
         this->comboBox_DeviceSelect->FormattingEnabled = true;
         this->comboBox_DeviceSelect->ImeMode = System::Windows::Forms::ImeMode::Off;
         this->comboBox_DeviceSelect->Items->AddRange(gcnew cli::array< System::Object^  >(4) {L"AP2/USB2/AT3", L"C7", L"AP1/USB1", 
            L"Custom USB"});
         this->comboBox_DeviceSelect->Location = System::Drawing::Point(92, 12);
         this->comboBox_DeviceSelect->Name = L"comboBox_DeviceSelect";
         this->comboBox_DeviceSelect->Size = System::Drawing::Size(145, 21);
         this->comboBox_DeviceSelect->TabIndex = 2;
         this->comboBox_DeviceSelect->Text = L"AP2/USB2/AT3";
         this->comboBox_DeviceSelect->SelectedValueChanged += gcnew System::EventHandler(this, &MainForm::comboBox_DeviceSelect_SelectedValueChanged);
         // 
         // flowLayoutPanel_ActiveDeviceList
         // 
         this->flowLayoutPanel_ActiveDeviceList->AutoScroll = true;
         this->flowLayoutPanel_ActiveDeviceList->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
         this->flowLayoutPanel_ActiveDeviceList->Location = System::Drawing::Point(377, 26);
         this->flowLayoutPanel_ActiveDeviceList->Name = L"flowLayoutPanel_ActiveDeviceList";
         this->flowLayoutPanel_ActiveDeviceList->Size = System::Drawing::Size(408, 127);
         this->flowLayoutPanel_ActiveDeviceList->TabIndex = 4;
         // 
         // tabControl_Channels
         // 
         this->tabControl_Channels->Location = System::Drawing::Point(45, 170);
         this->tabControl_Channels->Name = L"tabControl_Channels";
         this->tabControl_Channels->SelectedIndex = 0;
         this->tabControl_Channels->Size = System::Drawing::Size(740, 325);
         this->tabControl_Channels->TabIndex = 5;
         this->tabControl_Channels->TabStop = false;
         // 
         // richTextBox_MainStatus
         // 
         this->richTextBox_MainStatus->BackColor = System::Drawing::SystemColors::Window;
         this->richTextBox_MainStatus->HideSelection = false;
         this->richTextBox_MainStatus->Location = System::Drawing::Point(45, 63);
         this->richTextBox_MainStatus->Name = L"richTextBox_MainStatus";
         this->richTextBox_MainStatus->ReadOnly = true;
         this->richTextBox_MainStatus->Size = System::Drawing::Size(272, 90);
         this->richTextBox_MainStatus->TabIndex = 7;
         this->richTextBox_MainStatus->TabStop = false;
         this->richTextBox_MainStatus->Text = L"Select device type and press connect to begin...";
         // 
         // label_ConnectedDevices
         // 
         this->label_ConnectedDevices->AutoSize = true;
         this->label_ConnectedDevices->Location = System::Drawing::Point(374, 6);
         this->label_ConnectedDevices->Name = L"label_ConnectedDevices";
         this->label_ConnectedDevices->Size = System::Drawing::Size(104, 13);
         this->label_ConnectedDevices->TabIndex = 8;
         this->label_ConnectedDevices->Text = L"Connected Devices:";
         // 
         // label_USBPort
         // 
         this->label_USBPort->AutoSize = true;
         this->label_USBPort->Location = System::Drawing::Point(42, 42);
         this->label_USBPort->Name = L"label_USBPort";
         this->label_USBPort->Size = System::Drawing::Size(69, 13);
         this->label_USBPort->TabIndex = 9;
         this->label_USBPort->Text = L"on USB Port:";
         // 
         // numericUpDown_USBSelect
         // 
         this->numericUpDown_USBSelect->Location = System::Drawing::Point(117, 40);
         this->numericUpDown_USBSelect->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
         this->numericUpDown_USBSelect->Name = L"numericUpDown_USBSelect";
         this->numericUpDown_USBSelect->Size = System::Drawing::Size(50, 20);
         this->numericUpDown_USBSelect->TabIndex = 3;
         // 
         // linkLabel_About
         // 
         this->linkLabel_About->AutoSize = true;
         this->linkLabel_About->Location = System::Drawing::Point(750, 6);
         this->linkLabel_About->Name = L"linkLabel_About";
         this->linkLabel_About->Size = System::Drawing::Size(35, 13);
         this->linkLabel_About->TabIndex = 10;
         this->linkLabel_About->TabStop = true;
         this->linkLabel_About->Text = L"About";
         this->linkLabel_About->LinkClicked += gcnew System::Windows::Forms::LinkLabelLinkClickedEventHandler(this, &MainForm::linkLabel_About_LinkClicked);
         // 
         // textBox_baudRate
         // 
         this->textBox_baudRate->Location = System::Drawing::Point(226, 39);
         this->textBox_baudRate->Name = L"textBox_baudRate";
         this->textBox_baudRate->ReadOnly = true;
         this->textBox_baudRate->Size = System::Drawing::Size(68, 20);
         this->textBox_baudRate->TabIndex = 11;
         this->textBox_baudRate->Text = L"57600";
         // 
         // label1
         // 
         this->label1->AutoSize = true;
         this->label1->Location = System::Drawing::Point(173, 42);
         this->label1->Name = L"label1";
         this->label1->Size = System::Drawing::Size(47, 13);
         this->label1->TabIndex = 12;
         this->label1->Text = L"at Baud:";
         // 
         // MainForm
         // 
         this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
         this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
         this->ClientSize = System::Drawing::Size(816, 525);
         this->Controls->Add(this->label1);
         this->Controls->Add(this->textBox_baudRate);
         this->Controls->Add(this->linkLabel_About);
         this->Controls->Add(this->numericUpDown_USBSelect);
         this->Controls->Add(this->label_USBPort);
         this->Controls->Add(this->label_ConnectedDevices);
         this->Controls->Add(this->richTextBox_MainStatus);
         this->Controls->Add(this->tabControl_Channels);
         this->Controls->Add(this->flowLayoutPanel_ActiveDeviceList);
         this->Controls->Add(this->comboBox_DeviceSelect);
         this->Controls->Add(this->label_DeviceProfile);
         this->Controls->Add(this->button_Connect);
         this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
         this->Name = L"MainForm";
         this->Text = L"ANT+ Simulator";
         (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_USBSelect))->EndInit();
         this->ResumeLayout(false);
         this->PerformLayout();

      }
#pragma endregion

/**************************************************************************
 * System::Void MainForm_FormClosing
 * 
 * Disconnect from ANT when closing the application
 * 
 * returns: N/A
 *
 **************************************************************************/
private: System::Void MainForm_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
		if(bMyConnected)
      {
			ANTClass::Disconnect(); 
		}
   }

/**************************************************************************
 * System::Void button_Connect_Click
 * 
 * Connect/disconnect to ANT 
 * 
 * returns: N/A
 *
 **************************************************************************/
private: System::Void button_Connect_Click(System::Object^  sender, System::EventArgs^  e) 
{
   Connect();
}


/**************************************************************************
 * linkLabel_About_LinkClicked
 * 
 * Displays simulator "About" box
 * 
 * returns: N/A
 *
 **************************************************************************/
private: System::Void linkLabel_About_LinkClicked(System::Object^  sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^  e) {
			 AboutSim^ formAbout = gcnew AboutSim(strAbout, strLibVersion);
			 formAbout->Show();
			 
		 }
private: System::Void comboBox_DeviceSelect_SelectedValueChanged(System::Object^  sender, System::EventArgs^  e) {
            if(comboBox_DeviceSelect->SelectedItem->Equals("Custom USB"))
            {
               this->textBox_baudRate->Text = "57600";
               textBox_baudRate->ReadOnly = false;
            }
            else if(comboBox_DeviceSelect->SelectedItem->Equals("AP2/USB2/AT3") || comboBox_DeviceSelect->SelectedItem->Equals("C7"))
            {
               this->textBox_baudRate->Text = "57600";			   
               textBox_baudRate->ReadOnly = true;
            }
            else if(comboBox_DeviceSelect->SelectedItem->Equals("AP1/USB1"))
            {
               this->textBox_baudRate->Text = "50000";			   
               textBox_baudRate->ReadOnly = true;
            }
            else
            {
               this->textBox_baudRate->Text = "?";
               textBox_baudRate->ReadOnly = true;
            }
         }
};
}

