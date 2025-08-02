#include "led.h"
/**************************************************************************************************************
	版本：
		v0.0.0 2025.07.24
**************************************************************************************************************/


/**************************************************************************************************************
	led相关宏定义
**************************************************************************************************************/
#define LED_PORT	GPIOB
#define LED_PIN		GPIO_Pin_9

#define LED_LIST_SIZE	10

#define LED_PERIOD_200MS		200
#define LED_PERIOD_500MS		500
#define LED_PERIOD_1000MS		1000
#define LED_PERIOD_2000MS		2000

#define LED_TASK_RUN_PERIOD		100



/**************************************************************************************************************
	led状态定义
**************************************************************************************************************/
typedef enum _led_state
{
	LED_STATE_OFF = 0,
	LED_STATE_ON,
}led_state_t;

typedef enum _led_status
{
	LED_OFF = 0,
	LED_ON,
	LED_BLINK,	
}led_status_t;

typedef struct _led_device
{
	GPIO_TypeDef 	*port;
	uint32_t  	  	pin;
	led_status_t	status;
	uint16_t 		period;
	led_run_status_t run_status;
	led_state_t     cur_state;
}led_device_t;


/**************************************************************************************************************
	bsp--led设备
**************************************************************************************************************/
private led_device_t *led_list[LED_LIST_SIZE] = {0};

private led_device_t led_sys = {
	.port = GPIOB,
	.pin  = GPIO_Pin_9,
	.status = LED_BLINK,
	.period = LED_PERIOD_200MS,
	.cur_state  = LED_STATE_OFF,
};

/**************************************************************************************************************
	led开启函数
**************************************************************************************************************/
void led_on(led_device_t *led_dev)
{
	GPIO_SetBits(led_dev->port,led_dev->pin);
	led_dev->cur_state = LED_STATE_ON;
}

/**************************************************************************************************************
	led关闭函数
**************************************************************************************************************/
void led_off(led_device_t *led_dev)
{
	GPIO_ResetBits(led_dev->port,led_dev->pin);
	led_dev->cur_state = LED_STATE_OFF;
}


/**************************************************************************************************************
	led闪烁函数
**************************************************************************************************************/
void led_blink(led_device_t *led_dev)
{
	static uint32_t time = 0;
	if(time > led_dev->period){
		time = 0;
		if(led_dev->cur_state == LED_STATE_OFF){
			led_on(led_dev);
		}else{
			led_off(led_dev);
		}
	}else{
		time += LED_TASK_RUN_PERIOD;
	}
}

/**************************************************************************************************************
	led硬件初始化函数
**************************************************************************************************************/
void led_gpio_init(led_device_t *led_dev)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = led_dev->pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
		
	GPIO_ResetBits(led_dev->port,led_dev->pin);	
	
	for(int i=0; i<LED_LIST_SIZE; i++){
		if(led_list[i] == NULL){
			led_list[i] = led_dev;
			break;
		}else{
			continue;
		}			
	}
}


/**************************************************************************************************************
	系统指示灯状态切换函数
**************************************************************************************************************/
void led_sys_status_set(led_run_status_t run_status)
{
	switch(run_status)
	{
		case LED_RUN_STATUS_ERROR:
			led_sys.status = LED_OFF;
			led_sys.run_status = LED_RUN_STATUS_ERROR;
			break;
		case LED_RUN_STATUS_WORK:
			led_sys.run_status = LED_RUN_STATUS_WORK;
			led_sys.period = LED_PERIOD_500MS;
			led_sys.status = LED_BLINK;
			break;
		case LED_RUN_STATUS_UPGRADING:
			led_sys.run_status = LED_RUN_STATUS_UPGRADING;
			led_sys.period = LED_PERIOD_2000MS;
			led_sys.status = LED_BLINK;
			break;
		default:
			break;
	}
	
}


/**************************************************************************************************************
	led运行函数
**************************************************************************************************************/
void led_run(led_device_t *led_dev)
{
	switch(led_dev->status)
	{
		case LED_OFF:
			led_off(led_dev);
			break;
		case LED_ON:
			led_on(led_dev);
			break;
		case LED_BLINK:
			led_blink(led_dev);
			break;
		default:
			led_dev->status = LED_OFF;
			break;		
		
	}
}

/**************************************************************************************************************
	led任务回调函数
**************************************************************************************************************/
void led_task_callback(void *param)
{
	for(int i=0; i<LED_LIST_SIZE; i++){
		if(led_list[i] != NULL){
			led_run(led_list[i]);
		}else{
			continue;
		}
	}
}

/**************************************************************************************************************
	led任务
**************************************************************************************************************/
void led_task(void)
{
	static uint32_t start_time = 0,time = LED_TASK_RUN_PERIOD;
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

/**************************************************************************************************************
	led设备初始化
**************************************************************************************************************/
void led_init(void)
{
	led_gpio_init(&led_sys);
	led_off(&led_sys);
}

/**************************************************************************************************************
	系统运行指示灯状态切换测试任务
**************************************************************************************************************/
void led_change_task(void)
{
	static uint32_t start_time = 0,time = 12000;
	static uint8_t  start_flag = 0,change_flag = 0;
	
	if(start_flag == 0){
		start_time = system_tick_get();
		start_flag = 1;
	}
	
	if(system_timer_triggered(start_time,time)){
		start_flag = 0;
		if(change_flag == 0){
			change_flag = 1;
			led_sys_status_set(LED_RUN_STATUS_WORK);
		}else{
			change_flag = 0;
			led_sys_status_set(LED_RUN_STATUS_UPGRADING);
		}
	}
}

