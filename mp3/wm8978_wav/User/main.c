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
#include "SysTick.h"
#include "lcd.h"
#include "fm_radio.h"

#include "bsp_wm8978.h"

/*SD 卡需要的头文件*/
#include "sdio_sd.h"
#include "ff.h"
#include "ffconf.h"

#include "mad.h"

#include <string.h>

extern unsigned char WAV_NUM;			/*保存wav文件的数目*/ 
extern char WAV_Files[100][40];			/*最多100个文件,*/
void Find_FileName(void);
void WAVE_Test(void);
int MP3_Test(int argc, char *argv[]);

static void InitBoard(void);
void wm8978_I2S_Init(void);
int main(void)
 	{	
	NVIC_InitTypeDef NVIC_InitStructure;
	/* Configure the NVIC Preemption Priority Bits */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
#if defined (SD_DMA_MODE)
	NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif
    InitBoard();	/* 为了是main函数看起来更简洁些，我们将初始化的代码封装到这个函数 */
	//查找在主目录可播放的wav文件
	Find_FileName();
	printf("dsfsadfsd\r\nfsadfsdaf\r\n");
	while(1)
	{//	WAVE_Test();
		MP3_Test(WAV_NUM,(char**)WAV_Files);
//		WAVE_Test();
//    	DispLogo();		/* 显示例程Logo */
    
//    	RadioMain();	/* FM收音机主程序 */
	}
}

/*******************************************************************************
	函数名：GPIO_Configuration
	输  入:
	输  出:
	功能说明：配置7个按键为输入口线，4个LED为输出口线

	按键口线分配：
	USER键     : PG8  (低电平表示按下)
	TAMPEER键  : PC13 (低电平表示按下)
	WKUP键     : PA0  (!!!高电平表示按下)
	摇杆UP键   : PG15 (低电平表示按下)
	摇杆DOWN键 : PD3  (低电平表示按下)
	摇杆LEFT键 : PG14 (低电平表示按下)
	摇杆RIGHT键: PG13 (低电平表示按下)
	摇杆SELECT键: PG7 (低电平表示按下)

	LED口线分配：
	LED1 : PF6  (输出0点亮)
	LED2 : PF7  (输出0点亮)
	LED3 : PF8  (输出0点亮)
	LED4 : PF9  (输出0点亮)

*/
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* 第1步：打开GPIOA GPIOC GPIOD GPIOF GPIOG的时钟
	   注意：这个地方可以一次性全打开
	*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA |RCC_AHB1Periph_GPIOB 
		| RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOF,ENABLE);

	/* 第2步：配置所有的按键GPIO为浮动输入模式(实际上CPUf复位后就是输入状态) */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

	/* 第3步：配置所有的LED指示灯GPIO为推挽输出模式 */
	/* 由于将GPIO设置为输出时，GPIO输出寄存器的值缺省是0，因此会驱动LED点亮
		这是我不希望的，因此在改变GPIO为输出前，先修改输出寄存器的值为1 */
//	GPIO_SetBits(GPIOA,  GPIO_Pin_2 | GPIO_Pin_7 | GPIO_Pin_3 | GPIO_Pin_4);
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_7 | GPIO_Pin_3 | GPIO_Pin_4;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOD, &GPIO_InitStructure);
}

/*******************************************************************************
	函数名：InitBoard
	输  入:
	输  出:
	功能说明：初始化硬件设备
*/
void InitButtonVar(void);
void uart3_init(void);
void wm8978_I2S_Init(void);
static void InitBoard(void)
{
	SD_Error Status;

	 uart_init();
	/*
		这个函数是ST库中的函数，函数实体在
		Libraries\CMSIS\Core\CM3\system_stm32f10x.c
    */
	/* 配置按键GPIO和LED GPIO */
	//GPIO_Configuration();

	/* 在 SysTick_Config()前，必须先调用 */
//	InitButtonVar();

	/* 配置systic作为1ms中断,这个函数在
	\Libraries\CMSIS\Core\CM3\core_cm3.h */
//    SysTick_Init();

	//SD 卡
	do{
		Status = SD_Init();	                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     
	}while(Status!= SD_OK);

	//WM8978
	if (i2c_CheckDevice(WM8978_SLAVE_ADDRESS) == 0)
	{
		wm8978_Cfg(DAC_OFF|1, AUX_ON&0, LINE_OFF|0, SPK_OFF|1, EAR_ON);

		wm8978_ChangeVolume(0x30,0x30);
	}

//	while(1);
	wm8978_I2S_Init();
}
/*********************************************************************************************************
** Functoin name:       WAV_Files
** Descriptions:        查找WAV文件,文件存放于WAV_Files
** input paraments:     无
** output paraments:    无    
** Returned values:     无
*********************************************************************************************************/

unsigned char WAV_NUM=0;			/*保存wav文件的数目*/ 
char WAV_Files[100][40];			/*最多100个文件,*/
void Find_FileName()
{
	DIR dirs;
	FILINFO finfo;
	FATFS fs;

	const XCHAR path[]="";
	unsigned short i;
	FRESULT res=0;

/*长文件名支持*/	
#if _USE_LFN
	static char lfn[_MAX_LFN * (_DF1S ? 2 : 1) + 1];
	finfo.lfname = lfn;
	finfo.lfsize = sizeof(lfn);
#endif


	/*挂载文件系统*/
	res = f_mount(0, &fs);
	res=f_opendir(&dirs, path);

	if (res == FR_OK) {								/*如果打开主目录成功*/
		
		while (f_readdir(&dirs, &finfo) == FR_OK){	/*通过dirs，依次阅读文件名，知道末尾*/
			
			   	
			if(finfo.fattrib & AM_DIR){	 			/*是目录就结束本次循环*/
				continue;	
			}
			if (finfo.fattrib & AM_ARC) { 			/*刚打开的文件属性是存档文件,*/
				
				if(!finfo.fname[0])	 break; 		/*文件尾，跳出while*/		
				if(finfo.lfname[0]){				/*长文件名	*/
					i = (unsigned char )strlen(finfo.lfname);/*求文件名的长度，求出之后得到后缀*/

					if(((finfo.lfname[i-3]=='w') && (finfo.lfname[i-2]=='a') && (finfo.lfname[i-1]=='v'))\
						||((finfo.lfname[i-3]=='W') && (finfo.lfname[i-2]=='A') && (finfo.lfname[i-1]=='V'))){
						strcpy(WAV_Files[WAV_NUM],(const char *)finfo.lfname);
						WAV_NUM++;
					}
			
				}
				else{								/*短文件名*/
					i = (unsigned char )strlen(finfo.fname);/*求文件名的长度，求出之后得到后缀*/

					if(((finfo.fname[i-3]=='w') && (finfo.fname[i-2]=='a') && (finfo.fname[i-1]=='v'))\
						||((finfo.fname[i-3]=='W') && (finfo.fname[i-2]=='A') && (finfo.fname[i-1]=='V'))){
						strcpy(WAV_Files[WAV_NUM],(const char *)finfo.fname);
						WAV_NUM++;
					}	
				}
			}
		}	
	}
}

