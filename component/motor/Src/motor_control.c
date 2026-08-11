//
// Created by Dolores on 2026/8/9.
//
#include "motor_control.h"
#include "can_app.h"
#include <math.h>
#include <string.h>

MotorCtrl_t g_motor;

void MotorControl_Init(void)
{
    memset(&g_motor, 0, sizeof(g_motor));
    PID_Init(&g_motor.speed_pid, MOTOR_KP_DEFAULT,
             MOTOR_KI_DEFAULT, MOTOR_KD_DEFAULT,
             MOTOR_OUTPUT_MIN, MOTOR_OUTPUT_MAX,
             MOTOR_PID_MODE);
    BSP_Motor_Init();
    g_motor.motor_online = 1U;
    g_motor.motor_error = 0U;
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
