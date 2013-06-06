

#ifndef __LCD_H__
#define __LCD_H__	1
//#include "main.h"
#include "stm32f4xx.h"



/*lcd reset pin def*/
//#define LCD_RST_PORT_RCC	RCC_AHB1Periph_GPIOD
//#define LCD_RST_PIN			GPIO_Pin_3
//#define LCD_RST_PORT		GPIOD






void lcd_init(void);
void lcd_clear(unsigned short color);




#define LCD_WIDTH   240
#define LCD_HEIGHT  320

/* LCD color */
#define White          0xFFFF
#define Black          0x0000
#define Grey           0xF7DE
#define Blue           0x001F
#define Blue2          0x051F
#define Red            0xF800
#define Magenta        0xF81F
#define Green          0x07E0
#define Cyan           0x7FFF
#define Yellow         0xFFE0


void lcd_init(void);
void lcd_clear(unsigned short Color);



#endif
