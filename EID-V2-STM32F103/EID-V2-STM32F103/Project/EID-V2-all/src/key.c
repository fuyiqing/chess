/***********************************************************************
文件名称：LED.C
功    能：led  IO初始化
编写时间：2013.4.25
编 写 人：
注    意：
***********************************************************************/
#include "stm32f10x.h"

#define ROW_PORT GPIOE
#define COL_PORT GPIOE

#define ROW_1 GPIO_Pin_4
#define ROW_2 GPIO_Pin_5
#define ROW_3 GPIO_Pin_6
#define ROW_4 GPIO_Pin_7

#define COL_1 GPIO_Pin_0
#define COL_2 GPIO_Pin_1
#define COL_3 GPIO_Pin_2
#define COL_4 GPIO_Pin_3

void KEY_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 |GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7; //行
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(ROW_PORT,&GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 |GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3; //列
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOE,&GPIO_InitStructure); 
	
}




int  KEY_read(void)
{
	int key_value = 0 ;
	u8 key_1 = 1;
	u8 key_2 = 1;
	u8 key_3 = 1;
	u8 key_4 = 1;
	
	GPIO_ResetBits(ROW_PORT,ROW_1);
	GPIO_SetBits(ROW_PORT,ROW_2 |ROW_3|ROW_4);
	key_1 = GPIO_ReadInputDataBit(COL_PORT,COL_1);
	key_2 = GPIO_ReadInputDataBit(COL_PORT,COL_2);
	key_3 = GPIO_ReadInputDataBit(COL_PORT,COL_3);
	key_4 = GPIO_ReadInputDataBit(COL_PORT,COL_4);
	if(key_1 == 0)
	{
		key_value =1;
	}
	else
	{
	}
	if(key_2 == 0)
	{
		key_value =2;
	}
	else
	{
	}
	if(key_3 == 0)
	{
		key_value =3;
	}
	else
	{
	}
	if(key_4 == 0)
	{
		key_value =4;
	}
	else
	{
	}	
	
	GPIO_ResetBits(ROW_PORT,ROW_2);
	GPIO_SetBits(ROW_PORT,ROW_1 |ROW_3|ROW_4);
	key_1 = GPIO_ReadInputDataBit(COL_PORT,COL_1);
	key_2 = GPIO_ReadInputDataBit(COL_PORT,COL_2);
	key_3 = GPIO_ReadInputDataBit(COL_PORT,COL_3);
	key_4 = GPIO_ReadInputDataBit(COL_PORT,COL_4);
	if(key_1 == 0)
	{
		key_value =5;
	}
	else
	{
	}
	if(key_2 == 0)
	{
		key_value =6;
	}
	else
	{
	}
	if(key_3 == 0)
	{
		key_value =7;
	}
	else
	{
	}
	if(key_4 == 0)
	{
		key_value =8;
	}
	else
	{
	}
	
	GPIO_ResetBits(ROW_PORT,ROW_3);
	GPIO_SetBits(ROW_PORT,ROW_1 |ROW_2|ROW_4);
	key_1 = GPIO_ReadInputDataBit(COL_PORT,COL_1);
	key_2 = GPIO_ReadInputDataBit(COL_PORT,COL_2);
	key_3 = GPIO_ReadInputDataBit(COL_PORT,COL_3);
	key_4 = GPIO_ReadInputDataBit(COL_PORT,COL_4);
	if(key_1 == 0)
	{
		key_value =9;
	}
	else
	{
	}
	if(key_2 == 0)
	{
		key_value =10;
	}
	else
	{
	}
	if(key_3== 0)
	{
		key_value =11;
	}
	else
	{
	}
	if(key_4 == 0)
	{
		key_value =12;
	}
	else
	{
	}
	
	GPIO_ResetBits(ROW_PORT,ROW_4);
	GPIO_SetBits(ROW_PORT,ROW_1 |ROW_2|ROW_3);
	key_1 = GPIO_ReadInputDataBit(COL_PORT,COL_1);
	key_2 = GPIO_ReadInputDataBit(COL_PORT,COL_2);
	key_3 = GPIO_ReadInputDataBit(COL_PORT,COL_3);
	key_4 = GPIO_ReadInputDataBit(COL_PORT,COL_4);
	if(key_1 == 0)
	{
		key_value =13;
	}
	else
	{
	}
	if(key_2 == 0)
	{
		key_value =14;
	}
	else
	{
	}
	if(key_3 == 0)
	{
		key_value =15;
	}
	else
	{
	}
	if(key_4 == 0)
	{
		key_value =16;
	}
	else
	{
	}
	return key_value;
}

int keydown;
void key_test()
{
	KEY_Configuration();
	while(1)
		keydown = KEY_read();
}