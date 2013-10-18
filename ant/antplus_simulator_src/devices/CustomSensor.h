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
#include "custom.h"
#include "types.h"
#include "antdefines.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


public ref class CustomSensor : public System::Windows::Forms::Form, public ISimBase{
	public:
		CustomSensor(System::Timers::Timer^ channelTimer, dRequestAckMsg^ channelAckMsg){
			InitializeComponent();
			arrayUserData = gcnew array<UCHAR>(8);
			InitializeSim();
			requestAckMsg = channelAckMsg;
		}

		~CustomSensor(){
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
	virtual UCHAR getDeviceType(){return CUSTOM_DEVICE_TYPE;}
	virtual UCHAR getTransmissionType(){return CUSTOM_TX_TYPE;}
	virtual USHORT getTransmitPeriod(){return CUSTOM_MSG_PERIOD;}
	virtual DOUBLE getTimerInterval(){return 1000;} // One second
	virtual void onTimerTock(USHORT eventTime); 
	virtual System::Windows::Forms::Panel^ getSimSettingsPanel(){return this->panel_Settings;}
	virtual System::Windows::Forms::Panel^ getSimTranslatedDisplay(){return this->panel_Display;}
		
private:
	void InitializeSim();
	void HandleTransmit(UCHAR* pucTxBuffer_);
	void ValidateInput();
	System::Void button_UpdateData_Click(System::Object^  sender, System::EventArgs^  e);
	System::Void radioButton_TranslateSelect_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void button_SendAck_Click(System::Object^  sender, System::EventArgs^  e);

private:
	dRequestAckMsg^ requestAckMsg;		// Handle to delegate function handling transmission of acknowledged messages
	ULONG ulEventCounter;				// Number of events		
	BOOL bSendHex;						// Text box interpreted as Hex or characters
	array<UCHAR,1>^ arrayUserData;		// Array to hold user data for transmission

private: System::Windows::Forms::Panel^  panel_Settings;
private: System::Windows::Forms::TextBox^  textBox_customTxData;
private: System::Windows::Forms::Label^  label_customTxData;
private: System::Windows::Forms::RadioButton^  radioButton_asChar;
private: System::Windows::Forms::RadioButton^  radioButton_asHex;
private: System::Windows::Forms::Label^  label_inputFormat;
private: System::Windows::Forms::Label^  label_TxEventCount;
private: System::Windows::Forms::Label^  label_TxData;
private: System::Windows::Forms::Label^  label_DataTxd;
private: System::Windows::Forms::Label^  label_EventCount;
private: System::Windows::Forms::Panel^  panel_Display;
private: System::Windows::Forms::Label^  label_InputError;
private: System::Windows::Forms::Button^  button_SendAck;
private: System::Windows::Forms::Label^  label_Trans_Title;
private: System::Windows::Forms::Button^  button_UpdateData;

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
			this->button_SendAck = (gcnew System::Windows::Forms::Button());
			this->label_InputError = (gcnew System::Windows::Forms::Label());
			this->label_inputFormat = (gcnew System::Windows::Forms::Label());
			this->radioButton_asChar = (gcnew System::Windows::Forms::RadioButton());
			this->radioButton_asHex = (gcnew System::Windows::Forms::RadioButton());
			this->textBox_customTxData = (gcnew System::Windows::Forms::TextBox());
			this->label_customTxData = (gcnew System::Windows::Forms::Label());
			this->panel_Display = (gcnew System::Windows::Forms::Panel());
			this->label_Trans_Title = (gcnew System::Windows::Forms::Label());
			this->label_DataTxd = (gcnew System::Windows::Forms::Label());
			this->label_EventCount = (gcnew System::Windows::Forms::Label());
			this->label_TxData = (gcnew System::Windows::Forms::Label());
			this->label_TxEventCount = (gcnew System::Windows::Forms::Label());
			this->button_UpdateData = (gcnew System::Windows::Forms::Button());
			this->panel_Settings->SuspendLayout();
			this->panel_Display->SuspendLayout();
			this->SuspendLayout();
			// 
			// panel_Settings
			// 
			this->panel_Settings->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->panel_Settings->Controls->Add(this->button_UpdateData);
			this->panel_Settings->Controls->Add(this->button_SendAck);
			this->panel_Settings->Controls->Add(this->label_InputError);
			this->panel_Settings->Controls->Add(this->label_inputFormat);
			this->panel_Settings->Controls->Add(this->radioButton_asChar);
			this->panel_Settings->Controls->Add(this->radioButton_asHex);
			this->panel_Settings->Controls->Add(this->textBox_customTxData);
			this->panel_Settings->Controls->Add(this->label_customTxData);
			this->panel_Settings->Location = System::Drawing::Point(322, 50);
			this->panel_Settings->Name = L"panel_Settings";
			this->panel_Settings->Size = System::Drawing::Size(400, 140);
			this->panel_Settings->TabIndex = 0;
			// 
			// button_SendAck
			// 
			this->button_SendAck->Location = System::Drawing::Point(14, 98);
			this->button_SendAck->Name = L"button_SendAck";
			this->button_SendAck->Size = System::Drawing::Size(120, 23);
			this->button_SendAck->TabIndex = 14;
			this->button_SendAck->Text = L"Send Acknowledged";
			this->button_SendAck->UseVisualStyleBackColor = true;
			this->button_SendAck->Click += gcnew System::EventHandler(this, &CustomSensor::button_SendAck_Click);
			// 
			// label_InputError
			// 
			this->label_InputError->AutoSize = true;
			this->label_InputError->Location = System::Drawing::Point(47, 82);
			this->label_InputError->Name = L"label_InputError";
			this->label_InputError->Size = System::Drawing::Size(0, 13);
			this->label_InputError->TabIndex = 13;
			// 
			// label_inputFormat
			// 
			this->label_inputFormat->AutoSize = true;
			this->label_inputFormat->Location = System::Drawing::Point(160, 39);
			this->label_inputFormat->Name = L"label_inputFormat";
			this->label_inputFormat->Size = System::Drawing::Size(90, 13);
			this->label_inputFormat->TabIndex = 12;
			this->label_inputFormat->Text = L"Interpret Data As:";
			// 
			// radioButton_asChar
			// 
			this->radioButton_asChar->AutoSize = true;
			this->radioButton_asChar->Checked = true;
			this->radioButton_asChar->Location = System::Drawing::Point(261, 37);
			this->radioButton_asChar->Name = L"radioButton_asChar";
			this->radioButton_asChar->Size = System::Drawing::Size(101, 17);
			this->radioButton_asChar->TabIndex = 12;
			this->radioButton_asChar->TabStop = true;
			this->radioButton_asChar->Text = L"Character String";
			this->radioButton_asChar->UseVisualStyleBackColor = true;
			this->radioButton_asChar->CheckedChanged += gcnew System::EventHandler(this, &CustomSensor::radioButton_TranslateSelect_CheckedChanged);
			// 
			// radioButton_asHex
			// 
			this->radioButton_asHex->AutoSize = true;
			this->radioButton_asHex->Location = System::Drawing::Point(261, 60);
			this->radioButton_asHex->Name = L"radioButton_asHex";
			this->radioButton_asHex->Size = System::Drawing::Size(130, 30);
			this->radioButton_asHex->TabIndex = 13;
			this->radioButton_asHex->Text = L"Hex Couplets\nSeparated by commas";
			this->radioButton_asHex->UseVisualStyleBackColor = true;
			// 
			// textBox_customTxData
			// 
			this->textBox_customTxData->Location = System::Drawing::Point(10, 36);
			this->textBox_customTxData->MaxLength = 8;
			this->textBox_customTxData->Name = L"textBox_customTxData";
			this->textBox_customTxData->Size = System::Drawing::Size(130, 20);
			this->textBox_customTxData->TabIndex = 10;
			this->textBox_customTxData->Text = L"hi/68,69";
			// 
			// label_customTxData
			// 
			this->label_customTxData->AutoSize = true;
			this->label_customTxData->Location = System::Drawing::Point(7, 20);
			this->label_customTxData->Name = L"label_customTxData";
			this->label_customTxData->Size = System::Drawing::Size(88, 13);
			this->label_customTxData->TabIndex = 1;
			this->label_customTxData->Text = L"Data to Transmit:";
			// 
			// panel_Display
			// 
			this->panel_Display->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->panel_Display->Controls->Add(this->label_Trans_Title);
			this->panel_Display->Controls->Add(this->label_DataTxd);
			this->panel_Display->Controls->Add(this->label_EventCount);
			this->panel_Display->Controls->Add(this->label_TxData);
			this->panel_Display->Controls->Add(this->label_TxEventCount);
			this->panel_Display->Location = System::Drawing::Point(58, 188);
			this->panel_Display->Name = L"panel_Display";
			this->panel_Display->Size = System::Drawing::Size(200, 90);
			this->panel_Display->TabIndex = 1;
			// 
			// label_Trans_Title
			// 
			this->label_Trans_Title->AutoSize = true;
			this->label_Trans_Title->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Underline, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->label_Trans_Title->Location = System::Drawing::Point(2, 2);
			this->label_Trans_Title->Name = L"label_Trans_Title";
			this->label_Trans_Title->Size = System::Drawing::Size(92, 13);
			this->label_Trans_Title->TabIndex = 4;
			this->label_Trans_Title->Text = L"Transmission Info:";
			// 
			// label_DataTxd
			// 
			this->label_DataTxd->AutoSize = true;
			this->label_DataTxd->Location = System::Drawing::Point(130, 50);
			this->label_DataTxd->Name = L"label_DataTxd";
			this->label_DataTxd->Size = System::Drawing::Size(16, 13);
			this->label_DataTxd->TabIndex = 3;
			this->label_DataTxd->Text = L"---";
			this->label_DataTxd->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label_EventCount
			// 
			this->label_EventCount->AutoSize = true;
			this->label_EventCount->Location = System::Drawing::Point(130, 27);
			this->label_EventCount->Name = L"label_EventCount";
			this->label_EventCount->Size = System::Drawing::Size(16, 13);
			this->label_EventCount->TabIndex = 2;
			this->label_EventCount->Text = L"---";
			this->label_EventCount->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label_TxData
			// 
			this->label_TxData->AutoSize = true;
			this->label_TxData->Location = System::Drawing::Point(57, 50);
			this->label_TxData->Name = L"label_TxData";
			this->label_TxData->Size = System::Drawing::Size(67, 13);
			this->label_TxData->TabIndex = 1;
			this->label_TxData->Text = L"Transmitting:";
			this->label_TxData->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// label_TxEventCount
			// 
			this->label_TxEventCount->AutoSize = true;
			this->label_TxEventCount->Location = System::Drawing::Point(12, 27);
			this->label_TxEventCount->Name = L"label_TxEventCount";
			this->label_TxEventCount->Size = System::Drawing::Size(112, 13);
			this->label_TxEventCount->TabIndex = 0;
			this->label_TxEventCount->Text = L"Transmit Event Count:";
			this->label_TxEventCount->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// button_UpdateData
			// 
			this->button_UpdateData->Location = System::Drawing::Point(14, 64);
			this->button_UpdateData->Name = L"button_UpdateData";
			this->button_UpdateData->Size = System::Drawing::Size(120, 23);
			this->button_UpdateData->TabIndex = 15;
			this->button_UpdateData->Text = L"Update Data";
			this->button_UpdateData->UseVisualStyleBackColor = true;
			this->button_UpdateData->Click += gcnew System::EventHandler(this, &CustomSensor::button_UpdateData_Click);
			// 
			// CustomSensor
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(786, 317);
			this->Controls->Add(this->panel_Display);
			this->Controls->Add(this->panel_Settings);
			this->Name = L"CustomSensor";
			this->Text = L"CustomSensor";
			this->panel_Settings->ResumeLayout(false);
			this->panel_Settings->PerformLayout();
			this->panel_Display->ResumeLayout(false);
			this->panel_Display->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion

};