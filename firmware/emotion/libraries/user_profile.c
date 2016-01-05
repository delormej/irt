// Wraps access to non-volatile storage.
#include <stdbool.h>
#include <string.h>
#include "user_profile.h"
#include "nordic_common.h"
#include "nrf_error.h"
#include "app_error.h"
#include "pstorage.h"
#include "app_scheduler.h"

#include "debug.h"

#define USER_PROFILE_BLOCK_ID		0u
#define USER_PROFILE_OFFSET			0u	

#define DEFAULT_WHEEL_SIZE_MM			2096ul										// Default wheel size.
#define DEFAULT_TOTAL_WEIGHT_KG			8180ul 										// Default weight (convert 180.0lbs to KG).
#define DEFAULT_SETTINGS				SETTING_INVALID								// Default 32bit field of settings.

/**@brief Debug logging for main module.
 *
 */
#ifdef ENABLE_DEBUG_LOG
#define UP_LOG debug_log
#else
#define UP_LOG(...)
#endif // ENABLE_DEBUG_LOG

static bool					m_profile_updating;         // Flag to track when the profile is process of being updated.
static pstorage_handle_t    m_user_profile_storage;  	// Internal pointer to persistent storage.
static user_profile_t       m_user_profile __attribute__ ((aligned (4))) = { .version = 0 }; // Force required 4-byte alignment of struct loaded from flash.           

// Type declarations for profile updating.
static void profile_update_sched_handler(void *p_event_data, uint16_t event_size);
static void profile_update_sched(void);
static void profile_update_pstorage_cb_handler(pstorage_handle_t *  p_handle,
                                  uint8_t              op_code,
                                  uint32_t             result,
                                  uint8_t *            p_data,
                                  uint32_t             data_len);

/**@brief Initializes access to storage. */
static uint32_t user_profile_storage_init(pstorage_ntf_cb_t cb)
{
    uint32_t err_code;
    pstorage_module_param_t param;

	// Initialize storage module.
	err_code = pstorage_init();
	if (err_code != NRF_SUCCESS)
	{
		return err_code;
	}

	// We're smaller than the min block size, so use that.
	param.block_size  = sizeof (user_profile_t);
    param.block_count = 1u;
    param.cb          = cb;

    err_code = pstorage_register(&param, &m_user_profile_storage);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

	return NRF_SUCCESS;
}

static uint32_t user_profile_load()
{
    uint32_t err_code;

    err_code = pstorage_load((uint8_t *)&m_user_profile,
    						&m_user_profile_storage,
    						sizeof (user_profile_t),
    						USER_PROFILE_OFFSET);

    UP_LOG("[UP]:user_profile_load {version: %i, weight:%i, wheel:%i, settings:%i, slope:%i, intercept:%i, servo_offset:%i}\r\n",
    		m_user_profile.version,
    		m_user_profile.total_weight_kg,
    		m_user_profile.wheel_size_mm,
    		m_user_profile.settings,
    		m_user_profile.ca_slope,
    		m_user_profile.ca_intercept,
    		m_user_profile.servo_offset);

    return err_code;
}

/**@brief	Initializes user profile and loads from flash.  Sets defaults for
 * 				simulation parameters if not set.
 */
