#include "gas.h"
#include "flash_bsp.h"
#include "adc.h"
#include "math.h"
#include "config.h"

#include "modbus.h"
#include "sysTask.h"

gas_info_t gas_info;    


void gas_para_save(void)
{
	userDataErase();
	writeData_to_flash(FLASH_USER_DATA_ADDR,(uint16_t*)&gas_info.param,sizeof(gas_param_t));

}

void gas_param_init(void)
{
	readData_from_flash(FLASH_USER_DATA_ADDR,(uint8_t *)&gas_info.param,sizeof(gas_param_t));
	if(gas_info.param.save_flag != 1)
	{
		gas_info.param.save_flag = 1;
		gas_info.param.H2.k = 223310;
		gas_info.param.H2.b = -3.821;
		gas_info.param.CO.k = 516125;
		gas_info.param.CO.b = -1.889;
	}
}
uint16_t device_num = 4;
void gas_calculate(void)
{
	
	float r_co=0,r_h2=0;
	
	if(adc3_val[0])
	{
		r_co = (4095-adc3_val[0]) / adc3_val[0] * 10;
	}
	else
	{
#ifdef USE_DEBUG_MODULE
		port_printf(0,"co adc : 0\r\n");
#endif
	}
	
	if(adc3_val[1])
	{
		r_h2 = (8190 / adc3_val[1]) - 2;
	}
	else
	{
#ifdef USE_DEBUG_MODULE
		port_printf(0,"h2 adc : 0\r\n");
#endif
	}
	
	gas_info.co = (float)(gas_info.param.CO.k * pow(r_co,gas_info.param.CO.b));
	gas_info.h2 = (float)(gas_info.param.H2.k * pow(r_h2,gas_info.param.H2.b));
	gas_info.smoke = (float)adc2_val[0];
	
//	if(gas_st.value.co.data<40) gas_st.value.co.data=0;
//	if(gas_st.value.h2.data<20) gas_st.value.h2.data=0;
	uint16_t gas_msg[3] = {0};
	gas_msg[0] = (uint16_t)gas_info.co;
	gas_msg[1] = (uint16_t)gas_info.h2;
	gas_msg[2] = (uint16_t)gas_info.smoke;
	
	modbus_reg_write(DATA_REG_ADDR,gas_msg,3);
	
	/*****************±ê¶¨*****************/
	//ÇâÆø
	BDBuf[0] = adc3_val[1];
	//co
	BDBuf[1] = adc3_val[0];
	//yw
	BDBuf[2] = adc2_val[0];
	
	BDBuf[3] = device_num;

}
void gas_init(void)
{
	gas_param_init();
}

void gas_task(void)
{
	gas_calculate();

}

void gas_task_handler(void *param)
{
	static uint32_t start_time = 0,time = 500;
	static uint8_t  start_flag = 0;
	
	if(start_flag == 0){
		start_time = system_tick_get();
		start_flag = 1;
	}
	
	if(system_timer_triggered(start_time,time)){
		start_flag = 0;
		sysTask_publish(gas_task,NULL);
	}
}

