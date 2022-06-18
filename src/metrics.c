#include "metrics.h"
#include <omp.h>

void calculate_confusion_matrix(int num_unique_classes, int conf_matrix[num_unique_classes][num_unique_classes], 
    int *unique_classes, int num_test, int *test_classes, int *test_classes_pred)
{
    for (int i = 0; i < num_unique_classes; i++)
    {
        for (int j = 0; j < num_unique_classes; j++)
        {
            conf_matrix[i][j] = 0;
        }
    }

    for (int i = 0; i < num_test; i++)
    {
        int idx_true = 0;
        while(idx_true < num_unique_classes)
        {
            if(test_classes[i] == unique_classes[idx_true])
            {
                break;
            }
            idx_true++;
        }
        int idx_pred = 0;
        while(idx_pred < num_unique_classes)
        {
            if(test_classes_pred[i] == unique_classes[idx_pred])
            {
                break;
            }
            idx_pred++;
        }
        conf_matrix[idx_true][idx_pred]++;
    }
}

void calculate_tp_fp_tn_fn(tp_fp_tn_fn *tp_fp_tn_fn_arr, int *unique_classes, 
    int num_unique_classes, int num_test, int *test_classes, int *test_classes_pred)
{
    int conf_matrix[num_unique_classes][num_unique_classes];
    calculate_confusion_matrix(num_unique_classes, conf_matrix, unique_classes,
        num_test, test_classes, test_classes_pred);

    for (int i = 0; i < num_unique_classes; i++)
    {
        int tp, fp, tn, fn;
        tp = conf_matrix[i][i];
        fp = fn = 0;
        for (int j = 0; j < num_unique_classes; j++)
        {
            if(i != j)
            {
                fp += conf_matrix[j][i];
                fn += conf_matrix[i][j];
            }
        }
        tn = num_test - tp - fp - fn;
        tp_fp_tn_fn_arr[i].tp = tp;
        tp_fp_tn_fn_arr[i].fp = fp;
        tp_fp_tn_fn_arr[i].tn = tn;
        tp_fp_tn_fn_arr[i].fn = fn;
    }
}

int calculate_accuracy_for_class(tp_fp_tn_fn *tp_fp_tn_fn_arr, int class_idx)
{
    int tp = tp_fp_tn_fn_arr[class_idx].tp;
    int fp = tp_fp_tn_fn_arr[class_idx].fp;
    int tn = tp_fp_tn_fn_arr[class_idx].tn;
    int fn = tp_fp_tn_fn_arr[class_idx].fn;
    return 100.0 * (tp + tn) / (tp + fp + tn + fn);
}

void calculate_accuracy(float *acc, tp_fp_tn_fn *tp_fp_tn_fn_arr, int num_unique_classes,
    int num_test)
{
    *acc = 0;
    for (int i = 0; i < num_unique_classes; i++)
    {
        *acc += tp_fp_tn_fn_arr[i].tp;
    }
    *acc /= num_test;
    *acc *= 100;
}

#include <stdio.h>

int calculate_balanced_accuracy_for_class(tp_fp_tn_fn *tp_fp_tn_fn_arr, int class_idx)
{
    int tp = tp_fp_tn_fn_arr[class_idx].tp;
    int fn = tp_fp_tn_fn_arr[class_idx].fn;
    return 100.0 * ((tp == 0 && fn == 0) ? 1: (float)tp / (tp + fn));
}

void calculate_balanced_accuracy(float *balanced_acc, tp_fp_tn_fn *tp_fp_tn_fn_arr, 
    int num_unique_classes)
{
    *balanced_acc = 0;
    for (int i = 0; i < num_unique_classes; i++)
    {
        *balanced_acc += calculate_balanced_accuracy_for_class(tp_fp_tn_fn_arr, i);
    }
    *balanced_acc /= (float)num_unique_classes;
}

void calculate_metrics(int num_test, int num_unique_classes, int *unique_classes, 
    int *test_classes, int *test_classes_pred, metrics *pred_metrics)
{
    tp_fp_tn_fn tp_fp_tn_fn_arr[num_unique_classes];

    calculate_tp_fp_tn_fn(tp_fp_tn_fn_arr, unique_classes, 
        num_unique_classes, num_test, test_classes, test_classes_pred);

    calculate_accuracy(&(pred_metrics->acc), tp_fp_tn_fn_arr, num_unique_classes, num_test);
    calculate_balanced_accuracy(&(pred_metrics->balanced_acc), tp_fp_tn_fn_arr, 
        num_unique_classes);
}
#include <stdio.h>
void calculate_metrics_for_each_class(int num_test, int num_unique_classes, int *unique_classes, 
    int *test_classes, int *test_classes_pred, metrics *pred_metrics)
{
    tp_fp_tn_fn tp_fp_tn_fn_arr[num_unique_classes];

    calculate_tp_fp_tn_fn(tp_fp_tn_fn_arr, unique_classes, 
        num_unique_classes, num_test, test_classes, test_classes_pred);

    for (int i = 0; i < num_unique_classes; i++)
    {
        pred_metrics[i].acc = calculate_accuracy_for_class(tp_fp_tn_fn_arr, i);
        pred_metrics[i].balanced_acc = calculate_balanced_accuracy_for_class(tp_fp_tn_fn_arr, i);
    }
}

void calculate_average_metrics(int num, metrics *pred_metrics, metrics *avg_metrics)
{
    avg_metrics->acc = 0;
    avg_metrics->balanced_acc = 0;
    for(int k = 0; k < num; k++)
    {
        avg_metrics->acc += pred_metrics[k].acc;
        avg_metrics->balanced_acc += pred_metrics[k].balanced_acc;
    }
    avg_metrics->acc /= num;
    avg_metrics->balanced_acc /= num;

    avg_metrics->std_acc = 0;
    avg_metrics->std_balanced_acc = 0;
    for(int k = 0; k < num; k++)
    {
        avg_metrics->std_acc += pow(pred_metrics[k].acc - avg_metrics->acc, 2);
        avg_metrics->std_balanced_acc += pow(pred_metrics[k].balanced_acc - avg_metrics->balanced_acc, 2);
    }
    avg_metrics->std_acc = sqrt((avg_metrics->std_acc) / num);
    avg_metrics->std_balanced_acc = sqrt((avg_metrics->std_balanced_acc) / num);
}