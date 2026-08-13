//
// Created by Dolores on 2026/8/9.
//
#include "bsp_led.h"
#include "cmsis_os.h"
#include <math.h>

static const uint8_t breath_table[LED_PWM_RESOLUTION] = {
    0, 1, 3, 5, 7, 9, 12, 15, 18, 22, 26, 30, 34, 39, 43, 48, 53, 58, 63, 68,
    74, 79, 85, 90, 96, 100, 100, 100, 96, 90, 85, 79, 74, 68, 63, 58, 53, 48,
    43, 39, 34, 30, 26, 22, 18, 15, 12, 9, 7, 5, 3, 1, 0
};

void LED_StartBlink(void){
    for (int i = 0; i < 3; i++) {
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
        vTaskDelay(pdMS_TO_TICKS(80));
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
        vTaskDelay(pdMS_TO_TICKS(80));
    }
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
}

void LED_On(void) {
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
}

void LED_Off(void) {
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
}

void LED_Breath(uint8_t should_breath) {
    static uint8_t breath_idx = 0;
    if (!should_breath) {
        LED_On();
        breath_idx = 0;
        return;
    }
    uint8_t level = breath_table[breath_idx];
    uint32_t on_ms = (uint32_t)level * LED_BREATH_STEP_MS / 100;
    uint32_t off_ms = LED_BREATH_STEP_MS - on_ms;
    if (on_ms > 0) {
        LED_On();
        vTaskDelay(pdMS_TO_TICKS(on_ms));
    }
    if (off_ms > 0) {
        LED_Off();
        vTaskDelay(pdMS_TO_TICKS(off_ms));
    }
    breath_idx++;
    if (breath_idx >= LED_PWM_RESOLUTION) breath_idx = 0;
}