#include "ble_cps.h"
#include <string.h>
#include "nordic_common.h"
#include "ble_l2cap.h"
#include "ble_srv_common.h"
#include "app_util.h"

// https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicsHome.aspx
#define BLE_UUID_SENSOR_LOCATION_CHAR										0x2A5D 		/**< Sensor Location UUID.  */
#define BLE_UUID_CYCLING_POWER_FEATURE_CHAR							0x2A65 		/**< Cycling Power Feature UUID. */
#define BLE_UUID_CYCLING_POWER_MEASUREMENT_CHAR					0x2A63		/**< Cycling Power Measurement UUID. */

// Cycling Power Measurement flag bits: https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.cycling_power_measurement.xml
#define CPS_MEAS_FLAG_PEDAL_POWER_PRESENT        				(0x01 << 0)		
#define CPS_MEAS_FLAG_PEDAL_POWER_REFERENCE        			(0x01 << 1) 
#define CPS_MEAS_FLAG_ACCUM_TORQUE_PRESENT							(0x01 << 2)
#define CPS_MEAS_FLAG_ACCUM_TORQUE_SOURCE								(0x01 << 3)
#define CPS_MEAS_FLAG_WHEEL_REV_PRESENT									(0x01 << 4)
#define CPS_MEAS_FLAG_CRANK_REV_PRESENT									(0x01 << 5)
#define CPS_MEAS_FLAG_EXTREME_FORCE_PRESENT							(0x01 << 6)
#define CPS_MEAS_FLAG_EXTREME_TORQUE_PRESENT						(0x01 << 7)
#define CPS_MEAS_FLAG_EXTREME_ANGLES_PRESENT						(0x01 << 8)
#define CPS_MEAS_FLAG_TOP_DEAD_SPOT_PRESENT							(0x01 << 9)
#define CPS_MEAS_FLAG_BOTTOM_DEAD_SPOT_PRESENT					(0x01 << 10)
#define CPS_MEAS_FLAG_ACCUM_ENERGY_PRESENT							(0x01 << 11)
#define CPS_MEAS_FLAG_OFFSET_COMP_INDICATOR							(0x01 << 12)

#define MAX_CPM_LEN																			34	// This is unscientific? I'm just added up all the bytes of all possible fields in the structure.

/**@brief Function for encoding a Cycling Power Measurement.
 *
 * @param[in]   p_cps              Cycling Power Service structure.
 * @param[in]   p_cps_measurement  Measurement to be encoded.
 * @param[out]  p_encoded_buffer   Buffer where the encoded data will be written.
 *
 * @return      Size of encoded data.
 */
static uint8_t cps_measurement_encode(ble_cps_t *      p_cps,
                                      ble_cps_meas_t * p_cps_measurement,
                                      uint8_t *        p_encoded_buffer)
{
    uint8_t flags = 0;
    uint8_t len   = 1;

		// TODO: could this encoding be a problem? The actual value should be a *signed* int16.
		// length should be the same, but we don't care about the most significant bit do we?
		// Instantaneous power field
		len += uint16_encode(p_cps_measurement->instant_power, &p_encoded_buffer[len]);

		// NOTE: Skipping Pedal Power Balance, NOT USED
		
		// Accumulated torque field
		if (p_cps->feature & BLE_CPS_FEATURE_ACCUMULATED_TORQUE_BIT)
		{
			if (p_cps_measurement->accum_torque != NULL)
			{
				flags |= CPS_MEAS_FLAG_ACCUM_TORQUE_PRESENT;
				len += uint16_encode(p_cps_measurement->accum_torque, &p_encoded_buffer[len]);
			}
		}

		// Wheel revolution data fields (sent as a pair)
		if (p_cps->feature & BLE_CPS_FEATURE_WHEEL_REV_BIT)
		{
			if (p_cps_measurement->accum_wheel_revs != NULL)
			{
				flags |= CPS_MEAS_FLAG_WHEEL_REV_PRESENT;
				len += uint32_encode(p_cps_measurement->accum_wheel_revs, &p_encoded_buffer[len]);
				len += uint16_encode(p_cps_measurement->last_wheel_event_time, &p_encoded_buffer[len]);
			}
		}
		
		// NOTE: Skipping Crank revolution, NOT USED.
		// NOTE: Skipping Extreme force, torque and angles, top and bottom dead spot, NONE USED.
		
		if (p_cps->feature & BLE_CPS_FEATURE_ACCUM_ENERGY_BIT)
		{
			if (p_cps_measurement->accum_energy != NULL)
			{
				flags |= CPS_MEAS_FLAG_ACCUM_ENERGY_PRESENT;
				len += uint16_encode(p_cps_measurement->accum_energy, &p_encoded_buffer[len]);
			}
		}
		    
    // Flags field
    p_encoded_buffer[0] = flags;
    
    return len;
}

