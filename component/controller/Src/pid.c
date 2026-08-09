//
// Created by Dolores on 2026/8/9.
//
#include "pid.h"

void PID_Init(PID_Controller *pid, float kp, float ki, float kd, float out_min, float out_max) {
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->integral = 0.0f;
    pid->prev_error = 0.0f;
    pid->setpoint = 0.0f;
    pid->integral_limit = 0.0f;
    pid->output_min = out_min;
    pid->output_max = out_max;
}

float PID_Calculate(PID_Controller *pid, float measurement) {
    float error = pid->setpoint - measurement;
    pid->integral += error;
    if (pid->integral_limit > 0.0f) {
        if (pid->integral > pid->integral_limit) pid->integral = pid->integral_limit;
        if (pid->integral < -pid->integral_limit) pid->integral = -pid->integral_limit;
    }
    float output = pid->kp * error + pid->ki * pid->integral + pid->kd * (error - pid->prev_error);
    pid->prev_error = error;
    if (output > pid->output_max) output = pid->output_max;
    if (output < pid->output_min) output = pid->output_min;
    return output;
}

void PID_SetSetpoint(PID_Controller *pid, float setpoint) {
    pid->setpoint = setpoint;
}

void PID_Reset(PID_Controller *pid) {
    pid->integral = 0.0f;
    pid->prev_error = 0.0f;
}
