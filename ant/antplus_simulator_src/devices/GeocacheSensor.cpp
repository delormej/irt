/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/


#pragma once
#include "stdafx.h"
#include "GeocacheSensor.h"
#include "antmessage.h"

using namespace System::Runtime::InteropServices;

static ULONG ulMessageCount = 0;
static UCHAR ucPageCount = 0;

static BOOL bCloseEvent = FALSE;	


/**************************************************************************
* GeocacheSensor::InitializeSim
* 
* Initialize the simulator variables
*
* returns: N/A
*
**************************************************************************/
void GeocacheSensor::InitializeSim()
{
	ulTimerInterval = 250; // in ms - simulates the sensor sending the most relevant information every message period
	ulTotalTime = 0;

	// initialize Geocache
	GeocacheData->InitSensor();
	GeocacheData->ucTotalPages = GeocacheData->START_PROG_PAGES;
	GeocacheData->usMessagePeriod = GeocacheData->MESG_P5HZ_PERIOD;	// Defaults to Beacon Rate of 0.5Hz
	GeocacheData->b4HzRotation = FALSE;

	// required common pages
	commonData->ulSerialNum = System::Convert::ToUInt32(this->textBox_Glb_SerialNumChange->Text);
	commonData->ucSwVersion = System::Convert::ToByte(this->textBox_Glb_SoftwareVerChange->Text);
	commonData->ucHwVersion = System::Convert::ToByte(this->textBox_Glb_HardwareVerChange->Text);
	commonData->usMfgID = System::Convert::ToUInt16(this->textBox_Glb_ManfIDChange->Text);
	commonData->usModelNum = System::Convert::ToUInt16(this->textBox_Glb_ModelNumChange->Text);

	//battery common page
	commonData->bBattPageEnabled = TRUE;
	// update the variables
	UpdateBatStatus();
	commonData->eTimeResolution = CommonData::TimeResolution::TWO;
	commonData->ulOpTime = ulTotalTime / (UCHAR) commonData->eTimeResolution;
	commonData->usBatVoltage256 = ((USHORT) System::Convert::ToByte(this->numericUpDown_Bat_VoltInt->Value)) << 8 | (commonData->usBatVoltage256 & 0xFF);	// Integer portion in high byte
	commonData->usBatVoltage256 |= System::Convert::ToByte(this->numericUpDown_Bat_VoltFrac->Value) | (commonData->usBatVoltage256 & 0xFF00);	// Fractional portion in low byte
	commonData->eTimeResolution = CommonData::TimeResolution::TWO;
	commonData->ulOpTime = ulTotalTime;
}

/**************************************************************************
* GeocacheSensor::ANT_eventNotification
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
void GeocacheSensor::ANT_eventNotification(UCHAR ucEventCode_, UCHAR* pucEventBuffer_)
{
	switch(ucEventCode_)
	{
	case EVENT_TX:
		HandleTransmit((UCHAR*) pucEventBuffer_);
		break;

	case EVENT_RX_BROADCAST:
	case EVENT_RX_ACKNOWLEDGED:
		HandleReceive((UCHAR*) pucEventBuffer_);
		break;

	case EVENT_TRANSFER_TX_COMPLETED:
		UpdateDisplayAckStatus(GeocacheData->ACK_SUCCESS);
		break;

	case EVENT_ACK_TIMEOUT:	
	case EVENT_TRANSFER_TX_FAILED: // intentional fall through
		UpdateDisplayAckStatus(GeocacheData->ACK_FAIL);
		break;

	case MESG_CLOSE_CHANNEL_ID:
		bCloseEvent = TRUE;
		break;

	default:
		break;
	}
}

/**************************************************************************
 * GeocacheSensor::HandleReceive
 * 
 * Handle incoming (GPS) requests
 *
 * pucRxBuffer_: pointer to the receive buffer
 * 
 * returns: N/A
 *
 **************************************************************************/
