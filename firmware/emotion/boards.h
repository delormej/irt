/* Copyright (c) 2014 Inside Ride Technologies, LLC. All Rights Reserved.
*
* @brief	Includes the appropriate board header which defines port pins and what they do.
*
*/
#ifndef BOARDS_H
#define BOARDS_H

#if defined(BOARD_IRT_REV_A)
  #include "boards/board_irt_rev_a.h"
  #define HW_REVISION	0x01
#elif defined(BOARD_IRT_V2_REV_A)
  #include "boards/board_irt_v2a.h"
  #define HW_REVISION	0x02
#elif defined(BOARD_DYN_BAT_N548)
  #include "boards/dyn_bat_N548.h"
  #define HW_REVISION	0xFF
#elif defined(BOARD_PCA10001)
  #include "boards/pca10001.h"
  #define HW_REVISION	0xFF
#else
#error "Board is not defined"
#endif

#endif
