/**
 * @file CAL_PWM.c
 * @brief Implementation of the PWM abstraction layer for STM32 timers.
 *
 * This file implements helper functions for controlling PWM outputs
 * using STM32 HAL timer peripherals. The functions provided here
 * simplify PWM management by wrapping HAL APIs and directly
 * manipulating timer registers when required.
 *
 * The implementation supports:
 * - Starting and stopping PWM generation
 * - Adjusting duty cycle
 * - Setting pulse width
 * - Changing PWM frequency
 *
 * These utilities allow higher-level modules to control PWM signals
 * without directly interacting with low-level timer registers or
 * HAL functions.
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

#include "CAL_PWM.h"

#ifdef USE_CAL_PWM

CAL_status CAL_PWM_Start(TIM_HandleTypeDef* timer, uint8_t channel){
	switch(channel){
	case 1: HAL_TIM_PWM_Start(timer, TIM_CHANNEL_1); break;
	case 2: HAL_TIM_PWM_Start(timer, TIM_CHANNEL_2); break;
	case 3: HAL_TIM_PWM_Start(timer, TIM_CHANNEL_3); break;
	case 4: HAL_TIM_PWM_Start(timer, TIM_CHANNEL_4); break;
	default: break;
	}
	return CAL_OK;
}

CAL_status CAL_PWM_Stop(TIM_HandleTypeDef* timer, uint8_t channel){
    switch (channel) {
        case 1: HAL_TIM_PWM_Stop(timer, TIM_CHANNEL_1); break;
        case 2: HAL_TIM_PWM_Stop(timer, TIM_CHANNEL_2); break;
        case 3: HAL_TIM_PWM_Stop(timer, TIM_CHANNEL_3); break;
        case 4: HAL_TIM_PWM_Stop(timer, TIM_CHANNEL_4); break;
        default: break;
    }
    return CAL_OK;
}

CAL_status CAL_PWM_SetDutyCycle(TIM_HandleTypeDef* timer, uint8_t channel, uint16_t dutycycle){
    uint32_t CCR= (dutycycle*(timer->Init.Period))/100;
    switch (channel) {
        case 1: timer->Instance->CCR1 = CCR; break;
        case 2: timer->Instance->CCR2 = CCR; break;
        case 3: timer->Instance->CCR3 = CCR; break;
        case 4: timer->Instance->CCR4 = CCR; break;
        default: break;
    }
    return CAL_OK;
}

CAL_status CAL_PWM_SetPulseWidth(TIM_HandleTypeDef* timer, uint8_t channel, uint16_t pulse_width){
    switch (channel) {
        case 1: timer->Instance->CCR1 = pulse_width; break;
        case 2: timer->Instance->CCR2 = pulse_width; break;
        case 3: timer->Instance->CCR3 = pulse_width; break;
        case 4: timer->Instance->CCR4 = pulse_width; break;
        default: break;
    }
    return CAL_OK;
}

CAL_status CAL_PWM_setPulseFrequency(TIM_HandleTypeDef* timer, uint16_t pwmFrequency){
	int32_t ARR= (int32_t)((HAL_RCC_GetPCLK1Freq())/timer->Instance->PSC/pwmFrequency);
	timer->Instance->ARR=ARR;
	return CAL_OK;
}
#endif
