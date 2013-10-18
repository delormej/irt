/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/


#include "StdAfx.h"
#include "TemperatureDisplay.h"
#include "antmessage.h"


static BOOL bCheckPeriod = FALSE;

/**************************************************************************
 * TemperatureDisplay::ANT_eventNotification
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
void TemperatureDisplay::ANT_eventNotification(UCHAR ucEventCode_, UCHAR* pucEventBuffer_)
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
      case MESG_CLOSE_CHANNEL_ID:
         this->groupBox_RxPeriod->Enabled = false;
         break;
      case MESG_OPEN_CHANNEL_ID:
         bCheckPeriod = TRUE;
			break;
		default:
			break;
	}
}
/**************************************************************************
 * TemperatureDisplay::InitializeSim
 * 
 * Initializes simulator variables
 * 
 * returns: N/A
 *
 **************************************************************************/
void TemperatureDisplay::InitializeSim()
{
	sTemp = 0;					
	ucEventCount = 0;				
	ulElapsedTime2 = 0;
	ulAcumEventCount = 0;
	usMfgID = 0;
	ucHwVersion = 0;
	ucSwVersion = 0;
	usModelNum = 0;	
	ulSerialNum = 0;

   ucMsgExpectingAck = 0;

   ucRqTxTimes = System::Convert::ToByte(this->numericUpDown_Req_Copies->Value);
}
/**************************************************************************
 * TemperatureDisplay::HandleReceive
 * 
 * Decodes received data
 *
 * pucRxBuffer_: pointer to the buffer containing the received data
 * 
 * returns: N/A
 *
 **************************************************************************/
void TemperatureDisplay::HandleReceive(UCHAR* pucRxBuffer_)
{
	UCHAR ucPageNum = 0;

   if (bCheckPeriod)
	{		    
      bCheckPeriod = FALSE;
      this->groupBox_RxPeriod->Enabled = true;
      if(radioButton_RxP5Hz->Checked == true )
      {
         TemperatureData->usMessagePeriod = TemperatureData->MESG_P5HZ_PERIOD;
         requestUpdateMesgPeriod(TemperatureData->MESG_P5HZ_PERIOD);
      }
      else if(radioButton_Rx4Hz->Checked == true )
      {
         
         TemperatureData->usMessagePeriod = TemperatureData->MESG_4HZ_PERIOD;
         requestUpdateMesgPeriod(TemperatureData->MESG_4HZ_PERIOD);
      }
   }

	// Find out what page has come in
   ucPageNum = pucRxBuffer_[0];

   // Decode appropraitely
   switch (ucPageNum)
   {
      case TemperatureData->PAGE_0:
      case TemperatureData->PAGE_1: // intentional fall through
         TemperatureData->Decode(pucRxBuffer_);
         break;
      case commonData->PAGE80:
      case commonData->PAGE81:   
      case commonData->PAGE82:// intentional fall through
         commonData->Decode(pucRxBuffer_);
         break;
      default:
         break;
   }
   
   // Move received data to TemperatureDisplay variables
   switch (ucPageNum)
   {
      case TemperatureData->PAGE_0:
         ucSupportedPages = TemperatureData->ucSupportedPages;
         ucTxInfo = TemperatureData->ucTxInfo;
         break;
      case TemperatureData->PAGE_1:
         // 24 hour low, 24 hour high, current temp
         ucEventCount = TemperatureData->ucEventCount;
         s24HrLow = TemperatureData->s24HrLow;
         s24HrHigh = TemperatureData->s24HrHigh;
         sTemp = TemperatureData->sCurrentTemp;
         break;
      case commonData->PAGE80:
         // HW Revision, Manufacturer ID, Model Number
         ucHwVersion = commonData->ucHwVersion;
         usMfgID = commonData->usMfgID;
         usModelNum = commonData->usModelNum;
         break;
      case commonData->PAGE81:
         // SW Revision, Serial Number
         ucSwVersion = commonData->ucSwVersion;
         ulSerialNum = commonData->ulSerialNum;
         break;
      default:
         break;
   }
   // Update Cumulative Event Count
   ++ulAcumEventCount;

	// Display data
	UpdateDisplay(ucPageNum);
}


/**************************************************************************
 * TemperatureDisplay::UpdateDisplay
 * 
 * Shows received decoded data on GUI
 *
 * ucPageNum_: received page
 *
 * returns:  N/A
 *
 **************************************************************************/
