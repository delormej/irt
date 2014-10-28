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
#include "nrf51.h"
#include "nrf_delay.h"
#include "app_util.h"
#include "app_error.h"
#include "debug.h"

#define PULSE_TRAIN_MAX_COUNT		50		// In reality it should only take 500ms, but we're giving it 1000ms to be safe, which is 20ms * count of 50
#define RETRY_STOP_MAX				20		// # of times we should try to stop before just HARD stopping the timer.

#define PWM_PERIOD_WIDTH_US			20*1000		// 20ms
#define PWM_DURATION_US				500*1000	// 500ms is how long we should pulse for.
#define PWM_TIMER_PRESCALER 		4U 				// Prescaler 4 results in 1 tick == 1 microsecond (μs)
#define PWM_PULSE_MIN				699
#define PWM_PULSE_MAX				2107

// TODO: move generic define of channels to irt_peripheral and then assignment to specific funciton here.
#define IRT_GPIOTE_CH_TASK_TOGGLE	3
#define IRT_PPI_CH_4				4
#define IRT_PPI_CH_5				5

#define IRT_PPI_CH_SERVO_1			IRT_PPI_CH_4
#define IRT_PPI_CH_SERVO_2			IRT_PPI_CH_5

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

/**@brief	Forces hard timer stop.
 */
static void __INLINE pwm_timer_stop()
{
	// Stop & reset timer
	NRF_TIMER2->TASKS_STOP = 1;
	NRF_TIMER2->TASKS_CLEAR = 1;

	// Disable interrupt.
	sd_nvic_DisableIRQ(TIMER2_IRQn);

	// Clear state flags.
	m_is_running = false;
	m_stop_requested = false;

	//PWM_LOG("[PWM] Stopping at count: %i.\r\n", m_period_count);
	m_period_count = 0;
}

/**@brief Function for initializing the Timer 2 peripheral.
 */
static void pwm_timer_init(void)
{
	NRF_TIMER2->MODE        = TIMER_MODE_MODE_Timer;
	NRF_TIMER2->BITMODE     = TIMER_BITMODE_BITMODE_16Bit << TIMER_BITMODE_BITMODE_Pos;
	NRF_TIMER2->PRESCALER   = PWM_TIMER_PRESCALER;		// Such that each count is 1us.

	// Clears the timer, sets it to 0.
	NRF_TIMER2->TASKS_CLEAR = 1;

    sd_nvic_SetPriority(TIMER2_IRQn, APP_IRQ_PRIORITY_LOW);
}


/** @brief Function for initializing the GPIO Tasks/Events peripheral.
 */
static void pwm_gpiote_init(void)
{
    nrf_gpio_cfg_output(m_pwm_pin_output);

    // Configure GPIOTE channel 0 to toggle the PWM pin state
	// @note Only one GPIOTE task can be connected to an output pin.
    nrf_gpiote_task_config(IRT_GPIOTE_CH_TASK_TOGGLE,
    		m_pwm_pin_output,
    		NRF_GPIOTE_POLARITY_TOGGLE,
    		NRF_GPIOTE_INITIAL_VALUE_LOW);
}

/**@brief Function for initializing the Programmable Peripheral Interconnect peripheral.
 *
 * @note	The PPI allows an event on one peripheral to automatically execute a task on
 *			another peripheral without using the CPU.  Events are tied to the capture compare
 *			of the timer, when a time is hit, toggles the output pin forming the pulse width
 *			required to move the servo.  One channel per event/task pair.
 */
static void pwm_ppi_init(void)
{
	uint32_t err_code; 
	
	// Assign PPI channels.
	err_code = sd_ppi_channel_assign(IRT_PPI_CH_SERVO_1,
			&NRF_TIMER2->EVENTS_COMPARE[0],
			&NRF_GPIOTE->TASKS_OUT[IRT_GPIOTE_CH_TASK_TOGGLE]);

	APP_ERROR_CHECK(err_code);
	
	err_code = sd_ppi_channel_assign(IRT_PPI_CH_SERVO_2,
			&NRF_TIMER2->EVENTS_COMPARE[1],
			&NRF_GPIOTE->TASKS_OUT[IRT_GPIOTE_CH_TASK_TOGGLE]);

	APP_ERROR_CHECK(err_code);
	
	// Enable PPI channels.
	err_code = sd_ppi_channel_enable_set((PPI_CHEN_CH4_Enabled << PPI_CHEN_CH4_Pos)
			| (PPI_CHEN_CH5_Enabled << PPI_CHEN_CH5_Pos));

	APP_ERROR_CHECK(err_code);
}

