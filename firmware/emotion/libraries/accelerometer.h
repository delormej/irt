#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#define ACCELEROMETER_SRC_FF_MT			0x04
#define ACCELEROMETER_SRC_WAKE_SLEEP	0x80

typedef struct
{
	uint8_t mode;
	uint8_t source;
	uint8_t data;
	uint8_t out_y_lsb;
	uint8_t out_y_msb;
} accelerometer_data_t;

/**@brief Initializes I2C Two wire interface (TWI) and addresses the MMA8652FC 
 * 				accelerometer.
 */ 
void accelerometer_init(void);

/**@brief Puts the accelerometer in standby mode. */
void accelerometer_standby(void);

/**@brief Clears and returns the interrupt source of the last event. */
uint8_t accelerometer_src(void);

uint32_t accelerometer_data_get(accelerometer_data_t* p_data);

#endif // ACCELEROMETER_H
