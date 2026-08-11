//
// Created by Dolores on 2026/8/9.
//

#ifndef SUMMER_BSP_LED_H
#define SUMMER_BSP_LED_H

#include "main.h"
#include <stdint.h>

#define LED_BREATH_STEP_MS 10
#define LED_PWM_RESOLUTION 100

void LED_StartBlink(void);
void LED_On(void);
void LED_Off(void);
void LED_Breath(uint8_t should_breath);

#endif //SUMMER_BSP_LED_H
