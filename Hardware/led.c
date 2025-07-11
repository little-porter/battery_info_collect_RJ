#include "led.h"


#define LED_PORT	GPIOB
#define LED_PIN		GPIO_Pin_9

typedef enum _led_state
{
	LED_OFF = 0,
	LED_ON	= 1,	
}led_state_t;

typedef struct _led_device
{
	GPIO_TypeDef 	*port;
	uint32_t  	  	pin;
	
	
	
}led_device_t;

void led_on(void)
{
	GPIO_SetBits(LED_PORT,LED_PIN);
}

void led_off(void)
{
	GPIO_ResetBits(LED_PORT,LED_PIN);
}



void led_gpio_init(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
		
	GPIO_ResetBits(GPIOB,GPIO_Pin_9);	
}


void led_init(void)
{
	led_gpio_init();
	
	led_off();
}

void led_task_callback(void *param)
{
	static uint8_t led_flag = 0;
	if(led_flag == 0){
		led_flag = 1;
		led_off();
	}else{
		led_flag =0;
		led_on();
	}
}

void led_task(void)
{
	static uint32_t start_time = 0,time = 500;
	static uint8_t  start_flag = 0;
	
	if(start_flag == 0){
		start_time = system_tick_get();
		start_flag = 1;
	}
	
	if(system_timer_triggered(start_time,time)){
		start_flag = 0;
		sysTask_publish(led_task_callback,NULL);
	}
}


