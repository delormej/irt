/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/


#pragma once

#include	"types.h"
#define		CONTROLS_REV				2 Alpha 9				//Device Profile revision number	

public ref class AntPlusControls
{
public:

   // Channel Parameters
	static const UCHAR	DEVICE_TYPE = 16;
	static const UCHAR	TX_TYPE = 5;
	static const USHORT MSG_PERIOD = 8192;		//4 Hz

	//Data Pages
	static const UCHAR PAGE_AUDIO_UPDATE = 0x01;			
	static const UCHAR PAGE_DEVICE_AVAILABILITY = 0x02;		
	static const UCHAR PAGE_CALL_STATUS = 0x03;				
	static const UCHAR PAGE_TEXT_STATUS = 0x04;				
	static const UCHAR PAGE_TEXT_SUBPAGE = 0x05;			
	static const UCHAR PAGE_TEXT_COUNT = 0x06;				
	static const UCHAR PAGE_VIDEO_UPDATE = 0x07;			
   static const UCHAR PAGE_ADVANCED_CAPABILITIES = 0x08;

	static const UCHAR PAGE_AUDIO_VIDEO_COMMAND = 0x10;			
	static const UCHAR PAGE_UTF_8_CHARACTER_COMMAND = 0x11;	
	static const UCHAR PAGE_CUSTOM_COMMAND = 0x12;			
	static const UCHAR PAGE_CALL_COMMAND = 0x13;			
	static const UCHAR PAGE_TEXT_COMMAND = 0x14;				

	//Reserved/Invalid Values
	static const UCHAR	UNKNOWN_BYTE = 0xFF;
	static const USHORT UNKNOWN_SHORT = 0xFFFF;
	static const UCHAR	RESERVED_BYTE = 0xFF;
	static const USHORT	RESERVED_SHORT = 0xFFFF;
	static const UCHAR RESERVED_BYTE_0x00 = 0x00;
	static const UCHAR ALL_SUBPAGES = 0xFF;
   
   // Ack Messages
   static const UCHAR MAX_RETRIES = 4;
   static const UCHAR ACK_FAIL = 0;
   static const UCHAR ACK_SUCCESS = 1;
   static const UCHAR ACK_RETRY = 2;

	//Transmission Timing Values
	static const UCHAR COMMON_INTERVAL = 65;				//Common data pages are sent every 65 messages from the Control device

public:
	//Audio Control Commands 
	enum class AudioVideoCommands : UCHAR
	{
		SET_VIDEO = 0x80,
		SET_AUDIO = 0x00,
		PLAY = 0x01,
		PAUSE = 0x02,
		STOP = 0x03, 
		VOLUME_UP = 0x04,
		VOLUME_DOWN = 0x05,
		MUTE = 0x06,
		TRACK_AHEAD = 0x07,
		TRACK_BACK = 0x08,
		REPEAT_CURRENT_TRACK = 0x09,
		REPEAT_ALL = 0x0A,
		REPEAT_OFF = 0x0B,
		SHUFFLE_SONGS = 0x0C,
		SHUFFLE_ALBUMS = 0x0D,
		SHUFFLE_OFF = 0x0E, 
		FFW = 0x0F,
		FRWD = 0x10,
		REPEAT_CUSTOM = 0x11,
		SHUFFLE_CUSTOM = 0x12,
		RECORD = 0x13,
	};
	enum class AudioCapabilities : UCHAR
	{
		IGNORE_BITFIELD = 0x80,
		CUSTOM_REPEAT = 0x04,
		CUSTOM_SHUFFLE = 0x02,
		SONG_TITLE = 0x01,
	};
   enum class Notifications : UCHAR
   {
      CALL_NOTIFICATION = 0x01,
      TEXT_NOTIFICATION = 0x02,
   };
	enum class VideoCapabilities : UCHAR
	{
		PLAYBACK = 0x02,
		RECORDER = 0x01,
	};
	enum class AudioState : UCHAR
	{
		CLEAR_AUDIO_STATE = 0x0F,	//& with state to clear bits 4 - 7 (0000 1111)
		OFF = 0x00,
		PLAY = 0x10,
		PAUSE = 0x20,
		STOP = 0x30,
		BUSY = 0x40,
		UNKNOWN = 0xF0,
	};
	enum class VideoStates : UCHAR
	{
		OFF = 0x00,
		PLAY = 0x01, 
		PAUSE = 0x02, 
		STOP = 0x03, 
		BUSY = 0x04, 
		FFWD = 0x05, 
		FRWD = 0x06,
		FWD = 0x07,
		RWD = 0x08,
		RECORDING = 0x09,
		UNKNOWN = 0xFF,
	};
	enum class RepeatState : UCHAR
	{
		CLEAR_REPEAT_STATE = 0xF3,	//& with state to clear bits 2-3 (1111 0011)
		OFF = 0x00,
		CURRENT_TRACK = 0x04,
		ALL_SONGS = 0x08,
		CUSTOM = 0x0C,
	};
	enum class ShuffleState : UCHAR
	{
		CLEAR_SHUFFLE_STATE = 0xFC,	//& with state to clear bits 0-1 (1111 1100)
		OFF = 0x00,
		TRACK_LEVEL = 0x01,
		ALBUM_LEVEL = 0x02,
		CUSTOM = 0x03,
	};
	enum class AvailDevices : UCHAR
	{
		AUDIO = 0x01,	//bit 0 is high if Audio supported, low if not supported
		PHONE = 0x02,	//bit 1 is high if Phone Control supported, low if not supported
      TEXT = 0x04,
		KEYPAD = 0x08,	//bit 2 is high if Keypad Control supported, low if not supported
		CUSTOM = 0x10,	//bit 3 is high if Custom Control supported, low if not supported
		VIDEO = 0x20,	//bit 5 is high if Video Control supported, low if not supported
	};
	enum class CurrentNotifications : UCHAR
	{
		CALL = 0x01,
		ALERT = 0x02,
	};
	enum class CallCommands : UCHAR
	{
		ANSWER_CALL = 0x01,
		SILENCE_RINGER ,
		HANG_UP,
	};
	enum class TextCommands : UCHAR
	{
		PROCESS = 0x80,
		CALENDAR = 0x00,
		SMS,
		EMAIL, 
		TWITTER,
		FACEBOOK,
		SONG_TITLE,
      CALLER_ID,
	};
	enum class AlertID : UCHAR
	{
		NO_ALERT = 0x00,
		CALENDAR,
		SMS, 
		EMAIL,
		TWITTER,
		FACEBOOK,
	};
	enum class CallStatus : UCHAR
	{
		INCOMING = 0x01,
		IN_CALL,
		CALL_HELD,
		MISSED_CALL,
	};
	enum class CallNumbers	: unsigned long long
	{
		EMERGENCY = 0x000000000000,
		WITHHELD = 0x000000000001,
		UNKNOWN = 0xFFFFFFFFFFFF,
	};
   enum class AdvancedCapabiltiesSubpages
   {
      TEXT_CAPABILITIES = 0x00,
      SONG_TITLE_SUBPAGES = 0x01,
      CALLER_ID_SUBPAGES = 0x02,
   };
   enum class TextCapabilities
   {
      FOUR_HZ = 0x03,
   };

public:
	//Error handling
	//Flags indicate errors causing the exception
	ref class Error : public System::Exception
	{
	public:
		BOOL bBadReserved;				//Invalid values on reserved fields
		BOOL bUndefPage;				//Undefined page	

		enum class Code : UCHAR			//Error code definitions
		{
			INVALID_RESERVED,			//Invalid value in reserved fields
			UNDEF_PAGE,					//Undefined page data
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


//Page variable declarations
public:

   //Helper Variables
   USHORT usExpectedSubpages;    //Counter to hold how many subpages are expected

	//Audio Update Page 1
	UCHAR	ucVolume;				//Used to set volume as a percent (min 0, max 100)
	USHORT usTotalTrackTime;	//Used to set the total time of the current track
	USHORT usCurrentTrackTime;	//Used to set the elapsed time of the current track
	UCHAR	ucAudioCapabilities;	//Used to determine what repeat/shuffle modes are supported 
	UCHAR	ucState;				   //Used to store the audio device's current state
									   //Bits 0-1: Shuffle State Bits 2-3: Repeat State Bits 4-7: Device State
	
	//Control Device Availability Page 2 
	UCHAR	ucCurrentNotifications;	//Bitfield to indicate specific device notifications
	UCHAR	ucDeviceAvailability;	//Bitfield indicating the supported control features

	//Call Status Page 3
   BOOL bCallerIDAvailable;         // True if the text device caller ID is available, false otherwise
	UCHAR	ucCallStatus;			      //Byte used to indicate current status of the call
	USHORT usCallNumberUpper2Bytes;  //Upper 2 bytes of the Call Number
	ULONG	ulCallNumberLower4Bytes;   //Lower 4 bytes of the Call Number

	//Text Status Page 4
	UCHAR	ucPage4AlertID;			//Used to indicate the type of alert received
	UCHAR	ucNumOfSubpages;		   //Used to indicate the number of subpages accompanying the text alert
	USHORT usPage4SerialNumber;

	//Text Subpage Data Page 5
	UCHAR	ucSubpageNum;			   //Used to indicate the subpage number
	USHORT usUTF8TextUpper2Bytes;	//Upper 2 bytes of UTF-8 Text data
	ULONG	ulUTF8TextLower4Bytes;	//Lower 4 bytes of UTF-8 Text data

	//Stored Text Count Page 6
	UCHAR	ucUnreadSMSCount;		   //Stores the number of unread SMS's
	UCHAR	ucUnreadEmailCount;		//Stores the number of unread emails
	UCHAR	ucUnreadTwitterCount;	//Stores the number of unread twitter updates
	UCHAR	ucUnreadFacebookCount;	//Stores the number of unread facebook updates

	//Video Update Page 7
	UCHAR	ucVideoVolume;			   //Used to set volume as a percent (min 0, max 100). MSB indicates muted or not
	USHORT usRemainingTime;		   //Total time remaining on video device (seconds, min 0, max 65534, unknown 65545)
	USHORT usTimeProgressed;		//Current number of seconds into playback/recording (seconds, min 0, max 65534, unknown 65535)
	UCHAR	ucVideoCapabilities;	   //Determines if video is playback or recorder device
	UCHAR	ucVideoState;			   //Video device state

   //Advanced Capabilities Page 8
   UCHAR ucSubpage1;
   UCHAR ucSubpage2;
   USHORT usDataField1;
   USHORT usDataField2;

	//Audio Command Page 16
	USHORT	usPage16SerialNumber;	//2 byte serial number of the remote device
	UCHAR	ucPage16Volume;			//Volume Increment 
	UCHAR	ucPage16Command;		//Command requested by the remote device (i.e. play, stop, pause etc.)

	//Keypad Command Page 17
	USHORT	usPage17SerialNumber;	//2 byte serial number of the remote device
	ULONG	ulPage17Command;		//Unicode character requested using UTF-8 encoding

	//Custom Command Page 18
	USHORT	usPage18SerialNumber;	//2 byte serial number of the remote device
	UCHAR	ucPage18Command;		//Custom command requested by remote device (application specific)

	//Call Command Page 19
	USHORT	usPage19SerialNumber;	//2 byte serial number of the remote device
	UCHAR	ucPage19Command;		//Call control command requested by remote device

	//Text Command Page 20
	USHORT	usPage20SerialNumber;		//2 byte serial number of the remote device
	UCHAR	ucPage20ChannelFreq;		//Used to specify the requested RF channel frequency 
	UCHAR	ucPage20ChannelPeriod;		//Used to specify the channel period to change to 
	UCHAR	ucPage20NumberOfSubpages;	//Used to indicate the max number of subpages requested 
	UCHAR	ucPage20Command;			//Text Alert Command requested by remote device

	//General Command Page 21
	USHORT usPage21SerialNumber;	//2 byte serial number of the remote device
	UCHAR ucPage21Command;			//General Command number	

public:
	AntPlusControls()
	{
		//Page 1 
		ucVolume = UNKNOWN_BYTE;			
		usTotalTrackTime = UNKNOWN_SHORT;	
		usCurrentTrackTime = UNKNOWN_SHORT;	
		ucAudioCapabilities = 0x80;			
		ucState = UNKNOWN_BYTE;				

		//Page 2 
		ucCurrentNotifications = 0;		
		ucDeviceAvailability = 0;		

		//Page 3
      bCallerIDAvailable = FALSE;
		ucCallStatus = 0;						
		usCallNumberUpper2Bytes = 0xFFFF;		
		ulCallNumberLower4Bytes = 0xFFFFFFFF;	

		//Page 4 
		ucPage4AlertID = 0;				
		ucNumOfSubpages = 0;				
		usPage4SerialNumber = 0xFFFF;	

		//Page 5
		ucSubpageNum = 0;			
		usUTF8TextUpper2Bytes = 0;	
		ulUTF8TextLower4Bytes = 0;	

		//Page 6
		ucUnreadSMSCount = UNKNOWN_BYTE;		 
		ucUnreadEmailCount = UNKNOWN_BYTE;		
		ucUnreadTwitterCount = UNKNOWN_BYTE;	
		ucUnreadFacebookCount = UNKNOWN_BYTE;	

      // Page 7
      ucVideoVolume = 0;			
	   usRemainingTime = 0;		
		usTimeProgressed = 0;		
		ucVideoCapabilities = 0;	
      ucVideoState = (UCHAR)AntPlusControls::VideoStates::OFF;			

      //Advanced Capabilities Page 8
      ucSubpage1 = 0;
      ucSubpage2 = 0;
      usDataField1 = 0;
      usDataField2 = 0;

		//Page 16
		usPage16SerialNumber = UNKNOWN_SHORT;	
		ucPage16Volume = 0xFF;					
		ucPage16Command = 3;					

		//Page 17 
		usPage17SerialNumber = UNKNOWN_SHORT;	
		ulPage17Command = 0x00000000;			

		//Page 18 
		usPage18SerialNumber = UNKNOWN_SHORT;	
		ucPage18Command = UNKNOWN_BYTE;			

		//Page 19
		usPage19SerialNumber = UNKNOWN_SHORT;	
		ucPage19Command = RESERVED_BYTE_0x00;	 

		//Page 20
		usPage20SerialNumber = UNKNOWN_SHORT;		
		ucPage20ChannelFreq = RESERVED_BYTE;					
		ucPage20ChannelPeriod = RESERVED_BYTE;
		ucPage20NumberOfSubpages = ALL_SUBPAGES;	
		ucPage20Command = 0;						 
		
		//Page 21
		usPage21SerialNumber = UNKNOWN_SHORT;	
		ucPage21Command = UNKNOWN_BYTE;			 
	}

	~AntPlusControls()
	{
	}

public:

	/**************************************************************************
	* AntPlusControls::Decode
	* 
	* Decodes all main data pages 
	* Exceptions are thrown when dealing with non compliant pages
	* Further breakdowns inside the fields are dealt win in the display method
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
		case PAGE_AUDIO_UPDATE:
			{
				ucVolume = pucRxBuffer_[1];										//Volume as a percent
				usTotalTrackTime = pucRxBuffer_[2] | (pucRxBuffer_[3] << 8);	//Total time in current track (s) 
				usCurrentTrackTime = pucRxBuffer_[4] | (pucRxBuffer_[5] << 8);	//Elapsed time in current track (s)
				ucAudioCapabilities = pucRxBuffer_[6];							//Audio device capabilities bitfield
				ucState = pucRxBuffer_[7];										//Current state of audio device
				break;
			}
		case PAGE_DEVICE_AVAILABILITY:
			{
				ucCurrentNotifications = pucRxBuffer_[1];	//Text/Call notifications bitfield
				ucDeviceAvailability = pucRxBuffer_[7];		//Available devices bitfield
				break;
			}
		case PAGE_CALL_STATUS:
			{
            if(pucRxBuffer_[1] & 0x80)
               bCallerIDAvailable = TRUE;
            else
               bCallerIDAvailable = FALSE;

				ucCallStatus = (pucRxBuffer_[1] & 0x7F);										//Call status of the device
				usCallNumberUpper2Bytes = pucRxBuffer_[6] | (pucRxBuffer_[7] << 8);	//Upper 2 bytes  of call number
				ulCallNumberLower4Bytes = pucRxBuffer_[2] | (pucRxBuffer_[3] << 8)	//Lower 4 bytes of call number
					| (pucRxBuffer_[4] << 16) | (pucRxBuffer_[5] << 24);  
				break;
			}
		case PAGE_TEXT_STATUS:
			{
				ucPage4AlertID = pucRxBuffer_[1];		//Alert ID of current notification
				ucNumOfSubpages = pucRxBuffer_[2];		//Number of subpages needed to transmit notification
				usPage4SerialNumber = pucRxBuffer_[6]	//Serial Number of the device that is registered
					| (pucRxBuffer_[7] << 8);
				break;
			}
		case PAGE_TEXT_SUBPAGE:
			{
				ucSubpageNum = pucRxBuffer_[1];										//Subpage number
				usUTF8TextUpper2Bytes = pucRxBuffer_[6] | (pucRxBuffer_[7] << 8);	//Upper 2 bytes of UTF-8 data
				ulUTF8TextLower4Bytes = pucRxBuffer_[2] | (pucRxBuffer_[3] << 8)	//Lower 4 bytes of UTF-8 data
					| (pucRxBuffer_[4] << 16) | (pucRxBuffer_[5] << 24);
				break;
			}
		case PAGE_TEXT_COUNT:
			{
				ucUnreadSMSCount = pucRxBuffer_[4];			//Number of unread SMS messages
				ucUnreadEmailCount = pucRxBuffer_[5];		//Number of unread emails
				ucUnreadTwitterCount = pucRxBuffer_[6];		//Number of unread twitter updates
				ucUnreadFacebookCount = pucRxBuffer_[7];	//Number of unread facebook updates
				break;
			}
		case PAGE_VIDEO_UPDATE:
			{
				ucVideoVolume = pucRxBuffer_[1];
				usRemainingTime = pucRxBuffer_[2] | (pucRxBuffer_[3] << 8);
				usTimeProgressed = pucRxBuffer_[4] | (pucRxBuffer_[5] << 8);
				ucVideoCapabilities = pucRxBuffer_[6];
				ucVideoState = pucRxBuffer_[7];
				break;
			}
      case PAGE_ADVANCED_CAPABILITIES:
         {
            ucSubpage1 = pucRxBuffer_[2];
            ucSubpage2 = pucRxBuffer_[3];
            usDataField1 = pucRxBuffer_[4] | (pucRxBuffer_[5] << 8);
            usDataField2 = pucRxBuffer_[6] | (pucRxBuffer_[7] << 8);
         }
         break;
		case PAGE_AUDIO_VIDEO_COMMAND:
			{
				usPage16SerialNumber = pucRxBuffer_[1]	//Remote device serial number
					| (pucRxBuffer_[2] << 8);	
				ucPage16Volume = pucRxBuffer_[6];		//Volume Increment if volume command received
				ucPage16Command = pucRxBuffer_[7];		//Audio Control Command
				break;
			}
		case PAGE_UTF_8_CHARACTER_COMMAND:
			{
				usPage17SerialNumber = pucRxBuffer_[1] | (pucRxBuffer_[2] << 8);	//Remote device serial number
				ulPage17Command = pucRxBuffer_[4] | (pucRxBuffer_[5] << 8)			//UTF-8 character command
					| (pucRxBuffer_[6] << 16) | (pucRxBuffer_[7] << 24);
				break;
			}
		case PAGE_CUSTOM_COMMAND:
			{
				usPage18SerialNumber = pucRxBuffer_[1]	//Remote device serial number
					| (pucRxBuffer_[2] << 8);	
				ucPage18Command = pucRxBuffer_[7];		//Custom command
				break;
			}
		case PAGE_CALL_COMMAND:
			{
				usPage19SerialNumber = pucRxBuffer_[1]	//Remote device serial number 
					| (pucRxBuffer_[2] << 8);	
				ucPage19Command = pucRxBuffer_[7];		//Call command
				break;
			}
		case PAGE_TEXT_COMMAND:
			{
				usPage20SerialNumber = pucRxBuffer_[1]		//Remote device serial number 
					| (pucRxBuffer_[2] << 8);
				ucPage20ChannelFreq = pucRxBuffer_[4];			//Channel frequency requested
				ucPage20ChannelPeriod = pucRxBuffer_[5];		//Channel Period requeted
				ucPage20NumberOfSubpages  = pucRxBuffer_[6];	//Number of supbages requested
				ucPage20Command = pucRxBuffer_[7];				//Text Command requested
				break;
			}
		default:
			{
				break;
			}
		}
	}



   /**************************************************************************
    * AntPlusControls::Encode
    * 
    * Encodes the Controls Device Profile data pages
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
		case PAGE_AUDIO_UPDATE:
			{
				pucTxBuffer_[0] = ucPageNum_;							//Page number (0x01)
				pucTxBuffer_[1] = ucVolume;								//Volume as a percent (0-100)
				pucTxBuffer_[2] = usTotalTrackTime & 0x00FF;			//Mask off MSB
				pucTxBuffer_[3] = (usTotalTrackTime >> 8) & 0x00FF;		//Shift MSB right 8 bits, mask off the unwanted byte
				pucTxBuffer_[4] = usCurrentTrackTime & 0x00FF;			//Mask off MSB
				pucTxBuffer_[5] = (usCurrentTrackTime >> 8) & 0x00FF;	//Shift right 8 bits, mask off the unwanted byte
				pucTxBuffer_[6] = ucAudioCapabilities;					//Capabilities of Repeat and Shuffle states
				pucTxBuffer_[7] = ucState;								//Current state of audio device
				break;
			}
		case PAGE_DEVICE_AVAILABILITY:
			{
				pucTxBuffer_[0] = ucPageNum_;				//Page number (0x02)
				pucTxBuffer_[1] = ucCurrentNotifications;	//Notifications bitfield
				pucTxBuffer_[2] = RESERVED_BYTE_0x00;
				pucTxBuffer_[3] = RESERVED_BYTE_0x00;
				pucTxBuffer_[4] = RESERVED_BYTE_0x00;
				pucTxBuffer_[5] = RESERVED_BYTE_0x00;
				pucTxBuffer_[6] = RESERVED_BYTE_0x00;
				pucTxBuffer_[7] = ucDeviceAvailability;		//Available device bitfield
				break;
			}
		case PAGE_CALL_STATUS:
			{
				pucTxBuffer_[0] = ucPageNum_;									//Page Number (0x03)
				pucTxBuffer_[1] = ucCallStatus;								//Call status

            if(bCallerIDAvailable)
               pucTxBuffer_[1] |= 0x80;
            else
               pucTxBuffer_[1] &= 0x7F;

				pucTxBuffer_[2] = ulCallNumberLower4Bytes & 0x000000FF;			//Mask off all but the LSB of the Call Number
				pucTxBuffer_[3] = (ulCallNumberLower4Bytes >> 8) & 0x000000FF;	//Shift call number right 8 bits (1 byte), mask off unwanted bytes 
				pucTxBuffer_[4] = (ulCallNumberLower4Bytes >> 16) & 0x000000FF;	//Shift call number right 16 bits (2 bytes), mask off unwanted bytes
				pucTxBuffer_[5] = (ulCallNumberLower4Bytes >> 24) & 0x000000FF;	//Shift call number right 25 bits (3 bytes), mask off unwanted bytes
				pucTxBuffer_[6] = usCallNumberUpper2Bytes & 0x00FF;				//Mask off MSB of Call Number
				pucTxBuffer_[7] = (usCallNumberUpper2Bytes >> 8) & 0x00FF;		//Shift call number right 8 bits (1 byte), mask off unwanted byte
				break;
			}
		case PAGE_TEXT_STATUS:
			{
				pucTxBuffer_[0] = ucPageNum_;		//Page Number (0x04)
				pucTxBuffer_[1] = ucPage4AlertID;	//Alert ID of the message received (SMS, email, twitter, fb etc)
				pucTxBuffer_[2] = ucNumOfSubpages;	//Number of subpages indicates pages to include accompanying text
				pucTxBuffer_[3] = RESERVED_BYTE;	
				pucTxBuffer_[4] = RESERVED_BYTE;
				pucTxBuffer_[5] = RESERVED_BYTE;
				pucTxBuffer_[6] = usPage4SerialNumber & 0x00FF;
				pucTxBuffer_[7] = (usPage4SerialNumber >> 8) & 0x00FF;
				break;
			}
		case PAGE_TEXT_SUBPAGE:
			{
				pucTxBuffer_[0] = ucPageNum_;									//Page Number (0x05)
				pucTxBuffer_[1] = ucSubpageNum;									//Subpage number of the received message
				pucTxBuffer_[2] = ulUTF8TextLower4Bytes & 0x000000FF;			//Mask off all  but the LSB of the UTF8 Text
				pucTxBuffer_[3] = (ulUTF8TextLower4Bytes >> 8) & 0x000000FF;	//Shift UTF8 Text right 8 bits (1 byte), mask off unwanted bytes
				pucTxBuffer_[4] = (ulUTF8TextLower4Bytes >> 16) & 0x000000FF;	//shift UTF8 Text right 16 bits (2 bytes), mask off unwanted bytes
				pucTxBuffer_[5] = (ulUTF8TextLower4Bytes >> 24) & 0x000000FF;	//Shift UTF8 Text right 24 bits (3 byte), mask off unwanted bytes
				pucTxBuffer_[6] = usUTF8TextUpper2Bytes & 0x00FF;				//Mask off MSB of UTF8 Text
				pucTxBuffer_[7] = (usUTF8TextUpper2Bytes >> 8) & 0x00FF;		//Shift UTF8 Text right 8 bits (1 byte), mask off unwanted bytes
				break;
			}
		case PAGE_TEXT_COUNT:
			{
				pucTxBuffer_[0] = ucPageNum_;
				pucTxBuffer_[1] = RESERVED_BYTE;
				pucTxBuffer_[2] = RESERVED_BYTE;
				pucTxBuffer_[3] = RESERVED_BYTE;
				pucTxBuffer_[4] = ucUnreadSMSCount;
				pucTxBuffer_[5] = ucUnreadEmailCount;
				pucTxBuffer_[6] = ucUnreadTwitterCount;
				pucTxBuffer_[7] = ucUnreadFacebookCount;
				break;
			}
		case PAGE_VIDEO_UPDATE:
			{
				pucTxBuffer_[0] = ucPageNum_;
				pucTxBuffer_[1] = ucVideoVolume;
				pucTxBuffer_[2] = usRemainingTime & 0xFF;
				pucTxBuffer_[3] = (usRemainingTime >> 8) & 0xFF;
				pucTxBuffer_[4] = usTimeProgressed & 0xFF;
				pucTxBuffer_[5] = (usTimeProgressed >> 8) & 0xFF;
				pucTxBuffer_[6] = ucVideoCapabilities;
				pucTxBuffer_[7] = ucVideoState;
				break;
			}
      case PAGE_ADVANCED_CAPABILITIES:
         {
            pucTxBuffer_[0] = ucPageNum_;
            pucTxBuffer_[1] = RESERVED_BYTE;
            pucTxBuffer_[2] = ucSubpage1;
            pucTxBuffer_[3] = ucSubpage2;
            pucTxBuffer_[4] = usDataField1 & 0xFF;
            pucTxBuffer_[5] = (usDataField1 >> 8) & 0xFF;
            pucTxBuffer_[6] = usDataField2 & 0xFF;
            pucTxBuffer_[7] = (usDataField2 >> 8) & 0xFF;
            break;
         }
		case PAGE_AUDIO_VIDEO_COMMAND:
			{
				pucTxBuffer_[0] = ucPageNum_;							         //Page Number (0x10)
				pucTxBuffer_[1] = usPage16SerialNumber & 0x00FF;		   //Mask off Serial Number MSB
				pucTxBuffer_[2] = (usPage16SerialNumber >> 8) & 0x00FF;	//Shift serial number right 8 bits, mask off unwanted byte
				pucTxBuffer_[3] = RESERVED_BYTE;						         //Bytes 3 through 6 are reserved
				pucTxBuffer_[4] = RESERVED_BYTE;
				pucTxBuffer_[5] = RESERVED_BYTE;
				pucTxBuffer_[6] = ucPage16Volume;
				pucTxBuffer_[7] = ucPage16Command;						      //Command requested by remote device
				break;
			}
		case PAGE_UTF_8_CHARACTER_COMMAND:
			{
				pucTxBuffer_[0] = ucPageNum_;							         //Page number (0x11)
				pucTxBuffer_[1] = usPage17SerialNumber & 0x00FF;		   //Mask off serial number MSB
				pucTxBuffer_[2] = (usPage17SerialNumber >> 8) & 0x00FF;	//Shift serial number right 8 bits, mask off unwated byte
				pucTxBuffer_[3] = RESERVED_BYTE;						         //Byte 3 is reserved
				pucTxBuffer_[4] = ulPage17Command & 0x000000FF;			   //Mask off all but LSB of UTF-8 command
				pucTxBuffer_[5] = (ulPage17Command >> 8) & 0x000000FF;	//Shift UTF-8 command right 8 bits, mask off unwanted bytes
				pucTxBuffer_[6] = (ulPage17Command >> 16) & 0x000000FF;	//Shift UTF-8 command right 16 bits, mask off unwanted bytes
				pucTxBuffer_[7] = (ulPage17Command >> 24) & 0x000000FF;	//Shift UTF-8 command right 24 bits, mask off unwanted bytes
				break;
			}
		case PAGE_CUSTOM_COMMAND:
			{
				pucTxBuffer_[0] = ucPageNum_;							         //Page number (0x12)
				pucTxBuffer_[1] = usPage18SerialNumber & 0x00FF;		   //Mask off serial number MSB
				pucTxBuffer_[2] = (usPage18SerialNumber >> 8) & 0x00FF;	//Shift serial number right 8 bits, mask off unwanted byte
				pucTxBuffer_[3] = RESERVED_BYTE;						         //Bytes 3 through 6 are reserved
				pucTxBuffer_[4] = RESERVED_BYTE;
				pucTxBuffer_[5] = RESERVED_BYTE;
				pucTxBuffer_[6] = RESERVED_BYTE;
				pucTxBuffer_[7] = ucPage18Command;						      //Custom command (application specific)
				break;
			}
		case PAGE_CALL_COMMAND:
			{
				pucTxBuffer_[0] = ucPageNum_;							         //Page number (0x13)
				pucTxBuffer_[1] = usPage19SerialNumber & 0x00FF;		   //Mask off serial Number MSB
				pucTxBuffer_[2] = (usPage19SerialNumber >> 8) & 0x00FF;  //Shift serial number right 8 bits (1 byte), mask off unwanted byte
				pucTxBuffer_[3] = RESERVED_BYTE;
				pucTxBuffer_[4] = RESERVED_BYTE;
				pucTxBuffer_[5] = RESERVED_BYTE;
				pucTxBuffer_[6] = RESERVED_BYTE;
				pucTxBuffer_[7] = ucPage19Command;						      //Call control command requested by remote device
				break;
			}
		case PAGE_TEXT_COMMAND:
			{
				pucTxBuffer_[0] = ucPageNum_;							         //Page number (0x14)
				pucTxBuffer_[1] = usPage20SerialNumber & 0x00FF;		   //Mask off serial number MSB
				pucTxBuffer_[2] = (usPage20SerialNumber >> 8) & 0x00FF;	//Shift serial number right 8 bits (1 byte), mask off unwanted byte
				pucTxBuffer_[3] = RESERVED_BYTE;
				pucTxBuffer_[4] = ucPage20ChannelFreq;					      //RF channel frequency at which text data exchange will take place
				pucTxBuffer_[5] = ucPage20ChannelPeriod;				      //Channel period at which text data exchange will take place
				pucTxBuffer_[6] = ucPage20NumberOfSubpages;				   //Indicates the request
				pucTxBuffer_[7] = ucPage20Command;						      //Text command requested by remote device
				break;
			}
		default:
			{
				break;
			}
		}
	}
};