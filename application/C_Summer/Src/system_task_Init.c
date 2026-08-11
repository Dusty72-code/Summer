//
// Created by Dolores on 2026/8/10.
//
#include "system_task_init.h"
#include "Summer.h"
#include "bsp_can.h"
#include "can_app.h"
#include "can.h"

#ifdef GIMBAL
#include "servo_app.h"
#include "bsp_joystick.h"
#endif

#ifdef CHASSIS
#include "bsp_oled.h"
#include "motor_control.h"
#endif

void System_Periph_Init(void) {
    CAN_App_Init();

#ifdef GIMBAL
    ServoControl_Init();
#endif

#ifdef CHASSIS
    BSP_OLED_Init();
    MotorControl_Init();
#endif
}