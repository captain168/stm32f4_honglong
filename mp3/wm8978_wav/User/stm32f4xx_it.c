/**
  ******************************************************************************
  * @file    ADC/DualADC_RegulSimu_DMAmode1/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    30-September-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
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
#include "stm32f4xx_it.h"
#include "i2s.h"

/** @addtogroup STM32F4xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup ADC_DualADC_RegulSimu_DMAmode1
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/
#include "sdio_sd.h"
SD_Error SD_ProcessIRQSrc(void);
void SDIO_IRQHandler(void)
{
 	SD_ProcessIRQSrc();
}

extern u8 buf[512],buf2[512];
extern u16 wav_count,wav_count2;
extern u8 select_buf;
unsigned short *addr;
void SPI3_IRQHandler(void)
{
  /* Check on the I2S TXE flag */  
  if (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) != RESET)
  { 
  	if(select_buf==2){
		if(0!=(I2Sx->SR&0x4)){
			SPI_I2S_SendData(I2Sx,(addr[wav_count+1]));wav_count=wav_count+2;}//右声道
		else{
			SPI_I2S_SendData(I2Sx, addr[wav_count]);}	 //左声道
	}
  	else if(select_buf==1){ 
		if(0!=(I2Sx->SR&0x4)){
			SPI_I2S_SendData(I2Sx,(buf2[wav_count+3]<<8)+buf2[wav_count+2]);wav_count=wav_count+4;}//右声道
		else{
			SPI_I2S_SendData(I2Sx, (buf2[wav_count+1])+buf2[wav_count]);}	 //左声道
	}
	else{
	 	if(0!=(I2Sx->SR&0x4)){
			SPI_I2S_SendData(I2Sx,(buf[wav_count2+3]<<8)+buf[wav_count2+2]);wav_count2=wav_count2+4;}//右声道
		else{
			SPI_I2S_SendData(I2Sx, (buf[wav_count2+1])+buf[wav_count2]);}	 //左声道	
	}
  }
}


void DMA1_Stream5_IRQHandler(void)
{    
  	//传送完成中断
  	if (DMA_GetFlagStatus(DMA1_Stream5, DMA_FLAG_TCIF5)!=RESET)
  	{         

      	DMA_ClearFlag(DMA1_Stream5, DMA_FLAG_TCIF5);       
  	}  
}

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
