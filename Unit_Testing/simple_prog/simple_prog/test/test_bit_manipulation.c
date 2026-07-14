#include "unity.h"
#include "bit_manipulation.h"
#include "mock_adc.h"   /* CMock auto-generates this from adc.h */

extern uint8_t Jill;
extern uint8_t Jung;
extern uint8_t Jukk;

void setUp(void)
{
    Jill = 0x00;
    Jung = 0xFF;
    Jukk = 0x00;
}

void tearDown(void)
{
}

/* Test Case 0: invalid position (too high) -> should return -1, no ADC call */
void test_do_bit_man_invalid_position_high(void)
{
    int8_t result;

    result = do_bit_man( 15 );

    TEST_ASSERT_EQUAL_INT8( -1, result );
    TEST_ASSERT_EQUAL_INT8( 0x00, Jill );
    TEST_ASSERT_EQUAL_INT8( 0xFF, Jung );
    TEST_ASSERT_EQUAL_INT8( 0x00, Jukk );
}

/* Test Case 1: invalid position (negative) -> should return -1, no ADC call */
void test_do_bit_man_invalid_position_negative(void)
{
    int8_t result;

    result = do_bit_man( -1 );

    TEST_ASSERT_EQUAL_INT8( -1, result );
    TEST_ASSERT_EQUAL_INT8( 0x00, Jill );
    TEST_ASSERT_EQUAL_INT8( 0xFF, Jung );
    TEST_ASSERT_EQUAL_INT8( 0x00, Jukk );
}

/* Test Case 2: valid position, ADC reads LOW temperature (< 30) */
void test_do_bit_man_valid_position_temp_low(void)
{
    int8_t result;
    int8_t position = 5;

    /* Mock adc_read() so the real hardware/module is never touched */
    adc_read_ExpectAndReturn(10);   /* pretend ADC returns 10 -> "Low" */

    result = do_bit_man( position );

    TEST_ASSERT_EQUAL_INT8( 0, result );
    TEST_ASSERT_BIT_HIGH( position, Jill );
    TEST_ASSERT_BIT_LOW( position, Jung );
    TEST_ASSERT_BIT_HIGH( position, Jukk );
}

/* Test Case 3: valid position, ADC reads HIGH temperature (>= 30) */
void test_do_bit_man_valid_position_temp_high(void)
{
    int8_t result;
    int8_t position = 3;

    adc_read_ExpectAndReturn(45);   /* pretend ADC returns 45 -> "High" */

    result = do_bit_man( position );

    TEST_ASSERT_EQUAL_INT8( 0, result );
    TEST_ASSERT_BIT_HIGH( position, Jill );
    TEST_ASSERT_BIT_LOW( position, Jung );
    TEST_ASSERT_BIT_HIGH( position, Jukk );
}
