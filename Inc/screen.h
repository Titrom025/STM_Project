#ifndef STM_PROJECT_SCREEN_H
#define STM_PROJECT_SCREEN_H

#define RW_SET_MODER (GPIOB->MODER |= GPIO_MODER_MODER3_0);         // Init RW
#define D4_SET_MODER (GPIOB->MODER |= GPIO_MODER_MODER4_0);         // Init D4
#define D5_SET_MODER (GPIOB->MODER |= GPIO_MODER_MODER5_0);         // Init D5
#define D6_SET_MODER (GPIOB->MODER |= GPIO_MODER_MODER6_0);         // Init D6
#define D7_SET_MODER (GPIOB->MODER |= GPIO_MODER_MODER7_0);         // Init D7
#define RS_SET_MODER (GPIOB->MODER |= GPIO_MODER_MODER12_0);        // Init RS
#define EN_SET_MODER (GPIOC->MODER |= GPIO_MODER_MODER4_0);         // Init EN
#define LIGHT_SET_MODER (GPIOC->MODER |= GPIO_MODER_MODER6_0);      // Init Light

#define ENABLE_NOTHING
#define ENABLE_RW (GPIOB->BSRR = GPIO_BSRR_BS_3);                   // ENABLE RW
#define ENABLE_D4 (GPIOB->BSRR = GPIO_BSRR_BS_4);                   // ENABLE D4
#define ENABLE_D5 (GPIOB->BSRR = GPIO_BSRR_BS_5);                   // ENABLE D5
#define ENABLE_D6 (GPIOB->BSRR = GPIO_BSRR_BS_6);                   // ENABLE D6
#define ENABLE_D7 (GPIOB->BSRR = GPIO_BSRR_BS_7);                   // ENABLE D7
#define ENABLE_RS (GPIOB->BSRR = GPIO_BSRR_BS_12);                  // ENABLE RS
#define ENABLE_EN (GPIOC->BSRR = GPIO_BSRR_BS_4);                   // ENABLE EN
#define ENABLE_LIGHT (GPIOC->BSRR = GPIO_BSRR_BS_6);                // ENABLE Light

#define DISABLE_ALL DISABLE_RS DISABLE_RW DISABLE_D7 DISABLE_D6 DISABLE_D5 DISABLE_D4
#define DISABLE_RW (GPIOB->BSRR = GPIO_BSRR_BR_3);                  // DISABLE RW
#define DISABLE_D4 (GPIOB->BSRR = GPIO_BSRR_BR_4);                  // DISABLE D4
#define DISABLE_D5 (GPIOB->BSRR = GPIO_BSRR_BR_5);                  // DISABLE D5
#define DISABLE_D6 (GPIOB->BSRR = GPIO_BSRR_BR_6);                  // DISABLE D6
#define DISABLE_D7 (GPIOB->BSRR = GPIO_BSRR_BR_7);                  // DISABLE D7
#define DISABLE_RS (GPIOB->BSRR = GPIO_BSRR_BR_12);                 // DISABLE RS
#define DISABLE_EN (GPIOC->BSRR = GPIO_BSRR_BR_4);                  // DISABLE EN
#define DISABLE_LIGHT (GPIOC->BSRR = GPIO_BSRR_BR_6);               // DISABLE Light

#define BIT_7_MASK                                               (0x80)
#define BIT_6_MASK                                               (0x40)
#define BIT_5_MASK                                               (0x20)
#define BIT_4_MASK                                               (0x10)
#define BIT_3_MASK                                               (0x08)
#define BIT_2_MASK                                               (0x04)
#define BIT_1_MASK                                               (0x02)
#define BIT_0_MASK                                               (0x01)

void LCD_INIT();
void waitMls(int mls);
void enablePulse();
void initScreeen();
void writeScreen();

#endif //STM_PROJECT_SCREEN_H
