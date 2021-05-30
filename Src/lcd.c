#include "lcd.h"

const uint8_t ROW_16[] = {0x00, 0x40, 0x10, 0x50};
extern int screen[8];
extern volatile int timerCount;

static Lcd_HandleTypeDef lcd;

static void waitMls(int mls) {
    int timerState = timerCount;
    while (timerCount - timerState < mls + 1);
}

void INIT_LCD_IRQ() {
    static Lcd_PortType ports[] = {
            D4_GPIO_Port, D5_GPIO_Port, D6_GPIO_Port, D7_GPIO_Port};

    static Lcd_PinType pins[] = {
            D4_Pin, D5_Pin, D6_Pin, D7_Pin};

    lcd = Lcd_create(ports, pins, RS_GPIO_Port, RS_Pin, EN_GPIO_Port, EN_Pin, RW_GPIO_Port, RW_Pin);
//    Lcd_clear();
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
	return lcd;
}


/**
 * Initialize 16x2-lcd without cursor
 */

void GPIO_LCD_INIT() {
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

void Lcd_init()
{
    waitMls(100);
    lcd_write(0x2);
    lcd_write_command(0b00100000);
    lcd_write_command(0b00001100);
    lcd_write_command(0b00000110);
}

/**
 * Write a number on the current position
 */
void Lcd_int(int number)
{
    Lcd_clear();
	char buffer[16];
	sprintf(buffer, "    HEHE: %d", number);
	Lcd_string(buffer);
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

void Lcd_define_char(uint8_t code, uint8_t bitmap[]){
	lcd_write_command(SETCGRAM_ADDR + (code << 3));
	for(uint8_t i=0;i<8;++i){
		lcd_write_data(bitmap[i]);
	}
}

int checkForBusy() {
    HAL_GPIO_WritePin(lcd.rs_port, lcd.rs_pin, LCD_COMMAND_REG);
    HAL_GPIO_WritePin(lcd.rw_port, lcd.rw_pin, LCD_DATA_REG);			// Write to data register

    lcd_write(0);
    lcd_write(0);

    HAL_GPIO_WritePin(lcd.rw_port, lcd.rw_pin, LCD_COMMAND_REG);
    return HAL_GPIO_ReadPin(lcd.rw_port, lcd.rw_pin) == GPIO_PIN_SET;
}

int getCurrentAC() {
    HAL_GPIO_WritePin(lcd.rs_port, lcd.rs_pin, LCD_COMMAND_REG);
    HAL_GPIO_WritePin(lcd.rw_port, lcd.rw_pin, LCD_DATA_REG);			// Write to data register

//    lcd_write(0);
    int bit6 = 0; //HAL_GPIO_ReadPin(lcd.data_port[2], lcd.data_pin[2]);
    int bit5 = 0; //HAL_GPIO_ReadPin(lcd.data_port[1], lcd.data_pin[1]);
    int bit4 = 0; //HAL_GPIO_ReadPin(lcd.data_port[0], lcd.data_pin[0]);
//    lcd_write(0);
    int bit3 = HAL_GPIO_ReadPin(lcd.data_port[3], lcd.data_pin[3]);
    int bit2 = HAL_GPIO_ReadPin(lcd.data_port[2], lcd.data_pin[2]);
    int bit1 = HAL_GPIO_ReadPin(lcd.data_port[1], lcd.data_pin[1]);
    int bit0 = HAL_GPIO_ReadPin(lcd.data_port[0], lcd.data_pin[0]);

    HAL_GPIO_WritePin(lcd.rw_port, lcd.rw_pin, LCD_COMMAND_REG);
    return (bit6 << 6) | (bit5 << 5) | (bit4 << 4) | (bit3 << 3) | (bit2 << 2) | (bit1 << 1) | bit0;
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
