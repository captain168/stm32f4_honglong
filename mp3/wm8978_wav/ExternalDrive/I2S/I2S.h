#ifndef __I2S_H__
#define __I2S_H__
#include "stm32f4xx.h"

/* I2Sx Communication boards Interface */
#define I2Sx                           SPI3
#define I2Sxext                        I2S3ext
#define I2Sx_CLK                       RCC_APB1Periph_SPI3
#define I2Sx_CLK_INIT                  RCC_APB1PeriphClockCmd

#define I2Sx_WS_PIN                    GPIO_Pin_15
#define I2Sx_WS_GPIO_PORT              GPIOA
#define I2Sx_WS_GPIO_CLK               RCC_AHB1Periph_GPIOA
#define I2Sx_WS_SOURCE                 GPIO_PinSource15
#define I2Sx_WS_AF                     GPIO_AF_SPI3

#define I2Sx_CK_PIN                    GPIO_Pin_3
#define I2Sx_CK_GPIO_PORT              GPIOB
#define I2Sx_CK_GPIO_CLK               RCC_AHB1Periph_GPIOB
#define I2Sx_CK_SOURCE                 GPIO_PinSource3
#define I2Sx_CK_AF                     GPIO_AF_SPI3

#define I2Sx_SD_PIN                    GPIO_Pin_5
#define I2Sx_SD_GPIO_PORT              GPIOB
#define I2Sx_SD_GPIO_CLK               RCC_AHB1Periph_GPIOB
#define I2Sx_SD_SOURCE                 GPIO_PinSource5
#define I2Sx_SD_AF                     GPIO_AF_SPI3

#define I2Sx_MCLK_PIN                    GPIO_Pin_7
#define I2Sx_MCLK_GPIO_PORT              GPIOC
#define I2Sx_MCLK_GPIO_CLK               RCC_AHB1Periph_GPIOC
#define I2Sx_MCLK_SOURCE                 GPIO_PinSource7
#define I2Sx_MCLK_AF                     GPIO_AF_SPI3
//
//#define I2Sxext_SD_PIN                 GPIO_Pin_5
//#define I2Sxext_SD_GPIO_PORT           GPIOB
//#define I2Sxext_SD_GPIO_CLK            RCC_AHB1Periph_GPIOI
//#define I2Sxext_SD_SOURCE              GPIO_PinSource2
//#define I2Sxext_SD_AF                  GPIO_AF_SPI3

void I2S_Test1(void);
void wm8978_I2S_Init(void);
void I2S_Mode_Config(uint16_t _usStandard, uint16_t _usWordLen, uint16_t _usAudioFreq, uint16_t _usMode);


#endif
