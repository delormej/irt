/* Driver for the temperature sensor. */
#ifndef __TEMPERATURE_H__
#define __TEMPERATURE_H__

#include <stdint.h>

void temperature_init(void);
void temperature_read(uint8_t *temperature);

#endif // __TEMPERATURE_H__
