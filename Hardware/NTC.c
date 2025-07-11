#include "NTC.h"
#include "adc.h"
#include "modbus.h"
#include "average_filter.h"

#define NTC_TEMP_TABLE		0
#define NTC_RES_TABLE		1

#define NTC_TABLE_SIZE  	181
#define NTC_FIFO_SIZE		10

static float NTC_temperture = 0;
static float NTC_average_fifo[NTC_FIFO_SIZE] = {0};
static int   NTC_fifo_index = 0;
static uint8_t full_flag = 0;

const float NTC_table[2][NTC_TABLE_SIZE] = {
{	-55,-54,-53,-52,-51,-50,-49,-48,-47,-46,-45,-44,-43,-42,-41,
	-40,-39,-38,-37,-36,-35,-34,-33,-32,-31,-30,-29,-28,-27,-26,
	-25,-24,-23,-22,-21,-20,-19,-18,-17,-16,-15,-14,-13,-12,-11,
	-10, -9, -8, -7, -6, -5, -4, -3, -2, -1,  0,  1,  2,  3,  4,
	  5,  6,  7,  8,  9, 10, 11, 12 ,13, 14, 15, 16, 17, 18 ,19,
	 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34,
	 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
	 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64,
	 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
	 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94,
	 95, 96, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,
	110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,
	125},
{	1034.6, 959.006, 889.452, 825.419, 766.434, 712.066, 661.926, 615.656, 572.934, 533.466,
	496.983,463.24,  432.015, 403.104, 376.32,  351.495, 328.472, 307.11,  287.279, 268.859,
	251.741,235.826, 221.021, 207.242, 194.412, 182.46,  171.32,  160.932, 151.241, 142.196,
	133.75, 125.859, 118.485, 111.589, 105.139, 99.102,  93.45,   88.156,  83.195,  78.544,
	74.183, 70.091,  66.25,   62.643,  59.255,  56.071,  53.078,  50.263,  47.614,  45.121,
	42.774, 40.563,  38.480,  36.517,  34.665,  32.919,  31.270,  29.715,  28.246,  26.858,
	25.547, 24.307,  23.135,  22.026,  20.977,  19.987,  19.044,  18.154,  17.310 , 16.510 ,
	15.752, 15.034,  14.352,  13.705,  13.090 , 12.507,  11.953,  11.427,  10.927,  10.452,
	10.000, 9.570 ,  9.161,   8.771,   8.401,   8.048,   7.712,   7.391,   7.086,   6.795,
	6.518,  6.254,   6.001,   5.761,   5.531,   5.311,   5.102,   4.902,   4.710 ,  4.528,
	4.353,  4.186,   4.026,   3.874,   3.728,   3.588,   3.454,   3.326,   3.203,   3.085,
	2.973,  2.865,   2.761,   2.662,   2.567,   2.476,   2.388,   2.304,   2.223,   2.146,
	2.072,  2.000 ,  1.932,   1.866,   1.803,   1.742,   1.684,   1.627,   1.573,   1.521,
	1.471,  1.423,   1.377,   1.332,   1.289,   1.248,   1.208,   1.170 ,  1.133,   1.097,
	1.063,  1.030 ,  0.998,   0.968,   0.938,   0.909,   0.882,   0.855,   0.829,   0.805,
	0.781,  0.758,   0.735,   0.714,   0.693,   0.673,   0.653,   0.635,   0.616,   0.599, 
	0.582,  0.565,   0.550 ,  0.534,   0.519,   0.505,   0.491,   0.478,   0.465,   0.452,
	0.440 , 0.428,   0.416,   0.405 , 0.395,   0.384,   0.374,   0.364,   0.355,   0.345,
	0.337}
};


float NTC_temperture_calculate(float ntcRes)
{
	uint16_t index = NTC_TABLE_SIZE;
	float temp = 0;
	
	for(int i=0; i<NTC_TABLE_SIZE; i++)
	{
		if(ntcRes > NTC_table[NTC_RES_TABLE][i])
		{
			index = i;
			break;
		}
	}
	
	if(index == 0){
		temp = NTC_table[NTC_TEMP_TABLE][index];
	}else if(index == NTC_TABLE_SIZE){
		temp = NTC_table[NTC_TEMP_TABLE][index-1];
	}else{
		temp = NTC_table[NTC_TEMP_TABLE][index] - (ntcRes - NTC_table[NTC_RES_TABLE][index])/(NTC_table[NTC_RES_TABLE][index-1] - NTC_table[NTC_RES_TABLE][index]);
	}
	NTC_temperture = temp;
	return NTC_temperture;
	
}




void NTC_task_callback(void *param)
{
	uint16_t ntc_adc = get_adc4_value();	//获取ADC值
	float r = 0,temp = 0;
	uint16_t temperture = 0;
	float v = ntc_adc*3.3f/4096;				//计算v
	
	
	if((3.3f-v))
		r = 10*v/(3.3f-v);					//计算r		/* v/3.3 = r/(r+10) */
	
	temp = NTC_temperture_calculate(r);		//计算温度
	
	temp = (float)average_filter_float_calculate(temp,NTC_average_fifo,&NTC_fifo_index,NTC_FIFO_SIZE,&full_flag);
	
	temperture = (uint16_t)(temp * 100);	//温度保留两位小数
	modbus_reg_write(BAT_TEMP_REG_ADDR,&temperture,1);
}





void NTC_task(void)
{
	static uint32_t start_time = 0,time = 500;
	static uint8_t  start_flag = 0;
	
	if(start_flag == 0){
		start_time = system_tick_get();
		start_flag = 1;
	}
	
	if(system_timer_triggered(start_time,time)){
		start_flag = 0;
		sysTask_publish(NTC_task_callback,NULL);
	}
	
}













