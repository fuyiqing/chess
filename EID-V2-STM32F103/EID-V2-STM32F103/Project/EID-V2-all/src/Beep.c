/***********************************************************************
文件名称：LED.C
功    能：led  IO初始化
编写时间：2013.4.25
编 写 人：
注    意：
***********************************************************************/
#include "stm32f10x.h"


/***********************************************************************
函数名称：LED_Configuration(void)
功    能：完成LED的配置
输入参数：
输出参数：
编写时间：2013.4.25
编 写 人：
注    意：LED1->PE2,
		  LED2->PE3,
		  LED3->PE4,
		  LED4->PE5,
***********************************************************************/
void BEEP_Configuration(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	/* Enable the GPIO_LED Clock */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO , ENABLE); 	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE); //Disable jtag	,Enable SWD
			
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/***********************************************************************
函数名称：LED_Blink(void)
功    能：完成LED闪烁
输入参数：
输出参数：
编写时间：2013.4.25
编 写 人：
注    意：
***********************************************************************/
void Beep(void)
{
	GPIO_SetBits(GPIOB , GPIO_Pin_3);
	Delay(0xffff);

	/*====LED-OFF=======*/ 
	GPIO_ResetBits(GPIOB , GPIO_Pin_3);
	Delay(0xfffff);
	Delay(0xfffff); 
}
