#include <stdbool.h>
#include "rtc.h"
#include "lcd.h"

extern int cursorPosition;
RTC_HandleTypeDef hrtc;

void RTC_Init(void) {
    RTC_AlarmTypeDef sAlarm = {0};

    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv = 127;
    hrtc.Init.SynchPrediv = 319;
    hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
    hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
    HAL_RTC_Init(&hrtc);

    sAlarm.AlarmTime.Hours = 0x0;
    sAlarm.AlarmTime.Minutes = 0x0;
    sAlarm.AlarmTime.Seconds = 0x0;
    sAlarm.AlarmTime.SubSeconds = 0x0;
    sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
    sAlarm.AlarmMask = RTC_ALARMMASK_ALL;
    sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
    sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
    sAlarm.AlarmDateWeekDay = 0x1;
    sAlarm.Alarm = RTC_ALARM_A;
    HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD);
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