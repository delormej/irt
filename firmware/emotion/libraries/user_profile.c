// Wraps access to non-volatile storage.
#include <stdbool.h>
#include "user_profile.h"
#include "nrf_error.h"
#include "pstorage.h"

#define USER_PROFILE_BLOCK_ID		1u
#define USER_PROFILE_OFFSET			0u

static bool mb_initialized;
static user_profile_t *mp_user_profile;  // Internal pointer to persistent storage.

static void up_pstorage_cb_handler(pstorage_handle_t * handle,
                                   uint8_t             op_code,
                                   uint32_t            result,
                                   uint8_t           * p_data,
                                   uint32_t            data_len)
{
    if (result != NRF_SUCCESS)
    {
        // TODO: assert some sort of error here.
    }
}


/**@brief Initializes access to storage. */
static uint32_t user_profile_init(void)
{
    pstorage_module_param_t param;
    uint32_t err_code;

	// if already inialized, just return.
	if (mb_initialized)
		return NRF_SUCCESS;

    param.block_size  = sizeof (user_profile_t) + sizeof (uint32_t);
    param.block_count = 1u;
    param.cb          = up_pstorage_cb_handler;

    err_code = pstorage_register(&param, &mp_user_profile);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    mb_initialized = true;

	return err_code;
}

uint32_t user_profile_load(user_profile_t *p_user_profile)
{
    uint32_t          err_code;
    pstorage_handle_t source_block;

    err_code = user_profile_init();
	if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code = pstorage_block_identifier_get(&mp_user_profile,
                                             1,
                                             &source_block);
	if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code = pstorage_load((uint8_t *)&p_user_profile,
    						&source_block,
    						sizeof(user_profile_t),
    						USER_PROFILE_OFFSET);

    return err_code;
}

uint32_t user_profile_store(user_profile_t *p_user_profile)
{
    uint32_t          err_code;
    pstorage_handle_t dest_block;

    err_code = user_profile_init();
	if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code = pstorage_block_identifier_get(&mp_user_profile,
                                             1,
                                             &dest_block);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code = pstorage_store(&dest_block,
                               (uint8_t *)p_user_profile,
                               sizeof(user_profile_t),
                               USER_PROFILE_OFFSET);

	return err_code;
}
