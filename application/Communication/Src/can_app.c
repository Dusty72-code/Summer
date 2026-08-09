//
// Created by Dolores on 2026/8/9.
//
#include "can_app.h"
#include "Summer.h"

CAN_AppState_t g_can_state;

void CAN_App_Init(void) {
    g_can_state.gimbal_ctrl_updated = 0;
    g_can_state.gimbal_ctrl.servo_target_speed = 0;
    g_can_state.gimbal_ctrl.wheel_target_speed = 0;
    g_can_state.gimbal_ctrl.servo_online = 0;
    g_can_state.gimbal_ctrl.gimbal_heartbeat = 0;
    g_can_state.gimbal_ctrl.status_flags = STATUS_SYSTEM_OK;
    g_can_state.chassis_feedback.motor_actual_speed = 0;
    g_can_state.chassis_feedback.motor_encoder_raw = 0;
    g_can_state.chassis_feedback.motor_online = 0;
    g_can_state.chassis_feedback.chassis_heartbeat = 0;
    g_can_state.chassis_feedback.status_flags = STATUS_SYSTEM_OK;
}

void CAN_App_SetGimbalCtrl(int16_t servo_speed, int16_t wheel_speed) {
    g_can_state.gimbal_ctrl.servo_target_speed = servo_speed;
    g_can_state.gimbal_ctrl.wheel_target_speed = wheel_speed;
    g_can_state.gimbal_ctrl_updated = 1;
}

void CAN_App_SetStatusFlag(uint8_t flag, uint8_t enable) {
    if (enable)
        g_can_state.gimbal_ctrl.status_flags |= flag;
    else
        g_can_state.gimbal_ctrl.status_flags &= (uint8_t)~flag;
}

ChassisFeedbackMsg_t CAN_App_GetChassisFeedback(void) {
    return g_can_state.chassis_feedback;
}

void CAN_App_SetChassisFeedback(int16_t actual_speed, int16_t encoder_raw) {
    g_can_state.chassis_feedback.motor_actual_speed = actual_speed;
    g_can_state.chassis_feedback.motor_encoder_raw = encoder_raw;
}

GimbalCtrlMsg_t CAN_App_GetGimbalCtrl(void) {
    return g_can_state.gimbal_ctrl;
}

uint8_t CAN_App_IsGimbalCtrlUpdated(void) {
    uint8_t f = g_can_state.gimbal_ctrl_updated;
    g_can_state.gimbal_ctrl_updated = 0;
    return f;
}
