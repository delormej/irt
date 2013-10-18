/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/



#include "StdAfx.h"
#include "SDMDisplay.h"


/**************************************************************************
 * SDMDisplay::ANT_eventNotification
 * 
 * Process ANT channel event
 *
 * ucEventCode_: code of ANT channel event
 * pucEventBuffer_: pointer to buffer containing data received from ANT,
 *		or a pointer to the transmit buffer in the case of an EVENT_TX
 * 
 * returns: N/A
 *
 **************************************************************************/
void SDMDisplay::ANT_eventNotification(UCHAR ucEventCode_, UCHAR* pucEventBuffer_)
{
	switch(ucEventCode_)
	{
		case EVENT_RX_ACKNOWLEDGED:
		case EVENT_RX_BURST_PACKET: // intentional fall thru
		case EVENT_RX_BROADCAST:
			HandleReceive((UCHAR*) pucEventBuffer_);
			break;		
		case EVENT_TRANSFER_TX_COMPLETED:
			ucAckRetryCount = 0;			// Reset retransmission counter
			ucMsgExpectingAck = 0;			// Clear pending msg code
			UpdateDisplayAckStatus(ACK_SUCCESS);	// Tx successful
			break;
		case EVENT_TRANSFER_TX_FAILED:	
		case EVENT_ACK_TIMEOUT:		// Intentional fall thru
			if(ucMsgExpectingAck)
			{
				if(HandleRetransmit())
					UpdateDisplayAckStatus(ACK_RETRY);	// Data was retransmitted
				else
					UpdateDisplayAckStatus(ACK_FAIL);	// Maximum number of retries exceeded, Tx failed
			}
			break;
		default:
			break;
	}
}


/**************************************************************************
 * SDMDisplay::InitializeSim
 * 
 * Initializes simulator variables
 * 
 * returns: N/A
 *
 **************************************************************************/
void SDMDisplay::InitializeSim()
{
	ucLatency32 = 0;
	usSpeed256 = 0;
	usCadence16 = 0;
	sStatusFlags.ucBatteryStatus = 0;
	sStatusFlags.ucHealth = 0;
	sStatusFlags.ucLocation = 0;
	sStatusFlags.ucUseState = 0;
	ucStrideCount = 0;	
	usTime200 = 0;		
	usDistance16 = 0;
	ucCalCount = 0;
	ulAcumStrideCount = 0;	
	ulAcumDistance16 = 0;
	ulAcumTime200 = 0;
	ulAcumCalCount = 0;

	ucMsgExpectingAck = 0;
	ucCapabFlags = 0;

	ucRqTxTimes = System::Convert::ToByte(this->numericUpDown_RqTxTimes->Value);
	bRqAckReply = FALSE;
	bRqTxUntilAck = FALSE;

	bInitialized = FALSE;
	bCalInitialized = FALSE;
}


/**************************************************************************
 * SDMDisplay::HandleReceive
 * 
 * Decodes received data
 *
 * pucRxBuffer_: pointer to the buffer containing the received data
 * 
 * returns: N/A
 *
 **************************************************************************/
