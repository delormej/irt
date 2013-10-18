/*
This software is subject to the license described in the License.txt file
included with this software distribution. You may not use this file except in compliance
with this license.

Copyright (c) Dynastream Innovations Inc. 2013
All rights reserved.
 */



#ifndef MOBILE_DISPLAY											//STATIONARY_DISPLAY or MOBILE_DISPLAY
#define MOBILE_DISPLAY
#ifndef NO_USER_ID_NO_PROFILE_EXCHANGE
#define NO_USER_ID_NO_PROFILE_EXCHANGE

#include "types.h"
#include "antinterface.h"
#include "antdefines.h"
#include "timer.h"
#include "printf.h"
#include "common_pages.h"
#include "ioboard.h"
#include "wgt_rx.h"

#define ANT_CHANNEL_WGTRX                          ((UCHAR) 0)        // Default ANT Channel


extern volatile USHORT usLowPowerMode;                                         // low power mode control


static const UCHAR aucNetworkKey[] = ANTPLUS_NETWORK_KEY;

void ProcessANTWGTRxEvents(ANTPLUS_EVENT_RETURN* pstEventProcess_);
static void ProcessAntEvents(UCHAR* pucEventBuffer_);
extern BOOL ggbBranchUserPage;

#ifdef NO_USER_ID_NO_PROFILE_EXCHANGE
BOOL Display_UserInformationInit()
{
   //Create a Display User Profile(for ex: one in the FR60 watch to set up personal information such as Age, Height, Weight etc))
   Display_UserProfilePage* pstDisplayUserProfile = Display_GetUserProfilePage();
   BOOL bTransmit = 1;

   pstDisplayUserProfile->usUserProfileIdentification = 0xFFFF;   //unique user id for device
   //0xFFFF for No Profile Set Test
   //0x0000 - 0x000F for Profiles generated on WGT(LOWEST PRIORITY)
   //0x0010 - 0x00ff for Profiles generated on Stationary Display(MID PRIORITY)
   //0x0100 - 0xFFFE for Profiles generated on Mobile Display(HIGHEST PRIORITY))
   pstDisplayUserProfile->ucCapabilitiesBitField = 0x80;		   //display has no user profile exchange capabilities
   pstDisplayUserProfile->ucReserved = 0xFF;				       //Not used
   pstDisplayUserProfile->ucGender = 0x80;					       //Set as male
   pstDisplayUserProfile->ucAge = 0x14;							//20
   pstDisplayUserProfile->ucHeight = 0xB4;							//180cm
   pstDisplayUserProfile->ucDescriptiveBitField = 0x84;			//Activity Level 4 and Lifetime Athlete

   if(pstDisplayUserProfile->usUserProfileIdentification == 0xFFFF)
   {
      pstDisplayUserProfile->ucGender = 0x00;
      pstDisplayUserProfile->ucAge = 0x00;
      pstDisplayUserProfile->ucHeight = 0x00;
      bTransmit = 0;
   }

   return bTransmit;
}
#endif


#ifdef NO_USER_ID_PROFILE_EXCHANGE
BOOL Display_UserInformationInit()
{
   //Create a Display User Profile(for ex: one in the FR60 watch to set up personal information such as Age, Height, Weight etc))
   Display_UserProfilePage* pstDisplayUserProfile = Display_GetUserProfilePage();
   BOOL bTransmit = 1;

   pstDisplayUserProfile->usUserProfileIdentification = 0xFFFF;   //unique user id for device
   //0xFFFF for No Profile Set Test
   //0x0000 - 0x000F for Profiles generated on WGT(LOWEST PRIORITY)
   //0x0010 - 0x00ff for Profiles generated on Stationary Display(MID PRIORITY)
   //0x0100 - 0xFFFE for Profiles generated on Mobile Display(HIGHEST PRIORITY))
   pstDisplayUserProfile->ucCapabilitiesBitField = 0x00;		   //display has  user profile exchange capabilities
   pstDisplayUserProfile->ucReserved = 0xFF;				       //Not used
   pstDisplayUserProfile->ucGender = 0x80;					       //Set as male
   pstDisplayUserProfile->ucAge = 0x14;							//20
   pstDisplayUserProfile->ucHeight = 0xB4;							//180cm
   pstDisplayUserProfile->ucDescriptiveBitField = 0x84;			//Activity Level 4 and Lifetime Athlete

   if(pstDisplayUserProfile->usUserProfileIdentification == 0xFFFF)
   {
      pstDisplayUserProfile->ucGender = 0x00;
      pstDisplayUserProfile->ucAge = 0x00;
      pstDisplayUserProfile->ucHeight = 0x00;
      bTransmit = 0;
   }

   return bTransmit;
}
#endif

