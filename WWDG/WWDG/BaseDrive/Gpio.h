#ifndef _GPIO_H_
#define _GIPO_H_

#define KEY_NUMBER         4    //°´¼ü×ÜÊý

#define LED1_RCC           RCC_AHB1Periph_GPIOA
#define LED1_GPIO          GPIOA
#define LED1_GPIO_PIN      GPIO_Pin_6
#define LED1_ONOFF(x)      GPIO_WriteBit(LED1_GPIO,LED1_GPIO_PIN,x);
#define LED1_TURN()        GPIO_ToggleBits(LED1_GPIO,LED1_GPIO_PIN);

#define LED2_RCC           RCC_AHB1Periph_GPIOA
#define LED2_GPIO          GPIOA
#define LED2_GPIO_PIN      GPIO_Pin_4
#define LED2_ONOFF(x)      GPIO_WriteBit(LED2_GPIO,LED2_GPIO_PIN,x);
#define LED2_TURN()        GPIO_ToggleBits(LED2_GPIO,LED2_GPIO_PIN);

#define LED3_RCC           RCC_AHB1Periph_GPIOB
#define LED3_GPIO          GPIOB
#define LED3_GPIO_PIN      GPIO_Pin_6
#define LED3_ONOFF(x)      GPIO_WriteBit(LED3_GPIO,LED3_GPIO_PIN,x);
#define LED3_TURN()        GPIO_ToggleBits(LED3_GPIO,LED3_GPIO_PIN);

#define LED4_RCC           RCC_AHB1Periph_GPIOC
#define LED4_GPIO          GPIOC
#define LED4_GPIO_PIN      GPIO_Pin_6
#define LED4_ONOFF(x)      GPIO_WriteBit(LED4_GPIO,LED4_GPIO_PIN,x);
#define LED4_TURN()        GPIO_ToggleBits(LED4_GPIO,LED4_GPIO_PIN);

typedef struct{
    uint32_t     rcc;
    GPIO_TypeDef *gpio;
    uint16_t     pin;
}Gpio_Info;

void LEDGpio_Init(void);

#endif
