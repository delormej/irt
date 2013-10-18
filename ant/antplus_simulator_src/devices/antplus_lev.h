/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/


#pragma once

#include "types.h"
#define LEV_REV				0.5					// Device Profile Revision Number that this was written to

public ref class LEV
{

public:
	// Channel Parameters
	static const UCHAR DEVICE_TYPE = 20;
	static const UCHAR TX_TYPE = 5;
	static const USHORT MSG_PERIOD = 8192;	// 4 Hz

	// Data Pages
	static const UCHAR PAGE1_SPEED_SYSTEM1 = 0x01;
	static const UCHAR PAGE2_SPEED_DISTANCE = 0x02;
	static const UCHAR PAGE3_SPEED_SYSTEM2 = 0x03;
	static const UCHAR PAGE4_BATTERY = 0x04;
	static const UCHAR PAGE5_CAPABILITES = 0x05;
	static const UCHAR PAGE16_DISPLAY_DATA = 0x10;
	static const UCHAR PAGE34_SPEED_DISTANCE = 0x22;
	static const UCHAR PAGE70_REQUEST = 0x46;

	// Acknowledged Messages
	static const UCHAR ACK_FAIL = 0;
	static const UCHAR ACK_SUCCESS = 1;
	static const UCHAR ACK_RETRY = 2;
	static const UCHAR MAX_RETRIES = 10;		// defines the max number of times to retry before failing

	// Reserved/invalid values
	static const UCHAR RESERVED = 0xFF;
	
	// Bit Masks
	static const UCHAR BYTE_MASK = 0xFF;

	// Defines how often to send common pages (every 20th message)
	static const UCHAR LEV_COMMON_INTERVAL = 20;	
	

public:

	// LEV Data Page 1
	UCHAR ucTemperature;			  // Indicates current motor / battery temperature (0 = unsued)
	UCHAR ucTravelModeState;		  // Indicates the LEV's current travel mode
	UCHAR ucSystemState;			  // Indicates the current state of the LEV's peripheral sustem
	UCHAR ucGearState;				  // Indicates the current state of the LEV's gears
	UCHAR ucErrorMessage;			  // Indicates LEV error
	USHORT usCurrentLEVSpeed;	      // Instantaneous speed (0.1 km/hr), shared between page1 and page2

	// LEV Data Page 2
	ULONG ulOdometer;				  // 3 bytes, accumulated total distance (0.01 km)
	USHORT usRemainingRange;		  // Remaining range of the LEV battery (km)

	// LEV Data Page 3
	UCHAR ucBatteryCharge;			  // State of battery charge (0-100%)
	UCHAR ucPercentAssist;			  // Percent assistance the LEV motor is providing (0-100%)	

	// LEV Data Page 4
	USHORT usChargeCycleCount;		  // Tracks the number of charge cycles the LEV has gone through
	USHORT usFuelConsumption;		  // Tracks the current rate of battery fuel consumption in watt-hours per km
	UCHAR ucBatteryVoltage;			  // Measured battery voltage (0 = unsued)
	USHORT usDistanceOnCharge;		  // Tracks the distance in km the LEV has traveled on the current charge	

	// LEV Data Page 5
	UCHAR ucSupportedTravelModes;     // Travel Modes Supported bit field
	USHORT usWheelCircumference;	  // Holds the wheel cirumference of the LEV in mm

	// LEV Data Page 16
	USHORT usP16WheelCircum;		  // Used to set the wheel circumference
	UCHAR ucP16TravelMode;			  // Indicates the current travel mode set/requested
	USHORT usP16DisplayCommand;		  // Command sent from the Display to the LEV
	USHORT usP16ManID;				  // Manufacturer ID of the Display

	//LEV Data Page 34
	ULONG ulP34Odometer;				  // Accumulated total distance travelled
	USHORT usP34FuelConsumption;		  // Indicates current rate of battery fuel consumption in watt-hrs per km. 
	USHORT usSpeed;					  // Current LEV speed

	// LEV Data Page 70
	UCHAR ucP70TxTimes;				      // number of times the sensor should send the requested pages (broadcast only supported on LEV)
    UCHAR ucP70RequestedPage;			  // holds the page number to be requested


public:
	LEV()
	{
		// initialize variables to invalid / zero
		ucTravelModeState = 0;
		ucSystemState = 0;
		ucGearState = 0;
		ucErrorMessage = 0;
		usCurrentLEVSpeed = 0;
		ucPercentAssist = 0;
		ulOdometer = 0;
		ucTemperature = 0;
		ucBatteryCharge = 0;
		usRemainingRange = 0;
		usChargeCycleCount = 0;
		usFuelConsumption = 0;
		ucBatteryVoltage = 0;
		usDistanceOnCharge = 0;
		ucSupportedTravelModes = 0;
		usWheelCircumference = 0;
		usP16WheelCircum = 0xFF;
		ucP16TravelMode = 0xFF;
		usP16DisplayCommand = 0;
		usP16ManID = 0;
		ulP34Odometer = 0;
		usP34FuelConsumption = 0;
		usSpeed = 0;
		ucP70TxTimes = 0;
		ucP70RequestedPage = 0;
	}

	~LEV()
	{
	}

public:

/**************************************************************************
 * LEV::Decode
 * 
 * Decodes all main data pages 
 * Exceptions are thrown when dealing with non compliant pages
 * Further breakdowns inside the fields are dealt with in the display method
 *
 * pucRxBuffer_: pointer to the buffer containing the received data
 * 
 * returns: N/A
 *
 **************************************************************************/
	void Decode(UCHAR* pucRxBuffer_)
	{
		ULONG ulTempHolder = 0;

		switch(pucRxBuffer_[0])
		{
			case PAGE1_SPEED_SYSTEM1:
				ucTemperature = pucRxBuffer_[1];
				ucTravelModeState = pucRxBuffer_[2];			
				ucSystemState = pucRxBuffer_[3];
				ucGearState = pucRxBuffer_[4];
				ucErrorMessage = pucRxBuffer_[5];
				usCurrentLEVSpeed = pucRxBuffer_[6] | ((pucRxBuffer_[7] & 0x0F) << 8);		// mask off the top bits which are reserved
	            break;

			case PAGE2_SPEED_DISTANCE:				
				ulOdometer = pucRxBuffer_[1];                    // odometer LSB
                ulTempHolder = pucRxBuffer_[2] & 0xFF;           // odometer 
                ulOdometer = ulOdometer | (ulTempHolder << 8);
                ulTempHolder = pucRxBuffer_[3] & 0xFF;           // odometer MSB
                ulOdometer = ulOdometer | (ulTempHolder << 16);
				usRemainingRange = pucRxBuffer_[4] | ((pucRxBuffer_[5] & 0x0F) << 8);       // mask off top bits of byte 5 which are reserved
				usCurrentLEVSpeed = pucRxBuffer_[6] | ((pucRxBuffer_[7] & 0x0F) << 8);		// mask off the top bits which are reserved
	            break;

			case PAGE3_SPEED_SYSTEM2:
				ucBatteryCharge = pucRxBuffer_[1];
				ucTravelModeState = pucRxBuffer_[2];			
				ucSystemState = pucRxBuffer_[3];
				ucGearState = pucRxBuffer_[4];
				ucPercentAssist = pucRxBuffer_[5];
				usCurrentLEVSpeed = pucRxBuffer_[6] | ((pucRxBuffer_[7] & 0x0F) << 8);		// mask off the top bits which are reserved
				break;

			case PAGE4_BATTERY:
				usChargeCycleCount = pucRxBuffer_[2] | ((pucRxBuffer_[3] & 0x0F) << 8);        // mask off top bits of byte 3 which are part of fuel consumption
				usFuelConsumption = pucRxBuffer_[4] | (((pucRxBuffer_[3] & 0xF0) >> 4) << 8);  // LSB of fuel consumption is byte 4
				ucBatteryVoltage = pucRxBuffer_[5];
				usDistanceOnCharge = pucRxBuffer_[6] | (pucRxBuffer_[7] << 8);
				break;

			case PAGE5_CAPABILITES:
				ucSupportedTravelModes = pucRxBuffer_[2];		
				usWheelCircumference = pucRxBuffer_[3] | ((pucRxBuffer_[4] & 0x0F) << 8);
				break;

			case PAGE16_DISPLAY_DATA:
				usP16WheelCircum = pucRxBuffer_[1] | ((pucRxBuffer_[2] & 0x0F) << 8);
				ucP16TravelMode = pucRxBuffer_[3];
				usP16DisplayCommand = pucRxBuffer_[4] | (pucRxBuffer_[5] << 8);
				usP16ManID = pucRxBuffer_[6] | (pucRxBuffer_[7] << 8);
				break;

			case PAGE34_SPEED_DISTANCE:
				ulP34Odometer = pucRxBuffer_[1] | ((pucRxBuffer_[2] & 0xFF) << 8 ) | ((pucRxBuffer_[3] & 0xFF) << 16);
				usP34FuelConsumption = (pucRxBuffer_[4] | ((pucRxBuffer_[5] & 0xFF) << 8)) & 0x0FFF;
				usSpeed = (pucRxBuffer_[6] | ((pucRxBuffer_[7] & 0xFF) << 8)) & 0x0FFF;
				break;

			default:
				break;
		}
	}

/**************************************************************************
 * LEV::EncodeData
 * 
 * Encodes the LEV data pages:
 * - Data Page 1 - Speed and System Information 1
 * - Data Page 2 - Speed and Distance Information
 * - Data Page 3 - Speed and System Information 2
 * - Data Page 4 - Battery Info
 * - Data Page 5 - Capabilities
 * - Data Page 16 - Display Data
 * - Data Page 70 - Request Data
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
		switch(ucPageNum_)
		{
			case PAGE1_SPEED_SYSTEM1:
				pucTxBuffer_[0] = ucPageNum_;					// page number
				pucTxBuffer_[1] = ucTemperature;				// current temperature state
				pucTxBuffer_[2] = ucTravelModeState;		    // current travel mode
				pucTxBuffer_[3] = ucSystemState;				// current state of peripheral system
				pucTxBuffer_[4] = ucGearState;					// current state of gears
				pucTxBuffer_[5] = ucErrorMessage;				// LEV error
				pucTxBuffer_[6] = usCurrentLEVSpeed & BYTE_MASK;		// mask off top byte
				pucTxBuffer_[7] = (usCurrentLEVSpeed >> 8) & 0x0FFF;	// shift top down and mask off top 
				break;

			case PAGE2_SPEED_DISTANCE:
				pucTxBuffer_[0] = ucPageNum_;
				pucTxBuffer_[1] = ulOdometer & BYTE_MASK;				// accum distance
				pucTxBuffer_[2] = (ulOdometer >> 8) & BYTE_MASK;
				pucTxBuffer_[3] = (ulOdometer >> 16) & BYTE_MASK;
				pucTxBuffer_[4] = usRemainingRange & BYTE_MASK;			// mask off top byte
				pucTxBuffer_[5] = (usRemainingRange >> 8) & 0x0FFF;	    // shift top down and mask off top
				pucTxBuffer_[6] = usCurrentLEVSpeed & BYTE_MASK;		// mask off top byte
				pucTxBuffer_[7] = (usCurrentLEVSpeed >> 8) & 0x0FFF;	// shift top down and mask off top 
				break;

			case PAGE3_SPEED_SYSTEM2:
				pucTxBuffer_[0] = ucPageNum_;
				pucTxBuffer_[1] = ucBatteryCharge;				// battery charge
				pucTxBuffer_[2] = ucTravelModeState;		    // current travel mode
				pucTxBuffer_[3] = ucSystemState;				// current state of peripheral system
				pucTxBuffer_[4] = ucGearState;					// current state of gears
				pucTxBuffer_[5] = ucPercentAssist;				// percent assist of motor
				pucTxBuffer_[6] = usCurrentLEVSpeed & BYTE_MASK;		// mask off top byte
				pucTxBuffer_[7] = (usCurrentLEVSpeed >> 8) & 0x0FFF;	// shift top down and mask off top 
				break;

			case PAGE4_BATTERY:
				pucTxBuffer_[0] = ucPageNum_;					        // page number
				pucTxBuffer_[1] = RESERVED;                  
				pucTxBuffer_[2] = usChargeCycleCount & BYTE_MASK;		// mask off top byte
				pucTxBuffer_[3] = ((usChargeCycleCount >> 8) | (usFuelConsumption >> 4) & 0xF0);  // msn for both                      
				pucTxBuffer_[4] = usFuelConsumption & BYTE_MASK;		// mask off top byte
				pucTxBuffer_[5] = ucBatteryVoltage;						// battery voltage
				pucTxBuffer_[6] = usDistanceOnCharge & BYTE_MASK;		// mask off top byte
				pucTxBuffer_[7] = (usDistanceOnCharge >> 8) & BYTE_MASK;	// shift top down and mask off top 
				break;

			case PAGE5_CAPABILITES:
				pucTxBuffer_[0] = ucPageNum_;					// page number
				pucTxBuffer_[1] = RESERVED;
				pucTxBuffer_[2] = ucSupportedTravelModes;    	// travel modes supported
				pucTxBuffer_[3] = usWheelCircumference & BYTE_MASK;		 // mask off top byte
				pucTxBuffer_[4] = (usWheelCircumference >> 8) & 0x0FFF;	 // shift top down and mask off top 
				pucTxBuffer_[5] = RESERVED;
				pucTxBuffer_[6] = RESERVED;
				pucTxBuffer_[7] = RESERVED;
				break;

			case PAGE16_DISPLAY_DATA:
				pucTxBuffer_[0] = ucPageNum_;
				pucTxBuffer_[1] = usP16WheelCircum & BYTE_MASK;		        // mask off top byte
				pucTxBuffer_[2] = (usP16WheelCircum >> 8) & 0x0FFF;	        // shift top down and mask off top 
				pucTxBuffer_[3] = ucP16TravelMode;							// current travel mode
				pucTxBuffer_[4] = usP16DisplayCommand & BYTE_MASK;			// mask off top byte
				pucTxBuffer_[5] = (usP16DisplayCommand >> 8) & BYTE_MASK;	// shift top down and mask off top
				pucTxBuffer_[6] = usP16ManID & BYTE_MASK;					// mask off top byte
				pucTxBuffer_[7] = (usP16ManID >> 8) & BYTE_MASK;			// shift top down and mask off top
				break;

			case PAGE34_SPEED_DISTANCE:
				pucTxBuffer_[0] = ucPageNum_;
				pucTxBuffer_[1] = ulP34Odometer & BYTE_MASK;
				pucTxBuffer_[2] = (ulP34Odometer >> 8) & BYTE_MASK;
				pucTxBuffer_[3] = (ulP34Odometer >> 16) & BYTE_MASK;
				pucTxBuffer_[4] = usP34FuelConsumption & BYTE_MASK;
				pucTxBuffer_[5] = ((usFuelConsumption >> 8) & BYTE_MASK) | 0xF0;
				pucTxBuffer_[6] = usSpeed & BYTE_MASK;
				pucTxBuffer_[7] = ((usSpeed >> 8) & BYTE_MASK) | 0xF0;
				break;

			case PAGE70_REQUEST:
				pucTxBuffer_[0] = ucPageNum_;
				pucTxBuffer_[1] = RESERVED;
				pucTxBuffer_[2] = RESERVED;
				pucTxBuffer_[3] = RESERVED;
				pucTxBuffer_[4] = RESERVED;
				pucTxBuffer_[5] = ucP70TxTimes;								// number of times the sensor should send the requested pages (broadcast only supported on LEV)
				pucTxBuffer_[6] = ucP70RequestedPage;						// this is the page number that the display is requesting
				pucTxBuffer_[7] = 0x01;										// always 0x01 for Request Data Page
				break;

			default:				
				break;
		}
	}
};  