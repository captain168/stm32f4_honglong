/****************************************************************************
*
* 文件名: button.c
* 内容简述: 本模块实现7个按键的检测，具有滤波机制，支持按下，弹起和长按动作。
*			通过GetKey()函数可以读取键值
* 文件历史:
* 版本号  日期       作者    说明
* v0.1    2009-12-27 armfly  创建该文件
*
*/

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include <stdio.h>

#include "button.h"

static BUTTON_T s_BtnUser;		/* USER 键 */
static BUTTON_T s_BtnTamper;	/* TAMPER 键 */
static BUTTON_T s_BtnWakeUp;	/* WAKEUP 键 */
static BUTTON_T s_BtnUp;		/* 摇杆UP键 */
static BUTTON_T s_BtnDown;		/* 摇杆DOWN键 */
static BUTTON_T s_BtnLeft;		/* 摇杆LEFT键 */
static BUTTON_T s_BtnRight;		/* 摇杆RIGHT键 */
static BUTTON_T s_BtnOk;		/* 摇杆OK键 */

static KEY_FIFO_T s_Key;		/* 按键FIFO变量,结构体 */

/*
	安富莱STM32F103ZE-EK 按键口线分配：
	USER键     : PG8  (低电平表示按下)
	TAMPEER键  : PC13 (低电平表示按下)
	WKUP键     : PA0  (!!!高电平表示按下)
	摇杆UP键   : PG15 (低电平表示按下)
	摇杆DOWN键 : PD3  (低电平表示按下)
	摇杆LEFT键 : PG14 (低电平表示按下)
	摇杆RIGHT键: PG13 (低电平表示按下)
	摇杆OK键   : PG7 (低电平表示按下)

	定义7个函数，判断按键是否按下，返回值1 表示按下，0表示未按下
*/
static uint8_t IsKeyDownUser(void) 		{if (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_11) == Bit_SET) return 0; return 1;}
static uint8_t IsKeyDownTamper(void) 	{if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == Bit_SET) return 0; return 1;}
static uint8_t IsKeyDownWakeUp(void) 	{if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == Bit_SET) return 0; return 1;}
static uint8_t IsKeyDownUp(void) 		{if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == Bit_SET) return 0; return 1;}
static uint8_t IsKeyDownDown(void) 		{if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_2) == Bit_SET) return 0; return 1;}
static uint8_t IsKeyDownLeft(void) 		{if (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_3) == Bit_SET) return 0; return 1;}
static uint8_t IsKeyDownRight(void) 	{if (GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_6) == Bit_SET) return 0; return 1;}
static uint8_t IsKeyDownOk(void) 		{if (GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_7) == Bit_SET) return 0; return 1;}

