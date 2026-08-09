//
// Created by Dolores on 2026/8/9.
//

#ifndef SUMMER_BSP_MOTOR_H
#define SUMMER_BSP_MOTOR_H

#include "main.h"
#include "tim.h"

#define MOTOR_PWM_PERIOD 7199

void BSP_MotorInit(void);
void BSP_MotorSetPWM(int16_t pwm);
void BSP_MotorStop(void);
int32_t BSP_GetEncoderCount(void);

#endif //SUMMER_BSP_MOTOR_H
