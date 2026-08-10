//
// Created by Dolores on 2026/8/9.
//
#include "motor_control.h"
#include "bsp_motor.h"
#include "can_app.h"
#include "self_test.h"

MotorController g_motor;
volatile uint8_t motor_error = 0;

static uint32_t g_no_move_since = 0;
static uint32_t g_probe_start = 0;
static int32_t g_probe_enc0 = 0;
static uint8_t g_probing = 0;

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
    g_no_move_since = 0;
    g_probe_start = 0;
    g_probing = 0;
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
    uint32_t now = HAL_GetTick();
    if (SelfTest_IsActive()) {
        g_motor.motor_online = 1;
        motor_error = 0;
        return;
    }

    if (g_motor.target_rpm != 0.0f) {
        PID_SetSetpoint(&g_motor.speed_pid, g_motor.target_rpm);
        float out = PID_Calculate(&g_motor.speed_pid, g_motor.actual_rpm);
        g_motor.output_pwm = (int16_t)out;
        BSP_MotorSetPWM(g_motor.output_pwm);
        if (g_motor.actual_rpm > MOTOR_ZERO_SPEED_THRESHOLD ||
            g_motor.actual_rpm < -MOTOR_ZERO_SPEED_THRESHOLD) {
            g_motor.motor_online = 1;
            motor_error = 0;
            g_no_move_since = 0;
        } else {
            if (g_no_move_since == 0) g_no_move_since = now;
            else if (now - g_no_move_since > 1500) {
                g_motor.motor_online = 0;
                motor_error = 1;
            }
        }
    } else {
        if (!g_probing && now - g_probe_start > 1500) {
            g_probe_enc0 = BSP_GetEncoderCount();
            BSP_MotorSetPWM(800);
            g_probing = 1;
            g_probe_start = now;
        } else if (g_probing && now - g_probe_start > 80) {
            int32_t diff = BSP_GetEncoderCount() - g_probe_enc0;
            BSP_MotorStop();
            g_probing = 0;
            g_probe_start = now;
            g_motor.motor_online = (diff != 0) ? 1 : 0;
            motor_error = g_motor.motor_online ? 0 : 1;
        } else if (!g_probing) {
            BSP_MotorStop();
        }
    }
    g_can_state.chassis_feedback.motor_online = g_motor.motor_online;
    CAN_App_SetChassisFeedback((int16_t)g_motor.actual_rpm, (int16_t)delta);
}
