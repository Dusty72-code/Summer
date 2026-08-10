//
// Created by Dolores on 2026/8/9.
//
#include "OLED_app.h"
#include "bsp_oled.h"
#include "can_app.h"
#include "motor_control.h"
#include "self_test.h"
#include "stm32f1xx_hal.h"

void OLED_app_Update(void) {
    GimbalCtrlMsg_t gimbal_ctrl = CAN_App_GetGimbalCtrl();
    uint32_t up_sec = (uint32_t)(HAL_GetTick() / 1000);
    BSP_OLED_Clear();
    if (SelfTest_IsActive()) {
        BSP_OLED_ShowString(0, 0, "=  SELF TEST  =");
    } else {
        BSP_OLED_ShowString(0, 0, "RM Chassis C Board");
    }
    BSP_OLED_ShowString(0, 1, "Srv :");
    BSP_OLED_ShowNum(48, 1, gimbal_ctrl.servo_target_speed, 5);
    BSP_OLED_ShowString(88, 1, gimbal_ctrl.servo_online ? "[OK]" : "[OFF]");
    BSP_OLED_ShowString(0, 2, "MtrT:");
    BSP_OLED_ShowNum(40, 2, gimbal_ctrl.wheel_target_speed, 5);
    BSP_OLED_ShowString(0, 3, "MtrR:");
    BSP_OLED_ShowNum(40, 3, (int32_t)g_motor.actual_rpm, 5);
    BSP_OLED_ShowString(88, 3, g_motor.motor_online ? "[OK]" : "[ERR]");
    BSP_OLED_ShowString(0, 4, "CAN:[");
    BSP_OLED_ShowString(40, 4, g_can_state.can_comm_ok ? "OK" : "ERR");
    BSP_OLED_ShowString(72, 4, "]G-C");
    BSP_OLED_ShowString(0, 5, "TX:");
    BSP_OLED_ShowNum(24, 5, (int32_t)g_can_state.can_tx_cnt, 4);
    BSP_OLED_ShowString(56, 5, "RX:");
    BSP_OLED_ShowNum(80, 5, (int32_t)g_can_state.can_rx_cnt, 4);
    BSP_OLED_ShowString(0, 6, "Up:");
    BSP_OLED_ShowNum(24, 6, (int32_t)up_sec, 5);
    BSP_OLED_Refresh();
}
