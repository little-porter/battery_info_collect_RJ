#include "adc.h"
#include "battery_info.h"

#define ADC_WAVE_SIZE 	WAVE_SAMPLES

/**************************************************************************************************************
	adc对应关系
**************************************************************************************************************/
/*	AD_SMK -->  PA6 --> ADC2_IN3
	AD_H2  -->  PA7 --> ADC2_IN4
	AD_CO  -->  PB0 --> ADC3_IN12
	
	adc_V+ -->  PB11 --> 跟随器
	adc_V- -->  PB10\PB2 --> ADC2_IN12
	adc_V  -->  PB12 --> ADC4_IN3
	
	adc_sr101 --> PB13 --> ADC3_IN5
	adc_sr102 --> PB14 --> ADC4_IN4
	NTC		  --> PB15 --> ADC4_IN5
*/
/**************************************************************************************************************
	adc采集缓存区域
**************************************************************************************************************/
//uint16_t adc1_val[FFT_COUNT];
private uint16_t adc2_val[3];
private uint16_t adc3_val[ADC_WAVE_SIZE][2];
private uint16_t adc4_val[2];
private uint16_t adc_co;
private uint8_t  co_full_flag = 0;


/**************************************************************************************************************
	adc硬件初始化
**************************************************************************************************************/
void adc_gpio_config(void)
{
	GPIO_InitTypeDef    GPIO_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;						//AD_SMK;AD_H2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;					//AD_CO;adc_V-;adc_V+;adc_sr101;adc_sr102  //GPIO_Pin_2
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**************************************************************************************************************
	adc芯片内部跟随器配置，映射无adc功能的引脚到有adc功能引脚
**************************************************************************************************************/
void adc_opamp_config(void)
{
	/* 3. 配置OPAMP4为跟随器模式 */
    OPAMP_InitTypeDef OPAMP_InitStruct;

    // 选择OPAMP4
    OPAMP_DeInit(OPAMP_Selection_OPAMP4);
    OPAMP_StructInit(&OPAMP_InitStruct);

    OPAMP_InitStruct.OPAMP_InvertingInput = OPAMP_InvertingInput_Vout; 			// 反相端连接Vout，跟随器模式
    OPAMP_InitStruct.OPAMP_NonInvertingInput = OPAMP_NonInvertingInput_IO2; 	 /*!< IO2 (PA5 for OPAMP1, PB14 for OPAMP2, PA5 for OPAMP3, PB11 for OPAMP4)*/ //引脚映射关系选择

    OPAMP_Init(OPAMP_Selection_OPAMP4, &OPAMP_InitStruct);

	
//	OPAMP_PGAConfig(OPAMP_Selection_OPAMP4,OPAMP_OPAMP_PGAGain_2,OPAMP_PGAConnect_IO1);
	//校准
	OPAMP_StartCalibration(OPAMP_Selection_OPAMP4,ENABLE);
	
	OPAMP_StartCalibration(OPAMP_Selection_OPAMP4,DISABLE);
	
    /* 5. 使能OPAMP4 */
    OPAMP_Cmd(OPAMP_Selection_OPAMP4, ENABLE);	
}
/**************************************************************************************************************
	adc2 dma功能配置
**************************************************************************************************************/
void adc2_dma2_ch1_config(void)
{
	DMA_InitTypeDef        DMA_InitStructure;
//	NVIC_InitTypeDef      NVIC_InitStructure;
	/* Enable DMA1 clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);

	/* DMA configuration */
	/* DMA1 Channel1 Init Test */
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC2->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&adc2_val;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 2;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	
	
//	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel2_IRQn;   	 
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//	
//	DMA_ITConfig(DMA2_Channel2,DMA_IT_TC,ENABLE);
	
	DMA_Init(DMA2_Channel1, &DMA_InitStructure);
}
/**************************************************************************************************************
	adc3  dma功能配置
**************************************************************************************************************/
void adc3_dma2_ch5_config(void)
{
	DMA_InitTypeDef        DMA_InitStructure;
	NVIC_InitTypeDef    NVIC_InitStructure;
	/* Enable DMA1 clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);

	/* DMA configuration */
	/* DMA1 Channel1 Init Test */
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC3->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&adc3_val;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = ADC_WAVE_SIZE*2;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel5_IRQn;   	 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	DMA_ITConfig(DMA2_Channel5,DMA_IT_TC,ENABLE);
	
	DMA_Init(DMA2_Channel5, &DMA_InitStructure); 
	
}

/**************************************************************************************************************
	adc4 dma功能配置
**************************************************************************************************************/
void adc4_dma2_ch2_config(void)
{
	DMA_InitTypeDef        DMA_InitStructure;
//	NVIC_InitTypeDef      NVIC_InitStructure;
	/* Enable DMA1 clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);

	/* DMA configuration */
	/* DMA1 Channel1 Init Test */
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC4->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&adc4_val;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 2;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	
	
//	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel2_IRQn;   	 
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
	
//	DMA_ITConfig(DMA2_Channel2,DMA_IT_TC,ENABLE);
	
	DMA_Init(DMA2_Channel2, &DMA_InitStructure); 
}




/**************************************************************************************************************
	adc2 初始化配置
**************************************************************************************************************/
void adc2_config(void)
{
	
	ADC_InitTypeDef       ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	
	ADC_VoltageRegulatorCmd(ADC2, ENABLE);
	
//	delay(1000);
	//Delay(10);
	
	/* 校准 */
	ADC_SelectCalibrationMode(ADC2, ADC_CalibrationMode_Single);
	ADC_StartCalibration(ADC2);
	
	while(ADC_GetCalibrationStatus(ADC2) != RESET );
	
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;                                                                    
	ADC_CommonInitStructure.ADC_Clock = ADC_Clock_AsynClkMode;                    
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_1;            
	ADC_CommonInitStructure.ADC_DMAMode = ADC_DMAMode_Circular;                  
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = 10;          
	ADC_CommonInit(ADC2, &ADC_CommonInitStructure);         
	
	ADC_InitStructure.ADC_ContinuousConvMode = ADC_ContinuousConvMode_Enable;
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b; 
	ADC_InitStructure.ADC_ExternalTrigConvEvent = ADC_ExternalTrigConvEvent_0;  		//软件触发，此参数无效     
	ADC_InitStructure.ADC_ExternalTrigEventEdge = ADC_ExternalTrigEventEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_OverrunMode = ADC_OverrunMode_Disable;   
	ADC_InitStructure.ADC_AutoInjMode = ADC_AutoInjec_Disable;  
    ADC_InitStructure.ADC_NbrOfRegChannel = 2;
	ADC_Init(ADC2, &ADC_InitStructure);
	
	
	ADC_RegularChannelConfig(ADC2, ADC_Channel_3, 1, ADC_SampleTime_181Cycles5);  	   //SMK
	ADC_RegularChannelConfig(ADC2, ADC_Channel_4, 2, ADC_SampleTime_181Cycles5);  	   //H2
//	ADC_RegularChannelConfig(ADC2, ADC_Channel_12, 3, ADC_SampleTime_181Cycles5);  	   //V-
	
	ADC_DMAConfig(ADC2, ADC_DMAMode_Circular);
	ADC_DMACmd(ADC2, ENABLE);
	
	
	ADC_Cmd(ADC2, ENABLE);
	
	
	while(!ADC_GetFlagStatus(ADC2, ADC_FLAG_RDY));
	
	
	DMA_Cmd(DMA2_Channel1, ENABLE);
	
	ADC_StartConversion(ADC2);
	
	
}

/**************************************************************************************************************
	adc3 初始化配置
**************************************************************************************************************/
void adc3_config(void)
{
	
	ADC_InitTypeDef       ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
					
	
	ADC_VoltageRegulatorCmd(ADC3, ENABLE);
	
//	delay(1000);
	//Delay(10);
	
	/* 校准 */
	ADC_SelectCalibrationMode(ADC3, ADC_CalibrationMode_Single);
	ADC_StartCalibration(ADC3);
	
	while(ADC_GetCalibrationStatus(ADC3) != RESET );
//	calibration_value_2 = ADC_GetCalibrationValue(ADC3);
	
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;                                                                    
	ADC_CommonInitStructure.ADC_Clock = ADC_Clock_AsynClkMode;                    
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_1;            
	ADC_CommonInitStructure.ADC_DMAMode = ADC_DMAMode_Circular;                  
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = 10;               
	ADC_CommonInit(ADC3, &ADC_CommonInitStructure);
	
	ADC_InitStructure.ADC_ContinuousConvMode = ADC_ContinuousConvMode_Disable;
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b; 
	ADC_InitStructure.ADC_ExternalTrigConvEvent = ADC_ExternalTrigConvEvent_7; 				//TIM2
	ADC_InitStructure.ADC_ExternalTrigEventEdge = ADC_ExternalTrigEventEdge_RisingEdge;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_OverrunMode = ADC_OverrunMode_Disable;   
	ADC_InitStructure.ADC_AutoInjMode = ADC_AutoInjec_Disable;  
	ADC_InitStructure.ADC_NbrOfRegChannel = 2;
	
	ADC_Init(ADC3, &ADC_InitStructure);
		
	ADC_RegularChannelConfig(ADC3, ADC_Channel_5, 1, ADC_SampleTime_181Cycles5);  //adc_sr101
	ADC_RegularChannelConfig(ADC3, ADC_Channel_12, 2, ADC_SampleTime_181Cycles5);  //CO
	
	ADC_DMAConfig(ADC3, ADC_DMAMode_Circular);
	
	ADC_DMACmd(ADC3, ENABLE);
	
	ADC_Cmd(ADC3, ENABLE);
	
	while(!ADC_GetFlagStatus(ADC3, ADC_FLAG_RDY));
	
	DMA_Cmd(DMA2_Channel5, ENABLE);
	
	ADC_StartConversion(ADC3);
}
/**************************************************************************************************************
	adc4 初始化配置
**************************************************************************************************************/
void adc4_config(void)
{	
	ADC_InitTypeDef       ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	
	ADC_VoltageRegulatorCmd(ADC4, ENABLE);
	
//	delay(1000);
	//Delay(10);
	
	/* 校准 */
	ADC_SelectCalibrationMode(ADC4, ADC_CalibrationMode_Single);
	ADC_StartCalibration(ADC4);
	
	while(ADC_GetCalibrationStatus(ADC4) != RESET );
//	calibration_value_2 = ADC_GetCalibrationValue(ADC3);
	
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;                                                                    
	ADC_CommonInitStructure.ADC_Clock = ADC_Clock_AsynClkMode;                    
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_1;            
	ADC_CommonInitStructure.ADC_DMAMode = ADC_DMAMode_Circular;                  
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = 10;          
	ADC_CommonInit(ADC4, &ADC_CommonInitStructure);         
	
	ADC_InitStructure.ADC_ContinuousConvMode = ADC_ContinuousConvMode_Enable;
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b; 
	ADC_InitStructure.ADC_ExternalTrigConvEvent = ADC_ExternalTrigConvEvent_7;  		//软件触发，此参数无效     
	ADC_InitStructure.ADC_ExternalTrigEventEdge = ADC_ExternalTrigEventEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_OverrunMode = ADC_OverrunMode_Disable;   
	ADC_InitStructure.ADC_AutoInjMode = ADC_AutoInjec_Disable;  
    ADC_InitStructure.ADC_NbrOfRegChannel = 2;
	ADC_Init(ADC4, &ADC_InitStructure);
	
	
	ADC_RegularChannelConfig(ADC4, ADC_Channel_5, 1, ADC_SampleTime_181Cycles5);  	   //NTC
	ADC_RegularChannelConfig(ADC4, ADC_Channel_3, 2, ADC_SampleTime_181Cycles5);  	   //V+
	
	
	ADC_DMAConfig(ADC4, ADC_DMAMode_Circular);
	ADC_DMACmd(ADC4, ENABLE);
	
	
	ADC_Cmd(ADC4, ENABLE);
	
	
	while(!ADC_GetFlagStatus(ADC4, ADC_FLAG_RDY));
	
	
	DMA_Cmd(DMA2_Channel2, ENABLE);
	
	ADC_StartConversion(ADC4);

	
	
}
/**************************************************************************************************************
	adc硬件初始化
**************************************************************************************************************/
void adc_init(void)
{
	ADC_DeInit(ADC2);			//关闭所有ADC时钟
	ADC_DeInit(ADC3);
	ADC_DeInit(ADC4);
	RCC_ADCCLKConfig(RCC_ADC12PLLCLK_Div1);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ADC12, ENABLE);
	RCC_ADCCLKConfig(RCC_ADC34PLLCLK_Div1);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ADC34, ENABLE);
	
	
	adc_gpio_config();
	adc_opamp_config();
	/*内阻采集*/
	adc3_dma2_ch5_config();
	adc3_config();
	
	/*NTC采集配置*/
	adc4_dma2_ch2_config();
	adc4_config();
	
	/*气体采集配置*/
