#ifndef IRT_PERIPHERAL_H__
#define IRT_PERIPHERAL_H__

#include <stdbool.h>
#include <stdint.h>

#define APP_TIMER_PRESCALER         0   /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_MAX_TIMERS        5   /**< Maximum number of simultaneously created timers. */
#define APP_TIMER_OP_QUEUE_SIZE     4   /**< Size of timer operation queues. */
#define APP_GPIOTE_MAX_USERS        1	/**< Maximum number of users of the GPIOTE handler. */

// RTC1 is based on a 32.768khz crystal, or in other words it oscillates
// 32768 times per second.  The PRESCALER determines how often a tick gets
// counted.  With a prescaler of 0, there are 32,768 ticks in 1 second
// 1/2048th of a second would be 16 ticks (32768/2048)
// # of 2048th's would then be ticks / 16.
#define	TICK_FREQUENCY	(32768 / (NRF_RTC1->PRESCALER + 1))

#define LED_1						1
#define LED_2						2
#define LED_BOTH					LED_1 | LED_2

typedef struct peripheral_evt_s
{
	void (*on_button_pbsw)(void);
	void (*on_accelerometer_evt)(void);
	void (*on_power_plug)(bool plugged_in);
	void (*on_battery_result)(uint16_t);
} peripheral_evt_t;

void peripheral_init(peripheral_evt_t *p_on_peripheral_evt);
void peripheral_powerdown(bool accelerometer_off);

#ifdef BOARD_IRT_V2_REV_A1
/**@brief	Turn J7-6 power on/off.
 */
void peripheral_aux_pwr_set(bool turn_on);
#endif

// LED functions.
void set_led_red(uint8_t led_mask);
void set_led_green(uint8_t led_mask);
void clear_led(uint8_t led_mask);
void blink_led_green_start(uint8_t led_mask, uint16_t interval_ms);
void blink_led_green_stop(uint8_t led_mask);

/**@brief Get the current time in 1/2048s.
 *
 */
uint16_t seconds_2048_get(void);

#endif // IRT_PERIPHERAL_H__
