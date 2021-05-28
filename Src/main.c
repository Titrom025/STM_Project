#include "main.h"

uint8_t screen[8] = {0};
volatile static int timerCount = 0;

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

void Init() {
    GPI_init();

    ticker(1000);
    NVIC_EnableIRQ(SPI2_IRQn);

    SPI2->DR = 0;
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
    screen[3] = (RTC->TR & RTC_TR_MNT) >> 12;
    screen[2] = (RTC->TR & RTC_TR_MNU) >> 8;
    screen[1] = (RTC->TR & RTC_TR_ST) >> 4;
    screen[0] = RTC->TR & RTC_TR_SU;
}

void RTC_Update(int volatile value)
{
    // Выключаем защиту от записи
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;

    // Входим в режим редактирования
    RTC->ISR |= RTC_ISR_INIT;
    // Ждем подтверждения входа в режим редактирования
    while( !(RTC->ISR & RTC_ISR_INITF) ){};

    // Устанавливаем асинхронный предделитель на 100(99+1).
    RTC->PRER = (uint32_t)(99 << 16);
    // Установим синхронный предделитель на 400(399+1).
    RTC->PRER |= (uint32_t)399;

    // Устанавливаем время
//    TR = ( ((uint32_t)(2/10*16 + 1%10) << 16) | ((uint32_t)(3/10*16 + 2%10) << 8) | ((uint32_t)2/10*16 + 2%10) );
//    RTC->TR = TR;
    // Устанавливаем дату
//    DR = (uint32_t)(2021/10*16 + 2021%10) << 16  | (uint32_t)(7/10*16 + 7%10) << 13 | (uint32_t)(4/10*16 + 4%10) << 8 | (uint32_t)(3/10*16 + 3%10);
//    RTC->DR = DR;

    RTC->ISR &= ~(RTC_ISR_INIT);
    RTC->WPR = 0xFF;
}

void rtcInit() {

//    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
//    PWR->CR |= PWR_CR_DBP;
//
//    RCC->BDCR |= RCC_BDCR_LSEON;
//    while (!(RCC->BDCR & RCC_BDCR_LSERDY)){}
//
//    RCC->BDCR |= RCC_BDCR_RTCSEL_LSE;
//    RCC->BDCR |= RCC_BDCR_RTCEN;

    RCC->CSR |= RCC_CSR_LSION;
    while ( !(RCC->CSR & RCC_CSR_LSIRDY) ){}

    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR |= PWR_CR_DBP;

    RCC->BDCR |= RCC_BDCR_RTCSEL_LSI;
    RCC->BDCR |= RCC_BDCR_RTCEN;
    
    RTC_Update(5);
}

void RTC_IRQHandler() {
    screen[7] = 255;
}

int main(void) {
    Init();
    rtcInit();

//    RTC->CR |= RTC_CR_
//    initPeriphery();
//    initScreeen();
//    writeScreen();
    while(1) {}
}





//RTC->WPR = 0xCA; /* (1) */
//RTC->WPR = 0x53; /* (1) */
//RTC->ISR |= RTC_ISR_INIT; /* (2) */
//while ((RTC->ISR & RTC_ISR_INITF) != RTC_ISR_INITF) /* (3) */ {
///* add time out here for a robust application */
//}
//RTC->PRER = 0x007F0137; /* (4) */ RTC->TR = RTC_TR_PM | Time; /* (5) */ RTC->ISR &=~ RTC_ISR_INIT; /* (6) */ RTC->WPR = 0xFE; /* (7) */
//RTC->WPR = 0x64; /* (7) */