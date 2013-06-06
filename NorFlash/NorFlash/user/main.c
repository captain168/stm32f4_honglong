/**
  ******************************************************************************
  * @file    main.c 
  * @author  Wangbao Dev Team
  * @version V1.0.0
  * @date    07-10-2012
  * @brief   Main program body
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


#include "stm32f4xx.h"
#include "fsmc_nor.h"
#include "main.h"
#include "uart.h"
#include "lcd.h"


 



#define BUFFER_SIZE        0x400
#define WRITE_READ_ADDR    0x8000


ErrorStatus HSEStartUpStatus;
static vu32 TimingDelay = 0;
u16 TxBuffer[BUFFER_SIZE];
u16 RxBuffer[BUFFER_SIZE];
u32 WriteReadStatus = 0, Index = 0;
NOR_IDTypeDef NOR_ID;

void nvic_config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	/* NVIC configuration */
	/* Configure the Priority Group to 2 bits */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	/* Enable the USARTx Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}





void delay(uint32_t dl)
{
	uint32_t i;
	while(dl)
	{
		i = 0;
		dl--;
		while(i < 1000)
		{
			i++;
		}
	}	
}

void led_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void led_all_off(void)
{
	GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_SET);
	GPIO_WriteBit(GPIOA,GPIO_Pin_6,Bit_SET);
	GPIO_WriteBit(GPIOB,GPIO_Pin_6,Bit_SET);
	GPIO_WriteBit(GPIOC,GPIO_Pin_6,Bit_SET);
}

void led_on(unsigned char num)
{
	switch(num)
	{
	case 0 :
		led_all_off();
		break;
	case 1:
		GPIO_WriteBit(GPIOA,GPIO_Pin_6,Bit_RESET);
		GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_SET);
		GPIO_WriteBit(GPIOB,GPIO_Pin_6,Bit_SET);
		GPIO_WriteBit(GPIOC,GPIO_Pin_6,Bit_SET);
		break;
	case 2:
		GPIO_WriteBit(GPIOA,GPIO_Pin_6,Bit_SET);
		GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_RESET);
		GPIO_WriteBit(GPIOB,GPIO_Pin_6,Bit_SET);
		GPIO_WriteBit(GPIOC,GPIO_Pin_6,Bit_SET);
		break;
	case 3:
		GPIO_WriteBit(GPIOA,GPIO_Pin_6,Bit_SET);
		GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_SET);
		GPIO_WriteBit(GPIOB,GPIO_Pin_6,Bit_RESET);
		GPIO_WriteBit(GPIOC,GPIO_Pin_6,Bit_SET);
		break;
	case 4:
		GPIO_WriteBit(GPIOA,GPIO_Pin_6,Bit_SET);
		GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_SET);
		GPIO_WriteBit(GPIOB,GPIO_Pin_6,Bit_SET);
		GPIO_WriteBit(GPIOC,GPIO_Pin_6,Bit_RESET);
		break;
	default:
		break;

	}
}
void Fill_Buffer(u16 *pBuffer, u16 BufferLenght, u32 Offset)
{
	u16 IndexTmp = 0;

	/* Put in global buffer same values */
	for (IndexTmp = 0; IndexTmp < BufferLenght; IndexTmp++ )
	{
		pBuffer[IndexTmp] = IndexTmp + Offset;
	}
}



/**
  * @brief   Main program
  * @param  None
  * @retval None
  */
int main(void)
{
	nvic_config();
	led_init();
	led_all_off();
	//SysTick_Config(720000);
	/* Configure FSMC Bank1 NOR/SRAM2 */
	FSMC_NOR_Init();

	FSMC_NOR_ReturnToReadMode();
	
	


	/* Erase the NOR memory block to write on */
	FSMC_NOR_EraseBlock(WRITE_READ_ADDR);

	/* Write data to FSMC NOR memory */
	/* Fill the buffer to send */
	Fill_Buffer(TxBuffer, BUFFER_SIZE, 0x3210);
	
	FSMC_NOR_WriteBuffer(TxBuffer, WRITE_READ_ADDR, BUFFER_SIZE);

	
	/* Read data from FSMC NOR memory */
	FSMC_NOR_ReadBuffer(RxBuffer, WRITE_READ_ADDR, BUFFER_SIZE);  

	/* Read back NOR memory and check content correctness */   
	for (Index = 0x00; (Index < BUFFER_SIZE) && (WriteReadStatus == 0); Index++)
	{
		if (RxBuffer[Index] != TxBuffer[Index])
		{
			WriteReadStatus = Index + 1;
		}
	}	

	if (WriteReadStatus == 0)
	{	/* OK */
		/* Turn on LD1 */		
		led_on(1);
		while (1)
		{
		}
	}
	else
	{ /* KO */
		/* Turn on LD2 */		
		led_on(2); 
		while (1)
		{
		}
	}


}


void Decrement_TimingDelay(void)
{
	if (TimingDelay != 0x00)
	{
		TimingDelay--;
	}
}



#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
