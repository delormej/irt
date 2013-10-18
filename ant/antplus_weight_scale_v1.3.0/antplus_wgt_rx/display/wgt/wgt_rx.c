/*
This software is subject to the license described in the License.txt file
included with this software distribution. You may not use this file except in compliance
with this license.

Copyright (c) Dynastream Innovations Inc. 2013
All rights reserved.
*/

//Vipin Bali Bakshi
//Date:June 9, 2010
/////////////////////////////////////////////////////////////////////////////////////////
#include "types.h"
#include "antinterface.h"
#include "antmessage.h"
#include "antdefines.h"
#include "wgt_rx.h"
#include "antplus.h"
#include "common_pages.h"


//ANT Channel
#define WGTRX_CHANNEL_TYPE		((UCHAR) 0x00) //Slave

//Channel ID
#define WGTRX_DEVICE_TYPE		((UCHAR) 0x77) //WeightScale

//Channel Period
#define WGTRX_CHANNEL_PERIOD	((ULONG) 0x2000)	//4Hz

static WGT_DataPage1 stDataPage1;
static WGT_DataPage2 stDataPage2;
static WGT_DataPage3 stDataPage3;
static WGT_DataPage4 stDataPage4;
static WGT_UserProfilePage58 stUserProfilePage58;
//static CommonPage70_Data stCommonDataPage70;			//ANT_FS capabilities not in use
static CommonPage80_Data stCommonDataPage80;
static CommonPage81_Data stCommonDataPage81;
static CommonPage82_Data stCommonDataPage82;
static Display_UserProfilePage stDisplay_UserProfilePage;

static UCHAR ucAntChannel;
static USHORT usDeviceNumber;
static UCHAR ucTransType;
static BOOL bUserProfileSent = 0;						//flag to check if Valid USer Profile Tranmitted to WGT. Used by TransmitUserProfile()

//Local Functions
static BOOL HandleResponseEvents(UCHAR* pucBuffer_);
static BOOL HandleDataMessages(UCHAR* pucBuffer_, ANTPLUS_EVENT_RETURN* pstEventStruct_);


void WGTRX_Init()  												//Initialize the global datapage structs
{
	WGT_DataPage1* pstDataPage1 = WGT_GetDataPage1();			//pointers pointing to the global datapage structs
	WGT_DataPage2* pstDataPage2 = WGT_GetDataPage2();
	WGT_DataPage3* pstDataPage3 = WGT_GetDataPage3();
	WGT_DataPage4* pstDataPage4 = WGT_GetDataPage4();
	WGT_UserProfilePage58* pstUserProfilePage58 = WGT_GetUserProfilePage58();
	//CommonPage70_Data* pstCommonDataPage70 = WGT_GetCommonDataPage70();      //ANT_FS capabilities not in use
	CommonPage80_Data* pstCommonDataPage80 = WGT_GetCommonDataPage80();
	CommonPage81_Data* pstCommonDataPage81 = WGT_GetCommonDataPage81();
	CommonPage82_Data* pstCommonDataPage82 = WGT_GetCommonDataPage82();
	
	pstDataPage1->usWeightValue = 0;										//Initialize data values to be displayed 
	pstDataPage2->usHydration = 0;
	pstDataPage2->usBodyFat = 0;
	pstDataPage3->usActiveRate = 0;
	pstDataPage3->usBasalRate = 0;
	pstDataPage4->usMuscleMass = 0;
	pstDataPage4->ucBoneMass = 0;
	pstUserProfilePage58->ucGender = 0;
	pstUserProfilePage58->ucAge = 0;
	pstUserProfilePage58->ucHeight = 0;
	pstCommonDataPage80->ucHwVersion = 0;
	pstCommonDataPage80->usManId = 0;
	pstCommonDataPage80->usModelNumber = 0;
	pstCommonDataPage80->ucPg80ReserveByte1 = 0xFF;
	pstCommonDataPage80->ucPg80ReserveByte2 = 0xFF;
	pstCommonDataPage81->ucSwVersion = 0;
	pstCommonDataPage81->ulSerialNumber = 0;
	pstCommonDataPage81->ucPg81ReserveByte1 = 0xFF;
	pstCommonDataPage81->ucPg81ReserveByte2 = 0xFF;
	pstCommonDataPage82->ulCumOperatingTime = 0;
	pstCommonDataPage82->ucBattVoltage256 = 0;
	pstCommonDataPage82->ucBattVoltage = 0;
	pstCommonDataPage82->ucBattStatus = 0;
	pstCommonDataPage82->ucCumOperatingTimeRes = 0;
} 

/////////////////////////////////////////////////////////////////////////////////////////
// Functon: WGTRX_Open
//
// Descrption:
// Opens WGT recieve channel. Once the channel has been properly initialize an WGTRX_EVENT_INIT_COMPLETE
// event will be generated via the callback. A positive response from this function does not
// indicate that the channel successfully opened. 
//
// Params:
// usDeviceNumber_: Device number to pair to. 0 for wild-card.
//
// returns: TRUE if all configuration messages were correctly setup and queued. FALSE otherwise. 
/////////////////////////////////////////////////////////////////////////////////////////
BOOL WGTRX_Open(UCHAR ucAntChannel_, USHORT usSearchDeviceNumber_, UCHAR ucTransType_)				//Open and Assign RX Channel
{
	WGTRX_Init();
	
	ucAntChannel = ucAntChannel_;
	usDeviceNumber = usSearchDeviceNumber_;
	ucTransType = ucTransType_;
	
	
   if(!ANT_AssignChannel(ucAntChannel,WGTRX_CHANNEL_TYPE,ANTPLUS_NETWORK_NUMBER ))
      return FALSE;
 
   return(TRUE);
}


