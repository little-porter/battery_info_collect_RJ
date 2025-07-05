#include "GZP6816D.h"
#include "stdint.h"
#include "string.h"
#include "stm32f30x.h"
#include <stdio.h>
#include <math.h>
#include "main.h"
 
#define DELAY_TIME 20
#define PMIN 30.0 // The minimum range pressure value for example 30Kpa 传感器最低量程
#define PMAX 110.0 //The ful scale pressure value, for example 110Kpa 传感器最大量程
#define DMIN 1677722.0 //AD value corresponding to The minimum range pressure,// for example 10%AD=2^24*0.1 最低量程对应的 AD值
#define DMAX 15099494.0 //AD Value Corresponding to The ful scale pressure value,// for example 90%AD=2^24*0.9 最高量程对应的 AD值
//sbit SCL = P3 ^ 3; //IIC clock line IIC 时钟线
//sbit SDA = P1 ^ 5; //IIC data line IIC 数据线
#define SCL(n)  n == 1 ? GPIO_SetBits(GPIOB,GPIO_Pin_6) : GPIO_ResetBits(GPIOB,GPIO_Pin_6)
#define SDA(n)	n == 1 ? GPIO_SetBits(GPIOB,GPIO_Pin_7) : GPIO_ResetBits(GPIOB,GPIO_Pin_7)
#define SDA_DATA GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7)
//The 7-bit IIC address of the sensor is 0x78 传感器 7 位 IIC 总线地址
unsigned char Device_Address = 0x78 << 1;
float pressure_kpa = 0.0; //变量，用于保存压力值，单位为KPa
unsigned long pressure_pa = 0; //变量，用于保存压力值，单位为Pa
float temperature = 0.0; //变量，用于保存温度值，单位为℃

void Delay_Ms(unsigned char n) //Ms Time-Delay function Ms 延时函数
{
	 unsigned char i,j;
	 for(i=0;i<n;i++)
	 for(j=0;j<123;j++);
}

