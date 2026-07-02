/**
 * test_math_tools.c
 *
 * math_tools.c la irukka map() function ku pure Unity tests.
 * Idhukku edhavadhu HAL/CAL dependency illa (no #include "CAL/CAL.h"),
 * so CMock mocking type venaame illa -- PPT-3 la paatha
 * TEST_ASSERT_EQUAL_INT variants nera use pannalaam.
 */

#include "unity.h"
#include "math_tools.h"

void setUp(void) {}
void tearDown(void) {}

/* controller.c la ippadi use pannirukanga:
 *   map(channel_value, 1000, 2000, -100, 100)
 * IBUS channel raw value (1000-2000) ah -100% to +100% velocity ah convert
 * pannradhukku. Andha exact range ah vechu test pannurom.
 */

void test_map_midpoint_maps_to_zero(void)
{
    long result = map(1500, 1000, 2000, -100, 100);
    TEST_ASSERT_EQUAL_INT(0, result);
}

void test_map_minimum_input_maps_to_minimum_output(void)
{
    long result = map(1000, 1000, 2000, -100, 100);
    TEST_ASSERT_EQUAL_INT(-100, result);
}

void test_map_maximum_input_maps_to_maximum_output(void)
{
    long result = map(2000, 1000, 2000, -100, 100);
    TEST_ASSERT_EQUAL_INT(100, result);
}

void test_map_quarter_point_scales_proportionally(void)
{
    long result = map(1250, 1000, 2000, -100, 100);
    TEST_ASSERT_EQUAL_INT(-50, result);
}

void test_map_three_quarter_point_scales_proportionally(void)
{
    long result = map(1750, 1000, 2000, -100, 100);
    TEST_ASSERT_EQUAL_INT(50, result);
}

/* IBUS_MESSAGE variant example -- edhukaga fail aaguchunu clear ah sollum
 * (PPT-3 Slide 21: _MESSAGE variant) */
void test_map_full_scale_range_MESSAGE_example(void)
{
    long result = map(1500, 1000, 2000, 0, 100);
    TEST_ASSERT_EQUAL_INT_MESSAGE(50, result,
        "1500 is midpoint of [1000,2000], should map to midpoint of [0,100]");
}