/////////////////////////////////////////////////////////////////////////////////////////
// Functon: WGTRX_Close
//
// Description:
// Closes WGT recieve channel and initializes all state variables. Once the channel has 
// been successfuly closed, an WGTRX_EVENT_CHANNEL_CLOSED event will be generated via the
// callback function/
//
// Params:
// N/A
//
// Returns: TRUE if message was successfully sent to ANT, FALSE otherwise. 
// 
/////////////////////////////////////////////////////////////////////////////////////////
BOOL WGTRX_Close()																		//Close Rx Channel
{
   ANT_CloseChannel(ucAntChannel);
   return(TRUE);
}

//Get functions to reference to Data Pages 
/////////////////////////////////////////////////////////////////////////////////////////
// Functon: WGTRX_GetDataPage1
//
// Descrption:
// Returns a pointer to the page 0 buffer. This function should be called following 
// an WGTRX_EVENT_PAGE 0 event to extract the latest data from page 0. 
//
// Params:
// N/A
//
// returns: Pointer to Page 0 buffer, NULL if data not valid.  
/////////////////////////////////////////////////////////////////////////////////////////
WGT_DataPage1* WGT_GetDataPage1()								//Get Functions referencing to the global data pages. Can be called
{																//when wanting to read or write to these data page structs				
	return &stDataPage1;	
}


//Get functions to reference to Data Pages 
/////////////////////////////////////////////////////////////////////////////////////////
// Functon: WGTRX_GetDataPage2
//
// Descrption:
// Returns a pointer to the page 0 buffer. This function should be called following 
// an WGTRX_EVENT_PAGE 0 event to extract the latest data from page 0. 
//
// Params:
// N/A
//
// returns: Pointer to Page 0 buffer, NULL if data not valid.  
/////////////////////////////////////////////////////////////////////////////////////////
WGT_DataPage2* WGT_GetDataPage2()
{
	return &stDataPage2;
}


//Get functions to reference to Data Pages 
/////////////////////////////////////////////////////////////////////////////////////////
// Functon: WGTRX_GetDataPage3
//
// Descrption:
// Returns a pointer to the page 0 buffer. This function should be called following 
// an WGTRX_EVENT_PAGE 0 event to extract the latest data from page 0. 
//
// Params:
// N/A
//
// returns: Pointer to Page 0 buffer, NULL if data not valid.  
/////////////////////////////////////////////////////////////////////////////////////////
WGT_DataPage3* WGT_GetDataPage3()
{
	return &stDataPage3;
}


//Get functions to reference to Data Pages 
/////////////////////////////////////////////////////////////////////////////////////////
// Functon: WGTRX_GetDataPage4
//
// Descrption:
// Returns a pointer to the page 0 buffer. This function should be called following 
// an WGTRX_EVENT_PAGE 0 event to extract the latest data from page 0. 
//
// Params:
// N/A
//
// returns: Pointer to Page 0 buffer, NULL if data not valid.  
/////////////////////////////////////////////////////////////////////////////////////////
WGT_DataPage4* WGT_GetDataPage4()
{
	return &stDataPage4;
}


//Get functions to reference to Data Pages 
/////////////////////////////////////////////////////////////////////////////////////////
// Functon: WGTRX_GetDataPage58
//
// Descrption:
// Returns a pointer to the page 0 buffer. This function should be called following 
// an WGTRX_EVENT_PAGE 0 event to extract the latest data from page 0. 
//
// Params:
// N/A
//
// returns: Pointer to Page 0 buffer, NULL if data not valid.  
/////////////////////////////////////////////////////////////////////////////////////////
WGT_UserProfilePage58* WGT_GetUserProfilePage58()
{
	return &stUserProfilePage58;
}


//Get functions to reference to Data Pages 
/////////////////////////////////////////////////////////////////////////////////////////
// Functon: Display_GetDataPage
//
// Descrption:
// Returns a pointer to the DISPLAY's user profile page buffer. This function should be called during Initialization to load the device with 
// the required User Data. 
//
// Params:
// N/A
//
// returns: Pointer to Page 0 buffer, NULL if data not valid.  
Display_UserProfilePage* Display_GetUserProfilePage()
{
	return &stDisplay_UserProfilePage;
}


//Get functions to reference to Data Pages 
/////////////////////////////////////////////////////////////////////////////////////////
// Functon: WGT_GetCommonDataPage80()
//
// Descrption:
// Returns a pointer to the page 0 buffer. This function should be called following 
// an WGTRX_EVENT_PAGE 0 event to extract the latest data from page 0. 
//
// Params:
// N/A
//
// returns: Pointer to Page 0 buffer, NULL if data not valid.  
/////////////////////////////////////////////////////////////////////////////////////////
CommonPage80_Data* WGT_GetCommonDataPage80()
{
	return &stCommonDataPage80;
}


