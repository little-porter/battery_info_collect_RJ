#include "mcu_init.h"
#include "stdint.h"
#include "dac.h"
#include "adc.h"


#define ADC_CDR_ADDRESS    ((uint32_t)0x5000030C)
#define ADC1_DR_ADDRESS    ((uint32_t)0x50000040)
#define ADC2_DR_ADDRESS    ((uint32_t)0x50000140)
#define ADC3_DR_ADDRESS    ((uint32_t)0x50000440)
#define ADC_DR_ADDRESS    ((uint32_t)0x50000140)

#define DAC_DHR12R1_ADDRESS      0x40007408

__IO uint32_t TimingDelay = 0;
__IO uint32_t ADCDualConvertedValue;
__IO uint16_t calibration_value_1 = 0, calibration_value_2 = 0, calibration_value_4 = 0;


uint8_t  usart_adc1[6],usart_adc2[6];
uint16_t _1d25v = 1551;
uint16_t _1d25v_0d4[2]={2048,1055};

void delay(unsigned int i)
{
	while(i--)
	{
		__NOP();
	}
}

void Delay(__IO uint32_t nTime)
{ 
  TimingDelay = nTime;

  while(TimingDelay != 0);
}


void CAN1_Init(void)
{
    // 使能CAN1时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

    // 配置CAN引脚 PB8 (RX) 和 PB9 (TX)
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
		
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_9);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_9);
		
    // 初始化CAN1
    CAN_InitTypeDef CAN_InitStructure;
    CAN_FilterInitTypeDef CAN_FilterInitStructure;

    CAN_DeInit(CAN1);

    CAN_InitStructure.CAN_TTCM = DISABLE;
    CAN_InitStructure.CAN_ABOM = DISABLE;
    CAN_InitStructure.CAN_AWUM = DISABLE;
    CAN_InitStructure.CAN_NART = ENABLE;
    CAN_InitStructure.CAN_RFLM = DISABLE;
    CAN_InitStructure.CAN_TXFP = DISABLE;
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
    CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
    CAN_InitStructure.CAN_BS1 = CAN_BS1_6tq;
    CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
    CAN_InitStructure.CAN_Prescaler = 16; // 根据系统时钟和所需波特率调整此值
    CAN_Init(CAN1, &CAN_InitStructure);

    // 配置过滤器以接收特定ID的消息
    CAN_FilterInitStructure.CAN_FilterNumber = 0;
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
    CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
    CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);

    // 启动CAN1
    //CAN_Start(CAN1, ENABLE);

    // 使能CAN接收中断
    CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);

    // 使能CAN1中断
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void Usart_Init(uint32_t baudrate)
{
	
}

void Usart_Init1(uint32_t baudrate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // 使能GPIOA和USART1时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    // 配置USART1 Tx (PA.09) 和 Rx (PA.10) 引脚
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 将PA9和PA10复用到USART1功能
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_7); // TX
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_7); // RX

    // USART1 初始化结构体设置
    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    // 如果需要使用中断，请取消注释以下部分并根据需要进行修改
    
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // 启用接收中断
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE); // 启用接收中断
    

    // 使能USART1
    USART_Cmd(USART1, ENABLE);

    // 等待直到接收和发送使能确认标志被设置
    while (USART_GetFlagStatus(USART1, USART_FLAG_REACK) == RESET);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TEACK) == RESET);
}


//adc1_2,adc1_3,adc2_4,adc2_14,adc2_15,adc1_4
//V-      V+    ntc_adc adc_v  acc_ta   adc_sr
int vn,vp,sr,ntc,adc_v,ta;


void ADC1_DMA_Configuration(void)
{
	DMA_InitTypeDef        DMA_InitStructure;
	NVIC_InitTypeDef    NVIC_InitStructure;
  /* Enable DMA1 clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  /* DMA configuration */
  /* DMA1 Channel1 Init Test */
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_ADDRESS;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&adc1_val[0];
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = FFT_COUNT;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;   	 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);
	
  DMA_Init(DMA1_Channel1, &DMA_InitStructure); 
} 

void ADC2_DMA_Configuration(void)
{
	DMA_InitTypeDef        DMA_InitStructure;
  /* Enable DMA1 clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);

  /* DMA configuration */
  /* DMA1 Channel1 Init Test */
  DMA_InitStructure.DMA_PeripheralBaseAddr = /*ADC2->DR*/ADC2_DR_ADDRESS;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&adc2_val[0];
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = 3;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

  DMA_Init(DMA2_Channel1, &DMA_InitStructure); 
} 







void iic_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}





void mcu_init(void)
{

	Usart_Init(19200);
	Usart_Init1(115200);
	led_init();
	       
		
		
    ADC1_DMA_Configuration();
	ADC2_DMA_Configuration();
}
