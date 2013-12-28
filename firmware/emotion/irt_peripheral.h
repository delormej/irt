#ifndef IRT_PERIPHERAL_H__
#define IRT_PERIPHERAL_H__

#include "nrf_gpio.h"
#include "ble_nrf6310_pins.h"
#include "app_timer.h"

#define WAKEUP_BUTTON_PIN               1	// button_iii NRF6310_BUTTON_0                             /**< Button used to wake up the application. */
#define BONDMNGR_DELETE_BUTTON_PIN      NRF6310_BUTTON_1                             /**< Button used for deleting all bonded masters during startup. */

#define DEVICE_NAME                     "E-Motion"                               /**< Name of device. Will be included in the advertising data. */
#define MANUFACTURER_NAME               "Inside Ride Technologies"                        /**< Manufacturer. Will be passed to Device Information Service. */

#define APP_TIMER_PRESCALER             0                                            /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_MAX_TIMERS            1                                            /**< Maximum number of simultaneously created timers. */
#define APP_TIMER_OP_QUEUE_SIZE         4                                            /**< Size of timer operation queues. */

void peripheral_init(void);

#endif // IRT_PERIPHERAL_H__
