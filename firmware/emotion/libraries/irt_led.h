/*
 * irt_led.h
 *
 *  Created on: Nov 12, 2014
 *      Author: Jason
 */

#ifndef IRT_LED_H_
#define IRT_LED_H_

// this goes in the header:
typedef enum
{
	// Power indicators
	LED_POWER_OFF,
	LED_POWER_ON,
	LED_BATT_LOW,
	LED_BATT_WARN,
	LED_BATT_CRITICAL,
	LED_CHARGING,
	LED_CHARGED,

	// Function indicators
	LED_BLE_ADVERTISTING,
	LED_BLE_CONNECTED,
	LED_BLE_DISCONNECTED,
	LED_BLE_TIMEOUT,
	LED_BUTTON_UP,
	LED_BUTTON_DOWN,
	LED_MODE_STANDARD,
	LED_MODE_ERG,
	LED_MIN_MAX,
	LED_CALIBRATION_ENTER,
	LED_CALIBRATION_EXIT,

	LED_POWERING_DOWN,
	LED_ERROR

} led_pattern_e;

/**@brief	Turn one or more LEDs on.
 *
 */
void led_set(led_pattern_e pattern);

/**@brief	Initializes the LEDs vio GPIO and the timer use to control.
 *
 */
void led_init(void);

#endif /* IRT_LED_H_ */
