//
// Created by Roman Titkov on 30.05.2021.
//

#include "main.h"
#include "IRQ_Handlers.h"

void SysTick_Handler() {
    timerCount++;
}

void RTC_IRQHandler(void)
{
    static int val = 0;
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    GPIOC->MODER |= GPIO_MODER_MODER9_0;
    if (val) {
        GPIOC->BSRR = GPIO_BSRR_BR_9;
    } else {
        GPIOC->BSRR = GPIO_BSRR_BS_9;
    }
    val = !val;

    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;

    if (HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK) { Error_Handler(); }

    if (HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK) { Error_Handler(); }

    screen[2] = sTime.Hours;
    screen[1] = sTime.Minutes;
    screen[0] = sTime.Seconds;

    HAL_RTC_AlarmIRQHandler(&hrtc);
}

void SPI2_IRQHandler(void) {
    static int index = 0;

    if ((SPI2->SR & SPI_SR_RXNE)) {
        GPIOA->BSRR = GPIO_BSRR_BS_8;
        SPI2->DR;

        GPIOA->BSRR = GPIO_BSRR_BR_8;
        SPI2->DR = screen[index] | (1 << (index + 8));
        index = (index + 1) % 8;
    }
}