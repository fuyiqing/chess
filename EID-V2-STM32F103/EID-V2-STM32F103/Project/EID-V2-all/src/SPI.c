#include "stm32f10x.h"
#include "stm32f10x_spi.h"
/*******************************************************************************
//STM32F207VGT6 FOR ENC28J60
//SPI2初始化/IO初始化等
//SPI configuration
// JUST FOR STM32F2XX
*******************************************************************************/
void ENC25Q80_SPI2_Init(void)
{
    SPI_InitTypeDef  SPI_InitStructure;
    GPIO_InitTypeDef  GPIO_InitStructure;

     /*!< Enable the SPI2 clock */        
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
 
    /*!< Enable GPIO clocks */
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO , ENABLE); 	
		GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE); //Disable jtag	,Enable SWD
		GPIO_PinRemapConfig(GPIO_Remap_SPI1 , ENABLE); //Disable jtag	,Enable SWD
    /*!< SPI2 pins configuration */
    
   
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // also 100Mhz
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOB, &GPIO_InitStructure);// PB10/14/15-SCK,MISO,MOSI
      
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; // CS for enc28j60
    GPIO_Init(GPIOC, &GPIO_InitStructure);
  
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//SPI设置为双线双向全双工
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;//设置为主SPI
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;//设置SPI的数据大小:SPI发送接收8位帧结构
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;//选择了串行时钟的稳态:时钟悬空高
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;//数据捕获于第二个时钟沿
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;//(SPI_NSS_Soft)此时NSS引脚可以配置成普通GPIO去控制从设备
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;//Fclk/2
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; /* Initialize the SPI_FirstBit member */
    SPI_InitStructure.SPI_CRCPolynomial=7;
    SPI_Init(SPI2, &SPI_InitStructure);
    SPI_Cmd(SPI2, ENABLE);
 
}

/*******************************************************************************
* Function Name  : SPI_ReadWriteByte
* Description    : SPI读写一个字节（发送完成后返回本次通讯读取的数据）
* Input          : unsigned char TxData 
* Output         : None
* Return         : unsigned char RxData
*******************************************************************************/
unsigned char SPI2_ReadWriteByte(unsigned char TxData)
{
    unsigned char RxData = 0;
	
		GPIO_ResetBits(GPIOC,GPIO_Pin_3);
    /* Wait till Transmit buffer is empty */ 
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
    /* Send A data */        
    SPI_I2S_SendData(SPI2, TxData);
    // while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET);
    //等待数据接收
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
    //取数据
    RxData = SPI_I2S_ReceiveData(SPI2);
	
		GPIO_SetBits(GPIOC,GPIO_Pin_3);
	
    return (unsigned char)RxData;
}

#define W25X_DeviceID 0xAB
#define SPI_FLASH_CS_LOW() GPIO_ResetBits(GPIOC,GPIO_Pin_3)
#define SPI_FLASH_CS_HIGH() GPIO_SetBits(GPIOC,GPIO_Pin_3)

u8 SPI_FLASH_SendByte(u8 data)
{
	SPI_I2S_SendData(SPI2,data);
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
	return SPI_I2S_ReceiveData(SPI2);
}

u32 SPI_FLASH_ReadDeviceID(void) 
{ 
u32 Temp = 0; 
//使用的时候就拉低/ 
SPI_FLASH_CS_LOW(); 
/* Send “RDID ” instruction */ 
SPI_FLASH_SendByte(0xAB); 
SPI_FLASH_SendByte(0); 
SPI_FLASH_SendByte(0); 
SPI_FLASH_SendByte(0); 
/* Read a byte from the FLASH */ 
Temp = SPI_FLASH_SendByte(0); 
/* Deselect the FLASH: Chip Select high */ 
SPI_FLASH_CS_HIGH(); 
return Temp; 
} 