void SDMDisplay::HandleReceive(UCHAR* pucRxBuffer_)
{
	UCHAR ucPageNum = pucRxBuffer_[0];
	ULONG ulTempHolder = 0;
	
	switch(ucPageNum)
	{
		case SDM_PAGE1: //Main page
			usTime200 = pucRxBuffer_[1];				// Current time, fractional portion, in 1/200 seconds
			usTime200 += pucRxBuffer_[2] * 200;			// Convert integer portion of time to 1/200 seconds
			usDistance16 = pucRxBuffer_[3] << 4;		// Distance, integer portion, in 1/16 seconds
			usDistance16 |= (pucRxBuffer_[4] & 0xF0) >> 4;	// Distance, fractional part, in 1/16 seconds
			usSpeed256 = (pucRxBuffer_[4] & 0x0F) << 8;	// Speed, in 1/256 seconds
			usSpeed256 |= pucRxBuffer_[5];				// Speed, fractional part, in 1/256 seconds
			ucStrideCount = pucRxBuffer_[6];			// Stride count
			ucLatency32 = pucRxBuffer_[7];				// Latency (1/32 seconds)
			if(!bInitialized)
			{	// Initialize previous values for calculation of cumulative values
				ucPreviousStrideCount = ucStrideCount;
				usPreviousDistance16 = usDistance16;
				usPreviousTime200 = usTime200;			
				bInitialized = TRUE;
			}
			break;
		case SDM_PAGE2:		
		case SDM_PAGE3:
		case SDM_PAGE4:
		case SDM_PAGE5:
		case SDM_PAGE6:
		case SDM_PAGE7:
		case SDM_PAGE8:
		case SDM_PAGE9:
		case SDM_PAGE10:
		case SDM_PAGE11:
		case SDM_PAGE12:
		case SDM_PAGE13:
		case SDM_PAGE14:
		case SDM_PAGE15:	// Intentional fall thru (pages 2 - 15 should have bytes 3,4,5, and 7 of the template on Page 2).
			usCadence16 = pucRxBuffer_[3] << 4;				// Cadence (1/16 seconds)
			usCadence16 |= (pucRxBuffer_[4] & 0xF0) >>4;
			usSpeed256 = (pucRxBuffer_[4] & 0x0F) << 8;		// Speed (1/256 seconds)
			usSpeed256 |= pucRxBuffer_[5];
			sStatusFlags.ucLocation = (pucRxBuffer_[7] & SDM_LOCATION_MASK) >> SDM_LOCATION_SHIFT;	// Decode status flags
			sStatusFlags.ucBatteryStatus = (pucRxBuffer_[7] & SDM_BAT_MASK) >> SDM_BAT_SHIFT;
			sStatusFlags.ucHealth = (pucRxBuffer_[7] & SDM_HEALTH_MASK) >> SDM_HEALTH_SHIFT;
			sStatusFlags.ucUseState = (pucRxBuffer_[7] & SDM_STATE_MASK) >> SDM_STATE_SHIFT;
			// instead of handling Page 3 Calories separately, handle here for now. If more pages are added
			// should pull them out separately for readability.
			if (ucPageNum == SDM_PAGE3)			
				ucCalCount = pucRxBuffer_[6];
			if(!bCalInitialized)
			{	// Initialize previous values for calculation of cumulative values
				ucPreviousCalCount = ucCalCount;
				bCalInitialized = TRUE;
			}
			break;
		case SDM_PAGE16:	// request page
			ulStridesAccum = pucRxBuffer_[1];
			ulTempHolder = pucRxBuffer_[2] & 0xFF;
			ulStridesAccum = ulStridesAccum | (ulTempHolder << 8);
			ulTempHolder = pucRxBuffer_[3] & 0xFF;
			ulStridesAccum = ulStridesAccum | (ulTempHolder << 16);
			ulDistanceAccum = pucRxBuffer_[4];
			ulTempHolder = pucRxBuffer_[5] & 0xFF;
			ulDistanceAccum = ulDistanceAccum | (ulTempHolder << 8);
			ulTempHolder = pucRxBuffer_[6] & 0xFF;
			ulDistanceAccum = ulDistanceAccum | (ulTempHolder << 16);
			ulTempHolder = pucRxBuffer_[7] & 0xFF;
			ulDistanceAccum = ulDistanceAccum | (ulTempHolder << 24);
			break;
		case SDM_PAGE22:	// request page							
			ucCapabFlags = pucRxBuffer_[1];
			break;
		case CommonData::PAGE80:
		case CommonData::PAGE81:	// Intentional fall thru
			try
			{
				commonPages->Decode(pucRxBuffer_);
			}
			catch(CommonData::Error^ errorCommon)
			{
			}
			break;
		default:
			break;
	}

	// Update calculated data if dealing with a new event
	if(ucStrideCount != ucPreviousStrideCount)
	{
		// Update cumulative stride count
		if(ucStrideCount > ucPreviousStrideCount)
			ulAcumStrideCount += (ucStrideCount - ucPreviousStrideCount);
		else
			ulAcumStrideCount += (UCHAR) (0xFF - ucPreviousStrideCount + ucStrideCount + 1);

		// Update cumulative time
		if(usTime200 > usPreviousTime200)
			ulAcumTime200 += (usTime200 - usPreviousTime200);
		else
			ulAcumTime200 += (USHORT) (0xFFFF - usPreviousTime200 + usTime200 + 1);

		// Update cumulative distance
		if(usDistance16> usPreviousDistance16)
			ulAcumDistance16 += (usDistance16 - usPreviousDistance16);
		else
			ulAcumDistance16 += (0xFFF - usPreviousDistance16 + usDistance16 + 1) & 0x0FFF;

		ucPreviousStrideCount = ucStrideCount;
		usPreviousTime200 = usTime200;
		usPreviousDistance16 = usDistance16;
	}
	
	// Update calculated data if dealing with a new event for calories
	if (ucCalCount != ucPreviousCalCount)
	{
		if(ucCalCount > ucPreviousCalCount)
			ulAcumCalCount += (ucCalCount - ucPreviousCalCount);
		else
			ulAcumCalCount += (UCHAR) (0xFF - ucPreviousCalCount + ucCalCount + 1);
		ucPreviousCalCount = ucCalCount;
	}

	UpdateDisplay(ucPageNum);
}


