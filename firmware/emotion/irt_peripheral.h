#ifndef IRT_PERIPHERAL_H__
#define IRT_PERIPHERAL_H__

#include "nrf_gpio.h"
#include "app_timer.h"

#define BONDMNGR_DELETE_BUTTON_PIN      11                             /**< Button used for deleting all bonded masters during startup. */

#define APP_TIMER_PRESCALER             0                                            /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_MAX_TIMERS            4                                            /**< Maximum number of simultaneously created timers. */
#define APP_TIMER_OP_QUEUE_SIZE         4                                            /**< Size of timer operation queues. */

#define APP_GPIOTE_MAX_USERS            1                                         	 /**< Maximum number of users of the GPIOTE handler. */

#define PIN_FLYWHEEL 										0		// This is the output of the optical sensor.
#define PIN_SERVO_SIGNAL								1		// PWM signal to servo.
#define PIN_PULSE_OPTICAL								4		// If used, a 1 will turn on the sensor LED.
#define PIN_SHAKE												5		// This is shake on the accelermoter.
#define PIN_ENBATT											6		// To read the battery voltage, set to a 1, otherwise 0.
#define PIN_BATTVOLT										7		// Analog input of battery voltage.
#define PIN_BUTTON_II										8		// RXD4014 - RF3
#define PIN_BUTTON_III									9		// RXD4014 - RF2
#define PIN_BUTTON_IV 									10	// RXD4014 - RF1
#define PIN_BUTTON_I										11	// RXD4014 - RF0
#define	PIN_LED_B												12	/* LED_A and LED_B control the STATUS LED. If they are both  */
#define PIN_LED_A												13	/* the same value, the LED is off. If LEDA is a 1 and LEDB is a 
																						 * 0, the LED will be GREEN. The reverse will generate RED. 
																						 * By PWM the states, other colors may be obtained. */
#define PIN_SDA													14	// Bidirectional signal from I2C bus
#define PIN_SCL													15	// Clock signal for I2C bus.

// Port pins P0.16-P0.18 and P0.21-P0.23 are brought out to test points for possible use.

// Until I update the code. 										
#define ADVERTISING_LED_PIN_NO					PIN_LED_A
#define CONNECTED_LED_PIN_NO						PIN_LED_A
#define ASSERT_LED_PIN_NO								PIN_LED_B

typedef struct peripheral_evt_s
{
	void (*on_button_i)(void);
	void (*on_button_ii)(void);
	void (*on_button_iii)(void);
	void (*on_button_iv)(void);
	void (*on_accelerometer_evt)(uint8_t source);
} peripheral_evt_t;

void peripheral_init(peripheral_evt_t *p_on_peripheral_evt);

// LED functions.
void set_led_red(void);
void set_led_green(void);
void clear_led(void);

#endif // IRT_PERIPHERAL_H__
