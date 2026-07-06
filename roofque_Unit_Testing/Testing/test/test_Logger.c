/*
In Logger.h, USE_RTOS_LOGGER is commented out by default, which means the logger operates in bare-metal mode.
Therefore, this test covers that simpler execution path:
log_output() → CAL_GetTimerTicks() (to obtain the timestamp) → CAL_UART_Transmit() (for blocking transmission).
Both of these dependencies are mocked in the test.
 */

#include "unity.h"
#include "Logger.h"
#include "mock_CAL_UART.h"
#include "mock_CAL_timer.h"

/* Same reasoning as test_IBUS.c -- CAL.h pulls in CAL_GPIO.h/CAL_PWM.h
 * unconditionally too, so we mock them here even though Logger.c never
 * calls them, purely to keep the real (uncompilable-on-PC) versions out
 * of this test build. */

#include "mock_CAL_GPIO.h"
#include "mock_CAL_PWM.h"

/* Real addressable object -- see test_PMDC.c comment for why this
 * can't be an arbitrary integer-cast pointer. */

static UART_HandleTypeDef fake_uart_storage;
static UART_HandleTypeDef *fake_uart = &fake_uart_storage;

void setUp(void)
{
    log_init(fake_uart);
    log_set_level(LOG_LEVEL_DEBUG);
}

void tearDown(void) {}

void test_log_info_at_or_above_threshold_transmits_message(void)
{
	/* When the log level threshold is set to DEBUG, an INFO-level
            message should pass the filter and be transmitted. */

    CAL_GetTimerTicks_ExpectAndReturn(12532);

/* The message content is generated dynamically using snprintf(), so
   the exact buffer contents and length cannot be predicted. Therefore,
   we use the Ignore variant for this expectation, as described in the
   "Ignore, enabled by default" feature from PPT-4.
 */
    CAL_UART_Transmit_IgnoreAndReturn(CAL_OK);

    log_info("System initialized");
}

void test_log_below_threshold_is_suppressed(void)
{
    log_set_level(LOG_LEVEL_ERROR);

/* This should not invoke CAL_GetTimerTicks() or CAL_UART_Transmit().
  Since no CMock expectations are set for these functions, the test
  will automatically fail if either one is called unexpectedly.
  This is how we verify the expected "suppressed" behavior.*/
  
     log_info("This should never reach UART");
}

void test_log_error_always_transmits_even_at_ERROR_threshold(void)
{
    log_set_level(LOG_LEVEL_ERROR);

    CAL_GetTimerTicks_ExpectAndReturn(99000);
    CAL_UART_Transmit_IgnoreAndReturn(CAL_OK);

    log_error("Something failed");
}

void test_log_with_no_UART_bound_does_nothing(void)
{
 
/* Scenario where log_uart == NULL -- we simulate a case where
  initialization has not been performed successfully. In this case, no CAL functions should be called.
 */

/* Note: log_uart is a static variable in Logger.c and can only be
  set through log_init(), so we cannot directly force it to NULL
  from here without adding an extra test hook.
 
  A future improvement would be to provide a log_deinit() or reset
  helper function for testing purposes (see the README). For now,
  this is left as documentation rather than implementing a fragile test.
 */



    TEST_IGNORE_MESSAGE("Needs a log_deinit()/reset hook in Logger.c to test cleanly");
}
