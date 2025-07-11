#ifndef __SI2C_H__
#define __SI2C_H__

#include "system.h"

typedef struct{
	GPIO_TypeDef *periph;
	uint16_t	  pin;
}gpio_port_t;


typedef struct
{
	gpio_port_t scl;
	gpio_port_t sda;
	uint8_t 	addr;
}si2c_device_t;

typedef struct
{
	void (*start)(si2c_device_t *si2c_dev);
	void (*stop)(si2c_device_t *si2c_dev);
	void (*write_data)(si2c_device_t *si2c_dev,uint8_t byte);
	uint8_t (*read_data)(si2c_device_t *si2c_dev);
	void (*send_ack)(si2c_device_t *si2c_dev,uint8_t ack);
	uint8_t (*recive_ack)(si2c_device_t *si2c_dev);
}si2c_device_ops_t;


bool si2c_device_init(si2c_device_t *si2c_dev);

void  si2c_device_start(si2c_device_t *si2c_dev);
void  si2c_devcie_stop(si2c_device_t *si2c_dev);
void si2c_device_write_byte(si2c_device_t *si2c_dev,uint8_t byte);
uint8_t sim_i2c_read_byte(si2c_device_t *si2c_dev);
void si2c_device_send_ack(si2c_device_t *si2c_dev,uint8_t ack);
uint8_t si2c_device_recive_ack(si2c_device_t *si2c_dev);

extern si2c_device_ops_t si2c_ops;


#endif
