/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/
 

#ifndef HRM_H
#define HRM_H

#define HRM_REV				1.13				// Device Profile Rev.

#define HRM_DEVICE_TYPE		((UCHAR) 120)
#define HRM_TX_TYPE			((UCHAR) 1)
#define HRM_MSG_PERIOD		((USHORT) 8070)		// ~4.06 Hz
#define HRM_HALF_PERIOD		((USHORT 16140)		// ~2.03 Hz (only supported on receiver)
#define HRM_QUARTER_PERIOD	((USHORT) 32280)	// ~1.02 Hz (only supported on receiver)

#define HRM_RESERVED		((UCHAR) 0xFF)
#define HRM_INVALID_BPM		((UCHAR) 0)

#define HRM_PAGE0			((UCHAR) 0)
#define HRM_PAGE1			((UCHAR) 1)
#define HRM_PAGE2			((UCHAR) 2)
#define HRM_PAGE3			((UCHAR) 3)
#define HRM_PAGE4			((UCHAR) 4)

#define HRM_TOGGLE_MASK		((UCHAR) 0x80)

#define HRM_BACKGROUND_INTERVAL		65

typedef enum
{
	HRM_STD0_PAGE = 0,
	HRM_STD1_PAGE = 1,
	HRM_INIT_PAGE = 2,
	HRM_EXT_PAGE = 3
} HRMStatePage;

#endif