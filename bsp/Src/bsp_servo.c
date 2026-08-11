//
// Created by Dolores on 2026/8/9.
//
#include "bsp_servo.h"

void BSP_Servo_Init(void) {
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
    BSP_Servo_Stop();
}

void BSP_Servo_SetPulse(uint16_t pulse_us) {
    if (pulse_us < SERVO_PULSE_MIN) pulse_us = SERVO_PULSE_MIN;
    if (pulse_us > SERVO_PULSE_MAX) pulse_us = SERVO_PULSE_MAX;
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, pulse_us);
}

void BSP_Servo_SetSpeed(int8_t speed) {
    uint16_t pulse;
    if (speed > 100)  speed = 100;
    if (speed < -100) speed = -100;
    if (speed == 0)
        pulse = SERVO_PULSE_STOP;
    else
        pulse = SERVO_PULSE_STOP + (int32_t)speed * 10;
    BSP_Servo_SetPulse(pulse);
}

void BSP_Servo_Stop(void) {
    BSP_Servo_SetPulse(SERVO_PULSE_STOP);
}