////	adc2_dma1_ch2_config();
	adc2_dma2_ch1_config();
	adc2_config();
}

uint32_t t=0;
void DMA2_Channel5_IRQHandler(void)
{
	static uint32_t t1 = 0,t2 = 0;
	
	if(DMA_GetITStatus(DMA2_IT_TC5) != RESET)
	{
		t2 = system_tick_get();
		t = t2 -t1;
		t1 = t2;
		DMA_ClearITPendingBit(DMA2_IT_TC5);
		if(battery_info_calculate_flag_get() == 0)
		{
			battery_inres_window_ad_full((uint16_t *)adc3_val,ADC_WAVE_SIZE);
			battery_info_calculate_flag_set();
		}
		if(co_full_flag == 0)
		{
			co_full_flag = 1;
			adc_co = adc3_val[0][1];
		}
	}
	
}


void DMA2_Channel2_IRQHandler(void)
{
	
	if(DMA_GetITStatus(DMA2_IT_TC2) != RESET)
	{
		DMA_ClearITPendingBit(DMA2_IT_TC2);
		if(battery_info_calculate_flag_get() == 0)
		{
			battery_inres_window_ad_full((uint16_t *)adc3_val,ADC_WAVE_SIZE);
			battery_info_calculate_flag_set();
		}
	}
	
}

/**************************************************************************************************************
	V+参数获取接口
**************************************************************************************************************/
uint16_t get_v_low_ad_value(void)
{
	return adc2_val[2];
}
/**************************************************************************************************************
	V-参数获取接口
**************************************************************************************************************/
uint16_t get_v_high_ad_value(void)
{
	return adc4_val[1];
}
/**************************************************************************************************************
	NTC参数获取接口
**************************************************************************************************************/
uint16_t get_ntc_ad_value(void)
{
	return adc4_val[0];
}

/**************************************************************************************************************
	smoke参数获取接口
**************************************************************************************************************/
uint16_t get_smoke_ad_value(void)
{
	return adc2_val[0];
}
/**************************************************************************************************************
	h2参数获取接口
**************************************************************************************************************/
uint16_t get_h2_ad_value(void)
{
	return adc2_val[1];
}
/**************************************************************************************************************
	co参数获取接口
**************************************************************************************************************/
uint16_t get_co_ad_value(void)
{
	co_full_flag = 0;
	return adc_co;
}

