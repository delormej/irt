/****************************************************************************** 
 * Copyright (c) 2017 Inside Ride Technologies, LLC. All Rights Reserved.
 *
 * @author      Jason De Lorme <jason@insideride.com>
 * @file 		ant_ctf_offset.c 
 * @brief 		Collects ctf offset values and validates they are within range.
 *
 ******************************************************************************/

#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "ctf_offset.h"
#include "math_private.h"

#define SAMPLE_SIZE         8       // Max 8 samples to track at once

/**@brief Debug logging for module.
 *
 */
#ifdef ENABLE_DEBUG_LOG
#include "debug.h"
#define CTF_LOG debug_log
#else
#define CTF_LOG(...)
#endif // ENABLE_DEBUG_LOG

static uint16_t samples[SAMPLE_SIZE] = { 0 };
static uint16_t sampleIndex = 0;
static float ctfOffset = 0.0F;

static float average(uint16_t data[], uint8_t length)
{
    float sum = 0.0F;
    for (int i = 0; i < length; i++)
    {
        sum += data[i];
    }
    return sum / length;
}

static bool hasValidSampleSize() 
{
    return (sampleIndex >= SAMPLE_SIZE);
}

void resetCtfOffsetSamples()
{
    sampleIndex = 0;
    memset(samples, 0, sizeof(uint16_t)*SAMPLE_SIZE);
    CTF_LOG("[CTF] Resetting samples.\r\n");
}

/** 
 * Adds a sample to the buffer to be validated.
*/
void addCtfOffsetSample(uint16_t value)
{
    //BP_LOG("[BP] Received CTF offset: %i\r\n", ctf_offset);
    // Calculate the position 0-SAMPLE_SIZE, incrementing sampleIndex.
    samples[sampleIndex++ % SAMPLE_SIZE] = value;
    if (hasValidSampleSize()) 
    {
        float stddev = j_std_dev(samples, SAMPLE_SIZE);
        if (j_fabsf(stddev) <= 4.0) 
        {
            float avg = average(samples, SAMPLE_SIZE);       
            // CTF_LOG("[CTF] ctfOffset std_dev: %.2f, avg: %.2f, prev: %.2f\r\n", 
            //     stddev, avg, ctfOffset);
            if (ctfOffset == 0.0F || j_fabsf(avg - ctfOffset) <= 4.0) 
            {
                // Within the range of acceptible values.
                ctfOffset = avg;
                CTF_LOG("[CTF] New ctfOffset calculated (*100): %i\r\n",
                 (uint16_t)(ctfOffset * 100));
            }
        }
    }
}

/**
 * Gets the last valid offset value.
*/
float getCtfOffset()
{
    return ctfOffset;
}
