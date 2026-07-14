

#include "controller.h"

#include "Modules/Utilities/Logger/Logger.h"
#include "Modules/COMMS/IBUS/IBUS.h"
#include "Modules/Utilities/Tools/math_tools.h"
#include "Modules/Actuators/PMDC/PMDC.h"

#define CHANNEL_MIN_VALUE 1000
#define CHANNEL_MAX_VALUE 2000

#define CHANNEL_SWITCH_ON_VALUE  CHANNEL_MAX_VALUE
#define CHANNEL_SWITCH_OFF_VALUE CHANNEL_MIN_VALUE

#define LEFT_DRIVE_FWD_TIMER &htim1
#define LEFT_DRIVE_REV_TIMER &htim1
#define RIGHT_DRIVE_FWD_TIMER &htim1
#define RIGHT_DRIVE_REV_TIMER &htim1
#define LEFT_DRIVE_FWD_TIMER_CHANNEL   1
#define LEFT_DRIVE_REV_TIMER_CHANNEL   2
#define RIGHT_DRIVE_FWD_TIMER_CHANNEL  3
#define RIGHT_DRIVE_REV_TIMER_CHANNEL  4

#define BRUSH_FWD_TIMER &htim3
#define BRUSH_REV_TIMER &htim3
#define PUMP_FWD_TIMER &htim3
#define PUMP_REV_TIMER &htim3
#define BRUSH_FWD_TIMER_CHANNEL   1
#define BRUSH_REV_TIMER_CHANNEL   2
#define PUMP_FWD_TIMER_CHANNEL  3
#define PUMP_REV_TIMER_CHANNEL  4



#define IBUS_ANGULAR_CHANNEL 0
#define IBUS_LINEAR_CHANNEL  1
#define IBUS_MACHINE_ON_OFF  4
#define IBUS_BRUSH_ON        5


extern UART_HandleTypeDef huart1;

static void stopALL(void);

static PMDC Left_drive_motor = {
		.mode=PWM_mode,
		.Ftimer=  LEFT_DRIVE_FWD_TIMER,
		.Fchannel=LEFT_DRIVE_FWD_TIMER_CHANNEL,
		.Rtimer=  LEFT_DRIVE_REV_TIMER,
		.Rchannel=LEFT_DRIVE_REV_TIMER_CHANNEL,
		.Fwd_Port=NULL,
		.Fwd_Pin=0,
		.Rev_Port=NULL,
		.Rev_Pin=0,
		.EN_Port= LEFT_DRIVE_ENA_GPIO_Port,
		.EN_Pin=  LEFT_DRIVE_ENA_Pin,

};


static PMDC right_drive_motor = {
		.mode=PWM_mode,
		.Ftimer=  RIGHT_DRIVE_FWD_TIMER,
		.Fchannel=RIGHT_DRIVE_FWD_TIMER_CHANNEL,
		.Rtimer=  RIGHT_DRIVE_REV_TIMER,
		.Rchannel=RIGHT_DRIVE_REV_TIMER_CHANNEL,
		.Fwd_Port=NULL,
		.Fwd_Pin=0,
		.Rev_Port=NULL,
		.Rev_Pin=0,
		.EN_Port= RIGHT_DRIVE_ENA_GPIO_Port,
		.EN_Pin=  RIGHT_DRIVE_ENA_Pin,

};


static PMDC brush_motor = {
		.mode=PWM_mode,
		.Ftimer=  BRUSH_FWD_TIMER,
		.Fchannel=BRUSH_FWD_TIMER_CHANNEL,
		.Rtimer=  BRUSH_REV_TIMER,
		.Rchannel=BRUSH_REV_TIMER_CHANNEL,
		.Fwd_Port=NULL,
		.Fwd_Pin=0,
		.Rev_Port=NULL,
		.Rev_Pin=0,
		.EN_Port= BRUSH_ENA_GPIO_Port,
		.EN_Pin=  BRUSH_ENA_Pin,

};


