#include "stm32f30x.h"
#include "mcu_init.h"
#include "adc.h"
#include "dac.h"
#include "main.h"
#include "arm_math.h"
#include "arm_const_structs.h"
#include "GZP6816D.h"
#include "gxht30.h"
#include "usart.h"
#include "flash_bsp.h"
#include "config.h"
#include "gas.h"


#include "sysTask.h"
#include "led.h"
#include "timer.h"
#include "adc.h"
#include "uart.h"


#define FLATTOP_CORRECTION 0.6060f
#define BUFFER_SIZE 10
#define WINDOW_SIZE 10 // 滑动窗口大小


/* Private variables ---------------------------------------------------------*/
//输入寄存器内容																																												       	// 01   23   45   67  89   1011   1213      1415
uint16_t usRegInputBuf[REG_INPUT_NREGS] = {0x4020,0x0000,0x4060,0x0000,0x1004,0x1005,0x1006,0x1007,0x1008};//读04,H2, CO, SMOKE, R , V , TEMP , Humidity , Pa      //数值寄存器区
//标定测试寄存器
uint16_t BDBuf[REG_INPUT_NREGS];//读04,H2, CO, SMOKE, R , V , TEMP , Humidity , Pa      //数值寄存器区

//输入寄存器起始地址
uint16_t usRegInputStart = REG_INPUT_START;

//保持寄存器内容
uint16_t usRegHoldingBuf[REG_HOLDING_NREGS] = {0x147b,0x3f8e,0x147b,0x400e,0x500e,0x600e,0x700e,0x800e};//读03，写06，16,H2K,K2B,COK,COB    ////数值设置区
//保持寄存器起始地址
uint16_t usRegHoldingStart = REG_HOLDING_START;

//线圈状态
uint8_t ucRegCoilsBuf[REG_COILS_SIZE / 8] = {0x01,0x02};//读01，写05，15
//开关输入状态
uint8_t ucRegDiscreteBuf[REG_DISCRETE_SIZE / 8] = {0x01,0x03};//02

uint8_t  fft_flag;
uint8_t  V_flag = 0;

static float hanning_window[FFT_COUNT];
uint8_t cc_flag = 1;
uint32_t sysCount;
uint8_t isearting;



// 滑动平均滤波函数
double movingAverageFilter(double newData, double *buff, int *index, double *sm,uint8_t *buffman) 
{
    // 如果缓冲区已满，则从总和中减去最早的元素
//    if (*index >= WINDOW_SIZE) {
//        *sum -= buffer[(*index) % WINDOW_SIZE];
//    }
//		static uint8_t buffman = 0;
		*sm = 0;
    // 更新缓冲区和总和
    buff[(*index) % WINDOW_SIZE] = newData;
//    *sum += newData;
		(*index)++;
		for(uint8_t i = 0; i < WINDOW_SIZE;i++)
		{
			(*sm) += buff[i];
		}
    
		if((*index) == WINDOW_SIZE)
		{
			*buffman = 1;
			(*index) = 0;
		}
			
    
    // 计算平均值
    if (*buffman == 0) {
        return *sm / *index; // 在窗口未填满之前，取实际数量的平均值
    } else {
        return *sm / WINDOW_SIZE;
    }
}

double removeMaxMinAndAverage(double arr[], int length) 
{
    if (length <= 2) {
        // 如果数组长度小于等于 2，无法去掉最大值和最小值
        return 0.0;
    }

    // 初始化最大值和最小值
    double maxVal = arr[0];
    double minVal = arr[0];
    double sum = 0.0;

    // 计算总和，同时找到最大值和最小值
    for (int i = 0; i < length; i++) {
        sum += arr[i];
        if (arr[i] > maxVal) {
            maxVal = arr[i];
        }
        if (arr[i] < minVal) {
            minVal = arr[i];
        }
    }

    // 调整总和（去掉最大值和最小值）
    sum -= (maxVal + minVal);

    // 返回调整后的平均值
    return sum / (length - 2);
}

void init_hanning_window() {
    for (int i = 0; i < FFT_COUNT; i++) {
        hanning_window[i] = 0.5f * (1 - cosf(2 * PI * i / (FFT_COUNT - 1)));
    }
}

