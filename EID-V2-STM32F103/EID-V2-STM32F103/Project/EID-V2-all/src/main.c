#include "stm32f10x.h"
#include "delay.h"
#include "lcd.h"

extern u8 chessFlag;
extern u32 xScreen, yScreen;
extern int chessTable[5][5];

void BoardInit(void);
void drawChessBoard(void);
void drawBTN(void);
void drawInitGraph(void);
void drawInitChess(void);
void resetChessBoard(void); // 当按下rst的时候触发
void getXY(int* pos);
int isRST(int* pos);
int isChessBoard(int* pos, int* xy, int* ChessPos); // 如果是1，则说明在棋盘中，如果不是1，则说明不在棋盘中
void judge(void);
void win(int flag);

//#define _Exp_LED_  //ok
//#define _Exp_KEY_	//ok
//#define _EXP_BEEP_	//ok
//#define _EXP_UART_	//ok
#define _EXP_LCD_ //OK
//#define _EXP_SPI_  //OK
//#define _EXP_AIAO_  //AI OK锟斤拷 AO not supported by 103VB
//#define _EXP_COUNTER_  /OK
//#define _EXP_IIC_		 //OK
//#define _EXP_PWM_
//#define _EXP_7SEG_		 //OK

// int FlashID;

// void ExpTest()
// {
// #ifdef _Exp_LED_
// 	LED_Configuration(); //LED锟斤拷锟斤拷
// 	LED_Blink();
// #endif

// #ifdef _EXP_BEEP_
// 	BEEP_Configuration();
// 	Beep();

// #endif

// #ifdef _Exp_KEY_
// 	key_test();
// #endif

// #ifdef _EXP_UART_
// 	uart_test();
// #endif

// #ifdef _EXP_SPI_
// 	ENC25Q80_SPI2_Init();
// 	FlashID = SPI_FLASH_ReadDeviceID(); //ID = 0x13
// #endif

// #ifdef _EXP_AIAO_
// 	AI_Configuration();
// 	DAC1_Configuration();
// 	AIAO_test();
// #endif

// #ifdef _EXP_COUNTER_
// 	Counter_Configuration();
// 	Counter_test();
// #endif

// #ifdef _EXP_IIC_
// 	IIC_Configuration();
// 	IIC_test();
// #endif

// #ifdef _EXP_PWM_
// 	PWM_Configuration();
// 	PWM_test();
// #endif

// #ifdef _EXP_LCD_
// 	LCD_Configuration();
// 	TOUCH_SCREEN_INIT();
// 	//LCD_test();

// #endif

// #ifdef _EXP_7SEG_
// 	SEG_Configuration();
// 	SEG_test();
// #endif
// }
int pastChessBoard[5][5];
int turn = 1;
int endFlag;
int main(void)
{
	BoardInit();
	turn = 1;
	endFlag = 0;
	int pos[2] = {0, 0};
	int xy[2] = {0, 0};
	int ChessPos[2] = {0, 0};
	int chosen = 0;
	int chosenChess[2] = {0, 0};
	int chosenPos[2] = {0, 0};
	while (1)
	{
		if (touchFlag >= 1)
		{
			getXY(pos);
			if(isRST(pos))
			{
				resetChessBoard();
				chosen = 0;
				turn = 1;
			}
			else if(endFlag == 0 && isChessBoard(pos, xy, ChessPos))
			{
				if(chessTable[xy[0]][xy[1]] == turn)
				{
					if(chosen == 1)
					{
						if(turn == 1)
						{
							CHESS_COLOR = BLACK;
						}
						else
						{
							CHESS_COLOR = WHITE;
						}
						LCD_Draw_Circle_Chess(chosenPos[0], chosenPos[1], 15);
					}
					CHESS_COLOR = RED;
					chosenChess[0] = xy[0];
					chosenChess[1] = xy[1];
					chosenPos[0] = ChessPos[0];
					chosenPos[1] = ChessPos[1];
					LCD_Draw_Circle_Chess(ChessPos[0], ChessPos[1], 15);
					if(turn == 1)
					{
						CHESS_COLOR = BLACK;
					}
					else
					{
						CHESS_COLOR = WHITE;
					}
					chosen = 1;
				}
				else if(((xy[0] == chosenChess[0] - 1 && xy[1] == chosenChess[1]) ||
						(xy[0] == chosenChess[0] + 1 && xy[1] == chosenChess[1]) ||
						(xy[0] == chosenChess[0] && xy[1] == chosenChess[1] - 1) || 
						(xy[0] == chosenChess[0] && xy[1] == chosenChess[1] + 1)))
				{
					if(chessTable[xy[0]][xy[1]] == 0 && chosen == 1)
					{
						for(int i = 0; i < 5; i++)
						{
							for(int j = 0; j < 5; j++)
							{
								pastChessBoard[i][j] = chessTable[i][j];
							}
						}

						LCD_Rm_Circle_Chess(chosenPos[0], chosenPos[1], 15);
						chessTable[chosenChess[0]][chosenChess[1]] = 0;
						LCD_Draw_Circle_Chess(ChessPos[0], ChessPos[1], 15);
						chessTable[xy[0]][xy[1]] = turn;

						judge();

						if(turn == 1)
						{
							turn = 2;
						}else{
							turn = 1;
						}
						chosen = 0;
					}
				}
				else if(chosen)
				{
					LCD_Draw_Circle_Chess(chosenPos[0], chosenPos[1], 15);
					chosen = 0;
				}
			}
			//delay_ms(200);
			touchFlag = 0;
		}
		//		if (chessFlag == 1)
		//		{
		//			LCD_Rm_Circle_Chess((xScreen / 40) * 40 + 20, (yScreen / 40) * 40 + 20, 15);
		//		}
		//		else if (chessFlag == 2)
		//		{
		//			LCD_Draw_Circle_Chess((xScreen / 40) * 40 + 20, (yScreen / 40) * 40 + 20, 15);
		//			chessFlag = 0;
		//		}
	}
}


