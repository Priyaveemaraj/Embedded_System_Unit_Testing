/**
 * test_IBUS.c
 *
 * IBUS.c ku CAL_UART (DMA reception) matrum CAL_timer (tick count) ah
 * CMock use panni mock pannirukom.
 *
 * MUKKIYAMANA KURIPPU (Important note on test order):
 * IBUS.c la `ibus_uart`, `new_ibus_data_received`, `is_fail_safe_enabled`,
 * `channel_values[]` ellame *static* (file-scope) variables. Idhu andha
 * module oda internal state ah "hide" panna nallа design dhaan, aana
 * unit testing ku, ovvoru test case-um oru "fresh" state la start aaga
 * mudiyadhu -- ella test case-um same translation unit (same .c file
 * compile aana adhே binary) la run aaguthu, so static vars test-to-test
 * persist aagum.
 *
 * Adhanala indha test cases *specific order* la (mela irundhu kீழе)
 * run aaganum -- Ceedling/Unity, test file la irukka function order
 * follow pannும். Real production ku, oru `ibusReset()` madhiri
 * test-support function add pannuna, ithu innum clean ah irukkum
 * (suggestion -- README la eludhi irukken).
 */

#include "unity.h"
#include "IBUS.h"
/* IBUS.c calls log_info(...) internally (Modules/Utilities/Logger/Logger.h).
 * Ceedling only auto-compiles a real .c file for a header if that header
 * is explicitly mentioned in THIS test file -- it doesn't look inside
 * IBUS.c's own #includes. So we mention the REAL Logger.h here (we don't
 * want to mock it -- see test_Logger.c notes on why leaving it real and
 * relying on log_uart==NULL is the simplest, safest approach). */
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

/* ------------------------------------------------------------------ */
/* 1. ibusInitialize -- must be first, sets up static ibus_uart        */
/* ------------------------------------------------------------------ */

void test_1_ibusInitialize_arms_DMA_reception(void)
{
    /* ibusInitialize() calls CAL_UART_Receive_DMA(huart, ibus_rx_buffer, 0x20)
     * The buffer address is internal to IBUS.c, so we ignore that arg. */
    CAL_UART_Receive_DMA_ExpectAndReturn(fake_uart, NULL, 0x20, CAL_OK);
    CAL_UART_Receive_DMA_IgnoreArg_pData();

    int8_t result = ibusInitialize(fake_uart);

    TEST_ASSERT_EQUAL_INT8(0, result);
}

/* ------------------------------------------------------------------ */
/* 2. ibusUpdateData -- no packet received yet -> failsafe engaged     */
/* ------------------------------------------------------------------ */

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

/* ------------------------------------------------------------------ */
/* 3. ibusCallback -- flags new data + re-arms DMA for next packet     */
/* ------------------------------------------------------------------ */

void test_3_ibusCallback_records_timestamp_and_rearms_DMA(void)
{
    CAL_GetTimerTicks_ExpectAndReturn(500);
    CAL_UART_Receive_DMA_ExpectAndReturn(fake_uart, NULL, 0x20, CAL_OK);
    CAL_UART_Receive_DMA_IgnoreArg_pData();

    int8_t result = ibusCallback();

    TEST_ASSERT_EQUAL_INT8(0, result);
}