static uint32_t user_profile_init()
{
    uint32_t err_code;

    m_profile_updating = false;

    err_code = user_profile_storage_init(profile_update_pstorage_cb_handler);
    APP_ERROR_CHECK(err_code);

    memset(&m_user_profile, 0, sizeof(m_user_profile));

    // Attempt to load stored profile from flash.
    err_code = user_profile_load();
    APP_ERROR_CHECK(err_code);

    // TODO: should all DEFAULT_x defines live in profile.h? The reason they don't today is that
    // certain settings don't live in the profile - so it makes sense to keep all the default DEFINES together

    //
    // Check the version of the profile, if it's not the current version
    // set the default parameters.
    //
    if (m_user_profile.version != PROFILE_VERSION)
    {
        UP_LOG("[MAIN]: Older profile version %i. Setting defaults where needed.\r\n",
                m_user_profile.version);

        m_user_profile.version = PROFILE_VERSION;

        // TODO: under what situation would these ever be 0?
        if (m_user_profile.wheel_size_mm == 0 ||
                m_user_profile.wheel_size_mm == 0xFFFF)
        {
            UP_LOG("[MAIN]:profile_init using default wheel circumference.\r\n");

            // Wheel circumference in mm.
            m_user_profile.wheel_size_mm = DEFAULT_WHEEL_SIZE_MM;
        }

        if (m_user_profile.total_weight_kg == 0 ||
                m_user_profile.total_weight_kg == 0xFFFF)
        {
            UP_LOG("[MAIN]:profile_init using default weight.\r\n");

            // Total weight of rider + bike + shoes, clothing, etc...
            m_user_profile.total_weight_kg = DEFAULT_TOTAL_WEIGHT_KG;
        }

        if (m_user_profile.servo_offset == -1) 	// -1 == (int16_t)0xFFFF
        {
            UP_LOG("[MAIN]:profile_init using default servo offset.\r\n");
            m_user_profile.servo_offset = 0; // default to 0 offset.
        }

        // Check for default servo positions.
        if (m_user_profile.servo_positions.count == 0xFF)
        {
            UP_LOG("[MAIN]: Setting default servo positions.\r\n");

            m_user_profile.servo_positions.count = 7;
            m_user_profile.servo_positions.positions[0] = 2000;
            m_user_profile.servo_positions.positions[1] = 1300;
            m_user_profile.servo_positions.positions[2] = 1200;
            m_user_profile.servo_positions.positions[3] = 1100;
            m_user_profile.servo_positions.positions[4] = 1000;
            m_user_profile.servo_positions.positions[5] = 900;
            m_user_profile.servo_positions.positions[6] = 800;
        }

        // Initialize default magnet calibration.
        if (m_user_profile.ca_mag_factors.low_speed_mps == 0xFFFF)
        {
            // Default to no gap offset.
            m_user_profile.ca_mag_factors.gap_offset = 0;				
            
            // 15 mph in meters per second * 1,000.
            m_user_profile.ca_mag_factors.low_speed_mps = 6705;

            // 25 mph in meters per second * 1,000.
            m_user_profile.ca_mag_factors.high_speed_mps = 11176;
            
            // Position at which we no longer use power curve, revert to linear.
            m_user_profile.ca_mag_factors.root_position = 1454;

            m_user_profile.ca_mag_factors.low_factors[0] = 1.27516039631e-06f;
            m_user_profile.ca_mag_factors.low_factors[1] = -0.00401345920329f;
            m_user_profile.ca_mag_factors.low_factors[2] = 3.58655403892f;
            m_user_profile.ca_mag_factors.low_factors[3] = -645.523540742f;

            m_user_profile.ca_mag_factors.high_factors[0] = 2.19872670294e-06f;
            m_user_profile.ca_mag_factors.high_factors[1] = -0.00686992504214f;
            m_user_profile.ca_mag_factors.high_factors[2] = 6.03431060782f;
            m_user_profile.ca_mag_factors.high_factors[3] = -998.115074474f;
        }

        // Schedule an update.
        profile_update_sched();
    }

    UP_LOG("[MAIN]:profile_init:\r\n\t weight: %i \r\n\t wheel: %i \r\n\t " \
        "settings: %lu \r\n\t ca_root_position: %i \r\n\t " \
        "ca_mag_factors.low: %.12f, %.12f, %.12f, %.12f\r\n\t " \
        "ca_mag_factors.high: %.12f, %.12f, %.12f, %.12f\r\n",
            m_user_profile.total_weight_kg,
            m_user_profile.wheel_size_mm,
            m_user_profile.settings,
            m_user_profile.ca_mag_factors.root_position,
            m_user_profile.ca_mag_factors.low_factors[0],
            m_user_profile.ca_mag_factors.low_factors[1],
            m_user_profile.ca_mag_factors.low_factors[2],
            m_user_profile.ca_mag_factors.low_factors[3],			
            m_user_profile.ca_mag_factors.high_factors[0],
            m_user_profile.ca_mag_factors.high_factors[1],
            m_user_profile.ca_mag_factors.high_factors[2],
            m_user_profile.ca_mag_factors.high_factors[3]
            );
            
     return err_code;       
}

