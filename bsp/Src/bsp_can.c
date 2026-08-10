//
// Created by Dolores on 2026/8/8.
//
#include "bsp_can.h"

static void (*user_rx_callback)(CAN_HandleTypeDef *, uint32_t, uint8_t *) = NULL;

HAL_StatusTypeDef BSP_CAN_Send(CAN_HandleTypeDef *hcan, uint32_t id, uint8_t *data, uint8_t len) {
    CAN_TxHeaderTypeDef tx_header = {0};
    uint32_t tx_mailbox = 0;
    tx_header.StdId = id;
    tx_header.ExtId = 0;
    tx_header.IDE = CAN_ID_STD;
    tx_header.RTR = CAN_RTR_DATA;
    tx_header.DLC = len;
    tx_header.TransmitGlobalTime = DISABLE;
    return HAL_CAN_AddTxMessage(hcan, &tx_header, data, &tx_mailbox);
}

void BSP_CAN_ConfigFilter(CAN_HandleTypeDef *hcan, uint32_t filter_id, uint32_t filter_mask, uint32_t fifo) {
    CAN_FilterTypeDef filter = {0};
    filter.FilterIdHigh = (filter_id << 5) & 0xFFFF;
    filter.FilterIdLow = 0;
    filter.FilterMaskIdHigh = (filter_mask << 5) & 0xFFFF;
    filter.FilterMaskIdLow = 0;
    filter.FilterFIFOAssignment = fifo;
    filter.FilterBank = 0;
    filter.FilterMode = CAN_FILTERMODE_IDMASK;
    filter.FilterScale = CAN_FILTERSCALE_32BIT;
    filter.FilterActivation = ENABLE;
    filter.SlaveStartFilterBank = 14;
    HAL_CAN_ConfigFilter(hcan, &filter);
}

void BSP_CAN_InstallRxCallback(CAN_HandleTypeDef *hcan, void (*cb)(CAN_HandleTypeDef *, uint32_t, uint8_t *)) {
    user_rx_callback = cb;
}

void BSP_CAN_GetTxMailboxStatus(CAN_HandleTypeDef *hcan, uint32_t *mailboxes) {
    if (!mailboxes) return;
    for (int i = 0; i < 3; i++) {
        mailboxes[i] = (hcan->Instance->TSR & (1UL << (26 + i * 2))) ? 1 : 0;
    }
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    CAN_RxHeaderTypeDef rx_header;
    uint8_t rx_data[8];
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, rx_data) == HAL_OK) {
        if (user_rx_callback) user_rx_callback(hcan, rx_header.StdId, rx_data);
    }
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    CAN_RxHeaderTypeDef rx_header;
    uint8_t rx_data[8];
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO1, &rx_header, rx_data) == HAL_OK) {
        if (user_rx_callback) user_rx_callback(hcan, rx_header.StdId, rx_data);
    }
}

void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan) {
    uint32_t err = HAL_CAN_GetError(hcan);
    if (err & HAL_CAN_ERROR_BOF) {
        HAL_CAN_ResetError(hcan);
        if (HAL_CAN_GetState(hcan) == HAL_CAN_STATE_ERROR) {
            HAL_CAN_Stop(hcan);
            HAL_CAN_Start(hcan);
        }
    }
}
