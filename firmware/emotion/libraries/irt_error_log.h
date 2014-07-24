/* Copyright (c) 2012 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/** @file
 *
 * @defgroup ble_error_log_module Error Log Module
 * @{
 * @ingroup ble_sdk_lib
 * @brief Module for writing error and stack to flash memory.
 *
 * @details It contains functions for writing an error code, line number, filename/message and
 *          the stack to the flash during an error, e.g. in the assert handler.
 *
 */
#ifndef IRT_ERROR_LOG_H__
#define IRT_ERROR_LOG_H__

#include <stdint.h>
#include <stdbool.h>

#define ERROR_MESSAGE_LENGTH  128                                /**< Length of error message to stored. */
#define STACK_DUMP_LENGTH     256                                /**< Length of stack to be stored at max: 64 entries of 4 bytes each. */
#define FLASH_PAGE_ERROR_LOG  (BLE_FLASH_PAGE_END - 2)           /**< Address in flash where stack trace can be stored. */

#define IRT_ERROR_MODULE_ID	  1u									 /**< Persistent storage module offset. */
#define IRT_ERROR_OFFSET	  0u

/**@brief Error Log Data structure.
 *
 * @details The structure contains the error, message/filename, line number as well as the current
 *          stack, at the time where an error occurred.
 */
typedef struct
{
    uint16_t                  failure;                           /**< Indication that a major failure has occurred during last execution of the application. */
    uint16_t                  line_number;                       /**< Line number indicating at which line the failure occurred. */
    uint32_t                  err_code;                          /**< Error code when failure occurred. */
    uint8_t                   message[ERROR_MESSAGE_LENGTH];     /**< Will just use the first 128 bytes of filename to store for debugging purposes. */
    uint32_t                  stack_info[STACK_DUMP_LENGTH / 4]; /**< Will contain stack information, can be manually unwinded for debug purposes. */
} irt_error_log_data_t;


/**@brief Function for saving the file name/message, line number, and current program stack.
 * 
 *
 * @param[in]   err_code    Error code to be logged.
 * @param[in]   p_message   Message to be written to the flash together with stack dump, usually
 *                          the file name where the error occurred.
 * @param[in]   line_number Line number where the error occurred.
 *
 */
void irt_error_save(uint32_t err_code, uint16_t line_number, const uint8_t * p_message);

/**@brief	Initializes the error logging structure for first use.
 */
void irt_error_init();

/**@brief	Returns last error recorded, use .failure for a count.
 *
 */
irt_error_log_data_t* irt_error_last(void);

#endif /* IRT_ERROR_LOG_H__ */

/** @} */
