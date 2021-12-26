#include "lcd.h"
#include "stdlib.h"
#include "font.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_exti.h"
#include "delay.h"

//LCD�Ļ�����ɫ�ͱ���ɫ
u16 POINT_COLOR = WHITE; //������ɫ
u16 BACK_COLOR = BROWN;  //����ɫ
u16 CHESS_COLOR = BLACK;  //������ɫ

int chessTable[5][5] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
u8 chessFlag = 0;

_lcd_dev lcddev;

void LCD_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    /* Enable the GPIO_LED Clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7 | GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_SetBits(GPIOD, GPIO_Pin_12); //reset pin

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void LCD_Config_DIN(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = 0xC703; ////GPIO_Pin_14 | GPIO_Pin_15 |GPIO_Pin_1|GPIO_Pin_0|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10 ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOD, 0xC703);
    GPIO_InitStructure.GPIO_Pin = 0xff80; //PE7-15
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOE, 0xff80);
}

void LCD_Config_DOUT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = 0xC703; ////GPIO_Pin_14 | GPIO_Pin_15 |GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10 ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = 0xff80; //PE7-15
    GPIO_Init(GPIOE, &GPIO_InitStructure);
}

void DATAOUT(u16 x) //�������?
{
    LCD_Config_DOUT();

    GPIOD->BRR |= (0x03 << 14); // LCD_D0,D1; PD14,PD15
    GPIOD->BSRR |= (x & 0x0003) << 14;

    GPIOD->BRR |= (0x03 << 0); // LCD_D2,D3; PD0,PD1
    GPIOD->BSRR |= (x & 0x000c) >> 2;

    GPIOE->BRR |= (0x1ff << 7); // LCD_D4-12; PE7-15
    GPIOE->BSRR |= (x & 0x1ff0) << 3;

    GPIOD->BRR |= (0x07 << 8); // LCD_D13,D14,D15; PD8-10
    GPIOD->BSRR |= (x & 0xE000) >> 5;
}

u16 DATAIN() //��������
{
    u16 tmp = 0;
    LCD_Config_DIN();
    tmp = GPIOD->IDR >> 14;
    tmp |= ((GPIOD->IDR & 0x3) << 2);
    tmp |= ((GPIOE->IDR & 0xff80) >> 3);
    tmp |= ((GPIOD->IDR & 0x0700) << 5);
    return tmp;
}

//д�Ĵ�������
//data:�Ĵ���ֵ
void LCD_WR_REG(u16 data)
{
    LCD_RS_CLR; //д��ַ
    LCD_CS_CLR;
    DATAOUT(data);
    LCD_WR_CLR;
    LCD_WR_SET;
    LCD_CS_SET;
}

//д���ݺ���
//�������LCD_WR_DATAX��,��ʱ�任�ռ�.
//data:�Ĵ���ֵ
void LCD_WR_DATAX(u16 data)
{
    LCD_RS_SET;
    LCD_CS_CLR;
    DATAOUT(data);
    LCD_WR_CLR;
    LCD_WR_SET;
    LCD_CS_SET;
}

//��LCD����
//����ֵ:������ֵ
u16 LCD_RD_DATA(void)
{
    u16 t;

    DATAIN();

    LCD_RS_SET;
    LCD_CS_CLR;
    //��ȡ����(���Ĵ���ʱ,������Ҫ��2��)
    LCD_RD_CLR;
    t = DATAIN();
    LCD_RD_SET;
    LCD_CS_SET;

    DATAOUT(0xFFFF);
    return t;
}

//д�Ĵ���
//LCD_Reg:�Ĵ������?
//LCD_RegValue:Ҫд����?
void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue)
{
    LCD_WR_REG(LCD_Reg);
    LCD_WR_DATA(LCD_RegValue);
}

//���Ĵ���
//LCD_Reg:�Ĵ������?
//����ֵ:������ֵ
u16 LCD_ReadReg(u16 LCD_Reg)
{
    LCD_WR_REG(LCD_Reg); //д��Ҫ���ļĴ�����
    return LCD_RD_DATA();
}

//��ʼдGRAM
void LCD_WriteRAM_Prepare(void)
{
    LCD_WR_REG(lcddev.wramcmd);
}

//LCDдGRAM
//RGB_Code:��ɫֵ
void LCD_WriteRAM(u16 RGB_Code)
{
    LCD_WR_DATA(RGB_Code); //дʮ��λGRAM
}

//��ILI93xx����������ΪGBR��ʽ��������д���ʱ��ΪRGB��ʽ��
//ͨ���ú���ת��
//c:GBR��ʽ����ɫֵ
//����ֵ��RGB��ʽ����ɫֵ
u16 LCD_BGR2RGB(u16 c)
{
    u16 r, g, b, rgb;
    b = (c >> 0) & 0x1f;
    g = (c >> 5) & 0x3f;
    r = (c >> 11) & 0x1f;
    rgb = (b << 11) + (g << 5) + (r << 0);
    return (rgb);
}

//��mdk -O1ʱ���Ż�ʱ��Ҫ����
//��ʱi
void opt_delay(u8 i)
{
    while (i--)
        ;
}

//��ȡ��ĳ������?ֵ
//x,y:����
//����ֵ:�˵�����?
u16 LCD_ReadPoint(u16 x, u16 y)
{
    u16 r, g, b;
    if (x >= lcddev.width || y >= lcddev.height)
        return 0; //�����˷�Χ,ֱ�ӷ���
    LCD_SetCursor(x, y);
    LCD_WR_REG(0X2E); //9341 ���Ͷ�GRAMָ��
    DATAIN();

    LCD_RS_SET;
    LCD_CS_CLR;
    //��ȡ����(��GRAMʱ,��һ��Ϊ�ٶ�)
    LCD_RD_CLR;
    opt_delay(2); //��ʱ
    r = DATAIN(); //ʵ��������ɫ
    LCD_RD_SET;

    //dummy READ
    LCD_RD_CLR;
    opt_delay(2); //��ʱ
    r = DATAIN(); //ʵ��������ɫ
    LCD_RD_SET;

    LCD_RD_CLR;
    opt_delay(2); //��ʱ
    b = DATAIN(); //��ȡ��ɫֵ
    LCD_RD_SET;
    g = r & 0XFF; //����9341,��һ�ζ�ȡ����RG��ֵ,R��ǰ,G�ں�,��ռ8λ
    g <<= 8;
    LCD_CS_SET;
    DATAOUT(0xFFFF);                                           //ȫ�������?
    return (((r >> 11) << 11) | ((g >> 10) << 5) | (b >> 11)); //ILI9341/NT35310/NT35510��Ҫ��ʽת��һ��
}

//LCD������ʾ
void LCD_DisplayOn(void)
{
    LCD_WR_REG(0X29); //������ʾ
}

//LCD�ر���ʾ
void LCD_DisplayOff(void)
{
    LCD_WR_REG(0X28); //�ر���ʾ
}

//���ù��λ��?
//Xpos:������
//Ypos:������
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{
    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA(Xpos >> 8);
    LCD_WR_DATA(Xpos & 0XFF);
    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(Ypos >> 8);
    LCD_WR_DATA(Ypos & 0XFF);
}

//����LCD���Զ�ɨ�跽��
//ע��:�����������ܻ��ܵ��˺������õ�Ӱ��(������9341/6804����������),
//����,һ������ΪL2R_U2D����,��������?����ɨ�跽ʽ,���ܵ�����ʾ������.
//dir:0~7,����8������(���嶨���lcd.h)
//9320/9325/9328/4531/4535/1505/b505/5408/9341/5310/5510/1963��IC�Ѿ�ʵ�ʲ���
void LCD_Scan_Dir(u8 dir)
{
    u16 regval = 0;
    u16 dirreg = 0;
    u16 temp;
    if (lcddev.dir == 1)
    {
        switch (dir) //����ת��
        {
        case 0:
            dir = 6;
            break;
        case 1:
            dir = 7;
            break;
        case 2:
            dir = 4;
            break;
        case 3:
            dir = 5;
            break;
        case 4:
            dir = 1;
            break;
        case 5:
            dir = 0;
            break;
        case 6:
            dir = 3;
            break;
        case 7:
            dir = 2;
            break;
        }
    }

    switch (dir)
    {
    case L2R_U2D: //������,���ϵ���
        regval |= (0 << 7) | (0 << 6) | (0 << 5);
        break;
    case L2R_D2U: //������,���µ���
        regval |= (1 << 7) | (0 << 6) | (0 << 5);
        break;
    case R2L_U2D: //���ҵ���,���ϵ���
        regval |= (0 << 7) | (1 << 6) | (0 << 5);
        break;
    case R2L_D2U: //���ҵ���,���µ���
        regval |= (1 << 7) | (1 << 6) | (0 << 5);
        break;
    case U2D_L2R: //���ϵ���,������
        regval |= (0 << 7) | (0 << 6) | (1 << 5);
        break;
    case U2D_R2L: //���ϵ���,���ҵ���
        regval |= (0 << 7) | (1 << 6) | (1 << 5);
        break;
    case D2U_L2R: //���µ���,������
        regval |= (1 << 7) | (0 << 6) | (1 << 5);
        break;
    case D2U_R2L: //���µ���,���ҵ���
        regval |= (1 << 7) | (1 << 6) | (1 << 5);
        break;
    }
    dirreg = 0X36;
    regval |= 0X08; //BGR
    LCD_WriteReg(dirreg, regval);

    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA(0);
    LCD_WR_DATA(0);
    LCD_WR_DATA((lcddev.width - 1) >> 8);
    LCD_WR_DATA((lcddev.width - 1) & 0XFF);
    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(0);
    LCD_WR_DATA(0);
    LCD_WR_DATA((lcddev.height - 1) >> 8);
    LCD_WR_DATA((lcddev.height - 1) & 0XFF);
}

//����
//x,y:����
//POINT_COLOR:�˵�����?
void LCD_DrawPoint(u16 x, u16 y)
{
    LCD_SetCursor(x, y);    //���ù��λ��?
    LCD_WriteRAM_Prepare(); //��ʼд��GRAM
    LCD_WR_DATA(POINT_COLOR);
}

//���ٻ���
//x,y:����
//color:��ɫ
void LCD_Fast_DrawPoint(u16 x, u16 y, u16 color)
{
    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA(x >> 8);
    LCD_WR_DATA(x & 0XFF);
    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(y >> 8);
    LCD_WR_DATA(y & 0XFF);

    LCD_RS_CLR;
    LCD_CS_CLR;
    DATAOUT(lcddev.wramcmd); //дָ��
    LCD_WR_CLR;
    LCD_WR_SET;
    LCD_CS_SET;
    LCD_WR_DATA(color); //д����
}

//SSD1963 ��������
//pwm:����ȼ�?,0~100.Խ��Խ��.
void LCD_SSD_BackLightSet(u8 pwm)
{
    LCD_WR_REG(0xBE);        //����PWM���?
    LCD_WR_DATA(0x05);       //1����PWMƵ��
    LCD_WR_DATA(pwm * 2.55); //2����PWMռ�ձ�
    LCD_WR_DATA(0x01);       //3����C
    LCD_WR_DATA(0xFF);       //4����D
    LCD_WR_DATA(0x00);       //5����E
    LCD_WR_DATA(0x00);       //6����F
}

//����LCD��ʾ����
//dir:0,������1,����
void LCD_Display_Dir(u8 dir)
{
    if (dir == 0) //����
    {
        lcddev.dir = 0; //����
        lcddev.width = 240;
        lcddev.height = 320;

        lcddev.wramcmd = 0X2C;
        lcddev.setxcmd = 0X2A;
        lcddev.setycmd = 0X2B;
        if (lcddev.id == 0X6804 || lcddev.id == 0X5310)
        {
            lcddev.width = 320;
            lcddev.height = 480;
        }
    }
    else //����
    {
        lcddev.dir = 1; //����
        lcddev.width = 320;
        lcddev.height = 240;

        lcddev.wramcmd = 0X2C;
        lcddev.setxcmd = 0X2A;
        lcddev.setycmd = 0X2B;
    }
    LCD_Scan_Dir(DFT_SCAN_DIR); //Ĭ��ɨ�跽��
}

//���ô���,���Զ����û������굽�������Ͻ�(sx,sy).
//sx,sy:������ʼ����(���Ͻ�)
//width,height:���ڿ��Ⱥ͸߶�,�������?0!!
//������?:width*height.
void LCD_Set_Window(u16 sx, u16 sy, u16 width, u16 height)
{
    u8 hsareg, heareg, vsareg, veareg;
    u16 hsaval, heaval, vsaval, veaval;
    u16 twidth, theight;
    twidth = sx + width - 1;
    theight = sy + height - 1;

    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA(sx >> 8);
    LCD_WR_DATA(sx & 0XFF);
    LCD_WR_DATA(twidth >> 8);
    LCD_WR_DATA(twidth & 0XFF);
    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(sy >> 8);
    LCD_WR_DATA(sy & 0XFF);
    LCD_WR_DATA(theight >> 8);
    LCD_WR_DATA(theight & 0XFF);
}

//��ʼ��lcd
//�ó�ʼ���������Գ�ʼ������ALIENTEK��Ʒ��LCDҺ����
//������ռ�ýϴ�flash,�û����Ը����Լ���ʵ�����?,ɾ��δ�õ���LCD��ʼ������.�Խ�ʡ�ռ�.
void LCD_Init(void)
{
    delay_ms(50); // delay 50 ms
    LCD_WriteReg(0x0000, 0x0001);
    delay_ms(50); // delay 50 ms
    lcddev.id = LCD_ReadReg(0x0000);
    if (lcddev.id < 0XFF || lcddev.id == 0XFFFF || lcddev.id == 0X9300) //����ID����ȷ,����lcddev.id==0X9300�жϣ���Ϊ9341��δ����λ������»ᱻ����?9300
    {
        //����9341 ID�Ķ�ȡ
        LCD_WR_REG(0XD3);
        LCD_RD_DATA();             //dummy read
        LCD_RD_DATA();             //����0X00
        lcddev.id = LCD_RD_DATA(); //��ȡ93
        lcddev.id <<= 8;
        lcddev.id |= LCD_RD_DATA(); //��ȡ41
        if (lcddev.id != 0X9341)    //��9341,�����ǲ���6804
        {
            LCD_WR_REG(0XBF);
            LCD_RD_DATA();             //dummy read
            LCD_RD_DATA();             //����0X01
            LCD_RD_DATA();             //����0XD0
            lcddev.id = LCD_RD_DATA(); //�������?0X68
            lcddev.id <<= 8;
            lcddev.id |= LCD_RD_DATA(); //�������?0X04
            if (lcddev.id != 0X6804)    //Ҳ����6804,���Կ����ǲ���NT35310
            {
                LCD_WR_REG(0XD4);
                LCD_RD_DATA();             //dummy read
                LCD_RD_DATA();             //����0X01
                lcddev.id = LCD_RD_DATA(); //����0X53
                lcddev.id <<= 8;
                lcddev.id |= LCD_RD_DATA(); //�������?0X10
                if (lcddev.id != 0X5310)    //Ҳ����NT35310,���Կ����ǲ���NT35510
                {
                    LCD_WR_REG(0XDA00);
                    LCD_RD_DATA(); //����0X00
                    LCD_WR_REG(0XDB00);
                    lcddev.id = LCD_RD_DATA(); //����0X80
                    lcddev.id <<= 8;
                    LCD_WR_REG(0XDC00);
                    lcddev.id |= LCD_RD_DATA(); //����0X00
                    if (lcddev.id == 0x8000)
                        lcddev.id = 0x5510;  //NT35510���ص�ID��8000H,Ϊ��������,����ǿ������Ϊ5510
                    if (lcddev.id != 0X5510) //Ҳ����NT5510,���Կ����ǲ���SSD1963
                    {
                        LCD_WR_REG(0XA1);
                        lcddev.id = LCD_RD_DATA();
                        lcddev.id = LCD_RD_DATA(); //����0X57
                        lcddev.id <<= 8;
                        lcddev.id |= LCD_RD_DATA(); //����0X61
                        if (lcddev.id == 0X5761)
                            lcddev.id = 0X1963; //SSD1963���ص�ID��5761H,Ϊ��������,����ǿ������Ϊ1963
                    }
                }
            }
        }
    }

    if (lcddev.id == 0X9341) //9341��ʼ��
    {
        LCD_WR_REG(0xCF);
        LCD_WR_DATAX(0x00);
        LCD_WR_DATAX(0xC1);
        LCD_WR_DATAX(0X30);
        LCD_WR_REG(0xED);
        LCD_WR_DATAX(0x64);
        LCD_WR_DATAX(0x03);
        LCD_WR_DATAX(0X12);
        LCD_WR_DATAX(0X81);
        LCD_WR_REG(0xE8);
        LCD_WR_DATAX(0x85);
        LCD_WR_DATAX(0x10);
        LCD_WR_DATAX(0x7A);
        LCD_WR_REG(0xCB);
        LCD_WR_DATAX(0x39);
        LCD_WR_DATAX(0x2C);
        LCD_WR_DATAX(0x00);
        LCD_WR_DATAX(0x34);
        LCD_WR_DATAX(0x02);
        LCD_WR_REG(0xF7);
        LCD_WR_DATAX(0x20);
        LCD_WR_REG(0xEA);
        LCD_WR_DATAX(0x00);
        LCD_WR_DATAX(0x00);
        LCD_WR_REG(0xC0);   //Power control
        LCD_WR_DATAX(0x1B); //VRH[5:0]
        LCD_WR_REG(0xC1);   //Power control
        LCD_WR_DATAX(0x01); //SAP[2:0];BT[3:0]
        LCD_WR_REG(0xC5);   //VCM control
        LCD_WR_DATAX(0x30); //3F
        LCD_WR_DATAX(0x30); //3C
        LCD_WR_REG(0xC7);   //VCM control2
        LCD_WR_DATAX(0XB7);
        LCD_WR_REG(0x36); // Memory Access Control
        LCD_WR_DATAX(0x48);
        LCD_WR_REG(0x3A);
        LCD_WR_DATAX(0x55);
        LCD_WR_REG(0xB1);
        LCD_WR_DATAX(0x00);
        LCD_WR_DATAX(0x1A);
        LCD_WR_REG(0xB6); // Display Function Control
        LCD_WR_DATAX(0x0A);
        LCD_WR_DATAX(0xA2);
        LCD_WR_REG(0xF2); // 3Gamma Function Disable
        LCD_WR_DATAX(0x00);
        LCD_WR_REG(0x26); //Gamma curve selected
        LCD_WR_DATAX(0x01);
        LCD_WR_REG(0xE0); //Set Gamma
        LCD_WR_DATAX(0x0F);
        LCD_WR_DATAX(0x2A);
        LCD_WR_DATAX(0x28);
        LCD_WR_DATAX(0x08);
        LCD_WR_DATAX(0x0E);
        LCD_WR_DATAX(0x08);
        LCD_WR_DATAX(0x54);
        LCD_WR_DATAX(0XA9);
        LCD_WR_DATAX(0x43);
        LCD_WR_DATAX(0x0A);
        LCD_WR_DATAX(0x0F);
        LCD_WR_DATAX(0x00);
        LCD_WR_DATAX(0x00);
        LCD_WR_DATAX(0x00);
        LCD_WR_DATAX(0x00);
        LCD_WR_REG(0XE1); //Set Gamma
        LCD_WR_DATAX(0x00);
        LCD_WR_DATAX(0x15);
        LCD_WR_DATAX(0x17);
        LCD_WR_DATAX(0x07);
        LCD_WR_DATAX(0x11);
        LCD_WR_DATAX(0x06);
        LCD_WR_DATAX(0x2B);
        LCD_WR_DATAX(0x56);
        LCD_WR_DATAX(0x3C);
        LCD_WR_DATAX(0x05);
        LCD_WR_DATAX(0x10);
        LCD_WR_DATAX(0x0F);
        LCD_WR_DATAX(0x3F);
        LCD_WR_DATAX(0x3F);
        LCD_WR_DATAX(0x0F);
        LCD_WR_REG(0x2B);
        LCD_WR_DATAX(0x00);
        LCD_WR_DATAX(0x00);
        LCD_WR_DATAX(0x01);
        LCD_WR_DATAX(0x3f);
        LCD_WR_REG(0x2A);
        LCD_WR_DATAX(0x00);
        LCD_WR_DATAX(0x00);
        LCD_WR_DATAX(0x00);
        LCD_WR_DATAX(0xef);
        LCD_WR_REG(0x11); //Exit Sleep
        delay_ms(120);
        LCD_WR_REG(0x29); //display on
    }
    LCD_Display_Dir(0); //Ĭ��Ϊ����
    LCD_LED = 1;        //��������
    LCD_Clear(BROWN);
}

//��������
//color:Ҫ����������?
void LCD_Clear(u16 color)
{
    u32 index = 0;
    u32 totalpoint = lcddev.width;
    totalpoint *= lcddev.height; //�õ��ܵ���
    LCD_SetCursor(0x00, 0x0000); //���ù��λ��?
    LCD_WriteRAM_Prepare();      //��ʼд��GRAM
    for (index = 0; index < totalpoint; index++)
        LCD_WR_DATA(color);
}

//��ָ�����������ָ�����?
//������?:(xend-xsta+1)*(yend-ysta+1)
//xsta
//color:Ҫ������ɫ
void LCD_Fill(u16 sx, u16 sy, u16 ex, u16 ey, u16 color)
{
    u16 i, j;
    u16 xlen = 0;
    u16 temp;
    xlen = ex - sx + 1;
    for (i = sy; i <= ey; i++)
    {
        LCD_SetCursor(sx, i);   //���ù��λ��?
        LCD_WriteRAM_Prepare(); //��ʼд��GRAM
        for (j = 0; j < xlen; j++)
            LCD_WR_DATA(color); //���ù��λ��?
    }
}

//��ָ�����������ָ�����?��
//(sx,sy),(ex,ey):�����ζԽ�����,�����С�?:(ex-sx+1)*(ey-sy+1)
//color:Ҫ������ɫ
void LCD_Color_Fill(u16 sx, u16 sy, u16 ex, u16 ey, u16 *color)
{
    u16 height, width;
    u16 i, j;
    width = ex - sx + 1;  //�õ����Ŀ���
    height = ey - sy + 1; //�߶�
    for (i = 0; i < height; i++)
    {
        LCD_SetCursor(sx, sy + i); //���ù��λ��?
        LCD_WriteRAM_Prepare();    //��ʼд��GRAM
        for (j = 0; j < width; j++)
            LCD_WR_DATA(color[i * width + j]); //д������
    }
}

//����
//x1,y1:�������?
//x2,y2:�յ�����
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
    u16 t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    delta_x = x2 - x1; //������������
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;
    if (delta_x > 0)
        incx = 1; //���õ�������
    else if (delta_x == 0)
        incx = 0; //��ֱ��
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }
    if (delta_y > 0)
        incy = 1;
    else if (delta_y == 0)
        incy = 0; //ˮƽ��
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }
    if (delta_x > delta_y)
        distance = delta_x; //ѡȡ��������������
    else
        distance = delta_y;
    for (t = 0; t <= distance + 1; t++) //�������?
    {
        LCD_DrawPoint(uRow, uCol); //����
        xerr += delta_x;
        yerr += delta_y;
        if (xerr > distance)
        {
            xerr -= distance;
            uRow += incx;
        }
        if (yerr > distance)
        {
            yerr -= distance;
            uCol += incy;
        }
    }
}

//������
//(x1,y1),(x2,y2):���εĶԽ�����
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
    LCD_DrawLine(x1, y1, x2, y1);
    LCD_DrawLine(x1, y1, x1, y2);
    LCD_DrawLine(x1, y2, x2, y2);
    LCD_DrawLine(x2, y1, x2, y2);
}
//��ָ��λ�û�һ��ָ����С��Բ
//(x,y):���ĵ�
//r    :�뾶
void LCD_Draw_Circle(u16 x0, u16 y0, u8 r)
{
    int a, b;
    int di;
    a = 0;
    b = r;
    di = 3 - (r << 1); //�ж��¸���λ�õı�־
    while (a <= b)
    {
        LCD_DrawPoint(x0 + a, y0 - b); //5
        LCD_DrawPoint(x0 + b, y0 - a); //0
        LCD_DrawPoint(x0 + b, y0 + a); //4
        LCD_DrawPoint(x0 + a, y0 + b); //6
        LCD_DrawPoint(x0 - a, y0 + b); //1
        LCD_DrawPoint(x0 - b, y0 + a);
        LCD_DrawPoint(x0 - a, y0 - b); //2
        LCD_DrawPoint(x0 - b, y0 - a); //7
        a++;
        //ʹ��Bresenham�㷨��Բ
        if (di < 0)
            di += 4 * a + 6;
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
    }
}
//��ָ��λ����ʾһ���ַ�
//x,y:��ʼ����
//num:Ҫ��ʾ���ַ�:" "--->"~"
//size:������? 12/16/24
//mode:���ӷ�ʽ(1)���Ƿǵ��ӷ�ʽ(0)
void LCD_ShowChar(u16 x, u16 y, u8 num, u8 size, u8 mode)
{
    u8 temp, t1, t;
    u16 y0 = y;
    u8 csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2); //�õ�����һ���ַ���Ӧ������ռ���ֽ���
    num = num - ' ';                                           //�õ�ƫ�ƺ��ֵ��ASCII�ֿ��Ǵӿո�ʼȡģ������-' '���Ƕ�Ӧ�ַ����ֿ⣩
    for (t = 0; t < csize; t++)
    {
        if (size == 12)
            temp = asc2_1206[num][t]; //����1206����
        else if (size == 16)
            temp = asc2_1608[num][t]; //����1608����
        else if (size == 24)
            temp = asc2_2412[num][t]; //����2412����
        else
            return; //û�е��ֿ�
        for (t1 = 0; t1 < 8; t1++)
        {
            if (temp & 0x80)
                LCD_Fast_DrawPoint(x, y, POINT_COLOR);
            else if (mode == 0)
                LCD_Fast_DrawPoint(x, y, BACK_COLOR);
            temp <<= 1;
            y++;
            if (y >= lcddev.height)
                return; //��������
            if ((y - y0) == size)
            {
                y = y0;
                x++;
                if (x >= lcddev.width)
                    return; //��������
                break;
            }
        }
    }
}
//m^n����
//����ֵ:m^n�η�.
u32 LCD_Pow(u8 m, u8 n)
{
    u32 result = 1;
    while (n--)
        result *= m;
    return result;
}
//��ʾ����,��λΪ0,����ʾ
//x,y :�������?
//len :���ֵ�λ��
//size:������?
//color:��ɫ
//num:��ֵ(0~4294967295);
void LCD_ShowNum(u16 x, u16 y, u32 num, u8 len, u8 size)
{
    u8 t, temp;
    u8 enshow = 0;
    for (t = 0; t < len; t++)
    {
        temp = (num / LCD_Pow(10, len - t - 1)) % 10;
        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                LCD_ShowChar(x + (size / 2) * t, y, ' ', size, 0);
                continue;
            }
            else
                enshow = 1;
        }
        LCD_ShowChar(x + (size / 2) * t, y, temp + '0', size, 0);
    }
}
//��ʾ����,��λΪ0,������ʾ
//x,y:�������?
//num:��ֵ(0~999999999);
//len:����(��Ҫ��ʾ��λ��)
//size:������?
//mode:
//[7]:0,�����?;1,���?0.
//[6:1]:����
//[0]:0,�ǵ�����ʾ;1,������ʾ.
void LCD_ShowxNum(u16 x, u16 y, u32 num, u8 len, u8 size, u8 mode)
{
    u8 t, temp;
    u8 enshow = 0;
    for (t = 0; t < len; t++)
    {
        temp = (num / LCD_Pow(10, len - t - 1)) % 10;
        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                if (mode & 0X80)
                    LCD_ShowChar(x + (size / 2) * t, y, '0', size, mode & 0X01);
                else
                    LCD_ShowChar(x + (size / 2) * t, y, ' ', size, mode & 0X01);
                continue;
            }
            else
                enshow = 1;
        }
        LCD_ShowChar(x + (size / 2) * t, y, temp + '0', size, mode & 0X01);
    }
}
//��ʾ�ַ���
//x,y:�������?
//width,height:������?
//size:������?
//*p:�ַ�����ʼ��ַ
void LCD_ShowString(u16 x, u16 y, u16 width, u16 height, u8 size, u8 *p)
{
    u8 x0 = x;
    width += x;
    height += y;
    while ((*p <= '~') && (*p >= ' ')) //�ж��ǲ��ǷǷ��ַ�!
    {
        if (x >= width)
        {
            x = x0;
            y += size;
        }
        if (y >= height)
            break; //�˳�
        LCD_ShowChar(x, y, *p, size, 0);
        x += size / 2;
        p++;
    }
}

//*@ param x ������
//*@ param y ������
//*@ note ��һ�����ӵ�
void LCD_DrawChessPoint(u16 x, u16 y)
{
    LCD_SetCursor(x, y);    //??????
    LCD_WriteRAM_Prepare(); //????GRAM
    LCD_WR_DATA(CHESS_COLOR);
}

//*@ param x0 ������
//*@ param y0 ������
//*@ param r  �뾶
//*@ note  ���׵�
void LCD_RmChessPoint(u16 x, u16 y)
{
    LCD_SetCursor(x, y);    //??????
    LCD_WriteRAM_Prepare(); //????GRAM
    LCD_WR_DATA(BACK_COLOR);
}

//*@ param x0 ������
//*@ param y0 ������
//*@ param r  �뾶
//*@ note  ��һ������
void LCD_Draw_Circle_Chess(u16 x0, u16 y0, u8 r)
{
    int a, b;
    int di;
    int i;
    a = 0;
    b = r;
    di = 3 - (r << 1); //??????????
    for (i = 0; i <= r; i++)
    {
        a = 0;
        b = i;
        while (a <= b)
        {
            LCD_DrawChessPoint(x0 + a, y0 - b); //5
            LCD_DrawChessPoint(x0 + b, y0 - a); //0
            LCD_DrawChessPoint(x0 + b, y0 + a); //4
            LCD_DrawChessPoint(x0 + a, y0 + b); //6
            LCD_DrawChessPoint(x0 - a, y0 + b); //1
            LCD_DrawChessPoint(x0 - b, y0 + a);
            LCD_DrawChessPoint(x0 - a, y0 - b); //2
            LCD_DrawChessPoint(x0 - b, y0 - a); //7
            a++;
            //??Bresenham????
            if (di < 0)
                di += 4 * a + 6;
            else
            {
                di += 10 + 4 * (a - b);
                b--;
            }
        }
    }
    chessTable[x0][y0] = 1;
}

//*@ param x0 ������
//*@ param y0 ������
//*@ param r  �뾶
//*@ note  ��һ������
void LCD_Rm_Circle_Chess(u16 x0, u16 y0, u8 r)
{
    int a, b;
    int di;
    int i;
    a = 0;
    b = r;
    di = 3 - (r << 1); //??????????
    for (i = 0; i <= r; i++)
    {
        a = 0;
        b = i;
        while (a <= b)
        {
            LCD_RmChessPoint(x0 + a, y0 - b); //5
            LCD_RmChessPoint(x0 + b, y0 - a); //0
            LCD_RmChessPoint(x0 + b, y0 + a); //4
            LCD_RmChessPoint(x0 + a, y0 + b); //6
            LCD_RmChessPoint(x0 - a, y0 + b); //1
            LCD_RmChessPoint(x0 - b, y0 + a);
            LCD_RmChessPoint(x0 - a, y0 - b); //2
            LCD_RmChessPoint(x0 - b, y0 - a); //7
            a++;
            //??Bresenham????
            if (di < 0)
                di += 4 * a + 6;
            else
            {
                di += 10 + 4 * (a - b);
                b--;
            }
        }
    }

    LCD_Fast_DrawPoint(x0, y0, WHITE);
    LCD_Fast_DrawPoint(x0, y0, WHITE);
    LCD_Fast_DrawPoint(x0, y0, WHITE);
    LCD_Fast_DrawPoint(x0, y0, WHITE);
    if(x0 != 40) My_DrawLine(x0 - 15, y0, x0, y0, WHITE);
    if(x0 != 200) My_DrawLine(x0, y0, x0 + 15, y0, WHITE);
    if(y0 != 40) My_DrawLine(x0, y0 - 15, x0, y0, WHITE);
    if(y0 != 200) My_DrawLine(x0, y0, x0, y0 + 15, WHITE);
    //chessTable[x0][y0] = 0;
}

void My_DrawLine(int x0, int y0, int x1, int y1, u16 color)
{
    if(x0 == x1)
    {
        for(int i = y0; i <= y1; i++)
        {
            LCD_Fast_DrawPoint(x0, i, color);
        }
    }
    else
    {
        for(int i = x0; i <= x1; i++)
        {
            LCD_Fast_DrawPoint(i, y0, color);
        }
    }
}

void LCD_test()
{
    u8 x = 0;
    u8 lcd_id[12]; //���LCD ID�ַ���
    //��ʼ����LED���ӵ�Ӳ���ӿ�
    LCD_Init();
    POINT_COLOR = RED;
    while (1)
    {
        switch (x)
        {
        case 0:
            LCD_Clear(WHITE);
            break;
        case 1:
            LCD_Clear(BLACK);
            break;
        case 2:
            LCD_Clear(BLUE);
            break;
        case 3:
            LCD_Clear(RED);
            break;
        case 4:
            LCD_Clear(MAGENTA);
            break;
        case 5:
            LCD_Clear(GREEN);
            break;
        case 6:
            LCD_Clear(CYAN);
            break;

        case 7:
            LCD_Clear(YELLOW);
            break;
        case 8:
            LCD_Clear(BRRED);
            break;
        case 9:
            LCD_Clear(GRAY);
            break;
        case 10:
            LCD_Clear(LGRAY);
            break;
        case 11:
            LCD_Clear(BROWN);
            break;
        }
        POINT_COLOR = RED;
        LCD_ShowString(30, 40, 200, 24, 24, "Mini STM32 ^_^");
        LCD_ShowString(30, 70, 200, 16, 16, "TFTLCD TEST");
        LCD_ShowString(30, 90, 200, 16, 16, "ATOM@ALIENTEK");
        LCD_ShowString(30, 110, 200, 16, 16, lcd_id); //��ʾLCD ID
        LCD_ShowString(30, 130, 200, 12, 12, "2014/3/7");
        x++;
        if (x == 12)
            x = 0;

        delay_ms(1000);
    }
}

void TOUCH_SCREEN_INIT()
{

    GPIO_InitTypeDef GPIO_InitStructure;
    //SPI_InitTypeDef  SPI_InitStructure;
    /* Enable GPIOB, GPIOC and AFIO clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); //Disable jtag	,Enable SWD

    /* SPI pins configuration */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_Init(GPIOB, &GPIO_InitStructure); // MOSI

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_Init(GPIOA, &GPIO_InitStructure); // MISO
}