void GeocacheSensor::HandleReceive(UCHAR* pucRxBuffer_)
{
	ulMessageCount = 0;	// Reset Message Counter on Any Request

	// Change to 4Hz and begin Page Rotation on *ANY* RX
	if (!GeocacheData->b4HzRotation)
	{
		GeocacheData->b4HzRotation = TRUE;
		GeocacheData->usMessagePeriod = GeocacheData->MESG_4HZ_PERIOD;
		requestUpdateMesgPeriod(GeocacheData->MESG_4HZ_PERIOD);
	}

	if (pucRxBuffer_[0] == commonData->PAGE70)	// Data Page Request
	{
		GeocacheData->ucRequestedPageNum = pucRxBuffer_[6];

		// How should we respond ?  Broadcast (added to Page Rotation) or Ack'd ?
		if (pucRxBuffer_[5] == GeocacheData->MESG_REQ_RESP_ACK)
			GeocacheData->bAckResponseRequest = TRUE;
		else
			GeocacheData->bAckResponseRequest = FALSE;

		switch (GeocacheData->ucRequestedPageNum)	// Get Requested Page # 
		{
			case GeocacheData->PAGE_1 :
				GeocacheData->GetPageData(GeocacheData->ucRequestedPageNum, pucRxBuffer_);

				// Start Point for Page Rotation
				if (GeocacheData->ucTotalPages > 2)			
					ucPageCount = GeocacheData->PAGE_2;
				else
					ucPageCount = commonData->PAGE80;

				break;

			case commonData->PAGE80 :
				commonData->Encode(commonData->PAGE80, pucRxBuffer_);
				break;

			case commonData->PAGE81 :
				commonData->Encode(commonData->PAGE81, pucRxBuffer_);
				break;

			case commonData->PAGE82 :
				if (commonData->bBattPageEnabled)
				{
					commonData->Encode(commonData->PAGE82, pucRxBuffer_);
				}
				break;

			default:
				GeocacheData->GetPageData(GeocacheData->ucRequestedPageNum, pucRxBuffer_);
				break;

		}
		// Broadcast (added to Page Rotation) or Ack'd ?
		if (GeocacheData->bAckResponseRequest)
		{
			requestAckMsg(pucRxBuffer_);
		}
		else
		{
			GeocacheData->bPageRequest = TRUE;
		}
	}
	else if (pucRxBuffer_[0] < GeocacheData->PAGE_32)	// GPS is Programming Pages
	{
		GeocacheData->Decode(pucRxBuffer_);
	}
	else if (pucRxBuffer_[0] == GeocacheData->PAGE_32)	// Authentication Request
	{
		Authentication(pucRxBuffer_);
		GeocacheData->bPageRequest = TRUE;
		GeocacheData->ucRequestedPageNum = pucRxBuffer_[0];
	}
}

/**************************************************************************
 * Geocache::Authentication
 * 
 * Generate Authentication Token from GPS Serial # and Nonce
 *
 *
 * pucRxBuffer_: pointer to the buffer containing the received data
 * 
 * returns: N/A
 *
 **************************************************************************/
using namespace System::Security::Cryptography;

void GeocacheSensor::Authentication(UCHAR* pucRxBuffer_)
{
	cli::array<Byte>^ Nonce_SerialNum = gcnew cli::array<Byte>(6);
	for (int i=0; i<6; i++) Nonce_SerialNum[i] = (byte)pucRxBuffer_[2+i];

	HashAlgorithm^ sha = gcnew SHA1CryptoServiceProvider;

	array<Byte>^ result = sha->ComputeHash( Nonce_SerialNum );

	for (int i=0; i<7; i++)
		GeocacheData->ucAuthToken[i] = (UCHAR)result[i];
}	

/**************************************************************************
 * GeocacheSensor::HandleTransmit
 * 
 * Encode data generated by simulator for transmission
 *
 * ucPageNum_: The page number to be transmitted
 * pucTxBuffer_: pointer to the transmit buffer
 * 
 * returns: N/A
 *
 **************************************************************************/
void GeocacheSensor::HandleTransmit(UCHAR* pucTxBuffer_)
{
	// Kill Page Requests and Switch to 0.5Hz Beacon Mode
	if (bCloseEvent)
	{
		bCloseEvent = FALSE;
		GeocacheData->bPageRequest = FALSE;
		GeocacheData->b4HzRotation = FALSE;
		GeocacheData->usMessagePeriod = GeocacheData->MESG_P5HZ_PERIOD;
		requestUpdateMesgPeriod(GeocacheData->MESG_P5HZ_PERIOD);
	}

	if (GeocacheData->bPageRequest)
	{
		switch (GeocacheData->ucRequestedPageNum)
		{
			case commonData->PAGE80 :
				commonData->Encode(commonData->PAGE80, pucTxBuffer_);
				break;

			case commonData->PAGE81 :
				commonData->Encode(commonData->PAGE81, pucTxBuffer_);
				break;

			case commonData->PAGE82 :
				if (commonData->bBattPageEnabled)
				{
					commonData->Encode(commonData->PAGE82, pucTxBuffer_);
				}
				break;

			default:
				GeocacheData->GetPageData(GeocacheData->ucRequestedPageNum, pucTxBuffer_);
				break;
		}
		GeocacheData->bPageRequest = FALSE;
	}
	else if (GeocacheData->b4HzRotation)
	{
		switch (ucPageCount)
		{
			case GeocacheData->PAGE_0 :
				// Encode PAGE 0
				GeocacheData->GetPageData(GeocacheData->PAGE_0, pucTxBuffer_);
				ucPageCount = GeocacheData->PAGE_1;
				break;

			case GeocacheData->PAGE_1 :
				// Encode PAGE 1
				GeocacheData->GetPageData(GeocacheData->PAGE_1, pucTxBuffer_);

				// Do we have Programmed Pages ?
				if (GeocacheData->ucTotalPages > 2)			
					ucPageCount = GeocacheData->PAGE_2;
				else
					ucPageCount = commonData->PAGE80;
				break;

			case commonData->PAGE80 :
				commonData->Encode(ucPageCount, pucTxBuffer_);
				ucPageCount = commonData->PAGE81;
				break;

			case commonData->PAGE81 :
				commonData->Encode(ucPageCount, pucTxBuffer_);
				if (commonData->bBattPageEnabled)
					ucPageCount = commonData->PAGE82;
				else
					ucPageCount = GeocacheData->PAGE_0;
				break;

			case commonData->PAGE82 :
				commonData->Encode(ucPageCount, pucTxBuffer_);
				ucPageCount = GeocacheData->PAGE_0;
				break;

			default :
				GeocacheData->GetPageData(ucPageCount, pucTxBuffer_);
				ucPageCount++;
				if (ucPageCount > (GeocacheData->ucTotalPages-1)) ucPageCount = commonData->PAGE80;
				break;
		}
	}
	else	// Not in 4Hz Rotation - just send Beacon (PAGE 0)
	{
		GeocacheData->GetPageData(GeocacheData->PAGE_0, pucTxBuffer_);
	}

	ulMessageCount++;

	// Return to 0.5Hz Beacon ONLY after 30 secs ... of no GPS requests
	if (GeocacheData->b4HzRotation && (ulMessageCount > GeocacheData->MESG_4HZ_TIMEOUT)) 
	{
		GeocacheData->b4HzRotation = FALSE;
		GeocacheData->usMessagePeriod = GeocacheData->MESG_P5HZ_PERIOD;
		requestUpdateMesgPeriod(GeocacheData->MESG_P5HZ_PERIOD);
	}

	static UCHAR ucTimeCount = 0;
	// update the batt op time every second
	if (ucTimeCount == 3)
	{
		ulTotalTime++;
		commonData->ulOpTime = ulTotalTime / (UCHAR) commonData->eTimeResolution;
		ucTimeCount = 0;
	}
	else
		ucTimeCount++;
}

