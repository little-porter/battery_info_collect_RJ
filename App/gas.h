#ifndef __GAS_H__
#define __GAS_H__

#include "config.h"
#include "stdint.h"


#pragma pack(1)
typedef struct
{
	uint16_t  saveFlag;
	FloatType H2K;
	FloatType H2B;
	FloatType COK;
	FloatType COB;
	
}GAS_PARA;   //气体参数

typedef struct
{
	
	FloatType h2;
	FloatType co;
	FloatType smoke;

}GAS_INFO;   //气体信息


typedef struct
{
	GAS_PARA para;
	GAS_INFO value;
	
}GAS_ST;   
#pragma pack()

extern GAS_ST gas_st;

void gas_init(void);
void gas_task(void);
void gas_para_save(void);

#endif
