//
// Created by Dolores on 2026/8/9.
//
#include "pid.h"
#include <math.h>

void PID_Init(PID_t *pid, float kp, float ki, float kd, float out_min, float out_max, PID_Mode_t mode) {
    if (pid == NULL) return;
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->output_min = out_min;
    pid->output_max = out_max;
    pid->integral_limit = (out_max - out_min) * 0.5f;
    pid->mode = mode;
    PID_Reset(pid);
}

void PID_Reset(PID_t *pid) {
    if (pid == NULL) return;
    pid->setpoint = 0.0f;
    pid->last_error = 0.0f;
    pid->prev_error = 0.0f;
    pid->integral = 0.0f;
    pid->output = 0.0f;
}

float PID_Compute(PID_t *pid, float setpoint, float measured, float dt) {
    if (pid == NULL) return 0.0f;
    if (dt <= 0.0f) return pid->output;
    float error = setpoint - measured;
    switch (pid->mode) {
        case PID_MODE_INCREMENTAL: {
            float p_term = pid->kp * (error - pid->last_error);
            float i_term = pid->ki * error * dt;
            float d_term = pid->kd * (error - 2.0f * pid->last_error + pid->prev_error) / dt;
            float delta = p_term + i_term + d_term;
            pid->output += delta;
            if (pid->output > pid->output_max) pid->output = pid->output_max;
            if (pid->output < pid->output_min) pid->output = pid->output_min;
            pid->prev_error = pid->last_error;
            pid->last_error = error;
            break;
        }
        case PID_MODE_POSITIONAL:
        default: {
            pid->integral += error * dt;
            if (pid->integral > pid->integral_limit) pid->integral =  pid->integral_limit;
            if (pid->integral < -pid->integral_limit) pid->integral = -pid->integral_limit;
            float derivative = (dt > 0.0f) ? (error - pid->last_error) / dt : 0.0f;
            pid->output = pid->kp * error + pid->ki * pid->integral + pid->kd * derivative;
            if (pid->output > pid->output_max) pid->output = pid->output_max;
            if (pid->output < pid->output_min) pid->output = pid->output_min;
            pid->last_error = error;
            break;
        }
    }
    pid->setpoint = setpoint;
    return pid->output;
}
