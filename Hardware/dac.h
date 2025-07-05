#ifndef _DAC_H_
#define _DAC_H_

#include "system.h"

#include "stdint.h"
#include "math.h"

#define  SINE_WAVE_SAMPLES 128
#define  SINE_WAVE_FREQ 1000
#define  SINE_RATE 1000
#define  M_PI 3.14159265358979323846

extern uint16_t sineWave1[SINE_WAVE_SAMPLES];


void dac_init(void);

#endif
