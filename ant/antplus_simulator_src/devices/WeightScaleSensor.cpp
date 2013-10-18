/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/



#include "StdAfx.h"
#include "WeightScaleSensor.h"
#include "antmessage.h"


/**************************************************************************
 * WeightScaleSensor::ANT_eventNotification
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
void WeightScaleSensor::ANT_eventNotification(UCHAR ucEventCode_, UCHAR* pucEventBuffer_)
{
	switch(ucEventCode_)
	{
		case EVENT_TX:
			HandleTransmit((UCHAR*) pucEventBuffer_);
			break;
		case EVENT_RX_BROADCAST:
			HandleReceive((UCHAR*) pucEventBuffer_);
			break;	
		case MESG_OPEN_CHANNEL_ID:
			AllowChangesProfile(FALSE);
			break;
		case MESG_CLOSE_CHANNEL_ID:
			AllowChangesProfile(TRUE);
			break;
		default:
			break;
	}
}


/**************************************************************************
 * WeightScaleSensor::InitializeSim
 * 
 * Initializes simulator variables
 * 
 * returns: N/A
 *
 **************************************************************************/
void WeightScaleSensor::InitializeSim()
{
	// Initialization of ANT+ simulation variables
	wsPages->usBodyWeight100 = WeightScale::WEIGHT_COMPUTING;
	wsPages->usHydrationP100 = (USHORT) (this->numericUpDown_Hydration->Value * 100);	// Hydration in 0.01%
	wsPages->usBodyFatP100 = (USHORT) (this->numericUpDown_BodyFat->Value * 100);	// Body fat in 0.01%
	wsPages->usActiveMetRate4 = (USHORT) (this->numericUpDown_ActiveMetRate->Value * 4);	// Active metabolic rate (1/4 kcal)
	wsPages->usBasalMetRate4= (USHORT) (this->numericUpDown_BasalMetRate->Value * 4); // Basal metabolic rate (1/4 kcal)
   wsPages->usMuscleMass100 = (USHORT) (this->numericUpDown_MuscleMass->Value * 100);  // Muscle mass (1/100 kg)
   wsPages->ucBoneMass10 = (UCHAR) (this->numericUpDown_BoneMass->Value * 10);   // Bone mass (1/10 kg)


	// Common Pages (Read from UI values)
	commonPages->usMfgID = System::Convert::ToUInt16(this->textboxMfgID->Text);
	commonPages->usModelNum = System::Convert::ToUInt16(this->textboxModelNum->Text);
	commonPages->ucHwVersion= System::Convert::ToByte(this->textboxHwVer->Text);
	commonPages->ucSwVersion = System::Convert::ToByte(this->textboxSwVer->Text);
	commonPages->ulSerialNum = System::Convert::ToUInt32(this->textboxSerialNum->Text);

	// Preset User Profiles
	uprofUndef = gcnew WeightScaleUserProfile();
	uprofU1 = gcnew WeightScaleUserProfile((USHORT) 1, WeightScaleUserProfile::GENDER_MALE, (UCHAR) 47, (UCHAR) 170, WeightScaleUserProfile::ACTIVITY_LOW, WeightScaleUserProfile::PRIORITY_WATCH, WeightScaleUserProfile::NOT_ATHLETE);
	uprofU2 = gcnew WeightScaleUserProfile((USHORT) 5, WeightScaleUserProfile::GENDER_FEMALE, (UCHAR) 25, (UCHAR) 162, WeightScaleUserProfile::ACTIVITY_HIGH, WeightScaleUserProfile::PRIORITY_WATCH, WeightScaleUserProfile::IS_ATHLETE);
	uprofU3 = gcnew WeightScaleUserProfile((USHORT) 12, WeightScaleUserProfile::GENDER_MALE, (UCHAR) 8, (UCHAR) 117, WeightScaleUserProfile::ACTIVITY_CHILD, WeightScaleUserProfile::PRIORITY_WATCH, WeightScaleUserProfile::NOT_ATHLETE);
	
	// Status
	bTxAdvanced = FALSE;
	bTxUserProfile = FALSE;
	bTxPage2 = TRUE;
	bTxPage3 = TRUE;
   bTxPage4 = TRUE;
}


