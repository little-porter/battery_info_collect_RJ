#ifndef __SYSTEM_H__
#define __SYSTEM_H__


#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "stm32f30x.h"
#include "sysTask.h"


#define private
#define public



//typedef enum _BOOL{false=0,true=1}bool;


void system_incTick(void);
uint32_t system_tick_get(void);
bool system_timer_triggered(uint32_t start,uint32_t time);

#endif

