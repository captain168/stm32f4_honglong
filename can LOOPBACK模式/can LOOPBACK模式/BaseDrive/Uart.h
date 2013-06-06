#ifndef _UART_H_
#define _UART_H_

#include "stdio.h"

void User_Uart3SendChar(unsigned char ch);
void User_Uart3SendString(unsigned char *s);
void uart3_init(void);
void Uart3_IRQ(void);


#endif
