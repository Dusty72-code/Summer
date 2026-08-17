//
// Created by Dolores on 2026/8/8.
//
#include "bsp_can.h"
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"

static BSP_CAN_RxMsg_t can_rx_buf[BSP_CAN_RX_FIFO_SIZE];
static volatile uint8_t can_rx_head = 0;
static volatile uint8_t can_rx_tail = 0;
static volatile uint8_t can_rx_count = 0;

static uint8_t CAN_RxBuf_Push(const BSP_CAN_RxMsg_t *msg) {
    if (can_rx_count >= BSP_CAN_RX_FIFO_SIZE) return 0;
    memcpy((void *)&can_rx_buf[can_rx_head], msg, sizeof(BSP_CAN_RxMsg_t));
    can_rx_head = (can_rx_head + 1) & (BSP_CAN_RX_FIFO_SIZE - 1);
    can_rx_count++;
    return 1;
}

uint8_t BSP_CAN_GetRxMessage(BSP_CAN_RxMsg_t *msg) {
    if (can_rx_count == 0) return 0;
    if (msg != NULL) {
        memcpy(msg, (const void *)&can_rx_buf[can_rx_tail], sizeof(BSP_CAN_RxMsg_t));
    }
    can_rx_tail = (can_rx_tail + 1) & (BSP_CAN_RX_FIFO_SIZE - 1);
    taskENTER_CRITICAL();
    can_rx_count--;
    taskEXIT_CRITICAL();
    return 1;
}

void BSP_CAN_FilterInit(void) {
    CAN_FilterTypeDef filter_cfg = {0};
    filter_cfg.FilterActivation = ENABLE;
    filter_cfg.FilterMode = CAN_FILTERMODE_IDMASK;
    filter_cfg.FilterScale = CAN_FILTERSCALE_32BIT;
    filter_cfg.FilterIdHigh = 0x0000;
    filter_cfg.FilterIdLow = 0x0000;
    filter_cfg.FilterMaskIdHigh = 0x0000;
    filter_cfg.FilterMaskIdLow = 0x0000;
    filter_cfg.FilterBank = 0;
    filter_cfg.FilterFIFOAssignment = CAN_RX_FIFO0;
    HAL_CAN_ConfigFilter(&hcan, &filter_cfg);
    HAL_CAN_Start(&hcan);
    __HAL_CAN_DISABLE_IT(&hcan, CAN_IT_ERROR);
    HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
}

void BSP_CAN_Recover(void) {
    if (HAL_CAN_GetState(&hcan) == HAL_CAN_STATE_ERROR) {
        HAL_CAN_Stop(&hcan);
        HAL_CAN_Start(&hcan);
        __HAL_CAN_DISABLE_IT(&hcan, CAN_IT_ERROR);
        HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
    }
}

HAL_StatusTypeDef BSP_CAN_SendMessage(uint32_t std_id, uint8_t data[8], uint32_t timeout) {
    CAN_TxHeaderTypeDef tx_header = {0};
    uint32_t mailbox;
    HAL_StatusTypeDef status;
    (void)timeout;
    tx_header.StdId = std_id & 0x7FF;
    tx_header.IDE = CAN_ID_STD;
    tx_header.RTR = CAN_RTR_DATA;
    tx_header.DLC = 8;
    status = HAL_CAN_AddTxMessage(&hcan, &tx_header, data, &mailbox);
    if (status != HAL_OK) {
        BSP_CAN_Recover();
    }
    return status;
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    if (hcan->Instance != CAN1) return;
    BSP_CAN_RxMsg_t rx_msg;
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_msg.header, rx_msg.data) == HAL_OK) {
        CAN_RxBuf_Push(&rx_msg);
    }
}