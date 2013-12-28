#ifndef IRT_PERIPHERAL_H__
#define IRT_PERIPHERAL_H__

#include "nrf_gpio.h"
#include "ble_nrf6310_pins.h"
#include "app_timer.h"

#define BONDMNGR_DELETE_BUTTON_PIN      NRF6310_BUTTON_1                             /**< Button used for deleting all bonded masters during startup. */

#define DEVICE_NAME                     "E-Motion"                               		 /**< Name of device. Will be included in the advertising data. */
#define MANUFACTURER_NAME               "Inside Ride Technologies"                   /**< Manufacturer. Will be passed to Device Information Service. */

#define APP_TIMER_PRESCALER             0                                            /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_MAX_TIMERS            1                                            /**< Maximum number of simultaneously created timers. */
#define APP_TIMER_OP_QUEUE_SIZE         4                                            /**< Size of timer operation queues. */

#define APP_GPIOTE_MAX_USERS            1                                         	 /**< Maximum number of users of the GPIOTE handler. */

#define PIN_SERVO_SIGNAL								3		// P3 - P0.03
#define PIN_BUTTON_I										4		// P3 - P0.04
#define PIN_BUTTON_II										2		// P3 - P0.02
#define PIN_BUTTON_III 									1		// P3 - P0.01
#define PIN_BUTTON_IV										5		// P3 - P0.05
#define PIN_DRUM_REV 										0		// P3 - P0.00 

#define WAKEUP_BUTTON_PIN               PIN_SERVO_SIGNAL														/**< Button used to wake up the application. */
#define BUTTON_DETECTION_DELAY          APP_TIMER_TICKS(50, APP_TIMER_PRESCALER)  /**< Delay from a GPIOTE event until a button is reported as pushed (in number of timer ticks). */

void peripheral_init(void (*on_button_evt)(uint8_t pin_no));

#endif // IRT_PERIPHERAL_H__
