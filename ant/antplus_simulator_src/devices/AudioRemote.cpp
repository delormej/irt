/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/


#pragma once
#include "StdAfx.h"
#include "AudioRemote.h"

void AudioRemote::onTimerTock(USHORT eventTime)
{
}
void AudioRemote::ANT_eventNotification(UCHAR ucEventCode_, UCHAR *pucEventBuffer_)
{
   switch(ucEventCode_)
   {
   case EVENT_RX_BURST_PACKET:
   case EVENT_RX_ACKNOWLEDGED:
   case EVENT_RX_BROADCAST:
      //Check for common pages
      if(pucEventBuffer_[0] < CommonPages->START_COMMON_PAGE)
         AudioData->Decode(pucEventBuffer_);
      else
         CommonPages->Decode(pucEventBuffer_);

      UpdateUI(pucEventBuffer_[0]);
      break;
   case EVENT_TRANSFER_TX_COMPLETED:
      ucAckRetryCount = 0;
      ucMsgExpectingAck = 0;
      UpdateDisplayAckStatus(AudioData->ACK_SUCCESS);
      break;
   case EVENT_TRANSFER_TX_FAILED:
   case EVENT_ACK_TIMEOUT:
      if(ucMsgExpectingAck)
      {
         if(HandleRetransmit())
            UpdateDisplayAckStatus(AudioData->ACK_RETRY);
         else
            UpdateDisplayAckStatus(AudioData->ACK_FAIL);
      }
      break;
   default:
      break;
   }
}
void AudioRemote::InitializeSim()
{
   AudioData->usPage16SerialNumber = System::Convert::ToInt16(textBox_Serial->Text);
   ucAckRetryCount = 0;
   ucMsgExpectingAck = 0;
}
void AudioRemote::UpdateUI(UCHAR ucPageNumber_)
{
   System::String^ sizeUnit = "";

   switch(ucPageNumber_)
   {
   case AudioData->PAGE_AUDIO_UPDATE:
      if(AudioData->ucVolume == AntPlusControls::UNKNOWN_BYTE)
         lbVolume->Text = "Unknown";
      else
         lbVolume->Text = AudioData->ucVolume.ToString();

      if(AudioData->usTotalTrackTime == AntPlusControls::UNKNOWN_SHORT)
         lbTotalTrackTime->Text = "Unknown";
      else
         lbTotalTrackTime->Text = ConvertToMinSec(AudioData->usTotalTrackTime);

      if(AudioData->usCurrentTrackTime == AntPlusControls::UNKNOWN_SHORT)
         lbCurrentTrackTime->Text = "Unknown";
      else
         lbCurrentTrackTime->Text = ConvertToMinSec(AudioData->usCurrentTrackTime);
      
      if((UCHAR)(AudioData->ucState & (UCHAR)(~AntPlusControls::AudioState::CLEAR_AUDIO_STATE)) == 
         (UCHAR)AntPlusControls::AudioState::UNKNOWN)
      {
         lbAudioState->Text = "Unknown";
         lbRepeatState->Text = "Unknown";
         lbShuffleState->Text = "Unknown";
      }
      else
      {
         switch(AudioData->ucState & (UCHAR)(~AntPlusControls::AudioState::CLEAR_AUDIO_STATE))
         {
         case (UCHAR)AntPlusControls::AudioState::OFF:
            lbAudioState->Text = "Off";
            break;
         case (UCHAR)AntPlusControls::AudioState::PLAY:
            lbAudioState->Text = "Play";
            break;
         case (UCHAR)AntPlusControls::AudioState::PAUSE:
            lbAudioState->Text = "Pause";
            break;
         case (UCHAR)AntPlusControls::AudioState::STOP:
            lbAudioState->Text = "Stop";
            break;
         case (UCHAR)AntPlusControls::AudioState::BUSY:
            lbAudioState->Text = "Busy";
            break;
         case (UCHAR)AntPlusControls::AudioState::UNKNOWN:
            lbAudioState->Text = "Unknown";
            break;
         default:
            lbAudioState->Text = "----";
            break;
         }

         switch(AudioData->ucState & (UCHAR)(~AntPlusControls::RepeatState::CLEAR_REPEAT_STATE))
         {
         case (UCHAR)AntPlusControls::RepeatState::OFF:
            lbRepeatState->Text = "Off";
            break;
         case (UCHAR)AntPlusControls::RepeatState::CURRENT_TRACK:
            lbRepeatState->Text = "Track";
            break;
         case (UCHAR)AntPlusControls::RepeatState::ALL_SONGS:
            lbRepeatState->Text = "All";
            break;
         case (UCHAR)AntPlusControls::RepeatState::CUSTOM:
            lbRepeatState->Text = "Custom";
            break;
         default:
            lbRepeatState->Text = "----";
            break;
         }

         switch(AudioData->ucState & (UCHAR)(~AntPlusControls::ShuffleState::CLEAR_SHUFFLE_STATE))
         {
         case (UCHAR)AntPlusControls::ShuffleState::OFF:
            lbShuffleState->Text = "Off";
            break;
         case (UCHAR)AntPlusControls::ShuffleState::TRACK_LEVEL:
            lbShuffleState->Text = "Songs";
            break;
         case (UCHAR)AntPlusControls::ShuffleState::ALBUM_LEVEL:
            lbShuffleState->Text = "Album";
            break;
         case (UCHAR)AntPlusControls::ShuffleState::CUSTOM:
            lbShuffleState->Text = "Custom";
            break;
         default:
            lbShuffleState->Text = "----";
            break;
         }
      }

      if(AudioData->usTotalTrackTime < AudioData->usCurrentTrackTime ||
         AudioData->usCurrentTrackTime == AntPlusControls::UNKNOWN_SHORT ||
         AudioData->usTotalTrackTime == AntPlusControls::UNKNOWN_SHORT)
         pbAudio->Value = 0;
      else
      {
         pbAudio->Maximum = AudioData->usTotalTrackTime;
         pbAudio->Value = AudioData->usCurrentTrackTime;
      }

      if(!(AudioData->ucAudioCapabilities & (UCHAR)AntPlusControls::AudioCapabilities::IGNORE_BITFIELD))
      {
         if(AudioData->ucAudioCapabilities & (UCHAR)AntPlusControls::AudioCapabilities::CUSTOM_REPEAT)
            lbCustomRepeat->Text = "Supported";
         else
            lbCustomRepeat->Text = "Not Supported";
      
         if(AudioData->ucAudioCapabilities & (UCHAR)AntPlusControls::AudioCapabilities::CUSTOM_SHUFFLE)
            lbCustomShuffle->Text = "Supported";
         else
            lbCustomShuffle->Text = "Not Supported";

         if(AudioData->ucAudioCapabilities & (UCHAR)AntPlusControls::AudioCapabilities::SONG_TITLE)
         {
            lbSongTitle->Text = "----";
            if(!cbAudioCommands->Items->Contains("Song Title Text"))
            {
               cbAudioCommands->Items->AddRange(gcnew cli::array< System::Object^  >(19) {L"Play", L"Pause", L"Stop", L"Volume +", 
               L"Volume -", L"Mute/Unmute", L"Track Ahead", L"Track Back", L"Repeat Track", L"Repeat All", L"Repeat Off", L"Shuffle Songs", 
               L"Shuffle Albums", L"Shuffle Off", L"FFWD", L"FRWD", L"Custom Repeat", L"Custom Shuffle", L"Song Title Text"});
            }
         }
         else
         {
            lbSongTitle->Text = "Not Supported";
            if(cbAudioCommands->Items->Contains("Song Title Text"))
            {
               cbAudioCommands->Items->AddRange(gcnew cli::array< System::Object^  >(18) {L"Play", L"Pause", L"Stop", L"Volume +", 
               L"Volume -", L"Mute/Unmute", L"Track Ahead", L"Track Back", L"Repeat Track", L"Repeat All", L"Repeat Off", L"Shuffle Songs", 
               L"Shuffle Albums", L"Shuffle Off", L"FFWD", L"FRWD", L"Custom Repeat", L"Custom Shuffle"});
            }
         }
      }
      else
      {
         lbCustomShuffle->Text = "Not Supported";
         lbCustomRepeat->Text = "Not Supported";
         lbSongTitle->Text = "Not Supported";

         if(cbAudioCommands->Items->Contains("Song Title Text"))
         {
            cbAudioCommands->Items->AddRange(gcnew cli::array< System::Object^  >(18) {L"Play", L"Pause", L"Stop", L"Volume +", 
            L"Volume -", L"Mute/Unmute", L"Track Ahead", L"Track Back", L"Repeat Track", L"Repeat All", L"Repeat Off", L"Shuffle Songs", 
            L"Shuffle Albums", L"Shuffle Off", L"FFWD", L"FRWD", L"Custom Repeat", L"Custom Shuffle"});
         }
      }

      break;

       case CommonPages->PAGE80:
		if(CommonPages->usMfgID != 0)
		{
			this->label_HardwareVer->Text = CommonPages->ucHwVersion.ToString();
			this->label_ManuID->Text = CommonPages->usMfgID.ToString();
			this->label_ModelNumber->Text = CommonPages->usModelNum.ToString();
		}
		break;

   case CommonPages->PAGE81:
      if(CommonPages->ulSerialNum != 0)
		{
			if(CommonPages->ulSerialNum == 0xFFFFFFFF)
				this->label_SerialNumber->Text = "N/A";
			else
				this->label_SerialNumber->Text = CommonPages->ulSerialNum.ToString();

			this->label_SoftwareVer->Text = CommonPages->ucSwVersion.ToString();
		}
		break;

   case CommonPages->PAGE82:
		if (CommonPages->ulOpTime != 0)
		{
			this->labelBattVolt->Text = System::Math::Round((double)CommonPages->usBatVoltage256/256,4).ToString("N2");
		
			if (CommonPages->eTimeResolution == CommonData::TimeResolution::SIXTEEN)
				this->labelTimeRes->Text = "16";
			else
				this->labelTimeRes->Text = "2";

			// now that we know the time resolution we can display the run time
			this->labelOpTime->Text = (CommonPages->ulOpTime * (UCHAR) CommonPages->eTimeResolution).ToString();

			switch (CommonPages->eBatStatus)
			{
			case CommonData::BatStatus::CRITICAL:
				this->labelBattStatus->Text = "Critical";
				break;
			case CommonData::BatStatus::GOOD:
				this->labelBattStatus->Text = "Good";
				break;
			case CommonData::BatStatus::INVALID:
				this->labelBattStatus->Text = "Invalid";
				break;
			case CommonData::BatStatus::LOW:
				this->labelBattStatus->Text = "Low";
				break;
			case CommonData::BatStatus::NEW:
				this->labelBattStatus->Text = "New";
				break;
			case CommonData::BatStatus::OK:
				this->labelBattStatus->Text = "Ok";
				break;
			default:
				break;
			}
		}
		break;

   case CommonPages->PAGE85:
      		if(CommonPages->ucPercentUsed <= 100 && CommonPages->ucPercentUsed >= 0)
			lbPercentUsed->Text = CommonPages->ucPercentUsed.ToString();
		else
			lbPercentUsed->Text = "----";

		lbSizeExponent->Text = CommonPages->ucTotalSizeExponent.ToString();

		switch(CommonPages->ucTotalSizeUnit & 0x7F)
		{
		case 0:
			break;
		case 1:
			sizeUnit += "Kilo-";
			break;
		case 2:
			sizeUnit += "Mega-";
			break;
		case 3:
			sizeUnit += "Tera-";
			break;
		default:
			break;
		}

		if(CommonPages->ucTotalSizeUnit & 0x80)
			sizeUnit += "byte";
		else
			sizeUnit += "bit";

		lbSizeUnit->Text = sizeUnit;
		break;

   case CommonPages->PAGE86:
lbDeviceIndex->Text = CommonPages->ucPeripheralDeviceIndex.ToString();
		lbTotalDevices->Text = CommonPages->ucTotalConnectedDevices.ToString();
		lbDeviceId->Text = (CommonPages->ulDeviceChannelId & 0x0000FFFF).ToString() + "," + ((CommonPages->ulDeviceChannelId & 0x00FF0000) >> 16).ToString() + "," + ((CommonPages->ulDeviceChannelId & 0xFF000000) >> 24).ToString();

		switch(CommonPages->eNetworkKey)
		{
		case CommonData::NetworkKeys::NETWORK_ANTFS:
			lbNetworkKey->Text = "ANT-FS";
			break;
		case CommonData::NetworkKeys::NETWORK_ANTPLUS:
			lbNetworkKey->Text = "ANT+";
			break;
		case CommonData::NetworkKeys::NETWORK_PRIVATE:
			lbNetworkKey->Text = "Private";
			break;
		case CommonData::NetworkKeys::NETWORK_PUBLIC:
			lbNetworkKey->Text = "Public";
			break;
		default: 
			lbNetworkKey->Text = "----";
			break;
		}

		switch(CommonPages->eConnectionState)
		{
		case CommonData::ConnectionStates::CLOSED:
			lbConnStat->Text = "Closed";
			break;
		case CommonData::ConnectionStates::SEARCHING:
			lbConnStat->Text = "Searching";
			break;
		case CommonData::ConnectionStates::TRACKING:
			lbConnStat->Text = "Tracking";
			break;
		default:
			lbConnStat->Text = "----";
			break;
		}

		if(CommonPages->ePairingState == CommonData::PairingStates::PAIRED)
			lbPairingStat->Text = "Paired";
		else
			lbPairingStat->Text = "Not Paired";

		break;
   default:
      break;
   }
}
BOOL AudioRemote::HandleRetransmit()
{
   BOOL bSuccess = true;
   
   if(ucMsgExpectingAck)
   {
      if(ucAckRetryCount++ < AudioData->MAX_RETRIES)
         SendCommand(ucMsgExpectingAck);
      else
      {
         bSuccess = false;
         ucMsgExpectingAck = false;
         ucAckRetryCount = 0;
      }
   }

   return bSuccess;
}
void AudioRemote::SendCommand(UCHAR ucPageNumber_)
{
   UCHAR aucAckBuffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	switch(ucPageNumber_)
	{
	case AudioData->PAGE_AUDIO_VIDEO_COMMAND:
		AudioData->Encode(AudioData->PAGE_AUDIO_VIDEO_COMMAND, aucAckBuffer);
		break;
	default:
		return;
	}

	if(!ucMsgExpectingAck)	// If this message is not a retransmission
	{
		ucAckRetryCount = 0;			// Reset retransmission counter
		ucMsgExpectingAck = ucPageNumber_;	// Set code to message for which an ACK will be expected
	}

	requestAckMsg(aucAckBuffer);	// Send message
}
void AudioRemote::UpdateDisplayAckStatus(UCHAR ucStatus_)
{
   switch(ucStatus_)
   {
   case AudioData->ACK_SUCCESS:
      lbAckStatus->ForeColor = System::Drawing::Color::Green;
      lbAckStatus->Text = "SENT";
      break;
   case AudioData->ACK_RETRY:
      lbAckStatus->ForeColor = System::Drawing::Color::Blue;
      lbAckStatus->Text = "RTRY";
      break;
   case AudioData->ACK_FAIL:
      lbAckStatus->ForeColor = System::Drawing::Color::Red;
      lbAckStatus->Text = "FAIL";
      break;
   default:
      break;
   }
}
System::String^ AudioRemote::ConvertToMinSec(USHORT usTime_)
{
   //Set two temporary variables to hold the value of the minutes and seconds
	UCHAR Seconds = usTime_ % 60;
	UCHAR Minutes = (usTime_/60) % 60;
	UCHAR Hours = usTime_/3600;

	System::String^ time = "";

	if(Hours > 0)
      time = String::Format("{0}:{1}:{2}", Hours.ToString("D2"), Minutes.ToString("D2"), Seconds.ToString("D2"));
   else
      time = String::Format("{0}:{1}", Minutes.ToString("D2"), Seconds.ToString("D2"));

	return time;
}
void AudioRemote::btAudioCommands_Click(System::Object ^sender, System::EventArgs ^e)
{
   AudioData->ucPage16Command = (UCHAR)AntPlusControls::AudioVideoCommands::SET_AUDIO;

   if(cbInvalidVolInc->Checked)
      AudioData->ucPage16Volume = AntPlusControls::UNKNOWN_BYTE;
   else
      AudioData->ucPage16Volume = (UCHAR)nmAudioVolInc->Value;

   switch(cbAudioCommands->SelectedIndex)
   {
   case 0: // Play
      AudioData->ucPage16Command |= (UCHAR)AntPlusControls::AudioVideoCommands::PLAY;
      break;
   case 1: // Pause
      AudioData->ucPage16Command |= (UCHAR)AntPlusControls::AudioVideoCommands::PAUSE;
      break;
   case 2: // Stop
      AudioData->ucPage16Command |= (UCHAR)AntPlusControls::AudioVideoCommands::STOP;
      break;
   case 3: // Volume Up
      AudioData->ucPage16Command |= (UCHAR)AntPlusControls::AudioVideoCommands::VOLUME_UP;
      break;
   case 4: // Volume Down
      AudioData->ucPage16Command |= (UCHAR)AntPlusControls::AudioVideoCommands::VOLUME_DOWN;
      break;
   case 5: // Mute/Unmute
      AudioData->ucPage16Command |= (UCHAR)AntPlusControls::AudioVideoCommands::MUTE;
      break;
   case 6: // Track Ahead
      AudioData->ucPage16Command |= (UCHAR)AntPlusControls::AudioVideoCommands::TRACK_AHEAD;
      break;
   case 7: // Track Back
      AudioData->ucPage16Command |= (UCHAR)AntPlusControls::AudioVideoCommands::TRACK_BACK;
      break;
   case 8: // Repeat Current
      AudioData->ucPage16Command |= (UCHAR)AntPlusControls::AudioVideoCommands::REPEAT_CURRENT_TRACK;
      break;
   case 9: // Repeat All
      AudioData->ucPage16Command |= (UCHAR)AntPlusControls::AudioVideoCommands::REPEAT_ALL;
      break;
   case 10: // Repeat Off
      AudioData->ucPage16Command |= (UCHAR)AntPlusControls::AudioVideoCommands::REPEAT_OFF;
      break;
   case 11: // Shuffle Songs
      AudioData->ucPage16Command |= (UCHAR)AntPlusControls::AudioVideoCommands::SHUFFLE_SONGS;
      break;
   case 12: // Shuffle Albums
      AudioData->ucPage16Command |= (UCHAR)AntPlusControls::AudioVideoCommands::SHUFFLE_ALBUMS;
      break;
   case 13: // Shuffle Off
      AudioData->ucPage16Command |= (UCHAR)AntPlusControls::AudioVideoCommands::SHUFFLE_OFF;
      break;
   case 14: // FFWD
      AudioData->ucPage16Command |= (UCHAR)AntPlusControls::AudioVideoCommands::FFW;
      break;
   case 15: // RWD
      AudioData->ucPage16Command |= (UCHAR)AntPlusControls::AudioVideoCommands::FRWD;
      break;
   case 16: // Custom Repeat
      AudioData->ucPage16Command |= (UCHAR)AntPlusControls::AudioVideoCommands::REPEAT_CUSTOM;
      break;
   case 17: // Custom Shuffle
      AudioData->ucPage16Command |= (UCHAR)AntPlusControls::AudioVideoCommands::SHUFFLE_CUSTOM;
      break;
   default:
      return;
   }

   SendCommand(AudioData->PAGE_AUDIO_VIDEO_COMMAND);
}

void AudioRemote::cbInvalidVolInc_CheckedChanged(System::Object ^sender, System::EventArgs ^e)
{
   if(cbInvalidVolInc->Checked)
      nmAudioVolInc->Enabled = false;
   else
      nmAudioVolInc->Enabled = true;
}
void AudioRemote::button_UpdateSerial_Click(System::Object ^sender, System::EventArgs ^e)
{
   label_SerialNumberError->Visible = false;

   try
   {
      if(checkBox_UnknownSerialNumber->Checked)
         AudioData->usPage16SerialNumber = AntPlusControls::UNKNOWN_SHORT;
      else
         AudioData->usPage16SerialNumber = System::Convert::ToInt16(textBox_Serial->Text);
   }
   catch(...)
   {
      label_SerialNumberError->Visible = true;
   }
}
void AudioRemote::checkBox_UnknownSerialNumber_CheckedChanged(System::Object ^sender, System::EventArgs ^e)
{
   if(checkBox_UnknownSerialNumber->Checked)
      textBox_Serial->Enabled = false;
   else
      textBox_Serial->Enabled = true;
}