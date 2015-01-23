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

/*
**@brief Returns the watts for a given position & speed.
*
*/
float magnet_watts_from_position(uint16_t position, float speed_mps);

/*
 **@brief Solves for the servo position given desired target mag only watts.
 *
 */
uint16_t magnet_position_from_watts(uint16_t watts, float speed_mps);

#endif /* MAGNET_H_ */
