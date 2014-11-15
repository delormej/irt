/* Copyright (c) 2014 Inside Ride Technologies, LLC. All Rights Reserved.
 *
 * @brief	Defines port pins and what they do for the IRT REV_A board.
 *
 */
#ifndef IRT_REV_A_H
#define IRT_REV_A_H

#define BONDMNGR_DELETE_BUTTON_PIN  11  /**< Button used for deleting all bonded masters during startup. */

#define PIN_FLYWHEEL 				0	// This is the output of the optical sensor.
#define PIN_SERVO_SIGNAL			1	// PWM signal to servo.
#define PIN_PULSE_OPTICAL			4	// If used, a 1 will turn on the sensor LED.
#define PIN_SHAKE					5	// This is shake on the accelermoter.
#define PIN_ENBATT					6	// To read the battery voltage, set to a 1, otherwise 0.
#define PIN_BATTVOLT				7	// Analog input of battery voltage.
#define PIN_BUTTON_II				8	// RXD4014 - RF3
#define PIN_BUTTON_III				9	// RXD4014 - RF2
#define PIN_BUTTON_IV 				10	// RXD4014 - RF1
#define PIN_BUTTON_I				11	// RXD4014 - RF0
#define	PIN_LED_B					12	/* LED_A and LED_B control the STATUS LED. If they are both  */
#define PIN_LED_A					13	/* the same value, the LED is off. If LEDA is a 1 and LEDB is a 
										 * 0, the LED will be GREEN. The reverse will generate RED.
										 * By PWM the states, other colors may be obtained. */
#define PIN_SDA						14	// Bidirectional signal from I2C bus
#define PIN_SCL						15	// Clock signal for I2C bus.

#define PIN_UART_RTS				8
#define PIN_UART_CTS				9
#define PIN_UART_TXD				10
#define PIN_UART_RXD				11
#define UART_HWFC					false

// Port pins P0.16-P0.18 and P0.21-P0.23 are brought out to test points for possible use.

#endif // IRT_REV_A_H