#define TOUCH_READ_TIMES 5
u16 x_addata[TOUCH_READ_TIMES], z_addata[TOUCH_READ_TIMES], y_addata[TOUCH_READ_TIMES];
u32 Temp, r, sx, sy = 0;
u32 xScreen, yScreen;
#define QPINTERVAL 48
#define QZSIZE 20
#define QPSIZEX 240
#define QPSIZEY 320
#define QPSTARTX 0
#define QPSTARTY 0
#define PROMPTPOSX QPSIZEX / 3
#define PROMPTPOSY ((QPSIZEY - QPSIZEY / QPINTERVAL * QPINTERVAL) / 2 + QPSIZEY / QPINTERVAL * QPINTERVAL)
#define PROMPTSIZE ((QPSIZEY - QPSIZEY / QPINTERVAL * QPINTERVAL) * 2 / 5)

#define T_CS_SET GPIOA->BSRR = 1 << 15 //Ƭѡ�˿�  		PA15
#define T_WR_SET GPIOB->BSRR = 1 << 5  //д����MOSI			PB5
#define T_SCK_SET GPIOA->BSRR = 1 << 5 //SCK			PA5

#define T_CS_CLR GPIOA->BRR = 1 << 15 //Ƭѡ�˿�  		PA15
#define T_WR_CLR GPIOB->BRR = 1 << 5  //д����	MOSI		PB5
#define T_SCK_CLR GPIOA->BRR = 1 << 5 //SCK			PA5