//Get functions to reference to Data Pages 
/////////////////////////////////////////////////////////////////////////////////////////
// Functon: WGT_GetCommonDataPage81()
//
// Descrption:
// Returns a pointer to the page 0 buffer. This function should be called following 
// an WGTRX_EVENT_PAGE 0 event to extract the latest data from page 0. 
//
// Params:
// N/A
//
// returns: Pointer to Page 0 buffer, NULL if data not valid.  
/////////////////////////////////////////////////////////////////////////////////////////
CommonPage81_Data* WGT_GetCommonDataPage81()
{
	return &stCommonDataPage81;
}


//Get functions to reference to Data Pages 
/////////////////////////////////////////////////////////////////////////////////////////
// Functon: WGT_GetCommonDataPage82()
//
// Descrption:
// Returns a pointer to the page 0 buffer. This function should be called following 
// an WGTRX_EVENT_PAGE 0 event to extract the latest data from page 0. 
//
// Params:
// N/A
//
// returns: Pointer to Page 0 buffer, NULL if data not valid.  
/////////////////////////////////////////////////////////////////////////////////////////
CommonPage82_Data* WGT_GetCommonDataPage82()
{
	return &stCommonDataPage82;
}


//Get functions to reference to Data Pages 
/////////////////////////////////////////////////////////////////////////////////////////
// Functon: WGTRX_GetPage70
//
// Descrption:
// Returns a pointer to the page 0 buffer. This function should be called following 
// an WGTRX_EVENT_PAGE 0 event to extract the latest data from page 0. 
//
// Params:
// N/A
//
// returns: Pointer to Page 0 buffer, NULL if data not valid.  
//ANT_FS capabilities not in use
//CommonPage70_Data* WGT_GetCommonDataPage70()
//{
//	return &stCommonDataPage70;
//}
/////////////////////////////////////////////////////////////////////////////////////////

//Get functions Completed and ready to be Called

/////////////////////////////////////////////////////////////////////
/* WGT_ChannelEvent
 * Process channel event messages for the WGT 
 * pucEventBuffer_: Pointer to ANT message buffer.
 * return: TRUE if buffer handled. 
 * 
 * Checks to see if channel# in buffer matches the ANT assigned channel Number
 * check to see if buffer exists
 * checks Message_ID in buffer to determine either
 * a> RESPONSE_EVENT  to call BOOL HandleResponseEvents(UCHAR* pucBuffer_)
 * b> DATA PAGE EVENT to call BOOL HandleDataMessages(UCHAR* pucBuffer_, ANTPLUS_EVENT_RETURN* pstEventStruct_)
 *///////////////////////////////////////////////////////////////////////
BOOL WGTRX_ChannelEvent(UCHAR* pucEventBuffer_, ANTPLUS_EVENT_RETURN* pstEventStruct_)
{
   BOOL bTransmit = TRUE;
   
   UCHAR ucChannel = pucEventBuffer_[BUFFER_INDEX_CHANNEL_NUM] & 0x1F;
   pstEventStruct_->eEvent = ANTPLUS_EVENT_NONE;
   
   if(ucChannel == ucAntChannel)
   {
      if(pucEventBuffer_)
      {
         UCHAR ucANTEvent = pucEventBuffer_[BUFFER_INDEX_MESG_ID];   
         switch( ucANTEvent )
         {
            case MESG_RESPONSE_EVENT_ID:
            {
               bTransmit = HandleResponseEvents( pucEventBuffer_ );
               break;
            }
            case MESG_BROADCAST_DATA_ID:                    // Handle both BROADCAST, ACKNOWLEDGED and BURST data the same
            case MESG_ACKNOWLEDGED_DATA_ID:
            case MESG_BURST_DATA_ID:    
            {
               bTransmit = HandleDataMessages(pucEventBuffer_, pstEventStruct_);
               break;
               
            } 
            case MESG_CHANNEL_ID_ID:
            {
               usDeviceNumber = (USHORT)pucEventBuffer_[BUFFER_INDEX_MESG_DATA]; 
               usDeviceNumber |= (USHORT)pucEventBuffer_[BUFFER_INDEX_MESG_DATA+1] << 8;
               ucTransType    = (UCHAR)pucEventBuffer_[BUFFER_INDEX_MESG_DATA+3];

               pstEventStruct_->eEvent = ANTPLUS_EVENT_CHANNEL_ID;
               pstEventStruct_->usParam1 = usDeviceNumber;
               pstEventStruct_->usParam2 = ucTransType;
               break;     
            }
         }
      } 
   }
   return (bTransmit);
}

