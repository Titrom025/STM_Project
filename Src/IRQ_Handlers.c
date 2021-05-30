#include <rtc.h>
#include "lcd.h"
#include "IRQ_Handlers.h"
#include "button_lib.h"

#define TIMER_LIMIT 100

volatile int timerCount;
volatile int buttonPressed = -1;
volatile static bool led_states[5];
volatile static int led_buttons_count[5];
int lastPressed = NO_BUTTON;

int screen[8];

extern RTC_HandleTypeDef hrtc;
extern RTC_DateTypeDef sDate;
extern RTC_TimeTypeDef sTime;

RTC_TimeTypeDef alarmTime;
RTC_TimeTypeDef editTime;

bool editMode = false;
bool alarmMode = false;
bool alarmOn = false;
bool alarmDetected = false;
bool alarmSignalOn = false;

volatile int cursorPosition = 0;

void enterEditMode() {
    cursorPosition = 0;
    Lcd_cursor(0, cursorPosition);
    Lcd_string("CLK:");

    cursorPosition = 11;
    lcd_write_command(0b00001110);
    Lcd_cursor(0, cursorPosition);
}

void enterAlarmMode() {
    if (HAL_RTC_SetTime(&hrtc, &editTime, RTC_FORMAT_BIN) != HAL_OK) { Error_Handler(); }

    Lcd_clear();
    cursorPosition = 0;
    Lcd_cursor(0, cursorPosition);
    Lcd_string("ALM:");
    if (alarmOn) {
        char buffer[16];
        sprintf(buffer, "%02d:%02d:%02d", alarmTime.Hours, alarmTime.Minutes, alarmTime.Seconds);
        Lcd_string(buffer);
        cursorPosition = 13;
        Lcd_cursor(0, cursorPosition);
        Lcd_string("ON");
    } else {
        cursorPosition = 13;
        Lcd_cursor(0, cursorPosition);
        Lcd_string("OFF");
    }

    cursorPosition = 14;
    Lcd_cursor(0, cursorPosition);
}

void enterDefaultMode() {
    cursorPosition = 0;
    Lcd_cursor(0, cursorPosition);
    Lcd_clear();

    lcd_write_command(0b00001100);
    Lcd_clear();
    char buffer[16];
    sprintf(buffer, "    %02d:%02d:%02d", sTime.Hours, sTime.Minutes, sTime.Seconds);
    Lcd_string(buffer);
    if (alarmOn) {
        cursorPosition = 14;
        Lcd_cursor(0, cursorPosition);
        lcd_write_data(0x00);
    }
}

