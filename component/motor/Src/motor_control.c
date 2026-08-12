//
// Created by Dolores on 2026/8/9.
//
#include "motor_control.h"
#include "can_app.h"
#include "cmsis_os.h"
#include <math.h>
#include <string.h>

MotorCtrl_t g_motor;
static uint8_t g_motor_self_test_ok = 0;
static uint8_t g_motor_self_test_done = 0;

void MotorControl_Init(void)
{
    memset(&g_motor, 0, sizeof(g_motor));
    PID_Init(&g_motor.speed_pid, MOTOR_KP_DEFAULT,
             MOTOR_KI_DEFAULT, MOTOR_KD_DEFAULT,
             MOTOR_OUTPUT_MIN, MOTOR_OUTPUT_MAX,
             MOTOR_PID_MODE);
    BSP_Motor_Init();
    g_motor.motor_online = 0;
    g_motor.motor_error = 0;
    g_motor_self_test_ok = 0;
    g_motor_self_test_done = 0;
}

void MotorControl_SetTarget(float rpm) {
    g_motor.target_rpm = rpm;
    g_motor.target_updated = 1U;
}

float MotorControl_GetActualSpeed(void) {
    return g_motor.actual_rpm;
}

uint8_t MotorControl_IsOnline(void) {
    return g_motor.motor_online;
}

uint8_t MotorControl_IsError(void) {
    return g_motor.motor_error;
}

void Motor_SelfTest(void) {
    int32_t encoder_delta;
    g_motor.motor_online = 0;
    g_motor.motor_error = 1;
    BSP_Motor_Stop();
    osDelay(100);
    BSP_Motor_GetEncoderAndClear();
    BSP_Motor_SetSpeed(MOTOR_SELF_TEST_PWM, MOTOR_DIR_CW);
    osDelay(MOTOR_SELF_TEST_TIME_MS);
    BSP_Motor_Stop();
    encoder_delta = BSP_Motor_GetEncoderAndClear();
    if (encoder_delta >= MOTOR_SELF_TEST_MIN_ENCODER ||
        encoder_delta <= -MOTOR_SELF_TEST_MIN_ENCODER) {
        g_motor.motor_online = 1;
        g_motor.motor_error = 0;
    } else {
        g_motor.motor_online = 0;
        g_motor.motor_error = 1;
    }
    osDelay(MOTOR_SELF_TEST_STOP_MS);
}
