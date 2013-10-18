/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/
 

#pragma once

#include "types.h"
#define MSM_REV				1.0					// Device Profile Revision Number


public ref class MSM
{

public:
	// Channel Parameters
	static const UCHAR DEVICE_TYPE = 15;
	static const UCHAR TX_TYPE = 5;
	static const USHORT MSG_PERIOD = 8192;	// 4 Hz
   static const USHORT HALF_PERIOD = 16384;  // 2 Hz
	
	// Data Pages
	static const UCHAR PAGE_1 = 0x01;
	static const UCHAR PAGE_2 = 0x02;
	static const UCHAR PAGE_3 = 0x03;
	static const UCHAR PAGE_4 = 0x04;
	static const UCHAR PAGE_CALIBRATION = 0x30;

	// Calibration Messages
	static const USHORT CAL_SCALE_CONF = 0x2710;     // this translates to 10000 or 1.0000 for a scale factor
	static const USHORT CAL_SCALE_REQUEST = 0x0000;
	static const USHORT CAL_SCALE_INVALID = 0xFFFF;
	static const USHORT CAL_SF_SCALE_FACTOR = 10000;
	static const UCHAR CAL_MODE_INVALID = 0xFF;

	// Acknowledged Messages
	static const UCHAR ACK_FAIL = 0;
	static const UCHAR ACK_SUCCESS = 1;
	
	// Reserved/invalid/special values
	static const UCHAR RESERVED = 0xFF;
	static const USHORT INVALID_SPEED = 0xFFFF;
	static const USHORT INVALID_HEADING = 0x0FFF;
	static const USHORT INVALID_ELEVATION = 0xFFFF;
	static const USHORT INVALID_DISTANCE = 0xFFFF;
	static const USHORT MAX_SPEED = 0xFFFE;

	// Bit Masks
	static const UCHAR BYTE_MASK = 0xFF;
	static const UCHAR UPPER_NIBBLE_MASK = 0xF0;
	static const UCHAR LOWER_NIBBLE_MASK = 0x0F;
	static const UCHAR NEGATIVE_BYTE_MASK = 0x80;
	static const UCHAR REMOVE_SIGNED_BIT_MASK = 0x7F;
	static const USHORT HEADING_MASK = 0x0FF0;
	static const ULONG NEGATIVE_LONG_MASK = 0xF0000000;

	// Bit shifts
	static const UCHAR NIBBLE_SHIFT = 4;
	static const UCHAR BYTE_SHIFT = 8;
	static const UCHAR LAT_BYTE_2_SHIFT = 8;
	static const UCHAR LAT_BYTE_3_SHIFT = 16;
	static const UCHAR LAT_BYTE_4_SHIFT = 24;
	static const UCHAR LON_BYTE_5_SHIFT = 4;
	static const UCHAR LON_BYTE_6_SHIFT = 12;
	static const UCHAR LON_BYTE_7_SHIFT = 20;

	//Scale and offset Factors
	static const USHORT TIME_SCALE_FACTOR = 1024;
	static const UCHAR  DIST_SCALE_FACTOR = 10;
	static const USHORT SPEED_SCALE_FACTOR = 1000;
	static const UCHAR  HEADING_SCALE_FACTOR = 10;
	static const UCHAR  ELEVATION_SCALE_FACTOR = 5;
	static const USHORT ELEVATION_OFFSET = 500;
	static const DOUBLE SEMI_CIRCLE_CONVERSION = System::Math::Pow(2.0, 27) / 180;  // conversion to semicircles from degrees

	enum class GpsFix : UCHAR
	{
		NONE,
		SEARCHING,
		PROPAGATING,
		LAST_KNOWN,
		TWO_D,
		TWO_D_WAAS,
		TWO_D_DIF,
		THREE_D,
		THREE_D_WAAS,
		THREE_D_DIF,
		INVALID = 0x0F
	};

	enum class TurnByTurns : UCHAR
	{
		DESTINATION,
		TURN,
		HARD_TURN,
		SLIGHT_TURN, 
		U_TURN,
		EXIT,
		LEFT_MASK = 0x80,
		RIGHT_MASK = 0x7F,
	};
	enum class CalibrationStatus : UCHAR
	{
		NONE,
		REQUEST_IN_PROGRESS,
		SET_SCALE_IN_PROGRESS,
		COMPLETE
	};

	// Error handling
	// Flags indicate errors causing the exception
	ref class Error : public System::Exception{
	public:
		BOOL	bBadReserved;		// Invalid values on reserved fields
		BOOL	bUndefPage;			// Undefined page
		BOOL	bUndefCalID;		// Undefined calibration message ID
		BOOL	bUndefCTFID;		// Undefined CTF calibration message ID
		BOOL	bUndefCTFAck;		// Undefined CTF Acked message
		BOOL	bUndefAutoZero;		// Undefined auto zero status

		enum class Code : UCHAR		// Error code definitions
		{
			INVALID_RESERVED,		// Invalid value in reserved field
			UNDEF_PAGE,				// Undefined data page
			UNDEF_CAL_ID,			// Undefined calibration message ID
			UNDEF_CTF_ID,			// Undefined CTF calibration message ID
			UNDEF_CTF_ACKED,		// Undefined CTF acked message
			UNDEF_AUTOZERO,			// Invalid auto zero status
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
			bBadReserved = FALSE;
			bUndefPage = FALSE;
			bUndefCalID	= FALSE;
			bUndefCTFID = FALSE;
			bUndefCTFAck = FALSE;
			bUndefAutoZero = FALSE;
		}

		void SetFlags(Code eCode_)
		{
			switch(eCode_)
			{
			case Code::INVALID_RESERVED:
				bBadReserved = TRUE;
				break;
			case Code::UNDEF_PAGE:
				bUndefPage = TRUE;
				break;
			case Code::UNDEF_CAL_ID:
				bUndefCalID = TRUE;
				break;
			case Code::UNDEF_CTF_ID:
				bUndefCTFID = TRUE;
				break;
			case Code::UNDEF_CTF_ACKED:
				bUndefCTFAck = TRUE;
				break;
			case Code::UNDEF_AUTOZERO:
				bUndefAutoZero = TRUE;
				break;
			default:
				break;

			}
		}
	};

public:

	// MSM Data Page 1
	USHORT usAcumTime1024;          // Cumulative time (1/1024 s)
	USHORT usAcumDist10;            // Accumulated Distance (0.1 m)
	USHORT usInstSpeed1000;         // Instantaneous speed (0.001 m/s)

	// MSM Data Page 2
	SLONG slLatitude_SC;            // Latitude (pi/2^27 radians)
	SLONG slLongitude_SC;           // Longitude (pi/2^27 radians)
	BOOL bPage2Enabled;				  // Signals if page 2 is to be transmitted

	// MSM Data Page 3
	UCHAR  ucFixType;               // Current fix of GPS receiver
	USHORT usHeading10;             // Heading of the user (0.1 degrees)
	USHORT usElevation5;            // Elevation of position (0.2 m)
	BOOL bPage3Enabled;             // Determines if page 3 is to be transmitted

	//MSMS Data Page 4
	USHORT	usDistanceTwo;			//Distance between 2nd and 1st Turn by Turn comamnds
	UCHAR	ucTurnByTurnTwo;		//Second turn by turn command required
	USHORT	usDistanceOne;			//Distance until 1st turn by turn command
	UCHAR	ucTurnByTurnOne;		//First turn by turn command required
	BOOL	bPage4Enabled;

	// MSM Calibration Variables
	UCHAR ucMode;                   // The calibration mode (km/h)
	USHORT usScaleFactor10000;      // Scale Factor of calibration (0.0001)

	// Error handling
	BOOL bValidation;			         // Turns validation on/off

public:
	MSM()
	{
		bValidation = FALSE;          // Validation is disabled by default
		//initialize variables to invalid if possible, otherwise 0
		usAcumTime1024 = 0;
		usAcumDist10 = 0;
		usInstSpeed1000 = INVALID_SPEED;
		slLatitude_SC = 0;
		slLongitude_SC = 0;
		ucFixType = 0;
		usHeading10 = INVALID_HEADING;
		usElevation5 = INVALID_ELEVATION;
		ucMode = CAL_MODE_INVALID;
		usScaleFactor10000 = CAL_SCALE_INVALID;
		bPage2Enabled = FALSE;
		bPage3Enabled = FALSE;
		bPage4Enabled = FALSE;
		usDistanceTwo = INVALID_DISTANCE;			
		ucTurnByTurnTwo = 0;		
		usDistanceOne = 0;			
		ucTurnByTurnOne = 0;		
	}

	~MSM()
	{
	}


public:

/**************************************************************************
 * MSM::Decode
 * 
 * Decodes all main data pages and calibration pages
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
			case PAGE_1:
				// decode time and distance - required fields
				usAcumTime1024 = pucRxBuffer_[2] + (pucRxBuffer_[3] << BYTE_SHIFT);
				usAcumDist10 = pucRxBuffer_[4] + (pucRxBuffer_[5] << BYTE_SHIFT);

				// decode optional field - speed
				usInstSpeed1000 = pucRxBuffer_[6] + (pucRxBuffer_[7] << BYTE_SHIFT);

				// ensure the reserved fields are using the correct values
				if(pucRxBuffer_[1] != RESERVED && bValidation)
				{
					throw gcnew Error(Error::Code::INVALID_RESERVED);	// Error: Invalid value in reserved field
				}
				
            break;

			case PAGE_2:
				bPage2Enabled = TRUE;  // enable page 2 display

				// decode latitude
				slLatitude_SC = pucRxBuffer_[1] + (pucRxBuffer_[2] << LAT_BYTE_2_SHIFT) + (pucRxBuffer_[3] << LAT_BYTE_3_SHIFT) + (((pucRxBuffer_[4] & UPPER_NIBBLE_MASK) >> 4) << LAT_BYTE_4_SHIFT);
				// check if latitude is negative
				if (pucRxBuffer_[4] & NEGATIVE_BYTE_MASK)
					slLatitude_SC = slLatitude_SC | NEGATIVE_LONG_MASK;

				// decode longitude
				slLongitude_SC = (pucRxBuffer_[4] & LOWER_NIBBLE_MASK) + (pucRxBuffer_[5] << LON_BYTE_5_SHIFT) + (pucRxBuffer_[6] << LON_BYTE_6_SHIFT) + (pucRxBuffer_[7] << LON_BYTE_7_SHIFT);
				// check if longitude is negative
				if (pucRxBuffer_[7] & NEGATIVE_BYTE_MASK)
					slLongitude_SC = slLongitude_SC | NEGATIVE_LONG_MASK;
				
            break;

			case PAGE_3:
				bPage3Enabled = TRUE; // enabled page 3 display

				// decode fix type
				ucFixType = (pucRxBuffer_[4] & UPPER_NIBBLE_MASK) >> NIBBLE_SHIFT;

				// decode Heading and scale appropriatley
				usHeading10 = (pucRxBuffer_[4] & LOWER_NIBBLE_MASK) + (pucRxBuffer_[5] << NIBBLE_SHIFT);

				// decode Elevation and scale appropriately
				usElevation5 = pucRxBuffer_[6] + (pucRxBuffer_[7] << BYTE_SHIFT);

				// ensure the reserved fields are using the correct values
				for (int i = 1; i < 4; i++)
				{
					if(pucRxBuffer_[i] != RESERVED && bValidation)
						throw gcnew Error(Error::Code::INVALID_RESERVED);	// Error: Invalid value in reserved field
				}

				break;

			case PAGE_4:
				bPage4Enabled = TRUE;	//enable page 4 display
				usDistanceTwo = (pucRxBuffer_[3] << BYTE_SHIFT) | pucRxBuffer_[2];			
				ucTurnByTurnTwo = pucRxBuffer_[4];		
				usDistanceOne = (pucRxBuffer_[6] << BYTE_SHIFT) | pucRxBuffer_[5];			
				ucTurnByTurnOne = pucRxBuffer_[7];
				break;

			case PAGE_CALIBRATION:
				// decode calibration mode and the scale factor
				ucMode = pucRxBuffer_[5];
				usScaleFactor10000 = pucRxBuffer_[6] + (pucRxBuffer_[7] << BYTE_SHIFT);

				// ensure the reserved fields are using the correct values
				for (int i = 1; i < 5; i++)
				{
					if(pucRxBuffer_[i] != RESERVED && bValidation)
						throw gcnew Error(Error::Code::INVALID_RESERVED);	// Error: Invalid value in reserved field
				}

				break;

			default:
				break;
		}
	}
	

/**************************************************************************
 * MSM::EncodeData
 * 
 * Encodes the MSM data pages:
 * - Main data page 1 - time, distance, speed
 * - Calibration data page
 * - Secondary data pages - Lat/Long, Elevation/Heading
 *
 * Exceptions are thrown when dealing with invalid data
 *
 * ucPageNum_: number of page to encode
 * pucTxBuffer_: pointer to the buffer that will store the encoded data
 * 
 * returns: N/A
 *
 **************************************************************************/
	void EncodeData(UCHAR ucPageNum_, UCHAR* pucTxBuffer_)
	{
		// need these variables so that the bit shifting does not get messed up when using negative values
		ULONG ulTempLat = slLatitude_SC;
		ULONG ulTempLon = slLongitude_SC;

		switch(ucPageNum_)
		{
			case PAGE_1:
				pucTxBuffer_[0] = ucPageNum_;
				pucTxBuffer_[1] = RESERVED;
				pucTxBuffer_[2] = usAcumTime1024 & BYTE_MASK;     // Acumulated time in 1/1024 seconds
				pucTxBuffer_[3] = (usAcumTime1024 >> BYTE_SHIFT) & BYTE_MASK;
				pucTxBuffer_[4] = usAcumDist10 & BYTE_MASK;	     // Acumulated distance (0.1 m)
				pucTxBuffer_[5] = (usAcumDist10 >> BYTE_SHIFT) & BYTE_MASK;
				pucTxBuffer_[6] = usInstSpeed1000 & BYTE_MASK;    // Instantaneous speed (0.001 m/s)
				pucTxBuffer_[7] = (usInstSpeed1000 >> BYTE_SHIFT) & BYTE_MASK;
				break;

			case PAGE_2:
				pucTxBuffer_[0] = ucPageNum_;
				pucTxBuffer_[1] = slLatitude_SC & BYTE_MASK;             // Latitude in semicircles
				pucTxBuffer_[2] = (slLatitude_SC >> LAT_BYTE_2_SHIFT) & BYTE_MASK;
				pucTxBuffer_[3] = (slLatitude_SC >> LAT_BYTE_3_SHIFT) & BYTE_MASK;
				// byte 4 is the shared byte between lat and long
				if (slLatitude_SC < 0)  // check if latitude is negative
					pucTxBuffer_[4] = (((((slLatitude_SC >> LAT_BYTE_4_SHIFT) & LOWER_NIBBLE_MASK) << NIBBLE_SHIFT) & UPPER_NIBBLE_MASK) | NEGATIVE_BYTE_MASK) + (ulTempLon & LOWER_NIBBLE_MASK);
				else
					pucTxBuffer_[4] = ((((slLatitude_SC >> LAT_BYTE_4_SHIFT) & LOWER_NIBBLE_MASK) << NIBBLE_SHIFT) & UPPER_NIBBLE_MASK) + (slLongitude_SC & LOWER_NIBBLE_MASK);  // the shared byte between lat and long
				
				pucTxBuffer_[5] = (slLongitude_SC >> LON_BYTE_5_SHIFT) & BYTE_MASK;     // Longitude in semicircles
				pucTxBuffer_[6] = (slLongitude_SC >> LON_BYTE_6_SHIFT) & BYTE_MASK;
				// check if longitude is negative
				if (slLongitude_SC < 0)
					pucTxBuffer_[7] = ((slLongitude_SC >> LON_BYTE_7_SHIFT) & BYTE_MASK) | NEGATIVE_BYTE_MASK;
				else
					pucTxBuffer_[7] = (slLongitude_SC >> LON_BYTE_7_SHIFT) & BYTE_MASK;
				break;

			case PAGE_3:
				pucTxBuffer_[0] = ucPageNum_;
				pucTxBuffer_[1] = RESERVED;
				pucTxBuffer_[2] = RESERVED;
				pucTxBuffer_[3] = RESERVED;
				pucTxBuffer_[4] = (ucFixType << NIBBLE_SHIFT) + (usHeading10 & LOWER_NIBBLE_MASK);
				pucTxBuffer_[5] = (usHeading10 & HEADING_MASK) >> NIBBLE_SHIFT;
				pucTxBuffer_[6] = usElevation5 & BYTE_MASK;
				pucTxBuffer_[7] = (usElevation5 >> BYTE_SHIFT) & BYTE_MASK;
				break;

			case PAGE_4:
				pucTxBuffer_[0] = ucPageNum_;
				pucTxBuffer_[1] = RESERVED;
				pucTxBuffer_[2] = usDistanceTwo & BYTE_MASK;
				pucTxBuffer_[3] = (usDistanceTwo >> BYTE_SHIFT) & BYTE_MASK;
				pucTxBuffer_[4] = ucTurnByTurnTwo;
				pucTxBuffer_[5] = usDistanceOne & BYTE_MASK;
				pucTxBuffer_[6] = (usDistanceOne >> BYTE_SHIFT) & BYTE_MASK;
				pucTxBuffer_[7] = ucTurnByTurnOne;
				break;

			case PAGE_CALIBRATION:
				pucTxBuffer_[0] = ucPageNum_;
				pucTxBuffer_[1] = RESERVED;
				pucTxBuffer_[2] = RESERVED;
				pucTxBuffer_[3] = RESERVED;
				pucTxBuffer_[4] = RESERVED;
				pucTxBuffer_[5] = ucMode;
				pucTxBuffer_[6] = usScaleFactor10000 & BYTE_MASK;
				pucTxBuffer_[7] = (usScaleFactor10000 >> BYTE_SHIFT) & BYTE_MASK;
				break;

			default:
				break;
		}
	}

/**************************************************************************
 * MSM::IsSpeedValid
 * 
 * Checks if the speed value is invalid
 *
 * usSpeed1000_: speed (m/s)
 * 
 * returns: TRUE if speed is valid, FALSE otherwise
 *
 **************************************************************************/
	BOOL IsSpeedValid(USHORT usSpeed1000_)
	{
		if(usSpeed1000_ == INVALID_SPEED)
			return FALSE;
		else
			return TRUE;		
	}

/**************************************************************************
 * MSM::IsFixTypeValid
 * 
 * Checks if the fix type is invalid
 *
 * ucFixType_: GPS Fix Type
 * 
 * returns: TRUE if fix type is valid, FALSE otherwise
 *
 **************************************************************************/
	BOOL IsFixTypeValid(UCHAR ucFixType_)
	{
		if(ucFixType_ == 0x0F)//MSM::GpsFix::INVALID)
			return FALSE;
		else
			return TRUE;
	}


/**************************************************************************
 * MSM::IsHeadingValid
 * 
 * Checks if the heading is invalid
 *
 * usHeading10_: 0.1 degrees
 * 
 * returns: TRUE if heading is valid, false otherwise
 *
 **************************************************************************/
	BOOL IsHeadingValid()
	{
		if(usHeading10 == INVALID_HEADING)
			return FALSE;
		else
			return TRUE;
	}


/**************************************************************************
 * MSM::IsElevationValid
 * 
 * Checks if the elvation is invalid
 *
 * usElevation5_: 0.2 m
 * 
 * returns: TRUE if elevation is valid, FALSE otherwise
 *
 **************************************************************************/
	BOOL IsElevationValid()
	{
		if(usElevation5 == INVALID_ELEVATION)
			return FALSE;
		else
			return TRUE;
	}

/**************************************************************************
 * MSM::IsCalModeValid
 * 
 * Checks if the calibration mode is invalid
 *
 * ucCalMode_: mode (km/h)
 * 
 * returns: TRUE if calibration mode is valid, FALSE otherwise
 *
 **************************************************************************/
	BOOL IsCalModeValid(UCHAR ucCalMode_)
	{
		if(ucCalMode_ == CAL_MODE_INVALID)
			return FALSE;
		else
			return TRUE;
	}

/**************************************************************************
 * MSM::IsCalScaleValid
 * 
 * Checks if the scale factor is invalid
 *
 * usCalScale10000_: scale factor
 * 
 * returns: TRUE if the scale factor is valid, FALSE otherwise
 *
 **************************************************************************/
	BOOL IsCalScaleValid(USHORT usCalScale10000_)
	{
		if(usCalScale10000_ == CAL_SCALE_INVALID)
			return FALSE;
		else
			return TRUE;
	}
	BOOL IsDistanceValid(USHORT usDistance_)
	{
		if(usDistance_ == INVALID_DISTANCE)
			return FALSE;
		else 
			return TRUE;
	}

};