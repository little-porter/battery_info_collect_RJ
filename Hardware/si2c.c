#include "si2c.h"


si2c_device_ops_t si2c_ops = {
	.start = si2c_device_start,
	.stop = si2c_devcie_stop,
	.write_data = si2c_device_write_byte,
	.read_data = sim_i2c_read_byte,
	.send_ack = si2c_device_send_ack,
	.recive_ack = si2c_device_recive_ack,
};


#define IIC_PORT_SET(port,pin)		GPIO_SetBits(port,pin)
#define IIC_PORT_RESET(port,pin)	GPIO_ResetBits(port,pin)
#define IIC_PORT_READ(port,pin)		GPIO_ReadInputDataBit(port,pin)


bool si2c_device_init(si2c_device_t *si2c_dev)
{
	if(si2c_dev == NULL)
	{
		return false;
	}
	GPIO_InitTypeDef    GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
		
	IIC_PORT_RESET(GPIOB,GPIO_Pin_6);
	IIC_PORT_RESET(GPIOB,GPIO_Pin_7);
	
	return true;
}


static void si2c_device_delay(void) 
{
    // 简单的延时函数，用于提供必要的延迟以满足I2C时序要求
    for(volatile int i = 0; i < 100; i++); // 调整这个值以匹配你的时钟速度
	
}


static void si2c_device_sda_mode_set(gpio_port_t *sda,GPIOMode_TypeDef mode)
{
	GPIO_InitTypeDef    GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = sda->pin;
	GPIO_InitStructure.GPIO_Mode = mode;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_Init(sda->periph, &GPIO_InitStructure);
}



static void si2c_device_write_bit(si2c_device_t *si2c_dev,uint8_t bit)
{
	if(bit) 
	{
        IIC_PORT_SET(si2c_dev->sda.periph,si2c_dev->sda.pin);
    } 
	else 
	{
        IIC_PORT_RESET(si2c_dev->sda.periph,si2c_dev->sda.pin);
    }
	
    si2c_device_delay();
    IIC_PORT_SET(si2c_dev->scl.periph,si2c_dev->scl.pin);
    si2c_device_delay();
    IIC_PORT_RESET(si2c_dev->scl.periph,si2c_dev->scl.pin);
	
}

static uint8_t si2c_device_read_bit(si2c_device_t *si2c_dev)
{
	si2c_device_delay();
    IIC_PORT_SET(si2c_dev->scl.periph,si2c_dev->scl.pin);
    si2c_device_delay();
    uint8_t bit = IIC_PORT_READ(si2c_dev->sda.periph,si2c_dev->sda.pin);
    IIC_PORT_RESET(si2c_dev->scl.periph,si2c_dev->scl.pin);
    return bit;
}


void si2c_device_send_ack(si2c_device_t *si2c_dev,uint8_t ack) 
{
    // 将SDA设置为输出模式

    if(ack) {
        // 发送ACK
        IIC_PORT_RESET(si2c_dev->sda.periph,si2c_dev->sda.pin);
    } else {
        // 发送NACK
		IIC_PORT_SET(si2c_dev->sda.periph,si2c_dev->sda.pin);
    }
    si2c_device_delay();
    IIC_PORT_SET(si2c_dev->scl.periph,si2c_dev->scl.pin);
    si2c_device_delay();
    IIC_PORT_RESET(si2c_dev->scl.periph,si2c_dev->scl.pin);
	si2c_device_delay();
	IIC_PORT_SET(si2c_dev->sda.periph,si2c_dev->sda.pin);
}

uint8_t si2c_device_recive_ack(si2c_device_t *si2c_dev) 
{
    uint8_t ack = 0;

//	gpio_bit_set(si2c_dev->sda.periph,si2c_dev->sda.pin);
    // 将SDA设置为输入模式
    si2c_device_sda_mode_set(&si2c_dev->sda,GPIO_Mode_IN);

    si2c_device_delay();
    IIC_PORT_SET(si2c_dev->scl.periph,si2c_dev->scl.pin);
    si2c_device_delay();

    // 如果SDA为低，则表示从机发送了ACK
    if(RESET == IIC_PORT_READ(si2c_dev->sda.periph,si2c_dev->sda.pin)) 
	{
        ack = 1; // ACK received
    }

    IIC_PORT_RESET(si2c_dev->scl.periph,si2c_dev->scl.pin);
	si2c_device_delay();
	si2c_device_sda_mode_set(&si2c_dev->sda,GPIO_Mode_OUT);
	si2c_device_delay();
    return ack;
}

void  si2c_device_start(si2c_device_t *si2c_dev)
{
	// 设置SDA从高到低电平
    IIC_PORT_RESET(si2c_dev->sda.periph,si2c_dev->sda.pin);
    si2c_device_delay();
	// 设置SCL从高到低电平
    IIC_PORT_RESET(si2c_dev->scl.periph,si2c_dev->scl.pin);
	si2c_device_delay();
}

void  si2c_devcie_stop(si2c_device_t *si2c_dev)
{
	// 设置SDA从低到高电平，SCL保持高电平
    IIC_PORT_RESET(si2c_dev->sda.periph,si2c_dev->sda.pin);
    si2c_device_delay();
    IIC_PORT_SET(si2c_dev->scl.periph,si2c_dev->scl.pin);
    si2c_device_delay();
    IIC_PORT_SET(si2c_dev->sda.periph,si2c_dev->sda.pin);
	si2c_device_delay();
}

void si2c_device_write_byte(si2c_device_t *si2c_dev,uint8_t byte)
{
	for(uint8_t i = 0; i < 8; ++i) 
	{
        si2c_device_write_bit(si2c_dev,byte & 0x80);
        byte <<= 1;
    }
	
	//s发送完数据释放总线
	IIC_PORT_SET(si2c_dev->sda.periph,si2c_dev->sda.pin);
}

uint8_t sim_i2c_read_byte(si2c_device_t *si2c_dev)
{
	uint8_t byte = 0;
	
	si2c_device_sda_mode_set(&si2c_dev->sda,GPIO_Mode_IN);
    for(uint8_t i = 0; i < 8; ++i) {
        byte = (byte << 1) | si2c_device_read_bit(si2c_dev);
    }
	si2c_device_sda_mode_set(&si2c_dev->sda,GPIO_Mode_OUT);
    return byte;
}




