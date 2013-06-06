/****************************************************************************
*	操作提示：
*		[TAMPER]键     = 扬声器/耳机
*		[WAKEUP]键     = ADC电平切换
*		[USER]键       = 静音打开/静音关闭
*		摇杆上/下键    = 调节音量
*		摇杆左/右键    = 向下搜台/向上搜台
*		摇杆OK键       = 自动搜台/手动搜
*
* 文件历史:
* 版本号  日期       作者    说明
* v0.1    2010-09-10 armfly  创建该文件
*
*/

#include "stm32f4xx.h"
#include <stdio.h>
//#include "systick.h"

#include "button.h"

//#include "tft_lcd.h"
#include "bsp_TEA5767.h"
#include "bsp_WM8978.h"
#include "fm_radio.h"

#define LINE_CAP	18		/* 定义行间距 */

#define STR_Title	"FM调频收音机(TEA5767 + WM8978)"
#define STR_Help1	"[TAMPER]键     = 扬声器/耳机"
#define STR_Help2	"[WAKEUP]键     = 自动搜台ADC电平"
#define STR_Help3	"[USER]键       = 静音打开/静音关闭"
#define STR_Help4	"摇杆上/下键    = 调节音量"
#define STR_Help5	"摇杆左/右键    = 向下搜台/向上搜台"
#define STR_Help6	"摇杆OK键       = 自动搜台/手动搜台"

#define MII_MODE /* Mode MII with STM324xG-EVAL  */
#define PHY_CLOCK_MCO

//#define RMII_MODE

RADIO_T g_tRadio;
TEA5767_T g_tTEA;

static void DispTitle(void);
static void DispHelp(void);
static void DispStatus(void);
static void ReadStatus(void);

static void Wm8978_SysInit(void);

