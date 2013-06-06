/****************************************Copyright (c)****************************************************
**                                 http://www.openmcu.com
**--------------File Info---------------------------------------------------------------------------------
** File name:           can.h
** Last modified Date:  2012-07-23
** Last Version:        V1.00
** Descriptions:        
**
**--------------------------------------------------------------------------------------------------------
** Created by:          openmcu
** Created date:        2012-07-23
** Version:             V1.00
** Descriptions:        编写示例代码
**
**--------------------------------------------------------------------------------------------------------
** Modified by:         
** Modified date:       
** Version:             
** Descriptions:        
**
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "stm32f4xx.h"

#ifndef __CAN_H__
#define __CAN_H__

#define  USE_CAN1

#ifdef  USE_CAN1
  #define CANx                       CAN1
  #define CAN_CLK                    RCC_APB1Periph_CAN1
  #define CAN_RX_PIN                 GPIO_Pin_0
  #define CAN_TX_PIN                 GPIO_Pin_1
  #define CAN_GPIO_PORT              GPIOD
  #define CAN_GPIO_CLK               RCC_AHB1Periph_GPIOD
  #define CAN_AF_PORT                GPIO_AF_CAN1
  #define CAN_RX_SOURCE              GPIO_PinSource0
  #define CAN_TX_SOURCE              GPIO_PinSource1       
#else /*USE_CAN2*/
  #define CANx                       CAN2
  #define CAN_CLK                    (RCC_APB1Periph_CAN1 | RCC_APB1Periph_CAN2)
  #define CAN_RX_PIN                 GPIO_Pin_5
  #define CAN_TX_PIN                 GPIO_Pin_13
  #define CAN_GPIO_PORT              GPIOB
  #define CAN_GPIO_CLK               RCC_AHB1Periph_GPIOB
  #define CAN_AF_PORT                GPIO_AF_CAN2
  #define CAN_RX_SOURCE              GPIO_PinSource5
  #define CAN_TX_SOURCE              GPIO_PinSource13    
#endif  /* USE_CAN1 */

extern  CanRxMsg RxMessage;                                             /*  全局变量用来传递接收报文    */



/*********************************************************************************************************
**        外部接口函数声明
*********************************************************************************************************/
extern void	User_CAN1Init(void);
extern void User_CANTransmit(uint16_t val);
extern void CAN1_RX0_ISR(void);

#endif
/*********************************************************************************************************
**                                        End Of File
*********************************************************************************************************/

