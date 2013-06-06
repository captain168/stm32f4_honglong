/**
  ******************************************************************************
  * @file    stm32f4x7_eth_bsp.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-October-2011 
  * @brief   STM32F4x7 Ethernet hardware configuration.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4x7_eth.h"
#include "stm32f4x7_eth_bsp.h"
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t  EthInitStatus = 0;
__IO uint8_t EthLinkStatus = 0;

/* Private function prototypes -----------------------------------------------*/
static void ETH_GPIO_Config(void);
static void ETH_MACDMA_Config(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  ETH_BSP_Config
  * @param  None
  * @retval None
  */
void ETH_BSP_Config(void)
{
  RCC_ClocksTypeDef RCC_Clocks;
  
  /* Configure the GPIO ports for ethernet pins */
  ETH_GPIO_Config();
  
  /* Configure the Ethernet MAC/DMA */
  ETH_MACDMA_Config();

  if (EthInitStatus == 0)
  {
    LCD_SetTextColor(Red);
    LCD_DisplayStringLine(Line5, (uint8_t*)"   Ethernet Init   ");
    LCD_DisplayStringLine(Line6, (uint8_t*)"      failed      ");
    while(1);
  }

  /* Configure the PHY to generate an interrupt on change of link status */
  //Eth_Link_PHYITConfig(DP83848_PHY_ADDRESS);

  /* Configure the EXTI for Ethernet link status. */
  //Eth_Link_EXTIConfig();
  
  /* Configure Systick clock source as HCLK */
  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);

  /* SystTick configuration: an interrupt every 10ms */
  RCC_GetClocksFreq(&RCC_Clocks);
  SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);  
}

/**
  * @brief  Configures the Ethernet Interface
  * @param  None
  * @retval None
  */
static void ETH_MACDMA_Config(void)
{
  ETH_InitTypeDef ETH_InitStructure;

  /* Enable ETHERNET clock  */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_ETH_MAC | RCC_AHB1Periph_ETH_MAC_Tx |
                        RCC_AHB1Periph_ETH_MAC_Rx, ENABLE);
                        
  /* Reset ETHERNET on AHB Bus */
  ETH_DeInit();

  /* Software reset */
  ETH_SoftwareReset();

  /* Wait for software reset */
  while (ETH_GetSoftwareResetStatus() == SET);

  /* ETHERNET Configuration --------------------------------------------------*/
  /* Call ETH_StructInit if you don't like to configure all ETH_InitStructure parameter */
  ETH_StructInit(&ETH_InitStructure);

  /* Fill ETH_InitStructure parametrs */
  /*------------------------   MAC   -----------------------------------*/
  ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;
  //ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Disable; 
  //  ETH_InitStructure.ETH_Speed = ETH_Speed_10M;
  //  ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;   

  ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;
  ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable;
  ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;
  ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;
  ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;
  ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;
  ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;
  ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;
#ifdef CHECKSUM_BY_HARDWARE
  ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable;
#endif

  /*------------------------   DMA   -----------------------------------*/  
  
  /* When we use the Checksum offload feature, we need to enable the Store and Forward mode: 
  the store and forward guarantee that a whole frame is stored in the FIFO, so the MAC can insert/verify the checksum, 
  if the checksum is OK the DMA can handle the frame otherwise the frame is dropped */
  ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable; 
  ETH_InitStructure.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;         
  ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;     
 
  ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;       
  ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;   
  ETH_InitStructure.ETH_SecondFrameOperate = ETH_SecondFrameOperate_Enable;
  ETH_InitStructure.ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable;      
  ETH_InitStructure.ETH_FixedBurst = ETH_FixedBurst_Enable;                
  ETH_InitStructure.ETH_RxDMABurstLength = ETH_RxDMABurstLength_32Beat;          
  ETH_InitStructure.ETH_TxDMABurstLength = ETH_TxDMABurstLength_32Beat;
  ETH_InitStructure.ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_2_1;

  /* Configure Ethernet */
  EthInitStatus = ETH_Init(&ETH_InitStructure);
}

/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval None
  */
