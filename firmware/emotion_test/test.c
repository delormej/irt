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

#define GRAVITY 9.8f;

#define debug_log printf

#ifndef DM_DIABLE_LOGS
#define DM_LOG debug_log   /**< Used for logging details. */
#else // DM_DIABLE_LOGS
#define DM_LOG(...)        /**< Diasbles detailed logs. */
#endif // DM_DIABLE_LOGS

#define IRT_ERROR_BASE_NUM      (0x80000)       				///< Error base, hopefully well away from anything else.
#define IRT_ERROR_RC_BASE_NUM   IRT_ERROR_BASE_NUM + (0x100)   	///< Error base for Resistance Control


int16_t calc_watts(float grade, float speed_mps, float weight_kg, float _c, float _crr)
{
	float f_wind;
	float f_rolling;
	float f_slope;
	float result;

	f_wind = 0.5f * (_c * pow(speed_mps, 2));

	// Weight * GRAVITY * Co-efficient of rolling resistance.
	f_rolling = (weight_kg * 9.8f * _crr);

	f_slope = (weight_kg * 9.8f * grade);

	result = ((f_wind + f_rolling + f_slope) * speed_mps);
	printf("f_wind:%f, f_rolling:%f, f_slope:%f\r\n", 
		f_wind, f_rolling, f_slope);

	return (int16_t) ((f_wind + f_rolling + f_slope) * speed_mps);
}

void check(uint32_t err)
{
	if ((err & IRT_ERROR_RC_BASE_NUM) == IRT_ERROR_RC_BASE_NUM)
	{
		printf("Yes\r\n");
	}
	else
	{
		printf("No\r\n");
	}
}

int main(int argc, char *argv [])
{
	int16_t watts;
	
	watts = calc_watts(0.019f, 5.811f, 81.81f, 0.6f, 0.0033f);
	printf("[LOG] Watts == %i\r\n", watts);
	/*
	check(IRT_ERROR_RC_BASE_NUM);	// 1000 0000 0001 0000 0000
	check(12213);					//		  10 1111 1011 0101

	DM_LOG("[LOG] Testing\r\n");
	printf("Test DONE\r\n");
	*/
}
