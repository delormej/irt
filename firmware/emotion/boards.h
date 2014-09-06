/* Copyright (c) 2014 Inside Ride Technologies, LLC. All Rights Reserved.
*
* @brief	Includes the appropriate board header which defines port pins and what they do.
*
*/
#ifndef BOARDS_H
#define BOARDS_H

#if defined(BOARD_IRT_V2_REV_A1)
  #define HW_REVISION	3
  #include "boards/board_irt_v2a1.h"

#elif defined(BOARD_IRT_V2_REV_A)
  #define HW_REVISION	2
  #include "boards/board_irt_v2a.h"

#elif defined(BOARD_IRT_REV_A)
  #define HW_REVISION	1
  #include "boards/board_irt_rev_a.h"

#elif defined(BOARD_DYN_BAT_N548)
  #define HW_REVISION	0
  #include "boards/dyn_bat_N548.h"

#elif defined(BOARD_PCA10001)
  #define HW_REVISION	0
  #include "boards/pca10001.h"

#else
  #error "Board is not defined"

#endif

#endif