/**************************************************************************
 * WeightScaleSensor::HandleTransmit
 * 
 * Encode data into Tx Buffer
 * pucTxBuffer_: pointer to the Tx buffer
 *
 * returns: N/A
 *
 **************************************************************************/
void WeightScaleSensor::HandleTransmit(UCHAR* pucTxBuffer_)
{
	static UCHAR ucMsgCount = 0;
	UCHAR ucPageNum = WeightScale::PAGE_BODY_WEIGHT;

	if(bTxAdvanced && wsPages->usBodyWeight100 < WeightScale::WEIGHT_COMPUTING)	// Make sure profile has been enabled, and body weight is calculated
	{
		if(ucMsgCount % 4 == 1 && bTxAdvanced && bTxPage2)		// Interleaving advanced data pages
			ucPageNum = WeightScale::PAGE_BODY_COMPOSITION;
		if(ucMsgCount % 4 == 2 && bTxAdvanced && bTxPage3)
			ucPageNum = WeightScale::PAGE_METABOLIC_INFO;
      if(ucMsgCount % 4 == 3 && bTxAdvanced && bTxPage4)
         ucPageNum = WeightScale::PAGE_BODY_MASS;
	}
	if(ucMsgCount == (WeightScale::INTERVAL_COMMON-1))		// Interleaving common pages
		ucPageNum = CommonData::PAGE80;
	if(++ucMsgCount >= (WeightScale::INTERVAL_COMMON*2))
	{
		ucPageNum = CommonData::PAGE81;
		ucMsgCount = 0;
	}

	if(bTxUserProfile)			
	{
		ucPageNum = WeightScale::PAGE_USER_PROFILE;		// Transmit user profile 
		bTxUserProfile = FALSE;							// Only once
	}

	switch(ucPageNum)
	{
		case WeightScale::PAGE_BODY_WEIGHT:
		case WeightScale::PAGE_BODY_COMPOSITION:
		case WeightScale::PAGE_METABOLIC_INFO:
      case WeightScale::PAGE_BODY_MASS:
		case WeightScale::PAGE_USER_PROFILE:			// Intentional fall thru
			try
			{
				wsPages->Encode(ucPageNum, pucTxBuffer_);	// Encode weight scale pages
			}
			catch(WeightScale::Error^ errorWeightScale)
			{
			}
			break;		
		case CommonData::PAGE80:
		case CommonData::PAGE81:						// Intentional fall thru
			try
			{
				commonPages->Encode(ucPageNum, pucTxBuffer_);	// Encode common pages
			}
			catch(CommonData::Error^ errorCommon)
			{
			}
			break;
		default:
			break;
	}

	UpdateDisplay();
}


/**************************************************************************
 * WeightScaleSensor::HandleReceive
 * 
 * Decode incoming transmissions
 *
 * returns: N/A
 *
 **************************************************************************/
void WeightScaleSensor::HandleReceive(UCHAR* pucRxBuffer_)
{
	UCHAR ucPageNum = pucRxBuffer_[0];

	if(ucPageNum == WeightScale::PAGE_USER_PROFILE)	// Only decode user profile
	{
		try
			{
				wsPages->Decode(pucRxBuffer_);
			}
			catch(WeightScale::Error^ errorWeightScale)
			{
			}

		UpdateDisplayCapabilities();
		ProcessReceivedProfile();		// Process incoming profile
	}
}


/**************************************************************************
 * WeightScaleSensor::ProcessReceivedProfile
 * 
 * Process incoming profile (device specific decision tree)
 *
 * returns: N/A
 *
 **************************************************************************/