#ifdef MOBILE_USER_ID_PROFILE_EXCHANGE
BOOL Display_UserInformationInit()
{
   //Create a Display User Profile(for ex: one in the FR60 watch to set up personal information such as Age, Height, Weight etc))
   Display_UserProfilePage* pstDisplayUserProfile = Display_GetUserProfilePage();
   BOOL bTransmit = 1;

   pstDisplayUserProfile->usUserProfileIdentification = 0x1234;   //unique user id for device
   //0xFFFF for No Profile Set Test
   //0x0000 - 0x000F for Profiles generated on WGT(LOWEST PRIORITY)
   //0x0010 - 0x00ff for Profiles generated on Stationary Display(MID PRIORITY)
   //0x0100 - 0xFFFE for Profiles generated on Mobile Display(HIGHEST PRIORITY))
   pstDisplayUserProfile->ucCapabilitiesBitField = 0x00;		   //display has  user profile exchange capabilities
   pstDisplayUserProfile->ucReserved = 0xFF;				       //Not used
   pstDisplayUserProfile->ucGender = 0x80;					       //Set as male
   pstDisplayUserProfile->ucAge = 0x14;							//20
   pstDisplayUserProfile->ucHeight = 0xB4;							//180cm
   pstDisplayUserProfile->ucDescriptiveBitField = 0x84;			//Activity Level 4 and Lifetime Athlete

   if(pstDisplayUserProfile->usUserProfileIdentification == 0xFFFF)
   {
      pstDisplayUserProfile->ucGender = 0x00;
      pstDisplayUserProfile->ucAge = 0x00;
      pstDisplayUserProfile->ucHeight = 0x00;
      bTransmit = 0;
   }

   return bTransmit;
}
#endif

#ifdef STATIONARY_USER_ID_PROFILE_EXCHANGE
BOOL Display_UserInformationInit()
{
   //Create a Display User Profile(for ex: one in the FR60 watch to set up personal information such as Age, Height, Weight etc))
   Display_UserProfilePage* pstDisplayUserProfile = Display_GetUserProfilePage();
   BOOL bTransmit = 1;

   pstDisplayUserProfile->usUserProfileIdentification = 0x0010;   //unique user id for device
   //0xFFFF for No Profile Set Test
   //0x0000 - 0x000F for Profiles generated on WGT(LOWEST PRIORITY)
   //0x0010 - 0x00ff for Profiles generated on Stationary Display(MID PRIORITY)
   //0x0100 - 0xFFFE for Profiles generated on Mobile Display(HIGHEST PRIORITY))
   pstDisplayUserProfile->ucCapabilitiesBitField = 0x00;		   //display has  user profile exchange capabilities
   pstDisplayUserProfile->ucReserved = 0xFF;				       //Not used
   pstDisplayUserProfile->ucGender = 0x80;					       //Set as male
   pstDisplayUserProfile->ucAge = 0x14;							//20
   pstDisplayUserProfile->ucHeight = 0xB4;							//180cm
   pstDisplayUserProfile->ucDescriptiveBitField = 0x84;			//Activity Level 4 and Lifetime Athlete

   if(pstDisplayUserProfile->usUserProfileIdentification == 0xFFFF)
   {
      pstDisplayUserProfile->ucGender = 0x00;
      pstDisplayUserProfile->ucAge = 0x00;
      pstDisplayUserProfile->ucHeight = 0x00;
      bTransmit = 0;
   }

   return bTransmit;
}
#endif

