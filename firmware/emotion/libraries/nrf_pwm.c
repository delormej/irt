/* Copyright (c) 2014 Inside Ride Technologies, LLC. All Rights Reserved.
 *
 * Uses 3 compare registers: 	[0] Pulse Width in us (position of servo in microseconds)
 *														[1] Period Width of 20ms
 *														[2] Duration of 500ms (longest it would take to move 180 degrees)
 *
 *      |<--- Period Width (20ms) ------>|
 *
 *      +--------+                       +--------+
 *      |        |                       |        |
 * -----+        +-----------------------+        +------
 *
 *   -->|        |<-- Pulse Width 
*/
#include "nrf_pwm.h"
#include "nrf_sdm.h"
#include "app_error.h"
#include "app_timer.h"
#include "debug.h"

#define PULSE_TRAIN_DURATION	APP_TIMER_TICKS(1000, 0)		// In reality it should only take 500ms, but we're giving it 1000ms to be safe.

/**@brief Debug logging for module.
 *
 */
#ifdef ENABLE_DEBUG_LOG
#define PWM_LOG debug_log
#else
#define PWM_LOG(...)
#endif // ENABLE_DEBUG_LOG

uint32_t m_pwm_pin_output = 0;
app_timer_id_t m_stop_pulse_train_timer;
bool m_is_running = false;

/** @brief 	Called when the timer expires indicating it's time to stop the 
 *					pulse train going to the servo.
 */
static void pulse_train_timeout_handler(void * p_context)
{
	pwm_stop_servo();
}

/** @brief Function for initializing the Timer 2 peripheral.
 */
static void pwm_timers_init(void)
{
	//
	// Initialize timer2.
	//
	NRF_TIMER2->MODE        = TIMER_MODE_MODE_Timer;
	NRF_TIMER2->BITMODE     = TIMER_BITMODE_BITMODE_16Bit << TIMER_BITMODE_BITMODE_Pos;
	NRF_TIMER2->PRESCALER   = PWM_TIMER_PRESCALER;

	// Clears the timer, sets it to 0.
	NRF_TIMER2->TASKS_CLEAR = 1;

	// On compare 2 event, clear the counter and restart.
	NRF_TIMER2->SHORTS = TIMER_SHORTS_COMPARE2_CLEAR_Msk;

	//
	// Create a single shot app timer to be used to stop sending signals to the servo
	// after a preset amount of time (i.e. 2 seconds).
	//
	uint32_t err_code = app_timer_create(&m_stop_pulse_train_timer,
			APP_TIMER_MODE_SINGLE_SHOT,
			pulse_train_timeout_handler);

	// We assume that APP_TIMER_INIT has been previously called, if not
	// this will throw NRF_ERROR_INVALID_STATE.
	APP_ERROR_CHECK(err_code);
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
	
	// TODO: remove hardcode channel nos and define them globally (irt_common).
	// Using hardcoded channels that are available, 4 & 5.
	err_code = sd_ppi_channel_assign(4,
			&NRF_TIMER2->EVENTS_COMPARE[0],
			&NRF_GPIOTE->TASKS_OUT[PWM_CHANNEL_TASK_TOGGLE]);
	
	if (err_code == NRF_ERROR_SOC_PPI_INVALID_CHANNEL)
		APP_ERROR_HANDLER(NRF_ERROR_SOC_PPI_INVALID_CHANNEL);
	
	err_code = sd_ppi_channel_assign(5, 
			&NRF_TIMER2->EVENTS_COMPARE[1],
			&NRF_GPIOTE->TASKS_OUT[PWM_CHANNEL_TASK_TOGGLE]);
	
	if (err_code == NRF_ERROR_SOC_PPI_INVALID_CHANNEL)
	{
		APP_ERROR_HANDLER(NRF_ERROR_SOC_PPI_INVALID_CHANNEL);
	}

	sd_ppi_channel_enable_set((PPI_CHEN_CH4_Enabled << PPI_CHEN_CH4_Pos)
			| (PPI_CHEN_CH5_Enabled << PPI_CHEN_CH5_Pos));
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
    pwm_timers_init();

    // Enabling constant latency as indicated by PAN 11 "HFCLK: Base current with HFCLK 
    // running is too high" found at Product Anomaly document found at
    // https://www.nordicsemi.com/eng/Products/Bluetooth-R-low-energy/nRF51822/#Downloads
    //
    // @note This example does not go to low power mode therefore constant latency is not needed.
    //       However this setting will ensure correct behaviour when routing TIMER events through 
    //       PPI (shown in this example) and low power mode simultaneously.
    //NRF_POWER->TASKS_CONSTLAT = 1;
}

void pwm_set_servo(uint32_t pulse_width_us)
{
	if (pulse_width_us < PWM_PULSE_MIN ||
				pulse_width_us > PWM_PULSE_MAX)
	{
		PWM_LOG("[PWM]:pwm_set_servo WARN: Attempt to set servo out of range: %lu\r\n.",
				pulse_width_us);
		// TODO: should return an error here.
		return;
	}

	// Stop timer and clear any existing counts if already running.
	if (m_is_running)
		pwm_stop_servo();

	//
	// Configuring capture compares for the timer.
	//
	// First timer capture is just a time buffer for the 1st call, it can be any
	// value greater than a few hundred 'us'.  The 2nd is the actual pulse width
	// in microseconds (us).  The 3rd is the remainder of the 20ms total period.
	//
	NRF_TIMER2->CC[0] = pulse_width_us;
	NRF_TIMER2->CC[1] = pulse_width_us*2;
	NRF_TIMER2->CC[2] = PWM_PERIOD_WIDTH_US - (pulse_width_us*2);

	// Flag that the servo is running.
	m_is_running = true;

	// Start the timers.
	NRF_TIMER2->TASKS_START = 1;
	app_timer_start(m_stop_pulse_train_timer, PULSE_TRAIN_DURATION, NULL);
}

void pwm_stop_servo(void)
{
	NRF_TIMER2->TASKS_STOP = 1;
	NRF_TIMER2->TASKS_CLEAR = 1;

	nrf_gpio_pin_clear(m_pwm_pin_output);

	/*
	uint32_t tries = 0, max_tries = 5000;

	//
	// TODO: can't we just force the pin low here?
	//
	// Make sure that the pin is not high before sending another pulse train.
	while ((NRF_GPIO->IN & (1 << m_pwm_pin_output)) != 0)
	{
		// Keep trying for a while and if no success - just stop the timer.
		if (++tries > max_tries)
			break;
	}
	*/
}
