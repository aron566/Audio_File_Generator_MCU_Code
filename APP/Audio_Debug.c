/**                                                                             
 *  @file Audio_Debug.c                                                   
 *                                                                              
 *  @date 2021年09月28日 08:55:20 星期二
 *
 *  @author aron566
 *
 *  @copyright Copyright (c) 2021 aron566 <aron566@163.com>.
 *
 *  @brief 音频数据打包发送至上位机，由上位机合并成wav文件.
 *
 *  @details 1、发送区大小应至少满足：AUDIO_DEBUG_FRAME_STEREO_SIZE.
 *           2、发送音频至少为双通道，通过设置将某一通道数据复制到另一通道.
 *           3、最大支持8通道数据传输，发送缓冲区必须随之增大 8*AUDIO_DEBUG_FRAME_MONO_SIZE.
 *           4、数据格式：LEFT RIGHT LEFT RIGHT......
 *           5、多通道下数据格式：CH1 CH2 CH3 .... CH1 CH2 CH3 ....
 *
 *  @version V1.0
 */
/** Includes -----------------------------------------------------------------*/

/* Private includes ----------------------------------------------------------*/
#include "Audio_Debug.h"
#include "CircularQueue.h"

/** Use C compiler -----------------------------------------------------------*/
#ifdef __cplusplus ///< use C compiler
extern "C" {
#endif
/** Private typedef ----------------------------------------------------------*/
/*发送区音频*/
typedef struct 
{
  uint16_t *Send_Buf_Ptr;
  SEND_DATA_FUNC_PORT_Typedef_t Send_Audio_Data;
  GET_IDEL_STATE_PORT_Typedef_t Get_Idel_State;
}SEND_BUF_Typedef_t;
                                                     
/** Private macros -----------------------------------------------------------*/
#define AUDIO_DATA_BUF_SIZE CQ_BUF_2KB//(CHANNEL_8_EN*AUDIO_DEBUG_FRAME_MONO_SIZE)/**< 环形缓冲区大小 取2K*/                                                                                 
/** Private constants --------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/
/** Private variables --------------------------------------------------------*/
/*音频缓冲区*/
static uint16_t Audio_Data_Buf[AUDIO_DATA_BUF_SIZE];
static CQ_handleTypeDef CQ_Audio_Data_Handle;

/*发送设置*/
static AUDIO_DEBUG_CHANNEL_SEL_Typedef_t Current_Channel_Sel = CHANNEL_2_EN;
static uint32_t Current_Send_Size = AUDIO_DEBUG_FRAME_STEREO_SIZE;
/*发送区*/
static SEND_BUF_Typedef_t Send_Region;
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
  * @brief   音频调试通道使能
  * @param   [in]Ch_Sel 使能通道. 
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-09-28
  ******************************************************************
  */
static void Audio_Debug_Channel_Set(AUDIO_DEBUG_CHANNEL_SEL_Typedef_t Ch_Sel)
{
  if(Current_Channel_Sel == Ch_Sel || Ch_Sel == CHANNEL_0_NONE)
  {
    return;
  }
  Current_Channel_Sel = Ch_Sel;
  uint32_t Number = (uint32_t)Current_Channel_Sel > (uint32_t)CHANNEL_2_EN?(uint32_t)Current_Channel_Sel:(uint32_t)CHANNEL_2_EN;
  Current_Send_Size = Number * AUDIO_DEBUG_FRAME_MONO_SIZE;
  CQ_emptyData(&CQ_Audio_Data_Handle);
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
  * @brief   音频调试启动
  * @param   [in]None. 
  * @return  true 成功
  * @author  aron566
  * @version V1.0
  * @date    2021-09-28
  ******************************************************************
  */
bool Audio_Debug_Start(void)
{
  uint32_t Len = CQ_getLength(&CQ_Audio_Data_Handle);
  if(Len < Current_Send_Size)
  {
    return false;
  }
  if(Send_Region.Get_Idel_State() == false)
  {
    return false;
  }
  CQ_16getData(&CQ_Audio_Data_Handle, Send_Region.Send_Buf_Ptr, Current_Send_Size);
  Send_Region.Send_Audio_Data((uint8_t *)Send_Region.Send_Buf_Ptr, Current_Send_Size * sizeof(int16_t));
  return true;
}

/**
  ******************************************************************
  * @brief   音频数据打包发送
  * @param   [in]Left_Audio_Data 左音频数据
  * @param   [in]Right_Audio_Data 右音频数据
  * @param   [in]Channel_Number 其他通道总数
  * @param   [in]... 其他通道数据
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-09-28
  ******************************************************************
  */
void Audio_Debug_Put_Data(const int16_t *Left_Audio_Data, const int16_t *Right_Audio_Data, uint8_t Channel_Number, ...)
{
  int16_t Audio_Data[8*AUDIO_DEBUG_FRAME_MONO_SIZE];
  
  va_list args;
  
  uint32_t index = 0;
  /*更新当前通道*/
  Audio_Debug_Channel_Set((AUDIO_DEBUG_CHANNEL_SEL_Typedef_t) Channel_Number);
  
  for(uint32_t i = 0; i < AUDIO_DEBUG_FRAME_MONO_SIZE; i++)
  {
    switch(Current_Channel_Sel)
    {
      case CHANNEL_1_EN:
        Audio_Data[index++] = Left_Audio_Data[i];
        Audio_Data[index++] = Left_Audio_Data[i];
        break;
      case CHANNEL_2_EN:
        Audio_Data[index++] = Left_Audio_Data[i];
        Audio_Data[index++] = Right_Audio_Data[i];
        break;
      case CHANNEL_3_EN:
      case CHANNEL_4_EN:
      case CHANNEL_5_EN:
      case CHANNEL_6_EN:
      case CHANNEL_7_EN:
      case CHANNEL_8_EN:
      {
        Audio_Data[index++] = Left_Audio_Data[i];
        Audio_Data[index++] = Right_Audio_Data[i];
        
        /* args point to the first variable parameter */
        va_start(args, Channel_Number);
        for(uint8_t Channel_Index = 0; Channel_Index < Channel_Number; Channel_Index++)
        {
          Audio_Data[index++] = (va_arg(args, uint16_t *))[i];
        }
        break; 
      }
      default:
        Audio_Data[index++] = 0;
        Audio_Data[index++] = 0;
        break;
    }
  }
  va_end(args);
  CQ_16putData(&CQ_Audio_Data_Handle, (const uint16_t *)Audio_Data, (2 + Channel_Number) * AUDIO_DEBUG_FRAME_MONO_SIZE);
}

/**
  ******************************************************************
  * @brief   音频调试初始化
  * @param   [in]Send_Buf.
  * @param   [in]Send_Data_Func 发送数据接口.
  * @param   [in]Get_Idel_Func 获取空闲状态.
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-09-28
  ******************************************************************
  */
void Audio_Debug_Init(uint16_t *Send_Buf, SEND_DATA_FUNC_PORT_Typedef_t Send_Data_Func, GET_IDEL_STATE_PORT_Typedef_t Get_Idel_Func)
{
  /*初始化发送区*/
  Send_Region.Send_Buf_Ptr = Send_Buf;
  Send_Region.Send_Audio_Data = Send_Data_Func;
  Send_Region.Get_Idel_State = Get_Idel_Func;

  /*初始化缓冲区*/
  CQ_16_init(&CQ_Audio_Data_Handle, Audio_Data_Buf, AUDIO_DATA_BUF_SIZE);
}
#ifdef __cplusplus ///<end extern c                                             
}                                                                               
#endif                                                                          
/******************************** End of file *********************************/
