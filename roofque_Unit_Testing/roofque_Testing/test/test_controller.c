/**
 * test_controller.c
 *
 * controller.c source file ah **edhavum modify pannaame** test
 * pannradhukku, indha test file rendu special technique use pannudhu:
 *
 * 1. INFINITE LOOP ESCAPE HATCH
 *    `initializeAndRunController()` oda `for(;;)` loop, ovvoru
 *    iteration mudivilayum `HAL_Delay(50)` call pannudhu. Namba
 *    `test/support/hal_stubs.c` la `HAL_Delay()` ah intercept panni,
 *    N iterations kழிthu `longjmp()` moolama `setjmp()` panna idathukku
 *    thirumbi varom -- so andha infinite loop ah "N iterations mattum
 *    run pannitu, control ah thirumba test ku kொடு" nu safely
 *    control pannalaam.
 *
 * 2. STATIC MOTOR VARIABLES
 *    controller.c la `Left_drive_motor`, `right_drive_motor` etc ellame
 *    `static` -- test file la irundhu direct ah access panna mudiyadhu.
 *    Adhanala PMDC_* calls ah, exact pointer verify pannama
 *    (`_IgnoreArg_me()`), duty-cycle VALUE mattum verify pannurom --
 *    idhu correct behaviour dhaan, since andha motor "instances"
 *    encapsulated ah irukanumnu design ah irukku.
 *
 * 3. LOGGER CALLS
 *    controller.c la irukka `log_info(...)` / `log_error(...)` calls,
 *    REAL Logger.c moolama pogudhu (mock pannala) -- aana andha test
 *    binary la `log_init()` ஒருபோதும் call aagadhu (production la
 *    main.c thaan adha call pannum), so Logger oda internal `log_uart`
 *    NULL ah irukkும், adhanala `log_output()` edhavum transmit
 *    pannaame silent ah return aagidum. Adhanala Logger oda CAL_UART
 *    dependency ku (mock ah irundhaalum) edhavadhu expectation
 *    set panna theva illa.
 */

#include "unity.h"
#include <setjmp.h>
#include <string.h>

#include "controller.h"
#include "hal_stubs.h"

/* controller.c calls log_info()/log_error() (real Logger.c) and
 * map() (real math_tools.c) directly. Same reasoning as test_IBUS.c:
 * Ceedling only auto-links a real .c file for a header explicitly
 * mentioned in THIS test file, so both are listed here even though
 * we never call their functions directly ourselves. */
#include "Logger.h"
#include "math_tools.h"

#include "mock_IBUS.h"
#include "mock_PMDC.h"

/* Logger.c (real, unmocked) transitively needs CAL_UART.h / CAL_timer.h.
 * Mock them purely so Ceedling doesn't pull in the real, uncompilable-
 * on-PC CAL_UART.c/CAL_timer.c. No expectations will be set on these --
 * see note (3) above for why they're never actually called. */
#include "mock_CAL_UART.h"
#include "mock_CAL_timer.h"
#include "mock_CAL_GPIO.h"
#include "mock_CAL_PWM.h"

/* IBUS channel indices, copied from controller.c's own #defines
 * (controller.c doesn't expose them via its header, so we mirror the
 * ones relevant to our test scenarios here). */
#define IBUS_ANGULAR_CHANNEL 0
#define IBUS_LINEAR_CHANNEL  1
#define IBUS_MACHINE_ON_OFF  4
#define IBUS_BRUSH_ON        5

#define CHANNEL_SWITCH_ON_VALUE  2000
#define CHANNEL_SWITCH_OFF_VALUE 1000
#define CHANNEL_CENTER_VALUE     1500

void setUp(void)
{
    hal_stub_arm_loop_escape(-1);   /* disabled until a test arms it */
}

void tearDown(void) {}

static ibus_packet_t make_packet(bool failsafe, uint16_t angular, uint16_t linear,
                                  uint16_t machine_on_off, uint16_t brush_on)
{
    ibus_packet_t p;
    memset(&p, 0, sizeof(p));
    p.failsafe_engaged           = failsafe;
    p.channel_value[IBUS_ANGULAR_CHANNEL] = angular;
    p.channel_value[IBUS_LINEAR_CHANNEL]  = linear;
    p.channel_value[IBUS_MACHINE_ON_OFF]  = machine_on_off;
    p.channel_value[IBUS_BRUSH_ON]        = brush_on;
    return p;
}

/* ------------------------------------------------------------------ */
/* Scenario 1: IBUS reports failsafe engaged -> every motor must stop  */
/* ------------------------------------------------------------------ */

void test_controller_stops_all_motors_when_failsafe_engaged(void)
{
    ibus_packet_t failsafe_packet =
        make_packet(true, CHANNEL_CENTER_VALUE, CHANNEL_CENTER_VALUE,
                    CHANNEL_SWITCH_OFF_VALUE, CHANNEL_SWITCH_OFF_VALUE);

    /* --- Startup sequence --- */
    ibusInitialize_ExpectAndReturn(&huart1, 0);
    PMDC_init_IgnoreAndReturn(true);   /* called once per motor (4x), any pointer */

    /* --- One loop iteration: IBUS reports failsafe --- */
    ibusUpdateData_ExpectAndReturn(NULL, 0);
    ibusUpdateData_IgnoreArg_current_packet();
    ibusUpdateData_ReturnThruPtr_current_packet(&failsafe_packet);

    /* stopALL() calls PMDC_Stop() on all four motors */
    PMDC_Stop_IgnoreAndReturn(true);

    /* Run exactly 1 loop iteration, then escape via HAL_Delay */
    hal_stub_arm_loop_escape(1);

    if (setjmp(hal_stub_loop_escape) == 0)
    {
        initializeAndRunController();
        TEST_FAIL_MESSAGE("Loop never reached HAL_Delay() -- escape hatch did not fire");
    }
    /* Reaching here means the loop ran once and escaped cleanly */
    TEST_PASS();
}

