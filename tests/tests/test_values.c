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


static void test_get_unique_values(void **state) 
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

    
    int num_unique_values_exp[] = {12, 12, 9, 10, 11, 9, 11, 12, 11, 9};
    char attr_types[] = {NOMINAL_TYPE, NOMINAL_TYPE, NOMINAL_TYPE, NOMINAL_TYPE, NOMINAL_TYPE,
                        NOMINAL_TYPE, NOMINAL_TYPE, NOMINAL_TYPE, NOMINAL_TYPE, NOMINAL_TYPE};
    int num_unique_values[num_attr];
    float unique_values[num_attr][MAX_UNIQUE_VALUES];

    LargestIntegralType unique_values_exp[10][MAX_UNIQUE_VALUES] = {{0, 1, 3, 5, 6, 8, 10, 11, 13, 14, 15, 16},
                                                    {1, 2, 5, 6, 8, 9, 10, 11, 13, 14, 17, 19},
                                                    {1, 2, 3, 6, 7, 8, 9, 11, 18},
                                                    {0, 3, 4, 6, 7, 8, 9, 10, 12, 16},
                                                    {0, 1, 2, 4, 6, 7, 8, 10, 12, 14, 19},
                                                    {0, 2, 3, 9, 12, 13, 16, 18, 19},
                                                    {0, 2, 3, 4, 8, 9, 10, 11, 14, 16, 17},
                                                    {1, 2, 3, 5, 6, 7, 10, 13, 14, 15, 16, 17},
                                                    {0, 2, 4, 5, 6, 10, 14, 15, 17, 18, 19},
                                                    {0, 3, 4, 5, 7, 8, 11, 14, 19}};

    get_unique_values(num_attr, attr_types, num_inst, values, num_unique_values, unique_values);

    for (int i = 0; i < num_attr; i++)
    {
        assert_int_equal(num_unique_values_exp[i], num_unique_values[i]);
    }

    for (int i = 0; i < num_attr; i++)
    {
        LargestIntegralType unique_values_cast[num_unique_values_exp[i]];
        for (int j = 0; j <  num_unique_values_exp[i]; j++)
        {
            unique_values_cast[j] = unique_values_exp[i][j];
        }
        for (int j = 0; j <  num_unique_values_exp[i]; j++)
        {
            assert_in_set(unique_values_cast[j], unique_values_exp[i], num_unique_values_exp[i]);
            assert_in_set(unique_values_exp[i][j], unique_values_cast, num_unique_values_exp[i]);
        }
    }
}

