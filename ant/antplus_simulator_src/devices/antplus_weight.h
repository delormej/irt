/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/



#pragma once

#define WEIGHT_SCALE_REV				1.5.0


// User Profile Class
public ref class WeightScaleUserProfile
{
public:
	static const USHORT PROFILE_UNDEF = 0xFFFF;
	static const USHORT PROFILE_SCALE_MIN = 0x0000;
	static const USHORT PROFILE_SCALE_MAX = 0x000F;
	static const USHORT PROFILE_DISPLAY_STAT_MIN = 0x0010;
	static const USHORT PROFILE_DISPLAY_STAT_MAX = 0x00FF;
	static const USHORT PROFILE_DISPLAY_PERS_MIN = 0x0100;
	static const USHORT PROFILE_DISPLAY_PERS_MAX = 0xFFFE;
	static const USHORT PROFILE_DISPLAY_MIN = 0x0010;
	static const USHORT PROFILE_DISPLAY_MAX = 0xFFFE;

	static const UCHAR GENDER_SHIFT = 7;
	static const UCHAR GENDER_FEMALE = 0x00;
	static const UCHAR GENDER_MALE = 0x01;
	static const UCHAR GENDER_UNDEF = 0x00;
	static const UCHAR AGE_MASK = 0x7F;
	static const UCHAR AGE_UNDEF = 0x00;
	static const UCHAR HEIGHT_UNDEF = 0x00;
	static const BOOL IS_ATHLETE = TRUE;
	static const BOOL NOT_ATHLETE = FALSE;

	// Descriptive Bit Field
	static const UCHAR DESCR_RESERVED = 0x00;
   static const UCHAR DESCR_MASK = 0x07;
   static const UCHAR ACTIVITY_ATHLETE_MASK = 0x080;
   static const UCHAR ACTIVITY_ATHLETE_SHIFT = 7;
	static const UCHAR ACTIVITY_SHIFT = 0;
	static const UCHAR ACTIVITY_INVALID = ((UCHAR) 0x00 << ACTIVITY_SHIFT);
	static const UCHAR ACTIVITY_CHILD	= ((UCHAR) 0x01 << ACTIVITY_SHIFT);
	static const UCHAR ACTIVITY_SEDENTARY = ((UCHAR) 0x02 << ACTIVITY_SHIFT);
	static const UCHAR ACTIVITY_LOW = ((UCHAR) 0x03 << ACTIVITY_SHIFT);
	static const UCHAR ACTIVITY_MEDIUM = ((UCHAR) 0x04 << ACTIVITY_SHIFT);
	static const UCHAR ACTIVITY_HIGH = ((UCHAR) 0x05 << ACTIVITY_SHIFT);
	static const UCHAR ACTIVITY_INTENSE	= ((UCHAR) 0x06 << ACTIVITY_SHIFT);
	static const UCHAR ACTIVITY_RESERVED = ((UCHAR) 0x07 << ACTIVITY_SHIFT);

	// Priorities
	static const UCHAR PRIORITY_SCALE = 1;
	static const UCHAR PRIORITY_DISPLAY = 2;
	static const UCHAR PRIORITY_WATCH = 3;
	static const UCHAR PRIORITY_UNDEF = 0;

public:
	USHORT usID;
	UCHAR	ucGender;
	UCHAR	ucAge;
	UCHAR	ucHeight;
	UCHAR	ucDescription;
	UCHAR	ucPriority;
	BOOL bAthlete;

	WeightScaleUserProfile()
	{
		usID = PROFILE_UNDEF;
		ucGender = GENDER_UNDEF;
		ucAge = AGE_UNDEF;
		ucHeight = HEIGHT_UNDEF;
		ucDescription = ACTIVITY_INVALID;
		bAthlete = NOT_ATHLETE;
		this->SetPriority();
	}

	WeightScaleUserProfile(USHORT usID_, UCHAR ucGender_, UCHAR ucAge_, UCHAR ucHeight_, UCHAR ucDescription_, UCHAR ucPriority_, BOOL bAthlete_)
	{
		usID = usID_;
		ucGender = ucGender_;
		ucAge = ucAge_;
		ucHeight = ucHeight_;
		ucDescription = ucDescription_;
		ucPriority = ucPriority_;
		bAthlete = bAthlete_;
	}

	~WeightScaleUserProfile()
	{
		//delete this;
	}

	static void copyProfile(WeightScaleUserProfile^ Src, WeightScaleUserProfile^ Dst)
	{
		Dst->usID = Src->usID;
		Dst->ucAge = Src->ucAge;
		Dst->ucGender = Src->ucGender;
		Dst->ucHeight = Src->ucHeight;
		Dst->ucDescription = Src->ucDescription;
		Dst->ucPriority = Src->ucPriority;
		Dst->bAthlete = Src->bAthlete;
	}

	static BOOL isEqual(WeightScaleUserProfile^ Src, WeightScaleUserProfile^ Dst)
	{
		BOOL bEqual = TRUE;
		if(Dst->usID != Src->usID)
			bEqual = FALSE;
		if(Dst->ucAge != Src->ucAge)
			bEqual = FALSE;
		if(Dst->ucGender != Src->ucGender)
			bEqual = FALSE;
		if(Dst->ucHeight != Src->ucHeight)
			bEqual = FALSE;
		if(Dst->ucDescription != Src->ucDescription)
			bEqual = FALSE;
		if(Dst->bAthlete != Src->bAthlete)
			bEqual = FALSE;
		return bEqual;

	}

	void SetDefaultValues()
	{
		usID = PROFILE_UNDEF;
		ucGender = GENDER_UNDEF;
		ucAge = AGE_UNDEF;
		ucHeight = HEIGHT_UNDEF;
		ucDescription = ACTIVITY_INVALID;
		ucPriority = PRIORITY_SCALE;
		bAthlete = NOT_ATHLETE;
		this->SetPriority();
	}

	void SetPriority()
	{
		if(usID == PROFILE_UNDEF)
			ucPriority = PRIORITY_UNDEF;
		else if(usID <= PROFILE_SCALE_MAX)
			ucPriority = PRIORITY_SCALE;
		else if(usID <= PROFILE_DISPLAY_STAT_MAX)
			ucPriority = PRIORITY_DISPLAY;
		else if(usID <= PROFILE_DISPLAY_PERS_MAX)
			ucPriority = PRIORITY_WATCH;
	}

};


