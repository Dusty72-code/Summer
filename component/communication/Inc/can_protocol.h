//
// Created by Dolores on 2026/8/9.
//

#ifndef SUMMER_CAN_PROTOCOL_H
#define SUMMER_CAN_PROTOCOL_H

#include <stdint.h>
#include "Summer.h"

#define CAN_GIMBAL_TO_CHASSIS_ID 0x200
#define CAN_CHASSIS_TO_GIMBAL_ID 0x201
#define CAN_PROTOCOL_DLC 8

typedef struct {
    int16_t servo_target_speed;
    int16_t wheel_target_speed;
    uint8_t servo_online;
    uint8_t gimbal_heartbeat;
    uint8_t status_flags;
    uint8_t reserved;
} GimbalCtrlMsg_t;

typedef struct {
    int16_t motor_actual_speed;
    int16_t motor_encoder_raw;
    uint8_t motor_online;
    uint8_t chassis_heartbeat;
    uint8_t status_flags;
    uint8_t reserved;
} ChassisFeedbackMsg_t;

void Protocol_EncodeGimbalCtrl(const GimbalCtrlMsg_t *msg, uint8_t data[8]);
void Protocol_DecodeGimbalCtrl(const uint8_t data[8], GimbalCtrlMsg_t *msg);
void Protocol_EncodeChassisFeedback(const ChassisFeedbackMsg_t *msg, uint8_t data[8]);
void Protocol_DecodeChassisFeedback(const uint8_t data[8], ChassisFeedbackMsg_t *msg);

#endif //SUMMER_CAN_PROTOCOL_H
