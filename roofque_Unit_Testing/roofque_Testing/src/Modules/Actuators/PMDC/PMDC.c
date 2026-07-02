/*
 * PMDC.c
 *
 *  Created on: Jul 19, 2025
 *      Author: vividobots
 */

#include "PMDC.h"


bool PMDC_init(PMDC* me){
	if (me!= NULL){
			me->currentRPM=0;
			me->dutyCycle=0;

			if(me->mode == PWM_mode){
				CAL_PWM_Start(me->Ftimer, me->Fchannel);
				CAL_PWM_Start(me->Rtimer, me->Rchannel);

			}


	} else {
		return NULL;
	}
	return me;

}


static bool PMDC_setEnable(PMDC* me){
    if (!me) return false;
	CAL_GPIO_Write(me->EN_Port,me->EN_Pin,SET);
	return true;
}

static bool PMDC_resetEnable(PMDC* me){
    if (!me) return false;
	CAL_GPIO_Write(me->EN_Port,me->EN_Pin,RESET);
	return true;
}



static bool PMDC_PWM_rotateForward(PMDC* me, uint8_t dutyCycle){
    if (!me) return false;
	dutyCycle = (dutyCycle < 0) ? 0 : (dutyCycle > 100) ? 100 : dutyCycle;
    me->dutyCycle=dutyCycle;
    me->currentRPM=(int16_t) ((me->dutyCycle) * (me->maxRPM)) / (100);
    PMDC_setEnable(me);
    CAL_PWM_SetDutyCycle(me->Rtimer,me->Rchannel,0);
    CAL_PWM_SetDutyCycle(me->Ftimer,me->Fchannel,dutyCycle);
    return true;
}


static bool PMDC_PWM_rotateReverse(PMDC* me, uint8_t dutyCycle){
    if (!me) return false;
	dutyCycle = (dutyCycle < 0) ? 0 : (dutyCycle > 100) ? 100 : dutyCycle;
    me->dutyCycle=dutyCycle;
    me->currentRPM=(int16_t) ((me->dutyCycle) * (me->maxRPM)) / (100) *-1;
    PMDC_setEnable(me);
    CAL_PWM_SetDutyCycle(me->Ftimer,me->Fchannel,0);
    CAL_PWM_SetDutyCycle(me->Rtimer,me->Rchannel,dutyCycle);
    return true;
}


bool PMDC_rotateFwd(PMDC* me){
	if(me!=NULL && me->mode == GPIO_mode){
		PMDC_setEnable(me);
		CAL_GPIO_Write(me->Fwd_Port, me->Fwd_Pin, SET);
		CAL_GPIO_Write(me->Rev_Port, me->Rev_Pin, RESET);

		return true;
	}
	return false;
}

bool PMDC_rotateRev(PMDC* me){
	if(me!=NULL && me->mode == GPIO_mode){
		PMDC_setEnable(me);
		CAL_GPIO_Write(me->Fwd_Port, me->Fwd_Pin, RESET);
		CAL_GPIO_Write(me->Rev_Port, me->Rev_Pin, SET);
		return true;
	}
	return false;
}


bool PMDC_setDutyCycle(PMDC* me, int8_t dutyCycle){
		if(me != NULL && me->mode == PWM_mode){
			if(dutyCycle>0){
				PMDC_PWM_rotateForward(me, dutyCycle);

			} else if( dutyCycle < 0){

				PMDC_PWM_rotateReverse(me, dutyCycle*-1);

			} else{
				PMDC_Stop(me);
			}
			return true;
		}
		return false;

}

bool PMDC_Stop(PMDC* me){
	if (me!=NULL){
		PMDC_resetEnable(me);
		if(me->mode == PWM_mode){
		    CAL_PWM_SetDutyCycle(me->Ftimer,me->Fchannel,0);
		    CAL_PWM_SetDutyCycle(me->Rtimer,me->Rchannel,0);
		} else {
			CAL_GPIO_Write(me->Fwd_Port, me->Fwd_Pin, RESET);
			CAL_GPIO_Write(me->Rev_Port, me->Rev_Pin, RESET);
		}

	    return true;
	}
	return false;

}

bool PMDC_Destroy(PMDC* me){
    if (!me) return false;
    PMDC_Stop(me);
	if(me->mode == PWM_mode){
		CAL_PWM_Stop(me->Ftimer, me->Fchannel);
		CAL_PWM_Stop(me->Rtimer, me->Rchannel);
	}
    return true;

}
