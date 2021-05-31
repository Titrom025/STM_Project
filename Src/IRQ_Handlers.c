#include "main.h"
#include "rtc.h"
#include "lcd.h"
#include "IRQ_Handlers.h"
#include "button_lib.h"

#define TIMER_LIMIT 100

volatile int timerCount = 0;
volatile int buttonPressed = -1;
volatile static bool led_states[5];
volatile static int led_buttons_count[5];
int lastPressed = NO_BUTTON;

uint8_t incoming_messages = 0;
uint8_t outcoming_messages = 0;

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
    HAL_RTC_SetTime(&hrtc, &editTime, RTC_FORMAT_BIN);
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
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
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

    #ifdef BUTTONS_CONTROLLER
        updateButton();
        alarmDetected = incoming_messages;
        outcoming_messages = returnButton();
    #endif

    #ifdef SCREEN_CONTROLLER
    buttonPressed = incoming_messages;
    outcoming_messages = alarmDetected;
    handleButton();
#endif
}


void USART3_4_IRQHandler(void){
    if (USART3->ISR & USART_ISR_TC){
        USART3->ICR |= USART_ICR_TCCF;
        USART3->ICR |= USART_ICR_IDLECF;
    } else if (USART3->ISR & USART_ISR_RXNE){
        incoming_messages = (uint8_t)USART3->RDR;
        USART3->TDR = outcoming_messages;
    }
}

void RTC_IRQHandler(void) {
#ifdef SCREEN_CONTROLLER
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);

    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BCD);

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
#endif

#ifdef BUTTONS_CONTROLLER
    if (alarmDetected) {
        if (alarmSignalOn) {
            GPIOC->BSRR = GPIO_BSRR_BR_7; // GPIO_BSRR_BR_7;
        } else {
            GPIOC->BSRR = GPIO_BSRR_BS_7; // GPIO_BSRR_BR_7;
        }
        alarmSignalOn = !alarmSignalOn;
    } else {
        GPIOC->BSRR = GPIO_BSRR_BR_7; // GPIO_BSRR_BR_7;
    }
#endif
    HAL_RTC_AlarmIRQHandler(&hrtc);
}

void handleButton() {
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
            GPIOC->BSRR = GPIO_BSRR_BR_7;
            alarmDetected = false;
            alarmOn = false;
            cursorPosition = 14;
            Lcd_cursor(0, cursorPosition);
            Lcd_string(" ");
        } else if (editMode) {
            editMode = false;
            alarmMode = true;
            enterAlarmMode();
        } else if (alarmMode) {
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
            updateSelectedValue(&editTime, cursorPosition, true);
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
                updateSelectedValue(&alarmTime, cursorPosition, true);
            }
        }

    } else if (buttonPressed == DOWN_BUTTON) {
        if (editMode) {
            updateSelectedValue(&editTime, cursorPosition, false);
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
                updateSelectedValue(&alarmTime, cursorPosition, false);
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