#define T_IN_STATUE ((GPIOA->IDR & 0X40) ? 1 : 0) //����MISO PA6

u8 SPI_SndRecv(u8 data)
{
    u8 tmp = 0, i;

    Delay(1);
    for (i = 0; i < 8; i++)
    {
        T_SCK_CLR;

        if (data & 0x80)
            T_WR_SET;
        else
            T_WR_CLR;
        data = data << 1;
        Delay(1);

        T_SCK_SET;
        tmp = tmp << 1;
        tmp |= T_IN_STATUE;
        Delay(1);
    }

    return tmp;
}

u32 SPI_X(void)
{
    u16 i, j, k;
    //     GPIO_ResetBits(GPIOA,GPIO_Pin_15);
    for (i = 0; i < TOUCH_READ_TIMES; i++) //����4��.
    {
        T_CS_CLR; //cs
        SPI_SndRecv(0xD0);
        x_addata[i] = SPI_SndRecv(0x00);
        x_addata[i] <<= 8;
        x_addata[i] |= SPI_SndRecv(0x00);
        x_addata[i] >>= 3;
        T_CS_SET;
    }
    for (i = 0; i < TOUCH_READ_TIMES; i++)
    {
        for (j = TOUCH_READ_TIMES; j < TOUCH_READ_TIMES - 1; j++)
        {
            if (x_addata[j] > x_addata[i])
            {
                k = x_addata[i];
                x_addata[i] = x_addata[j];
                x_addata[j] = k;
            }
        }
    }
    Temp = (x_addata[1] + x_addata[2]) >> 1;
    r = Temp - 200;
    r *= 240;
    sx = r / (4000 - 200);
    if (sx <= 0 || sx > 240)
        return 0;
    return sx;
}