void ETH_GPIO_Config(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable SYSCFG clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC, ENABLE);
	
		SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_RMII);

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

    /* config MDIO and MDC. */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_ETH); /* config ETH_MDIO */
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource1, GPIO_AF_ETH); /* config ETH_MDC */
    /* config PA2: MDIO */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    /* config PC1: MDC */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
	
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_ETH); /* RMII_REF_CLK */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_ETH); /* RMII_CRS_DV */

    /* configure PA1:RMII_REF_CLK, PA7:RMII_CRS_DV. */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_7;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOC, GPIO_PinSource4, GPIO_AF_ETH); /* RMII_RXD0 */
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource5, GPIO_AF_ETH); /* RMII_RXD1 */

    /* configure PC4:RMII_RXD0, PC5:RMII_RXD1. */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
		
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);

    GPIO_PinAFConfig(GPIOG, GPIO_PinSource11, GPIO_AF_ETH); /* RMII_TX_EN */
    GPIO_PinAFConfig(GPIOG, GPIO_PinSource13, GPIO_AF_ETH); /* RMII_TXD0 */
    GPIO_PinAFConfig(GPIOG, GPIO_PinSource14, GPIO_AF_ETH); /* RMII_TXD1 */

    /* configure PG11:RMII_TX_EN, PG13:RMII_TXD0, PG14:RMII_TXD1 */
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_14;
    GPIO_Init(GPIOG, &GPIO_InitStructure);
//   GPIO_InitTypeDef GPIO_InitStructure;
// 	uint32_t i;
//   
//   /* Enable GPIOs clocks */
//   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB |
//                          RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOI |
//                          RCC_AHB1Periph_GPIOG | RCC_AHB1Periph_GPIOH |
//                          RCC_AHB1Periph_GPIOF, ENABLE);
// 	
// 	
// 	
// 	 

//    
// 		
// 		/* Configure PG7 */
//   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
// 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//     GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//     GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//   GPIO_Init(GPIOG, &GPIO_InitStructure);
// 	
// GPIO_ResetBits(GPIOG,GPIO_Pin_7);
// 	i = 100000;
// 	while(i--);
// 	GPIO_SetBits(GPIOG,GPIO_Pin_7); 
// i = 100000;
// 	while(i--);	
//   
//   /* MII/RMII Media interface selection --------------------------------------*/

//   SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_RMII);


// /* Ethernet pins configuration ************************************************/
//    /*
//         ETH_MDIO -------------------------> PA2
//         ETH_MDC --------------------------> PC1
//         ETH_PPS_OUT ----------------------> PB5
//         ETH_MII_CRS ----------------------> PH2
//         ETH_MII_COL ----------------------> PH3
//         ETH_MII_RX_ER --------------------> PI10
//         ETH_MII_RXD2 ---------------------> PH6
//         ETH_MII_RXD3 ---------------------> PH7
//         ETH_MII_TX_CLK -------------------> PC3
//         ETH_MII_TXD2 ---------------------> PC2
//         ETH_MII_TXD3 ---------------------> PB8
//         ETH_MII_RX_CLK/ETH_RMII_REF_CLK---> PA1
//         ETH_MII_RX_DV/ETH_RMII_CRS_DV ----> PA7
//         ETH_MII_RXD0/ETH_RMII_RXD0 -------> PC4
//         ETH_MII_RXD1/ETH_RMII_RXD1 -------> PC5
//         ETH_MII_TX_EN/ETH_RMII_TX_EN -----> PG11
//         ETH_MII_TXD0/ETH_RMII_TXD0 -------> PG13
//         ETH_MII_TXD1/ETH_RMII_TXD1 -------> PG14
//                                                   */

//   /* Configure PA1, PA2 and PA7 */
//   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_7;
//   GPIO_Init(GPIOA, &GPIO_InitStructure);
//   GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_ETH);
//   GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_ETH);
//   GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_ETH);

//   /* Configure PB5 and PB8 */
//   //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_8;
//   //GPIO_Init(GPIOB, &GPIO_InitStructure);
//   //GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_ETH);	
//   //GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_ETH);

