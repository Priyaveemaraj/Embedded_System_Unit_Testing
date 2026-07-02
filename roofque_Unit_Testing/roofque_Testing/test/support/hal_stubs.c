/**
 * @file hal_stubs.c
 * @brief TEST-ONLY support code. NOT part of the real firmware build.
 * See hal_stubs.h for the rationale.
 */
#include "hal_stubs.h"

/* Storage for the peripheral handles controller.c references via
 * `extern`. Never dereferenced (all real HAL access happens through
 * CAL_* functions, which are always mocked in these tests) -- they
 * only need to exist as valid, distinct addresses. */
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
