#ifndef __I2C_GPIO_H_
#define __I2C_GPIO_H_


//#include "includes.h"
#include <stdint.h>
#include "stm32f30x.h"
#include "mcu_init.h"
//#include "systick.h"

#define IIC_SCL_RCU   RCU_GPIOB
#define IIC_SCL_PORT	GPIOB
#define IIC_SCL_PIN		GPIO_Pin_6

#define IIC_SDA_RCU		RCU_GPIOB
#define IIC_SDA_PORT	GPIOB
#define IIC_SDA_PIN		GPIO_Pin_7

#define IIC_GPIO_SCL(n)    GPIO_WriteBit(GPIOB, GPIO_Pin_6, n)
#define IIC_GPIO_SDA(n)    GPIO_WriteBit(GPIOB, GPIO_Pin_7, n) 
#define IIC_GPIO_READ_SDA   GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_7)
#define IIC_GPIO_READ_SCL   GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_6)

//#define GPIO_PIN_SET   1
//#define GPIO_PIN_RESET 0

void IIC_GPIO_Init(void);
void IIC_GPIO_SDA_OUT(void);
void IIC_GPIO_SDA_IN(void);
void IIC_GPIO_Start(void);
void IIC_GPIO_Stop(void);
void IIC_GPIO_Ack(void);
void IIC_GPIO_NAck(void);
void IIC_GPIO_Send_Byte(uint8_t txd);
void IIC_GPIO_SCL_IN(void);
void IIC_GPIO_SCL_OUT(void);
uint8_t IIC_GPIO_Read_Byte(uint8_t ack);
uint8_t IIC_GPIO_Wait_Ack(void);
void delay_us(uint32_t us);
void delay_ms(uint32_t count);


void find_iic_device(void);


#endif

