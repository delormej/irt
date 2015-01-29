/******************************************************************************
 * Copyright (c) 2015 Inside Ride Technologies, LLC. All Rights Reserved.
 *
 * @file  	magnet.h
 * @brief	Encapsulates the calculations for magnetic resistance.
 *
 */

#ifndef MAGNET_H_
#define MAGNET_H_

#include <stdint.h>
#include <float.h>

/**@brief	Calculates watts added by the magnet for a given speed at magnet
 *			position.
 */
float magnet_watts(float speed_mps, uint16_t position);

/**@brief	Calculates magnet position for a given speed and watt target.
 *
 */
uint16_t magnet_position(float speed_mps, float mag_watts);

#endif /* MAGNET_H_ */
