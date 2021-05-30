#include "lcd.h"

const uint8_t ROW_16[] = {0x00, 0x40, 0x10, 0x50};
extern int screen[8];
extern volatile int timerCount;
extern RTC_HandleTypeDef hrtc;
extern int cursorPosition;

static Lcd_HandleTypeDef lcd;
RTC_DateTypeDef sDate;
RTC_TimeTypeDef sTime;

static void waitMls(int mls) {
    int timerState = timerCount;
    while (timerCount - timerState < mls + 1);
}

void INIT_LCD() {
    static Lcd_PortType ports[] = {
            D4_GPIO_Port, D5_GPIO_Port, D6_GPIO_Port, D7_GPIO_Port};

    static Lcd_PinType pins[] = {
            D4_Pin, D5_Pin, D6_Pin, D7_Pin};

    lcd = Lcd_create(ports, pins, RS_GPIO_Port, RS_Pin, EN_GPIO_Port, EN_Pin, RW_GPIO_Port, RW_Pin);
}

Lcd_HandleTypeDef Lcd_create(
		Lcd_PortType port[], Lcd_PinType pin[],
		Lcd_PortType rs_port, Lcd_PinType rs_pin,
		Lcd_PortType en_port, Lcd_PinType en_pin,
        Lcd_PortType rw_port, Lcd_PinType rw_pin)
{
	lcd.en_pin = en_pin;
	lcd.en_port = en_port;

	lcd.rs_pin = rs_pin;
	lcd.rs_port = rs_port;

	lcd.rw_pin = rw_pin;
	lcd.rw_port = rw_port;

	lcd.data_pin = pin;
	lcd.data_port = port;

    GPIO_LCD_INIT();

	Lcd_init();

    ENABLE_LIGHT

    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);

    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BCD);

    Lcd_clear();
    char buffer[16];
    sprintf(buffer, "    %02d:%02d:%02d", sTime.Hours, sTime.Minutes, sTime.Seconds);
    Lcd_string(buffer);
	return lcd;
}


/**
 * Initialize 16x2-lcd without cursor
 */

void GPIO_LCD_INIT() {
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOCEN;

    RW_SET_MODER    // pixel RW
    D4_SET_MODER    // pixel D4
    D5_SET_MODER    // pixel D5
    D6_SET_MODER    // pixel D6
    D7_SET_MODER    // pixel D7
    RS_SET_MODER    // pixel RS
    EN_SET_MODER    // pixel EN
    LIGHT_SET_MODER // pixel Light
}

void Lcd_init()
{
    GPIOB->BSRR |= GPIO_BSRR_BR_3;
    GPIOB->BSRR |= GPIO_BSRR_BR_12;
    waitMls(100);

    lcd_write(0x3);
    waitMls(10);

    lcd_write(0x3);
    waitMls(10);

    lcd_write(0x3);
    waitMls(10);

    lcd_write(0x2);
    waitMls(10);

    lcd_write_command(0x20);
    lcd_write_command(0x08);
    lcd_write_command(0x0C);
    lcd_write_command(0b00000110);

    lcd_write_command(0x40);
    lcd_write_data(0b00000000);
    lcd_write_data(0b00000100);
    lcd_write_data(0b00001110);
    lcd_write_data(0b00001110);
    lcd_write_data(0b00001110);
    lcd_write_data(0b00011111);
    lcd_write_data(0b00000100);
    lcd_write_data(0b00000000);
}

/**
 * Write a string on the current position
 */
void Lcd_string(char * string)
{
	for(int i = 0; i < strlen(string); i++)
	{
		lcd_write_data(string[i]);
	}
}

void drawDigit(int digit) {
    char buffer[3];
    sprintf(buffer, "%d", digit);
    Lcd_string(buffer);
    Lcd_cursor(0, cursorPosition);
}

void drawOnlyChanges(RTC_TimeTypeDef *time) {
    if (sTime.Hours / 10 != time->Hours / 10) {
        cursorPosition = HOURS_T;
        Lcd_cursor(0, cursorPosition);
        drawDigit(sTime.Hours / 10);
    }
    if (sTime.Hours % 10 != time->Hours % 10) {
        cursorPosition = HOURS_U;
        Lcd_cursor(0, cursorPosition);
        drawDigit(sTime.Hours % 10);
    }
    if (sTime.Minutes / 10 != time->Minutes / 10) {
        cursorPosition = MINUTES_T;
        Lcd_cursor(0, cursorPosition);
        drawDigit(sTime.Minutes / 10);
    }
    if (sTime.Minutes % 10 != time->Minutes % 10) {
        cursorPosition = MINUTES_U;
        Lcd_cursor(0, cursorPosition);
        drawDigit(sTime.Minutes % 10);
    }
    if (sTime.Seconds / 10 != time->Seconds / 10) {
        cursorPosition = SECONDS_T;
        Lcd_cursor(0, cursorPosition);
        drawDigit(sTime.Seconds / 10);
    }
    if (sTime.Seconds % 10 != time->Seconds % 10) {
        cursorPosition = SECONDS_U;
        Lcd_cursor(0, cursorPosition);
        drawDigit(sTime.Seconds % 10);
    }
}

/**
 * Set the cursor position
 */
void Lcd_cursor(uint8_t row, uint8_t col)
{
	lcd_write_command(SET_DDRAM_ADDR + ROW_16[row] + col);
}

/**
 * Clear the screen
 */
void Lcd_clear() {
	lcd_write_command(CLEAR_DISPLAY);
}

int checkForBusy() {
    HAL_GPIO_WritePin(lcd.rs_port, lcd.rs_pin, LCD_COMMAND_REG);
    HAL_GPIO_WritePin(lcd.rw_port, lcd.rw_pin, LCD_DATA_REG);			// Write to data register

    lcd_write(0);
    lcd_write(0);

    HAL_GPIO_WritePin(lcd.rw_port, lcd.rw_pin, LCD_COMMAND_REG);
    return HAL_GPIO_ReadPin(lcd.rw_port, lcd.rw_pin) == GPIO_PIN_SET;
}

void lcd_write_command(uint8_t command)
{
	HAL_GPIO_WritePin(lcd.rs_port, lcd.rs_pin, LCD_COMMAND_REG);		// Write to command register

    lcd_write(command >> 4);
    lcd_write(command & 0x0F);
}

void lcd_write_data(uint8_t data)
{
	HAL_GPIO_WritePin(lcd.rs_port, lcd.rs_pin, LCD_DATA_REG);			// Write to data register

    lcd_write(data >> 4);
    lcd_write(data & 0x0F);
}


void lcd_write(uint8_t data)
{
	for(int i = 0; i < 4; i++) {
		HAL_GPIO_WritePin(lcd.data_port[i], lcd.data_pin[i], (data >> i) & 0x01);
	}

	HAL_GPIO_WritePin(lcd.en_port, lcd.en_pin, 1);

    for (int i = 0; i < SystemCoreClock / 100000; ++i) {}

	HAL_GPIO_WritePin(lcd.en_port, lcd.en_pin, 0); 		// Data receive on falling edge
}
