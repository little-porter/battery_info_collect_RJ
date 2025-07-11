#include "gas.h"

#include "adc.h"
#include "math.h"
#include "config.h"

#include "modbus.h"
#include "sysTask.h"
#include "average_filter.h"
#include "flash.h"

#define gas_smoke_on()	GPIO_SetBits(GPIOC,GPIO_Pin_13)
#define gas_smoke_off()	GPIO_ResetBits(GPIOC,GPIO_Pin_13)
//#define gas_param_save_flag_set()	(gas_info.param.save_flag = 1)
//#define gas_param_save_flag_reset() (gas_info.param.save_flag = 0)

#define GAS_SMOKE_FIFO_SIZE 	10
#define GAS_H2_FIFO_SIZE 		10
#define GAS_CO_FIFO_SIZE 		10

gas_info_t gas_info;    


private static uint16_t gas_smoke_fifo[GAS_SMOKE_FIFO_SIZE];
private static int gas_smoke_fifo_index = 0;
private static uint8_t gas_smoke_calculate = 0;

private static uint16_t gas_h2_fifo[GAS_H2_FIFO_SIZE];
private static int gas_h2_fifo_index = 0;
private static uint8_t gas_h2_fifo_full = 0;

private static uint16_t gas_co_fifo[GAS_CO_FIFO_SIZE];
private static int gas_co_fifo_index = 0;
private static uint8_t gas_co_fifo_full = 0;

void gas_param_save_flag_set(void)
{
	gas_info.param.save_flag = 1;
}
void gas_param_save_flag_reset(void)
{
	gas_info.param.save_flag = 0;
}

//void gas_param_save(void)
//{
//	if(1 == gas_info.param.save_flag)
//	{
//		/*保存参数*/
//		gas_param_t read_param = {0};
//		uint8_t *data = (uint8_t *)&gas_info.param;
//		uint16_t len = sizeof(gas_param_t);
//		while(!memcmp(&read_param,&gas_info.param,sizeof(gas_param_t))){
//			flash_pages_erase(GAS_PARAM_SAVE_ADDR,1);
//			flash_write_data(GAS_PARAM_SAVE_ADDR,data,len);
//			flash_read_data(GAS_PARAM_SAVE_ADDR,(uint8_t *)&read_param,sizeof(gas_param_t));
//		}
//	}

//}

void gas_param_init(void)
{
	
//	flash_read_data(GAS_PARAM_SAVE_ADDR,(uint8_t *)&gas_info.param,sizeof(gas_param_t));
	if(gas_info.param.save_flag != 1){
		gas_info.param.H2.k = 1;
		gas_info.param.H2.b = 1;
		gas_info.param.CO.k = 1;
		gas_info.param.CO.b = 1;
	}
	
//	gas_param_save();
	
	gas_info.co = 0;
	gas_info.h2 = 0;
	gas_info.smoke = 0;
	
	
}
uint16_t device_num = 4;


/**************************************************************************************************************
	烟雾检测过程
**************************************************************************************************************/
void gas_smoke_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* GPIOC clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
	
	/* Configure PC13 as AF */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	gas_smoke_on();
	gas_info.smoke_process = GAS_SELFCHECK;
}

void gas_smoke_selfcheck(void)
{
	static uint8_t selfcheck_process = 0;
	uint16_t smoke_ad = get_smoke_ad_value();
	uint16_t min = 0,max = 0,diff = 0;
	switch(selfcheck_process)
	{
		case 0:
			smoke_ad = average_filter_uint16_calculate(smoke_ad,gas_smoke_fifo,&gas_smoke_fifo_index,GAS_SMOKE_FIFO_SIZE,&gas_smoke_calculate);
			if(gas_smoke_calculate == 1 && smoke_ad > 1000)
			{
				gas_smoke_calculate = 0;
				gas_smoke_fifo_index = 0;
				selfcheck_process = 1;
				gas_smoke_off();
			}
			break;
		case 1:
			smoke_ad = average_filter_uint16_calculate(smoke_ad,gas_smoke_fifo,&gas_smoke_fifo_index,GAS_SMOKE_FIFO_SIZE,&gas_smoke_calculate);
			if(gas_smoke_calculate == 1 && smoke_ad < 200)
			{
				gas_smoke_calculate = 0;
				gas_smoke_fifo_index = 0;
				selfcheck_process = 2;
				gas_smoke_on();
			}
		case 2:
			smoke_ad = average_filter_uint16_calculate(smoke_ad,gas_smoke_fifo,&gas_smoke_fifo_index,GAS_SMOKE_FIFO_SIZE,&gas_smoke_calculate);
			if(!gas_smoke_calculate)  return;
			max = gas_smoke_fifo[0];
			min = gas_smoke_fifo[0];
			for(int i=0;i<GAS_SMOKE_FIFO_SIZE;i++){
				if(max < gas_smoke_fifo[i]){
					max = gas_smoke_fifo[i];
				}else{;}
				if(min > gas_smoke_fifo[i]){
					min = gas_smoke_fifo[i];
				}else{;}
			}
			diff = max - min;
			if(diff < 200){
				gas_info.smoke_process = GAS_CHECK;
				gas_smoke_calculate = 0;
				gas_smoke_fifo_index = 0;
			}
		default:
			break;
		
	}	
}

