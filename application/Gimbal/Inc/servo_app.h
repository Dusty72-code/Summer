//
// Created by Dolores on 2026/8/9.
//

#ifndef SUMMER_SERVO_APP_H
#define SUMMER_SERVO_APP_H

#include "bsp_servo.h"

#define SERVO_MAX_SPEED_RPM 60
#define SERVO_CTRL_PERIOD_MS 10

typedef struct {
    int8_t target_speed;
    int16_t target_rpm;
    uint8_t servo_online;
} ServoCtrl_t;

extern ServoCtrl_t g_servo;

void ServoControl_Init(void);
void ServoControl_SetSpeed(int16_t speed_percent);

#endif //SUMMER_SERVO_APP_H
