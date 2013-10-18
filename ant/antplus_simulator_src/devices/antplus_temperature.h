/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/


#pragma once

#include "types.h"
#define Temperature_REV				ALPHA 1					// Device Profile Revision Number

public ref class Temperature
{

public:

	// Channel Parameters
	static const UCHAR DEVICE_TYPE = 25;
	static const UCHAR TX_TYPE_SENSOR = 5;
	static const UCHAR TX_TYPE_DISPLAY = 0;
	static const UCHAR RF_CHANNEL = 57;
	static const USHORT MESG_4HZ_PERIOD = 8192;			// 4Hz
   static const USHORT MESG_P5HZ_PERIOD = 65535;		// 0.5Hz
	static const UCHAR  MESG_4HZ_TIMEOUT = (4 * 30);	// 4Hz * 30secs
	static const UCHAR MESG_REQ_RESP_ACK = 0x81;
	static const UCHAR MESG_REQ_RESP_BROADCAST = 0x01;

	static const UCHAR  MESG_WAIT_REQ = 5;
	
	// Data Pages
   static const UCHAR PAGE_0 = 0;
	static const UCHAR PAGE_1 = 1;
	
	// Reserved/invalid/special values
	static const UCHAR RESERVED = 0xFF;
   static const UCHAR BACKGROUND_INTERVAL = 129; 
   static const UCHAR TXINFO_4HZ = 0x01;                     // 4.0 Hz transmission rate
   static const UCHAR TXINFO_P5HZ = 0x00;                    // 0.5 Hz transmission rate
   static const short HIGHLOW_INVALID = 0xF800;              // Sign extended 0x800 invalid value
   static const UCHAR SUPPORTED_PAGES_0 = 0x01;              // page 0 supported
   static const UCHAR SUPPORTED_PAGES_1 = 0x02;              // page 1 supported
   static const UCHAR SUPPORTED_PAGES_0_1 = 0x03;            // pages 0 and 1 supported
   static const UCHAR UTC_TIME_NOT_SUPPORTED = 0x00;         // UTC Time values for TxInfo
   static const UCHAR UTC_TIME_NOT_SET = 0x04;
   static const UCHAR UTC_TIME_SET = 0x08;
   static const UCHAR LOCAL_TIME_NOT_SUPPORTED = 0x00;       // LOCAL Time values for TxInfo
   static const UCHAR LOCAL_TIME_NOT_SET = 0x10;
   static const UCHAR LOCAL_TIME_SET = 0x20;

   static const UCHAR DATA_REQUEST = 0x01;

	// Bit Masks
	static const USHORT BYTE_MASK = 0x00FF;
   static const USHORT UPPER_BYTE_MASK = 0xFF00;
   static const USHORT UNIBBLE_UBYTE_SIGNXTND = 0xF000;
	static const UCHAR UPPER_NIBBLE_MASK = 0xF0;
	static const UCHAR LOWER_NIBBLE_MASK = 0x0F;
   static const UCHAR PERIOD_MASK = 0x03;
   static const UCHAR UTC_TIME_MASK = 0x0C;
   static const UCHAR LOCAL_TIME_MASK = 0x30;


	// Error handling
	// Flags indicate errors causing the exception
	ref class Error : public System::Exception{
	public:
		BOOL	bBadReserved;		// Invalid values on reserved fields
		BOOL	bUndefPage;			// Undefined page

		enum class Code : UCHAR		// Error code definitions
		{
			INVALID_RESERVED,		// Invalid value in reserved field
			UNDEF_PAGE,				// Undefined data page
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
			default:
				break;

			}
		}
	};

public:

	USHORT usMessagePeriod;			// Dynamic Message Period (0.5Hz Beacon, 4Hz when Communicating w/ Display)

	// Temperature Data Page 1
   UCHAR ucTxInfo;               // Transmission Info
   UCHAR ucEventCount;
   UCHAR ucSupportedPages;
   short s24HrLow;               // 24 Hour Low (hundredths of deg C)
   short s24HrHigh;              // 24 Hour High (hundredths of deg C)
   short sCurrentTemp;           // Current Temperature (hundredths of deg C) 

   // Period Switching
   BOOL b4HzRotation;            // Begin 4Hz Page Rotation

   // Temperature Data Request
   BOOL bPageRequest;            // True if slave has requested a page
   UCHAR ucRequestedPageNum;     // Requested Page Number

