#include "battery_info.h"

#include "sysTask.h"
#include "average_filter.h"
#include "modbus.h"
#include "adc.h"


#define INRES_INPUT_WINDOW_SIZE		WAVE_SAMPLES
#define INRES_WINDOW_SIZE			10
#define VOLTAGE_WINDOW_SIZE			10
private int inres_index = 0;
private uint8_t  inres_full = 0;
private uint16_t calculate_flag;

private uint16_t inres_input_window_ad[INRES_INPUT_WINDOW_SIZE];

private float    inres_window[INRES_WINDOW_SIZE];

private int voltage_H_index = 0,voltage_L_index = 0;
private uint8_t  voltage_H_full = 0,voltage_L_full = 0;
private uint16_t voltage_low_window[VOLTAGE_WINDOW_SIZE];
private uint16_t voltage_high_window[VOLTAGE_WINDOW_SIZE];


private static float inres = 0;
private static float voltage = 0;

typedef enum _battery_process
{
	BATTERY_PROCESS_INRES = 0,
	BATTERY_PROCESS_VOLTAGE,
}battery_process_t;

private battery_process_t battery_process = BATTERY_PROCESS_VOLTAGE;


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
	uint16_t h_value[INRES_INPUT_WINDOW_SIZE] = {0};
	uint16_t l_value[INRES_INPUT_WINDOW_SIZE] = {0};
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
	if(h_pos > 50){
		for(int i=20;i<(h_pos-20);i++){
			h_sum += h_value[i];
		}
		h_sum = h_sum/(h_pos-40);
	}else{
		return;
	}
	
	if(l_pos > 50){
		for(int i=20;i<(l_pos-20);i++){
			l_sum += l_value[i];
		}
		l_sum = l_sum/(l_pos-40);
	}else{
		return;
	}
	
	float v = (h_sum-l_sum)*3.3f/4096;
	
	inres = v/201/0.05f;
	
	inres = average_filter_float_calculate(inres,inres_window,&inres_index,INRES_WINDOW_SIZE,&inres_full);
	uint16_t u16_inres = 0xFFFF;
	if(inres < 0.300){
		u16_inres = inres*1000;
	}else{;}
	
	modbus_reg_write(INRES_REG_ADDR,&u16_inres,1);
}


void battery_voltage_calculate(void)
{
	uint16_t v1=0,v2=0,v_l_ad=0,v_h_ad=0;
	float v = 0,v_h=0,v_l=0,v_average;						//电池电压
	v1 = get_v_high_ad_value();
//	v2 = get_v_low_ad_value();
	v1 = average_filter_uint16_calculate(v1,voltage_high_window,&voltage_H_index,VOLTAGE_WINDOW_SIZE,&voltage_H_full);
//	v2 = average_filter_uint16_calculate(v2,voltage_low_window,&voltage_L_index,VOLTAGE_WINDOW_SIZE,&voltage_L_full);
	
//	if(v1 < v2){
//		v_l_ad = v1;
//		v_h_ad = v2;
//	}else{
//		v_l_ad = v2;
//		v_h_ad = v1;
//	}
	v_h_ad = v1;
	
	v_h = v_h_ad*3.3/4096;
//	v_l = v_l_ad*3.3/4096;
//	
//	v = (v_h-v_l)*12;
	
	uint16_t v_reg = 0xFFFF;
	
	if(v_h > 1.7){
		v = (v_h - 1.65) * 12 + 1.65 - 0.05;
		v_reg = v*1000;
	}else{
		v = 0.0f;
//		v =19.8 - v_h * 12 - 1.65;
	}
	
	modbus_reg_write(VOLTAGE_REG_ADDR,&v_reg,1);
 	voltage = v;
}


void battery_info_task_callback(void *param)
{
	
//	switch(battery_process)
//	{
//		case BATTERY_PROCESS_INRES:
//			if(!calculate_flag)  return;
//				calculate_flag = 0;
//			battery_inres_calculate();
//			TIM_Cmd(TIM2, DISABLE);
//			battery_process = BATTERY_PROCESS_VOLTAGE;
//			break;
//		case BATTERY_PROCESS_VOLTAGE:
//			TIM_Cmd(TIM2, ENABLE);
//			battery_voltage_calculate();
//			battery_process = BATTERY_PROCESS_INRES;
//			break;
//		default:
//			battery_process = BATTERY_PROCESS_VOLTAGE;
//			break;
//		
//	}
	
	if(!calculate_flag)  return;
				calculate_flag = 0;
	battery_inres_calculate();
	battery_voltage_calculate();
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
