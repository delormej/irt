/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/



#include "stdafx.h"
#include "MainForm.h"
#include "ANTPlus.h"
#include "SimTypes.h"

using namespace ANTPlusSim;

/**************************************************************************
 * MainForm
 * 
 * Constructor for MainForm class. 
 * 
 * 
 * Params:
 *
 * ucSimType_: Type of simulator this is (DISPLAY or SENSOR)
 *
 * returns: N/A
 *
 **************************************************************************/      
MainForm::MainForm(UCHAR ucSimType_)
{
	System::Windows::Forms::Control::CheckForIllegalCrossThreadCalls = FALSE;
	InitializeComponent();


   bMyConnected = FALSE;
   eMyANTPlatform = PLATFORM_UNKNOWN;
   sMyCapabilities.ucStandardOptions = 0xff;
   sMyCapabilities.ucAdvancedOptions = 0x00;   
   sMyCapabilities.ucAdvancedOptions2 = 0x00;
	numChannels = 2;
	ucSimType = ucSimType_;
	switch(ucSimType)
	{
		case SIM_SENSOR:
			strTitle = "ANT+ Sensor Simulator - v"SW_VER_NUM SW_VER_DEV SW_VER_SUFFIX SW_DESCR;
			strAbout = "ANT+ Sensor Simulator - v"SW_VER_NUM SW_VER_DEV SW_VER_SUFFIX;
			break;
      case SIM_DISPLAY:
			strTitle = "ANT+ Display Simulator - v"SW_VER_NUM SW_VER_DEV SW_VER_SUFFIX SW_DESCR;
			strAbout = "ANT+ Display Simulator - v"SW_VER_NUM SW_VER_DEV SW_VER_SUFFIX;
			break;                  
		default:
         // Should assert here.
			strTitle = "ANT+ Simulator";
			strAbout = "ANT+ Simulator";
			break;
	}
	this->Text = strTitle;
   
}


/**************************************************************************
 * ~MainForm
 * 
 * Destructor for MainForm class. 
 *  
 * Params: N/A
 *
 * returns: N/A
 *
 **************************************************************************/      
MainForm::~MainForm() 
{
	if(channelList)
   {
      while(numChannels)
      {
         delete channelList[--numChannels];
      }
      
      delete channelList;
   }
   if (components)
   {
      delete components;
   }
}


/**************************************************************************
 * createDevicePanel
 * 
 * Create panel after connecting to ANT
 * 
 * returns: N/A
 *
 **************************************************************************/
void MainForm::CreateDevicePanel()
{
   panel_devicePanel_1 = (gcnew System::Windows::Forms::Panel());
   richTextBox_deviceReport_1 = (gcnew System::Windows::Forms::RichTextBox());
   panel_devicePanel_1->Controls->Add(richTextBox_deviceReport_1);
   panel_devicePanel_1->Location = System::Drawing::Point(3, 3);
   panel_devicePanel_1->Name = L"panel_devicePanel1";
   panel_devicePanel_1->Size = System::Drawing::Size(398, 117);
   panel_devicePanel_1->TabIndex = 0;
   panel_devicePanel_1->TabStop = false;
   richTextBox_deviceReport_1->BackColor = System::Drawing::Color::LightYellow;
   richTextBox_deviceReport_1->Location = System::Drawing::Point(0, 0);
   richTextBox_deviceReport_1->Name = L"richTextBox_deviceReport1";
   richTextBox_deviceReport_1->ReadOnly = true;
   richTextBox_deviceReport_1->Size = System::Drawing::Size(395, 114);
   richTextBox_deviceReport_1->TabIndex = 0;
   richTextBox_deviceReport_1->TabStop = false;
   richTextBox_deviceReport_1->Text = L"";
   richTextBox_deviceReport_1->Text = System::String::Concat("Device 1: ", comboBox_DeviceSelect->SelectedItem);
   flowLayoutPanel_ActiveDeviceList->Controls->Add(this->panel_devicePanel_1);
}

/**************************************************************************
 * removeDevicePanel
 * 
 * Remove panel
 * 
 * returns: N/A
 *
 **************************************************************************/
 void MainForm::RemoveDevicePanel()
 {
	 panel_devicePanel_1->Controls->Remove(richTextBox_deviceReport_1);
	 delete richTextBox_deviceReport_1;
	 flowLayoutPanel_ActiveDeviceList->Controls->Remove(panel_devicePanel_1);
	 delete panel_devicePanel_1;
 }
/**************************************************************************
 * interpretCapabilities
 * 
 * Decode capabilities message returned by ANT
 * 
 * returns: N/A
 *
 **************************************************************************/
