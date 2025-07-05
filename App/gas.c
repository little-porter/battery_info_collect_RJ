#include "gas.h"
#include "flash_bsp.h"
#include "adc.h"
#include "math.h"
#include "config.h"

GAS_ST gas_st;    


void gas_para_save(void)
{
	userDataErase();
	writeData_to_flash(FLASH_USER_DATA_ADDR,(uint16_t*)&gas_st.para,(sizeof(GAS_PARA)+1) / 2);

}

void gas_para_init(void)
{
	readData_from_flash(FLASH_USER_DATA_ADDR,(uint8_t *)&gas_st.para,sizeof(GAS_PARA));
	if(gas_st.para.saveFlag != 1)
	{
		gas_st.para.saveFlag = 1;
		gas_st.para.H2K.data = 223310;
		gas_st.para.H2B.data = -3.821;
		gas_st.para.COK.data = 516125;
		gas_st.para.COB.data = -1.889;
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
	
	gas_st.value.co.data = (float)(gas_st.para.COK.data * pow(r_co,gas_st.para.COB.data));
	gas_st.value.h2.data = (float)(gas_st.para.H2K.data * pow(r_h2,gas_st.para.H2B.data));
	gas_st.value.smoke.data = (float)adc2_val[0];
	
//	if(gas_st.value.co.data<40) gas_st.value.co.data=0;
//	if(gas_st.value.h2.data<20) gas_st.value.h2.data=0;

	
	usRegInputBuf[6] = gas_st.value.h2.buff[1];
	usRegInputBuf[7] = gas_st.value.h2.buff[0];//H2
	usRegInputBuf[4] = gas_st.value.co.buff[1];
	usRegInputBuf[5] = gas_st.value.co.buff[0];//CO
	usRegInputBuf[8] = gas_st.value.smoke.buff[1];
	usRegInputBuf[9] = gas_st.value.smoke.buff[0];//SMOKE
	
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
	gas_para_init();
}

void gas_task(void)
{
	gas_calculate();

}
