#include <stdbool.h>
#include "rtc.h"
#include "lcd.h"

extern int cursorPosition;
RTC_HandleTypeDef hrtc;

HAL_StatusTypeDef hehe(RTC_HandleTypeDef *hrtc) {
    if (hrtc->State == HAL_RTC_STATE_RESET) {
        /* Allocate lock resource and initialize it */
        hrtc->Lock = HAL_UNLOCKED;

        /* Initialize RTC MSP */
        HAL_RTC_MspInit(hrtc);
    }

    /* Set RTC state */
    hrtc->State = HAL_RTC_STATE_BUSY;

    /* Disable the write protection for RTC registers */
    __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);

    /* Set Initialization mode */
    if (RTC_EnterInitMode(hrtc) != HAL_OK) {
        /* Enable the write protection for RTC registers */
        __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);

        /* Set RTC state */
        hrtc->State = HAL_RTC_STATE_ERROR;

        return HAL_ERROR;
    } else {
        /* Clear RTC_CR FMT, OSEL and POL Bits */
        hrtc->Instance->CR &= ((uint32_t)
                ~(RTC_CR_FMT | RTC_CR_OSEL | RTC_CR_POL));
        /* Set RTC_CR register */
        hrtc->Instance->CR |= (uint32_t)(hrtc->Init.HourFormat | hrtc->Init.OutPut | hrtc->Init.OutPutPolarity);

        /* Configure the RTC PRER */
        hrtc->Instance->PRER = (uint32_t)(hrtc->Init.SynchPrediv);
        hrtc->Instance->PRER |= (uint32_t)(hrtc->Init.AsynchPrediv << 16U);

        /* Exit Initialization mode */
        hrtc->Instance->ISR &= (uint32_t)
                ~RTC_ISR_INIT;

        /* If  CR_BYPSHAD bit = 0, wait for synchro else this check is not needed */
        if ((hrtc->Instance->CR & RTC_CR_BYPSHAD) == RESET) {
            if (HAL_RTC_WaitForSynchro(hrtc) != HAL_OK) {
                /* Enable the write protection for RTC registers */
                __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);

                hrtc->State = HAL_RTC_STATE_ERROR;

                return HAL_ERROR;
            }
        }

        hrtc->Instance->TAFCR &= (uint32_t)
                ~RTC_TAFCR_ALARMOUTTYPE;
        hrtc->Instance->TAFCR |= (uint32_t)(hrtc->Init.OutPutType);

        /* Enable the write protection for RTC registers */
        __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);

        /* Set RTC state */
        hrtc->State = HAL_RTC_STATE_READY;

        return HAL_OK;
    }
}

void setAlarm() {
    hrtc.Instance->WPR = 0xCAU;
    hrtc.Instance->WPR = 0x53U;
    hrtc.Instance->CR &= ~(RTC_CR_ALRAE);
    hrtc.Instance->ISR = ~((0x00000100U) | RTC_ISR_INIT) | ((hrtc).Instance->ISR & RTC_ISR_INIT);
    hrtc.Instance->ALRMAR = (1 << 16U) | (1 << 24U) | (RTC_ALARMMASK_ALL);
    hrtc.Instance->CR |= RTC_CR_ALRAE;
    hrtc.Instance->CR |= 0x00001000U;

    EXTI->IMR |= EXTI_IMR_MR17;
    EXTI->RTSR |= EXTI_IMR_MR17;

    hrtc.Instance->WPR = 0xFFU;
}

void RTC_Init(void) {
    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv = 127;
    hrtc.Init.SynchPrediv = 319;
    hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
    hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

    hehe(&hrtc);
    setAlarm();
}

void updateSelectedValue(RTC_TimeTypeDef *time, int selectedValue, bool up) {
    int diff = 1;
    if (up == false)
        diff = -1;

    switch (selectedValue) {
        case HOURS_T: {
            time->Hours = ((time->Hours / 10 * 10 + 10 * diff + 30) % 30) + ((time->Hours) % 10);
            if (time->Hours > 23) {
                time->Hours = 20;
                drawDigit(time->Hours);
            } else {
                drawDigit(time->Hours / 10);
            }
            break;
        }
        case HOURS_U: {
            if (time->Hours / 10 == 2) {
                time->Hours = (time->Hours / 10) * 10 + ((time->Hours + 4 + diff) % 4);
            } else {
                time->Hours = (time->Hours / 10) * 10 + ((time->Hours + 10 + diff) % 10);
            }
            drawDigit(time->Hours % 10);
            break;
        }
        case MINUTES_T: {
            time->Minutes = ((time->Minutes / 10 * 10 + 10 * diff + 60) % 60) + ((time->Minutes) % 10);
            drawDigit(time->Minutes / 10);
            break;
        }
        case MINUTES_U: {
            time->Minutes = (time->Minutes / 10) * 10 + ((time->Minutes + 10 + diff) % 10);
            drawDigit(time->Minutes % 10);
            break;
        }
        case SECONDS_T: {
            time->Seconds = ((time->Seconds / 10 * 10 + 10 * diff + 60) % 60) + ((time->Seconds) % 10);
            drawDigit(time->Seconds / 10);
            break;
        }
        case SECONDS_U: {
            time->Seconds = (time->Seconds / 10) * 10 + ((time->Seconds + 10 + diff) % 10);
            drawDigit(time->Seconds % 10);
            break;
        }
    }
}