void WeightScaleSensor::ProcessReceivedProfile()
{

	this->labelStatusProfileRx->Text = L"User Profile Received";

	// Process capabilities
	wsPages->capabScale->bDisplayExchange = wsPages->capabDisplay->bDisplayExchange; // Update display exchange support field as received from display

	// Does display support profile exchange?
	if(wsPages->capabDisplay->bDisplayExchange)
	{
		// Is user profile ID received from display valid?
		if(wsPages->uprofDisplay->usID != WeightScaleUserProfile::PROFILE_UNDEF)
		{
			// User profile ID match?
			// This implementation only looks for a match in the active profile of the scale
			// It could also be possible to look for a match in all profiles stored on the scale
			if(wsPages->uprofActive->usID == wsPages->uprofDisplay->usID)
			{
				this->labelStatusProfileMatch->Text = L"User Profile Match";		// Profile ID match
				// Check for changes to user profile for this ID
				if(!(WeightScaleUserProfile::isEqual(wsPages->uprofActive, wsPages->uprofDisplay)))
					this->labelStatusProfileUpdate->Text = L"User Profile Updated";	// Changes detected, profile will be updated
			}
			else
			{
				this->labelStatusProfileNew->Text = L"New User Profile";		// New user profile
			}
			// Apply user profile to calculations
			WeightScaleUserProfile::copyProfile(wsPages->uprofDisplay, wsPages->uprofActive);
			UpdateDisplayActiveProfile();
			wsPages->capabScale->bScaleSet = TRUE;
			bTxAdvanced = TRUE;			// Enable advanced calculations
		}
		else
		{
			// No valid user profile in display
			// Is user profile set on scale?
			// In this implementation, the scale only looks for a valid active user profile
			if(wsPages->uprofActive->usID != WeightScaleUserProfile::PROFILE_UNDEF)
			{
				// Transmit User Profile
				bTxUserProfile = TRUE;
				this->labelStatusProfileSent->Text = L"Sent User Profile";
				// Apply profile to calculations
				bTxAdvanced = TRUE;
			}
		}
	}
}


/**************************************************************************
 * WeightScaleSensor::UpdateDisplay
 * 
 * Updates display of current transmission
 *
 * returns: N/A
 *
 **************************************************************************/
void WeightScaleSensor::UpdateDisplay()
{
	// Update display
	if(wsPages->usBodyWeight100 == WeightScale::WEIGHT_COMPUTING)
		this->labelWeight->Text = "Comp";
	else if (wsPages->usBodyWeight100 == WeightScale::WEIGHT_INVALID)
		this->labelWeight->Text = "Invalid";
	else
		this->labelWeight->Text = System::Convert::ToString((double) wsPages->usBodyWeight100 / 100);	// Weight (kg)

	this->labelProfileActive->Text = Convert::ToString(wsPages->uprofActive->usID);		// Active profile


	if(bTxPage2 && bTxAdvanced && wsPages->usBodyWeight100 < WeightScale::WEIGHT_COMPUTING)
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


	if(bTxPage3 && bTxAdvanced && wsPages->usBodyWeight100 < WeightScale::WEIGHT_COMPUTING)
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


}


/**************************************************************************
 * WeightScaleSensor::UpdateDisplayActiveProfile
 * 
 * Updates display of current active user profile
 *
 * returns: N/A
 *
 **************************************************************************/
void WeightScaleSensor::UpdateDisplayActiveProfile()
{
	// Updates field on active profile tab
	this->labelProfileActive->Text = System::Convert::ToString(wsPages->uprofActive->usID);		// Active profile ID
	this->labelProfileActive2->Text = System::Convert::ToString(wsPages->uprofActive->usID);
	if(wsPages->uprofActive->ucGender == WeightScaleUserProfile::GENDER_FEMALE)				// Gender
		this->labelGenderActive->Text = L"Female";
	if(wsPages->uprofActive->ucGender == WeightScaleUserProfile::GENDER_MALE)
		this->labelGenderActive->Text = L"Male";
	this->labelAgeActive->Text = System::Convert::ToString(wsPages->uprofActive->ucAge);	// Age
	this->labelHeightActive->Text = System::Convert::ToString(wsPages->uprofActive->ucHeight);	// Height
	this->labelDescriptionActive->Text = System::Convert::ToString(wsPages->uprofActive->ucDescription);	// Description
	if (wsPages->uprofActive->bAthlete == TRUE)	 // Athlete bit
		this->labelLifestyle->Text = "Athlete";
	else
		this->labelLifestyle->Text = "Standard";

	// Update fields on custom tab
	this->numericUpDownActivityLevel->Value = wsPages->uprofActive->ucDescription;	// Description
	this->numericUpDownAge->Value = wsPages->uprofActive->ucAge;					// Age
	this->numericUpDownHeight->Value = wsPages->uprofActive->ucHeight;				// Height
	if (wsPages->uprofActive->ucGender == WeightScaleUserProfile::GENDER_MALE)		// Gender
		this->radioButtonMale->Checked = TRUE;
	else
		this->radioButtonFemale->Checked = TRUE;
	if (wsPages->uprofActive->usID <= WeightScaleUserProfile::PROFILE_SCALE_MAX)	// ID
		this->numericUpDownProfile->Value = wsPages->uprofActive->usID;
	else
		this->numericUpDownProfile->Value = 0;

	if(wsPages->uprofActive->bAthlete == TRUE)  // Athlete bit
		this->radioButtonLifetimeActivityTrue->Checked = TRUE;
	else
		this->radioButtonLifetimeActivityFalse->Checked = TRUE;
}