/*******************************************************************************
	函数名: InitButtonVar
	输  入:
	输  出:
	功能说明：初始化所有的按键变量，这个函数需要在systic中断启动钱调用1次
*/
void InitButtonVar(void)
{
	/* 对按键FIFO读写指针清零 */
	s_Key.Read = 0;
	s_Key.Write = 0;

	/* 初始化USER按键变量 */
	s_BtnUser.IsKeyDownFunc = IsKeyDownUser;	/* 判断按键按下的函数 */
	s_BtnUser.FilterTime = BUTTON_FILTER_TIME;	/* 按键滤波时间 */
	s_BtnUser.LongTime = BUTTON_LONG_TIME;		/* 长按时间 */
	s_BtnUser.Count = s_BtnUser.FilterTime / 2;	/* 计数器设置为滤波时间的一半 */
	s_BtnUser.State = 0;						/* 按键缺省状态，0为未按下 */
	s_BtnUser.KeyCodeDown = KEY_DOWN_USER;		/* 按键按下的键值代码 */
	s_BtnUser.KeyCodeUp = KEY_UP_USER;			/* 按键弹起的键值代码 */
	s_BtnUser.KeyCodeLong = KEY_HOLD_USER;		/* 按键被持续按下的键值代码 */

	/* 初始化TAMPER按键变量 */
	s_BtnTamper.IsKeyDownFunc = IsKeyDownTamper;	/* 判断按键按下的函数 */
	s_BtnTamper.FilterTime = BUTTON_FILTER_TIME;	/* 按键滤波时间 */
	s_BtnTamper.LongTime = 0;						/* 长按时间, 0表示不检测 */
	s_BtnTamper.Count = s_BtnTamper.FilterTime / 2;	/* 计数器设置为滤波时间的一半 */
	s_BtnTamper.State = 0;							/* 按键缺省状态，0为未按下 */
	s_BtnTamper.KeyCodeDown = KEY_DOWN_TAMPER;		/* 按键按下的键值代码 */
	s_BtnTamper.KeyCodeUp = 0;						/* 按键弹起的键值代码 */
	s_BtnTamper.KeyCodeLong = 0;					/* 按键被持续按下的键值代码 */

	/* 初始化WAKEUP按键变量 */
	s_BtnWakeUp.IsKeyDownFunc = IsKeyDownWakeUp;	/* 判断按键按下的函数 */
	s_BtnWakeUp.FilterTime = BUTTON_FILTER_TIME;	/* 按键滤波时间 */
	s_BtnWakeUp.LongTime = 0;						/* 长按时间 */
	s_BtnWakeUp.Count = s_BtnWakeUp.FilterTime / 2;	/* 计数器设置为滤波时间的一半 */
	s_BtnWakeUp.State = 0;							/* 按键缺省状态，0为未按下 */
	s_BtnWakeUp.KeyCodeUp = 0;						/* 按键弹起的键值代码，0表示不检测 */
	s_BtnWakeUp.KeyCodeDown = KEY_DOWN_WAKEUP;		/* 按键按下的键值代码 */
	s_BtnWakeUp.KeyCodeLong = 0;					/* 按键被持续按下的键值代码，0表示不检测 */

	/* 初始化UP按键变量 */
	s_BtnUp.IsKeyDownFunc = IsKeyDownUp;		/* 判断按键按下的函数 */
	s_BtnUp.FilterTime = BUTTON_FILTER_TIME;	/* 按键滤波时间 */
	s_BtnUp.LongTime = 0;						/* 长按时间 */
	s_BtnUp.Count = s_BtnUp.FilterTime / 2;		/* 计数器设置为滤波时间的一半 */
	s_BtnUp.State = 0;							/* 按键缺省状态，0为未按下 */
	s_BtnUp.KeyCodeDown = KEY_DOWN_JOY_UP;		/* 按键按下的键值代码 */
	s_BtnUp.KeyCodeUp = 0;						/* 按键弹起的键值代码，0表示不检测 */
	s_BtnUp.KeyCodeLong = 0;					/* 按键被持续按下的键值代码，0表示不检测 */

	/* 初始化DOWN按键变量 */
	s_BtnDown.IsKeyDownFunc = IsKeyDownDown;	/* 判断按键按下的函数 */
	s_BtnDown.FilterTime = BUTTON_FILTER_TIME;	/* 按键滤波时间 */
	s_BtnDown.LongTime = 0;						/* 长按时间 */
	s_BtnDown.Count = s_BtnDown.FilterTime / 2;	/* 计数器设置为滤波时间的一半 */
	s_BtnDown.State = 0;						/* 按键缺省状态，0为未按下 */
	s_BtnDown.KeyCodeDown = KEY_DOWN_JOY_DOWN;	/* 按键按下的键值代码 */
	s_BtnDown.KeyCodeUp = 0;					/* 按键弹起的键值代码，0表示不检测 */
	s_BtnDown.KeyCodeLong = 0;					/* 按键被持续按下的键值代码，0表示不检测 */

	/* 初始化LEFT按键变量 */
	s_BtnLeft.IsKeyDownFunc = IsKeyDownLeft;	/* 判断按键按下的函数 */
	s_BtnLeft.FilterTime = BUTTON_FILTER_TIME;	/* 按键滤波时间 */
	s_BtnLeft.LongTime = 0;						/* 长按时间 */
	s_BtnLeft.Count = s_BtnLeft.FilterTime / 2;	/* 计数器设置为滤波时间的一半 */
	s_BtnLeft.State = 0;						/* 按键缺省状态，0为未按下 */
	s_BtnLeft.KeyCodeDown = KEY_DOWN_JOY_LEFT;	/* 按键按下的键值代码 */
	s_BtnLeft.KeyCodeUp = 0;					/* 按键弹起的键值代码，0表示不检测 */
	s_BtnLeft.KeyCodeLong = 0;					/* 按键被持续按下的键值代码，0表示不检测 */

	/* 初始化RIGHT按键变量 */
	s_BtnRight.IsKeyDownFunc = IsKeyDownRight;	/* 判断按键按下的函数 */
	s_BtnRight.FilterTime = BUTTON_FILTER_TIME;	/* 按键滤波时间 */
	s_BtnRight.LongTime = 0;					/* 长按时间 */
	s_BtnRight.Count = s_BtnRight.FilterTime / 2;/* 计数器设置为滤波时间的一半 */
	s_BtnRight.State = 0;						/* 按键缺省状态，0为未按下 */
	s_BtnRight.KeyCodeDown = KEY_DOWN_JOY_RIGHT;/* 按键按下的键值代码 */
	s_BtnRight.KeyCodeUp = 0;					/* 按键弹起的键值代码，0表示不检测 */
	s_BtnRight.KeyCodeLong = 0;					/* 按键被持续按下的键值代码，0表示不检测 */

	/* 初始化OK按键变量 */
	s_BtnOk.IsKeyDownFunc = IsKeyDownOk;		/* 判断按键按下的函数 */
	s_BtnOk.FilterTime = BUTTON_FILTER_TIME;	/* 按键滤波时间 */
	s_BtnOk.LongTime = 0;						/* 长按时间 */
	s_BtnOk.Count = s_BtnOk.FilterTime / 2;		/* 计数器设置为滤波时间的一半 */
	s_BtnOk.State = 0;							/* 按键缺省状态，0为未按下 */
	s_BtnOk.KeyCodeDown = KEY_DOWN_JOY_OK;		/* 按键按下的键值代码 */
	s_BtnOk.KeyCodeUp = 0;						/* 按键弹起的键值代码，0表示不检测 */
	s_BtnOk.KeyCodeLong = 0;					/* 按键被持续按下的键值代码，0表示不检测 */
}

