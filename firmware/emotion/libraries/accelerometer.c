/*
*******************************************************************************
*
* Library for talking to the MMA8652FC digital accelerometer using I2C interface.
*
********************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include "irt_common.h"
#include "accelerometer.h"
#include "irt_peripheral.h"
#include "twi_master.h"

#define MMA8652FC_I2C_ADDRESS		0x1D
#define MMA8652FC_WRITE				(MMA8652FC_I2C_ADDRESS << 1)	// 0x3A
#define MMA8652FC_READ				(MMA8652FC_WRITE | 0x1)			// 0x3B

/* Interrupt Macros */
#define _BIT(x)				(0x01 << x)

#define FF_MT_ELE			_BIT(7)
#define FF_MT_OAE			_BIT(6)
#define FF_MT_ZEFE			_BIT(5)
#define FF_MT_YEFE			_BIT(4)
#define FF_MT_XEFE			_BIT(3)

/* CTRL_REG1 bits. */
#define MMA8652FC_ACTIVE	_BIT(0)
#define MMA8652FC_STANDBY	(0x00)

/* CTRL_REG2 bits. */
#define AUTO_SLEEP_ENABLE	_BIT(2)

/* CTRL_REG3 wake bits. */
#define WAKE_FF_MT			_BIT(3)		// Wake from freefall/motion interrupt.
#define INT_POLARITY 		_BIT(1)		// Configure interrupt polarity.
#define OPEN_DRAIN 			_BIT(0)		// Configure Open-Drain vs push/pull.

/* CTRL_REG4 register enables the following interrupts: Auto-WAKE/SLEEP,
	 Orientation Detection, Freefall/Motion, and Data Ready. */
#define INT_EN_ASLP			_BIT(7)		// Auto-SLEEP/WAKE
#define INT_EN_FIFO			_BIT(6)		// FIFO interrupt
#define INT_EN_TRANS		_BIT(5)		// Transient interrupt
#define INT_EN_LNDPRT 		_BIT(4)		// Landscape/portrait (orientation)
#define INT_EN_PULSE 		_BIT(3)		// Pulse
#define INT_EN_FF_MT 		_BIT(2)		// Freefall/motion interrupt
#define INT_EN_DRDY 		_BIT(0)		// Dataready interrupt

/* CTRL_REG5 register maps the desired interrupts to INT2 or INT1 pins.
		If the bit value is 0, then the functional block’s interrupt is routed to INT2 (default).
		If the bit value is 1, then the functional block’s interrupt is routed to INT1.
*/
#define INT_CFG_FF_MT		_BIT(2)		// Freefall/motion INT1/INT2 configuration.
#define INT_CFG_ASLP		_BIT(7)		// Auto-Sleep/Wake INT1/INT2 configuration.

/* FIFO Modes */
#define FMODE_DISABLE		(0x00 << 6)
#define FMODE_CIRCULAR		(0x01 << 6)
#define FMODE_FILL			(0x02 << 6)
#define FMODE_EVENT			(0x03 << 6)

/* FIFO status flags */
#define F_OVR					_BIT(7)
#define F_WMRK					_BIT(6)
#define F_COUNT					(0x3f)

/* Register Map for the MMA8652FC. */
enum {
	REG8652_STATUS = 0x00,
	REG8652_OUT_X_MSB,
	REG8652_OUT_X_LSB,
	REG8652_OUT_Y_MSB,
	REG8652_OUT_Y_LSB,
	REG8652_OUT_Z_MSB,
	REG8652_OUT_Z_LSB,
	REG8652_RSVD1,
	REG8652_RSVD2,
	REG8652_F_SETUP,
	REG8652_TRIG_CFG,
	REG8652_SYSMOD,
	REG8652_INT_SOURCE,
	REG8652_WHO_AM_I,
	REG8652_XYZ_DATA_CFG,
	REG8652_HP_FILTER_CUTOFF,
	REG8652_PL_STATUS,
	REG8652_PL_CFG,
	REG8652_PL_COUNT,
	REG8652_PL_BF_ZCOMP,
	REG8652_PL_THS_REG,
	REG8652_FF_MT_CFG,
	REG8652_FF_MT_SRC,
	REG8652_FF_MT_THS,
	REG8652_FF_MT_COUNT,
	REG8652_RSVD3,
	REG8652_RSVD4,
	REG8652_RSVD5,
	REG8652_RSVD6,
	REG8652_TRANSIENT_CFG,
	REG8652_TRANSIENT_SRC,
	REG8652_TRANSIENT_THS,
	REG8652_TRANSIENT_COUNT,
	REG8652_PULSE_CFG,
	REG8652_PULSE_SRC,
	REG8652_PULSE_THSX,
	REG8652_PULSE_THSY,
	REG8652_PULSE_THSZ,
	REG8652_PULSE_TMLT,
	REG8652_PULSE_LTCY,
	REG8652_PULSE_WIND,
	REG8652_ASLP_COUNT,
	REG8652_CTRL_REG1,
	REG8652_CTRL_REG2,
	REG8652_CTRL_REG3,
	REG8652_CTRL_REG4,
	REG8652_CTRL_REG5,
	REG8652_OFF_X,
	REG8652_OFF_Y,
	REG8652_OFF_Z,
	REG8652_RSVD7,
};

#define FMODE_WATERMARK(x)	(x & 0x3f)

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

