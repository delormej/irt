/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/


#pragma once
#include "stdafx.h"
#include "GeocacheDisplay.h"
#include "antmessage.h"

using namespace System::Runtime::InteropServices;

static ULONG ulMessageCount = 0;

static BOOL bFoundGeocache;		// GPS 'Found' the Geocache	
static BOOL bAllPagesRecvd;		// GPS 'Received' all Geocache Pages
static BOOL bProgramming;		// GPS 'Programming' Geocache
static BOOL bSendAuthReq;	
static BOOL bUpdateLoggedVisit;

static UCHAR ucNumPagesRecvd;	// GPS Number of Pages Received

static time_t tmtEpoch;
static struct tm tmEpoch;

/**************************************************************************
* GeocacheDisplay::InitializeSim
* 
* Initialize the simulator variables
*
* returns: N/A
*
**************************************************************************/
void GeocacheDisplay::InitializeSim()
{
	ulTimerInterval = 250; // in ms - simulates the sensor sending the most relevant information every message period

	dbDispAcumTime = 0;
	dbDispAcumDist = 0;
	usLastEventTime = 0;
	ulTotalTime = 0;

	bFoundGeocache = FALSE;
	bAllPagesRecvd = FALSE;

	GeocacheData->InitDisplay();

	this->buttonProgram->Enabled = FALSE;
	this->buttonErase->Enabled = FALSE;
	ulMessageCount = 0;
	ucNumPagesRecvd = 0;
	GeocacheData->ucTotalPages = 0;
	GeocacheData->ucCurrProgPage = 0;
	GeocacheData->ucProgTotalPages = 0;
	GeocacheData->InitDisplay();
	UpdateDisplay();


	commonData->eTimeResolution = CommonData::TimeResolution::TWO;
	commonData->ulOpTime = ulTotalTime;

	tmEpoch.tm_year = 1989 - 1900;
	tmEpoch.tm_mon = 12 - 1;
	tmEpoch.tm_mday = 31;
	tmEpoch.tm_hour = 00;
	tmEpoch.tm_min = 00;
	tmEpoch.tm_sec = 00;
	tmtEpoch = mktime(&tmEpoch);
}

/**************************************************************************
* GeocacheDisplay::ANT_eventNotification
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
void GeocacheDisplay::ANT_eventNotification(UCHAR ucEventCode_, UCHAR* pucEventBuffer_)
{
	switch(ucEventCode_)
	{
		case EVENT_TX:
//			HandleTransmit((UCHAR*) pucEventBuffer_);
			break;

		case EVENT_RX_BROADCAST:
		case EVENT_RX_ACKNOWLEDGED:
		case EVENT_RX_BURST_PACKET: // intentional fall thru
			HandleReceive((UCHAR*) pucEventBuffer_);
			break;

		case EVENT_TRANSFER_TX_COMPLETED:
			UpdateAckStatus((UCHAR*) pucEventBuffer_);
			break;

		case EVENT_ACK_TIMEOUT:	
		case EVENT_TRANSFER_TX_FAILED: // intentional fall through
			UpdateDisplayAckStatus(GeocacheData->ACK_FAIL);
			break;

		case MESG_CLOSE_CHANNEL_ID:
			InitializeSim();
			break;

		default:
			break;
	}
}

/**************************************************************************
 * GeocacheDisplay::HandleReceive
 * 
 * Handle incoming (Geocache) Broadcast Packets
 *
 * pucRxBuffer_: pointer to the receive buffer
 * 
 * returns: N/A
 *
 **************************************************************************/
