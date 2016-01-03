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
#define HEARTRATE_INVALID			0xFF

#define HIGH_BYTE(word)              		(uint8_t)((word >> 8u) & 0x00FFu)           /**< Get high byte of a uint16_t. */
#define LOW_BYTE(word)               		(uint8_t)(word & 0x00FFu)                   /**< Get low byte of a uint16_t. */


typedef enum __attribute__((packed)) {
	RESERVED = 0,
	ASLEEP_OFF,
	READY,
	IN_USE,
	FINISHED_PAUSED
} fe_state_e;	// Used in FE State bit.

typedef enum __attribute__((packed)) {
	TARGET_AT_POWER = 0,
	TARGET_SPEED_TOO_LOW,
	TARGET_SPEED_TOO_HIGH,
	TARGET_UNDETERMINED
} target_power_e;

typedef struct  __attribute__((packed)) {
	target_power_e	target_power_limits : 2;
	uint8_t			reserved : 2;
} flags_t; // Flags contains target Power Limits. (Table 6-29)

typedef struct __attribute__((packed)) {
	fe_state_e 	state : 3;			// specific this should only occupy 3 bits.
	uint8_t		lap_toggle : 1;
} fe_state_t;

/* Only the virtual speed flag wll ever get set in this structure, so we may just do awawy with it? */
typedef struct __attribute__((packed)) {
	uint8_t		hr_data_source : 2;
	uint8_t		distance_enabled : 1;
	uint8_t		virtual_speed_flag : 1;
} capabilities_t;

typedef struct __attribute__((packed)) {
	uint8_t		bike_power_calibration : 1;
	uint8_t		resistance_calibration : 1;
	uint8_t		user_configuration : 1;
	uint8_t		reserved : 1;
} trainer_status_t; // 4 bits

typedef struct {

	float speed;
	uint8_t distance;	// meters
	uint8_t elapsed_time; // 1/4 seconds

	fe_state_e			fe_state : 3;				// bit field (3 bits)
	uint8_t				lap_toggle : 1;
	uint8_t				distance_enabled : 1;
	uint8_t				virtual_speed_flag : 1;
	target_power_e		target_power_limits : 2;
	uint8_t				bike_power_calibration_required : 1;
	uint8_t				resistance_calibration_required : 1;
	uint8_t				user_configuration_required : 1;	//	};

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

// how to store efficently with assignment (structured)
// how to assign to the message (unstructred ok)

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

uint16_t float_to_int16(float f) {
	uint16_t i = (uint16_t)(f * 1000.0f);
	return i;
}

FEC_Page16* build_page16(irt_context_t* context) {	
	
	// byte 7 uses two nibbles for bit flags.
	

	static FEC_Page16 page;

	uint16_t speed_int = float_to_int16(context->speed);

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
	page.FEState = context->fe_state |			// bits 0-2 
		(context->lap_toggle << 3);				// bit 3 
		
	//page.capabilities = 1;
	//page.FEState = 2;
	
	return &page;
}


int main(int argc, char *argv [])
{	
	//ctx.distance = 205;
	// ctx.fe_state.fe_state = 
	irt_context_t context;
	context.elapsed_time = 154; // # of 1/4 seconds.
	context.distance = 180;
	context.speed = 8.333f;
	context.virtual_speed_flag = 1;
	context.lap_toggle = 1;
	context.fe_state = ASLEEP_OFF;

	FEC_Page16* p_page16 = build_page16(&context);
	print_hex((uint8_t*)p_page16);

	printf("size: %i\r\n", sizeof(fe_state_t));
	printf("size: %i\r\n", sizeof(irt_context_t));
	//printf("value: %i\r\n", page.message.FEState);

	return 0;
}
