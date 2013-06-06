 /**
  ******************************************************************************
  * @file    RTC/TimeStamp/main.c 
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    13-April-2012
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */
#include "stm32f4xx.h"
#include <stdio.h>
#include "RTC_Time.h"

/** @addtogroup STM32F4xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup RTC_TimeStamp
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Uncomment the corresponding line to select the RTC Clock source */
#define RTC_CLOCK_SOURCE_LSE   /* LSE used as RTC source clock */
/* #define RTC_CLOCK_SOURCE_LSI */ /* LSI used as RTC source clock. The RTC Clock
                                      may varies due to LSI frequency dispersion. */ 

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
USART_InitTypeDef USART_InitStructure;
RTC_InitTypeDef RTC_InitStructure;
RTC_TimeTypeDef RTC_TimeStructure;
RTC_DateTypeDef RTC_DateStructure;
RTC_TimeTypeDef  RTC_TimeStampStructure;
RTC_DateTypeDef  RTC_TimeStampDateStructure;
__IO uint32_t AsynchPrediv = 0, SynchPrediv = 0;
uint8_t flag = 0;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
void RTCTime_Test(void)
{
    uart_init();

    /* Output a message on Hyperterminal using printf function */
    printf("\n\r  *********************** RTC Time Stamp Example ***********************\n\r");
    
    if (RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x32F2)
    {
        /* RTC configuration  */
        RTC_Config();
        
        /* Configure the RTC data register and RTC prescaler */
        RTC_InitStructure.RTC_AsynchPrediv = AsynchPrediv;
        RTC_InitStructure.RTC_SynchPrediv = SynchPrediv;
        RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
        
        /* Check on RTC init */
        if (RTC_Init(&RTC_InitStructure) == ERROR)
        {
            printf("\n\r        /!\\***** RTC Prescaler Config failed ********/!\\ \n\r");
        }
        
        /* Configure the time register */
        RTC_TimeRegulate(); 
    }
    else
    {
        /* Check if the Power On Reset flag is set */
        if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
        {
          printf("\r\n Power On Reset occurred....\n\r");
        }
        /* Check if the Pin Reset flag is set */
        else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
        {
          printf("\r\n External Reset occurred....\n\r");
        }
        
        printf("\r\n No need to configure RTC....\n\r");
        
        /* Enable the PWR clock */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
        
        /* Allow access to RTC */
        PWR_BackupAccessCmd(ENABLE);
        
        /* Wait for RTC APB registers synchronisation */
        RTC_WaitForSynchro();
        
        /* Clear the RTC Alarm Flag */
        RTC_ClearFlag(RTC_FLAG_ALRAF);
        
        /* Clear the EXTI Line 17 Pending bit (Connected internally to RTC Alarm) */
        EXTI_ClearITPendingBit(EXTI_Line17);
        
        /* Display the RTC Time/Date and TimeStamp Time/Date */
        RTC_TimeShow();
        RTC_DateShow();
        RTC_TimeStampShow();
    }

    PUTOUT_KeyInit();
    
    while (1)
    {
    
    }
}

/**
  * @brief  Configure the RTC peripheral by selecting the clock source.
  * @param  None
  * @retval None
  */
void RTC_Config(void)
{
  /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Allow access to RTC */
  PWR_BackupAccessCmd(ENABLE);

#if defined (RTC_CLOCK_SOURCE_LSI)  /* LSI used as RTC source clock*/
/* The RTC Clock may varies due to LSI frequency dispersion. */
  /* Enable the LSI OSC */ 
  RCC_LSICmd(ENABLE);

  /* Wait till LSI is ready */  
  while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
  {
  }

  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

  SynchPrediv = 0xFF;
  AsynchPrediv = 0x7F;

#elif defined (RTC_CLOCK_SOURCE_LSE) /* LSE used as RTC source clock */
  /* Enable the LSE OSC */
  RCC_LSEConfig(RCC_LSE_ON);

  /* Wait till LSE is ready */  
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {
  }

  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

  SynchPrediv = 0xFF;
  AsynchPrediv = 0x7F;
    
#else
  #error Please select the RTC Clock source inside the main.c file
#endif /* RTC_CLOCK_SOURCE_LSI */

  /* Enable the RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC APB registers synchronisation */
  RTC_WaitForSynchro();

  /* Enable The TimeStamp */
  RTC_TimeStampCmd(RTC_TimeStampEdge_Falling, ENABLE);    
}

