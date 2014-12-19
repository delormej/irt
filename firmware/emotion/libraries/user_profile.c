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

static pstorage_handle_t m_user_profile_storage;  	// Internal pointer to persistent storage.

/**@brief Initializes access to storage. */
uint32_t user_profile_init(pstorage_ntf_cb_t cb)
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

uint32_t user_profile_load(user_profile_t *p_user_profile)
{
    uint32_t          err_code;

    err_code = pstorage_load((uint8_t *)p_user_profile,
    						&m_user_profile_storage,
    						sizeof (user_profile_t),
    						USER_PROFILE_OFFSET);

    UP_LOG("[UP]:user_profile_load {version: %i, weight:%i, wheel:%i, settings:%i, slope:%i, intercept:%i, servo_offset:%i}\r\n",
    		p_user_profile->version,
    		p_user_profile->total_weight_kg,
    		p_user_profile->wheel_size_mm,
    		p_user_profile->settings,
    		p_user_profile->ca_slope,
    		p_user_profile->ca_intercept,
    		p_user_profile->servo_offset);

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

    UP_LOG("[UP]:user_profile_store {version: %i, weight:%i, wheel:%i, settings:%i, slope:%i, intercept:%i, servo_offset:%i, addr:%lu}\r\n",
    		p_user_profile->version,
    		p_user_profile->total_weight_kg,
    		p_user_profile->wheel_size_mm,
    		p_user_profile->settings,
    		p_user_profile->ca_slope,
    		p_user_profile->ca_intercept,
    		p_user_profile->servo_offset,
    		p_user_profile);

	return err_code;
}
