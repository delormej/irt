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

public ref class CustomDisplay : public System::Windows::Forms::Form, public ISimBase{
	public:
		CustomDisplay(dRequestAckMsg^ channelAckMsg){
			InitializeComponent();
			InitializeSim();
			requestAckMsg = channelAckMsg;
		}

		~CustomDisplay(){
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
	virtual DOUBLE getTimerInterval(){return 3600000;} // Set interval to one hour, so timer events are not frequent (timer should be disabled tho)
	virtual void onTimerTock(USHORT eventTime){} // Do nothing
	virtual System::Windows::Forms::Panel^ getSimSettingsPanel(){return this->panel_Settings;}
	virtual System::Windows::Forms::Panel^ getSimTranslatedDisplay(){return this->panel_Display;}
			
private:
	void InitializeSim();
	void HandleReceive(UCHAR* pucRxBuffer_);
	System::Void radioButton_TranslateSelect_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	System::Void button_SendAck_Click(System::Object^  sender, System::EventArgs^  e);

private:
	dRequestAckMsg^ requestAckMsg;		// Handle to delegate function handling transmission of acknowledged messages
	ULONG ulEventCounter;				// Number of events		
	BOOL bSendHex;						// Text box interpreted as Hex or characters

private: System::Windows::Forms::Panel^  panel_Settings;
private: System::Windows::Forms::TextBox^  textBox_customTxData;
private: System::Windows::Forms::Label^  label_customTxData;
private: System::Windows::Forms::RadioButton^  radioButton_asChar;
private: System::Windows::Forms::RadioButton^  radioButton_asHex;
private: System::Windows::Forms::Label^  label_TxEventCount;
private: System::Windows::Forms::Label^  label_MsgCountDisplay;
private: System::Windows::Forms::Panel^  panel_Display;
private: System::Windows::Forms::Button^  button_SendAck;
private: System::Windows::Forms::Label^  label_Trans_Title;
private: System::Windows::Forms::GroupBox^  groupBox4;
private: System::Windows::Forms::GroupBox^  groupBox3;
private: System::Windows::Forms::GroupBox^  groupBox2;
private: System::Windows::Forms::GroupBox^  groupBox1;
private: System::Windows::Forms::Label^  label_Calc_AsInt;
private: System::Windows::Forms::Label^  label_Calc_AsChar;
private: System::Windows::Forms::Label^  label_Calc_AsHex;


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
			this->groupBox4 = (gcnew System::Windows::Forms::GroupBox());
			this->label_Calc_AsInt = (gcnew System::Windows::Forms::Label());
			this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
			this->label_Calc_AsChar = (gcnew System::Windows::Forms::Label());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->label_Calc_AsHex = (gcnew System::Windows::Forms::Label());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->button_SendAck = (gcnew System::Windows::Forms::Button());
			this->label_customTxData = (gcnew System::Windows::Forms::Label());
			this->radioButton_asHex = (gcnew System::Windows::Forms::RadioButton());
			this->radioButton_asChar = (gcnew System::Windows::Forms::RadioButton());
			this->textBox_customTxData = (gcnew System::Windows::Forms::TextBox());
			this->panel_Display = (gcnew System::Windows::Forms::Panel());
			this->label_Trans_Title = (gcnew System::Windows::Forms::Label());
			this->label_MsgCountDisplay = (gcnew System::Windows::Forms::Label());
			this->label_TxEventCount = (gcnew System::Windows::Forms::Label());
			this->panel_Settings->SuspendLayout();
			this->groupBox4->SuspendLayout();
			this->groupBox3->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->groupBox1->SuspendLayout();
			this->panel_Display->SuspendLayout();
			this->SuspendLayout();
			// 
			// panel_Settings
			// 
			this->panel_Settings->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->panel_Settings->Controls->Add(this->groupBox4);
			this->panel_Settings->Controls->Add(this->groupBox3);
			this->panel_Settings->Controls->Add(this->groupBox2);
			this->panel_Settings->Controls->Add(this->groupBox1);
			this->panel_Settings->Location = System::Drawing::Point(322, 50);
			this->panel_Settings->Name = L"panel_Settings";
			this->panel_Settings->Size = System::Drawing::Size(400, 140);
			this->panel_Settings->TabIndex = 0;
			// 
			// groupBox4
			// 
			this->groupBox4->Controls->Add(this->label_Calc_AsInt);
			this->groupBox4->Location = System::Drawing::Point(3, 94);
			this->groupBox4->Name = L"groupBox4";
			this->groupBox4->Size = System::Drawing::Size(237, 40);
			this->groupBox4->TabIndex = 15;
			this->groupBox4->TabStop = false;
			this->groupBox4->Text = L"Message Received as Decimal";
			// 
			// label_Calc_AsInt
			// 
			this->label_Calc_AsInt->AutoSize = true;
			this->label_Calc_AsInt->Location = System::Drawing::Point(9, 18);
			this->label_Calc_AsInt->Name = L"label_Calc_AsInt";
			this->label_Calc_AsInt->Size = System::Drawing::Size(16, 13);
			this->label_Calc_AsInt->TabIndex = 2;
			this->label_Calc_AsInt->Text = L"---";
			// 
			// groupBox3
			// 
			this->groupBox3->Controls->Add(this->label_Calc_AsChar);
			this->groupBox3->Location = System::Drawing::Point(3, 48);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(237, 40);
			this->groupBox3->TabIndex = 15;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = L"Message Received as Characters";
			// 
			// label_Calc_AsChar
			// 
			this->label_Calc_AsChar->AutoSize = true;
			this->label_Calc_AsChar->Location = System::Drawing::Point(9, 18);
			this->label_Calc_AsChar->Name = L"label_Calc_AsChar";
			this->label_Calc_AsChar->Size = System::Drawing::Size(16, 13);
			this->label_Calc_AsChar->TabIndex = 1;
			this->label_Calc_AsChar->Text = L"---";
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->label_Calc_AsHex);
			this->groupBox2->Location = System::Drawing::Point(3, 2);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(237, 40);
			this->groupBox2->TabIndex = 14;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Message Received as Hex";
			// 
			// label_Calc_AsHex
			// 
			this->label_Calc_AsHex->AutoSize = true;
			this->label_Calc_AsHex->Location = System::Drawing::Point(9, 18);
			this->label_Calc_AsHex->Name = L"label_Calc_AsHex";
			this->label_Calc_AsHex->Size = System::Drawing::Size(16, 13);
			this->label_Calc_AsHex->TabIndex = 0;
			this->label_Calc_AsHex->Text = L"---";
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->button_SendAck);
			this->groupBox1->Controls->Add(this->label_customTxData);
			this->groupBox1->Controls->Add(this->radioButton_asHex);
			this->groupBox1->Controls->Add(this->radioButton_asChar);
			this->groupBox1->Controls->Add(this->textBox_customTxData);
			this->groupBox1->Location = System::Drawing::Point(245, 2);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(148, 133);
			this->groupBox1->TabIndex = 2;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Controlled Messaging";
			// 
			// button_SendAck
			// 
			this->button_SendAck->Location = System::Drawing::Point(14, 107);
			this->button_SendAck->Name = L"button_SendAck";
			this->button_SendAck->Size = System::Drawing::Size(120, 23);
			this->button_SendAck->TabIndex = 14;
			this->button_SendAck->Text = L"Send Acknowledged";
			this->button_SendAck->UseVisualStyleBackColor = true;
			this->button_SendAck->Click += gcnew System::EventHandler(this, &CustomDisplay::button_SendAck_Click);
			// 
			// label_customTxData
			// 
			this->label_customTxData->AutoSize = true;
			this->label_customTxData->Location = System::Drawing::Point(6, 15);
			this->label_customTxData->Name = L"label_customTxData";
			this->label_customTxData->Size = System::Drawing::Size(88, 13);
			this->label_customTxData->TabIndex = 1;
			this->label_customTxData->Text = L"Data to Transmit:";
			// 
			// radioButton_asHex
			// 
			this->radioButton_asHex->AutoSize = true;
			this->radioButton_asHex->Location = System::Drawing::Point(9, 74);
			this->radioButton_asHex->Name = L"radioButton_asHex";
			this->radioButton_asHex->Size = System::Drawing::Size(130, 30);
			this->radioButton_asHex->TabIndex = 13;
			this->radioButton_asHex->Text = L"Hex Couplets\nSeparated by commas";
			this->radioButton_asHex->UseVisualStyleBackColor = true;
			// 
			// radioButton_asChar
			// 
			this->radioButton_asChar->AutoSize = true;
			this->radioButton_asChar->Checked = true;
			this->radioButton_asChar->Location = System::Drawing::Point(9, 56);
			this->radioButton_asChar->Name = L"radioButton_asChar";
			this->radioButton_asChar->Size = System::Drawing::Size(101, 17);
			this->radioButton_asChar->TabIndex = 12;
			this->radioButton_asChar->TabStop = true;
			this->radioButton_asChar->Text = L"Character String";
			this->radioButton_asChar->UseVisualStyleBackColor = true;
			this->radioButton_asChar->CheckedChanged += gcnew System::EventHandler(this, &CustomDisplay::radioButton_TranslateSelect_CheckedChanged);
			// 
			// textBox_customTxData
			// 
			this->textBox_customTxData->Location = System::Drawing::Point(9, 32);
			this->textBox_customTxData->MaxLength = 8;
			this->textBox_customTxData->Name = L"textBox_customTxData";
			this->textBox_customTxData->Size = System::Drawing::Size(130, 20);
			this->textBox_customTxData->TabIndex = 10;
			this->textBox_customTxData->Text = L"hi/68,69";
			// 
			// panel_Display
			// 
			this->panel_Display->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->panel_Display->Controls->Add(this->label_Trans_Title);
			this->panel_Display->Controls->Add(this->label_MsgCountDisplay);
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
			// label_MsgCountDisplay
			// 
			this->label_MsgCountDisplay->AutoSize = true;
			this->label_MsgCountDisplay->Location = System::Drawing::Point(123, 39);
			this->label_MsgCountDisplay->Name = L"label_MsgCountDisplay";
			this->label_MsgCountDisplay->Size = System::Drawing::Size(16, 13);
			this->label_MsgCountDisplay->TabIndex = 2;
			this->label_MsgCountDisplay->Text = L"---";
			this->label_MsgCountDisplay->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label_TxEventCount
			// 
			this->label_TxEventCount->AutoSize = true;
			this->label_TxEventCount->Location = System::Drawing::Point(28, 39);
			this->label_TxEventCount->Name = L"label_TxEventCount";
			this->label_TxEventCount->Size = System::Drawing::Size(89, 13);
			this->label_TxEventCount->TabIndex = 0;
			this->label_TxEventCount->Text = L"# Msg Received:";
			this->label_TxEventCount->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// CustomDisplay
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(786, 317);
			this->Controls->Add(this->panel_Display);
			this->Controls->Add(this->panel_Settings);
			this->Name = L"CustomDisplay";
			this->Text = L"CustomDisplay";
			this->panel_Settings->ResumeLayout(false);
			this->groupBox4->ResumeLayout(false);
			this->groupBox4->PerformLayout();
			this->groupBox3->ResumeLayout(false);
			this->groupBox3->PerformLayout();
			this->groupBox2->ResumeLayout(false);
			this->groupBox2->PerformLayout();
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->panel_Display->ResumeLayout(false);
			this->panel_Display->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion

};