/**************************************************************************
 * SDMDisplay::UpdateDisplay
 * 
 * Shows received decoded data on GUI
 *
 * ucPageNum_: received page
 *
 * returns:  N/A
 *
 **************************************************************************/
void SDMDisplay::UpdateDisplay(UCHAR ucPageNum_)
{
	switch(ucPageNum_)
	{
		case SDM_PAGE1:
			// Update stride count (required field)
			this->label_TransStrideCountDisplay->Text = ucStrideCount.ToString();	// Stride count
			this->label_P1StrideCountDisplay->Text = ulAcumStrideCount.ToString();  // Cumulative stride count
			if(usTime200)
			{
				this->label_Trn_EventTimeDisplay->Text = ((double)usTime200/200).ToString(); // Time in seconds
				this->label_Calc_TimeDisplay->Text = ((double) ulAcumTime200/200).ToString(); // Cumulative time in seconds
			}
			else
			{
				this->label_Trn_EventTimeDisplay->Text = "Off";
			}
			if(usDistance16)
			{
				this->label_TransDistDisplay->Text = ((double) usDistance16/16).ToString();	// Distance in m
				this->label_P1TotDistDisplay->Text = System::Math::Round((double) ulAcumDistance16/16 , 4).ToString();	// Cumulative distance in m
			}
			else
			{
				this->label_TransDistDisplay->Text = "Off";
			}
			if(usSpeed256)
			{
				this->label_TransInstSpeedDisplay->Text = ((double)usSpeed256/256).ToString();	// Speed in m/s
				this->label_Calc_SpeedDisplay->Text = System::Math::Round((double)usSpeed256/256 , 3).ToString();			// Speed in m/s
				this->label_Calc_PaceDisplay->Text = System::Math::Round((double) (256*1000/60)/usSpeed256 , 2).ToString();	// Pace in min/km
			}
			else
			{
				this->label_TransInstSpeedDisplay->Text = "Off";
			}
			if(ulAcumTime200)		// Calculate average speed
				this->label_Calc_AvgSpdDisplay->Text = System::Math::Round((double) 12.5 * ulAcumDistance16/ulAcumTime200 , 3).ToString();	// Dist(1/16m) / Time(1/200s) -> m/s
			else
				this->label_Calc_AvgSpdDisplay->Text = "Off";					
			if(ucLatency32)
				this->label_TransLatencyDisplay->Text = ((double) 1000 * ucLatency32/32).ToString();	// Latency in ms
			else	// Unused
				this->label_TransLatencyDisplay->Text = "Off";
			break;
		case SDM_PAGE2:
		case SDM_PAGE3:
		case SDM_PAGE4:
		case SDM_PAGE5:
		case SDM_PAGE6:
		case SDM_PAGE7:
		case SDM_PAGE8:
		case SDM_PAGE9:
		case SDM_PAGE10:
		case SDM_PAGE11:
		case SDM_PAGE12:
		case SDM_PAGE13:
		case SDM_PAGE14:
		case SDM_PAGE15:	// Intentional fall thru (pages 2 - 15 should have bytes 3,4,5, and 7 of the template on Page 2).
			if(usCadence16) // 0 is sent if disabled on sensor
			{
				this->label_Calc_CadenceDisplay->Text = ((double)usCadence16/16).ToString();	// Cadence in spm
				this->label_TransCadenceDisplay->Text = ((double)usCadence16/16).ToString();
			}
			else
				this->label_TransCadenceDisplay->Text = "Off";
			if(usSpeed256)
			{
				this->label_TransInstSpeedDisplay->Text = ((double)usSpeed256/256).ToString();	// Speed in m/s
				this->label_Calc_SpeedDisplay->Text =  System::Math::Round((double)usSpeed256/256 , 3).ToString();	// Speed in m/s
				this->label_Calc_PaceDisplay->Text = System::Math::Round((double) (256*1000/60)/usSpeed256 , 2).ToString();	// Pace in min/km
			}
			else
			{
				this->label_TransInstSpeedDisplay->Text = "Off";
			}
			try{
				this->listBox_P2Location->SelectedIndex = sStatusFlags.ucLocation;
				this->listBox_P2Battery->SelectedIndex = sStatusFlags.ucBatteryStatus;
				this->listBox_P2Health->SelectedIndex = sStatusFlags.ucHealth;
				this->listBox_P2Use->SelectedIndex = sStatusFlags.ucUseState;
			}
			catch(...){
				//bad sent flags received
			}
			// instead of handling Page 3 Calories separately, handle here for now. If more pages are added
			// should pull them out separately for readability.
			if (ucPageNum_ == SDM_PAGE3)	// calories must be enabled if page3 is enabled
			{
				this->label_TransCalDisplay->Text = ucCalCount.ToString();	// Current Calorie count
				this->label_P2Calories->Text = ulAcumCalCount.ToString();	// Total count
			}
			else
				this->label_TransCalDisplay->Text = "Off";
			break;
		case SDM_PAGE16:
			this->label_RqStridesDisplay->Text = ulStridesAccum.ToString();
			this->label_RqDistanceDisplay->Text = ulDistanceAccum.ToString();
			break;
		case SDM_PAGE22:
			if ((ucCapabFlags & SDM_TIME_MASK) > 0)
				this->label_RqTime->ForeColor = System::Drawing::Color::Green;
			else
				this->label_RqTime->ForeColor = System::Drawing::Color::Red;			
			if ((ucCapabFlags & SDM_DISTANCE_MASK) > 0)
				this->label_RqDistance->ForeColor = System::Drawing::Color::Green;
			else
				this->label_RqDistance->ForeColor = System::Drawing::Color::Red;
			if ((ucCapabFlags & SDM_SPEED_MASK) > 0)
				this->label_RqSpeed->ForeColor = System::Drawing::Color::Green;
			else
				this->label_RqSpeed->ForeColor = System::Drawing::Color::Red;
			if ((ucCapabFlags & SDM_LATENCY_MASK) > 0)
				this->label_RqLatency->ForeColor = System::Drawing::Color::Green;
			else
				this->label_RqLatency->ForeColor = System::Drawing::Color::Red;
			if ((ucCapabFlags & SDM_CADENCE_MASK) > 0)
				this->label_RqCadence->ForeColor = System::Drawing::Color::Green;
			else
				this->label_RqCadence->ForeColor = System::Drawing::Color::Red;
			if ((ucCapabFlags & SDM_CALORIES_MASK) > 0)
				this->label_RqCalories->ForeColor = System::Drawing::Color::Green;
			else
				this->label_RqCalories->ForeColor = System::Drawing::Color::Red;
			break;
		case CommonData::PAGE80:
			this->label_Glb_HardwareVerDisplay->Text = commonPages->ucHwVersion.ToString();
			this->label_Glb_ManfIDDisplay->Text = commonPages->usMfgID.ToString();
			this->label_Glb_ModelNumDisplay->Text = commonPages->usModelNum.ToString();
			break;
		case CommonData::PAGE81:
			this->label_Glb_SoftwareVerDisplay->Text = commonPages->ucSwVersion.ToString();
			if(commonPages->ulSerialNum == 0xFFFFFFFF)
				this->label_Glb_SerialNumDisplay->Text = "N/A";
			else
				this->label_Glb_SerialNumDisplay->Text = commonPages->ulSerialNum.ToString();
			break;
	}
}


