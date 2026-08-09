//
// Created by Dolores on 2026/8/9.
//
#include "bsp_led.h"
#include "cmsis_os.h"

void LED_On(void) {
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
}

void LED_Off(void) {
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
}

void LED_SelfTest(uint8_t n) {
    for (uint8_t i = 0; i < n; i++) {
        LED_On();
        osDelay(100);
        LED_Off();
        osDelay(100);
    }
}

void LED_Breathe(void) {
    const uint8_t steps = 20;
    const uint32_t tick = 15;
    for (uint8_t i = 0; i < steps; i++) {
        uint32_t duty = (i < steps / 2) ? i : (steps - 1 - i);
        LED_On();
        osDelay(tick * duty / (steps / 2));
        LED_Off();
        osDelay(tick * (steps / 2 - duty) / (steps / 2));
    }
}