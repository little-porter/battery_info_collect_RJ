#include "i2c_gpio.h"

void delay_us(uint32_t us)
{
    int16_t i = 0;
    for(i = us;i > 0;i--)
    {
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();

    }
}

void delay_ms(uint32_t count)
{
    uint16_t i = 0;
    for(i = count;i>0;i--)
    {
        delay_us(1000);
    }
}

void IIC_GPIO_Init(void)
{
		GPIO_InitTypeDef    GPIO_InitStructure;
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		
		IIC_GPIO_SCL(Bit_SET);
		IIC_GPIO_SDA(Bit_SET);
}

void IIC_GPIO_SCL_IN(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);                    /* GPIOA时钟使能 */
				
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void IIC_GPIO_SCL_OUT(void)
{	
    GPIO_InitTypeDef    GPIO_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);                    /* GPIOA时钟使能 */

    /* 初始化GPIOA5 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                /* 输入 */
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;                    /* 无上下拉 */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);                /* GPIO初始化 */          /* GPIO初始化 */
}


void IIC_GPIO_SDA_OUT(void)
{	
    GPIO_InitTypeDef    GPIO_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);                    /* GPIOA时钟使能 */

    /* 初始化GPIOA5 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                /* 输入 */
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;                    /* 无上下拉 */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);                /* GPIO初始化 */          /* GPIO初始化 */
}


void IIC_GPIO_SDA_IN(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);                    /* GPIOA时钟使能 */
				
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void IIC_GPIO_Start(void){
	IIC_GPIO_SDA_OUT();       // sda线输出
//    
	IIC_GPIO_SDA(Bit_SET);// = 1 ;	  	  
	IIC_GPIO_SCL(Bit_SET);// = 1 ;
//    
	delay_us( 5 );
 	IIC_GPIO_SDA(Bit_RESET);// = 0 ;    // START:when CLK is high,DATA change form high to low 
//    
	delay_us( 5 ) ;
	IIC_GPIO_SCL(Bit_RESET);// = 0 ;    // 钳制I2C总线，准备发送或者接受数据
}	



void IIC_GPIO_Stop(void){
	IIC_GPIO_SDA_OUT() ;      // sda线输出
    
	IIC_GPIO_SCL(Bit_RESET);// = 0 ;
	IIC_GPIO_SDA(Bit_RESET);// = 0 ;    // STOP:when CLK is high DATA change form low to high
 	IIC_GPIO_SCL(Bit_SET);// = 1 ; 
    
	delay_us( 6 ) ; 
    
	IIC_GPIO_SDA(Bit_SET);// = 1 ;    // 发送I2C总线结束信号
    
	delay_us( 6 ) ;							   	
}



uint8_t IIC_GPIO_Wait_Ack(void){
	uint32_t tempTime = 0 ;
//	IIC_GPIO_SDA(Bit_SET);// = 1 ;
    IIC_GPIO_SDA_IN( ) ;          // SDA设置为输入 	   
	
    
	while( IIC_GPIO_READ_SDA ){
		tempTime += 1 ;
        
		if(tempTime > 250){
			IIC_GPIO_Stop() ;
            
			return 0;
		}
	}
    IIC_GPIO_SCL(Bit_SET);// = 1 ;
	delay_us( 2 ) ;	 
	IIC_GPIO_SCL(Bit_RESET);// = 0;     // 时钟输出0
    delay_us( 2 ) ;
	return 1;  
} 


void IIC_GPIO_Ack(void){
	IIC_GPIO_SCL(Bit_RESET);// = 0 ;
	IIC_GPIO_SDA_OUT( ) ;
	IIC_GPIO_SDA(Bit_RESET);// = 0 ;
    
	delay_us( 2 ) ;
	IIC_GPIO_SCL(Bit_SET);// = 1 ;
	delay_us( 5 ) ;
    
	IIC_GPIO_SCL(Bit_RESET);// = 0 ;
}



void IIC_GPIO_NAck(void){
	IIC_GPIO_SCL(Bit_RESET);// = 0 ; 
	IIC_GPIO_SDA_OUT( ) ;
	IIC_GPIO_SDA(Bit_SET);// = 1 ;
    
	delay_us( 2 ) ;
    IIC_GPIO_SCL(Bit_SET);// = 1 ;
	
    delay_us( 5 ) ;
	IIC_GPIO_SCL(Bit_RESET);// = 0 ;
}	



void IIC_GPIO_Send_Byte(uint8_t txd){                        
    uint8_t t ;   
      
    IIC_GPIO_SDA_OUT();
    IIC_GPIO_SCL(Bit_RESET);// = 0 ;            // 拉低时钟开始数据传输
    
    for(t = 0; t < 8; t++)
		{              
        if( (txd & 0x80) > 0 )
				{
					IIC_GPIO_SDA(Bit_SET);// = 1 ;
				}
        else
				{
					IIC_GPIO_SDA(Bit_RESET);// = 0 ;
				}

        txd <<= 1; 	

        delay_us(1);  //delay_us(5);   
        IIC_GPIO_SCL(Bit_SET);//= 1 ;

        delay_us(2);  //delay_us( 5 ) ; 
        IIC_GPIO_SCL(Bit_RESET);// = 0 ;	
        delay_us(2);  //delay_us( 5 );
    }	 
//    IIC_GPIO_SDA(Bit_SET);// = 1 ;
} 
 



uint8_t IIC_GPIO_Read_Byte(uint8_t ack){
	uint8_t i, receive = 0 ;
    
	IIC_GPIO_SDA_IN( ) ;          // SDA设置为输入
    
    for(i = 0; i < 8; i++ ){
        IIC_GPIO_SCL(Bit_RESET);// = 0 ; 
        delay_us(2);  //delay_us( 5 ) ;
        
        IIC_GPIO_SCL(Bit_SET);// = 1 ;
        while(!GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_6));///11111
        receive <<= 1 ;

        if( IIC_GPIO_READ_SDA ) receive++ ;
        
        delay_us(1);//delay_us(5); 
    }	
    
	if ( !ack ) IIC_GPIO_NAck();  // 发送nACK
	else  IIC_GPIO_Ack();         // 发送ACK   
    
	return receive;
}


int ssss = 0;

void find_iic_device(void){ 
	uint8_t addrs = 0;
	while(1){
		//printf("sht35 test!\t");
		addrs++;
		//printf("addrs: num: %d\t\tHEX: %x\t\t" , addrs, addrs);
		IIC_GPIO_Start();
		IIC_GPIO_Send_Byte( 0xe0 );
		if(!IIC_GPIO_Wait_Ack()){
//			printf("Error :not found!\n");
		}
		else
        {
//            printf("OK !!! num: %d\t\tHEX：%x\n", addrs, addrs);
            ssss++;
        }
			
		IIC_GPIO_Stop( );
		delay_ms(50);
	}
}




