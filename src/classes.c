#include "classes.h"

void get_unique_classes(int num_inst, int *classes, int *unique_classes, 
    int *num_unique_classes)
{
    *num_unique_classes = 0;
    int i, j;
    for (i = 0; i < num_inst; i++) 
    {
        for (j = 0; j < i; j++)
        {
            if (classes[i] == classes[j])
            {
                break;
            }
        }
            
        if (i == j)
        {
            unique_classes[*num_unique_classes] = classes[i];
            (*num_unique_classes)++;
        }
    }
}

int get_unique_class_idx(int num_unique_classes, int *unique_classes, int target)
{
    int idx = 0;
    while(idx < num_unique_classes)
    {
        if(target == unique_classes[idx])
        {
            break;
        }
        idx++;
    }
    if(idx == num_unique_classes) 
    {
        idx = -1;
    }
    return idx;
}

void get_classes_cnt(int num_unique_classes, int *unique_classes, int num_inst, 
int *classes, int *classes_cnt)
{
    int i;
    for (i = 0; i < num_unique_classes; i++)
    {
        classes_cnt[i] = 0;
    }  
    for (i = 0; i < num_inst; i++)
    {
        int idx = get_unique_class_idx(num_unique_classes, unique_classes, classes[i]);
        classes_cnt[idx]++;
    }  
}

void get_most_freq_class_idx(int *classes_cnt, int num_unique_classes, int *most_freq_class_idx)
{
    *most_freq_class_idx = 0;
    int i;
    for (i = 1; i < num_unique_classes; i++)
    {
        if(classes_cnt[i] > classes_cnt[*most_freq_class_idx])
        {
            *most_freq_class_idx = i;
        }
    } 
}