//Handle Response Events
//Sampled directly from HRMrx
BOOL HandleResponseEvents(UCHAR* pucBuffer_)
{

   BOOL bTransmit = TRUE;
   
   if(pucBuffer_ && pucBuffer_[BUFFER_INDEX_RESPONSE_CODE] == RESPONSE_NO_ERROR)
   {
      switch(pucBuffer_[BUFFER_INDEX_RESPONSE_MESG_ID])
      {
         case MESG_ASSIGN_CHANNEL_ID:
         {
            ANT_ChannelId(ucAntChannel, usDeviceNumber,WGTRX_DEVICE_TYPE, ucTransType );
            break;
         }
         case MESG_CHANNEL_ID_ID:
         {
            ANT_ChannelRFFreq(ucAntChannel, ANTPLUS_RF_FREQ);
            break;
         }
         case MESG_CHANNEL_RADIO_FREQ_ID:
         {
            ANT_ChannelPeriod(ucAntChannel, WGTRX_CHANNEL_PERIOD);
            break;
         }
         case MESG_CHANNEL_MESG_PERIOD_ID:
         {
            ANT_OpenChannel(ucAntChannel);
            break;
         }
         case MESG_OPEN_CHANNEL_ID:
         case MESG_CLOSE_CHANNEL_ID:            // Fallthrough
         default:
         {
            bTransmit = FALSE;                  // Can go back to sleep   
         }
      }
   }
   return(bTransmit);
}



