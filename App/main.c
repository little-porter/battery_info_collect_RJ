#include "adc.h"
#include "dac.h"
#include "sysTask.h"
#include "led.h"
#include "timer.h"
#include "adc.h"
#include "uart.h"
#include "modbus.h"
#include "GXHTC3.h"
#include "NTC.h"
#include "gas.h"
#include "battery_info.h"



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

int main(void)
{
  	SysTick_Config(72000);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	modbus_generate_crcTable();
	sysTask_init();
	led_init();
	dac_init();
	adc_init();
	timer_init();
	uart2_init();
	gas_init();
	
	GXHTC3_device_init(&GXHTC3_device);
	
	while(1)
	{
		led_task();
		GXHTC3_task();
		battery_info_task();
		NTC_task();
		gas_task();

		sysTask_execute_handler();
	}
}
