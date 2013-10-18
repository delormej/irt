/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/
 

#pragma once
#include "StdAfx.h"
#include "AudioDevice.h"

void AudioDevice::onTimerTock(USHORT eventTime)
{
}
void AudioDevice::ANT_eventNotification(UCHAR ucEventCode_, UCHAR *pucEventBuffer_)
{
   switch(ucEventCode_)
   {
   case EVENT_TX:
      HandleTransmit(pucEventBuffer_);
      UpdateUI();
      break;
   case EVENT_RX_ACKNOWLEDGED:
   case EVENT_RX_BURST_PACKET:
   case EVENT_RX_BROADCAST:
      HandleReceive((UCHAR*)pucEventBuffer_);
      break;
   default:
      break;
   }
}
void AudioDevice::InitializeSim()
{
   AudioData->usCurrentTrackTime = (USHORT)nmCurrentTime->Value;
   AudioData->usTotalTrackTime = (USHORT)nmTotalTime->Value;
   if(cbAudioState->SelectedIndex == 5)
      AudioData->ucState = 0xF0;
   else
      AudioData->ucState = (UCHAR)cbAudioState->SelectedIndex | (UCHAR)cbRepeatState->SelectedIndex | (UCHAR)cbShuffleState->SelectedIndex;
   AudioData->ucVolume = (UCHAR)nmVolume->Value;

   ulTotalTime = 0;

   // required common pages
	CommonPages->ulSerialNum = System::Convert::ToUInt32(this->textBox_SerialNumber->Text);
	CommonPages->ucSwVersion = System::Convert::ToByte(this->textBox_SoftwareVer->Text);
	CommonPages->ucHwVersion = System::Convert::ToByte(this->textBox_HardwareVer->Text);
	CommonPages->usMfgID = System::Convert::ToUInt16(this->textBox_ManfID->Text);
	CommonPages->usModelNum = System::Convert::ToUInt16(this->textBox_ModelNumber->Text);
	
   //battery common page
	CommonPages->eTimeResolution = CommonData::TimeResolution::TWO;
	CommonPages->ulOpTime = ulTotalTime;
	CommonPages->bBattPageEnabled = FALSE;
	//Memory level common page
	CommonPages->ucPercentUsed = System::Convert::ToByte(this->nmPercentUsed->Value);
	CommonPages->ucTotalSizeExponent = System::Convert::ToByte(this->nmSizeExponent->Value);
	
	if(rbByteUnit->Checked)
		CommonPages->ucTotalSizeUnit = (UCHAR)CommonData::TotalSizeUnits::BYTE;
	else
		CommonPages->ucTotalSizeUnit = (UCHAR)CommonData::TotalSizeUnits::BIT;

	if(cbSizeUnit->Text->Equals("Base"))
		CommonPages->ucTotalSizeUnit |= (UCHAR)CommonData::TotalSizeUnits::BASE_UNIT;
	else if(cbSizeUnit->Text->Equals("Kilo"))
		CommonPages->ucTotalSizeUnit |= (UCHAR)CommonData::TotalSizeUnits::KILO;
	else if(cbSizeUnit->Text->Equals("Mega"))
		CommonPages->ucTotalSizeUnit |= (UCHAR)CommonData::TotalSizeUnits::MEGA;
	else if(cbSizeUnit->Text->Equals("Tera"))
		CommonPages->ucTotalSizeUnit |= (UCHAR)CommonData::TotalSizeUnits::TERA;

	//Peripheral Devices common page
	CommonPages->ucPeripheralDeviceIndex = System::Convert::ToByte(textBox_DeviceIndex->Text);
	CommonPages->ucTotalConnectedDevices = System::Convert::ToByte(textBox_TotalDevices->Text);
	CommonPages->ulDeviceChannelId = System::Convert::ToUInt16(textBox_DeviceNumber->Text) | (System::Convert::ToByte(textBox_TxType->Text) << 16) | (System::Convert::ToByte(textBox_DeviceType->Text) << 24);
		
	if(cbConnectionState->Text->Equals("Closed"))
		CommonPages->eConnectionState = CommonData::ConnectionStates::CLOSED;
	else if(cbConnectionState->Text->Equals("Searching"))
		CommonPages->eConnectionState = CommonData::ConnectionStates::SEARCHING;
	else if(cbConnectionState->Text->Equals("Tracking"))
		CommonPages->eConnectionState = CommonData::ConnectionStates::TRACKING;
	
	if(cbPaired->Checked)
		CommonPages->ePairingState = CommonData::PairingStates::PAIRED;
	else
		CommonPages->ePairingState = CommonData::PairingStates::UNPAIRED;

	if(cbNetworkKey->Text->Equals("Public"))
		CommonPages->eNetworkKey = CommonData::NetworkKeys::NETWORK_PUBLIC;
	else if(cbNetworkKey->Text->Equals("Private"))
		CommonPages->eNetworkKey = CommonData::NetworkKeys::NETWORK_PRIVATE;
	else if(cbNetworkKey->Text->Equals("ANT+"))
		CommonPages->eNetworkKey = CommonData::NetworkKeys::NETWORK_ANTPLUS;
	else if(cbNetworkKey->Text->Equals("ANT-FS"))
		CommonPages->eNetworkKey = CommonData::NetworkKeys::NETWORK_ANTFS;

   bFastForward = false;
   bRewind = false;

   UpdateCommonPattern();
}
void AudioDevice::HandleTransmit(UCHAR* pucTxBuffer_)
{
   static ULONG ulMessageCount = 0;
   static UCHAR ucCommonPageCount = 0;

   if(!(ulMessageCount % AudioData->COMMON_INTERVAL))
   {
      if(ucCommonPageCount >= aucCommonPattern.Count)
         ucCommonPageCount = 0;

      CommonPages->Encode(aucCommonPattern[ucCommonPageCount], pucTxBuffer_);

      ucCommonPageCount++;
   }
   else
   {
      UpdateAudioTime();
      AudioData->Encode(AudioData->PAGE_AUDIO_UPDATE, pucTxBuffer_);
   }
   ulMessageCount++;

   static UCHAR ucTimeCount = 0;
   if(ucTimeCount == 3)
   {
      ulTotalTime++;
      CommonPages->ulOpTime = ulTotalTime / (UCHAR) CommonPages->eTimeResolution;
      ucTimeCount = 0;
   }
   ucTimeCount++;
}
void AudioDevice::UpdateUI()
{
   label_Bat_ElpTimeDisplay->Text = ulTotalTime.ToString();
}
void AudioDevice::HandleReceive(UCHAR *pucRxBuffer_)
{
   UCHAR ucPageNumber = pucRxBuffer_[0];

   switch(ucPageNumber)
   {
   case AudioData->PAGE_AUDIO_VIDEO_COMMAND:
      AudioData->Decode(pucRxBuffer_);
      if(AudioData->ucPage16Command & 0x80)
         return;
      ReceiveAudioCommand();
   default:
      break;
   }
}
void AudioDevice::ReceiveAudioCommand()
{
   UCHAR ucVolumeIncrement = 1;

   switch(AudioData->ucPage16Command)
   {
   case AntPlusControls::AudioVideoCommands::PLAY:  
      AudioData->ucState &= (UCHAR)AntPlusControls::AudioState::CLEAR_AUDIO_STATE;
      AudioData->ucState |= (UCHAR)AntPlusControls::AudioState::PLAY;
      cbAudioState->Text = "Play";
      lbCommand->Text = "Play";
      bFastForward = false;
      bRewind = false;
      break;
   case AntPlusControls::AudioVideoCommands::PAUSE:
      AudioData->ucState &= (UCHAR)AntPlusControls::AudioState::CLEAR_AUDIO_STATE;
      AudioData->ucState |= (UCHAR)AntPlusControls::AudioState::PAUSE;
      cbAudioState->Text = "Pause";
      lbCommand->Text = "Pause";
      bFastForward = false;
      bRewind = false;
      break;
   case AntPlusControls::AudioVideoCommands::STOP:
      AudioData->ucState &= (UCHAR)AntPlusControls::AudioState::CLEAR_AUDIO_STATE;
      AudioData->ucState |= (UCHAR)AntPlusControls::AudioState::STOP;
      AudioData->usCurrentTrackTime = 0;
      cbAudioState->Text = "Stop";
      lbCommand->Text = "Stop";
      bFastForward = false;
      bRewind = false;
      break;
   case AntPlusControls::AudioVideoCommands::VOLUME_UP:
      if(AudioData->ucVolume == AntPlusControls::UNKNOWN_BYTE)
         break;

      if(!(AudioData->ucPage16Volume == AntPlusControls::UNKNOWN_BYTE))
         ucVolumeIncrement = AudioData->ucPage16Volume;
      if((AudioData->ucVolume + ucVolumeIncrement) >= 100)
         AudioData->ucVolume = 100;
      else
         AudioData->ucVolume += ucVolumeIncrement;

      nmVolume->Value = AudioData->ucVolume;
      lbCommand->Text = "Volume +";
      break;
   case AntPlusControls::AudioVideoCommands::VOLUME_DOWN:
      if(AudioData->ucVolume == AntPlusControls::UNKNOWN_BYTE)
         break;

      if(!(AudioData->ucPage16Volume == AntPlusControls::UNKNOWN_BYTE))
         ucVolumeIncrement = AudioData->ucPage16Volume;
      if((AudioData->ucVolume - ucVolumeIncrement) <= 0)
         AudioData->ucVolume = 0;
      else
         AudioData->ucVolume -= ucVolumeIncrement;

      nmVolume->Value = AudioData->ucVolume;
      lbCommand->Text = "Volume -";
      break;
   case AntPlusControls::AudioVideoCommands::MUTE:
      if(AudioData->ucVolume != 0)
      {
         AudioData->ucVolume = 0;
         lbCommand->Text = "Mute";
      }
      else
      {
         AudioData->ucVolume = 50;
         lbCommand->Text = "Un-mute";
      }

      nmVolume->Value = AudioData->ucVolume;
      break;
   case AntPlusControls::AudioVideoCommands::TRACK_AHEAD:
      lbCommand->Text = "Track Ahead";
      AudioData->ucState &= (UCHAR)AntPlusControls::AudioState::CLEAR_AUDIO_STATE;
      AudioData->ucState |= (UCHAR)AntPlusControls::AudioState::PLAY;
      AudioData->usCurrentTrackTime = 0;
      AudioData->usTotalTrackTime = 209;
      bFastForward = false;
      bRewind = false;
      break;
   case AntPlusControls::AudioVideoCommands::TRACK_BACK:
      lbCommand->Text = "Track Back";
      AudioData->ucState &= (UCHAR)AntPlusControls::AudioState::CLEAR_AUDIO_STATE;
      AudioData->ucState |= (UCHAR)AntPlusControls::AudioState::PLAY;
      AudioData->usCurrentTrackTime = 0;
      AudioData->usTotalTrackTime = 180;
      bFastForward = false;
      bRewind = false;
      break;
   case AntPlusControls::AudioVideoCommands::FFW:
      lbCommand->Text = "FFWD";
      bFastForward = true;
      bRewind = false;
      break;
   case AntPlusControls::AudioVideoCommands::FRWD:
      lbCommand->Text = "FRWD";
      bFastForward = false;
      bRewind = true;
      break;
   case AntPlusControls::AudioVideoCommands::REPEAT_CURRENT_TRACK:
      AudioData->ucState &= (UCHAR)AntPlusControls::RepeatState::CLEAR_REPEAT_STATE;
      AudioData->ucState |= (UCHAR)AntPlusControls::RepeatState::CURRENT_TRACK;
      lbCommand->Text = "Rep. Track";
      cbRepeatState->Text = "Track";
      break;
   case AntPlusControls::AudioVideoCommands::REPEAT_ALL:
      AudioData->ucState &= (UCHAR)AntPlusControls::RepeatState::CLEAR_REPEAT_STATE;
      AudioData->ucState |= (UCHAR)AntPlusControls::RepeatState::ALL_SONGS;
      lbCommand->Text = "Rep. All";
      cbRepeatState->Text = "All";
      break;
   case AntPlusControls::AudioVideoCommands::REPEAT_OFF:
      AudioData->ucState &= (UCHAR)AntPlusControls::RepeatState::CLEAR_REPEAT_STATE;
      AudioData->ucState |= (UCHAR)AntPlusControls::RepeatState::OFF;
      lbCommand->Text = "Rep. Off";
      cbRepeatState->Text = "Off";
      break;
   case AntPlusControls::AudioVideoCommands::SHUFFLE_SONGS:
      AudioData->ucState &= (UCHAR)AntPlusControls::ShuffleState::CLEAR_SHUFFLE_STATE;
      AudioData->ucState |= (UCHAR)AntPlusControls::ShuffleState::TRACK_LEVEL;
      lbCommand->Text = "Shfl. Songs";
      cbShuffleState->Text = "Songs";
      break;
   case AntPlusControls::AudioVideoCommands::SHUFFLE_ALBUMS:
      AudioData->ucState &= (UCHAR)AntPlusControls::ShuffleState::CLEAR_SHUFFLE_STATE;
      AudioData->ucState |= (UCHAR)AntPlusControls::ShuffleState::ALBUM_LEVEL;
      lbCommand->Text = "Shfl. Album";
      cbShuffleState->Text = "Album";
      break;
   case AntPlusControls::AudioVideoCommands::SHUFFLE_OFF:
      AudioData->ucState &= (UCHAR)AntPlusControls::ShuffleState::CLEAR_SHUFFLE_STATE;
      AudioData->ucState |= (UCHAR)AntPlusControls::ShuffleState::OFF;
      lbCommand->Text = "Shfl. Off";
      cbShuffleState->Text = "Off";
      break;
   case AntPlusControls::AudioVideoCommands::REPEAT_CUSTOM:
      if(cbCustomRepeat->Checked)
      {
         AudioData->ucState &= (UCHAR)AntPlusControls::RepeatState::CLEAR_REPEAT_STATE;
         AudioData->ucState |= (UCHAR)AntPlusControls::RepeatState::CUSTOM;
         lbCommand->Text = "Rep. Custom";
         cbRepeatState->Text = "Custom";
      }
      break;
   case AntPlusControls::AudioVideoCommands::SHUFFLE_CUSTOM:
      if(cbCustomShuffle->Checked)
      {
         AudioData->ucState &= (UCHAR)AntPlusControls::ShuffleState::CLEAR_SHUFFLE_STATE;
         AudioData->ucState |= (UCHAR)AntPlusControls::ShuffleState::CUSTOM;
         lbCommand->Text = "Shfl. Custom";
         cbShuffleState->Text = "Custom";
      }
      break;
   default:
      break;
   }
}
void AudioDevice::UpdateCommonPattern()
{
	aucCommonPattern.Clear();
	aucCommonPattern.Add(CommonPages->PAGE80);
	aucCommonPattern.Add(CommonPages->PAGE81);

	switch(checkBox_BatStatus->Checked)
	{
	case TRUE:
		//Add battery status to common page rotation
		aucCommonPattern.Add(CommonPages->PAGE82);
		switch(checkBox_MemoryLevel->Checked)
		{
		case TRUE:
			//Add Memory level to common page rotation
			aucCommonPattern.Add(CommonPages->PAGE85);
			switch(checkBox_PairedDevices->Checked)
			{
			case TRUE:
				//Add paired devices to common page rotation
				aucCommonPattern.Add(CommonPages->PAGE86);
				break;
			default:
				break;
			}
			break;
		default:
			switch(checkBox_PairedDevices->Checked)
			{
			case TRUE:
				//Add paired devices to common page rotation
				aucCommonPattern.Add(CommonPages->PAGE86);
				break;
			default:
				break;
			}
			break;
		}
		break;
	default:
		switch(checkBox_MemoryLevel->Checked)
		{
		case TRUE:
			//Add Memory level to common page rotation
			aucCommonPattern.Add(CommonPages->PAGE85);
			switch(checkBox_PairedDevices->Checked)
			{
			case TRUE:
				//Add paired devices to common page rotation
				aucCommonPattern.Add(CommonPages->PAGE86);
				break;
			default:
				break;
			}
			break;
		default:
			switch(checkBox_PairedDevices->Checked)
			{
			case TRUE:
				//Add paired devices to common page rotation
				aucCommonPattern.Add(CommonPages->PAGE86);
				break;
			default:
				break;
			}
			break;
		}
		break;
	}

	aucCommonPattern.TrimExcess();
}
void AudioDevice::cbAudioState_SelectedIndexChanged(System::Object ^sender, System::EventArgs ^e)
{
   //Check the selected index
	switch(cbAudioState->SelectedIndex)
	{
	case 0x00:	//"Off" was selected
		AudioData->ucState &= (UCHAR)AntPlusControls::AudioState::CLEAR_AUDIO_STATE;	
		AudioData->ucState |= (UCHAR)AntPlusControls::AudioState::OFF;	
      AudioData->usCurrentTrackTime = 0;
      nmCurrentTime->Value = 0;
		break;
	case 0x01:	//"Play" was selected
      AudioData->ucState &= (UCHAR)AntPlusControls::AudioState::CLEAR_AUDIO_STATE;
      AudioData->ucState |= (UCHAR)AntPlusControls::AudioState::PLAY;
		break;
	case 0x02:	//"Pause" was selected
      AudioData->ucState &= (UCHAR)AntPlusControls::AudioState::CLEAR_AUDIO_STATE;
      AudioData->ucState |= (UCHAR)AntPlusControls::AudioState::PAUSE;
		break;
	case 0x03:	//"Stop" was selected
      AudioData->ucState &= (UCHAR)AntPlusControls::AudioState::CLEAR_AUDIO_STATE;
      AudioData->ucState |= (UCHAR)AntPlusControls::AudioState::STOP;
      AudioData->usCurrentTrackTime = 0;
      nmCurrentTime->Value = 0;
		break;
	case 0x04:	//"Busy" was selected
	   AudioData->ucState &= (UCHAR)AntPlusControls::AudioState::CLEAR_AUDIO_STATE;
      AudioData->ucState |= (UCHAR)AntPlusControls::AudioState::BUSY;
		break;
	case 0x05:	//"Unknown" was selected
	   AudioData->ucState &= (UCHAR)AntPlusControls::AudioState::CLEAR_AUDIO_STATE;
      AudioData->ucState |= (UCHAR)AntPlusControls::AudioState::UNKNOWN;
		break;
	default:
		break;
	}

   bFastForward = false;
   bRewind = false;
}
void AudioDevice::cbRepeatState_SelectedIndexChanged(System::Object ^sender, System::EventArgs ^e)
{
	//Check the selected index
   switch(cbRepeatState->SelectedIndex)
	{
   case 0x00:	//"Off" was selected
		AudioData->ucState &= (UCHAR)AntPlusControls::RepeatState::CLEAR_REPEAT_STATE;	
		AudioData->ucState |= (UCHAR)AntPlusControls::RepeatState::OFF;	
		break;
	case 0x01:	//"Current Track" was selected
		AudioData->ucState &= (UCHAR)AntPlusControls::RepeatState::CLEAR_REPEAT_STATE;	
		AudioData->ucState |= (UCHAR)AntPlusControls::RepeatState::CURRENT_TRACK;
		break;
	case 0x02:	//"All" was selected
		AudioData->ucState &= (UCHAR)AntPlusControls::RepeatState::CLEAR_REPEAT_STATE;	
		AudioData->ucState |= (UCHAR)AntPlusControls::RepeatState::ALL_SONGS;	
		break;
	case 0x03:	//"Custom" was selected
		AudioData->ucState &= (UCHAR)AntPlusControls::RepeatState::CLEAR_REPEAT_STATE;
		AudioData->ucState |= (UCHAR)AntPlusControls::RepeatState::CUSTOM;	
		break;
	default:
		break;
	}
}
void AudioDevice::cbShuffleState_SelectedIndexChanged(System::Object ^sender, System::EventArgs ^e)
{
   //Check the selected index
	switch(cbShuffleState->SelectedIndex)
	{
   case 0x00:	//"Off" was selected
		AudioData->ucState &= (UCHAR)AntPlusControls::ShuffleState::CLEAR_SHUFFLE_STATE;
		AudioData->ucState |= (UCHAR)AntPlusControls::ShuffleState::OFF;
		break;
	case 0x01:	//"Tracks" was selected
		AudioData->ucState &= (UCHAR)AntPlusControls::ShuffleState::CLEAR_SHUFFLE_STATE;
		AudioData->ucState |= (UCHAR)AntPlusControls::ShuffleState::TRACK_LEVEL;
		break;
	case 0x02:	//"Album" was selected
		AudioData->ucState &= (UCHAR)AntPlusControls::ShuffleState::CLEAR_SHUFFLE_STATE;	
		AudioData->ucState |= (UCHAR)AntPlusControls::ShuffleState::ALBUM_LEVEL;	
		break;
	case 0x03:	//"Custom" was selected
		AudioData->ucState &= (UCHAR)AntPlusControls::ShuffleState::CLEAR_SHUFFLE_STATE;	
		AudioData->ucState |= (UCHAR)AntPlusControls::ShuffleState::CUSTOM;	
	default:
		break;
	}
}
void AudioDevice::cbCustomRepeat_CheckedChanged(System::Object ^sender, System::EventArgs ^e)
{
   cbRepeatState->Items->Clear();
	
	if(cbCustomRepeat->Checked)
   {
		cbRepeatState->Items->AddRange(gcnew cli::array< System::Object^  >(4) {L"Off", L"Track", L"All", L"Custom"});
      AudioData->ucAudioCapabilities &= (UCHAR)~AntPlusControls::AudioCapabilities::IGNORE_BITFIELD;
      AudioData->ucAudioCapabilities |= (UCHAR)AntPlusControls::AudioCapabilities::CUSTOM_REPEAT;
   }
   else
	{
		cbRepeatState->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"Off", L"Track", L"All"});
      AudioData->ucAudioCapabilities &= (UCHAR)(~AntPlusControls::AudioCapabilities::CUSTOM_REPEAT);

      if(AudioData->ucState & (UCHAR)AntPlusControls::RepeatState::CUSTOM)
      {
         AudioData->ucState &= (UCHAR)AntPlusControls::RepeatState::CLEAR_REPEAT_STATE;
         AudioData->ucState |= (UCHAR)AntPlusControls::RepeatState::ALL_SONGS;
         cbRepeatState->Text = "All";
      }
   }
}
void AudioDevice::cbCustomShuffle_CheckedChanged(System::Object ^sender, System::EventArgs ^e)
{
   cbShuffleState->Items->Clear();

	if(cbCustomShuffle->Checked)
	{
		cbShuffleState->Items->AddRange(gcnew cli::array<System::Object^ >(4) {L"Off", L"Songs", L"Album", L"Custom"});
      AudioData->ucAudioCapabilities &= (UCHAR)~AntPlusControls::AudioCapabilities::IGNORE_BITFIELD;
		AudioData->ucAudioCapabilities |= (UCHAR)AntPlusControls::AudioCapabilities::CUSTOM_SHUFFLE;
	}
	else
	{
		cbShuffleState->Items->AddRange(gcnew cli::array<System::Object^ >(3) {L"Off", L"Songs", L"Album"});
      AudioData->ucAudioCapabilities &= (UCHAR)(~AntPlusControls::AudioCapabilities::CUSTOM_SHUFFLE);

      if(AudioData->ucState & (UCHAR)AntPlusControls::ShuffleState::CUSTOM)
		{
			AudioData->ucState &= (UCHAR)AntPlusControls::ShuffleState::CLEAR_SHUFFLE_STATE;
			AudioData->ucState |= (UCHAR)AntPlusControls::ShuffleState::ALBUM_LEVEL;
			cbShuffleState->Text = "Album";
		}
	}
}
void AudioDevice::nmVolume_ValueChanged(System::Object ^sender, System::EventArgs ^e)
{
   AudioData->ucVolume = (UCHAR)nmVolume->Value;
}
void AudioDevice::cbInvalidVolume_CheckedChanged(System::Object ^sender, System::EventArgs ^e)
{
   if(cbInvalidVolume->Checked)
   {
      nmVolume->Enabled = FALSE;
      AudioData->ucVolume = AntPlusControls::UNKNOWN_BYTE;
   }
   else
   {
      nmVolume->Enabled = TRUE;
      AudioData->ucVolume = (UCHAR)nmVolume->Value;
   }
}
void AudioDevice::nmTotalTime_ValueChanged(System::Object ^sender, System::EventArgs ^e)
{
   AudioData->usTotalTrackTime = (USHORT)nmTotalTime->Value;
}
void AudioDevice::nmCurrentTime_ValueChanged(System::Object ^sender, System::EventArgs ^e)
{
   AudioData->usCurrentTrackTime = (USHORT)nmCurrentTime->Value;
}
void AudioDevice::cbInvalidTotal_CheckedChanged(System::Object ^sender, System::EventArgs ^e)
{
   if(cbInvalidTotal->Checked)
   {
      nmTotalTime->Enabled = false;
      AudioData->usTotalTrackTime = AntPlusControls::RESERVED_SHORT;
   }
   else
   {
      nmTotalTime->Enabled = true;
      AudioData->usTotalTrackTime = (USHORT)nmTotalTime->Value;
   }
}
void AudioDevice::cbInvalidCurrent_CheckedChanged(System::Object ^sender, System::EventArgs ^e)
{
   if(cbInvalidCurrent->Checked)
   {
      nmCurrentTime->Enabled = false;
      AudioData->usCurrentTrackTime = AntPlusControls::RESERVED_SHORT;
   }
   else
   {
      nmCurrentTime->Enabled = true;
      AudioData->usCurrentTrackTime = (USHORT)nmCurrentTime->Value; 
   }
}
void AudioDevice::button_UpdateCommon_Click(System::Object ^sender, System::EventArgs ^e)
{
   //Set the error label to be invisible
	this->label_ErrorCommon->Visible = false;
	this->label_ErrorCommon->Text = "Error: ";

	//Try setting the Manuf. ID
	try{
		CommonPages->usMfgID = System::Convert::ToUInt16(this->textBox_ManfID->Text);
	}
	//Display an error if the Manf. ID is not in the correct format
	catch(...){
		this->label_ErrorCommon->Text = System::String::Concat(this->label_ErrorCommon->Text, "Manf. ID");
		this->label_ErrorCommon->Visible = true;
	}

	//Try setting the device serial number
	try{
		if(this->checkBox_NoSerial->Checked)
			CommonPages->ulSerialNum = 0xFFFFFFFF;
		else
			CommonPages->ulSerialNum = System::Convert::ToUInt32(this->textBox_SerialNumber->Text);
	}
	//Display an error if the serial number is not in the correct format
	catch(...){
		this->label_ErrorCommon->Text = System::String::Concat(this->label_ErrorCommon->Text, "Ser. #");
		this->label_ErrorCommon->Visible = true;
	}

	//Try setting the device hardware version
	try{
		CommonPages->ucHwVersion = System::Convert::ToByte(this->textBox_HardwareVer->Text);
	}
	//Display an error if the hardware version is not in the correct format
	catch(...){
		this->label_ErrorCommon->Text = System::String::Concat(this->label_ErrorCommon->Text, "HW Ver.");
		this->label_ErrorCommon->Visible = true;
	}

	//Try setting the software version 
	try{
		CommonPages->ucSwVersion = System::Convert::ToByte(this->textBox_SoftwareVer->Text);
	}
	//Display an error if the software version is not in the correct format
	catch(...){
		this->label_ErrorCommon->Text = System::String::Concat(this->label_ErrorCommon->Text, "SW Ver.");
		this->label_ErrorCommon->Visible = true;
	}
}
void AudioDevice::checkBox_BatStatus_CheckedChanged(System::Object ^sender, System::EventArgs ^e)
{
   if (this->checkBox_BatStatus->Checked == TRUE)
	{
		CommonPages->bBattPageEnabled = TRUE;
		this->comboBoxBatStatus->Enabled = TRUE;
		this->label_Bat_ElpTimeDisplay->Enabled = TRUE;
		this->textBox_Bat_ElpTimeChange->Enabled = TRUE;
		this->checkBox_Bat_Voltage->Enabled = TRUE;
		this->checkBox_Bat_Voltage->Checked = TRUE;
		this->numericUpDown_Bat_VoltFrac->Enabled = TRUE;
		this->numericUpDown_Bat_VoltInt->Enabled = TRUE;
		this->groupBox_Resol->Enabled = TRUE;
		this->button_Bat_ElpTimeUpdate->Enabled = TRUE;

		// update the variables
		UpdateBatStatus();
		if (this->radioButton_Bat_Elp16Units->Checked)
			CommonPages->eTimeResolution = CommonData::TimeResolution::SIXTEEN;
		
		if (this->radioButton_Bat_Elp2Units->Checked)
			CommonPages->eTimeResolution = CommonData::TimeResolution::TWO;

		CommonPages->ulOpTime = ulTotalTime / (UCHAR) CommonPages->eTimeResolution;
		CommonPages->usBatVoltage256 = ((USHORT) System::Convert::ToByte(this->numericUpDown_Bat_VoltInt->Value)) << 8 | (CommonPages->usBatVoltage256 & 0xFF);	// Integer portion in high byte
		CommonPages->usBatVoltage256 = System::Convert::ToByte(this->numericUpDown_Bat_VoltFrac->Value) | (CommonPages->usBatVoltage256 & 0xFF00);	// Fractional portion in low byte
	}

	else
	{
		CommonPages->bBattPageEnabled = FALSE;
		this->comboBoxBatStatus->Enabled = FALSE;
		this->label_Bat_ElpTimeDisplay->Enabled = FALSE;
		this->textBox_Bat_ElpTimeChange->Enabled = FALSE;
		this->checkBox_Bat_Voltage->Enabled = FALSE;
		this->checkBox_Bat_Voltage->Checked = FALSE;
		this->numericUpDown_Bat_VoltFrac->Enabled = FALSE;
		this->numericUpDown_Bat_VoltInt->Enabled = FALSE;
		this->groupBox_Resol->Enabled = FALSE;
		this->button_Bat_ElpTimeUpdate->Enabled = FALSE;
	}

   UpdateCommonPattern();
}
void AudioDevice::UpdateBatStatus()
{
   if (this->comboBoxBatStatus->Text->Equals("New"))
	{
		CommonPages->eBatStatus = CommonData::BatStatus::NEW;
	}

	else if (this->comboBoxBatStatus->Text->Equals("Good"))
	{
		CommonPages->eBatStatus = CommonData::BatStatus::GOOD;
	}

	else if (this->comboBoxBatStatus->Text->Equals("Ok"))
	{
		CommonPages->eBatStatus = CommonData::BatStatus::OK;
	}

	else if (this->comboBoxBatStatus->Text->Equals("Low"))
	{
		CommonPages->eBatStatus = CommonData::BatStatus::LOW;
	}

	else if (this->comboBoxBatStatus->Text->Equals("Critical"))
	{
		CommonPages->eBatStatus = CommonData::BatStatus::CRITICAL;
	}

	else if (this->comboBoxBatStatus->Text->Equals("Invalid"))
	{
		CommonPages->eBatStatus = CommonData::BatStatus::INVALID;
	}
}
void AudioDevice::comboBoxBatStatus_SelectedIndexChanged(System::Object ^sender, System::EventArgs ^e)
{
   UpdateBatStatus();
}
void AudioDevice::checkBox_Bat_Voltage_CheckedChanged(System::Object ^sender, System::EventArgs ^e)
{
   if (this->checkBox_Bat_Voltage->Checked == TRUE)
	{
		this->numericUpDown_Bat_VoltInt->Enabled = TRUE;
		this->numericUpDown_Bat_VoltFrac->Enabled = TRUE;
		CommonPages->usBatVoltage256 = ((USHORT) System::Convert::ToByte(this->numericUpDown_Bat_VoltInt->Value)) << 8 | (CommonPages->usBatVoltage256 & 0xFF);	// Integer portion in high byte
		CommonPages->usBatVoltage256 = System::Convert::ToByte(this->numericUpDown_Bat_VoltFrac->Value) | (CommonPages->usBatVoltage256 & 0xFF00);	// Fractional portion in low byte
	}
	else
	{
		this->numericUpDown_Bat_VoltInt->Enabled = FALSE;
		this->numericUpDown_Bat_VoltFrac->Enabled = FALSE;
		CommonPages->usBatVoltage256 = CommonPages->BATTERY_VOLTAGE_INVALID;
	}
}
void AudioDevice::numericUpDown_Bat_VoltInt_ValueChanged(System::Object ^sender, System::EventArgs ^e)
{
   CommonPages->usBatVoltage256 = ((USHORT) System::Convert::ToByte(this->numericUpDown_Bat_VoltInt->Value)) << 8 | (CommonPages->usBatVoltage256 & 0xFF);	// Integer portion in high byte
}
void AudioDevice::numericUpDown_Bat_VoltFrac_ValueChanged(System::Object ^sender, System::EventArgs ^e)
{
   CommonPages->usBatVoltage256 = System::Convert::ToByte(this->numericUpDown_Bat_VoltFrac->Value) | (CommonPages->usBatVoltage256 & 0xFF00);	// Fractional portion in low byte
}
void AudioDevice::radioButton_Bat_Elp2Units_CheckedChanged(System::Object ^sender, System::EventArgs ^e)
{
   if (radioButton_Bat_Elp2Units->Checked)
		CommonPages->eTimeResolution = CommonData::TimeResolution::TWO;
   else
      CommonPages->eTimeResolution = CommonData::TimeResolution::SIXTEEN;
}
void AudioDevice::button_Bat_ElpTimeUpdate_Click(System::Object ^sender, System::EventArgs ^e)
{
   ulTotalTime = System::Convert::ToUInt32(this->textBox_Bat_ElpTimeChange->Text);
	CommonPages->ulOpTime = ulTotalTime / (UCHAR) CommonPages->eTimeResolution;
}
void AudioDevice::button_UpdateMemLevel_Click(System::Object ^sender, System::EventArgs ^e)
{
   CommonPages->ucPercentUsed = System::Convert::ToByte(this->nmPercentUsed->Value);
	CommonPages->ucTotalSizeExponent = System::Convert::ToByte(this->nmSizeExponent->Value);
	
	if(rbByteUnit->Checked)
		CommonPages->ucTotalSizeUnit = (UCHAR)CommonData::TotalSizeUnits::BYTE;
	else
		CommonPages->ucTotalSizeUnit = (UCHAR)CommonData::TotalSizeUnits::BIT;

	if(cbSizeUnit->Text->Equals("Base"))
		CommonPages->ucTotalSizeUnit |= (UCHAR)CommonData::TotalSizeUnits::BASE_UNIT;
	else if(cbSizeUnit->Text->Equals("Kilo"))
		CommonPages->ucTotalSizeUnit |= (UCHAR)CommonData::TotalSizeUnits::KILO;
	else if(cbSizeUnit->Text->Equals("Mega"))
		CommonPages->ucTotalSizeUnit |= (UCHAR)CommonData::TotalSizeUnits::MEGA;
	else if(cbSizeUnit->Text->Equals("Tera"))
		CommonPages->ucTotalSizeUnit |= (UCHAR)CommonData::TotalSizeUnits::TERA;
}
void AudioDevice::checkBox_MemoryLevel_CheckedChanged(System::Object ^sender, System::EventArgs ^e)
{
   if(checkBox_MemoryLevel->Checked)
	{
		cbSizeUnit->Enabled = true;
		rbBitUnit->Enabled = true;
		rbByteUnit->Enabled = true;
		button_UpdateMemLevel->Enabled = true;
		nmSizeExponent->Enabled = true;
		nmPercentUsed->Enabled = true;
	}
	else
	{
		cbSizeUnit->Enabled = false;
		rbBitUnit->Enabled = false;
		rbByteUnit->Enabled = false;
		button_UpdateMemLevel->Enabled = false;
		nmSizeExponent->Enabled = false;
		nmPercentUsed->Enabled = false;
	}

   UpdateCommonPattern();
}
void AudioDevice::checkBox_PairedDevices_CheckedChanged(System::Object ^sender, System::EventArgs ^e)
{
   if(checkBox_PairedDevices->Checked)
	{
		textBox_DeviceNumber->Enabled = true;
		textBox_TxType->Enabled = true;
		textBox_DeviceType->Enabled = true;
		textBox_DeviceIndex->Enabled = true;
		textBox_TotalDevices->Enabled = true;
		cbConnectionState->Enabled = true;
		cbNetworkKey->Enabled = true;
		cbPaired->Enabled = true;
		button_UpdatePeripheral->Enabled = true;
	}
	else
	{
		textBox_DeviceNumber->Enabled = false;
		textBox_TxType->Enabled = false;
		textBox_DeviceType->Enabled = false;
		textBox_DeviceIndex->Enabled = false;
		textBox_TotalDevices->Enabled = false;
		cbConnectionState->Enabled = false;
		cbNetworkKey->Enabled = false;
		cbPaired->Enabled = false;
		button_UpdatePeripheral->Enabled = false;
	}

   UpdateCommonPattern();
}
void AudioDevice::button_UpdatePeripheral_Click(System::Object ^sender, System::EventArgs ^e)
{
   label_PairedError->Visible = false;
	
	try
	{
		CommonPages->ucPeripheralDeviceIndex = System::Convert::ToByte(textBox_DeviceIndex->Text);
		CommonPages->ucTotalConnectedDevices = System::Convert::ToByte(textBox_TotalDevices->Text);
		CommonPages->ulDeviceChannelId = System::Convert::ToUInt16(textBox_DeviceNumber->Text) | (System::Convert::ToByte(textBox_TxType->Text) << 16) | (System::Convert::ToByte(textBox_DeviceType->Text) << 24);
		
		if(cbConnectionState->Text->Equals("Closed"))
			CommonPages->eConnectionState = CommonData::ConnectionStates::CLOSED;
		else if(cbConnectionState->Text->Equals("Searching"))
			CommonPages->eConnectionState = CommonData::ConnectionStates::SEARCHING;
		else if(cbConnectionState->Text->Equals("Tracking"))
			CommonPages->eConnectionState = CommonData::ConnectionStates::TRACKING;
		
		if(cbPaired->Checked)
			CommonPages->ePairingState = CommonData::PairingStates::PAIRED;
		else
			CommonPages->ePairingState = CommonData::PairingStates::UNPAIRED;

		if(cbNetworkKey->Text->Equals("Public"))
			CommonPages->eNetworkKey = CommonData::NetworkKeys::NETWORK_PUBLIC;
		else if(cbNetworkKey->Text->Equals("Private"))
			CommonPages->eNetworkKey = CommonData::NetworkKeys::NETWORK_PRIVATE;
		else if(cbNetworkKey->Text->Equals("ANT+"))
			CommonPages->eNetworkKey = CommonData::NetworkKeys::NETWORK_ANTPLUS;
		else if(cbNetworkKey->Text->Equals("ANT-FS"))
			CommonPages->eNetworkKey = CommonData::NetworkKeys::NETWORK_ANTFS;
	}
	catch(...)
	{
		label_PairedError->Visible = true;
	}
}
void AudioDevice::UpdateAudioTime()
{
   static UCHAR ucTimeCount = 0;

   if(ucTimeCount >4)
      ucTimeCount = 0;

   switch(AudioData->ucState & (UCHAR)~AntPlusControls::AudioState::CLEAR_AUDIO_STATE)
   {
   case AntPlusControls::AudioState::PLAY:
      if(bFastForward)
      {
         if((AudioData->usCurrentTrackTime + 1) > AudioData->usTotalTrackTime)
         {
            AudioData->ucState &= (UCHAR)AntPlusControls::AudioState::CLEAR_AUDIO_STATE;
            AudioData->ucState |= (UCHAR)AntPlusControls::AudioState::STOP;
            cbAudioState->Text = "Stop";
         }
         else
            AudioData->usCurrentTrackTime++;

      }
      else if(bRewind)
      {
         if((AudioData->usCurrentTrackTime - 1) < 0)
         {
            AudioData->ucState &= (UCHAR)AntPlusControls::AudioState::CLEAR_AUDIO_STATE;
            AudioData->ucState |= (UCHAR)AntPlusControls::AudioState::STOP;
            cbAudioState->Text = "Stop";
         }
         else
            AudioData->usCurrentTrackTime--;
      }
      else if(ucTimeCount++ == 4)
      {
         if((AudioData->usCurrentTrackTime + 1) > AudioData->usTotalTrackTime)
         {
            AudioData->ucState &= (UCHAR)AntPlusControls::AudioState::CLEAR_AUDIO_STATE;
            AudioData->ucState |= (UCHAR)AntPlusControls::AudioState::STOP;
            cbAudioState->Text = "Stop";
         }
         else
         {
            AudioData->usCurrentTrackTime++;
            nmCurrentTime->Value = AudioData->usCurrentTrackTime;
         }
      }
   default:
      break;
   }
}
void AudioDevice::checkBox_NoSerial_CheckedChanged(System::Object ^sender, System::EventArgs ^e)
{
   if(checkBox_NoSerial->Checked)
      textBox_SerialNumber->Enabled = false;
   else 
      textBox_SerialNumber->Enabled = true;
}
void AudioDevice::checkBoxSongTitle_CheckedChanged(System::Object ^sender, System::EventArgs ^e)
{
   if(checkBoxSongTitle->Checked)
      AudioData->ucAudioCapabilities |= (UCHAR)AntPlusControls::AudioCapabilities::SONG_TITLE;
   else
      AudioData->ucAudioCapabilities &= (UCHAR)~AntPlusControls::AudioCapabilities::SONG_TITLE;
}