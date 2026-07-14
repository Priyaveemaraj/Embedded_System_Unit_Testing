/**
  @file stm32g0xx_hal.h
  @brief Test-only stub of the STM32 HAL header.
 
  This is NOT the actual ST HAL implementation. It exists solely to
  allow CAL.h and its peripheral wrapper headers to compile on a PC
  (using Ceedling/GCC) for unit testing.
 
  Only the type names that are actually referenced by the CAL_*
  headers are declared here, preferably as opaque or incomplete types.
  The real HAL functions (such as HAL_GPIO_WritePin(),
  HAL_TIM_PWM_Start(), etc.) are never called directly in these tests;
  instead, the CAL_* wrapper functions that invoke them are mocked.
 
  This file is located in test/support so that it is used only during
 `ceedling test:*` builds and is never included in a real firmware
 build.
 */

#ifndef STM32G0XX_HAL_H
#define STM32G0XX_HAL_H

#include <stdint.h>

/* ---- Generic HAL status / flag enums -------------------------------- */
typedef enum { HAL_OK = 0, HAL_ERROR = 1, HAL_BUSY = 2, HAL_TIMEOUT = 3 } HAL_StatusTypeDef;
typedef enum { RESET = 0, SET = !RESET } FlagStatus, ITStatus, ErrorStatus;
typedef enum { GPIO_PIN_RESET = 0, GPIO_PIN_SET } GPIO_PinState;

#define HAL_MAX_DELAY 0xFFFFFFFFU

/* ---- Peripheral handle types --------------------------------------
  CMock-generated mocks perform byte-for-byte comparisons (using
  sizeof()) on pointer arguments to support ExpectWithArray-style
 deep comparisons.

  Therefore, these types must be COMPLETE definitions rather than
  simple forward declarations or opaque types, even though the tests
  never read from or write to their members.
 
  A single dummy field is sufficient to satisfy this requirement.
 */
typedef struct { uint32_t _unused; } GPIO_TypeDef;
typedef struct { uint32_t _unused; } TIM_HandleTypeDef;
typedef struct { uint32_t _unused; } UART_HandleTypeDef;
typedef struct { uint32_t _unused; } I2C_HandleTypeDef;
typedef struct { uint32_t _unused; } CAN_HandleTypeDef;
typedef struct { uint32_t _unused; } DMA_HandleTypeDef;

typedef struct { uint32_t _unused; } CAN_FilterTypeDef;
typedef struct { uint32_t _unused; } CAN_TxHeaderTypeDef;
typedef struct { uint32_t _unused; } CAN_RxHeaderTypeDef;


/* ---- HAL_Delay ---------------------------------------------------------
  controller.c calls HAL_Delay(50) directly at the end of its main
  loop; it does not use the CAL_timer wrapper.
 
  Since all CAL_* modules are mocked in the tests (meaning that the
  real CAL_GPIO.c, CAL_PWM.c, CAL_UART.c, and CAL_timer.c files are
  never compiled against this stub header), HAL_Delay() is the only
  actual HAL symbol for which the test environment still requires
  both a function prototype and a linkable implementation.
 
  See test/support/hal_stubs.c for the implementation. It also serves
  as a controlled "escape hatch" that allows tests to exit the
  infinite loop in controller.c safely.
 */
void HAL_Delay(uint32_t Delay);

#endif /* STM32G0XX_HAL_H */
