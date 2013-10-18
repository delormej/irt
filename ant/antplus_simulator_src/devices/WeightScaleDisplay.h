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
#include "antplus_common.h"
#include "antplus_weight.h"
#include "types.h"
#include "antdefines.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


public ref class WeightScaleDisplay : public System::Windows::Forms::Form, public ISimBase{
	public:
		WeightScaleDisplay(dRequestBcastMsg^ channelBcastMsg){
			commonPages = gcnew CommonData();
			wsPages = gcnew WeightScale(WeightScale::DeviceClass::DISPLAY);
			InitializeComponent();
			requestBcastMsg = channelBcastMsg;
			InitializeSim();
		}

		~WeightScaleDisplay(){
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
		virtual UCHAR getDeviceType(){return WeightScale::DEVICE_TYPE;}
		virtual UCHAR getTransmissionType(){return 0;}	// Set to 0 for a pairing search and future compatibility
		virtual USHORT getTransmitPeriod(){return WeightScale::MSG_PERIOD;}
		virtual DOUBLE getTimerInterval(){return 3600000;} // Set interval to one hour, as timer is not used in the simulation
		virtual void onTimerTock(USHORT eventTime){} // Do nothing
		virtual System::Windows::Forms::Panel^ getSimSettingsPanel(){return this->panel_Settings;}
		virtual System::Windows::Forms::Panel^ getSimTranslatedDisplay(){return this->panel_Display;}
	
	private:
		void InitializeSim();
		void HandleReceive(UCHAR* pucRxBuffer_);
		void HandleTransmit(UCHAR ucPageNum_);
		void ProcessReceivedMessage(UCHAR ucPageNum_);
		void SendUserProfile();
		void UpdateDisplay(UCHAR ucPageNum_);
		void UpdateDisplayActiveProfile();
		void UpdateDisplayCapabilities();
		void SelectCustomProfile();
		System::Void checkBoxProfileSet_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		System::Void checkBoxAntfs_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		System::Void checkBoxExchange_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		System::Void buttonReset_Click(System::Object^  sender, System::EventArgs^  e);
		
private:
		CommonData^ commonPages;			// Handle to common data pages
		WeightScale^ wsPages;				// Handle to weight scale pages
		dRequestBcastMsg^ requestBcastMsg;
		
		
		// Preset User Profiles
		WeightScaleUserProfile^ uprofUndef;
		WeightScaleUserProfile^ uprofCustom;

		// Extended
		BOOL bSentProfile;
		BOOL bRxAdvanced;
		BOOL bRxMain;

private: System::Windows::Forms::Button^  button2;
private: System::Windows::Forms::CheckBox^  checkBox3;
private: System::Windows::Forms::CheckBox^  checkBox4;
private: System::Windows::Forms::CheckBox^  checkBoxUPSet;
private: System::Windows::Forms::ComboBox^  comboBoxActivityLevel;
private: System::Windows::Forms::GroupBox^  groupBox1;
private: System::Windows::Forms::Label^  label_Trn_DisplayTitle;
private: System::Windows::Forms::Label^  label1;
private: System::Windows::Forms::Label^  label12;
private: System::Windows::Forms::Label^  label13;
private: System::Windows::Forms::Label^  label14;
private: System::Windows::Forms::Label^  label15;
private: System::Windows::Forms::Label^  label16;
private: System::Windows::Forms::Label^  label17;
private: System::Windows::Forms::Label^  label18;
private: System::Windows::Forms::Label^  label19;
private: System::Windows::Forms::Label^  label20;
private: System::Windows::Forms::Label^  label21;
private: System::Windows::Forms::Label^  label22;
private: System::Windows::Forms::Label^  label25;
private: System::Windows::Forms::Label^  label26;
private: System::Windows::Forms::Label^  label27;
private: System::Windows::Forms::Label^  label28;
private: System::Windows::Forms::Label^  label29;
private: System::Windows::Forms::Label^  labelPage;
private: System::Windows::Forms::Label^  labelProfileActive;
private: System::Windows::Forms::Label^  labelW1;
private: System::Windows::Forms::Label^  labelWeight;
private: System::Windows::Forms::Panel^  panel_Display;
private: System::Windows::Forms::Panel^  panel_Settings;
private: System::Windows::Forms::TabPage^  tabPage_GlobalData;
private: System::Windows::Forms::TabPage^  tabPage_UserProfile;
private: System::Windows::Forms::TextBox^  textboxHwVer;
private: System::Windows::Forms::TextBox^  textboxMfgID;
private: System::Windows::Forms::TextBox^  textboxModelNum;
private: System::Windows::Forms::TextBox^  textboxSerialNum;
private: System::Windows::Forms::TextBox^  textboxSwVer;
private: System::Windows::Forms::TabControl^  tabControl1;
private: System::Windows::Forms::TabPage^  tabPage_GlobalBattery;
private: System::Windows::Forms::Label^  label_Glb_SoftwareVer;
private: System::Windows::Forms::Label^  label_Glb_HardwareVer;
private: System::Windows::Forms::Label^  labelSwVer;
private: System::Windows::Forms::Label^  labelSerialNum;
private: System::Windows::Forms::Label^  label_Glb_ModelNum;
private: System::Windows::Forms::Label^  labelMfgID;
private: System::Windows::Forms::Label^  label_Glb_SerialNum;
private: System::Windows::Forms::Label^  labelModelNum;
private: System::Windows::Forms::Label^  labelHwVer;
private: System::Windows::Forms::Label^  label_Glb_ManfID;
private: System::Windows::Forms::TabPage^  tabPage_CustomProfile;
private: System::Windows::Forms::Label^  label2;
private: System::Windows::Forms::NumericUpDown^  numericUpDownProfile;
private: System::Windows::Forms::GroupBox^  groupBox2;
private: System::Windows::Forms::RadioButton^  radioButtonFemale;
private: System::Windows::Forms::RadioButton^  radioButtonMale;
private: System::Windows::Forms::Label^  label3;
private: System::Windows::Forms::Label^  label4;
private: System::Windows::Forms::NumericUpDown^  numericUpDownHeight;
private: System::Windows::Forms::Label^  label5;
private: System::Windows::Forms::NumericUpDown^  numericUpDownAge;
private: System::Windows::Forms::TabPage^  tabPage_ActiveProfile;
private: System::Windows::Forms::Label^  label156;
private: System::Windows::Forms::Label^  labelProfileSetScale;
private: System::Windows::Forms::Label^  label81;
private: System::Windows::Forms::Label^  labelExchangeScale;
private: System::Windows::Forms::Label^  label79;
private: System::Windows::Forms::Label^  labelAntfsRx;
private: System::Windows::Forms::Label^  label6;
private: System::Windows::Forms::Label^  labelDescriptionActive;
private: System::Windows::Forms::Label^  label_Bat_Status;
private: System::Windows::Forms::Label^  label32;
private: System::Windows::Forms::Label^  labelHeightActive;
private: System::Windows::Forms::Label^  label33;
private: System::Windows::Forms::Label^  labelAgeActive;
private: System::Windows::Forms::Label^  label37;
private: System::Windows::Forms::Label^  labelGenderActive;
private: System::Windows::Forms::Label^  label39;
private: System::Windows::Forms::Label^  labelProfileActive2;
private: System::Windows::Forms::NumericUpDown^  numericUpDownActivityLevel;
private: System::Windows::Forms::GroupBox^  groupBoxAthlete;
private: System::Windows::Forms::RadioButton^  radioButtonAthleteFalse;
private: System::Windows::Forms::RadioButton^  radioButtonAthleteTrue;
private: System::Windows::Forms::Label^  labelLifestyle;
private: System::Windows::Forms::Label^  label40;
private: System::Windows::Forms::TabPage^  tabPage2;
private: System::Windows::Forms::Button^  buttonReset;
private: System::Windows::Forms::Label^  labelStatusProfileSent;
private: System::Windows::Forms::Label^  labelStatusProfileUpdate;
private: System::Windows::Forms::Label^  labelStatusProfileMatch;
private: System::Windows::Forms::Label^  labelStatusProfileNew;
private: System::Windows::Forms::CheckBox^  checkBoxProfileSet;
private: System::Windows::Forms::CheckBox^  checkBoxExchange;
private: System::Windows::Forms::CheckBox^  checkBoxAntfs;
private: System::Windows::Forms::Label^  labelStatusProfileRx;
private: System::Windows::Forms::Label^  label_Dat_PageRec;
private: System::Windows::Forms::Label^  labelPage2;
private: System::Windows::Forms::Label^  labelExchangeDisplay;
private: System::Windows::Forms::Label^  label7;
private: System::Windows::Forms::Label^  labelBMR;
private: System::Windows::Forms::Label^  label_BasalMetRate;
private: System::Windows::Forms::Label^  labelAMR;
private: System::Windows::Forms::Label^  label_ActiveMetRate;
private: System::Windows::Forms::Label^  label_MR;
private: System::Windows::Forms::Label^  labelBF;
private: System::Windows::Forms::Label^  label_BodyFat;
private: System::Windows::Forms::Label^  labelH;
private: System::Windows::Forms::Label^  label_Hydration;
private: System::Windows::Forms::Label^  label8;
private: System::Windows::Forms::TabPage^  tabPage_DataPages;
private: System::Windows::Forms::Label^  label41;
private: System::Windows::Forms::Label^  label_adv_bonemass;
private: System::Windows::Forms::Label^  label43;
private: System::Windows::Forms::Label^  label_adv_musclemass;
private: System::Windows::Forms::Label^  label9;
private: System::Windows::Forms::Label^  label_adv_basalrate;
private: System::Windows::Forms::Label^  label11;
private: System::Windows::Forms::Label^  label_adv_activerate;
private: System::Windows::Forms::Label^  label24;
private: System::Windows::Forms::Label^  label30;
private: System::Windows::Forms::Label^  label_adv_bodyfat;
private: System::Windows::Forms::Label^  label34;
private: System::Windows::Forms::Label^  label_adv_hydration;
private: System::Windows::Forms::Label^  label36;
private: System::Windows::Forms::Label^  label_adv_weight;



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
         this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
         this->tabPage2 = (gcnew System::Windows::Forms::TabPage());
         this->buttonReset = (gcnew System::Windows::Forms::Button());
         this->labelStatusProfileSent = (gcnew System::Windows::Forms::Label());
         this->labelStatusProfileUpdate = (gcnew System::Windows::Forms::Label());
         this->labelStatusProfileMatch = (gcnew System::Windows::Forms::Label());
         this->labelStatusProfileNew = (gcnew System::Windows::Forms::Label());
         this->checkBoxProfileSet = (gcnew System::Windows::Forms::CheckBox());
         this->checkBoxExchange = (gcnew System::Windows::Forms::CheckBox());
         this->checkBoxAntfs = (gcnew System::Windows::Forms::CheckBox());
         this->labelStatusProfileRx = (gcnew System::Windows::Forms::Label());
         this->label_Dat_PageRec = (gcnew System::Windows::Forms::Label());
         this->labelPage2 = (gcnew System::Windows::Forms::Label());
         this->tabPage_DataPages = (gcnew System::Windows::Forms::TabPage());
         this->label41 = (gcnew System::Windows::Forms::Label());
         this->label_adv_bonemass = (gcnew System::Windows::Forms::Label());
         this->label43 = (gcnew System::Windows::Forms::Label());
         this->label_adv_musclemass = (gcnew System::Windows::Forms::Label());
         this->label9 = (gcnew System::Windows::Forms::Label());
         this->label_adv_basalrate = (gcnew System::Windows::Forms::Label());
         this->label11 = (gcnew System::Windows::Forms::Label());
         this->label_adv_activerate = (gcnew System::Windows::Forms::Label());
         this->label24 = (gcnew System::Windows::Forms::Label());
         this->label30 = (gcnew System::Windows::Forms::Label());
         this->label_adv_bodyfat = (gcnew System::Windows::Forms::Label());
         this->label34 = (gcnew System::Windows::Forms::Label());
         this->label_adv_hydration = (gcnew System::Windows::Forms::Label());
         this->label36 = (gcnew System::Windows::Forms::Label());
         this->label_adv_weight = (gcnew System::Windows::Forms::Label());
         this->tabPage_GlobalBattery = (gcnew System::Windows::Forms::TabPage());
         this->label_Glb_SoftwareVer = (gcnew System::Windows::Forms::Label());
         this->label_Glb_HardwareVer = (gcnew System::Windows::Forms::Label());
         this->labelSwVer = (gcnew System::Windows::Forms::Label());
         this->labelSerialNum = (gcnew System::Windows::Forms::Label());
         this->label_Glb_ModelNum = (gcnew System::Windows::Forms::Label());
         this->labelMfgID = (gcnew System::Windows::Forms::Label());
         this->label_Glb_SerialNum = (gcnew System::Windows::Forms::Label());
         this->labelModelNum = (gcnew System::Windows::Forms::Label());
         this->labelHwVer = (gcnew System::Windows::Forms::Label());
         this->label_Glb_ManfID = (gcnew System::Windows::Forms::Label());
         this->tabPage_CustomProfile = (gcnew System::Windows::Forms::TabPage());
         this->groupBoxAthlete = (gcnew System::Windows::Forms::GroupBox());
         this->radioButtonAthleteFalse = (gcnew System::Windows::Forms::RadioButton());
         this->radioButtonAthleteTrue = (gcnew System::Windows::Forms::RadioButton());
         this->numericUpDownActivityLevel = (gcnew System::Windows::Forms::NumericUpDown());
         this->label2 = (gcnew System::Windows::Forms::Label());
         this->numericUpDownProfile = (gcnew System::Windows::Forms::NumericUpDown());
         this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
         this->radioButtonFemale = (gcnew System::Windows::Forms::RadioButton());
         this->radioButtonMale = (gcnew System::Windows::Forms::RadioButton());
         this->label3 = (gcnew System::Windows::Forms::Label());
         this->label4 = (gcnew System::Windows::Forms::Label());
         this->numericUpDownHeight = (gcnew System::Windows::Forms::NumericUpDown());
         this->label5 = (gcnew System::Windows::Forms::Label());
         this->numericUpDownAge = (gcnew System::Windows::Forms::NumericUpDown());
         this->tabPage_ActiveProfile = (gcnew System::Windows::Forms::TabPage());
         this->labelExchangeDisplay = (gcnew System::Windows::Forms::Label());
         this->label7 = (gcnew System::Windows::Forms::Label());
         this->labelLifestyle = (gcnew System::Windows::Forms::Label());
         this->label40 = (gcnew System::Windows::Forms::Label());
         this->label156 = (gcnew System::Windows::Forms::Label());
         this->labelProfileSetScale = (gcnew System::Windows::Forms::Label());
         this->label81 = (gcnew System::Windows::Forms::Label());
         this->labelExchangeScale = (gcnew System::Windows::Forms::Label());
         this->label79 = (gcnew System::Windows::Forms::Label());
         this->labelAntfsRx = (gcnew System::Windows::Forms::Label());
         this->label6 = (gcnew System::Windows::Forms::Label());
         this->labelDescriptionActive = (gcnew System::Windows::Forms::Label());
         this->label_Bat_Status = (gcnew System::Windows::Forms::Label());
         this->label32 = (gcnew System::Windows::Forms::Label());
         this->labelHeightActive = (gcnew System::Windows::Forms::Label());
         this->label33 = (gcnew System::Windows::Forms::Label());
         this->labelAgeActive = (gcnew System::Windows::Forms::Label());
         this->label37 = (gcnew System::Windows::Forms::Label());
         this->labelGenderActive = (gcnew System::Windows::Forms::Label());
         this->label39 = (gcnew System::Windows::Forms::Label());
         this->labelProfileActive2 = (gcnew System::Windows::Forms::Label());
         this->panel_Display = (gcnew System::Windows::Forms::Panel());
         this->label8 = (gcnew System::Windows::Forms::Label());
         this->labelBMR = (gcnew System::Windows::Forms::Label());
         this->label_BasalMetRate = (gcnew System::Windows::Forms::Label());
         this->labelAMR = (gcnew System::Windows::Forms::Label());
         this->label_ActiveMetRate = (gcnew System::Windows::Forms::Label());
         this->label_MR = (gcnew System::Windows::Forms::Label());
         this->labelBF = (gcnew System::Windows::Forms::Label());
         this->label_BodyFat = (gcnew System::Windows::Forms::Label());
         this->labelH = (gcnew System::Windows::Forms::Label());
         this->label_Hydration = (gcnew System::Windows::Forms::Label());
         this->labelPage = (gcnew System::Windows::Forms::Label());
         this->label1 = (gcnew System::Windows::Forms::Label());
         this->labelProfileActive = (gcnew System::Windows::Forms::Label());
         this->label_Trn_DisplayTitle = (gcnew System::Windows::Forms::Label());
         this->labelW1 = (gcnew System::Windows::Forms::Label());
         this->labelWeight = (gcnew System::Windows::Forms::Label());
         this->tabPage_UserProfile = (gcnew System::Windows::Forms::TabPage());
         this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
         this->checkBoxUPSet = (gcnew System::Windows::Forms::CheckBox());
         this->label29 = (gcnew System::Windows::Forms::Label());
         this->comboBoxActivityLevel = (gcnew System::Windows::Forms::ComboBox());
         this->label28 = (gcnew System::Windows::Forms::Label());
         this->label27 = (gcnew System::Windows::Forms::Label());
         this->tabPage_GlobalData = (gcnew System::Windows::Forms::TabPage());
         this->label12 = (gcnew System::Windows::Forms::Label());
         this->label13 = (gcnew System::Windows::Forms::Label());
         this->label14 = (gcnew System::Windows::Forms::Label());
         this->label15 = (gcnew System::Windows::Forms::Label());
         this->label16 = (gcnew System::Windows::Forms::Label());
         this->checkBox3 = (gcnew System::Windows::Forms::CheckBox());
         this->checkBox4 = (gcnew System::Windows::Forms::CheckBox());
         this->button2 = (gcnew System::Windows::Forms::Button());
         this->label17 = (gcnew System::Windows::Forms::Label());
         this->label18 = (gcnew System::Windows::Forms::Label());
         this->label19 = (gcnew System::Windows::Forms::Label());
         this->textboxSerialNum = (gcnew System::Windows::Forms::TextBox());
         this->textboxSwVer = (gcnew System::Windows::Forms::TextBox());
         this->label20 = (gcnew System::Windows::Forms::Label());
         this->label21 = (gcnew System::Windows::Forms::Label());
         this->label22 = (gcnew System::Windows::Forms::Label());
         this->textboxHwVer = (gcnew System::Windows::Forms::TextBox());
         this->textboxModelNum = (gcnew System::Windows::Forms::TextBox());
         this->textboxMfgID = (gcnew System::Windows::Forms::TextBox());
         this->panel_Settings->SuspendLayout();
         this->tabControl1->SuspendLayout();
         this->tabPage2->SuspendLayout();
         this->tabPage_DataPages->SuspendLayout();
         this->tabPage_GlobalBattery->SuspendLayout();
         this->tabPage_CustomProfile->SuspendLayout();
         this->groupBoxAthlete->SuspendLayout();
         (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownActivityLevel))->BeginInit();
         (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownProfile))->BeginInit();
         this->groupBox2->SuspendLayout();
         (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownHeight))->BeginInit();
         (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownAge))->BeginInit();
         this->tabPage_ActiveProfile->SuspendLayout();
         this->panel_Display->SuspendLayout();
         this->tabPage_UserProfile->SuspendLayout();
         this->tabPage_GlobalData->SuspendLayout();
         this->SuspendLayout();
         // 
         // panel_Settings
         // 
         this->panel_Settings->Controls->Add(this->tabControl1);
         this->panel_Settings->Location = System::Drawing::Point(322, 50);
         this->panel_Settings->Name = L"panel_Settings";
         this->panel_Settings->Size = System::Drawing::Size(400, 157);
         this->panel_Settings->TabIndex = 0;
         // 
         // tabControl1
         // 
         this->tabControl1->Controls->Add(this->tabPage2);
         this->tabControl1->Controls->Add(this->tabPage_DataPages);
         this->tabControl1->Controls->Add(this->tabPage_GlobalBattery);
         this->tabControl1->Controls->Add(this->tabPage_CustomProfile);
         this->tabControl1->Controls->Add(this->tabPage_ActiveProfile);
         this->tabControl1->Location = System::Drawing::Point(4, 4);
         this->tabControl1->Name = L"tabControl1";
         this->tabControl1->SelectedIndex = 0;
         this->tabControl1->Size = System::Drawing::Size(395, 152);
         this->tabControl1->TabIndex = 0;
         // 
         // tabPage2
         // 
         this->tabPage2->Controls->Add(this->buttonReset);
         this->tabPage2->Controls->Add(this->labelStatusProfileSent);
         this->tabPage2->Controls->Add(this->labelStatusProfileUpdate);
         this->tabPage2->Controls->Add(this->labelStatusProfileMatch);
         this->tabPage2->Controls->Add(this->labelStatusProfileNew);
         this->tabPage2->Controls->Add(this->checkBoxProfileSet);
         this->tabPage2->Controls->Add(this->checkBoxExchange);
         this->tabPage2->Controls->Add(this->checkBoxAntfs);
         this->tabPage2->Controls->Add(this->labelStatusProfileRx);
         this->tabPage2->Controls->Add(this->label_Dat_PageRec);
         this->tabPage2->Controls->Add(this->labelPage2);
         this->tabPage2->Location = System::Drawing::Point(4, 22);
         this->tabPage2->Name = L"tabPage2";
         this->tabPage2->Padding = System::Windows::Forms::Padding(3);
         this->tabPage2->Size = System::Drawing::Size(387, 126);
         this->tabPage2->TabIndex = 1;
         this->tabPage2->Text = L"Data";
         this->tabPage2->UseVisualStyleBackColor = true;
         // 
         // buttonReset
         // 
         this->buttonReset->Location = System::Drawing::Point(46, 81);
         this->buttonReset->Name = L"buttonReset";
         this->buttonReset->Size = System::Drawing::Size(114, 25);
         this->buttonReset->TabIndex = 34;
         this->buttonReset->Text = L"Reset Session";
         this->buttonReset->UseVisualStyleBackColor = true;
         this->buttonReset->Click += gcnew System::EventHandler(this, &WeightScaleDisplay::buttonReset_Click);
         // 
         // labelStatusProfileSent
         // 
         this->labelStatusProfileSent->AutoSize = true;
         this->labelStatusProfileSent->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.5F, System::Drawing::FontStyle::Bold, 
            System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
         this->labelStatusProfileSent->ForeColor = System::Drawing::SystemColors::ControlText;
         this->labelStatusProfileSent->Location = System::Drawing::Point(223, 87);
         this->labelStatusProfileSent->Name = L"labelStatusProfileSent";
         this->labelStatusProfileSent->Size = System::Drawing::Size(0, 15);
         this->labelStatusProfileSent->TabIndex = 33;
         // 
         // labelStatusProfileUpdate
         // 
         this->labelStatusProfileUpdate->AutoSize = true;
         this->labelStatusProfileUpdate->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.5F, System::Drawing::FontStyle::Bold, 
            System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
         this->labelStatusProfileUpdate->ForeColor = System::Drawing::SystemColors::ControlText;
         this->labelStatusProfileUpdate->Location = System::Drawing::Point(223, 72);
         this->labelStatusProfileUpdate->Name = L"labelStatusProfileUpdate";
         this->labelStatusProfileUpdate->Size = System::Drawing::Size(0, 15);
         this->labelStatusProfileUpdate->TabIndex = 32;
         // 
         // labelStatusProfileMatch
         // 
         this->labelStatusProfileMatch->AutoSize = true;
         this->labelStatusProfileMatch->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.5F, System::Drawing::FontStyle::Bold, 
            System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
         this->labelStatusProfileMatch->ForeColor = System::Drawing::SystemColors::ControlText;
         this->labelStatusProfileMatch->Location = System::Drawing::Point(223, 57);
         this->labelStatusProfileMatch->Name = L"labelStatusProfileMatch";
         this->labelStatusProfileMatch->Size = System::Drawing::Size(0, 15);
         this->labelStatusProfileMatch->TabIndex = 31;
         // 
         // labelStatusProfileNew
         // 
         this->labelStatusProfileNew->AutoSize = true;
         this->labelStatusProfileNew->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.5F, System::Drawing::FontStyle::Bold, 
            System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
         this->labelStatusProfileNew->ForeColor = System::Drawing::SystemColors::ControlText;
         this->labelStatusProfileNew->Location = System::Drawing::Point(223, 42);
         this->labelStatusProfileNew->Name = L"labelStatusProfileNew";
         this->labelStatusProfileNew->Size = System::Drawing::Size(0, 15);
         this->labelStatusProfileNew->TabIndex = 30;
         // 
         // checkBoxProfileSet
         // 
         this->checkBoxProfileSet->AutoSize = true;
         this->checkBoxProfileSet->Checked = true;
         this->checkBoxProfileSet->CheckState = System::Windows::Forms::CheckState::Checked;
         this->checkBoxProfileSet->Location = System::Drawing::Point(18, 5);
         this->checkBoxProfileSet->Name = L"checkBoxProfileSet";
         this->checkBoxProfileSet->Size = System::Drawing::Size(99, 17);
         this->checkBoxProfileSet->TabIndex = 28;
         this->checkBoxProfileSet->Text = L"User Profile Set";
         this->checkBoxProfileSet->UseVisualStyleBackColor = true;
         this->checkBoxProfileSet->CheckedChanged += gcnew System::EventHandler(this, &WeightScaleDisplay::checkBoxProfileSet_CheckedChanged);
         // 
         // checkBoxExchange
         // 
         this->checkBoxExchange->AutoSize = true;
         this->checkBoxExchange->Checked = true;
         this->checkBoxExchange->CheckState = System::Windows::Forms::CheckState::Checked;
         this->checkBoxExchange->Location = System::Drawing::Point(18, 55);
         this->checkBoxExchange->Name = L"checkBoxExchange";
         this->checkBoxExchange->Size = System::Drawing::Size(171, 17);
         this->checkBoxExchange->TabIndex = 27;
         this->checkBoxExchange->Text = L"User Profile Exchange Support";
         this->checkBoxExchange->UseVisualStyleBackColor = true;
         this->checkBoxExchange->CheckedChanged += gcnew System::EventHandler(this, &WeightScaleDisplay::checkBoxExchange_CheckedChanged);
         // 
         // checkBoxAntfs
         // 
         this->checkBoxAntfs->AutoSize = true;
         this->checkBoxAntfs->Enabled = false;
         this->checkBoxAntfs->Location = System::Drawing::Point(18, 30);
         this->checkBoxAntfs->Name = L"checkBoxAntfs";
         this->checkBoxAntfs->Size = System::Drawing::Size(104, 17);
         this->checkBoxAntfs->TabIndex = 26;
         this->checkBoxAntfs->Text = L"ANT-FS Support";
         this->checkBoxAntfs->UseVisualStyleBackColor = true;
         this->checkBoxAntfs->CheckedChanged += gcnew System::EventHandler(this, &WeightScaleDisplay::checkBoxAntfs_CheckedChanged);
         // 
         // labelStatusProfileRx
         // 
         this->labelStatusProfileRx->AutoSize = true;
         this->labelStatusProfileRx->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.5F, System::Drawing::FontStyle::Bold, 
            System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
         this->labelStatusProfileRx->ForeColor = System::Drawing::SystemColors::ControlText;
         this->labelStatusProfileRx->Location = System::Drawing::Point(223, 27);
         this->labelStatusProfileRx->Name = L"labelStatusProfileRx";
         this->labelStatusProfileRx->Size = System::Drawing::Size(0, 15);
         this->labelStatusProfileRx->TabIndex = 25;
         // 
         // label_Dat_PageRec
         // 
         this->label_Dat_PageRec->AutoSize = true;
         this->label_Dat_PageRec->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
            static_cast<System::Byte>(0)));
         this->label_Dat_PageRec->Location = System::Drawing::Point(225, 6);
         this->label_Dat_PageRec->Name = L"label_Dat_PageRec";
         this->label_Dat_PageRec->Size = System::Drawing::Size(0, 13);
         this->label_Dat_PageRec->TabIndex = 23;
         // 
         // labelPage2
         // 
         this->labelPage2->AutoSize = true;
         this->labelPage2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
            static_cast<System::Byte>(0)));
         this->labelPage2->Location = System::Drawing::Point(325, 6);
         this->labelPage2->Name = L"labelPage2";
         this->labelPage2->Size = System::Drawing::Size(0, 13);
         this->labelPage2->TabIndex = 24;
         // 
         // tabPage_DataPages
         // 
         this->tabPage_DataPages->Controls->Add(this->label41);
         this->tabPage_DataPages->Controls->Add(this->label_adv_bonemass);
         this->tabPage_DataPages->Controls->Add(this->label43);
         this->tabPage_DataPages->Controls->Add(this->label_adv_musclemass);
         this->tabPage_DataPages->Controls->Add(this->label9);
         this->tabPage_DataPages->Controls->Add(this->label_adv_basalrate);
         this->tabPage_DataPages->Controls->Add(this->label11);
         this->tabPage_DataPages->Controls->Add(this->label_adv_activerate);
         this->tabPage_DataPages->Controls->Add(this->label24);
         this->tabPage_DataPages->Controls->Add(this->label30);
         this->tabPage_DataPages->Controls->Add(this->label_adv_bodyfat);
         this->tabPage_DataPages->Controls->Add(this->label34);
         this->tabPage_DataPages->Controls->Add(this->label_adv_hydration);
         this->tabPage_DataPages->Controls->Add(this->label36);
         this->tabPage_DataPages->Controls->Add(this->label_adv_weight);
         this->tabPage_DataPages->Location = System::Drawing::Point(4, 22);
         this->tabPage_DataPages->Name = L"tabPage_DataPages";
         this->tabPage_DataPages->Padding = System::Windows::Forms::Padding(3);
         this->tabPage_DataPages->Size = System::Drawing::Size(387, 126);
         this->tabPage_DataPages->TabIndex = 9;
         this->tabPage_DataPages->Text = L"Advanced";
         this->tabPage_DataPages->UseVisualStyleBackColor = true;
         // 
         // label41
         // 
         this->label41->AutoSize = true;
         this->label41->BackColor = System::Drawing::Color::Transparent;
         this->label41->Location = System::Drawing::Point(241, 46);
         this->label41->Name = L"label41";
         this->label41->Size = System::Drawing::Size(84, 13);
         this->label41->TabIndex = 53;
         this->label41->Text = L"Bone Mass (kg):";
         // 
         // label_adv_bonemass
         // 
         this->label_adv_bonemass->AutoSize = true;
         this->label_adv_bonemass->BackColor = System::Drawing::Color::Transparent;
         this->label_adv_bonemass->Location = System::Drawing::Point(340, 46);
         this->label_adv_bonemass->Name = L"label_adv_bonemass";
         this->label_adv_bonemass->Size = System::Drawing::Size(16, 13);
         this->label_adv_bonemass->TabIndex = 54;
         this->label_adv_bonemass->Text = L"---";
         // 
         // label43
         // 
         this->label43->AutoSize = true;
         this->label43->BackColor = System::Drawing::Color::Transparent;
         this->label43->Location = System::Drawing::Point(241, 30);
         this->label43->Name = L"label43";
         this->label43->Size = System::Drawing::Size(93, 13);
         this->label43->TabIndex = 51;
         this->label43->Text = L"Muscle Mass (kg):";
         // 
         // label_adv_musclemass
         // 
         this->label_adv_musclemass->AutoSize = true;
         this->label_adv_musclemass->BackColor = System::Drawing::Color::Transparent;
         this->label_adv_musclemass->Location = System::Drawing::Point(340, 30);
         this->label_adv_musclemass->Name = L"label_adv_musclemass";
         this->label_adv_musclemass->Size = System::Drawing::Size(16, 13);
         this->label_adv_musclemass->TabIndex = 52;
         this->label_adv_musclemass->Text = L"---";
         // 
         // label9
         // 
         this->label9->AutoSize = true;
         this->label9->BackColor = System::Drawing::Color::Transparent;
         this->label9->Location = System::Drawing::Point(138, 46);
         this->label9->Name = L"label9";
         this->label9->Size = System::Drawing::Size(36, 13);
         this->label9->TabIndex = 48;
         this->label9->Text = L"Basal:";
         // 
         // label_adv_basalrate
         // 
         this->label_adv_basalrate->AutoSize = true;
         this->label_adv_basalrate->BackColor = System::Drawing::Color::Transparent;
         this->label_adv_basalrate->Location = System::Drawing::Point(177, 46);
         this->label_adv_basalrate->Name = L"label_adv_basalrate";
         this->label_adv_basalrate->Size = System::Drawing::Size(16, 13);
         this->label_adv_basalrate->TabIndex = 49;
         this->label_adv_basalrate->Text = L"---";
         // 
         // label11
         // 
         this->label11->AutoSize = true;
         this->label11->BackColor = System::Drawing::Color::Transparent;
         this->label11->Location = System::Drawing::Point(138, 30);
         this->label11->Name = L"label11";
         this->label11->Size = System::Drawing::Size(40, 13);
         this->label11->TabIndex = 46;
         this->label11->Text = L"Active:";
         // 
         // label_adv_activerate
         // 
         this->label_adv_activerate->AutoSize = true;
         this->label_adv_activerate->BackColor = System::Drawing::Color::Transparent;
         this->label_adv_activerate->Location = System::Drawing::Point(177, 30);
         this->label_adv_activerate->Name = L"label_adv_activerate";
         this->label_adv_activerate->Size = System::Drawing::Size(16, 13);
         this->label_adv_activerate->TabIndex = 47;
         this->label_adv_activerate->Text = L"---";
         // 
         // label24
         // 
         this->label24->AutoSize = true;
         this->label24->Location = System::Drawing::Point(136, 13);
         this->label24->Name = L"label24";
         this->label24->Size = System::Drawing::Size(80, 13);
         this->label24->TabIndex = 45;
         this->label24->Text = L"Met Rate (kcal)";
         // 
         // label30
         // 
         this->label30->AutoSize = true;
         this->label30->BackColor = System::Drawing::Color::Transparent;
         this->label30->Location = System::Drawing::Point(6, 47);
         this->label30->Name = L"label30";
         this->label30->Size = System::Drawing::Size(69, 13);
         this->label30->TabIndex = 43;
         this->label30->Text = L"Body Fat (%):";
         // 
         // label_adv_bodyfat
         // 
         this->label_adv_bodyfat->AutoSize = true;
         this->label_adv_bodyfat->BackColor = System::Drawing::Color::Transparent;
         this->label_adv_bodyfat->Location = System::Drawing::Point(79, 48);
         this->label_adv_bodyfat->Name = L"label_adv_bodyfat";
         this->label_adv_bodyfat->Size = System::Drawing::Size(16, 13);
         this->label_adv_bodyfat->TabIndex = 44;
         this->label_adv_bodyfat->Text = L"---";
         // 
         // label34
         // 
         this->label34->AutoSize = true;
         this->label34->BackColor = System::Drawing::Color::Transparent;
         this->label34->Location = System::Drawing::Point(6, 30);
         this->label34->Name = L"label34";
         this->label34->Size = System::Drawing::Size(72, 13);
         this->label34->TabIndex = 41;
         this->label34->Text = L"Hydration (%):";
         // 
         // label_adv_hydration
         // 
         this->label_adv_hydration->AutoSize = true;
         this->label_adv_hydration->BackColor = System::Drawing::Color::Transparent;
         this->label_adv_hydration->Location = System::Drawing::Point(79, 31);
         this->label_adv_hydration->Name = L"label_adv_hydration";
         this->label_adv_hydration->Size = System::Drawing::Size(16, 13);
         this->label_adv_hydration->TabIndex = 42;
         this->label_adv_hydration->Text = L"---";
         // 
         // label36
         // 
         this->label36->AutoSize = true;
         this->label36->BackColor = System::Drawing::Color::Transparent;
         this->label36->Location = System::Drawing::Point(6, 13);
         this->label36->Name = L"label36";
         this->label36->Size = System::Drawing::Size(65, 13);
         this->label36->TabIndex = 39;
         this->label36->Text = L"Weight (kg):";
         // 
         // label_adv_weight
         // 
         this->label_adv_weight->AutoSize = true;
         this->label_adv_weight->BackColor = System::Drawing::Color::Transparent;
         this->label_adv_weight->Location = System::Drawing::Point(78, 13);
         this->label_adv_weight->Name = L"label_adv_weight";
         this->label_adv_weight->Size = System::Drawing::Size(16, 13);
         this->label_adv_weight->TabIndex = 40;
         this->label_adv_weight->Text = L"---";
         // 
         // tabPage_GlobalBattery
         // 
         this->tabPage_GlobalBattery->Controls->Add(this->label_Glb_SoftwareVer);
         this->tabPage_GlobalBattery->Controls->Add(this->label_Glb_HardwareVer);
         this->tabPage_GlobalBattery->Controls->Add(this->labelSwVer);
         this->tabPage_GlobalBattery->Controls->Add(this->labelSerialNum);
         this->tabPage_GlobalBattery->Controls->Add(this->label_Glb_ModelNum);
         this->tabPage_GlobalBattery->Controls->Add(this->labelMfgID);
         this->tabPage_GlobalBattery->Controls->Add(this->label_Glb_SerialNum);
         this->tabPage_GlobalBattery->Controls->Add(this->labelModelNum);
         this->tabPage_GlobalBattery->Controls->Add(this->labelHwVer);
         this->tabPage_GlobalBattery->Controls->Add(this->label_Glb_ManfID);
         this->tabPage_GlobalBattery->Location = System::Drawing::Point(4, 22);
         this->tabPage_GlobalBattery->Name = L"tabPage_GlobalBattery";
         this->tabPage_GlobalBattery->Size = System::Drawing::Size(387, 126);
         this->tabPage_GlobalBattery->TabIndex = 5;
         this->tabPage_GlobalBattery->Text = L"Global Data";
         this->tabPage_GlobalBattery->UseVisualStyleBackColor = true;
         // 
         // label_Glb_SoftwareVer
         // 
         this->label_Glb_SoftwareVer->AutoSize = true;
         this->label_Glb_SoftwareVer->Location = System::Drawing::Point(38, 28);
         this->label_Glb_SoftwareVer->Name = L"label_Glb_SoftwareVer";
         this->label_Glb_SoftwareVer->Size = System::Drawing::Size(71, 13);
         this->label_Glb_SoftwareVer->TabIndex = 4;
         this->label_Glb_SoftwareVer->Text = L"Software Ver:";
         // 
         // label_Glb_HardwareVer
         // 
         this->label_Glb_HardwareVer->AutoSize = true;
         this->label_Glb_HardwareVer->Location = System::Drawing::Point(34, 85);
         this->label_Glb_HardwareVer->Name = L"label_Glb_HardwareVer";
         this->label_Glb_HardwareVer->Size = System::Drawing::Size(75, 13);
         this->label_Glb_HardwareVer->TabIndex = 3;
         this->label_Glb_HardwareVer->Text = L"Hardware Ver:";
         // 
         // labelSwVer
         // 
         this->labelSwVer->AutoSize = true;
         this->labelSwVer->Location = System::Drawing::Point(115, 28);
         this->labelSwVer->Name = L"labelSwVer";
         this->labelSwVer->Size = System::Drawing::Size(16, 13);
         this->labelSwVer->TabIndex = 16;
         this->labelSwVer->Text = L"---";
         // 
         // labelSerialNum
         // 
         this->labelSerialNum->AutoSize = true;
         this->labelSerialNum->Location = System::Drawing::Point(115, 9);
         this->labelSerialNum->Name = L"labelSerialNum";
         this->labelSerialNum->Size = System::Drawing::Size(16, 13);
         this->labelSerialNum->TabIndex = 11;
         this->labelSerialNum->Text = L"---";
         // 
         // label_Glb_ModelNum
         // 
         this->label_Glb_ModelNum->AutoSize = true;
         this->label_Glb_ModelNum->Location = System::Drawing::Point(60, 66);
         this->label_Glb_ModelNum->Name = L"label_Glb_ModelNum";
         this->label_Glb_ModelNum->Size = System::Drawing::Size(49, 13);
         this->label_Glb_ModelNum->TabIndex = 5;
         this->label_Glb_ModelNum->Text = L"Model #:";
         // 
         // labelMfgID
         // 
         this->labelMfgID->AutoSize = true;
         this->labelMfgID->Location = System::Drawing::Point(115, 47);
         this->labelMfgID->Name = L"labelMfgID";
         this->labelMfgID->Size = System::Drawing::Size(16, 13);
         this->labelMfgID->TabIndex = 8;
         this->labelMfgID->Text = L"---";
         // 
         // label_Glb_SerialNum
         // 
         this->label_Glb_SerialNum->AutoSize = true;
         this->label_Glb_SerialNum->Location = System::Drawing::Point(63, 9);
         this->label_Glb_SerialNum->Name = L"label_Glb_SerialNum";
         this->label_Glb_SerialNum->Size = System::Drawing::Size(46, 13);
         this->label_Glb_SerialNum->TabIndex = 2;
         this->label_Glb_SerialNum->Text = L"Serial #:";
         // 
         // labelModelNum
         // 
         this->labelModelNum->AutoSize = true;
         this->labelModelNum->Location = System::Drawing::Point(115, 66);
         this->labelModelNum->Name = L"labelModelNum";
         this->labelModelNum->Size = System::Drawing::Size(16, 13);
         this->labelModelNum->TabIndex = 9;
         this->labelModelNum->Text = L"---";
         // 
         // labelHwVer
         // 
         this->labelHwVer->AutoSize = true;
         this->labelHwVer->Location = System::Drawing::Point(115, 85);
         this->labelHwVer->Name = L"labelHwVer";
         this->labelHwVer->Size = System::Drawing::Size(16, 13);
         this->labelHwVer->TabIndex = 14;
         this->labelHwVer->Text = L"---";
         // 
         // label_Glb_ManfID
         // 
         this->label_Glb_ManfID->AutoSize = true;
         this->label_Glb_ManfID->Location = System::Drawing::Point(58, 47);
         this->label_Glb_ManfID->Name = L"label_Glb_ManfID";
         this->label_Glb_ManfID->Size = System::Drawing::Size(51, 13);
         this->label_Glb_ManfID->TabIndex = 1;
         this->label_Glb_ManfID->Text = L"Manf. ID:";
         // 
         // tabPage_CustomProfile
         // 
         this->tabPage_CustomProfile->Controls->Add(this->groupBoxAthlete);
         this->tabPage_CustomProfile->Controls->Add(this->numericUpDownActivityLevel);
         this->tabPage_CustomProfile->Controls->Add(this->label2);
         this->tabPage_CustomProfile->Controls->Add(this->numericUpDownProfile);
         this->tabPage_CustomProfile->Controls->Add(this->groupBox2);
         this->tabPage_CustomProfile->Controls->Add(this->label3);
         this->tabPage_CustomProfile->Controls->Add(this->label4);
         this->tabPage_CustomProfile->Controls->Add(this->numericUpDownHeight);
         this->tabPage_CustomProfile->Controls->Add(this->label5);
         this->tabPage_CustomProfile->Controls->Add(this->numericUpDownAge);
         this->tabPage_CustomProfile->Location = System::Drawing::Point(4, 22);
         this->tabPage_CustomProfile->Name = L"tabPage_CustomProfile";
         this->tabPage_CustomProfile->Size = System::Drawing::Size(387, 126);
         this->tabPage_CustomProfile->TabIndex = 6;
         this->tabPage_CustomProfile->Text = L"Custom Profile";
         this->tabPage_CustomProfile->UseVisualStyleBackColor = true;
         // 
         // groupBoxAthlete
         // 
         this->groupBoxAthlete->Controls->Add(this->radioButtonAthleteFalse);
         this->groupBoxAthlete->Controls->Add(this->radioButtonAthleteTrue);
         this->groupBoxAthlete->Location = System::Drawing::Point(267, 48);
         this->groupBoxAthlete->Name = L"groupBoxAthlete";
         this->groupBoxAthlete->Size = System::Drawing::Size(67, 53);
         this->groupBoxAthlete->TabIndex = 64;
         this->groupBoxAthlete->TabStop = false;
         this->groupBoxAthlete->Text = L"Athlete";
         // 
         // radioButtonAthleteFalse
         // 
         this->radioButtonAthleteFalse->AutoSize = true;
         this->radioButtonAthleteFalse->Checked = true;
         this->radioButtonAthleteFalse->Location = System::Drawing::Point(6, 31);
         this->radioButtonAthleteFalse->Name = L"radioButtonAthleteFalse";
         this->radioButtonAthleteFalse->Size = System::Drawing::Size(39, 17);
         this->radioButtonAthleteFalse->TabIndex = 1;
         this->radioButtonAthleteFalse->TabStop = true;
         this->radioButtonAthleteFalse->Text = L"No";
         this->radioButtonAthleteFalse->UseVisualStyleBackColor = true;
         this->radioButtonAthleteFalse->CheckedChanged += gcnew System::EventHandler(this, &WeightScaleDisplay::radioButtonAthleteFalse_CheckChanged);
         // 
         // radioButtonAthleteTrue
         // 
         this->radioButtonAthleteTrue->AutoSize = true;
         this->radioButtonAthleteTrue->Location = System::Drawing::Point(6, 13);
         this->radioButtonAthleteTrue->Name = L"radioButtonAthleteTrue";
         this->radioButtonAthleteTrue->Size = System::Drawing::Size(43, 17);
         this->radioButtonAthleteTrue->TabIndex = 0;
         this->radioButtonAthleteTrue->Text = L"Yes";
         this->radioButtonAthleteTrue->UseVisualStyleBackColor = true;
         this->radioButtonAthleteTrue->CheckedChanged += gcnew System::EventHandler(this, &WeightScaleDisplay::radioButtonAthleteTrue_CheckChanged);
         // 
         // numericUpDownActivityLevel
         // 
         this->numericUpDownActivityLevel->Location = System::Drawing::Point(267, 22);
         this->numericUpDownActivityLevel->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {7, 0, 0, 0});
         this->numericUpDownActivityLevel->Name = L"numericUpDownActivityLevel";
         this->numericUpDownActivityLevel->Size = System::Drawing::Size(40, 20);
         this->numericUpDownActivityLevel->TabIndex = 63;
         this->numericUpDownActivityLevel->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {3, 0, 0, 0});
         this->numericUpDownActivityLevel->ValueChanged += gcnew System::EventHandler(this, &WeightScaleDisplay::numericUpDownActivtyLevel_ValueChanged);
         // 
         // label2
         // 
         this->label2->AutoSize = true;
         this->label2->Location = System::Drawing::Point(14, 6);
         this->label2->Name = L"label2";
         this->label2->Size = System::Drawing::Size(65, 13);
         this->label2->TabIndex = 62;
         this->label2->Text = L"User Prof ID";
         // 
         // numericUpDownProfile
         // 
         this->numericUpDownProfile->Location = System::Drawing::Point(17, 22);
         this->numericUpDownProfile->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {65534, 0, 0, 0});
         this->numericUpDownProfile->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {16, 0, 0, 0});
         this->numericUpDownProfile->Name = L"numericUpDownProfile";
         this->numericUpDownProfile->Size = System::Drawing::Size(60, 20);
         this->numericUpDownProfile->TabIndex = 61;
         this->numericUpDownProfile->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {16, 0, 0, 0});
         this->numericUpDownProfile->ValueChanged += gcnew System::EventHandler(this, &WeightScaleDisplay::numericUpDownProfile_ValueChanged);
         // 
         // groupBox2
         // 
         this->groupBox2->Controls->Add(this->radioButtonFemale);
         this->groupBox2->Controls->Add(this->radioButtonMale);
         this->groupBox2->Location = System::Drawing::Point(17, 48);
         this->groupBox2->Name = L"groupBox2";
         this->groupBox2->Size = System::Drawing::Size(97, 53);
         this->groupBox2->TabIndex = 14;
         this->groupBox2->TabStop = false;
         this->groupBox2->Text = L"Gender";
         // 
         // radioButtonFemale
         // 
         this->radioButtonFemale->AutoSize = true;
         this->radioButtonFemale->Checked = true;
         this->radioButtonFemale->Location = System::Drawing::Point(8, 13);
         this->radioButtonFemale->Name = L"radioButtonFemale";
         this->radioButtonFemale->Size = System::Drawing::Size(59, 17);
         this->radioButtonFemale->TabIndex = 0;
         this->radioButtonFemale->TabStop = true;
         this->radioButtonFemale->Text = L"Female";
         this->radioButtonFemale->UseVisualStyleBackColor = true;
         this->radioButtonFemale->CheckedChanged += gcnew System::EventHandler(this, &WeightScaleDisplay::radioButtonFemale_CheckedChanged);
         // 
         // radioButtonMale
         // 
         this->radioButtonMale->AutoSize = true;
         this->radioButtonMale->Location = System::Drawing::Point(8, 31);
         this->radioButtonMale->Name = L"radioButtonMale";
         this->radioButtonMale->Size = System::Drawing::Size(48, 17);
         this->radioButtonMale->TabIndex = 1;
         this->radioButtonMale->TabStop = true;
         this->radioButtonMale->Text = L"Male";
         this->radioButtonMale->UseVisualStyleBackColor = true;
         this->radioButtonMale->CheckedChanged += gcnew System::EventHandler(this, &WeightScaleDisplay::radioButtonMale_CheckedChanged);
         // 
         // label3
         // 
         this->label3->AutoSize = true;
         this->label3->Location = System::Drawing::Point(264, 6);
         this->label3->Name = L"label3";
         this->label3->Size = System::Drawing::Size(70, 13);
         this->label3->TabIndex = 12;
         this->label3->Text = L"Activity Level";
         // 
         // label4
         // 
         this->label4->AutoSize = true;
         this->label4->Location = System::Drawing::Point(140, 48);
         this->label4->Name = L"label4";
         this->label4->Size = System::Drawing::Size(61, 13);
         this->label4->TabIndex = 10;
         this->label4->Text = L"Height (cm)";
         // 
         // numericUpDownHeight
         // 
         this->numericUpDownHeight->Location = System::Drawing::Point(143, 64);
         this->numericUpDownHeight->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
         this->numericUpDownHeight->Name = L"numericUpDownHeight";
         this->numericUpDownHeight->Size = System::Drawing::Size(60, 20);
         this->numericUpDownHeight->TabIndex = 9;
         this->numericUpDownHeight->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {165, 0, 0, 0});
         this->numericUpDownHeight->ValueChanged += gcnew System::EventHandler(this, &WeightScaleDisplay::numericUpDownHeight_ValueChanged);
         // 
         // label5
         // 
         this->label5->AutoSize = true;
         this->label5->Location = System::Drawing::Point(140, 6);
         this->label5->Name = L"label5";
         this->label5->Size = System::Drawing::Size(60, 13);
         this->label5->TabIndex = 8;
         this->label5->Text = L"Age (years)";
         // 
         // numericUpDownAge
         // 
         this->numericUpDownAge->Location = System::Drawing::Point(142, 22);
         this->numericUpDownAge->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {99, 0, 0, 0});
         this->numericUpDownAge->Name = L"numericUpDownAge";
         this->numericUpDownAge->Size = System::Drawing::Size(60, 20);
         this->numericUpDownAge->TabIndex = 7;
         this->numericUpDownAge->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {22, 0, 0, 0});
         this->numericUpDownAge->ValueChanged += gcnew System::EventHandler(this, &WeightScaleDisplay::numericUpDownAge_ValueChanged);
         // 
         // tabPage_ActiveProfile
         // 
         this->tabPage_ActiveProfile->Controls->Add(this->labelExchangeDisplay);
         this->tabPage_ActiveProfile->Controls->Add(this->label7);
         this->tabPage_ActiveProfile->Controls->Add(this->labelLifestyle);
         this->tabPage_ActiveProfile->Controls->Add(this->label40);
         this->tabPage_ActiveProfile->Controls->Add(this->label156);
         this->tabPage_ActiveProfile->Controls->Add(this->labelProfileSetScale);
         this->tabPage_ActiveProfile->Controls->Add(this->label81);
         this->tabPage_ActiveProfile->Controls->Add(this->labelExchangeScale);
         this->tabPage_ActiveProfile->Controls->Add(this->label79);
         this->tabPage_ActiveProfile->Controls->Add(this->labelAntfsRx);
         this->tabPage_ActiveProfile->Controls->Add(this->label6);
         this->tabPage_ActiveProfile->Controls->Add(this->labelDescriptionActive);
         this->tabPage_ActiveProfile->Controls->Add(this->label_Bat_Status);
         this->tabPage_ActiveProfile->Controls->Add(this->label32);
         this->tabPage_ActiveProfile->Controls->Add(this->labelHeightActive);
         this->tabPage_ActiveProfile->Controls->Add(this->label33);
         this->tabPage_ActiveProfile->Controls->Add(this->labelAgeActive);
         this->tabPage_ActiveProfile->Controls->Add(this->label37);
         this->tabPage_ActiveProfile->Controls->Add(this->labelGenderActive);
         this->tabPage_ActiveProfile->Controls->Add(this->label39);
         this->tabPage_ActiveProfile->Controls->Add(this->labelProfileActive2);
         this->tabPage_ActiveProfile->Location = System::Drawing::Point(4, 22);
         this->tabPage_ActiveProfile->Name = L"tabPage_ActiveProfile";
         this->tabPage_ActiveProfile->Padding = System::Windows::Forms::Padding(3);
         this->tabPage_ActiveProfile->Size = System::Drawing::Size(387, 126);
         this->tabPage_ActiveProfile->TabIndex = 8;
         this->tabPage_ActiveProfile->Text = L"Active Profile";
         this->tabPage_ActiveProfile->UseVisualStyleBackColor = true;
         // 
         // labelExchangeDisplay
         // 
         this->labelExchangeDisplay->AutoSize = true;
         this->labelExchangeDisplay->BackColor = System::Drawing::Color::Transparent;
         this->labelExchangeDisplay->Location = System::Drawing::Point(353, 89);
         this->labelExchangeDisplay->Name = L"labelExchangeDisplay";
         this->labelExchangeDisplay->Size = System::Drawing::Size(16, 13);
         this->labelExchangeDisplay->TabIndex = 84;
         this->labelExchangeDisplay->Text = L"---";
         // 
         // label7
         // 
         this->label7->AutoSize = true;
         this->label7->BackColor = System::Drawing::Color::Transparent;
         this->label7->Location = System::Drawing::Point(175, 89);
         this->label7->Name = L"label7";
         this->label7->Size = System::Drawing::Size(167, 13);
         this->label7->TabIndex = 83;
         this->label7->Text = L"Display Profile Exchange Support:";
         // 
         // labelLifestyle
         // 
         this->labelLifestyle->AutoSize = true;
         this->labelLifestyle->BackColor = System::Drawing::Color::Transparent;
         this->labelLifestyle->Location = System::Drawing::Point(90, 110);
         this->labelLifestyle->Name = L"labelLifestyle";
         this->labelLifestyle->Size = System::Drawing::Size(16, 13);
         this->labelLifestyle->TabIndex = 80;
         this->labelLifestyle->Text = L"---";
         // 
         // label40
         // 
         this->label40->AutoSize = true;
         this->label40->Location = System::Drawing::Point(6, 110);
         this->label40->Name = L"label40";
         this->label40->Size = System::Drawing::Size(48, 13);
         this->label40->TabIndex = 79;
         this->label40->Text = L"Lifestyle:";
         // 
         // label156
         // 
         this->label156->AutoSize = true;
         this->label156->BackColor = System::Drawing::Color::Transparent;
         this->label156->Location = System::Drawing::Point(175, 26);
         this->label156->Name = L"label156";
         this->label156->Size = System::Drawing::Size(83, 13);
         this->label156->TabIndex = 77;
         this->label156->Text = L"User Profile Set:";
         // 
         // labelProfileSetScale
         // 
         this->labelProfileSetScale->AutoSize = true;
         this->labelProfileSetScale->BackColor = System::Drawing::Color::Transparent;
         this->labelProfileSetScale->Location = System::Drawing::Point(353, 26);
         this->labelProfileSetScale->Name = L"labelProfileSetScale";
         this->labelProfileSetScale->Size = System::Drawing::Size(16, 13);
         this->labelProfileSetScale->TabIndex = 78;
         this->labelProfileSetScale->Text = L"---";
         // 
         // label81
         // 
         this->label81->AutoSize = true;
         this->label81->BackColor = System::Drawing::Color::Transparent;
         this->label81->Location = System::Drawing::Point(175, 68);
         this->label81->Name = L"label81";
         this->label81->Size = System::Drawing::Size(160, 13);
         this->label81->TabIndex = 75;
         this->label81->Text = L"Scale Profile Exchange Support:";
         // 
         // labelExchangeScale
         // 
         this->labelExchangeScale->AutoSize = true;
         this->labelExchangeScale->BackColor = System::Drawing::Color::Transparent;
         this->labelExchangeScale->Location = System::Drawing::Point(353, 68);
         this->labelExchangeScale->Name = L"labelExchangeScale";
         this->labelExchangeScale->Size = System::Drawing::Size(16, 13);
         this->labelExchangeScale->TabIndex = 76;
         this->labelExchangeScale->Text = L"---";
         // 
         // label79
         // 
         this->label79->AutoSize = true;
         this->label79->BackColor = System::Drawing::Color::Transparent;
         this->label79->Location = System::Drawing::Point(175, 47);
         this->label79->Name = L"label79";
         this->label79->Size = System::Drawing::Size(88, 13);
         this->label79->TabIndex = 73;
         this->label79->Text = L"ANT-FS Support:";
         // 
         // labelAntfsRx
         // 
         this->labelAntfsRx->AutoSize = true;
         this->labelAntfsRx->BackColor = System::Drawing::Color::Transparent;
         this->labelAntfsRx->Location = System::Drawing::Point(353, 47);
         this->labelAntfsRx->Name = L"labelAntfsRx";
         this->labelAntfsRx->Size = System::Drawing::Size(16, 13);
         this->labelAntfsRx->TabIndex = 74;
         this->labelAntfsRx->Text = L"---";
         // 
         // label6
         // 
         this->label6->AutoSize = true;
         this->label6->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Underline, System::Drawing::GraphicsUnit::Point, 
            static_cast<System::Byte>(0)));
         this->label6->Location = System::Drawing::Point(175, 5);
         this->label6->Name = L"label6";
         this->label6->Size = System::Drawing::Size(92, 13);
         this->label6->TabIndex = 72;
         this->label6->Text = L"Scale capabilities:";
         // 
         // labelDescriptionActive
         // 
         this->labelDescriptionActive->AutoSize = true;
         this->labelDescriptionActive->BackColor = System::Drawing::Color::Transparent;
         this->labelDescriptionActive->Location = System::Drawing::Point(90, 89);
         this->labelDescriptionActive->Name = L"labelDescriptionActive";
         this->labelDescriptionActive->Size = System::Drawing::Size(16, 13);
         this->labelDescriptionActive->TabIndex = 59;
         this->labelDescriptionActive->Text = L"---";
         // 
         // label_Bat_Status
         // 
         this->label_Bat_Status->AutoSize = true;
         this->label_Bat_Status->Location = System::Drawing::Point(6, 89);
         this->label_Bat_Status->Name = L"label_Bat_Status";
         this->label_Bat_Status->Size = System::Drawing::Size(73, 13);
         this->label_Bat_Status->TabIndex = 58;
         this->label_Bat_Status->Text = L"Activity Level:";
         // 
         // label32
         // 
         this->label32->AutoSize = true;
         this->label32->BackColor = System::Drawing::Color::Transparent;
         this->label32->Location = System::Drawing::Point(6, 68);
         this->label32->Name = L"label32";
         this->label32->Size = System::Drawing::Size(64, 13);
         this->label32->TabIndex = 56;
         this->label32->Text = L"Height (cm):";
         // 
         // labelHeightActive
         // 
         this->labelHeightActive->AutoSize = true;
         this->labelHeightActive->BackColor = System::Drawing::Color::Transparent;
         this->labelHeightActive->Location = System::Drawing::Point(90, 68);
         this->labelHeightActive->Name = L"labelHeightActive";
         this->labelHeightActive->Size = System::Drawing::Size(16, 13);
         this->labelHeightActive->TabIndex = 57;
         this->labelHeightActive->Text = L"---";
         // 
         // label33
         // 
         this->label33->AutoSize = true;
         this->label33->BackColor = System::Drawing::Color::Transparent;
         this->label33->Location = System::Drawing::Point(6, 47);
         this->label33->Name = L"label33";
         this->label33->Size = System::Drawing::Size(63, 13);
         this->label33->TabIndex = 54;
         this->label33->Text = L"Age (years):";
         // 
         // labelAgeActive
         // 
         this->labelAgeActive->AutoSize = true;
         this->labelAgeActive->BackColor = System::Drawing::Color::Transparent;
         this->labelAgeActive->Location = System::Drawing::Point(90, 47);
         this->labelAgeActive->Name = L"labelAgeActive";
         this->labelAgeActive->Size = System::Drawing::Size(16, 13);
         this->labelAgeActive->TabIndex = 55;
         this->labelAgeActive->Text = L"---";
         // 
         // label37
         // 
         this->label37->AutoSize = true;
         this->label37->BackColor = System::Drawing::Color::Transparent;
         this->label37->Location = System::Drawing::Point(6, 26);
         this->label37->Name = L"label37";
         this->label37->Size = System::Drawing::Size(45, 13);
         this->label37->TabIndex = 52;
         this->label37->Text = L"Gender:";
         // 
         // labelGenderActive
         // 
         this->labelGenderActive->AutoSize = true;
         this->labelGenderActive->BackColor = System::Drawing::Color::Transparent;
         this->labelGenderActive->Location = System::Drawing::Point(90, 26);
         this->labelGenderActive->Name = L"labelGenderActive";
         this->labelGenderActive->Size = System::Drawing::Size(16, 13);
         this->labelGenderActive->TabIndex = 53;
         this->labelGenderActive->Text = L"---";
         // 
         // label39
         // 
         this->label39->AutoSize = true;
         this->label39->BackColor = System::Drawing::Color::Transparent;
         this->label39->Location = System::Drawing::Point(6, 5);
         this->label39->Name = L"label39";
         this->label39->Size = System::Drawing::Size(78, 13);
         this->label39->TabIndex = 50;
         this->label39->Text = L"User Profile ID:";
         // 
         // labelProfileActive2
         // 
         this->labelProfileActive2->AutoSize = true;
         this->labelProfileActive2->BackColor = System::Drawing::Color::Transparent;
         this->labelProfileActive2->Location = System::Drawing::Point(90, 5);
         this->labelProfileActive2->Name = L"labelProfileActive2";
         this->labelProfileActive2->Size = System::Drawing::Size(16, 13);
         this->labelProfileActive2->TabIndex = 51;
         this->labelProfileActive2->Text = L"---";
         // 
         // panel_Display
         // 
         this->panel_Display->Controls->Add(this->label8);
         this->panel_Display->Controls->Add(this->labelBMR);
         this->panel_Display->Controls->Add(this->label_BasalMetRate);
         this->panel_Display->Controls->Add(this->labelAMR);
         this->panel_Display->Controls->Add(this->label_ActiveMetRate);
         this->panel_Display->Controls->Add(this->label_MR);
         this->panel_Display->Controls->Add(this->labelBF);
         this->panel_Display->Controls->Add(this->label_BodyFat);
         this->panel_Display->Controls->Add(this->labelH);
         this->panel_Display->Controls->Add(this->label_Hydration);
         this->panel_Display->Controls->Add(this->labelPage);
         this->panel_Display->Controls->Add(this->label1);
         this->panel_Display->Controls->Add(this->labelProfileActive);
         this->panel_Display->Controls->Add(this->label_Trn_DisplayTitle);
         this->panel_Display->Controls->Add(this->labelW1);
         this->panel_Display->Controls->Add(this->labelWeight);
         this->panel_Display->Location = System::Drawing::Point(58, 188);
         this->panel_Display->Name = L"panel_Display";
         this->panel_Display->Size = System::Drawing::Size(200, 90);
         this->panel_Display->TabIndex = 1;
         // 
         // label8
         // 
         this->label8->AutoSize = true;
         this->label8->Location = System::Drawing::Point(139, 2);
         this->label8->Name = L"label8";
         this->label8->Size = System::Drawing::Size(32, 13);
         this->label8->TabIndex = 39;
         this->label8->Text = L"Page";
         // 
         // labelBMR
         // 
         this->labelBMR->AutoSize = true;
         this->labelBMR->BackColor = System::Drawing::Color::Transparent;
         this->labelBMR->Location = System::Drawing::Point(117, 55);
         this->labelBMR->Name = L"labelBMR";
         this->labelBMR->Size = System::Drawing::Size(36, 13);
         this->labelBMR->TabIndex = 37;
         this->labelBMR->Text = L"Basal:";
         // 
         // label_BasalMetRate
         // 
         this->label_BasalMetRate->AutoSize = true;
         this->label_BasalMetRate->BackColor = System::Drawing::Color::Transparent;
         this->label_BasalMetRate->Location = System::Drawing::Point(156, 55);
         this->label_BasalMetRate->Name = L"label_BasalMetRate";
         this->label_BasalMetRate->Size = System::Drawing::Size(16, 13);
         this->label_BasalMetRate->TabIndex = 38;
         this->label_BasalMetRate->Text = L"---";
         // 
         // labelAMR
         // 
         this->labelAMR->AutoSize = true;
         this->labelAMR->BackColor = System::Drawing::Color::Transparent;
         this->labelAMR->Location = System::Drawing::Point(117, 39);
         this->labelAMR->Name = L"labelAMR";
         this->labelAMR->Size = System::Drawing::Size(40, 13);
         this->labelAMR->TabIndex = 35;
         this->labelAMR->Text = L"Active:";
         // 
         // label_ActiveMetRate
         // 
         this->label_ActiveMetRate->AutoSize = true;
         this->label_ActiveMetRate->BackColor = System::Drawing::Color::Transparent;
         this->label_ActiveMetRate->Location = System::Drawing::Point(156, 39);
         this->label_ActiveMetRate->Name = L"label_ActiveMetRate";
         this->label_ActiveMetRate->Size = System::Drawing::Size(16, 13);
         this->label_ActiveMetRate->TabIndex = 36;
         this->label_ActiveMetRate->Text = L"---";
         // 
         // label_MR
         // 
         this->label_MR->AutoSize = true;
         this->label_MR->Location = System::Drawing::Point(119, 22);
         this->label_MR->Name = L"label_MR";
         this->label_MR->Size = System::Drawing::Size(80, 13);
         this->label_MR->TabIndex = 34;
         this->label_MR->Text = L"Met Rate (kcal)";
         // 
         // labelBF
         // 
         this->labelBF->AutoSize = true;
         this->labelBF->BackColor = System::Drawing::Color::Transparent;
         this->labelBF->Location = System::Drawing::Point(9, 55);
         this->labelBF->Name = L"labelBF";
         this->labelBF->Size = System::Drawing::Size(69, 13);
         this->labelBF->TabIndex = 32;
         this->labelBF->Text = L"Body Fat (%):";
         // 
         // label_BodyFat
         // 
         this->label_BodyFat->AutoSize = true;
         this->label_BodyFat->BackColor = System::Drawing::Color::Transparent;
         this->label_BodyFat->Location = System::Drawing::Point(82, 56);
         this->label_BodyFat->Name = L"label_BodyFat";
         this->label_BodyFat->Size = System::Drawing::Size(16, 13);
         this->label_BodyFat->TabIndex = 33;
         this->label_BodyFat->Text = L"---";
         // 
         // labelH
         // 
         this->labelH->AutoSize = true;
         this->labelH->BackColor = System::Drawing::Color::Transparent;
         this->labelH->Location = System::Drawing::Point(9, 38);
         this->labelH->Name = L"labelH";
         this->labelH->Size = System::Drawing::Size(72, 13);
         this->labelH->TabIndex = 30;
         this->labelH->Text = L"Hydration (%):";
         // 
         // label_Hydration
         // 
         this->label_Hydration->AutoSize = true;
         this->label_Hydration->BackColor = System::Drawing::Color::Transparent;
         this->label_Hydration->Location = System::Drawing::Point(82, 39);
         this->label_Hydration->Name = L"label_Hydration";
         this->label_Hydration->Size = System::Drawing::Size(16, 13);
         this->label_Hydration->TabIndex = 31;
         this->label_Hydration->Text = L"---";
         // 
         // labelPage
         // 
         this->labelPage->AutoSize = true;
         this->labelPage->BackColor = System::Drawing::Color::Transparent;
         this->labelPage->Location = System::Drawing::Point(175, 2);
         this->labelPage->Name = L"labelPage";
         this->labelPage->Size = System::Drawing::Size(16, 13);
         this->labelPage->TabIndex = 29;
         this->labelPage->Text = L"---";
         // 
         // label1
         // 
         this->label1->AutoSize = true;
         this->label1->BackColor = System::Drawing::Color::Transparent;
         this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Italic, System::Drawing::GraphicsUnit::Point, 
            static_cast<System::Byte>(0)));
         this->label1->Location = System::Drawing::Point(7, 73);
         this->label1->Name = L"label1";
         this->label1->Size = System::Drawing::Size(111, 13);
         this->label1->TabIndex = 26;
         this->label1->Text = L"Active User Profile ID:";
         // 
         // labelProfileActive
         // 
         this->labelProfileActive->AutoSize = true;
         this->labelProfileActive->BackColor = System::Drawing::Color::Transparent;
         this->labelProfileActive->Location = System::Drawing::Point(129, 73);
         this->labelProfileActive->Name = L"labelProfileActive";
         this->labelProfileActive->Size = System::Drawing::Size(16, 13);
         this->labelProfileActive->TabIndex = 27;
         this->labelProfileActive->Text = L"---";
         // 
         // label_Trn_DisplayTitle
         // 
         this->label_Trn_DisplayTitle->AutoSize = true;
         this->label_Trn_DisplayTitle->BackColor = System::Drawing::Color::Transparent;
         this->label_Trn_DisplayTitle->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Underline, 
            System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
         this->label_Trn_DisplayTitle->Location = System::Drawing::Point(5, 2);
         this->label_Trn_DisplayTitle->Name = L"label_Trn_DisplayTitle";
         this->label_Trn_DisplayTitle->Size = System::Drawing::Size(126, 13);
         this->label_Trn_DisplayTitle->TabIndex = 21;
         this->label_Trn_DisplayTitle->Text = L"Current Weight Scale Tx:";
         // 
         // labelW1
         // 
         this->labelW1->AutoSize = true;
         this->labelW1->BackColor = System::Drawing::Color::Transparent;
         this->labelW1->Location = System::Drawing::Point(9, 21);
         this->labelW1->Name = L"labelW1";
         this->labelW1->Size = System::Drawing::Size(65, 13);
         this->labelW1->TabIndex = 19;
         this->labelW1->Text = L"Weight (kg):";
         // 
         // labelWeight
         // 
         this->labelWeight->AutoSize = true;
         this->labelWeight->BackColor = System::Drawing::Color::Transparent;
         this->labelWeight->Location = System::Drawing::Point(81, 21);
         this->labelWeight->Name = L"labelWeight";
         this->labelWeight->Size = System::Drawing::Size(16, 13);
         this->labelWeight->TabIndex = 20;
         this->labelWeight->Text = L"---";
         // 
         // tabPage_UserProfile
         // 
         this->tabPage_UserProfile->Controls->Add(this->groupBox1);
         this->tabPage_UserProfile->Controls->Add(this->checkBoxUPSet);
         this->tabPage_UserProfile->Controls->Add(this->label29);
         this->tabPage_UserProfile->Controls->Add(this->comboBoxActivityLevel);
         this->tabPage_UserProfile->Controls->Add(this->label28);
         this->tabPage_UserProfile->Controls->Add(this->label27);
         this->tabPage_UserProfile->Location = System::Drawing::Point(4, 22);
         this->tabPage_UserProfile->Name = L"tabPage_UserProfile";
         this->tabPage_UserProfile->Size = System::Drawing::Size(388, 110);
         this->tabPage_UserProfile->TabIndex = 3;
         this->tabPage_UserProfile->Text = L"User Profile";
         this->tabPage_UserProfile->UseVisualStyleBackColor = true;
         // 
         // groupBox1
         // 
         this->groupBox1->Location = System::Drawing::Point(31, 37);
         this->groupBox1->Name = L"groupBox1";
         this->groupBox1->Size = System::Drawing::Size(87, 57);
         this->groupBox1->TabIndex = 14;
         this->groupBox1->TabStop = false;
         this->groupBox1->Text = L"Gender";
         // 
         // checkBoxUPSet
         // 
         this->checkBoxUPSet->AutoSize = true;
         this->checkBoxUPSet->Location = System::Drawing::Point(31, 14);
         this->checkBoxUPSet->Name = L"checkBoxUPSet";
         this->checkBoxUPSet->Size = System::Drawing::Size(99, 17);
         this->checkBoxUPSet->TabIndex = 13;
         this->checkBoxUPSet->Text = L"User Profile Set";
         this->checkBoxUPSet->UseVisualStyleBackColor = true;
         // 
         // label29
         // 
         this->label29->AutoSize = true;
         this->label29->Location = System::Drawing::Point(168, 79);
         this->label29->Name = L"label29";
         this->label29->Size = System::Drawing::Size(70, 13);
         this->label29->TabIndex = 12;
         this->label29->Text = L"Activity Level";
         // 
         // comboBoxActivityLevel
         // 
         this->comboBoxActivityLevel->FormattingEnabled = true;
         this->comboBoxActivityLevel->Items->AddRange(gcnew cli::array< System::Object^  >(6) {L"Child", L"Sedentary", L"Low Activity", 
            L"Medium Activity", L"High Activity", L"Intense Activity"});
         this->comboBoxActivityLevel->Location = System::Drawing::Point(259, 73);
         this->comboBoxActivityLevel->Name = L"comboBoxActivityLevel";
         this->comboBoxActivityLevel->Size = System::Drawing::Size(114, 21);
         this->comboBoxActivityLevel->TabIndex = 11;
         // 
         // label28
         // 
         this->label28->AutoSize = true;
         this->label28->Location = System::Drawing::Point(168, 48);
         this->label28->Name = L"label28";
         this->label28->Size = System::Drawing::Size(38, 13);
         this->label28->TabIndex = 10;
         this->label28->Text = L"Height";
         // 
         // label27
         // 
         this->label27->AutoSize = true;
         this->label27->Location = System::Drawing::Point(168, 20);
         this->label27->Name = L"label27";
         this->label27->Size = System::Drawing::Size(26, 13);
         this->label27->TabIndex = 8;
         this->label27->Text = L"Age";
         // 
         // tabPage_GlobalData
         // 
         this->tabPage_GlobalData->Controls->Add(this->label12);
         this->tabPage_GlobalData->Controls->Add(this->label13);
         this->tabPage_GlobalData->Controls->Add(this->label14);
         this->tabPage_GlobalData->Controls->Add(this->label15);
         this->tabPage_GlobalData->Controls->Add(this->label16);
         this->tabPage_GlobalData->Controls->Add(this->checkBox3);
         this->tabPage_GlobalData->Controls->Add(this->checkBox4);
         this->tabPage_GlobalData->Controls->Add(this->button2);
         this->tabPage_GlobalData->Controls->Add(this->label17);
         this->tabPage_GlobalData->Controls->Add(this->label18);
         this->tabPage_GlobalData->Controls->Add(this->label19);
         this->tabPage_GlobalData->Controls->Add(this->textboxSerialNum);
         this->tabPage_GlobalData->Controls->Add(this->textboxSwVer);
         this->tabPage_GlobalData->Controls->Add(this->label20);
         this->tabPage_GlobalData->Controls->Add(this->label21);
         this->tabPage_GlobalData->Controls->Add(this->label22);
         this->tabPage_GlobalData->Controls->Add(this->textboxHwVer);
         this->tabPage_GlobalData->Controls->Add(this->textboxModelNum);
         this->tabPage_GlobalData->Controls->Add(this->textboxMfgID);
         this->tabPage_GlobalData->Location = System::Drawing::Point(4, 22);
         this->tabPage_GlobalData->Name = L"tabPage_GlobalData";
         this->tabPage_GlobalData->Padding = System::Windows::Forms::Padding(3);
         this->tabPage_GlobalData->Size = System::Drawing::Size(388, 110);
         this->tabPage_GlobalData->TabIndex = 2;
         this->tabPage_GlobalData->Text = L"Global Data";
         this->tabPage_GlobalData->UseVisualStyleBackColor = true;
         // 
         // label12
         // 
         this->label12->Location = System::Drawing::Point(275, 51);
         this->label12->Name = L"label12";
         this->label12->Size = System::Drawing::Size(27, 13);
         this->label12->TabIndex = 16;
         this->label12->Text = L"1";
         // 
         // label13
         // 
         this->label13->Location = System::Drawing::Point(228, 27);
         this->label13->Name = L"label13";
         this->label13->Size = System::Drawing::Size(71, 13);
         this->label13->TabIndex = 11;
         this->label13->Text = L"1234567890";
         // 
         // label14
         // 
         this->label14->Location = System::Drawing::Point(81, 76);
         this->label14->Name = L"label14";
         this->label14->Size = System::Drawing::Size(27, 13);
         this->label14->TabIndex = 14;
         this->label14->Text = L"1";
         // 
         // label15
         // 
         this->label15->Location = System::Drawing::Point(67, 52);
         this->label15->Name = L"label15";
         this->label15->Size = System::Drawing::Size(42, 13);
         this->label15->TabIndex = 9;
         this->label15->Text = L"33669";
         // 
         // label16
         // 
         this->label16->Location = System::Drawing::Point(69, 27);
         this->label16->Name = L"label16";
         this->label16->Size = System::Drawing::Size(38, 13);
         this->label16->TabIndex = 8;
         this->label16->Text = L"2";
         // 
         // checkBox3
         // 
         this->checkBox3->AutoSize = true;
         this->checkBox3->Checked = true;
         this->checkBox3->CheckState = System::Windows::Forms::CheckState::Checked;
         this->checkBox3->Enabled = false;
         this->checkBox3->Location = System::Drawing::Point(205, 6);
         this->checkBox3->Name = L"checkBox3";
         this->checkBox3->Size = System::Drawing::Size(148, 17);
         this->checkBox3->TabIndex = 46;
         this->checkBox3->Text = L"Enable Product Info Page";
         this->checkBox3->UseVisualStyleBackColor = true;
         // 
         // checkBox4
         // 
         this->checkBox4->AutoSize = true;
         this->checkBox4->Checked = true;
         this->checkBox4->CheckState = System::Windows::Forms::CheckState::Checked;
         this->checkBox4->Enabled = false;
         this->checkBox4->Location = System::Drawing::Point(6, 6);
         this->checkBox4->Name = L"checkBox4";
         this->checkBox4->Size = System::Drawing::Size(184, 17);
         this->checkBox4->TabIndex = 42;
         this->checkBox4->Text = L"Enable Manufacturing Data Page";
         this->checkBox4->UseVisualStyleBackColor = true;
         // 
         // button2
         // 
         this->button2->Location = System::Drawing::Point(179, 74);
         this->button2->Name = L"button2";
         this->button2->Size = System::Drawing::Size(97, 20);
         this->button2->TabIndex = 49;
         this->button2->Text = L"Update All";
         this->button2->UseVisualStyleBackColor = true;
         // 
         // label17
         // 
         this->label17->AutoSize = true;
         this->label17->Location = System::Drawing::Point(180, 95);
         this->label17->Name = L"label17";
         this->label17->Size = System::Drawing::Size(32, 13);
         this->label17->TabIndex = 20;
         this->label17->Text = L"Error:";
         this->label17->Visible = false;
         // 
         // label18
         // 
         this->label18->AutoSize = true;
         this->label18->Location = System::Drawing::Point(198, 51);
         this->label18->Name = L"label18";
         this->label18->Size = System::Drawing::Size(71, 13);
         this->label18->TabIndex = 4;
         this->label18->Text = L"Software Ver:";
         // 
         // label19
         // 
         this->label19->AutoSize = true;
         this->label19->Location = System::Drawing::Point(6, 76);
         this->label19->Name = L"label19";
         this->label19->Size = System::Drawing::Size(75, 13);
         this->label19->TabIndex = 3;
         this->label19->Text = L"Hardware Ver:";
         // 
         // textboxSerialNum
         // 
         this->textboxSerialNum->Location = System::Drawing::Point(303, 24);
         this->textboxSerialNum->MaxLength = 10;
         this->textboxSerialNum->Name = L"textboxSerialNum";
         this->textboxSerialNum->Size = System::Drawing::Size(76, 20);
         this->textboxSerialNum->TabIndex = 47;
         this->textboxSerialNum->Text = L"1234567890";
         // 
         // textboxSwVer
         // 
         this->textboxSwVer->Location = System::Drawing::Point(303, 48);
         this->textboxSwVer->MaxLength = 3;
         this->textboxSwVer->Name = L"textboxSwVer";
         this->textboxSwVer->Size = System::Drawing::Size(29, 20);
         this->textboxSwVer->TabIndex = 48;
         this->textboxSwVer->Text = L"1";
         // 
         // label20
         // 
         this->label20->AutoSize = true;
         this->label20->Location = System::Drawing::Point(176, 27);
         this->label20->Name = L"label20";
         this->label20->Size = System::Drawing::Size(46, 13);
         this->label20->TabIndex = 2;
         this->label20->Text = L"Serial #:";
         // 
         // label21
         // 
         this->label21->AutoSize = true;
         this->label21->Location = System::Drawing::Point(18, 27);
         this->label21->Name = L"label21";
         this->label21->Size = System::Drawing::Size(51, 13);
         this->label21->TabIndex = 1;
         this->label21->Text = L"Manf. ID:";
         // 
         // label22
         // 
         this->label22->AutoSize = true;
         this->label22->Location = System::Drawing::Point(18, 52);
         this->label22->Name = L"label22";
         this->label22->Size = System::Drawing::Size(49, 13);
         this->label22->TabIndex = 5;
         this->label22->Text = L"Model #:";
         // 
         // textboxHwVer
         // 
         this->textboxHwVer->Location = System::Drawing::Point(110, 73);
         this->textboxHwVer->MaxLength = 3;
         this->textboxHwVer->Name = L"textboxHwVer";
         this->textboxHwVer->Size = System::Drawing::Size(29, 20);
         this->textboxHwVer->TabIndex = 45;
         this->textboxHwVer->Text = L"1";
         // 
         // textboxModelNum
         // 
         this->textboxModelNum->Location = System::Drawing::Point(110, 49);
         this->textboxModelNum->MaxLength = 5;
         this->textboxModelNum->Name = L"textboxModelNum";
         this->textboxModelNum->Size = System::Drawing::Size(49, 20);
         this->textboxModelNum->TabIndex = 44;
         this->textboxModelNum->Text = L"33669";
         // 
         // textboxMfgID
         // 
         this->textboxMfgID->Location = System::Drawing::Point(110, 24);
         this->textboxMfgID->MaxLength = 5;
         this->textboxMfgID->Name = L"textboxMfgID";
         this->textboxMfgID->Size = System::Drawing::Size(49, 20);
         this->textboxMfgID->TabIndex = 43;
         this->textboxMfgID->Text = L"2";
         // 
         // WeightScaleDisplay
         // 
         this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
         this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
         this->ClientSize = System::Drawing::Size(794, 351);
         this->Controls->Add(this->panel_Display);
         this->Controls->Add(this->panel_Settings);
         this->Name = L"WeightScaleDisplay";
         this->Text = L"Weight Scale";
         this->panel_Settings->ResumeLayout(false);
         this->tabControl1->ResumeLayout(false);
         this->tabPage2->ResumeLayout(false);
         this->tabPage2->PerformLayout();
         this->tabPage_DataPages->ResumeLayout(false);
         this->tabPage_DataPages->PerformLayout();
         this->tabPage_GlobalBattery->ResumeLayout(false);
         this->tabPage_GlobalBattery->PerformLayout();
         this->tabPage_CustomProfile->ResumeLayout(false);
         this->tabPage_CustomProfile->PerformLayout();
         this->groupBoxAthlete->ResumeLayout(false);
         this->groupBoxAthlete->PerformLayout();
         (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownActivityLevel))->EndInit();
         (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownProfile))->EndInit();
         this->groupBox2->ResumeLayout(false);
         this->groupBox2->PerformLayout();
         (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownHeight))->EndInit();
         (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownAge))->EndInit();
         this->tabPage_ActiveProfile->ResumeLayout(false);
         this->tabPage_ActiveProfile->PerformLayout();
         this->panel_Display->ResumeLayout(false);
         this->panel_Display->PerformLayout();
         this->tabPage_UserProfile->ResumeLayout(false);
         this->tabPage_UserProfile->PerformLayout();
         this->tabPage_GlobalData->ResumeLayout(false);
         this->tabPage_GlobalData->PerformLayout();
         this->ResumeLayout(false);

      }
#pragma endregion




private:
System::Void WeightScaleDisplay::numericUpDownProfile_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	SelectCustomProfile();
}

System::Void WeightScaleDisplay::radioButtonFemale_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	SelectCustomProfile();
}

System::Void WeightScaleDisplay::radioButtonMale_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	SelectCustomProfile();
}

System::Void WeightScaleDisplay::numericUpDownAge_ValueChanged(System::Object^  sender, System::EventArgs^  e) 
{	
	SelectCustomProfile(); 
}

System::Void WeightScaleDisplay::numericUpDownHeight_ValueChanged(System::Object^  sender, System::EventArgs^  e) 
{	
	SelectCustomProfile();
}

System::Void WeightScaleDisplay::numericUpDownActivtyLevel_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	SelectCustomProfile();
}

System::Void WeightScaleDisplay::radioButtonAthleteTrue_CheckChanged(System::Object^ sender, System::EventArgs^ e)
{
	SelectCustomProfile();
}

System::Void WeightScaleDisplay::radioButtonAthleteFalse_CheckChanged(System::Object^ sender, System::EventArgs^ e)
{
	SelectCustomProfile();
}
};