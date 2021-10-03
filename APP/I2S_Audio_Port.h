/**
 *  @file I2S_Audio_Port.h
 *
 *  @date 2021/10/1
 *
 *  @author Copyright (c) 2021 aron566 <aron566@163.com>.
 *
 *  @brief None
 *  
 *  @version v1.0
 */
#ifndef I2S_AUDIO_PORT_H
#define I2S_AUDIO_PORT_H
/** Includes -----------------------------------------------------------------*/
#include <stdint.h> /*need definition of uint8_t*/
#include <stddef.h> /*need definition of NULL*/
#include <stdbool.h>/*need definition of BOOL*/
#include <stdio.h>  /*if need printf*/
#include <stdlib.h>
#include <string.h>
#include <limits.h> /**< if need INT_MAX*/
/** Private includes ---------------------------------------------------------*/
/* Use C compiler ------------------------------------------------------------*/
#ifdef __cplusplus ///< use C compiler
extern "C" {
#endif
/** Private defines ----------------------------------------------------------*/

/** Exported typedefines -----------------------------------------------------*/

/** Exported constants -------------------------------------------------------*/
/** Exported macros-----------------------------------------------------------*/
#define MONO_FRAME_SIZE                       128
#define STEREO_FRAME_SIZE                     (MONO_FRAME_SIZE*2U)  
/** Exported variables -------------------------------------------------------*/
/** Exported functions prototypes --------------------------------------------*/

/*音频接口初始化*/
void I2S_Audio_Port_Init(void);
/*音频接口启动*/
void I2S_Audio_Port_Start(void);
/*音频接口任务使能*/
void I2S_Audio_Port_Task_Start(void);

#ifdef __cplusplus ///<end extern c
}
#endif
#endif
/******************************** End of file *********************************/