/**************************************************************************
 * GeocacheSensor::buttonProgram_Click
 * 
 * Update the Programmable Pages (per UI Values)
 * 
 * returns: N/A
 *
 **************************************************************************/
void GeocacheSensor::buttonProgram_Click(System::Object^  sender, System::EventArgs^  e)
{
	GeocacheData->bLatitudeEnabled = FALSE;
	GeocacheData->bLongitudeEnabled = FALSE;
	GeocacheData->bHintEnabled = FALSE;
	GeocacheData->bLoggedVisitsEnabled = FALSE;

	// ID
	if (this->checkBoxID->Checked)
	{
		char* stringPointer = (char*) Marshal::StringToHGlobalAnsi(this->textBoxID->Text->ToUpper()).ToPointer();
		for (int i=0; i<this->textBoxID->Text->Length; i++)
			GeocacheData->cID[i] = stringPointer[i] - 0x20;
		for (int i=this->textBoxID->Text->Length; i<9; i++)
			GeocacheData->cID[i] = 0x00;
		// Always free the unmanaged string.
		Marshal::FreeHGlobal(IntPtr(stringPointer));
	}

	// PIN
	if (this->checkBoxPIN->Checked)
	{
		if (this->textBoxPIN->Text->Length > 0)
			GeocacheData->ulPIN = System::Convert::ToUInt32(this->textBoxPIN->Text);
		else
			GeocacheData->ulPIN = GeocacheData->DEFAULT_PIN;
	}

	if (this->checkBoxLatitude->Checked)
	{
		GeocacheData->bLatitudeEnabled = TRUE;
		GeocacheData->slLatitude_SC = System::Convert::ToInt32(double (this->numericUpDownLatitude->Value) * GeocacheData->SEMI_CIRCLE_CONVERSION);
	}
	if (this->checkBoxLongitude->Checked)
	{
		GeocacheData->bLongitudeEnabled = TRUE;
		GeocacheData->slLongitude_SC = System::Convert::ToInt32(double (this->numericUpDownLongitude->Value) * GeocacheData->SEMI_CIRCLE_CONVERSION);
	}
	if (this->checkBoxLoggedVisits->Checked)
	{
		time_t tmtEpoch, tmtLastVisit;
		struct tm tmEpoch, tmLastVisit;

		tmEpoch.tm_year = 1989 - 1900;
		tmEpoch.tm_mon = 12 - 1;
		tmEpoch.tm_mday = 31;
		tmEpoch.tm_hour = 00;
		tmEpoch.tm_min = 00;
		tmEpoch.tm_sec = 00;
		tmtEpoch = mktime(&tmEpoch);

		tmLastVisit.tm_year = System::Convert::ToUInt16(this->numericUpDownYear->Value) - 1900;
		tmLastVisit.tm_mon = System::Convert::ToUInt16(this->numericUpDownMonth->Value) - 1;
		tmLastVisit.tm_mday = System::Convert::ToUInt16(this->numericUpDownDay->Value);
		tmLastVisit.tm_hour = System::Convert::ToUInt16(this->numericUpDownHours->Value);
		tmLastVisit.tm_min = System::Convert::ToUInt16(this->numericUpDownMinutes->Value);
		tmLastVisit.tm_sec = System::Convert::ToUInt16(this->numericUpDownSeconds->Value);
		tmtLastVisit = mktime(&tmLastVisit);

		if (tmLastVisit.tm_isdst > 0)  // Daylight Savings ?
			GeocacheData->ulLastVisitTimestamp = (ULONG) difftime(tmtLastVisit, tmtEpoch) - 3600;
		else
			GeocacheData->ulLastVisitTimestamp = (ULONG) difftime(tmtLastVisit, tmtEpoch);

		GeocacheData->bLoggedVisitsEnabled = TRUE;
		GeocacheData->usNumVisits = System::Convert::ToUInt16(this->numericUpDownNumVisits->Value);
	}
	if (this->checkBoxHint->Checked)
	{
		char* stringPointer = (char*) Marshal::StringToHGlobalAnsi(this->textBoxHint->Text).ToPointer();
		for (int i=0; i<this->textBoxHint->Text->Length; i++)
			GeocacheData->cHint[i] = stringPointer[i];

		GeocacheData->ucNumHintPages = (this->textBoxHint->Text->Length/GeocacheData->HINT_CHARS_PER_PAGE) + 1;
		GeocacheData->bHintEnabled = TRUE;
	}

	GeocacheData->GenerateProgPages();
	UpdateDisplay();
}

