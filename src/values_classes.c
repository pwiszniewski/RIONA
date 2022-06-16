#include "values_classes.h"

void shuffle_values_classes(int num_inst, int num_attr, float values[][num_attr], int *classes,
    int *indices_shuffled)
{
    for (int i = 0; i < num_inst; i++)
    {
        indices_shuffled[i] = i;
    }
    for (int i = 0; i < num_inst - 1; i++) 
    {
        int j = i + rand() / (RAND_MAX / (num_inst - i) + 1);
        int temp = classes[j];
        classes[j] = classes[i];
        classes[i] = temp;
        int temp_idx = indices_shuffled[j];
        indices_shuffled[j] = indices_shuffled[i];
        indices_shuffled[i] = temp_idx;
        for (int a = 0; a < num_attr; a++) 
        {
            float temp = values[j][a];
            values[j][a] = values[i][a];
            values[i][a] = temp;
        }
    }
}

void make_copy_values_classes(int num_inst, int num_attr, float values[][num_attr], 
    int *classes, float values_copy[][num_attr], int *classes_copy)
{
    for (int i = 0; i < num_inst; i++) 
    {
        classes_copy[i] = classes[i];
        for (int a = 0; a < num_attr; a++) 
        {
            values_copy[i][a] = values[i][a];
        }
    }
}