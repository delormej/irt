/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/
 

#ifndef BSC_H
#define BSC_H

#define BSC_REV				1.2					// Device Profile Rev.

#define BSC_DEVICE_TYPE		((UCHAR) 121)
#define BSC_TX_TYPE			((UCHAR) 1)
#define BSC_MSG_PERIOD		((USHORT) 8086)		// ~4.05 Hz
#define BSC_HALF_PERIOD		((USHORT) 16172)	// ~2.03 Hz
#define BSC_QUARTER_PERIOD	((USHORT) 32344)	// ~1.01 Hz

#define BSC_PAGE0			((UCHAR) 0)

typedef enum
{
	BSC_INIT = 0,
	BSC_ACTIVE = 1,
} BSCState;

#endif