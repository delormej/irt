/*
(1) Launch developer command prompt:
%comspec% /k ""C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\Tools\VsDevCmd.bat""
(2) build with debug symbols.
cl test.c /ZI 

cl test.c ../emotion/libraries/speed_event_fifo.c  /I . /I ../emotion/libraries /ZI

*/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>


int main(int argc, char *argv [])
{
	bool answer = false;

	// button pressed, raw value pressed.
	bool desired_state = 1;
	uint32_t irt_button_pressed = 0;

	answer = ( (desired_state) == !(irt_button_pressed) );
	printf("answer: %i\r\n", answer);

	// button not pressed, raw value not pressed.
	desired_state = 0;
	irt_button_pressed = 1;

	answer = ( (desired_state) == !(irt_button_pressed) );	
	printf("answer: %i\r\n", answer);

	// button not pressed, raw value pressed
	desired_state = 0;
	irt_button_pressed = 0;

	answer = ( (desired_state) == !(irt_button_pressed) );	
	printf("answer: %i\r\n", answer);

	// button pressed, raw value not pressed.
	desired_state = 1;
	irt_button_pressed = 1;

	answer = ( (desired_state) == !(irt_button_pressed) );	
	printf("answer: %i\r\n", answer);


	// When we want it to be pressed, and it is pressed:
	// 		( (desired_state = 1) == !(irt_button_pressed = 0) ) == 1
	//
	// When we want it to be released, and it is not pressed:
	// 		( (desired_state = 0) == !(irt_button_pressed = 1) ) == 1
}
