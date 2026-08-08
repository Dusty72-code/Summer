//
// Created by Dolores on 2026/8/8.
//

#ifndef SUMMER_BSP_CAN_H
#define SUMMER_BSP_CAN_H

#include "main.h"
#include <stdint.h>

typedef struct {
    uint32_t filter_id;
    uint32_t filter_mask;
    uint32_t filter_fifo;
} FilterConfig;

extern CAN_HandleTypeDef hcan;

HAL_StatusTypeDef BSP_CAN_Send(CAN_HandleTypeDef *hcan, uint32_t id, uint8_t *data, uint8_t len);
void BSP_CAN_ConfigFilter(CAN_HandleTypeDef *hcan, uint32_t filter_id, uint32_t filter_mask, uint32_t fifo);
void BSP_CAN_InstallRxCallback(CAN_HandleTypeDef *hcan, void (*cb)(CAN_HandleTypeDef *, uint32_t, uint8_t *));
void BSP_CAN_GetTxMailboxStatus(CAN_HandleTypeDef *hcan, uint32_t *mailboxes);

#endif //SUMMER_BSP_CAN_H