/**************************************************************************
 * SDMDisplay::SendRequestMsg
 * 
 * Sends an acknoledged Page Requset message according to the message code 
 *
 * ucMsgCode_: message ID of the calibration message to send
 * Supported messages:  
 * - Page 16 Request (Distance and Strides)
 * - Page 22 Request (Capabilities)
 * 
 * returns: N/A
 *
 **************************************************************************/
void SDMDisplay::SendRequestMsg(UCHAR ucMsgCode_) 
{
	UCHAR aucAckBuffer[8] = {0,0,0,0,0,0,0,0};

	switch(ucMsgCode_)
	{
		case SDM_PAGE16:	// Page 16 Request (Distance and Strides)
			EncodeRequestMsg(ucMsgCode_, aucAckBuffer);
			break;
		case SDM_PAGE22:	// Page 22 Request (Capabilities)
			EncodeRequestMsg(ucMsgCode_, aucAckBuffer);
			break;		
		default:
			return;		// Do not transmit if unsupported message code
	}

	// Send acknowledged message
	if(!ucMsgExpectingAck)	// If this message is not a retransmission
	{
		ucAckRetryCount = 0;			// Reset retransmission counter
		ucMsgExpectingAck = ucMsgCode_;	// Set code to message for which an ACK wil be expected
	}
	requestAckMsg(aucAckBuffer);	// Send message
}


