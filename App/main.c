#include "stm32f30x.h"
#include "mcu_init.h"
#include "adc.h"
#include "dac.h"
#include "main.h"

#include "GZP6816D.h"
#include "gxht30.h"
#include "usart.h"
#include "flash_bsp.h"
#include "config.h"
#include "gas.h"


#include "sysTask.h"
#include "led.h"
#include "timer.h"
#include "adc.h"
#include "uart.h"





/* Private variables ---------------------------------------------------------*/
//输入寄存器内容																																												       	// 01   23   45   67  89   1011   1213      1415
uint16_t usRegInputBuf[REG_INPUT_NREGS] = {0x4020,0x0000,0x4060,0x0000,0x1004,0x1005,0x1006,0x1007,0x1008};//读04,H2, CO, SMOKE, R , V , TEMP , Humidity , Pa      //数值寄存器区
//标定测试寄存器
uint16_t BDBuf[REG_INPUT_NREGS];//读04,H2, CO, SMOKE, R , V , TEMP , Humidity , Pa      //数值寄存器区

//输入寄存器起始地址
uint16_t usRegInputStart = REG_INPUT_START;

//保持寄存器内容
uint16_t usRegHoldingBuf[REG_HOLDING_NREGS] = {0x147b,0x3f8e,0x147b,0x400e,0x500e,0x600e,0x700e,0x800e};//读03，写06，16,H2K,K2B,COK,COB    ////数值设置区
//保持寄存器起始地址
uint16_t usRegHoldingStart = REG_HOLDING_START;

//线圈状态
uint8_t ucRegCoilsBuf[REG_COILS_SIZE / 8] = {0x01,0x02};//读01，写05，15
//开关输入状态
uint8_t ucRegDiscreteBuf[REG_DISCRETE_SIZE / 8] = {0x01,0x03};//02

uint8_t  fft_flag;
uint8_t  V_flag = 0;


uint8_t cc_flag = 1;
uint32_t sysCount;
uint8_t isearting;







#ifdef USE_DEBUG_MODULE	
void debug_task(void)
{
	static uint16_t times=0;
	
	times++;
	times%=200;
	
	if(!times)
	{
		port_printf(0,"\r\n");
		
	}
}
#endif		

//5ms
void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		if(isearting) return;
		sysCount++;
		gas_task();
#ifdef USE_DEBUG_MODULE		
		debug_task();
#endif			
  }
}

int main(void)
{
	SysTick_Config(72000);
	sysTask_init();
	led_init();
	dac_init();
	adc_init();
	timer_init();
	uart2_init();
	
	while(1)
	{
		led_task_handler();
		sysTask_execute_handler();
	}
}
