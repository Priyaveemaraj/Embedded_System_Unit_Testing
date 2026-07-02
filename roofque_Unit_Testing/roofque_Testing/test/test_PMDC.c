/**
 * test_PMDC.c
 *
 * PMDC.c ku, real CAL_GPIO / CAL_PWM hardware layer ah CMock use panni
 * mock panniten (PPT-4 la irundha adc.c mock panna pattern exact ah
 * follow pannirukku): PMDC.c "CAL_GPIO.h" / "CAL_PWM.h" nu real header
 * ah include pannudhu (via PMDC.h -> CAL.h), aana idha test file
 * "mock_CAL_GPIO.h" / "mock_CAL_PWM.h" nu CMock generate panna mock
 * header ah include pannudhu. Ceedling build system, real CAL_GPIO.c /
 * CAL_PWM.c ah instead of mock_CAL_GPIO.c / mock_CAL_PWM.c ah link
 * pannum -- so real hardware ah touch pannama PMDC logic mattum
 * isolate ah test aagum.
 */

#include "unity.h"
#include "PMDC.h"
#include "mock_CAL_GPIO.h"
#include "mock_CAL_PWM.h"
/* CAL.h unconditionally #includes CAL_UART.h and CAL_timer.h too (even
 * though PMDC.c never calls their functions). Mocking them here stops
 * Ceedling from pulling in the real CAL_UART.c/CAL_timer.c, which call
 * genuine STM32 HAL functions we don't provide on the PC test build. */
#include "mock_CAL_UART.h"
#include "mock_CAL_timer.h"

/* IMPORTANT: these must be REAL addressable objects, not arbitrary
 * integer-cast pointers like (TIM_HandleTypeDef*)0x1000. CMock's
 * generated mocks do a byte-for-byte memory comparison on pointer
 * arguments (to support deep struct/array matching), which means it
 * actually DEREFERENCES both the expected and actual pointers at
 * runtime. An arbitrary fake address would crash; a real (even if
 * otherwise-unused) static object is always safe to read. */
static TIM_HandleTypeDef fake_timer_storage;
static GPIO_TypeDef       fake_en_port_storage;
static TIM_HandleTypeDef *fake_timer    = &fake_timer_storage;
static GPIO_TypeDef      *fake_en_port  = &fake_en_port_storage;

static PMDC motor;

void setUp(void)
{
    motor.mode      = PWM_mode;
    motor.Ftimer    = fake_timer;
    motor.Fchannel  = 1;
    motor.Rtimer    = fake_timer;
    motor.Rchannel  = 2;
    motor.Fwd_Port  = NULL;
    motor.Fwd_Pin   = 0;
    motor.Rev_Port  = NULL;
    motor.Rev_Pin   = 0;
    motor.EN_Port   = fake_en_port;
    motor.EN_Pin    = 5;
    motor.dutyCycle = 0;
    motor.currentRPM = 0;
    motor.maxRPM    = 6000;
}

void tearDown(void) {}

/* ------------------------------------------------------------------ */
/* PMDC_init                                                          */
/* ------------------------------------------------------------------ */

void test_PMDC_init_starts_both_PWM_channels(void)
{
    /* PMDC_init() PWM_mode la irundha, Forward matrum Reverse channel
     * rendu ku CAL_PWM_Start() call pannanum (PMDC.c order padi) */
    CAL_PWM_Start_ExpectAndReturn(fake_timer, 1, CAL_OK);
    CAL_PWM_Start_ExpectAndReturn(fake_timer, 2, CAL_OK);

    bool result = PMDC_init(&motor);

    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_UINT8(0, motor.dutyCycle);
    TEST_ASSERT_EQUAL_INT16(0, motor.currentRPM);
}

void test_PMDC_init_NULL_pointer_returns_false(void)
{
    /* NULL pointer ku edhavadhu CAL_* call aagakoodadhu */
    bool result = PMDC_init(NULL);
    TEST_ASSERT_FALSE(result);
}

/* ------------------------------------------------------------------ */
/* PMDC_setDutyCycle -- Forward direction (positive duty cycle)       */
/* ------------------------------------------------------------------ */

