#include "timer.h"



void timer3_config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure; 
			
    NVIC_InitStructure.NVIC_IRQChannel 	= TIM3_IRQn ;				// �����ж���Դ
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;		// ���������ȼ�Ϊ 0 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;				// ������ռ���ȼ�Ϊ3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
		
	// ������ʱ��ʱ��,���ڲ�ʱ��CK_INT=72M
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
		
    TIM_TimeBaseStructure.TIM_Period = 49;							// �Զ���װ�ؼĴ�����ֵ���ۼ�TIM_Period+1��Ƶ�ʺ����һ�����»����ж�	
    TIM_TimeBaseStructure.TIM_Prescaler= 0;							// ʱ��Ԥ��Ƶ��Ϊ
    TIM_TimeBaseStructure.TIM_ClockDivision=0;						// ʱ�ӷ�Ƶ���� ��������ʱ��û�У����ù�
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 		// ����������ģʽ��������ʱ��ֻ�����ϼ�����û�м���ģʽ������
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;					// �ظ���������ֵ��������ʱ��û�У����ù�

    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);					// ��ʼ����ʱ��
		
	TIM_PrescalerConfig(TIM3, 7200-1, TIM_PSCReloadMode_Update);
		
	//TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update); 		// ʹ�ø����¼���Ϊ TRGO �ź�
		
		
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);							// ����������жϱ�־λ
    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);						// �����������жϣ���������������������¼����������ĸ����¼��ܹ������жϡ�����ʹ��	
		
    TIM_Cmd(TIM3, ENABLE);											// ʹ�ܼ�����
}

void timer1_config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure; 
		
   		
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM16_IRQn ;		// �����ж���Դ	
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;		// ���������ȼ�Ϊ 0 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;				// ������ռ���ȼ�Ϊ3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
		
		
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);			// ������ʱ��ʱ��,���ڲ�ʱ��CK_INT=72M
	
    TIM_TimeBaseStructure.TIM_Period = 99;							// �Զ���װ�ؼĴ�����ֵ���ۼ�TIM_Period+1��Ƶ�ʺ����һ�����»����ж�
    TIM_TimeBaseStructure.TIM_Prescaler= 0;							// ʱ��Ԥ��Ƶ��Ϊ
    TIM_TimeBaseStructure.TIM_ClockDivision=0;						// ʱ�ӷ�Ƶ���� ��������ʱ��û�У����ù�
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 		// ����������ģʽ��������ʱ��ֻ�����ϼ�����û�м���ģʽ������
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;					// �ظ���������ֵ��������ʱ��û�У����ù�
	
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);					// ��ʼ����ʱ��
		
	TIM_PrescalerConfig(TIM1, 3600-1, TIM_PSCReloadMode_Update);
		
	TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_Update); 			// ʹ�ø����¼���Ϊ TRGO �ź�
		
    TIM_ClearFlag(TIM1, TIM_FLAG_Update);							// ����������жϱ�־λ
	  
    TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);						// �����������жϣ���������������������¼����������ĸ����¼��ܹ������жϡ�����ʹ��
		
    TIM_Cmd(TIM1, ENABLE);											// ʹ�ܼ�����
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
  
	
//	TIM_OCInitTypeDef TIM_OCInitStructure;				 			//��ʼ���Ƚϵ�Ԫ
//	TIM_OCStructInit(&TIM_OCInitStructure);							//���ṹ�帳��ʼֵ       
//	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;				//��������Ƚϵ�ģʽ
//	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;		//��������Ƚϵļ���
//	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//�������ʹ��
//	TIM_OCInitStructure.TIM_Pulse = 200;							//����CCR��
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


