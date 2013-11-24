/* Copyright (c) 2013 Jason De Lorme. All Rights Reserved.
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

uint32_t m_pwm_pin_output = 0;
bool m_is_running = false;

/** @brief Function for initializing the Timer 2 peripheral.
 */
static void timer2_init(void)
{
    // Start 16 MHz crystal oscillator .
    NRF_CLOCK->EVENTS_HFCLKSTARTED  = 0;
    NRF_CLOCK->TASKS_HFCLKSTART     = 1;

    // Wait for the external oscillator to start up.
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) 
    {
        //Do nothing.
    }

    NRF_TIMER2->MODE        = TIMER_MODE_MODE_Timer;
    NRF_TIMER2->BITMODE     = TIMER_BITMODE_BITMODE_16Bit << TIMER_BITMODE_BITMODE_Pos;
    NRF_TIMER2->PRESCALER   = PWM_TIMER_PRESCALER;

    // Clears the timer, sets it to 0.
    NRF_TIMER2->TASKS_CLEAR = 1;

		// On compare 2 event, clear the counter and restart.
		NRF_TIMER2->SHORTS = TIMER_SHORTS_COMPARE2_CLEAR_Msk;
}


/** @brief Function for initializing the GPIO Tasks/Events peripheral.
 */
static void gpiote_init(void)
{
    nrf_gpio_cfg_output(m_pwm_pin_output);

    //NRF_GPIO->OUT = 0x00000000UL;

    // Configure GPIOTE channel 0 to toggle the PWM pin state
		// @note Only one GPIOTE task can be connected to an output pin.
    nrf_gpiote_task_config(0, m_pwm_pin_output, \
                           NRF_GPIOTE_POLARITY_TOGGLE, NRF_GPIOTE_INITIAL_VALUE_LOW);
}


/** @brief Function for initializing the Programmable Peripheral Interconnect peripheral.
 */
static void ppi_init(void)
{
    // Configure PPI channel 0 to toggle PWM_OUTPUT_PIN on every TIMER2 COMPARE[0] match.
    NRF_PPI->CH[0].EEP = (uint32_t)&NRF_TIMER2->EVENTS_COMPARE[0];
    NRF_PPI->CH[0].TEP = (uint32_t)&NRF_GPIOTE->TASKS_OUT[0];

    // Configure PPI channel 1 to toggle PWM_OUTPUT_PIN on every TIMER2 COMPARE[1] match.
    NRF_PPI->CH[1].EEP = (uint32_t)&NRF_TIMER2->EVENTS_COMPARE[1];
    NRF_PPI->CH[1].TEP = (uint32_t)&NRF_GPIOTE->TASKS_OUT[0];
    
    // Enable PPI channels 0-1.
    NRF_PPI->CHEN = (PPI_CHEN_CH0_Enabled << PPI_CHEN_CH0_Pos)
                    | (PPI_CHEN_CH1_Enabled << PPI_CHEN_CH1_Pos);
}


/**
 * @brief Function for application main entry.
 */
void pwm_init(uint32_t pwm_pin_output_number)
{
    m_pwm_pin_output = pwm_pin_output_number;
	
    gpiote_init();
    ppi_init();
    timer2_init();

    // Enabling constant latency as indicated by PAN 11 "HFCLK: Base current with HFCLK 
    // running is too high" found at Product Anomaly document found at
    // https://www.nordicsemi.com/eng/Products/Bluetooth-R-low-energy/nRF51822/#Downloads
    //
    // @note This example does not go to low power mode therefore constant latency is not needed.
    //       However this setting will ensure correct behaviour when routing TIMER events through 
    //       PPI (shown in this example) and low power mode simultaneously.
    NRF_POWER->TASKS_CONSTLAT = 1;
}

void pwm_set_servo(uint32_t pulse_width_us)
{
		if (pulse_width_us < PWM_PULSE_MIN ||
					pulse_width_us > PWM_PULSE_MAX)
				// TODO: should return an error here.
				return;
		
		// Stop timer and clear any existing counts if already running.
		if (m_is_running)
			pwm_stop_servo();		

		NRF_TIMER2->CC[0] = pulse_width_us;
		NRF_TIMER2->CC[1] = pulse_width_us*2; 
		NRF_TIMER2->CC[2] = PWM_PERIOD_WIDTH_US - (pulse_width_us*2);
		
		// Start the timer.
		NRF_TIMER2->TASKS_START = 1;
		m_is_running = true;
}

void pwm_stop_servo(void)
{
		uint32_t tries = 0, max_tries = 5000;
		
		// Make sure that the pin is not high before sending another pulse train.
		while ((NRF_GPIO->IN & (1 << m_pwm_pin_output)) != 0)
		{
			// Keep trying for a while and if no success - just stop the timer.
			if (++tries > max_tries)
				break;
		}	
		NRF_TIMER2->TASKS_STOP = 1;
		NRF_TIMER2->TASKS_CLEAR = 1;			
}

/** @} */