void GeocacheDisplay::HandleReceive(UCHAR* pucRxBuffer_)
{
	ulMessageCount++;

	// Check to see if we are Programming the Geocache ?
	//   ... ignore incoming requests if we are !!!
	if (!bProgramming)
	{
		UCHAR ucPage = pucRxBuffer_[0];

		// Mark Page Read ...
		if (ucPage <= commonData->PAGE82)
			GeocacheData->bPageRead[ucPage] = TRUE;

		// Check if this is Common Pages or Geocache Pages
		if (ucPage < commonData->START_COMMON_PAGE)
			GeocacheData->Decode(pucRxBuffer_);
		else
			commonData->Decode(pucRxBuffer_);

		UpdateDisplay();

		// If 'just' discovered the Geocache - Send PAGE_1 (PIN) Request
		if (!bFoundGeocache)
		{
			pucRxBuffer_[0] = 0x46;		// Page Request
			pucRxBuffer_[1] = 0xFF;
			pucRxBuffer_[2] = 0xFF;
			pucRxBuffer_[3] = 0xFF;
			pucRxBuffer_[4] = 0xFF;
			pucRxBuffer_[5] = 0x01;		// (Un)Acknowledged Response / 1 Time
			pucRxBuffer_[6] = 0x01;		// PAGE 1
			pucRxBuffer_[7] = 0x01;		// Data Page Request
			requestBcastMsg(pucRxBuffer_);
			bFoundGeocache = TRUE;
		}
		// Wait for Page Rotation Data before explicitly Requesting ...
		else if (!bAllPagesRecvd && (ulMessageCount > GeocacheData->MESG_WAIT_REQ))
		{
			pucRxBuffer_[0] = 0x46;		// Page Request
			pucRxBuffer_[1] = 0xFF;
			pucRxBuffer_[2] = 0xFF;
			pucRxBuffer_[3] = 0xFF;
			pucRxBuffer_[4] = 0xFF;
			pucRxBuffer_[5] = 0x01;		// (Un)Acknowledged Response / 1 Time
			pucRxBuffer_[6] = NextUnReadPage();
			pucRxBuffer_[7] = 0x01;		// Data Page Request
			requestBcastMsg(pucRxBuffer_);
		}
		else if (bSendAuthReq)
		{
			GeocacheData->GenerateAuthRequestPage(pucRxBuffer_);
			requestAckMsg(pucRxBuffer_);
			bSendAuthReq = FALSE;
			if (GeocacheData->bLoggedVisitsEnabled) bUpdateLoggedVisit = TRUE;
		}
		else if (bUpdateLoggedVisit)
		{
			UpdateLoggedVisit();
			GeocacheData->UpdateLoggedVisitPage();
			GeocacheData->GetPageData(GeocacheData->ucLoggedVisitPage, pucRxBuffer_);
			requestAckMsg(pucRxBuffer_);
			bUpdateLoggedVisit = FALSE;
		}
	}
	else
	{
		// Programming Pages !!!
		GeocacheData->GetPageData(GeocacheData->ucCurrProgPage, pucRxBuffer_);
		requestAckMsg(pucRxBuffer_);

		UpdateDisplay();
	}
}

/**************************************************************************
 * GeocacheDisplay::UpdateAckStatus
 * 
 * Updates Page Programming based on ACK status of the tx message
 *
 * UCHAR status_:  gives the success or fail status of the tx message
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheDisplay::UpdateAckStatus(UCHAR* pucRxBuffer_)
{
	if (bProgramming)
	{
		// Mark Page Read ...
		if (pucRxBuffer_[0] < GeocacheData->PAGE_32)
			GeocacheData->bPageProg[pucRxBuffer_[0]] = TRUE;

		if (GeocacheData->ucCurrProgPage >= GeocacheData->ucProgTotalPages)
		{
			bProgramming = FALSE;
			this->labelProgStatus->Text = "Done Programming ...";
			this->labelProgStatus->ForeColor = System::Drawing::Color::Black;
		}
		else
		{
			GeocacheData->ucCurrProgPage++;
		}
	}
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
void GeocacheDisplay::UpdateDisplayAckStatus(UCHAR status_)
{
	// Nothing to do ...
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

void GeocacheDisplay::Authentication(UCHAR* pucRxBuffer_)
{
	GeocacheData->GenerateAuthRequestPage(pucRxBuffer_);
	requestBcastMsg(pucRxBuffer_);
}	

/**************************************************************************
 * GeocacheDisplay::HandleTransmit
 * 
 * Encode data generated by simulator for transmission
 *
 * ucPageNum_: The page number to be transmitted
 * pucTxBuffer_: pointer to the transmit buffer
 * 
 * returns: N/A
 *
 **************************************************************************/
void GeocacheDisplay::HandleTransmit(UCHAR* pucTxBuffer_)
{
}

