/**
 * @file    CAL_timer.h
 * @brief   Timer Abstraction Layer
 *
 * @details
 * Provides timer utility functions used across the CAL (Controller
 * Abstraction Layer). This module wraps HAL timer functions so that
 * higher layers remain hardware independent.
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

#ifndef CAL_TIMER_CAL_TIMER_H_
#define CAL_TIMER_CAL_TIMER_H_

#include "CAL/CAL.h"

/**
 * @brief Get system timer tick value
 *
 * @details
 * Returns the current system tick in milliseconds.
 * This function is a wrapper for HAL_GetTick().
 *
 * @return uint32_t Current system tick in milliseconds
 */
uint32_t CAL_GetTimerTicks(void);

/**
 * @brief Delay execution for a specified time
 *
 * @param milliSeconds Delay duration in milliseconds
 */
CAL_status CAL_Delay(uint32_t milliSeconds);

#endif /* CAL_TIMER_CAL_TIMER_H_ */