/**************************************************************************
 * GeocacheSensor::Update Display
 * 
 * Update the GUI with all the changes
 * 
 * returns: N/A
 *
 **************************************************************************/
void GeocacheSensor::UpdateDisplay()
{
	// ID
	System::Text::StringBuilder^ builder = gcnew System::Text::StringBuilder();
	for(int i=0; i<GeocacheData->cID->Length; i++) builder->Append((Char)(GeocacheData->cID[i]+0x20));
	this->labelID->Text = builder->ToString();

	// PIN
	if (!this->checkBoxPIN->Checked) // Don't update if Selected
		this->textBoxPIN->Text = GeocacheData->ulPIN.ToString();
	this->labelPIN->Text = GeocacheData->ulPIN.ToString();

	// Total Pages
	this->labelTotNumPages->Text = GeocacheData->ucTotalPages.ToString();

	// Determine which pages are Enabled
	GeocacheData->EnabledProgPages();

	// Lat/Lon Pages
	if (GeocacheData->bLatitudeEnabled)
	{
		if (!this->checkBoxLatitude->Checked) // Don't update if Selected
			this->numericUpDownLatitude->Text = ((double)GeocacheData->slLatitude_SC / GeocacheData->SEMI_CIRCLE_CONVERSION).ToString("N2");
		this->labelLat->Text = ((double)GeocacheData->slLongitude_SC / GeocacheData->SEMI_CIRCLE_CONVERSION).ToString("N2");
	}
	else
	{
		this->labelLat->Text = "---";
	}
	if (GeocacheData->bLongitudeEnabled)
	{
		if (!this->checkBoxLongitude->Checked) // Don't update if Selected
			this->numericUpDownLongitude->Text = ((double)GeocacheData->slLongitude_SC / GeocacheData->SEMI_CIRCLE_CONVERSION).ToString("N2");
		this->labelLon->Text = ((double)GeocacheData->slLongitude_SC / GeocacheData->SEMI_CIRCLE_CONVERSION).ToString("N2");
	}
	else
	{
		this->labelLon->Text = "---";
	}

	// Hint Page(s)
	if (GeocacheData->bHintEnabled)
	{
		System::Text::StringBuilder^ builder = gcnew System::Text::StringBuilder();
		for(int i=0; i<GeocacheData->cHint->Length; i++) builder->Append((Char)GeocacheData->cHint[i]);
		this->labelHint->Text = builder->ToString();
		if (!this->checkBoxHint->Checked) // Don't update if Selected
			this->textBoxHint->Text = builder->ToString();
	}
	else
	{
		this->labelHint->Text = "---";
	}

	// Logged Visits Page
	if (GeocacheData->bLoggedVisitsEnabled && (GeocacheData->ulLastVisitTimestamp > 0))
	{
		if (!this->checkBoxLoggedVisits->Checked) // Don't update if Selected
			this->numericUpDownNumVisits->Value = GeocacheData->usNumVisits;
		this->labelNumVisits->Text = this->numericUpDownNumVisits->Text;

		time_t tmtEpoch;
		struct tm tmEpoch;

		tmEpoch.tm_year = 1989 - 1900;
		tmEpoch.tm_mon = 12 - 1;
		tmEpoch.tm_mday = 31;
		tmEpoch.tm_hour = 00;
		tmEpoch.tm_min = 00;
		tmEpoch.tm_sec = 00;
		tmtEpoch = mktime(&tmEpoch);

		time_t tmtLastVisit; 
		struct tm * tmLastVisit;

		tmtLastVisit = ((time_t)GeocacheData->ulLastVisitTimestamp + tmtEpoch);
#pragma warning (disable : 4068)
#pragma warning (disable : 4996)
		tmLastVisit = localtime(&tmtLastVisit);

		if (tmLastVisit->tm_isdst > 0)  // Daylight Savings ?
		{
			tmtLastVisit = ((time_t)GeocacheData->ulLastVisitTimestamp + tmtEpoch + 3600);
			tmLastVisit = localtime(&tmtLastVisit);
		}

		if (!this->checkBoxLoggedVisits->Checked) // Don't update if Selected
		{
			this->numericUpDownYear->Value = tmLastVisit->tm_year + 1900;
			this->numericUpDownMonth->Value = tmLastVisit->tm_mon + 1;
			this->numericUpDownDay->Value = tmLastVisit->tm_mday;
			this->numericUpDownHours->Value = tmLastVisit->tm_hour;
			this->numericUpDownMinutes->Value = tmLastVisit->tm_min;
			this->numericUpDownSeconds->Value = tmLastVisit->tm_sec;
		}

		System::Text::StringBuilder^ builder = gcnew System::Text::StringBuilder();
		for(int i=0; i<26; i++) builder->Append((Char)((asctime(&tmLastVisit[0]))[i]));
		this->labelTimestamp->Text = builder->ToString();
	}
	else
	{
		this->labelNumVisits->Text = "-";
		this->labelTimestamp->Text = "-";
	}

	// Authentication Token
	UInt64 ui64AuthToken =	((UInt64)GeocacheData->ucAuthToken[0]) +
							((UInt64)GeocacheData->ucAuthToken[1] << 8) +
							((UInt64)GeocacheData->ucAuthToken[2] << 16) +
							((UInt64)GeocacheData->ucAuthToken[3] << 24) +
							((UInt64)GeocacheData->ucAuthToken[4] << 32) +
							((UInt64)GeocacheData->ucAuthToken[5] << 40) +
							((UInt64)GeocacheData->ucAuthToken[6] << 48);
	this->labelAuthToken->Text = ui64AuthToken.ToString("X2");

	// update the operating time for the battery common page
	this->label_Bat_ElpTimeDisplay->Text = ulTotalTime.ToString();
}

