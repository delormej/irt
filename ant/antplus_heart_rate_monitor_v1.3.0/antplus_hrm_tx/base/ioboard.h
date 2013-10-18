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

#ifndef __IOBOARD_H__
#define __IOBOARD_H__

#include "types.h"

#define BUTTON_STATE_MASK    ((UCHAR) 0x03) // Button 1 bit mask
#define BUTTON_STATE_PRESSED ((UCHAR) 0x01) // Button has been pressed
#define BUTTON_STATE_WFC     ((UCHAR) 0x02) // Waiting for button to be released



#define BUTTON1_STATE_MASK    ((UCHAR) 0x03) // Button 1 bit mask
#define BUTTON1_STATE_OFFSET  ((UCHAR) 0x00) // Button 1 bit mask
#define BUTTON1_STATE_PRESSED ((UCHAR) 0x01) // Button has been pressed
#define BUTTON1_STATE_WFC     ((UCHAR) 0x02) // Waiting for button to be released

#define BUTTON2_STATE_MASK    ((UCHAR) 0x0C) // Button 2 bit mask
#define BUTTON2_STATE_OFFSET  ((UCHAR) 0x02) // Button 2 bit mask
#define BUTTON2_STATE_PRESSED ((UCHAR) 0x04) // Button has been pressed
#define BUTTON2_STATE_WFC     ((UCHAR) 0x08) // Waiting for button to be released

#define BUTTON3_STATE_MASK    ((UCHAR) 0x30) // Button 3 bit mask
#define BUTTON3_STATE_OFFSET  ((UCHAR) 0x04) // Button 3 bit mask
#define BUTTON3_STATE_PRESSED ((UCHAR) 0x10) // Button has been pressed
#define BUTTON3_STATE_WFC     ((UCHAR) 0x20) // Waiting for button to be released

#define BUTTON0_STATE_MASK    ((UCHAR) 0xC0) // Button 3 bit mask
#define BUTTON0_STATE_OFFSET  ((UCHAR) 0x06) // Button 3 bit mask
#define BUTTON0_STATE_PRESSED ((UCHAR) 0x40) // Button has been pressed
#define BUTTON0_STATE_WFC     ((UCHAR) 0x80) // Waiting for button to be released




typedef enum
{
   LED_0,
//   LED_1, 
//   LED_2,
   LED_3
}  LedEnum;


void IOBoard_Init();

UCHAR IOBoard_Button_Pressed();
void IOBoard_Button_Clear(UCHAR ucButtonWFC_);
void IOBoard_Check_Button();
//void IOBoard_Led_Set(LedEnum eLed_, BOOL bOn_);


#endif




























//
























