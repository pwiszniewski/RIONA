#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <string.h>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)


typedef uintmax_t LargestIntegralType;

#include "riona.h"

#include "mem_utils.h"

const float eps = 1e-5;

#define ARRAY_SIZE(arr)     (sizeof(arr) / sizeof((arr)[0]))


void test_sort_indices(void **unused)
{
    float dissim[] = {0.3, 0.1, 0.5, 0.8, 0.2, 0.4, 0.6, 0.7, 0.9};
    int indices[] = {2, 3, 4, 5, 1, 6, 7, 8, 0};

    sort_indices(9, dissim, indices);

    assert_int_equal(indices[0], 1);
    assert_int_equal(indices[1], 4);
    assert_int_equal(indices[2], 0);
    assert_int_equal(indices[3], 5);
    assert_int_equal(indices[4], 2);
    assert_int_equal(indices[5], 6);
    assert_int_equal(indices[6], 7);
    assert_int_equal(indices[7], 3);
    assert_int_equal(indices[8], 8);
}


void test_get_classification_vector()
{
    int num_attr = 4;
    char attr_types[4] = {NOMINAL_TYPE, NUMERICAL_TYPE, NOMINAL_TYPE, NUMERICAL_TYPE};
    float train_values[][4] = {{1, 1, 1, 1}, {1, 2, 1, 1}, {1, 1, 2, 1},
    {2, 2, 1, 1}, {2, 1, 2, 1}, {2, 2, 2, 1}, {2, 1, 1, 2}, {2, 2, 1, 2},
    {2, 1, 2, 2}, {2, 2, 2, 2}, {3, 1, 1, 1}, {3, 2, 1, 1}, {3, 1, 2, 1},
    {3, 2, 2, 1}, {3, 1, 1, 2}, {3, 2, 1, 2}, {3, 1, 2, 2}, {3, 2, 2, 2},
    {1, 1, 1, 3}, {1, 2, 1, 3}, {1, 1, 2, 3}, {2, 2, 1, 3}, {2, 1, 2, 3},
    {2, 2, 2, 3}, {2, 1, 1, 3}, {2, 2, 1, 3}, {2, 1, 2, 3}, {2, 2, 2, 3},
    {3, 1, 1, 3}, {3, 2, 1, 3}, {3, 1, 2, 3}, {3, 2, 2, 3}, {3, 1, 1, 3},
    {3, 2, 1, 3}, {3, 1, 2, 3}, {3, 2, 2, 3}, {1, 1, 1, 4}, {1, 2, 1, 4},
    {1, 1, 2, 4}, {2, 2, 1, 4}, {2, 1, 2, 4}, {2, 2, 2, 4}, {2, 1, 1, 4},
    {2, 2, 1, 4}, {2, 1, 2, 4}, {2, 2, 2, 4}, {3, 1, 1, 4}, {3, 2, 1, 4},
    {3, 1, 2, 4}, {3, 2, 2, 4}, {3, 1, 1, 4}, {3, 2, 1, 4}, {3, 1, 2, 4},
    {3, 2, 2, 4}, {1, 1, 1, 5}, {1, 2, 1, 5}, {1, 1, 2, 5}, {2, 2, 1, 5},
    {2, 1, 2, 5}, {2, 2, 2, 5}, {2, 1, 1, 5}, {2, 2, 1, 5}, {2, 1, 2, 5},
    {2, 2, 2, 5}, {3, 1, 1, 5}, {3, 2, 1, 5}, {3, 1, 2, 5}, {3, 2, 2, 5},
    {3, 1, 1, 5}, {3, 2, 1, 5}, {3, 1, 2, 5}, {3, 2, 2, 5}};

    int train_classes[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5,
        5, 5, 5, 5, 5};

    int k_max = 3;
    int k_plus = 4;
    int num_unique_classes = 5;
    int unique_classes[] = {1, 2, 3, 4, 5};
    int num_unique_values[] = {5, 5, 5, 5};
    int classes_cnt[] = {10, 10, 10, 10};
    float unique_values[][MAX_UNIQUE_VALUES] = {{1, 2, 3, 4, 5}, {1, 2, 3, 4, 5}, {1, 2, 3, 4, 5}, {1, 2, 3, 4, 5}};
    int values_classes_cnt[][5][MAX_UNIQUE_VALUES] = {{{2, 2, 2, 2, 2}, {2, 2, 2, 2, 2}, {2, 2, 2, 2, 2}, {2, 2, 2, 2, 2}, {2, 2, 2, 2, 2}}, 
    {{2, 2, 2, 2, 2}, {2, 2, 2, 2, 2}, {2, 2, 2, 2, 2}, {2, 2, 2, 2, 2}, {2, 2, 2, 2, 2}}, 
    {{2, 2, 2, 2, 2}, {2, 2, 2, 2, 2}, {2, 2, 2, 2, 2}, {2, 2, 2, 2, 2}, {2, 2, 2, 2, 2}}, 
    {{2, 2, 2, 2, 2}, {2, 2, 2, 2, 2}, {2, 2, 2, 2, 2}, {2, 2, 2, 2, 2}, {2, 2, 2, 2, 2}}};
    int unique_values_cnt[][MAX_UNIQUE_VALUES] = {{2, 2, 2, 2, 2}, {2, 2, 2, 2, 2}, {2, 2, 2, 2, 2}, {2, 2, 2, 2, 2}};
    int most_freq_class_idx_train = 0;
    float dec_strength[3];
    int decisions[3];
    int k_opt = 0;

    int indices_org_sorted[50] = {28, 9, 25, 49, 35, 29, 45, 22, 11, 21, 18, 6, 48, 15, 3, 12, 
        1, 23, 38, 42, 8, 30, 16, 14, 46, 26, 13, 5, 10, 2, 27, 19, 41, 44, 32, 4, 47, 31, 43, 
        17, 40, 33, 24, 37, 20, 34, 36, 39, 7, 50};
    
    float trn_values[] = {3, 2, 2, 5};

    int num_promising[k_max];
    double time_verif_rule;
    get_classification_vector(trn_values, 4, train_values, train_classes, indices_org_sorted, 
        k_plus, k_max, 5, unique_classes, classes_cnt, values_classes_cnt, unique_values, 
        unique_values_cnt, num_unique_values, attr_types, most_freq_class_idx_train, 
        dec_strength, false, num_promising, decisions, &time_verif_rule);
    
    
    assert_non_null(decisions);
    assert_int_equal(decisions[0], 1);
    assert_int_equal(decisions[1], 1);
    assert_int_equal(decisions[2], 1);
}