u32 SPI_Y(void)
{
    u16 i, j, k;
    //	GPIO_ResetBits(GPIOA,GPIO_Pin_15);
    for (i = 0; i < TOUCH_READ_TIMES; i++)
    {             //����4��.
        T_CS_CLR; //cs
        SPI_SndRecv(0x90);
        y_addata[i] = SPI_SndRecv(0x00);
        y_addata[i] <<= 8;
        y_addata[i] |= SPI_SndRecv(0x00);
        y_addata[i] >>= 3;
        T_CS_SET;
    }
    for (i = 0; i < TOUCH_READ_TIMES; i++)
    {
        for (j = TOUCH_READ_TIMES; j < TOUCH_READ_TIMES - 1; j++)
        {
            if (y_addata[j] > y_addata[i])
            {
                k = y_addata[i];
                y_addata[i] = y_addata[j];
                y_addata[j] = k;
            }
        }
    }
    Temp = (y_addata[1] + y_addata[2]) >> 1;
    r = Temp - 190;
    r *= 320;
    sy = r / (4000 - 190);
    if (sy <= 0 || sy > 320)
        return 0;
    return sy;
}

void TOUCH_INT_config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    /* Enable GPIOB, GPIOC and AFIO clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    /* LEDs pins configuration */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void TOUCH_INT_EXIT_Init(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); //AFIO??
                                                         /* Connect Button EXTI Line to Button GPIO Pin */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource2);
    /* Configure Button EXTI line */
    EXTI_InitStructure.EXTI_Line = EXTI_Line2;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    EXTI_ClearITPendingBit(EXTI_Line2);
}
void TOUCH_InterruptConfig(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    /* Set the Vector Table base address at 0x08000000 */
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000);
    /* Configure the Priority Group to 2 bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*static void LCD_BIG_POINT(u16 xScreen, u16 yScreen) {
	;
}*/
static void getPos(u16 xScreen, u16 yScreen)
{
    u16 x = xScreen, y = yScreen;
    int i, j;
    int dist = 0;
    for (i = QPSTARTX + QPINTERVAL / 2, dist = 9999; i <= QPSTARTX + QPSIZEX; i += QPINTERVAL)
    {
        if (abs(i - x) <= dist)
        {
            xScreen = i;
            dist = abs(i - x);
        }
    }
    for (j = QPSTARTY + QPINTERVAL / 2, dist = 9999; j <= QPSTARTY + QPSIZEY; j += QPINTERVAL)
    {
        if (abs(j - y) <= dist)
        {
            yScreen = j;
            dist = abs(j - y);
        }
    }
    //		play(xScreen/QPINTERVAL, yScreen/QPINTERVAL);
}

