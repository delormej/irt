// Wraps access to non-volatile storage.
#include <stdbool.h>
#include "user_profile.h"
#include "nrf_error.h"
#include "app_error.h"
#include "pstorage.h"
#include "debug.h"

#define USER_PROFILE_BLOCK_ID		0u
#define USER_PROFILE_OFFSET			0u

/**@brief Debug logging for main module.
 *
 */
#ifdef ENABLE_DEBUG_LOG
#define UP_LOG debug_log
#else
#define UP_LOG(...)
#endif // ENABLE_DEBUG_LOG

static pstorage_handle_t m_user_profile_storage;  // Internal pointer to persistent storage.

static void up_pstorage_cb_handler(pstorage_handle_t * handle,
                                   uint8_t             op_code,
                                   uint32_t            result,
                                   uint8_t           * p_data,
                                   uint32_t            data_len)
{
    if (result != NRF_SUCCESS)
    {
    	UP_LOG("[UP]:up_pstorage_cb_handler WARN: Error %lu\r\n", result);
    }
}


/**@brief Initializes access to storage. */
uint32_t user_profile_init(void)
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
    param.cb          = up_pstorage_cb_handler;

    err_code = pstorage_register(&param, &m_user_profile_storage);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

	return NRF_SUCCESS;
}

uint32_t user_profile_load(user_profile_t *p_user_profile)
{
    uint32_t          err_code;

    err_code = pstorage_load((uint8_t *)p_user_profile,
    						&m_user_profile_storage,
    						sizeof (user_profile_t),
    						USER_PROFILE_OFFSET);

    UP_LOG("[UP]:user_profile_load {weight:%i, wheel:%i, settings:%i, crr:%i}\r\n",
    		p_user_profile->total_weight_kg,
    		p_user_profile->wheel_size_mm,
    		p_user_profile->settings,
    		p_user_profile->calibrated_crr);

//TODO: Remove this, just for testing.
#ifdef SETTINGS
		// If there was a build override on settings.
    	p_user_profile->settings = SETTINGS;
#else
		//
    	// By default we'll get all 1's if nothing was previously stored.
    	//
		if (p_user_profile->settings == FEATURE_DEFAULT)
		{
			p_user_profile->settings = 0;
		}
#endif

    return err_code;
}

uint32_t user_profile_store(user_profile_t *p_user_profile)
{
    uint32_t err_code;

    // Seems like you have to clear pages before it will let you write to them.
    err_code = pstorage_clear(&m_user_profile_storage, sizeof (user_profile_t));
    if (err_code != NRF_SUCCESS)
    {
    	return err_code;
    }

    err_code = pstorage_store(&m_user_profile_storage,
                               (uint8_t *)p_user_profile,
                               sizeof (user_profile_t),
                               USER_PROFILE_OFFSET);

    UP_LOG("[UP]:user_profile_store {weight:%i, wheel:%i, settings:%i, crr:%i}\r\n",
    		p_user_profile->total_weight_kg,
    		p_user_profile->wheel_size_mm,
    		p_user_profile->settings,
    		p_user_profile->calibrated_crr);

	return err_code;
}
