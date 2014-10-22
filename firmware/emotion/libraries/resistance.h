/*
*******************************************************************************
* 
* By Jason De Lorme <jjdelorme@yahoo.com>
* http://www.roadacious.com
*
********************************************************************************/

#ifndef __RESISTANCE_H__
#define __RESISTANCE_H__

#include <stdint.h>
#include "irt_common.h"
#include "user_profile.h"

#define MAX_RESISTANCE_LEVEL_COUNT 	10u 	// Max number of resistance levels possible to set.
#define RESISTANCE_LEVELS 	7 				// Number of resistance levels available.
#define ERG_ADJUST_LEVEL	2U				// Watts to adjust increment /decrement

/**@brief		Array representing the servo position in micrseconds (us) by 
 *				resistance level 0-9, with a larger number representing more
 *				resistance applied.  0 is NO resistance.
 *
 */
static const uint16_t RESISTANCE_LEVEL[RESISTANCE_LEVELS] = { 
	2000, // 0 - no resistance
/*	1300,
	1225,
	1150,
	1075,
	1000,
	925,
	850,
	775, 
	700}; // Max resistance
*/ // TESTING 7 positions.
	1300,
	1200,
	1100,
	1000,
	900,
	800 };

#define MIN_RESISTANCE_LEVEL	resistance_position_min()	// Minimum by which there is no longer resistance.
#define MAX_RESISTANCE_LEVEL	resistance_position_max()	// Maximum resistance.

/**@brief Bike types, used for predefined resistance coefficients. */
typedef enum
{
	BIKE_ROAD					= 0x21,
	BIKE_MOUNT					= 0x2E
} bike_type_t;

/**@brief Resistance modes. */
typedef enum 
{
	RESISTANCE_SET_PERCENT 		= 0x40,
	RESISTANCE_SET_STANDARD		= 0x41,
	RESISTANCE_SET_ERG			= 0x42,
	RESISTANCE_SET_SIM			= 0x43, // Weight should come accross in this message?
	RESISTANCE_SET_BIKE_TYPE	= 0x44, // Co-efficient of rolling resistance
	RESISTANCE_SET_C			= 0x45, // Wind resistance offset.
	RESISTANCE_SET_SLOPE		= 0x46,
	RESISTANCE_SET_WIND			= 0x47,
	RESISTANCE_SET_WHEEL_CR		= 0x48,
	RESISTANCE_INIT_SPINDOWN	= 0x49,
	RESISTANCE_READ_MODE      	= 0x4A,
	RESISTANCE_SPINDOWN_RESULT	= 0x5A,
	RESISTANCE_SET_SERVO_POS 	= 0x5B,	// A new one that I'm adding for testing.
	RESISTANCE_SET_WEIGHT 		= 0x5C	// A new one that I'm adding for testing.
} resistance_mode_t;	// TODO: rename this OPCODE or something similar, it's not the mode necessarily.

/* This is from WAHOO FITNESS:
typedef enum
{
    // WF Trainer 0x40-0x5F
    WF_WCPMCP_OPCODE_TRAINER_SET_RESISTANCE_MODE      = 0x40,
    WF_WCPMCP_OPCODE_TRAINER_SET_STANDARD_MODE        = 0x41,
    WF_WCPMCP_OPCODE_TRAINER_SET_ERG_MODE             = 0x42,
    WF_WCPMCP_OPCODE_TRAINER_SET_SIM_MODE             = 0x43,
    WF_WCPMCP_OPCODE_TRAINER_SET_CRR                  = 0x44,
    WF_WCPMCP_OPCODE_TRAINER_SET_C                    = 0x45,
    WF_WCPMCP_OPCODE_TRAINER_SET_GRADE                = 0x46,
    WF_WCPMCP_OPCODE_TRAINER_SET_WIND_SPEED           = 0x47,
    WF_WCPMCP_OPCODE_TRAINER_SET_WHEEL_CIRCUMFERENCE  = 0x48,
    WF_WCPMCP_OPCODE_TRAINER_INIT_SPINDOWN            = 0x49,
    // 0x49 reserved.
    WF_WCPMCP_OPCODE_TRAINER_READ_MODE                = 0x4A,
    // 0x51-0x59 reserved.
    WF_WCPMCP_OPCODE_TRAINER_SPINDOWN_RESULT          = 0x5A,
    
} WFKICKRCommands_t;
*/
/**@brief Resistance control event payload. */
typedef struct
{
	resistance_mode_t 	operation;				// Operation to perform or mode to set for resistance.
	uint8_t				*pBuffer; 				// Pointer to values required for the operation.
} rc_evt_t;

/**@brief	Servo positions available.
 */
typedef struct
{
	uint8_t				count;
	uint16_t			positions[MAX_RESISTANCE_LEVEL_COUNT];
} servo_positions_t;

/**@brief Set resistance event handler type. */
typedef void(*rc_evt_handler_t) (rc_evt_t rc_evt);

/**@brief Factors used when calculating simulation forces. */
typedef struct rc_sim_forces_s
{
	float 	crr;
	float 	c;
	float 	wind_speed_mps;
	float 	grade;
	int16_t erg_watts;
} rc_sim_forces_t;

/**@brief		Initializes the pulse-width-modulation for the servo.
 * 				Returns the position of the servo after initialization.
 *
 */
uint16_t resistance_init(uint32_t servo_pin_number, user_profile_t* p_user_profile);

/**@brief		Gets the current position of the servo.
 *
 */
uint16_t resistance_position_get(void);

/**@brief		Sets the position of the servo.
 *
 */
uint16_t resistance_position_set(uint16_t position);

/**@brief		Gets the minimum (magnet off) resistance position.
 */
uint16_t resistance_position_min(void);

/**@brief 		Gets the maximum resistance position.
 */
uint16_t resistance_position_max(void);

/**@brief		Sets the resistance to a specific level by moving the servo to the 
 *					corresponding position.
 *
 *@note			Levels 0-9 allowable.
 */
uint16_t resistance_level_set(uint8_t level);

/**@brief		Sets the resistance to a value 0-100 percent.
 *
 */
uint16_t resistance_pct_set(float percent);

/**@brief		Adjusts magnetic resistance for erg and simulation modes.
 *
 */
void resistance_adjust(irt_power_meas_t* p_power_meas_first,
		irt_power_meas_t* p_power_meas_current,
		rc_sim_forces_t *p_sim_forces,
		resistance_mode_t resistance_mode,
		float 				rr_force);



#endif //__RESISTANCE_H__