/**************************************************************************
 * Geocache::NextUnReadPage
 * 
 * Returns the Page # of the next unread Page
 *
 * Exceptions are thrown when dealing with invalid data
 *
 * returns: N/A
 *
 **************************************************************************/
UCHAR GeocacheDisplay::NextUnReadPage(void)
{
	ucNumPagesRecvd = 0;

	// Check Geocache Pages
	for (UCHAR ucPage=0; ucPage<GeocacheData->ucTotalPages; ucPage++)
	{
		if (!GeocacheData->bPageRead[ucPage])
			return (ucPage);
		else
			ucNumPagesRecvd++;
	}

	// Common Data Pages
	if (!GeocacheData->bPageRead[commonData->PAGE80]) return (commonData->PAGE80);
	if (!GeocacheData->bPageRead[commonData->PAGE81]) return (commonData->PAGE81);
	if (!GeocacheData->bPageRead[commonData->PAGE82]) return (commonData->PAGE82);

	bAllPagesRecvd = TRUE;
	bSendAuthReq = TRUE;

	this->buttonProgram->Enabled = TRUE;
	this->buttonErase->Enabled = TRUE;

	return (0);
}

/**************************************************************************
 * GeocacheDisplay::UpdateLoggedVisit
 * 
 * Update the Logged Visits Page (current Time)
 * 
 * returns: N/A
 *
 **************************************************************************/
void GeocacheDisplay::UpdateLoggedVisit(void)
{
	time_t tmtNow;
	//struct tm * ptmNow;

	time(&tmtNow);
//	ptmNow = localtime(&tmtNow);
	GeocacheData->ulLastVisitTimestamp = (ULONG) difftime(tmtNow, tmtEpoch);

	GeocacheData->usNumVisits++;
}

/**************************************************************************
 * GeocacheDisplay::buttonForget_Click
 * 
 * Forget *this* Geocache
 * 
 * returns: N/A
 *
 **************************************************************************/
void GeocacheDisplay::buttonForget_Click(System::Object^  sender, System::EventArgs^  e)
{
	bFoundGeocache = FALSE;
	bAllPagesRecvd = FALSE;
	this->buttonProgram->Enabled = FALSE;
	this->buttonErase->Enabled = FALSE;
	ulMessageCount = 0;
	ucNumPagesRecvd = 0;
	GeocacheData->ucTotalPages = 0;
	GeocacheData->ucCurrProgPage = 0;
	GeocacheData->ucProgTotalPages = 0;
	GeocacheData->InitDisplay();
	UpdateDisplay();
}

/**************************************************************************
 * GeocacheDisplay::buttonErase_Click
 * 
 * Erase the Geocache
 * 
 * returns: N/A
 *
 **************************************************************************/
void GeocacheDisplay::buttonErase_Click(System::Object^  sender, System::EventArgs^  e)
{
	ucNumPagesRecvd = 0;
	GeocacheData->InitDisplay();
	GeocacheData->GenerateProgPages();
	GeocacheData->ucProgTotalPages = 32;		// Initialize ALL Pages to DEFAULTs !!!
	GeocacheData->ucCurrProgPage = GeocacheData->PAGE_0;	// Start Programming with Page 0
	bProgramming = TRUE;
	UpdateDisplay();
}

/**************************************************************************
 * GeocacheDisplay::buttonProgram_Click
 * 
 * Update the Programmable Pages (per UI Values)
 * 
 * returns: N/A
 *
 **************************************************************************/
