#ifndef BLE_ANT_H__
#define BLE_ANT_H__

#include <stdint.h>
#include "irt_common.h"
#include "ant_ctrl.h"
#include "resistance.h"
#include "ant_error.h"

#define HIGH_BYTE(word)              		(uint8_t)((word >> 8u) & 0x00FFu)           /**< Get high byte of a uint16_t. */
#define LOW_BYTE(word)               		(uint8_t)(word & 0x00FFu)                   /**< Get low byte of a uint16_t. */

#define TX_BUFFER_SIZE               		8u               

#define ANTPLUS_NETWORK_NUMBER          	0                                            /**< Network number. */
#define ANTPLUS_RF_FREQ                 	0x39                                         /**< Frequency, Decimal 57 (2457 MHz). */

// ANT Channels being used.
#define ANT_BG_SCAN_CHANNEL					0u
#define ANT_BP_RX_CHANNEL     	 			1u                                           /**< Bicycle Power ANT Channel. */
#define ANT_CTRL_CHANNEL					2u											 /**< ANT+ Remote Control Channel */
#define ANT_SP_TX_CHANNEL     	 			3u                                           /**< Bicycle Speed TX ANT Channel. */
#define ANT_FEC_TX_CHANNEL     	 			4u                                           /**< ANT+ FE-C Channel. */

// Generic ANT pages.
#define ANT_PAGE_GETSET_PARAMETERS			0x02
#define ANT_PAGE_MEASURE_OUTPUT				0x03	// Measurement Output Data Page (0x03)
#define ANT_PAGE_REQUEST_DATA				0x46
#define ANT_PAGE_BATTERY_STATUS				0x52
#define ANT_IRT_PAGE_EXTRA_INFO				0xF1   // Manufacturer specific page sending servo position, etc...
#define ANT_IRT_PAGE_SETTINGS				0xF2   // Manufacturer specific page sending device specific settings.
#define ANT_IRT_PAGE_POWER_ADJUST			0xF3   // Manufacturer specific page sending power adjustment settings.

#define ANT_COMMON_PAGE_80          		0x50u   /**< Manufacturer's identification common data page. */
#define ANT_COMMON_PAGE_81          		0x51u   /**< Product information common data page. */
//#define COMMON_PAGE_BATTERY_VOLTAGE 		0x52u   /**< TODO: Optional battery voltage reporting. */

#define PAGE_NUMBER_INDEX               	0                   /**< Index of the data page number field.  */

// Custom message fields.
#define EXTRA_INFO_SERVO_POS_LSB			1u
#define EXTRA_INFO_SERVO_POS_MSB			2u
#define EXTRA_INFO_TARGET_LSB				3u
#define EXTRA_INFO_TARGET_MSB				4u
#define EXTRA_INFO_FLYWHEEL_REVS_LSB		5u
#define EXTRA_INFO_FLYWHEEL_REVS_MSB		6u
#define EXTRA_INFO_TEMP						7u

#define WF_ANT_RESPONSE_PAGE_ID				0xF0	// Manufacturer-Specific pages (0xF0 - 0xFF).

#define BP_PAGE_RESERVE_BYTE    			0xFFu   /**< Page reserved value. */

#define ANT_BUFFER_INDEX_MESG_ID        	0x01                                         /**< Index for Message ID. */
#define ANT_BUFFER_INDEX_MESG_DATA      	0x03                                         /**< Index for Data. */
#define ANT_NETWORK_KEY						{0xB9,0xA5,0x21,0xFB,0xBD,0x72,0xC3,0x45}

static const uint8_t m_ant_network_key[] = ANT_NETWORK_KEY;

uint8_t ant_product_page[TX_BUFFER_SIZE];
uint8_t ant_manufacturer_page[TX_BUFFER_SIZE];

typedef enum
{
	DISCONNECTED,
	CONNECTED,
	ADVERTISING
} ble_state_e;

extern ble_state_e irt_ble_ant_state;

/**@brief	Checks to see if this ANT error could be treated as a warning.
 */
#define ANT_ERROR_AS_WARN(ERR) \
		(ERR == NRF_ANT_ERROR_TRANSFER_BUSY || \
			ERR == NRF_ANT_ERROR_TRANSFER_IN_PROGRESS)

