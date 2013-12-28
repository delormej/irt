/****************************************************************************** 
 * Copyright (c) 2013 Inside Ride Technologies, LLC. All Rights Reserved.
 *
 * E-Motion Roller firmware using BLE and ANT.
 *
 * The application uses the follwowing BLE services:
 *		Cycling Power Service
 *		Device Information Service, 
 * 		Battery Service 
 *		Nordic UART Service (for debugging purposes)
 *
 * The application uses the following ANT services:
 *		ANT Cycling Power RX profile
 *		ANT Cycling Power TX profile
 *		ANT Speed Sensor TX profile
 *
 * @file 		main.c 
 * @brief 	The purpose of main is to control program flow and manage any
 *					application specific state.  All other protocol details are handled 
 *					in referenced modules.
 *
 ******************************************************************************/

#include <stdint.h>
#include <string.h>
#include "nrf_error.h"
#include "irt_peripheral.h"
#include "ble_ant.h"

/*----------------------------------------------------------------------------
 * Error Handlers
 * ----------------------------------------------------------------------------*/

/**@brief Error handler function, which is called when an error has occurred. 
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze 
 *          how your product is supposed to react in case of error.
 *
 * @param[in] error_code  Error code supplied to the handler.
 * @param[in] line_num    Line number where the handler is called.
 * @param[in] p_file_name Pointer to the file name. 
 */
void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name)
{
    nrf_gpio_pin_set(ASSERT_LED_PIN_NO);

    // This call can be used for debug purposes during development of an application.
    // @note CAUTION: Activating this code will write the stack to flash on an error.
    //                This function should NOT be used in a final product.
    //                It is intended STRICTLY for development/debugging purposes.
    //                The flash write will happen EVEN if the radio is active, thus interrupting
    //                any communication.
    //                Use with care. Un-comment the line below to use.
    // ble_debug_assert_handler(error_code, line_num, p_file_name);

    // On assert, the system can only recover with a reset.
    NVIC_SystemReset();
}

/*----------------------------------------------------------------------------
 * 
 * ----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * Main program functions
 * ----------------------------------------------------------------------------*/

/**@brief Application main function.
 */
int main(void)
{
    // Initialize peripherals
		peripheral_init();

		// Initialize Bluetooth and ANT stacks.
		ble_ant_init();

    // Start execution
    advertising_start();
		
		// Start receiving ANT HRM messages.
		ant_hrm_rx_start();

    // Enter main loop
    for (;;)
    {
        power_manage();
    }
}
