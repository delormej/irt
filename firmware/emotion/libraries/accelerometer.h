#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#define MMA8652FC_SCL_ADDRESS		0x1D
#define MMA8652FC_WRITE				0x3A
#define MMA8652FC_READ				0x3B

/* Interrupt Macros */
#define _BIT(x)				(0x01 << x)

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
#define INT_CFG_FF_MT		_BIT(2)		// Freefall/motion INT1/INT2 Configuration.

/* FIFO Modes */
#define FMODE_DISABLE		(0x00 << 6)
#define FMODE_CIRCULAR	(0x01 << 6)
#define FMODE_FILL			(0x02 << 6)
#define FMODE_EVENT			(0x03 << 6)

/* FIFO status flags */
#define F_OVR						_BIT(7)
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

/**@brief Initializes I2C Two wire interface (TWI) and addresses the MMA8652FC 
 * 				accelerometer.
 */ 
void accelerometer_init(void);

#endif // ACCELEROMETER_H
