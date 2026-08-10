//
// Created by Dolores on 2026/8/9.
//
#include "motor_control.h"
#include "bsp_motor.h"
#include "can_app.h"
#include "self_test.h"

MotorController g_motor;
volatile uint8_t motor_error = 0;

static float calc_motor_rpm(int32_t delta_count, float period_s) {
    if (period_s <= 0.0f) return 0.0f;
    float rev_per_sec = (float)delta_count / (float)(11.0f * 30.0f) / period_s;
    return rev_per_sec * 60.0f;
}

void MotorControl_Init(void) {
    PID_Init(&g_motor.speed_pid, MOTOR_KP, MOTOR_KI, MOTOR_KD,
             MOTOR_OUTPUT_MIN, MOTOR_OUTPUT_MAX);
    g_motor.target_rpm = 0.0f;
    g_motor.actual_rpm = 0.0f;
    g_motor.last_encoder = BSP_GetEncoderCount();
    g_motor.output_pwm = 0;
    g_motor.motor_online = 1;
    BSP_MotorInit();
    BSP_MotorStop();
}

void MotorControl_Update(void) {
    if (CAN_App_IsSelfTest() && !SelfTest_IsActive()) {
        SelfTest_Start();
    }
    int32_t curr = BSP_GetEncoderCount();
    int32_t delta = curr - g_motor.last_encoder;
    g_motor.last_encoder = curr;
    float dt = (float)MOTOR_CTRL_PERIOD_MS / 1000.0f;
    g_motor.actual_rpm = calc_motor_rpm(delta, dt);
    if (!SelfTest_IsActive() && CAN_App_IsGimbalCtrlUpdated()) {
        GimbalCtrlMsg_t ctrl = CAN_App_GetGimbalCtrl();
        g_motor.target_rpm = (float)ctrl.wheel_target_speed;
    }
    if (g_motor.target_rpm == 0.0f) {
        g_motor.output_pwm = 0;
        BSP_MotorStop();
        motor_error = 0;
    } else {
        PID_SetSetpoint(&g_motor.speed_pid, g_motor.target_rpm);
        float out = PID_Calculate(&g_motor.speed_pid, g_motor.actual_rpm);
        g_motor.output_pwm = (int16_t)out;
        BSP_MotorSetPWM(g_motor.output_pwm);
    }
    CAN_App_SetChassisFeedback((int16_t)g_motor.actual_rpm, (int16_t)delta);
}
