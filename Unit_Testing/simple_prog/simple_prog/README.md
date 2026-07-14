# simple_prog — Ceedling + Unity + CMock Demo

Ungal PPT-3 (Unity) matrum PPT-4 (Mocking/CMock) la irundha example-a based pannu, oru complete, run pannalaam nu irukka Ceedling project.

## Enna irukku indha project la?

```
simple_prog/
├── project.yml                  # Ceedling configuration (Unity + CMock + gcov)
├── src/
│   ├── adc.h / adc.c            # "Dependency" module — ithu mock pannuvom
│   └── bit_manipulation.h/.c    # Module under test — adc_read() ah use pannudhu
└── test/
    └── test_bit_manipulation.c  # Unity test cases, CMock generated mock_adc.h use panni
```

## Idea enna?

`bit_manipulation.c` oda `do_bit_man()` function, `adc.c` oda `adc_read()` ah call pannudhu.
Real `adc_read()` ah use pannina, hardware dependency vandhudum — test isolated ah irukkadhu.

Adhukaga, test file la `#include "adc.h"` nu illama `#include "mock_adc.h"` nu pananom.
CMock automatic ah `adc.h` file paathu `mock_adc.h` / `mock_adc.c` generate pannidum.
Adha vachu, `adc_read_ExpectAndReturn(value)` nu test la value control pannalaam — real ADC hardware venaam.

## Setup (oru thadava mattum)

1. **Ruby install pannunga** (Ceedling Ruby gem):
   ```
   sudo apt-get install ruby
   ```
2. **GCC compiler** irukkanum (already Linux la irukkum, illna):
   ```
   sudo apt-get install build-essential
   ```
3. **Ceedling gem install pannunga:**
   ```
   gem install ceedling
   ```
4. Verify pannunga:
   ```
   ceedling version
   ```

## Run pannradhu eppadi?

Indha `simple_prog` folder ku pogunga, appuram:

```
cd simple_prog
ceedling test:all
```

Expected output (kitta kitta):

```
Test 'test_bit_manipulation.c'
------------------------------
Generating runner for test_bit_manipulation.c...
Compiling test_bit_manipulation_runner.c...
Compiling test_bit_manipulation.c...
Compiling unity.c...
Compiling bit_manipulation.c...
Compiling cmock.c...
Compiling mock_adc.c...
Linking test_bit_manipulation.out...
Running test_bit_manipulation.out...

-----------
TEST OUTPUT
-----------
[test_bit_manipulation.c]
  - "Temperature is Low"
  - "Temperature is High"

--------------------
OVERALL TEST SUMMARY
--------------------
TESTED:  4
PASSED:  4
FAILED:  0
IGNORED: 0
```

## Code Coverage report (gcov)

```
ceedling gcov:all
ceedling utils:gcov
```

Report `build/artifacts/gcov/` la generate aagum (HTML report um kidaikum).
`adc.c` ippo test scope la illa (adha mock panniten), so 100% dhaan varum;
`bit_manipulation.c` ku dhaan coverage %  meaningful ah irukkum.

## Innum test cases add panna vendumna

`test/test_bit_manipulation.c` la irukka pattern follow pannunga:

```c
void test_my_new_case(void)
{
    adc_read_ExpectAndReturn(50);   // ADC 50 return pannum nu expect pannuren
    int8_t result = do_bit_man(2);
    TEST_ASSERT_EQUAL_INT8(0, result);
}
```

Vera vera CMock variant venumna (Ignore, IgnoreArg, Array, ReturnThruPtr, StubWithCallback),
PPT-4 la irukka table paarunga — antha function names direct ah use pannalaam,
CMock automatic ah generate pannidum `mock_adc.h` file la.
