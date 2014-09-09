/* Copyright (c) 2014 Inside Ride Technologies, LLC. All Rights Reserved.
 *
 * PWM module for controlling servo.  Servo operates by sending the desired position as a HIGH pulse to a pin
 * timed in microseconds as part of a 20ms cycle.  Each 20ms, the period the pin is high is the target position
 * of the servo.
 *
 * Uses 3 compare registers: 	[0] Pulse Width in us (position of servo in microseconds) on time expiration toggles
 * 									the pin HIGH
 *								[1] Pulse Width * 2, on edge toggles pin LOW
 *								[2] Delta period width of 20ms - (pulse*2), on this expiration clears the timer and
 *									executes code to check if it should continue another cycle.
 *
 *         |<---   Period Width (20ms)  --->|
 *
 *         +--------+                       +--------+
 *         |        |                       |        |
 * --------+        +-----------------------+        +------
 *
 *      -->|        |<-- Pulse Width
*/
#include "nrf_pwm.h"
#include "nrf_sdm.h"
#include "nrf_delay.h"
#include "app_error.h"
#include "debug.h"

#define PULSE_TRAIN_MAX_COUNT	50		// In reality it should only take 500ms, but we're giving it 1000ms to be safe, which is 20ms * count of 50
#define RETRY_STOP_MAX			20		// # of times we should try to stop before just HARD stopping the timer.
#define RETRY_STOP_DELAY		1000	// Time in microseconds to delay in between retries.

#define IRT_PPI_CH_4			4
#define IRT_PPI_CH_5			5

#define IRT_PPI_CH_SERVO_1		IRT_PPI_CH_4
#define IRT_PPI_CH_SERVO_2		IRT_PPI_CH_5

/**@brief Debug logging for module.
 *
 */
#ifdef ENABLE_DEBUG_LOG
#define PWM_LOG debug_log
#else
#define PWM_LOG(...)
#endif // ENABLE_DEBUG_LOG

static uint32_t m_pwm_pin_output = 0;
static uint8_t m_period_count = 0;
static bool m_is_running = false;
static bool m_stop_requested = false;


/** @brief Function for handling timer 2 peripheral interrupts.
 */
void TIMER2_IRQHandler(void)
{
	if (m_stop_requested || m_period_count > PULSE_TRAIN_MAX_COUNT-1)
	{
		NRF_TIMER2->TASKS_STOP = 1;
		NRF_TIMER2->TASKS_CLEAR = 1;

		// Clear flags and reset counter.
		m_is_running = false;
		m_stop_requested = false;
		m_period_count = 0;
	}
	else
	{
		m_period_count++;
	}
}

/** @brief Function for initializing the Timer 2 peripheral.
 */
static void pwm_timer_init(void)
{
	//
	// Initialize timer2.
	//
	NRF_TIMER2->MODE        = TIMER_MODE_MODE_Timer;
	NRF_TIMER2->BITMODE     = TIMER_BITMODE_BITMODE_16Bit << TIMER_BITMODE_BITMODE_Pos;
	NRF_TIMER2->PRESCALER   = PWM_TIMER_PRESCALER;

	// Clears the timer, sets it to 0.
	NRF_TIMER2->TASKS_CLEAR = 1;

	/* Configure COMPARE[2] behaviors: */

	// On COMPARE[2] event, clear the counter and restart.
	NRF_TIMER2->SHORTS = TIMER_SHORTS_COMPARE2_CLEAR_Msk;

	// Enable interrupt handler.
    NVIC_EnableIRQ(TIMER2_IRQn);

    // Assign the interrupt to fire on COMPARE[2].
    NRF_TIMER2->INTENSET = (TIMER_INTENSET_COMPARE2_Enabled << TIMER_INTENSET_COMPARE2_Pos);
}


/** @brief Function for initializing the GPIO Tasks/Events peripheral.
 */
static void pwm_gpiote_init(void)
{
    nrf_gpio_cfg_output(m_pwm_pin_output);

    // Configure GPIOTE channel 0 to toggle the PWM pin state
	// @note Only one GPIOTE task can be connected to an output pin.
    nrf_gpiote_task_config(PWM_CHANNEL_TASK_TOGGLE, 
    		m_pwm_pin_output,
    		NRF_GPIOTE_POLARITY_TOGGLE,
    		NRF_GPIOTE_INITIAL_VALUE_LOW);
}

/** @brief Function for initializing the Programmable Peripheral Interconnect peripheral.
 *
 * 	@note	The PPI allows an event on one peripheral to automatically execute a task on 
 *				another peripheral without using the CPU.  In this case we are 
 */
