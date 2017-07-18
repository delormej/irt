/*
(1) Launch developer command prompt:
%comspec% /k ""C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\Tools\VsDevCmd.bat""
(2) build with debug symbols.
cl test.c /ZI 

cl test.c ../emotion/libraries/speed_event_fifo.c  /I . /I ../emotion/libraries /ZI

*/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include "speed_event_fifo.h"

speed_event_t buffer[8];

void print_speed(speed_event_t* p_speed) 
{
	printf("Hello World, %i, %i\r\n", 
		p_speed->event_time_2048, 
		p_speed->accum_flywheel_ticks);
}

int main(int argc, char *argv [])
{	
	printf("Any key to start.");
	getchar();

	// init
	event_fifo_t fifo = speed_event_fifo_init(buffer, 8);
	speed_event_t* p_event;

	for (int i = 0; i< 4; i++) {
		speed_event_t speed1 = { .event_time_2048 = 1 * i, .accum_flywheel_ticks = 100 * i };
		p_event = speed_event_fifo_put(&fifo, &speed1);
	}
	
	print_speed(p_event);

	speed_event_t* p_speed = speed_event_fifo_oldest(&fifo);
	print_speed(p_speed);
	p_speed = speed_event_fifo_get(&fifo);
	print_speed(p_speed);

	return 0;
}