void GeocacheDisplay::buttonProgram_Click(System::Object^  sender, System::EventArgs^  e)
{
	// Check PIN
	if (GeocacheData->ulPIN == GeocacheData->DEFAULT_PIN)	// Default/Unprogrammed
	{
		bProgramming = TRUE;
		this->labelProgStatus->Text = "Programming...";
		this->labelProgStatus->ForeColor = System::Drawing::Color::Green;
	}
	else if ((this->textBoxPIN->Text->Length > 0) && (System::Convert::ToInt32(this->textBoxPIN->Text) == GeocacheData->ulPIN))
	{
		bProgramming = TRUE;
		this->labelProgStatus->Text = "Programming ...";
		this->labelProgStatus->ForeColor = System::Drawing::Color::Green;
	}
	else
	{
		bProgramming = FALSE;
		this->labelProgStatus->Text = "Incorrect PIN";
		this->labelProgStatus->ForeColor = System::Drawing::Color::Red;
		return;
	}

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
		GeocacheData->ulPIN = System::Convert::ToUInt32(this->textBoxPIN->Text);
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
		time_t tmtLastVisit;
		struct tm tmLastVisit;

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

	UCHAR ucPriorTotalPages = GeocacheData->ucTotalPages;
	GeocacheData->GenerateProgPages();
	GeocacheData->ucProgTotalPages = (ucPriorTotalPages > GeocacheData->ucTotalPages) ? ucPriorTotalPages : GeocacheData->ucTotalPages;
	GeocacheData->ucCurrProgPage = GeocacheData->PAGE_0;	// Start Programming with Page 0
}

/**************************************************************************
 * GeocacheDisplay::Update Display
 * 
 * Update the GUI with all the changes
 * 
 * returns: N/A
 *
 **************************************************************************/
