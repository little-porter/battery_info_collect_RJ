#include "GXHTC3.h"
#include "crc8.h"
#include "sysTask.h"
#include "modbus.h"

#define GXHTC3_I2C_ADDR		0x70
#define GXHTC3_READ_ID_CMD	0xEFC8
#define GXHTC3_SLEEP_CMD	0xB098
#define GXHTC3_WAKEUP_CMD	0x3517

#define GXHTC3_TEMP_HUTY_NO_STRET_NORMAL_MEASURE_CMD	0x7866
#define GXHTC3_HUTY_TEMP_NO_STRET_NORMAL_MEASURE_CMD	0x58E0
#define GXHTC3_TEMP_HUTY_NO_STRET_LOW_MEASURE_CMD		0x609C
#define GXHTC3_HUTY_TEMP_NO_STRET_LOW_MEASURE_CMD		0x401A


GXHTC3_device_t GXHTC3_device;


void GXHTC3_device_init(GXHTC3_device_t *GXHTC3_dev)
{
	si2c_device_t *si2c_dev = &GXHTC3_dev->i2c_dev;
	
	si2c_dev->addr = GXHTC3_I2C_ADDR;
	si2c_dev->scl.periph       = GPIOB;
	si2c_dev->scl.pin		   = GPIO_Pin_6;
	si2c_dev->sda.periph       = GPIOB;
	si2c_dev->sda.pin		   = GPIO_Pin_7;
	
	GXHTC3_dev->addr = GXHTC3_I2C_ADDR;
	GXHTC3_dev->temperature = 0;
	GXHTC3_dev->humidity    = 0;
	GXHTC3_dev->process     = GXHTC3_WAKEUP;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);                    /* GPIOA时钟使能 */
	
	si2c_device_init(si2c_dev);
	
}

uint8_t ack = 1;

uint8_t GXHTC3_device_send_cmd(GXHTC3_device_t *GXHTC3_dev,uint16_t cmd)
{
	si2c_device_t *si2c_dev = &GXHTC3_dev->i2c_dev;
	uint8_t sed_cmd[2] = {cmd>>8,cmd&0xFF};
	si2c_ops.start(si2c_dev);
	si2c_ops.write_data(si2c_dev,GXHTC3_dev->addr<<1);
	ack = si2c_ops.recive_ack(si2c_dev);
	si2c_ops.write_data(si2c_dev,sed_cmd[0]);
	si2c_ops.recive_ack(si2c_dev);
	si2c_ops.write_data(si2c_dev,sed_cmd[1]);
	si2c_ops.recive_ack(si2c_dev);
	si2c_ops.stop(si2c_dev);
	return 0;
}

uint8_t GXHTC3_device_read_data(GXHTC3_device_t *GXHTC3_dev,uint8_t *data,uint8_t len)
{
	si2c_device_t *si2c_dev = &GXHTC3_dev->i2c_dev;
	
	si2c_ops.start(si2c_dev);
	si2c_ops.write_data(si2c_dev,GXHTC3_dev->addr<<1 | 0x01);
	si2c_ops.recive_ack(si2c_dev);
	
	for(uint8_t i = 0;i<len;i++)
	{
		data[i] = si2c_ops.read_data(si2c_dev);
		si2c_ops.send_ack(si2c_dev,1);
	}
	si2c_ops.stop(si2c_dev);
	return 0;
}



uint8_t GXHTC3_device_wake_up(GXHTC3_device_t *GXHTC3_dev)
{
	GXHTC3_device_send_cmd(GXHTC3_dev,GXHTC3_WAKEUP_CMD);
	return 0;
}


uint8_t GXHTC3_device_temp_huty_no_stret_normal_measure(GXHTC3_device_t *GXHTC3_dev)
{
	GXHTC3_device_send_cmd(GXHTC3_dev,GXHTC3_TEMP_HUTY_NO_STRET_NORMAL_MEASURE_CMD);
	return 0;
}


uint8_t crc = 0;

