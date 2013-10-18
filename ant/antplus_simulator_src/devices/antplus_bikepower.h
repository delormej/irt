/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/


#pragma once

#define BP_REV				2.0					// Device Profile Revision Number

public ref class BikePower
{

public:
	// Channel Parameters
	static const UCHAR DEVICE_TYPE = 11;
	static const UCHAR TX_TYPE = 5;
	static const USHORT MSG_PERIOD = 8182;	// ~4.049 Hz
	
	// Data Pages
	static const UCHAR PAGE_CALIBRATION = 0x01;
	static const UCHAR PAGE_GET_SET_PARAMETERS = 0x02;
	static const UCHAR PAGE_POWER = 0x10;
	static const UCHAR PAGE_WHEEL_TORQUE = 0x11;
	static const UCHAR PAGE_CRANK_TORQUE = 0x12;
   static const UCHAR PAGE_TEPS = 0x13;
	static const UCHAR PAGE_CTF = 0x20;

	// Subpages
	static const UCHAR SUBPAGE_CRANK_PARAMETERS = 0x01;

	// Calibration Messages
	static const UCHAR CAL_REQUEST = 0xAA;
	static const UCHAR CAL_AUTOZERO_CONFIG = 0xAB;
	static const UCHAR CAL_SUCCESS = 0xAC;
	static const UCHAR CAL_FAIL = 0xAF;
	static const UCHAR CAL_CTF = 0x10;
	static const UCHAR CAL_TORQUE_METER_CAPABILITIES = 0x12;
	static const UCHAR CAL_RAW_TORQUE = 0x13;
	static const UCHAR CTF_OFFSET = 0x01;
	static const UCHAR CTF_SLOPE = 0x02;
	static const UCHAR CTF_SERIAL = 0x03;
	static const UCHAR CTF_ACK = 0xAC;

	// Reserved/invalid values
	static const UCHAR RESERVED = 0xFF;
	static const UCHAR RESERVED_DESCR = 0x00;
	static const SHORT INVALID_TORQUE = -0x7FFF;  // Signed 0xFFFF
	static const UCHAR INVALID_CADENCE = 0xFF;
	static const UCHAR INVALID_CRANK_LENGTH = 0xFF;
   static const UCHAR INVALID_TEPS = 0xFF;
   static const UCHAR COMBINED_PEDAL_SMOOTHNESS = 0xFE;
	static const UCHAR AUTO_CRANK = 0xFE;
	static const UCHAR READ_ONLY = 0x00;
	static const USHORT MIN_SLOPE = 100;
	static const USHORT MAX_SLOPE = 500;
	static const USHORT MIN_OFFSET = 0;
	static const USHORT MAX_OFFSET = 65535;
	static const SHORT MAX_CALDATA = 32767;
	static const SHORT MIN_CALDATA = -32768;
	static const UCHAR MAX_NOEVENT = 12;

	// Interleaving requirements
	static const UCHAR INTERVAL_COMMON_AND_AZ = 121;	// Pages 80, 81 & Auto Zero Support interleaved every 121 messages (~30.25s)
	static const UCHAR INTERVAL_BATTERY = 61;			// Page 82 interleaved every 61 messages (~15.25s)
	static const UCHAR INTERVAL_MIN_POWER = 9;			// Minimum: Interleave basic power every 9th message
	static const UCHAR INTERVAL_POWER = 5;				// Preferred: Interleave basic power every 5th message
   static const UCHAR INTERVAL_TEPS = 5;           // Preferred: Interleave once every 5 messages
	static const UCHAR INTERVAL_MAX_POWER = 2;			// Maximum: Interleave basic power every other page
	static const UCHAR MAX_RETRY_CTF = 40;				// CTF Offset message must be sent for 10 seconds (40 messages at 4Hz)


	enum class AutoZeroStatus : UCHAR
	{
		OFF = 0x00,
		ON = 0x01,
		UNSUPPORTED = 0xFF
	};

	enum class SensorType : UCHAR
	{
		UNKNOWN,
		POWER_ONLY,
		TORQUE_WHEEL,
		TORQUE_CRANK,
		CRANK_TORQUE_FREQ
	};

	enum class UpdateType : UCHAR
	{
		EVENT_SYNCH,
		TIME_SYNCH
	};

	enum class CrankLengthStatus : UCHAR
	{
		LENGTH_INVALID = 0x00,
		DEFAULT_LENGTH_USED = 0x01,
		LENGTH_MANUALLY_SET = 0x02,
		LENGTH_AUTOMATICALLY_SET = 0x03,
	};
	enum class SWMistmatchStatus : UCHAR
	{
		UNDEFINED = 0x00,
		RIGHT_SENSOR_OLDER = 0x01,
		LEFT_SENSOR_OLDER = 0x02,
		SW_SAME = 0x03,
	};
	enum class SensorStatus : UCHAR
	{
		UNDEFINED = 0x00,
		LEFT_PRESENT = 0x01,
		RIGHT_PRESENT = 0x02,
		LEFT_RIGHT_PRESENT = 0x03,
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
	SensorType eType;			// Type of sensor

	// Get/Set Crank Parameters Data Page
	UCHAR ucSubpageNumber;
	UCHAR ucCrankLength;
	UCHAR ucSensorStatus;
	UCHAR ucSensorCapabilities;

	// Standard Power-Only Main Data Page
	UCHAR ucPowEventCount;		// Power event count
	UCHAR ucCadence;			// Instantaneous crank cadence (rpm)
	UCHAR ucPedalPower;			// Pedal power (%)
	USHORT usAcumPower;			// Cumulative power (W)
	USHORT usPower;				// Instantaneous power (W)

	// Standard Wheel Torque Main Data Page
	UCHAR ucWTEventCount;		// Wheel torque event count
	UCHAR ucWheelTicks;			// Wheel revolutions
	USHORT usAcumWheelPeriod2048; // Cumulative wheel period (1/2048 s)
	USHORT usAcumTorque32;		// Cumulative torque (1/32 Nm)

	// Standard Crank Torque Main Data Page
	UCHAR ucCTEventCount;		// Crank torque event count
	UCHAR ucCrankTicks;			// Crank revolutions
	USHORT usAcumCrankPeriod2048;	// Cumulative crank period (1/2048 s)
	
	// Crank Torque Frequency Main Data Page
	UCHAR ucCTFEventCount;		// Rotation event count (pedal revolutions)
	USHORT usSlope10;			// Variation of output frequency (1/10 Hz/Nm)
	USHORT usTime2000;			// Time of most recent rotation event (1/2000 s)
	USHORT usTorqueTicks;		// Count of most recent torque event

   // Torque Effectiveness and Pedal Smoothness Main Data Page
   UCHAR ucLeftTorqueEffectiveness; // Left torque effectiveness (0.5%)
   UCHAR ucRightTorqueEffectiveness; // Right torque effectiveness (0.5%)
   UCHAR ucLeftPedalSmoothness;     // Left pedal smoothness (0.5%)
   UCHAR ucRightPedalSmoothness;    // Right pedal smoothness (0.5%)

	// General Calibration Response Main Data Page
	SHORT sCalibrationData;		// Calibration data (signed, no units)

	// Auto Zero Configuration Main Data Page
	// Torque Sensor Capabilities Main Data Page
	BOOL bAutoZeroEnable;		// Sensor description
	BOOL bAutoZeroOn;			// Auto zero status 
	SHORT sRawTorque;			// Raw torque counts (signed, no units)
	SHORT sOffsetTorque;		// Offset torque (signed, no units)

	// Torque-Frequency Defined Calibration Main Data Pages
	USHORT usCalOffset;			// Offset (100-1000)
	USHORT usCalSlope;			// Slope (100-500)
	USHORT usCalSerialNum;		// Serial number
	UCHAR ucAckedCTFMsg;		// CTF message acknowledged

	// General Calibration
	UCHAR ucRxCalibrationID;	// Calibration Message ID received
	UCHAR ucRxCTFMsgID;			// CTF Calibration Message ID received

	// Error handling
	BOOL bValidation;			// Turns validation on/off

private:
	UCHAR ucPowMsgCount;		// Power message count (to determine sensor type)
	UCHAR ucTorqueMsgCount;		// Torque message count (to determine sensor type)
	UCHAR ucPrevTorqueMsgCount;	// Previous torque message count
	static const UCHAR NUM_MSGS = 4;	// Maximum number of messages before deciding type of sensor


public:
	BikePower()
	{
		bValidation = FALSE;
		eType = SensorType::UNKNOWN;
		ucPowMsgCount = 0;
		ucTorqueMsgCount = 0;
		ucPrevTorqueMsgCount = 0;

		// Set cumulative values to 0
		usAcumTorque32 = 0;
		usAcumWheelPeriod2048 = 0;
		usAcumCrankPeriod2048 = 0;
		usAcumPower = 0;

		// Set event counters to 0
		ucPowEventCount = 0;
		ucWTEventCount = 0;
		ucWheelTicks = 0;
		ucCTEventCount = 0;
		ucCrankTicks = 0;
		ucCTFEventCount = 0;
		usTorqueTicks = 0;
	}

	~BikePower()
	{
	}


public:

/**************************************************************************
 * BikePower::Decode
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
			case PAGE_GET_SET_PARAMETERS:
				ucSubpageNumber = pucRxBuffer_[1];
				ucCrankLength = pucRxBuffer_[4];
				ucSensorStatus = pucRxBuffer_[5];
				ucSensorCapabilities = pucRxBuffer_[6];
				break;
			case PAGE_POWER:
				if(eType == SensorType::UNKNOWN || eType == SensorType::CRANK_TORQUE_FREQ)
				{
					if(++ucPowMsgCount >= NUM_MSGS)
					{
						ucPowMsgCount = 0;
						eType = SensorType::POWER_ONLY;	// After receiving NUM_MSGS successive power messages, determine it is a basic power sensor
					}
				}
				if(eType == SensorType::TORQUE_CRANK || eType == SensorType::TORQUE_WHEEL)
				{
					// If it is a torque sensor, and we stop receiving torque messages, set type to unknown
					if(ucPrevTorqueMsgCount == ucTorqueMsgCount)
					{
						eType = SensorType::UNKNOWN;
						ucPowMsgCount = 0;
						ucTorqueMsgCount = 0;
						ucPrevTorqueMsgCount = 0;
					}
					ucPrevTorqueMsgCount = ucTorqueMsgCount;
				}
				ucPowEventCount = pucRxBuffer_[1];		// Power event count
				ucPedalPower = pucRxBuffer_[2];			// Pedal Power
				ucCadence = pucRxBuffer_[3];			// Cadence
				usAcumPower = pucRxBuffer_[4] | pucRxBuffer_[5] << 8;	// Cumulative power (W)
				usPower = pucRxBuffer_[6] | pucRxBuffer_[7] << 8;		// Instantaneous power (W)
				//if(pucRxBuffer_[2] != RESERVED && bValidation)
					//throw gcnew Error(Error::Code::INVALID_RESERVED);	// Error: Invalid value in reserved field
				break;
			case PAGE_WHEEL_TORQUE:
				if(eType != SensorType::TORQUE_WHEEL)
				{
					eType = SensorType::TORQUE_WHEEL;	// Dealing with wheel torque sensor
					ucTorqueMsgCount = 0;			
					ucPrevTorqueMsgCount = 0;
				}
				ucTorqueMsgCount++;						// Update torque message count
				ucWTEventCount = pucRxBuffer_[1];		// Wheel torque event count
				ucWheelTicks = pucRxBuffer_[2];			// Wheel revolutions
				ucCadence = pucRxBuffer_[3];			// Cadence
				usAcumWheelPeriod2048 = pucRxBuffer_[4] | pucRxBuffer_[5] << 8;	// Cumulative wheel period (1/2048 s)
				usAcumTorque32 = pucRxBuffer_[6] | pucRxBuffer_[7] << 8;	// Cumulative torque (1/32 Nm)
				break;
			case PAGE_CRANK_TORQUE:
				if(eType != SensorType::TORQUE_CRANK)
				{
					eType = SensorType::TORQUE_CRANK;	// Dealing with crank torque sensor
					ucTorqueMsgCount = 0;				
					ucPrevTorqueMsgCount = 0;
				}
				ucTorqueMsgCount++;						// Update torque message count
				ucCTEventCount = pucRxBuffer_[1];		// Crank torque event count
				ucCrankTicks = pucRxBuffer_[2];			// Crank revolutions
				ucCadence = pucRxBuffer_[3];			// Cadence
				usAcumCrankPeriod2048 = pucRxBuffer_[4] | pucRxBuffer_[5] << 8;	// Cumulative crank period (1/2048 s)
				usAcumTorque32 = pucRxBuffer_[6] | pucRxBuffer_[7] << 8;	// Cumulative torque (1/32 Nm)
				break;
         case PAGE_TEPS:
            ucPowEventCount = pucRxBuffer_[1];		// Power event count
            ucLeftTorqueEffectiveness = pucRxBuffer_[2];
            ucRightTorqueEffectiveness = pucRxBuffer_[3];
            ucLeftPedalSmoothness = pucRxBuffer_[4];
            ucRightPedalSmoothness = pucRxBuffer_[5];
            break;
			case PAGE_CTF:
				eType = SensorType::CRANK_TORQUE_FREQ;	// Dealing with crank torque frequency sensor
				ucCTFEventCount = pucRxBuffer_[1];		// Crank torque frequency event count (pedal revolutions)
				usSlope10 = pucRxBuffer_[3] | pucRxBuffer_[2] << 8;		// Variation of output frequency (10 Hz/Nm)
				usTime2000 = pucRxBuffer_[5] | pucRxBuffer_[4] << 8;	// Time of most recent rotation event (1/2000 s)
				usTorqueTicks = pucRxBuffer_[7] | pucRxBuffer_[6] << 8;	// Count of most recent torque event
				break;
			case PAGE_CALIBRATION:
				ucRxCalibrationID = pucRxBuffer_[1];	// Current calibration page received
				switch(ucRxCalibrationID)
				{
					case CAL_TORQUE_METER_CAPABILITIES:
						bAutoZeroEnable = (BOOL) (pucRxBuffer_[2] & 0x01);		// Auto Zero Enabled
						bAutoZeroOn = (BOOL) ((pucRxBuffer_[2] & 0x02) >> 1);	// Auto Zero Status
						sRawTorque = pucRxBuffer_[3] | pucRxBuffer_[4] << 8;	// Raw torque (signed)
						sOffsetTorque = pucRxBuffer_[5] | pucRxBuffer_[6] << 8;	// Offset torque (signed)
						if(bValidation && ((pucRxBuffer_[2] & 0xFC) != RESERVED_DESCR || pucRxBuffer_[7] != RESERVED))
							throw gcnew Error(Error::Code::INVALID_RESERVED);	// Error: Invalid value on reserved field
						break;
					case CAL_REQUEST:
						if(bValidation && (pucRxBuffer_[2] != RESERVED || pucRxBuffer_[3] != RESERVED || pucRxBuffer_[4] != RESERVED || pucRxBuffer_[5] != RESERVED || pucRxBuffer_[6] != RESERVED || pucRxBuffer_[7] != RESERVED))
							throw gcnew Error(Error::Code::INVALID_RESERVED);	// Error: Invalid value on reserved field
						break;
					case CAL_AUTOZERO_CONFIG:
					{
						BOOL bUndefAZ = FALSE;
						switch((AutoZeroStatus) pucRxBuffer_[2])
						{
							case AutoZeroStatus::OFF:
								bAutoZeroEnable = TRUE;
								bAutoZeroOn = FALSE;
								break;
							case AutoZeroStatus::ON:
								bAutoZeroEnable = TRUE;
								bAutoZeroOn = TRUE;
								break;
							case AutoZeroStatus::UNSUPPORTED:
								bAutoZeroEnable = FALSE;
								bAutoZeroOn = FALSE;
								break;
							default:
								bUndefAZ = TRUE;
								break;
						}
						if(bValidation)
						{
							if(pucRxBuffer_[3] != RESERVED || pucRxBuffer_[4] != RESERVED || pucRxBuffer_[5] != RESERVED || pucRxBuffer_[6] != RESERVED || pucRxBuffer_[7] != RESERVED)
							{
								if(bUndefAZ)
									throw gcnew Error(Error::Code::INVALID_RESERVED, Error::Code::UNDEF_AUTOZERO);	// Error: Bad auto zero status and reserved field
								else
									throw gcnew Error(Error::Code::INVALID_RESERVED);	// Error: bad reserved field
							}
							else if(bUndefAZ)
								throw gcnew Error(Error::Code::UNDEF_AUTOZERO);	// Error: undefined auto zero status
						}
						break;
					}
					case CAL_SUCCESS:
					case CAL_FAIL:		// intentional fall thru
					{
						sCalibrationData = pucRxBuffer_[6] | pucRxBuffer_[7] << 8;	// Calibration data (signed)
						BOOL bUndefAZ = FALSE;
						switch((AutoZeroStatus) pucRxBuffer_[2])
						{
							case AutoZeroStatus::OFF:
								bAutoZeroEnable = TRUE;
								bAutoZeroOn = FALSE;
								break;
							case AutoZeroStatus::ON:
								bAutoZeroEnable = TRUE;
								bAutoZeroOn = TRUE;
								break;
							case AutoZeroStatus::UNSUPPORTED:
								bAutoZeroEnable = FALSE;
								bAutoZeroOn = FALSE;
								break;
							default:
								bUndefAZ = TRUE;
								break;
						}
						if(bValidation)
						{
							if(pucRxBuffer_[3] != RESERVED || pucRxBuffer_[4] != RESERVED || pucRxBuffer_[5] != RESERVED)
							{
								if(bUndefAZ)
									throw gcnew Error(Error::Code::INVALID_RESERVED, Error::Code::UNDEF_AUTOZERO);	// Error: Bad auto zero status and reserved field
								else
									throw gcnew Error(Error::Code::INVALID_RESERVED);	// Error: bad reserved field
							}
							else if(bUndefAZ)
								throw gcnew Error(Error::Code::UNDEF_AUTOZERO);	// Error: invalid auto zero status
						}
						break;
					}
					case CAL_CTF:
						ucRxCTFMsgID = pucRxBuffer_[2];
						switch(ucRxCTFMsgID)
						{
						case CTF_OFFSET:
							usCalOffset = pucRxBuffer_[7] | pucRxBuffer_[6] << 8;
							if(bValidation && (pucRxBuffer_[3] != RESERVED || pucRxBuffer_[4] != RESERVED || pucRxBuffer_[5] != RESERVED))
								throw gcnew Error(Error::Code::INVALID_RESERVED); // Error: bad reserved fields
							break;
						case CTF_SLOPE:
							usCalSlope = pucRxBuffer_[7] | pucRxBuffer_[6] << 8;
							if(bValidation && (pucRxBuffer_[3] != RESERVED || pucRxBuffer_[4] != RESERVED || pucRxBuffer_[5] != RESERVED))
								throw gcnew Error(Error::Code::INVALID_RESERVED);	// Error: bad reserved field
							break;
						case CTF_SERIAL:
							usCalSerialNum = pucRxBuffer_[7] | pucRxBuffer_[6] << 8;
							if(bValidation && (pucRxBuffer_[3] != RESERVED || pucRxBuffer_[4] != RESERVED || pucRxBuffer_[5] != RESERVED))
								throw gcnew Error(Error::Code::INVALID_RESERVED);	// Error: bad reserved field
							break;
						case CTF_ACK:
							ucAckedCTFMsg = pucRxBuffer_[3];
							if(bValidation)
							{
								if(pucRxBuffer_[4] != RESERVED || pucRxBuffer_[5] != RESERVED || pucRxBuffer_[6] != RESERVED || pucRxBuffer_[7] != RESERVED)
								{
									if(ucAckedCTFMsg != CTF_SLOPE && ucAckedCTFMsg != CTF_SERIAL)
										throw gcnew Error(Error::Code::INVALID_RESERVED, Error::Code::UNDEF_CTF_ACKED);	// Error: bad reserved field and unsupported acked message
									else
										throw gcnew Error(Error::Code::INVALID_RESERVED);	// Error: bad reserved field
								}
								else if(ucAckedCTFMsg != CTF_SLOPE && ucAckedCTFMsg != CTF_SERIAL)
									throw gcnew Error(Error::Code::UNDEF_CTF_ACKED);	// Error: unsupported acked message
							}
							break;
						default:
							if(bValidation)
								throw gcnew Error(Error::Code::UNDEF_CTF_ID);	// Error: Undefined CTF message
							break;
						}
						break;

					default:
						if(bValidation)
							throw gcnew Error(Error::Code::UNDEF_CAL_ID);	// Error: Undefined calibration message
						break;
				}
				break;

			default:
				if(bValidation)
					throw gcnew Error(Error::Code::UNDEF_PAGE);		// Error: Undefined page
				break;
		}
	}
	

/**************************************************************************
 * BikePower::EncodeMainData
 * 
 * Encodes main bike power data pages:
 * - Basic Power
 * - Wheel Torque
 * - Crank Torque
 * - Crank Torque Frequency
 *
 * Exceptions are thrown when dealing with invalid data
 *
 * ucPageNum_: number of page to encode
 * pucTxBuffer_: pointer to the buffer that will store the encoded data
 * 
 * returns: N/A
 *
 **************************************************************************/
	void EncodeMainData(UCHAR ucPageNum_, UCHAR* pucTxBuffer_)
	{
		switch(ucPageNum_)
		{
			case PAGE_GET_SET_PARAMETERS:
				pucTxBuffer_[0] = ucPageNum_;
				pucTxBuffer_[1] = ucSubpageNumber;
				pucTxBuffer_[2] = RESERVED;
				pucTxBuffer_[3] = RESERVED;
				pucTxBuffer_[4] = ucCrankLength;
				pucTxBuffer_[5] = ucSensorStatus;
				pucTxBuffer_[6] = ucSensorCapabilities;
				pucTxBuffer_[7] = RESERVED;
				break;
			case PAGE_POWER:
				pucTxBuffer_[0] = ucPageNum_;
				pucTxBuffer_[1] = ucPowEventCount;		// Power event count
				pucTxBuffer_[2] = ucPedalPower;			// Pedal Power
				pucTxBuffer_[3] = ucCadence;			// Cadence
				pucTxBuffer_[4] = usAcumPower & 0xFF;	// Cumulative power (W)
				pucTxBuffer_[5] = (usAcumPower >> 8) & 0xFF;
				pucTxBuffer_[6] = usPower & 0xFF;		// Instantaneous power (W)
				pucTxBuffer_[7] = (usPower >> 8) & 0xFF;
				break;
			case PAGE_WHEEL_TORQUE:
				pucTxBuffer_[0] = ucPageNum_;
				pucTxBuffer_[1] = ucWTEventCount;		// Wheel torque event count
				pucTxBuffer_[2] = ucWheelTicks;			// Wheel revolutions
				pucTxBuffer_[3] = ucCadence;			// Cadence
				pucTxBuffer_[4] = usAcumWheelPeriod2048 & 0xFF;  // Cumulative wheel period (1/2048 s)
				pucTxBuffer_[5] = (usAcumWheelPeriod2048 >> 8) & 0xFF;
				pucTxBuffer_[6] = usAcumTorque32 & 0xFF;		// Cumulative torque (1/32 Nm)
				pucTxBuffer_[7] = (usAcumTorque32 >> 8) & 0xFF;
				break;
			case PAGE_CRANK_TORQUE:
				pucTxBuffer_[0] = ucPageNum_;
				pucTxBuffer_[1] = ucCTEventCount;		// Crank torque event count
				pucTxBuffer_[2] = ucCrankTicks;			// Crank revolutions
				pucTxBuffer_[3] = ucCadence;			// Cadence
				pucTxBuffer_[4] = usAcumCrankPeriod2048 & 0xFF;		// Cumulative crank period (1/2048 s)
				pucTxBuffer_[5] = (usAcumCrankPeriod2048 >> 8) & 0xFF;
				pucTxBuffer_[6] = usAcumTorque32 & 0xFF;		// Cumulative torque (1/32 Nm)
				pucTxBuffer_[7] = (usAcumTorque32 >> 8) & 0xFF;
				break;
         case PAGE_TEPS:
            pucTxBuffer_[0] = ucPageNum_;
            pucTxBuffer_[1] = ucPowEventCount;     // tied to event counter in power only page
            pucTxBuffer_[2] = ucLeftTorqueEffectiveness;
            pucTxBuffer_[3] = ucRightTorqueEffectiveness;
            pucTxBuffer_[4] = ucLeftPedalSmoothness;
            pucTxBuffer_[5] = ucRightPedalSmoothness;
            pucTxBuffer_[6] = RESERVED;
            pucTxBuffer_[7] = RESERVED;
            break;
			case PAGE_CTF:
				pucTxBuffer_[0] = ucPageNum_;
				pucTxBuffer_[1] = ucCTFEventCount;		// Crank torque frequency event count (pedal revolutions)
				pucTxBuffer_[2] = (usSlope10 >> 8) & 0xFF;	// Variation of output frequency (10 Hz/Nm)
				pucTxBuffer_[3] = usSlope10 & 0xFF;
				pucTxBuffer_[4] = (usTime2000 >> 8) & 0xFF;	// Time of most recent rotation event (1/2000 s)
				pucTxBuffer_[5] = usTime2000 & 0xFF;
				pucTxBuffer_[6] = (usTorqueTicks >> 8) & 0xFF;	// Count of most recent torque event
				pucTxBuffer_[7] = usTorqueTicks & 0xFF;
				break;
			default:
				if(bValidation)
					throw gcnew Error(Error::Code::UNDEF_PAGE);		// Error: Undefined page
				break;
		}
	}

/**************************************************************************
 * BikePower::EncodeAutoZeroSupport
 * 
 * Encodes the auto zero support (torque meter capabilities) page (0x12)
 *
 * pucTxBuffer_: pointer to the buffer that will store the encoded data
 * 
 * returns: N/A
 *
 **************************************************************************/
	void EncodeAutoZeroSupport(UCHAR* pucTxBuffer_)
	{
		pucTxBuffer_[0] = PAGE_CALIBRATION;
		pucTxBuffer_[1] = CAL_TORQUE_METER_CAPABILITIES;
		pucTxBuffer_[2] = (UCHAR) bAutoZeroEnable;		// Auto Zero Enabled (bit 0)
		pucTxBuffer_[2] |= (UCHAR) bAutoZeroOn  << 1;	// Auto Zero Status (bit 1)
		pucTxBuffer_[3] = sRawTorque & 0xFF;			// Raw torque (LSB)
		pucTxBuffer_[4] = (sRawTorque >> 8) & 0xFF;		// Raw torque (MSB)
		pucTxBuffer_[5] = sOffsetTorque & 0xFF;			// Offset torque (LSB)
		pucTxBuffer_[6] = (sOffsetTorque >> 8) & 0xFF;	// Offset torque (MSB)
		pucTxBuffer_[7] = RESERVED;
	}


/**************************************************************************
 * BikePower::EncodeManualCalibrationRequest
 * 
 * Encodes a manual calibration request (0xAA)
 *
 * pucTxBuffer_: pointer to the buffer that will store the encoded data
 * 
 * returns: N/A
 *
 **************************************************************************/
	void EncodeManualCalibrationRequest(UCHAR* pucTxBuffer_)
	{		
		pucTxBuffer_[0] = PAGE_CALIBRATION;
		pucTxBuffer_[1] = CAL_REQUEST;
		pucTxBuffer_[2] = RESERVED;
		pucTxBuffer_[3] = RESERVED;
		pucTxBuffer_[4] = RESERVED;
		pucTxBuffer_[5] = RESERVED;
		pucTxBuffer_[6] = RESERVED;
		pucTxBuffer_[7] = RESERVED;
	}


/**************************************************************************
 * BikePower::EncodeAZCalibrationRequest
 * 
 * Encodes an auto zero calibration request (0xAB)
 *
 * bAutoZeroEnabled_ = TRUE if AutoZero is supported, FALSE otherwise
 * bAutoZeroOn_ = TRUE if AutoZero is ON, FALSE otherwise
 * pucTxBuffer_: pointer to the buffer that will store the encoded data
 * 
 * returns: N/A
 *
 **************************************************************************/
	void EncodeAZCalibrationRequest(BOOL bAutoZeroEnabled_, BOOL bAutoZeroOn_, UCHAR* pucTxBuffer_)
	{
		pucTxBuffer_[0] = PAGE_CALIBRATION;
		pucTxBuffer_[1] = CAL_AUTOZERO_CONFIG;
		if(!bAutoZeroEnabled_)
			pucTxBuffer_[2] = (UCHAR) AutoZeroStatus::UNSUPPORTED;	 // Encode auto zero status
		else if(bAutoZeroOn_)
			pucTxBuffer_[2] = (UCHAR) AutoZeroStatus::ON;
		else
			pucTxBuffer_[2] = (UCHAR) AutoZeroStatus::OFF;
		pucTxBuffer_[3] = RESERVED;
		pucTxBuffer_[4] = RESERVED;
		pucTxBuffer_[5] = RESERVED;
		pucTxBuffer_[6] = RESERVED;
		pucTxBuffer_[7] = RESERVED;
	}

/**************************************************************************
 * BikePower::EncodeCalibrationResponse
 * 
 * Encodes a calibration response (0xAC or 0xAF)
 *
 * ucCalResponseID_: response to send (either CAL_FAIL or CAL_SUCCESS)
 * bAutoZeroEnabled_ = TRUE if AutoZero is supported, FALSE otherwise
 * bAutoZeroOn_ = TRUE if AutoZero is ON, FALSE otherwise
 * sCalData_ = calibration data to send in response
 * pucTxBuffer_: pointer to the buffer that will store the encoded data
 * 
 * returns: N/A
 *
 **************************************************************************/
	void EncodeCalibrationResponse(UCHAR ucCalResponseID_, BOOL bAutoZeroEnabled_, BOOL bAutoZeroOn_, SHORT sCalData_, UCHAR* pucTxBuffer_)
	{
		pucTxBuffer_[0] = PAGE_CALIBRATION;
		pucTxBuffer_[1] = ucCalResponseID_;
		if(!bAutoZeroEnabled_)
			pucTxBuffer_[2] = (UCHAR) AutoZeroStatus::UNSUPPORTED;	// Encode auto zero status
		else if(bAutoZeroOn_)
			pucTxBuffer_[2] = (UCHAR) AutoZeroStatus::ON;
		else
			pucTxBuffer_[2] = (UCHAR) AutoZeroStatus::OFF;
		pucTxBuffer_[3] = RESERVED;
		pucTxBuffer_[4] = RESERVED;
		pucTxBuffer_[5] = RESERVED;
		pucTxBuffer_[6] = sCalData_ & 0xFF;				// Calibration data, low byte
		pucTxBuffer_[7] = (sCalData_ >> 8) & 0x0FF;		// Calibration data, high byte
	}


/**************************************************************************
 * BikePower::EncodeCTFCalibrationPage
 * 
 * Encodes a CTF calibration page
 *
 * ucCTFMsgID_: ID of CTF defined message; either CTF_OFFSET, CTF_SLOPE or CTF_SERIAL
 * usParam_: parameter to include in page (offset, slope or serial number)
 * pucTxBuffer_: pointer to the buffer that will store the encoded data
 * 
 * returns: N/A
 *
 **************************************************************************/
	void EncodeCTFCalibrationPage(UCHAR ucCTFMsgID_, USHORT usParam_, UCHAR* pucTxBuffer_)
	{
		switch(ucCTFMsgID_)
		{
			case CTF_OFFSET:
			case CTF_SLOPE:
			case CTF_SERIAL:	// Intentional fall thru
				pucTxBuffer_[0] = PAGE_CALIBRATION;
				pucTxBuffer_[1] = CAL_CTF;
				pucTxBuffer_[2] = ucCTFMsgID_;
				pucTxBuffer_[3] = RESERVED;
				pucTxBuffer_[4] = RESERVED;
				pucTxBuffer_[5] = RESERVED;
				pucTxBuffer_[6] = (usParam_ >> 8) & 0xFF;	// CTF parameter (offset, slope, serial number), high byte
				pucTxBuffer_[7] = usParam_ & 0xFF;			// CTF parameter (offset, slope, serial number), low byte
				break;
			case CTF_ACK:
				pucTxBuffer_[0] = PAGE_CALIBRATION;
				pucTxBuffer_[1] = CAL_CTF;
				pucTxBuffer_[2] = ucCTFMsgID_;
				pucTxBuffer_[3] = (UCHAR) usParam_;			// parameter in this case is message code to acknowledge
				pucTxBuffer_[4] = RESERVED;
				pucTxBuffer_[5] = RESERVED;
				pucTxBuffer_[6] = RESERVED;
				pucTxBuffer_[7] = RESERVED;
				break;
			default:
				break;
		}
	}


/**************************************************************************
 * BikePower::IsCadenceInvalid
 * 
 * Checks if the cadence value is invalid
 *
 * ucCadence_: cadence (rpm)
 * 
 * returns: N/A
 *
 **************************************************************************/
	BOOL IsCadenceInvalid(UCHAR ucCadence_)
	{
		if(ucCadence_ == INVALID_CADENCE)
			return TRUE;
		else
			return FALSE;		
	}

/**************************************************************************
 * BikePower::IsSlopeInvalid
 * 
 * Checks if the slope is within the boundaries defined in the device profile
 *
 * usSlope10_: slope (1/10 Nm/Hz)
 * 
 * returns: N/A
 *
 **************************************************************************/
	BOOL IsSlopeInvalid(USHORT usSlope10_)
	{
		if(usSlope10_ < MIN_SLOPE || usSlope10_ > MAX_SLOPE)
			return TRUE;
		else
			return FALSE;
	}


/**************************************************************************
 * BikePower::IsRawTorqueInvalid
 * 
 * Checks if the raw torque is invalid
 *
 * sRawTorque: raw torque
 * 
 * returns: N/A
 *
 **************************************************************************/
	BOOL IsRawTorqueInvalid(SHORT sRawTorque_)
	{
		if(sRawTorque_ == INVALID_TORQUE)
			return TRUE;
		else
			return FALSE;
	}


/**************************************************************************
 * BikePower::IsOffsetTorqueInvalid
 * 
 * Checks if the offset torque is invalid
 *
 * sOffsetTorque_: offset torque
 * 
 * returns: N/A
 *
 **************************************************************************/
	BOOL IsOffsetTorqueInvalid(SHORT sOffsetTorque_)
	{
		if(sOffsetTorque_ == INVALID_TORQUE)
			return TRUE;
		else
			return FALSE;
	}

/**************************************************************************
 * BikePower::IsOffsetInvalid
 * 
 * Checks if the offset is within the boundaries defined in the device profile
 *
 * usCalOffset_: offset
 * 
 * returns: N/A
 *
 **************************************************************************/
	BOOL IsOffsetInvalid(USHORT usCalOffset_)
	{
		if(usCalOffset_ < MIN_OFFSET || usCalOffset_ > MAX_OFFSET)
			return TRUE;
		else
			return FALSE;
	}

};