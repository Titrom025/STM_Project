#include "IRQ_Handlers.h"
#include "rtc.h"
#include "main.h"
#include "lcd.h"

void SET_SYSTICK_TIMES(uint32_t timesInSecond) {
    SystemCoreClockUpdate();
    SysTick->LOAD =  SystemCoreClock / timesInSecond - 1;
    SysTick->VAL = SystemCoreClock / timesInSecond - 1;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk;
}

//void GPI_INIT() {
//    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
//    SPI2->CR1 = SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_BR | SPI_CR1_MSTR | SPI_CR1_CPOL | SPI_CR1_CPHA;
//    SPI2->CR2 = SPI_CR2_DS | SPI_CR2_RXNEIE;
//    SPI2->CR1 |= SPI_CR1_SPE;
//
//    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
//
//    GPIOB->AFR[1] |= (0 << 4 * (13 - 8)) | (0 << 4 * (15 - 8));
//    GPIOB->MODER |= GPIO_MODER_MODER13_1 | GPIO_MODER_MODER15_1;
//
//    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
//    GPIOA->MODER |= GPIO_MODER_MODER8_0;
//}

void BUTTONS_INIT() {
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

    GPIOC->MODER |= GPIO_MODER_MODER6_0;
    GPIOC->MODER |= GPIO_MODER_MODER7_0;
    GPIOC->MODER |= GPIO_MODER_MODER8_0;
    GPIOC->MODER |= GPIO_MODER_MODER9_0;

    GPIOC->MODER |= GPIO_MODER_MODER12_0;
    GPIOA->MODER |= GPIO_MODER_MODER15_0;

    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR0_1;
    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR4_1;
    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR5_1;
}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
    RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) { Error_Handler(); }

    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK| RCC_CLOCKTYPE_SYSCLK |RCC_CLOCKTYPE_PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) { Error_Handler(); }

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) { Error_Handler(); }
}

static void USART_init(){
    /* (1) Oversampling by 16, 9600 baud */
    /* (2) Single-wire half-duplex mode */
    /* (3) 8 data bit, 1 start bit, 1 stop bit, no parity, reception and
                 transmission enabled */
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    RCC->AHBENR |= RCC_AHBENR_GPIODEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
    GPIOD->MODER |= GPIO_MODER_MODER2_1;
    GPIOC->MODER |= GPIO_MODER_MODER10_1 | GPIO_MODER_MODER11_1;
    GPIOC->AFR[1] |= 1 << (4 * (10 - 8)) | 1 << (4 * (11 - 8));
    GPIOD->AFR[0] |= 1 << (4 * (2 - 0));
    USART3->BRR = 480000 / 96;
    USART3->CR3 |= USART_CR3_OVRDIS | USART_CR3_DEM;

    USART3->CR1 |= USART_CR1_DEAT_0 | USART_CR1_DEAT_1 | USART_CR1_DEAT_2 | USART_CR1_DEAT_3 | USART_CR1_DEAT_4;
    USART3->CR1 |= USART_CR1_DEDT_0 | USART_CR1_DEDT_1 | USART_CR1_DEDT_2 | USART_CR1_DEDT_3 | USART_CR1_DEDT_4;
    USART3->CR1 |=  USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
    while((USART3->ISR & USART_ISR_TC) == 0);
    NVIC_EnableIRQ(USART3_4_IRQn);
    USART3->CR1 |= USART_CR1_TCIE;
    USART3->CR1 |= USART_CR1_RXNEIE;
    USART3->TDR = 0x4;
}

int main(void) {
    SystemClock_Config();
    SET_SYSTICK_TIMES(1000);

#ifdef BUTTONS_CONTROLLER
    BUTTONS_INIT();
#endif

#ifdef SCREEN_CONTROLLER
    RTC_Init();
    INIT_LCD();
#endif

    USART_init();

    while(1) {}
}

