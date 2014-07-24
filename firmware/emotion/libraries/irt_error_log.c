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

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <nrf51.h>
#include "irt_error_log.h"
#include "nordic_common.h"
#include "app_util.h"

//
// RAM is never reset, so on a system reset - we pin error log to a specific spot in memory which is not re-initialized.
// Also retain a running count of the # of errors that have occurred.
//
static irt_error_log_data_t 			m_irt_error_log	__attribute__ ((section(".noinit")));

//lint -esym(526,__Vectors)
extern uint32_t             			* __Vectors;                  /**< The initialization vector holds the address to __initial_sp that will be used when fetching the stack. */

static void fetch_stack(irt_error_log_data_t * error_log)
{
    uint32_t * p_stack;
    uint32_t * initial_sp;
    uint32_t   length;
  
    initial_sp = (uint32_t *) __Vectors;
    p_stack    = (uint32_t *) GET_SP();
  
    length = ((uint32_t) initial_sp) - ((uint32_t) p_stack);
    memcpy(error_log->stack_info, 
           p_stack, 
           (length > STACK_DUMP_LENGTH) ? STACK_DUMP_LENGTH : length);
}

void irt_error_init()
{
	memset(&m_irt_error_log, 0, sizeof(irt_error_log_data_t));
}

irt_error_log_data_t* irt_error_last(void)
{
	return &m_irt_error_log;
}

void irt_error_save(uint32_t err_code, uint16_t line_number, const uint8_t * p_message)
{
    m_irt_error_log.failure++;	// Increment the failure count.
    m_irt_error_log.err_code    = err_code;
    m_irt_error_log.line_number = line_number;

    strncpy((char *)m_irt_error_log.message, (const char *)p_message, ERROR_MESSAGE_LENGTH - 1);
    m_irt_error_log.message[ERROR_MESSAGE_LENGTH - 1] = '\0';

    fetch_stack(&m_irt_error_log);
}
