#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <string.h>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)


typedef uintmax_t LargestIntegralType;

#include "values.h"
#include "classes.h"
#include "attributes.h"

#include "mem_utils.h"

const float eps = 1e-5;

#define ARRAY_SIZE(arr)     (sizeof(arr) / sizeof((arr)[0]))


static void test_get_min_max_attributes(void **state) 
{
    int num_attr = 10;
    int num_inst = 15;
    float values[15][10] = {{16, 14,  7,  8,  0, 16,  2,  3, 10, 14},
                            {15, 13, 11, 12, 10,  2, 17, 17, 17,  7},
                            { 0,  2,  7,  6,  0,  2, 11, 14, 10,  5},
                            { 6,  5,  3,  7,  1,  3, 16,  7, 19,  0},
                            { 3, 11,  3,  4,  2,  9, 10, 10, 17,  7},
                            {11, 17,  2,  3,  4,  9,  8, 13, 14,  7},
                            { 8,  6,  6, 10, 12,  9,  0,  2,  5,  4},
                            {10, 10,  7,  3, 14,  3,  3, 17,  4, 19},
                            {15,  2,  8,  4,  8, 13,  4,  6,  2,  8},
                            { 5,  8, 18,  4,  1, 19, 10, 15,  0,  3},
                            {15,  9,  2,  4, 10, 18,  3, 16, 18, 14},
                            {11,  1,  8, 16, 19,  0,  9,  7, 14, 11},
                            {13, 17,  9,  0,  7,  0, 11,  5, 18, 11},
                            { 1, 14,  1,  9,  6,  2,  9, 14,  6,  7},
                            {14, 19, 11, 12,  1, 12, 14,  1, 15, 11}};
    float min_values[num_attr];
    float max_values[num_attr];
    get_min_max_attributes(num_attr, num_inst, values, min_values, max_values);

    float min_values_exp[] = {0, 1, 1, 0, 0, 0, 0, 1, 0, 0};
    float max_values_exp[] = {16, 19, 18, 16, 19, 19, 17, 17, 19, 19};

    for (int i = 0; i < num_attr; i++)
    {
        assert_float_equal(min_values_exp[i], min_values[i], eps);
        assert_float_equal(max_values_exp[i], max_values[i], eps);
    }
}

static void test_get_num_numerical_and_nominal_attr(void **state) 
{
    char attr_types[] = {NOMINAL_TYPE, NOMINAL_TYPE, NOMINAL_TYPE, NOMINAL_TYPE, NOMINAL_TYPE, 
            NUMERICAL_TYPE, NOMINAL_TYPE, NOMINAL_TYPE, NUMERICAL_TYPE, NUMERICAL_TYPE};
    int num_attr = ARRAY_SIZE(attr_types);
    int num_numerical_attr;
    int num_nominal_attr;
    get_num_numerical_and_nominal_attr(attr_types, num_attr, &num_numerical_attr, 
        &num_nominal_attr);

    int num_numerical_attr_exp = 3;
    int num_nominal_attr_exp = 7;

    assert_int_equal(num_numerical_attr_exp, num_numerical_attr);
    assert_int_equal(num_nominal_attr_exp, num_nominal_attr);
}

int main(void) {
    printf("%s\n", __FILENAME__);
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_get_min_max_attributes),
        cmocka_unit_test(test_get_num_numerical_and_nominal_attr),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
