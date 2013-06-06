/*************************************************************************
* 文件名: bsp_WM8978.c
* 内容简述: WM8978音频芯片驱动
*
* 文件历史:
* 版本号 日期       作者    说明
* 1.0    2009-11-15 armfly  创建该文件
*
*/
#include "stm32f4xx.h"

#include "bsp_WM8978.h"
#include "i2c_gpio.h"

/*
	wm8978寄存器缓存cash
	由于I2C两线接口不支持读取操作，因此寄存器设置换存在内存中
	寄存器MAP 在WM8978.pdf 的第67页
	寄存器地址是7bit， 寄存器数据是9bit
*/
static uint16_t wm8978_RegCash[] = {
	0x000, 0x000, 0x000, 0x000, 0x050, 0x000, 0x140, 0x000,
	0x000, 0x000, 0x000, 0x0FF, 0x0FF, 0x000, 0x100, 0x0FF,
	0x0FF, 0x000, 0x12C, 0x02C, 0x02C, 0x02C, 0x02C, 0x000,
	0x032, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x038, 0x00B, 0x032, 0x000, 0x008, 0x00C, 0x093, 0x0E9,
	0x000, 0x000, 0x000, 0x000, 0x003, 0x010, 0x010, 0x100,
	0x100, 0x002, 0x001, 0x001, 0x039, 0x039, 0x039, 0x039,
	0x001, 0x001
};

/****************************************************************************
* 函数名: wm8978_ReadReg
* 功  能: 从cash中读回读回wm8978寄存器
* 输  入: _ucRegAddr ： 寄存器地址
* 输  出: 无.
* 返  回: 寄存器值.
*/
uint16_t wm8978_ReadReg(uint8_t _ucRegAddr)
{
	return wm8978_RegCash[_ucRegAddr];
}

/****************************************************************************
* 函数名: wm8978_WriteReg
* 功  能: 写wm8978寄存器
* 输  入: _ucRegAddr ： 寄存器地址
*		  _usValue ：寄存器值
* 输  出: 无.
* 返  回: 0表示失败， 1表示成功
*/
uint8_t wm8978_WriteReg(uint8_t _ucRegAddr, uint16_t _usValue)
{
	uint8_t ucAck;

	/* 发送起始位 */
	i2c_Start();

	/* 发送设备地址+读写控制bit（0 = w， 1 = r) bit7 先传 */
	i2c_SendByte(WM8978_SLAVE_ADDRESS | I2C_WR);

	/* 检测ACK */
	ucAck = i2c_Ack();
	if (ucAck == 1)
	{
		return 0;
	}

	/* 发送控制字节1 */
	i2c_SendByte(((_ucRegAddr << 1) & 0xFE) | ((_usValue >> 8) & 0x1));

	/* 检测ACK */
	ucAck = i2c_Ack();
	if (ucAck == 1)
	{
		return 0;
	}

	/* 发送控制字节2 */
	i2c_SendByte(_usValue & 0xFF);

	/* 检测ACK */
	ucAck = i2c_Ack();
	if (ucAck == 1)
	{
		return 0;
	}

	/* 发送STOP */
	i2c_Stop();

	wm8978_RegCash[_ucRegAddr] = _usValue;
	return 1;
}

/****************************************************************************
* 函数名: wm8978_Dac2Ear
* 功  能: 初始化wm8978硬件设备,DAC输出到耳机
* 输  入: 无.
* 输  出: 无.
* 返  回: 无.
*/
void wm8978_Dac2Ear(void)
{
	wm8978_Cfg(DAC_ON, AUX_OFF, LINE_OFF, SPK_OFF, EAR_ON);
}

/****************************************************************************
* 函数名: wm8978_Dac2Spk
* 功  能: 初始化wm8978硬件设备,DAC输出到扬声器
* 输  入: 无.
* 输  出: 无.
* 返  回: 无.
*/
void wm8978_Dac2Spk(void)
{
	wm8978_Cfg(DAC_ON, AUX_OFF, LINE_OFF, SPK_ON, EAR_OFF);
}

/****************************************************************************
* 函数名: wm8978_Aux2Ear
* 功  能: 初始化wm8978硬件设备,Aux(FM收音机)输出到耳机
* 输  入: 无.
* 输  出: 无.
* 返  回: 无.
*/
void wm8978_Aux2Ear(void)
{
	wm8978_Cfg(DAC_OFF, AUX_ON, LINE_OFF, SPK_ON, EAR_ON);
}

