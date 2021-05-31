#ifndef STM_PROJECT_RTC_H
#define STM_PROJECT_RTC_H

#include "stm32f0xx_hal.h"
#include "stdbool.h"

void RTC_Init(void);
void updateSelectedValue(RTC_TimeTypeDef *time, int selectedValue, bool up);

#endif
