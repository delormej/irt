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

#define debug_log printf

#ifndef DM_DIABLE_LOGS
#define DM_LOG debug_log   /**< Used for logging details. */
#else // DM_DIABLE_LOGS
#define DM_LOG(...)        /**< Diasbles detailed logs. */
#endif // DM_DIABLE_LOGS

#define IRT_ERROR_BASE_NUM      (0x80000)       				///< Error base, hopefully well away from anything else.
#define IRT_ERROR_RC_BASE_NUM   IRT_ERROR_BASE_NUM + (0x100)   	///< Error base for Resistance Control


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
	check(IRT_ERROR_RC_BASE_NUM);	// 1000 0000 0001 0000 0000
	check(12213);					//		  10 1111 1011 0101

	DM_LOG("[LOG] Testing\r\n");
	printf("Test DONE\r\n");
}