void BoardInit(void)
{
	SystemInit();
	delay_init();
	LCD_Configuration();
	LCD_Init();
	TOUCH_SCREEN_INIT();
    TOUCH_INT_config();
    TOUCH_INT_EXIT_Init();
    TOUCH_InterruptConfig();
	drawInitGraph();

	for(int i = 0; i < 5; i++)
	{
		chessTable[i][0] = 1;
		chessTable[i][1] = 0;
		chessTable[i][2] = 0;
		chessTable[i][3] = 0;
		chessTable[i][4] = 2;
	}
}

void drawChessBoard(void)
{
	//	LCD_DrawLine(0, 40, 240, 40);
	//	LCD_DrawLine(0, 80, 240, 80);
	//	LCD_DrawLine(0, 120, 240, 120);
	//	LCD_DrawLine(0, 160, 240, 160);
	//	LCD_DrawLine(0, 200, 240, 200);
	//	LCD_DrawLine(0, 240, 240, 240);
	//	LCD_DrawLine(40, 0, 40, 240);
	//	LCD_DrawLine(80, 0, 80, 240);
	//	LCD_DrawLine(120, 0, 120, 240);
	//	LCD_DrawLine(160, 0, 160, 240);
	//	LCD_DrawLine(200, 0, 200, 240);
	LCD_DrawLine(40, 40, 40, 200);
	LCD_DrawLine(80, 40, 80, 200);
	LCD_DrawLine(120, 40, 120, 200);
	LCD_DrawLine(160, 40, 160, 200);
	LCD_DrawLine(200, 40, 200, 200);
	LCD_DrawLine(40, 40, 200, 40);
	LCD_DrawLine(40, 80, 200, 80);
	LCD_DrawLine(40, 120, 200, 120);
	LCD_DrawLine(40, 160, 200, 160);
	LCD_DrawLine(40, 200, 200, 200);
}

void drawBTN(void)
{
	LCD_DrawRectangle(80, 260, 160, 300);
	LCD_ShowString(100, 270, 40, 20, 24, (u8 *)("RST"));
}

void drawInitGraph(void)
{
	drawChessBoard();
	drawBTN();
	drawInitChess();
}

void drawInitChess(void)
{
	CHESS_COLOR = BLACK;
	LCD_Draw_Circle_Chess(40, 40, 15);
	LCD_Draw_Circle_Chess(40, 80, 15);
	LCD_Draw_Circle_Chess(40, 120, 15);
	LCD_Draw_Circle_Chess(40, 160, 15);
	LCD_Draw_Circle_Chess(40, 200, 15);
	CHESS_COLOR = WHITE;
	LCD_Draw_Circle_Chess(200, 40, 15);
	LCD_Draw_Circle_Chess(200, 80, 15);
	LCD_Draw_Circle_Chess(200, 120, 15);
	LCD_Draw_Circle_Chess(200, 160, 15);
	LCD_Draw_Circle_Chess(200, 200, 15);
}

void resetChessBoard(void)
{
	LCD_Fill(25, 25, 215, 215, BROWN);
	endFlag = 0;
	LCD_Fill(60 , 220, 170, 250, BROWN);
	for(int i = 0; i < 5; i++)
	{
		chessTable[i][0] = 1;
		chessTable[i][1] = 0;
		chessTable[i][2] = 0;
		chessTable[i][3] = 0;
		chessTable[i][4] = 2;
	}
	
	drawChessBoard();
	drawInitChess();
}

void getXY(int* pos)
{
	int x = 0, y = 0;
	int count = 0;
	for(int i = 0; i < 4; i++)
	{
		TouchScreen();
		if(xScreen >= 10 && yScreen >= 10 && xScreen <= 240 && yScreen <= 320)
		{
			count++;
			x += xScreen;
			y += yScreen;
		}
	}
	pos[0] = x/count;
	pos[1] = y/count;
}