public:
	Temperature()
	{
	}

	~Temperature()
	{
	}


public:

/**************************************************************************
 * Temperature::Decode
 * 
 * Decodes all main data 
 *
 * pucRxBuffer_: pointer to the buffer containing the received data
 * 
 * returns: N/A
 *
 *************************************************************************/
	void Decode(UCHAR* pucRxBuffer_)
	{
		UCHAR ucPageNum = pucRxBuffer_[0];

      switch(ucPageNum)
      {
      case PAGE_0 :
         ucTxInfo = pucRxBuffer_[3];
         ucSupportedPages = pucRxBuffer_[4];
         break;

      case PAGE_1:
         ucEventCount = pucRxBuffer_[2];

         s24HrLow = pucRxBuffer_[4] & UPPER_NIBBLE_MASK;
         s24HrLow = (s24HrLow << 4) | pucRxBuffer_[3];
         // sign extend
         if(s24HrLow >> 11)
         s24HrLow = s24HrLow | UNIBBLE_UBYTE_SIGNXTND;

         s24HrHigh = pucRxBuffer_[5];
         s24HrHigh = (s24HrHigh << 4) | (pucRxBuffer_[4] & LOWER_NIBBLE_MASK);
         // sign extend
         if(s24HrHigh >> 11)
         s24HrHigh = s24HrHigh | UNIBBLE_UBYTE_SIGNXTND;

         sCurrentTemp = pucRxBuffer_[7];
         sCurrentTemp = (sCurrentTemp << 8) | pucRxBuffer_[6];
         break;

      default:
         break;
      }

	}
/**************************************************************************
 * Temperature::Encode
 * 
 * Encodes the temperature into the transmission buffer:
 *
 * ucPageNum_: number of page to encode (nothing happens if != 1)
 * pucTxBuffer_: pointer to the buffer that will store the encoded data
 * 
 * returns: N/A
 *
 **************************************************************************/
	void Encode(UCHAR ucPageNum_, UCHAR* pucTxBuffer_)
	{
      switch(ucPageNum_)
      {
      case 0:
         pucTxBuffer_[0] = ucPageNum_;
		   pucTxBuffer_[1] = RESERVED;
		   pucTxBuffer_[2] = RESERVED;	
		   pucTxBuffer_[3] = ucTxInfo;
	   	pucTxBuffer_[4] = SUPPORTED_PAGES_0_1;   //Supported pages LSB
	   	pucTxBuffer_[5] = 0x00;                   //Supported pages second LSB
	   	pucTxBuffer_[6] = 0x00;                   //Supported pages second MSB
	   	pucTxBuffer_[7] = 0x00;                   //Supported pages MSB
         break;
      case 1:
         pucTxBuffer_[0] = ucPageNum_;
		   pucTxBuffer_[1] = RESERVED;
		   pucTxBuffer_[2] = ucEventCount;	
		   pucTxBuffer_[3] = (s24HrLow & BYTE_MASK);         // 24 Hour Low LSB
         // for pucTxBuffer_[4] bits 4:7 = 24 Hour Low MSN,  bits 0:3 = 24 Hour High LSN
	   	pucTxBuffer_[4] =((s24HrLow>>4) & UPPER_NIBBLE_MASK) | (s24HrHigh & LOWER_NIBBLE_MASK);
	   	pucTxBuffer_[5] = (s24HrHigh>>4) & BYTE_MASK;		   // 24 Hour High MSB
	   	pucTxBuffer_[6] = System::Convert::ToByte(sCurrentTemp & BYTE_MASK); // Current Temp LSB
	   	pucTxBuffer_[7] = System::Convert::ToByte((sCurrentTemp>>8) & BYTE_MASK);		// Current Temp MSB
         break;
      default:
         break;
      }
	}

};