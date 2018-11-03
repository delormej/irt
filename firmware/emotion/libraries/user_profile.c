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

#define DEFAULT_WHEEL_SIZE_MM			        2096ul										// Default wheel size.
#define DEFAULT_TOTAL_WEIGHT_KG			        8500ul 										// Default weight 85kg (75kg user, 10kg bike) as per FE-C spec.
#define DEFAULT_SETTINGS				        SETTING_INVALID								// Default 32bit field of settings.
#define DEFAULT_RESISTANCE_MIN_SPEED_ADJUST		40u				// 40/10 = 4mps (~9mph) Minimum speed in meters per second at which we adjust resistance.

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

static void profile_dump_to_log()
{
    UP_LOG(
        "\tservo_offset: %i \r\n" \
        "\ttotal_weight_kg: %i \r\n" \
        "\tuser_weight_kg: %i \r\n" \
        "\twheel: %i \r\n " \
        "\tsettings: %lu \r\n " \
        "\tdrag: %.3f, rr: %.3f \r\n" \ 
        "\tmag gap offset: %i\r\n" \
        "\tpower_meter_id: %i\r\n" \
        "\tpower_adjust_seconds: %i, power_average_seconds: %i\r\n" \
        "\tservo_smoothing_steps: %i\r\n" \
        "\tmin resistance adjust speed: %i\r\n",
        m_user_profile.servo_offset,
        m_user_profile.total_weight_kg,
        m_user_profile.user_weight_kg,
        m_user_profile.wheel_size_mm,
        m_user_profile.settings,
        m_user_profile.ca_drag,
        m_user_profile.ca_rr,
        m_user_profile.ca_mag_factors.gap_offset,
        m_user_profile.power_meter_ant_id,
        m_user_profile.power_adjust_seconds,
        m_user_profile.power_average_seconds,
        m_user_profile.servo_smoothing_steps,
        m_user_profile.min_adjust_speed_mps
    );    
}

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

    UP_LOG("[UP]:user_profile_load {version: %i}\r\n", m_user_profile.version);
    profile_dump_to_log();

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
        UP_LOG("[UP]: Older profile version %i. Setting defaults where needed.\r\n",
                m_user_profile.version);

        m_user_profile.version = PROFILE_VERSION;

        // TODO: under what situation would these ever be 0?
        if (m_user_profile.wheel_size_mm == 0 ||
                m_user_profile.wheel_size_mm == 0xFFFF)
        {
            UP_LOG("[UP]:profile_init using default wheel circumference.\r\n");

            // Wheel circumference in mm.
            m_user_profile.wheel_size_mm = DEFAULT_WHEEL_SIZE_MM;
        }

        if (m_user_profile.total_weight_kg == 0 ||
                m_user_profile.total_weight_kg == 0xFFFF)
        {
            UP_LOG("[UP]:profile_init using default weight.\r\n");

            // Total weight of rider + bike + shoes, clothing, etc...
            m_user_profile.total_weight_kg = DEFAULT_TOTAL_WEIGHT_KG;
        }

        if (m_user_profile.servo_offset == -1) 	// -1 == (int16_t)0xFFFF
        {
            UP_LOG("[UP]:profile_init using default servo offset.\r\n");
            m_user_profile.servo_offset = 0; // default to 0 offset.
        }

        // Check for default servo positions.
        if (m_user_profile.servo_positions.count == 0xFF)
        {
            UP_LOG("[UP]: Setting default servo positions.\r\n");

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
            m_user_profile.ca_mag_factors = magnet_get_default_factors();
        }

        // By default don't try to connect to a power meter.
        /*if (m_user_profile.power_meter_ant_id == 0xFFFF) {
            m_user_profile.power_meter_ant_id = 0;
        }*/

        if (m_user_profile.power_adjust_seconds == 0xFF)
        {
            // In erg/sim mode, attempt to adjust power every n seconds. 
            m_user_profile.power_adjust_seconds = DEFAULT_POWER_ADJUST_SECONDS;
        }

        if (m_user_profile.power_average_seconds == 0xFF)
        {
            // In erg/sim mode, calculate average power every n seconds. 
            m_user_profile.power_average_seconds = DEFAULT_POWER_AVERAGE_SECONDS;
        }

		if (m_user_profile.servo_smoothing_steps == 0xFF)
		{
			// Default steps to move every 20ms when smoothing servo movement.
			m_user_profile.servo_smoothing_steps = DEFAULT_SMOOTHING_STEPS; 
		}

		if (m_user_profile.min_adjust_speed_mps == 0xFF)
		{
			m_user_profile.min_adjust_speed_mps = DEFAULT_RESISTANCE_MIN_SPEED_ADJUST; 
		}

        // Schedule an update.
        UP_LOG("[UP]:profile_init: scheduling update\r\n");
        profile_update_sched();
    }

    UP_LOG("[UP]:profile_init:\r\n");
    profile_dump_to_log();
            
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

    UP_LOG("[MAIN]:profile_update:\r\n");
    profile_dump_to_log();
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

    UP_LOG("[UP]:user_profile_store {version: %i}\r\n", m_user_profile.version);
    profile_dump_to_log();

	return err_code;
}
