/**
 *  @file User_Main.h
 *
 *  @date 2021/10/1
 *
 *  @author Copyright (c) 2021 aron566 <aron566@163.com>.
 *
 *  @brief None
 *  
 *  @version v1.0
 */
#ifndef USER_MAIN_H
#define USER_MAIN_H
/** Includes -----------------------------------------------------------------*/
#include <stdint.h> /*need definition of uint8_t*/
#include <stddef.h> /*need definition of NULL*/
#include <stdbool.h>/*need definition of BOOL*/
#include <stdio.h>  /*if need printf*/
#include <stdlib.h>
#include <string.h>
#include <limits.h> /**< if need INT_MAX*/
/** Private includes ---------------------------------------------------------*/
#include "CircularQueue.h"
#include "I2S_Audio_Port.h"
#include "Timer_Port.h"
#include "UART_Port.h"
/* Use C compiler ------------------------------------------------------------*/
#ifdef __cplusplus ///< use C compiler
extern "C" {
#endif
/** Private defines ----------------------------------------------------------*/

/** Exported typedefines -----------------------------------------------------*/

/** Exported constants -------------------------------------------------------*/
/** Exported macros-----------------------------------------------------------*/
/** Exported variables -------------------------------------------------------*/
/** Exported functions prototypes --------------------------------------------*/

/*初始化入口*/
void User_Main_Init(void);
/*主循环入口*/
void User_Main_Loop(void);

#ifdef __cplusplus ///<end extern c
}
#endif
#endif
/******************************** End of file *********************************/
