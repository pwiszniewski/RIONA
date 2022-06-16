#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <string.h>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)


typedef uintmax_t LargestIntegralType;

#include "classes.h"

#include "mem_utils.h"

const float eps = 1e-5;

#define ARRAY_SIZE(arr)     (sizeof(arr) / sizeof((arr)[0]))


static void test_get_unique_classes(void **state) 
{
    int classes[] = {8, 6, 10, 6, 5, 9, 6, 8, 6, 9};
    int num_inst = ARRAY_SIZE(classes);
    int unique_classes[num_inst];
    int num_unique_classes;

    get_unique_classes(num_inst, classes, unique_classes, &num_unique_classes);
    int num_unique_classes_exp = 5;
    LargestIntegralType unique_classes_exp[] = {5, 6, 8, 9, 10};

    assert_int_equal(num_unique_classes_exp, num_unique_classes);
    LargestIntegralType unique_classes_cast[num_unique_classes_exp];
    for (int i = 0; i < num_unique_classes; i++)
    {
        unique_classes_cast[i] = unique_classes[i];
    }
    for (int i = 0; i < num_unique_classes; i++)
    {
        assert_in_set(unique_classes_cast[i], unique_classes_exp, num_unique_classes_exp);
        assert_in_set(unique_classes_exp[i], unique_classes_cast, num_unique_classes_exp);
    }
}

static void test_get_unique_class_idx(void **state) 
{
    int unique_classes[] = {13, 15, 16, 9, 30, 14, 12, 2, 22, 6};
    int num_unique_classes = ARRAY_SIZE(unique_classes);
    assert_int_equal(0, get_unique_class_idx(num_unique_classes, unique_classes, 13));
    assert_int_equal(-1, get_unique_class_idx(num_unique_classes, unique_classes, 55));
    assert_int_equal(4, get_unique_class_idx(num_unique_classes, unique_classes, 30));
}

static void test_get_classes_cnt(void **state) 
{
    int classes[] = {8, 6, 10, 6, 5, 9, 6, 8, 6, 9};
    int num_inst = ARRAY_SIZE(classes);
    int unique_classes[num_inst];
    int num_unique_classes;

    get_unique_classes(num_inst, classes, unique_classes, &num_unique_classes);
    int classes_cnt[num_unique_classes];
    get_classes_cnt(num_unique_classes, unique_classes, num_inst, classes, classes_cnt);

    int unique_classes_exp[] = {5, 6, 8, 9, 10};
    int classes_cnt_exp[] =    {1, 4, 2, 2, 1};

    for (int i = 0; i < num_unique_classes; i++)
    {
        int idx = get_unique_class_idx(num_unique_classes, unique_classes, unique_classes_exp[i]);
        assert_int_equal(classes_cnt_exp[i], classes_cnt[idx]);
    }

}

static void test_most_freq_class_idx(void **state) 
{
    int classes_cnt[] = {49, 87, 71, 64, 80, 92, 29, 83, 14, 57};
    int num_unique_classes = ARRAY_SIZE(classes_cnt);
    int most_freq_class_idx_exp = 5;
    int most_freq_class_idx;
    get_most_freq_class_idx(classes_cnt, num_unique_classes, &most_freq_class_idx);

    assert_int_equal(most_freq_class_idx_exp, most_freq_class_idx);
}

int main(void) {
    printf("%s\n", __FILENAME__);
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_get_unique_classes),
        cmocka_unit_test(test_get_unique_class_idx),
        cmocka_unit_test(test_get_classes_cnt),
        cmocka_unit_test(test_most_freq_class_idx),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
