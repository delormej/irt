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
 * @defgroup ble_sdk_srv_nus Nordic UART Service
 * @{
 * @ingroup ble_sdk_srv
 * @brief Nordic UART Service module.
 *
 * @details This module implements the Nordic UART Service with an RX and TX characteristics. Data
 *          received will be passed to the application, while data sent will be transferred as 
 *          notifications. 
 *          During initialization it adds the Nordic UART Service and its characteristics to 
 *          the BLE stack datanuse. 
 *
 * @note The application must propagate BLE stack events to the Nordic UART Service module by calling
 *       ble_nus_on_ble_evt() from the from the @ref ble_stack_handler callback.
 */

#ifndef BLE_NUS_H__
#define BLE_NUS_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"

#define BLE_UUID_NUS_SERVICE 0x0001
#define BLE_UUID_NUS_TX_CHARACTERISTIC 0x0002
#define BLE_UUID_NUS_RX_CHARACTERISTIC 0x0003

#define NUS_MAX_DATA_LENGTH 20


// Forward declaration of the ble_nus_t type. 
typedef struct ble_nus_s ble_nus_t;

/**@brief Nordic UART Service event handler type. */
typedef void (*ble_nus_data_handler_t) (ble_nus_t * p_nus, uint8_t * data, uint16_t length);

/**@brief Nordic UART Service init structure. This contains all options and data needed for
 *        initialization of the service.*/
typedef struct
{
    ble_nus_data_handler_t data_handler;              /**< Event handler to be called for handling received data. */
} ble_nus_init_t;

/**@brief Nordic UART Service structure. This contains various status information for the service. */
typedef struct ble_nus_s
{
    uint8_t                  uuid_type;               /**< UUID type for Nordic UART Service Base UUID. */
    uint16_t                 service_handle;          /**< Handle of Nordic UART Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t tx_handles;              /**< Handles related to the TX characteristic. */
    ble_gatts_char_handles_t rx_handles;              /**< Handles related to the RX characteristic. */
    uint16_t                 conn_handle;             /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
    bool                     is_notification_enabled; 
    ble_nus_data_handler_t   data_handler;            /**< Event handler to be called for handling received data. */
} ble_nus_t;

/**@brief Initialize the Nordic UART Service.
 *
 * @param[out]  p_nus       Nordic UART Service structure. This structure will have to be supplied by
 *                          the application. It will be initialized by this function, and will later
 *                          be used to identify this particular service instance.
 * @param[in]   p_nus_init  Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
 */
uint32_t ble_nus_init(ble_nus_t * p_nus, const ble_nus_init_t * p_nus_init);

/**@brief Nordic UART Service BLE stack event handler.
 *
 * @details Handles all events from the BLE stack of interest to the Nordic UART Service.
 *
 * @note For the requirements in the nus specification to be fulfilled,
 *       ble_nus_battery_level_update() must be called upon reconnection if the
 *       battery level has changed while the service has been disconnected from a bonded
 *       client.
 *
 * @param[in]   p_nus      Nordic UART Service structure.
 * @param[in]   p_ble_evt  Event received from the BLE stack.
 */
void ble_nus_on_ble_evt(ble_nus_t * p_nus, ble_evt_t * p_ble_evt);

/**@brief Send string. 
 *
 * @details The application calls this function to send a string to the peer device. 
 *
 *
 * @param[in]   p_nus          Nordic UART Service structure.
 * @param[in]   string         String to be sent.
 * @param[in]   length         Length of string. 
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t ble_nus_send_string(ble_nus_t * p_nus, uint8_t * string, uint16_t length);

#endif // BLE_NUS_H__

/** @} */
