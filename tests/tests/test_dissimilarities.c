#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <string.h>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)


#include "dissimilarities.h"
#include "classes.h"
#include "values.h"
#include "attributes.h"

#include "mem_utils.h"

const float eps = 1e-5;

#define ARRAY_SIZE(arr)     (sizeof(arr) / sizeof((arr)[0]))

static void test_manhattan(void **state) {

    float value1 = 4;
    float value2 = 8;
    float min = 2;
    float max = 100;
    float manhattan;
    calculate_manhattan(value1, value2, min, max, &manhattan);

    assert_float_equal(manhattan, 0.040816, eps);
}

static void test_svdm(void **state) {

    int attr_idx = 1;
    int num_attr = 2;

    int classes[] = {1, 1, 0, 0, 1, 0, 0};
    int num_inst = ARRAY_SIZE(classes);

    float values[7][2] = {{35, 1},
                        {40, 3},
                        {45, 3},
                        {40, 3},
                        {45, 2},
                        {35, 2},
                        {45, 0}
                        };

    char attr_types[] = {NUMERICAL_TYPE, NOMINAL_TYPE};

    int num_unique_classes;
    int unique_classes[MAX_CLASSES];
    get_unique_classes(num_inst, classes, unique_classes, &num_unique_classes);

    int unique_values_cnt[num_attr][MAX_UNIQUE_VALUES];
    float unique_values[num_attr][MAX_UNIQUE_VALUES];
    int num_unique_values[num_attr];

    get_unique_values(num_attr, attr_types, num_inst, values, num_unique_values, unique_values);

    int values_classes_cnt[num_attr][num_unique_classes][MAX_UNIQUE_VALUES];
    get_unique_values_cnt(num_attr, attr_types, num_unique_classes, unique_classes, classes, 
    num_unique_values, num_inst, MAX_UNIQUE_VALUES, values, unique_values, values_classes_cnt, 
    unique_values_cnt);

    float test_values[] = {50, 1};
    int v1_idx = get_unique_value_idx(num_unique_values[attr_idx], unique_values[attr_idx], 
        test_values[attr_idx]);

    float svdm;
    float svdm_exp[] = {0, 1.3333333333, 1.3333333333, 1.333333333333, 1, 1, 2};
    for (int i = 0; i < num_inst; i++)
    {
        int v2_idx = get_unique_value_idx(num_unique_values[attr_idx], unique_values[attr_idx], 
            values[i][attr_idx]);
        calculate_svdm(attr_idx, v1_idx, v2_idx, num_unique_classes, values_classes_cnt, 
            unique_values_cnt, &svdm);
        assert_float_equal(svdm_exp[i], svdm, eps);
    }
}

static void test_calculate_dissimilairty(void **state) {

    int attr_idx = 1;
    int num_attr = 2;

    int classes[] = {1, 1, 0, 0, 1, 0, 0};
    int num_inst = ARRAY_SIZE(classes);

    float values[7][2] = {{35, 1},
                        {40, 3},
                        {45, 3},
                        {40, 3},
                        {45, 2},
                        {35, 2},
                        {45, 0}
                        };
    char attr_types[] = {NUMERICAL_TYPE, NOMINAL_TYPE};

    int num_unique_classes;
    int unique_classes[MAX_CLASSES];
    get_unique_classes(num_inst, classes, unique_classes, &num_unique_classes);

    int unique_values_cnt[num_attr][MAX_UNIQUE_VALUES];
    float unique_values[num_attr][MAX_UNIQUE_VALUES];
    int num_unique_values[num_attr];

    get_unique_values(num_attr, attr_types, num_inst, values, num_unique_values, unique_values);

    int values_classes_cnt[num_attr][num_unique_classes][MAX_UNIQUE_VALUES];
    get_unique_values_cnt(num_attr, attr_types, num_unique_classes, unique_classes, classes, 
    num_unique_values, num_inst, MAX_UNIQUE_VALUES, values, unique_values, values_classes_cnt, 
    unique_values_cnt);

    float min_values[num_attr];
    float max_values[num_attr];
    get_min_max_attributes(num_attr, num_inst, values, min_values, max_values);

    float test_values[] = {50, 1};
    int v1_idx = get_unique_value_idx(num_unique_values[attr_idx], unique_values[attr_idx], 
        test_values[attr_idx]);

    float dissim;
    float dissim_exp[] = {1.5, 2.3333333333, 1.8333333333, 2.333333333333, 1.5, 2.5, 2.5};
    for (int i = 0; i < num_inst; i++)
    {
        calculate_dissimilairty(test_values, values[i], num_attr, attr_types, 
            min_values, max_values, num_unique_classes, 
            values_classes_cnt, unique_values, 
            unique_values_cnt, num_unique_values, &dissim);
        assert_float_equal(dissim_exp[i], dissim, eps);
    }
}

int main(void) {
    printf("%s\n", __FILENAME__);
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_manhattan),
        cmocka_unit_test(test_svdm),
        cmocka_unit_test(test_calculate_dissimilairty),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