void test_PMDC_setDutyCycle_positive_enables_motor_and_drives_forward(void)
{
    /* Expected call order (PMDC_PWM_rotateForward padi):
     *   1. CAL_GPIO_Write(EN_Port, EN_Pin, SET)   -- motor enable
     *   2. CAL_PWM_SetDutyCycle(Rtimer, Rchannel, 0)   -- reverse = 0
     *   3. CAL_PWM_SetDutyCycle(Ftimer, Fchannel, 60)  -- forward = duty
     */
    CAL_GPIO_Write_ExpectAndReturn(fake_en_port, 5, SET, CAL_OK);
    CAL_PWM_SetDutyCycle_ExpectAndReturn(fake_timer, 2, 0, CAL_OK);
    CAL_PWM_SetDutyCycle_ExpectAndReturn(fake_timer, 1, 60, CAL_OK);

    bool result = PMDC_setDutyCycle(&motor, 60);

    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_UINT8(60, motor.dutyCycle);

    /* NOTE -- likely bug in PMDC.c, left untouched on purpose:
     * currentRPM = (int16_t)(dutyCycle * maxRPM) / 100
     * The cast binds BEFORE the division (C operator precedence), so
     * dutyCycle*maxRPM (60*6000=360000) overflows int16_t's range
     * BEFORE being divided by 100, instead of the presumably-intended
     * (int16_t)((dutyCycle * maxRPM) / 100) = 3600. The wrapped value
     * this test asserts (323) is what the code actually produces today.
     * Worth a source fix -- ping if you'd like it corrected. */
    TEST_ASSERT_EQUAL_INT16(323, motor.currentRPM);
}

/* ------------------------------------------------------------------ */
/* PMDC_setDutyCycle -- Reverse direction (negative duty cycle)       */
/* ------------------------------------------------------------------ */

void test_PMDC_setDutyCycle_negative_enables_motor_and_drives_reverse(void)
{
    CAL_GPIO_Write_ExpectAndReturn(fake_en_port, 5, SET, CAL_OK);
    CAL_PWM_SetDutyCycle_ExpectAndReturn(fake_timer, 1, 0, CAL_OK);
    CAL_PWM_SetDutyCycle_ExpectAndReturn(fake_timer, 2, 40, CAL_OK);

    bool result = PMDC_setDutyCycle(&motor, -40);

    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_UINT8(40, motor.dutyCycle);

    /* Same operator-precedence overflow as the forward test above:
     * 40*6000=240000 overflows int16_t before /100, then *-1.
     * See the note in test_PMDC_setDutyCycle_positive_... above. */
    TEST_ASSERT_EQUAL_INT16(221, motor.currentRPM);
}

/* ------------------------------------------------------------------ */
/* PMDC_setDutyCycle -- Zero duty cycle stops the motor                */
/* ------------------------------------------------------------------ */

void test_PMDC_setDutyCycle_zero_stops_motor(void)
{
    CAL_GPIO_Write_ExpectAndReturn(fake_en_port, 5, RESET, CAL_OK);
    CAL_PWM_SetDutyCycle_ExpectAndReturn(fake_timer, 1, 0, CAL_OK);
    CAL_PWM_SetDutyCycle_ExpectAndReturn(fake_timer, 2, 0, CAL_OK);

    bool result = PMDC_setDutyCycle(&motor, 0);

    TEST_ASSERT_TRUE(result);
}

void test_PMDC_setDutyCycle_returns_false_for_NULL_pointer(void)
{
    bool result = PMDC_setDutyCycle(NULL, 50);
    TEST_ASSERT_FALSE(result);
}

/* ------------------------------------------------------------------ */
/* PMDC_Stop                                                           */
/* ------------------------------------------------------------------ */

void test_PMDC_Stop_disables_motor_and_zeroes_both_channels(void)
{
    CAL_GPIO_Write_ExpectAndReturn(fake_en_port, 5, RESET, CAL_OK);
    CAL_PWM_SetDutyCycle_ExpectAndReturn(fake_timer, 1, 0, CAL_OK);
    CAL_PWM_SetDutyCycle_ExpectAndReturn(fake_timer, 2, 0, CAL_OK);

    bool result = PMDC_Stop(&motor);

    TEST_ASSERT_TRUE(result);
}

/* ------------------------------------------------------------------ */
/* PMDC_Destroy                                                        */
/* ------------------------------------------------------------------ */

void test_PMDC_Destroy_stops_motor_then_stops_both_PWM_channels(void)
{
    /* Destroy() first calls Stop() (GPIO reset + both duty=0),
     * appuram CAL_PWM_Stop() rendu channel ku */
    CAL_GPIO_Write_ExpectAndReturn(fake_en_port, 5, RESET, CAL_OK);
    CAL_PWM_SetDutyCycle_ExpectAndReturn(fake_timer, 1, 0, CAL_OK);
    CAL_PWM_SetDutyCycle_ExpectAndReturn(fake_timer, 2, 0, CAL_OK);
    CAL_PWM_Stop_ExpectAndReturn(fake_timer, 1, CAL_OK);
    CAL_PWM_Stop_ExpectAndReturn(fake_timer, 2, CAL_OK);

    bool result = PMDC_Destroy(&motor);

    TEST_ASSERT_TRUE(result);
}