int isRST(int* pos)
{
	if(pos[0] >= 80 && pos[0] <= 160 && pos[1] >= 260 && pos[1] <= 300)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int isChessBoard(int* pos, int* xy, int* ChessPos)
{
	if(!(pos[0] >= 20 &&  pos[0] <= 220 && pos[1] >= 20 && pos[1] <= 220))
	{
		return 0;
	}
	else
	{
		int col, row;
		col = (pos[0] - 20) / 40;
		row = (pos[1] - 20) / 40;
		xy[0] = row;
		xy[1] = col;
		ChessPos[0] = (col + 1) * 40;
		ChessPos[1] = (row + 1) * 40;
		return 1;
	}
}

void judge(void)
{
	int eatFlag = 0;
	int count = 0;
	int beginIndex = 0;
	// 横向
	for(int i = 0; i < 5; i++)
	{
		for(int j = 0; j < 5; j++)
		{
			if(chessTable[i][j] != 0)
			{
				count++;
			}
		}
		if(count == 3)
		{
			for(int j = 0; j < 3; j++)
			{
				if(chessTable[i][j] != 0 && chessTable[i][j] == chessTable[i][j+1] && 
					chessTable[i][j+2] != 0 && chessTable[i][j+2] != chessTable[i][j])
				{
					if(pastChessBoard[i][j] == chessTable[i][j] && pastChessBoard[i][j+1] == chessTable[i][j+1])
					{
						break;
					}
					else
					{
						chessTable[i][j+2] = 0;
						LCD_Rm_Circle_Chess((j+3)*40, (i+1)*40, 15);
						eatFlag = 1;
						break;
					}
				}
			}
			for(int j = 4; j > 1; j--)
			{
				if(chessTable[i][j] != 0 && chessTable[i][j] == chessTable[i][j-1] && 
					chessTable[i][j-2] != 0 && chessTable[i][j-2] != chessTable[i][j])
				{
					if(pastChessBoard[i][j] == chessTable[i][j] && pastChessBoard[i][j-1] == chessTable[i][j-1])
					{
						break;
					}
					else
					{
						chessTable[i][j-2] = 0;
						LCD_Rm_Circle_Chess((j-1)*40, (i+1)*40, 15);
						eatFlag = 1;
						break;
					}
				}
			}
		}
		count = 0;
	}
	// 纵向
	for(int i = 0; i < 5; i++)
	{
		for(int j = 0; j < 5; j++)
		{
			if(chessTable[j][i] != 0)
			{
				count++;
			}
		}
		if(count == 3)
		{
			for(int j = 0; j < 3; j++)
			{
				if(chessTable[j][i] != 0 && chessTable[j][i] == chessTable[j + 1][i] && 
					chessTable[j + 2][i] != 0 && chessTable[j + 2][i] != chessTable[j][i])
				{
					if(pastChessBoard[j][i] == chessTable[j][i] && pastChessBoard[j+1][i] == chessTable[j+1][i])
					{
						break;
					}
					else
					{
						chessTable[j+2][i] = 0;
						LCD_Rm_Circle_Chess((i+1)*40, (j+3)*40, 15);
						eatFlag = 1;
						break;
					}
				}
			}
			for(int j = 4; j > 1; j--)
			{
				if(chessTable[j][i] != 0 && chessTable[j][i] == chessTable[j-1][i] && 
					chessTable[j-2][i] != 0 && chessTable[j-2][i] != chessTable[j][i])
				{
					if(pastChessBoard[j][i] == chessTable[j][i] && pastChessBoard[j-1][i] == chessTable[j-1][i])
					{
						break;
					}
					else
					{
						chessTable[j-2][i] = 0;
						LCD_Rm_Circle_Chess((i+1)*40, (j-1)*40, 15);
						eatFlag = 1;
						break;
					}
				}
			}
		}
		count = 0;
	}



	if(eatFlag)
	{
		int anotherCount = 0;
		for(int i = 0; i < 5; i++)
		{
			for(int j = 0; j < 5; j++)
			{
				if(chessTable[i][j] == (turn % 2 + 1))
				{
					anotherCount++;
				}
			}
		}
		if(anotherCount <= 1)
		{
			win(turn);
			return;
		}
	}

	int freeFlag = 0;
	for(int i = 0; i < 5; i++)
	{
		for(int j = 0; j < 5; j++)
		{
			if(chessTable[i][j] == (turn % 2 + 1))
			{
				if(i != 0 && chessTable[i-1][j] == 0)
				{
					freeFlag = 1;
					return;
				}
				if(i!= 4 && chessTable[i+1][j] == 0)
				{
					freeFlag = 1;
					return;
				}
				if(j != 0 && chessTable[i][j-1] == 0)
				{
					freeFlag = 1;
					return;
				}
				if(j != 4 && chessTable[i][j+1] == 0)
				{
					freeFlag = 1;
					return;
				}
			}
		}
	}
	if(freeFlag == 0)
	{
		win(turn);
	}
}

void win(int flag)
{
	if(flag == 1)
	{
		LCD_ShowString(60, 220, 110, 40, 24, (u8 *)("BLACK WIN"));
	}
	else
	{
		LCD_ShowString(60, 220, 110, 40, 24, (u8 *)("WHITE WIN"));
	}
	endFlag = 1;
}