#ifndef _UART_
#define _UART_

extern char RS232InData;

void RS232_Configuration(void);
void NVIC_Configuration(void);
void RS232SendByte(char data);

#endif