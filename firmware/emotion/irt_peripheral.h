#ifndef IRT_PERIPHERAL_H__
#define IRT_PERIPHERAL_H__

#include <stdint.h>

#define APP_TIMER_PRESCALER         0   /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_MAX_TIMERS        4   /**< Maximum number of simultaneously created timers. */
#define APP_TIMER_OP_QUEUE_SIZE     4   /**< Size of timer operation queues. */
#define APP_GPIOTE_MAX_USERS        1	/**< Maximum number of users of the GPIOTE handler. */

typedef struct peripheral_evt_s
{
	void (*on_button_i)(void);
	void (*on_button_ii)(void);
	void (*on_button_iii)(void);
	void (*on_button_iv)(void);
	void (*on_accelerometer_evt)(void);
} peripheral_evt_t;

void peripheral_init(peripheral_evt_t *p_on_peripheral_evt);

// LED functions.
void set_led_red(void);
void set_led_green(void);
void clear_led(void);
void blink_led_green_start(uint16_t interval_ms);
void blink_led_green_stop(void);

#endif // IRT_PERIPHERAL_H__
