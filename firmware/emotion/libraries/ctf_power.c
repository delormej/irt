/* Copyright (c) 2017 Inside Ride Technologies, LLC. All Rights Reserved.
*/
#include <stdbool.h>
#include "math_private.h"
#include <math.h>
#include "string.h"
#include "app_util.h"
#include "ctf_power.h"
#include "speed_event_fifo.h"

#define CTF_DEFINED_MESSAGE         0x10
#define CTF_ZERO_OFFSET             0x01
#define EVENTS_PER_SECOND           4
#define CTF_CADENCE_TIMEOUT_UNITS	6000				// 3 seconds * 1/2000 of a second.
#define GET_TIMESTAMP(p_ctf)        (p_ctf->timestamp_msb << 8 | p_ctf->timestamp_lsb)
#define GET_TICKS(p_ctf)            (p_ctf->torque_ticks_msb << 8 | p_ctf->torque_ticks_lsb)
#define GET_SLOPE(p_ctf)            (p_ctf->slope_msb << 8 | p_ctf->slope_lsb)
#define DELTA_ROLLOVER_16(prior, current)	(prior > current ? (UINT16_MAX ^ prior) + current : current - prior)  /** Handles the delta between 2 16 bit ints, addressing potential rollover. */
#define DELTA_ROLLOVER_8(prior, current)	(prior > current ? (UINT8_MAX ^ prior) + current : current - prior)  /** Handles the delta between 2 8 bit ints, addressing potential rollover. */
#define IS_DELTA_NULL(delta)        (delta.events = 0 && delta.time == 0 && delta.ticks == 0)

static uint16_t page_count;
static ant_bp_ctf_t ctf_main_page[SPEED_EVENT_CACHE_SIZE];
static event_fifo_t ctf_main_page_fifo;
static uint16_t last_cadence_timestamp = 0;
static bool ctf_in_use = false;
static bool in_calibration = false;

typedef struct
{
    uint8_t events;
    uint16_t time;
    uint16_t ticks;
} ctf_power_delta_t;

static float get_torque(ctf_power_delta_t delta, uint16_t slope)
{  
    // The average torque per revolution of the pedal is calculated using the calibrated Offset parameter.
    float torque_frequency = (1.0f / ( (delta.time * 0.0005f) / delta.ticks)) - getCtfOffset(); // hz

    // Torque in Nm is calculated from torque rate (Torque Frequency) using the calibrated sensitivity Slope
    float torque = torque_frequency / (slope/10);    

    return torque;
}

static uint8_t get_cadence(ctf_power_delta_t delta)
{
    float cadence_period = (delta.time / delta.events) * 0.0005f; // Seconds
    return (uint8_t) ROUNDED_DIV(60, cadence_period); // Revs per minute (RPM)
}

static int16_t get_watts(ant_bp_ctf_t* p_current, ctf_power_delta_t delta)
{
    uint8_t cadence;
    float torque;
    int16_t watts;

    cadence = get_cadence(delta);
    if (cadence > 0)
    {
        torque = get_torque(delta, GET_SLOPE(p_current));
        watts = roundl(torque * cadence * (MATH_PI/30)); // watts
    }
    else
        watts = 0;

    return watts;
}

static ctf_power_delta_t get_ctf_delta_from_current(ant_bp_ctf_t* p_current, uint8_t events)
{
    ant_bp_ctf_t* p_previous;
    
    do
    {
        // dig back further, until you hit the end.
        p_previous = (ant_bp_ctf_t*)speed_event_fifo_oldest(&ctf_main_page_fifo, events);
        
    } while(p_previous->page_number == 0 && --events > 0);

    ctf_power_delta_t delta = {
        .events = DELTA_ROLLOVER_8(p_previous->event_count, p_current->event_count),
        .time = DELTA_ROLLOVER_16(GET_TIMESTAMP(p_previous), GET_TIMESTAMP(p_current)),
        .ticks = DELTA_ROLLOVER_16(GET_TICKS(p_previous), GET_TICKS(p_current))
    };
    return delta;
}

static ant_bp_ctf_t* get_current_ctf_main()
{
    return (ant_bp_ctf_t*) speed_event_fifo_get(&ctf_main_page_fifo);
}

static void ctf_reset()
{
    ctf_in_use = true;
    in_calibration = false;
    page_count = 0;
    ctf_main_page_fifo = speed_event_fifo_init((uint8_t*)ctf_main_page, 
        sizeof(ant_bp_ctf_t)); 
}

static void ctf_offset_reset()
{ 
    in_calibration = true;
    ctf_in_use = true;
    resetCtfOffsetSamples();
}

static void add_ctf_page(ant_bp_ctf_t* p_page)
{
    //TODO: do we want to put events that don't have net new data? i.e. timestamp hasn't advanced?
    page_count++;
    speed_event_fifo_put(&ctf_main_page_fifo, (uint8_t*)p_page);   
}

int16_t ctf_get_average_power(uint8_t seconds)
{
    ant_bp_ctf_t* p_current;
    ctf_power_delta_t delta;
    int16_t watts = 0;

    p_current = get_current_ctf_main();
    delta = get_ctf_delta_from_current(p_current, (EVENTS_PER_SECOND * seconds));
    watts = get_watts(p_current, delta);

    return watts;
}

// returns NO_EVENTS, CADENCE_TIMEOUT, CTF_SUCCESS
uint32_t ctf_get_power(int16_t* p_watts)
{
    uint32_t err = CTF_NO_EVENTS;
    ant_bp_ctf_t* p_current;
    ctf_power_delta_t delta;

    // Must have 2 events in order to calculate power.
    if (page_count < 1)
        return err;

    p_current = get_current_ctf_main();
    delta = get_ctf_delta_from_current(p_current, 1);

    if (delta.events == 0)
    {
        if (delta.time > CTF_CADENCE_TIMEOUT_UNITS)
        {
            // If after 3 seconds there is no cadence update, report 0 cadence/power.
            *p_watts = 0;
            err = CTF_CADENCE_TIMEOUT;
        }
    }
    else 
    {
        last_cadence_timestamp = GET_TIMESTAMP(p_current); // Keep track of last valid event.
        *p_watts = get_watts(p_current, delta);
        err = CTF_SUCCESS;
    }
    return err;
}

void ctf_set_calibration_page(ant_bp_ctf_calibration_t* p_page)
{
    if (p_page->calibration_id == CTF_DEFINED_MESSAGE && p_page->ctf_defined_id == CTF_ZERO_OFFSET)
    {
        if (!in_calibration)
            ctf_offset_reset();       
        addCtfOffsetSample(p_page->offset_msb << 8 | p_page->offset_lsb);
    } 
}

void ctf_set_main_page(ant_bp_ctf_t* p_page)
{
    if (!ctf_in_use || in_calibration)
        ctf_reset();
    add_ctf_page(p_page);   
}

bool ctf_power_in_use()
{
    return ctf_in_use;
}
