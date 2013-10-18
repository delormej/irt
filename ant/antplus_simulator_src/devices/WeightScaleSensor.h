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
#include "antplus_weight.h"
#include "antplus_common.h"
#include "types.h"
#include "antdefines.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


public ref class WeightScaleSensor : public System::Windows::Forms::Form, public ISimBase{
	public:
		WeightScaleSensor(System::Timers::Timer^ channelTimer){
			commonPages = gcnew CommonData();
			wsPages = gcnew WeightScale(WeightScale::DeviceClass::SCALE);
			InitializeComponent();
			timerHandle = channelTimer;
			InitializeSim();
		}

		~WeightScaleSensor(){
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
		virtual UCHAR getTransmissionType(){return WeightScale::TX_TYPE;}
		virtual USHORT getTransmitPeriod(){return WeightScale::MSG_PERIOD;}
		virtual DOUBLE getTimerInterval(){return 1000;} // Set interval to 1 second, just to keep track of time
		virtual void onTimerTock(USHORT eventTime){} // Do nothing
		virtual System::Windows::Forms::Panel^ getSimSettingsPanel(){return this->panel_Settings;}
		virtual System::Windows::Forms::Panel^ getSimTranslatedDisplay(){return this->panel_Display;}

	private:
		void InitializeSim();
		void HandleTransmit(UCHAR* pucTxBuffer_);
		void HandleReceive(UCHAR* pucRxBuffer_);
		void ProcessReceivedProfile();
		void UpdateDisplay();
		void UpdateDisplayActiveProfile();		
		void UpdateDisplayCapabilities();
		void SelectPresetProfile();
		void SelectWeightStatus();
		void SelectPages();
		void SelectCustomProfile();
		void AllowChangesProfile(BOOL bNoSession_);
		System::Void checkBoxProfileSet_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		System::Void checkBoxAntfs_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		System::Void checkBoxExchange_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		System::Void numericUpDownWeight_ValueChanged(System::Object^  sender, System::EventArgs^  e);
		System::Void numericUpDown_Hydration_ValueChanged(System::Object^  sender, System::EventArgs^  e);
		System::Void numericUpDown_BodyFat_ValueChanged(System::Object^  sender, System::EventArgs^  e);
		System::Void numericUpDown_ActiveMetRate_ValueChanged(System::Object^  sender, System::EventArgs^  e);
		System::Void numericUpDown_BasalMetRate_ValueChanged(System::Object^  sender, System::EventArgs^  e);
      System::Void numericUpDown_MuscleMass_ValueChanged(System::Object^  sender, System::EventArgs^  e);
      System::Void numericUpDown_BoneMass_ValueChanged(System::Object^  sender, System::EventArgs^  e);
		System::Void buttonUpdateGlobal_Click(System::Object^  sender, System::EventArgs^  e);
		System::Void buttonReset_Click(System::Object^  sender, System::EventArgs^  e);
		


private:
		CommonData^ commonPages;			// Handle to common data pages
		WeightScale^ wsPages;				// Handle to weight scale pages
		System::Timers::Timer^ timerHandle;
		dRequestAckMsg^ requestAckMsg;
		
		// Preset User Profiles
		WeightScaleUserProfile^ uprofUndef;
		WeightScaleUserProfile^ uprofU1;
		WeightScaleUserProfile^ uprofU2;
		WeightScaleUserProfile^ uprofU3;
				
		// Status
		BOOL bTxAdvanced;
		BOOL bTxUserProfile;
		BOOL bTxPage2;
		BOOL bTxPage3;
      BOOL bTxPage4;
		
private: System::Windows::Forms::Button^  buttonUpdateGlobal;
private: System::Windows::Forms::CheckBox^  checkBoxProd;
private: System::Windows::Forms::CheckBox^  checkBoxMfg;
private: System::Windows::Forms::CheckBox^  checkBoxProfileSet;
private: System::Windows::Forms::CheckBox^  checkBoxAntfs;
private: System::Windows::Forms::CheckBox^  checkBoxExchange;
private: System::Windows::Forms::NumericUpDown^  numericUpDownAge;
private: System::Windows::Forms::NumericUpDown^  numericUpDownHeight;
private: System::Windows::Forms::NumericUpDown^  numericUpDownWeight;
private: System::Windows::Forms::NumericUpDown^  numericUpDownProfile;
private: System::Windows::Forms::RadioButton^  radioButtonUser3;
private: System::Windows::Forms::RadioButton^  radioButtonUser2;
private: System::Windows::Forms::RadioButton^  radioButtonUser1;
private: System::Windows::Forms::RadioButton^  radioButtonFemale;
private: System::Windows::Forms::RadioButton^  radioButtonMale;
private: System::Windows::Forms::Label^  labelStatusProfileSent;
private: System::Windows::Forms::Label^  labelStatusProfileUpdate;
private: System::Windows::Forms::Label^  labelStatusProfileMatch;
private: System::Windows::Forms::Label^  labelStatusProfileNew;
private: System::Windows::Forms::Label^  labelAgeActive;
private: System::Windows::Forms::Label^  labelGenderActive;
private: System::Windows::Forms::Label^  labelHeightActive;
private: System::Windows::Forms::Label^  labelStatusProfileRx;
private: System::Windows::Forms::Label^  labelProfileActive2;
private: System::Windows::Forms::Label^  labelProfileActive;
private: System::Windows::Forms::Label^  labelWeight;
private: System::Windows::Forms::Label^  labelProfileSetScale;
private: System::Windows::Forms::Label^  labelExchangeScale;
private: System::Windows::Forms::Label^  labelAntfsRx;
private: System::Windows::Forms::Label^  labelDescriptionActive;
private: System::Windows::Forms::Label^  labelError;
private: System::Windows::Forms::Label^  label_Bat_Status;
private: System::Windows::Forms::Label^  label_Glb_GlobalDataError;
private: System::Windows::Forms::Label^  label_Glb_HardwareVer;
private: System::Windows::Forms::Label^  label_Glb_HardwareVerDisplay;
private: System::Windows::Forms::Label^  label_Glb_ManfID;
private: System::Windows::Forms::Label^  label_Glb_ManfIDDisplay;
private: System::Windows::Forms::Label^  label_Glb_ModelNum;
private: System::Windows::Forms::Label^  label_Glb_ModelNumDisplay;
private: System::Windows::Forms::Label^  label_Glb_SerialNum;
private: System::Windows::Forms::Label^  label_Glb_SerialNumDisplay;
private: System::Windows::Forms::Label^  label_Glb_SoftwareVer;
private: System::Windows::Forms::Label^  label_Glb_SoftwareVerDisplay;
private: System::Windows::Forms::Label^  label_Trn_DisplayTitle;
private: System::Windows::Forms::Label^  label1;
private: System::Windows::Forms::Label^  label10;
private: System::Windows::Forms::Label^  label11;
private: System::Windows::Forms::Label^  label18;
private: System::Windows::Forms::Label^  label19;
private: System::Windows::Forms::Label^  label2;
private: System::Windows::Forms::Label^  label20;
private: System::Windows::Forms::Label^  label21;
private: System::Windows::Forms::Label^  label22;
private: System::Windows::Forms::Label^  label27;
private: System::Windows::Forms::Label^  label28;
private: System::Windows::Forms::Label^  label29;
private: System::Windows::Forms::Label^  label3;
private: System::Windows::Forms::Label^  label31;
private: System::Windows::Forms::Label^  label32;
private: System::Windows::Forms::Label^  label33;
private: System::Windows::Forms::Label^  label37;
private: System::Windows::Forms::Label^  label39;
private: System::Windows::Forms::Label^  label4;
private: System::Windows::Forms::Label^  label5;
private: System::Windows::Forms::Label^  label6;
private: System::Windows::Forms::Label^  label7;
private: System::Windows::Forms::Label^  label8;
private: System::Windows::Forms::Label^  label9;
private: System::Windows::Forms::Label^  labelW1;
private: System::Windows::Forms::Panel^  panel_Display;
private: System::Windows::Forms::Panel^  panel_Settings;
private: System::Windows::Forms::TabControl^  tabControl1;
private: System::Windows::Forms::TabPage^  tabPage_GlobalData;
private: System::Windows::Forms::TabPage^  tabPage_Main;
private: System::Windows::Forms::TabPage^  tabPage_CustomProfile;
private: System::Windows::Forms::TabPage^  tabPage_ActiveProfile;
private: System::Windows::Forms::TextBox^  textboxHwVer;
private: System::Windows::Forms::TextBox^  textboxMfgID;
private: System::Windows::Forms::TextBox^  textboxModelNum;
private: System::Windows::Forms::TextBox^  textboxSerialNum;
private: System::Windows::Forms::TextBox^  textboxSwVer;
private: System::Windows::Forms::Label^  label24;
private: System::Windows::Forms::Label^  label156;
private: System::Windows::Forms::Label^  label81;
private: System::Windows::Forms::Label^  label79;
private: System::Windows::Forms::Label^  label17;
private: System::Windows::Forms::GroupBox^  groupBox1;
private: System::Windows::Forms::GroupBox^  groupBox2;
private: System::Windows::Forms::NumericUpDown^  numericUpDownActivityLevel;
private: System::Windows::Forms::GroupBox^  groupBox6;
private: System::Windows::Forms::RadioButton^  radioButtonWeightValid;
private: System::Windows::Forms::RadioButton^  radioButtonWeightError;
private: System::Windows::Forms::RadioButton^  radioButtonWieghtComputing;
private: System::Windows::Forms::RadioButton^  radioButtonGuest;
private: System::Windows::Forms::GroupBox^  groupBoxLifetimeActivity;
private: System::Windows::Forms::RadioButton^  radioButtonLifetimeActivityFalse;
private: System::Windows::Forms::RadioButton^  radioButtonLifetimeActivityTrue;
private: System::Windows::Forms::Label^  labelLifestyle;
private: System::Windows::Forms::Label^  label40;
private: System::Windows::Forms::Button^  buttonReset;
private: System::Windows::Forms::Label^  labelExchangeDisplay;
private: System::Windows::Forms::Label^  label12;
private: System::Windows::Forms::TabPage^  tabPage_Advanced;
private: System::Windows::Forms::CheckBox^  checkBox_TxPage2;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_Hydration;
private: System::Windows::Forms::Label^  label_Disp_Hydration;
private: System::Windows::Forms::CheckBox^  checkBox_TxPage3;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_BodyFat;
private: System::Windows::Forms::Label^  label_Disp_BodyFat;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_ActiveMetRate;
private: System::Windows::Forms::Label^  label_Disp_Active;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_BasalMetRate;
private: System::Windows::Forms::Label^  label_Disp_Basal;
private: System::Windows::Forms::Label^  label_MetRate;
private: System::Windows::Forms::Label^  labelBF;
private: System::Windows::Forms::Label^  label_BodyFat;
private: System::Windows::Forms::Label^  labelH;
private: System::Windows::Forms::Label^  label_Hydration;
private: System::Windows::Forms::Label^  labelBMR;
private: System::Windows::Forms::Label^  label_BasalMetRate;
private: System::Windows::Forms::Label^  labelAMR;
private: System::Windows::Forms::Label^  label_ActiveMetRate;
private: System::Windows::Forms::Label^  label_MR;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_BoneMass;
private: System::Windows::Forms::Label^  label13;
private: System::Windows::Forms::Label^  label14;
private: System::Windows::Forms::NumericUpDown^  numericUpDown_MuscleMass;
private: System::Windows::Forms::Label^  label15;
private: System::Windows::Forms::CheckBox^  checkBox_TxPage4;
private: System::Windows::Forms::CheckBox^  checkBox_InvalidSerial;

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
			this->tabPage_Main = (gcnew System::Windows::Forms::TabPage());
			this->buttonReset = (gcnew System::Windows::Forms::Button());
			this->groupBox6 = (gcnew System::Windows::Forms::GroupBox());
			this->radioButtonWeightError = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonWieghtComputing = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonWeightValid = (gcnew System::Windows::Forms::RadioButton());
			this->numericUpDownWeight = (gcnew System::Windows::Forms::NumericUpDown());
			this->labelStatusProfileSent = (gcnew System::Windows::Forms::Label());
			this->labelStatusProfileUpdate = (gcnew System::Windows::Forms::Label());
			this->labelStatusProfileMatch = (gcnew System::Windows::Forms::Label());
			this->labelStatusProfileNew = (gcnew System::Windows::Forms::Label());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->radioButtonGuest = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonUser3 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonUser2 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonUser1 = (gcnew System::Windows::Forms::RadioButton());
			this->checkBoxProfileSet = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxExchange = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxAntfs = (gcnew System::Windows::Forms::CheckBox());
			this->labelStatusProfileRx = (gcnew System::Windows::Forms::Label());
			this->tabPage_Advanced = (gcnew System::Windows::Forms::TabPage());
			this->numericUpDown_BoneMass = (gcnew System::Windows::Forms::NumericUpDown());
			this->label13 = (gcnew System::Windows::Forms::Label());
			this->label14 = (gcnew System::Windows::Forms::Label());
			this->numericUpDown_MuscleMass = (gcnew System::Windows::Forms::NumericUpDown());
			this->label15 = (gcnew System::Windows::Forms::Label());
			this->checkBox_TxPage4 = (gcnew System::Windows::Forms::CheckBox());
			this->numericUpDown_BasalMetRate = (gcnew System::Windows::Forms::NumericUpDown());
			this->label_Disp_Basal = (gcnew System::Windows::Forms::Label());
			this->label_MetRate = (gcnew System::Windows::Forms::Label());
			this->numericUpDown_ActiveMetRate = (gcnew System::Windows::Forms::NumericUpDown());
			this->label_Disp_Active = (gcnew System::Windows::Forms::Label());
			this->checkBox_TxPage3 = (gcnew System::Windows::Forms::CheckBox());
			this->numericUpDown_BodyFat = (gcnew System::Windows::Forms::NumericUpDown());
			this->label_Disp_BodyFat = (gcnew System::Windows::Forms::Label());
			this->numericUpDown_Hydration = (gcnew System::Windows::Forms::NumericUpDown());
			this->label_Disp_Hydration = (gcnew System::Windows::Forms::Label());
			this->checkBox_TxPage2 = (gcnew System::Windows::Forms::CheckBox());
			this->tabPage_GlobalData = (gcnew System::Windows::Forms::TabPage());
			this->checkBox_InvalidSerial = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxProd = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxMfg = (gcnew System::Windows::Forms::CheckBox());
			this->buttonUpdateGlobal = (gcnew System::Windows::Forms::Button());
			this->labelError = (gcnew System::Windows::Forms::Label());
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
			this->tabPage_CustomProfile = (gcnew System::Windows::Forms::TabPage());
			this->groupBoxLifetimeActivity = (gcnew System::Windows::Forms::GroupBox());
			this->radioButtonLifetimeActivityFalse = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonLifetimeActivityTrue = (gcnew System::Windows::Forms::RadioButton());
			this->numericUpDownActivityLevel = (gcnew System::Windows::Forms::NumericUpDown());
			this->label24 = (gcnew System::Windows::Forms::Label());
			this->numericUpDownProfile = (gcnew System::Windows::Forms::NumericUpDown());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->radioButtonFemale = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonMale = (gcnew System::Windows::Forms::RadioButton());
			this->label29 = (gcnew System::Windows::Forms::Label());
			this->label28 = (gcnew System::Windows::Forms::Label());
			this->numericUpDownHeight = (gcnew System::Windows::Forms::NumericUpDown());
			this->label27 = (gcnew System::Windows::Forms::Label());
			this->numericUpDownAge = (gcnew System::Windows::Forms::NumericUpDown());
			this->tabPage_ActiveProfile = (gcnew System::Windows::Forms::TabPage());
			this->labelExchangeDisplay = (gcnew System::Windows::Forms::Label());
			this->label12 = (gcnew System::Windows::Forms::Label());
			this->labelLifestyle = (gcnew System::Windows::Forms::Label());
			this->label40 = (gcnew System::Windows::Forms::Label());
			this->label156 = (gcnew System::Windows::Forms::Label());
			this->labelProfileSetScale = (gcnew System::Windows::Forms::Label());
			this->label81 = (gcnew System::Windows::Forms::Label());
			this->labelExchangeScale = (gcnew System::Windows::Forms::Label());
			this->label79 = (gcnew System::Windows::Forms::Label());
			this->labelAntfsRx = (gcnew System::Windows::Forms::Label());
			this->label17 = (gcnew System::Windows::Forms::Label());
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
			this->labelBMR = (gcnew System::Windows::Forms::Label());
			this->label_BasalMetRate = (gcnew System::Windows::Forms::Label());
			this->labelAMR = (gcnew System::Windows::Forms::Label());
			this->label_ActiveMetRate = (gcnew System::Windows::Forms::Label());
			this->label_MR = (gcnew System::Windows::Forms::Label());
			this->labelBF = (gcnew System::Windows::Forms::Label());
			this->label_BodyFat = (gcnew System::Windows::Forms::Label());
			this->labelH = (gcnew System::Windows::Forms::Label());
			this->label_Hydration = (gcnew System::Windows::Forms::Label());
			this->labelProfileActive = (gcnew System::Windows::Forms::Label());
			this->label31 = (gcnew System::Windows::Forms::Label());
			this->label_Trn_DisplayTitle = (gcnew System::Windows::Forms::Label());
			this->labelW1 = (gcnew System::Windows::Forms::Label());
			this->labelWeight = (gcnew System::Windows::Forms::Label());
			this->label_Glb_SoftwareVerDisplay = (gcnew System::Windows::Forms::Label());
			this->label_Glb_SerialNumDisplay = (gcnew System::Windows::Forms::Label());
			this->label_Glb_HardwareVerDisplay = (gcnew System::Windows::Forms::Label());
			this->label_Glb_ModelNumDisplay = (gcnew System::Windows::Forms::Label());
			this->label_Glb_ManfIDDisplay = (gcnew System::Windows::Forms::Label());
			this->label_Glb_GlobalDataError = (gcnew System::Windows::Forms::Label());
			this->label_Glb_SoftwareVer = (gcnew System::Windows::Forms::Label());
			this->label_Glb_HardwareVer = (gcnew System::Windows::Forms::Label());
			this->label_Glb_SerialNum = (gcnew System::Windows::Forms::Label());
			this->label_Glb_ManfID = (gcnew System::Windows::Forms::Label());
			this->label_Glb_ModelNum = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->label11 = (gcnew System::Windows::Forms::Label());
			this->panel_Settings->SuspendLayout();
			this->tabControl1->SuspendLayout();
			this->tabPage_Main->SuspendLayout();
			this->groupBox6->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownWeight))->BeginInit();
			this->groupBox2->SuspendLayout();
			this->tabPage_Advanced->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_BoneMass))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_MuscleMass))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_BasalMetRate))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_ActiveMetRate))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_BodyFat))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Hydration))->BeginInit();
			this->tabPage_GlobalData->SuspendLayout();
			this->tabPage_CustomProfile->SuspendLayout();
			this->groupBoxLifetimeActivity->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownActivityLevel))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownProfile))->BeginInit();
			this->groupBox1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownHeight))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownAge))->BeginInit();
			this->tabPage_ActiveProfile->SuspendLayout();
			this->panel_Display->SuspendLayout();
			this->SuspendLayout();
			// 
			// panel_Settings
			// 
			this->panel_Settings->Controls->Add(this->tabControl1);
			this->panel_Settings->Location = System::Drawing::Point(294, 39);
			this->panel_Settings->Name = L"panel_Settings";
			this->panel_Settings->Size = System::Drawing::Size(433, 166);
			this->panel_Settings->TabIndex = 0;
			// 
			// tabControl1
			// 
			this->tabControl1->Controls->Add(this->tabPage_Main);
			this->tabControl1->Controls->Add(this->tabPage_Advanced);
			this->tabControl1->Controls->Add(this->tabPage_GlobalData);
			this->tabControl1->Controls->Add(this->tabPage_CustomProfile);
			this->tabControl1->Controls->Add(this->tabPage_ActiveProfile);
			this->tabControl1->Location = System::Drawing::Point(3, 3);
			this->tabControl1->Name = L"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(431, 163);
			this->tabControl1->TabIndex = 0;
			// 
			// tabPage_Main
			// 
			this->tabPage_Main->Controls->Add(this->buttonReset);
			this->tabPage_Main->Controls->Add(this->groupBox6);
			this->tabPage_Main->Controls->Add(this->labelStatusProfileSent);
			this->tabPage_Main->Controls->Add(this->labelStatusProfileUpdate);
			this->tabPage_Main->Controls->Add(this->labelStatusProfileMatch);
			this->tabPage_Main->Controls->Add(this->labelStatusProfileNew);
			this->tabPage_Main->Controls->Add(this->groupBox2);
			this->tabPage_Main->Controls->Add(this->checkBoxProfileSet);
			this->tabPage_Main->Controls->Add(this->checkBoxExchange);
			this->tabPage_Main->Controls->Add(this->checkBoxAntfs);
			this->tabPage_Main->Controls->Add(this->labelStatusProfileRx);
			this->tabPage_Main->ForeColor = System::Drawing::SystemColors::ControlText;
			this->tabPage_Main->Location = System::Drawing::Point(4, 22);
			this->tabPage_Main->Name = L"tabPage_Main";
			this->tabPage_Main->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_Main->Size = System::Drawing::Size(423, 137);
			this->tabPage_Main->TabIndex = 0;
			this->tabPage_Main->Text = L"Main";
			this->tabPage_Main->UseVisualStyleBackColor = true;
			// 
			// buttonReset
			// 
			this->buttonReset->Location = System::Drawing::Point(122, 79);
			this->buttonReset->Name = L"buttonReset";
			this->buttonReset->Size = System::Drawing::Size(111, 25);
			this->buttonReset->TabIndex = 35;
			this->buttonReset->Text = L"Reset Session";
			this->buttonReset->UseVisualStyleBackColor = true;
			this->buttonReset->Click += gcnew System::EventHandler(this, &WeightScaleSensor::buttonReset_Click);
			// 
			// groupBox6
			// 
			this->groupBox6->Controls->Add(this->radioButtonWeightError);
			this->groupBox6->Controls->Add(this->radioButtonWieghtComputing);
			this->groupBox6->Controls->Add(this->radioButtonWeightValid);
			this->groupBox6->Controls->Add(this->numericUpDownWeight);
			this->groupBox6->Location = System::Drawing::Point(109, 0);
			this->groupBox6->Name = L"groupBox6";
			this->groupBox6->Size = System::Drawing::Size(309, 36);
			this->groupBox6->TabIndex = 20;
			this->groupBox6->TabStop = false;
			this->groupBox6->Text = L"Weight Validity";
			// 
			// radioButtonWeightError
			// 
			this->radioButtonWeightError->AutoSize = true;
			this->radioButtonWeightError->Location = System::Drawing::Point(239, 13);
			this->radioButtonWeightError->Name = L"radioButtonWeightError";
			this->radioButtonWeightError->Size = System::Drawing::Size(56, 17);
			this->radioButtonWeightError->TabIndex = 4;
			this->radioButtonWeightError->Text = L"Invalid";
			this->radioButtonWeightError->UseVisualStyleBackColor = true;
			this->radioButtonWeightError->CheckedChanged += gcnew System::EventHandler(this, &WeightScaleSensor::radioButtonWeightError_CheckedChanged);
			// 
			// radioButtonWieghtComputing
			// 
			this->radioButtonWieghtComputing->AutoSize = true;
			this->radioButtonWieghtComputing->Checked = true;
			this->radioButtonWieghtComputing->Location = System::Drawing::Point(158, 13);
			this->radioButtonWieghtComputing->Name = L"radioButtonWieghtComputing";
			this->radioButtonWieghtComputing->Size = System::Drawing::Size(75, 17);
			this->radioButtonWieghtComputing->TabIndex = 3;
			this->radioButtonWieghtComputing->TabStop = true;
			this->radioButtonWieghtComputing->Text = L"Computing";
			this->radioButtonWieghtComputing->UseVisualStyleBackColor = true;
			this->radioButtonWieghtComputing->CheckedChanged += gcnew System::EventHandler(this, &WeightScaleSensor::radioButtonWeightComputing_CheckedChanged);
			// 
			// radioButtonWeightValid
			// 
			this->radioButtonWeightValid->AutoSize = true;
			this->radioButtonWeightValid->Location = System::Drawing::Point(5, 13);
			this->radioButtonWeightValid->Name = L"radioButtonWeightValid";
			this->radioButtonWeightValid->Size = System::Drawing::Size(80, 17);
			this->radioButtonWeightValid->TabIndex = 2;
			this->radioButtonWeightValid->Text = L"Weight (kg)";
			this->radioButtonWeightValid->UseVisualStyleBackColor = true;
			this->radioButtonWeightValid->CheckedChanged += gcnew System::EventHandler(this, &WeightScaleSensor::radioButtonWeightValid_CheckedChanged);
			// 
			// numericUpDownWeight
			// 
			this->numericUpDownWeight->DecimalPlaces = 2;
			this->numericUpDownWeight->Enabled = false;
			this->numericUpDownWeight->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 131072});
			this->numericUpDownWeight->Location = System::Drawing::Point(91, 10);
			this->numericUpDownWeight->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {65533, 0, 0, 131072});
			this->numericUpDownWeight->Name = L"numericUpDownWeight";
			this->numericUpDownWeight->Size = System::Drawing::Size(61, 20);
			this->numericUpDownWeight->TabIndex = 0;
			this->numericUpDownWeight->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {6550, 0, 0, 131072});
			this->numericUpDownWeight->ValueChanged += gcnew System::EventHandler(this, &WeightScaleSensor::numericUpDownWeight_ValueChanged);
			// 
			// labelStatusProfileSent
			// 
			this->labelStatusProfileSent->AutoSize = true;
			this->labelStatusProfileSent->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.5F, System::Drawing::FontStyle::Bold, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->labelStatusProfileSent->ForeColor = System::Drawing::SystemColors::ControlText;
			this->labelStatusProfileSent->Location = System::Drawing::Point(239, 100);
			this->labelStatusProfileSent->Name = L"labelStatusProfileSent";
			this->labelStatusProfileSent->Size = System::Drawing::Size(22, 15);
			this->labelStatusProfileSent->TabIndex = 19;
			this->labelStatusProfileSent->Text = L"---";
			// 
			// labelStatusProfileUpdate
			// 
			this->labelStatusProfileUpdate->AutoSize = true;
			this->labelStatusProfileUpdate->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.5F, System::Drawing::FontStyle::Bold, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->labelStatusProfileUpdate->ForeColor = System::Drawing::SystemColors::ControlText;
			this->labelStatusProfileUpdate->Location = System::Drawing::Point(239, 85);
			this->labelStatusProfileUpdate->Name = L"labelStatusProfileUpdate";
			this->labelStatusProfileUpdate->Size = System::Drawing::Size(22, 15);
			this->labelStatusProfileUpdate->TabIndex = 18;
			this->labelStatusProfileUpdate->Text = L"---";
			// 
			// labelStatusProfileMatch
			// 
			this->labelStatusProfileMatch->AutoSize = true;
			this->labelStatusProfileMatch->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.5F, System::Drawing::FontStyle::Bold, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->labelStatusProfileMatch->ForeColor = System::Drawing::SystemColors::ControlText;
			this->labelStatusProfileMatch->Location = System::Drawing::Point(239, 70);
			this->labelStatusProfileMatch->Name = L"labelStatusProfileMatch";
			this->labelStatusProfileMatch->Size = System::Drawing::Size(22, 15);
			this->labelStatusProfileMatch->TabIndex = 17;
			this->labelStatusProfileMatch->Text = L"---";
			// 
			// labelStatusProfileNew
			// 
			this->labelStatusProfileNew->AutoSize = true;
			this->labelStatusProfileNew->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.5F, System::Drawing::FontStyle::Bold, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->labelStatusProfileNew->ForeColor = System::Drawing::SystemColors::ControlText;
			this->labelStatusProfileNew->Location = System::Drawing::Point(239, 55);
			this->labelStatusProfileNew->Name = L"labelStatusProfileNew";
			this->labelStatusProfileNew->Size = System::Drawing::Size(22, 15);
			this->labelStatusProfileNew->TabIndex = 16;
			this->labelStatusProfileNew->Text = L"---";
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->radioButtonGuest);
			this->groupBox2->Controls->Add(this->radioButtonUser3);
			this->groupBox2->Controls->Add(this->radioButtonUser2);
			this->groupBox2->Controls->Add(this->radioButtonUser1);
			this->groupBox2->Enabled = false;
			this->groupBox2->Location = System::Drawing::Point(9, 34);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(224, 37);
			this->groupBox2->TabIndex = 15;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Scale User Profiles";
			// 
			// radioButtonGuest
			// 
			this->radioButtonGuest->AutoSize = true;
			this->radioButtonGuest->Location = System::Drawing::Point(163, 14);
			this->radioButtonGuest->Name = L"radioButtonGuest";
			this->radioButtonGuest->Size = System::Drawing::Size(53, 17);
			this->radioButtonGuest->TabIndex = 3;
			this->radioButtonGuest->TabStop = true;
			this->radioButtonGuest->Text = L"Guest";
			this->radioButtonGuest->UseVisualStyleBackColor = true;
			this->radioButtonGuest->CheckedChanged += gcnew System::EventHandler(this, &WeightScaleSensor::radioButtonGuest_CheckedChanged);
			// 
			// radioButtonUser3
			// 
			this->radioButtonUser3->AutoSize = true;
			this->radioButtonUser3->Location = System::Drawing::Point(113, 14);
			this->radioButtonUser3->Name = L"radioButtonUser3";
			this->radioButtonUser3->Size = System::Drawing::Size(45, 17);
			this->radioButtonUser3->TabIndex = 2;
			this->radioButtonUser3->Text = L"Dan";
			this->radioButtonUser3->UseVisualStyleBackColor = true;
			this->radioButtonUser3->CheckedChanged += gcnew System::EventHandler(this, &WeightScaleSensor::radioButtonUser3_CheckedChanged);
			// 
			// radioButtonUser2
			// 
			this->radioButtonUser2->AutoSize = true;
			this->radioButtonUser2->Location = System::Drawing::Point(55, 14);
			this->radioButtonUser2->Name = L"radioButtonUser2";
			this->radioButtonUser2->Size = System::Drawing::Size(53, 17);
			this->radioButtonUser2->TabIndex = 1;
			this->radioButtonUser2->Text = L"Jenny";
			this->radioButtonUser2->UseVisualStyleBackColor = true;
			this->radioButtonUser2->CheckedChanged += gcnew System::EventHandler(this, &WeightScaleSensor::radioButtonUser2_CheckedChanged);
			// 
			// radioButtonUser1
			// 
			this->radioButtonUser1->AutoSize = true;
			this->radioButtonUser1->Checked = true;
			this->radioButtonUser1->Location = System::Drawing::Point(6, 14);
			this->radioButtonUser1->Name = L"radioButtonUser1";
			this->radioButtonUser1->Size = System::Drawing::Size(44, 17);
			this->radioButtonUser1->TabIndex = 0;
			this->radioButtonUser1->TabStop = true;
			this->radioButtonUser1->Text = L"Bob";
			this->radioButtonUser1->UseVisualStyleBackColor = true;
			this->radioButtonUser1->CheckedChanged += gcnew System::EventHandler(this, &WeightScaleSensor::radioButtonUser1_CheckedChanged);
			// 
			// checkBoxProfileSet
			// 
			this->checkBoxProfileSet->AutoSize = true;
			this->checkBoxProfileSet->Location = System::Drawing::Point(9, 6);
			this->checkBoxProfileSet->Name = L"checkBoxProfileSet";
			this->checkBoxProfileSet->Size = System::Drawing::Size(99, 17);
			this->checkBoxProfileSet->TabIndex = 14;
			this->checkBoxProfileSet->Text = L"User Profile Set";
			this->checkBoxProfileSet->UseVisualStyleBackColor = true;
			this->checkBoxProfileSet->CheckedChanged += gcnew System::EventHandler(this, &WeightScaleSensor::checkBoxProfileSet_CheckedChanged);
			// 
			// checkBoxExchange
			// 
			this->checkBoxExchange->AutoSize = true;
			this->checkBoxExchange->Checked = true;
			this->checkBoxExchange->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBoxExchange->Location = System::Drawing::Point(9, 108);
			this->checkBoxExchange->Name = L"checkBoxExchange";
			this->checkBoxExchange->Size = System::Drawing::Size(171, 17);
			this->checkBoxExchange->TabIndex = 8;
			this->checkBoxExchange->Text = L"User Profile Exchange Support";
			this->checkBoxExchange->UseVisualStyleBackColor = true;
			this->checkBoxExchange->CheckedChanged += gcnew System::EventHandler(this, &WeightScaleSensor::checkBoxExchange_CheckedChanged);
			// 
			// checkBoxAntfs
			// 
			this->checkBoxAntfs->AutoSize = true;
			this->checkBoxAntfs->Enabled = false;
			this->checkBoxAntfs->Location = System::Drawing::Point(9, 84);
			this->checkBoxAntfs->Name = L"checkBoxAntfs";
			this->checkBoxAntfs->Size = System::Drawing::Size(104, 17);
			this->checkBoxAntfs->TabIndex = 7;
			this->checkBoxAntfs->Text = L"ANT-FS Support";
			this->checkBoxAntfs->UseVisualStyleBackColor = true;
			this->checkBoxAntfs->CheckedChanged += gcnew System::EventHandler(this, &WeightScaleSensor::checkBoxAntfs_CheckedChanged);
			// 
			// labelStatusProfileRx
			// 
			this->labelStatusProfileRx->AutoSize = true;
			this->labelStatusProfileRx->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.5F, System::Drawing::FontStyle::Bold, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->labelStatusProfileRx->ForeColor = System::Drawing::SystemColors::ControlText;
			this->labelStatusProfileRx->Location = System::Drawing::Point(239, 40);
			this->labelStatusProfileRx->Name = L"labelStatusProfileRx";
			this->labelStatusProfileRx->Size = System::Drawing::Size(22, 15);
			this->labelStatusProfileRx->TabIndex = 4;
			this->labelStatusProfileRx->Text = L"---";
			// 
			// tabPage_Advanced
			// 
			this->tabPage_Advanced->Controls->Add(this->numericUpDown_BoneMass);
			this->tabPage_Advanced->Controls->Add(this->label13);
			this->tabPage_Advanced->Controls->Add(this->label14);
			this->tabPage_Advanced->Controls->Add(this->numericUpDown_MuscleMass);
			this->tabPage_Advanced->Controls->Add(this->label15);
			this->tabPage_Advanced->Controls->Add(this->checkBox_TxPage4);
			this->tabPage_Advanced->Controls->Add(this->numericUpDown_BasalMetRate);
			this->tabPage_Advanced->Controls->Add(this->label_Disp_Basal);
			this->tabPage_Advanced->Controls->Add(this->label_MetRate);
			this->tabPage_Advanced->Controls->Add(this->numericUpDown_ActiveMetRate);
			this->tabPage_Advanced->Controls->Add(this->label_Disp_Active);
			this->tabPage_Advanced->Controls->Add(this->checkBox_TxPage3);
			this->tabPage_Advanced->Controls->Add(this->numericUpDown_BodyFat);
			this->tabPage_Advanced->Controls->Add(this->label_Disp_BodyFat);
			this->tabPage_Advanced->Controls->Add(this->numericUpDown_Hydration);
			this->tabPage_Advanced->Controls->Add(this->label_Disp_Hydration);
			this->tabPage_Advanced->Controls->Add(this->checkBox_TxPage2);
			this->tabPage_Advanced->Location = System::Drawing::Point(4, 22);
			this->tabPage_Advanced->Name = L"tabPage_Advanced";
			this->tabPage_Advanced->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_Advanced->Size = System::Drawing::Size(423, 137);
			this->tabPage_Advanced->TabIndex = 8;
			this->tabPage_Advanced->Text = L"Advanced Data";
			this->tabPage_Advanced->UseVisualStyleBackColor = true;
			// 
			// numericUpDown_BoneMass
			// 
			this->numericUpDown_BoneMass->DecimalPlaces = 1;
			this->numericUpDown_BoneMass->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
			this->numericUpDown_BoneMass->Location = System::Drawing::Point(346, 83);
			this->numericUpDown_BoneMass->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {253, 0, 0, 65536});
			this->numericUpDown_BoneMass->Name = L"numericUpDown_BoneMass";
			this->numericUpDown_BoneMass->Size = System::Drawing::Size(63, 20);
			this->numericUpDown_BoneMass->TabIndex = 16;
			this->numericUpDown_BoneMass->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {182, 0, 0, 65536});
			this->numericUpDown_BoneMass->ValueChanged += gcnew System::EventHandler(this, &WeightScaleSensor::numericUpDown_BoneMass_ValueChanged);
			// 
			// label13
			// 
			this->label13->AutoSize = true;
			this->label13->Location = System::Drawing::Point(303, 85);
			this->label13->Name = L"label13";
			this->label13->Size = System::Drawing::Size(32, 13);
			this->label13->TabIndex = 15;
			this->label13->Text = L"Bone";
			// 
			// label14
			// 
			this->label14->AutoSize = true;
			this->label14->Location = System::Drawing::Point(303, 36);
			this->label14->Name = L"label14";
			this->label14->Size = System::Drawing::Size(53, 13);
			this->label14->TabIndex = 14;
			this->label14->Text = L"Mass (kg)";
			this->label14->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// numericUpDown_MuscleMass
			// 
			this->numericUpDown_MuscleMass->DecimalPlaces = 2;
			this->numericUpDown_MuscleMass->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 131072});
			this->numericUpDown_MuscleMass->Location = System::Drawing::Point(346, 55);
			this->numericUpDown_MuscleMass->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {65533, 0, 0, 131072});
			this->numericUpDown_MuscleMass->Name = L"numericUpDown_MuscleMass";
			this->numericUpDown_MuscleMass->Size = System::Drawing::Size(63, 20);
			this->numericUpDown_MuscleMass->TabIndex = 13;
			this->numericUpDown_MuscleMass->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {4762, 0, 0, 131072});
			this->numericUpDown_MuscleMass->ValueChanged += gcnew System::EventHandler(this, &WeightScaleSensor::numericUpDown_MuscleMass_ValueChanged);
			// 
			// label15
			// 
			this->label15->AutoSize = true;
			this->label15->Location = System::Drawing::Point(303, 57);
			this->label15->Name = L"label15";
			this->label15->Size = System::Drawing::Size(41, 13);
			this->label15->TabIndex = 12;
			this->label15->Text = L"Muscle";
			// 
			// checkBox_TxPage4
			// 
			this->checkBox_TxPage4->AutoSize = true;
			this->checkBox_TxPage4->Checked = true;
			this->checkBox_TxPage4->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBox_TxPage4->Location = System::Drawing::Point(306, 11);
			this->checkBox_TxPage4->Name = L"checkBox_TxPage4";
			this->checkBox_TxPage4->Size = System::Drawing::Size(82, 17);
			this->checkBox_TxPage4->TabIndex = 11;
			this->checkBox_TxPage4->Text = L"Use Page 4";
			this->checkBox_TxPage4->UseVisualStyleBackColor = true;
			this->checkBox_TxPage4->CheckedChanged += gcnew System::EventHandler(this, &WeightScaleSensor::checkBox_TxPage4_CheckedChanged);
			// 
			// numericUpDown_BasalMetRate
			// 
			this->numericUpDown_BasalMetRate->DecimalPlaces = 2;
			this->numericUpDown_BasalMetRate->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {25, 0, 0, 131072});
			this->numericUpDown_BasalMetRate->Location = System::Drawing::Point(222, 83);
			this->numericUpDown_BasalMetRate->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {16383, 0, 0, 0});
			this->numericUpDown_BasalMetRate->Name = L"numericUpDown_BasalMetRate";
			this->numericUpDown_BasalMetRate->Size = System::Drawing::Size(63, 20);
			this->numericUpDown_BasalMetRate->TabIndex = 10;
			this->numericUpDown_BasalMetRate->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {145075, 0, 0, 131072});
			this->numericUpDown_BasalMetRate->ValueChanged += gcnew System::EventHandler(this, &WeightScaleSensor::numericUpDown_BasalMetRate_ValueChanged);
			// 
			// label_Disp_Basal
			// 
			this->label_Disp_Basal->AutoSize = true;
			this->label_Disp_Basal->Location = System::Drawing::Point(179, 85);
			this->label_Disp_Basal->Name = L"label_Disp_Basal";
			this->label_Disp_Basal->Size = System::Drawing::Size(33, 13);
			this->label_Disp_Basal->TabIndex = 9;
			this->label_Disp_Basal->Text = L"Basal";
			// 
			// label_MetRate
			// 
			this->label_MetRate->AutoSize = true;
			this->label_MetRate->Location = System::Drawing::Point(179, 36);
			this->label_MetRate->Name = L"label_MetRate";
			this->label_MetRate->Size = System::Drawing::Size(95, 13);
			this->label_MetRate->TabIndex = 8;
			this->label_MetRate->Text = L"Metab. Rate (kcal)";
			this->label_MetRate->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// numericUpDown_ActiveMetRate
			// 
			this->numericUpDown_ActiveMetRate->DecimalPlaces = 2;
			this->numericUpDown_ActiveMetRate->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {25, 0, 0, 131072});
			this->numericUpDown_ActiveMetRate->Location = System::Drawing::Point(222, 55);
			this->numericUpDown_ActiveMetRate->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {16383, 0, 0, 0});
			this->numericUpDown_ActiveMetRate->Name = L"numericUpDown_ActiveMetRate";
			this->numericUpDown_ActiveMetRate->Size = System::Drawing::Size(63, 20);
			this->numericUpDown_ActiveMetRate->TabIndex = 7;
			this->numericUpDown_ActiveMetRate->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {214525, 0, 0, 131072});
			this->numericUpDown_ActiveMetRate->ValueChanged += gcnew System::EventHandler(this, &WeightScaleSensor::numericUpDown_ActiveMetRate_ValueChanged);
			// 
			// label_Disp_Active
			// 
			this->label_Disp_Active->AutoSize = true;
			this->label_Disp_Active->Location = System::Drawing::Point(179, 57);
			this->label_Disp_Active->Name = L"label_Disp_Active";
			this->label_Disp_Active->Size = System::Drawing::Size(37, 13);
			this->label_Disp_Active->TabIndex = 6;
			this->label_Disp_Active->Text = L"Active";
			// 
			// checkBox_TxPage3
			// 
			this->checkBox_TxPage3->AutoSize = true;
			this->checkBox_TxPage3->Checked = true;
			this->checkBox_TxPage3->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBox_TxPage3->Location = System::Drawing::Point(182, 11);
			this->checkBox_TxPage3->Name = L"checkBox_TxPage3";
			this->checkBox_TxPage3->Size = System::Drawing::Size(82, 17);
			this->checkBox_TxPage3->TabIndex = 5;
			this->checkBox_TxPage3->Text = L"Use Page 3";
			this->checkBox_TxPage3->UseVisualStyleBackColor = true;
			this->checkBox_TxPage3->CheckedChanged += gcnew System::EventHandler(this, &WeightScaleSensor::checkBox_TxPage3_CheckedChanged);
			// 
			// numericUpDown_BodyFat
			// 
			this->numericUpDown_BodyFat->DecimalPlaces = 2;
			this->numericUpDown_BodyFat->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 131072});
			this->numericUpDown_BodyFat->Location = System::Drawing::Point(91, 57);
			this->numericUpDown_BodyFat->Name = L"numericUpDown_BodyFat";
			this->numericUpDown_BodyFat->Size = System::Drawing::Size(63, 20);
			this->numericUpDown_BodyFat->TabIndex = 4;
			this->numericUpDown_BodyFat->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {2248, 0, 0, 131072});
			this->numericUpDown_BodyFat->ValueChanged += gcnew System::EventHandler(this, &WeightScaleSensor::numericUpDown_BodyFat_ValueChanged);
			// 
			// label_Disp_BodyFat
			// 
			this->label_Disp_BodyFat->AutoSize = true;
			this->label_Disp_BodyFat->Location = System::Drawing::Point(6, 59);
			this->label_Disp_BodyFat->Name = L"label_Disp_BodyFat";
			this->label_Disp_BodyFat->Size = System::Drawing::Size(66, 13);
			this->label_Disp_BodyFat->TabIndex = 3;
			this->label_Disp_BodyFat->Text = L"Body Fat (%)";
			// 
			// numericUpDown_Hydration
			// 
			this->numericUpDown_Hydration->DecimalPlaces = 2;
			this->numericUpDown_Hydration->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 131072});
			this->numericUpDown_Hydration->Location = System::Drawing::Point(91, 29);
			this->numericUpDown_Hydration->Name = L"numericUpDown_Hydration";
			this->numericUpDown_Hydration->Size = System::Drawing::Size(63, 20);
			this->numericUpDown_Hydration->TabIndex = 2;
			this->numericUpDown_Hydration->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {5215, 0, 0, 131072});
			this->numericUpDown_Hydration->ValueChanged += gcnew System::EventHandler(this, &WeightScaleSensor::numericUpDown_Hydration_ValueChanged);
			// 
			// label_Disp_Hydration
			// 
			this->label_Disp_Hydration->AutoSize = true;
			this->label_Disp_Hydration->Location = System::Drawing::Point(6, 31);
			this->label_Disp_Hydration->Name = L"label_Disp_Hydration";
			this->label_Disp_Hydration->Size = System::Drawing::Size(69, 13);
			this->label_Disp_Hydration->TabIndex = 1;
			this->label_Disp_Hydration->Text = L"Hydration (%)";
			// 
			// checkBox_TxPage2
			// 
			this->checkBox_TxPage2->AutoSize = true;
			this->checkBox_TxPage2->Checked = true;
			this->checkBox_TxPage2->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBox_TxPage2->Location = System::Drawing::Point(9, 11);
			this->checkBox_TxPage2->Name = L"checkBox_TxPage2";
			this->checkBox_TxPage2->Size = System::Drawing::Size(82, 17);
			this->checkBox_TxPage2->TabIndex = 0;
			this->checkBox_TxPage2->Text = L"Use Page 2";
			this->checkBox_TxPage2->UseVisualStyleBackColor = true;
			this->checkBox_TxPage2->CheckedChanged += gcnew System::EventHandler(this, &WeightScaleSensor::checkBox_TxPage2_CheckedChanged);
			// 
			// tabPage_GlobalData
			// 
			this->tabPage_GlobalData->Controls->Add(this->checkBox_InvalidSerial);
			this->tabPage_GlobalData->Controls->Add(this->checkBoxProd);
			this->tabPage_GlobalData->Controls->Add(this->checkBoxMfg);
			this->tabPage_GlobalData->Controls->Add(this->buttonUpdateGlobal);
			this->tabPage_GlobalData->Controls->Add(this->labelError);
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
			this->tabPage_GlobalData->Size = System::Drawing::Size(423, 137);
			this->tabPage_GlobalData->TabIndex = 2;
			this->tabPage_GlobalData->Text = L"Global Data";
			this->tabPage_GlobalData->UseVisualStyleBackColor = true;
			// 
			// checkBox_InvalidSerial
			// 
			this->checkBox_InvalidSerial->AutoSize = true;
			this->checkBox_InvalidSerial->Location = System::Drawing::Point(303, 50);
			this->checkBox_InvalidSerial->Name = L"checkBox_InvalidSerial";
			this->checkBox_InvalidSerial->Size = System::Drawing::Size(79, 17);
			this->checkBox_InvalidSerial->TabIndex = 50;
			this->checkBox_InvalidSerial->Text = L"No Serial #";
			this->checkBox_InvalidSerial->UseVisualStyleBackColor = true;
			this->checkBox_InvalidSerial->CheckedChanged += gcnew System::EventHandler(this, &WeightScaleSensor::checkBox_InvalidSerial_CheckedChanged);
			// 
			// checkBoxProd
			// 
			this->checkBoxProd->AutoSize = true;
			this->checkBoxProd->Checked = true;
			this->checkBoxProd->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBoxProd->Enabled = false;
			this->checkBoxProd->Location = System::Drawing::Point(205, 6);
			this->checkBoxProd->Name = L"checkBoxProd";
			this->checkBoxProd->Size = System::Drawing::Size(148, 17);
			this->checkBoxProd->TabIndex = 46;
			this->checkBoxProd->Text = L"Enable Product Info Page";
			this->checkBoxProd->UseVisualStyleBackColor = true;
			// 
			// checkBoxMfg
			// 
			this->checkBoxMfg->AutoSize = true;
			this->checkBoxMfg->Checked = true;
			this->checkBoxMfg->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBoxMfg->Enabled = false;
			this->checkBoxMfg->Location = System::Drawing::Point(6, 6);
			this->checkBoxMfg->Name = L"checkBoxMfg";
			this->checkBoxMfg->Size = System::Drawing::Size(184, 17);
			this->checkBoxMfg->TabIndex = 42;
			this->checkBoxMfg->Text = L"Enable Manufacturing Data Page";
			this->checkBoxMfg->UseVisualStyleBackColor = true;
			// 
			// buttonUpdateGlobal
			// 
			this->buttonUpdateGlobal->Location = System::Drawing::Point(229, 99);
			this->buttonUpdateGlobal->Name = L"buttonUpdateGlobal";
			this->buttonUpdateGlobal->Size = System::Drawing::Size(97, 20);
			this->buttonUpdateGlobal->TabIndex = 49;
			this->buttonUpdateGlobal->Text = L"Update All";
			this->buttonUpdateGlobal->UseVisualStyleBackColor = true;
			this->buttonUpdateGlobal->Click += gcnew System::EventHandler(this, &WeightScaleSensor::buttonUpdateGlobal_Click);
			// 
			// labelError
			// 
			this->labelError->AutoSize = true;
			this->labelError->Location = System::Drawing::Point(29, 112);
			this->labelError->Name = L"labelError";
			this->labelError->Size = System::Drawing::Size(32, 13);
			this->labelError->TabIndex = 20;
			this->labelError->Text = L"Error:";
			this->labelError->Visible = false;
			// 
			// label18
			// 
			this->label18->AutoSize = true;
			this->label18->Location = System::Drawing::Point(226, 76);
			this->label18->Name = L"label18";
			this->label18->Size = System::Drawing::Size(71, 13);
			this->label18->TabIndex = 4;
			this->label18->Text = L"Software Ver:";
			// 
			// label19
			// 
			this->label19->AutoSize = true;
			this->label19->Location = System::Drawing::Point(29, 76);
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
			this->textboxSwVer->Location = System::Drawing::Point(303, 73);
			this->textboxSwVer->MaxLength = 3;
			this->textboxSwVer->Name = L"textboxSwVer";
			this->textboxSwVer->Size = System::Drawing::Size(29, 20);
			this->textboxSwVer->TabIndex = 48;
			this->textboxSwVer->Text = L"1";
			// 
			// label20
			// 
			this->label20->AutoSize = true;
			this->label20->Location = System::Drawing::Point(226, 27);
			this->label20->Name = L"label20";
			this->label20->Size = System::Drawing::Size(46, 13);
			this->label20->TabIndex = 2;
			this->label20->Text = L"Serial #:";
			// 
			// label21
			// 
			this->label21->AutoSize = true;
			this->label21->Location = System::Drawing::Point(29, 26);
			this->label21->Name = L"label21";
			this->label21->Size = System::Drawing::Size(51, 13);
			this->label21->TabIndex = 1;
			this->label21->Text = L"Manf. ID:";
			// 
			// label22
			// 
			this->label22->AutoSize = true;
			this->label22->Location = System::Drawing::Point(29, 51);
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
			this->textboxMfgID->Text = L"1";
			// 
			// tabPage_CustomProfile
			// 
			this->tabPage_CustomProfile->Controls->Add(this->groupBoxLifetimeActivity);
			this->tabPage_CustomProfile->Controls->Add(this->numericUpDownActivityLevel);
			this->tabPage_CustomProfile->Controls->Add(this->label24);
			this->tabPage_CustomProfile->Controls->Add(this->numericUpDownProfile);
			this->tabPage_CustomProfile->Controls->Add(this->groupBox1);
			this->tabPage_CustomProfile->Controls->Add(this->label29);
			this->tabPage_CustomProfile->Controls->Add(this->label28);
			this->tabPage_CustomProfile->Controls->Add(this->numericUpDownHeight);
			this->tabPage_CustomProfile->Controls->Add(this->label27);
			this->tabPage_CustomProfile->Controls->Add(this->numericUpDownAge);
			this->tabPage_CustomProfile->Location = System::Drawing::Point(4, 22);
			this->tabPage_CustomProfile->Name = L"tabPage_CustomProfile";
			this->tabPage_CustomProfile->Size = System::Drawing::Size(423, 137);
			this->tabPage_CustomProfile->TabIndex = 3;
			this->tabPage_CustomProfile->Text = L"Custom Profile";
			this->tabPage_CustomProfile->UseVisualStyleBackColor = true;
			// 
			// groupBoxLifetimeActivity
			// 
			this->groupBoxLifetimeActivity->Controls->Add(this->radioButtonLifetimeActivityFalse);
			this->groupBoxLifetimeActivity->Controls->Add(this->radioButtonLifetimeActivityTrue);
			this->groupBoxLifetimeActivity->Enabled = false;
			this->groupBoxLifetimeActivity->Location = System::Drawing::Point(320, 64);
			this->groupBoxLifetimeActivity->Name = L"groupBoxLifetimeActivity";
			this->groupBoxLifetimeActivity->Size = System::Drawing::Size(99, 53);
			this->groupBoxLifetimeActivity->TabIndex = 66;
			this->groupBoxLifetimeActivity->TabStop = false;
			this->groupBoxLifetimeActivity->Text = L"Athlete";
			// 
			// radioButtonLifetimeActivityFalse
			// 
			this->radioButtonLifetimeActivityFalse->AutoSize = true;
			this->radioButtonLifetimeActivityFalse->Checked = true;
			this->radioButtonLifetimeActivityFalse->Location = System::Drawing::Point(15, 31);
			this->radioButtonLifetimeActivityFalse->Name = L"radioButtonLifetimeActivityFalse";
			this->radioButtonLifetimeActivityFalse->Size = System::Drawing::Size(39, 17);
			this->radioButtonLifetimeActivityFalse->TabIndex = 66;
			this->radioButtonLifetimeActivityFalse->TabStop = true;
			this->radioButtonLifetimeActivityFalse->Text = L"No";
			this->radioButtonLifetimeActivityFalse->UseVisualStyleBackColor = true;
			this->radioButtonLifetimeActivityFalse->CheckedChanged += gcnew System::EventHandler(this, &WeightScaleSensor::radioButtonLifetimeActivityFalse_CheckedChanged);
			// 
			// radioButtonLifetimeActivityTrue
			// 
			this->radioButtonLifetimeActivityTrue->AutoSize = true;
			this->radioButtonLifetimeActivityTrue->Location = System::Drawing::Point(15, 13);
			this->radioButtonLifetimeActivityTrue->Name = L"radioButtonLifetimeActivityTrue";
			this->radioButtonLifetimeActivityTrue->Size = System::Drawing::Size(43, 17);
			this->radioButtonLifetimeActivityTrue->TabIndex = 65;
			this->radioButtonLifetimeActivityTrue->TabStop = true;
			this->radioButtonLifetimeActivityTrue->Text = L"Yes";
			this->radioButtonLifetimeActivityTrue->UseVisualStyleBackColor = true;
			this->radioButtonLifetimeActivityTrue->CheckedChanged += gcnew System::EventHandler(this, &WeightScaleSensor::radioButtonLifetimeActivityTrue_CheckedChanged);
			// 
			// numericUpDownActivityLevel
			// 
			this->numericUpDownActivityLevel->Enabled = false;
			this->numericUpDownActivityLevel->Location = System::Drawing::Point(320, 22);
			this->numericUpDownActivityLevel->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {7, 0, 0, 0});
			this->numericUpDownActivityLevel->Name = L"numericUpDownActivityLevel";
			this->numericUpDownActivityLevel->Size = System::Drawing::Size(44, 20);
			this->numericUpDownActivityLevel->TabIndex = 63;
			this->numericUpDownActivityLevel->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {3, 0, 0, 0});
			this->numericUpDownActivityLevel->ValueChanged += gcnew System::EventHandler(this, &WeightScaleSensor::numericUpDownActivityLevel_ValueChanged);
			// 
			// label24
			// 
			this->label24->AutoSize = true;
			this->label24->Location = System::Drawing::Point(3, 6);
			this->label24->Name = L"label24";
			this->label24->Size = System::Drawing::Size(65, 13);
			this->label24->TabIndex = 62;
			this->label24->Text = L"User Prof ID";
			// 
			// numericUpDownProfile
			// 
			this->numericUpDownProfile->Enabled = false;
			this->numericUpDownProfile->Location = System::Drawing::Point(6, 22);
			this->numericUpDownProfile->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {15, 0, 0, 0});
			this->numericUpDownProfile->Name = L"numericUpDownProfile";
			this->numericUpDownProfile->Size = System::Drawing::Size(60, 20);
			this->numericUpDownProfile->TabIndex = 61;
			this->numericUpDownProfile->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {2, 0, 0, 0});
			this->numericUpDownProfile->ValueChanged += gcnew System::EventHandler(this, &WeightScaleSensor::numericUpDownProfile_ValueChanged);
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->radioButtonFemale);
			this->groupBox1->Controls->Add(this->radioButtonMale);
			this->groupBox1->Location = System::Drawing::Point(6, 64);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(97, 53);
			this->groupBox1->TabIndex = 14;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Gender";
			// 
			// radioButtonFemale
			// 
			this->radioButtonFemale->AutoSize = true;
			this->radioButtonFemale->Checked = true;
			this->radioButtonFemale->Enabled = false;
			this->radioButtonFemale->Location = System::Drawing::Point(8, 13);
			this->radioButtonFemale->Name = L"radioButtonFemale";
			this->radioButtonFemale->Size = System::Drawing::Size(59, 17);
			this->radioButtonFemale->TabIndex = 0;
			this->radioButtonFemale->TabStop = true;
			this->radioButtonFemale->Text = L"Female";
			this->radioButtonFemale->UseVisualStyleBackColor = true;
			this->radioButtonFemale->CheckedChanged += gcnew System::EventHandler(this, &WeightScaleSensor::radioButtonFemale_CheckedChanged);
			// 
			// radioButtonMale
			// 
			this->radioButtonMale->AutoSize = true;
			this->radioButtonMale->Enabled = false;
			this->radioButtonMale->Location = System::Drawing::Point(8, 31);
			this->radioButtonMale->Name = L"radioButtonMale";
			this->radioButtonMale->Size = System::Drawing::Size(48, 17);
			this->radioButtonMale->TabIndex = 1;
			this->radioButtonMale->TabStop = true;
			this->radioButtonMale->Text = L"Male";
			this->radioButtonMale->UseVisualStyleBackColor = true;
			this->radioButtonMale->CheckedChanged += gcnew System::EventHandler(this, &WeightScaleSensor::radioButtonMale_CheckedChanged);
			// 
			// label29
			// 
			this->label29->AutoSize = true;
			this->label29->Location = System::Drawing::Point(318, 6);
			this->label29->Name = L"label29";
			this->label29->Size = System::Drawing::Size(70, 13);
			this->label29->TabIndex = 12;
			this->label29->Text = L"Activity Level";
			// 
			// label28
			// 
			this->label28->AutoSize = true;
			this->label28->Location = System::Drawing::Point(160, 64);
			this->label28->Name = L"label28";
			this->label28->Size = System::Drawing::Size(61, 13);
			this->label28->TabIndex = 10;
			this->label28->Text = L"Height (cm)";
			// 
			// numericUpDownHeight
			// 
			this->numericUpDownHeight->Enabled = false;
			this->numericUpDownHeight->Location = System::Drawing::Point(163, 80);
			this->numericUpDownHeight->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
			this->numericUpDownHeight->Name = L"numericUpDownHeight";
			this->numericUpDownHeight->Size = System::Drawing::Size(60, 20);
			this->numericUpDownHeight->TabIndex = 9;
			this->numericUpDownHeight->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {165, 0, 0, 0});
			this->numericUpDownHeight->ValueChanged += gcnew System::EventHandler(this, &WeightScaleSensor::numericUpDownHeight_ValueChanged);
			// 
			// label27
			// 
			this->label27->AutoSize = true;
			this->label27->Location = System::Drawing::Point(160, 6);
			this->label27->Name = L"label27";
			this->label27->Size = System::Drawing::Size(60, 13);
			this->label27->TabIndex = 8;
			this->label27->Text = L"Age (years)";
			// 
			// numericUpDownAge
			// 
			this->numericUpDownAge->Enabled = false;
			this->numericUpDownAge->Location = System::Drawing::Point(163, 22);
			this->numericUpDownAge->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {99, 0, 0, 0});
			this->numericUpDownAge->Name = L"numericUpDownAge";
			this->numericUpDownAge->Size = System::Drawing::Size(60, 20);
			this->numericUpDownAge->TabIndex = 7;
			this->numericUpDownAge->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {22, 0, 0, 0});
			this->numericUpDownAge->ValueChanged += gcnew System::EventHandler(this, &WeightScaleSensor::numericUpDownAge_ValueChanged);
			// 
			// tabPage_ActiveProfile
			// 
			this->tabPage_ActiveProfile->Controls->Add(this->labelExchangeDisplay);
			this->tabPage_ActiveProfile->Controls->Add(this->label12);
			this->tabPage_ActiveProfile->Controls->Add(this->labelLifestyle);
			this->tabPage_ActiveProfile->Controls->Add(this->label40);
			this->tabPage_ActiveProfile->Controls->Add(this->label156);
			this->tabPage_ActiveProfile->Controls->Add(this->labelProfileSetScale);
			this->tabPage_ActiveProfile->Controls->Add(this->label81);
			this->tabPage_ActiveProfile->Controls->Add(this->labelExchangeScale);
			this->tabPage_ActiveProfile->Controls->Add(this->label79);
			this->tabPage_ActiveProfile->Controls->Add(this->labelAntfsRx);
			this->tabPage_ActiveProfile->Controls->Add(this->label17);
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
			this->tabPage_ActiveProfile->Size = System::Drawing::Size(423, 137);
			this->tabPage_ActiveProfile->TabIndex = 7;
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
			this->labelExchangeDisplay->TabIndex = 82;
			this->labelExchangeDisplay->Text = L"---";
			// 
			// label12
			// 
			this->label12->AutoSize = true;
			this->label12->BackColor = System::Drawing::Color::Transparent;
			this->label12->Location = System::Drawing::Point(171, 89);
			this->label12->Name = L"label12";
			this->label12->Size = System::Drawing::Size(167, 13);
			this->label12->TabIndex = 81;
			this->label12->Text = L"Display Profile Exchange Support:";
			// 
			// labelLifestyle
			// 
			this->labelLifestyle->AutoSize = true;
			this->labelLifestyle->BackColor = System::Drawing::Color::Transparent;
			this->labelLifestyle->Location = System::Drawing::Point(90, 111);
			this->labelLifestyle->Name = L"labelLifestyle";
			this->labelLifestyle->Size = System::Drawing::Size(16, 13);
			this->labelLifestyle->TabIndex = 80;
			this->labelLifestyle->Text = L"---";
			// 
			// label40
			// 
			this->label40->AutoSize = true;
			this->label40->Location = System::Drawing::Point(6, 111);
			this->label40->Name = L"label40";
			this->label40->Size = System::Drawing::Size(45, 13);
			this->label40->TabIndex = 79;
			this->label40->Text = L"Lifestyle";
			// 
			// label156
			// 
			this->label156->AutoSize = true;
			this->label156->BackColor = System::Drawing::Color::Transparent;
			this->label156->Location = System::Drawing::Point(171, 26);
			this->label156->Name = L"label156";
			this->label156->Size = System::Drawing::Size(139, 13);
			this->label156->TabIndex = 77;
			this->label156->Text = L"Scale User Profile Selected:";
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
			this->label81->Location = System::Drawing::Point(171, 68);
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
			this->label79->Location = System::Drawing::Point(171, 47);
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
			// label17
			// 
			this->label17->AutoSize = true;
			this->label17->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Underline, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label17->Location = System::Drawing::Point(171, 5);
			this->label17->Name = L"label17";
			this->label17->Size = System::Drawing::Size(99, 13);
			this->label17->TabIndex = 72;
			this->label17->Text = L"Display capabilities:";
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
			this->label_Bat_Status->Size = System::Drawing::Size(66, 13);
			this->label_Bat_Status->TabIndex = 58;
			this->label_Bat_Status->Text = L"Activity level";
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
			this->panel_Display->Controls->Add(this->labelBMR);
			this->panel_Display->Controls->Add(this->label_BasalMetRate);
			this->panel_Display->Controls->Add(this->labelAMR);
			this->panel_Display->Controls->Add(this->label_ActiveMetRate);
			this->panel_Display->Controls->Add(this->label_MR);
			this->panel_Display->Controls->Add(this->labelBF);
			this->panel_Display->Controls->Add(this->label_BodyFat);
			this->panel_Display->Controls->Add(this->labelH);
			this->panel_Display->Controls->Add(this->label_Hydration);
			this->panel_Display->Controls->Add(this->labelProfileActive);
			this->panel_Display->Controls->Add(this->label31);
			this->panel_Display->Controls->Add(this->label_Trn_DisplayTitle);
			this->panel_Display->Controls->Add(this->labelW1);
			this->panel_Display->Controls->Add(this->labelWeight);
			this->panel_Display->Location = System::Drawing::Point(58, 188);
			this->panel_Display->Name = L"panel_Display";
			this->panel_Display->Size = System::Drawing::Size(200, 90);
			this->panel_Display->TabIndex = 1;
			// 
			// labelBMR
			// 
			this->labelBMR->AutoSize = true;
			this->labelBMR->BackColor = System::Drawing::Color::Transparent;
			this->labelBMR->Location = System::Drawing::Point(117, 54);
			this->labelBMR->Name = L"labelBMR";
			this->labelBMR->Size = System::Drawing::Size(36, 13);
			this->labelBMR->TabIndex = 26;
			this->labelBMR->Text = L"Basal:";
			// 
			// label_BasalMetRate
			// 
			this->label_BasalMetRate->AutoSize = true;
			this->label_BasalMetRate->BackColor = System::Drawing::Color::Transparent;
			this->label_BasalMetRate->Location = System::Drawing::Point(157, 54);
			this->label_BasalMetRate->Name = L"label_BasalMetRate";
			this->label_BasalMetRate->Size = System::Drawing::Size(16, 13);
			this->label_BasalMetRate->TabIndex = 27;
			this->label_BasalMetRate->Text = L"---";
			// 
			// labelAMR
			// 
			this->labelAMR->AutoSize = true;
			this->labelAMR->BackColor = System::Drawing::Color::Transparent;
			this->labelAMR->Location = System::Drawing::Point(117, 38);
			this->labelAMR->Name = L"labelAMR";
			this->labelAMR->Size = System::Drawing::Size(40, 13);
			this->labelAMR->TabIndex = 24;
			this->labelAMR->Text = L"Active:";
			// 
			// label_ActiveMetRate
			// 
			this->label_ActiveMetRate->AutoSize = true;
			this->label_ActiveMetRate->BackColor = System::Drawing::Color::Transparent;
			this->label_ActiveMetRate->Location = System::Drawing::Point(157, 38);
			this->label_ActiveMetRate->Name = L"label_ActiveMetRate";
			this->label_ActiveMetRate->Size = System::Drawing::Size(16, 13);
			this->label_ActiveMetRate->TabIndex = 25;
			this->label_ActiveMetRate->Text = L"---";
			// 
			// label_MR
			// 
			this->label_MR->AutoSize = true;
			this->label_MR->Location = System::Drawing::Point(117, 21);
			this->label_MR->Name = L"label_MR";
			this->label_MR->Size = System::Drawing::Size(80, 13);
			this->label_MR->TabIndex = 23;
			this->label_MR->Text = L"Met Rate (kcal)";
			// 
			// labelBF
			// 
			this->labelBF->AutoSize = true;
			this->labelBF->BackColor = System::Drawing::Color::Transparent;
			this->labelBF->Location = System::Drawing::Point(5, 54);
			this->labelBF->Name = L"labelBF";
			this->labelBF->Size = System::Drawing::Size(69, 13);
			this->labelBF->TabIndex = 21;
			this->labelBF->Text = L"Body Fat (%):";
			// 
			// label_BodyFat
			// 
			this->label_BodyFat->AutoSize = true;
			this->label_BodyFat->BackColor = System::Drawing::Color::Transparent;
			this->label_BodyFat->Location = System::Drawing::Point(78, 55);
			this->label_BodyFat->Name = L"label_BodyFat";
			this->label_BodyFat->Size = System::Drawing::Size(16, 13);
			this->label_BodyFat->TabIndex = 22;
			this->label_BodyFat->Text = L"---";
			// 
			// labelH
			// 
			this->labelH->AutoSize = true;
			this->labelH->BackColor = System::Drawing::Color::Transparent;
			this->labelH->Location = System::Drawing::Point(5, 37);
			this->labelH->Name = L"labelH";
			this->labelH->Size = System::Drawing::Size(72, 13);
			this->labelH->TabIndex = 19;
			this->labelH->Text = L"Hydration (%):";
			// 
			// label_Hydration
			// 
			this->label_Hydration->AutoSize = true;
			this->label_Hydration->BackColor = System::Drawing::Color::Transparent;
			this->label_Hydration->Location = System::Drawing::Point(78, 38);
			this->label_Hydration->Name = L"label_Hydration";
			this->label_Hydration->Size = System::Drawing::Size(16, 13);
			this->label_Hydration->TabIndex = 20;
			this->label_Hydration->Text = L"---";
			// 
			// labelProfileActive
			// 
			this->labelProfileActive->AutoSize = true;
			this->labelProfileActive->BackColor = System::Drawing::Color::Transparent;
			this->labelProfileActive->Location = System::Drawing::Point(122, 71);
			this->labelProfileActive->Name = L"labelProfileActive";
			this->labelProfileActive->Size = System::Drawing::Size(16, 13);
			this->labelProfileActive->TabIndex = 18;
			this->labelProfileActive->Text = L"---";
			// 
			// label31
			// 
			this->label31->AutoSize = true;
			this->label31->BackColor = System::Drawing::Color::Transparent;
			this->label31->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Italic, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label31->Location = System::Drawing::Point(3, 71);
			this->label31->Name = L"label31";
			this->label31->Size = System::Drawing::Size(111, 13);
			this->label31->TabIndex = 14;
			this->label31->Text = L"Active User Profile ID:";
			// 
			// label_Trn_DisplayTitle
			// 
			this->label_Trn_DisplayTitle->AutoSize = true;
			this->label_Trn_DisplayTitle->BackColor = System::Drawing::Color::Transparent;
			this->label_Trn_DisplayTitle->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Underline, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->label_Trn_DisplayTitle->Location = System::Drawing::Point(3, 4);
			this->label_Trn_DisplayTitle->Name = L"label_Trn_DisplayTitle";
			this->label_Trn_DisplayTitle->Size = System::Drawing::Size(175, 13);
			this->label_Trn_DisplayTitle->TabIndex = 13;
			this->label_Trn_DisplayTitle->Text = L"Current Weight Scale Transmission:";
			// 
			// labelW1
			// 
			this->labelW1->AutoSize = true;
			this->labelW1->BackColor = System::Drawing::Color::Transparent;
			this->labelW1->Location = System::Drawing::Point(5, 21);
			this->labelW1->Name = L"labelW1";
			this->labelW1->Size = System::Drawing::Size(65, 13);
			this->labelW1->TabIndex = 11;
			this->labelW1->Text = L"Weight (kg):";
			// 
			// labelWeight
			// 
			this->labelWeight->AutoSize = true;
			this->labelWeight->BackColor = System::Drawing::Color::Transparent;
			this->labelWeight->Location = System::Drawing::Point(77, 22);
			this->labelWeight->Name = L"labelWeight";
			this->labelWeight->Size = System::Drawing::Size(16, 13);
			this->labelWeight->TabIndex = 12;
			this->labelWeight->Text = L"---";
			// 
			// label_Glb_SoftwareVerDisplay
			// 
			this->label_Glb_SoftwareVerDisplay->Location = System::Drawing::Point(275, 51);
			this->label_Glb_SoftwareVerDisplay->Name = L"label_Glb_SoftwareVerDisplay";
			this->label_Glb_SoftwareVerDisplay->Size = System::Drawing::Size(27, 13);
			this->label_Glb_SoftwareVerDisplay->TabIndex = 16;
			this->label_Glb_SoftwareVerDisplay->Text = L"1";
			// 
			// label_Glb_SerialNumDisplay
			// 
			this->label_Glb_SerialNumDisplay->Location = System::Drawing::Point(228, 27);
			this->label_Glb_SerialNumDisplay->Name = L"label_Glb_SerialNumDisplay";
			this->label_Glb_SerialNumDisplay->Size = System::Drawing::Size(71, 13);
			this->label_Glb_SerialNumDisplay->TabIndex = 11;
			this->label_Glb_SerialNumDisplay->Text = L"1234567890";
			// 
			// label_Glb_HardwareVerDisplay
			// 
			this->label_Glb_HardwareVerDisplay->Location = System::Drawing::Point(81, 76);
			this->label_Glb_HardwareVerDisplay->Name = L"label_Glb_HardwareVerDisplay";
			this->label_Glb_HardwareVerDisplay->Size = System::Drawing::Size(27, 13);
			this->label_Glb_HardwareVerDisplay->TabIndex = 14;
			this->label_Glb_HardwareVerDisplay->Text = L"1";
			// 
			// label_Glb_ModelNumDisplay
			// 
			this->label_Glb_ModelNumDisplay->Location = System::Drawing::Point(67, 52);
			this->label_Glb_ModelNumDisplay->Name = L"label_Glb_ModelNumDisplay";
			this->label_Glb_ModelNumDisplay->Size = System::Drawing::Size(42, 13);
			this->label_Glb_ModelNumDisplay->TabIndex = 9;
			this->label_Glb_ModelNumDisplay->Text = L"33669";
			// 
			// label_Glb_ManfIDDisplay
			// 
			this->label_Glb_ManfIDDisplay->Location = System::Drawing::Point(69, 27);
			this->label_Glb_ManfIDDisplay->Name = L"label_Glb_ManfIDDisplay";
			this->label_Glb_ManfIDDisplay->Size = System::Drawing::Size(38, 13);
			this->label_Glb_ManfIDDisplay->TabIndex = 8;
			this->label_Glb_ManfIDDisplay->Text = L"2";
			// 
			// label_Glb_GlobalDataError
			// 
			this->label_Glb_GlobalDataError->AutoSize = true;
			this->label_Glb_GlobalDataError->Location = System::Drawing::Point(180, 95);
			this->label_Glb_GlobalDataError->Name = L"label_Glb_GlobalDataError";
			this->label_Glb_GlobalDataError->Size = System::Drawing::Size(32, 13);
			this->label_Glb_GlobalDataError->TabIndex = 20;
			this->label_Glb_GlobalDataError->Text = L"Error:";
			this->label_Glb_GlobalDataError->Visible = false;
			// 
			// label_Glb_SoftwareVer
			// 
			this->label_Glb_SoftwareVer->AutoSize = true;
			this->label_Glb_SoftwareVer->Location = System::Drawing::Point(198, 51);
			this->label_Glb_SoftwareVer->Name = L"label_Glb_SoftwareVer";
			this->label_Glb_SoftwareVer->Size = System::Drawing::Size(71, 13);
			this->label_Glb_SoftwareVer->TabIndex = 4;
			this->label_Glb_SoftwareVer->Text = L"Software Ver:";
			// 
			// label_Glb_HardwareVer
			// 
			this->label_Glb_HardwareVer->AutoSize = true;
			this->label_Glb_HardwareVer->Location = System::Drawing::Point(6, 76);
			this->label_Glb_HardwareVer->Name = L"label_Glb_HardwareVer";
			this->label_Glb_HardwareVer->Size = System::Drawing::Size(75, 13);
			this->label_Glb_HardwareVer->TabIndex = 3;
			this->label_Glb_HardwareVer->Text = L"Hardware Ver:";
			// 
			// label_Glb_SerialNum
			// 
			this->label_Glb_SerialNum->AutoSize = true;
			this->label_Glb_SerialNum->Location = System::Drawing::Point(176, 27);
			this->label_Glb_SerialNum->Name = L"label_Glb_SerialNum";
			this->label_Glb_SerialNum->Size = System::Drawing::Size(46, 13);
			this->label_Glb_SerialNum->TabIndex = 2;
			this->label_Glb_SerialNum->Text = L"Serial #:";
			// 
			// label_Glb_ManfID
			// 
			this->label_Glb_ManfID->AutoSize = true;
			this->label_Glb_ManfID->Location = System::Drawing::Point(18, 27);
			this->label_Glb_ManfID->Name = L"label_Glb_ManfID";
			this->label_Glb_ManfID->Size = System::Drawing::Size(51, 13);
			this->label_Glb_ManfID->TabIndex = 1;
			this->label_Glb_ManfID->Text = L"Manf. ID:";
			// 
			// label_Glb_ModelNum
			// 
			this->label_Glb_ModelNum->AutoSize = true;
			this->label_Glb_ModelNum->Location = System::Drawing::Point(18, 52);
			this->label_Glb_ModelNum->Name = L"label_Glb_ModelNum";
			this->label_Glb_ModelNum->Size = System::Drawing::Size(49, 13);
			this->label_Glb_ModelNum->TabIndex = 5;
			this->label_Glb_ModelNum->Text = L"Model #:";
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(275, 51);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(27, 13);
			this->label1->TabIndex = 16;
			this->label1->Text = L"1";
			// 
			// label2
			// 
			this->label2->Location = System::Drawing::Point(228, 27);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(71, 13);
			this->label2->TabIndex = 11;
			this->label2->Text = L"1234567890";
			// 
			// label3
			// 
			this->label3->Location = System::Drawing::Point(81, 76);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(27, 13);
			this->label3->TabIndex = 14;
			this->label3->Text = L"1";
			// 
			// label4
			// 
			this->label4->Location = System::Drawing::Point(67, 52);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(42, 13);
			this->label4->TabIndex = 9;
			this->label4->Text = L"33669";
			// 
			// label5
			// 
			this->label5->Location = System::Drawing::Point(69, 27);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(38, 13);
			this->label5->TabIndex = 8;
			this->label5->Text = L"2";
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(180, 95);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(32, 13);
			this->label6->TabIndex = 20;
			this->label6->Text = L"Error:";
			this->label6->Visible = false;
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(198, 51);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(71, 13);
			this->label7->TabIndex = 4;
			this->label7->Text = L"Software Ver:";
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Location = System::Drawing::Point(6, 76);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(75, 13);
			this->label8->TabIndex = 3;
			this->label8->Text = L"Hardware Ver:";
			// 
			// label9
			// 
			this->label9->AutoSize = true;
			this->label9->Location = System::Drawing::Point(176, 27);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(46, 13);
			this->label9->TabIndex = 2;
			this->label9->Text = L"Serial #:";
			// 
			// label10
			// 
			this->label10->AutoSize = true;
			this->label10->Location = System::Drawing::Point(18, 27);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(51, 13);
			this->label10->TabIndex = 1;
			this->label10->Text = L"Manf. ID:";
			// 
			// label11
			// 
			this->label11->AutoSize = true;
			this->label11->Location = System::Drawing::Point(18, 52);
			this->label11->Name = L"label11";
			this->label11->Size = System::Drawing::Size(49, 13);
			this->label11->TabIndex = 5;
			this->label11->Text = L"Model #:";
			// 
			// WeightScaleSensor
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(794, 351);
			this->Controls->Add(this->panel_Display);
			this->Controls->Add(this->panel_Settings);
			this->Name = L"WeightScaleSensor";
			this->Text = L"Weight Scale";
			this->panel_Settings->ResumeLayout(false);
			this->tabControl1->ResumeLayout(false);
			this->tabPage_Main->ResumeLayout(false);
			this->tabPage_Main->PerformLayout();
			this->groupBox6->ResumeLayout(false);
			this->groupBox6->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownWeight))->EndInit();
			this->groupBox2->ResumeLayout(false);
			this->groupBox2->PerformLayout();
			this->tabPage_Advanced->ResumeLayout(false);
			this->tabPage_Advanced->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_BoneMass))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_MuscleMass))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_BasalMetRate))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_ActiveMetRate))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_BodyFat))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_Hydration))->EndInit();
			this->tabPage_GlobalData->ResumeLayout(false);
			this->tabPage_GlobalData->PerformLayout();
			this->tabPage_CustomProfile->ResumeLayout(false);
			this->tabPage_CustomProfile->PerformLayout();
			this->groupBoxLifetimeActivity->ResumeLayout(false);
			this->groupBoxLifetimeActivity->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownActivityLevel))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownProfile))->EndInit();
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownHeight))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownAge))->EndInit();
			this->tabPage_ActiveProfile->ResumeLayout(false);
			this->tabPage_ActiveProfile->PerformLayout();
			this->panel_Display->ResumeLayout(false);
			this->panel_Display->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion


