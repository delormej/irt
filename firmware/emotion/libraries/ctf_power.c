/* Copyright (c) 2017 Inside Ride Technologies, LLC. All Rights Reserved.
*/
#include <stdbool.h>
#include "string.h"
#include "ctf_power.h"

#define CTF_DEFINED_MESSAGE     0x10
#define CTF_ZERO_OFFSET         0x01

static ant_bp_ctf_t ctf_main_page[2] = {0};
static bool current_page_index = 0;
static bool in_calibration_sequence = false;


static uint8_t calculate_cadence();
static float calculate_torque();
static float calculate_torque_frequency();
static uint16_t get_elapsed_time();
static uint16_t get_torque_ticks();

static void set_current_ctf_page(ant_bp_ctf_t* p_page)
{
    current_page_index = !current_page_index;
    memcpy(&ctf_main_page[current_page_index], (const void*)p_page, sizeof(ant_bp_ctf_calibration_t));
}

static ant_bp_ctf_t* get_previous_ctf_page()
{
    return &ctf_main_page[!current_page_index];
}

static void clear_ctf_pages()
{
    memset(ctf_main_page, 0, sizeof(ant_bp_ctf_t) *2);
}

uint8_t ctf_get_cadence()
{
    return 0;
}

int16_t ctf_get_power()
{
    return 0;
}

void ctf_set_calibration_page(ant_bp_ctf_calibration_t* p_page)
{
    if (p_page->calibration_id == CTF_DEFINED_MESSAGE && p_page->ctf_defined_id == CTF_ZERO_OFFSET)
    {
        // If this is the first offset message, reset the state.
        if (!in_calibration_sequence)
        {
            in_calibration_sequence = true;
            resetCtfOffsetSamples();
        }

        //BP_LOG("[BP] Received CTF offset: %i\r\n", ctf_offset);
        addCtfOffsetSample(p_page->offset_msb << 8 | p_page->offset_lsb);
    } 
}

void ctf_set_main_page(ant_bp_ctf_t* p_page)
{
    if (in_calibration_sequence)
    {
        in_calibration_sequence = false;
        clear_ctf_pages();
    }

    set_current_ctf_page(p_page);
}
