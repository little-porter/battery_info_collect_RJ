#ifndef __MODBUS_H__
#define __MODBUS_H__

#include <stdint.h>
#include <string.h>

void modbus_generate_crcTable(void);
void modbus_msg_deal_handler(uint8_t *data,uint16_t length);

void uart_data_send(uint8_t *data,uint16_t len);

void modbus_reg_write(uint16_t addr,uint16_t *data,uint16_t num);
void modbus_reg_read(uint16_t addr,uint16_t *data,uint16_t num);


#define CONFIG_REG_ADDR			0x0000
#define DATA_REG_ADDR			0x1000

#define VOLTAGE_REG_ADDR		0x1000
#define INRES_REG_ADDR			0x1002
#define CFDLC_REG_ADDR			0x1003
#define CFDZT_REG_ADDR			0x1004
#define BAT_TEMP_REG_ADDR		0x1001
#define CO_REG_ADDR				0x1006
#define H2_REG_ADDR				0x1007
#define SMOKE_REG_ADDR			0x1008
#define	ENV_TEMP_REG_ADDR		0x1005
#define	HUMIDITY_REG_ADDR		0x1009

#define CAlIBRATE_REG_ADDR		0x4001

#endif
