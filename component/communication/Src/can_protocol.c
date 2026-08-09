//
// Created by Dolores on 2026/8/9.
//
#include "can_protocol.h"

void Protocol_EncodeGimbalCtrl(const GimbalCtrlMsg_t *msg, uint8_t data[8]) {
    int16_t s = msg->servo_target_speed;
    int16_t w = msg->wheel_target_speed;
    data[0] = (uint8_t)(s & 0xFF);
    data[1] = (uint8_t)((s >> 8) & 0xFF);
    data[2] = (uint8_t)(w & 0xFF);
    data[3] = (uint8_t)((w >> 8) & 0xFF);
    data[4] = msg->servo_online;
    data[5] = msg->gimbal_heartbeat;
    data[6] = msg->status_flags;
    data[7] = msg->reserved;
}

void Protocol_DecodeGimbalCtrl(const uint8_t data[8], GimbalCtrlMsg_t *msg) {
    msg->servo_target_speed = (int16_t)(data[0] | ((int16_t)data[1] << 8));
    msg->wheel_target_speed = (int16_t)(data[2] | ((int16_t)data[3] << 8));
    msg->servo_online = data[4];
    msg->gimbal_heartbeat = data[5];
    msg->status_flags = data[6];
    msg->reserved = data[7];
}

void Protocol_EncodeChassisFeedback(const ChassisFeedbackMsg_t *msg, uint8_t data[8]) {
    int16_t a = msg->motor_actual_speed;
    int16_t e = msg->motor_encoder_raw;
    data[0] = (uint8_t)(a & 0xFF);
    data[1] = (uint8_t)((a >> 8) & 0xFF);
    data[2] = (uint8_t)(e & 0xFF);
    data[3] = (uint8_t)((e >> 8) & 0xFF);
    data[4] = msg->motor_online;
    data[5] = msg->chassis_heartbeat;
    data[6] = msg->status_flags;
    data[7] = msg->reserved;
}

void Protocol_DecodeChassisFeedback(const uint8_t data[8], ChassisFeedbackMsg_t *msg) {
    msg->motor_actual_speed = (int16_t)(data[0] | ((int16_t)data[1] << 8));
    msg->motor_encoder_raw = (int16_t)(data[2] | ((int16_t)data[3] << 8));
    msg->motor_online = data[4];
    msg->chassis_heartbeat = data[5];
    msg->status_flags = data[6];
    msg->reserved = data[7];
}
