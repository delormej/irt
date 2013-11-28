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
#include "ble_sc_ctrlpt.h"

// https://developer.bluetooth.org/gatt/services/Pages/ServicesHome.aspx
#define BLE_UUID_CYCLING_POWER_SERVICE                  0x1818    /**< Cycling Power Service UUID. */

// Sensor Location values (unit8)
#define BLE_CPS_SENSOR_LOCATION_OTHER										0
#define BLE_CPS_SENSOR_LOCATION_TOP_SHOE								1 // Top of shoe
#define BLE_CPS_SENSOR_LOCATION_IN_SHOE									2 // In shoe
#define BLE_CPS_SENSOR_LOCATION_HIP											3
#define BLE_CPS_SENSOR_LOCATION_FRONT_WHEEL							4 
#define BLE_CPS_SENSOR_LOCATION_LEFT_CRANK							5 
#define BLE_CPS_SENSOR_LOCATION_RIGHT_CRANK							6 
#define BLE_CPS_SENSOR_LOCATION_LEFT_PEDAL							7 
#define BLE_CPS_SENSOR_LOCATION_RIGHT_PEDAL							8 
#define BLE_CPS_SENSOR_LOCATION_FRONT_HUB								9 
#define BLE_CPS_SENSOR_LOCATION_REAR_DROP								10 // Rear Dropout
#define BLE_CPS_SENSOR_LOCATION_CHAINSTAY								11 
#define BLE_CPS_SENSOR_LOCATION_REAR_WHEEL							12 
#define BLE_CPS_SENSOR_LOCATION_REAR_HUB								13 
#define BLE_CPS_SENSOR_LOCATION_CHEST										14 

// Cycling Power Feature bits
#define BLE_CPS_FEATURE_PEDAL_POWER_BALANCE_BIT         (0x01 << 0)		// Pedal Power Balance Supported
#define BLE_CPS_FEATURE_ACCUMULATED_TORQUE_BIT          (0x01 << 1)		// Accumulated Torque Supported
#define BLE_CPS_FEATURE_WHEEL_REV_BIT										(0x01 << 2)		// Wheel Revolution Data Supported
#define BLE_CPS_FEATURE_CRANK_REV_BIT										(0x01 << 3)		// Crank Revolution Data Supported
#define BLE_CPS_FEATURE_EXTREME_MAGNITUDES_BIT          (0x01 << 4)		// Extreme Magnitudes Supported
#define BLE_CPS_FEATURE_EXTREME_ANGLES_BIT							(0x01 << 5)		// Extreme Angles Supported
#define BLE_CPS_FEATURE_DEAD_SPOT_ANGLES_BIT						(0x01 << 6)		// Top and Bottom Dead Spot Angles Supported
#define BLE_CPS_FEATURE_ACCUM_ENERGY_BIT								(0x01 << 7)		// Accumulated Energy Supported
#define BLE_CPS_FEATURE_OFFSET_COMP_INDICATOR_BIT       (0x01 << 8)		// Offset Compensation Indicator Supported
#define BLE_CPS_FEATURE_OFFSET_COMP_BIT									(0x01 << 9)		// Offset Compensation Supported
#define BLE_CPS_FEATURE_POWER_MASK_BIT									(0x01 << 10)	// Cycling Power Measurement Characteristic Content Masking Supported
#define BLE_CPS_FEATURE_MULTIPLE_SENSORS_BIT						(0x01 << 11)	// Multiple Sensor Locations Supported
#define BLE_CPS_FEATURE_CRANK_LEN_ADJUST_BIT						(0x01 << 12)	// Crank Length Adjustment Supported
#define BLE_CPS_FEATURE_CHAIN_LEN_ADJUST_BIT						(0x01 << 13)	// Chain Length Adjustment Supported
#define BLE_CPS_FEATURE_CHAIN_WEIGHT_ADJUST_BIT         (0x01 << 14)	// Chain Weight Adjustment Supported
#define BLE_CPS_FEATURE_SPAN_LEN_ADJUST_BIT							(0x01 << 15)	// Span Length Adjustment Supported
#define BLE_CPS_FEATURE_SENSOR_MEAS_CONTEXT_BIT					(0x01 << 16)	// Sensor Measurement Context
#define BLE_CPS_FEATURE_INSTANT_MEAS_DIRECTION_BIT			(0x01 << 17)	// Instantaneous Measurement Direction Supported
#define BLE_CPS_FEATURE_FACTORY_CALIBRATION_BIT					(0x01 << 18)	// Factory Calibration Date Supported

typedef enum
{
	BLE_CPS_BIKE_ROAD				= 0x21,
	BLE_CPS_BIKE_MOUNT			= 0x2E
} ble_cps_bike_type_t;

//
// TODO: These values are not specific to BLE so they should be moved out to 
// the resistance module.
//
typedef enum 
{
	BLE_CPS_RESISTANCE_PERCENT 	= 0x40,
	BLE_CPS_RESISTANCE_LEVEL		= 0x41,
	BLE_CPS_RESISTANCE_ERG			= 0x42,
	BLE_CPS_SET_BIKE_TYPE				= 0x44,
	BLE_CPS_SET_SLOPE						= 0x46,
	BLE_CPS_SET_WIND						= 0x47
} ble_cps_resistance_mode_t;

/**@brief Cycling Power Service set resistance event. */
typedef struct
{
	ble_cps_resistance_mode_t resistance_mode;					/**< Data containing the resistance mode. */
	uint16_t* p_value;																	/**< Value to set the mode to. */
} ble_cps_rc_evt_t;

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
typedef void(*ble_cps_evt_handler_t) (ble_cps_t * p_cps, ble_cps_rc_evt_t * p_evt);

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
	ble_sc_ctrlpt_evt_handler_t  ctrlpt_evt_handler;                    							/**< Control point event handler */
	ble_srv_error_handler_t      error_handler;                         							/**< Function to be called in case of an error. */
	ble_srv_security_mode_t      cps_cpf_attr_md;                                     /**< Initial security level for cycling power feature attribute */
	ble_srv_cccd_security_mode_t cps_cpm_attr_md;                                     /**< Initial security level for cycling power measurement attribute */
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
	ble_sc_ctrlpt_t              ctrl_pt;                               							/**< Data for cycling power control point. */
	ble_gatts_char_handles_t     cpf_handles;                                         /**< Handles related to the Cycling Power Feature characteristic. */
	ble_gatts_char_handles_t     cpm_handles;                                         /**< Handles related to the Cycling Power Measurement characteristic. */
	ble_gatts_char_handles_t     sl_handles;                                          /**< Handles related to the Sensor Location characteristic. */
	ble_gatts_char_handles_t     cpcp_handles;                                        /**< Handles related to the Cycling Power Control Point characteristic. */
	ble_gatts_char_handles_t     cpv_handles;                                         /**< Handles related to the Cycling Power Vector characteristic. */
	ble_gatts_char_handles_t     cprc_handles;                                        /**< Handles related to the Cycling Power Resistance Control characteristic. */
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
uint32_t ble_cps_cycling_power_measurement_send(ble_cps_t * p_cps, ble_cps_meas_t * p_cps_meas);

#endif // BLE_CPS_H__

/** @} */
