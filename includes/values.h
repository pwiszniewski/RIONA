#pragma once

#include "definitions.h"
#include "classes.h"
#include <stdbool.h>
#include <stdio.h>

void get_unique_values(int num_attr, char *attr_types, int num_inst, 
    float values[num_inst][num_attr], int *num_unique_values, float unique_values[][MAX_UNIQUE_VALUES]);

int get_unique_value_idx(int num_unique_values, float *unique_values, int target);

bool get_unique_values_cnt(int num_attr, char *attr_types, int num_unique_classes, 
    int *unique_classes, int *classes, int *num_unique_values, int num_inst, int max_values, 
    float values[][num_attr], float unique_values[][max_values], 
    int values_classes_cnt[][num_unique_classes][max_values], int unique_values_cnt[][max_values]);