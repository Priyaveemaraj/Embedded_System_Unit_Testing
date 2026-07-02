/**
 * @file CAL.h
 * @brief Top-level Common Abstraction Layer (CAL) header for STM32.
 *
 * This header serves as the central entry point for the CAL library. It
 * handles MCU-specific HAL inclusions based on build definitions and
 * provides common status types used across all peripheral abstractions.
 * * The layer currently supports:
 * - STM32G070xx
 * - STM32F446xx
 *
 * ---------------------------------------------------------------------------
 * Author           : aadhi
 * Created Date     : 06-03-2026
 *
 * Last Modified By : Harish karthick R
 * Last Modified On : 16-03-2026
 *
 * Version          : 1.0.1
 *
 * Remarks / Change Log:
 * ---------------------------------------------------------------------------
 * Date         Author               Version     Description
 * ---------------------------------------------------------------------------
 * 06-03-2026   aadhi                1.0.0       Initial implementation
 * 16-03-2026   Harish karthick      1.0.1       Added C++ guards,Doxygen
 * 												 headers, stm32f446xx support
 * 												 and CAL_CAN support
 *
 * ---------------------------------------------------------------------------
 */

#ifndef CAL_H_
#define CAL_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

/* MCU-Specific HAL Selection ------------------------------------------------*/
#if defined(STM32G070xx)
  #include "stm32g0xx_hal.h"
#elif defined (STM32F446xx)
  #include "stm32f4xx_hal.h"
#else
  #error "Unsupported STM32 device. Ensure target is defined in symbols."
#endif

/* Exported Types ------------------------------------------------------------*/

/**
 * @brief CAL Status structures used for function return values.
 */

typedef enum {
  CAL_OK,
  CAL_ERROR
 } CAL_status;


//#define USE_CAL_CAN
#define USE_CAL_PWM
//#define CAL_USE_RTOS
//#define USE_CAL_I2C


/* Peripheral Driver Includes ------------------------------------------------*/
#include "CAL_GPIO/CAL_GPIO.h"
#include "CAL_i2c/CAL_i2c.h"
#include "CAL_PWM/CAL_PWM.h"
#include "CAL_UART/CAL_UART.h"
#include "CAL_timer/CAL_timer.h"
#include "CAL_CAN/CAL_CAN.h"





#ifdef __cplusplus
}
#endif

#endif /* CAL_H_ */

