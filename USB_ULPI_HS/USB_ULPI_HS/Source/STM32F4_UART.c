
#include "STM32F4_UART.h"
#include <stdio.h>

void UART_Init(void)
{
   USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* 开启GPIO_D的时钟 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	/* 开启串口6的时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);

	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14;
	GPIO_Init(GPIOG, &GPIO_InitStructure);

	
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource9, GPIO_AF_USART6);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource14, GPIO_AF_USART6);


	USART_InitStructure.USART_BaudRate   = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits   = USART_StopBits_1;
	USART_InitStructure.USART_Parity     = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode                = USART_Mode_Tx | USART_Mode_Rx;

	USART_Init(USART6, &USART_InitStructure);

	/* 使能串口6 */
	USART_Cmd(USART6, ENABLE);
	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);
}

int fputc(int ch, FILE *f)
{
	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART */
	USART_SendData(USART6, (uint8_t) ch);

	/* Loop until the end of transmission */
	while (USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET)
	{}

	return ch;
}