//Handle Data Pages
BOOL HandleDataMessages(UCHAR* pucBuffer_, ANTPLUS_EVENT_RETURN* pstEventStruct_)
{
	BOOL bTransmit = FALSE;
	UCHAR ucPageNumber = pucBuffer_[BUFFER_INDEX_MESG_DATA];
	
	if(pucBuffer_)
	{
		switch(ucPageNumber)
		{
			case WEIGHT_PAGE_BODY_WEIGHT:
			{
				WGT_DataPage1* pstDataPage1 = WGT_GetDataPage1();
				
				pstDataPage1->usUserProfileInformation = (USHORT)pucBuffer_[BUFFER_INDEX_MESG_DATA +1];
				pstDataPage1->usUserProfileInformation |= (USHORT)pucBuffer_[BUFFER_INDEX_MESG_DATA +2] << 8;
				pstDataPage1->ucCapabilitiesField = (USHORT)pucBuffer_[BUFFER_INDEX_MESG_DATA +3];
				pstDataPage1->usWeightValue = (USHORT)pucBuffer_[BUFFER_INDEX_MESG_DATA + 6];
				pstDataPage1->usWeightValue |=(USHORT)pucBuffer_[BUFFER_INDEX_MESG_DATA + 7] << 8;
				
				pstEventStruct_->eEvent = ANTPLUS_EVENT_PAGE;
				pstEventStruct_->usParam1 = WEIGHT_PAGE_BODY_WEIGHT;
				break;
			}
			case WEIGHT_PAGE_BODY_COMPOSITION:
			{
				WGT_DataPage2* pstDataPage2 = WGT_GetDataPage2();
				
				pstDataPage2->usUserProfileInformation = (USHORT)pucBuffer_[BUFFER_INDEX_MESG_DATA + 1];
				pstDataPage2->usUserProfileInformation |= (USHORT)pucBuffer_[BUFFER_INDEX_MESG_DATA + 2] << 8;
				pstDataPage2->usHydration = (USHORT)pucBuffer_[BUFFER_INDEX_MESG_DATA + 4];
				pstDataPage2->usHydration |= (USHORT)pucBuffer_[BUFFER_INDEX_MESG_DATA + 5] << 8;
				pstDataPage2->usBodyFat = (USHORT)pucBuffer_[BUFFER_INDEX_MESG_DATA + 6];
				pstDataPage2->usBodyFat |= (USHORT)pucBuffer_[BUFFER_INDEX_MESG_DATA + 7] << 8;
					
				pstEventStruct_->eEvent = ANTPLUS_EVENT_PAGE;
				pstEventStruct_->usParam1 = WEIGHT_PAGE_BODY_COMPOSITION;
				break;
			}
			case WEIGHT_PAGE_METABOLIC_INFO:
			{
				WGT_DataPage3* pstDataPage3 = WGT_GetDataPage3();
				
				pstDataPage3->usUserProfileInformation = (USHORT)pucBuffer_[BUFFER_INDEX_MESG_DATA + 1];
				pstDataPage3->usUserProfileInformation |= (USHORT)pucBuffer_[BUFFER_INDEX_MESG_DATA + 2] << 8;
				pstDataPage3->usActiveRate = (USHORT)pucBuffer_[BUFFER_INDEX_MESG_DATA + 4];
				pstDataPage3->usActiveRate |= (USHORT)pucBuffer_[BUFFER_INDEX_MESG_DATA + 5] << 8;
				pstDataPage3->usBasalRate = (USHORT)pucBuffer_[BUFFER_INDEX_MESG_DATA + 6];
				pstDataPage3->usBasalRate |= (USHORT)pucBuffer_[BUFFER_INDEX_MESG_DATA + 7] << 8;
					
				pstEventStruct_->eEvent = ANTPLUS_EVENT_PAGE;
				pstEventStruct_->usParam1 = WEIGHT_PAGE_METABOLIC_INFO ;
				break;
			}
			case WEIGHT_PAGE_BODY_MASS:
			{
				WGT_DataPage4* pstDataPage4 = WGT_GetDataPage4();
				
				pstDataPage4->usUserProfileInformation = (USHORT)pucBuffer_[BUFFER_INDEX_MESG_DATA + 1];
				pstDataPage4->usUserProfileInformation |= (USHORT)pucBuffer_[BUFFER_INDEX_MESG_DATA + 2] << 8;
				pstDataPage4->usMuscleMass = (USHORT)pucBuffer_[BUFFER_INDEX_MESG_DATA + 5];
				pstDataPage4->usMuscleMass |= (USHORT)pucBuffer_[BUFFER_INDEX_MESG_DATA + 6] << 8;
				pstDataPage4->ucBoneMass = (UCHAR)pucBuffer_[BUFFER_INDEX_MESG_DATA + 7];
					
				pstEventStruct_->eEvent = ANTPLUS_EVENT_PAGE;
				pstEventStruct_->usParam1 = WEIGHT_PAGE_BODY_MASS;
				break;
			}
			case WEIGHT_PAGE_USER_PROFILE :
			{
				WGT_UserProfilePage58* pstUserProfilePage58 = WGT_GetUserProfilePage58();
				
				pstUserProfilePage58->usUserProfileIdentification = (USHORT)pucBuffer_[BUFFER_INDEX_MESG_DATA + 1];
				pstUserProfilePage58->usUserProfileIdentification |= (USHORT)pucBuffer_[BUFFER_INDEX_MESG_DATA + 2] << 8;
				pstUserProfilePage58->ucCapabilitiesBitField = (UCHAR)pucBuffer_[BUFFER_INDEX_MESG_DATA + 3];
				pstUserProfilePage58->ucGender = (UCHAR)(pucBuffer_[BUFFER_INDEX_MESG_DATA + 5] & 0x01);
				pstUserProfilePage58->ucAge = (UCHAR)(pucBuffer_[BUFFER_INDEX_MESG_DATA + 5] & 0x7F);
				pstUserProfilePage58->ucHeight = (UCHAR)pucBuffer_[BUFFER_INDEX_MESG_DATA + 6];
				pstUserProfilePage58->ucDescriptiveBitField = (UCHAR)pucBuffer_[BUFFER_INDEX_MESG_DATA + 7];
					
				pstEventStruct_->eEvent = ANTPLUS_EVENT_PAGE;
				pstEventStruct_->usParam1 = WEIGHT_PAGE_USER_PROFILE;
				break;
			}
			case WEIGHT_COMMON_PAGE80:
			{
				CommonPage80_Data* pstCommonDataPage80 = WGT_GetCommonDataPage80();
				
				pstCommonDataPage80->ucHwVersion = (UCHAR)pucBuffer_[BUFFER_INDEX_MESG_DATA + 3];
				pstCommonDataPage80->usManId = (USHORT)pucBuffer_[BUFFER_INDEX_MESG_DATA + 4];
				pstCommonDataPage80->usManId |= (USHORT)pucBuffer_[BUFFER_INDEX_MESG_DATA + 5] << 8;
				pstCommonDataPage80->usModelNumber = (USHORT)pucBuffer_[BUFFER_INDEX_MESG_DATA + 6];
				pstCommonDataPage80->usModelNumber |= (USHORT)pucBuffer_[BUFFER_INDEX_MESG_DATA + 7] << 8;
				
					
				pstEventStruct_->eEvent = ANTPLUS_EVENT_PAGE;
				pstEventStruct_->usParam1 = WEIGHT_COMMON_PAGE80;
				break;
			}
			case WEIGHT_COMMON_PAGE81:
			{
				CommonPage81_Data* pstCommonDataPage81 = WGT_GetCommonDataPage81();
				
				pstCommonDataPage81->ucSwVersion = (UCHAR)pucBuffer_[BUFFER_INDEX_MESG_DATA + 3];
				
				pstCommonDataPage81->ulSerialNumber = (ULONG)pucBuffer_[BUFFER_INDEX_MESG_DATA + 4];
				pstCommonDataPage81->ulSerialNumber |= (ULONG)pucBuffer_[BUFFER_INDEX_MESG_DATA + 5] << 8;
				pstCommonDataPage81->ulSerialNumber |= (ULONG)pucBuffer_[BUFFER_INDEX_MESG_DATA + 6] << 16;
				pstCommonDataPage81->ulSerialNumber |= (ULONG)pucBuffer_[BUFFER_INDEX_MESG_DATA + 7] << 24;
						
				pstEventStruct_->eEvent = ANTPLUS_EVENT_PAGE;
				pstEventStruct_->usParam1 = WEIGHT_COMMON_PAGE81;
				break;
			}
			case WEIGHT_COMMON_PAGE82:
			{
				CommonPage82_Data* pstCommonDataPage82 = WGT_GetCommonDataPage82();
				
				pstCommonDataPage82->ulCumOperatingTime = (UCHAR)pucBuffer_[BUFFER_INDEX_MESG_DATA + 3];
				pstCommonDataPage82->ulCumOperatingTime |= (ULONG)pucBuffer_[BUFFER_INDEX_MESG_DATA + 4] << 8;
				pstCommonDataPage82->ulCumOperatingTime |= (ULONG)pucBuffer_[BUFFER_INDEX_MESG_DATA + 5] << 16;
				pstCommonDataPage82->ucBattVoltage256 = (ULONG)pucBuffer_[BUFFER_INDEX_MESG_DATA + 6];
				pstCommonDataPage82->ucDescriptiveField = (ULONG)pucBuffer_[BUFFER_INDEX_MESG_DATA + 7];
						
				pstEventStruct_->eEvent = ANTPLUS_EVENT_PAGE;
				pstEventStruct_->usParam1 = WEIGHT_COMMON_PAGE82;
				break;
			}
			/*
			case WEIGHT_PAGE_ANTFS_REQUEST :											//ANT_FS capabilities not in use
			{
				CommonPage70_Data* pstCommonDataPage70 = WGT_GetCommonDataPage70();
				
				pstCommonDataPage70->ucSubfield1 = (UCHAR)pucBuffer_[BUFFER_INDEX_MESG_DATA + 3];
				pstCommonDataPage70->ucSubfield2 = (UCHAR)pucBuffer_[BUFFER_INDEX_MESG_DATA + 4];
				pstCommonDataPage70->ucAckMessages = (UCHAR)pucBuffer_[BUFFER_INDEX_MESG_DATA + 5] ;
				pstCommonDataPage70->ucPageNumber = (UCHAR)pucBuffer_[BUFFER_INDEX_MESG_DATA + 6] ;
				pstCommonDataPage70->ucCommandType = (UCHAR)pucBuffer_[BUFFER_INDEX_MESG_DATA + 7] ;
						
				pstEventStruct_->eEvent = ANTPLUS_EVENT_PAGE;
				pstEventStruct_->usParam1 = WEIGHT_PAGE_ANTFS_REQUEST ;
				break;
			}
				*/
			default:
			{
				pstEventStruct_->eEvent = ANTPLUS_EVENT_UNKNOWN_PAGE;
               	break;
			}
		}
		
	if(usDeviceNumber == 0)
  	{
      if(ANT_RequestMessage(ucAntChannel, MESG_CHANNEL_ID_ID))
         bTransmit = TRUE;
   	}
}
 return(bTransmit);
}


