/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/
 

#ifndef BS_H
#define BS_H

#define BS_REV				1.2					// Device Profile Rev.

#define BS_DEVICE_TYPE		((UCHAR) 123)
#define BS_TX_TYPE			((UCHAR) 1)
#define BS_MSG_PERIOD		((USHORT) 8118)		// ~4.04 Hz
#define	BS_HALF_PERIOD		((USHORT) 16236)	// ~2.02 Hz (supported only on receivers)
#define BS_QUARTER_PERIOD	((USHORT) 32472)	// ~1.01 Hz (supported only on receivers)

#define BS_RESERVED			((UCHAR) 0xFF)

#define BS_PAGE0			((UCHAR) 0)
#define BS_PAGE1			((UCHAR) 1)
#define BS_PAGE2			((UCHAR) 2)
#define BS_PAGE3			((UCHAR) 3)

#define BS_TOGGLE_MASK		((UCHAR) 0x80)

#define BS_BACKGROUND_INTERVAL	65

typedef enum
{
	BS_STD0_PAGE = 0,
	BS_STD1_PAGE = 1,
	BS_INIT_PAGE = 2,
	BS_EXT_PAGE = 3
} BSStatePage;

#endif