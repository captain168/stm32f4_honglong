/****************************************Copyright (c)***************************************
**                                 http://www.openmcu.com
**-------------------------------------------------------------------------------------------
** File name:           main.c
** Last modified Date:  2013-01-04
** Last Version:        V1.00
** Descriptions:        红龙407开发板 LCD刷屏例程
**
**-------------------------------------------------------------------------------------------
** Created by:          FXL
** Last modified Date:  2013-01-04
** Last Version:        V1.00
** Descriptions:        红龙407开发板 LCD刷屏例程
**
**-------------------------------------------------------------------------------------------
** Modified by:         
** Modified date:       
** Version:             
** Descriptions:        
**-------------------------------------------------------------------------------------------
********************************************************************************************/
#include "stm32f4xx.h"
#include "can.h"
#include "Uart.h"
#include "Nvic.h"

int main(void)
{
    uint32_t i,j;

    nvic_config();
    uart3_init();

    User_Uart3SendString("  \r\n");
    User_Uart3SendString("欢迎使用本例程\r\n");                         /* 输出欢迎信息测试串口         */
    User_Uart3SendString("www.openmcu.com\r\n");                        /* 输出欢迎信息测试串口         */
    User_Uart3SendString("\r\n");                                       /* 输出欢迎信息测试串口         */

    User_CAN1Init(); 

    while(1){
        for(i=0;i<30000;i++) {
            for(j=0;j<30;j++);
        }
       //User_Uart3SendString("www.openmcu.com\r\n"); 
       //LED_Turn(GPIO_Pin_6);                                           /* 闪烁LED表示系统无卡死        */     	   
    }
}