/**************************************************************************
 * SDMDisplay::HandleRetransmit
 * 
 * Retransmits request message, up to the maximum retransmission number 
 * 
 * returns: TRUE if message was retransmitted
 *          FALSE if maximum number of retransmissions was reached
 *
 **************************************************************************/
BOOL SDMDisplay::HandleRetransmit()
{
	BOOL bSuccess = TRUE;

	if(ucMsgExpectingAck)	// Message still expecting an ack
	{
		if(ucAckRetryCount++ < MAX_RETRIES)
			SendRequestMsg(ucMsgExpectingAck);
		else
			bSuccess = FALSE;
	}
	
	return bSuccess;
}


/**************************************************************************
 * SDMDisplay::UpdateDisplayAckStatus
 * 
 * Updates display to show if acknowledged request messages were
 * transmitted successfully
 *
 * returns:  N/A
 *
 **************************************************************************/
void SDMDisplay::UpdateDisplayAckStatus(UCHAR ucStatus_)
{
	switch(ucStatus_)
	{
		case ACK_SUCCESS:
			this->label_AckMsgStatus->ForeColor = System::Drawing::Color::Green;
			this->label_AckMsgStatus->Text = "SENT";
			break;
		case ACK_RETRY:
			this->label_AckMsgStatus->ForeColor = System::Drawing::Color::Blue;
			this->label_AckMsgStatus->Text = "RTRY";
			break;
		case ACK_FAIL:
			this->label_AckMsgStatus->ForeColor = System::Drawing::Color::Red;
			this->label_AckMsgStatus->Text = "FAIL";
			break;
		default:
			break;
	}
}