/*
 * FUNCTION: Display_UserInformationInit()
 * Initializes a Display Device User Information Page
 * Similar to the one found in an ANT supported watch that loads User Information such as weight, height etc upon first use
 * Called in main_wgtrx.c before while loop to setup before opening RX channel
 * Manually choosing values for the parameters for the User Profile
 * ARGS: N/A
 * RETURN: N/A
 */






// DISPLAY DECISION TREE FUNCTIONS//////////////////////////////////////////////////////////////////////////////////////////////////

//STEP2 FUNCTION when main page is recieved
/* FUNCTION: Branch_Main_Data()
 * RETURN: BOOL bTransmit
 * USE:	Checks to see if User Profile Exchange is supported on BOTH Display and WGT
 * 		IF YES -> ProcessScaleCapabilities.
 * 			NO ->  bTransmit = 0
 */

BOOL Branch_Main_Page()
{
	BOOL bTransmit = 1;
	
	//Check to see if User Profile Exchange supported in the weight scale and Display Device
	Display_UserProfilePage* pstDisplayProfile = Display_GetUserProfilePage();   //this is pointing to the Display's User Profile 
	WGT_DataPage1* pstDP1 = WGT_GetDataPage1();									 //This is pointing to the Main Page(Page1)recieved from WGT 
		
	//if User Profile Exchange is supported on BOTH Display and WGT	
	if(!(pstDisplayProfile->ucCapabilitiesBitField) && (pstDP1->ucCapabilitiesField & 0x02))      
	{
		if(!ProcessScaleCapabilities())								//STEP3 OF MOBILE DISPLAY DECISION TREE
		{
				bTransmit = 0;
				ggbBranchUserPage = 1;
		}
	}
	else
	{
		bTransmit = 0;										//User Profile Exchange is not supported on either/Both Display and WGT
		ggbBranchUserPage = 2;
	}
	
	return bTransmit;
}


//STEP3 FUNCTION when main page is recieved
/* FUNCTION: ProcessScaleCapabilities()
 * RETURN: BOOL bTransmit
 * USE:	Determines if scale supports ANT_FS and if User Profile has been setup in Scale
 * if(User Profile setup on Scale AND Display transmits User Profile AND both Display and Scale IDs Match)
 * 		bTransmit = TRUE 
 * else
 * 		Transmit User Profile Page from Display to Weight Scale
 * 		bTransmit = 0 
 * 	bUserProfileSent GLOBAL to keep track of if transmit from Display to WGT takes place. Also used in TransmitUserProfile()
 */
BOOL ProcessScaleCapabilities()
{
	BOOL bTransmit;
	WGT_DataPage1* pstDP1 = WGT_GetDataPage1();
	Display_UserProfilePage* pstDisplayProfile = Display_GetUserProfilePage();
	
	if((pstDP1->ucCapabilitiesField & 0x04))				//Check if ANT-FS Supported on Weight Scale
	{
		// Could request ANT-FS
		// Possible future implementation
	}
	else if((pstDP1->ucCapabilitiesField & 0x01) && bUserProfileSent && (pstDP1->usUserProfileInformation == pstDisplayProfile->usUserProfileIdentification))
	{			
			bTransmit = 1;
	}
    else
    {
    	TransmitUserProfile();
    	bTransmit = 0;
    }
return bTransmit;		
}

//STEP4 FUNCTION to TransmitUserProfile to Weight Scale 
/* FUNCTION: TransmitUserProfile()
 * RETURN: BOOL bTransmit
 * USE:	Sends display User Profile Page to Weight Scale if not sent already
 * 		if(transmit is successful)
 * 			bUserProfileSent = 1
 * 		else
 * 			bUserProfileSent = 0
 */	
