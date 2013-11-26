/* Copyright (c) 2013 Inside Ride Technologies, LLC. All Rights Reserved.
*/

/** @file
*
* @brief Cycling Power Service module.
*
* @details This module implements the Cycling Power Service with the Cycling Power Measurement,
*          Sensor Location, Cycling Power Feature and Cycling Power Control Point characteristics.
*          During initialization it adds the Cycling Power Service and Cycling Power Measurement
*          characteristic to the BLE stack database. It also adds the Sensor Location and optionally
*          the Cycling Power Control Point characteristics.
*
*          If enabled, notification of the Cycling Power Measurement characteristic is performed
*          when the application calls ble_cps_cycling_power_measurement_send().
*
*          If an event handler is supplied by the application, the Cycling Power Service will
*          generate Cycling Power Service events to the application.
*
* @note The application must propagate BLE stack events to the Cycling Power Service module by calling
*       ble_cps_on_ble_evt() from the @ref ble_stack_handler callback.
*/


/*

This is what the WAHOO reports:

Primary Service: Generic Access 0x1800
	DeviceName, Read, UUID: 0x2A00
	Appearance, Read, UUID: 0x2A01
	SlavePreferredConnectionParameters, Read, UUID: 0x2A04
Primary Service: Generic Attribute 0x1801
Primary Service: 0xA026EE01-0A7D-4AB3-97FA-F1500F9FEB8B
	WriteWithoutResponse, Notify, UUID: 0xA026EE02-0A7D-4AB3-97FA-F1500F9FEB8B
	Notify, UUID: 0xA026EE04-0A7D-4AB3-97FA-F1500F9FEB8B
Primary Service: Cycling Power org.bluetooth.service.cycling_power 0x1818 

https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicsHome.aspx
Cycling Power Control Point bluetooth.characteristic.cycling_power_control_point 0x2A66 
Cycling Power Feature org.bluteooth.characteristic.cycling_power_feature 0x2A65 
Cycling Power Measurement org.blueeooth.cycling_power_measurement 0x2A63 
Cycling Power Vector org.bluetooth.characteristic.cycling_power_vector 0x2A64 


*/


#ifndef BLE_CPS_H__
#define BLE_CPS_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"


/**@brief Cycling Power Service event type. */
typedef enum
{
	BLE_CPS_EVT_NOTIFICATION_ENABLED,                   /**< Cycling Power value notification enabled event. */
	BLE_CPS_EVT_NOTIFICATION_DISABLED                   /**< Cycling Power value notification disabled event. */
} ble_cps_evt_type_t;

/**@brief Cycling Power Service event. */
typedef struct
{
	ble_cps_evt_type_t evt_type;                        /**< Type of event. */
} ble_cps_evt_t;

/**@brief Cycling Power Service measurement type. */
typedef struct ble_cps_meas_s
{
	uint16_t  flags;																		// 16 bits defined here: https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.cycling_power_measurement.xml
	int16_t		instant_power;         										// Note this is a SIGNED int16
	uint8_t		pedal_power_balance;
	uint16_t	accum_torque;															// Unit is in newton metres with a resolution of 1/32
	uint32_t	accum_wheel_revs;
	uint16_t	last_wheel_event_time;										// Unit is in seconds with a resolution of 1/2048. 
	uint16_t	accum_crank_rev;
	uint16_t	last_crank_event_time;										// Unit is in seconds with a resolution of 1/1024. 
	int16_t		max_force_magnitude;											// Unit is in newtons with a resolution of 1.
	int16_t		min_force_magnitude;											// Unit is in newtons with a resolution of 1.
	int16_t		max_torque_magnitude;											// Unit is in newton metres with a resolution of 1/32
	int16_t		min_torque_magnitude;											// Unit is in newton metres with a resolution of 1/32
	uint16_t	max_angle:12;																// Unit is in degrees with a resolution of 1. 
	uint16_t	min_angle:12;																// Unit is in degrees with a resolution of 1. 
	uint16_t	top_dead_spot_angle;											// Unit is in degrees with a resolution of 1. 
	uint16_t	bottom_dead_spot_angle;										// Unit is in degrees with a resolution of 1. 
	uint16_t	accum_energy;															// Unit is in kilojoules with a resolution of 1.
} ble_cps_meas_t;


