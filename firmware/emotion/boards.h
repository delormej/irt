/* Copyright (c) 2014 Inside Ride Technologies, LLC. All Rights Reserved.
*
* @brief	Includes the appropriate board header which defines port pins and what they do.
*
*/
#ifndef BOARDS_H
#define BOARDS_H

#if defined(BOARD_IRT_REV_A)
  #include "boards/irt_rev_a.h"
#elif defined(BOARD_IRT_REV_2A)
  #include "boards/irt_rev_2a.h"
#elif defined(BOARD_DYN_BAT_N548)
  #include "boards/dyn_bat_N548.h"
#else
#error "Board is not defined"
#endif

#endif
