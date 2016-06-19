/*
 * Driver for the MCP4725 Digital to Analog Converter.
 *
 *  Created on: Jun 18, 2016
 *      Author: jasondel
 */
#include "twi_master.h"

/* ... 
 * */
void d2ac_init();

/* Set the 12 bit output voltage, relative to VDD. */
uint32_t d2ac_set(uint16_t code);

/* Read the DAC register. */
uint16_t d2ac_read();