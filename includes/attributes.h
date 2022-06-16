#pragma once

#include "definitions.h"

void get_min_max_attributes(int num_attr, int num_inst, float values[][num_attr], 
    float *min_values, float *max_values);
void get_num_numerical_and_nominal_attr(char *attr_types, int num_attr, int *num_numerical_attr,
        int *num_nominal_attr);