#ifndef __DEBUG_H_
#define __DEBUG_H_

#include <stdio.h>
#include "ble_nus.h"

#ifdef ENABLE_DEBUG_LOG

/**
 * @brief Debug module initialization.
 */
void debug_init(void);

/**@brief Initialize debug with BLE UART service.
 */
void debug_ble_init(ble_nus_t * p_nus);

/**
 * @brief Debug logging retargets printf().
 */
#define debug_log printf

#else // ENABLE_DEBUG_LOG

#define debug_init(...)
#define debug_log(...)
#define debug_ble_init(...)

#endif //ENABLE_DEBUG_LOG

#endif // __DEBUG_H_