void MainForm::InterpretCapabilities(UCHAR* pcBuffer_)
{
   capabilityDefs basicCapDefs[] =
   {
      { CAPABILITIES_NO_RX_CHANNELS, "No Rx Channels"},
      { CAPABILITIES_NO_TX_CHANNELS, "No Tx Channels"},
      { CAPABILITIES_NO_RX_MESSAGES, "No Rx Messages"},
      { CAPABILITIES_NO_TX_MESSAGES, "No Tx Messages"},
      { CAPABILITIES_NO_ACKD_MESSAGES, "No Ack Messages"},
      { CAPABILITIES_NO_BURST_TRANSFER, "No Burst Transfer"}
   };
   capabilityDefs extendedCapDefs1[] =
   {
      { CAPABILITIES_NETWORK_ENABLED, "Private Networks"},
      { CAPABILITIES_SERIAL_NUMBER_ENABLED, "Serial Number"},
      { CAPABILITIES_PER_CHANNEL_TX_POWER_ENABLED, "Per Channel Tx Power"},
      { CAPABILITIES_LOW_PRIORITY_SEARCH_ENABLED, "Low Priority Search"},
      { CAPABILITIES_SENSRCORE_ENABLED, "SensRcore Mode"},
      { CAPABILITIES_SEARCH_LIST_ENABLED, "Search List"}
   };

   capabilityDefs extendedCapDefs2[] =
   {
      { CAPABILITIES_LED_ENABLED, "Onboard LED"},
      { CAPABILITIES_EXT_MESSAGE_ENABLED, "Extended Message Support"},
      { CAPABILITIES_SCAN_CHANNEL_ENABLED, "Scan Channel"},
      { CAPABILITIES_TX_SEARCH_CMD_ENABLED, "Tx Search on Next"},
      { CAPABILITIES_PROX_SEARCH_ENABLED, "Proximity Search"},
      { CAPABILITIES_EXT_ASSIGN_ENABLED, "Extended Assign"},
      { CAPABILITIES_FS_ANTFS, "ANTFS"}
   };

   int i;
   //Now we handle the flags
   richTextBox_MainStatus->AppendText("\nDevice Sent Capabilities Info");
   richTextBox_deviceReport_1->AppendText(System::String::Concat("\n Max ANT Channels: ", pcBuffer_[0]));
   numChannels = pcBuffer_[0];
 
   // Store the capabilities bytes
   sMyCapabilities.ucStandardOptions = pcBuffer_[2];
   sMyCapabilities.ucAdvancedOptions = pcBuffer_[3];      
   sMyCapabilities.ucAdvancedOptions2 = pcBuffer_[4];

   // Need to call CreateChannels here using Invoke to avoid cross-thread issues. 
   MethodInvoker^ CreateChannelsDelegate = gcnew MethodInvoker(this, &MainForm::CreateChannels);
   this->Invoke(CreateChannelsDelegate);
   
   richTextBox_deviceReport_1->AppendText(System::String::Concat("\n Max Networks: ", pcBuffer_[1]));
   richTextBox_deviceReport_1->AppendText(System::String::Concat("\n Max Data Channels: ", pcBuffer_[5]));
   richTextBox_deviceReport_1->AppendText("\nBasic Disabilities: ");
   for(i=NUM_BASIC_CAPS-1; i>=0; --i)
   {
      if(pcBuffer_[2] & basicCapDefs[i].ucBitMask)
      {
         richTextBox_deviceReport_1->AppendText("\n  -");
         richTextBox_deviceReport_1->AppendText(gcnew System::String(basicCapDefs[i].flagDefinition));
      }
   }
   if(!pcBuffer_[2])
      richTextBox_deviceReport_1->AppendText("None");
   
   richTextBox_deviceReport_1->AppendText("\nExtended Capabilities: ");
   
   for(i=NUM_EXTD_CAPS1-1; i>=0; --i)
   {
      if(pcBuffer_[3] & extendedCapDefs1[i].ucBitMask)
      {
         richTextBox_deviceReport_1->AppendText("\n  -");
         richTextBox_deviceReport_1->AppendText(gcnew System::String(extendedCapDefs1[i].flagDefinition));
      }
   }
   for(i=NUM_EXTD_CAPS2-1; i>=0; --i)
   {
      if(pcBuffer_[4] & extendedCapDefs2[i].ucBitMask)
      {
         richTextBox_deviceReport_1->AppendText("\n  -");
         richTextBox_deviceReport_1->AppendText(gcnew System::String(extendedCapDefs2[i].flagDefinition));
      }
   }
   
   if(!pcBuffer_[3] && !pcBuffer_[4])
       richTextBox_deviceReport_1->AppendText("None");
}

/**************************************************************************
 * createChannels
 * 
 * Creates an array of channels, based on the device capabilities
 * 
 * returns: N/A
 *
 **************************************************************************/
