#ifndef __RESISTANCE_H__
#define __RESISTANCE_H__

#include <stdint.h>

#define MAX_RESISTANCE_LEVELS 10

uint32_t RESISTANCE_LEVEL[MAX_RESISTANCE_LEVELS] = { 
	2107, // 0 - no resistance
	1300,	// this was 1273
	1272,	
	1252,
	1212,
	1150,
	1068,
	966,
	843,
	699 };

#endif
