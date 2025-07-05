/**
  ******************************************************************************
  * @file    ADC/ADC_AnalogWatchdog/stm32f30x_it.c
  * @author  MCD Application Team
  * @version V1.1.3
  * @date    14-December-2021
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2015 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f30x_it.h"
#include "mcu_init.h"
#include "stdint.h"
#include "string.h"
#include "adc.h"
#include "usart.h"
#include "main.h"
#include <stdlib.h>
#include <stdbool.h>


#include "serial.h"
#include "fifo.h"
#include "console.h"

/** @addtogroup STM32F30x_StdPeriph_Examples
  * @{
  */

/** @addtogroup AWDG_Mode
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern __IO uint32_t TimingDelay;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	TimingDelay--;
	system_incTick();
}

/******************************************************************************/
/*                 STM32F30x Peripherals Interrupt Handlers                   */
/******************************************************************************/

/******************************************************************************/
/*                 STM32F30x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f30x.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles ADC1 global interrupts requests.
  * @param  None
  * @retval None
  */
uint8_t sr_count = 0,lock = 0,ccc = 0;
uint16_t adc_value;
uint16_t adc11_value[3];

uint16_t max,min,dt_v;
static bool adc_flag = true;
static bool dma_flag = true;
static bool AdcStartFlag = false;
extern uint8_t  V_flag;


extern float V;
double BuffWindow[10];
int index2 = 0;
double sum2 = 0.0;
double filteredData2;
uint8_t windwosbuffman2 = 0;

//计算电池电压
void TIM1_UP_TIM16_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
	{
		static uint8_t j = 0,i = 0;
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);

		_V.data = (adc2_val[1] - adc2_val[2]) * 101.0f * 3.3f / 4096.f;
		filteredData2 = movingAverageFilter(_V.data, BuffWindow, &index2, &sum2,&windwosbuffman2);
		usRegInputBuf[2] = _V.buff[1];
		usRegInputBuf[3] = _V.buff[0];
	}
}

uint16_t ass[FFT_COUNT];
void DMA1_Channel1_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC1) != RESET)
	{
		DMA_ClearITPendingBit(DMA1_IT_TC1);
		static uint8_t f_count = 0;
		f_count++;
		if((f_count == FFT_COUNT) && (dma_flag == true))
		{
			fft_flag = 1;
		}
		f_count %= FFT_COUNT;

		ADC_StopConversion(ADC1);
		DMA_Cmd(DMA1_Channel1,DISABLE);
		DMA_SetCurrDataCounter(DMA1_Channel1,FFT_COUNT);
      
		DMA_Cmd(DMA1_Channel3,DISABLE);
		DMA_SetCurrDataCounter(DMA1_Channel3,FFT_COUNT);
		DMA_Cmd(DMA1_Channel3,ENABLE);
      
		DMA_Cmd(DMA1_Channel1,ENABLE);
		ADC_StartConversion(ADC1);
   
	}
}

void DMA1_Channel3_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC3) != RESET)
	{
		DMA_ClearITPendingBit(DMA1_IT_TC3);
		DMA_Cmd(DMA1_Channel3,DISABLE);
		DMA_SetCurrDataCounter(DMA1_Channel3,FFT_COUNT);
		DMA_Cmd(DMA1_Channel3,ENABLE);

	}
}



void USART1_IRQHandler(void)
{
	uint16_t crc16 = 0;
	uint8_t tmp = 0;
	
	USART_ClearITPendingBit(USART1, USART_IT_ORE);
	//发生接收中断
	if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
    //清除中断标志位    
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);   
		tmp = USART_ReceiveData(USART1); 
		uart1_rx_st.data[uart1_rx_st.now_Count++] = USART_ReceiveData(USART1); 
		uart1_rx_st.now_Count %= UART_RX_BUF_LEN;
	}
	
	if(USART_GetITStatus(USART1, USART_IT_IDLE) == SET)
	{
		//清除中断标志
		USART_ClearITPendingBit(USART1, USART_IT_IDLE);
		USART_ReceiveData(USART1);
		
		
		if(uart1_rx_st.data[0] == SlaveAddress)
		{
			crc16 = getcrc16(uart1_rx_st.data,uart1_rx_st.now_Count-2);
			if(((crc16 >> 8) == uart1_rx_st.data[uart1_rx_st.now_Count -2]) && ((crc16 & 0x00ff) == uart1_rx_st.data[uart1_rx_st.now_Count -1]))
			{
				ModBusDeal_uart1(uart1_rx_st.data,uart1_rx_st.now_Count);
			}
		}
		uart1_rx_st.now_Count = 0;
	}
	

//	//发生完成中断
//	if(USART_GetITStatus(USART1, USART_IT_TC) == SET)
//	{
//		//清除中断标志
//		USART_ClearITPendingBit(USART1, USART_IT_TC);
//		
//		if(uart1_tx_struct.buflen[uart1_tx_struct.now_Count] != 0)
//		{
//			USART_SendData(USART1, uart1_tx_struct.bufDate[uart1_tx_struct.now_Count][uart1_tx_struct.SendIndex[uart1_tx_struct.now_Count]]);
//			uart1_tx_struct.SendIndex[uart1_tx_struct.now_Count]++;
//			uart1_tx_struct.buflen[uart1_tx_struct.now_Count]--;
//		}
//		if(uart1_tx_struct.buflen[uart1_tx_struct.now_Count] == 0)
//		{
////			USART_ITConfig(USART1, USART_IT_TC, ENABLE);
////			USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
//			uart1_tx_struct.now_Count++;
//			uart1_tx_struct.now_Count %= 20;
//		}
//  }
}