#ifdef WGT_USER_ID_PROFILE_EXCHANGE
BOOL Display_UserInformationInit()
{
   //Create a Display User Profile(for ex: one in the FR60 watch to set up personal information such as Age, Height, Weight etc))
   Display_UserProfilePage* pstDisplayUserProfile = Display_GetUserProfilePage();
   BOOL bTransmit = 1;

   pstDisplayUserProfile->usUserProfileIdentification = 0x000A;   //unique user id for device
   //0xFFFF for No Profile Set Test
   //0x0000 - 0x000F for Profiles generated on WGT(LOWEST PRIORITY)
   //0x0010 - 0x00ff for Profiles generated on Stationary Display(MID PRIORITY)
   //0x0100 - 0xFFFE for Profiles generated on Mobile Display(HIGHEST PRIORITY))
   pstDisplayUserProfile->ucCapabilitiesBitField = 0x00;		   //display has  user profile exchange capabilities
   pstDisplayUserProfile->ucReserved = 0xFF;				       //Not used
   pstDisplayUserProfile->ucGender = 0x80;					       //Set as male
   pstDisplayUserProfile->ucAge = 0x14;							//20
   pstDisplayUserProfile->ucHeight = 0xB4;							//180cm
   pstDisplayUserProfile->ucDescriptiveBitField = 0x84;			//Activity Level 4 and Lifetime Athlete

   if(pstDisplayUserProfile->usUserProfileIdentification == 0xFFFF)
   {
      pstDisplayUserProfile->ucGender = 0x00;
      pstDisplayUserProfile->ucAge = 0x00;
      pstDisplayUserProfile->ucHeight = 0x00;
      bTransmit = 0;
   }

   return bTransmit;
}
#endif

/* Main function called by main.c
 * contains the main while loop after ant module reset to enable interrupts and finish initialization
 * Calls:
 * BOOL WGTRX_CHANNELEVENT() in wgt_rx.c to load event struct
 * ProcessANTWGTRXEvents() to Process and Display Page Events from struct loaded by BOOL WGTRX_CHANNELEVENT() 
 * ProcessANTEvents()	to process ANT Channel Events
 * 
//----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////
// main
//
// main function  
//
// Configures device simulator and WGT TX channel.
//
// \return: This function does not return. 
////////////////////////////////////////////////////////////////////////////
 * */ 
