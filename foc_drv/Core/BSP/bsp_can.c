#include "can.h"
#include "stdint.h"
#include "main.h"

#define CAN_MODULE_NUM 1

typedef void rx_callback(uint8_t*, uint16_t);

uint8_t can_rxbuf[256];

typedef struct can_module_s
{
  uint16_t id;
  rx_callback* callback;
}can_module;

static can_module can_port[CAN_MODULE_NUM];

void BSP_CAN_Init()
{
  CAN_FilterTypeDef filter_cfg;
  filter_cfg.FilterActivation = CAN_FILTER_ENABLE;
  filter_cfg.FilterBank = 0;
  filter_cfg.SlaveStartFilterBank = 14;
  filter_cfg.FilterFIFOAssignment = CAN_RX_FIFO0;
  filter_cfg.FilterMode = CAN_FILTERMODE_IDLIST;
  filter_cfg.FilterScale = CAN_FILTERSCALE_16BIT;
  filter_cfg.FilterIdHigh = 0x401 << 5;  
  filter_cfg.FilterIdLow = 0x800 << 5;
  filter_cfg.FilterMaskIdHigh = 0x800 << 5;
  filter_cfg.FilterMaskIdLow = 0x800 << 5;
  HAL_CAN_ConfigFilter(&hcan, &filter_cfg);
  HAL_CAN_Start(&hcan);
  HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_RX_FIFO1_MSG_PENDING);

  can_port[0].id = 0x401;
}

void BSP_CAN_Transmit(uint8_t* pdata, uint16_t len, uint16_t id)
{
  uint32_t txmailbox;
  CAN_TxHeaderTypeDef txheader;
  txheader.DLC = len;
  txheader.IDE = CAN_ID_STD;
  txheader.RTR = CAN_RTR_DATA;
  txheader.ExtId = 0;
  txheader.StdId = id;
  txheader.TransmitGlobalTime = DISABLE;
  
  HAL_CAN_AddTxMessage(&hcan, &txheader, pdata, &txmailbox);
}

void BSP_CAN_Fifo_Callback(CAN_HandleTypeDef *hcan, uint32_t fifo)
{
  CAN_RxHeaderTypeDef rxheader;
  HAL_CAN_GetRxMessage(hcan, fifo, &rxheader, can_rxbuf);
    for (uint8_t i = 0; i < CAN_MODULE_NUM; i++){
      if (can_port[i].id == rxheader.StdId){
        if (can_port[i].callback != NULL) can_port[i].callback(can_rxbuf, rxheader.DLC);
      }
  }
}

void BSP_CAN_RegisterCallback(rx_callback* callback, uint16_t id)
{
  for (uint8_t i = 0; i < CAN_MODULE_NUM; i++){
    if (can_port[i].id == id){
      can_port[i].callback = callback;
    }
  }
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
  BSP_CAN_Fifo_Callback(hcan, CAN_RX_FIFO0);
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan) {
  BSP_CAN_Fifo_Callback(hcan, CAN_RX_FIFO1);
}