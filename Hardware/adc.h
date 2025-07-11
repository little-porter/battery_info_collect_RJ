#ifndef _ADC_H_
#define _ADC_H_

#include "stdint.h"
#include "string.h"
#include "math.h"
#include "main.h"
#include "config.h"

#include "system.h"

#define FFT_COUNT 128
#define SAMPLE_RATE 128000//100000
#define M_PI 3.14159265358979323846

typedef struct {
    double real;
    double imag;
} Complex;

extern Complex input[FFT_COUNT];

extern uint16_t sr_100[100];

extern FloatType h2float,cofloat,smokefloat;
extern uint16_t ad1_dy[3],ad2_dy[3];



void adc_init(void);


uint16_t get_adc4_value(void);			
uint16_t get_smoke_ad_value(void);
uint16_t get_h2_ad_value(void);
uint16_t get_co_ad_value(void);

#endif
