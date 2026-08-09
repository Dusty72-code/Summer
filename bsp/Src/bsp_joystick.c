//
// Created by Dolores on 2026/8/9.
//
#include "bsp_joystick.h"

static uint16_t cal_center_x = JOYSTICK_CENTER;
static uint16_t cal_center_y = JOYSTICK_CENTER;
static uint16_t axis_range_pos = 1900;
static uint16_t axis_range_neg = 1900;

static uint16_t read_adc_channel(uint32_t channel) {
    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = channel;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) return 0;
    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, 10) != HAL_OK) return 0;
    return (uint16_t)HAL_ADC_GetValue(&hadc1);
}

uint16_t BSP_Joystick_ReadX(void) {
    return read_adc_channel(ADC_CHANNEL_0);
}

uint16_t BSP_Joystick_ReadY(void) {
    return read_adc_channel(ADC_CHANNEL_1);
}

uint8_t BSP_Joystick_IsSWPressed(void) {
    return (HAL_GPIO_ReadPin(SW_GPIO_Port, SW_Pin) == GPIO_PIN_RESET) ? 1 : 0;
}

static int8_t map_adc_to_percent(uint16_t adc, uint16_t center) {
    int32_t diff = (int32_t)adc - (int32_t)center;
    int32_t range = (diff >= 0) ? (int32_t)axis_range_pos : (int32_t)axis_range_neg;
    if (range < 300) range = 300;
    if (range > 2000) range = 2000;
    int32_t pct = (diff * 100) / range;
    if (pct > 100) pct = 100;
    if (pct < -100) pct = -100;
    return (int8_t)pct;
}

int8_t BSP_Joystick_GetXPercent(void) {
    uint16_t x = BSP_Joystick_ReadX();
    int8_t pct = map_adc_to_percent(x, cal_center_x);
    if (pct > 0 && pct < 5) pct = 0;
    if (pct < 0 && pct > -5) pct = 0;
    return pct;
}

int8_t BSP_Joystick_GetYPercent(void) {
    uint16_t y = BSP_Joystick_ReadY();
    int8_t pct = map_adc_to_percent(y, cal_center_y);
    if (pct > 0 && pct < 5) pct = 0;
    if (pct < 0 && pct > -5) pct = 0;
    return pct;
}

void BSP_Joystick_Calibrate(void) {
    uint32_t sum_x = 0, sum_y = 0;
    uint16_t min_x = 4095, max_x = 0, min_y = 4095, max_y = 0;
    for (uint16_t i = 0; i < JOYSTICK_CAL_SAMPLES; i++) {
        uint16_t x = BSP_Joystick_ReadX();
        uint16_t y = BSP_Joystick_ReadY();
        sum_x += x; sum_y += y;
        if (x < min_x) min_x = x;
        if (x > max_x) max_x = x;
        if (y < min_y) min_y = y;
        if (y > max_y) max_y = y;
        HAL_Delay(2);
    }
    cal_center_x = (uint16_t)(sum_x / JOYSTICK_CAL_SAMPLES);
    cal_center_y = (uint16_t)(sum_y / JOYSTICK_CAL_SAMPLES);
    axis_range_pos = (uint16_t)((max_x - cal_center_x) * 8 / 10);
    axis_range_neg = (uint16_t)((cal_center_x - min_x) * 8 / 10);
}
