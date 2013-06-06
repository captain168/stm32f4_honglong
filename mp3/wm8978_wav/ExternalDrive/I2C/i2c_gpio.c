/*
*
* 文件名: i2c_gpio.c
* 内容简述: 用gpio模拟i2c总线, 适用于STM32系列CPU
*
* 文件历史:
* 版本号 日期       作者    说明
* 1.0    2009-11-15 armfly  创建该文件
*
*/

#include "stm32f4xx.h"

#include "i2c_gpio.h"

#define GPIO_I2C_PORT  GPIOF
#define RCC_APB2Periph_I2C_PORT  RCC_AHB1Periph_GPIOF
#define I2C_SCL_PIN  GPIO_Pin_1			/* 连接到SCL时钟线的GPIO */
#define I2C_SDA_PIN  GPIO_Pin_0			/* 连接到SDA数据线的GPIO */

#define I2C_SCL_1()  GPIO_SetBits(GPIO_I2C_PORT, I2C_SCL_PIN);		/* SCL = 1 */
#define I2C_SCL_0()  GPIO_ResetBits(GPIO_I2C_PORT, I2C_SCL_PIN);	/* SCL = 0 */

#define I2C_SDA_1()  GPIO_SetBits(GPIO_I2C_PORT, I2C_SDA_PIN);		/* SDA = 1 */
#define I2C_SDA_0()  GPIO_ResetBits(GPIO_I2C_PORT, I2C_SDA_PIN);	/* SDA = 0 */

#define I2C_SDA_READ()  GPIO_ReadInputDataBit(GPIO_I2C_PORT, I2C_SDA_PIN)

#define GPIO_Speed_MHz GPIO_Speed_50MHz

/*******************************************************************************
	函数名：i2c_Delay
	输  入: 无
	输  出: 无
	功能说明：I2C总线位延迟，最快400KHz
*/
static void i2c_Delay(void)
{
	uint16_t i;

	for (i = 0; i < 80; i++);
}

/*******************************************************************************
	函数名：i2c_Start
	输  入: 无
	输  出: 无
	功能说明：产生I2C总线起始信号
*/
void i2c_Start(void)
{
	I2C_SDA_1();
	I2C_SCL_1();
	i2c_Delay();
	I2C_SDA_0();
	i2c_Delay();
	I2C_SCL_0();
	i2c_Delay();
}

/*******************************************************************************
	函数名：i2c_Stop
	输  入: 无
	输  出: 无
	功能说明：产生I2C总线停止信号
*/
void i2c_Stop(void)
{
	I2C_SDA_0();
	I2C_SCL_1();
	i2c_Delay();
	I2C_SDA_1();
}

/*******************************************************************************
	函数名：i2c_Stop
	输  入: _ucByte ：发送的数据
	输  出: 无
	功能说明：向I2C总线发送8bit数据
*/
void i2c_SendByte(uint8_t _ucByte)
{
	uint8_t i;

	for (i = 0; i < 8; i++)
	{
		if (_ucByte & 0x80)
		{
			I2C_SDA_1();
		}
		else
		{
			I2C_SDA_0();
		}
		i2c_Delay();
		I2C_SCL_1();
		i2c_Delay();
		I2C_SCL_0();
		_ucByte <<= 1;
		i2c_Delay();
	}
}

/*******************************************************************************
	函数名：i2c_Stop
	输  入: 无
	输  出: 返回独到的数据
	功能说明：从I2C总线读取8bit数据
*/
uint8_t i2c_ReadByte(void)
{
	uint8_t i;
	uint8_t value;

	value = 0;
	for (i = 0; i < 8; i++)
	{
		value <<= 1;
		I2C_SCL_1();
		i2c_Delay();
		if (I2C_SDA_READ())
		{
			value++;
		}
		I2C_SCL_0();
		i2c_Delay();
	}
	return value;
}

/*******************************************************************************
	函数名：i2c_Ack
	输  入: 无
	输  出: 返回0表示正确应答，1表示无器件响应
	功能说明：产生1个ACK应答信号，同时采样设备返回的SDA值
*/
uint8_t i2c_Ack(void)
{
	uint8_t re;
	GPIO_InitTypeDef GPIO_InitStructure;

    /* Configure in output pushpull mode */
    GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN | I2C_SDA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIO_I2C_PORT, &GPIO_InitStructure);
//	GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN | I2C_SDA_PIN;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  /* 开漏输出 */
//	GPIO_Init(GPIO_I2C_PORT, &GPIO_InitStructure);

	I2C_SDA_1();
	i2c_Delay();
	I2C_SCL_1();
	i2c_Delay();
	if (I2C_SDA_READ())
	{
		re = 1;
	}
	else
	{
		re = 0;
	}
	I2C_SCL_0();
	i2c_Delay();
	I2C_SDA_1();

    /* Configure in output pushpull mode */
    GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN | I2C_SDA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIO_I2C_PORT, &GPIO_InitStructure);
//	GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN | I2C_SDA_PIN;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  /* 开漏输出 */
//	GPIO_Init(GPIO_I2C_PORT, &GPIO_InitStructure);
	return re;
}

/*******************************************************************************
	函数名：i2c_NAck
	输  入: 无
	输  出: 返回0表示正确应答，1表示无器件响应
	功能说明：产生1个ACK应答信号，同时采样设备返回的SDA值
*/
void i2c_NAck(void)
{
	I2C_SDA_0();
	i2c_Delay();
	I2C_SCL_1();
	i2c_Delay();
	I2C_SCL_0();
	i2c_Delay();
	I2C_SDA_1();
}

/*******************************************************************************
	函数名：i2c_Configuration
	输  入: 无
	输  出: 无
	功能说明：配置I2C总线，采用模拟IO的方式实现
*/
void i2c_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_APB2Periph_I2C_PORT, ENABLE);

    /* Configure in output pushpull mode */
    GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN | I2C_SDA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIO_I2C_PORT, &GPIO_InitStructure);

//	/* I2C1 SCL PB6 and SDA PB7 pins configuration */
//	GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN | I2C_SDA_PIN;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  /* 开漏输出 */
//	GPIO_Init(GPIO_I2C_PORT, &GPIO_InitStructure);

	/* 给一个停止信号 */
	i2c_Stop();
}

/*******************************************************************************
	函数名：i2c_Configuration
	输  入: _Address：设备的I2C总线地址
	输  出: 返回值 0 表示正确， 返回1表示未探测到
	功能说明：	检测I2C总线设备，发送指定地址数据，然后读取设备应答
*/
uint8_t i2c_CheckDevice(uint8_t _Address)
{
	uint8_t ucAck;

	i2c_Configuration();

	/* 发送起始位 */
	i2c_Start();

	/* 发送设备地址+读写控制bit（0 = w， 1 = r) bit7 先传 */
	i2c_SendByte(_Address | I2C_WR);

	/* 检测ACK */
	ucAck = i2c_Ack();

	/* 发送停止位 */
	i2c_Stop();

	return ucAck;
}