/**************************************************************************
 * WeightScaleSensor::UpdateDisplayCapabilities
 * 
 * Updates display of capabilities (received from display)
 *
 * returns: N/A
 *
 **************************************************************************/
void WeightScaleSensor::UpdateDisplayCapabilities()
{
	// Show capabilities of display
	if(wsPages->capabDisplay->bScaleSet)
		this->labelProfileSetScale->Text = L"Yes";
	else
		this->labelProfileSetScale->Text = L"No";

	if(wsPages->capabDisplay->bScaleExchange)
		this->labelExchangeScale->Text = L"Yes";
	else
		this->labelExchangeScale->Text = L"No";

	if(wsPages->capabDisplay->bAntfs)
		this->labelAntfsRx->Text = L"Yes";
	else
		this->labelAntfsRx->Text = L"No";	

	if(wsPages->capabDisplay->bDisplayExchange)
		this->labelExchangeDisplay->Text = L"Yes";
	else
		this->labelExchangeDisplay->Text = L"No";
}



/**************************************************************************
 * WeightScaleSensor::SelectPresetProfile
 * 
 * Selects between preset user profiles
 * Application specific
 *
 * returns: N/A
 *
 **************************************************************************/
void WeightScaleSensor::SelectPresetProfile()
{
	if(this->radioButtonUser1->Checked)			// Select among predefined profiles
		WeightScaleUserProfile::copyProfile(uprofU1, wsPages->uprofScale);
	if(this->radioButtonUser2->Checked)
		WeightScaleUserProfile::copyProfile(uprofU2, wsPages->uprofScale);
	if(this->radioButtonUser3->Checked)
		WeightScaleUserProfile::copyProfile(uprofU3, wsPages->uprofScale);
	if(this->radioButtonGuest->Checked)
	{
		// Enable Custom Profile
		this->numericUpDownProfile->Enabled = TRUE;
		this->numericUpDownAge->Enabled = TRUE;
		this->numericUpDownHeight->Enabled = TRUE;
		this->radioButtonFemale->Enabled = TRUE;
		this->radioButtonMale->Enabled = TRUE;
		this->numericUpDownActivityLevel->Enabled = TRUE;
		this->groupBoxLifetimeActivity->Enabled = TRUE;
		SelectCustomProfile();
	}
	else
	{
		// Disble Custom Profile
		this->numericUpDownProfile->Enabled = FALSE;
		this->numericUpDownAge->Enabled = FALSE;
		this->numericUpDownHeight->Enabled = FALSE;
		this->radioButtonFemale->Enabled = FALSE;
		this->radioButtonMale->Enabled = FALSE;
		this->numericUpDownActivityLevel->Enabled = FALSE;
		this->groupBoxLifetimeActivity->Enabled = FALSE;
	}

	// If there is no active profile, or current active profile was set on a scale, set scale profile as active
	if((wsPages->uprofActive->usID == WeightScaleUserProfile::PROFILE_UNDEF) || (wsPages->uprofActive->usID <= WeightScaleUserProfile::PROFILE_SCALE_MAX))
	{
		WeightScaleUserProfile::copyProfile(wsPages->uprofScale, wsPages->uprofActive);
		UpdateDisplayActiveProfile();
	}
}

