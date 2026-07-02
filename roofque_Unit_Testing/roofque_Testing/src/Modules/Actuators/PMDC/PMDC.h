
#ifndef __PMDC_HDR
#define  __PMDC_HDR


#ifdef __cplusplus
 extern "C" {
#endif

#include "CAL/CAL.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
/*#include "logging.h"*/


typedef enum{
	GPIO_mode,
	PWM_mode
} PMDC_Mode;


typedef struct{
			PMDC_Mode mode;
			TIM_HandleTypeDef* Ftimer;
			uint8_t  Fchannel;
			TIM_HandleTypeDef* Rtimer;
			uint8_t  Rchannel;
			GPIO_TypeDef *Fwd_Port;
			uint16_t Fwd_Pin;
			GPIO_TypeDef *Rev_Port;
			uint16_t Rev_Pin;
			GPIO_TypeDef *EN_Port;
			uint16_t EN_Pin;
			uint8_t  dutyCycle;
			int16_t currentRPM;
			uint16_t maxRPM;
}PMDC;


bool PMDC_init(PMDC* me);
bool PMDC_setDutyCycle(PMDC* me, int8_t dutyCycle);
bool PMDC_rotateFwd(PMDC* me);
bool PMDC_rotateRev(PMDC* me);
bool PMDC_Stop(PMDC* me);
bool PMDC_Destroy(PMDC* me);


#endif
#ifdef __cplusplus
 }

#endif
