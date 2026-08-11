//
// Created by Dolores on 2026/8/9.
//

#ifndef SUMMER_PID_H
#define SUMMER_PID_H

#include <stdint.h>

typedef enum {
    PID_MODE_POSITIONAL = 0,
    PID_MODE_INCREMENTAL,
} PID_Mode_t;

typedef struct {
    float kp, ki, kd;
    float output_min, output_max;
    float integral_limit;
    PID_Mode_t mode;
    float setpoint;
    float last_error, prev_error;
    float integral, output;
} PID_t;

void PID_Init(PID_t *pid, float kp, float ki, float kd, float out_min, float out_max, PID_Mode_t mode);
void PID_Reset(PID_t *pid);
float PID_Compute(PID_t *pid, float setpoint, float measured, float dt);

#endif //SUMMER_PID_H
