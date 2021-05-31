#ifndef LCD_H_
#define LCD_H_

#include "stm32f0xx_hal.h"
#include "string.h"
#include "stdio.h"

#define Lcd_PortType GPIO_TypeDef*
#define Lcd_PinType uint16_t

typedef struct {
	Lcd_PortType * data_port;
	Lcd_PinType * data_pin;

	Lcd_PortType rs_port;
	Lcd_PinType rs_pin;

    Lcd_PortType rw_port;
    Lcd_PinType rw_pin;

	Lcd_PortType en_port;
	Lcd_PinType en_pin;
} Lcd_HandleTypeDef;

Lcd_HandleTypeDef Lcd_create(
        Lcd_PortType port[], Lcd_PinType pin[],
        Lcd_PortType rs_port, Lcd_PinType rs_pin,
        Lcd_PortType en_port, Lcd_PinType en_pin,
        Lcd_PortType rw_port, Lcd_PinType rw_pin);

void INIT_LCD();
static void waitMls(int mls);
void GPIO_LCD_INIT();
void Lcd_init();
void Lcd_string(char * string);
void Lcd_cursor(uint8_t row, uint8_t col);

void Lcd_clear();
void drawDigit(int digit);
void drawOnlyChanges(RTC_TimeTypeDef *time);

void lcd_write_data(uint8_t data);
void lcd_write_command(uint8_t command);
void lcd_write(uint8_t data);

#define RS_Pin GPIO_PIN_12
#define RS_GPIO_Port GPIOB
#define RW_Pin GPIO_PIN_3
#define RW_GPIO_Port GPIOB
#define EN_Pin GPIO_PIN_4
#define EN_GPIO_Port GPIOC
#define D4_Pin GPIO_PIN_4
#define D4_GPIO_Port GPIOB
#define D5_Pin GPIO_PIN_5
#define D5_GPIO_Port GPIOB
#define D6_Pin GPIO_PIN_6
#define D6_GPIO_Port GPIOB
#define D7_Pin GPIO_PIN_7
#define D7_GPIO_Port GPIOB

#define LIGHT_Pin GPIO_PIN_6
#define LIGHT_GPIO_Port GPIOС


#define RW_SET_MODER (GPIOB->MODER |= GPIO_MODER_MODER3_0);         // Init RW
#define D4_SET_MODER (GPIOB->MODER |= GPIO_MODER_MODER4_0);         // Init D4
#define D5_SET_MODER (GPIOB->MODER |= GPIO_MODER_MODER5_0);         // Init D5
#define D6_SET_MODER (GPIOB->MODER |= GPIO_MODER_MODER6_0);         // Init D6
#define D7_SET_MODER (GPIOB->MODER |= GPIO_MODER_MODER7_0);         // Init D7
#define RS_SET_MODER (GPIOB->MODER |= GPIO_MODER_MODER12_0);        // Init RS
#define EN_SET_MODER (GPIOC->MODER |= GPIO_MODER_MODER4_0);         // Init EN
#define LIGHT_SET_MODER (GPIOC->MODER |= GPIO_MODER_MODER6_0);      // Init Light
#define ENABLE_LIGHT (GPIOC->BSRR = GPIO_BSRR_BS_6);


#define HOURS_T 4
#define HOURS_U 5
#define MINUTES_T 7
#define MINUTES_U 8
#define SECONDS_T 10
#define SECONDS_U 11

#endif