void SysTick_Handler() {
    timerCount++;
    updateButton();
    buttonPressed = returnButton();

    if (buttonPressed == NO_BUTTON) {
        lastPressed = NO_BUTTON;
        return;
    }

    if (timerCount - led_buttons_count[buttonPressed] > TIMER_LIMIT && lastPressed != buttonPressed) {
        led_states[buttonPressed] = !led_states[buttonPressed];
        led_buttons_count[buttonPressed] = timerCount;
        lastPressed = buttonPressed;
    } else {
        return;
    }

    if (buttonPressed == USER_BUTTON) {
        if (alarmDetected) {
            GPIOC->BSRR = GPIO_BSRR_BR_8;
            alarmDetected = false;
            cursorPosition = 14;
            Lcd_cursor(0, cursorPosition);
            Lcd_string(" ");
        } else if (editMode) {
            editMode = false;
            alarmMode = true;
            enterAlarmMode();
        } else if (alarmMode) {
            editMode = false;
            alarmMode = false;
            enterDefaultMode();
        } else {
            editMode = true;
            enterEditMode();
        }
    }

    if (editMode == false && alarmMode == false)
        return;

    if (buttonPressed == TOP_BUTTON) {
        if (editMode) {
            changeValue(&editTime, true);
        } else {
            if (cursorPosition == 14) {
                cursorPosition = 4;
                Lcd_cursor(0, cursorPosition);
                if (alarmOn == false) {
                    char buffer[16];
                    sprintf(buffer, "%02d:%02d:%02d", alarmTime.Hours, alarmTime.Minutes, alarmTime.Seconds);
                    Lcd_string(buffer);

                    cursorPosition = 13;
                    Lcd_cursor(0, cursorPosition);

                    Lcd_string("ON ");
                    alarmOn = true;
                } else {
                    Lcd_string("         OFF");
                    alarmOn = false;
                    alarmTime.Hours = 0;
                    alarmTime.Minutes = 0;
                    alarmTime.Seconds = 0;
                }
                cursorPosition = 14;
                Lcd_cursor(0, cursorPosition);
            } else {
                changeValue(&alarmTime, true);
            }
        }

    } else if (buttonPressed == DOWN_BUTTON) {
        if (editMode) {
            changeValue(&editTime, false);
        } else {
            if (cursorPosition == 14) {
                cursorPosition = 4;
                Lcd_cursor(0, cursorPosition);
                if (alarmOn == false) {
                    char buffer[16];
                    sprintf(buffer, "%02d:%02d:%02d", alarmTime.Hours, alarmTime.Minutes, alarmTime.Seconds);
                    Lcd_string(buffer);

                    cursorPosition = 13;
                    Lcd_cursor(0, cursorPosition);

                    Lcd_string("ON ");
                    alarmOn = true;
                } else {
                    Lcd_string("         OFF");
                    alarmOn = false;
                    alarmTime.Hours = 0;
                    alarmTime.Minutes = 0;
                    alarmTime.Seconds = 0;
                }
                cursorPosition = 14;
                Lcd_cursor(0, cursorPosition);
            } else {
                changeValue(&alarmTime, false);
            }
        }

    } else if (buttonPressed == LEFT_BUTTON) {
        if (editMode) {
            if (cursorPosition > 4) { cursorPosition--; }
            if (cursorPosition == 6 || cursorPosition == 9) { cursorPosition--; }

            Lcd_cursor(0, cursorPosition);
        } else {
            if (cursorPosition == 14 && alarmOn) {
                cursorPosition = 11;
                Lcd_cursor(0, cursorPosition);
            } else if (cursorPosition > 4 && alarmOn) {
                cursorPosition--;
            }
            if (cursorPosition == 6 || cursorPosition == 9) { cursorPosition--; }

            Lcd_cursor(0, cursorPosition);
        }
    } else if (buttonPressed == RIGHT_BUTTON) {
        if (editMode) {
            if (cursorPosition < 11) { cursorPosition++; }
            if (cursorPosition == 6 || cursorPosition == 9) { cursorPosition++; }

            Lcd_cursor(0, cursorPosition);
        } else {
            if (cursorPosition == 11) {
                cursorPosition = 14;
                Lcd_cursor(0, cursorPosition);
            } else if (cursorPosition < 11) {
                cursorPosition++;
            }
            if (cursorPosition == 6 || cursorPosition == 9) { cursorPosition++; }

            Lcd_cursor(0, cursorPosition);
        }
    }
}

void RTC_IRQHandler(void) {
    static int val = 0;
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    GPIOC->MODER |= GPIO_MODER_MODER9_0;
    if (val) {
        GPIOC->BSRR = GPIO_BSRR_BR_9;
    } else {
        GPIOC->BSRR = GPIO_BSRR_BS_9;
    }
    val = !val;

    if (HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) { Error_Handler(); }

    if (HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK) { Error_Handler(); }

    screen[2] = sTime.Hours;
    screen[1] = sTime.Minutes;
    screen[0] = sTime.Seconds;

    if (editMode == false && alarmMode == false) {
        drawOnlyChanges(&editTime);
        editTime.Seconds = sTime.Seconds;
        editTime.Minutes = sTime.Minutes;
        editTime.Hours = sTime.Hours;
    }

    if (alarmOn &&
        alarmTime.Hours == sTime.Hours &&
        alarmTime.Minutes == sTime.Minutes &&
        alarmTime.Seconds == sTime.Seconds) {
        alarmDetected = true;
    }

    if (alarmDetected) {
        if (alarmSignalOn) {
            GPIOC->BSRR = GPIO_BSRR_BR_8; // GPIO_BSRR_BR_7;
        } else {
            GPIOC->BSRR = GPIO_BSRR_BS_8; // GPIO_BSRR_BR_7;
        }
        alarmSignalOn = !alarmSignalOn;
    }

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

void Error_Handler(void) {
    screen[0] = 129;
    screen[1] = 66;
    screen[2] = 36;
    screen[3] = 24;
    screen[4] = 24;
    screen[5] = 36;
    screen[6] = 66;
    screen[7] = 129;
    while (1) {}
}
