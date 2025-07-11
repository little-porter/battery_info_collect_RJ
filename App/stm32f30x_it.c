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

#include "stdint.h"
#include "string.h"
#include "adc.h"

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


//º∆À„µÁ≥ÿµÁ—π
void TIM1_UP_TIM16_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
	{
//		static uint8_t j = 0,i = 0;
//		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);

//		_V.data = (adc2_val[1] - adc2_val[2]) * 101.0f * 3.3f / 4096.f;
//		filteredData2 = movingAverageFilter(_V.data, BuffWindow, &index2, &sum2,&windwosbuffman2);
//		usRegInputBuf[2] = _V.buff[1];
//		usRegInputBuf[3] = _V.buff[0];
	}
}





