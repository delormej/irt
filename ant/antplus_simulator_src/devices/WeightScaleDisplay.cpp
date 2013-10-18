/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/



#include "StdAfx.h"
#include "WeightScaleDisplay.h"


/**************************************************************************
 * WeightScaleDisplay::ANT_eventNotification
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
void WeightScaleDisplay::ANT_eventNotification(UCHAR ucEventCode_, UCHAR* pucEventBuffer_)
{
	switch(ucEventCode_)
	{
		case EVENT_RX_BROADCAST:
			HandleReceive((UCHAR*) pucEventBuffer_);	// Decode current data
			break;
		default:
			break;
	}
}


/**************************************************************************
 * WeightScaleDisplay::InitializeSim
 * 
 * Initializes simulator variables
 * 
 * returns: N/A
 *
 **************************************************************************/
void WeightScaleDisplay::InitializeSim()
{
	// Initialization of simulation variables
	wsPages->usBodyWeight100 = WeightScale::WEIGHT_INVALID;


	uprofUndef = gcnew WeightScaleUserProfile();

	// Set the display's U.P. to be the same as what is defaulted in the custom boxes
	if (this->radioButtonFemale->Checked == TRUE)
	{
		if(this->radioButtonAthleteTrue->Checked == TRUE)
			uprofCustom = gcnew WeightScaleUserProfile((USHORT) this->numericUpDownProfile->Value, WeightScaleUserProfile::GENDER_FEMALE, (UCHAR) this->numericUpDownAge->Value, (UCHAR) this->numericUpDownHeight->Value, (UCHAR) this->numericUpDownActivityLevel->Value, WeightScaleUserProfile::PRIORITY_WATCH, WeightScaleUserProfile::IS_ATHLETE);
		else
			uprofCustom = gcnew WeightScaleUserProfile((USHORT) this->numericUpDownProfile->Value, WeightScaleUserProfile::GENDER_FEMALE, (UCHAR) this->numericUpDownAge->Value, (UCHAR) this->numericUpDownHeight->Value, (UCHAR) this->numericUpDownActivityLevel->Value, WeightScaleUserProfile::PRIORITY_WATCH, WeightScaleUserProfile::NOT_ATHLETE);
	}
	else
	{
		if(this->radioButtonAthleteTrue->Checked == TRUE)
			uprofCustom = gcnew WeightScaleUserProfile((USHORT) this->numericUpDownProfile->Value, WeightScaleUserProfile::GENDER_MALE, (UCHAR) this->numericUpDownAge->Value, (UCHAR) this->numericUpDownHeight->Value, (UCHAR) this->numericUpDownActivityLevel->Value, WeightScaleUserProfile::PRIORITY_WATCH, WeightScaleUserProfile::IS_ATHLETE);
		else
			uprofCustom = gcnew WeightScaleUserProfile((USHORT) this->numericUpDownProfile->Value, WeightScaleUserProfile::GENDER_MALE, (UCHAR) this->numericUpDownAge->Value, (UCHAR) this->numericUpDownHeight->Value, (UCHAR) this->numericUpDownActivityLevel->Value, WeightScaleUserProfile::PRIORITY_WATCH, WeightScaleUserProfile::NOT_ATHLETE);
	}
	WeightScaleUserProfile::copyProfile(uprofCustom, wsPages->uprofDisplay);
	WeightScaleUserProfile::copyProfile(wsPages->uprofDisplay, wsPages->uprofActive);

	// Extended
	bSentProfile = FALSE;
	bRxAdvanced = FALSE;
	bRxMain = FALSE;
}


/**************************************************************************
 * WeightScaleDisplay::HandleReceive
 * 
 * Decode incoming transmissions
 * pucRxBuffer_: pointer to receive buffer
 *
 * returns: N/A
 *
 **************************************************************************/