uint8_t GXHTC3_device_read_temp_huty_value(GXHTC3_device_t *GXHTC3_dev)
{
	uint8_t read_buf[6] = {0};
	
	GXHTC3_device_read_data(GXHTC3_dev,read_buf,6);
	
	
	GXHTC3_dev->temperature = read_buf[0]<<8 |read_buf[1];
	GXHTC3_dev->humidity = read_buf[3]<<8 | read_buf[4];
	
	crc = calculate_crc8(read_buf,2);
	if(crc != read_buf[2])	return 0;
	crc = calculate_crc8(&read_buf[3],2);
	if(crc != read_buf[5])	return 0;
	
	GXHTC3_dev->temperature_value = ((float)GXHTC3_dev->temperature)/65536*175-45;
	GXHTC3_dev->humidity_value = ((float)GXHTC3_dev->humidity)/65536*100;
	
	int16_t reg_temp = (int16_t)(GXHTC3_dev->temperature_value*100);
	modbus_reg_write(ENV_TEMP_REG_ADDR,(uint16_t *)&reg_temp,1);
	
	return 0;
}

uint8_t GXHTC3_device_sleep(GXHTC3_device_t *GXHTC3_dev)
{	
	GXHTC3_device_send_cmd(GXHTC3_dev,GXHTC3_SLEEP_CMD);
	return 0;
}

uint8_t id1[3] = {0};

uint8_t GXHTC3_device_read_device_id(GXHTC3_device_t *GXHTC3_dev)
{
//	sim_i2c_device_t *sim_i2c_dev = &GXHTC3_dev->i2c_dev;
//	uint32_t id = 0x00000000;

	
	GXHTC3_device_send_cmd(GXHTC3_dev,GXHTC3_WAKEUP_CMD);
	
//	HAL_Delay(15);
	
	GXHTC3_device_send_cmd(GXHTC3_dev,GXHTC3_READ_ID_CMD);
	
	GXHTC3_device_read_data(GXHTC3_dev,(uint8_t *)id1,3);
	
//	GXHTC3_dev->device_id = (uint16_t)id;
	GXHTC3_dev->device_id = id1[0]<<8 | id1[1];
	
//	uint8_t crc = calculate_crc8(id1,2);
	
	GXHTC3_device_send_cmd(GXHTC3_dev,GXHTC3_SLEEP_CMD);
	return 0;
}


uint8_t GXHTC3_device_read_temp_huty_value_process(GXHTC3_device_t *GXHTC3_dev)
{
	return 0;
}


void GXHTC3_task_callback(void *param)
{
	GXHTC3_device_t *GXHTC3_dev = (GXHTC3_device_t *)param;
	
	switch(GXHTC3_dev->process)
	{
		case GXHTC3_WAKEUP:
			GXHTC3_device_wake_up(GXHTC3_dev);
			GXHTC3_dev->process = GXHTC3_MEASURE;
			break;
		case GXHTC3_READ_ID:
			GXHTC3_device_send_cmd(GXHTC3_dev,GXHTC3_READ_ID_CMD);
			GXHTC3_device_read_data(GXHTC3_dev,(uint8_t *)id1,3);
//			GXHTC3_dev->process = GXHTC3_SLEEP;
			break;
		case GXHTC3_MEASURE:
			GXHTC3_device_temp_huty_no_stret_normal_measure(GXHTC3_dev);
			GXHTC3_dev->process = GXHTC3_READ_DATA;
			break;
		case GXHTC3_READ_DATA:
			GXHTC3_device_read_temp_huty_value(GXHTC3_dev);
			GXHTC3_dev->process = GXHTC3_SLEEP;
			break;
		case GXHTC3_SLEEP:
			GXHTC3_device_sleep(GXHTC3_dev);
			GXHTC3_dev->process = GXHTC3_WAKEUP;
			break;
		default:
			GXHTC3_dev->process = GXHTC3_WAKEUP;
			break;
	}
}

void GXHTC3_task(void)
{
	static uint32_t start_time = 0,time = 100;
	static uint8_t  start_flag = 0;
	
	if(start_flag == 0){
		start_time = system_tick_get();
		start_flag = 1;
	}
	
	if(system_timer_triggered(start_time,time)){
		start_flag = 0;
		sysTask_publish(GXHTC3_task_callback,&GXHTC3_device);
	}
	
	
}



