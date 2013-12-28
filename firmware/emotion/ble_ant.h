#ifndef BLE_ANT_H__
#define BLE_ANT_H__

#include "ant_parameters_ds.h"
#include "ant_interface_ds.h"
#include "ble_ant_stack_handler.h"

#define ANT_HRMRX_ANT_CHANNEL           0                                            /**< Default ANT Channel. */
#define ANT_HRMRX_CHANNEL_TYPE          0x40                                         /**< Channel Type Slave RX only. */
#define ANT_HRMRX_DEVICE_TYPE           0x78                                         /**< Channel ID device type. */
#define ANT_HRMRX_DEVICE_NUMBER         0                                            /**< Device Number. */
#define ANT_HRMRX_TRANS_TYPE            0                                            /**< Transmission Type. */
#define ANT_HRMRX_MSG_PERIOD            0x1F86                                       /**< Message Periods, decimal 8070 (4.06Hz). */
#define ANT_HRMRX_EXT_ASSIGN            0x00                                         /**< ANT Ext Assign. */
#define ANT_HRM_TOGGLE_MASK             0x80                                         /**< HRM Page Toggle Bit Mask. */
#define ANTPLUS_NETWORK_NUMBER          0                                            /**< Network number. */
#define ANTPLUS_RF_FREQ                 0x39                                         /**< Frequency, Decimal 57 (2457 MHz). */
#define ANT_HRM_PAGE_0                  0                                            /**< HRM page 0 constant. */
#define ANT_HRM_PAGE_1                  1                                            /**< HRM page 1 constant. */
#define ANT_HRM_PAGE_2                  2                                            /**< HRM page 2 constant. */
#define ANT_HRM_PAGE_3                  3                                            /**< HRM page 3 constant. */
#define ANT_HRM_PAGE_4                  4                                            /**< HRM page 4 constant. */
#define ANT_BUFFER_INDEX_MESG_ID        0x01                                         /**< Index for Message ID. */
#define ANT_BUFFER_INDEX_MESG_DATA      0x03                                         /**< Index for Data. */
#define ANT_HRMRX_NETWORK_KEY           {0xB9,0xA5,0x21,0xFB,0xBD,0x72,0xC3,0x45}                     /**< The default network key used. */

static uint8_t                          m_ant_network_key[] = ANT_HRMRX_NETWORK_KEY; /**< ANT PLUS network key. */

void ble_gap_sec_params_reply(uint16_t m_conn_handle);
void ble_advertising_start(void);
void ble_ant_init();
									
#endif // BLE_ANT_H__
