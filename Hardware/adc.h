#ifndef _ADC_H_
#define _ADC_H_

#include "stdint.h"
#include "string.h"
#include "math.h"
#include "main.h"
#include "config.h"

#include "system.h"



typedef struct {
    double real;
    double imag;
} Complex;



void adc_init(void);

uint16_t get_v_low_ad_value(void);
uint16_t get_v_high_ad_value(void);
uint16_t get_ntc_ad_value(void);			
uint16_t get_smoke_ad_value(void);
uint16_t get_h2_ad_value(void);
uint16_t get_co_ad_value(void);

#endif