void sda_out(void)
{	
	GPIO_InitTypeDef GPIO_InitStructure;
	
//	GPIO_DeInit(GPIOB);
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void sda_in(void)
{	
	GPIO_InitTypeDef GPIO_InitStructure;
	
//	GPIO_DeInit(GPIOB);
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void Start(void)
{
	sda_out();
	SDA(1);
	//Start signal IIC 总线起始信号
	Delay_Ms(DELAY_TIME);
	SCL(1);
	Delay_Ms(DELAY_TIME);
	SDA(0);
	Delay_Ms(DELAY_TIME);
	SCL(0);
	Delay_Ms(DELAY_TIME);
}
 
void Stop(void)//Stop signal IIC 总线停止信号
{
	sda_out();
	SDA(0);
	Delay_Ms(DELAY_TIME);
	SCL(1);
	Delay_Ms(DELAY_TIME);
	SDA(1);
	Delay_Ms(DELAY_TIME);
	SCL(0);
	Delay_Ms(DELAY_TIME);
}

unsigned char Check_ACK(void) //Read sensor’s ACK signal 读取传感器的 ACK 信号
{
	unsigned char ack;
	sda_out();
	SDA(1);
	Delay_Ms(DELAY_TIME);
	SCL(1);
	sda_in();
	Delay_Ms(DELAY_TIME / 2);
	ack = SDA_DATA;
	Delay_Ms(DELAY_TIME / 2);
	SCL(0);
	Delay_Ms(DELAY_TIME);
	return ack;
}
void Send_ACK(void) //Send ACK signal to sensor 向传感器发送 ACK 信号
{
	sda_out();
	SDA(0);
	Delay_Ms(DELAY_TIME);
	SCL(1);
	Delay_Ms(DELAY_TIME);
	SCL(0);
	Delay_Ms(DELAY_TIME);
	SDA(1);
	Delay_Ms(DELAY_TIME);
}

void SendByte(unsigned char byte1)
{
	unsigned char i = 0;
	sda_out();
	do
	{
		if (byte1 & 0x80)
		{
			SDA(1); 
		}
		else
		{
			SDA(0); 
		}
		Delay_Ms(DELAY_TIME);
		SCL(1);
		Delay_Ms(DELAY_TIME);
		byte1 <<= 1;
		i++;
		SCL(0);
	} while (i < 8);
	SCL(0);
	Delay_Ms(DELAY_TIME);
}

unsigned char ReceiveByte(void)
{
	unsigned char i = 0, tmp = 0;
	sda_in();
	do
	{
		tmp <<= 1;
		SCL(1);
		Delay_Ms(DELAY_TIME);
		if (SDA_DATA == 1)
		{
			tmp |= 1; 
		}
		SCL(0);
		Delay_Ms(DELAY_TIME);
		i++;
	} while (i < 8);
	return tmp;
}

//Write several bytes of data to the sensor 向传感器写若干个字节
unsigned char GZP6816D_IIC_Write(unsigned char address, unsigned char *buf, unsigned char count)
{
	unsigned char timeout, ack;
	address &= 0xFE; // The lowest bit of address is 0 means writing 地址值最低位为 0 表示写
	Start();
	Delay_Ms(DELAY_TIME);
	SendByte(address); //Send the address to the sensor 向传感器发送地址值
	Delay_Ms(DELAY_TIME);
	timeout = 0;
	do //Getting the ACK response from the sensor means success, otherwise means failure
	{
	//如能获取传感器的ACK信号，则表示IIC总线通信成功；否则表示通信失败
		ack = Check_ACK();
		timeout++;
		if (timeout == 10)
		{
			Stop();
			return 1;
		}
	} while (ack);
	while (count)
	{
		SendByte(*buf); //Write bytes to the sensor 向传感器写字节数据
		Delay_Ms(DELAY_TIME);
		timeout = 0;
		do
		{
			ack = Check_ACK();
			timeout++;
			if (timeout == 10)
			{
				return 2;
			}
		} while (0);
		buf++;
		count--;
	}
	Stop();
	return 0;
}
	 
//Read several bytes of data from the sensor 从传感器读取若干个字节
unsigned char GZP6816D_IIC_Read(unsigned char address, unsigned char *buf, unsigned char count)
{
	unsigned char timeout, ack;
	address |= 0x01; // The lowest bit of address is 1 means reading 地址值最低位为 1 表示读
	Start();
	SendByte(address);
	Delay_Ms(DELAY_TIME);
	timeout = 0;
	do //Getting the ACK response from the sensor means success, otherwise means failure
	{
		//如能获取传感器的ACK信号，则表示IIC总线通信成功；否则表示通信失败
		ack = Check_ACK();
		timeout++;
		if (timeout == 10)
		{
			Stop();
			return 1;
		}
	} while (ack);
	Delay_Ms(DELAY_TIME);
	while (count)
	{
		*buf = ReceiveByte();
		if (count != 1) //Except the last byte, send ACK to the sensor after receiving byte
		Send_ACK(); //除了最后一个字节外，每接收一个字节，向传感器发送ACK信号
		buf++;
		count--;
	}
	Stop();
	return 0;
}

//Read the status of the sensor and judge whether IIC is busy
unsigned char GZP6816D_IsBusy(void) //读取传感器的状态变量，用于判断数据是否已准备好被读取
{
	unsigned char status;
	GZP6816D_IIC_Read(Device_Address, &status, 1);
	status = (status >> 5) & 0x01;
	return status;
}

void GZP6816D_get_cal(void) //The function of reading pressure and temperature from the sensor
{
	//从传感器GZP6816D读取压力值和温度值的函数
	unsigned char buffer[6] = {0}; //Temp variables used to restoring bytes from the sensor
	//临时变量，用于保存从传感器中读出的字节数据
	unsigned long Dtest = 0;
	unsigned int temp_raw = 0;
	buffer[0] = 0xAC;
	//Send 0xAC command and read the returned six-byte data
	GZP6816D_IIC_Write(Device_Address, buffer, 1); //发送 0XAC 命令，启动一次压力和温度测量
	Delay_Ms(DELAY_TIME);
	while (1)
	{
		if (GZP6816D_IsBusy()) //判断传感器数据是否已准备好被读取
		{
			Delay_Ms(DELAY_TIME);
		}
		else
				break;
	}
	GZP6816D_IIC_Read(Device_Address, buffer, 6); //读取传感器 6 个字节数据
	//Computing the calibrated pressure and temperature values 计算校准后的压力值和温度值
	Dtest = (unsigned long)((((unsigned long)buffer[1]) << 16) | (((unsigned int)buffer[2]) << 8) | ((unsigned char)buffer[3]));
	temp_raw = ((unsigned int)buffer[4] << 8) | (buffer[5] << 0);
	//The calibrated pressure value is converted into actual values
	if (Dtest != 0)
	{
		//将校准后的压力值转换成真实的压力值
		pressure_kpa = (float) ((PMAX-PMIN)/(DMAX-DMIN)*(Dtest-DMIN)+PMIN); //单位：KPa
		pressure_pa = (unsigned long) (pressure_kpa * 1000.0);							//单位：Pa
	}
	else
	{
		pressure_kpa = 0.0;	//pressure value, its unit is KPa 压力值，单位：KPa
		pressure_pa = 0;		//pressure value, its unit is Pa 压力值，单位：Pa
	}
	temperature = (float) temp_raw / 65536; //The calibrated temperature value is converted into actualvalues
	temperature = (temperature * 19000 - 4000) / 100; // its unit is ℃ //将校准后的温度值转换成真实的温度值，单位：℃
//	temperature = 190 * temperature - 40;
	usRegInputBuf[14] = pressure_pa >> 16;
	usRegInputBuf[15] = pressure_pa & 0x000000000000ffff;
}

//void main() //The main function
//{
//	Printf(“CFSensor Ltd”);
//	while(1)
//	{
//		GZP6816D_get_cal();
//		Delay_Ms(1000);
//		Printf(“The pressure from GZP6816D is %ld Pa\r\n”, pressure_pa);
//		Printf(“The temperature from GZP6816D is %f CTG\r\n\r\n”, temperature);
//	}
//}

