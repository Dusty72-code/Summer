#include "joystick_app.h"
#include "bsp_joystick.h"
#include "servo_app.h"
#include "Summer.h"

void Joystick_APP_Update(int16_t *servo_rpm, int16_t *wheel_rpm) {
    int8_t x_percent = BSP_Joystick_GetXPercent();
    int8_t y_percent = BSP_Joystick_GetYPercent();
    int16_t s_rpm = (int16_t)((float)x_percent / 100.0f * SERVO_MAX_SPEED_RPM);
    int16_t w_rpm = (int16_t)((float)y_percent / 100.0f * WHEEL_MAX_RPM);
    ServoControl_SetSpeed(x_percent);
    if (servo_rpm) *servo_rpm = s_rpm;
    if (wheel_rpm) *wheel_rpm = w_rpm;
}

uint8_t Joystick_APP_IsSWPressed(void) {
    return BSP_Joystick_IsSWPressed();
}
