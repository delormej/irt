#ifndef __DEBUG_H_
#define __DEBUG_H_

#include <stdio.h>
#include "irt_peripheral.h"

#ifdef ENABLE_DEBUG_LOG

#define SECONDS_CURRENT \
	(NRF_RTC1->COUNTER / (float)TICK_FREQUENCY)

/**
 * @brief Debug module initialization.
 */
void debug_init(void);

/**
 * @brief Debug logging retargets printf().
 */
#define debug_log printf

#else // ENABLE_DEBUG_LOG

#define debug_init(...)
#define debug_log(...)

#endif //ENABLE_DEBUG_LOG

#endif // __DEBUG_H_
