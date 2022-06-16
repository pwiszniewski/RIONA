#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <string.h>
#include <stdio.h>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#include "metrics.h"


const float eps = 1e-10;

#define ARRAY_SIZE(arr)     (sizeof(arr) / sizeof((arr)[0]))

static void test_conf_mat(void **state) {

    const int num_unique_classes = 3;
    int unique_classes[] = {0, 1, 2};
    int conf_matrix[num_unique_classes][num_unique_classes];

    int num_test = 6;
    int test_classes[] = {2, 0, 2, 2, 0, 1};
    int test_classes_pred[] = {0, 0, 2, 2, 0, 2};

	calculate_confusion_matrix(num_unique_classes, conf_matrix, unique_classes, 
    num_test, test_classes, test_classes_pred);
    int conf_matrix_expected[3][3] = {{2, 0, 0},
                                    {0, 0, 1},
                                    {1, 0, 2}};
    for (int i = 0; i < num_unique_classes; i++)
    {
        for (int j = 0; j < num_unique_classes; j++)
        {
            assert_int_equal(conf_matrix_expected[i][j], conf_matrix[i][j]);
        }
    }
}


static void test_tp_fp_tn_fn(void **state) {

    const int num_unique_classes = 3;
    int unique_classes[] = {0, 1, 2};
    tp_fp_tn_fn tp_fp_tn_fn_arr[num_unique_classes];

    int num_test = 6;
    int test_classes[] = {2, 0, 2, 2, 0, 1};
    int test_classes_pred[] = {0, 0, 2, 2, 0, 2};
    tp_fp_tn_fn tp_fp_tn_fn_arr_expected[3] = {{2, 1, 3, 0},
                                      {0, 0, 5, 1},
                                      {2, 1, 2, 1}};

    calculate_tp_fp_tn_fn(tp_fp_tn_fn_arr, unique_classes, 
    num_unique_classes, num_test, test_classes, test_classes_pred);

    for (int i = 0; i < num_unique_classes; i++)
    {
        assert_int_equal(tp_fp_tn_fn_arr[i].tp, tp_fp_tn_fn_arr_expected[i].tp);
        assert_int_equal(tp_fp_tn_fn_arr[i].fp, tp_fp_tn_fn_arr_expected[i].fp);
        assert_int_equal(tp_fp_tn_fn_arr[i].tn, tp_fp_tn_fn_arr_expected[i].tn);
        assert_int_equal(tp_fp_tn_fn_arr[i].fn, tp_fp_tn_fn_arr_expected[i].fn);
    }

}

static void test_acc(void **state) {

    const int num_unique_classes = 4;
    int unique_classes[] = {0, 5, 2, 4};
    tp_fp_tn_fn tp_fp_tn_fn_arr[num_unique_classes];

    int num_test = 4;
    int test_classes[] = {0, 5, 2, 4};
    int test_classes_pred[] = {0, 1, 2, 3};

    calculate_tp_fp_tn_fn(tp_fp_tn_fn_arr, unique_classes, 
        num_unique_classes, num_test, test_classes, test_classes_pred);
    float acc;
    calculate_accuracy(&acc, tp_fp_tn_fn_arr, num_unique_classes, num_test);
    
    assert_float_equal(acc, 50, eps);

}


static void test_balanced_acc(void **state) {

    int unique_classes[] = {0, 1};
    const int num_unique_classes = ARRAY_SIZE(unique_classes);

    tp_fp_tn_fn tp_fp_tn_fn_arr[num_unique_classes];

    int test_classes[] = {0, 1, 0, 0, 1, 0};
    int test_classes_pred[] = {0, 1, 0, 0, 0, 1};
    int num_test = ARRAY_SIZE(test_classes);

    calculate_tp_fp_tn_fn(tp_fp_tn_fn_arr, unique_classes, 
        num_unique_classes, num_test, test_classes, test_classes_pred);
    float bacc;
    calculate_balanced_accuracy(&bacc, tp_fp_tn_fn_arr, num_unique_classes);
    
    assert_float_equal(bacc, 62.5, eps);

}

static void test_metrics(void **state) {

    int unique_classes[] = {0, 1, 2, 3, 4, 6, 7};
    const int num_unique_classes = ARRAY_SIZE(unique_classes);

    tp_fp_tn_fn tp_fp_tn_fn_arr[num_unique_classes];

    int test_classes[] = {0, 3, 1, 2, 4, 6, 7, 1};
    int test_classes_pred[] = {1, 2, 1, 0, 2, 7, 7, 1};
    int num_test = ARRAY_SIZE(test_classes);

    calculate_tp_fp_tn_fn(tp_fp_tn_fn_arr, unique_classes, 
        num_unique_classes, num_test, test_classes, test_classes_pred);
    metrics met;

    calculate_metrics(num_test, num_unique_classes, unique_classes, 
        test_classes, test_classes_pred, &met);
    
    assert_float_equal(met.acc, 37.5, eps);
    assert_float_equal(met.balanced_acc, 28.57142857142857, eps);

}

static void test_avg_metrics(void **state) {

    float acc_avg_exp = 63.54582715111584;
    float acc_std_exp = 14.597612978349659;
    float bacc_avg_exp = 38.671688341167446;
    float bacc_std_exp = 26.896429964716912;

    metrics met[] = {{.acc=75.29398630145752, .balanced_acc=70.13771967682615},
                    {.acc=45.47345222068551, .balanced_acc=74.49706339249552},
                    {.acc=52.24547726663891, .balanced_acc=11.866732162819826},
                    {.acc=88.71293768913097, .balanced_acc=13.625942730799355},
                    {.acc=56.28274307728771, .balanced_acc=13.602902173976105},
                    {.acc=63.2663663514944, .balanced_acc=48.29976991008771},
    };
    metrics avg_met;

    int num = ARRAY_SIZE(met);

    calculate_average_metrics(num, met, &avg_met);
    
    assert_float_equal(avg_met.acc, acc_avg_exp, eps);
    assert_float_equal(avg_met.std_acc, acc_std_exp, eps);
    assert_float_equal(avg_met.balanced_acc, bacc_avg_exp, eps);
    assert_float_equal(avg_met.std_balanced_acc, bacc_std_exp, eps);
}

int main(void) {
    printf("%s\n", __FILENAME__);
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_conf_mat),
        cmocka_unit_test(test_tp_fp_tn_fn),
        cmocka_unit_test(test_acc),
        cmocka_unit_test(test_balanced_acc),
        cmocka_unit_test(test_metrics),
        cmocka_unit_test(test_avg_metrics),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
