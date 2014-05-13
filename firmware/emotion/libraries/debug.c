/* Copyright (c) 2014 Inside Ride Technologies, LLC. All Rights Reserved.
 *
 * @brief	Implements debug logging by redirecting printf().
 *
 */
 
#include <stdio.h>
#include <stdint.h>
#include "simple_uart.h"
#include "irt_peripheral.h"

struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;

void debug_init(void)
{
#ifdef SIMPLE_UART
	simple_uart_config(PIN_UART_RTS, PIN_UART_TXD, PIN_UART_CTS, PIN_UART_RXD, UART_HWFC);
	simple_uart_putstring("[DBG]:Initialized with Simple UART\r\n");
}

int _write(int fd, char * str, int len)
{
    for (int i = 0; i < len; i++)
    {
        simple_uart_put(str[i]);
    }
    return len;
#endif // SIMPLE_UART
}


