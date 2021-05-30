#ifndef STM_PROJECT_IRQ_HANDLERS_H
#define STM_PROJECT_IRQ_HANDLERS_H

extern uint8_t screen[8];
extern volatile int timerCount;
extern RTC_HandleTypeDef hrtc;
extern RTC_TimeTypeDef sTime;

void SPI2_IRQHandler(void);
void RTC_IRQHandler(void);
void SysTick_Handler();

#endif
