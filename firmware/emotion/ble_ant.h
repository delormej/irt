#ifndef BLE_ANT_H__
#define BLE_ANT_H__

#include <stdint.h>
#include "irt_emotion.h"
#include "ble_cps.h"
#include "ble_gatt.h"
#include "ant_ctrl.h"

#define HIGH_BYTE(word)              		(uint8_t)((word >> 8u) & 0x00FFu)           /**< Get high byte of a uint16_t. */
#define LOW_BYTE(word)               		(uint8_t)(word & 0x00FFu)                   /**< Get low byte of a uint16_t. */

#define BLE_STACK_HANDLER_SCHED_EVT_SIZE	GATT_MTU_SIZE_DEFAULT
#define TX_BUFFER_SIZE               		8u               

#define ANTPLUS_NETWORK_NUMBER          	0                                            /**< Network number. */
#define ANTPLUS_RF_FREQ                 	0x39                                         /**< Frequency, Decimal 57 (2457 MHz). */

// ANT Channels being used.
#define ANT_BP_TX_CHANNEL     	 			1u                                           /**< Bicycle Speed TX ANT Channel. */
#define ANT_CTRL_CHANNEL					2u											 /**< ANT+ Remote Control Channel */

#define ANT_COMMON_PAGE_80          		0x50u   /**< Manufacturer's identification common data page. */
#define ANT_COMMON_PAGE_81          		0x51u   /**< Product information common data page. */
//#define COMMON_PAGE_BATTERY_VOLTAGE 		0x52u   /**< TODO: Optional battery voltage reporting. */

#define BP_PAGE_RESERVE_BYTE    			0xFFu   /**< Page reserved value. */

#define ANT_BUFFER_INDEX_MESG_ID        	0x01                                         /**< Index for Message ID. */
#define ANT_BUFFER_INDEX_MESG_DATA      	0x03                                         /**< Index for Data. */
#define ANT_NETWORK_KEY						{0xB9,0xA5,0x21,0xFB,0xBD,0x72,0xC3,0x45}	 /**< The default network key used. */

static const uint8_t                    	m_ant_network_key[] = ANT_NETWORK_KEY;		 /**< ANT+ network key. */

static const uint8_t ant_product_page[TX_BUFFER_SIZE] =
{
	ANT_COMMON_PAGE_81,
	BP_PAGE_RESERVE_BYTE,
	SW_REVISION_MIN,
	SW_REVISION_MAJ,
	(uint8_t)(SERIAL_NUMBER),
	(uint8_t)(SERIAL_NUMBER >> 8u),
	(uint8_t)(SERIAL_NUMBER >> 16u),
	(uint8_t)(SERIAL_NUMBER >> 24u)
};

static const uint8_t ant_manufacturer_page[TX_BUFFER_SIZE] =
{
	ANT_COMMON_PAGE_80,
	BP_PAGE_RESERVE_BYTE,
	BP_PAGE_RESERVE_BYTE,
	HW_REVISION,
	LOW_BYTE(MANUFACTURER_ID),
	HIGH_BYTE(MANUFACTURER_ID),
	LOW_BYTE(MODEL_NUMBER),
	HIGH_BYTE(MODEL_NUMBER)
};


// MACRO for sending manufacturer and product pages.
#define ANT_COMMON_PAGE_TRANSMIT(ANT_CHANNEL, COMMON_PAGE)					\
    do																		\
    {																		\
        uint32_t ERR_CODE = sd_ant_broadcast_message_tx((ANT_CHANNEL),		\
                                            TX_BUFFER_SIZE,					\
                                            (uint8_t*)&(COMMON_PAGE) );		\
        APP_ERROR_CHECK(ERR_CODE);											\
    } while (0)																\

typedef void(*ant_bp_evt_dfu_enable)(void);

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
	ctrl_evt_handler_t on_ant_ctrl_command;
	ant_bp_evt_dfu_enable on_enable_dfu_mode;
} ant_ble_evt_handlers_t;

// Public methods.
void ble_ant_init(ant_ble_evt_handlers_t * ant_ble_evt_handlers);
void ble_ant_start(void);
void ble_advertising_start(void);
void power_manage(void);
void cycling_power_send(irt_power_meas_t * p_cps_meas);
void debug_send(uint8_t * data, uint16_t length);

#endif // BLE_ANT_H__
