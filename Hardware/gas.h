#ifndef __GAS_H__
#define __GAS_H__

#include "config.h"
#include "stdint.h"


#pragma pack(1)


typedef enum _gas_process
{
	GAS_INIT = 0,
	GAS_SELFCHECK = 1,
	GAS_CHECK = 2,
}gas_process_t;

typedef union _gas_float
{
	float f_data;
	uint16_t u16_data[2];
}gas_float_t;



typedef struct _gas_ratio
{
	float k;
	float b;
}gas_ratio_t;

typedef struct _gas_param
{
	uint8_t     save_flag;
	gas_ratio_t CO;
	gas_ratio_t H2;
}gas_param_t;

typedef struct _gas_info
{
	
	gas_param_t param;
	gas_process_t h2_process;
	gas_process_t smoke_process;
	gas_process_t co_process;
	float 		h2;
	float 		co;
	float       smoke;
	uint32_t    smoke_keep_time;
}gas_info_t;
#pragma pack()

/*********************************************************************************************************************
 public 外部调用接口
*********************************************************************************************************************/
void gas_init(void);
void gas_task(void);
void gas_param_save_flag_set(void);
void gas_param_set(uint16_t *reg);

#endif
