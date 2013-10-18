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

#ifndef __SERIAL_H___
#define __SERIAL_H__

#include "types.h"

BOOL 
Serial_Init();

BOOL 
Serial_SyncReset(void);

void 
Serial_Transaction(void);

UCHAR* 
Serial_Get_Tx_Buffer();

void 
Serial_Put_Tx_Buffer(BOOL bGoToSleep_);

UCHAR* 
Serial_Get_Rx_Buffer();

void 
Serial_Put_Rx_Buffer();

UCHAR* 
Serial_Scan_Rx_Top();

UCHAR*
Serial_Read_Rx_Top();


UCHAR* 
Serial_Read_Rx_Buffer();

void 
Serial_Flush_Tx();

void 
Serial_Flush_Rx();

BOOL
Serial_Rx_Buffer_Overflow();

int 
Asynchronous_WriteByte(
   int iByte);
   
#endif
