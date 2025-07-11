#ifndef __BATTERY_INFO_H__
#define __BATTERY_INFO_H__


#include "system.h"



void battery_info_task(void);

void battery_inres_window_ad_full(uint16_t *data,uint16_t len);
void battery_info_calculate_flag_set(void);
uint16_t battery_info_calculate_flag_get(void);

#endif

