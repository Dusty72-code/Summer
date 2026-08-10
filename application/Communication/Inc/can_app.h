//
// Created by Dolores on 2026/8/9.
//

#ifndef SUMMER_CAN_APP_H
#define SUMMER_CAN_APP_H

#include <stdint.h>
#include "can_protocol.h"

typedef struct {
    GimbalCtrlMsg_t gimbal_ctrl;
    ChassisFeedbackMsg_t chassis_feedback;
    uint8_t gimbal_ctrl_updated;
    uint8_t can_comm_ok;
    uint32_t can_tx_cnt;
    uint32_t can_rx_cnt;
} CAN_AppState_t;

extern CAN_AppState_t g_can_state;

void CAN_App_Init(void);
void CAN_App_SetGimbalCtrl(int16_t servo_speed, int16_t wheel_speed);
void CAN_App_SetStatusFlag(uint8_t flag, uint8_t enable);
ChassisFeedbackMsg_t CAN_App_GetChassisFeedback(void);
void CAN_App_SetChassisFeedback(int16_t actual_speed, int16_t encoder_raw);
GimbalCtrlMsg_t CAN_App_GetGimbalCtrl(void);
uint8_t CAN_App_IsGimbalCtrlUpdated(void);
void CAN_App_SetSelfTest(uint8_t enable);
uint8_t CAN_App_IsSelfTest(void);
void CAN_App_RecvPeer(void);
void CAN_App_SendDone(void);
void CAN_App_UpdateComm(void);

#endif //SUMMER_CAN_APP_H
