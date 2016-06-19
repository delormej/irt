/*
 * Driver for the MCP4725 Digital to Analog Converter.
 *
 *  Created on: Jun 18, 2016
 *      Author: jasondel
 */
#include "twi_master.h"
#include "d2ac.h"

/* Interrupt Macros */
#define _BIT(x)					(0x01 << x)

#define NRF_TWI					    0		// Define which instance of NRF_TWI to use.
#define MCP4725_I2C_ADDRESS 		0x60    // 1100 000
#define MCP4725_WRITE				(MCP4725_I2C_ADDRESS << 1)	
#define MCP4725_READ				(MCP4725_WRITE | 0x1)			


/* Initializes and sets power on defaults for d2ac. */ 
void d2ac_init() 
{
    twi_master_init(NRF_TWI);
}

/* Set the 12 bit output voltage, relative to VDD. */
uint32_t d2ac_set(uint16_t code)
{
                                    // 1st byte is the address
	uint8_t buffer[2];
	buffer[0] = code >> 8;                  // C2, C1, C0, X, X, PD1, PD0, X
	buffer[1] = (code & 0xFF); // 3rd byte

	// Write the register address.
	return twi_master_transfer(NRF_TWI,
					         MCP4725_WRITE,
							 buffer,
							 sizeof(buffer),
							 true);
}

/* Read the DAC register. */
uint16_t d2ac_read()
{

}