void WeightScaleDisplay::HandleReceive(UCHAR* pucRxBuffer_)
{
	UCHAR ucPageNum = pucRxBuffer_[0];

	switch(ucPageNum)
	{
		case WeightScale::PAGE_BODY_WEIGHT:
		case WeightScale::PAGE_BODY_COMPOSITION:
		case WeightScale::PAGE_METABOLIC_INFO:
      case WeightScale::PAGE_BODY_MASS:
		case WeightScale::PAGE_USER_PROFILE:	// Intentional fall thru
			try
			{
				wsPages->Decode(pucRxBuffer_);
			}
			catch(CommonData::Error^ errorWeightScale)
			{
			}
			ProcessReceivedMessage(ucPageNum);
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

	UpdateDisplay(ucPageNum);
}


/**************************************************************************
 * WeightScaleDisplay::HandleTransmit
 * 
 * Encode data into Tx Buffer
 * ucPageNum_: Page to transmit
 *
 * returns: N/A
 *
 **************************************************************************/
void WeightScaleDisplay::HandleTransmit(UCHAR ucPageNum_)
{
	UCHAR aucTxBuffer[8];
	memset(aucTxBuffer, 8, 0x00);

	if(ucPageNum_ == WeightScale::PAGE_USER_PROFILE)	// Encode user profile
	{
		try
		{
			wsPages->Encode(ucPageNum_, aucTxBuffer);
		}
		catch(WeightScale::Error^ errorWeightScale)
		{
		}
		requestBcastMsg(aucTxBuffer);	// Send as broadcast message
	}
}


/**************************************************************************
 * WeightScaleDisplay::ProcessReceivedMessage
 * 
 * Processes capabilities and user profile received from the scale
 * Device specific decision tree for personal display
 * ucPageNum_: Page where profile was received
 *
 * returns: N/A
 *
 **************************************************************************/
void WeightScaleDisplay::ProcessReceivedMessage(UCHAR ucPageNum_)
{
	// Weight message?
	if(ucPageNum_ == WeightScale::PAGE_BODY_WEIGHT)
	{
		// Does display support profiles?
		if(wsPages->capabDisplay->bDisplayExchange)
		{
			// Process scale capabilities
			wsPages->capabDisplay->bScaleExchange = wsPages->capabScale->bScaleExchange;
			wsPages->capabDisplay->bScaleSet = wsPages->capabScale->bScaleSet;
			// Is user profile set on scale?
			if(wsPages->capabScale->bScaleSet)
			{
				// Has display sent user profile?
				if(bSentProfile)
				{
					// Does user profile ID match?
					// Compare scale user profile ID against profile loaded on display (active profile)
					if(wsPages->uprofActive->usID == wsPages->uprofScale->usID)
					{
						// Display all defined weight data
						bRxAdvanced = TRUE;
						bRxMain = TRUE;
					}
					else
					{
						SendUserProfile();	// Transmit user profile
					}
				}
				else
				{	
					SendUserProfile();		// Transmit user profile
				}
			}
			else
			{
				if(!bSentProfile)
					SendUserProfile();		// Transmit user profile
			}
		}
		else
		{
			// Display does not support profiles
			// Weight only
			bRxMain = TRUE;
		}
	}
	// User profile message?
	else if(ucPageNum_ == WeightScale::PAGE_USER_PROFILE)
	{
		// User profile defined on display?
		if(wsPages->uprofActive->usID == WeightScaleUserProfile::PROFILE_UNDEF)
		{
			this->labelStatusProfileNew->Text = L"New User Profile";
			// Accept new user profile
			WeightScaleUserProfile::copyProfile(wsPages->uprofScale, wsPages->uprofActive);
			// Apply a higher priority user ID (display generated) to the new profile
			wsPages->uprofActive->usID = 1754;
			UpdateDisplayActiveProfile();
		}
	}

}


/**************************************************************************
 * WeightScaleDisplay::SendUserProfile
 * 
 * Send user profile
 *
 * returns: N/A
 *
 **************************************************************************/
void WeightScaleDisplay::SendUserProfile()
{
	if(wsPages->capabScale->bScaleExchange)
	{
		HandleTransmit(WeightScale::PAGE_USER_PROFILE);
		bSentProfile = TRUE;
		this->labelStatusProfileSent->Text = L"User Profile Sent";
		UpdateDisplayActiveProfile();
		
		// Disable changes to the user profile after it has been sent
		this->numericUpDownProfile->Enabled = FALSE;
		this->numericUpDownAge->Enabled = FALSE;
		this->numericUpDownHeight->Enabled = FALSE;
		this->radioButtonFemale->Enabled = FALSE;
		this->radioButtonMale->Enabled = FALSE;
		this->numericUpDownActivityLevel->Enabled = FALSE;
		this->groupBoxAthlete->Enabled = FALSE;
	}
}


/**************************************************************************
 * WeightScaleDisplay::UpdateDisplay
 * 
 * Update display of incoming transmissions
 *
 * returns: N/A
 *
 **************************************************************************/
void WeightScaleDisplay::UpdateDisplay(UCHAR ucPageNum_)
{
	this->labelPage->Text = System::Convert::ToString(ucPageNum_);
	
	switch(ucPageNum_)
	{
		case WeightScale::PAGE_BODY_WEIGHT:
			this->labelPage2->Text = L"MAIN";
			// Display capabilities
			UpdateDisplayCapabilities();
			// Display the recieved weight data
			if(wsPages->usBodyWeight100 == WeightScale::WEIGHT_COMPUTING)
				this->labelWeight->Text = "Comp";
			else if (wsPages->usBodyWeight100 == WeightScale::WEIGHT_INVALID)
				this->labelWeight->Text = "Invalid";
			else
				this->labelWeight->Text = System::Convert::ToString((double) wsPages->usBodyWeight100 / 100);	// Weight in kg
         this->label_adv_weight->Text = this->labelWeight->Text;
			break;
		case WeightScale::PAGE_BODY_COMPOSITION:
			this->labelPage2->Text = L"BCOMP";
			if(bRxAdvanced)
			{
				if(wsPages->usHydrationP100 == WeightScale::WEIGHT_COMPUTING)
					this->label_Hydration->Text = "Comp";
				else if (wsPages->usHydrationP100 == WeightScale::WEIGHT_INVALID)
					this->label_Hydration->Text = "Invalid";
				else
					this->label_Hydration->Text = System::Convert::ToString((double) wsPages->usHydrationP100 / 100);	// Hydration (%)

				if(wsPages->usBodyFatP100 == WeightScale::WEIGHT_COMPUTING)
					this->label_BodyFat->Text = "Comp";
				else if (wsPages->usBodyFatP100 == WeightScale::WEIGHT_INVALID)
					this->label_BodyFat->Text = "Invalid";
				else
					this->label_BodyFat->Text = System::Convert::ToString((double) wsPages->usBodyFatP100 / 100);	// Body Fat (%)
			}
			else
			{
				this->label_Hydration->Text = "---";
				this->label_BodyFat->Text = "---";
			}
         this->label_adv_hydration->Text = this->label_Hydration->Text;
         this->label_adv_bodyfat->Text = this->label_BodyFat->Text;
			break;
		case WeightScale::PAGE_METABOLIC_INFO:
			this->labelPage2->Text = L"METAB";
			if(bRxAdvanced)
			{
				if(wsPages->usActiveMetRate4 == WeightScale::WEIGHT_COMPUTING)
					this->label_ActiveMetRate->Text = "Comp";
				else if (wsPages->usActiveMetRate4 == WeightScale::WEIGHT_INVALID)
					this->label_ActiveMetRate->Text = "Invalid";
				else
					this->label_ActiveMetRate->Text = System::Convert::ToString((double) wsPages->usActiveMetRate4 / 4);	// Active Metabolic Rate (kcal)

				if(wsPages->usBasalMetRate4 == WeightScale::WEIGHT_COMPUTING)
					this->label_BasalMetRate->Text = "Comp";
				else if (wsPages->usBasalMetRate4 == WeightScale::WEIGHT_INVALID)
					this->label_BasalMetRate->Text = "Invalid";
				else
					this->label_BasalMetRate->Text = System::Convert::ToString((double) wsPages->usBasalMetRate4 / 4);	// Basal Metabolic Rate (kcal)
			}
			else
			{
				this->label_ActiveMetRate->Text = "---";
				this->label_BasalMetRate->Text = "---";
			}
         this->label_adv_activerate->Text = this->label_ActiveMetRate->Text;
         this->label_adv_basalrate->Text = this->label_BasalMetRate->Text;
			break;
      case WeightScale::PAGE_BODY_MASS:
         this->labelPage2->Text = L"MASS";
         if(bRxAdvanced)
         {
            if(wsPages->usMuscleMass100 == WeightScale::WEIGHT_COMPUTING)
               this->label_adv_musclemass->Text = "Comp";
            else if (wsPages->usMuscleMass100 == WeightScale::WEIGHT_INVALID)
               this->label_adv_musclemass->Text = "Invalid";
            else
               this->label_adv_musclemass->Text = System::Convert::ToString((double) wsPages->usMuscleMass100/100);  // Muscle mass (1/100kg)

            if(wsPages->ucBoneMass10 == WeightScale::MASS_COMPUTING)
               this->label_adv_bonemass->Text = "Comp";
            else if (wsPages->ucBoneMass10 == WeightScale::MASS_INVALID)
               this->label_adv_bonemass->Text = "Invalid";
            else
               this->label_adv_bonemass->Text = System::Convert::ToString((double) wsPages->ucBoneMass10/10);  // Bone mass (1/10kg)
         }
         else
         {
            this->label_adv_musclemass->Text = "---";
            this->label_adv_bonemass->Text = "---";
         }
         break;
		case WeightScale::PAGE_USER_PROFILE:
			this->labelPage2->Text = L"UPR";
			UpdateDisplayCapabilities();
			this->labelStatusProfileRx->Text = L"User Profile Received";
			break;
		case CommonData::PAGE80:
			this->labelPage2->Text = L"MFG";
			this->labelHwVer->Text = System::Convert::ToString(commonPages->ucHwVersion);
			this->labelMfgID->Text = System::Convert::ToString(commonPages->usMfgID);
			this->labelModelNum->Text = System::Convert::ToString(commonPages->usModelNum);
			break;
		case CommonData::PAGE81:
			this->labelPage2->Text = L"PRD";
			this->labelSwVer->Text = System::Convert::ToString(commonPages->ucSwVersion);
			if(commonPages->ulSerialNum != 0xFFFFFFFF)
				this->labelSerialNum->Text = System::Convert::ToString((unsigned int) commonPages->ulSerialNum);
			else
				this->labelSerialNum->Text = "N/A";
			break;		
		default:
			break;
	}	
}

/**************************************************************************
 * WeightScaleDisplay::UpdateDisplayCapabilities
 * 
 * Update display of the capabilities received from the scale
 *
 * returns: N/A
 *
 **************************************************************************/
void WeightScaleDisplay::UpdateDisplayCapabilities()
{
	if(wsPages->capabScale->bScaleSet)
		this->labelProfileSetScale->Text = L"Yes";
	else
		this->labelProfileSetScale->Text = L"No";
	if(wsPages->capabScale->bScaleExchange)
		this->labelExchangeScale->Text = L"Yes";
	else
		this->labelExchangeScale->Text = L"No";
	if(wsPages->capabScale->bAntfs)
		this->labelAntfsRx->Text = L"Yes";
	else
		this->labelAntfsRx->Text = L"No";	
	if(wsPages->capabScale->bDisplayExchange)
		this->labelExchangeDisplay->Text = L"Yes";
	else
		this->labelExchangeDisplay->Text = L"No";
}

/**************************************************************************
 * WeightScaleDisplay::UpdateDisplayActiveProfile
 * 
 * Update display of the active profile
 *
 * returns: N/A
 *
 **************************************************************************/
void WeightScaleDisplay::UpdateDisplayActiveProfile()
{
	this->labelProfileActive->Text = System::Convert::ToString(wsPages->uprofActive->usID);
	this->labelProfileActive2->Text = System::Convert::ToString(wsPages->uprofActive->usID);
	if(wsPages->uprofActive->ucGender == WeightScaleUserProfile::GENDER_FEMALE)
		this->labelGenderActive->Text = L"Female";
	else
		this->labelGenderActive->Text = L"Male";
	this->labelAgeActive->Text = System::Convert::ToString(wsPages->uprofActive->ucAge);
	this->labelHeightActive->Text = System::Convert::ToString(wsPages->uprofActive->ucHeight);
	this->labelDescriptionActive->Text = System::Convert::ToString(wsPages->uprofActive->ucDescription);
	if(wsPages->uprofActive->bAthlete == TRUE)
		this->labelLifestyle->Text = "Athlete";
	else
		this->labelLifestyle->Text = "Standard";
}


/**************************************************************************
 * WeightScaleDisplay::checkBoxProfileSet_CheckedChanged
 * 
 * Set user profile
 *
 * returns: N/A
 *
 **************************************************************************/
System::Void WeightScaleDisplay::checkBoxProfileSet_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(this->checkBoxProfileSet->Checked)
	{	
		// Enable Custom Profile
		this->numericUpDownProfile->Enabled = TRUE;
		this->numericUpDownAge->Enabled = TRUE;
		this->numericUpDownHeight->Enabled = TRUE;
		this->radioButtonFemale->Enabled = TRUE;
		this->radioButtonMale->Enabled = TRUE;
		this->numericUpDownActivityLevel->Enabled = TRUE;
		this->groupBoxAthlete->Enabled = TRUE;
		
		// Display current active profile values in Custom Profile
		if (wsPages->uprofActive->usID == WeightScaleUserProfile::PROFILE_UNDEF)
			wsPages->uprofActive->usID = 1754;  // choose random user ID (eg. lowest two bytes of display serial number)
		this->numericUpDownProfile->Value = wsPages->uprofActive->usID;
		this->numericUpDownAge->Value = wsPages->uprofActive->ucAge;
		this->numericUpDownHeight->Value = wsPages->uprofActive->ucHeight;
		if(wsPages->uprofActive->ucGender == WeightScaleUserProfile::GENDER_FEMALE)
			this->radioButtonFemale->Checked = TRUE;
		else
			this->radioButtonMale->Checked = TRUE;
		if(wsPages->uprofActive->bAthlete == TRUE)
			this->radioButtonAthleteTrue->Checked = TRUE;
		else
			this->radioButtonAthleteFalse->Checked = TRUE;
	}
	else
	{	// Unset user profile
		WeightScaleUserProfile::copyProfile(uprofUndef, wsPages->uprofActive);
		// Disble Custom Profile
		this->numericUpDownProfile->Enabled = FALSE;
		this->numericUpDownAge->Enabled = FALSE;
		this->numericUpDownHeight->Enabled = FALSE;
		this->radioButtonFemale->Enabled = FALSE;
		this->radioButtonMale->Enabled = FALSE;
		this->numericUpDownActivityLevel->Enabled = FALSE;
		this->groupBoxAthlete->Enabled = FALSE;
	}
	UpdateDisplayActiveProfile();
}


