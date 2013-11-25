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

#ifndef BLE_CPS_H__
#define BLE_CPS_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"

// Sensor Location values (unit8)
#define BLE_CPS_SENSOR_LOCATION_OTHER		0
#define BLE_CPS_SENSOR_LOCATION_TOP_SHOE	1 // Top of shoe
#define BLE_CPS_SENSOR_LOCATION_IN_SHOE		2 // In shoe
#define BLE_CPS_SENSOR_LOCATION_HIP			3
#define BLE_CPS_SENSOR_LOCATION_FRONT_WHEEL	4 
#define BLE_CPS_SENSOR_LOCATION_LEFT_CRANK	5 
#define BLE_CPS_SENSOR_LOCATION_RIGHT_CRANK	6 
#define BLE_CPS_SENSOR_LOCATION_LEFT_PEDAL	7 
#define BLE_CPS_SENSOR_LOCATION_RIGHT_PEDAL	8 
#define BLE_CPS_SENSOR_LOCATION_FRONT_HUB	9 
#define BLE_CPS_SENSOR_LOCATION_REAR_DROP	10 // Rear Dropout
#define BLE_CPS_SENSOR_LOCATION_CHAINSTAY	11 
#define BLE_CPS_SENSOR_LOCATION_REAR_WHEEL	12 
#define BLE_CPS_SENSOR_LOCATION_REAR_HUB	13 
#define BLE_CPS_SENSOR_LOCATION_CHEST		14 

// Cycling Power Feature bits
#define BLE_CPS_FEATURE_PEDAL_POWER_BALANCE_BIT         (0x01 << 0)		// Pedal Power Balance Supported
#define BLE_CPS_FEATURE_ACCUMULATED_TORQUE_BIT          (0x01 << 1)		// Accumulated Torque Supported
#define BLE_CPS_FEATURE_WHEEL_REV_BIT					(0x01 << 2)		// Wheel Revolution Data Supported
#define BLE_CPS_FEATURE_CRANK_REV_BIT					(0x01 << 3)		// Crank Revolution Data Supported
#define BLE_CPS_FEATURE_EXTREME_MAGNITUDES_BIT          (0x01 << 4)		// Extreme Magnitudes Supported
#define BLE_CPS_FEATURE_EXTREME_ANGLES_BIT				(0x01 << 5)		// Extreme Angles Supported
#define BLE_CPS_FEATURE_DEAD_SPOT_ANGLES_BIT			(0x01 << 6)		// Top and Bottom Dead Spot Angles Supported
#define BLE_CPS_FEATURE_ACCUM_ENERGY_BIT				(0x01 << 7)		// Accumulated Energy Supported
#define BLE_CPS_FEATURE_OFFSET_COMP_INDICATOR_BIT       (0x01 << 8)		// Offset Compensation Indicator Supported
#define BLE_CPS_FEATURE_OFFSET_COMP_BIT					(0x01 << 9)		// Offset Compensation Supported
#define BLE_CPS_FEATURE_POWER_MASK_BIT					(0x01 << 10)	// Cycling Power Measurement Characteristic Content Masking Supported
#define BLE_CPS_FEATURE_MULTIPLE_SENSORS_BIT			(0x01 << 11)	// Multiple Sensor Locations Supported
#define BLE_CPS_FEATURE_CRANK_LEN_ADJUST_BIT			(0x01 << 12)	// Crank Length Adjustment Supported
#define BLE_CPS_FEATURE_CHAIN_LEN_ADJUST_BIT			(0x01 << 13)	// Chain Length Adjustment Supported
#define BLE_CPS_FEATURE_CHAIN_WEIGHT_ADJUST_BIT         (0x01 << 14)	// Chain Weight Adjustment Supported
#define BLE_CPS_FEATURE_SPAN_LEN_ADJUST_BIT				(0x01 << 15)	// Span Length Adjustment Supported
#define BLE_CPS_FEATURE_SENSOR_MEAS_CONTEXT_BIT			(0x01 << 16)	// Sensor Measurement Context
#define BLE_CPS_FEATURE_INSTANT_MEAS_DIRECTION_BIT		(0x01 << 17)	// Instantaneous Measurement Direction Supported
#define BLE_CPS_FEATURE_FACTORY_CALIBRATION_BIT			(0x01 << 18)	// Factory Calibration Date Supported

