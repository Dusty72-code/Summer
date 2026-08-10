//
// Created by Dolores on 2026/8/9.
//
#include "joystick_app.h"
#include "bsp_joystick.h"
#include "bsp_servo.h"
#include "can_app.h"
#include "self_test.h"
#include "Summer.h"

void Joystick_App_Update(void) {
    if (BSP_Joystick_IsSWPressed() && !SelfTest_IsActive()) {
        SelfTest_Start();
    }
    int8_t x_percent = BSP_Joystick_GetXPercent();
    int8_t y_percent = BSP_Joystick_GetYPercent();
    int8_t servo_rpm = (int8_t)((x_percent * (int16_t)SERVO_MAX_SPEED_RPM) / 100);
    int8_t wheel_rpm = (int8_t)((y_percent * (int16_t)WHEEL_MAX_SPEED_RPM) / 100);
    CAN_App_SetGimbalCtrl(servo_rpm, wheel_rpm);
}