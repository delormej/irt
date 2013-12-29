/*
*******************************************************************************
*
* By Jason De Lorme <jjdelorme@yahoo.com>
* http://www.roadacious.com
*
* This module is responsible for calculating power based on rider profile 
* e.g. weight, current speed and resistance.
*
********************************************************************************/

#ifndef __USER_PROFILE_H__
#define __USER_PROFILE_H__

#include <stdint.h>

typedef struct user_profile_s {
	float			total_weight_lb;
	uint16_t	wheel_size_mm;
} user_profile_t;

#endif // __USER_PROFILE_H__
