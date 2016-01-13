/*
(1) Launch developer command prompt:
%comspec% /k ""C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\Tools\VsDevCmd.bat""
(2) build with debug symbols.
cl test.c /ZI 

cl test.c ../emotion/libraries/math/acosf.c ../emotion/libraries/math/sqrtf.c ../emotion/libraries/math/cosf.c ../emotion/libraries/math/sinf.c ../emotion/libraries/math/fabsf.c ../emotion/libraries/math/rem_pio2f.c /I ../emotion/libraries/math

*/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <float.h>
#include <math.h>

#define HR_DATA_SOURCE 				0	
#define DISTANCE_TRAVELED_ENABLED	1	
#define EQUIPMENT_TYPE				25
#define CADENCE_INVALID				0xFF
#define HEARTRATE_INVALID			0xFF
#define INCLINE_INVALID				0x7FFF
#define RESISTANCE_INVALID			0xFF

#define HIGH_BYTE(word)              		(uint8_t)((word >> 8u) & 0x00FFu)           /**< Get high byte of a uint16_t. */
#define LOW_BYTE(word)               		(uint8_t)(word & 0x00FFu)                   /**< Get low byte of a uint16_t. */

#define FESTATE_CONTEXT(context)			(context->fe_state | (context->lap_toggle << 3))			

#define CAPABILITIES_CONTEXT(context) 	\
		(HR_DATA_SOURCE |	\	
		(DISTANCE_TRAVELED_ENABLED << 2) |	\	
		(context->virtual_speed_flag << 3))   	

#define FLYWHEEL_TICK_PER_REV		2																// # of ticks per flywheel revolution.
#define FLYWHEEL_ROAD_DISTANCE		0.115f															// Virtual road distance traveled in meters per complete flywheel rev.
#define FLYWHEEL_TICK_PER_METER		((1.0f / FLYWHEEL_ROAD_DISTANCE) * FLYWHEEL_TICK_PER_REV)		// # of flywheel ticks per meter

typedef enum  {
	FE_RESERVED = 0,
	FE_ASLEEP_OFF,
	FE_READY,
	FE_IN_USE,
	FE_FINISHED_PAUSED
} fe_state_e;	// Used in FE State bit.

typedef enum __attribute__((packed)) {
	TARGET_AT_POWER = 0,
	TARGET_SPEED_TOO_LOW,
	TARGET_SPEED_TOO_HIGH,
	TARGET_UNDETERMINED
} target_power_e;


/*
typedef struct  __attribute__((packed)) {
	target_power_e	target_power_limits : 2;
	uint8_t			reserved : 2;
} flags_t; // Flags contains target Power Limits. (Table 6-29)


typedef struct __attribute__((packed)) {
	fe_state_e 	state : 3;			// specific this should only occupy 3 bits.
	uint8_t		lap_toggle : 1;
} fe_state_t; */

/* Only the virtual speed flag wll ever get set in this structure, so we may just do awawy with it? 
typedef struct __attribute__((packed)) {
	uint8_t		hr_data_source : 2;
	uint8_t		distance_enabled : 1;
	uint8_t		virtual_speed_flag : 1;
} capabilities_t; */

/*
typedef struct __attribute__((packed)) {
	uint8_t		bike_power_calibration : 1;
	uint8_t		resistance_calibration : 1;
	uint8_t		user_configuration : 1;
	uint8_t		reserved : 1;
} trainer_status_t; // 4 bits */

#define RESISTANCE_SET_PERCENT		0x40
#define RESISTANCE_SET_STANDARD		0x41

