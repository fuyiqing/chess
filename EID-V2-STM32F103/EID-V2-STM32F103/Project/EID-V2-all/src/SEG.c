/***********************************************************************
文件名称：LED.C
功    能：led  IO初始化
编写时间：2013.4.25
编 写 人：
注    意：
***********************************************************************/
#include "stm32f10x.h"
#include "stm32f10x_bkp.h"
#include "stm32f10x_pwr.h"
/***********************************************************************
函数名称：SEG_Configuration(void)
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
void SEG_Configuration(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	/* Enable the GPIO_LED Clock */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_AFIO , ENABLE); 						 
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_BKP|RCC_APB1Periph_PWR,ENABLE); 						 
	PWR_BackupAccessCmd( ENABLE );//允许修改RTC和后备寄存器
	RCC_LSEConfig( RCC_LSE_OFF );
	BKP_TamperPinCmd(DISABLE);//关闭入侵检测功能，PC13才可以使用，作为IO，且只能2M
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	PWR_BackupAccessCmd(DISABLE);
	
	GPIO_InitStructure.GPIO_Pin = 0xff;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);	
}

void SEG_test()
{		
		int i ;
		u8 Seg_Code[17]={ 0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x88,0x83,0xCC,0xA1,0x86,0x8e,0x7F};
		while(1)
		{
				GPIO_ResetBits(GPIOC, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_10 | GPIO_Pin_11);
				for( i=0 ; i < 17; i ++ )
				{
					GPIOD->ODR &= 0xff00;
					GPIOD->ODR |= Seg_Code[i];
					delay_ms(1000);
				}
		}
}