/**************************************************************************
 * GeocacheSensor::onTimerTock
 * 
 * Simulates a sensor event, updating simulator data based on this event
 * Modifications to the timer interval are applied immediately after this
 * at ANTChannel
 *
 * usEventTime_: current time (ms)
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheSensor::onTimerTock(USHORT eventTime)
{
	UpdateDisplay();
}

/**************************************************************************
 * GeocacheSensor::UpdateDisplayAckStatus
 * 
 * Adjusts the GUI depending on the ACK status of the tx message
 *
 * UCHAR status_:  gives the success or fail status of the tx message
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheSensor::UpdateDisplayAckStatus(UCHAR status_)
{
	// Nothing to do ...
}


/**************************************************************************
 * GeocacheSensor::numericUpDownLongitude_ValueChanged
 * 
 * Adjusts the current longitude via the GUI
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheSensor::numericUpDownLongitude_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
//	GeocacheData->slLongitude_SC = System::Convert::ToInt32(double (this->numericUpDownLongitude->Value) * GeocacheData->SEMI_CIRCLE_CONVERSION);
}

/**************************************************************************
 * GeocacheSensor::numericUpDownLatitude_ValueChanged
 * 
 * Adjusts the current latitude via the GUI
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheSensor::numericUpDownLatitude_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
//	GeocacheData->slLatitude_SC = System::Convert::ToInt32(double (this->numericUpDownLatitude->Value) * GeocacheData->SEMI_CIRCLE_CONVERSION);
}

/**************************************************************************
 * GeocacheSensor::checkBoxID_CheckedChanged
 * 
 * Enables and disables the ID Text Box
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheSensor::checkBoxID_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	if (this->checkBoxID->Checked == TRUE)
	{
		this->textBoxID->Enabled = TRUE;
	}
	else
	{
		this->textBoxID->Enabled = FALSE;
	}
}

// Boolean flag used to determine when a character other than a number is entered.
bool nonNumberEntered;

/**************************************************************************
 * GeocacheSensor:textBoxID_KeyDown
 * 
 * Handle the KeyDown event to determine the type of character entered into the control.
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheSensor::textBoxID_KeyDown( Object^ /*sender*/, System::Windows::Forms::KeyEventArgs^ e )
   {
      // Initialize the flag to false.
      nonNumberEntered = false;

      // Determine whether the keystroke is a number from the top of the keyboard.
	  // ... or a valid KEY ...
      if ( e->KeyCode < Keys::D0 || e->KeyCode > Keys::Z )
      {
         // Determine whether the keystroke is a number from the keypad.
         if ( e->KeyCode < Keys::NumPad0 || e->KeyCode > Keys::NumPad9 )
         {
            // Determine whether the keystroke is a backspace.
            if ( e->KeyCode != Keys::Back )
            {
               // A non-numerical keystroke was pressed.
               // Set the flag to true and evaluate in KeyPress event.
               nonNumberEntered = true;
            }
         }
      }
      // If shift key was pressed, it's not a number.
      if (Control::ModifierKeys == Keys::Shift) {
	     // Only allow Shifted A-Z
		 if ( e->KeyCode < Keys::A || e->KeyCode > Keys::Z )
            nonNumberEntered = true;
      }
   }

