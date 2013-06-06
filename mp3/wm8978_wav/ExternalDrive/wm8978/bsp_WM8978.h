/****************************************************************************
* Copyright (C), 2009-2010, www.armfly.com
*
* 文件名: bsp_wm8978.h
* 内容简述: wm8978音频芯片驱动模块的头文件
*
* 文件历史:
* 版本号 日期       作者    说明
* 1.0    2009-11-15 armfly  创建该文件
*
*/

#ifndef _BSP_wm8978_H
#define _BSP_wm8978_H

#include <inttypes.h>
#include "i2c_gpio.h"

/* wm8978 register space */
struct wm8978_setup_data {
	unsigned short i2c_address;
};

#define WM8978_SLAVE_ADDRESS    0x34

#define wm8978_PLL1			0
#define wm8978_PLL2			1

/* clock inputs */
#define wm8978_MCLK		0
#define wm8978_PCMCLK		1

/* clock divider id's */
#define wm8978_PCMDIV		0
#define wm8978_BCLKDIV		1
#define wm8978_VXCLKDIV		2

/* PCM clock dividers */
#define wm8978_PCM_DIV_1	(0 << 6)
#define wm8978_PCM_DIV_3	(2 << 6)
#define wm8978_PCM_DIV_5_5	(3 << 6)
#define wm8978_PCM_DIV_2	(4 << 6)
#define wm8978_PCM_DIV_4	(5 << 6)
#define wm8978_PCM_DIV_6	(6 << 6)
#define wm8978_PCM_DIV_8	(7 << 6)

/* BCLK clock dividers */
#define wm8978_BCLK_DIV_1	(0 << 3)
#define wm8978_BCLK_DIV_2	(1 << 3)
#define wm8978_BCLK_DIV_4	(2 << 3)
#define wm8978_BCLK_DIV_8	(3 << 3)
#define wm8978_BCLK_DIV_16	(4 << 3)

/* VXCLK clock dividers */
#define wm8978_VXCLK_DIV_1	(0 << 6)
#define wm8978_VXCLK_DIV_2	(1 << 6)
#define wm8978_VXCLK_DIV_4	(2 << 6)
#define wm8978_VXCLK_DIV_8	(3 << 6)
#define wm8978_VXCLK_DIV_16	(4 << 6)

#define wm8978_DAI_HIFI		0
#define wm8978_DAI_VOICE		1

/* wm8978工作状态 */
#define WM8978_STOP		 		0	/* 停止状态 */
#define WM8978_DAC_SPK_PLAY		1	/* DAC,喇叭播放 */
#define WM8978_DAC_EAR_PLAY		2	/* DAC,耳机播放 */
#define WM8978_AUX_SPK_PLAY		3	/* FM,喇叭播放 */
#define WM8978_AUX_EAR_PLAY		4	/* FM,耳机播放 */

/*  用于wm8978_Cfg() 形参 */
#define DAC_ON			1
#define DAC_OFF			0

#define AUX_ON			1
#define AUX_OFF			0

#define LINE_ON			1
#define LINE_OFF		0

#define EAR_ON			1
#define EAR_OFF			0

#define SPK_ON			1
#define SPK_OFF			0

/* 定义最大音量 */
#define VOLUME_MAX		63		/* 最大音量 */
#define VOLUME_STEP		3		/* 音量调节步长 */

//void wm8978_I2C_Configuration(void);
void wm8978_ChangeVolume(uint8_t _ucLeftVolume, uint8_t _ucRightVolume);
uint8_t wm8978_ReadVolume(void);
void wm8978_Mute(uint8_t _ucMute);
void wm8978_Reset(void);
void wm8978_PowerDown(void);
uint8_t wm8978_WriteReg(uint8_t _ucRegAddr, uint16_t _usValue);
void wm8978_GPIO1(uint8_t _ucData);

void wm8978_Dac2Ear(void);
void wm8978_Dac2Spk(void);
void wm8978_Aux2Ear(void);
void wm8978_Aux2Spk(void);

void wm8978_Cfg(uint8_t _ucDacEn, uint8_t _ucAuxEn, uint8_t _ucLineEn, uint8_t _ucSpkEn, uint8_t _ucEarEn);

#endif
