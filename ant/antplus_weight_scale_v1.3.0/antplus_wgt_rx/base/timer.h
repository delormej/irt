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

#ifndef TIMER_H
#define TIMER_H

#include "types.h"



//////////////////////////////////////////////////////////////////////////////////
/// This delay must be at least 2.5us
/// So this must be modified for a specfic processor speed
//////////////////////////////////////////////////////////////////////////////////
#define TIMER_SRDY_PULSE_DELAY()    {__no_operation();__no_operation();__no_operation();__no_operation();\
                                     __no_operation();__no_operation();__no_operation();__no_operation();\
                                     __no_operation();__no_operation();__no_operation();__no_operation();}
									/*{asm("nop");asm("nop");asm("nop");asm("nop");\
                                     asm("nop");asm("nop");asm("nop");asm("nop");\
                                     asm("nop");asm("nop");asm("nop");asm("nop");}*/



// Flags for course timer used for command timeouts and sensor simulator event
#define ALARM_FLAG_ONESHOT                ((UCHAR) 0x01)    // Timer runs once only
#define ALARM_FLAG_CONTINUOUS             ((UCHAR) 0x02)    // Timer will run continuously
#define ALARM_FLAG_ENABLED                ((UCHAR) 0x08)    // Alarm is enabled
#define ALARM_FLAG_BLINK0                 ((UCHAR) 0x10)    // Timer will force a blink of the LED on timer event
#define ALARM_FLAG_BLINK1                 ((UCHAR) 0x20)    // Timer will force a blink of the LED on timer event
#define ALARM_FLAG_BLINK2                 ((UCHAR) 0x40)    // Timer will force a blink of the LED on timer event
#define ALARM_FLAG_BLINK3                 ((UCHAR) 0x80)    // Timer will force a blink of the LED on timer event
#define ALARM_TIMER_PERIOD                ((USHORT) 1024)   // Rate at which timer will interrupt (per second)



typedef void (*EVENT_FUNC)(USHORT usTimeStamp_, UCHAR ucAlarmNumber_);
//////////////////////////////////////////////////////////////////////////////////
/// This delay must be at least 2.5us
/// So this must be modified for a specfic processor speed
//////////////////////////////////////////////////////////////////////////////////
void
Timer_Init(void);

void
Timer_AssignTimerHandler(
   EVENT_FUNC pfTimeHandler_);

USHORT
Timer_GetTime();

void
Timer_DelayTime(
   USHORT usTimein10us);

UCHAR                                  // Alarm number (0 if alarm failed to get assigned)
Timer_RegisterAlarm(
   EVENT_FUNC pfCallback_,             // Pointer to function to call on alarm elapsed event
   UCHAR ucFlags_);                    // Alarm function flags

BOOL                                   // TRUE if alarm unregistered succesfully
Timer_UnRegisterAlarm(
   UCHAR ucAlarmNumber_);              // Alarm number

BOOL                                   // Alarm started successfully
Timer_Start(
   UCHAR ucAlarmNumber_,               // Alarm number
   USHORT ucCount_);                   // Alarm elapsed count

USHORT                                 // 1024 tick count
Timer_GetTickCount();

void
Timer_Stop(
   UCHAR ucAlarmNumber_);              // Alarm number



#endif