/****************************************************************************
* 函数名: wm8978_Aux2Spk
* 功  能: 初始化wm8978硬件设备,Aux(FM收音机)输出到扬声器
* 输  入: 无.
* 输  出: 无.
* 返  回: 无.
*/
void wm8978_Aux2Spk(void)
{
	wm8978_Cfg(DAC_OFF, AUX_ON, LINE_OFF, SPK_ON, EAR_OFF);
}

/****************************************************************************
* 函数名: wm8978_Cfg
* 功  能: 初始化wm8978硬件设备,切换到AUX输入模式
* 输  入: _ucSpkEn 0: 扬声器输出, 1:耳机输出 
* 输  出: 无.
* 返  回: 无.
*/
void wm8978_Cfg(uint8_t _ucDacEn, uint8_t _ucAuxEn, uint8_t _ucLineEn, uint8_t _ucSpkEn, uint8_t _ucEarEn)
{
	uint16_t usReg;
	
	/* pdf 67页，寄存器列表 */

	/*	REG 1
		B8		BUFDCOPEN	= x
		B7		OUT4MIXEN	= x
		B6		OUT3MIXEN	= X  耳机输出时，必须设置为1 (for 安富莱开发板)
		B5		PLLEN	= x
		b4`		MICBEN = x
		B3		BIASEN = 1		必须设置为1模拟放大器才工作
		B2		BUFIOEN = x
		B1:0	VMIDSEL = 3  必须设置为非00值模拟放大器才工作
	*/
	usReg = 0;
	if ((_ucSpkEn == 1) || (_ucEarEn == 1))
	{
		usReg = ((1 << 3) | (3 << 0));
	}
	if (_ucEarEn == 1)
	{
		usReg |= (1 << 6);
	}
	wm8978_WriteReg(1, usReg);

	/*	REG 2
		B8		ROUT1EN = 1;	耳机输出通道
		B7		LOUT1EN = 1;	耳机输出通道	
		B6		SLEEP = x;
		B5		BOOSTENR = x;
		B4		BOOSTENL = x;
		B3		INPGAENR = x;
		B2		NPPGAENL = x;
		B1		ADCENR = x;
		B0		ADCENL = x;
	*/
	usReg = 0;
	if (_ucEarEn == 1)
	{
		usReg = ((1 << 8) | (1 << 7));
	}
	wm8978_WriteReg(2, usReg);		
		
	/* REG 3
		B8	OUT4EN = 0
		B7	OUT3EN = x;		耳机输出，用于耳机的地线
		B6	LOUT2EN = 1;	扬声器输出通道
		B5	ROUT2EN = 1;	扬声器输出通道
		B4	----   = x
		B3	RMIXEN = 1;		输出MIX, 耳机和扬声器公用
		B2	LMIXEN = 1;		输出MIX, 耳机和扬声器公用
		B1	DACENR = x;		DAC用
		B0	DACENL = x;
	*/
	usReg = 0;
	if ((_ucSpkEn == 1) || (_ucEarEn == 1))
	{
		usReg |= ((1 << 3) | (1 << 2));
	}
	if (_ucEarEn == 1)
	{
		usReg |= (1 << 7);
	}
	if (_ucSpkEn == 1)
	{
		usReg |= ((1 << 6) | (1 << 5));
	}
	if (_ucDacEn == 1)
	{
		usReg |= ((1 << 1) | (1 << 0));
	}
	wm8978_WriteReg(3, usReg);

	/*
		REG 11,12
		DAC 音量
	*/
	if (_ucDacEn == 1)
	{	
		#if 0	/* 此处不要设置音量, 避免切换时音量状态被改变 */
		wm8978_WriteReg(11, 255);
		wm8978_WriteReg(12, 255 | 0x100);
		#endif
	}
	else
	{
		;
	}

	/*	REG 43
		B8:6 = 0
		B5	MUTERPGA2INV = 0;	Mute input to INVROUT2 mixer
		B4	INVROUT2 = ROUT2 反相; 用于扬声器推挽输出
		B3:1	BEEPVOL = 7;	AUXR input to ROUT2 inverter gain
		B0	BEEPEN = 1;	1 = enable AUXR beep input
			
	*/
	usReg = 0;
	if (_ucSpkEn == 1)
	{
		usReg |= (1 << 4);
	} 
	if (_ucAuxEn == 1)
	{
		usReg |= ((7 << 1) | (1 << 0));
	}
	wm8978_WriteReg(43, usReg);
		
	/* REG 49
		B8:7	0
		B6		DACL2RMIX = x
		B5		DACR2LMIX = x
		B4		OUT4BOOST = 0
		B3		OUT3BOOST = 0
		B2		SPKBOOST = x	SPK BOOST
		B1		TSDEN = 1    扬声器热保护使能（缺省1）
		B0		VROI = 0	Disabled Outputs to VREF Resistance
	*/
	usReg = 0;
	if (_ucDacEn == 1)
	{
		usReg |= ((0 << 6) | (0 << 5));
	}
	if (_ucSpkEn == 1)
	{
		usReg |=  ((0 << 2) | (1 << 1));	/* 1.5x增益,  热保护使能 */
	}
	wm8978_WriteReg(49, usReg);
	
	/*	REG 50    (50是左声道，51是右声道，配置寄存器功能一致
		B8:6	AUXLMIXVOL = 111	AUX用于FM收音机信号输入
		B5		AUXL2LMIX = 1		Left Auxilliary input to left channel
		B4:2	BYPLMIXVOL			音量
		B1		BYPL2LMIX = 0;		Left bypass path (from the left channel input boost output) to left output mixer
		B0		DACL2LMIX = 1;		Left DAC output to left output mixer
	*/
	usReg = 0;
	if (_ucAuxEn == 1)
	{
		usReg |= ((7 << 6) | (1 << 5));
	}
	if (_ucLineEn == 1)
	{
		usReg |= ((7 << 2) | (1 << 1));
	}
	if (_ucDacEn == 1)
	{
		usReg |= (1 << 0);
	}
	wm8978_WriteReg(50, usReg);
	wm8978_WriteReg(51, usReg);

	/*	
		REG 52,53	控制EAR音量
		REG 54,55	控制SPK音量
		
		B8		HPVU		用于同步更新左右声道
		B7		LOUT1ZC = 1  零位切换
		B6		LOUT1MUTE    0表示正常， 1表示静音
	*/
#if 0	/* 此处不要设置音量, 避免应用程序切换输出时，音量状态被改变 */
	if (_ucEarEn == 1)
	{
		usReg = (0x3f | (1 << 7));
		wm8978_WriteReg(52, usReg);
		wm8978_WriteReg(53, usReg | (1 << 8));
	}
	else
	{
		usReg = ((1 << 7) | (1 << 6));
		wm8978_WriteReg(52, usReg);
		wm8978_WriteReg(53, usReg | (1 << 8));
	}
	
	if (_ucSpkEn == 1)
	{
		usReg = (0x3f | (1 << 7));
		wm8978_WriteReg(54, usReg);
		wm8978_WriteReg(55, usReg | (1 << 8));
	}
	else
	{
		usReg = ((1 << 7) | (1 << 6));
		wm8978_WriteReg(54, usReg);
		wm8978_WriteReg(55, usReg | (1 << 8));
	}		
#endif	
		
	/*	REG 56   OUT3 mixer ctrl
		B6		OUT3MUTE = 1;
		B5		LDAC2OUT3 = 0;
		B4		OUT4_2OUT3
		B3		BYPL2OUT3
		B2		LMIX2OUT3	
		B1		LDAC2OUT3
	*/
	wm8978_WriteReg(56, (1 <<6));		/**/
				
	/* 	Softmute enable = 0 */
	if (_ucDacEn == 1)
	{
		wm8978_WriteReg(10, 0);
	}
}

