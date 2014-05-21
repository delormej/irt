/* Copyright (c) 2014 Inside Ride Technologies, LLC. All Rights Reserved.
 *
 * @brief	Implements debug logging by redirecting printf().
 *
 */
 
#include <stdio.h>
#include <stdint.h>
#include "simple_uart.h"
#include "ble_nus.h"
#include "boards.h"

struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;

#ifdef BLE_NUS_ENABLED
static ble_nus_t * mp_nus;			// BLE UART service instance.
#endif

void debug_init(void)
{
#ifdef SIMPLE_UART
	simple_uart_config(PIN_UART_RTS, PIN_UART_TXD, PIN_UART_CTS, PIN_UART_RXD, UART_HWFC);
	simple_uart_putstring((const uint8_t*)"[DBG]:Initialized with Simple UART\r\n");
#endif // SIMPLE_UART
#ifdef BLE_NUS_ENABLED
	// Set to invalid connection to start.
	mp_nus = NULL;
#endif // BLE_NUS_ENABLED
}

#ifdef BLE_NUS_ENABLED
/**@brief Initializes debugger to send messages on BLE UART service.
 */
void debug_ble_init(ble_nus_t * p_nus)
{
	mp_nus = p_nus;
}
#endif

/**@brief Overwrites the built-in printf() behavior.
 */
int _write(int fd, char * str, int len)
{
#ifdef SIMPLE_UART
    for (int i = 0; i < len; i++)
    {
        simple_uart_put(str[i]);
    }
#endif // SIMPLE_UART

#if defined(BLE_NUS_ENABLED)
    uint32_t err_code;
    uint8_t position;
    //uint8_t temp_str[BLE_NUS_MAX_DATA_LEN];

    if (mp_nus != NULL)
    {
		// Split messages greater than the max length.
    	position = 0;

		do
		{
			if ( (len-position) > (BLE_NUS_MAX_DATA_LEN /*-1*/ ) )	// subtract 1 to accommodate \f
			{
				// Append formfeed char '\f' as an indication of continuation to the client.
				//temp_str = (uint8_t *)(str[position]);
				//temp_str[BLE_NUS_MAX_DATA_LEN] = '\f';

				err_code = ble_nus_send_string(mp_nus, /*temp_str*/ (uint8_t *)&(str[position]), BLE_NUS_MAX_DATA_LEN);

				if (err_code == BLE_ERROR_GATTS_SYS_ATTR_MISSING)
				{
					err_code = sd_ble_gatts_sys_attr_set(mp_nus->conn_handle, NULL, 0);
				}
				else if (err_code != NRF_SUCCESS && err_code != NRF_ERROR_INVALID_STATE)
				{
					char err_str[16];

					sprintf(&err_str, "[BLE_NUS] Error was: %#.8x\r\n\0", err_code);
					simple_uart_putstring((const uint8_t*)err_str);
				}

				position += (BLE_NUS_MAX_DATA_LEN-1);
			}
			else
			{
				ble_nus_send_string( mp_nus, (uint8_t *)&(str[position]), (len-position) );
				position = len;
			}

		} while (position < len);
    }
#endif // BLE_NUS_ENABLED

    return len;
}