/**@brief Macros for identifying ANT Burst Sequence in upper 3 bits of channel byte.
 */
#define BURST_LAST_PACKET						0x04				// 0b100 Flags last packet.
#define BURST_SEQ_NUM(CHANNEL_BYTE) 			(CHANNEL_BYTE >> 5)
#define BURST_SEQ_FIRST_PACKET(SEQ_NUM) 		(SEQ_NUM == 0x0)
#define BURST_SEQ_LAST_PACKET(SEQ_NUM) \
	(SEQ_NUM & BURST_LAST_PACKET) == BURST_LAST_PACKET

typedef void(*ant_bp_evt_dfu_enable)(void);

/**@brief Bike power Event Handler type.
 */
typedef enum 
{
	BP_MSG_POWER_DATA = 0,
	BP_MSG_DEVICE_CONNECTED,
	BP_MSG_DEVICE_SEARCH_TIME_OUT,
	BP_MSG_DEVICE_CLOSED,
	FEC_MSG_NEW_DEVICE_ID
} ant_bp_message_type_e;
typedef void (*bp_evt_handler_t)(ant_bp_message_type_e state, uint16_t data);

//
// Event callbacks needed for program flow and control.
//
typedef struct ant_ble_evt_handlers_s {
	void (*on_ble_connected)(void);					// BLE connection is established.
	void (*on_ble_disconnected)(void);				// BLE connection is dropped.
	void (*on_ble_timeout)(void);					// BLE advertising times out.
	void (*on_ble_advertising)(void);				// BLE starts advertising.
	void (*on_ble_uart)(uint8_t * data, uint16_t length);	// When UART over BLE is enabled and data is received.
	rc_evt_handler_t on_set_resistance;				// Device receives a command to set resistance.
	ctrl_evt_handler_t on_ant_ctrl_command;			// ANT control command is received from the remote control.
	ant_bp_evt_dfu_enable on_enable_dfu_mode;		// Device receives a request to enter device firmware update mode.
	void (*on_request_data)(uint8_t* buffer);		// Device receives page (0x46) requesting data page.
	void (*on_set_parameter)(uint8_t* buffer);		// Device receives page (0x02) with values to set.
	void (*on_set_servo_positions)(servo_positions_t* positions); // Received command to set servo positions.
	void (*on_request_calibration)(void);			// Display requests calibration to begin.
	void (*on_set_mag_calibration)(mag_calibration_factors_t* factors); // Received command to set magnet calibration.
	bp_evt_handler_t bp_evt_handler;				// Device receives relevant ant power meter message.
	void (*on_toggle_bg_scanning)(void);			// Toggles background scanning channel on/off for discovering power meter.
	void (*on_blink_led)(void);						// Blinks LED for identification.
} ant_ble_evt_handlers_t;

/**@brief	Represents Common Data Page 70.
 *
 *@NOTE: Use memcpy to move the buffer into this struct since you can't just cast an 8 byte buffer to a
 *@NOTE: struct due to compiler alignment unless __attribute__((packed)) was used.
 */
typedef struct ant_request_data_page_s
{
	uint8_t data_page;								// = 0x46 Common Data Page 70: Request Data Page
	uint8_t reserved[2];							// Unused
	uint8_t descriptor[2];							// Allows data specification.
	uint8_t tx_response;							// Bit 0-6: Number of times to transmit page., bit 7: If set, send ack message (0x80)
	uint8_t tx_page;								// Page to transmit
	uint8_t command_type;							// 0x01 Request Data Page, 0x02 request ANT-FS Session
} ant_request_data_page_t;

// Public methods.
void ble_ant_init(ant_ble_evt_handlers_t * ant_ble_evt_handlers);
void ble_ant_start(void);
void ble_advertising_start(void);
uint8_t encode_resistance_level(irt_context_t * p_power_meas);
void cycling_power_send(irt_context_t * p_cps_meas);
void ble_ant_resistance_ack(uint8_t op_code, uint16_t value);
void ant_common_page_transmit(uint8_t ant_channel, uint8_t* common_page);

#endif // BLE_ANT_H__