BOOL TransmitUserProfile()
{
	UCHAR aucTxBuffer[8];								//tx message buffer to be transmitted to WGT
	Display_UserProfilePage* pstDisplayProfile = Display_GetUserProfilePage();
	
	bUserProfileSent = 0;
	
	// Load Transmit Buffer with Display User Profile to be Transmitted from the Display to the WGT
	aucTxBuffer[0] = 0x3A;								//Sent as Page 58
	aucTxBuffer[1] = pstDisplayProfile->usUserProfileIdentification;
	aucTxBuffer[2] = pstDisplayProfile->usUserProfileIdentification >> 8;
	aucTxBuffer[3] = pstDisplayProfile->ucCapabilitiesBitField;
	aucTxBuffer[4] = pstDisplayProfile->ucReserved;
	aucTxBuffer[5] = pstDisplayProfile->ucGender + pstDisplayProfile->ucAge;
	aucTxBuffer[6] = pstDisplayProfile->ucHeight;
	aucTxBuffer[7] = pstDisplayProfile->usUserProfileIdentification;
	
	 if(ANT_Broadcast(ucAntChannel, aucTxBuffer))						//Transmit using Display Device's ANT_RF Chip to WGT
   {
   	 IOBOARD_LED1_OUT ^= IOBOARD_LED1_BIT;								//Toggle LED everytime it transmits
     bUserProfileSent = 1;
   }
   
   return(bUserProfileSent);
}

//STEP5 FUNCTION when USER PROFILE PAGE 58 is recieved from WGT
/* FUNCTION: Branch_User_Page()
 * RETURN: N/A
 * USE:	Loads User Profile Page from WGT to Display Device User Profile Page
 */
 void Branch_User_Page()
 {
 	Display_UserProfilePage* pstDisplayProfile = Display_GetUserProfilePage();
 	WGT_UserProfilePage58* pstUserProfilePage58 = WGT_GetUserProfilePage58();
 	
  		pstDisplayProfile->usUserProfileIdentification = pstUserProfilePage58->usUserProfileIdentification;
  		pstDisplayProfile->ucGender = pstUserProfilePage58->ucGender;
  		pstDisplayProfile->ucAge = pstUserProfilePage58->ucAge;
  		pstDisplayProfile->ucHeight = pstUserProfilePage58->ucHeight;
  		pstDisplayProfile->ucDescriptiveBitField = pstUserProfilePage58->ucDescriptiveBitField;
  		pstDisplayProfile->ucCapabilitiesBitField = 0x00;
 }

//STATIONARY DISPLAY FUNCTION  
/* FUNCTION: ScalePriorityTransmit()
 * RETURN: BOOL bTransmit
 * USE:	check priority of Loaded Profile on Scale
 * 		if(Scale is Higher Priority or same Scale)
 * 			bTransmit = 1						//No Trasmit from Display to WGT in main_wgtrx.c
 * 		else
 * 			bTransmit = 0						//Transmit from Display to WGT in main_wgtrx.c
 */				
BOOL ScalePriorityTransmit()
{
	BOOL bTransmit;
	WGT_DataPage1* pstDataPage1 = WGT_GetDataPage1();
	Display_UserProfilePage* pstDisplayProfile = Display_GetUserProfilePage();
	BOOL bMobilePriorityFlag;
	BOOL bStationaryPriorityFlag;
	BOOL bWGTPriorityFlag;
	
	bMobilePriorityFlag = 0;
	bStationaryPriorityFlag = 0;
	bWGTPriorityFlag = 0;
	
	//Exisiting Profile ID on Display Device
	if(pstDisplayProfile->usUserProfileIdentification <= 0x000F)
		bWGTPriorityFlag = 1;				//Profile set on Display is a Local WeightScale Profile between  0 - 15
	else if((pstDisplayProfile->usUserProfileIdentification >= 0x0010) && (pstDisplayProfile->usUserProfileIdentification <= 0x00FF))
		bStationaryPriorityFlag = 1;		//Profile set on Display is a Stationary Profile
	else if((pstDisplayProfile->usUserProfileIdentification >= 0x0100) && (pstDisplayProfile->usUserProfileIdentification <= 0xFFFE))
		bMobilePriorityFlag = 1;				//Profile set on Display is from a Mobile Display
	else									//Profile is not set (0xFFFF)
		bTransmit = 0;						//Transmit user profile from Display to WGT to request for Page58(Valid User Profile Page)
		
	//Process when New Profile ID comming from the WGT is of Higher or Lower or Same Priority
	
	//1. New Profile ID from WGT is of Local WGT priority
	if(pstDataPage1->usUserProfileInformation <= 0x000F)
	{
		if(bMobilePriorityFlag)			//if Mobile Priority on Display
		bTransmit = 0;					//Transmit Profile from Display to WGT
		else if(bStationaryPriorityFlag) //if Stationary Priority on Display
		bTransmit = 0;					//Transmit Profile from Display to WGT
		else if(bWGTPriorityFlag)		//WGT Priority on Display
		bTransmit = 1;					//Do not tranmit from Display to WGT because same priority
		else;
	}
	
	//2. New Profile ID from WGT is of Stationary priority
	else if((pstDataPage1->usUserProfileInformation >= 0x0010) && (pstDataPage1->usUserProfileInformation <= 0x00FF))
	{
		if(bMobilePriorityFlag)			//if Mobile Priority on Display
		bTransmit = 0;					//Transmit Profile from Display to WGT
		else if(bStationaryPriorityFlag) //if Stationary Priority on Display
		bTransmit = 1;					//Do Not Transmit Profile from Display to WGT 
		else if(bWGTPriorityFlag)		//WGT Priority on Display
		bTransmit = 1;					//Do not transmit from Display to WGT because lower priority than scale priority
		else;
	}
	
	//3. New Profile ID from WGT is of Mobile priority
	else if((pstDataPage1->usUserProfileInformation >= 0x0100) && (pstDataPage1->usUserProfileInformation <= 0xFFFE))
	{
		if(bMobilePriorityFlag)			//if Mobile Priority on Display
		bTransmit = 1;					//Do Not Transmit Profile from Display to WGT
		else if(bStationaryPriorityFlag) //if Stationary Priority on Display
		bTransmit = 1;					//Do Not Transmit Profile from Display to WGT because lower priority
		else if(bWGTPriorityFlag)		//WGT Priority on Display
		bTransmit = 1;					//Do not transmit from Display to WGT because lower priority than scale priority
		else;
	}
	else;
	
	return bTransmit;
}