/**
  * @brief  Returns the time entered by user, using Hyperterminal.
  * @param  None
  * @retval None
  */
void RTC_TimeRegulate(void)
{
  uint32_t tmp_hh = 0xFF, tmp_mm = 0xFF, tmp_ss = 0xFF;

  printf("\n\r==============Time Settings=====================================\n\r");
  RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
  printf("  Please Set Hours\n\r");
  while (tmp_hh == 0xFF)
  {
    tmp_hh = USART_Scanf(0, 23);
    RTC_TimeStructure.RTC_Hours = tmp_hh;
  }
  printf(":  %0.2d\n\r", tmp_hh);
  
  printf("  Please Set Minutes\n\r");
  while (tmp_mm == 0xFF)
  {
    tmp_mm = USART_Scanf(0, 59);
    RTC_TimeStructure.RTC_Minutes = tmp_mm;
  }
  printf(":  %0.2d\n\r", tmp_mm);
  
  printf("  Please Set Seconds\n\r");
  while (tmp_ss == 0xFF)
  {
    tmp_ss = USART_Scanf(0, 59);
    RTC_TimeStructure.RTC_Seconds = tmp_ss;
  }
  printf(":  %0.2d\n\r", tmp_ss);

  /* Configure the RTC time register */
  if(RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure) == ERROR)
  {
    printf("\n\r>> !! RTC Set Time failed. !! <<\n\r");
  } 
  else
  {
    printf("\n\r>> !! RTC Set Time success. !! <<\n\r");
    RTC_TimeShow();
    /* Indicator for the RTC configuration */
    RTC_WriteBackupRegister(RTC_BKP_DR0, 0x32F2);
  }
  
  tmp_hh = 0xFF;
  tmp_mm = 0xFF;
  tmp_ss = 0xFF;

  printf("\n\r==============Date Settings=====================================\n\r");

  printf("  Please Set WeekDay (01-07)\n\r");
  while (tmp_hh == 0xFF)
  {
    tmp_hh = USART_Scanf(1, 7);
    RTC_DateStructure.RTC_WeekDay = tmp_hh;
  }
  printf(":  %0.2d\n\r", tmp_hh);
  tmp_hh = 0xFF;
  printf("  Please Set Date (01-31)\n\r");
  while (tmp_hh == 0xFF)
  {
    tmp_hh = USART_Scanf(1, 31);
    RTC_DateStructure.RTC_Date = tmp_hh;
  }
  printf(":  %0.2d\n\r", tmp_hh);
  
  printf("  Please Set Month (01-12)\n\r");
  while (tmp_mm == 0xFF)
  {
    tmp_mm = USART_Scanf(1, 12);
    RTC_DateStructure.RTC_Month = tmp_mm;
  }
  printf(":  %0.2d\n\r", tmp_mm);
  
  printf("  Please Set Year (00-99)\n\r");
  while (tmp_ss == 0xFF)
  {
    tmp_ss = USART_Scanf(0, 99);
    RTC_DateStructure.RTC_Year = tmp_ss;
  }
  printf(":  %0.2d\n\r", tmp_ss);

  /* Configure the RTC date register */
  if(RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure) == ERROR)
  {
    printf("\n\r>> !! RTC Set Date failed. !! <<\n\r");
  } 
  else
  {
    printf("\n\r>> !! RTC Set Date success. !! <<\n\r");
    RTC_DateShow();
    /* Indicator for the RTC configuration */
    RTC_WriteBackupRegister(RTC_BKP_DR0, 0x32F2);
  }

}

/**
  * @brief  Display the current time on the Hyperterminal.
  * @param  None
  * @retval None
  */
void RTC_TimeShow(void)
{
  /* Get the current Time and Date */
  RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
  printf("\n\r============== Current Time Display ============================\n\r");
  printf("\n\r  The current time (Hour-Minute-Second) is :  %0.2d:%0.2d:%0.2d \n\r", RTC_TimeStructure.RTC_Hours, RTC_TimeStructure.RTC_Minutes, RTC_TimeStructure.RTC_Seconds);
  /* Unfreeze the RTC DR Register */
  (void)RTC->DR;
}

