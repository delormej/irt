/*
    Custom Standard Deviation implementation.
*/

#include <stdint.h>
#include "math_private.h"

float j_std_dev(uint16_t data[], uint8_t length)
{
    uint16_t sum = 0;
    float mean, standardDeviation = 0.0;

    int i;

    for(i=0; i<length; ++i)
    {
        sum += data[i];
    }

    mean = (float)sum/length;

    for(i=0; i<length; ++i)
    {
        float diff = (float)(data[i]) - mean;
        standardDeviation += diff * diff; //pow(data[i] - mean, 2);
    }

    return j_sqrtf(standardDeviation/length);
}