static PMDC pump_motor = {
		.mode=PWM_mode,
		.Ftimer=  PUMP_FWD_TIMER,
		.Fchannel=PUMP_FWD_TIMER_CHANNEL,
		.Rtimer=  PUMP_REV_TIMER,
		.Rchannel=PUMP_REV_TIMER_CHANNEL,
		.Fwd_Port=NULL,
		.Fwd_Pin=0,
		.Rev_Port=NULL,
		.Rev_Pin=0,
		.EN_Port= PUMP_ENA_GPIO_Port,
		.EN_Pin=  PUMP_ENA_Pin,

};


void initializeAndRunController()
{


	  log_info("##### Initializing controller ####");


	  ibus_packet_t current_packet;
	  ibusInitialize(&huart1);

	  PMDC_init(&Left_drive_motor);
	  PMDC_init(&right_drive_motor);
	  PMDC_init(&pump_motor);
	  PMDC_init(&brush_motor);

	  for(;;)
	  {

		  ibusUpdateData(&current_packet);


		  if(current_packet.failsafe_engaged)
		  {
			  stopALL();
			  log_error("FAIL SAFE ENGAGED");

		  } else {

		      if(CHANNEL_SWITCH_ON_VALUE == current_packet.channel_value[IBUS_MACHINE_ON_OFF])
		      {

		    	  	  int8_t linear_velocity_in_percentage=map(current_packet.channel_value[IBUS_LINEAR_CHANNEL], CHANNEL_MIN_VALUE, CHANNEL_MAX_VALUE, -100, 100);
		    	  	  int8_t angular_velocity_in_percentage=map(current_packet.channel_value[IBUS_ANGULAR_CHANNEL], CHANNEL_MIN_VALUE, CHANNEL_MAX_VALUE, -100, 100);
//		    	  	  log_info("linear  : %d",linear_velocity_in_percentage);
//		    	   	  log_info("angular : %d",angular_velocity_in_percentage);

					// 1. Calculate ideal mix (Standard Differential Drive Kinematics)
					int mixed_left  = linear_velocity_in_percentage + angular_velocity_in_percentage;
					int mixed_right = linear_velocity_in_percentage - angular_velocity_in_percentage;

					// 2. Clamp the values to stay within the bounds of your motor controller (-100% to 100%)
					if (mixed_left > 100)   mixed_left = 100;
					if (mixed_left < -100)  mixed_left = -100;

					if (mixed_right > 100)  mixed_right = 100;
					if (mixed_right < -100) mixed_right = -100;

					// 3. Safe cast to your final 8-bit variables
					int8_t left_drive_percentage  = (int8_t)mixed_left;
					int8_t right_drive_percentage = (int8_t)mixed_right;

		    	   	  log_info("left_drive : %d",left_drive_percentage);
		    	   	  log_info("right_drive : %d",right_drive_percentage);
	    	   		  PMDC_setDutyCycle(&Left_drive_motor, left_drive_percentage);
	    	   		  PMDC_setDutyCycle(&right_drive_motor, right_drive_percentage);


		    	   	  if(CHANNEL_SWITCH_ON_VALUE == current_packet.channel_value[IBUS_BRUSH_ON]){
		    	   		  PMDC_setDutyCycle(&brush_motor, 50);
		    	   		  PMDC_setDutyCycle(&pump_motor, 100);
		    	   	  } else {
		    	   		  PMDC_setDutyCycle(&brush_motor, 0);
		    	   		  PMDC_setDutyCycle(&pump_motor,  0);
		    	   	  }


		      } else
		      {
		    	  stopALL();
		      }
		  }

		  HAL_Delay(50);
	  }
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart == &huart1)
    {
    	ibusCallback();
    }
}


static void stopALL(void){
	  PMDC_Stop(&Left_drive_motor);
	  PMDC_Stop(&right_drive_motor);
	  PMDC_Stop(&pump_motor);
	  PMDC_Stop(&brush_motor);

}