// Capabilities Class
public ref class WeightScaleCapabilities
{

public:
	static const UCHAR RESERVED	= 0x00;

	static const UCHAR SCALE_SELECTED_SHIFT = 0;	// bit 0
	static const UCHAR SCALE_SELECTED = ((UCHAR) 0x01 << SCALE_SELECTED_SHIFT);
	static const UCHAR SCALE_UNSELECTED = ((UCHAR) 0x00 << SCALE_SELECTED_SHIFT);
	static const UCHAR SCALE_SELECTED_MASK = 0x01;

	static const UCHAR SCALE_EXCHANGE_SHIFT = 1;	// bit 1
	static const UCHAR SCALE_EXCHANGE = ((UCHAR) 0x01 << SCALE_EXCHANGE_SHIFT);
	static const UCHAR SCALE_NO_EXCHANGE = ((UCHAR) 0x00 << SCALE_EXCHANGE_SHIFT);
	static const UCHAR SCALE_EXCHANGE_MASK = 0x02;

	static const UCHAR ANTFS_SHIFT = 2;				// bit 2
	static const UCHAR ANTFS = ((UCHAR) 0x01 << ANTFS_SHIFT);
	static const UCHAR NO_ANTFS = ((UCHAR) 0x00 << ANTFS_SHIFT);
	static const UCHAR ANTFS_MASK = 0x04;

	static const UCHAR DISPLAY_EXCHANGE_SHIFT = 7;		// bit 7
	static const UCHAR DISPLAY_EXCHANGE = ((UCHAR) 0x00 << DISPLAY_EXCHANGE_SHIFT);	
	static const UCHAR DISPLAY_NO_EXCHANGE = ((UCHAR) 0x01 << DISPLAY_EXCHANGE_SHIFT);
	static const UCHAR DISPLAY_EXCHANGE_MASK = 0x80;	

	enum class DeviceClass : UCHAR
	{
		SIMPLE_SCALE,
		ADVANCED_SCALE,
		SIMPLE_DISPLAY,
		ADVANCED_DISPLAY
	};

public:
	BOOL	bScaleSet;
	BOOL	bAntfs;
	BOOL	bScaleExchange;
	BOOL	bDisplayExchange;
	UCHAR	ucCapabilities;

	WeightScaleCapabilities(DeviceClass eType)
	{
		SetDefault(eType);
	}

	~WeightScaleCapabilities()
	{
		//delete this;
	}

	// Default settings
	void SetDefault(DeviceClass eType)
	{
		bScaleSet = FALSE;			// No user profile set initially
		bAntfs = FALSE;				// Future capability, currently unsupported

		switch(eType)
		{
			default:						// Treat any invalid config as a simple scale
			case DeviceClass::SIMPLE_SCALE:
				bScaleExchange = FALSE;		// Simple scale does not support profile exchange
				bDisplayExchange = FALSE;	// Initially, we do not know if the display supports profile exchange
				break;
			case DeviceClass::ADVANCED_SCALE:
				bScaleExchange = TRUE;		// Simple scale does not support profile exchange
				bDisplayExchange = FALSE;	// Initially, we do not know if the display supports profile exchange
				break;
			case DeviceClass::SIMPLE_DISPLAY:
				bScaleExchange = FALSE;		// Initially, we do not know if the scale supports profile exchange
				bDisplayExchange = FALSE;	// Simple display does not support profile exchange
				break;
			case DeviceClass::ADVANCED_DISPLAY:
				bScaleExchange = FALSE;		// Initially, we do not know if the scale supports profile exchange
				bDisplayExchange = TRUE;	// Advanced display supports profile exchange
				break;
		}
		encode(this);
	}

	static void encode(WeightScaleCapabilities^ capab)
	{
		capab->ucCapabilities = ((UCHAR) capab->bScaleSet << SCALE_SELECTED_SHIFT);
		capab->ucCapabilities |= ((UCHAR) capab->bScaleExchange << SCALE_EXCHANGE_SHIFT);
		capab->ucCapabilities |= ((UCHAR) capab->bAntfs << ANTFS_SHIFT);
		capab->ucCapabilities |= ((UCHAR) !(capab->bDisplayExchange) << DISPLAY_EXCHANGE_SHIFT);   // Note support for profile exchange in the display is indicated by 0!
	}

	static void decode(WeightScaleCapabilities^ capab)
	{
		capab->bScaleSet = (BOOL) ((capab->ucCapabilities & SCALE_SELECTED_MASK) >> SCALE_SELECTED_SHIFT);
		capab->bScaleExchange = (BOOL) ((capab->ucCapabilities & SCALE_EXCHANGE_MASK) >> SCALE_EXCHANGE_SHIFT);
		capab->bAntfs = (BOOL) ((capab->ucCapabilities & ANTFS_MASK) >> ANTFS_SHIFT);
		capab->bDisplayExchange = (BOOL) !((capab->ucCapabilities & DISPLAY_EXCHANGE_MASK) >> DISPLAY_EXCHANGE_SHIFT);
	}
};


