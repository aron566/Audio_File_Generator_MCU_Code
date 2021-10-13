/**
 *  @file User_Main.c
 *
 *  @date 2021/10/1
 *
 *  @author aron566
 *
 *  @copyright Copyright (c) 2021 aron566 <aron566@163.com>.
 *
 *  @brief None
 *
 *  @details 应用层入口
 *
 *  @version v1.0
 */
/** Includes -----------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
#include "User_Main.h"
/* Use C compiler ------------------------------------------------------------*/
#ifdef __cplusplus ///< use C compiler
extern "C" {
#endif
/** Private typedef ----------------------------------------------------------*/
/** Private macros -----------------------------------------------------------*/
/** Private constants --------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/
/** Private variables --------------------------------------------------------*/
/** Private function prototypes ----------------------------------------------*/
/** Private user code --------------------------------------------------------*/

/** Private application code -------------------------------------------------*/
/*******************************************************************************
*
*       Static code
*
********************************************************************************
*/
/** Public application code --------------------------------------------------*/
/*******************************************************************************
*
*       Public code
*
********************************************************************************
*/
/**
  ******************************************************************
  * @brief   主循环入口
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/10/1
  ******************************************************************
  */  
void User_Main_Loop(void)
{
  /*定时器接口启动*/
  Timer_Port_Start();
  
  /*音频接口启动*/
  SPI_Audio_Port_Start();
}

/**
  ******************************************************************
  * @brief   初始化入口
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/10/1
  ******************************************************************
  */
void User_Main_Init(void)
{
  /*串口接口初始化*/
  Uart_Port_Init();
  
  /*定时器接口初始化*/
  Timer_Port_Init();
  
  /*音频接口初始化*/
  SPI_Audio_Port_Init();
}

#ifdef __cplusplus ///<end extern c
}
#endif
/******************************** End of file *********************************/
