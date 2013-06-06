/****************************************Copyright (c)***************************************
**                                 http://www.openmcu.com
**-------------------------------------------------------------------------------------------
** File name:           main.c
** Last modified Date:  2013-01-04
** Last Version:        V1.0
** Descriptions:        红龙407开发板 按键例程
**
**-------------------------------------------------------------------------------------------
** Created by:          FXL
** Last modified Date:  2013-01-04
** Last Version:        V1.00
** Descriptions:        红龙407开发板 按键例程
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

void Delay(int counter);                      //函数声明

int main(void)
{
    KEYGpio_Init();                           //按键IO口初始化
    LEDGpio_Init();                           //LED灯IO口初始化

    while (1)
    {
        if(KEY_S1_UPDOWM() == Bit_RESET)      //判断按键状态
        {
            Delay(0x8fffff);                  //延时去抖
            if(KEY_S1_UPDOWM() == Bit_RESET)  //再次确认按键状态
            {
                if(GPIO_ReadOutputDataBit(LED1_GPIO,LED1_GPIO_PIN) == Bit_SET)     //判断灯的当前状态，然后对灯取反
                    LED1_ONOFF(Bit_RESET);
                else
                    LED1_ONOFF(Bit_SET); 
            }        
        }

        if(KEY_S2_UPDOWM() == Bit_RESET)
        {
            Delay(0x8fffff);
            if(KEY_S2_UPDOWM() == Bit_RESET)
            {
                if(GPIO_ReadOutputDataBit(LED2_GPIO,LED2_GPIO_PIN) == Bit_SET)     //判断灯的当前状态，然后对灯取反
                    LED2_ONOFF(Bit_RESET);
                else
                    LED2_ONOFF(Bit_SET); 
            }        
        }

        if(KEY_S3_UPDOWM() == Bit_RESET)
        {
            Delay(0x8fffff);
            if(KEY_S3_UPDOWM() == Bit_RESET)
            {
                if(GPIO_ReadOutputDataBit(LED3_GPIO,LED3_GPIO_PIN) == Bit_SET)     //判断灯的当前状态，然后对灯取反
                    LED3_ONOFF(Bit_RESET);
                else
                    LED3_ONOFF(Bit_SET); 
            }        
        }

        if(KEY_S4_UPDOWM() == Bit_RESET)
        {
            Delay(0x8fffff);
            if(KEY_S4_UPDOWM() == Bit_RESET)
            {
                if(GPIO_ReadOutputDataBit(LED4_GPIO,LED4_GPIO_PIN) == Bit_SET)     //判断灯的当前状态，然后对灯取反
                    LED4_ONOFF(Bit_RESET);
                else
                    LED4_ONOFF(Bit_SET); 
            }        
        }

        if(KEY_S5_UPDOWM() == Bit_RESET)
        {
            Delay(0x8fffff);
            if(KEY_S5_UPDOWM() == Bit_RESET)
            {
                if(GPIO_ReadOutputDataBit(LED1_GPIO,LED1_GPIO_PIN) == Bit_SET)     //判断灯的当前状态，然后对灯取反
                {
                    LED1_ONOFF(Bit_RESET);
                    LED2_ONOFF(Bit_RESET);
                    LED3_ONOFF(Bit_RESET);
                    LED4_ONOFF(Bit_RESET);
                }
                else
                {
                    LED1_ONOFF(Bit_SET);
                    LED2_ONOFF(Bit_SET);
                    LED3_ONOFF(Bit_SET);
                    LED4_ONOFF(Bit_SET);
                }
            }
        }
    }
}

void Delay(int counter)
{
    while(counter--);
}



