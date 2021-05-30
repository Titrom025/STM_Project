#ifndef STM_PROJECT_RTC_H
#define STM_PROJECT_RTC_H

#include "stm32f0xx_hal.h"
#include "stdbool.h"

void RTC_Init(void);
void changeValue(RTC_TimeTypeDef *time, bool up);

#endif
