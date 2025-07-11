#include "fft.h"
#include "sysTask.h"

#include "arm_math.h"
#include "arm_const_structs.h"
#include "average_filter.h"

#define FLATTOP_CORRECTION 0.6060f
#define BUFFER_SIZE 10
#define WINDOW_SIZE 10 // 滑动窗口大小

#define FFT_WINDOW_SIZE		128
//static float fft_hanning_window[FFT_WINDOW_SIZE];

uint16_t fft_flag;


/*初始化汉宁窗*/
void fft_hanning_window_init(float *window) 
{
    for (int i = 0; i < FFT_WINDOW_SIZE; i++) {
        window[i] = 0.5f * (1 - cosf(2 * PI * i / (FFT_WINDOW_SIZE - 1)));
    }
}

arm_cfft_radix2_instance_f32 fft_inst;


void fft_init(void)
{
	arm_cfft_radix2_init_f32(&fft_inst,FFT_WINDOW_SIZE,0,1);
}

uint16_t fft_input_window_ad[FFT_WINDOW_SIZE];
float fft_input_window[FFT_WINDOW_SIZE];
float magnitude[FFT_WINDOW_SIZE];
//float frequency[FFT_WINDOW_SIZE];
#define INR_SIZE	10
float inr[INR_SIZE] = {0};
int  inr_index = 0;
uint8_t inr_full = 0;


void fft_window_ad_full(uint16_t *data,uint16_t len)
{
	uint16_t fft_len = 0;
	uint16_t (*ptr)[2];
	ptr = (uint16_t (*)[2])data;
	if(len < FFT_WINDOW_SIZE){
		fft_len = len;
	}else{
		fft_len = FFT_WINDOW_SIZE;
	}
//	memcpy(fft_input_window_ad,data,fft_len*2);
	for(int i=0;i<fft_len;i++)
	{
		fft_input_window_ad[i] = ptr[i][0];
	}
}

void fft_window_full(void)
{
	memset(fft_input_window,0,FFT_WINDOW_SIZE);
	for(int i=0; i<FFT_WINDOW_SIZE; i++)
	{
		fft_input_window[i] = fft_input_window_ad[i]*3.3/4095;
	}
	
}

float inres = 0;
float fft_run(float *data,uint16_t len)
{
	float fft_window[FFT_WINDOW_SIZE*2] = {0};
	float fft_1khz = 0;
	uint16_t fft_len = FFT_WINDOW_SIZE;
	if(fft_len < len){
		fft_len = FFT_WINDOW_SIZE;
	}else{
		fft_len = len;
	}
	for(int i=0; i<fft_len;i++)
	{
		fft_window[2*i] = data[i];
		fft_window[2*i + 1] = 0.0f;
	}
	
	/*正向傅里叶变化*/
	arm_cfft_radix2_f32(&fft_inst, fft_window);
	
	//计算1KHz幅值
	float real = 0;
	float image = 0;
	for(uint16_t i=0; i<FFT_WINDOW_SIZE;i++)
	{
		real = fft_window[2*i];
		image = fft_window[2*i+1];
		magnitude[i] = sqrtf(real*real + image*image);
//		frequency[i] = 64.0/128*i;
	}
	
	
	
	real = fft_window[2];
	image = fft_window[3];
	fft_1khz = sqrtf(real*real + image*image);
	
//	inres = fft_1khz*3.3/4095/201/128/0.05;
	
	inres = fft_1khz/100/201/128/0.0026f * 2.027f;		// *2.027;
	
	return inres;
}

void bubbleSort(float arr[], int n) {
    int i, j; 
	float temp;
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



void square_calculate(void)
{
	float h_value[128] = {0};
	float l_value[128] = {0};
	int h_pos = 0,l_pos = 0;
	float h_sum = 0,l_sum = 0;
	
	for(int i=0;i<FFT_WINDOW_SIZE;i++){
		if(fft_input_window[i] > 1.65f)
		{
			h_value[h_pos++] = fft_input_window[i];
		}else{
			l_value[l_pos++] = fft_input_window[i];
		}
	}
	bubbleSort(h_value,h_pos);
	bubbleSort(l_value,l_pos);
	if(h_pos > 40)
	{
		for(int i=10;i<(h_pos-25);i++)
		{
			h_sum += h_value[i];
		}
		h_sum = h_sum/(h_pos-35);
	}
	
	if(l_pos > 40)
	{
		for(int i=25;i<(l_pos-10);i++)
		{
			l_sum += l_value[i];
		}
		l_sum = l_sum/(l_pos-35);
	}
	
	float v = h_sum-l_sum;
	
	inres = v/201/0.05f;
	
	inres = average_filter_float_calculate(inres,inr,&inr_index,INR_SIZE,&inr_full);
}

void fft_task(void *param)
{
	if(fft_flag == 0)	return;
	
	fft_flag = 0;
	fft_window_full();
//	arm_cfft_radix2_init_f32(&fft_inst,FFT_WINDOW_SIZE,0,NULL);
//	fft_run(fft_input_window,FFT_WINDOW_SIZE);
	
	square_calculate();
}

void fft_task_handler(void)
{
	static uint32_t start_time = 0,time = 100;
	static uint8_t  start_flag = 0;
	
	if(start_flag == 0){
		start_time = system_tick_get();
		start_flag = 1;
	}
	
	if(system_timer_triggered(start_time,time)){
		start_flag = 0;
		sysTask_publish(fft_task,NULL);
	}
}