/**************************************************************************
 * WeightScaleDisplay::buttonReset_Click
 * 
 * Reset session (without modifying user profiles)
 *
 * returns: N/A
 *
 **************************************************************************/
System::Void WeightScaleDisplay::buttonReset_Click(System::Object^  sender, System::EventArgs^  e) 
{
	// Reset session
	bSentProfile = FALSE;	// Send user profile again after a weight message from the scale
	bRxAdvanced = FALSE;
	// Clear status
	this->labelStatusProfileMatch->Text = "---";
	this->labelStatusProfileNew->Text = "---";
	this->labelStatusProfileRx->Text = "---";
	this->labelStatusProfileSent->Text = "---";
	this->labelStatusProfileUpdate->Text = "---";
	// Clear Rx data
	this->labelWeight->Text = "---";
	// Enable custom profile
	this->numericUpDownProfile->Enabled = TRUE;
	this->numericUpDownAge->Enabled = TRUE;
	this->numericUpDownHeight->Enabled = TRUE;
	this->radioButtonFemale->Enabled = TRUE;
	this->radioButtonMale->Enabled = TRUE;
	this->numericUpDownActivityLevel->Enabled = TRUE;
	this->groupBoxAthlete->Enabled = TRUE;
}


/**************************************************************************
 * WeightScaleDisplay::checkBoxAntfs_CheckedChanged
 * 
 * Enable/disable ANT-FS (not implemented yet, for future capabilities)
 *
 * returns: N/A
 *
 **************************************************************************/
