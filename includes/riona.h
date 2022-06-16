#pragma once

#include "classes.h"
#include "values.h"
#include "mem_utils.h"
#include "definitions.h"
#include "dissimilarities.h"
#include "attributes.h"
#include "rules.h"
#include "timing.h"
#include <string.h>
#include <omp.h>

typedef struct stats_find_optk {
    int *num_k_plus;
    int *num_k_plus_promising; 
    int *k_plus_1; 
    int *k_plus_promising_1; 
    int *k_plus_3; 
    int *k_plus_promising_3; 
} stats_find_optk;

typedef struct stats_predict {
    int k_plus;
    int k_plus_promising; 
} stats_predict;

void find_optimal_k(int k_max, int num_train, char *attr_types, int num_attr, 
    float values[][num_attr], int *classes, int num_unique_classes, int *unique_classes, 
    int *classes_cnt, float *min_values, float *max_values, bool is_norm, int *k_opt,
    int classes_pred[][k_max], stats_find_optk *sfoptk, timing_optk *toptk);

void predict(int num_train, int num_test, int num_attr, int k_neigh, float train_values[num_train][num_attr], 
    int *train_classes, float test_values[num_test][num_attr], char *attr_types, 
    int num_unique_classes, int *unique_classes, int *test_classes_pred, bool is_norm,
    stats_predict *sp, timing_predict *tp);

void sort_indices(int num, float *dissim, int *indices);

void get_classification_vector(float *trn_values, int num_attr, float train_values[][num_attr], 
    int *train_classes, int *indices, int k_plus, int k_max, int num_unique_classes, 
    int *unique_classes, int *classes_cnt, int values_classes_cnt[][num_unique_classes][MAX_UNIQUE_VALUES], 
    float unique_values[][MAX_UNIQUE_VALUES], int unique_values_cnt[][MAX_UNIQUE_VALUES], int *num_unique_values, 
    char *attr_types, int most_freq_class_idx_train, float *dec_strength, bool is_norm, 
    int *num_promising, int *decisions, double *time_verif_rule);