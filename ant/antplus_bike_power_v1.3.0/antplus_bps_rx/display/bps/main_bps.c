/*
This software is subject to the license described in the License.txt file
included with this software distribution. You may not use this file except in compliance
with this license.

Copyright (c) Dynastream Innovations Inc. 2013
All rights reserved.
*/

#include "types.h"
#include "antinterface.h"
#include "antdefines.h"
#include "timer.h"
#include "printf.h"

#include "ioboard.h"
#include "bps_rx.h"
#include "BPS_pages.h" 


extern volatile USHORT usLowPowerMode;                // low power mode control

// other defines
#define BPS_PRECISION             ((ULONG)1000)
#define BPSRX_ANT_CHANNEL         ((UCHAR) 0)         // Default ANT Channel

static CalibrationType eCalibrationType;
static UCHAR ucCalibrationEnable;
static const UCHAR aucNetworkKey[] = ANTPLUS_NETWORK_KEY;

static void ProcessANTBPSRXEvents(ANTPLUS_EVENT_RETURN* pstEvent_);
static void ProcessAntEvents(UCHAR* pucEventBuffer_);
static void ProcessButtonState();                      // Checks for user calibration request (button press)
                                                       // - '1' cycles through calibration options, '3' sends request)

//----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////
// main
//
// main function  
//=6
// Configures device simulator and HRM TX channel.
//
// \return: This function does not return. 
////////////////////////////////////////////////////////////////////////////
void main_bpsrx()
{
   UCHAR* pucRxBuffer;
   ANTPLUS_EVENT_RETURN stBPSEventStruct;
   eCalibrationType = CALIBRATION_TYPE_MANUAL;
   ucCalibrationEnable = 0;						         // Disable Calibration request until init complete
   
   // Main loop
   // Set network key. Do not send other commands
   // until after a response is recieved to this one.
   ANT_NetworkKey(ANTPLUS_NETWORK_NUMBER, aucNetworkKey);

   // Main loop
   while(TRUE)
   {
      usLowPowerMode = 0;//LPM1_bits;                 // Initialize the low power mode -  move to processor specific file and generic function
      
      // Check the state of the buttons
      // Try to send or recieve a message to/from ANT
      // If there is a message to recieve, the message will be
      // recieved and buffered up
      // If there is a message to transmit, this function
      // will block until the message is sent.

      IOBoard_Check_Button();                         // Check the button state
      ProcessButtonState();
      
      if(CheckCalibrationTimeout(&stBPSEventStruct))  // Calibration timed out
         ProcessANTBPSRXEvents(&stBPSEventStruct);    
      
      pucRxBuffer = ANTInterface_Transaction();       // Check if any data has been recieved from serial
             
      if(pucRxBuffer)
      {
         if(BPSRX_ChannelEvent(pucRxBuffer, &stBPSEventStruct))
            usLowPowerMode = 0;
         
         ProcessANTBPSRXEvents(&stBPSEventStruct);               
         
         ProcessAntEvents(pucRxBuffer);
      }
      _BIS_SR(usLowPowerMode);                        // Go to sleep if we can  -  move to processor specific file and generic function
   } 
} 


