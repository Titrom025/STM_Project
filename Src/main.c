#include "main.h"
#define TIMER_LIMIT 50

volatile static bool led_states[4];
volatile static int led_buttons_count[4];

volatile static int timerCount = 0;

volatile int direction = -1;

void ticker(uint32_t timesInSecond) {
    SystemCoreClockUpdate();
    SysTick->LOAD =  SystemCoreClock / timesInSecond - 1;
    SysTick->VAL = SystemCoreClock / timesInSecond - 1;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk;
}

void Init() {
     // PC12 - output 1
     // PA15 - output 2
     // PA4 - input 1
     // PA5 - input 2

     RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

     GPIOC->MODER |= GPIO_MODER_MODER12_0;
     GPIOA->MODER |= GPIO_MODER_MODER15_0;

     GPIOA->PUPDR |= GPIO_PUPDR_PUPDR4_1;
     GPIOA->PUPDR |= GPIO_PUPDR_PUPDR5_1;

     ticker(1000);
}


void initPeriphery() {
    // Init GPIOB, GPIOC
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOCEN;
    GPIOC->OSPEEDR |= GPIO_SPEED_FREQ_HIGH;

    RW_SET_MODER    // Init RW
    D4_SET_MODER    // Init D4
    D5_SET_MODER    // Init D5
    D6_SET_MODER    // Init D6
    D7_SET_MODER    // Init D7
    RS_SET_MODER    // Init RS
    EN_SET_MODER    // Init EN
    LIGHT_SET_MODER // Init Light
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
void SysTick_Handler() {
    timerCount++;
}

int main(void) {
    Init();
    initPeriphery();
    initScreeen();
    writeScreen();
    GPIOC->MODER |= GPIO_MODER_MODER8_0;
    GPIOC->BSRR |= GPIO_BSRR_BS_8;
    while(1) {}
}
