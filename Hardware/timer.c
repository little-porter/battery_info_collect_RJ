#include "timer.h"



void timer3_config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure; 
			
    NVIC_InitStructure.NVIC_IRQChannel 	= TIM3_IRQn ;				// 设置中断来源
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;		// 设置主优先级为 0 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;				// 设置抢占优先级为3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
		
	// 开启定时器时钟,即内部时钟CK_INT=72M
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
		
    TIM_TimeBaseStructure.TIM_Period = 49;							// 自动重装载寄存器的值，累计TIM_Period+1个频率后产生一个更新或者中断	
    TIM_TimeBaseStructure.TIM_Prescaler= 0;							// 时钟预分频数为
    TIM_TimeBaseStructure.TIM_ClockDivision=0;						// 时钟分频因子 ，基本定时器没有，不用管
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 		// 计数器计数模式，基本定时器只能向上计数，没有计数模式的设置
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;					// 重复计数器的值，基本定时器没有，不用管

    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);					// 初始化定时器
		
	TIM_PrescalerConfig(TIM3, 7200-1, TIM_PSCReloadMode_Update);
		
	//TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update); 		// 使用更新事件作为 TRGO 信号
		
		
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);							// 清除计数器中断标志位
    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);						// 开启计数器中断：计数器溢出、产生更新事件、计数器的更新事件能够产生中断、并被使能	
		
    TIM_Cmd(TIM3, ENABLE);											// 使能计数器
}

void timer1_config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure; 
		
   		
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM16_IRQn ;		// 设置中断来源	
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;		// 设置主优先级为 0 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;				// 设置抢占优先级为3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
		
		
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);			// 开启定时器时钟,即内部时钟CK_INT=72M
	
    TIM_TimeBaseStructure.TIM_Period = 99;							// 自动重装载寄存器的值，累计TIM_Period+1个频率后产生一个更新或者中断
    TIM_TimeBaseStructure.TIM_Prescaler= 0;							// 时钟预分频数为
    TIM_TimeBaseStructure.TIM_ClockDivision=0;						// 时钟分频因子 ，基本定时器没有，不用管
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 		// 计数器计数模式，基本定时器只能向上计数，没有计数模式的设置
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;					// 重复计数器的值，基本定时器没有，不用管
	
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);					// 初始化定时器
		
	TIM_PrescalerConfig(TIM1, 3600-1, TIM_PSCReloadMode_Update);
		
	TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_Update); 			// 使用更新事件作为 TRGO 信号
		
    TIM_ClearFlag(TIM1, TIM_FLAG_Update);							// 清除计数器中断标志位
	  
    TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);						// 开启计数器中断：计数器溢出、产生更新事件、计数器的更新事件能够产生中断、并被使能
		
    TIM_Cmd(TIM1, ENABLE);											// 使能计数器
}

void timer2_config(void)
{
	TIM_DeInit(TIM2);
	TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
  
	/* TIM2 Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
  
	/* Time base configuration */
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); 
	TIM_TimeBaseStructure.TIM_Period = T_DIV_FREQUENCY;			       
	TIM_TimeBaseStructure.TIM_Prescaler = 1-1;       
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;    
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

//	TIM_PrescalerConfig(TIM2, 0, TIM_PSCReloadMode_Update);

	/* TIM2 TRGO selection */
	TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);
  
	
//	TIM_OCInitTypeDef TIM_OCInitStructure;				 			//初始化比较单元
//	TIM_OCStructInit(&TIM_OCInitStructure);							//给结构体赋初始值       
//	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;				//设置输出比较的模式
//	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;		//设置输出比较的极性
//	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//设置输出使能
//	TIM_OCInitStructure.TIM_Pulse = 200;							//设置CCR的
//	//	TIM_SetCompare1(TIM2,1000);
//	
//	
//	TIM_OC2Init(TIM2,&TIM_OCInitStructure);
	
	TIM_Cmd(TIM2, ENABLE);
	TIM_ClearFlag(TIM2, TIM_FLAG_Update|TIM_FLAG_CC1|TIM_FLAG_CC2|TIM_FLAG_CC3|TIM_FLAG_CC4);
}



void timer_init(void)
{
//	timer1_config();
	timer2_config();
//	timer3_config();
}

void TIM2_IRQHandler(void)
{
//	TIM_ClearFlag(TIM2, TIM_FLAG_Update|TIM_FLAG_CC1|TIM_FLAG_CC2|TIM_FLAG_CC3|TIM_FLAG_CC4);
	if(TIM_GetFlagStatus(TIM2,TIM_FLAG_Update))
	{
		TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	}	
}


