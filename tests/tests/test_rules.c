#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <string.h>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)


typedef uintmax_t LargestIntegralType;

#include "rules.h"
#include "values.h"

#include "mem_utils.h"

const float eps = 1e-5;

#define ARRAY_SIZE(arr)     (sizeof(arr) / sizeof((arr)[0]))


static void test_create_rule(void **state) 
{
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

    int unique_classes[] = {0, 1};
    int num_unique_classes = ARRAY_SIZE(unique_classes);

    int num_unique_values[num_attr];
    float unique_values[num_attr][MAX_UNIQUE_VALUES];

    get_unique_values(num_attr, attr_types, num_inst, values, num_unique_values, unique_values);
                                                    
    int values_classes_cnt[num_attr][num_unique_classes][MAX_UNIQUE_VALUES];
    int unique_values_cnt[num_attr][MAX_UNIQUE_VALUES];

    get_unique_values_cnt(num_attr, attr_types, num_unique_classes, unique_classes,
    classes, num_unique_values, num_inst, MAX_UNIQUE_VALUES, values, 
    unique_values, values_classes_cnt, unique_values_cnt);

    float test_values[] = {50, 1};

    rule_premise rp[num_attr];

    rule_premise rp_exp[][2] = {
        {{.numerical.min = 35, .numerical.max=50}, {.nominal.num = 1, .nominal.values = {1}}},
        {{.numerical.min = 45, .numerical.max=50}, {.nominal.num = 3, .nominal.values = {1, 2, 3}}},
        {{.numerical.min = 45, .numerical.max=50}, {.nominal.num = 2, .nominal.values = {1, 2}}},
    };
    int neigh_idxs[] = {0, 2, 4};
    for (int i = 0; i < ARRAY_SIZE(neigh_idxs); i++)
    {
        int idx = neigh_idxs[i];
        create_rule(test_values, values[idx], num_attr, rp,
            num_unique_classes, values_classes_cnt, 
            unique_values, unique_values_cnt,
            num_unique_values, attr_types);
        assert_int_equal(rp_exp[i][0].numerical.min, rp[0].numerical.min);
        assert_int_equal(rp_exp[i][0].numerical.max, rp[0].numerical.max);
        assert_int_equal(rp_exp[i][1].nominal.num, rp[1].nominal.num);

        LargestIntegralType nom_values_cast[rp[1].nominal.num];
        LargestIntegralType nom_values_exp_cast[rp[1].nominal.num];
        for (int j = 0; j < rp[1].nominal.num; j++)
        {
            nom_values_cast[j] = rp[1].nominal.values[j];
            nom_values_exp_cast[j] = rp_exp[i][1].nominal.values[j];
        }
        for (int j = 0; j < rp[1].nominal.num; j++)
        {
            assert_in_set(nom_values_cast[j], nom_values_exp_cast, rp[1].nominal.num);
            assert_in_set(nom_values_exp_cast[j], nom_values_cast, rp[1].nominal.num);
        }
    }
}

static void test_check_rule(void **state) 
{
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

    int unique_classes[] = {0, 1};
    int num_unique_classes = ARRAY_SIZE(unique_classes);

    int num_unique_values[num_attr];
    float unique_values[num_attr][MAX_UNIQUE_VALUES];

    get_unique_values(num_attr, attr_types, num_inst, values, num_unique_values, unique_values);
                                                    
    int values_classes_cnt[num_attr][num_unique_classes][MAX_UNIQUE_VALUES];
    int unique_values_cnt[num_attr][MAX_UNIQUE_VALUES];

    get_unique_values_cnt(num_attr, attr_types, num_unique_classes, unique_classes,
    classes, num_unique_values, num_inst, MAX_UNIQUE_VALUES, values, 
    unique_values, values_classes_cnt, unique_values_cnt);

    rule_premise rp[num_attr];

    float test_values[] = {50, 1};

    int neigh_idxs[] = {0, 2, 4};
    bool res_exp[][ARRAY_SIZE(neigh_idxs) - 1] = {{false, false},
                                                {false, true},
                                                {false, false},
                                                };
    for (int i = 0; i < ARRAY_SIZE(neigh_idxs); i++)
    {
        int idxi = neigh_idxs[i];
        create_rule(test_values, values[idxi], num_attr, rp,
            num_unique_classes, values_classes_cnt, 
            unique_values, unique_values_cnt,
            num_unique_values, attr_types);
        int cnt = 0;
        for (int j = 0; j < ARRAY_SIZE(neigh_idxs); j++)
        {
            if(i == j)
            {
                continue;
            }
            
            int idxj = neigh_idxs[j];
            bool res = check_rule(rp, num_attr, values[idxj], attr_types);
            assert_int_equal(res_exp[i][cnt], res);
            cnt++;
        }   
    }
}

static void test_is_consistent(void **state) 
{
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

    int unique_classes[] = {0, 1};
    int num_unique_classes = ARRAY_SIZE(unique_classes);

    int num_unique_values[num_attr];
    float unique_values[num_attr][MAX_UNIQUE_VALUES];

    get_unique_values(num_attr, attr_types, num_inst, values, num_unique_values, unique_values);
                                                    
    int values_classes_cnt[num_attr][num_unique_classes][MAX_UNIQUE_VALUES];
    int unique_values_cnt[num_attr][MAX_UNIQUE_VALUES];

    get_unique_values_cnt(num_attr, attr_types, num_unique_classes, unique_classes,
    classes, num_unique_values, num_inst, MAX_UNIQUE_VALUES, values, 
    unique_values, values_classes_cnt, unique_values_cnt);

    rule_premise rp[num_attr];

    float test_values[] = {50, 1};

    int k_neigh = 3;
    int k_neigh_indices_sorted[] = {0, 4, 2, 5, 6, 1, 3};
    bool res_exp[] = {true, true, false};

    for (int i = 0; i < k_neigh; i++)
    {
        create_rule(test_values, values[k_neigh_indices_sorted[i]], num_attr, rp,
            num_unique_classes, values_classes_cnt, 
            unique_values, unique_values_cnt,
            num_unique_values, attr_types);
        bool res = is_consistent(test_values, num_attr, values, 
            classes, i, k_neigh_indices_sorted, k_neigh, num_unique_classes,
            values_classes_cnt, unique_values, 
            unique_values_cnt, num_unique_values, attr_types);
        assert_int_equal(res_exp[i], res);
    }
}

int main(void) {
    printf("%s\n", __FILENAME__);
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_create_rule),
        cmocka_unit_test(test_check_rule),
        cmocka_unit_test(test_is_consistent),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