/**************************************************************************
 * SDMDisplay::EncodeRequestMsg
 * 
 * Encodes the Request Data Page
 *
 * ucPageID_: ID of the page to request;  
 * pucTxBuffer_: pointer to the buffer that will store the encoded data
 * 
 * returns: N/A
 *
 **************************************************************************/
void SDMDisplay::EncodeRequestMsg(UCHAR ucPageID_, UCHAR* pucTxBuffer_)
{
	if (bRqAckReply)
		ucRqTxTimes |= 0x80;	// set the MSB bit to 1
	else
		ucRqTxTimes &= ~0x80;	// clear the MSB
	
	if (bRqTxUntilAck)
		ucRqTxTimes = 0x80;		// 0x80 is a special value

	pucTxBuffer_[0] = SDM_PAGE70;			// Command ID
	pucTxBuffer_[1] = SDM_RESERVED;
	pucTxBuffer_[2] = SDM_RESERVED;
	pucTxBuffer_[3] = SDM_RESERVED;			// invalid
	pucTxBuffer_[4] = SDM_RESERVED;			// invalid
	pucTxBuffer_[5] = ucRqTxTimes;			// number of times for sensor to send, based on UI	
	pucTxBuffer_[6] = ucPageID_;			// Page number to request
	pucTxBuffer_[7] = SDM_DATA_REQUEST;		// Specify that we are requesting data (not ANT-FS)	
}


/**************************************************************************
 * SDMDisplay::button_RequestPage16_Click
 * 
 * Initates the sending of a message to the sensor to request data page 16
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void SDMDisplay::button_RequestPage16_Click(System::Object^  sender, System::EventArgs^  e)
{
	SendRequestMsg(SDM_PAGE16);	
}


/**************************************************************************
 * SDMDisplay::button_RequestPage22_Click
 * 
 * Initates the sending of a message to the sensor to request data page  16 
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void SDMDisplay::button_RequestPage22_Click(System::Object^  sender, System::EventArgs^  e)
{
	SendRequestMsg(SDM_PAGE22);	
}


/**************************************************************************
 * SDMDisplay::numericUpDown_numericUpDown_RqTxTimes_ValueChanged
 * 
 * Updates the number of times for the sensor to resend the request 
 * response when modified in UI
 *
 * returns: N/A
 *
 **************************************************************************/
System::Void SDMDisplay::numericUpDown_RqTxTimes_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	ucRqTxTimes = System::Convert::ToByte(this->numericUpDown_RqTxTimes->Value);
}


/**************************************************************************
 * SDMDisplay::checkBox_RqAckReply_CheckedChanged
 * 
 * Select whether the sensor should request an ACK on its replies
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void SDMDisplay::checkBox_RqAckReply_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	if(this->checkBox_RqAckReply->Checked)  // If checked set flag
	{
		bRqAckReply = TRUE;
	}
	else   // If not checked clear flag
	{
		bRqAckReply = FALSE;		
	}
}


/**************************************************************************
 * SDMDisplay::checkBox_RqTxUntilAck_CheckedChanged
 * 
 * Select whether the sensor should transmit repeatedly until it gets an ACK
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void SDMDisplay::checkBox_RqTxUntilAck_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	if(this->checkBox_RqTxUntilAck->Checked)  // If checked set flag
	{
		bRqTxUntilAck = TRUE;
		// disable other options in this case
		this->checkBox_RqAckReply->Enabled = FALSE;
		this->numericUpDown_RqTxTimes->Enabled = FALSE;
	}
	else   // If not checked clear flag
	{
		bRqTxUntilAck = FALSE;
		// enable other options 
		this->checkBox_RqAckReply->Enabled = TRUE;
		this->numericUpDown_RqTxTimes->Enabled = TRUE;
		ucRqTxTimes = System::Convert::ToByte(this->numericUpDown_RqTxTimes->Value);  // reset this value 
	}
}