/**
  * @brief  Display the current date on the Hyperterminal.
  * @param  None
  * @retval None
  */
void RTC_DateShow(void)
{
  /* Get the current Date */
  RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
  printf("\n\r============== Current Date Display ============================\n\r");
  printf("\n\r  The current date (WeekDay-Date-Month-Year) is :  %0.2d-%0.2d-%0.2d-%0.2d \n\r", RTC_DateStructure.RTC_WeekDay, RTC_DateStructure.RTC_Date, RTC_DateStructure.RTC_Month, RTC_DateStructure.RTC_Year);
}


/**
  * @brief  Display the current TimeStamp (time and date) on the Hyperterminal.
  * @param  None
  * @retval None
  */
void RTC_TimeStampShow(void)
{
  /* Get the current TimeStamp */
  RTC_GetTimeStamp(RTC_Format_BIN, &RTC_TimeStampStructure, &RTC_TimeStampDateStructure);
  printf("\n\r==============TimeStamp Display (Time and Date)=================\n\r");
  printf("\n\r  The current time stamp time (Hour-Minute-Second) is :  %0.2d:%0.2d:%0.2d \n\r", RTC_TimeStampStructure.RTC_Hours, RTC_TimeStampStructure.RTC_Minutes, RTC_TimeStampStructure.RTC_Seconds);
  printf("\n\r  The current timestamp date (WeekDay-Date-Month) is :  %0.2d-%0.2d-%0.2d \n\r", RTC_TimeStampDateStructure.RTC_WeekDay, RTC_TimeStampDateStructure.RTC_Date, RTC_TimeStampDateStructure.RTC_Month);
}


/**
  * @brief  Gets numeric values from the hyperterminal.
  * @param  MinValue: minimum value to be used.
  * @param  MaxValue: maximum value to be used.
  * @retval None
  */

uint32_t tmp[2] = {0, 0};
uint8_t USART_Scanf(uint32_t MinValue, uint32_t MaxValue)
{
  uint32_t index = 0;

  while (index < 2)
  {
    /* Loop until RXNE = 1 */
    while (USART_GetFlagStatus(USART6, USART_FLAG_RXNE) == RESET)
    {}  
    tmp[index++] = (USART_ReceiveData(USART6));
    USART_ClearFlag(USART6,USART_FLAG_RXNE);

    if ((tmp[index - 1] < 0x30) || (tmp[index - 1] > 0x39))
    {
      printf("\n\r Please enter valid number between 0 and 9\n\r");
      index--;
    }

  }
  /* Calculate the Corresponding value */
  index = (tmp[1] - 0x30) + ((tmp[0] - 0x30) * 10);
  /* Checks the value */
  if ((index < MinValue) || (index > MaxValue))
  {
    printf("\n\r Please enter valid number between %d and %d\n\r", MinValue, MaxValue);
    return 0xFF;
  }
  return index;
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
int fputc(int ch, FILE *f)
{
	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART */
	USART_SendData(USART6, (uint8_t) ch);

	/* Loop until the end of transmission */
	while (USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET)
	{}

	return ch;
}
void uart_init(void)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* 开启GPIO_D的时钟 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	/* 开启串口3的时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);

	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14;
	GPIO_Init(GPIOG, &GPIO_InitStructure);

	
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource9, GPIO_AF_USART6);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource14, GPIO_AF_USART6);


	USART_InitStructure.USART_BaudRate   = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits   = USART_StopBits_1;
	USART_InitStructure.USART_Parity     = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode                = USART_Mode_Tx | USART_Mode_Rx;

	USART_Init(USART6, &USART_InitStructure);

	/* 使能串口 */
	USART_Cmd(USART6, ENABLE);
}

void PUTOUT_KeyInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOF , ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOF, EXTI_PinSource6);

    EXTI_InitStructure.EXTI_Line = EXTI_Line6;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

   /* Enable and set Button EXTI Interrupt to the lowest priority */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);

    EXTI_GenerateSWInterrupt(EXTI_Line6); 
}

