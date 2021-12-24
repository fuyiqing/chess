/***********************************************************************
文件名称：LED.C
功    能：led  IO初始化
编写时间：2013.4.25
编 写 人：
注    意：
***********************************************************************/
#include "stm32f10x.h"

void Counter_Configuration()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_ClocksTypeDef RCC_ClockFreq;
	TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;	
	TIM_ICInitTypeDef TIM_ICInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	
	
	/*PB6-> ?????? */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //50M????
	GPIO_Init(GPIOB, &GPIO_InitStructure);
		
	
	  TIM_TimeBaseStructure.TIM_Period = 0x1000;   //ARR,     
	  TIM_TimeBaseStructure.TIM_Prescaler = 0;       
	  TIM_TimeBaseStructure.TIM_ClockDivision = 0;    
	  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 
	
	  TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	  TIM_ICInitStructure.TIM_ICFilter = 0x0;
	  
	  TIM_ICInit(TIM4, &TIM_ICInitStructure);
	  TIM_SelectInputTrigger(TIM4, TIM_TS_TI1FP1);
	
	  /* Select the slave Mode: Reset Mode */
	  TIM4->SMCR |= 0x07 ;  //外部触发模式
	  /* Enable the Master/Slave Mode */

	TIM_SetCounter(TIM4,0);
	TIM_Cmd(TIM4, ENABLE);
	
}


int TimeCnt=0;
void Counter_test()
{
		while(1)
		{
			TimeCnt = TIM4->CNT;
		}
}