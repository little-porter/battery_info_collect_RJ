#include "fft.h"

#include "arm_math.h"
#include "arm_const_structs.h"


#define FLATTOP_CORRECTION 0.6060f
#define BUFFER_SIZE 10
#define WINDOW_SIZE 10 // 滑动窗口大小

#define FFT_WINDOW_SIZE		128
static float fft_hanning_window[FFT_WINDOW_SIZE];

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

/*初始化汉宁窗*/
void fft_hanning_window_init() 
{
    for (int i = 0; i < FFT_WINDOW_SIZE; i++) {
        fft_hanning_window[i] = 0.5f * (1 - cosf(2 * PI * i / (FFT_WINDOW_SIZE - 1)));
    }
}

arm_cfft_radix2_instance_f32 fft_inst;


void fft_init(void)
{
	arm_cfft_radix2_init_f32(&fft_inst,FFT_WINDOW_SIZE,0,NULL);
}


float magnitude[FFT_WINDOW_SIZE];
float frequency[FFT_WINDOW_SIZE];

float fft_run(float *data,uint16_t len)
{
	float fft_window[FFT_WINDOW_SIZE] = {0};
	float fft_1khz = 0;
	uint16_t fft_len = FFT_WINDOW_SIZE;
	if(fft_len < len){
		memcpy(fft_window,data,fft_len);
	}else{
		memcpy(fft_window,data,len);
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
		frequency[i] = 64.0/128*i;
	}
	
//	float real = fft_window[4];
//	float image = fft_window[5];
//	fft_1khz = sqrtf(real*real + image*image);
	
	
	
}