void TemperatureDisplay::UpdateDisplay(UCHAR ucPageNum_)
{	
	// Display basic data
	this->label_Trn_EventCountDisplay->Text = ucEventCount.ToString();

   if(sTemp != 0x8000) 
      this->label_Trn_RawTempDisplay->Text = (System::Convert::ToDecimal(sTemp)/100).ToString();
   else
      this->label_Trn_RawTempDisplay->Text = "Invalid";

   if(s24HrHigh != TemperatureData->HIGHLOW_INVALID) // 0x800 is the invalid 1.5 byte number, 0xF800 is sign extended
      this->label_Trn_24HrHighDisplay->Text = (System::Convert::ToDecimal(s24HrHigh)/10).ToString();
   else
      this->label_Trn_24HrHighDisplay->Text = "Invalid";

   if(s24HrLow != TemperatureData->HIGHLOW_INVALID)
      this->label_Trn_24HrLowDisplay->Text = (System::Convert::ToDecimal(s24HrLow)/10).ToString();
   else
      this->label_Trn_24HrLowDisplay->Text = "Invalid";

   // Decode TxInfo
   // Determine Transmission Rate

   if((ucTxInfo & TemperatureData->PERIOD_MASK) == TemperatureData->TXINFO_4HZ)
      this->label_Glb_MsgPeriod_Display->Text = "4 Hz";
   else if((ucTxInfo & TemperatureData->PERIOD_MASK) == TemperatureData->TXINFO_P5HZ)
      this->label_Glb_MsgPeriod_Display->Text = "0.5 Hz";
   else
      this->label_Glb_MsgPeriod_Display->Text = "";

   // Determine UTC Time Support
   if((ucTxInfo & TemperatureData->UTC_TIME_MASK)== TemperatureData->UTC_TIME_NOT_SUPPORTED)
      this->label_UTCTimeDisplay->Text = "Not Supported";
   else if ((ucTxInfo & TemperatureData->UTC_TIME_MASK)== TemperatureData->UTC_TIME_NOT_SET)
      this->label_UTCTimeDisplay->Text = "Not Set";
   else
      this->label_UTCTimeDisplay->Text = "Set";

   // Determine Local Time Support
   if((ucTxInfo & TemperatureData->LOCAL_TIME_MASK)== TemperatureData->LOCAL_TIME_NOT_SUPPORTED)
      this->label_LocalTime_Display->Text = "Not Supported";
   else if ((ucTxInfo & TemperatureData->LOCAL_TIME_MASK)== TemperatureData->LOCAL_TIME_NOT_SET)
      this->label_LocalTime_Display->Text = "Not Set";
   else
      this->label_LocalTime_Display->Text = "Set";

   // Determine supported pages
   switch (ucSupportedPages)
   {
      case TemperatureData->SUPPORTED_PAGES_0:
         this->label_Glb_SpprtdPgs_Display->Text = "0";
         break;
      case TemperatureData->SUPPORTED_PAGES_1 :
         this->label_Glb_SpprtdPgs_Display->Text = "1";
         break;
      case TemperatureData->SUPPORTED_PAGES_0_1 :
         this->label_Glb_SpprtdPgs_Display->Text = "0, 1";
         break;
      default :
         break;
   }



	// Display background data, if available
		switch(ucPageNum_)
		{
      case commonData->PAGE80:
			// HW Version, Manufacturer ID, Model Number
         this->label_Glb_HardwareVerDisplay->Text = ucHwVersion.ToString();
         this->label_Glb_MfgIDDisplay->Text = usMfgID.ToString();
         this->label_Glb_ModelNumDisplay->Text = usModelNum.ToString();
			break;
		case commonData->PAGE81:
			// SW Version, Serial Number
         this->label_Glb_SoftwareVerDisplay->Text = ucSwVersion.ToString();
         if(ulSerialNum != 0xFFFFFFFF)
            this->label_Glb_SerialNumDisplay->Text = ulSerialNum.ToString();
         else
            this->label_Glb_SerialNumDisplay->Text = "No Serial";
			break;
      case commonData->PAGE82:
          // update the values for the battery common data page
         if(commonData->IsBatteryVoltageInvalid(commonData->usBatVoltage256))		// Battery voltage
            this->labelBattVolt->Text = "Invalid";
         else
            this->labelBattVolt->Text = System::Math::Round((double)commonData->usBatVoltage256/256,4).ToString("N2");

         if (commonData->eTimeResolution == CommonData::TimeResolution::SIXTEEN)
            this->labelTimeRes->Text = "16";
         else
            this->labelTimeRes->Text = "2";

         // now that we know the time resolution we can display the run time
         this->labelOpTime->Text = (commonData->ulOpTime * (UCHAR) commonData->eTimeResolution).ToString();

         switch (commonData->eBatStatus)
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
         break;

      default:
         break;
      }

	
	// Display calculated data
	// Cumulative event count
	this->label_Calc_TotEventCountDisplay->Text = ulAcumEventCount.ToString();

  


}
/**************************************************************************
 * TemperatureDisplay::SendRequestMsg
 * 
 * Sends an acknoledged Page Requset message according to the message code 
 *
 * ucMsgCode_: message ID of the message to send
 * Supported messages:  
 * - Page 80 Request
 * - Page 81 Request
 * 
 * returns: N/A
 *
 **************************************************************************/
