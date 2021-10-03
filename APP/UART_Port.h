/**
 *  @file UART_Port.h
 *
 *  @date 2021-01-04
 *
 *  @author aron566
 *
 *  @brief 串口操作接口
 *  
 *  @version V1.0
 */
#ifndef UART_PORT_H
#define UART_PORT_H
#ifdef __cplusplus //use C compiler
extern "C" {
#endif
/** Includes -----------------------------------------------------------------*/
/*库接口*/
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
/** Private includes ---------------------------------------------------------*/
/** Exported defines ----------------------------------------------------------*/
#define USE_FREERTOS    0
#define USE_USB_CDC     1
/*外部接口*/
#include "stm32F4xx_hal.h"
#include "usart.h"
#if USE_FREERTOS
  #include "cmsis_os.h"
#endif

/*内部接口*/
#include "CircularQueue.h"
/** Exported typedefines -----------------------------------------------------*/
typedef enum
{
  UART_NUM_0 = 0,
  UART_NUM_1,
  UART_NUM_2,
  UART_NUM_3,
  UART_NUM_4,
  UART_NUM_5,
  UART_NUM_6,
  UART_NUM_MAX,
}UART_NUM_Typedef_t;

typedef struct
{
  UART_HandleTypeDef *phuart;     /**< UART句柄     */
  DMA_HandleTypeDef  *phdma_rx;   /**<              */
  DMA_HandleTypeDef  *phdma_tx;   /**<              */
  CQ_handleTypeDef *cb;           /**< 环形队列     */
  uint8_t *RX_Buff_Temp;          /**< 接收缓冲     */
  uint16_t MAX_RX_Temp;           /**< 最大接收数量 */
  int Is_Half_Duplex;             /**< 半双工模式   */
#if USE_USB_CDC
  int Is_USB_CDC_Mode;            /**< USB CDC模式*/
#endif  
#if USE_FREERTOS
  osSemaphoreId *pRX_Sem;         /**< 接收二值信号量*/
#endif
}Uart_Dev_Handle_t;
/** Exported variables -------------------------------------------------------*/
/** Exported functions prototypes --------------------------------------------*/

/*串口操作初始化*/
void Uart_Port_Init(void);
/*串口操作句柄获取*/
Uart_Dev_Handle_t *Uart_Port_Get_Handle(UART_NUM_Typedef_t uart_num);
/*串口发送数据*/
bool Uart_Port_Transmit_Data(Uart_Dev_Handle_t *uart_opt_handle, uint8_t *data, uint16_t size, uint32_t BlockTime);
/*串口接收中断处理*/
void USER_UART_IRQHandler(UART_HandleTypeDef *huart);
/*设置通讯波特率*/
void Uart_Port_Set_Baudrate(UART_NUM_Typedef_t uart_num, uint32_t BaudRate);
#if USE_USB_CDC
  /*数据接收中断*/
  void Uart_Port_IRQHandler(UART_NUM_Typedef_t uart_num, const uint8_t *data, uint32_t size);
#endif

#ifdef __cplusplus //end extern c
}
#endif
#endif
