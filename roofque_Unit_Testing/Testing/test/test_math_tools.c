/* Pure Unity tests for the map() function in math_tools.c.
 *
 * This function has no HAL or CAL dependencies (it does not include
 * "CAL/CAL.h"), so there is no need to use CMock or create any mocks.
 * We can directly use the TEST_ASSERT_EQUAL_INT variants, as shown
 * in PPT-3.
 */

#include "unity.h"
#include "math_tools.h"

void setUp(void) {}
void tearDown(void) {}

/* controller.c uses the map() function as follows:
 *
 *     map(channel_value, 1000, 2000, -100, 100)
 *
 * to convert the raw IBUS channel value (1000–2000) into a velocity
 * percentage ranging from -100% to +100%.
 *
 * This test uses that exact input and output range to verify the
 * expected behavior.
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

/* Example of the TEST_ASSERT_*_MESSAGE variant.
 * It provides a clear reason for the failure when the assertion
 * does not pass (see PPT-3, Slide 21: _MESSAGE variant).
 */

void test_map_full_scale_range_MESSAGE_example(void)
{
    long result = map(1500, 1000, 2000, 0, 100);
    TEST_ASSERT_EQUAL_INT_MESSAGE(50, result,
        "1500 is midpoint of [1000,2000], should map to midpoint of [0,100]");
}