/**************************************************************************
 * WeightScaleSensor::SelectCustomProfile
 * 
 * Updates custom profile set on scale
 * Application specific
 *
 * returns: N/A
 *
 **************************************************************************/
void WeightScaleSensor::SelectCustomProfile()
{
	if(wsPages->capabScale->bScaleSet && this->radioButtonGuest->Checked)	// User profile selected through interface and custom profile enabled
	{
		// Read user profile from GUI
		wsPages->uprofScale->usID = (USHORT) (this->numericUpDownProfile->Value);	// ID
		wsPages->uprofScale->ucAge = (UCHAR) (this->numericUpDownAge->Value);		// Age
		wsPages->uprofScale->ucHeight = (UCHAR) (this->numericUpDownHeight->Value);	// Height
		wsPages->uprofScale->ucDescription = (UCHAR) (this->numericUpDownActivityLevel->Value);	// Activity Level
		if(this->radioButtonFemale->Checked)
			wsPages->uprofScale->ucGender = WeightScaleUserProfile::GENDER_FEMALE;	// Gender
		else if(this->radioButtonMale->Checked)
			wsPages->uprofScale->ucGender = WeightScaleUserProfile::GENDER_MALE;
		else
			wsPages->uprofScale->ucGender = WeightScaleUserProfile::GENDER_UNDEF;
		if (this->radioButtonLifetimeActivityTrue->Checked == TRUE)
			wsPages->uprofScale->bAthlete = TRUE;									// Lifetime activity
		else
			wsPages->uprofScale->bAthlete = FALSE;

	}

	// Set scale profile as active
	// - if there is no active profile
	// - if updating current active profile (ID match)
	// - if current active profile was set on a scale
	if((wsPages->uprofActive->usID == WeightScaleUserProfile::PROFILE_UNDEF) || (wsPages->uprofActive->usID == wsPages->uprofScale->usID) || (wsPages->uprofActive->usID <= WeightScaleUserProfile::PROFILE_SCALE_MAX))
	{
		WeightScaleUserProfile::copyProfile(wsPages->uprofScale, wsPages->uprofActive);
		UpdateDisplayActiveProfile();
	}
}


/**************************************************************************
 * WeightScaleSensor::checkBoxProfileSet_CheckedChanged
 * 
 * Selects user profile through the interface of the weight scale
 *
 * returns: N/A
 *
 **************************************************************************/
System::Void WeightScaleSensor::checkBoxProfileSet_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	if(this->checkBoxProfileSet->Checked == TRUE)	// Profile set
	{
		this->groupBox2->Enabled = TRUE;
		SelectPresetProfile();						// Use profile selected
		// The selected profile is only applied if there is no active profile
		wsPages->capabScale->bScaleSet = TRUE;		// Set capabilities		
	}
	else
	{
		this->groupBox2->Enabled = FALSE;			// Profile not set
		WeightScaleUserProfile::copyProfile(uprofUndef, wsPages->uprofScale);	// Invalid profile
		// Set the profile as active if current profile was set on the scale
		if(wsPages->uprofActive->usID <= WeightScaleUserProfile::PROFILE_SCALE_MAX)
		{
			WeightScaleUserProfile::copyProfile(wsPages->uprofScale, wsPages->uprofActive);
			UpdateDisplayActiveProfile();
		}
		wsPages->capabScale->bScaleSet = FALSE;		// Set capabilities
		// This only affects the profile selected at the scale, it will not delete the current active profile
	}
}


/**************************************************************************
 * WeightScaleSensor::checkBoxExchange_CheckedChanged
 * 
 * Enable/disable profile exchange
 *
 * returns: N/A
 *
 **************************************************************************/
System::Void WeightScaleSensor::checkBoxExchange_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(this->checkBoxExchange->Checked)
		wsPages->capabScale->bScaleExchange = TRUE;
	else
		wsPages->capabScale->bScaleExchange = FALSE;
		 
}

