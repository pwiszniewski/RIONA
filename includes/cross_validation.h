#pragma once

#include <string.h>
#include <stdlib.h>
#include "riona.h"

void split_train_test(int num_inst, int num_attr, float values[][num_attr], int *classes, 
    int num_k, int k, int k_fold_indices[][(num_inst / num_k) + 1], int *k_fold_cnt, 
    float train_values[][num_attr], int *train_classes, float test_values[][num_attr], 
    int *test_classes);

void prepare_k_fold_results(int num_inst, int num_test, int *test_indices, 
    stats_predict *test_sp, stats_predict *sp, int *test_classes_pred,
    int *classes_pred);

void create_k_fold(int num_inst, int max_k_fold_inst, int num_k, 
    int k_fold_indices[][max_k_fold_inst], int *k_fold_cnt);

void get_num_train_test(int k, int num_inst, int *k_fold_cnt, int *num_train, int *num_test);