void gas_smoke_check(void)
{
	static uint16_t pre_drop = 0;
	uint16_t smoke = 0;
	uint16_t pre_value = 0;
	uint16_t min = 0,drop = 0;
	uint16_t smoke_ad = get_smoke_ad_value();
	smoke_ad = average_filter_uint16_calculate(smoke_ad,gas_smoke_fifo,&gas_smoke_fifo_index,GAS_SMOKE_FIFO_SIZE,&gas_smoke_calculate);
	pre_value = gas_smoke_fifo[gas_smoke_fifo_index];
	min = gas_smoke_fifo[gas_smoke_fifo_index];
	
	for(int i=0; i<GAS_SMOKE_FIFO_SIZE; i++){
		if(gas_smoke_fifo[i] < min){
			min = gas_smoke_fifo[i];
		}
	}
	
	
	if(0 == gas_info.smoke && 0 == gas_info.smoke_keep_time){
		drop = pre_value - min;
		if(drop >= 500){
			pre_drop = drop;
			gas_info.smoke = 1;
			gas_info.smoke_keep_time = 30;
		}
	}else{
		if((min - pre_drop) >= 300 && gas_info.smoke_keep_time != 0){
			gas_info.smoke_keep_time--;
		}
		if(0 == gas_info.smoke_keep_time){
			gas_info.smoke = 0;
		}
	}
	smoke = gas_info.smoke;
	modbus_reg_write(SMOKE_REG_ADDR,&smoke,1);
}

void gas_smoke_check_process(void)
{
	switch(gas_info.smoke_process)
	{
		case GAS_INIT:
			gas_smoke_init();
			break;
		case GAS_SELFCHECK:
			gas_smoke_selfcheck();
			break;
		case GAS_CHECK:
			gas_smoke_check();
			break;
		default:
			break;
		
	}	
}

/**************************************************************************************************************
	氢气检测过程
**************************************************************************************************************/
void gas_h2_init(void)
{
	gas_h2_fifo[gas_h2_fifo_index++] = get_h2_ad_value();
	if(gas_h2_fifo_index == GAS_H2_FIFO_SIZE)			/*等待填满滑窗*/
		gas_info.h2_process = GAS_SELFCHECK;
	gas_h2_fifo_index %= GAS_H2_FIFO_SIZE;
}

void gas_h2_selfcheck(void)
{
	gas_h2_fifo[gas_h2_fifo_index++] = get_h2_ad_value();
	gas_h2_fifo_index %= GAS_H2_FIFO_SIZE;
	uint16_t max = 0,min =0;
	uint32_t diff = 0,sum = 0;
	max = min = gas_h2_fifo[0];
	for(int i=0; i<GAS_H2_FIFO_SIZE;i++){
		if(max < gas_h2_fifo[i]){
			max = gas_h2_fifo[i];
		}
		
		if(min < gas_h2_fifo[i]){
			min = gas_h2_fifo[i];
		}
		sum += gas_h2_fifo[i];
	}
	/*计算采样电压*/
	float v = 0,max_v = 0, min_v = 0;
	v = (sum/GAS_H2_FIFO_SIZE)*3.3/4096;
	max_v = max*3.3/4096;
	min_v = min*3.3/4096;
	/*计算敏感电阻*/
	float r = 0,max_r = 0, min_r = 0;
	if(v)
		r = (6.6f-2*v)/v;
	if(min_v)
		max_r = (6.6f-2*max_v)/min_v;
	if(min_v)
		min_r = (6.6f-2*min_v)/min_v;
	diff = max_r - min_r;
	
	if((diff < 5) && (r > 10)){				/*敏感电阻大于10k，且敏感电阻变化不大于5k自检合格*/
		gas_info.h2_process = GAS_CHECK;
		gas_h2_fifo_index = 0;
	}
}

void gas_h2_calculate(void)
{
	uint16_t h2_ad_value = get_h2_ad_value();
	h2_ad_value = average_filter_uint16_calculate(h2_ad_value,gas_h2_fifo,&gas_h2_fifo_index,GAS_H2_FIFO_SIZE,&gas_h2_fifo_full);
	float    h2_v = h2_ad_value * 3.3/4095;
	float    h2_r = 0;
	uint16_t h2_ppm = 0;
	if(h2_v)	
		h2_r = (6.6f-2*h2_v)/h2_v;                  /*h2_v/2 = 3.3/(2+r)*/
	float    h2 = (float)(gas_info.param.H2.k * pow(h2_r,gas_info.param.H2.b));
	h2_ppm = (uint16_t)h2;
	gas_info.h2 = h2;
	modbus_reg_write(H2_REG_ADDR,&h2_ppm,1);	
}


