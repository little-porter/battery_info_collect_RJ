#include "average_filter.h"





/**************************************************************************************************************
	float类型窗口，滑动平均滤波函数，剔除最大、最小异常值
**************************************************************************************************************/
float average_filter_float_calculate(float newData, float *window, int *index,int window_size,uint8_t *full) 
{
    // 如果缓冲区已满，则从总和中减去最早的元素
	double max = 0,min =0, sum = 0;
		
    // 更新缓冲区和总和
    window[(*index) % window_size] = newData;
	(*index)++;
	max = min = window[0];
	
	for(uint8_t i = 0; i < window_size;i++)
	{
		sum += window[i];
		if(max < window[i])
		{
			max = window[i];
		}
		if(min > window[i])
		{
			min = window[i];
		}
	}
    
	if((*index) == window_size)
	{
		*full = 1;
		(*index) = 0;
	}
    
    // 计算平均值
    if (*full == 0) {
        sum = sum / *index; // 在窗口未填满之前，取实际数量的平均值
    } else {
		sum  = sum - max - min;
        sum  = sum / (window_size-2);
    }
	
	return (float)sum;
}

/**************************************************************************************************************
	uint16_t类型窗口，滑动平均滤波函数，剔除最大、最小异常值
**************************************************************************************************************/
uint16_t average_filter_uint16_calculate(uint16_t newData, uint16_t *window, int *index,int window_size,uint8_t *full) 
{
    // 如果缓冲区已满，则从总和中减去最早的元素
	uint16_t max = 0,min =0; 
	uint32_t sum = 0;
		
    // 更新缓冲区和总和
    window[(*index) % window_size] = newData;
	(*index)++;
	max = min = window[0];
	
	for(uint8_t i = 0; i < window_size;i++)
	{
		sum += window[i];
		if(max < window[i])
		{
			max = window[i];
		}
		if(min > window[i])
		{
			min = window[i];
		}
	}
    
	if((*index) == window_size)
	{
		*full = 1;
		(*index) = 0;
	}
    
    // 计算平均值
    if (*full == 0) {
        sum = sum / *index; // 在窗口未填满之前，取实际数量的平均值
    } else {
		sum  = sum - max - min;
        sum  = sum / (window_size-2);
    }
	
	return (uint16_t)sum;
}