/**@brief 	Callback function used by pstorage which reports result of trying to store
 * 			data to flash.  Main controls event flow, so handles this even here.
 *
 */
static void profile_update_pstorage_cb_handler(pstorage_handle_t * handle,
											   uint8_t             op_code,
											   uint32_t            result,
											   uint8_t           * p_data,
											   uint32_t            data_len)
{
	static uint8_t retry = 0;

	// Regardless of success the operation is done.
	m_profile_updating = false;

    if (result != NRF_SUCCESS)
    {
    	UP_LOG("[MAIN]:up_pstorage_cb_handler WARN: Error %lu\r\n", result);

    	if (retry++ < 20)
    	{
			// Try again.
			profile_update_sched();
    	}
    	else
    	{
    		UP_LOG("MAIN:up_pstorage_cb_handler aborting save attempt after %i retries.\r\n", retry);
    	}
    }
}

/**@brief Function for handling profile update.
 *
 * @details This function will be called by the scheduler to update the profile.
 * 			Writing to flash causes the radio to stop, so we don't want to do this
 * 			too often, so we let the scheduler decide when.
 */
static void profile_update_sched_handler(void *p_event_data, uint16_t event_size)
{
	UNUSED_PARAMETER(p_event_data);
	UNUSED_PARAMETER(event_size);

	uint32_t err_code;

	if (m_profile_updating)
	{
		// Throw it back on the queue until we're not currently writing.
		profile_update_sched();

		UP_LOG("[MAIN]:profile_update write already in progress, re-queuing.\r\n");
		return;
	}

	m_profile_updating = true;

	// This method ensures the device is in a proper state in order to update
	// the profile.
	err_code = user_profile_store();
	APP_ERROR_CHECK(err_code);

	UP_LOG("[MAIN]:profile_update:\r\n\t weight: %i \r\n\t wheel: %i \r\n\t settings: %lu \r\n\t slope: %i \r\n\t intercept: %i \r\n",
			m_user_profile.total_weight_kg,
			m_user_profile.wheel_size_mm,
			m_user_profile.settings,
			m_user_profile.ca_slope,
			m_user_profile.ca_intercept);
}

/**@brief   Schedules an update to the profile.  See notes above in handler.
 *
 */
static void profile_update_sched(void)
{
	uint32_t err_code;

	err_code = app_sched_event_put(NULL, 0, profile_update_sched_handler);
	APP_ERROR_CHECK(err_code);
}

/**@brief   Returns a pointer to the user profile object.
            If first access, will initialize and read persistent storage. 
 */
user_profile_t* user_profile_get()
{
    uint32_t err_code;

    // If this is the first load, initialize.
    if (m_user_profile.version == 0)
    {
        err_code = user_profile_init();
        APP_ERROR_CHECK(err_code);
    }
    
    return &m_user_profile;
}

/**@brief   Stores user profile in persistent storage on the device. 
 */
uint32_t user_profile_store()
{
    uint32_t err_code;

    // Seems like you have to clear pages before it will let you write to them.
    err_code = pstorage_clear(&m_user_profile_storage, sizeof (user_profile_t));
    if (err_code != NRF_SUCCESS)
    {
    	return err_code;
    }

    err_code = pstorage_store(&m_user_profile_storage,
                               (uint8_t *)&m_user_profile,
                               sizeof (user_profile_t),
                               USER_PROFILE_OFFSET);

    UP_LOG("[UP]:user_profile_store {version: %i, weight:%i, wheel:%i, settings:%i, slope:%i, intercept:%i, servo_offset:%i, addr:%lu}\r\n",
    		m_user_profile.version,
    		m_user_profile.total_weight_kg,
    		m_user_profile.wheel_size_mm,
    		m_user_profile.settings,
    		m_user_profile.ca_slope,
    		m_user_profile.ca_intercept,
    		m_user_profile.servo_offset,
    		m_user_profile);

	return err_code;
}
