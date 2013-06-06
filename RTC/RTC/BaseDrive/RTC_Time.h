#ifndef _RTC_TIME_H_
#define _RTC_TIME_H_

void RTCTime_Test(void);
void RTC_Config(void);
void RTC_TimeRegulate(void);
void RTC_TimeShow(void);
void RTC_DateShow(void);
void RTC_TimeStampShow(void);
uint8_t USART_Scanf(uint32_t MinValue, uint32_t MaxValue);
void uart_init(void);
void PUTOUT_KeyInit(void);

#endif