/*******************************************************************************
*	函 数 名：RadioMain
*	功能说明：收音机主程序
*	形    参：无
*	返 回 值: 无
*/
void RadioMain(void)
{
	uint8_t ucKeyCode;	/* 按键代码 */
	uint8_t ucRefresh;	/* 刷新LCD请求 */
	
	DispTitle();		/* 显示标题 */

	//i2c_Configuration();	/* 配置I2C的GPIO */
	//Wm8978_SysInit();

	#if  1	/* 下面这段代码可以检测2个芯片 */
		if (i2c_CheckDevice(TEA5767_SLAVE_ADDRESS) != 0)       
		{
			////LCD_DisplayString(20, LINE_CAP, "Don't Find TEA5767");
			//printf("Don't Find TEA5767\r\n");
            //while(1);
		}
		
		if (i2c_CheckDevice(WM8978_SLAVE_ADDRESS) != 0)
		{
			////LCD_DisplayString(20, LINE_CAP, "Don't Find WM8978");
			//printf("Don't Find WM8978\r\n");
            //while(1);
		}
	#endif
	
	DispHelp();		/* 显示帮助信息 */

	/* 初始化全局变量 */
	g_tRadio.ucSpkEn = 0;		/* 扬声器输出 */
	g_tRadio.ucMuteOn = 0;		/* 静音关闭 */
	g_tRadio.ucAutoMode = 1;	/* 自动搜台模式使能 */
	g_tRadio.ucVolume = 30;		/* 缺省音量 */
	g_tRadio.ulFreq = FM_FREQ_MIN;	/* 美国/欧洲(87.5-108M), 日本(76-91MHz) */
	g_tRadio.ucAdcLevelSet = ADC_LEVEL_7;

	/* 配置WM8978芯片，耳机输出FM信号 */
	wm8978_Aux2Ear();
	
	wm8978_ChangeVolume(g_tRadio.ucVolume, g_tRadio.ucVolume);	/* 调节音量，左右相同音量 */

	tea5767_Set(g_tRadio.ulFreq, MUTE_OFF, SEARCH_ON, SEARCH_UP, ADC_LEVEL_7);
	
	ucRefresh = 1;

	/* 进入主程序循环体 */
	while (1)
	{	g_tRadio.ucAutoMode  = 1;
//		CPU_IDLE();

		/* 搜索时，实时读取TEA5767的状态，如果搜到电台或抵达频率极限，则停止 */
		if (g_tRadio.ucSearch == 1)
		{
			ReadStatus();	/* 读取TEA5767的状态 */
			if ((g_tTEA.ucReady == 1) || (g_tTEA.ucBandLimit == 1))
			{
				g_tRadio.ucSearch = 0;
				g_tRadio.ucMuteOn = 0;
				wm8978_Mute(0);			/* 取消静音 */				
				ucRefresh = 1;
			}
		}
		
		/* 刷新状态栏 */
		if (ucRefresh == 1)
		{
			ucRefresh = 0;
			ReadStatus();		/* 读取TEA5767的状态 */
			DispStatus();		/* 显示当前状态，频率，音量等 */
		}
		
		/* 处理按键事件 */
		ucKeyCode = GetKey();
		if (ucKeyCode > 0)
		{
			/* 有键按下 */
			switch (ucKeyCode)
			{
				case KEY_DOWN_TAMPER:		/* TAMPER键按下 */
					if (g_tRadio.ucSpkEn == 1)
					{
						g_tRadio.ucSpkEn = 0;
						wm8978_Aux2Ear();	/* 配置WM8978芯片，耳机输出FM信号 */
					}
					else
					{
						g_tRadio.ucSpkEn = 1;
						wm8978_Aux2Spk();	/* 配置WM8978芯片，扬声器输出FM信号 */
					}
					ucRefresh = 1;	
					break;

				case KEY_DOWN_WAKEUP:		/* WAKEUP键按下 */
					g_tRadio.ucAdcLevelSet++;
					if (g_tRadio.ucAdcLevelSet > 3)
					{
						g_tRadio.ucAdcLevelSet = 1;
					}
					ucRefresh = 1;					
					break;

				case KEY_DOWN_USER:			/* USER键按下 */
					if (g_tRadio.ucMuteOn == 1)
					{
						g_tRadio.ucMuteOn = 0;
						wm8978_Mute(0);		/* 不静音 */
					}
					else
					{
						g_tRadio.ucMuteOn = 1;
						wm8978_Mute(1);		/* 静音 */
					}
					ucRefresh = 1;					
					break;
					
				case KEY_DOWN_JOY_UP:		/* 摇杆UP键按下 */
					if (g_tRadio.ucVolume <= VOLUME_MAX - VOLUME_STEP)
					{
						g_tRadio.ucVolume += VOLUME_STEP;
						wm8978_ChangeVolume(g_tRadio.ucVolume, g_tRadio.ucVolume);
						ucRefresh = 1;
					}
					break;

				case KEY_DOWN_JOY_DOWN:		/* 摇杆DOWN键按下 */
					if (g_tRadio.ucVolume >= VOLUME_STEP)
					{
						g_tRadio.ucVolume -= VOLUME_STEP;
						wm8978_ChangeVolume(g_tRadio.ucVolume, g_tRadio.ucVolume);
						ucRefresh = 1;
					}					
					break;

				case KEY_DOWN_JOY_LEFT:		/* 摇杆LEFT键按下 */
					g_tRadio.ulFreq -= FREQ_STEP;
					if (g_tRadio.ulFreq < FM_FREQ_MIN)
					{
						g_tRadio.ulFreq = FM_FREQ_MAX;
					}
					if (g_tRadio.ucAutoMode == 1)
					{
						tea5767_Set(g_tRadio.ulFreq, MUTE_OFF, SEARCH_ON, SEARCH_DOWN, g_tRadio.ucAdcLevelSet);
						g_tRadio.ucSearch = 1;
						g_tRadio.ucMuteOn = 1;
						g_tRadio.ucDirection = SEARCH_DOWN;
						wm8978_Mute(1);		/* 静音 */
					}
					else
					{
						tea5767_Set(g_tRadio.ulFreq, MUTE_OFF, SEARCH_OFF, SEARCH_DOWN, g_tRadio.ucAdcLevelSet);							
					}
					ucRefresh = 1;
					break;

				case KEY_DOWN_JOY_RIGHT:	/* 摇杆RIGHT键按下 */
					g_tRadio.ulFreq += FREQ_STEP;
					if (g_tRadio.ulFreq > FM_FREQ_MAX)
					{
						g_tRadio.ulFreq = FM_FREQ_MIN;
					}
					if (g_tRadio.ucAutoMode == 1)
					{
						tea5767_Set(g_tRadio.ulFreq, MUTE_OFF, SEARCH_ON, SEARCH_UP, g_tRadio.ucAdcLevelSet);
						g_tRadio.ucSearch = 1;
						g_tRadio.ucMuteOn = 1;
						g_tRadio.ucDirection = SEARCH_UP;
						wm8978_Mute(1);		/* 静音 */						
					}
					else
					{
						tea5767_Set(g_tRadio.ulFreq, MUTE_OFF, SEARCH_OFF, SEARCH_UP, g_tRadio.ucAdcLevelSet);							
					}						
					ucRefresh = 1;
					break;

				case KEY_DOWN_JOY_OK:		/* 摇杆OK键按下 */
					if (g_tRadio.ucAutoMode == 1)
					{
						g_tRadio.ucAutoMode = 0;
					}
					else
					{
						g_tRadio.ucAutoMode = 1;
					}
					ucRefresh = 1;	
					break;

				default:
					break;
			}
		}
	}
}

/*******************************************************************************
*	函 数 名：DispTitle
*	功能说明：显示标题行
*	形    参：无
*	返 回 值: 无
*/
static void DispTitle(void)
{
//	//LCD_Clear(Blue);  			/* 清屏，背景蓝色 */
//	//LCD_SetBackColor(Blue);		/* 设置文字背景颜色 */
//	//LCD_SetTextColor(White);	/* 设置文字颜色 */
//	
//	////LCD_DisplayString(60, 0, STR_Title);
}