void MainForm::CreateChannels()
{
   int i;


   channelList = gcnew array<ANTChannel^, 1>(numChannels);



   for(i=0; i<numChannels; ++i)
   {
       channelList[i] = gcnew ANTChannel((UCHAR) i, ucSimType, eMyANTPlatform, sMyCapabilities);
       tabControl_Channels->Controls->Add(channelList[i]->getTab());
   }


}

/**************************************************************************
 * Connect
 * 
 * If not connected, attempt to connect to ANT device. Will initialize ANT 
 * library, connect at selected baud rate, set non channel specific parameters 
 * (like ANT+ network key) and create the device GUI. If already connected, 
 * will disconnect from device and library and remove device panel GUI. IF 
 * fail, message will be reported. This function does not block pending
 * responses from ANT. 
 *  
 * Params: N/A
 *
 * returns: N/A
 *
 **************************************************************************/      
void MainForm::Connect()
{
	UCHAR aucANTSportNetworkKey[] = ANTPLUS_NETWORK_KEY;

   this->button_Connect->Enabled = false;
   this->comboBox_DeviceSelect->Enabled = false;
   this->numericUpDown_USBSelect->Enabled = false;
   this->textBox_baudRate->Enabled = false;
   

   if(!bMyConnected)
   {
	   ULONG ulBaud;
	   UCHAR ucUSB;
      UCHAR portType = PORT_TYPE_USB;
      UCHAR framerType = FRAMER_TYPE_BASIC;

	   try
      {
		   if(comboBox_DeviceSelect->SelectedItem->Equals("AP2/USB2/AT3"))
         {
            ulBaud = 57600;		   
            eMyANTPlatform = PLATFORM_AP2_USB2_AT3;
         }
         else if(comboBox_DeviceSelect->SelectedItem->Equals("C7"))
         {
			   ulBaud = 57600;
            eMyANTPlatform = PLATFORM_C7;
         }
		   else if(comboBox_DeviceSelect->SelectedItem->Equals("AP1/USB1"))
         {
			   ulBaud = 50000;
            eMyANTPlatform = PLATFORM_AP1_ANT11TR;
         }
         else
         {
            try
            {
               ulBaud = System::UInt32::Parse(this->textBox_baudRate->Text);
            }
            catch(...)
            {
               this->richTextBox_MainStatus->AppendText("\nCould not parse baud rate. Try again using a valid unsigned long in decimal format.");
            }

            if(comboBox_DeviceSelect->SelectedItem->Equals("Custom USB"))
            {
               eMyANTPlatform = PLATFORM_UNKNOWN;
            }
            else
            {
               throw "Invalid selection";
            }            
         }
      }
	   catch(...)
      {
		   this->richTextBox_MainStatus->AppendText("\nPlease select a valid device before connecting.");
		   this->button_Connect->Enabled = true;
		   this->comboBox_DeviceSelect->Enabled = true;
		   this->numericUpDown_USBSelect->Enabled = true;
         this->textBox_baudRate->Enabled = true;
		   return;
	   }
   
	   ucUSB = (UCHAR)numericUpDown_USBSelect->Value;

      // Load ANT Library
      if( !ANTClass::LibInit((ANT_EVENT_RESPONSE_FUNC )UnmanagedEventSender) )
      {
         MessageBox::Show("Failed to load ANT DLL", "Error", MessageBoxButtons::OK,MessageBoxIcon::Error, MessageBoxDefaultButton::Button1);
         return;
	   }

	   // Get library version
	   const UCHAR* aucVerBuffer = ANTClass::GetLibVersion();
	    
		if(aucVerBuffer)
         strLibVersion = gcnew System::String((char*) aucVerBuffer);
      else
         strLibVersion = nullptr;

      // Initialize the ANT Library
      if (ANTClass::Init(safe_cast<UCHAR>(numericUpDown_USBSelect->Value),ulBaud, portType, framerType) == TRUE)
      {

         ANTClass::ResetSystem();   // Will sleep for 600 ms

         // Update the GUI
			button_Connect->Text = "Disconnect";
			bMyConnected = TRUE;
			richTextBox_MainStatus->AppendText("\n...Connecting");
			CreateDevicePanel();

         // Set the Network Key 
         {            
	         UCHAR aucANTSportNetworkKey[] = ANTPLUS_NETWORK_KEY;
	         ANTClass::SetNetworkKey(ANTPLUS_NETWORK_NUMBER, aucANTSportNetworkKey);
         }
         
      }
      else
      {
	      this->comboBox_DeviceSelect->Enabled = true;
	      this->numericUpDown_USBSelect->Enabled = true;
         this->textBox_baudRate->Enabled = true;
      
         MessageBox::Show("ANT Initialization failed", "Error", MessageBoxButtons::OK,MessageBoxIcon::Error, MessageBoxDefaultButton::Button1);
      }
   }
   else
   {
	   ANTClass::Disconnect();
            
      // Update GUI
		button_Connect->Text = "Connect";
		bMyConnected = FALSE;
		richTextBox_MainStatus->AppendText("\n...Disconnecting");
		RemoveDevicePanel();
		
      if(channelList){
			while(numChannels)
				delete channelList[--numChannels];
			delete channelList;
		}
           
      this->Height = 553;
	   numChannels=0;
	   this->comboBox_DeviceSelect->Enabled = true;
	   this->numericUpDown_USBSelect->Enabled = true;
      this->textBox_baudRate->Enabled = true;
   }
   
   this->button_Connect->Enabled = true;

}

