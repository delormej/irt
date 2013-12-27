/* Copyright (c) 2013 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/** @file
 *
 * @defgroup stack_handler Stack Event Handler
 * @{
 * @ingroup ble_sdk_lib
 * @brief Module for initializing the multiprotocol BLE + ANT stack, and propagating BLE and ANT
 *        stack events to the application.
 *
 * @details Use the USE_SCHEDULER parameter of the BLE_ANT_STACK_HANDLER_INIT() macro to select if
 *          the @ref app_scheduler is to be used or not.
 *
 * @note Even if the scheduler is not used, ble_stack_handler.h will include app_scheduler.h,
 *       so when compiling, app_scheduler.h must be available in one of the compiler include paths.
 */

#ifndef BLE_ANT_STACK_HANDLER_H__
#define BLE_ANT_STACK_HANDLER_H__

#include <stdlib.h>
#include "ble.h"
#include "nrf_sdm.h"
#include "app_error.h"
#include "app_scheduler.h"
#include "app_util.h"

#define BLE_ANT_STACK_HANDLER_SCHED_EVT_SIZE  0               /**< Size of button events being passed through the scheduler (is to be used for computing the maximum size of scheduler events). For stack events, this size is 0, since the events are being pulled in the event handler. */
#define BLE_ANT_STACK_ANT_EVENT_MSG_BUF_SIZE  32              /**< Size of ANT event message buffer. */

/**@brief Application ANT stack event type. */
typedef struct
{
    uint8_t channel;                                          /**< Channel number. */
    uint8_t event;                                            /**< Event code. */
    uint8_t evt_buffer[BLE_ANT_STACK_ANT_EVENT_MSG_BUF_SIZE]; /**< Event message buffer. */
} ant_evt_t;

/**@brief Application BLE stack event handler type. */
typedef void (*ble_ant_stack_ble_evt_handler_t) (ble_evt_t * p_ble_evt);

/**@brief Application ANT stack event handler type. */
typedef void (*ble_ant_stack_ant_evt_handler_t) (ant_evt_t * p_ant_evt);

/**@brief Type of function for passing events from the stack handler module to the scheduler. */
typedef uint32_t (*ble_ant_stack_evt_schedule_func_t) (void);

/**@brief Macro for initializing the stack event handler.
 *
 * @details It will handle dimensioning and allocation of the memory buffer required for reading
 *          events from the stack, making sure the buffer is correctly aligned. It will also
 *          connect the stack event handler to the scheduler (if specified).
 *
 * @param[in] CLOCK_SOURCE     Low frequency clock source and accuracy (type nrf_clock_lfclksrc_t,
 *                             see sd_softdevice_enable() for details).
 * @param[in] MTU_SIZE         Maximum size of BLE transmission units to be used by this
 *                             application.
 * @param[in] BLE_EVT_HANDLER  Pointer to function to be executed when a BLE  event has been
 *                             received.
 * @param[in] ANT_EVT_HANDLER  Pointer to function to be executed when an ANT event has been
 *                             received.
 * @param[in] USE_SCHEDULER    TRUE if the application is using the event scheduler, FALSE
 *                             otherwise.
 *
 * @note Since this macro allocates a buffer, it must only be called once (it is OK to call it
 *       several times as long as it is from the same location, e.g. to do a reinitialization).
 */
/*lint -emacro(506, BLE_ANT_STACK_HANDLER_INIT) */ /* Suppress "Constant value Boolean */
#define BLE_ANT_STACK_HANDLER_INIT(CLOCK_SOURCE,                                                   \
                                   MTU_SIZE,                                                       \
                                   BLE_EVT_HANDLER,                                                \
                                   ANT_EVT_HANDLER,                                                \
                                   USE_SCHEDULER)                                                  \
    do                                                                                             \
    {                                                                                              \
        static uint32_t EVT_BUFFER[CEIL_DIV(MAX(sizeof(ble_evt_t) + (MTU_SIZE),                    \
                                                sizeof(ant_evt_t)),                                \
                                            sizeof(uint32_t))];                                    \
        uint32_t ERR_CODE;                                                                         \
        ERR_CODE = ble_ant_stack_handler_init((CLOCK_SOURCE),                                      \
                                              EVT_BUFFER,                                          \
                                              sizeof(EVT_BUFFER),                                  \
                                              (BLE_EVT_HANDLER),                                   \
                                              (ANT_EVT_HANDLER),                                   \
                                              (USE_SCHEDULER) ? ble_ant_stack_evt_schedule : NULL);\
        APP_ERROR_CHECK(ERR_CODE);                                                                 \
    } while (0)

/**@brief Initialize stack handler module.
 *
 * @details Enables the SoftDevice and the stack event interrupt handler.
 *
 * @note This function must be called before calling any function in the SoftDevice API.
 *
 * @note Normally initialization should be done using the BLE_ANT_STACK_HANDLER_INIT() macro, as
 *       that will both allocate the event buffer, and also align the buffer correctly.
 *
 * @param[in]  clock_source        Low frequency clock source to be used by the SoftDevice.
 * @param[in]  p_evt_buffer        Buffer for holding one stack event. Since we are
 *                                 not using the heap, this buffer must be provided by
 *                                 the application. The buffer must be large enough to hold
 *                                 the biggest stack event the application is supposed to
 *                                 handle. The buffer must be aligned to a 4 byte boundary.
 * @param[in]  evt_buffer_size     Size of stack event buffer.
 * @param[in]  ble_evt_handler     Handler to be called for each received stack BLE event.
 * @param[in]  ant_evt_handler     Handler to be called for each received stack ANT event.
 * @param[in]  evt_schedule_func   Function for passing events to the scheduler. Point to
 *                                 ble_ant_stack_evt_schedule() to connect to the scheduler.
 *                                 Set to NULL to make the stack handler module call the event
 *                                 handler directly from the stack event interrupt handler.
 *
 * @retval     NRF_SUCCESS               Successful initialization.
 * @retval     NRF_ERROR_INVALID_PARAM   Invalid parameter (buffer not aligned to a 4 byte
 *                                       boundary) or NULL.
 */
uint32_t ble_ant_stack_handler_init(nrf_clock_lfclksrc_t              clock_source,
                                    void *                            p_evt_buffer,
                                    uint16_t                          evt_buffer_size,
                                    ble_ant_stack_ble_evt_handler_t   ble_evt_handler,
                                    ble_ant_stack_ant_evt_handler_t   ant_evt_handler,
                                    ble_ant_stack_evt_schedule_func_t evt_schedule_func);


// Functions for connecting the Stack Event Handler to the scheduler:

/**@cond NO_DOXYGEN */
void intern_ble_ant_stack_events_execute(void);

static __INLINE void ble_ant_stack_evt_get(void * p_event_data, uint16_t event_size)
{
    APP_ERROR_CHECK_BOOL(event_size == 0);
    intern_ble_ant_stack_events_execute();
}

static __INLINE uint32_t ble_ant_stack_evt_schedule(void)
{
    return app_sched_event_put(NULL, 0, ble_ant_stack_evt_get);
}
/**@endcond */

#endif // BLE_ANT_STACK_HANDLER_H__

/** @} */