void TemperatureDisplay::SendRequestMsg(UCHAR ucMsgCode_) 
{
	UCHAR aucAckBuffer[8] = {0,0,0,0,0,0,0,0};

	switch(ucMsgCode_)
	{
   case commonData->PAGE80:	
   case commonData->PAGE81:
   case commonData->PAGE82: // Intentional fallthrough
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
 * TemperatureDisplay::HandleRetransmit
 * 
 * Retransmits request message, up to the maximum retransmission number 
 * 
 * returns: TRUE if message was retransmitted
 *          FALSE if maximum number of retransmissions was reached
 *
 **************************************************************************/
BOOL TemperatureDisplay::HandleRetransmit()
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
 * TemperatureDisplay::UpdateDisplayAckStatus
 * 
 * Updates display to show if acknowledged request messages were
 * transmitted successfully
 *
 * returns:  N/A
 *
 **************************************************************************/
void TemperatureDisplay::UpdateDisplayAckStatus(UCHAR ucStatus_)
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
 * TemperatureDisplay::EncodeRequestMsg
 * 
 * Encodes the Request Data Page
 *
 * ucPageID_: ID of the page to request;  
 * pucTxBuffer_: pointer to the buffer that will store the encoded data
 * 
 * returns: N/A
 *
 **************************************************************************/
void TemperatureDisplay::EncodeRequestMsg(UCHAR ucPageID_, UCHAR* pucTxBuffer_)
{
	ucRqTxTimes &= ~0x80;	// clear the MSB (Temp sensor should respond with ack messages)

   pucTxBuffer_[0] = commonData->PAGE70;		// Command ID
   pucTxBuffer_[1] = commonData->RESERVED;
	pucTxBuffer_[2] = commonData->RESERVED;
	pucTxBuffer_[3] = commonData->RESERVED;	// invalid
	pucTxBuffer_[4] = commonData->RESERVED;	// invalid
	pucTxBuffer_[5] = ucRqTxTimes;			   // number of times for sensor to send, based on UI	
	pucTxBuffer_[6] = ucPageID_;			      // Page number to request
	pucTxBuffer_[7] = TemperatureData->DATA_REQUEST;	   // Specify that we are requesting data (not ANT-FS)	
}

/**************************************************************************
 * TemperatureDisplay::button_Req_Page80_Click
 * 
 * Initates the sending of a message to the sensor to request data page 80
 *
 * returns:  N/A
 *
 **************************************************************************/
System::Void TemperatureDisplay::button_Req_Page80_Click(System::Object^ sender, System::EventArgs^ e)
{
   SendRequestMsg(commonData->PAGE80);	
}
/**************************************************************************
 * TemperatureDisplay::button_Req_Page81_Click
 * 
 * Initates the sending of a message to the sensor to request data page 81
 *
 * returns:  N/A
 *
 **************************************************************************/
System::Void TemperatureDisplay::button_Req_Page81_Click(System::Object^ sender, System::EventArgs^ e)
{
   SendRequestMsg(commonData->PAGE81);	
}
/**************************************************************************
 * TemperatureDisplay::button_Req_Page82_Click
 * 
 * Initates the sending of a message to the sensor to request data page 81
 *
 * returns:  N/A
 *
 **************************************************************************/
System::Void TemperatureDisplay::button_Req_Page82_Click(System::Object^ sender, System::EventArgs^ e)
{
   SendRequestMsg(commonData->PAGE82);	
}
/**************************************************************************
 * TemperatureDisplay::numericUpDown_Req_Copies_ValueChaned
 * 
 * Updates the number of times for the sensor to resend the request 
 * response when modified in UI
 *
 * returns: N/A
 *
 **************************************************************************/
System::Void TemperatureDisplay::numericUpDown_Req_Copies_ValueChaned(System::Object^ sender, System::EventArgs^ e)
 {
    ucRqTxTimes = System::Convert::ToByte(this->numericUpDown_Req_Copies->Value);
 }
/**************************************************************************
 * TemperatureDisplay::radioButton_RxPeriod_Changed
 * 
 * Changed the channel period of the display, to make listening to a
 * 0.5Hz sensor easier.
 *
 * returns: N/A
 *
 **************************************************************************/
System::Void TemperatureDisplay::radioButton_RxPeriod_Changed(System::Object^  sender, System::EventArgs^  e)
{
   bCheckPeriod = TRUE;
}