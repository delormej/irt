/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/
 


#include "StdAfx.h"
#include "CustomDisplay.h"


/**************************************************************************
 * CustomDisplay::ANT_eventNotification
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
void CustomDisplay::ANT_eventNotification(UCHAR ucEventCode_, UCHAR* pucEventBuffer_)
{
	switch(ucEventCode_)
	{
		case EVENT_RX_ACKNOWLEDGED:
		case EVENT_RX_BURST_PACKET: // intentional fall thru
		case EVENT_RX_BROADCAST:
			HandleReceive((UCHAR*) pucEventBuffer_);
			break;
		default:
			break;
	}
}


/**************************************************************************
 * CustomDisplay::InitializeSim
 * 
 * Initializes simulator variables
 * 
 * returns: N/A
 *
 **************************************************************************/
void CustomDisplay::InitializeSim()
{
	ulEventCounter = 0;
	bSendHex = FALSE;

}


/**************************************************************************
 * CustomDisplay::HandleReceive
 * 
 * Since custom data does not follow a particular format, it is displayed
 * as received
 *
 * pucRxBuffer_: pointer to the buffer containing the received data
 * 
 * returns: N/A
 *
 **************************************************************************/
void CustomDisplay::HandleReceive(UCHAR* pucRxBuffer_){
	UCHAR i;
	System::String^ tempc;
	System::String^ temph;
	System::String^ tempi;

	for(i=0; i<8; ++i){
		tempc = System::String::Concat(tempc,System::Convert::ToChar(pucRxBuffer_[i]));   // char
		temph = System::String::Concat(temph,"[",System::Convert::ToString(pucRxBuffer_[i],16),"]"); // hex
		tempi = System::String::Concat(tempi,"[",pucRxBuffer_[i].ToString(),"]"); // decimal
	}
	this->label_Calc_AsChar->Text = tempc;
	this->label_Calc_AsHex->Text = temph;
	this->label_Calc_AsInt->Text = tempi;
}

/**************************************************************************
 * CustomDisplay::radioButton_TranslateSelect_CheckedChanged
 * 
 * Selects format of input box for message to send: Hex/Char
 *
 * pucRxBuffer_: pointer to the buffer containing the received data
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void CustomDisplay::radioButton_TranslateSelect_CheckedChanged(System::Object^  sender, System::EventArgs^  e){
	if(this->radioButton_asChar->Checked){
		this->textBox_customTxData->Text = "Max8Char";
		this->textBox_customTxData->MaxLength = 8;
		bSendHex = FALSE;
	}
	else if(this->radioButton_asHex->Checked){
		this->textBox_customTxData->Text = "00,00,00,00,00,00,00,00";
		this->textBox_customTxData->MaxLength = 23;
		bSendHex = TRUE;
	}
}


/**************************************************************************
 * CustomDisplay::button_SendAck_Click
 * 
 * Validates and sends user data as an acknowledged messaage
 *
 * pucRxBuffer_: pointer to the buffer containing the received data
 * 
 * returns: N/A
 *
 **************************************************************************/
System::Void CustomDisplay::button_SendAck_Click(System::Object^  sender, System::EventArgs^  e){
	UCHAR i;
	UCHAR aucTxBuffer[8] = {0,0,0,0,0,0,0,0};

	// Validate data is not an empty string
	if(System::String::IsNullOrEmpty(this->textBox_customTxData->Text))
	{
		this->textBox_customTxData->Text = "Invalid Input, try again";
		return;
	}
	
	if(!bSendHex)
	{
		// Parse char string
		UCHAR ucTextLength = textBox_customTxData->Text->Length;
		for(i=0; i < ucTextLength; ++i)
			aucTxBuffer[i] = (UCHAR)textBox_customTxData->Text[i];
		while(i<8)
			aucTxBuffer[i++] = 0x00;
	}
	else
	{
		// Parse hex couplets
		array<System::String^>^ hexes = this->textBox_customTxData->Text->Split(',');
		UCHAR ucHexesLength = hexes->Length;
		try{
			if(ucHexesLength >8)
				throw "Too many commas";
			for(i=0; i<ucHexesLength; ++i)
			{
				if(hexes[i]->Length >2)
					throw "Incorrect couplet format";
				aucTxBuffer[i] = Byte::Parse(hexes[i],System::Globalization::NumberStyles::HexNumber);
			}
			while(i<8)
				aucTxBuffer[i++] = 0x00;
		}
		catch(...){
			this->textBox_customTxData->Text = "Invalid Input, try again";
			for(i=0; i<8; ++i)
				aucTxBuffer[i] = 0x00;
			return;
		}
	}
	requestAckMsg(aucTxBuffer);
}