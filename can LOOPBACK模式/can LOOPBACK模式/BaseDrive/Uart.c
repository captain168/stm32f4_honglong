
#include "stm32f4xx.h"
#include "stdio.h"
#include "uart.h"

void uart3_init(void)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* 开启GPIO_D的时钟 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	/* 开启串口3的时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);


	USART_InitStructure.USART_BaudRate   = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits   = USART_StopBits_1;
	USART_InitStructure.USART_Parity     = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode                = USART_Mode_Tx | USART_Mode_Rx;

	USART_Init(USART3, &USART_InitStructure);

	/* 使能串口3 */
	USART_Cmd(USART3, ENABLE);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
}

/*********************************************************************************************************
** Function name:       User_Uart3SendChar
** Descriptions:        从串口发送数据
** input parameters:    ch: 发送的数据
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void User_Uart3SendChar(unsigned char ch)
{
     USART_SendData (USART3,ch);                                        /* 发送字符                     */
     while( USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET );         /* 等待发送完成                 */
     USART_ClearFlag(USART3,USART_FLAG_TC);                             /* 清除发送完成标识             */
}

/*********************************************************************************************************
** Function name:       User_Uart3SendString
** Descriptions:        向串口发送字符串
** input parameters:    s:   要发送的字符串指针
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void User_Uart3SendString(unsigned char *s)
{
    while (*s != '\0') {
           User_Uart3SendChar(*s++);
    }
}

void Uart3_IRQ(void)
{
	unsigned char ch;				
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		/* Read one byte from the receive data register */
		ch = (USART_ReceiveData(USART3));
        //User_Uart3SendChar(ch);

        User_CANTransmit(ch);     // CAN1转发数据                
	} 
}

int fputc(int ch, FILE *f)
{
	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART */
	USART_SendData(USART3, (uint8_t) ch);

	/* Loop until the end of transmission */
	while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET)
	{}

	return ch;
}


