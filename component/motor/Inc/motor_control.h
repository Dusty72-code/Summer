//
// Created by Dolores on 2026/8/9.
//

#ifndef SUMMER_MOTOR_PROTOCOL_H
#define SUMMER_MOTOR_PROTOCOL_H

#include "pid.h"
#include "stdint.h"

#define MOTOR_KP 120.0f
#define MOTOR_KI 5.0f
#define MOTOR_KD 0.0f
#define MOTOR_OUTPUT_MIN (-7199.0f)
#define MOTOR_OUTPUT_MAX 7199.0f
#define MOTOR_CTRL_PERIOD_MS 10
#define MOTOR_ZERO_SPEED_THRESHOLD 2.0f

typedef struct {
    PID_Controller speed_pid;
    float target_rpm;
    float actual_rpm;
    int32_t last_encoder;
    int16_t output_pwm;
} MotorController;

extern volatile uint8_t motor_error;
extern MotorController g_motor;

void MotorControl_Init(void);
void MotorControl_Update(void);

#endif //SUMMER_MOTOR_PROTOCOL_H