static uint32_t cycling_power_feature_char_add(ble_cps_t * p_cps, const ble_cps_init_t * p_cps_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    
    memset(&char_md, 0, sizeof(char_md));
    
    char_md.char_props.read  = 1;
    char_md.p_char_user_desc = NULL;
    char_md.p_char_pf        = NULL;
    char_md.p_user_desc_md   = NULL;
    char_md.p_cccd_md        = NULL;
    char_md.p_sccd_md        = NULL;
    
    BLE_UUID_BLE_ASSIGN(ble_uuid, BLE_UUID_CYCLING_POWER_FEATURE_CHAR);
    
    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_cps_init->cps_cpf_attr_md.read_perm;
    attr_md.write_perm = p_cps_init->cps_cpf_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;
		
    memset(&attr_char_value, 0, sizeof(attr_char_value));
    
    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = 0; // FIX
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = 0; // FIX
    attr_char_value.p_value      = 0;//p_cps_init->p_cps_features;
    
    return sd_ble_gatts_characteristic_add(p_cps->service_handle,
                                           &char_md,
                                           &attr_char_value,
                                           &p_cps->cpf_handles);		
}

static uint32_t cycling_power_measurement_char_add(ble_cps_t * p_cps, const ble_cps_init_t * p_cps_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;		
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
		ble_cps_meas_t			initial_cpm;
		uint8_t							encoded_cpm[MAX_CPM_LEN];
    
    memset(&cccd_md, 0, sizeof(cccd_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
		BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc = BLE_GATTS_VLOC_STACK;
		
    memset(&char_md, 0, sizeof(char_md));
    
    char_md.char_props.notify = 1;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = &cccd_md;
    char_md.p_sccd_md         = NULL;
    
    BLE_UUID_BLE_ASSIGN(ble_uuid, BLE_UUID_CYCLING_POWER_MEASUREMENT_CHAR);
    
    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_cps_init->cps_cpm_attr_md.read_perm;
    attr_md.write_perm = p_cps_init->cps_cpm_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 1;
    
    memset(&attr_char_value, 0, sizeof(attr_char_value));
    
    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = cps_measurement_encode(p_cps, &initial_cpm, encoded_cpm);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = MAX_CPM_LEN;
    attr_char_value.p_value      = encoded_cpm;
    
    return sd_ble_gatts_characteristic_add(p_cps->service_handle,
                                           &char_md,
                                           &attr_char_value,
                                           &p_cps->cpm_handles);	
}

static uint32_t sensor_location_char_add(ble_cps_t * p_cps, const ble_cps_init_t * p_cps_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    
    memset(&char_md, 0, sizeof(char_md));
    
    char_md.char_props.read  = 1;
    char_md.p_char_user_desc = NULL;
    char_md.p_char_pf        = NULL;
    char_md.p_user_desc_md   = NULL;
    char_md.p_cccd_md        = NULL;
    char_md.p_sccd_md        = NULL;
    
    BLE_UUID_BLE_ASSIGN(ble_uuid, BLE_UUID_SENSOR_LOCATION_CHAR);
    
    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_cps_init->cps_sl_attr_md.read_perm;
    attr_md.write_perm = p_cps_init->cps_sl_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;
    
    memset(&attr_char_value, 0, sizeof(attr_char_value));
    
    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(uint8_t);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(uint8_t);
    attr_char_value.p_value      = p_cps_init->p_sensor_location;
    
    return sd_ble_gatts_characteristic_add(p_cps->service_handle,
                                           &char_md,
                                           &attr_char_value,
                                           &p_cps->sl_handles);	
}

static uint32_t cycling_power_control_point_char_add(ble_cps_t * p_cps, const ble_cps_init_t * p_cps_init)
{
	return 0;
}

static uint32_t cycling_power_vector_char_add(ble_cps_t * p_cps, const ble_cps_init_t * p_cps_init)
{
	return 0;
}


uint32_t ble_cps_init(ble_cps_t * p_cps, const ble_cps_init_t * p_cps_init)
{
    uint32_t   err_code;
    ble_uuid_t ble_uuid;

    // Initialize service structure
    p_cps->evt_handler                 = p_cps_init->evt_handler;
    p_cps->conn_handle                 = BLE_CONN_HANDLE_INVALID;
    
    // Add service
    BLE_UUID_BLE_ASSIGN(ble_uuid, BLE_UUID_CYCLING_POWER_SERVICE);

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_cps->service_handle);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Add cycling power feature characteristic
    err_code = cycling_power_feature_char_add(p_cps, p_cps_init);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Add cycling power measurement characteristic
    err_code = cycling_power_measurement_char_add(p_cps, p_cps_init);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Add sensor location characteristic
    err_code = sensor_location_char_add(p_cps, p_cps_init);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    if (p_cps_init->use_cycling_power_control_point)
    {
        // Add cycling power control point characteristic
        err_code = cycling_power_control_point_char_add(p_cps, p_cps_init);
        if (err_code != NRF_SUCCESS)
        {
            return err_code;
        }
    }
    
    if (p_cps_init->use_cycling_power_vector)
    {
        // Add cycling power control point characteristic
        err_code = cycling_power_vector_char_add(p_cps, p_cps_init);
        if (err_code != NRF_SUCCESS)
        {
            return err_code;
        }
    }
		
    return NRF_SUCCESS;	
}