/****************************************************************************
* 函数名: wm8978_ChangeVolume
* 功  能: 改变音量.
* 输  入: 两个左右声道音量的值.
* 返  回: 无.
*/
void wm8978_ChangeVolume(uint8_t _ucLeftVolume, uint8_t _ucRightVolume)
{
#if 0
	wm8978_WriteReg(11, _ucLeftVolume);
	wm8978_WriteReg(12, _ucRightVolume | 0x100);
#else
	uint16_t regL;
	uint16_t regR;

	if (_ucLeftVolume > 0x3F)
	{
		_ucLeftVolume = 0x3F;
	}

	if (_ucRightVolume > 0x3F)
	{
		_ucRightVolume = 0x3F;
	}

	regL = _ucLeftVolume;
	regR = _ucRightVolume;

	/* 先更新左声道缓存值 */
	wm8978_WriteReg(52, regL | 0x00);

	/* 再同步更新左右声道的音量 */
	wm8978_WriteReg(53, regR | 0x100);	/* 0x180表示 在音量为0时再更新，避免调节音量出现的“嘎哒”声 */

	/* 先更新左声道缓存值 */
	wm8978_WriteReg(54, regL | 0x00);

	/* 再同步更新左右声道的音量 */
	wm8978_WriteReg(55, regR | 0x100);	/* 在音量为0时再更新，避免调节音量出现的“嘎哒”声 */

#endif
}