#ifdef KURT
/**@brief	Moves a continuous rotation servo forward / backward / stop.
 * 			Speed is a percentage of speed -100...+100
 * 			- = reverse, 0 = stop + = forward
 */
uint32_t pwm_continuous_servo(int8_t speed)
{
	// TODO: figure out speed, see onenote for details.
	uint32_t position = 0;

	if (speed == 0)
	{
		position = pwm_set_servo(1523); // stop position 1521 - 1525
	}
	else if (speed < 0)
	{
		// reverse
		position = pwm_set_servo(1703); // 40% speed
	}
	else
	{
		position = pwm_set_servo(1297); // 40% speed
		// move forward
	}

	return position;
}
#endif

/**@brief	Starts the process of moving the servo to valid range (1,000 - 2,000) as
 * 			defined in microseconds.
 *			This call returns immediately and doesn't wait for the move to complete.
 *
 *			NRF_ERROR_INVALID_PARAM returned if servo target is out of range.
 */
uint32_t pwm_set_servo(uint32_t pulse_width_us)
{
	uint32_t err_code;

	if (pulse_width_us < PWM_PULSE_MIN ||
				pulse_width_us > PWM_PULSE_MAX)
	{
		PWM_LOG("[PWM]:pwm_set_servo ERROR: Attempt to set servo out of range: %lu\r\n.",
				pulse_width_us);

		return NRF_ERROR_INVALID_PARAM;
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

	// Reset the counter & flag.
	m_period_count = 0;
	m_is_running = true;

    // Assign the interrupt to fire on COMPARE[2].
    NRF_TIMER2->INTENSET = (TIMER_INTENSET_COMPARE2_Enabled << TIMER_INTENSET_COMPARE2_Pos);

	// Enable interrupt handler.
	sd_nvic_EnableIRQ(TIMER2_IRQn);

	//PWM_LOG("[PWM] Set Servo started: %.2f.\r\n", SECONDS_CURRENT);

	// Start the timer to begin moving the servo.
	NRF_TIMER2->TASKS_CLEAR = 1;
	NRF_TIMER2->TASKS_START = 1;

	return NRF_SUCCESS;
}

/**@brief	Attempts to stop current servo movement if running.  This call blocks until the
 * 			servo stops or a timeout occurs and an error is returned.
 *
 */
uint32_t pwm_stop_servo(void)
{
	uint8_t retries;

	// Flag a stop.
	m_stop_requested = true;
	retries = 0;

	while(nrf_gpio_pin_read(m_pwm_pin_output) == 1 && (retries++ < RETRY_STOP_MAX))
	{
		// This should never spin more than 2ms.
		nrf_delay_us(100);	// nominal delay
	}

	pwm_timer_stop();

	if (retries > RETRY_STOP_MAX)
	{
		// Force off.
		nrf_gpio_pin_write(m_pwm_pin_output, 0);

		PWM_LOG("[PWM] pwm_stop_servo ERROR: Unable to properly stop servo.\r\n");

		// Return error.
		return NRF_ERROR_INVALID_STATE;
	}
	else
	{
		return NRF_SUCCESS;
	}
}

/**@brief 	Initializes PWM.
 *
 */
void pwm_init(uint32_t pwm_pin_output_number)
{
    m_pwm_pin_output = pwm_pin_output_number;

    pwm_gpiote_init();
    pwm_ppi_init();
    pwm_timer_init();
}

/**@brief Function for handling timer 2 peripheral interrupts.
 */
void TIMER2_IRQHandler(void)
{
    if ((NRF_TIMER2->EVENTS_COMPARE[2] != 0) &&
       ((NRF_TIMER2->INTENSET & TIMER_INTENSET_COMPARE2_Msk) != 0))
	{
    	// Clear the event compare interrupt, otherwise interrupt handler will get
    	// called continuously.
		NRF_TIMER2->EVENTS_COMPARE[2] = 0;
	}

	// This interrupt handler should get called every 20ms - not more frequently.
	//PWM_LOG("[PWM] IRQ: %i @ %.2f Stop? %i.\r\n", NRF_TIMER2->CC[2], SECONDS_CURRENT, m_stop_requested);

	if (m_stop_requested || m_period_count++ >= PULSE_TRAIN_MAX_COUNT)
	{
		pwm_timer_stop();
	}
	else
	{
		// Clear the timer and restart.
		NRF_TIMER2->TASKS_CLEAR = 1;
	}
}
