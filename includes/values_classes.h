#pragma once

#include <stdlib.h>

void shuffle_values_classes(int num_inst, int num_attr, float values[][num_attr], int *classes,
    int *indices_shuffled);

void make_copy_values_classes(int num_inst, int num_attr, float values[][num_attr], 
    int *classes, float values_copy[][num_attr], int *classes_copy);