/**************************************************************************
 * WeightScaleSensor::checkBoxAntfs_CheckedChanged
 * 
 * Enable/disable use of ANT-FS (future functionality)
 * ANT-FS usage on weight scale is not yet defined on the device profile
 *
 * returns: N/A
 *
 **************************************************************************/
System::Void WeightScaleSensor::checkBoxAntfs_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if(this->checkBoxAntfs->Checked)
		wsPages->capabScale->bAntfs = TRUE;
	else
		wsPages->capabScale->bAntfs = FALSE;
		 
}

/**************************************************************************
 * WeightScaleSensor::SelectWeightStatus
 * 
 * Sets weight as valid, invalid or computing
 *
 * returns: N/A
 *
 **************************************************************************/
void WeightScaleSensor::SelectWeightStatus()
{
	if (this->radioButtonWeightValid->Checked)	// Valid weight
	{
		this->numericUpDownWeight->Enabled = TRUE;
		wsPages->usBodyWeight100 = (USHORT) (this->numericUpDownWeight->Value * 100);
	}
	else
	{
		this->numericUpDownWeight->Enabled = FALSE;
		this->label_Hydration->Text = "---";
		this->label_BodyFat->Text = "---";
		this->label_ActiveMetRate->Text = "---";
		this->label_BasalMetRate->Text = "---";
		if (this->radioButtonWieghtComputing->Checked == TRUE)
		{
			wsPages->usBodyWeight100 = WeightScale::WEIGHT_COMPUTING;
		}
		if (this->radioButtonWeightError->Checked == TRUE)
		{
			wsPages->usBodyWeight100 = WeightScale::WEIGHT_INVALID;
		}
	}
}


/**************************************************************************
 * WeightScaleSensor::SelectPages
 * 
 * Selects advanced pages to transmit
 *
 * returns: N/A
 *
 **************************************************************************/
void WeightScaleSensor::SelectPages()
{
	bTxPage2 = (BOOL) (this->checkBox_TxPage2->Checked);
	bTxPage3 = (BOOL) (this->checkBox_TxPage3->Checked);
   bTxPage4 = (BOOL) (this->checkBox_TxPage4->Checked);
}

/**************************************************************************
 * WeightScaleSensor::buttonUpdateGlobal_Click
 * 
 * Updates and validates global data
 *
 * returns: N/A
 *
 **************************************************************************/
System::Void WeightScaleSensor::buttonUpdateGlobal_Click(System::Object^  sender, System::EventArgs^  e)
{
	this->labelError->Visible = false;
	try
	{
		commonPages->usMfgID = System::Convert::ToUInt16(this->textboxMfgID->Text);
		commonPages->usModelNum = System::Convert::ToUInt16(this->textboxModelNum->Text);
		commonPages->ucHwVersion = System::Convert::ToByte(this->textboxHwVer->Text);
		commonPages->ucSwVersion = System::Convert::ToByte(this->textboxSwVer->Text);
		if(!this->checkBox_InvalidSerial->Checked)
			commonPages->ulSerialNum = System::Convert::ToUInt32(this->textboxSerialNum->Text);
		else
			commonPages->ulSerialNum = 0xFFFFFFFF;
	}
	catch(...)
	{
		this->labelError->Visible = true;
		this->labelError->Text = L"Invalid input";
	}
		 
}

/**************************************************************************
 * WeightScaleSensor::buttonReset_Click
 * 
 * Resets session, without changing user profiles
 *
 * returns: N/A
 *
 **************************************************************************/
System::Void WeightScaleSensor::buttonReset_Click(System::Object^  sender, System::EventArgs^  e)
{
	// Reset session
	bTxAdvanced = FALSE;
	bTxUserProfile = FALSE;
	// Set weight to calculating
	radioButtonWieghtComputing->Checked = TRUE;
	// Clear status
	this->labelStatusProfileMatch->Text= "---";
	this->labelStatusProfileNew->Text = "---";
	this->labelStatusProfileRx->Text = "---";
	this->labelStatusProfileSent->Text = "---";
	this->labelStatusProfileUpdate->Text = "---";

}

/**************************************************************************
 * WeightScaleSensor::AllowChangesProfile
 * 
 * Enables/disables changes to user profile
 *
 * returns: N/A
 *
 **************************************************************************/

