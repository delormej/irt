#include "ble_cps.h"
#include <string.h>
#include "nordic_common.h"
#include "ble_l2cap.h"
#include "ble_srv_common.h"
#include "app_util.h"

static uint32_t cycling_power_feature_char_add(ble_cps_t * p_hrs, const ble_cps_init_t * p_hrs_init)
{
}

static uint32_t cycling_power_measurement_char_add(ble_cps_t * p_hrs, const ble_cps_init_t * p_hrs_init)
{
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

static uint32_t cycling_power_control_point_char_add(ble_cps_t * p_hrs, const ble_cps_init_t * p_hrs_init)
{
}

static uint32_t cycling_power_vector_char_add(ble_cps_t * p_hrs, const ble_cps_init_t * p_hrs_init)
{
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

