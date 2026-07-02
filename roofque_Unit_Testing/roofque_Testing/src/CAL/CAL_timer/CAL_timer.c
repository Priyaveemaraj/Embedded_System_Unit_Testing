/**
 * @file    CAL_timer.c
 * @brief   Timer Abstraction Layer Implementation
 *
 * @details
 * Implements timer utility functions for the Controller Abstraction Layer.
 * These functions wrap STM32 HAL timer functions to isolate hardware
 * dependencies from higher level modules.
 *
 * ---------------------------------------------------------------------------
 * Author           : Aadhithya
 * Created Date     : 07-03-2026
 *
 * Last Modified By : Aadhithya
 * Last Modified On : 10-03-2026
 *
 * Version          : 1.0.1
 *
 * Remarks / Change Log:
 * ---------------------------------------------------------------------------
 * Date         Author        Version     Description
 * ---------------------------------------------------------------------------
 * 07-03-2026   Aadhithya     1.0.0       Initial implementation
 * 10-03-2026	Aadhithya	  1.0.1		  Added RTOS functionalities
 * 10-03-2026	Aadhithya	  1.0.1		  Added CAL_status return type
 * ---------------------------------------------------------------------------
 */

#include "CAL_timer.h"
#ifdef CAL_USE_RTOS
#include "FreeRTOS.h"
#include "cmsis_os.h"
#endif
/**
 * @brief Get current system tick
 *
 * @return uint32_t System tick in milliseconds
 */
uint32_t CAL_GetTimerTicks(void)
{
#ifdef CAL_USE_RTOS
	return osKernelGetTickCount();
#else
    return HAL_GetTick();
#endif
}

/**
 * @brief Delay execution
 *
 * @param milliSeconds Delay time in milliseconds
 */
CAL_status CAL_Delay(uint32_t milliSeconds)
{
#ifdef CAL_USE_RTOS
	osDelay(milliSeconds);
#else
    HAL_Delay(milliSeconds);
#endif
    return CAL_OK;
}
