#ifndef _MCU_INIT_
#define 	 _MCU_INIT_

#include "stm32f30x.h"
#include <stdbool.h>


void led_init(void);
void mcu_init(void);
void delay(unsigned int i);
void Delay(__IO uint32_t nTime);
void adc1_ch23_init(bool adc1ch);
void TIM1_Config(uint16_t period);
void Usart_Init(uint32_t baudrate);

#endif
