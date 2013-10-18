/*
   Copyright 2012 Dynastream Innovations, Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "types.h"
#include "config.h"
#include "timer.h"

#define DELAY_10us                            ((USHORT)10)          // Approximate number of counts for 10us delay


extern volatile USHORT usLowPowerMode;

#define ALARM_CLOCK_PERIOD                    ((ULONG) 0x000F4240)   // 1MHz clock



#define MAX_ALARMS                            ((UCHAR) 4)          // Used for course timeouts and sensor simulators - make this bigger if doing several sensor sims at once.
#define DEVICE_EVENT_PERIOD                   ((ULONG) ALARM_CLOCK_PERIOD/ALARM_TIMER_PERIOD)   // Interrupt at 1024Hz 
#define DEVICE_EVENT_BLINK                    ((USHORT) ALARM_TIMER_PERIOD/5 )   // About 0.2 seconds



typedef struct
{
   USHORT usAlarmCount;
   USHORT usAlarmMax;
   UCHAR ucFlags;
   EVENT_FUNC pfCallback;
} AlarmStruct;

static AlarmStruct astAlarms[MAX_ALARMS];

static USHORT usTheTimeTimer1 = 0;
static UCHAR ucTheLEDClearsFlags;   // Bit 0 = LED0, etc ... if 1 LED should be cleared by CCR1 interrupt


//------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
/// Timer_Init
///
/// This function is used initialize the timer
///
/////////////////////////////////////////////////////////////////////////////
void Timer_Init(void)
{
   int i;

   TIMER_A_TACTL = TIMER_A_TASSEL |                      // set the clock source as SMCLK
                   TIMER_A_ID |                          // set the divider to 8
                   TACLR |                               // clear the timer
                   TIMER_A_MC;                           // continuous mode
    
    
   TIMER_A_CCR0 = DEVICE_EVENT_PERIOD-10;                // Initialize CCR0 count
   TIMER_A_CCR1 = TIMER_A_CCR0 + DEVICE_EVENT_BLINK;     // Initialize CCR1 count
   TIMER_A_TACTL |= TIMER_A_TAIE;                        // timer interrupt enabled
   TIMER_A_TACCTL0  |= 0x10;                             // enable CTL0 interrupt
   TIMER_A_TACCTL0  &= ~0x01;                            // clear CTL0 interrupt
    


   usTheTimeTimer1 = 0;
   ucTheLEDClearsFlags  = 0;
      
   
   for (i = 0; i < MAX_ALARMS; i++)
   {
      astAlarms[i].usAlarmCount = 0;
      astAlarms[i].usAlarmMax = 0;
      astAlarms[i].ucFlags = 0;
      astAlarms[i].pfCallback = NULL;
   }
   
//   P1SEL &= ~BIT4;
//   P1DIR |= BIT4;
}


//------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
/// Timer_GetTime
///
/// This function is used to ensure that a stable value is read from the timer 
///
/// \return The 16-bit timer value is returned
/////////////////////////////////////////////////////////////////////////////
USHORT Timer_GetTime(void)
{
   volatile USHORT usTime;

   // read a stable value from timer A register
   do
   {
      usTime = TIMER_A_TAR;
   }
   while (usTime != TIMER_A_TAR);

   return (usTime);                                      // return the current time
}



//------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
/// Timer_DelayTime
///
/// This function uses the timer to generate delays that are at least as long as
/// the specified duration
///
/// \param usTimein10us - The minimum amout of time to delay in 10 us units
/////////////////////////////////////////////////////////////////////////////
void Timer_DelayTime(USHORT usTimein10us)
{
   USHORT usTime;

   while (usTimein10us)                                 // wait for given time
   {
      usTime = Timer_GetTime();

      while(Timer_GetTime() - usTime < DELAY_10us);
      usTimein10us--;
   }
}


//----------------------------------------------------------------------------


////////////////////////////////////////////////////////////////////////////
// Timer_RegisterAlarm
//
////////////////////////////////////////////////////////////////////////////
UCHAR Timer_RegisterAlarm( EVENT_FUNC pfCallback_, UCHAR ucFlags_)
{
   int i;
   UCHAR ucAlarmNumber = 0;   // 0 is invalid
   
   TIMER_A_TACTL &= ~TIMER_A_TAIE;                        // timer interrupt disabled
   
   for (i = 0; i < MAX_ALARMS; i++)
   {
      if(astAlarms[i].pfCallback == NULL)
      {
         astAlarms[i].usAlarmCount = 0;
         astAlarms[i].usAlarmMax = 0;
         astAlarms[i].ucFlags = ucFlags_;
         astAlarms[i].pfCallback = pfCallback_;
         ucAlarmNumber = i+1;
         break;
      }      
   }
   
   TIMER_A_TACTL |= TIMER_A_TAIE;                        // timer interrupt enabled
   
   return(ucAlarmNumber);
}                  

////////////////////////////////////////////////////////////////////////////
// Timer_UnRegisterAlarm
//
////////////////////////////////////////////////////////////////////////////
BOOL Timer_UnRegisterAlarm( UCHAR ucAlarmNumber_ )
{  
   BOOL bSuccess = TRUE;
   TIMER_A_TACTL &= ~TIMER_A_TAIE;                        // timer interrupt disabled
 
   if(ucAlarmNumber_ <= MAX_ALARMS && ucAlarmNumber_ > 0 && astAlarms[ucAlarmNumber_-1].pfCallback != NULL)
   {
      astAlarms[ucAlarmNumber_-1].usAlarmCount = 0;
      astAlarms[ucAlarmNumber_-1].usAlarmMax = 0;
      astAlarms[ucAlarmNumber_-1].ucFlags = 0;
      astAlarms[ucAlarmNumber_-1].pfCallback = NULL;
   }
   else
   {
      bSuccess = FALSE;
   }
   
   TIMER_A_TACTL |= TIMER_A_TAIE;                        // timer interrupt enabled
   
   
   return(bSuccess);
}            

////////////////////////////////////////////////////////////////////////////
// Timer_Start
//
////////////////////////////////////////////////////////////////////////////
BOOL Timer_Start( UCHAR ucAlarmNumber_, USHORT usCount_ )
{
   if(astAlarms[ucAlarmNumber_-1].pfCallback)
   {
      astAlarms[ucAlarmNumber_-1].usAlarmCount = 0;
      astAlarms[ucAlarmNumber_-1].usAlarmMax = usCount_-1;
      astAlarms[ucAlarmNumber_-1].ucFlags |= ALARM_FLAG_ENABLED;
   }
   else
   {
      return(FALSE);
   }
   
   return(TRUE);
}                  
   
////////////////////////////////////////////////////////////////////////////
// Timer_Stop
//
////////////////////////////////////////////////////////////////////////////
void Timer_Stop( UCHAR ucAlarmNumber_ )
{
   astAlarms[ucAlarmNumber_-1].usAlarmCount = 0;
   astAlarms[ucAlarmNumber_-1].ucFlags &= ~ALARM_FLAG_ENABLED;
}            

////////////////////////////////////////////////////////////////////////////
// Timer_GetTickCount 
//
// Returns 1024 tick counter count
//
////////////////////////////////////////////////////////////////////////////
USHORT Timer_GetTickCount()
{
   return(usTheTimeTimer1);
}

//----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////
// INT_TimerA1
//
// Interrupt 1 for timer A 
//
// \return: N/A. 
////////////////////////////////////////////////////////////////////////////
#pragma vector=TIMERA1_VECTOR // TAIV Interrupt Vector for TIMER A
__interrupt void INT_TimerA1(void)
{
   USHORT usVector = TIMER_A_TAIV;
   
   if(usVector & 0x0002)
   {
      if(ucTheLEDClearsFlags & ALARM_FLAG_BLINK0)
            IOBOARD_LED0_OUT |= IOBOARD_LED0_BIT;                // Turn OFF LED 0
//      if(ucTheLEDClearsFlags & ALARM_FLAG_BLINK1)
//            IOBOARD_LED1_OUT |= IOBOARD_LED1_BIT;                // Turn OFF LED 1
//      if(ucTheLEDClearsFlags & ALARM_FLAG_BLINK2)
//            IOBOARD_LED2_OUT |= IOBOARD_LED2_BIT;                // Turn OFF LED 2
//      if(ucTheLEDClearsFlags & ALARM_FLAG_BLINK3)
//            IOBOARD_LED3_OUT |= IOBOARD_LED3_BIT;                // Turn OFF LED 3
   
      ucTheLEDClearsFlags = 0;
   }
   
   
   return;
}

//----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////
// INT_TimerB1
//
// Interrupt 0 for timer A (CCR0)  
//
// \return: N/A. 
////////////////////////////////////////////////////////////////////////////
#pragma vector=TIMERA0_VECTOR // TCCR0 Interrupt Vector for TIMER A
__interrupt void INT_TimerA0(void) 
{
   int i;
   usLowPowerMode = 0;                                   // Prevent the Main loop from going to sleep if it was just about too (prevents a race condition)
         
   usTheTimeTimer1++;                                    // Update time (resolution is 1/1024)



   P1OUT ^= BIT4;


#if 1
   // Check if any alarms need servicing ...
   for (i = 0; i < MAX_ALARMS; i++)
   {
   
      if( (astAlarms[i].ucFlags & ALARM_FLAG_ENABLED) && 
          (astAlarms[i].usAlarmCount++ >= astAlarms[i].usAlarmMax ))
      {
         astAlarms[i].pfCallback(usTheTimeTimer1, i+1);
         astAlarms[i].usAlarmCount = 0;
         
         if( astAlarms[i].ucFlags & ALARM_FLAG_ONESHOT)
            astAlarms[i].ucFlags &= ~ALARM_FLAG_ENABLED;
              
         if( astAlarms[i].ucFlags & (ALARM_FLAG_BLINK0 | ALARM_FLAG_BLINK1 | ALARM_FLAG_BLINK2 | ALARM_FLAG_BLINK3))
         {
            TIMER_A_CCR1 = TIMER_A_CCR0 + DEVICE_EVENT_BLINK;     // Update counter for blink off 

            ucTheLEDClearsFlags |= astAlarms[i].ucFlags & 0xF0;   
            
            if(astAlarms[i].ucFlags & ALARM_FLAG_BLINK0)
               IOBOARD_LED0_OUT &= ~IOBOARD_LED0_BIT;                // Turn OFF LED 0
//            if(astAlarms[i].ucFlags & ALARM_FLAG_BLINK1)
//                  IOBOARD_LED1_OUT &= ~IOBOARD_LED1_BIT;                // Turn OFF LED 1
//            if(astAlarms[i].ucFlags & ALARM_FLAG_BLINK2)
//                  IOBOARD_LED2_OUT &= ~IOBOARD_LED2_BIT;                // Turn OFF LED 2
//            if(astAlarms[i].ucFlags & ALARM_FLAG_BLINK3)
//                  IOBOARD_LED3_OUT &= ~IOBOARD_LED3_BIT;                // Turn OFF LED 3
         }
      }
   }      
#endif
   TIMER_A_CCR0 = TIMER_A_CCR0 + DEVICE_EVENT_PERIOD;          // Update device counter for next event epoch
   
   __bic_SR_register_on_exit(CPUOFF | OSCOFF | SCG0 | SCG1); // Cause MCU to wake up when interrupt exits
}

















