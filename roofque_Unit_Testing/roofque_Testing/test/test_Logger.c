/**
 * test_Logger.c
 *
 * Logger.h la USE_RTOS_LOGGER default ah commented out irukku
 * (bare-metal mode), so idhu andha simple path ah test pannudhu:
 * log_output() -> CAL_GetTimerTicks() (timestamp) + CAL_UART_Transmit()
 * (blocking send). Rendaiyum mock pannirukom.
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
    /* DEBUG threshold la irukka pothu, INFO level message pass aagi
     * transmit aaganum */
    CAL_GetTimerTicks_ExpectAndReturn(12532);
    /* Message content dynamic ah snprintf aaguthu (buffer + length ku
     * exact value predict panna mudiyadhu), so Ignore variant use
     * pannurom -- PPT-4 la paatha "Ignore, enabled by default" feature */
    CAL_UART_Transmit_IgnoreAndReturn(CAL_OK);

    log_info("System initialized");
}

void test_log_below_threshold_is_suppressed(void)
{
    log_set_level(LOG_LEVEL_ERROR);

    /* Idha edhavadhu CAL_GetTimerTicks / CAL_UART_Transmit call pannaadhu.
     * CMock ku edhavadhu Expect set pannalainu, andha function
     * unexpected ah call aana automatic ah test fail aagum -- so idhu
     * thaan andha "suppressed" behaviour ah verify pannradhu. */
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
    /* log_uart == NULL scenario -- init panna mudinjadhu illa nu
     * simulate pannurom. Idha edhavadhu CAL call pannakoodadhu. */
    /* Note: log_uart is a static in Logger.c set only via log_init(),
     * so we can't directly force it to NULL from here without an
     * extra test hook. This is a good candidate for a future
     * log_deinit()/reset helper -- see README. Left as documentation
     * for now instead of a fragile test. */
    TEST_IGNORE_MESSAGE("Needs a log_deinit()/reset hook in Logger.c to test cleanly");
}
