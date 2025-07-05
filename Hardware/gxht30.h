

#ifndef _GXHT30_H_
#define	_GXHT30_H_

#include <stdint.h>
#include "i2c_gpio.h"
#include "string.h"

#define SMOK_BUFF_MAX	50
#define CO_H2_BUFF_MAX	50

#define SMOKE_BJ_COUNT	35
#define CO_BJ_COUNT		35
#define H2_BJ_COUNT		35

#define SMOKE_DISBJ_COUNT	20
#define CO_DISBJ_COUNT		20
#define H2_DISBJ_COUNT		20


#define SMOKE_BJ_FLAG	0x01
#define CO_BJ_FLAG		0x01
#define H2_BJ_FLAG		0x02


typedef union _float_type{
	float data;
	uint16_t buf[2];
}FLOAT_TYPE; 

typedef struct _alarm_type{
	//烟雾报警
	uint8_t smoke_alarm:1;
	//H2报警
	uint8_t co_alarm:1;
	//CO报警
	uint8_t h2_alarm:1;
	//温度报警
	uint8_t temputer_alarm:1;
	//预留
	uint8_t yl_var:4;
	
}ALARM_TYPE;

typedef struct __sys_par{
	//开机过程
	uint8_t kj_state;
	
	uint16_t kj_count;
	uint16_t device_addr;
	
	//数据采集周期
	uint16_t cj_circle;
	
	//co和H2状态
	uint8_t co_h2_state[CO_H2_BUFF_MAX];
	//smoke状态
	uint8_t smoke_state[SMOK_BUFF_MAX];
	//co和H2下标
	uint16_t co_h2_indx;
	//烟雾下标
	uint16_t smoke_inex;
	
	uint16_t smoke_bj_count;
	uint16_t co_bj_count;
	uint16_t h2_bj_count;
	
	
	//烟雾数据
	FLOAT_TYPE smoke_data;
	//H2数据
	FLOAT_TYPE h2_data;
	//co数据
	FLOAT_TYPE co_data;
	//温度
	FLOAT_TYPE	 temputer;
	//湿度
	FLOAT_TYPE Humidity;
	
	//报警阈值
	FLOAT_TYPE smoke_limt;
	FLOAT_TYPE h2_limt;
	FLOAT_TYPE co_limt;
	FLOAT_TYPE temputer_limt;
	FLOAT_TYPE humidity_limt;
	
	//报警状态
	ALARM_TYPE alarm_state;
}SYS_PAR;


// 窗口缓存结构体
typedef struct _watch_arage_struct
{
	uint16_t buff[20];
	uint8_t index;
	uint8_t is_full_flag;
	int32_t arrage_data;
} WATCH_ARAGE_STRUCT;

extern SYS_PAR sys_para;


typedef struct{
	float temp;
	float humi;
    
    uint16_t count;
}TemphuiMsg_t;


void SHT35_Init(void);
void SHT35_CL(void);
void SHT35_read(void);
void SHT35_ReadDdate(void);

void SHT35_TestDemo(void);


#endif


