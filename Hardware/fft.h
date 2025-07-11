#ifndef __FFT_H__
#define __FFT_H__



#include "system.h"


extern uint16_t fft_flag;


void fft_init(void);
float fft_run(float *data,uint16_t len);

void fft_window_ad_full(uint16_t *data,uint16_t len);
void fft_task_handler(void);

#endif
