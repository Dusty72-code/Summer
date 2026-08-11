//
// Created by Dolores on 2026/8/9.
//
#include "bsp_led.h"
#include "cmsis_os.h"
#include <math.h>

void LED_StartBlink(void){
    for (int i = 0; i < 3; i++) {
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
        vTaskDelay(pdMS_TO_TICKS(80));
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
        vTaskDelay(pdMS_TO_TICKS(80));
    }
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
}