/***********************************************************************
文件名称：LED.C
功    能：led  IO初始化
编写时间：2013.4.25
编 写 人：
注    意：
***********************************************************************/
#include "stm32f10x.h"
void PWM_Configuration()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE|RCC_APB2Periph_TIM1, ENABLE);	
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM1, ENABLE); //Timer1?????  ?????????? tim1??????? ???tim1
	

	
		GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_9 | GPIO_Pin_11  ;  		//
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_Init(GPIOE, &GPIO_InitStructure);	
	
		//TIM_DeInit(TIM1); //
	
    //配置时间基，按此配置，TIM2频率为72M/TIM_Period
		TIM_TimeBaseStructure.TIM_Period = 10000;   						//周期       
		TIM_TimeBaseStructure.TIM_Prescaler = 71;                        //分频10K
		TIM_TimeBaseStructure.TIM_ClockDivision = 0;                    //时钟分割为0  
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;     //向上计数模式    
		TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); 

    //配置输出模式
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;               //脉冲宽度调制模式1 
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   //输出状态使能  
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;        //输出极性低


		TIM_OC1Init(TIM1, &TIM_OCInitStructure);						//T3-CH1
		TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);		


		TIM_Cmd(TIM1, ENABLE) ;

             
		TIM_CtrlPWMOutputs(TIM1,ENABLE);        //TIM1定时器

   		
}


void PWM_test()
{
	while(1)
	{
		TIM1->CCR1 =  5000; //时钟计数值最大值5000，与电压最大值5000范围相同，所以直接赋值

	}
}