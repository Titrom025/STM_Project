#include "lcd.h"
#include "main.h"
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

RTC_TimeTypeDef editTime;

bool editMode = false;
volatile int cursorPosition = 0;

void enterEditMode() {
    cursorPosition = 11;
    lcd_write_command(0b00001110);
    Lcd_cursor(0, cursorPosition);
    editMode = true;
}

void exitEditMode() {
    lcd_write_command(0b00001100);
    if (HAL_RTC_SetTime(&hrtc, &editTime, RTC_FORMAT_BIN) != HAL_OK) { Error_Handler(); }
    editMode = false;
}

void drawDigit(int digit) {
    char buffer[3];
    sprintf(buffer, "%d", digit);
    Lcd_string(buffer);
    Lcd_cursor(0, cursorPosition);
}

void changeValue(bool up) {
    int diff = 1;
    if (up == false) 
        diff = -1;

    switch (cursorPosition) {
        case HOURS_T: {
            editTime.Hours = ((editTime.Hours / 10 * 10 + 10 * diff + 30) % 30) + ((editTime.Hours) % 10);
            if (editTime.Hours > 23) {
                editTime.Hours = 20;
                drawDigit(editTime.Hours);
            } else {
                drawDigit(editTime.Hours / 10);
            }
            break;
        }
        case HOURS_U: {
            if (editTime.Hours / 10 == 2) {
                editTime.Hours = (editTime.Hours / 10) * 10 + ((editTime.Hours + 4 + diff) % 4);
            } else {
                editTime.Hours = (editTime.Hours / 10) * 10 + ((editTime.Hours + 10 + diff) % 10);
            }
            drawDigit(editTime.Hours % 10);
            break;
        }
        case MINUTES_T: {
            editTime.Minutes = ((editTime.Minutes / 10 * 10 + 10 * diff + 60) % 60) + ((editTime.Minutes) % 10);
            drawDigit(editTime.Minutes / 10);
            break;
        }
        case MINUTES_U: {
            editTime.Minutes = (editTime.Minutes / 10) * 10 + ((editTime.Minutes + 10 + diff) % 10);
            drawDigit(editTime.Minutes % 10);
            break;
        }
        case SECONDS_T: {
            editTime.Seconds = ((editTime.Seconds / 10 * 10 + 10 * diff + 60) % 60) + ((editTime.Seconds) % 10);
            drawDigit(editTime.Seconds / 10);
            break;
        }

        case SECONDS_U: {
            editTime.Seconds = (editTime.Seconds / 10) * 10 + ((editTime.Seconds + 10 + diff) % 10);
            drawDigit(editTime.Seconds % 10);
            break;
        }
    }
}

void SysTick_Handler() {
    timerCount++;
    updateButton();
    buttonPressed = returnButton();

    if (buttonPressed == NO_BUTTON) {
        lastPressed = NO_BUTTON;
        GPIOC->BSRR |= GPIO_BSRR_BR_8;
        return;
    }

    if (timerCount - led_buttons_count[buttonPressed] > TIMER_LIMIT && lastPressed != buttonPressed) {
        led_states[buttonPressed] = !led_states[buttonPressed];
        led_buttons_count[buttonPressed] = timerCount;
        lastPressed = buttonPressed;
    } else {
        return;
    }

    if (buttonPressed == TOP_BUTTON) {
        changeValue(true);

    } else if (buttonPressed == DOWN_BUTTON) {
        changeValue(false);

    } else if (buttonPressed == LEFT_BUTTON) {
        cursorPosition--;
        Lcd_cursor(0, cursorPosition);
    } else if (buttonPressed == RIGHT_BUTTON) {
        cursorPosition++;
        Lcd_cursor(0, cursorPosition);
    } else if (buttonPressed == USER_BUTTON) {
        if (editMode) {
            exitEditMode();
        } else {
            enterEditMode();
        }
    }
}

void drawOnlyChanges() {
    if (sTime.Hours / 10 != editTime.Hours / 10) {
        cursorPosition = HOURS_T;
        Lcd_cursor(0, cursorPosition);
        drawDigit(sTime.Hours / 10);
    }
    if (sTime.Hours % 10 != editTime.Hours % 10) {
        cursorPosition = HOURS_U;
        Lcd_cursor(0, cursorPosition);
        drawDigit(sTime.Hours % 10);
    }
    if (sTime.Minutes / 10 != editTime.Minutes / 10) {
        cursorPosition = MINUTES_T;
        Lcd_cursor(0, cursorPosition);
        drawDigit(sTime.Minutes / 10);
    }
    if (sTime.Minutes % 10 != editTime.Minutes % 10) {
        cursorPosition = MINUTES_U;
        Lcd_cursor(0, cursorPosition);
        drawDigit(sTime.Minutes % 10);
    }
    if (sTime.Seconds / 10 != editTime.Seconds / 10) {
        cursorPosition = SECONDS_T;
        Lcd_cursor(0, cursorPosition);
        drawDigit(sTime.Seconds / 10);
    }
    if (sTime.Seconds % 10 != editTime.Seconds % 10) {
        cursorPosition = SECONDS_U;
        Lcd_cursor(0, cursorPosition);
        drawDigit(sTime.Seconds % 10);
        screen[7] = 255;
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

    if (editMode == false) {
        drawOnlyChanges();
        editTime.Seconds = sTime.Seconds;
        editTime.Minutes = sTime.Minutes;
        editTime.Hours = sTime.Hours;
//        Lcd_clear();
//        char buffer[16];
//        sprintf(buffer, "    %02d:%02d:%02d", sTime.Hours, sTime.Minutes, sTime.Seconds);
//        Lcd_string(buffer);
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
