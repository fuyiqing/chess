/***********************************************************************
ÎÄ¼þÃû³Æ£ºLED.C
¹¦    ÄÜ£ºled  IO³õÊ¼»¯
±àÐ´Ê±¼ä£º2013.4.25
±à Ð´ ÈË£º
×¢    Òâ£º
***********************************************************************/
#include "stm32f10x.h"
#include "stm32f10x_i2c.h"


//³õÊ¼»¯IIC
void IIC_Configuration(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
  I2C_InitTypeDef I2C_InitStructure;
	//RCC->APB2ENR|=1<<4;//ÏÈÊ¹ÄÜÍâÉèIO PORTCÊ±ÖÓ 
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	
	RCC_APB1PeriphClockCmd(	RCC_APB1Periph_I2C1, ENABLE );	
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD ;   //af
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);


	I2C_DeInit(I2C1);
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0XA0;//?????        
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress= I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 10000;//100KHZ
	I2C_Init(I2C1, &I2C_InitStructure);
	I2C_Cmd(I2C1, ENABLE);                

}

uint8_t I2C_Master_BufferWrite(I2C_TypeDef * I2Cx, uint8_t* pBuffer, uint32_t NumByteToWrite, uint8_t SlaveAddress)
{
    if(NumByteToWrite==0)
        return 1;
    /* 1.??*/
    I2C_GenerateSTART(I2Cx, ENABLE);
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));

    /* 2.????·/? */
    I2C_Send7bitAddress(I2Cx, SlaveAddress, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    /* 3.????? */
    while(NumByteToWrite--)
    {
      I2C_SendData(I2Cx, *pBuffer);
      while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
      pBuffer++;
    }

    /* 4.?? */
    I2C_GenerateSTOP(I2Cx, ENABLE);
    while ((I2Cx->CR1&0x200) == 0x200);
    return 0;
}

uint8_t I2C_Master_BufferRead(I2C_TypeDef * I2Cx, uint8_t* pBuffer, uint32_t NumByteToRead, uint8_t SlaveAddress,uint8_t wordaddress)
{
    if(NumByteToRead==0)
        return 1;

    while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));  
    I2C_AcknowledgeConfig(I2Cx, ENABLE);
    /* 1.??*/
    I2C_GenerateSTART(I2Cx, ENABLE);
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));
    /* 2.????·/? */
    I2C_Send7bitAddress(I2Cx, SlaveAddress, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

		I2C_SendData(I2Cx, wordaddress);  //send word address
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));		
		
    /* 3.??*/
    I2C_GenerateSTART(I2Cx, ENABLE);
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));
    /* 4.????·/? */
    I2C_Send7bitAddress(I2Cx, SlaveAddress, I2C_Direction_Receiver);
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

    /* 5.????? */
    while (NumByteToRead)
    {
        if(NumByteToRead==1)
        {
            I2C_AcknowledgeConfig(I2Cx, DISABLE);
            I2C_GenerateSTOP(I2Cx, ENABLE);//6.??,???
        }

        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED));  /* EV7 */
        *pBuffer++ = I2C_ReceiveData(I2Cx);
        NumByteToRead--;
    }

    I2C_AcknowledgeConfig(I2Cx, ENABLE);
    return 0;
}   

//ÂÒÂë²Î¿¼one note ±Ê¼Ç

unsigned char IICInBuf[20]={0};
unsigned char IICWriteBuf[20]={0,1,2,3,4,5,6,7,8,9,0}; //first byte is word address, data is start from 1
int writeflag = 0 ;
void IIC_test()
{
		
		while(1) {
			if (writeflag == 1)
			{
					I2C_Master_BufferWrite(	I2C1,IICWriteBuf,10,0xA0);
					writeflag = 0;
			}
		//	delay_ms(100);
		I2C_Master_BufferRead(I2C1,IICInBuf,20,0xA0,0);
		//	delay_ms(100);
		}
}