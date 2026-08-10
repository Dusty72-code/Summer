//
// Created by Dolores on 2026/8/9.
//
#include "bsp_led.h"
#include "cmsis_os.h"
#include <math.h>

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
    const uint16_t frames = 200;
    const uint32_t frame_ms = 5;
    for (uint16_t i = 0; i < frames; i++) {
        float phase = (float)i / (float)frames * 2.0f * (float)M_PI;
        float duty  = (1.0f - cosf(phase)) * 0.5f;
        uint32_t on_ms  = (uint32_t)(frame_ms * duty);
        uint32_t off_ms = frame_ms - on_ms;
        if (on_ms > 0)  { LED_On();  osDelay(on_ms); }
        if (off_ms > 0) { LED_Off(); osDelay(off_ms); }
    }
}