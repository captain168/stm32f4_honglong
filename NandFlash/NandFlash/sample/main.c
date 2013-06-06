
#include <stm32f4xx.h>
#include "uart.h"
#include "fsmc_nand.h"

 
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert error line source number
* Output         : None
* Return         : None
*******************************************************************************/


void assert_failed(u8* file, u32 line)
{
 
    debug("fail");
	while (1) ;
}

 
int main(void)
{
    uart_init();
    debug("hello wb-F4!\n\r");
    FSMC_NAND_Test();
		
	while(1)
	{}
	
    return 0;
}


