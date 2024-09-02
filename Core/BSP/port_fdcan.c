#include "main.h"
#include "fdcan.h"
#include "cvector.h"
#include "stdint.h"

#define BUF_MAX_LEN 16 //缓冲包长度

typedef void can_rx_func(uint16_t id, uint8_t* pdata, uint16_t len);
typedef struct
{
  cvector* rxcallbackS;
  FDCAN_HandleTypeDef* hfdcan;
}can_module;

#define CAN_MODULE_NUM 1
can_module can_port[CAN_MODULE_NUM];

void BSP_FDCAN_FilterInit()
{
  /*
    filter配置根据要求自行更改
    当前配置:
    RANGE 0x300 - 0x302 FIFO0
    RANGE 0x303 - 0x305 FIFO1
  */

  FDCAN_FilterTypeDef filter_cfg0;
  filter_cfg0.IdType = FDCAN_STANDARD_ID;
  filter_cfg0.FilterIndex = 0;
  filter_cfg0.FilterType = FDCAN_FILTER_RANGE;
  filter_cfg0.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
  filter_cfg0.FilterID1 = 0x300;
  filter_cfg0.FilterID2 = 0x302;
    if (HAL_FDCAN_ConfigFilter(&hfdcan1, &filter_cfg0) != HAL_OK){
      Error_Handler();
    }

  FDCAN_FilterTypeDef filter_cfg1;
  filter_cfg1.IdType = FDCAN_STANDARD_ID;
  filter_cfg1.FilterIndex = 0;
  filter_cfg1.FilterType = FDCAN_FILTER_RANGE;
  filter_cfg1.FilterConfig = FDCAN_FILTER_TO_RXFIFO1;
  filter_cfg1.FilterID1 = 0x303;
  filter_cfg1.FilterID2 = 0x305;
    if (HAL_FDCAN_ConfigFilter(&hfdcan1, &filter_cfg1) != HAL_OK){
      Error_Handler();
    }
}

void BSP_FDCAN_Init()
{
  can_port[0].hfdcan = &hfdcan1;
  can_port[0].rxcallbackS = cvector_create(sizeof(can_rx_func*));

  BSP_FDCAN_FilterInit();
  HAL_FDCAN_Start(&hfdcan1);
  HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE | FDCAN_IT_RX_FIFO1_NEW_MESSAGE, 0); //第三个参数在不开启FDCAN_IT_TX_COMPLETE时不用管
}

void BSP_FDCAN_Transmit(uint8_t* pdata, uint16_t len, uint16_t id)
{
  FDCAN_TxHeaderTypeDef txheader;
  txheader.DataLength = len;
  txheader.Identifier = id;
  txheader.IdType = FDCAN_STANDARD_ID;      //标准帧
  txheader.TxFrameType = FDCAN_DATA_FRAME;  //数据帧
  txheader.BitRateSwitch = FDCAN_BRS_OFF;   //关闭速率切换？
  txheader.FDFormat = FDCAN_CLASSIC_CAN;    //传统的CAN模式？
  txheader.ErrorStateIndicator = FDCAN_ESI_ACTIVE; //无发送事件？
  txheader.MessageMarker = 0;
  HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &txheader, pdata);
}

void BSP_FDCAN_Fifo_Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t fifo)
{
  FDCAN_RxHeaderTypeDef rxheader;
  uint8_t can_rxbuf[BUF_MAX_LEN];
  HAL_FDCAN_GetRxMessage(hfdcan, fifo, &rxheader, can_rxbuf);
  for (uint8_t i = 0; i < CAN_MODULE_NUM; i++){
    if (can_port[i].hfdcan == hfdcan){
        for (uint8_t j = 0; j < can_port[i].rxcallbackS->cv_len; j++){
          can_rx_func* func = *(can_rx_func**)cvector_val_at(can_port[i].rxcallbackS, j);
          func(rxheader.Identifier, can_rxbuf, rxheader.DataLength);
        }
    }
  }
}

void BSP_FDCAN_RegisterCallback(uint8_t can_module_index, can_rx_func* callback)
{
  cvector_pushback(can_port[can_module_index].rxcallbackS, &callback);
}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hcan, uint32_t RxFIifo0ITs) {
  if ((RxFIifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != 0){
    BSP_FDCAN_Fifo_Callback(hcan, FDCAN_RX_FIFO0);
  }
}

void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef *hcan, uint32_t RxFIifo0ITs) {
  if ((RxFIifo0ITs & FDCAN_IT_RX_FIFO1_NEW_MESSAGE) != 0){
    BSP_FDCAN_Fifo_Callback(hcan, FDCAN_RX_FIFO1);
  }
}