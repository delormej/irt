/*
*******************************************************************************
*
* Library for talking to the MMA8652FC digital accelerometer using I2C interface.
*
********************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include "accelerometer.h"
#include "irt_peripheral.h"
#include "twi_master.h"

static void accelerometer_read(uint8_t *reg, uint8_t *data, uint8_t size);

/**@brief Writes to the accelerometer using I2C. */
static bool accelerometer_write(uint8_t reg, uint8_t data, uint8_t size)
{
	// Write the register address.
	if (!twi_master_transfer(MMA8652FC_WRITE, 
													 &reg, 
													 sizeof(reg), 
													 false))
	{
		// Something failed with the write.
		return false;
	}
	else 
	{
		// Write the value.
		return twi_master_transfer(MMA8652FC_WRITE, 
													 &data, 
													 size, 
													 true);	
	}
}

static void enable_interrupt(void)
{
	/* To WAKE the device, the desired function(s) must be enabled in CTRL_REG4 
		 register and set to WAKE-to-SLEEP in CTRL_REG3 register. 	

		 CTRL_REGx contents can only be modified while the device is in STANDBY mode; 
		 the only exceptions to this are the CTRL_REG1[ACTIVE] and CTRL_REG2[RST] bits.
		 
		 Wake ODR (Output Data Rate) is set by CTRL_REG1[DR] bits.
		 
		 *NOTE: By default an interrupt event is signaled LOW (logical 0).  This can be
		 configured, but we'll use the default - just need to make sure we set this
		 properly in the GPIO pin cfg.
*/
	bool ret = false;
	uint8_t reg = 0;
	uint8_t data = 0;
		
	//
	// Set device to STANDBY by setting bit 0 to value 0 in CTRL_REG1.
	//
	ret = accelerometer_write(REG8652_CTRL_REG1, MMA8652FC_STANDBY, sizeof(data));
	
	//
	// Set CTRL_REG3 WAKE_FF_MT bit to 1 to enable motion interrupt.
	// CTRL_REG3 register is used to control the Auto-WAKE/SLEEP function by 
	// setting the orientation or Freefall/Motion as an interrupt to wake.
	//
	ret = accelerometer_write(REG8652_CTRL_REG3, WAKE_FF_MT, sizeof(data));
	
	//
	// Set CTRL_REG4 INT_EN_FF_MT bit to 1.
	// CTRL_REG4 register enables the following interrupts: Auto-WAKE/SLEEP, 
	// Orientation Detection, Freefall/Motion, and Data Ready.
	//
	ret = accelerometer_write(REG8652_CTRL_REG4, INT_EN_FF_MT, sizeof(data));	
	
	//
	// Set configuration in CTRL_REG5 to route interupt to INT1.
	//
	ret = accelerometer_write(REG8652_CTRL_REG5, INT_CFG_FF_MT, sizeof(data));	
	
	//
	// Set device to ACTIVE by setting bit 0 to value 1 in CTRL_REG1.
	//
	ret = accelerometer_write(REG8652_CTRL_REG1, MMA8652FC_ACTIVE, sizeof(data));
}


void accelerometer_init(void)
{
	twi_master_init();
	enable_interrupt();
}