//STATIONARY DISPLAY FUNCTION  
/* FUNCTION: ScalePriorityRecieve()
 * RETURN: BOOL bTransmit
 * USE:	If User Profile Priority Recieved from WGT Page58 is same, DO NOT load recieved page 58 to Display user Profile
 * 		if(Scale is same Priority)
 * 			bTransmit = 0						//No Loading of Profile from WGT to Display in main_wgtrx.c
 * 		else
 * 			bTransmit = 1						//Load from WGT Page58 to Display						
 */				
BOOL ScalePriorityRecieve()
{
	BOOL bTransmit;
	WGT_UserProfilePage58* pstUserProfilePage58 = WGT_GetUserProfilePage58();
	Display_UserProfilePage* pstDisplayProfile = Display_GetUserProfilePage();
	BOOL bMobilePriorityFlag;
	BOOL bStationaryPriorityFlag;
	BOOL bWGTPriorityFlag;
	
	bMobilePriorityFlag = 0;
	bStationaryPriorityFlag = 0;
	bWGTPriorityFlag = 0;
	
	//Exisiting Profile ID on Display Device
	if(pstDisplayProfile->usUserProfileIdentification <= 0x000F)
		bWGTPriorityFlag = 1;				//Profile set on Display is a Local WeightScale Profile between  0 - 15
	else if((pstDisplayProfile->usUserProfileIdentification >= 0x0010) && (pstDisplayProfile->usUserProfileIdentification <= 0x00FF))
		bStationaryPriorityFlag = 1;		//Profile set on Display is a Stationary Profile
	else if((pstDisplayProfile->usUserProfileIdentification >= 0x0100) && (pstDisplayProfile->usUserProfileIdentification <= 0xFFFE))
		bMobilePriorityFlag = 1;				//Profile set on Display is from a Mobile Display
	else									//Profile is not set (0xFFFF)
		bTransmit = 0;						//Transmit user profile from Display to WGT to request for Page58(Valid User Profile Page)
		
	
	//1. New Profile ID from WGT is of Local WGT priority
	if(pstUserProfilePage58->usUserProfileIdentification <= 0x000F)
	{
		if(bMobilePriorityFlag)			//if Mobile Priority on Display
		bTransmit = 0;					//Do NOT Load Profile from WGT to Display
		else if(bStationaryPriorityFlag) //if Stationary Priority on Display
		bTransmit = 0;					//Do NOT Load Profile from WGT to Display
		else if(bWGTPriorityFlag)		//WGT Priority on Display
		bTransmit = 0;					//Do NOT Load Profile from WGT to Display
		else;
	}
	
	//2. New Profile ID from WGT is of Stationary priority
	else if((pstUserProfilePage58->usUserProfileIdentification >= 0x0010) && (pstUserProfilePage58->usUserProfileIdentification <= 0x00FF))
	{
		if(bMobilePriorityFlag)			//if Mobile Priority on Display
		bTransmit = 0;					//Do NOT Load Profile from WGT to Display
		else if(bStationaryPriorityFlag) //if Stationary Priority on Display
		bTransmit = 0;					//Do NOT Load Profile from WGT to Display 
		else if(bWGTPriorityFlag)		//WGT Priority on Display
		bTransmit = 1;					//Load Profile from WGT to Display
		else;
	}
	
	//3. New Profile ID from WGT is of Mobile priority
	else if((pstUserProfilePage58->usUserProfileIdentification >= 0x0100) && (pstUserProfilePage58->usUserProfileIdentification <= 0xFFFE))
	{
		if(bMobilePriorityFlag)			//if Mobile Priority on Display
		bTransmit = 0;					//Do NOT Load Profile from WGT to Display
		else if(bStationaryPriorityFlag) //if Stationary Priority on Display
		bTransmit = 1;					//Load Profile from WGT to Display
		else if(bWGTPriorityFlag)		//WGT Priority on Display
		bTransmit = 1;					//Load Profile from WGT to Display
		else;
	}
	else;
	
	return bTransmit;
}




	
