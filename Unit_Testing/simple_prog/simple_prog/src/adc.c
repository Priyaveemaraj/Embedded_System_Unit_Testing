#include "adc.h"

/* Assume this value is updated elsewhere (e.g. an ISR reading real hardware) */
volatile int adc_value = 0;

int adc_read( void )
{
    return adc_value;
}
