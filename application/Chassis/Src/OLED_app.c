//
// Created by Dolores on 2026/8/9.
//
#include "OLED_app.h"
#include "bsp_oled.h"
#include "can_app.h"
#include "motor_control.h"
#include "Summer.h"

void OLED_app_Update(void) {
    GimbalCtrlMsg_t gimbal_ctrl = CAN_App_GetGimbalCtrl();
    ChassisFeedbackMsg_t chassis_fb = CAN_App_GetChassisFeedback();
    BSP_OLED_Clear();
    BSP_OLED_ShowString(0, 0, "=== STATUS ===");
    BSP_OLED_ShowString(0, 1, "SrvT:");
    BSP_OLED_ShowNum(40, 1, gimbal_ctrl.servo_target_speed, 4);
    if (gimbal_ctrl.servo_online) {
        BSP_OLED_ShowString(80, 1, "ON ");
    } else {
        BSP_OLED_ShowString(80, 1, "OFF");
    }
    BSP_OLED_ShowString(0, 2, "MtrT:");
    BSP_OLED_ShowNum(40, 2, gimbal_ctrl.wheel_target_speed, 4);
    BSP_OLED_ShowString(0, 3, "MtrA:");
    BSP_OLED_ShowNum(40, 3, (int32_t)g_motor.actual_rpm, 4);
    if (chassis_fb.motor_online) {
        BSP_OLED_ShowString(80, 3, "ON ");
    } else {
        BSP_OLED_ShowString(80, 3, "OFF");
    }
    BSP_OLED_ShowString(0, 4, "CAN:");
    if (gimbal_ctrl.servo_online) {
        BSP_OLED_ShowString(40, 4, "OK ");
    } else {
        BSP_OLED_ShowString(40, 4, "LOST");
    }
    BSP_OLED_Refresh();
}
