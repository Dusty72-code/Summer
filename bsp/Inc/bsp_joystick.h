//
// Created by Dolores on 2026/8/9.
//

#ifndef SUMMER_BSP_JOYSTICK_H
#define SUMMER_BSP_JOYSTICK_H

#include "main.h"
#include "adc.h"

#define JOYSTICK_CENTER 2048
#define JOYSTICK_CAL_SAMPLES 120

uint16_t BSP_Joystick_ReadX(void);
uint16_t BSP_Joystick_ReadY(void);
int8_t BSP_Joystick_GetXPercent(void);
int8_t BSP_Joystick_GetYPercent(void);
uint8_t BSP_Joystick_IsSWPressed(void);
void BSP_Joystick_Calibrate(void);

#endif //SUMMER_BSP_JOYSTICK_H
