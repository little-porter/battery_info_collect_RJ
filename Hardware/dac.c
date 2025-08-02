#include "dac.h"

#define DAC_REF			2047					//2047		2482	1551  1240
#define DAC_WAVE_SIZE	WAVE_SAMPLES

private uint16_t sineWave1[DAC_WAVE_SIZE];


/**************************************************************************************************************
	产生模拟1KHZ正弦波
**************************************************************************************************************/
void generateSineWave(uint16_t *buff,uint16_t amplitude,uint16_t offset,uint16_t samples)
{
	for(int i = 0;i < samples;i++)
	{
		buff[i] = (uint16_t)(amplitude * sin(2 * M_PI * i / samples) + offset);
	}
}

/**************************************************************************************************************
	生成模拟1KHZ方波
**************************************************************************************************************/
void generateSquareWave(uint16_t *buff,uint16_t amplitude,uint16_t offset,uint16_t samples)
{
	for(int i = 0;i < samples;i++)
	{
		if(i < samples/2)
		{
			buff[i] = offset + amplitude;
			
		}
		else
		{
			buff[i] = offset - amplitude;
		}
	}
}

/**************************************************************************************************************
	dac gpio初始化配置
**************************************************************************************************************/
void dac_gpio_config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* GPIOA clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
	
	/* Configure PA.04 (DAC_OUT1) as analog */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/**************************************************************************************************************
	dac dma配置
**************************************************************************************************************/
void dac_dma_config(void)
{
	DMA_DeInit(DMA2_Channel3);
	DMA_InitTypeDef         DMA_InitStructure;
//	NVIC_InitTypeDef 		NVIC_InitStructure; 
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
	
	/* DMA2 channel3 configuration */
	DMA_DeInit(DMA2_Channel3); 
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&DAC->DHR12R1;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&sineWave1[0];
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = DAC_WAVE_SIZE;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA2_Channel3, &DMA_InitStructure);
	
//	SYSCFG_DMAChannelRemapConfig(SYSCFG_DMARemap_TIM6DAC1Ch1,ENABLE);
//	SYSCFG_DMAChannelRemapConfig(SYSCFG_DMARemap_TIM7DAC1Ch2,ENABLE);
	
//	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel3_IRQn;   	 
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//	
//	DMA_ITConfig(DMA1_Channel3,DMA_IT_TC,ENABLE);
   

	/* Enable DMA2 Channel3 */
	DMA_Cmd(DMA2_Channel3, ENABLE);
   
}

/**************************************************************************************************************
	DAC 配置
**************************************************************************************************************/
void dac_config(void)
{
	DAC_InitTypeDef            DAC_InitStructure;
	DAC_DeInit(DAC1);
	/* DAC Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC1, ENABLE);
	 
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_T2_TRGO;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
	DAC_InitStructure.DAC_Buffer_Switch = DAC_BufferSwitch_Disable;

	/* DAC Channel2 Init */
	DAC_Init(DAC1, DAC_Channel_1, &DAC_InitStructure);

	/* Enable DAC Channel2 */
	DAC_Cmd(DAC1, DAC_Channel_1, ENABLE);
	

	/* Enable DMA for DAC Channel2 */
	DAC_DMACmd(DAC1, DAC_Channel_1, ENABLE);
	
	/* Escalator Wave generator ----------------------------------------*/

	/* DAC channel1 Configuration */
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
	DAC_InitStructure.DAC_Buffer_Switch = DAC_BufferSwitch_Enable;;
	DAC_Init(DAC1, DAC_Channel_2, &DAC_InitStructure);
	
	
	DAC_Cmd(DAC1, DAC_Channel_2, ENABLE);
	DAC_SetChannel2Data(DAC1, DAC_Align_12b_R, DAC_REF);

	
}

/**************************************************************************************************************
	DAC 硬件初始化
**************************************************************************************************************/
void dac_init(void)
{
//	generateSineWave(sineWave1,32,DAC_REF,128);
	generateSquareWave(sineWave1,310,DAC_REF,DAC_WAVE_SIZE);			//310 248
	dac_gpio_config();
	dac_config();
	dac_dma_config();
}