void GeocacheDisplay::UpdateDisplay()
{
	// ID
	System::Text::StringBuilder^ builder = gcnew System::Text::StringBuilder();
	for(int i=0; i<GeocacheData->cID->Length; i++) builder->Append((Char)(GeocacheData->cID[i]+0x20));
	this->labelGCdID->Text = builder->ToString();
	if (this->tabControl1->SelectedIndex != 3) // Don't update if Programming Tab Selected
	{
		this->textBoxID->Text = builder->ToString();
		this->labelID->Text = builder->ToString();
	}

	// PIN
	this->labelPIN->Text = GeocacheData->ulPIN.ToString();
	this->labelGCdPIN->Text = GeocacheData->ulPIN.ToString();
	if (this->tabControl1->SelectedIndex != 3) // Don't update if Programming Tab Selected
		this->textBoxPIN->Text = GeocacheData->ulPIN.ToString();

	// Total Pages
	this->labelGCdTotNumPages->Text = GeocacheData->ucTotalPages.ToString();
	this->labelGCdPagesRecvd->Text = ucNumPagesRecvd.ToString();

	// Total Prog Pages
	this->labelProgTotalPages->Text = GeocacheData->ucProgTotalPages.ToString();
	this->labelCurrProgPage->Text = GeocacheData->ucCurrProgPage.ToString();

	// Lat/Lon Pages
	if (GeocacheData->bLatitudeEnabled)
	{
		this->labelLat->Text = ((double)GeocacheData->slLatitude_SC / GeocacheData->SEMI_CIRCLE_CONVERSION).ToString("N2");
		this->labelGCdLat->Text = ((double)GeocacheData->slLatitude_SC / GeocacheData->SEMI_CIRCLE_CONVERSION).ToString("N2");
		if (this->tabControl1->SelectedIndex != 3) // Don't update if Programming Tab Selected
			this->numericUpDownLatitude->Text = ((double)GeocacheData->slLatitude_SC / GeocacheData->SEMI_CIRCLE_CONVERSION).ToString();
	}
	else
	{
		this->labelLat->Text = "---";
		this->labelGCdLat->Text = "---";
	}

	if (GeocacheData->bLongitudeEnabled)
	{
		this->labelLon->Text = ((double)GeocacheData->slLongitude_SC / GeocacheData->SEMI_CIRCLE_CONVERSION).ToString("N2");
		this->labelGCdLon->Text = ((double)GeocacheData->slLongitude_SC / GeocacheData->SEMI_CIRCLE_CONVERSION).ToString("N2");
		if (this->tabControl1->SelectedIndex != 3) // Don't update if Programming Tab Selected
			this->numericUpDownLongitude->Text = ((double)GeocacheData->slLongitude_SC / GeocacheData->SEMI_CIRCLE_CONVERSION).ToString();
	}
	else
	{
		this->labelLon->Text = "---";
		this->labelGCdLon->Text = "---";
	}

	// Hint Page(s)
	if (GeocacheData->bHintEnabled)
	{
		System::Text::StringBuilder^ builder = gcnew System::Text::StringBuilder();
		for(int i=0; i<GeocacheData->cHint->Length; i++) builder->Append((Char)GeocacheData->cHint[i]);
		this->labelHint->Text = builder->ToString();
		this->labelGCdHint->Text = builder->ToString();
		if (this->tabControl1->SelectedIndex != 3) // Don't update if Programming Tab Selected
			this->textBoxHint->Text = builder->ToString();
	}
	else
	{
		this->labelHint->Text = "---";
		this->labelGCdHint->Text = "---";
	}

	// Logged Visits Page
	if (GeocacheData->bLoggedVisitsEnabled && (GeocacheData->ulLastVisitTimestamp > 0))
	{
		this->labelNumVisits->Text = GeocacheData->usNumVisits.ToString();
		this->labelGCdNumVisits->Text = GeocacheData->usNumVisits.ToString();

		if (this->tabControl1->SelectedIndex != 3) // Don't update if Programming Tab Selected
			this->numericUpDownNumVisits->Value = GeocacheData->usNumVisits;

		time_t tmtLastVisit; 
		struct tm * tmLastVisit;

		tmtLastVisit = ((time_t)GeocacheData->ulLastVisitTimestamp + tmtEpoch);
#pragma warning (disable : 4996)	//This is used to quiet the compiler from an invalid warning
		tmLastVisit = localtime(&tmtLastVisit);

		if (this->tabControl1->SelectedIndex != 3) // Don't update if Programming Tab Selected
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
		this->labelGCdTimestamp->Text = builder->ToString();
	}
	else
	{
		this->labelNumVisits->Text = "---";
		this->labelTimestamp->Text = "---";
		this->labelGCdNumVisits->Text = "---";
		this->labelGCdTimestamp->Text = "---";
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
	this->labelAuthSerialNum->Text = GeocacheData->ulSerialNum.ToString();
	this->labelAuthNonce->Text = GeocacheData->usNonce.ToString();

	// update the values for the HW common data page
	if (commonData->usMfgID != 0)
	{
		this->label_Glb_ManfIDDisplay->Text = commonData->usMfgID.ToString();
		this->label_Glb_HardwareVerDisplay->Text = commonData->ucHwVersion.ToString();
		this->label_Glb_ModelNumDisplay->Text = commonData->usModelNum.ToString();
	}

	// update the values for the SW common data page
	if (commonData->ulSerialNum != 0)
	{
		if(commonData->ulSerialNum == 0xFFFFFFFF)
			this->label_Glb_SerialNumDisplay->Text = "N/A";
		else
			this->label_Glb_SerialNumDisplay->Text = commonData->ulSerialNum.ToString();
		
		this->label_Glb_SoftwareVerDisplay->Text = commonData->ucSwVersion.ToString();
	}

	// update the values for the battery common data page
	if (commonData->ulOpTime != 0)
	{
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
	}
}

/**************************************************************************
 * GeocacheDisplay::onTimerTock
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
void GeocacheDisplay::onTimerTock(USHORT eventTime)
{
	UpdateDisplay();
}

/**************************************************************************
 * GeocacheDisplay::numericUpDownLongitude_ValueChanged
 * 
 * Adjusts the current longitude via the GUI
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheDisplay::numericUpDownLongitude_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
//	GeocacheData->slLongitude_SC = System::Convert::ToInt32(double (this->numericUpDownLongitude->Value) * GeocacheData->SEMI_CIRCLE_CONVERSION);
}

/**************************************************************************
 * GeocacheDisplay::numericUpDownLatitude_ValueChanged
 * 
 * Adjusts the current latitude via the GUI
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheDisplay::numericUpDownLatitude_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
//	GeocacheData->slLatitude_SC = System::Convert::ToInt32(double (this->numericUpDownLatitude->Value) * GeocacheData->SEMI_CIRCLE_CONVERSION);
}

/**************************************************************************
 * GeocacheDisplay::checkBoxID_CheckedChanged
 * 
 * Enables and disables the ID Text Box
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheDisplay::checkBoxID_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
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
 * GeocacheDisplay:textBoxID_KeyDown
 * 
 * Handle the KeyDown event to determine the type of character entered into the control.
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheDisplay::textBoxID_KeyDown( Object^ /*sender*/, System::Windows::Forms::KeyEventArgs^ e )
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
 * GeocacheDisplay:textBoxID_KeyPress
 * 
 * This event occurs after the KeyDown event and can be used to prevent
 * characters from entering the control.
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheDisplay::textBoxID_KeyPress( Object^ /*sender*/, System::Windows::Forms::KeyPressEventArgs^ e )
   {
      // Check for the flag being set in the KeyDown event.
      if ( nonNumberEntered == true )
      {         // Stop the character from being entered into the control since it is non-numerical.
         e->Handled = true;
      }
   }


