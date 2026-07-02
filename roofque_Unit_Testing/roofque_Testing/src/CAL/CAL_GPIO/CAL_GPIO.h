/**
 * @file    CAL_GPIO.h
 * @brief   GPIO Abstraction Layer Interface using STM32 HAL.
 *
 * @details
 * This header file defines the interface for the GPIO module
 * in the Complex Abstraction Layer (CAL). It provides function
 * prototypes for performing common GPIO operations such as:
 *
 *  - Writing a logic value to a GPIO pin
 *  - Reading the state of a GPIO pin
 *  - Toggling the state of a GPIO pin
 *
 * These functions act as wrappers around the STM32 HAL GPIO
 * driver to provide a simplified and consistent interface
 * for higher-level application modules.
 *
 * The CAL layer helps decouple application code from direct
 * hardware access and improves portability and maintainability.
 *
 * ---------------------------------------------------------------------------
 * Author           : Bojith akash
 * Created Date     : 07-03-2026
 *
 * Last Modified By : Bojith akash
 * Last Modified On : 07-03-2026
 *
 * Version          : 1.0.0
 *
 * Remarks / Change Log:
 * ---------------------------------------------------------------------------
 * Date         Author        Version     Description
 * ---------------------------------------------------------------------------
 * 07-03-2026   Bojith akash  1.0.0       Initial implementation
 * ---------------------------------------------------------------------------
 */

#ifndef CAL_GPIO_H_
#define CAL_GPIO_H_

#include "CAL/CAL.h"
#include  "gpio.h"

/**
 * @brief Write a value to a GPIO pin.
 *
 * This function writes a logic level (HIGH or LOW) to a specified GPIO pin
 * using the STM32 HAL GPIO driver.
 *
 * @param GPIOx Pointer to the GPIO port (e.g., GPIOA, GPIOB).
 * @param GPIO_Pin Specifies the GPIO pin number (e.g., GPIO_PIN_5).
 * @param PinState Desired pin state:
 *        - GPIO_PIN_SET   : Set pin HIGH
 *        - GPIO_PIN_RESET : Set pin LOW
 *
 * @return HAL_StatusTypeDef
 *         - CAL_OK if operation is successful.
 */
CAL_status CAL_GPIO_Write(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);


/**
 * @brief Read the current state of a GPIO pin.
 *
 * This function reads the logic level present on the specified GPIO pin.
 *
 * @param GPIOx Pointer to the GPIO port (e.g., GPIOA, GPIOC).
 * @param GPIO_Pin Specifies the GPIO pin number to read.
 *
 * @return GPIO_PinState
 *         - GPIO_PIN_SET   : Pin is HIGH
 *         - GPIO_PIN_RESET : Pin is LOW
 */
GPIO_PinState CAL_GPIO_Read(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);


/**
 * @brief Toggle the state of a GPIO pin.
 *
 * This function switches the current state of the pin:
 * HIGH becomes LOW and LOW becomes HIGH.
 *
 * @param GPIOx Pointer to the GPIO port (e.g., GPIOA).
 * @param GPIO_Pin Specifies the GPIO pin number to toggle.
 *
 * @return HAL_StatusTypeDef
 *         - CAL_OK if toggle operation succeeds.
 */
CAL_status CAL_GPIO_Toggle(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);


#endif /* CAL_GPIO_H_ */
