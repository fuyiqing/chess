/***********************************************************************
�ļ����ƣ�LED.C
��    �ܣ�led  IO��ʼ��
��дʱ�䣺2013.4.25
�� д �ˣ�
ע    �⣺
***********************************************************************/
#include "stm32f10x.h"
#include <stm32f10x_usart.h>

void RS232_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
 	USART_InitTypeDef USART_InitStructure; 
	//NVIC_InitTypeDef NVIC_InitStructure1;
	//����ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	//����ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);


  /*
  *  USART1_TX -> PA9 , USART1_RX ->	PA10
  */				
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	         
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);		   

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	        
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
		//����1��ʼ��
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
//	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//�����ж�
	USART_Cmd(USART1, ENABLE);
	USART_ClearITPendingBit(USART1, USART_IT_TC);//����ж�TCλ
	
}

void RS485_Configuration()
{
	GPIO_InitTypeDef GPIO_InitStructure;
 	USART_InitTypeDef USART_InitStructure; 
	//NVIC_InitTypeDef NVIC_InitStructure1;
	//����ʱ��	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3 , ENABLE); 	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);
	//����3��ʼ��
	
  /*
  *  USART3_TX -> PB10 , USART3_RX ->	PB11, RS485_DIR -> PD13
  */				
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	         
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOB, &GPIO_InitStructure);		   

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;	        
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	        
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOD, &GPIO_InitStructure);	

	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART3, &USART_InitStructure);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
//	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);//�����ж�
	USART_Cmd(USART3, ENABLE);
	USART_ClearITPendingBit(USART3, USART_IT_TC);//����ж�TCλ	
	
}
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef   NVIC_InitStructure;

  /* Set the Vector Table base location at 0x08000000 */
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);

  /* 2 bit for pre-emption priority, 2 bits for subpriority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
  
  /* Enable the Ethernet global Interrupt */
  
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;	  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 
}
/***********************************************************************
�������ƣ�void USART1_IRQHandler(void) 
��    �ܣ����SCI�����ݵĽ��գ�������ʶ
���������
���������
��дʱ�䣺2012.11.22
�� д �ˣ�
ע    ��  RS485�õ���USART1.
***********************************************************************/


char RS232InData;

void USART1_IRQHandler(void)  
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{	
		RS232InData = USART1->DR;//
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
	if (USART_GetITStatus(USART1, USART_IT_TXE) != RESET) 
	{
        USART_ClearITPendingBit(USART1, USART_IT_TXE);           /* Clear the USART transmit interrupt                  */
  	}	
}

void RS232SendByte(char data)
{
		USART1->DR = data; 
}

char RS485InData;

void USART3_IRQHandler(void)  
{
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{	
		RS485InData = USART3->DR;//
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
	}
	if (USART_GetITStatus(USART3, USART_IT_TXE) != RESET) 
	{
        USART_ClearITPendingBit(USART3, USART_IT_TXE);           /* Clear the USART transmit interrupt                  */
  }	
}

void RS485SendByte(char data)
{
		USART3->DR = data; 
}

void SetRS485Dir(char dir)
{
		if (dir ==0 ) //receive
		{
				GPIO_ResetBits(GPIOD , GPIO_Pin_13);
		}
		else
		{
				GPIO_SetBits(GPIOD , GPIO_Pin_13);
		}
}





void RS232_test()
{
		if ( RS232InData != 0 )
		{
				RS232SendByte(RS232InData);
				RS232InData = 0 ;
		}

}

void RS485_test()
{
		SetRS485Dir(0);
		//RS485InData = 0x55;
		if ( RS485InData != 0 )
		{
				SetRS485Dir(1);
				Delay(0xffff);
				RS485SendByte(RS485InData);
				Delay(0xffff);
				SetRS485Dir(0);
				RS485InData = 0 ;
		}
		Delay(0xffff);
}



void uart_test()
{
		NVIC_Configuration();
		RS232_Configuration();
		RS485_Configuration();
		while(1)
		{
			RS232_test();    //send=receive,one byte
			RS485_test();
		}
}
