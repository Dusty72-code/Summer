#include "OLED_app.h"
#include "bsp_oled.h"
#include "can_app.h"
#include "motor_control.h"
#include "Summer.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"

void OLED_app_Update(void)
{
    BSP_OLED_Clear();
    char line[22];
    {
        GimbalCtrlMsg_t ctrl = g_can_state.gimbal_ctrl_rx;
        if (ctrl.status_flags & STATUS_SELF_TEST) {
            BSP_OLED_ShowString(0, 0, "=  SELF TEST  =");
        } else {
            BSP_OLED_ShowString(0, 0, "Chassis C Board");
        }
    }
    {
        GimbalCtrlMsg_t ctrl = g_can_state.gimbal_ctrl_rx;
        const char *st = ctrl.servo_online ? "OK" : "OFF";
        snprintf(line, sizeof(line), "Srv :%5drpm[%s]", ctrl.servo_target_speed, st);
        BSP_OLED_ShowString(0, 10, line);
    }
    {
        GimbalCtrlMsg_t ctrl = g_can_state.gimbal_ctrl_rx;
        snprintf(line, sizeof(line), "MtrT:%5drpm", ctrl.wheel_target_speed);
        BSP_OLED_ShowString(0, 19, line);
    }
    {
        int16_t actual = (int16_t)g_motor.actual_rpm;
        const char *st = g_motor.motor_online ? "OK" : "ERR";
        snprintf(line, sizeof(line), "MtrR:%5drpm[%s]", actual, st);
        BSP_OLED_ShowString(0, 28, line);
    }
    {
        const char *st = g_can_state.can_comm_ok ? "OK" : "ERR";
        snprintf(line, sizeof(line), "CAN:[%s]G-C", st);
        BSP_OLED_ShowString(0, 37, line);
    }
    {
        snprintf(line, sizeof(line), "TX:%4lu RX:%4lu",
                 g_can_state.can_tx_cnt, g_can_state.can_rx_cnt);
        BSP_OLED_ShowString(0, 46, line);
    }
    {
        uint32_t sec = xTaskGetTickCount() / 1000U;
        snprintf(line, sizeof(line), "Up:%5lus", (unsigned long)sec);
        BSP_OLED_ShowString(0, 55, line);
    }
    BSP_OLED_Refresh();
}