/**@brief Cycling Power Service measurement type. */
typedef struct ble_cps_meas_s
{
	uint16_t    flags;									// 16 bits defined here: https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.cycling_power_measurement.xml
	int16_t		instant_power;                          // Note this is a SIGNED int16
	uint8_t		pedal_power_balance;
	uint16_t	accum_torque;							// Unit is in newton metres with a resolution of 1/32
	uint32_t	accum_wheel_revs;
	uint16_t	last_wheel_event_time;					// Unit is in seconds with a resolution of 1/2048. 
	uint16_t	accum_crank_rev;
	uint16_t	last_crank_event_time;					// Unit is in seconds with a resolution of 1/1024. 
	int16_t		max_force_magnitude;					// Unit is in newtons with a resolution of 1.
	int16_t		min_force_magnitude;					// Unit is in newtons with a resolution of 1.
	int16_t		max_torque_magnitude;					// Unit is in newton metres with a resolution of 1/32
	int16_t		min_torque_magnitude;					// Unit is in newton metres with a resolution of 1/32
	uint12_t	max_angle;								// Unit is in degrees with a resolution of 1. 
	uint12_t	min_angle;								// Unit is in degrees with a resolution of 1. 
	uint16_t	top_dead_spot_angle;					// Unit is in degrees with a resolution of 1. 
	uint16_t	bottom_dead_spot_angle;					// Unit is in degrees with a resolution of 1. 
	uint16_t	accum_energy;							// Unit is in kilojoules with a resolution of 1.
} ble_cps_meas_t;


// Forward declaration of the ble_hrs_t type. 
typedef struct ble_hrs_s ble_hrs_t;

/**@brief Heart Rate Service event handler type. */
typedef void(*ble_hrs_evt_handler_t) (ble_hrs_t * p_hrs, ble_hrs_evt_t * p_evt);

/**@brief Heart Rate Service init structure. This contains all options and data needed for
*        initialization of the service. */
typedef struct
{
	ble_hrs_evt_handler_t        evt_handler;                                          /**< Event handler to be called for handling events in the Heart Rate Service. */
	bool                         is_sensor_contact_supported;                          /**< Determines if sensor contact detection is to be supported. */
	uint8_t *                    p_body_sensor_location;                               /**< If not NULL, initial value of the Body Sensor Location characteristic. */
	ble_srv_cccd_security_mode_t hrs_hrm_attr_md;                                      /**< Initial security level for heart rate service measurement attribute */
	ble_srv_security_mode_t      hrs_bsl_attr_md;                                      /**< Initial security level for body sensor location attribute */
} ble_hrs_init_t;

/**@brief Heart Rate Service structure. This contains various status information for the service. */
typedef struct ble_hrs_s
{
	ble_hrs_evt_handler_t        evt_handler;                                          /**< Event handler to be called for handling events in the Heart Rate Service. */
	bool                         is_expended_energy_supported;                         /**< TRUE if Expended Energy measurement is supported. */
	bool                         is_sensor_contact_supported;                          /**< TRUE if sensor contact detection is supported. */
	uint16_t                     service_handle;                                       /**< Handle of Heart Rate Service (as provided by the BLE stack). */
	ble_gatts_char_handles_t     hrm_handles;                                          /**< Handles related to the Heart Rate Measurement characteristic. */
	ble_gatts_char_handles_t     bsl_handles;                                          /**< Handles related to the Body Sensor Location characteristic. */
	ble_gatts_char_handles_t     hrcp_handles;                                         /**< Handles related to the Heart Rate Control Point characteristic. */
	uint16_t                     conn_handle;                                          /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
	bool                         is_sensor_contact_detected;                           /**< TRUE if sensor contact has been detected. */
	uint16_t                     rr_interval[BLE_HRS_MAX_BUFFERED_RR_INTERVALS];       /**< Set of RR Interval measurements since the last Heart Rate Measurement transmission. */
	uint16_t                     rr_interval_count;                                    /**< Number of RR Interval measurements since the last Heart Rate Measurement transmission. */
} ble_hrs_t;