/*******************************************************************************
	函数名: PutKey
	输  入: 键值
	输  出:
	功能说明：将1个键值压入按键FIFO缓冲区
*/
void PutKey(uint8_t _KeyCode)
{
	s_Key.Buf[s_Key.Write] = _KeyCode;

	if (++s_Key.Write  >= KEY_FIFO_SIZE)
	{
		s_Key.Write = 0;
	}
}

/*******************************************************************************
	函数名: GetKey
	输  入:
	输  出: 返回键值, KEY_NONE ( = 0) 表示无键按下
	功能说明：从按键FIFO取1个键值
*/
uint8_t GetKey(void)
{
	uint8_t ret;

	if (s_Key.Read == s_Key.Write)
	{
		return KEY_NONE;
	}
	else
	{
		ret = s_Key.Buf[s_Key.Read];

		if (++s_Key.Read >= KEY_FIFO_SIZE)
		{
			s_Key.Read = 0;
		}
		return ret;
	}
}

/*******************************************************************************
	函数名：DetectButton
	输  入: 按键结构变量指针
	输  出:
	功能说明：检测指定的按键
*/
static void DetectButton(BUTTON_T *_pBtn)
{
	/* 如果没有初始化按键函数，则报错
	if (_pBtn->IsKeyDownFunc == 0)
	{
		printf("Fault : DetectButton(), _pBtn->IsKeyDownFunc undefine");
	}
	*/

	if (_pBtn->IsKeyDownFunc())
	{
		if (_pBtn->Count < _pBtn->FilterTime)
		{
			_pBtn->Count = _pBtn->FilterTime;
		}
		else if(_pBtn->Count < 2 * _pBtn->FilterTime)
		{
			_pBtn->Count++;
		}
		else
		{
			if (_pBtn->State == 0)
			{
				_pBtn->State = 1;

				/* 发送按钮按下的消息 */
				if (_pBtn->KeyCodeDown > 0)
				{
					/* 键值放入按键FIFO */
					PutKey(_pBtn->KeyCodeDown);
				}
			}

			if (_pBtn->LongTime > 0)
			{
				if (_pBtn->LongCount < _pBtn->LongTime)
				{
					/* 发送按钮持续按下的消息 */
					if (++_pBtn->LongCount == _pBtn->LongTime)
					{
						/* 键值放入按键FIFO */
						PutKey(_pBtn->KeyCodeLong);
					}
				}
			}
		}
	}
	else
	{
		if(_pBtn->Count > _pBtn->FilterTime)
		{
			_pBtn->Count = _pBtn->FilterTime;
		}
		else if(_pBtn->Count != 0)
		{
			_pBtn->Count--;
		}
		else
		{
			if (_pBtn->State == 1)
			{
				_pBtn->State = 0;

				/* 发送按钮弹起的消息 */
				if (_pBtn->KeyCodeUp > 0)
				{
					/* 键值放入按键FIFO */
					PutKey(_pBtn->KeyCodeUp);
				}
			}
		}

		_pBtn->LongCount = 0;
	}
}

/*******************************************************************************
	函数名：KeyPro
	输  入:
	输  出:
	功能说明：检测所有的按键，这个函数要被systic的中断服务程序调用
*/
void KeyPro(void)
{
	DetectButton(&s_BtnUser);	/* USER 键 */
	DetectButton(&s_BtnTamper);	/* TAMPER 键 */
	DetectButton(&s_BtnWakeUp);	/* WAKEUP 键 */
	DetectButton(&s_BtnUp);		/* 摇杆UP键 */
	DetectButton(&s_BtnDown);	/* 摇杆DOWN键 */
	DetectButton(&s_BtnLeft);	/* 摇杆LEFT键 */
	DetectButton(&s_BtnRight);	/* 摇杆RIGHT键 */
	DetectButton(&s_BtnOk);		/* 摇杆OK键 */
}