System::Void WeightScaleDisplay::checkBoxAntfs_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	if(this->checkBoxAntfs->Checked)
		wsPages->capabDisplay->bAntfs = TRUE;
	else
		wsPages->capabDisplay->bAntfs = FALSE;		 
}


/**************************************************************************
 * WeightScaleDisplay::checkBoxExchange_CheckedChanged
 * 
 * Enable/disable user profile exchange
 *
 * returns: N/A
 *
 **************************************************************************/
System::Void WeightScaleDisplay::checkBoxExchange_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	if(this->checkBoxExchange->Checked)
		wsPages->capabDisplay->bDisplayExchange = TRUE;
	else
		wsPages->capabDisplay->bDisplayExchange = FALSE;
}


/**************************************************************************
 * WeightScaleDisplay::SelectCustomProfile
 * 
 * Updates custom profile set on display
 * Application specific
 *
 * returns: N/A
 *
 **************************************************************************/
void WeightScaleDisplay::SelectCustomProfile()
{
	// Read user profile from GUI
	wsPages->uprofActive->usID = (USHORT) (this->numericUpDownProfile->Value);	// ID
	wsPages->uprofActive->ucAge = (UCHAR) (this->numericUpDownAge->Value);		// Age
	wsPages->uprofActive->ucHeight = (UCHAR) (this->numericUpDownHeight->Value);	// Height
	wsPages->uprofActive->ucDescription = (UCHAR) (this->numericUpDownActivityLevel->Value);	// Activity Level
	if(this->radioButtonFemale->Checked)
		wsPages->uprofActive->ucGender = WeightScaleUserProfile::GENDER_FEMALE;	// Gender
	else if(this->radioButtonMale->Checked)
		wsPages->uprofActive->ucGender = WeightScaleUserProfile::GENDER_MALE;
	else
		wsPages->uprofActive->ucGender = WeightScaleUserProfile::GENDER_UNDEF;
	if (this->radioButtonAthleteTrue->Checked == TRUE)							// Lifetime activity
		wsPages->uprofActive->bAthlete = TRUE;
	else
		wsPages->uprofActive->bAthlete = FALSE;

	UpdateDisplayActiveProfile();
}
