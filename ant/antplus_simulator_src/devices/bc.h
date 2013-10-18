/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/
 

#ifndef BC_H
#define BC_H

#define BC_REV				1.2					// Device Profile Rev.

#define BC_DEVICE_TYPE		((UCHAR) 122)
#define BC_TX_TYPE			((UCHAR) 1)
#define BC_MSG_PERIOD		((USHORT) 8102)		// ~4.04 Hz
#define BC_HALF_PERIOD		((USHORT) 16204)	// ~2.02 Hz
#define BC_QUARTER_PERIOD	((USHORT) 32408)	// ~1.01 Hz

#define BC_RESERVED			((UCHAR) 0xFF)

#define BC_PAGE0			((UCHAR) 0)
#define BC_PAGE1			((UCHAR) 1)
#define BC_PAGE2			((UCHAR) 2)
#define BC_PAGE3			((UCHAR) 3)

#define BC_TOGGLE_MASK		((UCHAR) 0x80)

#define BC_BACKGROUND_INTERVAL	65

typedef enum
{
	BC_STD0_PAGE = 0,
	BC_STD1_PAGE = 1,
	BC_INIT_PAGE = 2,
	BC_EXT_PAGE = 3
} BCStatePage;

#endif