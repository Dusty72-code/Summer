//
// Created by Dolores on 2026/8/9.
//
#include "bsp_motor.h"
#include "FreeRTOS.h"
#include "task.h"

static volatile int32_t encoder_overflow_cnt = 0;
static int32_t last_encoder_val = 0;

void BSP_Motor_Init(void) {
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
    BSP_Motor_Enable();
    BSP_Motor_Stop();
    encoder_overflow_cnt = 0;
    last_encoder_val = 0;
}

void BSP_Motor_SetSpeed(uint16_t duty, MotorDir_t dir) {
    if (duty > MOTOR_PWM_MAX) duty = MOTOR_PWM_MAX;
    switch (dir) {
    case MOTOR_DIR_CW:
        HAL_GPIO_WritePin(TB6612_AIN1_GPIO_Port, TB6612_AIN1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(TB6612_AIN2_GPIO_Port, TB6612_AIN2_Pin, GPIO_PIN_RESET);
        break;
    case MOTOR_DIR_CCW:
        HAL_GPIO_WritePin(TB6612_AIN1_GPIO_Port, TB6612_AIN1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(TB6612_AIN2_GPIO_Port, TB6612_AIN2_Pin, GPIO_PIN_SET);
        break;
    case MOTOR_DIR_STOP:
    default:
        HAL_GPIO_WritePin(TB6612_AIN1_GPIO_Port, TB6612_AIN1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(TB6612_AIN2_GPIO_Port, TB6612_AIN2_Pin, GPIO_PIN_RESET);
        break;
    }
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, duty);
}

void BSP_Motor_Stop(void) {
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
    HAL_GPIO_WritePin(TB6612_AIN1_GPIO_Port, TB6612_AIN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TB6612_AIN2_GPIO_Port, TB6612_AIN2_Pin, GPIO_PIN_RESET);
}

void BSP_Motor_Enable(void) {
    HAL_GPIO_WritePin(TB6612_STBY_GPIO_Port, TB6612_STBY_Pin, GPIO_PIN_SET);
}

static int32_t read_encoder_total(void) {
    int16_t cnt_now = (int16_t)__HAL_TIM_GET_COUNTER(&htim3);
    int16_t cnt_prev = (int16_t)last_encoder_val;
    int32_t delta = (int32_t)cnt_now - (int32_t)cnt_prev;
    return last_encoder_val + delta;
}

int32_t BSP_Motor_GetEncoderAndClear(void) {
    taskENTER_CRITICAL();
    int32_t total = read_encoder_total();
    last_encoder_val = 0;
    __HAL_TIM_SET_COUNTER(&htim3, 0);
    taskEXIT_CRITICAL();
    return total;
}