#ifndef __SYSTEM_H__
#define __SYSTEM_H__


#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "stm32f30x.h"
#include "sysTask.h"


#define private
#define public



#if USE_SINE
	#define  WAVE_SAMPLES 		128
	#define  T_DIV_FREQUENCY	562      //72MHz/128
#else
	#define  WAVE_SAMPLES 		200
	#define  T_DIV_FREQUENCY   	360-1    //72MHZ/200
#endif



//typedef enum _BOOL{false=0,true=1}bool;


void system_incTick(void);
uint32_t system_tick_get(void);
bool system_timer_triggered(uint32_t start,uint32_t time);

#endif