/**************************************************************************
 * GeocacheSensor:textBoxID_KeyPress
 * 
 * This event occurs after the KeyDown event and can be used to prevent
 * characters from entering the control.
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheSensor::textBoxID_KeyPress( Object^ /*sender*/, System::Windows::Forms::KeyPressEventArgs^ e )
   {
      // Check for the flag being set in the KeyDown event.
      if ( nonNumberEntered == true )
      {         // Stop the character from being entered into the control since it is non-numerical.
         e->Handled = true;
      }
   }


/**************************************************************************
 * GeocacheSensor::checkBoxPIN_CheckedChanged
 * 
 * Enables and disables the PIN Text Box
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheSensor::checkBoxPIN_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	if (this->checkBoxPIN->Checked == TRUE)
	{
		this->textBoxPIN->Enabled = TRUE;
	}
	else
	{
		this->textBoxPIN->Enabled = FALSE;
	}
}

/**************************************************************************
 * GeocacheSensor:textBoxPIN_KeyDown
 * 
 * Handle the KeyDown event to determine the type of character entered into the control.
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheSensor::textBoxPIN_KeyDown( Object^ /*sender*/, System::Windows::Forms::KeyEventArgs^ e )
   {
      // Initialize the flag to false.
      nonNumberEntered = false;

      // Determine whether the keystroke is a number from the top of the keyboard.
	  // ... or a valid KEY ...
      if ( e->KeyCode < Keys::D0 || e->KeyCode > Keys::D9 )
      {
         // Determine whether the keystroke is a number from the keypad.
         if ( e->KeyCode < Keys::NumPad0 || e->KeyCode > Keys::NumPad9 )
         {
            // Determine whether the keystroke is a backspace.
            if ( e->KeyCode != Keys::Back )
            {
               // A non-numerical keystroke was pressed.
               // Set the flag to true and evaluate in KeyPress event.
               nonNumberEntered = true;
            }
         }
      }
      // If shift key was pressed, it's not a number.
      if (Control::ModifierKeys == Keys::Shift) {
         nonNumberEntered = true;
      }
   }

/**************************************************************************
 * GeocacheSensor:textBoxPIN_KeyPress
 * 
 * This event occurs after the KeyDown event and can be used to prevent
 * characters from entering the control.
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheSensor::textBoxPIN_KeyPress( Object^ /*sender*/, System::Windows::Forms::KeyPressEventArgs^ e )
   {
      // Check for the flag being set in the KeyDown event.
      if ( nonNumberEntered == true )
      {         // Stop the character from being entered into the control since it is non-numerical.
         e->Handled = true;
      }
   }