private:
System::Void WeightScaleSensor::radioButtonUser1_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	SelectPresetProfile();
}

System::Void WeightScaleSensor::radioButtonUser2_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	SelectPresetProfile();
}

System::Void WeightScaleSensor::radioButtonUser3_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	SelectPresetProfile();
}

System::Void WeightScaleSensor::radioButtonGuest_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	SelectPresetProfile();
}

System::Void WeightScaleSensor::radioButtonWeightValid_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	SelectWeightStatus();	
}

System::Void WeightScaleSensor::radioButtonWeightComputing_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	SelectWeightStatus();	
}

System::Void WeightScaleSensor::radioButtonWeightError_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	SelectWeightStatus();	
}

System::Void WeightScaleSensor::numericUpDownProfile_ValueChanged(System::Object^  sender, System::EventArgs^  e) 
{
	SelectCustomProfile();	
}

System::Void WeightScaleSensor::radioButtonFemale_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{	
	SelectCustomProfile();
}

System::Void WeightScaleSensor::radioButtonMale_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	SelectCustomProfile();
}

System::Void WeightScaleSensor::numericUpDownAge_ValueChanged(System::Object^  sender, System::EventArgs^  e) 
{	
	SelectCustomProfile();
}

System::Void WeightScaleSensor::numericUpDownHeight_ValueChanged(System::Object^  sender, System::EventArgs^  e) 
{	
	SelectCustomProfile();
}

System::Void WeightScaleSensor::numericUpDownActivityLevel_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	SelectCustomProfile();
}

System::Void WeightScaleSensor::radioButtonLifetimeActivityTrue_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	SelectCustomProfile();
}

System::Void WeightScaleSensor::radioButtonLifetimeActivityFalse_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	SelectCustomProfile();
}

System::Void checkBox_TxPage2_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	SelectPages();
}

System::Void checkBox_TxPage3_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	SelectPages();
}

private: System::Void checkBox_TxPage4_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
   SelectPages();
}

private: System::Void checkBox_InvalidSerial_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
};
