/*
  For IBUS.c, we use CMock to mock CAL_UART (which handles DMA reception) and CAL_timer (which provides the tick count). 

In IBUS.c, ibus_uart, new_ibus_data_received, is_fail_safe_enabled, and channel_values[] are all declared as static (file-scope) variables.
This is good design because it hides the module's internal state and maintains encapsulation. However, it creates a challenge for unit testing: each test case cannot start with a completely fresh state.
All test cases run within the same translation unit (the same .c file compiled into a single test binary), so these static variables persist from one test to the next. As a result, the internal state established by one test may affect subsequent tests unless it is explicitly reset.


Therefore, these test cases must be executed in a specific order (from top to bottom). Ceedling/Unity follows the order in which the test functions are defined in the test file.
For a cleaner approach, a test-support function such as ibusReset() could be added in the production code to reset the module's internal state before each test. This improvement has been suggested and documented in the README.

 */

#include "unity.h"
#include "IBUS.h"

/* IBUS.c internally calls log_info(...) from Modules/Utilities/Logger/Logger.h.
Ceedling automatically compiles a real .c file only if its corresponding header is explicitly included in the current test file; it does not inspect the #include directives inside IBUS.c itself.
Therefore, we explicitly include the real Logger.h here. We do not want to mock it—see the notes in test_Logger.c for the reasoning. Keeping the real implementation and relying on log_uart == NULL is the simplest and safest approach */

#include "Logger.h"
#include "mock_CAL_UART.h"
#include "mock_CAL_timer.h"

/* CAL.h also unconditionally #includes CAL_GPIO.h and CAL_PWM.h (IBUS.c
 * never calls them, but they'd otherwise pull in real CAL_GPIO.c/
 * CAL_PWM.c, which need real STM32 HAL functions we don't provide). */

#include "mock_CAL_GPIO.h"
#include "mock_CAL_PWM.h"

/* Real addressable object -- see test_PMDC.c comment for why this
 * can't be an arbitrary integer-cast pointer (CMock dereferences
 * pointer args for its deep comparison). */

static UART_HandleTypeDef fake_uart_storage;
static UART_HandleTypeDef *fake_uart = &fake_uart_storage;

void setUp(void) {}
void tearDown(void) {}

/*  1. ibusInitialize -- must be first, sets up static ibus_uart  */

void test_1_ibusInitialize_arms_DMA_reception(void)
{
    /* ibusInitialize() calls CAL_UART_Receive_DMA(huart, ibus_rx_buffer, 0x20)
     * The buffer address is internal to IBUS.c, so we ignore that arg. */
    CAL_UART_Receive_DMA_ExpectAndReturn(fake_uart, NULL, 0x20, CAL_OK);
    CAL_UART_Receive_DMA_IgnoreArg_pData();

    int8_t result = ibusInitialize(fake_uart);

    TEST_ASSERT_EQUAL_INT8(0, result);
}

/*  2. ibusUpdateData -- no packet received yet -> failsafe engaged */

void test_2_ibusUpdateData_reports_failsafe_when_no_packet_received_yet(void)
{
    ibus_packet_t packet;

    /* new_ibus_data_received is still false at this point (only
     * ibusInitialize ran so far), so ibusParseData() takes the
     * "check for timeout" branch, which reads the tick counter once. */
    
    CAL_GetTimerTicks_ExpectAndReturn(0);

    int8_t result = ibusUpdateData(&packet);

    TEST_ASSERT_EQUAL_INT8(0, result);
    
    /* channel_values[] ellame 0 -- CHANNEL_MIN_VALUE (1000) kku keezha,
     * so failsafe ah force pannanum */
    
    TEST_ASSERT_TRUE(packet.failsafe_engaged);
    TEST_ASSERT_EQUAL_UINT16(0, packet.channel_value[0]);
}

/*  3. ibusCallback -- flags new data + re-arms DMA for next packet */

void test_3_ibusCallback_records_timestamp_and_rearms_DMA(void)
{
    CAL_GetTimerTicks_ExpectAndReturn(500);
    CAL_UART_Receive_DMA_ExpectAndReturn(fake_uart, NULL, 0x20, CAL_OK);
    CAL_UART_Receive_DMA_IgnoreArg_pData();

    int8_t result = ibusCallback();

    TEST_ASSERT_EQUAL_INT8(0, result);
}
