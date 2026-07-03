robot_controller_tests — Ceedling + Unity + CMock for the actual project

This test suite was built around the real project files — controller.c,
PMDC, IBUS, Logger, math_tools, and CAL (Common Abstraction
Layer) — using a real Ceedling test project. It follows the same pattern
demonstrated in the simple_prog example (PPT-3 Unity assertions +
PPT-4 CMock mocking).

Folder Structure (mirrors the real project structure)

robot_controller_tests/
├── project.yml
├── src/
│   ├── CAL/
│   │   ├── CAL.h
│   │   ├── CAL_GPIO/CAL_GPIO.c/.h
│   │   ├── CAL_PWM/CAL_PWM.c/.h
│   │   ├── CAL_UART/CAL_UART.c/.h
│   │   ├── CAL_timer/CAL_timer.c/.h
│   │   ├── CAL_CAN/CAL_CAN.c/.h        (unused — USE_CAL_CAN off)
│   │   └── CAL_i2c/CAL_i2c.c/.h        (unused — USE_CAL_I2C off)
│   ├── Modules/
│   │   ├── Utilities/Tools/math_tools.c/.h
│   │   ├── Utilities/Logger/Logger.c/.h
│   │   ├── COMMS/IBUS/IBUS.c/.h
│   │   └── Actuators/PMDC/PMDC.c/.h
│   └── controller.c/.h
└── test/
    ├── test_math_tools.c    ← plain Unity, no mocking
    ├── test_PMDC.c          ← CMock mocks CAL_GPIO + CAL_PWM
    ├── test_IBUS.c          ← CMock mocks CAL_UART + CAL_timer
    ├── test_Logger.c        ← CMock mocks CAL_UART + CAL_timer
    └── support/
        ├── stm32g0xx_hal.h  ← TEST-ONLY fake HAL header (see below)
        └── gpio.h           ← TEST-ONLY stub for CubeMX gpio.h

Why the fake stm32g0xx_hal.h and gpio.h?

The real CAL.h expects the actual STM32 hardware SDK
(stm32g0xx_hal.h), which is normally provided by STM32CubeIDE.
Since these unit tests run on a PC using GCC (not on real hardware),
the real HAL types are not available and are not needed — the tests
never touch real registers.

To solve this, test/support/stm32g0xx_hal.h defines a minimal stub:
types like GPIO_TypeDef, TIM_HandleTypeDef, and UART_HandleTypeDef
are declared as simple structs. The actual register layout doesn't
matter here, because these pointers are never dereferenced — all
real hardware interaction is mocked out by CMock
(CAL_GPIO_Write, CAL_PWM_SetDutyCycle, etc.).

This stub is never used in the real firmware build. It lives only
inside test/support/, so it's only picked up when running
ceedling test:all. The real STM32CubeIDE project continues to use the
actual HAL header and is completely unaffected by this file.

Setup

Same as the earlier simple_prog example:

bashsudo apt-get install ruby build-essential
gem install ceedling

Running the tests

bashcd robot_controller_tests
ceedling test:all

What each module tests

1. math_tools.c — Pure logic, no mocking needed

The map() function has no hardware dependency (doesn't include
CAL.h), so it's tested directly with TEST_ASSERT_EQUAL_INT across
5 test cases, using the exact input range used in controller.c
(1000–2000 → -100 to +100).

2. PMDC.c — CMock mocks CAL_GPIO + CAL_PWM

PMDC_init, PMDC_setDutyCycle (forward/reverse/stop), PMDC_Stop,
and PMDC_Destroy are all tested without touching real motor hardware.
CAL_GPIO_Write_ExpectAndReturn(...) and
CAL_PWM_SetDutyCycle_ExpectAndReturn(...) verify the exact call order
and arguments (enforce_strict_ordering: TRUE is set in project.yml,
so calls made in the wrong order will fail the test).

🐛 A bug found during testing (source code was not modified):
In PMDC.c, lines 49 and 58:

cme->currentRPM = (int16_t) ((me->dutyCycle) * (me->maxRPM)) / (100);

Due to C operator precedence, the (int16_t) cast is applied before
the division. So dutyCycle * maxRPM (e.g. 60 × 6000 = 360000)
overflows the int16_t range (max 32767) before being divided by 100.
As a result, at 60% duty cycle, currentRPM comes out as 323 instead
of 3600 (a wraparound value). The test cases were written to match
this current (buggy) behavior, with a comment explaining why. If you'd
like this fixed, the correct fix is:

cme->currentRPM = (int16_t)((me->dutyCycle * me->maxRPM) / 100);

(simply moving the cast to apply after the division)

3. IBUS.c — CMock mocks CAL_UART + CAL_timer

ibusInitialize, ibusUpdateData, and ibusCallback are tested with
mocked DMA reception and timer ticks.

⚠️ Test order matters here — IBUS.c uses static (file-scope)
variables like ibus_uart and new_ibus_data_received, so state
persists between tests. The test file relies on running in the order
they're written (test_1_..., test_2_..., test_3_...). A cleaner
long-term solution would be to add a test-only ibusReset() function
to IBUS.c — safe for production, but callable from setUp() to
guarantee a fresh state before each test.

4. Logger.c — CMock mocks CAL_UART + CAL_timer

Verifies that the log level threshold logic (log_set_level) works
correctly, and that messages are transmitted or suppressed as expected.

Why isn't controller.c tested?

initializeAndRunController() contains an infinite for(;;) loop —
correct behavior for production firmware (an embedded main loop), but
it cannot be called directly from a unit test, since it never
returns. To make it testable, the function would need to be split:

cvoid controllerInit(void);          // setup once
void controllerRunOnce(void);       // one iteration of the loop body

and then in main.c:

ccontrollerInit();
for(;;) { controllerRunOnce(); HAL_Delay(50); }

If this refactor is approved, a full test suite for
controllerRunOnce() (mocking Logger, IBUS, and PMDC) can be added.

Adding more tests

Follow the same pattern:

c#include "unity.h"
#include "ModuleUnderTest.h"
#include "mock_Dependency.h"

void test_something(void)
{
    Dependency_function_ExpectAndReturn(args..., return_value);
    // call function under test
    // TEST_ASSERT_* the result
}