/*******************************************************************************
*	函 数 名：DispHelp
*	功能说明：显示帮助信息
*	形    参：无
*	返 回 值: 无
*/
static void DispHelp(void)
{
//	uint16_t y;
//
//	//LCD_SetBackColor(Blue);		/* 设置文字背景颜色 */
//	//LCD_SetTextColor(White);	/* 设置文字颜色 */
//	
//	y = LINE_CAP * 2; 	/* 行间距(单位：像素) */
//	
//	////LCD_DisplayString(20, y, STR_Help1);
//	y += LINE_CAP;
//
//	////LCD_DisplayString(20, y, STR_Help2);
//	y += LINE_CAP;	
//
//	////LCD_DisplayString(20, y, STR_Help3);
//	y += LINE_CAP;
//
//	////LCD_DisplayString(20, y, STR_Help4);
//	y += LINE_CAP;
//
//	////LCD_DisplayString(20, y, STR_Help5);
//	y += LINE_CAP;		
//
//	////LCD_DisplayString(20, y, STR_Help6);
//	y += LINE_CAP;
}
//static void Wm8978_SysInit(void)
//{
//    GPIO_InitTypeDef GPIO_InitStructure;
//    
//    /* Enable GPIOs clocks */
//    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA , ENABLE);
//    
//    /* Enable SYSCFG clock */
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);  
//    
//    /* Configure MCO (PA8) */
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;  
//    GPIO_Init(GPIOA, &GPIO_InitStructure);
//    
//    /* MII/RMII Media interface selection --------------------------------------*/
//#ifdef MII_MODE /* Mode MII with STM324xG-EVAL  */
//    #ifdef PHY_CLOCK_MCO
//    /* Output HSE clock (25MHz) on MCO pin (PA8) to clock the PHY */
//    RCC_MCO1Config(RCC_MCO1Source_HSE, RCC_MCO1Div_2);
//    #endif /* PHY_CLOCK_MCO */
//    SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_MII);
//#elif defined RMII_MODE  /* Mode RMII with STM324xG-EVAL */
//    SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_RMII);
//#endif
//}
/*******************************************************************************
*	函 数 名：ReadStatus
*	功能说明：读取TEA5767的状态
*	形    参：无
*	返 回 值: 无
*/
static void ReadStatus(void)
{
	tea5767_ReadStatus(&g_tTEA);	/* 读取TEA5767的状态 */
	g_tRadio.ulFreq = g_tTEA.ulFreq;
	g_tRadio.ucAdcLevelNow = g_tTEA.ucAdcLevel;
}

/*******************************************************************************
*	函 数 名：DispStatus
*	功能说明：显示当前状态，频率，音量等
*	形    参：无
*	返 回 值: 无
*/
static void DispStatus(void)
{
	uint16_t y, x1, x2;
	uint8_t buf[80];
	uint8_t ucTemp;

	//LCD_SetTextColor(Yellow);	/* 设置文字颜色 */
	
	/* 画一个矩形框 */
	//LCD_DrawRect(10, LINE_CAP * 9 + 12, 63, 400 - 20);

	x1 = 20;	/* 状态栏第1列X坐标 */
	x2 = 200;	/* 状态栏第2列X坐标 */	
	y = LINE_CAP * 10;
	
	if (g_tRadio.ucSearch == 1)
	{
		if (g_tRadio.ucDirection == SEARCH_UP)
		{
			sprintf((char *)buf, "频率 = >>>>>>>>    ");
		}
		else
		{
			sprintf((char *)buf, "频率 = <<<<<<<<    ");
		}
	}
	else
	{
		sprintf((char *)buf, "频率 = %d.%d兆赫    ",
			g_tRadio.ulFreq / 1000000, (g_tRadio.ulFreq % 1000000) / 100000);		
	}
	////LCD_DisplayString(x1, y, buf);
	
	sprintf((char *)buf, "音量 = %d ", g_tRadio.ucVolume);
//	////LCD_DisplayString(x2, y, buf);	

	y += LINE_CAP;
	
	if (g_tRadio.ucMuteOn == 1)
	{
		////LCD_DisplayString(x1, y, "静音 = 开 ");
	}
	else
	{
		////LCD_DisplayString(x1, y, "静音 = 关");
	}

	if (g_tRadio.ucAutoMode == 1)
	{
		////LCD_DisplayString(x2, y, "搜台 = 自动");
	}
	else
	{
		////LCD_DisplayString(x2, y, "搜台 = 手动");
	}

	y += LINE_CAP;
	
	if (g_tRadio.ucAdcLevelSet == 1)
	{
		ucTemp = 5;	
	}
	else if (g_tRadio.ucAdcLevelSet == 2)
	{
		ucTemp = 7;
	}
	else if (g_tRadio.ucAdcLevelSet == 3)
	{
		ucTemp = 10;
	}
	else
	{
		ucTemp = 0;
	}
	sprintf((char *)buf, "ADC  = [%d], %d ",	ucTemp, g_tRadio.ucAdcLevelNow);
	////LCD_DisplayString(x1, y, buf);

	if (g_tRadio.ucSpkEn == 1)
	{
		////LCD_DisplayString(x2, y, "输出 = 扬声器");
	}
	else
	{
		////LCD_DisplayString(x2, y, "输出 = 耳机  ");
	}	
		
}
