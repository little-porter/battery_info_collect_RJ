#ifndef __AVERAGE_FILTER_H__
#define __AVERAGE_FILTER_H__


#include <stdint.h>


float average_filter_float_calculate(float newData, float *window, int *index,int window_size,uint8_t *full) ;
uint16_t average_filter_uint16_calculate(uint16_t newData, uint16_t *window, int *index,int window_size,uint8_t *full) ;



#endif

