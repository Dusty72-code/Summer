//
// Created by Dolores on 2026/8/9.
//

#ifndef SUMMER_BSP_OLED_H
#define SUMMER_BSP_OLED_H

#include "main.h"
#include "i2c.h"
#include <stdint.h>
#include <string.h>

#define OLED_I2C_HANDLE hi2c1
#define OLED_I2C_ADDR 0x78U
#define OLED_WIDTH 128U
#define OLED_HEIGHT 64U
#define OLED_PAGES (OLED_HEIGHT / 8U)
#define OLED_REFRESH_PERIOD_MS 400U

void BSP_OLED_Init(void);
void BSP_OLED_Clear(void);
void BSP_OLED_ShowChar(uint8_t x, uint8_t y, char ch);
void BSP_OLED_ShowString(uint8_t x, uint8_t y, const char *str);
void BSP_OLED_ShowNum(uint8_t x, uint8_t y, int32_t num, uint8_t width);
void BSP_OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t color);
void BSP_OLED_Refresh(void);

#endif //SUMMER_BSP_OLED_H
