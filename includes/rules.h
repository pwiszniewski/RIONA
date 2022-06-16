#pragma once

#include <stdbool.h>
#include "mem_utils.h"
#include "definitions.h"
#include "dissimilarities.h"
#include "values.h"

typedef struct rule_premise_numerical {
   float min;
   float max;
} rule_premise_numerical;

typedef struct rule_premise_nominal {
    int num;
    float values[MAX_UNIQUE_VALUES];
} rule_premise_nominal;

typedef union rule_premise{
    rule_premise_numerical numerical;
    rule_premise_nominal nominal;
} rule_premise;


void create_rule(float *test_values, float *train_values, int num_attr, rule_premise *rp,
    int num_unique_classes, int values_classes_cnt[][num_unique_classes][MAX_UNIQUE_VALUES], 
    float unique_values[][MAX_UNIQUE_VALUES], int unique_values_cnt[][MAX_UNIQUE_VALUES],
    int *num_unique_values, char* attr_types);
bool check_rule(rule_premise *rp, int num_attr, float *train_values, char *attr_types);
bool is_consistent(float *test_values, int num_attr, float train_values[][num_attr], 
    int *train_classes, int train_idx, int *indices, int k, int num_unique_classes,
    int values_classes_cnt[][num_unique_classes][MAX_UNIQUE_VALUES], float unique_values[][MAX_UNIQUE_VALUES], 
    int unique_values_cnt[][MAX_UNIQUE_VALUES], int *num_unique_values,
    char *attr_types);