void main_wgtrx()
{
   UCHAR* pucRxBuffer;						//used to point to serial message buffer from ANT
   ANTPLUS_EVENT_RETURN stEventStruct;		//Struct loaded by WGTRX_CHANNELEVENT() and used by PROCESSANTWGTEVENTS()
   Display_UserProfilePage* pstDisplayUserProfile = Display_GetUserProfilePage();
   ANT_Reset();							//Resets the ant module to initialize while loop



   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   //STEP 1 DECISION TREE
   //Create a Display User Profile (for ex: one in the FR60 watch to set up personal information such as Age, Height, Weight etc))



   //Printing for debugging purposes
   printf("DISPLAY DEVICE User Information Page \n");					//Display unique User Profile Id
   if(!Display_UserInformationInit())									//If User Profile Created
      printf("USER PROFILE NOT SET UP \n");
   else
   {
      printf("User Profile Id: %d \n", pstDisplayUserProfile->usUserProfileIdentification);

      if(!(pstDisplayUserProfile->ucCapabilitiesBitField & 0x80))
         printf("DISPLAY capable of user profile exchange \n");
      else
         printf("DISPLAY not capable of user profile exchange \n");

      if(pstDisplayUserProfile->ucGender)
         printf("Gender: Male \n");
      else
         printf("Gender: Female \n");

      printf("Age: %d \n", pstDisplayUserProfile->ucAge);
      printf("Height: %d \n", pstDisplayUserProfile->ucHeight);

      if(pstDisplayUserProfile->ucDescriptiveBitField & 0x80)
         printf("ATHLETE LEVEL: Lifetime  \n");
      else
         printf("ATHLETE LEVEL: Standard \n");

      switch(pstDisplayUserProfile->ucDescriptiveBitField & 0x07)
      {
         case 0x00:
         {
            printf("(0)ACTIVITY: Sedentary, No Exercise \n\n");
            break;
         }
         case 0x01:
         {
            printf("(1)ACTIVITY: Light Occasional Exercise, Less than 40mins. \n\n");
            break;
         }
         case 0x02:
         {
            printf("(2)ACTIVITY: Light Occasional Exercise, Less than 1hr. Once Every 2 weeks \n\n");
            break;
         }
         case 0x03:
         {
            printf("(3)ACTIVITY: Light Occasional Exercise, Less than 1hr. Once a week. \n\n");
            break;
         }
         case 0x04:
         {
            printf("(4)ACTIVITY: Regular Exercise. 1 to 5 hr per week  \n\n");
            break;
         }
         case 0x05:
         {
            printf("(5)ACTIVITY: Regular Exercise. 5 to 9 hr per week  \n\n");
            break;
         }
         case 0x06:
         {
            printf("(6)ACTIVITY: Regular Exercise. More than 9hr per week  \n\n");
            break;
         }
         case 0x07:
         {
            printf("(7)Reserved/Invalid \n\n");
            break;
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   //END OF MOBILE DISPLAY STEP1







   //Main While Loop after creation of Display User Profile
   while(1)
   {
      usLowPowerMode = 0;					//Turn off the low power mode so that the processor is awake for processing

      pucRxBuffer = ANTInterface_Transaction();	//pointing to ANT Message buffer if it exists in Serial Interface

      if(pucRxBuffer)						//check to see if buffer exists
      {
         if(WGTRX_ChannelEvent(pucRxBuffer, &stEventStruct))	//loads stEventReturn with Event type
            usLowPowerMode = 0;

         ProcessANTWGTRxEvents(&stEventStruct);					//uses struct EventType to process Data or Display

         ProcessAntEvents(pucRxBuffer);							//Process ANT channel events
      }

      _BIS_SR(usLowPowerMode);									//go to sleep
   }
}


////////////////////////////////////////////////////////////////////////////
// ProcessANTHRMRXEvents
//
// WGT Reciever event processor  
//
// Processes events recieved from WGT module.
//
// \return: N/A 
///////////////////////////////////////////////////////////////////////////
void ProcessANTWGTRxEvents(ANTPLUS_EVENT_RETURN* pstEventProcess_)
{
   switch(pstEventProcess_-> eEvent)						//Accessing enum of Struct ANTPLUS_EVENT_RETURN
   {
      case ANTPLUS_EVENT_CHANNEL_ID:						//no data page recieved, only device info loaded into Struct ANTPLUS_EVENT_RETURN
      {
         printf("Device Number is %d \n", pstEventProcess_-> usParam1);
         printf("Transmission type is %d \n \n", pstEventProcess_-> usParam2);
         break;
      }
      case ANTPLUS_EVENT_PAGE:							//data page event
      {
         IOBOARD_LED3_OUT &= ~IOBOARD_LED3_BIT;

         if(pstEventProcess_-> usParam1 <= 0x82)
            printf("Weight Scale Page: %d \n", pstEventProcess_-> usParam1);

         switch(pstEventProcess_ -> usParam1)
         {
            case WEIGHT_PAGE_BODY_WEIGHT:								//Page1
            {
               Display_UserProfilePage* pstDUProfile = Display_GetUserProfilePage(); //Pointing to Display's User Profile Page
               WGT_DataPage1* pstDataPage1 = WGT_GetDataPage1();

               printf("Main Page: Body Weight \n");

               /////////////////////////////////////MOBILE DISPLAY MAIN PAGE//////////////////////////////////////////////////////////////////////////////////
               ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef MOBILE_DISPLAY
               if(Branch_Main_Page())					//STEP2 of DECISION TREE(IF MAIN PAGE RECIEVED run this funciton)
               {
                  printf("User Profile Exchange Supported \n");
                  printf("Display UserProfileID: %d \n", pstDUProfile->usUserProfileIdentification);
                  if(pstDUProfile->ucGender)
                     printf("DisplayGender: Male \n");
                  else if(pstDUProfile->ucGender)
                     printf("DisplayGender: Female \n");
                  else
                     printf("DisplayGender: Invalid Data \n");

                  printf("Display User Age: %d years \n", pstDUProfile->ucAge);
                  printf("Display User Height: %d cm \n\n",pstDUProfile->ucHeight);
                  if((pstDataPage1->usWeightValue) == 0xFFFE)
                     printf("Measured Weight: COMPUTING \n\n");
                  else if((pstDataPage1->usWeightValue) == 0xFFFF)
                     printf("Measured Weight: INVALID \n\n" );
                  else
                     printf("Measured Weight: %d kg \n\n", ((pstDataPage1->usWeightValue) /100));
                  break;
               }
               else
               {
                  if((pstDataPage1->usWeightValue) == 0xFFFE)
                     printf("Measured Weight: COMPUTING \n\n");
                  else if((pstDataPage1->usWeightValue) == 0xFFFF)
                     printf("Measured Weight: INVALID \n\n" );
                  else
                     printf("Measured Weight: %d kg \n\n", ((pstDataPage1->usWeightValue) /100));
               }

               /////////////////////////////////////STATIONARY DISPLAY MAIN PAGE////////////////////////////////////////////////////////////////////////////////
               ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#elif defined (STATIONARY_DISPLAY)				//FOR STATIONARY DISPLAY
               printf("Display UserProfileID: %d \n", pstDUProfile->usUserProfileIdentification);
               if(pstDUProfile->ucGender)						// 1. Display ALL Defined Data
                  printf("Display Gender: Male \n");
               else if(pstDUProfile->ucGender)
                  printf("Display Gender: Female \n");
               else
                  printf("Display Gender: Invalid Data \n");

               printf("Display User Age: %d years \n", pstDUProfile->ucAge);
               printf("Display User Height: %d cm \n",pstDUProfile->ucHeight);
               if((pstDataPage1->usWeightValue) == 0xFFFE)
                  printf("Measured Weight: COMPUTING \n\n");
               else if((pstDataPage1->usWeightValue) == 0xFFFF)
                  printf("Measured Weight: INVALID \n\n" );
               else
                  printf("Measured Weight: %d kg \n\n", ((pstDataPage1->usWeightValue) /100));

               if(!(pstDUProfile->ucCapabilitiesBitField) && (pstDataPage1->ucCapabilitiesField & 0x02))	//2. Check to see if User Profile Exchange Supported
               {
                  printf("User Profile Exchange Supported \n\n");
                  if(!(pstDataPage1->ucCapabilitiesField & 0x01))	//3. if User Profile NOT set on Scale, transmit user Profile
                     TransmitUserProfile();
                  else if(!ScalePriorityTransmit()) 						//else, if Scale User ProfilePriority  is lower than Stationary Display Profile Priority
                     TransmitUserProfile();
               }
#endif
               ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
               break;
            }

            case WEIGHT_PAGE_BODY_COMPOSITION:							//Page2
            {
               WGT_DataPage2* pstDataPage2 = WGT_GetDataPage2();
               printf("Optional Page:Body Composition \n");
               printf("Measured Hydration: %d percent \n", (pstDataPage2->usHydration /100));
               printf("Measured BodyFat: %d percent \n\n", (pstDataPage2->usBodyFat /100));
               break;
            }
            case WEIGHT_PAGE_METABOLIC_INFO:							//Page3
            {
               WGT_DataPage3* pstDataPage3 = WGT_GetDataPage3();
               printf("Optional Page: Body Metabolism \n");
               printf("Active Metabolic Rate: %d KCal \n", (pstDataPage3->usActiveRate /4));
               printf("Basal Metabolic Rate: %d  KCal \n\n", (pstDataPage3->usBasalRate /4));
               break;
            }
            case WEIGHT_PAGE_BODY_MASS:									//Page4
            {
               WGT_DataPage4* pstDataPage4 = WGT_GetDataPage4();
               printf("Optional Page: Body Mass \n");
               printf("Muscle Mass: %d kg \n", (pstDataPage4->usMuscleMass /100));
               printf("Bone Mass: %d kg \n\n", (pstDataPage4->ucBoneMass /10));
               break;
            }
            case WEIGHT_PAGE_USER_PROFILE:								//Page58
            {
               Display_UserProfilePage* pstDUProfile = Display_GetUserProfilePage();
#ifdef STATIONARY_DISPLAY
               WGT_UserProfilePage58* pstUserProfilePage58 = WGT_GetUserProfilePage58();
#endif

               if(pstDUProfile->usUserProfileIdentification == 0xFFFF)			//Run Step5 only IF no DISPLAY DEVICE User Profile Page Set
               {
                  Branch_User_Page();						//STEP5 of Decision Tree


                  printf("User Profile from WeightScale loaded to Display Device User Profile \n");
                  printf("DISPLAY DEVICE User Information Page \n");					//Display unique User Profile Id
                  printf("User Profile Id: %d \n", pstDUProfile->usUserProfileIdentification);

                  if(!(pstDUProfile->ucCapabilitiesBitField & 0x80))
                     printf("DISPLAY capable of user profile exchange \n");
                  else
                     printf("DISPLAY not capable of user profile exchange \n");

                  if(pstDUProfile->ucGender)
                     printf("Gender: Male \n");
                  else
                     printf("Gender: Female \n");

                  printf("Age: %d \n", pstDUProfile->ucAge);
                  printf("Height: %d \n", pstDUProfile->ucHeight);

                  if(pstDUProfile->ucDescriptiveBitField & 0x80)
                     printf("ATHLETE LEVEL: Lifetime  \n");
                  else
                     printf("ATHLETE LEVEL: Standard \n");

                  switch(pstDUProfile->ucDescriptiveBitField & 0x07)
                  {
                     case 0x00:
                     {
                        printf("(0)ACTIVITY: Sedentary, No Exercise \n\n");
                        break;
                     }
                     case 0x01:
                     {
                        printf("(1)ACTIVITY: Light Occasional Exercise, Less than 40mins. \n\n");
                        break;
                     }
                     case 0x02:
                     {
                        printf("(2)ACTIVITY: Light Occasional Exercise, Less than 1hr. Once Every 2 weeks \n\n");
                        break;
                     }
                     case 0x03:
                     {
                        printf("(3)ACTIVITY: Light Occasional Exercise, Less than 1hr. Once a week. \n\n");
                        break;
                     }
                     case 0x04:
                     {
                        printf("(4)ACTIVITY: Regular Exercise. 1 to 5 hr per week  \n\n");
                        break;
                     }
                     case 0x05:
                     {
                        printf("(5)ACTIVITY: Regular Exercise. 5 to 9 hr per week  \n\n");
                        break;
                     }
                     case 0x06:
                     {
                        printf("(6)ACTIVITY: Regular Exercise. More than 9hr per week  \n\n");
                        break;
                     }
                     case 0x07:
                     {
                        printf("(7)Reserved/Invalid \n\n");
                        break;
                     }
                  }
               }
#ifdef STATIONARY_DISPLAY	//if user page recieved from WGT is higher priority. Load User Profile to Stat. Display
               else if(ScalePriorityRecieve())
               {
                  Branch_User_Page();				//Load User Info from WGT to Stationary Display

                  printf("(ELSE IF STATEMENT RUN)User Profile from WeightScale loaded to Display Device User Profile \n");
                  printf("DISPLAY DEVICE User Information Page \n");					//Display unique User Profile Id
                  printf("User Profile Id: %d \n", pstDUProfile->usUserProfileIdentification);

                  if(!(pstDUProfile->ucCapabilitiesBitField & 0x80))
                     printf("DISPLAY capable of user profile exchange \n");
                  else
                     printf("DISPLAY not capable of user profile exchange \n");

                  if(pstDUProfile->ucGender)
                     printf("Gender: Male \n");
                  else
                     printf("Gender: Female \n");

                  printf("Age: %d \n", pstDUProfile->ucAge);
                  printf("Height: %d \n", pstDUProfile->ucHeight);

                  if(pstDUProfile->ucDescriptiveBitField & 0x80)
                     printf("ATHLETE LEVEL: Lifetime  \n");
                  else
                     printf("ATHLETE LEVEL: Standard \n");

                  switch(pstDUProfile->ucDescriptiveBitField & 0x07)
                  {
                     case 0x00:
                     {
                        printf("(0)ACTIVITY: Sedentary, No Exercise \n\n");
                        break;
                     }
                     case 0x01:
                     {
                        printf("(1)ACTIVITY: Light Occasional Exercise, Less than 40mins. \n\n");
                        break;
                     }
                     case 0x02:
                     {
                        printf("(2)ACTIVITY: Light Occasional Exercise, Less than 1hr. Once Every 2 weeks \n\n");
                        break;
                     }
                     case 0x03:
                     {
                        printf("(3)ACTIVITY: Light Occasional Exercise, Less than 1hr. Once a week. \n\n");
                        break;
                     }
                     case 0x04:
                     {
                        printf("(4)ACTIVITY: Regular Exercise. 1 to 5 hr per week  \n\n");
                        break;
                     }
                     case 0x05:
                     {
                        printf("(5)ACTIVITY: Regular Exercise. 5 to 9 hr per week  \n\n");
                        break;
                     }
                     case 0x06:
                     {
                        printf("(6)ACTIVITY: Regular Exercise. More than 9hr per week  \n\n");
                        break;
                     }
                     case 0x07:
                     {
                        printf("(7)Reserved/Invalid \n\n");
                        break;
                     }
                  }
               }
               else
                  printf("New User Profile Ignored due to Same or Lower Priority\n\n");
#endif

               break;
            }

            case WEIGHT_COMMON_PAGE80:								//Common Page80
            {
               CommonPage80_Data* pstCommonDataPage80 = WGT_GetCommonDataPage80();
               printf("Common Page: Manufacturer Identification \n");
               printf("HW Revision: %d \n", pstCommonDataPage80->ucHwVersion);
               printf("Manufacturers ID: %u \n", pstCommonDataPage80->usManId);
               printf("Model Number: %u \n\n",pstCommonDataPage80->usModelNumber);
               break;
            }
            case WEIGHT_COMMON_PAGE81:								//Common Page81
            {
               CommonPage81_Data* pstCommonDataPage81 = WGT_GetCommonDataPage81();
               printf("Common Page: Product Identification \n");
               printf("SW Revision: %d \n", pstCommonDataPage81->ucSwVersion);
               printf("Serial Number: %u \n\n", pstCommonDataPage81->ulSerialNumber);
               break;
            }
            case WEIGHT_COMMON_PAGE82:								//Common Page82
            {
               CommonPage82_Data* pstCommonDataPage82 = WGT_GetCommonDataPage82();

               //Battery Status
               switch((pstCommonDataPage82->ucDescriptiveField & 0x70) >> 4)
               {
                  case 0x00:
                  {
                     printf("Reserved \n");
                     break;
                  }
                  case 0x01:
                  {
                     printf("New \n");
                     break;
                  }
                  case 0x02:
                  {
                     printf("Good \n");
                     break;
                  }
                  case 0x03:
                  {
                     printf("OK \n");
                     break;
                  }
                  case 0x04:
                  {
                     printf("Low \n");
                     break;
                  }
                  case 0x05:
                  {
                     printf("Critical \n");
                     break;
                  }
                  case 0x06:
                  {
                     printf("Reserved \n");
                     break;
                  }
                  case 0x07:
                  {
                     printf("Invalid \n");
                     break;
                  }
               }

               //Battery Voltage
               printf("Battery Voltage: %d V", (pstCommonDataPage82->ucBattVoltage & 0x0F) + (pstCommonDataPage82->ucBattVoltage256 / 256));

               //Cumulative Operating time based on the resolution
               if(((pstCommonDataPage82->ucDescriptiveField & 0x80) >> 7) == 0x00)   //mask to get res bit and move to LSB from MSB
                  printf("Cumulative Operating Time: %u hours", ((pstCommonDataPage82->ulCumOperatingTime * 16)/3600));
               else
                  printf("Cumulative Operating Time: %u hours", ((pstCommonDataPage82->ulCumOperatingTime * 2)/3600));
            }
            default:
               printf("Unknown Page \n\n");
         }
         IOBOARD_LED3_OUT |= IOBOARD_LED3_BIT;				//Turn off LED. This way LED should toggle each time new Page Recieved
         break;
      }

      case ANTPLUS_EVENT_UNKNOWN_PAGE:  // Decode unknown page manually
      case ANTPLUS_EVENT_NONE:
      default:
      {
         break;
      }
   }
}				


/* Function to process ANT Channel Events
 * Happens if no data events on the serial bus and only Channel Events
 * Decisions based on Message ID Byte
 */
void ProcessAntEvents(UCHAR* pucEventBuffer_)
{

   if(pucEventBuffer_)
   {
      UCHAR ucANTEvent = pucEventBuffer_[BUFFER_INDEX_MESG_ID];   

      switch( ucANTEvent )
      {
         case MESG_RESPONSE_EVENT_ID:
         {
            switch(pucEventBuffer_[BUFFER_INDEX_RESPONSE_CODE])
            {
               case EVENT_RX_SEARCH_TIMEOUT:
               {
                  IOBOARD_LED0_OUT |= IOBOARD_LED0_BIT;         
                  break;
               }
               case EVENT_TX:
               {
                  break;
               }

               case RESPONSE_NO_ERROR:
               {   

                  if(pucEventBuffer_[3] == MESG_SYSTEM_RESET_ID)  //For modules that do not support the startup message (0x6F)
                  {
                     ANT_NetworkKey(ANTPLUS_NETWORK_NUMBER, aucNetworkKey);
                     printf("Reset is complete.\n");
                  }

                  else if (pucEventBuffer_[3] == MESG_OPEN_CHANNEL_ID)
                  {
                     IOBOARD_LED0_OUT &= ~IOBOARD_LED0_BIT;         
                     printf("initialization is complete.\n\n");
                  }
                  else if (pucEventBuffer_[3] == MESG_CLOSE_CHANNEL_ID)
                  {
                  }
                  else if (pucEventBuffer_[3] == MESG_NETWORK_KEY_ID)
                  {
                     //Once we get a response to the set network key
                     //command, start opening the HRM channel
                     WGTRX_Open(ANT_CHANNEL_WGTRX, 0, 0); 
                  }
                  break;
               }
            }
            break;
         }
         case MESG_START_UP:  //Supported by AP2 and later
         {
            ANT_NetworkKey(ANTPLUS_NETWORK_NUMBER, aucNetworkKey);
            printf("Reset is complete.\n");
            break;
         }
      }
   }      
}

#endif
#endif