// Weight Scale Class
public ref class WeightScale
{
public:
	// Channel parameters
	static const UCHAR DEVICE_TYPE = 0x77;
	static const UCHAR TX_TYPE = 0x05;
	static const USHORT MSG_PERIOD = 8192;	// 4Hz

	// Data pages
	static const UCHAR PAGE_BODY_WEIGHT = 0x01;
	static const UCHAR PAGE_BODY_COMPOSITION = 0x02;
	static const UCHAR PAGE_METABOLIC_INFO = 0x03;
   static const UCHAR PAGE_BODY_MASS = 0x04;
	static const UCHAR PAGE_USER_PROFILE = 0x3A;

	// Body weight
	static const UCHAR RESERVED = 0xFF;
	static const USHORT WEIGHT_INVALID = 0xFFFF;
	static const USHORT WEIGHT_COMPUTING = 0xFFFE;
   static const UCHAR MASS_INVALID = 0xFF;
   static const UCHAR MASS_COMPUTING = 0xFF;

	// Interleaving requirements
	static const UCHAR INTERVAL_COMMON = 21;	// Pages 80, 81 required every 21 messages

	// Type of device
	enum class DeviceClass : UCHAR
	{
		SCALE,
		DISPLAY
	};

	// Error handling
	// Flags indicate errors causing the exception
	ref class Error : public System::Exception{
	public:
		BOOL	bUndefPage;			// Undefined page
		BOOL	bBadReserved;		// Invalid values on reserved fields

		enum class Code : UCHAR		// Error code definitions
		{
			UNDEF_PAGE,
			INVALID_RESERVED		
		};

		Error()
		{
			ClearFlags();
		}

		Error(Code eCode1_)
		{
			ClearFlags();
			SetFlags(eCode1_);		
		}

		Error(Code eCode1_, Code eCode2_)
		{		
			ClearFlags();
			SetFlags(eCode1_);	
			SetFlags(eCode2_);
		}


	private:
		void ClearFlags()
		{
			bUndefPage = FALSE;
			bBadReserved = FALSE;
		}

		void SetFlags(Code eCode_)
		{
			switch(eCode_)
			{
			case Code::UNDEF_PAGE:
				bUndefPage = TRUE;
				break;
			case Code::INVALID_RESERVED:
				bBadReserved = TRUE;
				break;
			default:
				break;

			}
		}
	};

public:
	DeviceClass eType;				// Type of device (eg. sensor, display)

	// Page 1
	USHORT	usBodyWeight100;		// Body weight (1/100kg)
	
	// Page 2
	USHORT usHydrationP100;			// Hydration (0.01%)
	USHORT usBodyFatP100;			// Body fat (0.01%)
	
	// Page 3
	USHORT usActiveMetRate4;		// Active metabolic rate (1/4 kcal)
	USHORT usBasalMetRate4;			// Basal metabolic rate (1/4 kcal)

   // Page 4
   USHORT usMuscleMass100;       // Muscle mass (1/100kg)
   UCHAR ucBoneMass10;           // Bone mass (1/10kg)

	// User profiles
	WeightScaleUserProfile^ uprofActive;		// Active user profile
	WeightScaleUserProfile^ uprofScale;			// Scale user profile
	WeightScaleUserProfile^ uprofDisplay;		// Display user profile

	// Capabilities
	WeightScaleCapabilities^ capabScale;		// Scale capabilities
	WeightScaleCapabilities^ capabDisplay;		// Display capabilities

	// Error handling
	BOOL bValidation;				// Turns validation on/off

public:
	WeightScale(DeviceClass eType_)
	{
		eType = eType_;	
		uprofActive = gcnew WeightScaleUserProfile();
		uprofScale = gcnew WeightScaleUserProfile();
		uprofDisplay = gcnew WeightScaleUserProfile();
		capabScale = gcnew WeightScaleCapabilities(WeightScaleCapabilities::DeviceClass::ADVANCED_SCALE);
		capabDisplay = gcnew WeightScaleCapabilities(WeightScaleCapabilities::DeviceClass::ADVANCED_DISPLAY);

		bValidation = FALSE;
	}

	~WeightScale()
	{
	}


public:

/**************************************************************************
 * Decode
 * 
 * Decodes all defined weight scale pages:
 * - Body weight
 * - User profile
 * Exceptions are thrown when dealing with non compliant pages
 *
 * pucRxBuffer_: pointer to the buffer containing the received data
 * 
 * returns: N/A
 *
 **************************************************************************/
	void Decode(UCHAR* pucRxBuffer_)
	{
		switch(pucRxBuffer_[0])
		{
			case PAGE_BODY_WEIGHT:	// Received from scale
				uprofScale->usID = (USHORT) pucRxBuffer_[1];
				uprofScale->usID |= (((USHORT) pucRxBuffer_[2]) << 8);
				capabScale->ucCapabilities = pucRxBuffer_[3];
				usBodyWeight100 = (USHORT) pucRxBuffer_[6];
				usBodyWeight100 |= (((USHORT) pucRxBuffer_[7]) << 8);
				WeightScaleCapabilities::decode(capabScale);
				break;
			case PAGE_BODY_COMPOSITION:
				uprofScale->usID = (USHORT) pucRxBuffer_[1];
				uprofScale->usID |= (((USHORT) pucRxBuffer_[2]) << 8);
				usHydrationP100 = (USHORT) pucRxBuffer_[4];
				usHydrationP100 |= (((USHORT) pucRxBuffer_[5]) << 8);
				usBodyFatP100 = (USHORT) pucRxBuffer_[6];
				usBodyFatP100 |= (((USHORT) pucRxBuffer_[7]) << 8);
				if(bValidation)
				{
					if(pucRxBuffer_[3] != RESERVED)
						throw gcnew Error(Error::Code::INVALID_RESERVED);	// Error: Invalid reserved field
				}
				break;
			case PAGE_METABOLIC_INFO:
				uprofScale->usID = (USHORT) pucRxBuffer_[1];
				uprofScale->usID |= (((USHORT) pucRxBuffer_[2]) << 8);
				usActiveMetRate4 = (USHORT) pucRxBuffer_[4];
				usActiveMetRate4 |= (((USHORT) pucRxBuffer_[5]) << 8);
				usBasalMetRate4 = (USHORT) pucRxBuffer_[6];
				usBasalMetRate4 |= (((USHORT) pucRxBuffer_[7]) << 8);
				if(bValidation)
				{
					if(pucRxBuffer_[3] != RESERVED)
						throw gcnew Error(Error::Code::INVALID_RESERVED);	// Error: Invalid reserved field
				}
				break;
         case PAGE_BODY_MASS:
            uprofScale->usID = (USHORT) pucRxBuffer_[1];
            uprofScale->usID |= (((USHORT) pucRxBuffer_[2]) << 8);
            usMuscleMass100 = (USHORT) pucRxBuffer_[5];
            usMuscleMass100 |= (((USHORT) pucRxBuffer_[6]) << 8);
            ucBoneMass10 = (UCHAR) pucRxBuffer_[7];
            if(bValidation)
            {
               if(pucRxBuffer_[3] != RESERVED || pucRxBuffer_[4] != RESERVED)
                  throw gcnew Error(Error::Code::INVALID_RESERVED);  // Error: Invalid reserved field
            }
            break;
			case PAGE_USER_PROFILE:
				if(eType == DeviceClass::SCALE)		// Profile received from display
				{
					uprofDisplay->usID = (ULONG) pucRxBuffer_[1];				// Profile ID (low byte)
					uprofDisplay->usID |= (((ULONG) pucRxBuffer_[2]) << 8);		// Profile ID (high byte)
					capabDisplay->ucCapabilities = pucRxBuffer_[3];				// Capabilities
					uprofDisplay->ucGender = pucRxBuffer_[5] >> WeightScaleUserProfile::GENDER_SHIFT;	// Gender
					uprofDisplay->ucAge = pucRxBuffer_[5] & WeightScaleUserProfile::AGE_MASK;			// Age
					uprofDisplay->ucHeight = pucRxBuffer_[6];					// Height

					// Decode the athlete bit
					if (pucRxBuffer_[7] & WeightScaleUserProfile::ACTIVITY_ATHLETE_MASK)
						uprofDisplay->bAthlete = TRUE;
					else
                  uprofDisplay->bAthlete = FALSE;
                  
               uprofDisplay->ucDescription = pucRxBuffer_[7] & WeightScaleUserProfile::DESCR_MASK; // Activity level

					WeightScaleCapabilities::decode(capabDisplay);				// Decode display capabilities
					
					if(bValidation)
					{
						if(pucRxBuffer_[4] != RESERVED)
							throw gcnew Error(Error::Code::INVALID_RESERVED);	// Error: Invalid reserved field
					}
				}
				else if(eType == DeviceClass::DISPLAY)	// Profile received from scale
				{
					uprofScale->usID = (ULONG) pucRxBuffer_[1];
					uprofScale->usID |= (((ULONG) pucRxBuffer_[2]) << 8);
					capabScale->ucCapabilities = pucRxBuffer_[3];
					uprofScale->ucGender = pucRxBuffer_[5] >> WeightScaleUserProfile::GENDER_SHIFT;
					uprofScale->ucAge = pucRxBuffer_[5] & WeightScaleUserProfile::AGE_MASK;
					uprofScale->ucHeight = pucRxBuffer_[6];

					if (pucRxBuffer_[7] & WeightScaleUserProfile::ACTIVITY_ATHLETE_MASK)
						uprofScale->bAthlete = TRUE;
					else
						uprofScale->bAthlete = FALSE;

               uprofDisplay->ucDescription = pucRxBuffer_[7] & WeightScaleUserProfile::DESCR_MASK;

					WeightScaleCapabilities::decode(capabScale);
				}
				if(bValidation)
				{
					if(pucRxBuffer_[4] != RESERVED)
						throw gcnew Error(Error::Code::INVALID_RESERVED);	// Error: Invalid reserved field
				}
				break;
			default:
				if(bValidation)
					throw gcnew Error(Error::Code::UNDEF_PAGE);		// Error: Undefined page
				break;
		}
	}


/**************************************************************************
 * Encode
 * 
 * Encodes main weight scale data pages:
 * - Body Weight
 * - User Profile
 *
 * Exceptions are thrown when dealing with invalid data
 *
 * ucPageNum_: number of page to encode
 * pucTxBuffer_: pointer to the buffer that will store the encoded data
 * 
 * returns: N/A
 *
 **************************************************************************/
	void Encode(UCHAR ucPageNum_, UCHAR* pucTxBuffer_)
	{
		switch(ucPageNum_)
		{
			case PAGE_BODY_WEIGHT:
				pucTxBuffer_[0] = ucPageNum_;
				pucTxBuffer_[1] = (UCHAR) uprofActive->usID;			// Active user profile ID (low byte)
				pucTxBuffer_[2] = (UCHAR) (uprofActive->usID >> 8);		// Active user profile ID (high byte)
				if(eType == DeviceClass::SCALE)
				{
					WeightScaleCapabilities::encode(capabScale);		// Encode scale capabilities
					pucTxBuffer_[3] = capabScale->ucCapabilities;		// Scale capabilities
				}
				pucTxBuffer_[4] = RESERVED;
				pucTxBuffer_[5] = RESERVED;
				pucTxBuffer_[6] = (UCHAR) usBodyWeight100;				// Body weight in 1/100 kg (low byte)
				pucTxBuffer_[7] = (UCHAR) (usBodyWeight100 >> 8);		// Body weight in 1/100 kg (high byte)
				break;
			case PAGE_BODY_COMPOSITION:
				pucTxBuffer_[0] = ucPageNum_;
				pucTxBuffer_[1] = (UCHAR) uprofActive->usID;			// Active user profile ID (low byte)
				pucTxBuffer_[2] = (UCHAR) (uprofActive->usID >> 8);		// Active user profile ID (high byte)
				pucTxBuffer_[3] = RESERVED;
				pucTxBuffer_[4] = (UCHAR) usHydrationP100;				// % Hydration in intervals of 0.01% (low byte)
				pucTxBuffer_[5] = (UCHAR) (usHydrationP100 >> 8);		// % Hydration in intervals of 0.01% (high byte)
				pucTxBuffer_[6] = (UCHAR) usBodyFatP100;				// % Body fat in intervals of 0.01% (low byte)
				pucTxBuffer_[7] = (UCHAR) (usBodyFatP100 >> 8);			// % Body fat in intervals of 0.01% (high byte)
				break;
			case PAGE_METABOLIC_INFO:
				pucTxBuffer_[0] = ucPageNum_;
				pucTxBuffer_[1] = (UCHAR) uprofActive->usID;			// Active user profile ID (low byte)
				pucTxBuffer_[2] = (UCHAR) (uprofActive->usID >> 8);	// Active user profile ID (high byte)
				pucTxBuffer_[3] = RESERVED;
				pucTxBuffer_[4] = (UCHAR) usActiveMetRate4;				// Active metabolic rate in kJ (low byte)
				pucTxBuffer_[5] = (UCHAR) (usActiveMetRate4 >> 8);		// Active metabolic rate in kJ (high byte)
				pucTxBuffer_[6] = (UCHAR) usBasalMetRate4;				// Basal metabolic rate in kJ (low byte)
				pucTxBuffer_[7] = (UCHAR) (usBasalMetRate4 >> 8);		// Basal metabolic rate in kJ (high byte)
				break;
         case PAGE_BODY_MASS:
            pucTxBuffer_[0] = ucPageNum_;
            pucTxBuffer_[1] = (UCHAR) uprofActive->usID;			// Active user profile ID (low byte)
				pucTxBuffer_[2] = (UCHAR) (uprofActive->usID >> 8);   // Active user profile ID (high byte)
				pucTxBuffer_[3] = RESERVED;
            pucTxBuffer_[4] = RESERVED;
            pucTxBuffer_[5] = (UCHAR) usMuscleMass100;         // Muscle mass in 1/100 kg (low byte)
            pucTxBuffer_[6] = (UCHAR) (usMuscleMass100 >> 8);  // Muscle mass in 1/100 kg (high byte)
            pucTxBuffer_[7] = ucBoneMass10;                    // Bone mass in 1/10kg
            break;
			case PAGE_USER_PROFILE:
				pucTxBuffer_[0] = ucPageNum_;
				pucTxBuffer_[1] = (UCHAR) uprofActive->usID;			// Active user profile ID (low byte)
				pucTxBuffer_[2] = (UCHAR) (uprofActive->usID >> 8);		// Active user profile ID (high byte)
				if(eType == DeviceClass::SCALE)
				{
					WeightScaleCapabilities::encode(capabScale);		// Encode scale capabilities
					pucTxBuffer_[3] = capabScale->ucCapabilities;		// Scale capabilities
				}
				else if(eType == DeviceClass::DISPLAY)
				{
					WeightScaleCapabilities::encode(capabDisplay);		// Encode display capabilities
					pucTxBuffer_[3] = capabDisplay->ucCapabilities;		// Display capabilities
				}
				pucTxBuffer_[4] = RESERVED;
				pucTxBuffer_[5] = uprofActive->ucGender << WeightScaleUserProfile::GENDER_SHIFT;	// Gender
				pucTxBuffer_[5] |= (uprofActive->ucAge & WeightScaleUserProfile::AGE_MASK);			// Age
				pucTxBuffer_[6] = uprofActive->ucHeight;								// Height
            pucTxBuffer_[7] = uprofActive->ucDescription & WeightScaleUserProfile::DESCR_MASK;  // Description bit field
            pucTxBuffer_[7] |= (uprofActive->bAthlete << WeightScaleUserProfile::ACTIVITY_ATHLETE_SHIFT);   // Athlete bit
				break;
			default:
				if(bValidation)
					throw gcnew Error(Error::Code::UNDEF_PAGE);		// Error: Undefined page
				break;
		}
	}



};