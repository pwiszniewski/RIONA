#include "dissimilarities.h"

#include <cblas.h>

#include "my_cuda.h"

void calculate_svdm(int attr_idx, int value1_idx, int value2_idx, int num_unique_classes,
    int values_classes_cnt[][num_unique_classes][MAX_UNIQUE_VALUES], 
    int unique_values_cnt[][MAX_UNIQUE_VALUES], float *svdm)
{
    *svdm = 0;

    for (int i = 0; i < num_unique_classes; i++) 
    {
        float value1_class_cnt = values_classes_cnt[attr_idx][i][value1_idx];
        float value1_cnt = unique_values_cnt[attr_idx][value1_idx];
        float value2_class_cnt = values_classes_cnt[attr_idx][i][value2_idx];
        float value2_cnt = unique_values_cnt[attr_idx][value2_idx];
        float part = fabs(value1_class_cnt / value1_cnt - value2_class_cnt / value2_cnt);

        *svdm += part;
    }
}

void calculate_manhattan(float value1, float value2, float min, float max, float *manhattan)
{
    *manhattan = abs(value1 - value2) / (max - min);
    // *manhattan = abs(value1 - value2) ;
}

void calculate_dissimilairty(float *values1, float *values2, int num_attr, char *attr_types, 
    float *min_values, float *max_values, int num_unique_classes, 
    int values_classes_cnt[][num_unique_classes][MAX_UNIQUE_VALUES], 
    float unique_values[][MAX_UNIQUE_VALUES], int unique_values_cnt[][MAX_UNIQUE_VALUES], 
    int *num_unique_values, float *dissim)
{
    *dissim = 0;

    for (int i = 0; i < num_attr; i++) 
    {
        if(attr_types[i] == NOMINAL_TYPE)
        {
            float svdm;
            float value1 = values1[i];
            int value1_idx = get_unique_value_idx(num_unique_values[i], unique_values[i], value1);
            float value2 = values2[i];
            int value2_idx = get_unique_value_idx(num_unique_values[i], unique_values[i], value2);

            calculate_svdm(i, value1_idx, value2_idx, num_unique_classes,
                values_classes_cnt, unique_values_cnt, &svdm);
            *dissim += svdm;
        }
        else if(attr_types[i] == NUMERICAL_TYPE)
        {
            float manhattan;
            calculate_manhattan(values1[i], values2[i], min_values[i], max_values[i], &manhattan);
            *dissim += manhattan;
        }
        else 
        {
            printf("unknown attr type\n");
        }
    }
}

void calculate_manhattan_vect(float value1, int num_values2, int idx_value2, float *value2, 
int num_attr,float min, 
float max, float *dissim)
{
    float temp ;
    for (int i = 0; i < num_values2; i++) 
    {
        temp = value2[idx_value2*num_values2 + i] - value1;
        temp = temp > 0 ? temp : -temp;
        dissim[i] += temp / (max - min);
    }
}


void calculate_dissimilairty_vect(float *values1, int num_values2, float *values2, int num_attr, char *attr_types, 
    float *min_values, float *max_values, int num_unique_classes, 
    int values_classes_cnt[][num_unique_classes][MAX_UNIQUE_VALUES], 
    float unique_values[][MAX_UNIQUE_VALUES], int unique_values_cnt[][MAX_UNIQUE_VALUES], 
    int *num_unique_values, float *dissim)
{
    for (int i = 0; i < num_attr; i++) 
    {
        if(attr_types[i] == NOMINAL_TYPE)
        {
            for (int j = 0; j < num_values2; j++)
            {
                float svdm;
                float value1 = values1[i];
                int value1_idx = get_unique_value_idx(num_unique_values[i], unique_values[i], value1);
                float value2 = values2[i*num_values2 + j];
                int value2_idx = get_unique_value_idx(num_unique_values[i], unique_values[i], value2);

                calculate_svdm(i, value1_idx, value2_idx, num_unique_classes,
                    values_classes_cnt, unique_values_cnt, &svdm);
                dissim[j] += svdm;
            }
        }
        else if(attr_types[i] == NUMERICAL_TYPE)
        {
            calculate_manhattan_vect(values1[i], num_values2, i, values2, num_attr, 
            min_values[i], max_values[i], dissim);
        }
        else 
        {
            printf("unknown attr type\n");
        }
    }
}

#ifdef CUDA
void calculate_dissimilairty_cuda(int id, float *values1, int num_values2, float *values2, int num_attr, char *attr_types, 
    float *min_values, float *max_values, int num_unique_classes, 
    int values_classes_cnt[][num_unique_classes][MAX_UNIQUE_VALUES], 
    float unique_values[][MAX_UNIQUE_VALUES], int unique_values_cnt[][MAX_UNIQUE_VALUES], 
    int *num_unique_values, float *dissim)
{
    for (int i = 0; i < num_attr; i++) 
    {
        if(attr_types[i] == NOMINAL_TYPE)
        {

        }
        else if(attr_types[i] == NUMERICAL_TYPE)
        {
            calculate_manhattan_cuda(id, values1[i], num_values2, i, values2, num_attr, 
            min_values[i], max_values[i], dissim);
        }
        else 
        {
            printf("unknown attr type\n");
        }
    }
}
#endif
