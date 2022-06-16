#include "values.h"

void get_unique_values(int num_attr, char *attr_types, int num_inst, 
    float values[num_inst][num_attr], int *num_unique_values, float unique_values[][MAX_UNIQUE_VALUES])
{
    int k;
    for (int i = 0; i < num_attr; i++) 
    {
        num_unique_values[i] = 0;
        if(attr_types[i] == NOMINAL_TYPE)
        {
            for (int j = 0; j < num_inst; j++) 
            {
                for (k = 0; k < j; k++)
                {
                    if (values[j][i] == values[k][i])
                    {
                        break;
                    }
                }
                    
                if (j == k)
                {
                    unique_values[i][num_unique_values[i]] = values[j][i];
                    num_unique_values[i]++;
                }
            }
        }
    }
}

int get_unique_value_idx(int num_unique_values, float *unique_values, int target)
{
    int idx = 0;
    while(idx < num_unique_values)
    {
        if(target == unique_values[idx])
        {
            break;
        }
        idx++;
    }
    if(idx == num_unique_values) 
    {
        idx = -1;
    }
    return idx;
}

bool get_unique_values_cnt(int num_attr, char *attr_types, int num_unique_classes, 
    int *unique_classes, int *classes, int *num_unique_values, int num_inst, int max_values, 
    float values[][num_attr], float unique_values[][max_values], 
    int values_classes_cnt[][num_unique_classes][max_values], int unique_values_cnt[][max_values])
{
    for (int i = 0; i < num_attr; i++) 
    {
        for (int j = 0; j < num_unique_classes; j++) 
        {
            for (int k = 0; k < max_values; k++)
            {
                values_classes_cnt[i][j][k] = 0;
            }
        }
    }

    for (int i = 0; i < num_attr; i++) 
    {
        for (int j = 0; j < num_unique_values[i]; j++) 
        {
            for (int k = 0; k < max_values; k++)
            {
                unique_values_cnt[i][j] = 0;
            }
        }
    }

    for (int i = 0; i < num_attr; i++) 
    {
        if(attr_types[i] == NOMINAL_TYPE)
        {
            for (int j = 0; j < num_inst; j++) 
            {
                int k = get_unique_value_idx(num_unique_values[i], unique_values[i], values[j][i]);
                int m = get_unique_class_idx(num_unique_classes, unique_classes, classes[j]);

                values_classes_cnt[i][m][k]++;
                unique_values_cnt[i][k]++;
            }
        }
    }
    return true;
}