////////////////////////////////////////////////////////////////////////////
// ProcessANTBPSRXEvents
//
// BPS Reciever event processor  
//
// Processes events recieved BPS HRM module.
//
// \return: N/A 
///////////////////////////////////////////////////////////////////////////
void ProcessANTBPSRXEvents(ANTPLUS_EVENT_RETURN* pstEvent_)
{
   switch (pstEvent_->eEvent)
   {
      case ANTPLUS_EVENT_CALIBRATION_TIMEOUT:
      {
      	ucCalibrationEnable = 1;
      	printf("Calibration timed out, enable reset\n\n");
      	break;
      }
      case ANTPLUS_EVENT_PAGE:
      {
         IOBOARD_LED3_OUT &= ~IOBOARD_LED3_BIT;       // TURN ON LED 3
         
         // Get data correspinding to the page. Only get the data you 
         // care about.
         switch(pstEvent_->usParam2)
         {
            case BPSRX_SUBEVENT_FIRSTPAGE:
            {
               printf("\n\nFirst page received - no calculations made\n\n");
               break;
            }
            case BPSRX_SUBEVENT_COASTING:
            {
               printf("\n\nCoasting conditions\n\n");
               break;
            }

            case BPSRX_SUBEVENT_ZERO_SPEED:
            {
               printf("\n\nZero Speed conditions\n\n");
               break;
            }
             case BPSRX_SUBEVENT_NOUPDATE:
            {
               printf("\n\nNo Update\n\n");
               break;
            }
            case BPSRX_SUBEVENT_UPDATE:
            {
               switch(pstEvent_->usParam1)
               {
                  case BPS_PAGE_1:
                  {
                     BPSPage1_Data* pstPage1Data = BPSRX_GetPage1();
                     printf("Page1:\n");
                           
                     switch(pstPage1Data->ucCalibrationID)
                     {
                        case PBS_CID_18:
                        {
                           printf("CID 18:\n");          // Calibration page 1, 18
                           printf("SensorConfig EN: %u\n",pstPage1Data->stBPSCalibrationData.stBPSPage1_CID18_Data.ucCID18_SensorConfiguration_EN);
                           printf("SensorConfig STAT: %u\n",pstPage1Data->stBPSCalibrationData.stBPSPage1_CID18_Data.ucCID18_SensorConfiguration_STATUS);
                           printf("Raw Torque: %d\n",pstPage1Data->stBPSCalibrationData.stBPSPage1_CID18_Data.ssCID18_RawTorqueCount);
                           printf("Offset Torque: %d\n\n",pstPage1Data->stBPSCalibrationData.stBPSPage1_CID18_Data.ssCID18_OffsetTorque);
                           // printf ("%u \n", pstPage1Data->stBPSCalibrationData.stBPSPage1_CID18_Data.ucCID18_ReserveByte7);
                           break;
                        }
                        case PBS_CID_172:
                        {
                           printf("CID 172 (0xAC):\n");  // Calibration page 1, 172
                           printf("Calibration Successful\n");
                           ucCalibrationEnable = 1;
                           printf("AutoZero Status: %u\n",pstPage1Data->stBPSCalibrationData.stBPSPage1_CID172_Data.ucCID172_AutoZeroStatus);
                           printf("Calibration Data: %u\n\n",pstPage1Data->stBPSCalibrationData.stBPSPage1_CID172_Data.usCID172_CalibrationData);
                           // printf ("%u \n", pstPage1Data->stBPSCalibrationData.stBPSPage1_CID18_Data.ucCID18_ReserveByte7);
                           break;
                        }
                        case PBS_CID_175:
                        {
                           printf("CID 175 (0xAF):\n");  // Calibration page 1, 175
                           printf("Calibration Failed\n");
                           ucCalibrationEnable = 1;
                           printf("AutoZero Status: Data: %u\n",pstPage1Data->stBPSCalibrationData.stBPSPage1_CID175_Data.ucCID175_AutoZeroStatus);
                           printf("Calibration Data: %u\n\n",pstPage1Data->stBPSCalibrationData.stBPSPage1_CID175_Data.usCID175_CalibrationData);
                           // printf ("%u \n", pstPage1Data->stBPSCalibrationData.stBPSPage1_CID18_Data.ucCID18_ReserveByte7);
                           break;
                        } 
                        case PBS_CID_16:                 // CTF defined msgs
                        {
                           UCHAR ucPage1CTF_ID = BPSRX_GetCTF_ID();
                           printf("CID 16 (x010) CTFID: %u (0x%x)\n",ucPage1CTF_ID,ucPage1CTF_ID);
                           switch(ucPage1CTF_ID)
                           {
                              case PBS_CID_16_CTFID_1:   // Calibration page 1, 16 - CTF defined ID: 1
                              {
                                 // ****** NB BIG ENDIAN ******
                                 //pstPage1Data->stBPSCalibrationData.stBPSPage1_CID16_CTFID1_Data.ucCID16_CTFID1_ReserveByte3
                                 //pstPage1Data->stBPSCalibrationData.stBPSPage1_CID16_CTFID1_Data.ucCID16_CTFID1_ReserveByte4
                                 //pstPage1Data->stBPSCalibrationData.stBPSPage1_CID16_CTFID1_Data.ucCID16_CTFID1_ReserveByte5
                                 printf("Offset Data: %u \n",pstPage1Data->stBPSCalibrationData.stBPSPage1_CID16_CTFID1_Data.usCID16_CTFID1_OffsetData);
                                 ucCalibrationEnable = 2;  //set to 1 when RX normal CTF message (ie Page 32)
                                 break;
                              }
                              case PBS_CID_16_CTFID_172: // Calibration page 1, 16 - CTF defined ID: 175
                              {
                                 printf("Acknowedged: %u  (2 - slope, 3 - serial)\n\n",pstPage1Data->stBPSCalibrationData.stBPSPage1_CID16_CTFID172_Data.ucCID16_CTFID172_CTFAckMessage);
                                 ucCalibrationEnable = 1;
                                 //pstPage1Data->stBPSCalibrationData.stBPSPage1_CID16_CTFID172_Data.ucCID16_CTFID172_ReserveByte4
                                 //pstPage1Data->stBPSCalibrationData.stBPSPage1_CID16_CTFID172_Data.ucCID16_CTFID172_ReserveByte5
                                 //pstPage1Data->stBPSCalibrationData.stBPSPage1_CID16_CTFID172_Data.ucCID16_CTFID172_ReserveByte6
                                 //pstPage1Data->stBPSCalibrationData.stBPSPage1_CID16_CTFID172_Data.ucCID16_CTFID172_ReserveByte7
                                 break;
                              }
                              default:
                              {
                                 printf("Invalid CTFID Received\n\n");
                                 break;
                              }
                           }
                           break;
                        }	
                        default:
                        {
                           printf("\nUnknown PAGE 1 Calibration format\n\n");
                           break;
                        }     // CTF defined pages break
                     }
                     break;   // Calibration page 1 break
                  }
                  case BPS_PAGE_16:
                  {
                     BPSPage16_Data* pstPage16Data = BPSRX_GetPage16();
                     CalculatedStdPower* pstCalcStdPower = BPSRX_GetCalcStdPwr();
                     
                     // Transmitted Data
                     printf("Page16:\n");  
                     printf("Event Count: %u\n", pstPage16Data->ucPOEventCount);
                     if (pstPage16Data->ucPOInstantCadence==0xff)
                        printf("Invalid Instant Cadence\n");                                
                     else
                        printf("Instant Cadence: %u RPM\n", pstPage16Data->ucPOInstantCadence);
                     printf("Accumulated Power: %u Watts\n", pstPage16Data->usPOAccumulatedPower);
                     printf("Instant Power: %u Watts\n", pstPage16Data->usPOInstantPower);
               
                     // Calculated Data
                     printf("Average Power: %u Watts\n\n",pstCalcStdPower->ulAvgStandardPower);
                     break;
                  }
                  case BPS_PAGE_17:
                  {
                     BPSPage17_Data* pstPage17Data = BPSRX_GetPage17();
                     CalculatedStdWheelTorque* pstCalcWT = BPSRX_GetCalcWT();
                  
                     // Transmitted Data
                     printf("Page17:\n");
   			         printf("WTEventCount: %u\n", pstPage17Data->ucWTEventCount);                    
   			         printf("WTWheelTicks: %u Revolutions\n", pstPage17Data->ucWTWheelTicks);
   			         if (pstPage17Data->ucWTInstantCadence==0xff)
                        printf("Invalid Instant Cadence\n");                                
                     else
                        printf("WTInstantCadence: %u RPM\n", pstPage17Data->ucWTInstantCadence);                  
   			         printf("WTAccumulatedWheelPeriod: %u (1/2048)seconds\n", pstPage17Data->usWTAccumulatedWheelPeriod);
   			         printf("WTAccumulatedTorque: %u (1/32)Nm\n", pstPage17Data->usWTAccumulatedTorque);
      			   
         			   // Calculated Data
   			         if(!pstCalcWT->ulWTAvgSpeed)
   			         printf("WTAverage Speed: %u kph\n", pstCalcWT->ulWTAvgSpeed);
   			         printf("WTdelta distance: %u m\n", pstCalcWT->ulWTDistance);
   			         printf("WTAngular Velocity: %u Rad/s\n", pstCalcWT->ulWTAngularVelocity);
   			         printf("WTAverage Torque: %u Nm\n", pstCalcWT->ulWTAverageTorque);
   			         printf("WTAverage Power: %u Watts\n\n", pstCalcWT->ulWTAveragePower);
                     break;
                  }
                  case BPS_PAGE_18:
                  {
                     BPSPage18_Data* pstPage18Data = BPSRX_GetPage18();
                     CalculatedStdCrankTorque* pstCalcCT = BPSRX_GetCalcCT();
                     
                     // Transmitted Data
                     printf("Page18:\n");
                     printf("CTEventCount: %u\n",pstPage18Data->ucCTEventCount);                                  
   			         printf("CTCrankTicks: %u\n", pstPage18Data->ucCTCrankTicks);
   			         if (pstPage18Data->ucCTInstantCadence==0xff)
                        printf("Invalid Instant Cadence\n");                                
                     else
                        printf("CTInstantCadence: %u RPM\n", pstPage18Data->ucCTInstantCadence);                  
   			         printf("CTAccumulatedCrankPeriod: %u (1/2048)seconds\n", pstPage18Data->usCTAccumulatedCrankPeriod); 
   			         printf("CTAccumulatedTorque: %u (1/32)Nm\n", pstPage18Data->usCTAccumulatedTorque);
         			   
   			         // Calculated Data
   			         printf("Average Cadence: %u RPM\n",pstCalcCT->ulCTAverageCadence);
   			         printf("CTAngular Velocity: %u Rad/s\n", pstCalcCT->ulCTAngularVelocity);
   			         printf("CTAverage Torque: %u Nm\n", pstCalcCT->ulCTAverageTorque);
   			         printf("CTAverage Power: %u Watts\n\n", pstCalcCT->ulCTAveragePower);
                     break;
                  }
                  case BPS_PAGE_32:
                  {
                     BPSPage32_Data* pstPage32Data = BPSRX_GetPage32();
                     CalculatedStdCTF* pstCalcCTF = BPSRX_GetCalcCTF();
                     
                     if(ucCalibrationEnable ==2)
                        ucCalibrationEnable = 1;   
                     
                     // Transmitted Data
                     printf("Page32:\n");
   			         printf("ucCTFEventCount: %u\n", pstPage32Data->ucCTFEventCount);                                  
   			         printf("usCTFSlope: %u 1/10 Nm/Hz\n", pstPage32Data->usCTFSlope);
   			         printf("usCTFTimeStamp: %u 1/2000s\n", pstPage32Data->usCTFTimeStamp); 
   			         printf("usCTFTorqueTickStamp: %u\n", pstPage32Data->usCTFTorqueTickStamp);
         			   
   			         // Calculated Data
   			         printf("Average Cadence: %u RPM\n",pstCalcCTF->ulCTFAverageCadence);
   			         printf("CTF Torque Frequency: %u Hz\n",pstCalcCTF->ulCTFTorqueFrequency);
   			         printf("CTF Average Torque: %u Nm\n", pstCalcCTF->ulCTFAverageTorque);
   			         printf("CTF Average Power: %u Watts\n\n", pstCalcCTF->ulCTFAveragePower);
                     break;
                  }
                  case GLOBAL_PAGE_80:
                  {
                     CommonPage80_Data* pstPage80Data = Common_GetPage80();
      
                     printf("Common Data Page 80:\n");
                     printf("Manufacturer ID: %u\n", (USHORT)pstPage80Data->usManId);
                     printf("Model %u, ", (USHORT)pstPage80Data->usModelNumber);
                     printf("Hardware rev. %d\n\n", pstPage80Data->ucHwVersion);
                     break;
                  }
                  case GLOBAL_PAGE_81:
                  {
                     CommonPage81_Data* pstPage81Data = Common_GetPage81();
      
                     printf("Common Data Page 81:\n");
                     printf("Software version %d, ", pstPage81Data->ucSwVersion); 
                     printf("SN# 0x%04X", (USHORT)((pstPage81Data->ulSerialNumber >> 16) & MAX_USHORT));
                     printf("%04X\n\n", (USHORT)(pstPage81Data->ulSerialNumber & MAX_USHORT)); //display limited by 16-bit CPU
                     break;
                  }
                  case GLOBAL_PAGE_82:
                  {
                     CommonPage82_Data* pstPage82Data = Common_GetPage82();
                     UCHAR ucTimeRes = pstPage82Data->ucCumOperatingTimeRes ? 2 : 16; //2 or 16 second resolution

                     printf("Common Data Page 82:\n");
                     printf("Cumulative operating time: %u", (ULONG)((pstPage82Data->ulCumOperatingTime * ucTimeRes) / 3600)); // 3600s/hr
                     printf(".%03u hrs\n", (ULONG)((((pstPage82Data->ulCumOperatingTime * ucTimeRes) % 3600) * BPS_PRECISION) + 1800) /3600); // 3600s/hr  (***define bps precision)
                     if (pstPage82Data->ucBattVoltage != GBL82_COARSE_BATT_INVALID_VOLTAGE)
                     {
                        printf("Battery voltage: %u", pstPage82Data->ucBattVoltage);
                        printf("%03u V, ", (ULONG)(((pstPage82Data->ucBattVoltage256 * BPS_PRECISION) + 128) / 256));
                     }
                     else
                        printf("Invalid battery voltage, ");

                     switch (pstPage82Data->ucBattStatus)
                     {
                        case GBL82_BATT_STATUS_NEW:
                        {
                           printf("battery new.\n\n");
                           break;
                        }
                        case GBL82_BATT_STATUS_GOOD:
                        {
                           printf("battery good.\n\n");
                           break;
                        }
                        case GBL82_BATT_STATUS_OK:
                        {
                           printf("battery ok.\n\n");
                           break;
                        }
                        case GBL82_BATT_STATUS_LOW:
                        {
                           printf("battery low.\n\n");
                           break;
                        }
                        case GBL82_BATT_STATUS_CRITICAL:
                        {
                           printf("battery critical.\n\n");
                           break;
                        }
                        default:
                        {
                           printf("invalid battery status.\n\n");
                           break;
                        }
                     }
      
                     break;
                  }
                  default:
                  {
                     printf("\nUnknown PAGE format recieved\n\n");
                     break; 
                  }
               }
               IOBOARD_LED3_OUT |= IOBOARD_LED3_BIT;    // TURN OFF LED 3
               break;
            }
         }
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

void ProcessButtonState()
{
   UCHAR ucButtonStates = IOBoard_Button_Pressed();
   
   if (ucButtonStates & BUTTON3_STATE_PRESSED)  // Calibration requested
   {
      if (ucCalibrationEnable == 1)
      {
         printf("BUTTON 3:  ");
         ucCalibrationEnable = CalibrationRequest(eCalibrationType);
         // set to zero on successful Calibration Request Transmit 
         // calibration not be set to 1 until calibration success/fail received

         if (ucCalibrationEnable)
         // Calibration Request returned FAIL, request not possible, stays enabled
         {
            printf("AUTO_CAL_NOT_SUPPORTED_NO_TX\n\n");
         }
         else if (!ucCalibrationEnable)
         // Calibration request successful, and not re-enabled until calibration completes
         {
            switch(eCalibrationType)
            {
               case CALIBRATION_TYPE_MANUAL:
               {
                  printf("Manual calibration requested\n\n");
                  break;
               }
               case CALIBRATION_TYPE_AUTO:
               {
                  printf("AUTO calibration requested\n\n");
                  break;
               }                                                                  
               case CALIBRATION_TYPE_SLOPE:
               {
                  printf("SLOPE calibration requested\n\n");
                  break;
               }                                                             
               case CALIBRATION_TYPE_SERIAL:
               {
                  printf("SERIAL calibration requested\n\n");
                  break;
               }
            }
         }
      }
      else if (ucCalibrationEnable==0)
      {
         printf("Calibration Already Processing, Please wait...\nBUTTON3 pressed\n");
      }
      IOBoard_Button_Clear(BUTTON3_STATE_OFFSET);
   }

   else if(ucButtonStates & BUTTON1_STATE_PRESSED)
   {
      // Change of calibration type requested
      // Only change states is calibration enabled
      if(ucCalibrationEnable == 1)
      { 
         switch(eCalibrationType)
         {
            case CALIBRATION_TYPE_MANUAL:
            {
               eCalibrationType = CALIBRATION_TYPE_AUTO;
               printf("\nCALIBRATION_TYPE_AUTO\n\n");
               break;
            }                                
            case CALIBRATION_TYPE_AUTO:
            {
               eCalibrationType = CALIBRATION_TYPE_SLOPE;-
               printf("\nCALIBRATION_TYPE_SLOPE\n\n");
               break;
            }                                                                  
            case CALIBRATION_TYPE_SLOPE:
            {
               eCalibrationType = CALIBRATION_TYPE_SERIAL;
               printf("\nCALIBRATION_TYPE_SERIAL\n\n");
               break;
            }                                                             
            case CALIBRATION_TYPE_SERIAL:
            {
               eCalibrationType = CALIBRATION_TYPE_MANUAL;
               printf("\nCALIBRATION_TYPE_MANUAL\n\n");
               break;
            }
         }
      }
      else
      // Calibration state cannot be changed as calibration not enabled
      {
         printf("Calibration Processing, Cannot Change State. Please Wait...\nBUTTON1 pressed\n");
      }
      IOBoard_Button_Clear(BUTTON1_STATE_OFFSET);
   }
}

////////////////////////////////////////////////////////////////////////////
// ProcessAntEvents
//
// Generic ANT Event handling  
//
// \return: N/A 
///////////////////////////////////////////////////////////////////////////
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
               case EVENT_TRANSFER_TX_COMPLETED:
               {
                  printf("Calibration Page Transmitted.\n\n");
                  break;
               }
               case EVENT_TRANSFER_TX_FAILED:
               {
                  printf("Calibration Page Transmit Failed. Retrying...\n\n");
                  break;
               }
               case EVENT_RX_FAIL:
               {
               	  printf("Rx Fail...\n\n");
               	  break;
               }
               case RESPONSE_NO_ERROR:
               {   
                  if (pucEventBuffer_[3] == MESG_OPEN_CHANNEL_ID)
                  {
                     IOBOARD_LED0_OUT &= ~IOBOARD_LED0_BIT;         
                     printf("initialization is complete.\n");
                     ucCalibrationEnable = 1;
                  }
                  else if (pucEventBuffer_[3] == MESG_CLOSE_CHANNEL_ID)
                  {
                     printf("Channel Closed\n");
                  }
                  else if (pucEventBuffer_[3] == MESG_NETWORK_KEY_ID)
                  {
                     //Once we get a response to the set network key
                     //command, start opening the BPS channel

                     BPSRX_Open(BPSRX_ANT_CHANNEL,0,0); 
                  }
                  break;
               }
            }
         }
      }
   }      
}


