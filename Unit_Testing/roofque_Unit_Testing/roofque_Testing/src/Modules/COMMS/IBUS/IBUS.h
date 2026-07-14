#ifndef IBUS_LIB__

#define IBUS_LIB__

#include <stdbool.h>
#include <stdint.h>



#include "CAL/CAL.h"



#define IBUS_MAX_CHANNELS 14


typedef struct{
	uint16_t channel_value[IBUS_MAX_CHANNELS];
	bool    failsafe_engaged;
}ibus_packet_t;


int8_t ibusInitialize(UART_HandleTypeDef * huart);

int8_t ibusUpdateData(ibus_packet_t* current_packet);

int8_t ibusCallback(void);


#endif