typedef struct user_profile_s {
	uint8_t		version;					// Version of the profile for future compatibility purposes.
	uint8_t		reserved;					// Padding (word alignment size is 16 bits).
	uint16_t	settings;					// Bitmask of feature/settings to turn on/off.
	uint16_t	total_weight_kg;			// Stored in int format 1/100, e.g. 8181 = 81.81kg
	uint16_t	wheel_size_mm;
	uint16_t	ca_slope;					// Calibration slope.  Stored in 1/1,000 e.g. 20741 = 20.741
	uint16_t	ca_intercept;				// Calibration intercept. This value is inverted on the wire -1/1,000 e.g. 40144 = -40.144
	uint16_t	ca_temp;					// need Temperature recorded when calibration was set.  See: Bicycling Science (1% drop in Crr proportional to 1 degree change in temp).
	uint16_t	ca_reserved;				// Placeholder for another calibration value if necessary.
	int16_t		servo_offset;				// Calibration offset for servo position.
	//servo_positions_t servo_positions;		// Servo positions (size should be 21 bytes)
	float		ca_drag;					// Calibration co-efficient of drag which produces the "curve" from a coastdown.
	float		ca_rr;						// Co-efficient of rolling resistance.
	//mag_calibration_factors_t ca_mag_factors; // Magnet calibration factors.
	//uint8_t		reserved_2[7]; // (sizeof(servo_positions_t)+2) % 16];					// For block size alignment -- 16 bit alignment
} user_profile_t;

typedef struct {
	// Existing fields.
	float			instant_speed_mps;
	int16_t			instant_power;         										// Note this is a SIGNED int16
	uint8_t 		distance;	// meters	
	uint8_t 	resistance_mode;
	uint16_t	resistance_level;
	uint16_t	servo_position;
	
	//
	// New state to track.
	//
	
	uint8_t 		elapsed_time; // 1/4 seconds

	fe_state_e		fe_state : 3;				// bit field (3 bits)
	uint8_t			lap_toggle : 1;
	uint8_t			virtual_speed_flag : 1;
	target_power_e	target_power_limits : 2;
	uint8_t			bike_power_calibration_required : 1;
	uint8_t			resistance_calibration_required : 1;
	uint8_t			user_configuration_required : 1;	//	};

} irt_context_t;

typedef struct {
	uint8_t 	DataPageNumber;
	uint8_t 	EquipmentType;
	uint8_t 	ElapsedTime;
	uint8_t 	Distance;
	uint8_t 	SpeedLSB;
	uint8_t 	SpeedMSB;
	uint8_t 	HeartRate;
	/* On the wire FEState bits appear in most significant nibble, before capabilities
	   nibble. bit 0 of EACH nibble (4 bits) is the right most bit (least significant). */
	uint8_t		capabilities : 4;
	uint8_t		FEState : 4;
	
} FEC_Page16; // General FE Data Page

typedef struct {
	uint8_t 	DataPageNumber;
	uint8_t 	Reserved[2];
	uint8_t 	CycleLength;				// Wheel Circumference on a Trainer in meters. 0.01 - 2.54m
	uint16_t 	Incline;                    // Not used, send 0x7FFF.
	uint8_t		ResistanceLevelFEC; 		// Percentage of maximum applicable resitsance (0-100%)
	uint8_t		Capabilities:4; 			// Reserved for future, set to: 0x0
	uint8_t		FEState:4;					//  
} FEC_Page17; // General Settings Page

typedef struct {
	uint8_t 	DataPageNumber;
	uint8_t		UpdateEventCount;
	uint8_t		InstantCadence;
	uint8_t		AccumulatedPowerLSB;
	uint8_t		AccumulatedPowerMSB;
	uint8_t		InstantPowerLSB;
	uint8_t		InstantPowerMSB:4;			// Uses 1.5 bytes
	uint8_t		TrainerStatusBit:4;			
	uint8_t		Flags:4;
	uint8_t		FEState:4;
} FEC_Page25; // Specific Trainer Data Page

// Page 54

