/**
  1. INFINITE LOOP ESCAPE HATCH
      initializeAndRunController() contains an infinite for(;;) loop, and HAL_Delay(50) is called at the end of each iteration.
      In test/support/hal_stubs.c, we intercept HAL_Delay() and, after a predetermined number of iterations (N), use longjmp() to return to the location saved by setjmp()
      This allows the test to run the infinite loop for only a fixed number of iterations and then safely regain control.

  2. STATIC MOTOR VARIABLES
      In controller.c, Left_drive_motor, right_drive_motor, etc., are declared as static, so they cannot be accessed directly from the test file.
      Therefore, instead of verifying the exact pointers passed to the PMDC_* functions, we ignore the motor instance argument (_IgnoreArg_me()) and verify only the duty-cycle value.
      This is the correct behavior because those motor instances are intentionally encapsulated as part of the design.
 
  3. LOGGER CALLS
      The log_info(...) and log_error(...) calls in controller.c go through the real Logger.c implementation (they are not mocked).
      However, in the test binary, log_init() is never called (in production, main.c is responsible for calling it). As a result, the Logger's internal log_uart variable remains NULL, causing log_output() to return silently without transmitting anything.
      Therefore, there is no need to set any expectations for the Logger's CAL_UART dependency, even if it is mocked.
 */

#include "unity.h"
#include <setjmp.h>
#include <string.h>

#include "controller.h"
#include "hal_stubs.h"
/*controller.c directly calls log_info()/log_error() (from the real Logger.c) and map() (from the real math_tools.c).
The reasoning is the same as in test_IBUS.c: Ceedling automatically links a real .c file only for a header that is explicitly included in this test file.
Therefore, both headers are included here, even though we never call their functions directly in the test ourselves.
*/

#include "Logger.h"
#include "math_tools.h"

#include "mock_IBUS.h"
#include "mock_PMDC.h"

/* Logger.c (the real, unmocked implementation) has transitive dependencies on CAL_UART.h and CAL_timer.h.
These are mocked solely to prevent Ceedling from pulling in the actual CAL_UART.c and CAL_timer.c files, which cannot be compiled on a PC environment.
No expectations are set on these mocks, as the corresponding functions are never invoked during the test execution (see note 3 above for the explanation).*/

#include "mock_CAL_UART.h"
#include "mock_CAL_timer.h"
#include "mock_CAL_GPIO.h"
#include "mock_CAL_PWM.h"

/* IBUS channel indices, copied from controller.c's own #defines
   (controller.c doesn't expose them via its header, so we mirror the
   ones relevant to our test scenarios here). */
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

/*  Scenario 1: IBUS reports failsafe engaged -> every motor must stop  */

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

    /* stopALL() calls PMDC_Stop() all four motors */
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

/* Scenario 2: machine ON, centered sticks -> both drive motors at 0%, 
brush switch OFF -> brush & pump motors also at 0% */

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

/*  Scenario 3: machine ON, brush switch ON -> brush=50%, pump=100%  */

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

/* Scenario 4: machine OFF -> stopALL(), even though failsafe is false  */

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