/**@brief Function for initializing the Heart Rate Service.
*
* @param[out]  p_hrs       Heart Rate Service structure. This structure will have to be supplied by
*                          the application. It will be initialized by this function, and will later
*                          be used to identify this particular service instance.
* @param[in]   p_hrs_init  Information needed to initialize the service.
*
* @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
*/
uint32_t ble_cps_init(ble_hrs_t * p_hrs, const ble_hrs_init_t * p_hrs_init);

/**@brief Function for handling the Application's BLE Stack events.
*
* @details Handles all events from the BLE stack of interest to the Heart Rate Service.
*
* @param[in]   p_hrs      Heart Rate Service structure.
* @param[in]   p_ble_evt  Event received from the BLE stack.
*/
void ble_cps_on_ble_evt(ble_hrs_t * p_hrs, ble_evt_t * p_ble_evt);

/**@brief Function for sending heart rate measurement if notification has been enabled.
*
* @details The application calls this function after having performed a heart rate measurement.
*          If notification has been enabled, the heart rate measurement data is encoded and sent to
*          the client.
*
* @param[in]   p_hrs                    Heart Rate Service structure.
* @param[in]   heart_rate               New heart rate measurement.
* @param[in]   include_expended_energy  Determines if expended energy will be included in the
*                                       heart rate measurement data.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
uint32_t ble_cps_cycling_power_measurement_send(ble_hrs_t * p_hrs, uint16_t heart_rate);

/**@brief Function for adding a RR Interval measurement to the RR Interval buffer.
*
* @details All buffered RR Interval measurements will be included in the next heart rate
*          measurement message, up to the maximum number of measurements that will fit into the
*          message. If the buffer is full, the oldest measurement in the buffer will be deleted.
*
* @param[in]   p_hrs        Heart Rate Service structure.
* @param[in]   rr_interval  New RR Interval measurement (will be buffered until the next
*                           transmission of Heart Rate Measurement).
*/
void ble_hrs_rr_interval_add(ble_hrs_t * p_hrs, uint16_t rr_interval);

/**@brief Function for checking if RR Interval buffer is full.
*
* @param[in]   p_hrs        Heart Rate Service structure.
*
* @return      true if RR Interval buffer is full, false otherwise.
*/
bool ble_hrs_rr_interval_buffer_is_full(ble_hrs_t * p_hrs);

/**@brief Function for setting the state of the Sensor Contact Supported bit.
*
* @param[in]   p_hrs                        Heart Rate Service structure.
* @param[in]   is_sensor_contact_supported  New state of the Sensor Contact Supported bit.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
uint32_t ble_hrs_sensor_contact_supported_set(ble_hrs_t * p_hrs, bool is_sensor_contact_supported);

/**@brief Function for setting the state of the Sensor Contact Detected bit.
*
* @param[in]   p_hrs                        Heart Rate Service structure.
* @param[in]   is_sensor_contact_detected   TRUE if sensor contact is detected, FALSE otherwise.
*/
void ble_hrs_sensor_contact_detected_update(ble_hrs_t * p_hrs, bool is_sensor_contact_detected);

/**@brief Function for setting the Body Sensor Location.
*
* @details Sets a new value of the Body Sensor Location characteristic. The new value will be sent
*          to the client the next time the client reads the Body Sensor Location characteristic.
*
* @param[in]   p_hrs                 Heart Rate Service structure.
* @param[in]   body_sensor_location  New Body Sensor Location.
*
* @return      NRF_SUCCESS on success, otherwise an error code.
*/
uint32_t ble_hrs_body_sensor_location_set(ble_hrs_t * p_hrs, uint8_t body_sensor_location);

#endif // BLE_CPS_H__

/** @} */
