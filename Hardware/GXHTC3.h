#ifndef __GXHTC3_DEVICE_H__
#define __GXHTC3_DEVICE_H__

#include "si2c.h"

typedef enum _GXHTC3_process
{
	GXHTC3_WAKEUP = 0,
	GXHTC3_READ_ID = 1,
	GXHTC3_MEASURE = 2,
	GXHTC3_READ_DATA = 3,
	GXHTC3_SLEEP = 4,
	
}GXHTC3_process_t;



typedef struct
{
	uint8_t addr;
	uint16_t device_id;
	si2c_device_t i2c_dev;
	
	int16_t temperature;
	uint16_t humidity;
	
	float temperature_value;
	float humidity_value;
	
	GXHTC3_process_t process;
}GXHTC3_device_t;

extern GXHTC3_device_t GXHTC3_device;

void GXHTC3_device_init(GXHTC3_device_t *GXHTC3_dev);

uint8_t GXHTC3_device_wake_up(GXHTC3_device_t *GXHTC3_dev);

uint8_t GXHTC3_device_read_device_id(GXHTC3_device_t *GXHTC3_dev);

uint8_t GXHTC3_device_read_temp_huty_value(GXHTC3_device_t *GXHTC3_dev);
uint8_t GXHTC3_device_read_temp_huty_value_process(GXHTC3_device_t *GXHTC3_dev);

void GXHTC3_task(void);

#endif