void test_predict(void **state)
{
    int num_train = 5;
    int num_test = 5;
    int num_attr = 3;
    int k_neigh = 3;
    int train_classes[5] = {0, 0, 1, 1, 2};
    float train_values[5][3] = {{0, 1, 2}, {0, 2, 1}, {1, 0, 2}, {2, 0, 1}, {1, 2, 0}};
    float test_values[5][3] = {{0, 1, 2}, {1, 0, 2}, {1, 2, 0}, {2, 1, 0}, {0, 2, 1}};
    char attr_types[3] = {NOMINAL_TYPE, NUMERICAL_TYPE, NOMINAL_TYPE};
    bool is_norm = false;
    int test_classes_pred[num_test];

    stats_predict sp[num_test];
    timing_predict tp;

    predict(num_train, num_test, num_attr, k_neigh, train_values, train_classes, test_values, 
        attr_types, 3, (int[3]){0, 1, 2}, test_classes_pred, is_norm, sp, &tp);

    assert_int_equal(test_classes_pred[0], 0);
    assert_int_equal(test_classes_pred[1], 1);
    assert_int_equal(test_classes_pred[2], 2);
    assert_int_equal(test_classes_pred[3], 1);
    assert_int_equal(test_classes_pred[4], 0);
}



int main(void) {
    printf("%s\n", __FILENAME__);
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_sort_indices),
        cmocka_unit_test(test_get_classification_vector),
        cmocka_unit_test(test_predict),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
