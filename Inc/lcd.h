#ifndef LCD_H_
#define LCD_H_

#include "stm32f0xx_hal.h"
#include "string.h"
#include "stdio.h"

// For row start addresses
extern const uint8_t ROW_16[];

/************************************** Command register **************************************/
#define CLEAR_DISPLAY 0x01

#define RETURN_HOME 0x02

#define ENTRY_MODE_SET 0x04
#define OPT_S	0x01					// Shift entire display to right
#define OPT_INC 0x02					// Cursor increment

#define DISPLAY_ON_OFF_CONTROL 0x08
#define OPT_D	0x04					// Turn on display
#define OPT_C	0x02					// Turn on cursor
#define OPT_B 	0x01					// Turn on cursor blink

#define CURSOR_DISPLAY_SHIFT 0x10		// Move and shift cursor
#define OPT_SC 0x08
#define OPT_RL 0x04

#define FUNCTION_SET 0x20
#define OPT_DL 0x10						// Set interface data length
#define OPT_N 0x08						// Set number of display lines
#define OPT_F 0x04						// Set alternate font
#define SETCGRAM_ADDR 0x040
#define SET_DDRAM_ADDR 0x80				// Set DDRAM address


/************************************** Helper macros **************************************/
#define DELAY(X) HAL_Delay(X)


/************************************** LCD defines **************************************/
#define LCD_COMMAND_LEN 4
#define LCD_BYTE 8
#define LCD_DATA_REG 1
#define LCD_COMMAND_REG 0


/************************************** LCD typedefs **************************************/
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


/************************************** Public functions **************************************/
void INIT_LCD_IRQ();
int getCurrentAC();
static void waitMls(int mls);
void GPIO_LCD_INIT();
void Lcd_init();
void Lcd_int(int number);
void Lcd_string(char * string);
void Lcd_cursor(uint8_t row, uint8_t col);
Lcd_HandleTypeDef Lcd_create(
		Lcd_PortType port[], Lcd_PinType pin[],
		Lcd_PortType rs_port, Lcd_PinType rs_pin,
		Lcd_PortType en_port, Lcd_PinType en_pin,
        Lcd_PortType rw_port, Lcd_PinType rw_pin);
void Lcd_define_char(uint8_t code, uint8_t bitmap[]);
void Lcd_clear();

static void lcd_write_data(uint8_t data);
void lcd_write_command(uint8_t command);
static void lcd_write(uint8_t data);

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
#endif