/**************************************************************************
 * GeocacheDisplay::checkBoxPIN_CheckedChanged
 * 
 * Enables and disables the PIN Text Box
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheDisplay::checkBoxPIN_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
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
 * GeocacheDisplay:textBoxPIN_KeyDown
 * 
 * Handle the KeyDown event to determine the type of character entered into the control.
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheDisplay::textBoxPIN_KeyDown( Object^ /*sender*/, System::Windows::Forms::KeyEventArgs^ e )
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
 * GeocacheDisplay:textBoxPIN_KeyPress
 * 
 * This event occurs after the KeyDown event and can be used to prevent
 * characters from entering the control.
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheDisplay::textBoxPIN_KeyPress( Object^ /*sender*/, System::Windows::Forms::KeyPressEventArgs^ e )
   {
      // Check for the flag being set in the KeyDown event.
      if ( nonNumberEntered == true )
      {         // Stop the character from being entered into the control since it is non-numerical.
         e->Handled = true;
      }
   }


/**************************************************************************
 * GeocacheDisplay::checkBoxLatitude_CheckedChanged
 * 
 * Enables and disables the Latitude numeric upDown
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheDisplay::checkBoxLatitude_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
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
 * GeocacheDisplay::checkBoxLongitude_CheckedChanged
 * 
 * Enables and disables the Longitude numeric upDown
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheDisplay::checkBoxLongitude_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
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
 * GeocacheDisplay::checkBoxHint_CheckedChanged
 * 
 * Enables and disables the Hint Test Box
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheDisplay::checkBoxHint_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
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
 * GeocacheDisplay::checkBoxLoggedVisits_CheckedChanged
 * 
 * Enables and disables the Various Logged Visit Parameters ...
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheDisplay::checkBoxLoggedVisits_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
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
 * GeocacheDisplay::numericUpDownNumVisits_ValueChanged
 * 
 * Adjusts the current Number of Visits via the GUI
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheDisplay::numericUpDownNumVisits_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	// Nothing to do ...
}


/**************************************************************************
 * GeocacheDisplay::numericUpDownDay_ValueChanged
 * 
 * Sets value of the day
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheDisplay::numericUpDownDay_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	commonData->ucDays = System::Convert::ToByte(this->numericUpDownDay->Value);
}

/**************************************************************************
 * GeocacheDisplay::numericUpDownMonth_ValueChanged
 * 
 * Sets value of the month
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheDisplay::numericUpDownMonth_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	commonData->ucMonth = System::Convert::ToByte(this->numericUpDownMonth->Value);
}

/**************************************************************************
 * GeocacheDisplay::numericUpDownYear_ValueChanged
 * 
 * Sets value of the year
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheDisplay::numericUpDownYear_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	commonData->ucYears = System::Convert::ToByte(this->numericUpDownYear->Value);
}

/**************************************************************************
 * GeocacheDisplay::numericUpDownHours_ValueChanged
 * 
 * Sets value of the hours
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheDisplay::numericUpDownHours_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	commonData->ucHours = System::Convert::ToByte(this->numericUpDownHours->Value);
}

/**************************************************************************
 * GeocacheDisplay::numericUpDownMinutes_ValueChanged
 * 
 * Sets value of the minutes
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheDisplay::numericUpDownMinutes_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	commonData->ucMinutes = System::Convert::ToByte(this->numericUpDownMinutes->Value);
}

/**************************************************************************
 * GeocacheDisplay::numericUpDownSeconds_ValueChanged
 * 
 * Sets value of the minutes
 *
 * returns: N/A
 *
 **************************************************************************/
void GeocacheDisplay::numericUpDownSeconds_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
	commonData->ucSeconds = System::Convert::ToByte(this->numericUpDownSeconds->Value);
}

