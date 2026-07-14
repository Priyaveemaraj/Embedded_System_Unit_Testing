/**
 * @file    CAL_GPIO.c
 * @brief   GPIO Abstraction Layer implementation using STM32 HAL.
 *
 * @details
 * This file provides wrapper functions for basic GPIO operations
 * such as write, read, and toggle. These functions abstract the
 * STM32 HAL GPIO driver to provide a consistent interface for
 * higher-level modules.
 *
 * The module is part of the Complex Abstraction Layer (CAL) and
 * is intended to simplify GPIO access for application and driver
 * layers.
 *
 * Supported operations:
 *  - Writing logic values to GPIO pins
 *  - Reading GPIO pin states
 *  - Toggling GPIO pin outputs
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
 * Date          Author        Version     Description
 * ---------------------------------------------------------------------------
 * 07-03-2026    Bojith akash  1.0.0       Initial implementation
 * ---------------------------------------------------------------------------
 */

#include "CAL_GPIO.h"


CAL_status CAL_GPIO_Write(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState){
	HAL_GPIO_WritePin(GPIOx, GPIO_Pin, PinState);
	return CAL_OK;
}

GPIO_PinState CAL_GPIO_Read(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin){
	return HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);
}

CAL_status CAL_GPIO_Toggle(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin){
	HAL_GPIO_TogglePin(GPIOx, GPIO_Pin);
	return CAL_OK;
}
