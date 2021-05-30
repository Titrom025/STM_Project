#include "screen.h"
#include "main.h"

void LCD_INIT() {
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOCEN;
    GPIOC->OSPEEDR |= GPIO_SPEED_FREQ_HIGH;

    RW_SET_MODER    // pixel RW
    D4_SET_MODER    // pixel D4
    D5_SET_MODER    // pixel D5
    D6_SET_MODER    // pixel D6
    D7_SET_MODER    // pixel D7
    RS_SET_MODER    // pixel RS
    EN_SET_MODER    // pixel EN
    LIGHT_SET_MODER // pixel Light

    initScreeen();
}

void waitMls(int mls) {
    int timerState = timerCount;
    while (timerCount - timerState < mls + 1);
}

void enablePulse() {
    ENABLE_EN
    waitMls(1);
    DISABLE_EN
            DISABLE_ALL
    waitMls(1);
}

void initScreeen() {
    waitMls(200);

    ENABLE_D5
    enablePulse();

    ENABLE_D5
    enablePulse();
    ENABLE_NOTHING
    enablePulse();

    ENABLE_NOTHING
    enablePulse();
    ENABLE_D5
            ENABLE_D6
    ENABLE_D7
    enablePulse();

    ENABLE_LIGHT
}

void writeScreen() {

    ENABLE_RS
            ENABLE_D6
    enablePulse();
    ENABLE_RS
            ENABLE_D7
    enablePulse();

    ENABLE_RS
            ENABLE_D6
    enablePulse();
    ENABLE_RS
            ENABLE_D6
    ENABLE_D4
    enablePulse();

    ENABLE_RS
            ENABLE_D6
    enablePulse();
    ENABLE_RS
            ENABLE_D7
    enablePulse();

    ENABLE_RS
            ENABLE_D6
    enablePulse();
    ENABLE_RS
            ENABLE_D6
    ENABLE_D4
    enablePulse();

    ENABLE_RS
            ENABLE_D5
    enablePulse();
    ENABLE_RS
    enablePulse();

    ENABLE_RS
            ENABLE_D7
    ENABLE_D6
            ENABLE_D5
    enablePulse();
    ENABLE_RS
            ENABLE_D7
    ENABLE_D6
            ENABLE_D5
    ENABLE_D4
    enablePulse();
}