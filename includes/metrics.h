#pragma once

#include <math.h>

typedef struct {
  int tp, fp, tn, fn;
} tp_fp_tn_fn;

typedef struct {
  float acc, std_acc, balanced_acc, std_balanced_acc;
} metrics;



void calculate_confusion_matrix(int num_unique_classes, int conf_matrix[num_unique_classes][num_unique_classes], 
    int *unique_classes, int num_test, int *test_classes, int *test_classes_pred);

void calculate_tp_fp_tn_fn(tp_fp_tn_fn *tp_fp_tn_fn_arr, int *unique_classes, 
    int num_unique_classes, int num_test, int *test_classes, int *test_classes_pred);

void calculate_accuracy(float *acc, tp_fp_tn_fn *tp_fp_tn_fn_arr, int num_unique_classes,
    int num_test);

void calculate_balanced_accuracy(float *balanced_acc, tp_fp_tn_fn *tp_fp_tn_fn_arr, 
    int num_unique_classes);

void calculate_metrics(int num_test, int num_unique_classes, int *unique_classes, 
    int *test_classes, int *test_classes_pred, metrics *pred_metrics);

void calculate_metrics_for_each_class(int num_test, int num_unique_classes, int *unique_classes, 
    int *test_classes, int *test_classes_pred, metrics *pred_metrics);

void calculate_average_metrics(int num, metrics *pred_metrics, metrics *avg_metrics);