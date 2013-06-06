/****************************************Copyright (c)***************************************
**                                 http://www.openmcu.com
**-------------------------------------------------------------------------------------------
** File name:           main.c
** Last modified Date:  2012-12-28
** Last Version:        V1.0
** Descriptions:        红龙407开发板 LED测试
**
**-------------------------------------------------------------------------------------------
** Created by:          FXL
** Last modified Date:  2012-12-28
** Last Version:        V1.00
** Descriptions:        红龙407开发板 LED测试
**
**-------------------------------------------------------------------------------------------
** Modified by:         
** Modified date:       
** Version:             
** Descriptions:        
**-------------------------------------------------------------------------------------------
********************************************************************************************/
#include "stm32f4xx.h"
#include "Gpio.h"

void Delay(__IO uint32_t nCount);  //函数声明

int main(void)
{
  LEDGpio_Init();                  //GPIO管脚初始化

  while (1)
  {
      LED4_ONOFF(Bit_SET);         //LED4置高（灭灯） 从原理图可以看出，LED为低电平亮。
      LED1_ONOFF(Bit_RESET);       //LED1置低（开灯）
      Delay(0X3FFFFF);             //延时
      LED1_ONOFF(Bit_SET);         //以下同上注视
      LED2_ONOFF(Bit_RESET);
      Delay(0X3FFFFF);
      LED2_ONOFF(Bit_SET);
      LED3_ONOFF(Bit_RESET);
      Delay(0X3FFFFF);
      LED3_ONOFF(Bit_SET);
      LED4_ONOFF(Bit_RESET);
      Delay(0X3FFFFF);
  }
}
/********************************************************************************************
*函数名称：void Delay(__IO uint32_t nCount)
*
*入口参数：无
*
*出口参数：无
*
*功能说明：延时函数
*******************************************************************************************/
void Delay(__IO uint32_t nCount)
{
  while(nCount--)
  {
  }
}


