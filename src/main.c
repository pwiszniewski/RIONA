// C program to implement

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <omp.h>

#include <sys/time.h>
#include <time.h> 

#include "files.h"
#include "definitions.h"
#include "metrics.h"
#include "rules.h"
#include "timing.h"
#include "riona.h"
#include "classes.h"
#include "values.h"
#include "values_classes.h"
#include "attributes.h"
#include "cross_validation.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define DATA_FOLDER_PATH "./data"
#define OUT_FOLDER_PATH "./out"

void get_k_max(int num_train, int *k_max)
{
    *k_max = (int)sqrt(num_train);
}

void translate_predictions(int num_inst, int *indices_shuffled, int *classes_pred_shuffled, 
    int *classes_pred)
{
    for (int i = 0; i < num_inst; i++)
    {
        classes_pred[indices_shuffled[i]] = classes_pred_shuffled[i];
    }
}

int get_optimal_k_neigh(int k_max, int num_inst, int num_attr, float values[][num_attr], 
    int *classes, int num_unique_classes, int *unique_classes, char *attr_types, bool is_norm,
    int classes_pred_find_optk[][k_max], int *classes_pred_optk, stats_find_optk *sfoptk, 
    timing_optk *toptk)
{
    printf("2\n");
    float min_values[num_attr];
    float max_values[num_attr];
    get_min_max_attributes(num_attr, num_inst, values, min_values, 
    max_values);
    printf("3\n");
    int classes_cnt[num_unique_classes];
    get_classes_cnt(num_unique_classes, unique_classes, num_inst, classes,
        classes_cnt);
printf("4\n");
    for (int i = 0; i < num_inst; i++)
    {
        sfoptk[i].num_k_plus = malloc(sizeof(*(sfoptk[i].num_k_plus)) * k_max);
        if(sfoptk[i].num_k_plus == NULL) {printf("Blad allokacji");};
        sfoptk[i].num_k_plus_promising = malloc(sizeof(*(sfoptk[i].num_k_plus_promising)) * k_max);
        if(sfoptk[i].num_k_plus_promising == NULL) {printf("Blad allokacji");};
//         sfoptk[i].k_plus_promising_1 = malloc(sizeof(*(sfoptk[i].k_plus_promising_1)) * num_inst);
//         if(sfoptk[i].k_plus_promising_1 == NULL) {printf("Blad allokacji");};
//         sfoptk[i].k_plus_1 = malloc(sizeof(*(sfoptk[i].k_plus_1)) * num_inst);
//         if(sfoptk[i].k_plus_1 == NULL) {printf("Blad allokacji");};
//         sfoptk[i].k_plus_promising_3 = malloc(sizeof(*(sfoptk[i].k_plus_promising_3)) * num_inst);
//         if(sfoptk[i].k_plus_promising_3 == NULL) {printf("Blad allokacji");};
//         sfoptk[i].k_plus_3 = malloc(sizeof(*(sfoptk[i].k_plus_3)) * num_inst);
// if(sfoptk[i].k_plus_3 == NULL) {printf("Blad allokacji");};
    } 
printf("5\n");
    int k_opt;
    // find_optimal_k(k_max, num_inst, attr_types, num_attr, values, classes, 
    //     num_unique_classes, unique_classes, classes_cnt, min_values, max_values, is_norm, 
    //     &k_opt, classes_pred_find_optk, sfoptk, toptk);
printf("44\n");
    for (int i = 0; i < num_inst; i++)
    {
        classes_pred_optk[i] = classes_pred_find_optk[i][k_opt-1];
    } 
    
    return k_opt;
}

void deallocate_sfoptk(int num_inst, stats_find_optk *sfoptk)
{
    for (int i = 0; i < num_inst; i++)
    {
        free(sfoptk[i].num_k_plus);
        free(sfoptk[i].num_k_plus_promising);
        free(sfoptk[i].k_plus_promising_1);
        free(sfoptk[i].k_plus_1);
        free(sfoptk[i].k_plus_promising_3);
        free(sfoptk[i].k_plus_3);
    } 
}

