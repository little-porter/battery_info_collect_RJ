#include "average_filter.h"





/**************************************************************************************************************
	float���ʹ��ڣ�����ƽ���˲��������޳������С�쳣ֵ
**************************************************************************************************************/
float average_filter_float_calculate(float newData, float *window, int *index,int window_size,uint8_t *full) 
{
    // �������������������ܺ��м�ȥ�����Ԫ��
	double max = 0,min =0, sum = 0;
		
    // ���»��������ܺ�
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
    
    // ����ƽ��ֵ
    if (*full == 0) {
        sum = sum / *index; // �ڴ���δ����֮ǰ��ȡʵ��������ƽ��ֵ
    } else {
		sum  = sum - max - min;
        sum  = sum / (window_size-2);
    }
	
	return (float)sum;
}

/**************************************************************************************************************
	uint16_t���ʹ��ڣ�����ƽ���˲��������޳������С�쳣ֵ
**************************************************************************************************************/
uint16_t average_filter_uint16_calculate(uint16_t newData, uint16_t *window, int *index,int window_size,uint8_t *full) 
{
    // �������������������ܺ��м�ȥ�����Ԫ��
	uint16_t max = 0,min =0; 
	uint32_t sum = 0;
		
    // ���»��������ܺ�
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
    
    // ����ƽ��ֵ
    if (*full == 0) {
        sum = sum / *index; // �ڴ���δ����֮ǰ��ȡʵ��������ƽ��ֵ
    } else {
		sum  = sum - max - min;
        sum  = sum / (window_size-2);
    }
	
	return (uint16_t)sum;
}