void WeightScaleSensor::AllowChangesProfile(BOOL bNoSession_)
{
	if(bNoSession_)
	{
		this->checkBoxProfileSet->Enabled = true;
		this->radioButtonUser1->Enabled = true;
		this->radioButtonUser2->Enabled = true;
		this->radioButtonGuest->Enabled = true;
		this->radioButtonGuest->Enabled = true;
		if(this->radioButtonGuest->Checked)	// Enable custom profiles
		{
			this->numericUpDownProfile->Enabled = true;
			this->numericUpDownAge->Enabled = true;
			this->numericUpDownHeight->Enabled = true;
			this->radioButtonFemale->Enabled = true;
			this->radioButtonMale->Enabled = true;
			this->numericUpDownActivityLevel->Enabled = true;
			this->groupBoxLifetimeActivity->Enabled = true;
		}
	}
	else
	{
		this->checkBoxProfileSet->Enabled = false;
		this->radioButtonUser1->Enabled = false;
		this->radioButtonUser2->Enabled = false;
		this->radioButtonGuest->Enabled = false;
		this->radioButtonGuest->Enabled = false;
		this->numericUpDownProfile->Enabled = false;
		this->numericUpDownAge->Enabled = false;
		this->numericUpDownHeight->Enabled = false;
		this->radioButtonFemale->Enabled = false;
		this->radioButtonMale->Enabled = false;
		this->numericUpDownActivityLevel->Enabled = false;
		this->groupBoxLifetimeActivity->Enabled = false;
	}
}



/**************************************************************************
 * Functions to update input from numeric boxes:
 * Weight, Hydration, Body Fat, Active Metabolic Rate, Basal Metabolic Rate
 * Validation of input is done by the control
 *
 **************************************************************************/
System::Void WeightScaleSensor::numericUpDownWeight_ValueChanged(System::Object^  sender, System::EventArgs^  e) 
{
	wsPages->usBodyWeight100 = (USHORT) (this->numericUpDownWeight->Value * 100);  // Weight in 1/100 kg
}

System::Void WeightScaleSensor::numericUpDown_Hydration_ValueChanged(System::Object^  sender, System::EventArgs^  e) 
{
	wsPages->usHydrationP100 = (USHORT) (this->numericUpDown_Hydration->Value * 100);	// Hydration in 0.01%
}

System::Void WeightScaleSensor::numericUpDown_BodyFat_ValueChanged(System::Object^  sender, System::EventArgs^  e) 
{
	wsPages->usBodyFatP100 = (USHORT) (this->numericUpDown_BodyFat->Value * 100);	// Body fat in 0.01%
}

System::Void WeightScaleSensor::numericUpDown_ActiveMetRate_ValueChanged(System::Object^  sender, System::EventArgs^  e) 
{
	wsPages->usActiveMetRate4 = (USHORT) (this->numericUpDown_ActiveMetRate->Value * 4);	// Active metabolic rate (1/4 kcal)
}

System::Void WeightScaleSensor::numericUpDown_BasalMetRate_ValueChanged(System::Object^  sender, System::EventArgs^  e) 
{
	wsPages->usBasalMetRate4 = (USHORT) (this->numericUpDown_BasalMetRate->Value * 4); // Basal metabolic rate (1/4 kcal)
}

System::Void WeightScaleSensor::numericUpDown_MuscleMass_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
   wsPages->usMuscleMass100 = (USHORT) (this->numericUpDown_MuscleMass->Value * 100);  // Muscle mass (1/100 kg)
}

System::Void WeightScaleSensor::numericUpDown_BoneMass_ValueChanged(System::Object^  sender, System::EventArgs^  e)
{
   wsPages->ucBoneMass10 = (UCHAR) (this->numericUpDown_BoneMass->Value * 10);   // Bone mass (1/10 kg)
}

System::Void WeightScaleSensor::checkBox_InvalidSerial_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
{
	if(this->checkBox_InvalidSerial->Checked)
		this->textboxSerialNum->Enabled = false;
	else
		this->textboxSerialNum->Enabled = true;
}
