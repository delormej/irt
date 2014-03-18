#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H


/**@brief Initializes I2C Two wire interface (TWI) and addresses the MMA8652FC 
 * 				accelerometer.
 */ 
void accelerometer_init(void);

/**@brief Clears and returns the interrupt source of the last event. */
uint8_t accelerometer_src(void);

#endif // ACCELEROMETER_H