/**************************************************************************
 * GeocacheSensor::checkBoxLatitude_CheckedChanged
 * 
 * Enables and disables the Latitude numeric upDown
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheSensor::checkBoxLatitude_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	if (this->checkBoxLatitude->Checked == TRUE)
	{
		this->numericUpDownLatitude->Enabled = TRUE;
	}
	else
	{
		this->numericUpDownLatitude->Enabled = FALSE;
	}
}

/**************************************************************************
 * GeocacheSensor::checkBoxLongitude_CheckedChanged
 * 
 * Enables and disables the Longitude numeric upDown
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheSensor::checkBoxLongitude_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	if (this->checkBoxLongitude->Checked == TRUE)
	{
		this->numericUpDownLongitude->Enabled = TRUE;
	}
	else
	{
		this->numericUpDownLongitude->Enabled = FALSE;
	}
}

/**************************************************************************
 * GeocacheSensor::checkBoxHint_CheckedChanged
 * 
 * Enables and disables the Hint Test Box
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheSensor::checkBoxHint_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	if (this->checkBoxHint->Checked == TRUE)
	{
		this->textBoxHint->Enabled = TRUE;
	}
	else
	{
		this->textBoxHint->Enabled = FALSE;
	}
}

/**************************************************************************
 * GeocacheSensor::checkBoxLoggedVisits_CheckedChanged
 * 
 * Enables and disables the Various Logged Visit Parameters ...
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheSensor::checkBoxLoggedVisits_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	if (this->checkBoxLoggedVisits->Checked == TRUE)
	{
		this->numericUpDownNumVisits->Enabled = TRUE;
		this->numericUpDownYear->Enabled = TRUE;
		this->numericUpDownMonth->Enabled = TRUE;
		this->numericUpDownDay->Enabled = TRUE;
		this->numericUpDownHours->Enabled = TRUE;
		this->numericUpDownMinutes->Enabled = TRUE;
		this->numericUpDownSeconds->Enabled = TRUE;
	}
	else
	{
		this->numericUpDownNumVisits->Enabled = FALSE;
		this->numericUpDownYear->Enabled = FALSE;
		this->numericUpDownMonth->Enabled = FALSE;
		this->numericUpDownDay->Enabled = FALSE;
		this->numericUpDownHours->Enabled = FALSE;
		this->numericUpDownMinutes->Enabled = FALSE;
		this->numericUpDownSeconds->Enabled = FALSE;
	}
}

/**************************************************************************
 * GeocacheSensor::numericUpDownNumVisits_ValueChanged
 * 
 * Adjusts the current Number of Visits via the GUI
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheSensor::numericUpDownNumVisits_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	// Nothing to do ...
}

/**************************************************************************
 * GeocacheSensor::button_Glb_GlobalDataUpdate_Click
 * 
 * Adjusts the global pages 80 and 81 via the GUI
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheSensor::button_Glb_GlobalDataUpdate_Click(System::Object^  sender, System::EventArgs^  e)
{
	this->labelError->Visible = false;
	// required common pages
	try
	{
		if(this->checkBox_NoSerial->Checked)
			commonData->ulSerialNum = 0xFFFFFFFF;
		else
			commonData->ulSerialNum = System::Convert::ToUInt32(this->textBox_Glb_SerialNumChange->Text);

		commonData->ucSwVersion = System::Convert::ToByte(this->textBox_Glb_SoftwareVerChange->Text);
		commonData->ucHwVersion = System::Convert::ToByte(this->textBox_Glb_HardwareVerChange->Text);
		commonData->usMfgID = System::Convert::ToUInt16(this->textBox_Glb_ManfIDChange->Text);
		commonData->usModelNum = System::Convert::ToUInt16(this->textBox_Glb_ModelNumChange->Text);
	}
	catch(...)
	{
		this->labelError->Visible = true;
	}
}


/**************************************************************************
 * GeocacheSensor::checkBox_Bat_Status_CheckedChanged
 * 
 * Enables and disables the battery voltage GUI elements
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheSensor::checkBox_Bat_Status_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	if (this->checkBox_Bat_Status->Checked == TRUE)
	{
		commonData->bBattPageEnabled = TRUE;
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
			commonData->eTimeResolution = CommonData::TimeResolution::SIXTEEN;
		
		if (this->radioButton_Bat_Elp2Units->Checked)
			commonData->eTimeResolution = CommonData::TimeResolution::TWO;

		commonData->ulOpTime = ulTotalTime / (UCHAR) commonData->eTimeResolution;
		commonData->usBatVoltage256 = ((USHORT) System::Convert::ToByte(this->numericUpDown_Bat_VoltInt->Value)) << 8 | (commonData->usBatVoltage256 & 0xFF);	// Integer portion in high byte
		commonData->usBatVoltage256 |= System::Convert::ToByte(this->numericUpDown_Bat_VoltFrac->Value) | (commonData->usBatVoltage256 & 0xFF00);	// Fractional portion in low byte
	}

	else
	{
		commonData->bBattPageEnabled = FALSE;
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
}

/**************************************************************************
 * GeocacheSensor::checkBox_Bat_Voltage_CheckedChanged
 * 
 * Enables and disables the battery voltage numeric upDown
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheSensor::checkBox_Bat_Voltage_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	if (this->checkBox_Bat_Voltage->Checked == TRUE)
	{
		this->numericUpDown_Bat_VoltInt->Enabled = TRUE;
		this->numericUpDown_Bat_VoltFrac->Enabled = TRUE;
		commonData->usBatVoltage256 = ((USHORT) System::Convert::ToByte(this->numericUpDown_Bat_VoltInt->Value)) << 8 | (commonData->usBatVoltage256 & 0xFF);	// Integer portion in high byte
		commonData->usBatVoltage256 |= System::Convert::ToByte(this->numericUpDown_Bat_VoltFrac->Value) | (commonData->usBatVoltage256 & 0xFF00);	// Fractional portion in low byte
	}
	else
	{
		this->numericUpDown_Bat_VoltInt->Enabled = FALSE;
		this->numericUpDown_Bat_VoltFrac->Enabled = FALSE;
		commonData->usBatVoltage256 = commonData->BATTERY_VOLTAGE_INVALID;
	}
}

/**************************************************************************
 * GeocacheSensor::numericUpDown_Bat_VoltInt_ValueChanged
 * 
 * changes the coarse battery voltage
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheSensor::numericUpDown_Bat_VoltInt_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	commonData->usBatVoltage256 = ((USHORT) System::Convert::ToByte(this->numericUpDown_Bat_VoltInt->Value)) << 8 | (commonData->usBatVoltage256 & 0xFF);	// Integer portion in high byte
}

/**************************************************************************
 * GeocacheSensor::numericUpDown_Bat_VoltFrac_ValueChanged
 * 
 * changes the fractional battery voltage
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheSensor::numericUpDown_Bat_VoltFrac_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	commonData->usBatVoltage256 = System::Convert::ToByte(this->numericUpDown_Bat_VoltFrac->Value) | (commonData->usBatVoltage256 & 0xFF00);	// Fractional portion in low byte
}

/**************************************************************************
 * GeocacheSensor::comboBoxBatStatus_SelectedIndexChanged
 * 
 * changes the battery status
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheSensor::comboBoxBatStatus_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
{
	UpdateBatStatus();
}

/**************************************************************************
 * GeocacheSensor::UpdateBatStatus
 * 
 * Updates the battery status
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheSensor::UpdateBatStatus()
{
	if (this->comboBoxBatStatus->Text->Equals("New"))
	{
		commonData->eBatStatus = CommonData::BatStatus::NEW;
	}

	else if (this->comboBoxBatStatus->Text->Equals("Good"))
	{
		commonData->eBatStatus = CommonData::BatStatus::GOOD;
	}

	else if (this->comboBoxBatStatus->Text->Equals("Ok"))
	{
		commonData->eBatStatus = CommonData::BatStatus::OK;
	}

	else if (this->comboBoxBatStatus->Text->Equals("Low"))
	{
		commonData->eBatStatus = CommonData::BatStatus::LOW;
	}

	else if (this->comboBoxBatStatus->Text->Equals("Critical"))
	{
		commonData->eBatStatus = CommonData::BatStatus::CRITICAL;
	}

	else if (this->comboBoxBatStatus->Text->Equals("Invalid"))
	{
		commonData->eBatStatus = CommonData::BatStatus::INVALID;
	}
}

/**************************************************************************
 * GeocacheSensor::button_Bat_ElpTimeUpdate_Click
 * 
 * changes the elapsed battery time
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheSensor::button_Bat_ElpTimeUpdate_Click(System::Object^  sender, System::EventArgs^  e)
{
	ulTotalTime = System::Convert::ToUInt32(this->textBox_Bat_ElpTimeChange->Text);
	commonData->ulOpTime = ulTotalTime / (UCHAR) commonData->eTimeResolution;
}

/**************************************************************************
 * GeocacheSensor::radioButton_Bat_Elp2Units_CheckedChanged
 * 
 * changes the elapsed battery time units
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheSensor::radioButton_Bat_Elp2Units_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	if (this->radioButton_Bat_Elp2Units->Checked)
		commonData->eTimeResolution = CommonData::TimeResolution::TWO;
}

/**************************************************************************
 * GeocacheSensor::radioButton_Bat_Elp2Units_CheckedChanged
 * 
 * changes the elapsed battery time units
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheSensor::radioButton_Bat_Elp16Units_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	if (this->radioButton_Bat_Elp16Units->Checked)
		commonData->eTimeResolution = CommonData::TimeResolution::SIXTEEN;
}



/**************************************************************************
 * GeocacheSensor::numericUpDownDay_ValueChanged
 * 
 * Sets value of the day
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheSensor::numericUpDownDay_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	commonData->ucDays = System::Convert::ToByte(this->numericUpDownDay->Value);
}

/**************************************************************************
 * GeocacheSensor::numericUpDownMonth_ValueChanged
 * 
 * Sets value of the month
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheSensor::numericUpDownMonth_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	commonData->ucMonth = System::Convert::ToByte(this->numericUpDownMonth->Value);
}

/**************************************************************************
 * GeocacheSensor::numericUpDownYear_ValueChanged
 * 
 * Sets value of the year
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheSensor::numericUpDownYear_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	commonData->ucYears = System::Convert::ToByte(this->numericUpDownYear->Value);
}

/**************************************************************************
 * GeocacheSensor::numericUpDownHours_ValueChanged
 * 
 * Sets value of the hours
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheSensor::numericUpDownHours_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	commonData->ucHours = System::Convert::ToByte(this->numericUpDownHours->Value);
}

/**************************************************************************
 * GeocacheSensor::numericUpDownMinutes_ValueChanged
 * 
 * Sets value of the minutes
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheSensor::numericUpDownMinutes_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	commonData->ucMinutes = System::Convert::ToByte(this->numericUpDownMinutes->Value);
}

/**************************************************************************
 * GeocacheSensor::numericUpDownSeconds_ValueChanged
 * 
 * Sets value of the minutes
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheSensor::numericUpDownSeconds_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	commonData->ucSeconds = System::Convert::ToByte(this->numericUpDownSeconds->Value);
}



/**************************************************************************
 * GeocacheSensor::checkBox_NoSerial_CheckedChanged
 * 
 * Handles checkBox CheckedChanged event
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheSensor::checkBox_NoSerial_CheckedChanged(System::Object ^sender, System::EventArgs ^e)
{
	if(this->checkBox_NoSerial->Checked)
		this->textBox_Glb_SerialNumChange->Enabled = false;
	else
		this->textBox_Glb_SerialNumChange->Enabled = true;
}