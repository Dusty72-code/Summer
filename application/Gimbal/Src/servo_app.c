//
// Created by Dolores on 2026/8/9.
//
#include "servo_app.h"
#include <string.h>

ServoCtrl_t g_servo;

void ServoControl_Init(void) {
    memset(&g_servo, 0, sizeof(g_servo));
    g_servo.servo_online = 1;
    BSP_Servo_Init();
}

void ServoControl_SetSpeed(int16_t speed_percent) {
    g_servo.target_speed = (int8_t)speed_percent;
    g_servo.target_rpm = (int16_t)((float)speed_percent / 100.0f * (float)SERVO_MAX_SPEED_RPM);
}