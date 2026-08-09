//
// Created by Dolores on 2026/8/9.
//
#include "bsp_motor.h"

void BSP_MotorInit(void) {
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    __HAL_TIM_SET_COUNTER(&htim3, 0);
    HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
}

void BSP_MotorSetPWM(int16_t pwm) {
    if (pwm > MOTOR_PWM_PERIOD) pwm = MOTOR_PWM_PERIOD;
    if (pwm < -MOTOR_PWM_PERIOD) pwm = -MOTOR_PWM_PERIOD;

    if (pwm >= 0) {
        HAL_GPIO_WritePin(TB6612_AIN1_GPIO_Port, TB6612_AIN1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(TB6612_AIN2_GPIO_Port, TB6612_AIN2_Pin, GPIO_PIN_RESET);
    } else {
        HAL_GPIO_WritePin(TB6612_AIN1_GPIO_Port, TB6612_AIN1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(TB6612_AIN2_GPIO_Port, TB6612_AIN2_Pin, GPIO_PIN_SET);
        pwm = -pwm;
    }
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, (uint32_t)pwm);
}

void BSP_MotorStop(void) {
    HAL_GPIO_WritePin(TB6612_AIN1_GPIO_Port, TB6612_AIN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TB6612_AIN2_GPIO_Port, TB6612_AIN2_Pin, GPIO_PIN_RESET);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
}

int32_t BSP_GetEncoderCount(void) {
    return (int32_t)(__HAL_TIM_GET_COUNTER(&htim3));
}