bool get_params(int argc, char *argv[], file_info *fi, file_info *fi_default)
{
    if(argc == 1)
    {
        memcpy(fi, fi_default, sizeof(*fi));
    }
    else if (argc == 2)
    {
        strncpy(fi->file_name, argv[1], MAX_PATH_LEN - 1);
        strncpy(fi->sep, " ", MAX_SEP_LEN);
    }
    else if (argc == 3)
    {
        strncpy(fi->file_name, argv[1], MAX_PATH_LEN - 1);
        strncpy(fi->sep, argv[2], MAX_SEP_LEN - 1);
    }
    else
    {
        printf("Wrong number of argumetns\n");
        return false;
    }
    return true;
}

int main(int argc, char *argv[])
{
    #ifdef DEBUG
printf("DEBUG\n");
    #endif
    //------------------------ READ FILES ----------------------

    file_info fi_default = {"simple", " "};
    // file_info fi_default = {"heart", " "};
    // file_info fi_default = {"spambase", ","};
    // file_info fi_default = {"lymphography", ","};
    file_info fi;
    get_params(argc, argv, &fi, &fi_default); 

    double time_total_s = get_time_sec();
    double time_read_in_file_s = get_time_sec();

    char file_path[MAX_PATH_LEN];
    get_file_path(DATA_FOLDER_PATH, fi.file_name, file_path);
    printf("File path %s\n", file_path);

    int num_inst, num_attr;
    get_num_inst_and_attributes(file_path, fi.sep, &num_inst, &num_attr);
    printf("Number of attributes %d\n", num_attr);
    printf("Number of instances %d\n", num_inst);

    char info_path[MAX_PATH_LEN];
    get_info_file_path(DATA_FOLDER_PATH, fi.file_name, info_path);
    char attr_types[num_attr];
    char attr_names[num_attr][MAX_NAME_LEN];
    char target_name[MAX_NAME_LEN];
    if(!load_info(info_path, num_attr, attr_types, attr_names, target_name,
        &(fi.class_idx)))
    {
        return -1;
    }
    printf("Decision class index: %d\n", fi.class_idx);

    // int classes[num_inst];
    int *classes = malloc( sizeof(*classes) * num_inst);
    float (*values) [num_attr] = malloc( sizeof(float[num_inst][num_attr]) );
    if(!load_file(&fi, file_path, classes, num_attr, values))
    {
        return -1;
    }

    double time_read_in_file = get_time_sec() - time_read_in_file_s;
    
    int unique_classes[MAX_CLASSES];
    int num_unique_classes;
    get_unique_classes(num_inst, classes, unique_classes, &num_unique_classes);

    int k_max;
    get_k_max(num_inst, &k_max);

    for(int is_norm = 0; is_norm < 2; is_norm++) // normalization
    {
        //------------------------ FIND OPTIMAL K NEIGHBOURS ----------------------
        printf("FINDING OPTIMAL K NEIGHBOURS - ");
        printf("%s\n", is_norm ? "NORMALIZED" : "STANDARD");
        double time_total_optk_s = get_time_sec();
        printf("0\n");

        int (*classes_pred_find_optk) [k_max] = malloc( sizeof(float[num_inst][k_max]) );
        printf("01\n");

        int classes_pred_optk[num_inst];
        printf("001\n");

        timing_optk toptk;
        stats_find_optk *sfoptk = malloc(sizeof(*sfoptk) * num_inst);
        printf("1\n");
        int k_opt = get_optimal_k_neigh(k_max, num_inst, num_attr, values, classes, num_unique_classes, 
            unique_classes, attr_types, is_norm, classes_pred_find_optk, classes_pred_optk, sfoptk,
            &toptk);

        double time_total_optk = get_time_sec() - time_total_optk_s;

        printf("\tk_max: %d, k_opt: %d\n", k_max, k_opt);

        metrics pred_metrics_optk[num_unique_classes];
        calculate_metrics_for_each_class(num_inst, num_unique_classes, unique_classes, 
            classes, classes_pred_optk, pred_metrics_optk);

        metrics avg_metrics;
        calculate_average_metrics(num_unique_classes, pred_metrics_optk, &avg_metrics);

        int num_numerical_attr, num_nominal_attr;
        get_num_numerical_and_nominal_attr(attr_types, num_attr, &num_numerical_attr,
            &num_nominal_attr);
printf("55\n");
        //--------------------------- SAVE FIND OPTIMAL K NEIGHBOURS RESULTS -------------------------

        char out_folder_path[MAX_PATH_LEN];
        char out_path_optk[MAX_PATH_LEN];
        get_out_folder_path(OUT_FOLDER_PATH, fi.file_name, out_folder_path);
        get_out_file_path_optk(out_folder_path, fi.file_name, num_attr, num_inst, k_max, is_norm, 
            out_path_optk);

        create_out_folder_if_not_exist(OUT_FOLDER_PATH);
        create_out_folder_if_not_exist(out_folder_path);

        double time_save_out_file_optk_s = get_time_sec();
        printf("66\n");
        // save_out_file_optk(out_path_optk, num_inst, num_attr, values, attr_types, classes, k_max, 
        //     classes_pred_find_optk, sfoptk);
        double time_save_out_file_optk = get_time_sec() - time_save_out_file_optk_s;
        printf("77\n");
        // deallocate_sfoptk(num_inst, sfoptk);
        // free(classes_pred_find_optk);
        printf("88\n");
        char stat_path_optk[MAX_PATH_LEN];
        // get_stat_file_path_optk(out_folder_path, fi.file_name, num_attr, num_inst, k_max, is_norm, 
        //     stat_path_optk);

        stats_optk stoptk = { .file_name = fi.file_name,
                            .num_numerical_attr = num_numerical_attr,
                            .num_nominal_attr = num_nominal_attr,
                            .num_attr = num_attr,
                            .num_inst = num_inst,
                            .num_classes = num_unique_classes,
                            .unique_classes = unique_classes,
                            .is_norm = is_norm,
                            .k_max = k_max,
                            .pred_metrics = pred_metrics_optk,
                            .avg_metrics = &avg_metrics,
                            .time_total = time_total_optk,
                            .time_read_in_file = time_read_in_file,
                            .time_save_out_file = time_save_out_file_optk,
                            .toptk = toptk
                        };

        // save_stat_file_optk(stat_path_optk, &stoptk);

        // ------------------------------ GRIONA - PREDICT ----------------------------------
        printf("PREDICTION - ");
        printf("%s\n", is_norm ? "NORMALIZED" : "STANDARD");
        (void)k_opt;
        // int k_neigh[] = {1, 3, k_opt, k_max};
        int k_neigh[] = {1, 3, k_max};
//  printf("aa\n");
        for(int k = 0; k < ARRAY_SIZE(k_neigh); k++) // k neighbours
        {
            //  printf("bb\n");
            printf("\tk+ neighbours:%3d - ", k_neigh[k]);
            metrics pred_metrics[NUM_CV][NUM_K_FOLD];
            metrics avg_metrics[NUM_CV];
            metrics final_metrics;
            //  printf("cc\n");

            // int classes_pred[NUM_CV][num_inst];
            // stats_predict sp[NUM_CV][num_inst];

            int (*classes_pred) [num_inst] = malloc( sizeof(int[NUM_CV][num_inst]) );
            stats_predict (*sp) [num_inst] = malloc( sizeof(stats_predict[NUM_CV][num_inst]) );

            timing_predict tp[NUM_CV];
            //  printf("dd\n");

            // #pragma omp parallel for 
            for(int cv = 0; cv < NUM_CV; cv++)  // cross validation
            {
                int max_k_fold_inst = (num_inst / NUM_K_FOLD) + 1;
                int k_fold_indices[NUM_K_FOLD][max_k_fold_inst];
                int k_fold_cnt[NUM_K_FOLD];
                create_k_fold(num_inst, max_k_fold_inst, NUM_K_FOLD, k_fold_indices, k_fold_cnt);
            //  printf("ee\n");

                for(int k_fold = 0; k_fold < NUM_K_FOLD; k_fold++) // k-fold
                {
                    int num_train, num_test;
                    get_num_train_test(k_fold, num_inst, k_fold_cnt, &num_train, &num_test);

                    float (*train_values) [num_attr] = malloc( sizeof(float[num_train][num_attr]) );
                    int train_classes[num_train];
                    float (*test_values) [num_attr] = malloc( sizeof(float[num_test][num_attr]) );
                    int test_classes[num_test];
                    split_train_test(num_inst, num_attr, values, classes, NUM_K_FOLD, k_fold, 
                        k_fold_indices, k_fold_cnt, train_values, train_classes, test_values, 
                        test_classes);
                    //    printf("ff\n");
                    int test_classes_pred[num_test];
                    stats_predict test_sp[num_test];
                    //    printf("gg\n");
                    predict(num_train, num_test, num_attr, k_neigh[k], train_values, train_classes,
                        test_values, attr_types, num_unique_classes, unique_classes,  
                        test_classes_pred, is_norm, test_sp, &tp[cv]);
                    free(train_values);
                    free(test_values);
// printf("end_pred\n");
                    int *test_indices = k_fold_indices[k_fold];
                    prepare_k_fold_results(num_inst, num_test, test_indices, test_sp, sp[cv], 
                        test_classes_pred, classes_pred[cv]);

                    int unique_classes_test[MAX_CLASSES];
                    int num_unique_classes_test;
                    get_unique_classes(num_test, test_classes, unique_classes_test, 
                        &num_unique_classes_test);

                    calculate_metrics(num_test, num_unique_classes_test, unique_classes_test, 
                        test_classes, test_classes_pred, &pred_metrics[cv][k_fold]);
                }   // k-fold
                calculate_average_metrics(NUM_K_FOLD, pred_metrics[cv], &avg_metrics[cv]);
            }   // cross validation

            calculate_average_metrics(NUM_CV, avg_metrics, &final_metrics);
            
            timing_predict tp_avg;
            calculate_average_times(NUM_CV, tp, &tp_avg);

            printf("accuracy:%.2f +- %.2f balanced accuracy:%.2f +- %.2f\n", final_metrics.acc, final_metrics.std_acc, 
                final_metrics.balanced_acc, final_metrics.std_balanced_acc);

            //--------------------------------- SAVE GRIONA RESULTS --------------------------------
            char out_path[MAX_PATH_LEN];
            get_out_file_path_griona(out_folder_path, fi.file_name, num_attr, num_inst, k_neigh[k],
                is_norm, out_path);
// printf("qq\n");
            create_out_folder_if_not_exist(out_folder_path);

            double time_save_out_file_s = get_time_sec();
            save_out_file_griona(out_path, num_inst, num_attr, values, attr_types, classes, 
                NUM_K_FOLD, sp, classes_pred);
            double time_save_out_file = get_time_sec() - time_save_out_file_s;
// printf("ww\n");
            free(classes_pred);
            free(sp);

            char stat_path[MAX_PATH_LEN];
            get_stat_file_path_griona(out_folder_path, fi.file_name, num_attr, num_inst, k_neigh[k],
                is_norm, stat_path);
// printf("ee\n");
            stats_griona stgr = { .file_name = fi.file_name,
                                    .num_numerical_attr = num_numerical_attr,
                                    .num_nominal_attr = num_nominal_attr,
                                    .num_attr = num_attr,
                                    .num_inst = num_inst,
                                    .num_classes = num_unique_classes,
                                    .num_cv = NUM_CV,
                                    .is_norm = is_norm,
                                    .k_fold = NUM_K_FOLD,
                                    .pred_metrics = pred_metrics,
                                    .avg_metrics = avg_metrics,
                                    .final_metrics = final_metrics,
                                    .time_read_in_file = time_read_in_file,
                                    .time_save_out_file = time_save_out_file,
                                    .tp_avg = tp_avg
                                };
            save_stat_file_griona(stat_path, &stgr);
        }   // k neighbours
    }   // normalization

    free(values);

    double time_total = get_time_sec() - time_total_s;
    printf("Total time: %f sec", time_total);

    return 0;
}