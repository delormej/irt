/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/
 

#ifndef SDM_H
#define SDM_H

#define SDM_REV					1.3					// Device Profile Rev.

#define SDM_DEVICE_TYPE			((UCHAR) 124)
#define SDM_TX_TYPE				((UCHAR) 5)
#define SDM_MSG_PERIOD			((USHORT) 8134)		// ~4.03Hz
#define SDM_HALF_PERIOD			((USHORT) 16268)	// ~2.01Hz

#define SDM_RESERVED			((UCHAR) 0xFF)
#define SDM_UNUSED				((UCHAR) 0x00)

#define SDM_PAGE1				((UCHAR) 1)
#define SDM_PAGE2				((UCHAR) 2)
#define SDM_PAGE3				((UCHAR) 3)
#define SDM_PAGE4				((UCHAR) 4)
#define SDM_PAGE5				((UCHAR) 5)
#define SDM_PAGE6				((UCHAR) 6)
#define SDM_PAGE7				((UCHAR) 7)
#define SDM_PAGE8				((UCHAR) 8)
#define SDM_PAGE9				((UCHAR) 9)
#define SDM_PAGE10				((UCHAR) 10)
#define SDM_PAGE11				((UCHAR) 11)
#define SDM_PAGE12				((UCHAR) 12)
#define SDM_PAGE13				((UCHAR) 13)
#define SDM_PAGE14				((UCHAR) 14)
#define SDM_PAGE15				((UCHAR) 15)
#define SDM_PAGE16				((UCHAR) 0x10)
#define SDM_PAGE22				((UCHAR) 0x16)
#define SDM_PAGE70				((UCHAR) 70)

#define SDM_DATA_REQUEST		((UCHAR) 0x01)

#define SDM_MAX_SPEED256		((USHORT) 0x0FFF)
//#define SDM_MAX_CADENCE16		((USHORT) 0x0FFF)	// 4095 = 0xFFF = 255.9375 spm != 15 m/s (this was the original value that caused a bug in the code)
#define SDM_MAX_CADENCE16		((USHORT) 0x4FFB)	// 20475 = 0x4FFB = 1279.6875 spm = 15 m/s

//Status definitions
#define SDM_STATE_SHIFT			0
#define SDM_STATE_MASK			((UCHAR) (0x03 << SDM_STATE_SHIFT))
#define SDM_STATE_INACTIVE		((UCHAR) 0x00)
#define SDM_STATE_ACTIVE		((UCHAR) 0x01)

#define SDM_HEALTH_SHIFT		2
#define SDM_HEALTH_MASK			((UCHAR) (0x03 << SDM_HEALTH_SHIFT))
#define SDM_HEALTH_OK			((UCHAR) 0x00)
#define SDM_HEALTH_ERROR		((UCHAR) 0x01)
#define SDM_HEALTH_WARNING		((UCHAR) 0x02)

#define SDM_BAT_SHIFT			4
#define SDM_BAT_MASK			((UCHAR) (0x03 << SDM_BAT_SHIFT))
#define SDM_BAT_NEW				((UCHAR) 0x00)
#define SDM_BAT_GOOD			((UCHAR) 0x01)
#define SDM_BAT_OK				((UCHAR) 0x02)
#define SDM_BAT_LOW				((UCHAR) 0x03)

#define SDM_LOCATION_SHIFT		6
#define SDM_LOCATION_MASK		((UCHAR) (0x03 << SDM_LOCATION_SHIFT))
#define SDM_LOCATION_LACES		((UCHAR) 0x00)
#define SDM_LOCATION_MIDSOLE	((UCHAR) 0x01)
#define SDM_LOCATION_CHEST		((UCHAR) 0x02)
#define SDM_LOCATION_OTHER		((UCHAR) 0x03)

#define SDM_COMMON_INTERVAL		65

#define SDM_TIME_MASK			((UCHAR) 0x01)		// Bit masks for the capabilities byte
#define SDM_DISTANCE_MASK		((UCHAR) 0x02)
#define SDM_SPEED_MASK			((UCHAR) 0x04)
#define SDM_LATENCY_MASK		((UCHAR) 0x08)
#define SDM_CADENCE_MASK		((UCHAR) 0x10)
#define SDM_CALORIES_MASK		((UCHAR) 0x20)

#define SDM_CAPAB_ALL			((UCHAR) 0x3F)		// value for having all capabilities on - CHANGE if you add new capabilities
#define SDM_CAPAB_INITIAL		((UCHAR) 0x1F)		// value to initialized the capabilities flag to - this is calories off

public ref struct SDMStatus{
	UCHAR ucLocation;
	UCHAR ucBatteryStatus;
	UCHAR ucHealth;
	UCHAR ucUseState;
};

public ref struct SDMCapab{		// For tracking the capabilities (Request Page 22) bit fields
	UCHAR ucTime;
	UCHAR ucDistance;
	UCHAR ucSpeed;
	UCHAR ucLatency;
	UCHAR ucCadence;
	UCHAR ucCalories;
};

typedef enum{	// Units available for representing simulator data
	SDM_MS,		// m/s (speed)
	SDM_KPH,	// km/h (speed)
	SDM_MPH,	// miles/h (speed)
	SDM_SPM,	// strides/minute (cadence)
} SDMUnit;


#endif