static bool accelerometer_reset(void)
{
	uint8_t val = 0;
	uint8_t tries = 100;

	accelerometer_write(REG8652_CTRL_REG2, 0x40);		// Reset all registers to POR values

	do		// Wait for the RST bit to clear
	{
		accelerometer_read(REG8652_CTRL_REG2, &val, sizeof(val));
		tries--;
	} 	while (tries &&  (val & 0x40));

	// Returns success if happened in less than max tries.
	return (tries > 0);
}

static void enable_interrupt(void)
{
	/* To WAKE the device, the desired function(s) must be enabled in CTRL_REG4 
		 register and set to WAKE-to-SLEEP in CTRL_REG3 register. 	

		 CTRL_REGx contents can only be modified while the device is in STANDBY mode; 
		 the only exceptions to this are the CTRL_REG1[ACTIVE] and CTRL_REG2[RST] bits.
		 
		 Wake ODR (Output Data Rate) is set by CTRL_REG1[DR] bits.
	 */
	bool ret = false;

	ret = accelerometer_reset();

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
	ret = accelerometer_write(REG8652_FF_MT_THS, 0x04);

	//
	// Set the minimum time period of inactivity required to switch the part
	// between Wake and Sleep status.
	//
	ret = accelerometer_write(REG8652_ASLP_COUNT, 0xFF); // Set auto-sleep wait period to 81seconds (255/0.32s=~81.6s)

	//
	// Set the debounce count.
	/*
	const uint8_t DEBOUNCE_COUNT = 1u;
	ret = accelerometer_write(REG8652_FF_MT_COUNT, DEBOUNCE_COUNT);
	*/

	//
	// Enable Auto-Sleep.
	//
	ret = accelerometer_write(REG8652_CTRL_REG2, AUTO_SLEEP_ENABLE);

	//
	// CTRL_REG3 register is used to control the Auto-WAKE/SLEEP function by 
	// setting the orientation or Freefall/Motion as an interrupt to wake.
	// Open-Drain configuration can be used for connecting multiple interrupt 
	// signals on the same interrupt line.
	// Configure interrupt polarity to be LOW to HIGH on interrupt.
	//
	ret = accelerometer_write(REG8652_CTRL_REG3, WAKE_FF_MT | OPEN_DRAIN); //  | INT_POLARITY
	
	//
	// Set CTRL_REG4's freefall/motion interrupt bit INT_EN_FF_MT .
	// CTRL_REG4 register enables the following interrupts: Auto-WAKE/SLEEP, 
	// Orientation Detection, Freefall/Motion, and Data Ready.
	//
	ret = accelerometer_write(REG8652_CTRL_REG4, INT_EN_ASLP | INT_EN_FF_MT);

	//
	// Configure +/-8g full scale range.
	/*
	uint8_t FS1_MASK = _BIT(1);
	ret = accelerometer_write(REG8652_XYZ_DATA_CFG, FS1_MASK);
	*/

	//
	// Set configuration in CTRL_REG5 to route interrupt to INT1.
	//
	ret = accelerometer_write(REG8652_CTRL_REG5, INT_CFG_ASLP | INT_CFG_FF_MT);
	
	//
	// Set device to ACTIVE by setting bit 0 to value 1 in CTRL_REG1.
	// Also set the data rate to 100hz to match NRF TWI defaults.
	//
	uint8_t data_rate = (1 << 4) | (1 << 3); // binary 011000
	ret = accelerometer_write(REG8652_CTRL_REG1, 0x99); // MMA8652FC_ACTIVE | data_rate); // 0x99 == ASLP ODR=6.25Hz, ODR=100 Hz, Active mode
}


static void read_test(void)
{
	uint8_t data = 0;

	accelerometer_read(REG8652_WHO_AM_I, &data, sizeof(data));

	if (data == 0x4a)
		printf("Matches!\n");

	data = 0;
	accelerometer_read(REG8652_INT_SOURCE, &data, sizeof(data));
	if (data & (0x1 << 2))
		printf("SRC_FF_MT\n");

	data = 0;
	accelerometer_read(REG8652_SYSMOD, &data, sizeof(data));
	if (data & 1)
		printf("WAKE mode\n");
}

void accelerometer_init(void)
{
	twi_master_init();
	enable_interrupt();
	read_test();
}

uint32_t accelerometer_data(accelerometer_data_t* p_data)
{
	// Read the relevant source registers to clear for other events.

	accelerometer_read(REG8652_INT_SOURCE, &(p_data->source), sizeof(p_data->source));

	if (p_data->source & ACCELEROMETER_SRC_FF_MT) // Free-fall / motion interrupt.
	{
		accelerometer_read(REG8652_FF_MT_SRC, &(p_data->data), sizeof(p_data->data));

		// Read the actual data.
		accelerometer_read(REG8652_OUT_Y_MSB, &(p_data->out_y_msb),sizeof(p_data->out_y_msb));
		accelerometer_read(REG8652_OUT_Y_LSB, &(p_data->out_y_lsb),sizeof(p_data->out_y_lsb));
	}
	
	if (p_data->source & 0x80) // Auto-sleep/wake interrupt.
	{
		// Service and clear the auto-sleep/wake interrupt.
		accelerometer_read(REG8652_SYSMOD, &(p_data->mode), sizeof(p_data->mode));
	}
	
	return IRT_SUCCESS;
}