// Forward declaration of the ble_cps_t type. 
typedef struct ble_cps_s ble_cps_t;

/**@brief Cycling Power Service event handler type. */
typedef void(*ble_cps_evt_handler_t) (ble_cps_t * p_cps, ble_cps_evt_t * p_evt);

/**@brief Cycling Power Service init structure. This contains all options and data needed for
*        initialization of the service. */
typedef struct
{
	ble_cps_evt_handler_t        evt_handler;                                         /**< Event handler to be called for handling events in the Cycling Power Service. */
	bool                         use_cycling_power_control_point;                     /**< Determines if cycling power control point is supported.  This is required if sensor supports wheel revolution data. */
	bool                         use_cycling_power_vector;
	uint8_t *                    p_sensor_location;                               		/**< Pointer to initial value of the Sensor Location characteristic. */
	uint32_t 									 	 feature;																							/**< Bitmask of enabled features. */
	ble_cps_meas_t *						 p_cps_meas;																					/**< Initial cycling power service measurement structure. */
	ble_srv_security_mode_t      cps_cpf_attr_md;                                     /**< Initial security level for cycling power feature attribute */
	ble_srv_security_mode_t      cps_cpm_attr_md;                                     /**< Initial security level for cycling power measurement attribute */
	ble_srv_security_mode_t      cps_sl_attr_md;                                      /**< Initial security level for sensor location attribute */
	ble_srv_security_mode_t      cps_cpcp_attr_md;                                    /**< Initial security level for cycling power control point attribute */
	ble_srv_security_mode_t      cps_cpv_attr_md;                                    	/**< Initial security level for cycling power vector attribute */	
} ble_cps_init_t;

/**@brief Cycling Power Service structure. This contains various status information for the service. */
typedef struct ble_cps_s
{
	ble_cps_evt_handler_t        evt_handler;                                         /**< Event handler to be called for handling events in the Cycling Power Service. */
	uint16_t                     service_handle;                                      /**< Handle of Cycling Power Service (as provided by the BLE stack). */
	uint32_t										 feature;																							/**< Bitmask of features enabled. */
	ble_gatts_char_handles_t     cpf_handles;                                         /**< Handles related to the Cycling Power Feature characteristic. */
	ble_gatts_char_handles_t     cpm_handles;                                         /**< Handles related to the Cycling Power Measurement characteristic. */
	ble_gatts_char_handles_t     sl_handles;                                          /**< Handles related to the Sensor Location characteristic. */
	ble_gatts_char_handles_t     cpcp_handles;                                        /**< Handles related to the Cycling Power Control Point characteristic. */
	ble_gatts_char_handles_t     cpv_handles;                                         /**< Handles related to the Cycling Power Vector characteristic. */
	uint16_t                     conn_handle;                                         /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
} ble_cps_t;

/**@brief Function for initializing the Cycling Power Service.
*
* @param[out]  p_cps       Cycling Power Service structure. This structure will have to be supplied by
*                          the application. It will be initialized by this function, and will later
*                          be used to identify this particular service instance.
* @param[in]   p_cps_init  Information needed to initialize the service.
*
* @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
*/
uint32_t ble_cps_init(ble_cps_t * p_cps, const ble_cps_init_t * p_cps_init);

/**@brief Function for handling the Application's BLE Stack events.
*
* @details Handles all events from the BLE stack of interest to the Cycling Power Service.
*
* @param[in]   p_cps      Cycling Power Service structure.
* @param[in]   p_ble_evt  Event received from the BLE stack.
*/
void ble_cps_on_ble_evt(ble_cps_t * p_cps, ble_evt_t * p_ble_evt);

/**@brief Function for sending cycling power measurement if notification has been enabled.
*
* @details The application calls this function after having performed a cycling power measurement.
*          If notification has been enabled, the measurement data is encoded and sent to
*          the client.
*
* @param[in]   p_cps                    Cycling Power Service structure.
* @param[in]   ble_cps_meas_t           New cycling power measurement structure.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
uint32_t ble_cps_cycling_power_measurement_send(ble_cps_t * p_cps, ble_cps_meas_t * power);

#endif // BLE_CPS_H__

/** @} */
