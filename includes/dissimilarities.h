#pragma once

#include "definitions.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "values.h"

#ifdef VECT
#include <cblas.h>
#endif

#ifdef CUDA
#include "my_cuda.h"
#endif

void calculate_svdm(int attr_idx, int value1_idx, int value2_idx, int num_unique_classes,
    int values_classes_cnt[][num_unique_classes][MAX_UNIQUE_VALUES], 
    int unique_values_cnt[][MAX_UNIQUE_VALUES], float *svdm);
void calculate_manhattan(float value1, float value2, float min, float max, float *manhattan);
void calculate_dissimilairty(float *values1, float *values2, int num_attr, char *attr_types, 
    float *min_values, float *max_values, int num_unique_classes, 
    int values_classes_cnt[][num_unique_classes][MAX_UNIQUE_VALUES], 
    float unique_values[][MAX_UNIQUE_VALUES], int unique_values_cnt[][MAX_UNIQUE_VALUES], 
    int *num_unique_values, float *dissim);

void calculate_dissimilairty_vect(float *values1, int num_values2, float *values2, int num_attr, 
    char *attr_types, float *min_values, float *max_values, int num_unique_classes, 
    int values_classes_cnt[][num_unique_classes][MAX_UNIQUE_VALUES], 
    float unique_values[][MAX_UNIQUE_VALUES], int unique_values_cnt[][MAX_UNIQUE_VALUES], 
    int *num_unique_values, float *dissim);

#ifdef CUDA
void calculate_dissimilairty_cuda(int id, float *values1, int num_values2, float *values2, int num_attr, char *attr_types, 
    float *min_values, float *max_values, int num_unique_classes, 
    int values_classes_cnt[][num_unique_classes][MAX_UNIQUE_VALUES], 
    float unique_values[][MAX_UNIQUE_VALUES], int unique_values_cnt[][MAX_UNIQUE_VALUES], 
    int *num_unique_values, float *dissim);
#endif