/* ------------------------------------------------------------------ */
/* Scenario 2: machine ON, centered sticks -> both drive motors at 0%, */
/* brush switch OFF -> brush & pump motors also at 0%                  */
/* ------------------------------------------------------------------ */

void test_controller_drives_motors_at_zero_when_sticks_centered_and_brush_off(void)
{
    ibus_packet_t run_packet =
        make_packet(false, CHANNEL_CENTER_VALUE, CHANNEL_CENTER_VALUE,
                    CHANNEL_SWITCH_ON_VALUE, CHANNEL_SWITCH_OFF_VALUE);

    ibusInitialize_ExpectAndReturn(&huart1, 0);
    PMDC_init_IgnoreAndReturn(true);

    ibusUpdateData_ExpectAndReturn(NULL, 0);
    ibusUpdateData_IgnoreArg_current_packet();
    ibusUpdateData_ReturnThruPtr_current_packet(&run_packet);

    /* Centered sticks -> map(1500,1000,2000,-100,100) = 0 for both axes
     * -> left_drive = 0, right_drive = 0 (in that call order) */
    PMDC_setDutyCycle_ExpectAndReturn(NULL, 0, true);   /* Left  */
    PMDC_setDutyCycle_IgnoreArg_me();
    PMDC_setDutyCycle_ExpectAndReturn(NULL, 0, true);   /* Right */
    PMDC_setDutyCycle_IgnoreArg_me();

    /* Brush switch OFF -> brush=0, pump=0 (in that call order) */
    PMDC_setDutyCycle_ExpectAndReturn(NULL, 0, true);   /* Brush */
    PMDC_setDutyCycle_IgnoreArg_me();
    PMDC_setDutyCycle_ExpectAndReturn(NULL, 0, true);   /* Pump  */
    PMDC_setDutyCycle_IgnoreArg_me();

    hal_stub_arm_loop_escape(1);

    if (setjmp(hal_stub_loop_escape) == 0)
    {
        initializeAndRunController();
        TEST_FAIL_MESSAGE("Loop never reached HAL_Delay() -- escape hatch did not fire");
    }
    TEST_PASS();
}

/* ------------------------------------------------------------------ */
/* Scenario 3: machine ON, brush switch ON -> brush=50%, pump=100%     */
/* ------------------------------------------------------------------ */

void test_controller_turns_on_brush_and_pump_when_brush_switch_on(void)
{
    ibus_packet_t run_packet =
        make_packet(false, CHANNEL_CENTER_VALUE, CHANNEL_CENTER_VALUE,
                    CHANNEL_SWITCH_ON_VALUE, CHANNEL_SWITCH_ON_VALUE);

    ibusInitialize_ExpectAndReturn(&huart1, 0);
    PMDC_init_IgnoreAndReturn(true);

    ibusUpdateData_ExpectAndReturn(NULL, 0);
    ibusUpdateData_IgnoreArg_current_packet();
    ibusUpdateData_ReturnThruPtr_current_packet(&run_packet);

    /* Left/Right at 0% (centered sticks) */
    PMDC_setDutyCycle_ExpectAndReturn(NULL, 0, true);
    PMDC_setDutyCycle_IgnoreArg_me();
    PMDC_setDutyCycle_ExpectAndReturn(NULL, 0, true);
    PMDC_setDutyCycle_IgnoreArg_me();

    /* Brush switch ON -> brush=50, pump=100 (in that call order) */
    PMDC_setDutyCycle_ExpectAndReturn(NULL, 50, true);
    PMDC_setDutyCycle_IgnoreArg_me();
    PMDC_setDutyCycle_ExpectAndReturn(NULL, 100, true);
    PMDC_setDutyCycle_IgnoreArg_me();

    hal_stub_arm_loop_escape(1);

    if (setjmp(hal_stub_loop_escape) == 0)
    {
        initializeAndRunController();
        TEST_FAIL_MESSAGE("Loop never reached HAL_Delay() -- escape hatch did not fire");
    }
    TEST_PASS();
}

/* ------------------------------------------------------------------ */
/* Scenario 4: machine OFF -> stopALL(), even though failsafe is false */
/* ------------------------------------------------------------------ */

void test_controller_stops_all_motors_when_machine_switched_off(void)
{
    ibus_packet_t off_packet =
        make_packet(false, CHANNEL_CENTER_VALUE, CHANNEL_CENTER_VALUE,
                    CHANNEL_SWITCH_OFF_VALUE, CHANNEL_SWITCH_OFF_VALUE);

    ibusInitialize_ExpectAndReturn(&huart1, 0);
    PMDC_init_IgnoreAndReturn(true);

    ibusUpdateData_ExpectAndReturn(NULL, 0);
    ibusUpdateData_IgnoreArg_current_packet();
    ibusUpdateData_ReturnThruPtr_current_packet(&off_packet);

    PMDC_Stop_IgnoreAndReturn(true);

    hal_stub_arm_loop_escape(1);

    if (setjmp(hal_stub_loop_escape) == 0)
    {
        initializeAndRunController();
        TEST_FAIL_MESSAGE("Loop never reached HAL_Delay() -- escape hatch did not fire");
    }
    TEST_PASS();
}
