//
// Created by Dolores on 2026/8/9.
//

#ifndef SUMMER_BSP_MOTOR_H
#define SUMMER_BSP_MOTOR_H

#include "main.h"
#include "tim.h"

#define MOTOR_PWM_MAX 7199
#define MOTOR_ENCODER_PPR 11
#define MOTOR_GEAR_RATIO 30
#define MOTOR_ENCODER_CPR (MOTOR_ENCODER_PPR * MOTOR_GEAR_RATIO * 4)

typedef enum {
    MOTOR_DIR_STOP = 0,
    MOTOR_DIR_CW = 1,
    MOTOR_DIR_CCW = 2,
} MotorDir_t;

void BSP_Motor_Init(void);
void BSP_Motor_SetSpeed(uint16_t duty, MotorDir_t dir);
void BSP_Motor_Stop(void);
int32_t BSP_Motor_GetEncoderAndClear(void);
void BSP_Motor_Enable(void);

#endif //SUMMER_BSP_MOTOR_H
