/**
 *  @file Timer_Port.c
 *
 *  @date 2021-10-01
 *
 *  @author aron566
 *
 *  @copyright Copyright (c) 2021 aron566 <aron566@163.com>.
 *
 *  @brief 定时任务接口
 *
 *  @details 1、
 *
 *  @version V1.0
 */
#ifdef __cplusplus ///<use C compiler
extern "C" {
#endif
/** Includes -----------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
#include "Timer_Port.h"
#include "main.h"  
/** Private typedef ----------------------------------------------------------*/

/** Private macros -----------------------------------------------------------*/
#define ENABLE_SYSTICK_COUNT  1/**< 是否启用Systick计数*/
/** Private constants --------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/
extern TIM_HandleTypeDef htim1;
/** Private variables --------------------------------------------------------*/
static uint32_t Timer_Port_TimeMS  = 0;
static uint32_t Timer_Port_TimeSec = 0;
/** Private function prototypes ----------------------------------------------*/
static inline void Timer_Port_IRQHandler(void);
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
  * @brief   定时器中断回调
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-01-13
  ******************************************************************
  */
static inline void Timer_Port_IRQHandler(void)
{
  Timer_Port_TimeMS++;
  if(Timer_Port_TimeMS == 1000)
  {
    Timer_Port_TimeMS = 0;
    Timer_Port_TimeSec++;
  }
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
  * @brief   定时器周期时间到回调
  * @param   [in]htim 定时器句柄
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-02-25
  ******************************************************************
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim->Instance == TIM1)
  {
    /*音频接口启动*/
    I2S_Audio_Port_Task_Start();
  }
}

/**
  ******************************************************************
  * @brief   滴答时钟回调
  * @param   [in]None.
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2020-02-25
  ******************************************************************
  */
void HAL_SYSTICK_Callback(void)
{
#if ENABLE_SYSTICK_COUNT
  Timer_Port_IRQHandler();
#endif
}


/**
  ******************************************************************
  * @brief   获取当前运行累计时间
  * @param   [in]time_base 单位
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-01-13
  ******************************************************************
  */
uint32_t Timer_Port_Get_Current_Time(TIMER_TIME_UNIT_Typedef_t time_unit)
{
  return (time_unit == TIMER_MS)?Timer_Port_TimeMS:Timer_Port_TimeSec;
}

/**
  ******************************************************************
  * @brief   定时器接口启动
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-01-13
  ******************************************************************
  */
void Timer_Port_Start(void)
{

}

/**
  ******************************************************************
  * @brief   定时器初始化
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-04-19
  ******************************************************************
  */
void Timer_Port_Init(void)
{
  /*启动定时器*/
  HAL_TIM_Base_Start_IT(&htim1);
}

/******************************** End of file *********************************/
