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

static const uint8_t PIN_SDA[] = {14,1};
static const uint8_t PIN_SCL[] = {15,2};

#define NRF_TWI(TWI)                (TWI==0 ? 101 : 100 )

int main(int argc, char *argv [])
{
		
	printf("pin: %i\r\n", PIN_SDA[1]);
	printf("pin: %i\r\n", NRF_TWI(1));
	return 0;
}
