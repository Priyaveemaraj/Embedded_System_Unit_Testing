# robot_controller_tests — Ceedling + Unity + CMock for your actual project

Ungal `controller.c`, `PMDC`, `IBUS`, `Logger`, `math_tools`, `CAL` (Common
Abstraction Layer) files ah base pannitu, oru real Ceedling test project
build panniten — `simple_prog` demo la irundha exact same pattern
(PPT-3 Unity assertions + PPT-4 CMock mocking).

## Folder Structure (ungal real project ah exact ah mirror pannirukku)

```
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
```

## Why the fake `stm32g0xx_hal.h` and `gpio.h`?

Ungal `CAL.h` real STM32 hardware SDK (`stm32g0xx_hal.h`) expect pannudhu.
Adhu unga IDE (STM32CubeIDE) la irukkum, aana namba idha **PC-la, GCC vachu**
compile pannurom (unit tests hardware la run pannradhu illa — CPU la
directly run pannurom, adhukaga real hardware types theva illa).

Adhanala `test/support/stm32g0xx_hal.h` nu oru **minimal stub** create
panniten — `GPIO_TypeDef`, `TIM_HandleTypeDef`, `UART_HandleTypeDef` mudhaliya
types ah "opaque" (empty) struct ah declare pannirukku. Idhu real hardware
register layout theva illa, enna vena — namba andha pointers ah
**dereference pannave maatom**, andha work ellam CMock mock pannitu
irukkum (`CAL_GPIO_Write`, `CAL_PWM_SetDutyCycle` mudhaliyavai).

**Idhu real firmware build ku use pannadheenga** — idhu `test/support/`
folder la mattum irukku, so `ceedling test:all` run pannumbodhu mattum
pick aagum. Unga real STM32CubeIDE project andha real HAL header ah
already use pannikittu irukku, ithoda edhavum touch pannaadhu.

## Setup

Munnadi share panna `simple_prog` mari:

```
sudo apt-get install ruby build-essential
gem install ceedling
```

## Run pannradhu

```
cd robot_controller_tests
ceedling test:all
```

## Ovvoru module-ku enna test pannirukom?

### 1. `math_tools.c` — Pure logic, mocking venaame illa
`map()` function ku edhavadhu hardware dependency illa (`CAL.h`
include pannala), so straight ah `TEST_ASSERT_EQUAL_INT` use panni
5 test cases eludhirukom — controller.c la use pannra exact range
(1000-2000 → -100 to +100) vachu.

### 2. `PMDC.c` — CMock mocks `CAL_GPIO` + `CAL_PWM`
`PMDC_init`, `PMDC_setDutyCycle` (forward/reverse/stop),
`PMDC_Stop`, `PMDC_Destroy` — ellathukkum, real motor hardware
touch pannama, `CAL_GPIO_Write_ExpectAndReturn(...)` matrum
`CAL_PWM_SetDutyCycle_ExpectAndReturn(...)` vachu exact call order &
arguments verify pannirukom (`enforce_strict_ordering: TRUE` project.yml
la set pannirukken, so wrong order na test fail aagum).

**🐛 Testing la kண்டுpidicha oru bug (source touch pannala):**
`PMDC.c` line 49/58 la:
```c
me->currentRPM = (int16_t) ((me->dutyCycle) * (me->maxRPM)) / (100);
```
C operator precedence padi, `(int16_t)` cast **division-ku munnadi**
apply aagudhu — so `dutyCycle * maxRPM` (e.g. 60 × 6000 = 360000)
`int16_t` range (max 32767) ah **overflow pannitu**, appuram dhaan 100
ah divide pannudhu. Result: 60% duty ku `currentRPM` **3600 illama 323**
nu varudhu (wraparound value). Test cases idha current (buggy)
behavior ku match aagum padி eludhi irukken, oru comment-oda. Idha fix
pannanuma sonna, sollunga — `(int16_t)((me->dutyCycle * me->maxRPM) / 100)`
nu bracket ah maathina sari aagidum.

### 3. `IBUS.c` — CMock mocks `CAL_UART` + `CAL_timer`
`ibusInitialize`, `ibusUpdateData`, `ibusCallback` ku DMA reception &
timer ticks ah mock pannirukom.

**⚠️ Test order matters here** — `IBUS.c` la `ibus_uart`,
`new_ibus_data_received` mudhaliyavai `static` (file-scope) variables,
so test-to-test state persist aagum. Test file la eludhi irukka order
(`test_1_...`, `test_2_...`, `test_3_...`) follow pannanum. Idha
better ah panna, `IBUS.c` la oru `ibusReset()` (test-only) function
add pannalaam — production code ku andha function safe ah irukkum,
aana test setUp() la call panni fresh state guarantee pannalaam.

### 4. `Logger.c` — CMock mocks `CAL_UART` + `CAL_timer`
Log level threshold logic (`log_set_level`) sariya work aaguthaanu,
matrum message transmit/suppress aaguthaanu verify pannirukom.

## `controller.c` yaen test pannala?

`initializeAndRunController()` oru **infinite `for(;;)` loop** — idhu
production firmware ku sariyaanadhu (embedded main loop), aana **unit
test ah call panna mudiyadhu** because adhu ever return pannadhu!
Idha test panna, function ah rendu ah split pannanum:

```c
void controllerInit(void);          // setup once
void controllerRunOnce(void);       // one iteration of the loop body
```

appuram `main.c` la:
```c
controllerInit();
for(;;) { controllerRunOnce(); HAL_Delay(50); }
```

Idha refactor panna solreengala? Sonna, andha split panni,
`controllerRunOnce()` ku full test suite (mocking Logger, IBUS,
PMDC ellam vachu) eludhi tharen.

## Adding more tests

Same pattern follow pannunga:
```c
#include "unity.h"
#include "ModuleUnderTest.h"
#include "mock_Dependency.h"

void test_something(void)
{
    Dependency_function_ExpectAndReturn(args..., return_value);
    // call function under test
    // TEST_ASSERT_* the result
}
```
