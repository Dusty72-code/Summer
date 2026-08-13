//
// Created by Dolores on 2026/8/9.
//
#include "bsp_joystick.h"
#include <stdlib.h>

static uint16_t cal_center_x = JOYSTICK_CENTER;
static uint16_t cal_center_y = JOYSTICK_CENTER;
static uint8_t cal_done = 0;

static uint16_t read_adc_channel(uint32_t channel) {
    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = channel;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, 5) == HAL_OK) {
        uint16_t ret = (uint16_t)HAL_ADC_GetValue(&hadc1);
        HAL_ADC_Stop(&hadc1);
        return ret;
    }
    HAL_ADC_Stop(&hadc1);
    return JOYSTICK_CENTER;
}

uint16_t BSP_Joystick_ReadX(void) {
    return read_adc_channel(ADC_CHANNEL_0);
}

uint16_t BSP_Joystick_ReadY(void) {
    return read_adc_channel(ADC_CHANNEL_1);
}

void BSP_Joystick_Calibrate(void) {
    uint32_t sum_x = 0, sum_y = 0;
    for (uint16_t i = 0; i < JOYSTICK_CAL_SAMPLES; i++) {
        sum_x += read_adc_channel(ADC_CHANNEL_0);
        sum_y += read_adc_channel(ADC_CHANNEL_1);
        for (volatile uint32_t d = 0; d < 12000U; d++) { __NOP(); }
    }
    cal_center_x = (uint16_t)(sum_x / JOYSTICK_CAL_SAMPLES);
    cal_center_y = (uint16_t)(sum_y / JOYSTICK_CAL_SAMPLES);
    cal_done = 1;
}

static int8_t map_adc_to_percent(uint16_t adc_val, uint16_t center, uint16_t deadzone) {
    int32_t val = (int32_t)adc_val;
    int32_t cen = (int32_t)center;
    int32_t dz = (int32_t)deadzone;
    if (val >= cen - dz && val <= cen + dz) return 0;
    int32_t pct;
    if (val < cen) {
        int32_t range = cen - dz;
        if (range <= 0) return 0;
        int32_t offset = cen - dz - val;
        pct = -offset * 100 / range;
    } else {
        int32_t range = (int32_t)JOYSTICK_ADC_MAX - cen - dz;
        if (range <= 0) return 0;
        int32_t offset = val - cen - dz;
        pct = offset * 100 / range;
    }
    if (pct >= 99) {
        pct = 100;
    } else if (pct <= -99) {
        pct = -100;
    } //满量程钳位
    return (int8_t)pct;
}

int8_t BSP_Joystick_GetXPercent(void) {
    uint16_t raw = BSP_Joystick_ReadX();
    uint16_t center = cal_done ? cal_center_x : JOYSTICK_CENTER;
    uint16_t dz = cal_done ? JOYSTICK_CAL_DEADZONE : JOYSTICK_DEADZONE;
    return map_adc_to_percent(raw, center, dz);
}

int8_t BSP_Joystick_GetYPercent(void) {
    uint16_t raw = BSP_Joystick_ReadY();
    uint16_t center = cal_done ? cal_center_y : JOYSTICK_CENTER;
    uint16_t dz = cal_done ? JOYSTICK_CAL_DEADZONE : JOYSTICK_DEADZONE;
    int8_t pct = map_adc_to_percent(raw, center, dz);
    return (int8_t)(-pct);
}

uint8_t BSP_Joystick_IsSWPressed(void) {
    static uint32_t last_press_time = 0;
    static uint8_t first_done = 0;
    static uint8_t last_state = 0;
    uint8_t raw = (HAL_GPIO_ReadPin(SW_GPIO_Port, SW_Pin) == GPIO_PIN_RESET) ? 1 : 0;
    if (raw != last_state) {
        last_state = raw;
        if (raw) {
            uint32_t now = HAL_GetTick();
            if (first_done && ((now - last_press_time) < 300)) return 0;
            first_done = 1;
            last_press_time = now;
            return 1;
        }
    }
    return 0;
}
