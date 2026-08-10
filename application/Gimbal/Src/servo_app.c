//
// Created by Dolores on 2026/8/9.
//
#include "servo_app.h"
#include "bsp_servo.h"
#include "can_app.h"
#include "self_test.h"

void Servo_App_Update(void) {
    if (SelfTest_IsActive()) return;
    GimbalCtrlMsg_t ctrl = CAN_App_GetGimbalCtrl();
    int8_t srv = (int8_t)((ctrl.servo_target_speed * 100) / SERVO_MAX_SPEED_RPM);
    BSP_Servo_SetSpeed(srv);
}