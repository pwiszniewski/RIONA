#include "rules.h"

void create_rule(float *test_values, float *train_values, int num_attr, rule_premise *rp,
    int num_unique_classes, int values_classes_cnt[][num_unique_classes][MAX_UNIQUE_VALUES], 
    float unique_values[][MAX_UNIQUE_VALUES], int unique_values_cnt[][MAX_UNIQUE_VALUES],
    int *num_unique_values, char* attr_types)
{

    for (int i = 0; i < num_attr; i++)
    {
        if(attr_types[i] == NOMINAL_TYPE)
        {
            float value1 = test_values[i];
            int value1_idx = get_unique_value_idx(num_unique_values[i], unique_values[i], value1);
            float value2 = train_values[i];
            int value2_idx = get_unique_value_idx(num_unique_values[i], unique_values[i], value2);

            float svdm_thres;
            calculate_svdm(i, value1_idx, value2_idx, num_unique_classes,
                values_classes_cnt, unique_values_cnt, &svdm_thres);

            value2_idx = 0;
            rp[i].nominal.num = 0;
            while(value2_idx < num_unique_values[i])
            {
                float svdm;
                calculate_svdm(i, value1_idx, value2_idx, num_unique_classes,
                values_classes_cnt, unique_values_cnt, &svdm);
                if(svdm <= svdm_thres)
                {
                    rp[i].nominal.values[rp[i].nominal.num] = unique_values[i][value2_idx];
                    rp[i].nominal.num++;
                }
                value2_idx++;
            }
        }
        else if(attr_types[i] == NUMERICAL_TYPE)
        {
            if(test_values[i] > train_values[i])
            {
                rp[i].numerical.max = test_values[i];
                rp[i].numerical.min = train_values[i];
            }
            else
            {
                rp[i].numerical.max = train_values[i];
                rp[i].numerical.min = test_values[i];
            }
        }
        else 
        {
            printf("unknown attr type\n");
        }
    }
}

bool check_rule(rule_premise *rp, int num_attr, float *train_values, char *attr_types)
{
    for (int i = 0; i < num_attr; i++)
    {
        if(attr_types[i] == NOMINAL_TYPE)
        {
            int j = 0;
            while (j < rp[i].nominal.num)
            {
                if(rp[i].nominal.values[j] == train_values[i])
                {
                    break;
                }
                j++;
            }
            if(j == rp[i].nominal.num)
            {
                return false;
            }
        }
        else if(attr_types[i] == NUMERICAL_TYPE)
        {
            if(train_values[i] < rp[i].numerical.min || train_values[i] > rp[i].numerical.max)
            {
                return false;
            }
        }
        else 
        {
            printf("unknown attr type\n");
        }
    }

    return true;
}

bool is_consistent(float *test_values, int num_attr, float train_values[][num_attr], 
    int *train_classes, int train_idx, int *indices, int k, int num_unique_classes,
    int values_classes_cnt[][num_unique_classes][MAX_UNIQUE_VALUES], float unique_values[][MAX_UNIQUE_VALUES], 
    int unique_values_cnt[][MAX_UNIQUE_VALUES], int *num_unique_values, char *attr_types)
{
    rule_premise rp[num_attr];

    create_rule(test_values, train_values[indices[train_idx]], num_attr, rp,
        num_unique_classes, values_classes_cnt, unique_values, unique_values_cnt,
        num_unique_values, attr_types);

    int cls = train_classes[indices[train_idx]];

    bool ret = true;
    for (int i = 0; i < k; i++) 
    {
        int idx = indices[i];
        if(i == train_idx || train_classes[idx] == cls)
        {
            continue;
        }

        if(check_rule(rp, num_attr, train_values[idx], attr_types))
        {
            ret = false;
            break;
        }
    }


    return ret;
}