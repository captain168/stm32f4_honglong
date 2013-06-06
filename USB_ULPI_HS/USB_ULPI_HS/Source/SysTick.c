
/*********************************************************/
#include "SysTick.h"
	 
static uint32_t  My_us=168;//us延时倍乘数
static uint32_t  My_ms=168000;//ms延时倍乘数
/********************************************************
*函数名：void delay_init(uint32_t SYSCLK)
*参  数：SYSCLK:系统时钟  单位 M  如168MHz 填 168
*功  能：初始化 systick定时器
*********************************************************/
void delay_init(uint32_t SYSCLK)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	My_us=SYSCLK/8;		    
	My_ms=(uint16_t)My_us*1000;
}								    
/********************************************************
*函数名：void delay_ms(u16 nms)
*参  数：nms 延时时间 n ms
*功  能：ms延时 注意范围不要超过  nms<=0xffffff*8*1000/SYSCLK
*********************************************************/
void delay_ms(uint16_t nms)
{	 		  	  
	uint32_t  temp;		   
	SysTick->LOAD=(u32)nms*My_ms;
	SysTick->VAL =0x00;           
	SysTick->CTRL=0x01 ;          
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));   
	SysTick->CTRL=0x00;               
	SysTick->VAL =0X00;               	  	    
}   
/********************************************************
*函数名：void delay_us(u32 nus)
*参  数：nus 延时时间 n us
*功  能：us延时
*********************************************************/	    								   
void delay_us(uint32_t nus)
{		
	uint32_t temp;	    	 
	SysTick->LOAD=nus*My_us;   		 
	SysTick->VAL=0x00;        
	SysTick->CTRL=0x01 ;      	 
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));  
	SysTick->CTRL=0x00;               
	SysTick->VAL =0X00;                
}







