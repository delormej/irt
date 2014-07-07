/* Copyright (c) 2014 Inside Ride Technologies, LLC. All Rights Reserved.
 *
 * @brief	Defines port pins and what they do for the IRT REV_A board.
 *
 */
#ifndef IRT_REV_2A_H
#define IRT_REV_2A_H

#define PIN_FLYWHEEL 				0	// This is the output of the optical sensor.
#define PIN_SERVO_SIGNAL			1	// PWM signal to servo.
#define PIN_BATT_VOLT				2	// Analog voltage resulting from dividing battery voltage by 2.5.
#define PIN_CHG_EN_N				3	// TP17 and signal to control battery charger - 0=ON, 1=OFF
#define PIN_EN_SERVO_PWR			4	// 1 = power to servo and OPTO LED, 0 = no power.
#define PIN_SHAKE					5	// This is shake on the accelermoter.
#define PIN_ENBATT					6	// 1 enables reading battery voltage, 0 turns off current drain

#define PIN_TP19					8	// TP19, not currently used.

#define PIN_STAT1					9	// Battery Status Indicators
#define PIN_STAT2					11	// See battery.h for details

#define	PIN_LED_B					12	/* LED_A and LED_B control the STATUS LED. If they are both  */
#define PIN_LED_A					13	/* the same value, the LED is off. If LEDA is a 1 and LEDB is a 
										 * 0, the LED will be GREEN. The reverse will generate RED.
										 * By PWM the states, other colors may be obtained. */

#define PIN_SCL						15	// Clock signal for I2C bus.
#define PIN_SDA						16	// Bidirectional signal from I2C bus

#define	PIN_LED_C					18	// See above for desc
#define PIN_LED_D					19	//

#define PIN_PG_N					20  // 0 = adapter power, 1= no adapter, also TP18

#define PIN_PBSW					22	// User Pushbutton - 0 when pressed. Must debounce.

#define PIN_SLEEP_N					30	// 0 = turn off power regulator, 1 = turn on power regulator
#define PIN_TP16					31	// Not currently used

// UART configuration
#define PIN_UART_TXD				21
#define PIN_UART_RTS				23
#define PIN_UART_RXD				24
#define PIN_UART_CTS				25
#define UART_HWFC					false

// Until I update the code. 										
#define ADVERTISING_LED_PIN_NO		PIN_LED_A
#define CONNECTED_LED_PIN_NO		PIN_LED_A
#define ASSERT_LED_PIN_NO			PIN_LED_B

#endif // IRT_REV_2A_H
