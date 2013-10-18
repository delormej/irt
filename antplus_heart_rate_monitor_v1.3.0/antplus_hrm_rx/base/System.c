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

#include "Config.h"

//------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
/// System_Init
///
/// This function is used initialize the clocks and watchdog parameters
///
/////////////////////////////////////////////////////////////////////////////
void System_Init(void)
{
	// Setup the Main System Clock
   SYSTEM_DCOCTL = CALDCO_8MHZ;              // Set DCO to highest frequency (max speed)
   SYSTEM_BCSCTL1 = CALBC1_8MHZ | XT2OFF;  
   SYSTEM_BCSCTL1 &= ~SYSTEM_XTS;             // selecte low frequency mode for external clock source
   SYSTEM_BCSCTL1 &= ~(SYSTEM_DIVA0  | SYSTEM_DIVA1);          // set ACLK divider to 1
 
   SYSTEM_BCSCTL3 |= XCAP0 | XCAP1;          // Set LFXT1 cap at 12.5 pF  
 
//   SYSTEM_BCSCTL2 |= DIVS_3;                // set SMCLK divider to 8
   
   SYSTEM_IE1  &= ~SYSTEM_OFIE;              // Disable Oscillator Fault Interrupt
   SYSTEM_IFG1 &= ~SYSTEM_OFIFG;             // Clear the Oscillator Fault Interrupt
	
   SYSTEM_IE1  &= ~SYSTEM_WATCHDOG_WDTIE;    // disable watchdog interrupt
   SYSTEM_IFG1 &= ~SYSTEM_WATCHDOG_WDTIFG;   // clear the watchdog interrupt flag
}

//------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
/// System_DisableWatchdog
///
/// This function disables the watchdog
///
/////////////////////////////////////////////////////////////////////////////
void System_DisableWatchdog(void)
{
   SYSTEM_WATCHDOG_TCTL = SYSTEM_WATCHDOG_PW | SYSTEM_WATCHDOG_HOLD;
}
