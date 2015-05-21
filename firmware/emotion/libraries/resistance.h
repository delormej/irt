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
#include <float.h>
#include "irt_common.h"
#include "user_profile.h"

#define RESISTANCE_LEVELS 		resistance_level_count() 	// Number of resistance levels available.
#define ERG_ADJUST_MINOR		1U							// Percent watts to adjust increment /decrement for minor step
#define ERG_ADJUST_MAJOR		10U							// Percent watts to adjust increment /decrement for major step
#define ERG_ADJUST_MIN			50U							// Erg doesn't go below this watt level.
#define DEFAULT_ERG_WATTS				175u				// Default erg target_watts when not otherwise set.
#define RESISTANCE_MIN_SPEED_ADJUST		3.0f				// (~6.71mph) Minimum speed in meters per second at which we adjust resistance.

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
	RESISTANCE_SET_CRR			= 0x44, // Co-efficient of rolling resistance
	RESISTANCE_SET_C			= 0x45, // Wind resistance offset.
	RESISTANCE_SET_SLOPE		= 0x46,
	RESISTANCE_SET_WIND			= 0x47,
	RESISTANCE_SET_WHEEL_CR		= 0x48,
	RESISTANCE_INIT_SPINDOWN	= 0x49,
	RESISTANCE_READ_MODE      	= 0x4A,
	RESISTANCE_SPINDOWN_RESULT	= 0x5A,
	RESISTANCE_SET_SERVO_POS 	= 0x5B,	// A new one that I'm adding for testing.
	RESISTANCE_SET_WEIGHT 		= 0x5C,	// A new one that I'm adding for testing.
	RESISTANCE_START_STOP_TR	= 0x65	// Indicator to Trainer Road to stop/start.
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

/**@brief 		Resistance control event payload.
 * 				TODO: this belongs in a separate module responsible for decoding
 * 				resistance control messages.
 */
typedef struct
{
	resistance_mode_t 	operation;				// Operation to perform or mode to set for resistance.
	uint8_t				*pBuffer; 				// Pointer to values required for the operation.
} rc_evt_t;

/**@brief Set resistance event handler type. */
typedef void(*rc_evt_handler_t) (rc_evt_t rc_evt);

/**@brief Factors used when calculating simulation forces. */
typedef struct
{
	resistance_mode_t mode;
	uint16_t 	servo_position;
	float 		crr;
	float 		c;
	float 		wind_speed_mps;
	float 		grade;
	int16_t 	erg_watts;
	int16_t 	unadjusted_erg_watts;	// Unadjusted, original erg target.
	uint8_t 	level;
	uint8_t		adjust_pct;				// Tracks a percentage of adjustment 0-254%
} irt_resistance_state_t;

/**@brief	Initializes the resistance module which controls the servo.
 *
 */
irt_resistance_state_t* resistance_init(uint32_t servo_pin_number, user_profile_t* p_user_profile);

/**@brief		Gets the current resistance state object.
 *
 */
irt_resistance_state_t* resistance_state_get(void);

/**@brief		Sets the position of the servo.
 *
 */
uint16_t resistance_position_set(uint16_t position, bool smooth);

/**@brief		Validates the values of positions are in range.
 *
 */
bool resistance_positions_validate(servo_positions_t* positions);

/**@brief		Sets the resistance to a specific level by moving the servo to the 
 *					corresponding position.
 *
 *@note			Levels 0-9 allowable.
 */
uint16_t resistance_level_set(uint8_t level);

/**@brief		Gets the levels of standard resistance available.
  */
uint8_t resistance_level_count(void);

/**@brief		Sets the resistance to a value 0-100 percent.
 *
 */
uint16_t resistance_pct_set(float percent);

/**@brief		Sets erg mode with a watt target.
 *
 */
void resistance_erg_set(uint16_t watts);

/**@brief		Sets simulation grade.
 *
 */
void resistance_grade_set(float grade);

/**@brief		Sets simulation wind speed.
 *
 */
void resistance_windspeed_set(float windspeed_mps);

/**@brief		Sets simulation coefficient of drag.
 *
 */
void resistance_c_set(float drag);

/**@brief		Sets simulation coefficient of rolling resistance.
 *
 */
void resistance_crr_set(float crr);

/**@brief		Adjusts dynamic magnetic resistance control based on current
 * 				speed and watts.
 *
 */
void resistance_adjust(float speed_mps, int16_t magoff_watts);

/**@brief		Adjusts resistance +/- by either a step or % (erg mode).
 *
 */
uint32_t resistance_step(bool increment, bool minor_step);


#endif //__RESISTANCE_H__
