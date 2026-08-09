//
// Created by Dolores on 2026/8/9.
//

#ifndef SUMMER_PID_H
#define SUMMER_PID_H

typedef struct {
    float kp, ki, kd;
    float setpoint;
    float integral;
    float prev_error;
    float integral_limit;
    float output_min, output_max;
} PID_Controller;

void PID_Init(PID_Controller *pid, float kp, float ki, float kd, float out_min, float out_max);
float PID_Calculate(PID_Controller *pid, float measurement);
void PID_SetSetpoint(PID_Controller *pid, float setpoint);
void PID_Reset(PID_Controller *pid);

#endif //SUMMER_PID_H
