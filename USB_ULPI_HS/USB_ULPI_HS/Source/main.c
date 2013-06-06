
#include "main.h"

/********************************************************************/
uint8_t Buffer[64];
uint16_t *Data=0;
//--------------------------------------------------------------------
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USB_OTG_CORE_HANDLE     USB_OTG_dev __ALIGN_END ;
/********************************************************************/
void Delay(__IO uint32_t nCount);
/********************************************************************/
int main()
  {
		UART_Init();
		
		delay_init(168);
		printf("Welcome...\r\n");
    
		
		
	  DiskInit();		
	  printf("BaudRate = 115200 baud\r\n");
		printf("USB Init Start!\r\n");
  USBD_Init(&USB_OTG_dev,
            USB_OTG_HS_CORE_ID,     
            &USR_desc,
            &USBD_MSC_cb, 
            &USR_cb);
	 while(1)
	   {
     }
}
/********************************************************************/
void Delay(__IO uint32_t nCount)
{
  while(nCount--)
  {
  }
}
/********************************************************************/

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

//------------------- The end ---------------------------------

