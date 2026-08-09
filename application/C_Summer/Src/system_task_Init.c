//
// Created by Dolores on 2026/8/10.
//
#include "system_task_init.h"
#include "bsp_can.h"
#include "can_app.h"
#include "can.h"
#include "Summer.h"

#ifdef GIMBAL
#include "bsp_servo.h"
#include "bsp_joystick.h"
#endif

#ifdef CHASSIS
#include "bsp_oled.h"
#include "motor_control.h"
#endif

void System_Periph_Init(void) {
    MX_CAN_Init();
    CAN_App_Init();

#ifdef GIMBAL
    BSP_Servo_Init();
    BSP_Joystick_Calibrate();
#endif

#ifdef CHASSIS
    BSP_OLED_Init();
    MotorControl_Init();
#endif
}