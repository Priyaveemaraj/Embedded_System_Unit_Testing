/**
 * @file CAL_PWM.h
 * @brief PWM abstraction layer built on top of STM32 HAL timer PWM functions.
 *
 * This module provides a simple wrapper interface for controlling PWM
 * outputs using STM32 timers. It abstracts the underlying HAL timer
 * functions and allows the user to:
 *
 * - Start PWM generation on a specified timer channel
 * - Stop PWM generation
 * - Adjust PWM duty cycle
 * - Set PWM pulse width directly
 * - Configure PWM output frequency
 *
 * The driver operates on STM32 HAL timer handles and supports
 * channels 1 through 4 of a timer peripheral.
 *
 * This abstraction simplifies PWM control in higher-level
 * application modules without directly interacting with HAL APIs.
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
 * 10-03-2026	Aadhithya	  1.0.1		  Added CAL_status return type
 * ---------------------------------------------------------------------------
 */

#ifndef CAL_PWM_CAL_PWM_H_
#define CAL_PWM_CAL_PWM_H_

#include "CAL/CAL.h"


#ifdef USE_CAL_PWM

extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim2;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;

/**
 * @brief Start PWM generation on a specified timer channel.
 *
 * This function enables PWM output for the selected timer channel
 * using the HAL PWM start function.
 *
 * @param timer Pointer to the timer handle used for PWM generation.
 * @param channel Timer channel number (1-4).
 *
 * @return CAL_status
 *         -  CAL_OK if the transmission is successful
 *         -  CAL_ERROR or other HAL status codes if the operation fails
 */
CAL_status CAL_PWM_Start(TIM_HandleTypeDef* timer, uint8_t channel);

/**
 * @brief Stop PWM generation on a specified timer channel.
 *
 * This function disables PWM output for the selected timer channel.
 *
 * @param timer Pointer to the timer handle used for PWM generation.
 * @param channel Timer channel number (1-4).
 *
 * @return CAL_status
 *         -  CAL_OK if the transmission is successful
 *         -  CAL_ERROR or other HAL status codes if the operation fails
 */
CAL_status CAL_PWM_Stop(TIM_HandleTypeDef* timer, uint8_t channel);

/**
 * @brief Set the PWM duty cycle for a timer channel.
 *
 * This function calculates the compare register (CCR) value based
 * on the desired duty cycle percentage and the timer period.
 *
 * @param timer Pointer to the timer handle used for PWM generation.
 * @param channel Timer channel number (1-4).
 * @param dutycycle Duty cycle percentage (0-100).
 *
 * @return CAL_status
 *         -  CAL_OK if the transmission is successful
 *         -  CAL_ERROR or other HAL status codes if the operation fails
 */
CAL_status CAL_PWM_SetDutyCycle(TIM_HandleTypeDef* timer, uint8_t channel, uint16_t dutycycle);

/**
 * @brief Set PWM pulse width directly.
 *
 * This function directly writes the provided pulse width value
 * to the corresponding timer capture/compare register (CCR).
 *
 * @param timer Pointer to the timer handle used for PWM generation.
 * @param channel Timer channel number (1-4).
 * @param pulse_width Pulse width value to be written to CCR.
 *
 * @return CAL_status
 *         -  CAL_OK if the transmission is successful
 *         -  CAL_ERROR or other HAL status codes if the operation fails
 */
CAL_status CAL_PWM_SetPulseWidth(TIM_HandleTypeDef* timer, uint8_t channel, uint16_t pulse_width);

/**
 * @brief Configure the PWM output frequency.
 *
 * This function updates the timer auto-reload register (ARR) to
 * achieve the desired PWM frequency based on the timer clock
 * frequency and prescaler value.
 *
 * @param timer Pointer to the timer handle used for PWM generation.
 * @param pwmFrequency Desired PWM frequency in Hertz.
 *
 * @return CAL_status
 *         -  CAL_OK if the transmission is successful
 *         -  CAL_ERROR or other HAL status codes if the operation fails
 */
CAL_status CAL_PWM_setPulseFrequency(TIM_HandleTypeDef* timer, uint16_t pwmFrequency);


#endif
#endif /* CAL_PWM_CAL_PWM_H_ */
