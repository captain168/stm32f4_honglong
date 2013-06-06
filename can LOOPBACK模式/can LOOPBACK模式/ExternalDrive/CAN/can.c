/****************************************Copyright (c)****************************************************
**                                 http://www.openmcu.com
**--------------File Info---------------------------------------------------------------------------------
** File name:           can.c
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
#include "can.h"
#include "uart.h"
//#include "led.h"

 CanRxMsg RxMessage;                                                    /*  全局变量用来传递接收报文    */

/*********************************************************************************************************
** Function name:       UserCAN1_NVIC_Config
** Descriptions:        允许CAN1接收中断 
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
static void UserCAN1_NVIC_Config(void)
{
  NVIC_InitTypeDef  NVIC_InitStructure;
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

#ifdef  USE_CAN1 
  NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
#else  /* USE_CAN2 */
  NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;
#endif /* USE_CAN1 */

  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/*********************************************************************************************************
** Function name:       User_CAN1Init
** Descriptions:        CAN1初始化,500kbps
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void User_CAN1Init(void)
{ 
  CAN_InitTypeDef        CAN_InitStructure;                             /*  定义CAN初始化结构体         */
  CAN_FilterInitTypeDef  CAN_FilterInitStructure;                       /*  定义过滤器初始化结构体      */
  GPIO_InitTypeDef       GPIO_InitStructure;                            /*  定义GPIO初始化结构体        */

  UserCAN1_NVIC_Config();                                               /*  允许CAN1接收中断            */

/*-----------将CAN引脚设置到相关模式 ----------------------------------*/
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|                        /*  使能相关CAN引脚时钟         */
//                           RCC_APB2Periph_AFIO,ENABLE);    
//    GPIO_PinRemapConfig(GPIO_Remap1_CAN1,ENABLE);                       /*  重映射CAN引脚               */
//
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//    GPIO_Init(GPIOB, &GPIO_InitStructure);
//
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//    GPIO_InitStructure.GPIO_Speed =    GPIO_Speed_10MHz;
//    GPIO_Init(GPIOB, &GPIO_InitStructure);
    /* Enable GPIO clock */
    RCC_AHB1PeriphClockCmd(CAN_GPIO_CLK, ENABLE);
    
    /* Connect CAN pins to AF9 */
    GPIO_PinAFConfig(CAN_GPIO_PORT, CAN_RX_SOURCE, CAN_AF_PORT);
    GPIO_PinAFConfig(CAN_GPIO_PORT, CAN_TX_SOURCE, CAN_AF_PORT); 
    
    /* Configure CAN RX and TX pins */
    GPIO_InitStructure.GPIO_Pin = CAN_RX_PIN | CAN_TX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(CAN_GPIO_PORT, &GPIO_InitStructure);

/*-----------CAN控制器初始化 ------------------------------------------*/
  RCC_APB1PeriphClockCmd(CAN_CLK, ENABLE);

  CAN_DeInit(CAN1);                                                     /*  复位所有设置                */

  CAN_InitStructure.CAN_TTCM = DISABLE;
  CAN_InitStructure.CAN_ABOM = ENABLE ;                                 /*  使能离线管理                */
  CAN_InitStructure.CAN_AWUM = DISABLE;
  CAN_InitStructure.CAN_NART = ENABLE ;                                 /*  使能自动重传                */
  CAN_InitStructure.CAN_RFLM = DISABLE;
  CAN_InitStructure.CAN_TXFP = DISABLE;                                 /*  优先级由什么决定            */
  CAN_InitStructure.CAN_Mode = CAN_Mode_LoopBack;  //;     CAN_Mode_Normal                    /*  设置为正常模式              */
  CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;                              /*  波特率相关设置              */
  CAN_InitStructure.CAN_BS1 = CAN_BS1_6tq;
  CAN_InitStructure.CAN_BS2 = CAN_BS2_5tq;
  CAN_InitStructure.CAN_Prescaler = 14;                                 //250
  CAN_Init(CAN1, &CAN_InitStructure);                                   /*  载入设置                    */

/*-----------初始化滤波设置--------------------------------------------*/
  CAN_FilterInitStructure.CAN_FilterNumber=0;
  CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;
  CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;
  CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;
  CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment=0;
  CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
  CAN_FilterInit(&CAN_FilterInitStructure);                             /*  载入设置                    */           
 
  CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
}

/*********************************************************************************************************
** Function name:       User_CANTransmit
** Descriptions:        CAN1发送数据
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void User_CANTransmit(uint16_t val){
  CanTxMsg TxMessage;                                                   /*  定义CAN报文结构体           */
  uint8_t  TransmitMailbox;

  TxMessage.StdId=val&0x7FF;                                            /*  动态填充帧ID，防止碰撞      */
  //TxMessage.ExtId=0x0001;
  TxMessage.RTR=CAN_RTR_DATA;                                           /*  数据帧                      */
  TxMessage.IDE=CAN_ID_STD;                                             /*  标准帧                      */
  TxMessage.DLC=2;                                                      /*  数据场长度                  */
  TxMessage.Data[0]=val;                                                /*  填充数据场                  */
  TxMessage.Data[1]=(val>>8);                                           /*  填充数据场                  */
     
  TransmitMailbox=CAN_Transmit(CAN1,&TxMessage);                        /*  发送并获取邮箱号            */ 
  while((CAN_TransmitStatus(CAN1,TransmitMailbox) != CANTXOK));         /*  等待发送完成                */ 
}

/*********************************************************************************************************
** Function name:       CAN1_RX0_ISR
** Descriptions:        CAN1接收中断服务程序
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void CAN1_RX0_ISR(void)
{
    if(CAN_GetITStatus(CAN1,CAN_IT_FMP0)!= RESET)
    {
        uint16_t ch;

        CAN_ClearITPendingBit(CAN1,CAN_IT_FMP0);                        /*  清除中断标志                */
     
        CAN_Receive(CAN1,CAN_FIFO0,&RxMessage);                         /* 读取数据                     */
        ch=(RxMessage.Data[1]<<8)|RxMessage.Data[0];
        User_Uart3SendChar(ch);                                         /* 串口1转发数据                */          
      
//        LED_Turn(GPIO_Pin_7);                                           /* 指示CAN接收到数据            */ 
    }
}
/*********************************************************************************************************
**                                        End Of File
*********************************************************************************************************/