void generate_flattop_window(float *window, int n) 
{
    const float a0 = 0.21557895f;
    const float a1 = 0.41663158f;
    const float a2 = 0.277263158f;
    const float a3 = 0.083578947f;
    const float a4 = 0.006947368f;
    
    for (int i = 0; i < n; i++) {
        float theta = 2.0f * M_PI * i / (n - 1);
        window[i] = a0 
                   - a1 * cosf(theta)
                   + a2 * cosf(2 * theta)
                   - a3 * cosf(3 * theta)
                   + a4 * cosf(4 * theta);
    }
}

arm_cfft_radix2_instance_f32 fft_inst;
float32_t output[FFT_COUNT * 2];
RCC_ClocksTypeDef rccclk;
double magnitude[FFT_COUNT/ 2];
double MagnitudeAVG[10];
double MagnitudeSum=0;
double frequency[FFT_COUNT / 2];
float voltageValues[2];
float R = 0.0;
float V = 0.0;
float window[FFT_COUNT];
uint16_t Swing = 500;
uint8_t SwingFlag = 1;
uint8_t magAvgCount = 0;
double buffer[BUFFER_SIZE] = {0};
int index1 = 0;
double sum = 0.0;
double filteredData1;
uint16_t adc_fft[FFT_COUNT];
uint8_t windwosbuffman = 0;
void ExecuteFft(void)
{
	
	static uint16_t time=0;
	
	time++;
	time%=500;
	
	if(!time) return;
	
		if(fft_flag == 1)
		{
			fft_flag = 0;
			
			
			for (int i = 0; i < FFT_COUNT; i++)
			{
				output[2 * i] = (float)adc1_val[i] /** window[i]*/;     // Real part
				output[2 * i + 1] = 0.0f;     // Imaginary part
			}
			
			arm_cfft_radix2_f32(&fft_inst, output);
			
			for (int i = 0; i < FFT_COUNT / 2; i++)
			{
				float real = output[2 * i];
				float imag = output[2 * i + 1];
				magnitude[i] = sqrtf(real * real + imag * imag) * 2 / (FFT_COUNT) /** FLATTOP_CORRECTION*/;
				frequency[i] = (i * SAMPLE_RATE) / FFT_COUNT;
				
			}
			MagnitudeAVG[magAvgCount] = magnitude[1] /*+ magnitude[2] + magnitude[3] + magnitude[4] + magnitude[5] + magnitude[6] + magnitude[7] + magnitude[8]*/;
			
			magAvgCount++;
			magAvgCount %= 10;
		}
		if(magAvgCount == 0)
		{ 
			MagnitudeSum = removeMaxMinAndAverage(MagnitudeAVG,10);
			filteredData1 = movingAverageFilter(MagnitudeSum, buffer, &index1, &sum,&windwosbuffman);
				
			_R.data = filteredData1 * 10 * 1000 / (Swing * 201);
			R = filteredData1 * 10 * 1000 / (Swing * 201);
			usRegInputBuf[0] = _R.buff[1];
			usRegInputBuf[1] = _R.buff[0];
			MagnitudeSum = 0;
		}
		if(V_flag == 1)
		{
			V = (adc1_val[0] - adc1_val[1]) * 3.3f / 4096.f;
		}
}

#ifdef USE_DEBUG_MODULE	
void debug_task(void)
{
	static uint16_t times=0;
	
	times++;
	times%=200;
	
	if(!times)
	{
		port_printf(0,"co_h2_yw adc_value--> %4d  %4d  %4d    ",adc3_val[0],adc3_val[1],adc2_val[0]);
		port_printf(0,"co_h2_yw real_value--> %4.3f  %4.3f  %4.3f",gas_st.value.co.data,gas_st.value.h2.data,gas_st.value.smoke.data);
		port_printf(0,"\r\n");
		
	}
}
#endif		

//5ms
void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		if(isearting) return;
		sysCount++;
		gas_task();
		ExecuteFft();
		inference_module_id_set();
		uart1_data_send();
#ifdef USE_DEBUG_MODULE		
		debug_task();
#endif			
  }
}

int main(void)
{
	SysTick_Config(72000);
	sysTask_init();
	led_init();
	dac_init();
	adc_init();
	timer_init();
	uart2_init();
	
	while(1)
	{
		led_task_handler();
		sysTask_execute_handler();
	}
}
