/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/
 

#pragma once

#include "stdafx.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace ANTPlusSim {

	/// <summary>
	/// Summary for AboutSim
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class AboutSim : public System::Windows::Forms::Form
	{
	public:
		AboutSim(System::String^ strTitle_, System::String^ strLibrary_)
		{
			InitializeComponent();
			if(System::String::IsNullOrEmpty(strTitle_))
				this->label_Version->Text = "";
			else
				this->label_Version->Text = strTitle_;
			if(System::String::IsNullOrEmpty(strLibrary_))
				this->label_Library->Text = "";
			else
				this->label_Library->Text = System::String::Concat("ANT Library Ver. ", strLibrary_);
			this->textBox1->SelectionStart = this->textBox1->Text->Length;
			this->textBox1->SelectionLength = 0;
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~AboutSim()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^  label_Version;
	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::Button^  button_OK;
	private: System::Windows::Forms::Label^  label_Library;

	protected: 

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
         System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(AboutSim::typeid));
         this->label_Version = (gcnew System::Windows::Forms::Label());
         this->textBox1 = (gcnew System::Windows::Forms::TextBox());
         this->button_OK = (gcnew System::Windows::Forms::Button());
         this->label_Library = (gcnew System::Windows::Forms::Label());
         this->SuspendLayout();
         // 
         // label_Version
         // 
         this->label_Version->AutoSize = true;
         this->label_Version->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
            static_cast<System::Byte>(0)));
         this->label_Version->Location = System::Drawing::Point(43, 9);
         this->label_Version->Name = L"label_Version";
         this->label_Version->Size = System::Drawing::Size(143, 24);
         this->label_Version->TabIndex = 0;
         this->label_Version->Text = L"ANT+ Simulator";
         // 
         // textBox1
         // 
         this->textBox1->BackColor = System::Drawing::SystemColors::Control;
         this->textBox1->Location = System::Drawing::Point(33, 53);
         this->textBox1->Multiline = true;
         this->textBox1->Name = L"textBox1";
         this->textBox1->ReadOnly = true;
         this->textBox1->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
         this->textBox1->Size = System::Drawing::Size(299, 183);
         this->textBox1->TabIndex = 1;
         this->textBox1->Text = resources->GetString(L"textBox1.Text");
         // 
         // button_OK
         // 
         this->button_OK->Location = System::Drawing::Point(140, 245);
         this->button_OK->Name = L"button_OK";
         this->button_OK->Size = System::Drawing::Size(76, 28);
         this->button_OK->TabIndex = 2;
         this->button_OK->Text = L"OK";
         this->button_OK->UseVisualStyleBackColor = true;
         this->button_OK->Click += gcnew System::EventHandler(this, &AboutSim::button_OK_Click);
         // 
         // label_Library
         // 
         this->label_Library->AutoSize = true;
         this->label_Library->Location = System::Drawing::Point(105, 33);
         this->label_Library->Name = L"label_Library";
         this->label_Library->Size = System::Drawing::Size(63, 13);
         this->label_Library->TabIndex = 3;
         this->label_Library->Text = L"ANT Library";
         // 
         // AboutSim
         // 
         this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
         this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
         this->ClientSize = System::Drawing::Size(361, 285);
         this->Controls->Add(this->label_Library);
         this->Controls->Add(this->button_OK);
         this->Controls->Add(this->textBox1);
         this->Controls->Add(this->label_Version);
         this->Name = L"AboutSim";
         this->Text = L"About";
         this->ResumeLayout(false);
         this->PerformLayout();

      }
#pragma endregion
	private: System::Void button_OK_Click(System::Object^  sender, System::EventArgs^  e) {
				 Close();
			 }
	};
}
