/**
 *  @file I2S_Audio_Port.c
 *
 *  @date 2021/10/1
 *
 *  @author aron566
 *
 *  @copyright Copyright (c) 2021 aron566 <aron566@163.com>.
 *
 *  @brief 音频传输控制接口
 *
 *  @details None
 *
 *  @version v1.0
 */
/** Includes -----------------------------------------------------------------*/
#include <math.h>
/* Private includes ----------------------------------------------------------*/
#include "I2S_Audio_Port.h"
#include "USB_Audio_Port.h"
#include "Audio_Debug.h"
#include "main.h"
/* Use C compiler ------------------------------------------------------------*/
#ifdef __cplusplus ///< use C compiler
extern "C" {
#endif
/** Private typedef ----------------------------------------------------------*/
/** Private macros -----------------------------------------------------------*/
#define SIN_WAVE_SAMPLE_RATE	16000 /**< 16K采样*/
#define SIN_WAVE_FQ				    250   /**< 100Hz正弦*/
#define SIN_WAVE_MAX_POINTS		SIN_WAVE_SAMPLE_RATE/SIN_WAVE_FQ
#define SIN_WAVE_DB_VAL 		  60.l
  
/** Private constants --------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/
extern I2S_HandleTypeDef hi2s2;  
/** Private variables --------------------------------------------------------*/
/*音频缓冲区*/
static int16_t Audio_Data_Rec_Buf[STEREO_FRAME_SIZE];
/*测试音频缓冲区*/
static int16_t Sin_Wave_PCM_Buf[SIN_WAVE_MAX_POINTS];
/*音频发送区*/
static int16_t Audio_Data_Send_Buf[STEREO_FRAME_SIZE];
/*音频调试缓冲区*/
static int16_t Debug_Auido_Buf[STEREO_FRAME_SIZE];
/*音频标志位*/
static volatile int16_t *Current_Opt_Rec_Buf_Sel = Audio_Data_Rec_Buf;
static volatile uint8_t Received_Ok_Flag = 0;
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
  * @brief   正弦生成
  * @param   [in]None.
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-06-01
  ******************************************************************
  */
static void Sin_Audio_Init(void)
{  
  float Average_Range = (float)pow(10.L, (double)((double)SIN_WAVE_DB_VAL - 94.f) / 20.L)*32767.f;
  for(int i = 0; i < SIN_WAVE_MAX_POINTS; i++)
  {
    Sin_Wave_PCM_Buf[i] = (int16_t)(Average_Range * sin(2.f*3.14f * SIN_WAVE_FQ / SIN_WAVE_SAMPLE_RATE * i));
  }
}

/**
  ******************************************************************
  * @brief   测试USB音频数据
  * @param   None.
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-06-01
  ******************************************************************
  */
static void Test_Audio_Port_Put_Data(void)
{
  /*更新USB音频数据*/
  static int index = 0;
  for(int i = 0; i < MONO_FRAME_SIZE; i++)
  {
    Audio_Data_Send_Buf[i] = Sin_Wave_PCM_Buf[index];/**< TO USB LEFT*/
    Audio_Data_Send_Buf[MONO_FRAME_SIZE+i] = Sin_Wave_PCM_Buf[index];/**< TO USB RIGHT*/
    index = ((index+1)%(SIN_WAVE_MAX_POINTS));
  }
}

/**
  ******************************************************************
  * @brief   发送数据接口
  * @param   Data 数据.
  * @param   Len 数据字节数.
  * @return  已发送长度.
  * @author  aron566
  * @version V1.0
  * @date    2021-10-02
  ******************************************************************
  */
static uint32_t Send_Data_Func_Port(uint8_t *Data, uint32_t Len)
{
  int16_t *Ptr = (int16_t *)Data;
  /*分离左右通道数据*/
  int16_t Left_Audio[MONO_FRAME_SIZE], Right_Audio[MONO_FRAME_SIZE];
  int index = 0;
  for(int i = 0; i < MONO_FRAME_SIZE; i++)
  {
    Left_Audio[i] = Ptr[index++];
    Right_Audio[i] = Ptr[index++];
  }
  /*发送音频数据到USB*/ 
  USB_Audio_Port_Put_Data(Left_Audio, Right_Audio, Len/sizeof(int16_t));
  return Len;
}

/**
  ******************************************************************
  * @brief   获取发送空闲状态
  * @param   None.
  * @return  true 空闲.
  * @author  aron566
  * @version V1.0
  * @date    2021-10-02
  ******************************************************************
  */
static bool Get_Idel_State_Port(void)
{
  if(Received_Ok_Flag == 0)
  {
    return false;
  }
  return true;
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
  * @brief   音频半传输完成中断
  * @param   [in]hi2s 句柄
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/10/1
  ******************************************************************
  */
void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
  (void)(hi2s);
  Current_Opt_Rec_Buf_Sel = Audio_Data_Rec_Buf;
  Received_Ok_Flag = 1;
}

/**
  ******************************************************************
  * @brief   音频传输完成中断
  * @param   [in]hi2s 句柄
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/10/1
  ******************************************************************
  */
void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s)
{
  (void)(hi2s);
  Current_Opt_Rec_Buf_Sel = &Audio_Data_Rec_Buf[MONO_FRAME_SIZE];
  Received_Ok_Flag = 1;
}

/**
  ******************************************************************
  * @brief   音频接口任务使能
  * @param   [in]None.
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/10/1
  ******************************************************************
  */
void I2S_Audio_Port_Task_Start(void)
{
  Received_Ok_Flag = 1;
}

/**
  ******************************************************************
  * @brief   音频接口启动
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/10/1
  ******************************************************************
  */
void I2S_Audio_Port_Start(void)
{
  if(Received_Ok_Flag == 0)
  {
    return;
  }
  /*加入音频到调试接口 -> USB*/
  if(USB_Audio_Port_Can_Put_Data() == false)
  {
    return;
  }
  Test_Audio_Port_Put_Data();
  Audio_Debug_Put_Data(Audio_Data_Send_Buf, &Audio_Data_Send_Buf[MONO_FRAME_SIZE], 0);
  Audio_Debug_Start();
  
  /*取出音频数据给USB*/
//  Test_Audio_Port_Put_Data();
//  USB_Audio_Port_Put_Data(Audio_Data_Send_Buf, &Audio_Data_Send_Buf[MONO_FRAME_SIZE], STEREO_FRAME_SIZE);
  
  Received_Ok_Flag = 0;
}

/**
  ******************************************************************
  * @brief   音频接口初始化
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/10/1
  ******************************************************************
  */
void I2S_Audio_Port_Init(void)
{
  /*正弦音频*/
  Sin_Audio_Init();
  
  /*初始化音频调试接口*/
  Audio_Debug_Init((uint16_t *)Debug_Auido_Buf, Send_Data_Func_Port, Get_Idel_State_Port);
  
  /*启动接收*/
  HAL_I2S_Receive_DMA(&hi2s2, (uint16_t *)Audio_Data_Rec_Buf, STEREO_FRAME_SIZE);
}

#ifdef __cplusplus ///<end extern c
}
#endif
/******************************** End of file *********************************/
