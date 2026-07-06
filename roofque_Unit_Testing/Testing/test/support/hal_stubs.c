/* @file hal_stubs.c
  @brief Test-only support code. This file is NOT part of the actual
  firmware build. See hal_stubs.h for the rationale and implementation
  details.
 */

#include "hal_stubs.h"

/* Storage for the peripheral handles that controller.c references through `extern` declarations.
 
  These handles are never dereferenced in the tests, since all actual
  hardware access is performed through CAL_* functions, which are
  always mocked.
 
  They simply need to exist as valid, distinct objects so that their
  addresses can be safely referenced.
 */

UART_HandleTypeDef huart1;
TIM_HandleTypeDef  htim1;
TIM_HandleTypeDef  htim3;

jmp_buf hal_stub_loop_escape;

static int s_delay_calls_remaining = -1;   /* -1 = escape hatch disabled */

void hal_stub_arm_loop_escape(int iterations)
{
    s_delay_calls_remaining = iterations;
}

void HAL_Delay(uint32_t Delay)
{
    (void)Delay;

    if (s_delay_calls_remaining > 0)
    {
        s_delay_calls_remaining--;
        if (s_delay_calls_remaining == 0)
        {
            longjmp(hal_stub_loop_escape, 1);
        }
    }
}
