#include "main.h"
#define TIMER_LIMIT 50

volatile static bool led_states[4];
volatile static int led_buttons_count[4];

volatile static int timerCount = 0;

uint8_t screen[8] = {0};

volatile int direction = -1;

void ticker(uint32_t timesInSecond) {
    SystemCoreClockUpdate();
    SysTick->LOAD =  SystemCoreClock / timesInSecond - 1;
    SysTick->VAL = SystemCoreClock / timesInSecond - 1;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk;
}

void GPI_init() {
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
    SPI2->CR1 = SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_BR | SPI_CR1_MSTR | SPI_CR1_CPOL | SPI_CR1_CPHA;
    SPI2->CR2 = SPI_CR2_DS | SPI_CR2_RXNEIE;
    SPI2->CR1 |= SPI_CR1_SPE;

    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;


    GPIOB->AFR[1] |= (0 << 4 * (13 - 8)) | (0 << 4 * (15 - 8));
    GPIOB->MODER |= GPIO_MODER_MODER13_1 | GPIO_MODER_MODER15_1;

    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER |= GPIO_MODER_MODER8_0;
}

void Init() {
     // PC12 - output 1
     // PA15 - output 2
     // PA4 - input 1
     // PA5 - input 2

     RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
     RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

     GPIOC->MODER |= GPIO_MODER_MODER12_0;
     GPIOA->MODER |= GPIO_MODER_MODER15_0;

     GPIOA->PUPDR |= GPIO_PUPDR_PUPDR4_1;
     GPIOA->PUPDR |= GPIO_PUPDR_PUPDR5_1;

     screen[5] = 2;
     screen[6] = 7;
     screen[7] = 2;

     GPI_init();

     ticker(1000);

     NVIC_EnableIRQ(SPI2_IRQn);

     SPI2->DR = 0;
}

void SPI2_IRQHandler(void) {
    static int index = 0;
    static bool latch = false;


    if ((SPI2->SR & SPI_SR_RXNE)) {
        if (latch) {
            GPIOA->BSRR = GPIO_BSRR_BR_8;
            SPI2->DR = screen[index] | (1 << (index + 8));
            index = (index + 1) % 8;
            SPI2->DR;

        } else {
            GPIOA->BSRR = GPIO_BSRR_BS_8;
        }

        latch = !latch;
    }
}

void redrawMatrix(int dir) {
    switch(dir) {
        case DOWN: {
            for(int i = 0; i < 8; i++) {
                int leftBit = screen[i] & 0x80;
                screen[i] = (screen[i] << 1) | (leftBit >> 7);
            }
            break;
        }
        case TOP: {
            for(int i = 0; i < 8; i++) {
                int rightBit = screen[i] & 1;
                screen[i] = (screen[i] >> 1) | (rightBit << 7);
            }
            break;
        }
        case RIGHT: {
            int topLine = screen[0];

            for(int i = 0; i < 7; i++) {
                screen[i] = screen[i + 1];
            }
            screen[7] = topLine;
            break;
        }
        case LEFT: {
            int bottomLine = screen[7];

            for(int i = 7; i > 0; i--) {
                screen[i] = screen[i - 1];
            }
            screen[0] = bottomLine;
            break;
        }

        default:
            break;
    }
}

void SysTick_Handler() {
    timerCount++;
    updateButton();
    direction = returnButton();


    if (direction != NO_BUTTON && timerCount - led_buttons_count[direction] > TIMER_LIMIT) {
        led_buttons_count[direction] = timerCount;
        redrawMatrix(direction);
     }
}



void initPeriphery() {
    // Init GPIOB, GPIOC
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOCEN;
    GPIOC->OSPEEDR |= GPIO_SPEED_FREQ_HIGH;

    GPIOB->MODER |= GPIO_MODER_MODER3_0;    // Init RW
    GPIOB->MODER |= GPIO_MODER_MODER4_0;    // Init D4
    GPIOB->MODER |= GPIO_MODER_MODER5_0;    // Init D5
    GPIOB->MODER |= GPIO_MODER_MODER6_0;    // Init D6
    GPIOB->MODER |= GPIO_MODER_MODER7_0;    // Init D7
    GPIOB->MODER |= GPIO_MODER_MODER12_0;   // Init RS

    GPIOC->MODER |= GPIO_MODER_MODER4_0;    // Init EN
    GPIOC->MODER |= GPIO_MODER_MODER6_0;    // Init Light

}

void initScreeen() {

}
int main(void) {
    Init();
    initPeriphery();
    while(1) {}
}