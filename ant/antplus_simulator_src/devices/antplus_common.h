/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/
 

#pragma once

#define COMMON_REV				1.1				// Device Profile Revision Number

public ref class CommonData
{

public:
	static const UCHAR START_COMMON_PAGE = 0x40;
	static const UCHAR PAGE70 = 70;
	static const UCHAR PAGE80 = 80;
	static const UCHAR PAGE81 = 81;
	static const UCHAR PAGE82 = 82;
	static const UCHAR PAGE83 = 83;
	static const UCHAR PAGE85 = 85;
	static const UCHAR PAGE86 = 86;

	static const UCHAR RESERVED = 0xFF;
	static const USHORT BATTERY_VOLTAGE_INVALID = 0xFFF;
	static const ULONG MAX_2SEC = (ULONG) 0xFFFFFF << 1;
	static const ULONG MAX_16SEC = (ULONG) 0xFFFFFF << 4;
	static const ULONG OPERATING_TIME_MASK = 0x00FFFFFF;
	static const UCHAR COMMAND_TYPE_DATA = 0x01;
	static const UCHAR COMMAND_TYPE_FS = 0x02;

	enum class BatStatus : UCHAR			// Battery status definitions
	{
		NEW = 1,
		GOOD = 2,
		OK = 3,
		LOW = 4,
		CRITICAL = 5,
		INVALID = 7
	};

	enum class TimeResolution : UCHAR		// Time resolution definitions
	{
		TWO = 2,		// Time in interval of 2 seconds, up to 1.1 years rollover
		SIXTEEN = 16	// Time in interval of 16 seconds, up to 8.5 years rollover
	};


	enum class DayOfWeek : UCHAR            // defines the day of the week
	{
		SUNDAY = 0,
		MONDAY,
		TUESDAY,
		WEDNESDAY,
		THURSDAY,
		FRIDAY,
		SATURDAY,
		INVALID
	};

	enum class TotalSizeUnits : UCHAR
	{
		BIT = 0x00,
		BYTE = 0x80,
		BASE_UNIT = 0x08,
		KILO = 0x01,
		MEGA = 0x02,
		TERA = 0x03,
	};

	enum class PairingStates : UCHAR
	{
		PAIRED = 0x00,
		UNPAIRED = 0x80,
	};

	enum class ConnectionStates : UCHAR
	{
		CLOSED = 0x00,
		SEARCHING = 0x08,
		TRACKING = 0x10,
	};

	enum class NetworkKeys : UCHAR
	{
		NETWORK_PUBLIC = 0x00,
		NETWORK_PRIVATE = 0x01,
		NETWORK_ANTPLUS = 0x02,
		NETWORK_ANTFS = 0x03,
	};

	// Error handling
	// Flags indicate errors causing the exception
	ref class Error : public System::Exception{
	public:
		BOOL	bUndefPage;			// Undefined page
		BOOL	bBadReserved;		// Invalid values on reserved fields
		BOOL	bUndefBatStatus;	// Undefined battery status value

		enum class Code : UCHAR		// Error code definitions
		{
			UNDEF_COMMON_PAGE,
			UNDEF_BAT_STATUS,
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
			bUndefBatStatus = FALSE;
		}

		void SetFlags(Code eCode_)
		{
			switch(eCode_)
			{
			case Code::UNDEF_COMMON_PAGE:
				bUndefPage = TRUE;
				break;
			case Code::UNDEF_BAT_STATUS:
				bUndefBatStatus = TRUE;
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
	//Request (page 70)
	UCHAR ucDescriptorByte1;
	UCHAR ucDescriptorByte2;
	UCHAR ucReqTransResp;
	UCHAR ucReqPageNum;
	UCHAR ucCommandType;

	//Manufacturer's Information (page 80)
	UCHAR ucHwVersion;				// Hardware revision
	USHORT usMfgID;					// Manufacturer ID
	USHORT usModelNum;				// Model number
	// Product Information (Page 81)
	
	//Product Information (page 81)
	UCHAR ucSwVersion;				// Software revision
	ULONG ulSerialNum;				// Serial number
	// Battery Voltage (Page 82)
	
	//Battery Status (page 82)
	BOOL bBattPageEnabled;        // variable to determine if the optional battery page is enabled
	ULONG ulOpTime;					// Cumulative operating time since battery insertion (resolution specified by bit field)
	USHORT usBatVoltage256;			// Battery voltage (1/256 V)
	BatStatus eBatStatus;			// Battery status
	
	//Time and Date (page 83)
	TimeResolution eTimeResolution;	// Resolution of cumulative operating time
	// Common Time Page (Page 83)
	BOOL bTimePageEnabled;	        // determines if this page is to be sent
	UCHAR ucSeconds;
	UCHAR ucMinutes;
	UCHAR ucHours;
	UCHAR ucDays;
	UCHAR ucMonth;
	UCHAR ucYears;
	DayOfWeek eDayOfWeek;

	//Memory Level (page 85)
	UCHAR ucPercentUsed;
	UCHAR ucTotalSizeExponent;
	UCHAR ucTotalSizeUnit;

	//Paired Devices (page 86)
	UCHAR	ucPeripheralDeviceIndex;
	UCHAR	ucTotalConnectedDevices;
	PairingStates ePairingState;
	ConnectionStates eConnectionState;
	NetworkKeys eNetworkKey;
	ULONG	ulDeviceChannelId;

	// Error handling
	BOOL bValidation;				// Turns validation on/off

public:
	CommonData()
	{
		bValidation = FALSE;
	}

	~CommonData()
	{
	}


public:

/**************************************************************************
 * CommonData::Encode
 * 
 * Encodes common data
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
			case PAGE70:
				pucTxBuffer_[0] = ucPageNum_;						//Page Number (70)
				pucTxBuffer_[1] = RESERVED;
				pucTxBuffer_[2] = RESERVED;
				pucTxBuffer_[3] = ucDescriptorByte1;				//First descriptor byte
				pucTxBuffer_[4] = ucDescriptorByte2;				//Second descriptor byte
				pucTxBuffer_[5] = ucReqTransResp;					//The requested transmission response
				pucTxBuffer_[6] = ucReqPageNum;						//Request page number
				pucTxBuffer_[7] = ucCommandType;					//Command type (Data Page or ANTFS Session)
				break;
			case PAGE80:
				pucTxBuffer_[0] = ucPageNum_;						// Page number
				pucTxBuffer_[1] = RESERVED;
				pucTxBuffer_[2] = RESERVED;						
				pucTxBuffer_[3] = ucHwVersion;						// Hw revision
				pucTxBuffer_[4] = usMfgID & 0xFF;					// Low byte of Mfg ID
				pucTxBuffer_[5] = (usMfgID >> 8) & 0xFF;			// High byte of Mfg ID
				pucTxBuffer_[6] = usModelNum & 0xFF;				// Low byte of Model Number
				pucTxBuffer_[7] = (usModelNum >> 8) & 0xFF;			// High byte of Model Number
				break;
			case PAGE81:
				pucTxBuffer_[0] = ucPageNum_;						// Page number
				pucTxBuffer_[1] = RESERVED;
				pucTxBuffer_[2] = RESERVED;
				pucTxBuffer_[3] = ucSwVersion;						// Sw revision
				pucTxBuffer_[4] = ulSerialNum & 0xFF;				// Serial number (bits 0-7)
				pucTxBuffer_[5] = (ulSerialNum >> 8) & 0xFF;		// Serial number (bits 8-15)
				pucTxBuffer_[6] = (ulSerialNum >> 16) & 0xFF;		// Serial number (bits 16-23)
				pucTxBuffer_[7] = (ulSerialNum >> 24) & 0xFF;		// Serial number (bits 24-31)
				break;
			case PAGE82:
				pucTxBuffer_[0] = ucPageNum_;						// Page number
				pucTxBuffer_[1] = RESERVED;
				pucTxBuffer_[2] = RESERVED;
				pucTxBuffer_[3] = ulOpTime & 0xFF;					// Operating time (bits 0-7)
				pucTxBuffer_[4] = (ulOpTime >> 8) & 0xFF;			// Operating time (bits 8-15)
				pucTxBuffer_[5] = (ulOpTime >> 16) & 0xFF;		// Operating time (bits 16-23)
				pucTxBuffer_[6] = usBatVoltage256 & 0XFF;			// Fractional battery voltage (1/256V)
				pucTxBuffer_[7] = (usBatVoltage256 >> 8) & 0x0F;// Coarse battery voltage (V) in bits 0-3 of bit field
				pucTxBuffer_[7] |= ((UCHAR)eBatStatus << 4) & 0x70;// Battery status in bits 4-6 of bit field
				if(eTimeResolution == TimeResolution::TWO)
					pucTxBuffer_[7] |= 0x80;						// If using two second resolution, set msb
				break;
			case PAGE83:
				pucTxBuffer_[0] = ucPageNum_;						// Page number
				pucTxBuffer_[1] = RESERVED;
				pucTxBuffer_[2] = ucSeconds;
				pucTxBuffer_[3] = ucMinutes;
				pucTxBuffer_[4] = ucHours;			
				pucTxBuffer_[5] = ((UCHAR) eDayOfWeek << 5) & 0xE0;	// day of week (bits 7-5)
				pucTxBuffer_[5] |= ucDays & 0x1F;                    // the & ensures clean bits
				pucTxBuffer_[6] = ucMonth;			
				pucTxBuffer_[7] = ucYears;	
				break;
			case PAGE85:
				pucTxBuffer_[0] = ucPageNum_;
				pucTxBuffer_[1] = RESERVED;
				pucTxBuffer_[2] = RESERVED;
				pucTxBuffer_[3] = RESERVED;
				pucTxBuffer_[4] = RESERVED;
				pucTxBuffer_[5] = ucPercentUsed;
				pucTxBuffer_[6] = ucTotalSizeExponent;
				pucTxBuffer_[7] = ucTotalSizeUnit;
				break;
			case PAGE86:
				pucTxBuffer_[0] = ucPageNum_;
				pucTxBuffer_[1] = ucPeripheralDeviceIndex;
				pucTxBuffer_[2] = ucTotalConnectedDevices;
				pucTxBuffer_[3] = (UCHAR)ePairingState;
				pucTxBuffer_[3] |= (UCHAR) eConnectionState;
				pucTxBuffer_[3] |= (UCHAR)eNetworkKey;
				pucTxBuffer_[4] = ulDeviceChannelId & 0xFF;
				pucTxBuffer_[5] = (ulDeviceChannelId >> 8) & 0xFF;
				pucTxBuffer_[6] = (ulDeviceChannelId >> 16) & 0xFF;
				pucTxBuffer_[7] = (ulDeviceChannelId >> 24) & 0xFF;
				break;
			default:
				break;
		}
	}


/**************************************************************************
 * CommonData::Decode
 * 
 * Decodes received Common Data Pages
 * Exceptions are thrown when dealing with invalid data
 *
 * pucRxBuffer_: pointer to the buffer containing the received data
 * 
 * returns: N/A
 *
 **************************************************************************/
	void Decode(UCHAR* pucRxBuffer_)
	{
		BOOL bUndefBatStatus = FALSE;

		switch(pucRxBuffer_[0])
		{
			case PAGE70:
				ucDescriptorByte1 = pucRxBuffer_[3];
				ucDescriptorByte2 = pucRxBuffer_[4];
				ucReqTransResp = pucRxBuffer_[5];
				ucReqPageNum = pucRxBuffer_[6];
				ucCommandType = pucRxBuffer_[7];
				break;
			case PAGE80: 
				ucHwVersion = pucRxBuffer_[3];							// Hw revision	
				usMfgID = pucRxBuffer_[4] | pucRxBuffer_[5] <<8;		// Mfg ID
				usModelNum = pucRxBuffer_[6] | pucRxBuffer_[7] <<8;		// Model number
				break;
			case PAGE81: 
				ucSwVersion = pucRxBuffer_[3];							// Sw revision
				ulSerialNum = pucRxBuffer_[4] | pucRxBuffer_[5]<<8 | pucRxBuffer_[6]<<16 | pucRxBuffer_[7]<<24;		// Serial number
				break;
			case PAGE82:
				ulOpTime = pucRxBuffer_[3] | pucRxBuffer_[4] << 8 | pucRxBuffer_[5] << 16;	// Operating time
				usBatVoltage256 = pucRxBuffer_[6] | (pucRxBuffer_[7] & 0x0F) << 8;			// Battery voltage
				if((pucRxBuffer_[7] & 0x70) == 0 || (pucRxBuffer_[7] & 0x70) == 0x60)
					bUndefBatStatus = TRUE;								// Undefined battery status
				eBatStatus = (BatStatus) ((pucRxBuffer_[7] & 0x70) >> 4);	// Battery status
				if(pucRxBuffer_[7] & 0x80)								// Time resolution
					eTimeResolution = TimeResolution::TWO;
				else
					eTimeResolution = TimeResolution::SIXTEEN;
				break;
			case PAGE83:
				ucSeconds = pucRxBuffer_[2];
				ucMinutes = pucRxBuffer_[3];
				ucHours = pucRxBuffer_[4];
				eDayOfWeek = (DayOfWeek) ((pucRxBuffer_[5] & 0xE0) >> 5);
				ucDays = pucRxBuffer_[5] & 0x1F;
				ucMonth = pucRxBuffer_[6];
				ucYears = pucRxBuffer_[7];
				break;
			case PAGE85:
				ucPercentUsed = pucRxBuffer_[5];
				ucTotalSizeExponent = pucRxBuffer_[6];
				ucTotalSizeUnit = pucRxBuffer_[7];
				break;
			case PAGE86:
				ucPeripheralDeviceIndex = pucRxBuffer_[1];
				ucTotalConnectedDevices = pucRxBuffer_[2];
				ePairingState = (PairingStates)(pucRxBuffer_[3] & 0x80);
				eConnectionState = (ConnectionStates)(pucRxBuffer_[3] & 0x78);
				eNetworkKey = (NetworkKeys)(pucRxBuffer_[3] & 0x07);
				ulDeviceChannelId = pucRxBuffer_[4] | (pucRxBuffer_[5] << 8) | (pucRxBuffer_[6] << 16) | (pucRxBuffer_[7] << 24);
			default:
				break;			
		}
	}

/**************************************************************************
 * CommonData::IsBatteryVoltageInvalid
 * 
 * Checks whether the battery voltage is set as invalid
 *
 * usBatVoltage256_: battery voltage (1/256 V)
 * 
 * returns: N/A
 *
 **************************************************************************/
	BOOL IsBatteryVoltageInvalid(USHORT usBatVoltage256_)
	{
		if((usBatVoltage256 >> 8) == BATTERY_VOLTAGE_INVALID)
			return TRUE;
		else
			return FALSE;
	}


/**************************************************************************
 * CommonData::IsBatteryStatusInvalid
 * 
 * Checks whether the battery status is set as invalid
 *
 * eBatStatus_: battery status code
 * 
 * returns: N/A
 *
 **************************************************************************/
	BOOL IsBatteryStatusInvalid(BatStatus eBatStatus_)
	{
		if(eBatStatus_ == BatStatus::INVALID)
			return TRUE;
		else
			return FALSE;
	}

/**************************************************************************
 * CommonData::IsDayofWeekValid
 * 
 * Checks whether the battery status is set as invalid
 *
 * eBatStatus_: battery status code
 * 
 * returns: N/A
 *
 **************************************************************************/
	BOOL IsDayOfWeekValid(DayOfWeek eDay_)
	{
		if (eDay_ == DayOfWeek::INVALID)
			return FALSE;
		else
			return TRUE;
	}
};