/**************************************************************************
 * ProcessChannelEvent
 * 
 * Process ANT channel events by passing them off to the appropriate channel
 *
 * ucChannel_: ANT channel
 * ucMessageCode_: message code returned from ANT
 * pucBuffer_: Buffer of message recieved from ANT
 *
 * returns: N/A
 *
 **************************************************************************/
void MainForm::ProcessChannelEvent(UCHAR ucChannel_, UCHAR ucMessageCode_, UCHAR*  pucBuffer_)
{
   if(channelList && ucChannel_ < numChannels)
	   channelList[ucChannel_]->ANTChannelEvent(ucMessageCode_, pucBuffer_);
}

/**************************************************************************
 * ProcessProtocolEvent
 * 
 * Process ANT protocol responses.
 *
 * ucChannel_: channel number
 * ucMessageCode_: message code received from ANT
 * pucBuffer_: Buffer of message recieved from ANT
 *
 * returns: N/A
 *
 **************************************************************************/
void MainForm::ProcessProtocolEvent(UCHAR ucChannel_, UCHAR ucMessageCode_, UCHAR* pucBuffer_)
{
   BOOL bPassOn = FALSE;

   switch(ucMessageCode_)
   {

      case MESG_CAPABILITIES_ID:
      {     
         InterpretCapabilities(&pucBuffer_[0]);
                
         ANTClass::AssignChannelEventFunctions(numChannels, (ANT_RESPONSE_FUNC) UnmanagedSender);
         
         
         // Check if serial number available, if it is then 
         // request it.
         if(pucBuffer_[3] & CAPABILITIES_SERIAL_NUMBER_ENABLED)
            ANTClass::RequestMessage(0,MESG_GET_SERIAL_NUM_ID);
         
         break;
      }
      case MESG_GET_SERIAL_NUM_ID:
      {
      
			int iSerialNum = (pucBuffer_[0] | (pucBuffer_[1] << 8) | (pucBuffer_[2] << 16) | (pucBuffer_[3] << 24));
			richTextBox_MainStatus->AppendText("\nDevice Sent Serial Number");
			richTextBox_deviceReport_1->AppendText(System::String::Concat("\n Serial Number: ", iSerialNum));
         
         // If serial number avialable, assume that version is 
         // as well so request it.
	      ANTClass::RequestMessage(0,MESG_VERSION_ID);
         
         break;
      }
      
      case MESG_VERSION_ID:
      {
			richTextBox_MainStatus->AppendText("\nDevice Sent Version Info");
			richTextBox_deviceReport_1->AppendText(System::String::Concat("\n RF Protocol Software Version: ", gcnew System::String((char*)pucBuffer_)));
         break;
      }

      case MESG_RESPONSE_EVENT_ID:
      { 
         // Handle responses to non-channel specific commands here
         // Pass the other events to individual channels to handle.  
         if(pucBuffer_[2] == RESPONSE_NO_ERROR)
         {
            switch (pucBuffer_[1])
            {
               case MESG_NETWORK_KEY_ID:
               {
                  // Request capabilites from the ANT device. 
	               ANTClass::RequestMessage(0,MESG_CAPABILITIES_ID);                  
                  break;
               }
               case MESG_RADIO_TX_POWER_ID:
               {
                  richTextBox_MainStatus->AppendText("\nDevice Changed Transmit Power");              
                  break;               
               }
               default:
               {        
                  bPassOn = TRUE;
                  break;
               }
            }
         }
         else
         {
            String^ strError = gcnew String ("\nError Response " + pucBuffer_[2].ToString() + " to command " + pucBuffer_[1].ToString() + "\n");
		      richTextBox_MainStatus->AppendText(strError);

            if( pucBuffer_[2] == EVENT_COMMAND_TIMEOUT)
            {
			      richTextBox_MainStatus->AppendText("COMMAND TIMEOUT!!!\n");
            }
         }
         break;
      }
      default:
      {
         bPassOn = TRUE;
         break;
      }
   }
   
   // If not handled here, pass it off to the individual channel.
   if(bPassOn && channelList && ucChannel_ < numChannels)
      channelList[ucChannel_]->ANTProtocolEvent(ucMessageCode_, pucBuffer_);
 
}



