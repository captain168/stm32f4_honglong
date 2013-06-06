/*
 * File      : uart.c
 * RT-Thread evaluation board RealTouch sample with no os
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2012/08/08     bloom5       first version
 */

#include <stdarg.h>
#include <stdio.h>
#include <stm32f4xx.h>
#include <stm32f4xx_usart.h>
#include "uart.h"

void uart_init(void)
{
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 开启GPIO_B的时钟 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
    /* 开启串口3的时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14;

    GPIO_Init(GPIOG, &GPIO_InitStructure);

    /* 将PB10作为复用功能中的USART3通信引脚使用 */
    GPIO_PinAFConfig(GPIOG, GPIO_PinSource14, GPIO_AF_USART6);

	
    USART_InitStructure.USART_BaudRate   = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits   = USART_StopBits_1;
    USART_InitStructure.USART_Parity     = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                = USART_Mode_Tx;
    
    USART_Init(USART6, &USART_InitStructure);

    /* 使能串口3 */
    USART_Cmd(USART6, ENABLE);
}

static void _send(const char *str, unsigned int size)
{
    int pos = 0;

    while(size)
    {
        if (str[pos] == '\0')
            break;
        
				/* 等待缓冲区空 */
        while(!(USART6->SR & 0x80));
        /* 发送数据 */
        USART6->DR = str[pos];

        pos ++;
        size --;
    }
}

void debug(const char* fmt,...)
{
    va_list ap;
    char string[129];

    string[128]='\0';
    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
    va_end(ap);
    _send(string,64);
}
