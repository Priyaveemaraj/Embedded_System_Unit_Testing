#include "IBUS.h"
#include "Modules/Utilities/Logger/Logger.h"
#include <stddef.h>

#define IBUS_PACKET_LENGTH 0x20
#define IBUS_START_BYTE    0x20
#define IBUS_TIMEOUT   500
#define IBUS_USER_CHANNELS  6



typedef enum {
    STATE_LOOKING_FOR_START,
    STATE_COLLECTING_DATA
} IBUS_State_t;



static int8_t ibusParseData(void);

static uint8_t ibusPopulateChannels();

static bool ibusCheckCRC(void);

static uint8_t ibusResetChannelValues(void);



static uint8_t ibus_rx_buffer[IBUS_PACKET_LENGTH]={0};

static uint8_t temp_buffer[IBUS_PACKET_LENGTH]= {0};

static uint8_t bytes_received=0;


static UART_HandleTypeDef* ibus_uart=NULL;

static bool new_ibus_data_received =false;

static uint32_t last_ibus_data_received_time=0;

static  IBUS_State_t current_parse_state= STATE_LOOKING_FOR_START;

static bool is_fail_safe_enabled = true;

static uint16_t channel_values[IBUS_MAX_CHANNELS]={0};


int8_t ibusInitialize(UART_HandleTypeDef * huart)
{
	log_info("########## Initializing IBUS #################");
	ibus_uart=huart;
	CAL_UART_Receive_DMA(ibus_uart,ibus_rx_buffer , IBUS_PACKET_LENGTH);
	return 0;
}

int8_t ibusUpdateData(ibus_packet_t* current_packet)
{
	ibusParseData();

	for(int i=0; i< IBUS_USER_CHANNELS ; i++){
		current_packet->channel_value[i]=channel_values[i];
		if(channel_values[i] < 1000 || channel_values[i] > 2000 ){
					is_fail_safe_enabled=true;
		}
	}

	current_packet->failsafe_engaged=is_fail_safe_enabled;


	return 0;
}


int8_t ibusCallback(void)
{
	new_ibus_data_received=true;
	last_ibus_data_received_time=CAL_GetTimerTicks();
	CAL_UART_Receive_DMA(ibus_uart,ibus_rx_buffer , IBUS_PACKET_LENGTH);
	return 0;
}


static int8_t ibusParseData(void)
{

	if(new_ibus_data_received)
	{

		 for (int i = 0; i < IBUS_PACKET_LENGTH; i++)
		 {
		                uint8_t byte = ibus_rx_buffer[i];

		                switch (current_parse_state)
		                {
		                    case STATE_LOOKING_FOR_START:
		                        if (IBUS_START_BYTE == byte)
		                        {
		                            temp_buffer[0] = byte;
		                            bytes_received = 1;
		                            current_parse_state = STATE_COLLECTING_DATA;
		                        }
		                        break;

		                    case STATE_COLLECTING_DATA:
		                        temp_buffer[bytes_received++] = byte;
		                        if (bytes_received >= IBUS_PACKET_LENGTH)
		                        {
		                        	current_parse_state = STATE_LOOKING_FOR_START;
		                            bytes_received = 0;
		                            is_fail_safe_enabled=false;
		                            ibusPopulateChannels();
		                        }
		                        break;
		                    default:
		                    	break;

		                }
		 }

		 new_ibus_data_received=false;
	}
	else
	{
		if( CAL_GetTimerTicks() - last_ibus_data_received_time > IBUS_TIMEOUT)
		{
			is_fail_safe_enabled=true;
			ibusResetChannelValues();

		}
	}
	return 0;
}



static uint8_t ibusResetChannelValues(void)
{
		for(int i=0 ; i< IBUS_MAX_CHANNELS ;i++)
		{
			channel_values[i]=0;
		}
		return 0;
}

static uint8_t ibusPopulateChannels()
{


	if(IBUS_START_BYTE == temp_buffer[0] && 0x40 == temp_buffer[1])
	{

		if(ibusCheckCRC())
		{
			for(int ch_index = 0, bf_index = 2; ch_index < IBUS_USER_CHANNELS; ch_index++, bf_index += 2)
			{
				channel_values[ch_index] = temp_buffer[bf_index + 1] << 8 | temp_buffer[bf_index];

			}
		}
	}
	return 0;
}

static bool ibusCheckCRC(void){

	 	uint16_t checksum_cal = 0xffff;
		uint16_t checksum_ibus;

		for(int i = 0; i < 30; i++)
		{
			checksum_cal -= temp_buffer[i];
		}

		checksum_ibus = temp_buffer[31] << 8 | temp_buffer[30]; // checksum value from ibus
		return (checksum_ibus == checksum_cal);

}