void gas_h2_check_process(void)
{
	switch(gas_info.h2_process)
	{
		case GAS_INIT:
			gas_h2_init();
			break;
		case GAS_SELFCHECK:
			gas_h2_selfcheck();
			break;
		case GAS_CHECK:
			gas_h2_calculate();
			break;
		default:
			break;
		
	}
}
/**************************************************************************************************************
	一氧化碳检测过程
**************************************************************************************************************/
void gas_co_init(void)
{
	gas_co_fifo[gas_co_fifo_index++] = get_co_ad_value();
	if(gas_co_fifo_index == GAS_CO_FIFO_SIZE)			/*填满滑窗*/
		gas_info.co_process = GAS_SELFCHECK;
	gas_co_fifo_index %= GAS_CO_FIFO_SIZE;
}

void gas_co_selfcheck(void)
{
	gas_co_fifo[gas_co_fifo_index++] = get_co_ad_value();
	gas_co_fifo_index %= GAS_CO_FIFO_SIZE;
	uint16_t max = 0,min =0;
	uint32_t diff = 0,sum = 0;
	max = min = gas_co_fifo[0];
	for(int i=0; i<GAS_CO_FIFO_SIZE;i++){
		if(max < gas_co_fifo[i]){
			max = gas_co_fifo[i];
		}
		
		if(min < gas_co_fifo[i]){
			min = gas_co_fifo[i];
		}
		sum += gas_co_fifo[i];
	}
	/*计算采样电压*/
	float v = 0,max_v = 0, min_v = 0;
	v = (sum/GAS_CO_FIFO_SIZE)*3.3/4096;
	max_v = max*3.3/4096;
	min_v = min*3.3/4096;
	/*计算敏感电阻*/
	float r = 0,max_r = 0, min_r = 0;
	if(v)
		r = (6.6f-2*v)/v;
	if(min_v)
		max_r = (6.6f-2*max_v)/min_v;
	if(min_v)
		min_r = (6.6f-2*min_v)/min_v;
	diff = max_r - min_r;
	
	if((diff < 5) && (r > 10)){				/*敏感电阻大于10k，且敏感电阻变化不大于5k自检合格*/
		gas_info.co_process = GAS_CHECK;
		gas_co_fifo_index = 0;
	}
}

void gas_co_calculate(void)
{
	uint16_t co_ad_value = get_co_ad_value();
	co_ad_value = average_filter_uint16_calculate(co_ad_value,gas_co_fifo,&gas_co_fifo_index,GAS_CO_FIFO_SIZE,&gas_co_fifo_full);
	float    co_v = co_ad_value * 3.3/4095;
	float    co_r = 0;
	uint16_t co_ppm = 0;
	if(co_v)	
		co_r = (33.0f-10*co_v)/co_v;                  /*co_v/10 = 3.3/(10+r)*/
	float    co = (float)(gas_info.param.CO.k * pow(co_r,gas_info.param.CO.b));
	co_ppm = (uint16_t)co;
	gas_info.co = co;
	modbus_reg_write(H2_REG_ADDR,&co_ppm,1);	
}


void gas_co_check_process(void)
{
	switch(gas_info.co_process)
	{
		case GAS_INIT:
			gas_co_init();
			break;
		case GAS_SELFCHECK:
			gas_co_selfcheck();
			break;
		case GAS_CHECK:
			gas_co_calculate();
			break;
		default:
			break;
		
	}
}



/**************************************************************************************************************
	气体检测参数初始化
**************************************************************************************************************/
void gas_init(void)
{
	gas_param_init();
	gas_info.co_process = GAS_INIT;
	gas_info.h2_process = GAS_INIT;
	gas_info.smoke_process = GAS_INIT;
}


/**************************************************************************************************************
	气体检测任务回调函数
**************************************************************************************************************/
void gas_task_callback(void *param)
{
	gas_smoke_check_process();
	
	gas_h2_check_process();
	
	gas_co_check_process();
	
//	gas_param_save();
}
/**************************************************************************************************************
	气体检测任务，检测周期100ms
**************************************************************************************************************/
void gas_task(void)
{
	static uint32_t start_time = 0,time = 100;
	static uint8_t  start_flag = 0;
	
	if(start_flag == 0){
		start_time = system_tick_get();
		start_flag = 1;
	}
	
	if(system_timer_triggered(start_time,time)){
		start_flag = 0;
		sysTask_publish(gas_task_callback,NULL);
	}
}

