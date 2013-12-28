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
#include "ble.h"
#include "ble_bondmngr.h"
#include "ble_ant_stack_handler.h"
#include "irt_peripheral.h"
#include "ble_ant.h"

static bool                             m_is_advertising = false;                    /**< True when in advertising state, False otherwise. */

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

/**@brief Power manager.
 */
static void power_manage(void)
{
    uint32_t err_code = sd_app_event_wait();
    APP_ERROR_CHECK(err_code);
}

/**@brief Start advertising.
 */
static void advertising_start(void)
{
    ble_advertising_start();
    m_is_advertising = true;
    nrf_gpio_pin_set(ADVERTISING_LED_PIN_NO);
}


/*----------------------------------------------------------------------------
 * ANT message and state handlers
 * ----------------------------------------------------------------------------*/


/**@brief Start receiving the ANT HRM data.
 */
static void ant_hrm_rx_start(void)
{
    uint32_t err_code = sd_ant_channel_open(ANT_HRMRX_ANT_CHANNEL);
    APP_ERROR_CHECK(err_code);
}

/**@brief Handle received ANT data message.
 * 
 * @param[in]  p_evt_buffer   The buffer containg received data. 
 */
static void ant_data_messages_handle(uint8_t * p_evt_buffer)
{
    static uint32_t s_previous_beat_count = 0;    // Heart beat count from previously received page
    uint32_t        err_code;
    uint32_t        current_page;
    uint8_t         beat_count;
    uint8_t         computed_heart_rate;
    uint16_t        beat_time;

    // Decode the default page data present in all pages
    beat_time           = uint16_decode(&p_evt_buffer[ANT_BUFFER_INDEX_MESG_DATA + 4]);
    beat_count          = (uint8_t)p_evt_buffer[ANT_BUFFER_INDEX_MESG_DATA + 6];
    computed_heart_rate = (uint8_t)p_evt_buffer[ANT_BUFFER_INDEX_MESG_DATA + 7];

    // Decode page specific data
    current_page = p_evt_buffer[ANT_BUFFER_INDEX_MESG_DATA];
    switch (current_page & ~ANT_HRM_TOGGLE_MASK)
    {
        case ANT_HRM_PAGE_4:
            // Ensure that there is only one beat between time intervals.
            if ((beat_count - s_previous_beat_count) == 1)
            {
                uint16_t prev_beat = uint16_decode(&p_evt_buffer[ANT_BUFFER_INDEX_MESG_DATA + 2]);
                
                // Subtracting the event time gives the R-R interval
                ble_hrs_rr_interval_add(mp_hrs, beat_time - prev_beat);
            }

            s_previous_beat_count = beat_count;
            break;
          
        case ANT_HRM_PAGE_0:
        case ANT_HRM_PAGE_1:
        case ANT_HRM_PAGE_2:
        case ANT_HRM_PAGE_3:
        default:
            break;
    }
    
    // Notify the received heart rate measurement
    err_code = ble_hrs_heart_rate_measurement_send(mp_hrs, computed_heart_rate);
    if (
        (err_code != NRF_SUCCESS)
        &&
        (err_code != NRF_ERROR_INVALID_STATE)
        &&
        (err_code != BLE_ERROR_NO_TX_BUFFERS)
        &&
        (err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING)
    )
    {
        APP_ERROR_HANDLER(err_code);
    }	
}

/**@brief ANT RX event handler.
 *
 * @param[in]   p_ant_evt   Event received from the stack.
 */
static void on_ant_evt_rx(ant_evt_t * p_ant_evt)
{
   uint32_t message_id = p_ant_evt->evt_buffer[ANT_BUFFER_INDEX_MESG_ID];

    switch (message_id)
    {
        case MESG_BROADCAST_DATA_ID:
        case MESG_ACKNOWLEDGED_DATA_ID:
            ant_data_messages_handle(p_ant_evt->evt_buffer);
            break;

        default:
            break;
    }	
}

/**@brief ANT CHANNEL_CLOSED event handler.
 */
static void on_ant_evt_channel_closed(void)
{
    uint32_t err_code;

    if (!m_is_advertising && (m_conn_handle == BLE_CONN_HANDLE_INVALID))
    {
        // We do not have any activity on the radio at this time, so it is safe to store bonds
        err_code = ble_bondmngr_bonded_masters_store();
        APP_ERROR_CHECK(err_code);
        
        // ANT channel was closed due to a BLE disconnection, restart advertising
        advertising_start();
    }

    // Reopen ANT channel
    ant_hrm_rx_start();
}



/**@brief Application's Stack ANT event handler.
 *
 * @param[in]   p_ant_evt   Event received from the stack.
 */
void on_ant_evt(ant_evt_t * p_ant_evt)
{
    if (p_ant_evt->channel == ANT_HRMRX_ANT_CHANNEL)
    {
        switch (p_ant_evt->event)
        {
            case EVENT_RX:
                on_ant_evt_rx(p_ant_evt);
                break;

            case EVENT_CHANNEL_CLOSED:
                on_ant_evt_channel_closed();
                break;

            default:
                break;
        }
    }
}


/*----------------------------------------------------------------------------
 * BLE message and state handlers
 * ----------------------------------------------------------------------------*/

/**@brief Application's Stack BLE event handler.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 */
void on_ble_evt(ble_evt_t * p_ble_evt)
{
    uint32_t err_code = NRF_SUCCESS;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            m_is_advertising = false;
            nrf_gpio_pin_set(CONNECTED_LED_PIN_NO);
            nrf_gpio_pin_clear(ADVERTISING_LED_PIN_NO);
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            break;
            
        case BLE_GAP_EVT_DISCONNECTED:
            nrf_gpio_pin_clear(CONNECTED_LED_PIN_NO);
            m_conn_handle = BLE_CONN_HANDLE_INVALID;

            // Need to close the ANT channel to make it safe to write bonding information to flash
            err_code = sd_ant_channel_close(ANT_HRMRX_ANT_CHANNEL);
            APP_ERROR_CHECK(err_code);
            
            // Note: Bonding information will be stored, advertising will be restarted and the
            //       ANT channel will be reopened when ANT event CHANNEL_CLOSED is received.
            break;

        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
						ble_gap_sec_params_reply(m_conn_handle);
            break;

        case BLE_GAP_EVT_TIMEOUT:
            if (p_ble_evt->evt.gap_evt.params.timeout.src == BLE_GAP_TIMEOUT_SRC_ADVERTISEMENT)
            { 
                m_is_advertising = false;
                nrf_gpio_pin_clear(ADVERTISING_LED_PIN_NO);
                
                // Go to system-off mode (this function will not return; wakeup will cause a reset)
                err_code = sd_power_system_off();
                APP_ERROR_CHECK(err_code);
            }
            break;
            
        default:
            break;
    }
}


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
		ble_ant_init(on_ble_evt, on_ant_evt, mp_hrs);

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
