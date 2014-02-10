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

static void accelerometer_read(uint8_t reg, uint8_t *data, uint8_t size)
{
	twi_master_transfer(MMA8652FC_WRITE,
						&reg,
						1,
						false);

	twi_master_transfer(MMA8652FC_READ,
						data,
						size,
						true);
}

/**@brief Writes to the accelerometer using I2C. */
static bool accelerometer_write(uint8_t reg, uint8_t data)
{
	uint8_t buffer[2];
	buffer[0] = reg;
	buffer[1] = data;

	// Write the register address.
	return twi_master_transfer(MMA8652FC_WRITE,
							 buffer,
							 sizeof(buffer),
							 true);
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
		
	//
	// Set device to STANDBY by setting bit 0 to value 0 in CTRL_REG1.
	//
	ret = accelerometer_write(REG8652_CTRL_REG1, MMA8652FC_STANDBY);

	//
	// Configure motion detection on Y axis only.
	// NOTE: When I tried setting additional axis, Y wasn't signaling.
	//
	ret = accelerometer_write(REG8652_FF_MT_CFG, (FF_MT_OAE | FF_MT_YEFE));

	//
	// Configure threshold for motion.
	// The threshold resolution is 0.063 g/LSB and the threshold register has a
	// range of 0 to 127 counts. The maximum range is to ±8 g.
	// Note that even when the full scale value is set to ±2 g or ±4 g, the motion
	// still detects up to ±8 g.
	//
	ret = accelerometer_write(REG8652_FF_MT_THS, 1u);

	//
	// Set the minimum time period of inactivity required to switch the part
	// between Wake and Sleep status.
	//
	const uint8_t ASLP_COUNT = 15u; // 127u;
	ret = accelerometer_write(REG8652_ASLP_COUNT, ASLP_COUNT);

	//
	// Enable Auto-Sleep.
	//
	ret = accelerometer_write(REG8652_CTRL_REG2, AUTO_SLEEP_ENABLE);

	//
	// Set CTRL_REG3 WAKE_FF_MT bit to 1 to enable motion interrupt.
	// CTRL_REG3 register is used to control the Auto-WAKE/SLEEP function by 
	// setting the orientation or Freefall/Motion as an interrupt to wake.
	// You could also configure to use open-drain.
	// Configure interrupt polarity to be LOW to HIGH on interrupt.
	//
	ret = accelerometer_write(REG8652_CTRL_REG3, (WAKE_FF_MT | INT_POLARITY)); // | OPEN_DRAIN));
	
	//
	// Set CTRL_REG4's freefall/motion interrupt bit INT_EN_FF_MT .
	// CTRL_REG4 register enables the following interrupts: Auto-WAKE/SLEEP, 
	// Orientation Detection, Freefall/Motion, and Data Ready.
	//
	ret = accelerometer_write(REG8652_CTRL_REG4, INT_EN_ASLP | INT_EN_FF_MT);  // 9:13am.

	//
	// Configure +/-8g full scale range.
	/*
	uint8_t FS1_MASK = _BIT(1);
	ret = accelerometer_write(REG8652_XYZ_DATA_CFG, FS1_MASK);
	*/

	//
	// Set configuration in CTRL_REG5 to route interrupt to INT1.
	//
	ret = accelerometer_write(REG8652_CTRL_REG5, (INT_CFG_ASLP | INT_CFG_FF_MT));
	
	//
	// Set device to ACTIVE by setting bit 0 to value 1 in CTRL_REG1.
	// Also set the data rate to 100hz to match NRF TWI defaults.
	//
	uint8_t data_rate = (1 << 4) | (1 << 3); // binary 011000
	ret = accelerometer_write(REG8652_CTRL_REG1, MMA8652FC_ACTIVE | data_rate);
}

/*
static void read_test(void)
{
	uint8_t data = 0;

	accelerometer_read(REG8652_WHO_AM_I, &data, sizeof(data));

	if (data == 0x4a)
		printf("Matches!\n");

	data = 0;
	accelerometer_read(REG8652_SYSMOD, &data, sizeof(data));
	if (data & 1)
		printf("WAKE mode\n");

	data = 0;
	accelerometer_read(REG8652_INT_SOURCE, &data, sizeof(data));
	if (data & (0x1 << 2))
		printf("SRC_FF_MT\n");

}*/

void accelerometer_init(void)
{
	twi_master_init();
	enable_interrupt();
}

uint8_t accelerometer_src(void)
{
	uint8_t data = 0;
	accelerometer_read(REG8652_INT_SOURCE, &data, sizeof(data));

	return data;
}
