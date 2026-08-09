//
// Created by Dolores on 2026/8/9.
//

#ifndef SUMMER_BSP_LED_H
#define SUMMER_BSP_LED_H

#include "main.h"
#include <stdint.h>

void LED_On(void);
void LED_Off(void);
void LED_SelfTest(uint8_t n);
void LED_Breathe(void);

#endif //SUMMER_BSP_LED_H
