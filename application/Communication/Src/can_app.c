//
// Created by Dolores on 2026/8/9.
//
#include "can_app.h"
#include "cmsis_os.h"
#include <string.h>
#include "Summer.h"

CAN_AppState_t g_can_state;

void CAN_App_Init(void) {
    memset(&g_can_state, 0, sizeof(g_can_state));
    g_can_state.can_comm_ok = 0U;
    g_can_state.last_chassis_rx_time = HAL_GetTick();
    g_can_state.gimbal_ctrl.status_flags = STATUS_SYSTEM_OK;
    BSP_CAN_FilterInit();
}

uint8_t CAN_App_IsCommOK(void) {
    return g_can_state.can_comm_ok;
}

#ifdef GIMBAL
void CAN_App_SetGimbalCtrl(int16_t servo_speed, int16_t wheel_speed) {
    g_can_state.gimbal_ctrl.servo_target_speed = servo_speed;
    g_can_state.gimbal_ctrl.wheel_target_speed = wheel_speed;
    g_can_state.gimbal_ctrl.servo_online = 1U;
}

void CAN_App_SetStatusFlag(uint8_t flag, uint8_t enable) {
    if (enable) g_can_state.gimbal_ctrl.status_flags |= flag;
    else g_can_state.gimbal_ctrl.status_flags &= ~flag;
}

ChassisFeedbackMsg_t CAN_App_GetChassisFeedback(void) {
    ChassisFeedbackMsg_t fb;
    fb = g_can_state.chassis_feedback_rx;
    g_can_state.chassis_fb_updated = 0U;
    return fb;
}

uint8_t CAN_App_IsChassisFeedbackUpdated(void) {
    return g_can_state.chassis_fb_updated;
}
#endif

#ifdef CHASSIS
void CAN_App_SetChassisFeedback(int16_t actual_speed, int16_t encoder_raw) {
    g_can_state.chassis_feedback.motor_actual_speed = actual_speed;
    g_can_state.chassis_feedback.motor_encoder_raw = encoder_raw;
    g_can_state.chassis_feedback.motor_online = 1U;
}

GimbalCtrlMsg_t CAN_App_GetGimbalCtrl(void) {
    GimbalCtrlMsg_t ctrl;
    ctrl = g_can_state.gimbal_ctrl_rx;
    g_can_state.gimbal_ctrl_updated = 0U;
    return ctrl;
}

uint8_t CAN_App_IsGimbalCtrlUpdated(void) {
    return g_can_state.gimbal_ctrl_updated;
}

#endif