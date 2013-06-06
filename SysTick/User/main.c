/****************************************Copyright (c)***************************************
**                                 http://www.openmcu.com
**-------------------------------------------------------------------------------------------
** File name:           main.c
** Last modified Date:  2013-01-04
** Last Version:        V1.0
** Descriptions:        红龙407开发板 SysTick例程
**
**-------------------------------------------------------------------------------------------
** Created by:          FXL
** Last modified Date:  2013-01-04
** Last Version:        V1.0
** Descriptions:        红龙407开发板 SysTick例程
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
#include "SysTick.h"

int main(void)
{
    SysTick_Init();        //滴答时钟初始化
    LEDGpio_Init();        //GPIO初始化

    while (1)
    {
        if(GPIO_ReadOutputDataBit(LED1_GPIO,LED1_GPIO_PIN) == Bit_SET)     //判断灯的当前状态，然后对灯取反
            LED1_ONOFF(Bit_RESET);
        else
            LED1_ONOFF(Bit_SET);
        Delay(1000);      //延时1秒
    }
}

