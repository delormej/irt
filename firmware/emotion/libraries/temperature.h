/* Driver for the temperature sensor. */
#ifndef __TEMPERATURE_H__
#define __TEMPERATURE_H__

#include <stdint.h>

void temperature_init(void);
float temperature_read(void);

#endif // __TEMPERATURE_H__