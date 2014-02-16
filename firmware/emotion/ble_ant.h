#ifndef BLE_ANT_H__
#define BLE_ANT_H__

#include <stdint.h>
#include "ble_cps.h"
#include "ble_gatt.h"

#define BLE_STACK_HANDLER_SCHED_EVT_SIZE	GATT_MTU_SIZE_DEFAULT

#define ANTPLUS_NETWORK_NUMBER          0                                            /**< Network number. */
#define ANTPLUS_RF_FREQ                 0x39                                         /**< Frequency, Decimal 57 (2457 MHz). */

#define ANT_BP_TX_CHANNEL     	 		 		1                                            /**< Bicycle Speed TX ANT Channel. */

#define ANT_BUFFER_INDEX_MESG_ID        0x01                                         /**< Index for Message ID. */
#define ANT_BUFFER_INDEX_MESG_DATA      0x03                                         /**< Index for Data. */
#define ANT_HRMRX_NETWORK_KEY           {0xB9,0xA5,0x21,0xFB,0xBD,0x72,0xC3,0x45}                     /**< The default network key used. */

static uint8_t                          m_ant_network_key[] = ANT_HRMRX_NETWORK_KEY; /**< ANT PLUS network key. */

//
// Event callbacks needed for program flow and control.
//
typedef struct ant_ble_evt_handlers_s {
	void (*on_ble_connected)(void);
	void (*on_ble_disconnected)(void);
	void (*on_ble_timeout)(void);
	void (*on_ble_advertising)(void);
	void (*on_ble_uart)(uint8_t * data, uint16_t length); 
	void (*on_ant_channel_closed)(void);
	void (*on_ant_power_data)(void); // This will have a parameter.
	rc_evt_handler_t on_set_resistance;
} ant_ble_evt_handlers_t;

void ble_ant_init(ant_ble_evt_handlers_t * ant_ble_evt_handlers);
void ble_ant_start(void);
void power_manage(void);
void cycling_power_send(irt_power_meas_t * p_cps_meas);
void debug_send(uint8_t * data, uint16_t length);

#endif // BLE_ANT_H__
