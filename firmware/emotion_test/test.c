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

int main(int argc, char *argv [])
{
#define DEVICE_NAME "E-Motion (DFU)"
	printf("%i\r\n",strlen(DEVICE_NAME));
	
	bool dfu_start = 0;
	dfu_start = 1;
	dfu_start |= false;

	if (dfu_start)
		printf("true");
	else
		printf("false");
	
	return;

	// E-Motion-DFU
	// DdfuTarg
}
