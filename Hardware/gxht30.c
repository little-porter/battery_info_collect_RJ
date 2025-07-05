#include "gxht30.h"
#include "main.h"

SYS_PAR sys_para;

TemphuiMsg_t temphuimsg;
uint16_t modbus_reg_buff[14];
uint32_t modbus_reg_get[5];

//工作模式寄存器
uint8_t SHT25_Modecmd_Buffer[2] = { 0x22, 0x36};
//读取寄存器
uint8_t SHT25_Fetchcmd_Buffer[2] = { 0xE0, 0x00};

//读取的数据 0,1为温度 4,5为温度
uint8_t SHT35_Date_Buffer[6];
float Humidity,Tempertur;

void b_turn_s(uint8_t *src)
{
	uint8_t tmp[2],i;
	for(i = 0; i<2; i++)
	{
		tmp[i] = src[1-i];
	}
	memcpy(src,tmp,2);
}

void SHT35_read(void)
{
    uint16_t temp1,temp2;
    uint16_t temp1_ckc, temp2_ckc;
    
    
    if(temphuimsg.count == 0)
		{
        IIC_GPIO_Start();
        IIC_GPIO_Send_Byte(0xe0);
        IIC_GPIO_Wait_Ack();

        IIC_GPIO_Send_Byte(0x58);
        IIC_GPIO_Wait_Ack();

        IIC_GPIO_Send_Byte(0xE0);
        IIC_GPIO_Wait_Ack();
        IIC_GPIO_Stop();
        
    }
		else if(temphuimsg.count == 3)
		{
        IIC_GPIO_Start();
        IIC_GPIO_Send_Byte(0xe1);
        if(IIC_GPIO_Wait_Ack())
        {
            temp1 = IIC_GPIO_Read_Byte(1);
            temp1 =(temp1 << 8 ) | IIC_GPIO_Read_Byte(1);
            temp1_ckc = IIC_GPIO_Read_Byte(1);
            
            temp2 = IIC_GPIO_Read_Byte(1);
            temp2 =(temp2 << 8 ) | IIC_GPIO_Read_Byte(1);
            temp2_ckc = IIC_GPIO_Read_Byte(0);

            IIC_GPIO_Stop( );
            
            //数据转换
            sys_para.Humidity.data = (temp1 * 100) / 65536.0f; 
//            memcpy(&modbus_reg_buff[3],&sys_para.Humidity.buff,4);
		//			modbus_reg_buff[3] = L2B_B2L_32b(modbus_reg_buff[3]);
						modbus_reg_buff[6] = sys_para.Humidity.buf[0];
						modbus_reg_buff[7] = sys_para.Humidity.buf[1];
						
						b_turn_s((uint8_t *)&modbus_reg_buff[6]);
						b_turn_s((uint8_t *)&modbus_reg_buff[7]);
									//摄氏温度
									sys_para.temputer.data = (float)((temp2 * 175) / 65536.0f) - 45;
			//            memcpy(&modbus_reg_buff[2],&sys_para.temputer.buff,4);
			//			modbus_reg_buff[2] = L2B_B2L_32b(modbus_reg_buff[2]);
						modbus_reg_buff[4] = sys_para.temputer.buf[0];
						modbus_reg_buff[5] = sys_para.temputer.buf[1];
						b_turn_s((uint8_t *)&modbus_reg_buff[4]);
						b_turn_s((uint8_t *)&modbus_reg_buff[5]);
						usRegInputBuf[10] = sys_para.temputer.buf[1];
						usRegInputBuf[11] = sys_para.temputer.buf[0];
						usRegInputBuf[12] = sys_para.Humidity.buf[1];
						usRegInputBuf[13] = sys_para.Humidity.buf[0];
		//			memcpy(&modbus_reg_buff[0],&sys_para.temputer.buff,4);
		//			memcpy(&modbus_reg_buff[1],&sys_para.temputer.buff,4);
		//			memcpy(&modbus_reg_buff[2],&sys_para.temputer.buff,4);
        }
    }
    temphuimsg.count += 1;
    temphuimsg.count %= 4;
}

void SHT35_CL(void)
{
    IIC_GPIO_Start();
    IIC_GPIO_Send_Byte(0xe0);
    IIC_GPIO_Wait_Ack();

    IIC_GPIO_Send_Byte(0x78);
    IIC_GPIO_Wait_Ack();

    IIC_GPIO_Send_Byte(0x66);
    IIC_GPIO_Wait_Ack();
    IIC_GPIO_Stop();
    
    delay_ms(15);
}


void SHT35_Init(void)
{
	
//		while(1)
//		{
//			IIC_GPIO_SCL(Bit_RESET);
//		}
//    IIC_GPIO_Start();
		while(1)
		{
			IIC_GPIO_Start();
			IIC_GPIO_Send_Byte( 0xe0 );
			if(IIC_GPIO_Wait_Ack())
			{
				IIC_GPIO_Send_Byte( 0x35 );
				if(IIC_GPIO_Wait_Ack())
				{
						IIC_GPIO_Send_Byte( 0x17 );
						if(IIC_GPIO_Wait_Ack())
						{
								break;
						}
				}
			}
		}
    IIC_GPIO_Stop( );
    delay_us(500);
}



void SHT35_ReadDdate(void){	
//    IIC_GPIO_Start();
//    IIC_GPIO_Send_Byte( 0X88 );
//    if(!IIC_GPIO_Wait_Ack()){
//    }
//    IIC_GPIO_Send_Byte( 0X2C );
//    if(!IIC_GPIO_Wait_Ack()){
//    }
//    IIC_GPIO_Send_Byte( 0X10 );
//    if(!IIC_GPIO_Wait_Ack()){
//    }
//    IIC_GPIO_Stop( );

//    delay_us(2);

//    IIC_GPIO_Start();
//    IIC_GPIO_Send_Byte( 0X89 );
//    if(!IIC_GPIO_Wait_Ack()){
//    }
//    delay_ms(5);

    uint16_t temp1 = IIC_GPIO_Read_Byte(1);
    temp1 =(temp1 << 8 ) | IIC_GPIO_Read_Byte(1);
    uint16_t temp1_ckc = IIC_GPIO_Read_Byte(1);

    uint16_t temp2 = IIC_GPIO_Read_Byte(1);
    temp2 =(temp2 << 8 ) | IIC_GPIO_Read_Byte(1);
    uint16_t temp2_ckc = IIC_GPIO_Read_Byte(0);

    IIC_GPIO_Stop( );
    
    //数据转换
	sys_para.Humidity.data = (temp2 * 100) / 65536.0f; 
//    memcpy(&modbus_reg_buff[3],&sys_para.Humidity.buff,4);
	//摄氏温度
	sys_para.temputer.data = (float)((temp1 * 175) / 65536.0f) - 45;
//    memcpy(&modbus_reg_buff[4],&sys_para.temputer.buff,4);
	
//	//数据转换
//	Humidity = (((SHT35_Date_Buffer[3] << 8) + SHT35_Date_Buffer[4]) * 100) / 65536.0f; 
//	//摄氏温度
//	Tempertur = (float)((((SHT35_Date_Buffer[0] << 8) + SHT35_Date_Buffer[1]) * 175) / 65536.0f) - 45;
}

void SHT35_TestDemo(void){

}


