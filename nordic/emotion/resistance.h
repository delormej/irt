#ifndef __RESISTANCE_H__
#define __RESISTANCE_H__

#include <stdint.h>

#define PIN_SERVO_SIGNAL		3		// P3 - P0.03
#define MAX_RESISTANCE_LEVELS 10

void set_resistance(uint32_t level);

#endif
