/**
 *  @file USB_Audio_Port.c
 *
 *  @date 2021-06-01
 *
 *  @author aron566
 *
 *  @copyright Copyright (c) 2021 aron566 <aron566@163.com>.
 *
 *  @brief USB音频类接口，对外提供音频更新及输出接口
 *
 *  @details 1、开启对应FS中断，中断处理和初始化位于usbd_conf.c
 *           2、16k采样，双声道，10ms出320点数据
 *           3、1ms间隔发送，10ms发送10次，每次发送320/10 = 32点数据 数据大小64字节（16bit*32）
 *           4、接收来自MIC数据，10ms来一次每次双通道160点*2
 *  @version V1.0
 */
/** Includes -----------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
#include "USB_Audio_Port.h"
#include "main.h"
#include "usbd_audio.h"
/** Use C compiler -----------------------------------------------------------*/
#ifdef __cplusplus ///<use C compiler
extern "C" {
#endif
/** Private typedef ----------------------------------------------------------*/
/** Private macros -----------------------------------------------------------*/
#define USB_RX_BUF_SIZE_MAX       1024 /**< 接收缓冲区设置1024*2Bytes*/

#define USB_PORT_AUDIO_OUT_PACKET AUDIO_PORT_OUT_SIZE    /**< 一次发送大小字节数*/
#define USB_PORT_AUDIO_BUF_SIZE   AUDIO_TOTAL_BUF_SIZE
#define USB_PORT_AUDIO_IN_EP      AUDIO_PORT_IN_EP_DIR_ID
#define USB_PORT_AUDIO_OUT_EP     AUDIO_PORT_OUT_EP_DIR_ID
/** Private constants --------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/
/** Private variables --------------------------------------------------------*/
/*音频缓冲区*/
static CQ_handleTypeDef USB_Audio_Data_Handle;
static int16_t USB_Audio_Receive_Buf[STEREO_FRAME_SIZE] = {0};
static uint16_t USB_Audio_Send_Buf[USB_RX_BUF_SIZE_MAX];
/** Private function prototypes ----------------------------------------------*/

/** Private user code --------------------------------------------------------*/

/** Private application code -------------------------------------------------*/
/*******************************************************************************
*
*       Static code
*
********************************************************************************
*/
/**
  ******************************************************************
  * @brief   USB音频接口初始化
  * @param   [in]None.
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-06-02
  ******************************************************************
  */
static void USB_Audio_Port_Init(void)
{
  /*初始化接收音频缓冲区*/
  CQ_16_init(&USB_Audio_Data_Handle, USB_Audio_Send_Buf, USB_RX_BUF_SIZE_MAX);
}

/**
  ******************************************************************
  * @brief   向USB缓冲区数据加入数据
  * @param   [in]Data 待加入数据.
  * @param   [in]Size 待加入数据长度Size个16Bit.
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-06-04
  ******************************************************************
  */
static inline void USB_Audio_Port_Put_Audio_Data(const int16_t *Data, uint32_t Size)
{
  CQ_16putData(&USB_Audio_Data_Handle, (const uint16_t *)Data, Size);
}

/** Public application code --------------------------------------------------*/
/*******************************************************************************
*
*       Public code
*
********************************************************************************
*/

/**
  ******************************************************************
  * @brief   USB缓冲区数据接收来自HOST
  * @param   [in]pdev device instance
  * @param   [in]epnum 端点号
  * @return  USBD_OK 正常.
  * @author  aron566
  * @version V1.0
  * @date    2021-06-01
  ******************************************************************
  */
uint8_t USB_Audio_Port_DataOut(void *xpdev, uint8_t epnum)
{
  uint16_t PacketSize;
  USBD_AUDIO_HandleTypeDef *haudio;
  USBD_HandleTypeDef *pdev = (USBD_HandleTypeDef *)xpdev;
  
  haudio = (USBD_AUDIO_HandleTypeDef *)pdev->pClassData;

  if (haudio == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  if (epnum == USB_PORT_AUDIO_OUT_EP)
  {
    /* Get received data packet length */
    PacketSize = (uint16_t)USBD_LL_GetRxDataSize(pdev, epnum);

    /* Packet received Callback */
    ((USBD_AUDIO_ItfTypeDef *)pdev->pUserData)->PeriodicTC(&haudio->buffer[haudio->wr_ptr],
                                                           PacketSize, AUDIO_OUT_TC);

    /* Increment the Buffer pointer or roll it back when all buffers are full */
    haudio->wr_ptr += PacketSize;

    if (haudio->wr_ptr == AUDIO_TOTAL_BUF_SIZE)
    {
      /* All buffers are full: roll back */
      haudio->wr_ptr = 0U;

      if (haudio->offset == AUDIO_OFFSET_UNKNOWN)
      {
        ((USBD_AUDIO_ItfTypeDef *)pdev->pUserData)->AudioCmd(&haudio->buffer[0],
                                                             AUDIO_TOTAL_BUF_SIZE / 2U,
                                                             AUDIO_CMD_START);
        haudio->offset = AUDIO_OFFSET_NONE;
      }
    }

    if (haudio->rd_enable == 0U)
    {
      if (haudio->wr_ptr == (AUDIO_TOTAL_BUF_SIZE / 2U))
      {
        haudio->rd_enable = 1U;
      }
    }

    /* Prepare Out endpoint to receive next audio packet */
    (void)USBD_LL_PrepareReceive(pdev, USB_PORT_AUDIO_OUT_EP,
                                 &haudio->buffer[haudio->wr_ptr],
                                 AUDIO_OUT_PACKET);
  }

  return (uint8_t)USBD_OK;
}

/**
  ******************************************************************
  * @brief   USB缓冲区数据发送至HOST
  * @param   [in]pdev device instance
  * @param   [in]epnum 端点号
  * @return  USBD_OK 正常.
  * @author  aron566
  * @version V1.0
  * @date    2021-06-01
  ******************************************************************
  */
uint8_t USB_Audio_Port_DataIn(void *xpdev, uint8_t epnum)
{
  USBD_HandleTypeDef *pdev = (USBD_HandleTypeDef *)xpdev;
  USBD_AUDIO_HandleTypeDef *haudio = (USBD_AUDIO_HandleTypeDef*) pdev->pClassData;
  
	USBD_LL_FlushEP(pdev, USB_PORT_AUDIO_IN_EP);
  
  if(CQ_getLength(&USB_Audio_Data_Handle) < USB_PORT_AUDIO_OUT_PACKET/2)
  {
    return (uint8_t)USBD_BUSY;
  }
  CQ_16getData(&USB_Audio_Data_Handle, (uint16_t *)haudio->buffer, USB_PORT_AUDIO_OUT_PACKET/2);
  return USBD_LL_Transmit(pdev, USB_PORT_AUDIO_IN_EP, haudio->buffer, USB_PORT_AUDIO_OUT_PACKET);
}

/**
  ******************************************************************
  * @brief   反初始化音频输出端点
  * @param   [in]pdev
  * @param   [in]cfgidx
  * @return  USBD_OK 正常.
  * @author  aron566
  * @version V1.0
  * @date    2021-06-01
  ******************************************************************
  */
uint8_t USB_Audio_Port_EP_OUT_DeInit(void *xpdev, uint8_t cfgidx)
{
  UNUSED(cfgidx);
  USBD_HandleTypeDef *pdev = (USBD_HandleTypeDef *)xpdev;
  
  /* Open EP OUT */
  (void)USBD_LL_CloseEP(pdev, USB_PORT_AUDIO_OUT_EP);
  pdev->ep_out[USB_PORT_AUDIO_OUT_EP & 0xFU].is_used = 0U;
  pdev->ep_out[USB_PORT_AUDIO_OUT_EP & 0xFU].bInterval = 0U;

  /* DeInit  physical Interface components */
  if (pdev->pClassData != NULL)
  {
    ((USBD_AUDIO_ItfTypeDef *)pdev->pUserData)->DeInit(0U);
    (void)USBD_free(pdev->pClassData);
    pdev->pClassData = NULL;
  }

  return (uint8_t)USBD_OK;
}

/**
  ******************************************************************
  * @brief   反初始化音频输入端点
  * @param   [in]pdev
  * @param   [in]cfgidx
  * @return  USBD_OK 正常.
  * @author  aron566
  * @version V1.0
  * @date    2021-06-01
  ******************************************************************
  */
uint8_t USB_Audio_Port_EP_IN_DeInit(void *xpdev, uint8_t cfgidx)
{
  UNUSED(cfgidx);
  USBD_HandleTypeDef *pdev = (USBD_HandleTypeDef *)xpdev;
  
  /* Open EP OUT */  
  USBD_LL_CloseEP(pdev, USB_PORT_AUDIO_IN_EP);
  
  /* DeInit  physical Interface components */
  if (pdev->pClassData != NULL)
  {
    ((USBD_AUDIO_ItfTypeDef *)pdev->pUserData)->DeInit(0U);
    (void)USBD_free(pdev->pClassData);
    pdev->pClassData = NULL;
  }
  return (uint8_t)USBD_OK;
}

/**
  ******************************************************************
  * @brief   初始化为音频输出端点
  * @param   [in]pdev
  * @param   [in]cfgidx
  * @return  USBD_OK 正常.
  * @author  aron566
  * @version V1.0
  * @date    2021-06-01
  ******************************************************************
  */
uint8_t USB_Audio_Port_EP_OUT_Init(void *xpdev, uint8_t cfgidx)
{
  UNUSED(cfgidx);
  USBD_AUDIO_HandleTypeDef *haudio;
  USBD_HandleTypeDef *pdev = (USBD_HandleTypeDef *)xpdev;
  
  /* Allocate Audio structure */
  haudio = USBD_malloc(sizeof(USBD_AUDIO_HandleTypeDef));

  if (haudio == NULL)
  {
    pdev->pClassData = NULL;
    return (uint8_t)USBD_EMEM;
  }

  pdev->pClassData = (void *)haudio;
    
  if (pdev->dev_speed == USBD_SPEED_HIGH)
  {
    pdev->ep_out[USB_PORT_AUDIO_OUT_EP & 0xFU].bInterval = AUDIO_HS_BINTERVAL;
  }
  else   /* LOW and FULL-speed endpoints */
  {
    pdev->ep_out[USB_PORT_AUDIO_OUT_EP & 0xFU].bInterval = AUDIO_FS_BINTERVAL;
  }

  /* Open EP OUT */
  (void)USBD_LL_OpenEP(pdev, USB_PORT_AUDIO_OUT_EP, USBD_EP_TYPE_ISOC, USB_PORT_AUDIO_OUT_PACKET);
  pdev->ep_out[USB_PORT_AUDIO_OUT_EP & 0xFU].is_used = 1U;

  haudio->alt_setting = 0U;
  haudio->offset = AUDIO_OFFSET_UNKNOWN;
  haudio->wr_ptr = 0U;
  haudio->rd_ptr = 0U;
  haudio->rd_enable = 0U;

  /* Initialize the Audio output Hardware layer */
  if (((USBD_AUDIO_ItfTypeDef *)pdev->pUserData)->Init(USBD_AUDIO_FREQ,
                                                       AUDIO_DEFAULT_VOLUME,
                                                       0U) != 0U)
  {
    return (uint8_t)USBD_FAIL;
  }

  /* Prepare Out endpoint to receive 1st packet */
  (void)USBD_LL_PrepareReceive(pdev, USB_PORT_AUDIO_OUT_EP, haudio->buffer,
                               AUDIO_OUT_PACKET);
  return (uint8_t)USBD_OK; 
}

/**
  ******************************************************************
  * @brief   初始化为音频输入端点
  * @param   [in]pdev
  * @param   [in]cfgidx
  * @return  USBD_OK 正常.
  * @author  aron566
  * @version V1.0
  * @date    2021-06-01
  ******************************************************************
  */
uint8_t USB_Audio_Port_EP_IN_Init(void *xpdev, uint8_t cfgidx)
{
  UNUSED(cfgidx);
  USBD_AUDIO_HandleTypeDef *haudio;
  USBD_HandleTypeDef *pdev = (USBD_HandleTypeDef *)xpdev;
  
  /*USB接口初始化*/
  USB_Audio_Port_Init();
  
  /* Allocate Audio structure */
  haudio = USBD_malloc(sizeof(USBD_AUDIO_HandleTypeDef));

  if (haudio == NULL)
  {
    pdev->pClassData = NULL;
    return (uint8_t)USBD_EMEM;
  }

  pdev->pClassData = (void *)haudio;
  
  if (pdev->dev_speed == USBD_SPEED_HIGH)
  {
    pdev->ep_in[USB_PORT_AUDIO_IN_EP & 0xFU].bInterval = AUDIO_HS_BINTERVAL;
  }
  else   /* LOW and FULL-speed endpoints */
  {
    pdev->ep_in[USB_PORT_AUDIO_IN_EP & 0xFU].bInterval = AUDIO_FS_BINTERVAL;
  }
  
    /* Open EP IN */
  (void)USBD_LL_OpenEP(pdev, USB_PORT_AUDIO_IN_EP, USBD_EP_TYPE_ISOC, USB_PORT_AUDIO_OUT_PACKET); 
  pdev->ep_in[USB_PORT_AUDIO_IN_EP & 0xFU].is_used = 1U;
  
  haudio->alt_setting = 0U;
  haudio->offset = AUDIO_OFFSET_UNKNOWN;
  haudio->wr_ptr = 0U;
  haudio->rd_ptr = 0U;
  haudio->rd_enable = 0U;
  
  memset(haudio->buffer, 0, USB_PORT_AUDIO_BUF_SIZE);

  USBD_LL_Transmit(pdev, USB_PORT_AUDIO_IN_EP, haudio->buffer, USB_PORT_AUDIO_OUT_PACKET);
  return (uint8_t)USBD_OK;
}

/**
  ******************************************************************
  * @brief   更新USB音频数据
  * @param   [in]Left_Audio 左通道数据
  * @param   [in]Right_Audio 右通道数据
  * @param   [in]Size 右通道总点数,不可大于STEREO_FRAME_SIZE
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-06-01
  ******************************************************************
  */
void USB_Audio_Port_Put_Data(const int16_t *Left_Audio, const int16_t *Right_Audio, int Size)
{
  /*更新USB音频数据*/
  int index = 0, i = 0;
  for(i = 0; i < Size; index++)
  {
    USB_Audio_Receive_Buf[i++] = Left_Audio[index];/**< TO USB LEFT*/
    USB_Audio_Receive_Buf[i++] = Right_Audio[index];/**< TO USB RIGHT*/
  }
  
  USB_Audio_Port_Put_Audio_Data(USB_Audio_Receive_Buf, Size);
}

/**
  ******************************************************************
  * @brief   是否可以更新音频数据
  * @param   [in]None.
  * @return  true 可以.
  * @author  aron566
  * @version V1.0
  * @date    2021-10-03
  ******************************************************************
  */
bool USB_Audio_Port_Can_Put_Data(void)
{
  if(USB_Audio_Data_Handle.size - CQ_getLength(&USB_Audio_Data_Handle) >= STEREO_FRAME_SIZE)
  {
    return true;
  }
  return false;
}

#ifdef __cplusplus ///<end extern c
}
#endif
/******************************** End of file *********************************/
