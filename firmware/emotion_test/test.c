/*
(1) Launch developer command prompt:
%comspec% /k ""C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\Tools\VsDevCmd.bat""
(2) build with debug symbols.
cl test.c /ZI 


cl test.c ..\emotion\libraries\power.c ..\emotion\libraries\resistance.c ..\emotion\libraries\nrf_pwm.c /ZI /I..\emotion\libraries\
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <float.h>

typedef struct blink_s
{
	uint32_t pin_mask; 			// Mask of pins to toggle
	uint8_t pattern; 			// each bit represents what to in 1 of 8 slots
	uint8_t count : 3; 			// running count of where in the sequence
	uint8_t interval_ms;		// Interval time to repeat in ms.
} blink_t;

// if it's a repeat, save that to the local variable
// restart the timer
// 

static blink_t repeated_blink;

static void blink_handler(void * p_context)
{
	blink_t* p_blink = (blink_t*) p_context;

	printf("count: %u\r\n", p_blink->count++);
	return;
}


int main(int argc, char *argv [])
{
	uint32_t i = 0;
	
	blink_t blink = {
		.pin_mask = (1UL << 18 | 1UL << 23),	
		.pattern = 5, //  0b00000101
		.count = 0 };

	/*uint8_t pins = 10;					// 0b1010;

	uint8_t pins = 14;						// 0b1110;
	uint8_t alt_mask = 8;					// 0b1000;

	printf("not: %i\r\n", pins ^ alt_mask);	// 0b0110
	printf("and: %i\r\n", pins & alt_mask); // 0b1000

	return 0;
	*/ 

	for (i = 0; i < 16; i++)
	{
		if (i & 1)
			printf("{ODD}  ");
		else
			printf("{EVEN} ");
		blink_handler((void*) &blink);
	}

	return 0;
}
