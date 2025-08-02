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

#include "iap.h"



int main(void)
{
	modbus_generate_crcTable();
	iap_init();
	system_gpio_deinit();
  	SysTick_Config(72000);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	sysTask_init();
	led_init();
	dac_init();
	adc_init();
	timer_init();
	
	gas_init();
	
	uart2_init();
	
	while(1)
	{
		led_task();
//		led_change_task();
		GXHTC3_task();
		battery_info_task();
		NTC_task();
		gas_task();
		iap_task();

		sysTask_execute_handler();
	}
}
