/**
 * @file stm32g0xx_hal.h
 * @brief TEST-ONLY stub of the STM32 HAL header.
 *
 * This is NOT the real ST HAL. It exists purely so that CAL.h and its
 * peripheral wrapper headers can compile on a PC (via Ceedling/GCC) for
 * unit testing. Only the type names actually referenced inside the CAL_*
 * headers are declared, as opaque/incomplete types wherever possible,
 * since the real HAL functions (HAL_GPIO_WritePin, HAL_TIM_PWM_Start,
 * etc.) are never called directly in these tests -- the CAL_* wrapper
 * functions that call them are mocked instead.
 *
 * This file lives in test/support so it is only picked up during
 * `ceedling test:*` builds, never during a real firmware build.
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
 * CMock's generated mocks do a byte-for-byte comparison (via sizeof())
 * on any pointer argument, to support ExpectWithArray-style deep
 * comparisons -- so these types must be COMPLETE (not just forward-
 * declared/opaque), even though nothing in these tests ever reads or
 * writes their members. A single dummy field is enough.
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
 * controller.c calls HAL_Delay(50) DIRECTLY at the bottom of its main
 * loop -- it does NOT go through the CAL_timer wrapper. Since we mock
 * every CAL_* module in tests (so real CAL_GPIO.c/CAL_PWM.c/CAL_UART.c/
 * CAL_timer.c never get compiled against this fake header), HAL_Delay
 * is the one real HAL symbol a test still needs a prototype + a linkable
 * implementation for. See test/support/hal_stubs.c for the implementation
 * (it doubles as a controlled "escape hatch" out of controller.c's
 * infinite loop during testing).
 */
void HAL_Delay(uint32_t Delay);

#endif /* STM32G0XX_HAL_H */