// Page 55
typedef struct {
	uint8_t		DataPageNumber;
	uint8_t		UserWeightLSB;
	uint8_t		UserWeightMSB;
	uint8_t		Reserved;
	uint8_t		WheelDiameterOffset : 4;		// units: 1mm, range: 0 - 10 mm.  No offset = 0xF.
	uint8_t		BikeWeightLSN : 4;				// units: 0.05kg, range: 0 - 50 kg Least significant nibble.
    uint8_t     BikeWeightMSB;					// Most significant byte.
	uint8_t		WheelDiameter;					// units: 0.01m, range: 0 - 2.54m
	uint8_t		GearRatio;						// Invalid 0.00, units: 0.03, range 0.03 - 7.65    
} FEC_Page55; // FE User Configuration

// how to store efficently with assignment (structured)
// how to assign to the message (unstructred ok)


/**@brief		Gets the percentage of total resistance at a given servo position.
 * @returns		Percentage of maximum resistance (Units: 0.5%, Range: 0-100%).

uint8_t resistance_pct_get(uint16_t position) {
	float resistance_pct =  
		(float) ((1600 - position) / (1600 - 800.0)) * 200;
} */
uint8_t resistance_pct_get(uint16_t position)
{
	float resistance_pct = 0.0f;
    uint8_t pct = 0;
    if (position != 2000)
    {
        // Subtract position from min position, divide by full range,
        // multiply by 200 to remove the decimal and represent units of 0.5%.
        /*resistance_pct =  
            (float)((1600 - position) /
            (float)(1600 - 800)) * 200;*/
            
        pct =  
            ((1600 - position) * 200) /
            ((1600 - 800) * 200);            
    }	
    
	return pct; //(uint8_t)resistance_pct;
}

void print_hex(uint8_t* buffer) {
	printf("[BUFFER] [%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x]\r\n",
			buffer[0],
			buffer[1],
			buffer[2],
			buffer[3],
			buffer[4],
			buffer[5],
			buffer[6],
			buffer[7]);
}

uint16_t speed_mps_to_int16(float f) {
	uint16_t i = (uint16_t)(f * 1000.0f);
	return i;
}

FEC_Page16* build_page16(irt_context_t* context) {	

	static FEC_Page16 page;

	uint16_t speed_int = speed_mps_to_int16(context->instant_speed_mps);

	page.DataPageNumber = 16;
	page.EquipmentType = EQUIPMENT_TYPE;
	page.ElapsedTime = context->elapsed_time;
	page.Distance = context->distance;
	page.SpeedLSB = LOW_BYTE(speed_int);
	page.SpeedMSB = HIGH_BYTE(speed_int);
	page.HeartRate = HEARTRATE_INVALID;
	page.capabilities = 
		HR_DATA_SOURCE | 						// bits 0-1
		(DISTANCE_TRAVELED_ENABLED << 2) | 		// bit 2
		(context->virtual_speed_flag << 3);   	// bit 3 
/* page.FEState = context->fe_state |			// bits 0-2 
		(context->lap_toggle << 3);				// bit 3 */ 
	page.FEState = FESTATE_CONTEXT(context);
	
	return &page;
}

FEC_Page17* build_page17(irt_context_t* context, user_profile_t* profile) {
	
	static FEC_Page17 page;
	
	page.DataPageNumber = 17;
	page.CycleLength = (uint8_t)(2071 / 10);	// Convert wheel centimeters.

	// In basic modes, calculate percentage.
	switch (context->resistance_mode) 
	{
		case RESISTANCE_SET_STANDARD:
		case RESISTANCE_SET_PERCENT:
			page.ResistanceLevelFEC = resistance_pct_get(context->servo_position);
		default:
			page.ResistanceLevelFEC = RESISTANCE_INVALID;
	} 

	page.Capabilities = CAPABILITIES_CONTEXT(context);
	page.FEState = FESTATE_CONTEXT(context);
	
	return &page;	
}

