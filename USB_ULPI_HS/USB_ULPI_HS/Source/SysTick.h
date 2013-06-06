/*******************************************************
*名  称STM32F4_SysTick头文件
*作  者：天下的人
*日  期：2012年12月08日
*********************************************************/
#ifndef __STM32F4xx_SYSTICK_H
#define __STM32F4xx_SYSTICK_H

#include "stm32f4xx.h"

extern void delay_init(uint32_t SYSCLK);
extern void delay_ms(uint16_t nms);
extern void delay_us(uint32_t nus);

#endif /*__STM32F4xx_SYSTICK_H */