//   /* Configure PC1, PC2, PC3, PC4 and PC5 */
//   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | /*GPIO_Pin_2 | GPIO_Pin_3 |*/ GPIO_Pin_4 | GPIO_Pin_5;
//   GPIO_Init(GPIOC, &GPIO_InitStructure);
//   GPIO_PinAFConfig(GPIOC, GPIO_PinSource1, GPIO_AF_ETH);
//   //GPIO_PinAFConfig(GPIOC, GPIO_PinSource2, GPIO_AF_ETH);
//   //GPIO_PinAFConfig(GPIOC, GPIO_PinSource3, GPIO_AF_ETH);
//   GPIO_PinAFConfig(GPIOC, GPIO_PinSource4, GPIO_AF_ETH);
//   GPIO_PinAFConfig(GPIOC, GPIO_PinSource5, GPIO_AF_ETH);
//                                 
//   /* Configure PG11, PG14 and PG13 */
//   GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_14;
//   GPIO_Init(GPIOG, &GPIO_InitStructure);
//   GPIO_PinAFConfig(GPIOG, GPIO_PinSource11, GPIO_AF_ETH);
//   GPIO_PinAFConfig(GPIOG, GPIO_PinSource13, GPIO_AF_ETH);
//   GPIO_PinAFConfig(GPIOG, GPIO_PinSource14, GPIO_AF_ETH);

//   
//   

//   /* Configure PI10 */
//   //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
//   //GPIO_Init(GPIOI, &GPIO_InitStructure);
//   //GPIO_PinAFConfig(GPIOI, GPIO_PinSource10, GPIO_AF_ETH);
}

/**
  * @brief  Configure the PHY to generate an interrupt on change of link status.
  * @param PHYAddress: external PHY address  
  * @retval None
  */
uint32_t Eth_Link_PHYITConfig(uint16_t PHYAddress)
{
  uint32_t tmpreg = 0;

  /* Read MICR register */
  tmpreg = ETH_ReadPHYRegister(PHYAddress, PHY_MICR);

  /* Enable output interrupt events to signal via the INT pin */
  tmpreg |= (uint32_t)PHY_MICR_INT_EN | PHY_MICR_INT_OE;
  if(!(ETH_WritePHYRegister(PHYAddress, PHY_MICR, tmpreg)))
  {
    /* Return ERROR in case of write timeout */
    return ETH_ERROR;
  }

  /* Read MISR register */
  tmpreg = ETH_ReadPHYRegister(PHYAddress, PHY_MISR);

  /* Enable Interrupt on change of link status */
  tmpreg |= (uint32_t)PHY_MISR_LINK_INT_EN;
  if(!(ETH_WritePHYRegister(PHYAddress, PHY_MISR, tmpreg)))
  {
    /* Return ERROR in case of write timeout */
    return ETH_ERROR;
  }
  /* Return SUCCESS */
  return ETH_SUCCESS;   
}

/**
  * @brief  EXTI configuration for Ethernet link status.
  * @param PHYAddress: external PHY address  
  * @retval None
  */
void Eth_Link_EXTIConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the INT (PB14) Clock */
  RCC_AHB1PeriphClockCmd(ETH_LINK_GPIO_CLK, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  /* Configure INT pin as input */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = ETH_LINK_PIN;
  GPIO_Init(ETH_LINK_GPIO_PORT, &GPIO_InitStructure);

  /* Connect EXTI Line to INT Pin */
  SYSCFG_EXTILineConfig(ETH_LINK_EXTI_PORT_SOURCE, ETH_LINK_EXTI_PIN_SOURCE);

  /* Configure EXTI line */
  EXTI_InitStructure.EXTI_Line = ETH_LINK_EXTI_LINE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set the EXTI interrupt to the highest priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  This function handles Ethernet link status.
  * @param  None
  * @retval None
  */
void Eth_Link_ITHandler(uint16_t PHYAddress)
{
  /* Check whether the link interrupt has occurred or not */
  if(((ETH_ReadPHYRegister(PHYAddress, PHY_MISR)) & PHY_LINK_STATUS) != 0)
  {
    EthLinkStatus = ~EthLinkStatus;

#ifdef USE_LCD
    /* Set the LCD Text Color */
    LCD_SetTextColor(Red);

    if(EthLinkStatus != 0)
    {
      /* Display message on the LCD */
      LCD_DisplayStringLine(Line5, (uint8_t*)"  Network Cable is  ");
      LCD_DisplayStringLine(Line6, (uint8_t*)"     unplugged      ");
    }
    else
    {
      /* Display message on the LCD */
      LCD_DisplayStringLine(Line5, (uint8_t*)"  Network Cable is  ");
      LCD_DisplayStringLine(Line6, (uint8_t*)"   now connected    ");
    }
#endif
  }
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