static void test_get_unique_values_cnt(void **state) 
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

    int classes[15] = {2, 2, 4, 2, 2, 1, 5, 4, 2, 3, 2, 5, 1, 2, 3};
    int unique_classes[] = {1, 2, 3, 4, 5};
    int num_unique_classes = ARRAY_SIZE(unique_classes);

    int num_unique_values[] = {12, 12, 9, 10, 11, 9, 11, 12, 11, 9};
    char attr_types[] = {NOMINAL_TYPE, NOMINAL_TYPE, NOMINAL_TYPE, NOMINAL_TYPE, NOMINAL_TYPE,
                        NOMINAL_TYPE, NOMINAL_TYPE, NOMINAL_TYPE, NOMINAL_TYPE, NOMINAL_TYPE};

    float unique_values[10][MAX_UNIQUE_VALUES] = {{0, 1, 3, 5, 6, 8, 10, 11, 13, 14, 15, 16},
                                            {1, 2, 5, 6, 8, 9, 10, 11, 13, 14, 17, 19},
                                            {1, 2, 3, 6, 7, 8, 9, 11, 18},
                                            {0, 3, 4, 6, 7, 8, 9, 10, 12, 16},
                                            {0, 1, 2, 4, 6, 7, 8, 10, 12, 14, 19},
                                            {0, 2, 3, 9, 12, 13, 16, 18, 19},
                                            {0, 2, 3, 4, 8, 9, 10, 11, 14, 16, 17},
                                            {1, 2, 3, 5, 6, 7, 10, 13, 14, 15, 16, 17},
                                            {0, 2, 4, 5, 6, 10, 14, 15, 17, 18, 19},
                                            {0, 3, 4, 5, 7, 8, 11, 14, 19}};
                                                    

    int values_classes_cnt[num_attr][num_unique_classes][MAX_UNIQUE_VALUES];
    int unique_values_cnt[num_attr][MAX_UNIQUE_VALUES];

    int unique_values_cnt_exp[10][MAX_UNIQUE_VALUES] = {{1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 3, 1},
                                                {1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 1},
                                                {1, 2, 2, 1, 3, 2, 1, 2, 1},
                                                {1, 2, 4, 1, 1, 1, 1, 1, 2, 1},
                                                {2, 3, 1, 1, 1, 1, 1, 2, 1, 1, 1},
                                                {2, 3, 2, 3, 1, 1, 1, 1, 1},
                                                {1, 1, 2, 1, 1, 2, 2, 2, 1, 1, 1},
                                                {1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 2},
                                                {1, 1, 1, 1, 1, 2, 2, 1, 2, 2, 1},
                                                {1, 1, 1, 1, 4, 1, 3, 2, 1}};

    int values_classes_cnt_exp[10][5][MAX_UNIQUE_VALUES] = {{{0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0},
                                                    {0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 3, 1},
                                                    {0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0},
                                                    {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
                                                    {0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0}},

                                                    {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0},
                                                    {0, 1, 1, 0, 0, 1, 0, 1, 1, 2, 0, 0},
                                                    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
                                                    {0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
                                                    {1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}},

                                                    {{0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
                                                    {1, 1, 2, 0, 1, 1, 0, 1, 0, 0, 0, 0},
                                                    {0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0},
                                                    {0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0},
                                                    {0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0}},

                                                    {{1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                    {0, 0, 3, 0, 1, 1, 1, 0, 1, 0, 0, 0},
                                                    {0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                                                    {0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
                                                    {0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0}},

                                                    {{0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0},
                                                    {1, 1, 1, 0, 1, 0, 1, 2, 0, 0, 0, 0},
                                                    {0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
                                                    {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0}},

                                                    {{1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
                                                    {0, 2, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0},
                                                    {0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0},
                                                    {0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                    {1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}},

                                                    {{0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0},
                                                    {0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0},
                                                    {0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0},
                                                    {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0},
                                                    {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}},

                                                    {{0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0},
                                                    {0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1},
                                                    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
                                                    {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
                                                    {0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}},

                                                    {{0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0},
                                                    {0, 1, 0, 0, 1, 1, 0, 0, 2, 1, 1, 0},
                                                    {1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
                                                    {0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0},
                                                    {0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0}},

                                                    {{0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0},
                                                    {1, 0, 0, 0, 3, 1, 0, 2, 0, 0, 0, 0},
                                                    {0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
                                                    {0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0},
                                                    {0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0}}};

    get_unique_values_cnt(num_attr, attr_types, num_unique_classes, unique_classes,
    classes, num_unique_values, num_inst, MAX_UNIQUE_VALUES, values, 
    unique_values, values_classes_cnt, unique_values_cnt);

    for (int i = 0; i < num_attr; i++)
    {
        for (int j = 0; j < num_unique_values[i]; j++)
        {
            assert_int_equal(unique_values_cnt_exp[i][j], unique_values_cnt[i][j]);
        }        
    }

    for (int i = 0; i < num_attr; i++)
    {
        for (int j = 0; j < num_unique_classes; j++)
        {
            for (int k = 0; k < num_unique_values[i]; k++)
            {
                assert_int_equal(values_classes_cnt_exp[i][j][k], values_classes_cnt[i][j][k]);
            }    
        }    
    }
}

int main(void) {
    printf("%s\n", __FILENAME__);
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_get_unique_values),
        cmocka_unit_test(test_get_unique_values_cnt),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
