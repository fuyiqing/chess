/***********************************************************************
文件名称：LED.C
功    能：led  IO初始化
编写时间：2013.4.25
编 写 人：
注    意：
***********************************************************************/
#include "stm32f10x.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dac.h"
void AI_Configuration()
{      
//ADC手动转换,在定时器里启动转换
		ADC_InitTypeDef ADC_InitStructure;

	  GPIO_InitTypeDef GPIO_InitStructure;
  /* ADC1 configuration ------------------------------------------------------*/
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_ADC1| RCC_APB2Periph_GPIOC, ENABLE); 		
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOC, &GPIO_InitStructure);	
	
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
//	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure);

// Enable ADC1 
  ADC_Cmd(ADC1, ENABLE);

  /* Enable ADC1 reset calibaration register */   
  ADC_ResetCalibration(ADC1);
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));

  /* Start ADC1 calibaration */
  ADC_StartCalibration(ADC1);
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);	
	
//	ADC_InitTypeDef adc;
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

//	adc.ADC_Resolution = ADC_Resolution_12b;
//	adc.ADC_ScanConvMode = DISABLE;
//	adc.ADC_ContinuousConvMode = DISABLE;
//	adc.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
//	adc.ADC_DataAlign = ADC_DataAlign_Right;
//	adc.ADC_NbrOfConversion = 1;
//	ADC_Init(ADC1, &adc);

//	ADC_RegularChannelConfig(ADC1, ADC_Channel_4 , 1, ADC_SampleTime_15Cycles );		
//	ADC_Cmd(ADC1, ENABLE);
//	ADC_SoftwareStartConv(ADC1);	
	
	
	
	
}


void DAC1_Configuration(void)
{

    GPIO_InitTypeDef GPIO_InitStructure;
    DAC_InitTypeDef DAC_InitType;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );      //??PORTA????
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;                 // ????
     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;          //????
     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
     GPIO_Init(GPIOA, &GPIO_InitStructure);
     
	
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE );      //??DAC????



    DAC_InitType.DAC_Trigger=DAC_Trigger_None;    //??????? TEN1=0
    DAC_InitType.DAC_WaveGeneration=DAC_WaveGeneration_None;//???????
    DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude=DAC_LFSRUnmask_Bit0;//???????
    DAC_InitType.DAC_OutputBuffer=DAC_OutputBuffer_Disable ;    //DAC1?????? BOFF1=1
    DAC_Init(DAC_Channel_1,&DAC_InitType);     //???DAC??1

    DAC_Cmd(DAC_Channel_1, ENABLE);  //??DAC1

    DAC_SetChannel1Data(DAC_Align_12b_R, 0);  //12??????????DAC?

}


//vol:0~3300,??0~3.3V
void AO_test(u16 dac)
{
    DAC_SetChannel1Data(DAC_Align_12b_R,dac);//12??????????DAC?
}


u16 AItmp ;
void AIAO_test()
{

	while(1)
	{
		AItmp = ADC1->DR ; 
		ADC_RegularChannelConfig(ADC1, ADC_Channel_11 , 1, ADC_SampleTime_28Cycles5 );		  			    
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);   // 转换一次

		AO_test(AItmp);
		delay_ms(1000);	
	}
}