FEC_Page25* build_page25(irt_context_t* context) {
	
	static uint8_t event_count = 6;
	static uint16_t accumulated_power = 14031;
	static FEC_Page25 page;

	// Increment static fields.
	event_count++;
	accumulated_power += context->instant_power;
	
	// Update the page.
	page.DataPageNumber = 25;
	page.UpdateEventCount = event_count;
	page.InstantCadence = CADENCE_INVALID;
	page.AccumulatedPowerLSB = LOW_BYTE(accumulated_power);
	page.AccumulatedPowerMSB = HIGH_BYTE(accumulated_power);
	/* 1.5 bytes used for instantaneous power.  Full 8 bits for byte LSB
	   only 4 bits (0-3) used for the MSB. */
	page.InstantPowerLSB = LOW_BYTE(context->instant_power);
	page.InstantPowerMSB = HIGH_BYTE(context->instant_power) & 0x0F;
	
	/* MSB (bit 3) reserved, 3 bits used for calibration required flags */
	page.TrainerStatusBit = 
		context->bike_power_calibration_required | 			// bit 0
		(context->resistance_calibration_required << 1) |	// bit 1
		(context->user_configuration_required << 2) |		// bit 2
		(0 << 3);											// bit 3 - reserved 
	page.Flags = context->target_power_limits;
/*	page.FEState = context->fe_state |						// bits 0-2 
		(context->lap_toggle << 3);							// bit 3 */
	page.FEState = FESTATE_CONTEXT(context);
	
	return &page;
} 

uint8_t speed_distance_get(void) 
{
    return (uint8_t)(0 / FLYWHEEL_TICK_PER_METER);     
}


static FEC_Page55* UserConfigurationPage_Send(uint16_t bike_weight)
{
    static FEC_Page55 page;

    // DEFAULT_TOTAL_WEIGHT_KG

    // Unit for bike weight is 0.05kg, 0x0FFF == 50kg, 
    // calculate binary value by dividing by 5.
    bike_weight = (uint16_t)(bike_weight / 5u);
    
    // Bike weight == 1.5 bytes.
    page.BikeWeightMSB = (uint8_t)((bike_weight >> 4) & 0x0FF);
    page.BikeWeightLSN = (uint8_t)(bike_weight & 0x000F);

    return &page;
}


static float track_grade_get(uint16_t raw_grade)
{
	// 6% grade: 20600
	// (800 * 0.01) - 200
	
    // Simulated Grade (%) = (Raw Grade Value x 0.01%) – 200.00%
    float grade = (raw_grade * 0.01f) - 200.0f;    
    
    return grade; // / 100;
}


int main(int argc, char *argv [])
{
	uint8_t buffer = 80u;
	float crr = (float)(buffer * 0.00005f);
	
	printf("grade (-200%):\t%.3f\r\n", track_grade_get(0x0));
	printf("grade (200%):\t%.2f\r\n", track_grade_get(0x9c40));
	printf("crr (0%):\t%.5f\r\n", crr);
	
	printf("grade:\t%.4f\r\n", track_grade_get(20601) / 100.0f);
    return;
    
    /*
    uint8_t buffer[2] = { 0x30, 0x00 };
    printf("size: %.2f\r\n", (float)(buffer[0] / 2.55f));
    	
    return 0;
    
	// setup some dummy context. 
	irt_context_t context;
	context.elapsed_time = 154; // # of 1/4 seconds.
	context.distance = 180;
	context.instant_speed_mps = 8.333f;
	context.instant_power = 304;
	context.virtual_speed_flag = 1;
	context.lap_toggle = 0;
	context.fe_state = FE_IN_USE;
	context.user_configuration_required = 1;
	context.target_power_limits = TARGET_SPEED_TOO_HIGH;

	context.resistance_mode = RESISTANCE_SET_STANDARD; 
	context.resistance_level = 3;

    FEC_Page17 page = { .DataPageNumber = 17, .Reserved[0] = 0xFF, .Reserved[1] = 0xFF, .Incline = 0x7FFF };
    page.ResistanceLevelFEC = resistance_pct_get(1200);

    print_hex((uint8_t*)&page);	
    */
    printf("distance: %i\r\n", speed_distance_get());
    

	return 0;
}
