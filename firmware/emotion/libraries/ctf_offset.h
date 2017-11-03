/* Copyright (c) 2017 Inside Ride Technologies, LLC. All Rights Reserved.
*/
#ifndef ANT_CTF_OFFSET_H__
#define ANT_CTF_OFFSET_H__

#include <stdint.h>

void addCtfOffsetSample(uint16_t value);
void resetCtfOffsetSamples();
float getCtfOffset();

#endif	// ANT_CTF_OFFSET_H__
