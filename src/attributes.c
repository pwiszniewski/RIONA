#include "attributes.h"

void get_min_max_attributes(int num_attr, int num_inst, float values[][num_attr], 
    float *min_values, float *max_values)
{
    int i, j;
    for (i = 0; i < num_attr; i++)
    {
        float min = values[0][i];
        for (j = 1; j < num_inst; j++)
        {
            if (values[j][i] < min)
            {
                min = values[j][i];
            }
        }
        min_values[i] = min;
    }

    for (i = 0; i < num_attr; i++)
    {
        float max = values[0][i];
        for (j = 1; j < num_inst; j++)
        {
            if (values[j][i] > max)
            {
                max = values[j][i];
            }
        }
        max_values[i] = max;
    }
}

void get_num_numerical_and_nominal_attr(char *attr_types, int num_attr, int *num_numerical_attr,
        int *num_nominal_attr)
{
    *num_numerical_attr = 0;
    *num_nominal_attr = 0;
    for (int j = 0; j < num_attr; j++)
    {
        if(attr_types[j] == NUMERICAL_TYPE)
        {
            (*num_numerical_attr)++;
        }
        else if(attr_types[j] == NOMINAL_TYPE)
        {
            (*num_nominal_attr)++;
        }
    }
}