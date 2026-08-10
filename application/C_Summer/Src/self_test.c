//
// Created by Dolores on 2026/8/10.
//
#include "self_test.h"
#include "Summer.h"
#include "can_app.h"
#include "bsp_led.h"
#include "cmsis_os.h"

#ifdef GIMBAL
#include "bsp_servo.h"
#endif

#ifdef CHASSIS
#include "motor_control.h"
#endif

static uint8_t g_active = 0;
static uint32_t g_stage_start = 0;
static uint8_t g_blink_count = 0;

void SelfTest_Start(void) {
    if (g_active) return;
    g_active = 1;
    g_stage_start = HAL_GetTick();
#ifdef GIMBAL
    g_blink_count = 2;
#endif
    CAN_App_SetSelfTest(1);
}

uint8_t SelfTest_IsActive(void) {
    return g_active;
}

uint8_t SelfTest_ConsumeBlink(void) {
    uint8_t n = g_blink_count;
    g_blink_count = 0;
    return n;
}

#ifdef GIMBAL
void SelfTest_Update(void) {
    if (!g_active) return;
    uint32_t elapsed = HAL_GetTick() - g_stage_start;
    if (elapsed < 2000) {
        BSP_Servo_SetSpeed(60);
    } else if (elapsed < 4000) {
        BSP_Servo_SetSpeed(-60);
    } else {
        BSP_Servo_Stop();
        g_active = 0;
        CAN_App_SetSelfTest(0);
    }
}
#endif

#ifdef CHASSIS
void SelfTest_Update(void) {
    if (!g_active) return;
    uint32_t elapsed = HAL_GetTick() - g_stage_start;
    if (elapsed < 3000) {
        g_motor.target_rpm = 100.0f;
    } else if (elapsed < 6000) {
        g_motor.target_rpm = -100.0f;
    } else {
        g_motor.target_rpm = 0.0f;
        g_active = 0;
        CAN_App_SetSelfTest(0);
    }
}
#endif
