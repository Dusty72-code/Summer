#ifndef SUMMER_JOYSTICK_APP_H
#define SUMMER_JOYSTICK_APP_H

#include <stdint.h>

void Joystick_APP_Update(int16_t *servo_rpm, int16_t *wheel_rpm);
uint8_t Joystick_APP_IsSWPressed(void);

#endif //SUMMER_JOYSTICK_APP_H