static void pwm_ppi_init(void)
{
	uint32_t err_code; 
	
	// Assign PPI channels.
	err_code = sd_ppi_channel_assign(IRT_PPI_CH_SERVO_1,
			&NRF_TIMER2->EVENTS_COMPARE[0],
			&NRF_GPIOTE->TASKS_OUT[PWM_CHANNEL_TASK_TOGGLE]);

	APP_ERROR_CHECK(err_code);
	
	err_code = sd_ppi_channel_assign(IRT_PPI_CH_SERVO_2,
			&NRF_TIMER2->EVENTS_COMPARE[1],
			&NRF_GPIOTE->TASKS_OUT[PWM_CHANNEL_TASK_TOGGLE]);

	APP_ERROR_CHECK(err_code);
	
	// Enable PPI channels.
	err_code = sd_ppi_channel_enable_set((PPI_CHEN_CH4_Enabled << PPI_CHEN_CH4_Pos)
			| (PPI_CHEN_CH5_Enabled << PPI_CHEN_CH5_Pos));

	APP_ERROR_CHECK(err_code);
}

/**
 * @brief Initializes PWM.
 *
 * @note	APP_TIMER_INIT must be previously called, if not NRF_ERROR_INVALID_STATE 
 *				will get thrown as an exception.
 *
 *				TODO: we could use NRF_TIMER1 or we could remove PPI dependency and do it
 *				in CPU code and not use so many HW resources.  This should be considered.
 */
void pwm_init(uint32_t pwm_pin_output_number)
{
    m_pwm_pin_output = pwm_pin_output_number;
	
    pwm_gpiote_init();
    pwm_ppi_init();
    pwm_timer_init();

    // Enabling constant latency as indicated by PAN 11 "HFCLK: Base current with HFCLK 
    // running is too high" found at Product Anomaly document found at
    // https://www.nordicsemi.com/eng/Products/Bluetooth-R-low-energy/nRF51822/#Downloads
    //
    // @note This example does not go to low power mode therefore constant latency is not needed.
    //       However this setting will ensure correct behaviour when routing TIMER events through 
    //       PPI (shown in this example) and low power mode simultaneously.
    //NRF_POWER->TASKS_CONSTLAT = 1;
}

/**@brief	Starts the process of moving the servo to valid range (1,000 - 2,000) as
 * 			defined in microseconds.
 *			This call returns immediately and doesn't wait for the move to complete.
 *			Error returned if servo target is out of range.
 */
uint32_t pwm_set_servo(uint32_t pulse_width_us)
{
	uint32_t err_code;

	if (pulse_width_us < PWM_PULSE_MIN ||
				pulse_width_us > PWM_PULSE_MAX)
	{
		PWM_LOG("[PWM]:pwm_set_servo ERROR: Attempt to set servo out of range: %lu\r\n.",
				pulse_width_us);
		// TODO: assign an actual error number here.
		return -1;
	}

	// Ensure servo is not currently running.
	if (m_is_running)
	{
		err_code = pwm_stop_servo();

		if (err_code != NRF_SUCCESS)
		{
			return err_code;
		}
	}

	//
	// Configuring capture compares for the timer. See file header for description.
	// In theory, we could set the initial pin value to HIGH and get away with 1 less
	// capture and hence free up a PPI channel if needed.
	//
	NRF_TIMER2->CC[0] = pulse_width_us;
	NRF_TIMER2->CC[1] = pulse_width_us*2;
	NRF_TIMER2->CC[2] = PWM_PERIOD_WIDTH_US - (pulse_width_us*2);

	// Reset the counter.
	m_period_count = 0;

	// Start the timer to begin moving the servo.
	NRF_TIMER2->TASKS_START = 1;

	m_is_running = true;

	return NRF_SUCCESS;
}

/**@brief	Attempts to stop current servo movement if running.  This call blocks until the
 * 			servo stops or a timeout occurs and an error is returned.
 *
 */
uint32_t pwm_stop_servo(void)
{
	uint8_t retries = 0;

	// Flag a stop.
	m_stop_requested = true;

	// Wait until stop is executed or we timeout.
	while (m_is_running && ++retries < RETRY_STOP_MAX)
	{
		nrf_delay_us(RETRY_STOP_DELAY);
	}

	// If we reach here and still running, there's a problem.
	if (m_is_running)
	{
		PWM_LOG("[PWM] ERROR: Unable to stop servo.\r\n");
		// we failed, TODO: assign an actual error number here.
		return -1;
	}
	else
	{
		// Success in stopping the servo.
		return NRF_SUCCESS;
	}
}