void TouchScreen()
{
    //	static u16 sDataX,sDataY;

    xScreen = SPI_X();
    yScreen = SPI_Y();

//    if ((xScreen > 1) && (yScreen > 1) && (xScreen < 240 - 1) && (yScreen < 240 - 1))
//    {
//        /*if(!(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_2)) ) {
//			if((xScreen < QPSIZEX/QPINTERVAL*QPINTERVAL-1)&&(yScreen <= QPSIZEY/QPINTERVAL*QPINTERVAL-1)) {
//				getPos(xScreen,yScreen);
//				chessFlag=(chessFlag+1)%3;
//			}*/
//        if ((chessTable[((int)xScreen / 40) * 40 + 20][((int)yScreen / 40) * 40 + 20] == 1) && chessFlag == 0)
//            chessFlag = 1;
//        if (chessFlag == 1 && (chessTable[((int)xScreen / 40) * 40 + 20][((int)yScreen / 40) * 40 + 20] == 0))
//            chessFlag = 2;
//    }
}

u16 touchFlag = 0;
void EXTI2_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line2) != RESET)
    {
        touchFlag = 1;
        EXTI_ClearITPendingBit(EXTI_Line2);
    }
}

void Touch_test()
{
    TOUCH_SCREEN_INIT();
    TOUCH_INT_config();
    TOUCH_INT_EXIT_Init();
    TOUCH_InterruptConfig();

}