/****************************************************************************
* 函数名: wm8978_ReadVolume
* 功  能: 读回通道的音量.
* 输  入: 无.
* 返  回: 当前音量.
*/
uint8_t wm8978_ReadVolume(void)
{
	return (uint8_t)(wm8978_ReadReg(52) & 0x3F );
}

/****************************************************************************
* 函数名: wm8978_Mute
* 功  能: 输出静音
* 输  入: _ucMute ：1是静音，0是不静音.
* 返  回: 无.
*/
void wm8978_Mute(uint8_t _ucMute)
{
	uint16_t usRegValue;

	if (_ucMute == 1) /* 静音 */
	{
		usRegValue = wm8978_ReadReg(52); /* Left Mixer Control */
		usRegValue |= (1u << 6);
		wm8978_WriteReg(52, usRegValue);

		usRegValue = wm8978_ReadReg(53); /* Left Mixer Control */
		usRegValue |= (1u << 6);
		wm8978_WriteReg(53, usRegValue);

		usRegValue = wm8978_ReadReg(54); /* Right Mixer Control */
		usRegValue |= (1u << 6);
		wm8978_WriteReg(54, usRegValue);

		usRegValue = wm8978_ReadReg(55); /* Right Mixer Control */
		usRegValue |= (1u << 6);
		wm8978_WriteReg(55, usRegValue);
	}
	else	/* 取消静音 */
	{
		usRegValue = wm8978_ReadReg(52);
		usRegValue &= ~(1u << 6);
		wm8978_WriteReg(52, usRegValue);

		usRegValue = wm8978_ReadReg(53); /* Left Mixer Control */
		usRegValue &= ~(1u << 6);
		wm8978_WriteReg(53, usRegValue);

		usRegValue = wm8978_ReadReg(54);
		usRegValue &= ~(1u << 6);
		wm8978_WriteReg(54, usRegValue);

		usRegValue = wm8978_ReadReg(55); /* Left Mixer Control */
		usRegValue &= ~(1u << 6);
		wm8978_WriteReg(55, usRegValue);
	}
}

/****************************************************************************
* 函数名: wm8978_Reset
* 功  能: 复位wm8978，所有的寄存器值恢复到缺省值
* 输  入: 无.
* 返  回: 无.
*/
void wm8978_Reset(void)
{
	/* wm8978寄存器缺省值 */
	const uint16_t reg_default[] = {
	0x000, 0x000, 0x000, 0x000, 0x050, 0x000, 0x140, 0x000,
	0x000, 0x000, 0x000, 0x0FF, 0x0FF, 0x000, 0x100, 0x0FF,
	0x0FF, 0x000, 0x12C, 0x02C, 0x02C, 0x02C, 0x02C, 0x000,
	0x032, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x038, 0x00B, 0x032, 0x000, 0x008, 0x00C, 0x093, 0x0E9,
	0x000, 0x000, 0x000, 0x000, 0x003, 0x010, 0x010, 0x100,
	0x100, 0x002, 0x001, 0x001, 0x039, 0x039, 0x039, 0x039,
	0x001, 0x001
	};
	uint8_t i;

	i2c_Configuration();

	wm8978_WriteReg(0x00, 0);

	for (i = 0; i < sizeof(reg_default) / 2; i++)
	{
		wm8978_RegCash[i] = reg_default[i];
	}
}

/****************************************************************************
* 函数名: wm8978_PowerDown
* 功  能: 关闭wm8978，进入低功耗模式
* 输  入: 无.
* 返  回: 无.
*/
void wm8978_PowerDown(void)
{
	/*
	Set DACMU = 1 to mute the audio DACs.
	Disable all Outputs.
	Disable VREF and VMIDSEL.
	Switch off the power supplies
	*/
	uint16_t usRegValue;

	usRegValue = wm8978_ReadReg(10);
	usRegValue |= (1u <<6);
	wm8978_WriteReg(10, usRegValue);

	/* 未完 */
}

/****************************************************************************
* 函数名: wm8978_Func1
* 功  能: 备用，用于I2S接口配置（未完）
* 输  入: 无.
* 返  回: 无.
*/
void wm8978_Func1(void)
{
	/*
		REG	4	Audio Interface
		REG	6	Clock Gen ctrl
	*/
	
	/* ms = 0  Enable Slave Mode, I2S Mode, 16bit */
	wm8978_WriteReg(4, 0x10);
	/* CLKSEL = 0 */
	wm8978_WriteReg(6, 0x000);
}
