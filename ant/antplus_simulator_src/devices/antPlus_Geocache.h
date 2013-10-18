/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/


#pragma once

#include "types.h"
#define Geocache_REV				1.0					// Device Profile Revision Number

public ref class Geocache
{

public:

	// Channel Parameters
	static const UCHAR DEVICE_TYPE = 19;
	static const UCHAR TX_TYPE_SENSOR = 5;
	static const UCHAR TX_TYPE_DISPLAY = 0;
	static const UCHAR RF_CHANNEL = 57;
	static const USHORT MESG_4HZ_PERIOD = 8192;			// 4Hz
    static const USHORT MESG_P5HZ_PERIOD = 65535;		// 0.5Hz
	static const UCHAR  MESG_4HZ_TIMEOUT = (4 * 30);	// 4Hz * 30secs
	static const UCHAR	MESG_REQ_RESP_ACK = 0x81;
	static const UCHAR	MESG_REQ_RESP_BROADCAST = 0x01;

	static const UCHAR  MESG_WAIT_REQ = 5;
	
	// Data Pages
	static const UCHAR PAGE_0 = 0;
	static const UCHAR PAGE_1 = 1;
	static const UCHAR PAGE_2 = 2;
	static const UCHAR PAGE_3 = 3;
	static const UCHAR PAGE_32 = 32;
	static const UCHAR PAGE_AUTHENTICATION = 32;
	static const UCHAR START_PROG_PAGES = 2;			// Page0 + Page1

	// Acknowledged Messages
	static const UCHAR ACK_FAIL = 0;
	static const UCHAR ACK_SUCCESS = 1;
	
	// Reserved/invalid/special values
	static const UCHAR RESERVED = 0xFF;

	// Bit Masks
	static const UCHAR BYTE_MASK = 0xFF;
	static const UCHAR UPPER_NIBBLE_MASK = 0xF0;
	static const UCHAR LOWER_NIBBLE_MASK = 0x0F;
	static const UCHAR NEGATIVE_BYTE_MASK = 0x80;
	static const UCHAR REMOVE_SIGNED_BIT_MASK = 0x7F;
	static const ULONG NEGATIVE_LONG_MASK = 0xF0000000;

	// Bit shifts
	static const UCHAR NIBBLE_SHIFT = 4;
	static const UCHAR BYTE_SHIFT = 8;
	static const UCHAR LATLON_BYTE_2_SHIFT = 8;
	static const UCHAR LATLON_BYTE_3_SHIFT = 16;
	static const UCHAR LATLON_BYTE_4_SHIFT = 24;

	// Scale and offset Factors
	static const USHORT TIME_SCALE_FACTOR = 1024;
	static const DOUBLE SEMI_CIRCLE_CONVERSION = System::Math::Pow(2.0, 31) / 180;  // conversion to semicircles from degrees

	static const UCHAR ID_LEN = 9;				// not including the NULL
	static const UCHAR TXT_LEN = 90;
	static const UCHAR AUTH_LEN = 41;			// Page 2-31 Data ID Types

	static const UCHAR DEFAULT_ID = 0x00;			// 'Space' (0x20)
	static const UCHAR DEFAULT_TOT_PAGES = 0xFF;	// 0xFF used to indicate UnProgrammed Geocache
	static const ULONG DEFAULT_PIN = 0xFFFFFFFF;	//

	static const UCHAR HINT_CHARS_PER_PAGE = 6;			// 6 Characters per Programmable 'Hint' Page

	// Programmable Page Definitions
	static const UCHAR PROGRAMMABLE_PAGES = 31;
	static const UCHAR DATA_ID_LATITUDE = 0;
	static const UCHAR DATA_ID_LONGITUDE = 1;
	static const UCHAR DATA_ID_HINT = 2;
	static const UCHAR DATA_ID_PIN = 3;
	static const UCHAR DATA_ID_LOGGED_VISITS = 4;

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

	USHORT usMessagePeriod;			// Dynamic Message Period (0.5Hz Beacon, 4Hz Communicating w/ GPS)

	UCHAR ucReceivedPageNum;		// Incoming (RX) Page Number

	// Geocache Data Page 0
	// Geocache ID (9 6-bit ASCII Digits == ASCII - 0x20)
	array<UCHAR, 1>^ cID;

	// Geocache Data Page 1
   ULONG ulPIN;					   // Geocache PIN
	UCHAR ucTotalPages;				// Total # of Pages Programmed (Including PAGE_1)

	// Geocache Data Page(s) 2..31
	BOOL bPINRequest;				   // GPS Request for PAGE_1 (PIN)
	BOOL b4HzRotation;				// Begin 4Hz Page Rotation
	BOOL bPageRequest;				// GPS 'Specific' Page Request
	BOOL bAckResponseRequest;		// GPS Requests Acknowledged Reponse
	UCHAR ucRequestedPageNum;		// Requested Page Number

	BOOL bLatitudeEnabled;
	BOOL bLongitudeEnabled;
	BOOL bHintEnabled;
	BOOL bLoggedVisitsEnabled;

	// GPS Total 'Programming' Pages
	UCHAR ucProgTotalPages;

	// GPS Current 'Programing' Page
	UCHAR ucCurrProgPage;

	// Geocache Page 'Programming' Status
	array<BOOL, 1>^ bPageProg;

	// Geocache Page 'Read' Status
	array<BOOL, 1>^ bPageRead;

	// Geocache Data Page(s) 2..31 - Raw [Byte] Data
	array<UCHAR, 2>^ ucProgPages;

	// Geocache Data ID 0 - Latitude (SemiCircles)
	SLONG slLatitude_SC;

	// Geocache Data ID 1 - Longitude (SemiCircles)
	SLONG slLongitude_SC;

	// Geocache Data ID 2 - "Hint"
	array<UCHAR, 1>^ cHint;
	UCHAR	ucHintStartPage;
	UCHAR	ucNumHintPages;

	// Geocache Data ID 4 - Logged Visits
	UCHAR	ucLoggedVisitPage;
	ULONG	ulLastVisitTimestamp;
	USHORT usNumVisits;
	UCHAR	ucSeconds;
	UCHAR	ucMinutes;
	UCHAR	ucHours;
	UCHAR	ucDays;
	UCHAR	ucMonth;
	UCHAR	ucYears;

	// Geocache Data Page 32 - Authentication Token (Hash of GPS Serial Number and Nonce)
	array <UCHAR, 1>^ ucAuthToken;
	ULONG	ulSerialNum;
	USHORT	usNonce;

public:
	Geocache()
	{
		//initialize variables to invalid if possible, otherwise 0
		slLatitude_SC = 0;
		slLongitude_SC = 0;
		cID = gcnew array<UCHAR>(10);
		cHint = gcnew array<UCHAR>(180);
		ucAuthToken = gcnew array<UCHAR>(7);
		ucProgPages = gcnew array<UCHAR,2>(32,8);
		bPageRead = gcnew array<BOOL>(83);
		bPageProg = gcnew array<BOOL>(32);
	}

	~Geocache()
	{
	}


public:

/**************************************************************************
 * Geocache::Decode
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
		UCHAR ucPageNum = pucRxBuffer_[0];
		UCHAR ucDataID;
		static UCHAR ucHintOffset;

		switch(ucPageNum)
		{
			case PAGE_0:
				// Unpack the Geocache ID
				cID[0] = ((pucRxBuffer_[1] >> 2) & 0x3F);
				cID[1] = ((pucRxBuffer_[1] << 4) & 0x30) + ((pucRxBuffer_[2] >> 4) & 0x0F);
				cID[2] = ((pucRxBuffer_[2] << 2) & 0x3C) + ((pucRxBuffer_[3] >> 6) & 0x03);
				cID[3] = ((pucRxBuffer_[3] >> 0) & 0x3F);
				cID[4] = ((pucRxBuffer_[4] >> 2) & 0x3F);
				cID[5] = ((pucRxBuffer_[4] << 4) & 0x30) + ((pucRxBuffer_[5] >> 4) & 0x0F);
				cID[6] = ((pucRxBuffer_[5] << 2) & 0x3C) + ((pucRxBuffer_[6] >> 6) & 0x03);
				cID[7] = ((pucRxBuffer_[6] >> 0) & 0x3F);
				cID[8] = ((pucRxBuffer_[7] >> 2) & 0x3F);
            break;

			case PAGE_1:
				// decode PIN and Number of Pages
				ulPIN = (pucRxBuffer_[2] << 0)  + (pucRxBuffer_[3] << 8) +
					(pucRxBuffer_[4] << 16) + (pucRxBuffer_[5] << 24) ;
				ucTotalPages = pucRxBuffer_[6];
				ucHintStartPage = 0;
            break;

			case PAGE_32:
				// decode Authentication Token
				ucAuthToken[0] = pucRxBuffer_[1];
				ucAuthToken[1] = pucRxBuffer_[2];
				ucAuthToken[2] = pucRxBuffer_[3];
				ucAuthToken[3] = pucRxBuffer_[4];
				ucAuthToken[4] = pucRxBuffer_[5];
				ucAuthToken[5] = pucRxBuffer_[6];
				ucAuthToken[6] = pucRxBuffer_[7];
            break;

			default:
				ucProgPages[ucPageNum,0] = pucRxBuffer_[0];
				ucProgPages[ucPageNum,1] = pucRxBuffer_[1];
				ucProgPages[ucPageNum,2] = pucRxBuffer_[2];
				ucProgPages[ucPageNum,3] = pucRxBuffer_[3];
				ucProgPages[ucPageNum,4] = pucRxBuffer_[4];
				ucProgPages[ucPageNum,5] = pucRxBuffer_[5];
				ucProgPages[ucPageNum,6] = pucRxBuffer_[6];
				ucProgPages[ucPageNum,7] = pucRxBuffer_[7];

				ucDataID = pucRxBuffer_[1];

				switch (ucDataID)
				{
					case DATA_ID_LATITUDE:
						bLatitudeEnabled = TRUE;
						slLatitude_SC = (long)((pucRxBuffer_[2]) |		// Latitude in semicirles
										(pucRxBuffer_[3] << LATLON_BYTE_2_SHIFT) |
										(pucRxBuffer_[4] << LATLON_BYTE_3_SHIFT) |
										(pucRxBuffer_[5] << LATLON_BYTE_4_SHIFT));
						break;
							
					case DATA_ID_LONGITUDE:
						bLongitudeEnabled = TRUE;
						slLongitude_SC = (long)((pucRxBuffer_[2]) |	// Longitude in semicirles
										(pucRxBuffer_[3] << LATLON_BYTE_2_SHIFT) |
										(pucRxBuffer_[4] << LATLON_BYTE_3_SHIFT) |
										(pucRxBuffer_[5] << LATLON_BYTE_4_SHIFT));
						break;

					case DATA_ID_HINT :
						bHintEnabled = TRUE;
						if (ucHintStartPage == 0) ucHintStartPage = ucPageNum;	// First Hint Page
						ucHintOffset = (ucPageNum - ucHintStartPage) * HINT_CHARS_PER_PAGE;;	// Offset # of Chars into Hint Array
						cHint[ucHintOffset + 0] = pucRxBuffer_[2];
						cHint[ucHintOffset + 1] = pucRxBuffer_[3];
						cHint[ucHintOffset + 2] = pucRxBuffer_[4];
						cHint[ucHintOffset + 3] = pucRxBuffer_[5];
						cHint[ucHintOffset + 4] = pucRxBuffer_[6];
						cHint[ucHintOffset + 5] = pucRxBuffer_[7];
						break;

					case DATA_ID_LOGGED_VISITS :
						bLoggedVisitsEnabled = TRUE;
						ucLoggedVisitPage = ucPageNum;
						ulLastVisitTimestamp =	(pucRxBuffer_[2]) + // Last Visit Timestamp
												(pucRxBuffer_[3] << 8) +
												(pucRxBuffer_[4] << 16) +
												(pucRxBuffer_[5] << 24);
						usNumVisits	= (pucRxBuffer_[6]) + (pucRxBuffer_[7] << 8);	// Number of Visits
						break;

					default :
						break;							
				}
				break;
		}
	}
	

/**************************************************************************
 * Geocache::GetPageData
 * 
 * Gets the Geocache Page Data :
 *
 * Exceptions are thrown when dealing with invalid data
 *
 * ucPageNum_: number of page to encode
 * pucTxBuffer_: pointer to the buffer that will store the encoded data
 * 
 * returns: N/A
 *
 **************************************************************************/
	void GetPageData(UCHAR ucPageNum_, UCHAR* pucTxBuffer_)
	{
		switch (ucPageNum_)
		{
			case PAGE_0 :
				pucTxBuffer_[0] = ucPageNum_;
				pucTxBuffer_[1] = ((cID[0] << 2) & 0xFC) | ((cID[1] >> 4) & 0x03);
				pucTxBuffer_[2] = ((cID[1] << 4) & 0xF0) | ((cID[2] >> 2) & 0x0F);
				pucTxBuffer_[3] = ((cID[2] << 6) & 0xC0) | ((cID[3] >> 0) & 0x3F);
				pucTxBuffer_[4] = ((cID[4] << 2) & 0xFC) | ((cID[5] >> 4) & 0x03);
				pucTxBuffer_[5] = ((cID[5] << 4) & 0xF0) | ((cID[6] >> 2) & 0x0F);
				pucTxBuffer_[6] = ((cID[6] << 6) & 0xC0) | ((cID[7] >> 0) & 0x3F);
				pucTxBuffer_[7] = ((cID[8] << 2) & 0xFC);
				break;

			case PAGE_1 :
				pucTxBuffer_[0] = ucPageNum_;
				pucTxBuffer_[1] = RESERVED;
				pucTxBuffer_[2] = ulPIN & 0xFF;				// PIN (bits 0-7)
				pucTxBuffer_[3] = (ulPIN >> 8) & 0xFF;		// PIN (bits 8-15)
				pucTxBuffer_[4] = (ulPIN >> 16) & 0xFF;		// PIN (bits 16-23)
				pucTxBuffer_[5] = (ulPIN >> 24) & 0xFF;		// PIN (bits 24-31)
				pucTxBuffer_[6] = ucTotalPages;
				pucTxBuffer_[7] = 0xFF;			// RESERVED
				break;

			case PAGE_32 :
				pucTxBuffer_[0] = ucPageNum_;
				pucTxBuffer_[1] = ucAuthToken[0];
				pucTxBuffer_[2] = ucAuthToken[1];
				pucTxBuffer_[3] = ucAuthToken[2];
				pucTxBuffer_[4] = ucAuthToken[3];
				pucTxBuffer_[5] = ucAuthToken[4];
				pucTxBuffer_[6] = ucAuthToken[5];
				pucTxBuffer_[7] = ucAuthToken[6];
				break;

			default :	// Programmable Page Data
				pucTxBuffer_[0] = ucPageNum_;
				pucTxBuffer_[1] = ucProgPages[ucPageNum_,1];
				pucTxBuffer_[2] = ucProgPages[ucPageNum_,2];
				pucTxBuffer_[3] = ucProgPages[ucPageNum_,3];
				pucTxBuffer_[4] = ucProgPages[ucPageNum_,4];
				pucTxBuffer_[5] = ucProgPages[ucPageNum_,5];
				pucTxBuffer_[6] = ucProgPages[ucPageNum_,6];
				pucTxBuffer_[7] = ucProgPages[ucPageNum_,7];
				break;
		}
	}


/**************************************************************************
 * Geocache::GenerateAuthRequestPage
 * 
 * Generate Auth (Request) Page Data :
 *
 * Exceptions are thrown when dealing with invalid data
 *
 * pucTxBuffer_: pointer to the buffer that will store the encoded data
 * 
 * returns: N/A
 *
 **************************************************************************/
	void GenerateAuthRequestPage(UCHAR* pucTxBuffer_)
	{
		ulSerialNum = 0x33221100;
		usNonce = rand();
		pucTxBuffer_[0] = PAGE_32;
		pucTxBuffer_[1] = RESERVED;
		pucTxBuffer_[2] = usNonce & 0xFF;					// Nonce (bits 0-7)
		pucTxBuffer_[3] = (usNonce >> 8) & 0xFF;			// Nonce (bits 8-15)
		pucTxBuffer_[4] = ulSerialNum & 0xFF;				// SerialNum (bits 0-7)
		pucTxBuffer_[5] = (ulSerialNum >> 8) & 0xFF;		// SerialNumIN (bits 8-15)
		pucTxBuffer_[6] = (ulSerialNum >> 16) & 0xFF;		// SerialNum (bits 16-23)
		pucTxBuffer_[7] = (ulSerialNum >> 24) & 0xFF;		// SerialNum (bits 24-31)
	}

/**************************************************************************
 * Geocache::GenerateProgPageData
 * 
 * Generate the Geocache 'Programmable' data pages (2..31) from UI Values :
 *
 * Exceptions are thrown when dealing with invalid data
 *
 * returns: N/A
 *
 **************************************************************************/
	void GenerateProgPages(void)
	{
		UCHAR ucHintOffset;
		UCHAR ucPageCount = START_PROG_PAGES;

		if (bLatitudeEnabled)
		{
			ucProgPages[ucPageCount,0] = ucPageCount;
			ucProgPages[ucPageCount,1] = DATA_ID_LATITUDE;
			ucProgPages[ucPageCount,2] = slLatitude_SC & BYTE_MASK;             // Latitude in semicircles
			ucProgPages[ucPageCount,3] = (slLatitude_SC >> LATLON_BYTE_2_SHIFT) & BYTE_MASK;
			ucProgPages[ucPageCount,4] = (slLatitude_SC >> LATLON_BYTE_3_SHIFT) & BYTE_MASK;
			ucProgPages[ucPageCount,5] = (slLatitude_SC >> LATLON_BYTE_4_SHIFT) & BYTE_MASK;
			ucPageCount++;
		}

		if (bLongitudeEnabled)
		{
			ucProgPages[ucPageCount,0] = ucPageCount;
			ucProgPages[ucPageCount,1] = DATA_ID_LONGITUDE;
			ucProgPages[ucPageCount,2] = slLongitude_SC & BYTE_MASK;             // Longitude in semicircles
			ucProgPages[ucPageCount,3] = (slLongitude_SC >> LATLON_BYTE_2_SHIFT) & BYTE_MASK;
			ucProgPages[ucPageCount,4] = (slLongitude_SC >> LATLON_BYTE_3_SHIFT) & BYTE_MASK;
			ucProgPages[ucPageCount,5] = (slLongitude_SC >> LATLON_BYTE_4_SHIFT) & BYTE_MASK;
			ucPageCount++;
		}

		if (bLoggedVisitsEnabled)
		{
			ucProgPages[ucPageCount,0] = ucPageCount;
			ucProgPages[ucPageCount,1] = DATA_ID_LOGGED_VISITS;
			ucProgPages[ucPageCount,2] = ulLastVisitTimestamp & 0xFF;				// Last Visit Timestamp (bits 0-7)
			ucProgPages[ucPageCount,3] = (ulLastVisitTimestamp >> 8) & 0xFF;		// Last Visit Timestamp (bits 8-15)
			ucProgPages[ucPageCount,4] = (ulLastVisitTimestamp >> 16) & 0xFF;		// Last Visit Timestamp (bits 16-23)
			ucProgPages[ucPageCount,5] = (ulLastVisitTimestamp >> 24) & 0xFF;		// Last Visit Timestamp (bits 24-31)
			ucProgPages[ucPageCount,6] = usNumVisits & 0xFF;						// Number of Visits (bits 0-7)
			ucProgPages[ucPageCount,7] = (usNumVisits >> 8) & 0xFF;					// Number of Visits (bits 8-15)
			ucPageCount++;
		}

		if (bHintEnabled)
		{
			ucHintStartPage = ucPageCount;

			for (UCHAR ucHintPage=ucHintStartPage; ucHintPage<(ucHintStartPage+ucNumHintPages); ucHintPage++)
			{
				ucHintOffset = (ucHintPage - ucHintStartPage) * HINT_CHARS_PER_PAGE;;	// Offset # of Chars into Hint Array
				ucProgPages[ucHintPage,0] = ucHintPage;
				ucProgPages[ucHintPage,1] = DATA_ID_HINT;
				ucProgPages[ucHintPage,2] = cHint[ucHintOffset + 0];
				ucProgPages[ucHintPage,3] = cHint[ucHintOffset + 1];
				ucProgPages[ucHintPage,4] = cHint[ucHintOffset + 2];
				ucProgPages[ucHintPage,5] = cHint[ucHintOffset + 3];
				ucProgPages[ucHintPage,6] = cHint[ucHintOffset + 4];
				ucProgPages[ucHintPage,7] = cHint[ucHintOffset + 5];
				ucPageCount++;
			}
		}
		ucTotalPages = ucPageCount;

		// Initialize (remaining) Programmable Page 'Store' - to RESERVED [0xFF]
		for (; ucPageCount<32; ucPageCount++)
		{
			ucProgPages[ucPageCount,0] = ucPageCount;
			ucProgPages[ucPageCount,1] = RESERVED;
			ucProgPages[ucPageCount,2] = RESERVED;
			ucProgPages[ucPageCount,3] = RESERVED;
			ucProgPages[ucPageCount,4] = RESERVED;
			ucProgPages[ucPageCount,5] = RESERVED;
			ucProgPages[ucPageCount,6] = RESERVED;
			ucProgPages[ucPageCount,7] = RESERVED;
		}
	}

/**************************************************************************
 * Geocache::UpdateLoggedVisitPage()
 * 
 * Updates the Geocache Logged Visit Page from values :
 *
 * Exceptions are thrown when dealing with invalid data
 *
 * returns: N/A
 *
 **************************************************************************/
	void UpdateLoggedVisitPage(void)
	{
		if (bLoggedVisitsEnabled)
		{
			ucProgPages[ucLoggedVisitPage,0] = ucLoggedVisitPage;
			ucProgPages[ucLoggedVisitPage,1] = DATA_ID_LOGGED_VISITS;
			ucProgPages[ucLoggedVisitPage,1] = DATA_ID_LOGGED_VISITS;
			ucProgPages[ucLoggedVisitPage,2] = ulLastVisitTimestamp & 0xFF;				// Last Visit Timestamp (bits 0-7)
			ucProgPages[ucLoggedVisitPage,3] = (ulLastVisitTimestamp >> 8) & 0xFF;		// Last Visit Timestamp (bits 8-15)
			ucProgPages[ucLoggedVisitPage,4] = (ulLastVisitTimestamp >> 16) & 0xFF;		// Last Visit Timestamp (bits 16-23)
			ucProgPages[ucLoggedVisitPage,5] = (ulLastVisitTimestamp >> 24) & 0xFF;		// Last Visit Timestamp (bits 24-31)
			ucProgPages[ucLoggedVisitPage,6] = usNumVisits & 0xFF;						// Number of Visits (bits 0-7)
			ucProgPages[ucLoggedVisitPage,7] = (usNumVisits >> 8) & 0xFF;				// Number of Visits (bits 8-15)
		}
	}

/**************************************************************************
 * Geocache::EnabledProgPages
 * 
 * Determines which Programmable Pages (Data IDs) are Present
 *
 * Exceptions are thrown when dealing with invalid data
 *
 * returns: N/A
 *
 **************************************************************************/
	void EnabledProgPages(void)
	{
		UCHAR ucDataID;
		UCHAR ucPageCount;

		bLatitudeEnabled = FALSE;
		bLongitudeEnabled = FALSE;
		bHintEnabled = FALSE;
		bLoggedVisitsEnabled = FALSE;

		for(ucPageCount=START_PROG_PAGES; ucPageCount<=ucTotalPages; ucPageCount++)
		{
				ucDataID = ucProgPages[ucPageCount,1];
				switch (ucDataID)
				{
					case DATA_ID_LATITUDE:
						bLatitudeEnabled = TRUE;
						break;
							
					case DATA_ID_LONGITUDE:
						bLongitudeEnabled = TRUE;
						break;

					case DATA_ID_HINT :
						bHintEnabled = TRUE;
						break;

					case DATA_ID_LOGGED_VISITS :
						bLoggedVisitsEnabled = TRUE;
						break;

					default :
						break;							
				}
		}
	}

/**************************************************************************
 * Geocache::InitSensor
 * 
 * Initialize the Geocache Sensor
 *   - PAGE_0 to 'Programmable' data pages (2..31) from UI Values :
 *
 * Exceptions are thrown when dealing with invalid data
 *
 * returns: N/A
 *
 **************************************************************************/
	void InitSensor(void)
	{
		// Initialize PAGE_0 (ID set to default value)
		for (UCHAR i=0; i<ID_LEN; i++) cID[i] = DEFAULT_ID;

		// Initialize PAGE_1 (PIN set to RESERVED)
		ulPIN = DEFAULT_PIN;

		// Initialize Programmable Page 'Store' - to RESERVED [0xFF]
		for (UCHAR ucPageCount=0; ucPageCount<32; ucPageCount++)
		{
			ucProgPages[ucPageCount,0] = ucPageCount;
			ucProgPages[ucPageCount,1] = RESERVED;
			ucProgPages[ucPageCount,2] = RESERVED;
			ucProgPages[ucPageCount,3] = RESERVED;
			ucProgPages[ucPageCount,4] = RESERVED;
			ucProgPages[ucPageCount,5] = RESERVED;
			ucProgPages[ucPageCount,6] = RESERVED;
			ucProgPages[ucPageCount,7] = RESERVED;
		}
	}

/**************************************************************************
 * Geocache::InitDisplay
 * 
 * Initialize the Geocache Display
 *   - PAGE_0 to 'Programmable' data pages (2..31) from UI Values :
 *
 * Exceptions are thrown when dealing with invalid data
 *
 * returns: N/A
 *
 **************************************************************************/
	void InitDisplay(void)
	{
		// Initialize PageRead Status to FALSE
		for (UCHAR i=0; i<=82; i++) bPageRead[i] = FALSE;

		// Initialize PageProg Status to FALSE
		for (UCHAR i=0; i<32; i++) bPageProg[i] = FALSE;

		// Initialize PAGE_0 (ID set to default value)
		for (UCHAR i=0; i<ID_LEN; i++) cID[i] = DEFAULT_ID;

		// Initialize PAGE_1
		ulPIN = DEFAULT_PIN;
		ucTotalPages = DEFAULT_TOT_PAGES;

		bLatitudeEnabled = FALSE;
		slLatitude_SC = 0xFFFFFFFF;

		bLongitudeEnabled = FALSE;
		slLongitude_SC = 0xFFFFFFFF;

		bHintEnabled = FALSE;

		bLoggedVisitsEnabled = FALSE;
		ulLastVisitTimestamp = 0xFFFFFFFF;
		usNumVisits = 0xFFFF;

		// Initialize Programmable Page 'Store' - to RESERVED [0xFF]
		for (UCHAR ucPageCount=0; ucPageCount<32; ucPageCount++)
		{
			ucProgPages[ucPageCount,0] = ucPageCount;
			ucProgPages[ucPageCount,1] = RESERVED;
			ucProgPages[ucPageCount,2] = RESERVED;
			ucProgPages[ucPageCount,3] = RESERVED;
			ucProgPages[ucPageCount,4] = RESERVED;
			ucProgPages[ucPageCount,5] = RESERVED;
			ucProgPages[ucPageCount,6] = RESERVED;
			ucProgPages[ucPageCount,7] = RESERVED;
		}
	}

};