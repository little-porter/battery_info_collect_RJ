#include "fft.h"

#include "arm_math.h"
#include "arm_const_structs.h"


#define FLATTOP_CORRECTION 0.6060f
#define BUFFER_SIZE 10
#define WINDOW_SIZE 10 // �������ڴ�С

#define FFT_WINDOW_SIZE		128
static float fft_hanning_window[FFT_WINDOW_SIZE];

// ����ƽ���˲�����
double movingAverageFilter(double newData, double *buff, int *index, double *sm,uint8_t *buffman) 
{
    // �������������������ܺ��м�ȥ�����Ԫ��
//    if (*index >= WINDOW_SIZE) {
//        *sum -= buffer[(*index) % WINDOW_SIZE];
//    }
//		static uint8_t buffman = 0;
		*sm = 0;
    // ���»��������ܺ�
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
			
    
    // ����ƽ��ֵ
    if (*buffman == 0) {
        return *sm / *index; // �ڴ���δ����֮ǰ��ȡʵ��������ƽ��ֵ
    } else {
        return *sm / WINDOW_SIZE;
    }
}

double removeMaxMinAndAverage(double arr[], int length) 
{
    if (length <= 2) {
        // ������鳤��С�ڵ��� 2���޷�ȥ�����ֵ����Сֵ
        return 0.0;
    }

    // ��ʼ�����ֵ����Сֵ
    double maxVal = arr[0];
    double minVal = arr[0];
    double sum = 0.0;

    // �����ܺͣ�ͬʱ�ҵ����ֵ����Сֵ
    for (int i = 0; i < length; i++) {
        sum += arr[i];
        if (arr[i] > maxVal) {
            maxVal = arr[i];
        }
        if (arr[i] < minVal) {
            minVal = arr[i];
        }
    }

    // �����ܺͣ�ȥ�����ֵ����Сֵ��
    sum -= (maxVal + minVal);

    // ���ص������ƽ��ֵ
    return sum / (length - 2);
}

/*��ʼ��������*/
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
	
	/*������Ҷ�仯*/
	arm_cfft_radix2_f32(&fft_inst, fft_window);
	
	//����1KHz��ֵ
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




