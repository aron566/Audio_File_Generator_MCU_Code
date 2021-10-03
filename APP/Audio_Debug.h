/**                                                                             
 *  @file Audio_Debug.h                                                    
 *                                                                              
 *  @date 2021年09月28日 08:54:51 星期二
 *                                                                              
 *  @author Copyright (c) 2021 aron566 <aron566@163.com>.                       
 *                                                                              
 *  @brief None.                                                                
 *                                                                              
 *  @version V1.0                                                               
 */                                                                             
#ifndef AUDIO_DEBUG_H                                                          
#define AUDIO_DEBUG_H                                                          
/** Includes -----------------------------------------------------------------*/
#include <stdint.h> /**< need definition of uint8_t */                          
#include <stddef.h> /**< need definition of NULL    */                          
#include <stdbool.h>/**< need definition of BOOL    */                        
#include <stdio.h>  /**< if need printf             */                          
#include <stdlib.h>                                                             
#include <string.h>                                                             
#include <limits.h> /**< need variable max value    */ 
#include <stdarg.h>                         
/** Private includes ---------------------------------------------------------*/
#include "User_Main.h"                                                                     
/** Use C compiler -----------------------------------------------------------*/
#ifdef __cplusplus ///< use C compiler                                          
extern "C" {                                                                  
#endif                                                                          
/** Private defines ----------------------------------------------------------*/
                                                                        
/** Exported typedefines -----------------------------------------------------*/
/*通道使能*/
typedef enum
{
  CHANNEL_0_NONE = 0,
  CHANNEL_1_EN,
  CHANNEL_2_EN,
  CHANNEL_3_EN,
  CHANNEL_4_EN,
  CHANNEL_5_EN,
  CHANNEL_6_EN,
  CHANNEL_7_EN,
  CHANNEL_8_EN,
  CHANNEL_NUMBER_MAX
}AUDIO_DEBUG_CHANNEL_SEL_Typedef_t;

typedef uint32_t (*SEND_DATA_FUNC_PORT_Typedef_t)(uint8_t *, uint32_t);
typedef bool (*GET_IDEL_STATE_PORT_Typedef_t)(void);
/** Exported constants -------------------------------------------------------*/
                                                                                
/** Exported macros-----------------------------------------------------------*/
#define AUDIO_DEBUG_FRAME_MONO_SIZE   MONO_FRAME_SIZE   /**< 单通道数据每帧点数*/
#define AUDIO_DEBUG_FRAME_STEREO_SIZE STEREO_FRAME_SIZE /**< 双通道数据每帧点数*/
/** Exported variables -------------------------------------------------------*/
/** Exported functions prototypes --------------------------------------------*/

/*音频调试初始化*/
void Audio_Debug_Init(uint16_t *Send_Buf, SEND_DATA_FUNC_PORT_Typedef_t Send_Data_Func, GET_IDEL_STATE_PORT_Typedef_t Get_Idel_Func);
/*音频调试启动*/
bool Audio_Debug_Start(void);
/*音频数据打包发送*/
void Audio_Debug_Put_Data(const int16_t *Left_Audio_Data, const int16_t *Right_Audio_Data, uint8_t Channel_Number, ...);

#ifdef __cplusplus ///<end extern c                                             
}                                                                               
#endif                                                                          
#endif                                                                          
/******************************** End of file *********************************/
