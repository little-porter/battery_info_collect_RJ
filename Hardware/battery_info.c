#include "battery_info.h"

#include "sysTask.h"
#include "average_filter.h"
#include "modbus.h"



#define INRES_INPUT_WINDOW_SIZE		128
#define INRES_WINDOW_SIZE			10
int inres_index = 0;
uint8_t  inres_full = 0;
uint16_t calculate_flag;
static float inres = 0;

uint16_t inres_input_window_ad[INRES_INPUT_WINDOW_SIZE];

float    inres_window[INRES_WINDOW_SIZE];


void battery_info_calculate_flag_set(void)
{
	calculate_flag = 1;
}

uint16_t battery_info_calculate_flag_get(void)
{
	return calculate_flag;
}

void bubbleSort(uint16_t arr[], int n) {
    int i, j; 
	uint16_t temp;
    // 外层循环遍历每一个元素
    for (i = 0; i < n-1; i++) {     
        // 内层循环进行相邻元素的比较和交换
        for (j = i+1; j < n; j++) { 
            if (arr[i] > arr[j]) {
                // 交换操作
                temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
    }
}

void battery_inres_window_ad_full(uint16_t *data,uint16_t len)
{
	uint16_t fft_len = 0;
	uint16_t (*ptr)[2];
	ptr = (uint16_t (*)[2])data;
	if(len < INRES_INPUT_WINDOW_SIZE){
		fft_len = len;
	}else{
		fft_len = INRES_INPUT_WINDOW_SIZE;
	}
//	memcpy(fft_input_window_ad,data,fft_len*2);
	for(int i=0;i<fft_len;i++)
	{
		inres_input_window_ad[i] = ptr[i][0];
	}
}


//void battery_inres_window_full(void)
//{
//	memset(fft_input_window,0,FFT_WINDOW_SIZE);
//	for(int i=0; i<FFT_WINDOW_SIZE; i++)
//	{
//		fft_input_window[i] = fft_input_window_ad[i]*3.3/4095;
//	}
//	
//}

void battery_inres_calculate(void)
{
	uint16_t h_value[128] = {0};
	uint16_t l_value[128] = {0};
	int h_pos = 0,l_pos = 0;
	uint32_t h_sum = 0,l_sum = 0;
	
	for(int i=0;i<INRES_INPUT_WINDOW_SIZE;i++){
		if(inres_input_window_ad[i] > 2047)
		{
			h_value[h_pos++] = inres_input_window_ad[i];
		}else{
			l_value[l_pos++] = inres_input_window_ad[i];
		}
	}
	bubbleSort(h_value,h_pos);
	bubbleSort(l_value,l_pos);
	if(h_pos > 40){
		for(int i=10;i<(h_pos-25);i++){
			h_sum += h_value[i];
		}
		h_sum = h_sum/(h_pos-35);
	}else{
		return;
	}
	
	if(l_pos > 40){
		for(int i=25;i<(l_pos-10);i++){
			l_sum += l_value[i];
		}
		l_sum = l_sum/(l_pos-35);
	}else{
		return;
	}
	
	float v = (h_sum-l_sum)*3.3f/4096;
	
	inres = v/201/0.05f;
	
	inres = average_filter_float_calculate(inres,inres_window,&inres_index,INRES_WINDOW_SIZE,&inres_full);
	uint16_t u16_inres = inres*1000;
	
	modbus_reg_write(INRES_REG_ADDR,&u16_inres,1);
}

void battery_info_task_callback(void *param)
{
	if(!calculate_flag)  return;
	calculate_flag = 0;
	
	battery_inres_calculate();
}

void battery_info_task(void)
{
	static uint32_t start_time = 0,time = 500;
	static uint8_t  start_flag = 0;
	
	if(start_flag == 0){
		start_time = system_tick_get();
		start_flag = 1;
	}
	
	if(system_timer_triggered(start_time,time)){
		start_flag = 0;
		sysTask_publish(battery_info_task_callback,NULL);
	}
	
}
