//
// Created by Dolores on 2026/8/9.
//

#ifndef SUMMER_BSP_SERVO_H
#define SUMMER_BSP_SERVO_H

#include "main.h"
#include "tim.h"

#define SERVO_PULSE_STOP 1500
#define SERVO_PULSE_MIN 500
#define SERVO_PULSE_MAX 2500

#define SERVO_MAX_SPEED_RPM 60
#define SERVO_SAMPLE_PERIOD_MS 10

void BSP_Servo_Init(void);
void BSP_Servo_SetPulse(uint16_t pulse_us);
void BSP_Servo_SetSpeed(int8_t speed);
void BSP_Servo_Stop(void);

#endif //SUMMER_BSP_SERVO_H
