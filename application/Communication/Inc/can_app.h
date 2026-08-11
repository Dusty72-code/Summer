//
// Created by Dolores on 2026/8/9.
//

#ifndef SUMMER_CAN_APP_H
#define SUMMER_CAN_APP_H

#include "main.h"
#include "can_protocol.h"
#include "bsp_can.h"
#include "Summer.h"

#define CAN_SEND_PERIOD_MS 10
#define CAN_HEARTBEAT_TIMEOUT 100
#define CAN_TX_TIMEOUT 5

typedef struct {
    GimbalCtrlMsg_t gimbal_ctrl;
    GimbalCtrlMsg_t gimbal_ctrl_rx;
    ChassisFeedbackMsg_t chassis_feedback;
    ChassisFeedbackMsg_t chassis_feedback_rx;
    uint8_t gimbal_heartbeat;
    uint8_t chassis_heartbeat;
    uint8_t chassis_fb_updated;
    uint8_t gimbal_ctrl_updated;
    uint8_t chassis_online;
    uint8_t gimbal_online;
    uint8_t last_chassis_hb;
    uint8_t last_gimbal_hb;
    uint32_t last_chassis_rx_time;
    uint32_t last_gimbal_rx_time;
    uint8_t can_comm_ok;
    uint32_t can_tx_cnt;
    uint32_t can_rx_cnt;
} CAN_AppState_t;

extern CAN_AppState_t g_can_state;

void CAN_App_Init(void);
uint8_t CAN_App_IsCommOK(void);
#ifdef GIMBAL
void CAN_App_SetGimbalCtrl(int16_t servo_speed, int16_t wheel_speed);
void CAN_App_SetStatusFlag(uint8_t flag, uint8_t enable);
ChassisFeedbackMsg_t CAN_App_GetChassisFeedback(void);
uint8_t CAN_App_IsChassisFeedbackUpdated(void);
#endif
#ifdef CHASSIS
void CAN_App_SetChassisFeedback(int16_t actual_speed, int16_t encoder_raw);
GimbalCtrlMsg_t CAN_App_GetGimbalCtrl(void);
uint8_t CAN_App_IsGimbalCtrlUpdated(void);
#endif

#endif //SUMMER_CAN_APP_H
