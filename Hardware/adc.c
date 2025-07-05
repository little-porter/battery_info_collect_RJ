#include "adc.h"

uint16_t adc1_val[FFT_COUNT];
uint16_t adc2_val[3];
uint16_t adc3_val[FFT_COUNT];


/*	AD_SMK -->  PA6 --> ADC2_IN3
	AD_H2  -->  PA7 --> ADC2_IN4
	AD_CO  -->  PB0 --> ADC3_IN12
	
	adc_V+ -->  PB11 --> 
	adc_V- -->  PB10
	adc_V  -->  PB12 --> ADC4_IN3
	
	adc_sr101 --> PB13 --> ADC3_IN5
	adc_sr102 --> PB14 --> ADC4_IN4

*/
void adc_gpio_config(void)
{
	GPIO_InitTypeDef    GPIO_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;						//AD_SMK;AD_H2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_10 | GPIO_Pin_11  \
								  | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;					//AD_CO;adc_V-;adc_V+;adc_sr101;adc_sr102
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void adc_opamp_config(void)
{
	/* 3. 配置OPAMP4为跟随器模式 */
    OPAMP_InitTypeDef OPAMP_InitStruct;

    // 选择OPAMP4
    OPAMP_DeInit(OPAMP_Selection_OPAMP4);
    OPAMP_StructInit(&OPAMP_InitStruct);

    OPAMP_InitStruct.OPAMP_InvertingInput = OPAMP_InvertingInput_IO1; 			// VINM = PB11
    OPAMP_InitStruct.OPAMP_NonInvertingInput = OPAMP_NonInvertingInput_IO1; 	// VINP = PB10
//    OPAMP_InitStruct.OPAMP_Output_Trimming = OPAMP_Trimming_Factory;

    // 设置为跟随器模式（Gain=1）
//    OPAMP_InitStruct.OPAMP_Mode = OPAMP_Mode_Follower;

    OPAMP_Init(OPAMP_Selection_OPAMP4, &OPAMP_InitStruct);

    /* 4. 校准（可选） */
    // 如果需要更精确的输出，可以调用校准函数
    // OPAMP_OffsetTrim_Cmd(OPAMP4, ENABLE);

    /* 5. 使能OPAMP4 */
    OPAMP_Cmd(OPAMP_Selection_OPAMP4, ENABLE);	
}

void adc3_dma2_ch5_config(void)
{
	DMA_InitTypeDef        DMA_InitStructure;
	NVIC_InitTypeDef    NVIC_InitStructure;
	/* Enable DMA1 clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);

	/* DMA configuration */
	/* DMA1 Channel1 Init Test */
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC3->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&adc3_val[0];
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = FFT_COUNT;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	
	
//	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel5_IRQn;   	 
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
	
//	DMA_ITConfig(DMA2_Channel5,DMA_IT_TC,ENABLE);
	
	DMA_Init(DMA2_Channel5, &DMA_InitStructure); 
	
}



void adc_config(void)
{
	ADC_InitTypeDef       ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	
	RCC_ADCCLKConfig(RCC_ADC12PLLCLK_Div1);  
	RCC_ADCCLKConfig(RCC_ADC34PLLCLK_Div1);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ADC12, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ADC34, ENABLE);
					
	//ADC_Cmd(ADC1, DISABLE);
	//ADC_Cmd(ADC2, DISABLE);
	
	ADC_VoltageRegulatorCmd(ADC1, ENABLE);
	ADC_VoltageRegulatorCmd(ADC2, ENABLE);
	ADC_VoltageRegulatorCmd(ADC3, ENABLE);
	
//	delay(1000);
	//Delay(10);
	
	/* 校准 */
	ADC_SelectCalibrationMode(ADC1, ADC_CalibrationMode_Differential);
	ADC_StartCalibration(ADC1);
	
	ADC_SelectCalibrationMode(ADC2, ADC_CalibrationMode_Differential);
	ADC_StartCalibration(ADC2);
	
	ADC_SelectCalibrationMode(ADC3, ADC_CalibrationMode_Differential);
	ADC_StartCalibration(ADC3);
	
	while(ADC_GetCalibrationStatus(ADC1) != RESET );
//	calibration_value_1 = ADC_GetCalibrationValue(ADC1);

	while(ADC_GetCalibrationStatus(ADC2) != RESET );
//	calibration_value_2 = ADC_GetCalibrationValue(ADC2);
	
	while(ADC_GetCalibrationStatus(ADC3) != RESET );
//	calibration_value_2 = ADC_GetCalibrationValue(ADC3);
	
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;                                                                    
	ADC_CommonInitStructure.ADC_Clock = ADC_Clock_AsynClkMode;                    
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_1;            
	ADC_CommonInitStructure.ADC_DMAMode = ADC_DMAMode_Circular;                  
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = 10;          
	ADC_CommonInit(ADC1, &ADC_CommonInitStructure);         
	ADC_CommonInit(ADC2, &ADC_CommonInitStructure);      
	ADC_CommonInit(ADC3, &ADC_CommonInitStructure);
	
	ADC_InitStructure.ADC_ContinuousConvMode = ADC_ContinuousConvMode_Disable;
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b; 
	ADC_InitStructure.ADC_ExternalTrigConvEvent = ADC_ExternalTrigConvEvent_11; //TIM2        
	ADC_InitStructure.ADC_ExternalTrigEventEdge = ADC_ExternalTrigEventEdge_RisingEdge;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_OverrunMode = ADC_OverrunMode_Disable;   
	ADC_InitStructure.ADC_AutoInjMode = ADC_AutoInjec_Disable;  
    ADC_InitStructure.ADC_NbrOfRegChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	ADC_InitStructure.ADC_NbrOfRegChannel = 3;
	ADC_InitStructure.ADC_ContinuousConvMode = ADC_ContinuousConvMode_Disable;
	ADC_InitStructure.ADC_ExternalTrigConvEvent = ADC_ExternalTrigConvEvent_9; //TIM1 
	ADC_InitStructure.ADC_ExternalTrigEventEdge = ADC_ExternalTrigEventEdge_RisingEdge;

	ADC_Init(ADC2, &ADC_InitStructure);
	ADC_InitStructure.ADC_NbrOfRegChannel = 1;
	ADC_Init(ADC3, &ADC_InitStructure);
		
////	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_181Cycles5);  //10r
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 1, ADC_SampleTime_181Cycles5);    //sr101 
//	//ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_181Cycles5);  //sr103
//			
//	ADC_RegularChannelConfig(ADC2, ADC_Channel_3, 3, ADC_SampleTime_181Cycles5);     //adc-
//	ADC_RegularChannelConfig(ADC2, ADC_Channel_4, 2, ADC_SampleTime_181Cycles5);     //adc+
//	ADC_RegularChannelConfig(ADC2, ADC_Channel_12, 1, ADC_SampleTime_181Cycles5);     //烟雾
//	
//	ADC_RegularChannelConfig(ADC3, ADC_Channel_1, 2, ADC_SampleTime_601Cycles5);   //h2
//	ADC_RegularChannelConfig(ADC3, ADC_Channel_12, 1, ADC_SampleTime_601Cycles5);  //co
	
	ADC_RegularChannelConfig(ADC3, ADC_Channel_5, 1, ADC_SampleTime_181Cycles5);  //co
	
	
	ADC_DMAConfig(ADC1, ADC_DMAMode_Circular);
	ADC_DMACmd(ADC1, ENABLE);
	
	ADC_DMAConfig(ADC2, ADC_DMAMode_Circular);
	ADC_DMACmd(ADC2, ENABLE);
	
	ADC_DMAConfig(ADC3, ADC_DMAMode_Circular);
	ADC_DMACmd(ADC3, ENABLE);
	

//	ADC_Cmd(ADC1, ENABLE);
//	ADC_Cmd(ADC2, ENABLE);
	ADC_Cmd(ADC3, ENABLE);
	
	
//	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_RDY));
//	while(!ADC_GetFlagStatus(ADC2, ADC_FLAG_RDY));
	while(!ADC_GetFlagStatus(ADC3, ADC_FLAG_RDY));
	
	
//	DMA_Cmd(DMA1_Channel1, ENABLE);
//	DMA_Cmd(DMA2_Channel1, ENABLE);
	DMA_Cmd(DMA2_Channel5, ENABLE);
	
//	ADC_StartConversion(ADC1); 
//	ADC_StartConversion(ADC2); 
	ADC_StartConversion(ADC3);

}


void adc_init(void)
{
	adc_gpio_config();
	adc_opamp_config();
	adc3_dma2_ch5_config();
	adc_config();
}




