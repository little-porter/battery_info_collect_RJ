#include "uart.h"
#include "modbus.h"


#define DATA_FIFO_SIZE		1024

typedef struct _data_fifo
{
	uint8_t data[DATA_FIFO_SIZE];
	uint16_t length;
}data_fifo_t;


data_fifo_t uart2_rx_fifo;



void uart2_gpio_config(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	
	// 配置CAN引脚 PB3(RX) 和 PB4 (TX)
    GPIO_InitTypeDef GPIO_InitStructure;
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
		
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_7);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_7);
	
}



void uart_data_send(uint8_t *data,uint16_t len)
{
	for(int i=0; i<len;i++)
	{
		USART_SendData(USART2,data[i]);
		while(!USART_GetFlagStatus(USART2,USART_FLAG_TC)){;};	
	}	
}



void uart2_config(void)
{
	// 使能usart时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
  
	/* 配置中断源 */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	
		
	USART_DeInit(USART2);
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART2, &USART_InitStructure);
	
	
	
	//USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
	
	USART_Cmd(USART2, ENABLE);
	
	/* Wait until Receive enable acknowledge flag is set */
	while(USART_GetFlagStatus(USART2, USART_FLAG_REACK) == RESET)
	{}  
  
	/* Wait until Transmit enable acknowledge flag is set */  
	while(USART_GetFlagStatus(USART2, USART_FLAG_TEACK) == RESET)
	{} 	
}


void uart2_init(void)
{
	uart2_gpio_config();
	uart2_config();
}


void USART2_IRQHandler(void)
{
	USART_ClearITPendingBit(USART2, USART_IT_ORE);
//	if(USART_GetITStatus(USART2, USART_IT_ORE) == SET){
//		USART_ClearITPendingBit(USART2, USART_IT_ORE);
//		USART_ReceiveData(USART2);
//		uart2_rx_fifo.length = 0;
//	}
	
	//发生接收中断
	if(USART_GetITStatus(USART2, USART_IT_RXNE) == SET){
		//清除中断标志位    
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);   
		uart2_rx_fifo.data[uart2_rx_fifo.length] = USART_ReceiveData(USART2);
		uart2_rx_fifo.length++;
		uart2_rx_fifo.length %= DATA_FIFO_SIZE;
	}
	
	if(USART_GetITStatus(USART2, USART_IT_IDLE) == SET){
		//清除中断标志
		USART_ClearITPendingBit(USART2, USART_IT_IDLE);
		USART_ReceiveData(USART2);
//		modbus_msg_deal_handler(uart2_rx_fifo.data,uart2_rx_fifo.length);
		uart2_rx_fifo.length = 0;	
	}
	

	//发生完成中断
	if(USART_GetITStatus(USART2, USART_IT_TC) == SET)
	{
		//清除中断标志
		USART_ClearITPendingBit(USART2, USART_IT_TC);
		
	}
}



