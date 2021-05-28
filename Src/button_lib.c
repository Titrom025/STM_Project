#include "button_lib.h"
#include "main.h"
#include "stdbool.h"

volatile int button = NO_BUTTON;

void updateButton() {
    static bool input1 = false;

    if (input1) {
        GPIOA->BSRR = GPIO_BSRR_BS_15;

        if (GPIOA->IDR & GPIO_IDR_4) {
        	button = TOP;
            return;
        } else if (GPIOA->IDR & GPIO_IDR_5) {
        	button = DOWN;
            return;
        } else {
        	button = NO_BUTTON;
        }

        GPIOA->BSRR = GPIO_BSRR_BR_15;

    } else {
        GPIOC->BSRR = GPIO_BSRR_BS_12;

        if (GPIOA->IDR & GPIO_IDR_4) {
        	button = LEFT;
            return;
        } else if (GPIOA->IDR & GPIO_IDR_5) {
        	button = RIGHT;
            return;
        } else {
        	button = NO_BUTTON;
        }

        GPIOC->BSRR = GPIO_BSRR_BR_12;
    }

    input1 = !input1;
}

int returnButton() {
	return button;
}
