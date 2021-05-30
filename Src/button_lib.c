#include "button_lib.h"
#include "main.h"
#include "stdbool.h"

volatile int button = NO_BUTTON;

void updateButton() {
    static bool input1 = false;

    if (GPIOA->IDR & GPIO_IDR_0) {
        button = USER_BUTTON;
        return;
    }

    if (input1) {
        GPIOA->BSRR = GPIO_BSRR_BS_15;
        for (int i = 0; i < 1; ++i) {}

        if (GPIOA->IDR & GPIO_IDR_4) {
        	button = LEFT_BUTTON;
            return;
        } else if (GPIOA->IDR & GPIO_IDR_5) {
        	button = RIGHT_BUTTON;
            return;
        } else {
        	button = NO_BUTTON;
        }

        GPIOA->BSRR = GPIO_BSRR_BR_15;

    } else {
        GPIOC->BSRR = GPIO_BSRR_BS_12;

        if (GPIOA->IDR & GPIO_IDR_4) {
        	button = TOP_BUTTON;
            return;
        } else if (GPIOA->IDR & GPIO_IDR_5) {
        	button = DOWN_BUTTON;
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
