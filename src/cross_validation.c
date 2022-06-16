#include "cross_validation.h"

void split_train_test(int num_inst, int num_attr, float values[][num_attr], int *classes, 
    int num_k, int k, int k_fold_indices[][(num_inst / num_k) + 1], int *k_fold_cnt, 
    float train_values[][num_attr], int *train_classes, float test_values[][num_attr], 
    int *test_classes)
{
    int num_train = 0;
    int num_test = 0;
    for (int i = 0; i < num_k; i++)
    {
        if(i != k)
        {
            for (int j = 0; j < k_fold_cnt[i]; j++)
            {
                int idx = k_fold_indices[i][j];
                memcpy(train_values[num_train], values[idx], sizeof(**values) * num_attr);
                train_classes[num_train] = classes[idx];
                num_train++;
            }
        }
        else
        {
            for (int j = 0; j < k_fold_cnt[i]; j++)
            {
                int idx = k_fold_indices[i][j];
                memcpy(test_values[num_test], values[idx], sizeof(**values) * num_attr);
                test_classes[num_test] = classes[idx];
                num_test++;
            }
        }
    }
}

void prepare_k_fold_results(int num_inst, int num_test, int *test_indices, 
    stats_predict *test_sp, stats_predict *sp, int *test_classes_pred,
    int *classes_pred)
{
    for (int i = 0; i < num_test; i++)
    {
        int idx = test_indices[i];
        sp[idx].k_plus = test_sp[i].k_plus;
        sp[idx].k_plus_promising = test_sp[i].k_plus_promising;
        classes_pred[idx] = test_classes_pred[i];
    }
}

static void shuffle_array(int *arr, int n)
{
    for (int i = 0; i < n - 1; i++) 
    {
        int j = i + rand() / (RAND_MAX / (n - i) + 1);
        int temp = arr[j];
        arr[j] = arr[i];
        arr[i] = temp;
    }
}

void create_k_fold(int num_inst, int max_k_fold_inst, int num_k, 
    int k_fold_indices[][max_k_fold_inst], int *k_fold_cnt)
{
    for(int i = 0; i < num_k; i++)
    {
        k_fold_cnt[i] = 0;
    }

    int indices[num_inst];

    for(int i = 0; i < num_inst; i++)
    {
        indices[i] = i;
    }

    shuffle_array(indices, num_inst);

    for(int i = 0; i < num_inst; i++)
    {
        int k = i % num_k;
        k_fold_indices[k][k_fold_cnt[k]] = indices[i];
        k_fold_cnt[k]++;
    }
}

void get_num_train_test(int k, int num_inst, int *k_fold_cnt, int *num_train, int *num_test)
{
    *num_test = k_fold_cnt[k];
    *num_train = num_inst - *num_test;
}