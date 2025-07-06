#ifndef __GAS_H__
#define __GAS_H__

#include "config.h"
#include "stdint.h"


#pragma pack(1)

typedef union _gas_float
{
	float f_data;
	uint16_t u8_data;
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
	float 		h2;
	float 		co;
	float       smoke;
}gas_info_t;
#pragma pack()


void gas_init(void);
void gas_task(void);
void gas_para_save(void);

#endif
