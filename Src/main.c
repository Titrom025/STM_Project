#include "main.h"

uint8_t screen[8] = {0};
volatile static int timerCount = 0;

void SET_SYSTICK_TIMES(uint32_t timesInSecond) {
    SystemCoreClockUpdate();
    SysTick->LOAD =  SystemCoreClock / timesInSecond - 1;
    SysTick->VAL = SystemCoreClock / timesInSecond - 1;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk;
}

void GPI_INIT() {
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

void PIXEL_SCREEN_INIT() {
    GPI_INIT();
    NVIC_EnableIRQ(SPI2_IRQn);
    SPI2->DR = 0;
}

void initPeriphery() {
    // pixel GPIOB, GPIOC
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

RTC_HandleTypeDef hrtc;


void RTC_IRQHandler(void)
{
    static int val = 0;
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    GPIOC->MODER |= GPIO_MODER_MODER9_0;
    if (val) {
        GPIOC->BSRR = GPIO_BSRR_BR_9;
    } else {
        GPIOC->BSRR = GPIO_BSRR_BS_9;
    }
    val = !val;

    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;

    if (HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK) { Error_Handler(); }

    if (HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK) { Error_Handler(); }

    screen[1] = sTime.Minutes;
    screen[0] = sTime.Seconds;

    HAL_RTC_AlarmIRQHandler(&hrtc);
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

static void MX_RTC_Init(void)
{
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    RTC_AlarmTypeDef sAlarm = {0};

    /** Initialize RTC Only
    */
    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv = 127;
    hrtc.Init.SynchPrediv = 319;
    hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
    hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
    if (HAL_RTC_Init(&hrtc) != HAL_OK) { Error_Handler(); }

    /** Initialize RTC and set the Time and Date
    */
    sTime.Hours = 0x0;
    sTime.Minutes = 0x0;
    sTime.Seconds = 0x0;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;
//    if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK) { Error_Handler(); }

    sDate.WeekDay = RTC_WEEKDAY_MONDAY;
    sDate.Month = RTC_MONTH_JANUARY;
    sDate.Date = 0x1;
    sDate.Year = 0x0;

//    if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK) { Error_Handler();}
    /** Enable the Alarm A
    */
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
    if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK) { Error_Handler(); }
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

int main(void) {
    PIXEL_SCREEN_INIT();
    SET_SYSTICK_TIMES(1000);
//    rtcInit();

    SystemClock_Config();
    MX_RTC_Init();

//    RTC->CR |= RTC_CR_
//    initPeriphery();
//    initScreeen();
//    writeScreen();
    while(1) {}
}


